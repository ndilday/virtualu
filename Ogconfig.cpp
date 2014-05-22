//File: OGCONFIG.CPP
//Description: GoalConfig

#include <ALL.H>
#include <OFILETXT.H>
#include <OGCONFIG.H>

//----- Begin of function GoalConfig::init --------//

void GoalConfig::init() {
    memset( this, 0, sizeof(GoalConfig) );

    read_goal_scen();
    read_goal_task();
}

//------- End of function GoalConfig::init --------//

//----- Begin of function GoalConfig::deinit --------//

void GoalConfig::deinit() {
}

//------- End of function GoalConfig::deinit --------//

//----- Begin of function GoalConfig::read_goal_scen --------//
//!
//! GOALSCEN.RES format:
//!
//! 1: Balance The Budget
//! ---------------------
//! Gold Years: 9
//! Silver Years: 6
//! Bronze Years: 3
//! Gold value: 0, 30, 50
//! Silver value: 9000, 50, 100
//! Bronze value: 1000, 20, 300
//!
void GoalConfig::read_goal_scen() {
    FileTxt fileTxt( DIR_RES"GOALSCEN.RES" );

    while( !fileTxt.is_eof() ) {
	int scenId = (int) fileTxt.get_num();

	if( scenId < 1 || scenId > SCENARIO_COUNT )
	    continue;

	//-------------------------------------//

	fileTxt.next_line();
	fileTxt.next_line();

	GoalScen* goalScen = goal_scen_array+scenId-1;

	//--- read scenario objective time limits ------//

	for( int i=0 ; i<3 ; i++ ) {
	    fileTxt.get_token();                        // bypass "Gold"
	    fileTxt.get_token();                        // bypass "Years:"
	    goalScen->goal_year[i] = (short) fileTxt.get_num();
	    fileTxt.next_line();
	}

	//--- read scenario objective variables ------//

	for( int i=0 ; i<3 ; i++ ) {
	    fileTxt.get_token();                        // bypass "Gold"
	    fileTxt.get_token();                        // bypass "Value:"
	    goalScen->goal_value[i] = (float) fileTxt.get_num();
	    fileTxt.next_line();
	}

	//------- if all scenario settings have been read -----//

	if( scenId == SCENARIO_COUNT )
	    break;
    }
}

//------- End of function GoalConfig::read_goal_scen --------//

//----- Begin of function GoalConfig::read_goal_task --------//
//!
//! GOALTASK.RES format:
//!
//! 1: TASK_RISE_PERFORMANCE_RATING
//! -------------------------------
//! Years Lower: 4
//! Years Upper: 6
//! Goal Lower: 30
//! Goal Upper: 40
//! Bonus	  : 100
//!
void GoalConfig::read_goal_task() {
    FileTxt fileTxt( DIR_RES"GOALTASK.RES" );

    while( !fileTxt.is_eof() ) {
	int taskId = (int) fileTxt.get_num();

	if( taskId < 1 || taskId > SCENARIO_COUNT )
	    continue;

	//-------------------------------------//

	fileTxt.next_line();
	fileTxt.next_line();

	GoalTask* goalTask = goal_task_array+taskId-1;

	//--- read task time limits ------//

	fileTxt.get_token();                          // bypass "Year"
	fileTxt.get_token();                          // bypass "Lower"
	goalTask->years_lower = (short) fileTxt.get_num();
	fileTxt.next_line();

	fileTxt.get_token();                          // bypass "Year"
	fileTxt.get_token();                          // bypass "Upper"
	goalTask->years_upper = (short) fileTxt.get_num();
	fileTxt.next_line();

	//--- read task values ------//

	fileTxt.get_token();                          // bypass "Value"
	fileTxt.get_token();                          // bypass "Lower"
	goalTask->value_lower = (short) fileTxt.get_num();
	fileTxt.next_line();

	fileTxt.get_token();                          // bypass "Value"
	fileTxt.get_token();                          // bypass "Upper"
	goalTask->value_upper = (short) fileTxt.get_num();
	fileTxt.next_line();

	//-------- read bonus ---------//

	fileTxt.get_token();                          // bypass "Bonus"
	goalTask->bonus = (short) fileTxt.get_num();
	fileTxt.next_line();

	//------- if all scenario settings have been read -----//

	if( taskId == TASK_COUNT )
	    break;
    }
}

//------- End of function GoalConfig::read_goal_task --------//

//----- Begin of function GoalTask::get_value -----//
//!
//! Get a random value between value_lower & value_upper.
//!
int GoalTask::get_value() {
    return value_lower + m.random(value_upper-value_lower+1);
}

//------- End of function GoalTask::get_value --------//

//----- Begin of function GoalTask::get_years -----//
//!
//! Get a random year between year_lower & year_upper.
//!
int GoalTask::get_years() {
    return years_lower + m.random(years_upper-years_lower+1);
}

//------- End of function GoalTask::get_years --------//
