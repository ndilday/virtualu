//Filename    : ODEPT_PI.CPP
//Description : Functions for calculating departmental performance indicators
//Owner       : Fred

// #include <OSYS.H>
#include <ODEPT.H>
#include <OMATH.H>
//#include <OFINANCE.H>
#include <OPSCHOOL.H>
#include <OFACULTY.H>
#include <OFACURES.H>
#include <OSCHLRES.H>
#include <OLIBTECH.H>
#include <ODEPTRES.H>
#include <OCOURRES.H>

const float Department::var_educational_quality_weight[EDUCATIONAL_QUALITY_VAR_COUNT] =
{ 0.35f, 0.15f, 0.10f, 0.05f, 0.15f, 0.10f, 0.10f, };

const float Department::var_academic_standing_weight[ACADEMIC_STANDING_VAR_COUNT] =
{ 0.5f, 0.3f, 0.1f, 0.1f, };

//------ Begin of function Department::calc_all_performance_indicator -------//
//!
//! Calculate the Faculty Publication performance indicator.
//!
void Department::calc_all_performance_indicator() {
    err_here();
    return;
}

//------- End of function Department::calc_all_performance_indicator --------//

//------ Begin of function Department::calc_faculty_research -------//
int Department::get_regular_faculty_count() {
    return faculty_level_history[ASST_PROF][THIS_YEAR] + faculty_level_history[ASSOC_PROF][THIS_YEAR]
	+ faculty_level_history[FULL_PROF][THIS_YEAR];
}

//------- End of function Department::calc_all_performance_indicator --------//

//------ Begin of function Department::calc_faculty_research -------//
//!
//! Calculate the Faculty Publication performance indicator.
//!
void Department::calc_faculty_research() {
    Faculty* facultyPtr;
    int facultyCount = 0;

    p_faculty_research = 0;                         // reset it first

    for( int i=faculty_array.size() ; i>0 ; i-- ) {
	if( faculty_array.is_deleted(i) )
	    continue;

	facultyPtr = faculty_array[i];
	facultyCount++;

	p_faculty_research += facultyPtr->performance_research;
    }

    if ( facultyCount )
	p_faculty_research /=  facultyCount;
    else
	p_faculty_research = 0;

    // just for error checking, they should be the same
    err_when( faculty_array.packed_size() != facultyCount );
    // just for error checking, they should be the same
    err_when( faculty_array.packed_size() != faculty_array.faculty_count );
}

//------- End of function Department::calc_faculty_research --------//

//------ Begin of function Department::calc_academic_standing -------//
//!
//! Calculate Departmental academic standing.
//!
void Department::calc_academic_standing() {
    //--------------------------------------------------//
    //
    // Calculate average of "total monthly expenditures
    // on sponsored research projects" in the faculty database.
    //
    //--------------------------------------------------//

    Faculty* facultyPtr;

    int totalResearchExpense=0;
    int facultyCount=0;

    for( int i=faculty_array.size() ; i>0 ; i-- ) {
	if( faculty_array.is_deleted(i) )
	    continue;

	facultyPtr = faculty_array[i];

	totalResearchExpense += facultyPtr->research_month_expense;

	facultyCount++;
    }

    // just for error checking, they should be the same
    err_when( faculty_array.packed_size() != facultyCount );
    // just for error checking, they should be the same
    err_when( faculty_array.packed_size() != faculty_array.faculty_count );

    float avgResearchExpense;

    if ( research_dollar_norm )
	avgResearchExpense = float(totalResearchExpense) / research_dollar_norm;
    else
	avgResearchExpense = 0.0f;

    //-----------------------------------------------------//
    //
    // Calculate the average of "academic talent index" of
    // students who are doctoral students.
    //
    //-----------------------------------------------------//

    Student* studentPtr;
    float   totalTalentAcademic=0;
    int   studentCount=0;

    for( int i=student_array.size() ; i>0 ; i-- ) {
	if( student_array.is_deleted(i) )
	    continue;

	studentPtr = student_array[i];

	if( studentPtr->student_level != DOCTOR )
	    continue;

	totalTalentAcademic += (float) studentPtr->talent_academic;
	studentCount++;
    }

    float avgTalentAcademic;

    if( studentCount > 0 )
	avgTalentAcademic = float(totalTalentAcademic) / studentCount;
    else
	avgTalentAcademic = 0;

    //---------------//
    float doctorCountPerFac;                        // Doctoral STUs (SL4): number of sims per regular faculty

    int facCount = get_regular_faculty_count();

    doctorCountPerFac = facCount > 0 ? float(studentCount) / facCount : 0 ;

    //----- pass the parameters through response functions ----//

    float facultyPublication = math.single_response_func(1, 100, 55.94f, 51.19f, p_faculty_research);
    avgResearchExpense = math.dual_response_func(0,3,100,2.069f,0.505f,0.015f, 1.872f, avgResearchExpense);
    avgTalentAcademic = math.dual_response_func(1, 35, 100, 17.11f, 51.61f, 12.38f, 62.41f, avgTalentAcademic);
    doctorCountPerFac = math.dual_response_func(1,60,100,0.394f,2.058f,0.915f,1.212f,doctorCountPerFac);

    //--------------//

    //float weight[ACADEMIC_STANDING_VAR_COUNT] = { 0.5f, 0.3f, 0.1f, 0.1f };
    float input[ACADEMIC_STANDING_VAR_COUNT];
    input[0] = facultyPublication;
    input[1] = avgResearchExpense;
    input[2] = avgTalentAcademic;
    input[3] = doctorCountPerFac;

    float newValue = 0;

    for (int i=0; i<ACADEMIC_STANDING_VAR_COUNT; i++)
	newValue += var_academic_standing_weight[i] * input[i];

    p_academic_standing_no_latency = newValue;

    if( info.prerun_year == 1 )
	p_academic_standing = newValue;               // initialize it without latency effect
    else
	// 0.33 is the latency parameter
	p_academic_standing = player_school.latency_func( 0.33f, p_academic_standing, newValue);

    //--------------//

    for (int i=0; i<ACADEMIC_STANDING_VAR_COUNT; i++) {
	var_academic_standing_history[i][THIS_YEAR] = input[i];

	float lastYear = var_academic_standing_history[i][THIS_YEAR-1];
	//if (lastYear)
	//1130 // * var_academic_standing_weight[i] * 100 / lastYear;
	var_change_academic_standing[i] = ( input[i] - lastYear );
	//else
	//	var_change_academic_standing[i] = 0;
    }
}

