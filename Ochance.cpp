//Filename    : OCHANCE.CPP
//Description : Random events (refer to chance card specifications (WFM: March 10,1999)
//Owner			: Fred

#include <OMISC.H>
#include <OMATH.H>
#include <OSYS.H>
#include <ODATE.H>
#include <OGCONFIG.H>
#include <OINFO.H>
#include <ONEWS.H>
#include <OTASK.H> 
#include <ODEVELOP.H>
#include <ODEVELOP.H>
#include <OENROLL.H>
#include <OINVEST.H>
#include <ODEVELOP.H>
#include <OFACILIT.H>
#include <OSTUDENT.H>
#include <OATHLETI.H>
#include <ODEPT.H>
#include <OPSCHOOL.H>
#include <OFINANCE.H>
#include <OPEERSCH.H>
#include <OSCHLRES.H>

#include <OCHANCE.H>

//------ Define functions------//

static EventF alumni_die, rape_in_campus, retrofit_facilities, earth_quake, congress_research, stock_market, scandalous_fund_use,
    new_governor, court_limit_minority, rise_performance_rating,
    risky_research,scandal_in_athletics,
    research_prize,technology_transfer,capital_campaign,student_loans,
    plagiarism_scandal,
    for_profit_competition,minority_student_percentage_falls,successful_grad,media_obtain_information,
    rise_educational_quality,rise_student_morale, rise_department_morale, rise_faculty_research_Per,
    //## chea 220999
    rise_faculty_diversity_index, rise_use_of_IT, rise_staff_morale;

static char get_random_event_id();

static void get_best_research_dept(float& bestPrefResearch, int& bestDeptRecno, int& bestFacultyRecno, short* fieldList);

static void set_task(int taskId, int src, int& target, int& year, int& month, int& bonus, int& ratingType);

//------ Define static function pointers array ------//

EventStruct news_event_function_array[NEWS_ID_COUNT_ONLY_CHANCE] = { 
    //1                                                           //## chea 091199 checked
    {  &alumni_die, 25 },
    //2	// = 20 if 100 and 2 if administrative_performance = 0  //## chea 091199 checked
  
    {  &rape_in_campus, 20 },
    //3                                                           //## chea 091199 checked
 
    { &retrofit_facilities, 25 },
    //4                                                           //## chea 091199 checked
  
    { &earth_quake, 50 },
    //5                                                           //## chea 091199 checked
  
    { &congress_research, 18 }, 
    //6                                                           //## chea 091199 checked
  
    { &stock_market, 15 },
    //7	// = 30 if 100 and 1 if administrative_performance = 0  //## chea 091199 checked
  
    { &scandalous_fund_use, 30 },
    //8	// = 0 or impossible if private school                  //## chea 091199 checked
  
    { &new_governor, 12 },
    //9	// = 0 after occurs once                                //## chea 091199 checked
  
    { &court_limit_minority, 20 },
    //10	//  //## chea 150699                                    //## chea 091199 checked
  
    { &risky_research, 20 },
    //11	//  //## chea 150699                                    //## chea 091199 checked
  
    { &scandal_in_athletics, 25 },
    //12                                                           //## chea 091199 checked
  
    { &research_prize,10 },
    //13                                                           //## chea 091199 checked
  
    { technology_transfer,20 },
    //14                                                           //## chea 091199 checked
  
    { &capital_campaign,20 },
    //15                                                           //## chea 091199 checked
  
    { &student_loans,15 },
    //16									//## chwg 17Jun99	              //## chea 091199 checked
  

    { &plagiarism_scandal,15 },
    //17									//## chwg 19Jun99	              //## chea 091199 checked
  
    { &for_profit_competition,20 },
    //18						//## chwg 21Jun99	              //## chea 091199 checked
  
    { &minority_student_percentage_falls,20 },
    //19									//## chwg 21Jun99	              //## chea 091199 checked
  
    { &successful_grad,100 },
    //20									//## chwg 21Jun99	              //## chea this has no effect at all
  
    { &media_obtain_information,20 },
  
};

static EventFP task_event_function_array[TASK_COUNT] = {
    &rise_performance_rating,                       //21
    &rise_educational_quality,                      //## chea 220999  //22
    &rise_student_morale,                           //23
    &rise_department_morale,                        //24
    &rise_faculty_research_Per,                     //25
    &rise_faculty_diversity_index,                  //26
    &rise_use_of_IT,                                //27
    &rise_staff_morale,                             //28
};

//------ Begin of function ChanceEvent::init -----//
//!
void ChanceEvent::init() {
    memset(this, 0, sizeof(ChanceEvent));

    research_adjust_multiplier = 1;
    capital_campaign_end_year  = -1;
    alumni_budget_increase = 1.0f;
}

//------- End of function ChanceEvent::init -----//

//------ Begin of function ChanceEvent::deinit -----//
//!
void ChanceEvent::deinit() {
}

//------- End of function ChanceEvent::deinit -----//

//------ Begin of function ChanceEvent::next_day -----//
//!
void ChanceEvent::next_day() {
    process_chance_event();
    process_objective_event();
}

//------- End of function ChanceEvent::next_day -----//

//------ Begin of function ChanceEvent::process_chance_event -------//
//!
void ChanceEvent::process_chance_event() {
    if (info.prerun_year==1)                        // during initialization
	return;

    if( player_school.scenario_id > SCN_NONE )      // never happen if this is a scenario game
	return;

    update_chance_vars();
    process_individual_chance();                    // process individual chance cards

    if( info.game_day == 1 || info.game_day > 27 )  // avoid Performance indicator calculation at 1st of each month or the end of each month
	return;

    //---- if a chance event has been triggered by hotkey ------//

    int eventId;

    if( chance_event.test_chance &&
	// NEWS_FOOTBALL_GAME & NEWS_BASKETBALL_GAME are triggered elsewhere
	chance_event.test_chance != NEWS_FOOTBALL_GAME &&
	chance_event.test_chance != NEWS_BASKETBALL_GAME ) {
	eventId = chance_event.test_chance-1;         // eventId starts with 0 while test_chance starts with 1.
    }
    else {                                          //--- otherwise, randomly choose one with consideration of the event's odds ---//
	//------------ determine the chance based on the frequency setting ----//

	// if frequency is set to none
	if( player_school.chance_event_frequency == 0 ) {
	    return;
	}
	// occasional
	else if( player_school.chance_event_frequency == 1 ) {
	    // don't have two in a row within a year
	    if( info.game_date < any_task_last_happen_date+365 )
		return;

	    if( m.random(60) > 0 )
		return;
	}
	// frequent
	else if( player_school.chance_event_frequency == 2 ) {
	    // don't have two in a row within 100 days
	    if( info.game_date < any_task_last_happen_date+120 )
		return;

	    if( m.random(20) > 0 )
		return;
	}

	//-------- randomly pick an event --------//

	eventId = m.random(NEWS_ID_COUNT_ONLY_CHANCE);

	// don't happen twice in 10 years
	if( event_last_happen_date[eventId] + 10*360 > info.game_date )
	    return;
    }

    //------------- run the event now -----------//

    int oldNewsSize = news_array.size();

    EventFP eventFP = news_event_function_array[eventId].event_func;

    (*eventFP)();                                   // run the event

    chance_event.test_chance = 0;

    //------- display the news -------//

    int newsSize = news_array.size();

    if( newsSize > oldNewsSize ) {
	sys.set_news_pop_up();

	News* newsPtr = news_array[newsSize];
	news_str=newsPtr->msg();

	news_news_str=news_str;

	// memcpy(news_date_str,date.date_str(newsPtr->news_date),55);
	strncpy( news_date_str, info.date_str_julian(newsPtr->news_date), 55 );
	news_date_str[55-1] = '\0';
	// date_str=date.date_str( newsPtr->news_date, 1, 0 );
	date_str=info.date_str_julian(newsPtr->news_date, INFO_DATE_DEFAULT|INFO_DATE_SHORT_MONTH|INFO_DATE_NO_DAY );

	//------- update last happen dates --------//

	any_event_last_happen_date      = info.game_date;
	event_last_happen_date[eventId] = info.game_date;
    }
}

