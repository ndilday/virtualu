//Filename    : OOPT1.CPP
//Description : Optimization class - stage 1

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

//------- Define constants -------//

enum {
    REPORT_X1 = ZOOM_X1 + 15,
    REPORT_Y1 = ZOOM_Y1 + 25,
    REPORT_X2 = ZOOM_X2 - 15,
    REPORT_Y2 = ZOOM_Y2 - 60
};

enum {
    SEPARATOR_1 = REPORT_X1 + 250,
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

static Text result_text[REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT];
static Button help_button_array[REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT];

static char *title[] = {
    "Description",
    "Target",
    "      Preference",
    "Result(%)"
};

static char *heading[] = {
    "Revenue policies",
    "Cost rise and net budget change"
};

static char *pref_label[] = {
    "1", "2", "3", "R"
};

static char *optimize_label = "Optimize";

//------- Begin of function Optimization::init_stage_1 -----------//
//!
void Optimization::init_stage_1() {
    for (int i = 0; i < REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT; i++) {
	for (int j = 0; j < PREF_COUNT; j++) {
	    opt1_button_group_up_bitmap[i * PREF_COUNT + j] = NULL;
	    opt1_button_group_down_bitmap[i * PREF_COUNT + j] = NULL;
	}
    }
}

//--------- End of function Optimization::init_stage_1 -----------//

//------- Begin of function Optimization::deinit_stage_1 ---------//
//!
void Optimization::deinit_stage_1() {
    for (int i = 0; i < REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT; i++) {
	delete opt1_button_group_array[i];

	if(opt1_button_group_array[i]) {
	    opt1_button_group_array[i]=NULL;
	}

	opt1_slider_group[i].deinit();

	for (int j = 0; j < PREF_COUNT; j++) {
	    mem_del(opt1_button_group_up_bitmap[i * PREF_COUNT + j]);
	    mem_del(opt1_button_group_down_bitmap[i * PREF_COUNT + j]);
	    opt1_button_group_up_bitmap[i * PREF_COUNT + j] = NULL;
	    opt1_button_group_down_bitmap[i * PREF_COUNT + j] = NULL;
	}

	//		opt1_button_group_array[i]->button_pressed=2;
	result_text[i].deinit();
	// ###### begin Gilbert 28/04/2001 #######//
	help_button_array[i].deinit();
	// ###### end Gilbert 28/04/2001 #######//
    }
    stage_init_flag = 0;
}

//--------- End of function Optimization::deinit_stage_1 -------//

//------- Begin of function Optimization::report_stage_1 -------//
//!
void Optimization::report_stage_1(int refreshFlag) {
    if (refreshFlag == INFO_REPAINT || refresh_optimization_screen ) {
	user_interface.brighten(REPORT_X1+100, ZOOM_Y1, REPORT_X2-100, REPORT_Y1);
	user_interface.brighten(REPORT_X1+4, REPORT_Y1+4, REPORT_X2-2, REPORT_Y2-2);
	user_interface.rect(REPORT_X1+100, ZOOM_Y1, REPORT_X2-100, REPORT_Y1);
	//		font_charts.center_put( REPORT_X1, ZOOM_Y1, REPORT_X2, REPORT_Y1, "Stage 1 : Overall Revenue and Expenditure" );
	font_charts.center_put( REPORT_X1, ZOOM_Y1, REPORT_X2, REPORT_Y1, "Revenue and Expenditure" );

	// current stage not init'ed
	if (stage_init_flag != 1 || refresh_optimization_screen ) {
	    deinit();
	    ButtonGroup *buttonGroupPtr;
	    double *targetValue, *lowerBound, *upperBound, *importWeight, lowerLimit, upperLimit;
	    int pressedButton;

	    for (int i = 0; i < REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT; i++) {
		opt1_button_group_array[i] = new ButtonGroup(PREF_COUNT);

		//				if(finance.revenue_policy_array[i].target_value >= finance.revenue_policy_array[i].upper_limit)
		//					finance.revenue_policy_array[i].target_value = finance.revenue_policy_array[i].upper_limit;
		//				else if(finance.revenue_policy_array[i].target_value <= finance.revenue_policy_array[i].lower_limit)
		//					finance.revenue_policy_array[i].target_value = finance.revenue_policy_array[i].lower_limit;

		//##### begin fred 980904#####//
		targetValue = (i < REVENUE_POLICY_COUNT) ? &(finance.revenue_policy_array[i].target_value) : &(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].target_value);
		lowerBound = (i < REVENUE_POLICY_COUNT) ? &(finance.revenue_policy_array[i].lower_bound) : &(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].lower_bound);
		upperBound = (i < REVENUE_POLICY_COUNT) ? &(finance.revenue_policy_array[i].upper_bound) : &(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].upper_bound);
		importWeight = (i < REVENUE_POLICY_COUNT) ? &(finance.revenue_policy_array[i].import_weight) : &(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].import_weight);
		lowerLimit = (i < REVENUE_POLICY_COUNT) ? (finance.revenue_policy_array[i].lower_limit) : (finance.expense_policy_array[i - REVENUE_POLICY_COUNT].lower_limit);
		upperLimit = (i < REVENUE_POLICY_COUNT) ? (finance.revenue_policy_array[i].upper_limit) : (finance.expense_policy_array[i - REVENUE_POLICY_COUNT].upper_limit);

