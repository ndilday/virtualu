//Filename    : OINVE_IF.CPP
//Description : Investment (Investment and Alumni Affairs Office) class
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
#include <OHELP.H>
#include <OGRPHYR.H>
#include <OGRPHMTH.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OINVEST.H>
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
#include <OPIECHRT.H>
#include <OFINANCE.H>
#include <OTEXT.H>
#include <ORADIOV.H>
#include <ORADIO.H>
#include <OBOX.H>
#include <STDIO.H>

//------- Define constants -------//

enum {                                            // the 'sub' view-modes
    MODE_INVESTMENT = 0,
    MODE_FINANCIAL = 1,
    MODE_TRANSFER = 2,
};

enum {                                            // the bg image id
    BG_PIC_ID = 2
};

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+15,
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
    MODE_INVESTMENT_GRAPH_WIDTH = 150,
    MODE_INVESTMENT_GRAPH_HEIGHT = 150,
    MODE_INVESTMENT_LEGEND_HEIGHT = 26,
    MODE_INVESTMENT_LEGEND_WIDTH = 370,
};

// end kevin 0723

enum {
    MODE_FINANCIAL_BOX1_HEIGHT = 100,
    MODE_FINANCIAL_BOX2_HEIGHT = 245,
    MODE_FINANCIAL_GRAPH_WIDTH = 400,
    MODE_FINANCIAL_GRAPH_HEIGHT = 170,
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
    FINANCIAL_BUTTON_X1 = REPORT_X1+MODE_FINANCIAL_TAB4,
    FINANCIAL_BUTTON_Y1 = REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+165,
    FINANCIAL_BUTTON_X2 = REPORT_X2,
    FINANCIAL_BUTTON_Y2 = REPORT_Y2
};

enum {
    FINANCIAL_BUTTON_WIDTH = 200,
    FINANCIAL_BUTTON_HEIGHT = 22,
    FINANCIAL_BUTTON_DISTANCE = 4
};

enum {                                            // the 'sub' view-modes
    MODE_FINANCIAL_IMPLEMENT = 2,
    MODE_FINANCIAL_ANNOUNCE = 1,
    MODE_FINANCIAL_CONSIDER = 0,
};

//-------- define static vars -----------//

static char report_init_flag = 0;
static char mode_init_flag = -1;                  // -1: non of the mode was init'ed

static ButtonGroup bottom_button_group(3);
static short *bottom_button_up_bitmap[3];
static short *bottom_button_down_bitmap[3];
static char *bottom_button_label[] = {
    "Overview1", "Overview2", "Funds transfer"
};

// Notes:	the strings could be moved to Ogamestr.cpp
// Begin Investment Mode Setting
// ### begin Gilbert 02/05/2001 #####//
#ifdef DEBUG
// check memory overwritten
PieChart mode_investment_piechart;                // make global
#else
static PieChart mode_investment_piechart;
#endif
// ### end Gilbert 02/05/2001 #####//

static ValueSlider mode_investment_slider1;
static ValueSlider mode_investment_slider2;
static ValueSlider mode_investment_slider3;
static Text mode_financial_text;
static GraphMonth mode_investment_graph_snp;
static char *mode_investment_legend_label[] = {
    //	"Market index : 60% S&P + 40% bond return  ",
    //## chea 031299 11.1.1
    //	"Endowment performance",
    "Index of Endowment Market Value",
};

//## fred 990507
// Begin Financial Mode Setting
static char *mode_financial_str[] = {
    "Endowment spending",
    "Total endowment market value",
    "New gifts to endowment",
    "",
    "Change",
    ""
};
//## fred 990507

static short *mode_financial_stats_bitmap;
static Spinner financial_spinner_budget;
static ButtonGroup financial_button_group(3);
static short *financial_button_up_bitmap[3];
static short *financial_button_down_bitmap[3];
static char *financial_button_label[] = {
    "Consider for next year", "Promise for next year", "Implement now"
};
static RadioButtonVert mode_financial_rbv;
static char financial_current_mode = MODE_FINANCIAL_CONSIDER;
static GraphYear mode_financial_graph_profitloss;
static char *mode_financial_graph_title="";
static char *mode_financial_rbvlabel[] = {
    "no smoothing",
    "3-year smoothing",
    "5-year smoothing",
};
static char *mode_financial_legend_label[] = {
    "Endowment spending : projected maximum",
    "Projected minimum value",
    "Projected average value",
};

// End Financial Mode Setting

// Begin Fund Transfer
static RadioButtonVert transfer_from;
static RadioButtonVert transfer_to;
static Spinner transfer_per_spinner;
static float spinner_val = 0.0;

static Button implement_button;

static char *transfer_label[]= {
    "Operating reserve",
    "Facilities reserve",
    "Quasi endowment",
};

static int sel_from =0;
static int sel_to = 2;

//-------- define static functions ------//

// Notes:	the deinit_x functions should deallocate memory used by the
//				GraphDoubleBar, ValueSlider, RadioButton, etc. objects
//				the functions are called when a mode-switching is triggered
//	Norris 0716

static void report_investment(int);
static void report_financial(int);
static void report_transfer(int);
static int detect_investment();
static int detect_financial();
static int detect_transfer();
static void deinit_investment();
static void deinit_financial();
static void deinit_transfer();

