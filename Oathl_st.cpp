//Filename    : OAthl_st.cpp
//Description : Athletics Class Definition related to Simulation and Statistics/History
//Owner       : Fred

// ES library header file
#include <OMATH.H>

// CU header file
#include <OINFO.h>
#include <ONEWS.h>
#include <OCHANCE.H>
#include <OPSCHOOL.h>
#include <OFinance.h>
#include <OEnroll.H>
#include <OSCHLRES.h>
#include <OPEERSCH.h>
#include "OAthleti.h"

//----------- Define constants -------------//
//int Athletics::football_game_count[NCAA_LEVEL_COUNT] = { 10, 11, 12 };
//int Athletics::basketball_game_count[NCAA_LEVEL_COUNT] = { 25, 30, 35 };

//----------- Define static variables -------------//
static int base_cost_array[NCAA_LEVEL_COUNT] = { 700, 1800, 3000 };
static int base_revenue_array[NCAA_LEVEL_COUNT] = { 125, 650, 2500};

//----------- Define static functions -------------//
//----------- Define static class member variables -------------//

//----------- Begin of function Athletics::init_data  -----//
//!
void Athletics::init_data_pre_finance() {
    //0304 subce memset(athletic_office,... in battle.cpp

    int dum1[] = { 10, 11, 12 };
    int dum2[] = { 25, 30, 35 };

    memcpy(football_game_count, dum1, sizeof(dum1));
    memcpy(basketball_game_count, dum2, sizeof(dum2));

    //---------------------//
    // refer to email: Re: Q24. misc.

    /*
      >>> During game initialization:

      * Parse total athletics cost as follows:

      InterCollCost[0]  = Athletics:D [initLevel]

      FrInterCollCost   = InterCollCost[0]/TotAthCost[0]

      TotAthCost[0]     = HE.Initialization:Initial_Conditions:(cols AP + AQ)
    */

    err_when(NCAA_LEVEL_COUNT != INPUT_OPTION_COUNT);

    PeerSchool *player = school_res.player_peer_school;

    int interCollCost = base_cost_array[ncaa_level_input];
    int totalCost = player->athletics_salaries + player->athletics_other_expense;

    fraction_intercollege_cost = char(interCollCost * 100 / totalCost);
}

//----------- End of function Athletics::init_data_pre_finance -----//

//----------- Begin of function Athletics::init_data -----//
//!
void Athletics::init_data() {
    //assume called: memset(this, 0, sizeof(Athletics));

    init_expense = (int) finance.expense_array[AC_ATHLETICS].total;

    //assume after enroll.init_data() called
    init_top_male_matrics = enroll_res.male_matrics_top_athletes;
    init_top_female_matrics = enroll_res.matrics_top_athletes - enroll_res.male_matrics_top_athletes;

    if ( !init_top_male_matrics )
	init_top_male_matrics = 10;
    if ( !init_top_female_matrics )
	init_top_female_matrics = 7;

    init_ability_male_matrics = 50;
    init_ability_female_matrics = 50;

    total_cost_norm = (int) finance.expense_array[AC_ATHLETICS].total;

    calc_probability_win();

    init_win_prob = (football_probability_win + basketball_probability_win ) / 2;

    err_when(init_expense <=0 || init_top_male_matrics<=0 || init_top_female_matrics<=0
	     || init_win_prob<=0
	     || init_ability_female_matrics<=0 || init_ability_male_matrics<=0);

    //---------------------//
    // assume called after finance
    update_history(UPDATE_ALL);
}

//------------- End of function Athletics::init_data -----//

