//Filename    : ODEPT_P.CPP
//Description : Functions for calculating departmental performance indicators
//Owner       : Fred

#include <OSYS.H>

#include <ODEPT.H>
#include <OMATH.H>
#include <OFINANCE.H>
#include <ODEPTRES.H>
#include <OSCHLRES.H>
#include <OPSCHOOL.H>

#ifdef DEBUG
#include <STDIO.H>
#include <OLOG.H>
#endif

//------ Begin of function sort_function_faculty_research ------//
//!
//! Sort department_array by their distances in an ascending order.
//!
static int sort_function_faculty_research( const void *a, const void *b ) {
    float valA = department_array[(*((short*)a))]->p_faculty_research_history[THIS_MONTH];
    float valB = department_array[(*((short*)b))]->p_faculty_research_history[THIS_MONTH];

    float rc = valA-valB;

    if( rc > 0 )
	return -1;
    else if( rc < 0 )
	return 1;
    else
	return 0;
}

//------- End of function sort_function_faculty_research ------//

//------ Begin of function sort_function_faculty_morale ------//
//!
//! Sort department_array by their distances in an ascending order.
//!
static int sort_function_faculty_morale( const void *a, const void *b ) {
    float valA = department_array[(*((short*)a))]->p_faculty_morale_history[THIS_MONTH];
    float valB = department_array[(*((short*)b))]->p_faculty_morale_history[THIS_MONTH];

    float rc = valA-valB;

    if( rc > 0 )
	return -1;
    else if( rc < 0 )
	return 1;
    else
	return 0;
}

//------- End of function sort_function_faculty_morale ------//

//------ Begin of function sort_function_educational_quality ------//
//!
//! Sort department_array by their distances in an ascending order.
//!
static int sort_function_educational_quality( const void *a, const void *b ) {
    float valA = department_array[(*((short*)a))]->p_educational_quality_history[THIS_TRIMESTER];
    float valB = department_array[(*((short*)b))]->p_educational_quality_history[THIS_TRIMESTER];

    float rc = valA-valB;

    if( rc > 0 )
	return -1;
    else if( rc < 0 )
	return 1;
    else
	return 0;
}

//------- End of function sort_function_educational_quality ------//

//------ Begin of function sort_function_student_morale ------//
//!
//! Sort department_array by their distances in an ascending order.
//!
static int sort_function_student_morale( const void *a, const void *b ) {
    float valA = department_array[(*((short*)a))]->p_student_morale_history[THIS_TRIMESTER];
    float valB = department_array[(*((short*)b))]->p_student_morale_history[THIS_TRIMESTER];

    float rc = valA-valB;

    if( rc > 0 )
	return -1;
    else if( rc < 0 )
	return 1;
    else
	return 0;
}

//------- End of function sort_function_student_morale ------//

//------ Begin of function sort_function_academic_standing ------//
//!
//! Sort department_array by their distances in an ascending order.
//!
static int sort_function_academic_standing( const void *a, const void *b ) {
    float valA = department_array[(*((short*)a))]->p_academic_standing_history[THIS_YEAR];
    float valB = department_array[(*((short*)b))]->p_academic_standing_history[THIS_YEAR];

    float rc = valA-valB;

    if( rc > 0 )
	return -1;
    else if( rc < 0 )
	return 1;
    else
	return 0;
}

//------- End of function sort_function_academic_standing ------//

//------ Begin of function sort_function_faculty_diversity ------//
//!
//! Sort department_array by their distances in an ascending order.
//!
static int sort_function_faculty_diversity( const void *a, const void *b ) {
    float valA = department_array[(*((short*)a))]->p_faculty_diversity_history[THIS_YEAR];
    float valB = department_array[(*((short*)b))]->p_faculty_diversity_history[THIS_YEAR];

    float rc = valA-valB;

    if( rc > 0 )
	return -1;
    else if( rc < 0 )
	return 1;
    else
	return 0;
}

//------- End of function sort_function_faculty_diversity ------//

