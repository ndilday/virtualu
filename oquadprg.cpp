// filename    : OQUADPRG.CPP
// Description : specialized quadratic programming for finance

#include <ALL.H>
#include <math.h>
#include <OQUADPRG.H>

// ------ begin of function QuadProgramming::QuadProgramming ------ //
//
QuadProgramming::QuadProgramming() {
    memset( this, 0, sizeof(*this) );
}

// ------ end of function QuadProgramming::QuadProgramming ------ //

// ------ begin of function QuadProgramming::~QuadProgramming ------ //
//
QuadProgramming::~QuadProgramming() {
    vars_count = 0;
    constraint_count = 0;

    if( test_value ) {
	mem_del(test_value);
	test_value = NULL;
    }

    if( value_tol ) {
	mem_del(value_tol);
	value_tol = NULL;
    }

    if( nX1_vector ) {
	mem_del(nX1_vector);
	nX1_vector = NULL;
    }

    if( nXn_matrix ) {
	mem_del(nXn_matrix);
	nXn_matrix = NULL;
    }

    if( nom_matrix ) {
	mem_del(nom_matrix);
	nom_matrix = NULL;
    }

    if( constraint_sum ) {
	mem_del(constraint_sum);
	constraint_sum = NULL;
    }
    if( selected_i ) {
	mem_del(selected_i);
	selected_i = NULL;
    }
    if( lambda_c ) {
	mem_del(lambda_c);
	lambda_c = NULL;
    }
    if( mX1_vector ) {
	mem_del(mX1_vector);
	mX1_vector = NULL;
    }
    if( mXm_matrix ) {
	mem_del(mXm_matrix);
	mXm_matrix = NULL;
    }

    if( constraint_coefficient ) {
	mem_del(constraint_coefficient);
	constraint_coefficient = NULL;
    }

    if( lambda_p ) {
	mem_del(lambda_p);
	lambda_p = NULL;
    }

    if( determinant_swap_tmp ) {
	mem_del(determinant_swap_tmp);
	determinant_swap_tmp = NULL;
    }

    determinant_row_alloc = 0;
    constraint_alloc = 0;
    vars_alloc = 0;
}

// ------ end of function QuadProgramming::~QuadProgramming ------ //