//------ End of function ChanceEvent::process_chance_event -------//

//------ Begin of function ChanceEvent::process_objective_event -------//
//!
void ChanceEvent::process_objective_event() {
    if( !task_array.can_add_more_task() )
	return;

    //------------ determine the chance based on the frequency setting ----//

    // if frequency is set to none
    if( player_school.chance_objective_frequency == 0 ) {
	return;
    }
    // occasional
    else if( player_school.chance_objective_frequency == 1 ) {
	// don't have two in a row within a year
	if( info.game_date < any_task_last_happen_date+365 )
	    return;

	if( m.random(365) > 0 )
	    return;
    }
    // frequency
    else if( player_school.chance_objective_frequency == 2 ) {
	// don't have two in a row within 100 days
	if( info.game_date < any_task_last_happen_date+120 )
	    return;

	if( m.random(120) > 0 )
	    return;
    }

    //------------ randomly choose an objective -------------//

    int taskId = m.random(TASK_COUNT);

    // don't have same objective in 20 years
    if( task_last_happen_date[taskId] + 365*20 > info.game_date )
	return;

    //------ run the task function now ---------//

    EventFP eventFP = task_event_function_array[taskId];

    (*eventFP)();                                   // ini the task; ASSUME the task is added

    sys.set_news_pop_up();
    Task* taskPtr = task_array[task_array.size()];
    news_str=taskPtr->get_task_msg();
    news_news_str=news_str;

    // display in 200? format
    int thisDate = date.julian( date.year(taskPtr->news_date),
				date.month(taskPtr->news_date), date.day(taskPtr->news_date) );

    // memcpy(news_date_str,date.date_str(thisDate),55);
    strncpy( news_date_str, info.date_str_julian(thisDate), 55 );
    news_date_str[55-1] = '\0';
    // date_str=date.date_str(taskPtr->news_date,1,0);
    date_str=info.date_str_julian(taskPtr->news_date, INFO_DATE_DEFAULT|INFO_DATE_SHORT_MONTH|INFO_DATE_NO_DAY);

    //------- update last happen dates --------//

    any_task_last_happen_date   = info.game_date;
    task_last_happen_date[taskId] = info.game_date;

    return;
}

//------ End of function ChanceEvent::process_objective_event -------//

//------ Begin of function ChanceEvent::update_chance_vars -------//
//!
void ChanceEvent::update_chance_vars() {
    //------ update minority student percentage falls counter ------//

    if ( info.game_day == 1 && info.game_month == 10 ) {
	int sum=0,sumLastYear=0;

	for(int i=0;i<MAX_STUDENT_LEVEL;i++) {
	    sum += department_array.student_gender_history[i][HISTORY_YEAR_COUNT-1];
	    sumLastYear += department_array.student_gender_history[i][HISTORY_YEAR_COUNT-2];
	}

	int nonminorityCount=0,nonminorityCountLastYear=0;
	nonminorityCount += department_array.student_gender_history[NONMINORITY_MALE][HISTORY_YEAR_COUNT-1];
	nonminorityCount += department_array.student_gender_history[NONMINORITY_FEMALE][HISTORY_YEAR_COUNT-1];
	nonminorityCountLastYear += department_array.student_gender_history[NONMINORITY_MALE][HISTORY_YEAR_COUNT-2];
	nonminorityCountLastYear += department_array.student_gender_history[NONMINORITY_FEMALE][HISTORY_YEAR_COUNT-2];

	float percentLastYear=((float)nonminorityCountLastYear)/sumLastYear;
	float percent=((float)nonminorityCount)/sum;

	if((percent-percentLastYear)<0)
	    minority_student_percentage_falls_consYrs++;
	else
	    minority_student_percentage_falls_consYrs=0;
    }

    //----- reset variables at the beginning of a financial year ----//

    if ( info.game_day == 1 && info.game_month == 9 ) {
	scandal_in_alt_drop_gift =0;                  // the effect of this is lasting so we don't set it to negative to cancel the previous effect.

	football_game_bonus = 0;
	basketball_game_bonus = 0;

	operating_reserve_modifier = 0;

	if( gift_modifier < 0 )
	    gift_modifier = -gift_modifier / 2.0f;      // recover 50% next year
	else
	    gift_modifier = 0;                          // diminish every year

	gift_growth_rate_multiplier = 0;
    }
}

//------ End of function ChanceEvent::update_chance_vars -------//

//------ Begin of function ChanceEvent::process_individual_chance -------//
//!
void ChanceEvent::process_individual_chance() {
    //--------------- chance_event.profit_competition_act --------//

    if(chance_event.profit_competition_act == 1 ) {
	if( chance_event.profit_competition_end_date < info.game_date ) {
	    //unlock
	    FinancePolicy *policy;
	    policy = finance.get_policy_var(1, 0);
	    policy->required_flag = false;
	    policy->applied_flag = P_NONE;
	    policy->import_weight = OPT_MID_WEIGHT;
	    policy->lower_bound = policy->lower_limit;
	    policy->upper_bound = policy->upper_limit;

	    sys.redraw_all_flag=1;
	    chance_event.profit_competition_act = 0;
	    chance_event.profit_competition_end_date = 0;
	}

	if( chance_event.profit_competition_end_date > info.game_date) {
	    //lock back after a new year
	    FinancePolicy *policy;
	    policy = finance.get_policy_var(1, 0);

	    if(policy->required_flag != true) {         // original: policy->required_flag = true, BUG FIXED in 2.0
		policy->required_flag = true;
		policy->applied_flag = P_IMPLEMENT;
	    }
	}
    }
}

//------ End of function ChanceEvent::process_individual_chance -------//

//------ Begin of function alumni_die -------//
//!
void alumni_die() {
    if( m.random(25) && !chance_event.test_chance ) // odds: 25:1
	return;

    // at least after 2 years for another instance
    if( info.financial_year() <= chance_event.alumni_die_financial_year+1 )
	return;

    //---------- 1/3 gen random parameters --------//

    err_when( GENDER_MALE != 0 );

    int gender = m.random(2);

    // $(000)
    int bequest = int( development_office.this_year.total * m.fmin(0.3f, math.get_random_snd(0.75f, 0.2f)));

    //---------- 2/3 add news --------//

    news_array.alumni_die(gender, bequest);

    //---------- 3/3 action --------//

    chance_event.alumni_die_gift += bequest;
    chance_event.alumni_die_financial_year = info.financial_year();

    err_when(bequest<0);
}

//------ End of function alumni_die -------//

