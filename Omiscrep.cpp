//Filename    : OMISCERP.CPP
//Description : class
// owner			: Fred.
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
#include <ODATE.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>

#include <OVGA.H>
#include <OINFO.H>
#include <OIFACE.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>

#include <OBUTTON.H>
//#include <ORADIOV.H>

#include <ONEWS.H>
#include <OPSCHOOL.H>
#include <OMISCREP.H>

//------- Define constants -------//

enum {                                            // the 'sub' view-modes
    MODE_AWARD1 = 0,
    //MODE_FINANCIAL = 1,
};

enum {                                            // the bg image id
    BG_PIC_ID = 19
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
    MODE_AWARD1_BOX1_HEIGHT = 120,
    MODE_AWARD1_BOX2_HEIGHT = 215,
    MODE_AWARD1_BOX_WIDTH = 650,
    MODE_AWARD1_TAB0 = 15,
    MODE_AWARD1_TAB1 = 100,
    MODE_AWARD1_TAB2 = 455,
};

enum {
    MODE_AWARD1_GRAPH_WIDTH = 370,
    MODE_AWARD1_GRAPH_HEIGHT = 160,
    MODE_AWARD1_LEGEND_HEIGHT = 26,
    MODE_AWARD1_LEGEND_WIDTH = 370,
};

// end kevin 0723

//-------- define static vars -----------//

static char report_init_flag = 0;
static char mode_init_flag = -1;                  // -1: non of the mode was init'ed

static char current_mode = MODE_AWARD1;

static ButtonGroup bottom_button_group(2);
static short *bottom_button_up_bitmap[2];
static short *bottom_button_down_bitmap[2];
static char *bottom_button_label[] = {
    "Overview", "BUGHER"
};

static Button button_back;

static char *plaque_image_str[] = { "AW_GOLD", "AW_SIL", "AW_COP" };

// Notes:	the strings could be moved to Ogamestr.cpp
// Begin MiscReport Mode Setting
//static GraphDoubleBarYear mode_award_graphdbar_winloss;
//static RadioButton mode_award_radio_button_participation;

//static char *mode_award_str4[] = {
//	"Number of top athletes :",

// End MiscReport Mode Setting

//-------- define static functions ------//

// Notes:	the deinit_x functions should deallocate memory used by the
//				GraphDoubleBar, ValueSlider, RadioButton, etc. objects
//				the functions are called when a mode-switching is triggered
//	Norris 0716

static void report_award(int);
static int detect_award();
static void deinit_award();

typedef void (*ReportFP)(int refreshFlag);
typedef int (*DetectFP)();
typedef void (*DeinitFP)();

static ReportFP report_func_ptr[] = {
    &report_award,
    //	&report_financial
};

static DetectFP detect_func_ptr[] = {
    &detect_award,
    //	&detect_financial
};

static DeinitFP deinit_func_ptr[] = {
    &deinit_award,
    //	&deinit_financial,
};

//------- Begin of function MiscReport::MiscReport -----------//
//!
MiscReport::MiscReport() {
    page_num=1;
    award_init();
}

//--------- End of function MiscReport::MiscReport -----------//

//------- Begin of function MiscReport::~MiscReport -----------//
//!
MiscReport::~MiscReport() {
    award_deinit();
}

//--------- End of function MiscReport::~MiscReport -----------//

//------- Begin of function MiscReport::deinit ---------------//
//!
void MiscReport::award_deinit() {
    //--------- deallocate memory -------------//
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

//--------- End of function MiscReport::deinit ---------------//

//------- Begin of function MiscReport::init -----------------//
//!
void MiscReport::award_init() {
    memset(this, 0, sizeof(MiscReport));

    for (int i = 0; i < 2; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }
}

//--------- End of function MiscReport::init -----------------//

//------- Begin of function MiscReport::report ---------------//
//!
void MiscReport::award_report(int refreshFlag) {
    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    award_deinit();

	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	//REMOVE

	//----- initialize the bottom sub-menus buttons -----//
	if (!report_init_flag) {
	    /*	short x = BOTTOM_BUTTON_X1;
		for (int i = 0; i < 2; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		{
		user_interface.create_button_bitmap(x, BOTTOM_BUTTON_Y1,
		x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
		bottom_button_label[i], &bottom_button_up_bitmap[i],
		&bottom_button_down_bitmap[i], &vga_back);

		bottom_button_group[i].create_bitmapW(x, BOTTOM_BUTTON_Y1,
		x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
		((i == current_mode) ? bottom_button_down_bitmap[i] : bottom_button_up_bitmap[i])+4);
		}
	    */
	    report_init_flag = 1;
	}

	//REMOVE
	//----------- paint the bottom button group ---------//
	//bottom_button_group.paint(current_mode);

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
    vga.use_front();                                //##trevor 2807
}

//---------- End of function MiscReport::report ------------//

//-------- Begin of function MiscReport::detect ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int MiscReport::award_detect() {
    vga.use_back();

    int rc = award_detect2();

    vga.use_front();

    return rc;
}

//---------- End of function MiscReport::detect ------------//

