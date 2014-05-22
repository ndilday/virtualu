//Filename   : OSTUD_SC.CPP
//Description: Student selecting courses functions
// 1102 // see  td2.2 sections 5.2-5.4 and td3.3 section 3.2

#include <OSYS.H>
#include <OMATH.H>
#include <OSTUDENT.H>
#include <OFACULTY.H>
#include <OCOURSE.H>
#include <OPSCHOOL.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OFIELD.H>
#include <OCOURRES.H>
#include <OFINANCE.H>

const int RECORD_SEPARATOR_LEN = 2;
static char *RECORD_SEPARATOR = "\r\n";
const int FIELD_SEPARATOR_LEN = 1;
static char *FIELD_SEPARATOR = "\t";

static char *export_num(int value) {
    static char num_str[20];
    return itoa( value, num_str, 10 );              // 10 is radix
}

static char *export_num(double value) {
    static char num_str[20];
    return gcvt( value, 7, num_str );               // a few more more than 7, for negative sign, decimal point and exponent
}

static char *export_percent(double value) {       // value already multiplied by 100
    static char num_str[20];
    return gcvt( value*0.01, 7, num_str );          // a few more more than 7, for negative sign, decimal point and exponent
}

#define WRITE_FIELD_SEPARATOR(f)      f->file_write(FIELD_SEPARATOR, FIELD_SEPARATOR_LEN)
#define WRITE_STR_FIELD(f,s)          { f->file_write(s, strlen(s)); }
#define WRITE_NUM_FIELD(f,n)          { char *s=export_num(n); f->file_write(s, strlen(s)); }
#define WRITE_PERCENT_FIELD(f,n)      { char *s=export_percent(n); f->file_write(s, strlen(s)); }
#define WRITE_RECORD_SEPARATOR(f)     f->file_write(RECORD_SEPARATOR, RECORD_SEPARATOR_LEN)

//------- define static variables --------------//

static char total_course_this_trimester;          // total course to take this trimester
static char total_required_course_this_trimester;

//---------- Begin of function Student::select_course -----------//
//!
//! [int] ignoreFaculty - if this is true, courses will be assigned
//!                       without faculty constraints. This is for
//!                       initialization only.
//!                       (default: 0)
//! [int] initPrevFlag -  don't test or increase student count, denial count in course
//!
void Student::select_course(int ignoreFaculty, int initPrevFlag)
{
    // called by StudentArray::select_course()

    think_summer_course();

    //--- reset counters before starting the selection process ---//

    this->course_denials_this_trimester = 0;
    this->course_failures_this_trimester = 0;
    memset(course_taking_array, 0, sizeof(course_taking_array));

    total_course_this_trimester=0;
    total_required_course_this_trimester=0;

    //--------------------------------------------//

    int loopCount=0;
    int oldDept = department_recno;

    while(1) {
	if( has_selected_all_course() )
	    return;

	//sys.yield();

	if ( oldDept == 0 && department_recno != 0 ) {
	    return;                                     // since think_change_major(1) called
	}

	loopCount++;
	err_when( loopCount > 1000 );

	//---- first attempts to select a required course ----//

	int isRequiredCourse=1;

	if( select_course2( isRequiredCourse, ignoreFaculty, initPrevFlag ) )
	    {
		//			info.debug_enroll5++;
		continue;                                 // select next course until no course can be selected
	    }
	//sys.yield();

	//--- if the first attempt failed and it is a required course, convert it to an elective course ---//

	if( isRequiredCourse ) {
	    isRequiredCourse = 0;

	    if( select_course2( isRequiredCourse, ignoreFaculty, initPrevFlag ) )
		{
		    //				info.debug_enroll6++;
		    continue;
		}
	}

	if (loopCount>5) {                            // 1118   //try making a bigger loop
	    //		if (loopCount>100)	// 1118   //try making a bigger loop 118
	    return;
	}

    }

    /*
      if(total_course_this_trimester<4) //## chea 270899
      info.debug_enroll4++;
      else if (total_course_this_trimester<3)
      info.debug_enroll5++;
      else if (total_course_this_trimester<2)
      info.debug_enroll6++;
      else if (total_course_this_trimester<1)
      info.debug_enroll7++;
    */

}

//----------- End of function Student::select_course ------------//

