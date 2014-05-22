//Filename    : OPSCHOOL.CPP
//Description : School class
//Owner       : Fred

#include <OCONFIG.H>
#include <OAUDIO.H>
#include <OSYS.H>
#include <OMATH.H>
#include <OINFO.H>
#include <ODATE.H>
#include <OGAMESTR.H>
#include <OMOUSECR.H>
#include <OGAMESET.H>
#include <OSCHLRES.H>
#include <OSTUDENT.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OFACULTY.H>
#include <OENROLL.H>
#include <OFINANCE.H>
#include <OCOURSE.H>
#include <OATHLETI.H>
#include <OPSCHOOL.H>
#include <ONEWS.H>

// ## begin chwg0408
enum {
    REPORT_TYPE_DEPARTMENT_PROFILE1, REPORT_TYPE_DEPARTMENT_PROFILE2,
    REPORT_TYPE_DEPARTMENT_ACTIVITIES, REPORT_TYPE_TURNOVER,
    REPORT_TYPE_INSTITUTION_PROFILE1, REPORT_TYPE_INSTITUTION_PROFILE2,
    REPORT_TYPE_INSTITUTION_ACTIVITIES, REPORT_TYPE_INSTITUTION_UNDERGRADUATE,
    REPORT_TYPE_FINANCE,
    REPORT_TYPE_RESIDENCE_HALL,
};
enum { REPORT_MODE_INSTITUTION, REPORT_MODE_DEPARTMENT};
enum {
    MODE_OVERALL=0,
    MODE_RANK=1,
    MODE_OUTPUTS=2,
    MODE_GENERAL_INSTITUTIONAL_PERFORMANCE=3,
    MODE_ATTITUDES_TOWARD_THE_INSTITUTION=4,
    MODE_FINANCE=5,
    MODE_MANAGEMENT=6,
    MODE_ADMISSION=7,
};

// ## end chwg0408

//---------- Begin of function PlayerSchool::init -----------//
//!
void PlayerSchool::init() {
    memset( this, 0, sizeof(PlayerSchool) );

    //## chea 270999 make the student no. report update
    memset( ins_student_level_history,0,sizeof(ins_student_level_history));

    init_config();
    default_setting();
    // ## begin chwg0408
    current_score_report_type=MODE_OVERALL;
    current_report_type=REPORT_TYPE_INSTITUTION_PROFILE1;
    current_report_mode=REPORT_MODE_INSTITUTION;

    // ## end chwg0408
}

//---------- End of function PlayerSchool::init -----------//

//---------- Begin of function PlayerSchool::deinit -----------//
//!
void PlayerSchool::deinit() {
    //student_array.deinit();
}

//---------- End of function PlayerSchool::deinit -----------//

//---------- Begin of function PlayerSchool::init_config -----------//
//!
void PlayerSchool::init_config() {
    memset( this, 0, sizeof(PlayerSchool) );

    //------ initialize teaching_method_pref -----//

    teaching_method_pref[SEMINAR]           = 25;
    teaching_method_pref[DISTANCE_LEARN_COURSE] = 5;// class without breakout and class with breakout are classified as one group in selecting teaching method
    teaching_method_pref[CLASS_WITH_BREAKOUT]   = 30;
    teaching_method_pref[BREAKOUT_LAB]        = 0;
    teaching_method_pref[GENERAL]           = 40;

    //------ initialize course_match_order -------//

    course_match_order[0] = DOCTOR;
    course_match_order[1] = MASTER;
    course_match_order[2] = UG_TRADITION;
    course_match_order[3] = UG_NONTRADITION;
    course_match_order[4] = DISTANCE_LEARN;

    //------ initialize trimester -------//
    //##### begin fred 0805 #####//
    //BUGHERE0 the holidays among the trimesters may introduce semantic BUG
    // should define one more constant besides SUMMER,...
    //##### end fred 0805 #####//

    trimester_array[AUTUMN].start_month = 9;        // 1 Sep
    trimester_array[AUTUMN].start_day   = 1;
    trimester_array[AUTUMN].end_month   = 12;       // 26 Dec
    trimester_array[AUTUMN].end_day     = 26;

    trimester_array[WINTER].start_month = 1;        // 14 Jan
    trimester_array[WINTER].start_day   = 14;
    trimester_array[WINTER].end_month   = 5;        // 9 May
    trimester_array[WINTER].end_day     = 9;

    trimester_array[SUMMER].start_month = 5;        // 20 May
    trimester_array[SUMMER].start_day   = 20;
    trimester_array[SUMMER].end_month   = 8;        // 31 Aug
    trimester_array[SUMMER].end_day     = 30;       // game_day is limited to 1-30; see info.next_day()

    //--- calculate the total number of days in each trimester ---//

    trimester_array[AUTUMN].calc_total_days();
    trimester_array[WINTER].calc_total_days();
    trimester_array[SUMMER].calc_total_days();
}