// ------ begin of function QuadProgramming::sp_quadratic_programming ------ //
//
// input : lower and upper limit of each variable, and error tolerance of each (lowLimit, highLimit, tolLimit)
// equality constraint (limitCoeff, limitSum)
// objective function to minimize (cost2, cost1)
//
// if return true, result variable in resultValue
//
int QuadProgramming::sp_quadratic_programming( int vars,
					       double *resultValue,                              // output : result
					       // lower and upper limit of variables, and error tolerance
					       double *lowLimit, double *highLimit, double *tolLimit,
					       double *limitCoeff, double limitSum,              // constraint : limitCoeff[i] * variable[i] = limitSum
					       double *cost1,                                    // to minimize cost1[i] * variable[i] + cost[i] * variable[i]^2
					       double *cost2 ) {
    int hasResult = 0;

    int maxConstraints = vars + 1; {                // 1 is limitCoeff&limitSum constraint

	// check the lowLimit and highLimit is contradictory to limitCoeff[i] * variable[i] = limitSum
	// return before new to avoid leak
	double lowRange = 0.0;
	double highRange = 0.0;
	for( int i = 0; i < vars; ++i ) {
	    if( limitCoeff[i] >= 0 ) {
		highRange += limitCoeff[i] * highLimit[i];
		lowRange += limitCoeff[i] * lowLimit[i];
	    }
	    else {
		highRange += limitCoeff[i] * lowLimit[i];
		lowRange += limitCoeff[i] * highLimit[i];
	    }
	}
	if( limitSum > highRange || limitSum < lowRange ) {
	    return 0;
	}
    }

    set_var_count( vars );
    set_constraint_count( maxConstraints );

    // use double *test_value = new double[vars];
    // use double *value_tol = new double[vars];
    // use double *constraint_coefficient = new double[maxConstraints*vars]; // [constraints][vars]
    // use double *constraintSum = new double[maxConstraints];

    int constraintCount = 0;

    int i;

    err_when( vars < 2 || vars > 18 );              // max limit is 19 variables, combinationCount will overflow

    // determinte tolerance of limitSum
    double limitTol = 0.0f;
    for( i = 0; i < vars; ++i ) {
	value_tol[i] = tolLimit[i];
	limitTol += value_tol[i] * fabs(limitCoeff[i]);
    }
    limitTol /= vars;

    // initialize constraint set, add limitCoeff&limitSum constraint

    for( i = 0; i < vars; ++i )
	constraint_coefficient[constraintCount * vars + i] = limitCoeff[i];
    constraint_sum[constraintCount] = limitSum;
    ++constraintCount;
    int equalityConstraintCount = constraintCount;

    // try optimize, use lagrange multiplier to solve

    while(1) {
	int solveFlag = 0;
	if( constraintCount < vars )
	    solveFlag = lcqf_lagrange_multiplier(vars, constraintCount, test_value, constraint_coefficient, constraint_sum, cost1, cost2);
	else if( constraintCount >= vars )
	    solveFlag = solve_linear_system( vars, test_value, constraint_coefficient, constraint_sum );

	if( solveFlag ) {
	    int oldConstraintCount = constraintCount;
	    // reset constraint count, so as to remove inequality constraint
	    constraintCount = equalityConstraintCount;

	    // test break lower/upper limit constraint

	    int passAllFlag = 1;
	    for( i = 0; i < vars; ++i ) {
		if( test_value[i] < lowLimit[i] - value_tol[i] ) {
		    for( int k = 0; k < vars; ++k )
			constraint_coefficient[constraintCount * vars + k] = 0.0;
		    constraint_coefficient[constraintCount * vars + i] = 1.0;
		    constraint_sum[constraintCount] = lowLimit[i];
		    ++constraintCount;
		    passAllFlag = 0;
		}
		else if( test_value[i] > highLimit[i] + value_tol[i] ) {
		    for( int k = 0; k < vars; ++k )
			constraint_coefficient[constraintCount * vars + k] = 0.0;
		    constraint_coefficient[constraintCount * vars + i] = -1.0;
		    constraint_sum[constraintCount] = -highLimit[i];
		    ++constraintCount;
		    passAllFlag = 0;
		}
	    }

	    if( passAllFlag || constraintCount <= oldConstraintCount ) {
		// if loop back with same set of constraints, will become infinite loop, so also return a result
		for( i = 0; i < vars; ++i )
		    resultValue[i] = test_value[i];
		hasResult = 1;
		break;
	    }
	}
	else {
	    hasResult = 0;
	    break;
	}
    }

    return hasResult;
}

// ------ end of function QuadProgramming::sp_quadratic_programming ------ //

