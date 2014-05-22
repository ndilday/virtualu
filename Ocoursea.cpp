//Filename    : OCOURSEA.CPP
//Description : CourseArray Class Definition

#include <ALL.H>
#include <OMATH.H>
#include <OSYS.H>
#include <OFINANCE.H>
#include <OINFO.H>
#include <OCOURSE.H>

//--------- Define static variables ---------//

static char depthArray[] =
{ DEPTH_D1, DEPTH_D1, DEPTH_D1, DEPTH_D2, DEPTH_D2, DEPTH_D3, DEPTH_GR };

static char focusArray[] =
{ FOCUS_LD, FOCUS_M, FOCUS_UD, FOCUS_M, FOCUS_UD, FOCUS_UD, FOCUS_GR };

//----------- Begin of function CourseArray Constructor -----//
//!
CourseArray::CourseArray() : DynArrayB(sizeof(Course), 100, DEFAULT_REUSE_INTERVAL_DAYS) {
    //## fred 1019 ##//
    memset(course_enrollments, 0, sizeof(course_enrollments));

    // 1218
    memset(experienced_class_size, 0, sizeof(experienced_class_size));

    memset(outsourcing_credits_history, 0, sizeof(outsourcing_credits_history));
}

//------------- End of function CourseArray Constructor -----//

//----------- Begin of function CourseArray Destructor ------//
//!
CourseArray::~CourseArray() {
}

//------------- End of function CourseArray Destructor ------//

//--------- Begin of function CourseArray::add ---------//

void CourseArray::add(Course *newCourse) {
    linkin(newCourse);

    ((Course*)get())->course_recno = recno();
}

//----------- End of function CourseArray::add -----------//

//--------- Begin of function CourseArray::del ---------//

void CourseArray::del(int recNo) {
    linkout(recNo);
}

//----------- End of function CourseArray::del -----------//

//--------- Begin of function CourseArray::get_depth ---------//
//!
//! Return the depth type of a depth focus category.
//!
int CourseArray::get_depth(int depthFocusId) {
    return depthArray[depthFocusId];
}

//----------- End of function CourseArray::get_depth -----------//

//--------- Begin of function CourseArray::get_focus ---------//
//!
//! Return the focus type of a depth focus category.
//!
int CourseArray::get_focus(int depthFocusId) {
    return focusArray[depthFocusId];
}

//----------- End of function CourseArray::get_focus -----------//

//--------- Begin of function CourseArray::get_course ---------//
//!
//! Get a specific Course data.
//!
Course* CourseArray::get_course(int courseDepth, int courseFocus, int teachingMethod) {
    for( int i=0 ; i<MAX_COURSE_DEPTH_FOCUS ; i++ ) {
	if( depthArray[i] == courseDepth && focusArray[i] == courseFocus ) {
	    int courseRecno = i * MAX_TEACHING_METHOD + teachingMethod + 1;

	    return operator[](courseRecno);
	}
    }

    err_here();

    return NULL;
}

//----------- End of function CourseArray::get_course -----------//

//--------- Begin of function CourseArray::next_trimester ---------//
//!
//! This function is called when a trimester has ended.
//! BIG ASSUMPTION: coursePtr->next_trimester() only call coursePtr->reset_vars()
//!
void CourseArray::next_trimester() {
    //--- reset course variables in a new trimester ---//

    Course* coursePtr;

    for( int i=size() ; i>0 ; i-- ) {
	coursePtr = operator[](i);

	coursePtr->next_trimester();
    }
}

//----------- End of function CourseArray::next_trimester -----------//

//#### begin fred 1019 ####//
//--------- Begin of function CourseArray::next_day ---------//
//!
//! This function is called to calc the max student enrollment in each teaching method for the course_array
//!
void CourseArray::next_day(int initial)
{
    if ( info.game_day == 1 ) {
	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month )
	    memset(course_enrollments, 0, sizeof(course_enrollments));
    }

    // start of a trimester
    if ( info.is_begin_trimester() ) {
	//--------  calc course enrollments for facility office --------//

	int i, enrollments[MAX_TEACHING_METHOD];

	memset(enrollments, 0, sizeof(enrollments));

	for (i=size(); i>0; i--) {
	    Course* cPtr = this->operator[](i);
	    enrollments[cPtr->teaching_method] = cPtr->convention_student_count;
	}

	// should be the maximum trimestrer enrollment over the three trimesters last year

	for ( i=0; i<MAX_TEACHING_METHOD; i++) {
	    if ( course_enrollments[i] < enrollments[i] ) {
		course_enrollments[i] = enrollments[i];
	    }
	}

	if ( !initial )
	    update_history();
    }
}

//----------- End of function CourseArray::next_day -----------//

//------- Begin of function CourseArray::update_history -----//

