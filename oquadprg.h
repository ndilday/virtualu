// Filename   : OQUADPRG.H
// Description : specialized quadratic programming for finance

#ifndef __OQUADPRG_H
#define __OQUADPRG_H

//!
class QuadProgramming {
public:
    int vars_count;
    int vars_alloc;
    double *test_value;                           // vars_count but allocated size is vars_alloc
    double *value_tol;
    double *nX1_vector;                           // [n];
    double *nXn_matrix;                           // [n*n];
    double *nom_matrix;                           //[vars * vars];

    int constraint_count;
    int constraint_alloc;
    double *constraint_sum;
    int *selected_i;
    double *lambda_c;
    double *mX1_vector;                           // [m]
    double *mXm_matrix;                           // [m*m]

    // constraint_count * vars_count
    double *constraint_coefficient;               // [constraints][vars]

    // vars_count * constraint_count
    double *lambda_p;                             // [N][M] // beware, transposed

    // for matrix determinant

    int determinant_row_alloc;
    double *determinant_swap_tmp;

public:
    QuadProgramming();
    ~QuadProgramming();

    int sp_quadratic_programming( int vars,
				  double *resultValue,
				  // lower and upper limit of variables, and error tolerance
				  double *lowLimit, double *highLimit, double *tolLimit,
				  double *limitCoeff, double limitSum,        // constraint : limitCoeff[i] * variable[i] = limitSum
				  double *cost1,                              // to minimize cost1[i] * variable[i] + cost[i] * variable[i]^2
				  double *cost2 );

    int lcqf_lagrange_multiplier( int vars, int constraints,
				  double *resultValue,                        // output : result
				  double *constraintCoefficient,              // [constraints][vars] coefficient of linear constraint
				  double *constraintSum,                      // [constraints]
				  double *cost1,                              // to minimize cost1[i] * variable[i] + cost[i] * variable[i]^2
				  double *cost2 );

    int solve_linear_system( int vars,
			     double *resultValue,                        // [vars]
			     double *constraintCoefficient,              // [vars][vars] coefficient of linear constraint
			     double *constraintSum );                    // [vars] such that Summation(i, constraintCoefficient[j][i]) = constraintSum[j]

    double matrix_determinant( int nSqSize, int rowPitch, double *matrix );

private:
    void  set_var_count( int );
    void  set_constraint_count( int );
    void  set_determinant_size( int );
};
#endif
