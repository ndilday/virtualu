#include <OSYS.H>
#include <OCONFIG.H>
#include <OCRIME.H>
#include <OWORLDMT.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OMOUSE2.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <OGRPHMTH.H>
#include <OFACILIT.H>
#include <OBUTTON.H>
#include <OSPINNER.H>
#include <OFINANCE.H>

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

enum {
    MODE_FINANCIAL_BOX1_HEIGHT = 10,
    MODE_FINANCIAL_BOX2_HEIGHT = 270,
    MODE_FINANCIAL_GRAPH_WIDTH = 400,
    MODE_FINANCIAL_GRAPH_HEIGHT = 160,
    MODE_FINANCIAL_LEGEND_HEIGHT = 26,
    MODE_FINANCIAL_LEGEND_WIDTH = 400,

    MODE_FINANCIAL_TAB1 = 15,
    MODE_FINANCIAL_TAB2 = 280,
    MODE_FINANCIAL_TAB3 = 360,
    MODE_FINANCIAL_TAB4 = 490,
    MODE_FINANCIAL_TAB5 = 520,
    MODE_FINANCIAL_TAB6 = 600,
};

enum {
    FINANCIAL_BUTTON_X1 = REPORT_X1+MODE_FINANCIAL_TAB4,
    FINANCIAL_BUTTON_Y1 = REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+180,
    FINANCIAL_BUTTON_X2 = REPORT_X2,
    FINANCIAL_BUTTON_Y2 = REPORT_Y2-40
};

enum {
    FINANCIAL_BUTTON_WIDTH = 200,
    FINANCIAL_BUTTON_HEIGHT = 23,
    FINANCIAL_BUTTON_DISTANCE
    = (FINANCIAL_BUTTON_Y2-FINANCIAL_BUTTON_Y1-FINANCIAL_BUTTON_HEIGHT*4)/3
};

enum {                                            // the bg image id
    BG_PIC_ID = 2
};

static char crime_init_flag;
static GraphMonth crime_index;

static char *legend_label[] = {
    "Crime Index",
};

static Spinner financial_spinner_budget1;

static ButtonGroup financial_button_group(3);
static short *financial_button_up_bitmap[3];
static short *financial_button_down_bitmap[3];
static char *financial_button_label[] = {
    "Consider for next year", "Promise for next year", "Implement now"
};

//------- Begin of function Crime::Crime -----------//
//!
Crime::Crime() {
    init();

    for( int i = 0; i < 3; ++i ) {
	financial_button_up_bitmap[i] = NULL;
	financial_button_down_bitmap[i] = NULL;
    }
}

//--------- End of function Crime::Crime -----------//

//------- Begin of function Crime::~Crime -----------//
//!
Crime::~Crime() {
    deinit();
}

//--------- End of function Crime::~Crime -----------//

//------- Begin of function Crime::deinit ---------------//
//!
void Crime::deinit() {
    crime_index.deinit();
    financial_spinner_budget1.deinit();
    financial_button_group.deinit_buttons();

    for( int i = 0; i < 3; ++i ) {
	if( financial_button_up_bitmap[i] ) {
	    mem_del(financial_button_up_bitmap[i]);
	    financial_button_up_bitmap[i] = NULL;
	}
	if( financial_button_down_bitmap[i] ) {
	    mem_del(financial_button_down_bitmap[i]);
	    financial_button_down_bitmap[i] = NULL;
	}
    }
    crime_init_flag = 0;
}

//--------- End of function Crime::deinit ---------------//

//------- Begin of function Crime::init -----------------//
//!
void Crime::init() {
    crime_init_flag = 0;
    spinner_var1 = 0.0;
}

//--------- End of function Crime::init -----------------//

