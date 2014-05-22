//Filename    : OFACURES.CPP
//Description : Faculty Resource object

#include <OSYS.H>
#include <OGAMESET.H>
#include <OFACURES.H>

//---------- #define constant ------------//

#define FACULTY_TEMPLATE_DB   "FAC_TMPL"

//------- Begin of function FacultyRes::FacultyRes -----------//
enum {
    REPORT_TYPE_DEPARTMENT_PROFILE1,
    REPORT_TYPE_DEPARTMENT_PROFILE2,
    REPORT_TYPE_DEPARTMENT_ACTIVITIES,
    REPORT_TYPE_DEPARTMENT_RESEARCH,
    REPORT_TYPE_DEPARTMENT_HIRING,
    REPORT_TYPE_DEPARTMENT_DIRECTORY,
    REPORT_TYPE_DEPARTMENT_TURNOVER,
    REPORT_TYPE_INSTITUTION_PROFILE1,
    REPORT_TYPE_INSTITUTION_PROFILE2,
    REPORT_TYPE_INSTITUTION_ACTIVITIES,
    REPORT_TYPE_INSTITUTION_RESEARCH,
    REPORT_TYPE_INSTITUTION_SALARY,
    REPORT_TYPE_INSTITUTION_PROMOTION,
    REPORT_TYPE_INSTITUTION_TURNOVER,
};

FacultyRes::FacultyRes() {
    memset( this, 0, sizeof(FacultyRes) );
}

//--------- End of function FacultyRes::FacultyRes -----------//

//---------- Begin of function FacultyRes::init -----------//
//!
//! This function must be called after a map is generated.
//!
void FacultyRes::init() {
    deinit();

    //------- load database information --------//

    memset( this, 0, sizeof(FacultyRes) );

    load_template();

    init_flag=1;
    current_report_type=REPORT_TYPE_INSTITUTION_PROFILE1;
}

//---------- End of function FacultyRes::init -----------//

//---------- Begin of function FacultyRes::deinit -----------//

void FacultyRes::deinit() {
    if( init_flag ) {
	if( faculty_template_count > 0 )
	    free_template();

	// ##### begin Gilbert 20/04/2001 #####///
	summary_report_deinit();                      // free memory allocated by faculty report
	// ##### end Gilbert 20/04/2001 #####///
	init_flag=0;
    }
}

//---------- End of function FacultyRes::deinit -----------//

//------- Begin of function FacultyRes::load_template -------//
//!
void FacultyRes::load_template() {
    err_when( faculty_template_count != 0 );

    FacultyTemplateRec    *templateRec;
    FacultyTemplate       *templatePtr;
    Database        *dbTemplate = game_set.open_db(FACULTY_TEMPLATE_DB);

    faculty_template_count = (short) dbTemplate->rec_count();

    err_when( faculty_template_array );             // to prevent double allocations

    faculty_template_array = (FacultyTemplate*) mem_add( sizeof(FacultyTemplate)*faculty_template_count );

    //------ read in faculty information array -------//

    memset( faculty_template_array, 0, sizeof(FacultyTemplate) * faculty_template_count );

    for( int i=0 ; i<faculty_template_count ; i++ ) {
	templateRec  = (FacultyTemplateRec*) dbTemplate->read(i+1);
	templatePtr = faculty_template_array+i;

	//##### fred 0730 #####//

	templatePtr->template_code = templateRec->template_code[0];

	err_when( templatePtr->template_code < FIRST_TEMPLATE_CODE || templatePtr->template_code > 'E' );

	templatePtr->rank_age_group_id = (char) (m.atoi( templateRec->rank_age_group_id, 9 ) - 1);

	err_when( templatePtr->rank_age_group_id > LAST_RANK_AGE_GROUP || templatePtr->rank_age_group_id < FIRST_RANK_AGE_GROUP );

	//----- multipliers -----//
	templatePtr->rank_age_multiplier = (float) m.atof(templateRec->rank_age_multiplier, 9);
	templatePtr->female_multiplier= (float) m.atof(templateRec->female_multiplier, 9);
	templatePtr->minority_multiplier= (float) m.atof(templateRec->minority_multiplier, 9);

	templatePtr->overall_salary_multiplier = (float) m.atof(templateRec->overall_salary_multiplier, 9);
	templatePtr->female_salary_multiplier = (float) m.atof(templateRec->female_salary_multiplier, 9);
	templatePtr->minority_salary_multiplier = (float) m.atof(templateRec->minority_salary_multiplier, 9);

	templatePtr->talent_teaching_multiplier = (float) m.atof(templateRec->talent_teaching_multiplier, 9);
	templatePtr->talent_scholarship_multiplier = (float) m.atof(templateRec->talent_scholarship_multiplier, 9);
	templatePtr->talent_research_multiplier = (float) m.atof(templateRec->talent_research_multiplier, 9);

	templatePtr->normal_teaching_load_multiplier = (float) m.atof(templateRec->normal_teaching_load_multiplier, 9);

	for( int j=0 ; j<DISCRETIONARY_TYPE_COUNT ; j++ ) {
	    templatePtr->discretionary_time_pref[j] = (char) (int) (m.atof(templateRec->discretionary_time_pref[j], 9) * 100);

	    err_when(templatePtr->discretionary_time_pref[j] < 0);
	}
    }
}

