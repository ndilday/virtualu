#include <OSYS.H>
#include <KEY.H>
#include <OLOG.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OGSET.H>
#include <ORADIO.H>
#include <ORESTXT.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OFINANCE.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OTABMENU.H>
#include <OSLIDER.H>
#include <OVALSLDR.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <OSPINNER.H>
#include <OTEXT.H>
#include <OVBROW98.H>
#include <OBNDSLDR.H>
#include <OPSCHOOL.H>
#include <ODEPTRES.H>
#include <OBOX.H>
#include <OAUDIO.H>
#include <OCONFIG.H>
#include <OGFILE.H>
#include <OVGALOCK.H>
#include <STDIO.H>

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

enum {
    TAB_SCREEN,
    SAVE_EXPRESS,
    CUSTOMIZE_SCORE,                                // mode for changing weight of goal
};

static int current_report_mode=TAB_SCREEN;
static int button_state = ALL_POP;
static int button_timing = 0;
static int selected_scenario=PRIVATE_SCHOOL;

static TabMenu tab_menu;
static int tab_sel;
static char *tab_short_desc[] = {
    "Basic options",
    "Advanced options I",
    "Advanced options II",
    "Express games",
};
static char *tab_long_desc[] = {
    "Basic options",
    "Advanced options I",
    "Advanced options II",
    "Express games",
};

static char *field_name[] = {
    "",
    "Applied Science",
    "Business",
    "Creative Arts",
    "Engineering",
    "Humanities",
    "Language Studies",
    "Math and Computer Science",
    "Natural Science",
    "Social Science",
};

static Button button_start,button_cancel;
static Button button_customize_score;
static Button button_randomize_setting;
static Button scr_button_start,scr_button_cancel;
static char * button_label[]={"Start scenario","Cancel"};

static Button button_add,button_remove,button_reset,button_addall;

static char init_flag;
static char redraw_browser_flag;
static int pic_id;
// ###### begin Gilbert 19/04/2001 ###### //
static short *scr2_vbrowser1_bitmap = NULL;
static short *scr2_vbrowser2_bitmap = NULL;
// ###### end Gilbert 19/04/2001 ###### //

// Screen Express Game
static VBrowse98 scr1_vbrowser1;
static VBrowse98 scr1_vbrowser2;
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
static VBrowse98 scr2_vbrowser1;
static VBrowse98 scr2_vbrowser2;
static int scr2_browseRecCount1=1;
static int scr2_browseRecCount2=1;
static int scr2_browseRecHeight=20;
static void scr2_disp_rep_row1(int,int,int,int);
static void scr2_disp_rep_row2(int,int,int,int);

//static short* button_refresh_bitmap;

static GetAGroup scr2_name(3);
//static int player_school.tmp_department_pool_flag[DEPARTMENT_TYPE_COUNT];
//static int player_school.right_bws_pool[DEPARTMENT_TYPE_COUNT];
//static int player_school.left_bws_pool[DEPARTMENT_TYPE_COUNT];
//static int player_school.right_bws_pool_size;
//static int player_school.left_bws_pool_size;
//static int player_school.pool_selected_size;
//static int player_school.just_add=0,player_school.just_remove=0;
static int player_school_prestige;
static int player_school_doctoral_program_intensity;
static int player_school_sponsored_research_intensity;

//static int dummy=5000;

// Screen Custom Option 1

static RadioButton scr3_rbutton1;
static RadioButton scr3_rbutton2;
static RadioButton scr3_rbutton3;

static RadioButton scr3_rbutton4;
static RadioButton scr3_rbutton5;
static RadioButton scr3_rbutton6;
static RadioButton scr3_rbutton7;
static RadioButton scr3_rbutton8;
static RadioButton scr3_rbutton9;

static char* scr3_label[]= {
    "Institutional control",
    "Student headcount",
    "Percentage of part-time students",
    "Sponsored research intensity",
    "Percentage of graduate students",
    "Percentage of undergrads getting degree in five years",
    "Percentage of undergrads in student housing",
    "Athletics program intensity"
};

static char* scr3_rbutton1_options[]={"Private","Public"};
static char* scr3_rbutton2_options[]={"Small","Medium","High"};
static char* scr3_rbutton3_options[]={"None","Medium","High"};
static char* scr3_rbutton4_options[]={"None","Medium","High"};
static char* scr3_rbutton5_options[]={"Low","Medium","High"};
static char* scr3_rbutton6_options[]={"Low","Medium","High"};
static char* scr3_rbutton7_options[]={"Low","Medium","High"};
static char* scr3_rbutton8_options[]={"Division III","Division II","Division I",};


// Screen Custom Option 2
static RadioButton scr4_rbutton1;
static RadioButton scr4_rbutton2;
static RadioButton scr4_rbutton3;
static RadioButton scr4_rbutton4;
static RadioButton scr4_rbutton5;

static RadioButton scr4_rbutton6;
static RadioButton scr4_rbutton7;
static RadioButton scr4_rbutton8;
static RadioButton scr4_rbutton9;
static char* scr4_label[]= {
    "Prestige",
    "Relative wealth",
    "Campus environment",
    "Random events",
    "Random objectives",
    "Random perturbations during play",
    "Behavioral response speed",
    "Auto-save every year",
    "Log key outputs to file",
};
static char* scr4_rbutton1_options[]={"Low","Medium","High"};
static char* scr4_rbutton2_options[]={"Rich","In between","Struggling"};
static char* scr4_rbutton3_options[]={"Urban","Suburban","Rural"};
static char* scr4_rbutton4_options[]={"None","Occasional","Frequent"};
static char* scr4_rbutton5_options[]={"None","Occasional","Frequent"};
static char* scr4_rbutton6_options[]={"None","Normal"};
static char* scr4_rbutton7_options[]={"Normal","Faster","Light-speed"};
static char* scr4_rbutton8_options[]={"No","Yes"};
static char* scr4_rbutton9_options[]={"No","Year","Trimester"};


static char just_some_items_detected_flag=0;

static void new_game_deinit() {
    if(scr2_vbrowser1_bitmap) {
	mem_del(scr2_vbrowser1_bitmap);
	scr2_vbrowser1_bitmap=NULL;
    }
    if(scr2_vbrowser2_bitmap) {
	mem_del(scr2_vbrowser2_bitmap);
	scr2_vbrowser2_bitmap=NULL;
    }

    scr3_rbutton1.deinit();
    scr3_rbutton2.deinit();
    scr3_rbutton3.deinit();
    scr3_rbutton4.deinit();
    scr3_rbutton5.deinit();
    scr3_rbutton6.deinit();
    scr3_rbutton7.deinit();
    scr3_rbutton8.deinit();

    scr4_rbutton1.deinit();
    scr4_rbutton2.deinit();
    scr4_rbutton3.deinit();
    scr4_rbutton4.deinit();
    scr4_rbutton5.deinit();
    scr4_rbutton6.deinit();
    scr4_rbutton7.deinit();
    scr4_rbutton8.deinit();
    scr4_rbutton9.deinit();

    scr2_browseRecCount1=1;
    scr2_browseRecCount2=1;
}

static String save_express_name;
static void save_express_report(int refreshFlag) {
    int vtab1=TMENU_Y1+95,vtab2=TMENU_Y1+108,vtab3=TMENU_Y1+150,vtab4=TMENU_Y1+185;
    int tab1=TMENU_X1+40,tab2=TMENU_X1+90,tab3=TMENU_X1+600,tab4=TMENU_X1+650;

    if(refreshFlag==INFO_REPAINT) {
	user_interface.darken(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);
	info.disp_column_bitmap(&vga_back);
	user_interface.brighten(tab1,vtab1,tab4,vtab4);
	user_interface.rect(tab1,vtab1,tab4,vtab4);
	scr5_button_save.create_text
	    (tab2+70,vtab3+5,tab2+170,vtab3+25,"Save");
	//		scr5_button_save.set_key('S');
	scr5_button_cancel.create_text
	    (tab2+360,vtab3+5,tab2+460,vtab3+25,"Cancel");
	//		scr5_button_cancel.set_key('C');

	font_chartsm.put(tab1+10,vtab2-3,"Express game name");

	user_interface.bar(tab1+10,vtab2+15,tab1+600,vtab2+40,V_WHITE);
	user_interface.rect(tab1+10,vtab2+15,tab1+600,vtab2+40,1);
	strcpy(save_express_name.str_buf,player_school.school_name);
	scr5_game_name.init(tab1+20,vtab2+21,tab1+600,save_express_name.str_buf,sizeof(save_express_name.str_buf)-1,&font_charts,0,1);
	sys.redraw_all_flag=0;
	vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
    }
    else {
	user_interface.bar(tab1+10,vtab2+15,tab1+600,vtab2+40,V_WHITE);
	user_interface.rect(tab1+10,vtab2+15,tab1+600,vtab2+40,1);
	scr5_game_name.paint();
	scr5_button_save.paint();
	scr5_button_cancel.paint();
	vga.blt_buf(tab1+10,vtab2+15,tab1+600,vtab2+40);
	vga.blt_buf(tab2+70,vtab3+5,tab2+170,vtab3+25);
	vga.blt_buf(tab2+360,vtab3+5,tab2+460,vtab3+25);
    }
}

