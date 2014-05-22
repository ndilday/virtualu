//Filename    : OVALSLDR.CPP
//Description : Object ValueSlider

#include <ALL.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OVALSLDR.H>
#include <OMISC.H>
#include <OIFACE.H>

//-------------- Define static vars ------------//

static char paint_flag = 0;

//-------- Begin of function ValueSlider::ValueSlider ----//
//!
ValueSlider::ValueSlider() {
    value_slider_bitmap = NULL;
    init_flag = 0;
    target_flat = 0;
}

//---------- End of function ValueSlider::ValueSlider ----//

//-------- Begin of function ValueSlider::~ValueSlider ---//
//!
ValueSlider::~ValueSlider() {
    deinit();
}

//---------- End of function ValueSlider::~ValueSlider ---//

//-------- Begin of function ValueSlider::init --------//
//!
//! <int>     x1, y1        = upper left corner of the value slider
//!									  (coordinate includes the tabs)
//! <int>		 barWidth		= width of slider bar
//! <int>		 barHeight		= height of slider bar
//! <int>		 minVal			= minimum value
//! <int>		 maxVal			= maximum value
//! <int*>	 varPtr			= pointer to variable
//! [int]	 transparentFlag	= transparent flag (default = 1)
//!									  1 - transparent, 0 - solid
//! [int]		 barColor		= color of the slider bar (default = Vga::active_buf->color_up)
//! [int]		 formatType		= value display format (default = 1)
//!									  1 - 1,000,000  add thousand seperator
//!									  2 - $1,000,000 add thousand seperator and dollar sign
//!									  3 - 56% add percentage sign % at the end of the number
//!			                    4 - 1000000, no thousand seperator
//!
void ValueSlider::init( int x1, int y1, int barWidth, int barHeight, int minVal, int maxVal,
			int* varPtr, int transparentFlag, int barColor, int formatType ) {
    err_when(minVal > maxVal || barWidth < 0 || barHeight < 0);

    value_slider_x1 = x1;
    value_slider_y1 = y1;
    value_slider_x2 = x1 + barWidth + 3;
    value_slider_y2 = y1 + barHeight + 2;

    bar_width = value_slider_x2 - value_slider_x1 - 6;

    min_val = minVal;
    max_val = maxVal;
    var_ptr = varPtr;

    if(*varPtr>maxVal)max_val=*varPtr;
    if(*varPtr<minVal)min_val=*varPtr;

    bar_color = barColor;
    format_type = formatType;
    transparent_flag = transparentFlag;

    font_ptr = &font_barcharts;

    init_flag = 1;

    paint();
}

//------------- End of function ValueSlider::init -----------//

//## chea 060999 begin for adding a target bar on the value slider
//-------- Begin of function init_target::init --------//
//!
//! <int>     x_pos         = x & y pos. of the target bar
//! <int>		 height		   = height of the target bar
//! <int>		 width		   = width of the target bar
//! <int>		 borderColor	= border color the target bar
//! <int>		 shadowColor	= shadow color the target bar
//!

void ValueSlider::init_target( int x1, int y1, int height, int width, int borderColor, int shadowColor) {

    tar_x_pos = x1;
    tar_y_pos = y1;
    tar_height = height;
    tar_width = width;
    tar_borderColor = borderColor;
    tar_shadowColor = shadowColor;

    target_flat = 1;
    // ##### begin Gilbert 23/04/2001 ##### //
    // user_interface.v_line(tar_x_pos,tar_y_pos,tar_height,tar_width,tar_borderColor,tar_shadowColor,&vga_front); //## chea 030999 bar draw here
    user_interface.v_line(tar_x_pos,tar_y_pos,tar_height,tar_width,tar_borderColor,tar_shadowColor,Vga::active_buf);
    // ##### end Gilbert 23/04/2001 ##### //
}

//------------- End of function ValueSlider::init_target -----------//

//-------- Begin of function ValueSlider::set_target --------//
//!
//! <int>     x_pos         = x & y pos. of the target bar
//! <int>		 height		   = height of the target bar
//! <int>		 width		   = width of the target bar
//! <int>		 borderColor	= border color the target bar
//! <int>		 shadowColor	= shadow color the target bar
//!

void ValueSlider::set_target( int x1, int y1, int height, int width, int borderColor, int shadowColor) {

    if(target_flat == 0)
	return;

    tar_x_pos = x1;
    tar_y_pos = y1;
    tar_height = height;
    tar_width = width;
    tar_borderColor = borderColor;
    tar_shadowColor = shadowColor;

}

//------------- End of function ValueSlider::set_target -----------//

//-------- Begin of function ValueSlider::deinit ------------//
//!
void ValueSlider::deinit() {
    if (init_flag) {
	if (value_slider_bitmap) {
	    mem_del(value_slider_bitmap);
	    value_slider_bitmap = NULL;
	}
    }

    init_flag = 0;
    target_flat = 0;

}

//------------- End of function ValueSlider::deinit ---------//

//----------- Begin of function ValueSlider::paint ----------//
//!
void ValueSlider::paint() {
    if (transparent_flag) {
	user_interface.brighten(value_slider_x1+3, value_slider_y1+3, value_slider_x2-2, value_slider_y2-2);
	value_slider_bitmap = vga_back.save_area(value_slider_x1+3, value_slider_y1+3, value_slider_x2-2, value_slider_y2-2, value_slider_bitmap);
    }

    paint_flag = 1;
    //refresh();
}

