//Filename 	 : OMATH.CPP
//Description: A class of math functions
//Owner      : Fred

#include <All.h>
#include <OMATH.H>
#include <OMISC.H>

//------- Define constant --------//

#define LN81    4.394449f                         // natural algorithm for response functions

//------ Begin of function Math::double_poisson -------//
//!
//! Double Poisson(n, mu) 	= (exp[-mu]*mu^n)/n!  	/* Poison probability distribution */
//!
float Math::double_poisson(int n, float mu) {
    int nFactor=1;

    for (int i=2; i<=n; i++) {
	nFactor *= i;
    }

    return float( exp(-mu) * safe_pow(mu,(float)n) ) / nFactor;
}

//------- End of function Math::double_poisson --------//

//------ Begin of function Math::single_response_func -------//
//!
//! Standard single response function.
//!
float Math::single_response_func(float floor, float ceiling, float b1, float m1, float v) {
    return float( floor + (ceiling - floor) / ( 1 + exp(-LN81 * (v-m1) / b1) ) );
}

//------- End of function Math::single_response_func --------//

//------ Begin of function Math::dual_response_func -------//
//!
//! Standard dual response function.
//!
float Math::dual_response_func(float floor, float base, float ceiling,
			       float b1, float b2, float m1, float m2, float v) {
    // chwong 071599
    // avoid divide 0
    if( fabs(b1)<1e-9 ) return floor;
    if( fabs(b2)<1e-9 ) return floor;
    if( fabs(1+exp(-LN81*(v-m1)/b1))<1e-9 ) return 1e9f;
    if( fabs(1+exp(-LN81*(v-m2)/b2))<1e-9 ) return 1e9f;

    return float( floor + (base-floor) / ( 1 + exp(-LN81 * (v-m1) / b1) ) +
		  (ceiling-base) / (1 + exp(-LN81 * (v-m2)/b2) ) );
}

//------- End of function Math::dual_response_func --------//

//------ Begin of function Math::latency_func -------//
//!
//! Standard latency function.
//!
//! <float> latencyPara - latency parameter
//! <float> curValue    - the current value
//! <float> inputValue  - the new input
//!
float Math::latency_func(float latencyPara, float curValue, float inputValue) {
    // maintenance upgrade Aug 2001
    // return latencyPara * inputValue + (1 - latencyPara) * curValue;
    return latencyPara * curValue + (1 - latencyPara) * inputValue;
}

//------- End of function Math::latency_func --------//

//------ Begin of function Math::average_float -------//
//!
//! Calculate the average values of a list of variables.
//!
//! <char*> structArray - pointer to an structure array.
//! <short> structSize  - size of a structure in the array
//! <short> structCount - number of structures
//! <short> varOffset   - offset of the variable to be averaged
//! [bool]  sizeZero    - skip field which equals to zero (default=false)
//!
float Math::average_float(char* structArray, short structSize, short structCount, short varOffset, bool skipZero) {
#define BOUND 0.000001
    char* ptr = structArray;
    float totalValue = 0.0f, tmp;

    if ( skipZero ) {
	short structCountSkipZero = 0;

	for( int i=0 ; i<structCount ; i++, ptr += structSize ) {
	    tmp = *((float*)(ptr + varOffset));
	    if ( !(tmp < BOUND && tmp > -BOUND ) ) {
		totalValue += tmp;
		structCountSkipZero++;
	    }
	}

	if ( structCountSkipZero )
	    return totalValue / structCountSkipZero;
	else
	    return 0;
    }
    else {
	for( int i=0 ; i<structCount ; i++, ptr += structSize ) {
	    totalValue += *((float*)(ptr + varOffset));
	}

	return totalValue / structCount;
    }
}

//------- End of function Math::average_float --------//

//------ Begin of function Math::average_char -------//
//!
//! Calculate the average values of a list of variables.
//!
//! <char*> structArray - pointer to an structure array.
//! <short> structSize  - size of a structure in the array
//! <short> structCount - number of structures
//! <short> varOffset   - offset of the variable to be averaged
//! [bool]  sizeZero    - skip field which equals to zero (default=false)
//!
char Math::average_char(char* structArray, short structSize, short structCount, short varOffset, bool skipZero) {
    char* ptr = structArray;
    int   totalValue = 0, tmp;

    if ( skipZero ) {
	short structCountSkipZero = 0;

	for( int i=0 ; i<structCount ; i++, ptr += structSize ) {
	    tmp = *((char*)(ptr + varOffset));

	    if ( tmp != 0 ) {
		totalValue += tmp;
		structCountSkipZero++;
	    }
	}

	if ( structCountSkipZero )
	    return totalValue / structCountSkipZero;
	else
	    return 0;
    }
    else {
	for( int i=0 ; i<structCount ; i++, ptr += structSize ) {
	    totalValue += *((char*)(ptr + varOffset));
	}

	return totalValue / structCount;
    }
}

//------- End of function Math::average_char --------//

