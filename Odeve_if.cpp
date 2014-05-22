//Filename    : ODEVE_IF.CPP
//Description : Development (Development and Alumni Affairs Office) class
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
#include <OMISC.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <ODEVELOP.H>
#include <OVGA.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OBUTTON.H>
#include <OVALSLDR.H>
#include <OSPINNER.H>
#include <OGRPHYR.H>
#include <OGRPH_PY.H>
#include <OGRPH_PM.H>
#include <OPIECHRT.H>
#include <OFINANCE.H>
#include <OHELP.H>

//------- Define constants -------//

enum {                                            // the 'sub' view-modes
    MODE_DEVELOPMENT = 0,
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
    MODE_DEVELOPMENT_BOX1_HEIGHT = 265,
    MODE_DEVELOPMENT_BOX2_HEIGHT = 60,
    MODE_DEVELOPMENT_BOX_WIDTH = 650,
    MODE_DEVELOPMENT_TAB0 = 40,
    MODE_DEVELOPMENT_TAB1 = 140,
    MODE_DEVELOPMENT_TAB2 = 340,
    MODE_DEVELOPMENT_TAB3 = 530,
    MODE_DEVELOPMENT_TAB4 = 700,
};

enum {
    MODE_DEVELOPMENT_GRAPH_WIDTH = 170,
    MODE_DEVELOPMENT_GRAPH_HEIGHT = 150,
    MODE_DEVELOPMENT_LEGEND_HEIGHT = 26,
    MODE_DEVELOPMENT_LEGEND_WIDTH = 370,
};

// end kevin 0723

enum {
    MODE_FINANCIAL_BOX1_HEIGHT = 120,
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
    "Uses of gifts", "Finance"
};

// Notes:	the strings could be moved to Ogamestr.cpp
// Begin Development Mode Setting
static PieChart mode_development_piechart1;
static PieChart mode_development_piechart2;
static PieChart mode_development_piechart3;
static GraphPerformanceIndicatorYear mode_development_graphicon1;
static GraphPerformanceIndicatorYear mode_development_graphicon2;
static ValueSlider mode_development_slider1;
static ValueSlider mode_development_slider2;
static ValueSlider mode_development_slider3;
static char *mode_development_graph_title="";
static char *mode_development_legend_label[] = {
    "Current Use","Endowment","Facilities"
};

static char *mode_development_str1[] = {
    "Use\n of \nGifts",
};

static char *mode_development_str2[] = {
    "Last year",
    "Projected for this year",
    "Policy target",
    "Alumni who donated over past year",
    "Alumni morale"
};

static short *mode_development_last_year_bitmap;
static short *mode_development_this_year_bitmap;
static short *mode_development_gift_bitmap;
static short *mode_development_morale_bitmap;

// End Development Mode Setting

// Begin Financial Mode Setting
static char *mode_financial_str[] = {
    //	"Gifts revenue",

    //	"Gifts",  //## chea 180899 8.2.1
    "Total gifts",                                  //## chea 121099 8.2.1

    //	"Development expenditure",

    "Development and alumni affairs",               //## chea 180899 8.2.1

    "Balance",
    "",
    "Change",
    "Change in expenditure budget for fund-raising"
};

static short *mode_financial_stats_bitmap;
static Spinner financial_spinner_budget;
static ButtonGroup financial_button_group(3);
static short *financial_button_up_bitmap[3];
static short *financial_button_down_bitmap[3];
static char *financial_button_label[] = {
    "Consider for next year", "Promise for next year", "Implement now"
};
static char financial_current_mode = MODE_FINANCIAL_CONSIDER;

static GraphYear mode_financial_graph_profitloss;
static char *mode_financial_graph_title="";
static char *mode_financial_legend_label[] = {
    "Revenue                ","Expenditure             ","","Ratio of expenditure to revenue",
};

// End Financial Mode Setting

//-------- define static functions ------//

// Notes:	the deinit_x functions should deallocate memory used by the
//				GraphDoubleBar, ValueSlider, RadioButton, etc. objects
//				the functions are called when a mode-switching is triggered
//	Norris 0716

static void report_development(int);
static void report_financial(int);
static int detect_development();
static int detect_financial();
static void deinit_development();
static void deinit_financial();

typedef void (*ReportFP)(int refreshFlag);
typedef int (*DetectFP)();
typedef void (*DeinitFP)();

static ReportFP report_func_ptr[] = {
    &report_development,
    &report_financial
};

static DetectFP detect_func_ptr[] = {
    &detect_development,
    &detect_financial
};

static DeinitFP deinit_func_ptr[] = {
    &deinit_development,
    &deinit_financial,
};

//------- Begin of function Development::Development -----------//
//!
Development::Development() {
    init();
}

//--------- End of function Development::Development -----------//

//------- Begin of function Development::~Development -----------//
//!
Development::~Development() {
    deinit();
}

//--------- End of function Development::~Development -----------//

