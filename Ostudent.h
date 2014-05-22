//Filename    : OStudent.h
//Description : Student Class Declaration

#ifndef __OSTUDENT_H
#define __OSTUDENT_H

#ifndef __GAMEDEF_H
#include <GAMEDEF.H>
#endif

#ifndef __ODYNARRB_H
#include <ODYNARRB.H>
#endif

#ifndef __OCOURSE_H
#include <OCOURSE.H>
#endif

#include <GAMEDEF.H>

//----------- Define constant ----------//

#define MAX_STUDENT_LEVEL   5
#define MAX_STUDENT_LEVEL_INI 4                   // for student database init; see student_par spreadsheet 4: ug, master and distance learn // 0107
#define MAX_GRADUATE_YEARS    8                   // for calc transistion probabilities for student techdoc 3.3, section 4.2

enum {                                            // student_level
    UG_TRADITION, UG_NONTRADITION, MASTER, DOCTOR, DISTANCE_LEARN
};

enum {
    DEGREE_TYPE_COUNT = 3,                          // for interface Student: Activities

    BACHELOR_DEGREE = 0,                            // sl-1 only
    MASTER_DEGREE,                                  // sl-3
    DOCTOR_DEGREE,                                  // sl-4
};

enum {
    MAX_STUDENT_LEVEL_N_TOTAL = MAX_STUDENT_LEVEL+1,
    MAX_STUDENT_LEVEL_TOTAL = MAX_STUDENT_LEVEL,
};
//##### fred 0928 #####//

enum {                                            // for array with data of UG_TRADITION, UG_NONTRADITION, MASTER
    MASTER_ARRAY_SIZE = MASTER + 1
};

enum {                                            // student_level, for student database init; see student_par spreadsheet
    SL_UG, SL_MASTER, SL_DOCTOR, SL_DISTANCE_LEARN
};

enum {                                            // division_status
    UPPER_DIVISION=1, LOWER_DIVISION
};

//---- Define the normal number of years for different degree types ----//

enum {
    BACHELOR_YEAR_COUNT = 4,
    MASTER_YEAR_COUNT = 1,
    DOCTOR_YEAR_COUNT = 2
};

//------ constants for for Oenroll.h -------//
enum {
    TALENT_INCOME_VAR_COUNT = 4,
    TALENT_VAR_COUNT = 3,                           // not include INCOME

    ACADEMIC = 0, EXTRA_CURRI, ATHLETIC, INCOME,
};

//--------- Define static function -------//
static inline bool is_nontradition_ug_group(char sl) {
    return sl == UG_NONTRADITION || sl == DISTANCE_LEARN;
}

//--------- Define static function -------//
static inline bool is_ug_group(char sl) {
    return sl == UG_TRADITION || sl == UG_NONTRADITION || sl == DISTANCE_LEARN;
}

//--------- Define class Student -------//

//!Class for representing a member of the student body, including personal
//!information, course history, and talents. Contains methods for computing
//!satisfaction, selcting courses, and thinking/deciding on future
//!educational pursuits. Responds to next_day() and read/write. See Tech
//!doc 3.3 section 4.
class Student {
public:
    //--- vars that will remain constant once initialized ---//
    char  audit_flag;
    int file_course_id;
    int old_student_recno;

    //	short	student_recno;
    int student_recno;                            //## chea 141099

    short department_recno;                       // this means major field of the student

    char  student_level;
    char  gender_ethnic_group;
    char  student_segment;                        //#### fred 0917//

    int date_admitted;
    int   year_admitted();

    //------- vars that will change over time ------//

    char  year_in_program;

    int   division_status()     { return year_in_program<=2 ? UPPER_DIVISION : LOWER_DIVISION; }
    float academic_achievement;                   // a percentage: 0 to 100
    int financial_aid;

    // FOR DESIGN1028_RESPONSE_FUNC.TXT
    float initializationValueAC_LIBRARIES;        // ## CHWG290699	//func 29
    float initializationValueAC_ATHLETICS;        //func 30
    //------- vars pertaining to courses ------//

    // the number of courses the student has to take in each trimester in a year, this array is set based on the summer teaching policy, the total of the values in the array should be 8 (STD_COURSES_PER_TRIMESTER*2)
    char  course_per_trimester[TRIMESTER_PER_YEAR];

    char  total_course_all;                       // this is the total of all elements in course_to_take_array[]
    char  total_elective_course_this_year;        // total number of elective courses taken and to take this school year

    // courses taken before, taking now and will be taking next trimester
    char  course_taken_array[MAX_DEPARTMENT][MAX_COURSE_DEPTH_WITHOUT_GR];
    char  course_taking_array[MAX_DEPARTMENT];    // courses taking this trimester, taking now and will be taking next trimester

    //##### begin fred 980915 #####//
    char  failed_course_count;                    // accumulated value from year 1 till graduate
    char  course_denials_this_trimester;
    char  course_failures_this_trimester;
    //##### end fred 980915 #####//

    char  off_quarter;                            // = AUTUMN, WINTER or SUMMER

    //---- talent and satisfaction/performance indices ----//

    //##### begin fred 980917 #####//

    char  talent_academic;
    char  talent_extracurricular;
    char  talent_athletics;

    char  satisfaction_academic;
    char  satisfaction_student_life;
    char  satisfaction_athletics;
    char  satisfaction_overall;
    char  faculty_workweek;

    // for calc satisfaction_overall
    char  reaction_summer_course, reaction_summer_course_last;

    float probability_change_major;
    float multiplier_on_dropout_prob;

    //--- extra effect for chance card ---//

    float extra_effect_satisfaction_academic;

    //-----------------------------------------//

public:
    void  init(char studentLevel, char yearInProgram, char genderEthnicGroup, char stuSeg, char major, int finAid, float* talentArr);

