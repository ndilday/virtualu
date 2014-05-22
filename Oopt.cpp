
//Filename    : OOPT.CPP
//Description : Optimization class - main body

#include <OSYS.H>
#include <OBOX.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OBNDSLDR.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OFINANCE.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OOPT.H>
#include <OIFACE.H>
#include <OBUTTON.H>
#include <ODEPT.H>

//------- Define constants -------//

enum {
    REPORT_X1 = ZOOM_X1 + 15,
    REPORT_Y1 = ZOOM_Y1 + 25,
    REPORT_X2 = ZOOM_X2 - 15,
    REPORT_Y2 = ZOOM_Y2 - 60
};

enum {
    BUTTON_X1 = REPORT_X1 + 70,
    BUTTON_Y1 = REPORT_Y2 + 15,
    BUTTON_X2 = REPORT_X2 - 70,
    BUTTON_Y2 = REPORT_Y2 + 40,
    BUTTON_OPTIMIZE_WIDTH = 350,
    BUTTON_BACK_WIDTH = 80,
    BUTTON_NEXT_WIDTH = 80,
    BUTTON_DISTANCE = (BUTTON_X2 - BUTTON_X1 - BUTTON_OPTIMIZE_WIDTH - BUTTON_BACK_WIDTH - BUTTON_NEXT_WIDTH) / 2
};

enum {  BG_PIC_ID = 3 };

//-------- define static vars -----------//

static char redraw_flag = 0;
static Button button_optimize, button_back, button_next;

//------- Begin of function Optimization::Optimization -----------//
//!
Optimization::Optimization() {
    init();

    init_stage_1();
    init_stage_2();
    init_stage_3();
}

//--------- End of function Optimization::Optimization -----------//

//------- Begin of function Optimization::~Optimization -----------//
//!
Optimization::~Optimization() {
    deinit();
}

//--------- End of function Optimization::~Optimization -----------//

//------- Begin of function Optimization::deinit ---------------//
//!
void Optimization::deinit() {
    switch (stage_init_flag) {
    case 1: deinit_stage_1();
	break;

    case 2: deinit_stage_2();
	break;

    case 3: deinit_stage_3();
	break;

    default:
	stage_init_flag = 0;
    }
}

//--------- End of function Optimization::deinit ---------------//

//------- Begin of function Optimization::init -----------------//
//!
void Optimization::init() {
}

//--------- End of function Optimization::init -----------------//

//------- Begin of function Optimization::disp ---------------//
//!
void Optimization::disp(int refreshFlag) {
    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	short x = BUTTON_X1;

	button_back.paint_text( x, BUTTON_Y1,
				x+BUTTON_BACK_WIDTH, BUTTON_Y2, "<< Back" );

	x += BUTTON_BACK_WIDTH + BUTTON_DISTANCE;

	button_optimize.paint_text( x, BUTTON_Y1,
				    x+BUTTON_OPTIMIZE_WIDTH, BUTTON_Y2, "Optimize");

	x += BUTTON_OPTIMIZE_WIDTH + BUTTON_DISTANCE;

	if ( current_stage == 3 ) {
	    char buttonText[123];
	    strcpy ( buttonText, "Start Year " );
	    strcat( buttonText, m.format(info.game_year) );
	    button_next.paint_text( x, BUTTON_Y1, x+BUTTON_NEXT_WIDTH*2-20, BUTTON_Y2, buttonText );
	}
	else
	    button_next.paint_text( x, BUTTON_Y1,
				    x+BUTTON_NEXT_WIDTH, BUTTON_Y2, "Next >>" );
    }

    //-------------------------------------------------//

    switch(current_stage) {
    case 1: report_stage_1(refreshFlag); break;
    case 2: report_stage_2(refreshFlag); break;
    case 3: report_stage_3(refreshFlag); break;
    }

    if (refreshFlag == INFO_REPAINT) {
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    }

    if((button_optimize_is_down==1)
       // chwg992407
       &&(is_optimize_slider_modified[current_stage-1]==0)) {
	button_optimize.paint();
	vga.blt_buf( BUTTON_X1 + BUTTON_BACK_WIDTH + BUTTON_DISTANCE,
		     BUTTON_Y1,
		     BUTTON_X1 + BUTTON_BACK_WIDTH + BUTTON_DISTANCE + BUTTON_OPTIMIZE_WIDTH, BUTTON_Y2);
	button_optimize_is_down=0;
	is_optimize_slider_modified[current_stage-1]=1;
    }
    //---- can only proceed to the next when the optimization has been completed ----//

    vga.use_front();
    if(!is_optimize_slider_modified[current_stage-1]) {
	button_optimize.enable();
	button_next.disable();

    }
    /*
      //## chea 051199 BEGIN to change th back button
      else if(info.debug_enroll == 8 && button_optimize_is_down == 0)
      {
      if(!is_optimize_slider_modified[current_stage-1])
      {
      button_optimize.disable();
      button_next.enable();
      }
      else
      {
      button_optimize.enable();
      button_next.disable();
      }
      }
      //## chea 051199 END to change th back button
    */
    else {
	button_optimize.disable();
	button_next.enable();
    }

}

