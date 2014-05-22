//Filename    : ODEPT.CPP
//Description : Department object

#include <OSYS.H>
#include <OGAMESET.H>
#include <OMATH.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OFIRM.H>
#include <OWORLD.H>
#include <OROAD.H>
#include <OFIRMRES.H>
#include <OFACULTY.H>
#include <OSCHLRES.H>
#include <OPSCHOOL.H>
#include <OFINANCE.H>                             // fiscal_start_month

// --- Begin of function Department::Department --- //
//
Department::Department() : cur_faculty_array(sizeof(Faculty)), display_faculty_array(sizeof(Faculty)) {
}

//
// --- End of function Department::Department --- //

//---------- Begin of function Department::init -----------//
//!
void Department::init(int departmentId) {
    GeneralDepartment::init();
    department_id = departmentId;
    firm_recno    = 0;

    enum {
	REPORT_TYPE_DEPARTMENT_OVERVIEW, REPORT_TYPE_DEPARTMENT_CLASSTYPES,
	REPORT_TYPE_INSTITUTION_OVERVIEW, REPORT_TYPE_INSTITUTION_SUMMERTEACHING,
    };
    current_report_type=REPORT_TYPE_INSTITUTION_OVERVIEW;

    faculty_array.department_recno = department_recno;
    student_array.department_recno = department_recno;
    course_array.department_recno = department_recno;

    department_res[department_id];                  // for debug

    //------ init performance indicators ------//

    p_faculty_research = 0;
    p_academic_standing = 0;
    // add in GAME_VERSION>=200
    p_academic_standing_no_latency = 0;
    p_tech_utilization = 0;
    p_educational_quality = 0;
    p_faculty_morale = 0;
    p_student_morale = 0;
    p_faculty_diversity = 0;                        //##### fred 0818

    cummulative_tech_utilization = 0;               // 1104
    department_incentives = 50;
    ease_outsourcing_credits = 50;
    distance_learning_credits = 50;

    distance_learning_enrollment = 0;
    distance_credits_provided = 0;
    total_distance_learning_place = 0;

    //----- link this department to a firm -----//

    create_department_firm();

    //----------- generate data; orders according to page3 of techdoc 3.3 -----------//

    generate_course();

    //##### begin fred 0824 #####//
    total_research_dollar = 0;
    total_research_dollar_direct = 0;
    research_dollar_norm  = 0;
    percent_investigator = 0;

    regular_faculty_count = 0;

    memset(p_educational_quality_history, 0, sizeof(p_educational_quality_history));
    memset(p_academic_standing_history, 0, sizeof(p_academic_standing_history));
    memset(p_student_morale_history, 0, sizeof(p_student_morale_history));

    memset(p_faculty_research_history, 0, sizeof(p_faculty_research_history));
    memset(p_faculty_morale_history, 0, sizeof(p_faculty_morale_history));
    memset(p_faculty_diversity_history, 0, sizeof(p_faculty_diversity_history));
    memset(p_tech_utilization_history, 0, sizeof(p_tech_utilization_history));
    memset(distance_credits_provided_history, 0, sizeof(distance_credits_provided_history));

    memset(departures_history, 0, sizeof(departures_history));
    memset(hires_history, 0, sizeof(hires_history));
    memset(promotion_history, 0, sizeof(promotion_history));

    memset(var_educational_quality_history, 0, sizeof(var_educational_quality_history));
    memset(var_academic_standing_history, 0, sizeof(var_academic_standing_history));
    memset(var_student_morale_history, 0, sizeof(var_student_morale_history));

    memset( var_change_educational_quality,0,sizeof(var_change_educational_quality));
    memset( var_change_academic_standing,0,sizeof(var_change_academic_standing));
    //	memset( var_change_student_morale,0,sizeof(var_change_student_morale));

    memset( faculty_level_history,0,sizeof(faculty_level_history));
    memset( faculty_gender_history,0,sizeof(faculty_gender_history));

    memset( student_level_history,0,sizeof(student_level_history));
    memset( student_gender_history,0,sizeof(student_gender_history));

    memset( research_m_history,0,sizeof(research_m_history));
    memset( research_y_history,0,sizeof(research_y_history));
    memset( research_total_history,0,sizeof(research_total_history));
    memset( research_direct_history,0,sizeof(research_direct_history));
    memset( regular_faculty_with_research,0,sizeof(regular_faculty_with_research));

    //----- init player input ------------------------------//
    int i;

    //## 990609 fred
    for ( i=0; i<DISCRETIONARY_TYPE_COUNT; i++ ) {
	int t = math.get_random_snd(5, 0.6f);
	priority_discretionary_hour_array[i] = max(0, min(10,t));
    }

    memcpy( last_priority_discretionary_hour_array, priority_discretionary_hour_array, sizeof(priority_discretionary_hour_array));

    //## 990609 fred

    pressure_to_change_teaching_load = 0;

    CourseTemplate* templ = course_res.get_template(department_id);

    int sum;

    for ( sum=0,i=0; i<COURSE_PREFERENCE_COUNT; i++ ) {
	course_preference_input[i] = templ->target_preference[i];
	course_preference[i] = course_preference_input[i];
	sum+=course_preference[i];
    }

    //------ scenario special handling -------//

    if( player_school.scenario_id == SCN_TEACHING_QUALITY ) {
	//---- increase class with breakout preference by 50% -----//

	int t = course_preference_input[CLASS_WITH_BREAKOUT];

	course_preference_input[CLASS_WITH_BREAKOUT] += t;
	course_preference[CLASS_WITH_BREAKOUT]      += t;

	//----- normalize the sum to 100 ------//

	int newSum=0;

	for ( i=0; i<COURSE_PREFERENCE_COUNT; i++ ) {
	    course_preference_input[i] = (int) course_preference_input[i] * 100 / sum;
	    newSum += course_preference_input[i];
	}

	// the total may not be exactly 100, do this to make it 100.
	course_preference_input[COURSE_PREFERENCE_COUNT-1] += 100-newSum;
    }

    //----------------------------------------//

    err_when(sum!=100);

    dept_salary_multiplier = 1.0f;

    teaching_talent_priority = 1;                   // for new hires // = {Very important = 3; Important = 2; Moderately important = 1; Not important = 0}
    scholarship_talent_priority = 1;
    research_talent_priority = 1;

    //## chea 260799 try to init scenario SCN_RAISE_SALARY
    if (player_school.scenario_id==SCN_RAISE_SALARY)
	assistant_professor_priority = 0;             // = {Very important = 3; Important = 2; Doesn't matter = 1; Don't do it = 0}
    else
	assistant_professor_priority = 2;

    cost_containment_priority = 0;
    tenured_faculty_priority = 0;
    longterm_adjunct_faculty_priority = 1;
    shortterm_adjunct_faculty_priority = 1;
    female_faculty_priority = 1;
    minority_faculty_priority = 1;

    for (i=0; i<GENDER_ETHNIC_TYPE_COUNT; i++)
	salary_multiplier[i] = 1.0f;

    doctors_per_research_dollar = 0;
    breakout_section_by_doctor = 0;
}

