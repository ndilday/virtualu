//Filename    : OLIBTECH.H
//Description : Athletics Office Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OLIBTECH_H
#define __OLIBTECH_H

//##### begin fred 980829 #####//
#include <GAMEDEF.h>

#define INTERNET_QUALITY_LEVEL_COUNT  4
//##### end fred 980829 #####//

//----------- Define class LibTech --------------//
//
//!LibTech Class Definition related to Simulation and Statistics/History of
//!technical library.
class LibTech {
public:
    LibTech();
    ~LibTech();
    void init();
    void deinit();
    void report(int);
    int detect();
    int detect2();
    char current_mode;

    //##### begin fred 981020 #####//
    int     write_file(File* filePtr);
    int     read_file(File* filePtr);

public:

    //----- vars and methods related to simulation and statistics
    // IF output
    //### begin zhoubin 02/28/99
    float spinner_var1,spinner_var2;
    //### end zhoubin 02/28/99
    int volumns_count_library;
    float percent_faculty_using_it;
    char  internet_quality;                       // 0: bad -- (INTERNET_QUALITY_LEVLE_COUNT-1): best
    float internet_quality_percent;               // for other model // = [0:100]
    float tech_utilization;                       // Institution-level IT usage
    //float	it_usage;						//addif: old var to be removed

    // IF input
    float faculty_incentive_using_it;             // = [0:100]
    float percent_dl_tuition;                     // = [0:100]

    // vars for overview screen
    int volumns_count_library_history[HISTORY_MONTH_COUNT];
    float percent_faculty_using_it_history[HISTORY_MONTH_COUNT];
    int internet_quality_history[HISTORY_MONTH_COUNT];
    float tech_utilization_history[HISTORY_MONTH_COUNT];

    // vars for finance screen
    float library_history[HISTORY_MONTH_COUNT];
    float it_history[HISTORY_MONTH_COUNT];
    float overall_history[HISTORY_MONTH_COUNT];
    float net_dl_expenditures_history[HISTORY_TRIMESTER_COUNT];

    // ##### Begin Marco #### //
    int initial_volumns_count_library;
    float initial_percent_faculty_using_it;
    float initial_internet_quality_percent;
    float initial_tech_utilization;
    float initial_faculty_incentive_using_it;
    float initial_percent_dl_tuition;
    // ##### End Marco ##### //

public:
    void  init_data();
    void  next_day();
    void  update_history(char update_flag);
    void  update_history_monthly();
    void  calc_it_usage();

    // ##### Begin Marco ##### //
    void  save_initial_data();
    // ##### End Marco ##### //
};
//##### end fred 981020 #####//

extern LibTech library_tech_office;
//-------------------------------------------------//
#endif
