//Filename	  : OCONFIG.H
//Description : Header file for Game Config class

#ifndef __OCONFIG_H
#define __OCONFIG_H

#include <GAMEDEF.H>

#ifndef __OFILE_H
#include <OFILE.H>
#endif

//------------- Define constant -------------//

enum { OPTION_NONE=0, OPTION_LOW, OPTION_MODERATE, OPTION_HIGH, OPTION_VERY_HIGH };

//---------- Define class Config -----------//

//!Game class for configuring game settings.
class Config {
public:
    void      init();
    void      deinit();

    int       write_file(File* filePtr);
    int     read_file(File* filePtr, int keepSavedSysSettings=1);

    void      default_game_setting();

    //int			save(char *);		// save to file
    //int			load(char *);		// load from file

public:

    //------- game options --------//

    char      disp_year_end_report;               // whether a report should be displayed at the end of each academic year
    char      context_sensitive_help;
    int     auto_save;
    char      auto_save_file_name[MAX_PATH];      // include extension, which is .<year>, inital is empty, ask name in first autosave
    int     log_enable;                           // 0=none, 1=year, 2=trimester
    char      log_file_name[MAX_PATH];
    char      disp_building_label;

    //------ speed settings -------//

    short     scroll_speed;                       // map scrolling speed. 1-slowest, 10-fastest
    short     frame_speed;                        // game speed, the desired frames per second

    //------- sound effect --------//

    char      music_flag;
    short     cd_music_volume;                    // a value from 0 to 100
    short     wav_music_volume;                   // a value from 0 to 100

    char      sound_effect_flag;
    short     sound_effect_volume;                // a value from 0 to 100
};

//------------------------------------------//

extern Config config;
#endif
