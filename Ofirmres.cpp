//Filename    : OFIRMRES.CPP
//Description : FirmRes Class Definition

#include <All.h>
#include <COLCODE.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OSTR.h>
#include <OMISC.h>
#include <OCOLTBL.H>
#include <OWORLD.H>
#include <OGAMESET.H>
#include <OFIELD.H>
#include <OPSCHOOL.H>
#include <OFIRMRES.h>

//----------- Define constants -------------//

#define FIRM_DB     "FIRM"

//----------- Begin of function FirmRes Constructor -----//
//!
FirmRes::FirmRes() {
    init_flag=0;
}

//------------- End of function FirmRes Constructor -----//

//----------- Begin of function FirmRes Destructor ------//
//!

FirmRes::~FirmRes() {
    deinit();
}

//------------- End of function FirmRes Destructor ------//

//----------- Begin of function FirmRes::init --------//
//!
void FirmRes::init() {
    if ( init_flag )
	return;

    //------------------------------------//

    String str;

    str = "F_";

    switch( player_school.school_type ) {
    case LIBERAL_ARTS_SCHOOL:
	str += "L";
	break;

    case COMPREHENSIVE_SCHOOL:
	str += "C";
	break;

	//## fred 121
    case PRIVATE_SCHOOL:
	str += "VR";                                // private
	break;

    case PUBLIC_SCHOOL:
	str += "PR";                                // public
	break;
	//## fred 1211
    }

    //----- read info from FIRM.DBF and F_?.DBF ------//

    load_info();
    load_bmp_info(str);

    //---- open bitmap resource of different resolutions ----//

    load_res( ZOOM_SMALL , str, "S" );
    load_res( ZOOM_MEDIUM, str, "M" );
    load_res( ZOOM_LARGE , str, "L" );

    //----------------------------------------//

    init_flag=1;
}

//------------- End of function FirmRes::init --------//

//----------- Begin of function FirmRes::deinit --------//
//!
void FirmRes::deinit() {
    if( init_flag ) {
	for( int i=0 ; i<MAX_ZOOM_LEVEL ; i++ )
	    res_bitmap[i].deinit();

	mem_del(info_array);

	init_flag=0;
    }
}

//------------- End of function FirmRes::deinit --------//

//----------- Begin of function FirmRes::load_info -------//
//!
//! Read in information of FIRM.DBF into memory array
//!
void FirmRes::load_info() {
    FirmRec  *firmRec;
    FirmInfo *firmInfo;
    int      i;
    Database *dbFirm = game_set.open_db(FIRM_DB);

    firm_count = (short) dbFirm->rec_count();
    info_array = (FirmInfo*) mem_add( sizeof(FirmInfo)*firm_count );

    //------ read in firm bitmap info array -------//

    memset( info_array, 0, sizeof(FirmInfo) * firm_count );

    for( i=0 ; i<firm_count ; i++ ) {
	firmRec  = (FirmRec*) dbFirm->read(i+1);
	firmInfo = info_array+i;

	m.rtrim_fld( firmInfo->code, firmRec->code, firmRec->CODE_LEN );
	m.rtrim_fld( firmInfo->name, firmRec->name, firmRec->NAME_LEN );

	firmInfo->field_id = field_res.look_up_code( firmRec->field );
    }
}

//----------- End of function FirmRes::load_info -------//

//----------- Begin of function FirmRes::load_bmp_info -------//
//!
void FirmRes::load_bmp_info(char* fileName) {
    // e.g. "F_L" instead of "F_L_?"
    Database *dbFirmBitmap = game_set.open_db(fileName);

    //---- read location width and height info from FBITMAP -----//

    FirmBitmapRec  *firmBitmapRec;
    FirmInfo *firmInfo;
    int      i;

    for( i=0 ; i<firm_count ; i++ ) {
	firmBitmapRec  = (FirmBitmapRec*) dbFirmBitmap->read(i+1);
	firmInfo = info_array+i;

	firmInfo->loc_width  = m.atoi( firmBitmapRec->loc_width, firmBitmapRec->LOC_WIDTH_LEN );
	firmInfo->loc_height = m.atoi( firmBitmapRec->loc_height, firmBitmapRec->LOC_HEIGHT_LEN );
    }
}

//----------- End of function FirmRes::load_bmp_info -------//