typedef void (*ReportFP)(int refreshFlag);
typedef int (*DetectFP)();
typedef void (*DeinitFP)();

static ReportFP report_func_ptr[] = {
    &report_investment,
    &report_financial,
    &report_transfer
};

static DetectFP detect_func_ptr[] = {
    &detect_investment,
    &detect_financial,
    &detect_transfer
};

static DeinitFP deinit_func_ptr[] = {
    &deinit_investment,
    &deinit_financial,
    &deinit_transfer
};

//------- Begin of function Investment::Investment -----------//
//!
Investment::Investment() {
    init();
}

//--------- End of function Investment::Investment -----------//

//------- Begin of function Investment::~Investment -----------//
//!
Investment::~Investment() {
    deinit();
}

//--------- End of function Investment::~Investment -----------//

//------- Begin of function Investment::deinit ---------------//
//!
void Investment::deinit() {
    //--------- deallocate memory -------------//
    // ##### begin Gilbert 28/04/2001 #####//
    bottom_button_group.deinit_buttons();
    // ##### end Gilbert 28/04/2001 #####//
    if (*bottom_button_up_bitmap) {
	for (int i = 0; i < 3; i++) {
	    mem_del(bottom_button_up_bitmap[i]);
	    mem_del(bottom_button_down_bitmap[i]);
	    bottom_button_up_bitmap[i] = NULL;
	    bottom_button_down_bitmap[i] = NULL;
	}
    }

    (*deinit_func_ptr[current_mode])();

    report_init_flag = 0;
}

//--------- End of function Investment::deinit ---------------//

//------- Begin of function Investment::init -----------------//
//!
void Investment::init() {
    memset(this,0,sizeof(Investment));

    for (int i = 0; i < 3; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }
    current_mode = MODE_INVESTMENT;
}

//--------- End of function Investment::init -----------------//