		//				opt1_slider_group[i].init(
		//					TARGET_OFFSET, SLIDER_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)),
		//					TARGET_WIDTH, TARGET_HEIGHT, targetValue, lowerBound, upperBound, *targetValue-(*targetValue-*lowerBound)*2, *targetValue+(*upperBound-*targetValue)*2);
		if ( *targetValue > *upperBound )
		    *targetValue = *upperBound;

		opt1_slider_group[i].init(
		    TARGET_OFFSET, SLIDER_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)),
		    TARGET_WIDTH, TARGET_HEIGHT, targetValue, lowerBound, upperBound, lowerLimit, upperLimit,
		    1,V_YELLOW,V_GREEN,24);                 //## chea 110809 change by b.massy
		//##### end fred 980904#####//

		if ( i == 0 ) {
		    if ( *upperBound < 5.0 )
			opt1_slider_group[i].lock_max = 1;
		}

		// lock indirect cost rate's upper bound

		if( i == PL_INDIRECT_COST_RATE ) {
		    opt1_slider_group[i].lock_max = 1;
		}

		user_interface.create_button_bitmap(
		    PREF_BUTTON_OFFSET1-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))-2,
		    PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT,
		    pref_label[0], &opt1_button_group_up_bitmap[PREF_COUNT * i], &opt1_button_group_down_bitmap[PREF_COUNT * i], &vga_back);
		user_interface.create_button_bitmap(
		    PREF_BUTTON_OFFSET2-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))-2,
		    PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT,
		    pref_label[1], &opt1_button_group_up_bitmap[PREF_COUNT * i + 1], &opt1_button_group_down_bitmap[PREF_COUNT * i + 1], &vga_back);
		user_interface.create_button_bitmap(
		    PREF_BUTTON_OFFSET3-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))-2,
		    PREF_BUTTON_OFFSET3 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT,
		    pref_label[2], &opt1_button_group_up_bitmap[PREF_COUNT * i + 2], &opt1_button_group_down_bitmap[PREF_COUNT * i + 2], &vga_back);
		user_interface.create_button_bitmap(
		    PREF_BUTTON_OFFSET4-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))-2,
		    PREF_BUTTON_OFFSET4 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT,
		    pref_label[3], &opt1_button_group_up_bitmap[PREF_COUNT * i + 3], &opt1_button_group_down_bitmap[PREF_COUNT * i + 3], &vga_back);
		buttonGroupPtr = opt1_button_group_array[i];

		//				pressedButton = int(*importWeight * 2.0) - 1;
		if(*importWeight==OPT_LOW_WEIGHT)
		    pressedButton =0;
		if(*importWeight==OPT_MID_WEIGHT)
		    pressedButton =1;
		if(*importWeight==OPT_HIGH_WEIGHT)
		    pressedButton =2;

		if(((i < REVENUE_POLICY_COUNT) ?
		    (finance.revenue_policy_array[i].required_flag) :
		    (finance.expense_policy_array[i - REVENUE_POLICY_COUNT].required_flag) )) {
		    pressedButton = 3;
		    opt1_slider_group[i].enable_flag=0;
		}
		//				else
		//				{
		//					if(pressedButton==3)
		//						pressedButton = 1;
		//					opt1_slider_group[i].enable_flag=1;
		//				}

		(*buttonGroupPtr)[0].create_bitmapW(
		    PREF_BUTTON_OFFSET1-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))-2,
		    PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT,
		    ((pressedButton == 0) ? opt1_button_group_down_bitmap[PREF_COUNT * i] : opt1_button_group_up_bitmap[PREF_COUNT * i]) + 4);
		(*buttonGroupPtr)[1].create_bitmapW(
		    PREF_BUTTON_OFFSET2-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))-2,
		    PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT,
		    ((pressedButton == 1) ? opt1_button_group_down_bitmap[PREF_COUNT * i + 1] : opt1_button_group_up_bitmap[PREF_COUNT * i + 1]) + 4);
		(*buttonGroupPtr)[2].create_bitmapW(
		    PREF_BUTTON_OFFSET3-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))-2,
		    PREF_BUTTON_OFFSET3 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT,
		    ((pressedButton == 2) ? opt1_button_group_down_bitmap[PREF_COUNT * i + 2] : opt1_button_group_up_bitmap[PREF_COUNT * i + 2]) + 4);
		(*buttonGroupPtr)[3].create_bitmapW(
		    PREF_BUTTON_OFFSET4-2,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))-2,
		    PREF_BUTTON_OFFSET4 + PREF_BUTTON_WIDTH,
		    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT,
		    ((pressedButton == 3) ? opt1_button_group_down_bitmap[PREF_COUNT * i + 3] : opt1_button_group_up_bitmap[PREF_COUNT * i + 3]) + 4);

		buttonGroupPtr->button_pressed = pressedButton;

		if(user_interface.backgd_flag==1)
		    help_button_array[i].create_text(REPORT_X1+10,
						     FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)),
						     REPORT_X1+28,
						     FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))+18,
						     "?");
	    }

	    stage_init_flag = 1;
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

	//		font_chartsm.put( DESCRIPTION_OFFSET, FIRST_LINE_OFFSET, heading[0]);
	//		font_chartsm.put( DESCRIPTION_OFFSET, FIRST_LINE_OFFSET + (REVENUE_POLICY_COUNT + 1) * LINE_SPACING, heading[1]);

	for (int i = 0; i < REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT; i++) {
	    font_chartsm.put(
		DESCRIPTION_OFFSET + DESCRIPTION_INDENT,
		FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)),
		game_str_res.opt_stage1_str(i) );
	    if(user_interface.backgd_flag==1)
		image_interface.put_back(REPORT_X1+10,
					 FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))
					 ,"QUEST_UP");

	    opt1_slider_group[i].paint();
	    opt1_button_group_array[i]->paint(opt1_button_group_array[i]->button_pressed);
	    opt1_slider_group[i].result_flag=optimization.is_optimized[0];
	    opt1_slider_group[i].result_value = (i < REVENUE_POLICY_COUNT) ? finance.revenue_policy_array[i].result_value : finance.expense_policy_array[i - REVENUE_POLICY_COUNT].result_value;
	    //			opt1_slider_group[i].result_value=(double)((int)(opt1_slider_group[i].result_value*100.0)/100);
	    result_text[i].init(
		RESULT_OFFSET + RESULT_LENGTH - 50,
		FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)),
		&(opt1_slider_group[i].result_value),24); //## chea 110899 change by b.massy add 24(1dec)
	}

	refresh_optimization_screen = 0;
    }
    else {                                          // refreshFlag == INFO_REFRESH
	vga.use_back();
	for (int i = 0; i < REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT; i++) {
	    opt1_slider_group[i].result_flag=optimization.is_optimized[0]; {
		//			if (optimization.is_optimized[0]==1)
		opt1_slider_group[i].result_value = (i < REVENUE_POLICY_COUNT) ? finance.revenue_policy_array[i].result_value : finance.expense_policy_array[i - REVENUE_POLICY_COUNT].result_value;

		// chwg0513
		if(i < REVENUE_POLICY_COUNT) {
		    if(finance.revenue_policy_array[i].required_flag) {
			opt1_slider_group[i].enable_flag=0;
			opt1_button_group_array[i]->push(3);
			double *importWeight = &(finance.revenue_policy_array[i].import_weight);
			*importWeight = OPT_MID_WEIGHT;
		    }
		    //## chea 031299 11.2.3 to make the opt bar enable again this is when a bar is locked use in report and will release here
		    else if(finance.revenue_policy_array[i].required_flag == 0 && opt1_slider_group[i].enable_flag ==0) {
			opt1_slider_group[i].enable_flag=1;
			opt1_button_group_array[i]->push(1);
			double *importWeight = &(finance.revenue_policy_array[i].import_weight);
			*importWeight = OPT_MID_WEIGHT;
		    }

		    //					else
		    //					{
		    //						if(opt1_button_group_array[i]->button_pressed==3)
		    //							opt1_button_group_array[i]->push(1);
		    //
		    //						opt1_slider_group[i].enable_flag=1;
		    //					}
		}
		else {
		    if(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].required_flag) {
			opt1_slider_group[i].enable_flag=0;
			opt1_button_group_array[i]->push(3);
			double *importWeight = &(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].import_weight);
			*importWeight = OPT_MID_WEIGHT;
		    }
		    //## chea 031299 11.2.3 to make the opt bar enable again this is when a bar is locked use in report and will release here
		    else if(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].required_flag == 0 && opt1_slider_group[i].enable_flag ==0) {
			opt1_slider_group[i].enable_flag=1;
			opt1_button_group_array[i]->push(1);
			double *importWeight = &(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].import_weight);
			*importWeight = OPT_MID_WEIGHT;
		    }

		    //					else
		    //					{	if(opt1_button_group_array[i]->button_pressed==3)
		    //							opt1_button_group_array[i]->push(1);
		    //						opt1_slider_group[i].enable_flag=1;
		    //					}
		}

		result_text[i].paint();
		vga.blt_buf(RESULT_OFFSET + RESULT_LENGTH - 50,
			    FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)),
			    RESULT_OFFSET + RESULT_LENGTH,
			    FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))+16);
	    }
	    opt1_slider_group[i].refresh();
	}
    }
}

