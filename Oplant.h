//Filename    : OPLANT.h
//Description : PlantRes Class Declaration

#ifndef __OPLANT_H
#define __OPLANT_H

#ifndef __ORESX_H
#include <ORESX.H>
#endif

//------- Define PlantRec --------//

//!Structure contiaining the name of the resource file which stores the
//!bitmap for the plant.
struct PlantRec {
    enum { FILE_NAME_LEN=8 };

    char  file_name[FILE_NAME_LEN];
};

//------ Define struct PlantInfo --------//

//!Structure containing the name of the resource file and a pointer to the
//!bitmap it contains.
struct PlantInfo {
    enum { FILE_NAME_LEN=8 };

    char  file_name[FILE_NAME_LEN+1];
    char* bitmap_ptr[MAX_ZOOM_LEVEL];
};

//------- Define class PlantRes ---------//

//!Resource class which loads and makes available the plant names and
//!bitmaps.
class PlantRes {
public:
    bool      init_flag;

    short     plant_count;
    PlantInfo*  info_array;

    ResourceIdx res_bitmap[MAX_ZOOM_LEVEL];

public:
    PlantRes();
    ~PlantRes();                                  // public for compilation error if private

    void        init();
    void        deinit();

    PlantInfo*    operator[](int plantId);

private:
    void        load_info();
    void        load_bmp_info(char* fileName);

    void        load_res(int bmpSizeType, char* namePrefix, char* nameSuffix);
};

extern PlantRes plant_res;

//-----------------------------------------------//
#endif
