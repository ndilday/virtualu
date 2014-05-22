//Filename    : OFINANCE.h
//Description : Finance Class Declaration
//Owner       : Fred

//IMPORTANT NOTE: most of the financial-model-related values are in $000 format!

#ifndef __OFINANCE_H
#define __OFINANCE_H

#include <OFINANC2.H>
#include <OHISTORY.H>

//------- define report types -------//

enum { REPORT_TYPE_COUNT = 4 };
enum { REPORT_YEAR_COUNT = 4 };
enum { REPORT_OPT_COUNT = 3 };

enum { REPORT_TYPE_REVENUE, REPORT_TYPE_BALANCE, REPORT_TYPE_OPTIMIZATION, REPORT_TYPE_DETAIL_REN, REPORT_TYPE_BUDGET_VS_ACTUAL};
enum { REPORT_OPT_REVENUE, REPORT_OPT_FUNCTION, REPORT_OPT_FACULTY};

enum { REPORT_YEAR_THREE_YEARS, REPORT_YEAR_PREV, REPORT_YEAR_THIS, REPORT_YEAR_NEXT };

enum { POSITIVE = 1, NEGATIVE = -1, NO_SIGN = 0 };

//----- define asset items in the balance sheet -----//

enum { ASSET_ITEM_COUNT = 7 };

enum {
    AC_OPERATING_RESERVE,
    AC_CAPITAL_RESERVE,                             // also called "plant reserve"
    AC_ENDOWMENT,                                   // also called "endowment market value"
    AC_BUILDINGS,                                   // also called "replacement cost of facilities" or "plant value"
    AC_RESIDENCE,
    AC_QUASI,

    AC_ASSET_TOTAL,
};

//--- define liability items in the balance sheet ---//

enum { LIABILITY_ITEM_COUNT = 3 };

enum {
    AC_GENERAL_PLANT_DEBT,
    AC_RESIDENCE_HALL_DEBT,

    AC_LIABILITY_TOTAL,

    AC_BANK_BORROWING = 2,
};

//-------- define revenue items ---------//

enum { REVENUE_ITEM_COUNT = 10 };

enum {
    AC_GROSS_TUITION_INCOME,
    AC_FINANCIAL_AID,
    AC_NET_TUITION_INCOME,                          // fred 0603
    AC_SPONSORED_RESEARCH_REVENUE,
    AC_GIFTS,
    AC_ENDOWMENT_SPENDING,
    AC_INTERCOLLEGIATE_ATHLETICS,
    AC_OTHER_OPERATING_INCOME,
    AC_INTEREST_EARNED_OR_PAID,
    AC_STATE_APPROPRIATION,                         // fred 1106
};

enum {
    OPERATING_EXPENSE_ITEM_COUNT = 10,
    TOTAL_EXPENSE_ITEM_COUNT = OPERATING_EXPENSE_ITEM_COUNT + 2
};

//-------- define expense items ---------//

enum { EXPENSE_ITEM_COUNT = TOTAL_EXPENSE_ITEM_COUNT };

enum {
    AC_ACADEMIC_DEPARTMENTS,
    AC_SPONSORED_RESEARCH_EXPENSE,
    AC_LIBRARIES=2,
    AC_IT_RESOURCES=3,
    AC_STUDENT_LIFE,
    AC_ATHLETICS,
    AC_INST_ADVANCEMENT,
    AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE,
    AC_OPERATIONS_AND_MAINTENANCE,
    AC_ENROLLMENT_MANAGEMENT,
    AC_SERVICE_ON_GENERAL_PLANT_DEBT,               // other uses of funds
    AC_TRANSFER_TO_CAPITAL_RESERVE,
};

//-------- define student policies -------//

enum { STUDENT_POLICY_COUNT = 6 };

enum {
    PL_TRADITIONAL_UG,
    PL_NON_TRADITIONAL_UG,
    PL_MASTERS_PROFESSIONAL,
    PL_DOCTORAL,
    PL_DISTANCE_LEARN,
    PL_TOTAL,
};

//-------- define revenue policies -------//

enum { REVENUE_POLICY_COUNT = 4 };