static int save_express_detect() {
    vga.use_back();
    int saveNow = scr5_game_name.detect()==KEY_RETURN;

    vga.use_front();                                // ## use_front to make the button flashing in release mode.
    if( saveNow || scr5_button_save.detect()) {
	String tmp;
	strcpy(tmp.str_buf,scr5_game_name.input_field);
	if(stricmp((tmp.str_buf+tmp.len()-4),".xps")!=0) {
	    tmp+=".xps";
	}
	game_file.save_express_game(tmp.str_buf);
	current_report_mode=TAB_SCREEN;
	sys.redraw_all_flag=1;
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
    sys.yield();
    //##	begin	zhoubin		000405
    player_school.right_bws_pool_size=0;
    player_school.left_bws_pool_size=0;

    for(int i=0,l=0,r=0;i<department_res.department_count;i++) {
	if(player_school.tmp_department_pool_flag[i]==0) {
	    player_school.left_bws_pool[l]=i+1;
	    player_school.left_bws_pool_size++;
	    l++;
	    if(player_school.just_remove==(i+1)) {
		scr2_browseRecCount2=l;
		player_school.just_remove=0;
	    }
	}
	else {
	    player_school.right_bws_pool[r]=i+1;
	    player_school.right_bws_pool_size++;
	    r++;
	    if(player_school.just_add==(i+1)) {
		scr2_browseRecCount1=r;
		player_school.just_add=0;
	    }
	}
    }
    //##	end		zhoubin	000405

    if(selNum==3) {
	//---init. of screen 1 -//
	if(refreshFlag== INFO_REPAINT) {
	    new_game_deinit();

	    user_interface.brighten(TMENU_X1+5,TMENU_Y1+5,TMENU_X2-5,TMENU_Y2-40);
	    user_interface.rect(TMENU_X1+5,TMENU_Y1+5,TMENU_X2-5,TMENU_Y2-40);
	    scr1_vbrowser1.init(TMENU_X1+10,TMENU_Y1+10,TMENU_X2-5,TMENU_Y2-40,
				0,  scr1_browseRecHeight,scr1_browseRecCount1,
				scr1_disp_rep_row1,NULL,1);
	    //			scr1_vbrowser2.init(TMENU_X1+5,TMENU_Y2-175,TMENU_X2-5,TMENU_Y2-40,
	    //			  0,	scr1_browseRecHeight, scr1_browseRecCount2,
	    //			  scr1_disp_rep_row2,NULL,0);

	    //--------- create buttons ---------//

	    scr1_button_load.create_text
		( TMENU_X1+50,TMENU_Y2-25,TMENU_X1+220,TMENU_Y2-5,
		  "Load express game");
	    scr1_button_load.set_key('L');

	    scr1_button_save.create_text
		( TMENU_X1+260,TMENU_Y2-25,TMENU_X1+430,TMENU_Y2-5,
		  "Save express game");
	    scr1_button_save.set_key('S');

	    scr1_button_del.create_text
		( TMENU_X2-220,TMENU_Y2-25,TMENU_X2-50,TMENU_Y2-5,
		  "Delete express game");
	    scr1_button_del.set_key('D');

	    //----------------------------------//

	    scr1_vbrowser1.open(scr1_vbrowser1.recno());
	    //		  scr1_vbrowser2.open(scr1_vbrowser2.recno());
	}
	//		scr1_vbrowser1.update();
	//		scr1_vbrowser2.update();
	scr1_button_load.paint();
	scr1_button_save.paint();
	scr1_button_del.paint();
    }

    ///////////////////////////////////////////////////////////////////////////////////

    if(selNum==0) {
	int vtab1=TMENU_Y1+5,vtab2=TMENU_Y1+60,vtab3=TMENU_Y1+120,vtab4=TMENU_Y1+320;
	int vtab5=TMENU_Y1+280,vtab6=TMENU_Y1+220,vtab7=TMENU_Y1+255,vtab8=TMENU_Y1+290;
	int tab1=TMENU_X1+15,tab2=TMENU_X1+150,tab3=TMENU_X1+390,tab4=TMENU_X1+680;

	if(refreshFlag== INFO_REPAINT) {
	    //---init. of screen 2 -//
	    new_game_deinit();

	    // Basic options --//
	    user_interface.rect(tab1,vtab1+20,tab1+115,vtab1+100);
	    //			button_up.create_text(tab1+80,vtab1+33,tab1+105,vtab1+60,"^");
	    //			button_down.create_text(tab1+80,vtab1+63,tab1+105,vtab1+90,"v");

	    button_add.create_text(tab1+285,vtab3+60,tab1+385,vtab3+80,"Add >>");
	    button_remove.create_text(tab1+285,vtab3+100,tab1+385,vtab3+120,"<< Remove ");
	    button_reset.create_text(tab1+285,vtab3+140,tab1+385,vtab3+160,"Reset");

	    font_chart_blue_sm.put(tab1+5,vtab1+5,"Institution logo");
	    font_chart_blue_sm.put(tab2,vtab1+5,"Institution name");
	    font_chart_blue_sm.put(tab2,vtab1+60,"Player first name");
	    font_chart_blue_sm.put(tab2+268,vtab1+60,"Last name");

	    scr2_name[0].init(tab2+5,vtab1+28,tab2+520,player_school.school_name,sizeof(player_school.school_name)-1,&font_charts,0,1);
	    scr2_name[1].init(tab2+5,vtab1+83,tab2+250,player_school.player_first_name,sizeof(player_school.player_first_name)-1,&font_charts,0,1);
	    scr2_name[2].init(tab2+275,vtab1+83,tab2+520,player_school.player_last_name,sizeof(player_school.player_last_name)-1,&font_charts,0,1);
	    scr2_name.set_focus(0);
	    scr2_name[0].paint();
	    scr2_name[1].paint();
	    scr2_name[2].paint();

	    font_charts.put(tab1+40,vtab3,"Available departments");
	    font_charts.put(tab1+460,vtab3,"Included departments");
	}

	if(redraw_browser_flag || refreshFlag==INFO_REPAINT) {
	    player_school.right_bws_pool_size=0;
	    player_school.left_bws_pool_size=0;

	    for(int i=0,l=0,r=0;i<department_res.department_count;i++) {
		if(player_school.tmp_department_pool_flag[i]==0) {
		    player_school.left_bws_pool[l]=i+1;
		    player_school.left_bws_pool_size++;
		    l++;
		    if(player_school.just_remove==(i+1)) {
			scr2_browseRecCount2=l;
			player_school.just_remove=0;
		    }
		}
		else {
		    player_school.right_bws_pool[r]=i+1;
		    player_school.right_bws_pool_size++;
		    r++;
		    if(player_school.just_add==(i+1)) {
			scr2_browseRecCount1=r;
			player_school.just_add=0;
		    }
		}
	    }

	    if(refreshFlag==INFO_REPAINT) {
		// backup the background of the vbrowser
		scr2_vbrowser1_bitmap=
		    vga.active_buf->save_area(
			tab1+400,vtab3+30,tab1+670,vtab4,
			scr2_vbrowser1_bitmap
			);
		scr2_vbrowser2_bitmap=
		    vga.active_buf->save_area(
			tab1,vtab3+30,tab1+270,vtab4,
			scr2_vbrowser2_bitmap
			);
		char* bitmapPtr = image_mascot.get_data(player_school.mascot);
		if( bitmapPtr ) {
		    vga_back.put_bitmap(tab1+10,vtab1+33,bitmapPtr );
		}

		image_interface.put_back(tab1+80,vtab1+35,"U-ARW-UP");
		image_interface.put_back(tab1+80,vtab1+65,"D-ARW-UP");

		user_interface.bar(tab2-5,vtab1+22,tab2+520,vtab1+46,V_WHITE);
		user_interface.bar(tab2-5,vtab1+77,tab2+250,vtab1+101,V_WHITE);
		user_interface.bar(tab2+265,vtab1+77,tab2+520,vtab1+101,V_WHITE);
		user_interface.rect(tab2-5,vtab1+22,tab2+520,vtab1+46,1);
		user_interface.rect(tab2-5,vtab1+77,tab2+250,vtab1+101,1);
		user_interface.rect(tab2+265,vtab1+77,tab2+520,vtab1+101,1);

		button_add.paint();
		button_remove.paint();
		button_reset.paint();
		button_add.pop();
		button_remove.pop();
		button_reset.pop();
		scr2_name.paint();

	    }
	    else {
		// redraw the background
		vga.active_buf->rest_area(scr2_vbrowser1_bitmap,0,0);
		vga.active_buf->rest_area(scr2_vbrowser2_bitmap,0,0);
	    }

	    user_interface.rect(tab1+395,vtab3+25,tab1+670,vtab4);
	    user_interface.rect(tab1-5,vtab3+25,tab1+270,vtab4);

	    scr2_vbrowser1.init(tab1+400,vtab3+30,tab1+670,vtab4,
				0,  scr2_browseRecHeight,player_school.right_bws_pool_size,
				scr2_disp_rep_row1,NULL,1);
	    scr2_vbrowser2.init(tab1,vtab3+30,tab1+270,vtab4,
				0,  scr2_browseRecHeight,player_school.left_bws_pool_size,
				scr2_disp_rep_row2,NULL,1);

	    scr2_vbrowser1.open(scr2_browseRecCount1);
	    scr2_vbrowser2.open(scr2_browseRecCount2);
	    redraw_browser_flag=0;
	}
	//////////////////////////////////////////////////////////////////////
	scr2_browseRecCount1=scr2_vbrowser1.recno();
	scr2_browseRecCount2=scr2_vbrowser2.recno();

	if(just_some_items_detected_flag) {
	    char* bitmapPtr = image_mascot.get_data(player_school.mascot);
	    if( bitmapPtr ) {
		vga_back.put_bitmap(tab1+10,vtab1+33,bitmapPtr );
	    }

	    image_interface.put_back(tab1+80,vtab1+35,"U-ARW-UP");
	    image_interface.put_back(tab1+80,vtab1+65,"D-ARW-UP");

	    user_interface.bar(tab2-5,vtab1+22,tab2+520,vtab1+46,V_WHITE);
	    user_interface.bar(tab2-5,vtab1+77,tab2+250,vtab1+101,V_WHITE);
	    user_interface.bar(tab2+265,vtab1+77,tab2+520,vtab1+101,V_WHITE);
	    user_interface.rect(tab2-5,vtab1+22,tab2+520,vtab1+46,1);
	    user_interface.rect(tab2-5,vtab1+77,tab2+250,vtab1+101,1);
	    user_interface.rect(tab2+265,vtab1+77,tab2+520,vtab1+101,1);

	    button_add.paint();
	    button_remove.paint();
	    button_reset.paint();
	    button_add.pop();
	    button_remove.pop();
	    button_reset.pop();
	    scr2_name.paint();
	    just_some_items_detected_flag=0;
	}
    }
    ///////////////////////////////////////////////////////////////////////////////

    if(selNum==1) {
	int vtab1=20,vtab2=55,vtab3=90,vtab4=125;
	int vtab5=185,vtab6=220,vtab7=255,vtab8=290;
	int tab1=25,tab2=300,tab3=400,tab4=680;
	tab2=360;
	int TMENU_X2_2CHOICE = (TMENU_X1+tab2 + 2*TMENU_X2) / 3;
	//---init. of screen 3 -//
	if(refreshFlag== INFO_REPAINT) {
	    new_game_deinit();
	    user_interface.rect(TMENU_X1+5,TMENU_Y1+5,TMENU_X2-5,TMENU_Y1+vtab5-25, 1);
	    user_interface.rect(TMENU_X1+5,TMENU_Y1+vtab5-15,TMENU_X2-5,TMENU_Y2-5, 1);

	    // option in ver2 is re-arranged

	    scr3_rbutton1.init(TMENU_X1+tab2,TMENU_Y1+vtab1,TMENU_X2_2CHOICE,TMENU_Y1+vtab1+20,
			       2, &(player_school.control),
			       scr3_rbutton1_options);

	    scr3_rbutton2.init(TMENU_X1+tab2,TMENU_Y1+vtab2,TMENU_X2, TMENU_Y1+vtab2+20,
			       3, &(player_school.student_count),
			       scr3_rbutton2_options);

	    scr3_rbutton3.init(TMENU_X1+tab2,TMENU_Y1+vtab3,TMENU_X2, TMENU_Y1+vtab3+20,
			       3, &(player_school.parttime_undergrad_percent),
			       scr3_rbutton3_options);

	    scr3_rbutton4.init(TMENU_X1+tab2,TMENU_Y1+vtab4,TMENU_X2, TMENU_Y1+vtab4+20,
			       3, &(player_school.sponsored_research_intensity),
			       scr3_rbutton4_options);

	    scr3_rbutton5.init(TMENU_X1+tab2,TMENU_Y1+vtab5,TMENU_X2, TMENU_Y1+vtab5+20,
			       3, &(player_school.graduate_student_percent),
			       scr3_rbutton5_options);

	    scr3_rbutton6.init(TMENU_X1+tab2,TMENU_Y1+vtab6,TMENU_X2, TMENU_Y1+vtab6+20,
			       3, &(player_school.ug_get_degree_in_5year_percent),
			       scr3_rbutton6_options);

	    scr3_rbutton7.init(TMENU_X1+tab2,TMENU_Y1+vtab7,TMENU_X2, TMENU_Y1+vtab7+20,
			       3, &(player_school.fulltime_ug_in_housing_percent),
			       scr3_rbutton7_options);

	    scr3_rbutton8.init(TMENU_X1+tab2,TMENU_Y1+vtab8,TMENU_X2, TMENU_Y1+vtab8+20,
			       3, &(player_school.athletic_program_intensity),
			       scr3_rbutton8_options);

	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab1+5,
				   scr3_label[0]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab2+5,
				   scr3_label[1]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab3+5,
				   scr3_label[2]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab4+5,
				   scr3_label[3]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab5+5,
				   scr3_label[4]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab6+5,
				   scr3_label[5]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab7+5,
				   scr3_label[6]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab8+5,
				   scr3_label[7]);

	    //			scr3_bounded_slider0.refresh();
	    //			scr3_bounded_slider2.refresh();
	    //			scr3_slider2.refresh();
	    //			scr3_slider3.refresh();
	    //			scr3_slider4.refresh();
	}

	if(just_some_items_detected_flag) {
	    scr3_rbutton1.refresh();
	    scr3_rbutton2.refresh();
	    scr3_rbutton3.refresh();
	    scr3_rbutton4.refresh();
	    scr3_rbutton5.refresh();
	    scr3_rbutton6.refresh();
	    scr3_rbutton7.refresh();
	    scr3_rbutton8.refresh();
	    just_some_items_detected_flag=0;
	}

    }

    if(selNum==2) {
	int vtab1=25,vtab2=50,vtab3=75;
	int vtab4=135,vtab5=160,vtab6=185,vtab7=210,vtab8=235,vtab9=260;
	int tab1=25,tab2=300,tab3=400,tab4=680;
	int TMENU_X2_2CHOICE = (TMENU_X1+tab2 + 2*TMENU_X2) / 3;

	//--- initialize screen 4 -----//

	if(refreshFlag==INFO_REPAINT) {
	    new_game_deinit();

	    user_interface.rect(TMENU_X1+5,TMENU_Y1+vtab1-15,TMENU_X2-5,TMENU_Y1+vtab3+35, 1);
	    user_interface.rect(TMENU_X1+5,TMENU_Y1+vtab4-15,TMENU_X2-5,TMENU_Y1+vtab9+35, 1);

	    scr4_rbutton1.init(TMENU_X1+tab2,TMENU_Y1+vtab1,TMENU_X2,TMENU_Y1+vtab1+20,
			       3, &(player_school.prestige), scr4_rbutton1_options);
	    scr4_rbutton2.init(TMENU_X1+tab2,TMENU_Y1+vtab2,TMENU_X2,TMENU_Y1+vtab2+20,
			       3, &(player_school.relative_wealth), scr4_rbutton2_options);
	    scr4_rbutton3.init(TMENU_X1+tab2,TMENU_Y1+vtab3,TMENU_X2,TMENU_Y1+vtab3+20,
			       3, &(player_school.campus_environment), scr4_rbutton3_options);
	    scr4_rbutton4.init(TMENU_X1+tab2,TMENU_Y1+vtab4,TMENU_X2,TMENU_Y1+vtab4+20,
			       3, &(player_school.chance_event_frequency), scr4_rbutton4_options);
	    scr4_rbutton5.init(TMENU_X1+tab2,TMENU_Y1+vtab5,TMENU_X2,TMENU_Y1+vtab5+20,
			       3, &(player_school.chance_objective_frequency), scr4_rbutton5_options);
	    scr4_rbutton6.init(TMENU_X1+tab2,TMENU_Y1+vtab6,TMENU_X2_2CHOICE,TMENU_Y1+vtab6+20,
			       2, &(player_school.chance_perturbations), scr4_rbutton6_options);
	    scr4_rbutton7.init(TMENU_X1+tab2,TMENU_Y1+vtab7,TMENU_X2,TMENU_Y1+vtab7+20,
			       3, &(player_school.behavioral_response_speed), scr4_rbutton7_options);
	    scr4_rbutton8.init(TMENU_X1+tab2,TMENU_Y1+vtab8,TMENU_X2_2CHOICE,TMENU_Y1+vtab8+20,
			       2, &(config.auto_save), scr4_rbutton8_options);
	    scr4_rbutton9.init(TMENU_X1+tab2,TMENU_Y1+vtab9,TMENU_X2,TMENU_Y1+vtab9+20,
			       3, &(config.log_enable), scr4_rbutton9_options);

	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab1+5,
				   scr4_label[0]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab2+5,
				   scr4_label[1]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab3+5,
				   scr4_label[2]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab4+5,
				   scr4_label[3]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab5+5,
				   scr4_label[4]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab6+5,
				   scr4_label[5]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab7+5,
				   scr4_label[6]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab8+5,
				   scr4_label[7]);
	    font_chart_blue_sm.put(TMENU_X1+tab1,TMENU_Y1+vtab9+5,
				   scr4_label[8]);

	}

	if(just_some_items_detected_flag) {
	    scr4_rbutton1.refresh();
	    scr4_rbutton2.refresh();
	    scr4_rbutton3.refresh();
	    scr4_rbutton4.refresh();
	    scr4_rbutton5.refresh();
	    scr4_rbutton6.refresh();
	    scr4_rbutton7.refresh();
	    scr4_rbutton8.refresh();
	    scr4_rbutton9.refresh();
	    just_some_items_detected_flag=0;
	}
    }
    return 0;
}

