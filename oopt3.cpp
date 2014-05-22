//Filename    : OOPT3.CPP
//Description : OptStage3 class

#include <OSYS.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OOPT3.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OFINANCE.H>
#include <OFONT.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OTEXT.H>
#include <OIFACE.H>
#include <OHELPIF.H>
#include <OBNDSLDR.H>
#include <OVALSLDR.H>
#include <OHELP.H>
#include <OOPT.H>
#include <OAUDIO.H>
#include <OCONFIG.H>
#include <OPSCHOOL.H>
//------- Define constants -------//

enum {
    REPORT_X1 = ZOOM_X1 + 15,
    REPORT_Y1 = ZOOM_Y1 + 25,
    REPORT_X2 = ZOOM_X2 - 15,
    REPORT_Y2 = ZOOM_Y2 - 105
};

enum {  SEPARATOR_1 = REPORT_X1 + 200 };
enum {  SEPARATOR_2 = REPORT_X1 + 330 };
enum {  SEPARATOR_3 = REPORT_X1 + 420 };
enum {  SEPARATOR_4 = REPORT_X1 + 650 };
enum {  SEPARATOR_5 = REPORT_X1 + 650 };
enum {  SEPARATOR_6 = REPORT_X1 + 715 };
enum {  SEPARATOR_7 = REPORT_X1 + 750 };

enum {
    LINE_SPACING = 21,
    HEADING_HEIGHT = 21,
    FIRST_LINE_OFFSET = REPORT_Y1 + HEADING_HEIGHT + 14,
    DESCRIPTION_OFFSET = REPORT_X1 + 10,
    EXISTING_FACULTY_OFFSET = (SEPARATOR_1 + SEPARATOR_2)/2 -10,
    SALARY_OFFSET = (SEPARATOR_2 + SEPARATOR_3)/2 -10,
    SLIDER_OFFSET = 1,
    TARGET_OFFSET = SEPARATOR_3 + 20,
    TARGET_WIDTH = SEPARATOR_4 - SEPARATOR_3 - 35,
    TARGET_HEIGHT = 6,
    FACULTY_OFFSET_X = SEPARATOR_2 + 20,
    FACULTY_OFFSET_Y = -1,
    FACULTY_WIDTH = 14,
    FACULTY_HEIGHT = 14,
    PREF_BUTTON_WIDTH = 16,
    PREF_BUTTON_HEIGHT = 16,
    PREF_BUTTON_DISTANCE = 10,
    PREF_BUTTON_OFFSET = 0,
    PREF_BUTTON_OFFSET1 = SEPARATOR_4+17,
    PREF_BUTTON_OFFSET2 = PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH + PREF_BUTTON_DISTANCE,
    PREF_BUTTON_OFFSET3 = PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH + PREF_BUTTON_DISTANCE,
    REMAIN_MESSAGE_X = (REPORT_X2-REPORT_X1-400)/2+DESCRIPTION_OFFSET,
    REMAIN_MESSAGE_Y1 = REPORT_Y2 + 5,
    REMAIN_MESSAGE_Y2 = REPORT_Y2 + 22,
    REMAIN_MESSAGE_Y3 = REPORT_Y2 + 39,
    REMAIN_X = REMAIN_MESSAGE_X + 350,
    REMAIN_Y1 = REMAIN_MESSAGE_Y1,
    REMAIN_Y2 = REMAIN_MESSAGE_Y2,
    REMAIN_Y3 = REMAIN_MESSAGE_Y3,
    REMAIN_CLEAR = 20 ,
    RESULT_OFFSET = SEPARATOR_5 + 28,
    RESULT_LENGTH = 50
};

enum {  BG_PIC_ID = 3 };

//-------- define static vars -----------//

static char *title[] = {
    "Department",
    "Existing faculty",
    "Salary",
    "Target hires and upper limit",
    "Preference",
    "Result",
};

static char *pref_label[] = {
    "1", "2", "3"
};

//static char *remain_msg1 = "Total number of faculty you can still hire : ";
static char *remain_msg1 = "Number of budgeted new faculty positions : ";
static char *remain_msg2 = "Sum of departmental hiring targets : ";
static char *remain_msg3 = "Maximum faculty to be hired : ";
static char *optimize_label = "Optimize";

