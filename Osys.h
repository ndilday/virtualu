//Filename	  : OSYS.H
//Description : Header file for Game class

#ifndef __OSYS_H
#define __OSYS_H

#ifndef __ALL_H
#include <ALL.H>
#endif

#ifndef __OFIRMID_H
#include <OFIRMID.H>
#endif

#ifndef __WINDOWS_
#include <WINDOWS.H>
#endif

//------ Define common data buffer size  ------//

enum {                                            // keep a 256K common buffer for all temporary memory storage like reading files temporarily into memory
    COMMON_DATA_BUF_SIZE = 256*1024
};

//------ Define number of frames per day ------//

#define FRAMES_PER_DAY  10

//------------ sys_flag --------------//

enum { SYS_PREGAME=0, SYS_RUN };

//--------- game version ---------//

enum {
    VERSION_FULL,
    VERSION_DEMO
};

//--------- Define info modes --------//

enum { DEFAULT_SPEED=25 };
enum { VIEW_MODE_COUNT=1 };

enum {
    MODE_NORMAL,

    MODE_FACULTY_SUMMARY,
    MODE_STUDENT_REPORT,
    MODE_COURSE_REPORT,
    MODE_PERFORMANCE_REPORT,
    MODE_FINANCIAL_REPORT,
    MODE_SCORE_REPORT,

    MODE_FACULTY_LIST,                              // place here because it is not one of the report buttons.
    MODE_GAME_OPTIONS,                              // game options
    MODE_YEAR_END_REPORT,
    MODE_MORE_REPORT,
    MODE_DEPT_LIST,
    MODE_SCENARIO_LETTER,
    MODE_RETIREMENT,
    MODE_BANKRUPTCY,
    MODE_LOSE_GAME,
    MODE_FINAL_SCORE,
    MODE_AWARD_REPORT,
    MODE_PRINTING_MENU,
    MODE_PAUSE,
    MODE_TEST,
    MODE_HELP_TOPIC,
    MODE_HELP_DESC,
    MODE_SCEN_WELCOME_LETTER,
    MODE_WELCOME_LETTER,
    MODE_EVALUATION_LETTER,
    MODE_STATE_REPORT,
    MODE_BLACKBOARD_LOG,
    MODE_MISC,                                      // for other screens that take exclusive control of the program flow, e.g.the report displayed at the end of a fiscal year

    MODE_DEPARTMENTAL_FIRM,
    MODE_FIRST_NONDEPARTMENTAL_FIRM,
    MODE_LAST_NONDEPARTMENTAL_FIRM = MODE_FIRST_NONDEPARTMENTAL_FIRM+NONDEPARTMENTAL_FIRM_TYPE_COUNT-1,
};

//-------- Define class Sys -----------//

//!System resource management class. Sys::run() is the main loop of the
//!game. The most important part of this loop is a call of Sys::process(),
//!which increments the day frame counter and checks if a new day has
//!arrived. If so, it calls Info::next_day(), which informs each module
//!that a new day has arrived. Subsequently, it also informs each module if
//!a new month, trimester, or year has come. Also implements autosave
//!feature.
class Sys {
private:
    char    init_flag;

public:
    HWND    main_hwnd;
    HINSTANCE app_hinstance;                      // handle of the application running
    char    game_version;                         // VERSION_???

    char     active_flag;                         // we are the active app.
    char     paused_flag;                         // whether the game(video?) is paused
    char     pause_game_flag;                     // whether the game is paused
    char    sys_flag;
    char    signal_exit_flag;

    char    redraw_all_flag;                      // redraw map & zoom map
    char    redraw_zoom_flag;                     // redraw zoom map only
    int   last_redraw_time;
    char    cheat_enabled_flag;
    char    no_true_output_flag;

    char    prev_view_mode;
    char    view_mode;                            // the view mode can be MODE_???
    char    back_view_mode;                       // the view mode to go back
    char    staying_view_mode;                    // the view mode which the game must stay at until the player actually finish seeing the report, e.g. the year-end report