static char can_remove_this_department( int deptId ) {
    for(int i=0;i<DEPARTMENT_TYPE_COUNT;i++) {
	if ( i+1 == deptId )                            // no need to do if the same department
	    continue;

	// in the selected list
	if ( player_school.tmp_department_pool_flag[i] ) {
	    if ( department_res[deptId]->field_id == department_res[i+1]->field_id )
		return 1;
	}
    }

    return 0;
}

static int callback_detect(int selNum) {

    if(selNum==3) {
	scr1_vbrowser1.detect();
	vga.use_front();

	//-------- detect Load button --------//

	if(scr1_button_load.detect()||scr1_vbrowser1.double_click==1) {
	    if(scr1_vbrowser1.recno()>0) {
		GameFile* gfPtr = game_file_array[scr1_vbrowser1.recno()];
		if(gfPtr) {
		    char* name = gfPtr->file_name;
		    if(strlen(name)>0) {
			gfPtr->load_express_game(name);
			current_report_mode=TAB_SCREEN;
			*(tab_menu.sel_num)=selNum=0;
			sys.redraw_all_flag=1;
		    }
		}
	    }
	    scr1_vbrowser1.double_click=0;
	}

	//-------- detect Save button --------//

	if(scr1_button_save.detect()) {
	    current_report_mode=SAVE_EXPRESS;
	    sys.redraw_all_flag=1;
	}

	//-------- detect Delete button --------//

	if(scr1_button_del.detect() ) {
	    if( scr1_vbrowser1.recno()>0 ) {
		game_file_array.del_express_game(scr1_vbrowser1.recno());
		sys.redraw_all_flag=1;
	    }
	}

	vga.use_back();
    }

    if(selNum==0) {
	int tab1=TMENU_X1+15,vtab1=TMENU_Y1+5;
	vga.use_back();

	//		if(button_up.detect())
	// if(mouse.single_click(tab1+80,vtab1+33,tab1+105,vtab1+60))
	// use any_click in version 2
	if(mouse.any_click(tab1+80,vtab1+33,tab1+105,vtab1+60)) {
	    do {
		player_school.mascot--;
		if(player_school.mascot<1)
		    player_school.mascot=image_mascot.rec_count;
	    } while( player_school.is_hidden_mascot(player_school.mascot) );
	    image_interface.put_front(tab1+80,vtab1+35,"U-ARW-DN");
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    just_some_items_detected_flag=1;
	}

	//		if(button_down.detect())
	// if(mouse.single_click(tab1+80,vtab1+63,tab1+105,vtab1+90))
	if(mouse.any_click(tab1+80,vtab1+63,tab1+105,vtab1+90)) {
	    do {
		player_school.mascot++;
		if(player_school.mascot>image_mascot.rec_count)
		    player_school.mascot=1;
	    } while( player_school.is_hidden_mascot(player_school.mascot) );
	    image_interface.put_front(tab1+80,vtab1+65,"D-ARW-DN");
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    just_some_items_detected_flag=1;
	}

	sys.yield();

	if(scr2_name.detect())
	    just_some_items_detected_flag=1;

	vga.use_front();                              // ## use_front to make the button flashing in release mode.

	if(button_add.detect()) {
	    if(scr2_vbrowser2.recno()>0) {
		for(int i=0;i<DEPARTMENT_TYPE_COUNT;i++)
		    if(player_school.selected_department_list[i]==0) {
			player_school.just_add=player_school.left_bws_pool[scr2_vbrowser2.recno()-1];
			player_school.selected_department_list[i]=player_school.just_add;
			player_school.tmp_department_pool_flag[player_school.just_add-1]=1;
			break;
		    }
		redraw_browser_flag=1;
	    }
	    just_some_items_detected_flag=1;
	}

	sys.yield();

	if(button_remove.detect()) {
	    if(scr2_vbrowser1.recno()>0) {
		int i;
		int tmp=player_school.right_bws_pool[scr2_vbrowser1.recno()-1];

		if ( can_remove_this_department( tmp ) ) {
		    for(i=0;i<DEPARTMENT_TYPE_COUNT;i++)
			if(player_school.selected_department_list[i]==tmp) {
			    player_school.just_remove=player_school.right_bws_pool[scr2_vbrowser1.recno()-1];
			    player_school.selected_department_list[i]=0;
			    player_school.tmp_department_pool_flag[player_school.just_remove-1]=0;
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

		    redraw_browser_flag=1;
		}
		else {
		    // msg box
		    char msgStr[200];

		    short fieldId = department_res[tmp]->field_id;

		    sprintf( msgStr, "At least one department is required in this domain: %s. You must add another department in the domain before removing this one.", field_name[fieldId] );

		    box.msg( msgStr );
		}
	    }
	    just_some_items_detected_flag=1;
	}

	if(button_reset.detect()) {
	    for(int i=0;i<department_res.department_count;i++) {
		player_school.tmp_department_pool_flag[i]=0;
		player_school.selected_department_list[i]=0;

	    }

	    redraw_browser_flag=1;

	    //			{11,7,22,15,16,20,26,3,6};
	    //			for(i=0;i<DEPARTMENT_TYPE_COUNT;i++)
	    //			player_school.selected_department_list[i]=(i<MIN_DEPARTMENT)?(i+1):0;

	    player_school.selected_department_list[0]=11;
	    player_school.tmp_department_pool_flag[11-1]=1;
	    player_school.selected_department_list[1]=7;
	    player_school.tmp_department_pool_flag[7-1]=1;
	    player_school.selected_department_list[2]=2;
	    player_school.tmp_department_pool_flag[2-1]=1;
	    player_school.selected_department_list[3]=15;
	    player_school.tmp_department_pool_flag[15-1]=1;
	    player_school.selected_department_list[4]=16;
	    player_school.tmp_department_pool_flag[16-1]=1;
	    player_school.selected_department_list[5]=10;
	    player_school.tmp_department_pool_flag[10-1]=1;
	    player_school.selected_department_list[6]=20;
	    player_school.tmp_department_pool_flag[20-1]=1;
	    player_school.selected_department_list[7]=26;
	    player_school.tmp_department_pool_flag[26-1]=1;
	    player_school.selected_department_list[8]=3;
	    player_school.tmp_department_pool_flag[3-1]=1;
	    player_school.selected_department_list[9]=6;
	    player_school.tmp_department_pool_flag[6-1]=1;

	    scr2_browseRecCount1=1;
	    scr2_browseRecCount2=1;
	    just_some_items_detected_flag=1;
	}
	sys.yield();

	vga.use_back();
	scr2_vbrowser1.detect();
	scr2_vbrowser2.detect();
    }

    if(selNum==1) {
	if(scr3_rbutton1.detect())
	    just_some_items_detected_flag=1;
	if(scr3_rbutton2.detect())
	    just_some_items_detected_flag=1;
	if(scr3_rbutton3.detect())
	    just_some_items_detected_flag=1;
	if(scr3_rbutton4.detect())
	    just_some_items_detected_flag=1;
	if(scr3_rbutton5.detect())
	    just_some_items_detected_flag=1;
	if(scr3_rbutton6.detect())
	    just_some_items_detected_flag=1;
	if(scr3_rbutton7.detect())
	    just_some_items_detected_flag=1;
	if(scr3_rbutton8.detect())
	    just_some_items_detected_flag=1;

	sys.yield();

	//  	if(scr3_bounded_slider0.detect())
	//			player_school.prestige=player_school_prestige/1000;
	//		scr3_slider2.detect();
	//		scr3_slider3.detect();
	//		scr3_slider4.detect();
	//		scr3_slider5.detect();
	//		scr3_bounded_slider2.detect();
    }

    if(selNum==2) {
	if(scr4_rbutton1.detect())
	    just_some_items_detected_flag=1;
	if(scr4_rbutton2.detect())
	    just_some_items_detected_flag=1;
	if(scr4_rbutton3.detect())
	    just_some_items_detected_flag=1;
	if(scr4_rbutton4.detect())
	    just_some_items_detected_flag=1;
	if(scr4_rbutton5.detect())
	    just_some_items_detected_flag=1;
	if(scr4_rbutton6.detect())
	    just_some_items_detected_flag=1;
	if(scr4_rbutton7.detect())
	    just_some_items_detected_flag=1;
	if(scr4_rbutton8.detect()) {
	    just_some_items_detected_flag=1;
	    // ask directory of auto save game
	    if( config.auto_save ) {
		if( !config.auto_save_file_name[0] ) {
		    MouseDispCount mouseDispCount;          // redisplay windows cursor

		    char tempFilename[sizeof(config.auto_save_file_name)];

		    strcpy(tempFilename, "save001.sav");

		    // ask filename now
		    OPENFILENAME openFileName;
		    memset( &openFileName, 0, sizeof(openFileName) );
		    openFileName.lStructSize = sizeof(openFileName);
		    openFileName.hwndOwner = sys.main_hwnd;
		    openFileName.hInstance = sys.app_hinstance;
		    // two '\0' at the end
		    openFileName.lpstrFilter = "Virtual U Auto Save(*.sav)\0*.sav\0All Files(*.*)\0*.*\0";
		    openFileName.lpstrCustomFilter = NULL;
		    openFileName.nMaxCustFilter = 0;
		    openFileName.nFilterIndex = 0;
		    openFileName.lpstrFile =  tempFilename; // first char is already NULL
		    openFileName.nMaxFile = sizeof(tempFilename);
		    openFileName.lpstrFileTitle = NULL;     // result filename, without path
		    openFileName.nMaxFileTitle = 0;
		    openFileName.lpstrInitialDir = ".";     // current directory
		    openFileName.lpstrTitle = NULL;
		    openFileName.Flags = OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
		    openFileName.nFileOffset = 0;
		    openFileName.nFileExtension = 0;
		    openFileName.lpstrDefExt = "sav";
		    openFileName.lCustData = NULL;
		    openFileName.lpfnHook = NULL;
		    openFileName.lpTemplateName = NULL;

		    if( !GetSaveFileName(&openFileName) )
			config.auto_save = 0;                 // cancel will disable autosave
		    else
			strcpy(config.auto_save_file_name, tempFilename );

		}                                         // call mouseDispCount destructor
	    }
	}
	if(scr4_rbutton9.detect()) {
	    just_some_items_detected_flag=1;
	    // ask log file name
	    if( config.log_enable ) {
		// ask log filename, first time only
		if( !config.log_file_name[0] ) { {
		    MouseDispCount mouseDispCount;        // redisplay windows cursor

		    strcpy( config.log_file_name, "autolog.log" );

		    // ask filename now
		    OPENFILENAME openFileName;
		    memset( &openFileName, 0, sizeof(openFileName) );
		    openFileName.lStructSize = sizeof(openFileName);
		    openFileName.hwndOwner = sys.main_hwnd;
		    openFileName.hInstance = sys.app_hinstance;
		    // two '\0' at the end
		    openFileName.lpstrFilter = "Log file(*.log)\0*.log\0All Files(*.*)\0*.*\0";
		    openFileName.lpstrCustomFilter = NULL;
		    openFileName.nMaxCustFilter = 0;
		    openFileName.nFilterIndex = 0;
		    // first char is already NULL
		    openFileName.lpstrFile =  config.log_file_name;
		    openFileName.nMaxFile = sizeof(config.log_file_name);
		    openFileName.lpstrFileTitle = NULL;   // result filename, without path
		    openFileName.nMaxFileTitle = 0;
		    openFileName.lpstrInitialDir = ".";   // current directory
		    openFileName.lpstrTitle = NULL;
		    openFileName.Flags = OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
		    openFileName.nFileOffset = 0;
		    openFileName.nFileExtension = 0;
		    openFileName.lpstrDefExt = "log";
		    openFileName.lCustData = NULL;
		    openFileName.lpfnHook = NULL;
		    openFileName.lpTemplateName = NULL;

		    if( !GetSaveFileName(&openFileName) ) {
			config.log_enable = 0;              // cancel will disable autosave
			config.log_file_name[0] = '\0';     // clear name
		    }
		}                                       // call mouseDispCount destructor

		if( config.log_enable && config.log_file_name[0] )
		    // overwrite old log file
		    game_file.create_game_log(config.log_file_name);
		}
	    }
	}
	sys.yield();
	/*
	  scr4_slider1.detect();

	  if(scr4_slider3.detect())
          player_school.doctoral_program_intensity=player_school_doctoral_program_intensity/1000;

	  if(scr4_slider4.detect())
          player_school.sponsored_research_intensity=player_school_sponsored_research_intensity/1000;
	*/
    }
    sys.yield();
    return 0;
}

static void scr1_disp_rep_row1(int recNo, int x, int y, int refreshFlag) {
    if((recNo>0)&&(recNo<=game_file_array.size())) {

	GameFile* gfPtr = game_file_array[recNo];
	char* name = gfPtr->get_save_name();
	font_chartsm.put(x,y,name);
    }
}

static void scr1_disp_rep_row2(int recNo, int x, int y, int refreshFlag) {
}

static void scr2_disp_rep_row1(int recNo, int x, int y, int refreshFlag) {
    //	user_interface.bar(x, y, x+240, y+16,Vga::active_buf->color_down);
	int j;
    for(int i=0,j=0;(i<recNo)&&(j<DEPARTMENT_TYPE_COUNT);j++)
	if(player_school.tmp_department_pool_flag[j]==1)
	    i++;
    //	err.run("department_res.department_count=%d\n",department_res.department_count);
    if(j<=department_res.department_count)
	font_letter.put(x,y,department_res[j]->name);
}

static void scr2_disp_rep_row2(int recNo, int x, int y, int refreshFlag) {
    //	user_interface.bar(x, y, x+240, y+16,Vga::active_buf->color_down);
	int j;
    for(int i=0,j=0;(i<recNo)&&(j<DEPARTMENT_TYPE_COUNT);j++)
	if(player_school.tmp_department_pool_flag[j]==0)
	    i++;
    //	DEBUG_LOG("j=");
    //	DEBUG_LOG(j);
    //	DEBUG_LOG("\n");
    if(j<=department_res.department_count)
	font_letter.put(x,y,department_res[j]->name);
}

static int current_score_report_type=0;           // 0 to SCORE_SUBGROUP_COUNT-1
static int customize_score_init_flag = 0;

static short *type_button_up_bitmap[SCORE_SUBGROUP_COUNT];
static short *type_button_down_bitmap[SCORE_SUBGROUP_COUNT];
static char  *type_button_label[SCORE_SUBGROUP_COUNT]= {
    "Outputs",
    "Performance indicators",
    "Attitudes toward the institution",
    "Financial indicators",
};
static Text text_sub_group_weight;
static ButtonGroup type_button_group(SCORE_SUBGROUP_COUNT);
static Button button_restore_weight;
static Button button_normalize_weight;

enum {
    BOTTOM_BUTTON_WIDTH = 100,
    BOTTOM_BUTTON_WIDTH2 = 150,
    BOTTOM_BUTTON_WIDTH3 = 220,
    BOTTOM_BUTTON_HEIGHT = 25,
    BOTTOM_BUTTON_DISTANCE = (REPORT_X2-REPORT_X1-BOTTOM_BUTTON_WIDTH*6)/6+30,
};

enum {
    WINDOW_X1 = REPORT_X1+10,
    WINDOW_Y1 = REPORT_Y1+16,
    WINDOW_X2 = REPORT_X2-10,
    //	WINDOW_Y2 = REPORT_Y2-44,
    WINDOW_Y2 = REPORT_Y2-52,
};

static char * sub_score_title[SCORE_VAR_COUNT]= {
    "Degrees granted",
    "Sponsored performance",

    "Institutional prestige",
    "Educational quality",
    "Scholarship, broadly defined",
    "Student diversity",
    "Faculty diversity",
    "Percentage of alumni who have donated during the last five years",

    "Faculty morale",
    "Student morale",
    "Staff morale",

    "Current surplus (deficit) as a percentage of expenditure",
    "Smoothed surplus (deficit) as a percentage of expenditure",
    "Endowment payout rate relative to the equilibrium payout rate",
    "Deferred maintenance backlog",
};
static Spinner sub_score_weight_spinner[SCORE_VAR_COUNT];

static void customize_score_deinit() {
    if( customize_score_init_flag ) {
	//------- deinit type buttons --------//
	type_button_group.deinit_buttons();
	for (int i = 0; i < SCORE_SUBGROUP_COUNT; i++) {
	    mem_del(type_button_up_bitmap[i]);
	    mem_del(type_button_down_bitmap[i]);
	    type_button_up_bitmap[i] = NULL;
	    type_button_down_bitmap[i] = NULL;
	}
	text_sub_group_weight.deinit();
	for( int i = 0; i < SCORE_VAR_COUNT; ++i ) {
	    sub_score_weight_spinner[i].pause_when_focus = 0;
	    sub_score_weight_spinner[i].deinit();
	}
	customize_score_init_flag = 0;
    }
}

static void customize_score_mode(int refreshFlag) {
    const static int tab1=10,tab2=150,tab3=320,tab4=420,tab5=490,tab6=620,tab7=680;
    const static int vtab1=22,vtab2=37,vtab3=200,vtab4=330,vtab5=320;

    int w0 = 0;
    int w1 = 0;
    int i;

    switch(current_score_report_type) {
    case 0: w0=GROUP1_BEGIN_ID; w1=GROUP2_BEGIN_ID; break;
    case 1: w0=GROUP2_BEGIN_ID; w1=GROUP3_BEGIN_ID; break;
    case 2: w0=GROUP3_BEGIN_ID; w1=GROUP4_BEGIN_ID; break;
    case 3: w0=GROUP4_BEGIN_ID; w1=SCORE_VAR_COUNT; break;
    }

    if( refreshFlag == INFO_REPAINT ) {
	customize_score_deinit();

	user_interface.bg_img(pic_id, &vga_back);

	// top title
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-10,REPORT_X1+630,REPORT_Y1+13);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-10,REPORT_X1+630,REPORT_Y1+13,1);
	font_charts.center_put(REPORT_X1+100,REPORT_Y1-10,REPORT_X1+630,REPORT_Y1+13,
			       "Customize Trustee Performance Weights");

	// create buttons

	for (i = 0; i < SCORE_SUBGROUP_COUNT; i++) {
	    type_button_up_bitmap[i] = NULL;
	    type_button_down_bitmap[i] = NULL;
	}

	int x, y;
	// Outputs page button
	x=REPORT_X1+tab2;
	y=REPORT_Y1+vtab4;

	user_interface.create_button_bitmap(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    type_button_label[0], &type_button_up_bitmap[0],
					    &type_button_down_bitmap[0], &vga_back);
	type_button_group[0].create_bitmapW(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    ((0 == current_score_report_type) ?
					     type_button_down_bitmap[0] :
					     type_button_up_bitmap[0])+4, 0);

	// Performance Indicators page button

	x+=BOTTOM_BUTTON_WIDTH3+BOTTOM_BUTTON_DISTANCE;

	user_interface.create_button_bitmap(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    type_button_label[1], &type_button_up_bitmap[1],
					    &type_button_down_bitmap[1], &vga_back);
	type_button_group[1].create_bitmapW(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    ((1 == current_score_report_type) ?
					     type_button_down_bitmap[1] :
					     type_button_up_bitmap[1])+4, 0);

	// Attitudes toward the institution page button

	x=REPORT_X1+tab2;
	y=REPORT_Y1+vtab4+30;

	user_interface.create_button_bitmap(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    type_button_label[2], &type_button_up_bitmap[2],
					    &type_button_down_bitmap[2], &vga_back);
	type_button_group[2].create_bitmapW(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    ((2 == current_score_report_type) ?
					     type_button_down_bitmap[2] :
					     type_button_up_bitmap[2])+4, 0);

	// Financial indicators page button

	x+=BOTTOM_BUTTON_WIDTH3+BOTTOM_BUTTON_DISTANCE;

	user_interface.create_button_bitmap(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    type_button_label[3], &type_button_up_bitmap[3],
					    &type_button_down_bitmap[3], &vga_back);
	type_button_group[3].create_bitmapW(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    ((3 == current_score_report_type) ?
					     type_button_down_bitmap[3] :
					     type_button_up_bitmap[3])+4, 0);

	err_when( SCORE_SUBGROUP_COUNT != 4 );        // initialize more type_button_label here

	// back button
	x=REPORT_X1+tab1;
	y=REPORT_Y1+vtab4-4;

	button_customize_score.create_text(x, y,
					   x+BOTTOM_BUTTON_WIDTH+35, y+20, "Back" );   // becomes button for going back

	y+=23;
	button_restore_weight.create_text(x, y,
					  // call player_school.init_sub_score_weight
					  x+BOTTOM_BUTTON_WIDTH+35, y+20, "Restore defaults" );

	y+=23;
	button_normalize_weight.create_text(x, y,
					    x+BOTTOM_BUTTON_WIDTH+35, y+20, "Rescale all" );

	// column labels
	user_interface.brighten(WINDOW_X1,WINDOW_Y1,WINDOW_X2,WINDOW_Y2);
	user_interface.rect(WINDOW_X1,WINDOW_Y1,WINDOW_X2,WINDOW_Y2);
	user_interface.rect(WINDOW_X1,WINDOW_Y1,WINDOW_X2,WINDOW_Y1+vtab1);
	font_chartsm.put(WINDOW_X1+tab1,WINDOW_Y1+vtab1-17,"Contributing factors");
	font_chartsm.put(WINDOW_X1+tab4-15,WINDOW_Y1+vtab1-17,"Default");
	font_chartsm.put(WINDOW_X1+tab5,WINDOW_Y1+vtab1-17,"Current weight");

	int j;
	float defSubGroupWeight = 0.0f;
	for( i=w0, j=0; i < w1; ++i,++j ) {
	    font_chartsm.put_paragraph(WINDOW_X1+tab1,WINDOW_Y1+vtab2+j*40,
				       WINDOW_X1+tab3-15,WINDOW_Y1+vtab2+j*40+40-1, sub_score_title[i] );
	    font_chartsm.right_put(WINDOW_X1+tab4+25,WINDOW_Y1+vtab2+j*40, m.format(player_school.get_default_sub_score_weight(i),24));
	    defSubGroupWeight += player_school.get_default_sub_score_weight(i);
	    // weight is player_school.sub_score_weight[i]

	    sub_score_weight_spinner[i].pause_when_focus = 0;
	    sub_score_weight_spinner[i].init(
		WINDOW_X1+tab5, WINDOW_Y1+vtab2+j*40-6,
		WINDOW_X1+tab6-20, WINDOW_Y1+vtab2+j*40+22,
		&player_school.sub_score_weight[i],
		0.0f, 100.0f, 0.1f, 0, NULL, 24 );
	}

	// display total weight of group
	font_chartsm.put(WINDOW_X1+tab1+10, WINDOW_Y2-20, "Total Category Weight", 0, WINDOW_X1+tab3-15);
	font_chartsm.right_put(WINDOW_X1+tab4+25, WINDOW_Y2-20, m.format(defSubGroupWeight,24) );

	customize_score_init_flag = 1;
	sys.redraw_all_flag=0;
    }

    float subGroupWeight = 0.0f;
    for( i=w0; i < w1; ++i ) {
	sub_score_weight_spinner[i].paint();
	subGroupWeight += player_school.sub_score_weight[i];
    }
    // font_charts.right_put(WINDOW_X1+tab6-30, WINDOW_Y2-20, m.format(subGroupWeight,24) );
    text_sub_group_weight.right_put( WINDOW_X1+tab6-30, WINDOW_Y2-20, &subGroupWeight, 24 );

    button_customize_score.paint();
    button_restore_weight.paint();
    button_normalize_weight.paint();
    type_button_group.paint(current_score_report_type);
    vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
}

