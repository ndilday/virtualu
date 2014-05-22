//Filename    : OTASK.H
//Description : Object News
//Owner		  : Fred

//------------------------------------//
//
// Task array/list properties (for each task):
//
// - shown on both message (normal news) and objective sections on screen
// - will be removed from the objective section after the task offer expires or the task is completed
// - checked monthly the success of the task; add bonus if task is completed
//
//------------------------------------//

#ifndef __OTASK_H
#define __OTASK_H

#include <ONEWS.H>

//--------- Define constant --------------//

//#define DISP_TASKS_COUNT 5			// maximum no. of task message displayed on the screen at a time

//#define TASKS_STR_PARA_LEN 20

//-------- Define task type ---------//

enum { TASK_WHO_NUM=1 };
enum {                                            //, TASKS_DISP_FRIENDLY, TASKS_DISP_PLAYER, TASKS_DISP_NONE };
    TASK_DISP_ALL=0
};

//------- Define other constant -------//

//--------- Define task id. ----------//

#define TASK_COUNT 8                              //## chea 220999 1 is org. & is the first ele
enum {
    TASK_RISE_PERFORMANCE_RATING = 1,               // rise value of performance indicator
    RISE_EDUCATIONAL_QUALITY,                       //## chea 220999
    RISE_STUDENT_MORALE,
    RISE_DEPARTMENT_MORALE,
    RISE_FACULTY_RESEARCH,
    RISE_FACULTY_DIVERSITY_INDEX,
    RISE_USE_OF_IT,
    RISE_STAFF_MORALE,
};

//------------------------------------//
// typedef News Task;

//!Subclass of News class. Tasks are shown on both message (normal news)
//!and objective sections on screen. They are removed from the objective
//!section after the task offer expires or the task is completed. The
//!successful completion of the task is checked monthly and a bonus is
//!added for completed tasks.
struct Task : News {
    //bool (*complete_func)();
    //bool (*expiry_func)();

    // temp functions for each task type
    bool  is_completed_rise_performance();
    bool  is_completed_rise_educational_quality();  //## chea 220999
    bool  is_completed_rise_student_morale();
    bool  is_completed_rise_department_morale();
    bool  is_completed_rise_faculty_research_Per();
    bool  is_completed_rise_faculty_diversity_index();
    bool  is_completed_rise_use_of_IT();
    bool  is_completed_rise_staff_morale();

    // functions for all task types
    bool  is_expired();
    void  set_completed();
};

//-------- Define class TaskArray ----------//

//!Array containing the history of the tasks assigned during this game.
//!Responds to init, next_day, and read/write.
class TaskArray : public DynArray {
public:
    //------ display options ------//

    char  task_who_option;
    char  task_add_flag;
    int total_bonus_pt;

    float rand_init;

public:
    TaskArray();

    void  init();                                 // called in ogame.cpp
    void  deinit();                               // called in ogame.cpp

    int write_file(File* filePtr);
    int read_file(File* filePtr);

    void  enable()        { task_add_flag = 1; }
    void  disable()   { task_add_flag = 0; }

    void  reset();
    void  default_setting();

    void  clear_task_disp();

    void  next_day();
    Task* add_task(int taskId);
    void  remove(int index);
    bool  can_add_more_task();

    //------ functions for adding task -------//

    void  rise_performance_rating(int deptRecno, int ratingType,
				  int target, int src, int month, int year, int bonus);

    void  rise_educational_quality(int deptRecno, int ratingType,
				   int target, int src, int month, int year, int bonus);

    void  rise_student_morale(int deptRecno, int ratingType,
			      int target, int src, int month, int year, int bonus);

    void  rise_department_morale(int deptRecno, int ratingType,
				 int target, int src, int month, int year, int bonus);

    void  rise_faculty_research_Per(int deptRecno, int ratingType,
				    int target, int src, int month, int year, int bonus);

    void  rise_faculty_diversity_index(int deptRecno, int ratingType,
				       int target, int src, int month, int year, int bonus);

    void  rise_use_of_IT(int deptRecno, int ratingType,
			 int target, int src, int month, int year, int bonus);

    void  rise_staff_morale(int deptRecno, int ratingType,
			    int target, int src, int month, int year, int bonus);

    //--------------------------------------------//

    Task* operator[](int recNo);
};

extern TaskArray task_array;

//-------------------------------------------//
#endif
