//Filename    : OGAMGEN2.CPP
//Description : Game Generation - Environment options

#include <OVGA.H>
#include <OSTR.H>
#include <OFONT.H>
#include <OBUTTON.H>
#include <OSPINNER.H>
#include <OPERSON.H>
#include <OWORLD.H>
#include <OEVENT.H>
#include <OTOWN.H>
#include <OGRPAI.H>
#include <OGAME.H>

//--- The united field description width of all fields ---//

#define FLD_DES_WIDTH     226

#define MAX_SEAPORT       10                      // Max no. of seaport of each type : consumer and industrial

//-------- Define static variables ----------//

static ButtonGroup button_local(3), button_character(7), button_capital(5), button_skill(5),
    button_show(2), button_constant(2), button_quality(3),
    button_complex_eco(2), button_town_emerge(2), button_seaport_emerge(2),
    button_stock_enable(2), button_event(4), button_ai_type(2),
    button_random_pref(2), button_change_ai_type(2),
    button_difficulty(7);

static Spinner spinner_town, spinner_competitor,
    spinner_seaport1, spinner_seaport2;

static Spinner spinner_ai_type[TOTAL_AI_TYPE];

static short   diff_rating_x, diff_rating_y;
static char    internal_call=0;

//------- Define static functions ----------//

static void put_yes_no_option(int, int, char*, ButtonGroup&, int);

//--------- Begin of function Game::gen_competitor --------//
//!
//! Game Generator - Environment options
//!
//! <int> x1,y1,x2,y2 = the coordination of the window area
//!
void Game::gen_competitor(int x1, int y1, int x2, int y2) {
    gen_opt_difficulty(x1, y1, x2, y1+70);

    gen_opt_competitor(x1, y1+71, x2, y2);
}

//----------- End of function Game::gen_competitor ---------//

//--------- Begin of function Game::gen_env --------//
//!
//! Game Generator - Environment options
//!
//! <int> x1,y1,x2,y2 = the coordination of the window area
//!
void Game::gen_env(int x1, int y1, int x2, int y2) {
    gen_opt_difficulty(x1, y1, x2, y1+70);

    //--------- City Options ------------//

    gen_opt_city(x1, y1+71, x2, y1+160);

    gen_opt_stock(x1, y1+161, x2, y1+211);

    gen_opt_event(x1, y1+212, x2, y2);
}

//----------- End of function Game::gen_env ---------//

//--------- Begin of function Game::gen_import --------//
//!
//! Game Generator - Import options
//!
//! <int> x1,y1,x2,y2 = the coordination of the window area
//!
void Game::gen_import(int x1, int y1, int x2, int y2) {
    gen_opt_difficulty(x1, y1, x2, y1+70);

    gen_opt_seaport(x1, y1+71, x2, y2);
}

//----------- End of function Game::gen_import ---------//

//..........................................................//

//--------- Begin of function Game::gen_competitor_detect --------//
//!
void Game::gen_competitor_detect() {
    gen_opt_difficulty_detect();

    //--------- Detect buttons ----------//

    if( button_character.detect() >= 0 )
	average_ai_character = button_character.button_pressed;

    if( button_capital.detect() >= 0 )
	competitor_capital = button_capital.button_pressed;

    if( button_skill.detect() >= 0 )
	competitor_skill = button_skill.button_pressed;

    if( button_show.detect() >= 0 )
	show_competitor_info = button_show.button_pressed;

    if( button_random_pref.detect() >= 0 )
	random_person_pref = button_random_pref.button_pressed;

    if( button_ai_type.detect() >= 0 )
	random_ai_type = button_ai_type.button_pressed;

    //-------- Detect Spinner -----------//

    spinner_competitor.detect();

    //--- recalculate the difficulty rating and refresh if changed ---//

    calc_difficulty_rating();
}

//----------- End of function Game::gen_competitor_detect ---------//