static int customize_score_detect() {
    // switch sub score group
    if( type_button_group.detect() >= 0 ) {
	current_score_report_type = type_button_group();
	sys.redraw_all_flag=1;
	return 1;
    }

    int w0 = 0;
    int w1 = 0;
    int i;

    switch(current_score_report_type) {
    case 0: w0=GROUP1_BEGIN_ID; w1=GROUP2_BEGIN_ID; break;
    case 1: w0=GROUP2_BEGIN_ID; w1=GROUP3_BEGIN_ID; break;
    case 2: w0=GROUP3_BEGIN_ID; w1=GROUP4_BEGIN_ID; break;
    case 3: w0=GROUP4_BEGIN_ID; w1=SCORE_VAR_COUNT; break;
    }

    for( i=w0; i < w1; ++i ) {
	if( sub_score_weight_spinner[i].detect() )
	    return 1;
    }

    if( button_restore_weight.detect() ) {
	player_school.init_sub_score_weight();
    }

    if( button_normalize_weight.detect() ) {
	player_school.normalize_sub_score_weight();
    }

    if( button_customize_score.detect() ) {
	player_school.normalize_sub_score_weight();
	customize_score_deinit();
	current_report_mode=TAB_SCREEN;
	sys.redraw_all_flag=1;
	return 1;
    }
    return 0;
}

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
//-----------------------------------------------------------------//

