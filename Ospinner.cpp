//Filename    : OSPINNER.CPP
//Description : Spinner class

#include <ALL.H>
#include <KEY.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OCONFIG.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OSPINNER.H>
#include <OIFACE.H>
#include <OIMGRES.H>
#include <OBUTTON.H>
#include <OMISC.H>
#include <OSTR.H>
#include <stdio.h>

//--------- define constant ---------//

enum {
    BUTTON_WIDTH = 12,
    GET_A_FIELD_OFFSET = 15,
    FIELD_WIDTH = 15
};

//-------------- Define static vars ------------//

//### begin zb 02/23/99
char Spinner::focus=0;
Spinner* Spinner::which=NULL;
//### end zb 02/23/99

//-------- Begin of function Spinner::Spinner ------//
//!
Spinner::Spinner() {
    button_up_bitmap = NULL;
    button_down_bitmap = NULL;
    button_up_pressed_bitmap = NULL;
    button_down_pressed_bitmap = NULL;
    button_up_disable_bitmap = NULL;
    button_down_disable_bitmap = NULL;
    field_bitmap = NULL;

    get_a_field = new char[FIELD_WIDTH+1];
    paint_flag = 0;
    // ##### begin Gilbert 01/09/2001 ######//
    pause_when_focus = 1;                           // modify pause_when_focus before init
    // ##### end Gilbert 01/09/2001 ######//
    // ##### begin Gilbert 06/03/2001 ######//
    original_game_speed = -1;
    // ##### end Gilbert 06/03/2001 ######//
    init_flag = 0;
}

//---------- End of function Spinner::Spinner ------//

//-------- Begin of function Spinner::~Spinner -----//
//!
Spinner::~Spinner() {
    deinit();
    // #### begin Gilbert 29/08/2001 //
    delete[] get_a_field;
    // #### end Gilbert 29/08/2001 //
}

//---------- End of function Spinner::~Spinner -----//

//-------- Begin of function Spinner::deinit -------//
//!
void Spinner::deinit() {
    // ##### begin Gilbert 01/09/2001 ######//
    if( pause_when_focus ) {
	// ##### end Gilbert 01/09/2001 ######//
	//### begin zb 02/23/99
	focus=0;
	which=NULL;
	// ##### begin Gilbert 06/03/2002 ######//
	if(sys.main_hwnd!=NULL&&config.frame_speed==0 && original_game_speed!=-1 )
	    // ##### end Gilbert 06/03/2002 ######//
	    sys.set_speed(original_game_speed);
	//### end zb 02/23/99
    }

    // ##### begin Gilbert 27/04/2001 #######//
    button_up.deinit();
    button_down.deinit();
    // ##### end Gilbert 27/04/2001 #######//
    if (button_up_bitmap) {
	mem_del(button_up_bitmap);
	mem_del(button_down_bitmap);
	mem_del(button_up_pressed_bitmap);
	mem_del(button_down_pressed_bitmap);
	mem_del(button_up_disable_bitmap);
	mem_del(button_down_disable_bitmap);
	button_up_bitmap = NULL;
	button_down_bitmap = NULL;
	button_up_pressed_bitmap = NULL;
	button_down_pressed_bitmap = NULL;
	button_up_disable_bitmap = NULL;
	button_down_disable_bitmap = NULL;
    }
    if (field_bitmap) {
	mem_del(field_bitmap);
	field_bitmap = NULL;
    }

    init_flag = 0;
}

//---------- End of function Spinner::deinit ----------//

