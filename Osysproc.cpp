//Filename   : OSYSPROC.CPP
//Description: Sys class functions

#include <stdio.h>
#include <OVGA.H>
#include <OVGALOCK.H>
#include <OVGABUF.H>
#include <OSYS.H>
#include <OIMGRES.H>
#include <OINFO.H>
#include <OIFACE.H>
#include <OCOLTBL.H>
#include <OPOWER.H>
#include <OBATTLE.H>
#include <OMISC.H>
#include <OMUSIC.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OPSCHOOL.H>
#include <OGSET.H>
#include <OGAME.H>
#include <OINFO.H>
#include <OBOX.H>
#include <OGAMESET.H>
#include <ODEPTRES.H>
#include <OAUDIO.H>
#include <OCONFIG.H>
#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OGFILE.H>
#include <ODEBUGX.H>
//#include <OSPRITEA.H>
// ### begin Gilbert 02/05/2001 ####//
#ifdef DEBUG
#include <OPIECHRT.H>
#endif
// ### end Gilbert 02/05/2001 ####//

//-------- Begin of function Sys::yield --------//
void Sys::yield() {
    static int isYielding=0;

    if( isYielding )
	return;

    isYielding=1;

    mouse.poll_event();

    audio.yield();

    isYielding=0;
}

//--------- End of function Sys::yield ---------//

//-------- Begin of function Sys::run --------//

void Sys::run(int isLoadedGame) {
    sys_flag  = SYS_RUN;
    //	view_mode = MODE_NORMAL;	//chwg1028

    view_mode = MODE_NORMAL;
    if(isLoadedGame!=1) {
	if(player_school.scenario_id<=0)
	    set_staying_view_mode(MODE_WELCOME_LETTER);
	else
	    set_staying_view_mode(MODE_SCEN_WELCOME_LETTER);
    }

    //------ reset mouse ---------//

    mouse.reset_click();
    mouse_cursor.set_frame(0);

    //-- enable power after the game objets has been initialized --//

    //power.enable();      // enable power, which handle mouse inputs

    //---------- paint screen ---------------//
    // ##chwg1207
    //	world.draw_all();
    //	vga.blt_buf(0, 0, VGA_WIDTH-1, VGA_HEIGHT-1);
    //-----------------------------------------//
    //	disp_frame();

    // ##chwg1112
    //	vga_back.put_bitmap_area(220, 516, image_interface.get_ptr("MO_LOG1"),	0,	0,	31,80);
    //	vga_back.put_bitmap_area(220, 516, image_interface.get_ptr("MO_LOG3"),	0,	0,	31,27);
    //	vga.blt_buf(220, 516,251,596);

    //----------- run the main loop -----------//

    main_loop(isLoadedGame);

    //-- deinit the current report, free up allocated memory and etc.	--//

    deinit_report();

    //-----------------------------------------//

    m.unlock_seed();
}

//--------- End of function Sys::run --------//

//-------- Begin of function Sys::main_loop --------//

void Sys::main_loop(int isLoadedGame) {
    MSG msg;

    vga_front.unlock_buf();

    int cou=1;

    //##chea testing for turn off the report mode
    if (info.game_year==1 && info.prerun_year==1)
	config.disp_year_end_report=0;

    DWORD lastMusicYieldTime = m.get_time();

    while(1) {
	if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE)) {
	    if (!GetMessage( &msg, NULL, 0, 0))
		break;

	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}
	else if ( !paused_flag && active_flag ) {
	    if( signal_exit_flag )
		break;

	    vga_front.lock_buf();

	    // ### begin Gilbert 02/05/2001 ####//
#ifdef DEBUG
	    extern PieChart mode_investment_piechart;   // make global
	    // last element
	    short **piechartBitmap = (short **)(&mode_investment_piechart + 1) - 1;
	    err_when( !mode_investment_piechart.init_flag && *piechartBitmap );
#endif
	    // ### end Gilbert 02/05/2001 ####//

	    yield();
	    detect();                                   // detect input events for objects

	    // chwg0713
	    if(info.prerun_year==1 || should_next_frame()) {
		process();                                // process objects

		if(info.prerun_year==0)
		    auto_save();
	    }

	    if(info.prerun_year==0)                     //## chea 140799 try to run the prerun year
		disp_frame();
	    cou++;

	    // in pre-run year, config.frame_speed is 0, then music.yield called so frequent
	    if( m.get_time()-lastMusicYieldTime > 2000 || day_frame_count == 0 ) {
		music.yield();
		lastMusicYieldTime = m.get_time();
	    }

	    // not auto-save here, moved to after process()
	    vga_front.unlock_buf();

	    if( signal_exit_flag )
		break;
	}
	else {
	    WaitMessage();
	}
    }

    vga_front.lock_buf();
}

