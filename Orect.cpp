//Filename    : ORECT.cpp
//Description : RECT Class Definition
//Owner       : Fred

#include "ORECT.h"

//----------- Begin of function Rect Constructor -----//
//!
Rect::Rect(short inX1, short inY1, short inX2, short inY2) {
    x1 = inX1;
    y1 = inY1;
    x2 = inX2;
    y2 = inY2;
}

//------------- End of function Rect Constructor -----//

//----------- Begin of function Rect::clip_me ------//
//!
void Rect::clip_me(Rect clipRect) {
    //TO ? add helper mehtod(s) for this
    if ( x1 < clipRect.x1)
	x1 = clipRect.x1;
    else if ( x1 > clipRect.x2 )
	x1 = clipRect.x2;

    if ( x2 < clipRect.x1)
	x2 = clipRect.x1;
    else if ( x2 > clipRect.x2 )
	x2 = clipRect.x2;

    if ( y1 < clipRect.y1)
	y1 = clipRect.y1;
    else if ( y1 > clipRect.y2 )
	y1 = clipRect.y2;

    if ( y2 < clipRect.y1)
	y2 = clipRect.y1;
    else if ( y2 > clipRect.y2 )
	y2 = clipRect.y2;
}

//------------- End of function Rect::clipRect_me ------//
