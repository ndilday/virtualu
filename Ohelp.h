//Filename    : OHELP.H
//Description : Header file of object Help

#ifndef __OHELP_H
#define __OHELP_H

//----------- Define constant -------------//

#define NULL 0

enum {                                            // Maximum no. of help info allowed
    MAX_HELP_INFO=200
};

//------------------------------------------//
//
// Structure of the HELP.RES :
//
// <Area X1>, <Area Y1>, <Area X2>, <Area Y2>
// <Help Title>
// <Help Text>
//
// <Page Break Code, CTRL-L>
//
// <Area X1>, <Area Y1>, <Area X2>, <Area Y2>
// <Help Title>
// <Help Text>
//      .
//		  .
// (Another help block, continue)
//
//------------------------------------------//

//---------- Define struct HelpInfo ------------//

//!
struct HelpInfo {
    enum { HELP_CODE_LEN=8, HELP_TITLE_LEN=100 };

    char  help_code[HELP_CODE_LEN+1];               // either identify the help message by help code or area position
    short area800x600_x1, area800x600_y1, area800x600_x2, area800x600_y2;
    short area1024x768_x1, area1024x768_y1, area1024x768_x2, area1024x768_y2;
    short monster_human_interface;                  // 0 when display for both interfaces
    // 1 when display only for monster interface
    // 2 when display only for human interface

    char  help_title[HELP_TITLE_LEN+1];

    char* help_text_ptr;                            // offset of the help text in the text buffer
    short help_text_len;                            // length of the help text
};

class Blob2DW;
class VgaBuf;

//!Class for cacheing recently viewed help screens for quick viewing. Help
//!class performs swapping and management.
class HelpSaveScreen {
    char      clear_flag;
    char      hiding_flag;
    char      hide_area_count;
    char      dummy;
    short       save_scr_x1, save_scr_y1, save_scr_x2, save_scr_y2;

public:
    HelpSaveScreen();
    ~HelpSaveScreen();

    void      init();
    void      deinit();
    void      swap( HelpSaveScreen &);

    void      save_scr(int x1, int y1, int x2, int y2, VgaBuf* =0 );
    void      rest_scr(VgaBuf* =0, int keepUnclear=0 );
    void      hide_area(int x1, int y1, int x2, int y2);
    void      show_area();
    void      clear();
    void      unclear();
    int     is_clear();

    Blob2DW   *save_scr_buf;                      // have a save screen buffer for displaying the helpial text
};

//-----------------------------------------------//

//!Class which manages displaying the help screens and keeping track of
//!which screen return to when back is selected.
class Help {
public:
    enum      { SCREEN_CODE_LEN=8 };
    enum      { CUSTOM_HELP_TITLE_LEN=80, CUSTOM_HELP_DETAIL_LEN=200 };

    char      custom_help_title[CUSTOM_HELP_TITLE_LEN+1];
    char      custom_help_detail[CUSTOM_HELP_DETAIL_LEN+1];

    char      help_code[HelpInfo::HELP_CODE_LEN+1];
    short     help_x, help_y;
    short     help_x1, help_y1, help_x2, help_y2;

    short       help_info_count;

    // ##### begin Gilbert 9/9 ######//
    short     first_help_by_help_code;
    short     last_help_by_help_code;
    short     first_help_by_area;
    short     last_help_by_area;
    // ##### end Gilbert 9/9 ######//

    HelpInfo    *help_info_array;
    char        *help_text_buf;
    int     help_text_buf_size;

    HelpSaveScreen  long_save_buf;
    HelpSaveScreen  short_front_buf;
    HelpSaveScreen  short_back_buf;

    short     last_mouse_x;
    short     last_mouse_y;
    int     mouse_still_time;

public:
    Help();
    ~Help();

    void       init(char* resName);
    void       deinit();

    void       load(char*);

    void      push_screen(char* screenCode);
    void      pop_screen();
    void      set_screen(char* screenCode);

    void      set_help(int x1, int y1, int x2, int y2, char* helpCode);
    void      set_help2(int x1, int y1, int width, int height, char* helpCode);
    void      set_unit_help(int unitId, int rankId, int x1, int y1, int x2, int y2);
    void      set_custom_help(int x1, int y1, int x2, int y2, char* helpTitle, char* helpDetail=NULL);

    int     should_disp();
    void       disp();
    void      disp_help(int centerX, int centerY, char* helpTitle, char* helpDetail);

    void      disp_short_help(VgaBuf *);
    void      hide_short_help(VgaBuf *);
    void      hide_area(int x1, int y1, int x2, int y2);
    void      show_area();

    void      flip();
};

//-----------------------------------------------//

extern Help help;
#endif
