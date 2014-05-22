//Filename: OINFO.CPP
//Description : Info class

#include <time.h>
#include <ALL.H>
#include <ODATE.H>
#include <OINFO.H>
#include <OIMGRES.H>
#include <OMISC.H>
#include <OMUSIC.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OFIRMRES.H>
#include <OSTR.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OFONT.H>
#include <ODEPT.H>
#include <OFIRM.H>
#include <OWORLD.H>
#include <OSTUDENT.H>
#include <OFACULTY.H>
#include <OCOURSE.H>
#include <OSCHOOL.H>
#include <OPSCHOOL.H>
#include <OSCHLRES.H>
#include <OCOLTBL.H>
#include <OFINANCE.H>
#include <OAthleti.h>
#include <ODEVELOP.H>
#include <OFACILIT.H>
#include <OLIBTECH.H>
#include <OINVEST.H>                              //##### fred 981016 #####//
#include <ODEPTRES.H>
#include <OENROLL.H>                              //##### fred 980908 #####//
#include <OSTUOFF.H>                              //##### fred 980908 #####//
#include <OTEXT.H>                                //##### chw 981014 #####//
#include <OTASK.H>                                //##### fred 981015 #####//
#include <OCHANCE.H>                              //##### fred 981015 #####//
#include <OCONFIG.H>                              //##### chea 140799 #####//
#include <OBOX.H>
#include <OLETTER.H>                              //##### chea 140799 #####//
#include <OMATH.H>                                //##### chea 240100 #####//

//---------- define constant ------------//

#define DATE_BMP_RES_FILE_NAME    DIR_RES"I_DATE.RES"

//---------- define static variables ------------//

static int last_month = 0, last_year = 0, last_day = 0;
static short digit_y1, year_x1_1, year_x1_2, day_x1_0, day_x1_1, day_x1_2;

//--------- define marco function ----------//

#define YIELD { if ( info.game_day == 1 ) sys.yield(); }

//------- declare static function ----------//

static void update_faculty_count();

//-------- Begin of function Info::Info --------//
//!
Info::Info() {
    disp_6_graphs_flag=true;
}

//--------- End of function Info::Info ---------//

//-------- Begin of function Info::~Info --------//
//!
Info::~Info() {
    deinit();
}

//--------- End of function Info::~Info ---------//

//-------- Begin of function Info::init --------//
//!
void Info::init() {
    deinit();
#define GAME_START_YEAR 1
    game_day   = 1;
    game_month = 9;
    game_year  = GAME_START_YEAR;                   // may have bug if 99!!!

    game_start_date = date.julian( game_year, game_month, game_day );

    game_date  = game_start_date;
    year_day   = date.day_year( game_year, game_month, game_day );

    year_passed= 1;                                 // no. of years has been passed since the game begins

    //#### begin fred 0911 ####//
    graph_year_passed = 1;
    graph_month_passed = 1;                         // see next_day() also
    graph_trimester_passed = 1;

    graph_year = 3;                                 // previous, this and next year on graph
    graph_month = 3;
    graph_trimester = 3;
    //#### end fred 0911 ####//

    //----------------------------------//

    start_play_time = m.get_time();                 // the time player start playing the game
    total_play_time = 0;                            // total time the player has played in all saved games

    //############### begin Norris 0706 ##############//

    date_res_bitmap.init(DATE_BMP_RES_FILE_NAME, 1, 0);

    short *shortPtr = (short *) date_res_bitmap.get_ptr("0");
    short digitWidth = *shortPtr++;
    short digitHeight = *shortPtr;

    digit_y1 = (DIGIT_Y1 + DIGIT_Y2 - digitHeight) / 2;
    day_x1_2 = (DAY_X1 + DAY_X2) / 2;
    day_x1_1 = day_x1_2 - digitWidth;
    day_x1_0 = day_x1_2 - digitWidth / 2;
    year_x1_2 = (YEAR_X1 + YEAR_X2) / 2;
    year_x1_1 = year_x1_2 - digitWidth;

    //################# end Norris 0706 ##############//

    disp_panel_init();
    image_interface.put_large( &vga_back, 0, 0, "MAINSCR" );
}

//--------- End of function Info::init ---------//

//-------- Begin of function Info::deinit --------//
//!
void Info::deinit() {
    //############### begin Norris 0706 ##############//

    if( top_info_bitmap ) {
	mem_del( top_info_bitmap );
	top_info_bitmap = NULL;

	mem_del( bottom_info_bitmap );
	bottom_info_bitmap = NULL;

	mem_del( surplus_info_bitmap );
	surplus_info_bitmap = NULL;

	mem_del( score_info_bitmap );
	score_info_bitmap = NULL;

	mem_del( uscore_info_bitmap );
	uscore_info_bitmap = NULL;

	mem_del( date_info_bitmap );
	mem_del( year_info_bitmap );

	mem_del( top_left_column_bitmap );
	mem_del( top_right_column_bitmap );

	mem_del( bottom_left_column_bitmap );
	mem_del( bottom_right_column_bitmap );

	mem_del( menu_up_bitmap );
	mem_del( menu_down_bitmap );

	mem_del( more_report_bitmap );
	more_report_bitmap = NULL;

	mem_del( help_quest_bitmap );
	help_quest_bitmap = NULL;

	mem_del( list_button_bitmap );
	list_button_bitmap = NULL;

	for (int i = 0; i < REPORT_ITEMS + 1; i++)
	    mem_del( report_bitmap[i] );
    }

    //################# end Norris 0706 ##############//
}

//--------- End of function Info::deinit ---------//

