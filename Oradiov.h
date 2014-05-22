//Filename    : ORADIOV.H
//Description : Header of object vertical RadioButton
//Owner		  : Kevin(ho)
#ifndef __ORADIOV_H
#define __ORADIOV_H

#include <OVGA.H>
#include <OFONT.H>

//--------- Define class RadioButton --------//
//
//!UI base class for creating and handling vertical radio buttons.
class RadioButtonVert {
public:
    short radio_button_x1, radio_button_y1,
	radio_button_x2, radio_button_y2;
    short radio_button_x;
    short radio_button_width, radio_button_height;
    short paragraph_width, paragraph_height;
    int option_num;
    int *sel_num;
    int button_color;
    char **option_desc;
    short *desc_y;
    short *radio_button_bitmap;
    Font *font_ptr;
    char init_flag;

private:
    void fill(short,short,int);
    void circle(short,short,int,int,char);

public:
    RadioButtonVert();
    ~RadioButtonVert();
    void init(int,int,int,int,int,int*,char**,int=vga_back.color_border);
    void deinit();
    void paint();
    void refresh();
    int detect();
    void set_font(Font *fontPtr) { font_ptr = fontPtr; }

};

//--------------------------//
#endif
