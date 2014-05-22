//Filename    : OSprite.h
//Description : Sprite Class Declaration
//Owner       : Fred

#include <memory.h>
#include <ALL.H>
#include <GAMEDEF.H>
#include <OMISC.H>
#include <ORECT.H>
#include <OSPRITE.H>
#include <OSPRTRES.H>
#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OMATRIX.H>

// for this->draw()
#include <OVGA.H>
//#include <OSTR.H>
//#include <OFONT.H>

//-------- Begin of function Sprite::Sprite --------//
Sprite::Sprite() {
    // preserve virtual pointer
    memset( (char *)this + sizeof(void *), 0, sizeof(Sprite) - sizeof(void *) );
}

//--------- End of function Sprite::Sprite --------//

//-------- Begin of function Sprite::~Sprite --------//
Sprite::~Sprite() {
    deinit();
}

//--------- End of function Sprite::~Sprite --------//

//--------- Begin of function Sprite::init ---------//
void Sprite::init(short spriteId, short startXLoc, short startYLoc, char startSubXLoc, char startSubYLoc) {
    sprite_id = spriteId;

    go_x = next_x = cur_x = startXLoc;
    go_y = next_y = cur_y = startYLoc;

    /*
      cur_attack = 0;
      cur_action = SPRITE_IDLE;
      cur_dir 	  = m.random(GameDef::MAX_SPRITE_DIR_TYPE);	// facing any of the eight directions
      cur_frame  = 1;
      final_dir  = cur_dir;
    */

    //----- clone vars from sprite_res for fast access -----//
    sprite_id = 1;                                  //temp
    err_here();
    //990511		sprite_info = sprite_res[sprite_id];

    sprite_info->load_bitmap_res();

    //----- set the sprite's location & absolute positions on the map -----//

    set_abs(ZOOM_SMALL , MAP_LOC_WIDTH   , MAP_LOC_HEIGHT);
    set_abs(ZOOM_MEDIUM, ZOOM1_LOC_WIDTH, ZOOM1_LOC_HEIGHT);
    set_abs(ZOOM_LARGE , ZOOM2_LOC_WIDTH, ZOOM2_LOC_HEIGHT);

    will_not_move_next_process = 0;
}

//----------- End of function Sprite::init -----------//

//--------- Begin of function Sprite::deinit ---------//
void Sprite::deinit() {
    world.get_loc(cur_x, cur_y)->remove_sprite();
}

//----------- End of function Sprite::deinit -----------//

//--------- Begin of function Sprite::set_abs ---------//
//!
void Sprite::set_abs(int zoomLevel, int locWidth, int locHeight) {
}

//----------- End of function Sprite::set_abs ---------//

