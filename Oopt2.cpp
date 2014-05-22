//Filename    : OOPT2.CPP
//Description : Optimization class - stage 2

#include <OSYS.H>
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
#include <OBUTTON.H>
#include <OIFACE.H>
#include <OIMGRES.H>
#include <OTEXT.H>
#include <OHELPIF.H>
#include <OHELP.H>
#include <OAUDIO.H>
#include <OCONFIG.H>
#include <OCHANCE.H>

//------- Define constants -------//

enum {
    REPORT_X1 = ZOOM_X1 + 15,
    REPORT_Y1 = ZOOM_Y1 + 25,
    REPORT_X2 = ZOOM_X2 - 15,
    REPORT_Y2 = ZOOM_Y2 - 60
};

enum {
    SEPARATOR_1 = REPORT_X1 + 300,
    SEPARATOR_2 = REPORT_X1 + 500,
    SEPARATOR_3 = REPORT_X1 + 620
};

enum {
    LINE_SPACING = 25,
    HEADING_HEIGHT = 30,
    FIRST_LINE_OFFSET = REPORT_Y1 + HEADING_HEIGHT + 20,
    DESCRIPTION_OFFSET = REPORT_X1 + 30,
    SLIDER_OFFSET = 1,
    DESCRIPTION_INDENT = 20,
    TARGET_OFFSET = SEPARATOR_1 + 15,
    TARGET_WIDTH = SEPARATOR_2 - SEPARATOR_1 - 30,
    TARGET_HEIGHT = 10,
    PREF_BUTTON_WIDTH = 16,
    PREF_BUTTON_HEIGHT = 16,
    PREF_BUTTON_DISTANCE = 10,
    PREF_BUTTON_OFFSET = 0,
    PREF_BUTTON_OFFSET1 = SEPARATOR_2 + 15,
    PREF_BUTTON_OFFSET2 = PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH + PREF_BUTTON_DISTANCE,
    PREF_BUTTON_OFFSET3 = PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH + PREF_BUTTON_DISTANCE,
    PREF_BUTTON_OFFSET4 = PREF_BUTTON_OFFSET3 + PREF_BUTTON_WIDTH + PREF_BUTTON_DISTANCE,
    RESULT_OFFSET = SEPARATOR_3 + 20,
    RESULT_LENGTH = 50
};

//-------- define static vars -----------//

static Text result_text[COST_RISE_POLICY_COUNT];
static Button help_button_array[COST_RISE_POLICY_COUNT];

static char *title[] = {
    "Description",
    "Target",
    "      Preference",
    "Result"
};

static char *heading = "Allocation of net budget change";

static char *pref_label[] = {
    "1", "2", "3", "R"
};

//------- Begin of function Optimization::init_stage_2 -----------//
//!
void Optimization::init_stage_2() {
    for (int i = 0; i < COST_RISE_POLICY_COUNT; i++) {
	for (int j = 0; j < PREF_COUNT; j++) {
	    opt2_button_group_up_bitmap[i * PREF_COUNT + j] = NULL;
	    opt2_button_group_down_bitmap[i * PREF_COUNT + j] = NULL;
	}
    }
}

//--------- End of function Optimization::init_stage_2 -----------//

//------- Begin of function Optimization::deinit_stage_2 ---------//
//!
void Optimization::deinit_stage_2() {
    for (int i = 0; i < COST_RISE_POLICY_COUNT; i++) {
	delete opt2_button_group_array[i];

	if(opt2_button_group_array[i]) {
	    opt2_button_group_array[i]=NULL;
	}

	opt2_slider_group[i].deinit();

	for (int j = 0; j < PREF_COUNT; j++) {
	    if(opt2_button_group_up_bitmap[i * PREF_COUNT + j]) {
		mem_del(opt2_button_group_up_bitmap[i * PREF_COUNT + j]);
		opt2_button_group_up_bitmap[i * PREF_COUNT + j] = (short*)NULL;
	    }
	    if(opt2_button_group_down_bitmap[i * PREF_COUNT + j]) {
		mem_del(opt2_button_group_down_bitmap[i * PREF_COUNT + j]);
		opt2_button_group_down_bitmap[i * PREF_COUNT + j] = (short*)NULL;
	    }
	}

	//		opt2_button_group_array[i]->button_pressed=2;
	result_text[i].deinit();
	// ##### begin Gilbert 28/04/2001 ######//
	help_button_array[i].deinit();
	// ##### end Gilbert 28/04/2001 ######//
    }
    stage_init_flag = 0;
}

