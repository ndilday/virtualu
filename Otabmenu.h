//Filename    : OTABMENU.H
//Description : Header of object TabMenu

#ifndef __TABMENU_H
#define __TABMENU_H

//--------- Define class TabMenu --------//
//
//!UI base class for creating and handling Tab menus.
class TabMenu {
public:
    short tab_menu_x1, tab_menu_y1,
	tab_menu_x2, tab_menu_y2;
    short short_view_length, long_view_length;
    short *back_img;
    int option_num;
    int *sel_num;
    char **short_desc, **long_desc;
    int (*disp_func)(int, int);
    int (*detect_func)(int);
    char init_flag;

public:
    TabMenu();
    ~TabMenu();
    void init(int,int,int,int,int,int*,int,int,char**,char**,int(*)(int,int),int(*)(int));
    void deinit();
    void refresh(int);
    int detect();

private:
    void draw_tab();
    void label_tab();
    short *tab_start, *tab_end;
    short *scr_buf;

};

//--------------------------//
#endif