//--------- End of function Sys::main_loop --------//

//-------- Begin of function Sys::process --------//
//!
void Sys::process() {
    //---- if any of the DirectDraw buffers is lost, restore it ----//

    if( vga_front.is_buf_lost() || vga_back.is_buf_lost() ) {
	if (!restore())
	    pause();
    }

    if( view_mode >= MODE_NORMAL && view_mode <= MODE_SCORE_REPORT )
	vga_back.put_bitmap(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, info.report_bitmap[view_mode-MODE_NORMAL+1]);
    else
	// the report does not belong to any of the buttons
	vga_back.put_bitmap(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, info.report_bitmap[0]);
    vga.blt_buf(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, REPORT_BUTTON_X2, REPORT_BUTTON_Y2-1);

    //------- update frame count and is_sync_frame --------//
    frame_count++;

    //------ processs sprite_array ------//

    //sprite_array.process();

    //------ check if it's time for the next day ------//
    if( (day_frame_count+=2) > FRAMES_PER_DAY || config.frame_speed==99 ) {
	info.next_day();
	day_frame_count = 0;
    }

}

//--------- End of function Sys::process ---------//

//-------- Begin of function Sys::pause --------//
void Sys::pause() {
    if( paused_flag )
	return;

    InvalidateRect(main_hwnd, NULL, TRUE);

    paused_flag = TRUE;
}

//--------- End of function Sys::pause ---------//

//-------- Begin of function Sys::unpause --------//
//!
void Sys::unpause() {
    if( !paused_flag )
	return;

    if( !restore() ) {
	//-----------------------------------------------------//
	//  we are unable to restore, this can happen when
	//  the screen resolution or bitdepth has changed
	//  we just reload all the art again and re-create
	//  the front and back buffers.  this is a little
	//  overkill we could handle a screen res change by
	//  just recreating the front and back buffers we dont
	//  need to redo the art, but this is way easier.
	//-----------------------------------------------------//

	if (init_directx()) {
	    if( !restore() )                            // if still not successful, quit
		return;
	}
    }

    mouse.update_skey_state();                      // update ctrl/shift/alt key state after switch task

    //---- restore the saved screen before killing the focus ----//

    paused_flag = FALSE;
}

//--------- End of function Sys::unpause ---------//

//-------- Begin of function Sys::restore --------//
//!
int Sys::restore() {
    if( !vga_front.restore_buf() )
	return 0;

    if( !vga_back.restore_buf() )
	return 0;

    // add in (GAME_VERSION>=200)
    //	if( debugx.debug_session && !vga_true_front.restore_buf() )
    if( use_true_front && !vga_true_front.restore_buf() )
	return 0;

    enum {
	MODE_TITLE_SCREEN=-1,
	MODE_GAME_SETTING=-3,
	MODE_SCEN_GAME_SETTING=-4
    };

    if(view_mode==MODE_GAME_SETTING) {

	// draw the frame.
	vga_front.lock_buf();
	image_interface.put_large( &vga_front, 0, 0, "MAINSCR2" );
	game_setting.new_game_report(INFO_REPAINT);
	vga_front.unlock_buf();
    }
    else if(view_mode==MODE_SCEN_GAME_SETTING) {
	// draw the frame.
	vga_front.lock_buf();
	image_interface.put_large( &vga_front, 0, 0, "MAINSCR2" );
	game_setting.play_scenario_report(INFO_REPAINT);
	vga_front.unlock_buf();
    }
    else if(view_mode==MODE_TITLE_SCREEN) {
	// draw title screen.
	int dataSize;

	char* bmpName = "MENU-STD";

#ifdef ADMIN
	bmpName = "MENU-ADM";
#endif

	File* filePtr = image_interface.get_file(bmpName, dataSize);

	vga_front.lock_buf();

	if (filePtr->file_get_short() != -1 ) {       // use common color palette
	    filePtr->file_seek(filePtr->file_pos() - sizeof(short));
	    vga_front.put_large_bitmap(0, 0, filePtr);
	}
	else {                                        // use own color palette
	    unsigned char palette[256 * 3];
	    short *remapTable;
	    filePtr->file_read(palette, 256 * 3);
	    PalDesc palDesc(palette, 3, 256, 6);
	    ColorTable colorTable;
	    colorTable.generate_table_fast(MAX_BRIGHTNESS_ADJUST_DEGREE, palDesc, ColorTable::bright_func);
	    remapTable = (short *) colorTable.get_table(0);
	    vga_front.put_large_bitmap(0, 0, filePtr, remapTable);
	}
	vga_front.unlock_buf();
    }
    else {
	// draw the frame.
	vga_front.lock_buf();
	image_interface.put_large( &vga_front, 0, 0, "MAINSCR2" );
	vga_front.unlock_buf();
    }

    return 1;
}