//------- Begin of function Info::init_random_seed ------//
//!
//! [unsigned] randomSeed - if given, it will be the random seed of the game.
//!									random seed. otherwise a random seed will be
//!								   picked.
//!								   (default:0)
//!
void Info::init_random_seed(int randomSeed) {
    if( randomSeed )
	random_seed = randomSeed;
    else {
	randomSeed = time(NULL);
	randomSeed = (int) _rotr( randomSeed, 4 );
	if( randomSeed < 0 )
	    randomSeed = ~randomSeed;
	if( randomSeed == 0 )
	    randomSeed = 1;
	random_seed = randomSeed;
    }

    m.set_random_seed(random_seed);
}

//------- End of function Info::init_random_seed ------//

//-------- Begin of function Info::next_day ---------//

void Info::next_day() {
    inc_game_day();

    //-------- show waiting mouse cursor it is the beginning of a month -----------//

    int oldCursor;

    if ( game_day == 1 ) {
	oldCursor = mouse_cursor.get_icon();
	mouse_cursor.set_icon(CURSOR_WAITING);
	sys.redraw_all_flag=1;                        // ## chwong0625
    }

    //-------- yield player_school ---------//

    player_school.next_day();                       // it must be called first as it set trimester;
    // included enrollment's run_student_dropout_graduate()
    // next_trimester() will be called and hence dropout, graduate, faculty departure, etc
    YIELD;

    //------- call core models' next_day() ------//

    enroll_res.next_day();                          // will recalc student_count

    development_office.next_day();

    YIELD;

    //---------------------// course enrollment, after enrollment model

    if ( is_begin_trimester() )
	new_trimester_course_enrollment();

    finance.next_day();                             // should after new_trimester_course_enrollment, to initial variables to calculate net distance learning expenditures

    YIELD;

    //---- call offices' next_day() functions -----//
    // offices which must called after finance.next_day

    facility_office.next_day();                     // should before(!?) course enrollment, after development office, before deparment_array's course_array.next_day()
    library_tech_office.next_day();

    athletics_office.next_day();                    // after enrollment
    student_office.next_day();
    investment_office.next_day();                   // after development_office

    YIELD;

    //---- call data array next_day() functions -----//
    // should(?) after all offices for calc pi

    department_array.next_day();                    // after course selection

    department_res.general_dept.next_day();

    school_res.next_day();

    YIELD;

    //------- start of a new trimster: calc pi of player_school ---------//

    if ( is_begin_trimester() ) {
	player_school.update_history(UPDATE_TRIMESTER);
    }

    //--------- start of a new month -------------//

    if ( info.game_day == 1 ) {                     //## chea 300899 change to call after UPDATE_TRIMESTER
	player_school.update_history(UPDATE_MONTH);

	// ---- start of a new year ------//

	if ( info.game_month == finance.fiscal_year_start_month ) {
	    player_school.update_history(UPDATE_YEAR);

	    //--- re-calculate percentages and totals in financial reports to make sure all are correct. Although the calculation is done once already, to be safe, do it once again here after all new year processes are done. ----//

	    finance.calc_total_last_year();             // last year
	    finance.calc_projected_total_n_percent(1);  // next year. 1-re-calculation, not first time calculation
	}
    }

    //------- scenario check targets -------//

    if (info.prerun_year==0)
	player_school.scenario_check_target();

    //---- the custom game is approaching its time limit of 99 years, display warning message ----//

    if (info.prerun_year==0) {
	int lastYear = 90;

#ifndef ADMIN                                 // standard and demo versions have 3 year time limit only
	lastYear = 3;
#endif

	if( info.game_year == lastYear-1 && info.game_month == 9 && info.game_day==1 ) {
	    news_array.open_game_near_end();            // the game will soon end
	}
	else if( info.game_year == lastYear && info.game_month == 9 && info.game_day==1 ) {
	    news_array.open_game_end();                 // the game ends
	}
	// retire on the next day
	else if( info.game_year == lastYear && info.game_month == 9 && info.game_day>1 ) {
	    sys.set_staying_view_mode(MODE_RETIREMENT);

	    if ( config.cd_music_volume <=0 )
		music.stop();
	}
    }

    //---- process chances and objectives ----//

    if (info.prerun_year==0) {
	chance_event.next_day();
	task_array.next_day();                        // order: after all departments and offices

	player_school.think_protagonist();

    }

    //------- update initializzation progress bar -------------//

    if( info.prerun_year==1 && info.game_day==1 ) {
	// increment 2% per month
	box.progress_update( 28 + int( float(info.game_date - info.game_start_date)/5.7f ) );
    }

    //------- end of prerun year -----------//

    //## chea 190799 scenario not run prerun year
    if (info.game_year==2 && info.game_month==9 && info.game_day==1 && info.prerun_year==1) {
	end_prerun_year();
	oldCursor=CURSOR_NORMAL;
    }

    //---------------------------------------//

    if ( game_day == 1 )
	mouse_cursor.restore_icon(oldCursor);
}

//---------- End of function Info::next_day --------//

//-------- Begin of function Info::inc_game_day ---------//

void Info::inc_game_day() {
    //--- process game day --------//

    if( ++game_day > 30 )
	game_day = 30;                                // game_day is limited to 1-30 for
    // calculation of e.g. revenue_30days()
    game_date++;

    week_day=game_date%7;

    if( date.month(game_date) != game_month ) {
	game_day   = 1;
	game_month = date.month(game_date);

	graph_month_passed = min(graph_month_passed+1, HISTORY_MONTH_COUNT);
	graph_month = min(graph_month+1, HISTORY_MONTH_COUNT);

	if ( game_month == finance.fiscal_year_start_month ) {
	    year_passed++;

	    graph_year_passed = min(graph_year_passed+1, HISTORY_YEAR_COUNT);
	    graph_year = min(graph_year+1, HISTORY_YEAR_COUNT);
	}
    }

    if( date.year(game_date) != game_year ) {
	game_month = 1;
	game_year = date.year(game_date);
    }

    // set year_day
    year_day = date.day_year( game_year, game_month, game_day );
}

