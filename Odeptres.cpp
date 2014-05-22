//Filename    : ODEPTRES.CPP
//Description : Department resource object

#include <ALL.H>
#include <OMATH.H>
#include <OSYS.H>
#include <OFIELD.H>
#include <OGAMESET.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OSTUDENT.H>                             // for enum	{ SL_UG, SL_MASTER, SL_DOCTOR, SL_DISTANCE_LEARN };
#include <OFIELD.H>                               // for IPED_FILED_COUNT
#include <OCOURRES.H>                             // template_course_type
#include <opschool.h>

//---------- #define constant ------------//

#define DEPARTMENT_DB    "DEPT"
#define DEPARTMENT_DB2   "DEPT2"
#define DEPARTMENT_DB3   "FACILITY"

// db from HE.GDB.init
#define FACULTY_TRANSITION_PROBABILITY    "FAC_TRAN"
#define COURSE_UG_DB        "COUR_B"              // db from HE.GDB.init
#define COURSE_MASTER_DB    "COUR_M"
#define COURSE_DOCTOR_DB    "COUR_D"
#define ELECTIVE_PREF_DB    "ELECPREF"

//----------------------------------------------//
//
void change_templ_code(char* code) {
    switch (code[0]) {
    case 'S':
	code[0] = 'A';
	break;
    case 'N':
	code[0] = 'B';
	break;
    case 'M':
	code[0] = 'C';
	break;
    case 'H':
	code[0] = 'D';
	break;
    case 'O':
	code[0] = 'E';
	break;
    default:
	err_here();
	break;
    }
}

//----------------------------------------------//

//------- Begin of function DepartmentRes::DepartmentRes -----------//

DepartmentRes::DepartmentRes() {
    init_flag=0;
}

//--------- End of function DepartmentRes::DepartmentRes -----------//

//------- Begin of function DepartmentRes::DepartmentRes -----------//

DepartmentRes::~DepartmentRes() {
    deinit();
}

//--------- End of function DepartmentRes::DepartmentRes -----------//

//---------- Begin of function DepartmentRes::init -----------//
//!
//! This function must be called after a map is generated.
//!
void DepartmentRes::init() {
    deinit();

    general_dept.init();
    memset(&general_dept_info, 0, sizeof(DepartmentInfo));

    //------- load database information --------//

    load_info();
    load_faculty_info();
    load_course_info();
    load_elective_course_pref();

    init_flag=1;
}

//---------- End of function DepartmentRes::init -----------//

//---------- Begin of function DepartmentRes::deinit -----------//

void DepartmentRes::deinit() {
    if( init_flag ) {
	mem_del(info_array);

	init_flag=0;
    }

    //----------------------------------// 990603

    for( int i=general_dept.student_array.size() ; i>0 ; i-- ) {
	if( !general_dept.student_array.is_deleted(i) )
	    general_dept.student_array.del(i);
    }

    //----------------------------------//

}

//---------- End of function DepartmentRes::deinit -----------//