// ------ begin of function QuadProgramming::lcqf_lagrange_multiplier ------ //
//
// solve lagrange multiplier
//
// input : constraints, in constraintCoefficient and constraintSum
// and objective function to minimize constraintSum
//
// see description in http://www.chass.utoronto.ca/~osborne/MathTutorial/MEMF.HTM
//
int QuadProgramming::lcqf_lagrange_multiplier( int vars, int constraints,
					       double resultValue[],                             // [vars], output : result
					       double constraintCoefficient[],                   // [constraints][vars] coefficient of linear constraint
					       double constraintSum[],                           // [constraints]
					       double cost1[],                                   // to minimize cost1[i] * variable[i] + cost[i] * variable[i]^2
					       double cost2[] ) {
    err_when( constraints >= vars );                // otherwise no need use optimize, just solve the constraints directly

  // resultValue will be p
    int n = vars;
    int m = constraints;
    double *Q = cost2;
    double *c = cost1;
    double *A = constraintCoefficient;
    double *b = constraintSum;

  // L(p[0],p[1],...p[N-1]) = Sum(i=0,n-1, Q[i]*p[i]^2 + c[i]*p[i]) - Sum(j=0,m-1,Lambda[j] * (Sum(i=0,n-1, A[j,i]*p[i]) - b[j]) )

  // 1st order condition (partial differetial)
  // for i = 0 to N-1, Sum(j=0,m-1, Lambda[j]*A[j,i]) = 2*Q[i]*p[i] + c[i]
  // ie : Lambda[0] * A[0,i] + lambda[1] * A[1,i] + lambda[2] * A[2,i] + ... + lambda[m-1] * A[2,m-1] = 2 * Q[i] * p[i] + c[i]

  // Lambda[j] is linear on each p[k] :
  // let Lambda[j] = Sum(k=0,n-1, LambdaP[j,k]*p[k]) + LambdaC[j]
  // ie : Lambda[j] = LambdaP[j,0] * p[0] + LambdaP[j,1] * p[1] + ... + LambdaP[j,n-1] * p[n-1] + LambdaC[j]
  //

  // equate coefficient of p[0] for j = 0 to m-1 :
  // for i = 0 to N-1, Sum(j=0,m-1, A[j,i]*LambdaC[j]) = c[i]
  // for i = 0 to N-1, for k = 0 to N-1, Sum(j=0,m-1, A[j,i]*LambdaP[j,k]) = 2*Qi (if k == i)
  //                                                                       = 0 (if k != i )

  // solving LambdaC, and LambdaP, but now find which m out of n 1st order conditions to solve

    set_var_count( n );
    set_constraint_count( m );

  // use int *selectedI = new int[m];
  // use double *lambdaC = new double[m]; // [M]
  // use double *lambdaP = new double[n*m]; // [N][M] // beware, transposed
  // use double *mXmMatrix = new double[m*m];
  // use double *mX1Vector = new double[m];
  // use double *nXnMatrix = new double[n*n];
  // use double *nX1Vector = new double[n];

    int i, j, k;
    int selectI;

  // select find m values from 0 to n (store in i) such that
  // the row mXmMatrix[i*m + j] = A[j*n+i] is not in ratio with previous rows added to mxmMatrix

    int rc = 0;

    // generate combination (nCr)

    for( selectI = 0; selectI < m; ++selectI )      // set initial value
	selected_i[selectI] = selectI;

    while(1) {
	// set mXmMatrix;
	for( selectI = 0; selectI < m; ++selectI ) {
	    i = selected_i[selectI];
	    for( j = 0; j < m; ++j )
		mXm_matrix[selectI*m + j] = A[j*n + i];
	}

	if( fabs(matrix_determinant( m, m, mXm_matrix) ) != 0.0 ) {
	    rc = 1;
	    break;                                      // a matrix is found
	}

	// matrix fail, select next combination
	for( j=m-1; j >= 0; --j ) {
	    if( ++selected_i[j] < n+j-m+1 )
		break;
	}
	if( j < 0 )
	    break;                                      // finish, no matrix is found

	// reset value after j
	for( ++j; j < m; ++j )
	    selected_i[j] = selected_i[j-1] + 1;
    }

    if( rc ) {
	rc = 0;

	// solving LambdaC

	for( selectI = 0; selectI < m; ++selectI ) {
	    i = selected_i[selectI];
	    mX1_vector[selectI] = c[i];
	    for( j = 0; j < m; ++j )
		mXm_matrix[selectI*m + j] = A[j*n + i];
	}

	if( solve_linear_system( m, lambda_c, mXm_matrix, mX1_vector ) ) {
	    // solve lambdaP
	    for(k = 0; k < n; ++k ) {
		// for( i = 0; i < m; ++i ) // use up to m constraint enough to solve
		for( selectI = 0; selectI < m; ++selectI ) {
		    i = selected_i[selectI];
		    mX1_vector[selectI] = (i == k) ? 2 * Q[i] : 0;

		    // can keep using mXmMatrix from the above
		    //for( j = 0; j < m; ++j )
		    //{
		    //	mXmMatrix[selectI*m + j] = A[j*n + i];
		    //}
		}

		// since use same mXmMatrix, solvable again
		solve_linear_system( m, lambda_p + k * m, mXm_matrix, mX1_vector );
	    }

	    // finally solve
	    // for(j=0 to m-1)  Sum(i=0,n-1, A[j,i] * p[i]) = b[j]          (constraints)
	    // and remaining 1st order condition : for i not in selected_i, Sum(j=0,m-1, Lambda[j]*A[j,i] ) = 2 * Q[i] * p[i] + c[i]      (remaining 1st order condition)

	    for( j = 0; j < m; ++j ) {
		for( i = 0; i < n; ++i ) {
		    nXn_matrix[j * n + i] = A[j * n + i ];
		}
		nX1_vector[j] = b[j];
	    }

	    // now add n-m rows of 1st order constraint into nXn_matrix, representing coefficient of linear equations

	    // for i not in selected_i,
	    // Sum(j=0,m-1, Lambda[j]*A[j,i] ) = 2 * Q[i] * p[i] + c[i]
	    // = Sum(j=0,m-1, Sum(k=0,n-1, LambdaP[j,k] * p[k]) * A[j,i] + LambdaC[j] * A[j,i] ) -  2*Q[i]*p[i] = c[i]
	    // = Sum(j=0,m-1, Sum(k=0,n-1, LambdaP[j,k] * A[j,i] * p[k])) - 2*Q[i]*p[i] = c[i] - Sum(j=0,m-1, LambdaC[j] * A[j,i])
	    // = Sum(k=0,n-1, Sum(j=0,m-1, LambdaP[j,k] * A[j,i]) * p[k]) - 2*Q[i]*p[i] = c[i] - Sum(j=0,m-1, LambdaC[j] * A[j,i])

	    int eqCount = m;                            // m row has been added
	    selectI = 0;
	    for( i = 0; i < n; ++i ) {                  // just do i row and copy to to n-th row
		// see i is in selected_i, if inside, skip (assume selected_i is ascending)
		if( selectI < m && i == selected_i[selectI] ) {
		    ++selectI;
		    continue;
		}

		for(k = 0; k < n; ++k ) {
		    double nXnMatrixIK = 0.0;
		    for( j = 0; j < m; ++j ) {
			nXnMatrixIK += lambda_p[k * m + j] * A[j * n + i];
		    }
		    nXn_matrix[eqCount * n + k] = nXnMatrixIK;
		}
		nXn_matrix[eqCount * n + i] -= 2 * Q[i];

		double nX1VectorI = c[i];
		for( j = 0; j < m; ++j ) {
		    nX1VectorI -= lambda_c[j] * A[j * n + i];
		}
		nX1_vector[eqCount] = nX1VectorI;

		++eqCount;
	    }

	    if( solve_linear_system( n, resultValue, nXn_matrix, nX1_vector ) ) {
		rc = 1;
	    }
	}
    }

    return rc;
}

