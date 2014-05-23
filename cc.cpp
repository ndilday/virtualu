//Filename    : CC.CPP
//Description : Virtual U Entry Program
//Owner		  : Kevin(ho)

#include <windows.h>
#include <ALL.H>
#include <OBATTLE.H>
#include <OCOMBUF.H>
#include <OCONFIG.H>
#include <ODATE.H>
#include <ODEBUGX.H>
#include <OGAME.H>
#include <OGAMESET.H>
#include <OIMGRES.H>
#include <OINFO.H>
#include <OMISC.H>
#include <OMATH.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OPOWER.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OWORLD.H>
#include <OTRANSL.H>
#include <OAUDIO.H>
#include <OBOX.H>
#include <OFONT.H>
#include <OGFILE.H>
#include <OVIDEO.H>
#include <OSE.H>
#include <OSERES.H>
#include <OMUSIC.H>
#include <OHELP.H>
#include <OGCONFIG.H>
#include <OSCHLRES.H>
#include <OCOURRES.H>
#include <ODEPTRES.H>
#include <OFACURES.H>
#include <OGAMESTR.H>
#include <OFPHOTO.H>
#include <OFIELD.H>
#include <OFIRMRES.H>
#include <OSPRTRES.H>
#include <OTERRAIN.H>
#include <OPLANT.H>
#include <OROAD.H>
#include <ONAMERES.H>

#include <OENROLL.H>                              //##### fred 980702
#include <ONEWS.H>                                //##### fred 981013
#include <OTASK.H>                                //##### fred 981013
#include <OCHANCE.H>                              //##### fred 981013
#include <ODEPT.H>
#include <OFINANCE.H>
#include <OPSCHOOL.H>
#include <OFIRMA.H>
//## 990511 fred	#include <OSPRITEA.H>

//########### begin Ho 0805 ############//
#include <OIFACE.H>
#include <OADM_OFF.H>
#include <OOPT.H>
#include <OOPT3.H>
#include <OATHLETI.H>
#include <ODEVELOP.H>
#include <OFACILIT.H>
#include <OLIBTECH.H>
#include <OINVEST.H>
#include <OSTUOFF.H>                              //##### fred
#include <OTEST.H>
#include <OGSET.H>
#include <OHELPIF.H>                              //## chwg1015
#include <OLETTER.H>                              //## chwg1021
#include <OBLCKBRD.H>                             //## chwg1026
#include <OMORERPT.H>                             //## chwg010799
#include <ODEPTLST.H>                             //## chwg010999
#include <OPARKING.H>                             //## chwg021299
#include <OCRIME.H>                               //## chwg021299
#include <ORETIRE.H>                              //## chwg021299
#include <OBANKRUP.H>                             //## chwg021299
#include <OFINSCOR.H>                             //## chwg021299
#include <OSCEN_IF.H>                             //## chwg021299
#include <OLOSEGAM.H>                             //## chwg021299
#include <OPRTMENU.H>                             //## chwg021299
#include <OMISCREP.H>                             //## fred0213

#include <OPRINT.H>                               //## chwg031299
#include <excpt.h>

//############# end Ho 0805 ############//

//------- define game version constant --------//

char *GAME_VERSION_STR = "2.01";

//-------- System class ----------//

#ifndef NO_MEM_CLASS
Mem   mem;                                        // constructor only init var and allocate memory
#endif

CommonBuf     common_buf;
DateInfo          date;
DebugX        debugx;
Error             err;                            // constructor only call set_new_handler()d
Misc              m;
Math          math;
// ###### begin Gilbert 26/04/2001 #######//
// modified order to arrange destructor order
Video             video;
Config            config;

//-------- objects initialized in Sys::init_directx --------//

Audio             audio;
Music             music;
SECtrl            se_ctrl(&audio);
SERes             se_res;
Vga         vga;
VgaBuf            vga_front, vga_back, vga_true_front;

//--------- objects initalized in Sys::init_objects --------//

MouseCursor       mouse_cursor;
Mouse         mouse;
Help          help;

Box               box;
Translate     translate;
UserInterface   user_interface;