//-------- Begin of function Spinner::init ------------//
//!
//! <int>		x1, y1				- coordinate of upper left corner
//! <int>		x2, y2				- coordinate of lower right corner
//! <float*>	varPtr				- ptr to spinner variable
//! <float>	minVal				- minimum value
//! <float>	maxVal				- maximum value
//! <float>	stepVal				- spinner step value
//!	[char]	transparentFlag	- 1-transparent, 0-filled with color
//!										  (default = 1)
//! [int]		fillColor			- fill color
//!										  (default = vga_back.color_up)
//! [int]		formatType			- field format type (valid values : 1-3)
//!										  (default = 1)
//!
void Spinner::init(int x1, int y1, int x2, int y2, float *varPtr, float minVal, float maxVal, float stepVal, char transparentFlag, int fillColor, int formatType) {
    //### begin zb 02/23/99
    focus=0;
    which=NULL;
    //### end zb 02/23/99

    err_when (x2 - x1 < BUTTON_WIDTH);

    spinner_x1 = x1;
    spinner_y1 = y1;
    spinner_x2 = x2;
    spinner_y2 = y2;
    var_ptr = varPtr;
    min_val = minVal;
    max_val = maxVal;
    step_val = stepVal;

    //---- BUGHERE, override all from 0.1 to 0.5 for Virtual U project only ---//

    if( step_val == 0.1f )
	step_val = 0.5f;

    //-------------------------------------------------//

    transparent_flag = transparentFlag;
    fill_color = fillColor;
    format_type = formatType;

    font_ptr = &font_charts;

    deinit();

    // ##### begin Gilbert 06/03/2001 ######//
    original_game_speed = -1;
    // ##### end Gilbert 06/03/2001 ######//
    init_flag = 1;

    cursor_flag = 0;
    input_flag = 0;

    if (transparent_flag) {
	button_up.create_bitmapW(spinner_x2-BUTTON_WIDTH-1, spinner_y1+5, spinner_x2-2, (spinner_y1+spinner_y2)/2, button_up_bitmap);
	button_down.create_bitmapW(spinner_x2-BUTTON_WIDTH-1, (spinner_y1+spinner_y2)/2+1, spinner_x2-2, spinner_y2-2, button_down_bitmap);
    }
    else {
	button_up.create_bitmapW(spinner_x2-BUTTON_WIDTH-1, spinner_y1+5, spinner_x2-2, (spinner_y1+spinner_y2)/2, NULL);
	button_down.create_bitmapW(spinner_x2-BUTTON_WIDTH-1, (spinner_y1+spinner_y2)/2+1, spinner_x2-2, spinner_y2-2, NULL);
    }

    image_interface.put_back(spinner_x2-BUTTON_WIDTH, spinner_y1+5, "SPIN_U_O" );
    image_interface.put_back(spinner_x2-BUTTON_WIDTH, (spinner_y1+spinner_y2)/2+1, "SPIN_D_O" );
    button_up_bitmap = vga_back.save_area(
	spinner_x2-BUTTON_WIDTH, spinner_y1+5,
	spinner_x2, spinner_y1+5+BUTTON_WIDTH, button_up_bitmap);
    button_down_bitmap = vga_back.save_area(
	spinner_x2-BUTTON_WIDTH, (spinner_y1+spinner_y2)/2+1,
	spinner_x2, (spinner_y1+spinner_y2)/2+1+BUTTON_WIDTH, button_down_bitmap);
    image_interface.put_back(spinner_x2-BUTTON_WIDTH, spinner_y1+5, "SPIN_U_F" );
    image_interface.put_back(spinner_x2-BUTTON_WIDTH, (spinner_y1+spinner_y2)/2+1, "SPIN_D_F" );
    button_up_pressed_bitmap = vga_back.save_area(
	spinner_x2-BUTTON_WIDTH, spinner_y1+5,
	spinner_x2, spinner_y1+5+BUTTON_WIDTH, button_up_pressed_bitmap);
    button_down_pressed_bitmap = vga_back.save_area(
	spinner_x2-BUTTON_WIDTH, (spinner_y1+spinner_y2)/2+1,
	spinner_x2, (spinner_y1+spinner_y2)/2+1+BUTTON_WIDTH, button_down_pressed_bitmap);
    image_interface.put_back(spinner_x2-BUTTON_WIDTH, spinner_y1+5, "SPIN_DIS" );
    image_interface.put_back(spinner_x2-BUTTON_WIDTH, (spinner_y1+spinner_y2)/2+1, "SPIN_DIS" );
    user_interface.darken(spinner_x2-BUTTON_WIDTH, spinner_y1+5,
			  spinner_x2, (spinner_y1+spinner_y2)/2+1+BUTTON_WIDTH  );
    button_up_disable_bitmap = vga_back.save_area(
	spinner_x2-BUTTON_WIDTH, spinner_y1+5,
	spinner_x2, spinner_y1+5+BUTTON_WIDTH, button_up_disable_bitmap);
    button_down_disable_bitmap = vga_back.save_area(
	spinner_x2-BUTTON_WIDTH, (spinner_y1+spinner_y2)/2+1,
	spinner_x2, (spinner_y1+spinner_y2)/2+1+BUTTON_WIDTH, button_down_disable_bitmap);

    get_a.init(spinner_x1+GET_A_FIELD_OFFSET, (spinner_y1+2+spinner_y2-font_ptr->font_height)/2, spinner_x2-GET_A_FIELD_OFFSET-BUTTON_WIDTH, get_a_field, FIELD_WIDTH, font_ptr, -1, 0);
    get_a.cursor_pos = 0;
    get_a.mark_cursor_pos = 0;

    if (transparent_flag)
	paint();
}