//------- Begin of function DepartmentRes::load_info -------//
//!
void DepartmentRes::load_info() {
    DepartmentRec  *deptRec;
    DepartmentInfo *deptInfo;
    Database    *dbDepartment = game_set.open_db(DEPARTMENT_DB);

    department_count = (short) dbDepartment->rec_count();
    info_array      = (DepartmentInfo*) mem_add( sizeof(DepartmentInfo)*department_count );

    err_when( department_count != DEPARTMENT_TYPE_COUNT );

    //------ read in department information array -------//

    memset( info_array, 0, sizeof(DepartmentInfo) * department_count );

    for( int i=0 ; i<department_count ; i++ ) {
	deptRec   = (DepartmentRec*) dbDepartment->read(i+1);
	deptInfo  = info_array+i;

	if (i+1==department_count)
	    i=i;

	m.rtrim_fld( deptInfo->name, deptRec->name, deptRec->NAME_LEN );

	//String str = deptInfo->name;
	//str += " Department";
	//strncpy(deptInfo->name, str, MAX_STR_LEN );

	deptInfo->field_id = field_res.look_up_code( deptRec->field_code[0] );
	// deptInfo->total_courses_taught = m.atoi( deptRec->total_courses_taught, 9 );

	//0406 if ( deptInfo->total_courses_taught <= 0 )
	//	deptInfo->total_courses_taught = 200;		// fix bug on fly		//1230: it's a place holder and is calculated during init course enrollment

	deptInfo->max_dl_class_size = m.atoi( deptRec->max_dl_class_size, 9 );

	deptInfo->salary_multiplier   = (float) m.atof( deptRec->salary_multiplier, 9 );
	deptInfo->teaching_load_multiplier = (float) m.atof( deptRec->teaching_load_multiplier, 9 );
	deptInfo->research_per_faculty_multiplier = (float) m.atof( deptRec->research_per_faculty_multiplier, 9 );
	deptInfo->female_multiplier   = (float) m.atof( deptRec->female_multiplier, 9 );
	deptInfo->minority_multiplier = (float) m.atof( deptRec->minority_multiplier, 9 );

	deptInfo->talent_teaching    = m.atoi( deptRec->talent_teaching, 9 );
	deptInfo->talent_scholarship = m.atoi( deptRec->talent_scholarship, 9 );
	deptInfo->talent_research    = m.atoi( deptRec->talent_research, 9 );

	deptInfo->percent_pi_faculty = char(100 * m.atof( deptRec->percent_faculty_pi, 9 ));
	//## chea 061299 1.8.2
	//		deptInfo->normal_research_project_size = m.atoi( deptRec->research_project_size, 9 );
	deptInfo->normal_research_project_size =2 * m.atoi( deptRec->research_project_size, 9 );

	deptInfo->doctoral_grad_rate_multiplier     = (float) m.atof( deptRec->doctoral_graduation_rate_multiplier, 9 );
	deptInfo->doctoral_time_to_dropout_multiplier = (float) m.atof( deptRec->doctoral_time_to_dropout_multiplier, 9 );
	deptInfo->doctoral_time_to_degree_multiplier    = (float) m.atof( deptRec->doctoral_time_to_degree_multiplier, 9 );
	deptInfo->graduate_tuition_rate_multiplier    = (float) m.atof( deptRec->graduate_tuition_rate_multiplier, 9 );

	memcpy( deptInfo->template_rank_and_age, deptRec->template_rank_and_age, 1 );
	memcpy( deptInfo->template_gender_ethnic, deptRec->template_gender_ethnic, 1 );
	memcpy( deptInfo->template_salary, deptRec->template_salary, 1 );
	memcpy( deptInfo->template_talent, deptRec->template_talent, 1 );
	memcpy( deptInfo->template_course_load, deptRec->template_course_load, 1 );

	memcpy( deptInfo->template_discretionary_time, deptRec->faculty_discretionary_time, 1 );
	memcpy( deptInfo->template_course_type, deptRec->course_type, 2 );

	memcpy( deptInfo->base_fac_discretionary_time, deptRec->base_fac_discretionary_time, 1);

	change_templ_code(deptInfo->template_rank_and_age);
	change_templ_code(deptInfo->template_gender_ethnic);
	change_templ_code(deptInfo->template_salary);
	change_templ_code(deptInfo->template_talent);
	change_templ_code(deptInfo->template_course_load);
	change_templ_code(deptInfo->template_discretionary_time);
	change_templ_code(deptInfo->base_fac_discretionary_time);

	course_res.change_course_templ_code(deptInfo->template_course_type);
    }

    //-----------------------------------------//

    DepartmentRec2 *deptRec2;
    Database    *dbDepartment2 = game_set.open_db(DEPARTMENT_DB2);

    err_when( department_count != (short) dbDepartment2->rec_count());

    for( int i=0 ; i<department_count ; i++ ) {
	deptRec2  = (DepartmentRec2*) dbDepartment2->read(i+1);
	deptInfo  = info_array+i;

	//----- read data from database dept2 -----//

	deptInfo->iped_field = m.atoi( deptRec2->iped_field, 2) - 1;
	err_when( deptInfo->iped_field < 0 || deptInfo->iped_field >= IPED_FIELD_COUNT );

	deptInfo->field_fraction_pct[SL_UG] = (float) m.atof( deptRec2->field_fraction_pct_sl1n2, 10);
	deptInfo->field_fraction_pct[SL_MASTER] = (float) m.atof( deptRec2->field_fraction_pct_sl3, 9);
	deptInfo->field_fraction_pct[SL_DOCTOR] = (float) m.atof( deptRec2->field_fraction_pct_sl4, 9);
	deptInfo->field_fraction_pct[DISTANCE_LEARN] = (float) m.atof( deptRec2->field_fraction_pct_sl5, 9);
    }

    //-----------------------------------------//

    init_facility_info();

    DepartmentInfo  *gdeptInfo;
    DepartmentRec3 *deptRec3;
    Database    *dbDepartment3 = game_set.open_db(DEPARTMENT_DB3);

    err_when( department_count + 1 != (short) dbDepartment3->rec_count());

    for( int i=0 ; i<department_count+1 ; i++ ) {
	if ( i < department_count ) {
	    // dbf has been arrange
	    deptRec3  = (DepartmentRec3*) dbDepartment3->read(i+1);
	    if( i == 0 ) // 1st record is for general department
		gdeptInfo = &general_dept_info;
	    else
		gdeptInfo  = info_array+i-1;
	}
	else
	    gdeptInfo = &general_dept_info;

	//----- read data from database dept3 -----//

	gdeptInfo->replacement_cost = m.atoi( deptRec3->replacement_cost, 9);
	gdeptInfo->normal_onm_cost = (float) m.atof( deptRec3->normal_onm_cost, 9);
	gdeptInfo->months_to_construct = (char) m.atoi( deptRec3->months_to_construct, 9);

	gdeptInfo->sf_per_course_enrollment_in_seminars = (float) m.atof( deptRec3->sf_per_course_enrollment, 9);
	gdeptInfo->sf_per_course_enrollment_in_general_courses = (float) m.atof( deptRec3->sf_per_course_enrollment, 9);
	gdeptInfo->sf_per_course_enrollment_in_lecture_courses = (float) m.atof( deptRec3->sf_per_course_enrollment, 9);

	gdeptInfo->sf_per_faculty = (float) m.atof( deptRec3->sf_per_faculty, 9);
	gdeptInfo->sf_per_research_dollar = (float) m.atof( deptRec3->sf_per_research_dollar, 9);
	gdeptInfo->sf_per_dollar_of_central_exp = (float) m.atof( deptRec3->sf_per_dollar_of_central_exp, 9);
	gdeptInfo->fixed_sf_per_dept = (float) m.atof( deptRec3->fixed_sf_per_dept, 9);
    }

    //----//
}