//---------- End of function PlayerSchool::init_config -----------//

//---------- Begin of function PlayerSchool::default_setting ----------//
//!
void PlayerSchool::default_setting() {
    cur_trimester=FIRST_TRIMESTER;
    //	new_trimester(AUTUMN);

    //--- for testing only, set to private research school ---//

    control      = PRIVATE;
    school_type = PRIVATE_SCHOOL;

    mascot = 1;                                     // not used yet
    while( player_school.is_hidden_mascot(player_school.mascot) ) {
	mascot++;
    }

    strcpy( school_name,       "Player University" );
    strcpy( player_first_name, "Player"     );
    strcpy( player_last_name,  "Name"     );

    scenario_id = 0;                                // not a scenario

    student_market = REGIONAL;

    location =  PACIFIC;

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

    difficulty_multiplier = 1.0f;

    chance_event_frequency = INPUT_MEDIUM;          // INPUT_LOW, INPUT_MEDIUM
    chance_objective_frequency = INPUT_MEDIUM;
    chance_perturbations = INPUT_MEDIUM;
    behavioral_response_speed = INPUT_LOW;          // INPUT_LOW=Normal,INPUT_MEDIUM=faster,INPUT_HIGH=light-speed
    threshold_event_flag = 1;

    summer_emphasis = INPUT_LOW;
    summer_emphasis_last = INPUT_LOW;
    percent_summer_teaching_load = 0;

    err_when(INPUT_LOW != 0);

    calc_setting_vars();
}

//----------- End of function PlayerSchool::default_setting -----------//

//---------- Begin of function PlayerSchool::init_setting_non_scenario ----------//
//!
void PlayerSchool::init_setting_non_scenario() {
    char pPrestige[]    = {                         //## chea 170899 larger range between med & high org. {1,5,10}
	1,3,10
    };

    //	char pDistanceL[]		= {0,10,30}; //## chea 0ringal idea
    char pDistanceL[]   = {                         //## chea 170999 I find out there are too much sl5 so I lower it.
	0,5,20
    };

    char pUgPt[]      = {0,20,50};

    //char pDegFiveYear[]	= {50,70,90};  //## chea org.
    char pDegFiveYear[] = {                         //## chea 130899 useed in oschlres.cpp ln 225
	50,70,100
    };

    char pUgInHousing[] = {30,60,90};

    char pGradStu[]   = {5,20,35};
    char docIntense[]   = {0,5,10};
    char researchIntense[]  = {                     //## chea the org. setting
	0,5,10
    };

    prestige        = pPrestige[prestige];
    sl5_student_percent = pDistanceL[sl5_student_percent];
    parttime_undergrad_percent = pUgPt[parttime_undergrad_percent];
    ug_get_degree_in_5year_percent = pDegFiveYear[ug_get_degree_in_5year_percent];
    fulltime_ug_in_housing_percent = pUgInHousing[fulltime_ug_in_housing_percent];

    graduate_student_percent    = pGradStu[graduate_student_percent];
    doctoral_program_intensity    = docIntense[doctoral_program_intensity];
    sponsored_research_intensity  = researchIntense[sponsored_research_intensity];
    /*
      //BUGHERE
      student_market = NATIONAL;
      prestige 		= 8;
      student_count	= INPUT_MEDIUM;
      sl5_student_percent	= 1;
      parttime_undergrad_percent = 1;
      fulltime_ug_in_housing_percent = 60;
      athletic_program_intensity	= INPUT_MEDIUM;
      graduate_student_percent	= 30;
      doctoral_program_intensity = 7;
      sponsored_research_intensity =7;
      control = PRIVATE;
      campus_environment = SUBURBAN;	// (m.random(2) == 0)?URBAN:SUBURBAN;
    */
}

//----------- End of function PlayerSchool::init_setting_non_scenario -----------//

