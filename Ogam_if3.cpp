//Filename    : OGAM_IF3.CPP
//Description : Game Class Definition (Hall of Fame)
//Owner       : Fred

#include <OSYS.H>
#include <OLOG.H>
#include <OMOUSE.H>
#include <OVGA.H>

/*
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>

#include <OGAMESET.H>
#include <OGSET.H>

#include <ORADIO.H>
#include <OTABMENU.H>
#include <OSLIDER.H>
#include <OVALSLDR.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <OSPINNER.H>
#include <OVBROWSE.H>
#include <OBNDSLDR.H>
#include <OBOX.H>
*/

enum {
    REPORT_X1 = ZOOM_X1 + 15,
    REPORT_Y1 = ZOOM_Y1 + 15,
    REPORT_X2 = ZOOM_X2 - 15,
    REPORT_Y2 = ZOOM_Y2 - 40
};

enum {
    TMENU_X1 = REPORT_X1 + 5,
    TMENU_Y1 = REPORT_Y1 + 35,
    TMENU_X2 = REPORT_X2 - 5,
    TMENU_Y2 = REPORT_Y2 - 5
};

enum { LEFT_PUSHED, RIGHT_PUSHED, ALL_POP };

enum { TAB_SCREEN,SAVE_EXPRESS};

static current_report_mode=TAB_SCREEN;
static button_state = ALL_POP;
static button_timing = 0;

static TabMenu tab_menu;
static int tab_sel;
static char *tab_short_desc[] = {
    "Express games",
    "Basic options",
    "Advanced options I",
    "Advanced options II",
};
static char *tab_long_desc[] = {
    "Express games",
    "Basic options",
    "Advanced options I",
    "Advanced options II",
};
static Button button_start,button_cancel;
static char * button_label[]={"Start game","Cancel"};

static Button button_left,button_right,button_add,button_remove,button_reset,button_addall;

static char init_flag;
static char redraw_browser_flag;
static int pic_id;

// Screen Express Game
static VBrowse scr1_vbrowser1;
static VBrowse scr1_vbrowser2;
static Button scr1_button_load;
static Button scr1_button_save;
static Button scr1_button_del;
static int scr1_browseRecCount1=1;
static int scr1_browseRecCount2=1;
static int scr1_browseRecHeight=20;
static void scr1_disp_rep_row1(int,int,int,int);
static void scr1_disp_rep_row2(int,int,int,int);

// Screen Save Express Game
static Button scr5_button_save;
static Button scr5_button_cancel;
static GetA scr5_game_name;

// Screen Basic Option
static VBrowse scr2_vbrowser1;
static VBrowse scr2_vbrowser2;
static int scr2_browseRecCount1=1;
static int scr2_browseRecCount2=1;
static int scr2_browseRecHeight=20;
static void scr2_disp_rep_row1(int,int,int,int);
static void scr2_disp_rep_row2(int,int,int,int);
// ###### begin Gilbert 19/04/2001 ###### //
static short* button_refresh_bitmap = NULL;
// ###### end Gilbert 19/04/2001 ###### //
static GetAGroup scr2_name(3);
static int tmp_department_pool_flag[DEPARTMENT_TYPE_COUNT];
static int right_bws_pool[DEPARTMENT_TYPE_COUNT];
static int left_bws_pool[DEPARTMENT_TYPE_COUNT];
static int right_bws_pool_size;
static int left_bws_pool_size;
static int pool_selected_size;
static int just_add=0,just_remove=0;
static int player_school_prestige;
static int player_school_doctoral_program_intensity;
static int player_school_sponsored_research_intensity;
//static int dummy=5000;

