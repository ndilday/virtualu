//Filename    : OLoc.cpp
//Description : Location Class Definition

// ES library header file
#include <OVgaBuf.h>

// CU header file
#include <ODefTile.h>                             // for Def::BIG
#include <OFirm.h>
#include <OFirmA.h>                               // for ::select_obj

#include "OLoc.h"

//temp
#include <OFirmRes.h>

//----------- Define constants -------------//
//----------- Define static variables -------------//
//----------- Define static functions -------------//
//----------- Define static class member variables -------------//
Firm  *Location::selected_object=0;

//----------- Begin of function Location Constructor -----//
//! desc
//! <int> -
//! [int] -
//!
Location::Location() {
    object = 0;
    //selected_object = 0;
    is_selected = false;
}

//------------- End of function Location Constructor -----//

//----------- Begin of function Location Destructor ------//
//!
Location::~Location() {

}

//------------- End of function Location Destructor ------//

//----------- Begin of function Location::init --------//
//!
void Location::init() {
}

//------------- End of function Location::init --------//

//----------- Begin of function Location::deinit --------//
//!
void Location::deinit() {
}

//------------- End of function Location::deinit --------//

//----------- Begin of function Location::get ------//
//! paint plant or firm, etc
//!
void  Location::paint_object(short tileType, int x, int y, Rect *clip, VgaBuf *vgaBuf) {
#ifdef DEBUG_SELECT_MAP
    if ( is_selected &&  tileType == 1 )
	Firm::firm_res.get_bitmap(DefTile::MARK_BMP)->paint(tileType, x+4,y+2,clip,vgaBuf);
#endif

    if ( object )
	object->paint(tileType, x,y,clip,vgaBuf);
}

//------------- End of function Location::get ------//

//----------- Begin of function Location::get ------//
//! desc
//!
void  Location::paint_terrain(short tileType, int x, int y, Rect *clip, VgaBuf *vgaBuf) {
    //temp grass
    firm_res.get_bitmap(DefTile::GRASS_BMP)->paint(tileType, x,y,clip,vgaBuf);
}

//------------- End of function Location::get ------//

//----------- Begin of function Location::get ------//
//! desc
//!
void Location::set_object(Firm *f) {
    object = f;
}

//------------- End of function Location::get ------//

//----------- Begin of function Location::get ------//
//!
//!
bool Location::is_object() {
    return object != NULL;
}

//------------- End of function Location::get ------//

//----------- Begin of function Location::get ------//
//! is this object include this point and location
//!
bool Location::select_object(short tileType, short locx, short locy, int mx, int my) {
    if (object && object->select_me(tileType, locx, locy, mx, my) ) {
	if ( selected_object != object ) {
	    if( selected_object )
		selected_object->is_selected = false;     // reset last sel.
	    selected_object = object;
	}
	else {
	    selected_object = NULL;
	}
	return true;
    }
    return false;

#ifdef DEBUG_SELECT_MAP
    //temp
    is_selected = !is_selected;
#endif
}

//------------- End of function Location::get ------//