//------- Resource class ----------//

Font              font_san, font_std, font_small, font_mid, font_news, font_casa;
// ##### begin chwg 24/7 ###########//
Font          font_yellow_diamond;
Font          font_fdiamond;
Font          font_fblack;
Font          font_charts;
Font          font_chartsm;
Font          font_chart_red_sm;
Font          font_chart_green_sm;
Font          font_chart_blue_sm;
Font          font_chart_orange_sm;
Font          font_chart_purple_sm;
Font          font_redcharts;
Font          font_barcharts;
Font          font_fscore;
Font          font_numbers;
Font          font_stone;
Font          font_hstone;
Font          font_small_san;
Font          font_red_san;
Font          font_green_san;
Font          font_blue_san;
Font          font_letter;
Font          font_pgold;
Font          font_psilver;
Font          font_pbronze;
// ##### end chwg 24/7 ###########//
ImageRes          image_sys, image_interface, image_mascot, image_pict;
GameSet           game_set;
Print         print;

//-------- resource initialized in Game::init --------//

TerrainRes      terrain_res;
PlantRes        plant_res;
RoadRes       road_res;

SchoolRes       school_res;
FieldRes        field_res;
DepartmentRes   department_res;
FirmRes       firm_res;
FacultyRes      faculty_res;
FacultyPhotoRes faculty_photo_res;
NameRes       name_res;
Letter        letter;
CourseRes     course_res;
EnrollRes     enroll_res;

//--------- arrays initialized in Game::init -------//

FirmArray         firm_array;
DepartmentArray department_array;

//## 990511 fred	SpriteRes			sprite_res;
GameStrRes      game_str_res;
BlackBoard      blackboard;

//-------- Game Data class -----------//

//## 990511 fred SpriteArray			sprite_array;
NewsArray     news_array;                         // ##fred 1014
TaskArray     task_array;                         // 1014
ChanceEvent     chance_event;                     // 1014

//--------- Game Surface class ------------//

PlayerSchool    player_school;
Battle            battle;
GoalConfig      goal_config;
Info              info;
Power             power;
World             world;
Finance       finance;

//########### begin Ho 0805 ############//
AdmOffice     adm_office;
Development     development_office;
Optimization    optimization;
OptStage3         opt_stage3;
Athletics     athletics_office;
LibTech       library_tech_office;
Facility        facility_office;
Investment      investment_office;
StudentOffice   student_office;                   // fred 980918
HelpIf        help_if;
GameSetting     game_setting;
MiscReport      misc_report;                      //## fred 0213
MoreReport      more_report;
DeptList        dept_list;
Crime         crime;
Parking       parking;

BankruptcyReport bankruptcy;
FinalScoreReport final_score;
ScenarioReport scenario_report;
RetirementReport retirement;
LoseGameReport lose_game;

PrintMenu     print_menu;
//DisplayModeInfo current_display_mode;
//############# end Ho 0805 ############//

// ---------------------------------------- //

Game              game;
GameFileArray     game_file_array;
GameFile          game_file;

// ---------------------------------------- //

Sys               sys;

//------- Define static functions --------//

static void extra_error_handler();
static int is_math_exception( DWORD, _EXCEPTION_POINTERS * );
static void play_video(HINSTANCE hInstance, int videoId);