// Screen Custom Option 1
static RadioButton scr3_rbutton1;
static RadioButton scr3_rbutton2;
static ValueSlider scr3_slider1;
static ValueSlider scr3_slider2;
static ValueSlider scr3_slider3;
static ValueSlider scr3_slider4;
static ValueSlider scr3_slider5;
static ValueSlider scr3_bounded_slider0;
static ValueSlider scr3_bounded_slider1;
static ValueSlider scr3_bounded_slider2;
//static BoundedSlider scr3_bounded_slider1;
static char* scr3_label[]= {
    "Student marketing area",
    //									"Location",
    "Prestige",
    "Number of on-campus students",
    "Number of distance learning students",
    "Percent of undergrads that are part-time",
    "Percent of undergrads getting degree in five years",
    "Percent of full-time undergrads in student housing",
    "Athletics program intensity"
};
static char* scr3_rbutton1_options[]={"National","Regional","Local"};
static char* scr3_rbutton2_options[]= {
    "Low","Normal",
    "High"
};

// Screen Custom Option 2
static RadioButton scr4_rbutton1;
static RadioButton scr4_rbutton2;
static RadioButton scr4_rbutton3;
static ValueSlider scr4_slider1;
static ValueSlider scr4_slider2;
static ValueSlider scr4_slider3;
static ValueSlider scr4_slider4;
static char* scr4_label[]= {
    "Percent of graduate students",
    "Percent of nondegree students",
    "Doctoral program intensity",
    "Sponsored research intensity",
    "Relative wealth",
    "Institutional control",
    "Campus environment",
};
static char* scr4_rbutton1_options[]={"Rich","In between","Struggling"};
static char* scr4_rbutton2_options[]={"Private","Public"};
static char* scr4_rbutton3_options[]={"Urban","Suburban","Rural"};

Game::scenario_deinit() {
    if(button_refresh_bitmap) {
	mem_del(button_refresh_bitmap);
	button_refresh_bitmap=NULL;
    }
    scr3_rbutton1.deinit();
    scr3_rbutton2.deinit();
    scr3_slider1.deinit();
    scr3_slider2.deinit();
    scr3_slider3.deinit();
    scr3_slider4.deinit();
    scr3_slider5.deinit();
    scr3_bounded_slider1.deinit();
    scr3_bounded_slider2.deinit();

    scr4_rbutton1.deinit();
    scr4_rbutton2.deinit();
    scr4_rbutton3.deinit();
    scr4_slider1.deinit();
    scr4_slider3.deinit();
    scr4_slider4.deinit();

    //		for(int i=0;i<department_res.department_count;i++)
    //		{
    //			tmp_department_pool_flag[i]=(i<MIN_DEPARTMENT)?1:0;
    //			redraw_browser_flag=1;
    //		}

    scr2_browseRecCount1=1;
    scr2_browseRecCount2=1;

}

static void save_express_report(int refreshFlag) {
    int vtab1=TMENU_Y1+15,vtab2=TMENU_Y1+28,vtab3=TMENU_Y1+230,vtab4=TMENU_Y1+265;
    int tab1=TMENU_X1+40,tab2=TMENU_X1+90,tab3=TMENU_X1+600,tab4=TMENU_X1+650;
    if(refreshFlag==INFO_REPAINT) {
	user_interface.darken(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);
	info.disp_column_bitmap(&vga_back);
	user_interface.brighten(tab1,vtab1,tab4,vtab4);
	user_interface.rect(tab1,vtab1,tab4,vtab4);
	scr5_button_save.create_text
	    (tab2+70,vtab3,tab2+170,vtab3+20,"Save");
	//		scr5_button_save.set_key('S');
	scr5_button_cancel.create_text
	    (tab2+370,vtab3,tab2+470,vtab3+20,"Cancel");
	//		scr5_button_cancel.set_key('C');

	font_chartsm.put(tab1+50,vtab2,"Express game name");

	user_interface.bar(tab1+205,vtab2-4,tab1+600,vtab2+18,V_WHITE);
	user_interface.rect(tab1+205,vtab2-4,tab1+600,vtab2+18,2);
	scr5_game_name.init(tab1+215,vtab2,tab1+600,player_school.school_name,sizeof(player_school.school_name)-1,&font_chartsm,0,1);
	sys.redraw_all_flag=0;
    }
    user_interface.bar(tab1+205,vtab2-4,tab1+600,vtab2+18,V_WHITE);
    user_interface.rect(tab1+205,vtab2-4,tab1+600,vtab2+18);
    scr5_game_name.paint();

    scr5_button_save.paint();
    scr5_button_cancel.paint();
}

