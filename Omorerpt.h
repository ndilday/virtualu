//Filename    : OMORERPT.H
//Description : More report Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OMORERPT_H
#define __OMORERPT_H

//----------- Define class Development --------------//
//
//!Class which handles UI for detailed reporting of major topics:
//!admissions and financial aid [A], athletics [T], development and alumni
//!affairs [D], facilities management [F], libraries and information
//!technology [L], investment [I], crime [C], parking [K]
class MoreReport {
public:
    MoreReport();
    ~MoreReport();
    void init();
    void deinit();
    void more_report_report(int);
    int more_report_detect();
};

extern MoreReport more_report;

//-------------------------------------------------//
#endif
