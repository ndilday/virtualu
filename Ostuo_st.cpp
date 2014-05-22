//Filename    : OStuO_st.cpp
//Description : StudentOffice Class Definition; includes Residence Hall; refer to HE.facilities.xls -> residence hall
//Owner       : Fred

// ES library header file
#include <OMATH.H>

// CU header file
#include <OINFO.H>
#include <OPSCHOOL.H>
#include <OFinance.H>
#include <OPEERSCH.H>
#include <OSCHLRES.H>

#include "OStuOff.h"

//----------- Define constants -------------//
//----------- Define static variables -------------//
//----------- Define static functions -------------//
//----------- Define static class member variables -------------//

//----------- Begin of function StudentOffice Constructor -----//
//! desc
//! <int> -
//! [int] -
//!
StudentOffice::StudentOffice() {

}

//------------- End of function StudentOffice Constructor -----//

//----------- Begin of function StudentOffice Destructor ------//
//!
StudentOffice::~StudentOffice() {

}

//------------- End of function StudentOffice Destructor ------//

//----------- Begin of function StudentOffice::init_data --------//
//!
//! refer to HE.facilities.xls -> residence hall
//!
void StudentOffice::init_data() {
    memset(this, 0, sizeof(StudentOffice));

    // player input
    //targe_sl1_in_hall_percent = 50;

    //------------------------//
    // -> COLUMN E: initialize
    //------------------------//

    //------ Parameters -------//
    accomodate_excess_demand = true;
    //33%
    percent_excess_demand_before_capacity_increased = 33;
    replacement_cost_per_student_space = 15000;     // not ($000)
    replacement_cost_last2 = replacement_cost_per_student_space;
    raw_initial_onm_cost_per_student_space = replacement_cost_per_student_space / 40.0f;
    raw_initial_variable_cost_per_occupied_space =  0.175f * replacement_cost_per_student_space;

    //------ Capacity, demand, and revenue -------//

    PeerSchool *playerSchool = school_res.player_peer_school;

    // E95
    room_n_board_rate = playerSchool->room_and_board_rate;
    //percent_demanding_places_sl1 = min(max(0,10+playerSchool->percent_traditional_ug_in_residence_halls), 100);
    percent_demanding_places_sl1 = (float) m.fmin(m.fmax(0,math.get_random_snd(player_school.fulltime_ug_in_housing_percent, PSCH_SD(0.03f * player_school.fulltime_ug_in_housing_percent))), 100);

    if(percent_demanding_places_sl1>=100)
	percent_demanding_places_sl1=100;

    // E98
    demand_places = int(percent_demanding_places_sl1 * enroll_res.student_count[UG_TRADITION] / 100);
    current_places = demand_places;
    current_places = int(m.fmax(0, current_places + math.get_random_snd(0, PSCH_SD(current_places * 0.04f))));
    filled_places = min(current_places, demand_places);
    // ($000)
    float total_revenue = float(filled_places * room_n_board_rate) / 1000;

    // Replacement cost of beginning spaces ($000)
    // replacement_cost = current_places * replacement_cost_per_student_space / 1000;

    //------- Cost/Expense -----------------//

    // ($000)
    float debtBalance = (float) finance.last_year.liability_array[AC_RESIDENCE_HALL_DEBT];

    // ($000)
    float serviceOnDebt = debtBalance * float(finance.debt_fraction_repaid_annually + finance.long_term_debt_interest) / 100;
    // ($000)
    float fixed_onm = raw_initial_onm_cost_per_student_space * current_places / 1000;
    // ($000)
    float variable_cost = raw_initial_variable_cost_per_occupied_space * filled_places / 1000;
    // ($000)
    float total_expense = serviceOnDebt + fixed_onm + variable_cost;

    // E106-108
    float multipler = (total_revenue - serviceOnDebt) / (total_expense - serviceOnDebt);
    fixed_onm *= multipler;
    variable_cost *= multipler;
    // ($000)
    total_expense = serviceOnDebt + fixed_onm + variable_cost;

    //askbill?  multipler is useless and is < 0?

    //------------------------//
    // -> COLUMN G: year1
    //------------------------//

    // G97: percent_demanding_places_sl1
    // The "+.005" represents a random normal deviate with mean zero and standard deviation
    // tentatively equal to .025*Sqrt(p*(1-p)), where p is the value in E97. The result
    // should be truncated at zero (no negative percents are allowed).
    // The mean value may depend on the scenario.
    //
    // The deviation is applied to year 1 to get a potential demand imbalance at
    // the start of the game.

    percent_demanding_places_sl1 += 100*math.get_random_snd(0, PSCH_SD(0.025f*float(sqrt(percent_demanding_places_sl1/100*(1-percent_demanding_places_sl1/100)))) );
    // min & max bug chea
    percent_demanding_places_sl1 = max(0.0f,percent_demanding_places_sl1);

    if(percent_demanding_places_sl1>=100)           //## chea 270999
	percent_demanding_places_sl1=100;

    demand_places = int(percent_demanding_places_sl1 * enroll_res.student_count[UG_TRADITION] / 100);
    filled_places = min(current_places, demand_places);

    //------------------------//
    // ($000)
    fixed_onm = raw_initial_onm_cost_per_student_space * current_places / 1000;
    // ($000)
    variable_cost = raw_initial_variable_cost_per_occupied_space * filled_places / 1000;
    // ($000)
    total_expense = serviceOnDebt + fixed_onm + variable_cost;

    room_n_board_rate = (1000.0f * total_expense / filled_places);

    //------------------------//
    // update total_revenue after calc of room and board rate
    // ($000)
    total_revenue = float(filled_places * room_n_board_rate) / 1000;

    //------------------------//
    //new_places;
    int unmetDemand = demand_places - filled_places;
    if ( accomodate_excess_demand
	 && float(unmetDemand) * 100 / demand_places > percent_excess_demand_before_capacity_increased
	)
	new_places = unmetDemand;
    else
	new_places = 0;

    //($000)
    new_places_expense_last_year = replacement_cost_per_student_space * new_places / 1000.0f;
    finance.last_year.asset_array[AC_RESIDENCE] = replacement_cost_per_student_space * current_places / 1000.0f;
    finance.this_year.asset_array[AC_RESIDENCE] = finance.last_year.asset_array[AC_RESIDENCE];

    finance.this_year.liability_array[AC_RESIDENCE_HALL_DEBT] = finance.last_year.liability_array[AC_RESIDENCE_HALL_DEBT];

    finance.this_year.liability_array[AC_LIABILITY_TOTAL] = 0;
    for( int i=0 ; i<LIABILITY_ITEM_COUNT-1 ; i++ ) {
	finance.this_year.liability_array[AC_LIABILITY_TOTAL] += finance.this_year.liability_array[i];
    }

    //------------------------//
    err_when(finance.this_year.asset_array[AC_RESIDENCE] < 0);
    err_when(new_places_expense_last_year<0);
    err_when(current_places < 0 || demand_places<0 || filled_places<0);

    err_when(int(total_expense) != int(total_revenue));
    err_when(percent_demanding_places_sl1 > 100 || percent_demanding_places_sl1 <0);

    //------------------------//
    update_history(UPDATE_ALL);
}

