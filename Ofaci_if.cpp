//Filename    : OFACI_IF.CPP
//Description : Facility (Facility and Alumni Affairs Office) class
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
#include <OBOX.H>
#include <OSTR.H>
#include <OMISC.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OFACILIT.H>
#include <OVGA.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OText.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OBUTTON.H>
#include <OVALSLDR.H>
#include <OSPINNER.H>
#include <OGRPHYR.H>
#include <OGRPHMTH.H>
#include <OGRPNFM.H>
#include <OGRPARE2.H>
#include <OPIECHRT.H>
#include <OFINANCE.H>
#include <OHELP.H>
#include <ODEPTRES.H>
#include <OMATH.H>

//------- Define constants -------//

enum {                                            // the 'sub' view-modes
    MODE_FINANCIAL = 0,
    MODE_SPACEUSAGE = 1,
    MODE_CAPITAL = 2,
    MODE_DEBT = 3,
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
    BOTTOM_BUTTON_X1 = REPORT_X1+40,
    BOTTOM_BUTTON_Y1 = REPORT_Y2+15,
    BOTTOM_BUTTON_X2 = REPORT_X2-40,
    BOTTOM_BUTTON_Y2 = REPORT_Y2+40
};

enum {
    BOTTOM_BUTTON_WIDTH = 125,
    BOTTOM_BUTTON_HEIGHT = 25,
    BOTTOM_BUTTON_DISTANCE
    = (BOTTOM_BUTTON_X2-BOTTOM_BUTTON_X1-BOTTOM_BUTTON_WIDTH*4)/3
};

enum {
    MODE_FINANCIAL_BOX1_HEIGHT = 68,
    MODE_FINANCIAL_BOX2_HEIGHT = 270,
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
    MODE_SPACEUSAGE_BOX1_HEIGHT = 300,
    MODE_SPACEUSAGE_BOX2_HEIGHT = 60,
    MODE_SPACEUSAGE_BOX_WIDTH = 650,
    MODE_SPACEUSAGE_GRAPH_WIDTH = 300,
    MODE_SPACEUSAGE_GRAPH_HEIGHT = 300,
    MODE_SPACEUSAGE_TAB1 = 10,
    MODE_SPACEUSAGE_VTAB1 = 45,
    MODE_SPACEUSAGE_VTAB2 = REPORT_Y2-40,
    SPACEUSAGE_REC_HEIGHT = 20,
};

enum {
    FINANCIAL_BUTTON_X1 = REPORT_X1+MODE_FINANCIAL_TAB4,
    FINANCIAL_BUTTON_Y1 = REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+180,
    FINANCIAL_BUTTON_X2 = REPORT_X2,
    FINANCIAL_BUTTON_Y2 = REPORT_Y2+10
};

enum {
    FINANCIAL_BUTTON_WIDTH = 200,
    FINANCIAL_BUTTON_HEIGHT = 23,
    FINANCIAL_BUTTON_DISTANCE
    = (FINANCIAL_BUTTON_Y2-FINANCIAL_BUTTON_Y1-FINANCIAL_BUTTON_HEIGHT*4)/3
};

enum {                                            // the 'sub' view-modes
    MODE_FINANCIAL_IMPLEMENT = 2,
    MODE_FINANCIAL_ANNOUNCE = 1,
    MODE_FINANCIAL_CONSIDER = 0,
};

enum {
    MODE_CAPITAL_BOX1_HEIGHT = 75,
    MODE_CAPITAL_BOX2_HEIGHT = 260,
    MODE_CAPITAL_GRAPH_WIDTH = 400,
    MODE_CAPITAL_GRAPH_HEIGHT = 160,
    MODE_CAPITAL_LEGEND_HEIGHT = 26,
    MODE_CAPITAL_LEGEND_WIDTH = 400,

    MODE_CAPITAL_TAB1 = 15,
    MODE_CAPITAL_TAB2 = 280,
    MODE_CAPITAL_TAB3 = 360,
    MODE_CAPITAL_TAB4 = 440,
    MODE_CAPITAL_TAB5 = 520,
    MODE_CAPITAL_TAB6 = 600,
};

enum {
    CAPITAL_BUTTON_X1 = REPORT_X1+MODE_CAPITAL_TAB4+30,
    CAPITAL_BUTTON_Y1 = REPORT_Y1+MODE_CAPITAL_BOX1_HEIGHT+150,
    CAPITAL_BUTTON_X2 = REPORT_X2,
    CAPITAL_BUTTON_Y2 = REPORT_Y2
};

enum {
    CAPITAL_BUTTON_WIDTH = 200,
    CAPITAL_BUTTON_HEIGHT = 25,
    CAPITAL_BUTTON_DISTANCE
    = (CAPITAL_BUTTON_Y2-CAPITAL_BUTTON_Y1-CAPITAL_BUTTON_HEIGHT*4)/3
};

enum {                                            // the 'sub' view-modes
    MODE_CAPITAL_IMPLEMENT = 2,
    MODE_CAPITAL_ANNOUNCE = 1,
    MODE_CAPITAL_CONSIDER = 0,
};

enum {
    MODE_DEBT_BOX1_HEIGHT = 70,
    MODE_DEBT_BOX2_HEIGHT = 250,
    MODE_DEBT_GRAPH_WIDTH = 400,
    MODE_DEBT_GRAPH_HEIGHT = 340,

    MODE_DEBT_TAB1 = 400,
    MODE_DEBT_TAB2 = 405,
    MODE_DEBT_TAB3 = 430,

    MODE_DEBT_VTAB1 = 165,
    MODE_DEBT_VTAB2 = 170,

};

enum {
    DEBT_BUTTON_X1 = REPORT_X1+MODE_DEBT_TAB2+40,
    DEBT_BUTTON_Y1 = REPORT_Y1+120,
    DEBT_BUTTON_X2 = REPORT_X2,
    DEBT_BUTTON_Y2 = REPORT_Y1+150
};

enum {
    DEBT_BUTTON_WIDTH = 200,
    DEBT_BUTTON_HEIGHT = 25,
    DEBT_BUTTON_DISTANCE  =0
};

enum {
    DEBT_BUTTON2_X1 = REPORT_X1+MODE_DEBT_TAB2+40,
    DEBT_BUTTON2_Y1 = MODE_DEBT_VTAB2+200,
    DEBT_BUTTON2_X2 = REPORT_X2,
    DEBT_BUTTON2_Y2 = MODE_DEBT_VTAB2+230
};

enum {
    DEBT_BUTTON2_WIDTH = 200,
    DEBT_BUTTON2_HEIGHT = 25,
    DEBT_BUTTON2_DISTANCE =0
};

enum {                                            // the 'sub' view-modes
    MODE_DEBT_IMPLEMENT = 2,
    MODE_DEBT_ANNOUNCE = 1,
    MODE_DEBT_CONSIDER = 0,
};

//-------- define static vars -----------//

static char report_init_flag = 0;
static char mode_init_flag = -1;                  // -1: non of the mode was init'ed

static ButtonGroup bottom_button_group(4);
static short *bottom_button_up_bitmap[4];
static short *bottom_button_down_bitmap[4];
static char *bottom_button_label[] = {
    "O&M", "Space in use", "Capital reserve", "Debt"
};

// Notes:	the strings could be moved to Ogamestr.cpp

// Begin Financial Mode Setting
static char *mode_financial_str[] = {
    "O&M expenditure",
    "Amount needed to offset without decay",
    "Current O&M budget",
    "Change in deferred maintenance backlog",
    "Facilities staff morale",
    "Change",
    "Change in O&M expenditure budget",
};

static short *mode_financial_stats_bitmap;
static short *mode_financial_stats2_bitmap;
static Spinner financial_spinner_budget1;
static Spinner financial_spinner_budget2;
static Spinner square_feet_spinner;
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
    "O&M expenditure", "Total deferred maintenance backlog",
};

// End Financial Mode Setting

