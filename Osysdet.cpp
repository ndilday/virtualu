//Filename   : OSYSDET.CPP
//Description: System functions for detecting user inputs
//Owner		 : chwg
#include <OCONFIG.H>
#include <OMOUSE.H>
#include <OPOWER.H>
#include <OAUDIO.H>
#include <OVGA.H>
#include <OFACURES.H>
#include <OPSCHOOL.H>
#include <OFINANCE.H>

#include <OSYS.H>
#include <OFONT.H>
#include <OStr.H>                                 // for this's capture_screen
#include <OMisc.H>
#include <OVgaBuf.H>
#include <ODebugx.H>
#include <OMOUSE2.H>                              // for K_UNIQUE_KEY
#include <KEY.H>                                  // for KEY_F11
#include <OWORLD.H>                               // World::on_event
#include <OINFO.H>                                // for button coordinates - Norris 0713

#include <OADM_OFF.H>
#include <OOPT.H>
#include <OATHLETI.H>
#include <ODEVELOP.H>
#include <OLIBTECH.H>
#include <OFACILIT.H>
#include <OINVEST.H>
#include <ODEPT.H>
#include <OTEST.H>
#include <OLETTER.H>                              // ##chwg1021
#include <OHELPIF.H>                              // ##chwg1015
#include <OBLCKBRD.H>                             // ##chwg1026
#include <OMORERPT.H>
#include <OMISCREP.H>
#include <ODEPTLST.H>                             //## chwg010999
#include <ORETIRE.H>                              //## chwg021299
#include <OBANKRUP.H>                             //## chwg021299
#include <OFINSCOR.H>                             //## chwg021299
#include <OSCEN_IF.H>                             //## chwg021299
#include <OLOSEGAM.H>                             //## chwg021299
#include <OPRTMENU.H>                             //## chwg021299
#include <OENROLL.H>
#include <OHELP.H>
#include <OIFACE.H>
#include <OPRINT.H>
#include <OBOX.H>
#include <OCRIME.H>
#include <OVGALOCK.H>

#include <OCHANCE.H>                              //## chea021000 for testing chance card only
//----- Define interface coordinates ------//

enum {
    REPORT_BUTTON_CAMPUS_X1 = 5,
    REPORT_BUTTON_CAMPUS_Y1 = 0,
    REPORT_BUTTON_CAMPUS_X2 = 56,
    REPORT_BUTTON_CAMPUS_Y2 = 32,
    REPORT_BUTTON_FACULTY_X1 = 57,
    REPORT_BUTTON_FACULTY_Y1 = 0,
    REPORT_BUTTON_FACULTY_X2 = 101,
    REPORT_BUTTON_FACULTY_Y2 = 32,
    REPORT_BUTTON_STUDENTS_X1 = 102,
    REPORT_BUTTON_STUDENTS_Y1 = 0,
    REPORT_BUTTON_STUDENTS_X2 = 155,
    REPORT_BUTTON_STUDENTS_Y2 = 32,
    REPORT_BUTTON_COURSES_X1 = 156,
    REPORT_BUTTON_COURSES_Y1 = 0,
    REPORT_BUTTON_COURSES_X2 = 205,
    REPORT_BUTTON_COURSES_Y2 = 32,
    REPORT_BUTTON_PERFORMANCE_X1 = 206,
    REPORT_BUTTON_PERFORMANCE_Y1 = 0,
    REPORT_BUTTON_PERFORMANCE_X2 = 276,
    REPORT_BUTTON_PERFORMANCE_Y2 = 32,
    REPORT_BUTTON_FINANCIAL_X1 = 277,
    REPORT_BUTTON_FINANCIAL_Y1 = 0,
    REPORT_BUTTON_FINANCIAL_X2 = 324,
    REPORT_BUTTON_FINANCIAL_Y2 = 32,
    REPORT_BUTTON_SCORE_X1 = 325,
    REPORT_BUTTON_SCORE_Y1 = 0,
    REPORT_BUTTON_SCORE_X2 = 361,
    REPORT_BUTTON_SCORE_Y2 = 32,
    MORE_REPORT_BUTTON_X1 = 10,
    MORE_REPORT_BUTTON_Y1 = 33,
    MORE_REPORT_BUTTON_X2 = 150,
    MORE_REPORT_BUTTON_Y2 = 45,
    NEWS_POPUP_X1 = 120,
    NEWS_POPUP_Y1 = 130,
    NEWS_POPUP_X2 = NEWS_POPUP_X1+532,
    NEWS_POPUP_Y2 = NEWS_POPUP_Y1+222
};
//-------- Begin of function Sys::detect --------//

static int m_o_log_state[3]={2,0,0};
static int m_o_log_bitmap_y[4]={0,27,57,80};