//------------- End of function StudentOffice::init_data --------//

//----------- Begin of function StudentOffice::next_day ------//
//! desc
//!
void StudentOffice::next_day() {
    // start of a month
    if ( info.game_day == 1 ) {
	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month ) {
	    run_residence_hall_yearly();                // shouldn't be called through init_data()
	    update_history(UPDATE_YEAR);
	}

	update_history(UPDATE_MONTH);
    }

    // start of a trimester
    if ( info.game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	 && info.game_month == player_school.trimester_array[player_school.cur_trimester].start_month ) {
	update_history(UPDATE_TRIMESTER);
    }

}

//------------- End of function StudentOffice::next_day ------//

//----------- Begin of function StudentOffice::update_history_sub ------//
//! desc
//!
void StudentOffice::update_history_sub() {
    tuition_income_history[H_PREV_YEAR] = (int) finance.revenue_array[AC_GROSS_TUITION_INCOME].total;
    student_life_history[H_PREV_YEAR] = (int) finance.expense_array[AC_STUDENT_LIFE].total;

    tuition_income_history[H_THIS_YEAR] = (int) finance.projected_revenue_array[AC_GROSS_TUITION_INCOME].this_year.total;
    student_life_history[H_THIS_YEAR] = (int) finance.projected_expense_array[AC_STUDENT_LIFE].this_year.total;

    tuition_income_history[H_NEXT_YEAR] = (int) finance.projected_revenue_array[AC_GROSS_TUITION_INCOME].next_year.total;
    student_life_history[H_NEXT_YEAR] = (int) finance.projected_expense_array[AC_STUDENT_LIFE].next_year.total;
}

