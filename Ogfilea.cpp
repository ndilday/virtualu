//Filename    : OGFILEA.CPP
//Description : Game File Array

#include <IO.H>
#include <ODIR.H>
#include <OSYS.H>
#include <ODATE.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OPOWER.H>

#include <OBOX.H>
#include <OFONT.H>
#include <OINFO.H>
#include <OGAME.H>
#include <OGAMESET.H>
#include <OGFILE.H>

//--------- Define constant ---------//
//----- File name of the game file array --------//

#define MAX_BROWSE_DISP_REC    5                  // max. no. of records can be displayed in the saved game browser

#define HALL_OF_FAME_FILE_NAME  "HALLFAME.DAT"

//------- Declare static vars & functions ----------//

static int     sort_game_file_function( const void *a, const void *b );
//static int     last_game_recno();

//------ Begin of function GameFileArray constuctor ------//

GameFileArray::GameFileArray() : DynArray( sizeof(GameFile), 10 ) {
    demo_format = 0;

#ifdef DEMO_DESIGN
    demo_format = 1;
#endif

    last_file_name[0] = NULL;
    has_read_hall_of_fame = 0;

    memset( hall_fame_array, 0, sizeof(HallFame)*HALL_FAME_NUM );
}

//-------- End of function GameFileArray constuctor ------//

//------ Begin of function GameFileArray::init ------//

void GameFileArray::init(char *extStr) {
    //------------- Read Hall of Fame ------------//

    if( !has_read_hall_of_fame ) {                  // only read once, GameFileArray::init() is called every time the load/save game menu is brought up.
	read_hall_of_fame();
	has_read_hall_of_fame = 1;
    }

    //-- Load all headers of all saved game files in current directory --//
	// load auto save as well if extension is numeric
    load_all_game_header(extStr, stricmp(extStr,"*.SAV")==0);
}

//-------- End of function GameFileArray::init ------//

//------ Begin of function GameFileArray::deinit ------//

void GameFileArray::deinit() {
    has_read_hall_of_fame = 0;                      // 0228
}

//-------- End of function GameFileArray::deinit ------//

//-------- Begin of function GameFileArray::save_new_game -----//
//!
//! Save current game to a new saved game file immediately without
//! prompting menu.
//!
//! Called by GameFileArray::process_action() and error handler.
//!
//! [char*] fileName - file name of the saved game
//!
void GameFileArray::save_new_game(char* fileName) {
    GameFile  gameFile;
    GameFile* gameFilePtr;
    int       addFlag=1;
    int       gameFileRecno;

    memset( &gameFile, 0, sizeof(GameFile) );

    if( fileName ) {
	//----- check for overwriting an existing file ----//

	for( gameFileRecno=1 ; gameFileRecno<=game_file_array.size() ; gameFileRecno++ ) {
	    gameFilePtr = game_file_array[gameFileRecno];

	    // if this file name already exist
	    if( strcmp(gameFilePtr->file_name, fileName)==0 ) {
		addFlag=0;
		break;
	    }
	}

	strcpy( gameFile.file_name, fileName );
    }
    else {
	gameFile.set_file_name();                     // give it a new game_file_name based on current group name
    }

    //----------- save game now ------------//

    if( gameFile.save_game(fileName) ) {
	strcpy( last_file_name, gameFile.file_name );

	if( addFlag ) {
	    linkin(&gameFile);

	    quick_sort( sort_game_file_function );
	}
	else {
	    // dynarr's func
	    game_file_array.update(&gameFile, gameFileRecno);
	}
    }
}

//-------- End of function GameFileArray::save_new_game -------//

//------- Begin of function GameFileArray::del_game ------//
//!
void GameFileArray::del_game(int recNo) {
    if( !box.ask( "This saved game will be deleted, proceed ?" ) )
	return;

    //---------------------------------------------------//

    unlink(game_file_array[recNo]->file_name);      // unlink() is a C standard function in io.h, for delete file

    go(recNo);
    linkout();

    go(recNo-1);                                    // skip back one record
}

//--------- End of function GameFileArray::del_game ------//

//------- Begin of function GameFileArray::del_express_game ------//
//!
void GameFileArray::del_express_game(int recNo) {
    if( !box.ask( "This express game will be deleted, proceed ?" ) )
	return;

    //---------------------------------------------------//

    unlink(game_file_array[recNo]->file_name);      // unlink() is a C standard function in io.h, for delete file

    go(recNo);
    linkout();

    go(recNo-1);                                    // skip back one record
}

//--------- End of function GameFileArray::del_express_game ------//

