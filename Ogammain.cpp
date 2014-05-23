//Filename    : OGAMMAIN.CPP
//Description : Main Game Object - Main menu

#include <OVGA.H>
#include <OVGALOCK.H>
#include <OWORLDMT.H>
#include <OMUSIC.H>
#include <OSYS.H>
#include <OIMGRES.H>
#include <OIFACE.H>
#include <OCOLTBL.H>
#include <OPOWER.H>
#include <OBATTLE.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OPSCHOOL.H>
#include <OGSET.H>
#include <OGFILE.H>
#include <OGAME.H>
#include <OINFO.H>
#include <OBOX.H>
#include <OGAMESET.H>
#include <ODEPTRES.H>
#include <OAUDIO.H>
#include <OCONFIG.H>

#define DEBUG_LOG_LOCAL 1
#include <OLOG.H>

//---------- define constant ----------//

enum { BUTTON_COUNT = 6 };

enum {
    BUTTON_WIDTH  = 201,
    BUTTON_HEIGHT = 37
};

enum {
    MODE_TITLE_SCREEN=-1,
    MODE_GAME_SETTING=-3,
    MODE_SCEN_GAME_SETTING=-4
};
//------- define struct GameOptionButton --------//

//!Structure containing the location on the screen and filename of the
//!bitmap for the buttons on the opening screen. Statically defined in
//!game_option_button_array.
struct GameOptionButton {
    short x1;
    short y1;
    char* file_name;
};

//----------- define static vars ---------//

static GameOptionButton game_option_button_array[] = {
    { 80 , 460, "MM-BUT1" },
    { 298, 460, "MM-BUT2" },
    { 518, 460, "MM-BUT3" },
    { 75 , 505, "MM-BUT4" },
    { 290, 505, "MM-BUT5" },
    { 510, 505, "MM-BUT6" },
};

static void init_school_type();

//---------- Begin of function Game::main_menu ----------//
//!
void Game::main_menu() {
    //------------- display menu options -------------//

    int refreshFlag=1;

    vga_front.unlock_buf();

    while(1) {
	sys.signal_exit_flag=0;
	//------- Display game title and paint menu box --------//

	// Handle window messages

	MSG msg;
	if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE)) {
	    if (!GetMessage( &msg, NULL, 0, 0)) {
		sys.signal_exit_flag = 1;
		return;
	    }
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	    continue;
	}
	else if( sys.paused_flag || !sys.active_flag ) {
	    WaitMessage();
	    continue;
	}

	if( sys.redraw_all_flag ) {                   // || m.get_time() - sys.last_redraw_time > 3000 )
	    refreshFlag = 1;
	    sys.redraw_all_flag = 0;
	    sys.last_redraw_time = m.get_time();        // redraw every 8 sec. Such that if background erased at the beginning, redraw
	}
	vga_front.lock_buf();                         // ## chwg1116
	vga.use_front();                              // many places called vga.used_back

	// -------- display ----------//
	if( refreshFlag ) {
	    sys.view_mode=-1;                           // -1 = in main menu

	    int dataSize;
	    char* bmpName = "MENU-STD";

#ifdef ADMIN
	    bmpName = "MENU-ADM";
#endif

	    File* filePtr = image_interface.get_file(bmpName, dataSize);

	    if (filePtr->file_get_short() != -1 ) {     // use common color palette
		filePtr->file_seek(filePtr->file_pos() - sizeof(short));
		vga_front.put_large_bitmap(0, 0, filePtr);
	    }
	    else {                                      // use own color palette
		unsigned char palette[256 * 3];
		short *remapTable;
		filePtr->file_read(palette, 256 * 3);
		PalDesc palDesc(palette, 3, 256, 6);
		ColorTable colorTable;
		colorTable.generate_table_fast(MAX_BRIGHTNESS_ADJUST_DEGREE, palDesc, ColorTable::bright_func);
		remapTable = (short *) colorTable.get_table(0);
		vga_front.put_large_bitmap(0, 0, filePtr, remapTable);
	    }

	    //------- display version info --------//
	    disp_version();
	    mouse_cursor.set_icon(CURSOR_NORMAL);
	    mouse.show();
	    refreshFlag=0;
	}

	sys.blt_virtual_buf();

	//---------- detect menu option buttons -----------//
	sys.yield();
	music.yield();
	mouse.get_event();

	GameOptionButton* buttonPtr = game_option_button_array;

	for( int i=0 ; i<BUTTON_COUNT ; i++, buttonPtr++ ) {
	    if(  mouse.single_click( buttonPtr->x1, buttonPtr->y1,
				     buttonPtr->x1+BUTTON_WIDTH-1, buttonPtr->y1+BUTTON_HEIGHT-1, 0 ) ) {
		mouse.hide_area(buttonPtr->x1, buttonPtr->y1,
				buttonPtr->x1+BUTTON_WIDTH-1,  buttonPtr->y1+BUTTON_HEIGHT-1);
		image_interface.put_front( buttonPtr->x1, buttonPtr->y1, buttonPtr->file_name );
		mouse.show_area();

		sys.blt_virtual_buf();

		while( mouse.left_press ) {
		    sys.yield();
		    mouse.get_event();
		    sys.blt_virtual_buf();
		}
		run_main_menu_option(i+1);

		refreshFlag=1;
		break;
	    }
	}

	vga_front.unlock_buf();                       // ## chwg1116
	//-------------------------------------------------//
	if( sys.signal_exit_flag ==1)
	    break;
    }

    vga_front.lock_buf();                           // ## chwg1116
}

