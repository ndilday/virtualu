//Filename    : ODEPTRES.H
//Description : Department Class Declaration

#ifndef __ODEPTRES_H
#define __ODEPTRES_H

#include <OFIELD.H>
#include <OFACULTY.H>
#include <OSTUDENT.H>
#include <ODEPTGEN.H>

//------ School type for faculty departure and promotion probability -----//

#define  DEPARTMENT_TYPE_COUNT  28                // the total number of types of departments

#define PROB_SCHOOL_TYPE_COUNT  3                 // in opschool.h enum { LOCAL, REGIONAL, NATIONAL };

// 1 for own deparment
#define COURSE_GAME_FIELD_COUNT   GAME_FIELD_COUNT + 1

//------ Define struct DepartmentRec -------//

//!
struct DepartmentRec {
    enum { NAME_LEN=26 };

    char  name[NAME_LEN];
    char  field_code[9];
    char  total_courses_taught[9];
    char  max_dl_class_size[9];                     //1102 add this?

    //---- multipliers & talent indices -----//

    char  salary_multiplier[9];
    char  teaching_load_multiplier[9];
    char  research_per_faculty_multiplier[9];
    char  female_multiplier[9];
    char  minority_multiplier[9];
    char  talent_teaching[9];
    char  talent_scholarship[9];
    char  talent_research[9];

    char  percent_faculty_pi[9];
    char  research_project_size[9];

    char  doctoral_graduation_rate_multiplier[9];
    char  doctoral_time_to_dropout_multiplier[9];
    char  doctoral_time_to_degree_multiplier[9];
    char  graduate_tuition_rate_multiplier[9];

    //-------- templates -------//

    char  template_rank_and_age[9];
    char  template_gender_ethnic[9];
    char  template_salary[9];
    char  template_talent[9];
    char  template_course_load[9];                  // Normal teaching load

    char  faculty_discretionary_time[9];
    char  course_type[9];
    char  base_fac_discretionary_time[9];
};

// for dept2.dbf
//!
struct DepartmentRec2 {
    enum { NAME_LEN=25 };

    char  name[NAME_LEN];
    char  iped_field[20];
    char  field_fraction_pct_sl1n2[10];
    char  field_fraction_pct_sl3[9];
    char  field_fraction_pct_sl4[9];
    char  field_fraction_pct_sl5[9];
};

// for facility.dbf
//!Structures for holding department data.
struct DepartmentRec3 {
    char  replacement_cost[9];
    char  normal_onm_cost[9];
    char  months_to_construct[9];

    char  sf_per_course_enrollment[9];
    char  sf_per_faculty[9];
    char  sf_per_research_dollar[9];
    char  sf_per_dollar_of_central_exp[9];
    char  fixed_sf_per_dept[9];
};

// for elecpref.dbf
//!Elective preference record?
struct ElecPrefRec {
    char  pref[DEPARTMENT_TYPE_COUNT][9];           // 0.999 => 99.9%
};

//------ Define struct DepartmentFacultyRec -------//

//!Structure for holding faculty information.
struct DepartmentFacultyRec {
    char trans_prob[PROB_SCHOOL_TYPE_COUNT][9];
};

//------ Define struct DepartmentFacultyRec -------//

//!Structure for holding course information.
struct DepartmentCourseRec {
    char  required[COURSE_GAME_FIELD_COUNT][9];
};

//!
struct DepartmentUGCourseRec : DepartmentCourseRec {
    char  elective[BACHELOR_YEAR_COUNT][9];
};

//!
struct DepartmentMasterCourseRec : DepartmentCourseRec {
    //char	elective[9];		// master program only has one year
};

//----------- Define class GeneralDepartmentInfo ---------------//
//
// "extract the same fields from DepartmentInfo for general education
// can be a base class of DepartmentInfo if necessary
//
//!Base class of DepartmentInfo. Needed for handling general department for
//!"undecided" student, and university general facilities for facilities
//!model.
struct GeneralDepartmentInfo {
    //----- curriculum requirment vars -------//

    char bachelor_depth1[COURSE_GAME_FIELD_COUNT];  // bachelor_depth1[0] --> own department
    char bachelor_depth2[COURSE_GAME_FIELD_COUNT];  // each element contains the number of courses a student must take in a department
    char bachelor_depth3[COURSE_GAME_FIELD_COUNT];

                                                  // it contain the number courses a student must take over the years
    char  bachelor_elective_year_count[BACHELOR_YEAR_COUNT];
    // the total of preference weight across all department is 100, it is
    float bachelor_elective_course_pref[DEPARTMENT_TYPE_COUNT];
    float total_bachelor_elective_course_pref;      // the total of pref weights for existing departments

  //----- for facility office
  //
  // sf: square foot

  // Parameters for replacement and normal operations and maintenance cost
    int replacement_cost;                           // unit:	$ per sf		(not $000)
    float cost_growth, cost_growth_last2;           //## fred // 1026 for calc time variation // 0.99%
    float normal_onm_cost;                          // unit: $ per sf    (not $000)
    char  months_to_construct;

