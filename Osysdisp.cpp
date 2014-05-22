//Filename    : OSYSDISP.CPP
//Description : Sys class functions for displaying

#include <ALL.H>
#include <ODEBUGX.H>
#include <OINFO.H>
#include <OMISC.H>
#include <OCONFIG.H>
#include <OMOUSE.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OVGA.H>
#include <OWORLD.H>
#include <OIFACE.H>
#include <OFINANCE.H>
#include <OFACURES.H>
#include <OPSCHOOL.H>
#include <OADM_OFF.H>
#include <OOPT.H>
#include <OCHANCE.H>
#include <OATHLETI.H>
#include <ODEVELOP.H>
#include <OFACILIT.H>
#include <OLIBTECH.H>
#include <OINVEST.H>
#include <OTEST.H>                                //Norris 0629
#include <OHELPIF.H>                              //chwg 1015
#include <OLETTER.H>                              //chwg 1021
#include <OBLCKBRD.H>                             //chwg 1026
#include <OMISCREP.H>
#include <OMORERPT.H>                             //chwg 0107
#include <ODEPTLST.H>                             //## chwg010999
#include <OPRTMENU.H>                             //## chwg010999
#include <ODEPT.H>
#include <OCRIME.H>
#include <OPARKING.H>
#include <ORETIRE.H>                              //## chwg021299
#include <OBANKRUP.H>                             //## chwg021299
#include <OFINSCOR.H>                             //## chwg021299
#include <OSCEN_IF.H>                             //## chwg021299
#include <OLOSEGAM.H>                             //## chwg021299

#include <OHELP.H>                                // ## chwg031099

//-------- Begin of function Sys::disp_frame --------//
enum {
    NEWS_POPUP_X1 = 120,
    NEWS_POPUP_Y1 = 130,
    NEWS_POPUP_X2 = NEWS_POPUP_X1+532,
    NEWS_POPUP_Y2 = NEWS_POPUP_Y1+222
};