//------- End of function Department::calc_academic_standing --------//

//------ Begin of function Department::calc_tech_utilization -------//
//!
void Department::calc_tech_utilization() {
    //--------------------------------------------------//
    //
    // Calculate average of "allocation of discretionary time this
    // trimester in educational development".
    //
    //--------------------------------------------------//

    //##### begin: fred 0526 ########//
    Faculty *facultyPtr;
    int facultyCount = 0;
    float avgITHour = 0;

    for( int i=faculty_array.size() ; i>0 ; i-- ) {
	if( faculty_array.is_deleted(i) )
	    continue;

	facultyPtr = faculty_array[i];

	avgITHour += facultyPtr->p_discretionary_hour_it;

	facultyCount++;
    }

    if ( facultyCount )
	avgITHour /= facultyCount;

    // Version 2.0 Item 8d iii)
    float edDevHrs = avgITHour;
    float timeShiftedFromScholarship=0;
    facultyCount=0;
    float totalEdDevHrs=0;

    // calculate timeShiftedFromScholarship
    for ( int i=faculty_array.size(); i>0; i-- ) {
	if ( faculty_array.is_deleted(i) )
	    continue;

	timeShiftedFromScholarship = (0.3*library_tech_office.faculty_incentive_using_it/100+
				      0.3*department_incentives/100)*(faculty_array.operator [](i)->discretionary_hour_array[DT_SCHOLARSHIP]);

	// restore back time shifted last trimester
	faculty_array.operator [](i)->discretionary_hour_array[DT_SCHOLARSHIP] += faculty_array[i]->time_shifted_scholarship;

	edDevHrs = edDevHrs + timeShiftedFromScholarship;

	faculty_array.operator [](i)->discretionary_hour_array[DT_SCHOLARSHIP] -= timeShiftedFromScholarship;

	faculty_array[i]->time_shifted_scholarship = timeShiftedFromScholarship;

	totalEdDevHrs += edDevHrs;
	facultyCount++;
    }

    float input1;

    // calculate p_tech_utilization
    if ( facultyCount != 0 )
	input1 = totalEdDevHrs/facultyCount;
    else
	input1 = 0;

    p_tech_utilization = player_school.latency_func( 0.9f, p_tech_utilization, input1 );



    cummulative_tech_utilization += p_tech_utilization;
}

//------- End of function Department::calc_tech_utilization --------//

