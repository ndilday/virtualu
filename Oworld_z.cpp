//Filename   : OWORLD_Z.CPP
//Description: ZoomMatrix class

#include <KEY.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OCONFIG.H>
#include <OMOUSE.H>
#include <OMATRIX.H>
#include <OWORLD.H>
#include <OFIRM.H>
#include <OFIRMRES.H>
#include <COLCODE.H>
#include <OLOG.H>

//------------ Begin of function ZoomMatrix::ZoomMatrix ---------------//

ZoomMatrix::ZoomMatrix() {
    next_scroll_time = 0;
}

//------------ End of function ZoomMatrix::ZoomMatrix ---------------//

//------------ Begin of function ZoomMatrix::draw_all ---------------//

void ZoomMatrix::draw_all() {
    Matrix::draw_all();
}

//------------ End of function ZoomMatrix::draw_all ---------------//

//------------ Begin of function ZoomMatrix::draw_update ---------------//

void ZoomMatrix::draw_update() {
    //Matrix::draw_all();		//temp for debug fred 0611
}

//-------------- End of function ZoomMatrix::draw_update --------------//
void ZoomMatrix::draw_text() {
    // Matrix::draw_text();

    if( config.disp_building_label ) {
	// draw text on firm

	for( int firmRecno = 1; firmRecno <= firm_array.size(); ++firmRecno ) {
	    if( firm_array.is_deleted(firmRecno) )
		continue;
	    Firm *firmPtr = firm_array[firmRecno];
	    if( firmPtr->name_on_map() ) {
		put_center_text( (firmPtr->abs_rect[zoom_level].x1+firmPtr->abs_rect[zoom_level].x2)/2,
				 (firmPtr->abs_rect[zoom_level].y1+firmPtr->abs_rect[zoom_level].y2)/2,
				 firmPtr->name_on_map() );
	    }
	}
    }
}

//------------ Begin of function ZoomMatrix::detect ---------------//

int ZoomMatrix::detect() {
    if( detect_scroll() )
	return 1;

    return detect_select();                         // detect selecting a firm
}

//-------------- End of function ZoomMatrix::detect ---------------//

//--------- Begin of function ZoomMatrix::detect_scroll ---------//
//!
//! Detect if the mouse cursor is pushed towards the border
//! of the screen to scroll the zoom window.
//!
int ZoomMatrix::detect_scroll() {
    // just scrolled not too long ago, wait for a little while before next scroll.
    if( next_scroll_time && m.get_time() < next_scroll_time )
	return 0;

    int rc=0;

    //----- scroll left -----//

    if( mouse.cur_x == mouse.bound_x1 || mouse.key_code==KEY_LEFT ) {
	world.map_matrix.scroll(-1,0);
	rc = 1;
    }

    //---- scroll right -----//

    if( mouse.cur_x == mouse.bound_x2 || mouse.key_code==KEY_RIGHT ) {
	world.map_matrix.scroll(1,0);
	rc = 1;
    }

    //---- scroll top -------//

    if( mouse.cur_y == mouse.bound_y1 || mouse.key_code==KEY_UP ) {
	world.map_matrix.scroll(0,-1);
	rc = 1;
    }

    //---- scroll bottom ----//

    if( mouse.cur_y == mouse.bound_y2 || mouse.key_code==KEY_DOWN ) {
	world.map_matrix.scroll(0,1);
	rc = 1;
    }

    //----- set next scroll time based on scroll_speed -----//
    //
    // slowest scroll speed: 500/1  = 500 milliseconds or 1/2 second
    // fastest scroll speed: 500/10 = 50  milliseconds or 1/20 second
    //
    //------------------------------------------------------//
    // ## chwong 1207
    if( rc )
	next_scroll_time = m.get_time() + 500/(config.scroll_speed+1);

    return rc;
}

//----------- End of function ZoomMatrix::detect_scroll -----------//

//--------- Begin of function ZoomMatrix::detect_select ------------//

int ZoomMatrix::detect_select() {
    int absClickX = abs_top_x + (mouse.cur_x-win_x1);
    int absClickY = abs_top_y + (mouse.cur_y-win_y1);

    firm_array.touched_recno = 0;

    // ## chwg062499
    int i,j,dispCount = disp_sort_array.size();

    for( i=dispCount ;i>=1; i-- ) {
	DisplaySort* displaySortPtr = (DisplaySort*) disp_sort_array.get(i);
	//		if(!displaySortPtr)
	//			continue;

	if(displaySortPtr->object_type!=OBJECT_FIRM)
	    continue;

	j=displaySortPtr->object_recno;

	Firm *firmPtr;
	if( firm_array.is_deleted(j) )
	    continue;

	firmPtr = firm_array[j];

	//------ if the mouse is over the building ------//
	if( firmPtr->abs_rect[zoom_level].is_inside( absClickX, absClickY ) ) {

	    //------ if the pixel which is pointed by cursor of the building image is not transparent ------//
	    FirmInfo* firmInfo = firm_res[firmPtr->firm_id];
	    //-------- check if the firm is within the view area --------//
	    //			int bmpWidth  = firmInfo->bitmap_width[zoom_level];
	    //			int bmpHeight = firmInfo->bitmap_height[zoom_level];
	    char* bmpPtr  = firmInfo->bitmap_ptr[zoom_level];

	    // - world.active_zoom_matrix->win_x1 + world.active_zoom_matrix->abs_top_x;
	    int cur_x = absClickX - firmPtr->abs_rect[zoom_level].x1;
	    // - world.active_zoom_matrix->win_y1 + world.active_zoom_matrix->abs_top_y;
	    int cur_y = absClickY - firmPtr->abs_rect[zoom_level].y1;
	    /*
	      String s=m.format(cur_x,1);
              s+=",";
              s+=m.format(cur_y,1);
              s+=",";
              s+=m.format(IMGgetTransDecompress(bmpPtr,cur_x,cur_y) ,1);
	      DEBUG_LOG(s);
	    */
	    if(IMGgetTransDecompress(bmpPtr,cur_x,cur_y)==TRANSPARENT_CODE)
		continue;
	    //------ if building is the toppest ------//

	    firm_array.touched_recno     = j;
	    // the frame id. which the building is being touched
	    firm_array.touched_frame_count = sys.frame_count;

	    //------ if the mouse clicks on the building ------//

	    int rc=mouse.any_click( win_x1,win_y1,win_x2,win_y2, LEFT_BUTTON );

	    if( rc==1 ) {
		firm_array.select_firm(j);
		sys.redraw_all_flag = 1;
		return 1;
	    }

	    //--- double-click to bring up the firm's detail report ----//

	    else if( rc==2 ) {
		firm_array[j]->double_click_firm();
		return 1;
	    }
	    return 0;
	}
    }

    return 0;
}

//---------- End of function ZoomMatrix::detect_select -----------//
