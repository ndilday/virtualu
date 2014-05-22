//Filename    : Ofaci_st.cpp
//Description : Facility Class Definition related to Simulation and Statistics/History
//              Refer to HE.Facilities.xls and techdoc 2.4
//Owner       : Fred

// ES library header file
#include <OMATH.H>                                //random_snd

// CU header file
#include <OSYS.H>
#include <OINFO.h>
#include <OPEERSCH.h>
#include <OSCHLRES.h>
#include <OPSCHOOL.h>
#include <OFinance.h>
#include <ODEVELOP.h>
#include <OCHANCE.H>
#include <Odeptres.h>
#include <Odept.h>
#include "OFacilit.h"

//----------- Define static variables -------------//
// enum { URBAN, SUBURBAN, RURAL };		// for campus_environment

/*
  Campus Setting	Faculty #	Adj staff $	SL-1 #	SL-2 #	SL-3 #	SL-4 #	SL-5 #

   Urban	{0.70f, 0.40f, 0.00f, 0.45f, 0.65f, 0.70f, 0.50f};
  Suburban	{1.00f, 0.65f, 0.10f, 0.60f, 0.80f, 0.90f, 0.80f};
  Rural	{1.00f, 0.90f, 0.25f, 1.00f, 1.00f, 1.00f, 1.00f};
*/

enum {
    COEF_COUNT = MAX_STUDENT_LEVEL+2,
};

static const float parking_demand_coef[ENVIRONMENT_COUNT][COEF_COUNT] = {
    {0.70f, 0.40f, 0.00f, 0.45f, 0.65f, 0.70f, 0.50f},
    {1.00f, 0.65f, 0.10f, 0.60f, 0.80f, 0.90f, 0.80f},
    {1.00f, 0.90f, 0.25f, 1.00f, 1.00f, 1.00f, 1.00f},
};

//----------- Define static functions -------------//
//----------- Define static class member variables -------------//

//----------- Begin of function Facility::init_data --------//
//!
void Facility::init_data_pre_finance() {
    // calc total_normal_onm for finance

    PeerSchool *playerSchoolEx = school_res.player_peer_school;

    finance.total_operating_expense.total = playerSchoolEx->total_operating_expenditure;
    finance.expense_array[AC_ACADEMIC_DEPARTMENTS].total = playerSchoolEx->dept_expense_faculty_salaries + playerSchoolEx->dept_expense_staff_salaries + playerSchoolEx->dept_expense_other;
    finance.expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total = playerSchoolEx->sponsored_research / (1+playerSchoolEx->overhead_rate_on_sponsored_research/100.0);

    begin_space_inventory();

    department_res.general_dept_info.actual_sf = 0; // so that this will re-calc during init_data()

    //----------------------//

    DepartmentInfo *deptInfo;
    //GeneralDepartment*	dept;

    int deptCount = department_array.department_count;

    total_normal_onm = 0;

  //------ read in active department array -------//

    for( int i=0 ; i<=deptCount; i++ ) {
	if (i) {
	    //dept		= (department_array[i];
	    deptInfo  = (department_res.info_array+department_array[i]->department_id - 1);
	}
	else {
	    // code to handle general deparment
	    //dept		= &department_res.general_dept;
	    deptInfo = &department_res.general_dept_info;
	}

	float normalOnm = (deptInfo->actual_sf * deptInfo->normal_onm_cost / 1000.0f);
	total_normal_onm += (int) normalOnm;
    }
}