//---------- End of function Info::inc_game_day --------//

//-------- Begin of function Info::new_trimester_course_enrollment --------//
//!
void Info::new_trimester_course_enrollment() {
    for( int i=department_array.size() ; i>0 ; i--) {
	if( department_array.is_deleted(i) )
	    continue;

	// clear course data in last trimester
	department_array[i]->course_array.next_trimester();
	//sys.yield();
    }

    int count_course = 0;

    for( int i=department_array.size(); i>0; i--) {
	if( department_array.is_deleted(i) )
	    continue;

	Department* deptPtr = department_array[i];

	deptPtr->student_array.select_course();
    }

    department_res.general_dept.student_array.select_course();

    // Item 8d v)
    update_distance_learning_variables();

    int w;
    int gd;
    avg_tal_acd = 0.0;
    avg_tal_ext = 0.0;
    avg_tal_ath = 0.0;
    int total_stu_no = 0;
    StudentArray* stuArray_cal_tal;
    Student *studentPtr_cal_tal;

    err_when( department_array.size() == 0 );

    for ( w=department_array.size(); w>=0; w-- ) {
	if (w) {
	    if ( department_array.is_deleted(w) )
		continue;
	    stuArray_cal_tal = &(department_array[w]->student_array);

	}
	else {
	    stuArray_cal_tal = &(department_res.general_dept.student_array);
	}

	// loop student

	for (gd=stuArray_cal_tal->size(); gd>0; gd--) {
	    if ( stuArray_cal_tal->is_deleted(gd) )
		continue;
	    studentPtr_cal_tal = stuArray_cal_tal->operator[](gd);
	    if ( studentPtr_cal_tal->talent_academic == 0)
		continue;
	    avg_tal_acd += studentPtr_cal_tal->talent_academic;
	    avg_tal_ext += studentPtr_cal_tal->talent_extracurricular;
	    avg_tal_ath += studentPtr_cal_tal->talent_athletics;
	    total_stu_no ++;
	}
    }

    avg_tal_acd /= total_stu_no;
    avg_tal_ext /= total_stu_no;
    avg_tal_ath /= total_stu_no;

    //##200100 chea END loop all student to cal avg. talent

    //##200100 chea BEGIN loop all student to make sure they all have talent

    int s;
    int g;
    StudentArray* stuArray;
    Student *studentPtr;
    //		float talentArr[TALENT_VAR_COUNT];
    err_when( department_array.size() == 0 );
    for ( s=department_array.size(); s>=0; s-- ) {
	if (s) {
	    if ( department_array.is_deleted(s) )
		continue;
	    stuArray = &(department_array[s]->student_array);

	}
	else {
	    stuArray = &(department_res.general_dept.student_array);
	}

	// loop student

	for (g=stuArray->size(); g>0; g--) {
	    if ( stuArray->is_deleted(g) )
		continue;
	    studentPtr = stuArray->operator[](g);

	    if (studentPtr->talent_academic == 0) {

		//try cal individual talent  BUGHERE
		char studentLevel = studentPtr->student_level;
		char stuSeg = studentPtr->student_segment;
		char genderEthnicGroup = studentPtr->gender_ethnic_group;

		if ( studentLevel == UG_TRADITION ) {
		    //-------- assume: enroll_res.init() (dbf init) called --------//
		    studentPtr->talent_academic = (char)(math.get_random_snd(avg_tal_acd, float(1.5),true));
		    studentPtr->talent_extracurricular = (char)math.get_random_snd(avg_tal_ext, float(1.5), true );
		    studentPtr->talent_athletics = (char)math.get_random_snd(avg_tal_ath, float(1.5), true );
		}
		else {
		    //-------- assume: value will be assigned after enroll_res.init_data() called
		    if ( studentLevel == DOCTOR )
			studentPtr->talent_academic = (char)(m.fmin(100.0f, math.get_random_snd(math.single_response_func(0.7f, 1.3f, 100, 50,avg_tal_acd ), float(1.5), true )));
		    else
			studentPtr->talent_academic = (char)(math.get_random_snd(avg_tal_acd, float(1.5),true));
		    studentPtr->talent_extracurricular = 0;
		    studentPtr->talent_athletics       = 0;

		}
	    }

	}
    }
}

//-------- End of function Info::new_trimester_course_enrollment --------//

