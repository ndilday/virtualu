//Filename    : OROAD.CPP
//Description : RoadRes Class Definition

#include <All.h>
#include <OVGA.H>
#include <OSYS.H>
#include <OSTR.h>
#include <OMISC.h>
#include <OGAMESET.H>
#include <OWORLD.H>
#include <OROAD.h>

//----------- Define constants -------------//

#define ROAD_DB     "ROAD"

//----------- Begin of function RoadRes Constructor -----//
//!
RoadRes::RoadRes() {
    init_flag=0;
}

//------------- End of function RoadRes Constructor -----//

//----------- Begin of function RoadRes Destructor ------//
//!

RoadRes::~RoadRes() {
    deinit();
}

//------------- End of function RoadRes Destructor ------//

//----------- Begin of function RoadRes::init --------//
//!
void RoadRes::init() {
    if ( init_flag )
	return;

    //----- read info from ROAD.DBF ------//

    load_info();

    //---- open bitmap resource of different resolutions ----//

    load_res( ZOOM_SMALL , "ROAD", "S" );
    load_res( ZOOM_MEDIUM, "ROAD", "M" );
    load_res( ZOOM_LARGE , "ROAD", "L" );

    //----------------------------------------//

    init_flag=1;
}

//------------- End of function RoadRes::init --------//

//----------- Begin of function RoadRes::deinit --------//
//!
void RoadRes::deinit() {
    if( init_flag ) {
	for( int i=0 ; i<MAX_ZOOM_LEVEL ; i++ )
	    res_bitmap[i].deinit();

	mem_del(info_array);

	init_flag=0;
    }
}

//------------- End of function RoadRes::deinit --------//

//----------- Begin of function RoadRes::load_info -------//
//!
//! Read in information of ROAD.DBF into memory array
//!
void RoadRes::load_info() {
    RoadRec  *roadRec;
    RoadInfo *roadInfo;
    int      i, j;
    Database *dbRoad = game_set.open_db(ROAD_DB);

    road_count = (short) dbRoad->rec_count();
    info_array = (RoadInfo*) mem_add( sizeof(RoadInfo)*road_count );

    //------ read in road bitmap info array -------//

    memset( info_array, 0, sizeof(RoadInfo) * road_count );

    for( i=0 ; i<road_count ; i++ ) {
	roadRec  = (RoadRec*) dbRoad->read(i+1);
	roadInfo = info_array+i;

	for( j=0 ; j<MAX_ROAD_CONNECT_TYPE ; j++ ) {
	    if( roadRec->connect_array[j]=='1' )
		roadInfo->connect_bits |= 1<<j;
	}

	m.rtrim_fld( roadInfo->file_name, roadRec->file_name, roadRec->FILE_NAME_LEN );
    }
}

//----------- End of function RoadRes::load_info -------//

//----------- Begin of function RoadRes::load_res --------//
//!
void RoadRes::load_res(int zoomLevel, char* namePrefix, char* nameSuffix) {
    //------ read bitmaps into memory -----//

    String str2;

    str2  = DIR_RES;
    str2 += namePrefix;
    str2 += "_";
    str2 += nameSuffix;

    str2 += ".RES";

    res_bitmap[zoomLevel].init( str2, 1 );          // 1-read all bitmaps into memory

    //-------- set RoadInfo::bitmap_ptr[] --------//

    char* bmpPtr;

    for( int i=0 ; i<road_count ; i++ ) {
	bmpPtr = res_bitmap[zoomLevel].read( info_array[i].file_name );

	info_array[i].bitmap_ptr[zoomLevel] = bmpPtr;
    }
}

//------------- End of function RoadRes::load_res --------//

