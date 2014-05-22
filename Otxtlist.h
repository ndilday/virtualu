//Filename    : OTXTLIST.H
//Description : Header file of Object Text List

#ifndef __OTXTLIST_H
#define __OTXTLIST_H

#ifndef __ORESX_H
#include <ORESX.H>
#endif

#ifndef __ORESTXT_H
#include <ORESTXT.H>
#endif

#ifndef __OVBROWSE_H
#include <OVBROWSE.H>
#endif

//--------- Define class TxtList ----------//

//!Class for maintaining a history of ResTxt to provide "back"
//!functionality.
class TxtList {
public:
    ResTxt      res_txt;
    ResourceIdx res_pict;

    char        init_flag;
    char        init_pict_flag;
    char        init_list_flag;

    short       text_x1  , text_y1  , text_x2  , text_y2;
    short       pict_x1  , pict_y1  , pict_x2  , pict_y2;
    short       list_x1  , list_y1  , list_x2  , list_y2;
    short       scroll_x1, scroll_y1, scroll_x2, scroll_y2;

    short       topic_id;
    short       top_line;

    char      can_back;

    enum     { HISTORY_SIZE=20 };

    short       history_array[HISTORY_SIZE];      // hyper-text topic history
    short       history_pos;

    char      double_clicked;

public:
    TxtList();

    void  init(int,int,int,int,char*);
    void  init_pict(int,int,int,int,char*);
    void  init_list(int,int,int,int);

    void  deinit();

    void  disp(int,int=0);
    int   detect();
    int   detect_text();

    void  add_history(int);
    void  back_history();
};

//-------------------------------------------//
#endif
