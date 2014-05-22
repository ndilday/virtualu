//Filename    : Olib_st.cpp
//Description : LibTech Class Definition related to Simulation and Statistics/History
//Owner       : Fred

// ES header file
#include <OMATH.H>

// CU header file
#include <OINFO.H>
#include <OENROLL.H>
#include <OPSCHOOL.H>
#include <ODEPT.H>
#include <OFACULTY.H>
#include <OFinance.H>
#include "Olibtech.h"
#include "OStuOff.h"

//----------- Define static variables -------------//
//----------- Define static functions -------------//
//----------- Define static class member variables -------------//

//----------- Begin of function LibTech::init_data -----//
//! desc
//! <int> -
//! [int] -
//!
void LibTech::init_data() {
    //assume called: memset(this, 0, sizeof(LibTech));
    //assume finance and enrollment called first

    // IF input
    faculty_incentive_using_it = 50;                // = [0:100]

    // IF output

    float remap[] = { 2, 1, 0.5f };
    int enrollment = enroll_res.total_student_count;

    // 1221; update from latest email
    volumns_count_library = int(76.60429808f * exp((float)math.get_random_snd(0,PSCH_SD(0.1f))) * enrollment *remap[player_school.relative_wealth] );

    //------------// graph

    update_history(UPDATE_ALL);
}

//------------- End of function LibTech::init_data -----//

//----------- Begin of function LibTech::next_day ------//
//!
void LibTech::next_day() {
    // start of a month
    if ( info.game_day == 1 ) {
	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month )
	    update_history(UPDATE_YEAR);

	update_history(UPDATE_MONTH);
    }

    // start of a trimester
    if ( info.game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	 && info.game_month == player_school.trimester_array[player_school.cur_trimester].start_month ) {
	update_history(UPDATE_TRIMESTER);
    }
}

//------------- End of function LibTech::next_day ------//

//---------- Begin of function LibTech::update_history -----------//
//!
//! update history arrays
//!
//! update_flag		= UPDATE_TRIMESTER, UPDATE_MONTH, UPDATE_YEAR; (ooffice.h)
//!
void LibTech::update_history(char update_flag) {
    switch (update_flag) {
    case UPDATE_MONTH:
	shift_history(library_history, HISTORY_MONTH_COUNT);
	shift_history(it_history,HISTORY_MONTH_COUNT);
	shift_history(overall_history,HISTORY_MONTH_COUNT);
	shift_history(volumns_count_library_history,HISTORY_MONTH_COUNT);
	shift_history(percent_faculty_using_it_history,HISTORY_MONTH_COUNT);
	shift_history(internet_quality_history,HISTORY_MONTH_COUNT);
	shift_history(tech_utilization_history,HISTORY_MONTH_COUNT);

	update_history_monthly();

	volumns_count_library_history[THIS_MONTH] = volumns_count_library;
	percent_faculty_using_it_history[THIS_MONTH] = percent_faculty_using_it;
	internet_quality_history[THIS_MONTH] = internet_quality;
	tech_utilization_history[THIS_MONTH] = tech_utilization;
	break;

    case UPDATE_TRIMESTER:
	calc_it_usage();
	shift_history(net_dl_expenditures_history, HISTORY_TRIMESTER_COUNT);
	net_dl_expenditures_history[H_PREV_TRIMESTER] = (float) finance.net_distance_learning_expense.total/1000;
	net_dl_expenditures_history[H_THIS_TRIMESTER] = (float) finance.projected_net_distance_learning_expense.this_year.total/1000;
	net_dl_expenditures_history[H_NEXT_TRIMESTER] = (float) finance.projected_net_distance_learning_expense.next_year.total/1000;
	if ( info.prerun_year )                     // delete the abnormal large record due to initial game data
	    net_dl_expenditures_history[19] = 0;
	break;

    case UPDATE_YEAR:
	break;

    case UPDATE_ALL:
	update_history(UPDATE_YEAR);
	update_history(UPDATE_TRIMESTER);
	update_history(UPDATE_MONTH);
	break;
    default:
	err_here();
	break;
    }
}

//---------- End of function Department::update_history -----------//