//--------- End of function Optimization::disp --------------//

//------- Begin of function Optimization::detect ---------------//
//!
int Optimization::detect() {
    vga.use_back();

    int rc = detect2();

    vga.use_front();

    return rc;
}

//--------- End of function Optimization::detect --------------//

//------- Begin of function Optimization::detect2 ---------------//
//!
int Optimization::detect2() {
    if (button_optimize.detect()) {
	// ##fred 1029
	//if (result_flag)
	//	return 1;

	//----------------------------------------------------------//
	// Notes:	a dialog box should pop-up when optimize_policy_x
	//				returns false, to be modified
	// Norris 0717
	//----------------------------------------------------------//
	button_optimize_is_down=1;
	switch (current_stage) {
	case 1:
	    if(is_optimize_slider_modified[0]==0) {
		if (finance.optimize_policy_1()) {
		    result_flag = 1;
		    is_optimized[0] = 1;
		}
		else
		    result_flag = 0;                      //## fred
		break;

		is_optimize_slider_modified[0]=1;
	    }
	case 2:
	    if(is_optimize_slider_modified[1]==0) {
		if (finance.optimize_policy_2()) {
		    result_flag = 1;
		    is_optimized[1] = 1;
		    finance.pre_optimization_stage_3();   //## fred 990507
		}
		else
		    result_flag = 0;                      //## fred
		break;

		is_optimize_slider_modified[1]=1;
	    }

	case 3:
	    if(is_optimize_slider_modified[2]==0) {
		if (finance.optimize_policy_3()) {

		    //## chea 300799 CDE

		    int deptC=0;
		    deptC = department_array.department_count;
		    deptC--;

		    //BUGHERE
		    for (int t=0; t <= deptC; t++) {
			*(optimization.opt3_slider_group[t].up_ptr)= (finance.hiring_policy_array[t].upper_bound);
			*(optimization.opt3_slider_group[t].var_ptr)= (finance.hiring_policy_array[t].target_value);
			vga.use_back();
			/*
                          if(info.debug_enroll!=8)    	//## chea 051199 BEGIN to change th back button
			  optimization.opt3_slider_group[t].refresh();
			*/
		    }     
		    //## chea 300799 cde

		    result_flag = 1;
		    is_optimized[2] = 1;
		}
		else
		    result_flag = 0;                      //## fred
		break;

		is_optimize_slider_modified[2]=1;
	    }
	}

	// display modal dialog box if optimization fails
	if(result_flag == 0)
	    box.msg("optimization fails");

	return 1;
    }

    //--------------------------------------//

    int newStage = -1;

    if( button_back.detect() ) {
	newStage = current_stage-1;

	if ( current_stage == 1 ) {                   // first page
	    // should return to faculty turnover
	    return 2;
	}
	/*
	  // ##chwg073099 begin
	  if(current_stage==3)
	  {
          is_optimized[2] = 0;
          is_optimize_slider_modified[2]=0;
	  }
	  // ##chwg073099 begin
	*/
	//## chea 051199 BEGIN to change th back button
	is_optimized[newStage] = 0;
	is_optimize_slider_modified[newStage]=0;
	is_optimized[current_stage] = 0;
	is_optimize_slider_modified[current_stage]=0;
	//## chea 051199 END to change th back button
    }

    if( button_next.detect() )
	newStage = current_stage+1;

    if( newStage >= 0 ) {
	result_flag = 0;

	switch( current_stage ) {                     // deinit the existing stage variables
	case 1:
	    deinit_stage_1();
	    break;

	case 2:
	    deinit_stage_2();
	    break;

	case 3:
	    deinit_stage_3();
	    break;
	}

	current_stage = newStage;
	optimization.button_optimize_is_down=0;
	redraw_flag = 1;

	return 1;
    }

    //-----------------------------------//

    switch (current_stage) {
    case 1:
	if (detect_stage_1())
	    return 1;
	break;

    case 2:
	if (detect_stage_2())
	    return 1;
	break;

    case 3:
	if (detect_stage_3())
	    return 1;
	break;
    }

    return 0;
}

//--------- End of function Optimization::detect2 --------------//
