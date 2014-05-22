//Filename   : OWORLDMT.H
//Description: Header for Class MapMatrix and ZoomMatrix

#ifndef __OWORLDMT_H
#define __OWORLDMT_H

#ifndef __OMATRIX_H
#include <OMATRIX.H>
#endif

//-------- World matrix size ------------//

#define MAX_WORLD_X_LOC  80
#define MAX_WORLD_Y_LOC  80

//------------- Map window -------------//

#define MAP_X1          (MAP_X2-MAP_WIDTH+1)
#define MAP_Y1          (MAP_Y2-MAP_HEIGHT+1)
#define MAP_X2          (VGA_WIDTH-7)
#define MAP_Y2        (VGA_HEIGHT-5)

// 40 (MAX_WORLD_X_LOC/2) * 8 (MAP_LOC_WIDTH)  = 320
#define MAP_WIDTH       (MAX_WORLD_X_LOC/2)*MAP_LOC_WIDTH
// 40 (MAX_WORLD_Y_LOC/2) * 4 (MAP_LOC_HEIGHT) = 160
#define MAP_HEIGHT    (MAX_WORLD_Y_LOC/2)*MAP_LOC_HEIGHT

#define MAP_LOC_WIDTH    4
#define MAP_LOC_HEIGHT   2

#define MAP_BORDER_WIDTH  2
#define MAP_BORDER_HEIGHT 2

//----------- Zoom window -------------//

#define ZOOM_X1     33
#define ZOOM_Y1     58
#define ZOOM_X2         769
#define ZOOM_Y2         477

#define ZOOM_WIDTH      (ZOOM_X2-ZOOM_X1+1)
#define ZOOM_HEIGHT   (ZOOM_Y2-ZOOM_Y1+1)

//------- 1st level zoom --------//

#define ZOOM1_LOC_WIDTH   32
#define ZOOM1_LOC_HEIGHT  16

//------ 2nd level zoom ---------//

#define ZOOM2_LOC_WIDTH   64
#define ZOOM2_LOC_HEIGHT  32

//----------- define class MapMatrix -----------//

//!Class which handles the drawing and UI relating to the Campus map.
class MapMatrix : public Matrix {
public:
    void  draw_all();
    void  draw_update();

    int   detect();
    void  scroll(int scrollX, int scrollY);

private:
    void  draw_zoom_box(int useVgaBack);
};

//---------- define class ZoomMatrix -----------//

//!Class which handles the drawing and UI relating to zooming in and out on
//!the world.
class ZoomMatrix : public Matrix {
private:
    unsigned long next_scroll_time;               // next scroll time

public:
    ZoomMatrix();

    void  draw_all();
    void  draw_text();
    void  draw_update();

    int   detect();

private:
    int   detect_scroll();
    int   detect_select();
};

//------------------------------------------------------//
#endif