void Sys::disp_frame() {

    if( sys.signal_exit_flag )
	return;

    //------- refresh info display --------//

    info.disp();

    if (prev_view_mode != view_mode) {
	deinit_report();                              // deinit the current report, free up allocated memory and etc.

	//---- update prev_view_mode ----//

	prev_view_mode = view_mode;
    }

    //## chwg021999
    if(news_pop_up_flag)
	vga.dont_blt=1;                               // if using pop_up menu, dont_blt first

    //## chwg021999
    //	if(help.should_disp())
    //		vga.dont_blt=1;		// if show short help, dont_blt first

    /*
      //------ if in staying view mode ------//
      if( staying_view_mode && staying_view_mode != view_mode
      && staying_view_mode != MODE_EVALUATION_LETTER )
      {
      vga_back.bar(260, 515, 620, 590, V_BLACK );
      vga.use_back();
      font_fblack.put_paragraph( 280, 535, 610, 570,
      "CLICK HERE TO GO BACK TO RESOURCE OPTIMIZATION SCREEN" );
      vga.blt_buf(260, 515, 620, 590);
      }
      else
      {
      //		blackboard.black_message_report(sys.redraw_all_flag);
      }
    */

    //-------- re-draw the whole screen if needed, such as after task switching ---------//

    if( redraw_all_flag ) {
	int mail_state=0;
	if( (view_mode==MODE_WELCOME_LETTER)
	    ||  (view_mode==MODE_EVALUATION_LETTER)
	    ||  (view_mode==MODE_SCEN_WELCOME_LETTER)
	    || (view_mode==MODE_STATE_REPORT)
	    || (view_mode==MODE_YEAR_END_REPORT)
	    )
	    mail_state=1;
	if(player_school.scenario_id>0 && (view_mode==MODE_SCENARIO_LETTER||view_mode==MODE_SCEN_WELCOME_LETTER))
	    mail_state=1;

	set_speed(-1);                                // refresh the speed indicator	//##chwg1008

	mouse.hide_area( 8, 562, 74, 599 );           // ## chw1014
	if((view_mode!=MODE_PAUSE)
	   &&(view_mode!=MODE_AWARD_REPORT)
	   &&(view_mode!=MODE_GAME_OPTIONS)
	   &&(view_mode!=MODE_YEAR_END_REPORT)
	   &&(view_mode!=MODE_PRINTING_MENU)) {
	    image_interface.put_back( 8, 562, mail_state==1 ? "MAIL-DN" : "MAIL-UP" );
	}
	else {
	    image_interface.put_back( 8, 562, "MAIL-UP" );
	    user_interface.darken(8, 562, 74, 599);
	}
	/*		if(sys.view_mode==MODE_PRINTING_MENU)
			if(	(print_menu.print_view_mode==MODE_WELCOME_LETTER)
			||	(print_menu.print_view_mode==MODE_EVALUATION_LETTER)
			||	(print_menu.print_view_mode==MODE_SCEN_WELCOME_LETTER)
			||	(print_menu.print_view_mode==MODE_SCENARIO_LETTER))
			image_interface.put_back( 8, 562, "MAIL-DN");
			mouse.show_area();                            */
	vga.blt_buf(8, 562, 74, 599 ,1);

	mouse.hide_area( 178, 515, 208, 541 );        // ## chw1014
	image_interface.put_back( 178, 515, world.zoom_level==1 ? "MAG-UP" : "MAG-DN" );
	mouse.show_area();
	vga.blt_buf( 178, 515, 208, 541 ,1);

	mouse.hide_area(177, 561, 207, 591);          // ## chw1014
	if((view_mode!=MODE_NORMAL)
	   &&(view_mode!=MODE_HELP_DESC)
	   &&(view_mode!=MODE_HELP_TOPIC)
	   &&(view_mode!=MODE_MORE_REPORT)
	   &&(view_mode!=MODE_PAUSE)
	   &&(view_mode!=MODE_DEPT_LIST)
	   &&(view_mode!=MODE_GAME_OPTIONS)
	   &&(view_mode!=MODE_AWARD_REPORT)
	   &&(view_mode!=MODE_YEAR_END_REPORT)) {
	    image_interface.put_back( 180, 564, "PRINT-UP" );
	}
	else {
	    image_interface.put_back( 180, 564, "PRINT-DN" );
	    user_interface.darken(180, 564, 180+26, 564+28);
	}
	mouse.show_area();
	vga.blt_buf(180, 564, 180+26, 564+28);
	/*
	  if((view_mode==MODE_PAUSE)
          ||(view_mode==MODE_GAME_OPTIONS)
          ||(view_mode==MODE_YEAR_END_REPORT)
          ||(view_mode==MODE_EVALUATION_LETTER)
          ||(view_mode==MODE_SCEN_WELCOME_LETTER)
          ||(view_mode==MODE_WELCOME_LETTER)
          ||(view_mode==MODE_FINAL_SCORE)
          ||(view_mode==MODE_AWARD_REPORT)
          ||(view_mode==MODE_YEAR_END_REPORT)
          ||(view_mode==MODE_SCENARIO_LETTER)
	  ||(view_mode==MODE_RETIREMENT)
	  ||(view_mode==MODE_BANKRUPTCY)
	  ||(view_mode==MODE_LOSE_GAME)
	  ||(view_mode==MODE_FINAL_SCORE)
	  )
	  {
	  vga_back.put_bitmap(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, info.report_bitmap[0]);		// the report does not belong to any of the buttons
	  user_interface.darken(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, REPORT_BUTTON_X2, REPORT_BUTTON_Y2-1);
	  vga.blt_buf(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, REPORT_BUTTON_X2, REPORT_BUTTON_Y2-1);
	  }
	*/
	sys.m_o_log_display_updated=1;
    }

    if( view_mode == MODE_NORMAL ) {
	if( redraw_all_flag ) {
	    // set the top bar to display the user university name
	    sys.is_institution_report = 1;

	    world.map_matrix.reset_image_buf();         // force ZoomMatrix to redraw the map

	    world.draw_all();

	    info.disp_6_graphs();

	    info.disp_column_bitmap(&vga_back);

#ifdef DEBUG
	    // display the value of ck1, ck2, ck3 in player_school
	    //	float ck1, ck2, ck3;
	    //	player_school.get_cks( ck1, ck2, ck3 );
	    //	font_chartsm.put( 200, 100, "CK1:" );
	    //	font_chartsm.put( 200, 130, m.format(ck1) );
	    //	font_chartsm.put( 200, 180, "CK2:" );
	    //	font_chartsm.put( 200, 210, m.format(ck2) );
	    //	font_chartsm.put( 200, 260, "CK3:" );
	    //	font_chartsm.put( 200, 290, m.format(ck3) );
#endif

	    set_view_mode(view_mode);                   // for displaying the pressed button on the top of the screen

	    redraw_all_flag = 0;
	    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2,1);
	    vga.blt_buf(MAP_X1, MAP_Y1, MAP_X2, MAP_Y2,1);
	}
	else if( redraw_zoom_flag ) {
	    // world.map_matrix.reset_image_buf();

	    world.draw_all();                           // ZoomMatrix will use the last saved image buf instead of redrawing a new map
	    info.disp_6_graphs();

	    info.disp_column_bitmap(&vga_back);
	    set_speed(-1);                              // refresh the speed indicator	//##chwg1008
	    redraw_zoom_flag = 0;
	    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2,1);
	    vga.blt_buf(MAP_X1, MAP_Y1, MAP_X2, MAP_Y2,1);
	}
	else {
	    //			world.draw_update();
	    //			info.disp_6_graphs();
	}
    }
    else {
	int refreshFlag = INFO_UPDATE;

	if( redraw_all_flag || redraw_zoom_flag ) {
	    refreshFlag     = INFO_REPAINT;
	    redraw_all_flag  = 0;
	    redraw_zoom_flag = 0;
	}

	//----------------------------------------------------------//
	//
	// Comments:	the info.report_bitmap[] is an array of bitmaps
	//					for the display of the upper-left corner button list
	//					[0] - bitmap with no button pressed
	//					[1] - first button pressed, etc.
	//					the array is initialzed at Info::disp_panel_init
	//
	//----------------------------------------------------------//

	//---- if the mode is set to the detail screen of a building -----//

	if( view_mode >= MODE_FIRST_NONDEPARTMENTAL_FIRM &&
	    view_mode <= MODE_LAST_NONDEPARTMENTAL_FIRM ) {
	    sys.is_institution_report=0;

	    switch( FIRST_NONDEPARTMENTAL_FIRM_ID + view_mode - MODE_FIRST_NONDEPARTMENTAL_FIRM ) {
	    case FIRM_ADMISSIONS:
		adm_office.report(refreshFlag);
		break;

	    case FIRM_DEVELOPMENT:
		development_office.report(refreshFlag);
		break;

	    case FIRM_LIBRARY:
		library_tech_office.report(refreshFlag);
		break;

	    case FIRM_FACILITIES:
		facility_office.report(refreshFlag);
		break;

	    case FIRM_OLD_MAIN:
		investment_office.report(refreshFlag);
		break;

	    case FIRM_PRESIDENT: {
		if(info.year_passed<=1) {
		    set_staying_view_mode(MODE_WELCOME_LETTER);
		}
		else {
		    set_staying_view_mode(MODE_EVALUATION_LETTER);
		}
	    }
	    break;

	    case FIRM_TOWER:
		player_school.score_report(refreshFlag);
		break;

	    case FIRM_PARK_LOTS:
	    case FIRM_PARK_STRUCTURE:
		parking.parking_report(refreshFlag);
		break;

	    case FIRM_POLICE:
		crime.crime_report(refreshFlag);
		break;

	    case FIRM_STADIUM:
	    case FIRM_PLAY_FIELD:
	    case FIRM_INDOOR_SPORTS:
		athletics_office.report(refreshFlag);
		break;

	    case FIRM_RESIDENCE:
		player_school.summary_student_report_call_residence();
		player_school.summary_student_report(refreshFlag);
		set_view_mode(MODE_STUDENT_REPORT);
		break;
	    }
	}
	else {
	    //----- if the mode is set to display one of the building independent reports ----//

	    switch( view_mode ) {
	    case MODE_NORMAL:
		sys.is_institution_report=0;
		world.detect();
		break;

	    case MODE_PAUSE:
		if(refreshFlag==INFO_REPAINT) {
		    user_interface.brighten(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
		    user_interface.brighten(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

		    info.disp_column_bitmap(&vga_back);
		    font_yellow_diamond.put(VGA_WIDTH/2-40,VGA_HEIGHT/3+50,"PAUSE GAME");
		    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2,1);
		}
		break;

	    case MODE_SCENARIO_LETTER:
		scenario_report.scenario_report_report(refreshFlag);
		break;

	    case MODE_RETIREMENT:
		retirement.retirement_report(refreshFlag);
		break;

	    case MODE_BANKRUPTCY:
		bankruptcy.bankruptcy_report(refreshFlag);
		break;

	    case MODE_LOSE_GAME:
		lose_game.lose_game_report(refreshFlag);
		break;

	    case MODE_FINAL_SCORE:
		final_score.final_score_report(refreshFlag);
		break;

	    case MODE_FACULTY_LIST:
		faculty_res.report(refreshFlag);
		break;

	    case MODE_FACULTY_SUMMARY:
		faculty_res.summary_report(refreshFlag);
		break;

	    case MODE_STUDENT_REPORT:
		player_school.summary_student_report(refreshFlag);
		break;

	    case MODE_FINANCIAL_REPORT:
		finance.disp_report_main(refreshFlag);
		break;

	    case MODE_PERFORMANCE_REPORT:
		department_array[department_array.selected_recno]->main_report(refreshFlag);
		break;

	    case MODE_COURSE_REPORT:
		department_array[department_array.selected_recno]->course_report(refreshFlag);
		break;

	    case MODE_SCORE_REPORT:
		player_school.score_report(refreshFlag);
		break;

	    case MODE_YEAR_END_REPORT:
		finance.new_fiscal_year_report(refreshFlag);
		break;

	    case MODE_GAME_OPTIONS:
		sys.setting_report(refreshFlag);
		break;

	    case MODE_HELP_TOPIC:
		help_if.help_topic_report(refreshFlag);
		break;

	    case MODE_HELP_DESC:
		help_if.help_desc_report(refreshFlag);
		break;

	    case MODE_SCEN_WELCOME_LETTER:
		letter.scen_welcome_letter_report(refreshFlag);
		break;

	    case MODE_WELCOME_LETTER:
		letter.welcome_letter_report(refreshFlag);
		break;

	    case MODE_EVALUATION_LETTER:
		letter.eval_letter_report(refreshFlag);
		break;

	    case MODE_STATE_REPORT:
		letter.state_report(refreshFlag);
		break;

	    case MODE_BLACKBOARD_LOG:
		blackboard.black_log_report(refreshFlag);
		break;

	    case MODE_AWARD_REPORT:
		if(misc_report.page_num==0)misc_report.page_num=1;
		misc_report.award_report(refreshFlag);
		break;

	    case MODE_MORE_REPORT:
		more_report.more_report_report(refreshFlag);
		break;

	    case MODE_DEPT_LIST:
		dept_list.dept_list_report(refreshFlag);
		break;

	    case MODE_PRINTING_MENU:
		print_menu.report(refreshFlag);
		break;

	    }
	}
    }

    vga_back.rest_area(menu_pull_down ? info.menu_down_bitmap : info.menu_up_bitmap, 0, 0);
    vga.blt_buf(MENU_BUTTON_X1, MENU_BUTTON_Y1, MENU_BUTTON_X2, MENU_BUTTON_Y2);

    //-------- display depat list button ---------------//
    //	vga_back.rest_area(list_button_bitmap,0,0);
    image_interface.put_back(LIST_BUTTON_X1, LIST_BUTTON_Y1, "LIST_UP" );
    vga.blt_buf(LIST_BUTTON_X1, LIST_BUTTON_Y1, LIST_BUTTON_X2, LIST_BUTTON_Y2);

    //----------- Display newspaper -------------//

    if(news_pop_up_flag) {
	image_interface.put_back(NEWS_POPUP_X1, NEWS_POPUP_Y1, "NEWSPOP" );

	if(config.frame_speed!=0)
	    save_speed = config.frame_speed;

	set_speed(0);                                 // set the gaming speed to frozen, the speed is frozen until the player reset the staying view_mode

	vga.use_back();

	font_letter.put_paragraph(
	    NEWS_POPUP_X1+40, NEWS_POPUP_Y1+110, NEWS_POPUP_X2-30, NEWS_POPUP_Y2,
	    chance_event.news_news_str.str_buf,8);

	font_letter.put(NEWS_POPUP_X1+200, NEWS_POPUP_Y1+77,chance_event.news_date_str);
	vga.blt_buf(NEWS_POPUP_X1, NEWS_POPUP_Y1, NEWS_POPUP_X2, NEWS_POPUP_Y2);
    }

    if(vga.dont_blt) {
	vga.dont_blt=0;
	vga.blt_buf(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);
	vga.blt_buf(LIST_BUTTON_X1, LIST_BUTTON_Y1, LIST_BUTTON_X2, LIST_BUTTON_Y2);
	vga.blt_buf(MENU_BUTTON_X1, MENU_BUTTON_Y1, MENU_BUTTON_X2, MENU_BUTTON_Y2);
	vga.blt_buf(MAP_X1, MAP_Y1, MAP_X2, MAP_Y2, 1);
	vga.blt_buf(8, 562, 74, 599 ,1);              // mail
	vga.blt_buf(178, 515, 208, 541 ,1);           // mag. glass
	//		vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1);
    }

    help.disp();

    if(news_pop_up_flag) {
	vga_back.put_bitmap( 110, 539, image_interface.get_ptr("SPED-BAS"));
	vga.blt_buf(110, 539, 110+63, 539+30);
    }

    //--------------------------------//
    blt_virtual_buf();
}

