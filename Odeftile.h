//Filename    : ODefTile.H
//Description : define game constant for tile bmp and size

#ifndef __ODefTile_H
#define __ODefTile_H

//!Holds game constants for tile bitmap and size.
class DefTile {
public:
    //-------------- TILE_TYPE for map/zoom paint ---------------//
    enum {
	NUM_TILE = 3,
	BIG = 0, SMALL, BIGGEST,

	//temp for paints
	GRASS_BMP = 12,                             // 12 is the real one
	MARK_BMP = 13,
	TREE_BMP = 20, NUM_TREE_BMP = 3,
    };

    enum {
	// for Axoview::paint_tiles whose size > 1x1
	MAX_WIDTH = 7,
	MAX_HEIGHT  = 10,
    };

    static inline bool is_firm(unsigned short id) {
	if ( id >= TREE_BMP && id < TREE_BMP + NUM_TREE_BMP )
	    return false;
	else
	    return true;
    };

    static inline bool is_not_valid_type(short tileType) {
	if ( tileType < NUM_TILE && tileType >= 0 )
	    return false;
	else
	    return true;
    };

    static const short CELL_FULL_WIDTH[NUM_TILE]; // = {	32, 8 };
    static const short CELL_FULL_HEIGHT[NUM_TILE];// = {	32, 8 };
};
#endif                                            // #ifndef __ODefTile_H