//---------- End of function Department::init -----------//

//------- Begin of function Department::name -------//
//!
char* Department::name() {
    static String str;

    str = department_res[department_id]->name;
    str += " Department";

    return str;
}

//------- End of function Department::name --------//

//------- Begin of function Department::create_department_firm -------//
//!
int Department::create_department_firm() {
    int blockWidth=10, blockHeight=8;
    char debugFlag=0;

    //------ get a firm type of the matched field ----//

    int curFieldId = department_res[department_id]->field_id;
	int firmId;
    for( firmId=firm_res.firm_count ; firmId>0 ; firmId-- ) {
	if( firm_res[firmId]->field_id == curFieldId ) {
	    if( firm_res[firmId]->is_available() ) {
		if( firm_res[firmId]->loc_width <= blockWidth-ROAD_WIDTH &&
		    firm_res[firmId]->loc_height <= blockHeight-ROAD_HEIGHT ) {
		    break;
		}
		else
		    debugFlag = 1;
	    }
	    else
		debugFlag = 2;
	}
    }

    err_when( firmId==0 );

    //------- attempt to add the firm on the map ------//

    FirmInfo* firmInfo = firm_res[firmId];

    for( int yLoc=2 ; yLoc+blockHeight<=MAX_WORLD_Y_LOC ; yLoc+=blockHeight ) {
	for( int xLoc=2 ; xLoc+blockWidth<=MAX_WORLD_X_LOC ; xLoc+=blockWidth ) {
	    int buildXLoc = xLoc + (blockWidth-ROAD_WIDTH-firmInfo->loc_width)/2;
	    int buildYLoc = yLoc + (blockHeight-ROAD_HEIGHT-firmInfo->loc_height)/2;

	    if( !firmInfo->can_build(buildXLoc, buildYLoc) )
		continue;

	    int firmRecno = firm_array.add_firm( buildXLoc, buildYLoc, firmId );

	    //----- set linkage between the firm and the department ----//

	    firm_array[firmRecno]->department_recno = department_recno;
	    firm_recno = firmRecno;

	    return 1;
	}
    }

    return 0;                                       // no space for adding the firm
}

//------- End of function Department::create_department_firm -------//