//-------- Begin of function Info::end_prerun_year --------//
//!
//! end of prerun year
//! if want to change the length of the prerun years.have to change the facultyAge in odept_gf.cpp as well
//! if (info.game_year==2 && info.game_month==9 && info.game_day==1 && info.prerun_year==1 && player_school.scenario_id <= 0) //## chea 190799 scenario not run prerun year
//!
void Info::end_prerun_year() {
    game_day   = 1;
    game_month = 9;
    game_year  = GAME_START_YEAR;                   // may have bug if 99!!!

    game_start_date = date.julian( game_year, game_month, game_day );

    game_date  = game_start_date;
    year_day   = date.day_year( game_year, game_month, game_day );
    year_passed= 1;                                 // no. of years has been passed since the game begins

    graph_year_passed = 1;
    graph_month_passed = 1;                         // see next_day() also
    graph_trimester_passed = 1;
    graph_year = 3;                                 // previous, this and next year on graph
    graph_month = 3;
    graph_trimester = 3;
    chance_event.profit_competition_act = 0;        //## chea 091199 4 chance only

    start_play_time = m.get_time();                 // the time player start playing the game
    total_play_time = 0;                            // total time the player has played in all saved games

    config.disp_year_end_report=1;                  //enable end of year report

    //----- update faculty count ---------//

    update_faculty_count();

    //---- set player_school.sub_score_start[SCORE_VAR_COUNT], set the inputs to the trustee evaluation score at the beginning of the game ---//

    player_school.init_score();

    //---- call scenario initialization ----//

    player_school.init_scenario();

    task_array.rand_init = 1.0f - m.random(15)/10;  // for init only - do this to init the prestige

    box.progress_update(89);
    box.progress_val=0;

    letter.init_welcome_letter();                   // addif

    // ##### begin Marco ##### //
    finance.save_initial_value();
    for (int i = department_array.size() ; i > 0; i--)
	department_array[i]->save_initial_data();

    department_array.save_initial_data();
    player_school.save_initial_data();

    // not update_appropiation here, moved to new Finance::new_fiscal_year
    //if ( player_school.is_public() )
    //{
    //	finance.update_appropiation();
    //	double temp = finance.revenue_array[AC_STATE_APPROPRIATION].total;
    //	RevenueItemChange* projRev;
    //	projRev = &finance.projected_revenue_array[AC_STATE_APPROPRIATION];
    //	projRev->change_budget_year.total
    //		= finance.calc_change( finance.budget_revenue_array[AC_STATE_APPROPRIATION].total, temp );
    //	projRev->change_last_year.total = finance.calc_change( projRev->this_year.total, temp );
    //}

    enroll_res.save_initial_data();
    development_office.save_initial_data();
    facility_office.save_initial_data();
    library_tech_office.save_initial_data();
    investment_office.save_initial_data();
    department_res.general_dept.save_initial_data();
    athletics_office.save_initial_data();
    // ##### End Marco ##### //

    //--- reset info.prerun_year to 0 -----//

    info.prerun_year=0;

}

//-------- End of function Info::end_prerun_year --------//

//-------- Begin of function update_faculty_count --------//
//!
//! Called by Info::end_prerun_year() only.
//!
static void update_faculty_count() {
    //------ for init letter loop faculty ------//

    department_array.faculty_count_tenured = 0;
    department_array.faculty_count_on_tenure_line = 0;
    Department* departPtr;

    for (int cond=department_array.size(); cond>=0; cond-- ) {
	if ( department_array.is_deleted(cond) )
	    continue;

	departPtr = department_array[cond];

	for (int con = departPtr->faculty_array.size(); con>0; con--) {
	    if (departPtr->faculty_array.is_deleted(con))
		continue;

	    switch(departPtr->faculty_array[con]->rank_level) {
	    case ASSOC_PROF:
	    case FULL_PROF:
		department_array.faculty_count_tenured++;
	    case ASST_PROF:
		department_array.faculty_count_on_tenure_line++;
		break;
	    }

	    //----- try to end the prerun year research project -----//

	    int testU =0;
	    int testU1 =0;
	    Faculty* facultyPtr;                        // for test only

	    for (int e = 0; e < MAX_RESEARCH_PROPOSAL ; e++) {
		testU = departPtr->faculty_array[con]->research_proposal_array[e].date_to_next_status;

		if(departPtr->faculty_array[con]->research_proposal_array[e].date_to_next_status!=0)
		    departPtr->faculty_array[con]->research_proposal_array[e].date_to_next_status -= 365;

		facultyPtr = departPtr->faculty_array[con];
		testU1 = departPtr->faculty_array[con]->research_proposal_array[e].date_to_next_status;
	    }
	}
    }
}

//-------- End of function update_faculty_count --------//

//-------- Begin of function Info::disp_panel_init --------//
//!
bool Info::is_begin_trimester() {
    return ( game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	     && game_month == player_school.trimester_array[player_school.cur_trimester].start_month );
}

//-------- Begin of function Info::disp_panel_init --------//

