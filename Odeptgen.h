//Filename    : ODEPT.H
//Description : Department Class Declaration
//            : handle general department for "undecided" student,
//              and university general facilities for facilities model
//Owner       : Fred

#ifndef __ODEPTGEN_H
#define __ODEPTGEN_H

#include <OSTUDENT.H>

//----------- Define constants -------------//
enum {
    MAX_PROJECT_COUNT=15,                           // = years needed to complete a project
};

//----------- Define class Project --------------//
//
//!Structure containing project information such as expense and duration.
//!An array of current projects related to Facilities office is stored in
//!the GeneralDepartment class.
struct Project {
    int new_sf;
    char  month_left;
    int expense;
};

//----------- Define class Department ---------------//

//!Base Class of Department. Needed for handling general department for
//!"undecided" student, and university general facilities for facilities
//!model.
class GeneralDepartment {
public:
    GeneralDepartment();

    void  init();
    int write_file(File* filePtr);
    int read_file(File* filePtr);

    //----------------------------------------//
    //----- for student database ONLY -----//

    StudentArray  student_array;                  // fred 0618: use playerSchool.student_array instead of this

    char        first_write_offset;               // for file read write;

    short       department_id;                    // department id. in department_res[]

    //## chea 270999 to update the student no. report.
    int   dept_student_level_no[MAX_STUDENT_LEVEL_N_TOTAL];

    //## chea 290999 to update the student no. report.
    int   dept_gender_student_level_no[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];

    // ##### Begin MArco ##### //
    int   initial_dept_student_level_no[MAX_STUDENT_LEVEL_N_TOTAL];
    // ##### End MArco ##### //

public:
    void  next_day();

    int get_course_requirement(int studentLevel, int targetDeptRecno, int courseDepth);

    // ##### Begin MArco ##### //
    void  save_initial_data();
    // ##### End Marco ##### //

public:
    //----- for statistics -----//	//0406

    int   student_level_history[MAX_STUDENT_LEVEL_N_TOTAL][HISTORY_YEAR_COUNT];
    int   student_gender_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL][HISTORY_YEAR_COUNT];

    //----- for facility_office ONLY -----//

    Project project_array[MAX_PROJECT_COUNT];
    char    project_count;
    bool    project_added_this_year;

public:

    void  calc_student_history();                 //0406

    //----------------------------------------//

    void  next_month_construction(int &newSF, int &expense);
    void  add_construction(int newSF, int expense);
    void  confirm_construction_yearly();
    int get_constructing_sf();
};
#endif