// Begin SPACEUSAGE Mode Setting
static GraphNonFinancialMonth mode_spaceusage_graph;
static short *mode_spaceusage_stats_bitmap;
static char *mode_spaceusage_legend[]= {
    "Norm               ",
    "Actual             ",
    "Projected          ",
    "","",
};
//static char *mode_spaceusage_title[]={"Space availability in relation to the norm"};
static int  report_spaceusage_recno = 1;
static Text report_spaceusage_text;
static char *mode_spaceusage_str[] = {
    "Surplus space valved at $",
    "Investment needed to increase the space availability to the norm",
    "New construction authorization",
    "Square feet (K)",
    "Investment required  ($K)",
    "Additional debt  ($K)",
    "Authorize now",
};
static void       disp_fin_rep_row(int,int,int,int);
static Text investment_required;
static Text additional_debt;
static Button authorize_button;

// End SPACEUSAGE Mode Setting

// Begin capital Mode Setting
static char *mode_capital_str[] = {
    "Transfer to capital reserve",
    "Expenditures from capital reserve",
    "Capital reserve balance",
    "",
    "Change",
    "Change in annual transfer to \r\ncapital reserve",
    " :"
};

static short *mode_capital_stats_bitmap;
static Spinner capital_spinner_budget1;
static Spinner capital_spinner_budget2;
static ButtonGroup capital_button_group(3);
static short *capital_button_up_bitmap[3];
static short *capital_button_down_bitmap[3];
static char *capital_button_label[] = {
    "Consider for next year", "Promise for next year", "Implement now"
};
static char capital_current_mode = MODE_CAPITAL_CONSIDER;

static char *mode_capital_legend_label[] = {
    "Gifts to facilities", "Expenditures from capital reserve", "Transfer to capital reserve"
};

static GraphYear mode_capital_graph_profitloss;
static char *mode_capital_graph_title="";

// End capital Mode Setting

// Begin debt Mode Setting
static char *mode_debt_str[] = {
    "Percent of new construction funded by debt",   //## chea 110899 change by b.massy
    "University debt limit",
    "",
    "",
    "Change",
    "Change in O&M expenditure budget",
    " :"
};

static short *mode_debt_stats_bitmap;
static Spinner debt_spinner_budget1;
static Spinner debt_spinner_budget2;

static ButtonGroup debt_button_group(1);
static short *debt_button_up_bitmap[1];
static short *debt_button_down_bitmap[1];
static char *debt_button_label[] = {
    "Implement now","Consider for next year", "Promise for next year"
};

static ButtonGroup debt_button2_group(1);
static short *debt_button2_up_bitmap[1];
static short *debt_button2_down_bitmap[1];
static char *debt_button2_label[] = {
    "Implement now","Consider for next year", "Promise for next year"
};
static char debt_current_mode = MODE_DEBT_CONSIDER;

static GraphArea2 mode_debt_graph;
static char *mode_debt_graph_title="";
static char *mode_debt_legend_label[] = {
    "Debt capacity",
    "Debt balance",
    "Remaining debt capacity",
    "",
    "University debt limit",
};

// End debt Mode Setting

//-------- define static functions ------//

// Notes:	the deinit_x functions should deallocate memory used by the
//				GraphDoubleBar, ValueSlider, RadioButton, etc. objects
//				the functions are called when a mode-switching is triggered
//	Norris 0716

static void report_spaceusage(int);
static void report_financial(int);
static void report_capital(int);
static void report_debt(int);
static int detect_spaceusage();
static int detect_financial();
static int detect_capital();
static int detect_debt();
static void deinit_spaceusage();
static void deinit_financial();
static void deinit_capital();
static void deinit_debt();

typedef void (*ReportFP)(int refreshFlag);
typedef int (*DetectFP)();
typedef void (*DeinitFP)();

static ReportFP report_func_ptr[] = {
    &report_financial,
    &report_spaceusage,
    &report_capital,
    &report_debt,
};

static DetectFP detect_func_ptr[] = {
    &detect_financial,
    &detect_spaceusage,
    &detect_capital,
    &detect_debt,
};

static DeinitFP deinit_func_ptr[] = {
    &deinit_financial,
    &deinit_spaceusage,
    &deinit_capital,
    &deinit_debt,
};

//------- Begin of function Facility::Facility -----------//
//!
Facility::Facility() {
    init();

}

//--------- End of function Facility::Facility -----------//

//------- Begin of function Facility::~Facility -----------//
//!
Facility::~Facility() {
    deinit();
}

//--------- End of function Facility::~Facility -----------//

//------- Begin of function Facility::deinit ---------------//
//!
void Facility::deinit() {
    //--------- deallocate memory -------------//
    // ##### begin Gilbert 28/04/2001 #####//
    bottom_button_group.deinit_buttons();
    // ##### end Gilbert 28/04/2001 #####//
    for (int i = 0; i < 4; i++) {
	if (bottom_button_up_bitmap[i]) {
	    mem_del(bottom_button_up_bitmap[i]);
	    mem_del(bottom_button_down_bitmap[i]);
	    bottom_button_up_bitmap[i] = NULL;
	    bottom_button_down_bitmap[i] = NULL;
	}
    }

    for (int i = 0; i < 4; i++) {
	(*deinit_func_ptr[i])();
    }

    report_init_flag = 0;
}

//--------- End of function Facility::deinit ---------------//

//------- Begin of function Facility::init -----------------//
//!
void Facility::init() {
    memset(this, 0, sizeof(Facility));

    for (int i = 0; i < 4; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }
    current_mode = MODE_FINANCIAL;

    /* BUGHERE -- 	break at once when the following code is called
       //----- constants or other factors -----//
       // Debt capacity parameters
       maximum_debt_balance_as_a_percent_of_funds_balances = 0.5f;
       max_interest_payments_as_a_percent_of_operating_expenditures = 0.08f;

       //Construction policy parameters
       percent_gap_required_before_new_construction_is_considered = 5f;

       escalation_factor_for_deferrd_maintenance_per_year = 0.025f;

       //---------- init player input/output -------------//

       percent_project_funded_with_debt = 0.333f;

       //BUGHERE assume finance is inited before
       total_debt_capacity[THIS_YEAR] = (int) min(	// min(E22*'$G$12,'$F$257*E23/'$C$74);
       maximum_debt_balance_as_a_percent_of_funds_balances
       * ( finance.last_year.asset_array[AC_ASSET_TOTAL] - finance.last_year.liability_array[AC_LIABILITY_TOTAL] ),
       finance.total_operating_expense.total *  max_interest_payments_as_a_percent_of_operating_expenditures
       / (finance.long_term_debt_interest / 100.0f) );
       err_when(total_debt_capacity[THIS_YEAR]<=0);

       cur_debt_limit = int(0.5f * total_debt_capacity[THIS_YEAR]);

       int beginCapitalReserve = int(finance.last_year.asset_array[AC_CAPITAL_RESERVE]);
       min_capital_reserve_balance = int(beginCapitalReserve * 0.6f);

       ratio_of_square_footage_to_benchmark[THIS_YEAR] = 1.0f;

       //---------- other vars -------------//

       cur_change_backlog = 0;

       //BUGHERE should we do:
       //begin_space_inventory();
       //update_backlog_monthly();
       //run_new_construction();
       /////////////////////////////////////////////////////////////////
    */
}

//--------- End of function Facility::init -----------------//

