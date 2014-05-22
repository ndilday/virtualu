//Filename    : OADM_OFF.CPP
//Description : AdmOffice (Admissions and Financial Aid Office) class
//Owner		  : Kevin(ho)

// Notes:	Steps to introduce a new class of user interface
//				1.	construct the new class
//				2.	in sys.h, add the new MODE, e.g. MODE_ADMISSIONS_AND_FINANCIAL_AID_OFFICE
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
// Norris 0714

#include <OSYS.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OADM_OFF.H>
#include <OVGA.H>
#include <OBOX.H>
#include <OENROLL.H>
#include <OINFO.H>
#include <OIFACE.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OGRPHYR.H>
#include <OMOUSE.H>
#include <OBUTTON.H>
#include <OGRPNFYR.H>
#include <ORADIO.H>
#include <OVALSLDR.H>
#include <OTEXT.H>
#include <OHELP.H>
#include <OPSCHOOL.H>
#include <OCHANCE.H>
#include <ONEWS.H>                                // 990505
#include <OGRPNFT.H>
#include <ODEPT.H>

//------- Define constants -------//

enum {                                            // the 'sub' view-modes
    MODE_ADMISSION = 0,
    MODE_STATISTICS = 1,
    MODE_PRIORITY = 2,
    MODE_FINANCIAL_AID = 3
};

enum {                                            // the bg image id
    BG_PIC_ID = 4
};

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-55
};

enum {
    BOTTOM_BUTTON_X1 = REPORT_X1+80,
    BOTTOM_BUTTON_Y1 = REPORT_Y2+15,
    BOTTOM_BUTTON_X2 = REPORT_X2-80,
    BOTTOM_BUTTON_Y2 = REPORT_Y2+40
};

enum {
    BOTTOM_BUTTON_WIDTH = 125,
    BOTTOM_BUTTON_HEIGHT = 25,
    BOTTOM_BUTTON_DISTANCE
    = (BOTTOM_BUTTON_X2-BOTTOM_BUTTON_X1-BOTTOM_BUTTON_WIDTH*4)/3
};

// begin kevin 0718
enum {
    MODE_ADMISSION_BOX_HEIGHT = 265,
    MODE_ADMISSION_BOX_WIDTH1 = 160,
    MODE_ADMISSION_BOX_WIDTH2 = 400,
    MODE_ADMISSION_BOX_WIDTH3 = 75,
    MODE_ADMISSION_LEGEND_HEIGHT = 26,
    MODE_ADMISSION_LEGEND_WIDTH = 400,

    MODE_ADMISSION_BARCHART_X1 = (REPORT_X1)+10,
    MODE_ADMISSION_BARCHART_Y1 = (REPORT_Y1)+55,
    MODE_ADMISSION_BARCHART_HEIGHT = 210,
    MODE_ADMISSION_BARCHART_WIDTH = 400,
    MODE_ADMISSION_BARCHART_ROW1 = 42,
    MODE_ADMISSION_BARCHART_ROW2 = 107,
    MODE_ADMISSION_BARCHART_ROW3 = 172,
    MODE_ADMISSION_BARCHART_ROW4 = 235,
    MODE_ADMISSION_BARCHART_ROW5 = 270,
    MODE_ADMISSION_SLIDER_WIDTH = 340,
};
// end kevin 0718

enum {
    MODE_STATISTICS_GRAPH_WIDTH = 220,
    MODE_STATISTICS_GRAPH_HEIGHT = 115,
    MODE_STATISTICS_LEGEND_HEIGHT = 26,
    MODE_STATISTICS_LEGEND_WIDTH = 400,
    MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE
    = (REPORT_X2-REPORT_X1-MODE_STATISTICS_GRAPH_WIDTH*3)/2,
    MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE = 20,
    MODE_STATISTICS_SUB_BUTTON_X1 = 130,
    MODE_STATISTICS_SUB_BUTTON_Y1 = REPORT_Y2-48,
    SUB_BOTTOM_BUTTON_WIDTH = 200,
    SUB_BOTTOM_BUTTON_HEIGHT = 23,
    SUB_BOTTOM_BUTTON_DISTANCE = 13
};

enum {
    MODE_PRIORITY_BOX_HEIGHT = 165,
    MODE_PRIORITY_BOX_WIDTH = 650,
    MODE_PRIORITY_BOX_TAB1 = 40,
    MODE_PRIORITY_BOX_TAB2 = 240,
    MODE_PRIORITY_BOX_TAB3 = 360,
    MODE_PRIORITY_VALUE_SLIDER_HEIGHT = 18,
    MODE_PRIORITY_VALUE_SLIDER_WIDTH = 400,
    MODE_PRIORITY_RADIO_BUTTON_HEIGHT = 18,
    MODE_PRIORITY_RADIO_BUTTON_WIDTH = 350
};

enum {
    MODE_FINANCIAL_AID_BOX_HEIGHT_1 = 100,
    MODE_FINANCIAL_AID_BOX_HEIGHT_2 = 80,
    MODE_FINANCIAL_AID_BOX_HEIGHT_3 = 120,
    MODE_FINANCIAL_AID_BOX_DISTANCE =0,
    //				= (REPORT_Y2-REPORT_Y1-MODE_FINANCIAL_AID_BOX_HEIGHT_1-MODE_FINANCIAL_AID_BOX_HEIGHT_2-MODE_FINANCIAL_AID_BOX_HEIGHT_3-15)/2,
    MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT = 18,
    MODE_FINANCIAL_AID_VALUE_SLIDER_WIDTH = 100,
    MODE_FINANCIAL_AID_RADIO_BUTTON_HEIGHT = 18,
    MODE_FINANCIAL_AID_RADIO_BUTTON_WIDTH = 80
};

//-------- define static vars -----------//

static char report_init_flag = 0;
static char mode_init_flag = -1;                  // -1: non of the mode was init'ed

static ButtonGroup bottom_button_group(4);
static short *bottom_button_up_bitmap[4];
static short *bottom_button_down_bitmap[4];
static char *bottom_button_label[] = {
    "Overview",
    "Trends",
    "Priorities",
    "Financial aid"
};

static ButtonGroup sub_bottom_button_group(5);
static short *sub_bottom_button_up_bitmap[5];
static short *sub_bottom_button_down_bitmap[5];
static char *sub_bottom_button_label[] = {
    "Undergraduate : traditional",
    "Undergraduate : nontraditional",
    "Master's",
    "Doctoral",
    "Distance learning",
};

// Notes:	the strings could be moved to Ogamestr.cpp
// Norris 0714

static GraphNonFinancialYear mode_statistics_graph_admit_rate;
static GraphNonFinancialYear mode_statistics_graph_yield_rate;
static GraphNonFinancialYear mode_statistics_graph_percent_minorities;
static GraphNonFinancialYear mode_statistics_graph_avg_academic_rating;
static GraphNonFinancialYear mode_statistics_graph_avg_extracurricular_activities_rating;
static GraphNonFinancialYear mode_statistics_graph_avg_athletics_rating;
static GraphNonFinancialTrimester distance_credits_graph;
static GraphNonFinancialYear *mode_statistics_graph[6] = {
    &mode_statistics_graph_admit_rate,
    &mode_statistics_graph_yield_rate,
    &mode_statistics_graph_avg_academic_rating,
    &mode_statistics_graph_percent_minorities,
    &mode_statistics_graph_avg_extracurricular_activities_rating,
    &mode_statistics_graph_avg_athletics_rating
};
static char *mode_statistics_graph_title[] = {
    "Distance credits demanded", "Distance credits provided",
    "Average academic rating", "Percent minorities",
    "Average extracurricular activities rating", "Average athletics rating"
};

static char *mode_statistics_legend_label[] = {
    "Player university", "Average of peer universities"
};

static RadioButton mode_financial_aid_radio_button_nontraditional;
static RadioButton mode_financial_aid_radio_button_minorities;
static RadioButton mode_financial_aid_radio_button_athletes;
static ValueSlider mode_financial_aid_value_slider_nontraditional;
static ValueSlider mode_financial_aid_value_slider_need;
static ValueSlider mode_financial_aid_value_slider_merit;
static char *mode_financial_aid_str[] = {
    "Percentage of traditional undergraduates' financial need the University agrees to pay",
    "Traditional undergraduates to be offered merit aid",
    //	"Will nontraditional undergraduates be eligible for financial aid?",
    "Nontraditional students to be offered financial aid",
    //	"Do you give special financial aid treatment to minorities for all students?",
    //## chea 121099
    "Do you give special financial aid treatment to minorities for all student levels?",
    "Do you give special financial aid treatment to athletes for all traditional undergraduates?"
};
static char *mode_financial_aid_radio_button_option_str1[] = {
    "Yes", "No"
};
static char *mode_financial_aid_radio_button_option_str2[] = {
    "None", "Medium", "High",
};

// begin kevin 0717
static char *mode_admission_label1[] = {
    "Admit","Yield","rate"
};

static char *mode_admission_label2[] = {
    "Traditional undergraduate","Nontraditional undergraduate",
    "Master's","Doctoral","Distance learning"
};

static char *mode_admission_label3[] = {
    "Matriculants","Admits (not matriculating)","Applicants (not admitted)"
};

static int mode_admission_bar_color[3]={V_RED,V_GREEN,V_BLUE};

static short *mode_admission_no_scaling_bitmap;
static short *mode_admission_no_ar_bitmap[5];
static short *mode_admission_no_yr_bitmap[5];
static short *mode_admission_no_bar_bitmap[5];

static ValueSlider mode_admission_target[5];

// end kevin 0717

