//Filename    : OSTUD_PI.cpp
//Description : STUDENT Class Definition
//Owner       : Fred

#include <OMATH.H>

#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OENROLL.H>                              // student_count
#include <OSCHLRES.H>
#include <OPSCHOOL.H>                             // subscore[]; player_peer_school
#include <OPEERSCH.H>
#include <OFINANCE.H>
#include <OATHLETI.H>
#include <OLIBTECH.H>
#include <OCHANCE.H>
#include <OSTUDENT.H>

static StudentArray *get_student_array(int deptRecno) {
    if (deptRecno)
	return &(department_array[deptRecno]->student_array);
    else
	return &department_res.general_dept.student_array;
}

//---------- Begin of function Student::update_history -----------//
//!
//! update history arrays
//!
//! update_flag		= UPDATE_TRIMESTER, UPDATE_MONTH, UPDATE_YEAR; (ooffice.h)
//!
void Student::update_history(char update_flag) {
    switch (update_flag) {
    case UPDATE_MONTH:
	calc_satisfaction_student_life();
	calc_satisfaction_athletics();
	calc_satisfaction_overall();
	calc_faculty_workweek();
	break;
    case UPDATE_TRIMESTER:
	calc_satisfaction_academic();

	// called when value is needed
	//calc_probability_change_major();
	//calc_multiplier_on_dropout_prob();
	break;
    case UPDATE_YEAR:
	reaction_summer_course_last = reaction_summer_course;
	break;
    case UPDATE_ALL:
	update_history(UPDATE_MONTH);
	update_history(UPDATE_TRIMESTER);
	update_history(UPDATE_YEAR);
	break;
    default:
	err_here();
	break;
    }
}

//---------- End of function Student::update_history -----------//

//---------- Begin of function Student::calc_satisfaction_academic -----------//
void Student::calc_satisfaction_academic() {
    // for SL 1-3
#define VAR_COUNT_SL1 5
    int i, count;
    float input[VAR_COUNT_SL1];
    memset(input, 0, sizeof(input));

    //--------------//
    // ave educational quality of departments in which the student is taking courses this trimester: CAN WE APPLY THIS FUNCTION SEPARATELY TO EACH DEPARTMENT AND WEIGHT THE RESULT BY THE NUMBER OF COURSES TAKEN IN THE DEPARTMENT BY THE STUDENT SPRITE?

    for (count=0, i=department_array.size(); i>0; i--) {
	if ( department_array.is_deleted(i) )
	    continue;

	if ( course_taking_array[i-1] > 0 ) {
	    input[0] += department_array[i]->p_educational_quality;
	    count++;
	}
    }
    if ( count > 0 )
	input[0] /= count;

    //--------------//
    //	input[1] = float( float(finance.expense_array[AC_LIBRARIES].total) / finance.total_operating_expense.total);

    //## chwg290699
    if ( initializationValueAC_LIBRARIES == 0 )
		initializationValueAC_LIBRARIES=(float)finance.expense_array[AC_LIBRARIES].total;
    input[1] = float(finance.expense_array[AC_LIBRARIES].total)/
	(initializationValueAC_LIBRARIES*(1+math.get_random_snd(0.0f, PSCH_SD(0.05f))));
    //--------------//
    if (department_recno)
	input[2] = (department_array[department_recno]->p_tech_utilization);
    else
	input[2] = library_tech_office.tech_utilization;

    //--------------//
    //course denials this tri
    input[3] = course_denials_this_trimester;

    //--------------//
    //course failures this tri
    input[4] = course_failures_this_trimester;

    //--------------//
    input[0] = math.dual_response_func(1,50,100,29.30f,32.83f,20.19f,76.09f, input[0]);
    //	input[1] = math.single_response_func(1,100, 3.9900f,1.8250f, input[1]);
    // ## chwg290699
    input[1] = math.single_response_func(1,100, 0.5200f,1.030f, input[1]);
    input[2] = math.dual_response_func(0,49.8f,100,40.49f,29.41f,20.04f,85.05f, input[2]);
    input[3] = math.single_response_func(1,100,-1.4230f,0.9125f, input[3]);
    input[4] = math.single_response_func(1,100,-1.1857f,0.7756f, input[4]);

    float newValue = 0;
    float weight[VAR_COUNT_SL1] = {0.40f, 0.10f, 0.1f, 0.20f, 0.20f, };

    for(i=0; i<VAR_COUNT_SL1; i++)
	newValue += weight[i] * input[i];

    //-------------------// 980924
    // for SL 1-3
    if ( student_level <= MASTER ) {
    }
    else if ( student_level == DOCTOR ) {
	newValue = 0.5f * newValue + 0.5f * (department_recno ? department_array[department_recno]->p_academic_standing_no_latency : 0);
    }
    else if ( student_level == DISTANCE_LEARN ) {
	newValue = 0;
	float weightSL5[VAR_COUNT_SL1] = { 0.30f, 0.30f, 0, 0.2f, 0.20f, };

	for(i=0; i<VAR_COUNT_SL1; i++)
	    newValue += weightSL5[i] * input[i];
    }
    else
	err_here();

    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    satisfaction_academic = (char) player_school.latency_func( 0.60f, satisfaction_academic, newValue);

    //---------- extra effect brought the chance cards ------------//

    if( extra_effect_satisfaction_academic ) {
	satisfaction_academic *= extra_effect_satisfaction_academic;

	extra_effect_satisfaction_academic += (1.0f - extra_effect_satisfaction_academic) * 0.3f;

	satisfaction_academic = max(0, min(100,satisfaction_academic ) );
    }
}

