#include <OSYS.H>
#include <OCONFIG.H>
#include <OFINSCOR.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OMOUSE2.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <OBUTTON.H>
#include <OPSCHOOL.H>
#include <OGFILE.H>

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-55
};

enum {
    BUTTON_X0 = REPORT_X1+100,
    BOTTOM_Y1 = REPORT_Y2+25,
};

enum {
    BOTTOM_BUTTON_WIDTH = 80,
    BOTTOM_BUTTON_HEIGHT = 25,
    BOTTOM_BUTTON_DISTANCE = 10,
};

enum {                                            // the bg image id
    BG_PIC_ID = 7
};

static char final_score_report_init_flag;
//------- Begin of function FinalScoreReport::FinalScoreReport -----------//
//!
FinalScoreReport::FinalScoreReport() {
    init();
}

//--------- End of function FinalScoreReport::FinalScoreReport -----------//

//------- Begin of function FinalScoreReport::~FinalScoreReport -----------//
//!
FinalScoreReport::~FinalScoreReport() {
    deinit();
}

//--------- End of function FinalScoreReport::~FinalScoreReport -----------//

//------- Begin of function FinalScoreReport::deinit ---------------//
//!
void FinalScoreReport::deinit() {
    final_score_report_init_flag = 0;
}

//--------- End of function FinalScoreReport::deinit ---------------//

//------- Begin of function FinalScoreReport::init -----------------//
//!
void FinalScoreReport::init() {
    final_score_report_init_flag = 0;
}

//--------- End of function FinalScoreReport::init -----------------//

//---------- Begin of function FinalScoreReport::eval_letter_report ------------//
//! You can set the value of FinalScoreReport::topic_id before running this functions
void FinalScoreReport::final_score_report(int refreshFlag) {
    int tab1=REPORT_X1+100,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+250,
	vtab3=REPORT_Y1+75,vtab4=REPORT_Y1+350;
    vga.use_back();

    sys.is_institution_report=1;
    if (refreshFlag == INFO_REPAINT) {
	if(final_score_report_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!final_score_report_init_flag) {
	    user_interface.bg_img(BG_PIC_ID, &vga_back);
	    info.disp_column_bitmap(&vga_back);
	    final_score_report_init_flag=1;
	    font_charts.put(tab2,vtab1+30,"Final Score");
	    font_charts.put(tab2,vtab1+70,"=    (Current Trustee Evaluation - Initial Trustee Evaluation) X 10 " );
	    font_charts.put(tab2,vtab1+110,"    + Number Of Gaming Years + Goal Bonus" );

	    String formula;
	    formula = "=    (";
	    formula += (long)player_school.cur_game_score;
	    formula += " - ";
	    formula += (long)player_school.init_game_score;
	    formula += ") X 10 + ";
	    formula += info.year_passed-1;
	    formula += " + ";
	    formula += (player_school.bonus_point + (int)player_school.threshold_event_bonus);
	    font_charts.put(tab2,vtab1+150,formula.str_buf);
	    formula = "=    ";
	    formula += player_school.ultimate_game_score;
	    font_charts.put(tab2,vtab1+190,formula.str_buf);

	    if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		image_interface.put_back( tab1+400,vtab2,"PCOPPER" );
	    if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		image_interface.put_back( tab1+200,vtab2,"PSILVER" );
	    if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		image_interface.put_back( tab1,vtab2,"PGOLD" );

	}
    }
    //------- blt the back buffer to front buffer -------//
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//---------- End of function Development::eval_letter_report ------------//

////////////////////////////////////////////////////////////////////////////////////////////

int FinalScoreReport::final_score_detect() {
    int tab1=REPORT_X1,tab2=REPORT_X1+100,
	tab3=REPORT_X1+200,tab4=REPORT_X1+600;
    int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    if (!final_score_report_init_flag)
	return 0;
    if (mouse.any_click(2) ) {
	if(player_school.scen_quit_game_flag == 2) {
	    player_school.scen_quit_game_flag = 0;
	    return 0;
	}
	else {
	    // may call hall_of_fame screen
	    game_file_array.add_hall_of_fame(player_school.ultimate_game_score);

	    sys.signal_exit_flag = 2;
	    sys.menu_pull_down = 0;
	    sys.set_staying_view_mode(0);
	    sys.set_speed(DEFAULT_SPEED);
	}
    }
    return 0;
}