//---------- End of function Spinner::init ------------//

//-------- Begin of function Spinner::paint ------------//
//!
void Spinner::paint() {
    if(transparent_flag) {
	user_interface.brighten(spinner_x1+4, spinner_y1+4, spinner_x2-BUTTON_WIDTH-3, spinner_y2-2);
	field_bitmap = vga_back.save_area(spinner_x1+4, spinner_y1+4, spinner_x2-BUTTON_WIDTH-3, spinner_y2-2, field_bitmap);
    }
    paint_flag = 1;
    refresh();
}

//---------- End of function Spinner::paint ------------//

//-------- Begin of function Spinner::refresh ------------//
//!
void Spinner::refresh() {
    //	err_when(*var_ptr < min_val);  //## chea 300899 should handle below
    //	err_when(*var_ptr > max_val);

    if (*var_ptr >= max_val)
	button_up.body_ptr =  (void*) (button_up_disable_bitmap+4);
    else
	button_up.body_ptr =  (void*) (button_up_bitmap+4);
    if (*var_ptr <= min_val)
	button_down.body_ptr =(void*) (button_down_disable_bitmap+4);
    else
	button_down.body_ptr =(void*) (button_down_bitmap+4);

    if (transparent_flag) {
	user_interface.rect(spinner_x1, spinner_y1, spinner_x2+3, spinner_y2, 2);
	user_interface.paint(spinner_x2-BUTTON_WIDTH-2, spinner_y1+4, spinner_x2-3, spinner_y2-3);
	vga_back.rest_area(field_bitmap, 0, 0);
    }
    else {
	user_interface.rect(spinner_x1, spinner_y1, spinner_x2+3, spinner_y2);
	user_interface.panel(spinner_x1+3, spinner_y1+3, spinner_x2-BUTTON_WIDTH-3, spinner_y2-2, &vga_back);
    }

    button_up.paint();
    button_down.paint();

    if (!input_flag)
	strncpy(get_a_field, m.format(*var_ptr, format_type), FIELD_WIDTH);

    get_a.paint(cursor_flag);

    //	vga.use_front();
    if (!paint_flag)
	vga.blt_buf(spinner_x1, spinner_y1, spinner_x2+2, spinner_y2+2);
    else
	paint_flag = 0;
}

//---------- End of function Spinner::refresh ------------//

