//Filename    : OFINANCE.CPP
//Description : School class
//Owner       : Fred

#include <OMATH.H>
#include <OSYS.H>
#include <OGAMESET.H>

#include <OSCHLRES.H>
#include <OPEERSCH.H>                             // for init_data
#include <OSTUDENT.H>                             // MAX_STUDENT_LEVEL
#include <ODEPT.H>                                // facutly_count
#include <OCHANCE.H>

// CU header file
#include <OPSCHOOL.H>
#include <OINVEST.H>
#include <OATHLETI.H>
#include <OFACILIT.H>
#include <ODEVELOP.H>                             //990608
#include <OOPT.H>

#include <OLinAlg.h>                              // for its quadratic_prog() and class Matrix
#include <OQUADPRG.H>
#include <OFINANCE.H>
#include <OCHANCE.H>                              //## chea 230699

#include <OSTUOFF.H>
#include <OLIBTECH.H>

#ifdef DEBUG
//#define _DEBUG_STAGE1
//#define _DEBUG_STAGE2
#endif

#define DEBUG_VC
#ifdef DEBUG_VC
#include <STDIO.H>                                //temp for debug matrix
#endif
#include <OLOG.H>                                 // for debug matrix

// --------- "Input from elsewhere in the model" --------- //
//TO make these all to member variables or macro

/*
X	faculty_ftes = 320;
  avg_faculty_compensation = $268
  Indirect cost rate	50.0%
  spons_research_for_AY_fac_sals = 20;
  percent_of_sponsored_research_for_other_salaries = 40.00;

X	tuition_rate = 18260;
X	Room and board rate	$6,583
X	Real growth of room & board rate	0.00%
  New building construction	$0
New debt	$20,000
*/
//const int	faculty_ftes = 230;					// for stage2
//const int	avg_faculty_compensation = 268;	// for stage2; from resalloc.ex file: "This is an unrealisticly large number, caused by an inconsistency between the financial data used for testing this partof the model and the assumed bumber of faculty. Setting initial conditions from the game's database should take care of the problem."
const int percent_spons_research_for_AY_fac = 20; // c61
// c62; for buget_report
const float percent_of_sponsored_research_for_other_salaries = 40.00;

// constants for buget_revenue/expense
//const int	library_acqs = 3780;
//const	int	utilities_expense	= 3285;

//---------- Begin of static function prt_vector -----------//
static void print_vector(const ColumnVector &xNames) {
#ifdef DEBUG_VC
    char s[300];
    sprintf(s, "%f, %f, %f, %f, %f, %f, %f, %f, %f",
	    xNames(1),xNames(2),xNames(3),
	    xNames(4),xNames(5),xNames(6),
	    xNames(7),xNames(8),xNames(9));
    DEBUG_LOG(s);
#endif
}

static void print_vector2(const ColumnVector &xNames) {
#ifdef DEBUG_VC
    int count = xNames.Storage();
    char t[30];

    String s = "-> ";
    for (int i=1; i<=count; i++ ) {
	sprintf(t, "%.3f, ", xNames(i));
	s+= t;
    }
    DEBUG_LOG(s);
#endif
}

//---------- Begin of static function prt_vector -----------//

//---------- Begin of function FinancePolicy::bound_correction -----------//
void FinancePolicy::bound_correction() {
    err_when(lower_limit > upper_limit);

    if (lower_bound > upper_bound)
	lower_bound = upper_bound;

    lower_bound = min(upper_limit, max(lower_bound, lower_limit));
    upper_bound = max(lower_limit, min(upper_bound, upper_limit));

    target_value = min(upper_bound, max(lower_bound, target_value));
    result_value = min(upper_bound, max(lower_bound, result_value));
}

//---------- End of function FinancePolicy::bound_correction -----------//

//---------- Begin of function FinancePolicy::constraint_policy -----------//
//!
//! [int] saveLast - save to last_target_value and etc (default: 1)
//!
void FinancePolicy::set_constraint_policy(int saveLast) {
    if( saveLast ) {
	last_target_value = target_value;
	last_upper_bound  = upper_bound;
	last_lower_bound  = lower_bound;
    }

    lower_bound = min(upper_limit,max(lower_limit, target_value - OPT_MINIMUM_RANGE ));
    upper_bound = max(lower_limit, min(upper_limit, target_value + OPT_MINIMUM_RANGE ));

    target_value = min( max(target_value , lower_bound), upper_bound);
    bound_correction();                             // ## 290699  //## chea 310899 try not using all the bound_correction
}

void FinancePolicy::reset_constraint_policy() {
    target_value = last_target_value;
    upper_bound  = last_upper_bound;
    lower_bound  = last_lower_bound;

    target_value = min( max(target_value , lower_bound), upper_bound);
    bound_correction();                             // ## 290699 //## chea 310899 try not using all the bound_correction
}

//---------- End of function FinancePolicy::constraint_policy -----------//

//---------- Begin of function FinancePolicy::reset_change -----------//
//!
void FinancePolicy::reset_change(bool resetAll) {
    //------- restore settings before Consider, Promise or Implement Now ---//

    if ( applied_flag > P_NONE ) {
	target_value = last_target_value ;
	upper_bound  = last_upper_bound;
	lower_bound  = last_lower_bound;
	import_weight = last_import_weight;
	required_flag = last_required_flag;
    }

    applied_flag = P_NONE;

    //-------- reset penality ---------//

    if ( resetAll ) {
	penalty_multiplier1 = 1;
	penalty_multiplier2 = 1;
    }
}

//---------- End of function FinancePolicy::reset_change -----------//

//---------- Begin of function FinancePolicy::set_policy_target -----------//
//!
void Finance::set_policy_target(bool isRevenue, char index) {
    FinancePolicy& policyP = isRevenue ? revenue_policy_array[index] : expense_policy_array[index];

    double targetArr[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT] = {
	1.5, 1.5, 0, ( policyP.upper_bound + policyP.lower_bound ) / 2,
	1.0, 1.0, 0, 0, 0,
    };

    policyP.target_value = targetArr[isRevenue?index:index+REVENUE_POLICY_COUNT];
}

//---------- End of function FinancePolicy::set_policy_target -----------//

//!
struct Revenue_and_Expense {
    double Surplus_Deficit;
    double Tuition_Rate;
    double Revenue_Array[REVENUE_ITEM_COUNT][4];    // max accessed [5][3] /[][1]=direct, [][2]=indirect, [][3]=total
    double Expense_Array[EXPENSE_ITEM_COUNT][4];    // max accessed [12][3] / [][0]=faculty [][1]=staff [][2]=other [][3]=total

    double calc_Surplus_Deficit() {
	int i;
	Revenue_Array[AC_NET_TUITION_INCOME][3] = 0.0f;
	for( i = 0; i < AC_NET_TUITION_INCOME; ++i )
	    Revenue_Array[AC_NET_TUITION_INCOME][3] += Revenue_Array[i][3];
	Surplus_Deficit = 0.0f;
	for( i = AC_NET_TUITION_INCOME; i < REVENUE_ITEM_COUNT; ++i )
	    Surplus_Deficit += Revenue_Array[i][3];
	for( i = 0; i < EXPENSE_ITEM_COUNT; ++i )
	    Surplus_Deficit -= Expense_Array[i][3];
	return Surplus_Deficit;
    }
};

static void Optimize_policy_ver2(
// output
    Revenue_and_Expense &Trial_Budget,
    Revenue_and_Expense &Final_Budget,
    // result of stage 1
    float Result_Vector_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT],
    float Result_Vector_S2[COST_RISE_POLICY_COUNT],   // result of stage 2
// input
    int endStage,                                     // finish in stage 1 or stage 2
    const Revenue_and_Expense &Base_Budget,           // Base_Budget is projected actual on 1st Aug
    const Revenue_and_Expense &Base_Actual,           // Base_Actual is actual at year end
    // PL_Policy_Array_S1 is slider in stage 1 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
    const float PL_Policy_Array_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT][4],
    // PL_Policy_Array_S2 is slider in stage 2 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
    const float PL_Policy_Array_S2[COST_RISE_POLICY_COUNT][4],

    double inflation, double Base_tuition_rate, double Base_enrollment, double Target_enrollment,
    double Actual_enrollment, double Base_endowmentMV, double Base_OpResBalance, float ST_int_rate_01Aug,
    float ST_int_rate_31Aug, double New_Debt_service );

static void Calc_Budget( Revenue_and_Expense &Budget_Obj, float *Policy_Vector_S1, float *Policy_Vector_S2,
			 const Revenue_and_Expense &Base_Budget,
			 double New_State_approp, double inflation, double Base_tuition_rate, double Target_enrollment,
			 double Base_endowmentMV, double Base_OpResBalance, double ST_int_rate_01Aug, double New_Debt_service );
static void Write_Budget( Revenue_and_Expense &, int, int );
static void Calc_Matrices_S1( double trial_Surplus_Deficit, ColumnVector &c, DiagonalMatrix &Q, Matrix &A, ColumnVector &b,
			      const float PL_Policy_Array_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT][4],
			      const Revenue_and_Expense &Base_Budget, double Base_tuition_rate, double Target_enrollment,
			      double Base_endowmentMV, double inflation );
static void quadratic_prog( ColumnVector &c, DiagonalMatrix &Q, Matrix &A, ColumnVector &b, ColumnVector &xNames, int );
static void Calc_Matrices_S2( double dollar_Constraint, ColumnVector &c, DiagonalMatrix &Q, Matrix &A, ColumnVector &b,
			      const Revenue_and_Expense &Base_Budget,
			      const float PL_Policy_Array_S2[COST_RISE_POLICY_COUNT][4] );
static void Calc_Actual( Revenue_and_Expense &Actual_Obj, const Revenue_and_Expense &Budget_Obj,
			 const Revenue_and_Expense &Base_Actual, double Actual_fac_salaries_31Aug, double ST_int_rate_31Aug, double Actual_enrollment,
			 double Base_OpResBalance );

static void Optimize_policy_ver2(
// output
    Revenue_and_Expense &Trial_Budget,
    Revenue_and_Expense &Final_Budget,
    // result of stage 1
    float Result_Vector_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT],
    float Result_Vector_S2[COST_RISE_POLICY_COUNT],   // result of stage 2

