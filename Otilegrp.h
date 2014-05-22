//Filename    : OTileGrp.h
//Description : OTileGrp Class Declaration

// for axoview's debug mode only; 0420
// blit() assume that entry '0' in bitmap means transparent

#ifndef __OTileGrp_H
#define __OTileGrp_H

// ES library header file
// CU header file

//----------- Define constants -------------//
//----------- Define enum types -------------//
//----------- Define struct -------------//

class Axoview;
class Rect;
class VgaBuf;
//----------- Define class TileGroup -----//
//!Not in use. A class to handle the containment and drawing of tiles.
class TileGroup {
    friend Axoview;                                 // let it access this.*

private:
    Rect *clip;                                   // clipping rectangle

    char **image;                                 // Pointer to array of tile bitmaps for this tile type
    int width,height;                             // Width and height of tile bitmap for this tile type
    int size;                                     // size of tile bitmap (in bytes)
    int num_tiles;

private:
    TileGroup(Rect *const vp, int numTiles, int w, int h);
    ~TileGroup();

    void  blit(int tile_num,int x,int y,          // Function to draw tile into screen buffer
	       VgaBuf *vgabuf);
};
#endif                                            //ifndef