//***** Temporary *****//
static char *heading[] = {
    "Department 1",
    "Department 2",
    "Department 3",
    "Department 4",
    "Department 5",
    "Department 6",
    "Department 7",
    "Department 8",
    "Department 9",
    "Department 10",
    "Department 11",
    "Department 12",
};
//*********************//

static Text opt3_text1;
static Text opt3_text2;
static Text opt3_text3;
static Text opt3_data1[MAX_DEPARTMENT];
static Text opt3_data2[MAX_DEPARTMENT];
//static BoundedSlider opt3_slider_group[MAX_DEPARTMENT];
static ValueSlider value_slider1;
static int v1[MAX_DEPARTMENT];
static float v2[MAX_DEPARTMENT];
static int v3[MAX_DEPARTMENT];
static int v4[MAX_DEPARTMENT];
static int v5[MAX_DEPARTMENT];
static double targetValue[MAX_DEPARTMENT];
static double upperBound[MAX_DEPARTMENT];
static double lowerBound;

static Button help_button;
static short *help_button_up_bitmap;
static short *help_button_down_bitmap;

static Text result_text_opt3[MAX_DEPARTMENT];
//------- Begin of function OptStage3::OptStage3 -----------//
//!
OptStage3::OptStage3() {
}

//--------- End of function OptStage3::OptStage3 -----------//

//------- Begin of function OptStage3::~OptStage3 -----------//
//!
OptStage3::~OptStage3() {
}

//--------- End of function OptStage3::~OptStage3 -----------//

//------- Begin of function OptStage3::report ---------------//
//!
void OptStage3::report(int refreshFlag) {
}

//---------- End of function OptStage3::report ------------//

//-------- Begin of function OptStage3::detect ------------//
//!
void OptStage3::detect() {
}

//---------- End of function OptStage3::detect ------------//

//-------- Begin of function OptStage3::draw_count_bar ------------//
//!
//! This function is useless now ## chwg 1215
void OptStage3::draw_count_bar(int rowId) {
}

//---------- End of function OptStage3::draw_count_bar ------------//

//-------- Begin of function OptStage3::detect_count_bar ----------//
//!
int OptStage3::detect_count_bar(int rowId) {
    return -1;
}

//---------- End of function OptStage3::detect_count_bar ----------//

//------- Begin of function Optimization::init_stage_3 -----------//
//!
void Optimization::init_stage_3() {
    //***** temporary *****//
    for (int i = 0; i < MAX_DEPARTMENT; i++) {
	pref[i] = 1;
	existing[i] = i + 2;
	new_hire[i] = 10 - i;
    }
    //*********************//
}

//--------- End of function Optimization::init_stage_3 -----------//

//------- Begin of function Optimization::deinit_stage_3 ---------//
//!
void Optimization::deinit_stage_3() {
    for (int i = 0; i < MAX_DEPARTMENT; i++) {
	delete opt3_button_group_array[i];

	if(opt3_button_group_array[i]) {
	    opt3_button_group_array[i]=NULL;
	}
	opt3_slider_group[i].deinit();

	for (int j = 0; j < PREF_COUNT; j++) {
	    if(opt3_button_group_up_bitmap[i * PREF_COUNT + j]) {
		mem_del(opt3_button_group_up_bitmap[i * PREF_COUNT + j]);
		mem_del(opt3_button_group_down_bitmap[i * PREF_COUNT + j]);
		opt3_button_group_up_bitmap[i * PREF_COUNT + j] = NULL;
		opt3_button_group_down_bitmap[i * PREF_COUNT + j] = NULL;
	    }
	}
	opt3_data1[i].deinit();
	opt3_data2[i].deinit();
	result_text_opt3[i].deinit();
    }
    opt3_text1.deinit();
    opt3_text2.deinit();
    opt3_text3.deinit();
    value_slider1.deinit();
    opt3_text3.deinit();
    //##### begin Gilbert 21/04/2001 #####//
    help_button.deinit();
    //##### end Gilbert 21/04/2001 #####//
    stage_init_flag = 0;
}

