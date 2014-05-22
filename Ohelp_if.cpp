// Filename    : OHELP_IF.CPP
// Description : The screen of help topic and help desc.
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
#include <OVGA.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OBUTTON.H>
#include <OFINANCE.H>
//#include <OTXTBOX.H>
#include <OTXTLIST.H>
#include  <OHELPIF.H>

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-55
};

enum {
    BOTTOM_BUTTON_X0 = REPORT_X1+70,
    BOTTOM_BUTTON_X1 = REPORT_X1+170,
    BOTTOM_BUTTON_Y1 = REPORT_Y2+15,
    BOTTOM_BUTTON_X2 = REPORT_X2-20,
    BOTTOM_BUTTON_Y2 = REPORT_Y2+40
};

enum {
    BOTTOM_BUTTON_WIDTH = 125,
    BOTTOM_BUTTON_HEIGHT = 25,
    HELP_BUTTON_COUNT = 3,
    BOTTOM_BUTTON_DISTANCE
    = (BOTTOM_BUTTON_X2-BOTTOM_BUTTON_X1-BOTTOM_BUTTON_WIDTH*2)/3
};

enum {                                            // the bg image id
    BG_PIC_ID = 2
};

static ButtonGroup help_topic_bottom_button_group(2);
static ButtonGroup help_desc_bottom_button_group(3);
static short *bottom_button_up_bitmap[HELP_BUTTON_COUNT];
static short *bottom_button_down_bitmap[HELP_BUTTON_COUNT];
static char *help_desc_bottom_button_label[] = {
    "Help Topics","Back","Close"
};
static char *help_topic_bottom_button_label[] = {
    "Open","Cancel"
};
static int help_topic_report_init_flag;
static int help_desc_report_init_flag;

static int help_topic_current_mode=-1;
static int help_desc_current_mode=-1;
//------- Begin of function HelpIf::HelpIf -----------//
//!
HelpIf::HelpIf() {
    init();
}

//--------- End of function HelpIf::HelpIf -----------//

//------- Begin of function HelpIf::~HelpIf -----------//
//!
HelpIf::~HelpIf() {
    deinit();
}

//--------- End of function HelpIf::~HelpIf -----------//

//------- Begin of function HelpIf::deinit ---------------//
//!
void HelpIf::deinit() {
    //--------- deallocate memory -------------//
    for (int i = 0; i < HELP_BUTTON_COUNT; i++) {
	if (bottom_button_up_bitmap[i]) {
	    mem_del(bottom_button_up_bitmap[i]);
	    mem_del(bottom_button_down_bitmap[i]);
	    bottom_button_up_bitmap[i] = NULL;
	    bottom_button_down_bitmap[i] = NULL;
	}
    }
    help_textlist.deinit();
    help_topic_report_init_flag = 0;
    help_desc_report_init_flag = 0;
}

//--------- End of function HelpIf::deinit ---------------//

//------- Begin of function HelpIf::init -----------------//
//!
void HelpIf::init() {
    memset(this,0,sizeof(HelpIf));
    for (int i = 0; i < HELP_BUTTON_COUNT; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }

    help_topic_report_init_flag = 0;
    help_desc_report_init_flag = 0;
}

//--------- End of function HelpIf::init -----------------//

