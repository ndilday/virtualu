#include <OSYS.H>
#include <OCONFIG.H>
#include <OPSCHOOL.H>
#include <OLOSEGAM.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OMOUSE2.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <OBUTTON.H>

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-55
};

enum {
    BUTTON_X0 = REPORT_X1+100,
    BOTTOM_Y1 = REPORT_Y2+25,
};

enum {
    BOTTOM_BUTTON_WIDTH = 80,
    BOTTOM_BUTTON_HEIGHT = 25,
    BOTTOM_BUTTON_DISTANCE = 10,
};

enum {                                            // the bg image id
    BG_PIC_ID = 17
};

static char lose_game_report_init_flag;
//------- Begin of function LoseGameReport::LoseGameReport -----------//
//!
LoseGameReport::LoseGameReport() {
    init();
}

//--------- End of function LoseGameReport::LoseGameReport -----------//

//------- Begin of function LoseGameReport::~LoseGameReport -----------//
//!
LoseGameReport::~LoseGameReport() {
    deinit();
}

//--------- End of function LoseGameReport::~LoseGameReport -----------//

//------- Begin of function LoseGameReport::deinit ---------------//
//!
void LoseGameReport::deinit() {
    lose_game_report_init_flag = 0;
}

//--------- End of function LoseGameReport::deinit ---------------//

//------- Begin of function LoseGameReport::init -----------------//
//!
void LoseGameReport::init() {
    lose_game_report_init_flag = 0;
    lose_reason_id = LOSE_REASON_TIME_UP;
    reason_para0 = 0;
    reason_para1 = 0;
    reason_para2 = 0;
    reason_para3 = 0;
    reason_para4 = 0;
}

//--------- End of function LoseGameReport::init -----------------//

//---------- Begin of function LoseGameReport::eval_letter_report ------------//
//! You can set the value of LoseGameReport::topic_id before running this functions
void LoseGameReport::lose_game_report(int refreshFlag) {
    int tab1=REPORT_X1+317;
    int vtab1=REPORT_Y1+135;
    vga.use_back();

    sys.is_institution_report=1;
    if (refreshFlag == INFO_REPAINT) {
	if(lose_game_report_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!lose_game_report_init_flag) {
	    user_interface.bg_img(BG_PIC_ID, &vga_back);
	    info.disp_column_bitmap(&vga_back);
	    lose_game_report_init_flag=1;
	    // BUGHERE ## chwg030299
	    switch( lose_reason_id ) {
	    case LOSE_REASON_PROTAGONIST:
		font_letter.put_paragraph( tab1+6, vtab1+6, tab1+184, vtab1+120,
					   get_reason_str_protagonist() );       // get_reason_str_protagonist() defined in OPSCH_PO.CPP
		break;

	    case LOSE_REASON_UNACCEPTABLE:
		font_letter.put_paragraph( tab1+6, vtab1+6, tab1+184, vtab1+120, "You're fired!" );
		break;

	    case LOSE_REASON_TIME_UP:                 // fall through
	    default:
		image_interface.put_back( tab1,vtab1,"LOSETXT2" );
	    }
	}
    }
    //------- blt the back buffer to front buffer -------//
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//---------- End of function Development::eval_letter_report ------------//

////////////////////////////////////////////////////////////////////////////////////////////

int LoseGameReport::lose_game_detect() {
    int tab1=REPORT_X1,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+600;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    if (!lose_game_report_init_flag)
	return 0;
    if (mouse.single_click(2))
	sys.set_staying_view_mode(MODE_FINAL_SCORE);

    return 0;
}

void LoseGameReport::set_reason(int reasonId, int para0, int para1, int para2, int para3, int para4 ) {
    lose_reason_id = reasonId;
    reason_para0 = para0;
    reason_para1 = para1;
    reason_para2 = para2;
    reason_para3 = para3;
    reason_para4 = para4;
}