//------------ End of function Game::main_menu -----------//

//-------- Begin of function Game::run_main_menu_option --------//
//!
//!
void Game::run_main_menu_option(int optionId) {
    //	vga_front.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, V_BLACK );
    //	sys.blt_virtual_buf();
    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));

    //------ New Game -----------//

    if( optionId==2 ) {
#ifndef ADMIN
	box.msg( "The custom game in this version is for demo purpose only. It has a time limit of 2 years. Please consider buying the Administrator version for fully functional custom games." );
#endif

	new_game();
    }

    //--------- Play Scenario ---------//
    if( optionId==1 )
	play_scenario();

    //----------- Load Game -----------//

    if( optionId==3 )
	loadgame_menu();

    //------------- Hall of Fame ----------------//

    if( optionId==4 )
	hall_of_fame();
    //		game_file_array.disp_hall_of_fame();

    //------------- Credits ----------------//

    if( optionId==5 )
	credit();

    //----------------- Quit Game ----------------//

    if( optionId==6 )
	sys.signal_exit_flag = 1;
}

//---------- End of function Game::run_main_menu_option ---------//

//-------- Begin of static function disp_version --------//
//
void Game::disp_version() {
    //----------- display version string --------//

    String str;

    str  = "Version ";
    str += GAME_VERSION_STR;

#ifdef BETA
    str = "This is a Beta version. Unathorized distribution of this Beta is illegal.";
#endif

    if( str.len() > 40 )
	font_fblack.center_put( 0, VGA_HEIGHT-20, VGA_WIDTH-1, VGA_HEIGHT-1, str );
    else
	font_fblack.put( 10, VGA_HEIGHT-22, str );
}

//---------- End of function Game::disp_version ---------//

//---------- Begin of function init_school_type ---------//
static void init_school_type() {
    if ( player_school.is_public() ) {
	if ( player_school.campus_environment != RURAL )
	    player_school.school_type = PUBLIC_SCHOOL;
	else
	    player_school.school_type = COMPREHENSIVE_SCHOOL;
    }
    else {
	if ( player_school.campus_environment != RURAL )
	    player_school.school_type = PRIVATE_SCHOOL;
	else
	    player_school.school_type = LIBERAL_ARTS_SCHOOL;
    }

    //firm_res.init();			// it depends on player_school.school_type
}

//---------- End of function init_school_type ---------//

