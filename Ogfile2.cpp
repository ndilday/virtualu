//Filename    : OGFILE2.CPP
//Description : Object Game file, save game and restore game, part 2

#include <stdio.h>
#include <stdlib.h>

#include <OSYS.H>
#include <OBATTLE.H>

//---------------------------//
#include <ODEPTRES.H>
#include <OSCHLRES.H>
#include <OENROLL.H>
#include <OLETTER.H>                              //##chea 990519
#include <OFACURES.H>                             //## chea 090699

//---------------------------//
#include <OCONFIG.H>
#include <OGAME.H>
#include <OINFO.H>
#include <OFINANCE.H>
#include <OPSCHOOL.H>
#include <OPOWER.H>

#include <OBOX.H>
#include <OSTR.H>
#include <ODATE.H>
#include <OMOUSECR.H>
#include <OWORLD.H>
#include <OGAME.H>
#include <OINFO.H>
#include <OAUDIO.H>
#include <OMUSIC.H>

//---------------------------//
#include <ODEPT.H>
#include <ONEWS.H>
#include <OTASK.H>
#include <OCHANCE.H>

#include <OATHLETI.H>
#include <ODEVELOP.H>
#include <OFACILIT.H>
#include <OLIBTECH.H>
#include <OINVEST.H>
#include <OSTUOFF.H>

//---------------------------//
#include <OGFILE.H>

//-------- Define constant ----------//

#define BOOK_MARK 0x1000                          // book mark for validing saving data

#define read_obj(obj, mark)\
{ \
      if( !read_book_mark( BOOK_MARK+(mark) ) )\
						   return 0;\
								if( !((obj).read_file(filePtr)) )\
												     return 0;\
														  }

#define write_obj(obj, mark)\
{ \
      write_book_mark( BOOK_MARK+(mark) );\
					      if( (!(obj).write_file(filePtr)) )\
										    return 0;\
												 }

//----------- Game file format --------------//
//
// Data of the following objects will be saved: (yes/no)
//
// DepartmentRes		department_res;
//
//	n	ImageRes          image_sys, image_interface, image_mascot, image_pict;
//	y	SchoolRes 			school_res;
//	n	CourseRes			course_res;
//	n	FieldRes				field_res;
//	y	DepartmentRes		department_res;
//	n	FirmRes				firm_res;
//	n	TerrainRes			terrain_res;
//	n	PlantRes				plant_res;
//	n	RoadRes				road_res;
//	n	SpriteRes			sprite_res;
//	y	FacultyRes			faculty_res;
//	n	FacultyPhotoRes	faculty_photo_res;
//	n	GameStrRes			game_str_res;
//	n	NameRes				name_res;
//	y	EnrollRes			enroll_res;
//	y	Letter				letter;
//	n	BlackBoard			blackboard;
//
// y PlayerSchool		player_school;
// n Battle            battle;
// y Config            config;
// y Game              game;
// n GameSet           game_set;
// y Finance				finance;
// y Info              info;
// n Power             power;
//   World             world;
//
//		FirmArray         firm_array;			// call battle.generate_static_objects() instead
//	n	SpriteArray			sprite_array;
//	y	DepartmentArray	department_array;
//	y	NewsArray			news_array;
//	y	TaskArray			task_array;
//	y	ChanceEvent			chance_event;
//
//	n	UserInterface		user_interface;
//	n	AdmOffice			adm_office;
//	y	Development			development_office;
//	n	Optimization		optimization;
//	n	OptStage3         opt_stage3;
//	y	Athletics			athletics_office;
//	y	LibTech				library_tech_office;
//	y	Facility				facility_office;
//	y	Investment			investment_office;
//	y	StudentOffice		student_office;		// fred 980918
//	n	Help					help;
//	n	Test					test;
//	n	GameSetting			game_setting;
//-------------------------------------------//

//--------- Define static vars ---------//

static int loaded_random_seed;

//-------- Define static class member vars -------//

File* GameFile::file_ptr;
char  GameFile::last_read_success_flag=0;