// input
    int endStage,                                     // finish in stage 1 or stage 2

    const Revenue_and_Expense &Base_Budget,           // Base_Budget is projected actual on 1st Aug
    const Revenue_and_Expense &Base_Actual,           // Base_Actual is actual at year end
    // PL_Policy_Array_S1 is slider in stage 1 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
    const float PL_Policy_Array_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT][4],
    // PL_Policy_Array_S2 is slider in stage 2 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
    const float PL_Policy_Array_S2[COST_RISE_POLICY_COUNT][4],

    double inflation,
    double Base_tuition_rate,                         // Tuition rate (eg, 4,000)
    double Base_enrollment,                           // Total number of students at the start of the year (eg 5,000)
    double Target_enrollment,                         // Sum of enrollment targets for the coming year + number of doctoral students at start of year (eg,5,200)
    double Actual_enrollment,                         // Actual number of student enrollments after the admissions process (eg.5,100)
    double Base_endowmentMV,                          // Endowment plus quasi endowment market value as of August 1; smoothed (eg, 220,000)
    double Base_OpResBalance,                         // Operating reserve balance as of August 1 (eg, 21,000)
    float ST_int_rate_01Aug,                          // Short-term interest rate as of August 1
    float ST_int_rate_31Aug,                          // short-term interest rate as of August 31
    double New_Debt_service                           // Debt service projected for next year (4,500)
    ) {
    int i;
    Revenue_and_Expense   S1_Output_Budget, New_Actual;

    double Real_Budget_Growth_Dollars;
    // state appropriation
    double New_State_approp = Base_Budget.Revenue_Array[AC_STATE_APPROPRIATION][3];
    double Actual_fac_salaries_31Aug = 0.0f;        // Total salaries of faculty sims, including new hires (assumes player changes hiring slider)
    // in spreadsheet, Bill use the formula :
    // (Sum(i) Base_Budget.Expense_Array[i][0])
    // * (1+ PL_Policy_Array_S1[REVENUE_POLICY_COUNT+PL_FACULTY_SALARY_INCREASES][0]
    //   + PL_Policy_Array_S2[PL_FACULTY_FTE][0] + 0.01 )
    for( i = 0; i < EXPENSE_ITEM_COUNT; ++i )
	Actual_fac_salaries_31Aug += Base_Budget.Expense_Array[i][0];
    Actual_fac_salaries_31Aug *= (1.0 + PL_Policy_Array_S1[REVENUE_POLICY_COUNT+PL_FACULTY_SALARY_INCREASES][0]
				  + PL_Policy_Array_S2[PL_FACULTY_FTE][0] + 0.01 );

    // Create temporary results vectors to get the stage 1 trial budget
    for( i = 0; i < REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i )
	// stage 1 allocations equal player input
	Result_Vector_S1[i] = PL_Policy_Array_S1[i][0];

    // Create temporary results vectors to get the stage 1 trial budget
    for( i = 0; i < COST_RISE_POLICY_COUNT; ++i )
	// stage 2 allocations all equal the planned real budget growth
	Result_Vector_S2[i] = PL_Policy_Array_S1[REVENUE_POLICY_COUNT+PL_BUDGET_ADJUSTMENT][0];

    // instantiate the trial budget class object; calculate and write the data
    // Set Trial_Budget = New Revenue_and_Expense; // ?
    Calc_Budget( Trial_Budget, Result_Vector_S1, Result_Vector_S2,
		 Base_Budget, New_State_approp, inflation, Base_tuition_rate, Target_enrollment,
		 Base_endowmentMV, Base_OpResBalance, ST_int_rate_01Aug, New_Debt_service );

    // DEBUG OUTPUT
    Write_Budget( Trial_Budget, 76, 3 ); {

	ColumnVector c(REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT);
	DiagonalMatrix Q(REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT);
	Matrix A((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+2,REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT);
	ColumnVector b((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+2);

	// row x column

	ColumnVector xNames(REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT);

	// Calculate S1 input matrices, do the S1 optimization, and update the S1 result vector
	Calc_Matrices_S1( Trial_Budget.Surplus_Deficit, c, Q, A, b,
			  PL_Policy_Array_S1, Base_Budget,
			  Base_tuition_rate, Target_enrollment, Base_endowmentMV, inflation );

	quadratic_prog( c, Q, A, b, xNames, 100);

	// Create temporary results vectors to get the Stage 1 output budget
	for( i = 0; i < REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i ) {
	    Result_Vector_S1[i] = (float)xNames(i+1);   // start from xNames(1)
	}

	// get the sum of last constraints
	float cSum = 0.0f;
	for( i = 1; i <= REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i ) {
	    cSum += xNames(i) * A((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+1, i );
	}
	// compare with b((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+2)

	// calc the value to minimize Qx^2 + cx
	float wSum = 0.0f;
	for( i = 1; i <= REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i ) {
	    wSum += (Q(i) * xNames(i) + c(i)) * xNames(i);
	}

	// use another method to do optimization

	double Alt_Result_Vector_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT];
	double lowLimit[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT];
	double highLimit[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT];
	double tolLimit[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT];
	double limitCoeff[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT];
	double limitSum;
	double cost1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT];
	double cost2[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT];
	for( i = 0; i < REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i ) {
	    lowLimit[i] = PL_Policy_Array_S1[i][1];
	    highLimit[i] = PL_Policy_Array_S1[i][2];
	    tolLimit[i] = 0.0005;                       // except PL_ENDOWMENT_SPENDING_RATE, PL_INDIRECT_COST_RATE
	    limitCoeff[i] = A((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+1, i+1 );
	    cost1[i] = c(i+1);
	    cost2[i] = Q(i+1);
	}
	limitSum = b((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+1);
	tolLimit[PL_ENDOWMENT_SPENDING_RATE] = 0.001;
	tolLimit[PL_INDIRECT_COST_RATE] = 0.005;
	// give bigger error tol on surplus/deficit
	tolLimit[REVENUE_POLICY_COUNT+PL_SURPLUS_DEFICIT] = 0.001;

	QuadProgramming quadProg;

	if( quadProg.sp_quadratic_programming( REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT,
					       Alt_Result_Vector_S1,
					       lowLimit, highLimit, tolLimit, limitCoeff, limitSum, cost1, cost2 ) ) {
	    // calc how good confront to constraint

	    double cSum2 = 0.0;
	    for( i = 0; i < REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i )
		cSum2 += limitCoeff[i] * Alt_Result_Vector_S1[i];
	    // now compare cSum2 with limitSum

	    double wSum2 = 0.0f;
	    for( i = 0; i < REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i ) {
		wSum2 += (cost2[i] * Alt_Result_Vector_S1[i] + cost1[i]) * Alt_Result_Vector_S1[i];
	    }

	    // now compare wSum2 with wSum
	    if( wSum2 < wSum ) {
		// better solution

		for( i = 0; i < REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i ) {
		    Result_Vector_S1[i] = Alt_Result_Vector_S1[i];
		}
	    }
	}
    }

    // DEBUG OUTPUT
    for( i = 0; i < COST_RISE_POLICY_COUNT; ++i ) {
	Result_Vector_S2[i] = Result_Vector_S1[REVENUE_POLICY_COUNT+PL_BUDGET_ADJUSTMENT];
    }

    // Set S1_Output_Budget = new Revenue_and_Expense
    Calc_Budget( S1_Output_Budget, Result_Vector_S1, Result_Vector_S2,
		 Base_Budget, New_State_approp, inflation, Base_tuition_rate, Target_enrollment,
		 Base_endowmentMV, Base_OpResBalance, ST_int_rate_01Aug, New_Debt_service );
    Write_Budget( S1_Output_Budget, 76, 8 );

  // find Base_Budget OperatingCost
    double Base_Budget_OperatingCost = 0.0f;
    for( i = 0; i < OPERATING_EXPENSE_ITEM_COUNT; ++i )
	Base_Budget_OperatingCost += Base_Budget.Expense_Array[i][3];

  // stage 1 finish

    if( endStage <= 1 )
	return;

  // Calculate S2 input matrices, do the S2 optimization, l and update the S2 result vector
    Real_Budget_Growth_Dollars = Result_Vector_S1[REVENUE_POLICY_COUNT+PL_BUDGET_ADJUSTMENT]
	* (Base_Budget_OperatingCost - Base_Budget.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][3]); {

	ColumnVector c(COST_RISE_POLICY_COUNT);
	DiagonalMatrix Q(COST_RISE_POLICY_COUNT);
	Matrix A(COST_RISE_POLICY_COUNT*2+2,COST_RISE_POLICY_COUNT);
	ColumnVector b(COST_RISE_POLICY_COUNT*2+2);

	ColumnVector xNames(COST_RISE_POLICY_COUNT);

	// Real_Budget_Growth_Dollars = (budget growth % from S1) * (total base operating expenditures - spons res)
	Calc_Matrices_S2( Real_Budget_Growth_Dollars, c, Q, A, b,
			  Base_Budget, PL_Policy_Array_S2 );

	quadratic_prog( c, Q, A, b, xNames, 100 );

	for( i = 0; i < COST_RISE_POLICY_COUNT; ++i )
	    Result_Vector_S2[i] = (float)xNames(i+1);   // start from xNames(1)

	// get the sum of last constraints
	float cSum = 0.0f;
	for( i = 1; i <= COST_RISE_POLICY_COUNT; ++i ) {
	    cSum += xNames(i) * A(COST_RISE_POLICY_COUNT*2+1, i );
	}
	// compare with b(COST_RISE_POLICY_COUNT*2+2)

	// calc the value to minimize Qx^2 + cx
	float wSum = 0.0f;
	for( i = 1; i <= COST_RISE_POLICY_COUNT; ++i ) {
	    wSum += (Q(i) * xNames(i) + c(i)) * xNames(i);
	}

	// use another method to do optimization

	double Alt_Result_Vector_S2[COST_RISE_POLICY_COUNT];
	double lowLimit[COST_RISE_POLICY_COUNT];
	double highLimit[COST_RISE_POLICY_COUNT];
	double tolLimit[COST_RISE_POLICY_COUNT];
	double limitCoeff[COST_RISE_POLICY_COUNT];
	double limitSum;
	double cost1[COST_RISE_POLICY_COUNT];
	double cost2[COST_RISE_POLICY_COUNT];
	for( i = 0; i < COST_RISE_POLICY_COUNT; ++i ) {
	    lowLimit[i] = PL_Policy_Array_S2[i][1];
	    highLimit[i] = PL_Policy_Array_S2[i][2];
	    tolLimit[i] = 0.0005;
	    limitCoeff[i] = A(COST_RISE_POLICY_COUNT*2+1, i+1 );
	    cost1[i] = c(i+1);
	    cost2[i] = Q(i+1);
	}
	limitSum = b(COST_RISE_POLICY_COUNT*2+1);

	QuadProgramming quadProg;

	if( quadProg.sp_quadratic_programming( COST_RISE_POLICY_COUNT,
					       Alt_Result_Vector_S2,
					       lowLimit, highLimit, tolLimit, limitCoeff, limitSum, cost1, cost2 ) ) {
	    // calc how good confront to constraint

	    double cSum2 = 0.0;
	    for( i = 0; i < COST_RISE_POLICY_COUNT; ++i )
		cSum2 += limitCoeff[i] * Alt_Result_Vector_S2[i];
	    // now compare cSum2 with limitSum

	    double wSum2 = 0.0f;
	    for( i = 0; i < COST_RISE_POLICY_COUNT; ++i ) {
		wSum2 += (cost2[i] * Alt_Result_Vector_S2[i] + cost1[i]) * Alt_Result_Vector_S2[i];
	    }

	    // now compare cSum2 with cSum
	    if( fabs(cSum2 - limitSum) < fabs(cSum - limitSum) ) {
		// better solution

		for( i = 0; i < COST_RISE_POLICY_COUNT; ++i ) {
		    Result_Vector_S2[i] = Alt_Result_Vector_S2[i];
		}
	    }
	}

    }

    // Set Final_Budget = new Revenue_and_Expense
    Calc_Budget( Final_Budget, Result_Vector_S1, Result_Vector_S2,
		 Base_Budget, New_State_approp, inflation, Base_tuition_rate, Target_enrollment,
		 Base_endowmentMV, Base_OpResBalance, ST_int_rate_01Aug, New_Debt_service );

    // DEBUG OUTPUT
    Write_Budget( Final_Budget, 107, 3 );

    // Set New_Actual = New Revenue_and_Expense

    Calc_Actual( New_Actual, Final_Budget,
		 Base_Actual, Actual_fac_salaries_31Aug, ST_int_rate_31Aug,
		 Actual_enrollment, Base_OpResBalance );

    // DEBUG OUTPUT
    Write_Budget( New_Actual, 107, 8 );
}

static void Calc_Actual( Revenue_and_Expense &Actual_Obj, const Revenue_and_Expense &Budget_Obj,
			 const Revenue_and_Expense &Base_Actual,
			 double Actual_fac_salaries_31Aug,
			 double ST_int_rate_31Aug,
			 double Actual_enrollment,
			 double Base_OpResBalance ) {
    int i, j;

  // beware some use Budget_Obj, some use Base_Actual
                                                  // Tuition revenue
    Actual_Obj.Revenue_Array[AC_GROSS_TUITION_INCOME][3] = Budget_Obj.Tuition_Rate * Actual_enrollment / 1000.0;
    // Financial Aid
    Actual_Obj.Revenue_Array[AC_FINANCIAL_AID][3] = Budget_Obj.Revenue_Array[AC_FINANCIAL_AID][3];
    Actual_Obj.Revenue_Array[AC_NET_TUITION_INCOME][3] = 0.0f;
    for( i = AC_GROSS_TUITION_INCOME; i < AC_NET_TUITION_INCOME; ++i )
	Actual_Obj.Revenue_Array[AC_NET_TUITION_INCOME][3] += Actual_Obj.Revenue_Array[i][3];

                                                  // Direct sponsored research
    Actual_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][1] = Base_Actual.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][1];
    Actual_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][2] = Base_Actual.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][1]
	// Indirect (=0 if denominator = 0)
	* math.safe_divide(Budget_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][2],Budget_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][1]);
    Actual_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][3] = Actual_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][1]
	+ Actual_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][2];

                                                  // Gifts
    Actual_Obj.Revenue_Array[AC_GIFTS][3] = Base_Actual.Revenue_Array[AC_GIFTS][3];
    // Endowment spending
    Actual_Obj.Revenue_Array[AC_ENDOWMENT_SPENDING][3] = Budget_Obj.Revenue_Array[AC_ENDOWMENT_SPENDING][3];
    // Athletics
    Actual_Obj.Revenue_Array[AC_INTERCOLLEGIATE_ATHLETICS][3] = Base_Actual.Revenue_Array[AC_INTERCOLLEGIATE_ATHLETICS][3];
    // Other income
    Actual_Obj.Revenue_Array[AC_OTHER_OPERATING_INCOME][3] = Base_Actual.Revenue_Array[AC_OTHER_OPERATING_INCOME][3];
    // Int on oper res
    Actual_Obj.Revenue_Array[AC_INTEREST_EARNED_OR_PAID][3] = ST_int_rate_31Aug * (Base_OpResBalance + Base_Actual.Surplus_Deficit);
    // State appropriation
    Actual_Obj.Revenue_Array[AC_STATE_APPROPRIATION][3] = Budget_Obj.Revenue_Array[AC_STATE_APPROPRIATION][3];

    // Most actuals equal forecast
    for( i = 0; i < OPERATING_EXPENSE_ITEM_COUNT; ++i ) {
	for( j = 0; j <= 2; ++j ) {
	    Actual_Obj.Expense_Array[i][j] = Budget_Obj.Expense_Array[i][j];
	}
    }

    // Now process the exceptions and "below the line" items
    // Faculy salaries
    Actual_Obj.Expense_Array[AC_ACADEMIC_DEPARTMENTS][0] = Actual_fac_salaries_31Aug;
    for( j = 0; j <= 2; ++j )
	// Sponsored research
	Actual_Obj.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][j] = Base_Actual.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][j];

    // although in VBA from Bill, is 11 and 12
    // change the index to 11-12, as Bill may think index 10 is used to operating expense total
    for( j = 0; j <= 2; ++j )
	// Debt service
	Actual_Obj.Expense_Array[AC_SERVICE_ON_GENERAL_PLANT_DEBT][j] = Budget_Obj.Expense_Array[AC_SERVICE_ON_GENERAL_PLANT_DEBT][j];
    for( j = 0; j <= 2; ++j )
	// Transfer to capital reserve
	Actual_Obj.Expense_Array[AC_TRANSFER_TO_CAPITAL_RESERVE][j] = Budget_Obj.Expense_Array[AC_TRANSFER_TO_CAPITAL_RESERVE][j];

    // calc faulty+staff+other to total of each expense item
    for( i = 0; i < EXPENSE_ITEM_COUNT; ++i ) {
	double expenseItemTotal = 0.0f;
	for( j = 0; j <= 2; ++j ) {
	    expenseItemTotal += Actual_Obj.Expense_Array[i][j];
	}
	Actual_Obj.Expense_Array[i][3] = expenseItemTotal;
    }
    Actual_Obj.calc_Surplus_Deficit();
}

static void Calc_Budget( Revenue_and_Expense &Budget_Obj, float *Policy_Vector_S1, float *Policy_Vector_S2,
			 const Revenue_and_Expense &Base_Budget,
			 double New_State_approp, double inflation, double Base_tuition_rate, double Target_enrollment,
			 double Base_endowmentMV, double Base_OpResBalance, double ST_int_rate_01Aug, double New_Debt_service ) {
    int i;
    // Forecasts revenue and expense items for use in budgeting

    // Revenues
    // Tuition revenue
    Budget_Obj.Tuition_Rate = (1.0 + inflation + Policy_Vector_S1[PL_TUITION_RATE_GROWTH]) * Base_tuition_rate;
    Budget_Obj.Revenue_Array[AC_GROSS_TUITION_INCOME][3] = (Budget_Obj.Tuition_Rate / 1000.0) * Target_enrollment;

    // Financial Aid
    Budget_Obj.Revenue_Array[AC_FINANCIAL_AID][3] = (1.0 + inflation + Policy_Vector_S1[PL_FINANCIAL_AID]) * Base_Budget.Revenue_Array[AC_FINANCIAL_AID][3];

    // calc net tuition income
    Budget_Obj.Revenue_Array[AC_NET_TUITION_INCOME][3] = 0.0f;
    for( i = AC_GROSS_TUITION_INCOME; i < AC_NET_TUITION_INCOME; ++i ) {
	Budget_Obj.Revenue_Array[AC_NET_TUITION_INCOME][3] += Budget_Obj.Revenue_Array[i][3];
    }

    // Sponsored research is calculated later

  // Gifts
    Budget_Obj.Revenue_Array[AC_GIFTS][3] = (1.0 + inflation) * Base_Budget.Revenue_Array[AC_GIFTS][3];

    // Endowment spending (row 6)
    Budget_Obj.Revenue_Array[AC_ENDOWMENT_SPENDING][3] = Policy_Vector_S1[PL_ENDOWMENT_SPENDING_RATE] * Base_endowmentMV;

    // Athletics
    Budget_Obj.Revenue_Array[AC_INTERCOLLEGIATE_ATHLETICS][3] = (1.0 + inflation) * Base_Budget.Revenue_Array[AC_INTERCOLLEGIATE_ATHLETICS][3];

    // Other operating income
    Budget_Obj.Revenue_Array[AC_OTHER_OPERATING_INCOME][3] = (1.0 + inflation) * Base_Budget.Revenue_Array[AC_OTHER_OPERATING_INCOME][3];

    // Interest on the operating reserve (adjust for current year surplus/deficit)
    Budget_Obj.Revenue_Array[AC_INTEREST_EARNED_OR_PAID][3] = (Base_OpResBalance + Base_Budget.Surplus_Deficit) * ST_int_rate_01Aug;

    // state appropriation
    Budget_Obj.Revenue_Array[AC_STATE_APPROPRIATION][3] = New_State_approp;

    // Expenses

  // Academic departments
    Budget_Obj.Expense_Array[AC_ACADEMIC_DEPARTMENTS][0] = Base_Budget.Expense_Array[AC_ACADEMIC_DEPARTMENTS][0]
	* (1.0 + Policy_Vector_S2[PL_FACULTY_FTE] + inflation
	   // Faculty salaries
	   + Policy_Vector_S1[REVENUE_POLICY_COUNT+PL_FACULTY_SALARY_INCREASES] );
    Budget_Obj.Expense_Array[AC_ACADEMIC_DEPARTMENTS][1] = Base_Budget.Expense_Array[AC_ACADEMIC_DEPARTMENTS][1]
	* (1.0 + Policy_Vector_S2[PL_NON_FACULTY_DEPARTMENTAL_EXPENSE] + inflation
	   // Staff salaries
	   + Policy_Vector_S1[REVENUE_POLICY_COUNT+PL_STAFF_SALARY_INCREASES]);
    Budget_Obj.Expense_Array[AC_ACADEMIC_DEPARTMENTS][2] = Base_Budget.Expense_Array[AC_ACADEMIC_DEPARTMENTS][2]
	// other salaries
	* (1.0 + Policy_Vector_S2[PL_NON_FACULTY_DEPARTMENTAL_EXPENSE] + inflation);
    Budget_Obj.Expense_Array[AC_ACADEMIC_DEPARTMENTS][3] = Budget_Obj.Expense_Array[AC_ACADEMIC_DEPARTMENTS][0]
	+ Budget_Obj.Expense_Array[AC_ACADEMIC_DEPARTMENTS][1]
	+ Budget_Obj.Expense_Array[AC_ACADEMIC_DEPARTMENTS][2];

  // Sponsered research (equal last year except for salary increases)
    Budget_Obj.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][0] = Base_Budget.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][0]
	// Faculty salaries
	* (1.0 + inflation + Policy_Vector_S1[REVENUE_POLICY_COUNT+PL_FACULTY_SALARY_INCREASES]);
    Budget_Obj.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][1] = Base_Budget.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][1]
	// Staff salaries
	* (1.0 + inflation + Policy_Vector_S1[REVENUE_POLICY_COUNT+PL_STAFF_SALARY_INCREASES]);
    Budget_Obj.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][2] = Base_Budget.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][2]
	* (1.0 + inflation);                          // other expense
    Budget_Obj.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][3] = Budget_Obj.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][0]
	+ Budget_Obj.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][1]
	+ Budget_Obj.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][2];

  // Support Service
    for( i = 1+AC_SPONSORED_RESEARCH_EXPENSE; i < OPERATING_EXPENSE_ITEM_COUNT; ++i ) {
	Budget_Obj.Expense_Array[i][0] = Base_Budget.Expense_Array[i][0]
	    * (1.0 + Policy_Vector_S2[i] + inflation
	       // Faculty salaries
	       + Policy_Vector_S1[REVENUE_POLICY_COUNT+PL_FACULTY_SALARY_INCREASES] );
	Budget_Obj.Expense_Array[i][1] = Base_Budget.Expense_Array[i][1]
	    * (1.0 + Policy_Vector_S2[i] + inflation
	       // Staff salaries
	       + Policy_Vector_S1[REVENUE_POLICY_COUNT+PL_STAFF_SALARY_INCREASES]);
	Budget_Obj.Expense_Array[i][2] = Base_Budget.Expense_Array[i][2]
	    * (1.0 + Policy_Vector_S2[i] + inflation);  // other salaries
	Budget_Obj.Expense_Array[i][3] = Budget_Obj.Expense_Array[i][0]
	    + Budget_Obj.Expense_Array[i][1] + Budget_Obj.Expense_Array[i][2];
    }

    // Service on general plant debt and Transfer to capital reserve

    Budget_Obj.Expense_Array[AC_SERVICE_ON_GENERAL_PLANT_DEBT][0] = 0.0f;
    Budget_Obj.Expense_Array[AC_SERVICE_ON_GENERAL_PLANT_DEBT][1] = 0.0f;
    Budget_Obj.Expense_Array[AC_SERVICE_ON_GENERAL_PLANT_DEBT][2] = New_Debt_service;
    Budget_Obj.Expense_Array[AC_SERVICE_ON_GENERAL_PLANT_DEBT][3] = Budget_Obj.Expense_Array[AC_SERVICE_ON_GENERAL_PLANT_DEBT][2];

    Budget_Obj.Expense_Array[AC_TRANSFER_TO_CAPITAL_RESERVE][0] = 0.0f;
    Budget_Obj.Expense_Array[AC_TRANSFER_TO_CAPITAL_RESERVE][1] = 0.0f;
    Budget_Obj.Expense_Array[AC_TRANSFER_TO_CAPITAL_RESERVE][2] = Base_Budget.Expense_Array[AC_TRANSFER_TO_CAPITAL_RESERVE][2]
	// Other Expense
	* (1.0 + inflation + Policy_Vector_S1[REVENUE_POLICY_COUNT+PL_REAL_TRANSFER_TO_PLANT_GROWTH]);
    Budget_Obj.Expense_Array[AC_TRANSFER_TO_CAPITAL_RESERVE][3] = Budget_Obj.Expense_Array[AC_TRANSFER_TO_CAPITAL_RESERVE][2];

  // Sponsored research REVENUE (direct revenue = sponsored research expense;
  //    indirect revenue = smoothed adjustment based on the new indirect cost rate)

    Budget_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][1] = Budget_Obj.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][3];
    Budget_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][2] = Base_Budget.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][2]
	+ 0.5 * (Budget_Obj.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][3]
		 * Policy_Vector_S1[PL_INDIRECT_COST_RATE] - Base_Budget.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][2]);
    Budget_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][3] = Budget_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][1]
	+ Budget_Obj.Revenue_Array[AC_SPONSORED_RESEARCH_REVENUE][2];

  // calc surplus deficit

    Budget_Obj.calc_Surplus_Deficit();
}