//----------- Begin of function LibTech::update_history_monthly ------//
//!
void LibTech::update_history_monthly() {
    int i, j; {

	//----//

	library_history[H_PREV_MONTH] = (float) finance.expense_array[AC_LIBRARIES].total;
	it_history[H_PREV_MONTH] = (float) finance.expense_array[AC_IT_RESOURCES].total;
	overall_history[H_PREV_MONTH] = (float) it_history[H_PREV_MONTH] + library_history[H_PREV_MONTH]; {
	}

	//----//

	library_history[H_THIS_MONTH] = (float) finance.projected_expense_array[AC_LIBRARIES].this_year.total;
	it_history[H_THIS_MONTH] = (float) finance.projected_expense_array[AC_IT_RESOURCES].this_year.total;
	overall_history[H_THIS_MONTH] = it_history[H_THIS_MONTH] + library_history[H_THIS_MONTH];
    }

    //----//
    library_history[H_NEXT_MONTH] = (float) finance.projected_expense_array[AC_LIBRARIES].next_year.total;
    it_history[H_NEXT_MONTH] = (float) finance.projected_expense_array[AC_IT_RESOURCES].next_year.total;
    overall_history[H_NEXT_MONTH] = library_history[H_NEXT_MONTH] + it_history[H_NEXT_MONTH];

    //----------//
    // input or output
    float oldInternetQuality = internet_quality_percent;

    internet_quality_percent = it_history[H_THIS_MONTH] * 100 / (float) finance.total_operating_expense.total * 5;
    //	internet_quality_percent = math.dual_response_func( 0,50,100,3.02f,2.35f,2.01f,5.54f,internet_quality_percent);
    internet_quality_percent = math.dual_response_func( 0,50,100,3.02f,2.35f,2.01f,15.54f,internet_quality_percent);

    //## chea 201099
    //	internet_quality_percent = min(100.0f,math.latency_func( 0.96f, oldInternetQuality, internet_quality_percent));// min & max bug chea   7.87
    // min & max bug chea   7.87
    internet_quality_percent = min(100.0f,player_school.latency_func( 0.94f, oldInternetQuality, internet_quality_percent));

    internet_quality = char(internet_quality_percent * (INTERNET_QUALITY_LEVEL_COUNT-1) / 100);

    //----------//
    float growth = library_history[H_THIS_MONTH] / volumns_count_library;
    growth = math.single_response_func(0.0f,0.90f,140,75,growth);

    volumns_count_library = int(volumns_count_library * (1+growth/100));

    err_when(volumns_count_library == 0);

    //----------//
    float totalPercent = 0;
    int totalFacultyCount = 0;

    for ( i=department_array.size(); i>0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;

	Department* deptPtr = department_array[i];

	float percentIT = 0;
	int facultyCount = 0;

	for( j=deptPtr->faculty_array.size() ; j>0 ; j-- ) {
	    if ( deptPtr->faculty_array.is_deleted(i) )
		continue;

	    percentIT += deptPtr->faculty_array[j]->p_discretionary_hour_it_percent;
	    facultyCount++;
	}

	totalFacultyCount += facultyCount;
	totalPercent += percentIT;
    }

    if ( totalFacultyCount )
	percent_faculty_using_it = (totalPercent)/totalFacultyCount;
    else
	percent_faculty_using_it = 0;

    //----------//
}

//------------- End of function LibTech::update_history_monthly ------//

//----------- Begin of function LibTech::calc_it_usage ------//
//!
//! update time-series graphs for interface display
//!
void LibTech::calc_it_usage() {
    int i,j;

    // Item 8d iii)
    // calculate tech_utilization for display
    float totalEdDevHrs=0;
    int deptCount=0;
    for ( i=department_array.size(); i>0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;

	totalEdDevHrs += department_array[i]->p_tech_utilization;
	deptCount++;
    }

    if ( deptCount != 0 )
	tech_utilization = totalEdDevHrs / deptCount;
    else
	tech_utilization = 0;

}

//------------- End of function LibTech::calc_it_usage ------//

//----------- Begin of function LibTech::init --------//
//!
//!void LibTech::update_win_lost(bool isFootballGame)
//!{

//}
//------------- End of function LibTech::init --------//

// ##### Begin Marco ##### //
//----------- Begin of function LibTech::save_initial_data  --------//
//!
void LibTech::save_initial_data() {
    initial_volumns_count_library = volumns_count_library;
    initial_percent_faculty_using_it = percent_faculty_using_it;
    initial_internet_quality_percent = internet_quality_percent;
    initial_tech_utilization = tech_utilization;
    initial_faculty_incentive_using_it = faculty_incentive_using_it;
    initial_percent_dl_tuition = percent_dl_tuition;
}

//------------- End of function LibTech::save_initial_data --------//
// ##### End Marco ##### //
