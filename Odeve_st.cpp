//Filename    : ODeve_st.cpp
//Description : Development Class Definition related to Simulation and Statistics/History
//Owner       : Fred

// ES library header file
#include <OMATH.H>

// CU header file
#include <OINFO.H>
#include <OCHANCE.H>
#include <OCHANCE.H>
#include <OPSCHOOL.H>
#include <OPEERSCH.H>
#include <OSCHLRES.H>
#include <OFinance.H>

#include <OATHLETI.H>
#include "ODevelop.h"

//----------- Define static variables -------------//
//----------- Define static functions -------------//
//----------- Define static class member variables -------------//

//----------- Begin of function Development::init_data -----//
//! desc
//! <int> -
//! [int] -
//!
void Development::init_data() {
    //assume called: memset(this, 0, sizeof(Development));
    allocation_policy.cur_operations = 0.50f;
    allocation_policy.endowment = 0.30f;
    allocation_policy.facilities = 0.20f;

    /*
      last_year.total = (float) finance.revenue_array[AC_GIFTS].total;
      last_year.cur_operations = last_year.total * allocation_policy.cur_operations ;
      last_year.endowment = last_year.total * allocation_policy.endowment ;
      last_year.facilities = last_year.total * allocation_policy.facilities ;
      this_month_total_gift = (float) finance.revenue_array[AC_GIFTS].total / 12;
      //next_month_total_gift = this_month_total_gift;
    */

    // 990417

    last_year.total = (float) finance.revenue_array[AC_GIFTS].total / allocation_policy.cur_operations;
    last_year.cur_operations = last_year.total * allocation_policy.cur_operations ;
    last_year.endowment = last_year.total * allocation_policy.endowment ;
    last_year.facilities = last_year.total * allocation_policy.facilities ;
    this_month_total_gift = last_year.cur_operations / 12;

    err_when(this_month_total_gift <= 0);
    memcpy(&this_year, &last_year, sizeof(GiftComponent));

    GiftComponent saveLastYear;
    memcpy(&saveLastYear, &last_year, sizeof(GiftComponent));

    init_inst_advancement = (int)finance.expense_array[AC_INST_ADVANCEMENT].total;
    init_prestige = (int) school_res.player_peer_school->prestige;

    update_history(UPDATE_ALL);

    // call after update_history
    memcpy(&last_year, &saveLastYear, sizeof(GiftComponent));

    err_when(this_year.total<=0||init_inst_advancement<=0 || init_prestige<=0);
}

//------------- End of function Development::init_data -----//

//----------- Begin of function Development::next_day ------//
//!
void Development::next_day() {
    // start of a month
    if ( info.game_day == 1 ) {
	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month )
	    update_history(UPDATE_YEAR);

	//		update_history_sub();		// this line shouldn't be called by init_data()!!??
	update_history(UPDATE_MONTH);

    }

    // start of a trimester
    if ( info.game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	 && info.game_month == player_school.trimester_array[player_school.cur_trimester].start_month ) {
	update_history(UPDATE_TRIMESTER);
    }
}

//------------- End of function Developmen::next_day ------//

//----------- Begin of function Development::init --------//
//!
//!void Development::update_win_lost(bool isFootballGame)
//!{
//!}
//!------------- End of function Development::init --------//

//----------- Begin of function Development::update_history_sub --------//
//!
void Development::update_history_sub() {
    history_value[FIRST_H_THIS_YEAR-1] = (float) last_year.total;
    history_value[SECOND_H_THIS_YEAR-1] = (float) finance.expense_array[AC_INST_ADVANCEMENT].total;

    history_value[FIRST_H_THIS_YEAR] = (float) this_year.total;
    history_value[SECOND_H_THIS_YEAR] = (float) finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.total;

    history_value[FIRST_H_THIS_YEAR+1] = (float) gift_total_next_year;
    history_value[SECOND_H_THIS_YEAR+1] = (float) finance.projected_expense_array[AC_INST_ADVANCEMENT].next_year.total;
}

//------------- End of function Development::update_history_sub --------//