//--------- Begin of function Game::gen_env_detect --------//
//!
void Game::gen_env_detect() {
    gen_opt_difficulty_detect();

    //--------- Detect buttons ----------//

    if( button_town_emerge.detect() >= 0 )
	new_town_emerge = button_town_emerge.button_pressed;
    /*
      if( button_complex_eco.detect() >= 0 )
      complex_town_eco = button_complex_eco.button_pressed;
    */
    if( button_local.detect() >= 0 )
	local_competitiveness = button_local.button_pressed;

    if( button_stock_enable.detect() >= 0 )
	stock_enable = button_stock_enable.button_pressed;

    if( button_event.detect() >= 0 )
	random_event_frequency = button_event.button_pressed;

    //-------- Detect Spinner -----------//

    // map_town_array is the no. of custom towns, the max. no. of cities cannot be less than it

    if( world.map_town_array.size() > init_town_num ) {
	init_town_num = world.map_town_array.size();
	spinner_town.refresh();
    }

    spinner_town.set_range( max( 1, world.map_town_array.size()), MAX_TOWN );

    spinner_town.detect();
    spinner_competitor.detect();

    //--- recalculate the difficulty rating and refresh if changed ---//

    calc_difficulty_rating();
}

//----------- End of function Game::gen_env_detect ---------//

//--------- Begin of function Game::gen_import_detect --------//
//!
void Game::gen_import_detect() {
    gen_opt_difficulty_detect();

    //--------- Detect buttons ----------//

    if( button_seaport_emerge.detect() >= 0 )
	new_seaport_emerge = button_seaport_emerge.button_pressed;

    if( button_constant.detect() >= 0 )
	constant_import = button_constant.button_pressed;

    if( button_quality.detect() >= 0 )
	import_quality = button_quality.button_pressed;

    //-------- Detect Spinner -----------//

    spinner_seaport1.detect();
    spinner_seaport2.detect();

    //--- recalculate the difficulty rating and refresh if changed ---//

    calc_difficulty_rating();
}

//----------- End of function Game::gen_import_detect ---------//

//..........................................................//

//--------- Begin of function Game::gen_opt_difficulty --------//
//!
//! Difficulty rating
//!
//! <int> x1,y1,x2,y2 = the coordination of the window area
//!
void Game::gen_opt_difficulty(int x1, int y1, int x2, int y2) {
    if( internal_call )                             // called by gen_opt_difficulty_detect()
	return;

    vga.d3_panel_down(x1, y1, x2, y2, 2, 1, "T_GAMGEN");

    int x=x1+10, y=y1+45;

    font_std.put( x, y+5, "Difficulty Level" );

    x+=115;

    button_difficulty[0].create_text( x    , y, "Very Easy", 0 );
    button_difficulty[1].create_text( x+70 , y, "Easy"     , 0 );
    button_difficulty[2].create_text( x+115, y, "Medium"   , 0 );
    button_difficulty[3].create_text( x+173, y, "Hard"     , 0 );
    button_difficulty[4].create_text( x+218, y, "Very Hard", 0 );
    button_difficulty[5].create_text( x+288, y, "Hardest"  , 0 );
    button_difficulty[6].create_text( x+346, y, "Custom"   , 0 );

    button_difficulty.paint(difficulty_level);

    font_mid.d3_put( x1+150, y1+8, x1+330, y1+38, "Difficulty Rating" );

    diff_rating_x=x1+336, diff_rating_y=y1+13;      // for calc_difficulty_rating()

    font_mid.put( diff_rating_x, diff_rating_y, m.format(difficulty_rating, 3) );
}

//----------- End of function Game::gen_opt_difficulty ---------//

//--------- Begin of function Game::gen_opt_difficulty_detect --------//
//!
void Game::gen_opt_difficulty_detect() {
    if( button_difficulty.detect() >= 0 ) {
	set_difficulty_level(button_difficulty.button_pressed);

	internal_call=1;                              // don't refresh in gen_opt_difficulty()
	gen_disp(INFO_UPDATE);
	internal_call=0;

	vga.tile( diff_rating_x, diff_rating_y, diff_rating_x+100, diff_rating_y+28, "T_GAMGEN" );

	font_mid.put( diff_rating_x, diff_rating_y, m.format(difficulty_rating, 3) );
    }
}

