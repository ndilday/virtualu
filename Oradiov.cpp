//Filename    : ORADIOV.CPP
//Description : Object RadioButton
//Owner		  : Kevin(ho)
#include <ALL.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <ORADIOV.H>
#include <OAUDIO.H>
#include <OCONFIG.H>
//--------- Define macro constant -----------//

enum {  BUTTON_DESC_DISTANCE = 8  };
enum {  BUTTON_RADIUS = 6       };

//-------------- Define static vars ------------//

static char paint_flag = 0;

//-------- Begin of function RadioButton::RadioButton ----//
//!
RadioButtonVert::RadioButtonVert() {
}

//---------- End of function RadioButton::RadioButton ----//

//-------- Begin of function RadioButton::~RadioButton ---//
//!
RadioButtonVert::~RadioButtonVert() {
    deinit();
}

//---------- End of function RadioButton::~RadioButton ---//

//-------- Begin of function RadioButton::init --------//
//!
//! <int>     x1, y1        = upper left corner of the radio button
//! <int>		 x2, y2			= lower right corner of the radio button
//! <int>		 optionNum		= number of options
//! <int*>	 selNum			= ptr to selection number
//! <char**>	 optionDesc		= description of the options
//! [int]		 buttonColor	= color of buttons (default = vga_back.color_border)
//!
void RadioButtonVert::init( int x1, int y1, int x2, int y2,
			    int optionNum, int *selNum, char **optionDesc, int buttonColor ) {
    err_when(optionNum <= 0);

    font_ptr = &font_chartsm;

    radio_button_x1 = x1;
    radio_button_y1 = y1;
    radio_button_x2 = x2;
    radio_button_y2 = y2;
    //	radio_button_y1 = (y1 + y2 - font_ptr->font_height) / 2;
    //	radio_button_y2 = (y1 + y2 + font_ptr->font_height) / 2;
    radio_button_width = 4*BUTTON_RADIUS;
    paragraph_width = (x2 - x1)-radio_button_width;
    paragraph_height = (y2 - y1)/optionNum;
    option_num = optionNum;
    sel_num = selNum;
    option_desc = optionDesc;
    button_color = buttonColor;

    radio_button_x = x1+2*BUTTON_RADIUS;
    radio_button_bitmap = NULL;

    init_flag = 1;

    paint();
}

//------------- End of function RadioButton::init -----------//

//-------- Begin of function RadioButton::deinit ------------//
//!
void RadioButtonVert::deinit() {
    if (init_flag) {
	if (radio_button_bitmap) {
	    mem_del(radio_button_bitmap);
	    radio_button_bitmap = NULL;
	}

	init_flag = 0;
    }
}

//------------- End of function RadioButton::deinit ---------//

//----------- Begin of function RadioButton::paint ----------//
//!
void RadioButtonVert::paint() {
    err_when(*sel_num > option_num || *sel_num < 0);

    //	vga.use_back();

    for (int i = 0; i < option_num; i++) {
	short y = radio_button_y1 + i * paragraph_height;

	font_ptr->put_paragraph(radio_button_x+BUTTON_DESC_DISTANCE,y,radio_button_x2, y+paragraph_height, option_desc[i] );
    }

    radio_button_bitmap = vga_back.save_area(radio_button_x1, radio_button_y1,
					     radio_button_x2, radio_button_y2, radio_button_bitmap);

    paint_flag = 1;
    refresh();
}

//------------- End of function RadioButton::paint ----------//

