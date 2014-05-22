//Filename    : OLinAlg.cpp
//Description : LinAlg Class Definition:LinearAlgebra
//Owner       : Fred

// ES library header file
#include <All.h>                                  // err*

// CU header file

#include "OLinAlg.h"

//#define DEBUG_VC

#ifdef DEBUG_VC
#include <STDIO.H>                                //temp for debug matrix
#endif
#include <OLOG.H>                                 // for debug matrix

#ifdef DEBUG_CONSOLE
#include <STDIO.H>
#define PRT_MAT (cout <<setw(10)<<setprecision(5))
#define PRT_MAT15 (cout <<setw(10)<<setprecision(15))
#else
#define PRT_MAT
#define PRT_MAT15
#endif

#define fabs(x) ((x)>0 ? (x):-(x))

//----------- Define constants -------------//
//----------- Define static variables -------------//
//----------- Define static functions -------------//
//----------- Define static class member variables -------------//

/*
//----------- Begin of function LinAlg Constructor -----//
//! desc
//! <int> -
//! [int] -
//!
LinearAlgebra::LinearAlgebra()
{

}
//------------- End of function LinAlg Constructor -----//

//----------- Begin of function LinAlg Destructor ------//
//!
LinearAlgebra::~LinearAlgebra()
{

}
//------------- End of function LinAlg Destructor ------//

//----------- Begin of function LinAlg::init --------//
//!
void LinearAlgebra::init()
{
}
//------------- End of function LinAlg::init --------//
*/

//----------- Begin of function LinAlg::deinit --------//
//!
bool LinearAlgebra::quadratic_prog_not_converged(const Vector &v_x, const Vector &v_y, const Vector &v_Road, const Vector &v_Sigma, REAL gamma) {
    //const int MAX			= 100000;
    //const	REAL EPSILON	= 1.0/(10000000000);		// 10^-10
    const int MAX     = 1000000;
    const REAL EPSILON  = 1.0/(1000000);            // 10^-10

    if ( v_x.MaximumValue() > MAX ) {
#ifdef DEBUG_CONSOLE
	cout << ("LinAlg: Problem is primal unbounded:");
#endif

	err_here();
    }

    if ( v_y.MaximumValue() > MAX ) {
#ifdef DEBUG_CONSOLE
	cout << ("LinAlg: Problem is dual unbounded");
#endif
	err_here();
    }

    if ( v_Road.Sum() < EPSILON && v_Sigma.Sum() < EPSILON
	 && gamma < EPSILON
	)
	return false;
    else
	return true;
}

//------------- End of function LinAlg::deinit --------//