//-------- Begin of function GameFile::write_express_file -------//
//!
//! Save a game to file
//!
//! Return : 1 - file written successfully
//!          0 - not successful
//!
int GameFile::save_express_game(char* fileName) {
    File   file;
    String errStr;

    power.win_opened=1;                             // to disable power.mouse_handler()

    if( fileName )
	strcpy( file_name, fileName );

    int rc = 1;
    char lowDiskSpaceFlag = 0;
    DWORD sectorPerCluster = 0;
    DWORD bytePerSector = 0;
    DWORD freeCluster = 0;
    DWORD totalCluster = 0;
    if( GetDiskFreeSpace( NULL,                     // address of root path, NULL means the current root directory
			  &sectorPerCluster, &bytePerSector, &freeCluster, &totalCluster)) {
	DWORD freeSpace = DWORD( (double)freeCluster * sectorPerCluster * bytePerSector / 1024.0);

	if( m.is_file_exist(file_name) ) {
	    // if overwritting existing file, count the file size of the existing file
	    file.file_open(file_name);
	    freeSpace += file.file_size() / 1024;       // count the existing space
	    file.file_close();
	}
	if( !(rc = freeSpace >= MIN_FREE_SPACE) ) {
	    errStr = "Insufficient disk space ! The game is not saved.";
	    lowDiskSpaceFlag = 1;
	}
    }

    if( rc ) {
	rc = file.file_create( file_name, 0, 1 );     // 0=tell File don't handle error itself
	// 1=allow the writing size and the read size to be different
	if( !rc )
	    errStr = "Error creating saved game file.";
    }

    File* filePtr = &file;
    file_ptr = &file;                               // for write_book_mark()

    //----- check valid version first ------//

    if( rc ) {
	//		save_process();      // process game data before saving the game

	rc = write_game_header(file_ptr);             // write saved game header information

	if( game_file_array.demo_format )
	    file_ptr->file_put_short( -GAME_VERSION );  // negative no. means shareware version
	else
	    file_ptr->file_put_short( GAME_VERSION );

	if( !rc )
	    errStr = "Error creating saved game header.";

	if( rc ) {
	    // put data to file here
	    write_obj(player_school, 1);

	    if( !rc )
		errStr = "Error writing saved game data.";
	}
    }

    //------------Close file----------------------------//
    file.file_close();

    power.win_opened=0;

    //------- when saving error ---------//

    if( !rc ) {
	if( !lowDiskSpaceFlag )
	    remove( file_name );                        // delete the file as it is not complete

#ifndef DEBUG
	// use this message for all types of error message in the release version
	errStr = "Insufficient disk space ! The game is not saved.";
#endif

	box.msg( errStr );
    }

    return rc;
}

//---------- End of function GameFile::save_express_game -------//

//-------- Begin of function GameFile::load_express_game -------//
//!
//! Restore a game from file
//!
//! Return : 1  - file read successfully
//!          0  - not successful
//!          -1 - incorrect game data version error
//!
int GameFile::load_express_game(char* fileName) {
    File file;
    int  rc=0;
    char *errMsg = NULL;

    power.win_opened=1;                             // to disable power.mouse_handler()

    int oldCursor = mouse_cursor.get_icon();
    mouse_cursor.set_icon( CURSOR_WAITING );

    int powerEnableFlag = power.enable_flag;

    if( fileName )
	strcpy( file_name, fileName );

    rc = 1;
    if( !file.file_open( file_name, 0, 1 ) ) {      // 0=tell File don't handle error itself
	rc = 0;
	errMsg = "Cannot open save game file";
    }

    //-------- read in the GameFile class --------//

    if( rc ) {
	char gameFileName[MAX_PATH+1];

	strcpy( gameFileName, file_name );            // save the file name actually read, in case that the file names are different

	// read_file_header: read the whole object from the saved game file
	if( !file.file_read(this, sizeof(GameFile)) ) {
	    rc = 0;
	    errMsg = "Cannot read file header";
	}

	if( rc ) {
	    err_if ( file.file_get_short() != 0x99 )    // the end of header
		err_here();

	    if( !validate_header() ) {
		rc = 0;
		errMsg = "Save game incompatible";
	    }
	    else
		strcpy( file_name, gameFileName );
	}
    }

    //--------------------------------------------//
    //----- check version no. first ------//
    File* filePtr = &file;
    file_ptr = &file;                               // for read_book_mark()

    //-------------------------------------//

    // compare major version
    if( file_ptr->file_get_short()/100 != (game_file_array.demo_format ? -(GAME_VERSION/100) : GAME_VERSION/100) )
	return -1;

    read_obj(player_school, 1);

    //-------------------------------------//

    file.file_close();

    power.enable_flag = powerEnableFlag;

    mouse_cursor.restore_icon( oldCursor );

    power.win_opened=0;

    //---------------------------------------//

    switch(rc) {                                    // don't display msg if loaded successfully (rc==1)
    case 0:
    case -1:
	box.msg( errMsg );
	break;
    }

    last_read_success_flag = rc;                    // for external functions to read.

    return rc;
}

