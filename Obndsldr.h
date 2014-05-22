//Filename    : OBNDSLDR.H
//Description : Header of object BoundedSlider

#ifndef __OBNDSLDR_H
#define __OBNDSLDR_H

#include <OVGA.H>

enum {NONE,INDICATOR,LOWER,UPPER};
//--------- Define class BoundedSlider --------//
//
//!UI class for drawing sliders.
class BoundedSlider {
public:
    short bounded_slider_x1, bounded_slider_x2, bounded_slider_y1, bounded_slider_y2;
    short bar_width;
    char bar_color, slider_color;
    double *lo_ptr, *up_ptr, *var_ptr, min_val, max_val, val_range;
    char lower_bound_null_flag;
    char init_flag, transparent_flag, enable_flag, result_flag;
    short *bounded_slider_bitmap;
    char highlight_indicator_flag;
    int data_fmt;
    double result_value;
    char  lock_max, lock_min;
public:
    BoundedSlider();
    ~BoundedSlider();
    void init(int,int,int,int,double*,double*,double*,double,double,char=1,int=Vga::active_buf->color_up,int=Vga::active_buf->color_dark,int=8);
    void deinit();
    void paint();
    void refresh();
    int detect();
};

//--------------------------//
#endif
