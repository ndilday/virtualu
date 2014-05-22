//Filename    : OFIRMRES.h
//Description : FirmRes Class Declaration
//owner		: ho

#ifndef __OFIRMRES_H
#define __OFIRMRES_H

#ifndef __ORESX_H
#include <ORESX.H>
#endif

//------- Define FirmRec --------//

//!Structure which stores the code of the Firm and its corresponding name
//!and the department which it belongs to.
struct FirmRec {
    enum { CODE_LEN=8, NAME_LEN=50 };

    char  code[CODE_LEN];
    char  name[NAME_LEN];

    char  field;
};

//------- Define FirmBitmapRec --------//

//!Structure which stores the codes and campus location of the buildings as
//!recorded in the database.
struct FirmBitmapRec {
    enum { CODE_LEN=8, LOC_WIDTH_LEN=2, LOC_HEIGHT_LEN=2 };

    char  code[CODE_LEN];
    char  loc_width[LOC_WIDTH_LEN];
    char  loc_height[LOC_HEIGHT_LEN];
};

//------ Define struct FirmInfo --------//

//!Structure which stores the loaded bitmaps of the buildings at various
//!resolutions, as well as their location on campus
struct FirmInfo {
public:
    enum { CODE_LEN=8, NAME_LEN=50 };

    char  code[CODE_LEN+1];
    char  name[NAME_LEN+1];

    short  field_id;                              // >0 if this is an academic departmental building, this tells the field of the department

    char  loc_width;
    char  loc_height;

    char* bitmap_ptr[MAX_ZOOM_LEVEL];
    short bitmap_width[MAX_ZOOM_LEVEL];
    short bitmap_height[MAX_ZOOM_LEVEL];

public:
    int is_available() {                          // err_when(loc_width<=0); if loc_width==0, that means there is no such firm in the current university type
	return loc_width > 0;
    }
    int can_build(int xLoc, int yLoc);
    int is_fully_visible(int xLoc1, int yLoc1);
};

//------- Define class FirmRes ---------//

//!Class which initializes and loads all of the FirmInfo structures from
//!the database.
class FirmRes {
public:
    bool      init_flag;

    short     firm_count;
    FirmInfo*   info_array;

    ResourceIdx res_bitmap[MAX_ZOOM_LEVEL];

public:
    FirmRes();
    ~FirmRes();                                   // public for compilation error if private

    void        init();
    void        deinit();

    short*      get_color_remap_table(int selectedFlag);

    FirmInfo*     operator[](int firmId);

private:
    void        load_info();
    void        load_bmp_info(char* fileName);

    void        load_res(int bmpSizeType, char* namePrefix, char* nameSuffix);
};

extern FirmRes firm_res;

//-----------------------------------------------//
#endif
