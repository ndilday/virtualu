//Filename    : OTASKADD.CPP
//Description : News adding functions for TaskArray::add_task()
//Owner			: Fred

#include <OPSCHOOL.H>
#include <OCONFIG.H>
#include <OAUDIO.H>
#include <OINFO.H>
#include <OTASK.H>

//------ Begin of function TaskArray::rise_performance_rating -----//
//!
//! short_para1 = the recno of department info
//!
void TaskArray::rise_performance_rating(int deptRecno, int ratingType,
					int target, int src, int month, int year, int bonus) {
    //----------- add news --------------//

    Task* taskPtr = add_task( TASK_RISE_PERFORMANCE_RATING );

    if( !taskPtr )
	return;

    taskPtr->short_para1 = deptRecno; err_when(deptRecno == 0);
    taskPtr->short_para2 = ratingType;
    taskPtr->short_para3 = target;
    taskPtr->short_para4 = src;
    taskPtr->short_para5 = month;
    taskPtr->short_para6 = year;
    taskPtr->short_para7 = bonus;

    //----------------------------------------------//
    // add task to news_array

    news_array.new_task(taskPtr);
}

//------- End of function TaskArray::rise_performance_rating -----//

//## chea 220999
//------ Begin of function TaskArray::rise_educational_quality -----//
//!
//! short_para1 = the recno of department info
//!
void TaskArray::rise_educational_quality(int deptRecno, int ratingType,
					 int target, int src, int month, int year, int bonus) {
    //----------- add news --------------//

                                                  //## chea 220999
    Task* taskPtr = add_task( RISE_EDUCATIONAL_QUALITY );

    if( !taskPtr )
	return;

    taskPtr->short_para1 = deptRecno; err_when(deptRecno == 0);
    taskPtr->short_para2 = ratingType;
    taskPtr->short_para3 = target;
    taskPtr->short_para4 = src;
    taskPtr->short_para5 = month;
    taskPtr->short_para6 = year;
    taskPtr->short_para7 = bonus;

    //----------------------------------------------//
    // add task to news_array

    news_array.new_task(taskPtr);
}

//------- End of function TaskArray::rise_educational_quality -----//

//------ Begin of function TaskArray::rise_student_morale -----//
//!
//! short_para1 = the recno of department info
//!
void TaskArray::rise_student_morale(int deptRecno, int ratingType,
				    int target, int src, int month, int year, int bonus) {
    //----------- add news --------------//

    Task* taskPtr = add_task( RISE_STUDENT_MORALE );

    if( !taskPtr )
	return;

    taskPtr->short_para1 = deptRecno; err_when(deptRecno == 0);
    taskPtr->short_para2 = ratingType;
    taskPtr->short_para3 = target;
    taskPtr->short_para4 = src;
    taskPtr->short_para5 = month;
    taskPtr->short_para6 = year;
    taskPtr->short_para7 = bonus;

    //----------------------------------------------//
    // add task to news_array

    news_array.new_task(taskPtr);
}

//------- End of function TaskArray::rise_student_morale -----//

//------ Begin of function TaskArray::rise_department_morale -----//
//!
//! short_para1 = the recno of department info
//!
void TaskArray::rise_department_morale(int deptRecno, int ratingType,
				       int target, int src, int month, int year, int bonus) {
    //----------- add news --------------//

    Task* taskPtr = add_task( RISE_DEPARTMENT_MORALE );

    if( !taskPtr )
	return;

    taskPtr->short_para1 = deptRecno; err_when(deptRecno == 0);
    taskPtr->short_para2 = ratingType;
    taskPtr->short_para3 = target;
    taskPtr->short_para4 = src;
    taskPtr->short_para5 = month;
    taskPtr->short_para6 = year;
    taskPtr->short_para7 = bonus;

    //----------------------------------------------//
    // add task to news_array

    news_array.new_task(taskPtr);
}

//------- End of function TaskArray::rise_department_morale -----//

//------ Begin of function TaskArray::rise_faculty_research_Per -----//
//!
//! short_para1 = the recno of department info
//!
void TaskArray::rise_faculty_research_Per(int deptRecno, int ratingType,
					  int target, int src, int month, int year, int bonus) {
    //----------- add news --------------//

    Task* taskPtr = add_task( RISE_FACULTY_RESEARCH );

    if( !taskPtr )
	return;

    taskPtr->short_para1 = deptRecno; err_when(deptRecno == 0);
    taskPtr->short_para2 = ratingType;
    taskPtr->short_para3 = target;
    taskPtr->short_para4 = src;
    taskPtr->short_para5 = month;
    taskPtr->short_para6 = year;
    taskPtr->short_para7 = bonus;

    //----------------------------------------------//
    // add task to news_array

    news_array.new_task(taskPtr);
}

