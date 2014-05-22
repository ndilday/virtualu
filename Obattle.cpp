//Filename    : OBATTLE.CPP
//Description : Battle Object
//Owner       : Fred

#include <OBATTLE.H>
#include <OMUSIC.H>
#include <OBOX.H>
#include <OGAME.H>
#include <OINFO.H>
#include <OMISC.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OSCHLRES.H>
#include <ODEPT.H>
#include <OSYS.H>
#include <OWORLD.H>
#include <ORECT.h>
#include <OFIRM.H>
#include <OROAD.H>
#include <OPLANT.H>
#include <OFIRMRES.H>
#include <OLETTER.H>
//#include <OSPRITEA.H>
#include <OSPRITE.H>
#include <OPSCHOOL.H>
#include <OPEERSCH.H>                             // enrollment
#include <OFINANCE.H>
//##### begin fred 980824 #####//
#include <OFACULTY.H>
#include <OSTUDENT.H>
#include <ODEPTRES.H>

#include <OOPT.H>
#include <OATHLETI.H>
#include <ODEVELOP.H>
#include <OFacilit.H>                             //##### fred 980810 #####//
#include <OLibtech.H>                             //##### fred 980828 #####//
#include <OStuOff.H>                              //##### fred 980918 #####//
#include <OINVEST.H>                              //##### fred 980923 #####//
#include <OMATH.H>
//##### end fred 980824 #####//
#include <ALL.H>

//#define DEBUG_LOG_LOCAL 1
//#include <OLOG.H>

static void init_doctoral_target();

//-------- Begin of function Battle::init --------//
//!
void Battle::init() {
}

//-------- End of function Battle::init --------//

//-------- Begin of function Battle::deinit --------//
//!
void Battle::deinit() {
}

//-------- End of function Battle::deinit --------//

//-------- Begin of function Battle::run --------//
//!
//! <int> mpGame - whether this is a multiplayer game or not.
//! int scenId not in use  //## chea
//!
void Battle::run(int scenId) {
    //DEBUG_LOG("Battle:: run()");

    // ## begin chwg072999
    for (int k = 0; k < REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT; k++) {
	if(optimization.opt1_button_group_array[k]!=NULL)
	    optimization.opt1_button_group_array[k]->button_pressed=1;
	if(optimization.opt1_slider_group[k].init_flag!=0)
	    optimization.opt1_slider_group[k].enable_flag=1;
    }
    for (int k = 0; k < COST_RISE_POLICY_COUNT; k++) {
	if(optimization.opt2_button_group_array[k]!=NULL)
	    optimization.opt2_button_group_array[k]->button_pressed=1;
	if(optimization.opt2_slider_group[k].init_flag!=0)
	    optimization.opt2_slider_group[k].enable_flag=1;
    }
    // ## end chwg072999

    int oldCursor = mouse_cursor.get_icon();

    mouse_cursor.set_icon(CURSOR_WAITING);

    info.prerun_year=1;                             //## chea 150799 start prerun year here

    info.init_random_seed(0);

    switch( scenId ) {
    case 0:
    default:
	create_pregame_object();
	//		letter.init_welcome_letter();			//## chea 130899 move to info.next_day

	/*		break;
			case 1:
			create_pregame_object_scenario1();
			break;
	*/

	//	err_here();
	break;
    }

    if ( info.prerun_year==1)                       //## chea 150799 for going out of the prerun
	mouse_cursor.set_icon(CURSOR_WAITING);

    mouse_cursor.restore_icon(oldCursor);

    //	int songId = music.random_bgm_track();
    //	music.play(songId, sys.cdrom_drive ? MUSIC_PLAY_CD : 0 );

    //DEBUG_LOG("Battle:: going to call sys.run()");

    //--- give the control to the system main loop, start the game now ---//

    sys.run();
}

//--------- End of function Battle::run ---------//

//-------- Begin of function Battle::create_static_game_object --------//
//!
//! also called by load game
//!
void Battle::create_static_game_object() {
    //---- generate roads ----//

    generate_road();

    //---- generate firms ----//

    generate_firm();

    //---- generate sprites ----//

    //	generate_sprite();

    //---- generate plant ----//

    generate_plant();

}

//-------- Begin of function Battle::create_static_game_object --------//

//-------- Begin of function Battle::generate_road ---------//

void Battle::generate_road() {
    int blockWidth=10, blockHeight=8;
    int firmId=0;

    //------- lay horizontal roads -------//

    // only 3/4 of the area has roads - so that we can have a big area for placing stadium
    for( int yLoc=0 ; yLoc+blockHeight<=MAX_WORLD_Y_LOC*3/4 ; yLoc+=blockHeight ) {
	road_res.build_road(0, yLoc, MAX_WORLD_X_LOC-2, yLoc );
    }

    //------- lay vertical roads -------//

    for( int xLoc=0 ; xLoc+blockWidth<=MAX_WORLD_X_LOC ; xLoc+=blockWidth ) {
	road_res.build_road(xLoc, 0, xLoc, MAX_WORLD_Y_LOC-32 );
    }
}