//------- Begin of function Development::deinit ---------------//
//!
void Development::deinit() {
    //--------- deallocate memory -------------//
    // ##### begin Gilbert 28/04/2001 ######//
    bottom_button_group.deinit_buttons();
    // ##### end Gilbert 28/04/2001 ######//
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

//--------- End of function Development::deinit ---------------//

//------- Begin of function Development::init -----------------//
//!
void Development::init() {
    memset(this,0,sizeof(Development));

    for (int i = 0; i < 2; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }
    current_mode = MODE_DEVELOPMENT;
}

//--------- End of function Development::init -----------------//

//------- Begin of function Development::report ---------------//
//!
void Development::report(int refreshFlag) {
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

//---------- End of function Development::report ------------//

//-------- Begin of function Development::detect ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int Development::detect() {
    vga.use_back();

    int rc = detect2();

    vga.use_front();

    return rc;
}

//---------- End of function Development::detect ------------//

//-------- Begin of function Development::detect2 ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int Development::detect2() {
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

//-------- Begin of function report_development -------------//
//!
static void report_development(int refreshFlag) {
    // begin Kevin 0723
    static int numSeriesData=3;
    static int dataArray1[] = {18000, 23000, 65000};
    static int dataArray2[] = {76000, 19000, 20000};
    static int dataArray3[] = {19000, 20000, 39000};
    static double v11=11,v12=12,v13=13,v14=14;
    static double v21=21,v22=22,v23=23,v24=24;
    static double v31=31,v32=32,v33=33,v34=34;
    static double gift=99,morale=98;
    static int sliderVar1=30,sliderVar2=20,sliderVar3=50;
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//
    static double total;
    static int ys_data_num=HISTORY_YEAR_COUNT;
    static float *ys_dataArray;
    static int ms_data_num=HISTORY_YEAR_COUNT;
    static float *ms_dataArray;

    // ##### Begin Marco 3###### //
    static double initialGift=99,initialMorale=98;  //marco
    // ##### End Marco ###### //

    //	Communicate with Sim Engine's Output
    v12=development_office.last_year.cur_operations;
    v22=development_office.last_year.endowment;
    v32=development_office.last_year.facilities;
    v14=development_office.this_year.cur_operations;
    v24=development_office.this_year.endowment;
    v34=development_office.this_year.facilities;
    total=v12+v22+v32;
    v11=100.0*v12/total;v21=100.0*v22/total;v31=100.0*v32/total;
    total=v14+v24+v34;
    v13=100.0*v14/total;v23=100.0*v24/total;v33=100.0*v34/total;

    sliderVar1=(int)((development_office.allocation_policy.cur_operations+0.005f)*100);
    sliderVar2=(int)((development_office.allocation_policy.endowment+0.005f)*100);
    sliderVar3=(int)((development_office.allocation_policy.facilities+0.005f)*100);
    xStart = info.game_year-numSeriesData+1;
    xEnd = info.game_year;
    total=(sliderVar1+sliderVar2+sliderVar3);
    if(total<1E-9) {
	sliderVar1=33;sliderVar2=33;sliderVar3=34;
	development_office.allocation_policy.cur_operations=0.33f;
	development_office.allocation_policy.endowment=0.33f;
	development_office.allocation_policy.facilities=0.34f;
    }

    dataArray1[0]=(int)v12;dataArray1[1]=(int)v22;dataArray1[2]=(int)v32;
    dataArray2[0]=(int)v14;dataArray2[1]=(int)v24;dataArray2[2]=(int)v34;
    dataArray3[0]=sliderVar1;dataArray3[1]=sliderVar2;dataArray3[2]=sliderVar3;
    gift=development_office.percent_alumni_gave_gift;
    morale=development_office.alumni_morale;
    ys_dataArray=development_office.percent_alumni_gave_gift_history;
    ms_dataArray=development_office.alumni_morale_history;

    // ##### begin MArco ##### //
    initialGift=development_office.initial_percent_alumni_gave_gift;
    initialMorale=development_office.initial_alumni_morale;
    // #### End Marco ##### //

    int field_space=5*font_chartsm.max_font_width;

    if (refreshFlag == INFO_REPAINT) {
	// top title
	user_interface.brighten(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3);
	user_interface.rect(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3,1);
	font_charts.put(REPORT_X1+255,REPORT_Y1-18,"Development and Alumni Affairs");

	//		//--------- draw the brightened boxes ------------//
	user_interface.brighten(REPORT_X1, REPORT_Y1, REPORT_X2,  REPORT_Y2);
	user_interface.rect(REPORT_X1, REPORT_Y1, REPORT_X2,
			    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+1);
	user_interface.rect(REPORT_X1, REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT,
			    REPORT_X2,REPORT_Y2);
	user_interface.rect(
	    REPORT_X1+MODE_DEVELOPMENT_TAB1,
	    REPORT_Y1,
	    REPORT_X1+MODE_DEVELOPMENT_TAB2-6,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+1);

	//		user_interface.rect(
	//				REPORT_X1+MODE_DEVELOPMENT_TAB0-20,
	//				REPORT_Y1+20,
	//				REPORT_X1+MODE_DEVELOPMENT_TAB1-MODE_DEVELOPMENT_TAB0+20,
	//				REPORT_Y1+MODE_DEVELOPMENT_GRAPH_HEIGHT+40);

	user_interface.rect(
	    REPORT_X1+MODE_DEVELOPMENT_TAB2-9,
	    REPORT_Y1,
	    REPORT_X1+MODE_DEVELOPMENT_TAB2+MODE_DEVELOPMENT_GRAPH_WIDTH+11,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+1
	    );

	user_interface.rect(
	    REPORT_X1+MODE_DEVELOPMENT_TAB2+MODE_DEVELOPMENT_GRAPH_WIDTH+8,
	    REPORT_Y1,
	    REPORT_X1+MODE_DEVELOPMENT_TAB3+MODE_DEVELOPMENT_GRAPH_WIDTH+5,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+1
	    );

	/*
	  font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB1+65,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
	  REPORT_X1+MODE_DEVELOPMENT_TAB1+75,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-53,
	  ",");
	  font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB1+65,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-45,
	  REPORT_X1+MODE_DEVELOPMENT_TAB1+75,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-33,
	  ",");
	  font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB1+65,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-25,
	  REPORT_X1+MODE_DEVELOPMENT_TAB1+75,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-13,
	  ",");

	  font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB2+65,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
	  REPORT_X1+MODE_DEVELOPMENT_TAB2+75,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-53,
	  ",");
	  font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB2+65,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-45,
	  REPORT_X1+MODE_DEVELOPMENT_TAB2+75,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-33,
	  ",");
	  font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB2+65,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-25,
	  REPORT_X1+MODE_DEVELOPMENT_TAB2+75,
	  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-13,
	  ",");
	*/
	if (mode_init_flag != MODE_DEVELOPMENT) {
	    //---------  initialization  -----------//
	    mode_init_flag= MODE_DEVELOPMENT;

	    mode_development_last_year_bitmap=NULL;
	    mode_development_this_year_bitmap=NULL;
	    mode_development_gift_bitmap=NULL;
	    mode_development_morale_bitmap=NULL;
	    mode_development_last_year_bitmap=
		vga_back.save_area(
		    REPORT_X1+MODE_DEVELOPMENT_TAB1,
		    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
		    REPORT_X1+MODE_DEVELOPMENT_TAB1+MODE_DEVELOPMENT_GRAPH_WIDTH,
		    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-13,
		    mode_development_last_year_bitmap);
	    mode_development_this_year_bitmap=
		vga_back.save_area(
		    REPORT_X1+MODE_DEVELOPMENT_TAB2,
		    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
		    REPORT_X1+MODE_DEVELOPMENT_TAB2+MODE_DEVELOPMENT_GRAPH_WIDTH,
		    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-13,
		    mode_development_this_year_bitmap);

	    mode_development_gift_bitmap=
		vga_back.save_area(
		    REPORT_X1+MODE_DEVELOPMENT_TAB2,
		    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+15,
		    REPORT_X1+MODE_DEVELOPMENT_TAB2+50,
		    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+15+font_chartsm.max_font_height,
		    mode_development_gift_bitmap);

	    mode_development_morale_bitmap=
		vga_back.save_area(
		    REPORT_X1+MODE_DEVELOPMENT_TAB0+100,
		    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+50,
		    REPORT_X1+MODE_DEVELOPMENT_TAB0+350,
		    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+50+font_chartsm.max_font_height,
		    mode_development_morale_bitmap);

	    //-------- graph title --------------//
	    mode_development_piechart1.init(
		REPORT_X1+MODE_DEVELOPMENT_TAB1+10,
		REPORT_Y1+30,
		REPORT_X1+MODE_DEVELOPMENT_TAB1+MODE_DEVELOPMENT_GRAPH_WIDTH+8,
		REPORT_Y1+30+MODE_DEVELOPMENT_GRAPH_HEIGHT,
		3, dataArray1, &xStart, &xEnd,
		NULL, NULL, NULL, 1, 1, 0, 3);
	    mode_development_piechart2.init(
		REPORT_X1+MODE_DEVELOPMENT_TAB2,
		REPORT_Y1+30,
		REPORT_X1+MODE_DEVELOPMENT_TAB2+MODE_DEVELOPMENT_GRAPH_WIDTH,
		REPORT_Y1+30+MODE_DEVELOPMENT_GRAPH_HEIGHT,
		numSeriesData, dataArray2, &xStart, &xEnd,
		NULL, NULL, NULL, 1, 1, 0, 3);
	    mode_development_piechart3.init(
		REPORT_X1+MODE_DEVELOPMENT_TAB3-3,
		REPORT_Y1+30,
		REPORT_X1+MODE_DEVELOPMENT_TAB3+MODE_DEVELOPMENT_GRAPH_WIDTH-5,
		REPORT_Y1+30+MODE_DEVELOPMENT_GRAPH_HEIGHT,
		numSeriesData, dataArray3, &xStart, &xEnd,
		NULL, NULL, NULL, 1, 1, 0, 3);

	    mode_development_slider1.init(
		REPORT_X1+MODE_DEVELOPMENT_TAB3+10,
		REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
		MODE_DEVELOPMENT_GRAPH_WIDTH-20,
		15,
		0,100,&sliderVar1,0,
		(mode_development_piechart1.series_color)[0],
		3);
	    mode_development_slider2.init(REPORT_X1+MODE_DEVELOPMENT_TAB3+10,
					  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-45,
					  MODE_DEVELOPMENT_GRAPH_WIDTH-20,
					  15,
					  0,100,&sliderVar2,0,
					  (mode_development_piechart1.series_color)[1],
					  3);
	    mode_development_slider3.init(REPORT_X1+MODE_DEVELOPMENT_TAB3+10,
					  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-25,
					  MODE_DEVELOPMENT_GRAPH_WIDTH-20,
					  15,
					  0,100,&sliderVar3,0,
					  (mode_development_piechart1.series_color)[2],
					  3);

	    mode_development_graphicon1.init(
		REPORT_X1+MODE_DEVELOPMENT_TAB3+30,
		REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+5,
		REPORT_X1+MODE_DEVELOPMENT_TAB3+MODE_DEVELOPMENT_GRAPH_WIDTH-30,
		REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+35,
		1 ,&ys_data_num ,ys_dataArray, 1, 0, 1);
	    mode_development_graphicon2.init(
		REPORT_X1+MODE_DEVELOPMENT_TAB3+30,
		REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+40,
		REPORT_X1+MODE_DEVELOPMENT_TAB3+MODE_DEVELOPMENT_GRAPH_WIDTH-30,
		REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+70,
		1 ,&ms_data_num ,ms_dataArray, 1, 0, 1);
	    mode_development_graphicon1.paint();
	    mode_development_graphicon2.paint();

	    mode_development_piechart1.paint();
	    mode_development_piechart2.paint();
	    mode_development_piechart3.paint();

	    mode_development_slider1.refresh();
	    mode_development_slider2.refresh();
	    mode_development_slider3.refresh();
	}
	else {
	    //-------- repaint -----------//
	    mode_development_piechart1.paint();
	    mode_development_piechart2.paint();
	    mode_development_piechart3.paint();
	    mode_development_slider1.paint();
	    mode_development_slider2.paint();
	    mode_development_slider3.paint();
	    mode_development_graphicon1.paint();
	    mode_development_graphicon2.paint();
	}

	//-------- statics text --------------//
	font_chartsm.put(REPORT_X1+MODE_DEVELOPMENT_TAB1+45,
			 REPORT_Y1+10, mode_development_str2[0]);
	font_chartsm.put(REPORT_X1+MODE_DEVELOPMENT_TAB2+10,
			 REPORT_Y1+10, mode_development_str2[1]);
	font_chartsm.put(REPORT_X1+MODE_DEVELOPMENT_TAB3+35,
			 REPORT_Y1+10, mode_development_str2[2]);
	font_chartsm.put(REPORT_X1+MODE_DEVELOPMENT_TAB0,
			 REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+15,
			 mode_development_str2[3]);
	font_chartsm.put(REPORT_X1+MODE_DEVELOPMENT_TAB0,
			 REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+50,
			 mode_development_str2[4]);

	//-----------Print uses of gifts------//
	//		font_chartsm.put(REPORT_X1+MODE_DEVELOPMENT_TAB0+15,
	//								REPORT_Y1+100, "Uses of gifts");

	//-------- draw the legend --------------//
	font_chartsm.put(
	    REPORT_X1+MODE_DEVELOPMENT_TAB0+10,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
	    mode_development_legend_label[0]);
	font_chartsm.put(
	    REPORT_X1+MODE_DEVELOPMENT_TAB0+10,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-45,
	    mode_development_legend_label[1]);
	font_chartsm.put(
	    REPORT_X1+MODE_DEVELOPMENT_TAB0+10,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-25,
	    mode_development_legend_label[2]);
	user_interface.bar(
	    REPORT_X1+MODE_DEVELOPMENT_TAB0-20,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
	    REPORT_X1+MODE_DEVELOPMENT_TAB0-9,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-53,
	    ((mode_development_piechart1.series_color)[0]));
	user_interface.bar(
	    REPORT_X1+MODE_DEVELOPMENT_TAB0-20,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-45,
	    REPORT_X1+MODE_DEVELOPMENT_TAB0-9,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-32,
	    ((mode_development_piechart1.series_color)[1]));
	user_interface.bar(
	    REPORT_X1+MODE_DEVELOPMENT_TAB0-20,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-25,
	    REPORT_X1+MODE_DEVELOPMENT_TAB0-9,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-12,
	    ((mode_development_piechart1.series_color)[2]));
    }
    else {                                          // refreshFlag == INFO_REFRESH

	//	debugging
	//	top_athletes++;

	// -------- refresh the stats ----------- //
	mode_development_piechart1.refresh();
	mode_development_piechart2.refresh();
	mode_development_piechart3.refresh();
	vga.blt_buf(REPORT_X1+MODE_DEVELOPMENT_TAB1+10,
		    REPORT_Y1+30,
		    REPORT_X1+MODE_DEVELOPMENT_TAB1+MODE_DEVELOPMENT_GRAPH_WIDTH+8,
		    REPORT_Y1+30+MODE_DEVELOPMENT_GRAPH_HEIGHT);
	vga.blt_buf(
	    REPORT_X1+MODE_DEVELOPMENT_TAB2,
	    REPORT_Y1+30,
	    REPORT_X1+MODE_DEVELOPMENT_TAB2+MODE_DEVELOPMENT_GRAPH_WIDTH,
	    REPORT_Y1+30+MODE_DEVELOPMENT_GRAPH_HEIGHT);
	vga.blt_buf(
	    REPORT_X1+MODE_DEVELOPMENT_TAB3-3,
	    REPORT_Y1+30,
	    REPORT_X1+MODE_DEVELOPMENT_TAB3+MODE_DEVELOPMENT_GRAPH_WIDTH-5,
	    REPORT_Y1+30+MODE_DEVELOPMENT_GRAPH_HEIGHT);

	mode_development_slider1.refresh();
	mode_development_slider2.refresh();
	mode_development_slider3.refresh();
	mode_development_graphicon1.refresh();
	mode_development_graphicon2.refresh();

	vga.blt_buf(
	    REPORT_X1+MODE_DEVELOPMENT_TAB3+30,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+5,
	    REPORT_X1+MODE_DEVELOPMENT_TAB3+MODE_DEVELOPMENT_GRAPH_WIDTH-30,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+35);
	vga.blt_buf(
	    REPORT_X1+MODE_DEVELOPMENT_TAB3+30,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+40,
	    REPORT_X1+MODE_DEVELOPMENT_TAB3+MODE_DEVELOPMENT_GRAPH_WIDTH-30,
	    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+70);
    }
    vga_back.rest_area(mode_development_last_year_bitmap,0,0);
    vga_back.rest_area(mode_development_this_year_bitmap,0,0);
    vga_back.rest_area(mode_development_gift_bitmap,0,0);
    vga_back.rest_area(mode_development_morale_bitmap,0,0);

    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB1+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB1+85,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-53,
			   m.format((int)v11,3));
    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB1+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-45,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB1+85,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-33,
			   m.format((int)v21,3));
    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB1+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-25,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB1+85,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-13,
			   m.format((int)v31,3));

    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB1+75+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB1+MODE_DEVELOPMENT_GRAPH_WIDTH,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-53,
			   m.format(v12,2));
    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB1+75+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-45,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB1+MODE_DEVELOPMENT_GRAPH_WIDTH,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-33,
			   m.format(v22,2));
    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB1+75+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-25,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB1+MODE_DEVELOPMENT_GRAPH_WIDTH,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-13,
			   m.format(v32,2));

    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB2+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB2+85,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-53,
			   m.format((int)v13,3));
    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB2+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-45,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB2+85,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-33,
			   m.format((int)v23,3));
    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB2+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-25,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB2+85,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-13,
			   m.format((int)v33,3));

    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB2+75+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB2+MODE_DEVELOPMENT_GRAPH_WIDTH,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-53,
			   m.format(v14,2));
    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB2+75+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-45,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB2+MODE_DEVELOPMENT_GRAPH_WIDTH,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-33,
			   m.format(v24,2));
    font_chartsm.right_put(REPORT_X1+MODE_DEVELOPMENT_TAB2+75+field_space,
			   REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-25,
			   //					REPORT_X1+MODE_DEVELOPMENT_TAB2+MODE_DEVELOPMENT_GRAPH_WIDTH,
			   //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-13,
			   m.format(v34,2));

    font_chartsm.center_put(REPORT_X1+MODE_DEVELOPMENT_TAB2,
			    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+15,
			    REPORT_X1+MODE_DEVELOPMENT_TAB2+50,
			    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+15+font_chartsm.max_font_height,
			    m.format((int)gift,3));

    // ##### Begin Marco ##### //
    font_chartsm.center_put(REPORT_X1+MODE_DEVELOPMENT_TAB2,
			    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+50,
			    REPORT_X1+MODE_DEVELOPMENT_TAB2+50,
			    REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+50+font_chartsm.max_font_height,
			    m.format((int)morale,3));

    font_chart_purple_sm.put(REPORT_X1+MODE_DEVELOPMENT_TAB2+50,
			     REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+15,
			     //m.format((int)initial_variable.development_gift,18));
			     m.format((int)initialGift,18));
    font_chart_purple_sm.put(REPORT_X1+MODE_DEVELOPMENT_TAB2+50,
			     REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+50,
			     //m.format((int)initial_variable.development_morale,18));
			     m.format((int)initialMorale,18));

    //		vga_back.indicator(REPORT_X1+MODE_DEVELOPMENT_TAB0+150,
    //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+45,
    //					REPORT_X1+MODE_DEVELOPMENT_TAB0+250,
    //					REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+45+font_chartsm.max_font_height,
    //					(float)morale,
    //					100.0,
    //					VGA_YELLOW,
    //					V_BLACK);

    vga.blt_buf(
	REPORT_X1+MODE_DEVELOPMENT_TAB3+30,
	REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+5,
	REPORT_X1+MODE_DEVELOPMENT_TAB3+MODE_DEVELOPMENT_GRAPH_WIDTH-30,
	REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+35);

    vga.blt_buf(
	REPORT_X1+MODE_DEVELOPMENT_TAB3+30,
	REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+40,
	REPORT_X1+MODE_DEVELOPMENT_TAB3+MODE_DEVELOPMENT_GRAPH_WIDTH-30,
	REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+70);

    vga.blt_buf(REPORT_X1+MODE_DEVELOPMENT_TAB1,
		REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
		REPORT_X1+MODE_DEVELOPMENT_TAB1+MODE_DEVELOPMENT_GRAPH_WIDTH,
		REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-13);

    vga.blt_buf(REPORT_X1+MODE_DEVELOPMENT_TAB2,
		REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
		REPORT_X1+MODE_DEVELOPMENT_TAB2+MODE_DEVELOPMENT_GRAPH_WIDTH,
		REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-13);

    vga.blt_buf(
	REPORT_X1+MODE_DEVELOPMENT_TAB2,
	REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+15,
	REPORT_X1+MODE_DEVELOPMENT_TAB2+50,
	REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+15+font_chartsm.max_font_height);

    vga.blt_buf(
	REPORT_X1+MODE_DEVELOPMENT_TAB0+100,
	REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+50,
	REPORT_X1+MODE_DEVELOPMENT_TAB0+350,
	REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT+50+font_chartsm.max_font_height);

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
    static int sliderVar1=0;
    //	static float spinnerVar1=0.0;
    static int numSeriesData = 8;                   //*** temporary ***//
    static float *dataArray;                        //*** temporary ***//
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//

    // ##### Begin MArco ##### //
    static double initialv21=663.23;
    static double initialv31=813.83;
    // ##### End MArco ##### //

  //	Communicate with Sim Engine's Output
  //## chea 031299 7.2.1 try to disp the graph
  //	dataArray=(float*)development_office.history_value;
    dataArray=(float*)development_office.history_value;

    xStart = info.game_year-numSeriesData+1;
    xEnd = info.game_year;

    v12=info.financial_year();
    v11=v12-1;
    v13=v12+1;

  //## chea 191099 try to display the total gifts
  //	v21=finance.revenue_array[AC_GIFTS].total;
    v21=development_office.last_year.total;

    //## chea 191099 try to display the total gifts
    //	v22=finance.projected_revenue_array[AC_GIFTS].change_last_year.total;
    v22=development_office.gift_change_last_year;

  //## chea 191099 try to display the total gifts
  //	v23=finance.projected_revenue_array[AC_GIFTS].this_year.total;

  //## chea 091299 7.2.3 should add change here
    v23=development_office.this_year.total;
    //	v23=development_office.this_year_total_up2date;

  //## chea 191099 try to display the total gifts
  //	v24=finance.projected_revenue_array[AC_GIFTS].change_next_year.total;
    v24=development_office.gift_change_next_year;

    //## chea 191099 try to display the total gifts
    //	v25=finance.projected_revenue_array[AC_GIFTS].next_year.total;
    v25=development_office.gift_total_next_year;

    v31=finance.expense_array[AC_INST_ADVANCEMENT].total;
    v32=finance.projected_expense_array[AC_INST_ADVANCEMENT].change_last_year.total;
    v33=finance.projected_expense_array[AC_INST_ADVANCEMENT].this_year.total;

    v34=finance.projected_expense_array[AC_INST_ADVANCEMENT].change_next_year.total;
    v35=finance.projected_expense_array[AC_INST_ADVANCEMENT].next_year.total;
    ///////////////

  // ##### Begin MArco ##### //
    initialv21=development_office.initial_gift_total;
    initialv31=finance.initial_expense_array[AC_INST_ADVANCEMENT].total;
    // ##### End MArco ##### //

  //## chea 191099 try to display the total gifts
    double change_total_last_year;
    double change_total_next_year;

    change_total_last_year = finance.calc_change((v23-v33) , (v21-v31));
    change_total_next_year = finance.calc_change((v25-v35) , (v23-v33));

    int field_space=5*font_chartsm.max_font_width;

    if (refreshFlag == INFO_REPAINT) {
	// top title
	user_interface.brighten(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3);
	user_interface.rect(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3,1);
	font_charts.put(REPORT_X1+255,REPORT_Y1-18,"Development and Alumni Affairs");

	if (mode_init_flag != MODE_FINANCIAL) {
	    //			spinnerVar1=0.0;
	    //			spinnerVar1=(float)finance.get_policy_var(false, AC_INST_ADVANCEMENT)->target_value;
	    mode_init_flag = MODE_FINANCIAL;
	    for (int i = 0; i < 3; i++) {
		financial_button_up_bitmap[i] = NULL;
		financial_button_down_bitmap[i] = NULL;
	    }

	    //--------- draw the brightened boxes ------------//
	    user_interface.brighten(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y2);
	    user_interface.rect(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+1);
	    user_interface.rect(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-9, REPORT_X1+MODE_FINANCIAL_TAB4-3, REPORT_Y2);
	    user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB4-5, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-9, REPORT_X2, REPORT_Y2);

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
		&development_office.spinner_var1,
		//### end zhoubin 02/25/99
		//				-100.0f,100.0f,0.1f,0,NULL,23);
		-5.0f,5.0f,0.1f,0,NULL,25);               //## chea 170999

	    //-------- graph title --------------//
	    //## chea 031299 7.2.1 try to disp the graph
	    //			mode_financial_graph_profitloss.init(
	    //				REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-9,
	    //				REPORT_X1+MODE_FINANCIAL_TAB4-3, REPORT_Y2,
	    //				2, &numSeriesData, dataArray, 1,
	    //				NULL, NULL, mode_financial_legend_label, 1, 1, 1, 2);
	    mode_financial_graph_profitloss.init(
		REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-9,
		REPORT_X1+MODE_FINANCIAL_TAB4-3, REPORT_Y2,
		2, &numSeriesData, dataArray, 1,
		NULL, NULL, mode_financial_legend_label, 1, 1, 1, 2);

	    financial_spinner_budget.paint();
	}
	else {
	    //--------- draw the brightened boxes ------------//
	    user_interface.brighten(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y2);
	    user_interface.rect(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+1);
	    user_interface.rect(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-9, REPORT_X1+MODE_FINANCIAL_TAB4-3, REPORT_Y2);
	    user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB4-5, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-9, REPORT_X2, REPORT_Y2);

	    //-------- paint the buttons -----------//
	    financial_spinner_budget.paint();
	}

	mode_financial_graph_profitloss.paint();

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
	    REPORT_X1+MODE_FINANCIAL_TAB4+22,
	    REPORT_Y1+30+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X2,
	    REPORT_Y1+65+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_str[5]);
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

	financial_spinner_budget.refresh();
	mode_financial_graph_profitloss.refresh();
	vga.blt_buf(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT,
		    REPORT_X1+MODE_FINANCIAL_TAB4-10, REPORT_Y2);
    }
    int pressed=finance.get_policy_var(false, AC_INST_ADVANCEMENT)->applied_flag;
    if(pressed==P_NONE)
	for(int i=0;i<3;i++)
	    financial_button_group[i].paint();
    else
	financial_button_group.paint(pressed-1);

    short y = FINANCIAL_BUTTON_Y1;
    for (int i = 0; i < 3; i++, y+=FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE) {
	vga.blt_buf(FINANCIAL_BUTTON_X1,y,
		    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT);
    }
    vga_back.rest_area(mode_financial_stats_bitmap,0,0);
    String *s1,*s2;

    //show years

    s1=new String(m.format(v11,16));
    s2=new String(m.format(v11+1,16));
    //	(*s1)+="-";   //## chea 061199 2.5.1
    //	(*s1)+=(*s2); //## chea 061199 2.5.1
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+15,
			    //				REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    //				REPORT_Y1+15+font_chartsm.max_font_height,
			    s1->str_buf);
    (*s1)=m.format(v12,16);
    (*s2)=m.format(v12+1,16);
    //	(*s1)+="-";   //## chea 061199 2.5.1
    //	(*s1)+=(*s2); //## chea 061199 2.5.1
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+15,
			    //				REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    //				REPORT_Y1+15+font_chartsm.max_font_height,
			    s1->str_buf);
    (*s1)=m.format(v13,16);
    (*s2)=m.format(v13+1,16);
    //	(*s1)+="-";   //## chea 061199 2.5.1
    //	(*s1)+=(*s2); //## chea 061199 2.5.1
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+15,
			    //				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    //				REPORT_Y1+15+font_chartsm.max_font_height,
			    s1->str_buf);
    delete s1,s2;

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+40,
			    //				REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    //				REPORT_Y1+40+font_chartsm.max_font_height,
			    m.format(v21, 2));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+60,
			    //				REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    //				REPORT_Y1+60+font_chartsm.max_font_height,
			    m.format(v31, 2));
    if((v21-v31)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
				REPORT_Y1+95,
				//				REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
				//				REPORT_Y1+95+font_chartsm.max_font_height,
				m.format(v21-v31, 2));
    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
				     REPORT_Y1+95,
				     //				REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
				     //				REPORT_Y1+95+font_chartsm.max_font_height,
				     m.format(v21-v31, 2));

    // ##### Begin Marco ##### //
    if(info.financial_year() > 1) {
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2,
					REPORT_Y1+15,
					"Yr. 0 (Initial)");
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2,
					REPORT_Y1+40,
					//m.format(initial_variable.development_finance_v21, 2));
					m.format(initialv21, 2));
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2,
					REPORT_Y1+60,
					//m.format(initial_variable.development_finance_v31, 2));
					m.format(initialv31, 2));
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2,
					REPORT_Y1+95,
					//m.format(initial_variable.development_finance_v21-initial_variable.development_finance_v31, 2));
					m.format(initialv21-initialv31, 2));
    }
    // ##### End Marco ##### //

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    REPORT_Y1+40,
			    //				REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    //					REPORT_Y1+40+font_chartsm.max_font_height,
			    m.format(v22, 24) );
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    REPORT_Y1+60,
			    m.format(v32, 24) );

    if((change_total_last_year)>=0) {
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				REPORT_Y1+95,
				m.format(change_total_last_year, 24));
    }
    else {
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				     REPORT_Y1+95,
				     m.format(change_total_last_year, 24));
    }

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+40,
			    //				REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    //				REPORT_Y1+40+font_chartsm.max_font_height,
			    m.format(v23, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+60,
			    //				REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    //				REPORT_Y1+60+font_chartsm.max_font_height,
			    m.format(v33, 2));
    if((v23-v33)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
				REPORT_Y1+95,
				//				REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
				//				REPORT_Y1+95+font_chartsm.max_font_height,
				m.format(v23-v33, 2));
    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
				     REPORT_Y1+95,
				     //				REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
				     //				REPORT_Y1+95+font_chartsm.max_font_height,
				     m.format(v23-v33, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    REPORT_Y1+40,
			    //				REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    //				REPORT_Y1+40+font_chartsm.max_font_height,
			    m.format(v24, 24));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    REPORT_Y1+60,
			    //				REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    //				REPORT_Y1+60+font_chartsm.max_font_height,
			    m.format(v34, 24));

    //## chea 191099 try to display the total gifts
    //	if((v24-v34)>=0)
    if((change_total_next_year)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				REPORT_Y1+95,
				//				REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				//				REPORT_Y1+95+font_chartsm.max_font_height,

				//## chea 191099 try to display the total gifts
				//				m.format(v24-v34, 3));
				m.format(change_total_next_year, 24));

    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				     REPORT_Y1+95,
				     //				REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				     //				REPORT_Y1+95+font_chartsm.max_font_height,

				     //## chea 191099 try to display the total gifts
				     //				m.format(v24-v34, 3));
				     m.format(change_total_next_year, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+40,
			    //				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    //				REPORT_Y1+40+font_chartsm.max_font_height,
			    m.format(v25, 2));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+60,
			    //				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    //				REPORT_Y1+60+font_chartsm.max_font_height,
			    m.format(v35, 2));

    if((v25-v35)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				REPORT_Y1+95,
				//				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				//				REPORT_Y1+95+font_chartsm.max_font_height,
				m.format(v25-v35, 2));
    else
	font_chart_red_sm.right_put(REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				    REPORT_Y1+95,
				    //				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				    //				REPORT_Y1+95+font_chart_red_sm.max_font_height,
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

//-------- Begin of function detect_development ---------//
//!
static int detect_development() {
    if (mode_init_flag != MODE_DEVELOPMENT)
	return 0;
    help.set_help(REPORT_X1+MODE_DEVELOPMENT_TAB3+10,
		  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-65,
		  REPORT_X1+MODE_DEVELOPMENT_TAB3+MODE_DEVELOPMENT_GRAPH_WIDTH-10,
		  REPORT_Y1+MODE_DEVELOPMENT_BOX1_HEIGHT-5,
		  "H6.1.1.");

    float sliderVar1,sliderVar2,sliderVar3,total;
    if(mode_development_slider1.detect()) {
	sliderVar1=development_office.allocation_policy.cur_operations=(float)(*(mode_development_slider1.var_ptr)/(float)100.0);
	sliderVar2=(float)(*(mode_development_slider2.var_ptr)/(float)100.0);
	sliderVar3=(float)(*(mode_development_slider3.var_ptr)/(float)100.0);
	total=sliderVar2+sliderVar3;
	if(total>1E-9) {
	    development_office.allocation_policy.endowment=
		(1.0f-sliderVar1)*(sliderVar2/total);
	    development_office.allocation_policy.facilities=
		(1.0f-sliderVar1)*(sliderVar3/total);
	}
	else {
	    development_office.allocation_policy.endowment=
		(0.5f-sliderVar1*0.5f);
	    development_office.allocation_policy.facilities=
		(0.5f-sliderVar1*0.5f);
	}
	return 1;
    }
    if(mode_development_slider2.detect()) {
	sliderVar1=(float)(*(mode_development_slider1.var_ptr)/(float)100.0);
	sliderVar2=development_office.allocation_policy.endowment=(float)(*(mode_development_slider2.var_ptr)/(float)100.0);
	sliderVar3=(float)(*(mode_development_slider3.var_ptr)/(float)100.0);
	total=sliderVar1+sliderVar3;
	if(total>1E-9) {
	    development_office.allocation_policy.cur_operations=
		(1.0f-sliderVar2)*(sliderVar1/total);
	    development_office.allocation_policy.facilities=
		(1.0f-sliderVar2)*(sliderVar3/total);
	}
	else {
	    development_office.allocation_policy.cur_operations=
		(0.5f-sliderVar2*0.5f);
	    development_office.allocation_policy.facilities=
		(0.5f-sliderVar2*0.5f);
	}
	return 1;
    }
    if(mode_development_slider3.detect()) {
	sliderVar1=(float)(*(mode_development_slider1.var_ptr)/(float)100.0);
	sliderVar2=(float)(*(mode_development_slider2.var_ptr)/(float)100.0);
	sliderVar3=development_office.allocation_policy.facilities=(float)(*(mode_development_slider3.var_ptr)/(float)100.0);
	total=sliderVar1+sliderVar2;
	if(total>1E-9) {
	    development_office.allocation_policy.cur_operations=
		(1.0f-sliderVar3)*(sliderVar1/total);
	    development_office.allocation_policy.endowment=
		(1.0f-sliderVar3)*(sliderVar2/total);
	}
	else {
	    development_office.allocation_policy.cur_operations=
		(0.5f-sliderVar3*0.5f);
	    development_office.allocation_policy.endowment=
		(0.5f-sliderVar3*0.5f);
	}
	return 1;
    }
    return 0;
}

//---------- End of function detect_development ---------//

//-------- Begin of function detect_financial ---------//
//!
static int detect_financial() {
    vga.use_front();
    help.set_help(
	REPORT_X1+MODE_FINANCIAL_TAB4+22,
	REPORT_Y1+30+MODE_FINANCIAL_BOX1_HEIGHT,
	REPORT_X2,
	REPORT_Y1+65+MODE_FINANCIAL_BOX1_HEIGHT,
	"A.5.5.1."
	);

    if (mode_init_flag != MODE_FINANCIAL)
	return 0;

    if (financial_spinner_budget.detect())
	return 1;

    for(int i=0;i<3;i++)
	if(financial_button_group[i].detect()) {
	    financial_button_group[i].body_ptr =
		(void *) (financial_button_down_bitmap[i] + 4);
	    //	Communicate with Sim Engine
	    finance.change_budget(i,false, AC_INST_ADVANCEMENT,(*financial_spinner_budget.var_ptr));
	    //##	begin zhoubin 000329
	    *financial_spinner_budget.var_ptr=0;
	    //##	end zhoubin 000329
	}

    return 0;
}

//---------- End of function detect_FINANCIAL ---------//

//-------- Begin of function deinit_development ---------//
//!
static void deinit_development() {
    if (mode_init_flag == MODE_DEVELOPMENT) {

	mode_development_piechart1.deinit();
	mode_development_piechart2.deinit();
	mode_development_piechart3.deinit();
	mode_development_slider1.deinit();
	mode_development_slider2.deinit();
	mode_development_slider3.deinit();

	mode_development_graphicon1.deinit();
	mode_development_graphicon2.deinit();

	if(mode_development_last_year_bitmap) {
	    mem_del(mode_development_last_year_bitmap);
	    mode_development_last_year_bitmap=NULL;
	}
	if(mode_development_this_year_bitmap) {
	    mem_del(mode_development_this_year_bitmap);
	    mode_development_this_year_bitmap=NULL;
	}
	if(mode_development_gift_bitmap) {
	    mem_del(mode_development_gift_bitmap);
	    mode_development_gift_bitmap=NULL;
	}
	if(mode_development_morale_bitmap) {
	    mem_del(mode_development_morale_bitmap);
	    mode_development_morale_bitmap=NULL;
	}

	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_development ---------//

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
	financial_button_group.deinit_buttons();
	// ##### end Gilbert 28/04/2001 ######//
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
