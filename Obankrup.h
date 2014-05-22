//Filename    : OBANKRUP.H
//Description : Bankruptcy report Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OBANKRUP_H
#define __OBANKRUP_H

//----------- Define class Development --------------//
//
//!Contains controls for the Bankruptcy Report screen.
class BankruptcyReport {
public:
    BankruptcyReport();
    ~BankruptcyReport();
    void init();
    void deinit();
    void bankruptcy_report(int);
    int bankruptcy_detect();
};

extern BankruptcyReport bankruptcy;

//-------------------------------------------------//
#endif