//------------- End of function ValueSlider::paint ----------//

//----------- Begin of function ValueSlider::refresh --------//
//!
void ValueSlider::refresh() {
    if(!init_flag)
	return;

    err_when(*var_ptr > max_val || *var_ptr < min_val);

    //	vga.use_back();

    //	mouse.hide_area( value_slider_x1, value_slider_y1,
    //						  value_slider_x2, value_slider_y2 );

    if(max_val==min_val)return;

    short x = (value_slider_x1 + 1) + bar_width
	* (*var_ptr - min_val) / (max_val - min_val);

    //----- Draw the slider -----//

    if (transparent_flag) {
	vga_back.rest_area(value_slider_bitmap, 0, 0);
	user_interface.rect(value_slider_x1, value_slider_y1, value_slider_x2+3, value_slider_y2, 2);
    }
    else {
	user_interface.bar(value_slider_x1, value_slider_y1, value_slider_x2+1, value_slider_y2);
	user_interface.panel(value_slider_x1+2, value_slider_y1+2, value_slider_x2-2, value_slider_y2-2);
    }

    //----- Draw the bar area -----//

    if (x >= value_slider_x1 + 3)
	user_interface.bar( value_slider_x1 + 4, value_slider_y1 + 4,
			    x + 1, value_slider_y2 - 3, bar_color );
    //		user_interface.bar( value_slider_x1 + 3, value_slider_y1 + 4,
    //								  x, value_slider_y2 - 4, bar_color );

    //----- Draw the value in text -----//

    // ##### begin Gilbert 07/09/2001 ##### //
    if( format_type > 0 )                           // can suppress display
	// ##### end Gilbert 07/09/2001 ##### //
	font_ptr->center_put(value_slider_x1, value_slider_y1, value_slider_x2, value_slider_y2, m.format(*var_ptr, format_type));

    // ##### begin Gilbert 23/04/2001 ##### //
    if(target_flat==1)
	user_interface.v_line(tar_x_pos,tar_y_pos,tar_height,tar_width,tar_borderColor,tar_shadowColor,Vga::active_buf);
    // ##### end Gilbert 23/04/2001 ##### //

    //	mouse.show_area();

    //	vga.use_front();
    if (!paint_flag)
	vga.blt_buf(value_slider_x1, value_slider_y1, value_slider_x2+2, value_slider_y2+2);
    else
	paint_flag = 0;

    // ##### begin Gilbert 23/04/2001 ##### //
    //## chea 060999 for adding a target bar on the value slider
    //if(target_flat==1)
    //user_interface.v_line(tar_x_pos,tar_y_pos,tar_height,tar_width,tar_borderColor,tar_shadowColor,&vga_front); //## chea 030999 bar draw here
    // ##### end Gilbert 23/04/2001 ##### //
}

//------------- End of function ValueSlider::refresh ---------//

//----------- Begin of function ValueSlider::detect ------------//
//!
//! Return: 1-pressed on button, variable updated
//!			  0-not pressed on button
//!
int ValueSlider::detect() {
    if (!init_flag)
	return 0;

    if (!mouse.single_click( value_slider_x1, value_slider_y1,
			     value_slider_x2, value_slider_y2 ))
	return 0;

    int lastVar = *var_ptr;

    char refreshFlag;

    do {
	sys.yield();
	mouse.get_event();
	sys.blt_virtual_buf();

	refreshFlag = 0;

	*var_ptr = (mouse.cur_x - value_slider_x1 - 1)
	    * (max_val - min_val) / bar_width + min_val;

	if( *var_ptr < min_val ) {
	    *var_ptr = min_val;
	    refresh();
	    refreshFlag = 1;
	}

	if( *var_ptr > max_val ) {
	    *var_ptr = max_val;
	    refresh();
	    refreshFlag = 1;
	}

	if( lastVar != *var_ptr && !refreshFlag )
	    refresh();
    }
    while( mouse.left_press );

    return 1;
}

//------------- End of function ValueSlider::detect ------------//

//.........................................................//

//-------- Begin of function ValueSliderGroup::ValueSliderGroup -------//

ValueSliderGroup::ValueSliderGroup(int valueSliderNum) {
    value_slider_num = valueSliderNum;
    value_slider_array = new ValueSlider[valueSliderNum];
}

//---------- End of function ValueSliderGroup::ValueSliderGroup -------//

//--------- Begin of function ValueSliderGroup::paint -----------------//
//!
void ValueSliderGroup::paint() {
    for (int i = 0; i < value_slider_num; i++)
	value_slider_array[i].paint();
}

//----------- End of function ValueSliderGroup::paint ----------------//

//--------- Begin of function ValueSliderGroup::detect ---------------//
//!
//! Return : <int> (>= 0) - the id of the slider detected
//!                   -1  - not detected
//!
int ValueSliderGroup::detect() {
    for (int i = 0; i < value_slider_num; i++) {
	if (value_slider_array[i].detect())
	    return i;
    }
    return -1;
}

//----------- End of function ValueSliderGroup::detect ---------------//
