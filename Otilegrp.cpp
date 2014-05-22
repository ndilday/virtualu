//Filename    : OTileGrp.cpp
//Description : OTileGrp Class Definition
/*
 - a class to handle the containment and drawing of tiles
*/
//TO redo the comment and style

// ES library header file
#include <All.h>
#include <Color.h>                                // for V_TRANS
#include <OVgaBuf.h>

// CU header file
#include "ORect.h"                                // for viewport clipping coor vars
#include  "OTileGrp.h"

//Temp

// Declare constants for the viewport clipping
/*#define VIEWPORT_X1 0
#define VIEWPORT_X2 319
#define VIEWPORT_Y1 0
#define VIEWPORT_Y2 199
*/
//#define SCREEN_WIDTH 320
//#define SCREEN_HEIGHT 200

/*******************************************************************
 Function:  TileGroup::TileGroup();
 Purpose:   The TileGroup class constructor
 Arguments: numTiles     - number of tiles (bitmaps)
      w             - width of the tile in pixels (bytes)
      h             - height of the tile in pixels (bytes)
 Comments:  Will compute size of each tile in bytes, and will
      allocate memory for the tile bitmaps
*******************************************************************/
TileGroup::TileGroup(Rect* viewPort, int numTiles, int w, int h) {
    clip = viewPort;
    width=w;
    height=h;

    size=w*h;                                       // Calculate size of tile bitmap
    num_tiles = numTiles;

    // Allocate memory for tile bitmaps
    image=new char *[num_tiles];
    for (int j=0; j<num_tiles; j++)
	image[j]=new char[size];
}

TileGroup::~TileGroup() {
    for (int j=0; j<num_tiles; j++) {
	mem_del(image[j]);
    }
    mem_del(image);
}

/*******************************************************************
 Function:  TileGroup::blit();
 Purpose:   TileGroup class function which blits the indicated tile
     bitmap to an offscreen buffer using the
     "Reverse Painter's Algorithm".
     Handles clipping and tile transparency.
 Arguments: tileId      - index of tile bitmap to display
     x, y          - offscreen coordinates where the tile
         should be drawn
     vgabuf        - pointer to offscreen buffer
 Comments:  The destination buffer (screen) must be 320x200.
*******************************************************************/
void TileGroup::blit(int tileId, int x, int y, VgaBuf *vgaBuf) {
    int txstart=0, txend=width, tystart=0, tyend=height;

    // Perform clipping before main loop
    // Clip tile to viewport and set boundaries to blit through
    if (x < clip->x1)
	txstart = clip->x1-x;

    if (y < clip->y1)
	tystart = clip->y1-y;

    if ((x+width-1) > clip->x2 )
	txend = clip->x2-x+1;

    if (( y+height-1) > clip->y2 )
	tyend = clip->y2-y+1;

    // Calculate tile and buffer starting offsets
    int toffset = (tystart*width)+txstart;
    int poffset = (y+tystart)*vgaBuf->buf_pitch()+(x+txstart);

    // Calculate next row increments
    int toffinc = ((width-txend)+txstart);
    int poffinc = (vgaBuf->buf_pitch()-(txend)+txstart);

    // Dereference one of the pointers to the tile bitmap for speed
    char* tileimage = image[tileId];
    char* screen = vgaBuf->buf_ptr();

    // Now loop through and copy the tile bitmap to the vga buffer
    for(int row=0; row<(tyend-tystart); row++) {
	for(int column=0; column<(txend-txstart); column++) {
	    // Get pixel from the offscreen buffer
	    int dest_pixel=0xFF & screen[poffset];

	    // *** Check if it is transparent, if so, copy
	    // the tile bitmap's pixel over it
	    if (dest_pixel == V_TRANS                   //!dest_pixel // && !tileimage[toffset]
		)
		screen[poffset] = tileimage[toffset];
	    poffset++; toffset++;
	}
	// Jump to start of next row
	toffset+=toffinc;
	poffset+=poffinc;
    }
    return;
}