//--------- End of function Optimization::deinit_stage_3 -------//

//------- Begin of function Optimization::report_stage_3 ---------------//
//!
void Optimization::report_stage_3(int refreshFlag) {
    Department *deptPtr;
    int i;
    int dept_sum;

    if(department_array.size()>0)
	line_spacing=((REPORT_Y2-REPORT_Y1-HEADING_HEIGHT)/department_array.size())-1;
    else
	line_spacing=LINE_SPACING;

    finance.calc_max_hires();                       //## chea test;
    remain_hire1 = finance.max_hires;
    remain_hire2 = 0;
    remain_hire3 = finance.total_hires;
    dept_sum = 0;
    for (i =department_array.size();i>0;i--) {
	if (department_array.is_deleted(i))
	    continue;
	deptPtr = department_array[i];

	v1[i-1]=finance.hiring_policy_array[i-1].fac_count;
	v2[i-1]=finance.hiring_policy_array[i-1].avg_salary;
	remain_hire2 += finance.hiring_policy_array[i-1].target_value;
    }

    if (refreshFlag == INFO_REPAINT) {
	// show the title
	user_interface.brighten(REPORT_X1+100, ZOOM_Y1, REPORT_X2-100, REPORT_Y1);
	user_interface.brighten(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y2+55);
	user_interface.rect(REPORT_X1+100, ZOOM_Y1, REPORT_X2-100, REPORT_Y1);
	font_charts.center_put( REPORT_X1, ZOOM_Y1, REPORT_X2, REPORT_Y1, "Faculty Hiring" );

	// show the frame outline
	//		user_interface.bg_img(BG_PIC_ID, &vga_back);

	user_interface.h_line(REPORT_X1, REPORT_Y1+HEADING_HEIGHT, REPORT_X2-REPORT_X1);
	user_interface.v_line(SEPARATOR_1-1, REPORT_Y1+1, REPORT_Y2-REPORT_Y1-2);
	user_interface.v_line(SEPARATOR_2-1, REPORT_Y1+1, REPORT_Y2-REPORT_Y1-2);
	user_interface.v_line(SEPARATOR_3-1, REPORT_Y1+1, REPORT_Y2-REPORT_Y1-2);
	//		user_interface.v_line(SEPARATOR_4-1, REPORT_Y1+1, REPORT_Y2-REPORT_Y1-2);
	user_interface.v_line(SEPARATOR_5-1, REPORT_Y1+1, REPORT_Y2-REPORT_Y1-2);
	user_interface.rect(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y2, 2);
	user_interface.rect(REPORT_X1, REPORT_Y2, REPORT_X2, REPORT_Y2+55, 2);

	font_chartsm.center_put( REPORT_X1+1, REPORT_Y1+1,
				 SEPARATOR_1-1, REPORT_Y1+1+HEADING_HEIGHT, title[0] );
	font_chartsm.center_put( SEPARATOR_1+1, REPORT_Y1+1,
				 SEPARATOR_2-1, REPORT_Y1+1+HEADING_HEIGHT, title[1] );
	font_chartsm.center_put( SEPARATOR_2+1, REPORT_Y1+1,
				 SEPARATOR_3-1, REPORT_Y1+1+HEADING_HEIGHT, title[2] );
	font_chartsm.center_put( SEPARATOR_3+1, REPORT_Y1+1,
				 SEPARATOR_4-1, REPORT_Y1+1+HEADING_HEIGHT, title[3] );
	//		font_chartsm.center_put( SEPARATOR_3+1, REPORT_Y1+1,
	//			SEPARATOR_5-1, REPORT_Y1+1+HEADING_HEIGHT, title[4] );
	font_chartsm.center_put( SEPARATOR_5+1, REPORT_Y1+1,
				 SEPARATOR_6-1, REPORT_Y1+1+HEADING_HEIGHT, title[5] );

	//		if(user_interface.backgd_flag==1)
	//			image_interface.put_back(SEPARATOR_4+5,REPORT_Y1+3,"QUEST_UP");

	for (i =department_array.size();i>0;i--) {
	    if (department_array.is_deleted(i))
		continue;
	    deptPtr = department_array[i];
	    font_chartsm.center_put( DESCRIPTION_OFFSET,
				     FIRST_LINE_OFFSET + line_spacing * (i-1),
				     SEPARATOR_1-1,
				     FIRST_LINE_OFFSET + line_spacing * (i-1) + 14,
				     department_res[deptPtr->department_id]->name);
	}

	font_chartsm.put( REMAIN_MESSAGE_X, REMAIN_MESSAGE_Y1, remain_msg1 );
	font_chartsm.put( REMAIN_MESSAGE_X, REMAIN_MESSAGE_Y2, remain_msg2 );
	font_chartsm.put( REMAIN_MESSAGE_X, REMAIN_MESSAGE_Y3, remain_msg3 );

	// show the preference's buttons
	if (stage_init_flag != 3) {                   // current stage not init'ed
	    // ##### begin Gilbert 23/04/2001 ###### //
	    deinit();                                   // for free memory
	    // ##### end Gilbert 23/04/2001 ###### //

	    for (i = 0; i < department_array.size(); i++) {
		targetValue[i] = (double)finance.hiring_policy_array[i].target_value;
		//121099 BUGHERE
		upperBound[i] = (double)finance.hiring_policy_array[i].upper_bound;
	    }

	    /*			deinit();
				ButtonGroup *button_ref;
				int pressedButton;

				for (i = 0; i < department_array.size(); i++)
				{
				opt3_button_group_array[i] = new ButtonGroup(PREF_COUNT);
				targetValue[i] = (double)finance.hiring_policy_array[i].target_value;
				upperBound[i] = (double)finance.hiring_policy_array[i].upper_bound;  //121099 BUGHERE
				pref[i] = finance.hiring_policy_array[i].weight;

				user_interface.create_button_bitmap(
				PREF_BUTTON_OFFSET1-2,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i-2,
				PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i + PREF_BUTTON_HEIGHT,
				pref_label[0], &opt3_button_group_up_bitmap[PREF_COUNT * i], &opt3_button_group_down_bitmap[PREF_COUNT * i], &vga_back);
				user_interface.create_button_bitmap(
				PREF_BUTTON_OFFSET2-2,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i-2,
				PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i + PREF_BUTTON_HEIGHT,
				pref_label[1], &opt3_button_group_up_bitmap[PREF_COUNT * i + 1], &opt3_button_group_down_bitmap[PREF_COUNT * i + 1], &vga_back);
				user_interface.create_button_bitmap(
				PREF_BUTTON_OFFSET3-2,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i-2,
				PREF_BUTTON_OFFSET3 + PREF_BUTTON_WIDTH,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i + PREF_BUTTON_HEIGHT,
				pref_label[2], &opt3_button_group_up_bitmap[PREF_COUNT * i + 2], &opt3_button_group_down_bitmap[PREF_COUNT * i + 2], &vga_back);

				button_ref = opt3_button_group_array[i];
				//				pressedButton = (int)(pref[i]*2.0-1.0);
				if(pref[i]==OPT_LOW_WEIGHT)
				pressedButton =0;
				if(pref[i]==OPT_MID_WEIGHT)
				pressedButton =1;
				if(pref[i]==OPT_HIGH_WEIGHT)
				pressedButton =2;

				(*button_ref)[0].create_bitmapW(
				PREF_BUTTON_OFFSET1-2,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i-2,
				PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i + PREF_BUTTON_HEIGHT,
				((pressedButton == 0) ? opt3_button_group_down_bitmap[PREF_COUNT * i] : opt3_button_group_up_bitmap[PREF_COUNT * i]) + 4);
				(*button_ref)[1].create_bitmapW(
				PREF_BUTTON_OFFSET2-2,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i-2,
				PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i + PREF_BUTTON_HEIGHT,
				((pressedButton == 1) ? opt3_button_group_down_bitmap[PREF_COUNT * i+1] : opt3_button_group_up_bitmap[PREF_COUNT * i+1]) + 4);
				(*button_ref)[2].create_bitmapW(
				PREF_BUTTON_OFFSET3-2,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i-2,
				PREF_BUTTON_OFFSET3 + PREF_BUTTON_WIDTH,
				PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i + PREF_BUTTON_HEIGHT,
				((pressedButton == 2) ? opt3_button_group_down_bitmap[PREF_COUNT * i+2] : opt3_button_group_up_bitmap[PREF_COUNT * i+2]) + 4);

				button_ref->button_pressed = pressedButton;
				}
	    */
	    if(user_interface.backgd_flag==1)
		help_button.create_text(
		    REMAIN_MESSAGE_X-22,
		    REMAIN_Y1,
		    REMAIN_MESSAGE_X-8,
		    REMAIN_Y1+18,
		    "?");

	    stage_init_flag = 3;
	}

	// ##### begin Gilbert 21/04/2001 ###### //
	err_when( department_array.size() > MAX_DEPARTMENT );
	// ##### end Gilbert 21/04/2001 ###### //

	// show the data
	for (i = 0; i < department_array.size(); i++) {
	    opt3_data1[i].put(EXISTING_FACULTY_OFFSET, FIRST_LINE_OFFSET + line_spacing * i,&v1[i],4);
	    opt3_data2[i].put(SALARY_OFFSET, FIRST_LINE_OFFSET + line_spacing * i,&v2[i],4);

	    //##### begin chwong 981215#####//

	    opt3_slider_group[i].init(
		TARGET_OFFSET,
		SLIDER_OFFSET + FIRST_LINE_OFFSET + line_spacing * i + 2,
		TARGET_WIDTH,
		line_spacing-15,
		//					&(targetValue[i]), NULL, &(upperBound[i]),0,10,  //121099 BUGHERE
		&(targetValue[i]), NULL, &(upperBound[i]),0,10,
		1,
		//					vga.active_buf->color_up,
		//					vga.active_buf->color_dark,
		V_YELLOW,
		V_GREEN,
		9);
	    if(user_interface.backgd_flag==1)
		image_interface.put_back(REMAIN_MESSAGE_X-27,
					 REMAIN_Y1,"QUEST_UP");
	    //			opt3_button_group_array[i]->paint(opt3_button_group_array[i]->button_pressed);
	    opt3_slider_group[i].paint();

	    result_text_opt3[i].init(
		RESULT_OFFSET + RESULT_LENGTH-50,
		FIRST_LINE_OFFSET + line_spacing * i,
		&(opt3_slider_group[i].result_value));

	}
	opt3_text1.put( REMAIN_X, REMAIN_Y1, &remain_hire1);
	opt3_text2.put( REMAIN_X, REMAIN_Y2, &remain_hire2);
	for (i =department_array.size();i>0;i--) {
	    if (department_array.is_deleted(i))
		continue;
	    //BUGHERE
	    //			if(finance.hiring_policy_array[i-1].upper_bound==0)
	    dept_sum += 20;
	    //				else
	    //					dept_sum += (int)finance.hiring_policy_array[i-1].upper_bound;
	}

	if( sys.view_mode == MODE_YEAR_END_REPORT ) { // only allow setting the value in optimization
	    value_slider1.init(
		REMAIN_X-45, REMAIN_Y3-1,
		100,15,0,remain_hire2+10,&(remain_hire3),1,V_YELLOW,1);
	}
	else {
	    opt3_text3.put( REMAIN_X, REMAIN_Y3, &remain_hire3);
	}
    }

    if( sys.view_mode == MODE_YEAR_END_REPORT )
	value_slider1.refresh();
    else
	opt3_text3.refresh();

    for (i = 0; i < department_array.size(); i++) {
	opt3_data1[i].refresh();
	opt3_data2[i].refresh();
	//		opt3_button_group_array[i]->push(opt3_button_group_array[i]->button_pressed);
	opt3_slider_group[i].refresh();
    }

    vga.use_back();
    opt3_text1.refresh();
    opt3_text2.refresh();
    vga.blt_buf(REMAIN_X,REMAIN_Y1,REMAIN_X+50,REMAIN_Y3-1);
    //	opt3_text3.refresh();
    for (i = 0; i < department_array.size(); i++) { {

	//		if (optimization.is_optimized[2]==1)
	opt3_slider_group[i].result_value = finance.hiring_policy_array[i].result_value;
	result_text_opt3[i].paint();
	vga.blt_buf(
	    RESULT_OFFSET + RESULT_LENGTH-50,
	    FIRST_LINE_OFFSET + line_spacing * i,
	    RESULT_OFFSET + RESULT_LENGTH,
	    FIRST_LINE_OFFSET + line_spacing * i + 16
	    );
    }
    }

    //------------------------------------------//

    int useBackBuf = vga.use_back_buf;
    vga.use_front();

    if( useBackBuf )
	vga.use_back();
}

