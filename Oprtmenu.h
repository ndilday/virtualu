//Filename    : OPRTMENU.H
//Description : Printing Menu Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OPRTMENU_H
#define __OPRTMENU_H

#include <OPSCHOOL.H>

//----------- Define class Development --------------//
//
//!Provides UI for printing options as well as handling headers and
//!printing of different kinds of reports. Wraps around Print class.
class PrintMenu {
private:
    static char* scn_name[SCENARIO_COUNT];

public:
    int print_view_mode;
    int print_option;
    int init_flag;
    char in_print_mode;

public:
    PrintMenu();
    ~PrintMenu();
    void init();
    void deinit();
    void report(int);
    void printing();
    void printing1();
    void printing2();
    void printing3();
    void printing4();
    void printing5();
    void printing6();
    void printing7();
    void printing8();
    int detect();

private:
    void print_header();
};

extern PrintMenu print_menu;

//-------------------------------------------------//
#endif