//---------- Begin of function Student::select_course2 -----------//
//!
//! <int&> isRequiredCourse - if the passed value is 1, the function will
//!                           attempt to select a required course first,
//!                           if it fails, it will then select an elective
//!                           course. If the passed value is 0, it will
//!                           attemp to select an elective course in the
//!                           first place.
//!
//!                           this var is also used for returning
//!                           whether a required course or an elective
//!                           course is selected.
//!
//! <int> ignoreFaculty - if this is true, courses will be assigned
//!                       without faculty constraints. This is for
//!                       initialization only.
//! <int> initPrevFlag -  don't test or increase student count, denial count in course
//!
//! return : <int> 1 - success
//!                0 - failure
//!
int Student::select_course2(int& isRequiredCourse, int ignoreFaculty, int initPrevFlag)
{

    int selectedCourseDept=0, selectedCourseFocus=0, selectedCourseDepth=0;
    //bool allSelected = false;

    //------- attempt to select a required course -------//

    if( isRequiredCourse ) {

	if(this->department_recno ==0)
	    int sdgf=0;

	select_required_course(selectedCourseDept, selectedCourseDepth, selectedCourseFocus);
	//		info.debug_enroll++;
	//		if(selectedCourseDept!=8 && selectedCourseDept!=7 && selectedCourseDept!=10
	//			&& selectedCourseDept!=9 && selectedCourseDept!=2 && selectedCourseDept!=4
	//			&& selectedCourseDept!=0 && selectedCourseDept!=1 && selectedCourseDept!=5
	//			&& selectedCourseDept!=3 && selectedCourseDept!=6)
	//			int gfg=1;

	//		info.debug[selectedCourseDept]++;
    }
    else {
	select_elective_course(selectedCourseDept, selectedCourseDepth, selectedCourseFocus);
	//		info.debug_enroll2++;
	//		info.debug1[selectedCourseDept]++;
    }

    if( !selectedCourseDept ) {
	//		info.debug_enroll3++;
	return 0;
    }
    //------- attemp to assign this student to a course ------//

    int maxTry=2;                                   // try a maximum of two times to select a teaching method and match a course

    for( int i=0 ; i<maxTry ; i++ ) {
	//-------- select a teaching method -------//

	int selectedTeachingMethod, selectedTeachingMethod2= -1;

	selectedTeachingMethod = select_teaching_method(selectedCourseDept);

	//      if( selectedTeachingMethod  == CLASS_WITH_BREAKOUT )
	//      {
	//         selectedTeachingMethod2 = BREAKOUT_LAB;      //**BUGHERE, selectedTeachingMethod2 hasn't be taken care of yet
	//      }

	//sys.yield();

	if( assign_course(selectedCourseDept, selectedCourseDepth,
			  selectedCourseFocus, selectedTeachingMethod, isRequiredCourse, ignoreFaculty, initPrevFlag) )
	    {
		//			info.debug[selectedCourseDept]++;
		return 1;
	    }
	    else {
		//			this->course_denials_this_trimester = 1;			// 990415
		//			this->course_denials_this_trimester++;
		this->course_denials_this_trimester += 1;   //## chea may be this is right

	    }
    }

    return 0;
}

//---------- End of function Student::select_course2 -----------//