//------------- End of function StudentOffice::update_history_sub ------//

//----------- Begin of function Athletics::update_history ------//
//!
//! update time-series graphs for interface display
//! A) win/lost; B) revenue history
//!
void StudentOffice::update_history(char updateFlag) {
    //float input;
    //int h;

    int h;

    switch (updateFlag) {
    case UPDATE_MONTH:
	update_history_sub();
	break;

    case UPDATE_TRIMESTER:

	//## chea 270999 for update stu_sum only
	for (h=0;h<MAX_STUDENT_LEVEL_N_TOTAL; h++)
	    shift_history(player_school.ins_student_level_history[h], HISTORY_TRIMESTER_COUNT);
	player_school.update_stu_sum();

	for (h=0;h<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; h++)
	    shift_history(player_school.ins_gender_student_level_history[h], HISTORY_TRIMESTER_COUNT);
	player_school.update_gender();              //## chea 290999

	break;

    case UPDATE_YEAR:
	//-----------------//
	shift_history(available_bed_count, HISTORY_YEAR_COUNT);
	shift_history(occupied_bed_count, HISTORY_YEAR_COUNT);

	available_bed_count[THIS_YEAR] = current_places;
	occupied_bed_count[THIS_YEAR] = filled_places;

	//-----------------//
	// not ($000)
	shift_history(tuition_rate_history, HISTORY_YEAR_COUNT);
	shift_history(room_n_board_rate_history, HISTORY_YEAR_COUNT);
	tuition_rate_history[THIS_YEAR] = finance.tuition_rate;
	room_n_board_rate_history[THIS_YEAR] = (int) room_n_board_rate;

	//-----------------//
	shift_history(tuition_income_history, HISTORY_YEAR_COUNT);
	shift_history(student_life_history, HISTORY_YEAR_COUNT);
	update_history_sub();                       //data from finance
	break;

    case UPDATE_ALL:
	// calling order is important for update_history_sub()
	update_history(UPDATE_YEAR);
	update_history(UPDATE_TRIMESTER);
	update_history(UPDATE_MONTH);
	break;

    default:
	err_here();
	break;
    }
}

//------------- End of function Athletics::update_history ------//