// this function original for Bill to fill fields on spreadsheet
static void Write_Budget( Revenue_and_Expense &Trial_Budget, int, int ) {
#ifdef DEBUG
    DEBUG_LOG("Revenue :");

    String s;
    for( int i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
	s  = Trial_Budget.Revenue_Array[i][3];
	if( i == AC_SPONSORED_RESEARCH_REVENUE ) {
	    s += "   {";
	    s += Trial_Budget.Revenue_Array[i][1];
	    s += " + ";
	    s += Trial_Budget.Revenue_Array[i][2];
	    s += "}";
	}
	DEBUG_LOG( (char *)s );
    }

    DEBUG_LOG("Expense :");
    for( int i = 0; i < EXPENSE_ITEM_COUNT; ++i ) {
	s  = Trial_Budget.Expense_Array[i][3];
	s += "  {";
	s += Trial_Budget.Expense_Array[i][0];
	s += " + ";
	s += Trial_Budget.Expense_Array[i][1];
	s += " + ";
	s += Trial_Budget.Expense_Array[i][2];
	s += "}";
	DEBUG_LOG( (char *)s );
    }

    s  = "Surplus = ";
    s += Trial_Budget.Surplus_Deficit;
    DEBUG_LOG( (char *)s );
#endif
}

// dimension of c is [REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT]
// dimension of Q is [REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT][REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT]
// dimension of A is [(REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+2][REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT]
// dimension of b is [(REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+2]

