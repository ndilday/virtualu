//Filename    : OGAM_IF2.H
//Description : Game Class Definition (Load game)
//Owner       : Fred

#include <OSYS.H>
#include <OLOG.H>
#include <OMOUSE.H>
#include <OVGA.H>

#include <OWORLDMT.H>                             // ZOOM_Y1, etc
#include <OIFACE.H>
#include <OGAME.H>
#include <OGFILE.H>

#include <OVBROW98.H>
#include <OBUTTON.H>
#include <OVGALOCK.H>

/*
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>

#include <OGAMESET.H>
#include <OGSET.H>

#include <OINFO.H>
*/

enum {
    REPORT_X1 = ZOOM_X1 + 15,
    REPORT_Y1 = ZOOM_Y1 + 15,
    REPORT_X2 = ZOOM_X2 - 15,
    REPORT_Y2 = ZOOM_Y2 - 40
};

enum {
    TMENU_X1 = REPORT_X1 + 5,
    TMENU_Y1 = REPORT_Y1 + 35,
    TMENU_X2 = REPORT_X2 - 5,
    TMENU_Y2 = REPORT_Y2 - 5
};

enum { TAB_SCREEN,SAVE_EXPRESS};

static int current_report_mode=TAB_SCREEN;
static int button_timing = 0;

static Button button_start,button_cancel;
static char * button_label[]={"Load Game","Cancel"};
static Button button_browse;

static Button button_left,button_right,button_add,button_remove,button_reset,button_addall;

static char screen_init_flag=0;
static char redraw_browser_flag=0;
static int pic_id=9;

enum {                                            // screen coordinates
    WIN_X1 = REPORT_X1+15,
    WIN_Y1 = REPORT_Y1+25,
    WIN_X2 = REPORT_X2-15,
    WIN_Y2 = REPORT_Y2-25
};

static int browser_rec_size;
static VBrowse98 game_browser;

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//-----------------------------------------------------------------//

static void disp_load_game_rec(int recNo, int x, int y, int refreshFlag) {
    if((recNo>0)&&(recNo<=game_file_array.size())) {
	GameFile* gfPtr = game_file_array[recNo];
	char* name = gfPtr->get_save_name();
	font_chartsm.put(x,y,name);
    }
};

//------- Begin of function Game::loadgame_init ------//
//!
void Game::loadgame_init() {
    screen_init_flag = 0;

    game_file_array.init("*.SAV");
    browser_rec_size=game_file_array.size();
}

void Game::loadgame_deinit() {
    screen_init_flag = 0;
}

//------- End of function Game::loadgame_init ------//

//------- Begin of function Game::loadgame_report ------//
//!
void Game::loadgame_report(int refreshFlag) {
    static const int  tab1=WIN_X1+10,
	tab2=WIN_X1+410,
	vtab0=WIN_Y1+10,
	vtab1=WIN_Y1+60,
	vtab2=WIN_Y2-30;

    vga.use_back();

    if(current_report_mode==TAB_SCREEN) {
	if((refreshFlag == INFO_REPAINT)) {
	    user_interface.bg_img(pic_id, &vga_back);
	    font_charts.put(tab1+275, vtab0,"Saved Game List");

	    if (!screen_init_flag) {
		button_start.create_text
		    (REPORT_X1+350-250, REPORT_Y2+15,
		     REPORT_X1+350-100, REPORT_Y2+35, button_label[0]);
		button_cancel.create_text
		    (REPORT_X1+350-75, REPORT_Y2+15,
		     REPORT_X1+350+75, REPORT_Y2+35, button_label[1]);
		button_browse.create_text
		    (REPORT_X1+350+100, REPORT_Y2+15,
		     REPORT_X1+350+250, REPORT_Y2+35, "Browse");

		screen_init_flag = 1;
		user_interface.brighten(tab1+100, vtab1+10, tab1+550, vtab2-20);
		user_interface.rect(tab1+100, vtab1+10, tab1+550, vtab2-20);
		game_browser.init( tab1+100, vtab1+10, tab1+550, vtab2-20,
				   0, 18, browser_rec_size, disp_load_game_rec );
		game_browser.open(game_browser.recno());
		vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
	    }
	    else {
		//				user_interface.brighten(tab1+100, vtab1+10, tab1+550, vtab2-20);
		//				user_interface.rect(tab1+100, vtab1+10, tab1+550, vtab2-20);
		user_interface.brighten(tab1+100, vtab1+10, tab1+550, vtab2-20);
		user_interface.rect(tab1+100, vtab1+10, tab1+550, vtab2-20);
		//				vga.use_back();
		game_browser.update();
		//				vga.blt_buf(tab1+100, vtab1+10, tab1+525, vtab2-20);
	    }

	    // chwg032999
	    // begin
	    button_start.paint();
	    button_cancel.paint();
	    button_browse.paint();
	    vga.blt_buf
		(REPORT_X1+350-250, REPORT_Y2+15,
		 REPORT_X1+350-100, REPORT_Y2+35);
	    vga.blt_buf
		(REPORT_X1+350-75, REPORT_Y2+15,
		 REPORT_X1+350+75, REPORT_Y2+35);
	    vga.blt_buf
		(REPORT_X1+350+100, REPORT_Y2+15,
		 REPORT_X1+350+250, REPORT_Y2+35);

	    //			vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
	    // end
	}
	else {
	}

	//		button_start.paint();
	//		button_cancel.paint();
	// chwg032999
	// begin
	//		vga.blt_buf(REPORT_X1+150, REPORT_Y2+15,
	//						REPORT_X1+300, REPORT_Y2+35);
	//		vga.blt_buf(REPORT_X1+400, REPORT_Y2+15,
	//						REPORT_X1+550, REPORT_Y2+35);
	sys.yield();
	// end
    }

    //	vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);		// DON'T need to blt all area...
}