//---------- Begin of function PlayerSchool::calc_setting_vars ----------//
//!
//! Calculate variables based on the settings.
//!
void PlayerSchool::calc_setting_vars() {
    //----- set percent_student_in_state -----//

    switch(student_market) {
    case NATIONAL:
	percent_student_in_state = 30;
	break;

    case REGIONAL:
	percent_student_in_state = 50;
	break;

    case LOCAL:
	percent_student_in_state = 70;
	break;
    }

    //----- set relative_wealth_value -------//

    switch(relative_wealth) {
    case WEALTH_RICH:
	relative_wealth_value = 2;                  // if we set it to 3, for some unknown reasons, we will get very low sponsored research amounts for all departments.
	break;

    case WEALTH_IN_BETWEEN:
	relative_wealth_value = 2;
	break;

    case WEALTH_STRUGGLING:
	relative_wealth_value = 1;
	break;
    }
}

//----------- End of function PlayerSchool::calc_setting_vars -----------//

//---------- Begin of function PlayerSchool::next_day ----------//
//!
void PlayerSchool::next_day() {

    //--- if now it is at the end of a trimester, close the current trimester and open a new trimester ---//

    if( trimester_array[cur_trimester].end_month == info.game_month &&
	trimester_array[cur_trimester].end_day == info.game_day ) {
	is_trimester_off  = 1;                        // it is now in a trimester break
	trimester_off_day = 1;                        // the nth day since the trimester is off

	//--- if the current trimester is the last trimester, proceeds to the first trimester in the new school year ---//

	if( ++cur_trimester >= TRIMESTER_PER_YEAR ) {
	    cur_trimester = 0;
	}

	new_trimester(cur_trimester);

	// new_trimester() above includes graduate() and dropout()

	enroll_res.calc_student_count();

	// faculty hiring can be added here

	//---------------------------------------//
    }

    //------- if the trimester is off -------//

    if( is_trimester_off ) {
	//--- the trimester break ends when the time reaches the beginning of the new trimester ---//

	if( trimester_array[cur_trimester].start_month == info.game_month &&
	    trimester_array[cur_trimester].start_day == info.game_day ) {
	    is_trimester_off = 0;
	    //#### begin fred 0911 ####//
	    info.graph_trimester_passed = min(info.graph_trimester_passed+1, HISTORY_TRIMESTER_COUNT);
	    //#### begin fred 0915 ####//
	    info.graph_trimester = min(info.graph_trimester+1, HISTORY_TRIMESTER_COUNT);
	}
	else {
	    trimester_off_day++;                        // for various function's uses like student selecting courses.
	}
    }
}

//----------- End of function PlayerSchool::next_day -----------//

//---------- Begin of function PlayerSchool::new_trimester ----------//
//!
void PlayerSchool::new_trimester(int trimesterId) {
    int oldCursor = mouse_cursor.get_icon();

    mouse_cursor.set_icon(CURSOR_WAITING);

    //------------------------------------//

    cur_trimester = trimesterId;

    cur_trimester_start_date = date.julian( info.game_year,
					    trimester_array[cur_trimester].start_month,
					    trimester_array[cur_trimester].start_day );

    //----- call next_trimester() of other classes -----//
    //#### begin fred 0922 ####//
    next_trimester();                               // recalc adjusted_student_major_pref for change major
    //#### end fred 0922 ####//

    department_array.calc_course_pref();            // for select course

    sys.yield();

    for( int i=department_array.size() ; i>0 ; i--) {
	if( department_array.is_deleted(i) )
	    continue;

	Department* deptPtr = department_array[i];

	deptPtr->faculty_array.next_trimester();      // will think_departure, promotion

	sys.yield();

	deptPtr->student_array.next_trimester();      // will think_graduate, dropout

	sys.yield();
    }

    department_res.general_dept.student_array.next_trimester();

    sys.yield();

    mouse_cursor.restore_icon(oldCursor);

    //------ the start of a new academic year -------//

    if( cur_trimester == FIRST_TRIMESTER )          // RESOURCE ALLOCATION HERE!
	finance.new_fiscal_year();                    // assume called after faculty departure
    // and graduation, dropout

    //---------------------------------------//

}

//----------- End of function PlayerSchool::new_trimester -----------//

//---------- Begin of function PlayerSchool::trimester_str ----------//
//!
char* PlayerSchool::trimester_str() {
    static String str;

    str  = game_str_res.trimester_str(cur_trimester);
    str += " \'";
    str += info.financial_year();

    return str;
}

//----------- End of function PlayerSchool::trimester_str -----------//

//---------- Begin of function Trimester::calc_total_days ----------//
//!
void Trimester::calc_total_days() {
    int startDate = date.julian( 1990, start_month, start_day );
    int endDate;

    if( end_month > start_month )                   // in the same year
	endDate = date.julian( 1990, end_month, end_day );
    else
	// in the next year
	endDate = date.julian( 1991, end_month, end_day );

    total_days = endDate - startDate + 1;
}

