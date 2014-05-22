//Filename    : OTASKEND.CPP
//Description : Object TaskArray; related functions.
//Owner			:Fred

#include <OSYS.H>

#include <OINFO.H>
#include <ODEPT.H>
#include <OPSCHOOL.H>
#include <OTASK.H>
#include <ODATE.H>
#include <OSTR.H>
#include <OCHANCE.H>

//--------- Define variable type ----------//

typedef bool (Task::*TaskFP)();                   // message functions are placed in class News for easy access

//-------- Define struct TaskInfo -------//

//!Structure containing function pointers to completed and expired
//!functions defined in Task.
struct TaskInfo {
    TaskFP is_completed_func;                       //[TASK_MSG_SUBTYPE_COUNT];
    TaskFP is_expired_func;
};

//------- Define static variables --------//

static String str;

static TaskInfo task_info_array[TASK_COUNT] = {
    { &Task::is_completed_rise_performance, &Task::is_expired }, {
	//## chea 220999 is_expired is for all task type
	&Task::is_completed_rise_educational_quality, &Task::is_expired
    },
    { &Task::is_completed_rise_student_morale, &Task::is_expired },
    { &Task::is_completed_rise_department_morale, &Task::is_expired },
    { &Task::is_completed_rise_faculty_research_Per, &Task::is_expired },
    { &Task::is_completed_rise_faculty_diversity_index, &Task::is_expired },
    { &Task::is_completed_rise_use_of_IT, &Task::is_expired },
    { &Task::is_completed_rise_staff_morale, &Task::is_expired },

};

//------- Begin of function TaskArray::next_day -----//
void TaskArray::next_day() {

    // - will be removed from the objective/task section after the task offer expires or the task is completed
    // - checked monthly the success of the task; add bonus if task is completed

    Task* taskPtr;

    // return if not start of a month
    if ( info.game_day != 1) {                      //## chea 210899 BUGHERE the news date will not match
	return;
    }

    //--------------------//

    for( int i=size() ; i>0 ; i-- ) {
	taskPtr = operator[](i);

	// already completed
	if ( taskPtr->task_msg_subtype == TASK_COMPLETED )
	    continue;

	if ( (taskPtr->*(task_info_array[taskPtr->id-1].is_completed_func))() == true ) {
	    // add msg to news array
	    news_array.new_task(taskPtr, TASK_COMPLETED);

	    // 1126 don't remove if complete // remove(i);
	    taskPtr->set_completed();
	}
	//else if ( (*taskPtr->expiry_func)() == true )
	else if ( (taskPtr->*(task_info_array[taskPtr->id-1].is_expired_func))() == true ) {
	    news_array.new_task(taskPtr, TASK_EXPIRED); // add msg to news array

	    //------- set for newspaper display --------//

	    News* newsPtr = news_array[news_array.size()];
	    chance_event.news_str=newsPtr->msg();
	    chance_event.news_news_str=chance_event.news_str;
	    //			memcpy(chance_event.news_date_str,date.date_str(newsPtr->news_date),55);
	    strncpy(chance_event.news_date_str, info.date_str_julian(newsPtr->news_date), 55 );
	    chance_event.news_date_str[55-1] = '\0';
	    //		   chance_event.date_str=date.date_str( newsPtr->news_date, 1, 0 );
	    chance_event.date_str=info.date_str_julian(newsPtr->news_date, INFO_DATE_DEFAULT|INFO_DATE_SHORT_MONTH|INFO_DATE_NO_DAY);

	    remove(i);
	}

	sys.yield();
    }
}

//------- End of function TaskArray::next_day -----//