static void Calc_Matrices_S1( double trial_Surplus_Deficit, ColumnVector &c, DiagonalMatrix &Q, Matrix &A, ColumnVector &b,
			      const float PL_Policy_Array_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT][4],
			      const Revenue_and_Expense &Base_Budget,
			      double Base_tuition_rate,
			      double Target_enrollment,
			      double Base_endowmentMV,
			      double inflation
    ) {
    // Calculate the matrices for stage 1 optimization input
    // minimize c'x + x'Qx/2 s.t. Ax >= b ( and x >= 0 )

    double remainder, coefficients[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT];
    double Base_Budget_OperatingCost[4];
    double Base_Budget_TotalUseOfFund[4];

    int i,j;
    for( j = 0; j < 4; ++j ) {
	Base_Budget_OperatingCost[j] = 0.0f;
	for(i=0; i < OPERATING_EXPENSE_ITEM_COUNT; ++i )
	    Base_Budget_OperatingCost[j] += Base_Budget.Expense_Array[i][j];
    }
    for( j = 0; j < 4; ++j ) {
	Base_Budget_TotalUseOfFund[j] = 0.0f;
	for(i=0; i < EXPENSE_ITEM_COUNT; ++i )
	    Base_Budget_TotalUseOfFund[j] += Base_Budget.Expense_Array[i][j];
    }

    // initialize A to zero
    A = 0.0;

    // Contraints
    // Tuition rate
    coefficients[PL_TUITION_RATE_GROWTH] = Base_tuition_rate * Target_enrollment / 1000.0;
    // Financial aid
    coefficients[PL_FINANCIAL_AID] = Base_Budget.Revenue_Array[AC_FINANCIAL_AID][3];
    // Endowment spending
    coefficients[PL_ENDOWMENT_SPENDING_RATE] = Base_endowmentMV;
    // indirect cost rate
    coefficients[PL_INDIRECT_COST_RATE] = Base_Budget.Expense_Array[AC_SPONSORED_RESEARCH_EXPENSE][3] * (1.015 + inflation) / 2.0f;

  // Approximates the salary increases to maintain linearity and allows for the smooth
                                                  // faculty salaries
    coefficients[REVENUE_POLICY_COUNT+PL_FACULTY_SALARY_INCREASES] = -Base_Budget_OperatingCost[0];
    // staff salaries
    coefficients[REVENUE_POLICY_COUNT+PL_STAFF_SALARY_INCREASES] = -Base_Budget_OperatingCost[1];
    // real budget growth
    coefficients[REVENUE_POLICY_COUNT+PL_BUDGET_ADJUSTMENT] = -Base_Budget_OperatingCost[3];
    // Transfer to capital reserve
    coefficients[REVENUE_POLICY_COUNT+PL_REAL_TRANSFER_TO_PLANT_GROWTH] = -Base_Budget.Expense_Array[AC_TRANSFER_TO_CAPITAL_RESERVE][3];
    // surplus/deficit
    coefficients[REVENUE_POLICY_COUNT+PL_SURPLUS_DEFICIT] = -Base_Budget_TotalUseOfFund[3] * (1.0 + inflation);

    // object function
    static double weightScale = 100000.0;
    for(i = 0; i < REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i ) {
	// original design
	//c(i+1) = -PL_Policy_Array_S1[i][0] * PL_Policy_Array_S1[i][3]; // -target * weight
	//Q(i+1) = PL_Policy_Array_S1[i][3] / 2.0; // weight / 2

	// change 1
	double adjWeight = 1.0;
	if( coefficients[i] != 0.0 ) {                // avoid adjWeight to be zero, (eg. sponsored research is zero, then indirect cost rate in unconstrainted. If adjWeight is zero, optimization can't get the target value (c/-2Q))
	    // order 1 : default
	    adjWeight = PL_Policy_Array_S1[i][3] * fabs(coefficients[i])/weightScale;
	    // order 0
	    //adjWeight = PL_Policy_Array_S1[i][3];
	    // order -1
	    //adjWeight = fabs(coefficients[i]) < 1.0 ? PL_Policy_Array_S1[i][3]
	    //	: PL_Policy_Array_S1[i][3] * weightScale / fabs(coefficients[i]);
	    // order 2
	    //adjWeight = PL_Policy_Array_S1[i][3] * coefficients[i] * coefficients[i] / weightScale;
	}

	// -target * weight
	c(i+1) = -PL_Policy_Array_S1[i][0] * adjWeight;
	Q(i+1) = adjWeight / 2.0;                     // weight / 2
    }

    remainder = trial_Surplus_Deficit;

    for( i = 0; i < REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i ) {
	if( i != REVENUE_POLICY_COUNT+PL_SURPLUS_DEFICIT )
	    remainder -= coefficients[i] * PL_Policy_Array_S1[i][0];
    }

    for( i = 0; i < REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT; ++i ) {
	A(i+1,i+1) = 1.0;
	b(i+1) = PL_Policy_Array_S1[i][1];            // lower bound
	A(REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT+i+1, i+1) = -1.0f;
	// upper bound
	b(REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT+i+1) = -PL_Policy_Array_S1[i][2];

	A((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+1, i+1) = -coefficients[i];
	// positive budget constraint
	b((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+1) = remainder;
	A((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+2, i+1) = coefficients[i];
	// negative budget constraint
	b((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+2) = -remainder;
    }

    // reverse the signs of the surplus/deficit terms
    // A((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2, REVENUE_POLICY_COUNT+PL_SURPLUS_DEFICIT)
    // = -A((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2, REVENUE_POLICY_COUNT+PL_SURPLUS_DEFICIT);
    // A((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+1, REVENUE_POLICY_COUNT+PL_SURPLUS_DEFICIT)
    // = -A((REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT)*2+1, REVENUE_POLICY_COUNT+PL_SURPLUS_DEFICIT);
}

//	dimension of c is [COST_RISE_POLICY_COUNT]
//	dimension of Q is [COST_RISE_POLICY_COUNT][COST_RISE_POLICY_COUNT]
//	dimension of A is [COST_RISE_POLICY_COUNT*2+2][COST_RISE_POLICY_COUNT]
// dimension of b is [COST_RISE_POLICY_COUNT*2+2]

static void Calc_Matrices_S2( double dollar_Constraint, ColumnVector &c, DiagonalMatrix &Q, Matrix &A, ColumnVector &b,
			      const Revenue_and_Expense &Base_Budget,
			      const float PL_Policy_Array_S2[COST_RISE_POLICY_COUNT][4]
    ) {
    // calculate the matrices for stage 2 optimization input
    // minimize c'x + x'Qx/2 s.t. Ax >= b ( and x >= 0 )
    // NOTE :  the second-order effects between real budget growth and salary increments
    // and faculty research offsets are ignored to make the results easy to track.

    double coefficients[COST_RISE_POLICY_COUNT];

    int i;

    // initialize A to zero
    A = 0.0;

    // constraint

    for( i = 0; i < COST_RISE_POLICY_COUNT; ++i ) {
	if( i == PL_FACULTY_FTE ) {
	    // faculty budget
	    coefficients[i] = Base_Budget.Expense_Array[AC_ACADEMIC_DEPARTMENTS][0];
	}
	else if( i == PL_NON_FACULTY_DEPARTMENTAL_EXPENSE ) {
	    coefficients[i] = Base_Budget.Expense_Array[AC_ACADEMIC_DEPARTMENTS][1]
		// nonfaculty dept'l expense
		+ Base_Budget.Expense_Array[AC_ACADEMIC_DEPARTMENTS][2];
	}
	else {
	    err_when( i >= OPERATING_EXPENSE_ITEM_COUNT );
	    // all other expenditure line items
	    coefficients[i] = Base_Budget.Expense_Array[i][3];
	}
    }

    // objective function
    for( i = 0; i < COST_RISE_POLICY_COUNT; ++i ) {
	// original design
	//c(i+1) = -PL_Policy_Array_S2[i][0] * PL_Policy_Array_S2[i][3]; // -target * weight
	//Q(i+1) = PL_Policy_Array_S2[i][3] / 2.0; // weight / 2

	// change 1
	double adjWeight = PL_Policy_Array_S2[i][3] * fabs(coefficients[i])/100000.0;
	// -target * weight
	c(i+1) = -PL_Policy_Array_S2[i][0] * adjWeight;
	Q(i+1) = adjWeight / 2.0;                     // weight / 2
    }

    for( i = 0; i < COST_RISE_POLICY_COUNT; ++i ) {
	A(i+1,i+1) = 1.0;
	b(i+1) = PL_Policy_Array_S2[i][1];            // lower bound
	A(COST_RISE_POLICY_COUNT+i+1,i+1) = -1.0f;
	// upper bound
	b(COST_RISE_POLICY_COUNT+i+1) = -PL_Policy_Array_S2[i][2];

	A(COST_RISE_POLICY_COUNT*2+1,i+1) = coefficients[i];
	// Positive budget constraint
	b(COST_RISE_POLICY_COUNT*2+1) = dollar_Constraint;
	A(COST_RISE_POLICY_COUNT*2+2,i+1) = -coefficients[i];
	// Negative budget constraint
	b(COST_RISE_POLICY_COUNT*2+2) = -dollar_Constraint;
    }
}

static void quadratic_prog( ColumnVector &c, DiagonalMatrix &Q, Matrix &A, ColumnVector &b, ColumnVector &xNames, int loopCountMultiplier ) {
    // use another way to do programming
    LinearAlgebra::quadratic_prog(c,Q,A,b,xNames, loopCountMultiplier);
    // may return false
}

// get input for Optimize_policy_ver2
static void input_Optimize_policy_ver2(
    Revenue_and_Expense& Base_Budget,                 // Base_Budget is projected actual on 1st Aug
    Revenue_and_Expense& Base_Actual,                 // Base_Actual is actual at year end
    // slider in stage 1 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
    float PL_Policy_Array_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT][4],
    // slider in stage 2 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
    float PL_Policy_Array_S2[COST_RISE_POLICY_COUNT][4],
    double& inflation, double& Base_tuition_rate,
    double& Base_enrollment, double& Target_enrollment, double& Actual_enrollment,
    double& Base_endowmentMV,double& Base_OpResBalance,
    float& ST_int_rate_01Aug, float& ST_int_rate_31Aug,
    double& New_Debt_service ) {
    // allocation stage 1
    // set input parameter

    int i;

    if( info.prerun_year && info.game_year == 1 ) { // initialization
	for( i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
	    Base_Budget.Revenue_Array[i][0] = 0.0;
	    Base_Budget.Revenue_Array[i][1] = finance.revenue_array[i].direct;
	    Base_Budget.Revenue_Array[i][2] = finance.revenue_array[i].indirect;
	    Base_Budget.Revenue_Array[i][3] = finance.revenue_array[i].total;
	}
	for( i = 0; i < EXPENSE_ITEM_COUNT; ++i ) {
	    Base_Budget.Expense_Array[i][0] = finance.expense_array[i].faculty;
	    Base_Budget.Expense_Array[i][1] = finance.expense_array[i].staff;
	    Base_Budget.Expense_Array[i][2] = finance.expense_array[i].other;
	    Base_Budget.Expense_Array[i][3] = finance.expense_array[i].total;
	}
    }
    else {
	for( i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
	    Base_Budget.Revenue_Array[i][0] = 0.0;
	    Base_Budget.Revenue_Array[i][1] = finance.projected_revenue_array[i].month_start.direct;
	    Base_Budget.Revenue_Array[i][2] = finance.projected_revenue_array[i].month_start.indirect;
	    Base_Budget.Revenue_Array[i][3] = finance.projected_revenue_array[i].month_start.total;
	}
	for( i = 0; i < EXPENSE_ITEM_COUNT; ++i ) {
	    Base_Budget.Expense_Array[i][0] = finance.projected_expense_array[i].month_start.faculty;
	    Base_Budget.Expense_Array[i][1] = finance.projected_expense_array[i].month_start.staff;
	    Base_Budget.Expense_Array[i][2] = finance.projected_expense_array[i].month_start.other;
	    Base_Budget.Expense_Array[i][3] = finance.projected_expense_array[i].month_start.total;
	}
    }

    if( info.prerun_year && info.game_year == 1 ) { // initialization
	for( i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
	    Base_Actual.Revenue_Array[i][0] = 0.0;
	    Base_Actual.Revenue_Array[i][1] = finance.revenue_array[i].direct;
	    Base_Actual.Revenue_Array[i][2] = finance.revenue_array[i].indirect;
	    Base_Actual.Revenue_Array[i][3] = finance.revenue_array[i].total;
	}
	for( i = 0; i < EXPENSE_ITEM_COUNT; ++i ) {
	    Base_Actual.Expense_Array[i][0] = finance.expense_array[i].faculty;
	    Base_Actual.Expense_Array[i][1] = finance.expense_array[i].staff;
	    Base_Actual.Expense_Array[i][2] = finance.expense_array[i].other;
	    Base_Actual.Expense_Array[i][3] = finance.expense_array[i].total;
	}
    }
    else {
	for( i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
	    Base_Actual.Revenue_Array[i][0] = 0.0;
	    Base_Actual.Revenue_Array[i][1] = finance.projected_revenue_array[i].this_year.direct;
	    Base_Actual.Revenue_Array[i][2] = finance.projected_revenue_array[i].this_year.indirect;
	    Base_Actual.Revenue_Array[i][3] = finance.projected_revenue_array[i].this_year.total;
	}
	for( i = 0; i < EXPENSE_ITEM_COUNT; ++i ) {
	    Base_Actual.Expense_Array[i][0] = finance.projected_expense_array[i].this_year.faculty;
	    Base_Actual.Expense_Array[i][1] = finance.projected_expense_array[i].this_year.staff;
	    Base_Actual.Expense_Array[i][2] = finance.projected_expense_array[i].this_year.other;
	    Base_Actual.Expense_Array[i][3] = finance.projected_expense_array[i].this_year.total;
	}
    }

    for( i = 0; i < REVENUE_POLICY_COUNT; ++i ) {
	// [0] is target
	PL_Policy_Array_S1[i][0] = (float)finance.revenue_policy_array[i].target_value / 100.0f;
	// [1] is ower bound
	PL_Policy_Array_S1[i][1] = (float)finance.revenue_policy_array[i].lower_bound / 100.0f;
	// [2] is upper bound
	PL_Policy_Array_S1[i][2] = (float)finance.revenue_policy_array[i].upper_bound / 100.0f;
	// [3] is preference (weight)
	PL_Policy_Array_S1[i][3] = (float)finance.revenue_policy_array[i].import_weight;
	if( finance.revenue_policy_array[i].required_flag ) {
	    // [0] is target
	    PL_Policy_Array_S1[i][1] = PL_Policy_Array_S1[i][2] = (float)finance.revenue_policy_array[i].target_value / 100.0f;
	    PL_Policy_Array_S1[i][3] = OPT_HIGH_WEIGHT * 10;
	}
    }
    for( i = 0; i < EXPENSE_POLICY_COUNT; ++i ) {
	// [0] is target
	PL_Policy_Array_S1[REVENUE_POLICY_COUNT+i][0] = (float)finance.expense_policy_array[i].target_value / 100.0f;
	// [1] is ower bound
	PL_Policy_Array_S1[REVENUE_POLICY_COUNT+i][1] = (float)finance.expense_policy_array[i].lower_bound / 100.0f;
	// [2] is upper bound
	PL_Policy_Array_S1[REVENUE_POLICY_COUNT+i][2] = (float)finance.expense_policy_array[i].upper_bound / 100.0f;
	// [3] is preference (weight)
	PL_Policy_Array_S1[REVENUE_POLICY_COUNT+i][3] = (float)finance.expense_policy_array[i].import_weight;
	if( finance.expense_policy_array[i].required_flag ) {
	    // [0] is target
	    PL_Policy_Array_S1[REVENUE_POLICY_COUNT+i][1] = PL_Policy_Array_S1[REVENUE_POLICY_COUNT+i][2] = (float)finance.expense_policy_array[i].target_value / 100.0f;
	    PL_Policy_Array_S1[REVENUE_POLICY_COUNT+i][3] = OPT_HIGH_WEIGHT * 10;
	}
    }

    for( i = 0; i < COST_RISE_POLICY_COUNT; ++i ) {
	PL_Policy_Array_S2[i][0] = (float)finance.cost_rise_policy_array[i].target_value / 100.0f;
	PL_Policy_Array_S2[i][1] = (float)finance.cost_rise_policy_array[i].lower_bound / 100.0f;
	PL_Policy_Array_S2[i][2] = (float)finance.cost_rise_policy_array[i].upper_bound / 100.0f;
	PL_Policy_Array_S2[i][3] = (float)finance.cost_rise_policy_array[i].import_weight;
	if( finance.cost_rise_policy_array[i].required_flag ) {
	    PL_Policy_Array_S2[i][1] = PL_Policy_Array_S2[i][2] = (float)finance.cost_rise_policy_array[i].target_value / 100.0f;
	    PL_Policy_Array_S2[i][3] = OPT_HIGH_WEIGHT * 10;
	}
    }

    inflation = finance.inflation_rate/100.0;
    Base_tuition_rate = finance.tuition_rate;
    // initialization
    Base_enrollment = (info.prerun_year && info.game_year == 1)
	? enroll_res.total_student_count
	: department_array.student_level_history[MAX_STUDENT_LEVEL_TOTAL][HISTORY_YEAR_COUNT-1];
    Target_enrollment = enroll_res.target_enrollment[UG_TRADITION] + enroll_res.target_enrollment[UG_NONTRADITION]
	+ enroll_res.target_enrollment[MASTER] + department_array.student_level_history[DOCTOR][HISTORY_YEAR_COUNT-1];
    Actual_enrollment = Target_enrollment;          // temp, don't know because not generate next year enrollment yet

    // Base_endowmentMV = investment_office.smoothed_endowment_value;
    // Base_endowmentMV = finance.this_year.asset_array[AC_ENDOWMENT] + finance.this_year.asset_array[AC_QUASI];
    Base_endowmentMV = investment_office.smoothed_endowment_value + investment_office.smoothed_quasi_endowment_value;

    Base_OpResBalance = finance.this_year.asset_array[AC_OPERATING_RESERVE];
    ST_int_rate_01Aug = (float)finance.short_term_debt_interest_history[THIS_MONTH-1] / 100.0f;
    ST_int_rate_31Aug = (float)finance.short_term_debt_interest_history[THIS_MONTH] / 100.0f;
    //initialization
    New_Debt_service = (info.prerun_year && info.game_year == 1)
	? finance.expense_array[AC_SERVICE_ON_GENERAL_PLANT_DEBT].total
	: finance.projected_expense_array[AC_SERVICE_ON_GENERAL_PLANT_DEBT].next_year.total;
}

//---------- Begin of function Finance::init -----------//
//!
void Finance::init() {
    memset( this, 0, sizeof(Finance) );

    fiscal_year_start_month = 9;
    fiscal_year_end_month = 8;

    //---- fully exogenous factors -----//

    inflation_rate          = 0.0;                  // 0222	3.0;				// C72
    short_term_debt_interest  = 3.0;                // 0406	3.0;
    long_term_debt_interest   = 6.0;                // 0406	6.0;
    debt_fraction_repaid_annually = 4.0;
    family_income_growth        = 0.0;
    construction_cost_growth    = 3.0;

    building_life_for_calc_depreciation = 50;       // G80

    //--- exogenous, but may be influenced by the player ---//

    investment_office.expected_real_annual_return_rate = 7.0f;  endowment_return = 7.0;
    sponsored_research_growth   = 0.0;
    gifts_growth            = 0.0;
    athletics_revenue_growth    = 0.0;              // G75
    bank_debt_interest        = 5.0;
    other_expense_growth        = 0.5;
    other_income_growth       = 0.0;

    //--------- time variation variable 1017 --------//
    //------- initialization ("design notes 0830 section8.2)

    float rate = m.fmax(.01f, math.get_random_snd(.04f, PSCH_SD(.01f))) / 12;

    //inflation_rate_hist.init(FA_HISTORY_YEAR, rate);
    inflation_rate_hist.init(FA_HISTORY_YEAR, 0);   //0222
    inflation_rate = inflation_rate_hist.get_sum();

    err_when(inflation_rate  != 0);

    //------------------------------------------------//
    // ----- init player input for stage1       ----- //
    //------------------------------------------------//

    int i;

    //--------//
    revenue_policy_array[PL_TUITION_RATE_GROWTH].lower_limit = -5;
    revenue_policy_array[PL_FINANCIAL_AID].lower_limit = -5;
    revenue_policy_array[PL_ENDOWMENT_SPENDING_RATE].lower_limit = 4;
    revenue_policy_array[PL_INDIRECT_COST_RATE].lower_limit = 20;

    expense_policy_array[0].lower_limit = 0.0;
    expense_policy_array[1].lower_limit = 0.0;
    expense_policy_array[2].lower_limit = -5;
    expense_policy_array[3].lower_limit = -5;
    expense_policy_array[4].lower_limit = -3;

    //----//
    revenue_policy_array[PL_TUITION_RATE_GROWTH].upper_limit = 5;
    revenue_policy_array[PL_FINANCIAL_AID].upper_limit = 5;
    revenue_policy_array[PL_ENDOWMENT_SPENDING_RATE].upper_limit = 10;
    revenue_policy_array[PL_INDIRECT_COST_RATE].upper_limit = 70;

    expense_policy_array[0].upper_limit = 5.0;
    expense_policy_array[1].upper_limit = 5.0;
    expense_policy_array[2].upper_limit = 5.0;
    expense_policy_array[3].upper_limit = 5.0;
    expense_policy_array[4].upper_limit = 3;

    revenue_policy_array[0].max_penalty1 = 10;
    revenue_policy_array[1].max_penalty1 = 10;
    revenue_policy_array[2].max_penalty1 = 5;
    revenue_policy_array[3].max_penalty1 = 20;

    expense_policy_array[0].max_penalty1 = 20;
    expense_policy_array[1].max_penalty1 = 20;
    expense_policy_array[2].max_penalty1 = 5;
    expense_policy_array[3].max_penalty1 = 10;
    expense_policy_array[4].max_penalty1 = 20;

    expense_policy_array[0].max_penalty2 = 20;
    expense_policy_array[1].max_penalty2 = 20;
    expense_policy_array[2].max_penalty2 = 5;
    expense_policy_array[3].max_penalty2 = 10;
    expense_policy_array[4].max_penalty2 = 20;

    //max_penalty_effect

    for ( i=0; i<REVENUE_POLICY_COUNT; i++) {
	//990506 revenue_policy_array[i].target_value = (revenue_policy_array[i].lower_limit+revenue_policy_array[i].upper_limit) / 2;

	revenue_policy_array[i].lower_bound = revenue_policy_array[i].lower_limit;
	revenue_policy_array[i].upper_bound = revenue_policy_array[i].upper_limit;
	revenue_policy_array[i].import_weight = OPT_MID_WEIGHT;
	revenue_policy_array[i].penalty_multiplier1 = 1;
	revenue_policy_array[i].penalty_multiplier2 = 1;

	set_policy_target(true, i);
    }

    for ( i=0; i<EXPENSE_POLICY_COUNT; i++) {
	// expense_policy_array[i].target_value = (expense_policy_array[i].lower_limit+expense_policy_array[i].upper_limit) / 2;

	expense_policy_array[i].lower_bound = expense_policy_array[i].lower_limit;
	expense_policy_array[i].upper_bound = expense_policy_array[i].upper_limit;
	expense_policy_array[i].import_weight = OPT_MID_WEIGHT;
	expense_policy_array[i].penalty_multiplier1 = 1;
	expense_policy_array[i].penalty_multiplier2 = 1;

	set_policy_target(false, i);
    }

    //---------//

    revenue_policy_array[0].lower_bound = 0;        // 990518; temp fix for 2.6.2 and 5.1.1

    //---------//
    for (i=0; i<REVENUE_POLICY_COUNT; i++) {
	revenue_policy_array[i].reset_change(true);
	revenue_policy_array[i].bound_correction();   //## chea 310899 try not using all the bound_correction
	revenue_policy_array[i].penalty_direction = NEGATIVE;
    }

    for (i=0; i<EXPENSE_POLICY_COUNT; i++) {
	expense_policy_array[i].reset_change(true);
	expense_policy_array[i].bound_correction();   //## chea 310899 try not using all the bound_correction
	expense_policy_array[i].penalty_direction = NEGATIVE;
    }

    revenue_policy_array[PL_TUITION_RATE_GROWTH].penalty_direction = POSITIVE;
    revenue_policy_array[PL_INDIRECT_COST_RATE].penalty_direction = POSITIVE;

    //
    // ----- for stage2 ----- //
    //

    cost_rise_policy_array[0].target_value = 0.0;
    cost_rise_policy_array[1].target_value = 0.0;
    cost_rise_policy_array[2].target_value = 0.0;
    cost_rise_policy_array[3].target_value = 0.0;
    cost_rise_policy_array[4].target_value = 0.0;

    cost_rise_policy_array[5].target_value = 0.0;
    cost_rise_policy_array[6].target_value = 0.0;
    cost_rise_policy_array[7].target_value = 0.0;
    cost_rise_policy_array[8].target_value = 0.0;
    cost_rise_policy_array[9].target_value = 0.0;

    cost_rise_policy_array[0].lower_bound = -5;
    cost_rise_policy_array[1].lower_bound = -5;
    cost_rise_policy_array[2].lower_bound = -5;
    cost_rise_policy_array[3].lower_bound = -5;
    cost_rise_policy_array[4].lower_bound = -5;

    cost_rise_policy_array[5].lower_bound = -5;
    cost_rise_policy_array[6].lower_bound = -5;
    cost_rise_policy_array[7].lower_bound = -5;
    cost_rise_policy_array[8].lower_bound = -5;
    cost_rise_policy_array[9].lower_bound = -5;

    cost_rise_policy_array[0].upper_bound = 5;
    cost_rise_policy_array[1].upper_bound = 5;
    cost_rise_policy_array[2].upper_bound = 5;
    cost_rise_policy_array[3].upper_bound = 5;
    cost_rise_policy_array[4].upper_bound = 5;

    cost_rise_policy_array[5].upper_bound = 5;
    cost_rise_policy_array[6].upper_bound = 5;
    cost_rise_policy_array[7].upper_bound = 5;
    cost_rise_policy_array[8].upper_bound = 5;
    cost_rise_policy_array[9].upper_bound = 5;

    cost_rise_policy_array[0].max_penalty1 = 5;
    cost_rise_policy_array[1].max_penalty1 = 10;
    cost_rise_policy_array[2].max_penalty1 = 5;
    cost_rise_policy_array[3].max_penalty1 = 50;
    cost_rise_policy_array[4].max_penalty1 = 5;

    cost_rise_policy_array[5].max_penalty1 = 5;
    cost_rise_policy_array[6].max_penalty1 = 50;
    cost_rise_policy_array[7].max_penalty1 = 10;
    cost_rise_policy_array[8].max_penalty1 = 10;
    cost_rise_policy_array[9].max_penalty1 = 50;

    cost_rise_policy_array[0].max_penalty2 = 0;
    cost_rise_policy_array[1].max_penalty2 = 0;
    cost_rise_policy_array[2].max_penalty2 = 5;
    cost_rise_policy_array[3].max_penalty2 = 10;
    cost_rise_policy_array[4].max_penalty2 = 0;

    cost_rise_policy_array[5].max_penalty2 = 50;
    cost_rise_policy_array[6].max_penalty2 = 0;
    cost_rise_policy_array[7].max_penalty2 = 0;
    cost_rise_policy_array[8].max_penalty2 = 0;
    cost_rise_policy_array[9].max_penalty2 = 0;

    for ( i=0; i<COST_RISE_POLICY_COUNT; i++) {
	cost_rise_policy_array[i].lower_limit = -5.0;
	cost_rise_policy_array[i].upper_limit = 5.0;
	cost_rise_policy_array[i].import_weight = OPT_MID_WEIGHT;
	cost_rise_policy_array[i].penalty_multiplier1 = 1;
	cost_rise_policy_array[i].penalty_multiplier2 = 1;
    }

    //---------//
    for ( i=0; i<COST_RISE_POLICY_COUNT; i++) {
	cost_rise_policy_array[i].bound_correction(); //## chea 310899 try not using all the bound_correction
	cost_rise_policy_array[i].reset_change(true);
	cost_rise_policy_array[i].penalty_direction = NEGATIVE;
    }
}

//---------- End of function Finance::init -----------//

//---------- Begin of function Finance::deinit -----------//
//!
void Finance::deinit() {

}

//---------- End of function Finance::deinit -----------//

//---------- Begin of function Finance::init_data -----------//
//!
//! All LHS variables of the assignment statments below refer to
//! cells in the first and second tables in the excel file ResAlloc.ex (file_ex)
//!
//! run before enroll_res.init_data() and enroll_main()
//! since it requires, inflation rate, tuition_rate
//!
void Finance::init_data_pre_enroll() {            // 1228
    PeerSchool *playerSchoolEx = school_res.player_peer_school;

    //---------------------------------------------------//

    tuition_rate = playerSchoolEx->tuition_rate;
}

void Finance::init_data() {
    int i;
    PeerSchool *playerSchoolEx = school_res.player_peer_school;

    //-----------------------------------------------------------//

    int totalFacResearchExpense = 0;                // monthly
    int d;

    Department *dept;

    for ( d=department_array.size(); d>0; d-- ) {
	if ( department_array.is_deleted(d) )
	    continue;

	dept = department_array[d];

	dept->calc_research_dollar();

	//## chea 221199
	//		totalFacResearchExpense += dept->total_research_dollar;
	totalFacResearchExpense += dept->total_research_dollar /1000;
    }

    int totalResearchExpense = 0;

    //0219	// 990421
    totalResearchExpense = int( 12 * totalFacResearchExpense );

    //------------ initialize balance sheet -------------//

    memset( last_year.asset_array, 0, sizeof(last_year.asset_array) );
    memset( last_year.liability_array, 0, sizeof(last_year.liability_array) );

    // 0305
    last_year.asset_array[AC_OPERATING_RESERVE] = playerSchoolEx->operating_reserve * 1.3;
    this_year.asset_array[AC_OPERATING_RESERVE]  = last_year.asset_array[AC_OPERATING_RESERVE];
    last_year.asset_array[AC_ENDOWMENT]       = playerSchoolEx->endowment_market;
    last_year.asset_array[AC_BUILDINGS]       = playerSchoolEx->buildings;
    last_year.asset_array[AC_CAPITAL_RESERVE]   = playerSchoolEx->capital_reserve;
    last_year.asset_array[AC_RESIDENCE]       = 0;
    investment_office.smoothed_endowment_value = (float) finance.last_year.asset_array[AC_ENDOWMENT];
    investment_office.smoothed_quasi_endowment_value = (float) finance.last_year.asset_array[AC_QUASI];

    float randomNo = (float)m.random(100);
    randomNo /= 4*100;                              // randomNo ( 0->0.25 )
    //BUGHERE Kenneth
    last_year.asset_array[AC_QUASI]         = last_year.asset_array[AC_ENDOWMENT]*randomNo;
    last_year.asset_array[AC_ENDOWMENT]       *= (1-randomNo);

    this_year.asset_array[AC_QUASI]         = last_year.asset_array[AC_QUASI];
    this_year.asset_array[AC_ENDOWMENT]       = last_year.asset_array[AC_ENDOWMENT];

    endowment_quasi_ratio = randomNo;

    err_when(playerSchoolEx->general_plant_debt < 1);
    last_year.liability_array[AC_GENERAL_PLANT_DEBT]  = playerSchoolEx->general_plant_debt;
    last_year.liability_array[AC_RESIDENCE_HALL_DEBT] = playerSchoolEx->residence_hall_debt;

    //------------ initialize revenue data -------------//

    memset( revenue_array, 0, sizeof(revenue_array) );
    memset( expense_array, 0, sizeof(expense_array) );

    // 0118; old: playerSchoolEx->gross_tuition_revenue;
    revenue_array[AC_GROSS_TUITION_INCOME].total    = enroll_res.total_student_count * tuition_rate / 1000;
    // 0118; old: playerSchoolEx->financial_aid;
    revenue_array[AC_FINANCIAL_AID].total         = -(enroll_res.total_aid)*0.3;
    // playerSchoolEx->net_tuition_revenue;
    revenue_array[AC_NET_TUITION_INCOME].total      = revenue_array[AC_GROSS_TUITION_INCOME].total + revenue_array[AC_FINANCIAL_AID].total;

    // 0118
    revenue_policy_array[PL_INDIRECT_COST_RATE].result_value = playerSchoolEx->overhead_rate_on_sponsored_research;
    //## chea 310899 try not using all the bound_correction
    revenue_policy_array[PL_INDIRECT_COST_RATE].bound_correction();

    //--------------------------//
    // 990527

    FinancePolicy* policy;

    policy = &revenue_policy_array[PL_ENDOWMENT_SPENDING_RATE];

    policy->target_value = policy->result_value = 5;
    policy->bound_correction();                     //## chea 310899 try not using all the bound_correction

    //-------------------------//

    //playerSchoolEx->sponsored_research;
    revenue_array[AC_SPONSORED_RESEARCH_REVENUE].total  = totalResearchExpense;
    //playerSchoolEx->sponsored_research / (1+playerSchoolEx->overhead_rate_on_sponsored_research/100.0);
    revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct = totalResearchExpense  / ( 1 + get_research_overhead_rate() / 100.0);
    revenue_array[AC_SPONSORED_RESEARCH_REVENUE].indirect = totalResearchExpense - revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct;

    //## chea testing....
    //	revenue_array[AC_SPONSORED_RESEARCH_REVENUE].total	= 0;	//playerSchoolEx->sponsored_research;
    //	revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct	= 0;	//playerSchoolEx->sponsored_research / (1+playerSchoolEx->overhead_rate_on_sponsored_research/100.0);
    //	revenue_array[AC_SPONSORED_RESEARCH_REVENUE].indirect	= 0;;

    // or = gifts_to_operations
    revenue_array[AC_GIFTS].total               = playerSchoolEx->total_gifts;
    // or = endowment_spending		// Or should we use the equation in file_ex instead?
    revenue_array[AC_ENDOWMENT_SPENDING].total      = playerSchoolEx->raw_endowment_spending;

    //## chea begin 080699 5.2.1
    //revenue_array[AC_ENDOWMENT_SPENDING].total= last_year.asset_array[AC_ENDOWMENT] * (revenue_policy_array[PL_ENDOWMENT_SPENDING_RATE].result_value / 100);
    //	last_year.asset_array[AC_ENDOWMENT]	= ((1+inflation_rate+0.06-0.05)*revenue_array[AC_ENDOWMENT_SPENDING].total)/0.05;
    investment_office.smoothed_endowment_value = (float) finance.last_year.asset_array[AC_ENDOWMENT];
    //## chea end 080699 5.2.1
    investment_office.smoothed_quasi_endowment_value = (float) finance.last_year.asset_array[AC_QUASI];

    revenue_array[AC_INTERCOLLEGIATE_ATHLETICS].total = playerSchoolEx->athletics_revenue;
    // or = playerSchoolEx->raw_other_operating_income ?
    revenue_array[AC_OTHER_OPERATING_INCOME].total    = playerSchoolEx->other_operating_income;
    // playerSchoolEx->interest_on_operating_reserve;
    revenue_array[AC_INTEREST_EARNED_OR_PAID].total   = last_year.asset_array[AC_OPERATING_RESERVE] * short_term_debt_interest / 100.0;

    if ( player_school.is_public() )
	revenue_array[AC_STATE_APPROPRIATION].total   = playerSchoolEx->state_appropriations;
    else
	revenue_array[AC_STATE_APPROPRIATION].total   = 0;

    //---------- initialize expenditure data -----------//

    // 0118; old: playerSchoolEx->dept_expense_faculty_salaries;
    expense_array[AC_ACADEMIC_DEPARTMENTS].faculty = calc_total_faculty_salary();

    // 0118
    double ratio = expense_array[AC_ACADEMIC_DEPARTMENTS].faculty / playerSchoolEx->dept_expense_faculty_salaries;

    expense_array[AC_ACADEMIC_DEPARTMENTS].staff   = ratio * playerSchoolEx->dept_expense_staff_salaries;
    expense_array[AC_ACADEMIC_DEPARTMENTS].other   = ratio * playerSchoolEx->dept_expense_other;

    // 0118: see notes 990114, point 5 for this new ini method

    double directRev = revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct;

    double tmp = playerSchoolEx->sponsored_research_faculty_salaries
	+ playerSchoolEx->sponsored_research_staff_salaries
	+ playerSchoolEx->sponsored_research_other_expense;

    if(tmp==0) {                                    //## chea 091199 research_expense_fac_ratio can be 0
	research_expense_fac_ratio = 0;
	research_expense_staff_ratio = 0;
    }
    else {
	research_expense_fac_ratio = playerSchoolEx->sponsored_research_faculty_salaries/ tmp;
	research_expense_staff_ratio = playerSchoolEx->sponsored_research_staff_salaries/ tmp;
    }

    expense_array[AC_SPONSORED_RESEARCH_EXPENSE].faculty = research_expense_fac_ratio * directRev;
    expense_array[AC_SPONSORED_RESEARCH_EXPENSE].staff   = research_expense_staff_ratio * directRev;
    expense_array[AC_SPONSORED_RESEARCH_EXPENSE].other   = directRev * (1-research_expense_fac_ratio-research_expense_staff_ratio);

    // new if 1005
    expense_array[AC_LIBRARIES].staff   = playerSchoolEx->library_staff_salaries;
    expense_array[AC_LIBRARIES].other   = playerSchoolEx->library_other_expense;
    expense_array[AC_IT_RESOURCES].staff   = playerSchoolEx->academic_it_staff_salaries;
    expense_array[AC_IT_RESOURCES].other   = playerSchoolEx->academic_it_other_expense;

    expense_array[AC_STUDENT_LIFE].staff = playerSchoolEx->student_life_staff_salaries;
    expense_array[AC_STUDENT_LIFE].other = playerSchoolEx->student_life_other_expense;

    expense_array[AC_ATHLETICS].staff = playerSchoolEx->athletics_salaries;
    expense_array[AC_ATHLETICS].other = playerSchoolEx->athletics_other_expense;

    expense_array[AC_INST_ADVANCEMENT].staff = playerSchoolEx->inst_advancement_staff_salaries;
    expense_array[AC_INST_ADVANCEMENT].other = playerSchoolEx->inst_advancement_other_expense;

    expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].staff = playerSchoolEx->administration_staff_salaries + playerSchoolEx->other_operating_expense_staff_salaries;
    expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].other = playerSchoolEx->administration_other_expense + playerSchoolEx->other_operating_expense_other_expense;

    // 0119 update ONM base on notes 0114

    tmp = playerSchoolEx->o_and_m_staff_salaries + playerSchoolEx->o_and_m_other_expense;
    double stratio = playerSchoolEx->o_and_m_staff_salaries / tmp;
    float tmpTotal = float(facility_office.total_normal_onm);

    if ( player_school.relative_wealth == WEALTH_RICH )
	tmpTotal *= math.get_random_snd(1, PSCH_SD(0.005f));
    //		tmpTotal *= 20;

    else if ( player_school.relative_wealth == WEALTH_IN_BETWEEN )
	tmpTotal *= math.get_random_snd(0.95f, PSCH_SD(0.01f));
    //		tmpTotal *= 10;

    else
	tmpTotal *= math.get_random_snd(0.60f, PSCH_SD(0.075f));
    //      tmpTotal *= 1;

    double staffR = expense_array[AC_OPERATIONS_AND_MAINTENANCE].staff = tmpTotal * stratio;
    double otherR = expense_array[AC_OPERATIONS_AND_MAINTENANCE].other = tmpTotal * (1-stratio);

    // AC_ENROLLMENT_MANAGEMENT: new item see WFM notes 0918 section 10
    int totalTargetIntake = 0;

    for (i=0; i<MAX_STUDENT_LEVEL; i++)
	totalTargetIntake += enroll_res.target_student_intake[i];

    if ( totalTargetIntake <= 0 )                   // 0118
	totalTargetIntake = 1000;                     // 1228

    float expensePerAdmit = 1;

    expense_array[AC_ENROLLMENT_MANAGEMENT].staff = expensePerAdmit * totalTargetIntake * math.get_random_snd(1.0f, PSCH_SD(0.1f));
    expense_array[AC_ENROLLMENT_MANAGEMENT].other = (otherR/(staffR+otherR)) * expense_array[AC_ENROLLMENT_MANAGEMENT].staff;

    expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].staff -= expense_array[AC_ENROLLMENT_MANAGEMENT].staff;
    expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].other -= expense_array[AC_ENROLLMENT_MANAGEMENT].other;

    // should we initialize this one according to the excel ex file
    expense_array[AC_SERVICE_ON_GENERAL_PLANT_DEBT].other = playerSchoolEx->service_on_general_plant_debt;
    //## CHEA 250899
    expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].other = playerSchoolEx->transfer_to_capital_reserve;

    //------- calculate the total of each expense item --------//
    // should we use the equation in file_ex for cell F31 (expense_array[AC_SPONSORED_RESEARCH])

    for( i=0 ; i<TOTAL_EXPENSE_ITEM_COUNT ; i++ ) {
	expense_array[i].total = expense_array[i].faculty + expense_array[i].staff +
	    expense_array[i].other;
    }

    //---------------------------------------// 1229; by WFM notes 1222

    //## chea begin "special case" when spon.=none 070699 5.1.4
    if ( player_school.sponsored_research_intensity <= 0 ) {
	revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct=0;
	revenue_array[AC_SPONSORED_RESEARCH_REVENUE].indirect=0;
	revenue_array[AC_SPONSORED_RESEARCH_REVENUE].total=0;
	revenue_array[AC_SPONSORED_RESEARCH_REVENUE].percent=0;

	expense_array[AC_SPONSORED_RESEARCH_EXPENSE].faculty=0;
	expense_array[AC_SPONSORED_RESEARCH_EXPENSE].staff=0;
	expense_array[AC_SPONSORED_RESEARCH_EXPENSE].other=0;
	expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total=0;
	expense_array[AC_SPONSORED_RESEARCH_EXPENSE].percent=0;
    }

    float maxRate = float(expense_array[AC_ACADEMIC_DEPARTMENTS].total );

    // 990527
    maxRate = float(maxRate) *100 / float(maxRate + expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total);

    //## chea begin 070699 14.1.5 2nd change
    calc_total_last_year();                         // calculate total_operating_expense for use here so no need to call at "spec1" below

    policy = &revenue_policy_array[PL_INDIRECT_COST_RATE];

    if ( policy->upper_bound > maxRate ) {
	//policy->upper_bound = maxRate; //## chea fed ori.
	policy->upper_bound =((total_operating_expense.total-expense_array[AC_ACADEMIC_DEPARTMENTS].total- expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total)/total_operating_expense.total)*100;
	policy->bound_correction();                   //## chea 310899 try not using all the bound_correction
    }
    //## chea end 070699 14.1.5 2nd change

    //policy->result_value = policy->target_value = ( policy->upper_bound + policy->lower_bound ) / 2;//## chea fed ori.
    //## chea 040699 1st change
    policy->result_value = policy->target_value = policy->upper_bound;

    // 990527 revenue_array[AC_SPONSORED_RESEARCH_REVENUE].indirect	= min(revenue_array[AC_SPONSORED_RESEARCH_REVENUE].indirect, maxRate * revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct / 100);
    revenue_array[AC_SPONSORED_RESEARCH_REVENUE].indirect =  policy->target_value * revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct / 100;
    revenue_array[AC_SPONSORED_RESEARCH_REVENUE].total  = revenue_array[AC_SPONSORED_RESEARCH_REVENUE].indirect + revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct;

    //______init Surplus or deficit-------------//

    policy = &expense_policy_array[PL_SURPLUS_DEFICIT];
    //	policy->upper_bound =(finance.last_year.surplus_deficit/total_expense.total)*100;
    policy->bound_correction();                     //## chea 310899 try not using all the bound_correction

    //----- Surplus or deficit = set optimization target value for surplus/deficit for the first running year based on Relative Weatlh setting ----//

    switch(player_school.relative_wealth) {
    case WEALTH_RICH:
	policy->target_value = 2.0f;
	break;

    case WEALTH_IN_BETWEEN:
	policy->target_value = 0.0f;
	break;

    case WEALTH_STRUGGLING:
	policy->target_value = -2.0f;
	break;
    }

    //--------- special case: scenario ---------//

    switch ( player_school.scenario_id ) {
    case ( SCN_WINNING_ATHLETICS ):
	revenue_array[AC_INTERCOLLEGIATE_ATHLETICS].total *= 0.5;
	expense_array[AC_ATHLETICS].staff *= 0.5;
	expense_array[AC_ATHLETICS].other *= 0.5;

	i = AC_ATHLETICS;
	expense_array[i].total = expense_array[i].faculty + expense_array[i].staff +
	    expense_array[i].other;
	break;

    case ( SCN_LIMIT_TUITION_GROWTH ):
	revenue_policy_array[PL_TUITION_RATE_GROWTH].upper_limit = 0;
	revenue_policy_array[PL_TUITION_RATE_GROWTH].upper_bound = 0;
	revenue_policy_array[PL_TUITION_RATE_GROWTH].lower_limit = -5.0f;
	revenue_policy_array[PL_TUITION_RATE_GROWTH].lower_bound = -5.0f;
	revenue_policy_array[PL_TUITION_RATE_GROWTH].target_value = 0;
	//## chea 310899 try not using all the bound_correction
	revenue_policy_array[PL_TUITION_RATE_GROWTH].bound_correction();
	player_school.scenario_base = tuition_rate;
	break;
    }

    //--------- calculate the total ---------//

    calc_total_last_year();

    //---- input affected by other models ----//

    err_when(tuition_rate < 1000);                  // this the model's main income!!!

    //---------------------------------------//
    // Section 5.3 Interest Rates "WFM notes 0919":
    // Section 8.6 of the Note of 30 August is hereby AMENDED so that
    // the short-term real interest rate for positive operating reserves ("Bank rate")
    // is subject to random variation. The figure given there (0.5%) should be used as
    // the initialization value. The borrowing rate will always be 4 percentage points
    // greater than the bank rate.

    double opReserve = last_year.asset_array[AC_OPERATING_RESERVE];
    base_short_term_debt_interest = (opReserve>0) ? inflation_rate + short_term_debt_interest : inflation_rate + 4.5;
    base_long_term_debt_interest  = short_term_debt_interest + math.get_random_snd(0, PSCH_SD(0.25f));

    //---------------------------------------//
    //---------------------------------------//

    //---------// 0407

    for (i=department_array.department_count-1; i>=0; i--) {
	hiring_policy_array[i].upper_bound = MAX_S3_SLIDER;
	hiring_policy_array[i].target_value = 1;
	hiring_policy_array[i].result_value = 0;
	hiring_policy_array[i].weight = OPT_MID_WEIGHT;
    }

    //---------------------------------------//
    // run optimizations

    //----------------- scenario special handling --------------//

    if( player_school.scenario_id == SCN_BALANCE_BUDGET ) {
	expense_policy_array[PL_SURPLUS_DEFICIT].target_value =
	    // set it to the lowest
	    expense_policy_array[PL_SURPLUS_DEFICIT].lower_limit;
    }

    //---------------------------------------------------//

    finance.process_budget();

    department_array.salary_setting();              // 990528
    department_array.faculty_hiring();              // 990528
    calc_total_faculty_salary();                    // 990528

    //----- update projected_array's year_to_date according to ratioMonthLeft -----//

    if (  info.game_month >= fiscal_year_start_month )
	month_left = (12 - info.game_month + fiscal_year_start_month);
    else
	month_left = (- info.game_month + fiscal_year_start_month);

    // IMPORTANT asumption: the start date of a game is
    // equal to the start of financial year.
    //
    // prepare data for update_projected_arrays_monthly_sub() later
    err_when(month_left != 12);

    float ratioMonth = (12-month_left) / 12.0f;

    for( i=0 ; i<REVENUE_ITEM_COUNT ; i++ ) {
	projected_revenue_array[i].year_to_date.direct = ratioMonth * revenue_array[i].direct;
	projected_revenue_array[i].year_to_date.indirect = ratioMonth * revenue_array[i].indirect;
	projected_revenue_array[i].year_to_date.total = ratioMonth * revenue_array[i].total;

	projected_revenue_array[i].last_month.direct = budget_revenue_array[i].direct / 12.0;
	projected_revenue_array[i].last_month.indirect = budget_revenue_array[i].indirect / 12.0;
	projected_revenue_array[i].last_month.total = budget_revenue_array[i].total / 12.0;
	// 990526 990422  projected_revenue_array[i].last_month.total = revenue_array[i].total / 12.0;
    }

    for( i=0 ; i<EXPENSE_ITEM_COUNT ; i++ ) {
	projected_expense_array[i].year_to_date.faculty = ratioMonth * expense_array[i].faculty;
	projected_expense_array[i].year_to_date.staff = ratioMonth * expense_array[i].staff;
	projected_expense_array[i].year_to_date.other = ratioMonth * expense_array[i].other;
	projected_expense_array[i].year_to_date.total = ratioMonth * expense_array[i].total;

	projected_expense_array[i].last_month.faculty = budget_expense_array[i].faculty / 12.0;
	projected_expense_array[i].last_month.staff = budget_expense_array[i].staff / 12.0;
	projected_expense_array[i].last_month.other = budget_expense_array[i].other / 12.0;
	projected_expense_array[i].last_month.total = budget_expense_array[i].total / 12.0;
    }

    //----------------------//

    const float perMonth = 1.0f/12.0f;

    projected_expense_array[AC_ACADEMIC_DEPARTMENTS].last_month.faculty
	= perMonth * total_faculty_salary;            // 990219

    update_external_factors_monthly();
    update_external_factors_yearly();
    update_projected_arrays_monthly_sub();          // which will not call update_external_factors_monthly()

    //--------------------//
    // 0526

