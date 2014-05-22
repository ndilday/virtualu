//Filename    : OOPT1.h
//Description : OptStage1 Class Declaration

#ifndef __OOPT1_H
#define __OOPT1_H

#include <OBNDSLDR.H>
#include <OBUTTON.H>
#include <OFINANCE.H>

//----------- Define class OptStage1 --------------//
//
//!No longer in use.
class OptStage1 {

public:
    //*********** temporary ****************//
    double var[REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT],
	min[REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT],
	max[REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT],
	bound_lo[REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT],
	bound_up[REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT],
	pref[REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT],
	result[REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT];
    char result_flag;
    //*************************************//

public:
    OptStage1();
    ~OptStage1();
    void report(int);
    void detect();
    void show_result();
    void hide_result();

};

extern OptStage1 opt_stage1;

//-------------------------------------------------//
#endif