//---------- End of function Student::calc_satisfaction_academic -----------//

//---------- Begin of function Student::calc_satisfaction_student_life -----------//
void Student::calc_satisfaction_student_life() {
    if ( student_level != UG_TRADITION ) {
	satisfaction_student_life = -1;               // -1: invalid for student level
	return;
    }

    int const SL_VAR_COUNT = 4;
    int i;
    float input[SL_VAR_COUNT];
    memset(input, 0, sizeof(input));

    //--------------//
    if ( 0 < enroll_res.student_count[UG_TRADITION] + enroll_res.student_count[UG_NONTRADITION] )
	input[0] = float(finance.expense_array[AC_STUDENT_LIFE].total) / (enroll_res.student_count[UG_TRADITION] + enroll_res.student_count[UG_NONTRADITION] );

    //--------------//
    input[1] = get_student_array(department_recno)->ave_satisfaction_student_life[student_level];

    //--------------//
    //	input[2] = float(finance.expense_array[AC_ATHLETICS].total) / float(finance.total_operating_expense.total);
    //## chwg290699
    if ( initializationValueAC_ATHLETICS == 0 )
		initializationValueAC_ATHLETICS=(float)finance.expense_array[AC_ATHLETICS].total;
    input[2] = float(finance.expense_array[AC_ATHLETICS].total)/
	(initializationValueAC_ATHLETICS*(1+math.get_random_snd(0.0f, PSCH_SD(0.05f))));

    //--------------//
    input[3] = talent_extracurricular;

    //--------------//
    input[0] = math.single_response_func(1,100,9.885f,5.475f, input[0]);
    // ## chwg290699
    input[2] = math.single_response_func(1,100,0.5200f,1.030f, input[2]);
    //	input[2] = math.single_response_func(1,100,5.884f,2.738f, input[2]);

    float newValue = 0;
    float weight[SL_VAR_COUNT] = {0.40f, 0.20f, 0.15f, 0.25f, };

    for(i=0; i<SL_VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    //--- if a rape event happen in the last 365 days ----//

    if( chance_event.event_last_happen_date[NEWS_RAPE_IN_CAMPUS-1] > info.game_date-365 ) {
	//--- increase the latency, slow down the recovery ----//

	satisfaction_student_life = (char) player_school.latency_func( 0.10f, satisfaction_student_life, newValue);
    }
    else
	//## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
	satisfaction_student_life = (char) player_school.latency_func( 0.90f, satisfaction_student_life, newValue);

    //--------------//

    satisfaction_student_life = char(satisfaction_student_life * finance.cost_rise_policy_array[PL_STUDENT_LIFE].penalty_multiplier1);

    satisfaction_student_life = max(0,min(100,satisfaction_student_life));
}

//---------- End of function Student::calc_satisfaction_student_life -----------//

//---------- Begin of function Student::calc_satisfaction_athletics -----------//
void Student::calc_satisfaction_athletics() {
    if ( student_level != UG_TRADITION ) {
	satisfaction_athletics = -1;                  // -1: invalid for student level
	return;
    }

    float input[2];
    memset(input, 0, sizeof(input));

    //--------------//
    input[0] = athletics_office.avg_percent_win;

    //--------------//
    input[1] = float(talent_athletics);

    //--------------//
    input[0] = math.dual_response_func(1,50,100,34.12f,27.83f,24.44f,79.95f, input[0]);

    float newValue = 0.5f * input[0] + 0.5f * input[1];

    //	satisfaction_athletics = (char) math.latency_func( 0.97f, satisfaction_athletics, newValue);
    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    satisfaction_athletics = (char) player_school.latency_func( 0.85f, satisfaction_athletics, newValue);

    //--------------//
    satisfaction_athletics = char(satisfaction_athletics * finance.cost_rise_policy_array[PL_ATHLETICS].penalty_multiplier1);

    satisfaction_athletics = max(0,min(100,satisfaction_athletics));
}

//---------- End of function Student::calc_satisfaction_athletics -----------//

//---------- Begin of function Student::calc_satisfaction_overall -----------//
void Student::calc_satisfaction_overall() {
    int i;

    if ( student_level != UG_TRADITION ) {
	const int VAR_COUNT = 2;

	float input[2];

	//--------------//
	input[0] = satisfaction_academic;

	//--------------//
	// student diversity index (see Departmental Performance Indicators for definition): for the student level

	input[1] = 0;                                 //askbill

	//--------------//
	float newValue = 0;
	float weight[VAR_COUNT] = {0.40f, 0.05f,};
	float weightMinority[VAR_COUNT] = {0.45f, 0.10f,};

	if ( gender_ethnic_group == NONMINORITY_MALE ) {
	    for(i=0; i<VAR_COUNT; i++)
		newValue += weight[i] * input[i];
	}
	else {
	    for(i=0; i<VAR_COUNT; i++)
		newValue += weightMinority[i] * input[i];
	}

	satisfaction_overall = (char) player_school.latency_func( 0.95f, satisfaction_overall, newValue);

	//--------------//
	return;
    }

    const int OVERALL_VAR_COUNT = 6;
    float input[OVERALL_VAR_COUNT];
    memset(input, 0, sizeof(input));

    //--------------//
    input[0] = satisfaction_academic;
    input[1] = satisfaction_student_life;
    input[2] = satisfaction_athletics;

    //--------------//
    // student diversity index (see Departmental Performance Indicators for definition): SL-1
    input[3] = player_school.sub_score[S_STUDENT_DIVERSITY][THIS_MONTH];

    //--------------//
    // Fac. DIVERSITY index (see Departmental Performance Indicators for definition): whole institution
    input[4] = department_recno ? department_array[department_recno]->p_faculty_diversity :0;

    //--------------//
    //Let x[t] = the value of the emphasis variable (0,1,2, or 3) at month t;
    //y[t]=the value of the reaction. Then:

    //char	emphasis_var, emphasis_var_last;		// changed monthly // to put it to "student Union"?
    //emphasis_var_last = emphasis_var = 1;

    const float lamda = 0.975f;

    reaction_summer_course = (char) max(0.0f,
					(1-lamda)*(-10)*(player_school.summer_emphasis*player_school.summer_emphasis - player_school.summer_emphasis_last*player_school.summer_emphasis_last)
					+ lamda*reaction_summer_course_last);         // min & max bug chea

    input[5] = (float) reaction_summer_course;

    //--------------//
    input[3] = math.dual_response_func(1,49.48f,100,20.74f,29.48f,20.30f,75.12f, input[3]);
    input[4] = math.dual_response_func(1,49.48f,100,20.74f,29.48f,20.30f,75.12f, input[4]);

    float newValue = 0;

    float weight[OVERALL_VAR_COUNT] = {0.40f, 0.35f, 0.10f, 0.05f, 0.05f, 0.05f, };
    float weightMinority[OVERALL_VAR_COUNT] = {0.45f, 0.25f, 0.10f, 0.10f, 0.10f, 0.05f,};

    if ( gender_ethnic_group == NONMINORITY_MALE ) {
	for(i=0; i<OVERALL_VAR_COUNT; i++)
	    newValue += weight[i] * input[i];
    }
    else {
	for(i=0; i<OVERALL_VAR_COUNT; i++)
	    newValue += weightMinority[i] * input[i];
    }

    //	satisfaction_overall = (char) math.latency_func( 0.95f, satisfaction_overall, newValue);
    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail) //## chea 280999 find bug so back to org.
    satisfaction_overall = (char) player_school.latency_func( 0.67f, satisfaction_overall, newValue);

    //---------------//
    satisfaction_overall = char(satisfaction_overall * finance.revenue_policy_array[PL_TUITION_RATE_GROWTH].penalty_multiplier1);
    satisfaction_overall = char(satisfaction_overall * finance.revenue_policy_array[PL_FINANCIAL_AID].penalty_multiplier1);

    //----//
    satisfaction_overall = char(satisfaction_overall * finance.cost_rise_policy_array[PL_LIBRARIES].penalty_multiplier2);

    //----//

    satisfaction_overall = max(0,min(100,satisfaction_overall));
}

