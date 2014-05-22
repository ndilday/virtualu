//Filename    : OLOSEGAM.H
//Description : Retirement report Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OLOSEGAM_H
#define __OLOSEGAM_H

enum {                                            // reasonId parameter of set_reason function
    LOSE_REASON_TIME_UP=0,
    LOSE_REASON_PROTAGONIST,                        // terminated by threshold event
    LOSE_REASON_UNACCEPTABLE,
};

//----------- Define class Development --------------//
//
//!Class for generating a report displayed when player loses the game.
class LoseGameReport {
public:
    int lose_reason_id;
    int reason_para0;
    int reason_para1;
    int reason_para2;
    int reason_para3;
    int reason_para4;

public:
    LoseGameReport();
    ~LoseGameReport();
    void init();
    void deinit();
    void lose_game_report(int);
    int lose_game_detect();

    void set_reason(int reasonId, int para0=0, int para1=0, int para2=0, int para3=0, int para4=0 );
    char *get_reason_str_protagonist();           // for reasonId == LOSE_REASON_PROTAGONIST
};

extern LoseGameReport lose_game;

//-------------------------------------------------//
#endif