//---------- Begin of function RoadRes::build_road -----------//
//!
//! Build a straight-line section of roads.
//!
//! <int> xLoc1, yLoc1 - the starting location of the road
//! <int> xLoc2, yLoc2 - the ending location of the road
//!
void RoadRes::build_road(int xLoc1, int yLoc1, int xLoc2, int yLoc2) {
    // all road postions must be even, so they will all fit with each other
    err_when( xLoc1%ROAD_WIDTH != 0 || yLoc1%ROAD_HEIGHT != 0 ||
	      xLoc2%ROAD_WIDTH != 0 || yLoc2%ROAD_HEIGHT != 0 );

    //----- if the length of the road is 1, don't construct it -----//

    if( xLoc1==xLoc2 && yLoc1==yLoc2 )
	return;

    //---- swap the starting and ending locations if they are in the wrong order ---//

    if( yLoc1 > yLoc2 ) {
	int ty = yLoc2;
	yLoc2  = yLoc1;
	yLoc1  = ty;
    }

    if( xLoc1 > xLoc2 ) {
	int tx = xLoc2;
	xLoc2  = xLoc1;
	xLoc1  = tx;
    }

    //---------- set the primary road id. ----------//

    int frontDir, backDir;

    if( xLoc1==xLoc2 ) {                            // vertical road (|)
	frontDir = ROAD_CONNECT_N;
	backDir  = ROAD_CONNECT_S;
    }

    else if( yLoc1==yLoc2 ) {                       // horizontal road (-)
	frontDir = ROAD_CONNECT_W;
	backDir  = ROAD_CONNECT_E;
    }

    else
	err_here();                                   // must be one of the above cases.

    //---------------lay the new roads ---------------//

    Location *locPtr;
    char    connectBits;
    int   roadId;
    int     xLoc=xLoc1, yLoc=yLoc1;
    int     loopCount=0;                            // for bug detection only

    //----------- start processing -------------//

    while(1) {
	locPtr = world.get_loc(xLoc, yLoc);

	//--- change existing roads to link with the new roads ---//

	if( locPtr->is_road() )
	    connectBits = road_res[locPtr->road_id()]->connect_bits;
	else
	    connectBits = 0;

	if( yLoc==yLoc2 && yLoc1 != yLoc2 )           //**BUGHERE
	    roadId = 0;

	if( !(xLoc==xLoc1 && yLoc==yLoc1) )           // don't go further outside on the front end
	    connectBits |= (1<<frontDir);

	if( !(xLoc==xLoc2 && yLoc==yLoc2) )           // don't go further outside on the back end
	    connectBits |= (1<<backDir);

	roadId = scan_road(connectBits);              // check if there is a road using the connections bits

	//------- if valid road, set it now --------//

	if( roadId ) {
	    locPtr->set_road(roadId);

	    if( xLoc+1 < MAX_WORLD_X_LOC )
		world.get_loc( xLoc+1, yLoc )->set_road_sub();

	    if( yLoc+1 < MAX_WORLD_Y_LOC )
		world.get_loc( xLoc, yLoc+1 )->set_road_sub();

	    if( xLoc+1 < MAX_WORLD_X_LOC && yLoc+1 < MAX_WORLD_Y_LOC )
		world.get_loc( xLoc+1, yLoc+1 )->set_road_sub();
	}

	//-------------------------------------------//

	if( xLoc1 != xLoc2 )
	    xLoc+=ROAD_WIDTH;

	if( yLoc1 != yLoc2 )
	    yLoc+=ROAD_HEIGHT;

	if( xLoc>xLoc2 || yLoc>yLoc2 )                // all done
	    break;

	++loopCount;

	err_when( loopCount > 10000 );
    }

    //------- redraw the zoom window ------//

    sys.redraw_all_flag = 1;
}

//---------- End of function RoadRes::build_road -----------//

//---------- Begin of function RoadRes::scan_road -----------//
//!
//! Check if any road in resource matches the connection bits.
//!
//! <char> connectBits - the connection bits
//!
//! return : <int> roadId  >0 - if any road matches the connection bits
//!								  ==0 - invalid connection bits
//!
int RoadRes::scan_road(char connectBits) {
    int      i;
    RoadInfo* roadInfo = info_array;

    for( i=0 ; i<road_count ; i++, roadInfo++ ) {
	if( roadInfo->connect_bits == connectBits )
	    return i+1;
    }

    return 0;
}

//---------- End of function RoadRes::scan_road -----------//

//---------- Begin of function RoadRes::operator[] -----------//

RoadInfo* RoadRes::operator[](int roadId) {
    err_if( roadId<1 || roadId>road_count )
	err_now( "RoadRes::operator[]" );

    return info_array+roadId-1;
}

//------------ End of function RoadRes::operator[] -----------//