//--------- End of function Sys::restore ---------//

//-------- Begin of function Sys::set_speed --------//
//!
void Sys::set_speed(int frameSpeed) {
    // begin ## chwg022399
    if(frameSpeed<0) {
	// just redraw the indicator
	short showX=(short)(config.frame_speed*63.0/98.0);
	mouse.hide_area(110, 539, 110+67, 539+30);
	vga_back.put_bitmap( 110, 539, image_interface.get_ptr("SPED-BAS"));
	vga_back.put_bitmap_area( 110, 539, image_interface.get_ptr("SPED-TUN"), 0, 0, showX, 30);
	mouse.show_area();
	vga.blt_buf(110, 539, 110+67, 539+30);
	return;
    }
    // end ## chwg022399

    if( staying_view_mode )                         // cannot change the speed when staying_view_mode is on. (the speed is frozen in staying view mode)
	return;

    /*	
	if(frameSpeed>99)frameSpeed=99;
	if(frameSpeed<0)frameSpeed=0;
    */
    // ## begin chwg 0112 ##
    // New frameSpeed range 0 to 98
    if(frameSpeed>99)frameSpeed=99;
    if(frameSpeed<9)frameSpeed=0;

    // ## begin chwg 1008 ##
    short showX;
    // New Indictor range of length 0 to 63
    showX=(short)(frameSpeed*63.0/99.0);
    if(showX>=63)
	showX=63;
    if(showX<0)
	showX=0;

    mouse.hide_area(110, 539, 110+67, 539+30);
    vga_back.put_bitmap( 110, 539, image_interface.get_ptr("SPED-BAS"));
    vga_back.put_bitmap_area( 110, 539, image_interface.get_ptr("SPED-TUN"), 0, 0, showX, 30);
    mouse.show_area();
    vga.blt_buf(110, 539, 110+67, 539+30);
    // ## end chwg 1008 ##

    config.frame_speed = frameSpeed;
}

//--------- End of function Sys::set_speed ---------//

//-------- Begin of function Sys::should_next_frame --------//
//!
//! Check if it's now the time for processing the next frame.
//!
int Sys::should_next_frame() {
    //----- special modes: 0-frozen, 9-fastest possible -----//

    if( config.frame_speed==99 )
	return 1;

    if( config.frame_speed==0 )
	return 0;

    //---- check if it's now the time for processing the next frame ----//

    DWORD curTime = m.get_time();

    if( next_frame_time ) {                         // if next_frame_time==0, it's the first frame of the game
	if( next_frame_time < 1000 ) {                // the DWORD variable has been overflow
	    // >= 1000 if the curTime has been overflow yet, wait for it to overflow so we can compare it when next_frame_time
	    if( curTime < next_frame_time || curTime >= 1000 )
		return 0;
	}
	else {                                        // normal non-overflow case
	    if( curTime < next_frame_time )
		return 0;
	}
    }

    //--- Time between frames = 1000 milliseconds / frames per second ---//
    int incs=(int)(500 - 500.0*(config.frame_speed-10)/88);
    if(incs<0)incs=0;
    next_frame_time = curTime + incs;

    return 1;
}

//--------- End of function Sys::should_next_frame ---------//