//-------- Begin of function GameFileArray::load_all_game_header --------//
//!
//! Load all headers of all saved game files in current directory.
//!
//! [int]readAutoSave    - include auto save which have numeric extension (eg: .001, .002)
//!

void GameFileArray::load_all_game_header(char *extStr, int readAutoSave) {
    int       i;
    Directory gameDir;
    GameFile  gameFile;
    File      file;

    if( readAutoSave ) {
	// see config.auto_save, config.auto_save_file_name
	// auto save file name has numeric extension

	// BUGHERE : need to check extStr is not numeric extension
	gameDir.read( "*.*", 1 );
	for( i = gameDir.size(); i > 0; --i ) {
	    char ext[_MAX_EXT];
	    _splitpath( gameDir[i]->name, NULL, NULL, NULL, ext );
	    // check for numeric extension
	    if( ext[0] == '.'
		&& ext[1] >= '0' && ext[1] <= '9'
		&& ext[2] >= '0' && ext[2] <= '9'
		&& ext[3] >= '0' && ext[3] <= '9'
		&& ext[4] == '\0' ) {
		continue;                                 // keep this file name
	    }
	    gameDir.linkout(i);                         // filter out this file
	}
    }

    // next gameDir.read will append other file name
    if( extStr )
	gameDir.read( extStr, 1 );                    // 1-Sort file names

    //-------- read in the headers of all game sets -------//

    zap();

    for( i=1 ; i<=gameDir.size() ; i++ ) {
	if( file.file_open( gameDir[i]->name, 1, 1 )  // last 1=allow varying read & write size
	    && file.file_read(&gameFile, sizeof(GameFile))
	    && gameFile.validate_header() ) {
	    // in case that the name may be different
	    strcpy( gameFile.file_name, gameDir[i]->name );
	    gameFile.file_date = gameDir[i]->time;
	    linkin(&gameFile);
	}
	file.file_close();
    }
}

//------ End of function GameFileArray::load_all_game_header --------//

//------ Begin of function sort_game_file_function ------//
//!
static int sort_game_file_function( const void *a, const void *b ) {
    return strcmp( ((GameFile*)a)->file_name, ((GameFile*)b)->file_name );
}

//------- End of function sort_game_file_function ------//

//------ Begin of function last_game_recno ------//
//!
static int last_game_recno() {
    int i;

    for( i=game_file_array.size() ; i>0 ; i-- ) {
	if( strcmp(game_file_array[i]->file_name, game_file_array.last_file_name)==0 )
	    return i;
    }

    return 0;                                       // if none of them match, return 1 as the first record
}

//------- End of function last_game_recno ------//

//------- Begin of function GameFileArray::operator[] -----//

GameFile* GameFileArray::operator[](int recNo) {
    GameFile* gameFile = (GameFile*) get(recNo);

    //	err_when(gameFile == NULL);  Let it return NULL value

    return gameFile;
}

//--------- End of function GameFileArray::operator[] ----//

//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//

//------- Begin of function GameFileArray::write_hall_of_fame ------//
//!
int GameFileArray::write_hall_of_fame() {
    int  rc;
    File file;

    // 0=don't handle error itself
    rc = file.file_create( HALL_OF_FAME_FILE_NAME, 0, 1 );

    if( !rc )
	return 0;
    // 1=allow the writing size and the read size to be different
    //--------- Write Hall of Fame ----------//

    if( rc )
	rc = file.file_write( hall_fame_array, sizeof(HallFame) * HALL_FAME_NUM );

    //------ write last saved game file name ------//

    if( rc )
	rc = file.file_write( last_file_name, MAX_PATH+1 );

    file.file_close();

    return rc;
}

//--------- End of function GameFileArray::write_hall_of_fame ------//

//------- Begin of function GameFileArray::read_hall_of_fame ------//
//!
int GameFileArray::read_hall_of_fame() {
    if( !m.is_file_exist(HALL_OF_FAME_FILE_NAME) )
	return 0;

    int  rc;
    File file;

    // 0=don't handle error itself
    rc = file.file_open( HALL_OF_FAME_FILE_NAME, 0, 1 );
    // 1=allow the writing size and the read size to be different
    if( !rc )
	return 0;
    // 1=allow the writing size and the read size to be different
    //--------- Read Hall of Fame ----------//

    if( rc )
	rc = file.file_read( hall_fame_array, sizeof(HallFame) * HALL_FAME_NUM );

    //------ read last saved game file name ------//

    if( rc )
	rc = file.file_read( last_file_name, MAX_PATH+1);

    file.file_close();

    return rc;
}

//--------- End of function GameFileArray::read_hall_of_fame ------//
