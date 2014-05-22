//Filename    : OSYS.CPP
//Description : System resource management object

#include <resource.h>
#include <ALL.H>
#include <OCONFIG.H>
#include <OIMGRES.H>
#include <OGAMESET.H>
#include <OGCONFIG.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OAUDIO.H>
#include <OMUSIC.H>
#include <OSE.H>
#include <OFONT.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OGAME.H>
#include <OMISC.H>
#include <OWORLD.H>
#include <OHELP.H>
#include <OPRINT.H>
#include <ODEBUGX.H>
#define DEBUG_LOG_LOCAL 1
#include <OLOG.H>

//----------- Declare static functions -----------//

static long FAR PASCAL static_main_win_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK win_hook_proc(int nCode, WORD wParam, LONG lParam);

//--------- Begin of static function static_main_win_proc --------//
// Callback for all Windows messages
//
static long FAR PASCAL static_main_win_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    return sys.main_win_proc(hWnd, message, wParam, lParam);
}

//--------- End of static function static_main_win_proc --------//

//----------- Begin of function Sys::Sys -----------//
Sys::Sys() {
    memset(this, 0, sizeof(Sys));

    common_data_buf = mem_add( COMMON_DATA_BUF_SIZE );

    view_mode = MODE_NORMAL;                        // the animation mode

    prev_view_mode = MODE_NORMAL;

    save_view_mode = MODE_NORMAL;                   // ## chwg 1106
    sys.save_speed=9;                               // ## chwg 1109
    back_view_mode = MODE_NORMAL;

    redraw_all_flag = 0;
    news_pop_up_flag = 0;
    redraw_zoom_flag = 0;
    //##begin zhoubin 02/23/99
    old_Dept=-1;
    //##end zhoubin 02/23/99
    // ## begin chwg1008
    //	set_speed( 3*3 );		// set to normal speed
    // ## end chwg1008

}

//----------- End of function Sys::Sys -----------//

//----------- Begin of function Sys::~Sys -----------//
Sys::~Sys() {
    mem_del(common_data_buf);

    deinit();
}

//----------- End of function Sys::~Sys -----------//

//------------ Begin of function Sys::init ----------//
int Sys::init( HANDLE hInstance ) {
    err_when( init_flag );

    //------- initialize basic vars --------//
    app_hinstance = (HINSTANCE)hInstance;

#ifdef BETA
    debug_session       = m.is_file_exist("DEBUG.SYS");
    testing_session     = m.is_file_exist("TESTING.SYS");
#endif

#ifdef DEBUG
    // Ben has put debug.sys in download version, so change to use DEBUGCC.SYS
    debug_session       = m.is_file_exist("DEBUGCC.SYS");
    testing_session     = m.is_file_exist("TESTING.SYS");
#endif

    // add in (GAME_VERSION>=200)
    use_true_front      = debug_session;

    // Ben what Win2K winXP has no lock buffer problem, so force to 1 to turn on triple buffer (1 primary + 2 system memory surfaces)
    use_true_front = 1;

    if( m.is_file_exist("WINMODE.SYS") )
	use_true_front = 2;                           // window mode

    set_game_dir();                                 // set game directories names and game version

    //------- initialize more stuff ---------//
    if( !init_win() )
	return FALSE;

    if( !init_directx() )
	return FALSE;

    if( !init_objects() )                           // initialize system objects which do not change from games to games.
	return FALSE;

    init_flag = 1;

    return TRUE;
}

//------------ End of function Sys::init ----------//

//-------- Begin of function Sys::deinit --------//
//! Finished with all objects we use; release them
//!
void Sys::deinit() {
    if( !init_flag )
	return;

    game.deinit();                                  // actually game.deinit() will be called by main_win_proc() and calling it here will have no effect
    deinit_objects();

    //-----------------------------------------------//

    unsigned curTime = m.get_time();

    while( m.get_time() < curTime + 500 ) {
	if( vga_back.buf_locked )
	    vga_back.unlock_buf();

	if( vga_front.buf_locked )
	    vga_front.unlock_buf();
    }

    //-----------------------------------------------//

    //### begin zb 02/24/99
    //	PostMessage(main_hwnd, WM_CLOSE, 0, 0);
    deinit_directx();
    DestroyWindow(sys.main_hwnd);
    //### end zb 02/24/99

    init_flag = 0;

    MSG msg;

    while( GetMessage(&msg, NULL, 0, 0) ) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }
}