//------ Begin of function rape_in_campus -------//
//!
void rape_in_campus() {
    //---	Scale the odds to be 20:1 for administrative performance=100 and 2:1 for administrative performance=0. (The chance card text should say there have been multiple rapes - that this is a serious matter.)

    int oddsChance = (short) (2 + (18/100.0f) * player_school.administrative_performance );

    if( m.random(oddsChance) && !chance_event.test_chance )
	return;

    //---------- add news --------//

    news_array.rape_in_campus();

    //---	Loop through the SL1 student sims and reduce their student life satisfaction index ----//

    int i,j;
    StudentArray* stuArr;
    Student *stuPtr;

    err_when( department_array.size() == 0 );

    for ( i=department_array.size(); i>=0; i-- ) {
	if (i) {
	    if ( department_array.is_deleted(i) )
		continue;

	    stuArr = &(department_array[i]->student_array);
	}
	else {
	    stuArr = &(department_res.general_dept.student_array);
	}

	sys.yield();

	// loop student
	for (j=stuArr->size(); j>0; j--) {
	    if ( stuArr->is_deleted(j) )
		continue;

	    stuPtr = stuArr->operator[](j);

	    //--- Min[80%,RandNormDev(60%,7%)] if Female ---//

	    if( stuPtr->gender_ethnic_group == NONMINORITY_FEMALE ||
		stuPtr->gender_ethnic_group == MINORITY_FEMALE ) {
		stuPtr->satisfaction_student_life = char( (float) stuPtr->satisfaction_student_life * m.fmin( 0.65f, math.get_random_snd(0.50f, 0.05f)));
	    }
	    else {                                      //--- Min[95%,RandNormDev(90%,2.5%)] if Male ---//
		stuPtr->satisfaction_student_life = char( (float) stuPtr->satisfaction_student_life * m.fmin( 0.75f, math.get_random_snd(0.60f, 0.04f)));
	    }
	}
    }

    //--- Multiply current media attitudes and alumni morale by Min[95%,RandNormDev(90%,2.5%)] ---//

    chance_event.extra_effect_media_att    = m.fmin(0.95f, math.get_random_snd(0.7f, 0.025f));
    chance_event.extra_effect_alumni_morale = m.fmin(0.95f, math.get_random_snd(0.7f, 0.025f));

    player_school.calc_media_attitudes();
    development_office.calc_alumni_morale();

    //---	Multiply the current "Monthly campus crime index" by Min[110%,RandNormDev(130%,7%)] ---//

    facility_office.crime_index[THIS_MONTH] = (char) int(facility_office.crime_index[THIS_MONTH] * m.fmax(1.10f, math.get_random_snd(1.3f, 0.07f)) );
}

//------ End of function rape_in_campus -------//

//------ Begin of function retrofit_facilities -------//
//!
void retrofit_facilities() {
    //-- Odds: 25:1 [Please replace "elderly" by "physically challenged" in the Chance Card text.] ---//

    if( m.random(25) && !chance_event.test_chance  )// odds: 50:1
	return;

    //---------- 1/3 gen random parameters --------//

    int dollarLost = int( finance.this_year.asset_array[AC_BUILDINGS]
			  * m.fmin(0.1f, math.get_random_snd(0.04f, 0.01f)) );

    if( dollarLost <= 0 )
	return;

    chance_event.operating_reserve_modifier = (float) -dollarLost;
    finance.this_year.asset_array[AC_OPERATING_RESERVE] -= (float) dollarLost;
    finance.calc_asset_total();

    //---------- 2/3 add news --------//

    news_array.retrofit_facilities(dollarLost);
}

//------ End of function retrofit_facilities -------//

//------ Begin of function earth_quake -------//
//!
void earth_quake() {
    if( m.random(50) && !chance_event.test_chance  )// odds: 50:1
	return;

    news_array.earth_quake();

    //---- multiply the current square footage for each department by a uniformly distributed random variable between 60% and 100% ---//

    DepartmentInfo *deptInfo;

    int deptCount = department_array.department_count;

    for( int i=0 ; i<=deptCount; i++ ) {
	if ( i )
	    deptInfo  = (department_res.info_array+department_array[i]->department_id - 1);
	else
	    deptInfo = &department_res.general_dept_info;

	deptInfo->actual_sf = int(deptInfo->actual_sf * ( 60 + m.random(41) ) / 100.0f );
    }
}

//------ End of function earth_quake -------//

//------ Begin of function congress_research -------//
//!
void congress_research() {
    if( m.random(18) && !chance_event.test_chance ) // odds: 18:1
	return;

    //---------- 1/3 gen random parameters --------//

    char dir = ( math.get_random_float() > 0.4 ) ? -1 : 1;

    //---------- 2/3 add news --------//

    news_array.congress_research(dir);

    //---------- 3/3 action --------//

    chance_event.research_adjust_multiplier = 1 + dir * m.fmin(0.1f, math.get_random_snd(0.5f, 0.1f));
}

//------ End of function congress_research -------//

//------ Begin of function stock_market -------//
//!
void stock_market() {
    if( m.random(15) && !chance_event.test_chance ) // odds: 15:1
	return;

    //---------- 1/3 gen random parameters --------//

    char dir = ( math.get_random_float() > 0.5 ) ? -1 : 1;

    chance_event.stock_dir = (int) dir;

    //---------- 2/3 add news --------//

    news_array.stock_market(dir);

    //---------- 3/3 action --------//

    // do the action in oinve_st.cpp
    //	investment_office.s_n_p_history[THIS_MONTH] *= 1 + dir * max(0.04f, math.get_random_snd(0.16f, 0.04f));

}

//------ End of function stock_market -------//

//------ Begin of function scandalous_fund_use -------//
//!
void scandalous_fund_use() {
    int oddsChance = (short) (1 + (29/100.0f) * player_school.administrative_performance );

    if( m.random(oddsChance) && !chance_event.test_chance )
	return;

    //-- reduce the operating reserve by: ---//

    float t = math.get_random_snd(2.0f, 0.5f);

    int dollarLost = int( 1000.0f * max(0.4f, t) );

    chance_event.operating_reserve_modifier = (float) -dollarLost;
    finance.this_year.asset_array[AC_OPERATING_RESERVE] -= (float) dollarLost;
    finance.calc_asset_total();

    //------ multiply the current level of gifts by -----//

    chance_event.gift_modifier = development_office.this_year.total * -(1.0f - math.get_random_snd(0.667f,0.05f));

    development_office.this_year.total += chance_event.gift_modifier;
    development_office.gift_change_next_year = (float)finance.calc_change(development_office.gift_total_next_year, development_office.this_year.total);

    //------------ show news ---------------//

    news_array.scandalous_fund_use(dollarLost);

    //------  change staff morale, alumni morale and media attitudes  -----//

    chance_event.extra_effect_staff_morale  = m.fmin(0.95f, math.get_random_snd(0.7f, 0.05f));

    chance_event.extra_effect_media_att    = m.fmin(0.97f, math.get_random_snd(0.7f, 0.03f));
    chance_event.extra_effect_alumni_morale = m.fmin(0.97f, math.get_random_snd(0.7f, 0.03f));

    player_school.calc_staff_morale();              // update the values now
    player_school.calc_media_attitudes();
    development_office.calc_alumni_morale();
}

//------ End of function scandalous_fund_use -------//

