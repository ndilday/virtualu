//Filename    : OGCompon.cpp
//Description : GComponent Class Definition

// ES library header file
#include <OMouse.h>                               // for this.on_event()

// CU header file
#include "OGCompon.h"

//----------- Define constants -------------//
//----------- Define static variables -------------//
//----------- Define static functions -------------//
//----------- Define static class member variables -------------//

//----------- Begin of function GComponent Constructor -----//
//! desc
//! <int> -
//! [int] -
//!
GComponent::GComponent() {

}

//------------- End of function GComponent Constructor -----//

//----------- Begin of function GComponent Destructor ------//
//!
GComponent::~GComponent() {

}

//------------- End of function GComponent Destructor ------//

//----------- Begin of function GComponent::init --------//
//!
void GComponent::init() {
    is_shown = true;
    is_active = false;
}

//------------- End of function GComponent::init --------//

//----------- Begin of function GComponent::deinit --------//
//!
void GComponent::deinit() {
}

//------------- End of function GComponent::deinit --------//

//----------- Begin of function GComponent::init --------//
//!
void GComponent::set_bounds(int x1, int y1, int x2, int y2) {
    clip.x1 = x1;
    clip.y1 = y1;
    clip.x2 = x2;
    clip.y2 = y2;
}

//------------- End of function GComponent::init --------//

//----------- Begin of function GComponent::init --------//
void GComponent::repaint(VgaBuf *vgaBuf) {
    if ( is_shown )
	paint(vgaBuf);
}

//------------- End of function GComponent::init --------//

//----------- Begin of function GComponent::on_event ------//
//! called on mouse/keyboard event
//!
void GComponent::on_event() {
    if ( !is_shown )
	return;

    if( mouse.is_key_event() ) {
	on_key(mouse.scan_code, mouse.event_skey_state);
    }
    // 0: L button
    else if ( mouse.any_click(clip.x1, clip.y1, clip.x2, clip.y2, 0) ) {
	is_active = true;
	on_mouse_click(mouse.click_x(MOUSE_LEFT) - clip.x1,
		       mouse.click_y(MOUSE_LEFT) - clip.y1, MOUSE_LEFT);
    }
    // 1: R button
    else if ( mouse.any_click(clip.x1, clip.y1, clip.x2, clip.y2, 1) ) {
	is_active = true;
	on_mouse_click(mouse.click_x(MOUSE_RIGHT) - clip.x1,
		       mouse.click_y(MOUSE_RIGHT) - clip.y1, MOUSE_RIGHT);
    }
}

//------------- End of function GComponent::on_event ------//

//----------- Begin of function GComponent::get ------//
//! set this to active/inactive
//!
bool GComponent::is_inside(int x, int y) {
    return ( x >= clip.x1 && y >= clip.y1
	     && x <= clip.x2 && y <= clip.y2 );
}

//------------- End of function GComponent::get ------//
