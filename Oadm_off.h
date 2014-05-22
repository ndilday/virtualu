//Filename    : OADM_OFF.H
//Description : AdmOffice Class Declaration

#ifndef __OADM_OFF_H
#define __OADM_OFF_H

//----------- Define class AdmOffice --------------//
//
//!UI class for the Admissions Office screen.
class AdmOffice {
public:
    char current_mode;
    char sub_current_mode;

    AdmOffice();
    ~AdmOffice();
    void init();
    void deinit();
    void report(int);
    int detect();
    int detect2();
};

extern AdmOffice adm_office;

//-------------------------------------------------//
#endif
