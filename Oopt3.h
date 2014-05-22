//Filename    : OOPT3.h
//Description : OptStage3 Class Declaration

#ifndef __OOPT3_H
#define __OOPT3_H

#include <OBUTTON.H>
#include <OFINANCE.H>

//***** Temporary *****//
enum {
    DEPARTMENT_NUM = 9,
    FACULTY_NUM = 20
};
//*********************//

//----------- Define class OptStage3 --------------//
//
//!UI for optimization of faculty and departments.
class OptStage3 {

public:
    //*********** temporary ****************//
    double pref[MAX_DEPARTMENT];
    short existing[MAX_DEPARTMENT];
    short new_hire[MAX_DEPARTMENT];
    int remain_hire1;
    int remain_hire2;
    int remain_hire3;
    int line_spacing;
    //*************************************//

private:
    void draw_count_bar(int);
    int detect_count_bar(int);
    void update_remain();

public:
    OptStage3();
    ~OptStage3();
    void report(int);
    void detect();

};

extern OptStage3 opt_stage3;

//-------------------------------------------------//
#endif
