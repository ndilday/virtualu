//Filename    : OFPHOTO.CPP
//Description : FacultyPhotoRes Class Definition

#include <All.h>
#include <OVGA.H>
#include <OSYS.H>
#include <OSTR.h>
#include <OMISC.h>
#include <OGAMESET.H>
#include <OFPHOTO.h>

//----------- Define constants -------------//

#define FACULTY_PHOTO_DB      "FPHOTO"

//----------- Begin of function FacultyPhotoRes Constructor -----//
//!
FacultyPhotoRes::FacultyPhotoRes() {
    init_flag=0;
}

//------------- End of function FacultyPhotoRes Constructor -----//

//----------- Begin of function FacultyPhotoRes Destructor ------//
//!

FacultyPhotoRes::~FacultyPhotoRes() {
    deinit();
}

//------------- End of function FacultyPhotoRes Destructor ------//

//----------- Begin of function FacultyPhotoRes::init --------//
//!
void FacultyPhotoRes::init() {
    if( init_flag )
	return;

    //---- open faculty photo resource file ----//

    String str;

    str  = DIR_RES;
    str += "I_FPHOTO.RES";

    res_photo.init_imported(str, 1);                // 1-don't read all into buffer

    //----- read info from FPHOTO.DBF ------//

    load_info();

    //----------------------------------------//

    init_flag=1;
}

//------------- End of function FacultyPhotoRes::init --------//

//----------- Begin of function FacultyPhotoRes::deinit --------//
//!
void FacultyPhotoRes::deinit() {
    if( init_flag ) {
	mem_del(info_array);

	res_photo.deinit();

	init_flag=0;
    }
}

//------------- End of function FacultyPhotoRes::deinit --------//

//----------- Begin of function FacultyPhotoRes::load_info -------//
//!
//! Read in information of FPHOTO.DBF into memory array
//!
void FacultyPhotoRes::load_info() {
    FacultyPhotoRec   *photoRec;
    FacultyPhoto    *photoInfo;
    Database      *dbFacultyPhoto = game_set.open_db(FACULTY_PHOTO_DB);
    int         bitmapOffset;

    photo_count = (short) dbFacultyPhoto->rec_count();
    info_array  = (FacultyPhoto*) mem_add( sizeof(FacultyPhoto)*photo_count );

    //------ read in photo bitmap info array -------//

    memset( info_array, 0, sizeof(FacultyPhoto) * photo_count );

    for( int i=0 ; i<photo_count ; i++ ) {
	photoRec  = (FacultyPhotoRec*) dbFacultyPhoto->read(i+1);
	photoInfo = info_array+i;

	switch( photoRec->nationality[0] ) {
	case 'A':                                   // Asian
	case 'B':                                   // Black
	    if( photoRec->sex == 'M' )
		photoInfo->gender_ethnic_group = MINORITY_MALE;
	    else
		photoInfo->gender_ethnic_group = MINORITY_FEMALE;
	    break;

	case 'W':                                   // Western
	    if( photoRec->sex == 'M' )
		photoInfo->gender_ethnic_group = NONMINORITY_MALE;
	    else
		photoInfo->gender_ethnic_group = NONMINORITY_FEMALE;
	    break;
	}

	memcpy( &bitmapOffset, photoRec->bitmap_ptr, sizeof(long) );

	photoInfo->bitmap_ptr = res_photo.read_imported(bitmapOffset);

	//## fred
	err_when( photoInfo->gender_ethnic_group < 0 || photoInfo->gender_ethnic_group >= GENDER_ETHNIC_TYPE_COUNT );
    }
}

//----------- End of function FacultyPhotoRes::load_info -------//

//---------- Begin of function FacultyPhotoRes::operator[] -----------//

FacultyPhoto* FacultyPhotoRes::operator[](int photoId) {
    err_if( photoId<1 || photoId>photo_count )
	err_now( "FacultyPhotoRes::operator[]" );

    return info_array+photoId-1;
}

//------------ End of function FacultyPhotoRes::operator[] -----------//