//------- Begin of function Investment::report ---------------//
//!
void Investment::report(int refreshFlag) {
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
	    short x = BOTTOM_BUTTON_X1-120;
	    for (int i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {

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

    if (refreshFlag == INFO_REPAINT) {
	//------- blt the back buffer to front buffer -------//
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    }

    vga.use_front();
}

//---------- End of function Investment::report ------------//

//-------- Begin of function Investment::detect ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int Investment::detect() {
    vga.use_back();

    int rc = detect2();

    vga.use_front();

    return rc;
}

//---------- End of function Investment::detect ------------//

//-------- Begin of function Investment::detect2 ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int Investment::detect2() {
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

//-------- Begin of function report_investment -------------//
//!
static void report_investment(int refreshFlag) {
    // begin Kevin 0723

    static int numSeriesData=HISTORY_MONTH_COUNT;
    static int dataArray[] = {18000, 23000, 65000};
    static float dataArray2[2*HISTORY_MONTH_COUNT];
    static int dataArray3[] = {19000, 20000, 39000};
    static float v1=11,v2=0;
    static int sliderVar1=0,sliderVar2=0,sliderVar3=0;
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//
    static double total;

    const static int tab1=REPORT_X1+10;
    const static int tab2=REPORT_X1+360;
    const static int tab3=REPORT_X1+365;
    const static int tab4=REPORT_X1+710;
    const static int vtab1=REPORT_Y1;
    const static int vtab2=REPORT_Y1+180;
    const static int vtab3=REPORT_Y1+185;
    const static int vtab4=REPORT_Y1+345;

    // ##### Begin Marco ##### //
    static int initialSliderVar1=0,initialSliderVar2=0,initialSliderVar3=0;
    //	initialSliderVar1=investment_office.initial_asset_array[0].allocation_percent;
    //	initialSliderVar2=investment_office.initial_asset_array[1].allocation_percent;
    //	initialSliderVar3=investment_office.initial_asset_array[2].allocation_percent;
    initialSliderVar1=investment_office.initial_asset_array0_allocation_percent;
    initialSliderVar2=investment_office.initial_asset_array1_allocation_percent;
    initialSliderVar3=investment_office.initial_asset_array2_allocation_percent;
    // ##### End Marco ##### //

    sliderVar1=investment_office.asset_array[0].allocation_percent;
    sliderVar2=investment_office.asset_array[1].allocation_percent;
    sliderVar3=investment_office.asset_array[2].allocation_percent;
    dataArray[0]=sliderVar1;
    dataArray[1]=sliderVar2;
    dataArray[2]=sliderVar3;

    for(int i=0;i<HISTORY_MONTH_COUNT;i++) {
	//		dataArray2[i]=investment_office.s_n_p_history[i];
	//		dataArray2[i+HISTORY_MONTH_COUNT]=investment_office.endowment_performance_history[i];
	dataArray2[i]=investment_office.endowment_performance_history[i];
    }
    if (refreshFlag == INFO_REPAINT) {
	/*
	  //		//--------- draw the brightened boxes ------------//
	  for (int i = 0; i < 3; i++)
	  {
          financial_button_up_bitmap[i] = NULL;
          financial_button_down_bitmap[i] = NULL;
	  }

	  short y = vtab1+85;
	  for (i = 0; i < 3; i++, y+=25)
	  {
	  user_interface.create_button_bitmap(
	  tab3+90,y,
	  tab3+280,y+23,
	  financial_button_label[i], &financial_button_up_bitmap[i],
	  &financial_button_down_bitmap[i], &vga_back);

	  financial_button_group[i].create_bitmapW(
	  tab3+90,y,
	  tab3+280,y+23,
	  (financial_button_up_bitmap[i])+4,0);
	  }
	*/
	user_interface.brighten(tab1,vtab1+15,tab4,vtab4);
	// ##### Begin Marco ##### //
	user_interface.rect(tab1,vtab1+15,tab4,vtab2);//marco
	// ##### End Marco ##### //
	user_interface.rect(tab1,vtab2-2,tab4,vtab4);
	user_interface.rect(  tab1+5,vtab1+25,
			      tab1+MODE_INVESTMENT_GRAPH_WIDTH,
			      vtab1+MODE_INVESTMENT_GRAPH_HEIGHT+5);
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	font_charts.put(REPORT_X1+315,REPORT_Y1-7,"Asset Allocation");

	if (mode_init_flag != MODE_INVESTMENT) {
	    //---------  initialization  -----------//
	    mode_init_flag= MODE_INVESTMENT;

	    //			financial_spinner_budget.init(
	    //				tab3+120,vtab1+50,
	    //				tab3+240,vtab1+75,
	    //				&v2,
	    //				-100.0f,100.0f,5.0f,0,NULL,3);

	    mode_investment_piechart.init(
		tab1+5,vtab1+25,
		tab1+MODE_INVESTMENT_GRAPH_WIDTH,
		vtab1+MODE_INVESTMENT_GRAPH_HEIGHT+5,
		3, dataArray, &xStart, &xEnd,
		NULL, NULL, NULL, 1, 1, 0, 23);

	    mode_investment_slider1.init(
		tab1+180,vtab1+45,
		MODE_INVESTMENT_GRAPH_WIDTH,
		15,
		0,100,&sliderVar1,0,
		(mode_investment_piechart.series_color)[0],
		3);
	    mode_investment_slider2.init(
		tab1+180,vtab1+100,
		MODE_INVESTMENT_GRAPH_WIDTH,
		15,
		0,100,&sliderVar2,0,
		(mode_investment_piechart.series_color)[1],
		3);
	    mode_investment_slider3.init(
		tab1+180,vtab1+155,
		MODE_INVESTMENT_GRAPH_WIDTH,
		15,
		0,100,&sliderVar3,0,
		(mode_investment_piechart.series_color)[2],
		3);

	    mode_investment_graph_snp.init(
		tab1,vtab2-2,tab4,vtab4,
		1, &numSeriesData, dataArray2, 2,         /*2*/
		//11 //4  //## chea 110899 23 change by b.massy(2dec)
		NULL, NULL, mode_investment_legend_label, 1, 1, 1, 23
		);

	    mode_investment_piechart.paint();
	    //			financial_spinner_budget.paint();
	    //			for( i=0;i<3;i++)
	    //				financial_button_group[i].paint();
	}
	else {
	    //-------- repaint -----------//
	    user_interface.brighten(tab1,vtab1+15,tab4,vtab4);
	    user_interface.rect(tab1,vtab1+15,tab3,vtab2);
	    user_interface.rect(tab3-2,vtab1+15,tab4,vtab2);
	    user_interface.rect(tab1,vtab2-2,tab4,vtab4);

	    //-------- paint the buttons -----------//
	    //			financial_spinner_budget.paint();
	    //			for(int i=0;i<3;i++)
	    //				financial_button_group[i].paint();
	    mode_investment_piechart.paint();
	    mode_investment_slider1.paint();
	    mode_investment_slider2.paint();
	    mode_investment_slider3.paint();
	}
	mode_investment_graph_snp.paint();

	//-------- draw the legend --------------//

	font_chartsm.put(
	    tab1+190,vtab1+25,
	    "Large company stock");
	font_chartsm.put(
	    tab1+190,vtab1+80,
	    "Small company stock");
	font_chartsm.put(
	    tab1+190,vtab1+135,
	    "Bonds");
	user_interface.bar(
	    tab1+160,vtab1+25,tab1+172,vtab1+37,
	    ((mode_investment_piechart.series_color)[0]));
	user_interface.bar(
	    tab1+160,vtab1+80,tab1+172,vtab1+92,
	    ((mode_investment_piechart.series_color)[1]));
	user_interface.bar(
	    tab1+160,vtab1+135,tab1+172,vtab1+147,
	    ((mode_investment_piechart.series_color)[2]));

	// #### Begin Marco ##### //
	font_chart_purple_sm.put(tab3,
				 vtab1+45,
				 "Initial : ");
	font_chart_purple_sm.put(tab3 + 45,
				 vtab1+45,
				 m.format(initialSliderVar1, 3));

	font_chart_purple_sm.put(tab3,
				 vtab1+100,
				 "Initial : ");
	font_chart_purple_sm.put(tab3 + 45,
				 vtab1+100,
				 m.format(initialSliderVar2, 3));
	font_chart_purple_sm.put(tab3,
				 vtab1+155,
				 "Initial : ");
	font_chart_purple_sm.put(tab3 + 45,
				 vtab1+155,
				 m.format(initialSliderVar3, 3));
	// #### End Marco ##### //
    }
    else {                                          // refreshFlag == INFO_REFRESH

	//	debugging
	//	top_athletes++;

	// -------- refresh the stats ----------- //
	mode_investment_piechart.refresh();

	vga.blt_buf(
	    tab1+5,vtab1+25,
	    tab1+MODE_INVESTMENT_GRAPH_WIDTH,
	    vtab1+MODE_INVESTMENT_GRAPH_HEIGHT+5
	    );

	mode_investment_slider1.refresh();
	mode_investment_slider2.refresh();
	mode_investment_slider3.refresh();
	//		for(int i=0;i<3;i++)
	//				financial_button_group[i].paint();
	//		financial_spinner_budget.refresh();

	mode_investment_graph_snp.refresh();
	vga.blt_buf(
	    tab1,vtab2-2,tab4,vtab4
	    );
	/*		short y = vtab1+85;
			for (i = 0; i < 3; i++, y+=25)
			{
			vga.blt_buf(tab3+90,y,tab3+280,y+23);
			}
	*/
    }

    //	vga.blt_buf(REPORT_X1+MODE_FINANCIAL_TAB2, REPORT_Y1,
    //				REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT);

    /*	for(int i=0;i<3;i++)
	{
	financial_button_group[i].pop();
	financial_button_group[i].body_ptr=
        (void*)(financial_button_up_bitmap[i]+4);
	}
    */
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
    static float v1;
    static int v2;
    static float spinnerVar1=0.0;
    static int numSeriesData = P_HISTORY_YEAR_COUNT;//*** temporary ***//
    static int dataArray[3*P_HISTORY_YEAR_COUNT];   //*** temporary ***//
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//
    const static int tab1=REPORT_X1+10;
    const static int tab2=REPORT_X1+360;
    const static int tab3=REPORT_X1+365;
    const static int tab4=REPORT_X1+710;
    const static int vtab1=REPORT_Y1;
    const static int vtab2=REPORT_Y1+165;
    const static int vtab3=REPORT_Y1+170;
    const static int vtab4=REPORT_Y1+330;

    // ##### Begin Marco ##### //
    static double initialv21=663.23;
    static double initialv31=813.83;
    static double initialv41=88.28;
    // ##### End Marco ##### //

  //	Communicate with Sim Engine's Output

  //	dataArray=(float*)development_office.history_value;

    for(int i=0;i<numSeriesData;i++) {
	dataArray[i]=investment_office.endowment_spending_history[2][i];
	dataArray[i+numSeriesData]=investment_office.endowment_spending_history[1][i];
	dataArray[i+2*numSeriesData]=investment_office.endowment_spending_history[0][i];
    }

    xStart = info.game_year-numSeriesData+1;
    xEnd = info.game_year;

    v12=info.financial_year();
    v11=v12-1;
    v13=v12+1;

  //## fred 0405
    int x = AC_ENDOWMENT_SPENDING;
    v21=finance.revenue_array[x].total;
    v23=finance.projected_revenue_array[x].this_year.total;
    v22 = finance.calc_change(v23,v21);

    v24=finance.projected_revenue_array[x].change_next_year.total;
    v25=finance.projected_revenue_array[x].next_year.total;
    //## fred 0405

    //## fred 990507
    /*
      v31=finance.expense_array[5].total;
      v32=finance.projected_expense_array[5].change_last_year.total;
      v33=finance.projected_expense_array[5].this_year.total;
      v34=finance.projected_expense_array[5].change_next_year.total;
      v35=finance.projected_expense_array[5].next_year.total;
  */

    {
	double l = finance.last_year.asset_array[AC_ENDOWMENT]+finance.last_year.asset_array[AC_QUASI];
	double t = finance.this_year.asset_array[AC_ENDOWMENT]+finance.this_year.asset_array[AC_QUASI];
	double n = finance.next_year.asset_array[AC_ENDOWMENT]+finance.this_year.asset_array[AC_QUASI];

	v31 = l;
	v32 = finance.calc_change(t,l);
	v33 = t;
	v34 = finance.calc_change(n,t);
	v35 = n;

	//		double v41,v42,v43,v44,v45;

	l=development_office.last_year.endowment;
	t=development_office.this_year.endowment;
	n=development_office.this_year.endowment;     // since no next_year data in engine, use this_year

	v41 = l;
	v42 = finance.calc_change(t,l);
	v43 = t;
	v44 = finance.calc_change(n,t);
	v45 = n;
    }

    //## fred 990507

  // ##### Begin Marco ##### //
    initialv21 = finance.initial_revenue_array[AC_ENDOWMENT_SPENDING].total;
    initialv31 = finance.initial_last_year.asset_array[AC_ENDOWMENT];
    //	initialv41 = development_office.initial_last_year.endowment;
    initialv41 = development_office.initial_last_year_endowment;
    // ####### End Marco ##### //

  ///////////////
  //## chea 021299 11.2.2
  //	v1=investment_office.equilibrium_payout_rate;
    v1 = (float)finance.revenue_policy_array[PL_ENDOWMENT_SPENDING_RATE].result_value;

    v2=investment_office.degree_smoothing_for_endowment_payout;
    int field_space=5*font_chartsm.max_font_width;

    if (refreshFlag == INFO_REPAINT) {
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+8);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+8,1);
	font_charts.put(REPORT_X1+310,REPORT_Y1-8,"The Endowment");

	if (mode_init_flag != MODE_FINANCIAL) {
	    //			spinnerVar1=0.0;
	    //			spinnerVar1=(float)finance.get_policy_var(true, AC_ENDOWMENT_SPENDING)->target_value;

	    mode_init_flag = MODE_FINANCIAL;

	    //--------- draw the brightened boxes ------------//
	    user_interface.brighten(REPORT_X1, REPORT_Y1+10, REPORT_X2, REPORT_Y2);
	    user_interface.rect(REPORT_X1, REPORT_Y1+10, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+2);
	    user_interface.rect(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-2, REPORT_X1+MODE_FINANCIAL_TAB4-82, REPORT_Y2);
	    user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB4-85, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-2, REPORT_X2, REPORT_Y2);

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

	    financial_spinner_budget.init(
		REPORT_X1+MODE_FINANCIAL_TAB5-40,
		REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+125,
		REPORT_X1+MODE_FINANCIAL_TAB5+80,
		REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+155,
		//### begin zhoubin 02/28/99
		&investment_office.spinner_var1,
		//### end zhoubin 02/28/99
		//				-100.0f,100.0f,0.1f,0,NULL,23);
		//				4.0f,10.0f,0.1f,0,NULL,25);  //## chea 170999
		-10.0f,10.0f,0.1f,0,NULL,25);             //## chea 031299 11.2.3
	    //-------- graph title --------------//
	    mode_financial_graph_profitloss.init(
		REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-2, REPORT_X1+MODE_FINANCIAL_TAB4-82, REPORT_Y2,
		3, &numSeriesData, dataArray, 1,
		NULL, NULL, mode_financial_legend_label, 1, 1, 1, 2);

	    //			mode_financial_text.put(tab3+280,vtab3+16,&v1,3);  //## chea 180899 12.2.1 ori.
	    //			mode_financial_text.put(tab3+280,vtab3+16,&v22,3);  //## chea 180899 change 12.2.1
	    //## chea 021199 change to the same as finance report
	    mode_financial_text.put(tab3+280,vtab3+16,&v1,24);

	    mode_financial_rbv.init(
		REPORT_X1+MODE_FINANCIAL_TAB4+10,
		REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT+23,
		REPORT_X2,
		REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT+73,
		3,
		&v2,
		mode_financial_rbvlabel,
		NULL
		);
	}
	else {
	    //--------- draw the brightened boxes ------------//
	    user_interface.brighten(REPORT_X1, REPORT_Y1+10, REPORT_X2, REPORT_Y2);
	    user_interface.rect(REPORT_X1, REPORT_Y1+10, REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+2);
	    user_interface.rect(REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-2, REPORT_X1+MODE_FINANCIAL_TAB4-82, REPORT_Y2);
	    user_interface.rect(REPORT_X1+MODE_FINANCIAL_TAB4-85, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT-2, REPORT_X2, REPORT_Y2);
	    //-------- paint the buttons -----------//
	    financial_spinner_budget.paint();
	    for(int i=0;i<3;i++)
		financial_button_group[i].paint();
	}

	mode_financial_graph_profitloss.paint();

	//-------- display static strings -----------//
	font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+40, mode_financial_str[0]);
	font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+60, mode_financial_str[1]);
	font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+80, mode_financial_str[2]);
	font_chartsm.put(
	    REPORT_X1+MODE_FINANCIAL_TAB3+25,REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT+5,
	    "Degree of smoothing for endowment spending");

	font_chartsm.put(
	    REPORT_X1+MODE_FINANCIAL_TAB3+25,vtab3+15,
	    "Current endowment spending rate");

	font_chartsm.put(
	    REPORT_X1+MODE_FINANCIAL_TAB3+25,vtab3+35,
	    "Change in endowment spending rate");

	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				REPORT_Y1+15,
				mode_financial_str[4]);

	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				REPORT_Y1+15,
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

    }
    else {                                          // refreshFlag == INFO_REFRESH
	//-------- refresh dynamic data --------------//
	// refresh() ==	1. rest()
	//						2. draw()
	//						3. blit buffer()

	mode_financial_graph_profitloss.refresh();
	vga.blt_buf(
	    REPORT_X1, REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT,
	    REPORT_X1+MODE_FINANCIAL_TAB4-90, REPORT_Y2
	    );

	mode_financial_text.refresh();
	mode_financial_rbv.refresh();

    }

    financial_spinner_budget.refresh();
    int pressed=finance.get_policy_var(true, AC_ENDOWMENT_SPENDING)->applied_flag;
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

  //--------- column: this year -----------//

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+40,
			    m.format(v21, 2));
    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
			    REPORT_Y1+60,
			    m.format(v31, 2));

    if((v41)>=0) {
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
				REPORT_Y1+80,
				m.format(v41, 2));
    }
    else {
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
				     REPORT_Y1+80,
				     m.format(v41, 2));
    }

    //--------- column: this year change -----------//

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    REPORT_Y1+40,
			    m.format(v22, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			    REPORT_Y1+60,
			    m.format(v32, 24));

    if((v42)>=0) {
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				REPORT_Y1+80,
				m.format(v42, 24));
    }
    else {
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
				     REPORT_Y1+80,
				     m.format(v42, 24));
    }

    //--------- column: this year -----------//

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+40,
			    m.format(v23, 2));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
			    REPORT_Y1+60,
			    m.format(v33, 2));

    if((v43)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
				REPORT_Y1+80,
				m.format(v43, 2));
    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
				     REPORT_Y1+80,
				     m.format(v43, 2));

    //--------- column: next year change % -----------//

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    REPORT_Y1+40,
			    m.format(v24, 24));

    font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			    REPORT_Y1+60,
			    m.format(v34, 24));

    if((v44)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				REPORT_Y1+80,
				m.format(v44, 24));
    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
				     REPORT_Y1+80,
				     m.format(v44, 24));

    //--------- column: next year -----------//

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

    if((v45)>=0)
	font_chartsm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				REPORT_Y1+80,
				//				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				//				REPORT_Y1+80+font_chartsm.max_font_height,
				m.format(v45, 2));
    else
	font_chart_red_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				     REPORT_Y1+80,
				     //				REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
				     //				REPORT_Y1+80+font_chart_red_sm.max_font_height,
				     m.format(v45, 2));

    // ##### Begin Marco ##### //
    if (info.financial_year() > 1) {
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2,
					REPORT_Y1+15,
					"Yr. 0 (Initial)");
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2,
					REPORT_Y1+40,
					m.format(initialv21, 2));
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2,
					REPORT_Y1+60,
					m.format(initialv31, 2));
	font_chart_purple_sm.right_put( REPORT_X1+MODE_FINANCIAL_TAB2,
					REPORT_Y1+80,
					m.format(initialv41, 2));
    }
    // ##### End Marco ##### //

    vga.blt_buf(REPORT_X1+MODE_FINANCIAL_TAB2, REPORT_Y1,
		REPORT_X2, REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT);

    for(int i=0;i<3;i++) {
	financial_button_group[i].pop();
	financial_button_group[i].body_ptr=
	    (void*)(financial_button_up_bitmap[i]+4);
    }
}

