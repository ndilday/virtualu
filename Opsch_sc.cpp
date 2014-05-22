//Filename    : OPSCHOOL.CPP
//Description : PlayerSchool class - scenario; refer to td3.6
//Owner       : Fred

#include <OCONFIG.H>
#include <OAUDIO.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OBOX.H>
#include <OMATH.H>
#include <OINFO.H>
#include <ODATE.H>
#include <OSCHLRES.H>
#include <OGCONFIG.H>
#include <OSTUDENT.H>
#include <ODEPT.H>
#include <OFACULTY.H>
#include <OENROLL.H>
#include <OFINANCE.H>
#include <OATHLETI.H>
#include <OPSCHOOL.H>
#include <ONEWS.H>

//---------- Begin of function PlayerSchool::init_setting_scenario ---------//
//!
void PlayerSchool::init_setting_scenario(int scenId) {
    //scenId = SCN_STUDENT_DIVERSITY;	//## BUGHER

    err_when( scenId <= SCN_NONE );

    scenario_id = scenId;

    //----------- set the base settings for all types first -------//

    student_market = REGIONAL;
    prestige    = INPUT_MEDIUM;
    student_count = INPUT_MEDIUM;
    sl5_student_percent = INPUT_MEDIUM;

    parttime_undergrad_percent = INPUT_MEDIUM;
    ug_get_degree_in_5year_percent = INPUT_MEDIUM;

    fulltime_ug_in_housing_percent = INPUT_MEDIUM;
    athletic_program_intensity     = INPUT_MEDIUM;

    graduate_student_percent  = INPUT_MEDIUM;
    nondegree_student_percent = 30;

    doctoral_program_intensity    = INPUT_MEDIUM;
    sponsored_research_intensity  = INPUT_MEDIUM;   // ## useless parameter

    relative_wealth     = WEALTH_IN_BETWEEN;        //WEALTH_STRUGGLING,WEALTH_RICH

    control         = PRIVATE;
    campus_environment  = SUBURBAN;

    chance_event_frequency = INPUT_LOW;
    chance_objective_frequency = INPUT_LOW;
    chance_perturbations = INPUT_MEDIUM;
    behavioral_response_speed = INPUT_LOW;          // INPUT_LOW=Normal,INPUT_MEDIUM=faster,INPUT_HIGH=light-speed
    threshold_event_flag = 1;

    ug_get_degree_in_5year_percent = INPUT_MEDIUM;
    location =  PACIFIC;

    //-------- custom set each school type ----------//

    switch (school_type) {
    case COMPREHENSIVE_SCHOOL:
	graduate_student_percent  = INPUT_LOW;
	control = PUBLIC;
	campus_environment = URBAN;
	sponsored_research_intensity = INPUT_LOW;
	parttime_undergrad_percent = INPUT_HIGH;
	doctoral_program_intensity  = INPUT_LOW;
	break;

    case PUBLIC_SCHOOL:
	control = PUBLIC;
	break;

    case LIBERAL_ARTS_SCHOOL:
	fulltime_ug_in_housing_percent = INPUT_HIGH;
	graduate_student_percent  = INPUT_LOW;
	sponsored_research_intensity = INPUT_LOW;
	campus_environment = RURAL;
	doctoral_program_intensity  = INPUT_LOW;
	break;

    case PRIVATE_SCHOOL:
	sponsored_research_intensity = INPUT_HIGH;

	if ( scenario_id == SCN_BALANCE_BUDGET )
	    sponsored_research_intensity = INPUT_MEDIUM;
	break;

    default:
	err_here();
	break;
    }

    //------- translate option to values for the settings -------//

    init_setting_non_scenario();
    calc_setting_vars();

  //-------------------------------------------------------------//
  // select active departments

    static const int deptList1[INSTITUTIONAL_TYPE_COUNT][DEPARTMENT_TYPE_COUNT] = {
	{ 16,17,18,20,7,22,6,23,11,12,3,15,26, },
	{ 16,21,18,11,12,13,22,7,26,3 },
	{ 16,17,18,20,7,22,6,23,11,12,3,15,26,5 },
	{ 1,8,3,5,16,18,22,11,6,7,12,14},
    };
    static const int deptList2[INSTITUTIONAL_TYPE_COUNT][DEPARTMENT_TYPE_COUNT] = {
	{ 0,0,0,19,0,25,4,24,0,13,5,14,28, },
	{ 17,24,20,0,9,0,25,0,28,5 },
	{ 0,0,0,19,0,25,1,24,0,13,0,14,28,0 },
	{ 0,0,0,0,17,20,25,0,0,4,15,19},
    };

    static const int deptList3[INSTITUTIONAL_TYPE_COUNT][DEPARTMENT_TYPE_COUNT] = {
	{ 0,0,0,0,0,0,0,21,0,9,0,0,27, },
	{ 0,0,19,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,21,0,9,0,0,27,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0},
    };

    //------ create a dept list without the constraint of MAX_DEPARTMENT ------//

    int deptCount=0;

    for (int i=0; i<DEPARTMENT_TYPE_COUNT; i++) {
	if ( deptList1[school_type][i] == 0 )
	    break;
	else
	    deptCount++;

	if ( deptList2[school_type][i] == 0 )
	    selected_department_list[i] = deptList1[school_type][i];
	else if ( deptList3[school_type][i] == 0 ) {
	    // 100% between list 1&2
	    selected_department_list[i] = (m.random(2)==0)?deptList1[school_type][i]:deptList2[school_type][i];
	}
	else {
	    int r=m.random(3);

	    // 100% between list 1&2&3
	    if ( r==0 )
		selected_department_list[i] = deptList1[school_type][i];
	    else
		selected_department_list[i] = (r==1)?deptList2[school_type][i]:deptList3[school_type][i];
	}

	err_when(selected_department_list[i] == 0);
    }

    //----- chop out some depts if total # of depts > MAX_DEPARTMENT -----//

    if ( deptCount > MAX_DEPARTMENT )
	for (int i=deptCount-MAX_DEPARTMENT; i>0; i--) {
	    int r=m.random(deptCount);

	    // "swap" with the last
	    selected_department_list[r] = selected_department_list[deptCount-1];
	    selected_department_list[deptCount-1] = 0;
	    deptCount--;
	}

    //------- pre-initialization init for some scenarios ------//

  //------------------- SCN_TEACHING_QUALITY ---------------//

    if ( scenario_id == SCN_TEACHING_QUALITY || scenario_id == SCN_RESEARCH_QUALITY ) {
	init_scen_dept_selected(66);                  // 66% of the department will be selected
    }

    //------------------- SCN_TEACHING_QUALITY ---------------//

    if ( scenario_id == SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS ) {
	init_scen_dept_selected(50);
    }
}

