//Filename    : OFAC_REP.CPP
//Description : Faculty Report

#include <KEY.H>
#include <OVGA.H>
#include <OFONT.H>
#include <OWORLD.H>
#include <OSYS.H>
#include <OINFO.H>
#include <OMOUSE.H>
#include <OIFACE.H>
#include <OBUTTON.H>
#include <ODEPT.H>
#include <OVBROW98.H>
#include <OSPINNER.H>
#include <OGAMESTR.H>
#include <OFACULTY.H>
#include <OFPHOTO.H>
#include <OFACURES.H>
#include <OBOX.H>
#include <OTEXT.H>
#include <OAUDIO.H>
#include <OCONFIG.H>
#include <STDIO.H>
#include <ALL.H>
#include <OPSCHOOL.H>

//---------- Define constant ------------//

enum {
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+15,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-40,
};

enum { FACULTY_REC_HEIGHT = 84, RESEARCH_REC_HEIGHT = 16 };

//--------- Define report modes ----------//

enum {                                            // the bg image id
    BG_PIC_ID = 4
};

enum { REPORT_TYPE_COUNT = 2 };
enum { REPORT_TYPE_LIST, REPORT_TYPE_DETAIL, REPORT_RETIRE };

enum { REPORT_MODE1_COUNT = 3 };
enum {                                            //### fred 1023
    REPORT_MODE1_BASIC, REPORT_MODE1_ACTIVITY, REPORT_MODE1_ASSEST
};

enum { REPORT_MODE2_COUNT = 2 };
enum { REPORT_MODE2_STD, REPORT_MODE2_RESEARCH };

enum { BUTTON_Y1=REPORT_Y2+10 };
enum { BUTTON_WIDTH=90, BUTTON_HEIGHT=20 };

//-------- define static vars -----------//

static int        last_faculty_count=0;
static int        last_research_count=0;
static VBrowse98    browse_faculty, browse_research;
static Department*  cur_dept_ptr;                 // the department whose faculty we are looking at

static ButtonGroup  button_type(REPORT_TYPE_COUNT);
static ButtonGroup  button_mode1_array(REPORT_MODE1_COUNT);
static ButtonGroup  button_mode2_array(REPORT_MODE2_COUNT);
static Button     button_prev, button_next, mode_fac_sum_button;
static Button     button_retire, button_retire_cancel, button_retire_offer;
static Button     button_dismiss;
static Spinner      retire_spinner;

static char* reportMode1StrArray[] = {            //### fred 1023
    "Basic", "Activity", "Assets"
};
static char* reportMode2StrArray[] = { "Standard", "Extended" };
static Button     button_back;
float spinner1=25.0;
static Text  retire_result_txt;
static int offerDollar;
//------ declare static functions -------//

static void   disp_faculty_rec(int,int,int,int);
static void   disp_display_faculty_rec(int,int,int,int);
static void   disp_research_rec(int,int,int,int);

