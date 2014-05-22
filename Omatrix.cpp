//Filename    : OMATRIX.CPP
//Description : Object road direction turn, derived by World, Chain class

#include <ALL.H>
#include <COLCODE.H>
#include <OVGA.H>
#include <OMOUSE.H>
#include <OTERRAIN.H>
#include <OPLANT.H>
#include <OROAD.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OFIRM.H>
#include <OSPRITE.H>
//## fred 990511 #include <OSPRITEA.H>
#include <OWORLD.H>
#include <OBITMAPW.H>
#include <OMATRIX.H>
#include <OBLOB2W.H>

#define DRAW_SPRITE_ON_MAPVIEW  0                 // 0 or 1

//-------- Declare static functions ---------//

static int sort_disp_function( const void *a, const void *b );

//------- Define static class member functions ----//

DynArray Matrix::disp_sort_array(sizeof(DisplaySort),100);

//----------- Begin of function Matrix::Matrix ----------//
//!
Matrix::Matrix() {
    save_image_buf = NULL;
}

//------------- End of function Matrix::Matrix -----------//

//----------- Begin of function Matrix::~Matrix ----------//
//!
Matrix::~Matrix() {
    if( save_image_buf ) {
	mem_del( save_image_buf );
	save_image_buf = NULL;
    }
    disp_sort_array.zap(0);                         // 0-don't resize the array, keep its current size
}

//------------- End of function Matrix::~Matrix -----------//

//----------- Begin of function Matrix::init ----------//
//!
//! <int> winX1,winY1 = the coordination of the win,
//!       winX2,winY2   including the scroll bar and up,down panel
//!
//! <int> locWidth    = width and height of each location
//! <int> locHeight
//!
//! <Location*> locMatrix = pointer to a pre-allocated loc_matrix to be
//!								   used by this class.
//!
//! <int> maxXLoc = number of locations horizontally and vertically
//! <int> maxYLoc	 in the matrix.
//!
//! <int> zoomLevel 	  = any of ZOOM_SMALL, ZOOM_MEDIUM, or ZOOM_LARGE
//! <int> saveImageFlag = whether save the image to a buffer after drawing and reuse it during updates
//!
void Matrix::init(int winX1, int winY1, int winX2, int winY2,
		  int locWidth, int locHeight, Location* locMatrix,
		  int maxXLoc, int maxYLoc, int zoomLevel, char saveImageFlag) {
    win_x1 = winX1;                                 // bitmap area only
    win_y1 = winY1;
    win_x2 = winX2;
    win_y2 = winY2;

    win_width  = win_x2 - win_x1 + 1;
    win_height = win_y2 - win_y1 + 1;

    loc_width  = locWidth;
    loc_height = locHeight;

    //---------------------------------------------//

    loc_matrix = locMatrix;

    max_x_loc = maxXLoc;
    max_y_loc = maxYLoc;

    disp_x_loc = win_width/loc_width;

    if( disp_x_loc > max_x_loc )
	disp_x_loc = max_x_loc;

    disp_y_loc = win_height/(loc_height/2);

    if( disp_y_loc > max_y_loc )
	disp_y_loc = max_y_loc;

    set_top_loc( MAX_WORLD_X_LOC/2, 0 );

    zoom_level = zoomLevel;

    //----- allocate a buffer for saving the image ------//

    save_image_flag = saveImageFlag;

    if( saveImageFlag ) {
	if ( save_image_buf )
	    mem_del( save_image_buf );

	save_image_buf = (short *)mem_add( BitmapW::size(win_width, win_height) );
	is_image_buf_latest = 0;
    }
}

//------------- End of function Matrix::init -----------//

//----------- Begin of function Matrix::set_top_loc ----------//
//!
void Matrix::set_top_loc(int topXLoc, int topYLoc) {
    top_x_loc = topXLoc;
    top_y_loc = topYLoc;

    loc_to_abs_center_left( abs_top_x, abs_top_y, top_x_loc, top_y_loc );
}

//------------- End of function Matrix::set_top_loc -----------//

//--------- Begin of function Matrix::loc_to_abs_top_left --------//
//!
//! Return the top-left absolute positions of a given location
//!
//! <int&> absX, absY - vars for returning the absolute positions
//!							  of the location.
//!
//! <int>  locX, locY - the given location
//!
void Matrix::loc_to_abs_top_left(int& absX, int& absY, int locX, int locY) {
    absX = (loc_width/2) * (locX + locY);
    absY = (max_y_loc/2*loc_height) + (loc_height/2) * (-locX + locY -1);
}

