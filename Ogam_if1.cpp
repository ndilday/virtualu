//Filename    : OGAM_IF1.H
//Description : Game Class Definition (Play a Scenario)
//Owner       : Fred

#include <OSYS.H>
#include <OLOG.H>
#include <OMOUSE.H>
#include <OVGA.H>

#include <OWORLDMT.H>                             // ZOOM_Y1, etc
#include <OIFACE.H>
#include <OGAME.H>
#include <OGFILE.H>

#include <OVBROWSE.H>

#include <OAUDIO.H>
#include <OCONFIG.H>

///*
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>

#include <OGAMESET.H>
#include <OGSET.H>

#include <ORADIO.H>
#include <OTABMENU.H>
#include <OSLIDER.H>
#include <OVALSLDR.H>

#include <OINFO.H>
#include <OSPINNER.H>
#include <OBNDSLDR.H>
#include <OBOX.H>

#include <OPSCHOOL.H>
#include <OFINANCE.H>
//*/

#define DEPARTMENT_TYPE_COUNT 28

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

static int current_report_mode=TAB_SCREEN;
static int button_state = ALL_POP;
static int button_timing = 0;

//static TabMenu tab_menu;
static int tab_sel;
static char *tab_short_desc[] = {
    "Express games",
    "Basic options",
    "Advanced options I",
    "Advanced options II",
};

static Button button_start,button_cancel;
static char * button_label[]={"Start scenario","Cancel"};

static Button button_left,button_right,button_add,button_remove,button_reset,button_addall;

static char screen_init_flag=0;
static char redraw_browser_flag=0;
static int pic_id=9;

// Screen Basic Option
static VBrowse scr2_vbrowser1;
static VBrowse scr2_vbrowser2;
static int scr2_browseRecCount1=1;
static int scr2_browseRecCount2=1;
static int scr2_browseRecHeight=20;
static void scr2_disp_rep_row1(int,int,int,int);
static void scr2_disp_rep_row2(int,int,int,int);
// ##### begin Gilber 19/04/2001 ####//
// static short* button_refresh_bitmap;
static short* button_refresh_bitmap = NULL;
// ##### end Gilber 19/04/2001 ####//
static GetAGroup scr2_name(3);
/*
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
*/

void Game::scenario_deinit() {
    if(button_refresh_bitmap) {
	mem_del(button_refresh_bitmap);
	button_refresh_bitmap=NULL;
    }

    scr2_browseRecCount1=1;
    scr2_browseRecCount2=1;

    screen_init_flag = 0;
}

///////////////////////////////////////////////////////////////////////////////////
//
static int callback_disp(int selNum, int refreshFlag) {

    if(selNum==1) {
	int vtab1=TMENU_Y1+15,vtab2=TMENU_Y1+60,vtab3=TMENU_Y1+120,vtab4=TMENU_Y1+320;
	int vtab5=TMENU_Y1+280,vtab6=TMENU_Y1+220,vtab7=TMENU_Y1+255,vtab8=TMENU_Y1+290;
	int tab1=TMENU_X1+15,tab2=TMENU_X1+90,tab3=TMENU_X1+390,tab4=TMENU_X1+680;

	if(refreshFlag== INFO_REPAINT) {
	    //---init. of screen 2 -//
	    game.scenario_deinit();

	    button_refresh_bitmap=
		vga_back.save_area(tab1,vtab1+65,tab1+60,vtab1+95);
	    button_left.create_text(tab1,vtab1+70,tab1+25,vtab1+90,"<");
	    button_right.create_text(tab1+35,vtab1+70,tab1+60,vtab1+90,">");

	    //button_add.create_text(tab1+285,vtab3+60,tab1+385,vtab3+80,"Add >>");
	    //button_remove.create_text(tab1+285,vtab3+100,tab1+385,vtab3+120,"<< Remove ");
	    //button_reset.create_text(tab1+285,vtab3+140,tab1+385,vtab3+160,"Reset");

	    //font_chart_blue_sm.put(tab2,vtab1,"Institution name");
	    font_chart_blue_sm.put_paragraph(tab2,vtab1+55,tab2+80,vtab1+100,"Player first name");
	    font_chart_blue_sm.put(tab3,vtab1+65,"Last name");
	    //font_charts.put(tab1+40,vtab3,"Nonactive department");
	    //font_charts.put(tab1+460,vtab3,"Active department");

	    // Basic options --//
	    scr2_name[0].init(tab2+15,vtab1+25,tab2+600,player_school.school_name,sizeof(player_school.school_name)-1,&font_chartsm,0,1);
	    scr2_name[1].init(tab2+80,vtab1+65,tab2+300,player_school.player_first_name,sizeof(player_school.player_first_name)-1,&font_chartsm,0,1);
	    scr2_name[2].init(tab3+80,vtab1+65,tab3+300,player_school.player_last_name,sizeof(player_school.player_last_name)-1,&font_chartsm,0,1);
	    scr2_name.set_focus(0);
	    scr2_name[0].paint();
	    scr2_name[1].paint();
	    scr2_name[2].paint();
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

	scr2_name.paint();
    }

    //	vga.blt_buf(TMENU_X1+5,TMENU_Y1+5,TMENU_X2-5,TMENU_Y2-5,0);
    return 0;
}

static int callback_detect(int selNum) {
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
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	}

	if(button_right.detect()) {
	    do {
		player_school.mascot++;
		if(player_school.mascot>image_mascot.rec_count)
		    player_school.mascot=1;
	    } while( player_school.is_hidden_mascot(player_school.mascot) );
	    vga_back.rest_area(button_refresh_bitmap,0,0);
	    image_interface.put_front(tab1+35,vtab1+70,"R-ARW-DN");
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	}

	scr2_name.detect();

	//vga.use_front();		// ## use_front to make the button flashing in release mode.

	//vga.use_back();
    }

    return 0;
}