// ------ end of function QuadProgramming::lcqf_lagrange_multiplier ------ //

// ------ begin of function QuadProgramming::solve_linear_system ------ //
//
int QuadProgramming::solve_linear_system( int vars,
					  double resultValue[],                             // [vars]
					  double constraintCoefficient[],                   // [vars][vars] coefficient of linear constraint
					  double constraintSum[] ) {                        // [vars] such that Summation(i, constraintCoefficient[j][i]) = constraintSum[j]
    int i, j;

    set_var_count( vars );                          // change var_count
    // use nom_matrix

    double denom = matrix_determinant( vars, vars, constraintCoefficient );

    if( fabs(denom) == 0.0 )
	return 0;

    for( i = 0; i < vars; ++i ) {
	// copy row of matrix
	memcpy( nom_matrix + i * vars, constraintCoefficient + i * vars, vars * sizeof(nom_matrix[0]) );
    }

    for( i = 0; i < vars; ++i ) {
	// copy one column of constraintSum to nomMatrix
	for( j = 0; j < vars; ++j ) {
	    nom_matrix[j*vars + i] = constraintSum[j];
	}

	double nominator = matrix_determinant( vars, vars, nom_matrix );
	resultValue[i] = nominator / denom;

	// restore one column of constraintSum from constraintCoefficient
	for( j = 0; j < vars; ++j ) {
	    nom_matrix[j*vars + i] = constraintCoefficient[j*vars + i];
	}
    }

    return 1;
}

