//Filename   : OFIRM.CPP
//Description: Class Firm functions
//owner		: ho
#include <OINFO.H>
#include <OWORLD.H>
#include <OAUDIO.H>
#include <OCONFIG.H>
#include <OSYS.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OFIRMRES.H>
#include <OFIRM.H>
#include <OBOX.H>

char* chapel_display_array[] = {
    "Praying for a win on Saturday, are we?",
    "The parking situation around here needs a few prayers!",
    "Praying should help but there are more ways to raise faculty morale!",
    "Welcome to the Chapel, peace can be found here.",
    "Asking for some help from above?"
};
static int chapel_display_count = 5;

//--------- Begin of function Firm::init --------//
//!
//! <int> xLoc, yLoc  = the location of firm in the world map
//! <int> firmId      = id(type) of the firm
//!
void Firm::init(int xLoc, int yLoc, int firmId) {
    firm_id     = firmId;
    setup_date = info.game_date;

    department_recno = 0;

    //----- set the firm's location & absolute positions on the map -----//

    FirmInfo* firmInfo = firm_res[firmId];

    loc_x1 = xLoc;
    loc_y1 = yLoc;
    loc_x2 = loc_x1 + firmInfo->loc_width - 1;
    loc_y2 = loc_y1 + firmInfo->loc_height - 1;

    //	firm_width=firmInfo->loc_width;
    //	firm_height=firmInfo->loc_height;

    init_abs(ZOOM_SMALL , MAP_LOC_WIDTH  , MAP_LOC_HEIGHT);
    init_abs(ZOOM_MEDIUM, ZOOM1_LOC_WIDTH, ZOOM1_LOC_HEIGHT);
    init_abs(ZOOM_LARGE , ZOOM2_LOC_WIDTH, ZOOM2_LOC_HEIGHT);

    //------ init loc_matrix --------//

    init_matrix();

    //-------- init derived ---------//

    //	init_derived();
}

//----------- End of function Firm::init ---------//

//--------- Begin of function Firm::deinit --------//
//!
void Firm::deinit() {
    deinit_matrix();
}

//----------- End of function Firm::deinit ---------//

//--------- Begin of function Firm::init_abs ---------//
//!
void Firm::init_abs(int zoomLevel, int locWidth, int locHeight) {
    Rect* rectPtr = abs_rect+zoomLevel;

    FirmInfo* firmInfo = firm_res[firm_id];

    int bmpWidth  = *( (short*)firmInfo->bitmap_ptr[zoomLevel] );
    int bmpHeight = *( ((short*)firmInfo->bitmap_ptr[zoomLevel])+1 );

    rectPtr->x1 = (locWidth/2) * (loc_x1 + loc_y1);
    rectPtr->x2 = rectPtr->x1 + bmpWidth - 1;

    rectPtr->y2 = (MAX_WORLD_Y_LOC / 2 * locHeight) + ((locHeight/2) * (-loc_x1 + loc_y2 + 1 ) - 1);
    rectPtr->y1 = rectPtr->y2 - bmpHeight + 1;
}

//----------- End of function Firm::init_abs ---------//

//--------- Begin of function Firm::init_matrix --------//
//!
void Firm::init_matrix() {
    //	for( int yLoc=max(1,loc_y1-1) ; yLoc<=min(MAX_WORLD_Y_LOC,loc_y2+1) ; yLoc++ )
    for( int yLoc=max(1,loc_y1-1) ; yLoc<=loc_y2 ; yLoc++ ) {
	//		for( int xLoc=max(1,loc_x1-1) ; xLoc<=min(MAX_WORLD_X_LOC,loc_x2+1) ; xLoc++ )
	for( int xLoc=loc_x1 ; xLoc<=min(MAX_WORLD_X_LOC,loc_x2+1) ; xLoc++ ) {
	    if(!world.get_loc(xLoc,yLoc)->is_road())
		if(!world.get_loc(xLoc,yLoc)->is_road_sub())
		    world.get_loc(xLoc,yLoc)->set_firm(firm_recno);
	}
    }

    // ##begin chwg0705
    // avoid invalid display of tree

    int w=loc_x2-loc_x1+1;
    int h=loc_y2-loc_y1+1;
    if(w>h) {
	int d=(w-h)/2;
	int c, yLoc;
	for(c=2, yLoc=max(1,loc_y1-d) ; yLoc<loc_y1 ; yLoc++ ,c++) {
	    for(int xLoc=loc_x1 ; xLoc<=loc_x1+c ; xLoc++ ) {
		if(!world.get_loc(xLoc,yLoc)->is_road())
		    if(!world.get_loc(xLoc,yLoc)->is_road_sub())
			world.get_loc(xLoc,yLoc)->set_firm(firm_recno);
	    }
	}
	for(c=0, yLoc=loc_y2+1 ; yLoc<=min(MAX_WORLD_Y_LOC,loc_y2+d) ; yLoc++ ,c++) {
	    for(int xLoc=loc_x2-d+c ; xLoc<=loc_x2 ; xLoc++ ) {
		if(!world.get_loc(xLoc,yLoc)->is_road())
		    if(!world.get_loc(xLoc,yLoc)->is_road_sub())
			world.get_loc(xLoc,yLoc)->set_firm(firm_recno);
	    }
	}
    }

    // ##end chwg0705

}

