//Filename    : ODEPTA.CPP
//Description : Department resource object

#include <OSYS.H>
#include <OMATH.H>
#include <OGAME.H>
#include <OGAMESET.H>
#include <ODEPTRES.H>
#include <ODEPT.H>
#include <OPSCHOOL.H>
#include <OINFO.H>
#include <OFINANCE.H>
#include <OSCHLRES.H>                             //## fred 0919
#include <OFACILIT.H>

//--------- Begin of function DepartmentArray::DepartmentArray ----------//
//!
DepartmentArray::DepartmentArray() : DynArrayB(sizeof(Department*), 20, DEFAULT_REUSE_INTERVAL_DAYS) {
}

//---------- End of function DepartmentArray::DepartmentArray ----------//

//--------- Begin of function DepartmentArray::~DepartmentArray ----------//
//!
DepartmentArray::~DepartmentArray() {
    deinit();
}

//---------- End of function DepartmentArray::~DepartmentArray ----------//

//--------- Begin of function DepartmentArray::init ----------//
//!
void DepartmentArray::init() {
    deinit();
    //## 200799
    memset( (char *)this + sizeof(DynArrayB), 0, sizeof(DepartmentArray)-sizeof(DynArrayB) );

    enum {
	MODE_DEPT_MAIN = 0,
	MODE_DEPT_EDUCATIONAL_QUALITY=1,
	MODE_DEPT_DEPARTMENTAL_ACADEMIC_STANDING=2,
	MODE_DEPT_STUDENT_MORALE=3,
	MODE_DEPT_FACULTY_RESEARCH_PERFORMANCE=4,
	MODE_DEPT_FACULTY_MORALE=5,
	MODE_DEPT_FACULTY_DIVERSITY_INDEX=6,
	MODE_DEPT_EDUCATIONAL_QUALITY2=7,
	MODE_DEPT_RANK=8,
    };
    enum {
	REPORT_TYPE_DEPARTMENT_OVERVIEW, REPORT_TYPE_DEPARTMENT_CLASSTYPES, REPORT_TYPE_DEPARTMENT_TECHNOLOGY,
	REPORT_TYPE_INSTITUTION_OVERVIEW, REPORT_TYPE_INSTITUTION_SUMMERTEACHING, REPORT_TYPE_INSTITUTION_TECHNOLOGY,
    };

    enum {
	MODE_INSTITUTION  = 0,
	MODE_DEPARTMENT = 1,
	MODE_RANK     = 2,
    };

    current_report_type=REPORT_TYPE_INSTITUTION_OVERVIEW;
    current_screen_mode = MODE_DEPT_MAIN;
    current_pref_report_type=MODE_DEPT_EDUCATIONAL_QUALITY;
    current_data_mode = MODE_INSTITUTION;

    department_count = 0;

    selected_recno = 0;

    weight_teaching = 40;                           // = [0,100]
    weight_scholarship = 30;
    weight_research = 30;

    teaching_weight_input=1;                        // {Very important = 3; Important = 2; Moderately important = 1; Not at all important = 0}
    scholarship_weight_input=1;
    research_weight_input=1;
    gender_ethnic_equity_weight_input=1;

    for (int i=0; i<MAX_RANK_AGE_GROUP; i++)
	rank_age_group_salary_increase_multiplier[i] = 1.0f;

    faculty_count_on_tenure_line = 0;
    faculty_count_tenured = 0;

}

//---------- End of function DepartmentArray::init ----------//

//--------- Begin of function DepartmentArray::deinit ----------//
//!
void DepartmentArray::deinit() {
    if( size()==0 )
	return;

    //----------------------------------//

    for( int i=size() ; i>0 ; i-- ) {
	if( !is_deleted(i) )
	    del(i);
    }

    //----------------------------------//

    zap();                                          // zap the DynArrayB
}

//---------- End of function DepartmentArray::deinit ----------//

//### fred 980921 ###//
//--------- Begin of function DepartmentArray::init_data ----------//
void DepartmentArray::init_data() {
    int i,j;
    memset(average_research_dollars,0,sizeof(average_research_dollars));

    // it's better to use db_school_array but not school_ex_array;
    // but it requires relocation of SchoolEx's active_research_dollars to School's active_research_dollars

    for (i=0; i<MAX_SCHOOL_EX_FOR_INIT_CONDITION; i++) {
	SchoolEx  *schPtr = &school_res.school_ex_array[i];

	for (j=0; j<FACULTY_RANK_LEVEL_COUNT; j++)
	    average_research_dollars[j] += schPtr->school_faculty_array_ex[j].active_research_dollars;
    }

    for (j=0; j<FACULTY_RANK_LEVEL_COUNT; j++)
	// var for calc research norm
	average_research_dollars[j] /= MAX_SCHOOL_EX_FOR_INIT_CONDITION;
}

//---------- End of function DepartmentArray::init_data ----------//
//### fred 980921 ###//

//--------- Begin of function DepartmentArray::add ---------//
int DepartmentArray::add(int departmentId) {
    Department* newDept = new Department;

    linkin(&newDept);

    ((Department*)get_ptr())->department_recno = recno();

    department_count++;

    err_when( department_count > MAX_DEPARTMENT );

    int recNo = recno();

    //----------------------------------//

    newDept->init(departmentId);

    if(selected_recno==0)
	selected_recno = recNo;

    //---- update total_elective_course_pref ------//

    //department_res.calc_total_elective_course_pref();

    return recNo;
}

//----------- End of function DepartmentArray::add -----------//