//-------- Begin of function Student::assign_course --------//
//!
//! [int] initPrevFlag -  don't test or increase student count, denial count in course
//!
//! return: 1 - success
//!         0 - failure
//!
int Student::assign_course(int selectedCourseDept, int selectedCourseDepth,
			   int selectedCourseFocus, int selectedTeachingMethod,
			   int isRequiredCourse, int ignoreFaculty, int initPrevFlag)
{
    // init var

    //if ( !ignoreFaculty && math.get_random_float() > player_school.delay_rate[student_level] )
    //	return 0;

    //-------- supply-demand balance --------//

    Department* deptPtr = department_array[selectedCourseDept];

    Course* coursePtr = deptPtr->course_array.get_course(selectedCourseDepth, selectedCourseFocus, selectedTeachingMethod);

    if ( audit_flag ) {
	File file;

	char fileName[123];

	switch( student_level ) {
	case UG_TRADITION:    strcpy( fileName, "TRADITION" );
	    break;
	case UG_NONTRADITION: strcpy( fileName, "NONTRADITION" );
	    break;
	case MASTER:        strcpy( fileName, "MASTER" );
	    break;
	case DOCTOR:        strcpy( fileName, "DOCTOR" );
	    break;
	}

	strcat( fileName, m.format(old_student_recno) );
	strcat( fileName, ".txt" );

	file.file_append( fileName );
	file.file_seek(0, FILE_END);

	file_course_id++;

	for ( int z=0; z<6; z++ ) {
	    // Cohet
	    if ( info.prerun_year ) {
		WRITE_NUM_FIELD( (&file), -1 );
	    }
	    else {
		WRITE_NUM_FIELD( (&file), info.financial_year() );
	    }

	    WRITE_FIELD_SEPARATOR( (&file) );

	    // Trimester
	    switch( player_school.cur_trimester ) {
	    case AUTUMN:  WRITE_STR_FIELD( (&file), "A" ); break;
	    case WINTER:  WRITE_STR_FIELD( (&file), "W" ); break;
	    case SUMMER:  WRITE_STR_FIELD( (&file), "S" ); break;
	    }

	    // ID
	    WRITE_FIELD_SEPARATOR( (&file) );
	    WRITE_FIELD_SEPARATOR( (&file) );
	    WRITE_NUM_FIELD( (&file), file_course_id );
	    WRITE_FIELD_SEPARATOR( (&file) );

	    // Variables
	    switch(z) {
	    case 0:                                   // CourseDept
		WRITE_STR_FIELD( (&file), "CourseDept" );
		WRITE_FIELD_SEPARATOR( (&file) );
		WRITE_STR_FIELD( (&file), department_res[department_array[coursePtr->department_recno]->department_id]->name );
		WRITE_RECORD_SEPARATOR( (&file) );
		break;

	    case 1:                                   // Depth
		WRITE_STR_FIELD( (&file), "Depth" );
		WRITE_FIELD_SEPARATOR( (&file) );
		WRITE_FIELD_SEPARATOR( (&file) );
		switch( selectedCourseDepth ) {
		case DEPTH_D1:  WRITE_STR_FIELD( (&file), "D1" ); break;
		case DEPTH_D2: WRITE_STR_FIELD( (&file), "D2" ); break;
		case DEPTH_D3: WRITE_STR_FIELD( (&file), "D3" ); break;
		case DEPTH_GR: WRITE_STR_FIELD( (&file), "GR" ); break;
		}
		WRITE_RECORD_SEPARATOR( (&file) );
		break;

	    case 2:                                   // Focus
		WRITE_STR_FIELD( (&file), "Focus" );
		WRITE_FIELD_SEPARATOR( (&file) );
		WRITE_FIELD_SEPARATOR( (&file) );
		switch( selectedCourseFocus ) {
		case FOCUS_LD: WRITE_STR_FIELD( (&file), "LD" ); break;
		case FOCUS_M:  WRITE_STR_FIELD( (&file), "M" ); break;
		case FOCUS_UD: WRITE_STR_FIELD( (&file), "UD" ); break;
		case FOCUS_GR: WRITE_STR_FIELD( (&file), "GR" ); break;
		}
		WRITE_RECORD_SEPARATOR( (&file) );
		break;

	    case 3:                                   // TchMethod
		WRITE_STR_FIELD( (&file), "TchMethod" );
		WRITE_FIELD_SEPARATOR( (&file) );
		switch( selectedTeachingMethod ) {
		case SEMINAR:             WRITE_STR_FIELD( (&file), "Sem" ); break;
		case DISTANCE_LEARN_COURSE:   err_here(); break;
		case CLASS_WITH_BREAKOUT:     WRITE_STR_FIELD( (&file), "CLA" ); break;
		case BREAKOUT_LAB:          err_here(); break;
		case GENERAL:             WRITE_STR_FIELD( (&file), "GEN" ); break;
		}
		WRITE_RECORD_SEPARATOR( (&file) );
		break;

	    case 4:                                   // Req/elect
		WRITE_STR_FIELD( (&file), "Req/elect" );
		WRITE_FIELD_SEPARATOR( (&file) );
		if ( isRequiredCourse ) {
		    WRITE_STR_FIELD( (&file), "Req" );
		}
		else
		    WRITE_STR_FIELD( (&file), "Elect" );
		WRITE_RECORD_SEPARATOR( (&file) );
		break;

	    case 5:                                   // Result
		WRITE_STR_FIELD( (&file), "Result" );
		WRITE_FIELD_SEPARATOR( (&file) );
		WRITE_FIELD_SEPARATOR( (&file) );
		break;
	    }
	}

	file.file_close();
    }

    //---- check max_class_size checking here	// see email req29a_course.txt and 0830 notes sectionn 6.5.3 and 6.8

    float maxClassSize = 0;

    if ( coursePtr->teaching_method == DISTANCE_LEARN_COURSE ) {
	maxClassSize = department_res[deptPtr->department_id]->max_dl_class_size;
    }
    else {
	maxClassSize = deptPtr->cummulative_tech_utilization;
	maxClassSize = math.single_response_func(0.0f, 3.0f, 10.0f, 5.0f, maxClassSize/100);

	//trick
	maxClassSize = float(coursePtr->normal_class_size() );
    }

    if( !initPrevFlag && coursePtr->is_closed ) {   // if initPrevFlag, never deny
	//      coursePtr->admission_denied_count++;
	// Version 2.0 Item 8d iv)
	int randomNumber = m.random(100);
	// if the denied is canceled
	if (randomNumber < deptPtr->ease_outsourcing_credits ) {
	    coursePtr->outsourcing_credits = 1;
	    //         coursePtr->cur_section_student_count++;
	    if ( audit_flag ) {
		File file;

		char fileName[123];

		switch( student_level ) {
		case UG_TRADITION:    strcpy( fileName, "TRADITION" );
		    break;
		case UG_NONTRADITION: strcpy( fileName, "NONTRADITION" );
		    break;
		case MASTER:        strcpy( fileName, "MASTER" );
		    break;
		case DOCTOR:        strcpy( fileName, "DOCTOR" );
		    break;
		}

		strcat( fileName, m.format(old_student_recno) );
		strcat( fileName, ".txt" );

		file.file_append( fileName );
		file.file_seek(0, FILE_END);

		// Pass
		WRITE_STR_FIELD( (&file), "Pass" );
		WRITE_RECORD_SEPARATOR( (&file) );

		file.file_close();
	    }
	    return 0;
	}
	coursePtr->admission_denied_count = 1;        // 990415
	if ( audit_flag ) {
	    File file;

	    char fileName[123];

	    switch( student_level ) {
	    case UG_TRADITION:    strcpy( fileName, "TRADITION" );
		break;
	    case UG_NONTRADITION: strcpy( fileName, "NONTRADITION" );
		break;
	    case MASTER:        strcpy( fileName, "MASTER" );
		break;
	    case DOCTOR:        strcpy( fileName, "DOCTOR" );
		break;
	    }

	    strcat( fileName, m.format(old_student_recno) );
	    strcat( fileName, ".txt" );

	    file.file_append( fileName );
	    file.file_seek(0, FILE_END);

	    // denied
	    WRITE_STR_FIELD( (&file), "Denied" );
	    WRITE_RECORD_SEPARATOR( (&file) );

	    file.file_close();
	}
	return 0;
    }

    //--- if a course already exists for the course cell ---//

    int facultyRecno;

    if( initPrevFlag )
	goto assign_course_succeeded;                 //always successful, skip initing course

    if( coursePtr->class_section_count > 0 ) {
	if( coursePtr->cur_section_student_count <=
	    // 1.25 implies that a 25% overload will be allowed before another section is created.
	    (float) coursePtr->normal_class_size() * 1.25 - 1 ) {
	    coursePtr->cur_section_student_count++;

	    goto assign_course_succeeded;
	}
    }

    //---- if no course exists or the current section is already full ----//

    //	if( ignoreFaculty ) // allow course denial in pre-run year (but not initialization)
    // use (info.prerun_year && info.game_year==1) to exclude start of year 1 (info.prerun_year still 1) )
    if( ignoreFaculty || (info.prerun_year && info.game_year==1) )
	facultyRecno = 1;
    else
	facultyRecno = get_available_faculty(selectedCourseDept);

    //--- if a faculty is found, open a new section for this course ---//

    if( facultyRecno ) {
	if( !ignoreFaculty )
	    deptPtr->faculty_array[facultyRecno]->teaching_contact_hour += CONTACT_HOUR_PER_COURSE;

	coursePtr->class_section_count++;
	coursePtr->cur_section_student_count = 1;     // 0112      282

    }
    // 1217
    else if( player_school.cur_trimester == SUMMER ) {
	// pretend to add a dummy phantom faculty by adding cost

	//BUGHERE, this following line will dramatically increase the academic staff expenses and make the financials go crazy. Therefore it is removed.

	//		finance.budget_expense_array[AC_ACADEMIC_DEPARTMENTS].staff += deptPtr->peer_numeraire_salary / 1000.0;
	coursePtr->cur_section_student_count++;       // 0112

    }
    else {                                          //--- no faculty is available for opening a new course section ---//
	if( coursePtr->convention_student_count <=
	    // 1.5 implies that a 33% overload will be allowed before a course is closed.
	    coursePtr->class_section_count * coursePtr->normal_class_size() * 1.5f - 1 ) {
	    coursePtr->cur_section_student_count++;
	}
	else {
	    coursePtr->is_closed=1;                     // 1 if this course is closed. This happens when there is no faculty member available for creating new course section.

	    /*#if(GAME_VERSION>=200)
	      if ( m.random(101) <= deptPtr->ease_outsourcing_credits )		// if canceled denied request
	      {
              deptPtr->outsourcing_credits++;
              coursePtr->cur_section_student_count++;

              goto assign_course_succeeded;
	      }
	      #endif*/

	    //coursePtr->admission_denied_count++;
	    // Version 2.0 Item 8d iv)
	    int randomNumber = m.random(100);
	    // if the denied is canceled
	    if (randomNumber < deptPtr->ease_outsourcing_credits ) {
		coursePtr->outsourcing_credits = 1;
		//         coursePtr->cur_section_student_count++;
		if ( audit_flag ) {
		    File file;

		    char fileName[123];

		    switch( student_level ) {
		    case UG_TRADITION:    strcpy( fileName, "TRADITION" );
			break;
		    case UG_NONTRADITION: strcpy( fileName, "NONTRADITION" );
			break;
		    case MASTER:        strcpy( fileName, "MASTER" );
			break;
		    case DOCTOR:        strcpy( fileName, "DOCTOR" );
			break;
		    }

		    strcat( fileName, m.format(old_student_recno) );
		    strcat( fileName, ".txt" );

		    file.file_append( fileName );
		    file.file_seek(0, FILE_END);

		    // Pass
		    WRITE_STR_FIELD( (&file), "Pass" );
		    WRITE_RECORD_SEPARATOR( (&file) );

		    file.file_close();
		}
		return 0;
	    }
	    coursePtr->admission_denied_count = 1;      // 990415
	    if ( audit_flag ) {
		File file;

		char fileName[123];

		switch( student_level ) {
		case UG_TRADITION:    strcpy( fileName, "TRADITION" );
		    break;
		case UG_NONTRADITION: strcpy( fileName, "NONTRADITION" );
		    break;
		case MASTER:        strcpy( fileName, "MASTER" );
		    break;
		case DOCTOR:        strcpy( fileName, "DOCTOR" );
		    break;
		}

		strcat( fileName, m.format(old_student_recno) );
		strcat( fileName, ".txt" );

		file.file_append( fileName );
		file.file_seek(0, FILE_END);

		// denied
		WRITE_STR_FIELD( (&file), "Denied" );
		WRITE_RECORD_SEPARATOR( (&file) );

		file.file_close();
	    }

	    return 0;                                   // admission denied, choice of D-DF-TM has failed.
	}
    }

    //--- increase counters as the course has been successfully assigned ---//

 assign_course_succeeded:

    if( !initPrevFlag ) {
	coursePtr->convention_student_count++;
    }

    total_course_this_trimester++;                  // increase this regardless of whether the student has passed the course or not, as it is for determining whether the student has taken all the required courses in a trimester

    if( isRequiredCourse )
	total_required_course_this_trimester++;
    else
	total_elective_course_this_year++;

    //	course_taking_array[selectedCourseDept-1]++;

    if( study_course(selectedCourseDept) ) {        // return 1 if the student has passed the course
	//	if(1)    //##testing
	if ( audit_flag ) {
	    File file;

	    char fileName[123];

	    switch( student_level ) {
	    case UG_TRADITION:    strcpy( fileName, "TRADITION" );
		break;
	    case UG_NONTRADITION: strcpy( fileName, "NONTRADITION" );
		break;
	    case MASTER:        strcpy( fileName, "MASTER" );
		break;
	    case DOCTOR:        strcpy( fileName, "DOCTOR" );
		break;
	    }

	    strcat( fileName, m.format(old_student_recno) );
	    strcat( fileName, ".txt" );

	    file.file_append( fileName );
	    file.file_seek(0, FILE_END);

	    // Pass
	    WRITE_STR_FIELD( (&file), "Pass" );
	    WRITE_RECORD_SEPARATOR( (&file) );

	    file.file_close();
	}
	// increment the student's course to_take counter
	course_taken_array[selectedCourseDept-1][selectedCourseDepth]++;

	total_course_all++;                           // only increase the total course count if the student has passed the course
	// if the student failed the course, the counters will not be incremented

	// no need test initPrevFlag, as course_taking_array will be clear in next select_course
	course_taking_array[selectedCourseDept-1]++;

	//---------------------------------------------//
	//
	// Select major for undecided students right after
	// they have passed their 8th course. (that's
	// 2 semesters worth of courses for traditional
	// undergraduates
	//
	//---------------------------------------------//

	if( department_recno==0 && total_course_all >= 8 ) {
	    think_change_major(1);                      // 1: course completed
	    return 1;                                   // this dummy student is removed!
	    //err_when( department_recno < 1 || department_recno > department_array.size() );
	}
    }
    else {
	if ( audit_flag ) {
	    File file;

	    char fileName[123];

	    switch( student_level ) {
	    case UG_TRADITION:    strcpy( fileName, "TRADITION" );
		break;
	    case UG_NONTRADITION: strcpy( fileName, "NONTRADITION" );
		break;
	    case MASTER:        strcpy( fileName, "MASTER" );
		break;
	    case DOCTOR:        strcpy( fileName, "DOCTOR" );
		break;
	    }

	    strcat( fileName, m.format(old_student_recno) );
	    strcat( fileName, ".txt" );

	    file.file_append( fileName );
	    file.file_seek(0, FILE_END);

	    // Fail
	    WRITE_STR_FIELD( (&file), "Fail" );
	    WRITE_RECORD_SEPARATOR( (&file) );

	    file.file_close();
	}
	if( !initPrevFlag )
	    coursePtr->failure_count++;
	this->failed_course_count++;
	// no need test initPrevFlag, as course_taking_array will be clear in next select_course
	this->course_failures_this_trimester++;
    }

    return 1;
}