static int save_express_detect() {
    vga.use_back();
    scr5_game_name.detect();

    vga.use_front();                                // ## use_front to make the button flashing in release mode.
    if(scr5_button_save.detect()) {
	return 1;
    }
    if(scr5_button_cancel.detect()) {
	current_report_mode=TAB_SCREEN;
	sys.redraw_all_flag=1;
	return 1;
    }
    vga.use_back();
    return 0;
}

static int callback_disp(int selNum, int refreshFlag) {
    static int scr4_v1=1,scr4_v2=2,scr4_v3=3,scr4_v4=4,scr4_v5=0,scr4_v6=2,scr4_v7=1;
    static int scr3_v1=1,scr3_v2=2,scr3_v3=3,scr3_v4=4,scr3_v5=0,scr3_v6=2,scr3_v7=1,scr3_v8=1;

    if(selNum==0) {
	//---init. of screen 1 -//
	if(refreshFlag== INFO_REPAINT) {
	    deinit();
	    //			vga_back.d3_panel_down(TMENU_X1,TMENU_Y1+5,TMENU_X2,TMENU_Y2-35, 1);
	    //			user_interface.rect(TMENU_X1,TMENU_Y1+5,TMENU_X2,TMENU_Y2-35, 1);

	    scr1_vbrowser1.init(TMENU_X1+5,TMENU_Y1+10,TMENU_X2-5,TMENU_Y1+140,
				0,  scr1_browseRecHeight,scr1_browseRecCount1,
				scr1_disp_rep_row1,0);
	    scr1_vbrowser2.init(TMENU_X1+5,TMENU_Y2-175,TMENU_X2-5,TMENU_Y2-40,
				0,  scr1_browseRecHeight, scr1_browseRecCount2,
				scr1_disp_rep_row2,0);

	    //--------- create buttons ---------//

	    scr1_button_load.create_text
		( TMENU_X1+50,TMENU_Y2-25,TMENU_X1+200,TMENU_Y2-5,
		  "Load express game");

	    scr1_button_save.create_text
		( TMENU_X1+250,TMENU_Y2-25,TMENU_X1+400,TMENU_Y2-5,
		  "Save express game");

	    scr1_button_del.create_text
		( TMENU_X2-200,TMENU_Y2-25,TMENU_X2-50,TMENU_Y2-5,
		  "Delete express game");

	    //----------------------------------//

	    scr1_vbrowser1.open(scr1_vbrowser1.recno());
	    scr1_vbrowser2.open(scr1_vbrowser2.recno());
	}
	scr1_vbrowser1.update();
	scr1_vbrowser2.update();
	scr1_button_load.paint();
	scr1_button_save.paint();
	scr1_button_del.paint();
    }

    ///////////////////////////////////////////////////////////////////////////////////

    if(selNum==1) {
	int vtab1=TMENU_Y1+15,vtab2=TMENU_Y1+60,vtab3=TMENU_Y1+120,vtab4=TMENU_Y1+320;
	int vtab5=TMENU_Y1+280,vtab6=TMENU_Y1+220,vtab7=TMENU_Y1+255,vtab8=TMENU_Y1+290;
	int tab1=TMENU_X1+15,tab2=TMENU_X1+90,tab3=TMENU_X1+390,tab4=TMENU_X1+680;

	//		user_interface.rect(tab1-1,vtab1-1,tab1+60,vtab1+60,1);

	if(refreshFlag== INFO_REPAINT) {
	    //---init. of screen 2 -//
	    deinit();
	    //			vga_back.d3_panel_down(TMENU_X1,TMENU_Y1+5,TMENU_X2,TMENU_Y2-15, 1);

	    button_refresh_bitmap=
		vga_back.save_area(tab1,vtab1+65,tab1+60,vtab1+95);
	    //			user_interface.rect(TMENU_X1,TMENU_Y1+5,TMENU_X2,TMENU_Y2-15,1);
	    button_left.create_text(tab1,vtab1+70,tab1+25,vtab1+90,"<");
	    button_right.create_text(tab1+35,vtab1+70,tab1+60,vtab1+90,">");

	    button_add.create_text(tab1+285,vtab3+60,tab1+385,vtab3+80,"Add >>");
	    button_remove.create_text(tab1+285,vtab3+100,tab1+385,vtab3+120,"<< Remove ");
	    button_reset.create_text(tab1+285,vtab3+140,tab1+385,vtab3+160,"Reset");
	    //			button_addall.create_text(tab1+285,vtab3+140,tab1+385,vtab3+160,"Add to max");

	    font_blue_san.put(tab2,vtab1,"Institution name");
	    font_blue_san.put_paragraph(tab2,vtab1+55,tab2+80,vtab1+100,"Player first name");
	    font_blue_san.put(tab3,vtab1+65,"Last name");
	    font_charts.put(tab1+40,vtab3,"Nonactive department");
	    font_charts.put(tab1+460,vtab3,"Active department");

	    // Basic options --//
	    scr2_name[0].init(tab2+15,vtab1+25,tab2+600,player_school.school_name,sizeof(player_school.school_name)-1,&font_chartsm,0,1);
	    scr2_name[1].init(tab2+80,vtab1+65,tab2+300,player_school.player_first_name,sizeof(player_school.player_first_name)-1,&font_chartsm,0,1);
	    scr2_name[2].init(tab3+80,vtab1+65,tab3+300,player_school.player_last_name,sizeof(player_school.player_last_name)-1,&font_chartsm,0,1);
	    scr2_name.set_focus(0);
	    scr2_name[0].paint();
	    scr2_name[1].paint();
	    scr2_name[2].paint();
	}

	if(redraw_browser_flag || refreshFlag==INFO_REPAINT) {
	    right_bws_pool_size=0;
	    left_bws_pool_size=0;

	    for(int i=0,l=0,r=0;i<department_res.department_count;i++) {
		if(tmp_department_pool_flag[i]==0) {
		    left_bws_pool[l]=i+1;
		    left_bws_pool_size++;
		    l++;
		    if(just_remove==(i+1)) {
			scr2_browseRecCount2=l;
			just_remove=0;
		    }
		}
		else {
		    right_bws_pool[r]=i+1;
		    right_bws_pool_size++;
		    r++;
		    if(just_add==(i+1)) {
			scr2_browseRecCount1=r;
			just_add=0;
		    }
		}
	    }

	    scr2_vbrowser1.init(tab1+400,vtab3+30,tab1+670,vtab4,
				0,  scr2_browseRecHeight,right_bws_pool_size,
				scr2_disp_rep_row1,1);
	    scr2_vbrowser2.init(tab1,vtab3+30,tab1+270,vtab4,
				0,  scr2_browseRecHeight,left_bws_pool_size,
				scr2_disp_rep_row2,1);

	    scr2_vbrowser1.open(scr2_browseRecCount1);
	    scr2_vbrowser2.open(scr2_browseRecCount2);
	    redraw_browser_flag=0;
	}
	//////////////////////////////////////////////////////////////////////

	scr2_browseRecCount1=scr2_vbrowser1.recno();
	scr2_browseRecCount2=scr2_vbrowser2.recno();

	// update screen//
	image_mascot.put_back(tab1,vtab1,player_school.mascot);

	image_interface.put_back(tab1,vtab1+70,"L-ARW-UP");
	image_interface.put_back(tab1+35,vtab1+70,"R-ARW-UP");

	user_interface.bar(tab2+10,vtab1+20,tab2+590,vtab1+42,V_WHITE);
	user_interface.bar(tab2+75,vtab1+60,tab2+280,vtab1+82,V_WHITE);
	user_interface.bar(tab3+75,vtab1+60,tab3+280,vtab1+82,V_WHITE);
	user_interface.rect(tab2+10,vtab1+20,tab2+590,vtab1+42,2);
	user_interface.rect(tab2+75,vtab1+60,tab2+280,vtab1+82,2);
	user_interface.rect(tab3+75,vtab1+60,tab3+280,vtab1+82,2);

	button_add.paint();
	button_remove.paint();
	button_reset.paint();
	//		button_addall.paint();
	button_add.pop();
	button_remove.pop();
	button_reset.pop();

	scr2_name.paint();
    }

    ///////////////////////////////////////////////////////////////////////////////

    if(selNum==2) {
	int vtab1=20,vtab2=55,vtab3=90,vtab4=125;
	int vtab5=185,vtab6=220,vtab7=255,vtab8=290;
	int tab1=25,tab2=300,tab3=400,tab4=680;

	//---init. of screen 3 -//
	if(refreshFlag== INFO_REPAINT) {
	    deinit();
	    user_interface.rect(TMENU_X1+5,TMENU_Y1+5,TMENU_X2-5,TMENU_Y1+vtab5-25, 1);
	    user_interface.rect(TMENU_X1+5,TMENU_Y1+vtab5-15,TMENU_X2-5,TMENU_Y2-5, 1);

	    scr3_rbutton1.init(TMENU_X1+tab2,TMENU_Y1+vtab1,TMENU_X2,TMENU_Y1+vtab1+20,
			       3, &(player_school.student_market),
			       scr3_rbutton1_options);

	    scr3_rbutton2.init(TMENU_X1+tab3,TMENU_Y1+vtab8,TMENU_X2, TMENU_Y1+vtab8+20,
			       3, &(player_school.athletic_program_intensity),
			       scr3_rbutton2_options);

	    scr3_bounded_slider0.init(TMENU_X1+tab2,TMENU_Y1+vtab2,380,17,
				      0,10000,&(player_school_prestige),0,V_YELLOW,9);

	    scr3_bounded_slider1.init
		(TMENU_X1+tab2,TMENU_Y1+vtab3, 380, 17,
		 2000,20000,&(player_school.student_count),0, V_YELLOW,4);

	    scr3_bounded_slider2.init
		(TMENU_X1+tab2,TMENU_Y1+vtab4, 380, 17,
		 //## fred 0118
		 0,100,&(player_school.sl5_student_percent),0, V_YELLOW,4);

	    scr3_slider2.init(TMENU_X1+tab3,TMENU_Y1+vtab5, 200, 17,
			      0,100,&(player_school.parttime_undergrad_percent),0, V_YELLOW,3);

	    scr3_slider3.init(TMENU_X1+tab3,TMENU_Y1+vtab6, 200, 17,
			      0,100,&(player_school.ug_get_degree_in_5year_percent),0, V_YELLOW,3);

	    scr3_slider4.init(TMENU_X1+tab3,TMENU_Y1+vtab7, 200, 17,
			      0,100,&(player_school.fulltime_ug_in_housing_percent),0, V_YELLOW,3);
	}

	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab1+5,
			  scr3_label[0]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab2+5,
			  scr3_label[1]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab3+5,
			  scr3_label[2]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab4+5,
			  scr3_label[3]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab5+5,
			  scr3_label[4]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab6+5,
			  scr3_label[5]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab7+5,
			  scr3_label[6]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab8+5,scr3_label[7]);

	scr3_rbutton1.refresh();
	scr3_rbutton2.refresh();
    }

    if(selNum==3) {
	int vtab1=25,vtab2=30,vtab3=65,vtab4=100;
	int vtab5=175,vtab6=215,vtab7=255;
	int tab1=25,tab2=200,tab3=320,tab4=680;

	//--- initialize screen 4 -----//

	if(refreshFlag==INFO_REPAINT) {
	    deinit();
	    user_interface.rect(TMENU_X1+5,TMENU_Y1+5,TMENU_X2-5,TMENU_Y1+vtab4+40, 1);
	    user_interface.rect(TMENU_X1+5,TMENU_Y1+vtab5-15,TMENU_X2-5,TMENU_Y2-30, 1);

	    scr4_slider1.init(TMENU_X1+tab3,TMENU_Y1+vtab2, 300,17,
			      0,100,&(player_school.graduate_student_percent),0, V_YELLOW,3);

	    scr4_slider3.init(TMENU_X1+tab3,TMENU_Y1+vtab3, 300, 17,
			      0,10000,&(player_school_doctoral_program_intensity),0, V_YELLOW,9);

	    scr4_slider4.init(TMENU_X1+tab3,TMENU_Y1+vtab4, 300, 17,
			      0,10000,&(player_school_sponsored_research_intensity),0, V_YELLOW,9);

	    scr4_rbutton1.init(TMENU_X1+tab2,TMENU_Y1+vtab5,TMENU_X2,TMENU_Y1+vtab5+20,
			       3, &(player_school.relative_wealth), scr4_rbutton1_options);

	    scr4_rbutton2.init(TMENU_X1+tab2 ,TMENU_Y1+vtab6,TMENU_X2-170,TMENU_Y1+vtab6+20,
			       2, &(player_school.control), scr4_rbutton2_options);

	    scr4_rbutton3.init(TMENU_X1+tab2,TMENU_Y1+vtab7,TMENU_X2,TMENU_Y1+vtab7+20,
			       3, &(player_school.campus_environment), scr4_rbutton3_options);
	}

	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab2+5,
			  scr4_label[0]);
	//		font_chartsm.put(TMENU_X1+tab1,TMENU_Y1+vtab2+5,
	//									scr4_label[1]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab3+5,
			  scr4_label[2]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab4+5,
			  scr4_label[3]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab5+5,
			  scr4_label[4]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab6+5,
			  scr4_label[5]);
	font_blue_san.put(TMENU_X1+tab1,TMENU_Y1+vtab7+5,
			  scr4_label[6]);

	scr4_rbutton1.refresh();
	scr4_rbutton2.refresh();
	scr4_rbutton3.refresh();
    }

    //	vga.blt_buf(TMENU_X1+5,TMENU_Y1+5,TMENU_X2-5,TMENU_Y2-5,0);
    return 0;
}