#ifdef DEBUG_VC

    String s;

    DEBUG_LOG("\n\n-------- START OF revenue_array --------");

    for( i=0 ; i<REVENUE_ITEM_COUNT ; i++ ) {
	s = m.format(revenue_array[i].total);   DEBUG_LOG(s);
    }

    s = m.format(total_revenue.total);  DEBUG_LOG(s);

    DEBUG_LOG("\n\n-------- START OF expense_array --------");

    for( i=0 ; i<EXPENSE_ITEM_COUNT ; i++ ) {
	s = m.format(expense_array[i].total);   DEBUG_LOG(s);
    }

    s = m.format(total_expense.total);  DEBUG_LOG(s);

    DEBUG_LOG("\n\n-------- START OF budget_revenue_array --------");

    for( i=0 ; i<REVENUE_ITEM_COUNT ; i++ ) {
	s = m.format(budget_revenue_array[i].total);    DEBUG_LOG(s);
    }

    s = m.format(budget_total_revenue.total); DEBUG_LOG(s);

    DEBUG_LOG("\n\n-------- START OF budget_expense_array --------");

    for( i=0 ; i<EXPENSE_ITEM_COUNT ; i++ ) {
	s = m.format(budget_expense_array[i].total);    DEBUG_LOG(s);
    }

    s = m.format(budget_total_expense.total); DEBUG_LOG(s);

    DEBUG_LOG("\n\n-------- end of Finance::init_data() --------\n\n");