//-------- End of function Sys::disp_frame --------//

//-------- Begin of function Sys::deinit_report --------//
//!
//! Deinit the current report, free up allocated memory and etc.
//!
void Sys::deinit_report() {
    if( prev_view_mode >= MODE_FIRST_NONDEPARTMENTAL_FIRM &&
	prev_view_mode <= MODE_LAST_NONDEPARTMENTAL_FIRM ) {
	switch( FIRST_NONDEPARTMENTAL_FIRM_ID + prev_view_mode
		- MODE_FIRST_NONDEPARTMENTAL_FIRM ) {
	case FIRM_ADMISSIONS:
	    adm_office.deinit();
	    break;

	case FIRM_STADIUM:
	    athletics_office.deinit();
	    break;

	case FIRM_DEVELOPMENT:
	    development_office.deinit();
	    break;

	case FIRM_LIBRARY:
	    library_tech_office.deinit();
	    break;

	case FIRM_FACILITIES:
	    facility_office.deinit();
	    break;

	case FIRM_PRESIDENT:
	    letter.deinit();
	    break;

	case FIRM_TOWER:
	    player_school.score_report_deinit();
	    break;

	case FIRM_OLD_MAIN:
	    investment_office.deinit();
	    break;

	case FIRM_RESIDENCE:
	    player_school.summary_student_report_deinit();
	    break;

	case FIRM_PARK_LOTS:
	case FIRM_PARK_STRUCTURE:
	    parking.deinit();
	    break;

	case FIRM_POLICE:
	    crime.deinit();
	    break;
	}
    }
    else {
	switch( prev_view_mode ) {
	case MODE_FACULTY_SUMMARY:
	    faculty_res.summary_report_deinit();
	    break;

	case MODE_STUDENT_REPORT:
	    player_school.summary_student_report_deinit();
	    break;

	case MODE_COURSE_REPORT:
	    department_array[department_array.selected_recno]->course_report_deinit();
	    break;

	case MODE_SCORE_REPORT:
	    player_school.score_report_deinit();
	    break;

	case MODE_FINANCIAL_REPORT:
	    finance.report_deinit();
	    break;

	case MODE_PERFORMANCE_REPORT:
	    department_array[department_array.selected_recno]->main_report_deinit();
	    break;

	case MODE_GAME_OPTIONS:
	    sys.setting_report_deinit();
	    break;

	case MODE_HELP_TOPIC:
	    help_if.deinit();
	    break;

	case MODE_HELP_DESC:
	    help_if.deinit();
	    break;

	case MODE_WELCOME_LETTER:
	    letter.deinit();
	    break;

	case MODE_SCEN_WELCOME_LETTER:
	    letter.deinit();
	    break;

	case MODE_EVALUATION_LETTER:
	    letter.deinit();
	    break;

	case MODE_STATE_REPORT:
	    letter.deinit();
	    break;

	case MODE_DEPT_LIST:
	    dept_list.deinit();
	    break;

	case MODE_MORE_REPORT:
	    more_report.deinit();
	    break;

	case MODE_PRINTING_MENU:
	    print_menu.deinit();
	    break;

	    //## fred 0213
	case MODE_AWARD_REPORT:
	    misc_report.award_deinit();
	    break;
	    //##

	}
    }
}

