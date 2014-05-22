//Filename	  : OINFO.H
//Description : Header file for class Info

#ifndef __OINFO_H
#define __OINFO_H

#include <OIMGRES.H>

//-------- Define interface constant --------//

enum {  REPORT_ITEMS = 7  };

enum {
    MENU_BUTTON_X1 = 753,
    MENU_BUTTON_Y1 = 0,
    MENU_BUTTON_X2 = 797,
    MENU_BUTTON_Y2 = 29
};

enum {
    REPORT_BUTTON_X1 = 0,
    REPORT_BUTTON_Y1 = 0,
    REPORT_BUTTON_X2 = 360,
    REPORT_BUTTON_Y2 = 31
};

enum {
    COLUMN_WIDTH  = 25,
    COLUMN_HEIGHT = 50
};

enum {
    SURPLUS_INFO_X1 = 370,
    SURPLUS_INFO_Y1 = 5,
    SURPLUS_INFO_X2 = 500,
    SURPLUS_INFO_Y2 = 23
};

enum {
    SCORE_INFO_X1 = 544,
    SCORE_INFO_Y1 = 11,
    SCORE_INFO_X2 = 570,
    SCORE_INFO_Y2 = 31
};

enum {
    U_SCORE_INFO_X1 = 624,
    U_SCORE_INFO_Y1 = 11,
    U_SCORE_INFO_X2 = 650,
    U_SCORE_INFO_Y2 = 31
};

enum {
    MONTH_X1      = 662,
    MONTH_Y1      = 11,
    SEASON_X1   = 704,
    SEASON_Y1   = 11,
    DAY_X1      = 663,
    DAY_X2      = 700,
    YEAR_X1     = 707,
    YEAR_X2     = 745,
    DIGIT_Y1      = 25,
    DIGIT_Y2      = 38,
};

enum {
    DATE_INFO_X1  = 663,
    DATE_INFO_Y1  = 25,
    DATE_INFO_X2  = 700,
    DATE_INFO_Y2  = 38,
};

enum {
    YEAR_INFO_X1  = 706,
    YEAR_INFO_Y1  = 25,
    YEAR_INFO_X2  = 744,
    YEAR_INFO_Y2  = 38,
};

enum {
    TOP_INFO_X1 = 150,                              //222,
    TOP_INFO_Y1 = 27,
    TOP_INFO_X2 = 570,
    TOP_INFO_Y2 = 43
};

enum {
    BOTTOM_INFO_X1  = 122,
    BOTTOM_INFO_Y1  = 488,
    BOTTOM_INFO_X2  = 684,
    BOTTOM_INFO_Y2  = 506,
};

enum {
    MORE_REPORT_X1  = 10,
    MORE_REPORT_Y1  = 28,
    MORE_REPORT_X2  = 150,
    MORE_REPORT_Y2  = 45
};

enum {
    HELP_QUEST_X1 = 772,
    HELP_QUEST_Y1 = 29,
    HELP_QUEST_X2 = 795,
    HELP_QUEST_Y2 = 46
};

enum {
    LIST_BUTTON_X1  = 630,
    LIST_BUTTON_Y1  = 31,
    LIST_BUTTON_X2  = 645,
    LIST_BUTTON_Y2  = 45
};

enum {
    GRAPH1_X1=450,GRAPH1_X2=600,
    GRAPH1_Y1=80,GRAPH1_Y2=180,
    GRAPH2_X1=450,GRAPH2_X2=600,
    GRAPH2_Y1=210,GRAPH2_Y2=310,
    GRAPH3_X1=450,GRAPH3_X2=600,
    GRAPH3_Y1=340,GRAPH3_Y2=440,
    GRAPH4_X1=610,GRAPH4_X2=760,
    GRAPH4_Y1=80,GRAPH4_Y2=180,
    GRAPH5_X1=610,GRAPH5_X2=760,
    GRAPH5_Y1=210,GRAPH5_Y2=310,
    GRAPH6_X1=610,GRAPH6_X2=760,
    GRAPH6_Y1=340,GRAPH6_Y2=440,
};

/*
enum{
  GRAPH1_X1=50,GRAPH1_X2=190,
  GRAPH1_Y1=80,GRAPH1_Y2=180,
  GRAPH2_X1=50,GRAPH2_X2=190,
  GRAPH2_Y1=210,GRAPH2_Y2=310,
  GRAPH3_X1=50,GRAPH3_X2=190,
  GRAPH3_Y1=340,GRAPH3_Y2=440,
  GRAPH4_X1=610,GRAPH4_X2=750,
  GRAPH4_Y1=80,GRAPH4_Y2=180,
  GRAPH5_X1=610,GRAPH5_X2=750,
GRAPH5_Y1=210,GRAPH5_Y2=310,
GRAPH6_X1=610,GRAPH6_X2=750,
GRAPH6_Y1=340,GRAPH6_Y2=440,
};
*/