//---------- Begin of function Department::generate_course -----------//
//!
void Department::generate_course() {
    for( int i=0 ; i<MAX_COURSE_DEPTH_FOCUS ; i++ ) {
	// total: 7*5 = 35
	for( int j=0 ; j<MAX_TEACHING_METHOD ; j++ ) {
	    Course course;

	    memset( &course, 0, sizeof(Course) );

	    course.department_recno = department_recno;

	    course.depth_category = course_array.get_depth(i);
	    course.focus_category = course_array.get_focus(i);

	    course.teaching_method = j;

	    course_array.add(&course);
	}
    }
}

//---------- End of function Department::generate_course -----------//

//##### begin fred 0824 #####//
//---------- Begin of function Department::next_day -----------//
//!
void Department::next_day() {
    // special case; before faculty_array.next_day()

    if ( info.game_day == 1 ) {
	for (int i=0; i<RESEARCH_STATUS_COUNT; i++)
	    shift_history(research_m_history[i], HISTORY_MONTH_COUNT);

	shift_history(research_total_history, HISTORY_MONTH_COUNT);
	shift_history(research_direct_history, HISTORY_MONTH_COUNT);
	shift_history(regular_faculty_with_research, HISTORY_MONTH_COUNT);

	if ( info.game_month == finance.fiscal_year_start_month )
	    for (int i=0; i<RESEARCH_STATUS_COUNT; i++)
		shift_history(research_y_history[i], HISTORY_YEAR_COUNT);
    }

    //--------------------------//

    student_array.next_day();

    faculty_array.next_day();

    course_array.next_day();

    // start of a trimester
    if ( info.game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	 && info.game_month == player_school.trimester_array[player_school.cur_trimester].start_month ) {
	update_history(UPDATE_TRIMESTER);
    }

    // start of a month
    if ( info.game_day == 1 ) {
	update_history(UPDATE_MONTH);
	sys.yield();

	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month ) {
	    update_history(UPDATE_YEAR);
	    // reset this variable value
	    this->project_added_this_year = false;
	}
    }
}

//---------- End of function Department::next_day -----------//

//---------- Begin of function Department::update_history -----------//
//!
//! update history arrays
//!
//! update_flag		= UPDATE_TRIMESTER, UPDATE_MONTH, UPDATE_YEAR; (ooffice.h)
//!
void Department::update_history(char update_flag) {
    int i;

    switch (update_flag) {
    case UPDATE_MONTH:
	shift_history(p_faculty_research_history, HISTORY_MONTH_COUNT);
	shift_history(p_faculty_morale_history, HISTORY_MONTH_COUNT);

	calc_faculty_research();
	calc_faculty_morale();

	p_faculty_research_history[THIS_MONTH] = p_faculty_research;
	p_faculty_morale_history[THIS_MONTH] = p_faculty_morale;

	// special case
	calc_research_dollar();

	calc_research_history();                    // 1217		// must after calc_research_dollar()
	break;

    case UPDATE_TRIMESTER:
	shift_history(p_educational_quality_history, HISTORY_TRIMESTER_COUNT);
	shift_history(p_student_morale_history, HISTORY_TRIMESTER_COUNT);

	for (i=0; i<EDUCATIONAL_QUALITY_VAR_COUNT; i++)
	    shift_history(var_educational_quality_history[i], HISTORY_TRIMESTER_COUNT);
	for (i=0; i<STUDENT_MORALE_VAR_COUNT; i++)
	    shift_history(var_student_morale_history[i], HISTORY_TRIMESTER_COUNT);

	shift_history(p_tech_utilization_history, HISTORY_TRIMESTER_COUNT);

	calc_educational_quality();
	calc_student_morale();                      // must after calc_educational_quality
	calc_tech_utilization();                    // special case don't have history

	p_educational_quality_history[THIS_TRIMESTER] = p_educational_quality;
	p_student_morale_history[THIS_TRIMESTER] = p_student_morale;
	p_tech_utilization_history[THIS_TRIMESTER] = p_tech_utilization;

	shift_history( distance_credits_provided_history, HISTORY_TRIMESTER_COUNT );
	distance_credits_provided_history[THIS_TRIMESTER] = distance_credits_provided;

	break;

    case UPDATE_YEAR:
	for (i=0;i<FACULTY_RANK_LEVEL_COUNT_N_TOTAL; i++)
	    shift_history(faculty_level_history[i], HISTORY_YEAR_COUNT);

	for (i=0;i<MAX_STUDENT_LEVEL_N_TOTAL; i++)
	    shift_history(student_level_history[i], HISTORY_YEAR_COUNT);

	for (i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; i++) {
	    shift_history(faculty_gender_history[i], HISTORY_YEAR_COUNT);
	    shift_history(student_gender_history[i], HISTORY_YEAR_COUNT);
	}

	calc_faculty_history();
	calc_student_history();

	//------//

	if( info.prerun_year==1 )                   // only do this once during initialization.
	    calc_research_norm();

	//------//
	shift_history(p_academic_standing_history, HISTORY_YEAR_COUNT);
	shift_history(p_faculty_diversity_history, HISTORY_YEAR_COUNT);
	for (i=0; i<ACADEMIC_STANDING_VAR_COUNT; i++)
	    shift_history(var_academic_standing_history[i], HISTORY_YEAR_COUNT);

	calc_academic_standing();
	calc_faculty_diversity();

	p_academic_standing_history[THIS_YEAR] = p_academic_standing;
	p_faculty_diversity_history[THIS_YEAR] = p_faculty_diversity;

	break;

    case UPDATE_ALL:
	update_history(UPDATE_MONTH);
	update_history(UPDATE_TRIMESTER);
	update_history(UPDATE_YEAR);

	// 0223; fix for interface bug
	for (i=THIS_MONTH-1; i>=THIS_MONTH-2; i--) {
	    research_total_history[i] = research_total_history[THIS_MONTH];
	    research_direct_history[i] = research_direct_history[THIS_MONTH];
	    regular_faculty_with_research[i] = regular_faculty_with_research[THIS_MONTH];
	}
	break;
    default:
	err_here();
	break;
    }
}