//--------- End of function Sys::deinit ---------//

//-------- Begin of function Sys::init_win --------//
int Sys::init_win() {
    //--------- register window class --------//
    WNDCLASS    wc;
    BOOL        rc;

    wc.style          = CS_DBLCLKS;
    wc.lpfnWndProc    = static_main_win_proc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = app_hinstance;
    wc.hIcon          = LoadIcon( app_hinstance, MAKEINTATOM(IDI_ICON2));
    wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = WIN_CLASS_NAME;

    rc = RegisterClass( &wc );

    if( !rc )
	return FALSE;

    //--------- create window -----------//

    if( use_true_front == 2 ) {                     // window mode
	DWORD exStyle = 0;
	DWORD style = WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_MINIMIZEBOX | WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE;

	RECT windowArea = { 0, 0, 0+VGA_WIDTH, 0+VGA_HEIGHT };
	AdjustWindowRectEx( &windowArea, style, NULL, exStyle );

	main_hwnd = CreateWindowEx(
	    exStyle,
	    WIN_CLASS_NAME,
	    WIN_TITLE,
	    style,
	    windowArea.left,
	    windowArea.top,
	    windowArea.right-windowArea.left,
	    windowArea.bottom-windowArea.top,
	    NULL,
	    NULL,
	    app_hinstance,
	    NULL );
    }
    else {
	main_hwnd = CreateWindowEx(
	    // ## begin zb 02/22/99
#ifdef _DEBUG
	    NULL,
#else
	    WS_EX_APPWINDOW | WS_EX_TOPMOST,
#endif
	    // ## end zb 02/22/99
	    WIN_CLASS_NAME,
	    WIN_TITLE,
	    WS_VISIBLE |                                // so we dont have to call ShowWindow
	    WS_POPUP,
	    0,
	    0,
	    GetSystemMetrics(SM_CXSCREEN),
	    GetSystemMetrics(SM_CYSCREEN),
	    NULL,
	    NULL,
	    app_hinstance,
	    NULL );
    }

    if( !main_hwnd )
	return FALSE;

    UpdateWindow( main_hwnd );
    SetFocus( main_hwnd );

    return TRUE;
}

//-------- End of function Sys::init_win --------//

//-------- Begin of function Sys::init_directx --------//
int Sys::init_directx() {
    audio.init();
    music.init();
    //	se_ctrl.init();

    //---------------------------------------//

    ShowCursor(FALSE);

    //-------- initialize DirectDraw --------//

    if( !vga.init() )
	return FALSE;

    vga.load_pal(DIR_RES"PAL_STD.RES");

    // add in (GAME_VERSION>=200)
    //	if( debugx.debug_session )                // if we are currently in a debug session, don't lock the front buffer otherwise the system will hang up
    //	{
    //		vga_front.init_back( vga.dd_obj );
    //		vga_front.is_front = 1;			// set it to 1, overriding the setting in init_back()
    //
    //		vga_true_front.init_front( vga.dd_obj );
    //
    //		vga.activate_pal(&vga_true_front);
    //	}
    //	else
    //	{
    //		vga_front.init_front( vga.dd_obj );
    //		vga.activate_pal(&vga_front);
    //	}

    if( !use_true_front ) {
	// try lock front buffer

	vga_front.init_front( vga.dd_obj );
	DDSURFACEDESC2  ddsd;
	memset( &ddsd, 0, sizeof(ddsd) );
	ddsd.dwSize = sizeof(ddsd);
	if( vga_front.dd_buf->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL) == DD_OK ) {
	    vga_front.dd_buf->Unlock(NULL);
	    vga.activate_pal(&vga_front);
	}
	else {
	    vga_front.deinit();
	    use_true_front = 1;                         // can't lock buffer, use vga_true_front
	}
    }

    if( use_true_front ) {
	vga_true_front.init_front( vga.dd_obj );

	vga_front.init_back( vga.dd_obj );
	vga_front.is_front = 1;                       // set it to 1, overriding the setting in init_back()

	vga.activate_pal(&vga_true_front);
    }

    vga_back.init_back( vga.dd_obj );
    vga_front.lock_buf();
    vga_back.lock_buf();

    return TRUE;
}