//-------- End of function Student::assign_course --------//

//--------- Begin of function Student::get_available_faculty ---------//
//!
//! Get a faculty available for teaching a new course section.
//!
int Student::get_available_faculty(int departmentRecno) {
    Department* deptPtr = department_array[departmentRecno];

    Faculty* facultyPtr;
    int      facultySize = deptPtr->faculty_array.size();
    // randomly pick a faculty to start the scanning
    int      facultyRecno=m.random( facultySize ) + 1;

    for( int i=facultySize ; i>0 ; i-- ) {
	if( ++facultyRecno > facultySize )
	    facultyRecno = 1;

	if( deptPtr->faculty_array.is_deleted(facultyRecno) )
	    continue;

	facultyPtr = deptPtr->faculty_array[facultyRecno];

	if( NORMAL_TEACHING_CONTACT_HOUR - facultyPtr->teaching_contact_hour
	    >= CONTACT_HOUR_PER_COURSE ) {
	    return facultyRecno;
	}
    }

    return 0;                                       //0112
}

//--------- End of function Student::get_available_faculty -----------//

//-------- Begin of function Student::select_required_course --------//
//!
//! The following vars are all for returning values.
//!
//! <int&> selectedCourseDept  - the department recno of the selected course
//! <int&> selectedCourseDepth - the course depth of the selected course
//! <int&> selectedCourseFocus - the course focus of the selected course
//!
//! return: <int> 1 - a course has been selected
//!               0 - the student has selected all required courses,
//!                   there is no additional ones required.
//!
int Student::select_required_course(int &selectedCourseDept, int& selectedCourseDepth, int &selectedCourseFocus) {
    //----------------------------------------------------------//
    //
    // Calculate the number of required courses that must be taken.
    // It is 4 (a constant) minus the minimum number of elective
    // courses that must be taken.
    //
    //----------------------------------------------------------//

    GeneralDepartment* studentDept;

    if( department_recno==0 )                       // if the student's major is undecided, the student is currently studying general education
	studentDept = &(department_res.general_dept);
    else
	// the department where the student is currently majoring at
	studentDept = department_array[department_recno];

    DepartmentInfo* studDeptInfo = department_res[studentDept->department_id];

    // master students do not study general education
    err_when( studentDept->department_id==0 && (student_level == MASTER || student_level == DOCTOR) );

    //------------------------------------------------------//

    int neededElectCourse=0;

    if( student_level == MASTER ) {
	if( year_in_program<=MASTER_YEAR_COUNT )      // master program only has one year
	    neededElectCourse = studDeptInfo->master_elective_year_count;
    }
    else if( is_ug_group(student_level) ) {
	if( year_in_program<=BACHELOR_YEAR_COUNT )
	    neededElectCourse = studDeptInfo->bachelor_elective_year_count[year_in_program-1];
    }

    //---------------------------------------------------//
    //
    // Since the sim only stores the min number of elective
    // courses in a year, we need to break it down into
    // a trimester basis. If the current trimester is the
    // first trimester, it will divide the total elective
    // courses in a year by 2 (for a trimester). If it is
    // in other trimester, it will get the remaining
    // elective courses that have yet been taken.
    //
    //---------------------------------------------------//

    if( player_school.cur_trimester == FIRST_TRIMESTER )
	neededElectCourse /= 2;
    else
	// minus the total number of elective courses that have been taken gives the total number of remaining elective courses that must be taken
	neededElectCourse -= total_elective_course_this_year;

    int neededRequiredCourse = course_per_trimester[player_school.cur_trimester] - neededElectCourse;

    //err_when( neededRequiredCourse <= 0 );    // this function should have been called at all if it <= 0

    if ( neededRequiredCourse <= 0 )
	return 0;

    if ( total_required_course_this_trimester == neededRequiredCourse )
	return 0;

    err_when( total_required_course_this_trimester > neededRequiredCourse );

    //------ reckon the remaining courses of this student in each department ---------//

    int remainCourse;
    int highestDepth;
    int mostRemainCourse=0;
    int selectedCourseHighestDepth = 0;
    int deptCount = department_array.size();

    selectedCourseDept=0;                           // it's a function parameter

    static char bachelorCourseDepth[] = {  DEPTH_D1, DEPTH_D2, DEPTH_D3 };
    static char masterCourseDepth[] = { DEPTH_D3, DEPTH_GR };
    static char doctorCourseDepth[] = { DEPTH_GR };

    char* courseDepthArray;
    int   courseDepthCount;
    char  courseDepth;

    switch( student_level ) {
    case UG_TRADITION:
    case UG_NONTRADITION:
    case DISTANCE_LEARN:
	courseDepthArray = bachelorCourseDepth;
	courseDepthCount = 3;
	break;

    case MASTER:
	courseDepthArray = masterCourseDepth;
	courseDepthCount = 2;
	break;

    case DOCTOR:
	courseDepthArray = doctorCourseDepth;
	courseDepthCount = 1;
	break;

    default:
	err_here();
    }

    //-------------------------------------------------------------//
    /*
      //  990415 for debug only
      {
      int total = 0;

      for( int deptRecno=1 ; deptRecno<=deptCount ; deptRecno++ )
      for( int i=0 ; i<courseDepthCount; i++ )
      {
      courseDepth = courseDepthArray[i];
      total += course_taken_array[deptRecno-1][courseDepth];
      }

      total += 0;
      }
    */

    //----//
    remainCourse = 0;

    // check for the department has mostRemainCourse
    int mostDept=0;
	int deptRecno;
    for ( deptRecno=1; deptRecno<=deptCount ; deptRecno++ ) 
	{
		for( int i=0 ; i<courseDepthCount; i++ ) 
		{
			int remainCourseinit = 0;                   //## chea 291199
			courseDepth = courseDepthArray[i];

			remainCourse += studentDept->get_course_requirement(student_level, deptRecno, courseDepth)
			- course_taken_array[deptRecno-1][courseDepth];
		}

		if ( remainCourse >= mostRemainCourse ) 
		{
			mostRemainCourse = remainCourse;
			mostDept = deptRecno;
		}
    }

    if ( mostDept == 0 )
	{
		return 0;
	}

    // check for the maximum depth largest number of courses
    for ( int i=0; i<courseDepthCount; i++ ) 
	{
		courseDepth = courseDepthArray[i];

		remainCourse += studentDept->get_course_requirement(student_level, mostDept, courseDepth)
			- course_taken_array[deptRecno-1][courseDepth];

		if( remainCourse > 0 )                        // the highest course depth of remaining courses needed to be taken in this department
			highestDepth = courseDepth;
    }

    selectedCourseDept  = mostDept;
    selectedCourseHighestDepth = highestDepth;

    //## chea 291199 test and this work to control the course no.
    //	selectedCourseDept = this->department_recno;
    //	info.debug[selectedCourseDept]++;

    if( selectedCourseDept==0 )                     // no department is selected
	return 0;

    //---------- select course depth ------------//

    int totalCourseTaken = total_course_taken_dept(selectedCourseDept);

    if( totalCourseTaken <=2 )
	selectedCourseDepth = DEPTH_D1;

    else if( totalCourseTaken <=4 )
	selectedCourseDepth = DEPTH_D2;

    else
	selectedCourseDepth = DEPTH_D3;

    //--------- select course focus ---------//

    switch( selectedCourseDepth ) {                 // updated by WFM notes 0909
    case DEPTH_D1:
	if( total_course_all < 9 ) {
	    selectedCourseFocus = FOCUS_LD;
	}
	else if( total_course_all < 16 ) {
	    selectedCourseFocus = (m.random(2)==0 ? FOCUS_LD : FOCUS_M );
	}
	else if( total_course_all < 24 ) {
	    selectedCourseFocus = (m.random(2)==0 ? FOCUS_M : FOCUS_UD );
	}
	else
	    selectedCourseFocus = FOCUS_UD;
	break;

    case DEPTH_D2:
	if( total_course_all < 16 ) {
	    selectedCourseFocus = FOCUS_M;
	}
	else if ( total_course_all < 24 ) {
	    selectedCourseFocus = (m.random(2)==0 ? FOCUS_M : FOCUS_UD );
	}
	else
	    selectedCourseFocus = FOCUS_UD;
	break;

    case DEPTH_D3:
	selectedCourseFocus = FOCUS_UD;
	break;
    }

    return 1;
}