static int callback_detect(int selNum) {
    //--- if no game is selected currently ---//

    if(selNum==0) {
	scr1_vbrowser1.detect();
	scr1_vbrowser2.detect();
	vga.use_front();

	scr1_button_load.detect();

	if(scr1_button_save.detect()) {
	    current_report_mode=SAVE_EXPRESS;
	    sys.redraw_all_flag=1;
	}

	scr1_button_del.detect();

	vga.use_back();
    }

    //----- if a game is selected now ----//

    if(selNum==1) {
	int tab1=TMENU_X1+15,vtab1=TMENU_Y1+15;
	vga.use_back();

	if(button_left.detect()) {
	    do {
		player_school.mascot--;
		if(player_school.mascot<1)
		    player_school.mascot=image_mascot.rec_count;
	    } while( player_school.is_hidden_mascot(player_school.mascot) );
	    vga_back.rest_area(button_refresh_bitmap,0,0);
	    image_interface.put_front(tab1,vtab1+70,"L-ARW-DN");
	}

	if(button_right.detect()) {
	    do {
		player_school.mascot++;
		if(player_school.mascot>image_mascot.rec_count)
		    player_school.mascot=1;
	    } while( player_school.is_hidden_mascot(player_school.mascot) );
	    vga_back.rest_area(button_refresh_bitmap,0,0);
	    image_interface.put_front(tab1+35,vtab1+70,"R-ARW-DN");
	}

	scr2_name.detect();

	vga.use_front();                              // ## use_front to make the button flashing in release mode.

	if(button_add.detect()) {
	    if(scr2_vbrowser2.recno()>0) {
		for(int i=0;i<DEPARTMENT_TYPE_COUNT;i++)
		    if(player_school.selected_department_list[i]==0) {
			just_add=left_bws_pool[scr2_vbrowser2.recno()-1];
			player_school.selected_department_list[i]=just_add;
			tmp_department_pool_flag[just_add-1]=1;
			break;
		    }
		redraw_browser_flag=1;
	    }
	}

	if(button_remove.detect()) {

	    if(scr2_vbrowser1.recno()>0) {
		int i;
		for(i=0;i<DEPARTMENT_TYPE_COUNT;i++)
		    if(player_school.selected_department_list[i]==right_bws_pool[scr2_vbrowser1.recno()-1]) {
			just_remove=right_bws_pool[scr2_vbrowser1.recno()-1];
			player_school.selected_department_list[i]=0;
			tmp_department_pool_flag[just_remove-1]=0;
			break;
		    }
		// Move the last nonzero entry to fill removed entry.
		for(int j=DEPARTMENT_TYPE_COUNT-1;j>i;j--) {
		    if(player_school.selected_department_list[j]!=0) {
			player_school.selected_department_list[i]=
			    player_school.selected_department_list[j];
			player_school.selected_department_list[j]=0;
			break;
		    }
		}
		//				sys.redraw_all_flag=1;
		redraw_browser_flag=1;
	    }
	}

	if(button_reset.detect()) {
	    for(int i=0;i<department_res.department_count;i++) {
		tmp_department_pool_flag[i]=(i<MIN_DEPARTMENT)?1:0;
		redraw_browser_flag=1;
	    }

	    for(i=0;i<DEPARTMENT_TYPE_COUNT;i++)
		player_school.selected_department_list[i]=(i<MIN_DEPARTMENT)?(i+1):0;

	    scr2_browseRecCount1=1;
	    scr2_browseRecCount2=1;
	}

	vga.use_back();
	if(scr2_vbrowser1.detect()) {
	};
	if(scr2_vbrowser2.detect()) {
	};
    }

    if(selNum==2) {
	scr3_rbutton1.detect();
	scr3_rbutton2.detect();
	//		scr3_bounded_slider0.detect();
	if(scr3_bounded_slider0.detect())
	    player_school.prestige=player_school_prestige/1000;
	scr3_slider2.detect();
	scr3_slider3.detect();
	scr3_slider4.detect();
	scr3_slider5.detect();
	scr3_bounded_slider1.detect();
	scr3_bounded_slider2.detect();
    }

    if(selNum==3) {
	scr4_rbutton1.detect();
	scr4_rbutton2.detect();
	scr4_rbutton3.detect();
	scr4_slider1.detect();

	if(scr4_slider3.detect())
	    player_school.doctoral_program_intensity=player_school_doctoral_program_intensity/1000;

	if(scr4_slider4.detect())
	    player_school.sponsored_research_intensity=player_school_sponsored_research_intensity/1000;
    }
    return 0;
}