// ------ end of function QuadProgramming::solve_linear_system ------ //

// ------ begin of function QuadProgramming::matrix_determinant ------ //
//
// matrix[nSqSize][nSqSize] coefficient of linear constraint
// coverted access element in matrix using to rowArray[row] * rowPitch + colArray[col]
//
// rowArray and colArray must be in separate memory location
// they can be NULL, the function generate for vector for itself
//
double QuadProgramming::matrix_determinant( int nSqSize, int rowPitch, double *matrix ) {
    int i;
    double detSum = 0.0f;

#define MATRIX_E(r,c) matrix[(r) * rowPitch + (c)]

    if( nSqSize <= 3 ) {
	if( nSqSize == 1 ) {
	    return MATRIX_E(0, 0);
	}
	else if( nSqSize == 2 ) {
	    return MATRIX_E(0, 0) * MATRIX_E(1, 1)
		- MATRIX_E(1, 0) * MATRIX_E(0, 1);
	}
	else if( nSqSize == 3 ) {
	    return MATRIX_E(0,0) * MATRIX_E(1,1) * MATRIX_E(2,2)
		+ MATRIX_E(1,0) * MATRIX_E(2,1) * MATRIX_E(0,2)
		+ MATRIX_E(2,0) * MATRIX_E(0,1) * MATRIX_E(1,2)
		- MATRIX_E(0,0) * MATRIX_E(2,1) * MATRIX_E(1,2)
		- MATRIX_E(1,0) * MATRIX_E(0,1) * MATRIX_E(2,2)
		- MATRIX_E(2,0) * MATRIX_E(1,1) * MATRIX_E(0,2);
	}

	err_here();
	return 0.0;
    }
    else {
	/*
	  double pSum = 0.0;
	  int addSign = 1;

	  for( i = 0; i < nSqSize; ++i, (addSign = !addSign) )
	  {
	  double row0I = MATRIX_E(0, i); // make a backup before change colArray,

	  // remove colArray[i]
	  int backupCol = colArray[i];
	  memmove( colArray+i, colArray+i+1, (nSqSize-i-1)*sizeof(colArray[0]) );

	  // use sum of each element in row 0 and its cofactor
	  if( addSign )
	  pSum += row0I * matrix_determinant( nSqSize-1, rowPitch, matrix, rowArray+1, colArray );
	  else
	  pSum -= row0I * matrix_determinant( nSqSize-1, rowPitch, matrix, rowArray+1, colArray );

	  // restore colArray[i], re-insert backupCol
	  memmove( colArray+i+1, colArray+i, (nSqSize-i-1)*sizeof(colArray[0]) );
	  colArray[i] = backupCol;
	  }

	  return pSum;
	*/

	set_determinant_size( nSqSize );

	// use each value on right most column and its co-factor
	double pSum = 0.0;
	int mulSign = 1;                              // add botton right corner * its co-factor

	for( i = nSqSize-1; i >= 0; --i, (mulSign = -mulSign) ) {
	    // swap rows so next co-factor is moved to the upper left of the matrix
	    // eg : original:
	    // a11 a12 a13 a14
	    // a21 a22 a23 a24
	    // a31 a32 a33 a34
	    // a41 a42 a43 a44	// we calculate a44 * A44 (=cofactor =  determinant of a11..a33)
	    //
	    // swap row 3 and 4
	    // a11 a12 a13 a14
	    // a21 a22 a23 a24
	    // a41 a42 a43 a44
	    // a31 a32 a33 a34	// we calculate a34 * A34
	    //
	    // swap row 2 and 4
	    // a11 a12 a13 a14
	    // a31 a32 a33 a34
	    // a41 a42 a43 a44
	    // a21 a22 a23 a24	// we calculate a24 * A24
	    //
	    // swap row 1 and 4
	    // a21 a22 a23 a24
	    // a31 a32 a33 a34
	    // a41 a42 a43 a44
	    // a11 a12 a13 a14	// we calculate a14 * A14
	    //
	    // cycle row 4 back to row 1 to restore to original

	    double entryValue = MATRIX_E(nSqSize-1, nSqSize-1);
	    if( entryValue != 0.0 ) {                   // skip if zero, safe time in recursion
		if( mulSign > 0 )                         // +ve
		    pSum += entryValue * matrix_determinant( nSqSize-1, rowPitch, matrix );
		else
		    pSum -= entryValue * matrix_determinant( nSqSize-1, rowPitch, matrix );
	    }

	    if( i > 0 ) {
		// swap i-1 th row nSqSize-1 row
		// so next loop will calc MATRIX_E(i-1,nSqSize-1) * cofactor
		memcpy( determinant_swap_tmp, &MATRIX_E(i-1,0), nSqSize * sizeof(matrix[0]) );
		memcpy( &MATRIX_E(i-1,0), &MATRIX_E(nSqSize-1,0), nSqSize * sizeof(matrix[0]) );
		memcpy( &MATRIX_E(nSqSize-1,0), determinant_swap_tmp, nSqSize * sizeof(matrix[0]) );
	    }
	    else {
		// cycle last row to 1st row
		memcpy( determinant_swap_tmp, &MATRIX_E(nSqSize-1,0), nSqSize * sizeof(matrix[0]) );
		for( int j = nSqSize-1; j > 0; --j )
		    memcpy( &MATRIX_E(j,0), &MATRIX_E(j-1,0), nSqSize * sizeof(matrix[0]) );
		memcpy( &MATRIX_E(0,0), determinant_swap_tmp, nSqSize * sizeof(matrix[0]) );
	    }
	}

	return pSum;
    }
}