//---------- End of function report_FINANCIAL ---------//

//---------- Begin of function report_TRANSFER -------- //
static void report_transfer(int refreshFlag) {
    const static int tab1=REPORT_X1+10;
    const static int tab2=REPORT_X1+360;
    const static int tab3=REPORT_X1+365;
    const static int tab4=REPORT_X1+710;
    const static int vtab1=REPORT_Y1;
    const static int vtab2=REPORT_Y1+180;
    const static int vtab3=REPORT_Y1+185;
    const static int vtab4=REPORT_Y1+345;

    if (refreshFlag == INFO_REPAINT) {

	if ( mode_init_flag != MODE_TRANSFER ) {
	    mode_init_flag = MODE_TRANSFER;

	    user_interface.brighten(tab1,vtab1+15,tab4,vtab4);
	    // ##### Begin Marco ##### //
	    //marco
	    user_interface.rect(tab1,vtab1+15,tab4,vtab2);

	    // ##### End Marco ##### //
	    user_interface.rect(tab1,vtab2-2,tab4,vtab4);
	    user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	    user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	    font_charts.put(REPORT_X1+195,REPORT_Y1-7,"Transfer of funds from the operating reserve");

	    font_chartsm.put( 100, vtab1+20+25, "Transfer from:" );
	    font_chartsm.put( 380, vtab1+20+25, "Transfer to:" );

	    transfer_from.init(200,vtab1+20+25,320,218,3,&sel_from,transfer_label,NULL);
	    transfer_to.init(460,vtab1+20+25,580,218,3,&sel_to,transfer_label,NULL);

	    font_chartsm.put( 140, 280, "Amount to transfer( percentage of the current source account balance):" );

	    transfer_per_spinner.init(
		140,
		305,
		260,
		335,
		//### begin zhoubin 02/25/99
		&spinner_val,
		//### end zhoubin 02/25/99
		//				-100.0f,100.0f,0.1f,0,NULL,23);
		0.0f,95.0f,5.0f,0,NULL,25);               //## chea 170999

	    transfer_per_spinner.paint();

	    implement_button.create_text( 140,
					  350,
					  260,
					  380,
					  "Implement now" );

	    implement_button.paint();
	}
    }
    else {
	transfer_from.refresh();
	transfer_to.refresh();
    }

    transfer_per_spinner.refresh();
}

