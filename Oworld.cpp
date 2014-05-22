//Filename    : OWORLD.CPP
//Description : class World functions

#include <OVGA.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OSYS.H>
#include <OCONFIG.H>
#include <OTERRAIN.H>
#include <OWORLD.H>

//----------- Begin of function World::World ----------//

World::World() {
    loc_matrix = NULL;
}

//------------- End of function World::World -----------//

//----------- Begin of function World::~World ----------//

World::~World() {
    deinit();
}

//------------- End of function World::~World -----------//

//----------- Begin of function World::init ------------//
//!
void World::init() {
    //----------- initialize loc_matrix -----------//

    loc_matrix = (Location*) mem_add( sizeof(Location) * MAX_WORLD_X_LOC * MAX_WORLD_Y_LOC );

    memset( loc_matrix, 0, sizeof(Location) * MAX_WORLD_X_LOC * MAX_WORLD_Y_LOC );

    max_x_loc = MAX_WORLD_X_LOC;
    max_y_loc = MAX_WORLD_Y_LOC;

    //---------- initialize matrix classes --------//

    map_matrix.init( MAP_X1, MAP_Y1, MAP_X2, MAP_Y2,
		     MAP_LOC_WIDTH, MAP_LOC_HEIGHT,
		     // 1-use image buffer
		     loc_matrix, max_x_loc, max_y_loc, ZOOM_SMALL, 1 );

    zoom1_matrix.init( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2,
		       ZOOM1_LOC_WIDTH, ZOOM1_LOC_HEIGHT,
		       loc_matrix, max_x_loc, max_y_loc, ZOOM_MEDIUM );

    zoom2_matrix.init( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2,
		       ZOOM2_LOC_WIDTH, ZOOM2_LOC_HEIGHT,
		       loc_matrix, max_x_loc, max_y_loc, ZOOM_LARGE );

    //------- initialize vars -----------//

    show_map = 1;

    set_zoom_level(1);                              // set the zoom level to 1

    //-------- init terrain ---------//

    init_terrain();
}

//------------ End of function World::init -------------//

//----------- Begin of function World::deinit ------------//
//!
void World::deinit() {
    if( loc_matrix ) {
	mem_del( loc_matrix );
	loc_matrix = NULL;
    }

    //	Matrix::disp_sort_array.deinit();
}

//------------ End of function World::deinit -------------//

//----------- Begin of function World::set_zoom_level ------------//
//!
void World::set_zoom_level(int newZoomLevel) {
    zoom_level = newZoomLevel;

    if( zoom_level==1 )
	active_zoom_matrix = &zoom1_matrix;
    else
	active_zoom_matrix = &zoom2_matrix;

    sys.redraw_zoom_flag = 1;
}

//----------- End of function World::set_zoom_level ------------//

//----------- Begin of function World::set_show_map ------------//
//!
void World::set_show_map(int showMapFlag) {
    show_map = showMapFlag;

    sys.redraw_all_flag = 1;
}

//----------- End of function World::set_show_map ------------//

//----------- Begin of function World::init_terrain ------------//
//!
void World::init_terrain() {
    int terrainTypeCount = terrain_res.terrain_count;

    for( int i=0 ; i<MAX_WORLD_X_LOC*MAX_WORLD_Y_LOC ; i++ ) {
	loc_matrix[i].set_terrain(m.random(terrainTypeCount)+1);
    }
}

//----------- End of function World::init_terrain ------------//

//----------- Begin of function World::draw_all ------------//
//!
void World::draw_all() {
    active_zoom_matrix->draw_all();

    if( show_map ) {
	vga_back.bar( MAP_X1, MAP_Y1, MAP_X2, MAP_Y2, V_WHITE );

	vga_back.rect( MAP_X1-MAP_BORDER_WIDTH, MAP_Y1-MAP_BORDER_HEIGHT,
		       MAP_X2+MAP_BORDER_WIDTH, MAP_Y2+MAP_BORDER_HEIGHT, 2, V_BLACK );

	map_matrix.draw_all();
    }
}

//----------- End of function World::draw_all ------------//

//----------- Begin of function World::draw_update ------------//
//!
void World::draw_update() {
    active_zoom_matrix->draw_update();

    if( show_map )
	map_matrix.draw_update();
}

//----------- End of function World::draw_update ------------//

//----------- Begin of function World::detect ------------//
//!
//! Detect mouse action from user
//!
//! Return : 1 - mouse pressed on World area
//!          0 - mouse not pressed on World area
//!
int World::detect() {
    //---------- detect map -----------//

    if( show_map ) {
	if( map_matrix.detect() )
	    return 1;
    }

    //------- detect zoom view --------//

    if( active_zoom_matrix->detect() )
	return 1;

    return 0;
}

//----------- End of function World::detect ------------//

//--------- Begin of function World::get_loc --------//
//!
Location* World::get_loc(int xLoc, int yLoc) {
    err_when( xLoc<0 || xLoc>=MAX_WORLD_X_LOC );
    err_when( yLoc<0 || yLoc>=MAX_WORLD_Y_LOC );

    return loc_matrix + MAX_WORLD_X_LOC * yLoc + xLoc;
}

//----------- End of function World::get_loc --------//

//--------- Begin of function World::is_loc_valid --------//
//!
Location* World::get_valid_loc(int xLoc, int yLoc) {
    if ( xLoc>=0 && xLoc<MAX_WORLD_X_LOC
	 && yLoc>=0 && yLoc<MAX_WORLD_Y_LOC) {
	return loc_matrix + MAX_WORLD_X_LOC * yLoc + xLoc;
    }
    else
	return NULL;
}

//----------- End of function World::is_loc_valid --------//
