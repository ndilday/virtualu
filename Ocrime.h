//Filename    : OCRIME.H
//Description : Crime report Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OCRIME_H
#define __OCRIME_H

//----------- Define class Development --------------//
//
//!
class Crime {
public:
    float spinner_var1;
public:
    Crime();
    ~Crime();
    void init();
    void deinit();
    void crime_report(int);
    int crime_detect();
};

extern Crime crime;

//-------------------------------------------------//
#endif