//----------- Begin of function RadioButton::refresh --------//
//!
void RadioButtonVert::refresh() {
    err_when(*sel_num > option_num || *sel_num < 0);

    vga_back.rest_area(radio_button_bitmap, 0, 0);

    //	vga.use_back();

    for (int i = 0; i < option_num; i++) {
	/*		short y = radio_button_y1 + i * paragraph_height;
			circle(	radio_button_x1+BUTTON_RADIUS, y+BUTTON_RADIUS,
			BUTTON_RADIUS, button_color, 0);
			if (i == *sel_num)
			circle(radio_button_x1+BUTTON_RADIUS, y+BUTTON_RADIUS,
			BUTTON_RADIUS-2,button_color, 1);
	*/
	short y = radio_button_y1 + i * paragraph_height;
	image_interface.put_back(radio_button_x1+BUTTON_RADIUS-7, y+BUTTON_RADIUS-6,"RRED");
	if (i == *sel_num)
	    image_interface.put_back(radio_button_x1+BUTTON_RADIUS-7, y+BUTTON_RADIUS-6,"RGOLD");
    }

    //	vga.use_front();
    if (!paint_flag)
	vga.blt_buf(radio_button_x1, radio_button_y1, radio_button_x2, radio_button_y2);
    else
	paint_flag = 0;
}

//------------- End of function RadioButton::refresh ---------//

//----------- Begin of function RadioButton::fill --------------//
//!
//! <short>	x, y		a coordinate inside the filling region
//! <int>		color		filling color
//!
void RadioButtonVert::fill(short x, short y, int color) {
    err_when(x > VGA_WIDTH || y > VGA_HEIGHT || x < 0 || y < 0);

    vga_back.draw_pixel( x, y, color );

    //----- recursively fill neighbouring pixels -----//

    if (x > 0 && *vga_back.buf_ptr(x-1, y) != vga_back.translate_color(color)) {
	fill(x-1, y, color);
    }

    if (x < VGA_WIDTH && *vga_back.buf_ptr(x+1, y) != vga_back.translate_color(color)) {
	fill(x+1, y, color);
    }

    if (y > 0 && *vga_back.buf_ptr(x, y-1) != vga_back.translate_color(color)) {
	fill(x, y-1, color);
    }

    if (y < VGA_HEIGHT && *vga_back.buf_ptr(x, y+1) != vga_back.translate_color(color)) {
	fill(x, y+1, color);
    }
}

//------------- End of function RadioButton::fill --------------//

//----------- Begin of function RadioButton::circle ------------//
//!
//! <short>	x, y			center of circle
//! <int>		radius		radius of circle
//!	<int>		color			color of circle
//! <char>	fill_flag	1-fill the circle, 0-not fill the circle
//!
void RadioButtonVert::circle(short x, short y, int radius, int color, char fill_flag) {
    int d = 3 - 2 * radius;
    int a = 0;
    int b = radius;

    while (a != b) {
	vga_back.draw_pixel( x+a, y+b, color );
	vga_back.draw_pixel( x-a, y+b, color );
	vga_back.draw_pixel( x+a, y-b, color );
	vga_back.draw_pixel( x-a, y-b, color );
	vga_back.draw_pixel( x+b, y+a, color );
	vga_back.draw_pixel( x-b, y+a, color );
	vga_back.draw_pixel( x+b, y-a, color );
	vga_back.draw_pixel( x-b, y-a, color );

	if (d < 0)
	    d += 4 * a + 6;
	else
	    d += 4 * (a - b--) + 10;

	++a;
    }
    vga_back.draw_pixel( x+a, y+a, color );
    vga_back.draw_pixel( x+a, y-a, color );
    vga_back.draw_pixel( x-a, y+a, color );
    vga_back.draw_pixel( x-a, y-a, color );

    if (fill_flag) {
	fill(x, y, color);
    }
}

//------------- End of function RadioButton::circle ------------//

//----------- Begin of function RadioButton::detect ------------//
//!
//! Return: 1-pressed on button, variable updated
//!			  0-not pressed on button
//!
int RadioButtonVert::detect() {
    if (!init_flag)
	return 0;

    //---- Variable update? ----//

    if (mouse.single_click( radio_button_x1, radio_button_y1,
			    radio_button_x2, radio_button_y2 )) {
	short y,x=radio_button_x;
	for (int i = 0; i < option_num; i++) {
	    y = radio_button_y1 + i * paragraph_height;
	    if (mouse.single_click( radio_button_x1, y, radio_button_x2 , y+paragraph_height )) {
		*sel_num = i;
		audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
		return 1;
	    }
	}
    }

    return 0;
}

//------------- End of function RadioButton::detect ------------//