void CourseArray::update_history() {
    int sz = sizeof(class_section_count);

    memset(class_section_count, 0, sz);
    memset(convention_student_count, 0, sz);
    memset(average_class_size, 0, sz);
    memset(normal_class_size, 0, sz);

    memset(size_deviation, 0, sz);
    memset(admission_denied_count, 0, sz);
    memset(failure_count, 0, sz);

    memset(outsourcing_credits, 0 ,sz);

    /*
      Re: Q37a. hiring;and Q38
      Date: Wed, 9 Dec 1998 19:14:12 -0700

      Q38

      Q1. Re "How to calculate the item "experienced class size" on the	screen Course
      for both "department" and "institution"?" This is just the average of the
      //!
      class sizes for the individual courses in the department or the
      institution. I.e, calculate (enrollment/number of primary sections) for
      each course and take the average. By "primary section" I mean to ignore the
      breakout sections for classes with breakouts: the breakouts don't enter the
      average. The "institution" number is the average for all courses in the
      instituion, not the average of the departmental averages.
    */

    shift_history(experienced_class_size, HISTORY_TRIMESTER_COUNT);

    //float total = 0;
    //int count = 0;

    for (int i=size(); i>0; i--) {
	Course* cPtr = this->operator[](i);
	int tm = cPtr->teaching_method;
	err_when(tm >= MAX_TEACHING_METHOD);

	class_section_count[tm] += cPtr->class_section_count;
	convention_student_count[tm] += cPtr->convention_student_count;
	// 0308 use = instead of +=
	normal_class_size[tm] = cPtr->normal_class_size();
	admission_denied_count[tm] += cPtr->admission_denied_count;
	failure_count[tm] += cPtr->failure_count;

	outsourcing_credits[tm] += cPtr->outsourcing_credits;

	if ( tm != BREAKOUT_LAB ) {
	    //			total += math.safe_divide(cPtr->convention_student_count, cPtr->class_section_count);
	    //			count++;
	}
    }

    //experienced_class_size[THIS_TRIMESTER] = (int) math.safe_divide(total, (float) count);

    for(char t=0; t<MAX_TEACHING_METHOD; t++) {
	outsourcing_credits[MAX_TEACHING_METHOD] += outsourcing_credits[t];

	if ( t == DISTANCE_LEARN_COURSE )             // Kenneth
	    continue;

	average_class_size[t] = (int) math.safe_divide(convention_student_count[t], class_section_count[t]);

	if( class_section_count[t] > 0 )
	    size_deviation[t] = (int) math.safe_divide(100 * (average_class_size[t]-normal_class_size[t]), normal_class_size[t]);
	else
	    size_deviation[t] = 0;                      // no deviation if there is no section.

	//0405
	experienced_class_size[THIS_TRIMESTER] += average_class_size[t];

	//----//

	class_section_count[MAX_TEACHING_METHOD] += class_section_count[t];
	convention_student_count[MAX_TEACHING_METHOD] += convention_student_count[t];
	//0405 average_class_size[MAX_TEACHING_METHOD] += average_class_size[t];
	normal_class_size[MAX_TEACHING_METHOD] += normal_class_size[t];
	// size_deviation[MAX_TEACHING_METHOD] += size_deviation[t];
	// fix in version 2 use weighted average (weight is class_section_count)
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

    //## chea end 990525

    normal_class_size[MAX_TEACHING_METHOD] /= (MAX_TEACHING_METHOD-1);

    // size_deviation[MAX_TEACHING_METHOD] /= (MAX_TEACHING_METHOD-1);
    // fix in version 2 use weighted average (weight is class_section_count)
    if( class_section_count[MAX_TEACHING_METHOD] > 0 )
	size_deviation[MAX_TEACHING_METHOD] = (int)math.safe_divide( size_deviation[MAX_TEACHING_METHOD], class_section_count[MAX_TEACHING_METHOD] );
    else
	size_deviation[MAX_TEACHING_METHOD] = 0;

    // after updating
    for(int t=0; t<MAX_TEACHING_METHOD_N_TOTAL; t++) {
	shift_history(class_section_count_history[t], HISTORY_TRIMESTER_COUNT);
	class_section_count_history[t][THIS_TRIMESTER] = class_section_count[t];
	shift_history(convention_student_count_history[t], HISTORY_TRIMESTER_COUNT);
	convention_student_count_history[t][THIS_TRIMESTER] = convention_student_count[t];
	shift_history(size_deviation_history[t], HISTORY_TRIMESTER_COUNT);
	size_deviation_history[t][THIS_TRIMESTER] = size_deviation[t];
	shift_history(admission_denied_count_history[t], HISTORY_TRIMESTER_COUNT);
	admission_denied_count_history[t][THIS_TRIMESTER] = admission_denied_count[t];
	shift_history(failure_count_history[t], HISTORY_TRIMESTER_COUNT);
	failure_count_history[t][THIS_TRIMESTER] = failure_count[t];
	shift_history( outsourcing_credits_history[t], HISTORY_TRIMESTER_COUNT);
	outsourcing_credits_history[t][THIS_TRIMESTER] = outsourcing_credits[t];
    }

}

//------- End of function CourseArray::update_history -----//

// ##### Begin Marco ##### //
//--------- Begin of function CourseArray::save_initial_data ---------//
void CourseArray::save_initial_data() {
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
}

//--------- End of function CourseArray::save_initial_data ---------//
// ##### End Marco ##### //

#ifdef DEBUG

//------- Begin of function CourseArray::operator[] -----//

Course* CourseArray::operator[](int recNo) {
    Course* coursePtr = (Course*) get(recNo);

    if( !coursePtr )
	err.run( "CourseArray[] is deleted" );

    return coursePtr;
}

//--------- End of function CourseArray::operator[] ----//
#endif

int CourseArray::is_deleted(int recNo) {
    Course* coursePtr = (Course*) get(recNo);

    if(!coursePtr || !coursePtr->course_recno)
	return 1;

    return 0;
}