//----------- Begin of function Development::update_history ------//
//!
//! update time-series graphs for interface display
//! A) win/lost; B) revenue history
//!
void Development::update_history(char update_flag) {
    //int h;

    switch (update_flag) {
    case UPDATE_MONTH:
	//		shift_history(history_value, HISTORY_YEAR_COUNT2);
	calc_gift_growth_rate();
	calc_gift_all();
	update_history_sub();

	break;

    case UPDATE_TRIMESTER:
	break;

    case UPDATE_YEAR:
	shift_history(percent_alumni_gave_gift_history, HISTORY_YEAR_COUNT);
	shift_history(alumni_morale_history, HISTORY_YEAR_COUNT);

	//-----------//

	calc_percent_alumni_gave_gift();
	percent_alumni_gave_gift_history[THIS_YEAR] = percent_alumni_gave_gift;

	calc_alumni_morale();
	alumni_morale_history[THIS_YEAR] = alumni_morale;

	//-----------//

	memcpy(&last_year, &this_year, sizeof(GiftComponent));
	memset(&this_year, 0, sizeof(GiftComponent));

	//## chea 091299 7.2.1 try to disp the graph
	shift_history(history_value, HISTORY_YEAR_COUNT2);

	calc_gift_growth_rate();
	calc_gift_all();

	//-----------//
	// 1 of 2: shift history to "left"
	//
	//## chea 031299 7.2.1 try to disp the graph
	//		shift_history(history_value, HISTORY_YEAR_COUNT2);
	//		shift_history(history_value, HISTORY_YEAR_COUNT2);

	// 2 of 2: update current year values

	update_history_sub();
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

//------------- End of function Development::update_history ------//

//---------- Begin of function Development::calc_gift_growth_rate -----------//
void Development::calc_gift_growth_rate() {
    const char M_VAR_COUNT=3;
    int i;
    float input[M_VAR_COUNT];
    memset(input, 0, sizeof(input));
    //--------------//
    input[0] = (float) finance.expense_array[AC_INST_ADVANCEMENT].total / init_inst_advancement;

    //----//
    input[0] *= (float) finance.cost_rise_policy_array[PL_INST_ADVANCEMENT].penalty_multiplier1;

    //--------------//
    if ( info.prerun_year )
	input[1] = float( school_res.player_peer_school->prestige )/ init_prestige;
    else
	input[1] = float( player_school.sub_score[S_PRESTIGE][THIS_MONTH] ) / player_school.sub_score_start[S_PRESTIGE];

    //--------------//
    input[2] = athletics_office.avg_percent_win;

    //--------------//
    input[0] = math.single_response_func(-0.012f,0.020f,1.000f,1.007f, input[0]);
    input[1] = math.dual_response_func(-0.012f,0.0048f,0.020f,5.516f,0.337f,0.588f,1.493f, input[1]);
    input[2] = math.dual_response_func(-0.012f,-0.0002f,0.020f,25.89f,28.74f,20.00f,75.00f, input[2]);

    float weight[M_VAR_COUNT] = {0.35f, 0.45f, 0.15f, };

    //Adjust the weight based on the current NCAA level of competition:
    //top level = weight as given (.15); mid-level = .05; lowest level = 0;
    //redistribute the reduced values to the other items proportionally.
    if ( athletics_office.ncaa_level_input == INPUT_MEDIUM )
	weight[2] = 0.05f;
    else if ( athletics_office.ncaa_level_input == INPUT_LOW )
	weight[2] = 0.0f;

    float remainWeight = 1 - weight[0] - weight[1] - weight[2];
    weight[0] += remainWeight/2;
    weight[1] += remainWeight/2;

    float newValue = 0;
    for(i=0; i<M_VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    //gift_growth_rate = math.latency_func( 0.9f, gift_growth_rate, newValue);
    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    gift_growth_rate = player_school.latency_func( 0.85f, gift_growth_rate, newValue);

    if ( chance_event.gift_growth_rate_multiplier )
	gift_growth_rate *= chance_event.gift_growth_rate_multiplier;
}

//---------- End of function Development::calc_gift_growth_rate -----------//

//---------- Begin of function Development::calc_gift_all -----------//
void Development::calc_gift_all() {

    //--------------//
    // value[t] = (1+inflation+realGR)*value[t-1]
    //	value[0] = initialization value of total gifts DIVIDED BY 12

    //static float next_month_total_gift;

    //this_month_total_gift = next_month_total_gift;

    //0115: moved this to finance: next_month_total_gift = float(1+finance.inflation_rate/1200+gift_growth_rate)
    //								* this_month_total_gift;

    //this_year.total += this_month_total_gift;

    /*
      //## chea orig. method it doesn't work

      float curOp = (float) finance.projected_revenue_array[AC_GIFTS].this_year.total;
      if (allocation_policy.cur_operations <= 0)
      allocation_policy.cur_operations = 0.01f;
      this_year.total = curOp / allocation_policy.cur_operations;

      //--------------//
      float mean = allocation_policy.cur_operations;
      this_year.cur_operations = curOp;	//990417		//this_year.total * max(0, min(1,			// 0115: "= this_year.total" replaced "+= this_month_total_gift"
      //math.get_random_snd(mean, float(0.1*sqrt(mean*(1-mean))))
      //));

      //--------------//
      mean = allocation_policy.endowment;
      this_year.endowment = this_year.total* max(0.0f,
      min(1.0f,math.get_random_snd(mean, float(0.1*sqrt(mean*(1-mean))))  //min & max bug chea
      ));

      //--------------//

      this_year.facilities = this_year.total	- this_year.endowment - this_year.cur_operations;
    */

    //## chea 180899 same alg. but diff method to avoid bug err_when(last_year.asset_array[AC_ENDOWMENT] <=0) 8.1.1;
    //   still not work because finance.projected_revenue_array[AC_GIFTS].this_year.total is not = gift_total
    /*
      if (finance.projected_revenue_array[AC_GIFTS].this_year.total>0)
      {
      float gift_total = (float) finance.projected_revenue_array[AC_GIFTS].this_year.total;
      this_year.cur_operations = gift_total * allocation_policy.cur_operations;
      this_year.endowment = gift_total * allocation_policy.endowment;
      this_year.facilities = gift_total * allocation_policy.facilities;
      this_year.total = gift_total;
      }
      else
      {

      //## assign 1 to gift_total to avoid bug

      float gift_tot = 1.0f;
      finance.projected_revenue_array[AC_GIFTS].this_year.total = 1.0f;
      this_year.cur_operations = gift_tot * allocation_policy.cur_operations;
      this_year.endowment = gift_tot * allocation_policy.endowment;
      this_year.facilities = gift_tot * allocation_policy.facilities;
      this_year.total = gift_tot;

      //		finance.projected_revenue_array[AC_GIFTS].this_year.total = 0.0f;
      //		this_year.cur_operations = 0.0f;
      //		allocation_policy.cur_operations = 0.0f;
      //		this_year.endowment = 0.0f;
      //		allocation_policy.endownment = 0,0f;
      //		this_year.facilities = 0.0f;
      //		allocation_policy.facilities = 0.0f;
      //		this_year.total = 0.0f;

      }
    */
    //------- calculate this year gifts ---------//

    if(info.game_month == 9 && info.game_day ==1)
	finance.revenue_array[AC_GIFTS].total = last_year.cur_operations;

    //## use last_year.total to calc. this _year total  //## chea 240899

    int month_left=0;

    if (info.game_month >=9 && info.game_month <=12)
	month_left = info.game_month - 9;
    else
	month_left = info.game_month + 3;

    // happen last year
    if( chance_event.alumni_die_financial_year == info.financial_year()-1 )
	this_year.total = (last_year.total - chance_event.alumni_die_gift) * (1+gift_growth_rate);
    else
	this_year.total = last_year.total * (1+gift_growth_rate);

    this_year.total += chance_event.gift_modifier;

    gift_total_next_year = this_year.total * (1+gift_growth_rate);

    //		this_year_total_up2date = this_year.total /12 * month_left;
    // chea 201299 change by b.massy in H.K.
    //	this_year_total_up2date = this_year.total;

    //----------- chance card effects -----------//

    //## chea begin 160699
    if ( chance_event.scandal_in_alt_drop_gift ) {
	this_year.total -= chance_event.scandal_in_alt_drop_gift;
	if(this_year.total<=0)
	    this_year.total = 0;
    }
    //## chea end 160699
    //## chea 180899 same alg. but diff method to avoid bug err_when(last_year.asset_array[AC_ENDOWMENT] <=0);

    if ( chance_event.alumni_die_gift &&
	 chance_event.alumni_die_financial_year == info.financial_year() ) {
	this_year.total+= chance_event.alumni_die_gift;

	if(this_year.total<=0)
	    this_year.total = 0;
    }

    //--------------------------------------------------------------//

    this_year.cur_operations = this_year.total * allocation_policy.cur_operations;
    this_year.endowment = this_year.total * allocation_policy.endowment;
    this_year.facilities = this_year.total * allocation_policy.facilities;

    //## chea 191099 begin
    finance.projected_revenue_array[AC_GIFTS].this_year.total = this_year.cur_operations*(1+finance.inflation_rate/1200+gift_growth_rate) ;
    finance.projected_revenue_array[AC_GIFTS].next_year.total = gift_total_next_year * allocation_policy.cur_operations;

    float last = (float)finance.revenue_array[AC_GIFTS].total;
    float thisy = (float)finance.projected_revenue_array[AC_GIFTS].this_year.total;
    float next = (float)finance.projected_revenue_array[AC_GIFTS].next_year.total;
    float bud = (float)finance.budget_revenue_array[AC_GIFTS].total;

    finance.projected_revenue_array[AC_GIFTS].change_last_year.total = finance.calc_change(thisy , last);

    finance.projected_revenue_array[AC_GIFTS].change_budget_year.total = finance.calc_change(bud , last);

    finance.projected_revenue_array[AC_GIFTS].change_next_year.total = finance.calc_change(next , thisy);

    float dev_last = last_year.total;
    float dev_this = this_year.total;
    float dev_next = gift_total_next_year;

    gift_change_last_year = (float)finance.calc_change(dev_this , dev_last);
    gift_change_next_year = (float)finance.calc_change(dev_next , dev_this);

    //## chea 191099 end

    //	this_year.facilities = this_year.total	- this_year.endowment - this_year.cur_operations;
}

//---------- End of function Development::calc_gift_all -----------//

//---------- Begin of function Development::calc_percent_alumni_gave_gift -----------//
void Development::calc_percent_alumni_gave_gift() {
    const char M_VAR_COUNT = 4;
    int i;
    float input[M_VAR_COUNT];
    memset(input, 0, sizeof(input));

    //--------------//
    // input[0] = float( finance.expense_array[AC_INST_ADVANCEMENT].total / float(finance.total_operating_expense.total));
    //## chwg290699

    if(info.game_year==1)
	if(info.game_month==9)
	    initializationValueAC_INST_ADVANCEMENT=(float)finance.expense_array[AC_INST_ADVANCEMENT].total;
    input[0] = float(finance.expense_array[AC_INST_ADVANCEMENT].total)/
	(initializationValueAC_INST_ADVANCEMENT*(1+math.get_random_snd(0.0f, PSCH_SD(0.05f))));

    //--------------//
    input[1] = float( school_res.player_peer_school->prestige );

    //--------------//
    input[2] = alumni_morale;

    //--------------//
    input[3] = athletics_office.avg_percent_win;

    //--------------//
    //	input[0] = math.dual_response_func(0,39.94f,100,2.87f,4.91f,3.79f,7.60f, input[0]);
    // chwg290699
    input[0] = math.dual_response_func(0,60.8f,100,1.60f,1.50f,0.65f,1.40f, input[0]);
    input[3] = math.dual_response_func(0,40,100,19.97f,30.00f,20.00f,75.00f, input[3]);

    float weight[M_VAR_COUNT] = {0.25f, 0.40f, 0.20f, 0.15f, };

    float newValue = 0;
    for(i=0; i<M_VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    multiplier_alumni = (char) player_school.latency_func( 0.5f, multiplier_alumni, newValue);

    //The actual percentage should equal the multiplier times the following:
    //Public institutions: 0.40
    //Private institutions: 0.70
    float ratio = ( player_school.control == PUBLIC ) ? 0.4f : 0.7f;
    percent_alumni_gave_gift = ratio * multiplier_alumni;
}

//---------- End of function Development::calc_percent_alumni_gave_gift -----------//

//---------- Begin of function Development::calc_alumni_morale -----------//
void Development::calc_alumni_morale() {
    const char M_VAR_COUNT=3;
    int i;
    float input[M_VAR_COUNT];
    memset(input, 0, sizeof(input));

    //--------------//
    input[0] = float( school_res.player_peer_school->prestige );

    //--------------//
    input[1] = float(finance.last_year.surplus_deficit) / float(finance.total_operating_expense.total);
    //--------------//
    input[2] = athletics_office.avg_percent_win;

    //--------------//
    input[1] = math.dual_response_func(0,60,100,0.486f,0.782f,-0.931f,1.000f, input[1]);
    input[2] = math.dual_response_func(0,40,100,19.97f,30.00f,20.00f,75.00f, input[2]);

    float weight[M_VAR_COUNT] = { 0.40f, 0.40f, 0.20f, };

    float newValue = 0;

    for(i=0; i<M_VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    //-------- pass throught the latency function ---------//

    //## chea 091199 change to adpot chance event
    alumni_morale = player_school.latency_func( 0.60f, alumni_morale, newValue);

    //---------- extra effect brought the chance cards ------------//

    if( chance_event.extra_effect_alumni_morale ) {
	alumni_morale *= chance_event.extra_effect_alumni_morale;

	chance_event.extra_effect_alumni_morale += (1.0f - chance_event.extra_effect_alumni_morale) * 0.3f;

	alumni_morale = max(0, min(100,alumni_morale) );

	alumni_morale_history[THIS_YEAR] = alumni_morale;
    }
}

//---------- End of function Development::calc_alumni_morale -----------//

// ##### Begin MArco ##### //
//---------- Begin of function Development::save_initial_data -----------//
void Development::save_initial_data() {
    initial_percent_alumni_gave_gift = percent_alumni_gave_gift;
    initial_alumni_morale = alumni_morale;
    initial_gift_total = last_year.total;

    //	initial_last_year = last_year;
    initial_last_year_endowment = last_year.endowment;
}

//---------- End of function Development::save_initial_data -----------//
// ##### End Marco ###### //