GameSetting::~GameSetting() {
    deinit();
}

static void scr1_disp_rep_row1(int recNo, int x, int y, int refreshFlag) {
}

static void scr1_disp_rep_row2(int recNo, int x, int y, int refreshFlag) {
}

static void scr2_disp_rep_row1(int recNo, int x, int y, int refreshFlag) {
    user_interface.bar(x, y, x+240, y+16,Vga::active_buf->color_down);
    for(int i=0,j=0;(i<recNo)&&(j<DEPARTMENT_TYPE_COUNT);j++)
	if(tmp_department_pool_flag[j]==1)
	    i++;
    //	err.run("department_res.department_count=%d\n",department_res.department_count);
    if(j<=department_res.department_count)
	font_chartsm.put(x,y,department_res[j]->name);
}

static void scr2_disp_rep_row2(int recNo, int x, int y, int refreshFlag) {
    user_interface.bar(x, y, x+240, y+16,Vga::active_buf->color_down);
    for(int i=0,j=0;(i<recNo)&&(j<DEPARTMENT_TYPE_COUNT);j++)
	if(tmp_department_pool_flag[j]==0)
	    i++;
    //	DEBUG_LOG("j=");
    //	DEBUG_LOG(j);
    //	DEBUG_LOG("\n");
    if(j<=department_res.department_count)
	font_chartsm.put(x,y,department_res[j]->name);
}

