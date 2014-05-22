//Filename   : OWORLD_M.CPP
//Description: MapMatrix class

#include <OVGA.H>
#include <OSYS.H>
#include <OMOUSE.H>
#include <OWORLD.H>

//------------ Begin of function MapMatrix::draw_all ---------------//

void MapMatrix::draw_all() {
    Matrix::draw_all();

    draw_zoom_box(1);                               // 1-use vga_back
}

//------------ End of function MapMatrix::draw_all ---------------//

//------------ Begin of function MapMatrix::draw_update ---------------//

void MapMatrix::draw_update() {
}

//-------------- End of function MapMatrix::draw_update --------------//

//------------ Begin of function MapMatrix::draw_zoom_box ---------------//

void MapMatrix::draw_zoom_box(int useVgaBack) {
    static short colorIndex = 0;
    static short colorAdd = 1;

    int boxX1, boxY1, boxX2, boxY2;

    loc_to_abs_center_left( boxX1, boxY1, world.active_zoom_matrix->top_x_loc,
			    world.active_zoom_matrix->top_y_loc );

    boxX1 = boxX1 - abs_top_x + MAP_X1;
    boxY1 = boxY1 - abs_top_y + MAP_Y1;

    boxX2 = boxX1 + (ZOOM_WIDTH  * loc_width  / world.active_zoom_matrix->loc_width) - 1;
    boxY2 = boxY1 + (ZOOM_HEIGHT * loc_height / world.active_zoom_matrix->loc_height) - 1;

    //-----------------------------------------//

    colorIndex += colorAdd;

    if( colorIndex >= 6 )
	colorAdd = -1;

    else if( colorIndex <= 0 )
	colorAdd = 1;

    colorIndex=0;                                   //BUGHERE

    if( useVgaBack )
	vga_back.rect( boxX1, boxY1, boxX2, boxY2, 2, V_ORANGE+colorIndex);
    else
	vga_front.rect( boxX1, boxY1, boxX2, boxY2, 2, V_ORANGE+colorIndex);
}

//-------------- End of function MapMatrix::draw_zoom_box --------------//

//------------ Begin of function MapMatrix::detect ---------------//

int MapMatrix::detect() {
    //--- if press left button, select zoom area ----//

    enum {                                          // effective when click on an outter area of the box
	EFFECTIVE_BORDER = 15
    };

    if(mouse.single_click(win_x1-EFFECTIVE_BORDER,win_y1-EFFECTIVE_BORDER,win_x2+EFFECTIVE_BORDER,win_y2+EFFECTIVE_BORDER, LEFT_BUTTON)) {
	/*
	  int clickX = max(mouse.cur_x, win_x1);
	  clickX = min(mouse.cur_x, win_x2);
	  int clickY = max(mouse.cur_y, win_y1);
	  clickY = min(mouse.cur_y, win_y2);
	  //---- if the click position results in a zoom window that goes beyond the map, reposition it ----//
	  int zoomFrameWidth  = (ZOOM_WIDTH * loc_width / world.active_zoom_matrix->loc_width);
	  int zoomFrameHeight = (ZOOM_HEIGHT * loc_height / world.active_zoom_matrix->loc_height);
	  if( clickX + zoomFrameWidth - 1 > win_x2 )
          clickX = win_x2 - zoomFrameWidth + 1;
	  if( clickY + zoomFrameHeight - 1 > win_y2 )
	  clickY = win_y2 - zoomFrameHeight + 1;
	  //----------------------------------------------//
	  int absX = abs_top_x + (clickX-win_x1);
	  int absY = abs_top_y + (clickY-win_y1);
	  //-------- convert to absolute positions -------//
	*/
	int zoomFrameWidth  = (ZOOM_WIDTH * loc_width / world.active_zoom_matrix->loc_width);
	int zoomFrameHeight = (ZOOM_HEIGHT * loc_height / world.active_zoom_matrix->loc_height);

	int absX = (mouse.cur_x-win_x1)-zoomFrameWidth/2;
	int absY = (mouse.cur_y-win_y1)-zoomFrameHeight/2;
	absX=max(absX,1);
	absY=max(absY,1);
	absX=min(absX,win_x2-win_x1-zoomFrameWidth+1);
	absY=min(absY,win_y2-win_y1-zoomFrameHeight+1);
	absX+=abs_top_x;
	absY+=abs_top_y;

	int locX, locY;
	static int old_locX=-1,old_locY=-1;

	abs_to_loc( locX, locY, absX, absY );

	if(locX!=old_locX)
	    if(locY!=old_locY) {
		// chwg0401
		world.active_zoom_matrix->set_top_loc( locX, locY );
		// begin chwg0401
		//				if(world.zoom_level==1)
		//					world.zoom2_matrix.set_top_loc(	locX*ZOOM2_LOC_WIDTH/ZOOM1_LOC_WIDTH,
		//													locY*ZOOM2_LOC_HEIGHT/ZOOM1_LOC_HEIGHT );
		//				else
		//					world.zoom1_matrix.set_top_loc(  locX*ZOOM1_LOC_WIDTH/ZOOM2_LOC_WIDTH,
		//													locY*ZOOM1_LOC_HEIGHT/ZOOM2_LOC_HEIGHT );
		// end chwg0401
	    }
	old_locX=locX;
	old_locY=locY;
	//		world.active_zoom_matrix->set_top_loc( locX, locY );
	sys.redraw_zoom_flag = 1;

	return 1;
    }

    if( mouse.press_area(win_x1-EFFECTIVE_BORDER,win_y1-EFFECTIVE_BORDER,win_x2+EFFECTIVE_BORDER,win_y2+EFFECTIVE_BORDER, LEFT_BUTTON )) {
	int zoomFrameWidth  = (ZOOM_WIDTH * loc_width / world.active_zoom_matrix->loc_width);
	int zoomFrameHeight = (ZOOM_HEIGHT * loc_height / world.active_zoom_matrix->loc_height);

	int absX = (mouse.cur_x-win_x1)-zoomFrameWidth/2;
	int absY = (mouse.cur_y-win_y1)-zoomFrameHeight/2;

	absX=max(absX,1);
	absY=max(absY,1);
	absX=min(absX,win_x2-win_x1-zoomFrameWidth+1);
	absY=min(absY,win_y2-win_y1-zoomFrameHeight+1);
	absX+=abs_top_x;
	absY+=abs_top_y;

	//-------- convert to absolute positions -------//

	int locX, locY;
	static int old_locX=-1,old_locY=-1;

	abs_to_loc( locX, locY, absX, absY );

	if(locX!=old_locX)
	    if(locY!=old_locY) {
		// chwg0401
		world.active_zoom_matrix->set_top_loc( locX, locY );
		// begin chwg0401
		//				if(world.zoom_level==1)
		//					world.zoom2_matrix.set_top_loc(	locX*ZOOM2_LOC_WIDTH/ZOOM1_LOC_WIDTH,
		//													locY*ZOOM2_LOC_HEIGHT/ZOOM1_LOC_HEIGHT );
		//				else
		//					world.zoom1_matrix.set_top_loc(  locX*ZOOM1_LOC_WIDTH/ZOOM2_LOC_WIDTH,
		//													locY*ZOOM1_LOC_HEIGHT/ZOOM2_LOC_HEIGHT );
		// end chwg0401
	    }
	old_locX=locX;
	old_locY=locY;

	sys.redraw_zoom_flag = 1;

	return 1;
    }

    return 0;
}