//----------- Begin of function Athletics::next_day ------//
//!
void Athletics::next_day() {
    static short trimesterDays = 0;

    bool isFootballGame;
    short trimesterTotalDays = player_school.trimester_array[player_school.cur_trimester].total_days;
    short tmp = -1;

    //------ after initalization, reset the variables ----//

    if( info.prerun_year==0 && info.game_year==1 && info.game_month==9 && info.game_day==2 ) {
	avg_percent_win = 0;
	football_percent_win = 0;
	basketball_percent_win = 0;
	total_game = 0;
	total_football_game = 0;
	total_basketball_game = 0;
    }

    //---------------------------------------------//

    // Fall: update football men
    if ( player_school.cur_trimester == AUTUMN ) {
	isFootballGame = true;

	if ( info.is_begin_trimester() ) {
	    trimesterDays = 0;

	    // assume optimization is called first
	    //update_history(UPDATE_YEAR);
	}

	tmp = trimesterTotalDays / football_game_count[ncaa_level_input];
    }

    // Spring: update women basketball
    else if ( player_school.cur_trimester == WINTER ) {
	isFootballGame = false;
	if ( info.game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	     && info.game_month == player_school.trimester_array[player_school.cur_trimester].start_month )
	    trimesterDays = 0;

	tmp = trimesterTotalDays / basketball_game_count[ncaa_level_input];
    }
    else
	tmp = -1;

    trimesterDays++;
    if ( tmp > 0 && trimesterDays % tmp == 0 ) {    // it's time to play
	update_win_lost(isFootballGame);
    }

    // start of a month
    if ( info.game_day == 1 ) {
	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month ) {
	    last_ncaa_level_input = ncaa_level_input;   // post last_ncaa_level_input
	    update_history(UPDATE_YEAR);
	}

	update_history(UPDATE_MONTH);
    }
}

//------------- End of function Athletics::next_day ------//

//----------- Begin of function Athletics::update_win_lost --------//
//!
void Athletics::update_win_lost(bool isFootballGame) {
    bool win;
    char graphIndex;
    char incr;

    if ( isFootballGame ) {
	if( total_football_game >= football_game_count[ncaa_level_input] * info.year_passed )
	    return;

	total_game++;
	total_football_game++;

	win = (math.get_random_float() < football_probability_win) ? true : false;
	graphIndex = (win) ? HISTORY_YEAR_COUNT-1 : HISTORY_YEAR_COUNT2-1;
	incr = (win) ? 1: 0;

	avg_percent_win = ((total_game-1) * avg_percent_win +incr) / total_game;
	football_percent_win = ((total_football_game-1) * football_percent_win +incr) / total_football_game;

	football_graph[graphIndex]++;

	if(info.prerun_year == 0 &&
	   // only in December or January
	   (info.game_month == 12 || info.game_month == 1) ) {
	    if ( win || chance_event.test_chance==NEWS_FOOTBALL_GAME )
		// 990427
		news_array.football_game( football_graph[HISTORY_YEAR_COUNT-1] );
	}
    }
    else {
	if( total_basketball_game >= basketball_game_count[ncaa_level_input]  * info.year_passed )
	    return;

	total_game++;
	total_basketball_game++;

	win = (math.get_random_float() < basketball_probability_win) ? true : false;
	graphIndex = (win) ? HISTORY_YEAR_COUNT-1 : HISTORY_YEAR_COUNT2-1;
	incr = (win) ? 1: 0;

	avg_percent_win = ((total_game-1) * avg_percent_win + incr) / total_game;
	basketball_percent_win = ((total_basketball_game-1) * basketball_percent_win +incr) / total_basketball_game;

	basketball_graph[graphIndex]++;

	// only in March
	if( info.prerun_year==0 && info.game_month==3 ) {
	    if ( win || chance_event.test_chance==NEWS_BASKETBALL_GAME )
		// 990505
		news_array.basketball_game( basketball_graph[HISTORY_YEAR_COUNT-1] );
	}
    }
}

//------------- End of function Athletics::update_win_lost --------//

//----------- Begin of function Athletics::update_history_sub --------//
//!
void Athletics::update_history_sub() {
    history_value[FIRST_H_THIS_YEAR-1] = (float) finance.revenue_array[AC_INTERCOLLEGIATE_ATHLETICS].total;
    history_value[SECOND_H_THIS_YEAR-1] = (float) finance.expense_array[AC_ATHLETICS].total;

    history_value[FIRST_H_THIS_YEAR] = (float) finance.projected_revenue_array[AC_INTERCOLLEGIATE_ATHLETICS].this_year.total;
    history_value[SECOND_H_THIS_YEAR] = (float) finance.projected_expense_array[AC_ATHLETICS].this_year.total;

    history_value[FIRST_H_THIS_YEAR+1] = (float) finance.projected_revenue_array[AC_INTERCOLLEGIATE_ATHLETICS].next_year.total;
    history_value[SECOND_H_THIS_YEAR+1] = (float) finance.projected_expense_array[AC_ATHLETICS].next_year.total;
}

//------------- End of function Athletics::update_history_sub --------//

