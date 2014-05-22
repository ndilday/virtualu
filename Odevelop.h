//Filename    : ODEVELOP.H
//Description : Athletics Office Class Declaration
//Owner		  : Kevin(ho)

#ifndef __ODEVELOP_H
#define __ODEVELOP_H

//##### begin fred 980803 #####//
#include <GAMEDEF.h>

//!Structure containing breakdown of cash gifts to the university;
//!operations, endowment, and facilities.
struct GiftComponent {
    float total;

    float cur_operations;
    float endowment;
    float facilities;
};
//##### end fred 980803 #####//

//----------- Define class Development --------------//
//
//!Class for simulating the development office. Calculates alumni
//!donations.
class Development {
public:
    Development();
    ~Development();
    void init();
    void deinit();
    void report(int);
    int detect();
    int detect2();

    char current_mode;
    //##### begin fred 980803 #####//
    int     write_file(File* filePtr);
    int     read_file(File* filePtr);

    //----- vars and methods related to simulation and statistics
public:
    //### begin zhoubin 02/25/99
    float spinner_var1;
    //### end zhoubin 02/25/99
    // allocation_policy: 0.3 => 30%
    GiftComponent this_year, last_year, allocation_policy;

    float percent_alumni_gave_gift;
    float alumni_morale;

    float percent_alumni_gave_gift_history[HISTORY_YEAR_COUNT];
    float alumni_morale_history[HISTORY_YEAR_COUNT];
    float history_value[HISTORY_YEAR_COUNT2];     // revenue and expense of the office

    float initializationValueAC_INST_ADVANCEMENT;

    //## chea 191099 try to display the total gifts
    float gift_total_next_year;
    float gift_change_last_year;
    float gift_change_next_year;

    //## chea 091299 7.2.3 should add change here
    //	float this_year_total_up2date;

    // ##### Begin MArco ##### //
    float initial_percent_alumni_gave_gift;
    float initial_alumni_morale;
    float initial_gift_total;
    //	GiftComponent	initial_last_year;
    float initial_last_year_endowment;
    // ##### End Marco ##### //

private:
    int init_inst_advancement;
    int init_prestige;

    char  multiplier_alumni;

public:
    float gift_growth_rate;
    float this_month_total_gift;
    //float	next_month_total_gift;

public:
    void  init_data();
    void  next_day();

    void  calc_alumni_morale();

    // ##### Begin Marco ##### //
    void save_initial_data();
    // ##### End Marco ##### //

private:
    void  update_history(char update_flag);
    void  update_history_sub();
    void  calc_gift_growth_rate();
    void  calc_gift_all();
    void  calc_percent_alumni_gave_gift();
};

extern Development development_office;

//-------------------------------------------------//
#endif