//---------- Begin of function WinMain ----------//
//!
//! WinMain - initialization, message loop
//!
//! Compilation constants:
//!
//! DEBUG  - normal debugging
//!
//! DEMO  - demo version
//! ADMIN - administrator version
//! NO_CDCHECK - no CD check
//! NO_AUTOSAVE - no autosave
//!
//! Release version defines:
//! Standard version: DEBUG, NO_MEM_CLASS
//! Administrator version: DEBUG, NO_MEM_CLASS, ADMIN
//! Demo version: DEBUG, NO_MEM_CLASS, DEMO
//!
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
		    LPSTR lpCmdLine, int nCmdShow) {
    //------ set game directory and check CD ------//

    sys.set_game_dir();

#ifndef NO_CDCHECK

#ifndef DEMO
    if( !sys.cdrom_drive ) {
#ifdef ADMIN
	char* msg = "Please insert Virtual U - Administrator Version CDROM and try again.";
#else
	char* msg = "Please insert Virtual U CDROM and try again.";
#endif

	MessageBox(sys.main_hwnd, msg, WIN_TITLE, MB_OK | MB_ICONERROR);
	return 0;
    }
#endif
#endif

    //----------- play movie ---------------//

    OSVERSIONINFO osVersion;
    memset( &osVersion, 0, sizeof(osVersion) );
    // do not play movie in Win2000
    osVersion.dwOSVersionInfoSize = sizeof(osVersion);

    if( !m.is_file_exist("SKIPAVI.SYS")
	&& GetVersionEx(&osVersion) && osVersion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) {
	play_video(hInstance, 0);
    }

    config.init();

  //--------------------------------------//

    if( !sys.init(hInstance) )
	return FALSE;

    err.set_extra_handler( extra_error_handler );   // set extra error handler, save the game when a error happens

#ifdef DEBUG
    // break at exception, for debugging
    if( sys.debug_session ) {
	// directX may change fpu control word, so change after sys.init
	// all exception but no inexact, underflow and denormal number exception
	_control87( _EM_INEXACT | _EM_UNDERFLOW | _EM_DENORMAL, _MCW_EM );
    }
#endif                                          // DEBUG

    game.main_menu();

    sys.deinit();

    return 0;
}

//---------- End of function WinMain ----------//

//----------- Begin of function Msg ----------//
//!
//! Routine for displaying debug messages
//!

#ifdef DEBUG

void __cdecl debug_msg( char* fmt, ... ) {
    char buff[256];

    lstrcpy( buff, "Virtual U: " );
    wvsprintf( buff+lstrlen(buff), fmt, (char*)(&fmt+1) );
    lstrcat( buff, "\r\n" );

    OutputDebugString( buff );
}
#endif

//----------- End of function Msg ----------//

//------- Begin of function extra_error_handler -----------//

static void extra_error_handler() {
    /*
      if( game.game_mode != GAME_SINGLE_PLAYER )
      return;

      game_file_array.save_new_game("ERROR.SAV");  // save a new game immediately without prompting menu

      box.msg( "Error encountered. The game has been saved to ERROR.SAV" );
    */
}

//----------- End of function extra_error_handler -------------//

//--------- Begin of function play_video --------------//

static void play_video(HINSTANCE hInstance, int videoId) {
    String movieFileStr;
    //	movieFileStr = DIR_MOVIE;
    if( videoId == 0 ) {
	movieFileStr += "INTRO.AVI";
    }
    else {
	movieFileStr += "MOVIE";
	movieFileStr += videoId;
	movieFileStr += ".AVI";
    }

    video.set_skip_on_fail();

    if( m.is_file_exist(movieFileStr) ) {
	//---------- play the movie now ---------//

	video.init();

	if( video.init_success ) {
	    video.play_until_end( movieFileStr, hInstance, 60 );
	}
	else {
	    // display a message box (note:sys.main_hwnd is not valid)
	    // MessageBox( NULL, "Cannot initialize ActiveMovie",
	    //   "Virtual U", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1 | MB_TASKMODAL );
	}

	video.deinit();
    }
}

//--------- End of function play_video --------------//

// value from GetExceptionCode()
static int is_math_exception( DWORD exceptionCode, _EXCEPTION_POINTERS *exceptInfo ) {
    // use in this way eg
    // __try
    // {
    // ....
    // }
    // __except( is_math_exception( GetExceptionCode(), GetExceptionInformation() )
    // {
    // }

    switch( exceptionCode ) {
    case EXCEPTION_FLT_INVALID_OPERATION:
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
    case EXCEPTION_INT_OVERFLOW: {
	void *pAddr =  exceptInfo->ExceptionRecord->ExceptionAddress;
	err.msg( "math exception at %p", pAddr );
    }
    return EXCEPTION_EXECUTE_HANDLER;
    default:
	return EXCEPTION_CONTINUE_SEARCH;
    }
}
