//Filename    : OBUTTON.H
//Description : Header file of button object

#ifndef __OBUTTON_H
#define __OBUTTON_H

#ifndef __OVGABUF_H
#include <OVGABUF.H>
#endif

//------- Define constant ---------//

//##### begin Norris 0713 #####//

// Added BUTTON_BITMAPW for 16-bit bitmap
enum  { BUTTON_TEXT=1, BUTTON_BITMAP=2, BUTTON_UDF=3, BUTTON_BITMAPW=4 };

//####### end Norris 0713 #####//

//----------- Define variable type -----------//

typedef void (*ButtonFP)(int,int,int,int);        // user defined function to be called

//-------------- Define class Button -------------//

class Font;

//!UI class for drawing buttons
class Button {
public:
    enum  { STR_BUF_LEN=40, HELP_CODE_LEN=8 };

    char          is_pushed;
    char          enable_flag;                    // either 1(Yes) or 0(No)
    short         x1,y1,x2,y2;                    // some function will need to access the button's coordination for area detection

    char         init_flag;
    char         button_type;
    char        use_texture_flag;
    unsigned short button_key;                    // button is pressed when user press this key
    char        is_showing;

    char          str_buf[STR_BUF_LEN+1];
    void*         body_ptr;                       // can be a text pointer, bitmap pointer or a pointer to user defined function
    char          elastic;
    Font*         font_ptr;
    short*    button_bk_bitmap;
    short*    button_up_bitmap;
    short*    button_dn_bitmap;
    char        help_code[HELP_CODE_LEN+1];

public:
    Button();
    ~Button();

    void deinit();
    void use_texture()             { use_texture_flag=1; }

    void reset()                         { init_flag=0; }
    void set_font(Font*);
    void set_key(unsigned keyCode)       { button_key = keyCode; }
    void create(int,int,int,int,int,void*,char=1,char=0);
    void set_body(void*);
    void set_help_code(char* helpCode);

    //-------- text button ------------//

    void create_text(int x1,int y1,int x2,int y2,char* textPtr,char elastic=1,char defIsPushed=0)
	{ create( BUTTON_TEXT,x1,y1,x2,y2,textPtr,elastic,defIsPushed); }

    void paint_text(int x1,int y1,int x2,int y2,char* textPtr,char elastic=1,char defIsPushed=0)
	{ create( BUTTON_TEXT,x1,y1,x2,y2,textPtr,elastic,defIsPushed); paint(); }

    void create_text(int x1,int y1,char* textPtr,char elastic=1,char defIsPushed=0);

    void paint_text(int x1,int y1,char* textPtr,char elastic=1,char defIsPushed=0);

    //-------- bitmap button -------------//

    void create_bitmap(int x1,int y1,int x2,int y2,char* iconPtr,char elastic=1,char defIsPushed=0)
	{ create( BUTTON_BITMAP,x1,y1,x2,y2,iconPtr,elastic,defIsPushed); }

    void paint_bitmap(int x1,int y1,int x2,int y2,char* iconPtr,char elastic=1,char defIsPushed=0)
	{ create( BUTTON_BITMAP,x1,y1,x2,y2,iconPtr,elastic,defIsPushed); paint(); }

    //###### begin Norris 0707 ########//
    void create_bitmapW(int x1,int y1,int x2,int y2,short* iconPtr,char elastic=1,char defIsPushed=0)
	{ create( BUTTON_BITMAPW,x1,y1,x2,y2,iconPtr,elastic,defIsPushed); }

    void paint_bitmapW(int x1,int y1,int x2,int y2,short* iconPtr,char elastic=1,char defIsPushed=0)
	{ create( BUTTON_BITMAPW,x1,y1,x2,y2,iconPtr,elastic,defIsPushed); paint(); }
    //######## end Norris 0707 ########//

    //-------- user-defined function button ---------//

    void create_udf(int x1,int y1,int x2,int y2,ButtonFP funcPtr,char elastic=1,char defIsPushed=0)
	{ create( BUTTON_UDF,x1,y1,x2,y2,funcPtr,elastic,defIsPushed); }

    void paint_udf(int x1,int y1,int x2,int y2,ButtonFP funcPtr,char elastic=1,char defIsPushed=0)
	{ create( BUTTON_UDF,x1,y1,x2,y2,funcPtr,elastic,defIsPushed); paint(); }

    //--------------------------------------//

    void paint(int= -1, int=1);

    int  detect(unsigned=0,unsigned=0,int=0,int=0);

    void hide(char);
    void show();

    void push()        { if(!is_pushed) paint(1); }
    void pop()         { if(is_pushed)  paint(0);  }

    void disable() {
	if(enable_flag) {
	    enable_flag=0; paint();
	}
    }
    void enable() {
	if(!enable_flag) {
	    enable_flag=1; paint();
	}
    }

    void wait_press(int inactiveTimeOut=0);
};

//------- Define class ButtonGroup ----------------//

//!UI class for drawing radio buttons.
class ButtonGroup {
public:
    int     button_num;
    int     button_pressed;
    Button* button_array;

public:
    ButtonGroup(int);
    ~ButtonGroup();
    void  deinit_buttons();                       //call deinit to each button

    void paint(int= -1);
    int  detect();
    void push(int);

    Button& operator[](int);
    int     operator()()          { return button_pressed; }
};

//-------------------------------------------------//
#endif