GameSetting::GameSetting() {
    tab_sel = 0;                                    // default to Basic Options
    init_flag = 0;
    redraw_browser_flag = 0;
    scr1_browseRecCount1=1;
    scr1_browseRecCount2=1;
    pic_id = 9;
}

void GameSetting::new_game_init() {
    /* reset browsers */
    //	for(int i=0;i<DEPARTMENT_TYPE_COUNT;i++)
    //		player_school.selected_department_list[i]=(i<MIN_DEPARTMENT)?(i+1):0;
    //	int i;

    for(int i=0;i<department_res.department_count;i++) {
	player_school.tmp_department_pool_flag[i]=0;
	player_school.selected_department_list[i]=0;
    }

    player_school.selected_department_list[0]=11;
    player_school.tmp_department_pool_flag[11-1]=1;
    player_school.selected_department_list[1]=7;
    player_school.tmp_department_pool_flag[7-1]=1;
    player_school.selected_department_list[2]=2;
    player_school.tmp_department_pool_flag[2-1]=1;
    player_school.selected_department_list[3]=15;
    player_school.tmp_department_pool_flag[15-1]=1;
    player_school.selected_department_list[4]=16;
    player_school.tmp_department_pool_flag[16-1]=1;
    player_school.selected_department_list[5]=10;
    player_school.tmp_department_pool_flag[10-1]=1;
    player_school.selected_department_list[6]=20;
    player_school.tmp_department_pool_flag[20-1]=1;
    player_school.selected_department_list[7]=26;
    player_school.tmp_department_pool_flag[26-1]=1;
    player_school.selected_department_list[8]=3;
    player_school.tmp_department_pool_flag[3-1]=1;
    player_school.selected_department_list[9]=6;
    player_school.tmp_department_pool_flag[6-1]=1;

    //	for(i=0;i<DEPARTMENT_TYPE_COUNT;i++)
    //		player_school.tmp_department_pool_flag[i]=(i<MIN_DEPARTMENT)?1:0;

    //	player_school_prestige=player_school.prestige*1000;
    //	player_school_doctoral_program_intensity=player_school.doctoral_program_intensity*1000;
    //	player_school_sponsored_research_intensity=player_school.sponsored_research_intensity*1000;

    player_school_prestige=player_school.prestige;
    player_school_doctoral_program_intensity=player_school.doctoral_program_intensity;
    player_school_sponsored_research_intensity=player_school.sponsored_research_intensity;

    player_school.init_sub_score_weight();
    config.auto_save = 0;
    config.auto_save_file_name[0] = '\0';
    config.log_enable = 0;                          // 0=none, 1=year, 2=trimester
    config.log_file_name[0] = '\0';
}