#endif
}

void Finance::post_init_data() {
    memcpy(&next_year, &this_year, sizeof(YearReport));
}

//---------- End of function Finance::init_data -----------//

// --- Begin of function Finance::calc_distance_learning_expediture --- //
//
void Finance::calc_distance_learning_expenditure() {
    // calculate total distance learning place in the institution
    int totalDLPlace = 0;
    int deptSize = department_array.size();
    for ( int i=deptSize; i>0; i-- ) {
	if (department_array.is_deleted(i))
	    continue;

	totalDLPlace += department_array[i]->total_distance_learning_place;
    }

    // Item 8d vi)

  // calculate DL_tuition
    float DLTuition = (student_office.tuition_rate_history[HISTORY_YEAR_COUNT-1]/8)*library_tech_office.percent_dl_tuition/100;

    // calculate net distance learning expense in finance
    finance.net_distance_learning_expense.total = finance.projected_net_distance_learning_expense.this_year.total;

    finance.projected_net_distance_learning_expense.this_year.total =
	(totalDLPlace - department_array.outsourcing_credits[MAX_TEACHING_METHOD])*DLTuition;

    finance.projected_net_distance_learning_expense.next_year.total =
	finance.projected_net_distance_learning_expense.this_year.total;
}

//
// --- End of function Finance::calc_distance_learning_expediture --- //

//---------- Begin of function Finance::calc_total_last_year -----------//
//!
//! called once from init_data()
//!
void Finance::calc_total_last_year() {
    //------- calculate the total revenue ---------//

    memset( &total_revenue, 0, sizeof(total_revenue) );

  //------- calculate net tuition ------//

    revenue_array[AC_NET_TUITION_INCOME].total =
	revenue_array[AC_GROSS_TUITION_INCOME].total + revenue_array[AC_FINANCIAL_AID].total;

  //----- calculate the total of one row ------//
  // some items do not have separate values for the direct and indirect categories.

    int i = AC_SPONSORED_RESEARCH_REVENUE;
    revenue_array[i].total = revenue_array[i].direct + revenue_array[i].indirect;

    for( i=AC_NET_TUITION_INCOME; i<REVENUE_ITEM_COUNT ; i++ ) {
	//------ calculate totals of all rows -------//

	total_revenue.direct   += revenue_array[i].direct;
	total_revenue.indirect += revenue_array[i].indirect;
	total_revenue.total    += revenue_array[i].total;
    }

    //------- calculate the total expense ---------//

    memset( &total_expense, 0, sizeof(total_expense) );

    for( i=0 ; i<OPERATING_EXPENSE_ITEM_COUNT ; i++ ) {
	//----- calculate the total of one row ------//

	expense_array[i].total = expense_array[i].faculty +
	    expense_array[i].staff + expense_array[i].other;

	//------ calculate totals of all rows -------//

	total_expense.faculty += expense_array[i].faculty;
	total_expense.staff   += expense_array[i].staff;
	total_expense.other   += expense_array[i].other;
	total_expense.total   += expense_array[i].total;
    }

    //------- calculate the total operating expense ---------//
    memcpy( &total_operating_expense, &total_expense, sizeof(total_expense) );

    for( i=OPERATING_EXPENSE_ITEM_COUNT ; i<TOTAL_EXPENSE_ITEM_COUNT ; i++ ) {
	//----- calculate the total of one row ------//

	expense_array[i].total = expense_array[i].faculty +
	    expense_array[i].staff + expense_array[i].other;

	//------ calculate totals of all rows -------//

	total_expense.faculty += expense_array[i].faculty;
	total_expense.staff   += expense_array[i].staff;
	total_expense.other   += expense_array[i].other;
	total_expense.total   += expense_array[i].total;
    }

    //----------------------//

    finance.last_year.asset_array[AC_ASSET_TOTAL] = 0 ;
    for( i=0 ; i<ASSET_ITEM_COUNT-1 ; i++ ) {
	finance.last_year.asset_array[AC_ASSET_TOTAL] += finance.last_year.asset_array[i];
    }

    finance.last_year.liability_array[AC_LIABILITY_TOTAL] = 0 ;
    for( i=0 ; i<LIABILITY_ITEM_COUNT-1 ; i++ ) {
	finance.last_year.liability_array[AC_LIABILITY_TOTAL] += finance.last_year.liability_array[i];
    }

    //------- calculate the total revenue AGAIN ---------//

    memset( &total_revenue, 0, sizeof(total_revenue) );

    //----- calculate the total of one row ------//
    // some items do not have separate values for the direct and indirect categories.
    i = AC_SPONSORED_RESEARCH_REVENUE;
    revenue_array[i].total = revenue_array[i].direct + revenue_array[i].indirect;

    for( i=AC_NET_TUITION_INCOME; i<REVENUE_ITEM_COUNT ; i++ ) {
	//------ calculate totals of all rows -------//

	total_revenue.direct   += revenue_array[i].direct;
	total_revenue.indirect += revenue_array[i].indirect;
	total_revenue.total    += revenue_array[i].total;
    }

    //------- calculate the total expense ---------//

    memset( &total_expense, 0, sizeof(total_expense) );

    for( i=0 ; i<OPERATING_EXPENSE_ITEM_COUNT ; i++ ) {
	//----- calculate the total of one row ------//

	expense_array[i].total = expense_array[i].faculty +
	    expense_array[i].staff + expense_array[i].other;

	//------ calculate totals of all rows -------//

	total_expense.faculty += expense_array[i].faculty;
	total_expense.staff   += expense_array[i].staff;
	total_expense.other   += expense_array[i].other;
	total_expense.total   += expense_array[i].total;
    }

    //------- calculate the total operating expense ---------//
    memcpy( &total_operating_expense, &total_expense, sizeof(total_expense) );

    for( i=OPERATING_EXPENSE_ITEM_COUNT ; i<TOTAL_EXPENSE_ITEM_COUNT ; i++ ) {
	//----- calculate the total of one row ------//

	expense_array[i].total = expense_array[i].faculty +
	    expense_array[i].staff + expense_array[i].other;

	//------ calculate totals of all rows -------//

	total_expense.faculty += expense_array[i].faculty;
	total_expense.staff   += expense_array[i].staff;
	total_expense.other   += expense_array[i].other;
	total_expense.total   += expense_array[i].total;
    }

    //----------------------//

                                                  // or = playerSchoolEx->surplus_or_deficit;//## chea 070699
    last_year.surplus_deficit = total_revenue.total - total_expense.total;

    //--- also calculate the percent of each item over the total ---//

    calc_percent_last_year();
}

//----------- End of function Finance::calc_total_last_year ------------//

//----------- End of function Finance::apply_relative_wealth_effect ------------//

//---------- Begin of function Finance::calc_percent_last_year -----------//
//!
void Finance::calc_percent_last_year() {
    //------- calculate the total revenue ---------//

    for( int i=AC_NET_TUITION_INCOME; i<REVENUE_ITEM_COUNT; i++ ) {
	revenue_array[i].percent = (float) 100.0 * revenue_array[i].total /
	    total_revenue.total;
    }

    //------- calculate the total expense ---------//
    total_operating_expense.percent = 0;

    for( int i=0 ; i<EXPENSE_ITEM_COUNT ; i++ ) {
	expense_array[i].percent = (float) 100.0 * expense_array[i].total /
	    total_expense.total;

	if ( i < OPERATING_EXPENSE_ITEM_COUNT )
	    total_operating_expense.percent += expense_array[i].percent;
    }

    total_revenue.percent = 100;
    total_expense.percent = 100;
}

//----------- End of function Finance::calc_percent_last_year ------------//

//------ Begin of function Finance::process_budget ------//
//!
//! should be "init_budget"
//!
void Finance::process_budget() {
    //---------------------------------------------------//
    //
    // Step 1: calculates the drivers of aggregate revenues
    //			  and expenditures based on policy parameters
    //			  that can be influenced by the player.
    //
    //---------------------------------------------------//

    optimize_policy_1(true);

    //---------------------------------------------------//
    //
    // Step 2: allocates the budget adjustments among
    //			  functions and activities: e.g. faculty,
    //			  other departmental expense, libraries and
    //			  etc.
    //
    //---------------------------------------------------//

    optimize_policy_2();

    //---------------------------------------------------//
    //
    // Step 3: allocates faculty salary expenditure adjustments
    //			  among departments. And also determines the
    //			  number of new faculty that each department
    //			  can hire.
    //
    //---------------------------------------------------//

    calc_budget_report();

    pre_optimization_stage_3(true);                 //true: init
    optimize_policy_3();

    //---------------------------------------------------//
    //
    // Step Final: calculate the player's budget
    //					"Year-End Financial Report"
    //
    //---------------------------------------------------//

    //990520 calc_budget_report();

    //---- scenario special handling ---------//

    if( player_school.scenario_id == SCN_RAISE_SALARY ) {
	player_school.scenario_faculty_salary_increase =
	    100.0f * ( (1.0f + player_school.scenario_faculty_salary_increase / 100.0f)
		       *(1.0f + (float) expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value / 100.0f) - 1.0f);
    }

    //---- reset user preference settings after optimization -----//

    sys.redraw_all_flag=1;
}

//------- End of function Finance::process_budget -------//

//------ Begin of function Finance::set_student_policy ------//
//!
//! init student_policy_array *according* to ResAlloc.ex.xls file
//!
void Finance::set_student_policy(bool init) {
    memset(student_policy_array, 0, sizeof(student_policy_array));

    err_when(PL_TRADITIONAL_UG != UG_TRADITION);
    err_when(MAX_STUDENT_LEVEL != STUDENT_POLICY_COUNT-1);

    int i;

    // init student_policy_array

    // assume number of continued students (enroll_res.student_count)
    // is updated (for stu dropout & gradutate) in enrollment in the *end* of last trimester
    // and enrollment of new student will be ran after optimization
    //
    //
    enroll_res.calc_student_count();                // may slow down the game, but it's safe

    for (i=0; i<MAX_STUDENT_LEVEL; i++) {
	if ( init ) {                                 // 990525
	    student_policy_array[i].continue_next_year
		= enroll_res.student_count[i] - enroll_res.target_student_intake[i];
	}
	else
	    student_policy_array[i].continue_next_year =  enroll_res.student_count[i];
    }

    //0107 just for safe
    student_policy_array[PL_TOTAL].continue_next_year = 0;
    //0107 just for safe
    student_policy_array[PL_TOTAL].estimate_next_year = 0;

    for (i=0; i<STUDENT_POLICY_COUNT-1; i++) {
	//student_policy_array[PL_TOTAL].last_year				+= student_policy_array[i].last_year;
	student_policy_array[PL_TOTAL].continue_next_year  += student_policy_array[i].continue_next_year; {

	    //if ( student_policy_array[i].continue_next_year != 0 ) // 0107 for student level 4: doctor always has no students
	    if ( init ) {                               // 990525
		student_policy_array[i].estimate_next_year
		    = enroll_res.target_student_intake[i] + student_policy_array[i].continue_next_year;
	    }
	    else {                                      // 990525

		//## chea 201299
		int newMatric = (int)((enroll_res.total_matrics[i] + enroll_res.target_student_intake[i]) / 2);
		//				int newMatric = int(( enroll_res.total_matrics[i]
		//					+ enroll_res.target_student_intake[i] * enroll_res.cur_yield_rate[i] ) / 2);

		student_policy_array[i].estimate_next_year
		    = int( newMatric + student_policy_array[i].continue_next_year);
	    }
	}

	student_policy_array[PL_TOTAL].estimate_next_year  += student_policy_array[i].estimate_next_year;
    }
}

//------ End of function Finance::set_student_policy ------//

//------ Begin of function Finance::init_optimize_policy_1 ------//
//!
//! init the table "Strucaltural equations (usedin Stage1)" in ResAlloc.ex.xls file
//! for optimize_policy_1() (the vaiables below are just enough as input to stage 1)
//!
void Finance::init_optimize_policy_1() {
    int athleticRev = 0;
    int athleticExp = 0;
    // passed by reference
    athletics_office.calc_adjusted_revenue_expense(athleticRev, athleticExp);

    if(last_year.asset_array[AC_ENDOWMENT] <=0)
	last_year.asset_array[AC_ENDOWMENT] = 1;

    err_when(last_year.asset_array[AC_ENDOWMENT] <=0);
}

//------ End of function Finance::init_optimize_policy_1 ------//

