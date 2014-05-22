//Owner: Fred
//$$ precisio.h                          floating point constants

#ifndef PRECISION_LIB
#define PRECISION_LIB 0

#ifdef use_namespace
namespace NEWMAT {
#endif

#ifndef SystemV                                 // if there is float.h

#ifdef USING_FLOAT

    //!Wrapper class for floating point operations - define the following to
    //!specify {SystemV, USING_FLOAT, USING_DOUBLE }
    class FloatingPointPrecision {
    public:
	static int Dig() {
	    // number of decimal digits or precision
	    return FLT_DIG;
	}

	static Real Epsilon() {
	    // smallest number such that 1+Eps!=Eps
	    return FLT_EPSILON;
	}

	static int Mantissa() {
	    // bits in mantisa
	    return FLT_MANT_DIG;
	}

	static Real Maximum() {
	    // maximum value
	    return FLT_MAX;
	}

	static int MaximumDecimalExponent() {
	    // maximum decimal exponent
	    return FLT_MAX_10_EXP;
	}

	static int MaximumExponent() {
	    // maximum binary exponent
	    return FLT_MAX_EXP;
	}

	static Real LnMaximum() {
	    // natural log of maximum
	    return (Real)log(Maximum());
	}

	static Real Minimum() {
	    // minimum positive value
	    return FLT_MIN;
	}

	static int MinimumDecimalExponent() {
	    // minimum decimal exponent
	    return FLT_MIN_10_EXP;
	}

	static int MinimumExponent() {
	    // minimum binary exponent
	    return FLT_MIN_EXP;
	}

	static Real LnMinimum() {
	    // natural log of minimum
	    return (Real)log(Minimum());
	}

	static int Radix() {
	    // exponent radix
	    return FLT_RADIX;
	}

	static int Rounds() {
	    // addition rounding (1 = does round)
	    return FLT_ROUNDS;
	}

    };
#endif

#ifdef USING_DOUBLE

    //!Wrapper class for floating point operations - define the following to
    //!specify {SystemV, USING_FLOAT, USING_DOUBLE }
    class FloatingPointPrecision {
    public:

	static int Dig() {
	    // number of decimal digits or precision
	    return DBL_DIG;
	}

	static Real Epsilon() {
	    // smallest number such that 1+Eps!=Eps
	    return DBL_EPSILON;
	}

	static int Mantissa() {
	    // bits in mantisa
	    return DBL_MANT_DIG;
	}

	static Real Maximum() {
	    // maximum value
	    return DBL_MAX;
	}

	static int MaximumDecimalExponent() {
	    // maximum decimal exponent
	    return DBL_MAX_10_EXP;
	}

	static int MaximumExponent() {
	    // maximum binary exponent
	    return DBL_MAX_EXP;
	}

	static Real LnMaximum() {
	    // natural log of maximum
	    return (Real)log(Maximum());
	}

	static Real Minimum() {
	    //#ifdef __BCPLUSPLUS__
	    //       return 2.225074e-308;     // minimum positive value
	    //#else
	    return DBL_MIN;
	    //#endif
	}

	static int MinimumDecimalExponent() {
	    // minimum decimal exponent
	    return DBL_MIN_10_EXP;
	}

	static int MinimumExponent() {
	    // minimum binary exponent
	    return DBL_MIN_EXP;
	}

	static Real LnMinimum() {
	    // natural log of minimum
	    return (Real)log(Minimum());
	}

	static int Radix() {
	    // exponent radix
	    return FLT_RADIX;
	}

	static int Rounds() {
	    // addition rounding (1 = does round)
	    return FLT_ROUNDS;
	}

    };
#endif
#endif

#ifdef SystemV                                  // if there is no float.h

#ifdef USING_FLOAT

    //!Wrapper class for floating point operations - define the following to
    //!specify {SystemV, USING_FLOAT, USING_DOUBLE }
    class FloatingPointPrecision {
    public:

	static Real Epsilon() {
	    // smallest number such that 1+Eps!=Eps
	    return pow(2.0,1-FSIGNIF);
	}

	static Real Maximum() {
	    // maximum value
	    return MAXFLOAT;
	}

	static Real LnMaximum() {
	    // natural log of maximum
	    return (Real)log(Maximum());
	}

	static Real Minimum() {
	    // minimum positive value
	    return MINFLOAT;
	}

	static Real LnMinimum() {
	    // natural log of minimum
	    return (Real)log(Minimum());
	}

    };
#endif

#ifdef USING_DOUBLE

    //!Wrapper class for floating point operations - define the following to
    //!specify {SystemV, USING_FLOAT, USING_DOUBLE }
    class FloatingPointPrecision {
    public:

	static Real Epsilon() {
	    // smallest number such that 1+Eps!=Eps
	    return pow(2.0,1-DSIGNIF);
	}

	static Real Maximum() {
	    // maximum value
	    return MAXDOUBLE;
	}

	static Real LnMaximum() {
	    // natural log of maximum
	    return LN_MAXDOUBLE;
	}

	static Real Minimum()
	    { return MINDOUBLE; }

	static Real LnMinimum() {
	    // natural log of minimum
	    return LN_MINDOUBLE;
	}
    };
#endif
#endif

#ifdef use_namespace
}
#endif
#endif