//---------- End of function PlayerSchool::init_setting_scenario ---------//

//---------- Begin of function PlayerSchool::scenario_check_target ----------//
//!
void PlayerSchool::scenario_check_target() {
    if ( scenario_id <= SCN_NONE )
	return;

    //---------------------------------------------------------------------------//
    // when the player has achieved all possible goals in a scenario, display the
    // following message in a popup window
    //---------------------------------------------------------------------------//

    if ( scen_quit_game_flag > 0  &&
	 sys.view_mode != MODE_AWARD_REPORT &&
	 sys.view_mode != MODE_FINAL_SCORE ) {
	int rc = 0;

	vga.use_front();
	rc = box.ask("You have achieved all possible scenario objectives.\n Do you want to continue playing?", "Yes", "No");
	vga.use_back();

	if ( rc == 1 ) {
	    scen_quit_game_flag = 0;                    // continue play
	    sys.redraw_all_flag = 1;
	}
	else
	    sys.set_staying_view_mode(MODE_FINAL_SCORE);// should go to final game score screen

	return;
    }

    //-------- check scenario target monthly ---------///

    if ( info.game_day != 1 )
	return;

    //----------------------------------//

    int i;
    int plaque = -1;                                // = enum { PLAQUE_GOLD=0, PLAQUE_SILVER, PLAQUE_BRONZE, TOTAL_PLAQUE_COUNT=3 };
    int bonus = 0;
    int gameStartYear = date.year(info.game_start_date);
    int yearPassed = info.game_year - gameStartYear;

    if ( info.game_month < date.month(info.game_start_date) )
	yearPassed--;

    err_when( yearPassed < 0 );

    //---- if the game is approaching its time limit, display warning message ------//

    if( yearPassed == scenario_max_game_years - 1 && info.game_month == 9 ) {
	news_array.scenario_near_end();
    }

    //------- check the target of individual scenarios ------//

    switch (  scenario_id ) {

    case SCN_BALANCE_BUDGET:
	if( scen_balance_budget_score() >= scenario_base ) {
	    if ( scenario_target[2] <= finance.this_year.surplus_deficit ) {
		if ( yearPassed <= goal_deadline[PLAQUE_BRONZE] )
		    plaque = PLAQUE_BRONZE;
	    }
	    else if ( scenario_target[1] <= finance.this_year.surplus_deficit ) {
		if ( yearPassed <= goal_deadline[PLAQUE_SILVER] )
		    plaque = PLAQUE_SILVER;
	    }
	    else if ( scenario_target[0] <= finance.this_year.asset_array[AC_OPERATING_RESERVE] ) {
		if ( yearPassed <= goal_deadline[PLAQUE_GOLD] )
		    plaque = PLAQUE_GOLD;
	    }
	}
	break;

    case SCN_RAISE_SALARY: {
	for( i=0 ; i<TOTAL_PLAQUE_COUNT ; i++ ) {
	    if ( yearPassed > goal_deadline[i] )      // exceeded time limit
		continue;

	    //## chea  050899 if ( curSal >= targetSal ) is the org. target checking
	    if ( scenario_faculty_salary_increase >= scenario_target[i] ) {
		plaque = i;
		break;
	    }
	}
    }
    break;

    case SCN_INVEST_ACADEMIC :
	for( i=0 ; i<TOTAL_PLAQUE_COUNT ; i++ ) {
	    if( yearPassed <= goal_deadline[i] &&
		//## chea 260799 another way to do the same alg.
		scenario_target[i] <= scenario_invest_academic_get_cur()-scenario_base ) {
		plaque = i;
		break;
	    }
	}
	break;

    case SCN_TEACHING_QUALITY :
	for( i=0 ; i<TOTAL_PLAQUE_COUNT ; i++ ) {
	    if( yearPassed <= goal_deadline[i] &&
		//## chea 260799 another way to do the same alg.
		scenario_target[i] <= sub_score[S_EDUCATIONAL_QUALITY][THIS_MONTH] ) {
		plaque = i;
		break;
	    }
	}
	break;

    case SCN_RESEARCH_QUALITY :
	for( i=0 ; i<TOTAL_PLAQUE_COUNT ; i++ ) {
	    if( yearPassed <= goal_deadline[i] &&
		//## chea 260799 another way to do the same alg.
		scenario_target[i] <= sub_score[S_SCHOLARSHIP_BROADLY_DEFINED][THIS_MONTH]  ) {
		plaque = i;
		break;
	    }
	}
	break;

    case SCN_WINNING_ATHLETICS:
	if ( athletics_office.football_percent_win*100 >= scenario_target[2] &&
	     athletics_office.basketball_percent_win*100 >= scenario_target[2] &&
	     yearPassed <= goal_deadline[PLAQUE_BRONZE] ) {
	    plaque = PLAQUE_BRONZE;
	}
	else if ( athletics_office.football_percent_win*100 >= scenario_target[1] &&
		  athletics_office.basketball_percent_win*100 >= scenario_target[1] &&
		  yearPassed <= goal_deadline[PLAQUE_SILVER] ) {
	    plaque = PLAQUE_SILVER;
	}
	else if ( athletics_office.football_percent_win*100 >= scenario_target[0] &&
		  athletics_office.basketball_percent_win*100 >= scenario_target[0] &&
		  yearPassed <= goal_deadline[PLAQUE_GOLD]
		  && athletics_office.ncaa_level_input == INPUT_HIGH
		  && INPUT_HIGH > scenario_base ) {
	    plaque = PLAQUE_GOLD;
	}
	break;

    case SCN_LIMIT_TUITION_GROWTH:
	if( 100.0f * (1.0f - finance.tuition_rate  / scenario_float_base) >= scenario_target[0]
	    && player_school.cur_game_score >= scenario_target[1] ) {
	    for ( i=0; i<TOTAL_PLAQUE_COUNT; i++ ) {
		if ( yearPassed <= goal_deadline[i] ) {
		    plaque = i;
		    break;
		}
	    }
	}
	break;

    case SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS:
	for ( i=0; i<TOTAL_PLAQUE_COUNT; i++ ) {
	    if( yearPassed <= goal_deadline[i] &&
		scenario_target[i] <= sub_score[S_EDUCATIONAL_QUALITY][THIS_MONTH] ) {
		plaque = i;
		break;
	    }
	}
	break;

    case SCN_STUDENT_DIVERSITY:
	for( i=0 ; i<TOTAL_PLAQUE_COUNT ; i++ ) {
	    if( yearPassed <= goal_deadline[i] &&
		scenario_target[i] <= enroll_res.overall_percent_minority ) {
		plaque = i;
		break;
	    }
	}
	break;

    case SCN_FACULTY_DIVERSITY: {

	int totalFacCount = department_array.faculty_gender_history[GENDER_ETHNIC_TYPE_COUNT][THIS_YEAR];
	int valueA = ( department_array.faculty_gender_history[MINORITY_MALE][THIS_YEAR] + department_array.faculty_gender_history[MINORITY_FEMALE][THIS_YEAR]) * 100 / totalFacCount;
	int valueB = ( department_array.faculty_gender_history[MINORITY_FEMALE][THIS_YEAR] + department_array.faculty_gender_history[NONMINORITY_FEMALE][THIS_YEAR]) * 100 / totalFacCount;

	if ( valueA >= scenario_target[0] && valueB >= scenario_target[1] ) {
	    for ( i=0; i<TOTAL_PLAQUE_COUNT; i++ )
		if ( yearPassed <= goal_deadline[i] ) {
		    plaque = i;
		    //0309 scen_quit_game_flag = 1;
		    break;
		}
	}

    }
    break;
    }

    //--------- if the player has achieved an objective --------//

    if( plaque >= 0 && !plaque_granted[plaque] ) {  // it is the first time the player gets the plaque
	plaque_granted[plaque] = 1;
	plaque_last = plaque;

	//------- add score bonus now -------//

	int bonusArray[TOTAL_PLAQUE_COUNT] = { 300,200,100 };
	add_bonus(bonusArray[plaque]);

	sys.set_staying_view_mode(MODE_AWARD_REPORT); // go to plaque screen

	// playsound
	audio.play_wav("NEWS", audio.int_to_DsVolume(config.sound_effect_volume));

	//--- if no other objectives can be possibly be achieved, prompt if the player wants to quit ---//

	for(int i=0 ; i<TOTAL_PLAQUE_COUNT ; i++) {
	    // if none of the plaque that have not been achieved are still within the time limit
	    if( !plaque_granted[i] && yearPassed <= goal_deadline[i] )
		break;
	}

	if( i==TOTAL_PLAQUE_COUNT )                   // if none of the plaque that have not been achieved are still within the time limit
	    scen_quit_game_flag = 1;
    }

    //----- End the game if it PASSED "scenario_max_game_years" years. -----//

    if ( yearPassed == scenario_max_game_years ) {
	if ( plaque_granted[0] + plaque_granted[1] + plaque_granted[2] <= 0 ) {
	    // go to lost screen and then final score screen
	    sys.set_staying_view_mode(MODE_LOSE_GAME);
	}
	else {                                        //--- if the player has previously achieved one of the objectives ---//
	    scen_quit_game_flag = 2;                    // signal MODE_FINAL_SCORE to ignore mouse.any_click

	    // display newspaper
	    news_array.scenario_news(1);                // 1: end of scenario

	    // to go to scenario achivement report then to final score screen
	    sys.set_staying_view_mode(MODE_FINAL_SCORE);
	}
    }
}