//--------- End of function Battle::generate_road ---------//

//-------- Begin of function Battle::generate_firm ---------//
//!
void Battle::generate_firm() {
    int    blockWidth=10, blockHeight=8;
    int    firmId=0;
    FirmInfo* firmInfo;

    while(1) {
	if( ++firmId > firm_res.firm_count )
	    break;                                      // complete creating all needed buildings

	firmInfo = firm_res[firmId];

	// don't add departmental buildings as they will be added in Battle::create_pregame_objects()
	if( !firmInfo->field_id && firmInfo->is_available() ) {
	    create_firm(firmId);
	}
    }

    //----- add stadium, hard code because it will display across the border ---//

    //	firm_array.add_firm( 13, MAX_WORLD_Y_LOC-29, FIRM_PLAY_FIELD );		// play field
    // stadium
    firm_array.add_firm( 28, MAX_WORLD_Y_LOC-29, FIRM_STADIUM );
    // indoor sports center
    firm_array.add_firm( 48, MAX_WORLD_Y_LOC-29, FIRM_INDOOR_SPORTS );
}

//--------- End of function Battle::generate_firm ---------//

//-------- Begin of function Battle::create_firm ---------//

void Battle::create_firm(int firmId) {
    int    blockWidth=10, blockHeight=8;
    FirmInfo* firmInfo = firm_res[firmId];

    for( int yLoc=2, yCount=0 ; yLoc+blockHeight<=MAX_WORLD_Y_LOC ; yLoc+=blockHeight, yCount++ ) {
	for( int xLoc=2, xCount=0 ; xLoc+blockWidth<=MAX_WORLD_X_LOC ; xLoc+=blockWidth, xCount++ ) {
	    if( firmInfo->loc_width > blockWidth-ROAD_WIDTH ||
		firmInfo->loc_height > blockHeight-ROAD_HEIGHT ) {
		continue;
	    }

	    int buildXLoc = xLoc + (blockWidth-ROAD_WIDTH-firmInfo->loc_width)/2;
	    int buildYLoc = yLoc + (blockHeight-ROAD_HEIGHT-firmInfo->loc_height)/2;

	    if( firmInfo->can_build(buildXLoc, buildYLoc) ) {
		firm_array.add_firm( buildXLoc, buildYLoc, firmId );
		return;
	    }
	}
    }
}

//--------- End of function Battle::create_firm ---------//

//-------- Begin of function Battle::generate_plant ---------//

void Battle::generate_plant() {
    for( int yLoc=0 ; yLoc<MAX_WORLD_Y_LOC ; yLoc++ ) {
	for( int xLoc=0 ; xLoc<MAX_WORLD_X_LOC ; xLoc++ ) {
	    Location* locPtr = world.get_loc(xLoc, yLoc);

	    if( !locPtr->is_empty() )
		continue;

	    if( m.random(3)==0 ) {
		int plantId = m.random(plant_res.plant_count)+1;

		locPtr->set_plant(plantId, m.random(25), 75+m.random(25));
	    }
	}
    }
}

//--------- End of function Battle::generate_plant ---------//

//--------- Begin of function Battle::generate_sprite --------//
//!
void Battle::generate_sprite() {
    int unitId = 1;
    int xLoc, yLoc;

    //TO only put sprite within the viewable area (area within mapview) but not the world
    //i.e. refer to Matrix::draw_objects()
    //
    for( xLoc=0 ; xLoc<MAX_WORLD_X_LOC ; xLoc++ ) {
	for( yLoc=0 ; yLoc<MAX_WORLD_Y_LOC ; yLoc++ ) {
	    Location* locPtr = world.get_loc(xLoc, yLoc);

	    if( !locPtr->is_road() )                    // place sprite on road
		continue;

	    //temp: only road is set to be walkable
	    err_when( locPtr->is_walkable() && !locPtr->is_road() );

	    if( !m.random(6) ) {
		locPtr->set_sprite(unitId);
		//				Sprite* spritePtr = new Sprite;
		//				spritePtr->init( unitId, xLoc, yLoc, 0, 0 );
		//				sprite_array.add(spritePtr);
		unitId++;
	    }

	}                                             // for yLoc
    }

    /*
      for( xLoc=0 ; xLoc<MAX_WORLD_X_LOC ; xLoc++ )
      {
      for( yLoc=0 ; yLoc<MAX_WORLD_Y_LOC ; yLoc++ )
      {
      Location* locPtr = world.get_loc(xLoc, yLoc);

      if ( locPtr->has_sprite() )
      {
      //	sprite_array[locPtr->sprite_recno()]
      }
      }
      }*/
#ifdef DEBUG
    debug_msg("generate_sprite end: %d", unitId);
#endif
}

