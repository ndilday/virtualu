//Filename    : OTASKMSG.CPP
//Description : Object news msg generating routines News::msg() and its helper
//Owner			: Fred

#include <STDIO.H>
#include <OSTR.H>
#include <ODATE.H>
#include <OFONT.H>

#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OPSCHOOL.H>

#include <OTASK.H>

//--------- Define constants ----------//

enum {
    TOTAL_RATING_NAME=9,                            //## chea 220999
};

static char *rating_name[TOTAL_RATING_NAME] = {
    "" , "departmental prestige", "educational quality",
    "student morale" , "faculty morale", "faculty research performance",
    //## chea 220999  make a empty string since I want it to start @ 1
    "faculty diversity index","use of information technology in teaching","staff morale"
};

static char *msg_head[TASK_MSG_SUBTYPE_COUNT] = { "New goal offer ", "Goal expired ", "Goal completed " };

//--------- Define variable type ----------//

typedef void (News::*NewsFP)();                   // message functions are placed in class News for easy access

//------- Define static variables --------//

//static String strt;

static NewsFP task_news_func_array[TASK_COUNT] = {
    &News::news_rise_performance,
    &News::news_educational_quality,
    &News::news_student_morale,
    &News::news_department_morale,
    &News::news_rise_faculty_research_Per,
    &News::news_rise_faculty_diversity_index,
    &News::news_rise_use_of_IT,
    &News::news_rise_staff_morale,
};

//--------- General message format ------------//
//
// Raise the [performance, educational quality, etc. ] rating of the [Geology, Engineering, etc.]
// Department from the current level of [ X ] to [ Y ] before [August, September, etc.].
// Bonus points: [ Z ]
//
//---------------------------------------------//

//------ Begin of function News::task_msg -----//
//!
void News::task_msg() {
    NewsFP newsFP = task_news_func_array[task_id-1];

    (this->*newsFP)();                              // call the corrsponding function to return the news msg
}

//------ Begin of function News::task_msg -----//

//------ Begin of function News::get_task_msg -----//
//!
char* News::get_task_msg() {
    task_msg();

    return str;
}

//------ Begin of function News::get_task_msg -----//

//------ Begin of function News::news_rise_performance -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
void News::news_rise_performance() {
    err_when( player_school.scenario_id > SCN_NONE );

    //---------------- Text Format -----------------//
    // \<Task <msg subtype>\>: Rise the <rating name> rating of the <> department
    // from the current value of <> to <> before 1 <month> <year>
    //
    //----------------------------------------------//

    err_when( task_msg_subtype<0 || task_msg_subtype>=TASK_MSG_SUBTYPE_COUNT );
    err_when(short_para2 >= TOTAL_RATING_NAME);

    str = msg_head[task_msg_subtype];

    str += ": Raise the ";
    str += rating_name[short_para2];
    str += " rating of the ";
    str += department_res[department_array[short_para1]->department_id]->name;
    str += " Department from the current value of ";
    str += m.format(short_para4);
    str += " to ";
    str += m.format(short_para3);

    str += " before ";
    // ### begin Gilbert 07/09/2001 #####//
    //	str += date.month_str(short_para5);
    //	str += " ";
    //	str += m.format(short_para6, 16);
    str += info.date_str_ymd( short_para6, short_para5, 0);
    // ### end Gilbert 07/09/2001 #####//
    str += ". Bonus Points: ";
    str += short_para7;
    str += ".";

}

//------ End of function News::news_rise_performance -----//

//------ Begin of function News::news_educational_quality  -------//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
void News::news_educational_quality() {
    err_when( player_school.scenario_id > SCN_NONE );

    //---------------- Text Format -----------------//
    // \<Task <msg subtype>\>: Rise the <rating name> rating of the <> department
    // from the current value of <> to <> before 1 <month> <year>
    //
    //----------------------------------------------//

    err_when( task_msg_subtype<0 || task_msg_subtype>=TASK_MSG_SUBTYPE_COUNT );
    err_when(short_para2 >= TOTAL_RATING_NAME);

    str = msg_head[task_msg_subtype];

    str += ": Raise the ";
    str += rating_name[short_para2];
    str += " rating of the ";
    str += department_res[department_array[short_para1]->department_id]->name;
    str += " Department from the current value of ";
    str += m.format(short_para4);
    str += " to ";
    str += m.format(short_para3);

    str += " before ";
    // ### begin Gilbert 07/09/2001 #####//
    //	str += date.month_str(short_para5);
    //	str += " ";
    //	str += m.format(short_para6, 16);
    str += info.date_str_ymd( short_para6, short_para5, 0);
    // ### end Gilbert 07/09/2001 #####//

    str += ". Bonus points: ";
    str += short_para7;
    str += ".";
}

