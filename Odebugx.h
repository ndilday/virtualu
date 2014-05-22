#ifndef __ODEBUGX_H
#define __ODEBUGX_H

//---------- define class DebugX ----------//
//!? ? ?
class DebugX {
public:
    char  debug_session;

public:
    DebugX();
    ~DebugX() {}

    int is_debug();
};

extern DebugX debugx;
#endif
