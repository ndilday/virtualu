//Owner: Fred
//Filename    : ONewmat.h
//Description : use matrix library

#define WANT_STREAM                               // include.h will get stream fns
//#define WANT_MATH              // include.h will get math fns

#include "include.h"
//#include "newmat.h"              // need basic matrix operation
#include "newmatap.h"                             // need basic matrix operation
// and SVD - singular value decomposition

#include "newmatio.h"                             // need matrix output routines

#ifdef use_namespace
using namespace NEWMAT;                           // access NEWMAT namespace
#endif