//------ Begin of function new_governor -------//
//!
void new_governor() {
    if( !player_school.is_public() )
	return;

    if( m.random(12) && !chance_event.test_chance ) // odds: 12:1
	return;

    //---------- 1/3 gen random parameters --------//

    char dir = (float) ( math.get_random_float() > 0.33 ) ? -1 : 1;

    //---------- 2/3 add news --------//

    news_array.new_governor(dir);

    //---------- 3/3 action --------//

    float change = (float) finance.budget_revenue_array[AC_STATE_APPROPRIATION].total *
	(float) dir * m.fmin(0.5f, math.get_random_snd(0.13f, 0.05f));

    finance.budget_revenue_array[AC_STATE_APPROPRIATION].total += change;
    finance.projected_revenue_array[AC_STATE_APPROPRIATION].this_year.total += change;

    finance.budget_calc_total();
    finance.budget_calc_percent();
    finance.budget_calc_change();

    finance.calc_projected_total_n_percent(1);      // 1-recalculate
}

//------ End of function new_governor -------//

//------ Begin of function court_limit_minority -------//
//!
void court_limit_minority() {
    if( m.random(20) && !chance_event.test_chance ) // odds: 20:1
	return;

    //---------- 2/3 add news --------//

    news_array.court_limit_minority();

    //---------- 3/3 action --------//
    // to lock out "high" for these two buttons Aand set them to none.

    enroll_res.minority_offers_special = SET_LOW;
    enroll_res.minority_aid_special = SET_LOW;

    chance_event.court_limit_minority_flag = 1;

    sys.redraw_all_flag=1;
}

//------ End of function court_limit_minority -------//

//------ Begin of function risky_research-------//

void risky_research() {
    //--- get the department with the highest research performance ---//

    short fieldList[] = { FIELD_APPLIED_SCIENCE, FIELD_ENGINEERING, FIELD_MATH_AND_COMPUTER_SCIENCE, FIELD_NATURAL_SCIENCE, 0 };

    float bestPrefResearch;
    int bestDeptRecno, bestFacultyRecno;

    get_best_research_dept( bestPrefResearch, bestDeptRecno, bestFacultyRecno, fieldList );

    //--- Odds: pick the science, engineering, or agriculture department with the highest research performance; scale the odds to be 20:1 for research performance=100 and 90:1 for research performance=0. The text should say which department is chosen (note that we no longer refer specifically to genetic research).

    // fix in version 2, bestDeptRecno may be zero
    if( !bestDeptRecno || m.random( int(20 + 80 * (100-bestPrefResearch) / 100) ) && !chance_event.test_chance )
	return;

    //---	Reduce the operating reserve by Min[$1.0million,RandNormDev($2.0million,$0.25million)]. ---//

    int moneyLost = int(1000* m.fmin(1.00f, math.get_random_snd(2.00f, 0.25f)));

    chance_event.operating_reserve_modifier = (float) -moneyLost;
    finance.this_year.asset_array[AC_OPERATING_RESERVE] -= (float) moneyLost;
    finance.calc_asset_total();

    //----------- add the news ----------//

    news_array.risky_research(moneyLost);

    ///--	Loop through the faculty sims in the chosen department. Multiply the satisfaction index for each sim by a new value of Min[100%,RandNormDev(85%,5%)] ---//

    FacultyArray* facultyArray = &department_array[bestDeptRecno]->faculty_array;

    for( int i=facultyArray->size() ; i>0 ; i-- ) {
	if( facultyArray->is_deleted(i) )
	    continue;

	Faculty* facultyPtr = facultyArray->operator[](i);

	facultyPtr->extra_effect_satisfaction_index = m.fmin( 1.00f, math.get_random_snd(0.85f, 0.05f) );
	facultyPtr->calc_satisfaction_index();
    }

    //------  change staff morale, alumni morale and media attitudes  -----//

    chance_event.extra_effect_media_att    = m.fmin(1.0f, math.get_random_snd(0.75f, 0.05f) );
    chance_event.extra_effect_alumni_morale = m.fmin(1.0f, math.get_random_snd(0.75f, 0.05f) );

    player_school.calc_media_attitudes();
    development_office.calc_alumni_morale();
}

//------ End of function risky_research -------//

//------ Begin of function scandal_in_athletics-------//

void scandal_in_athletics() {
    //-----	Odds are 25:1 if athletics level of competition is 1; 50:1 if 2; and 75:1 if level of competition is 3.---//

    if( !chance_event.test_chance ) {
	int oddsChance[] = { 25, 50, 75 };

	if( m.random(oddsChance[athletics_office.ncaa_level_input-1]) > 0 )
	    return;
    }

    //---------- display news --------//

    news_array.scandal_in_athletics();

    //---	Convert "Special treatment for athletes" in admissions and financial aid to "no special treatment" for the next three admissions cycles. -----//

    enroll_res.athlete_offers_special=SET_LOW;      //set to none
    enroll_res.athlete_aid_special=SET_LOW;         //set to none
    chance_event.athlete_special_ban_year=3;        //set a chance flag for interface
    sys.redraw_all_flag=1;

    //---- Loop through the sims for SL1. For each top athlete (athletic talent > _), force immediate dropout with probability = 0.5. ---//

    int i,j;
    StudentArray* stuArr;
    Student *stuPtr;
    char sl;

    err_when( department_array.size() == 0 );

    for ( i=department_array.size(); i>=0; i-- ) {
	if (i) {
	    if ( department_array.is_deleted(i) )
		continue;

	    stuArr = &(department_array[i]->student_array);
	}
	else {
	    stuArr = &(department_res.general_dept.student_array);
	}

	// loop student
	for (j=stuArr->size(); j>0; j--) {
	    if ( stuArr->is_deleted(j) )
		continue;

	    sys.yield();

	    stuPtr = stuArr->operator[](j);

	    sl = stuPtr->student_level;

	    if (sl==UG_TRADITION) {
		char check = stuPtr->talent_athletics;

		if (check>60) {
		    //----- remove the student from student_array -----//
		    const char level2Degree[] = {BACHELOR_DEGREE, BACHELOR_DEGREE, MASTER_DEGREE, DOCTOR_DEGREE, -1};

		    if ( level2Degree[stuPtr->student_level] >=0 ) {
			stuArr->last_year_dropout[stuPtr->student_level]++;
			stuArr->cur_dropout[level2Degree[stuPtr->student_level]]++;
		    }

		    stuArr->del(stuPtr->student_recno);
		}
	    }
	}
    }

    //--- Multiply the current level of gifts by Min[95%,RandNormDev(80%,5%)]. The change takes effect in the next game month and carries over as the prior value for the following month in the usual way. (This procedure applies wherever the current value of a variable is adjusted.) //

    int aidDrop = int( development_office.this_year.total *
		       (1 - m.fmin(0.85f, math.get_random_snd(0.70f, 0.05f)) ) );

    chance_event.scandal_in_alt_drop_gift += aidDrop;

    //------  change alumni morale and media attitudes  -----//

    chance_event.extra_effect_media_att    = m.fmin(0.90f, math.get_random_snd(0.7f, 0.025f) );
    chance_event.extra_effect_alumni_morale = m.fmin(0.90f, math.get_random_snd(0.7f, 0.025f) );

    player_school.calc_media_attitudes();
    development_office.calc_alumni_morale();
}

//------ End of function scandal_in_athletics -------//

//------ Begin of function plagiarism_scandal -------//