//----------- Begin of function LinAlg::quadratic_prog ------//
//! Interior Point Quadratic Programming
//! c, Q, A, b, xNames (no global or member variable access)
//!
//! try, by BM:
//!		c = { 0,0 }
//!		Q = { {2,0}, {0,5} }
//!		A = { {5,6} }
//!		b = { 10 }
//!		xNames={x1,x2}
//!
bool LinearAlgebra::quadratic_prog(const Vector &v_c, const Matrix &m_Q, const Matrix &m_A, const Vector &v_b, Vector &v_xNames, int loopCountMultiplier) {
    // init local variables
    int maxIteration = 20;
    const REAL SIGMA    = 1/15.0;                   // 1/10.0;
    const REAL R      = 9.0/10;
    int iter    = 0;

    int n = v_c.Storage();
    int m = v_b.Storage();

    maxIteration *= loopCountMultiplier;

#ifdef DEBUG_VC
    DEBUG_LOG("----------- quadratic_prog begin -----------");
    // print_input(c,Q,A,b,xNames)
    if ( n==10 && m==12 ) {
	char s[500];

	DEBUG_LOG("c = ");
	sprintf(s, "{ %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f };",
		v_c(1),v_c(2),v_c(3),
		v_c(4),v_c(5),v_c(6),
		v_c(7),v_c(8),v_c(9), v_c(10));
	DEBUG_LOG(s);
	DEBUG_LOG("Q = ");
	for (int i=1; i<=n; i++) {
	    sprintf(s, "{%f, %f, %f, %f, %f, %f, %f, %f, %f, %f},",
		    m_Q(i,1),m_Q(i,2),m_Q(i,3),m_Q(i,4),m_Q(i,5),m_Q(i,6),m_Q(i,7),m_Q(i,8),m_Q(i,9),m_Q(i,10)
		);
	    DEBUG_LOG(s);
	}
	DEBUG_LOG("A = ");
	for (i=1; i<=m; i++) {
	    sprintf(s, "{%f, %f, %f, %f, %f, %f, %f, %f, %f, %f},",
		    m_A(i,1),m_A(i,2),m_A(i,3),m_A(i,4),m_A(i,5),m_A(i,6),m_A(i,7),m_A(i,8),m_A(i,9),m_A(i,10)
		);
	    DEBUG_LOG(s);
	}
	DEBUG_LOG("b = ");
	sprintf(s, "{%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f,",
		v_b(1),v_b(2),v_b(3),
		v_b(4),v_b(5),v_b(6),
		v_b(7),v_b(8),v_b(9), v_b(10), v_b(11), v_b(12)
	    );
	DEBUG_LOG(s);
	/*sprintf(s, "%f, %f, %f, %f, %f, %f, %f, %f, %f,",
	  v_b(10),v_b(11),v_b(12),
	  v_b(13),v_b(14),v_b(15),
	  v_b(16),v_b(17),v_b(18)
	  );
	  DEBUG_LOG(s);
	  sprintf(s, "%f, %f};",
	  v_b(19),v_b(20));
	  DEBUG_LOG(s);*/

    }
    else if ( n==9 && m==20 ) {                     // stage 1
	char s[500];

	DEBUG_LOG("c = ");
	sprintf(s, "{ %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f };",
		v_c(1),v_c(2),v_c(3),
		v_c(4),v_c(5),v_c(6),
		v_c(7),v_c(8),v_c(9));
	DEBUG_LOG(s);
	DEBUG_LOG("Q = ");
	for (int i=1; i<=n; i++) {
	    sprintf(s, "{%f, %f, %f, %f, %f, %f, %f, %f, %f, },",
		    m_Q(i,1),m_Q(i,2),m_Q(i,3),m_Q(i,4),m_Q(i,5),m_Q(i,6),m_Q(i,7),m_Q(i,8),m_Q(i,9)
		);
	    DEBUG_LOG(s);
	}
	DEBUG_LOG("A = ");
	for (i=1; i<=m; i++) {
	    sprintf(s, "{%f, %f, %f, %f, %f, %f, %f, %f, %f },",
		    m_A(i,1),m_A(i,2),m_A(i,3),m_A(i,4),m_A(i,5),m_A(i,6),m_A(i,7),m_A(i,8),m_A(i,9)
		);
	    DEBUG_LOG(s);
	}
	DEBUG_LOG("b = ");
	sprintf(s, "{%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, }, ",
		v_b(1),v_b(2),v_b(3),
		v_b(4),v_b(5),v_b(6),
		v_b(7),v_b(8),v_b(9), v_b(10), v_b(11), v_b(12),
		v_b(13),v_b(14),v_b(15), v_b(16), v_b(17), v_b(18), v_b(19), v_b(20)
	    );
	DEBUG_LOG(s);
    }
    else if ( n==10 && m==22 ) {                    // stage 2
	char s[500];

	DEBUG_LOG("c = ");
	sprintf(s, "{ %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f };",
		v_c(1),v_c(2),v_c(3),
		v_c(4),v_c(5),v_c(6),
		v_c(7),v_c(8),v_c(9),v_c(10));
	DEBUG_LOG(s);
	DEBUG_LOG("Q = ");
	for (int i=1; i<=n; i++) {
	    sprintf(s, "{%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, },",
		    m_Q(i,1),m_Q(i,2),m_Q(i,3),m_Q(i,4),m_Q(i,5),m_Q(i,6),m_Q(i,7),m_Q(i,8),m_Q(i,9),m_Q(i,10)
		);
	    DEBUG_LOG(s);
	}
	DEBUG_LOG("A = ");
	for (i=1; i<=m; i++) {
	    sprintf(s, "{%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f },",
		    m_A(i,1),m_A(i,2),m_A(i,3),m_A(i,4),m_A(i,5),m_A(i,6),m_A(i,7),m_A(i,8),m_A(i,9),m_A(i,10)
		);
	    DEBUG_LOG(s);
	}
	DEBUG_LOG("b = ");
	sprintf(s, "{%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, }, ",
		v_b(1),v_b(2),v_b(3),
		v_b(4),v_b(5),v_b(6),
		v_b(7),v_b(8),v_b(9), v_b(10), v_b(11), v_b(12),
		v_b(13),v_b(14),v_b(15), v_b(16), v_b(17), v_b(18), v_b(19), v_b(20), v_b(21), v_b(22)
	    );
	DEBUG_LOG(s);
    }
#endif

    Vector  v_eN(n);    v_eN = 1;
    Vector  v_eM(m);    v_eM = 1;

    Vector  v_x(n);   v_x = 1;
    Vector  v_z(n);   v_z = 1;
    Vector  v_y(m);   v_y = 1;
    Vector  v_w(m);   v_w = 1;

    // check  m_A is nxm, m_Q is nxn
    err_when(m_A.Nrows() != m || m_A.Ncols() != n );
    err_when(m_Q.Nrows() != n || m_Q.Ncols() != n );

    // init for while loop not_converged() checking
    //
    Vector v_Road = v_b - m_A*(v_x) + v_w;
    Vector v_Sigma = v_c - m_A.t() * v_y - v_z + m_Q*v_x;
    REAL  gamma   = (v_z.t()*v_x + v_y.t()*v_w).AsScalar();
    REAL  mute    = SIGMA*gamma / (n+m);

    DiagonalMatrix X(n),  Xinv(n);
    DiagonalMatrix Y(m),  Yinv(m);
    DiagonalMatrix Z(n);
    DiagonalMatrix W(m);
    Matrix T1(n,n), T2(n,m), T3(m,n), T4(m,m);

    Vector KKFvec(n+m);                             //, v_tmp1(n), v_tmp2(m);

    Vector m_temp(m+n);
    Vector v_dx(n), v_dy(m), v_dz(n), v_dw(m);
    REAL  theeta;

    X=0; Xinv=0; Z=0;
    Y=0; Yinv=0; W=0; {

	//	Matrix _t1(n,n), _t2(n,n);		_t1=1;	_t2=2;
	//	_t1 = SP(_t1,_t2);
    }

    Matrix KKFmat(m+n,m+n);
    Matrix KKFmatInverse(m+n,m+n);

    REAL min=1;

    while ( quadratic_prog_not_converged(v_x,v_y,v_Road,v_Sigma,gamma) && iter <= maxIteration && min > 0.0000000001 ) {
	iter++;
	v_Road  = v_b - m_A*v_x + v_w;
	v_Sigma = v_c - m_A.t()*v_y - v_z + m_Q*v_x;
	gamma   = (v_z.t()*v_x + v_y.t()*v_w).AsScalar();
	mute    = SIGMA*gamma / (n+m);

	X.set_diagonal(v_x);
	Y.set_diagonal(v_y);
	Z.set_diagonal(v_z);
	W.set_diagonal(v_w);

	Xinv.set_diagonal(ElmDivide(v_eN, v_x));
	Yinv.set_diagonal(ElmDivide(v_eM, v_y));

	T1 = -(Xinv * Z + m_Q);
	T2 = m_A.t();
	T3 = m_A;
	T4 = Yinv*W;
	//PRT_MAT << "T1: " << T1 << endl;
	KKFmat = (T1|T2) & (T3|T4);

	KKFvec = (v_c - T2*v_y - mute*Xinv*v_eN + m_Q*v_x)
	    & (v_b - m_A*v_x + mute*Yinv*v_eM);

	Try {
	    /*{
	      bool _f;
	      DiagonalMatrix _dmat(n+m);		_dmat=1;
	      Matrix _inv(n+m,n+m);

	      SVD(KKFmat, _dmat, _inv);

	      _dmat=1;
	      if ( KKFmat*_inv == _dmat )
	      _f = true;
	      else
	      _f = false;
	      }*/

	    min = fabs(KKFmat(1,1));

	    for (int i=2; i<=m+n; i++) {
		if ( min > fabs(KKFmat(i,i)) )            // check diagonal element
		    min = fabs(KKFmat(i,i));
	    }

	    KKFmatInverse = KKFmat.i();

	    /*
	      if ( KKFmat.LogDeterminant().Value() )
	      KKFmatInverse = KKFmat.i();
	      else
	      break;
	    */

	    m_temp =  KKFmatInverse * KKFvec;

	    v_dx  = m_temp.Rows(1,n);
	    v_dy  = m_temp.Rows(n+1,n+m);

	    v_dz  = Xinv*(mute*v_eN - X*Z*v_eN - Z*v_dx);
	    v_dw  = Yinv*(mute*v_eM - Y*W*v_eM - W*v_dy);
	    //PRT_MAT << "v_dx,z,y,w: " << (v_dx|v_dz) <<", "<< (v_dy|v_dw) <<endl;
	    //PRT_MAT << "v_x: " << v_x << endl;
	    //PRT_MAT << "ElmDivide(v_x,v_dx): " << ElmDivide(v_x,v_dx) << endl;

	    theeta =
		(R/((
		    ElmDivide(-v_dx,v_x)
		    & ElmDivide(-v_dw,v_w)
		    & ElmDivide(-v_dy,v_y)
		    & ElmDivide(-v_dz,v_z)
		    )).MaximumValue()
		    );
	    if(theeta>1)                                // theeta = min(theeta,1)
		theeta = 1;

	    v_x += theeta * v_dx;
	    v_y += theeta * v_dy;
	    v_w += theeta * v_dw;
	    v_z += theeta * v_dz;

#ifdef DEBUG_CONSOLE
	    cout << "#iter "<< iter << ": " << v_Road.Sum() << ", " << v_Sigma.Sum() << ", " << gamma << endl;
	    cout << "Ro:" << v_Road << endl;
	    PRT_MAT15 << "v_x:" << v_x << endl;
	    PRT_MAT15 << "v_y:" << v_y << endl;
	    PRT_MAT15 << "v_w:" << v_w << endl;
	    PRT_MAT15 << "v_z:" << v_z << endl;
#elif defined(DEBUG_VC)

	    char s[200];
	    sprintf(s, "#iter %d: %f, %f, %f",
		    iter, v_Road.Sum(), v_Sigma.Sum(), gamma);
	    DEBUG_LOG(s);
	    if ( n == 9 && false ) {
		DEBUG_LOG("x = ");
		sprintf(s, "   %f, %f, %f, %f, %f, %f, %f, %f, %f",
			v_x(1),v_x(2),v_x(3),
			v_x(4),v_x(5),v_x(6),
			v_x(7),v_x(8),v_x(9));
		DEBUG_LOG(s);
	    }
#endif
	}
	CatchAll {
#ifdef DEBUG_CONSOLE
	    cout << Exception::what();
#endif
#ifdef DEBUG_VC
	    DEBUG_LOG("olinalg: failure in quad_prog");
	    DEBUG_LOG((char *)Exception::what());
#endif
	    return false;
	}
    }                                               // while

    v_xNames = v_x;

    if ( min < 0.00001 ) {                          // 1214 || KKFmat.LogDeterminant().Value() == 0 )
	//char s[200];

	//sprintf(s, "#iter %d: %f, %f, %f",
	//	iter, v_Road.Sum(), v_Sigma.Sum(), gamma);
	//DEBUG_LOG(s);
	DEBUG_LOG("--- quad_prog early exit for min < 0.00001 ---");
    }
    else
	DEBUG_LOG("----------- quad_prog normal exit -----------");

#ifdef DEBUG_CONSOLE
    cout << "#iter: " << iter;
#endif

    return true;
}

//------------- End of function LinAlg::quadratic_prog ------//