//---------- Begin of function Game::new_game ----------//
//!
void Game::new_game() {
    sys.view_mode=MODE_GAME_SETTING;
    //----- initialize player_school first -----//

    player_school.init();
    game_setting.new_game_init();

    game_set.open_set(1);                           // open the default game set

    field_res.init();
    department_res.init();

    //---- new game settings screen -----//

    // display the background
    image_interface.put_large( &vga_back, 0, 0, "EMPTYSCR" );

    info.init();                                    // ## chwg1015		no disp_column function for the game setting screen
    game_setting.new_game_report(INFO_REPAINT);

    sys.redraw_all_flag=0;

    vga_front.unlock_buf();                         // chwg1116

    int startTime = m.get_time();

    sys.redraw_all_flag = 1;                        // force repaint once

    while(1) {
	MSG msg;
	if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE)) {
	    if (!GetMessage( &msg, NULL, 0, 0)) {
		sys.signal_exit_flag = 1;
		// BUGHERE : vga_front is unlocked
		return;
	    }
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	    continue;
	}
	else if( sys.paused_flag || !sys.active_flag ) {
	    WaitMessage();
	    continue;
	}

	vga_front.lock_buf();                         // ## chwg1116
	sys.yield();

	if( sys.redraw_all_flag ) {                   // || m.get_time() - sys.last_redraw_time > 3000 )
	    sys.redraw_all_flag = 0;
	    sys.last_redraw_time = m.get_time();        // redraw every 8 sec. Such that if background erased at the beginning, redraw
	    game_setting.new_game_report(INFO_REPAINT);
	}

	sys.yield();

	int rc = game_setting.new_game_detect();
	if( rc==1 ) {                                 // Start button is pressed
	    //## fred
	    DEBUG_LOG("Game::new_game(): Start button is pressed");

	    player_school.calc_setting_vars();          //## chea 091099 getting option button

	    break;
	}
	else if( rc == -1 ) {                         // Cancel button is pressed
	    game_setting.new_game_init();               // reset game_setting
	    info.deinit();                              // ## chwg1015
	    field_res.deinit();
	    department_res.deinit();
	    game_set.close_set();                       // close the game set

	    //## fred
	    DEBUG_LOG("Game::new_game(): Cancel button is pressed");
	    return;
	}

	sys.blt_virtual_buf();
	sys.yield();
	mouse.get_event();

	if(sys.redraw_all_flag)
	    game_setting.new_game_report(INFO_REPAINT);
	else
	    game_setting.new_game_report(INFO_UPDATE);
	sys.yield();

	vga_front.unlock_buf();                       // chwg1116
    }

    user_interface.darken(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);
    info.disp_column_bitmap(&vga_back);
    vga.blt_buf(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);

    image_interface.put_large( &vga_back, 0, 0, "MAINSCR2" );
    vga.blt_buf(0,0,799,ZOOM_Y1-1);
    vga.blt_buf(0,ZOOM_Y2+1,799,599);
    vga.use_back();

    box.progress(0);

    info.deinit();                                  // ## chwg1015
    field_res.deinit();
    department_res.deinit();
    game_set.close_set();                           // close the game set

    vga.use_front();

    //------- start the game ------//
    init_school_type();                             // before firm_res.init()
    player_school.init_setting_non_scenario();      //## fred 0426

    init();
    sys.staying_view_mode=MODE_NORMAL;
    sys.view_mode=MODE_NORMAL;
    sys.prev_view_mode = MODE_NORMAL;
    sys.back_view_mode = MODE_NORMAL;
    sys.is_institution_report=0;
    sys.menu_pull_down=0;
    sys.pause_game_flag=0;
    sys.redraw_all_flag = 1;
    sys.redraw_zoom_flag = 0;
    sys.set_speed(DEFAULT_SPEED);
    sys.m_o_log_selected=0;
    sys.m_o_log_highlighted=-1;
    sys.m_o_log_old_selected=-2;
    sys.m_o_log_old_highlighted=-2;
    sys.m_o_log_display_updated=1;
    config.scroll_speed=10;

    vga_front.temp_unlock();
    //## fred
    DEBUG_LOG("Game::new_game(): calling battle.run() ---- ");
    vga_front.temp_restore_lock();

    battle.run();

    vga_front.temp_unlock();
    //## fred
    DEBUG_LOG("Game::new_game(): called  battle.run() ---- ");
    vga_front.temp_restore_lock();

    deinit();

}

//------------ End of function Game::new_game -----------//