void plagiarism_scandal() {
    //------- odds: 20: 1 --------//

    if( m.random(20) && !chance_event.test_chance )
	return;

    //-- Randomly select and dismiss "n" student sims (regardless of student level), effective immediately, where n = Min[20,RandNormDev(100,25)] --//
    //--------------------------------//

    StudentArray* stuArr;
    Student *stuPtr;
    chance_event.plagiarism_scandal_n=0;

    int n= (int)m.fmax(20.0f,math.get_random_snd(100.0f, 25.0f));

    for(int i=0,timeout=0;(i<n)&&(timeout<n*10);i++,timeout++) {
	int j=m.random(department_array.size())+1;

	if ( department_array.is_deleted(j) ) {
	    i--;
	    continue;
	}

	if(department_array.size()!=0)
	    stuArr = &(department_array[j]->student_array);
	else
	    stuArr = &(department_res.general_dept.student_array);

	int k=m.random(stuArr->size())+1;

	if (stuArr->is_deleted(k)) {
	    i--;
	    continue;
	}
	stuArr->del(k);
	chance_event.plagiarism_scandal_n++;
    }

    //---------- add news --------//

    news_array.plagiarism_scandal(chance_event.plagiarism_scandal_n);

    //---	Multiply the "Academic satisfaction index" for all remaining student sims by Min[95%,RandNormDev(80%,5%)].---//

    err_when( department_array.size() == 0 );

    for (int i=department_array.size(); i>=0; i-- ) {
	if (i) {
	    if ( department_array.is_deleted(i) )
		continue;

	    stuArr = &(department_array[i]->student_array);
	}
	else
	    stuArr = &(department_res.general_dept.student_array);

	// loop student
	for (int j=stuArr->size(); j>0; j--) {
	    if ( stuArr->is_deleted(j) )
		continue;

	    sys.yield();

	    stuPtr = stuArr->operator[](j);

	    stuPtr->extra_effect_satisfaction_academic = m.fmin(0.95f,math.get_random_snd(0.8f, 0.05f));
	    stuPtr->calc_satisfaction_academic();
	}
    }

    //---- Multiply current media attitudes and alumni morale by Min[97%,RandNormDev(90%,3%)] ---//

    chance_event.extra_effect_media_att    = m.fmin(0.97f, math.get_random_snd(0.7f, 0.03f) );
    chance_event.extra_effect_alumni_morale = m.fmin(0.97f, math.get_random_snd(0.7f, 0.03f) );

    player_school.calc_media_attitudes();
    development_office.calc_alumni_morale();

    //-------- Update History ----------//

    Department* deptPtr;

    for (int j=0;j<FACULTY_RANK_LEVEL_COUNT_N_TOTAL; j++)
	department_array.faculty_level_history[j][THIS_YEAR] =0;

    for (int j=0;j<MAX_STUDENT_LEVEL_N_TOTAL; j++)
	department_array.student_level_history[j][THIS_YEAR] =0;

    for (int j=0;j<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; j++) {
	department_array.faculty_gender_history[j][THIS_YEAR] =0;
	department_array.student_gender_history[j][THIS_YEAR] =0;
    }

    for ( int i=department_array.size(); i>0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;

	deptPtr = department_array[i];

	deptPtr->calc_student_history();

	for (int j=0;j<FACULTY_RANK_LEVEL_COUNT_N_TOTAL; j++)
	    department_array.faculty_level_history[j][THIS_YEAR] += deptPtr->faculty_level_history[j][THIS_YEAR];

	for (int j=0;j<MAX_STUDENT_LEVEL_N_TOTAL; j++)
	    department_array.student_level_history[j][THIS_YEAR] += deptPtr->student_level_history[j][THIS_YEAR];

	for (int j=0;j<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; j++) {
	    department_array.faculty_gender_history[j][THIS_YEAR] += deptPtr->faculty_gender_history[j][THIS_YEAR];
	    department_array.student_gender_history[j][THIS_YEAR] += deptPtr->student_gender_history[j][THIS_YEAR];
	}
	sys.yield();
    }

    //------ multiply the current level of gifts by -----//

    chance_event.gift_modifier = development_office.this_year.total * -(1.0f - math.get_random_snd(0.667f,0.05f));

    development_office.this_year.total += chance_event.gift_modifier;
    development_office.gift_change_next_year = (float)finance.calc_change(development_office.gift_total_next_year, development_office.this_year.total);
}

//------ End of function scandal_in_athletics -------//

//------ Begin of for_profit_competition -------//
void for_profit_competition() {
    //-- Scale the odds to be 15:1 for institutional prestige=100 and 1:1 for institutional prestige =0. ---//

    int oddsChance = (short) (1 + (14/100.0f) * player_school.prestige );

    if( m.random(oddsChance) && !chance_event.test_chance )
	return;

    //--	Choose a student level at random: Pr[SL1]=30%; Pr[SL2]=50%; Pr[SL3]=20%. (The choice should show up in the Chance Card text.) ---//

    int a_random_student_level;
    int prob;

    prob=m.random(100);
    if(prob<30)
	a_random_student_level=UG_TRADITION;
    else if(prob<80)
	a_random_student_level=UG_NONTRADITION;
    else
	a_random_student_level=MASTER;

    //--- Multiply the applications preference multiplier variable (e.g., as used in Equation 1 of Td_3.4 for SL1) for the chosen SL by Min[105%,RandNormDev(115%,4%)]. ---//

    //--- Multiply the yield preference variable (e.g., as used in Equation 4 of Td_3.4 for SL1) for the chosen SL by Min[110%,RandNormDev(125%,5%)]. ---//

    enroll_res.app_multiplier[a_random_student_level]*=m.fmin(1.05f,math.get_random_snd(1.15f, 0.04f));
    enroll_res.yield_multiplier[a_random_student_level]*=m.fmin(1.10f,math.get_random_snd(1.25f, 0.05f));

    chance_event.yield_for_profit_competition=enroll_res.yield_multiplier[a_random_student_level];

    enroll_res.update_enroll_history(1/*no shift*/); // chwg0621  added a noshift flag. 

    //---------- add news --------//

    news_array.for_profit_competition();

    //------- after effects -------//

    chance_event.profit_competition_end_date = info.game_date + (365*3);

    FinancePolicy *policy;
    policy = finance.get_policy_var(1, 0);
    policy->required_flag = true;
    policy->applied_flag = P_IMPLEMENT;

    chance_event.profit_competition_act = 1;	//## chea 091199 4 chance only

    sys.redraw_all_flag=1;
}
//------ End of function for_profit_competition -------//


//------ Begin of minority_student_percentage_falls -------//

void minority_student_percentage_falls()
{
    // comment out because we have difficulties in modifying current applications and current yield rate. 
/*
  //-------- odds: 20: 1 ----------//

  if( m.random(20) && !chance_event.test_chance )
  return;

	//---  Loop through the SL1 student sims. For each sim multiply the current student life satisfaction index by: ---//

	int i,j;
	StudentArray* stuArr;
	Student *stuPtr;
	char sl;
	
	err_when( department_array.size() == 0 );
	
	for ( i=department_array.size(); i>=0; i-- )
	{
	if (i)
	{
	if ( department_array.is_deleted(i) )
	continue;

	stuArr = &(department_array[i]->student_array);
	}
	else
	stuArr = &(department_res.general_dept.student_array);

	// loop student
	for (j=stuArr->size(); j>0; j--)
	{
	if ( stuArr->is_deleted(j) )
	continue;

	sys.yield();

	stuPtr = stuArr->operator[](j);

	sl = stuPtr->gender_ethnic_group;

	if(stuPtr->satisfaction_student_life!=-1)
	{
	if((sl==MINORITY_MALE)||(sl==MINORITY_FEMALE))
	stuPtr->satisfaction_student_life=(char)(((float)chance_event.minority_student_percentage_falls_consYrs*min(0.98f, math.get_random_snd(0.04f, 0.005f)))*stuPtr->satisfaction_student_life);
	else if((sl==NONMINORITY_MALE)||(sl==NONMINORITY_FEMALE))
	stuPtr->satisfaction_student_life=(char)(((float)chance_event.minority_student_percentage_falls_consYrs*min(1.00f, math.get_random_snd(0.02f, 0.003f)))*stuPtr->satisfaction_student_life);
	}			
	}	// end for j
	}

	//---- Multiply current media attitudes by ----//

	chance_event.extra_effect_media_att		 = min(0.97f, math.get_random_snd(0.9f, 0.03f)) );
	chance_event.extra_effect_alumni_morale = min(0.97f, math.get_random_snd(0.9f, 0.03f)) );

	player_school.calc_media_attitudes();
	development_office.calc_alumni_morale();

	//---------- add news --------//

	news_array.minority_student_percentage_falls();
*/
}
//------ End of minority_student_percentage_falls -------//