void Facility::init_data() {
    // assumed memset(this, 0, sizeof(Facility)); called

    //------------------------//

    update_parking(true);                           // true: init
    update_crime(true);

  //----- constants or other factors -----//
  // Debt capacity parameters

    maximum_debt_balance_as_a_percent_of_funds_balances = 0.5f;
    max_interest_payments_as_a_percent_of_operating_expenditures = 0.08f;

    percent_gap_required_before_new_construction_is_considered = 0.1f;
    escalation_factor_for_deferrd_maintenance_per_year = 0.025f;

  //---------- init player input/output -------------//

    percent_project_funded_with_debt = 33.3f;

    // assume finance is inited before
    /*
      total_debt_capacity[THIS_YEAR] = (int) min(	// min(E22*'$G$12,'$F$257*E23/'$C$74);
      maximum_debt_balance_as_a_percent_of_funds_balances
      * ( finance.last_year.asset_array[AC_ASSET_TOTAL] - finance.last_year.liability_array[AC_LIABILITY_TOTAL] ),
      finance.total_operating_expense.total *  max_interest_payments_as_a_percent_of_operating_expenditures
      / (finance.long_term_debt_interest / 100.0f) );
    */

  // after 981016:

    float opReserve = (float) finance.this_year.asset_array[AC_OPERATING_RESERVE];

    float availableFunds = float( opReserve
				  + 0.5*( finance.this_year.asset_array[AC_ENDOWMENT]
					  +finance.this_year.asset_array[AC_CAPITAL_RESERVE] )
				  + 0.25 *finance.this_year.asset_array[AC_BUILDINGS]
				  - finance.this_year.liability_array[AC_LIABILITY_TOTAL]);

    availableFunds = max(0.0f,availableFunds);      //min & max bug chea

    total_debt_capacity[THIS_YEAR] = (int) min(
	(float)(maximum_debt_balance_as_a_percent_of_funds_balances * availableFunds),
	(float)(finance.projected_total_operating_expense.this_year.total *  max_interest_payments_as_a_percent_of_operating_expenditures
		/ (finance.long_term_debt_interest / 100.0f)) );//min & max bug chea

    total_debt_capacity[THIS_YEAR] = max(0,total_debt_capacity[THIS_YEAR]);

                                                  //0113
    debt_limit[THIS_YEAR] = cur_debt_limit = int(total_debt_capacity[THIS_YEAR]);

    // e44 = outstanding_debt_balance[THIS_YEAR]
    outstanding_debt_balance[THIS_YEAR] = (int)finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT];

    int beginCapitalReserve = int(finance.last_year.asset_array[AC_CAPITAL_RESERVE]);
    //	min_capital_reserve_balance = int(beginCapitalReserve * 0.6f);
    min_capital_reserve_balance = 0.0f;

    ratio_of_square_footage_to_benchmark[THIS_YEAR] = 1.0f;

    //---------- other vars -------------//

    cur_change_backlog = 0;

    // should we do:
    begin_space_inventory();                        // see next_day()

    // special case - finance vars updated in facilities model

    finance.this_year.asset_array[AC_BUILDINGS] = finance.last_year.asset_array[AC_BUILDINGS];
    finance.this_year.asset_array[AC_CAPITAL_RESERVE] = finance.last_year.asset_array[AC_CAPITAL_RESERVE];
    finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT] = finance.last_year.liability_array[AC_GENERAL_PLANT_DEBT];

    finance.this_year.liability_array[AC_LIABILITY_TOTAL] = 0;
    for( int i=0 ; i<LIABILITY_ITEM_COUNT-1 ; i++ ) {
	finance.this_year.liability_array[AC_LIABILITY_TOTAL] += finance.this_year.liability_array[i];
    }

    update_vars_monthly();
    //update_history_sub();

    onm_n_backlog_history[SECOND_H_THIS_YEAR-1] = onm_n_backlog_history[SECOND_H_THIS_YEAR];

    calc_facility_staff_morale();
}

//------------- End of function Facility::init_data --------//

//----------- Begin of function Facility next_day ------//
//!
void Facility::next_day() {
    // per YEAR
    if ( ( info.game_day == 1 )
	 && info.game_month == finance.fiscal_year_start_month ) {
	// if it's a new financial year

	// assume finance.next_day() is called first and RA is just run
	// assume optimization is just run before this at the *same* day

	update_history_yearly();

	update_parking();                             // before begin_space_inventory()
    }

    // per MONTH
    if ( info.game_day == 1 ) {
	update_crime();

	update_vars_monthly();

	calc_facility_staff_morale();
    }
}

//------------- End of function Facility next_day ------//

//----------- Begin of function Facility::update_history_sub ------//
//!
//!
void Facility::update_history_sub() {
    //------ LAST_YEAR ---------//
    onm_n_backlog_history[FIRST_H_THIS_YEAR-1] = (int) finance.expense_array[AC_OPERATIONS_AND_MAINTENANCE].total;
    //onm_n_backlog_history[SECOND_H_THIS_YEAR-1] = not necessary

    transfer_history[H_PREV_YEAR] = (int) finance.expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].total;
    gifts_to_facility[H_PREV_YEAR] = (int) development_office.last_year.facilities;

    //------ THIS_YEAR ---------//
    onm_n_backlog_history[FIRST_H_THIS_YEAR] = (int) finance.projected_expense_array[AC_OPERATIONS_AND_MAINTENANCE].this_year.total;
    //onm_n_backlog_history[SECOND_H_THIS_YEAR] = updated elsewhere in this file

    transfer_history[H_THIS_YEAR] = (int) finance.projected_expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].this_year.total;

    // assume called after development_office.next_day() for each year
    gifts_to_facility[H_THIS_YEAR] = (int) development_office.this_year.facilities;

    //captial_reserve_expense[H_THIS_YEAR] = updated elsewhere in this file

    //------ NEXT_YEAR ---------//
    onm_n_backlog_history[FIRST_H_THIS_YEAR+1] = (int) finance.projected_expense_array[AC_OPERATIONS_AND_MAINTENANCE].next_year.total;
    //onm_n_backlog_history[SECOND_H_THIS_YEAR] = updated elsewhere in this file

    transfer_history[H_NEXT_YEAR] = (int) finance.projected_expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].next_year.total;
    gifts_to_facility[H_NEXT_YEAR] = (int) development_office.this_year.facilities;

    captial_reserve_expense[H_NEXT_YEAR] = captial_reserve_expense[H_THIS_YEAR];
}

//----------- End of function Facility::update_history_sub ------//

