//Filename    : OTERRAIN.h
//Description : TerrainRes Class Declaration

#ifndef __OTERRAIN_H
#define __OTERRAIN_H

#ifndef __ORESX_H
#include <ORESX.H>
#endif

//------ Define terrain types ------//

enum { TERRAIN_GRASS=1 };

//------- Define TerrainRec --------//

//!Structure containing the filename and type of the terrain image as
//!stored in the db.
struct TerrainRec {
    enum { TYPE_LEN=8 };
    enum { FILE_NAME_LEN=8 };

    char  type[TYPE_LEN];
    char  file_name[FILE_NAME_LEN];
};

//------ Define struct TerrainInfo --------//

//!Structure containing the type, filename, and bitmap of a terrain image.
struct TerrainInfo {
    enum { FILE_NAME_LEN=8 };

    char  type;
    char  file_name[FILE_NAME_LEN+1];

    char* bitmap_ptr[MAX_ZOOM_LEVEL];
};

//------- Define class TerrainRes ---------//

//!Resource class for loading and managing terrain bitmaps.
class TerrainRes {
public:
    bool        init_flag;

    short       terrain_count;
    TerrainInfo*  info_array;

    ResourceIdx   res_bitmap[MAX_ZOOM_LEVEL];

public:
    TerrainRes();
    ~TerrainRes();                                // public for compilation error if private

    void        init();
    void        deinit();

    TerrainInfo*  operator[](int terrainId);

private:
    void        load_info();
    void        load_bmp_info(char* fileName);

    void        load_res(int bmpSizeType, char* namePrefix, char* nameSuffix);
};

extern TerrainRes terrain_res;

//-----------------------------------------------//
#endif