//---------- Begin of function DepartmentArray::update_history -----------//
//!
void DepartmentArray::update_history(char update_flag) {
    int i,j;
    int totalFacCount = 0;
    int totalStuCount = 0;
    int totalRegularFacCount = 0;
    Department* deptPtr;

    switch (update_flag) {
    case UPDATE_MONTH:

	shift_history(p_faculty_research_history, HISTORY_MONTH_COUNT);
	shift_history(p_faculty_morale_history, HISTORY_MONTH_COUNT);
	p_faculty_research_history[THIS_MONTH] = 0;
	p_faculty_morale_history[THIS_MONTH] = 0;

	totalFacCount = 0;

	for ( i=size(); i>0; i-- ) {
	    if ( is_deleted(i) )
		continue;

	    deptPtr = department_array[i];

	    int facCount = deptPtr->faculty_array.faculty_count;

	    p_faculty_research_history[THIS_MONTH] += deptPtr->p_faculty_research_history[THIS_MONTH] * facCount;
	    p_faculty_morale_history[THIS_MONTH] += deptPtr->p_faculty_morale_history[THIS_MONTH] * facCount;

	    totalFacCount += facCount;

	    sys.yield();
	}

	err_when(totalFacCount == 0);

	p_faculty_research_history[THIS_MONTH] /= totalFacCount;
	p_faculty_morale_history[THIS_MONTH] /= totalFacCount;

	//------//

	for (i=0; i<RESEARCH_STATUS_COUNT; i++)
	    shift_history(research_m_history[i], HISTORY_MONTH_COUNT);

	shift_history(research_total_history, HISTORY_MONTH_COUNT);
	shift_history(research_direct_history, HISTORY_MONTH_COUNT);
	shift_history(regular_faculty_with_research, HISTORY_MONTH_COUNT);

	calc_research_history();                    // 1217
	break;

    case UPDATE_TRIMESTER:
	calc_course_history();

	shift_history(p_educational_quality_history, HISTORY_TRIMESTER_COUNT);
	shift_history(p_student_morale_history, HISTORY_TRIMESTER_COUNT);

	p_educational_quality_history[THIS_TRIMESTER] = 0;
	p_student_morale_history[THIS_TRIMESTER] = 0;

	totalFacCount = 0;
	totalStuCount = 0;
	totalRegularFacCount = 0;

	for ( i=size(); i>0; i-- ) {
	    if ( is_deleted(i) )
		continue;

	    deptPtr = department_array[i];

	    int facCount = deptPtr->faculty_array.faculty_count;
	    int stuCount = deptPtr->student_array.student_count;

	    p_educational_quality_history[THIS_TRIMESTER] += deptPtr->p_educational_quality_history[THIS_TRIMESTER] * facCount;
	    p_student_morale_history[THIS_TRIMESTER] += deptPtr->p_student_morale_history[THIS_TRIMESTER] * stuCount;

	    totalFacCount += facCount;
	    totalStuCount += stuCount;
	    totalRegularFacCount += deptPtr->regular_faculty_count;

	    // sys.yield();
	}
	sys.yield();

	err_when(totalFacCount == 0);
	err_when(totalStuCount == 0);

	p_educational_quality_history[THIS_TRIMESTER] /= totalFacCount;
	p_student_morale_history[THIS_TRIMESTER] /= totalStuCount;

	//------//
	shift_history(teaching_contact_hour, HISTORY_TRIMESTER_COUNT);
	for (i=0; i<DISCRETIONARY_TYPE_COUNT_N_TOTAL; i++)
	    shift_history(discretionary_hour_array[i], HISTORY_TRIMESTER_COUNT);

	//discretionary_hour_array[i][THIS_TRIMESTER];
	//teaching_contact_hour[THIS_TRIMESTER]=0; // chwg0602

	//------//
	for ( i=size(); i>0; i-- ) {
	    if ( is_deleted(i) )
		continue;

	    deptPtr = department_array[i];
	    int facCount = deptPtr->faculty_array.faculty_count;

	    teaching_contact_hour[THIS_TRIMESTER] += deptPtr->faculty_array.teaching_contact_hour[THIS_TRIMESTER] * facCount;

	    // 990419
	    for (char j=0;j<DISCRETIONARY_TYPE_COUNT_N_TOTAL; j++)
		discretionary_hour_array[j][THIS_TRIMESTER] += deptPtr->faculty_array.discretionary_hour_array[j][THIS_TRIMESTER] * facCount;

	    time_shifted_scholarship += deptPtr->faculty_array.time_shifted_scholarship*facCount;;
	}

	teaching_contact_hour[THIS_TRIMESTER] /= totalFacCount;

	for (j=0;j<DISCRETIONARY_TYPE_COUNT_N_TOTAL; j++)
	    discretionary_hour_array[j][THIS_TRIMESTER] /= totalFacCount;

	time_shifted_scholarship /= totalFacCount;

	//----- pi 980929 --------------------//
	shift_history(talent_teaching, HISTORY_TRIMESTER_COUNT);
	shift_history(talent_scholarship, HISTORY_TRIMESTER_COUNT);
	shift_history(talent_research, HISTORY_TRIMESTER_COUNT);
	shift_history(performance_teaching, HISTORY_TRIMESTER_COUNT);
	shift_history(performance_scholarship, HISTORY_TRIMESTER_COUNT);
	shift_history(performance_research, HISTORY_TRIMESTER_COUNT);

	for ( i=size(); i>0; i-- ) {
	    if ( is_deleted(i) )
		continue;

	    deptPtr = department_array[i];
	    int facCount = deptPtr->faculty_array.faculty_count;
	    int regularFacCount = deptPtr->regular_faculty_count;

	    talent_teaching[THIS_TRIMESTER] += deptPtr->faculty_array.talent_teaching[THIS_TRIMESTER] * regularFacCount;
	    talent_scholarship[THIS_TRIMESTER] += deptPtr->faculty_array.talent_scholarship[THIS_TRIMESTER] * regularFacCount;
	    talent_research[THIS_TRIMESTER] += deptPtr->faculty_array.talent_research[THIS_TRIMESTER] * regularFacCount;
	    performance_teaching[THIS_TRIMESTER] += deptPtr->faculty_array.performance_teaching[THIS_TRIMESTER] * regularFacCount;
	    performance_scholarship[THIS_TRIMESTER] += deptPtr->faculty_array.performance_scholarship[THIS_TRIMESTER] * regularFacCount;
	    performance_research[THIS_TRIMESTER] += deptPtr->faculty_array.performance_research[THIS_TRIMESTER] * regularFacCount;

	    // sys.yield();
	}
	sys.yield();

	talent_teaching[THIS_TRIMESTER] = math.safe_divide( talent_teaching[THIS_TRIMESTER], (float)totalRegularFacCount );
	talent_scholarship[THIS_TRIMESTER] = math.safe_divide( talent_scholarship[THIS_TRIMESTER], (float)totalRegularFacCount );
	talent_research[THIS_TRIMESTER] = math.safe_divide( talent_research[THIS_TRIMESTER], (float)totalRegularFacCount );
	performance_teaching[THIS_TRIMESTER] = math.safe_divide( performance_teaching[THIS_TRIMESTER], (float)totalRegularFacCount );
	performance_scholarship[THIS_TRIMESTER] = math.safe_divide( performance_scholarship[THIS_TRIMESTER], (float)totalRegularFacCount );
	performance_research[THIS_TRIMESTER] = math.safe_divide( performance_research[THIS_TRIMESTER], (float)totalRegularFacCount );

	for( j = 0; j < MAX_TEACHING_METHOD_N_TOTAL; ++j ) {
	    shift_history(class_section_count_history[j], HISTORY_TRIMESTER_COUNT);
	    shift_history(convention_student_count_history[j], HISTORY_TRIMESTER_COUNT);
	    shift_history(size_deviation_history[j], HISTORY_TRIMESTER_COUNT);
	    shift_history(admission_denied_count_history[j], HISTORY_TRIMESTER_COUNT);
	    shift_history(failure_count_history[j], HISTORY_TRIMESTER_COUNT);
	    shift_history(outsourcing_credits_history[j], HISTORY_TRIMESTER_COUNT);

	    /*
              for ( i=size(); i>0; i-- )
              {
	      if ( is_deleted(i) )
	      continue;

	      deptPtr = department_array[i];

	      class_section_count_history[j][THIS_TRIMESTER] += deptPtr->course_array.class_section_count_history[j][THIS_TRIMESTER];
	      convention_student_count_history[j][THIS_TRIMESTER] += deptPtr->course_array.convention_student_count_history[j][THIS_TRIMESTER];
	      // BUGHERE : don't use size_deviation_history, it is not additive
	      //size_deviation_history[j][THIS_TRIMESTER] += deptPtr->course_array.size_deviation_history[j][THIS_TRIMESTER];
	      admission_denied_count_history[j][THIS_TRIMESTER] += deptPtr->course_array.admission_denied_count_history[j][THIS_TRIMESTER];
	      // BUGHERE failure_count_history may not be reliable (prior trimester...)
	      failure_count_history[j][THIS_TRIMESTER] += deptPtr->course_array.failure_count_history[j][THIS_TRIMESTER];
	      outsourcing_credits_history[j][THIS_TRIMESTER] += deptPtr->course_array.outsourcing_credits_history[j][THIS_TRIMESTER];
	      }

	      sys.yield();
	    */
	    // fix in version 2 to get from result of the calc_course_history()
	    class_section_count_history[j][THIS_TRIMESTER] = class_section_count[j];
	    convention_student_count_history[j][THIS_TRIMESTER] = convention_student_count[j];
	    size_deviation_history[j][THIS_TRIMESTER] = size_deviation[j];
	    admission_denied_count_history[j][THIS_TRIMESTER] = admission_denied_count[j];
	    failure_count_history[j][THIS_TRIMESTER] = failure_count[j];
	    outsourcing_credits_history[j][THIS_TRIMESTER] = outsourcing_credits[j];
	}

	break;
	//------//

    case UPDATE_YEAR:
	shift_history(p_academic_standing_history, HISTORY_YEAR_COUNT);
	shift_history(p_faculty_diversity_history, HISTORY_YEAR_COUNT);

	p_academic_standing_history[THIS_YEAR] = 0;
	p_faculty_diversity_history[THIS_YEAR] = 0;

	totalFacCount = 0;

	for ( i=size(); i>0; i-- ) {
	    if ( is_deleted(i) )
		continue;

	    deptPtr = department_array[i];

	    int facCount = deptPtr->faculty_array.faculty_count;

	    //			p_academic_standing_history[THIS_YEAR] += deptPtr->p_academic_standing_history[THIS_YEAR] * facCount;
	    p_faculty_diversity_history[THIS_YEAR] += deptPtr->p_faculty_diversity_history[THIS_YEAR] * facCount;

	    totalFacCount += facCount;

	    sys.yield();
	}

	err_when(totalFacCount ==0);
	p_faculty_diversity_history[THIS_YEAR] /= totalFacCount;

	//		p_academic_standing_history[THIS_YEAR] /= totalFacCount;
	p_academic_standing_history[THIS_YEAR] = player_school.sub_score[S_PRESTIGE][THIS_MONTH];

	//------//

	for (i=0;i<FACULTY_RANK_LEVEL_COUNT_N_TOTAL; i++)
	    shift_history(faculty_level_history[i], HISTORY_YEAR_COUNT);

	for (i=0;i<MAX_STUDENT_LEVEL_N_TOTAL; i++)
	    shift_history(student_level_history[i], HISTORY_YEAR_COUNT);

	for (i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; i++) {
	    shift_history(faculty_gender_history[i], HISTORY_YEAR_COUNT);
	    shift_history(student_gender_history[i], HISTORY_YEAR_COUNT);
	}

	calc_faculty_student_history();             // 0406

	shift_history(salary_gender_ethnic_equity_history, HISTORY_YEAR_COUNT);
	shift_history(salary_departmental_equity_history, HISTORY_YEAR_COUNT);
	shift_history(salary_rank_equity_history, HISTORY_YEAR_COUNT);

	calc_salary_equity();

	break;

    case UPDATE_ALL:
	update_history(UPDATE_MONTH);
	update_history(UPDATE_TRIMESTER);
	update_history(UPDATE_YEAR);
	/*
          if ( player_school.scenario_id == SCN_FACULTY_DIVERSITY )
          {
	  totalFacCount = faculty_gender_history[GENDER_ETHNIC_TYPE_COUNT][THIS_YEAR];
	  player_school.scenario_target[0] = int(( faculty_gender_history[MINORITY_MALE][THIS_YEAR] + faculty_gender_history[MINORITY_FEMALE][THIS_YEAR]) * 100 * 1.5 / totalFacCount );		// should be 1.5 in design doc
	  player_school.scenario_target[1] = int(( faculty_gender_history[MINORITY_FEMALE][THIS_YEAR] + faculty_gender_history[NONMINORITY_FEMALE][THIS_YEAR]) * 100 * 1.25 / totalFacCount);

	  //## chea 270799 change to a higher target
	  //player_school.scenario_target[0] = int(( faculty_gender_history[MINORITY_MALE][THIS_YEAR] + faculty_gender_history[MINORITY_FEMALE][THIS_YEAR]) * 100 * 2.0 / totalFacCount );		// should be 1.5 in design doc
	  //player_school.scenario_target[1] = int(( faculty_gender_history[MINORITY_FEMALE][THIS_YEAR] + faculty_gender_history[NONMINORITY_FEMALE][THIS_YEAR]) * 100 * 1.75 / totalFacCount);

	  player_school.scenario_target[0] = min(100, player_school.scenario_target[0] );
	  player_school.scenario_target[1] = min(100, player_school.scenario_target[1] );
	  }
	*/
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

    //------------//

    p_sort_performance();
}

//---------- End of function DepartmentArray::update_history -----------//

//---------- Begin of function DepartmentArray::p_sort_performance -----------//
//!
void DepartmentArray::p_sort_performance() {
    int i, deptCount=0;

#ifdef DEBUG
    for ( i=size(); i>0; i-- ) {
	if ( is_deleted(i) )
	    continue;
	deptCount++;
    }

    err_when(deptCount != department_count);
    err_when(deptCount > MAX_DEPARTMENT);
#endif

    //-------------------//

    for( i=0; i<deptCount; i++ )
	p_educational_quality_sort[i] = i+1;

    qsort( p_educational_quality_sort, deptCount, sizeof(p_educational_quality_sort[0]), sort_function_educational_quality );

    //-------------------//

    for( i=0; i<deptCount; i++ )
	p_academic_standing_sort[i] = i+1;

    qsort( p_academic_standing_sort, deptCount, sizeof(p_academic_standing_sort[0]), sort_function_academic_standing );

    //-------------------//

    for( i=0; i<deptCount; i++ )
	p_student_morale_sort[i] = i+1;

    qsort( p_student_morale_sort, deptCount, sizeof(p_student_morale_sort[0]), sort_function_student_morale );

    //-------------------//

    for( i=0; i<deptCount; i++ )
	p_faculty_research_sort[i] = i+1;

    qsort( p_faculty_research_sort, deptCount, sizeof(p_faculty_research_sort[0]), sort_function_faculty_research );

    //-------------------//

    for( i=0; i<deptCount; i++ )
	p_faculty_morale_sort[i] = i+1;

    qsort( p_faculty_morale_sort, deptCount, sizeof(p_faculty_morale_sort[0]), sort_function_faculty_morale );

    //-------------------//

    for( i=0; i<deptCount; i++ )
	p_faculty_diversity_sort[i] = i+1;

    qsort( p_faculty_diversity_sort, deptCount, sizeof(p_faculty_diversity_sort[0]), sort_function_faculty_diversity );

    sys.yield();

    //-------------------//

#ifdef DEBUG_SORT
    for( i=0; i<deptCount; i++ ) {
	float value = department_array[p_educational_quality_sort[i]]->p_educational_quality_history[THIS_TRIMESTER];
	char s[300];
	sprintf(s, "A. sort %dth: %f", i, value);
	DEBUG_LOG(s);
    }

    for( i=0; i<deptCount; i++ ) {
	float value = department_array[p_faculty_morale_sort[i]]->p_faculty_morale_history[THIS_MONTH];
	char s[300];
	sprintf(s, "B. sort %dth: %f", i, value);
	DEBUG_LOG(s);
    }
#endif
}

//---------- End of function DepartmentArray::p_sort_performance -----------//

//------- Begin of function DepartmentArray::calc_course_history -----//

void DepartmentArray::calc_course_history() {
    int sz = sizeof(class_section_count), t, i;

    //	if (info.game_month == 1 || info.game_month == 9)  //## chea 190899
    //	{
    memset(class_section_count, 0, sz);
    memset(convention_student_count, 0, sz);
    memset(average_class_size, 0, sz);
    memset(normal_class_size, 0, sz);
    memset(convention_student_count, 0, sz);
    memset(size_deviation, 0, sz);
    memset(admission_denied_count, 0, sz);
    memset(failure_count, 0, sz);
    memset(outsourcing_credits, 0, sz);
    //	}

    shift_history(experienced_class_size, HISTORY_TRIMESTER_COUNT);
    //	float total = 0;
    //	int count = 0;

    for (i=size(); i>0; i--) {
	Department* deptPtr = this->operator[](i);

	for(t=0; t<MAX_TEACHING_METHOD; t++) {
	    class_section_count[t] += deptPtr->course_array.class_section_count[t];
	    convention_student_count[t] += deptPtr->course_array.convention_student_count[t];
	    average_class_size[t] += deptPtr->course_array.average_class_size[t];
	    normal_class_size[t] += deptPtr->course_array.normal_class_size[t];
	    // size_deviation[t] += deptPtr->course_array.size_deviation[t];
	    // fix in version 2, size_deviation use average class size and normal for each teaching method of institution
	    admission_denied_count[t] += deptPtr->course_array.admission_denied_count[t];
	    failure_count[t] += deptPtr->course_array.failure_count[t];
	    outsourcing_credits[t] += deptPtr->course_array.outsourcing_credits[t];
	}

	//------//

	for (int c=deptPtr->course_array.size(); c>0; c--) {
	    Course* cPtr = deptPtr->course_array[c];
	    //int tm = cPtr->teaching_method;

	    if ( cPtr->teaching_method != BREAKOUT_LAB ) {
		//				total += math.safe_divide(cPtr->convention_student_count, cPtr->class_section_count);
		//				count++;
	    }
	}
    }

    //experienced_class_size[THIS_TRIMESTER] = (int) math.safe_divide(total, (float) count);

    //if ( experienced_class_size[THIS_TRIMESTER] <= 0 )	//fix bug for summer
    //	experienced_class_size[THIS_TRIMESTER] = experienced_class_size[THIS_TRIMESTER-1];

    //	if (info.game_month == 5)                               //## chea 190899 18.2
    //		convention_student_count[MAX_TEACHING_METHOD]=0;

    for(t=0; t<MAX_TEACHING_METHOD; t++) {
	outsourcing_credits[MAX_TEACHING_METHOD] += outsourcing_credits[t];

	if ( t==DISTANCE_LEARN_COURSE )
	    continue;

	//class_section_count[t] /= department_count;
	//convention_student_count[t] /= department_count;
	//990506 average_class_size[t] /= department_count;
	// 990506
	average_class_size[t] = (int) math.safe_divide(convention_student_count[t], class_section_count[t]);
	normal_class_size[t] /= department_count;

	// size_deviation[t] /= department_count;
	// fix in version 2, size_deviation use (average_class_size-normal_class_size)/normal_class_size
	size_deviation[t] = (int) math.safe_divide(100 * (average_class_size[t]-normal_class_size[t]), normal_class_size[t]);

	//admission_denied_count[t] /= department_count;
	//failure_count[t] /= department_count;

	//0405
	experienced_class_size[THIS_TRIMESTER] += average_class_size[t];

	//----//

	class_section_count[MAX_TEACHING_METHOD] += class_section_count[t];

	convention_student_count[MAX_TEACHING_METHOD] += convention_student_count[t];

	//0405 average_class_size[MAX_TEACHING_METHOD] += average_class_size[t];

	normal_class_size[MAX_TEACHING_METHOD] += normal_class_size[t];
	// fix in version 2 use weighted average (weight is class_section_count[t])
	// size_deviation[MAX_TEACHING_METHOD] += size_deviation[t];
	size_deviation[MAX_TEACHING_METHOD] += size_deviation[t] * class_section_count[t];
	admission_denied_count[MAX_TEACHING_METHOD] += admission_denied_count[t];
	failure_count[MAX_TEACHING_METHOD] += failure_count[t];
    }

    //## chea begin 990525

    // 0405
    average_class_size[MAX_TEACHING_METHOD] = (int) math.safe_divide(convention_student_count[MAX_TEACHING_METHOD], class_section_count[MAX_TEACHING_METHOD]);

    if ( average_class_size[MAX_TEACHING_METHOD] <= 0)
	experienced_class_size[THIS_TRIMESTER] = experienced_class_size[THIS_TRIMESTER-1];
    else
	experienced_class_size[THIS_TRIMESTER] = average_class_size[MAX_TEACHING_METHOD];

    //0405 average_class_size[MAX_TEACHING_METHOD] /= department_count;
    //average_class_size[MAX_TEACHING_METHOD] = (int) math.safe_divide(convention_student_count[MAX_TEACHING_METHOD], class_section_count[MAX_TEACHING_METHOD]);	// 0405

    //## chea end 990525

    normal_class_size[MAX_TEACHING_METHOD] /= department_count;

    // size_deviation[MAX_TEACHING_METHOD] /= department_count;
    // fix in version 2 use weighted average (weight is class_section_count[t])
    if( class_section_count[MAX_TEACHING_METHOD] > 0 )
	size_deviation[MAX_TEACHING_METHOD] = (int)math.safe_divide( size_deviation[MAX_TEACHING_METHOD], class_section_count[MAX_TEACHING_METHOD] );
    else
	size_deviation[MAX_TEACHING_METHOD] = 0;
}

//------- End of function DepartmentArray::calc_course_history -----//

//------- Begin of function DepartmentArray::calc_research_history -----//
//!
void DepartmentArray::calc_research_history() {   // 1217
    int i;
    int total = 0;

    for (i=0; i<RESEARCH_STATUS_COUNT; i++)
	research_y_history[i][THIS_YEAR] = 0;

    for (i=size(); i>0; i--) {
	if ( is_deleted(i) )
	    continue;

	Department* deptPtr = department_array[i];

	for (int s=0; s<RESEARCH_STATUS_COUNT; s++) {
	    research_m_history[s][THIS_MONTH] += deptPtr->research_m_history[s][THIS_MONTH];
	    research_y_history[s][THIS_YEAR] += deptPtr->research_y_history[s][THIS_YEAR];
	}

	research_total_history[THIS_MONTH] += deptPtr->research_total_history[THIS_MONTH];
	research_direct_history[THIS_MONTH] += deptPtr->research_direct_history[THIS_MONTH];

	regular_faculty_with_research[THIS_MONTH] += deptPtr->regular_faculty_with_research[THIS_MONTH] * deptPtr->regular_faculty_count;
	total += deptPtr->regular_faculty_count;
    }

    if ( total )
	regular_faculty_with_research[THIS_MONTH] /= total;
}

//------- End of function DepartmentArray::calc_research_history -----//

//------- Begin of function DepartmentArray::calc_faculty_student_history -----//
//!
                                                  // 1217
void DepartmentArray::calc_faculty_student_history() {
    int total_student_count = 0;
    int total_faculty_count = 0;

    int  i,j;
    Department* deptPtr;

    err_when(faculty_gender_history[GENDER_ETHNIC_TYPE_COUNT][THIS_YEAR] != 0);

    for ( i=size(); i>0; i-- ) {
	if ( is_deleted(i) )
	    continue;

	deptPtr = department_array[i];

	for (j=0;j<FACULTY_RANK_LEVEL_COUNT_N_TOTAL; j++)
	    faculty_level_history[j][THIS_YEAR] += deptPtr->faculty_level_history[j][THIS_YEAR];

	for (j=0;j<MAX_STUDENT_LEVEL_N_TOTAL; j++)
	    student_level_history[j][THIS_YEAR] += deptPtr->student_level_history[j][THIS_YEAR];

	for (j=0;j<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; j++) {
	    faculty_gender_history[j][THIS_YEAR] += deptPtr->faculty_gender_history[j][THIS_YEAR];
	    student_gender_history[j][THIS_YEAR] += deptPtr->student_gender_history[j][THIS_YEAR];
	}

	total_student_count += deptPtr->student_array.student_count;
	total_faculty_count += deptPtr->faculty_array.faculty_count;

	sys.yield();
    }

    // 0406

    GeneralDepartment *genDeptPtr = &department_res.general_dept;

    // 0406
    department_res.general_dept.calc_student_history();
    // calc_degree_awarded		//0408

    for (j=0;j<MAX_STUDENT_LEVEL_N_TOTAL; j++)
	student_level_history[j][THIS_YEAR] += genDeptPtr->student_level_history[j][THIS_YEAR];

    for (j=0;j<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; j++)
	student_gender_history[j][THIS_YEAR] += genDeptPtr->student_gender_history[j][THIS_YEAR];

    total_student_count += genDeptPtr->student_array.student_count;
}

//------- End of function DepartmentArray::calc_faculty_student_history -----//

//------- Begin of function DepartmentArray::save_initial_data -----//
void DepartmentArray::save_initial_data() {
    for (int i = 0; i< FACULTY_RANK_LEVEL_COUNT_N_TOTAL; i++) {
	initial_faculty_level[i] = faculty_level_history[i][HISTORY_YEAR_COUNT-1];
    }

    for (int i = 0; i< GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; i++) {
	initial_faculty_gender[i] = faculty_gender_history[i][HISTORY_YEAR_COUNT-1];
    }

    //	a percentage from 0 to 100
    initial_talent_teaching = talent_teaching[HISTORY_TRIMESTER_COUNT-1];
    initial_talent_scholarship = talent_scholarship[HISTORY_TRIMESTER_COUNT-1];
    initial_talent_research = talent_research[HISTORY_TRIMESTER_COUNT-1];

    //	a percentage from 0 to 100
    initial_performance_teaching = performance_teaching[HISTORY_TRIMESTER_COUNT-1];
    initial_performance_scholarship = performance_scholarship[HISTORY_TRIMESTER_COUNT-1];
    // BUGHRE change to monthly as in Faculty::update_history()
    initial_performance_research = performance_research[HISTORY_TRIMESTER_COUNT-1];
    // unit: hour
    initial_teaching_contact_hour = teaching_contact_hour[HISTORY_TRIMESTER_COUNT-1];

    for (int i = 0; i < RESEARCH_STATUS_COUNT; i++) {
	// ($000)
	initial_research_m[i] = research_m_history[i][HISTORY_MONTH_COUNT-1];
    }

    initial_p_educational_quality = p_educational_quality_history[HISTORY_TRIMESTER_COUNT-1];
    initial_p_academic_standing = p_academic_standing_history[HISTORY_YEAR_COUNT-1];

    initial_p_faculty_research =  p_faculty_research_history[HISTORY_MONTH_COUNT-1];
    initial_p_faculty_morale = p_faculty_morale_history[HISTORY_MONTH_COUNT-1];

    // ($000)
    initial_research_total = research_total_history[HISTORY_MONTH_COUNT-1];
    // ($000)
    initial_research_direct = research_direct_history[HISTORY_MONTH_COUNT-1];
    // 99->99%
    initial_regular_faculty_with_research = regular_faculty_with_research[HISTORY_MONTH_COUNT-1];

    for (int i = 0; i < DISCRETIONARY_TYPE_COUNT_N_TOTAL; i++) {
	initial_discretionary_hour_array[i] = discretionary_hour_array[i][HISTORY_TRIMESTER_COUNT-1];
    }

    initial_time_shifted_scholarship = time_shifted_scholarship;

    for (int i = 0; i < RESEARCH_STATUS_COUNT; i++) {
	// ($000)
	initial_research_y[i] = research_y_history[i][HISTORY_YEAR_COUNT-1];
    }

    for (int i = 0; i < MAX_TEACHING_METHOD_N_TOTAL; i++) {
	initial_class_section_count[i] = class_section_count[i];
	initial_convention_student_count[i] = convention_student_count[i];
	initial_average_class_size[i] = average_class_size[i];
	initial_size_deviation[i] = size_deviation[i];
	initial_admission_denied_count[i] = admission_denied_count[i];
	initial_failure_count[i] = failure_count[i];
	initial_normal_class_size[i] = normal_class_size[i];
    }

    initial_experienced_class_size = experienced_class_size[HISTORY_TRIMESTER_COUNT-1];

    initial_salary_gender_ethnic_equity = salary_gender_ethnic_equity_history[HISTORY_YEAR_COUNT-1];
    initial_salary_departmental_equity = salary_departmental_equity_history[HISTORY_YEAR_COUNT-1];
    initial_salary_rank_equity = salary_rank_equity_history[HISTORY_YEAR_COUNT-1];
}

//------- End of function DepartmentArray::save_initial_data -----//

//------- Begin of function DepartmentArray::calc_salary_equity -----//
void DepartmentArray::calc_salary_equity() {
    // update salary_gender_ethnic_equity_history[HISTORY_YEAR_COUNT-1]
    //	salary_departmental_equity_history[HISTORY_YEAR_COUNT-1]
    // and salary_rank_equity_history[HISTORY_YEAR_COUNT-1]

    int deptRecno, geGroupId, rankAgeId;

    // sums of faculty salaries
    int facultyCount = 0;                           // number of faculty
    float facultySum = 0.0f;                        // sum of salary of all faculty
    float facultySumSq = 0.0f;                      // sum of salary^2 of all faculty

    // sums per department

    int deptCount[MAX_DEPARTMENT];                  // number of faculty of each deparment
    float deptSum[MAX_DEPARTMENT];                  // sum of salary of each department
    memset( deptCount, 0, sizeof(deptCount) );
    memset( deptSum, 0, sizeof(deptSum) );

    // sum per gender-ethnic group

    int geGroupCount[GENDER_ETHNIC_TYPE_COUNT];
    float geGroupSum[GENDER_ETHNIC_TYPE_COUNT];
    memset( geGroupCount, 0, sizeof(geGroupCount) );
    memset( geGroupSum, 0, sizeof(geGroupSum) );

    // sum per rank

    int rankCount[MAX_RANK_AGE_GROUP];              // FULL_PROF divied into 3 group, use Faculty::rank_age_group
    float rankSum[MAX_RANK_AGE_GROUP];
    memset( rankCount, 0, sizeof(rankCount) );
    memset( rankSum, 0, sizeof(rankSum) );

    // collect sum and count of each faculty

    err_when( size() > MAX_DEPARTMENT );
    for( deptRecno = 1; deptRecno <= size(); ++deptRecno ) {
	if( is_deleted(deptRecno) )
	    continue;

	FacultyArray &facultyArray = operator[](deptRecno)->faculty_array;
	for( int i = 1; i <= facultyArray.size(); ++i ) {
	    if( facultyArray.is_deleted(i) )
		continue;

	    float salary = facultyArray[i]->salary;

	    ++facultyCount;
	    facultySum += salary;
	    facultySumSq += salary * salary;

	    ++deptCount[deptRecno-1];
	    deptSum[deptRecno-1] += salary;

	    geGroupId = facultyArray[i]->gender_ethnic_group;
	    err_when( geGroupId< 0 || geGroupId >= GENDER_ETHNIC_TYPE_COUNT );
	    ++geGroupCount[geGroupId];
	    geGroupSum[geGroupId] += salary;

	    rankAgeId = facultyArray[i]->rank_age_group();
	    err_when( rankAgeId < 0 || rankAgeId >= MAX_RANK_AGE_GROUP );
	    ++rankCount[rankAgeId];
	    rankSum[rankAgeId] += salary;
	}
    }

    float facultyAvg = facultySum / (float)facultyCount;

    //            n   _   _
    // S  = (n-1)Sum (x - x)²
    //  d        d=1   d

    float deptDevSum = 0.0f;
    int deptExistCount = 0;                         // number of non-empty department
    for( deptRecno = 1; deptRecno <= size(); ++deptRecno ) {
	if( deptCount[deptRecno-1] > 0 ) {
	    ++deptExistCount;
	    float deptAvg = deptSum[deptRecno-1] / (float)deptCount[deptRecno-1];
	    deptDevSum += (deptAvg - facultyAvg) * (deptAvg - facultyAvg);
	}
    }
    float deptEquity = float(deptExistCount-1) * deptDevSum;

    //           n   _   _
    // S  = 3 * Sum (x - x)²
    //  g       g=1   g

    float geGroupDevSum = 0.0f;
    int geGroupExistCount = 0;
    for( geGroupId = 0; geGroupId < GENDER_ETHNIC_TYPE_COUNT; ++geGroupId ) {
	if( geGroupCount[geGroupId] > 0 ) {
	    ++geGroupExistCount;
	    float geGroupAvg = geGroupSum[geGroupId] / (float)geGroupCount[geGroupId];
	    geGroupDevSum += (geGroupAvg - facultyAvg) * (geGroupAvg - facultyAvg);
	}
    }
    float geGroupEquity = (geGroupExistCount-1) * geGroupDevSum;

    //           n   _   _
    // S  = 6 * Sum (x - x)²
    //  r       r=1   r

    float rankDevSum = 0.0f;
    int rankExistCount = 0;
    for( rankAgeId = 0; rankAgeId < MAX_RANK_AGE_GROUP; ++rankAgeId ) {
	if( rankCount[rankAgeId] > 0 ) {
	    ++rankExistCount;
	    float rankAvg = rankSum[rankAgeId] / (float)rankCount[rankAgeId];
	    rankDevSum += (rankAvg - facultyAvg) * (rankAvg - facultyAvg);
	}
    }
    float rankEquity = (rankExistCount-1) * rankDevSum;

    //       N      _
    // S  = Sum(x - x)² - S - S - S
    //  E   i=1  i         d   g   r

    //                           N          N
    // replace summation by use Sum x ² - (Sum x)²/N
    //                          i=1  i     i=1

    float facultyEquity = facultySumSq - facultySum * facultySum / float(facultyCount)
	- deptEquity - geGroupEquity - rankEquity;

    // do not shift history here

    if( facultyEquity == 0.0f ) {
	salary_departmental_equity_history[THIS_YEAR] = 0.0f;
	salary_gender_ethnic_equity_history[THIS_YEAR] = 0.0f;
	salary_rank_equity_history[THIS_YEAR] = 0.0f;
    }
    else {
	salary_departmental_equity_history[THIS_YEAR] = float(deptExistCount-1) * deptEquity / facultyEquity;
	salary_gender_ethnic_equity_history[THIS_YEAR] = float(geGroupExistCount-1) * geGroupEquity / facultyEquity;
	salary_rank_equity_history[THIS_YEAR] = float(rankExistCount-1) * rankEquity / facultyEquity;
    }
}

//------- End of function DepartmentArray::calc_salary_equity -----//
