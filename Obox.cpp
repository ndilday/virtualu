//Filename    : OBOX.CPP
//Description : A collection of box objects

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.H>

#include <KEY.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OMOUSE.H>
#include <OFONT.H>
#include <OBUTTON.H>
#include <OIMGRES.H>
#include <OIFACE.H>
#include <OPOWER.H>
#include <OBOX.H>
#include <OMOUSECR.H>
#include <OVALSLDR.H>

//----------- Define constant -------------//

enum {
    BOX_LINE_SPACE    = 8,
    MAX_BOX_WIDTH     = 600,
    BOX_X_MARGIN      = 30,                         // Horizontal margin
    BOX_TOP_MARGIN    = 20,
    BOX_BUTTON_MARGIN = 32,
    BOX_BOTTOM_MARGIN = 42                          // margin for display the box button
};

//------- constant for arrow box ----------//

enum {                                            // Height of the thick end of the arrow
    ARROW_HEIGHT     = 20,
    ARROW_MARGIN     = 10,                          // Y margin of the arrow between the box top
    ARROW_X_DISTANCE = 40,                          // X distance of the object to the box
    ARROW_Y_DISTANCE = 35                           // Y distance of the object to the box
};

//----- Define static member variables ------//

char Box::opened_flag=0;

//----------- Begin of function Box::open ---------//
//!
//! Open the box with the given coordinations
//!
//! <int> x1, y1, x2, y2 - coordination of the box
//!                        (default : use current settings)
//!
//! [int] downCentre     - whether paint down the center of the box
//!                        (default : 1)
//!
void Box::open(int inX1, int inY1, int inX2, int inY2, int downCentre) {
    box_x1 = inX1;
    box_y1 = inY1;
    box_x2 = inX2;
    box_y2 = inY2;

    paint( downCentre );
}

//------------ End of function Box::open -----------//

//----------- Begin of function Box::open ---------//
//!
//! Given the width & height of the box, and open the box
//!
//! <int> boxWidth, boxHeight - width & height of the box
//!
//! [int] downCentre - whether paint down the center of the box
//!                    (default : 1)
//!
void Box::open(int boxWidth, int boxHeight, int downCentre) {
    box_x1 = (VGA_WIDTH-boxWidth) / 2;
    box_x2 = box_x1 + boxWidth - 1;

    box_y1 = (VGA_HEIGHT-boxHeight) / 2;
    box_y2 = box_y1 + boxHeight - 1;

    paint( downCentre );
}

//------------ End of function Box::open -----------//

//----------- Begin of function Box::paint ---------//

void Box::paint(int downCentre) {
    err_when(opened_flag);                          // double open

    // save to Vga::image_buf
    vga_front.save_area_common_buf( box_x1, box_y1, box_x2, box_y2 );

    vga_front.d3_panel_up( box_x1, box_y1, box_x2, box_y2, 2 );

    if( downCentre )
	vga_front.d3_panel_down( box_x1+4, box_y1+4, box_x2-4, box_y2-4, 1 );

    user_interface.rect( box_x1, box_y1, box_x2, box_y2-2, 2 );

    opened_flag = 1;
}

//------------ End of function Box::paint -----------//

//----------- Begin of function Box::close --------//
//!
//! Close the previously opened box
//!
void Box::close() {
    err_when(!opened_flag);                         // double open

    vga_back.rest_area_common_buf();
    vga.blt_buf(box_x1, box_y1, box_x2, box_y2);

    mouse.get_event();                              // post the click, prevent effect on other windows

    opened_flag = 0;
}

//------------ End of function Box::close -----------//

//----------- Begin of function Box::calc_size ------//
//!
//! Given a string and then calculate the size of the box
//! which can contain the string.
//!
//! <char*> msgStr    = the message string
//! <int>   minHeight = minimum height of the box
//! [int]   x1, y1    = the top left corner of the box
//!
void Box::calc_size(char* msgStr, int minHeight, int x1, int y1) {
    int width  = BOX_X_MARGIN*2 + font_chartsm.text_width(msgStr, -1, MAX_BOX_WIDTH-BOX_X_MARGIN*2)+1;
    int height = minHeight + font_chartsm.text_height(BOX_LINE_SPACE);

    if( x1 < 0 ) {
	box_x1 = (VGA_WIDTH  - width)  / 2;
	box_y1 = (VGA_HEIGHT - height) / 2;
    }
    else {
	box_x1 = x1;
	box_y1 = y1;
    }

    box_x2 = box_x1+width-1;
    box_y2 = box_y1+height;
}