//------- Begin of function Facility::report ---------------//
//!
void Facility::report(int refreshFlag) {
    vga.use_back();

    // fix in version 2
    sys.is_institution_report = 1;

    if (refreshFlag == INFO_REPAINT) {

	if(report_init_flag) {                        // deinit it first if it has already been initialized
	    deinit();
	}
	report_init_flag = 1;

	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	//----- initialize the bottom sub-menus buttons -----//
	short x = BOTTOM_BUTTON_X1;
	for (int i = 0; i < 4; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {

	    user_interface.create_button_bitmap(x, BOTTOM_BUTTON_Y1,
						x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						bottom_button_label[i], &bottom_button_up_bitmap[i],
						&bottom_button_down_bitmap[i], &vga_back);
	    bottom_button_group[i].create_bitmapW(x, BOTTOM_BUTTON_Y1,
						  x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						  ((i == current_mode) ? bottom_button_down_bitmap[i] : bottom_button_up_bitmap[i])+4);
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

//---------- End of function Facility::report ------------//

//-------- Begin of function Facility::detect ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int Facility::detect() {
    vga.use_back();

    int rc = detect2();

    vga.use_front();

    return rc;
}

//---------- End of function Facility::detect ------------//

//-------- Begin of function Facility::detect2 ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int Facility::detect2() {
    if (!report_init_flag)
	return 0;

    int bottomButtonGroupDetect;
    if ((bottomButtonGroupDetect = bottom_button_group.detect()) >= 0) {
	//---- deinit the modes to free memory ----//
	if(current_mode>=0)
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

//---------- End of function Facility::detect ------------//

//-------- Begin of function report_FINANCIAL ---------//
//!
static void report_financial(int refreshFlag) {
    // Notes:	temporary variables for debugging
    //				to be interfaced with game engine

    static int v11=1000,v12=1999,v13=8888;
    static double v21=663.23,v22=123.23,v23=17.23,v24=723.23,v25=13.23;
    static double v31=813.83,v32=888.23,v33=13.83,v34=188.23,v35=183.23;
    static double v41=88.28,v42=777.23,v43=166.27,v44=777.23,v45=173.23;
    static double v1=700,v2=800,v3=900,v4=123;
    static int sliderVar1=0;
    static float spinnerVar1=0.0;
    static float spinnerVar2;
    static int numSeriesData = HISTORY_YEAR_COUNT;  //*** temporary ***//
    static int *dataArray;                          //*** temporary ***//
    //	static float dataArray[]={25,98,75,32,65,46,38,74,38,75,83,79,52,36,59,23};

    // ##### Begin Marco ##### //
    static double initialv21=663.23;
    // ##### End MArco ###### //
    //	Communicate with Sim Engine's Output
    //	dataArray=(float*)library_tech_office.history_value;

    v12=info.financial_year();
    v11=v12-1;
    v13=v12+1;

    // ##### Begin Marco ##### //
    initialv21=finance.initial_expense_array[AC_OPERATIONS_AND_MAINTENANCE].total;
    // ##### End MArco ###### //

    v21=finance.expense_array[AC_OPERATIONS_AND_MAINTENANCE].total;
    v22=finance.projected_expense_array[AC_OPERATIONS_AND_MAINTENANCE].change_last_year.total;
    v23=finance.projected_expense_array[AC_OPERATIONS_AND_MAINTENANCE].this_year.total;
    v24=finance.projected_expense_array[AC_OPERATIONS_AND_MAINTENANCE].change_next_year.total;
    v25=finance.projected_expense_array[AC_OPERATIONS_AND_MAINTENANCE].next_year.total;
    v1=facility_office.total_normal_onm;
    v2=finance.budget_expense_array[AC_OPERATIONS_AND_MAINTENANCE].total;
    v3=facility_office.begin_backlog;
    v4=facility_office.facility_staff_morale;
    dataArray=facility_office.onm_n_backlog_history;
    ///////////////

    int field_space=5*font_chartsm.max_font_width;

    if (refreshFlag == INFO_REPAINT) {
	// top title
	user_interface.brighten(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3);
	user_interface.rect(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3,1);
	font_charts.put(REPORT_X1+215,REPORT_Y1-18,"Operations and Maintenance Expenditure");

	if(facility_office.current_mode>=0)
	    (*deinit_func_ptr[facility_office.current_mode])();
	//--------- draw the brightened boxes ------------//
	user_interface.brighten(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT);
	user_interface.brighten(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT, REPORT_X1+MODE_FINANCIAL_TAB3-1, REPORT_Y2);
	user_interface.brighten(REPORT_X1+MODE_FINANCIAL_TAB3, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+105);
	user_interface.brighten(REPORT_X1+MODE_FINANCIAL_TAB3, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+110, REPORT_X2, REPORT_Y2);

	user_interface.rect(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT);
	user_interface.rect(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-2, REPORT_X1+MODE_FINANCIAL_TAB3+2, REPORT_Y2);
	user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB3, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-2, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+108);
	user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB3, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+110-2, REPORT_X2, REPORT_Y2);

	mode_init_flag = MODE_FINANCIAL;
	//		spinnerVar1=0.0;
	spinnerVar1=(float)finance.get_policy_var(false, AC_OPERATIONS_AND_MAINTENANCE)->target_value;
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
	    &facility_office.spinner_var1,
	    //### end zhoubin 02/25/99
	    //				-100.0f,100.0f,0.1f,0,NULL,23);
	    -5.0f,5.0f,0.1f,0,NULL,25);                 //## chea 170999

	//		   financial_spinner_budget2.init(
	//				REPORT_X1+MODE_FINANCIAL_TAB5+60,
	//				REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+70,
	//				REPORT_X1+MODE_FINANCIAL_TAB5+180,
	//				REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+100,
	//				&spinnerVar2,
	//				-100.0f,100.0f,5.0f,0,NULL,3);

	//-------- graph title --------------//
	mode_financial_graph_profitloss.init(
	    REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-2,
	    REPORT_X1+MODE_FINANCIAL_TAB3+2, REPORT_Y2,
	    2,
	    &numSeriesData, dataArray,
	    1,
	    NULL, " ",
	    mode_financial_legend_label, 1, 1, 1, 2);

	//-------- paint the buttons -----------//
	financial_spinner_budget1.paint();
	for(int i=0;i<3;i++)
	    financial_button_group[i].paint();

	//-------- display static strings -----------//

	mode_financial_graph_profitloss.paint();

	//		user_interface.bar(REPORT_X1+10, REPORT_Y1+82, REPORT_X2-10, REPORT_Y1+84, V_BROWN);

	font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+40, mode_financial_str[0]);

	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				REPORT_Y1+15,
				//					REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				//					REPORT_Y1+15+font_chartsm.max_font_height,
				mode_financial_str[5]);
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				REPORT_Y1+15,
				//					REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				//					REPORT_Y1+15+font_chartsm.max_font_height,
				mode_financial_str[5]);

	font_chartsm.put_paragraph(
	    REPORT_X1+MODE_FINANCIAL_TAB3+20,
	    REPORT_Y1+21+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X2,
	    REPORT_Y1+41+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_str[1]);
	font_chartsm.put_paragraph(
	    REPORT_X1+MODE_FINANCIAL_TAB3+20,
	    REPORT_Y1+41+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X2,
	    REPORT_Y1+61+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_str[2]);
	font_chartsm.put_paragraph(
	    REPORT_X1+MODE_FINANCIAL_TAB3+20,
	    REPORT_Y1+61+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X2,
	    REPORT_Y1+81+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_str[3]);
	font_chartsm.put_paragraph(
	    REPORT_X1+MODE_FINANCIAL_TAB3+20,
	    REPORT_Y1+81+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X2,
	    REPORT_Y1+101+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_str[4]);

	font_chartsm.put_paragraph(
	    REPORT_X1+MODE_FINANCIAL_TAB3+20,
	    REPORT_Y1+120+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X2,
	    REPORT_Y1+140+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_str[6]);

	//		font_chartsm.put_paragraph(
	//			REPORT_X1+MODE_FINANCIAL_TAB5+5,
	//			REPORT_Y1+30+MODE_FINANCIAL_BOX1_HEIGHT,
	//			REPORT_X1+MODE_FINANCIAL_TAB5+120,
	//			REPORT_Y1+60+MODE_FINANCIAL_BOX1_HEIGHT,
	//			mode_financial_str[6]);

	mode_financial_stats_bitmap=NULL;
	mode_financial_stats_bitmap=vga_back.save_area(
	    REPORT_X1+MODE_FINANCIAL_TAB2, REPORT_Y1,
	    REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_stats_bitmap);
	mode_financial_stats2_bitmap=NULL;
	mode_financial_stats2_bitmap=vga_back.save_area(
	    REPORT_X2-75, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+100,
	    mode_financial_stats2_bitmap);

    }
    else {                                          // refreshFlag == INFO_REFRESH
	//-------- refresh dynamic data --------------//
	// refresh() ==	1. rest()
	//						2. draw()
	//						3. blit buffer()

	//		financial_spinner_budget2.refresh();
	mode_financial_graph_profitloss.refresh();
	vga.blt_buf(REPORT_X1,
		    REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT,
		    REPORT_X1+MODE_FINANCIAL_TAB3-1,
		    REPORT_Y2);
    }
    financial_spinner_budget1.refresh();
    int pressed=finance.get_policy_var(false, AC_OPERATIONS_AND_MAINTENANCE)->applied_flag;
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

    s1=new String(m.format(v11,16));
    s2=new String(m.format(v11+1,16));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+15,
			    s1->str_buf);

    (*s1)=m.format(v12,16);
    (*s2)=m.format(v12+1,16);
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+15,
			    s1->str_buf);

    (*s1)=m.format(v13,16);
    (*s2)=m.format(v13+1,16);
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+15,
			    s1->str_buf);
    delete s1,s2;

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+40,
			    m.format(v21, 2));

    // ##### Begin Marco ##### //
    if (info.financial_year() > 1) {
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2,
					REPORT_Y1+15,
					"Yr. 0 (Initial)");
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2,
					REPORT_Y1+40,
					m.format(initialv21, 2));
    }
    // ##### End Marco ##### //

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    REPORT_Y1+40,
			    m.format(v22, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+40,
			    m.format(v23, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    REPORT_Y1+40,
			    m.format(v24, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
			    REPORT_Y1+40,
			    m.format(v25, 2));

    vga.blt_buf(REPORT_X1+MODE_FINANCIAL_TAB2, REPORT_Y1,
		REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT);

    vga_back.rest_area(mode_financial_stats2_bitmap,0,0);
    font_chartsm.right_put(
	REPORT_X2-87+field_space,
	REPORT_Y1+20+MODE_FINANCIAL_BOX1_HEIGHT,
	//			REPORT_X2,
	//			REPORT_Y1+40+MODE_FINANCIAL_BOX1_HEIGHT,
	m.format(v1,2));
    font_chartsm.right_put(
	REPORT_X2-87+field_space,
	REPORT_Y1+40+MODE_FINANCIAL_BOX1_HEIGHT,
	//			REPORT_X2,
	//			REPORT_Y1+60+MODE_FINANCIAL_BOX1_HEIGHT,
	m.format(v2,2));
    font_chartsm.right_put(
	REPORT_X2-87+field_space,
	REPORT_Y1+60+MODE_FINANCIAL_BOX1_HEIGHT,
	//			REPORT_X2,
	//			REPORT_Y1+80+MODE_FINANCIAL_BOX1_HEIGHT,
	m.format(v3,2));
    font_chartsm.right_put(
	REPORT_X2-87+field_space,
	REPORT_Y1+80+MODE_FINANCIAL_BOX1_HEIGHT,
	//			REPORT_X2,
	//			REPORT_Y1+100+MODE_FINANCIAL_BOX1_HEIGHT,
	m.format(v4,1));
    vga.blt_buf(REPORT_X2-80, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT,
		REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+100 );

    for(int i=0;i<3;i++) {
	financial_button_group[i].pop();
	financial_button_group[i].body_ptr=
	    (void*)(financial_button_up_bitmap[i]+4);
    }
}

//---------- End of function report_FINANCIAL ---------//

//-------- Begin of function report_spaceusage -------------//
//!
static void report_spaceusage(int refreshFlag) {
    static int dataArray[3*HISTORY_MONTH_COUNT];
    static int numSeriesData = HISTORY_MONTH_COUNT;
    static int v1, v2;

    //	Communicate with Sim Engine's Output
    for(int i=0;i<numSeriesData;i++) {
	dataArray[i] = facility_office.normal_area[i];
	dataArray[numSeriesData+i] = facility_office.actual_area[i];
	dataArray[2*numSeriesData+i] = facility_office.projected_area[i];
    }
    ///////////////////////////////////////////////

    v1 = int(facility_office.spinner_var4*department_res.general_dept_info.replacement_cost);
    v2 = int( v1*facility_office.debt_spinner_var1/100 );

    if (refreshFlag == INFO_REPAINT) {
	// top title
	user_interface.brighten(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3);
	user_interface.rect(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3,1);
	font_charts.put(REPORT_X1+290,REPORT_Y1-18,"Space Availability");

	if(facility_office.current_mode>=0)
	    (*deinit_func_ptr[facility_office.current_mode])();

	//---------  initialization  -----------//
	mode_init_flag= MODE_SPACEUSAGE;

	//--------- draw the brightened boxes ------------//

	user_interface.brighten(
	    REPORT_X1+MODE_SPACEUSAGE_TAB1,
	    MODE_SPACEUSAGE_VTAB2,
	    REPORT_X2,
	    REPORT_Y2
	    );

	user_interface.rect(
	    REPORT_X1+MODE_SPACEUSAGE_TAB1,
	    MODE_SPACEUSAGE_VTAB2,
	    REPORT_X2,
	    REPORT_Y2
	    );

	user_interface.rect(
	    REPORT_X1+MODE_SPACEUSAGE_TAB1,
	    REPORT_Y1,
	    REPORT_X2,
	    MODE_SPACEUSAGE_VTAB2
	    );

	mode_spaceusage_graph.init(
	    REPORT_X1+MODE_SPACEUSAGE_TAB1,
	    REPORT_Y1,
	    REPORT_X2/2-1,
	    MODE_SPACEUSAGE_VTAB2,
	    3, &numSeriesData, dataArray, 0,
	    NULL, "Square feet", mode_spaceusage_legend, 1, 1, 1, 12);

	report_spaceusage_text.put
	    ( REPORT_X1+480,
	      MODE_SPACEUSAGE_VTAB2+15,
	      facility_office.total_invest_close_gap,2);

	font_chartsm.center_put( REPORT_X2/2-1, REPORT_Y1+10,
				 REPORT_X2, REPORT_Y1+20,
				 mode_spaceusage_str[2]);

	font_chartsm.put( REPORT_X2/2-1+20, REPORT_Y1+10+50,
			  mode_spaceusage_str[3]);

	font_chartsm.put( REPORT_X2/2-1+20, REPORT_Y1+10+90,
			  mode_spaceusage_str[4]);

	font_chartsm.put( REPORT_X2/2-1+20, REPORT_Y1+10+130,
			  mode_spaceusage_str[5]);

	int oneStep = (int)(0.02*facility_office.normal_area[HISTORY_MONTH_COUNT-1]/1000);

	square_feet_spinner.init(
	    REPORT_X2/2-1+20+170,
	    REPORT_Y1+10+40,
	    REPORT_X2/2-1+20+170+120,
	    REPORT_Y1+10+40+30,
	    //### begin zhoubin 02/25/99
	    &facility_office.spinner_var4,
	    //### end zhoubin 02/25/99
	    //				-100.0f,100.0f,0.1f,0,NULL,23);
	    0,10*oneStep,oneStep,0,NULL,1);             //## chea 170999

	investment_required.init( REPORT_X2/2-1+20+170,
				  REPORT_Y1+10+90,
				  &v1, 1);
	investment_required.paint();

	additional_debt.init( REPORT_X2/2-1+20+170,
			      REPORT_Y1+10+130,
			      &v2, 1);
	additional_debt.paint();

	authorize_button.create_text( REPORT_X2/2-1+20+100,
				      REPORT_Y1+10+170,
				      REPORT_X2/2-1+20+240,
				      REPORT_Y1+10+190,
				      "Authorize now" );
	authorize_button.paint();

	// ##### Begin MArco ##### //
	font_chart_purple_sm.put
	    ( REPORT_X1+550,
	      MODE_SPACEUSAGE_VTAB2+15,
	      m.format(facility_office.initial_total_invest_close_gap, 27));
	// ##### End MArco ##### //

	//-----------Print message------//
	font_chartsm.put(
	    REPORT_X1+35,
	    MODE_SPACEUSAGE_VTAB2+15,
	    mode_spaceusage_str[1]
	    );

	mode_spaceusage_graph.paint();
	/*		font_chartsm.put(
			REPORT_X1+235,
			MODE_SPACEUSAGE_VTAB1+20,
			mode_spaceusage_title[0]
			);
	*/
    }  
    else {                                          // refreshFlag == INFO_REFRESH
	mode_spaceusage_graph.refresh();
	vga.blt_buf(
	    REPORT_X1+MODE_SPACEUSAGE_TAB1,
	    REPORT_Y1,
	    REPORT_X2,
	    MODE_SPACEUSAGE_VTAB2);
	report_spaceusage_text.refresh();

	square_feet_spinner.refresh();
	investment_required.refresh();
	additional_debt.refresh();
    }

    //-----------Print message------//
    /*	
	if(dataArray[numSeriesData]-dataArray[0]>0)
	{
	font_chartsm.put(
        REPORT_X1+35,
        MODE_SPACEUSAGE_VTAB2+15,
        mode_spaceusage_str[0]
	);
	font_chartsm.put(
        REPORT_X1+480,
        MODE_SPACEUSAGE_VTAB2+15,
	m.format(dataArray[numSeriesData]-dataArray[0],1)
	);
	}
	else
	{
    */

    //		font_chartsm.put(
    //				REPORT_X1+35,
    //				MODE_SPACEUSAGE_VTAB2+15,
    //				mode_spaceusage_str[1]
    //			);

    vga.blt_buf(
	REPORT_X1+35,
	MODE_SPACEUSAGE_VTAB2+15,
	REPORT_X2-5,
	REPORT_Y2
	);
    // end Kevin 0904
}

//-------- Begin of function report_CAPITAL ---------//
//!
static void report_capital(int refreshFlag) {
    // Notes:	temporary variables for debugging
    //				to be interfaced with game engine

    static int v11=1000,v12=1999,v13=8888;
    static double v21=663.23,v22=123.23,v23=17.23,v24=723.23,v25=13.23;
    static double v31=813.83,v32=888.23,v33=13.83,v34=188.23,v35=183.23;
    static double v41=88.28,v42=777.23,v43=166.27,v44=777.23,v45=173.23;
    static int sliderVar1=0;
    static float spinnerVar3=0.0;
    static float spinnerVar4=0.0;
    static int numSeriesData = 8;                   //*** temporary ***//
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//

    // ##### Begin Marco ##### //
    static double initialv21=663.23;
    static double initialv31=813.83;
    static double initialv41=88.28;
    // ##### End Marco ##### //
    //	static float *dataArray;														//*** temporary ***//
    //	static float dataArray[]={25,98,75,32,65,46,38,74,38,75,83,79,52,36,59,23,38,74,38,75,83,79,52,36,59,23};
    static int dataArray[24];
    for(int i=0;i<numSeriesData;i++) {
	dataArray[i]=facility_office.gifts_to_facility[i];
	dataArray[i+8]=(int)facility_office.captial_reserve_expense[i];
	dataArray[i+16]=facility_office.transfer_history[i];
    }

    //	Communicate with Sim Engine's Output
    v11=info.game_year-1;
    v12=info.game_year;
    v13=info.game_year+1;
    v21=finance.expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].total;
    v23=finance.projected_expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].this_year.total;
    v25=finance.projected_expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].next_year.total;
    v22 = finance.calc_change(v23,v21);
    v24 = finance.calc_change(v25,v23);

    v31=(float)facility_office.captial_reserve_expense[H_PREV_YEAR];
    v33=(float)facility_office.captial_reserve_expense[H_THIS_YEAR];
    v35=(float)facility_office.captial_reserve_expense[H_NEXT_YEAR];
    v32=finance.calc_change(v33,v31);
    v34=finance.calc_change(v35,v33);

    v41=finance.last_year.asset_array[AC_CAPITAL_RESERVE];
    v43=finance.this_year.asset_array[AC_CAPITAL_RESERVE];
    v45=finance.next_year.asset_array[AC_CAPITAL_RESERVE];
    v42=finance.calc_change(v43,v41);
    v44=finance.calc_change(v45,v43);

  // ##### Begin Marco ##### //
    initialv21=finance.initial_expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].total;
    //	initialv31=(float)facility_office.initial_captial_reserve_expense[H_PREV_YEAR];
    initialv41=finance.initial_last_year.asset_array[AC_CAPITAL_RESERVE];

    initialv31=(float)facility_office.initial_captial_reserve_expense;
    // ##### End Marco ##### //

    xStart = info.game_year-numSeriesData+1;
    xEnd = info.game_year;
    ///////////////

    int field_space=5*font_chartsm.max_font_width;

    if (refreshFlag == INFO_REPAINT) {
	// top title
	user_interface.brighten(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3);
	user_interface.rect(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3,1);
	font_charts.put(REPORT_X1+295,REPORT_Y1-18,"Capital Reserve");

	spinnerVar3 = (float)finance.get_policy_var(false, AC_TRANSFER_TO_CAPITAL_RESERVE)->target_value;

	if(facility_office.current_mode>=0)
	    (*deinit_func_ptr[facility_office.current_mode])();
	mode_init_flag = MODE_CAPITAL;

	for (int i = 0; i < 3; i++) {
	    capital_button_up_bitmap[i] = NULL;
	    capital_button_down_bitmap[i] = NULL;
	}

	//--------- draw the brightened boxes ------------//
	user_interface.brighten(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y2);

	user_interface.rect(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y1+MODE_CAPITAL_BOX1_HEIGHT+6);
	user_interface.rect(REPORT_X1, REPORT_Y2-MODE_CAPITAL_BOX2_HEIGHT, REPORT_X1+MODE_CAPITAL_TAB4-1, REPORT_Y2);
	user_interface.rect(REPORT_X1+MODE_CAPITAL_TAB4-3, REPORT_Y2-MODE_CAPITAL_BOX2_HEIGHT, REPORT_X2, REPORT_Y2);

	short y = CAPITAL_BUTTON_Y1;
	for (int i = 0; i < 3; i++, y+=CAPITAL_BUTTON_HEIGHT+CAPITAL_BUTTON_DISTANCE) {
	    user_interface.create_button_bitmap(CAPITAL_BUTTON_X1,y,
						CAPITAL_BUTTON_X1+CAPITAL_BUTTON_WIDTH,y+CAPITAL_BUTTON_HEIGHT,
						capital_button_label[i], &capital_button_up_bitmap[i],
						&capital_button_down_bitmap[i], &vga_back);

	    capital_button_group[i].create_bitmapW(CAPITAL_BUTTON_X1,y,
						   CAPITAL_BUTTON_X1+CAPITAL_BUTTON_WIDTH,y+CAPITAL_BUTTON_HEIGHT,
						   (capital_button_up_bitmap[i])+4,0);
	}
	capital_spinner_budget1.init(
	    REPORT_X1+MODE_CAPITAL_TAB5-20,
	    REPORT_Y1+MODE_CAPITAL_BOX1_HEIGHT+100,
	    REPORT_X1+MODE_CAPITAL_TAB5+100,
	    REPORT_Y1+MODE_CAPITAL_BOX1_HEIGHT+130 ,
	    //### begin zhoubin 02/25/99
	    &facility_office.spinner_var3,
	    //### end zhoubin 02/25/99
	    //				-100.0f,100.0f,0.1f,0,NULL,23);
	    -5.0f,5.0f,0.1f,0,NULL,25);                 //## chea 170999

	//-------- graph title --------------//
	mode_capital_graph_profitloss.init(
	    REPORT_X1, REPORT_Y2-MODE_CAPITAL_BOX2_HEIGHT,
	    REPORT_X1+MODE_CAPITAL_TAB4-1, REPORT_Y2,
	    3, &numSeriesData, dataArray, 1,
	    NULL," ",mode_capital_legend_label, 1, 1, 1, 2);

	//-------- paint the buttons -----------//
	capital_spinner_budget1.paint();
	for(int i=0;i<3;i++)
	    capital_button_group[i].paint();

	mode_capital_graph_profitloss.paint();

	//-------- display static strings -----------//
	font_chartsm.put(REPORT_X1+MODE_CAPITAL_TAB1, REPORT_Y1+17, mode_capital_str[0]);
	font_chartsm.put(REPORT_X1+MODE_CAPITAL_TAB1, REPORT_Y1+37, mode_capital_str[1]);
	font_chartsm.put(REPORT_X1+MODE_CAPITAL_TAB1, REPORT_Y1+57, mode_capital_str[2]);

	//		user_interface.bar(REPORT_X1+10, REPORT_Y1+82, REPORT_X2-10, REPORT_Y1+84, V_BROWN);
	//		font_chartsm.put(REPORT_X1+MODE_CAPITAL_TAB1, REPORT_Y1+95, mode_capital_str[2]);

	font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB3+field_space,
				REPORT_Y1+5,
				//					REPORT_X1+MODE_CAPITAL_TAB3+field_space,
				//					REPORT_Y1+5+font_chartsm.max_font_height,
				mode_capital_str[4]);
	font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB5+field_space,
				REPORT_Y1+5,
				//					REPORT_X1+MODE_CAPITAL_TAB5+field_space,
				//					REPORT_Y1+5+font_chartsm.max_font_height,
				mode_capital_str[4]);

	font_chartsm.put_paragraph(
	    REPORT_X1+MODE_CAPITAL_TAB4+20,
	    REPORT_Y1+55+MODE_CAPITAL_BOX1_HEIGHT,
	    REPORT_X1+MODE_CAPITAL_TAB5+160,
	    REPORT_Y1+90+MODE_CAPITAL_BOX1_HEIGHT,
	    mode_capital_str[5]);

	mode_capital_stats_bitmap=NULL;
	mode_capital_stats_bitmap=vga_back.save_area(
	    REPORT_X1+MODE_CAPITAL_TAB2, REPORT_Y1,
	    REPORT_X2, REPORT_Y1+MODE_CAPITAL_BOX1_HEIGHT,
	    mode_capital_stats_bitmap);

    }
    else {                                          // refreshFlag == INFO_REFRESH
	//-------- refresh dynamic data --------------//
	// refresh() ==	1. rest()
	//						2. draw()
	//						3. blit buffer()
    }

    //-------------------------------------------//

    capital_spinner_budget1.refresh();
    mode_capital_graph_profitloss.refresh();

    int pressed=finance.get_policy_var(false, AC_TRANSFER_TO_CAPITAL_RESERVE)->applied_flag;
    if(pressed==P_NONE)
	for(int i=0;i<3;i++)
	    capital_button_group[i].paint();
    else
	capital_button_group.paint(pressed-1);

    short y = CAPITAL_BUTTON_Y1;
    for (int i = 0; i < 3; i++, y+=CAPITAL_BUTTON_HEIGHT+CAPITAL_BUTTON_DISTANCE) {
	vga.blt_buf(CAPITAL_BUTTON_X1,y,
		    CAPITAL_BUTTON_X1+CAPITAL_BUTTON_WIDTH,y+CAPITAL_BUTTON_HEIGHT);
    }
    vga_back.rest_area(mode_capital_stats_bitmap,0,0);
    String *s1,*s2;

    //show years

    s1=new String(m.format(v11,16));
    s2=new String(m.format(v11+1,16));
    //		(*s1)+="-";    //## chea 061199 2.5.1
    //		(*s1)+=(*s2);  //## chea 061199 2.5.1
    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB2+field_space,
			    REPORT_Y1+5,
			    //					REPORT_X1+MODE_CAPITAL_TAB2+field_space,
			    //					REPORT_Y1+5+font_chartsm.max_font_height,
			    s1->str_buf);

    (*s1)=m.format(v12,16);
    (*s2)=m.format(v12+1,16);
    //		(*s1)+="-";    //## chea 061199 2.5.1
    //		(*s1)+=(*s2);  //## chea 061199 2.5.1
    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB4+field_space,
			    REPORT_Y1+5,
			    //					REPORT_X1+MODE_CAPITAL_TAB4+field_space,
			    //					REPORT_Y1+5+font_chartsm.max_font_height,
			    s1->str_buf);

    (*s1)=m.format(v13,16);
    (*s2)=m.format(v13+1,16);
    //		(*s1)+="-";    //## chea 061199 2.5.1
    //		(*s1)+=(*s2);  //## chea 061199 2.5.1
    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB6+field_space,
			    REPORT_Y1+5,
			    //					REPORT_X1+MODE_CAPITAL_TAB6+field_space,
			    //					REPORT_Y1+5+font_chartsm.max_font_height,
			    s1->str_buf);
    delete s1,s2;

  //--------- transfer to capital reserve -------//
  // ##### Begin Marco ##### //
    if (info.financial_year() > 1) {
	font_chart_purple_sm.right_put( REPORT_X1+MODE_CAPITAL_TAB2,
					REPORT_Y1+5,
					"Yr. 0 (Initial)");
	font_chart_purple_sm.right_put( REPORT_X1+MODE_CAPITAL_TAB2,
					REPORT_Y1+23,
					m.format(initialv21, 2));
    }
    // ##### End Marco ##### //

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB2+field_space,
			    REPORT_Y1+23,
			    m.format(v21, 2));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB3+field_space,
			    REPORT_Y1+23,
			    m.format(v22, 24));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB4+field_space,
			    REPORT_Y1+23,
			    m.format(v23, 2));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB5+field_space,
			    REPORT_Y1+23,
			    m.format(v24, 24));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB6+field_space,
			    REPORT_Y1+23,
			    m.format(v25, 2));

    //----- expenditures from capital reserve -------//
    // ##### Begin Marco ##### //
    if (info.financial_year() > 1) {
	font_chart_purple_sm.right_put( REPORT_X1+MODE_CAPITAL_TAB2,
					REPORT_Y1+40,
					m.format(initialv31, 2));
    }
    // ##### End Marco ##### //
    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB2+field_space,
			    REPORT_Y1+40,
			    m.format(v31, 2));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB3+field_space,
			    REPORT_Y1+40,
			    m.format(v32, 24));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB4+field_space,
			    REPORT_Y1+40,
			    m.format(v33, 2));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB5+field_space,
			    REPORT_Y1+40,
			    m.format(v34, 24));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB6+field_space,
			    REPORT_Y1+40,
			    m.format(v35, 2));

    //-------- capital reserve balance ---------//
    // ##### Begin Marco ##### //
    if (info.financial_year() > 1) {
	font_chart_purple_sm.right_put( REPORT_X1+MODE_CAPITAL_TAB2,
					REPORT_Y1+57,
					m.format(initialv41, 2));
    }
    // ##### End Marco ##### //
    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB2+field_space,
			    REPORT_Y1+57,
			    m.format(v41, 2));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB3+field_space,
			    REPORT_Y1+57,
			    m.format(v42, 24));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB4+field_space,
			    REPORT_Y1+57,
			    m.format(v43, 2));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB5+field_space,
			    REPORT_Y1+57,
			    m.format(v44, 24));

    font_chartsm.right_put( REPORT_X1+MODE_CAPITAL_TAB6+field_space,
			    REPORT_Y1+57,
			    m.format(v45, 2));

    //-------------------------------------------//

    vga.blt_buf(REPORT_X1+MODE_CAPITAL_TAB2, REPORT_Y1,
		REPORT_X2, REPORT_Y1+MODE_CAPITAL_BOX1_HEIGHT);

    vga.blt_buf(REPORT_X1, REPORT_Y2-MODE_CAPITAL_BOX2_HEIGHT,
		REPORT_X1+MODE_CAPITAL_TAB4-1, REPORT_Y2);

    //-------------------------------------------//

    for(int i=0;i<3;i++) {
	capital_button_group[i].pop();
	capital_button_group[i].body_ptr=
	    (void*)(capital_button_up_bitmap[i]+4);
    }
}