//---------- Begin of successful_grad -------------//

void successful_grad()
{
    // comment out because we have difficulties in modifying current applications and current yield rate. 
/*
  //-- Scale the odds to be 2:1 for institutional prestige=100 and 100:1 for institutional prestige =0. ----//

  int oddsChance = (short) (1 + (99/100.0f) * player_school.prestige );

  if( m.random(oddsChance) && !chance_event.test_chance )
  return;

	//--- Multiply current institutional prestige by Min[105%,RandNormDev(115%,4%)]; truncate the result at 100. ---//

	school_res.player_peer_school->prestige *= min(1.25f, math.get_random_snd(1.15f, 0.04f)) ;  //## chea 091199 try to higher th diff.

	if(school_res.player_peer_school->prestige>=100.0f)
	school_res.player_peer_school->prestige = 100.0f;

	//---	Multiply current media attitudes and alumni morale by Min[105%,RandNormDev(115%,4%)]; truncate the result at 100. ---//

	chance_event.alumni_morale_rise++;   //## chea 091199 to ccount how many time should media_attitudes deduce

	chance_event.media_att_count_up++;   //## chea 091199 to ccount how many time should media_attitudes increase

	//----------- add news --------------//

	news_array.successful_grad();
*/
}
//------ End of successful_grad -------//


//------ Begin of media_obtain_infomation -------//
void media_obtain_information()
{
    // comment out because we have difficulty in implementing many areas of it. 
/*
  float deptResFr,instrExp,deptResExp,totResFr;

  deptResFr=(float)department_array.discretionary_hour_array[DT_RESEARCH][HISTORY_TRIMESTER_COUNT-1]
  /(department_array.discretionary_hour_array[DT_INSTITUTIONAL_AND_PUBLIC_SERVICE][HISTORY_TRIMESTER_COUNT-1]
  -department_array.discretionary_hour_array[DISCRETIONARY_TYPE_COUNT_N_TOTAL][HISTORY_TRIMESTER_COUNT-1]);

  instrExp=(float)((1.0f-deptResFr)*finance.expense_array[AC_ACADEMIC_DEPARTMENTS].total);

  deptResExp=(float)(deptResFr*finance.expense_array[AC_ACADEMIC_DEPARTMENTS].total);

  totResFr = (float)((deptResExp+finance.expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total)
  /(finance.expense_array[AC_ACADEMIC_DEPARTMENTS].total
  +finance.expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total));

  chance_event.true_cost_education = instrExp + (1.0f-totResFr)*((float)finance.total_expense.total-(float)finance.expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total-(float)finance.expense_array[AC_ACADEMIC_DEPARTMENTS].total);

  chance_event.media_att_count_down++;   //## chea 091199 to ccount how many time should media_attitudes deduce

  news_array.media_obtain_information(chance_event.true_cost_education);
*/
}
//------ End of media_obtain_infomation -------//


//------ Begin of research_prize -------//

void research_prize()
{
    //--- get the department with the highest research performance ---//

    short fieldList[] = { FIELD_APPLIED_SCIENCE, FIELD_ENGINEERING, FIELD_MATH_AND_COMPUTER_SCIENCE, FIELD_NATURAL_SCIENCE, 0 };

    float bestPrefResearch;
    int	bestDeptRecno, bestFacultyRecno;

    get_best_research_dept( bestPrefResearch, bestDeptRecno, bestFacultyRecno, fieldList );

    //-- Odds: pick the science, engineering, or agriculture department with the highest research performance; scale the odds to be 20:1 for research performance=100 and 90:1 for research performance=0. The text should say which department is chosen (note that we no longer refer specifically to genetic research) --//

	// fix in version 2, bestDeptRecno may be zero
    if( !bestDeptRecno || bestPrefResearch < 50 )
	return;

    if( m.random( int(10 + 40 * (100-bestPrefResearch) / 50) ) && !chance_event.test_chance )
	return;

    chance_event.research_prize_dept_recno = bestDeptRecno;
    chance_event.research_prize_faculty_recno = bestFacultyRecno;

	//--------- add news now ------------//

    news_array.research_prize(bestDeptRecno, bestFacultyRecno);	

    //--- Multiply the named professor's research performance index by Min[120%,RandNormDev(140%,10%)], truncating the result at 100%. ---//

    Faculty* facultyPtr = department_array[bestDeptRecno]->faculty_array[bestFacultyRecno];
	
    facultyPtr->extra_effect_performance_research = m.fmin(1.20f, math.get_random_snd(1.40f, 0.10f));
    facultyPtr->calc_performance_research();

    //--- Multiply current gifts, alumni morale, and media satisfaction by Min[110%,RandNormDev(115%,5%)], truncating alumni morale at 100%. ---//

    chance_event.extra_effect_media_att		 = m.fmin(1.1f, math.get_random_snd(1.25f, 0.05f) );
    chance_event.extra_effect_alumni_morale = m.fmin(1.1f, math.get_random_snd(1.25f, 0.05f) );

    player_school.calc_media_attitudes();
    development_office.calc_alumni_morale();

	//-----------------------------------------------------//

    chance_event.research_prize_flag=1;
	
    chance_event.research_prize_happen_date=info.game_day;
    chance_event.research_prize_happen_month=info.game_month;
    chance_event.research_prize_happen_year=info.game_year;

	//--- Multiply current institutional prestige by Min[105%,RandNormDev(115%,4%)]; truncate the result at 100. ---//

    school_res.player_peer_school->prestige *= 
	m.fmax(1.0f, (1 + m.fmin(0.1f, math.get_random_snd(0.3f,0.05f)) ) );

    if(school_res.player_peer_school->prestige>=100.0f)
	school_res.player_peer_school->prestige = 100.0f;
}
//------ End of function research_prize ------//


// ------ Begin of technology_transfer------------//
void technology_transfer()
{
    //--- get the department with the highest research performance ---//

    short fieldList[] = { FIELD_APPLIED_SCIENCE, FIELD_ENGINEERING, FIELD_MATH_AND_COMPUTER_SCIENCE, FIELD_NATURAL_SCIENCE, 0 };

    float bestPrefResearch;
    int	bestDeptRecno, bestFacultyRecno;

    get_best_research_dept( bestPrefResearch, bestDeptRecno, bestFacultyRecno, fieldList );

    //--- Odds: pick the science, engineering, computer science, or agriculture department with the highest research performance; scale the odds to be 20:1 for research performance=100 and 50:1 for research performance=50 (zero for performance less than 50). The text should name the department and the amount of the revenue increment. 

	// fix in version 2, bestDeptRecno may be zero
    if( !bestDeptRecno || bestPrefResearch < 50 )
	return;

    if( m.random( int(20 + 30 * (100-bestPrefResearch) / 50) ) && !chance_event.test_chance )
	return;

	//------- set effect vars ------------//

    float randomValue = math.get_random_snd(4.0f, 1.0f);

    chance_event.dept_technology_transfer_research_amount = (1000 * m.fmax(1.5f, randomValue));

   // decided the end year
    chance_event.dept_technology_transfer_start_year=info.game_year;
    chance_event.dept_technology_transfer_start_month=info.game_month;

	//---- add news -----/

    news_array.technology_transfer( bestDeptRecno, chance_event.dept_technology_transfer_research_amount );
}
//------ End of technology_transfer------//


