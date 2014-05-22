//*************** Temporary ****************//

#ifndef __OGSET_H
#define __OGSET_H

//!Class for setting up new game parameters as well as playing scenario
//!introductions.
class GameSetting {
public:
    GameSetting();
    ~GameSetting();

    void new_game_init();
    void new_game_report(int);
    int  new_game_detect();
    void play_scenario_init();
    void play_scenario_report(int);
    int  play_scenario_detect();
};

extern GameSetting game_setting;
#endif