//----------- End of function Trimester::calc_total_days -----------//

//## chea 270999 for update stu_sum only
//---------- Begin of function PlayerSchool::update_stu_sum ----------//
//!
void PlayerSchool::update_stu_sum() {
    int i,j,k,l,m;
    int test_course_count=0;
    int stdnotest1 = 0;                             //## chea 270899
    int stdnotest2 = 0;                             //## chea 270899
    int underno = 0;
    int nontrno =0;
    int masterno = 0;
    int docno =0;
    int dlno =0;
    int is_gen_dept =0;
    int not_h_course = 0;
    int not_h_course1 = 0;
    int not_h_course2 = 0;
    int not_h_course3 = 0;
    int coures_less_40 =0 ;
    int coures_less_41 =0 ;
    int coures_less_42 =0 ;
    int coures_less_43 =0 ;
    int coures_less_44 =0 ;
    int count = 0;

    StudentArray* stuArr;
    Student *stuPtr;
    Department*  cur_dept_ptr;

    //reset all variable
    // zero it out
    memset(ins_student_level_no, 0, sizeof(ins_student_level_no));

    for ( l=department_array.size(); l>=0; l-- ) {
	if (l) {
	    if ( department_array.is_deleted(l) )
		continue;
	    cur_dept_ptr = department_array[l];

	    for(m=0; m<MAX_STUDENT_LEVEL+1; m++) {
		cur_dept_ptr->dept_student_level_no[m] = 0;
		if(info.prerun_year == 1 && info.game_month == 9 && info.game_day == 1) {
		    for(int b=0; b<HISTORY_TRIMESTER_COUNT; b++)
			cur_dept_ptr->dept_student_level_history[m][b] = 0;
		}
	    }
	}
	else {
	    for(m=0; m<MAX_STUDENT_LEVEL+1; m++)
		department_res.general_dept.dept_student_level_no[m] = 0;
	}
    }

    //dept_student_level_no
    err_when( department_array.size() == 0 );
    for ( i=department_array.size(); i>=0; i-- ) {
	is_gen_dept =0;
	if (i) {
	    if ( department_array.is_deleted(i) )
		continue;
	    stuArr = &(department_array[i]->student_array);
	    cur_dept_ptr = department_array[i];

	}
	else {
	    stuArr = &(department_res.general_dept.student_array);
	    is_gen_dept =1;
	}
	// loop student

	for (j=stuArr->size(); j>0; j--) {
	    if ( stuArr->is_deleted(j) )
		continue;
	    stdnotest1++;
	    sys.yield();
	    stuPtr = stuArr->operator[](j);

	    // student_level{ UG_TRADITION, UG_NONTRADITION, MASTER, DOCTOR, DISTANCE_LEARN }
	    if(stuPtr->student_level == UG_TRADITION) {
		if(is_gen_dept == 0) {
		    underno++;
		    cur_dept_ptr->dept_student_level_no[UG_TRADITION]++;
		}
		else
		    department_res.general_dept.dept_student_level_no[UG_TRADITION]++;
	    }
	    else if(stuPtr->student_level == UG_NONTRADITION) {
		if(is_gen_dept == 0) {
		    nontrno++;
		    cur_dept_ptr->dept_student_level_no[UG_NONTRADITION]++;
		}
		else
		    department_res.general_dept.dept_student_level_no[UG_NONTRADITION]++;
	    }
	    else if(stuPtr->student_level == MASTER) {
		if(is_gen_dept == 0) {
		    masterno++;
		    cur_dept_ptr->dept_student_level_no[MASTER]++;
		}
		else
		    department_res.general_dept.dept_student_level_no[MASTER]++;
	    }
	    else if(stuPtr->student_level == DOCTOR) {
		if(is_gen_dept == 0) {
		    docno++;
		    cur_dept_ptr->dept_student_level_no[DOCTOR]++;
		}
		else
		    department_res.general_dept.dept_student_level_no[DOCTOR]++;
	    }
	    else if(stuPtr->student_level == DISTANCE_LEARN) {
		if(is_gen_dept == 0) {
		    dlno++;
		    cur_dept_ptr->dept_student_level_no[DISTANCE_LEARN]++;
		}
		else
		    department_res.general_dept.dept_student_level_no[DISTANCE_LEARN]++;
	    }

	}                                             // end for j

	// fix in version 2
	if(is_gen_dept == 0) {
	    for(k=0; k<MAX_STUDENT_LEVEL; k++)
		cur_dept_ptr->dept_student_level_no[MAX_STUDENT_LEVEL] += cur_dept_ptr->dept_student_level_no[k];
	}
	else {
	    for(k=0; k<MAX_STUDENT_LEVEL; k++)
		department_res.general_dept.dept_student_level_no[MAX_STUDENT_LEVEL] += department_res.general_dept.dept_student_level_no[k];
	}

    }

    ins_student_level_no[UG_TRADITION] = underno;
    ins_student_level_no[UG_NONTRADITION] = nontrno;
    ins_student_level_no[MASTER] = masterno;
    ins_student_level_no[DOCTOR] = docno;
    ins_student_level_no[DISTANCE_LEARN] = dlno;
    ins_student_level_no[MAX_STUDENT_LEVEL] = underno + nontrno + masterno +docno +dlno;

    for(k=0; k<MAX_STUDENT_LEVEL+1; k++)
	player_school.ins_student_level_history[k][HISTORY_TRIMESTER_COUNT-1] = ins_student_level_no[k];

    //## chea 300999 create a history for those graphs in student no. report
    for ( i=department_array.size(); i>=0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;
	cur_dept_ptr = department_array[i];
	//shift history
	for(int count1=0;count1<MAX_STUDENT_LEVEL_N_TOTAL;count1++)
	    shift_history(cur_dept_ptr->dept_student_level_history[count1], HISTORY_TRIMESTER_COUNT);

	//-- department wide --//
	for(int count=0;count<MAX_STUDENT_LEVEL_N_TOTAL;count++) {
	    cur_dept_ptr->dept_student_level_history[count][HISTORY_TRIMESTER_COUNT-1] = cur_dept_ptr->dept_student_level_no[count];
	}
    }

}

