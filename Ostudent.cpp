//Filename    : OSTUDENT.cpp
//Description : STUDENT Class Definition

#include <OSYS.H>
#include <OINFO.H>
#include <OPSCHOOL.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <GAMEDEF.H>
#include <OFINANCE.H>
#include <ODATE.H>
#include <OSTUDENT.H>
#include <OMATH.H>                                // ### 980915 fred
#include <OBOX.H>                                 // ### testing chea
#include <OCOURSE.H>
#include <OSTUDENT.H>

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

//##### begin fred 980916 #####//
//---------- Begin of function Student::init -----------//
//!
void  Student::init(char studentLevel, char yearInProgram, char genderEthnicGroup, char stuSeg, char major, int finAid, float* talentArr) {
    memset( this, 0, sizeof(Student) );

    //--- vars that will remain constant once initialized ---//

    department_recno          = major;              // =0 if UNDECIDED

    student_level             = studentLevel;
    gender_ethnic_group       = genderEthnicGroup;
    student_segment         = stuSeg;

    date_admitted           = info.game_date-(yearInProgram-1)*365;

    //------- vars that will change over time ------//

    year_in_program         = yearInProgram;
    financial_aid           = finAid;
    academic_achievement        = 0;                // 0 means nil, it is updated after completion of each trimester

    //--- initialize the number courses to take per trimester ---//
    if ( student_level != UG_NONTRADITION ) {
	course_per_trimester[AUTUMN]  = STD_COURSE_PER_TRIMESTER;
	course_per_trimester[WINTER]  = STD_COURSE_PER_TRIMESTER;
    }
    else {
	course_per_trimester[AUTUMN]  = STD_COURSE_NUG_PER_TRIMESTER;
	course_per_trimester[WINTER]  = STD_COURSE_NUG_PER_TRIMESTER;
    }

    off_quarter = SUMMER;

    // newly matriculated
    if ( student_level == UG_TRADITION && year_in_program == 1 ) {
	if( m.random(4)==0 )                          // select 2/8 sample
	{

	    off_quarter = m.random(1) + AUTUMN;         // select autumn or winter

	    err_when(AUTUMN!=0);

	    course_per_trimester[off_quarter] = 0;      // this is determined by summer teaching policy
	    course_per_trimester[SUMMER] = STD_COURSE_PER_TRIMESTER;
	}
    }

    //-- to update these three from enrollres.talen_array ---//
    //## 071299 chea 1.12.1
    // min & max bug chea
    talent_academic         = (char) max(0.0f,min(100.0f, talentArr[ACADEMIC] * 10));
    //	talent_academic			 		= 33; //##200100 chea

    // min & max bug chea
    talent_extracurricular      = (char) max(0.0f,min(100.0f, talentArr[EXTRA_CURRI] * 10));
    // min & max bug chea
    talent_athletics          = (char) max(0.0f,min(100.0f, talentArr[ATHLETIC] * 10));

    //------ refer to techdoc3.3 section 4.10 -----//

    satisfaction_academic     = 0;
    satisfaction_student_life   = 0;
    satisfaction_athletics      = 0;

    err_when( studentLevel < 0 || studentLevel >= MAX_STUDENT_LEVEL );
    err_when( genderEthnicGroup < 0 || genderEthnicGroup >= GENDER_ETHNIC_TYPE_COUNT );
    err_when( major < 0 || major > department_array.department_count );

    // ug yr1 student registered in General Education Department with dept_recno=0
    err_when( major==0 && student_level != UG_TRADITION && !is_nontradition_ug_group(student_level) );
}

//---------- End of function Student::init -----------//
//##### end fred 980806 #####//

//---------- Begin of function Student::year_admitted -----------//
//!
int Student::year_admitted() {
    return date.year(date_admitted);
}

//---------- End of function Student::year_admitted -----------//

//---------- Begin of function Student::next_day -----------//
//!
void Student::next_day() {
    //##### begin fred 980814 #####//

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
	if ( info.game_month == finance.fiscal_year_start_month )
	    update_history(UPDATE_YEAR);
    }
    /*
      //##### end fred 980814 #####//

      //---- if the student has already selected all courses ----//

      if( has_selected_all_course() )
      return;

      //-- only next_day a specific student level at one time --//

      if( player_school.cur_select_course_student_level != student_level )
      return;

      //-------- only select courses in week days ---------//

      if( info.game_day > 5 )
      return;

      // remove select_cours()
    */
}