//---------- End of function report_TRANSFER -------- //

//-------- Begin of function detect_investment ---------//
//!
static int detect_investment() {
    if (mode_init_flag != MODE_INVESTMENT)
	return 0;
    double sliderVar1,sliderVar2,sliderVar3,total;

    vga.use_back();
    if(mode_investment_slider1.detect()) {
	sliderVar1=*(mode_investment_slider1.var_ptr);
	sliderVar2=*(mode_investment_slider2.var_ptr);
	sliderVar3=*(mode_investment_slider3.var_ptr);
	total=sliderVar2+sliderVar3;
	if(total>1E-9) {
	    *(mode_investment_slider2.var_ptr)=
		(int)((100.0-sliderVar1)*(sliderVar2/total));
	}
	else {
	    *(mode_investment_slider2.var_ptr)=
		(int)(50-sliderVar1/2);
	}
	*(mode_investment_slider3.var_ptr)=
	    100-*(mode_investment_slider1.var_ptr)-
	    *(mode_investment_slider2.var_ptr);
	investment_office.asset_array[0].allocation_percent=*(mode_investment_slider1.var_ptr);
	investment_office.asset_array[1].allocation_percent=*(mode_investment_slider2.var_ptr);
	investment_office.asset_array[2].allocation_percent=*(mode_investment_slider3.var_ptr);
	return 1;
    }
    if(mode_investment_slider2.detect()) {
	sliderVar1=*(mode_investment_slider1.var_ptr);
	sliderVar2=*(mode_investment_slider2.var_ptr);
	sliderVar3=*(mode_investment_slider3.var_ptr);
	total=sliderVar1+sliderVar3;
	if(total>1E-9) {
	    *(mode_investment_slider1.var_ptr)=
		(int)((100.0-sliderVar2)*(sliderVar1/total));
	}
	else {
	    *(mode_investment_slider1.var_ptr)=
		(int)(50-sliderVar2/2);
	}
	*(mode_investment_slider3.var_ptr)=
	    100-*(mode_investment_slider1.var_ptr)-
	    *(mode_investment_slider2.var_ptr);
	investment_office.asset_array[0].allocation_percent=*(mode_investment_slider1.var_ptr);
	investment_office.asset_array[1].allocation_percent=*(mode_investment_slider2.var_ptr);
	investment_office.asset_array[2].allocation_percent=*(mode_investment_slider3.var_ptr);
	return 1;
    }
    if(mode_investment_slider3.detect()) {
	sliderVar1=*(mode_investment_slider1.var_ptr);
	sliderVar2=*(mode_investment_slider2.var_ptr);
	sliderVar3=*(mode_investment_slider3.var_ptr);
	total=sliderVar1+sliderVar2;
	if(total>1E-9) {
	    *(mode_investment_slider1.var_ptr)=
		(int)((100.0-sliderVar3)*(sliderVar1/total));
	}
	else {
	    *(mode_investment_slider1.var_ptr)=
		(int)(50-sliderVar3/2);
	}
	*(mode_investment_slider2.var_ptr)=
	    100-*(mode_investment_slider1.var_ptr)-
	    *(mode_investment_slider3.var_ptr);
	investment_office.asset_array[0].allocation_percent=*(mode_investment_slider1.var_ptr);
	investment_office.asset_array[1].allocation_percent=*(mode_investment_slider2.var_ptr);
	investment_office.asset_array[2].allocation_percent=*(mode_investment_slider3.var_ptr);
	return 1;
    }

    return 0;
}