//----------- Begin of function Facility::update_history_yearly ------//
//!
//! update time-series graphs for interface display
//!
void Facility::update_history_yearly() {
    int h;

    // part 2

    // 1 of 2: shift history to "left"
    //
    for ( h=0; h<HISTORY_YEAR_COUNT-1; h++) {
	ratio_of_square_footage_to_benchmark[h] = ratio_of_square_footage_to_benchmark[h+1];

	total_debt_capacity[h] = total_debt_capacity[h+1];
	outstanding_debt_balance[h] = outstanding_debt_balance[h+1];
	debt_limit[h] = debt_limit[h+1];

	construction_project_spending[h] = construction_project_spending[h+1];

	transfer_history[h] = transfer_history[h+1];
	gifts_to_facility[h] = gifts_to_facility[h+1];
	captial_reserve_expense[h] = captial_reserve_expense[h+1];
    }

    shift_history(onm_n_backlog_history, HISTORY_YEAR_COUNT2);

    // 2 of 2: update current year values

    captial_reserve_expense[H_THIS_YEAR] = 0;

    // calc after RA

    // after 981016:

    float availableFunds = (float) finance.get_available_funds();

    total_debt_capacity[THIS_YEAR] = (int) min(
	(float)(maximum_debt_balance_as_a_percent_of_funds_balances * availableFunds),
	(float)(finance.projected_total_operating_expense.this_year.total *  max_interest_payments_as_a_percent_of_operating_expenditures
		/ (finance.long_term_debt_interest / 100.0f)) );//min & max bug chea

    total_debt_capacity[THIS_YEAR] = max(0,total_debt_capacity[THIS_YEAR]);

    //--------------------------//
    // Ending Space Inventory	 //
    //--------------------------//
    float inflationRate = (float) finance.inflation_rate / 100.0f;
    const float growth = float(1 + inflationRate + finance.construction_cost_growth/100.0f);

    DepartmentInfo *deptInfo;
    GeneralDepartment*  dept;

    int deptCount = department_array.department_count;
    int totalActualSF = 0, totalNormalSF = 0, totalProjectedSF = 0, i;

    for( i=0 ; i<=deptCount; i++ ) {
	if ( i ) {
	    dept    = (GeneralDepartment*) department_array[i];
	    // -1: otherwise will have bug
	    deptInfo  = (department_res.info_array + department_array[i]->department_id - 1);
	}
	else {
	    // code to handle general deparment
	    dept    = &department_res.general_dept;
	    deptInfo = &department_res.general_dept_info;
	}

	totalActualSF += deptInfo->actual_sf;
	totalNormalSF += deptInfo->normal_sf;
	totalProjectedSF += dept->get_constructing_sf() + deptInfo->actual_sf;

	// 1027; see response_func:Time_Variation

	float last1 = deptInfo->cost_growth;
	deptInfo->cost_growth = math.time_variation(last1, (deptInfo->cost_growth_last2), 1.4482f, -0.8011f, 0.003528950f) + 0.03f;
	deptInfo->cost_growth_last2 = last1;

	deptInfo->replacement_cost = int( deptInfo->replacement_cost * (1+inflationRate+deptInfo->cost_growth) );

	err_when(deptInfo->replacement_cost <= 0);
    }

    // average the replacement_cost among the departments
    int totalReplacementCost=0;
    // Calculate the total Replacement Cost
    for( i=0 ; i<=deptCount; i++ ) {
	if ( i ) {
	    // -1: otherwise will have bug
	    deptInfo  = (department_res.info_array + department_array[i]->department_id - 1);
	}
	else {
	    // code to handle general deparment
	    deptInfo = &department_res.general_dept_info;
	}

	totalReplacementCost += deptInfo->replacement_cost;
    }

    // get Average and store
    for( i=0 ; i<=deptCount; i++ ) {
	if ( i ) {
	    // -1: otherwise will have bug
	    deptInfo  = (department_res.info_array + department_array[i]->department_id - 1);
	}
	else {
	    // code to handle general deparment
	    deptInfo = &department_res.general_dept_info;
	}

	deptInfo->replacement_cost = totalReplacementCost/deptCount;
    }

    // askbillok: is it what he want:
    // update this ratio after project completed
    //float ratioSF;
    ratio_of_square_footage_to_benchmark[THIS_YEAR] = 1.0f;
    err_when(!totalActualSF);

    for( i=0 ; i<=deptCount; i++ ) {
	if ( i ) {
	    dept    = (GeneralDepartment*) department_array[i];
	    deptInfo  = (department_res.info_array+department_array[i]->department_id - 1);
	}
	else
	    // code to handle general deparment
	    deptInfo = &department_res.general_dept_info;

	float ratio_sf_to_benchmark = float(deptInfo->actual_sf) / deptInfo->normal_sf;

	ratio_of_square_footage_to_benchmark[THIS_YEAR] *= (float) math.safe_pow(ratio_sf_to_benchmark, float(deptInfo->actual_sf) / totalActualSF);
    }

    //-------- post last year data first -------//

    //	finance.last_year.asset_array[AC_BUILDINGS] = finance.this_year.asset_array[AC_BUILDINGS];
    //	finance.last_year.asset_array[AC_CAPITAL_RESERVE] = finance.this_year.asset_array[AC_CAPITAL_RESERVE];
    //	finance.last_year.liability_array[AC_GENERAL_PLANT_DEBT]	= finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT];

    //------------------------------------//
    // Year-end Replacement Cost and Debt //
    //------------------------------------//
    // assume called after finance optimization and finance.next_day()'s buget update//

    //askbill
    //option 1: update balance sheet yearly

    //update at the beginning of construction
    //finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT]
    //	= finance.last_year.liability_array[AC_GENERAL_PLANT_DEBT]
    //	+ total_actual_new_debt - finance.last_year.liability_array[AC_GENERAL_PLANT_DEBT] * finance.debt_fraction_repaid_annually / 100.0f;

    //option 2

    // assume called after develop_office.next_day
    finance.this_year.asset_array[AC_BUILDINGS]     // = E76 (year-end replacement cost
	= finance.this_year.asset_array[AC_BUILDINGS]
	- finance.this_year.asset_array[AC_BUILDINGS] / finance.building_life_for_calc_depreciation;

    finance.this_year.asset_array[AC_CAPITAL_RESERVE] += development_office.last_year.facilities;

    finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT]
	= finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT]
	- finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT] * finance.debt_fraction_repaid_annually / 100.0f;

    facility_office.outstanding_debt_balance[THIS_YEAR] = finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT];

    finance.this_year.liability_array[AC_LIABILITY_TOTAL] = 0;
    for( i=0 ; i<LIABILITY_ITEM_COUNT-1 ; i++ ) {
	finance.this_year.liability_array[AC_LIABILITY_TOTAL] += finance.this_year.liability_array[i];
    }

    //------------------------------------//
    // variables accumlated through a year
    construction_project_spending[THIS_YEAR] = 0;
    cur_change_backlog = 0;
}

