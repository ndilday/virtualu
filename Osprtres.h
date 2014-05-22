//Filename    : OSPRTRES.H
//Description : Header file of Object Sprite resource
//Owner       : Fred

#ifndef __OSPRTRES_H
#define __OSPRTRES_H

#include <ORESX.H>

//-------- Define struct SpriteRec ----------//

//!Sprite type and code as stored in the resource file.
struct SpriteRec {
    enum { TYPE_LEN=1, CODE_LEN=8, RECNO_LEN=5, COUNT_LEN=5, SPRITE_PARA_LEN=2, DAMAGE_LEN=3, TURN_RES_LEN=2 };

    char sprite_type;
    char sprite_code[CODE_LEN];

    //	char loc_width[SPRITE_PARA_LEN];
    //	char loc_height[SPRITE_PARA_LEN];

    //	char speed[SPRITE_PARA_LEN];
    //	char frames_per_step[SPRITE_PARA_LEN];
};

//----- Define struct SpriteActionRec ------//

//!Structure containing information needed for moving sprites.
struct SpriteActionRec {
    enum { NAME_LEN=8, ACTION_LEN=2, DIR_ID_LEN=2, RECNO_LEN=5, COUNT_LEN=2 };

    char sprite_name[NAME_LEN];
    char action[ACTION_LEN];
    char dir_id[DIR_ID_LEN];
    char first_frame_recno[RECNO_LEN];
    char frame_count[COUNT_LEN];
};

//----- Define struct SpriteMove ------//

//!Structure containing the rec number of the first frame in the sprite,
//!and the count of total frames.
struct SpriteMove {
    short first_frame_recno;                        // first frame recno to frame_array.
    char  frame_count;                              // no. of frames in the movement
};

//----- Define struct SpriteStop ------//

//!Structure containing the rec number of the frame the sprite was stopped,
//!and the count total frames.
struct SpriteStop {
    short frame_recno;                              // frame recno to frame_array.
    char  frame_count;
};

//---------- Define struct SpriteInfo -----------//

//!Contains sprite resource information and loads bitmap resources for each
//!zoom resolution.
class SpriteInfo {
public:
    enum { CODE_LEN=8 };

    char       sprite_code[CODE_LEN+1];           // used as filename to get bitmap from res_bitmap
    char       sprite_type;

    short      loc_width;                         // no. of locations it takes horizontally and vertically
    short      loc_height;

    char*      bitmap_ptr[MAX_ZOOM_LEVEL];        //temp
    short      bitmap_width[MAX_ZOOM_LEVEL];      //temp
    short      bitmap_height[MAX_ZOOM_LEVEL];     //temp

    int        loaded_count;                      // if it >= 1, it has been loaded into the memory
    ResourceIdx  res_bitmap[MAX_ZOOM_LEVEL];      // frame bitmap resource

    // move_array[24] to cater upward and downward directions for projectile
    // and also 16-direction movement for weapons
    //SpriteMove 	 move_array[3*GameDef::MAX_SPRITE_DIR_TYPE];
    //SpriteStop	 stop_array[3*GameDef::MAX_SPRITE_DIR_TYPE];

public:
    ~SpriteInfo();

    void       load_bitmap_res();
    void       free_bitmap_res();

    int      is_loaded()    { return loaded_count>0; }
    // SpriteInfo *get_sub_sprite(int i);
private:
    void      load_bitmap_res(int zoomLevel, char* namePrefix, char* nameSuffix);
};

//---------- Define class SpriteRes ------------//

//!Resource class which loads and manages all Sprites.
class SpriteRes {
public:
    char init_flag;
    int  sprite_info_count;

private:
    SpriteInfo* sprite_info_array;

public:
    SpriteRes()   { init_flag=0; }

    void  init();
    void  deinit();

    void  update_speed();

#ifdef DEBUG
    SpriteInfo* operator[](int recNo);
#else
    SpriteInfo* operator[](int recNo)   { return sprite_info_array+recNo-1; }
#endif

private:
    void  load_sprite_info();
};

extern SpriteRes sprite_res;

//----------------------------------------------//
#endif