//---------- End of function detect_investment ---------//

//-------- Begin of function detect_financial ---------//
//!
static int detect_financial() {
    if (mode_init_flag != MODE_FINANCIAL)
	return 0;
    const static int vtab3=REPORT_Y1+170;

    vga.use_front();

    help.set_help(
	REPORT_X1+MODE_FINANCIAL_TAB3+25,REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT+5,
	REPORT_X1+MODE_FINANCIAL_TAB3+305,REPORT_Y2-MODE_FINANCIAL_BOX2_HEIGHT+85,"A.1.1.1.");
    help.set_help(
	REPORT_X1+MODE_FINANCIAL_TAB3+25,vtab3+35,
	REPORT_X1+MODE_FINANCIAL_TAB3+305,vtab3+55,"A.1.1.2.");

    if(mode_financial_rbv.detect()) {
	investment_office.degree_smoothing_for_endowment_payout=*(mode_financial_rbv.sel_num);
	return 1;
    }

    if (financial_spinner_budget.detect())
	return 1;

    for(int i=0;i<3;i++)
	if(financial_button_group[i].detect()) {
	    financial_button_group[i].body_ptr =
		(void *) (financial_button_down_bitmap[i] + 4);
	    //	Communicate with Sim Engine
	    /*PL_ENDOWMENT_SPENDING_RATE*/
	    finance.change_budget(i,true, AC_ENDOWMENT_SPENDING,(*financial_spinner_budget.var_ptr));
	    //##	begin zhoubin 000329
	    *financial_spinner_budget.var_ptr=0;
	    //##	end zhoubin 000329
	}

    return 0;
}

