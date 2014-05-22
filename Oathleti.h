//Filename    : OATHLETI.H
//Description : Athletics Office Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OATHLETICS_H
#define __OATHLETICS_H

//##### begin fred 981024 #####//
#include <GAMEDEF.H>

//----------- Define constants -------------//

enum {
    NCAA_LEVEL_COUNT = 3,
};

enum {
    ATHLETICS_DIVISION_3 = 0,
    ATHLETICS_DIVISION_2,
    ATHLETICS_DIVISION_1
};

//----------- Define class AdmOffice --------------//
//
//!The Athletics Department module. This class contains the athletic office
//!functions. It calculates the revenues and expense for the athletics
//!department and win-loss records of the athletics teams. Used by
//!Info::next_day() to process daily operations.
class Athletics {
public:
    Athletics();
    ~Athletics();
    void init();
    void deinit();
    void report(int);
    int detect();
    int detect2();

    char current_mode;

    //##### begin fred 981021 #####//
    int     write_file(File* filePtr);
    int     read_file(File* filePtr);

    //----- vars and methods related to simulation and statistics
public:
    // player interface related
    //### begin zhoubin 02/25/99
    float spinner_var1;
    //### end zhoubin 02/25/99
    int ncaa_level_input;                         // INPUT_LOW (div3), INPUT_MID (div2), INPUT_HIGH (div1)
    float ncaa_level_value;                       //## 990510	// High:   value = 1.5; Medium: value = 3; Low:    value = 4.5
    int last_ncaa_level_input;

    int football_graph[HISTORY_YEAR_COUNT2];      // use int since class GraphA don't accept char array
    int basketball_graph[HISTORY_YEAR_COUNT2];
    int initial_football_graph[2];                // [0] inital of football_graph[0] and 1 for football_graph[HISTORY_YEAR_COUNT]
    int initial_basketball_graph[2];              // [0] inital of basketball_graph[0] and 1 for basketball_graph[HISTORY_YEAR_COUNT]

    float history_value[HISTORY_YEAR_COUNT2];

    //----- intermediate vars -----//
public:
    int total_game;
    int total_football_game;
    int total_basketball_game;
    float avg_percent_win;                        // 0.30 => 30%
    float football_percent_win;
    float basketball_percent_win;

private:
    int init_expense;
    int init_top_male_matrics;
    int init_top_female_matrics;
    char  init_ability_male_matrics;
    char  init_ability_female_matrics;
    float init_win_prob;

    float revenue_multiplier;

    char  fraction_intercollege_cost;
    int total_cost_norm;

public:
    // for welcome letter
    float football_probability_win;               // =[0.0, 1.0]
    float basketball_probability_win;

    int football_game_count[NCAA_LEVEL_COUNT];
    int basketball_game_count[NCAA_LEVEL_COUNT];

public:
    void  init_data_pre_finance();
    void  init_data();
    void  next_day();
    void  input_2_ncaa_level(int newNcaaLevel);
    void  calc_adjusted_revenue_expense(int &revenue, int &expense);
    void  save_initial_data();

private:
    void  update_win_lost(bool isFootballGame);
    void  update_history(char update_flag);
    void  calc_probability_win();
    void  update_history_sub();

    //##### end fred 981021 #####//
};

extern Athletics athletics_office;

//-------------------------------------------------//
#endif
