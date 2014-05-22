//Filename    : OLinAlg.h
//Description : LinAlg Class Declaration: Linear Algebra
//Owner       : Fred

#ifndef __OLinAlg_H
#define __OLinAlg_H

// ES library header file
// CU header file
#include <Onewmat.h>                              // for class Matrix and Vector

#define REAL  Real                                // Real in newmat's include.h
#define Vector  ColumnVector                      // same

//----------- Define constants -------------//
//----------- Define enum types -------------//
//----------- Define struct -------------//

//----------- Define class LinAlg -----//
// class ColumnVector;
// class Matrix;
//!Class which supports quadratic_programming solutions.
class LinearAlgebra {
private:

    //------------------------------------------//

public:
    LinearAlgebra();
    ~LinearAlgebra();
    static bool   quadratic_prog(const Vector &v_c, const Matrix &m_Q, const Matrix &m_A, const Vector &v_b, Vector &v_xNames, int loopCountMultiplier=1);

private:
    static bool   quadratic_prog_not_converged(const Vector &v_x, const Vector &v_y, const Vector &v_Road, const Vector &v_Sigma, REAL gamma);
    //------------------------------------------//

private:

};
#endif                                            //ifndef