//--------- End of function DepartmentRes::load_info ---------//

//##### begin fred 0730 #####//
//---------- Begin of function DepartmentRes::load_faculty_info -----------//
//!
void DepartmentRes::load_faculty_info() {
    DepartmentFacultyRec  *deptRec;
    DepartmentInfo *deptInfo;
    Database    *dbDeptFac = game_set.open_db(FACULTY_TRANSITION_PROBABILITY);

    short dbCount = (short) dbDeptFac->rec_count();

    //------ read in faculty transition probability array -------//

    // update these three variable in deptInfo

    //float	associate_professor_departure_probability[PROB_SCHOOL_TYPE_COUNT];
    //float	assistant_professor_departure_probability[PROB_SCHOOL_TYPE_COUNT];
    //enum { ASST_PROF, ASSOC_PROF, FULL_PROF, LONG_TERM_ADJUNCT, SHORT_TERM_ADJUNCT };	// 3 professers & 2 adjuncts

    err_when( dbCount !=  department_count * 7 );

    for( int i=0 ; i<dbCount; i++ ) {
	deptRec   = (DepartmentFacultyRec*) dbDeptFac->read(i+1);
	deptInfo  = info_array+i%department_count;

	if ( i < department_count )
	    load_faculty_info_sub((deptInfo->assistant_professor_promotion_probability), deptRec);
	else if ( i < department_count*2 )
	    load_faculty_info_sub((deptInfo->faculty_departure_probability[ASST_PROF]), deptRec);
	else if ( i < department_count*3 )
	    load_faculty_info_sub((deptInfo->associate_professor_promotion_probability), deptRec);
	else if ( i < department_count*4 )
	    load_faculty_info_sub((deptInfo->faculty_departure_probability[ASSOC_PROF]), deptRec);
	else if ( i < department_count*5 )
	    load_faculty_info_sub((deptInfo->faculty_departure_probability[FULL_PROF]), deptRec);
	else if ( i < department_count*6 )
	    load_faculty_info_sub((deptInfo->faculty_departure_probability[LONG_TERM_ADJUNCT]), deptRec);
	else if ( i < department_count*7 )
	    load_faculty_info_sub((deptInfo->faculty_departure_probability[SHORT_TERM_ADJUNCT]), deptRec);
    }
}