//--------- End of function Matrix::loc_to_abs_top_left --------//

//--------- Begin of function Matrix::loc_to_abs_center_left --------//
//!
//! Return the top-left absolute positions of a given location
//!
//! <int&> absX, absY - vars for returning the absolute positions
//!							  of the location.
//!
//! <int>  locX, locY - the given location
//!
void Matrix::loc_to_abs_center_left(int& absX, int& absY, int locX, int locY) {
    absX = (loc_width/2) * (locX + locY);
    absY = (max_y_loc/2*loc_height) + (loc_height/2) * (-locX + locY);
}

//--------- End of function Matrix::loc_to_abs_center_left --------//

//--------- Begin of function Matrix::loc_to_abs_bottom_right --------//
//!
//! Return the bottom-right absolute positions of a given location
//!
//! <int&> absX, absY - vars for returning the absolute positions
//!							  of the location.
//!
//! <int>  locX, locY - the given location
//!
void Matrix::loc_to_abs_bottom_right(int& absX, int& absY, int locX, int locY) {
    absX = (loc_width/2) * (locX + locY + 3) - 1;
    absY = (max_y_loc/2*loc_height) + (loc_height/2) * (-locX + locY + 1) - 1;
}

//--------- End of function Matrix::loc_to_abs_bottom_right --------//

//--------- Begin of function Matrix::abs_to_loc --------//
//!
//! Return the location position of a given absolute position
//!
//! <int&> locX, locY - vars for returning the location positions
//! <int>  absX, absY - the given absolute position
//!
void Matrix::abs_to_loc(int& locX, int& locY, int absX, int absY) {
    locX = (absX/loc_width) - (absY/loc_height) + (max_y_loc/2);
    locY = (absX/loc_width) + (absY/loc_height) - (max_y_loc/2);
}

//--------- End of function Matrix::abs_to_loc --------//

//---------- Begin of function Matrix::draw_all ------------//
//!
void Matrix::draw_all() {
    //--- if the saved image buffer contains the latest image, use it instead redrawing a new one ---//

    if( save_image_flag && is_image_buf_latest ) {
	vga_back.put_bitmapW( win_x1, win_y1, save_image_buf );
    }
    else {
	draw_objects();

	if( save_image_flag ) {
	    vga_back.read_bitmapW( win_x1, win_y1, win_x2, win_y2, save_image_buf );
	    is_image_buf_latest = 1;
	}
    }

    draw_text();
}

//------------ End of function Matrix::draw_all ------------//

//---------- Begin of function Matrix::draw_update ------------//
//!
void Matrix::draw_update() {
}

//------------ End of function Matrix::draw_update ------------//