//---------- Begin of function Game::play_scenario ----------//
//!
void Game::play_scenario() {
    //----- initialize player_school first -----//
    sys.view_mode=MODE_SCEN_GAME_SETTING;
    player_school.init();
    game_setting.play_scenario_init();

    game_set.open_set(1);                           // open the default game set

    field_res.init();
    department_res.init();
    field_res.deinit();
    department_res.deinit();

    //##  field_res.init();
    //## department_res.init();

    //---- new game settings screen -----//

    // display the background
    image_interface.put_large( &vga_back, 0, 0, "EMPTYSCR" );

    info.init();                                    // ## chwg1015		no disp_column function for the game setting screen

    player_school.scenario_id = 1;                  // auto select first scenario SCN_RAISE_SALARY is one
    game_setting.play_scenario_report(INFO_REPAINT);

    sys.redraw_all_flag=0;

    vga_front.unlock_buf();                         // chwg1116

    while(1) {

	// handle window message here //
	// ##  begin chwg1116
	MSG msg;
	if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE)) {
	    if (!GetMessage( &msg, NULL, 0, 0)) {
		sys.signal_exit_flag = 1;
		// BUGHERE : vga_front is unlocked
		return;
	    }
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	    continue;
	}
	else if( sys.paused_flag || !sys.active_flag ) {
	    WaitMessage();
	    continue;
	}

	vga_front.lock_buf();                         // ## chwg1116
	/*
	  if( sys.redraw_all_flag ) // || m.get_time() - sys.last_redraw_time > 3000 )
	  {
          sys.redraw_all_flag = 0;
          sys.last_redraw_time = m.get_time();	// redraw every 8 sec. Such that if background erased at the beginning, redraw
          game_setting.play_scenario_report(INFO_REPAINT);
	  }
	*/
	sys.yield();

	int rc = game_setting.play_scenario_detect();
	if( rc==1 )                                   // Start button is pressed
	    break;
	else if( rc == -1 ) {                         // Cancel to Main Menu button is pressed
	    game_setting.play_scenario_init();          // reset game_setting
	    info.deinit();                              // ## chwg1015
	    game_set.close_set();                       // close the game set
	    return;
	}
	sys.blt_virtual_buf();
	sys.yield();
	mouse.get_event();

	// fix in version 2.0
	// game_setting.play_scenario_report(sys.redraw_all_flag?INFO_REPAINT:INFO_UPDATE);
	if( sys.redraw_all_flag ) {
	    game_setting.play_scenario_report( INFO_REPAINT );
	    sys.redraw_all_flag = 0;
	}
	else {
	    game_setting.play_scenario_report( INFO_UPDATE );
	}

	sys.yield();

	vga_front.unlock_buf();                       // chwg1116
    }

    // the scenario file is loaded in scenario_detect which **called** game.init()	//## fred 0224
    // play now!

    game_setting.play_scenario_init();              //## fred 0224		// should be deinit!?
    user_interface.darken(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);
    info.disp_column_bitmap(&vga_back);
    vga.blt_buf(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);

    image_interface.put_large( &vga_back, 0, 0, "MAINSCR2" );
    vga.blt_buf(0,0,799,ZOOM_Y1-1);
    vga.blt_buf(0,ZOOM_Y2+1,799,599);
    vga.use_back();

    box.progress(0);

    info.deinit();                                  // ## chwg1015
    //## field_res.deinit();
    //## department_res.deinit();
    game_set.close_set();                           // close the game set

    vga.use_front();

    //------- start the game ------//
    // rc: scenario id

    //init_school_type();

    //## fred 0224
    player_school.init_setting_scenario(player_school.scenario_id);

    init();
    sys.staying_view_mode=MODE_NORMAL;
    sys.view_mode=MODE_NORMAL;
    sys.prev_view_mode = MODE_NORMAL;
    sys.back_view_mode = MODE_NORMAL;
    sys.is_institution_report=0;
    sys.menu_pull_down=0;
    sys.redraw_all_flag = 1;
    sys.redraw_zoom_flag = 0;
    sys.set_speed(DEFAULT_SPEED);
    sys.m_o_log_selected=0;
    sys.m_o_log_highlighted=-1;
    sys.m_o_log_old_selected=-2;
    sys.m_o_log_old_highlighted=-2;
    sys.m_o_log_display_updated=1;
    config.scroll_speed=10;

    battle.run(player_school.scenario_id);

    deinit();
}

//------------ End of function Game::play_scenario -----------//

