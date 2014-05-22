//Filename    : OFACURES.H
//Description : Faculty Template Class Declaration

#ifndef __OFACURES_H
#define __OFACURES_H

#include <OFACULTY.H>

#ifndef __GAMEDEF_H
#include <GAMEDEF.H>
#endif

//-------- Define constant --------//

#define MAX_RANK_AGE_GROUP    7
#define MAX_RANK_AGE_GROUP_N_TOTAL    (MAX_RANK_AGE_GROUP+1)
#define FIRST_RANK_AGE_GROUP  0                   // zero-based number
#define LAST_RANK_AGE_GROUP MAX_RANK_AGE_GROUP-1

#define FIRST_TEMPLATE_CODE   'A'

//------ Define struct FacultyTemplateRec -------//

//!Structure containing the faculty template data as stored in the
//!database.
struct FacultyTemplateRec {
    char  template_code[9];
    char  rank_age_group_id[9];

    //----- multipliers -----//

    char  rank_age_multiplier[9];
    char  female_multiplier[9];
    char  minority_multiplier[9];

    char  overall_salary_multiplier[9];
    char  female_salary_multiplier[9];
    char  minority_salary_multiplier[9];

    char  talent_teaching_multiplier[9];
    char  talent_scholarship_multiplier[9];
    char  talent_research_multiplier[9];

    char  normal_teaching_load_multiplier[9];

    char  discretionary_time_pref[DISCRETIONARY_TYPE_COUNT][9];
};

//------ Define struct FacultyTemplate -------//

//!Structure containing the faculty template attributes. The faculty
//!templates elaborate the department-level parameters to the faculty
//!age/rank categories. (See Tech Doc 3.3, section 3.3)
struct FacultyTemplate {
    char  template_code;
    char  rank_age_group_id;

    //----- multipliers -----//
    float rank_age_multiplier;
    float female_multiplier;
    float minority_multiplier;

    float overall_salary_multiplier;
    float female_salary_multiplier;
    float minority_salary_multiplier;

    float talent_teaching_multiplier;
    float talent_scholarship_multiplier;
    float talent_research_multiplier;

    float normal_teaching_load_multiplier;          // NTL multiplier

    // discretionary time preference // 0-100
    char  discretionary_time_pref[DISCRETIONARY_TYPE_COUNT];
};

//------- Define class FacultyRes ---------//

//!Collection Class of all Faculty templates and methods for delegating
//!requests to generate repots to the respective templates.
class FacultyRes {
public:
    char          init_flag;
    int         report_type;
    int         report_list_mode;
    int         report_det_mode;
    int         report_faculty_recno;
    int         report_research_recno;

    short           faculty_template_count;
    FacultyTemplate*  faculty_template_array;

    short show_department_detail_count;           // This is just used in Faculty report
    char  is_year_end_report;                     // This is just used in Faculty turnover report

    //--------------------------------------------------//
    // The following vars are for storing percentages
    // that are not realized because the percentage
    // are too small to produce one integer faculty.
    // They are accumulated here, so that they will
    // be added to the following calculations to
    // produce a overall population that reflects
    // the percentages calculated. They are set to zero
    // in init().
    //--------------------------------------------------//

    // since it's a fraction of an integer, so the var type is float
    float         unused_faculty_count_array[GENDER_ETHNIC_TYPE_COUNT];

public:
    FacultyRes();

    void        init();
    void        deinit();

    int     write_file(File* filePtr);
    int     read_file(File* filePtr);

    void      report(int refreshFlag);
    void      report_faculty_hired(int refreshFlag);
    int     report_hired_detect();
    void      report_detect();
    int     current_report_type;
    int     current_year_report;
    int     last_report_type;
    void      summary_report(int refreshFlag);
    void      summary_report_1(int refreshFlag);
    void      summary_report_2(int refreshFlag);
    void      summary_report_3(int refreshFlag);
    void      summary_report_4(int refreshFlag);
    void      summary_report_5(int refreshFlag);
    void      summary_report_6(int refreshFlag);
    void      summary_report_7(int refreshFlag);
    void      summary_report_8(int refreshFlag);
    void      summary_report_9(int refreshFlag);
    void      summary_report_10(int refreshFlag);
    int     summary_report_detect();
    void      summary_report_deinit();

    FacultyTemplate*  get_faculty_template(char* templateCode, int rankAgeGroup);

private:
    void        load_template();
    void      free_template();
    void      report_paint_button(int refreshFlag);
};

//------- Define class FacultySummary ---------//

extern FacultyRes faculty_res;

//-----------------------------------------//
#endif