//------- End of function TaskArray::rise_faculty_research_Per -----//

//------ Begin of function TaskArray::rise_faculty_diversity_index -----//
//!
//! short_para1 = the recno of department info
//!
void TaskArray::rise_faculty_diversity_index(int deptRecno, int ratingType,
					     int target, int src, int month, int year, int bonus) {
    //----------- add news --------------//

    Task* taskPtr = add_task( RISE_FACULTY_DIVERSITY_INDEX );

    if( !taskPtr )
	return;

    taskPtr->short_para1 = deptRecno; err_when(deptRecno == 0);
    taskPtr->short_para2 = ratingType;
    taskPtr->short_para3 = target;
    taskPtr->short_para4 = src;
    taskPtr->short_para5 = month;
    taskPtr->short_para6 = year;
    taskPtr->short_para7 = bonus;

    //----------------------------------------------//
    // add task to news_array

    news_array.new_task(taskPtr);
}

//------- End of function TaskArray::rise_faculty_diversity_index -----//

//------ Begin of function TaskArray::rise_use_of_IT -----//
//!
//! short_para1 = the recno of department info
//!
void TaskArray::rise_use_of_IT(int deptRecno, int ratingType,
			       int target, int src, int month, int year, int bonus) {
    //----------- add news --------------//

    Task* taskPtr = add_task( RISE_USE_OF_IT );

    if( !taskPtr )
	return;

    taskPtr->short_para1 = deptRecno; err_when(deptRecno == 0);
    taskPtr->short_para2 = ratingType;
    taskPtr->short_para3 = target;
    taskPtr->short_para4 = src;
    taskPtr->short_para5 = month;
    taskPtr->short_para6 = year;
    taskPtr->short_para7 = bonus;

    //----------------------------------------------//
    // add task to news_array

    news_array.new_task(taskPtr);
}

//------- End of function TaskArray::rise_use_of_IT -----//

//------ Begin of function TaskArray::rise_staff_morale -----//
//!
//! short_para1 = the recno of department info
//!
void TaskArray::rise_staff_morale(int deptRecno, int ratingType,
				  int target, int src, int month, int year, int bonus) {
    //----------- add news --------------//

    Task* taskPtr = add_task( RISE_STAFF_MORALE );

    if( !taskPtr )
	return;

    taskPtr->short_para1 = deptRecno; err_when(deptRecno == 0);
    taskPtr->short_para2 = ratingType;
    taskPtr->short_para3 = target;
    taskPtr->short_para4 = src;
    taskPtr->short_para5 = month;
    taskPtr->short_para6 = year;
    taskPtr->short_para7 = bonus;

    //----------------------------------------------//
    // add task to news_array

    news_array.new_task(taskPtr);
}

//------- End of function TaskArray::rise_staff_morale -----//

//------ Begin of function TaskArray::add_task -----//
//!
//! Called by news processing function to set news parameters
//!
//! <int> taskId       = the id. of the task
//! <int> newsType     = news type
//!
//! return : <Task*> return the pointer of the News
//!						  NULL - the nation of the news does not have contact with the player
//!
Task* TaskArray::add_task(int taskId) {
    //	if ( size() >=1 )		// 0208 // 990503
    //		return 0;

    //playsound
    audio.play_wav("NEWS",audio.int_to_DsVolume(config.sound_effect_volume));

    //----------------------------------------------//
    // add task to this: task_array

    static Task newTask;

    newTask.id   = taskId;
    newTask.task_id = taskId;
    newTask.task_msg_subtype = TASK_STARTED;
    newTask.news_date = info.game_date;

    //--- if the news adding flag is turned off, don't add the news ---//

    if( task_add_flag ) {
	//--- if no. of news reaches max., delete the oldest one ---//

	//--------- link in a new news ---------//

	linkin(&newTask);

	Task* taskPtr = (Task*) get();

	return taskPtr;
    }
    else {
	return &newTask;
    }
}

//------- End of function TaskArray::add_task -----//

//------- Begin of function TaskArray::can_add_more_task -----//
//!
bool TaskArray::can_add_more_task() {
    // it can be 1 or 2, and max can be 2 or 3
    int maxActiveTask = player_school.chance_objective_frequency+1;
    int taskCount=0;

    for( int i=size() ; i>0 ; i-- ) {
	Task* taskPtr = operator[](i);

	if ( taskPtr->task_msg_subtype != TASK_COMPLETED ) {
	    taskCount++;

	    if( taskCount == maxActiveTask )            // can't add more if it has reached the maximum
		return false;
	}
    }
    return true;
}

//------- End of function TaskArray::can_add_more_task -----//
