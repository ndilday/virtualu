//Filename    : OFIRMDRW.CPP
//Description : Firm drawing routines
//owner		: ho

#include <OVGA.H>
#include <OWORLD.H>
#include <OSTR.H>
#include <OFONT.H>
#include <OFIRMRES.H>
#include <OFIRM.H>
#include <OIFACE.H>

//--------- Begin of function Firm::draw ----------//
//!
//! <Matrix*> matrixPtr - pointer to the current active matrix
//!
void Firm::draw(Matrix* matrixPtr) {
    FirmInfo* firmInfo = firm_res[firm_id];

    //-------- check if the firm is within the view area --------//

    int zoomLevel = matrixPtr->zoom_level;
    int bmpWidth  = firmInfo->bitmap_width[zoomLevel];
    int bmpHeight = firmInfo->bitmap_height[zoomLevel];
    char* bmpPtr  = firmInfo->bitmap_ptr[zoomLevel];

    int x1 = abs_rect[zoomLevel].x1 - matrixPtr->abs_top_x;

    // out of the view area, not even a slight part of it appears in the view area
    if( x1 <= -bmpWidth || x1 >= matrixPtr->win_width )
	return;

    int y1 = abs_rect[zoomLevel].y1 - matrixPtr->abs_top_y;

    if( y1 <= -bmpHeight || y1 >= matrixPtr->win_height )
	return;

    int x2 = abs_rect[zoomLevel].x2 - matrixPtr->abs_top_x;
    int y2 = abs_rect[zoomLevel].y2 - matrixPtr->abs_top_y;

    if( x2 < 0 || y2 < 0 )
	return;

    //------ get the color remap table -------//

    // don't highlight building
    short* colorRemapTable = firm_res.get_color_remap_table( is_selected() );

    //---- only portion of the sprite is inside the view area ------//

    if( x1 < 0 || x2 >= matrixPtr->win_width || y1 < 0 || y2 >= matrixPtr->win_height ) {
	vga_back.put_bitmap_area_trans_remap_decompress( x1+matrixPtr->win_x1, y1+matrixPtr->win_y1, bmpPtr,
							 max(0,x1)-x1, max(0,y1)-y1, min(matrixPtr->win_width-1,x2)-x1, min(matrixPtr->win_height-1,y2)-y1, colorRemapTable );
	//		if(is_selected())
	//		{
	//			user_interface.brighten(max(0,x1+matrixPtr->win_x1), max(0,y1)+matrixPtr->win_y1,
	//				min(matrixPtr->win_width-1,x2)+matrixPtr->win_x1, min(matrixPtr->win_height-1,y2)+matrixPtr->win_y1);
	//		}
    }

    //---- the whole sprite is inside the view area ------//

    else {
	vga_back.put_bitmap_trans_remap_decompress( x1+matrixPtr->win_x1, y1+matrixPtr->win_y1, bmpPtr, colorRemapTable );
	//		if(is_selected())
	//		{
	//			user_interface.brighten(x1+matrixPtr->win_x1,y1+matrixPtr->win_y1,x2+matrixPtr->win_x1,y2+matrixPtr->win_y1);
	//		}
    }

}

//--------- End of function Firm::draw -----------//
