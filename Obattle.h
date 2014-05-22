//Filename    : OBATTLE.H
//Description : Header file for Battle object
//Owner       : Fred

#ifndef __OBATTLE_H
#define __OBATTLE_H

//---------- Define class Battle --------//

//!Physically generates the map and begins the simulation.
class Battle {
public:
    void  init();
    void  deinit();

    void  run(int scenId=0);                      // 0: not a scenario
    void  run_loaded();

    void  create_static_game_object();            // also called by load game; just before reading gfile

private:
    void  create_pregame_object();
    void  create_pregame_object_scenario1();      //0127

    void  generate_road();
    void  generate_firm();
    void  generate_plant();
    void  generate_sprite();

    void  init_performance_indicator();

    void  create_firm(int firmId);
};

extern Battle battle;

//---------------------------------------//
#endif