//---------- End of function DepartmentRes::load_faculty_info -----------//

//---------- Begin of function DepartmentRes::load_faculty_info_sub -----------//
//!
void DepartmentRes::load_faculty_info_sub(float trans_prob[PROB_SCHOOL_TYPE_COUNT], DepartmentFacultyRec *deptRec) { {
    // for (int s=0; s<PROB_SCHOOL_TYPE_COUNT; s++)

    trans_prob[NATIONAL] = (float) m.atof(deptRec->trans_prob[0], 9);

    trans_prob[REGIONAL] = (float) m.atof(deptRec->trans_prob[1], 9);

    trans_prob[LOCAL] = (float) m.atof(deptRec->trans_prob[2], 9);
}
}

//---------- End of function DepartmentRes::load_faculty_info_sub -----------//

//---------- Begin of function DepartmentRes::load_course_info -----------//
//!
void DepartmentRes::load_course_info() {
    //***
    //ASKBILLok order of departments in the curri_requriments is ***WRONG***
    //***

    DepartmentUGCourseRec     *deptUGRec;
    DepartmentInfo *deptInfo;
    Database    *dbDept = game_set.open_db(COURSE_UG_DB);

    short dbCount = (short) dbDept->rec_count();
    char rowsPerDept = 4;

    //------ read in course curriculam requirment array -------//

    // 1 for general education
    err_when( dbCount != (department_count + 1) * rowsPerDept );

    // a. Special case handle the "department": general education
    //
    deptUGRec   = (DepartmentUGCourseRec*) dbDept->read(1);

    for ( int e=0; e<BACHELOR_YEAR_COUNT; e++ )
	general_dept_info.bachelor_elective_year_count[e] = (char) m.atoi(deptUGRec->elective[e], 9);

    deptUGRec   = (DepartmentUGCourseRec*) dbDept->read(2);
    load_course_info_sub(general_dept_info.bachelor_depth1, (DepartmentCourseRec *)deptUGRec);

    deptUGRec   = (DepartmentUGCourseRec*) dbDept->read(3);
    load_course_info_sub(general_dept_info.bachelor_depth2, (DepartmentCourseRec *)deptUGRec);

    deptUGRec   = (DepartmentUGCourseRec*) dbDept->read(4);
    load_course_info_sub(general_dept_info.bachelor_depth3, (DepartmentCourseRec *)deptUGRec);

    // b. Genernal case
    //
    for( int i=4 ; i<dbCount; i++ ) {
	deptUGRec   = (DepartmentUGCourseRec*) dbDept->read(i+1);
	deptInfo  = info_array + (i-4)/rowsPerDept;

	switch ( i%rowsPerDept ) {
	case 0:
	    for ( int e=0; e<BACHELOR_YEAR_COUNT; e++ )
		deptInfo->bachelor_elective_year_count[e] = (char) m.atoi(deptUGRec->elective[e], 9);

	    //this is a dummy "total" row in the db

	    //bughere0 elective course has depth!!!  // 1229
	    break;
	case 1:
	    load_course_info_sub(deptInfo->bachelor_depth1, (DepartmentCourseRec *)deptUGRec);
	    break;
	case 2:
	    load_course_info_sub(deptInfo->bachelor_depth2, (DepartmentCourseRec *)deptUGRec);
	    break;
	case 3:
	    load_course_info_sub(deptInfo->bachelor_depth3, (DepartmentCourseRec *)deptUGRec);
	    break;
	}
    }

    //----- MASTER ------//

    DepartmentMasterCourseRec  *deptMasterRec;

    dbDept = game_set.open_db(COURSE_MASTER_DB);
    dbCount = (short) dbDept->rec_count();
    rowsPerDept = 3;

    err_when( dbCount != department_count * rowsPerDept );

    for( int i=0 ; i<dbCount; i++ ) {
	deptMasterRec   = (DepartmentMasterCourseRec*) dbDept->read(i+1);
	deptInfo  = info_array + i/rowsPerDept;

	switch ( i%rowsPerDept ) {
	case 0:
	    deptInfo->master_elective_year_count = 0; // (char) m.atoi(deptMasterRec->elective, 9);
	    //this is a dummy "total" row in the db

	    //TODO 1229 master has no elective, so remove class member: deptInfo->master_elective_year_count
	    break;
	case 1:
	    load_course_info_sub(deptInfo->master_depth3, deptMasterRec);
	    break;
	case 2:
	    load_course_info_sub(deptInfo->master_graduate, deptMasterRec);
	    break;
	}
    }

    //----- Doctor ------//

    DepartmentCourseRec  *deptRec;

    dbDept = game_set.open_db(COURSE_DOCTOR_DB);
    dbCount = (short) dbDept->rec_count();
    rowsPerDept = 2;

    err_when( dbCount != department_count * rowsPerDept );

    for( int i=0 ; i<dbCount; i++ ) {
	deptRec   = (DepartmentCourseRec*) dbDept->read(i+1);
	deptInfo  = info_array + i/rowsPerDept;

	switch ( i%rowsPerDept ) {
	case 0:
	    //this is a dummy "total" row in the db
	    break;
	case 1:
	    load_course_info_sub(deptInfo->doctor_graduate, deptRec);
	    break;
	}
    }
}

