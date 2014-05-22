//Filename    : OTASK.CPP
//Description : Object TaskArray; including sys, font related functions.
//Owner			:Fred

#include <OINFO.H>

#include <OTASK.H>

//------- define screen coordinations -------//
//--------- define static vars --------------//

//------ Begin of function TaskArray::TaskArray -----//

TaskArray::TaskArray() : DynArray(sizeof(Task), 100) {
}

//-------- End of function TaskArray::TaskArray -----//

//------ Begin of function TaskArray::init -----//
//!
void TaskArray::init() {
    reset();

    //	set_font(&font_san);		// use black font
}

//------- End of function TaskArray::init -----//

//------ Begin of function TaskArray::deinit -----//
//!
void TaskArray::deinit() {
    reset();
}

//------- End of function TaskArray::deinit -----//

//------ Begin of function TaskArray::reset -----//
//!
//! Reset all news display options and clear all news in the log
//!
void TaskArray::reset() {
    zap();                                          // clear all news in the log

    task_add_flag    = 1;
    total_bonus_pt   = 0;

    default_setting();
}

//------- End of function TaskArray::reset -----//

//------ Begin of function TaskArray::default_setting -----//
//!
//! Reset all news display options
//!
void TaskArray::default_setting() {
    //news_type_option[TASK_NORMAL]=1;

    task_who_option = TASK_DISP_ALL;                // default display news of groups controlled by the player
}

//------- End of function TaskArray::default_setting -----//

//------ Begin of function TaskArray::remove -----//
//!
//! Remove a specific news from the task_array.
//!
//! <int> newsId     = id. of the news
//!
void TaskArray::remove(int recNo) {
    Task* ptr;
    ptr = task_array[recNo];

    linkout(recNo);
}

//------- End of function TaskArray::remove -----//

//------- Begin of function TaskArray::operator[] -----//

Task* TaskArray::operator[](int recNo) {
#ifdef DEBUG

    Task* msgPtr;

    if( recNo )
	msgPtr = (Task*) get(recNo);
    else
	msgPtr = NULL;                                // NULL will then cause error

    err_if( !msgPtr || msgPtr->news_date==0 )
	err_now( "TaskArray[] is deleted" );

    return msgPtr;

#else

    return (Task*) get(recNo);
#endif
}

//--------- End of function TaskArray::operator[] ----//
