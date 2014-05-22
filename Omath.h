//Filename    : OMATH.H
//Description : Math Class Declaration
//Owner       : Fred

#ifndef __OMATH_H
#define __OMATH_H

#include <math.h>

//----------- Define class Math ---------------//

//!Commonly used general purpose math utilities.
class Math {
public:
    float single_response_func(float floor, float ceiling, float b1, float m1, float v);
    float dual_response_func(float floor, float base, float ceiling, float b1, float b2, float m1, float m2, float v);
    float latency_func(float latencyPara, float curValue, float inputValue);
    float double_poisson(int n, float mu);

    float average_float(char* structArray, short structSize, short structCount, short varOffset, bool skipZero=false);
    char  average_char (char* structArray, short structSize, short structCount, short varOffset, bool skipZero=false);
    short average_short(char* structArray, short structSize, short structCount, short varOffset, bool skipZero=false);
    int   average_int  (char* structArray, short structSize, short structCount, short varOffset, bool skipZero=false);

    float get_random_float();
    float get_random_snd(float avg, float sd, bool returnNonNegative=false);
    int get_random_snd(int avg, float sd);
    float get_random_gamma_CDF(float avg, float sd, float zbase);

    double inline safe_divide(double f1, double f2);
    float inline safe_divide(float f1, float f2);
    float inline safe_divide(int  f1, int f2);
    float inline safe_pow(float f1, float f2);
    float inline safe_log(float f1);

    float inline time_variation(float x1, float x2, float w1, float w2, float sd);

    // new in GAME_VERSION>=200
    float score_function(float x, float y, float mx, float my, int test );
};

extern Math math;

//---------------------------------------------//

//------- Begin of function Math::safe_divide --------//
double Math::safe_divide(double f1, double f2) {
    return ( f2 == 0 ) ? 0 : f1/f2;
}

float Math::safe_divide(float f1, float f2) {
    return ( f2 == 0 ) ? 0 : f1/f2;
}

float Math::safe_divide(int f1, int f2) {
    return ( f2 == 0 ) ? 0 : float(f1)/f2;
}

//------- End of function Math::safe_divide --------//

//------- End of function Math::safe_divide --------//
float Math::safe_pow(float f1, float f2) {
    if ( f1 == 0 && f2 < 0 )
	return 1;
    else if ( f1 < 0 && f2 != int(f2) ) {           // check f2 is floating point
	return 1;
    }
    else
	return (float) pow( (double) f1,  (double)f2);
}

//------- End of function Math::safe_divide --------//

//------- End of function Math::safe_divide --------//
float inline Math::safe_log(float f1) {
    return ( f1<=0 ) ? 1.0f : (float) log(f1);
}

//------- End of function Math::safe_divide --------//

//------- End of function Math::safe_divide --------//
float Math::time_variation(float x1, float x2, float w1, float w2, float sd) {
    return get_random_snd(0.0f, sd);                // 0405  + w1 * x1 + w2 * x2;
}

//------- End of function Math::safe_divide --------//
#endif