//------ Begin of function Math::average_short -------//
//!
//! Calculate the average values of a list of variables.
//!
//! <char*> structArray - pointer to an structure array.
//! <short> structSize  - size of a structure in the array
//! <short> structCount - number of structures
//! <short> varOffset   - offset of the variable to be averaged
//! [bool]  sizeZero    - skip field which equals to zero (default=false)
//!
short Math::average_short(char* structArray, short structSize, short structCount, short varOffset, bool skipZero) {
    char* ptr = structArray;
    int   totalValue = 0, tmp;

    if ( skipZero ) {
	short structCountSkipZero = 0;

	for( int i=0 ; i<structCount ; i++, ptr += structSize ) {
	    tmp = *((short*)(ptr + varOffset));

	    if ( tmp != 0 ) {
		totalValue += tmp;
		structCountSkipZero++;
	    }
	}

	if ( structCountSkipZero )
	    return totalValue / structCountSkipZero;
	else
	    return 0;
    }
    else {
	for( int i=0 ; i<structCount ; i++, ptr += structSize ) {
	    totalValue += *((short*)(ptr + varOffset));
	}

	return totalValue / structCount;
    }
}

//------- End of function Math::average_short --------//

//------ Begin of function Math::average_int -------//
//!
//! Calculate the average values of a list of variables.
//!
//! <char*> structArray - pointer to an structure array.
//! <short> structSize  - size of a structure in the array
//! <short> structCount - number of structures
//! <short> varOffset   - offset of the variable to be averaged
//! [bool]  sizeZero    - skip field which equals to zero (default=false)
//!
int Math::average_int(char* structArray, short structSize, short structCount, short varOffset, bool skipZero) {
    char* ptr = structArray;
    long  totalValue = 0;
    long  tmp;

    if ( skipZero ) {
	short structCountSkipZero = 0;

	for( int i=0 ; i<structCount ; i++, ptr += structSize ) {
	    tmp = *((int*)(ptr + varOffset));

	    if ( tmp != 0 ) {
		totalValue += tmp;
		structCountSkipZero++;
	    }
	}

	if ( structCountSkipZero )
	    return totalValue / structCountSkipZero;
	else
	    return 0;
    }
    else {
	for( int i=0 ; i<structCount ; i++, ptr += structSize ) {
	    totalValue += *((int*)(ptr + varOffset));
	}

	return totalValue / structCount;
    }

    /* //old code
       char* ptr = structArray;
       int   totalValue = 0;

       for( int i=0 ; i<structCount ; i++, ptr += structSize )
       {
       totalValue += *((int*)(ptr + varOffset));
       }

       return totalValue / structCount;
    */
}

//------- End of function Math::average_int --------//

//------ Begin of function Math::get_random_float -------//
//!
//! return a floating point value within the range [0:1]
//!
float Math:: get_random_float() {
    const int randomPrecesion = 10000;
    return float(m.random(randomPrecesion+1)) / randomPrecesion;
}

//-------- End of function Math::get_random_float -------//

//------ Begin of function Math::get_random_snd -------//
//!
//! returns a random value in standard normal distribution
//! with average avg and standard deviation sd
//! write code here according to the book "Numerical Receipe in C"
//!
//! nonNegative:	trancate the return value to zero or positive value;
//!						default=false;
//!
float Math::get_random_snd(float avg, float sd, bool returnNonNegative) {
    // 1. calc ret which is a normally distributed deviate with zero mean and unit variance
    static bool iset=false;
    static float gset;

    float fac, rsq, ret, v1, v2;

    if ( !iset ) {
	do {
	    v1 = 2.0f * get_random_float() - 1.0f;
	    v2 = 2.0f * get_random_float() - 1.0f;
	    rsq = v1*v1 + v2*v2;
	}
	while (rsq >= 1.0 || rsq == 0.0);

	fac = (float) sqrt(-2.0f * log(rsq) / rsq );
	iset = true;
	gset = v1 * fac;
	ret = v2 * fac;
    }
    else {
	iset = false;
	ret = gset;
    }

    // 2. re-scale gset for return
    if ( returnNonNegative )
	return max(0.0f, gset * sd + avg);            // min & max bug chea
    else
	return gset*sd + avg;
}

int Math::get_random_snd(int avg, float sd) {
    float r = get_random_snd((float)avg, sd);

    return int(r+0.5f);                             // round it
}

//------- End of function Math::get_random_snd --------//

//------ Begin of function Math::get_random_gamma_CDF -------//
//!
//! return a random value in gamma distribution
//! with average avg and temp sd
//!
float Math::get_random_gamma_CDF(float avg, float sd, float zbase) {
    //askbillok gamma
    //TO
    return get_random_snd(avg, sd);
}

//------- End of function Math::get_random_gamma_CDF --------//

// new in GAME_VERSION>=200
//------ Begin of function Math::score_function -------//

float Math::score_function(float x, float y, float mx, float my, int test ) {
    double theta = atan2( mx, my );

    if( test )
	return float(x * cos(theta) + y * sin(theta));// strength
    else
	// consistency
	return float(x * -sin(theta) + y * cos(theta));
}

//------ End of function Math::score_function -------//