//---------- End of function Optimization::report_stage_3 ------------//

//-------- Begin of function Optimization::detect_stage_3 ------------//
//!
int Optimization::detect_stage_3() {
    if (stage_init_flag != 3)
	return 0;
    help.set_help(  SEPARATOR_3, SLIDER_OFFSET + FIRST_LINE_OFFSET,
		    SEPARATOR_4-30,REPORT_Y2-20,"H8.2.1.");
    help.set_help(  SEPARATOR_4-30, SLIDER_OFFSET + FIRST_LINE_OFFSET,
		    SEPARATOR_4,REPORT_Y2-20,"H8.2.2.");
    help.set_help(  SEPARATOR_4, SLIDER_OFFSET + FIRST_LINE_OFFSET,
		    SEPARATOR_5,REPORT_Y2-20,"H8.2.3.");

    help.set_help(  REMAIN_X-45, REMAIN_Y3-1,REMAIN_X+55, REMAIN_Y3+15,"H8.2.4.");

    int i;
    //------ Detection in preference (right) column --------//
    /*
      if (mouse.single_click( SEPARATOR_4, REPORT_Y1+1+HEADING_HEIGHT,
      REPORT_X2, REPORT_Y2 ))
      {
      for (i = 0; i < department_array.size(); i++)
      if	( (int j = opt3_button_group_array[i]->detect()) >= 0)
      {

      optimization.is_optimize_slider_modified[2]=0;    //## chea 041099 try to release the opt button

      ButtonGroup *buttonGroupPtr = opt3_button_group_array[i];
      vga.use_back();
      for (int k = 0; k < PREF_COUNT; k++)
      {
      (*buttonGroupPtr)[k].body_ptr =
      (void *) ((k==j)?(opt3_button_group_down_bitmap[i*PREF_COUNT+k] + 4):(opt3_button_group_up_bitmap[i*PREF_COUNT+k] + 4));
      }
      buttonGroupPtr->paint(buttonGroupPtr->button_pressed);

      if(department_array.size()>0)
      line_spacing=((REPORT_Y2-REPORT_Y1-HEADING_HEIGHT)/department_array.size())-1;
      else
      line_spacing=LINE_SPACING;

      vga.blt_buf(
      PREF_BUTTON_OFFSET1,
      PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i,
      PREF_BUTTON_OFFSET1 + PREF_BUTTON_WIDTH,
      PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i + PREF_BUTTON_HEIGHT
      );

      vga.blt_buf(
      PREF_BUTTON_OFFSET2,
      PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i,
      PREF_BUTTON_OFFSET2 + PREF_BUTTON_WIDTH,
      PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i + PREF_BUTTON_HEIGHT
      );

      vga.blt_buf(
      PREF_BUTTON_OFFSET3,
      PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i,
      PREF_BUTTON_OFFSET3 + PREF_BUTTON_WIDTH,
      PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + line_spacing * i + PREF_BUTTON_HEIGHT
      );
      //				vga.blt_buf(PREF_BUTTON_OFFSET4, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)), PREF_BUTTON_OFFSET4 + PREF_BUTTON_WIDTH, PREF_BUTTON_OFFSET + FIRST_LINE_OFFSET + LINE_SPACING * (i + ((i >= REVENUE_POLICY_COUNT) ? 2 : 1)) + PREF_BUTTON_HEIGHT);

      if(j==0)
      {
      pref[i]  = OPT_LOW_WEIGHT;
      opt3_button_group_array[i]->push(0);
      optimization.is_optimize_slider_modified[2]=0;  //## chea 041099 try to release the opt button
      }
      else if(j==1)
      {
      pref[i]  = OPT_MID_WEIGHT;
      opt3_button_group_array[i]->push(1);
      optimization.is_optimize_slider_modified[2]=0;  //## chea 041099 try to release the opt button
      }
      else if(j==2)
      {
      pref[i]  = OPT_HIGH_WEIGHT;
      opt3_button_group_array[i]->push(2);
      optimization.is_optimize_slider_modified[2]=0;  //## chea 041099 try to release the opt button
      }

      finance.hiring_policy_array[i].weight = (float)pref[i];
      break;
      }
      }
    */
    /*
      if (mouse.single_click(SEPARATOR_4+5,REPORT_Y1+3,SEPARATOR_4+21,REPORT_Y1+19))
      {
      String str;
      str  = DIR_RES;
      str += "SCORE";
      ResTxt rs;
      rs.init(str);
      image_interface.put_back(SEPARATOR_4+5,REPORT_Y1+3,"QUEST_DN");
      vga.blt_buf(SEPARATOR_4+5,REPORT_Y1+3,SEPARATOR_4+21,REPORT_Y1+19);
      sys.back_view_mode=sys.view_mode;
      help_if.help_textlist.topic_id=rs.locate_topic("Preference");
      if(help_if.help_textlist.topic_id>0)
      {
      help_if.help_textlist.add_history(help_if.help_textlist.topic_id);
      sys.set_view_mode(MODE_HELP_DESC);
      sys.redraw_all_flag = 1;
      audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
      }
      return 1;
      }
    */
    if (mouse.in_area(TARGET_OFFSET,
		      REPORT_Y1+1+HEADING_HEIGHT,
		      TARGET_OFFSET+TARGET_WIDTH, REPORT_Y2 )) {
	for (i = 0; i < department_array.size(); i++)
	    if(opt3_slider_group[i].detect()) {
		optimization.is_optimize_slider_modified[2]=0;
		finance.hiring_policy_array[i].target_value=(int)(*(opt3_slider_group[i].var_ptr));
		finance.hiring_policy_array[i].upper_bound=(int)(*(opt3_slider_group[i].up_ptr));
		finance.calc_max_hires();

		// only allow setting the value in optimization
		if( sys.view_mode == MODE_YEAR_END_REPORT ) {
		    value_slider1.max_val = max( remain_hire2+10, *(value_slider1.var_ptr) );
		    value_slider1.refresh();
		}
		return 1;
	    }
    }

    if(help_button.detect()) {
	String str;
	str  = DIR_RES;
	str += "SCORE";

	ResTxt rs;
	rs.init(str);

	// ## chwg1203
	//		REMAIN_MESSAGE_X-20,
	//			REMAIN_Y1,
	//			REMAIN_MESSAGE_X-6,
	//			REMAIN_Y1+18,

	image_interface.put_back(
	    REMAIN_MESSAGE_X-27,
	    REMAIN_Y1,
	    "QUEST_DN");
	vga.blt_buf(
	    REMAIN_MESSAGE_X-27,
	    REMAIN_Y1,
	    REMAIN_MESSAGE_X-7,
	    REMAIN_Y1+20);

	sys.back_view_mode=sys.view_mode;
	help_if.help_textlist.topic_id=rs.locate_topic("Number of budgeted new faculty positions");
	if(help_if.help_textlist.topic_id>0) {
	    help_if.help_textlist.add_history(help_if.help_textlist.topic_id);
	    sys.set_view_mode(MODE_HELP_DESC);
	    sys.redraw_all_flag = 1;
	    audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	}
	return 1;
    }

    vga.use_back();

    if( sys.view_mode == MODE_YEAR_END_REPORT ) {   // only allow setting the value in optimization
	if(value_slider1.detect()) {
	    finance.total_hires=*(value_slider1.var_ptr);
	    optimization.is_optimize_slider_modified[2]=0;
	}
    }

    return 0;
}

//---------- End of function Optimization::detect_stage_3 ------------//