enum {
    MIN_MAX_X1=GRAPH4_X1+120,
    MIN_MAX_Y1=GRAPH4_Y1-20,
    MIN_MAX_X2=GRAPH4_X1+135,
    MIN_MAX_Y2=GRAPH4_Y1-3
};

enum {                                            // formatFlag for date_str_ymd and date_str_julian
    INFO_DATE_NO_DAY       = 0x0001,                // don't display date. if day is 0 also suppress
    INFO_DATE_NO_MONTH     = 0x0010,                // don't display month. if month is 0 also suppress
    INFO_DATE_SHORT_MONTH  = 0x0020,                // short form month (Jan/Feb...)
    INFO_DATE_NO_YEAR      = 0x0100,                // don't display year. if month is 0 also suppress
    INFO_DATE_F_YEAR       = 0x0200,                // use financial year
    INFO_DATE_ADD_YR       = 0x0400,                // add Yr before year, eg. DD MONTH Yr.YY instead of DD MONTH, YY

    INFO_DATE_DEFAULT = INFO_DATE_F_YEAR|INFO_DATE_ADD_YR
};

//----------- Define class Info ------------//

class VgaBuf;

//!Class representing the state of a particular scenario including dates,
//!statistics, random seed, and screen layout. Can be read/written to a
//!file.
class Info {
public:
    int   game_start_date;                        // set to Game::game_start_year in Info::init(), the actual date the game begins, not the scenario begins
    int   game_date;                              // 0429 public for odynarrb.cpp
    int   game_day;
    int   game_month;
    int   game_year;
    int   prerun_year;                            //## chea added to count the pre-run year
    int financial_year();

    //#### begin fred 0911 ####//
    int   year_passed;                            // no. of years passed since playing

    // for statistics graph plotting
    int   graph_year_passed;                      // no. of years passed since playing
    int   graph_month_passed;                     // no. of years passed since playing
    int   graph_trimester_passed;                 // no. of years passed since playing

    // for projected statistics graph
    int graph_year;
    int graph_month;
    int graph_trimester;
    //#### end fred 0911 ####//

    unsigned long start_play_time;                // the time player start playing the game today
    unsigned long total_play_time;                // total time the player has played in all saved games

    long random_seed_4_save;                      //## chea 210899 to save the random seed before save
    int last_year_array[5];                       //## chea 150999 student no.last year

    float avg_tal_acd;                            //## chea 200100
    float avg_tal_ext;                            //## chea 200100
    float avg_tal_ath;                            //## chea 200100

private:
    int   goal_deadline;
    short goal_difficulty;
    short goal_score_bonus;

    int   week_day;                               // = game_date&7
    int   year_day;                               // the nth day in a year

    int  random_seed;

    char  last_write_offset;                      // it is used for an offset locator for excluding static member vars during reading and writing files

    char* play_time_str();                        // return total time played in string
    char* game_duration_str();

    //---------------------------------//

    short *date_info_bitmap;
    short *year_info_bitmap;

    short *top_info_bitmap;
    short *bottom_info_bitmap;
    short *surplus_info_bitmap;
    short *score_info_bitmap;
    short *uscore_info_bitmap;
    short *more_report_bitmap;
    short *help_quest_bitmap;
    short *list_button_bitmap;

    short *top_left_column_bitmap;
    short *top_right_column_bitmap;
    short *bottom_left_column_bitmap;
    short *bottom_right_column_bitmap;

    ImageRes    date_res_bitmap;

public:
    short *menu_down_bitmap, *menu_up_bitmap;
    char  *report_bitmap[REPORT_ITEMS + 1];

public:
    Info();
    ~Info();

    void  init();
    void  deinit();

    int   write_file(File* filePtr);
    int read_file(File* filePtr);

    void  init_random_seed(int randomSeed);

    void  disp_panel_init();

    void  disp_column_bitmap(VgaBuf*);

    void  disp();

    void  next_day();
    bool  is_begin_trimester();
    void  disp_6_graphs();
    char  disp_6_graphs_flag;

    char* date_str_ymd( int year, int month, int day, int formatFlag=INFO_DATE_DEFAULT);
    char* date_str_julian( long julianDate, int formatFlag=INFO_DATE_DEFAULT);

    void  update_distance_learning_variables(char initValue=0);

private:
    void  disp_selected_name();
    void  disp_calendar();
    void  new_trimester_course_enrollment();
    void  end_prerun_year();
    void  inc_game_day();
};

extern Info info;

//------------------------------------------//
#endif