//---------- Begin of function Game::credit ----------//
//!
void Game::credit() {
    int vtab1=100,vtab2=120,vtab3=140;
    int tab1 =200,tab2=530,tab3=650,tab4=750;
    sys.redraw_all_flag=1;

    int credit_state=0;
    vga_front.unlock_buf();                         // chwg1116

    sys.redraw_all_flag = 1;                        // force repaint once

    while(1) {
	// handle window message here //
	// ##  begin chwg1116
	MSG msg;
	if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE)) {
	    if (!GetMessage( &msg, NULL, 0, 0)) {
		sys.signal_exit_flag = 1;
		// BUGHERE : vga_front is unlocked
		return;
	    }
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	    continue;
	}
	else if( sys.paused_flag || !sys.active_flag ) {
	    WaitMessage();
	    continue;
	}

	vga_front.lock_buf();                         // chwg1116

	if( sys.redraw_all_flag ) {                   // || m.get_time() - sys.last_redraw_time > 3000 )
	    vga.use_back();
	    // display the background
	    image_interface.put_large( &vga_back, 0, 0, "CREDIT" );

	    if(credit_state==0) {
		font_charts.put(tab1,vtab1+80,  "Published by");
		font_charts.put(tab1+50,vtab1+100,  "Alfred P. Sloan Foundation");
		font_charts.put(tab1,vtab1+140, "Developed by");
		font_charts.put(tab1+50,vtab1+160,  "Enlight Software");
		font_charts.put(tab1,vtab1+200, "Simulation Model Developed by");
		font_charts.put(tab1+50,vtab1+220,  "The Jackson Hole Higher Education Group");
	    }

	    if(credit_state==2) {
		font_charts.put(tab1,vtab1+40,"Original Concept");
		font_charts.put(tab1+50,vtab1+60,"Bill Massy");

		font_charts.put(tab1,vtab1+90,"Game Design");
		font_charts.put(tab1+50,vtab1+110,"Trevor Chan, Jesse Ausubel, Bill Massy, and Sally Vaughn");

		font_charts.put(tab1,vtab1+140,"Programming Version 2.0");
		font_charts.put(tab1+50,vtab1+160,"Gilbert Luis, Kenneth Mo, Macro Yuen");

		font_charts.put(tab1,vtab1+190,"Programming Version 1.0");
		font_charts.put(tab1+50,vtab1+210,"Fred Hui, Alan Chea, Kevin Wong, Zhou Bin and Zhang Bing");

		font_charts.put(tab1,vtab1+240,"Graphics");
		font_charts.put(tab1+50,vtab1+260,"Chris Whitehouse and Victor Ngai");

		font_charts.put(tab1,vtab1+290,"Game Text");
		font_charts.put(tab1+50,vtab1+310,"Jesse Ausubel, Bill Massy, and Sally Vaughn");
	    }

	    if(credit_state==3) {
		font_charts.put(tab1,vtab1+20,"User Manual and Strategy Guide");
		font_charts.put(tab1+50,vtab1+40,"Neil Salkind");

		font_charts.put(tab1,vtab1+80,"Project Manager for Documentation");
		font_charts.put(tab1+50,vtab1+100,"David Greely");
		//				font_charts.put(tab1,vtab1+220,"Technical Documentation - Bill Massy (if the technical documents are included in the CDROM)");
		font_charts.put(tab1,vtab1+140,"Music and Sound Effects");
		font_charts.put(tab1+50,vtab1+160,"Roberto Khan and Chris Whitehouse");
		font_charts.put(tab1,vtab1+200,"Beta-testing Coordination");
		font_charts.put(tab1+50,vtab1+220,"Eva Wong");
		font_charts.put(tab1,vtab1+260,"Marketing Management");
		font_charts.put(tab1+50,vtab1+280,"Ben Sawyer");
	    }
	    if(credit_state==1) {                       //## chea 120899 order change
		font_charts.put(tab1,vtab1+100,"Project Director (Sloan)");
		font_charts.put(tab1+50,vtab1+120,"Jesse Ausubel");
		font_charts.put(tab1,vtab1+160,"Project Director (The Jackson Hole Higher Education Group)");
		font_charts.put(tab1+50,vtab1+180,"Bill Massy");
		font_charts.put(tab1,vtab1+220,"Project Director (Enlight)");
		font_charts.put(tab1+50,vtab1+240,"Trevor Chan");
	    }

	    vga.blt_buf(0,0,799,599);
	    sys.redraw_all_flag = 0;
	    sys.last_redraw_time = m.get_time();        // redraw every 8 sec. Such that if background erased at the beginning, redraw
	}
	// ##  end chwg1116

	// detect
	sys.blt_virtual_buf();
	sys.yield();
	mouse.get_event();
	if(mouse.single_click(2)) {                   // Cancel to Main Menu button is pressed
	    credit_state++;
	    if(credit_state>3)
		return;
	    sys.redraw_all_flag=1;
	}
	vga_front.unlock_buf();                       // chwg1116
    }

}