// ----------- Begin of capital_campaign---------------//
//
void capital_campaign()
{
    //-------- odds: 20: 1----------//

    if( m.random(20) && !chance_event.test_chance )
	return;

	//------ display news -----------//

    news_array.capital_campaign();

    //--	Multiply the "Institutional advancement" budget by Min[150%,RandNormDev(200%,10%)], effective immediately. Lock the percent change in the institutional advancement budget at zero for five years, then remove the lock. (Player will have to reduce the budget manually after five years if desired.) ---//

    chance_event.capital_campaign_lock_ins_pect = m.fmax(1.50f, math.get_random_snd(2.00f, 0.10f)); //## chea 101199 this should be max to make a diff.

    finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.total*= chance_event.capital_campaign_lock_ins_pect;
    finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.other = finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.total 
	- finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.staff 
	- finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.faculty;
    finance.projected_expense_array[AC_INST_ADVANCEMENT].change_budget_year.total=(chance_event.capital_campaign_lock_ins_pect-1.0f)*10;

    finance.projected_expense_array[AC_INST_ADVANCEMENT].year_to_date = finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year;

    finance.calc_projected_total_n_percent(1);			// 1-recalc 

    //------ gift growth rate multiplier ----//

    chance_event.gift_growth_rate_multiplier = m.fmin( 1.3f, math.get_random_snd(2.0f, 0.3f) );

    chance_event.capital_campaign_end_year = info.game_year+4;		// last for 5 years, +4 is the last year 
}
//------ End of capital_campaign------//


// ------ Begin of student_loans------------//
void student_loans()
{
    // comment out because we have difficulty in implementing it. 
/*
  return;		// BUGHERE, not implemented 

	//-------- odds: 15: 1 ---------//

	if( m.random(15) )
	return;

   //---------- 1/3 gen random parameters --------//
	
   int isIncrease = (m.random(100) > 60) ? 1 : 0;
  
   //---------- 2/3 add news --------//

   news_array.student_loans(isIncrease);
*/
}
//------ End of student_loans------//


//------------------------ TASKS --------------------------------//


//------ Begin of function set_task -------//
//!
void set_task(int taskId, int src, int& target, int& year, int& month, int& bonus, int& ratingType)
{
    target	= src + goal_config.goal_task(taskId)->get_value();
    target	= min(100,target);

    month		= info.game_month;
    year		= info.game_year + goal_config.goal_task(taskId)->get_years();

    bonus		= goal_config.goal_task(taskId)->bonus;
    ratingType = taskId;        
}
//------ End of function set_task -------//


//------ Begin of function rise_performance_rating -------//
//!
void rise_performance_rating()
{
    //---------- 1/3 gen random parameters --------//
    int deptRecno = 0, ratingType;
    int target, src, month, year, bonus;

    int i;

    for (i = department_array.department_count * 2; i > 0; i--)
    {
	int rec = 1+m.random(department_array.department_count);

	if (department_array.is_deleted( rec ))
	    continue;

	if ( department_array[rec]->p_academic_standing < 60 )
	{
	    deptRecno = rec;
	    break;
	}
    }

    if ( deptRecno == 0 )
	deptRecno = 3;

	//-------- set objective settings --------//

    src = int(department_array[deptRecno]->p_academic_standing);

    set_task(TASK_RISE_PERFORMANCE_RATING, src, target, year, month, bonus, ratingType);

	//---------- 2/3 add news --------//

    task_array.rise_performance_rating(deptRecno, ratingType, target, src, month, year, bonus);

    //---------- 3/3 action --------//
    // no immediate action for task	
}
//------ End of function rise_performance_rating -------//


//------ Begin of function rise_educational_quality -------//
//!
void rise_educational_quality()
{

    int deptRecno = 0, rRecno, ratingType;
    int target, src, month, year, bonus;

    //---------- 1/3 gen random parameters --------//

    rRecno = m.random(department_array.department_count);  //m.random(3) = 0,1,2

    deptRecno= department_array.p_educational_quality_sort[rRecno];
    Department* cur_dept_ptr = department_array[deptRecno];

    //-------- set objective settings --------//

    src		= int(cur_dept_ptr->p_educational_quality);    //value now

    set_task(RISE_EDUCATIONAL_QUALITY, src, target, year, month, bonus, ratingType);

    //---------- 2/3 add news --------//

    task_array.rise_educational_quality(deptRecno, ratingType, target, src, month, year, bonus);  

    //---------- 3/3 action --------//
    // no immediate action for task	
}
//------ End of function rise_educational_quality -------//


//------ Begin of function rise_student_morale -------//
//!
void rise_student_morale()
{

    int deptRecno, ratingType;
    int target, src, month, year, bonus;

    //---------- 1/3 gen random parameters --------//

    src = (int)player_school.sub_score[S_STUDENT_MORALE][HISTORY_MONTH_COUNT-1];

    deptRecno= department_array.p_educational_quality_sort[0];  //## chea 230999 this is not going to use

    //-------- set objective settings --------//

    set_task(RISE_STUDENT_MORALE, src, target, year, month, bonus, ratingType);

    //---------- 2/3 add news --------//

    task_array.rise_student_morale(deptRecno, ratingType, target, src, month, year, bonus);  

    //---------- 3/3 action --------//
    // no immediate action for task	
}
//------ End of function rise_student_morale -------//

//------ Begin of function rise_department_morale -------//
//!this should be rise_factulty_morale
void rise_department_morale()
{

    int deptRecno = 0, rRecno, ratingType;
    int target, src, month, year, bonus;

    //---------- 1/3 gen random parameters --------//

    rRecno = m.random(department_array.department_count);  //m.random(3) = 0,1,2

    deptRecno= department_array.p_educational_quality_sort[rRecno];   // no use here

    //-------- set objective settings --------//

    src = int(department_array.p_faculty_morale_history[47]);    //value now

    set_task(RISE_DEPARTMENT_MORALE, src, target, year, month, bonus, ratingType);

    //---------- 2/3 add news --------//

    task_array.rise_department_morale(deptRecno, ratingType, target, src, month, year, bonus);  

    //---------- 3/3 action --------//
    // no immediate action for task	
}
//------ End of function rise_department_morale -------//

//------ Begin of function rise_faculty_research_Per -------//
//!
void rise_faculty_research_Per()
{

    int deptRecno = 0, rRecno, ratingType;
    int target, src, month, year, bonus;

    //---------- 1/3 gen random parameters --------//

    rRecno = m.random(department_array.department_count);  //m.random(3) = 0,1,2

    deptRecno= department_array.p_educational_quality_sort[rRecno];

    //-------- set objective settings --------//

    src = int(department_array.p_faculty_research_history[THIS_MONTH]);    //value now

    set_task(RISE_FACULTY_RESEARCH, src, target, year, month, bonus, ratingType);

    //---------- 2/3 add news --------//

    task_array.rise_faculty_research_Per(deptRecno, ratingType, target, src, month, year, bonus);  //## chea 220999

    //---------- 3/3 action --------//
    // no immediate action for task	
}
//------ End of function rise_faculty_research_Per -------//