/*
static ValueSlider value_slider;
static int value_slider_var;

//------- dummy----------//
//static BoundedSlider bounded_slider;
//static double bounded_slider_var, bounded_slider_lo, bounded_slider_up;

static char *legend[] = {
  "Legend 1", "Legend 2", "Legend 3", "Legend 4"
};
static int graph_a_start, graph_a_end, graph_data_num, graph_data_num1;
static int graph_data[] = {
24, 37, 50, 18, 40, 50, 90, 20, 18, 29, 94, 19, 2, 6, 13
};

static char *x_lab = "X-axis label";
static char *y_lab = "Y-axis label";

static Spinner spinner, spinner2;
static float spinner_val, spinner_val2;

static RadioButton radio_button;
static int radio_sel;
static char *radio_desc[] = {
"Selection 1", "Selection 2", "Selection 3"
};
//static SliderGroup slider_group(3);
//static int slider_var[3];

*/

//-----------------------------------------------------------------//

//------- End of function Game::scenario_init ------//
//
void Game::scenario_constructor() {
    tab_sel = 1;                                    // default to Basic Options
    init_flag = 0;
    redraw_browser_flag = 0;
    scr1_browseRecCount1=1;
    scr1_browseRecCount2=1;
    pic_id = 9;
}

//------- End of function Game::scenario_init ------//