//------------ End of function Game::credit -----------//

void Game::hall_of_fame() {
    int vtab1=100,vtab2=120,vtab3=140;
    int tab1 =150,tab2=530,tab3=650,tab4=750;
    int line_spacing = 90;

    game_file_array.init("*.SAV");                  //## fred 0228  // which will load hallfame.dat
    sys.redraw_all_flag=1;

    vga_front.unlock_buf();                         // chwg1116
    while(1) {
	// handle window message here //
	// ##  begin chwg1116
	MSG msg;
	if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE)) {
	    if (!GetMessage( &msg, NULL, 0, 0)) {
		sys.signal_exit_flag = 1;
		// BUGHERE : vga_front is unlocked
		return;
	    }
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	    continue;
	}
	else if( sys.paused_flag || !sys.active_flag ) {
	    WaitMessage();
	    continue;
	}

	vga_front.lock_buf();                         // chwg1116
	if( sys.redraw_all_flag ) {                   // || m.get_time() - sys.last_redraw_time > 3000 )

	    vga.use_back();
	    // display the background
	    image_interface.put_large( &vga_back, 0, 0, "HALL" );
	    for(int i=0;i<HALL_FAME_NUM;i++) {
		if(game_file_array.hall_fame_array[i].score<=0)
		    break;

		user_interface.brighten(tab1-5,vtab1+line_spacing*i-5,tab4,vtab1+line_spacing*i+65);
		user_interface.rect(tab1-5,vtab1+line_spacing*i-5,tab4,vtab1+line_spacing*i+65,1);
		font_charts.put
		    (tab1+5,vtab1+line_spacing*i,i+1);

		String title;
		title = game_file_array.hall_fame_array[i].player_first_name;
		title += " ";
		title += game_file_array.hall_fame_array[i].player_last_name;
		title += " of ";
		title += game_file_array.hall_fame_array[i].school_name;
		font_charts.put
		    (tab1+40,vtab1+line_spacing*i,title);

		title = "Score : ";
		title += game_file_array.hall_fame_array[i].score;
		title += "  Trustee Evaluation : ";
		title += game_file_array.hall_fame_array[i].trust_score;
		font_charts.put
		    (tab1+40,vtab2+line_spacing*i,title);

		title = "Period : ";
		title += m.format(game_file_array.hall_fame_array[i].start_year,16);
		title += " - ";
		title += m.format(game_file_array.hall_fame_array[i].end_year,16);
		font_charts.put
		    (tab2+40,vtab2+line_spacing*i,title);

		enum {TMP_SCENARIO_COUNT=SCENARIO_COUNT};
		static char* scn_name[TMP_SCENARIO_COUNT] = {
		    "1. [PAY BETTER]",
		    "2. [ALLOCATE NEW MONEY]",
		    "3. [TEACH BETTER]",
		    "4. [IMPROVE RESEARCH PERFORMANCE]",
		    "5. [WIN GAMES]",
		    "6. [REDUCE TUITION]",
		    "7. [RESPOND TO ENROLLMENT SHIFTS]",
		    "8. [ENROLL MORE MINORITY STUDENTS]",
		    "9. [HIRE MORE MINORITY FACULTY MEMBERS]",
		    "10. [BALANCE THE BUDGET]",

		    /*"1. BALANCE BUDGET",
		      "2. RAISE SALARY",
		      "3. INVEST ACADEMIC",
		      "4. WINNING ATHLETICS",
		      "5. LIMIT TUITION GROWTH",
		    */
		};

		if(game_file_array.hall_fame_array[i].scenario_id>0) {
		    title = "Scenario : ";
		    // BUGHERE ##chwg0228
		    title += scn_name[game_file_array.hall_fame_array[i].scenario_id-1];

		    title += "";
		    font_charts.put
			(tab1+40,vtab3+line_spacing*i,title);
		}
	    }
	    vga.blt_buf(0,0,799,599);

	    sys.redraw_all_flag = 0;
	    sys.last_redraw_time = m.get_time();        // redraw every 8 sec. Such that if background erased at the beginning, redraw
	}

	// detect
	sys.blt_virtual_buf();
	sys.yield();
	mouse.get_event();
	if(mouse.single_click(2)) {                   // Cancel to Main Menu button is pressed
	    return;
	}
	vga_front.unlock_buf();                       // chwg1116
    }

}