/*
static void scr2_disp_rep_row1(int recNo, int x, int y, int refreshFlag)
{
  user_interface.bar(x, y, x+240, y+16,Vga::active_buf->color_down);
  for(int i=0,j=0;(i<recNo)&&(j<DEPARTMENT_TYPE_COUNT);j++)
    if(tmp_department_pool_flag[j]==1)
        i++;
//	err.run("department_res.department_count=%d\n",department_res.department_count);
  if(j<=department_res.department_count)
    font_chartsm.put(x,y,department_res[j]->name);
}
static void scr2_disp_rep_row2(int recNo, int x, int y, int refreshFlag)
{
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
*/

//-----------------------------------------------------------------//

enum {                                            // screen coordinates
    WIN_X1 = REPORT_X1+15,
    WIN_Y1 = REPORT_Y1+25,
    WIN_X2 = REPORT_X2-15,
    WIN_Y2 = REPORT_Y2-25
};

static char *rbuttion_label[] = {
    "Private research university",
    "Private liberal arts college",
    "Public research university",
    "Public comprehensive university",
};

static int browser_rec_size;
static VBrowse game_browser;

static void disp_load_game_rec(int recNo, int x, int y, int refreshFlag) {
    if((recNo>0)&&(recNo<=game_file_array.size())) {
	GameFile* gfPtr = game_file_array[recNo];
	char* name = gfPtr->get_save_name();
	font_chartsm.put(x,y,name);
    }
};

//------- Begin of function Game::scenario_init ------//
//!
//!------- End of function Game::scenario_init ------//

//------- Begin of function Game::scenario_init ------//
//!
void Game::scenario_init() {
    screen_init_flag = 0;

    game_file_array.init("*.SAV");
    browser_rec_size=game_file_array.size();
}

//------- End of function Game::scenario_report ------//

//------- Begin of function Game::scenario_report ------//
//!
void Game::scenario_report(int refreshFlag) {
    static const int  tab1=WIN_X1+10,
	tab2=WIN_X1+410,
	vtab0=WIN_Y1+10,
	vtab1=WIN_Y1+60,
	vtab2=WIN_Y2-30;

    vga.use_back();

    if(current_report_mode==TAB_SCREEN) {
	if (refreshFlag == INFO_REPAINT) {
	    user_interface.bg_img(pic_id, &vga_back);
	    font_charts.put(tab1+290, vtab0,"Scenario Games");

	    if (!screen_init_flag) {
		button_start.create_text
		    (REPORT_X1+150, REPORT_Y2+15,
		     REPORT_X1+300, REPORT_Y2+35, button_label[0]);
		button_cancel.create_text
		    (REPORT_X1+400, REPORT_Y2+15,
		     REPORT_X1+550, REPORT_Y2+35, button_label[1]);
		screen_init_flag = 1;
	    }
	    else {

	    }

	    game_browser.init( tab1+100, vtab1+10, tab1+550, vtab2-20,
			       0, 18, browser_rec_size, disp_load_game_rec );
	    game_browser.open(game_browser.recno());
	}
	else {
	}

	button_start.paint();
	button_cancel.paint();
    }

    vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);      // DON'T need to blt all area...
}

//------- End of function GameSetting::report ------//

//------ Begin of function GameSetting::detect ------//
//!
//! return: <int> 1  - the Start button is pressed.
//! 				  -1 - the Cancel button is pressed.
//!					  0  - neither of them is pressed.
//!
int Game::scenario_detect() {
    int rc=0, rec=0;
    vga.use_back();

    if(current_report_mode==TAB_SCREEN) {
	vga.use_front();

	if( button_start.detect() ) {
	    if ( game_file_array.size()>=game_browser.recno() && game_browser.recno()>0) {
		rec = game_browser.recno();
		//0127 game_file_array[game_browser.recno()]->load_game();

		sys.redraw_all_flag=1;
		//menu_pull_down = 0;
		sys.set_staying_view_mode(0);
		sys.redraw_all_flag=1;
		rc = 1;                                   //BUGHERE should be the scenId
	    }

	}

	if( button_cancel.detect() )
	    rc = -1;

	vga.use_back();                               // why do this here? before vbrowse??

	if(rec=game_browser.detect()) {
	    game_browser.refresh(rec);
	    if(game_browser.double_click) {
		if (game_file_array.size()>=rec && rec>0) {
		    //0127 game_file_array[rec]->load_game();

		    sys.redraw_all_flag=1;
		    //menu_pull_down = 0;
		    sys.set_staying_view_mode(0);           // 0: restore last saved mode which is not "staying"
		    sys.redraw_all_flag=1;
		    rc = 1;
		    // 0127 					return 1;				//BUGHERE should be the scenId
		}
	    }
	}
    }
    vga.use_back();

    if (rc>=1)
	return rec;
    else
	return rc;
}

//------- End of function GameSetting::detect ------//