void GameSetting::new_game_report(int refreshFlag) {
    int vtab1=50,vtab2=150,vtab3=160,vtab4=250;
    int tab1=10,tab2=200,tab3=400,tab4=680;

    if(current_report_mode==TAB_SCREEN) {
	if (refreshFlag == INFO_REPAINT) {
	    game_file_array.init("*.XPS");
	    scr1_browseRecCount1=game_file_array.size();
	    vga.use_back();
	    user_interface.bg_img(pic_id, &vga_back);
	    //			vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
	    //			sys.blt_virtual_buf();
	    tab_menu.init(REPORT_X1,REPORT_Y1,REPORT_X2,REPORT_Y2,
			  4, &tab_sel, 150, 150,
			  tab_short_desc, tab_long_desc,
			  callback_disp, callback_detect);

	    button_customize_score.create_text
		(REPORT_X1+350-300, REPORT_Y2+10,
		 REPORT_X1+350-150, REPORT_Y2+30, "Customize score" );
	    button_randomize_setting.create_text
		(REPORT_X1+350-140, REPORT_Y2+10,
		 REPORT_X1+350+10, REPORT_Y2+30, "Randomize setting" );
	    button_start.create_text
		(REPORT_X1+350+20, REPORT_Y2+10,
		 REPORT_X1+350+170, REPORT_Y2+30, "Start game" );
	    button_cancel.create_text
		(REPORT_X1+350+180, REPORT_Y2+10,
		 REPORT_X1+350+330, REPORT_Y2+30, "Back");
	    sys.redraw_all_flag=0;
	    sys.yield();
	}
	else {
	    vga.use_back();
	    sys.yield();
	    tab_menu.refresh(refreshFlag);
	    sys.yield();
	}
	button_customize_score.paint();
	button_randomize_setting.paint();
	button_start.paint();
	button_cancel.paint();
	vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
    }

    if(current_report_mode==SAVE_EXPRESS) {
	vga.use_back();
	save_express_report(refreshFlag);
	//		sys.yield();
	//		vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
	sys.yield();
    }

    if(current_report_mode==CUSTOMIZE_SCORE) {
	vga.use_back();
	customize_score_mode(refreshFlag);
	sys.yield();
    }
}