//----------- End of function PlayerSchool::scenario_check_target -----------//

//---------- Begin of function PlayerSchool::scenario_invest_academic_get_cur ---------//
//!
int PlayerSchool::scenario_invest_academic_get_cur() {
    err_when(SCN_INVEST_ACADEMIC != scenario_id);

    //-------- build an array of departments whose performance is relevant to the scenario's objective ----//

    const int deptList[INSTITUTIONAL_TYPE_COUNT][DEPARTMENT_TYPE_COUNT] = {
	{ 16,17,18,20,19,6,14, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
	{ 9,10,11,12,13,7,26,28,27, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
	{ 21,22,23,24,25,15,4, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
	{ 1,2,3,5,8, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    };

    int scenSpecialDept[MAX_DEPARTMENT];

    memset(scenSpecialDept, 0, sizeof(scenSpecialDept));

    for (int i=0; i<DEPARTMENT_TYPE_COUNT; i++) {
	int deptId = deptList[school_type][i];

	if ( deptId <= 0 )
	    break;

	for ( int d=department_array.size(); d>0; d--) {
	    if ( department_array[d]->department_id == deptId ) {
		scenSpecialDept[i] = d;
		break;
	    }
	}
    }

    if ( scenSpecialDept[0] <= 0 )
	return 0;

    //---- calc weighted average prestige of listed depts over number of students: ----//

    float value = 0, stuCount = 0;
    Department *deptPtr;

    for ( int i=0; scenSpecialDept[i] > 0 && i < DEPARTMENT_TYPE_COUNT ; i++ ) {
	deptPtr = department_array[scenSpecialDept[i]];

	// academic standing = prestige
	value += deptPtr->p_academic_standing * deptPtr->student_array.student_count;
	stuCount += deptPtr->student_array.student_count;
    }

    return (int) max(1.0f, value/stuCount);         // min & max bug chea
}

//---------- Begin of function PlayerSchool::scenario_invest_academic_get_cur ---------//

//----------- End of function init_scen_dept_selected -----------//

void PlayerSchool::init_scen_dept_selected(int selectPercent) {
    //---- Select 2/3 (rounded) of the departments at random. -----//

    int i, toBeSelectedDeptCount = int((float) MAX_DEPARTMENT * (float) selectPercent / 100.0f + 0.5f);

    memset(scen_dept_selected_array, 0, sizeof(scen_dept_selected_array));

    for (i=0; i<toBeSelectedDeptCount ; i++) {
	while(1) {
	    int id = m.random(MAX_DEPARTMENT);

	    if ( scen_dept_selected_array[id] == 0 ) {
		scen_dept_selected_array[id] = 1;
		break;
	    }
	}
    }
}

//----------- End of function init_scen_dept_selected -----------//

//----------- End of function scen_balance_budget_score -----------//

float PlayerSchool::scen_balance_budget_score() {
    return cur_game_score;

    //	return (score_sub_total[1][THIS_MONTH] + score_sub_total[2][THIS_MONTH] ) *
    //			 100 / (score_group_weight[1] + score_group_weight[2]);
}

//----------- End of function scen_balance_budget_score -----------//

//---------- Being of function PlayerSchool::init_scenario ---------//

void PlayerSchool::init_scenario() {
    if( !scenario_id )
	return;

    //--- set scenario setting before update_external_factors_monthly ---//

    int i;

    //----------- SCN_BALANCE_BUDGET ------------------//

    if( scenario_id == SCN_BALANCE_BUDGET ) {
	if( school_type==LIBERAL_ARTS_SCHOOL || school_type==PRIVATE_SCHOOL ) {
	    if( school_type==LIBERAL_ARTS_SCHOOL )
		finance.projected_revenue_array[AC_FINANCIAL_AID].this_year.total *= 1.3f;
	    else
		finance.projected_revenue_array[AC_FINANCIAL_AID].this_year.total *= 1.375f;

	    // re-calculate net tuition
	    finance.projected_revenue_array[AC_NET_TUITION_INCOME].this_year.total =
		finance.projected_revenue_array[AC_GROSS_TUITION_INCOME].this_year.total +
		finance.projected_revenue_array[AC_FINANCIAL_AID].this_year.total;
	}
	else {
	    finance.projected_revenue_array[AC_STATE_APPROPRIATION].this_year.total *= 0.8;
	    finance.projected_revenue_array[AC_STATE_APPROPRIATION].next_year.total *= 0.8;
	    finance.projected_revenue_array[AC_STATE_APPROPRIATION].last_month.total *= 0.8;
	    finance.projected_revenue_array[AC_STATE_APPROPRIATION].year_to_date.total *= 0.8;
	}

	finance.calc_projected_total_n_percent(1);    // 1-re-calculation, not first time calculation

	//------------ set scenario targets ----------------//

	scenario_base = (int) scen_balance_budget_score()+5;

	scenario_target[0] = (int) goal_config.goal_scen(SCN_BALANCE_BUDGET)->goal_value[0];
	// op reseve
	scenario_target[1] = (int) finance.last_year.asset_array[AC_OPERATING_RESERVE];
	scenario_target[2] = (int) 0;
    }

    //----------- SCN_RAISE_SALARY ------------------//

    if ( scenario_id == SCN_RAISE_SALARY ) {
	scenario_base = 0;                            // it is not used, faculty salary increase is stored in player_school.scenario_faculty_salary_increase

	for( i=0 ; i<3 ; i++ )
	    scenario_target[i] = (int) goal_config.goal_scen(SCN_RAISE_SALARY)->goal_value[i];

	scenario_faculty_salary_increase = 0;         // reset it
    }

    //----------- SCN_INVEST_ACADEMIC ------------------//

    if( scenario_id == SCN_INVEST_ACADEMIC ) {
	//---- set base variables --------//

	float spendRate = float( finance.revenue_policy_array[PL_ENDOWMENT_SPENDING_RATE].target_value / 100 );
	int scenEndowment = 20000;                    // 20 million

	scenEndowment = (int) m.fmax((float)(math.get_random_snd(0.1f, 0.01f) * finance.last_year.asset_array[AC_ENDOWMENT]),
				     //min & max bug chea
				     (float)(math.get_random_snd(scenEndowment, 1000.0f)) );

	scenario_base1 = scenEndowment;               // increased endowment amount
	// spending
	scenario_base2 = int(spendRate * scenEndowment);

	//-------- set scenario target ---------//

	scenario_base = scenario_invest_academic_get_cur();

	for( i=0 ; i<3 ; i++ ) {
	    scenario_target[i] = (int) goal_config.goal_scen(SCN_INVEST_ACADEMIC)->goal_value[i];
	}

	//------ modify the financial vars --------//

	//--- increase Endowment asset by $20 million ----//

	scenario_endowment_increase = (float) scenEndowment;

	finance.this_year.asset_array[AC_ENDOWMENT]   += scenEndowment;
	// this one is used to display both "Total assets" and "Total liabilities and fund balance"
	finance.this_year.asset_array[AC_ASSET_TOTAL] += scenEndowment;

	finance.next_year.asset_array[AC_ENDOWMENT]   += scenEndowment;
	// this one is used to display both "Total assets" and "Total liabilities and fund balance"
	finance.next_year.asset_array[AC_ASSET_TOTAL] += scenEndowment;

	finance.this_year.asset_array[AC_QUASI] += scenEndowment;

	//---- promise to incrase endowment spending in the next year's budget ----//

	//		finance.change_budget(ANNOUNCE_NEXT_YEAR, true, AC_ENDOWMENT_SPENDING, spendRate*100, 0);		// *100 to convert to percentage. 0-do not show budget
    }

    //------------------- SCN_TEACHING_QUALITY ---------------//

    if ( scenario_id == SCN_TEACHING_QUALITY ) {
	//-----------------------------------------------------------------------//
	// For the selected departments, reset the priorities for "course preparation",
	// "out-of-class contact", and "educational development" to Rniv[3,1]; "research",
	// "scholarship," and "institutional and public service" to Rniv[7,1].
	// (See Section 2 for the definition of Rniv[m,s].)
	//-----------------------------------------------------------------------//

	for (i=0; i<department_array.department_count; i++) {
	    if ( scen_dept_selected_array[i] != 1 )
		continue;

	    //------ set faculty discretionary time priority -------//

	    Department* deptPtr = department_array[i+1];
	    int t;

	    t = math.get_random_snd(3, 1);
	    deptPtr->priority_discretionary_hour_array[DT_COURSE_PREPARATION] = max(0, min(10,t));
	    t = math.get_random_snd(3, 1);
	    deptPtr->priority_discretionary_hour_array[DT_OUT_OF_CLASS_STUDENT_CONTACT] = max(0, min(10,t));
	    t = math.get_random_snd(3, 1);
	    deptPtr->priority_discretionary_hour_array[DT_EDUCATIONAL_DEVELOPMENT] = max(0, min(10,t));

	    t = math.get_random_snd(7, 1);
	    deptPtr->priority_discretionary_hour_array[DT_RESEARCH] = max(0, min(10,t));
	    t = math.get_random_snd(7, 1);
	    deptPtr->priority_discretionary_hour_array[DT_SCHOLARSHIP] = max(0, min(10,t));
	    t = math.get_random_snd(7, 1);
	    deptPtr->priority_discretionary_hour_array[DT_INSTITUTIONAL_AND_PUBLIC_SERVICE] = max(0, min(10,t));

	    memcpy( deptPtr->last_priority_discretionary_hour_array, deptPtr->priority_discretionary_hour_array, sizeof(deptPtr->priority_discretionary_hour_array));
	}

	//---------- set scenario target ----------//

	scenario_base = (int) sub_score[S_EDUCATIONAL_QUALITY][THIS_MONTH];

	for( i=0 ; i<3 ; i++ ) {
	    scenario_target[i] = scenario_base + (int) goal_config.goal_scen(S_EDUCATIONAL_QUALITY)->goal_value[i];
	    scenario_target[i] = min(100, scenario_target[i]);
	}
    }

    //------------- SCN_RESEARCH_QUALITY --------------//

    if( scenario_id == SCN_RESEARCH_QUALITY ) {
	//-----------------------------------------------------------------------//
	// (3)	For these departments, reset the priorities for "course preparation"
	// and "out-of-class contact" to Rniv[7,1]; set "research" and "scholarship"
	// to Rniv[3,1]. (See Section 2 for the definition of Rniv[m,s].)
	//-----------------------------------------------------------------------//

	for (i=0; i<department_array.department_count; i++) {
	    if ( scen_dept_selected_array[i] != 1 )
		continue;

	    Department* deptPtr = department_array[i+1];
	    int t;

	    t = math.get_random_snd(7, 1);
	    deptPtr->priority_discretionary_hour_array[DT_COURSE_PREPARATION] = max(0, min(10,t));
	    t = math.get_random_snd(7, 1);
	    deptPtr->priority_discretionary_hour_array[DT_OUT_OF_CLASS_STUDENT_CONTACT] = max(0, min(10,t));

	    t = math.get_random_snd(3, 1);
	    deptPtr->priority_discretionary_hour_array[DT_RESEARCH] = max(0, min(10,t));
	    t = math.get_random_snd(3, 1);
	    deptPtr->priority_discretionary_hour_array[DT_SCHOLARSHIP] = max(0, min(10,t));

	    memcpy( deptPtr->last_priority_discretionary_hour_array, deptPtr->priority_discretionary_hour_array, sizeof(deptPtr->priority_discretionary_hour_array));
	}

	//-------- set scenario targets -----------//

	scenario_base = (int) sub_score[S_SCHOLARSHIP_BROADLY_DEFINED][THIS_MONTH];

	for( i=0 ; i<3 ; i++ ) {
	    scenario_target[i] = scenario_base + (int) goal_config.goal_scen(S_SCHOLARSHIP_BROADLY_DEFINED)->goal_value[i];
	    scenario_target[i] = min(100, scenario_target[i]);
	}
    }

    //------------------- SCN_WINNING_ATHLETICS -------------------//

    if ( scenario_id == SCN_WINNING_ATHLETICS ) {
	//-------- set scenario targets -----------//

	scenario_base = athletics_office.ncaa_level_input;

	for( i=0 ; i<3 ; i++ ) {
	    scenario_target[i] = (int) goal_config.goal_scen(SCN_WINNING_ATHLETICS)->goal_value[i];
	}
    }

    //------------------- SCN_LIMIT_TUITION_GROWTH -------------------//

    if ( scenario_id == SCN_LIMIT_TUITION_GROWTH ) {
	scenario_float_base = (float) finance.tuition_rate;

	scenario_target[0] = (int) goal_config.goal_scen(SCN_LIMIT_TUITION_GROWTH)->goal_value[0];
	scenario_target[1] = int( player_school.cur_game_score * 1.1f );
    }

    //------------- SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS --------//

    if ( scenario_id == SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS ) {
	// educational quality
	scenario_base = (int) sub_score[S_EDUCATIONAL_QUALITY][THIS_MONTH];

	for( i=0 ; i<3 ; i++ ) {
	    scenario_target[i] = int( (float) scenario_base * goal_config.goal_scen(SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS)->goal_value[i] );
	}
    }

    //------------------ SCN_STUDENT_DIVERSITY ---------------//

    if ( scenario_id == SCN_STUDENT_DIVERSITY ) {
	scenario_base = enroll_res.overall_percent_minority;

	for( i=0 ; i<3 ; i++ ) {
	    scenario_target[i] = int( enroll_res.overall_percent_minority *
				      goal_config.goal_scen(SCN_STUDENT_DIVERSITY)->goal_value[i] );

	    scenario_target[i] = min(100, scenario_target[i]);
	}
    }

    //------------- SCN_FACULTY_DIVERSITY ----------------//

    if ( scenario_id == SCN_FACULTY_DIVERSITY ) {
	int totalFacCount = department_array.faculty_gender_history[GENDER_ETHNIC_TYPE_COUNT][THIS_YEAR];
	scenario_base1 = ( department_array.faculty_gender_history[MINORITY_MALE][THIS_YEAR] + department_array.faculty_gender_history[MINORITY_FEMALE][THIS_YEAR]) * 100 / totalFacCount;
	scenario_base2 = ( department_array.faculty_gender_history[MINORITY_FEMALE][THIS_YEAR] + department_array.faculty_gender_history[NONMINORITY_FEMALE][THIS_YEAR]) * 100 / totalFacCount;

	float goalPerecent1 = goal_config.goal_scen(SCN_FACULTY_DIVERSITY)->goal_value[0];
	float goalPerecent2 = goal_config.goal_scen(SCN_FACULTY_DIVERSITY)->goal_value[1];

	// should be 1.5 in design doc
	scenario_target[0] = int(( department_array.faculty_gender_history[MINORITY_MALE][THIS_YEAR] + department_array.faculty_gender_history[MINORITY_FEMALE][THIS_YEAR]) * 100 * goalPerecent1 / totalFacCount );
	scenario_target[1] = int(( department_array.faculty_gender_history[MINORITY_FEMALE][THIS_YEAR] + department_array.faculty_gender_history[NONMINORITY_FEMALE][THIS_YEAR]) * 100 * goalPerecent2 / totalFacCount);

	scenario_target[0] = min(100, scenario_target[0] );
	scenario_target[1] = min(100, scenario_target[1] );
	scenario_target[2] = min(100, scenario_target[2] );
    }

    //---------- set scenario_max_game_years and goal year limit ----------//

    scenario_max_game_years = 0;

    for( i=0 ; i<3 ; i++ ) {
	goal_deadline[i] = (char) goal_config.goal_scen(scenario_id)->goal_year[i];

	if( goal_deadline[i] > scenario_max_game_years )
	    scenario_max_game_years = goal_deadline[i];
    }
}

//---------- End of function PlayerSchool::init_scenario ---------//