//----------- End of function Game::gen_opt_difficulty_detect ---------//

//------ Begin of function Game::set_difficulty_level -----//
//!
void Game::set_difficulty_level(int diffLevel) {
    difficulty_level = diffLevel;

    //----------- Very Easy -------------//

    if( diffLevel==0 ) {
	//---- competitors options -----//

	competitor_num        = 3;
	average_ai_character  = PERSON_CONSERATIVE;
	competitor_capital    = OPTION_LOW;
	competitor_skill      = AI_SKILL_NONE;
	show_competitor_info  = 1;

	//------ seaport options -------//

	consumer_seaport_num   = 2;
	industrial_seaport_num = 2;
	constant_import        = 1;
	import_quality         = OPTION_HIGH;

	//-------- City options --------//

	local_competitiveness= OPTION_LOW;
	complex_town_eco     = 0;

	//------- Random events --------//

	random_event_frequency = RANDOM_EVENT_FREQ_NONE;
    }

    //-------------- Easy ---------------//

    if( diffLevel==1 ) {
	//---- competitors options -----//

	competitor_num        = 5;
	average_ai_character  = AI_CHARACTER_PREDEFINED;
	competitor_capital    = OPTION_LOW;
	competitor_skill      = AI_SKILL_NONE;
	show_competitor_info  = 1;

	//------ seaport options -------//

	consumer_seaport_num   = 1;
	industrial_seaport_num = 2;
	constant_import        = 1;
	import_quality         = OPTION_HIGH;

	//-------- City options --------//

	local_competitiveness= OPTION_MODERATE;
	complex_town_eco     = 0;

	//------- Random events --------//

	random_event_frequency = RANDOM_EVENT_FREQ_NONE;
    }

    //------------ Medium ------------//

    if( diffLevel==2 ) {
	//---- competitors options -----//

	competitor_num        = 7;
	average_ai_character  = AI_CHARACTER_PREDEFINED;
	competitor_capital    = OPTION_MODERATE;
	competitor_skill      = AI_SKILL_NONE;
	show_competitor_info  = 1;

	//------ seaport options -------//

	consumer_seaport_num   = 1;
	industrial_seaport_num = 1;
	constant_import        = 1;
	import_quality         = OPTION_MODERATE;

	//-------- City options --------//

	local_competitiveness= OPTION_MODERATE;
	complex_town_eco     = 0;

	//------- Random events --------//

	random_event_frequency = RANDOM_EVENT_FREQ_NONE;
    }

    //------------ Hard ------------//

    if( diffLevel==3 ) {
	//---- competitors options -----//

	competitor_num        = 8;
	average_ai_character  = AI_CHARACTER_PREDEFINED;
	competitor_capital    = OPTION_HIGH;
	competitor_skill      = AI_SKILL_LOW;
	show_competitor_info  = 0;

	//------ seaport options -------//

	consumer_seaport_num   = 1;
	industrial_seaport_num = 1;
	constant_import        = 1;
	import_quality         = OPTION_LOW;

	//-------- City options --------//

	local_competitiveness = OPTION_MODERATE;
	complex_town_eco     = 0;

	//------- Random events --------//

	random_event_frequency = RANDOM_EVENT_FREQ_LOW;
    }

    //--------- Very Hard ------------//

    if( diffLevel==4 ) {
	//---- competitors options -----//

	competitor_num        = 8;
	average_ai_character  = PERSON_VERY_AGGRESSIVE;
	competitor_capital    = OPTION_VERY_HIGH;
	competitor_skill      = AI_SKILL_PREDEFINED;
	show_competitor_info  = 0;

	//------ seaport options -------//

	consumer_seaport_num   = 1;
	industrial_seaport_num = 1;
	constant_import        = 0;
	import_quality         = OPTION_LOW;

	//-------- City options --------//

	local_competitiveness= OPTION_HIGH;
	complex_town_eco     = 1;

	//------- Random events --------//

	random_event_frequency = RANDOM_EVENT_FREQ_MODERATE;
    }

    //--------- Hardest ------------//

    if( diffLevel==5 ) {
	//---- competitors options -----//

	competitor_num        = 10;
	average_ai_character  = PERSON_VERY_AGGRESSIVE;
	competitor_capital    = OPTION_EXTREMELY_HIGH;
	competitor_skill      = AI_SKILL_HIGH;
	show_competitor_info  = 0;

	//------ seaport options -------//

	consumer_seaport_num   = 0;
	industrial_seaport_num = 0;
	constant_import        = 0;
	import_quality         = OPTION_LOW;

	//-------- City options --------//

	local_competitiveness= OPTION_HIGH;
	complex_town_eco     = 1;

	//------- Random events --------//

	random_event_frequency = RANDOM_EVENT_FREQ_HIGH;
    }

    //------- calculate difficulty rating --------//

    calc_difficulty_rating(0);                      // don't refresh
}