//------------- End of function Facility::update_history_yearly ------//

//----------- Begin of function Facility::begin_space_inventory --------//
//!
//! initialize actual and normal sf yearly
//!
void Facility::begin_space_inventory() {
    DepartmentInfo *deptInfo;
    Department* dept;

    int deptCount = department_array.department_count;

    //-----------------------------------//
    // code to handle deparment_array
    //-----------------------------------//

    for( int i=1 ; i<=deptCount; i++ ) {
	dept    = department_array[i];
	deptInfo  = (department_res.info_array+dept->department_id - 1);

	// calc of normal_sf is
	// based on the prior year's enrollments and research, but the
	// current year's faculty numbers. Ie, this routine should run
	// after the resource allocation routine, which determines faculty numbers for the year.
	// course enroll stats should be the maximum trimestrer enrollment over the three trimesters last year <- checked OK on 0106

	deptInfo->normal_sf = int(
	    deptInfo->fixed_sf_per_dept
	    + dept->course_array.course_enrollments[SEMINAR] * deptInfo->sf_per_course_enrollment_in_seminars
	    + dept->course_array.course_enrollments[GENERAL] * deptInfo->sf_per_course_enrollment_in_general_courses
	    + dept->course_array.course_enrollments[CLASS_WITH_BREAKOUT] * deptInfo->sf_per_course_enrollment_in_lecture_courses
	    + deptInfo->sf_per_faculty * dept->faculty_array.faculty_count
	    //  per thousand dollars  //## chea 221199 /1000
	    + deptInfo->sf_per_research_dollar * dept->total_research_dollar /1000);

	// if 1st time calc
	if ( info.prerun_year && info.game_year == 1 ) {
	    deptInfo->actual_sf = (int)( (float)deptInfo->normal_sf * math.get_random_snd(1.1f, PSCH_SD(0.25f)) );
	}
    }

    //-----------------------------------//
    // code to handle general deparment
    //-----------------------------------//

    deptInfo = &department_res.general_dept_info;

    int tmp;

    //f20
    tmp = int(finance.total_operating_expense.total - finance.expense_array[AC_ACADEMIC_DEPARTMENTS].total - finance.expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total);
    tmp = max(0,tmp);
    //deptInfo->normal_sf = projected_parking_supply + int(deptInfo->fixed_sf_per_dept + tmp * deptInfo->sf_per_dollar_of_central_exp);
    deptInfo->normal_sf = int(deptInfo->fixed_sf_per_dept + tmp * deptInfo->sf_per_dollar_of_central_exp);

    if ( info.prerun_year && info.game_year == 1 ) {
	deptInfo->actual_sf = (int)( (float)deptInfo->normal_sf * math.get_random_snd(1.1f, PSCH_SD(0.25f)) );
    }
}

//------------- End of function Facility::begin_space_inventory --------//