//---------- End of function report_CAPITAL ---------//

//-------- Begin of function report_DEBT ---------//
//!
static void report_debt(int refreshFlag) {
    // Notes:	temporary variables for debugging
    //				to be interfaced with game engine

    static int numSeriesData = 8;                   //*** temporary ***//
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//
    static int dataArray[24];                       //*** temporary ***//
    //	static float dataArray[]={	25,98,75,32,65,46,38,74,
    //										38,75,83,79,52,36,75,83,
    //										79,52,36,59,23,59,23,25,
    //										98,75,32,65,46,38,74,38,
    //										75,62,83,79,52,36,59,23};

    //	Communicate with Sim Engine's Output
    xStart = (int)(info.game_year+(info.game_month-9.0)/12-1);
    xEnd = xStart+numSeriesData;
    for(int i=0;i<numSeriesData;i++) {
	dataArray[i]=facility_office.total_debt_capacity[i];
	dataArray[i+8]=facility_office.outstanding_debt_balance[i];
	dataArray[i+16]=facility_office.debt_limit[i];
    }

    if (refreshFlag == INFO_REPAINT) {
	// top title
	user_interface.brighten(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3);
	user_interface.rect(REPORT_X1+150,REPORT_Y1-23,REPORT_X1+575,REPORT_Y1-3,1);
	font_charts.put(REPORT_X1+320,REPORT_Y1-18,"Debt");

	if(facility_office.current_mode>=0)
	    (*deinit_func_ptr[facility_office.current_mode])();
	mode_init_flag = MODE_DEBT;
	debt_button_up_bitmap[0] = NULL;
	debt_button_down_bitmap[0] = NULL;
	debt_button2_up_bitmap[0] = NULL;
	debt_button2_down_bitmap[0] = NULL;

	//--------- draw the brightened boxes ------------//
	user_interface.brighten(
	    REPORT_X1+MODE_DEBT_TAB2,
	    REPORT_Y1,
	    REPORT_X2,
	    REPORT_Y1+MODE_DEBT_VTAB1);
	user_interface.brighten(
	    REPORT_X1+MODE_DEBT_TAB2,
	    REPORT_Y1+MODE_DEBT_VTAB2,
	    REPORT_X2,
	    REPORT_Y2);

	user_interface.rect(  REPORT_X1+MODE_DEBT_TAB2-2, REPORT_Y1, REPORT_X2,
			      REPORT_Y1+MODE_DEBT_VTAB1+4);
	user_interface.rect(  REPORT_X1+MODE_DEBT_TAB2-2, REPORT_Y1+MODE_DEBT_VTAB2, REPORT_X2,
			      REPORT_Y2);

	//--------- draw the brightened boxes ------------//
	/*		user_interface.brighten(
                        REPORT_X1+MODE_DEBT_TAB2, REPORT_Y1, REPORT_X2,
                        REPORT_Y2);
			user_interface.rect(	REPORT_X1+MODE_DEBT_TAB2, REPORT_Y1, REPORT_X2,
                        REPORT_Y1+MODE_DEBT_VTAB1+3);
			user_interface.rect(	REPORT_X1+MODE_DEBT_TAB2, REPORT_Y1+MODE_DEBT_VTAB2-1, REPORT_X2,
                        REPORT_Y2);
	*/
	user_interface.rect(  REPORT_X1, REPORT_Y1,   
			      REPORT_X1+MODE_DEBT_TAB1+4, REPORT_Y2);

	user_interface.create_button_bitmap(
	    DEBT_BUTTON_X1,DEBT_BUTTON_Y1,
	    DEBT_BUTTON_X1+DEBT_BUTTON_WIDTH,
	    DEBT_BUTTON_Y1+DEBT_BUTTON_HEIGHT,
	    debt_button_label[0], &debt_button_up_bitmap[0],
	    &debt_button_down_bitmap[0], &vga_back);

	user_interface.create_button_bitmap(
	    DEBT_BUTTON2_X1,
	    DEBT_BUTTON2_Y1,
	    DEBT_BUTTON2_X1+DEBT_BUTTON2_WIDTH,
	    DEBT_BUTTON2_Y1+DEBT_BUTTON2_HEIGHT,
	    debt_button2_label[0], &debt_button2_up_bitmap[0],
	    &debt_button2_down_bitmap[0], &vga_back);

	debt_button_group[0].create_bitmapW(DEBT_BUTTON_X1,DEBT_BUTTON_Y1,
					    DEBT_BUTTON_X1+DEBT_BUTTON_WIDTH,DEBT_BUTTON_Y1+DEBT_BUTTON_HEIGHT,
					    (debt_button_up_bitmap[0])+4,0);

	debt_button2_group[0].create_bitmapW(DEBT_BUTTON2_X1,DEBT_BUTTON2_Y1,
					     DEBT_BUTTON2_X1+DEBT_BUTTON2_WIDTH,DEBT_BUTTON2_Y1+DEBT_BUTTON2_HEIGHT,
					     (debt_button2_up_bitmap[0])+4,0);

	debt_spinner_budget1.init(
	    REPORT_X1+MODE_DEBT_TAB3+60,
	    REPORT_Y1+70,
	    REPORT_X1+MODE_DEBT_TAB3+180,
	    REPORT_Y1+100 ,
	    //### begin zhoubin 02/25/99
	    &facility_office.debt_spinner_var1,
	    //### end zhoubin 02/25/99
	    0.0f,100.0f,10.0f,0,NULL,3);
	debt_spinner_budget2.init(
	    REPORT_X1+MODE_DEBT_TAB3+50,
	    REPORT_Y1+MODE_DEBT_VTAB2+70,
	    REPORT_X1+MODE_DEBT_TAB3+190,
	    REPORT_Y1+MODE_DEBT_VTAB2+100,
	    &facility_office.debt_spinner_var2,
	    0.0f,100000000.0f,5000.0f,0,NULL,2);

	//-------- graph title --------------//
	mode_debt_graph.init(
	    REPORT_X1, REPORT_Y1,REPORT_X1+MODE_DEBT_TAB1+4, REPORT_Y2, 3,
	    &numSeriesData,dataArray, 0,facility_office.cur_debt_limit,"",
	    "", mode_debt_legend_label, 1, 1, 1,
	    2);

	//-------- paint the buttons -----------//
	debt_spinner_budget1.paint();
	debt_spinner_budget2.paint();
	debt_button_group[0].paint();
	debt_button2_group[0].paint();

	mode_debt_graph.paint();
	//-------- display static strings -----------//
	font_chartsm.put_paragraph( REPORT_X1+MODE_DEBT_TAB2+15, REPORT_Y1+40,
				    REPORT_X2, REPORT_Y1+70,
				    mode_debt_str[0]);
	font_chartsm.put_paragraph( REPORT_X1+MODE_DEBT_TAB2+15, REPORT_Y1+MODE_DEBT_VTAB2+40,
				    REPORT_X2, REPORT_Y1+MODE_DEBT_VTAB2+70,
				    mode_debt_str[1]);

	//		user_interface.bar(REPORT_X1+10, REPORT_Y1+82, REPORT_X2-10, REPORT_Y1+84, V_BROWN);

    }
    else {                                          // refreshFlag == INFO_REFRESH
	//-------- refresh dynamic data --------------//
	// refresh() ==	1. rest()
	//						2. draw()
	//						3. blit buffer()

	debt_button_group[0].paint();
	debt_button2_group[0].paint();
	debt_spinner_budget1.refresh();
	debt_spinner_budget2.refresh();
	mode_debt_graph.refresh();
	vga.blt_buf(REPORT_X1, REPORT_Y2-MODE_DEBT_GRAPH_HEIGHT,
		    REPORT_X1+MODE_DEBT_TAB1, REPORT_Y2);

	vga.blt_buf(DEBT_BUTTON_X1,DEBT_BUTTON_Y1,
		    DEBT_BUTTON_X1+DEBT_BUTTON_WIDTH,DEBT_BUTTON_Y1+DEBT_BUTTON_HEIGHT);
	vga.blt_buf(DEBT_BUTTON2_X1,DEBT_BUTTON2_Y1,
		    DEBT_BUTTON2_X1+DEBT_BUTTON2_WIDTH,DEBT_BUTTON2_Y1+DEBT_BUTTON2_HEIGHT);

    }

    debt_button_group[0].pop();
    debt_button_group[0].body_ptr=
	(void*)(debt_button_up_bitmap[0]+4);
    debt_button2_group[0].pop();
    debt_button2_group[0].body_ptr=
	(void*)(debt_button2_up_bitmap[0]+4);
}

