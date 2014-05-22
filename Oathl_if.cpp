//Filename    : OATHL_IF.CPP
//Description : Athletics (Athletics Office) class
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
//					the report function would re-allocate memory when it is called (note the report_init_flag)
//					notice the beginning section of Sys::disp_frame()
// Kevin 0723

#include <OSYS.H>
#include <OSTR.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OATHLETI.H>
#include <OVGA.H>
#include <OINFO.H>
#include <OENROLL.H>
#include <OIFACE.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OBUTTON.H>
#include <ORADIOV.H>
#include <ORADIO.H>
#include <OFINANCE.H>
#include <OVALSLDR.H>
#include <OSPINNER.H>
#include <OGRPHYR.H>
#include <OGRPAREA.H>
#include <OGRPDYR.H>
#include <OPSCHOOL.H>                             //#### fred 1021
#include <OHELP.H>                                //#### fred 1021
#include <OCHANCE.H>
//------- Define constants -------//

enum {                                            // the 'sub' view-modes
    MODE_ATHLETICS = 0,
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
    MODE_ATHLETICS_BOX1_HEIGHT = 120,
    MODE_ATHLETICS_BOX2_HEIGHT = 215,
    MODE_ATHLETICS_BOX_WIDTH = 650,
    MODE_ATHLETICS_TAB0 = 15,
    MODE_ATHLETICS_TAB1 = 100,
    MODE_ATHLETICS_TAB2 = 455,
};

enum {
    MODE_ATHLETICS_GRAPH_WIDTH = 370,
    MODE_ATHLETICS_GRAPH_HEIGHT = 160,
    MODE_ATHLETICS_LEGEND_HEIGHT = 26,
    MODE_ATHLETICS_LEGEND_WIDTH = 370,
};

// end kevin 0723

enum {
    MODE_FINANCIAL_BOX1_HEIGHT = 125,
    MODE_FINANCIAL_BOX2_HEIGHT = 210,
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
    FINANCIAL_BUTTON_Y1 = REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+120,
    FINANCIAL_BUTTON_X2 = REPORT_X2,
    FINANCIAL_BUTTON_Y2 = REPORT_Y2
};

enum {
    FINANCIAL_BUTTON_WIDTH = 200,
    FINANCIAL_BUTTON_HEIGHT = 25,
    FINANCIAL_BUTTON_DISTANCE
    = (FINANCIAL_BUTTON_Y2-FINANCIAL_BUTTON_Y1-FINANCIAL_BUTTON_HEIGHT*3)/4
};

enum {                                            // the 'sub' view-modes
    MODE_FINANCIAL_IMPLEMENT = 2,
    MODE_FINANCIAL_ANNOUNCE = 1,
    MODE_FINANCIAL_CONSIDER = 0,
};

//-------- define static vars -----------//

static char report_init_flag = 0;
static char mode_init_flag = -1;                  // -1: non of the mode was init'ed

static ButtonGroup bottom_button_group(2);
static short *bottom_button_up_bitmap[2];
static short *bottom_button_down_bitmap[2];
static char *bottom_button_label[] = {
    "Overview", "Finance"
};

// Notes:	the strings could be moved to Ogamestr.cpp
// Begin Athletics Mode Setting
static GraphDoubleBarYear mode_athletics_graphdbar_winloss;
static char *mode_athletics_graph_title="Win/loss records";
static char *mode_athletics_legend_label[] = {
    "Wins","Losses",
};
static RadioButtonVert mode_athletics_radio_button_football;
static RadioButton mode_athletics_radio_button_participation;
static RadioButton mode_athletics_radio_button_adm_treatment;
static RadioButton mode_athletics_radio_button_fin_treatment;
static char *mode_athletics_str1[] = {
    "Division III", "Division II", "Division I"
};
static char *mode_athletics_str2[] = {
    "Low", "Medium", "High"
};
static char *mode_athletics_str3[] = {
    "Men's football team record (Fall semester)",
    "Women's basketball team record \r\n(Spring semester)"
};

static char *mode_athletics_str4[] = {
    "Number of top athletes currently enrolled",
    "Current intercollegiate level of competition",
    "Special admission treatment for top athletes",
    "Special financial aid treatment for top athletes",
};

static short *mode_athletics_no_top_athletes_bitmap;

// End Athletics Mode Setting