//-------- Begin of function Info::disp_panel_init --------//
//!
void Info::disp_panel_init() {
    image_interface.put_large( &vga_back, 0, 0, "MAINSCR2" );

    //------ keep a copy of bitmap of the panel texture -----//

    top_info_bitmap = vga_back.save_area( TOP_INFO_X1, TOP_INFO_Y1+4, TOP_INFO_X2, TOP_INFO_Y2);
    bottom_info_bitmap = vga_back.save_area( BOTTOM_INFO_X1, BOTTOM_INFO_Y1, BOTTOM_INFO_X2, BOTTOM_INFO_Y2);
    surplus_info_bitmap = vga_back.save_area( SURPLUS_INFO_X1, SURPLUS_INFO_Y1, SURPLUS_INFO_X2, SURPLUS_INFO_Y2);
    score_info_bitmap = vga_back.save_area( SCORE_INFO_X1, SCORE_INFO_Y1, SCORE_INFO_X2, SCORE_INFO_Y2);

    // ## begin zb 02/21/99
    uscore_info_bitmap = vga_back.save_area( U_SCORE_INFO_X1, U_SCORE_INFO_Y1, U_SCORE_INFO_X2+10, U_SCORE_INFO_Y2);
    // ## end zb 02/21/99

    more_report_bitmap = vga_back.save_area( MORE_REPORT_X1, MORE_REPORT_Y1+2, MORE_REPORT_X2, MORE_REPORT_Y2);
    help_quest_bitmap = vga_back.save_area( HELP_QUEST_X1, HELP_QUEST_Y1+2, HELP_QUEST_X2, HELP_QUEST_Y2);
    list_button_bitmap = vga_back.save_area( LIST_BUTTON_X1, LIST_BUTTON_Y1, LIST_BUTTON_X2, LIST_BUTTON_Y2);
    date_info_bitmap = vga_back.save_area( DATE_INFO_X1, DATE_INFO_Y1, DATE_INFO_X2, DATE_INFO_Y2);
    year_info_bitmap = vga_back.save_area( YEAR_INFO_X1, YEAR_INFO_Y1, YEAR_INFO_X2, YEAR_INFO_Y2);

    //--- save areas of the column bitmap that touch the zoom viewing area ---//

    top_left_column_bitmap = vga_back.save_area( ZOOM_X1, ZOOM_Y1, ZOOM_X1+COLUMN_WIDTH-1, ZOOM_Y1+COLUMN_HEIGHT-1);
    top_right_column_bitmap = vga_back.save_area( ZOOM_X2-COLUMN_WIDTH+1, ZOOM_Y1, ZOOM_X2, ZOOM_Y1+COLUMN_HEIGHT-1);

    bottom_left_column_bitmap = vga_back.save_area( ZOOM_X1, ZOOM_Y2-COLUMN_HEIGHT+1, ZOOM_X1+COLUMN_WIDTH-1, ZOOM_Y2);
    bottom_right_column_bitmap = vga_back.save_area( ZOOM_X2-COLUMN_WIDTH+1, ZOOM_Y2-COLUMN_HEIGHT+1, ZOOM_X2, ZOOM_Y2);

    //------ store frequent used bitmaps into memory -----//

    menu_up_bitmap = vga_back.save_area(MENU_BUTTON_X1, MENU_BUTTON_Y1, MENU_BUTTON_X2, MENU_BUTTON_Y2);
    vga_back.put_bitmap(MENU_BUTTON_X1, MENU_BUTTON_Y1, image_interface.get_ptr("MENUDOWN"));
    menu_down_bitmap = vga_back.save_area(MENU_BUTTON_X1, MENU_BUTTON_Y1, MENU_BUTTON_X2, MENU_BUTTON_Y2);

    short *bitmapPtr = (short *)image_interface.get_ptr("REPORT-0");

    for (int i = 0; i < REPORT_ITEMS+1; i++)
	report_bitmap[i] = mem_add(*(bitmapPtr) * *(bitmapPtr+1) + 2*sizeof(short));

    //	char *strBuf = "REPORT-?";
    char strBuf[100];
    strcpy(strBuf,"REPORT-?");

    for (int i = 0; i < REPORT_ITEMS+1; i++) {          // 0 is the bitmap without all pressed buttons
	strBuf[7] = i + '0';
	bitmapPtr = (short *)image_interface.get_ptr(strBuf);
	memcpy(report_bitmap[i], bitmapPtr, *(bitmapPtr) * *(bitmapPtr+1) + 2);
    }
}

//--------- End of function Info::disp_panel_init ---------//

//-------- Begin of function Info::disp --------//
//!
void Info::disp() {
    if( sys.signal_exit_flag )
	return;

    //-- Display the name of the selected department or the current report. --//

    disp_selected_name();

    //-------- display surplus ---------------//

    vga.use_back();
    vga_back.rest_area( surplus_info_bitmap, 0 );
    font_fdiamond.center_put(SURPLUS_INFO_X1, SURPLUS_INFO_Y1, SURPLUS_INFO_X2, SURPLUS_INFO_Y2, m.format((float)((int)finance.this_year.surplus_deficit),7));
    vga.use_front();

    vga.blt_buf(SURPLUS_INFO_X1, SURPLUS_INFO_Y1, SURPLUS_INFO_X2, SURPLUS_INFO_Y2);

    //-------- display calendar ----------//

    disp_calendar();

    //-------- display game score ------------//

    int    scoreChange = (int)player_school.cur_game_score - (int)player_school.game_score_history[THIS_MONTH-1];
    String str;

    image_interface.put_back( 578, 0, "CUP" );
    vga.blt_buf(578,0,623,31);

    // format type 4 - no thousand separators
    str = m.format( (int) player_school.cur_game_score, 4 );

    if( scoreChange >= 0 ) {
	image_interface.put_back(514, 0, "REPU-UP" );
	vga.blt_buf(514,0,544,29);
    }
    else {
	image_interface.put_back( 508, 0, "REPU-DWN" );
	vga.blt_buf(514,0,544,29);
    }

    /*	if( scoreChange )
	{
	str += " (";

	if( scoreChange > 0 )
	str += "+";
	else
	str += "-";

	str += abs(scoreChange);
	str += ")";
	}
    */
    vga.use_back();
    vga_back.rest_area(score_info_bitmap,0,0);
    //	mouse.hide_area(SCORE_INFO_X1, SCORE_INFO_Y1, SCORE_INFO_X2, SCORE_INFO_Y2);
    font_numbers.put( SCORE_INFO_X1, SCORE_INFO_Y1, str, 1, SCORE_INFO_X2 );
    //	mouse.show_area();
    vga.blt_buf(SCORE_INFO_X1, SCORE_INFO_Y1, SCORE_INFO_X2, SCORE_INFO_Y2);

    // format type 4 - no thousand separators
    str = m.format( (int)player_school.ultimate_game_score, 4 );
    vga_back.rest_area(uscore_info_bitmap,0,0);

    //---- display ultimate score -----//

    if( player_school.ultimate_game_score > 0 )
	font_numbers.put( U_SCORE_INFO_X1, U_SCORE_INFO_Y1, str, 1, U_SCORE_INFO_X2+10 );
    else                                            // font_numbers does not have "(" & ")" characters, we have to use a different font
	font_redcharts.put( U_SCORE_INFO_X1, U_SCORE_INFO_Y1-5, str, 1, U_SCORE_INFO_X2+10 );

    vga.blt_buf(U_SCORE_INFO_X1, U_SCORE_INFO_Y1, U_SCORE_INFO_X2+10, U_SCORE_INFO_Y2);

    //-------- display more report button ---------------//
    vga_back.rest_area(more_report_bitmap,0,0);
    //	mouse.hide_area(MORE_REPORT_X1, MORE_REPORT_Y1, MORE_REPORT_X2, MORE_REPORT_Y2);
    if(sys.view_mode==MODE_MORE_REPORT)
	font_hstone.put( MORE_REPORT_X1, MORE_REPORT_Y1-1, "more reports", 1, MORE_REPORT_X2 );
    else
	font_stone.put( MORE_REPORT_X1, MORE_REPORT_Y1-1, "more reports", 1, MORE_REPORT_X2 );
    //	mouse.show_area();
    vga.blt_buf(MORE_REPORT_X1, MORE_REPORT_Y1-1, MORE_REPORT_X2, MORE_REPORT_Y2);

    //-------- display help quest button ---------------//
    vga_back.rest_area(help_quest_bitmap,0,0);
    //	mouse.hide_area(MORE_REPORT_X1, MORE_REPORT_Y1, MORE_REPORT_X2, MORE_REPORT_Y2);
    if(sys.view_mode==MODE_HELP_TOPIC)
	font_hstone.put( HELP_QUEST_X1, HELP_QUEST_Y1-1, "?", 1, HELP_QUEST_X2 );
    else
	font_stone.put( HELP_QUEST_X1, HELP_QUEST_Y1-1, "?", 1, HELP_QUEST_X2 );
    //	mouse.show_area();
    vga.blt_buf(HELP_QUEST_X1, HELP_QUEST_Y1-1, HELP_QUEST_X2, HELP_QUEST_Y2);

}