    char    news_pop_up_flag;                     // ## chwg0219
    int   news_pop_up_time;
    char    is_institution_report;                // whether there is an institution report displayed currently, if so the top area will show the university name rather than the selected department name

    char    map_need_redraw;
    char    zoom_need_redraw;

    //------ frame related vars -----//

    int     day_frame_count;
    DWORD   next_frame_time;                      // next frame's time for maintaining a specific game speed

    //----- multiplayer vars ----//

    DWORD   frame_count;                          // frame count, for is_sync_frame only

    //---- continous key string -----//

    enum {                                        // Maximum 10 different key string
	MAX_KEY_STR = 10
    };

    int     key_str_pos[MAX_KEY_STR];             // for detecting cheating codes

    //-------- statistic --------//

    DWORD   last_second_time;
    int   frames_in_this_second;
    int   frames_per_second;                      // the actual frames per second

    //------- file paths -------//

    char    cdrom_drive;

    char    dir_image[MAX_PATH+1];
    char    dir_encyc[MAX_PATH+1];
    char    dir_movie[MAX_PATH+1];
    char    dir_tutorial[MAX_PATH+1];
    char    dir_scenario[MAX_PATH+1];

    //------- other vars --------//

    char*   common_data_buf;
    char    debug_session;
    char    testing_session;
    // add in (GAME_VERSION>=200)
    char    use_true_front;

    char    menu_pull_down;                       // boolean flag indicating whether
    // the menu (upper-right corner) has
    // been pull down

    // ## begin chwg1012
    int save_speed, save_view_mode;
    // ## end chwg1012
public:
    Sys();
    ~Sys();

    //------------- init/deinit functions -------------//

    int   init(HANDLE hInstance);
    void    deinit();
    void    deinit_directx();
    void    deinit_objects();

    //------------ main process functions -------------//

    long    main_win_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    pause();
    void    unpause();

    void    run(int=0);
    void    yield();
    void    set_news_pop_up();

    //----------- other functions ----------//

    int   should_next_frame();

    void    set_speed(int frameSpeed);
    void    set_view_mode(int viewMode, int playSound=1);
    void    set_staying_view_mode(int viewMode);

    void    blt_virtual_buf();

    //---- for setting game directories ----//

    void    set_game_dir();
    int     set_one_dir( char* checkFileName, char* defaultDir, char* trueDir );
    void    get_cdrom_drive();

    //-------- for load/save games --------//

    int     write_file(File* filePtr);
    int   read_file(File* filePtr);

    void    save_game();
    void    load_game();

    //-------- for game options menu ---- ##chwg1006//
    void    setting_report(int refreshFlag);
    int   setting_report_detect(void);
    void    setting_report_deinit(void);
    void    auto_save(void);

    int   m_o_log_selected;
    int   m_o_log_highlighted;
    int   m_o_log_old_selected;
    int   m_o_log_old_highlighted;
    int   m_o_log_display_updated;
    int   report_before_morerpt_or_deptlist;      //chwg0111
    int    report_before_help;                    //chwg0713
    //##begin zhoubin 02/23/99
    int    old_Dept;
    bool   bDept;
    //##end zhoubin 02/23/99

private:
    int   init_win();
    int   init_directx();
    int     init_objects();

    void    main_loop(int);
    void    detect();
    void    process();
    void    process_key(unsigned scanCode, unsigned skeyState);
    int     detect_cheat_key(unsigned scanCode, unsigned skeyState);

    void    disp_button();
    int     detect_report_button();               // return value: 1-detected, 0-not detected
    int     detect_dept_button();
    int   detect_mail_button();
    int   detect_function_button();
    void    detect_main_screen_help();
    void    deinit_report();

    void    disp_frame();
    void    detect_view();

    BOOL    restore();

    void    load_game_now(int gameBrowseRecno);
};

extern Sys sys;

//-------------------------------------//
#endif
