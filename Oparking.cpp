#include <OSYS.H>
#include <OCONFIG.H>
#include <OPARKING.H>
#include <OWORLDMT.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OMOUSE2.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <OBUTTON.H>
#include <OGRPHYR.H>
#include <OFACILIT.H>

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
    BG_PIC_ID = 3
};

static char parking_init_flag;
static GraphYear parking_demand;

static char *legend_label[] = {
    //	"Parking Demand",
    //	"Parking Supply",
    "Parking Supply",
    "Parking Demand",                               //## chea 120899 change them around (break @ ofaci_st.cpp ln884 can prove)
};

//------- Begin of function Parking::Parking -----------//
//!
Parking::Parking() {
    init();
}

//--------- End of function Parking::Parking -----------//

//------- Begin of function Parking::~Parking -----------//
//!
Parking::~Parking() {
    deinit();
}

//--------- End of function Parking::~Parking -----------//

//------- Begin of function Parking::deinit ---------------//
//!
void Parking::deinit() {
    parking_init_flag = 0;
    parking_demand.deinit();
}

//--------- End of function Parking::deinit ---------------//

//------- Begin of function Parking::init -----------------//
//!
void Parking::init() {
    parking_init_flag = 0;
}

//--------- End of function Parking::init -----------------//

//---------- Begin of function Parking::eval_letter_report ------------//
//! You can set the value of Parking::topic_id before running this functions
void Parking::parking_report(int refreshFlag) {
    int tab1=REPORT_X1,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    static int data_num=HISTORY_YEAR_COUNT;
    int dataArray1[HISTORY_YEAR_COUNT*2];

    vga.use_back();

    for(int i=0;i<HISTORY_YEAR_COUNT;i++) {
	dataArray1[i]=facility_office.parking_demand[i];
	dataArray1[i+HISTORY_YEAR_COUNT]=facility_office.parking_supply[i];
    }

    sys.is_institution_report=1;
    if (refreshFlag == INFO_REPAINT) {
	if(parking_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!parking_init_flag) {
	    user_interface.bg_img(BG_PIC_ID, &vga_back);
	    info.disp_column_bitmap(&vga_back);
	    parking_init_flag = 1;
	    font_charts.put(tab3+120,REPORT_Y1,"Parking Report");
	    //-------- graph title --------------//

	    user_interface.brighten(
		REPORT_X1+100, REPORT_Y1+50,
		REPORT_X2-100, REPORT_Y2-50);

	    parking_demand.init(
		REPORT_X1+100, REPORT_Y1+50,
		REPORT_X2-100, REPORT_Y2-50,
		2, &data_num, dataArray1,1,
		NULL, NULL, legend_label, 1, 1, 1, 4);
	    parking_demand.paint();
	}
    }
    parking_demand.refresh();
    //------- blt the back buffer to front buffer -------//
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//---------- End of function Development::eval_letter_report ------------//

////////////////////////////////////////////////////////////////////////////////////////////

int Parking::parking_detect() {
    int tab1=REPORT_X1,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+600;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;
    if (!parking_init_flag)
	return 0;

    return 0;
}