//--------- Begin of function Sprite::cur_sprite_frame ---------//
//! Return the current frame of the sprite
//!
SpriteFrame* Sprite::cur_sprite_frame(int *needMirror) {
    /*
      UCHAR curDir = display_dir();
      SpriteStop *stopAction= sprite_info->stop_array +curDir;

      return sprite_frame_res[0];       //temp

      if ( cur_frame > stopAction->frame_count )
      return sprite_frame_res[stopAction->frame_recno];       // first frame
      else            // only few sprite has stopAction->frame_count > 1
      return sprite_frame_res[stopAction->frame_recno+cur_frame-1];
    */
    return NULL;                                    //temp

    /*UCHAR curDir = display_dir();
      if( needMirror)
      *needMirror = need_mirror(curDir);

      // do not update cur_dir as curDir
      err_when(curDir<0 || curDir>=3*MAX_SPRITE_DIR_TYPE);

      switch( cur_action )
      {
      case SPRITE_MOVE:
      case SPRITE_SHIP_EXTRA_MOVE:
      if( guard_count)
      {
      if( curDir >= MAX_SPRITE_DIR_TYPE)
      {
      err_here();
      curDir %= MAX_SPRITE_DIR_TYPE;
      }
      return sprite_frame_res[sprite_info->guard_move_array[curDir].first_frame_recno+cur_frame-1];
      }
      else
      return sprite_frame_res[sprite_info->move_array[curDir].first_frame_recno+cur_frame-1];

      case SPRITE_ATTACK:
      err_when(curDir<0 || curDir>=MAX_SPRITE_DIR_TYPE);
      return sprite_frame_res[sprite_info->attack_array[cur_attack][curDir].first_frame_recno+cur_frame-1];

      case SPRITE_TURN:
      case SPRITE_IDLE:
      case SPRITE_WAIT:
      // air unit needs it own stop frames to float on air
      {
      if( guard_count )
      {
      if( curDir >= MAX_SPRITE_DIR_TYPE)
      {
      // if the sprite is turning, adjust direction to next
      if( turn_delay > 0)
      curDir ++;
      curDir %= MAX_SPRITE_DIR_TYPE;
      }

      SpriteGuardStop *guardStopAction = sprite_info->guard_stop_array + curDir;
      return sprite_frame_res[guardStopAction->first_frame_recno+
      min(guard_count,guardStopAction->frame_count)-1];
      }
      else
      {
      SpriteStop *stopAction= sprite_info->stop_array +curDir;
      if(cur_frame > stopAction->frame_count)
      return sprite_frame_res[stopAction->frame_recno];       // first frame
      else            // only few sprite has stopAction->frame_count > 1
      return sprite_frame_res[stopAction->frame_recno+cur_frame-1];
      }
      }

      case SPRITE_DIE:
      if(sprite_info->die.first_frame_recno)			// only if this sprite has dying frame
      {
      if( needMirror)
      *needMirror = 0;			// no need to mirror at any direction
      return sprite_frame_res[sprite_info->die.first_frame_recno+cur_frame-1];
      }

      default:
      return sprite_frame_res[sprite_info->move_array[curDir].first_frame_recno+cur_frame-1];
      }*/
}

//----------- End of function Sprite::cur_sprite_frame -----------//

//--------- Begin of function Sprite::update_abs_pos ---------//
//! called before matrix.draw()'s sorting
//!
void Sprite::update_abs_pos() {                   //

    /*
      SpriteFrame	*spriteFrame;	//temp
      if(!spriteFrame)
      spriteFrame = cur_sprite_frame();
    */

    /*
      abs_x1 = sub_cur_x + spriteFrame->offset_x - world.zoom_matrix->top_x_loc*Location::SUB_LOC_WIDTH*GameDef::ZOOM_LOC_WIDTH;
      abs_y1 = sub_cur_y + spriteFrame->offset_y - world.zoom_matrix->top_y_loc*Location::SUB_LOC_HEIGHT*GameDef::ZOOM_LOC_HEIGHT;

      abs_x2 = abs_x1 + spriteFrame->width  - 1;
      abs_y2 = abs_y1 + spriteFrame->height - 1;
    */
}

//----------- End of function Sprite::update_abs_pos -----------//

/*

short Sprite::cur_x_loc()
{ return sub_cur_x>>GameDef::ZOOM_X_SHIFT_COUNT; }
short	Sprite::cur_y_loc()
{ return sub_cur_y>>GameDef::ZOOM_Y_SHIFT_COUNT; }

short	Sprite::next_x_loc()
{ return sub_next_x>>GameDef::ZOOM_X_SHIFT_COUNT; }
short	Sprite::next_y_loc()
{ return sub_next_y>>GameDef::ZOOM_Y_SHIFT_COUNT; }

short Sprite::go_x_loc()
{ return sub_go_x>>GameDef::ZOOM_X_SHIFT_COUNT; }
short Sprite::go_y_loc()
{ return sub_go_y>>GameDef::ZOOM_Y_SHIFT_COUNT; }

//------------------------------------------------------//

short Sprite::cur_sub_x_loc()
{ return sub_cur_x>>GameDef::ZOOM_SUB_X_SHIFT_COUNT; }
short	Sprite::cur_sub_y_loc()
{ return sub_cur_y>>GameDef::ZOOM_SUB_Y_SHIFT_COUNT; }

short	Sprite::next_sub_x_loc()
{ return sub_next_x>>GameDef::ZOOM_SUB_X_SHIFT_COUNT; }
short	Sprite::next_sub_y_loc()
{ return sub_next_y>>GameDef::ZOOM_SUB_Y_SHIFT_COUNT; }

short Sprite::go_sub_x_loc()
{ return sub_go_x>>GameDef::ZOOM_SUB_X_SHIFT_COUNT; }
short Sprite::go_sub_y_loc()
{ return sub_go_y>>GameDef::ZOOM_SUB_Y_SHIFT_COUNT; }
*/