//---------- End of function Student::next_day -----------//

//---------- Begin of function Student::next_trimester -----------//
//!
void Student::next_trimester() {
    //----- a student graduate when he/she has completed all the required number of courses ----//

    //	if(info.game_day == 30 && info.game_month == 8)  //## chea 090999 don't know is it only cl once
    //	{
    if( think_graduate() )                          // the student is deleted from student_array after graduation
	return;

    //	}

    if( think_dropout() )                           // the student is deleted from student_array after dropout
	return;                                       //##testing

    // change major runs after graduate and dropout

    if( think_change_major() )                      // the student is deleted from student_array after change
	return;                                       //##testing

    //--- if it is the first trimester, that means it's a new school year ---//

    // last call in this func, since calls above will use year_in_program.	//1125

    if( player_school.cur_trimester == FIRST_TRIMESTER )
	year_in_program++;
}

//---------- End of function Student::next_trimester -----------//

//---------- Begin of function Student::think_graduate -----------//
//!
//! graduate (use cummulative passed courses in general and grad prob for doctor);
//!
int Student::think_graduate() {
    //----- a student graduate when he/she has completed all the required number of courses ----//

    int requiredCourseCount;

    switch( student_level ) {
    case UG_TRADITION:
    case UG_NONTRADITION:
    case DISTANCE_LEARN:
	//32
	requiredCourseCount = COURSE_COUNT_REQUIRED_FOR_BACHELOR;
	break;

    case MASTER:
	//8
	requiredCourseCount = COURSE_COUNT_REQUIRED_FOR_MASTER;
	break;

    case DOCTOR:
	//16
	requiredCourseCount = COURSE_COUNT_REQUIRED_FOR_DOCTOR;
	break;
    }

    //##### begin fred 980915 #####//

    //Department* deptPtr = department_recno?department_array[department_recno] : department_res.general_dept;

    if( total_course_all < requiredCourseCount ) {
	//		info.debug_enroll++;   //## chea 281099
	return 0;
    }

    err_when(department_recno == 0);
    Department* deptPtr = department_array[department_recno];

    if ( student_level == DOCTOR ) {
	err_when(department_recno == 0);

	char yearIn = min(year_in_program-1, MAX_GRADUATE_YEARS-1);
	float gradProb = department_res[deptPtr->department_id]->doctor_graduate_trans_prob[yearIn];

	//
	float input[2];
	float multiplier = 1;
	//0223
	input[0] = math.dual_response_func(0.80f, 1.02f, 1.30f, 0.891f, 1.295f, 0.557f, 2.326f, 0);
	//0223
	input[1] = math.dual_response_func(0.80f, 1.00f, 1.30f, -0.912f, -0.863f, 2.502f, 0.489f, 0);

	multiplier = 0.5f * input[0] + 0.5f * input[1];
	// response_func var 28: multiplier = math.latency_func(0.33f, multiplier, lastMultiplier);

	if ( math.get_random_float() > gradProb * multiplier ) {
	    //			info.debug_enroll++;
	    return 0;
	}
    }
    //##### end fred 980915 #####//

    //-------- graduate now ----------------//

    //----- update the graduation count -----//

    switch( student_level ) {
    case UG_TRADITION:
    case UG_NONTRADITION:
	if ( student_level == UG_TRADITION ) {
	    deptPtr->student_array.cur_bachelor_degree++;
	    deptPtr->student_array.last_year_degree[0]++;
	}
	else {
	    deptPtr->student_array.cur_non_ug_bachelor_degree++;
	    deptPtr->student_array.last_year_degree[1]++;
	}
	deptPtr->student_array.time_to_degree_cumm[BACHELOR_DEGREE] += year_in_program;
	break;

    case MASTER:
	deptPtr->student_array.cur_master_degree++;
	deptPtr->student_array.last_year_degree[2]++;
	deptPtr->student_array.time_to_degree_cumm[MASTER_DEGREE] += year_in_program;
	break;

    case DOCTOR:
	deptPtr->student_array.cur_doctor_degree++;
	deptPtr->student_array.last_year_degree[3]++;
	deptPtr->student_array.time_to_degree_cumm[DOCTOR_DEGREE] += year_in_program;
	break;
    }

    //-- update average time to graduation for traditional undergraduates --//

    if( student_level == UG_TRADITION ) {

	int graduationTime = info.game_date - date_admitted;

	deptPtr->student_array.ave_time_to_graduation_for_ug =
	    ( deptPtr->student_array.ave_time_to_graduation_for_ug *
	      (deptPtr->student_array.cur_bachelor_degree - 1) + graduationTime ) /
	    deptPtr->student_array.cur_bachelor_degree;

#ifdef DEBUG
	static int  grad4Year[2] = { 0, 0 };
	if( graduationTime > 1461 ) {                 // 4 year
	    grad4Year[1]++;
	}
	else {
	    grad4Year[0]++;
	}
#endif
    }

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
	file_course_id++;
	WRITE_FIELD_SEPARATOR( (&file) );
	WRITE_FIELD_SEPARATOR( (&file) );
	WRITE_NUM_FIELD( (&file), file_course_id );
	WRITE_FIELD_SEPARATOR( (&file) );

	WRITE_STR_FIELD( (&file), "Status" );
	WRITE_FIELD_SEPARATOR( (&file) );
	WRITE_FIELD_SEPARATOR( (&file) );
	if ( department_recno ) {
	    WRITE_STR_FIELD( (&file), "Grad" );
	}

	file.file_close();
    }

    //----- remove the student from student_array -----//

    deptPtr->student_array.del(student_recno);
    //	info.debug_enroll++;
    //	info.debug_enroll2++;
    return 1;
}