// ------ end of function QuadProgramming::matrix_determinant ------ //

// ------ begin of function QuadProgramming::set_var_count ------ //
//
// allocate temp array space
//
void QuadProgramming::set_var_count( int newVarCount ) {
    if( vars_alloc < newVarCount ) {
	vars_alloc = newVarCount;

	// [vars]
	test_value = (double *)mem_resize(test_value, vars_alloc * sizeof(test_value[0]) );
	value_tol = (double *)mem_resize(value_tol, vars_alloc * sizeof(value_tol[0]) );
	nX1_vector = (double *)mem_resize(nX1_vector, vars_alloc * sizeof(nX1_vector[0]) );

	// [vars * vars];
	nXn_matrix = (double *)mem_resize(nXn_matrix, vars_alloc * vars_alloc * sizeof(nXn_matrix[0]) );
	nom_matrix = (double *)mem_resize(nom_matrix, vars_alloc * vars_alloc * sizeof(nom_matrix[0]) );

	// [constraints][vars]
	constraint_coefficient = (double *)mem_resize(constraint_coefficient, constraint_alloc * vars_alloc * sizeof(constraint_coefficient[0]) );

	// vars_count * constraint_count
	lambda_p = (double *)mem_resize(lambda_p, vars_alloc * constraint_alloc * sizeof(lambda_p[0]) );
    }
}

// ------ end of function QuadProgramming::set_var_count ------ //