//---------- End of function detect_FINANCIAL ---------//

//-------- Begin of function detect_transfer ---------//
//!
static int detect_transfer() {
    if (transfer_from.detect()) {
	sel_from
	    =*(transfer_from.sel_num);
	return 1;
    }

    if (transfer_to.detect()) {
	sel_to
	    = *(transfer_to.sel_num);
	return 1;
    }

    if ( transfer_per_spinner.detect() )
	return 1;

    if ( implement_button.detect() ) {
	// if they are the same
	if ( transfer_to.sel_num == transfer_from.sel_num )
	    return 1;

	char selFrom[123];
	char selTo[123];

	switch( *transfer_from.sel_num ) {
	case 0: strcpy( selFrom, "Operating reserve" );   break;
	case 1: strcpy( selFrom, "Facilities reserve" );  break;
	case 2: strcpy( selFrom, "Quasi endowment" );   break;
	}

	switch( *transfer_to.sel_num ) {
	case 0: strcpy( selTo, "Operating reserve" );   break;
	case 1: strcpy( selTo, "Facilities reserve" );  break;
	case 2: strcpy( selTo, "Quasi endowment" );   break;
	}

	int selFromA, selToA;
	int transferMoney;

	switch (*transfer_from.sel_num) {
	case 0:   transferMoney = finance.this_year.asset_array[AC_OPERATING_RESERVE]*(*(transfer_per_spinner.var_ptr))/100;
	    selFromA      = finance.this_year.asset_array[AC_OPERATING_RESERVE]-transferMoney;
	    break;
	case 1:   transferMoney = finance.this_year.asset_array[AC_CAPITAL_RESERVE]*(*(transfer_per_spinner.var_ptr))/100;
	    selFromA      = finance.this_year.asset_array[AC_CAPITAL_RESERVE]-transferMoney;
	    break;
	case 2:   transferMoney = finance.this_year.asset_array[AC_QUASI]*(*(transfer_per_spinner.var_ptr))/100;
	    selFromA      = finance.this_year.asset_array[AC_QUASI]-transferMoney;
	    break;
	}

	switch (*transfer_to.sel_num) {
	case 0:   selToA  = finance.this_year.asset_array[AC_OPERATING_RESERVE] + transferMoney;
	    break;
	case 1:   selToA  = finance.this_year.asset_array[AC_CAPITAL_RESERVE] + transferMoney;
	    break;
	case 2:   selToA  = finance.this_year.asset_array[AC_QUASI] + transferMoney;
	    break;
	}

	char askStr[128];
	sprintf( askStr, "Do you want to transfer $%d? After the transfer, the %s will contain $%d and the %s will contain $%d.", transferMoney, selFrom, selFromA, selTo, selToA );

	if ( !box.ask( askStr ) )
	    return 1;

	switch (*transfer_from.sel_num) {
	case 0:   transferMoney = finance.this_year.asset_array[AC_OPERATING_RESERVE]*(*(transfer_per_spinner.var_ptr))/100;
	    finance.this_year.asset_array[AC_OPERATING_RESERVE] -= transferMoney;
	    finance.transfer_operating_reserve -= transferMoney;
	    break;
	case 1:   transferMoney = finance.this_year.asset_array[AC_CAPITAL_RESERVE]*(*(transfer_per_spinner.var_ptr))/100;
	    finance.this_year.asset_array[AC_CAPITAL_RESERVE] -= transferMoney;
	    finance.transfer_capital_reserve -= transferMoney;
	    break;
	case 2:   transferMoney = finance.this_year.asset_array[AC_QUASI]*(*(transfer_per_spinner.var_ptr))/100;
	    finance.this_year.asset_array[AC_QUASI] -= transferMoney;
	    finance.transfer_quasi -= transferMoney;
	    break;
	}

	switch (*transfer_to.sel_num) {
	case 0:   finance.this_year.asset_array[AC_OPERATING_RESERVE] += transferMoney;
	    finance.transfer_operating_reserve += transferMoney;
	    break;
	case 1:   finance.this_year.asset_array[AC_CAPITAL_RESERVE] += transferMoney;
	    finance.transfer_capital_reserve += transferMoney;
	    break;
	case 2:   finance.this_year.asset_array[AC_QUASI] += transferMoney;
	    finance.transfer_quasi += transferMoney;
	    break;
	}

	sys.redraw_all_flag = 1;
	*(transfer_per_spinner.var_ptr) = 0.0;

	return 1;
    }

    return 0;
}

