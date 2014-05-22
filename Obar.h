// Bar.h: interface for the Bar class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __OBAR_H
#define __OBAR_H

#if _MSC_VER >= 1000
#pragma once
#endif                                            // _MSC_VER >= 1000

#include <OVGA.H>
#include <OFONT.H>

//!UI class for drawing bar graphs.
class Bar {
public:
    short bar_x1, bar_y1,
	bar_x2, bar_y2;
    short bar_y;
    short bar_width, bar_height;
    int bar_color;
    char **option_desc;
    short *bar_bitmap;
    Font *font_ptr;
    char init_flag;

public:
    void init( int, int, int, int, int);
    void deinit(void);
    void paint(void);
    int detect(void);
    void refresh(void);
    Bar();
    virtual ~Bar();

};

//--------------------------//
#endif
