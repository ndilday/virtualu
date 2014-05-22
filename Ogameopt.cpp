// Functions : Game options interface
// Flies : Ogameopt.cpp
// Owner: chwg1012

#include <KEY.H>
#include <Osys.h>
#include <OSTR.H>
#include <OMISC.H>
#include <OMUSIC.H>
#include <OGAMESET.H>
#include <OAUDIO.H>
#include <OWORLDMT.H>
#include <ODEPT.H>
#include <OVGA.H>
#include <OCONFIG.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <OBUTTON.H>
#include <OGETA.H>
#include <OVBROW98.h>
#include <OVGALOCK.H>
#include <OVALSLDR.H>
#include <OPSCHOOL.H>
#include <OBOX.H>
#include <OGFILE.H>                               //## fred 1109

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-25
};

enum {                                            // screen coordinates
    WIN_X1 = REPORT_X1+15,
    WIN_Y1 = REPORT_Y1+25,
    WIN_X2 = REPORT_X2-15,
    WIN_Y2 = REPORT_Y2-25
};

enum{SAVE_GAME,LOAD_GAME,LOG_OUTPUT_NOW,RETIRE,QUIT_TO_MAIN_MENU,QUIT_TO_WINDOW,CONTINUE_GAME,NONE,GO_BUTTON_COUNT=NONE};
static Button go_button[GO_BUTTON_COUNT];
//static short * go_button_up_bitmap[6];
//static short * go_button_down_bitmap[6];
static char * go_button_label[GO_BUTTON_COUNT]= {
    "Save Game",
    "Load Game",
    "Log Output Now",
    "Retire",
    "Quit to Main Menu",
    "Quit to Windows",
    "Continue Game"
};

enum{SCR_TOP,SCR_SAVE_GAME,SCR_LOAD_GAME};

static Button ar_button[2];
static Button help_button[2];
static Button music_button[2];
static Button threshold_event_button[2];
static Button building_label_button[2];
//static short * ar_button_up_bitmap[2];
//static short * ar_button_down_bitmap[2];
static short * background_bitmap=NULL;

static Button save_button;
static Button load_button;
static Button delete_button;
static Button cancel_button;
static Button browse_button;
static VBrowse98 game_browser;

static ValueSlider vslider[3];
static int current_option=NONE;
static int current_screen_type=SCR_TOP;

static void disp_load_game_rec(int,int,int,int);
static void disp_save_game_rec(int,int,int,int);

static int redraw_scr_save_browser;
static GetA game_name;
static String game_name_str;

static char * go_label[]= {
    "Sound Effects Volume",
    "Music Volume",
    "Game Speed",
    "Help",
    "Annual Report",
    "Threshold Events",
    "Building Labels",
};

static char * help_label[2]= {
    "On",
    "Off",
};

static char * ar_label[2]= {
    "On",
    "Off",
};

static int v[3];
static int browser_rec_size;

void free_button_memory(void);

