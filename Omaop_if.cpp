//Filename    : OMAOP_IF.CPP
//Description : Modeling_academic (Modeling_academic Operation) class
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
#include <OFACILIT.H>
#include <OVGA.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OBUTTON.H>
#include <OGRAPHA.H>
#include <OGRPAREA.H>

//------- Define constants -------//
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

enum {
    MODE_MAOP_GRAPH_WIDTH = 200,
    MODE_MAOP_GRAPH_HEIGHT = 340,

    MODE_MAOP_TAB1 = 200,
    MODE_MAOP_TAB2 = 400,

    MODE_MAOP_VTAB1 = 15,
    MODE_MAOP_VTAB2 = 170,
};

enum {                                            // the 'sub' view-modes
    MODE_MAOP_IMPLEMENT = 2,
    MODE_MAOP_ANNOUNCE = 1,
    MODE_MAOP_CONSIDER = 0,
};

// Begin Financial Mode Setting
static char *mode_maop_str[] = {
    "Educational quality",
    "Departmental academic standing",
    "Student morale",
    "Faculty publications detail",
    "Faculty morale",
    "Faculty diversity index",
};

static Button maop_detail_button;
static short *maop_detail_button_up_bitmap;
static short *maop_detail_button_down_bitmap;

static short *financial_button_up_bitmap[3];
static short *financial_button_down_bitmap[3];

static short *mode_financial_stats_bitmap;
static short *mode_financial_stats2_bitmap;
static Spinner financial_spinner_budget1;
static Spinner financial_spinner_budget2;
static ButtonGroup financial_button_group(3);
static char *financial_button_label[] = {
    "Consider", "Promise for next year", "Implement now"
};
static char financial_current_mode = MODE_FINANCIAL_CONSIDER;

static GraphA2 mode_financial_graph_profitloss;
static char *mode_financial_graph_title="";
static char *mode_financial_legend_label[] = {
    "O&M expenditure", "Total deferred maintenance backlog",
};

// End Financial Mode Setting

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

/*
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
*/

//------- Begin of function Modeling_academic::Modeling_academic -----------//
//!
Modeling_academic::Modeling_academic() {
    init();
}

//--------- End of function Modeling_academic::Modeling_academic -----------//

//------- Begin of function Modeling_academic::~Modeling_academic -----------//
//!
Modeling_academic::~Modeling_academic() {
    deinit();
}

//--------- End of function Modeling_academic::~Modeling_academic -----------//

//------- Begin of function Modeling_academic::deinit ---------------//
//!
void Modeling_academic::deinit() {
    //--------- deallocate memory -------------//
    if (*bottom_button_up_bitmap) {
	mem_del(maop_detail_button_up_bitmap);
	mem_del(maop_detail_button_down_bitmap);
	bottom_button_up_bitmap = NULL;
	bottom_button_down_bitmap = NULL;
    }

    (*deinit_func_ptr[current_mode])();

    init_flag = 0;
}

//--------- End of function Modeling_academic::deinit ---------------//