//-------- End of function Sys::deinit_report --------//

//-------- Begin of function Sys::blt_virtual_buf --------//
void Sys::blt_virtual_buf() {
    // add in (GAME_VERSION>=200)
    // if( !debugx.debug_session )
    if( !use_true_front )
	return;

    //--- in a debug sesion, vga_front is not the true front buffer, now copy it to the true one ---//

    int frontLocked=0;

    if( vga_front.buf_locked ) {
	vga_front.unlock_buf();
	frontLocked=1;
    }

    RECT bltRect;

    bltRect.left   = 0;
    bltRect.top    = 0;
    bltRect.right  = VGA_WIDTH-1;
    bltRect.bottom = VGA_HEIGHT-1;

    POINT destPoint;
    destPoint.x = 0;
    destPoint.y = 0;

    if( use_true_front == 2 ) {                     // window mode
	ClientToScreen( main_hwnd, &destPoint );
    }

    int rc = vga_true_front.dd_buf->BltFast(
	destPoint.x, destPoint.y,
	vga_front.dd_buf,                             // src surface
	&bltRect,                                     // src rect (all of it)
	DDBLTFAST_WAIT );

    if( frontLocked )
	vga_front.lock_buf();
}

//--------- End of function Sys::blt_virtual_buf ---------//

//-------- Begin of function Sys::set_news_pop_up --------//
//!
void Sys::set_news_pop_up() {
    news_pop_up_flag = 1;
    news_pop_up_time = m.get_time();
}

//-------- End of function Sys::set_news_pop_up --------//