//------- End of function Game::scenario_init ------//
//
void Game::scenario_init() {
    /* reset browsers */
    /*	for(int i=0;i<DEPARTMENT_TYPE_COUNT;i++)
	player_school.selected_department_list[i]=(i<MIN_DEPARTMENT)?(i+1):0;
	for(i=0;i<DEPARTMENT_TYPE_COUNT;i++)
	tmp_department_pool_flag[i]=(i<MIN_DEPARTMENT)?1:0;
    */
}

//------- End of function Game::scenario_report ------//

//------- End of function Game::scenario_report ------//
//
void Game::scenario_report(int refreshFlag) {
    int vtab1=50,vtab2=150,vtab3=160,vtab4=250;
    int tab1=10,tab2=200,tab3=400,tab4=680;

    vga.use_back();

    if(current_report_mode==TAB_SCREEN) {
	if (refreshFlag == INFO_REPAINT) {
	    user_interface.bg_img(pic_id, &vga_back);
	    tab_menu.init(REPORT_X1,REPORT_Y1,REPORT_X2,REPORT_Y2,
			  4, &tab_sel, 150, 150,
			  tab_short_desc, tab_long_desc,
			  callback_disp, callback_detect);
	    if (!init_flag) {
		button_start.create_text
		    (REPORT_X1+150, REPORT_Y2+15,
		     REPORT_X1+300, REPORT_Y2+35, button_label[0]);
		button_cancel.create_text
		    (REPORT_X1+400, REPORT_Y2+15,
		     REPORT_X1+550, REPORT_Y2+35, button_label[1]);
		init_flag = 1;
	    }
	    else {

	    }
	    //			if (++pic_id == 6) pic_id = 1;
	    sys.redraw_all_flag=0;
	}
	else {
	    tab_menu.refresh(refreshFlag);
	}

	button_start.paint();
	button_cancel.paint();
    }

    if(current_report_mode==SAVE_EXPRESS) {
	save_express_report(refreshFlag);
    }
    vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
}

//------- End of function GameSetting::report ------//

//------ Begin of function GameSetting::detect ------//
//!
//! return: <int> 1  - the Start button is pressed.
//! 				  -1 - the Cancel button is pressed.
//!					  0  - neither of them is pressed.
//!
int Game::scenario_detect() {
    int rc=0;
    vga.use_back();
    if(current_report_mode==SAVE_EXPRESS) {
	save_express_detect();
    }
    if(current_report_mode==TAB_SCREEN) {
	tab_menu.detect();
	vga.use_front();

	if( button_start.detect() ) {
	    if(right_bws_pool_size>MAX_DEPARTMENT) {
		String msg_text;
		msg_text = "The University must have \nat least ";
		msg_text += m.format(MAX_DEPARTMENT,4);
		msg_text += " but not more \nthan ";
		msg_text += m.format(MIN_DEPARTMENT,4);
		msg_text += " departments.";
		box.msg(msg_text.str_buf);
	    }
	    else
		rc = 1;
	}
	if( button_cancel.detect() )
	    rc = -1;
    }
    vga.use_back();

    return rc;
}

//------- End of function GameSetting::detect ------//
