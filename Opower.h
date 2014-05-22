//Filename    : OPOWER.H
//Description : Header file of Object Power

#ifndef __OPOWER_H
#define __OPOWER_H

//--------- Define class Power ----------//
//!Class which handles mouse inputs and allows for mouse events to be
//!dis/enabled.
class Power {
    //----------- Define constant -----------//
    enum {                                          // Maximum 5 different key string
	MAX_KEY_STR = 5
    };

public:
    char      win_opened;
    char      enable_flag;

    // in sys.h: int  			key_str_pos[MAX_KEY_STR];  // for detecting cheating codes

public:
    Power();
    ~Power();

    void      init();
    void      deinit();

    void      enable()    { enable_flag=1; }
    void      disable() { enable_flag=0; }

    void      mouse_handler();
    void      reset_selection();
};

extern Power power;

//---------------------------------------//
#endif