// Begin Financial Mode Setting
static char *mode_financial_str[] = {
    "Intercollegiate athletic revenue",
    "Athletic expenditure",
    "Balance",
    "",
    "Change",
    "Change in athletic expenditure budget"
};

static short *mode_financial_stats_bitmap;
static Spinner financial_spinner_budget;
static ValueSlider mode_financial_slider_budget;
static ButtonGroup financial_button_group(3);
static short *financial_button_up_bitmap[3];
static short *financial_button_down_bitmap[3];
static char *financial_button_label[] = {
    "Consider for next year", "Promise for next year", "Implement now"
};
static char financial_current_mode = MODE_FINANCIAL_CONSIDER;

static GraphArea mode_financial_graph_profitloss;
static char *mode_financial_graph_title="";
static char *mode_financial_legend_label[] = {
    "Revenue","Expenditure",
    "Profit","Loss",
};

// End Financial Mode Setting

//-------- define static functions ------//

// Notes:	the deinit_x functions should deallocate memory used by the
//				GraphDoubleBar, ValueSlider, RadioButton, etc. objects
//				the functions are called when a mode-switching is triggered
//	Norris 0716

static void report_athletics(int);
static void report_financial(int);
static int detect_athletics();
static int detect_financial();
static void deinit_athletics();
static void deinit_financial();

typedef void (*ReportFP)(int refreshFlag);
typedef int (*DetectFP)();
typedef void (*DeinitFP)();

static ReportFP report_func_ptr[] = {
    &report_athletics,
    &report_financial
};

static DetectFP detect_func_ptr[] = {
    &detect_athletics,
    &detect_financial
};

static DeinitFP deinit_func_ptr[] = {
    &deinit_athletics,
    &deinit_financial,
};

//------- Begin of function Athletics::Athletics -----------//
//!
Athletics::Athletics() {
    init();
}

//--------- End of function Athletics::Athletics -----------//

//------- Begin of function Athletics::~Athletics -----------//
//!
Athletics::~Athletics() {
    deinit();
}

//--------- End of function Athletics::~Athletics -----------//

