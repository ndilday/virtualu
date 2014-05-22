//Filename    : OSPRTRES.CPP
//Description : Object Sprite resource
//Owner       : Fred

#include <ALL.H>
#include <OMISC.H>
//#include <OWORLD.H>
#include <OGAMESET.H>
#include <OSPRTRES.H>
#include <OSTR.H>
#include <OSYS.H>
//#include <OWEATHER.H>

//-------- define file name -----------//

#define SPRITE_DB       "SPRITE"
#define SPRITE_ACTION_DB   "SACTION"
//#define SUB_SPRITE_DB		"SUB_SPR"

//-------- Begin of function SpriteRes::init ---------//

void SpriteRes::init() {
    deinit();

    load_sprite_info();
    //load_sub_sprite_info();

    init_flag=1;
}

//--------- End of function SpriteRes::init ----------//

//-------- Begin of function SpriteRes::deinit ---------//

void SpriteRes::deinit() {
    if( init_flag ) {
	delete[] sprite_info_array;
	//mem_del(sub_sprite_info_array);

	init_flag=0;
    }
}

//--------- End of function SpriteRes::deinit ----------//

//-------- Begin of function SpriteRes::load_sprite_info ---------//

void SpriteRes::load_sprite_info() {
    Database        *dbSprite    = game_set.open_db(SPRITE_DB);
    SpriteRec       *spriteRec;
    SpriteInfo      *spriteInfo;
    //SpriteActionRec *spriteActionRec;
    //SpriteMove      *spriteMove;
    //SpriteAttack    *spriteAttack;
    int           i;                                //, j, actionRecno, dirId;

    sprite_info_count = dbSprite->rec_count();
    sprite_info_array = new SpriteInfo[sprite_info_count];

    memset( sprite_info_array, 0, sizeof(SpriteInfo)*sprite_info_count );

    //short* first_dir_recno_array = (short*) mem_add( sizeof(short) * sprite_info_count );	// allocate temporary arrays for temporary storage
    //short* dir_count_array		  = (short*) mem_add( sizeof(short) * sprite_info_count );

    //------------ read in sprite info -------------//

    for( i=0 ; i<sprite_info_count ; i++ ) {
	spriteRec  = (SpriteRec*) dbSprite->read(i+1);
	spriteInfo = sprite_info_array+i;

	spriteInfo->sprite_type = spriteRec->sprite_type;
	m.rtrim_fld( spriteInfo->sprite_code, spriteRec->sprite_code, spriteRec->CODE_LEN );

	if(spriteInfo->sprite_type == ' ')
	    spriteInfo->sprite_type = 0;

	spriteInfo->loc_width  = 1;                   // m.atoi(spriteRec->loc_width , spriteRec->SPRITE_PARA_LEN);
	spriteInfo->loc_height = 1;                   // m.atoi(spriteRec->loc_height, spriteRec->SPRITE_PARA_LEN);

	//spriteInfo->frames_per_step = m.atoi(spriteRec->frames_per_step, spriteRec->SPRITE_PARA_LEN);

	//first_dir_recno_array[i] = m.atoi(spriteRec->first_move_recno, spriteRec->RECNO_LEN);
	//dir_count_array[i] 		 = m.atoi(spriteRec->move_count      , spriteRec->COUNT_LEN);
    }

    return;

    /*
      //------- read in sprite action info ---------//

      Database *dbSpriteMove = game_set.open_db(SPRITE_ACTION_DB);

      for( i=0 ; i<sprite_info_count ; i++ )
      {
      spriteInfo = sprite_info_array+i;

      for( j=0, actionRecno=first_dir_recno_array[i] ; j<dir_count_array[i] ; j++, actionRecno++ )
      {
      spriteActionRec = (SpriteActionRec*) dbSpriteMove->read(actionRecno);

      dirId = m.atoi(spriteActionRec->dir_id, spriteActionRec->DIR_ID_LEN);

      //--------- move motion --------//

      if( spriteActionRec->action[0] == 'M' )
      {
      spriteMove = spriteInfo->move_array+dirId;

      spriteMove->first_frame_recno = m.atoi(spriteActionRec->first_frame_recno, spriteActionRec->RECNO_LEN);
      spriteMove->frame_count 		= m.atoi(spriteActionRec->frame_count, spriteActionRec->COUNT_LEN);

      //--- the first movement frame is the default stop frame ---//

      if( spriteInfo->stop_array[dirId].frame_recno == 0)
      {
      spriteInfo->stop_array[dirId].frame_recno = spriteMove->first_frame_recno;
      spriteInfo->stop_array[dirId].frame_count = 1;
      }
      }

      //--------- stop bitmap ---------//

      else if( spriteActionRec->action[0] == 'S' )
      {
      spriteInfo->stop_array[dirId].frame_recno = m.atoi(spriteActionRec->first_frame_recno, spriteActionRec->RECNO_LEN);
      spriteInfo->stop_array[dirId].frame_count = m.atoi(spriteActionRec->frame_count, spriteActionRec->COUNT_LEN);
      }

      }
      }

      //----------- delete temp arrays -------------//

      //mem_del( first_dir_recno_array );
      //mem_del( dir_count_array );
    */
}