//------- End of function Game::set_difficulty_level ------//

//--------- Begin of function Game::calc_difficulty_rating --------//
//!
//! Difficulty rating =
//!
//! base rating                       15%
//! average_ai_character     x 5   (0-15%)  (4 levels)
//! competitor setup capital x 11  (0-44%)  (5 levels)
//! competitor skill         x 20  (0-60%)  (4 levels)
//! show competitor info     x 15  (0-15%)  (on/off)
//!-----------------------------------------------------
//! competitiveness rating           149%
//!
//! competitor rating = competitiveness rating *
//!                     number of competitors (0-10) / 10
//!
//!-----------------------------------------------------------------//
//!
//! import rating (0-30%) =
//!
//! if no. of seaport==0       30  (0-30%)
//!
//! if no. of seaport>0
//!    base rating                    10%
//!    constant import       x 10  (0-10%)  (on/off)
//!    import quality        x 5   (0-10%)  (3 levels)
//!    ------------------------------------------------
//!    final rating = seaport rating x (19-no. of seaports) / 19
//!
//!-----------------------------------------------------------------//
//!
//! base rating                        1%
//! competitor rating              (0-149%)
//! import rating                  (0-30%)
//! local competitiveness    x 10  (0-20%)  (3 levels)
//! random events                  (0-12%)
//!
//!-----------------------------------------------------------------//
//!                                   212%
//!
//!-----------------------------------------------------------------//
//!
//! [int] refreshFlag  - whether it should update the screen if the
//!                      difficulty rating has been changed.
//!                      (default:1)
//!
void Game::calc_difficulty_rating(int refreshFlag) {
    int oldRating = difficulty_rating;

    //-------- calc competitiveness rating ---------//

    int competitivenessRating = 15 + competitor_capital * 11 +
	!show_competitor_info * 15;

    if( average_ai_character < PERSON_CHARACTER_TYPE )
	competitivenessRating += average_ai_character * 5;
    else
	competitivenessRating += 7;

    if( competitor_skill == AI_SKILL_PREDEFINED )
	competitivenessRating += 40;

    else if( competitor_skill >= AI_SKILL_LOW )
	competitivenessRating += (competitor_skill-AI_SKILL_LOW+1)*20;

    //-------------- calc import rating ------------//

    int importRating;
    int seaportNum=consumer_seaport_num + industrial_seaport_num;

    if( seaportNum > 0 ) {
	importRating = 10 + !constant_import * 10 + (2-import_quality) * 5;
	// minimum one industrial seaport
	importRating = importRating * (20-seaportNum) / 20;
    }
    else
	importRating = 30;

    difficulty_rating = 1 + competitivenessRating * competitor_num / 10 +
	importRating + local_competitiveness * 10 +
	random_event_frequency * 4;

    // translate the number from a maximum of 242 to a maximum of 200
    difficulty_rating = difficulty_rating * 200 / 212;

    difficulty_rating = max( 1, difficulty_rating );

    //--- if the player overrides the settings, set it to "Custom" difficulty ---//

    if( refreshFlag && difficulty_rating != oldRating ) {
	difficulty_level = LEVEL_CUSTOM;              // custom difficulty level

	button_difficulty.push(difficulty_level);

	vga.tile( diff_rating_x, diff_rating_y, diff_rating_x+100, diff_rating_y+28, "T_GAMGEN" );

	font_mid.put( diff_rating_x, diff_rating_y, m.format(difficulty_rating, 3) );
    }
}