//------- Begin of function Athletics::deinit ---------------//
//!
void Athletics::deinit() {
    //--------- deallocate memory -------------//
    // ##### begin Gilbert 28/4/2001 ######//
    bottom_button_group.deinit_buttons();
    // ##### end Gilbert 28/4/2001 ######//
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

//--------- End of function Athletics::deinit ---------------//

//------- Begin of function Athletics::init -----------------//
//!
void Athletics::init() {

    memset(this, 0, sizeof(Athletics));

    for (int i = 0; i < 2; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }

    current_mode = MODE_ATHLETICS;
}

//--------- End of function Athletics::init -----------------//

//------- Begin of function Athletics::report ---------------//
//!
void Athletics::report(int refreshFlag) {
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

//---------- End of function Athletics::report ------------//

//-------- Begin of function Athletics::detect ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int Athletics::detect() {
    vga.use_back();

    int rc = detect2();

    vga.use_front();

    return rc;
}

//---------- End of function Athletics::detect ------------//

//-------- Begin of function Athletics::detect2 ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int Athletics::detect2() {
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

//-------- Begin of function report_athletics -------------//
//!
static void report_athletics(int refreshFlag) {
    // begin Kevin 0723

    //*** temporary ***//
    static int radioVar1 = 1, radioVar2 = 1, radioVar3 = 1, radioVar4 = 0;
    static int top_athletes = 99;
    static int numSeriesData = 8;                   //*** temporary ***//
    static int *dataArray;

    //	Communicate with Sim Engine's Output

    // ##### Begin MArco #### //
    static int initial_top_athletes = 99;
    // marco
    initial_top_athletes=enroll_res.initial_matrics_top_athletes;
    // ##### End Marco ##### //

    top_athletes=enroll_res.matrics_top_athletes;
    radioVar1 = athletics_office.ncaa_level_input;  //### fred 1021
    radioVar2 = enroll_res.athlete_offers_special;
    radioVar3 = enroll_res.athlete_aid_special;

    if(radioVar4==0) {
	dataArray = (int*)(athletics_office.football_graph);
	mode_athletics_graphdbar_winloss.data_array=dataArray;
    }
    else {
	dataArray = (int*)(athletics_office.basketball_graph);
	mode_athletics_graphdbar_winloss.data_array=dataArray;
    }

    //

    if (refreshFlag == INFO_REPAINT) {
	//		//--------- draw the brightened boxes ------------//
	user_interface.brighten(REPORT_X1, REPORT_Y1, REPORT_X2,REPORT_Y2);
	user_interface.rect(REPORT_X1, REPORT_Y1, REPORT_X2,
			    REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+3);
	user_interface.rect(REPORT_X1, REPORT_Y2-MODE_ATHLETICS_BOX2_HEIGHT-2,
			    REPORT_X2,REPORT_Y2);

	// top title
	user_interface.brighten(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3);
	user_interface.rect(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3,1);
	font_charts.put(REPORT_X1+290,REPORT_Y1-18,"Athletics Program");

	if (mode_init_flag != MODE_ATHLETICS) {
	    //---------  initialization  -----------//
	    mode_init_flag= MODE_ATHLETICS;
	    mode_athletics_no_top_athletes_bitmap=NULL;

	    image_mascot.put_back(
		REPORT_X1+20,REPORT_Y1+30,player_school.mascot);

	    mode_athletics_no_top_athletes_bitmap=
		vga_back.save_area(
		    MODE_ATHLETICS_TAB1+320,
		    REPORT_Y1+15,
		    MODE_ATHLETICS_TAB1+320+3*font_chartsm.max_font_width,
		    REPORT_Y1+15+font_chartsm.max_font_height,
		    mode_athletics_no_top_athletes_bitmap);

	    mode_athletics_radio_button_participation.init(
		MODE_ATHLETICS_TAB2-10,
		REPORT_Y1+40,
		REPORT_X2,
		REPORT_Y1+40+font_chartsm.max_font_height,
		3, &radioVar1, mode_athletics_str1);

	    mode_athletics_radio_button_adm_treatment.init(
		MODE_ATHLETICS_TAB2-10,
		REPORT_Y1+65,
		REPORT_X2,
		REPORT_Y1+65+font_chartsm.max_font_height,
		3, &radioVar2, mode_athletics_str2);

	    mode_athletics_radio_button_fin_treatment.init(
		MODE_ATHLETICS_TAB2-10,
		REPORT_Y1+90,
		REPORT_X2,
		REPORT_Y1+90+font_chartsm.max_font_height,
		3, &radioVar3, mode_athletics_str2);

	    mode_athletics_radio_button_football.init(
		MODE_ATHLETICS_TAB2,
		REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+MODE_ATHLETICS_BOX1_HEIGHT/2,
		REPORT_X2,
		REPORT_Y2,
		2, &radioVar4, mode_athletics_str3);

	    //-------- graph title --------------//
	    mode_athletics_graphdbar_winloss.init(
		REPORT_X1+MODE_ATHLETICS_TAB0,
		REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+30,
		REPORT_X1+MODE_ATHLETICS_TAB0+MODE_ATHLETICS_GRAPH_WIDTH,
		REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+30+MODE_ATHLETICS_GRAPH_HEIGHT,
		2, &numSeriesData, dataArray, 1,          // 1 - show this indicator
		NULL, NULL, NULL, 1, 1, 0, 4);
	    mode_athletics_graphdbar_winloss.paint();
	}
	else {
	    //-------- repaint -----------//
	    mode_athletics_radio_button_participation.paint();
	    mode_athletics_radio_button_adm_treatment.paint();
	    mode_athletics_radio_button_fin_treatment.paint();
	    mode_athletics_radio_button_football.paint();
	    mode_athletics_graphdbar_winloss.paint();
	}

	//-------- graph title --------------//
	font_chartsm.center_put(
	    REPORT_X1+MODE_ATHLETICS_TAB0,
	    REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+10,
	    REPORT_X1+MODE_ATHLETICS_TAB0+MODE_ATHLETICS_GRAPH_WIDTH,
	    REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+30,
	    mode_athletics_graph_title);

	//-------- graph text --------------//
	font_chartsm.put(REPORT_X1+MODE_ATHLETICS_TAB1,
			 REPORT_Y1+15, mode_athletics_str4[0]);
	font_chartsm.put(REPORT_X1+MODE_ATHLETICS_TAB1,
			 REPORT_Y1+40, mode_athletics_str4[1]);
	font_chartsm.put(REPORT_X1+MODE_ATHLETICS_TAB1,
			 REPORT_Y1+65, mode_athletics_str4[2]);
	font_chartsm.put(REPORT_X1+MODE_ATHLETICS_TAB1,
			 REPORT_Y1+90, mode_athletics_str4[3]);

	//-------- draw the legend --------------//
	font_chartsm.put(
	    REPORT_X1+MODE_ATHLETICS_LEGEND_WIDTH/3+25,
	    (REPORT_Y2*2-MODE_ATHLETICS_LEGEND_HEIGHT-font_chartsm.max_font_height)/2+4,
	    mode_athletics_legend_label[0]);
	font_chartsm.put(
	    REPORT_X1+2*MODE_ATHLETICS_LEGEND_WIDTH/3+25,
	    (REPORT_Y2*2-MODE_ATHLETICS_LEGEND_HEIGHT-font_chartsm.max_font_height)/2+4,
	    mode_athletics_legend_label[1]);
	user_interface.bar(
	    REPORT_X1+MODE_ATHLETICS_LEGEND_WIDTH/3+4,
	    (REPORT_Y2*2-MODE_ATHLETICS_LEGEND_HEIGHT)/2-6,
	    REPORT_X1+MODE_ATHLETICS_LEGEND_WIDTH/3+16,
	    (REPORT_Y2*2-MODE_ATHLETICS_LEGEND_HEIGHT)/2+6,
	    (mode_athletics_graphdbar_winloss.series_color)[0]);
	user_interface.bar(
	    REPORT_X1+2*MODE_ATHLETICS_LEGEND_WIDTH/3+4,
	    (REPORT_Y2*2-MODE_ATHLETICS_LEGEND_HEIGHT)/2-6,
	    REPORT_X1+2*MODE_ATHLETICS_LEGEND_WIDTH/3+16,
	    (REPORT_Y2*2-MODE_ATHLETICS_LEGEND_HEIGHT)/2+6,
	    (mode_athletics_graphdbar_winloss.series_color)[1]);
    }
    else {                                          // refreshFlag == INFO_REFRESH
	// -------- refresh the stats ----------- //

	mode_athletics_radio_button_football.refresh();
	mode_athletics_radio_button_participation.refresh();
	mode_athletics_graphdbar_winloss.refresh();

	vga.blt_buf(
	    REPORT_X1+MODE_ATHLETICS_TAB0,
	    REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+30,
	    REPORT_X1+MODE_ATHLETICS_TAB0+MODE_ATHLETICS_GRAPH_WIDTH,
	    REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+30+MODE_ATHLETICS_GRAPH_HEIGHT);

	mode_athletics_radio_button_adm_treatment.refresh();
	mode_athletics_radio_button_fin_treatment.refresh();

    }
    // end Kevin 0723
    vga_back.rest_area(mode_athletics_no_top_athletes_bitmap,0,0);
    font_chartsm.center_put( MODE_ATHLETICS_TAB1+320,
			     REPORT_Y1+15,
			     MODE_ATHLETICS_TAB1+320+3*font_chartsm.max_font_width,
			     REPORT_Y1+15+font_chartsm.max_font_height,
			     m.format(top_athletes, 4));

    // ##### Begin Marco ##### //
    font_chart_purple_sm.put(
	MODE_ATHLETICS_TAB1+380,
	REPORT_Y1+15,
	m.format(initial_top_athletes, 13));
    // ##### End Marco ##### //

    vga.blt_buf(MODE_ATHLETICS_TAB1+320,
		REPORT_Y1+15,
		MODE_ATHLETICS_TAB1+320+3*font_chartsm.max_font_width,
		REPORT_Y1+15+font_chartsm.max_font_height);
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
    static int sliderVar1=0;
    //	static float spinnerVar1=0.0;
    static int numSeriesData = 8;                   //*** temporary ***//
    static float *dataArray;                        //*** temporary ***//
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//

    // ##### Begin MArco #### //
    static double initialv21=663.23;
    static double initialv31=813.83;
    // ##### End MArco #### //

  //	Communicate with Sim Engine's Output
    dataArray = (float*)(athletics_office.history_value);
    xStart = (int)(info.game_year+(info.game_month-9.0)/12+2-info.graph_year);
    xEnd = xStart+numSeriesData-1;

    v12=info.financial_year();
    v11=v12-1;
    v13=v12+1;
    const char ix=AC_INTERCOLLEGIATE_ATHLETICS;
    v21=finance.revenue_array[ix].total;
    v22=finance.projected_revenue_array[ix].change_last_year.total;
    v23=finance.projected_revenue_array[ix].this_year.total;
    v24=finance.projected_revenue_array[ix].change_next_year.total;
    v25=finance.projected_revenue_array[ix].next_year.total;
    const char iy=AC_ATHLETICS;
    v31=finance.expense_array[iy].total;
    v32=finance.projected_expense_array[iy].change_last_year.total;
    v33=finance.projected_expense_array[iy].this_year.total;
    v34=finance.projected_expense_array[iy].change_next_year.total;
    v35=finance.projected_expense_array[iy].next_year.total;
    ///////////////

    // ##### Begin MArco #### //
    initialv21=finance.initial_revenue_array[AC_INTERCOLLEGIATE_ATHLETICS].total;
    initialv31=finance.initial_expense_array[AC_ATHLETICS].total;
    // ##### End MArco #### //

    int field_space=5*font_chartsm.max_font_width;

    if (refreshFlag == INFO_REPAINT) {
	user_interface.brighten(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3);
	user_interface.rect(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3,1);
	font_charts.put(REPORT_X1+240,REPORT_Y1-18,"Athletics Revenue and Expenditure");

	if (mode_init_flag != MODE_FINANCIAL) {
	    mode_init_flag = MODE_FINANCIAL;
	    //			spinnerVar1=(float)finance.get_policy_var(false, AC_ATHLETICS)->target_value;
	    //			spinnerVar1=0.0;
	    for (int i = 0; i < 3; i++) {
		financial_button_up_bitmap[i] = NULL;
		financial_button_down_bitmap[i] = NULL;
	    }

	    //--------- draw the brightened boxes ------------//
	    user_interface.brighten(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y2);
	    user_interface.rect(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+3);
	    user_interface.rect(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-3, REPORT_X1+MODE_FINANCIAL_TAB4, REPORT_Y2);
	    user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB4-2, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-3, REPORT_X2, REPORT_Y2);

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

	    financial_spinner_budget.init(
		REPORT_X1+MODE_FINANCIAL_TAB5-10,
		REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+70,
		REPORT_X1+MODE_FINANCIAL_TAB5+110,
		REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+100,
		//### begin zhoubin 02/25/99
		&athletics_office.spinner_var1,
		//### end zhoubin 02/25/99
		//				-100.0f,100.0f,0.1f,0,NULL,23);  //## chea 200899 the min & max val should between -5% to 5% format should in %
		-5.0f,5.0f,0.1f,0,NULL,25);
	    //-------- graph title --------------//
	    mode_financial_graph_profitloss.init(
		REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-3, REPORT_X1+MODE_FINANCIAL_TAB4, REPORT_Y2,
		2, &numSeriesData, dataArray, 1,
		NULL, NULL, mode_financial_legend_label, 1, 1, 1, 2);
	    financial_spinner_budget.paint();
	}
	else {
	    //--------- draw the brightened boxes ------------//
	    user_interface.brighten(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT);
	    user_interface.brighten(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT, REPORT_X1+MODE_FINANCIAL_TAB4-2, REPORT_Y2);
	    user_interface.brighten(REPORT_X1+MODE_FINANCIAL_TAB4, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT, REPORT_X2, REPORT_Y2);
	    user_interface.rect(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT);
	    user_interface.rect(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT, REPORT_X1+MODE_FINANCIAL_TAB4-2, REPORT_Y2);
	    user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB4, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT, REPORT_X2, REPORT_Y2);

	    //-------- paint the buttons -----------//
	    //			mode_financial_slider_budget.paint();
	    financial_spinner_budget.paint();
	    mode_financial_graph_profitloss.paint();
	}

	//-------- display static strings -----------//
	font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+40, mode_financial_str[0]);
	font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+60, mode_financial_str[1]);
	user_interface.bar(REPORT_X1+10, REPORT_Y1+82, REPORT_X2-10, REPORT_Y1+84, V_BROWN);
	font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+95, mode_financial_str[2]);
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				REPORT_Y1+15,
				//					REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				//					REPORT_Y1+15+font_chartsm.max_font_height,
				mode_financial_str[4]);
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				REPORT_Y1+15,
				//					REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				//					REPORT_Y1+15+font_chartsm.max_font_height,
				mode_financial_str[4]);
	font_chartsm.put_paragraph(
	    REPORT_X1+MODE_FINANCIAL_TAB4+30,
	    REPORT_Y1+30+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X2,
	    REPORT_Y1+60+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_str[5]);
	mode_financial_stats_bitmap=NULL;
	mode_financial_stats_bitmap=vga_back.save_area(
	    REPORT_X1+MODE_FINANCIAL_TAB2, REPORT_Y1,
	    REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_stats_bitmap);

	mode_financial_graph_profitloss.paint();
    }
    else {                                          // refreshFlag == INFO_REFRESH
	//-------- refresh dynamic data --------------//
	// refresh() ==	1. rest()
	//						2. draw()
	//						3. blit buffer()
	mode_financial_graph_profitloss.refresh();

	vga.blt_buf(
	    REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT,
	    REPORT_X1+MODE_FINANCIAL_TAB4-2, REPORT_Y2
	    );

    }

    financial_spinner_budget.refresh();
    // show buttons
    int pressed=finance.get_policy_var(false, AC_ATHLETICS)->applied_flag;
    if(pressed==P_NONE)
	for(int i=0;i<3;i++)
	    financial_button_group[i].paint();
    else
	financial_button_group.paint(pressed-1);
    short y = FINANCIAL_BUTTON_Y1;
    for (int i = 0; i < 3; i++, y+=FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE) {
	vga.blt_buf(FINANCIAL_BUTTON_X1,y,
		    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT
	    );
    }

    vga_back.rest_area(mode_financial_stats_bitmap,0,0);
    String *s1,*s2;

    //show years
    s1=new String(m.format(v11,16));
    s2=new String(m.format(v11+1,16));
    //		(*s1)+="-";          			//## chea 061199 2.5.1
    //		(*s1)+=(*s2);         			//## chea 061199 2.5.1

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+15,
			    //					REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    //					REPORT_Y1+15+font_chartsm.max_font_height,
			    s1->str_buf);

    (*s1)=m.format(v12,16);
    (*s2)=m.format(v12+1,16);
    //		(*s1)+="-";			//## chea 061199 2.5.1
    //		(*s1)+=(*s2);		//## chea 061199 2.5.1

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+15,
			    //					REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    //					REPORT_Y1+15+font_chartsm.max_font_height,
			    s1->str_buf);

    (*s1)=m.format(v13,16);
    (*s2)=m.format(v13+1,16);
    //		(*s1)+="-";			//## chea 061199 2.5.1
    //		(*s1)+=(*s2);		//## chea 061199 2.5.1
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+15,
			    //					REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    //					REPORT_Y1+15+font_chartsm.max_font_height,
			    s1->str_buf);
    delete s1,s2;

  // ##### Begin Marco ##### //
    if (info.financial_year() > 1 ) {
	font_chart_purple_sm.right_put( REPORT_X1+200+field_space,
					REPORT_Y1+15,
					"Yr. 0 (Initial)");
	font_chart_purple_sm.right_put( REPORT_X1+200+field_space,
					REPORT_Y1+40,
					m.format(initialv21, 2));
	font_chart_purple_sm.right_put( REPORT_X1+200+field_space,
					REPORT_Y1+60,
					m.format(initialv31, 2));
	font_chart_purple_sm.right_put( REPORT_X1+200+field_space,
					REPORT_Y1+95,
					m.format(initialv21-initialv31, 2));
    }
    // ##### End Marco ##### //

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+40,
			    //					REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    //					REPORT_Y1+40+font_chartsm.max_font_height,
			    m.format(v21, 2));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+60,
			    //					REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    //					REPORT_Y1+60+font_chartsm.max_font_height,
			    m.format(v31, 2));
    if((v21-v31)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
				REPORT_Y1+95,
				//					REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
				//					REPORT_Y1+95+font_chartsm.max_font_height,
				m.format(v21-v31, 2));
    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
				     REPORT_Y1+95,
				     //					REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
				     //					REPORT_Y1+95+font_chartsm.max_font_height,
				     m.format(v21-v31, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    REPORT_Y1+40,
			    //					REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    //					REPORT_Y1+40+font_chartsm.max_font_height,
			    m.format(v22, 24));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    REPORT_Y1+60,
			    //					REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    //					REPORT_Y1+60+font_chartsm.max_font_height,
			    m.format(v32, 24));
    if((v22-v32)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				REPORT_Y1+95,
				//					REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				//					REPORT_Y1+95+font_chartsm.max_font_height,
				m.format(v22-v32, 24));
    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				     REPORT_Y1+95,
				     //					REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				     //					REPORT_Y1+95+font_chartsm.max_font_height,
				     m.format(v22-v32, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+40,
			    //					REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    //					REPORT_Y1+40+font_chartsm.max_font_height,
			    m.format(v23, 2));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+60,
			    //					REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    //					REPORT_Y1+60+font_chartsm.max_font_height,
			    m.format(v33, 2));
    if((v23-v33)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
				REPORT_Y1+95,
				//					REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
				//					REPORT_Y1+95+font_chartsm.max_font_height,
				m.format(v23-v33, 2));
    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
				     REPORT_Y1+95,
				     //					REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
				     //					REPORT_Y1+95+font_chartsm.max_font_height,
				     m.format(v23-v33, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    REPORT_Y1+40,
			    //					REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    //					REPORT_Y1+40+font_chartsm.max_font_height,
			    m.format(v24, 24));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    REPORT_Y1+60,
			    //					REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    //					REPORT_Y1+60+font_chartsm.max_font_height,
			    m.format(v34, 24));
    if((v24-v34)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				REPORT_Y1+95,
				//					REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				//					REPORT_Y1+95+font_chartsm.max_font_height,
				m.format(v24-v34, 24));
    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				     REPORT_Y1+95,
				     //					REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				     //					REPORT_Y1+95+font_chartsm.max_font_height,
				     m.format(v24-v34, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+40,
			    //					REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    //					REPORT_Y1+40+font_chartsm.max_font_height,
			    m.format(v25, 2));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+60,
			    //					REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    //					REPORT_Y1+60+font_chartsm.max_font_height,
			    m.format(v35, 2));

    if((v25-v35)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				REPORT_Y1+95,
				//					REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				//					REPORT_Y1+95+font_chartsm.max_font_height,
				m.format(v25-v35, 2));
    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				     REPORT_Y1+95,
				     //					REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				     //					REPORT_Y1+95+font_chart_red_sm.max_font_height,
				     m.format(v25-v35, 2));

    vga.blt_buf(REPORT_X1+MODE_FINANCIAL_TAB2, REPORT_Y1,
		REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT);

    for(int i=0;i<3;i++) {
	financial_button_group[i].pop();
	financial_button_group[i].body_ptr=
	    (void*)(financial_button_up_bitmap[i]+4);
    }
}