//----------- Begin of function StudentOffice::deinit --------//
//!
//! refer to HE.facilities.xls -> residence hall -> Column H
//!
void StudentOffice::run_residence_hall_yearly() {
    float inflationRate = (float) finance.inflation_rate / 100.0f;
    float staffGrowthRate = (float) finance.expense_policy_array[PL_STAFF_SALARY_INCREASES].result_value / 100.0f;

    // oldcode: H90=(1+'[HE.ResAlloc.ex]'!$C$72+'[HE.ResAlloc.ex]'!$C$78)*G90
    // replacement_cost_per_student_space *= float(1 + inflationRate + finance.construction_cost_growth/100.0f);	// not ($000)
    // 0.99->99%
    static float replacement_cost_growth = math.get_random_snd(0.030f,PSCH_SD(.01f));
    static float replacement_cost_growth_last2 = 0;

    float lastValue = replacement_cost_growth;
    replacement_cost_growth = math.time_variation(lastValue, replacement_cost_growth_last2, 1.4482f, -0.8011f, 0.003528950f);
    replacement_cost_growth_last2 = lastValue;

    replacement_cost_per_student_space = int( replacement_cost_per_student_space * (1+inflationRate+replacement_cost_growth) );

    err_when(replacement_cost_per_student_space <= 0);

    raw_initial_onm_cost_per_student_space *= float(1 + 0.25f * inflationRate + 0.75f * staffGrowthRate );
    raw_initial_variable_cost_per_occupied_space *= float(1 + 0.5f * inflationRate + 0.5f * staffGrowthRate );

    //------ Capacity, demand, and revenue -------//

    //0106 percent_demanding_places_sl1 += 100 * math.get_random_snd(0, 0.025f*float(sqrt(percent_demanding_places_sl1/100*(1-percent_demanding_places_sl1/100))));
    //0106 percent_demanding_places_sl1 = max(0,percent_demanding_places_sl1);

    // Student spaces take 12 months to build. All spaces come on line at the beginning of the year.
    current_places += new_places;
    demand_places = int(percent_demanding_places_sl1 * enroll_res.student_count[UG_TRADITION] / 100);
    filled_places = min(current_places, demand_places);

    //------------------------//
    // =('[HE.ResAlloc.ex]Sheet1'!$C$74+'[HE.ResAlloc.ex]Sheet1'!$C$75)*H117
    // ($000)
    float debtBalance = (float) finance.last_year.liability_array[AC_RESIDENCE_HALL_DEBT];
    // ($000)
    float serviceOnDebt = debtBalance * float(finance.debt_fraction_repaid_annually + finance.long_term_debt_interest) / 100;

    // ($000)
    float fixed_onm = raw_initial_onm_cost_per_student_space * current_places / 1000;
    // ($000)
    float variable_cost = raw_initial_variable_cost_per_occupied_space * filled_places / 1000;
    // ($000)
    float total_expense = serviceOnDebt + fixed_onm + variable_cost;

    if ( filled_places > 0 )
	room_n_board_rate = (1000.0f * total_expense / filled_places);
    else
	room_n_board_rate = 0;                        //askbill this special case

    //------------------------//
    // update total_revenue after calc of room and board rate

    // ($000)
    float total_revenue = float(filled_places * room_n_board_rate) / 1000;

    //------------------------//

    int unmetDemand = demand_places - filled_places;
    if ( accomodate_excess_demand
	 && math.safe_divide(unmetDemand * 100, demand_places) > percent_excess_demand_before_capacity_increased
	)
	new_places = unmetDemand;
    else
	new_places = 0;

    finance.this_year.asset_array[AC_RESIDENCE]
	= new_places_expense_last_year + finance.last_year.asset_array[AC_RESIDENCE];
    finance.this_year.liability_array[AC_RESIDENCE_HALL_DEBT]
	= new_places_expense_last_year
	+ finance.last_year.liability_array[AC_RESIDENCE_HALL_DEBT] * ( 1-finance.debt_fraction_repaid_annually / 100.0f);

    //($000)
    new_places_expense_last_year = replacement_cost_per_student_space * new_places / 1000.0f;

    finance.this_year.liability_array[AC_LIABILITY_TOTAL] = 0;
    for( int i=0 ; i<LIABILITY_ITEM_COUNT-1 ; i++ ) {
	finance.this_year.liability_array[AC_LIABILITY_TOTAL] += finance.this_year.liability_array[i];
    }

    //------------------------//

    err_when(finance.this_year.asset_array[AC_RESIDENCE] < 0);
    err_when(new_places_expense_last_year<0);
    err_when(current_places < 0 || demand_places<0 || filled_places<0);
    err_when(int(total_expense) != int(total_revenue) && total_revenue != 0);
    err_when(percent_demanding_places_sl1 > 100 || percent_demanding_places_sl1 <0);
}

//------------- End of function StudentOffice::deinit --------//