//----------- Begin of function Facility::update_vars_monthly --------//
//!
void Facility::update_vars_monthly() {
    //----------------------//
    // update actual sf		//		(if new construction completed)
    //----------------------//

    DepartmentInfo *deptInfo;
    GeneralDepartment*  dept;

    int deptCount = department_array.department_count;

    float normalOnm;
    int newSF, projectExpense, projectExpenseTotal = 0;
    // unit: square foot
    int totalActualSF = 0, totalNormalSF = 0, totalProjectedSF = 0;

    total_normal_onm = 0;

    //------ read in active department array -------//

    for( int i=0 ; i<=deptCount; i++ ) {
	if (i) {
	    dept    = (GeneralDepartment*) department_array[i];
	    deptInfo  = (department_res.info_array+department_array[i]->department_id - 1);
	}
	else {
	    // code to handle general deparment
	    dept    = &department_res.general_dept;
	    deptInfo = &department_res.general_dept_info;
	}

	// check completed new construction
	//
	dept->next_month_construction(newSF, projectExpense);

	//Equals sf at the beginning of the prior year plus NEW CONSTRCUTION COMPLETED during the prior year.
	//This is the amount of space actually available for use during the year.
	deptInfo->actual_sf += newSF;
	projectExpenseTotal += projectExpense;

	// Same as actual_sf in that: this should be recalculated whenever new sf get completed.
	normalOnm = (deptInfo->actual_sf * deptInfo->normal_onm_cost / 1000.0f);
	total_normal_onm += (int) normalOnm;

	totalActualSF += deptInfo->actual_sf;
	totalNormalSF += deptInfo->normal_sf;
	totalProjectedSF += dept->get_constructing_sf() + deptInfo->actual_sf;

	err_when( deptInfo->normal_sf<=0 || deptInfo->actual_sf<=0 || normalOnm<=0 );
    }

    // for screen "space usage": norm-projected-actual curves graph
    shift_history(normal_area, HISTORY_MONTH_COUNT);
    shift_history(actual_area, HISTORY_MONTH_COUNT);
    shift_history(projected_area, HISTORY_MONTH_COUNT);

    normal_area[THIS_MONTH] = totalNormalSF;
    actual_area[THIS_MONTH] = totalActualSF;
    projected_area[THIS_MONTH] = totalProjectedSF;

    //----------------------//
    // update backlog			//
    //----------------------//

    // vars after the section "Beginning Space Inventory"

    // 990420	0.05f *
    int tmp = int(math.get_random_snd(0.0f, PSCH_SD(0.05f * float(total_normal_onm)) ));
    int beginBacklog = total_normal_onm + ( (tmp > 0) ? tmp : 0);

    // remove stage1_revenue, stage1_expense, stage2_expense

    float buget_monthly; {
	double cons1 = (1 + finance.inflation_rate/100.0 + finance.expense_policy_array[PL_STAFF_SALARY_INCREASES].result_value / 100.0);
	double cons2 = (1 + finance.inflation_rate/100.0 + finance.other_expense_growth/100.0);
	double computed_growthrate = finance.cost_rise_policy_array[AC_OPERATIONS_AND_MAINTENANCE].result_value;
	double base_value = cons1 * finance.expense_array[AC_OPERATIONS_AND_MAINTENANCE].staff + cons2 * finance.expense_array[AC_OPERATIONS_AND_MAINTENANCE].other;
	double change_value = base_value * computed_growthrate / 100.0;
	double new_value = (change_value + base_value );
	buget_monthly = float(new_value / 12.0f);
    }

    cur_change_backlog = int(
	beginBacklog * escalation_factor_for_deferrd_maintenance_per_year / 12.0f
	- total_normal_onm / 12.0f - buget_monthly);

    // update end_backlog
    onm_n_backlog_history[SECOND_H_THIS_YEAR] = max(0, beginBacklog + cur_change_backlog);
    onm_n_backlog_history[SECOND_H_THIS_YEAR+1] = onm_n_backlog_history[SECOND_H_THIS_YEAR];

    //------------------//
    update_history_sub();
}

//------------- End of function Facility::update_vars_monthly --------//