//------- Begin of function HelpIf::help_topic_report ---------------//
//!
void HelpIf::help_topic_report(int refreshFlag) {
    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	if(help_topic_report_init_flag )
	    // deinit it first if it has already been initialized
	    deinit();
	help_topic_current_mode=-1;
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	//----- initialize the bottom sub-menus buttons -----//
	if (!help_topic_report_init_flag) {
	    short x = BOTTOM_BUTTON_X1;
	    for (int i = 0; i < 2; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
		user_interface.create_button_bitmap(x, BOTTOM_BUTTON_Y1,
						    x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						    help_topic_bottom_button_label[i], &bottom_button_up_bitmap[i],
						    &bottom_button_down_bitmap[i], &vga_back);

		help_topic_bottom_button_group[i].create_bitmapW(x, BOTTOM_BUTTON_Y1,
								 x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
								 ((i == help_topic_current_mode) ? bottom_button_down_bitmap[i] :
								  bottom_button_up_bitmap[i])+4);
	    }

	    help_topic_report_init_flag = 1;
	    font_charts.put(REPORT_X1+300,REPORT_Y1-15,"Help Topics");

	    String str;
	    str  = DIR_RES;
	    str += "SCORE";
	    help_textlist.init(REPORT_X1,REPORT_Y1+5,REPORT_X2,REPORT_Y2,str);
	    help_textlist.init_list(REPORT_X1,REPORT_Y1+5,REPORT_X2,REPORT_Y2);
	}
	//----------- paint the bottom button group ---------//
	help_topic_bottom_button_group.paint(help_topic_current_mode);
    }

    help_textlist.disp(refreshFlag,help_textlist.topic_id); {

	//	if (refreshFlag == INFO_REPAINT)
	//------- blt the back buffer to front buffer -------//
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    }
    vga.use_front();
}

//---------- End of function HelpIf::help_topic_report ------------//

//---------- Begin of function HelpIf::help_desc_report ------------//
//! You can set the value of HelpIf::topic_id before running this functions
void HelpIf::help_desc_report(int refreshFlag) {
    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	if(help_desc_report_init_flag )
	    // deinit it first if it has already been initialized
	    deinit();

	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	//----- initialize the bottom sub-menus buttons -----//
	if (!help_desc_report_init_flag) {
	    short x = BOTTOM_BUTTON_X0;
	    help_desc_current_mode=-1;
	    for (int i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
		user_interface.create_button_bitmap(x, BOTTOM_BUTTON_Y1,
						    x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						    help_desc_bottom_button_label[i], &bottom_button_up_bitmap[i],
						    &bottom_button_down_bitmap[i], &vga_back);

		help_desc_bottom_button_group[i].create_bitmapW(x, BOTTOM_BUTTON_Y1,
								x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
								( ((i==1)&&(!help_textlist.can_back))||
								  (i == help_desc_current_mode) ? bottom_button_down_bitmap[i] :
								  bottom_button_up_bitmap[i])+4);
	    }

	    help_desc_bottom_button_group[1].enable_flag=help_textlist.can_back;
	    if(!help_textlist.can_back) {
		//help_desc_bottom_button_group[1].is_pushed=1;
		//help_desc_bottom_button_group[1].enable_flag=0;
		help_desc_bottom_button_group[1].body_ptr = (void *) (bottom_button_down_bitmap[1] + 4);
	    }

	    help_desc_report_init_flag = 1;
	    String str;
	    str  = DIR_RES;
	    str += "SCORE";
	    //			if(!help_textlist.init_flag)
	    help_textlist.init(REPORT_X1,REPORT_Y1+5,REPORT_X2,REPORT_Y2,str);
	    help_textlist.init_list_flag=0;
	}

	String help_title;
	help_title=help_textlist.res_txt.get_title(help_textlist.topic_id);
	int ww=help_title.len()*(font_charts.width()+3);
	int wx=(REPORT_X2+REPORT_X1-ww)/2;
	user_interface.brighten(wx-20,REPORT_Y1-25,wx+ww+10,REPORT_Y1+font_charts.height()-18);
	user_interface.rect(wx-20,REPORT_Y1-25,wx+ww+10,REPORT_Y1+font_charts.height()-18,2);
	font_charts.put((wx>0)?wx:0,REPORT_Y1-18,help_title);

	//----------- paint the bottom button group ---------//
	help_desc_bottom_button_group.paint(help_desc_current_mode);
	//		help_textlist.back_history();
	if(!help_textlist.can_back) {
	    short x=BOTTOM_BUTTON_X0+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE;
	    user_interface.darken(x, BOTTOM_BUTTON_Y1,
				  x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT);
	}
    }

    help_textlist.disp(refreshFlag,help_textlist.topic_id);

    if (refreshFlag == INFO_REPAINT) {
	//------- blt the back buffer to front buffer -------//
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
	sys.blt_virtual_buf();
    }
    vga.use_front();
}

