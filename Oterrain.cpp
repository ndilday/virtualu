//Filename    : OTERRAIN.CPP
//Description : TerrainRes Class Definition

#include <All.h>
#include <OVGA.H>
#include <OSYS.H>
#include <OSTR.h>
#include <OMISC.h>
#include <OGAMESET.H>
#include <OTERRAIN.h>

//----------- Define constants -------------//

#define TERRAIN_DB      "TERRAIN"

//----------- Begin of function TerrainRes Constructor -----//
//!
TerrainRes::TerrainRes() {
    init_flag=0;
}

//------------- End of function TerrainRes Constructor -----//

//----------- Begin of function TerrainRes Destructor ------//
//!

TerrainRes::~TerrainRes() {
    deinit();
}

//------------- End of function TerrainRes Destructor ------//

//----------- Begin of function TerrainRes::init --------//
//!
void TerrainRes::init() {
    if ( init_flag )
	return;

    //----- read info from TERRAIN.DBF ------//

    load_info();

    //---- open bitmap resource of different resolutions ----//

    load_res( ZOOM_SMALL , "TERRA", "S" );
    load_res( ZOOM_MEDIUM, "TERRA", "M" );
    load_res( ZOOM_LARGE , "TERRA", "L" );

    //----------------------------------------//

    init_flag=1;
}

//------------- End of function TerrainRes::init --------//

//----------- Begin of function TerrainRes::deinit --------//
//!
void TerrainRes::deinit() {
    if( init_flag ) {
	for( int i=0 ; i<MAX_ZOOM_LEVEL ; i++ )
	    res_bitmap[i].deinit();

	mem_del(info_array);

	init_flag=0;
    }
}

//------------- End of function TerrainRes::deinit --------//

//----------- Begin of function TerrainRes::load_info -------//
//!
//! Read in information of TERRAIN.DBF into memory array
//!
void TerrainRes::load_info() {
    TerrainRec  *terrainRec;
    TerrainInfo *terrainInfo;
    int       i;
    Database  *dbTerrain = game_set.open_db(TERRAIN_DB);

    terrain_count = (short) dbTerrain->rec_count();
    info_array = (TerrainInfo*) mem_add( sizeof(TerrainInfo)*terrain_count );

    //------ read in terrain bitmap info array -------//

    memset( info_array, 0, sizeof(TerrainInfo) * terrain_count );

    for( i=0 ; i<terrain_count ; i++ ) {
	terrainRec  = (TerrainRec*) dbTerrain->read(i+1);
	terrainInfo = info_array+i;

	switch( terrainRec->type[0] ) {
	case 'G':
	    terrainInfo->type = TERRAIN_GRASS;
	    break;
	}

	m.rtrim_fld( terrainInfo->file_name, terrainRec->file_name, terrainRec->FILE_NAME_LEN );
    }
}

//----------- End of function TerrainRes::load_info -------//

//----------- Begin of function TerrainRes::load_res --------//
//!
void TerrainRes::load_res(int zoomLevel, char* namePrefix, char* nameSuffix) {
    //------ read bitmaps into memory -----//

    String str2;

    str2  = DIR_RES;
    str2 += namePrefix;
    str2 += "_";
    str2 += nameSuffix;

    str2 += ".RES";

    res_bitmap[zoomLevel].init( str2, 1 );          // 1-read all bitmaps into memory

    //-------- set TerrainInfo::bitmap_ptr[] --------//

    char* bmpPtr;

    for( int i=0 ; i<terrain_count ; i++ ) {
	bmpPtr = res_bitmap[zoomLevel].read( info_array[i].file_name );

	info_array[i].bitmap_ptr[zoomLevel] = bmpPtr;
    }
}

//------------- End of function TerrainRes::load_res --------//

//---------- Begin of function TerrainRes::operator[] -----------//

TerrainInfo* TerrainRes::operator[](int terrainId) {
    err_if( terrainId<1 || terrainId>terrain_count )
	err_now( "TerrainRes::operator[]" );

    return info_array+terrainId-1;
}

//------------ End of function TerrainRes::operator[] -----------//