//----------- End of function Game::calc_difficulty_rating ---------//

//--------- Begin of function Game::gen_opt_competitor --------//
//!
//! AI Competitors options
//!
void Game::gen_opt_competitor(int x1, int y1, int x2, int y2) {
    vga.d3_panel_down(x1, y1, x2, y2, 2, 1, "T_GAMGEN");

    font_std.d3_put( x1+2, y1+2, x2-2, y1+18, "Competitors" );

    //------- No. of Competitors in the Game -------//

    int x=x1+4;
    int xm=x+FLD_DES_WIDTH+3;
    int y=y1+24;

    // -1 is GroupMisc
    int maxAiGroup = min( MAX_GROUP-expected_player_num-1, MAX_AI_GROUP );

    if( !multiplayer_game )
	maxAiGroup = MAX_AI_GROUP;

    // -2 is the Government Group and Player Group
    spinner_competitor.create( x, y, "No. of Competitors", x+FLD_DES_WIDTH, &competitor_num, 1, 0, maxAiGroup );

    y+=19;
    vga.separator( x1, y, x2, y+1 );
    y+=6;

    //------- Average Competitor Character ---------//

    font_san.put_field( x, y, "Competitor Character", x+FLD_DES_WIDTH, "" );

    button_character[0].create_text( xm    , y-2 , "Conservative"   , 0 );
    button_character[1].create_text( xm+72 , y-2 , "Moderate"       , 0 );
    button_character[2].create_text( xm+132, y-2 , "Aggressive"     , 0 );
    button_character[3].create_text( xm+200, y-2 , "Very Aggressive", 0 );

    y+=19;

    button_character[4].create_text( xm    , y-2 , "Pre-defined"    , 0 );
    button_character[5].create_text( xm+72 , y-2 , "Random"         , 0 );
    button_character[6].create_text( xm+132, y-2 , "Even"           , 0 );

    button_character.paint(average_ai_character);

    y+=21;

    //--------- Competitors Capital --------//

    font_san.put_field( x, y, "Competitor Capital", x+FLD_DES_WIDTH, "" );

    button_capital[0].create_text( xm    , y-2, "Low"      , 0 );
    button_capital[1].create_text( xm+37 , y-2, "Moderate" , 0 );
    button_capital[2].create_text( xm+97 , y-2, "High"     , 0 );
    button_capital[3].create_text( xm+137, y-2, "Very High", 0 );
    button_capital[4].create_text( xm+200, y-2, "Extremely High", 0 );
    button_capital.paint(competitor_capital);

    y+=21;

    //--------- Competitor Expertise ----------//

    font_san.put_field( x, y, "Competitor Expertise Level", x+FLD_DES_WIDTH, "" );

    button_skill[0].create_text( xm    , y-2, "None"       , 0 );
    button_skill[1].create_text( xm+42 , y-2, "Pre-defined", 0 );
    button_skill[2].create_text( xm+110, y-2, "Low"        , 0 );
    button_skill[3].create_text( xm+147, y-2, "Moderate"   , 0 );
    button_skill[4].create_text( xm+208, y-2, "High"       , 0 );
    button_skill.paint(competitor_skill);

    y+=21;

    //----------- Show Competitor Information ----------//

    put_yes_no_option( x, y, "Show Competitor Trade Secrets", button_show, show_competitor_info );

    y+=21;

    //----------- Random Competitor Personality -----------//

    put_yes_no_option( x, y, "Random Competitor Personality", button_random_pref, random_person_pref );

    y+=21;

    //----------- Random Competitor Strategy -----------//

    put_yes_no_option( x, y, "Random Competitor Strategy", button_ai_type, random_ai_type );

    y+=21;
}