//-------- End of function SpriteRes::load_sprite_info ---------//

//-------- Begin of function SpriteRes::update_speed -------//

void SpriteRes::update_speed() {
    /*	SpriteInfo *spriteInfo;
	//short rainScale = weather.rain_scale();
	//short snowScale = weather.snow_scale();
	short rainScale = 0;
	short snowScale = 0;
	short speedDrop;

	rainScale = rainScale > 7 ? 7 : rainScale;
	snowScale = snowScale > 7 ? 7 : snowScale;

	for( int i=0 ; i<sprite_info_count ; i++ )
	{
	speedDrop = 0;
	spriteInfo = sprite_info_array+i;

	if( rainScale > 0 && spriteInfo->max_rain_slowdown > 0 )
	{
	speedDrop += rainScale*spriteInfo->max_rain_slowdown/8 + 1;
	}

	if( snowScale > 0 && spriteInfo->max_snow_slowdown > 0 )
	{
	speedDrop += snowScale*spriteInfo->max_snow_slowdown/8 + 1;
	}
	spriteInfo->speed = spriteInfo->max_speed - speedDrop;
	}
    */
}

//-------- End of function SpriteRes::update_speed -------//

#ifdef DEBUG

//-------- Begin of function SpriteRes::operator[] -------//

SpriteInfo* SpriteRes::operator[](int recNo) {
    if( recNo<1 || recNo>sprite_info_count )
	err.run( "SpriteRes::operator[%d]", recNo );

    return sprite_info_array+recNo-1;
}

//--------- End of function SpriteRes::operator[] --------//
#endif

//------- Begin of function SpriteInfo::~SpriteInfo -------//

SpriteInfo::~SpriteInfo() {
    // in free_bitmap_res
    /*
      if( init_flag )
      {
      for( int i=0 ; i<MAX_ZOOM_LEVEL ; i++ )
      res_bitmap[i].deinit();

      init_flag=0;
      }*/
}

//--------- End of function SpriteInfo::~SpriteInfo -------//

//------- Begin of function SpriteInfo::load_bitmap_res -------//
//!
void SpriteInfo::load_bitmap_res() {              //int zoomLevel, char* namePrefix, char* nameSuffix)
    if( ++loaded_count > 1 )                        // if bitmaps of this sprite has been loaded
	return;

    //---- open bitmap resource of different resolutions ----//

    load_bitmap_res( ZOOM_SMALL , sprite_code, "S" );
    load_bitmap_res( ZOOM_MEDIUM, sprite_code, "M" );
    load_bitmap_res( ZOOM_LARGE , sprite_code, "L" );
}

//------------- End of function SpriteInfo::load_bitmap_res --------//

//------- Begin of function SpriteInfo::load_bitmap_res -------//
void SpriteInfo::load_bitmap_res(int zoomLevel, char* namePrefix, char* nameSuffix) {
    //------ read bitmaps into memory -----//

    String str2;

    str2  = DIR_RES;
    str2 += namePrefix;
    str2 += "_";
    str2 += nameSuffix;

    str2 += ".RES";

    res_bitmap[zoomLevel].init( str2, 1 );          // 1-read all bitmaps into memory

    //-------- set PlantInfo::bitmap_ptr[] --------//

    char* bmpPtr;

    bmpPtr = res_bitmap[zoomLevel].read( sprite_code );

    bitmap_ptr[zoomLevel] = bmpPtr;
    bitmap_width[MAX_ZOOM_LEVEL] = *((short *)bmpPtr);
    bitmap_height[MAX_ZOOM_LEVEL] = *(1 + (short *)bmpPtr);
}

//------------- End of function SpriteInfo::load_res --------//

//------- Begin of function SpriteInfo::free_bitmap_res -------//

void SpriteInfo::free_bitmap_res() {
    loaded_count--;

    err_when( loaded_count < 0 );

    if( loaded_count==0 ) {                         // if this bitmap is still needed by other sprites
	for( int i=0 ; i<MAX_ZOOM_LEVEL ; i++ )
	    res_bitmap[i].deinit();
    }
}

//-------- End of function SpriteInfo::free_bitmap_res -------//