//------ Begin of function Finance::optimize_policy_1 ------//
//!
//! This function is for stage 1 optimization of financial policies.
//! See file ResAlloc.Equation for details.
//!
bool Finance::optimize_policy_1(bool init) {

    DEBUG_LOG("----------- finance.opt_policy_1(): start -----------");

    int i;

    // to calculate policy_array.result_value based on other policy_array's member vars

#ifdef DEBUG

    // check interface bug, if any
    double target;

    for (i=0; i<REVENUE_POLICY_COUNT; i++) {
	target = revenue_policy_array[i].target_value;

	//		err_when(target < revenue_policy_array[i].lower_bound);
	//		err_when(target > revenue_policy_array[i].upper_bound);

	if(target < revenue_policy_array[i].lower_bound)
	    target = revenue_policy_array[i].target_value = revenue_policy_array[i].lower_bound;
	if (target > revenue_policy_array[i].upper_bound)
	    target = revenue_policy_array[i].target_value = revenue_policy_array[i].upper_bound;

	//		err_when(revenue_policy_array[i].import_weight > OPT_HIGH_WEIGHT);
	//		err_when(revenue_policy_array[i].import_weight < OPT_LOW_WEIGHT );
	if (revenue_policy_array[i].import_weight > OPT_HIGH_WEIGHT)
	    revenue_policy_array[i].import_weight = OPT_HIGH_WEIGHT;
	if (revenue_policy_array[i].import_weight < OPT_LOW_WEIGHT )
	    revenue_policy_array[i].import_weight = OPT_LOW_WEIGHT;
    }

    for (i=0; i<EXPENSE_POLICY_COUNT; i++) {
	target = expense_policy_array[i].target_value;

	err_when(expense_policy_array[i].import_weight > OPT_HIGH_WEIGHT);
	err_when(expense_policy_array[i].import_weight < OPT_LOW_WEIGHT);
    }
#endif

    //-------------------------//
    // adjust upper bound of research overhead rate

    FinancePolicy* policy = &revenue_policy_array[PL_INDIRECT_COST_RATE];

    // don't do adjustment here, moved to Finance::next_day on first day of trimester year

    if( info.prerun_year ) {
	// change the surplus/deficit target on prerun year
	double surplusRatio;
	if ( player_school.relative_wealth == WEALTH_RICH ) {
	    surplusRatio = 0.025f;
	}
	else if ( player_school.relative_wealth == WEALTH_STRUGGLING ) {
	    surplusRatio = -0.025f;
	}
	else
	    surplusRatio = 0.0f;

	surplusRatio += (double) math.get_random_snd(0.0f,PSCH_SD(0.001f));

	expense_policy_array[PL_SURPLUS_DEFICIT].target_value = surplusRatio * 100.0;
	expense_policy_array[PL_SURPLUS_DEFICIT].required_flag = 1;

	// force 60% indirect cost rate
	revenue_policy_array[PL_INDIRECT_COST_RATE].target_value = 60.0;
	revenue_policy_array[PL_INDIRECT_COST_RATE].required_flag = 1;
    }

    //-------------------------//

    //-------------------------//
    set_student_policy(init);                       // data will be used by optimize_policy_1()
    init_optimize_policy_1();

    // 0.99->99%  //## chea 300899 put these in the opt stage 1 above
    state_appropriation_real_growth = math.get_random_snd(.015f, PSCH_SD(.01f));

#ifndef _DEBUG_STAGE1

    Revenue_and_Expense Base_Budget;                // Base_Budget is projected actual on 1st Aug
    Revenue_and_Expense Base_Actual;                // Base_Actual is actual at year end
    // slider in stage 1 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
    float PL_Policy_Array_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT][4];
    // slider in stage 2 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
    float PL_Policy_Array_S2[COST_RISE_POLICY_COUNT][4];
    double inflation, Base_tuition_rate;
    double Base_enrollment, Target_enrollment, Actual_enrollment;
    double Base_endowmentMV, Base_OpResBalance;
    float ST_int_rate_01Aug, ST_int_rate_31Aug;
    double New_Debt_service;

    input_Optimize_policy_ver2(
	Base_Budget,                                  // Base_Budget is projected actual on 1st Aug
	Base_Actual,                                  // Base_Actual is actual at year end
	PL_Policy_Array_S1,                           // slider in stage 1 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
	PL_Policy_Array_S2,                           // slider in stage 2 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
	inflation, Base_tuition_rate,
	Base_enrollment, Target_enrollment, Actual_enrollment,
	Base_endowmentMV, Base_OpResBalance,
	ST_int_rate_01Aug, ST_int_rate_31Aug,
	New_Debt_service );

#else

    Revenue_and_Expense Base_Budget = {             // Base_Budget is projected actual on 1st Aug
	0.0, 0.0, {
	    // 10x4
	    { 0, 0, 0, 20000 },
	    { 0, 0, 0, -3000 },
	    { 0, 0, 0, 17000 },
	    { 0, 3771, 2200, 5971 },
	    { 0, 0, 0, 10000 },
	    { 0, 0, 0, 10000 },
	    { 0, 0, 0, 3000 },
	    { 0, 0, 0, 12000 },
	    { 0, 0, 0, 600 },
	    { 0, 0, 0, 10000 }, {
	    },
	    // 12x4
	    { 13311,    1430,     8321,     23062 },
	    {    89,    1663,     2019,      3771 },
	    {     0,    1622,    2019,     3641 },
	    {     0,     234,      798,     1032 },
	    {     0,    2921,     3225,     6146 },
	    {     0,    3923,    1578,     5501 },
	    {     0,    1778,    1550,     3328 },
	    {     0,    6410,    5676,    12086 },
	    {     0,     346,     992,     1338 },
	    {     0,    1014,     753,     1767 },
	    {     0,       0,    3887,     3887 },
	    {     0,       0,    3049,     3049 },
	},
    };

    Revenue_and_Expense Base_Actual = {             // Base_Actual is actual at year end
	0.0, 0.0, {
	    // 10x4
	    { 0, 0, 0, 20000 },
	    { 0, 0, 0, -3000 },
	    { 0, 0, 0, 17000 },
	    { 0, 3775, 2190, 5965 },
	    { 0, 0, 0, 9928 },
	    { 0, 0, 0, 10000 },
	    { 0, 0, 0, 3013 },
	    { 0, 0, 0, 12003 },
	    { 0, 0, 0, 600 },
	    { 0, 0, 0, 10000 }, {
	    },
	    // 12x4
	    { 13307,    1430,     8321,     23058 },
	    {    93,    1663,     2019,      3775 },
	    {     0,    1600,    2100,     3700 },
	    {     0,     230,      798,     1028 },
	    {     0,    3000,     3225,     6225 },
	    {     0,    3923,    1578,     5501 },
	    {     0,    1778,    1550,     3328 },
	    {     0,    6500,    5600,    12100 },
	    {     0,     346,     992,     1338 },
	    {     0,     998,     753,     1751 },
	    {     0,       0,    3887,     3887 },
	    {     0,       0,    3049,     3049 },
	},
    };

    float PL_Policy_Array_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT][4] = { {
	// revenue
	0.02f,   0.00f, 0.05f,  2
    },
									       { 0.01f, -0.05f, 0.05f, 2 },
									       { 0.05f,  0.04f, 0.10f, 2 },
									       { 0.583f, 0.20f, 0.70f, 2 }, {
										   // expense
										   0.01f, -0.05f, 0.05f, 2
									       },
									       { 0.01f, -0.05f, 0.05f, 2 },
									       { 0.02f, -0.05f, 0.05f, 2 },
									       { 0.02f, -0.05f, 0.05f, 2 },
									       { 0.01f, -0.03f, 0.03f, 2 },
    };
    float PL_Policy_Array_S2[COST_RISE_POLICY_COUNT][4] = {
	{ 0.01f, -0.05f, 0.05f, 2 },
	{ 0.00f, -0.05f, 0.05f, 2 },
	{ 0.01f, -0.05f, 0.05f, 2 },
	{ 0.00f, -0.05f, 0.05f, 2 },
	{ 0.01f, -0.05f, 0.05f, 2 },
	{ 0.00f, -0.05f, 0.05f, 2 },
	{ 0.01f, -0.05f, 0.05f, 2 },
	{-0.02f, -0.05f, 0.05f, 2 },
	{ 0.00f, -0.05f, 0.05f, 2 },
	{-0.02f, -0.05f, 0.05f, 2 },
    };

    double inflation = 0.0/100.0;
    double Base_tuition_rate = 4000.0;
    double Base_enrollment = 5000;
    double Target_enrollment = 5200;
    double Actual_enrollment = 5100;
    double Base_endowmentMV = 220000.0;
    double Base_OpResBalance = 21000.0;
    float ST_int_rate_01Aug = 0.03f;
    float ST_int_rate_31Aug = 0.031f;
    double New_Debt_service = 4500.0;
#endif

    // calc Surplus_Deficit
    Base_Budget.calc_Surplus_Deficit();
    Base_Actual.calc_Surplus_Deficit();

    // output
    Revenue_and_Expense Trial_Budget, Final_Budget;
    // result of stage 1
    float Result_Vector_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT];
    float Result_Vector_S2[COST_RISE_POLICY_COUNT]; // result of stage 2

    Optimize_policy_ver2( Trial_Budget, Final_Budget,
			  Result_Vector_S1, Result_Vector_S2,
			  1,                                            // end at stage 1
			  Base_Budget, Base_Actual,
			  PL_Policy_Array_S1, PL_Policy_Array_S2,
			  inflation, Base_tuition_rate, Base_enrollment, Target_enrollment,
			  Actual_enrollment, Base_endowmentMV, Base_OpResBalance, ST_int_rate_01Aug,
			  ST_int_rate_31Aug, New_Debt_service );

    for (i=0; i<REVENUE_POLICY_COUNT; i++)
	revenue_policy_array[i].result_value = Result_Vector_S1[i] * 100.0;
    for (i=0; i<EXPENSE_POLICY_COUNT; i++)
	expense_policy_array[i].result_value = Result_Vector_S1[REVENUE_POLICY_COUNT+i] * 100.0;

    // ignore Trial_Budget, Final_Budget and Result_Vector_S2
                                       // remove stage1_revenue, stage1_expense, stage2_expense

    //------- adjust budget --------------//

    DEBUG_LOG("finance.opt_policy_1(): end");

    return true;
}

//------- End of function Finance::optimize_policy_1 -------//

//------ Begin of function Finance::init_optimize_policy_2 ------//
//!
//! init the table "Strucaltural equations (usedin Stage1)" in ResAlloc.ex.xls file
//! for optimize_policy_1()
//!
void Finance::init_optimize_policy_2() {
    // temp put here
    prior_dept_fac_exp = expense_array[AC_ACADEMIC_DEPARTMENTS].faculty;
}

//------ End of function Finance::init_optimize_policy_2 ------//

//------ Begin of function Finance::optimize_policy_2 ------//
//!
//! This function is for stage 2 optimization of financial policies.
//! See file ResAlloc.Equation for details.
//!
bool Finance::optimize_policy_2() {
    // to calculate FinancePolicy::result_value based on
    // other vars in the struct FinancePolicy.

    int i;

#ifdef DEBUG_VC
    String s;

    DEBUG_LOG("---- INPUT TO Finance::optimize_policy_2(): l,t,u,w ----");
    for (i=0; i<COST_RISE_POLICY_COUNT; i++) {
	FinancePolicy& policy = cost_rise_policy_array[i];
	sprintf(s, "%.3f, %.3f, %.3f, %.3f,",
		policy.lower_bound, policy.target_value, policy.upper_bound, policy.import_weight);

	DEBUG_LOG(s);
    }
#endif

    init_optimize_policy_2();

#ifndef _DEBUG_STAGE2

    // allocation stage 1
    // set input parameter
    Revenue_and_Expense Base_Budget;                // Base_Budget is projected actual on 1st Aug
    Revenue_and_Expense Base_Actual;                // Base_Actual is actual at year end
    // slider in stage 1 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
    float PL_Policy_Array_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT][4];
    // slider in stage 2 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
    float PL_Policy_Array_S2[COST_RISE_POLICY_COUNT][4];
    double inflation, Base_tuition_rate;
    double Base_enrollment, Target_enrollment, Actual_enrollment;
    double Base_endowmentMV, Base_OpResBalance;
    float ST_int_rate_01Aug, ST_int_rate_31Aug;
    double New_Debt_service;

    input_Optimize_policy_ver2(
	Base_Budget,                                  // Base_Budget is projected actual on 1st Aug
	Base_Actual,                                  // Base_Actual is actual at year end
	PL_Policy_Array_S1,                           // slider in stage 1 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
	PL_Policy_Array_S2,                           // slider in stage 2 optimization; [0] is target, [1] is lower bound [2] is upper bound [3] is preference (weight)
	inflation, Base_tuition_rate,
	Base_enrollment, Target_enrollment, Actual_enrollment,
	Base_endowmentMV, Base_OpResBalance,
	ST_int_rate_01Aug, ST_int_rate_31Aug,
	New_Debt_service );

#else

    Revenue_and_Expense Base_Budget = {             // Base_Budget is projected actual on 1st Aug
	0.0, 0.0, {
	    // 10x4
	    { 0, 0, 0, 20000 },
	    { 0, 0, 0, -3000 },
	    { 0, 0, 0, 17000 },
	    { 0, 3771, 2200, 5971 },
	    { 0, 0, 0, 10000 },
	    { 0, 0, 0, 10000 },
	    { 0, 0, 0, 3000 },
	    { 0, 0, 0, 12000 },
	    { 0, 0, 0, 600 },
	    { 0, 0, 0, 10000 }, {
	    },
	    // 12x4
	    { 13311,    1430,     8321,     23062 },
	    {    89,    1663,     2019,      3771 },
	    {     0,    1622,    2019,     3641 },
	    {     0,     234,      798,     1032 },
	    {     0,    2921,     3225,     6146 },
	    {     0,    3923,    1578,     5501 },
	    {     0,    1778,    1550,     3328 },
	    {     0,    6410,    5676,    12086 },
	    {     0,     346,     992,     1338 },
	    {     0,    1014,     753,     1767 },
	    {     0,       0,    3887,     3887 },
	    {     0,       0,    3049,     3049 },
	},
    };

    Revenue_and_Expense Base_Actual = {             // Base_Actual is actual at year end
	0.0, 0.0, {
	    // 10x4
	    { 0, 0, 0, 20000 },
	    { 0, 0, 0, -3000 },
	    { 0, 0, 0, 17000 },
	    { 0, 3775, 2190, 5965 },
	    { 0, 0, 0, 9928 },
	    { 0, 0, 0, 10000 },
	    { 0, 0, 0, 3013 },
	    { 0, 0, 0, 12003 },
	    { 0, 0, 0, 600 },
	    { 0, 0, 0, 10000 }, {
	    },
	    // 12x4
	    { 13307,    1430,     8321,     23058 },
	    {    93,    1663,     2019,      3775 },
	    {     0,    1600,    2100,     3700 },
	    {     0,     230,      798,     1028 },
	    {     0,    3000,     3225,     6225 },
	    {     0,    3923,    1578,     5501 },
	    {     0,    1778,    1550,     3328 },
	    {     0,    6500,    5600,    12100 },
	    {     0,     346,     992,     1338 },
	    {     0,     998,     753,     1751 },
	    {     0,       0,    3887,     3887 },
	    {     0,       0,    3049,     3049 },
	},
    };

    float PL_Policy_Array_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT][4] = { {
	// revenue
	0.02f,   0.00f, 0.05f,  2
    },
									       { 0.01f, -0.05f, 0.05f, 2 },
									       { 0.05f,  0.04f, 0.10f, 2 },
									       { 0.583f, 0.20f, 0.70f, 2 }, {
										   // expense
										   0.01f, -0.05f, 0.05f, 2
									       },
									       { 0.01f, -0.05f, 0.05f, 2 },
									       { 0.02f, -0.05f, 0.05f, 2 },
									       { 0.02f, -0.05f, 0.05f, 2 },
									       { 0.01f, -0.03f, 0.03f, 2 },
    };
    float PL_Policy_Array_S2[COST_RISE_POLICY_COUNT][4] = {
	{ 0.01f, -0.05f, 0.05f, 2 },
	{ 0.00f, -0.05f, 0.05f, 2 },
	{ 0.01f, -0.05f, 0.05f, 2 },
	{ 0.00f, -0.05f, 0.05f, 2 },
	{ 0.01f, -0.05f, 0.05f, 2 },
	{ 0.00f, -0.05f, 0.05f, 2 },
	{ 0.01f, -0.05f, 0.05f, 2 },
	{-0.02f, -0.05f, 0.05f, 2 },
	{ 0.00f, -0.05f, 0.05f, 2 },
	{-0.02f, -0.05f, 0.05f, 2 },
    };

    double inflation = 0.0/100.0;
    double Base_tuition_rate = 4000.0;
    double Base_enrollment = 5000;
    double Target_enrollment = 5200;
    double Actual_enrollment = 5100;
    double Base_endowmentMV = 220000.0;
    double Base_OpResBalance = 21000.0;
    float ST_int_rate_01Aug = 0.03f;
    float ST_int_rate_31Aug = 0.031f;
    double New_Debt_service = 4500.0;