//-------- Begin of function Sys::auto_save --------//
//!
void Sys::auto_save() {
    // auto-save per year in ver2

    if( config.auto_save && day_frame_count==0
	&& info.game_day==player_school.trimester_array[AUTUMN].start_day
	&& info.game_month==player_school.trimester_array[AUTUMN].start_month ) {
	if( config.auto_save && config.auto_save_file_name[0] ) {
	    // append extension
	    // find the last '.', digits before this '.' will be replaced by <year>
	    char *dotPtr, *pathPtr;
	    if( (dotPtr=strrchr(config.auto_save_file_name, '.'))
		// make sure '.' in filename, not in path name
		&& (!(pathPtr=strrchr(config.auto_save_file_name,'\\')) || pathPtr < dotPtr ) ) {
		if( !pathPtr )
		    pathPtr = config.auto_save_file_name-1;

		// unit digit
		if( dotPtr-pathPtr > 1 ) {                // at least 1 char before '.'
		    dotPtr[-1] = '0' + info.game_year % 10;

		    // ten digit
		    if( dotPtr-pathPtr > 2                  // at least 2 char before '.'
			// if isdigit
			&& dotPtr[-2] >= '0' && dotPtr[-2] <= '9' ) {
			dotPtr[-2] = '0' + (info.game_year / 10) % 10;

			// hundred digit
			if( dotPtr-pathPtr > 3                // at least 3 char before '.'
			    // if isdigit
			    && dotPtr[-3] >= '0' && dotPtr[-3] <= '9' )
			    dotPtr[-3] = '0' + (info.game_year / 100) % 10;
		    }
		}
	    }

	    game_file.save_game( config.auto_save_file_name );
	}
    }

    // write log file in ver2

    if( config.log_enable && day_frame_count==0 ) {
	int writeLogFlag = 0;
	switch(config.log_enable) {
	case INPUT_MEDIUM:                          // per year
	    if( info.game_day == player_school.trimester_array[0].start_day
		&& info.game_month == player_school.trimester_array[0].start_month )
		writeLogFlag = 1;
	    break;

	case INPUT_HIGH: {                          // per trimester
	    for( int trimester = 0; trimester < TRIMESTER_PER_YEAR; ++trimester ) {
		if( info.game_day == player_school.trimester_array[trimester].start_day
		    && info.game_month == player_school.trimester_array[trimester].start_month )
		    writeLogFlag = 1;
	    }
	}
	break;
	}

	if( writeLogFlag ) {
	    // append one line to log file

	    if( config.log_enable && config.log_file_name[0] ) {
		if( !m.is_file_exist( config.log_file_name ) )
		    // log file may removed, re-create now
		    game_file.create_game_log(config.log_file_name);

		game_file.append_game_log(config.log_file_name);
	    }
	}
    }

#ifdef NO_AUTOSAVE
    return;
#endif

    //---------- single player auto save ----------//

    if ( info.game_month%4==0 && info.game_day==1 && day_frame_count==0) {
#ifdef DEBUG_SKIP
	if(1)
#else
	    if( debug_session || testing_session )
#endif
	    {
		static int saveCount = 0;
		switch(saveCount) {
		case 0:  game_file.save_game( "AUTO1.SAV" );
		    break;
		case 1:  game_file.save_game( "AUTO2.SAV" );
		    break;
		case 2:  game_file.save_game( "AUTO3.SAV" );
		    break;
		}

		// #### begin Gilbert 30/08/2001 ######//
		//if( ++saveCount>3 )
		if( ++saveCount>=3 )
		    saveCount = 0;
		// #### end Gilbert 30/08/2001 ######//
	    }
	    else {
		//--- rename the existing AUTO.SAV to AUTO2.SAV and save a new game ---//

		if( m.is_file_exist( "AUTO.SAV" ) ) {
		    if( m.is_file_exist( "AUTO2.SAV" ) )      // if there is already an AUTO2.SAV, delete it
			remove( "AUTO2.SAV" );

		    rename( "AUTO.SAV", "AUTO2.SAV" );
		}
	    }

	game_file.save_game( "AUTO.SAV" );

	//-*********** syn game test ***********-//
#ifdef DEBUG_SKIP
	if(debug_seed_status_flag==DEBUG_SYN_AUTO_SAVE) {
	    sp_write_seed();
	    sp_close_seed_file();

	    if(0) {
		sp_open_seed_file("nseed.rs");
		// continue recording
		debug_seed_status_flag = DEBUG_SYN_AUTO_SAVE;
	    }
	    else {
		debug_seed_status_flag = NO_DEBUG_SYN;
		//sp_load_seed_file();
		//SendMessage(main_hwnd, WM_KEYDOWN, 'L', 0);
		// load file for comparison
		mouse.add_key_event(DIK_BACKSLASH, m.get_time());
	    }
	}

	//debug_seed_status_flag = 2;
	//sp_seed_pos_reset();
	//sp_record_match_seed();
#endif
	//-*********** syn game test ***********-//
    }
}

//-------- End of function Sys::auto_save --------//