// begin kevin 0723
static RadioButton mode_priority_radio_button_minorities;
static RadioButton mode_priority_radio_button_athletes;
static RadioButton mode_priority_radio_button_distance_learners;
static ValueSlider mode_priority_value_slider_academic_rating;
static ValueSlider mode_priority_value_slider_extracur_rating;
static ValueSlider mode_priority_value_slider_athletic_rating;
//static ValueSlider mode_priority_value_slider_distance_learning;

static char *mode_priority_str[] = {
    "Traditional undergraduates",
    "Academic performance",
    "Extracurricular performance",
    "Athletic performance",
    "Distance learning",
    //	"Do you give special admission to underrepresented minorities for all students?",
    //## chea 121099
    "Do you give special admission to underrepresented minorities for all student levels?",
    "Do you give special admissions treatment to athletes for all traditional undergraduates?",
    "Rating priority for traditional undergraduates",
    "Acceptance of outside distance learners in courses",
};

static char *mode_priority_radio_button_option_str[] = {
    "None", "Medium", "High",
};

static char *mode_priority_radio_button_option_str1[] = {
    "Prohibit", "Accept", "Actively seek",
};

// end kevin 0723

//-------- define static functions ------//

// Notes:	the deinit_x functions should deallocate memory used by the
//				GraphA, ValueSlider, RadioButton, etc. objects
//				the functions are called when a mode-switching is triggered
//	Norris 0716

static void report_admission(int);
static void report_statistics(int);
static void report_priority(int);
static void report_financial_aid(int);
static int detect_admission();
static int detect_statistics();
static int detect_priority();
static int detect_financial_aid();
static void deinit_admission();
static void deinit_statistics();
static void deinit_priority();
static void deinit_financial_aid();

typedef void (*ReportFP)(int refreshFlag);
typedef int (*DetectFP)();
typedef void (*DeinitFP)();

static ReportFP report_func_ptr[] = {
    &report_admission, &report_statistics, &report_priority, &report_financial_aid
};
static DetectFP detect_func_ptr[] = {
    &detect_admission, &detect_statistics, &detect_priority, &detect_financial_aid
};
static DeinitFP deinit_func_ptr[] = {
    &deinit_admission, &deinit_statistics, &deinit_priority, &deinit_financial_aid
};

// 990505
extern EventStruct news_event_function_array[NEWS_ID_COUNT_ONLY_CHANCE];

//------- Begin of function AdmOffice::AdmOffice -----------//
//!
AdmOffice::AdmOffice() {
    init();
}

//--------- End of function AdmOffice::AdmOffice -----------//

//------- Begin of function AdmOffice::~AdmOffice -----------//
//!
AdmOffice::~AdmOffice() {
    deinit();
}

//--------- End of function AdmOffice::~AdmOffice -----------//

//------- Begin of function AdmOffice::deinit ---------------//
//!
void AdmOffice::deinit() {
    //--------- deallocate memory -------------//

    // ##### begin Gilbert 28/04/2001 #####//
    bottom_button_group.deinit_buttons();
    // ##### end Gilbert 28/04/2001 #####//
    if (*bottom_button_up_bitmap) {
	for (int i = 0; i < 4; i++) {
	    mem_del(bottom_button_up_bitmap[i]);
	    mem_del(bottom_button_down_bitmap[i]);
	    bottom_button_up_bitmap[i] = NULL;
	    bottom_button_down_bitmap[i] = NULL;
	}
    }

    (*deinit_func_ptr[current_mode])();

    report_init_flag = 0;
}

//--------- End of function AdmOffice::deinit ---------------//

//------- Begin of function AdmOffice::init -----------------//
//!
void AdmOffice::init() {
    for (int i = 0; i < 4; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }
    current_mode = MODE_ADMISSION;
    sub_current_mode=0;
}

//--------- End of function AdmOffice::init -----------------//