//--------- Begin of function Matrix::draw_objects ---------//
//!
void Matrix::draw_objects() {
    //----- get the location of the zoom area ------//

    enum { DRAW_OUTSIDE = 12 };

    int     xLocRow = top_x_loc;
    int     yLocRow = top_y_loc-DRAW_OUTSIDE;
    DisplaySort displaySort;

    disp_sort_array.zap(0);                         // 0-don't resize the array, keep its current size

    for( int yCount=disp_y_loc+DRAW_OUTSIDE*2 ; yCount>0 ; yCount-- ) {
	//------ process a diagonal row -----//

	int xLoc = xLocRow;
	int yLoc = yLocRow;

	for( int xCount=disp_x_loc+DRAW_OUTSIDE ; xCount>0 ; xCount--, xLoc++, yLoc++ ) {
	    if( xLoc < 0 || xLoc >= MAX_WORLD_X_LOC ||
		yLoc < 0 || yLoc >= MAX_WORLD_Y_LOC ) {
		continue;
	    }

	    //------------------------------------------//

	    Location* locPtr = world.get_loc(xLoc, yLoc);

	    //-------- if there is a road ---------//

	    if( locPtr->is_road() ) {                   // roads do not have height, so they can be drawn at the bottommost layer
		int absX, absY;

		loc_to_abs_top_left( absX, absY, xLoc, yLoc );

		char* bmpPtr = road_res[locPtr->road_id()]->bitmap_ptr[zoom_level];

		draw_bitmap( absX, absY-loc_height/2, bmpPtr );

		//continue;
	    }

	    else if( locPtr->is_road_sub() ) {
		//continue;
	    }

	    //------ draw terrain --------//

	    else {
		draw_terrain(xLoc, yLoc, locPtr->terrain_id);
	    }

	    //-------- there is a firm on the location --------//

	    if( locPtr->is_firm() ) {
		displaySort.object_type  = OBJECT_FIRM;
		displaySort.object_recno = locPtr->firm_recno();

		Firm* firmPtr = firm_array[locPtr->firm_recno()];

		//##chwg1207
		displaySort.loc_x = firmPtr->loc_x1;
		displaySort.loc_y = firmPtr->loc_y2;

		if( xLoc==firmPtr->loc_x1 &&              // only link in the top-left Location to prevent repeated adding of firms
		    yLoc==firmPtr->loc_y1 ) {                 // since size of firm can be greater than 1x1
		    disp_sort_array.linkin(&displaySort);
		}
		if( xLoc==firmPtr->loc_x2 &&              // only link in the top-right Location to prevent repeated adding of firms
		    yLoc==firmPtr->loc_y1 ) {                 // since size of firm can be greater than 1x1
		    disp_sort_array.linkin(&displaySort);
		}
	    }

	    //-------- there is a plant ---------//

	    else if( locPtr->is_plant() ) {
		err_when( locPtr->is_walkable() );
		displaySort.object_type = OBJECT_PLANT;

		int absX, absY;

		loc_to_abs_top_left( absX, absY, xLoc, yLoc );

		displaySort.bitmap_ptr = plant_res[locPtr->plant_id()]->bitmap_ptr[zoom_level];

		int bmpWidth  = *((short*)displaySort.bitmap_ptr);
		int bmpHeight = *( ((short*)displaySort.bitmap_ptr)+1 );

		//##chwg1207
		displaySort.abs_x1 = absX + (int)loc_width  * locPtr->plant_inner_x() / 100;
		// plant_inner_x() returns a relative offset as a percentage ranging from 0 to 100
		displaySort.abs_y2 = absY + (int)loc_height * locPtr->plant_inner_y() / 100;
		displaySort.abs_y1 = displaySort.abs_y2 - bmpHeight + 1;
		displaySort.loc_x = xLoc;
		displaySort.loc_y = yLoc;
		disp_sort_array.linkin(&displaySort);
	    }
	    //-------- there is a unit ---------//
	    /*	//## fred 990511
		else if( locPtr->has_sprite()
		&& ( zoom_level != ZOOM_SMALL || DRAW_SPRITE_ON_MAPVIEW ) )
		{
		// memset(&displaySort, 0, sizeof(displaySort));
		Sprite *spritePtr = sprite_array[locPtr->sprite_recno()];
		spritePtr->update_abs_pos();		// update its absolute position

		//##chwg1207
		displaySort.object_type  = OBJECT_SPRITE;
		displaySort.object_recno = locPtr->sprite_recno();
		displaySort.loc_x = xLoc;
		displaySort.loc_y = yLoc;
		disp_sort_array.linkin(&displaySort);
		}
	    */
	    //## fred 990511
	}                                             // for x

	//------- next row --------//

	if( yCount%2==0 )                             // alternately
	    yLocRow++;
	else
	    xLocRow--;
    }                                               // for y

    //---------- quicksort the array -----------//

    //	disp_sort_array.quick_sort( sort_disp_function );
    //	Quick sort cannot check all objects! Use bubble sort instead.
    disp_sort_array.bubble_sort( sort_disp_function );

    //------------ draw unit path and objects ---------------//

    draw_objects_now( &disp_sort_array );

    //----------- clean up the array ----------//

    //	disp_sort_array.zap(0);		// 0-don't resize the array, keep its current size
}

//----------- End of function Matrix::draw_objects -----------//

//---------- Begin of function Matrix::draw_objects_now -----------//
//!
void Matrix::draw_objects_now(DynArray* dispSortArray) {
    DisplaySort *displaySortPtr;
    Firm      *firmPtr;
    //## fred 990511	Sprite		*spritePtr;
    int       i, dispCount = dispSortArray->size();

    for( i=1 ; i<=dispCount ; i++ ) {
	if( i%10==1 )
	    sys.yield();

	displaySortPtr = (DisplaySort*) dispSortArray->get(i);

	switch(displaySortPtr->object_type) {
	case OBJECT_FIRM:
	    firmPtr = firm_array[displaySortPtr->object_recno];
	    firmPtr->draw(this);
	    break;

	case OBJECT_PLANT:
	    // shift the tree to left with 44 pixels for the space of shadow.
	    draw_bitmap( displaySortPtr->abs_x1-44, displaySortPtr->abs_y1-2, displaySortPtr->bitmap_ptr );
	    break;

	case OBJECT_SPRITE:
	    err_here();                               //## fred 990511
	    //## fred 990511	spritePtr = sprite_array[displaySortPtr->object_recno];
	    //## fred 990511	spritePtr->draw(this);
	    break;
	}
    }
}

