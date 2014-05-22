//Filename    : OBLACKBROAD.H
//Description : Help Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OBLACKBROAD_H
#define __OBLACKBROAD_H

//----------- Define class Development --------------//
//
//!Contains controls for Blackboard (help) screens.
class BlackBoard {
public:
    BlackBoard();
    ~BlackBoard();

    char log_mode;
    void init();
    void deinit();
    void black_message_report(int);
    void black_objective_report(int);
    void black_log_report(int);
    void black_log_deinit();
    int black_log_detect();
};

enum {NEWS_MODE, OBJECTIVE_MODE};

extern BlackBoard blackboard;

//-------------------------------------------------//
#endif