//-------- Begin of function MiscReport::detect2 ------------//
//!
//!	return value: 0-not detected, 1-detected
//!
int MiscReport::award_detect2() {
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

//-------- Begin of function report_award -------------//
//!
static void report_award(int refreshFlag) {
    int vtab1=REPORT_Y1+20,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    //static int radioVar1 = 1;	//*** temporary ***//
    static int numSeriesData = 8;                   //*** temporary ***//
    static int *dataArray;

    //	Communicate with Sim Engine's Output

    //		top_athletes=enroll_res.matrics_top_athletes;
    //		radioVar1 = athletics_office.ncaa_level_input;
    vga.use_back();

    // chwg031999 begin
    if (misc_report.page_num==1) {
	if (refreshFlag == INFO_REPAINT) {
	    user_interface.bg_img(7, &vga_back);
	    info.disp_column_bitmap(&vga_back);
	    Font* fontPtrArr[] = { &font_pgold, &font_psilver, &font_pbronze};
	    char * sentence[]= {
		"You have achieved the Summa Cum Laude plaque!",
		"You have achieved the Magna Cum Laude plaque!",
		"You have achieved the Cum Laude plaque!"
	    };

	    fontPtrArr[player_school.plaque_last]->center_put(
		REPORT_X1+30, REPORT_Y1+150, REPORT_X2-50, REPORT_Y1+180, "Congratulations!" );
	    fontPtrArr[player_school.plaque_last]->center_put(
		REPORT_X1+30, REPORT_Y1+180, REPORT_X2-50, REPORT_Y1+210, sentence[player_school.plaque_last]);

	    button_back.create_text(350,vtab4+10,450,vtab4+28, "next");
	    button_back.paint();
	}
	else {
	}
    }
    // chwg031999 end

    else if(misc_report.page_num==2) {
	if (refreshFlag == INFO_REPAINT) { {
	    //---------  initialization  -----------//
	    mode_init_flag= MODE_AWARD1;

	    image_interface.put_back(REPORT_X1+33,REPORT_Y1+20, plaque_image_str[player_school.plaque_last] );

	    String strT = player_school.player_first_name;
	    strT += " ";
	    strT += player_school.player_last_name;

	    Font* fontPtrArr[] = { &font_pgold, &font_psilver, &font_pbronze};

	    fontPtrArr[player_school.plaque_last]->center_put( MODE_AWARD1_TAB1,
							       REPORT_Y1+160,
							       //MODE_AWARD1_TAB1+20+strlen(player_school.player_last_name)*font_pgold.max_font_width,
							       50 + MODE_AWARD1_BOX_WIDTH,
							       REPORT_Y1+160+font_pgold.max_font_height,
							       strT);

	    // strT = date.date_str(info.game_date);
	    strT = info.date_str_julian(info.game_date);

	    fontPtrArr[player_school.plaque_last]->center_put(MODE_AWARD1_TAB1,
							      REPORT_Y1+210,
							      //MODE_AWARD1_TAB1+20+strlen(strT)*font_pgold.max_font_width,
							      50 + MODE_AWARD1_BOX_WIDTH,
							      REPORT_Y1+210+font_pgold.max_font_height,
							      strT);

	    button_back.create_text(350,vtab4+7,450,vtab4+28, "Proceed"); {
	    }
	    //else
	    //-------- repaint -----------//
	    // mode_award_radio_button_participation.paint();
	}

	//-------- graph text --------------//
	//font_san.put(REPORT_X1+MODE_AWARD1_TAB1,
	//						REPORT_Y1+15, mode_award_str4[0]);
	button_back.paint();
	}
	else {                                        // refreshFlag == INFO_REFRESH
	}
    }

    // button_back.paint();
}

//-------- Begin of function report_award ---------//

//-------- Begin of function detect_award ---------//
//!
static int detect_award() {
    // chwg031999 begin
    if(misc_report.page_num==1) {
	if(button_back.detect()) {
	    misc_report.page_num=2;
	    sys.redraw_all_flag=1;
	}
    }
    else if(misc_report.page_num==2) {
	if (mode_init_flag != MODE_AWARD1)
	    return 0;
	vga.use_front(); {
	    //if (mode_award_radio_button_participation.detect())
	    //athletics_office.ncaa_level_input=*(mode_award_radio_button_participation.sel_num);
	    //return 1;
	}

	if(button_back.detect()) {
	    //: go to last view mode or disp newspaper first!!!???
	    //		news_array.scenario_news(0, player_school.plaque_last, date.year(info.game_start_date) + player_school.goal_deadline[player_school.plaque_last]);
	    sys.news_pop_up_flag=0;                     // chwg031999 disable the newpopup after showing award.

	    sys.set_staying_view_mode(0);
	    sys.set_view_mode(MODE_NORMAL);
	    misc_report.page_num=1;
	}
    }
    // chwg031999 end
    return 0;
}

//---------- End of function detect_award ---------//

//-------- Begin of function deinit_award ---------//
//!
static void deinit_award() {
    if (mode_init_flag == MODE_AWARD1) {

	//mode_award_radio_button_participation.deinit();

	// reset flag
	mode_init_flag = -1;
    }
}

//---------- End of function deinit_award ---------//