//------ End of function News::news_educational_quality  -------//

//------ Begin of function News::news_student_morale -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
void News::news_student_morale() {
    err_when( player_school.scenario_id > SCN_NONE );

    //---------------- Text Format -----------------//
    // \<Task <msg subtype>\>: Rise the <rating name> rating of the <> department
    // from the current value of <> to <> before 1 <month> <year>
    //
    //----------------------------------------------//

    err_when( task_msg_subtype<0 || task_msg_subtype>=TASK_MSG_SUBTYPE_COUNT );
    err_when(short_para2 >= TOTAL_RATING_NAME);

    str = msg_head[task_msg_subtype];

    str += ": Raise the ";
    str += rating_name[short_para2];
    str += " rating from the current value of ";
    str += m.format(short_para4);
    str += " to ";
    str += m.format(short_para3);

    str += " before ";
    // ### begin Gilbert 07/09/2001 #####//
    //	str += date.month_str(short_para5);
    //	str += " ";
    //	str += m.format(short_para6, 16);
    str += info.date_str_ymd( short_para6, short_para5, 0);
    // ### end Gilbert 07/09/2001 #####//

    str += ". Bonus points: ";
    str += short_para7;
    str += ".";
}

//------ End of function News::news_student_morale -----//

//------ Begin of function News::news_department_morale -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
void News::news_department_morale() {
    err_when( player_school.scenario_id > SCN_NONE );

    //---------------- Text Format -----------------//
    // \<Task <msg subtype>\>: Rise the <rating name> rating of the <> department
    // from the current value of <> to <> before 1 <month> <year>
    //
    //----------------------------------------------//

    err_when( task_msg_subtype<0 || task_msg_subtype>=TASK_MSG_SUBTYPE_COUNT );
    err_when(short_para2 >= TOTAL_RATING_NAME);

    str = msg_head[task_msg_subtype];

    str += ": Raise the ";
    str += rating_name[short_para2];
    str += " rating from the current value of ";
    str += m.format(short_para4);
    str += " to ";
    str += m.format(short_para3);

    str += " before ";
    // ### begin Gilbert 07/09/2001 #####//
    //	str += date.month_str(short_para5);
    //	str += " ";
    //	str += m.format(short_para6, 16);
    str += info.date_str_ymd( short_para6, short_para5, 0);
    // ### end Gilbert 07/09/2001 #####//

    str += ". Bonus points: ";
    str += short_para7;
    str += ".";
}

//------ End of function News::news_department_morale -----//

//------ Begin of function News::news_rise_faculty_research_Per -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
void News::news_rise_faculty_research_Per() {
    err_when( player_school.scenario_id > SCN_NONE );

    //---------------- Text Format -----------------//
    // \<Task <msg subtype>\>: Rise the <rating name> rating of the <> department
    // from the current value of <> to <> before 1 <month> <year>
    //
    //----------------------------------------------//

    err_when( task_msg_subtype<0 || task_msg_subtype>=TASK_MSG_SUBTYPE_COUNT );
    err_when(short_para2 >= TOTAL_RATING_NAME);

    str = msg_head[task_msg_subtype];

    str += ": Raise the ";
    str += rating_name[short_para2];
    str += " rating from the current value of ";
    str += m.format(short_para4);
    str += " to ";
    str += m.format(short_para3);

    str += " before ";
    // ### begin Gilbert 07/09/2001 #####//
    //	str += date.month_str(short_para5);
    //	str += " ";
    //	str += m.format(short_para6, 16);
    str += info.date_str_ymd( short_para6, short_para5, 0);
    // ### end Gilbert 07/09/2001 #####//

    str += ". Bonus points: ";
    str += m.format(short_para7,4);
    str += ".";
}

//------ End of function News::news_rise_faculty_research_Per -----//

