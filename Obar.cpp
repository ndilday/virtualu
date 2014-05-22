// OBar.cpp: implementation of the Bar class.
//
//////////////////////////////////////////////////////////////////////
// kevin 0718

#include <ALL.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OIFACE.H>
#include <OMOUSE.H>
#include <ORADIO.H>
#include "OBar.h"

//-------------- Define static vars ------------//

static char paint_flag = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Bar::Bar() {

}

//------------- End of function Bar::Bar ------------//

Bar::~Bar() {
    deinit();
}

//------------- End of function Bar::~Bar ------------//

//-------- Begin of function Bar::init --------//
//!
//! <int>     x1, y1        = upper left corner of the bar
//! <int>		 x2, y2			= lower right corner of the bar
//! <int>		 barColor	= color of buttons (default = vga_back.color_border)
//!

void Bar::init( int x1, int y1, int x2, int y2, int barColor) {
    font_ptr = &font_chartsm;
    bar_x1 = x1;
    bar_y1 = (y1 + y2 - font_ptr->font_height) / 2;
    bar_x2 = x2;
    bar_y2 = (y1 + y2 + font_ptr->font_height) / 2;
    bar_width =  x2 - x1;
    bar_height = y2 - y1;
    bar_color=barColor;
    bar_bitmap=NULL;

    init_flag = 1;
}

//------------- End of function Bar::init ------------//

///////////////////////////////////////////////////////////
//
void Bar::deinit() {

    if (init_flag)
	if (bar_bitmap) {
	    mem_del(bar_bitmap);
	    bar_bitmap = NULL;
	}
    init_flag = 0;
}

//------------- End of function Bar::deinit ------------//

///////////////////////////////////////////////////////////
//
void Bar::paint(void) {
    // save area
    bar_bitmap=vga_back.save_area(bar_x1,bar_y1,bar_x2,bar_y2+2,bar_bitmap);
    // refresh
    paint_flag = 1;
    refresh();
}

//------------- End of function Bar::paint ------------//

///////////////////////////////////////////////////////////
//
void Bar::refresh(void) {
    vga_back.rest_area(bar_bitmap, 0, 0);
    user_interface.bar(bar_x1,bar_y1,bar_x2,bar_y2);
    if (!paint_flag)
	vga.blt_buf(bar_x1, bar_y1, bar_x2, bar_y2+2);
    else
	paint_flag = 0;
}

//------------- End of function Bar::refresh ------------//

///////////////////////////////////////////////////////////
//
// Return: 1-pressed on button, variable updated
//			  0-not pressed on button
//
int Bar::detect() {
    if (!init_flag)
	return 0;

    //---- Variable update? ----//

    if (mouse.single_click( bar_x1, bar_y1, bar_x2, bar_y2 )) {
	return 1;
    }

    return 0;
}

//------------- End of function Bar::detect ------------//