//-------- End of function Info::disp --------//

//-------- Begin of function Info::disp_column_bitmap --------//
//!
//! Display areas of the column bitmap that touch the zoom viewing area ---//
//!
void Info::disp_column_bitmap(VgaBuf* vgaBuf) {
    vgaBuf->rest_area( top_left_column_bitmap, 0, 1 );
    vgaBuf->rest_area( top_right_column_bitmap, 0, 1 );

    vgaBuf->rest_area( bottom_left_column_bitmap, 0, 1 );
    vgaBuf->rest_area( bottom_right_column_bitmap, 0, 1 );
}

//-------- End of function Info::disp_column_bitmap --------//

//-------- Begin of function Info::disp_selected_name --------//
//!
//! Display the name of the selected department or the current report.
//!
void Info::disp_selected_name() {
    //------ display the selected department name ------//

    vga_back.rest_area( top_info_bitmap, 0 );

    String deptStr;

    //-- if the report of a non-department building is being displayed currently --//

    if( sys.is_institution_report ) {               // if the report currently displayed is institution-wide
	if( sys.view_mode >= MODE_FIRST_NONDEPARTMENTAL_FIRM && sys.view_mode <= MODE_LAST_NONDEPARTMENTAL_FIRM )
	    deptStr = firm_res[sys.view_mode - MODE_FIRST_NONDEPARTMENTAL_FIRM + FIRST_NONDEPARTMENTAL_FIRM_ID]->name;
	else
	    deptStr = player_school.school_name;
    }
    else if( department_array.selected_recno ) {
	//		deptStr  = department_array.selected_recno;
	//		deptStr += ". ";
	//		deptStr = department_array[department_array.selected_recno]->name();
	deptStr = department_res[department_array[department_array.selected_recno]->department_id]->name;
	deptStr += " Department";
    }

    if( deptStr.len() > 0 ) {
	//------- De-Capitalize the string --------//

	for( char *strPtr = deptStr.str_buf; *strPtr != NULL; strPtr++ ) {
	    if (*strPtr >= 'A' && *strPtr <= 'Z')
		*strPtr += 32;
	    if (*strPtr ==':')
		*strPtr = ':'+2;
	}

	vga.use_back();
	font_stone.center_put(TOP_INFO_X1, TOP_INFO_Y1, TOP_INFO_X2, TOP_INFO_Y2, deptStr);
	vga.use_front();
    }

    vga.blt_buf( TOP_INFO_X1, TOP_INFO_Y1+4, TOP_INFO_X2, TOP_INFO_Y2 );

    //------ display the touched firm name ------//

    vga_back.rest_area( bottom_info_bitmap, 0 );

    if( sys.view_mode == MODE_NORMAL &&             // only display it in the campus view, as it displays the building the mouse is current over
	firm_array.touched_recno &&
	// it was just touched
	sys.frame_count - firm_array.touched_frame_count <= 1 ) {
	String deptStr(firm_array[firm_array.touched_recno]->name());

	//		for (char *strPtr = deptStr.str_buf; *strPtr != NULL; strPtr++)
	//		{
	//  		if (*strPtr >= 'a' && *strPtr <= 'z')
	//    			*strPtr -= 32;							// Capitalize the string
	//		}
	for( char *strPtr = deptStr.str_buf; *strPtr != NULL; strPtr++ ) {
	    if (*strPtr >= 'A' && *strPtr <= 'Z')
		*strPtr += 32;
	    if (*strPtr ==':')
		*strPtr = ':'+2;
	}

	vga.use_back();
	font_stone.center_put(BOTTOM_INFO_X1, BOTTOM_INFO_Y1, BOTTOM_INFO_X2, BOTTOM_INFO_Y2, deptStr);
	vga.use_front();
    }

    vga.blt_buf( BOTTOM_INFO_X1, BOTTOM_INFO_Y1, BOTTOM_INFO_X2, BOTTOM_INFO_Y2 );
}