//-------- End of function Student::select_required_course --------//

//-------- Begin of function Student::select_elective_course --------//
//!
//! The following vars are all for returning values.
//!
//! <int&> selectedCourseDept  - the department recno of the selected course
//! <int&> selectedCourseDepth - the course depth of the selected course
//! <int&> selectedCourseFocus - the course focus of the selected course
//!
void Student::select_elective_course(int &selectedCourseDept, int& selectedCourseDepth, int &selectedCourseFocus) {
    //------------------------------------------------------------//
    //
    // A student can take as many elective courses as he/she wants
    // as long as he/she has taken all the required courses.
    //
    //------------------------------------------------------------//

    //------ select the department of the elective course ------//

    int deptPref;
    float totalPref = (float) 0;
    int deptCount = department_array.size();

    GeneralDepartment* studentDept;

    if( department_recno==0 )                       // if the student's major is undecided, the student is currently studying general education
	studentDept = &(department_res.general_dept);
    else
	// the department where the student is currently majoring at
	studentDept = department_array[department_recno];

    // if studentDept->department_id is 0, it will return department_res.general_dept_info
    DepartmentInfo* studDeptInfo = department_res[studentDept->department_id];

    err_when( department_recno==0 && studDeptInfo != &(department_res.general_dept_info));

    // master students do not study general education
    err_when( studentDept->department_id==0 && (student_level == MASTER || student_level == DOCTOR) );

    selectedCourseDept = 0;

    //---- get the total preference weights of all existing departments and draw a random number based on it ----//

    if( student_level == MASTER )
	deptPref = m.random( (int) studDeptInfo->total_master_elective_course_pref );
    else
	deptPref = m.random( (int) studDeptInfo->total_bachelor_elective_course_pref );

    //------ see which department the random number points to ----//

    for( int i=department_array.size() ; i>0 ; i-- ) {
	if( department_array.is_deleted(i) )
	    continue;

	int deptId = department_array[i]->department_id;

	if( student_level == MASTER )
	    totalPref += studDeptInfo->master_elective_course_pref[deptId-1];
	else
	    totalPref += studDeptInfo->bachelor_elective_course_pref[deptId-1];

	if( totalPref > deptPref ) {                  //## fred BUGHER should be ">" ##//
	    //## chea 291199 the =1 below should be wrong
	    //			selectedCourseDept = 1;
	    selectedCourseDept = i;
	    break;

	}

    }

    err_when( department_recno==0 && studDeptInfo != &(department_res.general_dept_info));
    err_when( selectedCourseDept==0 );

    //------- now select the course focus --------//

    int        selectedCourseDeptId = department_array[selectedCourseDept]->department_id;
    FieldInfo* selectedCourseField = field_res[ department_res[selectedCourseDeptId]->field_id ];
    int        focusPref = m.random(100);           // preference on course focus. Draw a random number

    totalPref = 0;

    for( int i=0 ; i<MAX_COURSE_FOCUS_WITHOUT_GR ; i++ ) {
	totalPref += selectedCourseField->elective_course_focus_pref[i];

	if( focusPref < totalPref ) {
	    selectedCourseFocus = i;

	    //---- if the selected focus is LD but the student is not in LD, pick between M and UD randomly ----//

	    if( selectedCourseFocus==FOCUS_LD &&
		division_status() != LOWER_DIVISION ) {
		if( m.random(2)==0 )
		    selectedCourseFocus = FOCUS_M;
		else
		    selectedCourseFocus = FOCUS_UD;
	    }

	    //---- if the selected focus is UD but the student is not in UD, pick between M and UD randomly ----//

	    if( selectedCourseFocus==FOCUS_UD &&
		division_status() != UPPER_DIVISION ) {
		if( m.random(2)==0 )
		    selectedCourseFocus = FOCUS_M;
		else
		    selectedCourseFocus = FOCUS_LD;
	    }

	    break;
	}
    }

    err_when( selectedCourseFocus<0 );

    //--------- select course depth --------//

    int totalCourseTaken = total_course_taken_dept(selectedCourseDept);

    switch( selectedCourseFocus ) {
    case FOCUS_LD:
	selectedCourseDepth = DEPTH_D1;
	break;

    case FOCUS_M:
	if( totalCourseTaken <= 2 )
	    selectedCourseDepth = DEPTH_D1;
	else
	    selectedCourseDepth = DEPTH_D2;
	break;

    case FOCUS_UD:
	if( totalCourseTaken <= 2 )
	    selectedCourseDepth = DEPTH_D1;
	if( totalCourseTaken <= 4 )
	    selectedCourseDepth = DEPTH_D2;
	else
	    selectedCourseDepth = DEPTH_D3;
	break;
    }

    //	info.debug[selectedCourseDept]++;
}