//------ Begin of function GameSetting::detect ------//
//!
//! return: <int> 1  - the Start button is pressed.
//! 				  -1 - the Cancel button is pressed.
//!					  0  - neither of them is pressed.
//!
int GameSetting::new_game_detect() {
    int rc=0;
    vga.use_back();
    if(current_report_mode==SAVE_EXPRESS) {
	save_express_detect();
    }

    if(current_report_mode==CUSTOMIZE_SCORE) {
	customize_score_detect();
    }

    sys.yield();

    if(current_report_mode==TAB_SCREEN) {
	tab_menu.detect();
	vga.use_front();

	if( button_customize_score.detect() ) {
	    // go in CUSTOMIZE_SCORE
	    current_report_mode = CUSTOMIZE_SCORE;
	    sys.redraw_all_flag=1;
	    rc = 0;
	}
	if( button_randomize_setting.detect() ) {
	    // randomize department
	    for(int i=0;i<department_res.department_count;i++) {
		player_school.tmp_department_pool_flag[i]=0;
		player_school.selected_department_list[i]=0;
	    }

	    redraw_browser_flag=1;

	    int addedCount=0;
	    int maxAdd = MIN_DEPARTMENT + m.random(MAX_DEPARTMENT-(MIN_DEPARTMENT)+1);

	    for(int i=0;i<department_res.department_count;i++) {
		// chance of add a department is vacancy/(no. choice not decide)
		if( addedCount < maxAdd
		    && m.random(department_res.department_count-i) < maxAdd-addedCount ) {
		    player_school.selected_department_list[addedCount]=i+1;
		    player_school.tmp_department_pool_flag[i]=1;
		    addedCount++;
		}
		else {
		    player_school.tmp_department_pool_flag[i]=0;
		}
	    }

	    scr2_browseRecCount1=1;
	    scr2_browseRecCount2=1;
	    just_some_items_detected_flag=1;

	    // randomize score setting
	    player_school.control = m.random(2);
	    player_school.student_count = m.random(3);
	    player_school.parttime_undergrad_percent = m.random(3);
	    player_school.sponsored_research_intensity = m.random(3);
	    player_school.graduate_student_percent = m.random(3);
	    player_school.ug_get_degree_in_5year_percent = m.random(3);
	    player_school.fulltime_ug_in_housing_percent = m.random(3);
	    player_school.athletic_program_intensity = m.random(3);

	    player_school.prestige = m.random(3);
	    player_school.relative_wealth = m.random(3);
	    player_school.campus_environment = m.random(3);

	    just_some_items_detected_flag=1;
	}

	if( button_start.detect() ) {
	    //## fred 0118
	    if(player_school.right_bws_pool_size>MAX_DEPARTMENT) {
		String msg_text;
		msg_text = "The University must have \nat least ";
		msg_text += m.format(MIN_DEPARTMENT,4);
		msg_text += " but not more \nthan ";
		msg_text += m.format(MAX_DEPARTMENT,4);
		msg_text += " departments.";
		box.msg(msg_text.str_buf);
	    }
	    else if(player_school.right_bws_pool_size<MIN_DEPARTMENT) {
		String msg_text;
		msg_text = "You must choose at least \n";
		msg_text += m.format(MIN_DEPARTMENT,4);
		msg_text += " departments.";
		box.msg(msg_text.str_buf);
	    }
	    else {
		rc = 1;
		*(tab_menu.sel_num)=0;
	    }
	}
	if(button_cancel.detect()) {
	    rc = -1;
	}
    }
    vga.use_back();

    return rc;
}

//------- End of function GameSetting::detect ------//

//----------------------------
// - play scenario
//--------------------------------
enum {SCENARIO_DESC=1,SCENARIO_SELECT=2};
enum {TMP_SCENARIO_COUNT=SCENARIO_COUNT};
static int scenario_mode = SCENARIO_DESC;
static VBrowse98 scenario_vbrowser;
static int scenario_browseRecCount1=TMP_SCENARIO_COUNT;
static int scenario_browseRecHeight=19;
static void scenario_disp_rep_row(int,int,int,int);
static char* scn_name[TMP_SCENARIO_COUNT] = {
    "1. PAY BETTER",
    "2. ALLOCATE NEW MONEY",
    "3. TEACH BETTER",
    "4. IMPROVE RESEARCH PERFORMANCE",
    "5. WIN GAMES",
    "6. REDUCE TUITION",
    "7. RESPOND TO ENROLLMENT SHIFTS",
    "8. ENROLL MORE MINORITY STUDENTS",
    "9. HIRE MORE MINORITY FACULTY MEMBERS",
    "10. BALANCE THE BUDGET",
};

static int scn_id[TMP_SCENARIO_COUNT] = {
    SCN_RAISE_SALARY,
    SCN_INVEST_ACADEMIC,
    SCN_TEACHING_QUALITY,                           //## fred0301
    SCN_RESEARCH_QUALITY,                           //## fred0301
    SCN_WINNING_ATHLETICS,
    SCN_LIMIT_TUITION_GROWTH,
    SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS,           //## fred0301
    SCN_STUDENT_DIVERSITY,
    SCN_FACULTY_DIVERSITY,
    SCN_BALANCE_BUDGET,
};

// ###### begin Gilbert 19/04/2001 ###### //
static short *scn_bitmap = NULL;
// ###### end Gilbert 19/04/2001 ###### //
static int old_scn_id;

static void scenario_disp_rep_row(int recNo, int x, int y, int refreshFlag) {
    if(recNo<=TMP_SCENARIO_COUNT)
	font_letter.put(x,y,scn_name[recNo-1]);
}

static void play_scenario_deinit() {
    if(scn_bitmap) {
	mem_del(scn_bitmap);
	scn_bitmap=NULL;
    }
}

void GameSetting::play_scenario_init() {
}