//---------- End of function Box::calc_size -------------//

//---------- Begin of function Box::ok_button ---------//
//!
//! [int] timeOut - 1=enable inactive timeout
//!                 0=disable inactive timeout
//!                 (default : 1 )
//!
void Box::ok_button(int timeOut) {
    Button button;

    button.paint_text( box_x1+(box_x2-box_x1+1)/2-10, box_y2-BOX_BUTTON_MARGIN, "Ok" );

    //	if( sys.debug_session )
    sys.blt_virtual_buf();

    button.wait_press(timeOut);
}

//------------ End of function Box::ok_button ---------//

//---------- Begin of function Box::ask_button ---------//
//!
//! Display two buttons, and wait for player to click one of them
//!
//! [char*] buttonDes1      = the description of button 1 (default : "Ok")
//! [char*] buttonDes2 	   = the description of button 2 (default : "Cancel")
//! [int]   rightClickClose = whether pressing the right button will close the window
//!			     (default: 1)
//!
int Box::ask_button(char* buttonDes1, char* buttonDes2, int rightClickClose) {
    int width;

    width = box_x2-box_x1+1;

    Button buttonOk, buttonCancel;

    int buttonWidth1 = 20 + font_chartsm.text_width(buttonDes1 ? buttonDes1 : "Ok");

    buttonOk.create_text( box_x1+width/2-buttonWidth1-5, box_y2-BOX_BUTTON_MARGIN,
			  (buttonDes1 ? buttonDes1 : "Ok") );

    //## begin zhoubin	000329
    //   buttonCancel.create_text( box_x1+width/2-7 , box_y2-BOX_BUTTON_MARGIN,
    //			     (buttonDes2 ? buttonDes2 : "Cancel") );
    buttonCancel.create_text( box_x1+width/2+5 , box_y2-BOX_BUTTON_MARGIN,
			      (buttonDes2 ? buttonDes2 : "Cancel") );
    //## end zhoubin	000329

    buttonOk.paint();                               // paint button
    buttonCancel.paint();

    //	if( sys.debug_session )
    sys.blt_virtual_buf();

    //..........................................//
    mouse_cursor.set_icon(CURSOR_NORMAL);
    while( 1 ) {
	sys.yield();
	mouse.get_event();

	//		if( sys.debug_session )
	sys.blt_virtual_buf();

	if( buttonOk.detect(buttonOk.str_buf[0], KEY_RETURN) )
	    return 1;

	if( buttonCancel.detect(buttonCancel.str_buf[0], KEY_ESC)
	    // detect right button only when the button is "Cancel"
	    || (rightClickClose && mouse.single_click(1)) ) {
	    mouse.get_event();
	    return 0;
	}
    }
}

//--------- End of function Box::ask_button --------//

//---------- Begin of function Box::ask_button ---------//
//!
//! Display a Ok and Cancel button, and wait for player to click one of them
//!
//! <Button&> buttonOk, buttonCancel = the defined buttons
//! [char*]   strOk, strCancel       = the string of the button Ok & Cancel
//!                                 ( default : "Ok" & "Cancel" )
//!
void Box::ask_button(Button& buttonOk, Button& buttonCancel, char* strOk, char* strCancel ) {
    int width;

    width = box_x2-box_x1+1;

    buttonOk.create_text( box_x1+width/2-36, box_y2-BOX_BUTTON_MARGIN,
			  (strOk ? strOk : "Ok") );

    buttonCancel.create_text( box_x1+width/2+2 , box_y2-BOX_BUTTON_MARGIN,
			      (strCancel  ? strCancel  : "Cancel") );

    buttonOk.paint();                               // paint button
    buttonCancel.paint();

    //	if( sys.debug_session )
    sys.blt_virtual_buf();
}