    int write_file(File* filePtr);
    int read_file(File* filePtr);

    void  next_day();
    void  next_trimester();
    void  select_course(int ignoreFaculty=0, int initPrevYear=0);

    int  change_major(int couCompleted);          //## chea 170699

private:
    //------ functions for assigning courses -------//
    int   select_course2(int& isRequiredCourse, int ignoreFaculty, int initPrevYear );

    int   assign_course(int selectedCourseDept, int selectedCourseDepth,
			int selectedCourseFocus, int selectedTeachingMethod,
			int isRequiredCourse, int ignoreFaculty, int initPrevYear );

    int   get_available_faculty(int departmentRecno);
    int select_required_course(int &selectedCourseDept, int& selectedCourseDepth, int &selectedCourseFocus);
    void  select_elective_course(int &selectedCourseDept, int& selectedCourseDepth, int &selectedCourseFocus);
    int   select_teaching_method(int selectedCourseDept);

    int   study_course(int studyDeptRecno);

    int   total_course_taken_dept(int departmentRecno);
    int has_selected_all_course();

    //--------- other functions --------//

    int   think_graduate();
    //##### begin fred 980915 #####//
    int think_dropout();
    int think_change_major(char courseCompleted=0);
    void  think_summer_course();
    //##### end fred 980915 #####//

    //------ functions for performance indicators -------//
public:
    void  update_history(char update_flag);
    void  calc_satisfaction_academic();

private:
    void  calc_satisfaction_student_life();
    void  calc_satisfaction_athletics();
    void  calc_satisfaction_overall();
    void  calc_faculty_workweek();

    void  calc_probability_change_major();
    void  calc_multiplier_on_dropout_prob();
};

//----------- Define class StudentArray -----//

//!Collection class containing all of the students in the school as well as
//!methods for summarizing student data. Delegates calls to next_day() and
//!read/write to each student.
class StudentArray : public DynArrayB {
public:
    int     department_recno;
    int     student_count;

    //------------------------------------------//

public:
    StudentArray();
    ~StudentArray();

    int     write_file(File* filePtr);
    int     read_file(File* filePtr);

    int       add(char studentLevel, char yearInProgram, char genderEthnicGroup, char stuSeg, char major, int finAid, float* talentArr);

    void      del(int recNo);

    void      next_day();
    void      next_trimester();

    void      init_course(int processYearInProgram);

    int     is_deleted(int recNo);

#ifdef DEBUG
    Student*  operator[](int recNo);
#else
    Student*  operator[](int recNo) { return (Student*) get(recNo); }
#endif

    //---- functions for course selection ---//

    void      select_course(int ignoreFaculty=0);

    //##### begin fred 980921 #####//
    //----- for performance indicator -----//
public:
    //----------------//

    // ##### Begin MArco ##### //
    char  initial_talent_academic_all[DEGREE_TYPE_COUNT][HISTORY_MONTH_COUNT];
    char  initial_performance_academic_ug[HISTORY_MONTH_COUNT];

    int initial_bachelor_degree[HISTORY_YEAR_COUNT];
    int initial_master_degree[HISTORY_YEAR_COUNT];
    int initial_doctor_degree[HISTORY_YEAR_COUNT];

    float initial_fake_disp_time_to_degree[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];
    //	float	initial_fake_disp_time_to_degree[HISTORY_YEAR_COUNT];		//NEW

    float initial_time_to_degree[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];
    float initial_dropout_rate[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];
    // ##### End MArco ##### //
    // student summary for sl1-5
    char  talent_academic_all[DEGREE_TYPE_COUNT][HISTORY_MONTH_COUNT];
    char  performance_academic_ug[HISTORY_MONTH_COUNT];

    // student summary for sl1
    char  talent_academic[HISTORY_MONTH_COUNT];
    char  talent_extracurricular[HISTORY_MONTH_COUNT];
    char  talent_athletics[HISTORY_MONTH_COUNT];

    char  satisfaction_academic[HISTORY_MONTH_COUNT];
    char  satisfaction_student_life[HISTORY_MONTH_COUNT];
    char  satisfaction_athletics[HISTORY_MONTH_COUNT];
    // for ultimate score
    char  satisfaction_overall[HISTORY_MONTH_COUNT];

    //----------------//
    float time_to_degree[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];
    float dropout_rate[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];

    // special case handling: fake_disp_time_to_degree() for smoothing graph
    float fake_disp_time_to_degree[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];

    //----------------//
    int bachelor_degree[HISTORY_YEAR_COUNT];
    int non_ug_bachelor_degree[HISTORY_YEAR_COUNT];
    int master_degree[HISTORY_YEAR_COUNT];
    int doctor_degree[HISTORY_YEAR_COUNT];

    //----------------//
    int time_to_degree_cumm[DEGREE_TYPE_COUNT];
    int cur_dropout[DEGREE_TYPE_COUNT];
    float ave_entering_class[DEGREE_TYPE_COUNT];

    int cur_bachelor_degree;
    int cur_non_ug_bachelor_degree;               // store the graduations for nontraditional ug
    int cur_master_degree;
    int cur_doctor_degree;

    int last_year_degree[4];                      // for year end report student turnover
    int last_year_dropout[4];

    int ave_time_to_graduation_for_ug;

    char  ave_satisfaction_student_life[MAX_STUDENT_LEVEL];

public:
    void  update_history(char update_flag);
    void  calc_student_performance();
    void  calc_degree_awarded();

    // ##### Begin MArco ##### //
    void  save_initial_data();
    // ##### End MArco ##### //
    //--------------//
    //##### end fred 980921 #####//
};

//----------------------------------------//
#endif