//------- End of function Student::select_elective_course ---------//

//------ Begin of function Student::select_teaching_method --------//
//!
int Student::select_teaching_method(int selectedCourseDept) {
    // modified 1105 // see req29a_course.txt

    if ( student_level == DISTANCE_LEARN )          // dl student can take dl course only
	return DISTANCE_LEARN_COURSE;

    int teachingPref = m.random(100);               // preference on department. Draw a random number now
    int totalPref = 0;

    Department *deptPtr = department_array[selectedCourseDept];

    const char pref[MAX_TEACHING_METHOD]
	= {
	    deptPtr->course_preference[P_SEMINAR],
	    0, deptPtr->course_preference[P_CLASS_WITH_BREAKOUTS],
	    0, deptPtr->course_preference[P_OTHER]
	};

    for( int i=0 ; i<MAX_TEACHING_METHOD; i++ ) {
	totalPref += pref[i];

	if( teachingPref < totalPref ) {
	    err_when(DISTANCE_LEARN_COURSE == i );      // only dl student can take dl course
	    return i;
	}
    }

    err_here();

    return 0;
}

//---------- End of function Student::select_teaching_method -----------//

//-------- Begin of function Student::total_course_taken_dept --------//
//!
//! Return the total number of courses taken by this student in the given
//! department.
//!
int Student::total_course_taken_dept(int departmentRecno) {
    int totalCourse=0;

    for( int i=0 ; i<MAX_COURSE_DEPTH ; i++ )
	totalCourse += course_taken_array[departmentRecno-1][i];

    return totalCourse;
}