//---------- End of function Student::calc_satisfaction_overall -----------//

//---------- Begin of function Student::calc_faculty_workweek -----------//
void Student::calc_faculty_workweek() {
    float input;

    //--------------//
    input = department_recno ? department_array[department_recno]->p_faculty_morale_history[THIS_MONTH] : 0;

    //--------------//
    input = math.dual_response_func(30,44,60,20.18f,24.18f,19.88f,83.89f, input);

    //	faculty_workweek = (char) math.latency_func( 0.89f, faculty_workweek, input);
    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    faculty_workweek = (char) player_school.latency_func( 0.84f, faculty_workweek, input);

}

//---------- End of function Student::calc_faculty_workweek -----------//

//---------- Begin of function Student::calc_probability_change_major -----------//
void Student::calc_probability_change_major() {
    const char M_VAR_COUNT = 2;
    int i;
    float input[M_VAR_COUNT];
    memset(input, 0, sizeof(input));
    //--------------//
    input[0] = (float) satisfaction_overall;

    //--------------//
    int requiredCourseCount;

    switch( student_level ) {
    case UG_TRADITION:
    case UG_NONTRADITION:
	requiredCourseCount = COURSE_COUNT_REQUIRED_FOR_BACHELOR;
	break;

    case MASTER:
	requiredCourseCount = COURSE_COUNT_REQUIRED_FOR_MASTER;
	break;

    case DOCTOR:
	requiredCourseCount = COURSE_COUNT_REQUIRED_FOR_DOCTOR;
	break;

    case DISTANCE_LEARN:
	return;
    }

    // course remained to take
    input[1] = float(requiredCourseCount - total_course_all);

    //--------------//
    input[0] = math.dual_response_func(0.000f,0.189f,0.500f,-28.960f,-29.670f,80.010f,24.990f, input[0]);
    input[1] = math.single_response_func(0.00f,0.50f,25.00f,12.54f, input[1]);

    float weight[M_VAR_COUNT] = {0.6f, 0.4f };

    float newValue = 0;
    for(i=0; i<M_VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    probability_change_major = player_school.latency_func( 0.3f, probability_change_major, newValue);
}

//---------- End of function Student::calc_probability_change_major -----------//

//---------- Begin of function Student::calc_multiplier_on_dropout_prob -----------//
void Student::calc_multiplier_on_dropout_prob() {
    const char D_VAR_COUNT = 3;
    int i;
    float input[D_VAR_COUNT];
    memset(input, 0, sizeof(input));

    //--------------//
    input[0] = (float) satisfaction_overall;

    //--------------//
    // failed_course_count = accumulated count
    input[1] = float(failed_course_count) / ( 8 * year_in_program);

    //--------------//
    input[2] = player_school.sub_score[S_STUDENT_DIVERSITY][THIS_MONTH];

    //--------------//
    input[0] = math.dual_response_func(0.500f,0.950f,2.000f,-31.945f,-38.384f,79.997f,39.993f, input[0]);
    //	input[1] = math.single_response_func(0.500f,2.000f,0.096f,0.056f, input[1]);
    //## chea 151099 want to change to this but I am lost
    input[1] = math.single_response_func(0.500f,1.000f,0.096f,0.056f, input[1]);
    input[2] = math.dual_response_func(0.500f,1.001f,2.000f,-29.947f,-29.945f,75.015f,19.970f, input[2]);

    float weight[D_VAR_COUNT] = {0.50f, 0.50f, };
    float weightMinority[D_VAR_COUNT] = {0.30f, 0.50f, 0.2f, };
    float newValue = 0;

    if ( gender_ethnic_group == NONMINORITY_MALE ) {
	for(i=0; i<D_VAR_COUNT; i++)
	    newValue += weight[i] * input[i];
    }
    else {
	for(i=0; i<D_VAR_COUNT; i++)
	    newValue += weightMinority[i] * input[i];
    }

    multiplier_on_dropout_prob = player_school.latency_func( 0.67f, multiplier_on_dropout_prob, newValue);
}

//---------- End of function Student::calc_multiplier_on_dropout_prob -----------//

//===============================================================================//
//===============================================================================//

//---------- Begin of function StudentArray::update_history -----------//
void StudentArray::update_history(char update_flag) {

    //----- for calc performance indicator -----//
    // called before next day?

    // should this be put in player school
    Student *studentPtr;
    int i, sl;
    int studentCount[MAX_STUDENT_LEVEL];

    memset(studentCount, 0, sizeof(studentCount));
    memset(ave_satisfaction_student_life, 0, sizeof(ave_satisfaction_student_life));

    for( i=size() ; i>0 ; i-- ) {
	if( is_deleted(i) )
	    continue;

	studentPtr = operator[](i);

	sl = studentPtr->student_level;

	err_when(sl<0 || sl>=MAX_STUDENT_LEVEL);

	if ( sl != UG_TRADITION )
	    continue;

	studentCount[sl]++;
	ave_satisfaction_student_life[sl] += studentPtr->satisfaction_student_life;
    }

    for (i=0; i<MAX_STUDENT_LEVEL; i++) {
	if ( studentCount[i] )
	    ave_satisfaction_student_life[i] /= studentCount[i];
	else
	    err_when( ave_satisfaction_student_life[i] != 0 );
    }

    //----------------------------------------//
    PeerSchool *player;

    switch (update_flag) {
    case UPDATE_MONTH:
	calc_student_performance();
	break;
    case UPDATE_TRIMESTER:
	break;
    case UPDATE_YEAR:
	calc_degree_awarded();
	break;
    case UPDATE_ALL:
	// UPDATE_ALL: assumed called only once for each battle
	// update degree info for last year(97-98)

	player = school_res.player_peer_school;

	// since there's a bug in course selection, too few degrees awarded during game, hence the graphs are not "good" looking
	cur_bachelor_degree = player->total_bach_degrees / department_array.department_count;
	cur_master_degree   = player->total_masters_and_prof_degree / department_array.department_count;
	cur_doctor_degree   = player->total_doctoral_degrees / department_array.department_count;
	/*
          {
          // 990416

          char y;
          int stuCount[MAX_STUDENT_LEVEL][MAX_GRADUATE_YEARS];
          memset(stuCount, 0, sizeof(stuCount));

          for (i=size(); i>0; i--)
          {
	  if ( is_deleted(i) )
	  continue;

	  Student* stuPtr = operator[](i);
	  y = min(MAX_GRADUATE_YEARS, stuPtr->year_in_program);
	  stuCount[stuPtr->student_level][y]++;
	  }

	  //--//

	  cur_bachelor_degree = 0;
	  cur_master_degree = 0;
	  cur_doctor_degree = 0;

	  for (char sl=0; sl<MASTER; sl++)
	  for (i=0; i<MAX_GRADUATE_YEARS; i++)
	  cur_bachelor_degree += int(player_school.grad_trans_prob[sl][i] * stuCount[sl][i]);

	  sl = MASTER;
	  for (i=0; i<MAX_GRADUATE_YEARS; i++)
	  cur_master_degree += int(player_school.grad_trans_prob[sl][i] * stuCount[sl][i]);

	  sl = DOCTOR;
	  DepartmentInfo* deptInfo = department_res[department_array[department_recno]->department_id];
	  for (i=0; i<MAX_GRADUATE_YEARS; i++)
	  cur_doctor_degree += int(deptInfo->doctor_graduate_trans_prob[i] * stuCount[sl][i]);

	  sl = DISTANCE_LEARN;
	  for (i=0; i<MAX_GRADUATE_YEARS; i++)
	  cur_bachelor_degree += int(player_school.grad_trans_prob[UG_NONTRADITION][i] * stuCount[sl][i]);

	  }
	*/

	update_history(UPDATE_MONTH);
	update_history(UPDATE_TRIMESTER);
	update_history(UPDATE_YEAR);

	//-----------//

	for (i=0; i<DEGREE_TYPE_COUNT; i++) {       // see 0919 noets Q6.

	    // from db
	    time_to_degree[BACHELOR_DEGREE][THIS_YEAR] = 4.37f;
	    // from db
	    time_to_degree[MASTER_DEGREE][THIS_YEAR] = 1.05f;
	    //## chea 281099 student no.
	    time_to_degree[DOCTOR_DEGREE][THIS_YEAR] = player->doc_time_to_degree;

	    /*
	      dropout_rate[BACHELOR_DEGREE][THIS_YEAR] = 100-100*(player->target_grad_rate_sl[UG_TRADITION] / 100.0f
	      + ( 1-player->target_grad_rate_sl[UG_TRADITION] / 100.0f )
	      * ( 1-0.9f ));		// 0.9f: from db

	      dropout_rate[MASTER_DEGREE][THIS_YEAR] = 100 - player->target_grad_rate_sl[MASTER];
	      dropout_rate[DOCTOR_DEGREE][THIS_YEAR] = 100 - player->target_grad_rate_sl[DOCTOR];
	      BUGHERE 990408 */
	}

	break;

    default:
	err_here();
	break;
    }
}

//---------- End of function StudentArray::update_history -----------//

//---------- Begin of function StudentArray::calc_student_performance -----------//
//!
void StudentArray::calc_student_performance() {
    shift_history(talent_academic, HISTORY_MONTH_COUNT);
    shift_history(talent_extracurricular, HISTORY_MONTH_COUNT);
    shift_history(talent_athletics, HISTORY_MONTH_COUNT);

    shift_history(satisfaction_academic, HISTORY_MONTH_COUNT);
    shift_history(satisfaction_student_life, HISTORY_MONTH_COUNT);
    shift_history(satisfaction_athletics, HISTORY_MONTH_COUNT);

    shift_history(satisfaction_overall, HISTORY_MONTH_COUNT);

    for (int d=0; d<DEGREE_TYPE_COUNT; d++)
	shift_history(talent_academic_all[d], HISTORY_MONTH_COUNT);

    shift_history(performance_academic_ug, HISTORY_MONTH_COUNT);

    //-----------------//

    Student *stuPtr;
    const char  AVG_VAR_COUNT = 11;
    int total[AVG_VAR_COUNT];
    int i, ugtStuCount=0, ugAllCount=0, stuCount = 0, docCount=0, masCount=0;

    memset(total, 0, sizeof(total));

    for( i=size() ; i>0 ; i-- ) {
	if( is_deleted(i) )
	    continue;

	stuPtr = operator[](i);

	if ( stuPtr->student_level == UG_TRADITION ) {
	    total[0] += stuPtr->talent_academic;
	    total[1] += stuPtr->talent_extracurricular;
	    total[2] += stuPtr->talent_athletics;

	    total[3] += stuPtr->satisfaction_academic;
	    total[4] += stuPtr->satisfaction_student_life;
	    total[5] += stuPtr->satisfaction_athletics;
	    total[6] += stuPtr->satisfaction_overall;
	    ugtStuCount++;
	}

	if ( (stuPtr->student_level) == UG_TRADITION ) {
	    total[7] += (int) stuPtr->academic_achievement;
	    total[8] += stuPtr->talent_academic;
	    //ugtCount++;
	}
	else if ( stuPtr->student_level == MASTER ) {
	    total[9] += stuPtr->talent_academic;
	    masCount++;
	}
	else if ( stuPtr->student_level == DOCTOR ) {
	    total[10] += stuPtr->talent_academic;
	    docCount++;
	}

	stuCount++;
    }

    if (!stuCount)
	return;

    if ( ugtStuCount ) {
	//## 071299 chea 1.12.1
	talent_academic[THIS_MONTH] = char(total[0] / ugtStuCount);
	talent_extracurricular [THIS_MONTH] = char(total[1] / ugtStuCount);
	talent_athletics [THIS_MONTH] = char(total[2] / ugtStuCount);

	satisfaction_academic [THIS_MONTH] = char(total[3] / ugtStuCount);
	satisfaction_student_life [THIS_MONTH] = char(total[4] / ugtStuCount);
	satisfaction_athletics [THIS_MONTH] = char(total[5] / ugtStuCount);
	satisfaction_overall [THIS_MONTH] = char(total[6] / ugtStuCount);

	//		talent_academic[THIS_MONTH] = char(total[0] / stuCount);
	//		talent_extracurricular [THIS_MONTH] = char(total[1] / stuCount);
	//		talent_athletics [THIS_MONTH] = char(total[2] / stuCount);

	//		satisfaction_academic [THIS_MONTH] = char(total[3] / stuCount);
	//		satisfaction_student_life [THIS_MONTH] = char(total[4] / stuCount);
	//		satisfaction_athletics [THIS_MONTH] = char(total[5] / stuCount);
	//		satisfaction_overall [THIS_MONTH] = char(total[6] / stuCount);
    }

    if ( ugtStuCount ) {
	performance_academic_ug[THIS_MONTH] = char(total[7] / ugtStuCount);
	talent_academic_all[BACHELOR_DEGREE][THIS_MONTH] = char(total[8] / ugtStuCount);
    }

    if ( masCount )
	talent_academic_all[MASTER_DEGREE][THIS_MONTH] = char(total[9] / masCount);

    if ( docCount )
	talent_academic_all[DOCTOR_DEGREE][THIS_MONTH] = char(total[10] / docCount);

    /*
      talent_academic
      talent_extracurricular
      talent_athletics
      satisfaction_academic
      satisfaction_student_life
      satisfaction_athletics
    */

}

//---------- End of function StudentArray::calc_student_performance -----------//

//---------- Begin of function StudentArray::calc_degree_awarded -----------//
void StudentArray::calc_degree_awarded() {
    //-----------------------------------------//

    shift_history(bachelor_degree, HISTORY_YEAR_COUNT);
    shift_history(non_ug_bachelor_degree, HISTORY_YEAR_COUNT );
    shift_history(master_degree, HISTORY_YEAR_COUNT);
    shift_history(doctor_degree, HISTORY_YEAR_COUNT);

    for (int y=0; y<DEGREE_TYPE_COUNT; y++) {
	shift_history(time_to_degree[y], HISTORY_YEAR_COUNT);
	shift_history(fake_disp_time_to_degree[y], HISTORY_YEAR_COUNT);
	shift_history(dropout_rate[y], HISTORY_YEAR_COUNT);
    }

    //-----------------------------------------//
    //
    // The [THIS_YEAR] element in all historical array
    // is always the latest obtainable value of the
    // variables. Some will have values for the
    // current year, some will have values for the
    // last year. So THIS_YEAR does not mean the
    // current year.
    //
    // As for the graduation rate, the value is
    // graduation rate of the last year as the value
    // for the current year hasn't been known yet.
    //
    //-----------------------------------------//

    bachelor_degree[THIS_YEAR] = cur_bachelor_degree;
    non_ug_bachelor_degree[THIS_YEAR] = cur_non_ug_bachelor_degree;
    master_degree [THIS_YEAR]  = cur_master_degree;
    doctor_degree [THIS_YEAR]  = cur_doctor_degree;

    //---- special handling: the first year, the master & doctor degrees are too low, artificially increase them ---//
    if( info.prerun_year && info.game_year == 1 )
	{
	    //--- count the number of master & doctor students in this department ---//

	    int masterCount=0, doctorCount=0;

	    for( int i=size() ; i>0 ; i-- ) {
		if( is_deleted(i) )
		    continue;

		Student* stuPtr = operator[](i);

		if( stuPtr->student_level == MASTER )
		    masterCount++;
		else if( stuPtr->student_level == DOCTOR )
		    doctorCount++;
	    }

	    // divided by the average # of years to get a degree
	    master_degree[THIS_YEAR]  = int( (float) masterCount / 1.6f);
	    doctor_degree [THIS_YEAR] = doctorCount/5;
	}

    //---- see design note 0919 section 6 -----//

    time_to_degree[BACHELOR_DEGREE][THIS_YEAR] = math.safe_divide(time_to_degree_cumm[BACHELOR_DEGREE], cur_bachelor_degree);
    time_to_degree[MASTER_DEGREE][THIS_YEAR] = math.safe_divide(time_to_degree_cumm[MASTER_DEGREE], cur_master_degree);
    time_to_degree[DOCTOR_DEGREE][THIS_YEAR] = math.safe_divide(time_to_degree_cumm[DOCTOR_DEGREE], cur_doctor_degree);

    //--- special case handling: smooth the graph for bachelor Time to Degree ---//

    fake_disp_time_to_degree[BACHELOR_DEGREE][THIS_YEAR] = m.fmax( time_to_degree[BACHELOR_DEGREE][THIS_YEAR], math.get_random_snd(4.3f, 0.07f) );

    //-----------------------------------------//
    int curSize[DEGREE_TYPE_COUNT];                 // new student count this year
    const char degMap[MAX_STUDENT_LEVEL] = { BACHELOR_DEGREE, -1, MASTER_DEGREE, DOCTOR_DEGREE, -1};

    memset(curSize,0,sizeof(curSize));

    for (int i=size(); i>0; i--) {
	if ( is_deleted(i) )
	    continue;

	Student* stuPtr = operator[](i);

	// assume called after next_trimester and then enrollment
	if ( stuPtr->year_in_program == 1 ) {
	    char deg = degMap[stuPtr->student_level];

	    if (deg >= 0)
		curSize[deg]++;
	}
    }

    const float lamda[] = { 0.775f, 0.1f, 0.8f };

    for (int y=0; y<DEGREE_TYPE_COUNT; y++) {
	//990408 ave_entering_class[y] = math.latency_func(lamda[y], (float) curSize[y], ave_entering_class[y]);

	//990408 dropout_rate[y][THIS_YEAR] = math.safe_divide((float)cur_dropout[y], ave_entering_class[y]);

	dropout_rate[y][THIS_YEAR] = (float) cur_dropout[y];
    }

    //-----------------------------------------//

    cur_bachelor_degree = 0;                        // reset it, it will be incremented as students graduate
    cur_non_ug_bachelor_degree = 0;
    cur_master_degree   = 0;
    cur_doctor_degree   = 0;

    for (int y=0; y<DEGREE_TYPE_COUNT; y++) {
	time_to_degree_cumm[y] = 0;
	cur_dropout[y] = 0;
	last_year_dropout[y]=0;
	last_year_dropout[y+1]=0;
	last_year_degree[y]=0;
	last_year_degree[y+1]=0;
    }

    ave_time_to_graduation_for_ug = 0;
}

//---------- End of function Student::calc_degree_awarded -----------//

/*
//---------- Begin of function Student::calc_faculty_workweek -----------//
void Template::template()
{
  const char M_VAR_COUNT=6;
  int i;
  float input[M_VAR_COUNT];
  memset(input, 0, sizeof(input));
  //--------------//
  //--------------//
  //--------------//
//--------------//

= math.dual_response_func(, input);
= math.single_response_func(, input);

float weight[M_VAR_COUNT] = {0.40f, 0.20f, 0.15f, 0.25f, };

float newValue = 0;
for(i=0; i<M_VAR_COUNT; i++)
newValue += weight[i] * input[i];

= math.latency_func( 0.f, , newValue);
}
//---------- End of function Student::calc_faculty_workweek -----------//
*/

// ##### Begin MArco #### //
// -------- Begin of function StudentArray::save_initial_data ------ //
void StudentArray::save_initial_data() {
    for (int i = 0; i < DEGREE_TYPE_COUNT; i++) {
	for (int j = 0; j < HISTORY_MONTH_COUNT; j++)
	    initial_talent_academic_all[i][j] = talent_academic_all[i][j];

	for (int j = 0; j < HISTORY_YEAR_COUNT; j++) {
	    initial_fake_disp_time_to_degree[i][j] = fake_disp_time_to_degree[i][j];
	    initial_time_to_degree[i][j] = time_to_degree[i][j];
	    initial_dropout_rate[i][j] = dropout_rate[i][j];
	}
    }

    for (int i = 0; i < HISTORY_MONTH_COUNT; i++) {
	initial_performance_academic_ug[i] = performance_academic_ug[i];
	//		initial_talent_academic_all[i] = talent_academic_all[BACHELOR_DEGREE][i];		//NEW
    }

    for (int i = 0; i < HISTORY_YEAR_COUNT; i++) {
	//		initial_fake_disp_time_to_degree[j] = fake_disp_time_to_degree[BACHELOR_DEGREE][j];		//NEW

	initial_bachelor_degree[i] = bachelor_degree[i]+non_ug_bachelor_degree[i];
	initial_master_degree[i] = master_degree[i];
	initial_doctor_degree[i] = doctor_degree[i];
    }

}

// -------- End of function StudentArray::save_initial_data ------ //
// ##### End Marco ##### //