//------ Begin of function News::news_rise_faculty_diversity_index -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
void News::news_rise_faculty_diversity_index() {
    err_when( player_school.scenario_id > SCN_NONE );

    //---------------- Text Format -----------------//
    // \<Task <msg subtype>\>: Rise the <rating name> rating of the <> department
    // from the current value of <> to <> before 1 <month> <year>
    //
    //----------------------------------------------//

    err_when( task_msg_subtype<0 || task_msg_subtype>=TASK_MSG_SUBTYPE_COUNT );
    err_when(short_para2 >= TOTAL_RATING_NAME);

    str = msg_head[task_msg_subtype];

    str += ": Raise the ";
    str += rating_name[short_para2];

    str += " rating of the ";
    str += department_res[department_array[short_para1]->department_id]->name;

    str += " Department from the current value of ";
    str += m.format(short_para4);
    str += " to ";
    str += m.format(short_para3);

    str += " before ";
    // ### begin Gilbert 07/09/2001 #####//
    //	str += date.month_str(short_para5);
    //	str += " ";
    //	str += m.format(short_para6, 16);
    str += info.date_str_ymd( short_para6, short_para5, 0);
    // ### end Gilbert 07/09/2001 #####//

    str += ". Bonus points: ";
    str += m.format(short_para7,4);
    str += ".";
}

//------ End of function News::news_rise_faculty_diversity_index -----//

//------ Begin of function News::news_rise_use_of_IT -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
void News::news_rise_use_of_IT() {
    err_when( player_school.scenario_id > SCN_NONE );

    //---------------- Text Format -----------------//
    // \<Task <msg subtype>\>: Rise the <rating name> rating of the <> department
    // from the current value of <> to <> before 1 <month> <year>
    //
    //----------------------------------------------//

    err_when( task_msg_subtype<0 || task_msg_subtype>=TASK_MSG_SUBTYPE_COUNT );
    err_when(short_para2 >= TOTAL_RATING_NAME);

    str = msg_head[task_msg_subtype];

    str += ": Raise the ";
    str += rating_name[short_para2];
    str += " rating from the current value of ";
    str += m.format(short_para4);
    str += " to ";
    str += m.format(short_para3);

    str += " before ";
    // ### begin Gilbert 07/09/2001 #####//
    //	str += date.month_str(short_para5);
    //	str += " ";
    //	str += m.format(short_para6, 16);
    str += info.date_str_ymd( short_para6, short_para5, 0);
    // ### end Gilbert 07/09/2001 #####//

    str += ".  Bonus points: ";
    str += m.format(short_para7,4);
    str += ".";
}

//------ End of function News::news_rise_use_of_IT -----//

//------ Begin of function News::news_rise_staff_morale -----//
//!
//! newsPtr->short_para1 = deptRecno;
//!	newsPtr->short_para2 = ratingType;
//!	newsPtr->short_para3 = target;
//!	newsPtr->short_para4 = src;
//!	newsPtr->short_para5 = month;
//!	newsPtr->short_para6 = year;
//!	newsPtr->short_para7 = bonus;
//!
void News::news_rise_staff_morale() {
    err_when( player_school.scenario_id > SCN_NONE );

    //---------------- Text Format -----------------//
    // \<Task <msg subtype>\>: Rise the <rating name> rating of the <> department
    // from the current value of <> to <> before 1 <month> <year>
    //
    //----------------------------------------------//

    err_when( task_msg_subtype<0 || task_msg_subtype>=TASK_MSG_SUBTYPE_COUNT );
    err_when(short_para2 >= TOTAL_RATING_NAME);

    str = msg_head[task_msg_subtype];

    str += ": Raise the ";
    str += rating_name[short_para2];
    str += " rating from the current value of ";
    str += m.format(short_para4);
    str += " to ";
    str += m.format(short_para3);

    str += " before ";
    // ### begin Gilbert 07/09/2001 #####//
    //	str += date.month_str(short_para5);
    //	str += " ";
    //	str += m.format(short_para6, 16);
    str += info.date_str_ymd( short_para6, short_para5, 0);
    // ### end Gilbert 07/09/2001 #####//

    str += ". Bonus points: ";
    str += m.format(short_para7,4);
    str += ".";
}

//------ End of function News::news_rise_staff_morale -----//