//---------- End of function Development::help_desc_report ------------//

int HelpIf::help_topic_detect() {
    if (!help_topic_report_init_flag)
	return 0;

    int help_topic_bottomButtonGroupDetect;

    if ((help_topic_bottomButtonGroupDetect = help_topic_bottom_button_group.detect()) >= 0) {
	//-------- change display mode ------------//
	if(help_topic_current_mode>=0)
	    help_topic_bottom_button_group[help_topic_current_mode].body_ptr = (void *) (bottom_button_up_bitmap[help_topic_current_mode] + 4);
	else
	    for(int i=0;i<2;i++)
		help_topic_bottom_button_group[i].body_ptr = (void *) (bottom_button_up_bitmap[i] + 4);

	help_topic_current_mode = help_topic_bottomButtonGroupDetect;
	help_topic_bottom_button_group[help_topic_current_mode].body_ptr = (void *) (bottom_button_down_bitmap[help_topic_current_mode] + 4);

	//-------- post display redraw flag -------//
	if(help_topic_bottomButtonGroupDetect==0) {
	    // add current topic id to the history
	    help_textlist.add_history(help_textlist.topic_id);
	    sys.set_view_mode(MODE_HELP_DESC);
	}
	if((sys.back_view_mode==MODE_HELP_TOPIC)||
	   (sys.back_view_mode==MODE_HELP_DESC))
	    sys.back_view_mode=MODE_NORMAL;

	if(help_topic_bottomButtonGroupDetect==1)
	    sys.set_view_mode(sys.back_view_mode);
	sys.redraw_all_flag = 1;

	return 1;
    }

    vga.use_front();
    help_textlist.detect();
    if(help_textlist.double_clicked) {
	// add current topic id to the history
	help_textlist.add_history(help_textlist.topic_id);
	sys.set_view_mode(MODE_HELP_DESC);
	help_textlist.double_clicked=0;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////

int HelpIf::help_desc_detect() {
    if (!help_desc_report_init_flag)
	return 0;

    int help_desc_bottomButtonGroupDetect;

    if ((help_desc_bottomButtonGroupDetect = help_desc_bottom_button_group.detect()) >= 0) {
	if((!help_textlist.can_back)&&(help_desc_bottomButtonGroupDetect==1))return 0;

	if(help_desc_current_mode>=0)
	    //-------- change display mode ------------//
	    help_desc_bottom_button_group[help_desc_current_mode].body_ptr = (void *) (bottom_button_up_bitmap[help_desc_current_mode] + 4);
	else
	    for(int i=0;i<3;i++)
		help_desc_bottom_button_group[i].body_ptr = (void *) (bottom_button_up_bitmap[i] + 4);

	help_desc_current_mode = help_desc_bottomButtonGroupDetect;
	help_desc_bottom_button_group[help_desc_current_mode].body_ptr = (void *) (bottom_button_down_bitmap[help_desc_current_mode] + 4);

	//-------- post display redraw flag -------//
	if(help_desc_bottomButtonGroupDetect==0)sys.set_view_mode(MODE_HELP_TOPIC);
	if(help_desc_bottomButtonGroupDetect==1)help_textlist.back_history();
	if(help_desc_bottomButtonGroupDetect==2) {
	    if((sys.back_view_mode==MODE_HELP_TOPIC)||
	       (sys.back_view_mode==MODE_HELP_DESC))
		sys.back_view_mode=MODE_NORMAL;
	    sys.set_view_mode(sys.back_view_mode);
	}
	if(help_desc_bottomButtonGroupDetect==0 ||
	   help_desc_bottomButtonGroupDetect==1 ||
	   help_desc_bottomButtonGroupDetect==2)help_textlist.top_line=1;

	sys.redraw_all_flag = 1;
	return 1;
    }
    help_textlist.detect();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
