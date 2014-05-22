//Filename	  : OGCONFIG.H
//Description : Header file for Game GoalGoalConfig class

#ifndef __OGCONFIG_H
#define __OGCONFIG_H

#include <OPSCHOOL.H>
#include <OTASK.H>

//---------- Define struct GoalScen -----------//

#define TOTAL_GOAL_SCEN_VALUE_TYPE    3

//!
struct GoalScen {
    short goal_year[TOTAL_PLAQUE_COUNT];
    float goal_value[TOTAL_PLAQUE_COUNT];
};

//-------- Define struct GoalTask -------//

//!
struct GoalTask {
    short years_lower;
    short years_upper;
    short value_lower;
    short value_upper;
    short bonus;

    int get_value();                                // get a random value between value_lower & value_upper
    int get_years();
};

//---------- Define class GoalConfig -----------//

//!
class GoalConfig {
public:
    GoalScen goal_scen_array[SCENARIO_COUNT];
    GoalTask goal_task_array[TASK_COUNT];

public:
    void  init();
    void  deinit();

    GoalScen* goal_scen(int scenId)   { return goal_scen_array+scenId-1; }
    GoalTask* goal_task(int taskId)   { return goal_task_array+taskId-1; }

private:
    void  read_goal_scen();
    void  read_goal_task();
};

//------------------------------------------//

extern GoalConfig goal_config;
#endif