//------ Begin of function Department::calc_educational_quality -------//
//!
void Department::calc_educational_quality() {
    //##### begin fred 0811 #####//

    float input[EDUCATIONAL_QUALITY_VAR_COUNT];

    memset(input, 0, sizeof(input));

    //----- calculate the department's average teaching and scholarship performance ------//

    int female = 0, minority = 0;

    for( int i=faculty_array.size() ; i>0 ; i-- ) {
	if( faculty_array.is_deleted(i) )
	    continue;

	input[0] += faculty_array[i]->performance_teaching;
	input[2] += faculty_array[i]->performance_scholarship;
    }

    if ( faculty_array.faculty_count ) {
	input[0] /= faculty_array.faculty_count;
	input[2] /= faculty_array.faculty_count;
    }
    else {
	input[0] = 0;
	input[2] = 0;
    }

    //--------------//
    input[1] = (float) faculty_array.avg_discretionary_hour(DT_EDUCATIONAL_DEVELOPMENT);

    //--------------//
    input[3] = p_faculty_research;

    //--------------//
    input[4] = p_tech_utilization;

    //--------------//
    /*
      for each teaching method for the dept
      sum += (total enrollment for courses of this teaching method/number of
      sections for courses of this teaching method) - (normal class size for this
      teaching method);

      1st_factor = sum / number of teaching methods;

      In other words, this is the average, over teaching methods, of average per
      section (ie actual emrollment/no. of sections) minus normal class size. I'm
      afraid I left off the minus sign in the variable definition.
    */
    /*
      int	classSize[MAX_TEACHING_METHOD];
      int	classSectionCount[MAX_TEACHING_METHOD];
      int	normalClassSizePerSection[MAX_TEACHING_METHOD];

      memset(classSize, 0, sizeof(classSize));
      memset(classSectionCount, 0, sizeof(classSize));
      memset(normalClassSizePerSection, 0, sizeof(classSize));

      for( i=course_array.packed_size() ; i>0 ; i-- )  //## chea 080999 this should use size() since there is no del so this is alright
      //   for( i=course_array.size() ; i>0 ; i-- )
      {
      //		if(course_array.is_deleted(i))
      //			continue;

      Course* coursePtr = course_array[i];

      classSize[coursePtr->teaching_method]
      += coursePtr->convention_student_count;

      classSectionCount[coursePtr->teaching_method]
      += coursePtr->class_section_count;

      normalClassSizePerSection[coursePtr->teaching_method]
      += coursePtr->normal_class_size();

      err_when(coursePtr->teaching_method >= MAX_TEACHING_METHOD);
      }
    */
    input[5] = 0;

    for ( int i=0; i<MAX_TEACHING_METHOD; i++) {
	input[5] += (float) course_array.average_class_size[i] / course_array.normal_class_size[i];

	//		if ( classSectionCount[i] )
	//			input[5] += (float(classSize[i]) / classSectionCount[i]) / normalClassSizePerSection[i];
    }

    input[5] /= MAX_TEACHING_METHOD;

    //--------------//
    // "Percent course sections" for a teaching method = 100*(number sections for
    // that teaching method/total number of sections for the department).

    int totalSectionCount = 0;

    for ( int i=0; i<MAX_TEACHING_METHOD; i++) {
	totalSectionCount += course_array.class_section_count[i];
	//		totalSectionCount += classSectionCount[i];
    }

    float inputSub;

    if ( totalSectionCount ) {
	for ( int i=0; i<MAX_TEACHING_METHOD; i++) {
	    inputSub = float( 100 * course_array.class_section_count[i] / totalSectionCount )
		- course_res.get_template(department_res[department_id]->template_course_type)->target_preference[P_SEMINAR];

	    //			inputSub = float( 100 * classSectionCount[i] / totalSectionCount )
	    //							- course_res.get_template(department_res[department_id]->template_course_type)->target_preference[P_SEMINAR];

	    input[6] += inputSub>0 ? inputSub : -inputSub;
	}
    }

    input[6] /= MAX_TEACHING_METHOD;

    //-------- these two have to go thru the response function before they are updated to the historical array ---//

    input[5] = math.dual_response_func(1,50,100,-0.1783f,-0.1822f,1.2340f,0.7667f, input[5]);

    input[6] = 0.5f + input[6] / 20.0f;
    input[6] = math.dual_response_func(1,50,100,-0.1783f,-0.1822f,1.2340f,0.7667f, input[6]);
    //	input[6] = math.dual_response_func(0,48.32f,100,13.97f,20.21f,-18.14f,14.70f, input[6]);

    //----- save the input into historical array for later display ---------//

    for (int i=0; i<EDUCATIONAL_QUALITY_VAR_COUNT; i++) {
	var_educational_quality_history[i][THIS_TRIMESTER] = input[i];

	float lastYear = var_educational_quality_history[i][THIS_TRIMESTER-3];
	//if (lastYear)
	//1130 // * var_educational_quality_weight[i] * 100 / lastYear;
	var_change_educational_quality[i] = ( input[i] - lastYear );
	//else
	//	var_change_educational_quality[i] = 0;
    }

    //------ pass the input through response functions and use the results to calculate the educational quality value -----//

    input[0] = math.single_response_func(1,100,55.94f,51.19f, input[0]);
    input[1] = math.single_response_func(1,100,6.32f,7.58f, input[1]);
    input[2] = math.dual_response_func(1,50,100,28.35f,29.00f,22.31f,77.45f, input[2]);
    input[3] = math.dual_response_func(1,100,90,28.91f,20.78f,21.85f,80.03f, input[3]);
    input[4] = math.single_response_func(1,100,48.82f,40.98f, input[4]);

    //------- calculate the new educational quality -------//

    float newValue = 0;

    for (int i=0; i<EDUCATIONAL_QUALITY_VAR_COUNT; i++)
	newValue += var_educational_quality_weight[i] * input[i];

    p_educational_quality = newValue;
}