//----------- Begin of function Athletics::update_history ------//
//!
//! update time-series graphs for interface display
//! A) win/lost; B) revenue history
//!
void Athletics::update_history(char update_flag) {
    float input;
    int h;

    switch (update_flag) {
    case UPDATE_MONTH:
	input = (avg_percent_win) / init_win_prob;

	input = math.dual_response_func(0.300f,1.980f,3,1.279f,2.345f,1.087f,3.243f, input);

	//		revenue_multiplier = math.latency_func(0.89f, revenue_multiplier, input);
	//## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
	revenue_multiplier = player_school.latency_func(0.85f, revenue_multiplier, input);

	//askbill: eq. below and why monthly
	/*
	  BaseAthleticsRev =
	  BaseAthleticsRev*(1+finance.inflation_rate / 100.0f);

	  AthleticsRev[t] =	multiplier*BaseAthleticsRev;
	*/
	update_history_sub();                       // this line shouldn't be called by init_data()!!??
	break;

    case UPDATE_YEAR:
	calc_probability_win();

	// part 1	// shift_win_lost_graph

	for ( h=0; h<HISTORY_YEAR_COUNT2-1; h++) {
	    football_graph[h] = football_graph[h+1];
	    basketball_graph[h] = basketball_graph[h+1];
	}
	basketball_graph[h] = football_graph[h] = 0;

	h = HISTORY_YEAR_COUNT-1;
	basketball_graph[h] = football_graph[h] = 0;

	// part 2

	// 1 of 2: shift history to "left"
	//

	shift_history(history_value, HISTORY_YEAR_COUNT2);

	// 2 of 2: update current year values
	update_history_sub();
	break;

    case UPDATE_ALL:
	update_history(UPDATE_YEAR);
	update_history(UPDATE_MONTH);

	avg_percent_win = 0;
	football_percent_win = 0;
	basketball_percent_win = 0;
	total_game = 0;
	total_football_game = 0;
	total_basketball_game = 0;
	break;

    default:
	err_here();
	break;
    }
}

//------------- End of function Athletics::update_history ------//