//--------- End of function Box::ask_button ---------//

//------- Begin of function Box::ask ----------//
//!
//! Popup a message box and ask user 'Ok' or 'Cancel'
//!
//! Syntax :: ask( <char*> )
//!
//! <char*> msgStr     = pointer to the message, use '\n' to seperate lines
//! [char*] buttonDes1 = the description of button 1 (default : "Ok")
//! [char*] buttonDes2 = the description of button 2 (default : "Cancel")
//! [int]   x1, y1     = the left corner of the box, if not given
//!                      center the box on the screen
//!
//! Return : 1 - if user select "Ok" button
//!          0 - if user select "Cancel" button
//!
//!
int Box::ask(char* msgStr, char* buttonDes1, char* buttonDes2, int x1, int y1) {
    int rc;

    // calculate x1, y1, x2, y2 depended on the msgStr
    calc_size(msgStr,BOX_TOP_MARGIN+BOX_BOTTOM_MARGIN,x1,y1);

    vga.use_front();

    paint(1);

    font_chartsm.put_paragraph( box_x1+BOX_X_MARGIN, box_y1+BOX_TOP_MARGIN, box_x2-BOX_X_MARGIN,
				box_y2-BOX_BOTTOM_MARGIN, msgStr, BOX_LINE_SPACE );

    sys.yield();

    rc = ask_button(buttonDes1, buttonDes2);

    close();

    return rc;
}

//---------- End of function Box::ask ----------//

//------- Begin of function Box::msg ----------//
//!
//! Popup a message box and ask user to press the 'Ok' button
//!
//! Syntax :: msg( <char*> )
//!
//! <char*> msgStr 		 = pointer to the message, use '\n' to
//!									seperate lines.
//! [int]   enableTimeOut = enable time out or not (default: 1)
//! [int]   x1, y1 		 = the left corner of the box, if not given
//!                  		   center the box on the screen.
//!
void Box::msg(char* msgStr, int enableTimeOut, int x1, int y1) {
    int savedUseBack = vga.use_back_buf;

    vga.use_front();

    // calculate x1, y1, x2, y2 depended on the msgStr
    calc_size(msgStr,BOX_TOP_MARGIN+BOX_BOTTOM_MARGIN,x1,y1);

    paint(1);

    font_chartsm.put_paragraph( box_x1+BOX_X_MARGIN, box_y1+BOX_TOP_MARGIN, box_x2-BOX_X_MARGIN,
				box_y2-BOX_BOTTOM_MARGIN, msgStr, BOX_LINE_SPACE );

    sys.blt_virtual_buf();                          // blt tihe vrtual front buffer to the screen

    ok_button(enableTimeOut);
    close();

    if( savedUseBack )
	vga.use_back();
}

//---------- End of function Box::msg ----------//

//------- Begin of function Box::progress ----------//
//!
//! Popup a progress indicator
//!
//! Syntax :: progress( <char*> )
//!
//! <char*> msgStr 		 = pointer to the message, use '\n' to
//!									seperate lines.
//! [int]   enableTimeOut = enable time out or not (default: 1)
//! [int]   x1, y1 		 = the left corner of the box, if not given
//!                  		   center the box on the screen.
//!
void Box::progress(int progressVal, int enableTimeOut, int x1, int y1) {
    int savedUseBack = vga.use_back_buf;
    //	static char msgStr1[80]="A new game is being generated ....";
    //	static char msgStr2[80]="Note that all dollars in the game are in thousands.";

    progress_val=(int)(progressVal*8)/100;

    if(progress_val>7)progress_val=7;
    vga.use_front();
    image_interface.put_front(101,192,"IBASE");

    String str("IBASE");
    str+=progress_val;
    if(progress_val>0)
	image_interface.put_front(236,250,str.str_buf);

    //	calc_size(msgStr2,BOX_TOP_MARGIN+BOX_BOTTOM_MARGIN,x1,y1);   // calculate x1, y1, x2, y2 depended on the msgStr
    //	box_y1-=30;
    //	vga_front.d3_panel_up( box_x1, box_y1, box_x2, box_y2, 2 );
    //	vga_front.d3_panel_down( box_x1+4, box_y1+4, box_x2-4, box_y2-4, 1 );

    //	ValueSlider slider1;
    //	slider1.init(box_x1+20,(box_y1+box_y2)/2-8, box_x2-box_x1-40, 17,
    //									0,100,&progress_val,0, V_PINK,3);
    //	slider1.paint();

    //	font_chartsm.put( box_x1+BOX_X_MARGIN, box_y1+BOX_TOP_MARGIN, msgStr1);

    //	font_chartsm.put( box_x1+BOX_X_MARGIN, box_y2-BOX_TOP_MARGIN-10, msgStr2);

    sys.blt_virtual_buf();                          // blt the vrtual front buffer to the screen

    if( savedUseBack )
	vga.use_back();
}

