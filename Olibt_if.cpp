//Filename    : OLIBT_IF.CPP
//Description : LibTech (LibTech and Alumni Affairs Office) class
// owner			: Kevin(Ho)
// Notes:	Steps to introduce a new class of user interface
//				1.	construct the new class
//				2.	in sys.h, add the new MODE, e.g. MODE_ADMISSIONS_AND_FINANCIAL_OFFICE
//				3.	include the new header file in cc.cpp, osysdisp.cpp, osysdet.cpp
//				4.	in cc.cpp, declare a global object of the new class, with the name matching the 'extern' statement in the header file
//				5.	in osysdisp.cpp, under switch(view_mode), add the redirection to the display function
//				6.	in osysdet.cpp, under Sys::detect_view, add the redirection to the detect function
//				7.	in osysdet.cpp, under Sys::process_key, add the new hot key (optional)
//				8.	in osysdet.cpp, under Sys::detect_button, modify if necessary (see the comments there)
//				9.	the deinit function (AdmOffice::deinit()) can be called when the view_mode in 'sys' object is switched
//					the function frees memory occupied by this object
//					the report function would re-allocate memory when it is called (note the init_flag)
//					notice the beginning section of Sys::disp_frame()
// Kevin 0723

#include <OSYS.H>
#include <OSTR.H>
#include <OMISC.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OLIBTECH.H>
#include <OVGA.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <COLOR.H>
#include <OTEXT.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OBUTTON.H>
#include <OVALSLDR.H>
#include <ORADIO.H>
#include <OSPINNER.H>
#include <OGRPHYR.H>
#include <OGRPHMTH.H>
#include <OGRPHTRI.H>
#include <OGRPH_PY.H>
#include <OPIECHRT.H>
#include <OFINANCE.H>
#include <OHELP.H>

//------- Define constants -------//

enum {                                            // the 'sub' view-modes
    MODE_OVERVIEW = 0,
    MODE_FINANCIAL = 1,
};

enum {                                            // the bg image id
    BG_PIC_ID = 2
};

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-55
};

enum {
    BOTTOM_BUTTON_X1 = REPORT_X1+200,
    BOTTOM_BUTTON_Y1 = REPORT_Y2+15,
    BOTTOM_BUTTON_X2 = REPORT_X2-80,
    BOTTOM_BUTTON_Y2 = REPORT_Y2+40
};

enum {
    BOTTOM_BUTTON_WIDTH = 125,
    BOTTOM_BUTTON_HEIGHT = 25,
    BOTTOM_BUTTON_DISTANCE
    = (BOTTOM_BUTTON_X2-BOTTOM_BUTTON_X1-BOTTOM_BUTTON_WIDTH*2)/3
};

// begin kevin 0723
enum {
    MODE_OVERVIEW_BOX1_HEIGHT = 265,
    MODE_OVERVIEW_BOX2_HEIGHT = 60,
    MODE_OVERVIEW_BOX_WIDTH = 650,
    MODE_OVERVIEW_TAB0 = 40,
    MODE_OVERVIEW_TAB1 = 250,
    MODE_OVERVIEW_TAB4 = 440,
};

// end kevin 0723

enum {
    MODE_FINANCIAL_BOX1_HEIGHT = 85,
    MODE_FINANCIAL_BOX2_HEIGHT = 250,
    MODE_FINANCIAL_GRAPH_WIDTH = 400,
    MODE_FINANCIAL_GRAPH_HEIGHT = 160,
    MODE_FINANCIAL_LEGEND_HEIGHT = 26,
    MODE_FINANCIAL_LEGEND_WIDTH = 400,

    MODE_FINANCIAL_TAB1 = 15,
    MODE_FINANCIAL_TAB2 = 280,
    MODE_FINANCIAL_TAB3 = 360,
    MODE_FINANCIAL_TAB4 = 440,
    MODE_FINANCIAL_TAB5 = 520,
    MODE_FINANCIAL_TAB6 = 600,
};

enum {
    FINANCIAL_BUTTON_X1 = REPORT_X1+MODE_FINANCIAL_TAB4+30,
    FINANCIAL_BUTTON_Y1 = REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+60,
    FINANCIAL_BUTTON_X2 = REPORT_X2,
    FINANCIAL_BUTTON_Y2 = REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+180
};

enum {
    FINANCIAL_BUTTON_WIDTH = 200,
    FINANCIAL_BUTTON_HEIGHT = 20,
    FINANCIAL_BUTTON_DISTANCE= 5
};

enum {                                            // the 'sub' view-modes
    MODE_FINANCIAL_IMPLEMENT = 2,
    MODE_FINANCIAL_ANNOUNCE = 1,
    MODE_FINANCIAL_CONSIDER = 0,
};

//-------- define static vars -----------//

static char report_init_flag = 0;
static char mode_init_flag = -1;                  // -1: non of the mode was init'ed

static char* mode_overview_radio_button_option_str[4]= {
    "Bad        ",
    "Good       ",
    "Very good  ",
    "Best       "
};

static ButtonGroup bottom_button_group(2);
static short *bottom_button_up_bitmap[2];
static short *bottom_button_down_bitmap[2];
static char *bottom_button_label[] = {
    "Overview","Finance",
};

// Notes:	the strings could be moved to Ogamestr.cpp
// Begin LibTech Mode Setting