void Sys::setting_report(int refreshFlag) {
    static int  tab1=WIN_X1+10,
	tab2=WIN_X1+410,
	vtab0=WIN_Y1+10,
	vtab1=WIN_Y1+60,
	vtab2=WIN_Y2-30;

    //   vga.use_back();
    //------ begin : Top Screen of game options --//
    if(current_screen_type==SCR_TOP) {
	int i,j;
	if (refreshFlag == INFO_REPAINT) {

	    v[2]=(sys.save_speed+2)/10;
	    //    		v[2]=(config.frame_speed+2)/10;
	    v[1]=config.cd_music_volume/10;
	    v[0]=config.sound_effect_volume/10;
	    if(!background_bitmap) {
		user_interface.darken(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);
		info.disp_column_bitmap(&vga_back);
		user_interface.brighten(WIN_X1,WIN_Y1,WIN_X2,WIN_Y2);
		user_interface.rect(WIN_X1,WIN_Y1,WIN_X2,WIN_Y2);
		background_bitmap=vga_back.save_area
		    (ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2,background_bitmap);
	    }
	    else
		vga_back.rest_area(background_bitmap,0,0);

	    free_button_memory();

	    const int rv0 = vtab1;
	    const int rvi = 36;
	    const int rvh = 28;

	    const int lv0 = vtab1;
	    const int lvi = 36;
	    const int lvh = 22;

	    for(i=0;i<GO_BUTTON_COUNT;i++) {
		go_button[i].create_text(
		    tab2, rv0+i*rvi,                        // vtab1+i*40,
		    tab2+250, rv0+i*rvi+rvh,                // vtab1+i*40+30,
		    go_button_label[i] );
	    }

	    for(i=0;i<2;i++) {
		music_button[i].create_text(
		    tab1+200+i*50, lv0+1*lvi,               // vtab1+55,
		    tab1+240+i*50, lv0+1*lvi+lvh,           // vtab1+55+22,
		    ar_label[i]
		    );
		help_button[i].create_text(
		    tab1+200+i*50, lv0+3*lvi,               // vtab1+135,
		    tab1+240+i*50, lv0+3*lvi+lvh,           // vtab1+135+22,
		    help_label[i]
		    );
		ar_button[i].create_text(
		    tab1+200+i*50, lv0+4*lvi,               // vtab1+175,
		    tab1+240+i*50, lv0+4*lvi+lvh,           // vtab1+175+22,
		    ar_label[i]
		    );
		threshold_event_button[i].create_text(
		    tab1+200+i*50, lv0+5*lvi,
		    tab1+240+i*50, lv0+5*lvi+lvh,
		    ar_label[i]
		    );
		building_label_button[i].create_text(
		    tab1+200+i*50, lv0+6*lvi,
		    tab1+240+i*50, lv0+6*lvi+lvh,
		    ar_label[i]
		    );
	    }

	    font_charts.put(tab1+280, vtab0,"Game Menu");

	    for(j=0;j<7;j++)
		{
		    font_chartsm.put(
			tab1+25, lv0+j*lvi+5,                   // vtab1+j*40+20,
			go_label[j]
			);
		}

	    for(j=0;j<3;j+=2) {
		vslider[j].init(
		    tab1+200, lv0+j*lvi+2,                  // vtab1+j*40+17,
		    150,20,0, 10, &v[j], 1, V_YELLOW, 4);
		//				vslider[j].paint();
	    }
	}

	for(i=0;i<GO_BUTTON_COUNT;i++)
	    go_button[i].paint();

	for(i=0;i<2;i++) {
	    if(i == (1-config.cd_music_volume))
		music_button[i].push();
	    else
		music_button[i].pop();
	    music_button[i].paint();
	}

	for(i=0;i<2;i++) {
	    if(i == (1-config.context_sensitive_help))
		help_button[i].push();
	    else
		help_button[i].pop();
	    help_button[i].paint();
	}

	for(i=0;i<2;i++) {
	    if(i == (1-config.disp_year_end_report))
		ar_button[i].push();
	    else
		ar_button[i].pop();
	    ar_button[i].paint();
	}
	for(i=0;i<2;i++) {
	    if( i == (1-player_school.threshold_event_flag))
		threshold_event_button[i].push();
	    else
		threshold_event_button[i].pop();
	    threshold_event_button[i].paint();
	}

	for(i=0;i<2;i++) {
	    if( i == (1-config.disp_building_label))
		building_label_button[i].push();
	    else
		building_label_button[i].pop();
	    building_label_button[i].paint();
	}

	for(i=0;i<3;i+=2)
	    vslider[i].refresh();

	// the report does not belong to any of the buttons
	vga_back.put_bitmap(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, info.report_bitmap[0]);
	//		user_interface.darken(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, REPORT_BUTTON_X2, REPORT_BUTTON_Y2-1);
	//		vga.blt_buf(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, REPORT_BUTTON_X2, REPORT_BUTTON_Y2-1);
    }
    //------ begin : Save game Screen of game options --//
    if( current_screen_type==SCR_SAVE_GAME) {
	if (refreshFlag == INFO_REPAINT || redraw_scr_save_browser) {
	    vga_back.rest_area(background_bitmap,0,0);
	    user_interface.bar(tab1+200,vtab1-5,tab1+550,vtab1+22,V_WHITE);
	    user_interface.rect(tab1+200,vtab1-5,tab1+550,vtab1+22);

	    font_charts.put(tab1+290, vtab0,"Save Game");
	    save_button.create_text(tab1+150,vtab2,tab1+220,vtab2+20,"Save");
	    delete_button.create_text(tab1+300,vtab2,tab1+370,vtab2+20,"Delete");
	    cancel_button.create_text(tab1+450,vtab2,tab1+520,vtab2+20,"Close");

	    user_interface.brighten(tab1+100, vtab1+35, tab1+550, vtab2-20);
	    user_interface.rect(tab1+100, vtab1+35, tab1+550, vtab2-20);

	    game_browser.init( tab1+100, vtab1+35, tab1+550, vtab2-20,
			       0, 18, browser_rec_size, disp_save_game_rec );
	    game_browser.open(game_browser.recno());

	    font_chartsm.put(tab1+100,vtab1,"Game name");
	    if(!redraw_scr_save_browser)
		game_name.init(tab1+210,vtab1+2,tab1+550,game_name_str.str_buf,18,&font_charts,0,1);
	    redraw_scr_save_browser=0;
	}
	//    	int rc=game_browser.recno();
	//    	game_browser.update();
	user_interface.bar(tab1+200,vtab1-5,tab1+550,vtab1+22,V_WHITE);
	user_interface.rect(tab1+200,vtab1-5,tab1+550,vtab1+22);
	game_name.paint();

	save_button.paint();
	delete_button.paint();
	cancel_button.paint();
    }

    //------ begin : Load game Screen of game options --//
    if( current_screen_type==SCR_LOAD_GAME) {
	if (refreshFlag == INFO_REPAINT) {
	    vga_back.rest_area(background_bitmap,0,0);
	    font_charts.put(tab1+290, vtab0,"Load Game");
	    load_button.create_text(tab1+130,vtab2,tab1+200,vtab2+20,"Load");
	    delete_button.create_text(tab1+240,vtab2,tab1+310,vtab2+20,"Delete");
	    cancel_button.create_text(tab1+350,vtab2,tab1+420,vtab2+20,"Close");
	    browse_button.create_text(tab1+460,vtab2,tab1+530,vtab2+20,"Browse");

	    user_interface.brighten(tab1+100, vtab1+10, tab1+550, vtab2-20);
	    user_interface.rect(tab1+100, vtab1+10, tab1+550, vtab2-20);
	    game_browser.init( tab1+100, vtab1+10, tab1+550, vtab2-20,
			       0, 18, browser_rec_size, disp_load_game_rec );
	    game_browser.open(game_browser.recno());
	}

	//    	int rc=game_browser.recno();
	//    	game_browser.update();

	load_button.paint();
	delete_button.paint();
	cancel_button.paint();
	browse_button.paint();
    }

    vga.blt_buf(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);
    //  vga.use_front();
};