//------- End of function Department::calc_educational_quality --------//

//------ Begin of function Department::calc_student_morale -------//
//!
void Department::calc_student_morale() {
    // calc Student satisfaction with the department's academic program
    // 980923: = average of student satisfaction: academic for students majoring in the department

    float totalSat = 0, input;

    for( int i=student_array.size() ; i>0 ; i-- ) {
	if( student_array.is_deleted(i) )
	    continue;

	totalSat += student_array[i]->satisfaction_overall;
	//		totalSat += student_array[i]->satisfaction_academic;
    }

    if ( student_array.student_count )
	input = totalSat / student_array.student_count;
    else
	input = 0;

    if( info.prerun_year )                          // set it without latency during initialization
	p_student_morale = input;
    else
	p_student_morale = player_school.latency_func( 0.9f, p_student_morale, input);
}

//------- End of function Department::calc_student_morale --------//

//------ Begin of function Department::calc_faculty_morale -------//
//!
void Department::calc_faculty_morale() {
    float totalSat = 0;

    if ( info.game_month == 1 )                     // if it is Jananry, no need to re-calculate the faculty morale
	return;

    for( int i=faculty_array.size() ; i>0 ; i-- ) {
	if( faculty_array.is_deleted(i) )
	    continue;

	// have not taught in this semester
	if ( player_school.cur_trimester == faculty_array[i]->off_duty_trimester )
	    totalSat += faculty_array[i]->satisfaction_index_last_tri;
	else
	    totalSat += faculty_array[i]->satisfaction_index;
    }

    if (faculty_array.faculty_count )
	p_faculty_morale = totalSat / faculty_array.faculty_count;
    else
	p_faculty_morale = 0;

}

//------- End of function Department::calc_faculty_morale --------//

//------ Begin of function Department::calc_faculty_morale -------//
//!
void Department::calc_faculty_diversity() {
    // Formula used by the U.S. Census Bureau
    // diversity = 1 - {(%NonMinMale/100)^2 + (%NonMinFemale/100)^2 + (%MinMale/100)^2 + (%MinFemale/100)^2}

    int nonMinMale = 0;
    int nonMinFemale = 0;
    int minMale = 0;
    int minFemale = 0;

    for( int i=faculty_array.size() ; i>0 ; i-- ) {
	if( faculty_array.is_deleted(i) )
	    continue;

	switch(faculty_array[i]->gender_ethnic_group) {
	case NONMINORITY_MALE: ++nonMinMale; break;
	case NONMINORITY_FEMALE: ++nonMinFemale; break;
	case MINORITY_MALE: ++minMale; break;
	case MINORITY_FEMALE: ++minFemale; break;
	default:
	    err_here();
	    break;
	}
    }

    int facultyCount= nonMinMale + nonMinFemale + minMale + minFemale;
    if( facultyCount <= 0 ) {
	p_faculty_diversity = 0;
	return;
    }

    p_faculty_diversity = 100.0f * (1.0f -
				    float(nonMinMale*nonMinMale + nonMinFemale*nonMinFemale + minMale*minMale + minFemale*minFemale)
				    / float(facultyCount*facultyCount));
}

//------- End of function Department::calc_faculty_morale --------//

