//Filename    : OGAME.H
//Description : Game Class Declaration

#ifndef __OGAME_H
#define __OGAME_H

#ifndef __WINDOWS_
#include <WINDOWS.H>
#endif

#ifndef __ALL_H
#include <ALL.H>
#endif

#ifndef __OCONFIG_H
#include <OCONFIG.H>
#endif

//-------- Define game modes -----------//

enum {
    GAME_PREGAME=1,
    GAME_SINGLE_PLAYER,                             // game_mode is set once the player clicks an option on the screen, the game doesn't have to be actually running.
    GAME_TUTORIAL,
    GAME_TEST,
};

//-------- Define class Game -----------//

//!Primary entry point into the Game. Displays main menu page, creates and
//!initializes all objects in a new game or loaded game, preserves global
//!state of game and is written to the game file when the game is saved,
//!etc. To modify the initialization routines, see Game::init() for the
//!methods that are called to initialize each object.
class Game {
public:
    char      init_flag;
    char      started_flag;
    char      game_mode;
    char      game_has_ended;                     // whether game_end() has been called once already and the player is now just staying in the game to continue to play or observe

public:
    Game();

    int     init(int loadGameCall=0);
    void      deinit(int loadGameCall=0);

    void      main_menu();

    int       write_file(File* filePtr);
    int     read_file(File* filePtr);

private:
    void      run_main_menu_option(int optionId);
    void      new_game();
    void      play_scenario();
    void      credit();
    void      disp_version();

    //#### begin fred 0121 ####//
    void      scenario_menu();
    //void			scenario_constructor();
    void      scenario_init();
    void      scenario_report(int refreshFlag);
    int     scenario_detect();

    void      loadgame_menu();
    void      loadgame_init();
    void      loadgame_report(int refreshFlag);
    int     loadgame_detect();

public:
    void      hall_of_fame();

    void      scenario_deinit();
    void      loadgame_deinit();
    //#### end fred 0121 ####//

    //------- new game generation functions -----//

    int       generator(int genMode);
    void      gen_disp(int refreshFlag);
    void      gen_detect();
};

extern Game game;

//-------------------------------------//
#endif
