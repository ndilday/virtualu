//Filename    : OOPT2.h
//Description : OptStage2 Class Declaration

#ifndef __OOPT2_H
#define __OOPT2_H

#include <OBNDSLDR.H>
#include <OBUTTON.H>
#include <OFINANCE.H>

//----------- Define class OptStage2 --------------//
//
//!No longer in use.
class OptStage2 {

public:
    //*********** temporary ****************//
    double var[COST_RISE_POLICY_COUNT],
	min[COST_RISE_POLICY_COUNT],
	max[COST_RISE_POLICY_COUNT],
	bound_lo[COST_RISE_POLICY_COUNT],
	bound_up[COST_RISE_POLICY_COUNT],
	pref[COST_RISE_POLICY_COUNT],
	result[COST_RISE_POLICY_COUNT];
    char result_flag;
    //*************************************//

public:
    OptStage2();
    ~OptStage2();
    void report(int);
    void detect();
    void show_result();
    void hide_result();

};

extern OptStage2 opt_stage2;

//-------------------------------------------------//
#endif
