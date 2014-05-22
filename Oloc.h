//Filename    : OLoc.h
//Description : Location Class Declaration

#ifndef __OLoc_H
#define __OLoc_H

// ES library header file
// CU header file

//----------- Define constants -------------//
//----------- Define enum types -------------//
//----------- Define struct -------------//

class Firm;
class Rect;
//----------- Define class Location -----//
//!Structure representing the location and features of physical facilities
//!(firms, plants, vehicles, and sprites) on campus. (Location class is
//!deprecated).
class Location {
private:
    Firm  *object;                                //temp: point to firm, sprite, terrian...
    bool  is_selected;                            //temp: for debug mapview
    static Firm *selected_object;
    //------------------------------------------//

public:
    Location();
    ~Location();
    void  init();
    void  deinit();

    //temp for axoview's paint
    void  paint_object(short tileType, int x, int y, Rect* const clip, VgaBuf *vgaBuf);
    //temp for axoview's paint
    void  paint_terrain(short tileType, int x, int y, Rect* const clip, VgaBuf *vgaBuf);

    void  set_object(Firm *f);                    // for world.add_obj
    bool  is_object();                            //Temp for world.select_object
    // for world.select_object
    bool  select_object(short tileType, short locx, short locy, int mx, int my);
    //------------------------------------------//

private:

};
#endif                                            //ifndef
