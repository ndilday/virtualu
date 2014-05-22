//Filename    : OFPHOTO.h
//Description : FacultyPhotoRes Class Declaration

#ifndef __OFPHOTO_H
#define __OFPHOTO_H

#ifndef __ORESDB_H
#include <ORESDB.H>
#endif

//------ define constant --------//

enum {
    FACULTY_PHOTO_WIDTH  = 60,
    FACULTY_PHOTO_HEIGHT = 60
};

//------- Define FacultyPhotoRec --------//

//!Structure containing the information associated with the Faculty photo
//!as stored in the database.
struct FacultyPhotoRec {
    enum { NATIONALITY_LEN=10 };

    char sex;
    char nationality[NATIONALITY_LEN];

    char file_name[8];
    char bitmap_ptr[4];
};

//------ Define struct FacultyPhotoInfo --------//

//!Structure containing pointer to bitmap of photo as well as the
//!gender/ethnic group of the Faculty member.
struct FacultyPhoto {
    char  gender_ethnic_group;
    char* bitmap_ptr;
};

//------- Define class FacultyPhotoRes ---------//

//!Class for loading and initializing all Faculty photos.
class FacultyPhotoRes {
public:
    bool          init_flag;

    short         photo_count;
    FacultyPhoto*     info_array;

    ResourceDb      res_photo;

public:
    FacultyPhotoRes();
    ~FacultyPhotoRes();

    void          init();
    void          deinit();

    void          pick_photo();

    FacultyPhoto*     operator[](int photoId);

private:
    void          load_info();
};

extern FacultyPhotoRes faculty_photo_res;

//-----------------------------------------------//
#endif
