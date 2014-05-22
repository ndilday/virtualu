//Filename	  : OPOWER.CPP
//Description : Object Power

#include <OIMGRES.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OPOWER.H>
#include <OFIRM.H>
#include <OSYS.H>
#include <OWORLD.H>

//----------- Begin of function Power::Power -----------//
Power::Power() {
    memset( this, 0, sizeof(Power) );
}

//----------- End of function Power::Power -----------//

//----------- Begin of function Power::~Power -----------//
Power::~Power() {
}

//----------- End of function Power::~Power -----------//

//----------- Begin of function Power::init -----------//

void Power::init() {
    memset( this, 0, sizeof(Power) );

    reset_selection();                              // there may be selections left by the previous game.
}

//----------- End of function Power::init -----------//

//----------- Begin of function Power::deinit -----------//

void Power::deinit() {
}

//----------- End of function Power::deinit -----------//

//-------- Begin of function Power::mouse_handler --------//
//! React immediately when the mouse is moved or clicked.
//!
void Power::mouse_handler() {
    if( sys.view_mode != MODE_NORMAL ) {
	mouse_cursor.set_icon(CURSOR_NORMAL);
	return;
    }
}

//--------- End of function Power::mouse_handler ---------//

//--------- Begin of function Power::reset_selection ---------//
//!
//! Reset selection.
//!
void Power::reset_selection() {
    firm_array.select_firm(0);
}

//----------- End of function Power::reset_selection -----------//