//---------- Begin of function Athletics::calc_probability_win -----------//
//!
void Athletics::calc_probability_win() {
#define VAR_COUNT 3
    int i;
    float input[VAR_COUNT];
    memset(input, 0, sizeof(input));

    // For male

    //--------------//
    // 1103 athletic expenditures/normal athletic expenditures for chosen level of competition

    input[0] = (float) finance.projected_expense_array[AC_ATHLETICS].this_year.total / total_cost_norm;

    //----//
    input[0] *= (float) finance.cost_rise_policy_array[PL_ATHLETICS].penalty_multiplier2;

    //--------------//
    input[1] = float(enroll_res.male_matrics_top_athletes) / init_top_male_matrics;

    //--------------//
    input[2] = float(enroll_res.avg_athletic_talent_male_matrics) / init_ability_male_matrics;

    //--------------//
    input[0] = math.dual_response_func(0.200f,0.380f,0.800f,0.218f,1.441f,0.588f,1.266f, input[0]);
    input[1] = math.dual_response_func(0.200f,0.356f,0.800f,0.263f,1.528f,0.236f,1.219f, input[1]);
    input[2] = math.dual_response_func(0.200f,0.395f,0.800f,0.291f,1.773f,0.380f,1.367f, input[2]);

    float weight[VAR_COUNT] = {0.50f, 0.25f, 0.25f, };

    float newValue = 0;
    for(i=0; i<VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    if( ncaa_level_input==ATHLETICS_DIVISION_1 )
	newValue *= 0.666f;
    else if( ncaa_level_input==ATHLETICS_DIVISION_3 )
	newValue += (1-newValue) * 0.333f;

    //	football_probability_win = math.latency_func( 0.33f, football_probability_win, newValue);
    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    football_probability_win = player_school.latency_func( 0.40f, football_probability_win, newValue);

    //--------------//
    // For female
    //--------------//

    input[0] = (float) finance.projected_expense_array[AC_ATHLETICS].this_year.total / total_cost_norm;

    //----//
    input[0] *= (float) finance.cost_rise_policy_array[PL_ATHLETICS].penalty_multiplier2;

    //--------------//
    input[1] = float(enroll_res.matrics_top_athletes - enroll_res.male_matrics_top_athletes )/ init_top_female_matrics;

    //--------------//
    input[2] = float(enroll_res.avg_athletic_talent_female_matrics) / init_ability_female_matrics;

    //--------------//
    input[0] = math.dual_response_func(0.200f,0.500f,0.800f,0.401f,0.401f,0.701f,1.299f, input[0]);
    input[1] = math.dual_response_func(0.200f,0.621f,0.800f,0.768f,0.883f,0.833f,1.665f, input[1]);
    input[2] = math.dual_response_func(0.200f,0.588f,0.800f,0.779f,0.839f,0.787f,1.755f, input[2]);

    newValue = 0;
    for(i=0; i<VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    if( ncaa_level_input==ATHLETICS_DIVISION_1 )
	newValue *= 0.666f;
    else if( ncaa_level_input==ATHLETICS_DIVISION_3 )
	newValue += (1-newValue) * 0.333f;

    //	basketball_probability_win = math.latency_func( 0.33f, basketball_probability_win, newValue);
    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    basketball_probability_win = player_school.latency_func( 0.35f, basketball_probability_win, newValue);

}

//---------- End of function Athletics::calc_probability_win -----------//

//---------- Begin of function Athletics::input_2_ncaa_level -----------//
//!
//!
//!
void Athletics::input_2_ncaa_level(int newNcaaLevel) {
    //990528 calc_adjusted_revenue_expense(int &revenue, int &expense);

    //--------------//

    //	float remap[] = {4.5f, 3, 1.5f};	// High:   value = 1.5; Medium: value = 3; Low:    value = 4.5
    float remap[] = {3,3,3};

    ncaa_level_input = newNcaaLevel;                // set the current one to new one

    ncaa_level_value = remap[ncaa_level_input];     // get the new level value

    err_when(INPUT_LOW != 0 || ncaa_level_input > 2);
}

//---------- End of function Athletics::input_2_ncaa_level -----------//

//---------- Begin of function Athletics::calc_adjusted_revenue_expense -----------//
//!
//!
//!
void Athletics::calc_adjusted_revenue_expense(int &revenue, int &expense) {
    /*
      req24.txt:

      >>> At the time of a competition level change (in year t):

      * ChangeRatioInterCollCost = Athletics:D [newLevel]/Athletics:D[oldLevel]
      ChangeRatioAthRevenue    = Athletics:G [newLevel]/Athletics:G[oldLevel]

      * ExpenditureNormalizationRatio = Athletics:D [newLevel]/TotAthCost[t]

      * TotalAthCost[t#] = FrInterCollCost * TotAthCost[t] *
      ChangeRatioInterCollCost +  (1-FrInterCollCost)*TotAthCost[t]);
      then set the "other" cost to 60% of
      total cost and salary cost to the remainder

      AthRevenue[t#]   = AthRevenue[t] * ChangeRatioAthRevenue

      # Substitute the new values for the old ones just before the resource allocation
      for year t+1: the don't appear in the year-t financial but they represent the
      starting values for the resource allocation.

      * AthCostNorm[t+1] = Athletics:G [newLevel] * ExpenditureNormalizationRatio

      (Used to calculat win probabilities starting in year t+1.)

    */
    float changeRatioInterCollCost = float(base_cost_array[ncaa_level_input]) / base_cost_array[last_ncaa_level_input];
    float changeRatioAthRevenue = float(base_revenue_array[ncaa_level_input]) / base_revenue_array[last_ncaa_level_input];

    //---------------//
    expense = (int) finance.expense_array[AC_ATHLETICS].total;
    float expenditureNormalizationRatio = float(base_cost_array[ncaa_level_input]) / expense;

    expense = int( fraction_intercollege_cost * expense * changeRatioInterCollCost / 100.0f
		   + (1.0f-fraction_intercollege_cost/100) * expense );

    revenue = (int) finance.revenue_array[AC_INTERCOLLEGIATE_ATHLETICS].total;
    revenue = int(revenue * changeRatioAthRevenue);

    //---------------//
    total_cost_norm = int(base_revenue_array[ncaa_level_input] * expenditureNormalizationRatio);

    err_when(total_cost_norm<=0);
}

//---------- End of function Athletics::calc_adjusted_revenue_expense -----------//

//---------- Begin of function Athletics::save_initial_data -----------//
//!
void Athletics::save_initial_data() {
    // use result of pre-run year
    initial_football_graph[0] = football_graph[HISTORY_YEAR_COUNT-2];
    initial_football_graph[1] = football_graph[1*HISTORY_YEAR_COUNT+HISTORY_YEAR_COUNT-2];
    initial_basketball_graph[0] = basketball_graph[HISTORY_YEAR_COUNT-2];
    initial_basketball_graph[1] = basketball_graph[1*HISTORY_YEAR_COUNT+HISTORY_YEAR_COUNT-2];
}

//---------- End of function Athletics::save_initial_data -----------//
