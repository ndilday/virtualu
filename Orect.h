//Filename    : ORECT.h
//Description : Rect Class Declaration - designed for screen operation
//Owner       : Fred

#ifndef __ORECT_H
#define __ORECT_H

//----------- Define class Rect -----//

//!Rectangle class, designed for screen operations.
class Rect {
public:
    short x1, y1, x2, y2;

public:
    Rect()  {;}
    Rect(short x1,short y1,short x2,short y2);

    void  clip_me(Rect clipRect);
    bool  is_inside(short x, short y);
};

//----------- Begin of inline function Rect::is_inside --------//
//
inline bool Rect::is_inside(short x, short y) {
    return ( x>=x1 && x<=x2 && y>=y1 && y<=y2 );
}

//------------- End of inline function Rect::is_inside --------//
#endif