enum {
    PL_TUITION_RATE_GROWTH,
    PL_FINANCIAL_AID,
    PL_ENDOWMENT_SPENDING_RATE,
    PL_INDIRECT_COST_RATE,                          // = Research overhead rate // total = direct * (1+overhead)
};

//-------- define expense policies -------//

enum { EXPENSE_POLICY_COUNT = 5 };

enum {
    PL_FACULTY_SALARY_INCREASES,
    PL_STAFF_SALARY_INCREASES,
    //PL_OTHER_COST_RISE_PROVISION,
    PL_BUDGET_ADJUSTMENT,                           // = NET_OPERATING_BUDGET_GROWTH		// BUG ?
    PL_REAL_TRANSFER_TO_PLANT_GROWTH,
    PL_SURPLUS_DEFICIT,
};

//--- define policy variables for allocating net operating budget growth ---//
enum { COST_RISE_POLICY_COUNT = 10 };

enum {
    PL_FACULTY_FTE,
    PL_NON_FACULTY_DEPARTMENTAL_EXPENSE,
    PL_LIBRARIES = 2,
    PL_INFO_TECH_RESOURCES = 3,
    PL_STUDENT_LIFE,
    PL_ATHLETICS,
    PL_INST_ADVANCEMENT,
    PL_ADMINISTRATION,
    PL_OPERATIONS_AND_MAINTENANCE,
    PL_ENROLLMENT_MANAGEMENT,
};

//------- Define constants for budget change (BC)------//

enum {
    CONSIDER_NEXT_YEAR,
    ANNOUNCE_NEXT_YEAR,
    IMPLEMENT_NOW,
};

//------- Define struct RevenueItem ------//

//!Structure for itemizing revenue for the current year. Contains direct,
//!indirect, percent, and total revenues, as stipulated by the finance
//!model.
struct RevenueItem {
    double   direct;
    double indirect;
    double total;
    double percent;
};

//------- Define struct ExpenseItem ------//

//!Structure for itemizing expenses for the current year.
struct ExpenseItem {
    double   faculty;
    double staff;
    double other;
    double total;
    double percent;
};

//------ Define struct StudentPolicy ------//
//!Structure used by Finance class to track students, brokend down by
//!level, continuing to attend and estimate how many will be attending next
//!year.
struct StudentPolicy {                            // for estimate number of each of the 5 groups of student
    //int	last_year;
    int  continue_next_year;
    //int	intake_target;			// inputed by the player
    int  estimate_next_year;
};

//------ Define struct FinancePolicy ------//

//!
struct FinancePolicy {                            // for revenue policies, expense and surplus/deficit policies
    // inputed by the player
    double target_value;
    double lower_bound;
    double upper_bound;
    double import_weight;
    bool   required_flag;                           // = true if "implemented" is selected by player
    double result_value;

    char   applied_flag;                            // = P_NONE, P_CONSIDER, P_PROMOISE, P_IMPLEMENT,

    //--- model-determined constants; unchanged after battle start ---//

    double lower_limit;                             // limit of lower bound
    double upper_limit;                             // limit of upper bound

    //----- save last for restore -------//

    double last_target_value;
    double  last_upper_bound;
    double  last_lower_bound;

    double  last_import_weight;                     // these two are only used fore restore when applied_flag > none
    bool    last_required_flag;

    //--- for penalty of change of financial policy ---//
    //double	last_target_change;

    char   penalty_direction;                       // = { POSITIVE, NEGATIVE }
    int    max_penalty1;                            // 33%
    int    max_penalty2;                            // 33%

    double penalty_multiplier1;
    double penalty_multiplier2;

    //--------- functions --------//

    void   reset_change(bool resetAll=false);
    void   bound_correction();
    void   set_constraint_policy(int saveLast=1);
    void    reset_constraint_policy();
};

//!Structure which stores user defined parameters governing the hiring
//!policy as required by the model.
struct HiringPolicy {                             // for stage 3 optimization
    int  upper_bound;
    float  weight;                                  // = OPT_LOW_WEIGHT, OPT_MID_WEIGHT, OPT_HIGH_WEIGHT

    int  target_value;
    int  result_value;

    int  fac_count;
    float  avg_salary;                              // $000
};

