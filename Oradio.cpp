//Filename    : ORADIO.CPP
//Description : Object RadioButton

#include <ALL.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <ORADIO.H>
#include <OAUDIO.H>
#include <OCONFIG.H>
//--------- Define macro constant -----------//

enum {  BUTTON_DESC_DISTANCE = 8  };
enum {  BUTTON_RADIUS = 6       };

//-------------- Define static vars ------------//

static char paint_flag = 0;

//-------- Begin of function RadioButton::RadioButton ----//
//!
RadioButton::RadioButton() {
}

//---------- End of function RadioButton::RadioButton ----//

//-------- Begin of function RadioButton::~RadioButton ---//
//!
RadioButton::~RadioButton() {
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
void RadioButton::init( int x1, int y1, int x2, int y2,
			int optionNum, int *selNum, char **optionDesc, int buttonColor ) {
    err_when(optionNum <= 0);

    font_ptr = &font_chartsm;

    radio_button_x1 = x1;
    radio_button_y1 = (y1 + y2 - font_ptr->font_height) / 2;
    radio_button_x2 = x2;
    radio_button_y2 = (y1 + y2 + font_ptr->font_height) / 2;
    radio_button_width = (x2 - x1) / optionNum;
    radio_button_height = y2 - y1;
    option_num = optionNum;
    sel_num = selNum;
    option_desc = optionDesc;
    button_color = buttonColor;

    radio_button_y = (radio_button_y1 + radio_button_y2) / 2;

    desc_x = new short[optionNum];
    radio_button_bitmap = NULL;

    init_flag = 1;

    paint();
}

//------------- End of function RadioButton::init -----------//

//-------- Begin of function RadioButton::deinit ------------//
//!
void RadioButton::deinit() {
    if (init_flag) {
	delete [] desc_x;

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
void RadioButton::paint() {
    err_when(*sel_num > option_num || *sel_num < 0);

    //	vga.use_back();

    for (int i = 0; i < option_num; i++) {
	short x = radio_button_x1 + i * radio_button_width;

	desc_x[i] = font_ptr->put( x+2*BUTTON_RADIUS+BUTTON_DESC_DISTANCE,
				   radio_button_y-font_ptr->font_height/2, option_desc[i] );
    }

    radio_button_bitmap = vga_back.save_area(radio_button_x1, radio_button_y1,
					     radio_button_x2, radio_button_y2, radio_button_bitmap);

    paint_flag = 1;
    refresh();
}

//------------- End of function RadioButton::paint ----------//

//----------- Begin of function RadioButton::refresh --------//
//!
void RadioButton::refresh() {
    err_when(*sel_num > option_num || *sel_num < 0);

    vga_back.rest_area(radio_button_bitmap, 0, 0);

    //	vga.use_back();

    for (int i = 0; i < option_num; i++) {
	short x = radio_button_x1 + i * radio_button_width;
	image_interface.put_back(x+BUTTON_RADIUS-7, radio_button_y-10,"RRED");
	if (i == *sel_num)
	    image_interface.put_back(x+BUTTON_RADIUS-7, radio_button_y-10,"RGOLD");
	//		circle(x+BUTTON_RADIUS, radio_button_y, BUTTON_RADIUS, button_color, 0);
	//			circle(x+BUTTON_RADIUS, radio_button_y, BUTTON_RADIUS-2, button_color, 1);
    }

    //	vga.use_front();
    if (!paint_flag)
	vga.blt_buf(radio_button_x1, radio_button_y1, radio_button_x2, radio_button_y2,0);
    else
	paint_flag = 0;
}

//------------- End of function RadioButton::refresh ---------//

//----------- Begin of function RadioButton::fill --------------//
//!
//! <short>	x, y		a coordinate inside the filling region
//! <int>		color		filling color
//!
void RadioButton::fill(short x, short y, int color) {
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
void RadioButton::circle(short x, short y, int radius, int color, char fill_flag) {
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
int RadioButton::detect() {
    if (!init_flag)
	return 0;

    //---- Variable update? ----//

    if (mouse.single_click( radio_button_x1, radio_button_y1,
			    radio_button_x2, radio_button_y2 )) {
	short x, y = (radio_button_y1 + radio_button_y2) / 2;
	for (int i = 0; i < option_num; i++) {
	    x = radio_button_x1 + i * radio_button_width;
	    if (mouse.single_click( x, y-BUTTON_RADIUS, desc_x[i], y+BUTTON_RADIUS )) {
		*sel_num = i;
		audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
		return 1;
	    }
	}
    }

    return 0;
}

//------------- End of function RadioButton::detect ------------//