//-------- End of function Info::disp_selected_name --------//

//-------- Begin of function Info::disp_calendar --------//

void Info::disp_calendar() {
    //-------- display day ---------------//

    int newDay;

    if (last_day != (newDay = date.day(info.game_date))||sys.redraw_all_flag==1) {
	mouse.hide_area(DATE_INFO_X1,DATE_INFO_Y1,DATE_INFO_X2,DATE_INFO_Y2);
	vga_front.rest_area( date_info_bitmap, 0 );
	char digitStr[2];
	digitStr[1] = '\0';
	digitStr[0] = char(newDay % 10) + '0';
	if (newDay > 9) {
	    vga_front.put_bitmap_trans(day_x1_2, digit_y1, date_res_bitmap.get_ptr(digitStr));
	    digitStr[0] = char(newDay / 10) + '0';
	    vga_front.put_bitmap_trans(day_x1_1, digit_y1, date_res_bitmap.get_ptr(digitStr));
	}
	else {
	    vga_front.put_bitmap_trans(day_x1_0, digit_y1, date_res_bitmap.get_ptr(digitStr));
	}
	mouse.show_area();
	last_day = newDay;
    }

    //-------- display year ---------------//

    int newYear;
    if( last_year != (newYear=info.financial_year()) || sys.redraw_all_flag==1 )
	{
	    mouse.hide_area(YEAR_INFO_X1,YEAR_INFO_Y1,YEAR_INFO_X2,YEAR_INFO_Y2);
	    vga_front.rest_area( year_info_bitmap, 0 );
	    char digitStr[2];
	    digitStr[1] = '\0';
	    digitStr[0] = char((newYear % 100) / 10) + '0';
	    vga_front.put_bitmap_trans(year_x1_1, digit_y1, date_res_bitmap.get_ptr(digitStr));
	    digitStr[0] = char(newYear % 10) + '0';
	    vga_front.put_bitmap_trans(year_x1_2, digit_y1, date_res_bitmap.get_ptr(digitStr));
	    mouse.show_area();
	    last_year = newYear;
	}

    //--------- display trimester --------//

    char* bitmapNameArray[] = { "FALL", "WINTER", "SUMMER" };

    mouse.hide_area(YEAR_INFO_X1,YEAR_INFO_Y1,YEAR_INFO_X2,YEAR_INFO_Y2);
    if( player_school.is_trimester_off )            // Trimester break;
	vga_front.put_bitmap(SEASON_X1, SEASON_Y1, date_res_bitmap.get_ptr("BREAK") );
    else
	vga_front.put_bitmap(SEASON_X1, SEASON_Y1, date_res_bitmap.get_ptr(bitmapNameArray[player_school.cur_trimester]) );
    mouse.show_area();

    //-------- display month --------------//

    int newMonth;

    if (last_month != (newMonth = date.month(info.game_date))||sys.redraw_all_flag==1) {
	char *monthPtr;

	switch (last_month = newMonth) {
	case 1:
	    monthPtr = date_res_bitmap.get_ptr("JAN"); break;
	case 2:
	    monthPtr = date_res_bitmap.get_ptr("FEB"); break;
	case 3:
	    monthPtr = date_res_bitmap.get_ptr("MAR"); break;
	case 4:
	    monthPtr = date_res_bitmap.get_ptr("APR"); break;
	case 5:
	    monthPtr = date_res_bitmap.get_ptr("MAY"); break;
	case 6:
	    monthPtr = date_res_bitmap.get_ptr("JUN"); break;
	case 7:
	    monthPtr = date_res_bitmap.get_ptr("JUL"); break;
	case 8:
	    monthPtr = date_res_bitmap.get_ptr("AUG"); break;
	case 9:
	    monthPtr = date_res_bitmap.get_ptr("SEP"); break;
	case 10:
	    monthPtr = date_res_bitmap.get_ptr("OCT"); break;
	case 11:
	    monthPtr = date_res_bitmap.get_ptr("NOV"); break;
	case 12:
	    monthPtr = date_res_bitmap.get_ptr("DEC"); break;
	}

	mouse.hide_area(DATE_INFO_X1,DATE_INFO_Y1,DATE_INFO_X2,DATE_INFO_Y2);
	vga_front.put_bitmap(MONTH_X1, MONTH_Y1, monthPtr);
	mouse.show_area();
    }
}

//-------- End of function Info::disp_calendar --------//

//------ Start of function Info::financial_year -------//

int Info::financial_year() {
    if( game_month<finance.fiscal_year_start_month )
	return game_year-1;
    else
	return game_year;
}

//------ End of function Info::financial_year -------//

//------ Start of function Info::date_str_ymd -------//
//
char* Info::date_str_ymd( int year, int month, int day, int formatFlag) {
    static String str;

    str = "";

    int seperatorFlag = 0;

    // add month
    if( month && !(formatFlag & INFO_DATE_NO_MONTH) ) {
	if( seperatorFlag )
	    str += " ";

	if( formatFlag & INFO_DATE_SHORT_MONTH ) {
	    char shortMonthStr[3+1];
	    m.str_shorten(shortMonthStr, date.month_str(month), 3 );
	    str += shortMonthStr;
	}
	else
	    str += date.month_str(month);

	seperatorFlag = 1;
    }

    // add day
    if( day && !(formatFlag & INFO_DATE_NO_DAY) ) {
	if( seperatorFlag )
	    str += " ";

	str += day;

	seperatorFlag = 1;
    }

    // add year

    if( year && !(formatFlag & INFO_DATE_NO_YEAR) ) {
	// modify year
	if( (formatFlag & INFO_DATE_F_YEAR) && month && month < finance.fiscal_year_start_month )
	    year--;

	if( formatFlag & INFO_DATE_ADD_YR ) {
	    if( seperatorFlag )
		str += " ";
	    str += m.format( year, 16 );
	}
	else {
	    if( seperatorFlag )
		str += ", ";
	    str += m.format(year, 4);                   // no thousand separator
	}

	seperatorFlag = 1;
    }

    return str;
}