//----- Begin of function FacultyRes::report ------//
//!
//! Display Stock Ownership Report
//!
//! <int> refreshFlag = INFO_REPAINT or INFO_UPDATE
//!
void FacultyRes::report(int refreshFlag) {
    //---- determine the department for display the faculty records ----//
    int deptRecno=1;

    if( department_array.selected_recno )
	deptRecno = department_array.selected_recno;
    cur_dept_ptr = department_array[deptRecno];

    // fix in version 2, infinite loop if report_faculty_recno is 0
    if( report_faculty_recno <= 0)
	report_faculty_recno = 1;

    // begin chwg080999 avoiding (facultyPtr->faculty_recno==0)
    Faculty* facultyPtr = cur_dept_ptr->faculty_array.get_unpacked_faculty(report_faculty_recno);
    while((facultyPtr->faculty_recno==0)&&(report_faculty_recno!=1))
	facultyPtr = cur_dept_ptr->faculty_array.get_unpacked_faculty(--report_faculty_recno);
    // end chwg080999

    //------ paint the background -------//
    vga.use_back();

    if( refreshFlag == INFO_REPAINT ) {
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	if( report_type==REPORT_TYPE_LIST ) {
	    user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	    user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	    font_charts.put(REPORT_X1+300,REPORT_Y1-8,"Faculty Directory");
	}

	report_paint_button(refreshFlag);
    }
    else {
	//		report_paint_button(refreshFlag);
    }

    //----------- If it's in the list mode -------------//
    if( report_type==REPORT_TYPE_LIST ) {
	if( refreshFlag == INFO_REPAINT ) {
	    browse_faculty.init( REPORT_X1, REPORT_Y1+10, REPORT_X2, REPORT_Y2+10,
				 0, FACULTY_REC_HEIGHT,
				 cur_dept_ptr->faculty_array.faculty_count,
				 disp_faculty_rec, NULL ,1 );
	    browse_faculty.open(report_faculty_recno,cur_dept_ptr->faculty_array.faculty_count);
	}
	else {                                        // refreshFlag == INFO_UPDATE
	    //			browse_faculty.refresh(-1, cur_dept_ptr->faculty_array.faculty_count);
	}
	report_faculty_recno = browse_faculty.recno();
    }
    else if( report_type==REPORT_TYPE_DETAIL) {
	//------- if it's in detailed mode --------//
	if( cur_dept_ptr->faculty_array.faculty_count > 0 ) {
	    switch( faculty_res.report_det_mode ) {
	    case REPORT_MODE2_STD:
		facultyPtr->disp_detail_std(refreshFlag);
		break;
	    }
	}
    }
    last_faculty_count = cur_dept_ptr->faculty_array.faculty_count;

    if( report_type==REPORT_RETIRE ) {
	if( refreshFlag == INFO_REPAINT ) {
	    switch( faculty_res.report_det_mode ) {
	    case REPORT_MODE2_STD:
		facultyPtr->disp_detail_std(refreshFlag);
		break;
	    }

	    //			user_interface.darken(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
	    user_interface.darken(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
	    info.disp_column_bitmap(&vga_back);
	    user_interface.bar(200,200,600,400,V_WHITE);
	    user_interface.brighten(200,200,600,400);
	    user_interface.rect(200,200,600,400);

	    button_retire_offer.create_text(250,370,400,390,"Make offer",1);
	    button_retire_offer.paint();
	    button_retire_cancel.create_text(450,370,550,390,"Cancel",1);
	    button_retire_cancel.paint();

	    font_chartsm.put(260,220,"How much would you offer to ");
	    font_chart_blue_sm.put(430,220,facultyPtr->name());
	    font_chartsm.put(260,240,"for retirement?");

	    //			retire_spinner.init(260, 275, 360, 305, &spinner1, 25.0, 300.0, 5.0,0,NULL,23);
	    if ( facultyPtr->rank_level == LONG_TERM_ADJUNCT ) {
		int yearCount  = (info.game_date-facultyPtr->start_teaching_date) / 365;
		int monthCount = ((info.game_date-facultyPtr->start_teaching_date) % 365) / 30;
		int remainYear;

		if ( monthCount )
		    remainYear = 5-yearCount%5-1;
		else
		    remainYear = 5-yearCount%5;

		int x = remainYear;

		if ( spinner1 != (float)x*10 )
		    spinner1 = (float)x*10;
	    }
	    else {
		spinner1 = 100.0;
	    }

	    //## chea 230999
	    retire_spinner.init(260, 275, 360, 305, &spinner1, 10.0, 300.0, 5.0,0,NULL,25);
	    retire_spinner.paint();
	    font_chartsm.put(380,285,"of annual salary");
	    font_chart_blue_sm.put(480,285,m.format(facultyPtr->salary,2));

	    font_chartsm.put(260,315,"=");
	    offerDollar=(int)(facultyPtr->salary*(*(retire_spinner.var_ptr))/100);
	    //			font_chart_blue_sm.put(270,315,m.format(offerDollar,1));  //## chea 230999  don't why this is here
	    //			font_chart_blue_sm.put(270,315,m.format(offerDollar,2));  //## chea 230999  don't why this is here
	    //			retire_result_txt.put(280,315,&offerDollar,1);
	    //## chea 230999
	    retire_result_txt.put(280,315,&offerDollar,2);
	}
	else {
	    offerDollar=(int)(facultyPtr->salary*(*(retire_spinner.var_ptr))/100);
	    retire_spinner.refresh();
	    retire_result_txt.refresh();
	}
    }
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//------- End of function FacultyRes::report ------//

// --- Begin of function FacultyRes::report_faculty_hired --- //
//
void  FacultyRes::report_faculty_hired(int refreshFlag) {
    //---- determine the department for display the faculty records ----//
    int deptRecno=1;

    if( department_array.selected_recno )
	deptRecno = department_array.selected_recno;
    cur_dept_ptr = department_array[deptRecno];

    //------ paint the background -------//
    vga.use_back();

    if( refreshFlag == INFO_REPAINT ) {
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	if( report_type==REPORT_TYPE_LIST ) {
	    user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	    user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	    if ( is_year_end_report )
		font_charts.center_put(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,"Faculty Changes & Adjustments Report");
	    else
		font_charts.center_put(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,"Faculty Hires, Promotions, and Departures");
	}
    }
    else {
	//		report_paint_button(refreshFlag);
    }

    // paint the Back button
    int x=REPORT_X1+350, y=BUTTON_Y1;
    button_back.create_text( x-30, y, x+BUTTON_WIDTH, y+BUTTON_HEIGHT, "Back");
    button_back.paint();

    // if no details to show
    if ( cur_dept_ptr->display_faculty_array.size() == 0 ) {
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
	return;
    }

    // begin chwg080999 avoiding (facultyPtr->faculty_recno==0)
    Faculty* facultyPtr = (Faculty*) cur_dept_ptr->display_faculty_array.get(report_faculty_recno);
    // end chwg080999

    if( refreshFlag == INFO_REPAINT ) {
	if ( is_year_end_report ) {
	    browse_faculty.init( REPORT_X1, REPORT_Y1+10, REPORT_X2, REPORT_Y2+10,
				 0, FACULTY_REC_HEIGHT,
				 cur_dept_ptr->cur_faculty_array.size(),
				 disp_display_faculty_rec, NULL ,1 );
	    browse_faculty.open(report_faculty_recno,cur_dept_ptr->cur_faculty_array.size());
	}
	else {
	    browse_faculty.init( REPORT_X1, REPORT_Y1+10, REPORT_X2, REPORT_Y2+10,
				 0, FACULTY_REC_HEIGHT,
				 cur_dept_ptr->display_faculty_array.size(),
				 disp_display_faculty_rec, NULL ,1 );
	    browse_faculty.open(report_faculty_recno,cur_dept_ptr->display_faculty_array.size());
	}
    }
    else {                                          // refreshFlag == INFO_UPDATE
	//			browse_faculty.refresh(-1, cur_dept_ptr->faculty_array.faculty_count);
    }
    report_faculty_recno = browse_faculty.recno();

    last_faculty_count = cur_dept_ptr->faculty_array.faculty_count;
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//
// --- End of function FacultyRes::report_faculty_hired --- //

//------- Begin of function FacultyRes::report_paint_button ------//

void FacultyRes::report_paint_button(int refreshFlag) {
    //-------- paint type buttons ------------//
    int x=REPORT_X1+50, y=BUTTON_Y1;
    button_type[0].create_text( x, y, x+BUTTON_WIDTH, y+BUTTON_HEIGHT, "List" );
    //	button_type[0].set_key( 'L' );

    x+=BUTTON_WIDTH+10;
    button_type[1].create_text( x, y, x+BUTTON_WIDTH, y+BUTTON_HEIGHT, "Detail" );
    //	button_type[1].set_key( 'D' );
    button_type.paint(report_type);

    mode_fac_sum_button.create_text(  REPORT_X1+BUTTON_WIDTH*2+80, BUTTON_Y1,
				      REPORT_X1+BUTTON_WIDTH*3+80, BUTTON_Y1+BUTTON_HEIGHT,"Back",1);
    mode_fac_sum_button.paint();

    //---------- list mode -----------//

    if( report_type==REPORT_TYPE_LIST ) {
	x=REPORT_X1+BUTTON_WIDTH*3+110;
	for( int i=0 ; i<REPORT_MODE1_COUNT ; i++, x+=BUTTON_WIDTH+10 ) {
	    button_mode1_array[i].create_text( x, y, x+BUTTON_WIDTH, y+BUTTON_HEIGHT, reportMode1StrArray[i], 0 );
	    //			button_mode1_array[i].set_key( '1' + i );
	}

	button_mode1_array.paint(report_list_mode);
    }
    else {                                          //------- detail mode ---------//
	Faculty* facultyPtr;
	facultyPtr = cur_dept_ptr->faculty_array.get_unpacked_faculty(report_faculty_recno);
	if( facultyPtr->age()>=55) {
	    x=REPORT_X1+BUTTON_WIDTH*3+90;
	    button_retire.create_text(x, y, x+BUTTON_WIDTH+30, y+BUTTON_HEIGHT,"Retirement",1);
	    button_retire.paint();
	}

	if ( facultyPtr->rank_level != ASSOC_PROF && facultyPtr->rank_level != FULL_PROF ) {
	    button_dismiss.create_text( REPORT_X1+BUTTON_WIDTH*3+230, BUTTON_Y1,
					REPORT_X1+BUTTON_WIDTH*4+230, BUTTON_Y1+BUTTON_HEIGHT,"Dismiss",1);
	    if ( facultyPtr->is_dismissed )
		button_dismiss.disable();
	    button_dismiss.paint();
	}

	if( report_faculty_recno > 1 )
	    image_interface.put_back(REPORT_X2-90,BUTTON_Y1,"L-ARW-UP");
	else
	    image_interface.put_back(REPORT_X2-90,BUTTON_Y1,"L-ARW-DB");

	if( report_faculty_recno < cur_dept_ptr->faculty_array.faculty_count )
	    image_interface.put_back(REPORT_X2-40,BUTTON_Y1,"R-ARW-UP");
	else
	    image_interface.put_back(REPORT_X2-40,BUTTON_Y1,"R-ARW-DB");
    }
}

//------- End of function FacultyRes::report_paint_button ------//

//----- Begin of function FacultyRes::report_detect ------//
//!
void FacultyRes::report_detect() {
    //------- detect type buttons --------//

    if( report_type==REPORT_TYPE_LIST ||
	report_type==REPORT_TYPE_DETAIL ) {
	vga.use_front();
	if( button_type.detect() >= 0 ) {
	    report_type = button_type.button_pressed;
	    sys.redraw_all_flag = 1;
	}
    }

    //------- list mode ----------//
    if( report_type==REPORT_TYPE_LIST ) {
	browse_faculty.detect();

	if( button_mode1_array.detect() >= 0 ) {
	    report_list_mode = button_mode1_array.button_pressed;
	    sys.redraw_all_flag = 1;
	}
    }
    else if( report_type==REPORT_TYPE_DETAIL ) {
	//------- detail mode ----------//
	vga.use_back();
	browse_research.detect();

	vga.use_front();
	Faculty* facultyPtr;
	facultyPtr = cur_dept_ptr->faculty_array.get_unpacked_faculty(report_faculty_recno);
	if( facultyPtr->age()>=55) {
	    if( button_retire.detect() ) {
		report_type=REPORT_RETIRE;
		sys.redraw_all_flag = 1;
	    }
	}

	if ( facultyPtr->rank_level != ASSOC_PROF && facultyPtr->rank_level != FULL_PROF ) {
	    // need to ask player to confirm
	    if ( button_dismiss.detect() ) {
		char displayText[128];
		short okCancel=0;

		// Normal
		if ( player_school.behavioral_response_speed == INPUT_LOW ) {
		    if ( facultyPtr->rank_level == SHORT_TERM_ADJUNCT ) {
			sprintf( displayText, "The contract for %s will be terminated at the end of this year.", facultyPtr->name() );
			facultyPtr->dismiss_trimester = TRIMESTER_PER_YEAR - player_school.cur_trimester;
		    }
		    else
			if ( facultyPtr->rank_level == LONG_TERM_ADJUNCT ) {
			    int yearCount  = (info.game_date-facultyPtr->start_teaching_date) / 365;
			    int monthCount = ((info.game_date-facultyPtr->start_teaching_date) % 365) / 30;
			    int remainYear;

			    if ( monthCount )
				remainYear = 5-yearCount%5-1;
			    else
				remainYear = 5-yearCount%5;

			    sprintf( displayText, "The contract for %s will be terminated at the end of year %d.", facultyPtr->name(), remainYear+info.financial_year() );
			    facultyPtr->dismiss_trimester = (remainYear+1)*TRIMESTER_PER_YEAR-player_school.cur_trimester;
			}
			else
			    if ( facultyPtr->rank_level == ASST_PROF ) {
				int yearCount  = (info.game_date-facultyPtr->start_teaching_date) / 365;
				int monthCount = ((info.game_date-facultyPtr->start_teaching_date) % 365) / 30;
				int remainYear;

				if ( monthCount )
				    remainYear = 3-yearCount%3-1;
				else
				    remainYear = 3-yearCount%3;

				sprintf( displayText, "The contract for %s will be terminated at the end of year %d.", facultyPtr->name(), remainYear+info.financial_year() );
				facultyPtr->dismiss_trimester = (remainYear+1)*TRIMESTER_PER_YEAR-player_school.cur_trimester;
			    }
		}
		else
		    // Faster speed
		    if ( player_school.behavioral_response_speed == INPUT_MEDIUM ) {
			sprintf( displayText, "The contract for %s will be terminated at the end of this year.", facultyPtr->name() );
			facultyPtr->dismiss_trimester = TRIMESTER_PER_YEAR-player_school.cur_trimester;
		    }
		    else
			// Lightspeed
			if ( player_school.behavioral_response_speed == INPUT_HIGH ) {
			    sprintf( displayText, "The contract for %s will be terminated at the end of this trimester.", facultyPtr->name() );
			    facultyPtr->dismiss_trimester = 1;
			}

		if ( box.ask( displayText ) ) {
		    facultyPtr->satisfaction_index *= 0.1;
		    facultyPtr->is_dismissed = 1;
		    sys.redraw_all_flag = 1;
		}
	    }
	}

	if(mouse.single_click(REPORT_X2-90,BUTTON_Y1,REPORT_X2-65,BUTTON_Y1+25)) {
	    if( --report_faculty_recno < 1 ) {
		report_faculty_recno = 1;
		audio.play_wav("BEEPS-N1",audio.int_to_DsVolume(config.sound_effect_volume));
	    }
	    if(report_faculty_recno > 1) {
		image_interface.put_back(REPORT_X2-90,BUTTON_Y1,"L-ARW-DN");
		vga.blt_buf(REPORT_X2-90,BUTTON_Y1,REPORT_X2-65,BUTTON_Y1+25);
		audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    }
	    sys.redraw_all_flag=1;
	}

	if(mouse.single_click(REPORT_X2-40,BUTTON_Y1,REPORT_X2-15,BUTTON_Y1+25)) {
	    if( ++report_faculty_recno > cur_dept_ptr->faculty_array.faculty_count ) {
		report_faculty_recno = cur_dept_ptr->faculty_array.faculty_count;
		audio.play_wav("BEEPS-N1",audio.int_to_DsVolume(config.sound_effect_volume));
	    }
	    if(report_faculty_recno < cur_dept_ptr->faculty_array.faculty_count) {
		image_interface.put_back(REPORT_X2-40,BUTTON_Y1,"R-ARW-DN");
		vga.blt_buf(REPORT_X2-40,BUTTON_Y1,REPORT_X2-15,BUTTON_Y1+25);
		audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    }
	    sys.redraw_all_flag=1;
	}
    }
    else if( report_type==REPORT_RETIRE ) {
	vga.use_front();
	if(retire_spinner.detect()) {
	}

	if( button_retire_offer.detect() ) {
	    Faculty* facultyPtr;
	    String str1;
	    int rc;

	    facultyPtr = cur_dept_ptr->faculty_array.get_unpacked_faculty(report_faculty_recno);
	    str1 = facultyPtr->name();

	    if(cur_dept_ptr->faculty_array.is_deleted(report_faculty_recno))
		rc=2;
	    else {
		int offerDollar=(int)(facultyPtr->salary*(*(retire_spinner.var_ptr))/100);
		rc=facultyPtr->think_accept_retire(offerDollar);
	    }

	    if(rc==1)
		str1 += "  accepts the retirement offer. ";
	    else
		str1 += "  rejects the retirement offer. ";
	    box.msg(str1);

	    report_type=REPORT_TYPE_DETAIL;
	    sys.redraw_all_flag = 1;
	}
	if( button_retire_cancel.detect() ) {
	    report_type=REPORT_TYPE_DETAIL;
	    sys.redraw_all_flag = 1;
	}
    }
}

//------- End of function FacultyRes::report_detect ------//

// --- Begin of function FacultyRes::report_hired_detect --- //
//
int FacultyRes::report_hired_detect() {
    if (button_back.detect()) {
	if ( faculty_res.is_year_end_report ) {
	    button_back.push();
	    sys.redraw_all_flag = 1;
	    return 1;
	}
	else {
	    button_back.push();
	    current_report_type = last_report_type;     // REPORT_TYPE_INSTITUTION_TURNOVER
	    //			current_report_mode = 0;
	    sys.redraw_all_flag = 1;
	}
    }
    browse_faculty.detect();
    return 0;
}

//
// --- End of function FacultyRes::report_hired_detect --- //

//-------- Begin of static function disp_faculty_rec --------//
//
static void disp_faculty_rec(int recNo, int x, int y, int refreshFlag) {
    Faculty* facultyPtr = cur_dept_ptr->faculty_array.get_unpacked_faculty(recNo);

    vga_back.d3_panel_down( x, y, x+browse_faculty.rec_width-6, y+browse_faculty.rec_height-1, 1 );

    //--------- display the faculty portrait ---------//

    // 0-don't paint centre
    vga_back.d3_panel_down( x+5, y+8, x+FACULTY_PHOTO_WIDTH+6, y+FACULTY_PHOTO_HEIGHT+9, 1, 0 );

    vga_back.put_bitmap( x+6, y+9, faculty_photo_res[facultyPtr->photo_id]->bitmap_ptr);

    // 0=don't paint centre
    vga_back.d3_panel_down( x+74, y+4, x+browse_faculty.rec_width-10, y+browse_faculty.rec_height-6, 1, 0 );

    //------------ display info -------------//

    switch( faculty_res.report_list_mode ) {
    case REPORT_MODE1_BASIC:
	facultyPtr->disp_summary_basic(x+79, y+7, x+350, refreshFlag,0);
	break;

    case REPORT_MODE1_ACTIVITY:
	facultyPtr->disp_summary_activity(x+79, y+7, x+350, refreshFlag);
	break;

    case REPORT_MODE1_ASSEST:                     //### fred 1023
	facultyPtr->disp_summary_assess(x+79, y+7, x+350, refreshFlag);
	break;
    }
}

//-------- End of static function disp_faculty_rec --------//

//-------- Begin of static function disp_display_faculty_rec --------//
//
static void disp_display_faculty_rec(int recNo, int x, int y, int refreshFlag) {
    Faculty* facultyPtr;
    if ( faculty_res.is_year_end_report )
	facultyPtr = (Faculty*) cur_dept_ptr->cur_faculty_array.get(recNo);
    else
	facultyPtr = (Faculty*) cur_dept_ptr->display_faculty_array.get(recNo);

    if (( facultyPtr->employ_status == 0 ) && (faculty_res.is_year_end_report ))
	return;

    vga_back.d3_panel_down( x, y, x+browse_faculty.rec_width-6, y+browse_faculty.rec_height-1, 1 );

    //--------- display the faculty portrait ---------//

    // 0-don't paint centre
    vga_back.d3_panel_down( x+5, y+8, x+FACULTY_PHOTO_WIDTH+6, y+FACULTY_PHOTO_HEIGHT+9, 1, 0 );

    vga_back.put_bitmap( x+6, y+9, faculty_photo_res[facultyPtr->photo_id]->bitmap_ptr);

    // 0=don't paint centre
    vga_back.d3_panel_down( x+74, y+4, x+browse_faculty.rec_width-10, y+browse_faculty.rec_height-6, 1, 0 );

    //------------ display info -------------//

    facultyPtr->disp_summary_basic(x+79, y+7, x+350, refreshFlag,1);
}

//-------- End of static function disp_display_faculty_rec --------//

//------ Begin of function Faculty::disp_summary_basic -------//
//!
void Faculty::disp_summary_basic(int x, int y, int xColumn2, int refreshFlag, int displayModeFlag)
{
    int x2=x+97;

    //--------------- display fields -----------//

    font_chartsm.field( x, y   , "Name"         , x2, name(), xColumn2-1, refreshFlag );
    font_chartsm.field( x, y+17, "Gender/ethnicity", x2, game_str_res.gender_ethnic_str(gender_ethnic_group), xColumn2-1, refreshFlag );
    font_chartsm.field( x, y+34, "Age"        , x2, age(), 1, xColumn2-1, refreshFlag );
    font_chartsm.field( x, y+51, "Rank"      , x2, game_str_res.rank_str(rank_level), xColumn2-1, refreshFlag );

    x =xColumn2;
    x2=x+162;

    font_chartsm.field( x, y   , "Salary"           , x2, salary, 2, REPORT_X2-30, refreshFlag );
    //	font_chartsm.field( x, y+17, "Employed period"	, x2, employed_period(), REPORT_X2-30, refreshFlag );
    if ( displayModeFlag ) {
	char employStr[123];
	if ( employ_status == 0 )
	    strcpy(employStr,"0 years");
	else
	    strcpy(employStr,employed_period());
	font_chartsm.field( x, y+17, "Time in rank" , x2, employStr, REPORT_X2-30, refreshFlag );
    }
    else
	font_chartsm.field( x, y+17, "Time in rank" , x2, employed_period(), REPORT_X2-30, refreshFlag );
    if ( displayModeFlag ) {
	char statusStr[123];
	switch(employ_status) {
	case 0: strcpy(statusStr,"New hire"); break;
	case 1: strcpy(statusStr,"Departure"); break;
	case 2: strcpy(statusStr,"Promotion"); break;
	default: strcpy(statusStr,"");
	}
	font_chartsm.field( x, y+34, "Status change", x2, statusStr, REPORT_X2-30, refreshFlag );
    }
    else
	font_chartsm.field( x, y+34, "Off-duty trimester", x2, game_str_res.trimester_str(off_duty_trimester), REPORT_X2-30, refreshFlag );
    font_chartsm.field( x, y+51, "Satisfaction index", x2, (int) satisfaction_index, 3, REPORT_X2-30, refreshFlag );

    if( faculty_res.report_type==REPORT_TYPE_LIST) {
	x2+=140;
	int h1,h2,h3;
	h1=(int)(21*performance_teaching/100);
	h2=(int)(21*performance_scholarship/100);
	h3=(int)(21*performance_research/100);
	vga_back.bar(x2-1,y+62-h3-h2-h1,x2+13,y+64,V_BLACK);
	vga_back.v_indicator(x2, y+63-h3-h2-h1, x2+12, y+63-h3-h2, performance_teaching, performance_teaching, VGA_RED );
	vga_back.v_indicator(x2, y+63-h3-h2, x2+12, y+63-h3, performance_scholarship, performance_scholarship, VGA_YELLOW );
	vga_back.v_indicator(x2, y+63-h3, x2+12, y+63, performance_research, performance_research, VGA_SEA_GREEN );
    }
}

//-------- End of function Faculty::disp_summary_basic -------//

//------ Begin of function Faculty::disp_summary_activity -------//
//!
void Faculty::disp_summary_activity(int x, int y, int xColumn2, int refreshFlag) {
    int x2=x+200;

    //--------------- display fields -----------//
    font_chartsm.field( x, y   , "Teaching load",   x2, hour_str(teaching_contact_hour), xColumn2-1, refreshFlag );
    font_chartsm.field( x, y+17, "Course preparation", x2, hour_str(discretionary_hour_array[DT_COURSE_PREPARATION]), xColumn2-1, refreshFlag );
    font_chartsm.field( x, y+34, "Out-of-class student contact", x2, hour_str(discretionary_hour_array[DT_OUT_OF_CLASS_STUDENT_CONTACT]), xColumn2-1, refreshFlag );
    font_chartsm.field( x, y+51, "Educational development", x2, hour_str(discretionary_hour_array[DT_EDUCATIONAL_DEVELOPMENT]), xColumn2-1, refreshFlag );
    x=xColumn2;
    x2=x+210;
    font_chartsm.field( x, y   , "Research", x2, hour_str(discretionary_hour_array[DT_RESEARCH]), REPORT_X2-30, refreshFlag  );
    font_chartsm.field( x, y+17, "Scholarship", x2, hour_str(discretionary_hour_array[DT_SCHOLARSHIP]), REPORT_X2-30, refreshFlag  );
    font_chartsm.field( x, y+34, "Institutional and public service", x2, hour_str(discretionary_hour_array[DT_INSTITUTIONAL_AND_PUBLIC_SERVICE]), REPORT_X2-30, refreshFlag  );
    font_chartsm.field( x, y+51, "Total", x2, hour_str(total_hour()), REPORT_X2-30, refreshFlag  );
}

//-------- End of function Faculty::disp_summary_activity -------//

//------ Begin of function Faculty::disp_summary_assess -------//
//!
void Faculty::disp_summary_assess(int x, int y, int xColumn2, int refreshFlag) {
    int x2=x+135;

    //--------------- display fields -----------//

    int x3=x2+39;

    font_chartsm.field( x, y   , "Talent: Teaching"   , x2, (int) talent_teaching, 3, x3, refreshFlag );
    font_chartsm.field( x, y+17, "Talent: Scholarship", x2, (int) talent_scholarship, 3, x3, refreshFlag);
    font_chartsm.field( x, y+34, "Talent: Research"   , x2, (int) talent_research, 3, x3, refreshFlag );

    x3++;
    int x4 = xColumn2-5;

    vga_back.indicator( x3, y+3 , x4, y+14, talent_teaching   , 100, VGA_RED , -2);
    vga_back.indicator( x3, y+20, x4, y+31, talent_scholarship, 100, VGA_YELLOW , -2);
    vga_back.indicator( x3, y+37, x4, y+48, talent_research   , 100, VGA_SEA_GREEN , -2);

    //--------------------------------------------------//

    x =xColumn2;
    x2=x+170;

    x3=x2+39;

    font_chartsm.field( x, y   , "Performance: Teaching"   , x2, (int) performance_teaching, 3, x3, refreshFlag  );
    font_chartsm.field( x, y+17, "Performance: Scholarship", x2, (int) performance_scholarship, 3, x3, refreshFlag  );
    font_chartsm.field( x, y+34, "Performance: Research"   , x2, (int) performance_research, 3, x3, refreshFlag  );
    font_chartsm.field( x, y+51, "Satisfaction index"      , x2, (int) satisfaction_index, 3, x3, refreshFlag  );

    x3++;
    x4=REPORT_X2-30;

    vga_back.indicator( x3, y+3 , x4, y+14, performance_teaching   , 100, VGA_RED , -2);
    vga_back.indicator( x3, y+20, x4, y+31, performance_scholarship, 100, VGA_YELLOW , -2);
    vga_back.indicator( x3, y+37, x4, y+48, performance_research   , 100, VGA_SEA_GREEN , -2);
    vga_back.indicator( x3, y+54, x4, y+65, satisfaction_index     , 100, VGA_VIOLET , -2);
}

//-------- End of function Faculty::disp_summary_assess -------//

//------ Begin of function Faculty::disp_detail_std -------//
//!
void Faculty::disp_detail_std(int refreshFlag) {
    int x=REPORT_X1, y=REPORT_Y1;

    vga_back.d3_panel_down( REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y2, 2);

    //--------- display the faculty portrait ---------//
    // 0-don't paint centre
    vga_back.d3_panel_down( x+5, y+8, x+FACULTY_PHOTO_WIDTH+6, y+FACULTY_PHOTO_HEIGHT+9, 1, 0 );
    //## fred 990414 //BUGHERE photo_id == 0
    vga_back.put_bitmap( x+6, y+9, faculty_photo_res[photo_id]->bitmap_ptr );

    //---------- display details ----------//

    x+=73;
    y+=5;

    int xColumn2 = x+270;

    disp_summary_basic(x, y, xColumn2, refreshFlag, 0 );

    x=REPORT_X1+5;

    user_interface.h_line(REPORT_X1, y+72, REPORT_X2-REPORT_X1+1, 2 );

    y+=77;

    disp_summary_activity(x, y, xColumn2, refreshFlag );

    user_interface.h_line(REPORT_X1, y+72, REPORT_X2-REPORT_X1+1, 2 );

    y+=77;

    disp_summary_assess(x, y, xColumn2, refreshFlag );

    user_interface.h_line(REPORT_X1, y+72, REPORT_X2-REPORT_X1+1, 2 );

    y+=77;

    //--------------------------------------------//

    disp_research_proposal(y, refreshFlag);
}

//-------- End of function Faculty::disp_detail_std -------//

//------ Begin of function Faculty::disp_research_proposal -------//
//!
void Faculty::disp_research_proposal(int y, int refreshFlag) {
    int x=REPORT_X1+5;

    //------ display browser heading -------//

    font_chartsm.put( x+10   , y, "Sponsored research project dollars" );
    font_chartsm.put( x+355, y, "Status" );
    font_chartsm.put( x+500, y, "Months to status change" );

    y+=17;

    if( refreshFlag == INFO_REPAINT ) {
	//----------- create browser -----------//
	user_interface.rect(REPORT_X1+5, y-20, REPORT_X2-5, REPORT_Y2-25);

	browse_research.init( REPORT_X1+5, y, REPORT_X2-5, REPORT_Y2-25, 0, RESEARCH_REC_HEIGHT,
			      research_proposal_count, disp_research_rec, NULL, 0 );

	browse_research.open(faculty_res.report_research_recno,research_proposal_count);
    }
    else {                                          // refreshFlag == INFO_UPDATE
	//		if( research_proposal_count != last_research_count )      // no. of sharepersons are changed, refresh it
	//		browse_research.paint();
	//		else
	//			browse_research.update();                // update only
    }
    user_interface.rect(REPORT_X1+5, y-20, REPORT_X2-5, REPORT_Y2-25);
    browse_research.paint();
    browse_research.refresh(-1, research_proposal_count);
    faculty_res.report_research_recno = browse_research.recno();

    last_research_count = research_proposal_count;

    //---------- display total ------------//

    y=REPORT_Y2-20;

    font_chartsm.field( x, y, "Total projects", x+110, research_proposal_count, 1, x+195, refreshFlag );
    x+=340;

    //## chea 221199
    //	font_chartsm.field( x, y, "Total monthly expenditures", x+195, research_month_expense * 1000, 2, REPORT_X2-5, refreshFlag );		// it's in the thousand unit
    // it's in the thousand unit
    font_chartsm.field( x, y, "Total monthly expenditures", x+195, research_month_expense , 2, REPORT_X2-5, refreshFlag );

}

//-------- End of function Faculty::disp_research_proposal -------//

//-------- Begin of static function disp_research_rec --------//
//
static void disp_research_rec(int recNo, int x, int y, int refreshFlag) {
    Faculty* facultyPtr = cur_dept_ptr->faculty_array.get_unpacked_faculty(faculty_res.report_faculty_recno);

    ResearchProposal* researchProposal = facultyPtr->research_proposal_array + recNo - 1;

    x+=2;
    y+=1;

    //## chea 221199
    //	font_chartsm.disp(x	 , y, (int) researchProposal->total_dollars *1000, 2, x+200 );  //## chea 161199 1.8.3
    font_chartsm.disp(x  , y, (int) researchProposal->total_dollars , 2, x+200 );
    font_chartsm.disp(x+350, y, researchProposal->status==RESEARCH_PROPOSED ? "Proposed" : "Active", x+495 );
    font_chartsm.disp(x+500, y, (researchProposal->date_to_next_status-info.game_date)/30, 1, x+550 );
}

//-------- End of static function disp_research_rec --------//