//--------- End of function Optimization::deinit_stage_2 -------//

//------- Begin of function Optimization::report_stage_2 -------//
//!
void Optimization::report_stage_2(int refreshFlag) {
    if (refreshFlag == INFO_REPAINT || refresh_optimization_screen ) {
	user_interface.brighten(REPORT_X1+100, ZOOM_Y1, REPORT_X2-100, REPORT_Y1);
	user_interface.brighten(REPORT_X1+4, REPORT_Y1+4, REPORT_X2-2, REPORT_Y2-2);
	user_interface.rect(REPORT_X1+100, ZOOM_Y1, REPORT_X2-100, REPORT_Y1);
	//		font_charts.center_put( REPORT_X1, ZOOM_Y1, REPORT_X2, REPORT_Y1, "Stage 2 : Expenditure by Function" );
	font_charts.center_put( REPORT_X1, ZOOM_Y1, REPORT_X2, REPORT_Y1, "Budget Allocations" );

	// current stage not init'ed
	if (stage_init_flag != 2 || refresh_optimization_screen ) {
	    deinit();
	    ButtonGroup *buttonGroupPtr;
	    double *targetValue, *lowerBound, *upperBound, *importWeight, lowerLimit, upperLimit;
	    int pressedButton;

	    for (int i = 0; i < COST_RISE_POLICY_COUNT; i++) {
		opt2_button_group_array[i] = new ButtonGroup(PREF_COUNT);

		//##### begin fred 980904#####//
		targetValue = &(finance.cost_rise_policy_array[i].target_value);
		lowerBound = &(finance.cost_rise_policy_array[i].lower_bound);
		upperBound = &(finance.cost_rise_policy_array[i].upper_bound);
		importWeight = &(finance.cost_rise_policy_array[i].import_weight);
		lowerLimit = (finance.cost_rise_policy_array[i].lower_limit);
		upperLimit = (finance.cost_rise_policy_array[i].upper_limit);

		//################begin chwg0930 BUGHERE ####################/
		if(i==9)
		    importWeight = &(finance.cost_rise_policy_array[8].import_weight);

		//				opt2_slider_group[i].init(
		//					TARGET_OFFSET, SLIDER_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1),
		//					TARGET_WIDTH, TARGET_HEIGHT, targetValue, lowerBound, upperBound, *targetValue-(*targetValue-*lowerBound)*2, *targetValue+(*upperBound-*targetValue)*2);
		opt2_slider_group[i].init(
		    TARGET_OFFSET, SLIDER_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1),
		    TARGET_WIDTH, TARGET_HEIGHT, targetValue, lowerBound, upperBound, lowerLimit, upperLimit,
		    1,V_YELLOW,V_GREEN,24);                 //## chea 110899 change by b.massy add 24(1dec)
		//##### end fred 980904#####//

		user_interface.create_button_bitmap(PREF_BUTTON_OFFSET1-2,
						    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1)-2,
						    PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH,
						    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT,
						    pref_label[0], &opt2_button_group_up_bitmap[PREF_COUNT * i], &opt2_button_group_down_bitmap[PREF_COUNT * i], &vga_back);
		user_interface.create_button_bitmap(PREF_BUTTON_OFFSET2-2,
						    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1)-2,
						    PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH,
						    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT,
						    pref_label[1], &opt2_button_group_up_bitmap[PREF_COUNT * i + 1], &opt2_button_group_down_bitmap[PREF_COUNT * i + 1], &vga_back);
		user_interface.create_button_bitmap(PREF_BUTTON_OFFSET3-2,
						    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1)-2,
						    PREF_BUTTON_OFFSET3 + PREF_BUTTON_WIDTH,
						    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT,
						    pref_label[2], &opt2_button_group_up_bitmap[PREF_COUNT * i + 2], &opt2_button_group_down_bitmap[PREF_COUNT * i + 2], &vga_back);
		user_interface.create_button_bitmap(PREF_BUTTON_OFFSET4-2,
						    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1)-2,
						    PREF_BUTTON_OFFSET4 + PREF_BUTTON_WIDTH,
						    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT,
						    pref_label[3], &opt2_button_group_up_bitmap[PREF_COUNT * i + 3], &opt2_button_group_down_bitmap[PREF_COUNT * i + 3], &vga_back);
		buttonGroupPtr = opt2_button_group_array[i];

		//				pressedButton = int(*importWeight * 2.0) - 1;
		if(*importWeight==OPT_LOW_WEIGHT)
		    pressedButton =0;
		if(*importWeight==OPT_MID_WEIGHT)
		    pressedButton =1;
		if(*importWeight==OPT_HIGH_WEIGHT)
		    pressedButton =2;

		if(finance.cost_rise_policy_array[i].required_flag) {
		    opt2_slider_group[i].enable_flag=0;
		    pressedButton = 3;
		}
		//				else
		//				{
		//					if(opt2_button_group_array[i]->button_pressed==3)
		//						pressedButton = 1;
		//					opt2_slider_group[i].enable_flag=1;
		//				}

		//## chea captial campaign 230699
		if(chance_event.capital_campaign_end_year>info.game_year) {
		    // disable
		    opt2_slider_group[PL_INST_ADVANCEMENT].enable_flag=0;
		    pressedButton = 3;
		}
		//## chea captial campaign 230699

		(*buttonGroupPtr)[0].create_bitmapW(
		    PREF_BUTTON_OFFSET1-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1)-2,
		    PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT,
		    ((pressedButton == 0) ? opt2_button_group_down_bitmap[PREF_COUNT * i] : opt2_button_group_up_bitmap[PREF_COUNT * i]) + 4);
		(*buttonGroupPtr)[1].create_bitmapW(
		    PREF_BUTTON_OFFSET2-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1)-2,
		    PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT,
		    ((pressedButton == 1) ? opt2_button_group_down_bitmap[PREF_COUNT * i + 1] : opt2_button_group_up_bitmap[PREF_COUNT * i + 1]) + 4);
		(*buttonGroupPtr)[2].create_bitmapW(
		    PREF_BUTTON_OFFSET3-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1)-2,
		    PREF_BUTTON_OFFSET3 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT,
		    ((pressedButton == 2) ? opt2_button_group_down_bitmap[PREF_COUNT * i + 2] : opt2_button_group_up_bitmap[PREF_COUNT * i + 2]) + 4);
		(*buttonGroupPtr)[3].create_bitmapW(
		    PREF_BUTTON_OFFSET4-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1)-2,
		    PREF_BUTTON_OFFSET4 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT,
		    ((pressedButton == 3) ? opt2_button_group_down_bitmap[PREF_COUNT * i + 3] : opt2_button_group_up_bitmap[PREF_COUNT * i + 3]) + 4);

		buttonGroupPtr->button_pressed = pressedButton;

		if(user_interface.backgd_flag==1)
		    help_button_array[i].create_text(REPORT_X1+10,
						     FIRST_LINE_OFFSET + LINE_SPACING * (i+1),
						     REPORT_X1+28,
						     FIRST_LINE_OFFSET + LINE_SPACING * (i+1)+18,
						     "?");
	    }

	    stage_init_flag = 2;
	}

	user_interface.h_line(REPORT_X1, REPORT_Y1+HEADING_HEIGHT, REPORT_X2-REPORT_X1);
	user_interface.v_line(SEPARATOR_1-1, REPORT_Y1+1, REPORT_Y2-REPORT_Y1-2);
	user_interface.v_line(SEPARATOR_2-1, REPORT_Y1+1, REPORT_Y2-REPORT_Y1-2);
	user_interface.v_line(SEPARATOR_3-1, REPORT_Y1+1, REPORT_Y2-REPORT_Y1-2);
	user_interface.rect(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y2, 2);

	font_chartsm.center_put( REPORT_X1+1, REPORT_Y1+1,
				 SEPARATOR_1-1, REPORT_Y1+1+HEADING_HEIGHT, title[0] );
	font_chartsm.center_put( SEPARATOR_1+1, REPORT_Y1+1,
				 SEPARATOR_2-1, REPORT_Y1+1+HEADING_HEIGHT, title[1] );
	font_chartsm.center_put( SEPARATOR_2+1, REPORT_Y1+1,
				 SEPARATOR_3-1, REPORT_Y1+1+HEADING_HEIGHT, title[2] );
	font_chartsm.center_put( SEPARATOR_3+1, REPORT_Y1+1,
				 REPORT_X2-1, REPORT_Y1+1+HEADING_HEIGHT, title[3] );

	if(user_interface.backgd_flag==1)
	    image_interface.put_back(SEPARATOR_2+9,REPORT_Y1+7,"QUEST_UP");

	font_chartsm.put( DESCRIPTION_OFFSET, FIRST_LINE_OFFSET, heading);

	for (int i = 0; i < COST_RISE_POLICY_COUNT; i++) {
	    font_chartsm.put(
		DESCRIPTION_OFFSET + DESCRIPTION_INDENT,
		FIRST_LINE_OFFSET + LINE_SPACING * (i + 1),
		game_str_res.opt_stage2_str(i) );

	    if(user_interface.backgd_flag==1)
		image_interface.put_back(REPORT_X1+10,
					 FIRST_LINE_OFFSET + LINE_SPACING * (i+1),"QUEST_UP");
	    opt2_slider_group[i].paint();

	    opt2_button_group_array[i]->paint(opt2_button_group_array[i]->button_pressed);

	    opt2_slider_group[i].result_flag=optimization.is_optimized[1];
	    opt2_slider_group[i].result_value = finance.cost_rise_policy_array[i].result_value;
	    //			opt2_slider_group[i].result_value=(double)((int)(opt2_slider_group[i].result_value*100.0)/100);
	    result_text[i].init(
		RESULT_OFFSET + RESULT_LENGTH-50,
		FIRST_LINE_OFFSET + LINE_SPACING * (i + 1),
		&(opt2_slider_group[i].result_value),24); //## chea 110899 change by b.massy add 24(1dec)
	}

	refresh_optimization_screen = 0;
    }
    else {                                          // refreshFlag == INFO_REFRESH
	vga.use_back();
	for (int i = 0; i < COST_RISE_POLICY_COUNT; i++) {
	    opt2_slider_group[i].result_flag=optimization.is_optimized[1]; {

		//			if(optimization.is_optimized[1]==1)
		opt2_slider_group[i].result_value = finance.cost_rise_policy_array[i].result_value;

		// chwg0513
		if(finance.cost_rise_policy_array[i].required_flag) {
		    opt2_slider_group[i].enable_flag=0;
		    opt2_button_group_array[i]->push(3);
		    double *importWeight = &(finance.cost_rise_policy_array[i].import_weight);
		    *importWeight = OPT_MID_WEIGHT;
		}
		//## chea 031299 11.2.3 to make the opt bar enable again this is when a bar is locked use in report and will release here
		else if(finance.cost_rise_policy_array[i].required_flag == 0 && opt2_slider_group[i].enable_flag ==0) {
		    opt2_slider_group[i].enable_flag=1;
		    opt2_button_group_array[i]->push(1);
		    double *importWeight = &(finance.cost_rise_policy_array[i].import_weight);
		    *importWeight = OPT_MID_WEIGHT;
		}

		//				else
		//				{
		//					if(opt2_button_group_array[i]->button_pressed==3)
		//							opt2_button_group_array[i]->push(1);
		//						opt2_slider_group[i].enable_flag=1;
		//				}

		//				short bltX, bltY, bltHeight;
		//				user_interface.v_line(
		//					bltX = opt2_slider_group->bounded_slider_x1 + 4 + short(double(opt2_slider_group->bar_width) * (resultValue - opt2_slider_group[i].min_val) / (opt2_slider_group[i].max_val - opt2_slider_group[i].min_val)),
		//					bltY = opt2_slider_group[i].bounded_slider_y1 - 2, bltHeight = opt2_slider_group[i].bounded_slider_y2 - opt2_slider_group[i].bounded_slider_y1 + 6, 1, V_RED);
		//				vga.blt_buf(bltX, bltY, bltX+1, bltY+bltHeight);

		//				vga.use_front();
		//				font_chartsm.right_put(
		//					RESULT_OFFSET + RESULT_LENGTH,
		//					FIRST_LINE_OFFSET + LINE_SPACING * (i + 1),
		//					m.format(resultValue) );
		result_text[i].paint();
		vga.blt_buf(
		    RESULT_OFFSET + RESULT_LENGTH-50,
		    FIRST_LINE_OFFSET + LINE_SPACING * (i + 1),
		    RESULT_OFFSET + RESULT_LENGTH,
		    FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + 16
		    );
		//
	    }
	    opt2_slider_group[i].refresh();
	}
    }
}

