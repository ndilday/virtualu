//Filename    : OSLIDER.H
//Description : Header of object Slider

#ifndef __OSLIDER_H
#define __OSLIDER_H

//--------- Define class Slider --------//

//!UI base class for creating and handling sliders.
class Slider {
public:
    short slider_x1, slider_y1, slider_x2, slider_y2;
    short bar_width;

    char  bar_color, bar_color2;

    char  enable_flag;
    int*  var_ptr;
    int   max_value, std_value;

public:
    void init( int,int,int,int,int,int*,int,int=0,int=0);

    void paint();
    void refresh();
    int  detect();
};

//------- Define class SliderGroup --------//

//!UI base class for creating and handling logical groups of sliders whose
//!total value is constant.
class SliderGroup {
public:
    int     slider_num;
    Slider* slider_array;

public:
    SliderGroup(int);
    ~SliderGroup()                { delete slider_array; }

    void paint();
    int  detect();

    Slider& operator[](int recNo) { if( recNo>slider_num ) recNo=0; return slider_array[recNo]; }
};

//-----------------------------------------//
#endif