//---------- End of function report_debt ---------//

//-------- Begin of function detect_financial ---------//
//!
static int detect_financial() {
    if (mode_init_flag != MODE_FINANCIAL)
	return 0;

    vga.use_front();

    help.set_help(
	REPORT_X1+MODE_FINANCIAL_TAB3+20,
	REPORT_Y1+120+MODE_FINANCIAL_BOX1_HEIGHT,
	REPORT_X2,
	REPORT_Y1+140+MODE_FINANCIAL_BOX1_HEIGHT,
	"A.2.2.1.");

    if (financial_spinner_budget1.detect())
	return 1;
    //	if (financial_spinner_budget2.detect())
    //		return 1;

    for(int i=0;i<3;i++)
	if(financial_button_group[i].detect()) {
	    financial_button_group[i].body_ptr =
		(void *) (financial_button_down_bitmap[i] + 4);
	    //	Communicate with Sim Engine
	    finance.change_budget(i,false, AC_OPERATIONS_AND_MAINTENANCE,(*financial_spinner_budget1.var_ptr));
	    //##	begin zhoubin 000329
	    *financial_spinner_budget1.var_ptr=0;
	    //##	end zhoubin 000329
	}

    return 0;
}

//---------- End of function detect_financial--//

//-------- Begin of function detect_spaceusage ---------//
//!
static int detect_spaceusage() {
    if (square_feet_spinner.detect()) {
	square_feet_spinner.paint();
	return 1;
    }
    if ( authorize_button.detect() ) {
	// calculate money available
	int facilityReserve = finance.this_year.asset_array[1];
	float fundedByDebt = facility_office.debt_spinner_var1/100;
	int externalDebtLimit = facility_office.total_debt_capacity[THIS_YEAR];
	int internalDebtLimit = facility_office.debt_limit[THIS_YEAR];
	int existingDebt = facility_office.outstanding_debt_balance[THIS_YEAR];

	int investmentRequirement = int(facility_office.spinner_var4*department_res.general_dept_info.replacement_cost);
	int facilityReserveRequired = investmentRequirement*(1-fundedByDebt);

	int debtRequirement = investmentRequirement*fundedByDebt;

	// check there is enough money for new constructions
	if ( facilityReserveRequired > facilityReserve || debtRequirement > (min( externalDebtLimit, internalDebtLimit )-existingDebt) ) {
	    // not enough money
	    // show dialog box
	    box.msg( "Your capital budget can't fund the proposed new construction. Reduce the program or budget more money." );
	    facility_office.spinner_var4=0;
	    sys.redraw_all_flag=1;
	    return 1;
	}
	else {
	    // have money, start construction
	    facility_office.run_new_construction();
	    facility_office.spinner_var4=0;
	    sys.redraw_all_flag=1;
	    return 1;
	}
    }

    if (mode_init_flag != MODE_SPACEUSAGE)
	return 0;

    vga.use_front();
    return 0;
}