//-------- End of function Student::total_course_taken_dept --------//

//---------- Begin of function Student::study_course -----------//
//!
//! It simulates the process of the student studying the course,
//! updating the student's academic achievement.
//!
//! <int> studyDeptRecno - the department in which the studying is studying this course.
//!
//! Return: <int> 1 - the student has passed the course
//!               0 - the student has failed the course
//!
int Student::study_course(int studyDeptRecno) {
    //----- calculate the academic achievement rating first ----//

    Department* deptPtr = department_array[studyDeptRecno];

    //--- calculate the mean academic achievement ---//

    float meanAcadAch = 150 - (deptPtr->p_educational_quality+talent_academic) / 4
	- (100-deptPtr->p_educational_quality)*(100-talent_academic) / 100;

    meanAcadAch = min( meanAcadAch, 100.0f );       // min & max bug chea

    //---- calculate the standard deviation -----//

    float stdDevAcadAch = (float) sqrt( meanAcadAch * (100-meanAcadAch)/100 );

    //---- obtain the academic achievement ------//

    float courseAcadAch = math.get_random_snd(meanAcadAch, PSCH_SD(stdDevAcadAch));

    courseAcadAch = min(courseAcadAch, 100.0f);     // min & max bug chea

    //------- update the student's overall academic achievement rating -----//

    academic_achievement = float(academic_achievement * total_course_all + courseAcadAch)
	/ (total_course_all + 1);

    //------- calculate the Probability that a student will fail a course -----//
    //
    // rob_Course_Failure =
    // SingleResponseFunction[student_academic_talent,
    //     floor=50,ceiling=0,b1=100,m1=50]  *
    // SingleResponseFunction[departmental_educational_quality,
    //     floor=100,ceiling=0,b1=100,m1=50]/100
    //
    // The student talent and education quality numbers are in the 0-100 range.
    // Prob_Course_Failure will equal 0 if both variables are at 100 and about 40 if
    // both are at 0. Draw a random number between 0 and 100 and fail the student if
    // it is less than or equal to Prob_Course_Failure.
    //
    //--------------------------------------------------------------------------//
    /*
      float probCourseFail = math.single_response_func(50.0f, 0.0f, 100.0f, 50.0f, talent_academic) *
      math.single_response_func(100.0f, 0.0f, 100.0f, 50.0f, deptPtr->p_educational_quality) / 100;

      return m.random(101) > probCourseFail;
    */
    float newValue = 40+(talent_academic)*(.2f + .8f * (deptPtr->p_educational_quality-50)/100);

    // 0406
    // float newValue = 0.1f * (1 - ( 0.25f * talent_academic + 0.75f * deptPtr->p_educational_quality) / 100.0f );

    newValue = math.single_response_func(0.0f, 0.1f, -70.11f, 53.65f, newValue);

    return math.get_random_float() >= newValue;     // newValue equals to fail_rate
}