//------ End of function Info::date_str_ymd -------//

//------ Start of function Info::date_str_ymd -------//
//
char* Info::date_str_julian( long julianDate, int formatFlag) {
    // false field : INFO_DATE_NO_MONTH, INFO_DATE_NO_YEAR, INFO_DATE_F_YEAR
    // true field : INFO_DATE_ADD_YR
    // true/false acceptable : INFO_DATE_NO_DAY, INFO_DATE_SHORT_MONTH
    // except don't care fields here
    if( formatFlag & (INFO_DATE_NO_MONTH|INFO_DATE_NO_YEAR|INFO_DATE_F_YEAR|INFO_DATE_ADD_YR)
	== INFO_DATE_ADD_YR )                         // true fields here
	return date.date_str( julianDate, formatFlag & INFO_DATE_SHORT_MONTH, !(formatFlag & INFO_DATE_NO_DAY) );
    return date_str_ymd( date.year(julianDate), date.month(julianDate), date.day(julianDate), formatFlag );
}

//------ End of function Info::date_str_ymd -------//

// --- Begin of function Info::update_distance_learning_variables --- //
//
//	Implementation of Version 2.0 Item 8d v)
void  Info::update_distance_learning_variables(char initValue) {
    // calculate distance_learning_place in each course in course array in each department
    int deptSize = department_array.size();
    for( int i=deptSize ; i>0 ; i--) {              //	Loop for all departments
	if ( department_array.is_deleted(i) )
	    continue;

	Department*   deptPtr = department_array[i];
	deptPtr->total_distance_learning_place = 0;
	CourseArray*  courseArrayPtr = &department_array[i]->course_array;
	int courseSize = courseArrayPtr->size();
	for (int j=courseSize; j>0; j--) {            // Loop for courses
	    if ( courseArrayPtr->is_deleted(j) )
		continue;

	    Course* coursePtr = courseArrayPtr->operator [](j);
	    // check this course is optimized
	    int randomNo = m.random(100);
	    if ( randomNo < deptPtr->p_tech_utilization ) {
		// ************** calculate distance_learning_place here **************** //
		// courseEnr[course] : coursePtr->convention_student_count
		// nSections[course] : coursePtr->class_section_count
		// Normal_class_size : coursePtr->normal_class_size()
		// 1.5               : allowance for the 33% overload of the class
		if ( initValue )
		    coursePtr->distance_learning_place = (coursePtr->normal_class_size()*1.25*coursePtr->class_section_count-
							  coursePtr->convention_student_count)*coursePtr->class_section_count*
			deptPtr->distance_learning_credits/100;
		else
		    coursePtr->distance_learning_place = (coursePtr->normal_class_size()*1.5*coursePtr->class_section_count-
							  coursePtr->convention_student_count)*coursePtr->class_section_count*
			deptPtr->distance_learning_credits/100;
	    }
	    else
		coursePtr->distance_learning_place = 0;

	    // calculate the total_distance_learning_place
	    deptPtr->total_distance_learning_place += coursePtr->distance_learning_place;
	}
    }

    // calculate actual_distance_learning_enrollment & course_enrollment & distance_learning_enrollment
    int totalDLProvided = 0;
    for ( int i=deptSize; i>0; i-- ) {                  // Loop again for departments
	if ( department_array.is_deleted(i) )
	    continue;

	Department* deptPtr = department_array[i];
	CourseArray*  courseArrayPtr = &department_array[i]->course_array;
	int courseSize = courseArrayPtr->size();

	// reset the distance learning enrollment
	deptPtr->distance_learning_enrollment = 0;

	for ( int j=courseSize; j>0; j-- ) {          // Loop again for courses
	    if ( courseArrayPtr->is_deleted(j) )
		continue;

	    Course* coursePtr = courseArrayPtr->operator [](j);

	    // calculate actual distance learning enrollment
	    // DL_demand : enroll_res.total_matrics[4]
	    if ( deptPtr->total_distance_learning_place == 0 )
		coursePtr->actual_distance_learning_enrollment = 0;
	    else
		coursePtr->actual_distance_learning_enrollment = enroll_res.total_matrics[4]*coursePtr->distance_learning_place/
		    deptPtr->total_distance_learning_place;

	    // calculate course enrollment
	    coursePtr->convention_student_count += coursePtr->actual_distance_learning_enrollment;

	    // calculate distance learning enrollment
	    deptPtr->distance_learning_enrollment += coursePtr->actual_distance_learning_enrollment;
	}

	// calculate distance credits provided in this department
	deptPtr->distance_credits_provided = deptPtr->distance_learning_enrollment;
	totalDLProvided += deptPtr->distance_credits_provided;
    }

    finance.calc_distance_learning_expenditure();

    // Need to update the value of enroll_res.matric_history[sl][ER_CREDITS_PROVIDED]
    int h = THIS_YEAR;
    shift_history( enroll_res.distance_credits_provided_history, HISTORY_TRIMESTER_COUNT );
    enroll_res.distance_credits_provided_history[THIS_TRIMESTER] = (int)totalDLProvided;

}

//
// --- End of function Info::update_distance_learning_variables --- //