static char *mode_overview_str1[] = {
    "Number of volumes in library",
    "Weighted average percentage of faculty effort spent on educational development using information technology",
    "",
    "Quality of IT infrastructure and training",
    "Percentage of courses redesigned to make optimal use of information technology",
    "Institutional incentives for faculty to optimize IT in teaching",
    "Percent of regular tuition for distance learning credits",
};

static short *mode_overview_text_bitmap;
static Text mode_overview_text;
static RadioButton mode_overview_radiobutton;
static ValueSlider mode_overview_valueslider;
static ValueSlider mode_overview_valueslider2;
static GraphPerformanceIndicatorYear mode_overview_graphicon[5];

// End LibTech Mode Setting

// Begin Financial Mode Setting
static char *mode_financial_str[] = {
    "Library expenditures",
    "Information technology expenditures",
    "Net distance learning expenditures (revenues)",
    "",
    "Change",
    "Change in libraries \r\nexpenditure budget",
    "Change in IT \r\nexpenditure budget"
};

static char *mode_financial_label[] = {
    "Revenue", "Expenditure","Profit", "Loss",
};

static short *mode_financial_stats_bitmap;
static Spinner financial_spinner_budget1;
static Spinner financial_spinner_budget2;
static ButtonGroup financial_button1_group(3);
static short *financial_button1_up_bitmap[3];
static short *financial_button1_down_bitmap[3];
static ButtonGroup financial_button2_group(3);
static short *financial_button2_up_bitmap[3];
static short *financial_button2_down_bitmap[3];
static char *financial_button_label[] = {
    "Consider for next year", "Promise for next year", "Implement now"
};
static char financial_current_mode = MODE_FINANCIAL_CONSIDER;

static GraphMonth mode_financial_graph_profitloss;
static char *mode_financial_graph_title="";
static char *mode_financial_legend_label[] = {
    "Library expenditures",
    "Information technology expenditures",
};
static char *mode_financial_legend_label2[] = {
    "Net distance learning expenditures (revenues)",
};
static GraphTrimester mode_financial_net_expenditures;

// End Financial Mode Setting

//-------- define static functions ------//

// Notes:	the deinit_x functions should deallocate memory used by the
//				GraphDoubleBar, ValueSlider, RadioButton, etc. objects
//				the functions are called when a mode-switching is triggered
//	Norris 0716

static void report_overview(int);
static void report_financial(int);
static int detect_overview();
static int detect_financial();
static void deinit_overview();
static void deinit_financial();

typedef void (*ReportFP)(int refreshFlag);
typedef int (*DetectFP)();
typedef void (*DeinitFP)();

static ReportFP report_func_ptr[] = {
    &report_overview,
    &report_financial,
};

static DetectFP detect_func_ptr[] = {
    &detect_overview,
    &detect_financial,
};

static DeinitFP deinit_func_ptr[] = {
    &deinit_overview,
    &deinit_financial,
};

//------- Begin of function LibTech::LibTech -----------//
//!
LibTech::LibTech() {
    init();
}

//--------- End of function LibTech::LibTech -----------//

//------- Begin of function LibTech::~LibTech -----------//
//!
LibTech::~LibTech() {
    deinit();
}

//--------- End of function LibTech::~LibTech -----------//

//------- Begin of function LibTech::deinit ---------------//
//!
void LibTech::deinit() {
    //--------- deallocate memory -------------//
    // #### begin Gilbert 28/04/2001 #####//
    bottom_button_group.deinit_buttons();
    // #### end Gilbert 28/04/2001 #####//
    if (*bottom_button_up_bitmap) {
	for (int i = 0; i < 2; i++) {
	    mem_del(bottom_button_up_bitmap[i]);
	    mem_del(bottom_button_down_bitmap[i]);
	    bottom_button_up_bitmap[i] = NULL;
	    bottom_button_down_bitmap[i] = NULL;
	}
    }

    (*deinit_func_ptr[current_mode])();

    report_init_flag = 0;
}

//--------- End of function LibTech::deinit ---------------//

//------- Begin of function LibTech::init -----------------//
//!
void LibTech::init() {
    for (int i = 0; i < 2; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }
    current_mode = MODE_OVERVIEW;
    percent_dl_tuition = 50;
}

//--------- End of function LibTech::init -----------------//

//------- Begin of function LibTech::report ---------------//
//!
void LibTech::report(int refreshFlag) {
    vga.use_back();
    // fix in version 2
    sys.is_institution_report = 1;

    if (refreshFlag == INFO_REPAINT) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    deinit();

	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	//----- initialize the bottom sub-menus buttons -----//
	if (!report_init_flag) {
	    short x = BOTTOM_BUTTON_X1;
	    for (int i = 0; i < 2; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {

		user_interface.create_button_bitmap(x, BOTTOM_BUTTON_Y1,
						    x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						    bottom_button_label[i], &bottom_button_up_bitmap[i],
						    &bottom_button_down_bitmap[i], &vga_back);

		bottom_button_group[i].create_bitmapW(x, BOTTOM_BUTTON_Y1,
						      x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						      ((i == current_mode) ? bottom_button_down_bitmap[i] : bottom_button_up_bitmap[i])+4);
	    }
	    report_init_flag = 1;
	}

	//----------- paint the bottom button group ---------//
	bottom_button_group.paint(current_mode);
    }

    //---- repaint/refresh the display of the current mode ----//
    (*report_func_ptr[current_mode])(refreshFlag);

    // Comments:	blt buffer only if repaint, because the
    //					report_x functions would handle blt_buf
    //					when refresh
    //					equivalently, in the report_x functions,
    //					only blt buffer when refresh

    if (refreshFlag == INFO_REPAINT) {
	//------- blt the back buffer to front buffer -------//
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    }

    //##trevor 2807
    vga.use_front();
    //##trevor 2807
}