//----------- End of function Student::study_course ------------//

//---------- Begin of function Student::has_selected_all_course -----------//
//!
int Student::has_selected_all_course() {
    return total_course_this_trimester >= course_per_trimester[player_school.cur_trimester];
}

//----------- End of function Student::has_selected_all_course ------------//

//---------- Begin of function Student::think_summer_course -----------//
//!
//! see section 7 of design_notes0805: course demand
//!
void Student::think_summer_course() {
    //if ( player_school.cur_trimester != SUMMER )
    //	return;

    //--------------//

    err_when(player_school.summer_emphasis > 3);

    char remap[] = { 0,1,2,2 };
    char t=0;
    char prob = m.random(100);
	int i;
    for ( i=0; i<SUMMER_COURSE_COUNT; i++ ) 
	{
		t += course_res.summer_course_pref[student_level][remap[player_school.summer_emphasis]][ i ];

		if ( prob <= t )
			break;
    }

    if( i>=SUMMER_COURSE_COUNT )                    // BUGHERE, these lines are for fixing bug on-fly
	i = SUMMER_COURSE_COUNT-1;

    err_when(i>=SUMMER_COURSE_COUNT);               // i must be < SUMMER_COURSE_COUNT;

    //-------------// we found i as the number of courses for off_quarter

    course_per_trimester[off_quarter] = i;
}

//----------- End of function Student::think_summer_course ------------//

//---------- Begin of function FacultyArray::think_summer_teaching -----------//
//!
//! see section 7 of design_notes0805: course supply
//!
void FacultyArray::think_summer_teaching() {
    // called per summer

    Faculty *fPtr;
    Department *deptPtr = department_array[department_recno];
    int sum=0, i;

    for (i=deptPtr->course_array.size(); i>0; i--) {
	sum += deptPtr->course_array[i]->class_section_count;
    }

    for (i=size(); i>0; i--) {
	fPtr = operator[](i);
	fPtr->off_duty_trimester = SUMMER;
	fPtr->is_third_trimester_teaching = 0;
    }

    int targetSections = ( sum * player_school.percent_summer_teaching_load ) / 100;
    char  tri = AUTUMN;
    int sections=0;

    while ( sections < targetSections ) {
	int recno = m.random(faculty_count) + 1;
	fPtr = operator[](recno);

	if ( targetSections < faculty_count && fPtr->is_third_trimester_teaching )
	    continue;

	// shift course load to summer
	fPtr->off_duty_trimester = tri;
	fPtr->is_third_trimester_teaching = 1;

	// for next loop
	sections++;
	tri = (tri==AUTUMN) ? WINTER : AUTUMN;
    }
}

//----------- End of function FacultyArray::think_summer_teaching ------------//