//------ Start of function Sys::setting_report_detect -----//

int Sys::setting_report_detect() {
    //
    if( current_screen_type==SCR_TOP) {
	for(int i=0;i<3;i+=2) {
	    if(vslider[i].detect()) {
		v[i]=(*vslider[i].var_ptr);

		if(i==0) {
		    config.sound_effect_volume=v[0]*10;
		    audio.play_wav("BEEPS-TI",audio.int_to_DsVolume(config.sound_effect_volume));
		    audio.set_wav_volume(v[0]*10);
		}

		// ## begin zb 02/21/99
		if(i==2)
		    sys.save_speed=v[2]*98/10;
		// ## end zb 02/21/99

		return 1;
	    }
	}

	vga.use_front();
	//    	for(i=0;i<6;i++)
	//			if(go_button_up_bitmap[i])
	//	    		go_button[i].body_ptr = (void *) (go_button_up_bitmap[i] + 4);

	for(int i=0;i<GO_BUTTON_COUNT;i++)
	    if(go_button[i].detect()) {
		current_option=i;
		//				if(go_button_down_bitmap[i])
		//  				go_button[current_option].body_ptr = (void *) (go_button_down_bitmap[current_option] + 4);

		if(current_option==CONTINUE_GAME) {
		    menu_pull_down = 0;
		    set_staying_view_mode(0);
		    sys.redraw_all_flag=1;
		    //## fred 0213
		    if ( config.cd_music_volume <=0 )
			music.stop();
		    //## fred 0213
		}

		if(current_option==QUIT_TO_WINDOW) {
		    int rc;
		    rc=box.ask("Quit to Windows now?  ");
		    if(rc==1)
			sys.signal_exit_flag = 1;
		    else
			sys.redraw_all_flag = 1;
		}

		if(current_option==QUIT_TO_MAIN_MENU) {
		    int rc;
		    rc=box.ask("Quit to Main Menu now?  ");
		    if(rc==1) {
			sys.signal_exit_flag = 2;
			menu_pull_down = 0;
			set_staying_view_mode(0);
			sys.set_speed(DEFAULT_SPEED);
		    }
		    else
			sys.redraw_all_flag = 1;
		}

		if(current_option==RETIRE) {
		    int rc;
		    rc=box.ask("Retire now?  ");
		    if(rc==1) {
			/* 					sys.signal_exit_flag = 2;
								menu_pull_down = 0;
								set_staying_view_mode(0);
								sys.set_speed(DEFAULT_SPEED);
			*/
			sys.set_staying_view_mode(MODE_RETIREMENT);
			//## fred 0213
			//						game_file_array.add_hall_of_fame(player_school.ultimate_game_score);

			if ( config.cd_music_volume <=0 )
			    music.stop();
			//## fred 0213
		    }
		    else
			sys.redraw_all_flag = 1;
		}

		if(current_option==SAVE_GAME) {
		    game_file_array.init("*.SAV");            //## fred

		    sys.redraw_all_flag=1;
		    current_screen_type=SCR_SAVE_GAME;
		    game_name_str=player_school.player_last_name;
		    browser_rec_size=game_file_array.size();
		}

		if(current_option==LOAD_GAME) {
		    sys.redraw_all_flag=1;
		    current_screen_type=SCR_LOAD_GAME;
		    game_file_array.init("*.SAV");            //## fred
		    browser_rec_size=game_file_array.size();
		}

		if( current_option==LOG_OUTPUT_NOW ) {
		    // if config.log_enable, write to log file
		    if( config.log_enable && config.log_file_name[0] ) {
			game_file.append_game_log( config.log_file_name );
		    }
		    else {
			// if not ask file to write or append

			BOOL retValue;
			char writeLogFile[MAX_PATH];
			writeLogFile[0] = '\0'; {
			    MouseDispCount mouseDispCount;        // redisplay windows cursor

			    // ask filename now
			    OPENFILENAME openFileName;
			    memset( &openFileName, 0, sizeof(openFileName) );
			    openFileName.lStructSize = sizeof(openFileName);
			    openFileName.hwndOwner = main_hwnd;
			    openFileName.hInstance = app_hinstance;
			    // two '\0' at the end
			    openFileName.lpstrFilter = "Log file(*.log)\0*.log\0All Files(*.*)\0*.*\0";
			    openFileName.lpstrCustomFilter = NULL;
			    openFileName.nMaxCustFilter = 0;
			    openFileName.nFilterIndex = 0;
			    openFileName.lpstrFile =  writeLogFile;
			    openFileName.nMaxFile = sizeof(writeLogFile);
			    openFileName.lpstrFileTitle = NULL;   // result filename, without path
			    openFileName.nMaxFileTitle = 0;
			    openFileName.lpstrInitialDir = ".";   // current directory
			    openFileName.lpstrTitle = NULL;
			    // no OFN_OVERWRITEPROMPT, append
			    openFileName.Flags = OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
			    openFileName.nFileOffset = 0;
			    openFileName.nFileExtension = 0;
			    openFileName.lpstrDefExt = "log";
			    openFileName.lCustData = NULL;
			    openFileName.lpfnHook = NULL;
			    openFileName.lpTemplateName = NULL;

			    retValue = GetSaveFileName(&openFileName);
			}                                       // call mouseDispCount destructor

			if( retValue && writeLogFile[0] ) {
			    if( !m.is_file_exist(writeLogFile) )
				// overwrite old log file
				game_file.create_game_log(writeLogFile);

			    game_file.append_game_log(writeLogFile);
			}
		    }
		}

		current_option=NONE;
		return 1;
	    }

	for(int i=0;i<2;i++) {
	    if(help_button[i].detect()) {
		config.context_sensitive_help=1-i;
		help_button[i].push();
		help_button[1-i].pop();
		return 1;
	    }
	}

	for(int i=0;i<2;i++) {
	    if(ar_button[i].detect()) {
		config.disp_year_end_report=1-i;
		ar_button[i].push();
		ar_button[1-i].pop();
		return 1;
	    }
	}

	for(int i=0;i<2;i++) {
	    if( threshold_event_button[i].detect() ) {
		player_school.threshold_event_flag = 1-i;
		threshold_event_button[i].push();
		threshold_event_button[1-i].pop();
		return 1;
	    }
	}
	for(int i=0;i<2;i++) {
	    if( building_label_button[i].detect() ) {
		config.disp_building_label = 1-i;
		building_label_button[i].push();
		building_label_button[1-i].pop();
		return 1;
	    }
	}

	for(int i=0;i<2;i++)
	    if(music_button[i].detect()) {
		config.cd_music_volume=1-i;

		if( config.cd_music_volume ) {
		    if( music.is_playing() ) {
			music.change_volume(config.cd_music_volume);
		    }
		}
		else
		    music.stop();

		music_button[i].push();
		music_button[1-i].pop();
		return 1;
	    }
	vga.use_back();
    }

    if( current_screen_type==SCR_LOAD_GAME) {
	vga.use_front();

	//---- detect load button --------//

	if ( load_button.detect() ) {
	    if ( game_file_array.size()>=game_browser.recno() && game_browser.recno()>0) {
		load_game_now(game_browser.recno());
		return 1;
	    }
	}

	if ( delete_button.detect() ) {
	    if(game_browser.recno()>0) {
		//##  BUGHERE fred 1112 ##/
		game_file_array.del_game(game_browser.recno());
		browser_rec_size=game_file_array.size();
		sys.redraw_all_flag=1;
	    }
	}

	if(cancel_button.detect()) {
	    sys.redraw_all_flag=1;
	    current_screen_type=SCR_TOP;
	    return 1;
	}

	if( browse_button.detect() ) {
	    char loadFileName[MAX_PATH] = "";
	    BOOL retValue = FALSE; {
		MouseDispCount mouseDispCount;            // redisplay windows cursor

		// ask filename now
		OPENFILENAME openFileName;
		memset( &openFileName, 0, sizeof(openFileName) );
		openFileName.lStructSize = sizeof(openFileName);
		openFileName.hwndOwner = sys.main_hwnd;
		openFileName.hInstance = sys.app_hinstance;
		// 2 '\0' at the end
		openFileName.lpstrFilter = "All Files(*.*)\0*.*\0";
		openFileName.lpstrCustomFilter = NULL;
		openFileName.nMaxCustFilter = 0;
		openFileName.nFilterIndex = 0;
		openFileName.lpstrFile =  loadFileName;   // first char is already NULL
		openFileName.nMaxFile = sizeof(loadFileName);
		openFileName.lpstrFileTitle = NULL;       // result filename, without path
		openFileName.nMaxFileTitle = 0;
		openFileName.lpstrInitialDir = ".";       // current directory
		openFileName.lpstrTitle = NULL;
		openFileName.Flags = OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		openFileName.nFileOffset = 0;
		openFileName.nFileExtension = 0;
		openFileName.lpstrDefExt = "001";
		openFileName.lCustData = NULL;
		openFileName.lpfnHook = NULL;
		openFileName.lpTemplateName = NULL;

		retValue = GetOpenFileName(&openFileName);
	    }                                           // call mouseDispCount destructor

	    if( retValue ) {
		game_file.load_game(loadFileName);
		load_game_now(0);                         // game already loaded, don't load again
		return 1;
	    }
	}

	vga.use_back();

	if(int rc=game_browser.detect()) {
	    game_browser.refresh(rc);
	    if(game_browser.double_click) {
		if (game_file_array.size()>=rc && rc>0) {
		    load_game_now(rc);
		    return 1;
		}
	    }
	}
    }

    if( current_screen_type==SCR_SAVE_GAME) {
	vga.use_front();

	if( game_name.detect()==KEY_RETURN || save_button.detect() ) {
	    String str = game_name_str;
	    if( strchr( str, '.' ) == NULL )            // if overwriting auto save, don't append .sav
		str+=".sav";

	    game_file_array.save_new_game(str);
	    browser_rec_size=game_file_array.size();
	    //			game_browser.refresh();
	    //			redraw_scr_save_browser=1;
	    sys.redraw_all_flag=1;
	    current_screen_type=SCR_TOP;

	    menu_pull_down = 0;
	    set_staying_view_mode(0);
	    sys.redraw_all_flag=1;
	    //## fred 0213
	    if ( config.cd_music_volume <=0 )
		music.stop();
	    return 1;
	}

	if ( delete_button.detect() ) {
	    if(game_browser.recno()>0) {
		//## BUGHERE fred 1112 ##/
		game_file_array.del_game(game_browser.recno());
		browser_rec_size=game_file_array.size();
		sys.redraw_all_flag=1;
	    }
	}

	if(cancel_button.detect()) {
	    sys.redraw_all_flag=1;
	    current_screen_type=SCR_TOP;
	    return 1;
	}
	vga.use_back();

	if(int rc=game_browser.detect()) {
	    game_browser.refresh(rc);
	    //			if(game_browser.double_click)
	    //			{
	    game_name_str=game_file_array[rc]->get_save_name();
	    game_name.select_whole();
	    game_browser.refresh();
	    //			}
	}

    }

    return 0;
}