void Sys::detect() {
    //--- if it was previously in scenario letter or evaluation letter but it has just been set to a new mode, then set_staying_view_mode(0) to enable the speed to the original value ---//

    if( (staying_view_mode == MODE_SCENARIO_LETTER && view_mode != MODE_SCENARIO_LETTER) ||
	(staying_view_mode == MODE_EVALUATION_LETTER && view_mode != MODE_EVALUATION_LETTER)
	|| (staying_view_mode == MODE_STATE_REPORT && view_mode != MODE_STATE_REPORT)
	|| ((staying_view_mode == MODE_YEAR_END_REPORT && view_mode != MODE_YEAR_END_REPORT)
	    && (staying_view_mode == MODE_YEAR_END_REPORT && view_mode != MODE_HELP_DESC))
	) {
	save_view_mode = view_mode;
	set_staying_view_mode(0);
    }

    //--- if the user has just switched mode, don't detect as it will detect the new mode without first initializing the controls of the new mode ----//

    if( redraw_all_flag || redraw_zoom_flag )
	return;

    //--------------------------------------//

    mouse.get_event();
    /*
      if( in_game_menu.is_active() )
      {
      in_game_menu.detect();
      return;
      }
    */

    // BUGHERE
    // Test for context help
    //	help.set_custom_help(0,0,100,100,"Test title","Test detail, Why I cannot see anything?");
    //	help.set_help(200,200,400,400,"COMPUTER");

    if(news_pop_up_flag) {
	// 5 second timeout
	if( mouse.single_click(2) || m.get_time() > news_pop_up_time + 8000 ) {
	    set_speed(save_speed);
	    news_pop_up_flag=0;
	    redraw_all_flag=1;
	}
    }

    if((view_mode==MODE_BANKRUPTCY)
       ||(view_mode==MODE_RETIREMENT)
       ||(view_mode==MODE_FINAL_SCORE)
       ||(view_mode==MODE_LOSE_GAME)
       ||(view_mode==MODE_AWARD_REPORT)) {
	detect_view();
	return;
    }

    if(view_mode==MODE_PAUSE) {
	if( mouse.is_key_event() )
	    if( m.upper(mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_UNIQUE_KEY))=='P' ) {
		pause_game_flag = 1 - pause_game_flag;      // toggle menu pull down
		if(pause_game_flag)
		    set_staying_view_mode(MODE_PAUSE);
		else {
		    set_staying_view_mode(0);
		    //			redraw_all_flag=1;
		}
	    }
	return;
    }

    // ## begin chwg 1106
    // Used to disable all buttons in the screen while in these mode
    //	MODE_YEAR_END_REPORT,MODE_EVALUATION_LETTER and MODE_WELCOME_LETTER

    // Detect help question mark button only if in help_topic or help_desc report
    if((view_mode==MODE_HELP_TOPIC)||
       (view_mode==MODE_HELP_DESC)) {
	help.set_help(HELP_QUEST_X1,HELP_QUEST_Y1,HELP_QUEST_X2,HELP_QUEST_Y2,"U13.");
	if (mouse.single_click(HELP_QUEST_X1, HELP_QUEST_Y1, HELP_QUEST_X2, HELP_QUEST_Y2)) {
	    audio.play_wav("STONES",audio.int_to_DsVolume(config.sound_effect_volume));

	    if(view_mode!=MODE_HELP_TOPIC) {
		if(view_mode!=MODE_DEPT_LIST && view_mode!=MODE_MORE_REPORT
		   && view_mode!=MODE_HELP_TOPIC)
		    report_before_morerpt_or_deptlist=view_mode;
		else
		    report_before_morerpt_or_deptlist=MODE_NORMAL;

		set_view_mode(MODE_HELP_TOPIC);
	    }
	    else
		set_view_mode(report_before_morerpt_or_deptlist);

	    return;
	}
	detect_view();
	return;
    }

    if(view_mode==MODE_YEAR_END_REPORT) {
	finance.new_fiscal_year_report_detect();
	return;
    }

    if(view_mode==MODE_EVALUATION_LETTER) {
	// BUGHERE : temporary fix the bug which call in-correct
	// detect function when viewing evaluation letter (>1year and not in year-end report)
	if( save_view_mode==MODE_YEAR_END_REPORT ) {
	    finance.new_fiscal_year_report_detect();
	    return;
	}
    }

    if(view_mode==MODE_WELCOME_LETTER) {
	letter.welcome_letter_detect();
	detect_mail_button();
	return;
    }

    if(view_mode==MODE_SCEN_WELCOME_LETTER) {
	letter.scen_welcome_letter_detect();
	detect_mail_button();
	return;
    }

    if(view_mode==MODE_STATE_REPORT) {
	finance.new_fiscal_year_report_detect();
	//		detect_mail_button();
	return;
    }

    if(player_school.scenario_id>0 && view_mode==MODE_SCENARIO_LETTER) {
	scenario_report.scenario_report_detect();
	if(sys.view_mode!=MODE_NORMAL)
	    detect_mail_button();
	//		return;
    }

    // ## end chwg 1106

    //## chea 220699
    //## play department sound
    //#begin zhoubin 02/23/99
    //	remove static int old_Dept=-1;
    //#End zhoubin 02/23/99
    int dept=0;
    int deptNo=0;
    //##begin zhoubin 02/23/99
    if(old_Dept!=department_array.selected_recno&&bDept)
	//##end zhoubin 02/23/99
	if(
	    (view_mode!=MODE_SCEN_WELCOME_LETTER)
	    &&(view_mode!=MODE_WELCOME_LETTER)
	    ) {
	    dept=department_array.selected_recno;

	    deptNo = department_array[dept]->department_id;

	    // play sound
	    switch(deptNo) {
	    case 1:
		audio.play_wav("AGRICULT",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 2:
		audio.play_wav("TECHSERV",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 3:
		audio.play_wav("BUSINESS",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 4:
		audio.play_wav("COMMUNIC",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 5:
		audio.play_wav("EDUCATIO",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 6:
		audio.play_wav("ENGICONS",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 7:
		audio.play_wav("FORELANG",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 8:
		audio.play_wav("HEALSERV",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 9:
		audio.play_wav("AMERSTUD",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
		//		case 10:
		//			audio.play_wav("classic",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
		//		case 11:
		//			audio.play_wav("ENGLISH",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 12:
		audio.play_wav("HISTORY",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
		//		case 13:
		//			audio.play_wav("Philoso",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 14:
		audio.play_wav("ENVIBROO",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 15:
		audio.play_wav("GENDETHN",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 16:
		audio.play_wav("BIOLOGY",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 17:
		audio.play_wav("CHEMISTR",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
		//		case 18:
		//			audio.play_wav("Physics",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 19:
		audio.play_wav("COMPSCIE",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 20:
		audio.play_wav("MATHEMAT",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 21:
		audio.play_wav("ANTHSOCI",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 22:
		audio.play_wav("ECONOMIC",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 23:
		audio.play_wav("LAW",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
		//		case 24:
		//			audio.play_wav("Political Science",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
		//		case 25:
		//			audio.play_wav("Psychology",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
		//		case 26:
		//			audio.play_wav("Art",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
		//		case 27:
		//			audio.play_wav("Drama",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    case 28:
		audio.play_wav("MUSIC",audio.int_to_DsVolume(config.sound_effect_volume));
		break;
	    default:
		break;

	    }

	    old_Dept=department_array.selected_recno;
	}
    //## chea 220699

    if( detect_report_button() )
	return;

    // No further detection if game_options window is opened! //
    if(view_mode==MODE_GAME_OPTIONS) {
	setting_report_detect();
	if( mouse.is_key_event() )
	    if( m.upper(mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_UNIQUE_KEY))==KEY_F10 ) {
		menu_pull_down = 1 - menu_pull_down;        // toggle menu pull down
		if(menu_pull_down) {
		    world.draw_all();
		    info.disp_column_bitmap(&vga_back);
		    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2,1);
		    set_staying_view_mode(MODE_GAME_OPTIONS);
		}
		else
		    set_staying_view_mode(0);
	    }
	return;
    }

    if( mouse.is_key_event() ) {
	if( detect_cheat_key(mouse.scan_code, mouse.event_skey_state) )
	    return;

	process_key(mouse.scan_code, mouse.event_skey_state);
    }

    if( detect_dept_button() )
	return;

    if( detect_mail_button() )
	return;

    if( detect_function_button() )
	return;

    detect_main_screen_help();

    detect_view();

    //------ if in staying view mode -------//

    if(staying_view_mode && staying_view_mode!=MODE_EVALUATION_LETTER) {
	if( mouse.single_click( 234, 535, 609, 570 ) ) {
	    redraw_all_flag=1;
	    set_view_mode(staying_view_mode);
	}
    }
}

//--------- End of function Sys::detect ---------//

//-------- Begin of function Sys::detect_view --------//

void Sys::detect_view() {
    //---- if the mode is set to the detail screen of a building -----//

    if( view_mode >= MODE_FIRST_NONDEPARTMENTAL_FIRM &&
	view_mode <= MODE_LAST_NONDEPARTMENTAL_FIRM ) {
	switch( FIRST_NONDEPARTMENTAL_FIRM_ID + view_mode - MODE_FIRST_NONDEPARTMENTAL_FIRM ) {
	case FIRM_ADMISSIONS:
	    adm_office.detect();
	    break;

	case FIRM_STADIUM:
	case FIRM_PLAY_FIELD:

	case FIRM_INDOOR_SPORTS:
	    athletics_office.detect();
	    break;

	case FIRM_DEVELOPMENT:
	    development_office.detect();
	    break;

	case FIRM_LIBRARY:
	    library_tech_office.detect();
	    break;

	case FIRM_FACILITIES:
	    facility_office.detect();
	    break;

	case FIRM_OLD_MAIN:
	    investment_office.detect();
	    break;

	case FIRM_PRESIDENT: {
	    if(info.year_passed<=1) {
		letter.welcome_letter_detect();
	    }
	    else {
		letter.eval_letter_detect();
	    }
	}
	break;

	case FIRM_TOWER:
	    player_school.score_report_detect();
	    break;

	case FIRM_RESIDENCE:
	    player_school.summary_student_report_detect();
	    break;
	case FIRM_POLICE:
	    crime.crime_detect();
	    break;
	}
    }
    else {
	//----- if the mode is set to display one of the building independent reports ----//

	switch( view_mode ) {

	case MODE_NORMAL:
	    // BUGHERE ##chwg1112
	    // temporary placing here
	    m_o_log_old_selected=-2;
	    world.detect();
	    break;

	case MODE_FACULTY_LIST:
	    faculty_res.report_detect();
	    break;

	case MODE_FACULTY_SUMMARY:
	    faculty_res.summary_report_detect();
	    break;

	case MODE_STUDENT_REPORT:
	    player_school.summary_student_report_detect();
	    break;

	case MODE_SCORE_REPORT:
	    player_school.score_report_detect();
	    break;

	case MODE_FINANCIAL_REPORT:
	    finance.report_detect();
	    break;

	case MODE_PERFORMANCE_REPORT:
	    department_array[department_array.selected_recno]->main_report_detect();
	    break;

	case MODE_COURSE_REPORT:
	    department_array[department_array.selected_recno]->course_report_detect();
	    break;

	case MODE_YEAR_END_REPORT:
	    finance.new_fiscal_year_report_detect();
	    break;

	case MODE_SCENARIO_LETTER:
	    scenario_report.scenario_report_detect();
	    break;

	case MODE_RETIREMENT:
	    retirement.retirement_detect();
	    break;

	case MODE_BANKRUPTCY:
	    bankruptcy.bankruptcy_detect();
	    break;

	case MODE_LOSE_GAME:
	    lose_game.lose_game_detect();
	    break;

	case MODE_FINAL_SCORE:
	    final_score.final_score_detect();
	    break;

	case MODE_MORE_REPORT:
	    more_report.more_report_detect();
	    break;

	case MODE_DEPT_LIST:
	    dept_list.dept_list_detect();
	    break;

	case MODE_HELP_TOPIC:
	    help_if.help_topic_detect();
	    break;

	case MODE_HELP_DESC:
	    help_if.help_desc_detect();
	    break;

	case MODE_SCEN_WELCOME_LETTER:
	    letter.scen_welcome_letter_detect();
	    break;

	case MODE_WELCOME_LETTER:
	    letter.welcome_letter_detect();
	    break;

	case MODE_EVALUATION_LETTER:
	    letter.eval_letter_detect();
	    break;

	case MODE_STATE_REPORT:
	    letter.state_report_detect();
	    break;

	case MODE_BLACKBOARD_LOG:
	    blackboard.black_log_detect();
	    break;

	case MODE_PRINTING_MENU:
	    print_menu.detect();
	    break;

	    //## fred 0213
	case MODE_AWARD_REPORT:
	    misc_report.award_detect();
	    break;
	    //##

	}
    }
}

//-------- End of function Sys::detect_view --------//

//-------- Begin of function Sys::process_key --------//
//!
void Sys::process_key(unsigned scanCode, unsigned skeyState) {
    int keyCode,i;
    String str,str2;

    if( (keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, K_UNIQUE_KEY)) ) {
	if(!news_pop_up_flag) {
	    if( keyCode == '9' )
		set_speed(99);

	    if( keyCode >= '0' && keyCode <= '8' ) {
		set_speed( (keyCode-'0') * 10 );
	    }
	    /*
	      if( keyCode == '[' )
	      {
              if(config.frame_speed-10<=0)
              {
	      set_speed(0);
	      sys.save_speed=0;
              }
              else
	      set_speed(config.frame_speed-10);

	      return;
	      }

	      if( keyCode == ']' )
	      {
	      set_speed(config.frame_speed+10);
	      return;
	      }

	      if( keyCode == '-' )				// lowest game speed
	      {
	      set_speed(0);
	      }

	      if( keyCode == '=' )			// highest game speed
	      {
	      set_speed(99);
	      }
	    */
	}

	//----------------------------------------------//

	switch(m.upper(keyCode)) {
	case 'Z':
	    world.set_zoom_level( world.zoom_level==1 ? 2 : 1 );
	    break;

	case 'R':
	    config.disp_year_end_report = !config.disp_year_end_report;
	    break;

	    //----- detect hot keys for reports -------//

	case KEY_F1:
	    audio.play_wav("CAMPUS",audio.int_to_DsVolume(config.sound_effect_volume));
	    set_view_mode(MODE_NORMAL);
	    break;

	case KEY_F2:
	    set_view_mode(MODE_FACULTY_SUMMARY);
	    break;

	case KEY_F3:
	    set_view_mode(MODE_STUDENT_REPORT);
	    break;

	case KEY_F4:
	    set_view_mode(MODE_COURSE_REPORT);
	    break;

	case KEY_F5:
	    set_view_mode(MODE_PERFORMANCE_REPORT);
	    break;

	case KEY_F6:
	    set_view_mode(MODE_FINANCIAL_REPORT);
	    break;

	case KEY_F7:
	    set_view_mode(MODE_SCORE_REPORT);
	    break;

	case KEY_F9:
	    set_view_mode(MODE_HELP_DESC);
	    back_view_mode=view_mode;
	    break;

	case KEY_F10:
	    menu_pull_down = 1 - menu_pull_down;      // toggle menu pull down
	    if(menu_pull_down) {
		world.draw_all();
		info.disp_column_bitmap(&vga_back);
		vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2,1);
		set_staying_view_mode(MODE_GAME_OPTIONS);
	    }
	    else
		set_staying_view_mode(0);
	    break;

	    //----- detect hot keys for building interfaces ------//

	case 'A':
	    set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_ADMISSIONS - FIRST_NONDEPARTMENTAL_FIRM_ID );
	    break;

	case 'T':
	    set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_STADIUM - FIRST_NONDEPARTMENTAL_FIRM_ID );
	    break;

	case 'D':
	    set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_DEVELOPMENT - FIRST_NONDEPARTMENTAL_FIRM_ID );
	    break;

	case 'F':
	    set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_FACILITIES - FIRST_NONDEPARTMENTAL_FIRM_ID );
	    break;

	case 'L':
	    set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_LIBRARY - FIRST_NONDEPARTMENTAL_FIRM_ID );
	    break;

	case 'C':
	    set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_POLICE - FIRST_NONDEPARTMENTAL_FIRM_ID );
	    break;

	case 'K':
	    set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_PARK_LOTS - FIRST_NONDEPARTMENTAL_FIRM_ID );
	    break;

	case 'I':
	    set_view_mode(MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_OLD_MAIN - FIRST_NONDEPARTMENTAL_FIRM_ID );
	    break;

	case KEY_F12:
	    for(i=0 ; i<=99 ; i++ ) {
		str = "VUSCR";
		if( i<10 )
		    str += "0";

		str += i;
		str += ".BMP";
		if( !m.is_file_exist(str) )
		    break;
	    }

	    if( i>99 )                                // all file names from VUSCR00 to VUSCR99 have been occupied
		return;

	    //vga.use_back();
	    //vga_back.bar(260, 515, 620, 590, V_BLACK );
	    str2="Capture screen to file ";
	    str2+=str;
	    str2+=" ";
	    //font_fblack.put(265,520,str2);
	    //vga.blt_buf(260, 515, 620, 590);
	    vga_front.write_bmp_file(str);

	    box.msg( str2 );
	    break;
	}
    }
}

//--------- End of function Sys::process_key ---------//

//-------- Begin of function Sys::detect_cheat_key --------//
//!
int Sys::detect_cheat_key(unsigned scanCode, unsigned skeyState) {
    int keyCode;

    if((keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, K_IS_CTRL))) {
	switch(m.upper(keyCode)) {
	case 'M': player_school.set_cks( 1, 0.1 ); return 1;
	case 'N': player_school.set_cks( 1, -0.1 ); return 1;
	case 'O': player_school.set_cks( 2, 0.1 ); return 1;
	case 'P': player_school.set_cks( 2, -0.1 ); return 1;
	case 'Q': player_school.set_cks( 3, 0.1 ); return 1;
	case 'R': player_school.set_cks( 3, -0.1 ); return 1;
	}
    }

#ifdef TEST_CHANCE

    if((keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, K_IS_CTRL))) {
	switch(m.upper(keyCode)) {
	    //----- shortcut for testing chance cards ----//
	case '1':
	    chance_event.test_chance=1;
	    break;
	case '2':
	    chance_event.test_chance=2;
	    break;
	case '3':
	    chance_event.test_chance=3;
	    break;
	case '4':
	    chance_event.test_chance=4;
	    break;
	case '5':
	    chance_event.test_chance=5;
	    break;
	case '6':
	    chance_event.test_chance=6;
	    break;
	case '7':
	    chance_event.test_chance=7;
	    break;
	case '8':
	    chance_event.test_chance=8;
	    break;
	case '9':
	    chance_event.test_chance=9;
	    break;
	case '0':
	    chance_event.test_chance=10;
	    break;
	case 'A':
	    chance_event.test_chance=11;
	    break;
	case 'B':
	    chance_event.test_chance=12;
	    break;
	case 'C':
	    chance_event.test_chance=13;
	    break;
	case 'D':
	    chance_event.test_chance=14;
	    break;
	case 'E':
	    chance_event.test_chance=15;
	    break;
	case 'F':
	    chance_event.test_chance=16;
	    break;
	case 'G':
	    chance_event.test_chance=17;
	    break;
	case 'H':
	    chance_event.test_chance=18;
	    break;
	case 'I':
	    chance_event.test_chance=19;
	    break;
	case 'J':
	    chance_event.test_chance=20;
	    break;
	case 'K':
	    chance_event.test_chance=NEWS_FOOTBALL_GAME;
	    break;
	case 'L':
	    chance_event.test_chance=NEWS_BASKETBALL_GAME;
	    break;

	default:
	    return 0;
	}

	return 1;
    }
#endif

    return 0;
}

//--------- End of function Sys::detect_cheat_key ---------//

//-------- Begin of function Sys::set_view_mode --------//
//!
void Sys::set_view_mode(int viewMode, int playSound) {
    view_mode = viewMode;

    if( playSound ) {
	switch(viewMode) {
	case  MODE_NORMAL:
	    //			audio.play_wav("CAMPUS",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case MODE_PERFORMANCE_REPORT:
	    //			audio.play_wav("FACULTY",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_FACULTY_SUMMARY:
	    audio.play_wav("FACULTY",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_COURSE_REPORT:
	    audio.play_wav("COURSE",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_STUDENT_REPORT:
	    audio.play_wav("STUDENTS",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_FINANCIAL_REPORT:
	    audio.play_wav("FINANCE",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_SCORE_REPORT:
	    audio.play_wav("SCORE",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	    //--------------------------------------------//

	case  MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_ADMISSIONS - FIRST_NONDEPARTMENTAL_FIRM_ID:
	    audio.play_wav("ADMISSN",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_STADIUM - FIRST_NONDEPARTMENTAL_FIRM_ID:
	    if( m.random(2) )                         // randomly choose between football and basketball sound
		audio.play_wav("BBALL",audio.int_to_DsVolume(config.sound_effect_volume));
	    else
		audio.play_wav("FOOTBALL",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_DEVELOPMENT - FIRST_NONDEPARTMENTAL_FIRM_ID:
	    audio.play_wav("DEVALUMN",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_FACILITIES - FIRST_NONDEPARTMENTAL_FIRM_ID:
	    audio.play_wav("FACMAN",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_LIBRARY - FIRST_NONDEPARTMENTAL_FIRM_ID:
	    audio.play_wav("INFOTECH",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_OLD_MAIN - FIRST_NONDEPARTMENTAL_FIRM_ID:
	    audio.play_wav("INVEST",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_POLICE - FIRST_NONDEPARTMENTAL_FIRM_ID:
	    audio.play_wav("CRIME",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case  MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_PARK_LOTS - FIRST_NONDEPARTMENTAL_FIRM_ID:
	    audio.play_wav("PARKING",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;
	}
    }

    if( view_mode >= MODE_NORMAL && view_mode <= MODE_SCORE_REPORT )
	vga_back.put_bitmap(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, info.report_bitmap[view_mode-MODE_NORMAL+1]);
    else
	// the report does not belong to any of the buttons
	vga_back.put_bitmap(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, info.report_bitmap[0]);

    vga.blt_buf(REPORT_BUTTON_X1, REPORT_BUTTON_Y1, REPORT_BUTTON_X2, REPORT_BUTTON_Y2);

    redraw_all_flag = 1;

    //------- initialize interface control vars when the user enters the screen ----//

    if( view_mode == MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRM_FACILITIES - FIRST_NONDEPARTMENTAL_FIRM_ID ) {
	facility_office.debt_spinner_var1=(float)facility_office.percent_project_funded_with_debt;
	facility_office.debt_spinner_var2=(float)facility_office.cur_debt_limit;
    }
}

//--------- End of function Sys::set_view_mode ---------//

//-------- Begin of function Sys::set_staying_view_mode --------//
//!
//! The view mode which the game must stay at until the player
//! actually finish seeing the report, e.g. the year-end report.
//!
void Sys::set_staying_view_mode(int viewMode) {
    if( viewMode==0 ) {
	if(staying_view_mode>0) {
	    staying_view_mode = 0;

	    set_speed(save_speed);
	    set_view_mode(save_view_mode);
	}
    }
    else {
	save_speed = config.frame_speed;

	save_view_mode = view_mode;

	set_speed(0);                                 // set the gaming speed to frozen, the speed is frozen until the player reset the staying view_mode
	set_view_mode(viewMode);

	staying_view_mode = viewMode;
    }
}

//--------- End of function Sys::set_staying_view_mode ---------//

//-------- Begin of function Sys::detect_report_button ---------//
//!
//! return value:	1 - main screen button detected
//!						0 - not detected
//!
int Sys::detect_report_button() {

    if (mouse.single_click(MENU_BUTTON_X1, MENU_BUTTON_Y1, MENU_BUTTON_X2, MENU_BUTTON_Y2)) {
	audio.play_wav("STONES",audio.int_to_DsVolume(config.sound_effect_volume));

	menu_pull_down = 1 - menu_pull_down;          // toggle menu pull down

	if(menu_pull_down) {
	    world.draw_all();
	    info.disp_column_bitmap(&vga_back);
	    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2,1);
	    set_staying_view_mode(MODE_GAME_OPTIONS);
	    // BUGHERE
	    // We also need to disable all buttons, besides 'menu button'
	}
	else
	    set_staying_view_mode(0);

	return 1;
    }

    if(view_mode==MODE_GAME_OPTIONS)
	return 0;

    if(view_mode!=MODE_NORMAL)
	if(mouse.single_click(MAP_X1, MAP_Y1, MAP_X2, MAP_Y2)) {
	    set_view_mode(MODE_NORMAL);
	    return 1;
	}

    help.set_help(REPORT_BUTTON_CAMPUS_X1, REPORT_BUTTON_CAMPUS_Y1, REPORT_BUTTON_CAMPUS_X2, REPORT_BUTTON_CAMPUS_Y2,"U01.");
    help.set_help(REPORT_BUTTON_FACULTY_X1, REPORT_BUTTON_FACULTY_Y1, REPORT_BUTTON_FACULTY_X2, REPORT_BUTTON_FACULTY_Y2,"U02.");
    help.set_help(REPORT_BUTTON_STUDENTS_X1, REPORT_BUTTON_STUDENTS_Y1, REPORT_BUTTON_STUDENTS_X2, REPORT_BUTTON_STUDENTS_Y2,"U03.");
    help.set_help(REPORT_BUTTON_COURSES_X1, REPORT_BUTTON_COURSES_Y1, REPORT_BUTTON_COURSES_X2, REPORT_BUTTON_COURSES_Y2,"U04.");
    help.set_help(REPORT_BUTTON_PERFORMANCE_X1, REPORT_BUTTON_PERFORMANCE_Y1, REPORT_BUTTON_PERFORMANCE_X2, REPORT_BUTTON_PERFORMANCE_Y2,"U05.");
    help.set_help(REPORT_BUTTON_FINANCIAL_X1, REPORT_BUTTON_FINANCIAL_Y1, REPORT_BUTTON_FINANCIAL_X2, REPORT_BUTTON_FINANCIAL_Y2,"U06.");
    help.set_help(REPORT_BUTTON_SCORE_X1, REPORT_BUTTON_SCORE_Y1, REPORT_BUTTON_SCORE_X2, REPORT_BUTTON_SCORE_Y2,"U07.");

    help.set_help(SURPLUS_INFO_X1,SURPLUS_INFO_Y1,SURPLUS_INFO_X2,SURPLUS_INFO_Y2,"U08.");
    help.set_help(514,0,SCORE_INFO_X2,SCORE_INFO_Y2,"U09.");
    help.set_help(578,0,U_SCORE_INFO_X2,U_SCORE_INFO_Y2,"U10.");
    help.set_help(DATE_INFO_X1,MONTH_Y1,YEAR_INFO_X2,DATE_INFO_Y2,"U11.");

    //	help.set_help(YEAR_INFO_X1,YEAR_INFO_Y1,YEAR_INFO_X2,YEAR_INFO_Y2,"");
    //	help.set_help(TOP_INFO_X1,TOP_INFO_Y1,TOP_INFO_X2,TOP_INFO_Y2,"");
    //	help.set_help(BOTTOM_INFO_X1,BOTTOM_INFO_Y1,BOTTOM_INFO_X2,BOTTOM_INFO_Y2,"");

    help.set_help(MENU_BUTTON_X1, MENU_BUTTON_Y1, MENU_BUTTON_X2, MENU_BUTTON_Y2,"U12.");
    help.set_help(MORE_REPORT_X1,MORE_REPORT_Y1,MORE_REPORT_X2,MORE_REPORT_Y2,"U14.");
    help.set_help(LIST_BUTTON_X1,LIST_BUTTON_Y1,LIST_BUTTON_X2,LIST_BUTTON_Y2,"U15.");

    help.set_help(557, 31, 581, 44, "U16.");        // prev department
    help.set_help(590, 31, 614, 44, "U17.");        // next department

    // M
    help.set_help(223, 516+m_o_log_bitmap_y[0], 253, 516+m_o_log_bitmap_y[1],"U18.");
    // O
    help.set_help(223, 516+m_o_log_bitmap_y[1], 253, 516+m_o_log_bitmap_y[2],"U19.");
    // LOG
    help.set_help(223, 516+m_o_log_bitmap_y[2], 253, 516+m_o_log_bitmap_y[3],"U20.");

    help.set_help(260, 515, 620, 590, "U21.");      // U21 reserved for Message Board
    help.set_help(178, 515, 208, 541, "U22.");      // Zoom
    // speed indicator
    help.set_help( 112, 538, 110+67, 538+31, "U23." );
    help.set_help(180, 564, 210, 594, "U24.");      // Printer
    help.set_help(8, 562, 74, 599, "U25.");         // Mail
    help.set_help(MIN_MAX_X1,MIN_MAX_Y1,MIN_MAX_X2,MIN_MAX_Y2,"U26.");

    // U27 reserved for Small Map

    if (mouse.single_click(REPORT_BUTTON_CAMPUS_X1, REPORT_BUTTON_CAMPUS_Y1, REPORT_BUTTON_CAMPUS_X2, REPORT_BUTTON_CAMPUS_Y2)) {
	audio.play_wav("CAMPUS",audio.int_to_DsVolume(config.sound_effect_volume));
	set_view_mode(MODE_NORMAL);
	return 1;
    }

    if (mouse.single_click(REPORT_BUTTON_FACULTY_X1, REPORT_BUTTON_FACULTY_Y1, REPORT_BUTTON_FACULTY_X2, REPORT_BUTTON_FACULTY_Y2)) {
	set_view_mode(MODE_FACULTY_SUMMARY);
	return 1;
    }

    if (mouse.single_click(REPORT_BUTTON_STUDENTS_X1, REPORT_BUTTON_STUDENTS_Y1, REPORT_BUTTON_STUDENTS_X2, REPORT_BUTTON_STUDENTS_Y2)) {
	set_view_mode(MODE_STUDENT_REPORT);
	return 1;
    }

    if (mouse.single_click(REPORT_BUTTON_COURSES_X1, REPORT_BUTTON_COURSES_Y1, REPORT_BUTTON_COURSES_X2, REPORT_BUTTON_COURSES_Y2)) {
	set_view_mode(MODE_COURSE_REPORT);
	return 1;
    }

    if (mouse.single_click(REPORT_BUTTON_PERFORMANCE_X1, REPORT_BUTTON_PERFORMANCE_Y1, REPORT_BUTTON_PERFORMANCE_X2, REPORT_BUTTON_PERFORMANCE_Y2)) {
	set_view_mode(MODE_PERFORMANCE_REPORT);
	return 1;
    }

    if (mouse.single_click(REPORT_BUTTON_FINANCIAL_X1, REPORT_BUTTON_FINANCIAL_Y1, REPORT_BUTTON_FINANCIAL_X2, REPORT_BUTTON_FINANCIAL_Y2)) {
	set_view_mode(MODE_FINANCIAL_REPORT);
	return 1;
    }

    if (mouse.single_click(REPORT_BUTTON_SCORE_X1, REPORT_BUTTON_SCORE_Y1, REPORT_BUTTON_SCORE_X2, REPORT_BUTTON_SCORE_Y2)) {
	set_view_mode(MODE_SCORE_REPORT);
	return 1;
    }

    if (mouse.single_click(MORE_REPORT_BUTTON_X1, MORE_REPORT_BUTTON_Y1, MORE_REPORT_BUTTON_X2, MORE_REPORT_BUTTON_Y2)) {
	audio.play_wav("STONES",audio.int_to_DsVolume(config.sound_effect_volume));

	if(view_mode!=MODE_MORE_REPORT) {
	    if(view_mode!=MODE_DEPT_LIST && view_mode!=MODE_MORE_REPORT
	       && view_mode!=MODE_HELP_TOPIC)
		report_before_morerpt_or_deptlist=view_mode;
	    else
		report_before_morerpt_or_deptlist=MODE_NORMAL;

	    set_view_mode(MODE_MORE_REPORT);
	}
	else
	    set_view_mode(report_before_morerpt_or_deptlist);
	return 1;
    }

    help.set_help(HELP_QUEST_X1,HELP_QUEST_Y1,HELP_QUEST_X2,HELP_QUEST_Y2,"U13.");
    if (mouse.single_click(HELP_QUEST_X1, HELP_QUEST_Y1, HELP_QUEST_X2, HELP_QUEST_Y2)) {
	audio.play_wav("STONES",audio.int_to_DsVolume(config.sound_effect_volume));

	if(view_mode!=MODE_HELP_TOPIC) {
	    if(view_mode!=MODE_DEPT_LIST && view_mode!=MODE_MORE_REPORT
	       && view_mode!=MODE_HELP_TOPIC)
		report_before_morerpt_or_deptlist=view_mode;
	    else
		report_before_morerpt_or_deptlist=MODE_NORMAL;

	    set_view_mode(MODE_HELP_TOPIC);
	}
	else
	    set_view_mode(report_before_morerpt_or_deptlist);
	return 1;
    }

    return 0;
}

//--------- End of function Sys::detect_report_button ----------//

//-------- Begin of function Sys::detect_dept_button ---------//
//!
//! Detect buttons for going to the previous/next department.
//!
int Sys::detect_dept_button() {
    // detect list button
    //### begin zb 02/28/99
    if(is_institution_report==0&&mouse.single_click(LIST_BUTTON_X1, LIST_BUTTON_Y1, LIST_BUTTON_X2, LIST_BUTTON_Y2, LEFT_BUTTON)
       //### end zb 02/28/99
       //		||	mouse.single_click(TOP_INFO_X1,TOP_INFO_Y1,TOP_INFO_X2,TOP_INFO_Y2, LEFT_BUTTON)
	) {
	image_interface.put_back( LIST_BUTTON_X1, LIST_BUTTON_Y1,"LIST_DN" );
	vga.blt_buf(LIST_BUTTON_X1, LIST_BUTTON_Y1, LIST_BUTTON_X2, LIST_BUTTON_Y2, 1);

	if(view_mode!=MODE_DEPT_LIST) {
	    if(view_mode!=MODE_DEPT_LIST && view_mode!=MODE_MORE_REPORT
	       && view_mode!=MODE_HELP_TOPIC)
		report_before_morerpt_or_deptlist=view_mode;
	    else
		report_before_morerpt_or_deptlist=MODE_NORMAL;
	    set_view_mode(MODE_DEPT_LIST);
	    faculty_res.report_faculty_recno=1;
	}
	else
	    set_view_mode(report_before_morerpt_or_deptlist);
    }

    char* bmpName;
    int skipDir;

    if(is_institution_report==0) {
	if( mouse.single_click(557, 31, 581, 44, LEFT_BUTTON) ) {
	    bmpName = "UPARROW";
	    skipDir = -1;
	    redraw_all_flag=1;
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    faculty_res.report_faculty_recno=1;
	}
	else if( mouse.single_click(590, 31, 614, 44, LEFT_BUTTON) ) {
	    bmpName = "DWARROW";
	    skipDir = 1;
	    redraw_all_flag=1;
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    faculty_res.report_faculty_recno=1;
	}
	else
	    return 0;

	vga_front.save_area_common_buf( 555, 31, 616, 49 );
	// ## chw1014

	mouse.hide_area( 555, 31, 616, 49 );          // ## chw1014

	image_interface.put_back( 555, 31, bmpName );

	mouse.show_area();                            // ## chw1014
	vga.blt_buf( 555, 31, 616, 49 , 1);           // ## chw1014

	//---------- move forward or backward ----------//

	int loopCount=0;

	while(1) {
	    ++loopCount;
	    err_when( loopCount > 1000 );

	    department_array.selected_recno += skipDir;

	    if( department_array.selected_recno < 1 )
		department_array.selected_recno = department_array.size();

	    else if( department_array.selected_recno > department_array.size() )
		department_array.selected_recno = 1;

	    // if this record exists, we're done with it
	    if( !department_array.is_deleted(department_array.selected_recno) )
		break;
	}

	//------- wait until the user releases the button -------//

	while( mouse.left_press ) {
	    yield();
	    mouse.get_event();
	}

	mouse.hide_area( 555, 31, 616, 49 );          // ## chw1014
	vga_back.rest_area_common_buf();              // ## chw1014
	mouse.show_area();                            // ## chw1014
	vga.blt_buf( 555, 31, 616, 49 , 1);           // ## chw1014
    }
    else {
	if( mouse.single_click(557, 31, 581, 44, LEFT_BUTTON) ) {
	    audio.play_wav("BEEPS-N1",audio.int_to_DsVolume(config.sound_effect_volume));
	}
	else if( mouse.single_click(590, 31, 614, 44, LEFT_BUTTON) ) {
	    audio.play_wav("BEEPS-N1",audio.int_to_DsVolume(config.sound_effect_volume));
	}
    }

    return 0;
}

//--------- End of function Sys::detect_dept_button ----------//

//-------- Begin of function Sys::detect_mail_button ---------//
//!
int Sys::detect_mail_button() {
    //	if(!(print_menu.print_view_mode==MODE_SCENARIO_LETTER))
    if(!(player_school.scenario_id>0 && (view_mode==MODE_SCENARIO_LETTER)))
	if(view_mode!=MODE_PRINTING_MENU)
	    if( mouse.single_click( 8, 562, 74, 599 )
		||
		(m.upper(mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_UNIQUE_KEY))=='W')) {
		int mail_state=0;
		if(player_school.scenario_id<=0) {
		    if( (view_mode!=MODE_WELCOME_LETTER)
			&&  (view_mode!=MODE_EVALUATION_LETTER)
			&& (view_mode!=MODE_STATE_REPORT)
			) {
			if(info.year_passed<=1)
			    set_staying_view_mode(MODE_WELCOME_LETTER);
			else
			    set_staying_view_mode(MODE_EVALUATION_LETTER);
			mail_state=1;
		    }
		    else {
			if(save_view_mode!=MODE_YEAR_END_REPORT) {
			    set_staying_view_mode(0);
			    //						set_view_mode(MODE_NORMAL);
			    set_view_mode(sys.save_view_mode);
			}
		    }
		}
		else {
		    if( (view_mode!=MODE_WELCOME_LETTER)
			&&  (view_mode!=MODE_EVALUATION_LETTER)
			&&  (view_mode!=MODE_SCEN_WELCOME_LETTER)
			&&  (view_mode!=MODE_SCENARIO_LETTER)
			&& (view_mode!=MODE_STATE_REPORT)
			) {
			set_staying_view_mode(MODE_SCENARIO_LETTER);
		    }
		    else {
			if(save_view_mode!=MODE_YEAR_END_REPORT)
			    set_staying_view_mode(0);
		    }
		}
		mouse.hide_area( 8, 562, 74, 599 );           // ## chw1014
		if((view_mode!=MODE_PAUSE)
		   &&(view_mode!=MODE_GAME_OPTIONS)
		   &&(view_mode!=MODE_YEAR_END_REPORT)) {
		    image_interface.put_back( 8, 562, mail_state==1 ? "MAIL-DN" : "MAIL-UP" );
		}
		else {
		    image_interface.put_back( 8, 562, "MAIL-UP" );
		    user_interface.darken(8, 562, 74, 599);
		}
		mouse.show_area();
		vga.blt_buf(8, 562, 74, 599 ,1);
		return 1;
	    }

    //------ detect Print button -----------//
    if((view_mode!=MODE_NORMAL)
       && (view_mode!=MODE_HELP_DESC)
       && (view_mode!=MODE_HELP_TOPIC)
       && (view_mode!=MODE_PRINTING_MENU)
       && (view_mode!=MODE_MORE_REPORT)
       && (view_mode!=MODE_DEPT_LIST)
       && (view_mode!=MODE_PAUSE)
       && (view_mode!=MODE_GAME_OPTIONS)
       && (view_mode!=MODE_YEAR_END_REPORT)) {
	image_interface.put_back( 180, 564, "PRINT-UP" );
    }
    else {
	image_interface.put_back( 180, 564, "PRINT-DN" );
	user_interface.darken(180, 564, 180+26, 564+28);
    }

    if(view_mode!=MODE_NORMAL)
	if(view_mode!=MODE_MORE_REPORT)
	    if(view_mode!=MODE_DEPT_LIST)
		if(view_mode!=MODE_AWARD_REPORT)
		    if(view_mode!=MODE_GAME_OPTIONS)
			if( mouse.press_area( 180, 564, 210, 594 )||
			    (m.upper(mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_UNIQUE_KEY))==KEY_F8)) {
			    if(view_mode!=MODE_PRINTING_MENU) {
				image_interface.put_front( 180, 564, "PRINT-DN" );

				while( mouse.left_press ) {
				    yield();
				    mouse.get_event();
				}

				image_interface.put_back( 180, 564, "PRINT-UP" );

				print_menu.print_view_mode=view_mode;
				/*
				  if(	(view_mode==MODE_WELCOME_LETTER)
				  ||	(view_mode==MODE_EVALUATION_LETTER)
				  ||	(view_mode==MODE_SCEN_WELCOME_LETTER)
				  ||	(view_mode==MODE_SCENARIO_LETTER))
				  {

				  set_staying_view_mode(0);
				  sys.view_mode=MODE_NORMAL;
				  }
				*/
				set_staying_view_mode(MODE_PRINTING_MENU);

				return 1;
			    }
			    else {
				set_staying_view_mode(0);
				set_view_mode(print_menu.print_view_mode);
				/*
				  if(	(print_menu.print_view_mode==MODE_WELCOME_LETTER)
				  ||	(print_menu.print_view_mode==MODE_EVALUATION_LETTER)
				  ||	(print_menu.print_view_mode==MODE_SCEN_WELCOME_LETTER)
				  ||	(print_menu.print_view_mode==MODE_SCENARIO_LETTER))
				  {
				  sys.save_view_mode=MODE_NORMAL;
				  }
				*/
			    }
			}

    vga.blt_buf(180, 564, 180+26, 564+28);

    return 0;
}

//--------- End of function Sys::detect_mail_button ----------//

//-------- Begin of function Sys::detect_function_button ---------//
//!
int Sys::detect_function_button() {
    int result=0;
    //-------- detect Zoom button ---------//
    static int zoom_display_updated=1;

    if( mouse.single_click( 178, 515, 208, 541 ) || zoom_display_updated==1) {
	audio.play_wav("ZOOM",audio.int_to_DsVolume(config.sound_effect_volume));
	if(zoom_display_updated==0) {
	    world.set_zoom_level( world.zoom_level==1 ? 2 : 1 );
	    result=1;
	}
	mouse.hide_area( 178, 515, 208, 541 );        // ## chw1014
	image_interface.put_back( 178, 515, world.zoom_level==1 ? "MAG-UP" : "MAG-DN" );
	mouse.show_area();
	vga.blt_buf( 178, 515, 208, 541 ,1);
	zoom_display_updated=0;
    }

    //-------- detect Max-Min button ---------//
    if(view_mode==MODE_NORMAL) {
	if( mouse.single_click( MIN_MAX_X1, MIN_MAX_Y1, MIN_MAX_X2, MIN_MAX_Y2 ) ) {
	    info.disp_6_graphs_flag = !info.disp_6_graphs_flag;
	    redraw_all_flag=1;
	}
    }

    //-------- detect M-O log button ---------//
    // 32 x 81
    // Change the state of M-O-log buttons
    if(mouse.in_area(223, 516+m_o_log_bitmap_y[0], 253, 516+m_o_log_bitmap_y[3])) {
	for(int i=0;i<3;i++) {
	    if( mouse.in_area(223, 516+m_o_log_bitmap_y[i], 253, 516+m_o_log_bitmap_y[i+1])) {
		for(int j=0;j<3;j++) {
		    if(m_o_log_state[j]==1)
			m_o_log_state[j]=0;

		    if(m_o_log_state[i]==0) {               // highlight the button if it is not selected
			m_o_log_state[i]=1;
			m_o_log_highlighted=i;
		    }

		    if(m_o_log_state[i]==2)
			m_o_log_highlighted=-1;
		}

		if( mouse.single_click(223, 516+m_o_log_bitmap_y[i], 253, 516+m_o_log_bitmap_y[i+1])) {
		    audio.play_wav("NEWS",audio.int_to_DsVolume(config.sound_effect_volume));

		    if( i != 2 ) {                          // if click on Log, don't highlight it, as it just brings up the log report
			for(int j=0;j<3;j++)
			    m_o_log_state[j]=0;
			m_o_log_highlighted=-1;
			m_o_log_state[i]=2;
			m_o_log_selected=i;
		    }

		    if(i==2) {
			if(view_mode!=MODE_BLACKBOARD_LOG)
			    back_view_mode=view_mode;
			set_view_mode(MODE_BLACKBOARD_LOG);
		    }
		    else {
			if(view_mode==MODE_BLACKBOARD_LOG)
			    set_view_mode(back_view_mode);
		    }
		    break;
		    result=1;
		}
	    }
	}
    }
    else {
	for(int j=0;j<3;j++)
	    //			if(m_o_log_state[j]!=2)
	    m_o_log_state[j]=0;
	m_o_log_state[m_o_log_selected]=2;
	m_o_log_highlighted=-1;
    }

    // Check if the M_O_button need to refresh?
    if(m_o_log_old_selected!=m_o_log_selected||
       m_o_log_old_highlighted!=m_o_log_highlighted)
	m_o_log_display_updated=1;

    if(m_o_log_display_updated) {
	//		mouse.hide_area(223, 516+m_o_log_bitmap_y[0], 253, 516+m_o_log_bitmap_y[3]);
	for(int i=0;i<3;i++) {
	    if(m_o_log_state[i]==0)
		vga_back.put_bitmap_area(223, 516,
					 image_interface.get_ptr("MO_LOG1"),
					 0,  m_o_log_bitmap_y[i],
					 31,m_o_log_bitmap_y[i+1]);

	    if(m_o_log_state[i]==1) {
		vga_back.put_bitmap_area(223, 516,        //highlighted
					 image_interface.get_ptr("MO_LOG2"),
					 0,  m_o_log_bitmap_y[i],
					 31,m_o_log_bitmap_y[i+1]);
	    }

	    if(m_o_log_state[i]==2) {                   //selected
		vga_back.put_bitmap_area(223, 516,
					 image_interface.get_ptr("MO_LOG3"),
					 0,  m_o_log_bitmap_y[i],
					 31,m_o_log_bitmap_y[i+1]);
	    }
	}
	//		mouse.show_area();
	vga.blt_buf(223, 516+m_o_log_bitmap_y[0], 253, 516+m_o_log_bitmap_y[3],1);
	m_o_log_old_selected=m_o_log_selected;
	m_o_log_old_highlighted=m_o_log_highlighted;
	m_o_log_display_updated=0;
    }

    // ## begin chwg 050299
    if(staying_view_mode==0) {
	if (m_o_log_selected==0)
	    blackboard.black_message_report(redraw_all_flag);

	if (m_o_log_selected==1)
	    blackboard.black_objective_report(redraw_all_flag);

	if (m_o_log_selected==2)
	    if(blackboard.log_mode==NEWS_MODE)
		blackboard.black_message_report(redraw_all_flag);
	    else
		blackboard.black_objective_report(redraw_all_flag);
    }
    // ## end chwg 050299

    // detect speed indicator
    static int showX=0;
    if( mouse.press_area( 112, 538, 110+67, 538+31 ) ) {
	// Indictor range for detection 2 to 65
	// convert 0 to 67 -> 0 to 10
	float speedLevel =float(mouse.cur_x-112+5)/6.7f;

	if( speedLevel >= 9.5 )
	    set_speed(99);
	else
	    set_speed( speedLevel * 10 );

	// ## begin zb 02/21/99
	sys.save_speed=config.frame_speed;
	// ## end zb 02/21/99

	result=1;
    }

    //------ detect Print button -----------//

    if(view_mode!=MODE_NORMAL)
	if(mouse.press_area( 180, 564, 210, 594 )
	   ||(m.upper(mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_UNIQUE_KEY))==KEY_F8)) {
	    if(view_mode!=MODE_PRINTING_MENU) {
		image_interface.put_front( 180, 564, "PRINT-DN" );

		while( mouse.left_press ) {
		    yield();
		    mouse.get_event();
		}

		image_interface.put_back( 180, 564, "PRINT-UP" );
		vga.blt_buf(180, 564, 180+26, 564+28);

		print_menu.print_view_mode=view_mode;
		/*
		  if(	(view_mode==MODE_WELCOME_LETTER)
		  ||	(view_mode==MODE_EVALUATION_LETTER)
		  ||	(view_mode==MODE_SCEN_WELCOME_LETTER)
		  ||	(view_mode==MODE_SCENARIO_LETTER))
		  {
		  set_staying_view_mode(0);
		  sys.view_mode=MODE_NORMAL;
		  }
		*/
		set_staying_view_mode(MODE_PRINTING_MENU);

		return 1;
	    }
	    else {
		set_staying_view_mode(0);
		set_view_mode(print_menu.print_view_mode);
		/*
		  if(	(print_menu.print_view_mode==MODE_WELCOME_LETTER)
		  ||	(print_menu.print_view_mode==MODE_EVALUATION_LETTER)
		  ||	(print_menu.print_view_mode==MODE_SCEN_WELCOME_LETTER)
		  ||	(print_menu.print_view_mode==MODE_SCENARIO_LETTER))
		  {
		  sys.save_view_mode=MODE_NORMAL;
		  }
		*/
	    }
	}

    /*
      //------ detect Print button -----------//

      if( mouse.press_area( 180, 564, 210, 594 ) )
      {
      if(view_mode!=MODE_PRINTING_MENU)
      {
      image_interface.put_front( 180, 564, "PRINT-DN" );

      while( mouse.left_press )
      {
      yield();
      mouse.get_event();
      }

      image_interface.put_back( 180, 564, "PRINT-UP" );
      vga.blt_buf(180, 564, 210, 594);

      print_menu.print_view_mode=view_mode;
      set_staying_view_mode(MODE_PRINTING_MENU);

      return 1;
      }
      else
      {
      set_staying_view_mode(0);
      set_view_mode(print_menu.print_view_mode);
      }
      }
    */
    return 0;
}

//--------- End of function Sys::detect_function_button ----------//

//-------- Begin of function Sys::detect_main_screen_help --------//
//!
void Sys::detect_main_screen_help() {
    //------ report buttons --------//

    help.set_help2( 6  , 2, 50, 30, "H9.18.1." );
    help.set_help2( 56 , 2, 46, 30, "H9.19.1." );
    help.set_help2( 102, 2, 54, 30, "H9.20.1." );
    help.set_help2( 156, 2, 51, 30, "H9.21.1." );
    help.set_help2( 207, 2, 70, 30, "H9.22.1." );
    help.set_help2( 277, 2, 47, 30, "H9.23.1." );
    help.set_help2( 324, 2, 39, 30, "H9.24.1." );

    help.set_help2( 369,1,145,29, "H9.12.1." );     // surplus
    help.set_help2( 515,2, 48,28,  "H9.13.1." );    // trust evaluation score
    help.set_help2( 579,1, 66,30,  "H9.14.1." );    // game score
    help.set_help2( 661,2, 88,42,  "H9.15.1." );    // calender
    help.set_help2( 758,32, 34,14, "H9.16.1." );    // ?
    help.set_help2( 750,1, 49,30,  "H9.17.1." );    // menu
    help.set_help2( 728,60, 21,16, "H9.25.1." );    // minimize

    help.set_help2( 555,31, 65,15, "H9.10.1." );    // up/down arrows
    help.set_help2( 625,30, 25,16, "H9.11.1." );    // double down
    help.set_help2( 2,32, 153,14,  "H9.9.1."  );    // more report
    help.set_help2( 634,515, 161,81, "H9.8.1.");    // mini map
    help.set_help2( 224,518, 30,28, "H9.5.1." );    // 'M' button
    help.set_help2( 225,547, 28,25, "H9.6.1." );    // 'O' button
    help.set_help2( 225,575, 28,22, "H9.7.1." );    // log

    if( player_school.scenario_id > 0 )
	help.set_help2( 5,563, 67,31,  "H9.4.3." );   // scenario report
    else {
	// ### patch begin Gilbert 07/09/2001 #####//
	//int fiscalYear = info.game_year-1;
	//if( info.game_month >= 9 )
	//	fiscalYear++;
	int fiscalYear = info.financial_year();
	// ### patch end Gilbert 07/09/2001 #####//

	if( fiscalYear==1  )                          // welcome report
	    help.set_help2( 5,563, 67,31,  "H9.4.1." );
	else                                          // evaluation report
	    help.set_help2( 5,563, 67,31,  "H9.4.2." );
    }

    help.set_help2( 108,539, 70,32, "H9.1.1." );    // speed
    help.set_help2( 177,513, 34,33, "H9.3.1." );    // zoom

    if((view_mode!=MODE_NORMAL)
       &&(view_mode!=MODE_HELP_DESC)
       &&(view_mode!=MODE_HELP_TOPIC)
       &&(view_mode!=MODE_MORE_REPORT)
       &&(view_mode!=MODE_PAUSE)
       &&(view_mode!=MODE_DEPT_LIST)
       &&(view_mode!=MODE_GAME_OPTIONS)
       &&(view_mode!=MODE_AWARD_REPORT)
       &&(view_mode!=MODE_YEAR_END_REPORT)) {
	help.set_help2( 177,564, 33,31, "H9.2.1." );  // print enabled
    }
    else {
	help.set_help2( 177,564, 33,31, "H9.2.2." );  // print disabled
    }
}

//--------- End of function Sys::detect_main_screen_help ---------//