//---------- End of function Department::update_history -----------//
//##### end fred 0824 #####//

// ##### Begin MArco ##### //
//---------- Begin of functio Department::save_initial_data -------//
void Department::save_initial_data() {
    initial_p_faculty_morale = p_faculty_morale;
    initial_p_faculty_diversity = p_faculty_diversity;
    initial_p_faculty_research = p_faculty_research;
    initial_p_student_morale = p_student_morale;
    initial_p_academic_standing = p_academic_standing;
    initial_p_educational_quality = p_educational_quality;;
    initial_pressure_to_change_teaching_load = pressure_to_change_teaching_load;

    for (int i = 0 ; i < COURSE_PREFERENCE_COUNT; i++)
	initial_course_preference[i] = course_preference[i];

    for (int i = 0 ; i < MAX_STUDENT_LEVEL_N_TOTAL; i++) {
	//		for (int j = 0; j < HISTORY_TRIMESTER_COUNT; j++)
	//			initial_dept_student_level_history[i][j] = dept_student_level_history[i][j];
	//new
	initial_dept_student_level_history[i] = dept_student_level_history[i][HISTORY_TRIMESTER_COUNT - 1];
    }

    for (int i = 0; i < GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; i++) {
	//		for (int j = 0; j < HISTORY_TRIMESTER_COUNT; j++)
	//			initial_dept_gender_student_level_history[i][j] = dept_gender_student_level_history[i][j];
	//NEW
	initial_dept_gender_student_level_history[i] = dept_gender_student_level_history[i][HISTORY_TRIMESTER_COUNT - 1];
    }

    for (int i = 0; i < FACULTY_RANK_LEVEL_COUNT_N_TOTAL; i++) {
	initial_faculty_level[i] = faculty_level_history[i][HISTORY_YEAR_COUNT-1];
    }

    for (int i = 0; i < GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; i++) {
	initial_faculty_gender[i] = faculty_gender_history[i][HISTORY_YEAR_COUNT-1];
    }

    initial_p_faculty_morale = p_faculty_morale_history[HISTORY_MONTH_COUNT-1];

    initial_research_total = research_total_history[HISTORY_MONTH_COUNT-1];
    initial_research_direct = research_direct_history[HISTORY_MONTH_COUNT-1];
    // 99->99%
    initial_regular_faculty_with_research = regular_faculty_with_research[HISTORY_MONTH_COUNT-1];

    for (int i = 0; i < DISCRETIONARY_TYPE_COUNT; i++)
	initial_priority_discretionary_hour_array[i] = priority_discretionary_hour_array[i];

    for (int i = 0; i < RESEARCH_STATUS_COUNT; i++) {
	initial_research_m[i] = research_m_history[i][HISTORY_MONTH_COUNT-1];
	initial_research_y[i] = research_y_history[i][HISTORY_YEAR_COUNT-1];
    }

    for ( int i=0; i<EDUCATIONAL_QUALITY_VAR_COUNT; i++ ) {
	initial_education_equality[i] = var_educational_quality_history[i][HISTORY_TRIMESTER_COUNT-1];
    }

    for ( int i=0; i<ACADEMIC_STANDING_VAR_COUNT; i++ ) {
	initial_academic_standing[i] = var_academic_standing_history[i][HISTORY_YEAR_COUNT-1];
    }

    student_array.save_initial_data();
    faculty_array.save_initial_data();
    course_array.save_initial_data();
}

//---------- End of functio Department::save_initial_data -------//
// ##### End Marco ##### //