//---------- End of function Battle::generate_sprite ---------//

//--------- Begin of function Battle::run_loaded --------//
//!
void Battle::run_loaded() {
}

//---------- End of function Battle::run_loaded ---------//

//--------- Begin of function Battle::init_performance_indicator --------//
//!
void Battle::init_performance_indicator() {
    int i,j;
    StudentArray* stuArr;
    Department* deptPtr;

    for ( i=department_array.size(); i>0; i-- ) {
	if (department_array.is_deleted(i) )
	    continue;

	deptPtr = department_array[i];

	// loop faculty
	for ( j=deptPtr->faculty_array.faculty_count; j>0; j-- ) {
	    deptPtr->faculty_array[j]->update_history(UPDATE_ALL);
	}
	deptPtr->faculty_array.update_history(UPDATE_ALL);

	// loop student
	stuArr = &(department_array[i]->student_array);

	for (j=stuArr->size(); j>0; j--) {
	    //##	begin zhoubin 000329
	    if ( stuArr->is_deleted(j) )
		continue;
	    //##	end	zhoubin 000329
	    stuArr->operator[](j)->update_history(UPDATE_ALL);
	}
	deptPtr->student_array.update_history(UPDATE_ALL);

	deptPtr->course_array.update_history();

	deptPtr->update_history(UPDATE_ALL);
    }

    department_array.update_history(UPDATE_ALL);

    // special case for ug yr1 student:
    //
    stuArr = &(department_res.general_dept.student_array);
    for (j=stuArr->size(); j>0; j--) {
	//##	begin zhoubin 000329
	if ( stuArr->is_deleted(j) )
	    continue;
	//##	end	zhoubin 000329
	stuArr->operator[](j)->update_history(UPDATE_ALL);
    }

    // calc institutional-wide vars
    //
    //----------------//

    for (i=0; i<school_res.peer_school_count; i++) {
	//min & max bug chea
	school_res.peer_school_array[i].game_score = (float)m.fmax(20, math.get_random_snd(40,8));
    }

    //---------------//
    player_school.init_game_score = 0;
    player_school.cur_game_score = 0;
    player_school.ultimate_game_score = 0;

    player_school.init_pi();
    player_school.update_history(UPDATE_ALL);
}

//---------- End of function Battle::init_performance_indicator ---------//

//---------- Begin of function Battle::init_doctoral_target() ---------//
//!
void init_doctoral_target() {
    int i,j;
    CourseArray* courArr;
    Department* deptPtr;

    for ( i=department_array.size(); i>0; i-- ) {
	if (department_array.is_deleted(i) )
	    continue;

	deptPtr = department_array[i];

	//-------- PART 1 of 2 	 //0405
	// deptPtr->init_doctor_count = deptPtr->student_level_history[DOCTOR][THIS_YEAR];

	float doctorPct = 0; {
	    char s = DOCTOR;
	    doctorPct = player_school.student_level_pct[s] - player_school.student_level_pct[MASTER];
	}

	// "/4" means 4 year to graduate for DOCTOR in average
	deptPtr->init_doctor_count = (int) (player_school.get_player_expected_student_count() * doctorPct / 4);

	//-------- PART 2 of 2

	deptPtr->calc_research_dollar();
	deptPtr->doctors_per_research_dollar = math.safe_divide( deptPtr->init_doctor_count, deptPtr->total_research_dollar_direct*1000);

	//-------- PART B

	int totalBreakoutSections = 0;
	courArr = &(department_array[i]->course_array);

	for (j=courArr->size(); j>0; j--) {
	    if ( courArr->operator[](j)->teaching_method == BREAKOUT_LAB )
		totalBreakoutSections++;
	}

	deptPtr->init_breakout_section_count = totalBreakoutSections;
	//min & max bug chea
	deptPtr->breakout_section_by_doctor = (float) min((float)(totalBreakoutSections*TRIMESTER_PER_YEAR), (float)(DOCTORAL_AVERAGE_TEACHING_BREAKOUT_SECTIONS * deptPtr->init_doctor_count));
    }
}

//---------- End of function Battle::init_doctoral_target() ---------//

