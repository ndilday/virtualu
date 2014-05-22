//Filename		: OFINANC2.h
//Description	: Constants for Finance Class Declaration;
//						For stage 1 & 2 intermediate variables:
//						"Structural equations" and "Expense balance" in ResAlloc.ex.xls
//Ower		   : Fred

#ifndef __OFINANC2_H
#define __OFINANC2_H

// constants used in optimization
#define OPT_LOW_WEIGHT    1                       // 0.5		// '1' on interface
#define OPT_MID_WEIGHT    4                       // 1.0		// '2' on interface
#define OPT_HIGH_WEIGHT   10                      // 1.5		// '3' on interface

#define OPT_MINIMUM_RANGE 0.1                     // upper_bound lower_bound >= OPT_MINIMUM_RANGE

#define MAX_S3_SLIDER   10                        //990518

enum {
    P_NONE = 0, P_CONSIDER, P_PROMISE, P_IMPLEMENT,
};

//-------- define stage 1 revenue; part of Strucaltural equations -------//

enum { STAGE1_REVENUE_ITEM_COUNT = 10 };

enum {
    S1_GROSS_TUITION_INCOME,
    S1_FINANCIAL_AID,
    // S1_STUDENT_INCOME_NET,		// by audit the shell g188 in resalloc.ex.xls, this can be ignored
    S1_GIFTS,
    S1_INTERCOLLEGIATE_ATHLETICS,
    S1_OTHER_OPERATING_INCOME,
    S1_INTEREST_EARNED_OR_PAID,
    S1_STATE_APPROPRIATION,                         // 1106
    S1_RESEARCH_INDIRECTS,
    S1_ENDOWMENT_SPENDING,
    S1_DIRECT_SPONSORED_RESEARCH,                   // special case field for row 214 in resalloc.ex.xls
};

//-------- define stage 1 expenditure; part of Strucaltural equations -------//

enum { STAGE1_EXPENSE_ITEM_COUNT = 6 };

enum {
    S1_FACULTY_SALARY_INCREASES,
    S1_STAFF_SALARY_INCREASES,
    S1_OTHER_COST_RISE_PROVISION,
    S1_NET_OPERATING_BUGET_GROWTH,
    S1_DEBT_SERVICE_GROWTH,
    S1_REAL_TRANSFER_TO_PLANT_GROWTH,
};

//!Structure containing stage one revenue used for calculations in the
//!Finance class.
struct Stage1Revenue {
    double  base_value;
    double  adjust;
    double  adjusted_base;
    double  computed_growthrate;
    double  new_value;
};

//!Structure containing stage one expenses used for calculations in the
//!Finance class.
struct Stage1Expense {
    double  base_value;
    double  computed_growthrate;
    double  new_value;
};

//-------- define stage 2's "Expense balance" -------//

enum { STAGE2_EXPENSE_POLICY_COUNT = 10 };

enum {
    S2_DEPARTMENTAL_FACULTY,
    S2_DEPARTMENTAL_NON_FACULTY,
    S2_LIBRARIES,
    S2_IT_RESOURCES,                                // 1005
    S2_STUDENT_LIFE,
    S2_ATHLETICS,
    S2_INST_ADVANCEMENT,
    S2_ADMINISTRATION,
    S2_OPERATIONS_AND_MAINTENANCE,
    S2_ENROLLMENT_MANAGEMENT,
};

//!Structure containing stage two expenses used for calculations in the
//!Finance class.
struct Stage2Expense {
    double  base_value;
    double  dept_faculty_new_price;
    double  computed_growthrate;
    double  change_value;
    double  new_value;
};

//------------------------------------------//
#endif