//--------- Begin of function Sprite::draw ----------//
//!
//! <Matrix*> matrixPtr - pointer to the current active matrix
//!
void Sprite::draw(Matrix* matrixPtr) {
    //--------- draw sprite on the zoom window ---------//

    //int needMirror;

    //SpriteFrame* spriteFrame = cur_sprite_frame(&needMirror);

    //-------- check if the firm is within the view area --------//

    int zoomLevel = matrixPtr->zoom_level;
    int bmpWidth  = sprite_info->bitmap_width[zoomLevel];
    int bmpHeight = sprite_info->bitmap_height[zoomLevel];

    //char* bitmapPtr = sprite_info->res_bitmap.read_imported(spriteFrame->bitmap_offset);
    err_when( !sprite_info->res_bitmap[zoomLevel].init_flag );
    char* bmpPtr  = sprite_info->bitmap_ptr[zoomLevel];

    //update_abs_rect(zoomLevel);

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

    //---- only portion of the sprite is inside the view area ------//

    if( x1 < 0 || x2 >= matrixPtr->win_width || y1 < 0 || y2 >= matrixPtr->win_height ) {
	vga_back.put_bitmap_area_trans_decompress( x1+matrixPtr->win_x1, y1+matrixPtr->win_y1, bmpPtr,
						   max(0,x1)-x1, max(0,y1)-y1, min(matrixPtr->win_width-1,x2)-x1, min(matrixPtr->win_height-1,y2)-y1);
    }

    //---- the whole sprite is inside the view area ------//

    else {
	vga_back.put_bitmap_trans_decompress( x1+matrixPtr->win_x1, y1+matrixPtr->win_y1, bmpPtr);

	/*
	  if( zoomLevel == 1 )
	  {
          String str;
          str  = "(";
          str += loc_x1;
          str += ",";
          str += loc_y1;
          str += ")";

          vga.use_back();
	  font_san.put(x1+matrixPtr->win_x1, y1+matrixPtr->win_y1, str);
	  vga.use_front();
	  }
	*/
    }
}

//--------- End of function Sprite::draw -----------//

//--------- Begin of function Sprite::process ---------//
void Sprite::process() {
    if ( will_not_move_next_process ) {
	will_not_move_next_process = (will_not_move_next_process+1)%10;
	return;
    }
    else
	will_not_move_next_process = 1;

    // randomly move along walkable locations (road)
    //
    go_x = cur_x;
    go_y = cur_y;

    go_x += m.random(3) - 1;
    go_y += m.random(3) - 1;

    Location* locPtr = world.get_valid_loc(go_x, go_y);

    if ( !locPtr || !locPtr->is_walkable() || locPtr->has_sprite() )
	return;

    world.get_loc(cur_x, cur_y)->remove_sprite();
    cur_x = go_x; cur_y = go_y;
    locPtr->set_sprite(sprite_recno);

    set_abs(ZOOM_SMALL , MAP_LOC_WIDTH   , MAP_LOC_HEIGHT);
    set_abs(ZOOM_MEDIUM, ZOOM1_LOC_WIDTH, ZOOM1_LOC_HEIGHT);
    set_abs(ZOOM_LARGE , ZOOM2_LOC_WIDTH, ZOOM2_LOC_HEIGHT);

}

//----------- End of function Sprite::process -----------//