//----------- Begin of function Facility::run_new_construction --------//
//!
//! called a) when player change anything in Facility interface and b) in the beginning of a new fiscal year
//!
void Facility::run_new_construction() {
    //----------------------//
    // Capital Availability //		(pre construction)
    //----------------------//

    // calc after player change cur_debt_limit

    debt_limit[THIS_YEAR] = cur_debt_limit;

    // e44 = outstanding_debt_balance[THIS_YEAR]
    int e44;
    e44 = outstanding_debt_balance[THIS_YEAR] = (int)finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT];

    int maxNewDebt = int(min(total_debt_capacity[THIS_YEAR], debt_limit[THIS_YEAR])
			 - e44 + e44 * finance.debt_fraction_repaid_annually / 100.0f);

    int beginCapitalReserve = int(finance.last_year.asset_array[AC_CAPITAL_RESERVE]);

    //askbill update frequency for this two
    int interestReceived = int(beginCapitalReserve * (finance.inflation_rate + finance.short_term_debt_interest ) / 100.0f );

    int transferFromBudget = int(finance.budget_expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].total);

    // calc after player change (budget change)
    // should tell user it's at least zero
    if ( transferFromBudget < 0 )
	transferFromBudget = 0;

    // calc after player change min_capital_reserve_balance

    int availableCapitalReserveFunds
	= beginCapitalReserve + interestReceived + transferFromBudget - min_capital_reserve_balance;

    // need to reset this value if player don't change it!
    //	min_capital_reserve_balance = int(beginCapitalReserve * 0.6f);
    min_capital_reserve_balance = 0.0f;

    //askbill tell user about this:
    if ( availableCapitalReserveFunds < 0 )
	availableCapitalReserveFunds = 0;

    //----------------------//
    // New construction     //
    //----------------------//

    DepartmentInfo *deptInfo;
    GeneralDepartment*  dept;

    int deptCount = department_array.department_count;
    float percentGap;                               // 30% -> 0.3f
    int sfGap;
    int newSquareFoot;
    float actualInvest,                             // $000
	actualDrawOnCapitalReserve,
	totalActualInvest = 0,
	totalAdditionalDebt = 0,
	totalRequiredDrawOnCapitalReserve = 0;        // $000
    int totalAvailableSpace=0;

    //------ read in active department array -------//

    for( int i=0 ; i<=deptCount; i++ ) {
	if ( i ) {
	    dept    = department_array[i];
	    deptInfo  = (department_res.info_array+department_array[i]->department_id - 1);
	}
	else {
	    // code to handle general deparment
	    dept    = &department_res.general_dept;
	    deptInfo = &department_res.general_dept_info;
	}

	sfGap = max(0, deptInfo->normal_sf - deptInfo->actual_sf - dept->get_constructing_sf());

	totalAvailableSpace += sfGap;

	percentGap = float(sfGap) / deptInfo->normal_sf;

	deptInfo->invest_close_gap = ( percentGap > percent_gap_required_before_new_construction_is_considered )
	    ? (float(sfGap) * deptInfo->replacement_cost / 1000) : 0;

	deptInfo->additional_debt = (deptInfo->invest_close_gap * float(percent_project_funded_with_debt) / 100.0f );
	totalAdditionalDebt += deptInfo->additional_debt;

	err_when(deptInfo->invest_close_gap<0 || deptInfo->additional_debt < 0);
	err_when(deptInfo->invest_close_gap < deptInfo->additional_debt );
    }

    total_actual_new_debt = 0;

    //min & max bug chea
    float maxDebt = max(0.0f,min((float)maxNewDebt, totalAdditionalDebt));

    for( int i=0 ; i<=deptCount; i++ ) {
	if ( i ) {
	    dept    = department_array[i];
	    deptInfo  = (department_res.info_array+department_array[i]->department_id - 1);
	}
	else
	    // code to handle general deparment
	    deptInfo = &department_res.general_dept_info;

	if ( totalAdditionalDebt ) {
	    // avoid overflow
	    deptInfo->actual_new_debt = (float(deptInfo->additional_debt) * float(maxDebt) / totalAdditionalDebt);
	    total_actual_new_debt += deptInfo->actual_new_debt;
	    err_when(deptInfo->actual_new_debt < 0 );
	}
	else
	    deptInfo->actual_new_debt = 0;

	deptInfo->required_draw_on_capital_reserve = deptInfo->invest_close_gap - deptInfo->actual_new_debt;

	//		if ( deptInfo->required_draw_on_capital_reserve < 0 )
	if ( deptInfo->required_draw_on_capital_reserve < 0 ) {
	    deptInfo->actual_new_debt = deptInfo->invest_close_gap;
	    // 990419
	    total_actual_new_debt += deptInfo->required_draw_on_capital_reserve;
	    deptInfo->required_draw_on_capital_reserve = 0;
	}
	else
	    totalRequiredDrawOnCapitalReserve += deptInfo->required_draw_on_capital_reserve;

	err_when(deptInfo->required_draw_on_capital_reserve < 0);

	err_when(deptInfo->invest_close_gap >= 10
		 && deptInfo->invest_close_gap < deptInfo->actual_new_debt + deptInfo->required_draw_on_capital_reserve - 10 );
    }

    err_when(totalAdditionalDebt < 0 || totalRequiredDrawOnCapitalReserve < 0 || total_actual_new_debt < 0);

    //----- rows 62:67 -----//
    bool newConstructionAdded = false;

    float fundedByDebt = facility_office.debt_spinner_var1/100;
    int investmentRequirement = int(facility_office.spinner_var4*department_res.general_dept_info.replacement_cost);

    int facilityReserveRequired = investmentRequirement*(1-fundedByDebt);

    int debtRequirement = investmentRequirement*fundedByDebt;

    for( int i=0 ; i<=deptCount; i++ ) {
	if ( i ) {
	    dept    = department_array[i];
	    deptInfo  = (department_res.info_array+department_array[i]->department_id - 1);
	}
	else {
	    // code to handle general deparment
	    dept    = &department_res.general_dept;
	    deptInfo = &department_res.general_dept_info;
	}

	//## chea 160899 (9 August 99) 10.3.3
	if ( totalRequiredDrawOnCapitalReserve && availableCapitalReserveFunds ) {
	    float avaltRatio = min( 1.0f, availableCapitalReserveFunds / totalRequiredDrawOnCapitalReserve );

	    actualDrawOnCapitalReserve = float(deptInfo->required_draw_on_capital_reserve) * avaltRatio;
	}
	else
	    actualDrawOnCapitalReserve = 0;

	actualDrawOnCapitalReserve = min( actualDrawOnCapitalReserve, deptInfo->invest_close_gap - deptInfo->actual_new_debt);

	actualInvest = actualDrawOnCapitalReserve + deptInfo->actual_new_debt;
	totalActualInvest += actualInvest;

	if ( totalAvailableSpace != 0 )
	    newSquareFoot = facility_office.spinner_var4*1000*max(0, deptInfo->normal_sf - deptInfo->actual_sf - dept->get_constructing_sf())/totalAvailableSpace;
	else
	    // In case totalAvailable space is 0, build space divide evenly
	    newSquareFoot = facility_office.spinner_var4*1000/(deptCount+1);

	err_when(newSquareFoot<0 || deptInfo->replacement_cost <=0);

	// add this new construction into array of new construction, for each dept
	//
	if ( newSquareFoot > 0 )
	    {
		//
		construction_project_spending[THIS_YEAR] += actualInvest;

		//
		dept->add_construction(newSquareFoot, max(1,(int)actualInvest));
		newConstructionAdded = true;
	    }

	err_when(actualDrawOnCapitalReserve < 0 || deptInfo->actual_new_debt < 0);
    }

    captial_reserve_expense[H_THIS_YEAR] += facilityReserveRequired;

    //-- captial_reserve_expense[H_NEXT_YEAR] has not been initalized yet, initialize it now ---//

    if( captial_reserve_expense[H_NEXT_YEAR] == 0 )
	captial_reserve_expense[H_NEXT_YEAR] = captial_reserve_expense[H_THIS_YEAR];

    //---------------------------//

    if ( newConstructionAdded ) {
	if ( totalActualInvest > 0 )
	    finance.this_year.asset_array[AC_BUILDINGS] // = E76 (year-end replacement cost)
		= finance.this_year.asset_array[AC_BUILDINGS] + totalActualInvest;

	if ( facilityReserveRequired > 0 ) {
	    //			finance.this_year.asset_array[AC_CAPITAL_RESERVE] -= totalRequiredDrawOnCapitalReserve;  //## this should be wrong from fed
	    finance.this_year.asset_array[AC_CAPITAL_RESERVE] -= captial_reserve_expense[H_THIS_YEAR];
	    //       finance.this_year.asset_array[AC_CAPITAL_RESERVE] -= actualDrawOnCapitalReserve; //## chea 160899
	}

	if ( debtRequirement > 0 ) {
	    // update the amount of debt immediately
	    finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT] += debtRequirement;
	    facility_office.outstanding_debt_balance[THIS_YEAR] = finance.this_year.liability_array[AC_GENERAL_PLANT_DEBT];
	}
    }

    finance.this_year.liability_array[AC_LIABILITY_TOTAL] = 0;
    for( int i=0 ; i<LIABILITY_ITEM_COUNT-1 ; i++ ) {
	finance.this_year.liability_array[AC_LIABILITY_TOTAL] += finance.this_year.liability_array[i];
    }

    // assume this func called monthly

}