//--------- End of function FacultyRes::load_template ---------//

//------- Begin of function FacultyRes::free_template -------//
//!
void FacultyRes::free_template() {
    err_when( faculty_template_count == 0 );

    mem_del(faculty_template_array);
    faculty_template_count = 0;
}

//--------- End of function FacultyRes::free_template ---------//

//------- Begin of function FacultyRes::get_faculty_template -------//
//!
//! <char*> templateCode - if two template codes are given, a random value
//!								  r will be drawn between 0 and 1. Template 1 will
//!								  be weighted using r, template 2 will be weighted
//!								  using 1-r. The result will be a combination
//!								  of template 1 and template 2.
//!
//! return: <FacultyTemplate*> pointer to the first FacultyTemplate record of a series of resultant FacultyTemplate.
//!
FacultyTemplate* FacultyRes::get_faculty_template(char* templateCode, int rankAgeGroup) {
#ifdef READ_OLD_DB
    static FacultyTemplate resultTemplate;

    //--- when two templates are given, the result is a combination of the two ----//

    if( templateCode[1] >= FIRST_TEMPLATE_CODE &&
	templateCode[1] < FIRST_TEMPLATE_CODE + MAX_RANK_AGE_GROUP ) {
	FacultyTemplate* facultyTemplate1 = faculty_template_array +
	    (templateCode[0] - FIRST_TEMPLATE_CODE) * MAX_RANK_AGE_GROUP + rankAgeGroup;

	FacultyTemplate* facultyTemplate2 = faculty_template_array +
	    (templateCode[1] - FIRST_TEMPLATE_CODE) * MAX_RANK_AGE_GROUP + rankAgeGroup;

	//------------------------------------------//

	float r = (float) m.random(100) / (float) 100;

	resultTemplate.rank_age_multiplier = (char) ( float(facultyTemplate1->rank_age_multiplier) * r / (float)1
						      + float(facultyTemplate2->rank_age_multiplier) * ((float)1-r) / (float)1 );

	resultTemplate.female_multiplier = facultyTemplate1->female_multiplier * r / (float)1
	    + facultyTemplate2->female_multiplier * ((float)1-r) / (float)1;

	resultTemplate.minority_multiplier = facultyTemplate1->minority_multiplier * r / (float)1
	    + facultyTemplate2->minority_multiplier * ((float)1-r) / (float)1;

	resultTemplate.overall_salary_multiplier = facultyTemplate1->overall_salary_multiplier * r / (float)1
	    + facultyTemplate2->overall_salary_multiplier * ((float)1-r) / (float)1;

	resultTemplate.female_salary_multiplier = facultyTemplate1->female_salary_multiplier * r / (float)1
	    + facultyTemplate2->female_salary_multiplier * ((float)1-r) / (float)1;

	resultTemplate.minority_salary_multiplier = facultyTemplate1->minority_salary_multiplier * r / (float)1
	    + facultyTemplate2->minority_salary_multiplier * ((float)1-r) / (float)1;

	resultTemplate.talent_teaching_multiplier = facultyTemplate1->talent_teaching_multiplier * r / (float)1
	    + facultyTemplate2->talent_teaching_multiplier * ((float)1-r) / (float)1;

	resultTemplate.talent_scholarship_multiplier = facultyTemplate1->talent_scholarship_multiplier * r / (float)1
	    + facultyTemplate2->talent_scholarship_multiplier * ((float)1-r) / (float)1;

	resultTemplate.talent_research_multiplier = facultyTemplate1->talent_research_multiplier * r / (float)1
	    + facultyTemplate2->talent_research_multiplier * ((float)1-r) / (float)1;

	resultTemplate.normal_teaching_load_multiplier= facultyTemplate1->normal_teaching_load_multiplier* r / (float)1
	    + facultyTemplate2->normal_teaching_load * ((float)1-r) / (float)1;

	//-------------- discretionary time ---------------//

	for( int i=0 ; i<DISCRETIONARY_TYPE_COUNT ; i++ ) {
	    resultTemplate.discretionary_time_pref[i] =
		char( (float)facultyTemplate1->discretionary_time_pref[i] * r / (float)1
		      + (float)facultyTemplate2->discretionary_time_pref[i] * ((float)1-r) / (float)1 );
	}

	return &resultTemplate;
    }
    else                                            // there is just one template requested, return it now.
#endif

	int offset = templateCode[0] - FIRST_TEMPLATE_CODE;
    err_when( offset < 0 || offset >= faculty_template_count );
    err_when( rankAgeGroup < 0 || rankAgeGroup >= MAX_RANK_AGE_GROUP );

    return faculty_template_array + offset * MAX_RANK_AGE_GROUP + rankAgeGroup;
}

//--------- End of function FacultyRes::get_faculty_template ---------//