//---------- End of function report_FINANCIAL ---------//

//-------- Begin of function detect_athletics ---------//
//!
static int detect_athletics() {
    if (mode_init_flag != MODE_ATHLETICS)
	return 0;

    vga.use_front();

    help.set_help(  REPORT_X1+MODE_ATHLETICS_TAB1,
		    REPORT_Y1+40,
		    REPORT_X2,
		    REPORT_Y1+60,"H5.1.1.");

    help.set_help(  REPORT_X1+MODE_ATHLETICS_TAB1,
		    REPORT_Y1+65,
		    REPORT_X2,
		    REPORT_Y1+85,"H5.1.2.");

    help.set_help(  REPORT_X1+MODE_ATHLETICS_TAB1,
		    REPORT_Y1+90,
		    REPORT_X2,
		    REPORT_Y1+110,"H5.1.3.");

    char* helpCode1[] = {                           // different ncaa level have different number of games, thus we have to separate the help file.
	"H5.1.4a", "H5.1.4b", "H5.1.4c"
    };
    char* helpCode2[] = { "H5.1.5a", "H5.1.5b", "H5.1.5c" };

    help.set_help(  MODE_ATHLETICS_TAB2,
		    REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+MODE_ATHLETICS_BOX1_HEIGHT/2,
		    REPORT_X2,
		    REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+MODE_ATHLETICS_BOX1_HEIGHT/2+30,
		    helpCode1[athletics_office.ncaa_level_input] );

    help.set_help(  MODE_ATHLETICS_TAB2,
		    REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+MODE_ATHLETICS_BOX1_HEIGHT/2+70,
		    REPORT_X2,
		    REPORT_Y1+MODE_ATHLETICS_BOX1_HEIGHT+MODE_ATHLETICS_BOX1_HEIGHT/2+100,
		    helpCode2[athletics_office.ncaa_level_input] );

    if (mode_athletics_radio_button_participation.detect()) {
	//### fred 1021 ####//
	athletics_office.input_2_ncaa_level( *(mode_athletics_radio_button_participation.sel_num) );
	//### fred 1021 ####//
	return 1;
    }

    //## chea 160699 change for chance card
    if(chance_event.athlete_special_ban_year<=0) {
	if (mode_athletics_radio_button_adm_treatment.detect()) {
	    enroll_res.athlete_offers_special=*(mode_athletics_radio_button_adm_treatment.sel_num);
	    return 1;
	}

	if (mode_athletics_radio_button_fin_treatment.detect()) {
	    enroll_res.athlete_aid_special=*(mode_athletics_radio_button_fin_treatment.sel_num);
	    return 1;
	}
    }
    if (mode_athletics_radio_button_football.detect()) {
	sys.redraw_all_flag=1;
	return 1;
    }

    return 0;
}