//----------- End of function Matrix::draw_objects_now ------------//

//---------- Begin of function Matrix::draw_terrain -----------//
//!
void Matrix::draw_terrain(int xLoc, int yLoc, int terrainId) {
    int absX, absY;

    loc_to_abs_top_left( absX, absY, xLoc, yLoc );

    draw_bitmap( absX, absY, terrain_res[terrainId]->bitmap_ptr[zoom_level] );
}

//----------- End of function Matrix::draw_terrain ------------//

//---------- Begin of function Matrix::draw_plant -----------//
//!
void Matrix::draw_plant(int xLoc, int yLoc, int terrainId) {
    int absX, absY;

    loc_to_abs_top_left( absX, absY, xLoc, yLoc );

    draw_bitmap( absX, absY, terrain_res[terrainId]->bitmap_ptr[zoom_level] );
}

//----------- End of function Matrix::draw_plant ------------//

//--------- Begin of function Matrix::draw_bitmap ----------//
//!
//! <Matrix*> matrixPtr - pointer to the current active matrix
//!
void Matrix::draw_bitmap(int absX, int absY, char* bmpPtr) {
    int zoomLevel = zoom_level;
    int bmpWidth  = *( (short*)bmpPtr );
    int bmpHeight = *( ((short*)bmpPtr)+1 );

    int x1 = absX - abs_top_x;

    if( x1 <= -bmpWidth || x1 >= win_width )        // out of the view area, not even a slight part of it appears in the view area
	return;

    int y1 = absY - abs_top_y;

    if( y1 <= -bmpHeight || y1 >= win_height )
	return;

    int x2 = absX + bmpWidth - 1 - abs_top_x;
    int y2 = absY + bmpHeight - 1 - abs_top_y;

    if( x2 < 0 || y2 < 0 )
	return;
    if( x2 <= x1 || y2 <=y1 )
	return;

    //---- only portion of the sprite is inside the view area ------//

    if( x1 < 0 || x2 >= win_width || y1 < 0 || y2 >= win_height ) {
	vga_back.put_bitmap_area_trans_decompress( x1+win_x1, y1+win_y1, bmpPtr,
						   max(0,x1)-x1, max(0,y1)-y1, min(win_width-1,x2)-x1, min(win_height-1,y2)-y1 );
    }

    //---- the whole sprite is inside the view area ------//

    else {
	vga_back.put_bitmap_trans_decompress( x1+win_x1, y1+win_y1, bmpPtr );
    }
}

//--------- End of function Matrix::draw_bitmap -----------//

//---------- Begin of function Location::set_firm ------------//
//!
void Location::set_firm(int firmRecno) {
    loc_type    = LOC_IS_FIRM;
    cargo_recno = firmRecno;
    set_walkable_off();
}

//------------ End of function Location::set_firm ------------//

//---------- Begin of function Location::remove_firm ------------//
//!
void Location::remove_firm() {
    err_when( !is_firm() );

    loc_type = LOC_EMPTY;
    cargo_recno = 0;
}

//------------ End of function Location::remove_firm ------------//

//---------- Begin of function Location::set_plant ------------//
//!
void Location::set_plant(int plantId, int offsetX, int offsetY) {
    loc_type    = LOC_IS_PLANT;

    extra_para  = plantId;
    cargo_recno = (offsetY<<8) + offsetX;
    set_walkable_off();
}

//------------ End of function Location::set_plant ------------//

//---------- Begin of function Location::remove_plant ------------//
//!
void Location::remove_plant() {
    err_when( !is_plant() );

    loc_type  = LOC_EMPTY;
    cargo_recno = 0;
    extra_para  = 0;
}

//------------ End of function Location::remove_plant ------------//

//---------- Begin of function Location::set_road ------------//
//!
void Location::set_road(int roadId) {
    loc_type    = LOC_IS_ROAD;
    cargo_recno = roadId;
    extra_para  = 0;
    set_walkable_on();
}

//------------ End of function Location::set_road ------------//

//---------- Begin of function Location::remove_road ------------//
//!
void Location::remove_road() {
    err_when( !is_road() );

    loc_type  = LOC_EMPTY;
    cargo_recno = 0;
    extra_para  = 0;
    set_walkable_off();
}

//------------ End of function Location::remove_road ------------//