//-------- Begin of function Spinner::detect -------------//
//!
//! return value:		0-not detected, 1-detected
//!
//!### begin zb 02/23/99
int Spinner::detect() {
    if (!init_flag)
	return 0;

    //### begin zb 02/23/99
    char del=0;
    if(focus)
	if(which!=this)
	    return 0;
    //### end zb 02/23/99

    if (cursor_flag || mouse.in_area(spinner_x1, spinner_y1, spinner_x2, spinner_y2)) {
	switch (get_a.detect()) {
	case 0:
	    if (mouse.is_press_button_event()) {
		// Out focus
		cursor_flag = 0;
		//### begin zb 02/23/99
		del=1;
		focus=0;
		which=NULL;
		//### end zb 02/23/99
		// ##### begin Gilbert 01/09/2001 ######//
		if( pause_when_focus ) {
		    // ##### end Gilbert 01/09/2001 ######//
		    // ##### begin Gilbert 06/03/2002 ######//
		    if(config.frame_speed==0 && original_game_speed!=-1 )
			// ##### end Gilbert 06/03/2002 ######//
			sys.set_speed(original_game_speed);
		}

		if (input_flag) {
		    input_flag = 0;
		    get_a.cursor_pos = 0;
		    get_a.mark_cursor_pos = 0;

		    if(get_a_field[0]=='(') {
			sscanf(get_a_field+1, "%f", var_ptr);
			(*var_ptr)*=-1;
		    }
		    else
			sscanf(get_a_field, "%f", var_ptr);

		    if (*var_ptr >= max_val) {
			*var_ptr = max_val;
			button_up.disable();
			button_down.enable();
			button_up.body_ptr = (void*) (button_up_disable_bitmap+4);
			button_down.body_ptr = (void*) (button_up_bitmap+4);
		    }
		    else if (*var_ptr <= min_val) {
			*var_ptr = min_val;
			button_up.enable();
			button_down.disable();
			button_up.body_ptr = (void*) (button_up_bitmap+4);
			button_down.body_ptr = (void*) (button_up_disable_bitmap+4);
		    }
		    else {
			button_up.enable();
			button_down.enable();
			button_down.body_ptr = (void*) (button_down_bitmap+4);
			button_up.body_ptr = (void*) (button_up_bitmap+4);
		    }

		    return 1;
		}
	    }
	    break;

	case 1:
	    cursor_flag = 1;
	    //### begin zb 02/23/99
	    focus=1;
	    which=this;
	    //### end zb 02/23/99
	    // ##### begin Gilbert 01/09/2001 ######//
	    if( pause_when_focus ) {
		// ##### end Gilbert 01/09/2001 ######//
		// turn off speed
		if(cursor_flag) {
		    original_game_speed=config.frame_speed;
		    sys.set_speed(0);
		}
	    }
	    mouse.clear_event();
	    return 1;

	case '.':
	case '-':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case KEY_DEL:
	case KEY_BACK_SPACE:
	    if (cursor_flag) {
		input_flag = 1;
		mouse.clear_event();
		return 1;
	    }

	case KEY_RETURN:
	    //### begin zb 02/23/99
	    focus=0;
	    which=NULL;
	    cursor_flag = 0;

	    if (input_flag) {
		input_flag = 0;
		get_a.cursor_pos = 0;
		get_a.mark_cursor_pos = 0;

		if(get_a_field[0]=='(') {
		    sscanf(get_a_field+1, "%f", var_ptr);
		    (*var_ptr)*=-1;
		}
		else
		    sscanf(get_a_field, "%f", var_ptr);

		if (*var_ptr >= max_val) {
		    *var_ptr = max_val;
		    button_up.disable();
		    button_down.enable();
		    button_up.body_ptr = (void*) (button_up_disable_bitmap+4);
		    button_down.body_ptr = (void*) (button_up_bitmap+4);
		}
		else if (*var_ptr <= min_val) {
		    *var_ptr = min_val;
		    button_up.enable();
		    button_down.disable();
		    button_up.body_ptr = (void*) (button_up_bitmap+4);
		    button_down.body_ptr = (void*) (button_up_disable_bitmap+4);
		}
		else {
		    button_up.enable();
		    button_down.enable();
		    button_down.body_ptr = (void*) (button_down_bitmap+4);
		    button_up.body_ptr = (void*) (button_up_bitmap+4);
		}

		// ##### begin Gilbert 01/09/2001 ######//
		if( pause_when_focus ) {
		    // ##### end Gilbert 01/09/2001 ######//
		    // ##### begin Gilbert 06/03/2002 ######//
		    if(config.frame_speed==0 && original_game_speed!=-1)
			// ##### end Gilbert 06/03/2002 ######//
			sys.set_speed(original_game_speed);
		}
		mouse.clear_event();
		return 1;
	    }
	    //### end zb 02/23/99

	default:
	    if (cursor_flag) {
		if (mouse.is_key_event())
		    mouse.clear_event();
		return 1;
	    }
	}
    }

    //--------- detect Increment button ----------//

    if (button_up.detect() ) {
	button_up.body_ptr = (void*) (button_up_bitmap+4);
	*var_ptr += step_val;
	if (*var_ptr >= max_val) {
	    *var_ptr = max_val;
	    button_up.disable();
	    if (transparent_flag) {
		button_up.body_ptr = (void*) (button_up_disable_bitmap+4);
		//button_up.body_ptr = (void*) (image_interface.get_ptr("SPIN_DIS"));
	    }
	}
	if (*var_ptr > min_val) {
	    button_down.enable();
	    if (transparent_flag) {
		button_down.body_ptr = (void*) (button_down_bitmap+4);
		//button_down.body_ptr = (void*) (image_interface.get_ptr("SPIN_D_O"));
	    }
	}
	return 1;
    }

    //--------- detect Decrement button ----------//

    if (button_down.detect()) {
	button_down.body_ptr = (void*) (button_down_bitmap+4);
	*var_ptr -= step_val;
	if (*var_ptr <= min_val) {
	    *var_ptr = min_val;
	    button_down.disable();
	    if (transparent_flag) {
		button_down.body_ptr = (void*) (button_down_disable_bitmap+4);
		//button_down.body_ptr = (void*) (image_interface.get_ptr("SPIN_DIS"));
	    }
	}
	if (*var_ptr < max_val) {
	    button_up.enable();
	    if (transparent_flag) {
		button_up.body_ptr = (void*) (button_up_bitmap+4);
		//button_up.body_ptr = (void*) (image_interface.get_ptr("SPIN_U_O"));
	    }
	}
	return 1;
    }

    if(del)
	mouse.clear_event();

    return 0;
}

//### end zb 02/23/99
//---------- End of function Spinner::detect -------------//
