//Filename    : OFIRM.h
//Description : Firm Class Declaration
//owner		: ho

#ifndef __OFIRM_H
#define __OFIRM_H

#include <OFIRMID.H>

#ifndef __OFIRMA_H
#include <OFIRMA.H>
#endif

#ifndef __ORECT_H
#include <ORECT.H>
#endif

//----------- Define class Firm ------------//

class Matrix;

//!Class used for drawing the buildings on the screen. Responds to double
//!click event by changing the view mode to detailed report view.
class Firm {
public:
    short   firm_recno;                           // record number in (and assigned by) FirmArray
    short   firm_id;                              // const

    short   department_recno;                     // >0 if this is an academic department

    short   loc_x1, loc_y1, loc_x2, loc_y2;
    Rect    abs_rect[MAX_ZOOM_LEVEL];
    //	short		firm_width;
    //	short		firm_height;

    int   setup_date;

    char*   name();
    char*   name_on_map();                        // for display name on zoom matrix
    int   is_selected()   { return firm_array.selected_recno == firm_recno; }

    //------------------------------------------//

public:
    void    init(int xLoc, int yLoc, int firmId);
    void    deinit();

    void    draw(Matrix*);
    void    double_click_firm();

private:
    void    init_abs(int zoomLevel, int locWidth, int locHeight);
    void    init_matrix();
    void    deinit_matrix();
};

//-----------------------------------------//
#endif