void GameSetting::play_scenario_report(int refreshFlag) {
    int vtab1=REPORT_Y1+5,vtab2=REPORT_Y1+110,vtab3=REPORT_Y1+140,vtab4=REPORT_Y1+360;
    int tab1=REPORT_X1+30,tab2=REPORT_X1+165,tab3=REPORT_X1+405,tab4=REPORT_X1+695;

    vga.use_back();
    sys.yield();

    if(scenario_mode==SCENARIO_SELECT) {
	if (refreshFlag == INFO_REPAINT) {
	    user_interface.bg_img(pic_id, &vga_back);
	    play_scenario_deinit();
	    scr_button_start.create_text
		(REPORT_X1+130, REPORT_Y2+15,
		 REPORT_X1+300, REPORT_Y2+35, "Start scenario");
	    scr_button_cancel.create_text
		(REPORT_X1+400, REPORT_Y2+15,
		 REPORT_X1+570, REPORT_Y2+35, "Back");

	    //			button_up.create_text(tab1+80,vtab1+35,tab1+105,vtab1+60,"^");
	    //			button_down.create_text(tab1+80,vtab1+65,tab1+105,vtab1+90,"v");

	    user_interface.brighten(tab1-5,vtab1-5,tab4,vtab2);
	    user_interface.brighten(tab1-5,vtab3,tab4,vtab4);
	    user_interface.rect(tab1-5,vtab1-5,tab4,vtab2,1);
	    user_interface.rect(tab1-5,vtab3,tab4,vtab4,1);
	    user_interface.rect(tab1,vtab1+20,tab1+115,vtab1+100);

	    font_chart_blue_sm.put(tab1+5,vtab1+5,"Institution logo");
	    font_chart_blue_sm.put(tab2,vtab1+5,"Institution name");
	    font_chart_blue_sm.put(tab2,vtab1+60,"Player first name");
	    font_chart_blue_sm.put(tab2+268,vtab1+60,"Last name");
	    font_chart_blue_sm.put(tab1,vtab3-16,"Institution type");

	    scr2_name[0].init(tab2+5,vtab1+28,tab2+520,player_school.school_name,sizeof(player_school.school_name)-1,&font_charts,0,1);
	    scr2_name[1].init(tab2+5,vtab1+83,tab2+250,player_school.player_first_name,sizeof(player_school.player_first_name)-1,&font_charts,0,1);
	    scr2_name[2].init(tab2+275,vtab1+83,tab2+520,player_school.player_last_name,sizeof(player_school.player_last_name)-1,&font_charts,0,1);
	    scr2_name.set_focus(0);
	    scr2_name[0].paint();
	    scr2_name[1].paint();
	    scr2_name[2].paint();
	    sys.redraw_all_flag=0;
	    vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
	}
	else {
	}

	sys.yield();
	scr_button_start.paint();
	scr_button_cancel.paint();
	vga.blt_buf(REPORT_X1+130, REPORT_Y2+15,
		    REPORT_X1+300, REPORT_Y2+35);
	vga.blt_buf(REPORT_X1+400, REPORT_Y2+15,
		    REPORT_X1+570, REPORT_Y2+35);

	static int old_mascot=-1;
	if(old_mascot!=player_school.mascot) {
	    char* bitmapPtr = image_mascot.get_data(player_school.mascot);
	    if( bitmapPtr ) {
		vga_back.put_bitmap(tab1+10,vtab1+32,bitmapPtr );
	    }

	    image_interface.put_back(tab1+80,vtab1+33,"U-ARW-UP");
	    image_interface.put_back(tab1+80,vtab1+63,"D-ARW-UP");
	    vga.blt_buf(tab1,vtab1+20,tab1+115,vtab1+100);
	}

	sys.yield();
	user_interface.bar(tab2-5,vtab1+22,tab2+520,vtab1+46,V_WHITE);
	user_interface.bar(tab2-5,vtab1+77,tab2+250,vtab1+101,V_WHITE);
	user_interface.bar(tab2+265,vtab1+77,tab2+520,vtab1+101,V_WHITE);
	user_interface.rect(tab2-5,vtab1+22,tab2+520,vtab1+46,1);
	user_interface.rect(tab2-5,vtab1+77,tab2+250,vtab1+101,1);
	user_interface.rect(tab2+265,vtab1+77,tab2+520,vtab1+101,1);
	scr2_name.paint();

	vga.blt_buf(tab2-5,vtab1+22,tab2+520,vtab1+48);
	vga.blt_buf(tab2-5,vtab1+77,tab2+250,vtab1+103);
	vga.blt_buf(tab2+265,vtab1+77,tab2+520,vtab1+103);

	sys.yield();

	static int old_selected_scenario=-1;
	if(selected_scenario!=old_selected_scenario) {
	    image_interface.put_back(tab1+10,vtab3+7,"SCE_PV_F");
	    image_interface.put_back(tab1+175,vtab3+7,"SCE_LB_F");
	    image_interface.put_back(tab1+340,vtab3+7,"SCE_PB_F");
	    image_interface.put_back(tab1+505,vtab3+7,"SCE_CM_F");
	    if(selected_scenario==PRIVATE_SCHOOL)
		image_interface.put_back(tab1+10,vtab3+7,"SCE_PV_N");
	    if(selected_scenario==LIBERAL_ARTS_SCHOOL)
		image_interface.put_back(tab1+175,vtab3+7,"SCE_LB_N");
	    if(selected_scenario==PUBLIC_SCHOOL)
		image_interface.put_back(tab1+340,vtab3+7,"SCE_PB_N");
	    if(selected_scenario==COMPREHENSIVE_SCHOOL)
		image_interface.put_back(tab1+505,vtab3+7,"SCE_CM_N");
	    /*		font_fblack.put_paragraph(tab1+30,vtab4-40,tab1+150,vtab4-10,
			"Private Research University");
			font_fblack.put_paragraph(tab1+195,vtab4-40,tab1+335,vtab4-10,
			"Liberal Arts College");
			font_fblack.put_paragraph(tab1+360,vtab4-40,tab1+480,vtab4-10,
			"Public Research University");
			font_fblack.put_paragraph(tab1+525,vtab4-40,tab1+645,vtab4-10,
			"Comprehensive University");
	    */
	    user_interface.rect(tab1+10,vtab3+7,tab1+160,vtab3+207,1);
	    user_interface.rect(tab1+175,vtab3+7,tab1+325,vtab3+207,1);
	    user_interface.rect(tab1+340,vtab3+7,tab1+490,vtab3+207,1);
	    user_interface.rect(tab1+505,vtab3+7,tab1+655,vtab3+207,1);

	    vga.blt_buf(tab1+10,vtab3+7,tab1+160,vtab3+209);
	    vga.blt_buf(tab1+175,vtab3+7,tab1+325,vtab3+209);
	    vga.blt_buf(tab1+340,vtab3+7,tab1+490,vtab3+209);
	    vga.blt_buf(tab1+505,vtab3+7,tab1+655,vtab3+209);
	}
    }
    else {
	//		sys.yield();
	if (refreshFlag == INFO_REPAINT) {
	    user_interface.bg_img(pic_id, &vga_back);
	    play_scenario_deinit();
	    scr_button_start.create_text
		(REPORT_X1+130, REPORT_Y2+13,
		 REPORT_X1+300, REPORT_Y2+33, "Select scenario");
	    scr_button_cancel.create_text
		(REPORT_X1+400, REPORT_Y2+13,
		 REPORT_X1+570, REPORT_Y2+33, "Back");

	    user_interface.brighten(tab1,vtab1,tab4,vtab4);
	    user_interface.rect(tab1,vtab1,tab4,vtab4,1);
	    //			font_chart_blue_sm.put(tab1+50,vtab1+5,"Scenario");
	    //font_chart_blue_sm.put(tab1+530,vtab1+5,"Difficulty");
	    user_interface.brighten(tab1+20,vtab1+20,
				    tab1+645,vtab1+140);
	    user_interface.rect(  tab1+20,vtab1+20,
				  tab1+645,vtab1+140,1);
	    scenario_vbrowser.init(
		tab1+20,vtab1+20,
		tab1+645,vtab1+140,
		0,  scenario_browseRecHeight,scenario_browseRecCount1,
		scenario_disp_rep_row,NULL,1);
	    //			sys.yield();
	    // find the vbrowse recno of current scenario
		int scRecno;
	    for( scRecno=TMP_SCENARIO_COUNT; scRecno > 1 && scn_id[scRecno-1]!=player_school.scenario_id; --scRecno );
	    err_when( scRecno<1);
	    scenario_vbrowser.open(scRecno);
	    //			sys.yield();
	    scn_bitmap=vga.active_buf->save_area(
		tab1+15,vtab3,tab4-15,vtab4,
		scn_bitmap
		);

	    scr_button_start.paint();
	    scr_button_cancel.paint();
	    vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
	}
	sys.yield();

	if (old_scn_id !=scn_id[scenario_vbrowser.recno() - 1]) {
	    String str;
	    str  = DIR_RES;
	    str += "SCENDESC";
	    ResTxt rs;
	    rs.init(str);

	    int topic_id=rs.locate_topic(m.format(scn_id[scenario_vbrowser.recno() - 1],4));
	    char * textPtr = rs.get_body(topic_id);
	    vga.active_buf->rest_area(scn_bitmap,0,0);
	    font_letter.put_paragraph(tab1+15,vtab3+25,tab4-15,vtab4,textPtr);
	    old_scn_id=scn_id[scenario_vbrowser.recno() - 1];

	    //--- Demo: only the first scenario is available in the demo ---//

#ifdef DEMO
	    if( scenario_vbrowser.recno() > 1 )
		font_chart_red_sm.right_put( tab4-20, vtab3+25, "Not available in the demo version" );
#endif

	    vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
	}
    }
    sys.yield();
}

//------ Begin of function GameSetting::detect ------//
//!
//! return: <int> 1  - the Start button is pressed.
//! 				  -1 - the Cancel button is pressed.
//!					  0  - neither of them is pressed.
//!
int GameSetting::play_scenario_detect() {
    int rc=0;
    vga.use_back();

    int vtab1=REPORT_Y1+5,vtab2=REPORT_Y1+110,vtab3=REPORT_Y1+140,vtab4=REPORT_Y1+355;
    int tab1=REPORT_X1+30,tab2=REPORT_X1+165,tab3=REPORT_X1+405,tab4=REPORT_X1+695;
    vga.use_back();

    sys.yield();
    if(scenario_mode==SCENARIO_SELECT) {
	if(mouse.single_click(tab1+10,vtab3+7,tab1+160,vtab3+207)) {
	    player_school.school_type=selected_scenario=PRIVATE_SCHOOL;
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	}

	if(mouse.single_click(tab1+175,vtab3+7,tab1+325,vtab3+207)) {
	    player_school.school_type=selected_scenario=LIBERAL_ARTS_SCHOOL;
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	}

	if(mouse.single_click(tab1+340,vtab3+7,tab1+490,vtab3+207)) {
	    player_school.school_type=selected_scenario=PUBLIC_SCHOOL;
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	}
	if(mouse.single_click(tab1+505,vtab3+7,tab1+655,vtab3+207)) {
	    player_school.school_type=selected_scenario=COMPREHENSIVE_SCHOOL;
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	}

	//		if(button_up.detect())
	//		if(mouse.single_click(tab1+80,vtab1+35,tab1+105,vtab1+60))
	// switch to any_click in version 2
	if(mouse.any_click(tab1+80,vtab1+35,tab1+105,vtab1+60)) {
	    do {
		player_school.mascot--;
		if(player_school.mascot<1)
		    player_school.mascot=image_mascot.rec_count;
	    } while( player_school.is_hidden_mascot(player_school.mascot) );
	    image_interface.put_front(tab1+80,vtab1+33,"U-ARW-DN");
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	}

	//		if(button_down.detect())
	//		if(mouse.single_click(tab1+80,vtab1+65,tab1+105,vtab1+90))
	// switch to any_click in version 2
	if(mouse.any_click(tab1+80,vtab1+65,tab1+105,vtab1+90)) {
	    do {
		player_school.mascot++;
		if(player_school.mascot>image_mascot.rec_count)
		    player_school.mascot=1;
	    } while( player_school.is_hidden_mascot(player_school.mascot) );
	    image_interface.put_front(tab1+80,vtab1+63,"D-ARW-DN");
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	}

	scr2_name.detect();

	vga.use_front();
	if( scr_button_start.detect() ) {
	    rc = 1;
	    scenario_mode=SCENARIO_DESC;
	    selected_scenario = PRIVATE_SCHOOL;         // reset the selected_scenario
	    old_scn_id=-1;
	}

	if( scr_button_cancel.detect() ) {
	    rc = 2;
	    sys.redraw_all_flag=1;
	    scenario_mode=SCENARIO_DESC;
	    old_scn_id=-1;
	}
	vga.use_back();
    }
    else {
	vga.use_front();

	if(scenario_vbrowser.detect()) {
#ifdef DEMO
	    if( scenario_vbrowser.recno() > 1 )
		scr_button_start.disable();
	    else
		scr_button_start.enable();
	    return 0;
#endif
	}

	if( scr_button_start.detect() ) {
	    player_school.scenario_id=scn_id[scenario_vbrowser.recno() - 1];
	    rc = 2;
	    sys.redraw_all_flag=1;
	    scenario_mode=SCENARIO_SELECT;
	    old_scn_id=-1;
	}
	sys.yield();

	if( scr_button_cancel.detect() ) {
	    old_scn_id=-1;
	    rc = -1;
	}
	vga.use_back();
    }
    return rc;
}

//------- End of function GameSetting::detect ------//
GameSetting::~GameSetting() {
    new_game_deinit();
    play_scenario_deinit();
}