//---------- End of function detect_spaceusage ---------//

//-------- Begin of function detect_capital---//
//!
static int detect_capital() {
    if (mode_init_flag != MODE_CAPITAL)
	return 0;

    vga.use_front();

    help.set_help(
	REPORT_X1+MODE_CAPITAL_TAB4+20,
	REPORT_Y1+55+MODE_CAPITAL_BOX1_HEIGHT,
	REPORT_X1+MODE_CAPITAL_TAB5+160,
	REPORT_Y1+90+MODE_CAPITAL_BOX1_HEIGHT,
	"A.3.3.1.");

    if (capital_spinner_budget1.detect())
	return 1;

    for(int i=0;i<3;i++)
	if(capital_button_group[i].detect()) {
	    capital_button_group[i].body_ptr =
		(void *) (capital_button_down_bitmap[i] + 4);
	    //	Communicate with Sim Engine
	    finance.change_budget(i,false, AC_TRANSFER_TO_CAPITAL_RESERVE,(*capital_spinner_budget1.var_ptr));
	    //##	begin zhoubin 000329
	    *capital_spinner_budget1.var_ptr=0;
	    //##	end zhoubin 000329
	}

    return 0;
}

//---------- End of function detect_CAPITAL ---------//

//-------- Begin of function detect_debt ---------//
//!
static int detect_debt() {
    if (mode_init_flag != MODE_DEBT)
	return 0;

    vga.use_front();

    help.set_help(  REPORT_X1+MODE_DEBT_TAB2+15, REPORT_Y1+40,
		    REPORT_X2, REPORT_Y1+70,
		    "A.4.4.1."
	);
    help.set_help(  REPORT_X1+MODE_DEBT_TAB2+15, REPORT_Y1+MODE_DEBT_VTAB2+40,
		    REPORT_X2, REPORT_Y1+MODE_DEBT_VTAB2+70,
		    "A.4.4.2."
	);

    if (debt_spinner_budget1.detect())
	return 1;

    if (debt_spinner_budget2.detect())
	return 1;

    if(debt_button_group[0].detect()) {
	debt_button_group[0].body_ptr =
	    (void *) (debt_button_down_bitmap[0] + 4);
	//	Communicate with Sim Engine
	facility_office.percent_project_funded_with_debt=(float)(*debt_spinner_budget1.var_ptr);
    }

    if(debt_button2_group[0].detect()) {
	debt_button2_group[0].body_ptr =
	    (void *) (debt_button2_down_bitmap[0] + 4);
	//	Communicate with Sim Engine
	facility_office.cur_debt_limit=(int)(*debt_spinner_budget2.var_ptr);
	facility_office.debt_limit[THIS_YEAR] = facility_office.cur_debt_limit;
	sys.redraw_all_flag=1;
    }

    return 0;
}