//---------- End of function Student::think_graduate -----------//

//##### begin fred 980915 #####//
//---------- Begin of function Student::think_dropout -----------//
//!
//!
//! called only by pschool's next_day() at the end of trimester;
//!
//! called before optimization and after the end of the last trimester
//!
int Student::think_dropout() {
    calc_multiplier_on_dropout_prob();

    //-------- find dropout_prob ----------------//
    err_when(year_in_program<1);

    float dropProb = 0;
    char yearIn = min(year_in_program-1, MAX_GRADUATE_YEARS-1);
    GeneralDepartment* deptPtr = department_recno ? (GeneralDepartment *) department_array[department_recno] : &department_res.general_dept;

    // 0118
    if ( deptPtr->student_array.student_count <= 10 ) {
	//		info.debug_enroll3++; //## chea 281099 stu total
	return 0;
    }
    switch( student_level ) {
    case UG_NONTRADITION:
	dropProb = 0.5f * player_school.dropout_trans_prob[student_level][yearIn];
	break;
    case UG_TRADITION:
    case MASTER:
	// float dropout_trans_prob[MASTER_ARRAY_SIZE][MAX_GRADUATE_YEARS];
	dropProb = player_school.dropout_trans_prob[student_level][yearIn];
	break;

    case DOCTOR:
	err_when(&department_res.general_dept == deptPtr);
	dropProb = 0.5f * department_res[deptPtr->department_id]->doctor_dropout_trans_prob[yearIn];
	break;

    case DISTANCE_LEARN:
	dropProb = player_school.dropout_trans_prob[UG_NONTRADITION][yearIn];
	break;
    }

    // min & max bug chea
    dropProb = max(0.0f, min(1.0f, dropProb * multiplier_on_dropout_prob));

    if ( math.get_random_float() > dropProb ) {
	//		info.debug_enroll3++; //## chea 281099 student total
	return 0;
    }
    //---------// else dropout! //---------//
    //----- remove the student from student_array -----//

    char level2Degree[] = {BACHELOR_DEGREE, BACHELOR_DEGREE, MASTER_DEGREE, DOCTOR_DEGREE, -1};

    if ( level2Degree[student_level] >=0 ) {
	deptPtr->student_array.last_year_dropout[student_level]++;
	deptPtr->student_array.cur_dropout[level2Degree[student_level]]++;
    }

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
	file_course_id++;
	WRITE_FIELD_SEPARATOR( (&file) );
	WRITE_FIELD_SEPARATOR( (&file) );
	WRITE_NUM_FIELD( (&file), file_course_id );
	WRITE_FIELD_SEPARATOR( (&file) );

	WRITE_STR_FIELD( (&file), "Status" );
	WRITE_FIELD_SEPARATOR( (&file) );
	WRITE_FIELD_SEPARATOR( (&file) );
	if ( department_recno ) {
	    WRITE_STR_FIELD( (&file), "Drop" );
	}

	file.file_close();
    }

    deptPtr->student_array.del(student_recno);
    //   info.debug_enroll3++;
    //   info.debug_enroll4++;
    return 1;
}