#endif

    // calc Surplus_Deficit
    Base_Budget.calc_Surplus_Deficit();
    Base_Actual.calc_Surplus_Deficit();

    // output
    Revenue_and_Expense Trial_Budget, Final_Budget;
    // result of stage 1
    float Result_Vector_S1[REVENUE_POLICY_COUNT+EXPENSE_POLICY_COUNT];
    float Result_Vector_S2[COST_RISE_POLICY_COUNT]; // result of stage 2

    Optimize_policy_ver2( Trial_Budget, Final_Budget,
			  Result_Vector_S1, Result_Vector_S2,
			  2,                                            // end at stage 2
			  Base_Budget, Base_Actual,
			  PL_Policy_Array_S1, PL_Policy_Array_S2,
			  inflation, Base_tuition_rate, Base_enrollment, Target_enrollment,
			  Actual_enrollment, Base_endowmentMV, Base_OpResBalance, ST_int_rate_01Aug,
			  ST_int_rate_31Aug, New_Debt_service );

    // ignore Result_Vector_S1
    // Result_Vector_S2

    // -------------- assign the result to array ---------------- //
    //
    for (i=0; i<COST_RISE_POLICY_COUNT; i++) {
	//captial campaign
	if (i==PL_INST_ADVANCEMENT && info.game_year<=chance_event.capital_campaign_end_year) {

	    expense_array[AC_INST_ADVANCEMENT].total*= chance_event.capital_campaign_lock_ins_pect;
	    projected_expense_array[AC_INST_ADVANCEMENT].change_budget_year.total=(chance_event.capital_campaign_lock_ins_pect-1.0f);
	    cost_rise_policy_array[PL_INST_ADVANCEMENT].result_value = chance_event.capital_campaign_lock_ins_pect;
	}
	else {
	    cost_rise_policy_array[i].result_value = Result_Vector_S2[i] * 100.0;
	}
    }

    // Trial_Budget, Final_Budget

    // set budget_revenue_array, and budget_expense_array
    // instead of in calc_budget_report()
    for( i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
	if( i == AC_SPONSORED_RESEARCH_REVENUE ) {
	    budget_revenue_array[i].direct = Final_Budget.Revenue_Array[i][1];
	    budget_revenue_array[i].indirect = Final_Budget.Revenue_Array[i][2];
	}
	else {
	    budget_revenue_array[i].direct = 0.0f;
	    budget_revenue_array[i].indirect = 0.0f;
	}
	budget_revenue_array[i].total = Final_Budget.Revenue_Array[i][3];
    }
    for( i = 0; i < EXPENSE_ITEM_COUNT; ++i ) {
	budget_expense_array[i].faculty = Final_Budget.Expense_Array[i][0];
	budget_expense_array[i].staff = Final_Budget.Expense_Array[i][1];
	budget_expense_array[i].other = Final_Budget.Expense_Array[i][2];
	budget_expense_array[i].total = Final_Budget.Expense_Array[i][3];
    }

    return true;
}

//------- End of function Finance::optimize_policy_2 -------//

//------ Begin of function Finance::calc_budget_report ------//
//!
//!	Player's "Year-End Financial Report"
//!
void Finance::calc_budget_report() {
    int i,j;
    // remember to set tuition_rate,

    // tuition_rate = int( tuition_rate * (1.0 + stage1_revenue[S1_GROSS_TUITION_INCOME].computed_growthrate/100.0) );	// 990609
    // resolve : stage1_revenue[S1_GROSS_TUITION_INCOME].computed_growthrate = inflation_rate + revenue_policy_array[PL_TUITION_RATE_GROWTH].result_value;
    tuition_rate = int( tuition_rate * (1.0 + (inflation_rate + revenue_policy_array[PL_TUITION_RATE_GROWTH].result_value)/100.0) );

    // already set in optimize_report_2()
    // budget_revenue_array
    // budget_expense_array

    // budget_year.asset_array[AC_ENDOWMENT] = ?
    transfer_operating_reserve = 0;
    transfer_capital_reserve = 0;
    transfer_quasi = 0;

    // code where read finance.stage2_expense :
    // ofaci_st.cpp
    // float buget_monthly = float(finance.stage2_expense[S2_OPERATIONS_AND_MAINTENANCE].new_value / 12.0f);
    // stage2_expense[S2_OPERATIONS_AND_MAINTENANCE].new_value = (stage2_expense[S2_OPERATIONS_AND_MAINTENANCE].change_value + stage2_expense[S2_OPERATIONS_AND_MAINTENANCE].base_value );
    // stage2_expense[S2_OPERATIONS_AND_MAINTENANCE].change_value = stage2_expense[S2_OPERATIONS_AND_MAINTENANCE].base_value * stage2_expense[S2_OPERATIONS_AND_MAINTENANCE].computed_growthrate / 100.0;
    // stage2_expense[S2_OPERATIONS_AND_MAINTENANCE].base_value = cons1 * expense_array[AC_OPERATIONS_AND_MAINTENANCE].staff + cons2 * expense_array[AC_OPERATIONS_AND_MAINTENANCE].other
    // cons1 = (1 + inflation_rate/100.0 + expense_policy_array[PL_STAFF_SALARY_INCREASES].result_value / 100.0)
    // cons2 = (1 + inflation_rate/100.0 + other_expense_growth/100.0)
    // stage2_expense[S2_OPERATIONS_AND_MAINTENANCE].computed_growthrate = cost_rise_policy_array[AC_OPERATIONS_AND_MAINTENANCE].result_value;

    // ofin_s3.cpp
    //double growthRate = finance.stage2_expense[S2_DEPARTMENTAL_FACULTY].computed_growthrate / 100;		// = F203/100
    //double newValue = finance.stage2_expense[S2_DEPARTMENTAL_FACULTY].new_value;
    // stage2_expense[S2_DEPARTMENTAL_FACULTY].computed_growthrate = cost_rise_policy_array[AC_ACADEMIC_DEPARTMENTS].result_value;
    //stage2_expense[S2_DEPARTMENTAL_FACULTY].new_value
    //		= stage2_expense[S2_DEPARTMENTAL_FACULTY].dept_faculty_new_price
    //		+ stage2_expense[S2_DEPARTMENTAL_FACULTY].change_value
    //		- stage1_revenue[S1_DIRECT_SPONSORED_RESEARCH].new_value;
    // stage2_expense[S2_DEPARTMENTAL_FACULTY].dept_faculty_new_price
    //		= prior_dept_fac_exp
    //		* (1+inflation_rate/100.0+expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value/100.0)
    //stage2_expense[S2_DEPARTMENTAL_FACULTY].change_value =
    //	stage2_expense[S2_DEPARTMENTAL_FACULTY].dept_faculty_new_price
    //	*stage2_expense[S2_DEPARTMENTAL_FACULTY].computed_growthrate / 100.0
    //	+stage1_revenue[S1_DIRECT_SPONSORED_RESEARCH].new_value;
    // no need find stage1_revenue[S1_DIRECT_SPONSORED_RESEARCH].new_value, will be cancel out

    double cons2, cons1;

    //
    //----- complete the budget report -----//
    //

    budget_calc_total();
    budget_calc_percent();
    err_when(budget_total_expense.total<=0);
    budget_year.surplus_deficit = budget_total_revenue.total - budget_total_expense.total;

    //--- run the following function to make sure that the actual surplus/deficit matches the optimization result ---//

    correct_surplus_deficit();

    //----- balance sheet -----//
    //

    cons2 = 1.0 + inflation_rate/100.0 + investment_office.expected_real_annual_return_rate/100.0;
    cons1 = 1.0 + inflation_rate/100.0 + short_term_debt_interest/100.0;

    budget_year.asset_array[AC_OPERATING_RESERVE]   //=D50*(1+C72+C73)+F263
	= last_year.asset_array[AC_OPERATING_RESERVE] * cons2 + budget_year.surplus_deficit;

    double ratio = last_year.asset_array[AC_QUASI] / (last_year.asset_array[AC_QUASI]+last_year.asset_array[AC_ENDOWMENT]);

    budget_year.asset_array[AC_ENDOWMENT]           //=D51*(1+C72+G72)-F241		// = D51-F241 by fred 1023
	= last_year.asset_array[AC_ENDOWMENT]   + (investment_office.endowment_change_year_to_date
						   // 990527
						   - budget_revenue_array[AC_ENDOWMENT_SPENDING].total)*(1-ratio);
    + development_office.this_year.endowment;       // 990608
    budget_year.asset_array[AC_QUASI]               //=D51*(1+C72+G72)-F241		// = D51-F241 by fred 1023
	= last_year.asset_array[AC_QUASI] + (investment_office.endowment_change_year_to_date
					     // 990527
					     - budget_revenue_array[AC_ENDOWMENT_SPENDING].total)*ratio;
    + development_office.this_year.endowment;       // 990608

    transfer_operating_reserve = 0;
    transfer_capital_reserve = 0;
    transfer_quasi = 0;

    //vars not updated here
    //budget_year.asset_array[AC_BUILDINGS]
    //budget_year.asset_array[AC_CAPITAL_RESERVE]
    //budget_year.liability_array[AC_GENERAL_PLANT_DEBT]

    //budget_year.asset_array[AC_RESIDENCE]
    //budget_year.liability_array[AC_RESIDENCE_HALL_DEBT]

    finance.budget_year.asset_array[AC_ASSET_TOTAL] = 0 ;
    for( i=0 ; i<ASSET_ITEM_COUNT-1 ; i++ ) {
	finance.budget_year.asset_array[AC_ASSET_TOTAL] += finance.budget_year.asset_array[i];
    }

    finance.budget_year.liability_array[AC_LIABILITY_TOTAL] = 0 ;
    for( i=0 ; i<LIABILITY_ITEM_COUNT-1 ; i++ ) {
	finance.budget_year.liability_array[AC_LIABILITY_TOTAL] += finance.budget_year.liability_array[i];
    }

    // update budget_change w.r.t. last year
    // calculated yearly after optimization

    //
    // -------------- Step 2. -------------- change_last_year
    //

    budget_calc_change();
}

//------- End of function Finance::calc_budget_report -------//

//------ Begin of function Finance::correct_surplus_deficit ------//
//!
void Finance::correct_surplus_deficit() {
    //----------------------------//

    budget_calc_total();
    budget_calc_percent();
    budget_year.surplus_deficit = budget_total_revenue.total - budget_total_expense.total;

}

//------- End of function Finance::correct_surplus_deficit -------//

//------ Begin of function Finance::budget_calc_change ------//
//!
//! Calculate Budget to Last Year Actual change percentage.
//!
void Finance::budget_calc_change() {
    RevenueItemChange* projRev;
    ExpenseItemChange* projExp;
    double temp;

    for ( int i=0; i<REVENUE_ITEM_COUNT; i++) {
	//----- calculate change ------//

	temp = revenue_array[i].direct;
	projRev = &projected_revenue_array[i];
	projRev->change_budget_year.direct
	    = calc_change( budget_revenue_array[i].direct, temp );

	temp = revenue_array[i].indirect;
	projRev = &projected_revenue_array[i];
	projRev->change_budget_year.indirect
	    = calc_change( budget_revenue_array[i].indirect, temp );

	temp = revenue_array[i].total;
	projRev = &projected_revenue_array[i];
	projRev->change_budget_year.total
	    = calc_change( budget_revenue_array[i].total, temp );
    }

    for ( int i=0; i<EXPENSE_ITEM_COUNT; i++) {
	//----- calculate change ------//

	temp = expense_array[i].faculty;
	projExp = &projected_expense_array[i];
	projExp->change_budget_year.faculty
	    = calc_change( budget_expense_array[i].faculty, temp );

	temp = expense_array[i].staff;
	projExp = &projected_expense_array[i];
	projExp->change_budget_year.staff
	    = calc_change( budget_expense_array[i].staff, temp );

	temp = expense_array[i].other;
	projExp = &projected_expense_array[i];
	projExp->change_budget_year.other
	    = calc_change( budget_expense_array[i].other, temp );

	temp = expense_array[i].total;
	projExp = &projected_expense_array[i];
	projExp->change_budget_year.total
	    = calc_change( budget_expense_array[i].total, temp );
    }
}

//------- End of function Finance::budget_calc_change -------//

//---------- Begin of function Finance::budget_calc_total -----------//
//!
void Finance::budget_calc_total() {
    //------- calculate the total revenue ---------//

    memset( &budget_total_revenue, 0, sizeof(total_revenue) );

    //----- calculate the total of one row ------//
    int i = AC_SPONSORED_RESEARCH_REVENUE;
    budget_revenue_array[i].total = budget_revenue_array[i].direct + budget_revenue_array[i].indirect;

    for( i=AC_NET_TUITION_INCOME ; i<REVENUE_ITEM_COUNT ; i++ ) {
	//------ calculate totals of all rows -------//

	budget_total_revenue.direct   += budget_revenue_array[i].direct;
	budget_total_revenue.indirect += budget_revenue_array[i].indirect;
	budget_total_revenue.total    += budget_revenue_array[i].total;
    }

    //------- calculate the total expense ---------//

    memset( &budget_total_expense, 0, sizeof(budget_total_expense) );

    //## chea 250899 bughere
    for( i=0 ; i<OPERATING_EXPENSE_ITEM_COUNT ; i++ ) {
	//----- calculate the total of one row ------//
	budget_expense_array[i].total = budget_expense_array[i].faculty +
	    budget_expense_array[i].staff + budget_expense_array[i].other;

	//------ calculate totals of all rows -------//

	budget_total_expense.faculty += budget_expense_array[i].faculty;
	budget_total_expense.staff   += budget_expense_array[i].staff;
	budget_total_expense.other   += budget_expense_array[i].other;
	budget_total_expense.total   += budget_expense_array[i].total;
    }

    //------- calculate the total operating expense ---------//
    memcpy( &budget_total_operating_expense, &budget_total_expense, sizeof(budget_total_expense) );

    for( i=OPERATING_EXPENSE_ITEM_COUNT ; i<TOTAL_EXPENSE_ITEM_COUNT ; i++ ) {
	//----- calculate the total of one row ------//

	budget_expense_array[i].total = budget_expense_array[i].faculty +
	    budget_expense_array[i].staff + budget_expense_array[i].other;

	//------ calculate totals of all rows -------//

	budget_total_expense.faculty += budget_expense_array[i].faculty;
	budget_total_expense.staff   += budget_expense_array[i].staff;
	budget_total_expense.other   += budget_expense_array[i].other;
	budget_total_expense.total   += budget_expense_array[i].total;
    }

    err_when(budget_total_expense.total<=0);        //1228

    //------ update surplus / deficit -----------//

    budget_year.surplus_deficit = budget_total_revenue.total - budget_total_expense.total;
}

//----------- End of function Finance::budget_calc_total ------------//

//---------- Begin of function Finance::budget_calc_percent -----------//
//!
void Finance::budget_calc_percent() {
    //------- calculate the total revenue ---------//

    for( int i=AC_NET_TUITION_INCOME; i<REVENUE_ITEM_COUNT ; i++ ) {
	budget_revenue_array[i].percent = 100.0f * budget_revenue_array[i].total /
	    budget_total_revenue.total;
    }

    //------- calculate the total expense ---------//

    for( int i=0 ; i<EXPENSE_ITEM_COUNT ; i++ ) {
	budget_expense_array[i].percent = 100.0f * budget_expense_array[i].total /
	    budget_total_expense.total;
    }
}

//----------- End of function Finance::budget_calc_percent ------------//

//------ Begin of function Finance::get_research_overhead_rate ------//
//!
float Finance::get_research_overhead_rate() {
    return float(revenue_policy_array[PL_INDIRECT_COST_RATE].result_value);
}

//------- End of function Finance::get_research_overhead_rate -------//

// ##### Begin Marco ###### //
//------- Begin of function Finance::save_initial_value -------//
void Finance::save_initial_value() {
    for (int i = 0; i < REVENUE_ITEM_COUNT; i++)
	initial_revenue_array[i] = revenue_array[i];

    for (int j = 0; j < EXPENSE_ITEM_COUNT; j++)
	initial_expense_array[j] = expense_array[j];

    //	initial_total_revenue = total_revenue;
    initial_total_operating_expense = total_operating_expense;
    initial_total_expense = total_expense;
    initial_last_year = last_year;
    //	initial_projected_total_expense = projected_total_expense;

    // new //
    initial_total_revenue_total = total_revenue.total;

    initial_faculty_salary_increase = expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value;
    initial_tuition_increase = finance.revenue_policy_array[PL_TUITION_RATE_GROWTH].result_value;
}

//------- End of function Finance::save_initial_value -------//
// ##### End Marco ###### //
