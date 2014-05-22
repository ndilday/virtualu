//Filename    : OSTUDENA.CPP
//Description : Student Array Class Definition

#include <OSYS.H>

#include <OINFO.H>
#include <OPSCHOOL.H>
#include <ODEPT.H>
#include <OFINANCE.H>
#include <OSTUDENT.H>
#include <OENROLL.H>

//----------- Begin of function StudentArray Constructor -----//
//!
StudentArray::StudentArray() : DynArrayB(sizeof(Student), 200, DEFAULT_REUSE_INTERVAL_DAYS) {
    student_count=0;

    memset(talent_academic_all, 0, sizeof(talent_academic_all));
    memset(performance_academic_ug, 0, sizeof(performance_academic_ug));

    memset(talent_academic, 0, sizeof(talent_academic));
    memset(talent_extracurricular, 0, sizeof(talent_academic));
    memset(talent_athletics, 0, sizeof(talent_academic));

    memset(satisfaction_academic, 0, sizeof(talent_academic));
    memset(satisfaction_student_life, 0, sizeof(talent_academic));
    memset(satisfaction_athletics, 0, sizeof(talent_academic));
    memset(satisfaction_overall, 0, sizeof(satisfaction_overall));

    memset(time_to_degree, 0, sizeof(time_to_degree));
    memset(dropout_rate, 0, sizeof(dropout_rate));

    // ### begin Gilbert 05/06/2001 #####//
    memset(fake_disp_time_to_degree, 0, sizeof(fake_disp_time_to_degree) );
    // ### end Gilbert 05/06/2001 #####//

    memset(bachelor_degree,0,sizeof(doctor_degree));
    memset(non_ug_bachelor_degree,0,sizeof(doctor_degree));
    memset(master_degree,0,sizeof(doctor_degree));
    memset(doctor_degree,0,sizeof(doctor_degree));

    memset(time_to_degree_cumm,0,sizeof(time_to_degree_cumm));
    memset(cur_dropout,0,sizeof(cur_dropout));
    memset(ave_entering_class,0,sizeof(ave_entering_class));

    cur_bachelor_degree = 0;
    cur_non_ug_bachelor_degree = 0;
    cur_master_degree = 0;
    cur_doctor_degree = 0;

    for ( int i=0; i<4; i++ ) {
	last_year_degree[i] = 0;
	last_year_dropout[i] = 0;
    }

    ave_time_to_graduation_for_ug = 0;
}

//------------- End of function StudentArray Constructor -----//

//----------- Begin of function StudentArray Destructor ------//
//!
StudentArray::~StudentArray() {
    //----------------------------------//
    for( int i=size() ; i>0 ; i-- ) {
	if( !is_deleted(i) )
	    del(i);
    }

    //----------------------------------//
}

//------------- End of function StudentArray Destructor ------//

//--------- Begin of function StudentArray::add ---------//

int StudentArray::add(char studentLevel, char yearInProgram, char genderEthnicGroup, char stuSeg, char major, int finAid, float* talentArr) {
    Student student;

    student.init(studentLevel, yearInProgram, genderEthnicGroup, stuSeg, major, finAid, talentArr );

    linkin(&student);

    ((Student*)get())->student_recno = recno();

    student_count++;

    return recno();
}

//----------- End of function StudentArray::add -----------//

//--------- Begin of function StudentArray::del ---------//

void StudentArray::del(int recNo) {
    // BUG FIXED in Version 2.0
    Student *ptr = (Student*) get(recNo);

    // avoid bug 2107
    if(!ptr)
	return;                                       // temp use
    //	err_when(!ptr);

    //##### begin fred 980915 #####//
    student_count--;
    //##### end fred 980915 #####//
    linkout(recNo);
}

//----------- End of function StudentArray::del -----------//

//--------- Begin of function StudentArray::is_deleted ---------//

int StudentArray::is_deleted(int recNo) {
    Student* studentPtr = (Student*) get(recNo);
    if( studentPtr == NULL )
	return 1;
    return studentPtr->student_recno == 0;
}

//----------- End of function StudentArray::is_deleted -----------//

#ifdef DEBUG

//------- Begin of function StudentArray::operator[] -----//

Student* StudentArray::operator[](int recNo) {
    Student* StudentPtr = (Student*) get(recNo);

    if( !StudentPtr )
	err.run( "StudentArray[] is deleted" );

    return StudentPtr;
}

//--------- End of function StudentArray::operator[] ----//
#endif

//------------------------------------------------------------//
//------------------------------------------------------------//
//------------------------------------------------------------//
//------------------------------------------------------------//
//------------------------------------------------------------//

//--------- Begin of function StudentArray::next_day ---------//
//!
void StudentArray::next_day() {
    //----- call individual student's next_day() function ----//

    for( int i=size() ; i>0 ; i-- ) {
	if( is_deleted(i) )
	    continue;

	operator[](i)->next_day();
    }

    //##### begin fred 980819 #####//
    //----------------------------------------//
    // start of a month
    if ( info.game_day == 1 ) {
	//## 071299 chea 1.12.1
	//     update_history(UPDATE_MONTH);

	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month )
	    update_history(UPDATE_YEAR);
	//## 071299 chea 1.12.1
	else
	    update_history(UPDATE_MONTH);

    }

    //##### end fred 980819 #####//
}

//----------- End of function StudentArray::next_day -----------//

