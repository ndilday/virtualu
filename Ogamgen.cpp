//Filename    : OGAMGEN.CPP
//Description : Game Generator

#include <OVGA.H>
#include <OBOX.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OBUTTON.H>
#include <OLISTBOX.H>
#include <OPOWER.H>
#include <OWORLD.H>
#include <OITEMRES.H>
#include <OPERSON.H>
#include <OGRPRES.H>
#include <OGAMESET.H>
#include <OGAME.H>
#include <OHELPIF.H>

//--------- Define constant ------------//

enum {
    GEN_WIN_X1 = 45,
    GEN_WIN_Y1 = 30,
    GEN_WIN_X2 = 595,
    GEN_WIN_Y2 = 445
};

enum {
    GEN_HEADER_X1 = GEN_WIN_X1+10,
    GEN_HEADER_Y1 = GEN_WIN_Y1+10,
    GEN_HEADER_X2 = GEN_HEADER_X1+363,
    GEN_HEADER_Y2 = GEN_WIN_Y1+107
};

enum {
    GEN_TYPE_X1 = GEN_HEADER_X2+10,
    GEN_TYPE_Y1 = GEN_HEADER_Y1,
    GEN_TYPE_X2 = GEN_WIN_X2-10,
    GEN_TYPE_Y2 = GEN_HEADER_Y2
};

enum {
    GEN_BODY_X1 = GEN_WIN_X1+10,
    GEN_BODY_Y1 = GEN_HEADER_Y2+10,
    GEN_BODY_X2 = GEN_WIN_X2-10,
    GEN_BODY_Y2 = GEN_WIN_Y2-32
};

//-------- Define game settings ---------//

enum { OPTION_TYPE_NUM = 2 };

static char* option_des_array[] = {
    "Institution",                                  // 1
    "Characteristics",                              // 2
};

static char* screen_code_array[] = {
    "G_INST",                                       // 1
    "G_CHAR",                                       // 2
};

static short  normal_option_array[] = { 1, 2 };
static short  disp_option_array[]   = { 1, 2 };

//-------- Define static variables ---------//

static Button  button_reset, button_ok, button_cancel;
static ListBox type_list;
static short   gen_mode;
static short   option_type;

static void    disp_type_rec(int,int,int,int);

//------- Begin of function Game::generator -------//
//!
//! Game Generator Menu
//!
//! [int] genMode : GEN_MODE_NEW    - new game generation
//!                 GEN_MODE_DISP   - display the game settings only, without user input
//!
//! return : <int> 1 - ok, the game is generated and run
//!                0 - cancel
//!
int Game::generator(int genMode) {
    //----- the settings available to change -----//

    int optionNum;

    switch(genMode) {
    case GEN_MODE_NEW:
	avail_option_array = normal_option_array;
	optionNum = 2;
	break;

    case GEN_MODE_DISP:
	avail_option_array = disp_option_array;
	optionNum = 2;
	break;
    }

    gen_mode = genMode;

  //--------- paint window -------------//

    power.open_report( GEN_WIN_X1, GEN_WIN_Y1, GEN_WIN_X2, GEN_WIN_Y2 );

    //---------- Window Title ------------//

    vga.d3_panel_down( GEN_HEADER_X1, GEN_HEADER_Y1, GEN_TYPE_X1-10, GEN_HEADER_Y2, 2 );

    icon_title.put( GEN_HEADER_X1+2, GEN_HEADER_Y1+2, "GAMESET" );

  //-------- option type selection --------------//

    type_list.init(GEN_TYPE_X1, GEN_TYPE_Y1, GEN_TYPE_X2, GEN_TYPE_Y2,
		   optionNum, disp_type_rec );

    type_list.open(option_type);

  //---------- create buttons --------------//

    if( gen_mode==GEN_MODE_DISP ) {
	button_ok.paint_text( GEN_WIN_X1+250, GEN_WIN_Y2-25, "Ok" );
    }
    else {
	int x = GEN_WIN_X1 + 168;

	button_reset.paint_text( x+53 , GEN_WIN_Y2-25, "Reset" );

	button_ok     .paint_text( x+105, GEN_WIN_Y2-25, "Ok"      );
	button_cancel .paint_text( x+145, GEN_WIN_Y2-25, "Cancel"  );
    }

    //------------ detect loop ---------------//

    int rc=1;

    gen_disp();

    while( !button_ok.detect(KEY_RETURN) ) {
	mouse.get_event();

	//--------- detect option type ----------//

	if( type_list.detect() > 0 ) {
	    option_type = type_list.recno();
	    gen_disp();
	}

	//---------- detect controls ------------//

	if( gen_mode == GEN_MODE_DISP ) {             // if display only
	    if( mouse.single_click(1) )                 // right button to quit
		break;
	}
	else {
	    //-------- detect option body ---------//

	    gen_detect();

	    //-------- detect cancel button ---------//

	    if( button_cancel.detect(KEY_ESC) ) {
		rc = 0;
		break;
	    }

	    //---- set all game settings to their default values ----//

	    if( button_reset.detect() ) {
		if( box.ask( "Reset all settings to their default values ?" ) ) {
		    default_setting();
		    gen_disp();
		}
	    }
	}
    }

    //------------------------------------------------//

    power.close_report();

    return rc;
}

//-------- End of function Game::generator -------//

//------- Begin of function Game::gen_disp -------//
//!
//! [int] refreshFlag = refresh flag (default:INFO_REPAINT)
//!
void Game::gen_disp(int refreshFlag) {
    // 1 use texture background
    if( refreshFlag==INFO_REPAINT && avail_option_array[option_type-1]!=1 )
	vga.d3_panel_down( GEN_BODY_X1, GEN_BODY_Y1, GEN_BODY_X2, GEN_BODY_Y2, 2, 1, "T_GAMGEN" );

    switch( avail_option_array[option_type-1] ) {
    case 1:                                       // Institution
	// 1-display and generate the map, rather than only generate it
	gen_institution( GEN_BODY_X1, GEN_BODY_Y1, GEN_BODY_X2, GEN_BODY_Y2, 1 );
	break;

    case 2:                                       // Characteristics
	gen_characteristics( GEN_BODY_X1, GEN_BODY_Y1, GEN_BODY_X2, GEN_BODY_Y2, gen_mode );
	break;
    }
}

//-------- End of function Game::gen_disp -------//

//------- Begin of function Game::gen_detect -------//
//!
void Game::gen_detect() {
    switch( avail_option_array[option_type-1] ) {
    case 1:                                       // Institution
	gen_institution_detect();
	break;

    case 2:                                       // Characteristics
	gen_characteristics_detect();
	break;
    }
}

//-------- End of function Game::gen_detect -------//

//-------- Begin of static function disp_type_rec --------//
//
static void disp_type_rec(int recNo, int x1, int y1, int x2) {
    int optionId = avail_option_array[recNo-1];

    font_std.put( x1, y1, option_des_array[optionId-1], -1, 0, x2 );
}

//----------- End of static function disp_type_rec -----------//