//------ Begin of function Department::calc_faculty_history -------//
//!
void Department::calc_faculty_history() {
    int totalFacultyCount = 0, countInvest=0;

    int j;
    Faculty *facPtr;
    char gender, sl;

    // loop faculty
    for (j=faculty_array.size(); j>0; j--) {
	if ( faculty_array.is_deleted(j) )
	    continue;

	facPtr = faculty_array[j];
	sl = facPtr->rank_level;
	gender = facPtr->gender_ethnic_group;

	faculty_level_history[sl][THIS_YEAR]++;
	faculty_gender_history[gender][THIS_YEAR]++;

	if (facPtr->is_investigator)
	    countInvest++;

	totalFacultyCount++;
    }                                               // end for j

    faculty_level_history[FACULTY_RANK_LEVEL_TOTAL][THIS_YEAR] = totalFacultyCount;
    faculty_gender_history[GENDER_ETHNIC_TYPE_TOTAL][THIS_YEAR] = totalFacultyCount;
    percent_investigator = (char) math.safe_divide(countInvest * 100,totalFacultyCount);

    err_when(totalFacultyCount != faculty_array.faculty_count);
}

//------- End of function Department::calc_faculty_history --------//

//------ Begin of function GeneralDepartment::calc_student_history -------//
//!
void GeneralDepartment::calc_student_history() {
    int totalStudentCount = 0;

    int i, j;
    Student *stuPtr;
    char gender, sl;

    for (i=0; i<MAX_STUDENT_LEVEL; i++)             // 0406
	student_level_history[i][THIS_YEAR] = 0;
    for (i=0; i<GENDER_ETHNIC_TYPE_COUNT; i++)
	student_gender_history[i][THIS_YEAR] = 0;

    // loop student
    for (j=student_array.size(); j>0; j--) {
	if ( student_array.is_deleted(j) )
	    continue;

	stuPtr = student_array[j];
	sl = stuPtr->student_level;
	gender = stuPtr->gender_ethnic_group;

	student_level_history[sl][THIS_YEAR]++;
	student_gender_history[gender][THIS_YEAR]++;

	totalStudentCount++;
    }                                               // end for j

    student_level_history[MAX_STUDENT_LEVEL_TOTAL][THIS_YEAR] = totalStudentCount;
    student_gender_history[GENDER_ETHNIC_TYPE_TOTAL][THIS_YEAR] = totalStudentCount;
    err_when(totalStudentCount != student_array.student_count);
}

//------- End of function GeneralDepartment::calc_student_history --------//

//------- Begin of function Department::calc_research_history -----//
//!
void Department::calc_research_history() {        // 1217
    /*
      int	research_m_history[RESEARCH_STATUS_COUNT][HISTORY_MONTH_COUNT];		// ($000)
      int	research_y_history[RESEARCH_STATUS_COUNT][HISTORY_YEAR_COUNT];			// ($000)

      int	research_total_history[HISTORY_MONTH_COUNT];			// ($000)
      int	research_direct_history[HISTORY_MONTH_COUNT];		// ($000)

      int	regular_faculty_with_research[HISTORY_MONTH_COUNT];		// 99->99%
    */
    int i;

    // research_m_history[j][THIS_MONTH] is updated in faculty_array.next_day();

    for (i=0; i<RESEARCH_STATUS_COUNT; i++) {
	//research_m_history[i][THIS_MONTH] /= 1000;		//BUGHERE if not "/ 1000"
	research_y_history[i][THIS_YEAR] += research_m_history[i][THIS_MONTH];
    }

    //## chea 221199 This is controling the SR report value
    //	research_total_history[THIS_MONTH] = total_research_dollar;
    //	research_direct_history[THIS_MONTH] = total_research_dollar_direct;
    research_total_history[THIS_MONTH] = total_research_dollar/1000;
    research_direct_history[THIS_MONTH] = total_research_dollar_direct/1000;

    regular_faculty_with_research[THIS_MONTH] = 0;
    regular_faculty_count = 0;

    for( i=faculty_array.size() ; i>0 ; i-- ) {
	if( faculty_array.is_deleted(i) )
	    continue;

	Faculty* facPtr = faculty_array[i];

	if ( facPtr->research_month_expense )
	    regular_faculty_with_research[THIS_MONTH]++;

	if ( facPtr->is_regular() )
	    regular_faculty_count++;
    }

    if ( regular_faculty_count )
	regular_faculty_with_research[THIS_MONTH] = regular_faculty_with_research[THIS_MONTH] * 100 / regular_faculty_count;

    //---------//
}

//------- End of function Department::calc_research_history -----//

// ##### Begin MArco ##### //
//------ Begin of function GeneralDepartment::save_initial_data -------//
//!
void GeneralDepartment::save_initial_data() {
    for (int i = 0; i < MAX_STUDENT_LEVEL_N_TOTAL; i++)
	initial_dept_student_level_no[i] = dept_student_level_no[i];
}

//
// ------- End of function GeneralDepartment::save_initial_data -----//
// ##### End Marco ##### //
