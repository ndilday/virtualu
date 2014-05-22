#include <OSYS.H>
#include <OCONFIG.H>
#include <OBANKRUP.H>
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
    BG_PIC_ID = 18
};

static char bankruptcy_report_init_flag;
//------- Begin of function BankruptcyReport::BankruptcyReport -----------//
//!
BankruptcyReport::BankruptcyReport() {
    init();
}

//--------- End of function BankruptcyReport::BankruptcyReport -----------//

//------- Begin of function BankruptcyReport::~BankruptcyReport -----------//
//!
BankruptcyReport::~BankruptcyReport() {
    deinit();
}

//--------- End of function BankruptcyReport::~BankruptcyReport -----------//

//------- Begin of function BankruptcyReport::deinit ---------------//
//!
void BankruptcyReport::deinit() {
    bankruptcy_report_init_flag = 0;
}

//--------- End of function BankruptcyReport::deinit ---------------//

//------- Begin of function BankruptcyReport::init -----------------//
//!
void BankruptcyReport::init() {
    bankruptcy_report_init_flag = 0;
}

//--------- End of function BankruptcyReport::init -----------------//

//---------- Begin of function BankruptcyReport::eval_letter_report ------------//
//! You can set the value of BankruptcyReport::topic_id before running this functions
void BankruptcyReport::bankruptcy_report(int refreshFlag) {
    int tab1=REPORT_X1,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+75,vtab4=REPORT_Y1+350;
    vga.use_back();

    sys.is_institution_report=1;
    if (refreshFlag == INFO_REPAINT) {
	if(bankruptcy_report_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!bankruptcy_report_init_flag) {
	    user_interface.bg_img(BG_PIC_ID, &vga_back);
	    info.disp_column_bitmap(&vga_back);
	    bankruptcy_report_init_flag=1;
	}
    }
    //------- blt the back buffer to front buffer -------//
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//---------- End of function Development::eval_letter_report ------------//

////////////////////////////////////////////////////////////////////////////////////////////

int BankruptcyReport::bankruptcy_detect() {
    int tab1=REPORT_X1,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+600;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    if (!bankruptcy_report_init_flag)
	return 0;
    if (mouse.single_click(2))
	sys.set_staying_view_mode(MODE_FINAL_SCORE);
    return 0;
}