//--------- Begin of function DepartmentArray::create_dept ---------//
//!
//! called only by read_file()
//! return : <int> nationRecno = the recno. of the newly added nation
//!
int DepartmentArray::create_dept() {
    Department *deptPtr = new Department;

    linkin(&deptPtr);

    err_when( size() > MAX_DEPARTMENT );

    deptPtr->department_recno = recno();

    return recno();
}

//----------- End of function DepartmentArray::create_dept ---------//

//--------- Begin of function DepartmentArray::del ---------//

void DepartmentArray::del(int recNo) {
    Department* deptPtr = department_array[recNo];

    delete deptPtr;

    linkout(recNo);

    department_count--;

    //--------------------------------//
    if( recNo == selected_recno ) {
	//---- set the selected recno to first existing department in the array ---//

	selected_recno = 0;

	for( int i=1 ; i<=size() ; i++ ) {
	    if( !is_deleted(i) ) {
		selected_recno = i;
		break;
	    }
	}
    }

    //---- update total_elective_course_pref ------//

    department_res.calc_total_elective_course_pref();
}

//----------- End of function DepartmentArray::del -----------//

//--------- Begin of function DepartmentArray::next_day ---------//

void DepartmentArray::next_day() {
    //----- call individual department's next_day() function ----//

    for( int i=department_array.size() ; i>0 ; i-- ) {
	if( department_array.is_deleted(i) )
	    continue;

	department_array[i]->next_day();
    }

    //##### begin fred 980914 ####//
    // start of a trimester
    if ( info.game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	 && info.game_month == player_school.trimester_array[player_school.cur_trimester].start_month ) {
	update_history(UPDATE_TRIMESTER);
    }

    // start of a month
    if ( info.game_day == 1 ) {
	//--- 990421 ---//

	if ( info.game_month == finance.fiscal_year_start_month )
	    for (int i=0; i<RESEARCH_STATUS_COUNT; i++)
		shift_history(research_y_history[i], HISTORY_YEAR_COUNT);

	//-------------//

	update_history(UPDATE_MONTH);

	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month ) {
	    facility_office.begin_space_inventory();
	    update_history(UPDATE_YEAR);                // pi and statistic calculated here
	}
    }
    //##### end fred 980914 ####//
}

//----------- End of function DepartmentArray::next_day -----------//

//--------- Begin of function DepartmentArray::is_deleted ---------//

int DepartmentArray::is_deleted(int recNo) {
    Department* departmentPtr = (Department*) get_ptr(recNo);

    if( departmentPtr == NULL )
	return 1;

    return departmentPtr->department_recno == 0;
}

//----------- End of function DepartmentArray::is_deleted -----------//

//--------- Begin of function DepartmentArray::has_department ---------//
//!
//! Return whether this department array has the department whose name
//! is passed as the parameter.
//!
int DepartmentArray::has_department(char* departmentName) {
    for( int i=size() ; i>0 ; i-- ) {
	if( is_deleted(i) )
	    continue;

	if( strcmpi( department_res[ operator[](i)->department_id ]->name, departmentName )==0 )
	    return i;
    }

    return 0;
}

//----------- End of function DepartmentArray::has_department -----------//

//chwg ##1230
#ifdef DEBUG

//------- Begin of function DepartmentArray::operator[] -----//

Department* DepartmentArray::operator[](int recNo) {
    Department* DepartmentPtr = (Department*) get_ptr(recNo);

    if( !DepartmentPtr )
	err.run( "DepartmentArray[] is deleted" );

    return DepartmentPtr;
}

//--------- End of function DepartmentArray::operator[] ----//
#endif

//------- Begin of function DepartmentArray::calc_course_pref -----//

void DepartmentArray::calc_course_pref() {
    // 1105 see section req29a_course.txt and design notes 0830 section 6.9

    int d, i;

    for ( d=size(); d>0; d-- ) {
	if ( is_deleted(d) )
	    continue;

	Department* deptPtr = operator[](d);

	CourseTemplate* templ = course_res.get_template(deptPtr->department_id);
	float rawPref[COURSE_PREFERENCE_COUNT] = {0,0,0};
	float sumDev = 0;
	float sumRawPref = 0;
	const float lamda = 0.6f;

	int testsum = 0;

	for ( i=0; i<COURSE_PREFERENCE_COUNT; i++ ) {
	    sumDev += math.safe_pow(float(deptPtr->course_preference_input[i] - templ->target_preference[i]), 2);
	    testsum += deptPtr->course_preference_input[i];
	}

	err_when(testsum > 101 || testsum < 97);

	if ( sumDev > 0 )
	    for ( i=0; i<COURSE_PREFERENCE_COUNT; i++ ) {
		float dev = math.safe_pow(float(deptPtr->course_preference_input[i] - templ->target_preference[i]), 3);
		rawPref[i] = templ->target_preference[i] + dev / sumDev;
		sumRawPref += rawPref[i];
	    }

	if ( sumRawPref > 0 ) {
	    for ( i=0, sumDev=0; i<COURSE_PREFERENCE_COUNT; i++ ) {
		rawPref[i] = (char) player_school.latency_func(lamda, rawPref[i] * 100 / sumRawPref, deptPtr->course_preference[i]);
		sumDev += rawPref[i];
	    }

	    if ( sumDev > 0 ) {
		for ( testsum=0, i=0; i<COURSE_PREFERENCE_COUNT; i++ ) {
		    deptPtr->course_preference[i] = char( rawPref[i]*100/sumDev);
		    testsum += deptPtr->course_preference[i];
		}

		err_when(testsum > 101 || testsum < 97);

		// fix type cast precision problem above
		deptPtr->course_preference[0] += 100 - testsum;
	    }
	}
    }                                               // end for
}

//--------- End of function DepartmentArray::calc_course_pref ----//