//-------- End of function Sys::init_directx --------//

//-------- Begin of function Sys::deinit_directx --------//
void Sys::deinit_directx() {
    if( !vga.is_inited() )
	return;

    if( vga_back.dd_buf && vga_back.buf_locked )
	vga_back.unlock_buf();

    if( vga_front.dd_buf && vga_front.buf_locked )
	vga_front.unlock_buf();

    vga_back.deinit();

    // add in (GAME_VERSION>=200)
    //   if( debugx.debug_session )
    if( use_true_front )
	vga_true_front.deinit();

    vga_front.deinit();
    vga.deinit();

    //------------------------------------//

    //	se_ctrl.deinit();
    music.deinit();
    audio.deinit();
}

//--------- End of function Sys::deinit_directx ---------//

//------- Begin of function Sys::init_objects -----------//
//! Initialize system objects which do not change from games to games.
//!
int Sys::init_objects() {
    //--------- init system class ----------//

    mouse_cursor.init();
    mouse_cursor.set_frame_border(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);

    mouse.init(app_hinstance, main_hwnd, NULL);

    //------- init resource class ----------//
    help.init("CTXTHELP.RES");
    font_std.init("STD", 2);
    font_san.init("SAN", 0);                        // 0-zero inter-character space
    //	font_red_san.init("RSAN", 0);
    //	font_green_san.init("GSAN", 0);
    //	font_blue_san.init("BSAN", 0);
    //	font_san.init("CHRM", 0);		// 0-zero inter-character space
    font_red_san.init("RSAN", 0);
    font_green_san.init("GSAN", 0);
    font_blue_san.init("BSAN", 0);
    font_fdiamond.init("FDMD", 1);
    font_yellow_diamond.init("YDMD", 1);
    //	font_fblack.init("BLCK", 0);
    //	font_charts.init("CHRT", 0);
    font_charts.init("CHRT", 0);

    // font file behind is alternate font file used in printing
    font_chartsm.init("CHRM", 1);                   // CHRP
    font_chart_red_sm.init("RCSM", 1);              // RCSP
    font_chart_green_sm.init("GCSM", 1);            // GCSP
    font_chart_blue_sm.init("BCSM", 1);             // BCSP

    font_chart_orange_sm.init("OCSM", 1);
    font_chart_purple_sm.init("PCSM", 1);           // PCSP in ver2
    font_numbers.init("FNUM", 1);
    font_redcharts.init("RCHR", 0);
    font_barcharts.init("BCHR", 0);
    font_letter.init("LTTR", 0);
    font_fscore.init("FSCR", 0);
    font_pgold.init("PGLD", 0);
    font_psilver.init("PSLV", 0);
    font_pbronze.init("PBRZ", 0);
    font_stone.init("FSTN", 0);
    font_hstone.init("FHSN", 0);
    font_fblack.init("WBLK", 0);
    font_small_san.init("SMAL", 0);
    font_mid.init("MID");
    font_small.init("SMAL");
    font_news.init("NEWS");
    font_casa.init("CASA", 1, 3);

    image_sys.init(DIR_RES"I_SYS.RES",1,0);         // 1-read into buffer
    image_interface.init(DIR_RES"I_IF.RES",0,0);    // 0-don't read into the buffer, don't use common buffer
    image_mascot.init(DIR_RES"I_MASCOT.RES",0,0);
    image_pict.init(DIR_RES"I_PICT.RES",0,0);

    //---------- init other objects ----------//

    game_set.init();                                // this must be called before game.init() as game.init() assume game_set has been initialized
    goal_config.init();

    // #### begin Gilbert 27/04/2001 ######//
    print.init();
    // #### end Gilbert 27/04/2001 ######//

    return TRUE;
}

//------- End of function Sys::init_objects -----------//

//------- Begin of function Sys::deinit_objects -----------//

