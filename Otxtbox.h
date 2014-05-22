//Filename    : OTXTBOX.H
//Description : Header file of Object Text Box

#ifndef __OTXTBOX_H
#define __OTXTBOX_H

//--------- Define class TxtBox ----------//

//!UI base class for creating and handling Text Boxes.
class TxtBox {
public:
    char   init_flag;
    short  text_x1  , text_y1  , text_x2  , text_y2;
    short  scroll_x1, scroll_y1, scroll_x2, scroll_y2;

    char*  text_ptr;
    short  top_line;

public:
    TxtBox()   { init_flag=0; }

    void  init(int,int,int,int);

    void  disp(char*,int);
    int   detect();
};

//-------------------------------------------//
#endif