//---------- End of function detect_athletics ---------//

//-------- Begin of function detect_FINANCIAL ---------//
//!
static int detect_financial() {
    if (mode_init_flag != MODE_FINANCIAL)
	return 0;

    vga.use_front();

    help.set_help(
	REPORT_X1+MODE_FINANCIAL_TAB4+30,
	REPORT_Y1+30+MODE_FINANCIAL_BOX1_HEIGHT,
	REPORT_X2,
	REPORT_Y1+60+MODE_FINANCIAL_BOX1_HEIGHT,
	"A.8.8.1."
	);

    if(financial_spinner_budget.detect())
	return 1;

    for(int i=0;i<3;i++)
	if(financial_button_group[i].detect()) {
	    financial_button_group[i].body_ptr =
		(void *) (financial_button_down_bitmap[i] + 4);
	    //	Communicate with Sim Engine
	    finance.change_budget(i,false, AC_ATHLETICS,(*financial_spinner_budget.var_ptr));
	    //##	begin zhoubin 000329
	    *financial_spinner_budget.var_ptr=0;
	    //##	end zhoubin 000329
	}
    if(mode_financial_slider_budget.detect())
	return 1;

    return 0;
}

//---------- End of function detect_FINANCIAL ---------//

//-------- Begin of function deinit_athletics ---------//
//!
static void deinit_athletics() {
    if (mode_init_flag == MODE_ATHLETICS) {

	mode_athletics_radio_button_football.deinit();
	mode_athletics_radio_button_participation.deinit();
	mode_athletics_radio_button_adm_treatment.deinit();
	mode_athletics_radio_button_fin_treatment.deinit();
	mode_athletics_graphdbar_winloss.deinit();

	if(mode_athletics_no_top_athletes_bitmap) {
	    mem_del(mode_athletics_no_top_athletes_bitmap);
	    mode_athletics_no_top_athletes_bitmap=NULL;
	}

	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_athletics ---------//

//-------- Begin of function deinit_financial ------//
//!
static void deinit_financial() {
    if (mode_init_flag == MODE_FINANCIAL) {
	// deallocate memory used by the radio buttons and the value sliders
	if(mode_financial_stats_bitmap) {
	    mem_del(mode_financial_stats_bitmap);
	    mode_financial_stats_bitmap=NULL;
	}

	// #### begin Gilbert 28/04/2001 #####//
	financial_button_group.deinit_buttons();
	// #### end Gilbert 28/04/2001 #####//
	if (*financial_button_up_bitmap) {
	    for (int i = 0; i < 3; i++) {
		mem_del(financial_button_up_bitmap[i]);
		mem_del(financial_button_down_bitmap[i]);
		financial_button_up_bitmap[i] = NULL;
		financial_button_down_bitmap[i] = NULL;
	    }
	}

	//		mode_financial_slider_budget.deinit();
	financial_spinner_budget.deinit();
	mode_financial_graph_profitloss.deinit();

	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_financial ------//
