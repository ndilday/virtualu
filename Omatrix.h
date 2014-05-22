//Filename   : OMATRIX.H
//Description: Header file for the class Matrix

#ifndef __OMATRIX_H
#define __OMATRIX_H

//------- Define constant for object_type; for DisplaySort::object_type --------//

enum {
    OBJECT_FIRM,
    OBJECT_PLANT,
    OBJECT_VEHICLE,
    OBJECT_SPRITE,
};

//---------- Define struct DisplaySort ----------//

//!UI structure for sorting UI objects, such as bitmaps.
struct DisplaySort {
    char  object_type;
    short object_recno;
    short depth;
    short loc_x,loc_y;
    short abs_x1, abs_x2, abs_y1, abs_y2;
    char* bitmap_ptr;
};

//--- Define constant values for Location::loc_type ---//

enum {
    LOC_EMPTY,
    LOC_IS_FIRM,
    LOC_IS_ROAD,
    LOC_IS_ROAD_SUB,                                // the top-right, bottom-left & bottom-right of the road
    LOC_IS_PLANT,
    LOC_IS_SPRITE,
};

enum {
    LOCATE_WALK_LAND = 0x01,
    //LOCATE_HAS_SPRITE = 0x02,
};

//----------- Define class Location -----//

//!Structure representing the location and features of physical facilities
//!(firms, plants, vehicles, and sprites) on campus. (Location class is
//!deprecated).
struct Location {
public:
    unsigned short loc_flag;
    char    loc_type;
    short    cargo_recno;
    short   extra_para;                           //char		extra_para;
    char    terrain_id;

public:
    int   is_empty()            { return loc_type == LOC_EMPTY; }
    void    set_terrain(int terrainId)  { terrain_id = terrainId; set_walkable_on(); }

    //---------- for sprite animation functions ---------//
    int   is_walkable() {                         //  is_road() || is_road_sub(); }
	return loc_flag & LOCATE_WALK_LAND;
    }
    void    set_walkable_on()   { loc_flag |= LOCATE_WALK_LAND; }
    void    set_walkable_off()  { loc_flag &= ~LOCATE_WALK_LAND; }

    //---------- firm functions ---------//

    void    set_firm(int firmRecno);
    void    remove_firm();

    int   is_firm()   { return loc_type == LOC_IS_FIRM; }
    int   firm_recno()  { return cargo_recno; }

    //--------- plant functions ---------//

    int     is_plant()            { return loc_type==LOC_IS_PLANT; }
    void    set_plant(int plantId, int offsetX, int offsetY);
    void    remove_plant();

    int   plant_id()            { if( is_plant() ) return extra_para; else return 0; }
    int   plant_inner_x() {                       // a percentage from 0% to 100%
	return cargo_recno & 0xFF;
    }
    int   plant_inner_y() {                       // a percentage from 0% to 100%
	return cargo_recno >> 8;
    }

    //---------- road functions ---------//

    int   is_road()           { return loc_type == LOC_IS_ROAD; }
    int   is_road_sub()         { return loc_type == LOC_IS_ROAD_SUB; }
    void    set_road(int roadId);
    void    set_road_sub()          { loc_type=LOC_IS_ROAD_SUB; set_walkable_on();}
    void    remove_road();
    //TO? add remove_road_sub();
    int   road_id()           { if( is_road() ) return cargo_recno; else return 0; }

    //---------- sprite functions ---------//

    void    set_sprite(int spriteRecno);
    void    remove_sprite();

    int   has_sprite()    { return is_walkable() && extra_para; }
    int   sprite_recno()    { if ( has_sprite() ) return extra_para; else return 0; }
};

//--------- define class Matrix ------------//

class DynArray;

//!Class representing the Main map of the physical layout of the campus.
//!Handles the drawing of objects on the main map.
class Matrix {
public:
    int      max_x_loc, max_y_loc;
    Location *loc_matrix;
    int      loc_width, loc_height;

    int      top_x_loc, top_y_loc;                // the top left location of current zoom window
    int   abs_top_x, abs_top_y;
    int      disp_x_loc, disp_y_loc;              // calculated in Matrix()

    int      win_x1, win_y1, win_x2, win_y2;
    int   win_width, win_height;

    char    save_image_flag;
    short*  save_image_buf;
    char   is_image_buf_latest;

    char    zoom_level;                           // any of ZOOM_SMALL, ZOOM_MEDIUM, or ZOOM_LARGE

    static DynArray disp_sort_array;

public:
    Matrix();
    ~Matrix();

    void        init(int winX1, int winY1, int winX2, int winY2,
		     int locWidth, int locHeight, Location* locMatrix,
		     int maxXLoc, int maxYLoc, int zoomLevel, char saveImageFlag=0);

    virtual void  draw_all();                     // draw everything on the map
    virtual void  draw_update();                  // called each frame, only draw portions that need updating
    virtual int   detect()    { return 0; }

    void        reset_image_buf()   { is_image_buf_latest=0; }

    void        put_center_text(int x, int y, char* str);

    void        set_top_loc(int topXLoc, int topYLoc);
    void        loc_to_abs_top_left(int& absX, int& absY, int locX, int locY);
    void        loc_to_abs_center_left(int& absX, int& absY, int locX, int locY);
    void        loc_to_abs_bottom_right(int& absX, int& absY, int locX, int locY);
    void        abs_to_loc(int& locX, int& locY, int absX, int absY);

protected:
    virtual void  draw_text();                    // called by draw_all

private:
    void        draw_objects();
    void        draw_objects_now(DynArray* dispSortArray);

    void        draw_terrain(int xLoc, int yLoc, int terrainId);
    void        draw_plant(int xLoc, int yLoc, int terrainId);
    void        draw_bitmap(int absX, int absY, char* bmpPtr);
};

//------------------------------------------//
#endif
