//Filename    : OROAD.h
//Description : RoadRes Class Declaration

#ifndef __OROAD_H
#define __OROAD_H

#ifndef __ORESX_H
#include <ORESX.H>
#endif

//----- define bit meaning for RoadInfo::connect_bits -----//

enum { MAX_ROAD_CONNECT_TYPE = 4 };

enum { ROAD_CONNECT_N, ROAD_CONNECT_E, ROAD_CONNECT_S, ROAD_CONNECT_W };

enum { ROAD_WIDTH=2, ROAD_HEIGHT=2 };

//------- Define RoadRec --------//

//!Structure containg the type of road and the filename of the
//!corresponding bitmap.
struct RoadRec {
    enum { FILE_NAME_LEN=8 };

    char  connect_array[MAX_ROAD_CONNECT_TYPE];
    char  file_name[FILE_NAME_LEN];
};

//------ Define struct RoadInfo --------//

//!Structure managed by the RoadRes. Contains the bitmap for a road segment
//!of a particular type.
struct RoadInfo {
    enum { FILE_NAME_LEN=8 };

    char  connect_bits;

    char  file_name[FILE_NAME_LEN+1];
    char* bitmap_ptr[MAX_ZOOM_LEVEL];
};

//------- Define class RoadRes ---------//

//!Class for loading and managing road bitmap resources. Including
//!different types of roads and their connecting segments.
class RoadRes {
public:
    bool        init_flag;

    short       road_count;
    RoadInfo*     info_array;

    ResourceIdx   res_bitmap[MAX_ZOOM_LEVEL];

public:
    RoadRes();
    ~RoadRes();                                   // public for compilation error if private

    void        init();
    void        deinit();

    void        build_road(int xLoc1, int yLoc1, int xLoc2, int yLoc2);

    RoadInfo*     operator[](int roadId);

private:
    void        load_info();
    void        load_bmp_info(char* fileName);

    void        load_res(int bmpSizeType, char* namePrefix, char* nameSuffix);

    int         scan_road(char connectBits);
};

extern RoadRes road_res;

//-----------------------------------------------//
#endif