//---------- End of function LibTech::report ------------//

//-------- Begin of function LibTech::detect ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int LibTech::detect() {
    vga.use_back();

    int rc = detect2();

    vga.use_front();

    return rc;
}

//---------- End of function LibTech::detect ------------//

//-------- Begin of function LibTech::detect2 ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int LibTech::detect2() {
    if (!report_init_flag)
	return 0;

    int bottomButtonGroupDetect;
    if ((bottomButtonGroupDetect = bottom_button_group.detect()) >= 0) {
	//---- deinit the modes to free memory ----//
	(*deinit_func_ptr[current_mode])();

	//-------- change display mode ------------//
	bottom_button_group[current_mode].body_ptr = (void *) (bottom_button_up_bitmap[current_mode] + 4);
	current_mode = bottomButtonGroupDetect;
	bottom_button_group[current_mode].body_ptr = (void *) (bottom_button_down_bitmap[current_mode] + 4);

	//-------- post display redraw flag -------//
	sys.redraw_all_flag = 1;

	return 1;
    }

    if ((*detect_func_ptr[current_mode])())
	return 1;

    return 0;
}

//---------- End of function AdmOffice::detect ------------//

//-------- Begin of function report_overview -------------//
//!
static void report_overview(int refreshFlag) {
    // begin Kevin 0723

    static double v1=11,v2=12,v3=13,v5=314231,v6=343425;
    static double v7=3427;
    static int v4;
    static int data_num=HISTORY_YEAR_COUNT;
    static int *dataArray1,*dataArray3;
    static float *dataArray2,*dataArray4;
    static char * ptr1;

    // ##### Begin Marco ##### //
    static double initialv1=11,initialv2=12,initialv3=13,initialv5=314231,initialv6=343425,initialv7=50;
    static int initialv4;
    initialv1=library_tech_office.initial_volumns_count_library;
    initialv2=library_tech_office.initial_percent_faculty_using_it;
    initialv4=library_tech_office.initial_internet_quality_percent;
    initialv5=library_tech_office.initial_tech_utilization;
    initialv6=library_tech_office.initial_faculty_incentive_using_it;
    initialv7=library_tech_office.initial_percent_dl_tuition;
    // ##### End Marco ##### //
    //	//	Communicate with Sim Engine's Output
    v1=library_tech_office.volumns_count_library;
    v2=library_tech_office.percent_faculty_using_it;
    //	v3=library_tech_office.percent_faculty_using_it;
    v4=library_tech_office.internet_quality;
    //	v5=library_tech_office.it_usage;
    v5=library_tech_office.tech_utilization;
    v6=library_tech_office.faculty_incentive_using_it;
    v7=library_tech_office.percent_dl_tuition;
    dataArray1=library_tech_office.volumns_count_library_history;
    dataArray2=library_tech_office.percent_faculty_using_it_history;
    dataArray3=library_tech_office.internet_quality_history;
    dataArray4=library_tech_office.tech_utilization_history;

    static int slider1=(int)v6;
    static int slider2=(int)v7;
    ptr1=mode_overview_radio_button_option_str[v4];

    if (refreshFlag == INFO_REPAINT) {
	// top title
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1-4);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1-4,1);
	font_charts.put(REPORT_X1+225,REPORT_Y1-22,"Libraries and Information Technology");

	//--------- draw the brightened boxes ------------//
	user_interface.brighten(
	    REPORT_X1, REPORT_Y1, REPORT_X2,
	    REPORT_Y2);
	user_interface.rect(
	    REPORT_X1, REPORT_Y1, REPORT_X2,
	    REPORT_Y2-MODE_OVERVIEW_BOX2_HEIGHT+3);
	user_interface.rect(
	    REPORT_X1, REPORT_Y2-MODE_OVERVIEW_BOX2_HEIGHT,
	    REPORT_X2,REPORT_Y2);

	if (mode_init_flag != MODE_OVERVIEW) {
	    //---------  initialization  -----------//
	    mode_init_flag= MODE_OVERVIEW;

	    mode_overview_text_bitmap=NULL;
	    mode_overview_text_bitmap=
		vga_back.save_area(
		    REPORT_X1+MODE_OVERVIEW_TAB4,
		    REPORT_Y1,
		    REPORT_X1+MODE_OVERVIEW_TAB4+50,
		    REPORT_Y2,
		    mode_overview_text_bitmap);

	    mode_overview_graphicon[0].init(
		REPORT_X1+MODE_OVERVIEW_TAB4+60,
		REPORT_Y1+50,
		REPORT_X1+MODE_OVERVIEW_TAB4+120,
		REPORT_Y1+80,
		1 ,&data_num ,dataArray1, 1, 0, 1);
	    mode_overview_graphicon[0].paint();

	    mode_overview_graphicon[1].init(
		REPORT_X1+MODE_OVERVIEW_TAB4+60,
		REPORT_Y1+80,
		REPORT_X1+MODE_OVERVIEW_TAB4+120,
		REPORT_Y1+110,
		1 ,&data_num ,dataArray2, 1, 0, 3);
	    mode_overview_graphicon[1].paint();

	    mode_overview_valueslider.init(
		REPORT_X1+MODE_OVERVIEW_TAB4+100,
		REPORT_Y2-50,
		100,
		15,
		0, 100, &slider1, 1, V_YELLOW, 4
		);

	    mode_overview_valueslider2.init(
		REPORT_X1+MODE_OVERVIEW_TAB4+100,
		REPORT_Y2-25,
		100,
		15,
		0, 100, &slider2, 1, V_YELLOW, 4
		);

	    //			mode_overview_radiobutton.init(
	    //				REPORT_X1+50,
	    //				REPORT_Y1+160,
	    //				REPORT_X1+450,
	    //				REPORT_Y1+190,
	    //				4, &v4, mode_overview_radio_button_option_str);
	    //mode_overview_text.put(REPORT_X1+MODE_OVERVIEW_TAB4,REPORT_Y1+150,ptr1);
	    // marco
	    mode_overview_text.put(REPORT_X1+MODE_OVERVIEW_TAB4-55,REPORT_Y1+150,(int)library_tech_office.internet_quality_percent);
	    mode_overview_graphicon[4].init(
		REPORT_X1+MODE_OVERVIEW_TAB4+60,
		REPORT_Y1+210,
		REPORT_X1+MODE_OVERVIEW_TAB4+120,
		REPORT_Y1+240,
		1 ,&data_num ,dataArray4, 1, 0, 3);
	    mode_overview_graphicon[4].paint();
	}
	else {
	    //-------- repaint -----------//
	    mode_overview_graphicon[0].paint();
	    mode_overview_graphicon[1].paint();
	    mode_overview_graphicon[4].paint();
	    //			mode_overview_radiobutton.paint();
	    mode_overview_valueslider.paint();
	    mode_overview_valueslider2.paint();
	}

	//-------- statics text --------------//
	font_chartsm.put(REPORT_X1+MODE_OVERVIEW_TAB0,
			 REPORT_Y1+60, mode_overview_str1[0]);
	font_chartsm.put_paragraph(REPORT_X1+MODE_OVERVIEW_TAB0,
				   REPORT_Y1+90,
				   REPORT_X1+MODE_OVERVIEW_TAB4-40,
				   REPORT_Y1+150,
				   mode_overview_str1[1]);

	font_chartsm.put(REPORT_X1+MODE_OVERVIEW_TAB0,
			 REPORT_Y1+150, mode_overview_str1[3]);

	font_chartsm.put_paragraph(REPORT_X1+MODE_OVERVIEW_TAB0,
				   REPORT_Y1+210,
				   REPORT_X1+MODE_OVERVIEW_TAB4-20,
				   REPORT_Y1+270,
				   mode_overview_str1[4]);

	font_chartsm.put(REPORT_X1+MODE_OVERVIEW_TAB0,
			 REPORT_Y2-50, mode_overview_str1[5]);
	font_chartsm.put(REPORT_X1+MODE_OVERVIEW_TAB0,
			 REPORT_Y2-25, mode_overview_str1[6]);
    }
    else {                                          // refreshFlag == INFO_REFRESH
	mode_overview_graphicon[0].refresh();
	mode_overview_graphicon[1].refresh();
	mode_overview_graphicon[4].refresh();
	vga.blt_buf(
	    REPORT_X1+MODE_OVERVIEW_TAB4+60,
	    REPORT_Y1+50,
	    REPORT_X1+MODE_OVERVIEW_TAB4+120,
	    REPORT_Y1+80
	    );
	vga.blt_buf(
	    REPORT_X1+MODE_OVERVIEW_TAB4+60,
	    REPORT_Y1+80,
	    REPORT_X1+MODE_OVERVIEW_TAB4+120,
	    REPORT_Y1+110
	    );
	vga.blt_buf(
	    REPORT_X1+MODE_OVERVIEW_TAB4+60,
	    REPORT_Y1+210,
	    REPORT_X1+MODE_OVERVIEW_TAB4+120,
	    REPORT_Y1+240
	    );

	//			mode_overview_radiobutton.refresh();
	mode_overview_valueslider.refresh();
	mode_overview_valueslider2.refresh();
    }
    // -------- refresh the stats ----------- //
    vga_back.rest_area(mode_overview_text_bitmap,0,0);

    // ##### Begin Marco ##### //
    font_chartsm.put(REPORT_X1+MODE_OVERVIEW_TAB4 -55,
		     REPORT_Y1+60,m.format((int)v1,1));
    font_chartsm.put(REPORT_X1+MODE_OVERVIEW_TAB4 -55,
		     REPORT_Y1+90,m.format(v2,3));
    font_chartsm.put(REPORT_X1+MODE_OVERVIEW_TAB4 -55,
		     REPORT_Y1+210,m.format(v5,3));
    font_chartsm.put(REPORT_X1+MODE_OVERVIEW_TAB4 -55,
		     REPORT_Y2-50,m.format(v6,3));
    font_chartsm.put(REPORT_X1+MODE_OVERVIEW_TAB4 -55,
		     REPORT_Y2-25,m.format(v7,3));

    font_chart_purple_sm.put(REPORT_X1+MODE_OVERVIEW_TAB4,
			     REPORT_Y1+60,
			     m.format((int)initialv1,28));
    font_chart_purple_sm.put(REPORT_X1+MODE_OVERVIEW_TAB4,
			     REPORT_Y1+90,
			     m.format(initialv2,29));

    font_chart_purple_sm.put(REPORT_X1+MODE_OVERVIEW_TAB4,
			     REPORT_Y1+150,m.format((int)initialv4,28));

    font_chart_purple_sm.put(REPORT_X1+MODE_OVERVIEW_TAB4,
			     REPORT_Y1+210,
			     m.format(initialv5,29));
    font_chart_purple_sm.put(REPORT_X1+MODE_OVERVIEW_TAB4,
			     REPORT_Y2-50,
			     m.format(initialv6,29));
    font_chart_purple_sm.put(REPORT_X1+MODE_OVERVIEW_TAB4,
			     REPORT_Y2-25,
			     m.format(initialv7,29));
    // ##### End MArco ##### //
    mode_overview_text.refresh();

    vga.blt_buf(REPORT_X1+MODE_OVERVIEW_TAB4,
		REPORT_Y1,
		REPORT_X1+MODE_OVERVIEW_TAB4+50,
		REPORT_Y2);
    // end Kevin 0723
}