//---------- End of function GameFile::load_express_file -------//

//-------- Begin of function GameFile::write_file -------//
//!
//! Save a game to file
//!
//! Return : 1 - file written successfully
//!          0 - not successful
//!
int GameFile::write_file(File* filePtr) {
    file_ptr = filePtr;                             // for write_book_mark()

    //----- check valid version first ------//

    if( game_file_array.demo_format )
	filePtr->file_put_short( -GAME_VERSION );     // negative no. means shareware version
    else
	filePtr->file_put_short( GAME_VERSION );

    //------------------------------------------------//
    //
    // The order of writing data is different between
    // the shareware and registered version.
    //
    //------------------------------------------------//

    if( game_file_array.demo_format ) {
	if( !write_file_1(filePtr) )
	    return 0;

	if( !write_file_2(filePtr) )
	    return 0;
    }
    else {
	if( !write_file_2(filePtr) )
	    return 0;

	if( !write_file_1(filePtr) )
	    return 0;
    }

    if( !write_file_3(filePtr) )
	return 0;

    return 1;
}

//---------- End of function GameFile::write_file -------//

//-------- Begin of function GameFile::read_file -------//
//!
//! Restore a game from file
//!
//! Return : 1  - file read successfully
//!          0  - not successful
//!          -1 - incorrect game data version error
//!
int GameFile::read_file(File* filePtr) {
    file_ptr = filePtr;                             // for read_book_mark()

    //----- check version no. first ------//

    //-------------------------------------//

    battle.create_static_game_object();             // in order to call deptPtr->create_department_firm() later; since we don't save firm_array

    //-------------------------------------//

    int originalRandomSeed = m.get_random_seed();

    // compare major version
    if( filePtr->file_get_short()/100 != (game_file_array.demo_format ? -(GAME_VERSION/100) : GAME_VERSION/100) )
	return -1;

    //------------------------------------------------//
    //
    // The order of writing data is different between
    // the shareware and registered version.
    //
    //------------------------------------------------//

    if( game_file_array.demo_format ) {
	if( !read_file_1(filePtr) )
	    return 0;

	if( !read_file_2(filePtr) )
	    return 0;
    }
    else {
	if( !read_file_2(filePtr) )
	    return 0;

	if( !read_file_1(filePtr) )
	    return 0;
    }

    if( !read_file_3(filePtr) )
	return 0;

    //-------------------------------------//

    err_when( originalRandomSeed != m.get_random_seed() );

    loaded_random_seed = info.random_seed_4_save;   //## chea 210899 to load the random seed

    m.set_random_seed(loaded_random_seed);

    return 1;
}

//---------- End of function GameFile::read_file -------//

//-------- Begin of function GameFile::write_file_1 -------//
//!
//! Return : 1 - file written successfully
//!          0 - not successful
//!
int GameFile::write_file_1(File* filePtr) {
    // write *_res objects

    write_obj(department_res, 1);

    write_obj(school_res, 2);

    write_obj(enroll_res, 3);

    write_obj(letter, 4);                           //## chea 990519

    write_obj(faculty_res, 5);                      //## chea 090699

    return 1;
}

//---------- End of function GameFile::write_file_1 -------//