//------- Begin of function Modeling_academic::init -----------------//
//!
void Modeling_academic::init() {
    memset(this, 0, sizeof(Modeling_academic));

    for (int i = 0; i < 4; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }

    /* BUGHERE -- 	break at once when the following code is called
       //----- constants or other factors -----//
       // Debt capacity parameters
       maximum_debt_balance_as_a_percent_of_funds_balances = 0.5f;
       max_interest_payments_as_a_percent_of_operating_expenditures = 0.08f;

       //Construction policy parameters
       percent_gap_required_before_new_construction_is_considered = 0.1f;

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

//--------- End of function Modeling_academic::init -----------------//

//------- Begin of function Modeling_academic::report ---------------//
//!
void Modeling_academic::report(int refreshFlag) {
    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	//----- initialize the bottom sub-menus buttons -----//
	if (!init_flag) {
	    short x = BOTTOM_BUTTON_X1;
	    for (int i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
		user_interface.create_button_bitmap(x, BOTTOM_BUTTON_Y1,
						    x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						    bottom_button_label[i], maop_detail_button_up_bitmap,
						    maop_detail_button_down_bitmap, &vga_back);

		maop_detail_button.create_bitmapW(x, BOTTOM_BUTTON_Y1,
						  x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						  ((i == current_mode) ? bottom_button_down_bitmap : bottom_button_up_bitmap)+4);
	    }
	    init_flag = 1;
	}

	//----------- paint the bottom button group ---------//
	maop_detail_button.paint();
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

//---------- End of function Modeling_academic::report ------------//

//-------- Begin of function Modeling_academic::detect ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int Modeling_academic::detect() {
    vga.use_back();

    int rc = detect2();

    vga.use_front();

    return rc;
}

//---------- End of function Modeling_academic::detect ------------//

//-------- Begin of function Modeling_academic::detect2 ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int Modeling_academic::detect2() {
    if (!init_flag)
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

//---------- End of function Modeling_academic::detect ------------//

//-------- Begin of function report_DEBT ---------//
//!
static void report_debt(int refreshFlag) {
    // Notes:	temporary variables for debugging
    //				to be interfaced with game engine

    static float spinnerVar1=0.0;
    static float spinnerVar2=0.0;
    static int numSeriesData = 8;                   //*** temporary ***//
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//
    static int dataArray[24];                       //*** temporary ***//
    //	static float dataArray[]={	25,98,75,32,65,46,38,74,
    //										38,75,83,79,52,36,75,83,
    //										79,52,36,59,23,59,23,25,
    //										98,75,32,65,46,38,74,38,
    //										75,62,83,79,52,36,59,23};

    //	Communicate with Sim Engine's Output
    xStart = info.game_year-numSeriesData+1;
    xEnd = info.game_year;
    for(int i=0;i<numSeriesData;i++) {
	dataArray[i]=facility_office.total_debt_capacity[i];
	dataArray[i+8]=facility_office.outstanding_debt_balance[i];
	dataArray[i+16]=facility_office.debt_limit[i];
    }

    if (refreshFlag == INFO_REPAINT) {

	if (mode_init_flag != MODE_DEBT) {
	    mode_init_flag = MODE_DEBT;
	    debt_button_up_bitmap[0] = NULL;
	    debt_button_down_bitmap[0] = NULL;
	    debt_button2_up_bitmap[0] = NULL;
	    debt_button2_down_bitmap[0] = NULL;

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
		debt_button_label[0], &debt_button2_up_bitmap[0],
		&debt_button2_down_bitmap[0], &vga_back);

	    debt_button_group[0].create_bitmapW(DEBT_BUTTON_X1,DEBT_BUTTON_Y1,
						DEBT_BUTTON_X1+DEBT_BUTTON_WIDTH,DEBT_BUTTON_Y1+DEBT_BUTTON_HEIGHT,
						(debt_button_up_bitmap[0])+4,0);

	    debt_button2_group[0].create_bitmapW(DEBT_BUTTON2_X1,DEBT_BUTTON2_Y1,
						 DEBT_BUTTON2_X1+DEBT_BUTTON2_WIDTH,DEBT_BUTTON2_Y1+DEBT_BUTTON2_HEIGHT,
						 (debt_button2_up_bitmap[0])+4,0);

	    //--------- draw the brightened boxes ------------//
	    user_interface.rect(  REPORT_X1+MODE_DEBT_TAB2, REPORT_Y1, REPORT_X2,
				  REPORT_Y1+MODE_DEBT_VTAB1);
	    user_interface.rect(  REPORT_X1+MODE_DEBT_TAB2, REPORT_Y1+MODE_DEBT_VTAB2, REPORT_X2,
				  REPORT_Y2);

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

	    debt_spinner_budget1.init(
		REPORT_X1+MODE_DEBT_TAB3+60,
		REPORT_Y1+70,
		REPORT_X1+MODE_DEBT_TAB3+180,
		REPORT_Y1+100 ,
		&spinnerVar1,
		-100.0f,100.0f,0.1f,0,NULL,23);
	    debt_spinner_budget2.init(
		REPORT_X1+MODE_DEBT_TAB3+50,
		REPORT_Y1+MODE_DEBT_VTAB2+70,
		REPORT_X1+MODE_DEBT_TAB3+190,
		REPORT_Y1+MODE_DEBT_VTAB2+100,
		&spinnerVar2,
		-100.0f,100.0f,0.1f,0,NULL,2);

	    //-------- graph title --------------//
	    mode_debt_graph.init(
		REPORT_X1, REPORT_Y2-MODE_DEBT_GRAPH_HEIGHT,
		REPORT_X1+MODE_DEBT_TAB1, REPORT_Y2,
		3, &numSeriesData, dataArray, &xStart, &xEnd,
		NULL," ", mode_debt_legend_label, 1, 1, 1, 5);
	}
	else {
	    //--------- draw the brightened boxes ------------//
	    user_interface.rect(  REPORT_X1+MODE_DEBT_TAB2, REPORT_Y1, REPORT_X2,
				  REPORT_Y1+MODE_DEBT_VTAB1);
	    user_interface.rect(  REPORT_X1+MODE_DEBT_TAB2, REPORT_Y1+MODE_DEBT_VTAB2, REPORT_X2,
				  REPORT_Y2);
	    user_interface.brighten(
		REPORT_X1+MODE_DEBT_TAB2, REPORT_Y1, REPORT_X2,
		REPORT_Y1+MODE_DEBT_VTAB1);
	    user_interface.brighten(
		REPORT_X1+MODE_DEBT_TAB2, REPORT_Y1+MODE_DEBT_VTAB2, REPORT_X2,
		REPORT_Y2);

	    //-------- paint the buttons -----------//
	    debt_spinner_budget1.paint();
	    debt_spinner_budget2.paint();
	    mode_debt_graph.paint();
	    debt_button_group[0].paint();
	    debt_button2_group[0].paint();
	}
	//-------- display static strings -----------//
	font_san.put_paragraph( REPORT_X1+MODE_DEBT_TAB2+15, REPORT_Y1+40,
				REPORT_X2, REPORT_Y1+70,
				mode_debt_str[0]);
	font_san.put_paragraph( REPORT_X1+MODE_DEBT_TAB2+15, REPORT_Y1+MODE_DEBT_VTAB2+40,
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
	(void*)(debt_button_up_bitmap[0]+4);
}

//---------- End of function report_debt ---------//

//-------- Begin of function detect_debt ---------//
//!
static int detect_debt() {
    if (mode_init_flag != MODE_DEBT)
	return 0;

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
	facility_office.cur_debt_limit=(int)(*debt_spinner_budget1.var_ptr);
    }

    return 0;
}

//---------- End of function detect_debt--//

//-------- Begin of function deinit_debt ------//
//!
static void deinit_debt() {
    if (mode_init_flag == MODE_FINANCIAL) {
	// deallocate memory used by the radio buttons and the value sliders

	if (*debt_button_up_bitmap) {
	    mem_del(debt_button_up_bitmap[0]);
	    mem_del(debt_button_down_bitmap[0]);
	    debt_button_up_bitmap[0] = NULL;
	    debt_button_down_bitmap[0] = NULL;
	}

	debt_spinner_budget1.deinit();
	debt_spinner_budget2.deinit();
	mode_debt_graph.deinit();

	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_debt ------//