//---------- End of function detect_debt--//

//-------- Begin of function deinit_financial ------//
//!
static void deinit_financial() {
    //	if (mode_init_flag == MODE_FINANCIAL)
    //	{
    // deallocate memory used by the radio buttons and the value sliders
    if(mode_financial_stats_bitmap) {
	mem_del(mode_financial_stats_bitmap);
	mode_financial_stats_bitmap=NULL;
    }

    if(mode_financial_stats2_bitmap) {
	mem_del(mode_financial_stats2_bitmap);
	mode_financial_stats2_bitmap=NULL;
    }

    // ##### begin Gilbert 28/04/2001 #####//
    financial_button_group.deinit_buttons();
    // ##### end Gilbert 28/04/2001 #####//
    for (int i = 0; i < 3; i++) {
	if (financial_button_up_bitmap[i]) {
	    mem_del(financial_button_up_bitmap[i]);
	    mem_del(financial_button_down_bitmap[i]);
	    financial_button_up_bitmap[i] = NULL;
	    financial_button_down_bitmap[i] = NULL;
	}
    }

    financial_spinner_budget1.deinit();
    mode_financial_graph_profitloss.deinit();

    // reset flag
    mode_init_flag = -1;
    //	}
}

//---------- End of function deinit_financial ------//

//-------- Begin of function deinit_spaceusage ---------//
//!
static void deinit_spaceusage() {
    // ##### begin Gilbert 28/04/2001 #######//
    mode_spaceusage_graph.deinit();
    report_spaceusage_text.deinit();
    // ##### end Gilbert 28/04/2001 #######//

    if(mode_spaceusage_stats_bitmap) {
	mem_del(mode_spaceusage_stats_bitmap);
	mode_spaceusage_stats_bitmap=NULL;
    }
    report_spaceusage_text.deinit();
    mode_init_flag = -1;
}