//----- Define struct RevenueItemChange & ExpenseItemChange------//
// see "design doc June 5.doc" p.11

//!
struct RevenueItemChange {
    // values stored for report only; not for further calculation
    RevenueItem  change_last_year;                  // "change" of this year relative to last year.
    RevenueItem  change_next_year;                  // "change" of next year relative to this year.

    // values stored for engine and report
    //ExpenseItem	last_year;				// values stored in revenue_array and expense array instead
    RevenueItem  this_year;                         // projected value of revenue of the current financial year
    RevenueItem  next_year;                         // projected value of revenue of the next financial year

    RevenueItem last_month;                         // acutal value of last month
    RevenueItem  year_to_date;                      // for calc this_year
    RevenueItem  change_budget_year;                // "change" of budget year relative to the last year.
    RevenueItem month_start;                        // value this_year at month start
};

//!Structure for tracking changes in ExpenseItems in the past and future
//!for reporting and calculating.
struct ExpenseItemChange {
    // values stored for report only; not for further calculation
    ExpenseItem  change_last_year;                  //
    ExpenseItem  change_next_year;                  //

    // values stored for engine and report
    //ExpenseItem	last_year;				// values stored in revenue_array and expense array instead
    ExpenseItem  this_year;                         // projected value of revenue of the current financial year
    ExpenseItem  next_year;                         // projected value of revenue of the next financial year

    ExpenseItem last_month;                         // acutal value of last month
    ExpenseItem  year_to_date;                      // for calc this_year
    ExpenseItem  change_budget_year;                // values for calc next_year from budget_*_array
    ExpenseItem month_start;                        // value this_year at month start
};

//----- Define struct YearReport -----//
//!Structure used by Finance class containing assets, liabilities and
//!surplus/deficit.
struct YearReport {
    double surplus_deficit;

    //---- assets and liabilities ------//
    double asset_array[ASSET_ITEM_COUNT];
    double liability_array[LIABILITY_ITEM_COUNT];
};

//--------- Define class Finance ---------//

//!Class which implements the financial model described in Tech Doc 2.2,
//!section 3. Assets, endowments, loans, budgets, expenses, and revenue are
//!all represented here and methods for calculating, projecting, and
//!reporting are provided.
class Finance {
public:

    char     fiscal_year_start_month;
    char     fiscal_year_end_month;
    int      report_financial_recno;

    //------- revenues and expenses ---------//

    // 1 of 3: last year: displayed on report
    RevenueItem  revenue_array[REVENUE_ITEM_COUNT];
    ExpenseItem  expense_array[EXPENSE_ITEM_COUNT];

    ExpenseItem net_distance_learning_expense;
    float     transfer_operating_reserve;
    float     transfer_capital_reserve;
    float     transfer_quasi;

    float     endowment_quasi_ratio;              // store the ratio of quasi to total endowment(endowment+quasi)

    RevenueItem  total_revenue;
    ExpenseItem total_operating_expense;          // for optimize_policy_1()
    ExpenseItem total_expense;

    // 2 of 3: budget this year: NOT displayed on report
    RevenueItem  budget_revenue_array[REVENUE_ITEM_COUNT];
    ExpenseItem  budget_expense_array[EXPENSE_ITEM_COUNT];

    RevenueItem  budget_total_revenue;
    ExpenseItem budget_total_operating_expense;
    ExpenseItem budget_total_expense;

    // 3 of 3: projected this AND next years: displayed on report

    RevenueItemChange    projected_revenue_array[REVENUE_ITEM_COUNT];
    ExpenseItemChange    projected_expense_array[EXPENSE_ITEM_COUNT];

    ExpenseItemChange   projected_net_distance_learning_expense;

    RevenueItemChange    projected_total_revenue;
    ExpenseItemChange    projected_total_operating_expense;
    ExpenseItemChange    projected_total_expense;

    YearReport   this_year, next_year, last_year, budget_year;

public:

    //--- target_student_intake ---//
    int tradU_target_student_intake;
    int nontradU_target_student_intake;
    int master_target_student_intake;
    int dl_target_student_intake;

    //---- assets and liabilities ------//
    double smoothed_endowment_value;              // 981016		// no use after 981023