//---------- Begin of function Crime::eval_letter_report ------------//
//! You can set the value of Crime::topic_id before running this functions
void Crime::crime_report(int refreshFlag) {
    int tab1=REPORT_X1,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    static int data_num=HISTORY_MONTH_COUNT;
    int dataArray1[HISTORY_MONTH_COUNT];

    vga.use_back();

    for(int i=0;i<HISTORY_MONTH_COUNT;i++)
	dataArray1[i]=facility_office.crime_index[i];

    sys.is_institution_report=1;
    if (refreshFlag == INFO_REPAINT) {
	if(crime_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!crime_init_flag) {
	    user_interface.bg_img(BG_PIC_ID, &vga_back);
	    info.disp_column_bitmap(&vga_back);
	    crime_init_flag = 1;
	    font_charts.put(tab3+120,REPORT_Y1,"Crime Report");
	    user_interface.brighten(
		REPORT_X1+200, REPORT_Y1+50,
		REPORT_X2-220, REPORT_Y2-50);

	    crime_index.init(
		REPORT_X1+20, REPORT_Y1+50,
		REPORT_X2-220, REPORT_Y2-50,
		1, &data_num, dataArray1,1,
		NULL, NULL, legend_label, 1, 1, 1, 4);
	    crime_index.paint();

	    // print "Adjust administration budget"
	    font_charts.center_put( FINANCIAL_BUTTON_X1, 200,
				    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH, 230,
				    "Adjust administration budget" );

	    for (int i = 0; i < 3; i++) {
		financial_button_up_bitmap[i] = NULL;
		financial_button_down_bitmap[i] = NULL;
	    }

	    short y = FINANCIAL_BUTTON_Y1;
	    for (int i = 0; i < 3; i++, y+=FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE) {
		user_interface.create_button_bitmap(FINANCIAL_BUTTON_X1,y,
						    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT,
						    financial_button_label[i], &financial_button_up_bitmap[i],
						    &financial_button_down_bitmap[i], &vga_back);

		financial_button_group[i].create_bitmapW(FINANCIAL_BUTTON_X1,y,
							 FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT,
							 (financial_button_up_bitmap[i])+4,0);
	    }

	    financial_spinner_budget1.init(
		REPORT_X1+MODE_FINANCIAL_TAB4+30,
		REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+140,
		REPORT_X1+MODE_FINANCIAL_TAB4+150,
		REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+170 ,
		//### begin zhoubin 02/25/99
		&crime.spinner_var1,
		//### end zhoubin 02/25/99
		//				-100.0f,100.0f,0.1f,0,NULL,23);
		-5.0f,5.0f,0.1f,0,NULL,25);               //## chea 170999

	    font_chartsm.put_paragraph( REPORT_X1+20, REPORT_Y2-30, REPORT_X2-220, REPORT_Y2+10,
					"Increased crime may be combated by increasing money spent on administration, which includes spending on campus security." );
	}
    }
    crime_index.refresh();
    for ( int i=0; i<3; i++ )
	financial_button_group[i].paint();
    financial_spinner_budget1.paint();

    for(int i=0;i<3;i++) {
	financial_button_group[i].pop();
	financial_button_group[i].body_ptr=
	    (void*)(financial_button_up_bitmap[i]+4);
    }
    //------- blt the back buffer to front buffer -------//
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//---------- End of function Development::eval_letter_report ------------//

////////////////////////////////////////////////////////////////////////////////////////////

int Crime::crime_detect() {
    int tab1=REPORT_X1,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+600;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    if (financial_spinner_budget1.detect())
	return 1;
    //	if (financial_spinner_budget2.detect())
    //		return 1;

    for(int i=0;i<3;i++)
	if(financial_button_group[i].detect()) {
	    financial_button_group[i].body_ptr =
		(void *) (financial_button_down_bitmap[i] + 4);
	    //	Communicate with Sim Engine
	    finance.change_budget(i,false, AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE,(*financial_spinner_budget1.var_ptr));
	    //##	begin zhoubin 000329
	    *financial_spinner_budget1.var_ptr=0;
	    //##	end zhoubin 000329
	}

    if (!crime_init_flag)
	return 0;

    return 0;
}