//----------- End of function PlayerSchool::update_stu_sum -----------//

//## chea 290999 for update stu_sum only
//---------- Begin of function PlayerSchool::update_gender ----------//
//!
void PlayerSchool::update_gender() {
    int i,j,k,l,m;

    int gender1 = 0;
    int gender2 = 0;
    int gender3 = 0;
    int gender4 = 0;

    int is_gen_dept = 0;
    int not_h_course = 0;
    int not_h_course1 = 0;
    int not_h_course2 = 0;
    int not_h_course3 = 0;
    int coures_less_40 = 0 ;
    int coures_less_41 = 0 ;
    int coures_less_42 = 0 ;
    int coures_less_43 = 0 ;
    int coures_less_44 = 0 ;
    int count = 0;

    StudentArray* stuArr;
    Student *stuPtr;
    Department*  cur_dept_ptr;

    //reset all variable
    // zero it out
    memset(ins_gender_student_level_no, 0, sizeof(ins_gender_student_level_no));

    for ( l=department_array.size(); l>=0; l-- ) {
	if (l) {
	    if ( department_array.is_deleted(l) )
		continue;
	    cur_dept_ptr = department_array[l];

	    for(m=0; m<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; m++) {
		cur_dept_ptr->dept_gender_student_level_no[m] = 0;
		if(info.prerun_year == 1 && info.game_month == 9 && info.game_day == 1) {
		    for(int b=0; b<HISTORY_TRIMESTER_COUNT; b++)
			cur_dept_ptr->dept_gender_student_level_history[m][b] = 0;
		}
	    }
	}
	else {
	    for(m=0; m<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; m++)
		department_res.general_dept.dept_gender_student_level_no[m] = 0;
	}
    }

    //dept_gender_student_level_no
    err_when( department_array.size() == 0 );
    for ( i=department_array.size(); i>=0; i-- ) {
	is_gen_dept =0;
	if (i) {
	    if ( department_array.is_deleted(i) )
		continue;
	    stuArr = &(department_array[i]->student_array);
	    cur_dept_ptr = department_array[i];

	}
	else {
	    stuArr = &(department_res.general_dept.student_array);
	    is_gen_dept =1;
	}
	// loop student

	for (j=stuArr->size(); j>0; j--) {
	    if ( stuArr->is_deleted(j) )
		continue;

	    sys.yield();
	    stuPtr = stuArr->operator[](j);

	    if ( stuPtr->student_level == DISTANCE_LEARN )
		continue;

	    // gender_level  { NONMINORITY_MALE, NONMINORITY_FEMALE, MINORITY_MALE, MINORITY_FEMALE };
	    if(stuPtr->gender_ethnic_group == NONMINORITY_MALE) {
		if(is_gen_dept == 0) {
		    gender1++;
		    cur_dept_ptr->dept_gender_student_level_no[NONMINORITY_MALE]++;
		}
		else
		    department_res.general_dept.dept_gender_student_level_no[NONMINORITY_MALE]++;
	    }
	    else if(stuPtr->gender_ethnic_group == NONMINORITY_FEMALE) {
		if(is_gen_dept == 0) {
		    gender2++;
		    cur_dept_ptr->dept_gender_student_level_no[NONMINORITY_FEMALE]++;
		}
		else
		    department_res.general_dept.dept_gender_student_level_no[NONMINORITY_FEMALE]++;
	    }
	    else if(stuPtr->gender_ethnic_group == MINORITY_MALE) {
		if(is_gen_dept == 0) {
		    gender3++;
		    cur_dept_ptr->dept_gender_student_level_no[MINORITY_MALE]++;
		}
		else
		    department_res.general_dept.dept_gender_student_level_no[MINORITY_MALE]++;
	    }
	    else if(stuPtr->gender_ethnic_group == MINORITY_FEMALE) {
		if(is_gen_dept == 0) {
		    gender4++;
		    cur_dept_ptr->dept_gender_student_level_no[MINORITY_FEMALE]++;
		}
		else
		    department_res.general_dept.dept_gender_student_level_no[MINORITY_FEMALE]++;
	    }

	}                                             // end for j

	for(k=0; k<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1; k++)
	    cur_dept_ptr->dept_gender_student_level_no[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1] += cur_dept_ptr->dept_gender_student_level_no[k];
    }

    //I add the general department student as well
    ins_gender_student_level_no[NONMINORITY_MALE] = gender1 + department_res.general_dept.dept_gender_student_level_no[NONMINORITY_MALE];
    ins_gender_student_level_no[NONMINORITY_FEMALE] = gender2+ department_res.general_dept.dept_gender_student_level_no[NONMINORITY_FEMALE];
    ins_gender_student_level_no[MINORITY_MALE] = gender3+ department_res.general_dept.dept_gender_student_level_no[MINORITY_MALE];
    ins_gender_student_level_no[MINORITY_FEMALE] = gender4+ department_res.general_dept.dept_gender_student_level_no[MINORITY_FEMALE];
    ins_gender_student_level_no[GENDER_ETHNIC_TYPE_COUNT] = gender1+gender2+gender3+gender4;

    for(k=0; k<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; k++)
	player_school.ins_gender_student_level_history[k][HISTORY_TRIMESTER_COUNT-1] = ins_gender_student_level_no[k];

    //## chea 300999 create a history for those graphs in student no. report
    for ( i=department_array.size(); i>=0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;
	cur_dept_ptr = department_array[i];
	//shift history
	for(int count1=0;count1<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;count1++)
	    shift_history(cur_dept_ptr->dept_gender_student_level_history[count1], HISTORY_TRIMESTER_COUNT);

	//-- department wide --//
	for(int count=0;count<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;count++) {
	    cur_dept_ptr->dept_gender_student_level_history[count][HISTORY_TRIMESTER_COUNT-1] = cur_dept_ptr->dept_gender_student_level_no[count];
	}
    }

}

//----------- End of function PlayerSchool::update_gender -----------//

//----------- Begin of function PlayerSchool::latency_func -----------//
//!
//! adjust latencyPara for math.latency_func
//!
float PlayerSchool::latency_func(float latencyPara, float curValue, float inputValue) {
    if(behavioral_response_speed==INPUT_MEDIUM)     // faster
	// may be similar to gamma function
	return math.latency_func(latencyPara*0.5f, curValue, inputValue);
    else if(behavioral_response_speed==INPUT_HIGH)  // light-speed
	// or 1.0f or just return inputValue?
	return math.latency_func(0.0f, curValue, inputValue);
    else
	// INPUT_LOW
	return math.latency_func(latencyPara, curValue, inputValue);
}

//----------- End of function PlayerSchool::latency_func -----------//

//----------- Begin of function PlayerSchool::is_hidden_mascot -----------//
//!
//! introduced in version 2
//! return whether a mascot (school icon) is disabled
int PlayerSchool::is_hidden_mascot(int mascotId) {
    if( mascot==5 )                                 // hide indian mascot in version 2
	return 1;
    return 0;
}

//----------- End of function PlayerSchool::is_hidden_mascot -----------//
