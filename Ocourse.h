//Filename    : OCOURSE.h
//Description : Course Class Declaration

#ifndef __OCOURSE_H
#define __OCOURSE_H

#ifndef __ODYNARRB_H
#include <ODYNARRB.H>
#endif

//-------- Define constant -----------//

#define STD_COURSE_PER_TRIMESTER    4             // total number of courses in a trimester
#define STD_COURSE_NUG_PER_TRIMESTER    2
#define STD_COURSE_PER_YEAR      (STD_COURSE_PER_TRIMESTER * 2)

#define TOTAL_REQUIRED_COURSE   4
#define TOTAL_ELECTIVE_COURSE   2

#define CONTACT_HOUR_PER_COURSE  3                // needed contact hours of a faculty for teaching a section of the selected course

#define MAX_YEAR_REQUIRE_ELECTIVE_COURSE  4       // maximum number of years in program that require taking elective courses, for defining the size of Department::min_elective_course_by_year[MAX_YEAR_REQUIRE_ELECTIVE_COURSE]

//----- Define number of courses required for different types of degree -----//

#define COURSE_COUNT_REQUIRED_FOR_BACHELOR  32
#define COURSE_COUNT_REQUIRED_FOR_MASTER    8
#define COURSE_COUNT_REQUIRED_FOR_DOCTOR    16

//-------- Define constant -----------//

#define MAX_COURSE_DEPTH          4
#define MAX_COURSE_DEPTH_WITHOUT_GR   3

enum {                                            // depth category
    DEPTH_D1, DEPTH_D2, DEPTH_D3, DEPTH_GR
};

//------------------------------------//

#define MAX_COURSE_FOCUS          4
#define MAX_COURSE_FOCUS_WITHOUT_GR   3

enum {                                            // focus category
    FOCUS_LD, FOCUS_M, FOCUS_UD, FOCUS_GR
};

//------------------------------------//

#define MAX_COURSE_DEPTH_FOCUS  7

enum {                                            // depth-focus category
    D1_LD, D1_M, D1_UD, D2_M, D2_UD, D3_UD, GR
};

//------------------------------------//

enum {
    MAX_TEACHING_METHOD = 5,
    MAX_TEACHING_METHOD_N_TOTAL,
};

//enum { SEMINAR, CLASS_WITHOUT_BREAKOUT, CLASS_WITH_BREAKOUT,
//		 BREAKOUT_LAB, GENERAL };							// teaching method

// see req29a_course.txt

// teaching method; BREAKOUT_LAB also called breakout section
enum {
    SEMINAR, DISTANCE_LEARN_COURSE, CLASS_WITH_BREAKOUT, BREAKOUT_LAB, GENERAL,
};

//------- Define class Course --------//

//!Class for representing a course.
class Course {
public:
    short course_recno;
    short department_recno;

    char  depth_category;
    char  focus_category;

    char  teaching_method;
    char  is_closed;                              // 1 if this course is closed. This happens when there is no faculty member available for creating new course section.

    int distance_learning_place;
    int actual_distance_learning_enrollment;

    //-------- counter variables ---------//

    short convention_student_count;               // the total number of students in all sections of this course
    //short	convention_student_distance_learner_count;
    short distance_student_count;                 // not used yet

    int normal_class_size();                      // should be read "normal section size for any course with the corresponding teaching method (column A) for each template in course_template"

    short admission_denied_count;
    short failure_count;

    short outsourcing_credits;

    short class_section_count;                    // the total number of sections in this course
    short cur_section_student_count;              // number of students in the current active section

public:
    int write_file(File* filePtr);
    int read_file(File* filePtr);

    void  next_trimester();
    void  reset_vars();
};

//----------- Define class CourseArray -----//

//!Matrix subclass for LU decomposition
class CourseArray : public DynArrayB {
public:
    int department_recno;
    int course_enrollments[MAX_TEACHING_METHOD];  // for facility office

    //---------- data for interface -------------//
    int class_section_count[MAX_TEACHING_METHOD_N_TOTAL];
    int convention_student_count[MAX_TEACHING_METHOD_N_TOTAL];
    int average_class_size[MAX_TEACHING_METHOD_N_TOTAL];
    int normal_class_size[MAX_TEACHING_METHOD_N_TOTAL];

    int size_deviation[MAX_TEACHING_METHOD_N_TOTAL];
    int admission_denied_count[MAX_TEACHING_METHOD_N_TOTAL];
    int failure_count[MAX_TEACHING_METHOD_N_TOTAL];

    int outsourcing_credits[MAX_TEACHING_METHOD_N_TOTAL];

    int experienced_class_size[HISTORY_TRIMESTER_COUNT];

    int class_section_count_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    int convention_student_count_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    int size_deviation_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    int admission_denied_count_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    int failure_count_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];

    int outsourcing_credits_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];

    int initial_class_section_count[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_convention_student_count[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_average_class_size[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_size_deviation[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_admission_denied_count[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_failure_count[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_normal_class_size[MAX_TEACHING_METHOD_N_TOTAL];

    int initial_experienced_class_size;

public:
    CourseArray();
    ~CourseArray();

    int     write_file(File* filePtr);
    int     read_file(File* filePtr);

    void      add(Course *newCourse);
    void      del(int recNo);

    void      next_day(int initial=0);
    void     next_trimester();
    void      update_history();

    Course*   get_course(int courseDepth, int courseFocus, int teachingMethod);

    int       get_depth(int depthFocusId);
    int       get_focus(int depthFocusId);

    int     is_deleted(int recNo);

    // ###### Begin Marco ##### //
    void      save_initial_data();
    // ###### End Marco ##### //

#ifdef DEBUG
    Course*   operator[](int recNo);
#else
    Course*   operator[](int recNo) { return (Course*) get(recNo); }
#endif
};

//----------------------------------------//
#endif