//------- End of function GameSetting::loadgame_report ------//

//------ Begin of function GameSetting::loadgame_detect ------//
//!
//! return: <int> 1  - the Start button is pressed.
//! 				  -1 - the Cancel button is pressed.
//!					  0  - neither of them is pressed.
//!
int Game::loadgame_detect() {
    int rc=0;
    vga.use_back();

    if(current_report_mode==TAB_SCREEN) {
	vga.use_front();

	if( button_start.detect() ) {
	    if ( game_file_array.size()>=game_browser.recno() && game_browser.recno()>0) {
		game_file_array[game_browser.recno()]->load_game();
		//sys.redraw_all_flag=1;
		//menu_pull_down = 0;
		sys.set_staying_view_mode(0);
		sys.redraw_all_flag=1;
		rc = 1;
	    }

	}

	if( button_cancel.detect() )
	    rc = -1;

	if( button_browse.detect() ) {
	    char loadFileName[MAX_PATH] = "";
	    BOOL retValue = FALSE; {
		MouseDispCount mouseDispCount;            // redisplay windows cursor

		// ask filename now
		OPENFILENAME openFileName;
		memset( &openFileName, 0, sizeof(openFileName) );
		openFileName.lStructSize = sizeof(openFileName);
		openFileName.hwndOwner = sys.main_hwnd;
		openFileName.hInstance = sys.app_hinstance;
		// 2 '\0' at the end
		openFileName.lpstrFilter = "All Files(*.*)\0*.*\0";
		openFileName.lpstrCustomFilter = NULL;
		openFileName.nMaxCustFilter = 0;
		openFileName.nFilterIndex = 0;
		openFileName.lpstrFile =  loadFileName;   // first char is already NULL
		openFileName.nMaxFile = sizeof(loadFileName);
		openFileName.lpstrFileTitle = NULL;       // result filename, without path
		openFileName.nMaxFileTitle = 0;
		openFileName.lpstrInitialDir = ".";       // current directory
		openFileName.lpstrTitle = NULL;
		openFileName.Flags = OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		openFileName.nFileOffset = 0;
		openFileName.nFileExtension = 0;
		openFileName.lpstrDefExt = "001";
		openFileName.lCustData = NULL;
		openFileName.lpfnHook = NULL;
		openFileName.lpTemplateName = NULL;

		retValue = GetOpenFileName(&openFileName);
	    }                                           // call mouseDispCount destructor

	    if( retValue ) {
		game_file.load_game(loadFileName);
		//sys.redraw_all_flag=1;
		//menu_pull_down = 0;
		sys.set_staying_view_mode(0);
		sys.redraw_all_flag=1;
		rc = 1;
	    }
	}

	vga.use_back();                               // why do this here? before vbrowse??

	if(int rec=game_browser.detect()) {
	    game_browser.refresh(rec);
	    if(game_browser.double_click) {
		if (game_file_array.size()>=rec && rec>0) {
		    game_file_array[rec]->load_game();
		    //sys.redraw_all_flag=1;
		    //menu_pull_down = 0;
		    sys.set_staying_view_mode(0);           // 0: restore last saved mode which is not "staying"
		    sys.redraw_all_flag=1;
		    return 1;
		}
	    }
	    sys.redraw_all_flag=1;
	}
    }
    vga.use_back();

    return rc;
}

//------- End of function GameSetting::loadgame_detect ------//
