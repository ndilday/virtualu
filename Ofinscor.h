//Filename    : OFINSCOR.H
//Description : FinalScore report Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OFINSCOR_H
#define __OFINSCOR_H

//----------- Define class Development --------------//
//
//!UI class for the Final Score screen.
class FinalScoreReport {
public:
    FinalScoreReport();
    ~FinalScoreReport();
    void init();
    void deinit();
    void final_score_report(int);
    int final_score_detect();
};

extern FinalScoreReport final_score;

//-------------------------------------------------//
#endif