    //---- input affected by other models ("input from elsewhere in the model") ----//
    int    tuition_rate;                          // not ($000) // c63; for stage1
    float    get_research_overhead_rate();

    //--------- time variation variable 1017 --------//

    HistoryData  inflation_rate_hist;
    double base_short_term_debt_interest;
    double base_long_term_debt_interest;

    //double	other_operating_income_last2;
    double state_appropriation_last2;
    // "bank deposit rate" = annualInflationRate + .01

    //---- fully exogenous factors -----//

    double inflation_rate;                        // C72
    double short_term_debt_interest;              // Real int. rate on short-term investments	// "bank borrowing rate" = "short-term real interest rate" // = "bank rate" if OpReserve >0, else = "borrowing rate" = "bank rate" + 4%
    double long_term_debt_interest;               // Nominal interest rate on long term debt	// for facilities borrowing
    double debt_fraction_repaid_annually;
    double family_income_growth;                  // C76
    double construction_cost_growth;              // C78

    //--- exogenous, but may be influenced by the player ---//

    double endowment_return;                      // dummy since 0106
    double sponsored_research_growth;
    double gifts_growth;
    double athletics_revenue_growth;              // G75
    double bank_debt_interest;                    // 0222 assume this is bank borrowing rate
    double other_expense_growth;
    double other_income_growth;
    int    building_life_for_calc_depreciation;   // G80

    double prior_dept_fac_exp;                    // 990525 by email ccengine0525.txt

    float state_appropriation_real_growth;        //## chea 300899 put the state_appropriation_real_growth into opt stage 1

    double last_month_SR_D;                       //## chea 041199
    double last_month_SR_ID;                      //## chea 041199

    //---- for update_projected_arrays_monthly() ----//
    char   month_left;
    double total_faculty_salary;
    // ($000)
    int    projected_total_research_expense_this_year;
    float    academic_support_ratio;              // [0.0:5.0]; for faculty pi calc

    long    last_warn_bankrupt_date;

    double research_expense_fac_ratio;
    double research_expense_staff_ratio;

    //--------- budget policies --------//

    StudentPolicy  student_policy_array[STUDENT_POLICY_COUNT];
    FinancePolicy  revenue_policy_array[REVENUE_POLICY_COUNT];
    FinancePolicy  expense_policy_array[EXPENSE_POLICY_COUNT];
    FinancePolicy  cost_rise_policy_array[COST_RISE_POLICY_COUNT];

    //--------- stage 3 optimization ------//
    HiringPolicy hiring_policy_array[MAX_DEPARTMENT];

    int max_hires;                                // "Maximum number of leadership hires possible given the budget"
    int total_hires;                              // player input: "Total faculty to be hired"
    float cost_limit;                             // ($000)

    void calc_max_hires();

    //----- member variables for report display -----//
    char current_report_mode;

    //##### begin: ho 1012 #####//
    int    report_type;
    int    report_year;
    int    report_opt;
    char   report_is_expanded;
    //##### end: ho 1012 #####//

    // For calculate state appropiation and report each year
    double  base_appropriation[2];
    double  enroll_appropriation[2];
    double  performance_appropriation[2];
    double  education_quality_appropriation[2];
    double  total_appropriation[2];
    double  facility_reserve_appropriation[2];

    // store the initial value
    double  initial_state_appropriation;
    double  initial_base_appropriation;
    double  initial_enrollment_appropriation;
    double  initial_performance_appropriation;
    double  initial_eq_appropriation;

    //---------------------------------------------------------------//
    //--------------------- member methods --------------------------//
    //---------------------------------------------------------------//

public:
    void init();
    void   deinit();

    int  write_file(File* filePtr);
    int  read_file(File* filePtr);

    void init_data_pre_enroll();
    void init_data();
    void post_init_data();
    void process_budget();

    bool pre_optimization_stage_3(bool init=false);

    bool optimize_policy_1(bool init=false);
    bool optimize_policy_2();
    bool optimize_policy_3();
    void  correct_surplus_deficit();

    void next_day();

    //## begin trevor 31/7 ##//
    void  next_month();
    void   new_fiscal_year_report();
    //## end trevor 31/7 ##//