//---------- End of function Optimization::report_stage_2 -----//

//-------- Begin of function Optimization::detect_stage_2 -----//
//!
int Optimization::detect_stage_2() {
    if (stage_init_flag != 2)
	return 0;

    help.set_help(TARGET_OFFSET, SLIDER_OFFSET + FIRST_LINE_OFFSET,
		  TARGET_OFFSET+TARGET_WIDTH,REPORT_Y2-20,"H8.1.1.");
    help.set_help(TARGET_OFFSET, SLIDER_OFFSET + FIRST_LINE_OFFSET,
		  TARGET_OFFSET+30,REPORT_Y2-20,"H8.1.2.");
    help.set_help(TARGET_OFFSET+TARGET_WIDTH-30, SLIDER_OFFSET + FIRST_LINE_OFFSET,
		  TARGET_OFFSET+TARGET_WIDTH,REPORT_Y2-20,"H8.1.3.");
    help.set_help(  SEPARATOR_2, SLIDER_OFFSET + FIRST_LINE_OFFSET,
		    SEPARATOR_3,REPORT_Y2-20,"H8.1.4.");

    int i, j;

    vga.use_back();

    //	if((sys.view_mode!=MODE_YEAR_END_REPORT)
    //		|| optimization.is_optimized[1]==1)
    if (mouse.in_area( SEPARATOR_1, REPORT_Y1+1+HEADING_HEIGHT,
		       SEPARATOR_2, REPORT_Y2 )) {
	for (i = 0; i < COST_RISE_POLICY_COUNT; i++)
	    if(!finance.cost_rise_policy_array[i].required_flag)
		if (opt2_slider_group[i].detect()) {
		    optimization.is_optimize_slider_modified[1]=0;
		    return 1;
		}
    }

    if (mouse.single_click(SEPARATOR_2+9,REPORT_Y1+7,SEPARATOR_2+27,REPORT_Y1+25)) {
	String str;
	str  = DIR_RES;
	str += "SCORE";

	ResTxt rs;
	rs.init(str);
	image_interface.put_back(SEPARATOR_2+9,REPORT_Y1+7,"QUEST_DN");
	vga.blt_buf(SEPARATOR_2+9,REPORT_Y1+7,SEPARATOR_2+27,REPORT_Y1+25);
	sys.back_view_mode=sys.view_mode;
	help_if.help_textlist.topic_id=rs.locate_topic("Preference");
	if(help_if.help_textlist.topic_id>0) {
	    help_if.help_textlist.add_history(help_if.help_textlist.topic_id);
	    sys.set_view_mode(MODE_HELP_DESC);
	    sys.redraw_all_flag = 1;
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	}
	return 1;
    }

    vga.use_front();

    // Unlock line if the state is IMPLEMENT
    for (i = 0; i < COST_RISE_POLICY_COUNT; i++)
	if (mouse.single_click(
	    PREF_BUTTON_OFFSET4-2,
	    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1)-2,
	    PREF_BUTTON_OFFSET4 + PREF_BUTTON_WIDTH,
	    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT)) {
	    if(finance.cost_rise_policy_array[i].applied_flag!=P_IMPLEMENT)
		if(finance.cost_rise_policy_array[i].required_flag) {
		    finance.cost_rise_policy_array[i].required_flag=0;
		    opt2_slider_group[i].enable_flag=1;
		    opt2_slider_group[i].refresh();

		    //## chea 090699
		    finance.cost_rise_policy_array[i].reset_constraint_policy();
		    //## chea 090699

		    opt2_button_group_array[i]->push(1);

		    //## chea 041099 try to release the opt button
		    optimization.is_optimize_slider_modified[1]=0;

		    double *importWeight = &(finance.cost_rise_policy_array[i].import_weight);
		    *importWeight = OPT_MID_WEIGHT;
		    sys.redraw_all_flag=1;
		    return 1;
		}

	}

    if (mouse.single_click( SEPARATOR_2, REPORT_Y1+1+HEADING_HEIGHT,
			    SEPARATOR_3, REPORT_Y2 )) {
	for (i = 0; i < COST_RISE_POLICY_COUNT; i++) {

	    if(!finance.cost_rise_policy_array[i].required_flag)
		if  ( (j = opt2_button_group_array[i]->detect()) >= 0) {

		    //## chea 041099 try to release the opt button
		    optimization.is_optimize_slider_modified[1]=0;

		    // Lock the line when 'r' is pressed
		    if(j==3) {                                // addif to reset the bounds
			finance.cost_rise_policy_array[i].required_flag=1;
			opt2_slider_group[i].enable_flag=0;
			opt2_slider_group[i].refresh();

			finance.cost_rise_policy_array[i].set_constraint_policy();

			sys.redraw_all_flag=1;
		    }

		    ButtonGroup *buttonGroupPtr = opt2_button_group_array[i];
		    vga.use_back();
		    for (int k = 0; k < PREF_COUNT; k++) {
			(*buttonGroupPtr)[k].body_ptr =
			    (void *) ((k==j)?(opt2_button_group_down_bitmap[i*PREF_COUNT+k] + 4):(opt2_button_group_up_bitmap[i*PREF_COUNT+k] + 4));
		    }
		    buttonGroupPtr->paint(buttonGroupPtr->button_pressed);

		    vga.blt_buf(PREF_BUTTON_OFFSET1, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1), PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT);
		    vga.blt_buf(PREF_BUTTON_OFFSET2, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1), PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT);
		    vga.blt_buf(PREF_BUTTON_OFFSET3, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1), PREF_BUTTON_OFFSET3 + PREF_BUTTON_WIDTH, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT);
		    vga.blt_buf(PREF_BUTTON_OFFSET4, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1), PREF_BUTTON_OFFSET4 + PREF_BUTTON_WIDTH, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + 1) + PREF_BUTTON_HEIGHT);

		    double *importWeight = &(finance.cost_rise_policy_array[i].import_weight);

		    if(j==0)
			*importWeight = OPT_LOW_WEIGHT;
		    else if(j==1)
			*importWeight = OPT_MID_WEIGHT;
		    else if(j==2)
			*importWeight = OPT_HIGH_WEIGHT;

		    //					*importWeight = double(j + 1) / 2.0;
		    return 1;
		}
	}
    }

    vga.use_back();

    for (i = 0; i < COST_RISE_POLICY_COUNT; i++)
	if(help_button_array[i].detect()) {
	    String str;
	    str  = DIR_RES;
	    str += "SCORE";

	    ResTxt rs;
	    rs.init(str);

	    // ## chwg1203
	    image_interface.put_back(REPORT_X1+10,
				     FIRST_LINE_OFFSET + LINE_SPACING * (i+1),"QUEST_DN");
	    vga.blt_buf(
		REPORT_X1+10,
		FIRST_LINE_OFFSET + LINE_SPACING * (i+1),
		REPORT_X1+30,
		FIRST_LINE_OFFSET + LINE_SPACING * (i+1) + 20);

	    sys.back_view_mode=sys.view_mode;
	    help_if.help_textlist.topic_id=rs.locate_topic(game_str_res.opt_stage2_str(i));
	    if(help_if.help_textlist.topic_id>0) {
		help_if.help_textlist.add_history(help_if.help_textlist.topic_id);
		sys.set_view_mode(MODE_HELP_DESC);
		sys.redraw_all_flag = 1;
		audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    }
	    return 1;
	}

    return 0;
}

//---------- End of function Optimization::detect_stage_2 -----//