//---------- End of function Box::progress ----------//

void Box::progress_update(int progressVal, int enableTimeOut, int x1, int y1) {
    int savedUseBack = vga.use_back_buf;
    //	static int progress_val;
    progress_val=min(progressVal,100);

    sys.yield();
    vga.use_front();
    //	calc_size(msgStr,BOX_TOP_MARGIN+BOX_BOTTOM_MARGIN,x1,y1);   // calculate x1, y1, x2, y2 depended on the msgStr

    //	ValueSlider slider1;
    //	slider1.init(box_x1+20,(box_y1+box_y2)/2-8, box_x2-box_x1-40, 16,
    //									0,100,&progress_val,0, V_PINK,3);
    //	slider1.paint();

    progress_val=(int)(progressVal*8)/100;

    if(progress_val>7)progress_val=7;
    vga.use_front();

    String str("IBASE");
    str+=progress_val;
    if(progress_val>0)
	image_interface.put_front(236,249,str.str_buf);

    sys.blt_virtual_buf();                          // blt the virtual front buffer to the screen

    if( savedUseBack )
	vga.use_back();
}

//------- Begin of function Box::print ----------//
//!
//! Popup a message box and ask user to press the 'Ok' button
//!
//! <char*> formatStr - formated message with % argument
//! <....>  the argument list
//!
void Box::print(char* formatStr, ... ) {
    //---- format the message and the arguments into one message ----//

    enum { RESULT_STR_LEN=200 };

    static char resultStr[RESULT_STR_LEN+1];

    // the length of the format string cannot > 1/2 of the result string buffer length, the remaining 1/2 is reserved for substituted values.
    err_when( strlen(formatStr) > RESULT_STR_LEN/2 );

    va_list argPtr;                                 // the argument list structure

    va_start( argPtr, formatStr );
    vsprintf( resultStr, formatStr, argPtr );
    va_end( argPtr );

    //------------ display msg --------------//

    msg(resultStr);
}

//---------- End of function Box::print ----------//

//------- Begin of function Box::tell ----------//
//!
//! Popup a box and display a message. Call Box::close() to close the box.
//!
//! Syntax :: tell( <char*> )
//!
//! <char*> tellStr = pointer to the message, use '\n' to seperate lines
//! [int]   x1, y1 = the left corner of the box, if not given
//!                  center the box on the screen
//!
void Box::tell(char* tellStr, int x1, int y1) {
    int savedUseBack = vga.use_back_buf;

    vga.use_front();

    // calculate x1, y1, x2, y2 depended on the tellStr
    calc_size(tellStr,BOX_TOP_MARGIN+BOX_BOTTOM_MARGIN,x1,y1);

    opened_flag = 1;

    // save to Vga::image_buf
    vga_front.save_area_common_buf( box_x1, box_y1, box_x2, box_y2 );
    vga_front.d3_panel_up( box_x1, box_y1, box_x2, box_y2-20, 2 );
    vga_front.d3_panel_down( box_x1+4, box_y1+4, box_x2-4, box_y2-24, 1 );
    user_interface.rect( box_x1, box_y1, box_x2, box_y2-20, 2 );

    font_chartsm.put_paragraph(
	box_x1+BOX_X_MARGIN, box_y1+BOX_TOP_MARGIN, box_x2-BOX_X_MARGIN,
	box_y2-BOX_BOTTOM_MARGIN, tellStr, BOX_LINE_SPACE );

    sys.blt_virtual_buf();                          // blt tihe vrtual front buffer to the screen

    if( savedUseBack )
	vga.use_back();
}

