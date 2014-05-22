//Filename    : OTXTBOX.CPP
//Description : Object text box

#include <OVGA.H>
#include <OINFO.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OSCROLL.H>
#include <OIFACE.H>
#include <OTXTBOX.H>

//-------- Define macro constant ----------//

#define SCROLL_BAR_WIDTH 15

#define TEXT_LINE_SPACE   6
#define TEXT_MARGIN       8

//--------- Define static vars ------------//

static  ScrollBar scroll_bar;

//---------- Begin of function TxtBox::init -------//
//!
//! <int>   x1, y1, x2, y2 = the position of the text list
//!
void TxtBox::init(int x1, int y1, int x2, int y2) {
    text_x1 = x1;
    text_y1 = y1;
    text_x2 = x2-SCROLL_BAR_WIDTH-4;                // space + scroll bar width
    text_y2 = y2;

    scroll_x1 = x2-SCROLL_BAR_WIDTH+1;
    scroll_y1 = y1;
    scroll_x2 = x2;
    scroll_y2 = y2;

    init_flag = 1;
    top_line=1;

    vga.d3_panel_down( text_x1, text_y1, text_x2, text_y2, 2, 0 );
}

//---------- End of function TxtBox::init -------//

//------- Begin of function TxtBox::disp -------//
//!
//! <char*> textPtr     = pointer to the text
//! <int>   refreshFlag = refresh flag
//!
void TxtBox::disp(char* textPtr, int refreshFlag) {
    err_when( !init_flag );

    text_ptr = textPtr;

    //------------ INFO_REPAINT ------------//

    if( refreshFlag == INFO_REPAINT ) {
	//-------- count the no. of lines of the text --------//

	int dispLines;                                // no. of lines can be displayed on the area
	int totalLines;                               // total no. of lines of the text

	font_std.count_line( text_x1+TEXT_MARGIN, text_y1+TEXT_MARGIN,
			     text_x2-TEXT_MARGIN, text_y2-TEXT_MARGIN,
			     textPtr, TEXT_LINE_SPACE, dispLines, totalLines );

	//-------- display scroll bar ------//

	scroll_bar.init( 1, scroll_x1, scroll_y1, scroll_x2, scroll_y2,
			 dispLines, dispLines, totalLines );

	scroll_bar.paint();
    }

    //--------- display text body ----------//

    user_interface.bar( text_x1+2, text_y1+2, text_x2-2, text_y2-2, V_WHITE );

    font_std.put_paragraph( text_x1+TEXT_MARGIN, text_y1+TEXT_MARGIN,
			    text_x2-TEXT_MARGIN, text_y2-TEXT_MARGIN,
			    textPtr, TEXT_LINE_SPACE, top_line );
}

//-------- Begin of function TxtBox::disp -------//

//------- Begin of function TxtBox::detect -------//
//!
//! Return : <int> 1 - detected
//!                0 - not detected
//!
int TxtBox::detect() {
    if( !init_flag )
	return 0;

    //----------- detect scroll bar --------------//

    int rc;

    if( (rc=scroll_bar.detect()) > 0 ) {
	top_line = rc;
	disp(text_ptr, INFO_UPDATE);
	return 1;
    }

    //------- if mouse pressed on text area ------//
    //-- can pressed on hyper-texted field or just fast scrolling --//

    if( !mouse.single_click( text_x1, text_y1, text_x2, text_y2 ) )
	return 0;

    //------ detect fast scrolling --------//
    /*
      if( mouse.click_y() < text_y1 + (text_y2-text_y1) / 2 )
      top_line = scroll_bar.page_up(1);   // page up, 1 = skip less one line, so when scrolling text, want the last line of previous page be the first line of current page, then pass 1 as skipLess
      else
      top_line = scroll_bar.page_down(1); // page down
    */
    disp(text_ptr, INFO_UPDATE);

    return 1;
}

//-------- Begin of function TxtBox::detect -------//
