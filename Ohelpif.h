//Filename    : OHELPIF.H
//Description : Help Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OHELP_IF_H
#define __OHELP_IF_H

#include<OTxtList.h>

//----------- Define class Development --------------//
//
//!Class which manages the help screen interface, namely the help topic and
//!description buttons.
class HelpIf {
public:
    TxtList help_textlist;
    HelpIf();
    ~HelpIf();
    void init();
    void deinit();
    void help_topic_report(int);
    void help_desc_report(int);
    int help_topic_detect();
    int help_desc_detect();
};

extern HelpIf help_if;

//-------------------------------------------------//
#endif
