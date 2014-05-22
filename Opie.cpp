//Filename    : OPIE.CPP
//Description : Object Pie Chart

#include <math.h>

#include <ALL.H>
#include <VGAFUN.H>
#include <OVGA.H>
#include <OINFO.H>
#include <OICONRES.H>
#include <OGAME.H>
#include <OPIE.H>

#define PI_OVER180 1.74532925199433E-002

//-------- Begin of function PieChart::init() ----------//
//!
//! [char*] slotIconPtr = pointer to the pie slot icon
//!                       (default : icon_sys.read("PIESLOT1") )
//!
void PieChart::init(char* slotIconPtr) {
    if( slotIconPtr )
	slot_icon_ptr = slotIconPtr;
    else
	slot_icon_ptr = icon_sys.read("PIESLOT1");

    box_width = *((short*)slot_icon_ptr);

    pie_radius = box_width/2-3;

    //-------- create table of arc points -----------//

    int   i=0;
    float curRadian;

    curRadian = PI_OVER180 * 180;                   // start from 180" degree

    for( i=0 ; i<MAX_DEGREE ; curRadian+=PI_OVER180, i++ ) {
	pie_point_table[i].x = (box_width>>1) + sin(curRadian) * pie_radius;
	pie_point_table[i].y = (box_width>>1) + cos(curRadian) * pie_radius;

	if( i>=2 ) {                                  // fill gaps between lines
	    if( pie_point_table[i-1].x == pie_point_table[i-2].x &&
		pie_point_table[i-1].y == pie_point_table[i-2].y &&
		pie_point_table[i].x   -  pie_point_table[i-2].x &&
		pie_point_table[i].y   -  pie_point_table[i-2].y ) {
		pie_point_table[i-1].x = pie_point_table[i].x;
		pie_point_table[i-1].y = pie_point_table[i-2].y;
	    }
	}
    }
}

//-------- End of function PieChart::init() ----------//

//-------- Begin of function PieChart::init_image ----------//
//!
//! Call init_image() before starting calling draw_slice()
//!
//! <int> backColor = background color
//!                   (default : V_BACKGROUND)
//!
void PieChart::init_image(int backColor) {
    if( backColor == -1 )
	backColor = V_BACKGROUND;

    vga.init_image( box_width, box_width );

    IMGputIcon( 0, 0, slot_icon_ptr );

    last_degree= -1;                                // no last degree, first time
}

//-------- End of function PieChart::init_image ----------//

//-------- Begin of function PieChart::draw_slice ----------//
//!
//! Draw a pie slice
//!
//! <int> sliceShare = share of the slice, in percentage
//! <int> sliceColor = color of the slice
//!
void PieChart::draw_slice(float sliceShare, int sliceColor) {
    err_if( sliceShare<0 || sliceShare>100 || sliceShare==infinite_number )
	err_here();

    int i, lastX= -1, lastY = -1, pieCentre = box_width>>1;

    float endDegree = last_degree + (float) MAX_DEGREE * sliceShare / 100;
    endDegree = min( endDegree, MAX_DEGREE-1 );

    // last_degree+1 = current starting degree
    PiePoint *curPoint = pie_point_table + (int)last_degree+1;

    //---------- draw the slice -------------//

    for( i=last_degree+1 ; i<=endDegree ; i++, curPoint++ ) {
	if( curPoint->x != lastX || curPoint->y != lastY ) {
	    IMGline( pieCentre, pieCentre, curPoint->x, curPoint->y, sliceColor );

	    lastX = curPoint->x;
	    lastY = curPoint->y;
	}
    }

    last_degree = endDegree;                        // degree of the last drawn slice
}

//-------- End of function PieChart::draw_slice ----------//

//-------- Begin of function PieChart::put_image ----------//
//!
//! Call put_image() after calling draw_slice() to draw slice
//!
//! <int> x, y        = the top left corner of the image
//! <int> refreshFlag = refresh flag
//!
void PieChart::put_image(int x, int y) {
    if( last_degree < MAX_DEGREE-1 )
	draw_slice( 100L * (MAX_DEGREE - last_degree+1) / 360, V_WHITE );

    vga.put_image( x, y );                          // +3 = space between pie and panel down
    vga.deinit_image();
}

//-------- End of function PieChart::put_image ----------//
