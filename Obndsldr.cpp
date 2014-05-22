//Filename    : OBNDSLDR.CPP
//Description : Object BoundedSlider

#include <ALL.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OBNDSLDR.H>
#include <OIFACE.H>
#include <OAUDIO.H>
#include <OCONFIG.H>
#include <OINFO.H>                                //## chea 030999

//-------------- Define static vars ------------//

static char paint_flag = 0;
static double dummy_zero = 0.0;

//-------- Begin of function BoundedSlider::BoundedSlider --------//
//!
BoundedSlider::BoundedSlider() {
    bounded_slider_bitmap = NULL;
    init_flag = 0;
}

//---------- End of function BoundedSlider::BoundedSlider ----//

//-------- Begin of function BoundedSlider::~BoundedSlider ---//
//!
BoundedSlider::~BoundedSlider() {
    deinit();
}

//---------- End of function BoundedSlider::~BoundedSlider ---//

//-------- Begin of function BoundedSlider::init --------//
//!
//! <int>     x1, y1			= location of the bounded slider
//! <int>     barWidth		= width of the slider bar
//! <int>     barHeight		= height of the slider bar
//! <double*> varPtr			= pointer to the bounded slider variable
//! <double*> loPtr			= pointer to the lower-bound variable
//! <double*> upPtr			= pointer to the upper-bound variable
//! <double>  minVal			= minimum value of the variable
//! <double>  maxVal			= maximum value of the variable
//! [char]  transparentFlag	= transparent flag (default = 1)
//!									  1 - transparent, 0 - solid
//! [int]     barColor		= color of the bar (default = Vga::active_buf->color_up)
//! [int]		 sliderColor	= color of the slider (default = Vga::active_buf->color_dark)
//! [int]     dataFormat    = data format
void BoundedSlider::init( int x1, int y1, int barWidth, int barHeight,
			  double *varPtr, double *loPtr, double *upPtr,
			  double minVal, double maxVal, char transparentFlag,
			  int barColor, int sliderColor ,int dataFormat) {
    err_when(minVal >= maxVal || barWidth < 0 || barHeight < 0);

    bounded_slider_x1 = x1;
    bounded_slider_y1 = y1;
    bounded_slider_x2 = x1 + barWidth + 3;
    bounded_slider_y2 = y1 + barHeight + 2;

    bar_width = bounded_slider_x2 - bounded_slider_x1 - 8;

    slider_color = sliderColor;
    bar_color = barColor;
    data_fmt=dataFormat;
    result_flag=0;
    result_value=0;
    var_ptr = varPtr;
    if(loPtr==NULL) {
	lo_ptr = &dummy_zero;
	lower_bound_null_flag=1;
    }
    else {
	lo_ptr = loPtr;
	lower_bound_null_flag=0;
    }
    up_ptr = upPtr;
    min_val = minVal;
    max_val = maxVal;
    val_range = maxVal - minVal;

    transparent_flag = transparentFlag;
    enable_flag=1;

    lock_max = lock_min = 0;

    init_flag = 1;

    //	paint();
}

//------------- End of function BoundedSlider::init -----------//

//-------- Begin of function BoundedSlider::deinit ------------//
//!
void BoundedSlider::deinit() {
    if (bounded_slider_bitmap) {
	mem_del(bounded_slider_bitmap);
	bounded_slider_bitmap = NULL;
    }

    init_flag = 0;
}

//------------- End of function BoundedSlider::deinit ---------//

//----------- Begin of function BoundedSlider::paint ----------//
//!
void BoundedSlider::paint() {
    bounded_slider_bitmap = vga_back.save_area(bounded_slider_x1-15, bounded_slider_y1-6, bounded_slider_x2+20, bounded_slider_y2+6, bounded_slider_bitmap);

    paint_flag = 1;

    refresh();
}

//------------- End of funcion BoundedSlider::paint -----------//

