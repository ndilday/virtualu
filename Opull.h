//Filename    : OPULL.H
//Description : Header of object PullDownMenu

#ifndef __OPULL_H
#define __OPULL_H

//--------- Define class PullDownMenu --------//
//
//!UI base class for creating and handling pull down menus.
class PullDownMenu {
public:
    short pull_down_menu_x1, pull_down_menu_x2,
	pull_down_menu_y1, pull_down_menu_y2;
    short pull_down_length;
    int num_item, sel_item;
    char **menu_item;

    short bar_width;
    char bar_color1, bar_color2, slider_color;
    double *lo_ptr, *up_ptr, *var_ptr, min_val, max_val, val_range;
    char init_flag;
    char *scr_buf;

public:
    void init(int,int,int,int,int,int,int,double*,double*,double*,double,double);
    void paint();
    void refresh();
    int detect();
};

//--------------------------//
#endif