//---------- End of function Student::think_dropout -----------//

//---------- Begin of function Student::think_change_major -----------//
//!
//! courseCompleted = 0 by default
//!
int Student::think_change_major(char courseCompleted) {
    //-------- scenario special handling ----------//

    if ( player_school.scenario_id == SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS && info.prerun_year )
	return change_major(1);                       // 1-force changing major

    //-------- calc prob --------------//

    calc_probability_change_major();

    // 990414
    if ( department_recno != 0 && year_in_program == 1 )
	return 0;

    if ( department_recno != 0 && math.get_random_float() > probability_change_major )
	return 0;

    //---------// else change! //---------//

    return change_major(courseCompleted);           //do this from below
}

//---------- End of function Student::think_change_major -----------//
//##### end fred 980915 #####//

//## chea begin 170699
//---------- Begin of function Student::change_major -----------//
//!
//! courseCompleted = 0 by default
//!
int Student::change_major(int couCompleted) {     //if 1 is passed to courseCompleted then the student must change major.

    //----- relocate the student from student_array -----//

    int major = player_school.get_random_major(student_level);

    // ug yr1 student registered in General Education Department
    if ( couCompleted ) {
	// go selected the major above
	err_when(major == 0 );
    }
    else if ( department_recno==0 ) {
	err_when( student_level != UG_TRADITION && !is_nontradition_ug_group(student_level));

	if ( year_in_program == 1 )                   // change major after 1st year
	    return 0;

	err_when(year_in_program!=2);                 // this student should be just promoted to yr2
    }

    if ( department_recno == major )
	return 0;                                     // don't change

    err_when(major == 0);

    GeneralDepartment* oldDeptPtr = department_recno ? (GeneralDepartment *) department_array[department_recno] : &department_res.general_dept;
    Department* newDeptPtr = department_array[major];

    int ods = oldDeptPtr->student_array.packed_size();
    int nds = newDeptPtr->student_array.packed_size();
    int oldStudentRecno = student_recno;

    if (student_recno <=0 ) {
	//		info.debug_enroll++;
	oldDeptPtr->student_array.del(student_recno);
	return 0;
    }

    // add
    department_recno = major;
    newDeptPtr->student_array.linkin(this);

    // update
    ((Student*)(newDeptPtr->student_array.get()))->student_recno
	= newDeptPtr->student_array.recno();

    newDeptPtr->student_array.student_count++;

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
	file_course_id++;
	WRITE_FIELD_SEPARATOR( (&file) );
	WRITE_FIELD_SEPARATOR( (&file) );
	WRITE_NUM_FIELD( (&file), 0 );
	WRITE_FIELD_SEPARATOR( (&file) );

	WRITE_STR_FIELD( (&file), "Status" );
	WRITE_FIELD_SEPARATOR( (&file) );
	WRITE_FIELD_SEPARATOR( (&file) );
	WRITE_STR_FIELD( (&file), department_res[newDeptPtr->department_id]->name );

	WRITE_RECORD_SEPARATOR( (&file) );

	file.file_close();
    }

    // remove
    oldDeptPtr->student_array.del(oldStudentRecno);

    int oods = oldDeptPtr->student_array.packed_size();
    int onds = newDeptPtr->student_array.packed_size();

    err_when(ods-1 != oods);
    err_when(nds+1 != onds);
    err_when( newDeptPtr->student_array[newDeptPtr->student_array.recno()]->department_recno != major);

    //	this->department_recno = major;	// for think_change_major(1)
    this->department_recno = major;                 // for think_change_major(1)  //##testing
    return 1;
}

//---------- End of function Student::change_major -----------//
//## chea end 170699