//----------- Begin of function BoundedSlider::refresh --------//
//!
void BoundedSlider::refresh() {
    //	err_when(*var_ptr > max_val);
    //	err_when(*var_ptr < min_val);
    //	err_when(val_range <= 0.0);

    short barWidth1 = 0;
    short barWidth2 = 0;
    short slidePos = 0;
    short resSlidePos = 0;

    if(val_range>1e-5) {
	barWidth1 = short(double(bar_width) * (*lo_ptr - min_val) / val_range);
	barWidth2 = short(double(bar_width) * (*up_ptr - min_val) / val_range);
	slidePos = short(double(bar_width) * (*var_ptr - min_val) / val_range);
	resSlidePos = short(double(bar_width) * (result_value - min_val) / val_range);
    }

    if (barWidth1 < 0)
	barWidth1 = 0;
    else if (barWidth1 > barWidth2)
	barWidth1 = barWidth2;

    if (barWidth2 > bar_width)
	barWidth2 = bar_width;
    else if (barWidth2 < barWidth1)
	barWidth2 = barWidth1;

    //	err_when(slidePos > barWidth2 || slidePos < barWidth1);

    if(slidePos > barWidth2)slidePos=barWidth2;
    if(slidePos < barWidth1)slidePos=barWidth1;
    //--------- draw the slider ----------//

    vga_back.rest_area(bounded_slider_bitmap, 0, 0);

    if (transparent_flag) {
	user_interface.brighten(bounded_slider_x1+3, bounded_slider_y1+3, bounded_slider_x2-2, bounded_slider_y2-2);
	user_interface.rect(bounded_slider_x1, bounded_slider_y1, bounded_slider_x2, bounded_slider_y2, 2);
    }
    else {
	user_interface.bar(bounded_slider_x1, bounded_slider_y1, bounded_slider_x2, bounded_slider_y2);
	user_interface.panel(bounded_slider_x1+2, bounded_slider_y1+2, bounded_slider_x2-2, bounded_slider_y2-2);
    }

    //----- paint the area inside the bound -----//

    if (barWidth2 > barWidth1) {
	user_interface.bar(bounded_slider_x1+4+barWidth1, bounded_slider_y1+5,
			   bounded_slider_x1+4+barWidth2, bounded_slider_y2-3, bar_color);
    }

    //-------- paint the slider pointer --------//
    int icolor;
    if(highlight_indicator_flag==INDICATOR)
	icolor=V_GREEN;
    else
	icolor=vga_back.translate_color(slider_color);

    vga_back.line(bounded_slider_x1+slidePos+3, bounded_slider_y1-2,
		  bounded_slider_x1+slidePos+3, bounded_slider_y2+4, icolor);
    vga_back.line(bounded_slider_x1+slidePos+3, bounded_slider_y1-2,
		  bounded_slider_x1+slidePos+5, bounded_slider_y1-2, icolor);
    vga_back.line(bounded_slider_x1+slidePos+3, bounded_slider_y2+4,
		  bounded_slider_x1+slidePos+5, bounded_slider_y2+4, icolor);
    vga_back.line(bounded_slider_x1+slidePos+5, bounded_slider_y1-2,
		  bounded_slider_x1+slidePos+5, bounded_slider_y2+4, icolor);

    if (result_flag) {
	//## chea 030999 bar draw here
	//## chea 030999 decided where the ribbon should be draw or not
	int draw = 1;
	if(bounded_slider_x1 + 4 + resSlidePos > bounded_slider_x2)
	    draw = 0;
	if(bounded_slider_x1 + 4 + resSlidePos < bounded_slider_x1)
	    draw = 0;

	if(draw ==1) {
	    user_interface.v_line(
		bounded_slider_x1 + 4 + resSlidePos,
		bounded_slider_y1 - 2,
		bounded_slider_y2 - bounded_slider_y1 + 6,
		1, V_RED,V_RED);                          //## chea 030999 bar draw here
	}
    }

    if(highlight_indicator_flag==UPPER) {
	user_interface.bar(
	    bounded_slider_x1+barWidth2-1,
	    bounded_slider_y1+5,
	    bounded_slider_x1+4+barWidth2,
	    bounded_slider_y2-3,
	    V_GREEN);
    }
    else if(highlight_indicator_flag==LOWER) {
	user_interface.bar(
	    bounded_slider_x1+4+barWidth1,
	    bounded_slider_y1+5,
	    bounded_slider_x1+barWidth1+9,
	    bounded_slider_y2-3,
	    V_GREEN);
    }

    if (enable_flag==0)
	user_interface.darken(bounded_slider_x1, bounded_slider_y1, bounded_slider_x2+3, bounded_slider_y2+2);

    //	font_small_san.put(bounded_slider_x1-4, bounded_slider_y1-8,m.format(min_val,data_fmt));
    //	font_small_san.put(bounded_slider_x2-12, bounded_slider_y1-8,m.format(max_val,data_fmt));
    //	font_small_san.put(bounded_slider_x1+slidePos+7, bounded_slider_y1-6, m.format(*var_ptr,data_fmt));

    font_news.put(bounded_slider_x1-4, bounded_slider_y1-8,m.format(min_val,data_fmt));
    font_news.put(bounded_slider_x2-12, bounded_slider_y1-8,m.format(max_val,data_fmt));

    font_blue_san.put(bounded_slider_x1+slidePos-7,
		      (bounded_slider_y1+bounded_slider_y2)/2-6,
		      m.format(*var_ptr,data_fmt));

    if (!paint_flag)
	vga.blt_buf(bounded_slider_x1-15, bounded_slider_y1-6, bounded_slider_x2+20, bounded_slider_y2+6);
    else
	paint_flag = 0;
}