//--------- Begin of function StudentArray::next_trimester ---------//
//!
//! This function is called when a new school year begins.
//!
void StudentArray::next_trimester() {

    for( int i=size() ; i>0 ; i-- ) {
	if( is_deleted(i) )
	    continue;

	operator[](i)->next_trimester();
	sys.yield();
    }

    //----- select courses for the new trimester ------//

    //### fred 980916 ###//
    // select_cours() runs after next_trimester() which handle change of major and drop out//

    //select_cours();		// remove this because it should be called after enrollment
}

//----------- End of function StudentArray::next_trimester -----------//

//--------- Begin of function StudentArray::select_course ---------//
//!
//! All courses are selected during breaks between trimesters.
//!
//! [int] ignoreFaculty - if this is true, courses will be assigned
//!                       without faculty constraints. This is for
//!                       initialization only.
//!                       (default: 0)
//!
void StudentArray::select_course(int ignoreFaculty) {
    //-- select courses for students in various student levels based on the priority set by the user --//

    //	int studentCount = packed_size();		// 990302  BUGHERE should use size() instead

    int studentCount = size();                      //## chea 300899 used size() instead
    //   info.debug_enroll3 = packed_size();    //## chea 270899

    for( int i=0 ; i<MAX_STUDENT_LEVEL ; i++ ) {
	int studentLevel = player_school.course_match_order[i];
	int studentRecno = m.random(studentCount);

	for( int y=0 ; y<studentCount ; y++ ) {
	    if( ++studentRecno > studentCount )
		studentRecno = 1;

	    if( is_deleted(studentRecno) )
		continue;

	    Student* studentPtr = operator[](studentRecno);

	    if( studentPtr->student_level != studentLevel )
		continue;

	    studentPtr->file_course_id=0;
	    studentPtr->select_course(ignoreFaculty);

	    //			info.debug_enroll2++; //## chea 270899
	}
    }

    //--- reset counter variables after completing course selection for the trimester ---//

    // if this is the last trimester in a year, reset annual variables
    if( player_school.cur_trimester == LAST_TRIMESTER ) {
	for( int i=studentCount ; i>0 ; i-- ) {
	    if( is_deleted(i) )
		continue;

	    Student* studentPtr = operator[](i);

	    studentPtr->total_elective_course_this_year = 0;
	}
    }

}

//---------- End of function StudentArray::select_course ----------//

//--------- Begin of function StudentArray::init_course ---------//
//!
//! This function is called when **generating student** records during
//! initialization. It is to generate previous course selection records
//! for the students.
//!
//! <int> processYearInProgram = only process students whose year in
//!                              program is the same as this.
//!
void StudentArray::init_course(int processYearInProgram) {
    err_here();                                     // 990413---------------------------------------

    int studentCount = size();
    int studentRecno = m.random(studentCount);

    for( int thisTrimester=FIRST_TRIMESTER ; thisTrimester<=LAST_TRIMESTER ; thisTrimester++ ) {
	//---- call course_select() for all students in this department ----//

	for( int i=0 ; i<studentCount ; i++ ) {
	    if( ++studentRecno > studentCount )
		studentRecno = 1;

	    if( is_deleted(studentRecno) )
		continue;

	    Student* studentPtr = operator[](studentRecno);

	    //----------------------------------------//

	    if( studentPtr->year_in_program==1 )        // no need to initialize courses for students in the first year
		continue;

	    if( studentPtr->student_level == MASTER ) { // no course selection for non-matriculated. Master student only has one year, so no initialization also
		//studentPtr->student_level == NON_MATRICULATED )		//0107
		continue;
	    }

	    //---- clip the year in program to the standard year for easy handling ----//

	    int yearInProgram, maxYearInProgram;

	    switch( studentPtr->student_level ) {
	    case UG_TRADITION:
	    case UG_NONTRADITION:
	    case DISTANCE_LEARN:                      // 0107
		maxYearInProgram = BACHELOR_YEAR_COUNT;
		break;

	    case DOCTOR:
		maxYearInProgram = DOCTOR_YEAR_COUNT;
		break;
	    }

	    yearInProgram = min( studentPtr->year_in_program, maxYearInProgram );

	    //--- only process students whose year in program is the same as processYearInProgram ---//

	    if( yearInProgram == processYearInProgram ) {
		//-- if the current year_in_program exceeds the max, set it to max and restore the original later --//

		int saveYearInProgram=0;

		if( studentPtr->year_in_program > maxYearInProgram ) {
		    saveYearInProgram = studentPtr->year_in_program;
		    studentPtr->year_in_program = maxYearInProgram;
		}

		//----- select course now ------//

		studentPtr->select_course(1);             // 1-ignore faculty resource constraints.

		//----- restore the original year in program -----//

		if( saveYearInProgram )
		    studentPtr->year_in_program = saveYearInProgram;

		//--- reset yearly counter if this is the last trimester ---//

		if( thisTrimester==LAST_TRIMESTER )
		    studentPtr->total_elective_course_this_year = 0;
	    }
	}

	//------ reset vars for all courses in this department ------//

	Department* deptPtr = department_array[department_recno];

	for( int i=deptPtr->course_array.size() ; i>0 ; i-- ) {
	    deptPtr->course_array[i]->reset_vars();     // reset it so that we can call course later selection without previous constraints.
	}
    }
}

//----------- End of function StudentArray::init_course -----------//