//-------------- End of function MapMatrix::detect ---------------//

//------------ Begin of function MapMatrix::scroll ---------------//

void MapMatrix::scroll(int scrollX, int scrollY) {
    //---- scroll the zoom view area as if scrolling the zoom box ----//

    int boxX1, boxY1;

    loc_to_abs_center_left( boxX1, boxY1, world.active_zoom_matrix->top_x_loc,
			    world.active_zoom_matrix->top_y_loc );

    boxX1 = boxX1 - abs_top_x + MAP_X1;
    boxY1 = boxY1 - abs_top_y + MAP_Y1;

    int boxWidth  = (ZOOM_WIDTH  * loc_width  / world.active_zoom_matrix->loc_width);
    int boxHeight = (ZOOM_HEIGHT * loc_height / world.active_zoom_matrix->loc_height);

    //-------- scroll the box now --------//

    boxX1 += scrollX * loc_width;
    boxY1 += scrollY * loc_height;

    if( boxX1 < MAP_X1 )
	boxX1 = MAP_X1;

    if( boxY1 < MAP_Y1 )
	boxY1 = MAP_Y1;

    if( boxX1 + boxWidth - 1 > MAP_X2 )
	boxX1 = MAP_X2 - boxWidth + 1;

    if( boxY1 + boxHeight - 1 > MAP_Y2 )
	boxY1 = MAP_Y2 - boxHeight + 1;

    //------ set new top locations and redraw -----//

    int absX = boxX1 - MAP_X1 + abs_top_x;
    int absY = boxY1 - MAP_Y1 + abs_top_y;

    int locX, locY;

    abs_to_loc( locX, locY, absX, absY );

    world.active_zoom_matrix->set_top_loc( locX, locY );

    // begin chwg0401
    //	if(world.zoom_level==1)
    //		world.zoom2_matrix.set_top_loc(	locX*ZOOM2_LOC_WIDTH/ZOOM1_LOC_WIDTH,
    //													locY*ZOOM2_LOC_HEIGHT/ZOOM1_LOC_HEIGHT );
    //	else
    //		world.zoom1_matrix.set_top_loc(  locX*ZOOM1_LOC_WIDTH/ZOOM2_LOC_WIDTH,
    //													locY*ZOOM1_LOC_HEIGHT/ZOOM2_LOC_HEIGHT );
    // end chwg0401

    sys.redraw_zoom_flag = 1;
}

//-------------- End of function MapMatrix::scroll --------------//
