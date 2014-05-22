//Filename    : OVALSLDR.H
//Description : Header of object ValueSlider

#ifndef __VALSLDR_H
#define __VALSLDR_H

#include <OVGA.H>
#include <OFONT.H>

//--------- Define class ValueSlider --------//
//
//!UI base class for creating and handling value sliders.
class ValueSlider {
public:
    short value_slider_x1, value_slider_y1,
	value_slider_x2, value_slider_y2;
    int format_type;
    int min_val, max_val;
    int *var_ptr;
    int bar_color;
    int bar_width;
    char init_flag, transparent_flag;
    short *value_slider_bitmap;
    Font *font_ptr;

public:
    ValueSlider();
    ~ValueSlider();
    void init(int,int,int,int,int,int,int*,int=1,int=Vga::active_buf->color_up,int=1);
    void init_target(int,int,int,int,int,int);
    void set_target(int,int,int,int,int,int);
    void deinit();
    void paint();
    void refresh();
    int detect();
    void set_font(Font *fontPtr) { font_ptr = fontPtr; }

    //## chea 060999 for adding a target bar on the value slider
private:
    int target_flat;                              //1 is on default=0
    int tar_x_pos;                                //x_pos of the target bar
    int tar_y_pos;                                //y_pos of the target bar
    int tar_height;                               //height of the target bar
    int tar_width;                                //width of the target bar
    int tar_borderColor;                          //border color the target bar
    int tar_shadowColor;                          //shadow color the target bar

};

//--------------------------//

//--------- Define class ValueSliderGroup --------//
//
//!UI base class for creating and handling groups of value sliders whose
//!total value is constant
class ValueSliderGroup {
public:
    int value_slider_num;
    ValueSlider *value_slider_array;

public:
    ValueSliderGroup(int);
    ~ValueSliderGroup()           { delete [] value_slider_array; }
    void paint();
    int detect();
    ValueSlider& operator[](int recNo)  { if (recNo>value_slider_num) recNo=0; return value_slider_array[recNo]; }
};

//--------------------------//
#endif