//----------- End of function Game::gen_opt_competitor ---------//

//--------- Begin of function Game::gen_ai_type --------//
//!
//! AI Competitor Types
//!
void Game::gen_ai_type(int x1, int y1, int x2, int y2) {
    vga.d3_panel_down(x1, y1, x2, y2, 2, 1, "T_GAMGEN");

    font_std.d3_put( x1+2, y1+2, x2-2, y1+18, "Competitor Strategies" );

    //------ No. of Cities in the beginning -------//

    int    i, x=x1+4, y=y1+24;
    String str;

    for( i=0 ; i<TOTAL_AI_TYPE ; i++, y+=15 ) {
	str  = "Max No. of ";
	str += GroupAI::ai_type_str_array[i];
	str += " Competitors";

	spinner_ai_type[i].create( x, y, str, x+246, ai_type_num_array+i, 1, 0, MAX_AI_GROUP );
    }

    //----------- Allow change AI type ---------------//

    y+=6;

    put_yes_no_option( x, y, "Enable Competitors Change Strategies", button_change_ai_type, allow_change_ai_type );

    y+=19;
    vga.separator( x1, y, x2, y+1 );
}

//----------- End of function Game::gen_ai_type ---------//

//--------- Begin of function Game::gen_ai_type_detect --------//
//!
void Game::gen_ai_type_detect() {
    //-------- AI type spinners ---------//

    if( game_design_mode ) {
	int i;

	for( i=0 ; i<TOTAL_AI_TYPE ; i++ )
	    spinner_ai_type[i].detect();

	if( button_change_ai_type.detect() >= 0 )
	    allow_change_ai_type = button_change_ai_type.button_pressed;
    }
}

//----------- End of function Game::gen_ai_type_detect ---------//

//...........................................................//

//--------- Begin of function Game::gen_opt_city --------//
//!
//! City options
//!
void Game::gen_opt_city(int x1, int y1, int x2, int y2) {
    vga.d3_panel_down(x1, y1, x2, y2, 2, 1, "T_GAMGEN");

    font_std.d3_put( x1+2, y1+2, x2-2, y1+18, "Cities" );

    //------ No. of Cities in the beginning -------//

    int x=x1+4;
    int y=y1+24;

    // map_town_array is the no. of custom towns, the max. no. of cities cannot be less than it
    spinner_town.create( x, y, "No. of Cities at the Beginning", x+FLD_DES_WIDTH, &init_town_num, 1, max(1,world.map_town_array.size()), MAX_TOWN );

    //----------- New Cities Emerge During the Game ----------//

    y+=21;
    put_yes_no_option( x, y, "New Cities Emerge During the Game", button_town_emerge, new_town_emerge );
    /*
      //------------ Complex City Economy ----------//

      y+=21;
      put_yes_no_option( x, y, "Complex City Economy", button_complex_eco, complex_town_eco );
    */
    //--------- Local Competitors Strength --------//

    y+=21;
    font_san.put_field( x, y, "Competence of Local Competitors", x+FLD_DES_WIDTH, "" );

    x+=FLD_DES_WIDTH+3;
    button_local[0].create_text( x   , y-2, "Low"     , 0 );
    button_local[1].create_text( x+37, y-2, "Moderate", 0 );
    button_local[2].create_text( x+97, y-2, "High"    , 0 );
    button_local.paint(local_competitiveness);
}

//----------- End of function Game::gen_opt_city ---------//