void Sys::deinit_objects() {
    //--------- deinit system class ----------//

    mouse.deinit();                                 // mouse must be deinitialized first
    mouse_cursor.deinit();

    //------- deinit resource class ----------//

    font_std.deinit();
    font_san.deinit();
    font_charts.deinit();
    font_chartsm.deinit();
    font_chart_red_sm.deinit();
    font_chart_green_sm.deinit();
    font_chart_blue_sm.deinit();
    font_chart_orange_sm.deinit();
    font_numbers.deinit();
    font_chart_purple_sm.deinit();
    font_redcharts.deinit();
    font_barcharts.deinit();
    font_letter.deinit();
    font_fscore.deinit();
    font_pgold.deinit();
    font_psilver.deinit();
    font_pbronze.deinit();
    font_stone.deinit();
    font_hstone.deinit();
    font_small_san.deinit();
    font_red_san.deinit();
    font_fdiamond.deinit();
    font_yellow_diamond.deinit();
    font_red_san.deinit();
    font_green_san.deinit();
    font_blue_san.deinit();
    font_mid.deinit();
    font_small.deinit();
    font_news.deinit();
    font_casa.deinit();

    image_sys.deinit();
    image_interface.deinit();
    image_mascot.deinit();
    image_pict.deinit();

    //--------- deinit other objects ----------//

    game_set.deinit();
    // #### begin Gilbert 27/04/2001 ######//
    print.deinit();
    // #### end Gilbert 27/04/2001 ######//
    goal_config.deinit();
    //	help.deinit();
    //	tutor.deinit();
    config.deinit();
    //	game_file_array.deinit();

}

//------- End of function Sys::deinit_objects -----------//

//-------- Begin of function Sys::set_game_dir ----------//
//!
//! Set all game directories.
//!
void Sys::set_game_dir() {
    //------- If it should run from the CDROM ------//

    get_cdrom_drive();

    set_one_dir( "IMAGE\\HALLFAME.ICN"    , "IMAGE\\", dir_image );
    set_one_dir( "ENCYC\\SEAT\\NORMAN.ICN", "ENCYC\\", dir_encyc );
    set_one_dir( "MOVIE\\INTRO.AVI"       , "MOVIE\\", dir_movie );

#ifdef DEMO
    set_one_dir( "TUTORIAL\\STANDARD.TUT" , "TUTORIAL\\", dir_tutorial );
    set_one_dir( "SCENARIO\\DEMO.SCN"     , "SCENARIO\\", dir_scenario );
#else
    set_one_dir( "TUTORIAL\\1BAS_MIL.TUT" , "TUTORIAL\\", dir_tutorial );
    set_one_dir( "SCENARIO\\7FOR7.SCN"    , "SCENARIO\\", dir_scenario );
#endif

    //-------- set game version ---------//

#ifdef BETA
    game_version = VERSION_FULL;
#else
#ifdef DEMO
    game_version = VERSION_DEMO;
#else
    game_version = VERSION_FULL;                    // single player game is not available when game_version == VERSION_FULL
#endif
#endif
}

//----------- End of function Sys::set_game_dir ----------//

//-------- Begin of function Sys::set_one_dir ----------//
//!
int Sys::set_one_dir( char* checkFileName, char* defaultDir, char* trueDir ) {
    if( m.is_file_exist( checkFileName ) ) {
	strcpy( trueDir, defaultDir );
    }
    else {
	if( cdrom_drive ) {
	    strcpy( trueDir, "D:\\" );
	    strcat( trueDir, defaultDir );

	    trueDir[0] = cdrom_drive;
	}
	else {
	    strcpy( trueDir, "" );
	    return 0;
	}
    }

    return 1;
}

//----------- End of function Sys::set_one_dir ----------//

//-------- Start of function Sys::get_cdrom_drive -------------//
//
// Get the drive letter of the CDROM and restore the result in cdrom_drive.
//
void Sys::get_cdrom_drive() {
    unsigned char  i;
    char  driveStr[4];

    cdrom_drive = 0;

    // don't check at all
    return;
}

//--------- End of function Sys::get_cdrom_drive ---------------//
