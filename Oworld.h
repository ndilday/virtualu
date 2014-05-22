//Filename   : OWORLD.H
//Description: Header for the file World

#ifndef __OWORLD_H
#define __OWORLD_H

#ifndef __OWORLDMT_H
#include <OWORLDMT.H>
#endif

#ifndef __ODYNARR_H
#include <ODYNARR.H>
#endif

//--------------- define class World --------------//

//!World display class, including the map and the zoom level.
class World {
public:
    Location*     loc_matrix;
    short       max_x_loc, max_y_loc;

    MapMatrix   map_matrix;
    ZoomMatrix    zoom1_matrix;
    ZoomMatrix    zoom2_matrix;

    char       show_map;                          // whether display the mini map on the top of the zoom map
    char        zoom_level;                       // either 1 or 2
    ZoomMatrix*   active_zoom_matrix;             // either zoom_matrix or zoom2_matrix

public:
    World();
    ~World();

    void        init();
    void        deinit();

    void        draw_all();
    void        draw_update();

    virtual int   detect();

    Location*   get_loc(int xLoc, int yLoc);
    Location*   get_valid_loc(int xLoc, int yLoc);// fred 980611

    void        set_show_map(int showMapFlag);
    void        set_zoom_level(int newZoomLevel); // either 1 or 2

private:
    void        init_terrain();
};

extern World world;

//----------------------------------------------//
#endif