//------- Begin of function AdmOffice::report ---------------//
//!
void AdmOffice::report(int refreshFlag) {
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
	    for (int i = 0; i < 4; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
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
    // Norris 0714

    if (refreshFlag == INFO_REPAINT) {
	//------- blt the back buffer to front buffer -------//
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    }

    //##trevor 2807
    vga.use_front();
    //##trevor 2807
}

//---------- End of function AdmOffice::report ------------//

//-------- Begin of function AdmOffice::detect ------------//
//!
int AdmOffice::detect() {
    vga.use_back();

    int rc = detect2();

    vga.use_front();

    return rc;
}

//---------- End of function AdmOffice::detect ------------//

//-------- Begin of function AdmOffice::detect2 ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int AdmOffice::detect2() {
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

//---------- End of function AdmOffice::detect2 ------------//

//-------- Begin of function report_admission -------------//
//!
static int sliderVar1;
static int sliderVar2;
static int sliderVar3;
static int sliderVar4;
static int sliderVar5;

//## chea 060999 adding target bar
int last_year_tar = 0;
short target_pos = 0;

static void report_admission(int refreshFlag) {
    // begin Kevin 0716

    //temp * start
    static int ar_tug=10,yr_tug=24;
    static int ar_ug=20,yr_ug=42;
    static int ar_master=15,yr_master=23;
    static int ar_doc=12,yr_doc=23;
    static int ar_dist=12,yr_dist=23;
    static int bar[5][3];

    // ##### Begin Marco ##### //
    static int initial_ar_tug=10,initial_yr_tug=24;
    static int initial_ar_ug=20,initial_yr_ug=42;
    static int initial_ar_master=15,initial_yr_master=23;
    static int initial_ar_doc=12,initial_yr_doc=23;
    static int initial_ar_dist=12,initial_yr_dist=23;
    // ##### End Marco ###### //

    // top title
    user_interface.brighten(REPORT_X1+150,REPORT_Y1-18,REPORT_X1+575,REPORT_Y1+5);
    user_interface.rect(REPORT_X1+150,REPORT_Y1-18,REPORT_X1+575,REPORT_Y1+5,1);
    font_charts.put(REPORT_X1+220,REPORT_Y1-12,"Student Admissions Statistics and Targets");

    //	Communicate with Sim Engine's Output
    ar_tug=(int)(100*enroll_res.offers_rate[0]);
    yr_tug=(int)(100*enroll_res.cur_yield_rate[0]);
    ar_ug=(int)(100*enroll_res.offers_rate[1]);
    yr_ug=(int)(100*enroll_res.cur_yield_rate[1]);
    ar_master=(int)(100*enroll_res.offers_rate[2]);
    yr_master=(int)(100*enroll_res.cur_yield_rate[2]);
    ar_doc=(int)(100*enroll_res.offers_rate[3]);
    yr_doc=(int)(100*enroll_res.cur_yield_rate[3]);
    ar_dist=(int)(100*enroll_res.offers_rate[4]);
    yr_dist=(int)(100*enroll_res.cur_yield_rate[4]);

    // ##### Begin Marco ##### //
    initial_ar_tug=(int)(100*enroll_res.initial_offers_rate[0]);
    initial_yr_tug=(int)(100*enroll_res.initial_cur_yield_rate[0]);
    initial_ar_ug=(int)(100*enroll_res.initial_offers_rate[1]);
    initial_yr_ug=(int)(100*enroll_res.initial_cur_yield_rate[1]);
    initial_ar_master=(int)(100*enroll_res.initial_offers_rate[2]);
    initial_yr_master=(int)(100*enroll_res.initial_cur_yield_rate[2]);
    initial_ar_doc=(int)(100*enroll_res.initial_offers_rate[3]);
    initial_yr_doc=(int)(100*enroll_res.initial_cur_yield_rate[3]);
    initial_ar_dist=(int)(100*enroll_res.initial_offers_rate[4]);
    initial_yr_dist=(int)(100*enroll_res.initial_cur_yield_rate[4]);
    // ##### End Marco ##### //

    bar[0][0]=enroll_res.total_matrics[UG_TRADITION];
    bar[1][0]=enroll_res.total_matrics[UG_NONTRADITION];
    bar[2][0]=enroll_res.total_matrics[MASTER];
    bar[3][0]=enroll_res.total_matrics[DOCTOR];
    bar[4][0]=enroll_res.total_matrics[DISTANCE_LEARN];

    bar[0][1]=enroll_res.total_offers[UG_TRADITION]-bar[0][0];
    bar[1][1]=enroll_res.total_offers[UG_NONTRADITION]-bar[1][0];
    bar[2][1]=enroll_res.total_offers[MASTER]-bar[2][0];
    bar[3][1]=enroll_res.total_offers[DOCTOR]-bar[3][0];
    bar[4][1]=enroll_res.total_offers[DISTANCE_LEARN]-bar[4][0];

    bar[0][2]=enroll_res.total_applications[UG_TRADITION]-bar[0][1]-bar[0][0];
    bar[1][2]=enroll_res.total_applications[UG_NONTRADITION]-bar[1][1]-bar[1][0];
    bar[2][2]=enroll_res.total_applications[MASTER]-bar[2][1]-bar[2][0];
    bar[3][2]=enroll_res.total_applications[DOCTOR]-bar[3][1]-bar[3][0];
    bar[4][2]=enroll_res.total_applications[DISTANCE_LEARN]-bar[4][1]-bar[4][0];

    //## chea begin 020999 make the target_student_intake the same 4 every year
    sliderVar1=enroll_res.target_enrollment[0];
    sliderVar2=enroll_res.target_enrollment[1];
    sliderVar3=enroll_res.target_enrollment[2];

    /*
      finance.tradU_target_student_intake = enroll_res.target_student_intake[0];
      finance.nontradU_target_student_intake = enroll_res.target_student_intake[1];
      finance.master_target_student_intake = =enroll_res.target_student_intake[2];
      finance.dl_target_student_intake = enroll_res.target_student_intake[4];
    */
    //## chea begin 020999 make the target_student_intake the same 4 every year

    if (refreshFlag == INFO_REPAINT) {
	//--------- draw the brightened boxes ------------//
	user_interface.brighten(REPORT_X1, REPORT_Y1+30,
				REPORT_X2,
				REPORT_Y1+81+MODE_ADMISSION_BOX_HEIGHT);
	user_interface.rect(REPORT_X1, REPORT_Y1+25,
			    REPORT_X2,
			    REPORT_Y1+55+MODE_ADMISSION_BOX_HEIGHT);
	user_interface.rect(REPORT_X1+MODE_ADMISSION_BOX_WIDTH1,
			    REPORT_Y1+25,
			    REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
			    REPORT_Y1+55+MODE_ADMISSION_BOX_HEIGHT);
	user_interface.rect(REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2-3,
			    REPORT_Y1+25,
			    REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2+MODE_ADMISSION_BOX_WIDTH3,
			    REPORT_Y1+55+MODE_ADMISSION_BOX_HEIGHT);
	user_interface.rect(
	    REPORT_X1,
	    REPORT_Y1+25,
	    REPORT_X2,
	    REPORT_Y1+81+MODE_ADMISSION_BOX_HEIGHT);

	if (mode_init_flag != MODE_ADMISSION) {
	    //--------- initialization of   -----------//
	    mode_init_flag = MODE_ADMISSION;

	    if(mode_admission_no_scaling_bitmap) {
		mem_del(mode_admission_no_scaling_bitmap);
		mode_admission_no_scaling_bitmap=NULL;
	    }

	    for (int i = 0; i < 5; i++) {
		if(mode_admission_no_ar_bitmap[i]) {
		    mem_del(mode_admission_no_ar_bitmap[i]);
		    mode_admission_no_ar_bitmap[i]=NULL;
		}

		if(mode_admission_no_yr_bitmap[i]) {
		    mem_del(mode_admission_no_yr_bitmap[i]);
		    mode_admission_no_yr_bitmap[i]=NULL;
		}

		if(mode_admission_no_bar_bitmap[i]) {
		    mem_del(mode_admission_no_bar_bitmap[i]);
		    mode_admission_no_bar_bitmap[i]=NULL;
		}
	    }

	    mode_admission_no_ar_bitmap[0]
		=vga_back.save_area((REPORT_X2)-150,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
				    (REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chartsm.max_font_height,
				    mode_admission_no_ar_bitmap[0]);
	    mode_admission_no_ar_bitmap[1]
		=vga_back.save_area((REPORT_X2)-150,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
				    (REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chartsm.max_font_height,
				    mode_admission_no_ar_bitmap[1]);
	    mode_admission_no_ar_bitmap[2]
		=vga_back.save_area((REPORT_X2)-150,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
				    (REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chartsm.max_font_height,
				    mode_admission_no_ar_bitmap[2]);
	    mode_admission_no_ar_bitmap[3]
		=vga_back.save_area((REPORT_X2)-150,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
				    (REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chartsm.max_font_height,
				    mode_admission_no_ar_bitmap[3]);
	    mode_admission_no_ar_bitmap[4]
		=vga_back.save_area((REPORT_X2)-150,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5,
				    (REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chartsm.max_font_height,
				    mode_admission_no_ar_bitmap[4]);

	    mode_admission_no_yr_bitmap[0]
		=vga_back.save_area((REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
				    (REPORT_X2),
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chartsm.max_font_height,
				    mode_admission_no_yr_bitmap[0]);
	    mode_admission_no_yr_bitmap[1]
		=vga_back.save_area((REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
				    (REPORT_X2),
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chartsm.max_font_height,
				    mode_admission_no_yr_bitmap[1]);
	    mode_admission_no_yr_bitmap[2]
		=vga_back.save_area((REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
				    (REPORT_X2),
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chartsm.max_font_height,
				    mode_admission_no_yr_bitmap[2]);
	    mode_admission_no_yr_bitmap[3]
		=vga_back.save_area((REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
				    (REPORT_X2),
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chartsm.max_font_height,
				    mode_admission_no_yr_bitmap[3]);
	    mode_admission_no_yr_bitmap[4]
		=vga_back.save_area((REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5,
				    (REPORT_X2),
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chartsm.max_font_height,
				    mode_admission_no_yr_bitmap[4]);

	    mode_admission_no_bar_bitmap[0]
		=vga_back.save_area(
		    (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
		    (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+14,
		    mode_admission_no_bar_bitmap[0]);
	    mode_admission_no_bar_bitmap[1]
		=vga_back.save_area(
		    (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
		    (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+14,
		    mode_admission_no_bar_bitmap[1]);
	    mode_admission_no_bar_bitmap[2]
		=vga_back.save_area(
		    (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
		    (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+14,
		    mode_admission_no_bar_bitmap[2]);
	    mode_admission_no_bar_bitmap[3]
		=vga_back.save_area(
		    (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
		    (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+14,
		    mode_admission_no_bar_bitmap[3]);
	    mode_admission_no_bar_bitmap[4]
		=vga_back.save_area(
		    (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5,
		    (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+14,
		    mode_admission_no_bar_bitmap[4]);

	    mode_admission_target[0].init(
		REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+20,
		REPORT_Y1+MODE_ADMISSION_BARCHART_ROW1+19,
		MODE_ADMISSION_SLIDER_WIDTH,
		MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT-1,
		0.8*enroll_res.new_year_student_count[0], 1.2*enroll_res.new_year_student_count[0], &sliderVar1, 1, V_BROWN, 1);

	    mode_admission_target[1].init(
		REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+20,
		REPORT_Y1+MODE_ADMISSION_BARCHART_ROW2+19,
		MODE_ADMISSION_SLIDER_WIDTH,
		MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT-1,
		0.8*enroll_res.new_year_student_count[1], 1.2*enroll_res.new_year_student_count[1], &sliderVar2, 1, V_BROWN, 1);

	    mode_admission_target[2].init(
		REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+20,
		REPORT_Y1+MODE_ADMISSION_BARCHART_ROW3+19,
		MODE_ADMISSION_SLIDER_WIDTH,
		MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT-1,
		0.8*enroll_res.new_year_student_count[2], 1.2*enroll_res.new_year_student_count[2], &sliderVar3, 1, V_BROWN, 1);

	    mode_admission_target[0].refresh();
	    mode_admission_target[1].refresh();
	    mode_admission_target[2].refresh();

	}
	else {
	    //-------- repaint -----------//
	}

	// -------- display the strings ----------- //
	font_chartsm.put(
	    (REPORT_X2)-425,
	    (REPORT_Y1)+11,
	    "Number of students");
	font_chartsm.put(
	    (REPORT_X2)-125,
	    (REPORT_Y1)-1,
	    mode_admission_label1[0]);
	font_chartsm.put(
	    (REPORT_X2)-50,
	    (REPORT_Y1)-1,
	    mode_admission_label1[1]);
	font_chartsm.put(
	    (REPORT_X2)-120,
	    (REPORT_Y1)+11,
	    mode_admission_label1[2]);
	font_chartsm.put(
	    (REPORT_X2)-50,
	    (REPORT_Y1)+11,
	    mode_admission_label1[2]);

	font_chartsm.put_paragraph(
	    (REPORT_X1)+80,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1-10,
	    (REPORT_X1)+180,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+30,
	    mode_admission_label2[0],-1);

	font_chartsm.put_paragraph(
	    (REPORT_X1)+80,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2-15,
	    (REPORT_X1)+180,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+25,
	    mode_admission_label2[1],-1);
	font_chartsm.put(
	    (REPORT_X1)+110,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
	    mode_admission_label2[2]);
	font_chartsm.put(
	    (REPORT_X1)+107,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
	    mode_admission_label2[3]);

	font_chart_red_sm.put(
	    (REPORT_X1)+60,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+20,
	    "Target enrollment");
	font_chart_red_sm.put(
	    (REPORT_X1)+60,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+20,
	    "Target enrollment");
	font_chart_red_sm.put(
	    (REPORT_X1)+60,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+20,
	    "Target enrollment");

	font_chartsm.put(
	    145,
	    REPORT_Y1+78+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	    mode_admission_label3[0]);
	font_chartsm.put(
	    43+(REPORT_X2-REPORT_X1)/3+4,
	    REPORT_Y1+78+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	    mode_admission_label3[1]);
	font_chartsm.put(
	    43+2*(REPORT_X2-REPORT_X1)/3+4,
	    REPORT_Y1+78+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	    mode_admission_label3[2]);

	user_interface.bar(
	    127,
	    REPORT_Y1+77+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	    139,
	    REPORT_Y1+89+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	    mode_admission_bar_color[0]);

	user_interface.bar(
	    25+(REPORT_X2-REPORT_X1)/3+4,
	    REPORT_Y1+77+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	    25+(REPORT_X2-REPORT_X1)/3+16,
	    REPORT_Y1+89+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	    mode_admission_bar_color[1]);

	user_interface.bar(
	    25+2*(REPORT_X2-REPORT_X1)/3+4,
	    REPORT_Y1+77+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	    25+2*(REPORT_X2-REPORT_X1)/3+16,
	    REPORT_Y1+89+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	    mode_admission_bar_color[2]);
    }
    else {                                          // refreshFlag == INFO_REFRESH
	//		sliderVar1=enroll_res.total_matrics[UG_TRADITION];
	//		sliderVar2=enroll_res.total_matrics[UG_NONTRADITION];
	//		sliderVar3=enroll_res.total_matrics[MASTER];
	//		sliderVar5=enroll_res.total_matrics[DISTANCE_LEARN];

	if(sliderVar1>mode_admission_target[0].max_val)
	    sliderVar1=mode_admission_target[0].max_val;
	if(sliderVar2>mode_admission_target[1].max_val)
	    sliderVar2=mode_admission_target[1].max_val;
	if(sliderVar3>mode_admission_target[2].max_val)
	    sliderVar3=mode_admission_target[2].max_val;

	int row_tab=0;
	for(int i=0;i<4;i++)
	    {
		if(i==0)
		    row_tab = MODE_ADMISSION_BARCHART_ROW1;
		else if (i==1)
		    row_tab = MODE_ADMISSION_BARCHART_ROW2;
		else if (i==2)
		    row_tab = MODE_ADMISSION_BARCHART_ROW3;

		if(i!=3) {

		    //## chea 040999 set a ribbon to last year target
		    int update_last_year_tar = enroll_res.last_year_target_student_intake[i];
		    if(update_last_year_tar >= mode_admission_target[i].max_val)
			update_last_year_tar = mode_admission_target[i].max_val;
		    if(update_last_year_tar <= mode_admission_target[i].min_val)
			update_last_year_tar = 0;

		    int update_target_pos  = (short)((((float)mode_admission_target[i].value_slider_x2 - (float)mode_admission_target[i].value_slider_x1) / (float)mode_admission_target[i].max_val)*update_last_year_tar);

		    mode_admission_target[i].set_target(
			mode_admission_target[i].value_slider_x1 + update_target_pos -1,
			REPORT_Y1+row_tab+16,
			MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT +8,
			1, V_RED,V_RED);

		    /*
		      //## 080999 set target to this year

		      int update_year_tar = enroll_res.target_student_intake[i];
		      int update_target_pos  = (short)((((float)mode_admission_target[i].value_slider_x2 - (float)mode_admission_target[i].value_slider_x1) / (float)mode_admission_target[i].max_val)*update_year_tar);
		      mode_admission_target[i].init_target(
		      mode_admission_target[i].value_slider_x1 + update_target_pos -1,
		      REPORT_Y1+row_tab+16,
		      MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT+8,
		      1, V_RED,V_RED);
		    */

		    vga_back.rest_area(mode_admission_no_ar_bitmap[i], 0, 0);
		    vga_back.rest_area(mode_admission_no_yr_bitmap[i], 0, 0);
		    vga_back.rest_area(mode_admission_no_bar_bitmap[i], 0, 0);

		    mode_admission_target[i].refresh();

		}
	    }
    }

    // ---- Draw the statistics ---- //
    font_chartsm.center_put((REPORT_X2)-150,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
			    (REPORT_X2)-75,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chartsm.max_font_height,
			    m.format(ar_tug, 3));
    font_chartsm.center_put((REPORT_X2)-75,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
			    (REPORT_X2),
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chartsm.max_font_height,
			    m.format(yr_tug, 3));

    font_chartsm.center_put((REPORT_X2)-150,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
			    (REPORT_X2)-75,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chartsm.max_font_height,
			    m.format(ar_ug, 3));
    font_chartsm.center_put((REPORT_X2)-75,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
			    (REPORT_X2),
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chartsm.max_font_height,
			    m.format(yr_ug, 3));

    font_chartsm.center_put((REPORT_X2)-150,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
			    (REPORT_X2)-75,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chartsm.max_font_height,
			    m.format(ar_master, 3));
    font_chartsm.center_put((REPORT_X2)-75,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
			    (REPORT_X2),
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chartsm.max_font_height,
			    m.format(yr_master, 3));

    font_chartsm.center_put((REPORT_X2)-150,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
			    (REPORT_X2)-75,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chartsm.max_font_height,
			    m.format(ar_doc, 3));
    font_chartsm.center_put((REPORT_X2)-75,
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
			    (REPORT_X2),
			    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chartsm.max_font_height,
			    m.format(yr_doc, 3));

    // ##### Begin Marco ##### //
    font_chart_purple_sm.center_put((REPORT_X2)-150,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chart_purple_sm.max_font_height+3,
				    (REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chart_purple_sm.max_font_height*2,
				    m.format(initial_ar_tug, 3));
    font_chart_purple_sm.center_put((REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chart_purple_sm.max_font_height+3,
				    (REPORT_X2),
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chart_purple_sm.max_font_height*2,
				    m.format(initial_yr_tug, 3));

    font_chart_purple_sm.center_put((REPORT_X2)-150,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chart_purple_sm.max_font_height+3,
				    (REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chart_purple_sm.max_font_height*2,
				    m.format(initial_ar_ug, 3));
    font_chart_purple_sm.center_put((REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chart_purple_sm.max_font_height+3,
				    (REPORT_X2),
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chart_purple_sm.max_font_height*2,
				    m.format(initial_yr_ug, 3));

    font_chart_purple_sm.center_put((REPORT_X2)-150,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chart_purple_sm.max_font_height+3,
				    (REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chart_purple_sm.max_font_height*2,
				    m.format(initial_ar_master, 3));
    font_chart_purple_sm.center_put((REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chart_purple_sm.max_font_height+3,
				    (REPORT_X2),
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chart_purple_sm.max_font_height*2,
				    m.format(initial_yr_master, 3));

    font_chart_purple_sm.center_put((REPORT_X2)-150,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chart_purple_sm.max_font_height+3,
				    (REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chart_purple_sm.max_font_height*2,
				    m.format(initial_ar_doc, 3));
    font_chart_purple_sm.center_put((REPORT_X2)-75,
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chart_purple_sm.max_font_height+3,
				    (REPORT_X2),
				    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chart_purple_sm.max_font_height*2,
				    m.format(initial_yr_doc, 3));

    /*		Kenneth --- Remove initial value
		font_chart_purple_sm.center_put((REPORT_X2)-150,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chart_purple_sm.max_font_height+3,
		(REPORT_X2)-75,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chart_purple_sm.max_font_height*2,
		m.format(initial_ar_dist, 3));
		font_chart_purple_sm.center_put((REPORT_X2)-75,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chart_purple_sm.max_font_height+3,
		(REPORT_X2),
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chart_purple_sm.max_font_height*2,
		m.format(initial_yr_dist, 3));
    */
    // ##### End Marco ##### //

    // ---- Draw bars ---- //
    int series_x1 = (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+10;
    int series_x2 = (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2-10;
    int total,lastpos,newpos,barlen;
    int i;

    // BAR 1 //
    for(i=0,total=0;i<3;i++) {
	total+=bar[0][i];
    }

    //##chwg0525
    if(total==0)total=1;

    for(i=0,lastpos=series_x1;i<3;i++) {
	barlen=bar[0][i]*(series_x2-series_x1)/total;
	if(barlen<=0)barlen=0;
	newpos=lastpos+barlen;
	user_interface.bar(
	    lastpos,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
	    newpos,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+13,
	    mode_admission_bar_color[i]);
	if(barlen>20)
	    font_barcharts.center_put(
		lastpos,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
		newpos,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+13,
		m.format(bar[0][i],4)
		);
	lastpos=newpos;
    }

    // BAR 2 //
    for(i=0,total=0;i<3;i++) {
	total+=bar[1][i];
    }

    //##chwg0525
    if(total==0)total=1;

    for(i=0,lastpos=series_x1;i<3;i++) {
	barlen=bar[1][i]*(series_x2-series_x1)/total;
	if(barlen<=0)barlen=0;
	newpos=lastpos+barlen;
	user_interface.bar(
	    lastpos,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
	    newpos,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+13,
	    mode_admission_bar_color[i]);
	if(barlen>20)
	    font_barcharts.center_put(
		lastpos,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
		newpos,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+13,
		m.format(bar[1][i],4)
		);
	lastpos=newpos;
    }

    // BAR 3 //
    for(i=0,total=0;i<3;i++) {
	total+=bar[2][i];
    }

    //##chwg0525
    if(total==0)total=1;

    for(i=0,lastpos=series_x1;i<3;i++) {
	barlen=bar[2][i]*(series_x2-series_x1)/total;
	if(barlen<=0)barlen=0;
	newpos=lastpos+barlen;
	user_interface.bar(
	    lastpos,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
	    newpos,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+13,
	    mode_admission_bar_color[i]);
	if(barlen>20)
	    font_barcharts.center_put(
		lastpos,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
		newpos,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+13,
		m.format(bar[2][i],4)
		);
	lastpos=newpos;
    }

    // BAR 4 //
    for(i=0,total=0;i<3;i++) {
	total+=bar[3][i];
    }

    //##chwg0525
    if(total==0)total=1;

    for(i=0,lastpos=series_x1;i<3;i++) {
	barlen=bar[3][i]*(series_x2-series_x1)/total;
	if(barlen<=0)barlen=0;
	newpos=lastpos+barlen;
	user_interface.bar(
	    lastpos,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
	    newpos,
	    (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+13,
	    mode_admission_bar_color[i]);
	if(barlen>20)
	    font_barcharts.center_put(
		lastpos,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
		newpos,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+13,
		m.format(bar[3][i],4)
		);
	lastpos=newpos;
    }

    // blit the statics
    vga.blt_buf((REPORT_X2)-150,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
		(REPORT_X2),
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chartsm.max_font_height);
    vga.blt_buf((REPORT_X2)-150,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
		(REPORT_X2),
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chartsm.max_font_height);
    vga.blt_buf((REPORT_X2)-150,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
		(REPORT_X2),
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chartsm.max_font_height);
    vga.blt_buf((REPORT_X2)-150,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
		(REPORT_X2),
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chartsm.max_font_height);

    // blit the bars
    vga.blt_buf((REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
		(REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+14);
    vga.blt_buf((REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
		(REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+14);
    vga.blt_buf((REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
		(REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+14);
    vga.blt_buf((REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
		(REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		(REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+14);
    // end Kevin 0716

}

//---------- End of function report_admission -------------//

#define HISTORY_GRAPH_COUNT HISTORY_GRAPH_COUNT_GENERAL

//-------- Begin of function report_statistics ------------//
//!
static void report_statistics(int refreshFlag) {
    //	Begin - Communicate with Sim Engine's Output
    static int  numSeriesData = HISTORY_YEAR_COUNT;
    static int  numSeriesData2 = HISTORY_TRIMESTER_COUNT;
    static int  *dataArray[HISTORY_GRAPH_COUNT];
    static int dataArray2[HISTORY_TRIMESTER_COUNT];
    static int  xStart,xEnd;

  // top title
    user_interface.brighten(REPORT_X1+100,REPORT_Y1-23,REPORT_X1+630,REPORT_Y1);
    user_interface.rect(REPORT_X1+100,REPORT_Y1-23,REPORT_X1+630,REPORT_Y1,1);
    font_charts.put(REPORT_X1+270,REPORT_Y1-17,"Quality and Selectivity Trends");

    user_interface.darken(  MODE_STATISTICS_SUB_BUTTON_X1-50,REPORT_Y2-54,
			    MODE_STATISTICS_SUB_BUTTON_X1+600,REPORT_Y2+6);
    user_interface.brighten(  MODE_STATISTICS_SUB_BUTTON_X1-50,REPORT_Y2-54,
			      MODE_STATISTICS_SUB_BUTTON_X1+600,REPORT_Y2+6);
    user_interface.rect(  MODE_STATISTICS_SUB_BUTTON_X1-50,REPORT_Y2-54,
			  MODE_STATISTICS_SUB_BUTTON_X1+600,REPORT_Y2+6,1);

    // Starting year
    xStart = info.game_year-numSeriesData+1;
    xEnd = info.game_year;

    dataArray[0]=enroll_res.matric_history[adm_office.sub_current_mode][ER_CREDITS_DEMANDED];
    for ( int i=0; i<HISTORY_TRIMESTER_COUNT; i++ )
	dataArray2[i]=enroll_res.distance_credits_provided_history[i];
    dataArray[2]=enroll_res.matric_history[adm_office.sub_current_mode][ER_ACADEMIC];
    dataArray[3]=enroll_res.matric_history[adm_office.sub_current_mode][ER_PERCENT_MINORITY];
    dataArray[4]=enroll_res.matric_history[adm_office.sub_current_mode][ER_EXTRACURRICULAR];
    dataArray[5]=enroll_res.matric_history[adm_office.sub_current_mode][ER_ATHLETICS];

    //	End - Communicate with Sim Engine's Output

    if (refreshFlag == INFO_REPAINT) {
	if (mode_init_flag != MODE_STATISTICS) {
	    //--------- initialization of the graphs -----------//

	    // Notes:	graph settings
	    //				- no legend
	    //				- no axis labels
	    //				- transparent
	    //				- display most updated value
	    //				- number format 3 - percentage

	    if(adm_office.sub_current_mode==0) {
		mode_statistics_graph_admit_rate.init(
		    REPORT_X1,
		    REPORT_Y1+5,
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH,
		    REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+5,
		    1, &numSeriesData, dataArray[0], 0,     /*temp*/
		    NULL, NULL, NULL, 1, 1, 1, 1);
		distance_credits_graph.init(
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		    REPORT_Y1+5,
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*2+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		    REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+5,
		    1, &numSeriesData2, dataArray2, 0,
		    NULL, NULL, NULL, 1, 1, 1);

		mode_statistics_graph_avg_academic_rating.init(
		    REPORT_X1+(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)*2,
		    REPORT_Y1+5,
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*3+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE*2,
		    REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+5,
		    1, &numSeriesData, dataArray[2], 0,     /*temp*/
		    NULL, NULL, NULL, 1, 1, 1, 3);
		mode_statistics_graph_percent_minorities.init(
		    REPORT_X1,
		    REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE,
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH,
		    REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT*2+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE,
		    1, &numSeriesData, dataArray[3], 0,     /*temp*/
		    NULL, NULL, NULL, 1, 1, 1, 3);
		mode_statistics_graph_avg_extracurricular_activities_rating.init(
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		    REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE,
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*2+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		    REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT*2+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE,
		    1, &numSeriesData, dataArray[4], 0,     /*temp*/
		    NULL, NULL, NULL, 1, 1, 1, 3);
		mode_statistics_graph_avg_athletics_rating.init(
		    REPORT_X1+(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)*2,
		    REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE,
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*3+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE*2,
		    REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT*2+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE,
		    1, &numSeriesData, dataArray[5], 0,     /*temp*/
		    NULL, NULL, NULL, 1, 1, 1, 3);
	    }
	    else {
		mode_statistics_graph_admit_rate.init(
		    REPORT_X1,
		    REPORT_Y1+70,
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH,
		    REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+70,
		    1, &numSeriesData, dataArray[0], 0,     /*temp*/
		    NULL, NULL, NULL, 1, 1, 1, 1);
		distance_credits_graph.init(
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		    REPORT_Y1+70,
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*2+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		    REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+70,
		    1, &numSeriesData2, dataArray2, 0,
		    NULL, NULL, NULL, 1, 1, 1);

		mode_statistics_graph_avg_academic_rating.init(
		    REPORT_X1+(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)*2,
		    REPORT_Y1+70,
		    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*3+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE*2,
		    REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+70,
		    1, &numSeriesData, dataArray[2], 0,     /*temp*/
		    NULL, NULL, NULL, 1, 1, 1, 3);
	    }
	    mode_init_flag = MODE_STATISTICS;

		int i;
	    short x = MODE_STATISTICS_SUB_BUTTON_X1-35;
	    for (i = 0; i < 2; i++, x+=SUB_BOTTOM_BUTTON_WIDTH+SUB_BOTTOM_BUTTON_DISTANCE+105) {
		user_interface.create_button_bitmap(
		    x, MODE_STATISTICS_SUB_BUTTON_Y1,
		    x+SUB_BOTTOM_BUTTON_WIDTH+105,
		    MODE_STATISTICS_SUB_BUTTON_Y1+SUB_BOTTOM_BUTTON_HEIGHT,
		    sub_bottom_button_label[i],
		    &sub_bottom_button_up_bitmap[i],
		    &sub_bottom_button_down_bitmap[i], &vga_back);
		sub_bottom_button_group[i].create_bitmapW(
		    x, MODE_STATISTICS_SUB_BUTTON_Y1,
		    x+SUB_BOTTOM_BUTTON_WIDTH+105,
		    MODE_STATISTICS_SUB_BUTTON_Y1+SUB_BOTTOM_BUTTON_HEIGHT,
		    ((i == adm_office.sub_current_mode) ?
		     sub_bottom_button_down_bitmap[i] :
		     sub_bottom_button_up_bitmap[i])+4);
	    }
	    for (i = 2,x = MODE_STATISTICS_SUB_BUTTON_X1-35; i < 5; i++, x+=SUB_BOTTOM_BUTTON_WIDTH+SUB_BOTTOM_BUTTON_DISTANCE) {
		user_interface.create_button_bitmap(
		    x, MODE_STATISTICS_SUB_BUTTON_Y1+26,
		    x+SUB_BOTTOM_BUTTON_WIDTH,
		    MODE_STATISTICS_SUB_BUTTON_Y1+SUB_BOTTOM_BUTTON_HEIGHT+26,
		    sub_bottom_button_label[i],
		    &sub_bottom_button_up_bitmap[i],
		    &sub_bottom_button_down_bitmap[i], &vga_back);
		sub_bottom_button_group[i].create_bitmapW(
		    x, MODE_STATISTICS_SUB_BUTTON_Y1+26,
		    x+SUB_BOTTOM_BUTTON_WIDTH,
		    MODE_STATISTICS_SUB_BUTTON_Y1+SUB_BOTTOM_BUTTON_HEIGHT+26,
		    ((i == adm_office.sub_current_mode) ?
		     sub_bottom_button_down_bitmap[i] :
		     sub_bottom_button_up_bitmap[i])+4);
	    }
	}
	//----------- paint the bottom button group ---------//

	sub_bottom_button_group.paint(adm_office.sub_current_mode);

	//-------- repaint the graphs -----------//
	for (int i = 0; i < ((adm_office.sub_current_mode==0)?6:3); i++) {
	    // skip the graph when i == 1; replace by another graph
	    if ( i == 1 )
		continue;
	    mode_statistics_graph[i]->paint();
	}

	distance_credits_graph.paint();

	//-------- draw the graph title ---------//
	if(adm_office.sub_current_mode==0)
	    for (int i = 0; i < 6; i++) {
		font_chartsm.put_paragraph(
		    REPORT_X1+(i%3)*(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE),
		    REPORT_Y1+(i/3)*(MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE)+MODE_STATISTICS_GRAPH_HEIGHT+7,
		    REPORT_X1+(i%3)*(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)+MODE_STATISTICS_GRAPH_WIDTH+30,
		    REPORT_Y1+(i/3)*(MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE)+MODE_STATISTICS_GRAPH_HEIGHT+7+3*font_chartsm.max_font_height,
		    mode_statistics_graph_title[i]);
	    }
	else
	    for (int i = 0; i < 3; i++) {
		font_chartsm.put_paragraph(
		    REPORT_X1+(i%3)*(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE),
		    REPORT_Y1+(i/3)*(MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE)+MODE_STATISTICS_GRAPH_HEIGHT+72,
		    REPORT_X1+(i%3)*(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)+MODE_STATISTICS_GRAPH_WIDTH,
		    REPORT_Y1+(i/3)*(MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE)+MODE_STATISTICS_GRAPH_HEIGHT+72+3*font_chartsm.max_font_height,
		    mode_statistics_graph_title[i]);
	    }
    }
    else {                                          // refreshFlag == INFO_REFRESH
	//---------- refresh the graphs -----------//
	for (int i = 0; i < ((adm_office.sub_current_mode==0)?6:3); i++) {
	    if ( i == 1 )
		continue;
	    mode_statistics_graph[i]->refresh();
	}

	distance_credits_graph.refresh();

	if(adm_office.sub_current_mode==0) {
	    vga.blt_buf(
		REPORT_X1,
		REPORT_Y1+5,
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH,
		REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+5);
	    vga.blt_buf(
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		REPORT_Y1+5,
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*2+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+5);
	    vga.blt_buf(
		REPORT_X1+(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)*2,
		REPORT_Y1+5,
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*3+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE*2,
		REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+5);
	    vga.blt_buf(
		REPORT_X1,
		REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE,
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH,
		REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT*2+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE);

	    vga.blt_buf(
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE,
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*2+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT*2+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE);

	    vga.blt_buf(
		REPORT_X1+(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)*2,
		REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE,
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*3+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE*2,
		REPORT_Y1+5+MODE_STATISTICS_GRAPH_HEIGHT*2+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE);
	}
	else {
	    vga.blt_buf(
		REPORT_X1,
		REPORT_Y1+70,
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH,
		REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+70);
	    vga.blt_buf(
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		REPORT_Y1+70,
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*2+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE,
		REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+70);
	    vga.blt_buf(
		REPORT_X1+(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)*2,
		REPORT_Y1+70,
		REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*3+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE*2,
		REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+70);
	}
    }

    //&&
}

//---------- End of function report_statistics ------------//

//-------- Begin of function report_priority --------------//
//!
static void report_priority(int refreshFlag) {

    //*** temporary ***//
    static int sliderVar1 = 8, sliderVar2 = 7, sliderVar3 = 3, sliderVar4 = 4;
    static int radioVar1 = 1, radioVar2 = 1;        //*** temporary ***//
    static int radioVar3 = 1;

    // top title
    user_interface.brighten(REPORT_X1+100,REPORT_Y1-23,REPORT_X1+630,REPORT_Y1);
    user_interface.rect(REPORT_X1+100,REPORT_Y1-23,REPORT_X1+630,REPORT_Y1,1);
    font_charts.put(REPORT_X1+265,REPORT_Y1-17,"Admissions Priorities");

    //	Begin - Communicate with Sim Engine's Output
    sliderVar1  = (int)enroll_res.offers_priority[0]*1000;
    sliderVar2  = (int)enroll_res.offers_priority[1]*1000;
    sliderVar3  = (int)enroll_res.offers_priority[2]*1000;
    //	sliderVar4	= (int)enroll_res.offers_priority[2];
    radioVar1 = enroll_res.minority_offers_special;
    radioVar2 = enroll_res.athlete_offers_special;
    radioVar3 = enroll_res.distance_learner_acceptance;
    //	End - Communicate with Sim Engine's Output

    if (refreshFlag == INFO_REPAINT) {
	//--------- draw the brightened boxes ------------//
	user_interface.brighten(REPORT_X1+5, REPORT_Y1+5, REPORT_X2-3, REPORT_Y1+MODE_PRIORITY_BOX_HEIGHT-3);
	user_interface.brighten(REPORT_X1+5, REPORT_Y2+5-MODE_PRIORITY_BOX_HEIGHT, REPORT_X2-3, REPORT_Y2-3);
	user_interface.rect(REPORT_X1+5, REPORT_Y1+5, REPORT_X2-3, REPORT_Y1+MODE_PRIORITY_BOX_HEIGHT-3);
	user_interface.rect(REPORT_X1+5, REPORT_Y2+5-MODE_PRIORITY_BOX_HEIGHT, REPORT_X2-3, REPORT_Y2-3);

	if (mode_init_flag != MODE_PRIORITY) {
	    mode_init_flag = MODE_PRIORITY;
	    //--------- initialize the sliders ---------------//
	    mode_priority_value_slider_academic_rating.init(
		REPORT_X1+MODE_PRIORITY_BOX_TAB2,
		REPORT_Y1+40,
		MODE_PRIORITY_VALUE_SLIDER_WIDTH,
		MODE_PRIORITY_VALUE_SLIDER_HEIGHT,
		0, 10000, &sliderVar1, 1, V_YELLOW, 9);

	    mode_priority_value_slider_extracur_rating.init(
		REPORT_X1+MODE_PRIORITY_BOX_TAB2,
		REPORT_Y1+80,
		MODE_PRIORITY_VALUE_SLIDER_WIDTH,
		MODE_PRIORITY_VALUE_SLIDER_HEIGHT,
		0, 10000, &sliderVar2, 1, V_YELLOW, 9);

	    mode_priority_value_slider_athletic_rating.init(
		REPORT_X1+MODE_PRIORITY_BOX_TAB2,
		REPORT_Y1+120,
		MODE_PRIORITY_VALUE_SLIDER_WIDTH,
		MODE_PRIORITY_VALUE_SLIDER_HEIGHT,
		0, 10000, &sliderVar3, 1, V_YELLOW, 9);

	    //			mode_priority_value_slider_distance_learning.init(
	    //				REPORT_X1+MODE_PRIORITY_BOX_TAB2,
	    //				REPORT_Y1+120,
	    //				MODE_PRIORITY_VALUE_SLIDER_WIDTH,
	    //				MODE_PRIORITY_VALUE_SLIDER_HEIGHT,
	    //				0, 10, &sliderVar4, 1, V_YELLOW, 4);

	    //--------- initialize the radio buttons ---------------//
	    mode_priority_radio_button_minorities.init(
		REPORT_X1+MODE_PRIORITY_BOX_TAB3,
		REPORT_Y2-MODE_PRIORITY_BOX_HEIGHT+35,
		REPORT_X1+MODE_PRIORITY_BOX_TAB3+MODE_PRIORITY_RADIO_BUTTON_WIDTH,
		REPORT_Y2-MODE_PRIORITY_BOX_HEIGHT+65,
		(news_event_function_array[NEWS_COURT_LIMIT_MINORITY-1].odd_chance == 0)?2:3,
		&radioVar1, mode_priority_radio_button_option_str);

	    mode_priority_radio_button_athletes.init(
		REPORT_X1+MODE_PRIORITY_BOX_TAB3,
		REPORT_Y2-MODE_PRIORITY_BOX_HEIGHT+85,
		REPORT_X1+MODE_PRIORITY_BOX_TAB3+MODE_PRIORITY_RADIO_BUTTON_WIDTH,
		REPORT_Y2-MODE_PRIORITY_BOX_HEIGHT+115,
		3, &radioVar2, mode_priority_radio_button_option_str);

	    mode_priority_radio_button_distance_learners.init(
		REPORT_X1+MODE_PRIORITY_BOX_TAB3,
		REPORT_Y2-MODE_PRIORITY_BOX_HEIGHT+130,
		REPORT_X1+MODE_PRIORITY_BOX_TAB3+MODE_PRIORITY_RADIO_BUTTON_WIDTH,
		REPORT_Y2-MODE_PRIORITY_BOX_HEIGHT+160,
		3, &radioVar3, mode_priority_radio_button_option_str1);
	}
	else {
	    // repaint
	    mode_priority_radio_button_minorities.paint();
	    mode_priority_radio_button_athletes.paint();
	    mode_priority_radio_button_distance_learners.paint();
	    mode_priority_value_slider_academic_rating.paint();
	    mode_priority_value_slider_extracur_rating.paint();
	    mode_priority_value_slider_athletic_rating.paint();
	    //			mode_priority_value_slider_distance_learning.paint();
	}
	// display the text
	font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y1+42,mode_priority_str[1]);
	font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y1+82,mode_priority_str[2]);
	font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y1+122,mode_priority_str[3]);

	//		font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y1+122,mode_priority_str[4]);
	font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y2-145,mode_priority_str[5]);
	font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y2-95,mode_priority_str[6]);
	font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1+190,REPORT_Y1+15,mode_priority_str[7]);
	font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y2-50,mode_priority_str[8]);
    }
    else {                                          // refreshFlag == INFO_REFRESH

	mode_priority_radio_button_minorities.refresh();
	mode_priority_radio_button_athletes.refresh();
	mode_priority_radio_button_distance_learners.refresh();
	mode_priority_value_slider_academic_rating.refresh();
	mode_priority_value_slider_extracur_rating.refresh();
	mode_priority_value_slider_athletic_rating.refresh();
	//		mode_priority_value_slider_distance_learning.refresh();

    }
}

//---------- End of function report_priority --------------//

//-------- Begin of function report_financial_aid ---------//
//!
static void report_financial_aid(int refreshFlag) {
    // Notes:	temporary variables for debugging
    //				to be interfaced with game engine
    // Norris 0714

    //*** temporary ***//
    static int sliderVar1 = 18, sliderVar2 = 37, sliderVar3 = 37;
    //*** temporary ***//
    static int radioVar1 = 1, radioVar2 = 1, radioVar3 = 1;

    //	Begin - Communicate with Sim Engine's Output
    sliderVar1  = (int)(enroll_res.fraction_need_covered*100);
    sliderVar2  = (int)(enroll_res.fraction_sl1_offered_merit_aid*100);
    sliderVar3  = (int)(enroll_res.fraction_sl2_offered_aid*100);
    //	radioVar1	= enroll_res.minority_aid_special;
    radioVar2 = enroll_res.minority_aid_special;
    radioVar3 = enroll_res.athlete_aid_special;
    //	End - Communicate with Sim Engine's Output

    if (refreshFlag == INFO_REPAINT) {
	// top title
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-23,REPORT_X1+630,REPORT_Y1);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-23,REPORT_X1+630,REPORT_Y1,1);
	font_charts.put(REPORT_X1+260,REPORT_Y1-17,"Financial Aid Priorities");

	//--------- draw the brightened boxes ------------//
	user_interface.brighten(REPORT_X1, 10+REPORT_Y1, REPORT_X2, 10+REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1);
	user_interface.brighten(REPORT_X1, 10+REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_DISTANCE, REPORT_X2, 10+REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_DISTANCE);
	user_interface.brighten(REPORT_X1, 10+REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_DISTANCE*2, REPORT_X2, 10+REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_HEIGHT_3+MODE_FINANCIAL_AID_BOX_DISTANCE*2);
	user_interface.rect(REPORT_X1, 10+REPORT_Y1, REPORT_X2, 10+REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1);
	user_interface.rect(REPORT_X1, 10+REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_DISTANCE, REPORT_X2, 10+REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_DISTANCE);
	user_interface.rect(REPORT_X1, 10+REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_DISTANCE*2, REPORT_X2, 10+REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_HEIGHT_3+MODE_FINANCIAL_AID_BOX_DISTANCE*2);

	if (mode_init_flag != MODE_FINANCIAL_AID) {
	    //--------- initialize the sliders ---------------//
	    mode_financial_aid_value_slider_need.init(
		REPORT_X2-MODE_FINANCIAL_AID_VALUE_SLIDER_WIDTH-20,
		REPORT_Y1+43,
		MODE_FINANCIAL_AID_VALUE_SLIDER_WIDTH,
		MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT,
		0, 100, &sliderVar1, 1, V_YELLOW, 3);

	    // testing the  scroll bar
	    //			mode_financial_aid_value_slider_need.init(1,
	    //				REPORT_X1,
	    //				REPORT_Y1,
	    //				REPORT_X1+14,
	    //				REPORT_Y2,2,3,11);
	    //
	    mode_financial_aid_value_slider_merit.init(
		REPORT_X2-MODE_FINANCIAL_AID_VALUE_SLIDER_WIDTH-20,
		REPORT_Y1+76,
		MODE_FINANCIAL_AID_VALUE_SLIDER_WIDTH,
		MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT,
		0, 100, &sliderVar2, 1, V_YELLOW, 3);

	    mode_financial_aid_value_slider_nontraditional.init(
		REPORT_X2-MODE_FINANCIAL_AID_RADIO_BUTTON_WIDTH*2-15,
		REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_DISTANCE+(MODE_FINANCIAL_AID_BOX_HEIGHT_2/4)*2+20,
		MODE_FINANCIAL_AID_VALUE_SLIDER_WIDTH,
		MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT,
		0, 100, &sliderVar3, 1, V_YELLOW, 3);

	    //--------- initialize the radio buttons ---------//
	    //			mode_financial_aid_radio_button_nontraditional.init(
	    //				REPORT_X2-MODE_FINANCIAL_AID_RADIO_BUTTON_WIDTH*2-15,
	    //				REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_DISTANCE+(MODE_FINANCIAL_AID_BOX_HEIGHT_2/4)*2,
	    //				REPORT_X2-15,
	    //				REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_DISTANCE+(MODE_FINANCIAL_AID_BOX_HEIGHT_2/4)*2+MODE_FINANCIAL_AID_RADIO_BUTTON_HEIGHT,
	    //				2, &radioVar1, mode_financial_aid_radio_button_option_str1);

	    mode_financial_aid_radio_button_minorities.init(
		REPORT_X2-MODE_FINANCIAL_AID_RADIO_BUTTON_WIDTH*3-15,
		REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_DISTANCE*2+(MODE_FINANCIAL_AID_BOX_HEIGHT_3/7)*2+20,
		REPORT_X2-15,
		REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_DISTANCE*2+(MODE_FINANCIAL_AID_BOX_HEIGHT_3/7)*2+MODE_FINANCIAL_AID_RADIO_BUTTON_HEIGHT+20,
		(news_event_function_array[NEWS_COURT_LIMIT_MINORITY-1].odd_chance == 0)?2:3,
		&radioVar2, mode_financial_aid_radio_button_option_str2);

	    mode_financial_aid_radio_button_athletes.init(
		REPORT_X2-MODE_FINANCIAL_AID_RADIO_BUTTON_WIDTH*3-15,
		REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_DISTANCE*2+(MODE_FINANCIAL_AID_BOX_HEIGHT_3/7)*5+20,
		REPORT_X2-15,
		REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_DISTANCE*2+(MODE_FINANCIAL_AID_BOX_HEIGHT_3/7)*5+MODE_FINANCIAL_AID_RADIO_BUTTON_HEIGHT+20,
		3, &radioVar3, mode_financial_aid_radio_button_option_str2);
	    mode_init_flag = MODE_FINANCIAL_AID;
	}
	else {
	    //-------- repaint the sliders and radio buttons -----------//
	    mode_financial_aid_value_slider_need.paint();
	    mode_financial_aid_value_slider_merit.paint();
	    mode_financial_aid_value_slider_nontraditional.paint();
	    mode_financial_aid_radio_button_minorities.paint();
	    mode_financial_aid_radio_button_athletes.paint();
	}
	//-------- display the strings -----------//
	font_chartsm.put(REPORT_X1+25, REPORT_Y1+36, mode_financial_aid_str[0]);
	font_chartsm.put(REPORT_X1+25, REPORT_Y1+69, mode_financial_aid_str[1]);
	font_chartsm.put(REPORT_X1+25, REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_DISTANCE+MODE_FINANCIAL_AID_BOX_HEIGHT_2/4+20, mode_financial_aid_str[2]);
	font_chartsm.put(REPORT_X1+25, REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_DISTANCE*2+MODE_FINANCIAL_AID_BOX_HEIGHT_3/7+20, mode_financial_aid_str[3]);
	font_chartsm.put(REPORT_X1+25, REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_HEIGHT_2+MODE_FINANCIAL_AID_BOX_DISTANCE*2+MODE_FINANCIAL_AID_BOX_HEIGHT_3/7*4+20, mode_financial_aid_str[4]);
    }
    else {                                          // refreshFlag == INFO_REFRESH

	// testing the scroll bar
	//		mode_financial_aid_value_slider_need.refresh(1);
	mode_financial_aid_value_slider_need.refresh();
	mode_financial_aid_value_slider_merit.refresh();
	mode_financial_aid_value_slider_nontraditional.refresh();
	mode_financial_aid_radio_button_minorities.refresh();
	mode_financial_aid_radio_button_athletes.refresh();
    }
}

//---------- End of function report_financial_aid ---------//

//-------- Begin of function detect_admission ---------//
//!
static int detect_admission() {
    help.set_help(
	REPORT_X1+110,
	REPORT_Y1+MODE_ADMISSION_BARCHART_ROW1-10,
	REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
	REPORT_Y1+MODE_ADMISSION_BARCHART_ROW1+40,
	"H4.1.1.");
    help.set_help(
	REPORT_X1+110,
	REPORT_Y1+MODE_ADMISSION_BARCHART_ROW2-15,
	REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
	REPORT_Y1+MODE_ADMISSION_BARCHART_ROW2+35,
	"H4.1.2.");
    help.set_help(
	REPORT_X1+110,
	REPORT_Y1+MODE_ADMISSION_BARCHART_ROW3-5,
	REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
	REPORT_Y1+MODE_ADMISSION_BARCHART_ROW3+45,
	"H4.1.3.");

    for(int i=0;i<4;i++)
	if(i!=3)
	    if(mode_admission_target[i].detect()) {
		enroll_res.target_enrollment[i]=(*mode_admission_target[i].var_ptr);
		//				double tmp_total=enroll_res.total_matrics[i]+enroll_res.total_offers[i]+enroll_res.total_applications[i];
		//				if(tmp_total>1e-5)
		//					enroll_res.target_student_intake_ratio[i]=((double)enroll_res.target_student_intake[i])/(tmp_total);
	    }
    return 0;
}

//---------- End of function detect_admission ---------//

//-------- Begin of function detect_statistics ---------//
//!
static int detect_statistics() {
    vga.use_front();
    int bottomButtonGroupDetect;
    if ((bottomButtonGroupDetect = sub_bottom_button_group.detect()) >= 0) {
	//-------- change display mode ------------//
	sub_bottom_button_group[adm_office.current_mode].body_ptr = (void *) (bottom_button_up_bitmap[adm_office.current_mode] + 4);
	adm_office.sub_current_mode = bottomButtonGroupDetect;
	sub_bottom_button_group[adm_office.current_mode].body_ptr = (void *) (bottom_button_down_bitmap[adm_office.current_mode] + 4);
	//-------- post display redraw flag -------//
	sys.redraw_all_flag = 1;
	return 1;
    }
    return 0;
}

//---------- End of function detect_statistics ---------//

//-------- Begin of function detect_priority ---------//
//!
static int detect_priority() {
    if (mode_init_flag != MODE_PRIORITY)
	return 0;

    help.set_help(  REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y1+42,
		    REPORT_X1+MODE_PRIORITY_BOX_TAB1+600,REPORT_Y1+80,"H4.2.1.");
    help.set_help(  REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y1+82,
		    REPORT_X1+MODE_PRIORITY_BOX_TAB1+600,REPORT_Y1+120,"H4.2.2.");
    help.set_help(  REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y1+122,
		    REPORT_X1+MODE_PRIORITY_BOX_TAB1+600,REPORT_Y1+160,"H4.2.3.");

    help.set_help(
	REPORT_X1+MODE_PRIORITY_BOX_TAB1,
	REPORT_Y2-MODE_PRIORITY_BOX_HEIGHT+25,
	REPORT_X1+MODE_PRIORITY_BOX_TAB3+MODE_PRIORITY_RADIO_BUTTON_WIDTH,
	REPORT_Y2-MODE_PRIORITY_BOX_HEIGHT+85,
	"H4.2.4.");

    help.set_help(
	REPORT_X1+MODE_PRIORITY_BOX_TAB1,
	REPORT_Y2-MODE_PRIORITY_BOX_HEIGHT+90,
	REPORT_X1+MODE_PRIORITY_BOX_TAB3+MODE_PRIORITY_RADIO_BUTTON_WIDTH,
	REPORT_Y2-MODE_PRIORITY_BOX_HEIGHT+150,
	"H4.2.5.");

    if (mode_priority_radio_button_minorities.detect()) {
	//--- Special handling: chance card Court Limit Preferences based on Race ---//

	if( chance_event.court_limit_minority_flag ) {
	    box.msg( "You cannot set this to high as courts ban favoring admission and financial aid on the basis of race or ethnicity." );

	    //---- do not allow pressing the "High" button ----//

	    if( *(mode_priority_radio_button_minorities.sel_num) == INPUT_HIGH ) {
		*(mode_priority_radio_button_minorities.sel_num) = INPUT_MEDIUM;
		mode_priority_radio_button_minorities.refresh();
	    }
	}

	enroll_res.minority_offers_special=*(mode_priority_radio_button_minorities.sel_num);;
	return 1;
    }

    if (mode_priority_radio_button_athletes.detect()) {
	enroll_res.athlete_offers_special=*(mode_priority_radio_button_athletes.sel_num);
	return 1;
    }

    if (mode_priority_radio_button_distance_learners.detect()) {
	enroll_res.distance_learner_acceptance=*(mode_priority_radio_button_distance_learners.sel_num);
	return 1;
    }

    if (mode_priority_value_slider_academic_rating.detect()) {
	enroll_res.offers_priority[0]=(int)(*(mode_priority_value_slider_academic_rating.var_ptr)/1000);
	return 1;
    }
    if (mode_priority_value_slider_extracur_rating.detect()) {
	enroll_res.offers_priority[1]=(int)(*(mode_priority_value_slider_extracur_rating.var_ptr)/1000);
	return 1;
    }

    if (mode_priority_value_slider_athletic_rating.detect()) {
	enroll_res.offers_priority[2]=(int)(*(mode_priority_value_slider_athletic_rating.var_ptr)/1000);
	return 1;
    }

    //	if (mode_priority_value_slider_distance_learning.detect())
    //	{	enroll_res.offers_priority[2]=*(mode_priority_value_slider_distance_learning.var_ptr);
    //		return 1;
    //	}

    return 0;
}

//---------- End of function detect_priority ---------//

//-------- Begin of function detect_financial_aid ---------//
//!
static int detect_financial_aid() {
    if (mode_init_flag != MODE_FINANCIAL_AID)
	return 0;

    help.set_help(  REPORT_X1+30,
		    REPORT_Y1+33,
		    REPORT_X2-15,
		    REPORT_Y1+65,"H4.3.1.");
    help.set_help(  REPORT_X1+30,
		    REPORT_Y1+66,
		    REPORT_X2-15,
		    REPORT_Y1+98,"H4.3.2.");
    help.set_help(  REPORT_X1+30,
		    REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_DISTANCE+(MODE_FINANCIAL_AID_BOX_HEIGHT_2/4)*2-15,
		    REPORT_X2-15,
		    REPORT_Y1+MODE_FINANCIAL_AID_BOX_HEIGHT_1+MODE_FINANCIAL_AID_BOX_DISTANCE+(MODE_FINANCIAL_AID_BOX_HEIGHT_2/4)*2+35,
		    "H4.3.3.");

    help.set_help(
	REPORT_X1+30,
	REPORT_Y2-MODE_FINANCIAL_AID_BOX_HEIGHT_1-15,
	REPORT_X2-15,
	REPORT_Y2-MODE_FINANCIAL_AID_BOX_HEIGHT_1+35,
	"H4.3.4.");

    help.set_help(
	REPORT_X1+30,
	REPORT_Y2-MODE_FINANCIAL_AID_BOX_HEIGHT_1+35,
	REPORT_X2-15,
	REPORT_Y2-MODE_FINANCIAL_AID_BOX_HEIGHT_1+85,
	"H4.3.5.");

    if(mode_financial_aid_value_slider_nontraditional.detect()) {
	enroll_res.fraction_sl2_offered_aid=((float)*(mode_financial_aid_value_slider_nontraditional.var_ptr))/(float)100.0;
	return 1;
    }

    if(mode_financial_aid_value_slider_need.detect()) {
	enroll_res.fraction_need_covered=((float)*(mode_financial_aid_value_slider_need.var_ptr))/(float)100.0;
	return 1;
    }

    if(mode_financial_aid_radio_button_minorities.detect()) {
	//--- Special handling: chance card Court Limit Preferences based on Race ---//

	if( chance_event.court_limit_minority_flag ) {
	    //---- do not allow pressing the "High" button ----//

	    if( *(mode_financial_aid_radio_button_minorities.sel_num) == INPUT_HIGH ) {
		box.msg( "You cannot set this to high as courts ban favoring admission and financial aid on the basis of race or ethnicity." );

		*(mode_financial_aid_radio_button_minorities.sel_num) = INPUT_MEDIUM;
		mode_financial_aid_radio_button_minorities.refresh();
	    }
	}

	//---------------------------------------------------//

	enroll_res.minority_aid_special=*(mode_financial_aid_radio_button_minorities.sel_num);
	return 1;
    }

    if(mode_financial_aid_radio_button_athletes.detect()) {
	enroll_res.athlete_aid_special=*(mode_financial_aid_radio_button_athletes.sel_num);
	return 1;
    }

    if(mode_financial_aid_value_slider_merit.detect()) {
	enroll_res.fraction_sl1_offered_merit_aid=(*(mode_financial_aid_value_slider_merit.var_ptr))/(float)100.0;
	return 1;
    }

    return 0;
}

//---------- End of function detect_financial_aid ---------//

//-------- Begin of function deinit_admission ---------//
//!
static void deinit_admission() {
    if (mode_init_flag == MODE_ADMISSION) {
	if(mode_admission_no_scaling_bitmap) {
	    mem_del(mode_admission_no_scaling_bitmap);
	    mode_admission_no_scaling_bitmap=NULL;
	}
	for (int i = 0; i < 5; i++) {
	    if(mode_admission_no_ar_bitmap[i]) {
		mem_del(mode_admission_no_ar_bitmap[i]);
		mode_admission_no_ar_bitmap[i]=NULL;
	    }
	    if(mode_admission_no_yr_bitmap[i]) {
		mem_del(mode_admission_no_yr_bitmap[i]);
		mode_admission_no_yr_bitmap[i]=NULL;
	    }
	    if(mode_admission_no_bar_bitmap[i]) {
		mem_del(mode_admission_no_bar_bitmap[i]);
		mode_admission_no_bar_bitmap[i]=NULL;
	    }
	}
	for(int i=0;i<5;i++)
	    if(i!=3)
		mode_admission_target[i].deinit();

	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_admission ---------//

//-------- Begin of function deinit_statistics ---------//
//!
static void deinit_statistics() {
    if (mode_init_flag == MODE_STATISTICS) {
	// deallocate memory used by the graphs
	for (int i = 0; i < 6; i++) {
	    mode_statistics_graph[i]->deinit();
	}
	// ##### begin Gilbert 28/04/2001 ####//
	sub_bottom_button_group.deinit_buttons();
	// ##### end Gilbert 28/04/2001 ####//
	if (*sub_bottom_button_up_bitmap) {
	    for (int i = 0; i < 5; i++) {
		mem_del(sub_bottom_button_up_bitmap[i]);
		mem_del(sub_bottom_button_down_bitmap[i]);
		sub_bottom_button_up_bitmap[i] = NULL;
		sub_bottom_button_down_bitmap[i] = NULL;
	    }
	}
	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_statistics ---------//

//-------- Begin of function deinit_priority ---------//
//!
static void deinit_priority() {
    if (mode_init_flag == MODE_PRIORITY) {
	mode_priority_radio_button_minorities.deinit();
	mode_priority_radio_button_athletes.deinit();
	mode_priority_radio_button_distance_learners.deinit();
	mode_priority_value_slider_academic_rating.deinit();
	mode_priority_value_slider_extracur_rating.deinit();
	mode_priority_value_slider_athletic_rating.deinit();

	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_priority ---------//

//-------- Begin of function deinit_financial_aid ------//
//!
static void deinit_financial_aid() {
    if (mode_init_flag == MODE_FINANCIAL_AID) {
	// deallocate memory used by the radio buttons and the value sliders
	mode_financial_aid_value_slider_nontraditional.deinit();
	mode_financial_aid_radio_button_minorities.deinit();
	mode_financial_aid_radio_button_athletes.deinit();
	mode_financial_aid_value_slider_need.deinit();
	mode_financial_aid_value_slider_merit.deinit();

	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_financial_aid ------//