//------ Begin of function rise_faculty_diversity_index -------//
//!
void rise_faculty_diversity_index()
{

    int deptRecno = 0, rRecno, ratingType;
    int target, src, month, year, bonus;

    //---------- 1/3 gen random parameters --------//

//	rRecno = m.random(department_array.department_count);  //m.random(3) = 0,1,2
    rRecno = department_array.department_count-1;  //the lowest department 

    deptRecno= department_array.p_faculty_diversity_sort[rRecno];

    //-------- set objective settings --------//

    src = int(department_array[deptRecno]->p_faculty_diversity_history[7]);    //value now

    set_task(RISE_FACULTY_DIVERSITY_INDEX, src, target, year, month, bonus, ratingType);

    //---------- 2/3 add news --------//

    task_array.rise_faculty_diversity_index(deptRecno, ratingType, target, src, month, year, bonus);  

    //---------- 3/3 action --------//
    // no immediate action for task	
}
//------ End of function rise_faculty_diversity_index -------//


//------ Begin of function rise_use_of_IT -------//
//!
void rise_use_of_IT()
{

    int deptRecno = 0, rRecno, ratingType;
    int target, src, month, year, bonus;

    //---------- 1/3 gen random parameters --------//

    rRecno = m.random(department_array.department_count);  //m.random(3) = 0,1,2
    deptRecno= department_array.p_faculty_diversity_sort[rRecno];
    Department* cur_dept_ptr = department_array[deptRecno];

    //-------- set objective settings --------//

    src =(int)(cur_dept_ptr->var_educational_quality_history[4][23]);    //value now

    set_task(RISE_USE_OF_IT, src, target, year, month, bonus, ratingType);

    //---------- 2/3 add news --------//

    task_array.rise_use_of_IT(deptRecno, ratingType, target, src, month, year, bonus);  

    //---------- 3/3 action --------//
    // no immediate action for task	
}
//------ End of function rise_use_of_IT -------//


//------ Begin of function rise_staff_morale -------//
//!
void rise_staff_morale()
{

    int deptRecno = 0, rRecno, ratingType;
    int target, src, month, year, bonus;

    //---------- 1/3 gen random parameters --------//

    rRecno = m.random(department_array.department_count);  //m.random(3) = 0,1,2
    deptRecno= department_array.p_faculty_diversity_sort[rRecno];

    //-------- set objective settings --------//

    src =(int)(player_school.sub_score[S_STAFF_MORALE][THIS_MONTH]);    //value now

    set_task(RISE_STAFF_MORALE, src, target, year, month, bonus, ratingType);

    //---------- 2/3 add news --------//

    task_array.rise_staff_morale(deptRecno, ratingType, target, src, month, year, bonus);  

    //---------- 3/3 action --------//
    // no immediate action for task	
}
//------ End of function rise_staff_morale -------//


//------- Begin of get_best_research_dept ------//

void get_best_research_dept(float& bestPrefResearch, int& bestDeptRecno, int& bestFacultyRecno, short* fieldList)
{
    //----- loop through the departments --------//	

    FacultyArray* facultyArr;
	
    bestPrefResearch = 0;
    bestDeptRecno = 0;

    for( int i=department_array.size(); i>0; i-- )
    {
	if ( department_array.is_deleted(i) )
	    continue;

	//--- only scan departments whose fields are in the field list ---//

	int fieldId = department_res[department_array[i]->department_id]->field_id;
	int j;
	for( j=0 ; fieldList[j] ; j++ )
	{
	    if( fieldList[j] == fieldId )
		break;
	}

	if( !fieldList[j] )
	    continue;

	//--- loop through the faculty of each department ---//

	facultyArr = &(department_array[i]->faculty_array);

	for( j=facultyArr->size() ; j>0 ; j-- )
	{
	    if( facultyArr->is_deleted(j) )
		continue;

	    if( facultyArr->operator[](j)->performance_research > bestPrefResearch )
	    {
		bestPrefResearch = facultyArr->operator[](j)->performance_research;
		bestDeptRecno = i;
		bestFacultyRecno = j;
	    }
	}
    }

    // allow return 0, fix in version 2
    // err_when( !bestDeptRecno );
}
//------- End of function get_best_research_dept ------//

// these two event has no news associated, thus not randomly generated
// see OPSCH_PO.CPP PlayerSchool::think_protagonist

// percent increase in alumni budget (100 for 100%)

void ChanceEvent::signal_alumni_budget_increase(float percentInc, int financialYear)
{
    // refer to capital_campaign()

    //--	Multiply the "Institutional advancement" budget by Min[150%,RandNormDev(200%,10%)], effective immediately. Lock the percent change in the institutional advancement budget at zero for five years, then remove the lock. (Player will have to reduce the budget manually after five years if desired.) ---//

    alumni_budget_increase *= (1.0f + percentInc);

//	finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.total*= (1.0f + percentInc);
//	finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.other = finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.total 
//																								- finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.staff 
//																								- finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.faculty;
//	finance.projected_expense_array[AC_INST_ADVANCEMENT].change_budget_year.total=percentInc*10;
//	finance.projected_expense_array[AC_INST_ADVANCEMENT].year_to_date += finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year;

    float totalInc = finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.total * percentInc;
    finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.total += totalInc;
    finance.projected_expense_array[AC_INST_ADVANCEMENT].change_last_year.other = finance.calc_change( finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.other, finance.expense_array[AC_INST_ADVANCEMENT].other );
    finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.other += totalInc;
    finance.projected_expense_array[AC_INST_ADVANCEMENT].change_last_year.total = finance.calc_change( finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.total, finance.expense_array[AC_INST_ADVANCEMENT].total );
    finance.projected_expense_array[AC_INST_ADVANCEMENT].year_to_date.other += totalInc;
    // update finance.projected_expense_array[i].change_last_year

    finance.calc_projected_total_n_percent(1);			// 1-recalc 

    //------ gift growth rate multiplier ----//

	// chance_event.gift_growth_rate_multiplier = m.fmin( 1.3f, math.get_random_snd(2.0f, 0.3f) );

    alumni_budget_financial_year = financialYear;
}

// amount added to next year athetcis budget

void ChanceEvent::signal_athletics_budget_amount(float amount, int financialYear)
{
    athletics_budget_amount += amount;

    float totalInc = amount;
    finance.projected_expense_array[AC_ATHLETICS].this_year.total += totalInc;
    finance.projected_expense_array[AC_ATHLETICS].change_last_year.other = finance.calc_change( finance.projected_expense_array[AC_ATHLETICS].this_year.other, finance.expense_array[AC_ATHLETICS].other );
    finance.projected_expense_array[AC_ATHLETICS].this_year.other += totalInc;
    finance.projected_expense_array[AC_ATHLETICS].change_last_year.total = finance.calc_change( finance.projected_expense_array[AC_ATHLETICS].this_year.total, finance.expense_array[AC_ATHLETICS].total );
    finance.projected_expense_array[AC_ATHLETICS].year_to_date.other += totalInc;

    finance.calc_projected_total_n_percent(1);			// 1-recalc 

    athletics_budget_financial_year = financialYear;
}