//---------- End of function Box::tell ----------//

/*

//-------- Begin of function Box::arrow_box ---------//
//!
//! Display a box with an arrow pointing at a specific location.
//!
//! Note : box_x1...box_y2, arrow_x and arrow_y must be set before
//!	  calling arrow_box() calc_arrow_box() can be called to
//!	  set the positions.
//!
//! <char*> boxText	 - box text
//! [char*] boxTitle	 - box title, no title if NULL
//! [int]   saveScrFlag	 - save screen or not
//!      		   (default : 1)
//!
void Box::arrow_box(char* boxText, char* boxTitle, int saveScrFlag)
{
save_scr_flag = saveScrFlag;

if( saveScrFlag )
{
if( arrow_x == 0 )
vga_front.save_scr( box_x1, box_y1, box_x2, box_y2 );
else
vga_front.save_scr( min(box_x1,arrow_x), min(box_y1,arrow_y), max(box_x2,arrow_x), max(box_y2,arrow_y) );
}

//------- Draw box (and arrow if specified object) ------//

if( arrow_x )
draw_arrow();

vga_front.bar( box_x1, box_y1, box_x2, box_y2, V_WHITE );

vga_front.bar( box_x1, box_y1, box_x2, box_y1+1, V_BLACK );        // Top border
vga_front.bar( box_x1, box_y2-1, box_x2, box_y2, V_BLACK );        // Bottom border
vga_front.bar( box_x1, box_y1, box_x1+1, box_y2, V_BLACK );        // Left border
vga_front.bar( box_x2-1, box_y1, box_x2, box_y2, V_BLACK );        // Right border

//--------- display description and text -----------//

if( boxTitle && boxTitle[0] )  // has tutor title
{
font_chartsm.put( box_x1+10, box_y1+10, boxTitle );

vga_front.bar( box_x1, box_y1+ARROW_BOX_TITLE_HEIGHT, box_x2, box_y1+ARROW_BOX_TITLE_HEIGHT+1, V_BLACK );  // line between description and tutor text

font_chartsm.put_paragraph( box_x1+ARROW_BOX_X_MARGIN, box_y1+ARROW_BOX_TITLE_HEIGHT+ARROW_BOX_Y_MARGIN,
box_x2-ARROW_BOX_X_MARGIN, box_y2-ARROW_BOX_Y_MARGIN, boxText, ARROW_BOX_LINE_SPACE );
}
else
{
font_chartsm.put_paragraph( box_x1+ARROW_BOX_X_MARGIN, box_y1+ARROW_BOX_Y_MARGIN, box_x2-ARROW_BOX_X_MARGIN,
box_y2-ARROW_BOX_Y_MARGIN, boxText, ARROW_BOX_LINE_SPACE );
}
}
//---------- End of function Box::arrow_box ---------//

//-------- Begin of function Box::close_arrow_box ---------//

void Box::close_arrow_box()
{
if( save_scr_flag )
{
vga_front.rest_scr();
save_scr_flag = 0;
}
}
//-------- End of function Box::close_arrow_box ---------//

//-------- Begin of function Box::calc_arrow_box ---------//
//!
//! Calculate box_x1..box_y2 for arrow_box().
//!
//! <char*> textPtr        = box text ptr
//! <int>   arrowX, arrowY = the pointing location of the arrow
//! [int]   extraHeight    = extra height added to the box
//!
void Box::calc_arrow_box(char* textPtr, int arrowX, int arrowY, int extraHeight)
{
int winWidth  = font_chartsm.text_width( textPtr, -1, ARROW_BOX_WIDTH-ARROW_BOX_X_MARGIN*2) + ARROW_BOX_X_MARGIN*2;

int winHeight = ARROW_BOX_Y_MARGIN*2+font_chartsm.text_height(ARROW_BOX_LINE_SPACE);    // text_width() must be called before calling text_height()

calc_arrow_box( winWidth, winHeight+extraHeight, arrowX, arrowY );
}
//--------- End of function Box::calc_arrow_box ---------//

//-------- Begin of function Box::calc_arrow_box ---------//
//!
//! Calculate box_x1..box_y2 for arrow_box().
//!
//! <int> boxWidth, boxHeight = width and height of the arrow box
//! <int> arrowX  , arrowY    = the arrowing location of the arrow
//!
void Box::calc_arrow_box(int boxWidth, int boxHeight, int arrowX, int arrowY)
{
arrow_x = arrowX;
arrow_y = arrowY;

//---------- find the coordination of the arrow -------//

if( arrow_x < VGA_WIDTH/2 )
{
//------ Upper left part of the screen -------//

if( arrow_y < VGA_HEIGHT/2 )
{
box_x1  = arrow_x + ARROW_X_DISTANCE;
box_y1  = arrow_y + ARROW_Y_DISTANCE;
}
else //----- Lower left part of the screen -------//
{
box_x1  = arrow_x + ARROW_X_DISTANCE;
box_y1  = arrow_y - ARROW_Y_DISTANCE - boxHeight;
}
}
else
{
//------- Upper right part of the screen ---------//

if( arrow_y < VGA_HEIGHT/2 )
{
box_x1  = arrow_x - ARROW_X_DISTANCE - boxWidth;
box_y1  = arrow_y + ARROW_Y_DISTANCE;
}
else //----- Lower right part of the screen -------//
{
box_x1  = arrow_x - ARROW_X_DISTANCE - boxWidth;
box_y1  = arrow_y - ARROW_Y_DISTANCE - boxHeight;
}
}

//-----------------------------------------//

box_x2 = box_x1 + boxWidth  - 1;
box_y2 = box_y1 + boxHeight - 1;

//-------- valid box_x1,box_y1,box_x2 and box_y2 ----------//

if( box_x1 < 0 )
{
box_x2 += -box_x1+10;
box_x1 += -box_x1+10;
}

if( box_x2 >= VGA_WIDTH )
{
box_x1 -= (box_x2-VGA_WIDTH)+10;
box_x2 -= (box_x2-VGA_WIDTH)+10;
}

if( box_y1 < 0 )
{
box_y2 += -box_y1+10;
box_y1 += -box_y1+10;
}

if( box_y2 >= VGA_HEIGHT )
{
box_y1 -= (box_y2-VGA_HEIGHT)+10;
box_y2 -= (box_y2-VGA_HEIGHT)+10;
}
}
//-------- End of function Box::calc_arrow_box ---------//

//-------- Begin of function Box::draw_arrow ---------//
//!
//! Draw an arrow based on box_x1...box_y1 and arrow_x and arrow_y
//!
void Box::draw_arrow()
{
    int tx2, ty2;

//---------- find the coordination of the arrow -------//

    if( arrow_x < VGA_WIDTH/2 )
    {
//------ Upper left part of the screen -------//

	if( arrow_y < VGA_HEIGHT/2 )
	{
	    tx2 = box_x1+1;
	    ty2 = box_y1+ARROW_MARGIN;
	}
	else //----- Lower left part of the screen -------//
	{
	    tx2 = box_x1+1;
	    ty2 = box_y2-ARROW_MARGIN-ARROW_HEIGHT;
	}
    }
    else
    {
//------- Upper right part of the screen ---------//

	if( arrow_y < VGA_HEIGHT/2 )
	{
	    tx2 = box_x2-1;
	    ty2 = box_y1+ARROW_MARGIN;
	}
	else //----- Lower right part of the screen -------//
	{
	    tx2 = box_x2-1;
	    ty2 = box_y2-ARROW_MARGIN-ARROW_HEIGHT;
	}
    }

//-------- draw and fill the arrow --------//

    int ty;

    mouse.hide();

    for( ty=ty2 ; ty<=ty2+ARROW_HEIGHT ; ty++ )
	VGAline( arrow_x, arrow_y, tx2, ty, V_WHITE );

    VGAline( arrow_x, arrow_y, tx2, ty2, V_BLACK );
    VGAline( arrow_x, arrow_y, tx2, ty2+ARROW_HEIGHT, V_BLACK );

    mouse.show();
}
//---------- End of function Box::draw_arrow ---------//

*/