//----------- Begin of function FirmRes::load_res --------//
//!
void FirmRes::load_res(int zoomLevel, char* namePrefix, char* nameSuffix) {
    //------ read bitmaps into memory -----//

    String str2;

    str2  = DIR_RES;
    str2 += namePrefix;
    str2 += "_";
    str2 += nameSuffix;

    str2 += ".RES";

    res_bitmap[zoomLevel].init( str2, 1 );          // 1-read all bitmaps into memory

    //-------- set FirmInfo::bitmap_ptr[] --------//

    char* bmpPtr;

    for( int i=0 ; i<firm_count ; i++ ) {
	bmpPtr = res_bitmap[zoomLevel].read( info_array[i].code );

	if( !bmpPtr ) { {                             // no bimtap available
	    //if ( zoomLevel == ZOOM_MEDIUM )		// 0209
	    char* k = info_array[i].code;
	    err_here();
	}
	//else
	//	err_here();

	info_array[i].loc_width = 0;                // BUGHERE, fixing bug on-fly
	//err_when( info_array[i].loc_width > 0 );		// if loc_width > 0, it indicates that the school type has that firm, and there should be a bitmap for it
	continue;
	}

	info_array[i].bitmap_ptr[zoomLevel] = bmpPtr;

	info_array[i].bitmap_width[zoomLevel]  = *( (short*)bmpPtr );
	info_array[i].bitmap_height[zoomLevel] = *( ((short*)bmpPtr)+1 );
    }
}

//------------- End of function FirmRes::load_res --------//

//--------- Begin of function FirmRes::get_color_remap_table --------//
//!
//! <int> selectedFlag - whether display outline around the bitmap
//!
short* FirmRes::get_color_remap_table(int selectedFlag) {
    if( selectedFlag )
	return (short*) vga.vga_color_table->get_table(16);
    else
	// for selected buildings
	return (short*) vga.vga_color_table->get_table(0);
}

//--------- End of function FirmRes::get_color_remap_table --------//

//------- Begin of function FirmInfo::can_build ------//
//!
//! Whether this firm can be built on the given location.
//!
int FirmInfo::can_build(int xLoc1, int yLoc1) {
    if( !is_fully_visible(xLoc1, yLoc1) )
	return 0;

    int xLoc, yLoc;

    for( yLoc=yLoc1 ; yLoc<yLoc1+loc_height ; yLoc++ ) {
	for( xLoc=xLoc1 ; xLoc<xLoc1+loc_width ; xLoc++ ) {
	    if( !world.get_loc(xLoc, yLoc)->is_empty() &&
		!world.get_loc(xLoc, yLoc)->is_plant() ) {
		return 0;
	    }
	}
    }

    return 1;
}

//------- End of function FirmInfo::can_build --------//

//------- Begin of function FirmInfo::is_fully_visible ------//
//!
//! Whether this firm can be built on the given location.
//!
int FirmInfo::is_fully_visible(int xLoc1, int yLoc1) {
    Matrix* matrixPtr = &(world.map_matrix);

    //-------- check if the firm is within the view area --------//

    int zoomLevel = matrixPtr->zoom_level;
    int bmpWidth  = bitmap_width[zoomLevel];
    int bmpHeight = bitmap_height[zoomLevel];

    int absX1 = (matrixPtr->loc_width/2) * (xLoc1 + yLoc1);
    int absX2 = absX1 + bmpWidth - 1;

    int absY2 = (MAX_WORLD_Y_LOC / 2 * matrixPtr->loc_height) + ((matrixPtr->loc_height/2) * (-xLoc1 + yLoc1 + loc_height ) - 1);
    int absY1 = absY2 - bmpHeight + 1;

    return absX1 >= matrixPtr->abs_top_x &&
	absY1 >= matrixPtr->abs_top_y &&
	absX2 < matrixPtr->abs_top_x + matrixPtr->win_width &&
	absY2 < matrixPtr->abs_top_y + matrixPtr->win_height;
}

//------- End of function FirmInfo::is_fully_visible --------//

//---------- Begin of function FirmRes::operator[] -----------//

FirmInfo* FirmRes::operator[](int firmId) {
    err_if( firmId<1 || firmId>firm_count )
	err_now( "FirmRes::operator[]" );

    return info_array+firmId-1;
}

//------------ End of function FirmRes::operator[] -----------//
