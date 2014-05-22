//Filename    : OSCEN_IF.H
//Description : Scenario report Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OSCEN_IF_H
#define __OSCEN_IF_H

//----------- Define class Development --------------//
//
//!Class for displaying scenario description, acheivment and objective
//!reports (Gold/Silver/Bronze). Includes content.
class ScenarioReport {
public:
    ScenarioReport();
    ~ScenarioReport();
    void init();
    void deinit();
    void scenario_report_report(int);
    int scenario_report_detect();
    char scn_current_state;
};

extern ScenarioReport scenario_report;

//-------------------------------------------------//
#endif
