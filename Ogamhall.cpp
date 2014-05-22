//Filename    : OGAMHALL.CPP
//Description : Hall of Fame

#include <OVGA.H>
#include <OVGALOCK.H>
#include <ODATE.H>
#include <OSTR.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OIMGRES.H>
#include <OGAME.H>
#include <OGFILE.H>

//------ Begin of function GameFileArray::disp_hall_of_fame -----//
//!
//! Display the Hall of Fame
/*
  void GameFileArray::disp_hall_of_fame()
  {
  vga.disp_image_file("HALLFAME");

  //---------- display hall of fame records ------------//

  int i;
  int x=120, y=116;

  for( i=0 ; i<HALL_FAME_NUM ; i++, y+=76 )
  {
  hall_fame_array[i].disp_info( x, y, i+1 );
  }

  mouse.wait_press(60);		// 60 seconds to time out

  vga.finish_disp_image_file();
  }
*/
//------- End of function GameFileArray::disp_hall_of_fame -----//

//------ Begin of function HallFame::disp_info -------//
//!
//! Display a Hall of Fame record
//!
//! <int> x, y = the location of the information
//! <int> pos  = the position of the record.
//!
void HallFame::disp_info(int x, int y, int pos) {
    if( !start_year )                               // no information
	return;

    //------------------------------------------------------//
    //
    // e.g. 1. [Image] King T Chan
    //    	  [     ] Score : 150    Population : 1000    Period : 1001-1030
    //
    //------------------------------------------------------//

    Font* fontPtr;

#if( defined(GERMAN) || defined(FRENCH) || defined(SPANISH) )
    fontPtr = &font_hall;
#else
    fontPtr = &font_std;
#endif

    String str;
    int    y2 = y+17;

    //----------------------------------------//

    str  = pos;
    str += ".";

    fontPtr->put( x, y, str );

    x += 16;

    //----------------------------------------//

    str  = translate.process("King ");
    str += player_first_name;
    str += player_last_name;

    fontPtr->put( x, y, str );

    //----------------------------------------//

    str  = translate.process("Score : ");
    str += score;

    fontPtr->put( x, y2, str );

    //----------------------------------------//

    //str  = translate.process("Population : ");
    //str += population;

    //fontPtr->put( x+110, y2, str );

    //----------------------------------------//

#if( defined(GERMAN) || defined(FRENCH) || defined(SPANISH) )
    x-=10;                                          // position adjustment for the German version
#endif

    str  = translate.process("Period : ");
    str += m.num_to_str(start_year);                // without adding comma separators
    str += "-";
    str += m.num_to_str(end_year);

    fontPtr->put( x+260, y2, str );

    //----------------------------------------//

    //str  = translate.process("Difficulty : ");
    //str += difficulty_rating;

    fontPtr->put( x+420, y2, str );
}

//------- End of function HallFame::disp_info -------//

//--------- Begin of function HallFame::record_data --------//
//!
//! Record the hall of fame record_data
//!
void HallFame::record_data(int totalScore) {

    strncpy( school_name, player_school.school_name, PlayerSchool::SCHOOL_NAME_LEN);
    school_name[PlayerSchool::SCHOOL_NAME_LEN] = NULL;

    strncpy( player_first_name, player_school.player_first_name, PlayerSchool::FIRST_NAME_LEN );
    player_first_name[PlayerSchool::FIRST_NAME_LEN] = NULL;

    strncpy( player_last_name, player_school.player_last_name, PlayerSchool::LAST_NAME_LEN );
    player_last_name[PlayerSchool::LAST_NAME_LEN] = NULL;

    start_year = date.year(info.game_start_date);
    end_year   = info.game_year;

    score     = totalScore;                         // ultimate_game_score
    trust_score = (int) player_school.cur_game_score;

    scenario_id = player_school.scenario_id;
}

//----------- End of function HallFame::record_data ---------//

//------ Begin of function GameFileArray::add_hall_of_fame -----//
//!
//! Add current game into the hall of hame
//!
//! "save" hall of fame when retire, scenario ends (all win or all lose)
//!
//! <int> totalScore of the player.
//!
//! return : <int> 1-hall of fame updated
//!                0-not updated
//!
int GameFileArray::add_hall_of_fame(int totalScore) {
    //-------- insert the record -----------//

    if (totalScore <=0)
	return 0;

    //0301
    if( !has_read_hall_of_fame ) {                  // only read once, GameFileArray::init() is called every time the load/save game menu is brought up.
	read_hall_of_fame();
	has_read_hall_of_fame = 1;
    }

    int i;

    for( i=0 ; i<HALL_FAME_NUM ; i++ ) {
	if( totalScore > hall_fame_array[i].score ) {
	    //---------- move and insert the data --------//

	    if( i < HALL_FAME_NUM-1 ) {                 // it is not the last record
		memmove( hall_fame_array+i+1, hall_fame_array+i,
			 sizeof(HallFame) * (HALL_FAME_NUM-i-1) );
	    }

	    //-------- record the hall of fame rcord ------//

	    hall_fame_array[i].record_data(totalScore);

	    //--------- display the hall of fame ----------//

	    write_hall_of_fame();                       // must write hall of fame, because it also write the last saved game slot no.

	    err_when(hall_fame_array[i].scenario_id  != player_school.scenario_id );
	    game.hall_of_fame();
	    return 1;
	}
    }

    return 0;
}

//------- End of function GameFileArray::add_hall_of_fame -----//
