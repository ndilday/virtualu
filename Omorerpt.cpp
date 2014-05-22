#include <OSYS.H>
#include <OCONFIG.H>
#include <OMORERPT.H>
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
    BG_PIC_ID = 7
};

static char more_report_init_flag;
//------- Begin of function MoreReport::MoreReport -----------//
//!
MoreReport::MoreReport() {
    init();
}

//--------- End of function MoreReport::MoreReport -----------//

//------- Begin of function MoreReport::~MoreReport -----------//
//!
MoreReport::~MoreReport() {
    deinit();
}

//--------- End of function MoreReport::~MoreReport -----------//

//------- Begin of function MoreReport::deinit ---------------//
//!
void MoreReport::deinit() {
    more_report_init_flag = 0;
}

//--------- End of function MoreReport::deinit ---------------//

//------- Begin of function MoreReport::init -----------------//
//!
void MoreReport::init() {
    more_report_init_flag = 0;
}

//--------- End of function MoreReport::init -----------------//

static char *more_report_labels[8]= {
    "admissions and financial aid [A]",
    "athletics [T]",
    "development and alumni affairs [D]",
    "facilities management [F)",
    "libraries and information technology [L]",
    "investment [I]",
    "crime [C]",
    "parking [K]",
    //	"energy",
    //	"other minor reports",
};
//---------- Begin of function MoreReport::eval_letter_report ------------//
//! You can set the value of MoreReport::topic_id before running this functions
void MoreReport::more_report_report(int refreshFlag) {
    int tab1=REPORT_X1,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+75,vtab4=REPORT_Y1+350;
    vga.use_back();

    sys.is_institution_report=1;
    if (refreshFlag == INFO_REPAINT) {
	if(more_report_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!more_report_init_flag) {
	    user_interface.bg_img(BG_PIC_ID, &vga_back);
	    info.disp_column_bitmap(&vga_back);
	    more_report_init_flag = 1;
	    font_hstone.put(tab3+40,REPORT_Y1,"SELECT A REPORT TO SEE : ");
	    for(int i=0;i<8;i++) {
		//				image_interface.put_back( tab2,vtab1+40*i, "DSPB_UP" );
		//				vga.blt_buf(tab2,vtab1+40*i,tab2+70,vtab1+40*i+28);
		font_stone.put(tab3,vtab1+40*i+20,more_report_labels[i]);
	    }
	}
    }
    //------- blt the back buffer to front buffer -------//
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//---------- End of function Development::eval_letter_report ------------//

////////////////////////////////////////////////////////////////////////////////////////////

int MoreReport::more_report_detect() {
    int tab1=REPORT_X1,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+600;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;
    if (!more_report_init_flag)
	return 0;

    for(int j=0;j<8;j++)
	font_stone.put(tab3,vtab1+40*j+20,more_report_labels[j]);

    for(int i=0;i<8;i++) {
	if(mouse.in_area(tab3,vtab1+40*i+20,tab4,vtab1+40*i+48)) {
	    font_hstone.put
		(tab3,vtab1+40*i+20,more_report_labels[i]);
	}
	if(mouse.single_click(tab3,vtab1+40*i+20,tab4,vtab1+40*i+48)) {
	    //			image_interface.put_back( tab2,vtab1+40*i, "DSPB_DN" );
	    //			vga.blt_buf(tab2,vtab1+40*i,tab2+70,vtab1+40*i+28);
	    switch(i) {
	    case 0:
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_ADMISSIONS - FIRST_NONDEPARTMENTAL_FIRM_ID );
		break;

	    case 1:
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_STADIUM - FIRST_NONDEPARTMENTAL_FIRM_ID );
		break;

	    case 2:
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_DEVELOPMENT - FIRST_NONDEPARTMENTAL_FIRM_ID );
		break;

	    case 3:
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_FACILITIES - FIRST_NONDEPARTMENTAL_FIRM_ID );
		break;

	    case 4:
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_LIBRARY - FIRST_NONDEPARTMENTAL_FIRM_ID );
		break;

	    case 5:
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_OLD_MAIN - FIRST_NONDEPARTMENTAL_FIRM_ID );
		break;

	    case 6:
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_POLICE - FIRST_NONDEPARTMENTAL_FIRM_ID );
		break;

	    case 7:
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_PARK_LOTS - FIRST_NONDEPARTMENTAL_FIRM_ID );
		break;
	    }
	}

	//	"admissions and financial aid (A)",
	//	"athletics (T)",
	//	"development and alumni affairs (D)",
	//	"facilities management (F)",
	//	"library and informantion technology (L)",
	//	"investment (I)",
	//	"crime (C)",
	//	"parking (K)",
	int keyScanCode;
	if(keyScanCode=mouse.is_key_event()) {
	    if(mouse.is_key(keyScanCode,mouse.event_skey_state,'a',K_CHAR_KEY))
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_ADMISSIONS - FIRST_NONDEPARTMENTAL_FIRM_ID );

	    if(mouse.is_key(keyScanCode,mouse.event_skey_state,'t',K_CHAR_KEY))
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_STADIUM - FIRST_NONDEPARTMENTAL_FIRM_ID );

	    if(mouse.is_key(keyScanCode,mouse.event_skey_state,'d',K_CHAR_KEY))
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_DEVELOPMENT - FIRST_NONDEPARTMENTAL_FIRM_ID );

	    if(mouse.is_key(keyScanCode,mouse.event_skey_state,'f',K_CHAR_KEY))
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_FACILITIES - FIRST_NONDEPARTMENTAL_FIRM_ID );

	    if(mouse.is_key(keyScanCode,mouse.event_skey_state,'l',K_CHAR_KEY))
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_LIBRARY - FIRST_NONDEPARTMENTAL_FIRM_ID );

	    if(mouse.is_key(keyScanCode,mouse.event_skey_state,'i',K_CHAR_KEY))
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_OLD_MAIN - FIRST_NONDEPARTMENTAL_FIRM_ID );

	    if(mouse.is_key(keyScanCode,mouse.event_skey_state,'c',K_CHAR_KEY))
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_POLICE - FIRST_NONDEPARTMENTAL_FIRM_ID );

	    if(mouse.is_key(keyScanCode,mouse.event_skey_state,'k',K_CHAR_KEY))
		sys.set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_PARK_LOTS - FIRST_NONDEPARTMENTAL_FIRM_ID );
	}
    }
    return 0;
}
