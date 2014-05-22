//Filename    : Osprite.h
//Description : OSprite Class Declaration
//Owner       : Fred.

#ifndef __OSPRITE_H
#define __OSPRITE_H

#include <ORECT.H>
#include <OSFRMRES.H>

class SpriteInfo;
class Matrix;
//----------- define class Sprite -----------//
//!Class for representing and drawing possibly moving sprites (rectangular
//!graphics stored in memory that can be transfered to the screen).
class Sprite {
public:
    //---------- Define action types ---------//
    enum {
	SPRITE_IDLE=1,
	SPRITE_MOVE,
	SPRITE_WAIT,                                // During a movement course, waiting for blocked sprites to pass
    };

public:
    //------------------------------------------//
    short       sprite_id;                        // sprite id. in SpriteRes
    short       sprite_recno;
    char        mobile_type;

    unsigned char cur_dir;                        // current direction
    unsigned char cur_frame;                      // current frame
    unsigned char cur_action;                     // current action
    unsigned char cur_attack;                     // current attack mode
    unsigned char final_dir;                      // for turning dir before attacking or moving

    short cur_x , cur_y;                          // current location
    short go_x  , go_y;                           // the destination of the path
    short next_x, next_y;                         // next tile in the moving path

    Rect  abs_rect[MAX_ZOOM_LEVEL];

    int will_not_move_next_process;               // for process()

    //int	sub_cur_x , sub_cur_y;		// current location
    //int	sub_go_x  , sub_go_y;		// the destination of the path
    //int	sub_next_x, sub_next_y;		// next tile in the moving path

    //----- clone vars from sprite_res for fast access -----//
    SpriteInfo*   sprite_info;

    //--------- static member vars --------------//

    //	static short abs_x1, abs_y1;	// the absolute postion, taking in account of sprite offset
    //	static short abs_x2, abs_y2;

public:
    Sprite();
    virtual ~Sprite();

    void  init(short spriteId, short startX, short startY, char startSubX, char startSubY);
    void  deinit();

    void  set_abs(int zoomLevel, int locWidth, int locHeight);

    void    draw(Matrix*);
    void    process();
    /*
      short	cur_x_loc();
      short	cur_y_loc();
      short	next_x_loc();
      short	next_y_loc();
      short	go_x_loc();
      short	go_y_loc();
      short	cur_sub_x_loc();
      short	cur_sub_y_loc();
      short	next_sub_x_loc();
      short	next_sub_y_loc();
      short	go_sub_x_loc();
      short	go_sub_y_loc();
    */

    virtual void  update_abs_pos();               //(int zoomLevel);	//SpriteFrame* =0

    SpriteFrame*  cur_sprite_frame(int *needMirror=0);
};
#endif