//---------- Begin of function Location::set_sprite ------------//
//!
void Location::set_sprite(int recno) {
    err_when(recno <=0);
    err_when( !is_walkable() );
    //loc_type		= LOC_IS_SPRITE;
    extra_para  = recno;
}

//------------ End of function Location::set_sprite ------------//

//---------- Begin of function Location::remove_sprite ------------//
//!
void Location::remove_sprite() {
    err_when( !has_sprite() );
    //loc_type = LOC_EMPTY;
    extra_para  = 0;
}

//------------ End of function Location::remove_sprite ------------//

//---------- Begin of function Matrix::draw_text -----------//
//!
void Matrix::draw_text() {
    // nothing
}

//---------- End of function Matrix::draw_text -----------//

//---------- Begin of function Matrix::put_center_text -----------//
//!
//! <int>   x, y - center of the absolute position where the text should be put.
//! <char*> str  - the display string.
//!
void Matrix::put_center_text(int x, int y, char* str) {
    str = translate.process(str);

    // #### begin Gilbert 12/09/2001 ######//
    /*
      const TEMP_BUFFER_SIZE = 0x2000;
      char 	tempBuffer[TEMP_BUFFER_SIZE];

      short w = font_news.text_width(str);
      short h = font_news.max_font_height;

      if( w * h + 2*sizeof(short) <= TEMP_BUFFER_SIZE )
      {
      char *bufferPtr = tempBuffer;

      *(short *)bufferPtr = w;
      bufferPtr += sizeof(short);

      *(short *)bufferPtr = h;
      bufferPtr += sizeof(short);

      memset( bufferPtr, TRANSPARENT_CODE, w * h );
      font_news.put_to_buffer(bufferPtr, w, 0, 0, str);

      //-- test clipping against win_x1, win_y1, win_x2, win_y2 --//

      int x1 = x - abs_top_x - w / 2 ;
      int x2 = x1 + w - 1;
      int y1 = y - abs_top_y - h / 2;
      int y2 = y1 + h - 1;

      if( x1 < win_x2 && x2 >= 0 && y1 < win_height && y2 >= 0 )
      {
      if( x1 < 0 || x2 >= win_width || y1 < 0 || y2 >= win_height )
      {
      vga_back.put_bitmap_area_trans( x1+win_x1, y1+win_y1, tempBuffer,
      max(0,x1)-x1, max(0,y1)-y1, min(win_width-1,x2)-x1, min(win_height-1,y2)-y1);
      }
      else
      {
      vga_back.put_bitmap_trans( x1+win_x1, y1+win_y1, tempBuffer );
      }
      }
      }
    */

    static Blob2DW tempBuffer;

    const int gap = 2;                                  // outside area for drawing black background
    short w = font_fblack.text_width(str) + 2*gap;
    short h = font_fblack.max_font_height + 2*gap;

    //-- test clipping against win_x1, win_y1, win_x2, win_y2 --//

    int x1 = x - abs_top_x - w / 2 ;
    int x2 = x1 + w - 1;
    int y1 = y - abs_top_y - h / 2;
    int y2 = y1 + h - 1;

    if( x1 < win_width && x2 >= 0 && y1 < win_height && y2 >= 0 ) {
	tempBuffer.clear();
	tempBuffer.resize(-w/2, -h/2, w, h);
	// black background
	tempBuffer.fill_area(-w/2, -h/2, -w/2+w-1, -h/2+h-1, (short)vga_back.translate_color(V_BLACK), 0);

	font_fblack.put_to_bufferW(tempBuffer.ptr->get_ptr(gap,gap), tempBuffer.buf_true_pitch(), 0, 0, str);

	if( x1 < 0 || x2 >= win_width || y1 < 0 || y2 >= win_height ) {
	    vga_back.put_bitmapW_area_trans( x1+win_x1, y1+win_y1, tempBuffer.bitmap_ptr(),
					     max(0,x1)-x1, max(0,y1)-y1, min(win_width-1,x2)-x1, min(win_height-1,y2)-y1);
	}
	else {
	    vga_back.put_bitmapW_trans( x1+win_x1, y1+win_y1, tempBuffer.bitmap_ptr() );
	}
    }
}

//----------- End of function Matrix::put_center_text ------------//

//------ Begin of function sort_disp_function ------//
//!
static int sort_disp_function( const void *a, const void *b ) {
    //##chwg1207
    if (((DisplaySort*)a)->loc_y<((DisplaySort*)b)->loc_y)
	return -1;
    if (((DisplaySort*)a)->loc_x>((DisplaySort*)b)->loc_x)
	return -1;
    return 1;
}

//------- End of function sort_disp_function ------//