  // Parameters for calculating normal square footage
    float sf_per_course_enrollment_in_seminars;
    float sf_per_course_enrollment_in_general_courses;
    float sf_per_course_enrollment_in_lecture_courses;

    float sf_per_faculty;
    float sf_per_research_dollar;                   // per thousand dollars
    float sf_per_dollar_of_central_exp;             // per thousand dollars
    float fixed_sf_per_dept;

    // initermediate variables; not from db!!!
    int normal_sf, actual_sf;
    int projected_actual_sf;
    // $000
    float actual_new_debt, required_draw_on_capital_reserve, invest_close_gap, additional_debt;
};

//----- Define struct DepartmentInfo -------//

//!Contains information about a department, including its students and
//!faculty. Calculates the number of majors in the department, including
//!the number that switch to other departments.
struct DepartmentInfo : public GeneralDepartmentInfo {
    // fred 0620:  enum { NAME_LEN=50 };
    enum { NAME_LEN=40 };

    char  name[NAME_LEN+1];
    short field_id;

    // short total_courses_taught;	// 0406 it's a place holder in excel file
    short max_dl_class_size;                        // 1102

    //------ multipliers ------//

    float salary_multiplier;
    float teaching_load_multiplier;
    float research_per_faculty_multiplier;
    float female_multiplier;
    float minority_multiplier;

    //------ multipliers 0616 fred; for init student database ------//

    float doctoral_grad_rate_multiplier;
    float doctoral_time_to_dropout_multiplier;
    float doctoral_time_to_degree_multiplier;
    float graduate_tuition_rate_multiplier;

    //---- faculyt parameter-----//

    char  talent_teaching;
    char  talent_scholarship;
    char  talent_research;

    char  percent_pi_faculty;                       // 1117	// 99->99%
    int normal_research_project_size;

    //-------- templates -------//

    char  template_rank_and_age[1];
    char  template_gender_ethnic[1];
    char  template_salary[1];
    char  template_talent[1];
    char  template_course_load[1];

    char  template_discretionary_time[1];           //1116

    char  template_course_type[2];                  //1105

    char  base_fac_discretionary_time[1];           //1119

    //----- for database dept2 -----//

    char  iped_field;
    float field_fraction_pct[4];

    //----- for generate_student database -----//

    float doctor_dropout_trans_prob[MAX_GRADUATE_YEARS];
    float doctor_graduate_trans_prob[MAX_GRADUATE_YEARS];

    float doctor_year_in_program_pct[MAX_GRADUATE_YEARS+1];
    float student_major_pref[MAX_STUDENT_LEVEL_INI];

    //------ faculty promotion and departure probability ------//

    // 0.99->99%
    float faculty_departure_probability[FACULTY_RANK_LEVEL_COUNT][PROB_SCHOOL_TYPE_COUNT];
    float associate_professor_promotion_probability[PROB_SCHOOL_TYPE_COUNT];
    float assistant_professor_promotion_probability[PROB_SCHOOL_TYPE_COUNT];

    //------ required and elective courses tables ------//

    char master_depth3[COURSE_GAME_FIELD_COUNT];
    char master_graduate[COURSE_GAME_FIELD_COUNT];
    char master_elective_year_count;                // it contain the number courses a student must take over the years, master program only has one year

    // the total of preference weight across all department is 100, it is
    float master_elective_course_pref[DEPARTMENT_TYPE_COUNT];
    float total_master_elective_course_pref;

    char doctor_graduate[COURSE_GAME_FIELD_COUNT];

public:
    //---- functions for generating student ----//

    void  generate_student_transition_prob_n_year_in_program_doctor();
    void  generate_student_major_preference();
};

//----------- Define class DepartmentRes ---------------//

//!Class for initializing and loading faculty members for a department.
class DepartmentRes {
public:
    char            init_flag;

    short           department_count;
    DepartmentInfo*   info_array;

    DepartmentInfo    general_dept_info;          // = general_education_department, although we just need GeneralDepartmentInfo, we still use DepartmentInfo for ease of programming
    GeneralDepartment general_dept;               // year 1 ug student without major

public:
    DepartmentRes();
    ~DepartmentRes();

    void        init();
    void        deinit();

    int         write_file(File*);
    int         read_file(File*);

    void        calc_total_elective_course_pref();// this function should be called whenever a department is added or deleted, it updates adjusted_elective_course_rpef

    int     get_prob_school_type();

    DepartmentInfo*   operator[](int departmentId);

private:
    void        load_info();
    void      load_faculty_info();
    void      load_faculty_info_sub(float trans_prob[PROB_SCHOOL_TYPE_COUNT], DepartmentFacultyRec *deptRec);
    void      load_course_info();
    void      load_course_info_sub(char required[COURSE_GAME_FIELD_COUNT], DepartmentCourseRec *deptRec);
    void      load_elective_course_pref();
    void      init_facility_info();
};

extern DepartmentRes department_res;

//---------------------------------------------//
#endif