//---------- End of function DepartmentRes::load_course_info -----------//

//---------- Begin of function DepartmentRes::load_course_info_sub -----------//
//!
void DepartmentRes::load_course_info_sub(char required[COURSE_GAME_FIELD_COUNT], DepartmentCourseRec *deptRec) {
    for (int s=0; s<COURSE_GAME_FIELD_COUNT; s++) {
	required[s] = (char) m.atoi(deptRec->required[s], 9);
    }
}

//---------- End of function DepartmentRes::init_course_info_sub -----------//

//---------- Begin of function DepartmentRes::load_facility_info() -----------//
//!
//! for facility office;
//!
void DepartmentRes::init_facility_info() {
    DepartmentInfo *deptInfo;

    //DepartmentFacilityRec  *deptRec;
    //Database 		*dbDepartment = game_set.open_db(DEPARTMENT_DB);

    //department_count = (short) dbDepartment->rec_count();

    //------ read in department information array -------//

    for( int i=0 ; i<department_count; i++ ) {
	//deptRec   = (DepartmentRec*) dbDepartment->read(i+1);
	//deptInfo  = (GeneralDepartmentInfo *) (department_res.info_array+i);
	deptInfo  = info_array+i;

	// Parameters for replacement and normal operations and maintenance cost
	deptInfo->replacement_cost = 200;
	// 0.99->99%
	deptInfo->cost_growth = math.get_random_snd(0.030f, .01f);
	deptInfo->cost_growth_last2 = 0;
	deptInfo->normal_onm_cost = 4.0f;
	deptInfo->months_to_construct = 14;

	// Parameters for calculating normal square footage
	deptInfo->sf_per_course_enrollment_in_seminars = 6;
	deptInfo->sf_per_course_enrollment_in_general_courses = 5;
	deptInfo->sf_per_course_enrollment_in_lecture_courses = 4;

	deptInfo->sf_per_faculty = 2000;
	deptInfo->sf_per_research_dollar = 100;
	deptInfo->sf_per_dollar_of_central_exp = 0;
	deptInfo->fixed_sf_per_dept = 5000;

	// initialize variables
	deptInfo->actual_sf = -1;                     // important! for facilities model to initialize this
    }

    general_dept_info.replacement_cost = 180;
    general_dept_info.cost_growth = 0;
    general_dept_info.cost_growth_last2 = 0;
    general_dept_info.normal_onm_cost = 3.5f;
    general_dept_info.months_to_construct = 13;

    // three dummy field for general deparment
    general_dept_info.sf_per_course_enrollment_in_seminars = 0;
    general_dept_info.sf_per_course_enrollment_in_general_courses = 0;
    general_dept_info.sf_per_course_enrollment_in_lecture_courses = 0;
    general_dept_info.sf_per_faculty = 0;
    general_dept_info.sf_per_research_dollar = 0;

    general_dept_info.sf_per_dollar_of_central_exp = 5;
    general_dept_info.fixed_sf_per_dept = 50000;

    // initialize variables
    general_dept_info.actual_sf = -1;
}

