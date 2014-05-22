//Filename    : OCONFIG.CPP
//Description : Config Object

#include <OCONFIG.H>
#include <OHELP.H>

//----- Begin of function Config::init() ------//

void Config::init() {
    default_game_setting();
    music_flag=1;
    context_sensitive_help=1;
    cd_music_volume=1;                              // a value from 0 to 100
    wav_music_volume=100;                           // a value from 0 to 100
    sound_effect_volume=100;                        // a value from 0 to 100
}

//------ End of function Config::init() -------//

//----- Begin of function Config::deinit() ------//

void Config::deinit() {

}

//------ End of function Config::deinit() -------//

//---- Begin of function Config::default_game_setting ----//

void Config::default_game_setting() {
    disp_year_end_report = 1;
    auto_save = 0;
    log_enable = 0;                                 // 0=none, 1=trimester, 2=year
    log_file_name[0] = '\0';                        // not include extension, which is .<year>, inital is empty, ask name in first log file
    disp_building_label = 1;
}

//----- End of function Config::default_game_setting -----//