//-------- Start of function Sys::load_game_now --------//

void Sys::load_game_now(int gameBrowseRecno) {
    if( gameBrowseRecno )                           // now gameBrowseRecno can be zero
	game_file_array[gameBrowseRecno]->load_game();

    set_staying_view_mode(0);
    view_mode=MODE_NORMAL;
    prev_view_mode = MODE_NORMAL;
    back_view_mode = MODE_NORMAL;
    is_institution_report=0;
    menu_pull_down=0;
    redraw_all_flag = 1;
    redraw_zoom_flag = 0;
    set_speed(save_speed);
    m_o_log_selected=0;
    m_o_log_highlighted=-1;
    m_o_log_old_selected=-2;
    m_o_log_old_highlighted=-2;
    m_o_log_display_updated=1;
    config.scroll_speed=10;
    current_screen_type=SCR_TOP;
    redraw_all_flag=1;
}

//-------- End of function Sys::load_game_now --------//

void free_button_memory(void) {
    /*   for(int i=0;i<6;i++)
	 {
	 if(go_button_up_bitmap[i])
	 {
	 mem_del(go_button_up_bitmap[i]);
	 go_button_up_bitmap[i]=NULL;
	 }
	 if(go_button_down_bitmap[i])
	 {	mem_del(go_button_down_bitmap[i]);
	 go_button_down_bitmap[i]=NULL;
	 }
	 }
	 for(i=0;i<2;i++)
	 {
	 if(ar_button_up_bitmap[i])
	 {
	 mem_del(ar_button_up_bitmap[i]);
	 ar_button_up_bitmap[i]=NULL;
	 }
	 if(ar_button_down_bitmap[i])
	 {
	 mem_del(ar_button_down_bitmap[i]);
	 ar_button_down_bitmap[i]=NULL;
	 }
	 }
    */
    // #### begin Gilbert 02/05/2001 ####//
    for( int i = 0; i < GO_BUTTON_COUNT; ++i ) {
	go_button[i].deinit();
    }
    music_button[0].deinit();
    music_button[1].deinit();
    help_button[0].deinit();
    help_button[1].deinit();
    ar_button[0].deinit();
    ar_button[1].deinit();
    threshold_event_button[0].deinit();
    threshold_event_button[1].deinit();
    building_label_button[0].deinit();
    building_label_button[1].deinit();
    for(int j=0;j<3;j+=2) {
	vslider[j].deinit();
    }
    // #### end Gilbert 02/05/2001 ####//
}

void Sys::setting_report_deinit(void) {

    // set speed
    sys.set_speed((*vslider[2].var_ptr)*990/100);
    free_button_memory();
    // #### begin Gilbert 02/05/2001 ####//
    save_button.deinit();
    load_button.deinit();
    delete_button.deinit();
    cancel_button.deinit();
    browse_button.deinit();
    game_browser.deinit();
    // game_name.deinit();
    // #### end Gilbert 02/05/2001 ####//

    if(background_bitmap) {
	mem_del(background_bitmap);
	background_bitmap=NULL;
    }
    current_screen_type=SCR_TOP;
};

static void disp_load_game_rec(int recNo, int x, int y, int refreshFlag) {
    if((recNo>0)&&(recNo<=game_file_array.size())) {

	GameFile* gfPtr = game_file_array[recNo];
	char* name = gfPtr->get_save_name();
	font_chartsm.put(x,y,name);
    }
};

static void disp_save_game_rec(int recNo, int x, int y, int refreshFlag) {
    if((recNo>0)&&(recNo<=game_file_array.size())) {

	GameFile* gfPtr = game_file_array[recNo];
	char* name = gfPtr->get_save_name();
	font_chartsm.put(x,y,name);
    }
};
