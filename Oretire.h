//Filename    : ORETIRE.H
//Description : Retirement report Class Declaration
//Owner		  : Kevin(ho)

#ifndef __ORETIRE_H
#define __ORETIRE_H

//----------- Define class Development --------------//
//
//!Class for generating retirement report displayed at end of game.
class RetirementReport {
public:
    RetirementReport();
    ~RetirementReport();
    void init();
    void deinit();
    void retirement_report(int);
    int retirement_detect();
};

extern RetirementReport retirement;

//-------------------------------------------------//
#endif