//
//-------- End of function detect_transfer ---------//

//-------- Begin of function deinit_investment ---------//
//!
static void deinit_investment() {
    if (mode_init_flag == MODE_INVESTMENT) {
	/*		if (*financial_button_up_bitmap)
			{
			for (int i = 0; i < 3; i++)
			{
			mem_del(financial_button_up_bitmap[i]);
			mem_del(financial_button_down_bitmap[i]);
			financial_button_up_bitmap[i] = NULL;
			financial_button_down_bitmap[i] = NULL;
			}
			}

			financial_spinner_budget.deinit();
	*/
	mode_investment_piechart.deinit();
	mode_investment_slider1.deinit();
	mode_investment_slider2.deinit();
	mode_investment_slider3.deinit();
	mode_investment_graph_snp.deinit();
	mode_financial_text.deinit();
	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_investment ---------//

//-------- Begin of function deinit_financial ------//
//!
static void deinit_financial() {
    if (mode_init_flag == MODE_FINANCIAL) {
	// deallocate memory used by the radio buttons and the value sliders
	if(mode_financial_stats_bitmap) {
	    mem_del(mode_financial_stats_bitmap);
	    mode_financial_stats_bitmap=NULL;
	}

	// ##### begin Gilbert 28/04/2001 #####//
	financial_button_group.deinit_buttons();
	// ##### end Gilbert 28/04/2001 #####//
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
	mode_financial_rbv.deinit();
	// ##### begin Gilbert 28/04/2001 #######//
	mode_financial_text.deinit();
	// ##### end Gilbert 28/04/2001 #######//

	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_financial ------//

//-------- Begin of function deinit_tranfer ---------//
//!
static void deinit_transfer() {
    if ( mode_init_flag == MODE_TRANSFER ) {
	transfer_per_spinner.deinit();
	transfer_from.deinit();
	transfer_to.deinit();

	implement_button.deinit();

	// reset flag
	mode_init_flag = -1;
    }
}

//
//-------- End of function deinit_tranfer ---------//