//-------- Begin of function Battle::create_pregame_object --------//
/*

a) generate player school using game settings,
b) generate DEPARTMENTS
c) generate non-year-one STUDENT sims
d) finance.init_data_pre_enroll()
e) enrollment management: init_data() and enroll_main()
f) course enrollment for old and newly matriculated students.
f2) ini.talent for sl1  //later
g1) generate FACULTY sims
g2) simulate one-year research progress for FACULTY sims	// 990415
h) facility_office.init_data_pre_finance()
i) finance
j) other small offices
k) calc statistics: student/faculty count

*/
//
// Initialize pre-game objects - departments, students, etc
//
void Battle::create_pregame_object() {
    int i;

    //--------------------------------------//
    //special case:

    //	box.progress_update(14);

    //chwg0111
    sys.report_before_morerpt_or_deptlist=MODE_NORMAL;
    sys.report_before_help=MODE_NORMAL;             //chwg0713

    memset(&athletics_office, 0, sizeof(Athletics));

    athletics_office.last_ncaa_level_input = player_school.athletic_program_intensity;
    // assumed called before school_res.init_game()
    athletics_office.input_2_ncaa_level(player_school.athletic_program_intensity);

    school_res.init_game();                         // initialize school_res.player_peer_school and school_res.peer_school_array[]

    //---- generate testing departments ----//

    create_static_game_object();

    for (i=0; i<MAX_DEPARTMENT; i++) {
	int deptId = player_school.selected_department_list[i];
	if ( deptId <= 0 )
	    break;
	else
	    department_array.add(deptId);
    }

    department_array.init_data();

    department_res.calc_total_elective_course_pref();

    //----------------------------//
    //special case:
    athletics_office.init_data_pre_finance();

    //---- generate students ----//

    for( i=department_array.size() ; i>0 ; i--) {
	if( department_array.is_deleted(i) )
	    continue;

	// clear course data in last trimester
	department_array[i]->course_array.next_trimester();
	sys.yield();
    }

    box.progress_update(14);                        //## chea 150799 start prerun year here

    player_school.generate_student();               // must after deparment_array.add(); this will select courses taken

    //----- Enrollment model -----//
    // must run after school_res.build_peer_school_array()
    // must run after player_school.generate_student to calc student_count

    finance.init_data_pre_enroll();

    enroll_res.init_data();

    //---------------------------//
    // whether do this serval times or not???
    //td3.4 section 2.6: overall model initialization

    init_doctoral_target();                         // before enroll_main
    enroll_res.enroll_main(80);                     // GENERATE student; required for athletics_office.init_data	//1: init_data

    //---------------------------------------//

    // g1) generate_faculty() called after course enrollment; see email req42_misc.txt

    for( i=department_array.size(); i>0; i--) {
	if( department_array.is_deleted(i) )
	    continue;

	Department* deptPtr = department_array[i];

	deptPtr->generate_faculty();                  // GENERATE faculty
	deptPtr->init_hiring();                       // must after generate_faculty()
    }

    // g2) simulate one-year research progress for all FACULTY sims	// 990421

    department_array.init_research();               // 990421

    //---------------------------------------//

    //----- Financial model -----//

    facility_office.init_data_pre_finance();        // 0119 calc normal onm; after course enroll and faculty gen.

    finance.init_data();                            // after generate faculty and student; also after enroll since requires enroll_res.total_student_count

    //---------------------------------------//

    // offices must called after finance.init_data
    development_office.init_data();
    athletics_office.init_data();                   // after enroll also

    library_tech_office.init_data();

    student_office.init_data();

    investment_office.init_data();

    //--- post init ------------------------------------//

    init_performance_indicator();                   // must after finance and enroll

    info.update_distance_learning_variables(1);     // need call this function after init_performance_indicator

    library_tech_office.init_data();                // 990508 // after init_performance_indicator

    facility_office.init_data();                    // after department_array.add() and department_array.update_history(UPDATE_ALL);

    // must after finance and enroll
    finance.post_init_data();                       // 0218

    //-----------------//
    // 0130 scenario

#ifdef DEBUG
    for (i=department_array.size(); i>0; i--) {
	Department *deptPtr;

	deptPtr = department_array[i];
	err_when(i != deptPtr->department_recno);
    }
#endif

    for ( i=department_array.size(); i>0; i-- ) {
	if (department_array.is_deleted(i))
	    continue;

	Department* deptPtr = department_array[i];

	int arraySize = deptPtr->cur_faculty_array.size();

	if ( arraySize != 0 )
	    deptPtr->cur_faculty_array.zap();
    }

    box.progress_update(28);                        //## chea 150799 start prerun year here
}

//-------- End of function Battle::create_pregame_object --------//

//-------- Begin of function Battle::create_pregame_object_scenario1 --------//
//!
//! for scenario (1): Balance the budget
//!
void Battle::create_pregame_object_scenario1() {
    create_pregame_object();
}

//-------- End of function Battle::create_pregame_object_scenario1 --------//