//--------- Begin of function Game::gen_opt_seaport --------//
//!
//! Seaport options
//!
void Game::gen_opt_seaport(int x1, int y1, int x2, int y2) {
    vga.d3_panel_down(x1, y1, x2, y2, 2, 1, "T_GAMGEN");

    font_std.d3_put( x1+2, y1+2, x2-2, y1+18, "Imports" );

    //------- No. of Seaport Importing Consumer Goods -------//

    int x=x1+4;
    int y=y1+24;

    spinner_seaport1.create( x, y, "No. of Seaports Importing Consumer Goods", x+FLD_DES_WIDTH, &consumer_seaport_num, 1, 0, MAX_SEAPORT );

    //------- No. of Seaport Importing Industrial Goods -------//

    y+=21;

    // minimum one industrial seaport
    spinner_seaport2.create( x, y, "No. of Seaports Importing Industrial Goods", x+FLD_DES_WIDTH, &industrial_seaport_num, 1, 0, MAX_SEAPORT );

    //----------- Seaport Emerge ----------//

    y+=21;

    put_yes_no_option( x, y, "New Seaports Emerge During the Game", button_seaport_emerge, new_seaport_emerge );

    //----------- Dynamic seaport ----------//

    y+=21;

    put_yes_no_option( x, y, "Constant Import Supply", button_constant, constant_import );

    //----------- Quality of Imported Products ----------//

    y+=21;

    font_san.put_field( x, y, "Import Quality", x+FLD_DES_WIDTH, "" );

    x+=FLD_DES_WIDTH+3;
    button_quality[0].create_text( x   , y-2, "Low"     , 0 );
    button_quality[1].create_text( x+37, y-2, "Moderate", 0 );
    button_quality[2].create_text( x+97, y-2, "High"    , 0 );
    button_quality.paint(import_quality);

    y+=19;
    vga.separator( x1, y, x2, y+1 );
}

//----------- End of function Game::gen_opt_seaport ---------//

//--------- Begin of function Game::gen_opt_stock --------//
//!
//! Stock Market Options
//!
void Game::gen_opt_stock(int x1, int y1, int x2, int y2) {
    vga.d3_panel_down(x1, y1, x2, y2, 2, 1, "T_GAMGEN");

    font_std.d3_put( x1+2, y1+2, x2-2, y1+18, "Stock Market" );

    //------ Whether the game has stock market or not -------//

    int x=x1+4;
    int y=y1+24;

    //ERD
    if(!multiplayer_game)                           // don't allow stock investment if multiplayer enabled.
	put_yes_no_option( x, y, "Allow Stock Investment", button_stock_enable, stock_enable );
    else
	font_san.put( x, y, "The stock market is disabled in multiplayer games." );
    //ERD
}

//----------- End of function Game::gen_opt_stock ---------//

//--------- Begin of function Game::gen_opt_event --------//
//!
//! Disaster Option
//!
void Game::gen_opt_event(int x1, int y1, int x2, int y2) {
    vga.d3_panel_down(x1, y1, x2, y2, 2, 1, "T_GAMGEN");

    font_std.d3_put( x1+2, y1+2, x2-2, y1+18, "Random Events" );

    //------ Whether the game has disaster or not -------//

    int x=x1+4;
    int y=y1+24;
    int xm=x+FLD_DES_WIDTH+3;

    font_san.put_field( x, y, "Random Event Frequency", x+FLD_DES_WIDTH, "" );

    button_event[0].create_text( xm    , y-2, "None"     , 0 );
    button_event[1].create_text( xm+40 , y-2, "Low"      , 0 );
    button_event[2].create_text( xm+77 , y-2, "Moderate" , 0 );
    button_event[3].create_text( xm+137, y-2, "High"     , 0 );
    button_event.paint(random_event_frequency);
}

//----------- End of function Game::gen_opt_event ---------//

//........................................................//

//------- Begin of static function put_yes_no_option -------//

static void put_yes_no_option(int x, int y, char* optionDes,
			      ButtonGroup& buttonGroup, int defaultButton) {
    font_san.put_field( x, y, optionDes, x+FLD_DES_WIDTH, "" );

    x+=FLD_DES_WIDTH+3;
    buttonGroup[1].create_text( x    , y-2, "Yes", 0 );
    buttonGroup[0].create_text( x+35 , y-2, "No" , 0 );
    buttonGroup.paint(defaultButton);
}

//-------- End of static function put_yes_no_option -------//