//----------- End of function Firm::init_matrix --------//

//--------- Begin of function Firm::deinit_matrix --------//
//!
void Firm::deinit_matrix() {
    for( int yLoc=max(1,loc_y1-2) ; yLoc<=min(MAX_WORLD_Y_LOC,loc_y2+2) ; yLoc++ ) {
	//	for( int yLoc=loc_y1 ; yLoc<=loc_y2 ; yLoc++ )
	for( int xLoc=max(1,loc_x1-2) ; xLoc<=min(MAX_WORLD_X_LOC,loc_x2+2) ; xLoc++ ) {
	    //		for( int xLoc=loc_x1 ; xLoc<=loc_x2 ; xLoc++ )
	    if(world.get_loc(xLoc,yLoc)->is_firm())
		world.get_loc(xLoc,yLoc)->remove_firm();
	}
    }
}

//---------- End of function Firm::deinit_matrix ---------//

//--------- Begin of function Firm::name --------//
//!
char* Firm::name() {
    static String str;

    if( department_recno ) {
	int deptId = department_array[department_recno]->department_id;

	str  = department_res[deptId]->name;
	str += " Department";
    }
    else {
	str = firm_res[firm_id]->name;
    }

    return str;
}

//---------- End of function Firm::name ---------//

//--------- Begin of function Firm::name_on_map --------//
//!
//! for display name on zoom matrix
//! return NULL if name not going to be displayed on zoom matrix
//!
char* Firm::name_on_map() {
    if( department_recno ) {
	int deptId = department_array[department_recno]->department_id;
	return department_res[deptId]->name;
    }
    else {
	return firm_res[firm_id]->name;
    }
    return NULL;
}

//--------- End of function Firm::name_on_map --------//

//--------- Begin of function Firm::double_click_firm --------//
//!
//! This is called when the player double clicks on this firm.
//!
void Firm::double_click_firm() {
    int viewMode=0;

    if( firm_id < FIRST_NONDEPARTMENTAL_FIRM_ID ) { // if it's a departmental building
	viewMode = MODE_PERFORMANCE_REPORT;
	enum {
	    MODE_INSTITUTION  = 0,
	    MODE_DEPARTMENT = 1,
	    MODE_RANK     = 2,
	};
	department_array.current_data_mode=MODE_DEPARTMENT;
    }
    else {
	switch(firm_id) {
	case FIRM_STUDENT:                          // Student Union
	    viewMode = MODE_STUDENT_REPORT;
	    break;

	case FIRM_PRESIDENT: {                      // ## chwg0707
	    if(info.year_passed<=1) {
		viewMode = MODE_WELCOME_LETTER;
	    }
	    else {
		viewMode = MODE_EVALUATION_LETTER;
	    }
	}
	break;

	case FIRM_TOWER:                            // ## chwg0707
	    viewMode = MODE_SCORE_REPORT;
	    break;

	case FIRM_OLD_MAIN:
	case FIRM_POLICE:
	case FIRM_PARK_LOTS:
	case FIRM_PARK_STRUCTURE:
	case FIRM_ADMISSIONS:
	case FIRM_DEVELOPMENT:
	case FIRM_FACILITIES:
	case FIRM_LIBRARY:
	case FIRM_STADIUM:
	case FIRM_RESIDENCE:                        // ## chwg0930
	case FIRM_PLAY_FIELD:
	case FIRM_INDOOR_SPORTS:
	    // if is not a departmental building
	    viewMode = MODE_FIRST_NONDEPARTMENTAL_FIRM+
		firm_id-FIRST_NONDEPARTMENTAL_FIRM_ID;
	    break;
	case FIRM_CHAPEL:
	    int randomMsg = m.random( chapel_display_count );
	    box.msg( chapel_display_array[randomMsg] );
	    break;
	}
    }

    if( viewMode ) {
	sys.set_view_mode(viewMode, 0);               // 0-don't play sound as it is played in FirmArray::select_firm() already
    }
}

//---------- End of function Firm::double_click_firm ---------//