//---------- End of function DepartmentRes::init_facility_info() -----------//

//---------- Begin of function DepartmentRes::load_elective_course_pref -----------//
//!
void DepartmentRes::load_elective_course_pref() {
    ElecPrefRec  *deptRec;

    DepartmentInfo *deptInfo;
    Database  *dbDept = game_set.open_db(ELECTIVE_PREF_DB);
    short dbCount = (short) dbDept->rec_count();

    err_when(dbCount != department_count);

    for( int i=1; i<=department_count ; i++ ) {
	float totalPref=0;
	deptInfo = department_res[i];

	deptRec   = (ElecPrefRec*) dbDept->read(i);

	for( int j=0 ; j<department_count ; j++ ) {
	    deptInfo->bachelor_elective_course_pref[j] = 100 * (float) m.atof( deptRec->pref[j], 9);

	    // varsto be removed
	    deptInfo->master_elective_course_pref[j]   = 100.0f / (department_count);

	    //----- verify if the total is 100 -----//

	    totalPref += deptInfo->bachelor_elective_course_pref[j];
	}

	// error difference is too big, having taking into account of the floating point error
	err_when( totalPref <= 99 || totalPref >= 101 );
    }

    //------- initialize general_dept_info --------//

    for( int j=0 ; j<department_count ; j++ ) {
	general_dept_info.bachelor_elective_course_pref[j] = 100.0f / department_count;
    }
}

//---------- End of function DepartmentRes::load_elective_course_pref -----------//

//----- Begin of function DepartmentRes::calc_total_elective_course_pref -----//
//!
//! It calculates the total preference weights for all existing departments.
//!
void DepartmentRes::calc_total_elective_course_pref() {
    for( int i=0; i<=department_count; i++ ) {
	DepartmentInfo* deptInfo = department_res[i]; // 0 points to the general department

	deptInfo->total_bachelor_elective_course_pref = 0;
	deptInfo->total_master_elective_course_pref = 0;

	//---- only add preference for existing departments ---//

	for (int r=department_array.size(); r>0; r--) {
	    if( department_array.is_deleted(r) )
		continue;

	    int deptId = department_array[r]->department_id;

	    deptInfo->total_bachelor_elective_course_pref +=
		deptInfo->bachelor_elective_course_pref[deptId-1];

	    deptInfo->total_master_elective_course_pref +=
		deptInfo->master_elective_course_pref[deptId-1];

	    err_when(department_array[r]->department_recno != r);
	}

    }
}

//------ End of function DepartmentRes::calc_total_elective_course_pref ------//

//---------- Begin of function DepartmentRes::school_type_2_prob_school_type -----------//
//!
int DepartmentRes::get_prob_school_type() {
    return player_school.student_market;
}

//------ End of function DepartmentRes::calc_total_elective_course_pref ------//

//---------- Begin of function DepartmentRes::operator[] -----------//
//!
DepartmentInfo* DepartmentRes::operator[](int deptId) {
    if ( deptId==0 )
	return &general_dept_info;

    err_when( deptId < 1 || deptId > department_count );

    return info_array + deptId - 1;
}

//---------- End of function DepartmentRes::operator[] -----------//
