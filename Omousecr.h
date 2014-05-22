//Filename    : OMOUSECR.H
//Description : Header file of Object Cursor resource

#ifndef __OMOUSECR_H
#define __OMOUSECR_H

#include <ORESDB.H>
#include <OBLOB2W.H>

// ------------ Define constant -------- //

#define MAX_VGA_WIDTH 800
#define MAX_VGA_HEIGHT 600

//------------ Define cursor id. ----------//

enum {
    CURSOR_NORMAL=1,
    CURSOR_NORMAL_DOWN,                             // when the normal cursor is clicked on something
    CURSOR_WAITING,
};

//------------ Define struct CursorRec ---------------//

//!Structure for managing cursors.
struct CursorRec {
    enum { HOT_SPOT_LEN=3, FILE_NAME_LEN=8, BITMAP_PTR_LEN=4 };

    char file_name[FILE_NAME_LEN];

    char hot_spot_x[HOT_SPOT_LEN];
    char hot_spot_y[HOT_SPOT_LEN];

    char bitmap_ptr[BITMAP_PTR_LEN];
};

//------------- Define struct CursorInfo --------------//

//!UI structure for cursor information.
struct CursorInfo {
    short hot_spot_x;
    short hot_spot_y;

    char* bitmap_ptr;
};

//--------- Define class MouseCursor --------//

class Blob2DW;
class VgaBuf;

//!Class for controlling the display of the mouse cursor depending on the
//!mode, location, and wait state.
class MouseCursor {
public:
    char  init_flag;
    char  hide_all_flag;
    char  hide_area_flag;

    short cur_icon;

    int   hide_x1, hide_y1, hide_x2, hide_y2;
    int   cur_x1, cur_y1, cur_x2, cur_y2;

    int hot_spot_x, hot_spot_y;

    int   icon_width;
    int   icon_height;

    char  cursor_shown;                           // whether the cursor has been shown on the screen
    char  processing_flag;                        // whether process() is being run or not, prevent nested call by interrupt
    char  wait_mode_count;                        // allow nested wait cursor

    char  *icon_ptr, *icon_data_ptr;
    Blob2DW *save_scr, *save_back_scr, *merge_buf;

    //------- nation selection frame --------//

    char  frame_flag;                             // whether nation selection frame is on
    char  frame_shown;

    int frame_x1, frame_y1, frame_x2, frame_y2;
    int frame_origin_x, frame_origin_y;
    int frame_border_x1, frame_border_y1, frame_border_x2, frame_border_y2;

    short frame_top_save_scr[MAX_VGA_WIDTH+2];    // for saving the nation selection frame
    short frame_bottom_save_scr[MAX_VGA_WIDTH+2]; // +4 is for the width & height info
    short frame_left_save_scr[MAX_VGA_HEIGHT+2];
    short frame_right_save_scr[MAX_VGA_HEIGHT+2];

    //------- cursor bitmap resource --------//

private:
    short      cursor_count;
    CursorInfo*  cursor_info_array;

    ResourceDb   res_bitmap;

public:
    MouseCursor();
    ~MouseCursor();

    void  init();
    void  deinit();

    void  set_icon(int);
    void  set_frame(char,char=0);
    void  set_frame_border(int,int,int,int);

    void  process(int,int);
    void  disp_back_buf(int,int,int,int);

    // ####### begin Gilbert 18/8 ########//
    int get_icon()        { return cur_icon; }
    void  restore_icon(int);
    // ####### end Gilbert 18/8 ########//

private:
    void  process_frame(int,int);
    void  disp_frame(VgaBuf*);
    int is_touch(int,int,int,int);

    void  load_cursor_info();
};

extern MouseCursor mouse_cursor;

//----------------------------------------------//
#endif