//-------- Begin of function GameFile::write_file_2 -------//
//!
//! Save a game to file
//!
//! Return : 1 - file written successfully
//!          0 - not successful
//!
int GameFile::write_file_2(File* filePtr) {
    // write game surface objects

    write_book_mark( BOOK_MARK+101 );

    if( !game.write_file(filePtr) )
	return 0;

    write_book_mark( BOOK_MARK+102 );

    if( !config.write_file(filePtr) )               // 1110
	return 0;

    write_book_mark( BOOK_MARK+103 );

    if( !info.write_file(filePtr) )                 // 1110
	return 0;

    write_book_mark( BOOK_MARK+104 );

    if( !finance.write_file(filePtr) )              // 1110
	return 0;

    write_book_mark( BOOK_MARK+105 );

    if( !player_school.write_file(filePtr) )        // 1110
	return 0;

    return 1;
}

//---------- End of function GameFile::write_file_2 -------//

//-------- Begin of function GameFile::write_file_3 -------//
//!
//! Return : 1 - file written successfully
//!          0 - not successful
//!
int GameFile::write_file_3(File* filePtr) {
    // write game data objects

    write_obj(department_array, 201);

    write_obj(news_array, 202);
    write_obj(task_array, 203);
    write_obj(chance_event, 204);

    write_obj(development_office, 205);
    write_obj(athletics_office, 206);
    write_obj(library_tech_office, 207);
    write_obj(facility_office, 208);
    write_obj(investment_office, 209);
    write_obj(student_office, 210);

    return 1;
}

//---------- End of function GameFile::write_file_3 -------//

//-------- Begin of function GameFile::read_file_1 -------//
//!
//! Return : 1  - file read successfully
//!          0  - not successful
//!
int GameFile::read_file_1(File* filePtr) {
    read_obj(department_res, 1);

    read_obj(school_res, 2);

    read_obj(enroll_res, 3);

    read_obj(letter, 4);                            //## chea 990519

    read_obj(faculty_res, 5);                       //## chea 090699

    return 1;
}

//---------- End of function GameFile::read_file_1 -------//

//-------- Begin of function GameFile::read_file_2 -------//
//!
//! Return : 1  - file read successfully
//!          0  - not successful
//!
int GameFile::read_file_2(File* filePtr) {
    if( !read_book_mark( BOOK_MARK+101 ) )
	return 0;

    if( !game.read_file(filePtr) )
	return 0;

    if( !read_book_mark( BOOK_MARK+102 ) )
	return 0;

    if( !config.read_file(filePtr, 1) )             // 1-keep system settings
	return 0;

    if( !read_book_mark( BOOK_MARK+103 ) )
	return 0;

    if( !info.read_file(filePtr) )
	return 0;

    read_obj(finance, 104);

    read_obj(player_school, 105)

	return 1;
}

//---------- End of function GameFile::read_file_2 -------//

//-------- Begin of function GameFile::read_file_3 -------//
//!
//! Return : 1  - file read successfully
//!          0  - not successful
//!
int GameFile::read_file_3(File* filePtr) {

    read_obj(department_array, 201);

    read_obj(news_array, 202);
    read_obj(task_array, 203);
    read_obj(chance_event, 204);

    read_obj(development_office, 205);
    read_obj(athletics_office, 206);
    read_obj(library_tech_office, 207);
    read_obj(facility_office, 208);
    read_obj(investment_office, 209);
    read_obj(student_office, 210);

    return 1;
}

//---------- End of function GameFile::read_file_3 -------//

//-------- Begin of function GameFile::write_book_mark -------//
//!
void GameFile::write_book_mark(short bookMark) {
    sys.yield();

    file_ptr->file_put_short(bookMark);
}

//---------- End of function GameFile::write_book_mark -------//

//-------- Begin of function GameFile::read_book_mark -------//
//!
//! Return : 1  - the book mark is matched
//!          0  - the book mark is not matched
//!
int GameFile::read_book_mark(short bookMark) {
    sys.yield();

    short chk = file_ptr->file_get_short();

#ifdef DEBUG
    err_when( chk != bookMark);
#endif

    return chk == bookMark;
}

//---------- End of function GameFile::read_book_mark -------//