// ------ begin of function QuadProgramming::set_constraint_count ------ //
//
void QuadProgramming::set_constraint_count( int newConstraintCount ) {
    if( constraint_alloc < newConstraintCount ) {
	constraint_alloc = newConstraintCount;

	constraint_sum = (double *)mem_resize(constraint_sum, constraint_alloc * sizeof(constraint_sum[0]) );
	selected_i = (int *)mem_resize(selected_i, constraint_alloc * sizeof(selected_i[0]) );
	lambda_c = (double *)mem_resize(lambda_c, constraint_alloc * sizeof(lambda_c[0]) );
	mX1_vector = (double *)mem_resize(mX1_vector, constraint_alloc * sizeof(mX1_vector[0]) );

	// constraint_alloc * constraint_alloc
	mXm_matrix = (double *)mem_resize(mXm_matrix, constraint_alloc * constraint_alloc * sizeof(mXm_matrix[0]) );

	// [constraints][vars]
	constraint_coefficient = (double *)mem_resize(constraint_coefficient, constraint_alloc * vars_alloc * sizeof(constraint_coefficient[0]) );

	// vars_count * constraint_count
	lambda_p = (double *)mem_resize(lambda_p, vars_alloc * constraint_alloc * sizeof(lambda_p[0]) );
    }
}

// ------ end of function QuadProgramming::set_constraint_count ------ //

// ------ begin of function QuadProgramming::set_determinant_size ------ //
//
void QuadProgramming::set_determinant_size( int newMatrixSize ) {
    if( determinant_row_alloc < newMatrixSize ) {
	determinant_row_alloc = newMatrixSize;

	determinant_swap_tmp = (double *)mem_resize( determinant_swap_tmp, determinant_row_alloc * sizeof(determinant_swap_tmp[0]) );
    }
}

// ------ end of function QuadProgramming::set_determinant_size ------ //

// testing code for lcqf_lagrange_multiplier
#if(0)

const VARS = 3;
const CONSTRAINTS = 2;
double resultValue[VARS];
double constraintCoefficient[CONSTRAINTS * VARS] = {
    1.0,  2.0,  1.0,
    2.0, -1.0, -3.0,
};

double constraintSum[CONSTRAINTS] = { 1.0, 4.0 };
double cost1[VARS] = { 0.0, 0.0, 0.0 };
double cost2[VARS] = { 1.0, 1.0, 1.0 };

int lagRc = lcqf_lagrange_multiplier( VARS, CONSTRAINTS,
				      resultValue, constraintCoefficient, constraintSum, cost1, cost2 );
#endif

#if(0)
// testing code for sp_quadratic_programming

const VARS = 3;
double resultValue[VARS];
double lowLimit[VARS] = { -1.0, -1.0, -1.0 };
double highLimit[VARS] = { 5.0, 4.0, 5.0 };
double limitCoeff[VARS] = {                       // x+y+z = 6
    1.0, 1.0, 1.0
};
double limitSum = 7.0;
double cost1[VARS] = {                            // (x-2)^2 + (y-2)^2 + (z-2)^2
    -4.0, -4.0, -4.0
};
double cost2[VARS] = { 1.0, 1.0, 1.0 };

int quadRc = sp_quadratic_programming( VARS, resultValue,
				       lowLimit, highLimit, limitCoeff, limitSum, cost1, cost2 );
return 0;
#endif

#if(0) 
if(0) {
// test solve_linear_system

    const MAX_VARS = 10;
    int varCount = 6+m.random(MAX_VARS-6+1);
    double answer[MAX_VARS], resultValue[MAX_VARS];
    double coeff[MAX_VARS*MAX_VARS];
    double sumC[MAX_VARS];
    for( int i = 0; i < varCount; ++i )
	answer[i] = m.random(10);
    for( int j = 0; j < varCount; ++j ) {
	sumC[j] = 0;
	for( i = 0; i < varCount; ++i ) {
	    coeff[j * varCount + i] = m.random(10);
	    sumC[j] += coeff[j * varCount + i] * answer[i];
	}
    }

    if( solve_linear_system( varCount, resultValue, coeff, sumC ) ) {
	for(i = 0; i < varCount; ++i ) {
	    err_when( fabs( resultValue[i] - answer[i] ) > 0.0001 );
	}
    }
}
#endif
