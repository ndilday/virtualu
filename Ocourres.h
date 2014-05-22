//Filename    : OCOURRES.H
//Description : Course Template Class Declaration
//Owner			: Fred

#ifndef __OCOURRES_H
#define __OCOURRES_H

#include <OSTUDENT.H>

#define FIRST_COURSE_TEMPLATE_CODE  'A'

enum {
    COURSE_PREFERENCE_COUNT = 3,
    P_SEMINAR=0, P_CLASS_WITH_BREAKOUTS, P_OTHER,   // P_OTHER also called P_GENERAL

    SUMMER_PERF_COUNT = 3,
    SUMMER_COURSE_COUNT = 5,                        // 0,1,2,3 or 4 summer courses
};

//------ Define struct Rec for summer course -------//

//!Stucture for storing preferences for summer courses, used in CourseRes
//!class.
struct SummerRec {
    char course_pref[SUMMER_COURSE_COUNT][9];
};

//------ Define struct CourseTemplateRec -------//

//!Course record.
struct CourseTemplateRec {
    char  template_code[9];
    char  target_preference[9];
    char  normal_class_size[9];
};

//------ Define struct CourseTemplate -------//

//!
struct CourseTemplate {
    char  template_code;

    // 99->99%
    char  target_preference[COURSE_PREFERENCE_COUNT];
    short normal_class_size[COURSE_PREFERENCE_COUNT];
};

//------- Define class CourseRes ---------//

//!Class for representing course.
class CourseRes {
public:
    char          init_flag;

    // 99->99%
    char          summer_course_pref[MAX_STUDENT_LEVEL][SUMMER_PERF_COUNT][SUMMER_COURSE_COUNT];

    short           course_template_count;
    CourseTemplate* course_template_array;

public:
    void        init();
    void        deinit();
    CourseTemplate* get_template(char* templateCode);
    CourseTemplate* get_template(int deptId);

    int     get_normal_class_size(char* templateCode, char teachingMethod);
    void      get_course_pref(char* templateCode, char* pref);
    void      change_course_templ_code(char* code);

private:
    void        load_template();
    void      free_template();

    void        load_summer_db();
};

//------- Define class CourseSummary ---------//

extern CourseRes course_res;

//-----------------------------------------//
#endif
