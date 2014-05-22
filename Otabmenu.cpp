//Filename    : OTABMENU.CPP
//Description : Object TabMennu

// Comments:	still has bugs, memory leak
//					has to be modified for 16-bit color
// Norris 0716

#include <ALL.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OIFACE.H>
#include <OTABMENU.H>
#include <OAUDIO.H>
#include <OCONFIG.H>

//--------- Define macro constant -----------//

enum {
    TAB_HEIGHT = 20,
    TAB_BOUNDARY = 10
};

//-------- Begin of function TabMenu::TabMenu ----//
//!
TabMenu::TabMenu() {
}

//---------- End of function TabMenu::TabMenu ----//

//-------- Begin of function TabMenu::~TabMenu ---//
//!
TabMenu::~TabMenu() {
    deinit();
}

//---------- End of function TabMenu::~TabMenu ---//

//-------- Begin of function TabMenu::init --------//
//!
//! <int>     x1, y1        = upper left corner of the radio button
//!									  (coordinate includes the tabs)
//! <int>		 x2, y2			= lower right corner of the radio button
//! <int>		 optionNum		= number of options
//! <int*>	 selNum			= ptr to selection number
//! <int>		 shortLength	= lenght of an unopened tab
//! <int>		 longLength		= length of an opened tab
//! <char**>	 shortDesc		= description of the tabs (unopened tab)
//! <char**>	 longDesc		= description of the tabs (opened tab)
//! <int*()>	 dispFunc(int, int)
//!									= pointer to display function
//!									  (parameter1 = selected tab)
//!									  (parameter2 = INFO_REPAINT/INFO_REFRESH)
//! <int*()>	 detectFunc(int)
//!									= pointer to detect function
//!									  (parameter = selected tab)
//!									  (return value = 1-detected, 0-undetected)
//!
void TabMenu::init( int x1, int y1, int x2, int y2,
		    int optionNum, int *selNum,
		    int shortLength, int longLength,
		    char **shortDesc, char **longDesc,
		    int (*dispFunc)(int, int), int (*detectFunc)(int) ) {
    err_when(optionNum <= 0 || shortLength <= 0 || longLength <= 0);

    tab_menu_x1 = x1;
    tab_menu_x2 = x2;
    tab_menu_y1 = y1 + TAB_HEIGHT;
    tab_menu_y2 = y2;

    option_num = optionNum;
    sel_num = selNum;
    short_view_length = shortLength;
    long_view_length = longLength;
    short_desc = shortDesc;
    long_desc = longDesc;
    disp_func = dispFunc;
    detect_func = detectFunc;

    tab_start = new short[optionNum];
    tab_end = new short[optionNum];

    init_flag = 1;
    back_img = vga_back.save_area(tab_menu_x1, tab_menu_y1-TAB_HEIGHT,
				  tab_menu_x2, tab_menu_y2,back_img);

    refresh(INFO_REPAINT);
}

//------------- End of function TabMenu::init -----------//

//-------- Begin of function TabMenu::deinit ------------//
//!
void TabMenu::deinit() {
    if(back_img) {
	mem_del(back_img);
	back_img=NULL;
    }

    delete [] tab_start;
    delete [] tab_end;
}

//------------- End of function TabMenu::deinit ---------//

//----------- Begin of function TabMenu::refresh --------//
//!
void TabMenu::refresh(int refreshFlag) {
    err_when(*sel_num > option_num || *sel_num < 0);

    //	mouse.hide_area( tab_menu_x1, tab_menu_y1-TAB_HEIGHT,
    //						  tab_menu_x2, tab_menu_y2 );

    if (refreshFlag == INFO_REPAINT) {
	vga_back.rest_area(back_img,0,0);
	user_interface.brighten(tab_menu_x1, tab_menu_y1+3, tab_menu_x2, tab_menu_y2);
	draw_tab();
	//		vga.blt_buf( tab_menu_x1, tab_menu_y1-TAB_HEIGHT,
	//						 tab_menu_x2, tab_menu_y2 ,0);
	label_tab();
    }

    disp_func(*sel_num, refreshFlag);
    sys.yield();
    //	mouse.show_area();
}

//------------- End of function TabMenu::refresh ---------//

//----------- Begin of function TabMenu::detect ------------//
//!
//! Return: 1-pressed on button, variable updated
//!			  0-not pressed on button
//!
int TabMenu::detect() {
    if (!init_flag)
	return 0;

    //---- If mouse inside tab menu area, call sub-detect function ----//
    //	if (mouse.in_area( tab_menu_x1, tab_menu_y1, tab_menu_x2, tab_menu_y2 ))
    //	{
    //		return detect_func(*sel_num);
    //	}

    //---- Detect tab click ----//

    if (mouse.single_click( tab_menu_x1, tab_menu_y1-TAB_HEIGHT,
			    tab_end[option_num-1]+TAB_BOUNDARY, tab_menu_y1 )) {
	for (int i = 0; i < option_num; i++) {
	    if(*sel_num == i)continue;
	    if (mouse.single_click( tab_start[i]-TAB_BOUNDARY, tab_menu_y1-TAB_HEIGHT,
				    tab_end[i]+TAB_BOUNDARY, tab_menu_y1 )) {
		*sel_num = i;
		refresh(INFO_REPAINT);
		audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
		return 1;
	    }
	}
    }

    //## begin chwg1004 ##//
    return detect_func(*sel_num);
    //## end chwg1004 ##//
    return 0;
}

//------------- End of function TabMenu::detect ------------//

//----------- Begin of function TabMenu::draw_tab ----------//
//!
void TabMenu::draw_tab() {
    int i;
    short x = tab_menu_x1 + TAB_BOUNDARY;

    for (i = 0; i < option_num; i++) {
	tab_start[i] = x;
	x += (*sel_num == i) ? long_view_length : short_view_length;
	tab_end[i] = x;
	x += 2 * TAB_BOUNDARY;
    }

    for (i = 0; i < option_num; i++) {
	if (*sel_num != i) {
	    user_interface.brighten
		(tab_start[i], tab_menu_y1-TAB_HEIGHT,
		 tab_end[i], tab_menu_y1+1);
	}
    }

    //	user_interface.brighten
    //		(tab_start[*sel_num], tab_menu_y1-TAB_HEIGHT,
    //								tab_end[*sel_num], tab_menu_y1+3);
    vga_back.d3_panel_up
	(tab_start[*sel_num], tab_menu_y1-TAB_HEIGHT,
	 tab_end[*sel_num], tab_menu_y1+3);
    user_interface.brighten
	( tab_menu_x1, tab_menu_y1+3,
	  tab_menu_x2, tab_menu_y2);
    user_interface.rect
	( tab_menu_x1, tab_menu_y1+3,
	  tab_menu_x2, tab_menu_y2);

}

//------------- End of function TabMenu::draw_tab ---------//

//----------- Begin of function TabMenu::label_tab --------//
//!
void TabMenu::label_tab() {
    for (int i = 0; i < option_num; i++) {
	font_charts.center_put(tab_start[i], tab_menu_y1-TAB_HEIGHT+2,
			       tab_end[i], tab_menu_y1+4,
			       (*sel_num == i) ? long_desc[i] : short_desc[i]);
    }
}

//------------- End of function TabMenu::label_tab --------//
