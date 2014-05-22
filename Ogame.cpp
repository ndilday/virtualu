//Filename    : OGAME.CPP
//Description : Main Game Object

#include <OLOG.H>
#include <OSYS.H>
#include <OMUSIC.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OFIRMRES.H>
#include <OCOURSE.H>
#include <OCOURRES.H>
#include <OSCHOOL.H>
#include <ONAMERES.H>
#include <OFIELD.H>
#include <OPOWER.H>
#include <OWORLD.H>
#include <OBATTLE.H>
#include <OINFO.H>
#include <OAUDIO.H>
#include <ODEPTRES.H>
#include <ODEPT.H>
#include <OPSCHOOL.H>
#include <OSCHLRES.H>
#include <OFACURES.H>
#include <OENROLL.H>
#include <OFPHOTO.H>
#include <OFINANCE.H>
#include <OLETTER.H>                              // chwg1026
#include <OFIRM.H>
#include <OTERRAIN.H>
#include <OPLANT.H>
#include <OROAD.H>
#include <OGAMESET.H>
#include <OGAME.H>
//## fred 990511 #include <OSPRTRES.H>
//## fred 990511 #include <OSPRITEA.H>
#include <OFACILIT.H>                             // fred 980808
#include <ONEWS.H>                                // fred 981013
#include <OTASK.H>                                // fred 981014
#include <OCHANCE.H>                              // fred 981014
#include <OLOSEGAM.H>

//-------- Begin of function Game::Game --------//
//!
Game::Game() {
    init_flag = 0;
    game_mode = GAME_PREGAME;
}

//--------- End of function Game::Game ---------//

//-------- Begin of function Game::init --------//
//!
//! Note: all functions called in this function cannot call
//!			m.random(). Otherwise, it will cause random seed
//!		   sync error.
//!
//! [int] loadGameCall - weather this function is called
//!                      when a game is being loaded
//!                      (default: 0)
//!
int Game::init(int loadGameCall) {
    if( init_flag )
	deinit();

    DEBUG_LOG("Game::init called");

    int originalRandomSeed = m.get_random_seed();

    //	music.stop();

    // ----- set waiting cursor -------- //

    int oldCursor = mouse_cursor.get_icon();
    mouse_cursor.set_icon(CURSOR_WAITING);

    //--------- open game set ---------//
    game_set.open_set(1);                           // open the default game set

    //------- init game resource class ------//

    terrain_res.init();
    plant_res.init();
    road_res.init();

    school_res.init();
    field_res.init();
    department_res.init();
    firm_res.init();                                // inited in obattle.cpp
    //## fred 990511	sprite_res.init();			// fred 980608
    faculty_res.init();
    faculty_photo_res.init();
    name_res.init();
    letter.init();                                  // chw1026
    course_res.init();
    enroll_res.init();                              // fred 980702; must run after school_res.init()

    // modify parameters
    // light-speed
    if( player_school.behavioral_response_speed == INPUT_HIGH ) {
	// modify HE.GDB.Init.D4 to boost faculty promotion speed and reduce career length

	for( int deptId = 1; deptId <= department_res.department_count; ++deptId ) {
	    DepartmentInfo *deptInfo = department_res[deptId];
	    for( int probSchoolType = 0; probSchoolType < PROB_SCHOOL_TYPE_COUNT; ++probSchoolType ) {
		// B85, Is associate_professor_promotion_probability right field?
		deptInfo->associate_professor_promotion_probability[probSchoolType] *= 1.5f;
		// B86, Is assistant_professor_promotion_probability right field?
		deptInfo->assistant_professor_promotion_probability[probSchoolType] *= 1.5f;

		// C85:C89
		for( int rankLevel = 0; rankLevel < FACULTY_RANK_LEVEL_COUNT; ++rankLevel ) {
		    deptInfo->faculty_departure_probability[rankLevel][probSchoolType] *= 2.0f;
		}
	    }
	}

    }

    //------- init game data class ---------//

    firm_array.init();
    //## fred 990511	sprite_array.init();
    department_array.init();

    //------ init game surface class -------//

    finance.init();
    facility_office.init();                         // fred 980808
    power.init();
    world.init();
    battle.init();
    news_array.init();                              //## fred 981013
    task_array.init();                              //## fred 981013
    chance_event.init();                            //## fred 981013
    lose_game.init();

    if( !loadGameCall )
	info.init();                                  // it reads in the panel texture and copy it to vga_back's buffer

    //----- restore from waiting cursor --------//

    mouse_cursor.restore_icon(oldCursor);

    game_has_ended = 0;

    init_flag=1;

    return TRUE;
}

//--------- End of function Game::init ---------//

//-------- Begin of function Game::deinit --------//
//!
//! [int] loadGameCall - weather this function is called
//!                      when a game is being loaded
//!                      (default: 0)
//!
void Game::deinit(int loadGameCall) {
    if( !init_flag )
	return;

    DEBUG_LOG("Game::deinit called");

    power.disable();                                // disable power, which handle mouse inputs

    music.stop();
    audio.stop_wav();

    //----- set waiting cursor -------- //

    int oldCursor = mouse_cursor.get_icon();
    mouse_cursor.set_icon(CURSOR_WAITING);

    //------- deinit game data class ---------//

    //## fred 990511	sprite_array.deinit();
    firm_array.deinit();
    department_array.deinit();

    //------ deinit game surface class -------//

    finance.deinit();
    facility_office.deinit();                       // fred 980808
    power.deinit();
    world.deinit();
    battle.deinit();
    news_array.deinit();                            //## fred 981013
    task_array.deinit();                            //## fred 981013
    chance_event.deinit();                          //## fred 981013
    lose_game.deinit();

    if( !loadGameCall )
	info.deinit();

    //------- deinit game resource class ------//

    terrain_res.deinit();
    plant_res.deinit();
    road_res.deinit();

    //## fred 990511	sprite_res.deinit();			// fred 980608
    school_res.deinit();
    field_res.deinit();
    department_res.deinit();
    firm_res.deinit();
    faculty_res.deinit();
    faculty_photo_res.deinit();
    name_res.deinit();
    course_res.deinit();
    enroll_res.deinit();                            // fred 980702

    player_school.deinit();

    game_set.close_set();                           // close the game set

    //----- restore from waiting cursor -------- //

    mouse_cursor.restore_icon(oldCursor);

    init_flag=0;
}

//--------- End of function Game::deinit ---------//