//------------ End of function Game::hall_of_fame -----------//

//#### begin fred 0121 ####//
//---------- Begin of function Game::scenario_menu ----------//
//!
void Game::scenario_menu() {
    err_here();
}

//------------ End of function Game::scenario_menu -----------//

//---------- Begin of function Game::loadgame_menu ----------//
//!
void Game::loadgame_menu() {
    //----- initialize first -----//

    //player_school.init();

    loadgame_init();

    game_set.open_set(1);                           // open the default game set

    //---- new screen -----//

    // display the background
    image_interface.put_large( &vga_back, 0, 0, "EMPTYSCR" );

    info.init();                                    // no disp_column function for the game setting screen
    loadgame_report(INFO_REPAINT);

    sys.redraw_all_flag=1;

    vga_front.unlock_buf();                         // chwg1116

    while(1) {

	// handle window message here //
	// ##  begin chwg1116
	MSG msg;
	if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE)) {
	    if (!GetMessage( &msg, NULL, 0, 0)) {
		sys.signal_exit_flag = 1;
		// BUGHERE : vga_front is unlocked
		return;
	    }
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	    continue;
	}
	else if( sys.paused_flag || !sys.active_flag ) {
	    WaitMessage();
	    continue;
	}
	/*
	  if( sys.redraw_all_flag ) // || m.get_time() - sys.last_redraw_time > 3000 )
	  {
          sys.redraw_all_flag = 0;
          sys.last_redraw_time = m.get_time();	// redraw every 8 sec. Such that if background erased at the beginning, redraw
          loadgame_report(INFO_REPAINT);
	  }
	*/
	vga_front.lock_buf();                         // ## chwg1116
	// ##  end chwg1116

	sys.blt_virtual_buf();
	sys.yield();
	mouse.get_event();

	int rc = loadgame_detect();

	if( rc==1 )                                   // Start button is pressed
	    break;
	else if( rc == -1 ) {                         // Cancel button is pressed
	    loadgame_deinit();

	    info.deinit();
	    game_set.close_set();                       // close the game set
	    return;
	}

	loadgame_report(sys.redraw_all_flag?INFO_REPAINT:INFO_UPDATE);
	vga_front.unlock_buf();                       // ## chwg1116
    }

    //----------//
    // the loadgame file is loaded in loadgame_detect which **called** game.init()
    // play now!

    loadgame_deinit();

    user_interface.darken(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);
    info.disp_column_bitmap(&vga_back);
    vga.blt_buf(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);

    image_interface.put_large( &vga_back, 0, 0, "MAINSCR2" );
    vga.blt_buf(0,0,799,ZOOM_Y1-1);
    vga.blt_buf(0,ZOOM_Y2+1,799,599);

    vga.use_front();

    //------- start the game ------//

    sys.staying_view_mode=MODE_NORMAL;
    sys.view_mode=MODE_NORMAL;
    sys.prev_view_mode = MODE_NORMAL;
    sys.back_view_mode = MODE_NORMAL;
    sys.is_institution_report=0;
    sys.menu_pull_down=0;
    sys.redraw_all_flag = 1;
    sys.redraw_zoom_flag = 0;
    //### begin zb 02/25/99
    //	sys.set_speed(DEFAULT_SPEED);
    sys.set_speed(sys.save_speed);
    //### end zb 02/25/99
    sys.m_o_log_selected=0;
    sys.m_o_log_highlighted=-1;
    sys.m_o_log_old_selected=-2;
    sys.m_o_log_old_highlighted=-2;
    sys.m_o_log_display_updated=1;
    config.scroll_speed=10;

    sys.run(1);                                     // 1: load game

    deinit();
}

//------------ End of function Game::loadgame_menu -----------//
//#### end fred 0121 ####//