//------------- End of function BoundedSlider::refresh ---------//

//----------- Begin of function BoundedSlider::detect ----------//
//!
//! Return: 1-pressed on slider, variables updated
//!			  0-not pressed on slider
//!
int BoundedSlider::detect() {

    if (!init_flag)
	return 0;

    if (!enable_flag)
	return 0;

    //	Notes:	cursor update is temporary disabled since cursor
    //				resources are currently not available
    //				curosr update code not tested
    // Norris 0716

    short sliderPos, loPos, upPos;

    /*
      static int old_cursor;
      static char cursor_flag = 0;

      //---- Cursor update -----//

      char posDetected = 0;

      if (mouse.in_area(bounded_slider_x1-2, bounded_slider_y1-3, bounded_slider_x2+2, bounded_slider_y2+2))
      {
      sliderPos = bounded_slider_x1 + int(double(bar_width) * (*var_ptr - min_val) / val_range) - 2;
      loPos = bounded_slider_x1 + int(double(bar_width) * (*lo_ptr - min_val) / val_range) - 2;
      upPos = bounded_slider_x1 + int(double(bar_width) * (*up_ptr - min_val) / val_range) - 2;
      posDetected = 1;

      if (!cursor_flag)
      {
      if (  mouse.in_area(sliderPos, bounded_slider_y1-3, sliderPos+4, bounded_slider_y2+2)
      || mouse.in_area(loPos, bounded_slider_y1-5, loPos+4, bounded_slider_y2+2)
      || mouse.in_area(upPos, bounded_slider_y1-5, upPos+4, bounded_slider_y2+2))
      {
      old_cursor = mouse_cursor.get_icon();
      mouse_cursor.set_icon(CURSOR_UNIT);			// temporary cursor
      cursor_flag = 1;
      }
      }
      else
      {
      if (  !mouse.in_area(sliderPos, bounded_slider_y1-3, sliderPos+4, bounded_slider_y2+2)
      && !mouse.in_area(loPos, bounded_slider_y1-5, loPos+4, bounded_slider_y2+2)
      && !mouse.in_area(upPos, bounded_slider_y1-5, upPos+4, bounded_slider_y2+2))
      {
      mouse_cursor.set_icon(old_cursor);
      cursor_flag = 0;
      }
      }
      }
      else if (cursor_flag)
      {
      mouse_cursor.set_icon(old_cursor);
      cursor_flag = 0;
      }
    */

    //---- Mouse clicked on the slider----//

    //	if (!mouse.press_area(bounded_slider_x1, bounded_slider_y1-2, bounded_slider_x2, bounded_slider_y2+2, 0))
    //		return 0;

    //	if (!posDetected)
    //	{
    sliderPos = bounded_slider_x1 + short(double(bar_width) * (*var_ptr - min_val) / val_range);
    loPos = bounded_slider_x1 + short(double(bar_width) * (*lo_ptr - min_val) / val_range);
    upPos = bounded_slider_x1 + short(double(bar_width) * (*up_ptr - min_val) / val_range);
    //	}

    //---- Variable update? ----//

#define SENSITIVE_RANGE 6

    if (mouse.press_area(sliderPos-SENSITIVE_RANGE, bounded_slider_y1-2, sliderPos+SENSITIVE_RANGE, bounded_slider_y2+6, 0)) {
	double lastVar = *var_ptr;
	while (mouse.left_press) {
	    sys.yield();
	    mouse.get_event();
	    sys.blt_virtual_buf();

	    *var_ptr = double(mouse.cur_x-bounded_slider_x1-4) / double(bar_width) * val_range + min_val;

	    if (upPos - loPos < 6) {
		if (*var_ptr < min_val)
		    *var_ptr = min_val;
		else if (*var_ptr > max_val)
		    *var_ptr = max_val;

		if( lock_min ) {
		    if (*lo_ptr > *var_ptr)
			*var_ptr = *lo_ptr;                   // *lo_ptr can't change when locked
		}
		if( lock_max ) {
		    if (*up_ptr < *var_ptr)
			*var_ptr = *up_ptr;                   // *up_ptr can't change when locked
		}

		if (*lo_ptr > *var_ptr)
		    *lo_ptr = *var_ptr;
		if (*up_ptr < *var_ptr)
		    *up_ptr = *var_ptr;
		if (*up_ptr < *lo_ptr) {
		    *up_ptr = *lo_ptr = *var_ptr;
		    lastVar = *var_ptr+1;
		}
	    }
	    else {
		if (*var_ptr < *lo_ptr)
		    *var_ptr = *lo_ptr;
		else if (*var_ptr > *up_ptr)
		    *var_ptr = *up_ptr;

		if (*up_ptr < *lo_ptr) {
		    *up_ptr = *lo_ptr = *var_ptr;
		    lastVar = *var_ptr+1;
		}
	    }

	    if (lastVar != *var_ptr)
		refresh();
	}
	return 1;
    }

    if(highlight_indicator_flag!=INDICATOR)
	if(mouse.in_area(sliderPos-SENSITIVE_RANGE, bounded_slider_y1-2, sliderPos+SENSITIVE_RANGE, bounded_slider_y2+6)) {
	    highlight_indicator_flag=INDICATOR;
	    refresh();
	    return 0;
	}
    //---- Lower bound update? ----//
    if(lower_bound_null_flag==0) {
	if ( !lock_min )
	    if (mouse.press_area(loPos-SENSITIVE_RANGE, bounded_slider_y1-2, loPos+SENSITIVE_RANGE, bounded_slider_y2+2, 0)) {
		double lastVar = *lo_ptr;
		while (mouse.left_press) {
		    sys.yield();
		    mouse.get_event();
		    sys.blt_virtual_buf();

		    *lo_ptr = double(mouse.cur_x-bounded_slider_x1-4) / double(bar_width) * val_range + min_val;

		    if (*lo_ptr < min_val)
			*lo_ptr = min_val;
		    else if (*lo_ptr > *var_ptr)
			*lo_ptr = *var_ptr;

		    if (*up_ptr < *lo_ptr) {
			*up_ptr = *lo_ptr = *var_ptr;
			lastVar = *lo_ptr+1;
		    }

		    if (lastVar != *lo_ptr)
			refresh();
		}
		return 1;
	    }

	if(highlight_indicator_flag!=LOWER)
	    if(mouse.in_area(loPos-SENSITIVE_RANGE, bounded_slider_y1-2, loPos+SENSITIVE_RANGE, bounded_slider_y2+2)) {
		highlight_indicator_flag=LOWER;
		refresh();
		return 0;
	    }
    }
    //---- Upper bound update? ----//

    if (mouse.press_area(upPos-SENSITIVE_RANGE, bounded_slider_y1-2, upPos+SENSITIVE_RANGE, bounded_slider_y2+2, 0)) {
	if( !lock_max ) {
	    double lastVar = *up_ptr;
	    while (mouse.left_press) {
		sys.yield();
		mouse.get_event();
		sys.blt_virtual_buf();

		*up_ptr = double(mouse.cur_x-bounded_slider_x1-4) / double(bar_width) * val_range + min_val;

		if (*up_ptr < *var_ptr)
		    *up_ptr = *var_ptr;
		else if (*up_ptr > max_val)
		    *up_ptr = max_val;

		if (*up_ptr < *lo_ptr) {
		    *up_ptr = *lo_ptr = *var_ptr;
		    lastVar = *up_ptr+1;
		}

		if (lastVar != *up_ptr)
		    refresh();
	    }
	}
	return 1;
    }

    if(highlight_indicator_flag!=UPPER)
	if (mouse.in_area(upPos-SENSITIVE_RANGE, bounded_slider_y1-2, upPos+SENSITIVE_RANGE, bounded_slider_y2+2)) {
	    highlight_indicator_flag=UPPER;
	    refresh();
	    return 0;
	}
    highlight_indicator_flag=NONE;
    return 0;
}

//------------- End of function BoundedSlider::detect ----------//
