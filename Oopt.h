//Filename    : OOPT.H
//Description : Optimization Class Declaration

#ifndef __OOPT_H
#define __OOPT_H

#include <OFINANCE.H>
#include <OBUTTON.H>
#include <OBNDSLDR.H>

enum {
    PREF_COUNT = 4,
    PREF_DEFAULT = 0
};

//----------- Define class Optimization --------------//
//
//!Class used for calculating optimization settings for reporting
//!parameters based upon the model.
class Optimization {
public:
    char  current_stage;
    double pref[MAX_DEPARTMENT];
    short existing[MAX_DEPARTMENT];
    short new_hire[MAX_DEPARTMENT];
    int remain_hire1;
    int remain_hire2;
    int remain_hire3;
    int line_spacing;
    char stage_init_flag;
    char button_optimize_is_down;
    char is_optimized[3];
    char is_optimize_slider_modified[3];

    BoundedSlider opt1_slider_group[REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT];
    ButtonGroup *opt1_button_group_array[(REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT)];
    short *opt1_button_group_up_bitmap[(REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT) * PREF_COUNT];
    short *opt1_button_group_down_bitmap[(REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT) * PREF_COUNT];

    BoundedSlider opt2_slider_group[COST_RISE_POLICY_COUNT];
    ButtonGroup *opt2_button_group_array[COST_RISE_POLICY_COUNT];
    short *opt2_button_group_up_bitmap[COST_RISE_POLICY_COUNT * PREF_COUNT];
    short *opt2_button_group_down_bitmap[COST_RISE_POLICY_COUNT * PREF_COUNT];

    BoundedSlider opt3_slider_group[MAX_DEPARTMENT];
    ButtonGroup *opt3_button_group_array[MAX_DEPARTMENT];
    short *opt3_button_group_up_bitmap[MAX_DEPARTMENT * PREF_COUNT];
    short *opt3_button_group_down_bitmap[MAX_DEPARTMENT * PREF_COUNT];

    char  refresh_optimization_screen;

    int pre_target_value[MAX_DEPARTMENT];

private:
    char  result_flag;

public:
    Optimization();
    ~Optimization();

    void  init();
    void  deinit();

    void  disp(int);
    int   detect();
    int   detect2();

    void  report_stage_1(int refreshFlag);
    void  report_stage_2(int refreshFlag);
    void  report_stage_3(int refreshFlag);

    int   detect_stage_1();
    int   detect_stage_2();
    int   detect_stage_3();

private:
    void  init_stage_1();
    void  init_stage_2();
    void  init_stage_3();

    void  deinit_stage_1();
    void  deinit_stage_2();
    void  deinit_stage_3();
};

extern Optimization optimization;

//-------------------------------------------------//
#endif