//---------- End of function deinit_spaceusage ---------//

//-------- Begin of function deinit_capital ------//
//!
static void deinit_capital() {
    // deallocate memory used by the radio buttons and the value sliders
    if(mode_capital_stats_bitmap) {
	mem_del(mode_capital_stats_bitmap);
	mode_capital_stats_bitmap=NULL;
    }

    // ##### begin Gilbert 28/04/2001 #####//
    capital_button_group.deinit_buttons();
    // ##### end Gilbert 28/04/2001 #####//
    for (int i = 0; i < 3; i++) {
	if (capital_button_up_bitmap[i]) {
	    mem_del(capital_button_up_bitmap[i]);
	    mem_del(capital_button_down_bitmap[i]);
	    capital_button_up_bitmap[i] = NULL;
	    capital_button_down_bitmap[i] = NULL;
	}
    }

    capital_spinner_budget1.deinit();
    mode_capital_graph_profitloss.deinit();

    // reset flag
    mode_init_flag = -1;

}

//---------- End of function deinit_capital ------//

//-------- Begin of function deinit_debt ------//
//!
static void deinit_debt() {
    // deallocate memory used by the radio buttons and the value sliders
    // #### begin Gilbert 28/04/2001 #######//
    debt_button_group.deinit_buttons();
    // #### end Gilbert 28/04/2001 #######//
    if (debt_button_up_bitmap[0]) {
	mem_del(debt_button_up_bitmap[0]);
	mem_del(debt_button_down_bitmap[0]);
	debt_button_up_bitmap[0] = NULL;
	debt_button_down_bitmap[0] = NULL;
    }

    // #### begin Gilbert 28/04/2001 #######//
    debt_button2_group.deinit_buttons();
    // #### end Gilbert 28/04/2001 #######//
    if (debt_button2_up_bitmap[0]) {
	mem_del(debt_button2_up_bitmap[0]);
	mem_del(debt_button2_down_bitmap[0]);
	debt_button2_up_bitmap[0] = NULL;
	debt_button2_down_bitmap[0] = NULL;
    }

    debt_spinner_budget1.deinit();
    debt_spinner_budget2.deinit();
    mode_debt_graph.deinit();
    // reset flag
    mode_init_flag = -1;
}

//---------- End of function deinit_debt ------//