//------ Begin of function Task::is_expired -----//
//!
//! temp assumption for all kinds of task:
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!
bool Task::is_expired() {
    /*
      static  String expire_str; // for msg

      //## chea 220999 added to tell player time has expire.
      if(this->id == TASK_RISE_PERFORMANCE_RATING)
      expire_str = "departmental prestige expired";
      else if(this->id == RISE_EDUCATIONAL_QUALITY)
      expire_str = "educational quality expired";
      else if(this->id == RISE_STUDENT_MORALE)
      expire_str = "RISE STUDENT MORALE expired";
      else if(this->id == RISE_DEPARTMENT_MORALE)
      expire_str = "RISE DEPARTMENT MORALE expired";
      else if(this->id == RISE_FACULTY_RESEARCH)
      expire_str = "RISE FACULTY RESEARCH expired";
      else if(this->id == RISE_FACULTY_DIVERSITY_INDEX)
      expire_str = "RISE_FACULTY_DIVERSITY_INDEX expired";
      else if(this->id == RISE_USE_OF_IT)
      expire_str = "RISE USE OF IT expired";
      else if(this->id == RISE_STAFF_MORALE)
      expire_str = "RISE STAFF MORALE expired";
    */
    // #### begin Gilbert 07/09/2001 ######//
    // if ( info.game_month >= short_para5 && info.game_year >= short_para6 )
    if ( info.game_year > short_para6
	 || info.game_year == short_para6 && info.game_month >= short_para5 ) {
	// #### end Gilbert 07/09/2001 ######//
	sys.set_news_pop_up();
	//		chance_event.news_news_str=expire_str;

	// display date in 200? format
	long endDay = date.julian(info.game_year, info.game_month, info.game_day);
	//		memcpy(chance_event.news_date_str,date.date_str(endDay),55);
	strncpy(chance_event.news_date_str, info.date_str_julian(endDay), 55 );
	chance_event.news_date_str[55-1] = '\0';
	//		chance_event.date_str=date.date_str(endDay,1,0);
	chance_event.date_str=info.date_str_julian(endDay, INFO_DATE_DEFAULT|INFO_DATE_SHORT_MONTH|INFO_DATE_NO_DAY);

	return true;
    }
    else
	return false;
}

//------ End of function Task::is_expired -----//

//------ Begin of function Task::set_completed -----//
//!
//! temp assumption for all kinds of task:
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!
void Task::set_completed() {                      // 990503
    task_msg_subtype = TASK_COMPLETED;
    news_date = info.game_date;

    task_array.total_bonus_pt += short_para7;
}

//------ End of function Task::set_completed -----//

//------ Begin of function Task::is_completed_rise_performance -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
bool Task::is_completed_rise_performance() {

    //BUGHERE0 should depends on short_para2
    float curRating = department_array[short_para1]->p_academic_standing;

    return curRating >= short_para3;
}

//------ End of function Task::is_completed_rise_performance -----//

//------ Begin of function Task::is_completed_rise_educational_quality -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
bool Task::is_completed_rise_educational_quality() {
    float curRating = department_array[short_para1]->p_educational_quality;

    return curRating >= short_para3;
}

//------ End of function Task::is_completed_rise_educational_quality -----//

//------ Begin of function Task::is_completed_rise_student_morale -----//
bool Task::is_completed_rise_student_morale() {
    float curRating = player_school.sub_score[S_STUDENT_MORALE][HISTORY_MONTH_COUNT-1];

    return curRating >= short_para3;
}

//------ End of function Task::is_completed_rise_student_morale -----//

//------ Begin of function Task::is_completed_rise_department_morale -----//
//!this should be rise_factulty_morale
bool Task::is_completed_rise_department_morale() {
    float curRating = department_array.p_faculty_morale_history[47];

    return curRating >= short_para3;
}

//------ End of function Task::is_completed_rise_department_morale -----//

//------ Begin of function Task::is_completed_rise_faculty_research_Per -----//
bool Task::is_completed_rise_faculty_research_Per() {
    float curRating = department_array.p_faculty_research_history[47];

    return curRating >= short_para3;
}

//------ End of function Task::is_completed_rise_faculty_research_Per -----//

//------ Begin of function Task::is_completed_rise_faculty_diversity_index -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
bool Task::is_completed_rise_faculty_diversity_index() {
    //value now
    float curRating = department_array[short_para1]->p_faculty_diversity_history[7];

    return curRating >= short_para3;
}

//------ End of function Task::is_completed_rise_faculty_diversity_index -----//

//------ Begin of function Task::is_completed_rise_use_of_IT -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
bool Task::is_completed_rise_use_of_IT() {

    Department* cur_dept_ptr = department_array[short_para1];
    float curRating = cur_dept_ptr->var_educational_quality_history[4][23];

    return curRating >= short_para3;
}

//------ End of function Task::is_completed_rise_use_of_IT -----//

//------ Begin of function Task::is_completed_rise_staff_morale -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
bool Task::is_completed_rise_staff_morale() {
    float curRating = player_school.sub_score[S_STAFF_MORALE][THIS_MONTH];

    return curRating >= short_para3;
}

//------ End of function Task::is_completed_rise_staff_morale -----//