//------------- End of function Facility::run_new_construction --------//

//---------- Begin of function Facility::calc_facility_staff_morale -----------//
void Facility::calc_facility_staff_morale() {
    const int VAR_COUNT = 3;
    int i;
    float input[VAR_COUNT];
    memset(input, 0, sizeof(input));

    //--------------//
    input[0] = player_school.staff_morale;

    //--------------//
    input[1] = 100*float(onm_n_backlog_history[SECOND_H_THIS_YEAR]) / float(finance.this_year.asset_array[AC_BUILDINGS]);

    //--------------//
    DepartmentInfo *deptInfo;
    int deptCount = department_array.department_count;
    total_invest_close_gap = 0;

    for( i=0 ; i<=deptCount; i++ ) {
	if ( i ) {
	    deptInfo  = (department_res.info_array+department_array[i]->department_id - 1 );
	}
	else {
	    deptInfo = &department_res.general_dept_info;
	}

	total_invest_close_gap += deptInfo->invest_close_gap;
    }

    input[2] = 100*float(total_invest_close_gap) / float(finance.this_year.asset_array[AC_BUILDINGS]);

    err_when( finance.this_year.asset_array[AC_BUILDINGS] <= 0 );

    //--------------//
    input[1] = math.dual_response_func(0,60.74f,100,-7.15f,-5.94f,16.41f,3.95f, input[1]);
    input[2] = math.dual_response_func(0,68.17f,100,-13.40f,-7.24f,20.54f,4.32f, input[2]);

    float weight[VAR_COUNT] = {0.50f, 0.30f, 0.2f, };

    float newValue = 0;
    for(i=0; i<VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    //min & max bug chea
    facility_staff_morale = max(0.0f,min(100.0f,player_school.latency_func( 0.1f, facility_staff_morale, newValue)));

    //--------------//
    facility_staff_morale *= (float) finance.cost_rise_policy_array[PL_OPERATIONS_AND_MAINTENANCE].penalty_multiplier1;

    //min & max bug chea
    facility_staff_morale = max(0.0f,min(100.0f,facility_staff_morale));
}

//---------- End of function Facility::calc_facility_staff_morale -----------//

//----------- Begin of function Facility::update_parking --------//
//!
void Facility::update_parking(bool init) {
    //------------------//
    //990104 Parking and Crime model; see design response 0805.

    int demandVar[COEF_COUNT];
    int totalDemand = 0;

    demandVar[0] = department_array.faculty_level_history[FACULTY_RANK_LEVEL_COUNT][THIS_YEAR];
    demandVar[1] = int(finance.total_expense.staff / 65000000);

    for (int i=0; i<MAX_STUDENT_LEVEL; i++)
	demandVar[i+2] = department_array.student_level_history[i][THIS_YEAR];

    for (int x=0; x<COEF_COUNT; x++)
	totalDemand += int(parking_demand_coef[player_school.campus_environment][x] * demandVar[x]);

    //------------------//

    if ( init ) {
	const float meanByEnvir[ENVIRONMENT_COUNT] = { 1.0f, 1.1f, 1.2f };
	float mean = meanByEnvir[player_school.campus_environment];

	// initialize as close-in supply
	cur_parking_supply = int(totalDemand * math.get_random_snd(mean, PSCH_SD(mean*0.1f)));
	projected_parking_supply = cur_parking_supply;

	farout_capacity = int(cur_parking_supply * math.get_random_snd(1.3f , PSCH_SD(0.1f)));
    }

    if ( !init && totalDemand > cur_parking_supply ) {
	if ( player_school.campus_environment == RURAL )
	    cur_parking_supply = totalDemand;           // add far-out lots
	else if ( player_school.campus_environment == SUBURBAN && farout_capacity > 0 ) {
	    int newSpace = min(farout_capacity, totalDemand - cur_parking_supply);

	    farout_capacity -= newSpace;
	    cur_parking_supply += newSpace;             // add far-out lots
	}
	else {                                        // build close-in structure
	    // option 1: build structure instantly

	    // 30: $30,000 per space
	    finance.this_year.asset_array[AC_CAPITAL_RESERVE] -= 30 * (totalDemand - cur_parking_supply);

	    projected_parking_supply = totalDemand;
	    cur_parking_supply = projected_parking_supply;

	    // option 2: use normal facilities construction procedure
	}
    }

    shift_history(parking_demand, HISTORY_YEAR_COUNT);
    shift_history(parking_supply, HISTORY_YEAR_COUNT);

    parking_demand[THIS_YEAR] = cur_parking_supply;
    parking_supply[THIS_YEAR] = totalDemand;

    if ( init ) {
	err_when(totalDemand <= 0);
	err_when(cur_parking_supply <= 0);

	for (int i = THIS_YEAR; i>=THIS_YEAR-1; i--) {
	    parking_demand[i-1] = parking_demand[i];
	    parking_supply[i-1] = parking_supply[i];
	}
    }
}

//------------- End of function Facility::update_parking --------//

//----------- Begin of function Facility::update_crime --------//
//!
void Facility::update_crime(bool init) {
    static float initAdminRatio = float(finance.projected_expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].this_year.total / finance.projected_total_operating_expense.this_year.total ) ;

    //------------------//
    //990106 Parking and Crime model; see design response 0805.

    // var 40 in response_func

    static float currCrime=0;
    int i;

    //------------------//
    const int VAR_COUNT = 4;
    float input[VAR_COUNT];

    //------------------//
    const int remap[ENVIRONMENT_COUNT]={3,2,1};
    input[0] = 25 * (float) remap[player_school.campus_environment];

    //------------------//
    input[1] = (float) ((finance.projected_expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].this_year.total / finance.projected_total_operating_expense.this_year.total ) / initAdminRatio);

    //------------------//
    input[2] = player_school.sub_score[S_STUDENT_MORALE][THIS_MONTH];

    //------------------//
    input[3] = player_school.staff_morale;

    //------------------//
    input[1] = math.single_response_func(1,100,1.900f, 1.047f, input[1]);
    input[2] = math.dual_response_func(1, 50.1f, 100, -40.21f, -24.36f, 69.78f, 20.19f, input[2]);
    input[3] = math.dual_response_func(1, 51.5f, 100, -51.33f, -27.54f, 62.25f, 16.38f, input[3]);

    const float weight[VAR_COUNT] = { 0.40f,0.35f,0.15f,0.10f };
    float newValue=0;

    for (i=0;i<VAR_COUNT;i++)
	newValue  += weight[i] * input[i];

    currCrime = player_school.latency_func(0.1f, newValue, currCrime);

    //------------------//

    //min & max bug chea
    currCrime = m.fmin(100.0f, m.fmax(0.0f,math.get_random_snd(currCrime, PSCH_SD(currCrime * 0.075f))));

    shift_history(crime_index, HISTORY_MONTH_COUNT);
    crime_index[THIS_MONTH] = (char) player_school.latency_func(0.5f, currCrime, crime_index[THIS_MONTH]);

    if ( init ) {
	for (i = THIS_MONTH; i>=THIS_MONTH-1; i--) {
	    crime_index[i-1] = crime_index[i];
	}
    }
}

//------------- End of function Facility::update_crime --------//

// ##### Begin Marco ##### //
// --------- Begin of function Facility::save_initial_data ------- //
void Facility::save_initial_data() {
    initial_total_invest_close_gap = total_invest_close_gap;

    //	for (int i = 0; i < HISTORY_YEAR_COUNT; i++)
    //		initial_captial_reserve_expense[i] = captial_reserve_expense[i];
    initial_captial_reserve_expense = captial_reserve_expense[H_PREV_YEAR];
    initial_crime_index = crime_index[THIS_MONTH];  // 0-100
    initial_normal_area = normal_area[THIS_MONTH];
    initial_actual_area = actual_area[THIS_MONTH];
    initial_projected_area = projected_area[THIS_MONTH];
}

// --------- End of function Facility::save_initial_data ------- //
// ##### End Marco ##### //
