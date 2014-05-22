//Filename    : OSPINNER.H
//Description : Header of object Spinner

#ifndef __OSPINNER_H
#define __OSPINNER_H

#include <OVGABUF.H>
#include <OBUTTON.H>
#include <OGETA.H>
#include <OFONT.H>

//--------- Define class Spinner --------//
//
//!? ? ?
class Spinner {
public:
    //### begin zb 02/23/99
    static char focus;
    static Spinner* which;
    //### end zb 02/23/99
    short spinner_x1, spinner_y1, spinner_x2, spinner_y2;
    float min_val, max_val, step_val, *var_ptr;
    char transparent_flag, init_flag, cursor_flag, input_flag;
    int fill_color, format_type;
    int original_game_speed;
    Button button_up, button_down;
    short *button_up_bitmap, *button_down_bitmap;
    short *button_up_pressed_bitmap, *button_down_pressed_bitmap;
    short *button_up_disable_bitmap, *button_down_disable_bitmap;
    short *field_bitmap;
    char paint_flag;
    GetA get_a;
    char *get_a_field;
    Font *font_ptr;
    int pause_when_focus;

public:
    Spinner();
    ~Spinner();
    void init(int, int, int, int, float*, float, float, float, char=1, int=vga_back.color_up, int=1);
    void deinit();
    void paint();
    void refresh();
    int detect();
    void set_font(Font *fontPtr) { font_ptr = fontPtr; }
};
//--------------------------------------------//
#endif