//-------- Begin of function report_FINANCIAL ---------//
//!
static void report_financial(int refreshFlag) {
    // Notes:	temporary variables for debugging
    //				to be interfaced with game engine

    static int v11=1000,v12=1999,v13=8888;
    static double v21=663.23,v22=123.23,v23=17.23,v24=723.23,v25=13.23;
    static double v31=813.83,v32=888.23,v33=13.83,v34=188.23,v35=183.23;
    static double v41=88.28,v42=777.23,v43=166.27,v44=777.23,v45=173.23;
    static float spinnerVar1=0.0;
    static float spinnerVar2=0.0;
    static int numSeriesData = HISTORY_MONTH_COUNT; //*** temporary ***//
    static int numSeriesData2 = HISTORY_TRIMESTER_COUNT;
    static float dataArray[2*HISTORY_MONTH_COUNT];  //*** temporary ***//
    static float dataArray2[HISTORY_TRIMESTER_COUNT];
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//

  // ##### Begin Marco ##### //
    static double initialv21=663.23;
    static double initialv31=813.83;
    static double initialv41=88.28;
    // ##### End Marco ##### //

  //	Communicate with Sim Engine's Output
    for(int k=0;k<HISTORY_MONTH_COUNT;k++) {
	dataArray[k]=library_tech_office.library_history[k];
	dataArray[1*HISTORY_MONTH_COUNT+k]=library_tech_office.it_history[k];
	//		dataArray[2*HISTORY_MONTH_COUNT+k]=library_tech_office.it_history[k];
    }
    for ( int k=0; k<HISTORY_TRIMESTER_COUNT; k++ )
	dataArray2[k] = library_tech_office.net_dl_expenditures_history[k];
    xStart = info.game_year-numSeriesData+1;
    xEnd = info.game_year;

    v12=info.financial_year();
    v11=v12-1;
    v13=v12+1;

    // Item 8b)
    v21=finance.expense_array[AC_LIBRARIES].total;
    v23=finance.projected_expense_array[AC_LIBRARIES].this_year.total;
    v25=finance.projected_expense_array[AC_LIBRARIES].next_year.total;
    v22 = finance.calc_change(v23,v21);
    v24 = finance.calc_change(v25,v23);

    v31=finance.expense_array[AC_IT_RESOURCES].total;
    v33=finance.projected_expense_array[AC_IT_RESOURCES].this_year.total;
    v35=finance.projected_expense_array[AC_IT_RESOURCES].next_year.total;
    v32=finance.calc_change(v33,v31);
    v34=finance.calc_change(v35,v33);

    v41=finance.net_distance_learning_expense.total/1000;
    v43=finance.projected_net_distance_learning_expense.this_year.total/1000;
    v45=finance.projected_net_distance_learning_expense.next_year.total/1000;
    v42=finance.calc_change(v43,v41);
    v44=finance.calc_change(v45,v43);

    // ##### Begin Marco ##### //
    initialv21=finance.initial_expense_array[AC_LIBRARIES].total;
    initialv31=finance.initial_expense_array[AC_IT_RESOURCES].total;
    initialv21=50;
    // ##### End Marco ##### //
    ///////////////

    int field_space=5*font_chartsm.max_font_width;

    if (refreshFlag == INFO_REPAINT) {

	if (mode_init_flag != MODE_FINANCIAL) {
	    mode_init_flag = MODE_FINANCIAL;
	    //			spinnerVar1=0.0;
	    //			spinnerVar2=0.0;
	    //			spinnerVar1=(float)finance.get_policy_var(false, AC_LIBRARIES)->target_value;
	    //			spinnerVar2=(float)finance.get_policy_var(false, AC_IT_RESOURCES)->target_value;

	    // top title
	    user_interface.brighten(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1-4);
	    user_interface.rect(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1-4,1);
	    font_charts.put(REPORT_X1+225,REPORT_Y1-22,"Libraries and Information Technology");

	    //--------- draw the brightened boxes ------------//
	    user_interface.brighten(REPORT_X1, REPORT_Y1-5, REPORT_X2, REPORT_Y2);
	    user_interface.rect(REPORT_X1, REPORT_Y1-5, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+3);
	    user_interface.rect(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT, REPORT_X1+MODE_FINANCIAL_TAB4-2, REPORT_Y2);
	    user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB4-4, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT, REPORT_X2, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT+130);
	    user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB4-4, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT+130, REPORT_X2, REPORT_Y2);

	    for (int i = 0; i < 3; i++) {
		financial_button1_up_bitmap[i] = NULL;
		financial_button1_down_bitmap[i] = NULL;
		financial_button2_up_bitmap[i] = NULL;
		financial_button2_down_bitmap[i] = NULL;
	    }

	    short y = FINANCIAL_BUTTON_Y1;
	    for (int i = 0; i < 3; i++, y+=FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE) {
		user_interface.create_button_bitmap(FINANCIAL_BUTTON_X1,y,
						    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT,
						    financial_button_label[i], &financial_button1_up_bitmap[i],
						    &financial_button1_down_bitmap[i], &vga_back);

		financial_button1_group[i].create_bitmapW(FINANCIAL_BUTTON_X1,y,
							  FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT,
							  (financial_button1_up_bitmap[i])+4,0);
	    }

	    y = FINANCIAL_BUTTON_Y2;
	    for (int i = 0; i < 3; i++, y+=FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE) {
		user_interface.create_button_bitmap(FINANCIAL_BUTTON_X1,y,
						    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT,
						    financial_button_label[i], &financial_button2_up_bitmap[i],
						    &financial_button2_down_bitmap[i], &vga_back);

		financial_button2_group[i].create_bitmapW(FINANCIAL_BUTTON_X1,y,
							  FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT,
							  (financial_button2_up_bitmap[i])+4,0);
	    }

	    financial_spinner_budget1.init(
		REPORT_X1+MODE_FINANCIAL_TAB5+80,
		REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+15,
		REPORT_X1+MODE_FINANCIAL_TAB5+175,
		REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+45,
		//### begin zhoubin 02/28/99
		&library_tech_office.spinner_var1,
		//### end zhoubin 02/28/99
		//				-100.0f,100.0f,0.1f,0,NULL,23);
		-5.0f,5.0f,0.1f,0,NULL,25);               //## chea 170999

	    financial_spinner_budget2.init(
		REPORT_X1+MODE_FINANCIAL_TAB5+80,
		REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+143,
		REPORT_X1+MODE_FINANCIAL_TAB5+175,
		REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+173,
		//### begin zhoubin 02/28/99
		&library_tech_office.spinner_var2,
		//### end zhoubin 02/28/99
		//				-100.0f,100.0f,0.1f,0,NULL,23);
		-5.0f,5.0f,0.1f,0,NULL,25);               //## chea 170999

	    //-------- graph title --------------//
	    mode_financial_graph_profitloss.init(
		REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT,
		REPORT_X1+MODE_FINANCIAL_TAB4-2, (2*REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT)/2,
		2, &numSeriesData, dataArray,1,
		NULL, NULL, mode_financial_legend_label, 1, 1, 1, 2);
	    mode_financial_net_expenditures.init(
		REPORT_X1, (2*REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT)/2,
		REPORT_X1+MODE_FINANCIAL_TAB4-2, REPORT_Y2,
		1, &numSeriesData2, dataArray2, 1,
		NULL, NULL, mode_financial_legend_label2, 1, 1, 1, 2);
	}
	else {
	    //--------- draw the brightened boxes ------------//
	    user_interface.brighten(REPORT_X1, REPORT_Y1-5, REPORT_X2, REPORT_Y2);
	    user_interface.rect(REPORT_X1, REPORT_Y1-5, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+3);
	    user_interface.rect(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT, REPORT_X1+MODE_FINANCIAL_TAB4-2, REPORT_Y2);
	    user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB4-4, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT, REPORT_X2, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT+130);
	    user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB4-4, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT+130, REPORT_X2, REPORT_Y2);

	    //-------- paint the buttons -----------//
	    financial_spinner_budget1.paint();
	}

	mode_financial_graph_profitloss.paint();
	mode_financial_net_expenditures.paint();

	//-------- display static strings -----------//
	font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+25, mode_financial_str[0]);
	font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+45, mode_financial_str[1]);
	font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+65, mode_financial_str[2]);

	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				REPORT_Y1,
				//					REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				//					REPORT_Y1+font_chartsm.max_font_height,
				mode_financial_str[4]);
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				REPORT_Y1,
				//					REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				//					REPORT_Y1+font_chartsm.max_font_height,
				mode_financial_str[4]);

	font_chartsm.put_paragraph(
	    REPORT_X1+MODE_FINANCIAL_TAB4+10,
	    REPORT_Y1+20+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X1+MODE_FINANCIAL_TAB5+160,
	    REPORT_Y1+70+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_str[5]);

	font_chartsm.put_paragraph(
	    REPORT_X1+MODE_FINANCIAL_TAB4+10,
	    REPORT_Y1+145+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X1+MODE_FINANCIAL_TAB5+160,
	    REPORT_Y1+185+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_str[6]);

	mode_financial_stats_bitmap=NULL;
	mode_financial_stats_bitmap=vga_back.save_area(
	    REPORT_X1+MODE_FINANCIAL_TAB2, REPORT_Y1,
	    REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_stats_bitmap);

    }
    else {                                          // refreshFlag == INFO_REFRESH
	//-------- refresh dynamic data --------------//
	// refresh() ==	1. rest()
	//						2. draw()
	//						3. blit buffer()

	mode_financial_graph_profitloss.refresh();
	vga.blt_buf(
	    REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT,
	    REPORT_X1+MODE_FINANCIAL_TAB4-2, REPORT_Y2);
    }

    financial_spinner_budget1.refresh();
    financial_spinner_budget2.refresh();

    // show buttons
    int pressed=finance.get_policy_var(false, AC_LIBRARIES)->applied_flag;
    if(pressed==P_NONE)
	for(int i=0;i<3;i++)
	    financial_button1_group[i].paint();
    else
	financial_button1_group.paint(pressed-1);
    pressed=finance.get_policy_var(false, AC_IT_RESOURCES)->applied_flag;
    if(pressed==P_NONE)
	for(int i=0;i<3;i++)
	    financial_button2_group[i].paint();
    else
	financial_button2_group.paint(pressed-1);

    short y = FINANCIAL_BUTTON_Y1 ;
    for (int i = 0; i < 3; i++, y+=FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE) {
	vga.blt_buf(FINANCIAL_BUTTON_X1,y,
		    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT);
    }

    y = FINANCIAL_BUTTON_Y2 ;
    for (int i = 0; i < 3; i++, y+=FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE) {
	vga.blt_buf(FINANCIAL_BUTTON_X1,y,
		    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT);
    }

    vga_back.rest_area(mode_financial_stats_bitmap,0,0);
    String *s1,*s2;

    //show years

    s1=new String(m.format(v11,16));
    s2=new String(m.format(v11+1,16));
    //	(*s1)+="-";    //## chea 061199 2.5.1
    //	(*s1)+=(*s2);  //## chea 061199 2.5.1

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1,
			    //				REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    //				REPORT_Y1+font_chartsm.max_font_height,
			    s1->str_buf);

    (*s1)=m.format(v12,16);
    (*s2)=m.format(v12+1,16);
    //	(*s1)+="-";    //## chea 061199 2.5.1
    //	(*s1)+=(*s2);  //## chea 061199 2.5.1

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1,
			    //				REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    //				REPORT_Y1+font_chartsm.max_font_height,
			    s1->str_buf);

    (*s1)=m.format(v13,16);
    (*s2)=m.format(v13+1,16);
    //	(*s1)+="-";      //## chea 061199 2.5.1
    //	(*s1)+=(*s2);    //## chea 061199 2.5.1

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1,
			    //				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    //				REPORT_Y1+font_chartsm.max_font_height,
			    s1->str_buf);
    delete s1,s2;

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+25,
			    //				REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    //				REPORT_Y1+25+font_chartsm.max_font_height,
			    m.format(v21, 2));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    REPORT_Y1+25,
			    //				REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    //				REPORT_Y1+25+font_chartsm.max_font_height,
			    m.format(v22, 24));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+25,
			    //				REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    //				REPORT_Y1+25+font_chartsm.max_font_height,
			    m.format(v23, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    REPORT_Y1+25,
			    //				REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    //				REPORT_Y1+25+font_chartsm.max_font_height,
			    m.format(v24, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+25,
			    //				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    //				REPORT_Y1+25+font_chartsm.max_font_height,
			    m.format(v25, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+45,
			    //				REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    //				REPORT_Y1+45+font_chartsm.max_font_height,
			    m.format(v31, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    REPORT_Y1+45,
			    //				REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    //				REPORT_Y1+45+font_chartsm.max_font_height,
			    m.format(v32, 24));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+45,
			    //				REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    //				REPORT_Y1+45+font_chartsm.max_font_height,
			    m.format(v33, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    REPORT_Y1+45,
			    //				REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    //				REPORT_Y1+45+font_chartsm.max_font_height,
			    m.format(v34, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+45,
			    //				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    //				REPORT_Y1+45+font_chartsm.max_font_height,
			    m.format(v35, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+65,
			    //				REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    //				REPORT_Y1+65+font_chartsm.max_font_height,
			    m.format(v41, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    REPORT_Y1+65,
			    //				REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    //				REPORT_Y1+65+font_chartsm.max_font_height,
			    m.format(v42, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+65,
			    //				REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    //				REPORT_Y1+65+font_chartsm.max_font_height,
			    m.format(v43, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    REPORT_Y1+65,
			    //				REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    //				REPORT_Y1+65+font_chartsm.max_font_height,
			    m.format(v44, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+65,
			    //				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    //				REPORT_Y1+65+font_chartsm.max_font_height,
			    m.format(v45, 2));

    // ##### Begin Marco ##### //
    if (info.financial_year() > 1) {
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+10,
					REPORT_Y1,
					"Yr. 0");
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+10,
					REPORT_Y1+25,
					m.format(initialv21, 2));
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+10,
					REPORT_Y1+45,
					m.format(initialv31, 2));
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+10,
					REPORT_Y1+65,
					m.format(initialv41, 2));
	// Kenneth
    }
    // ##### End Marco ##### //

    vga.blt_buf(REPORT_X1+MODE_FINANCIAL_TAB2, REPORT_Y1,
		REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT);

    for(int i=0;i<3;i++) {
	financial_button1_group[i].pop();
	financial_button2_group[i].pop();
	financial_button1_group[i].body_ptr=
	    (void*)(financial_button1_up_bitmap[i]+4);
	financial_button2_group[i].body_ptr=
	    (void*)(financial_button2_up_bitmap[i]+4);
    }
}

//---------- End of function report_FINANCIAL ---------//

//-------- Begin of function detect_overview ---------//
//!
static int detect_overview() {
    if (mode_init_flag != MODE_OVERVIEW)
	return 0;

    help.set_help(  REPORT_X1+MODE_OVERVIEW_TAB0,
		    REPORT_Y2-55,
		    REPORT_X2-20,
		    REPORT_Y2-25,
		    "A.6.6.1.");
    vga.use_back();
    if (mode_overview_valueslider.detect()) {
	library_tech_office.faculty_incentive_using_it=(float)*(mode_overview_valueslider.var_ptr);
	return 1;
    }
    if ( mode_overview_valueslider2.detect()) {
	library_tech_office.percent_dl_tuition=(float)*(mode_overview_valueslider2.var_ptr);
	return 1;
    }
    vga.use_front();
    //	if (mode_overview_radiobutton.detect())
    //	{
    //		library_tech_office.internet_quality=(char)*(mode_overview_radiobutton.sel_num);
    //		return 1;
    //	}

    return 0;

}

//---------- End of function detect_overview ---------//

//-------- Begin of function detect_financial ---------//
//!
static int detect_financial() {
    if (mode_init_flag != MODE_FINANCIAL)
	return 0;

    vga.use_front();

    help.set_help(
	REPORT_X1+MODE_FINANCIAL_TAB4+10,
	REPORT_Y1+20+MODE_FINANCIAL_BOX1_HEIGHT,
	REPORT_X1+MODE_FINANCIAL_TAB5+160,
	REPORT_Y1+70+MODE_FINANCIAL_BOX1_HEIGHT,
	"A.7.7.1.");

    help.set_help(
	REPORT_X1+MODE_FINANCIAL_TAB4+10,
	REPORT_Y1+145+MODE_FINANCIAL_BOX1_HEIGHT,
	REPORT_X1+MODE_FINANCIAL_TAB5+160,
	REPORT_Y1+185+MODE_FINANCIAL_BOX1_HEIGHT,
	"A.7.7.2.");

    if (financial_spinner_budget1.detect())
	return 1;
    if (financial_spinner_budget2.detect())
	return 1;

    for(int i=0;i<3;i++) {
	if(financial_button1_group[i].detect()) {
	    financial_button1_group[i].body_ptr =
		(void *) (financial_button1_down_bitmap[i] + 4);
	    //	Communicate with Sim Engine
	    finance.change_budget(i,false, AC_LIBRARIES,(*financial_spinner_budget1.var_ptr));
	    //##	begin zhoubin 000329
	    *financial_spinner_budget1.var_ptr=0;
	    //##	end zhoubin 000329
	}
	if(financial_button2_group[i].detect()) {
	    financial_button2_group[i].body_ptr =
		(void *) (financial_button2_down_bitmap[i] + 4);
	    //	Communicate with Sim Engine
	    finance.change_budget(i,false, AC_IT_RESOURCES,(*financial_spinner_budget2.var_ptr));
	    //##	begin zhoubin 000329
	    *financial_spinner_budget2.var_ptr=0;
	    //##	end zhoubin 000329
	}
    }
    return 0;
}

//---------- End of function detect_FINANCIAL ---------//

//-------- Begin of function deinit_overview ---------//
//!
static void deinit_overview() {
    if (mode_init_flag == MODE_OVERVIEW) {
	if(mode_overview_text_bitmap) {
	    mem_del(mode_overview_text_bitmap);
	    mode_overview_text_bitmap=NULL;
	}

	// ###### begin Gilbert 25/04/2001 ######//
	mode_overview_graphicon[0].deinit();
	mode_overview_graphicon[1].deinit();
	mode_overview_graphicon[4].deinit();
	// ###### end Gilbert 25/04/2001 ######//

	//		mode_overview_radiobutton.deinit();
	mode_overview_text.deinit();
	mode_overview_valueslider.deinit();
	mode_overview_valueslider2.deinit();
	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_overview ---------//

//-------- Begin of function deinit_financial ------//
//!
static void deinit_financial() {
    if (mode_init_flag == MODE_FINANCIAL) {
	// deallocate memory used by the radio buttons and the value sliders
	if(mode_financial_stats_bitmap) {
	    mem_del(mode_financial_stats_bitmap);
	    mode_financial_stats_bitmap=NULL;
	}

	// ##### begin Gilbert 28/04/2001 ######//
	financial_button1_group.deinit_buttons();
	financial_button2_group.deinit_buttons();
	// ##### end Gilbert 28/04/2001 ######//
	if (*financial_button1_up_bitmap) {
	    for (int i = 0; i < 3; i++) {
		mem_del(financial_button1_up_bitmap[i]);
		mem_del(financial_button1_down_bitmap[i]);
		financial_button1_up_bitmap[i] = NULL;
		financial_button1_down_bitmap[i] = NULL;
		mem_del(financial_button2_up_bitmap[i]);
		mem_del(financial_button2_down_bitmap[i]);
		financial_button2_up_bitmap[i] = NULL;
		financial_button2_down_bitmap[i] = NULL;
	    }
	}

	//		mode_financial_slider_budget.deinit();
	financial_spinner_budget1.deinit();
	financial_spinner_budget2.deinit();
	mode_financial_graph_profitloss.deinit();

	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_financial ------//
