//Filename    : OMISCREP.H
//Description : misc report Class Declaration
//Owner		  : Fred

#ifndef __OMISCREP_H
#define __OMISCREP_H

//----------- Define class MiscReport --------------//
//
//!Class used for presenting and displaying gold/silver/bronze plaque
//!report.
class MiscReport {
public:
    MiscReport();
    ~MiscReport();
    void award_init();
    void award_deinit();

    void award_report(int);
    int award_detect();
    int award_detect2();

    int page_num;
};

extern MiscReport misc_report;

//-------------------------------------------------//
#endif