//---------- End of function Optimization::report_stage_1 ------//

//-------- Begin of function Optimization::detect_stage_1 ------//
//!
int Optimization::detect_stage_1() {
    if (stage_init_flag != 1)
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

    //--------- detect sliders ---------//

    if (mouse.in_area( SEPARATOR_1, REPORT_Y1+1+HEADING_HEIGHT,
		       SEPARATOR_2, REPORT_Y2 )) {
	for (i = 0; i < REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT; i++) {
	    if(!((i < REVENUE_POLICY_COUNT) ?
		 (finance.revenue_policy_array[i].required_flag) :
		 (finance.expense_policy_array[i - REVENUE_POLICY_COUNT].required_flag))) {
		if (opt1_slider_group[i].detect()) {
		    optimization.is_optimize_slider_modified[0]=0;
		    return 1;
		}
	    }
	}
    }

    //-------- detect "Preference Help" button ---------//

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

    //--------- if click on a pushed "R" to reset it ----------//

    // Unlock line if the state is IMPLEMENT
    for (i = 0; i <REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT; i++) {
	if (mouse.single_click(
	    PREF_BUTTON_OFFSET4-2,
	    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))-2,
	    PREF_BUTTON_OFFSET4 + PREF_BUTTON_WIDTH,
	    PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT
	    )) {
	    //------ click on a revenue item --------//

	    if(i < REVENUE_POLICY_COUNT) {
		if( finance.revenue_policy_array[i].applied_flag!=P_IMPLEMENT ) {
		    if(finance.revenue_policy_array[i].required_flag) {
			finance.revenue_policy_array[i].required_flag=0;
			opt1_slider_group[i].enable_flag=1;
			opt1_slider_group[i].refresh();

			finance.revenue_policy_array[i].reset_constraint_policy();

			opt1_button_group_array[i]->push(1);

			//## chea 041099 try to release the opt button
			optimization.is_optimize_slider_modified[0]=0;

			double *importWeight = &(finance.revenue_policy_array[i].import_weight);
			*importWeight = OPT_MID_WEIGHT;

			sys.redraw_all_flag=1;
			return 1;
		    }
		}
	    }
	    else {                                      //------ click on an expense item --------//
		if(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].applied_flag!=P_IMPLEMENT) {
		    if(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].required_flag) {
			finance.expense_policy_array[i - REVENUE_POLICY_COUNT].required_flag=0;
			opt1_slider_group[i].enable_flag=1;
			opt1_slider_group[i].refresh();
			//## fred 990528
			finance.expense_policy_array[i-REVENUE_POLICY_COUNT].reset_constraint_policy();
			//##
			opt1_button_group_array[i]->push(1);

			//## chea 041099 try to release the opt button
			optimization.is_optimize_slider_modified[0]=0;

			double *importWeight = &(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].import_weight);
			*importWeight = OPT_MID_WEIGHT;
			sys.redraw_all_flag=1;
			return 1;
		    }
		}
	    }
	}
    }

    //----------- click on Preference 1, 2, 3 or R --------------//

    if (mouse.single_click( SEPARATOR_2, REPORT_Y1+1+HEADING_HEIGHT,
			    SEPARATOR_3, REPORT_Y2 )) {
	for (i = 0; i < REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT; i++) {
	    if(!((i < REVENUE_POLICY_COUNT) ?
		 (finance.revenue_policy_array[i].required_flag) :
		 (finance.expense_policy_array[i - REVENUE_POLICY_COUNT].required_flag))) {
		if  ( (j = opt1_button_group_array[i]->detect()) >= 0) {
		    //## chea 041099 try to release the opt button
		    optimization.is_optimize_slider_modified[0]=0;

		    if(j==3) {
			if(i < REVENUE_POLICY_COUNT) {
			    finance.revenue_policy_array[i].required_flag=1;
			    finance.revenue_policy_array[i].set_constraint_policy();
			}
			else {
			    finance.expense_policy_array[i - REVENUE_POLICY_COUNT].required_flag=1;
			    finance.expense_policy_array[i - REVENUE_POLICY_COUNT].set_constraint_policy();
			}
			opt1_slider_group[i].enable_flag=0;
			opt1_slider_group[i].refresh();

			sys.redraw_all_flag=1;
		    }

		    ButtonGroup *buttonGroupPtr = opt1_button_group_array[i];
		    vga.use_back();
		    for (int k = 0; k < PREF_COUNT; k++) {
			(*buttonGroupPtr)[k].body_ptr =
			    (void *) ((k==j)?(opt1_button_group_down_bitmap[i*PREF_COUNT+k] + 4):(opt1_button_group_up_bitmap[i*PREF_COUNT+k] + 4));
		    }
		    buttonGroupPtr->paint(buttonGroupPtr->button_pressed);
		    vga.blt_buf(PREF_BUTTON_OFFSET1, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)), PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT);
		    vga.blt_buf(PREF_BUTTON_OFFSET2, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)), PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT);
		    vga.blt_buf(PREF_BUTTON_OFFSET3, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)), PREF_BUTTON_OFFSET3 + PREF_BUTTON_WIDTH, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT);
		    vga.blt_buf(PREF_BUTTON_OFFSET4, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)), PREF_BUTTON_OFFSET4 + PREF_BUTTON_WIDTH, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT);

		    double *importWeight = (i < REVENUE_POLICY_COUNT) ? &(finance.revenue_policy_array[i].import_weight) : &(finance.expense_policy_array[i - REVENUE_POLICY_COUNT].import_weight);
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
    }

    //------- click on the help of an item ---------//

    vga.use_back();

    for (i = 0; i < REVENUE_POLICY_COUNT + EXPENSE_POLICY_COUNT; i++) {
	if(help_button_array[i].detect()) {
	    String str;
	    str  = DIR_RES;
	    str += "SCORE";

	    ResTxt rs;
	    rs.init(str);

	    // ## chwg1203 begin ////////////////////////
	    image_interface.put_back(REPORT_X1+10,
				     FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))
				     ,"QUEST_DN");
	    vga.blt_buf(
		REPORT_X1+10,
		FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)),
		REPORT_X1+30,
		FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1))+20
		);
	    sys.back_view_mode=sys.view_mode;
	    help_if.help_textlist.topic_id=rs.locate_topic(game_str_res.opt_stage1_str(i));

	    if(help_if.help_textlist.topic_id>0) {
		help_if.help_textlist.add_history(help_if.help_textlist.topic_id);
		sys.set_view_mode(MODE_HELP_DESC);
		sys.redraw_all_flag = 1;
		audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    }
	    return 1;
	}
    }

    return 0;
}

//---------- End of function Optimization::detect_stage_1 ------//
