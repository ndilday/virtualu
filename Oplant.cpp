//Filename    : OPLANT.CPP
//Description : PlantRes Class Definition

#include <All.h>
#include <OVGA.H>
#include <OSYS.H>
#include <OSTR.h>
#include <OMISC.h>
#include <OGAMESET.H>
#include <OPLANT.h>

//----------- Define constants -------------//

#define PLANT_DB      "PLANT"

//----------- Begin of function PlantRes Constructor -----//
//!
PlantRes::PlantRes() {
    init_flag=0;
}

//------------- End of function PlantRes Constructor -----//

//----------- Begin of function PlantRes Destructor ------//
//!

PlantRes::~PlantRes() {
    deinit();
}

//------------- End of function PlantRes Destructor ------//

//----------- Begin of function PlantRes::init --------//
//!
void PlantRes::init() {
    if ( init_flag )
	return;

    //----- read info from PLANT.DBF ------//

    load_info();

    //---- open bitmap resource of different resolutions ----//

    load_res( ZOOM_SMALL , "PLANT", "S" );
    load_res( ZOOM_MEDIUM, "PLANT", "M" );
    load_res( ZOOM_LARGE , "PLANT", "L" );

    //----------------------------------------//

    init_flag=1;
}

//------------- End of function PlantRes::init --------//

//----------- Begin of function PlantRes::deinit --------//
//!
void PlantRes::deinit() {
    if( init_flag ) {
	for( int i=0 ; i<MAX_ZOOM_LEVEL ; i++ )
	    res_bitmap[i].deinit();

	mem_del(info_array);

	init_flag=0;
    }
}

//------------- End of function PlantRes::deinit --------//

//----------- Begin of function PlantRes::load_info -------//
//!
//! Read in information of PLANT.DBF into memory array
//!
void PlantRes::load_info() {
    PlantRec  *plantRec;
    PlantInfo *plantInfo;
    int      i;
    Database *dbPlant = game_set.open_db(PLANT_DB);

    plant_count = (short) dbPlant->rec_count();
    info_array = (PlantInfo*) mem_add( sizeof(PlantInfo)*plant_count );

    //------ read in plant bitmap info array -------//

    memset( info_array, 0, sizeof(PlantInfo) * plant_count );

    for( i=0 ; i<plant_count ; i++ ) {
	plantRec  = (PlantRec*) dbPlant->read(i+1);
	plantInfo = info_array+i;

	m.rtrim_fld( plantInfo->file_name, plantRec->file_name, plantRec->FILE_NAME_LEN );
    }
}

//----------- End of function PlantRes::load_info -------//

//----------- Begin of function PlantRes::load_res --------//
//!
void PlantRes::load_res(int zoomLevel, char* namePrefix, char* nameSuffix) {
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

    for( int i=0 ; i<plant_count ; i++ ) {
	bmpPtr = res_bitmap[zoomLevel].read( info_array[i].file_name );

	info_array[i].bitmap_ptr[zoomLevel] = bmpPtr;
    }
}

//------------- End of function PlantRes::load_res --------//

//---------- Begin of function PlantRes::operator[] -----------//

PlantInfo* PlantRes::operator[](int plantId) {
    err_if( plantId<1 || plantId>plant_count )
	err_now( "PlantRes::operator[]" );

    return info_array+plantId-1;
}

//------------ End of function PlantRes::operator[] -----------//