    //#### begin trevor 10/8 ####//
    void disp_report_main(int refreshFlag, int dispReport=1, char* reportTitle=NULL);
    void report_detect(int detectReport=1);
    //## end trevor 7/8 ##//

    void  calc_distance_learning_expenditure();

    //## begin Ho 1011 ##//
    void report_deinit();
    void  new_fiscal_year();
    void   new_fiscal_year_report(int refreshFlag);
    void   new_fiscal_year_report_detect();
    void report_paint_button(int refreshFlag);
    //## end Ho 1011 ##//

    //## chea 250899
    void  update_projected_arrays_post_optimization();

    //----- methods for updating projected arrays for this and next year (and for next_day())

private:
    //## begin trevor 7/8 ##//
    void disp_financial_reports(int refreshFlag);
    //## end trevor 7/8 ##//

    void update_external_factors_yearly();
    void update_external_factors_monthly();
    void calc_interest(char updateFlag=UPDATE_YEAR);
    //   	void	update_projected_arrays_post_optimization();
    void update_projected_arrays_monthly();
    void update_projected_arrays_monthly_sub();

public:
    FinancePolicy* get_policy_var(bool isRevenue, char index);

    bool update_projected_arrays_pre_optimization();
    void calc_projected_total_n_percent(int reCalc=0);
    void calc_total_last_year();

    void change_budget(char changeType, bool isRevenue, char index, float percentChange, char showMsg=1);
    void calc_budget_report();
    void set_policy_target(bool isRevenue, char index);

    void budget_calc_total();
    void budget_calc_percent();
    void  budget_calc_change();

    void  calc_asset_total();

    double     get_available_funds();
    double     calc_total_faculty_salary();

    //## chea 170899 calc_change have bug it can't calc. change with two - no.
    //   I have make some change.Since this function is called by other cpp. So may have bug
    static double calc_change(double dest, double base);

    //## chea 240699 for(WFM comments;3/19/99 15.3.1)
    double surplus_deficit_history[HISTORY_MONTH_COUNT];
    //## chea 240699 for(WFM comments;3/19/99 15.3.2)
    double operating_sub_surplus_history[HISTORY_MONTH_COUNT];
    //## chea 240699 for(WFM comments;3/19/99 15.3.4)
    double sponsored_research_history[HISTORY_MONTH_COUNT];
    float sl1_sel_history[HISTORY_YEAR_COUNT];    //## chea 240699 for(WFM comments;3/19/99 15.3.5)
    //## chea 110899 added by b.massy
    double net_tuition_history[HISTORY_MONTH_COUNT];

    // ##### Begin Marco ##### //
    // expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value
    double faculty_salary_increase_history[HISTORY_YEAR_COUNT];
    // finance.revenue_policy_array[PL_TUITION_RATE_GROWTH].result_value
    double tuition_increase_history[HISTORY_YEAR_COUNT];

    double short_term_debt_interest_history[HISTORY_MONTH_COUNT];
    double long_term_debt_interest_history[HISTORY_YEAR_COUNT];

    RevenueItem       initial_revenue_array[REVENUE_ITEM_COUNT];
    //	RevenueItem				initial_total_revenue;
    double          initial_total_revenue_total;
    ExpenseItem       initial_expense_array[EXPENSE_ITEM_COUNT];
    ExpenseItem       initial_total_operating_expense;
    ExpenseItem       initial_total_expense;
    //	ExpenseItemChange		initial_projected_total_expense;
    YearReport        initial_last_year;

    // expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value
    double          initial_faculty_salary_increase;
    double          initial_tuition_increase;     // finance.revenue_policy_array[PL_TUITION_RATE_GROWTH].result_value

    void save_initial_value();

    double          next_indirect_cost_rate_upper_bound;
    // ##### End Marco ##### //
    void  update_appropiation();

private:
    void  update_history(char update_flag);
    void calc_percent_last_year();
    void  update_hiring_policy();

    void set_student_policy(bool init);
    void init_optimize_policy_1();

    void init_optimize_policy_2();

    void optimization_input_reset();

    int  disp_last_year_report(int reportOrder, int reportType, int reportYear, char* reportTitle);
};

extern Finance finance;

//------------------------------------------//
#endif
