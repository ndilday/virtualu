//Filename   : OFIN_NEW.CPP
//Description: New fiscal year report

#include <OSYS.H>
#include <OIFACE.H>
#include <OMATH.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OINFO.H>
#include <OCONFIG.H>
#include <OPSCHOOL.H>
#include <OOPT.H>
#include <OMOUSE.H>
#include <ODEPT.H>
#include <OBUTTON.H>
#include <OLETTER.H>
#include <OWORLD.H>
#include <OFONT.H>
#include <OFINANCE.H>
#include <OAUDIO.H>
#include <OATHLETI.H>
#include <OENROLL.H>

//------- define constant -----------//

enum {
    BUTTON_Y1=ZOOM_Y2-31,
    BUTTON_Y2=BUTTON_Y1+24
};

enum {
    BUTTON_BACK_WIDTH = 80,
    BUTTON_NEXT_WIDTH = 80,
    BUTTON_DISTANCE = 60
};

enum {
    FACULTY_LISTS,
    FACULTY_DETAILS,
};

//---------- Define static vars ----------//

static int report_screen_id;
static Button button_back, button_next;

//--------- Define static functions ----------//

static void screen_intro();
static void screen_opt_intro();
static void screen_last_year_finance(int reportOrder, int reportType, int reportYear, char* reportTitle, int refreshFlag);
static void screen_last_year_finance_detect(int reportType, int reportYear);
static int screen_keeping;
static int speed_keeping;

//----- Begin of function Finance::new_fiscal_year -----//

void Finance::new_fiscal_year() {
    if ( !update_projected_arrays_pre_optimization() )
	return;

    //---- apply the game setting "relative wealth" effect to surplus/deficit ---//

    if( info.prerun_year==1 ) {
	//--- special case handling: set last year sponsored research revenue's all 12 months to it last month identically ---//

	int i=AC_SPONSORED_RESEARCH_REVENUE;

	revenue_array[i].direct =  projected_revenue_array[i].last_month.direct * 12;
	revenue_array[i].indirect =  projected_revenue_array[i].last_month.indirect * 12;
	revenue_array[i].total =  projected_revenue_array[i].last_month.total * 12;

	//---- special case handling: adjust sponsored research expense -------//

	i=AC_SPONSORED_RESEARCH_EXPENSE;

	double totalRevenue = revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct;

	expense_array[i].faculty = totalRevenue * research_expense_fac_ratio;
	expense_array[i].staff = totalRevenue * research_expense_staff_ratio;
	expense_array[i].other = totalRevenue * ( 1-research_expense_fac_ratio-research_expense_staff_ratio);
	expense_array[i].total = expense_array[i].faculty + expense_array[i].staff + expense_array[i].other;

	//--- special case handling: set endowment spending based on endowment asset (endowment spending generated is too small and has to be re-calculated) ----//

	//----- recalculate the total for revenue_array[] & expense_array[] -----//

	calc_total_last_year();                       // it must be updated here first

	//-------- re-calculate the total after the above adjustments ------------//

	calc_total_last_year();                       // it has to be updated after apply_relative_wealth_effect() changed the operating reserve
    }

    //--------- generate evaluation letter ------//

    letter.init_eval_letter();                      // initialize evaluation letter
 
    {
	// Item 7b-e)
	// if ( info.prerun_year == 0 ) // begin of 1st year also call update_appropiation
	if ( player_school.is_public() ) {
	    finance.update_appropiation();              // change the state appropiation
	    double temp = finance.revenue_array[AC_STATE_APPROPRIATION].total;
	    RevenueItemChange* projRev;
	    projRev = &finance.projected_revenue_array[AC_STATE_APPROPRIATION];
	    projRev->change_budget_year.total
		= finance.calc_change( finance.budget_revenue_array[AC_STATE_APPROPRIATION].total, temp );

	    projRev->change_last_year.total = finance.calc_change( projRev->this_year.total, temp );
	}
    }

    //---------- show year-end screen -----------//

    if( config.disp_year_end_report ) {
	report_screen_id = 0;

	for ( int i=department_array.size(); i>0; i-- ) {
	    optimization.pre_target_value[i-1] = finance.hiring_policy_array[i-1].target_value;

	    Department* deptPtr = department_array[i];

	    // set the upper bound = max( 10, 2*departures[dept] )
	    faculty_res.is_year_end_report = 1;
	    finance.hiring_policy_array[i-1].upper_bound = 10;
	    finance.hiring_policy_array[i-1].target_value = optimization.pre_target_value[i-1] + deptPtr->faculty_departures_number();
	    finance.hiring_policy_array[i-1].weight = 1/( max( 1, deptPtr->faculty_array.size() ) );
	}

	sys.set_staying_view_mode(MODE_YEAR_END_REPORT);
	// new ring sound to be added.
	audio.play_wav("PHONE",audio.int_to_DsVolume(config.sound_effect_volume));
    }
    else {                                          //-- if the player has elected not to see the year-end report, do the optimization automatically --//
	//-----------------------------------------------------------//

	//		sys.yield();
	optimize_policy_1();
	//		sys.yield();
	optimize_policy_2();
	sys.yield();
	pre_optimization_stage_3();                   //## fred 990507
	optimize_policy_3();

	finance.calc_budget_report();                 //redundant call?		// BUGHER0 double-check the call order! //## 201199 chea have to added back in 5.1.1

	for ( int i=department_array.size(); i>0; i-- ) {
	    // reset faculty target hire
	    finance.hiring_policy_array[i-1].target_value = 0;
	}

	//---- reset user preference settings after optimization -----//

	sys.redraw_all_flag=1;
    }
}

//----- End of function Finance::new_fiscal_year -----//

//----- Begin of function Finance::new_fiscal_year_report -----//

void Finance::new_fiscal_year_report(int refreshFlag) {
    switch( report_screen_id ) {
    case 0:
	screen_intro();
	speed_keeping=sys.save_speed;
	screen_keeping=sys.save_view_mode;
	optimization.is_optimized[0]=0;
	optimization.is_optimized[1]=0;
	optimization.is_optimized[2]=0;
	optimization.is_optimize_slider_modified[0]=0;
	optimization.is_optimize_slider_modified[1]=0;
	optimization.is_optimize_slider_modified[2]=0;
	break;

    case 1:
	sys.staying_view_mode=MODE_EVALUATION_LETTER;
	sys.set_view_mode(MODE_EVALUATION_LETTER);
	sys.save_view_mode=MODE_YEAR_END_REPORT;
	//			sys.redraw_all_flag=1;
	break;

    case 2:
	//			sys.staying_view_mode=MODE_YEAR_END_REPORT;
	//			sys.redraw_all_flag=1;
	sys.staying_view_mode=MODE_STATE_REPORT;
	sys.set_view_mode(MODE_STATE_REPORT);
	sys.save_view_mode=MODE_YEAR_END_REPORT;
	break;

    case 3:
	screen_last_year_finance(2, REPORT_TYPE_BALANCE, REPORT_YEAR_PREV, "Balance sheet", refreshFlag);
	break;

    case 4:  player_school.summary_student_report_7(refreshFlag);
	break;
    case 5: faculty_res.is_year_end_report = 1;
	faculty_res.summary_report_9(refreshFlag);
	break;

    case 6:  faculty_res.summary_report_10(refreshFlag);
	break;

    case 7:
	screen_opt_intro();
	break;

    case 8:
    case 9:
    case 10:
	sys.view_mode=MODE_YEAR_END_REPORT;
	optimization.disp(refreshFlag);
	break;

    case 11:
	finance.calc_budget_report();               //BUGHERE double-check the call order!
	sys.save_view_mode=screen_keeping;
	config.frame_speed=DEFAULT_SPEED;
	sys.save_speed=DEFAULT_SPEED;
	for ( int i=department_array.size(); i>0; i-- ) {
	    // reset faculty target hire
	    finance.hiring_policy_array[i-1].target_value = 0;
	}
	sys.set_staying_view_mode(0);               // reset staying view mode
	sys.set_view_mode(screen_keeping);
	optimization.is_optimized[0]=0;
	optimization.is_optimized[1]=0;
	optimization.is_optimized[2]=0;
	optimization.is_optimize_slider_modified[0]=0;
	optimization.is_optimize_slider_modified[1]=0;
	optimization.is_optimize_slider_modified[2]=0;
	sys.redraw_all_flag=1;
	break;
    }
}

//----- End of function Finance::new_fiscal_year_report -----//

//----- Begin of function Finance::new_fiscal_year_report_detect -----//

void Finance::new_fiscal_year_report_detect() {
    int reportScreenId = report_screen_id;

    switch( report_screen_id ) {
    case 0:
	if( mouse.single_click() )
	    report_screen_id++;
	break;

    case 1:
	if ( letter.eval_letter_detect() == 2 ) {
	    if ( player_school.is_public() ) {
		report_screen_id=2;
		sys.set_view_mode( MODE_STATE_REPORT );
		sys.staying_view_mode = MODE_STATE_REPORT;
	    }
	    else {
		report_screen_id=4;
		enroll_res.update_student_intake();
		sys.set_view_mode( MODE_YEAR_END_REPORT );
		sys.staying_view_mode = MODE_YEAR_END_REPORT;
	    }
	}
	break;

    case 2:
	switch ( letter.state_report_detect() ) {
        case 2:                                   //  next
	    report_screen_id=4;
	    enroll_res.update_student_intake();
	    sys.set_view_mode(MODE_YEAR_END_REPORT);
	    sys.staying_view_mode = MODE_YEAR_END_REPORT;
	    break;

        case 1:                                   // back
	    report_screen_id=1;
	    sys.set_view_mode(MODE_EVALUATION_LETTER);
	    sys.staying_view_mode = MODE_EVALUATION_LETTER;
	    break;
	}
	break;

    case 3:
	screen_last_year_finance_detect(REPORT_TYPE_BALANCE, REPORT_YEAR_PREV);
	break;

    case 4:
	switch ( player_school.summary_student_report_detect() ) {
        case 1:                                   // back to state report or evaluation letter
	    if ( player_school.is_public() ) {
		sys.set_view_mode( MODE_STATE_REPORT );
		sys.staying_view_mode = MODE_STATE_REPORT;
		report_screen_id = 2;
	    }
	    else {
		sys.set_view_mode( MODE_EVALUATION_LETTER );
		sys.staying_view_mode = MODE_EVALUATION_LETTER;
		report_screen_id = 1;
	    }
	    break;
        case 2: report_screen_id++;               // Faculty Turnover Report
	    faculty_res.current_year_report = FACULTY_LISTS;
	}
	break;

    case 5:
	switch ( faculty_res.summary_report_detect() ) {
        case 1:  report_screen_id++;
	    faculty_res.current_year_report = FACULTY_DETAILS;
	    break;
        case 2:  report_screen_id--;
	    break;
        case 3:  report_screen_id=7;              // Student Turnover
	}
	break;

    case 6:
	if ( faculty_res.summary_report_detect() ) {// Faculty Turnover details
	    report_screen_id--;
	    faculty_res.current_year_report = FACULTY_LISTS;
	}
	break;

    case 7:
	if( mouse.single_click() ) {
	    report_screen_id++;
	    optimization.current_stage = 1;
	}
	break;

    case 8:
	if ( optimization.detect() == 2 )           // back button
	    report_screen_id = 5;

	if( optimization.current_stage > 1 )
	    report_screen_id++;

	break;

    case 9:
	optimization.detect();

	if( optimization.current_stage < 2 )
	    report_screen_id--;

	else if( optimization.current_stage > 2 ) {
	    // set the result value to be all 0
	    for ( int i=department_array.size(); i>0; i-- )
		finance.hiring_policy_array[i].result_value = 0;
	    report_screen_id++;
	}
	break;

    case 10:
	optimization.detect();

	if( optimization.current_stage < 3 )
	    report_screen_id--;
	else if( optimization.current_stage > 3 ) {
	    report_screen_id++;
	    // unlock the maximum of opt1
	    optimization.opt1_slider_group[0].lock_max = 0;
	    float max = 5.0;
	    *optimization.opt1_slider_group[0].up_ptr = max;

	    faculty_res.is_year_end_report = 0;
	    player_school.is_year_end_report = 0;
	}
	break;

    case 11:
	screen_last_year_finance_detect(REPORT_TYPE_REVENUE, REPORT_YEAR_THIS);
	break;

    case 12:
	screen_last_year_finance_detect(REPORT_TYPE_BALANCE, REPORT_YEAR_THIS);
	break;
    }

if( report_screen_id != reportScreenId )
    sys.redraw_zoom_flag = 1;
}

//----- End of function Finance::new_fiscal_year_report_detect -----//

// --- Begin of function Finance::update_appropiation --- //
//
void Finance::update_appropiation() {
// ----------------- Task 7b)-7e) -------- //
// Refer to Design Specification: Version 1.2, P.11-12

  // Shift History for the variables of appropiation
finance.base_appropriation[0]           = finance.base_appropriation[1];
finance.enroll_appropriation[0]         = finance.enroll_appropriation[1];
finance.performance_appropriation[0]      = finance.performance_appropriation[1];
finance.education_quality_appropriation[0]  = finance.education_quality_appropriation[1];
finance.total_appropriation[0]          = finance.total_appropriation[1];
finance.facility_reserve_appropriation[0]   = finance.facility_reserve_appropriation[1];

  // ********************* Task 7c) ******************* //

  // First declare some useful variables
double STt;                                     // Current State Appropriation
double STDiv;                                   // STt / initial_state_appropriation
double SEt;                                     // Current Enrollment Appropriation
double SPIt;                                    // Current Performance Indicator Appropriation
double SEQt;                                    // Current Education Quality Appropriation
double SBt;                                     // Current Base Appropriation
double STAt;                                    // Current Total State Appropriation
//	double XTt,X1t,X2t,X3t,X4t;				// Current variable value
//	double XT0,X10,X20,X30,X40;				// Initial variable value
double XTDiv,X1Div,X2Div,X3Div,X4Div;           // XTt/XT0, X1t/X10 ...

if ( info.prerun_year ) {
// get the initial values
initial_state_appropriation = projected_revenue_array[AC_STATE_APPROPRIATION].this_year.total;
initial_base_appropriation  = 0.5*initial_state_appropriation;
initial_enrollment_appropriation = 0.35*initial_state_appropriation;
initial_performance_appropriation = 0.15*initial_state_appropriation;
}

// get STt (Current State Appropriation)
////	STt = projected_revenue_array[AC_STATE_APPROPRIATION].this_year.total;
STt = initial_state_appropriation;
if( info.prerun_year || initial_state_appropriation == 0.0 )
    STDiv = 1.0;
else
STDiv = STt / initial_state_appropriation;

  // Calculate SEt ( Current Enrollment Appropriation )
  // XTt = department_array.convention_student_count_history[MAX_TEACHING_METHOD_N_TOTAL-1][HISTORY_TRIMESTER_COUNT-3];
  // XT0 = department_array.initial_convention_student_count[MAX_TEACHING_METHOD_N_TOTAL-1];
                                                  // initial not set, so set convention_student_count
if( info.prerun_year || department_array.initial_convention_student_count[MAX_TEACHING_METHOD_N_TOTAL-1] == 0 )
    XTDiv = 1.0;
else
XTDiv = (double)department_array.convention_student_count_history[MAX_TEACHING_METHOD_N_TOTAL-1][HISTORY_TRIMESTER_COUNT-3]
/ department_array.initial_convention_student_count[MAX_TEACHING_METHOD_N_TOTAL-1];

SEt = initial_enrollment_appropriation
*STDiv                                          // *STt/initial_state_appropriation
*XTDiv;                                         // *XTt/XT0;

finance.enroll_appropriation[1] = SEt;

// Calculate SPIt ( Current Performance Indicator Appropriation )
//X1t = player_school.sub_score[S_DEGREES_GRANTED][HISTORY_MONTH_COUNT-1];
//X10 = player_school.sub_score_start[S_DEGREES_GRANTED];
if( info.prerun_year || player_school.sub_score_start[S_DEGREES_GRANTED] == 0.0 )
    X1Div = 1.0;
else
X1Div = player_school.sub_score[S_DEGREES_GRANTED][HISTORY_MONTH_COUNT-1]
/ player_school.sub_score_start[S_DEGREES_GRANTED];

  //X2t = player_school.sub_score[5][HISTORY_MONTH_COUNT-1];
  //X20 = player_school.sub_score_start[S_STUDENT_DIVERSITY];
if( info.prerun_year || player_school.sub_score_start[S_STUDENT_DIVERSITY] == 0.0 )
    X2Div = 1.0;
else
X2Div = player_school.sub_score[S_STUDENT_DIVERSITY][HISTORY_MONTH_COUNT-1]
/ player_school.sub_score_start[S_STUDENT_DIVERSITY];

  //	X3t = department_array.admission_denied_count_history[MAX_TEACHING_METHOD][HISTORY_TRIMESTER_COUNT-3]
  //		+ department_array.admission_denied_count_history[MAX_TEACHING_METHOD][HISTORY_TRIMESTER_COUNT-2]
  //		+ department_array.admission_denied_count_history[MAX_TEACHING_METHOD][HISTORY_TRIMESTER_COUNT-1];
  ////	X30 = department_array.initial_admission_denied_count[3]
  ////			+ department_array.initial_admission_denied_count[4]
  ////			+ department_array.initial_admission_denied_count[5];
  // X30 = department_array.initial_admission_denied_count[MAX_TEACHING_METHOD];
if( info.prerun_year || department_array.initial_admission_denied_count[MAX_TEACHING_METHOD] == 0 )
    X3Div = 1.0;
else
X3Div = (double)(department_array.admission_denied_count_history[MAX_TEACHING_METHOD][HISTORY_TRIMESTER_COUNT-3]
+ department_array.admission_denied_count_history[MAX_TEACHING_METHOD][HISTORY_TRIMESTER_COUNT-2]
+ department_array.admission_denied_count_history[MAX_TEACHING_METHOD][HISTORY_TRIMESTER_COUNT-1] )
    / department_array.initial_admission_denied_count[MAX_TEACHING_METHOD];

  //X4t = department_array.size_deviation[5]
  //X40 = department_array.initial_size_deviation[5];
if( info.prerun_year || department_array.initial_size_deviation[MAX_TEACHING_METHOD] == 0 )
    X4Div = 1.0;
else
X4Div = (double)department_array.size_deviation[MAX_TEACHING_METHOD]
/ department_array.initial_size_deviation[MAX_TEACHING_METHOD];

SPIt = initial_performance_appropriation
*STDiv                                          // *STt/initial_state_appropriation
*math.safe_pow(X1Div,0.1)                       // *math.safe_pow(X1t/X10,0.1)
    *math.safe_pow(X2Div,0.3)                       // *math.safe_pow(X2t/X20,0.3)
    *math.safe_pow(X3Div,0.2)                       // *math.safe_pow(X3t/X30,0.2)
    *math.safe_pow(X4Div,0.1);                      // *math.safe_pow(X4t/X40,0.1);

finance.performance_appropriation[1] = SPIt;

// Calculate SEQt ( Current Educational Quality Appropriation )

  // Calculate "result" variable for education quality
int result = player_school.think_protagonist_event(
player_school.sub_score[S_EDUCATIONAL_QUALITY][HISTORY_MONTH_COUNT-1],
    player_school.sub_score[S_EDUCATIONAL_QUALITY][HISTORY_MONTH_COUNT-1-12],
    player_school.sub_score_start[S_EDUCATIONAL_QUALITY],
    player_school.get_protagonist_info(5),
    -1, 1 );

switch ( result ) {
case 1: SEQt = 0.05*STt; break;
case 2: SEQt = 0.02*STt; break;
case 3: SEQt = -0.02*STt; break;
case 4:                                       // case 4 & 7
case 7: SEQt = -0.05*STt; break;
default:  SEQt = 0;
}

finance.education_quality_appropriation[1] = SEQt;

  // Calculate SBt ( Current Base Appropriation )

SBt = 0.5*STt;

finance.base_appropriation[1] = SBt;

  // Calculate STAt ( Current Total State Appropriation )
STAt = SBt + SEt + SPIt + SEQt;

finance.total_appropriation[1] = STAt;

  // Replace the Total State Appropriation to State Appropriation
projected_revenue_array[AC_STATE_APPROPRIATION].this_year.total = STAt;
budget_revenue_array[AC_STATE_APPROPRIATION].total = STAt;
//	revenue_array[AC_STATE_APPROPRIATION].total = STAt;

// ********************* Task 7d) ******************** //

  // First define some useful variables
double SFRt=0;                                  // Current Infusion to the Facilities Reserve

int rc1, rc2;

  // For Football performance and Basketball performance
rc1 = player_school.think_protagonist_event(
math.safe_divide((float)athletics_office.football_graph[THIS_YEAR], float(athletics_office.football_graph[THIS_YEAR] + athletics_office.football_graph[HISTORY_YEAR_COUNT+THIS_YEAR]) * 100.0f),
    math.safe_divide((float)athletics_office.football_graph[THIS_YEAR-1], float(athletics_office.football_graph[THIS_YEAR-1] + athletics_office.football_graph[HISTORY_YEAR_COUNT+THIS_YEAR-1]) * 100.0f),
    math.safe_divide((float)athletics_office.initial_football_graph[0], float(athletics_office.initial_football_graph[0] + athletics_office.initial_football_graph[1]) * 100.0f),
    player_school.get_protagonist_info(29), -1,1 ); // -1=institutional level

rc2 = player_school.think_protagonist_event(
math.safe_divide((float)athletics_office.basketball_graph[THIS_YEAR], float(athletics_office.basketball_graph[THIS_YEAR] + athletics_office.basketball_graph[HISTORY_YEAR_COUNT+THIS_YEAR]) * 100.0f),
    math.safe_divide((float)athletics_office.basketball_graph[THIS_YEAR-1], float(athletics_office.basketball_graph[THIS_YEAR-1] + athletics_office.basketball_graph[HISTORY_YEAR_COUNT+THIS_YEAR-1]) * 100.0f),
    math.safe_divide((float)athletics_office.initial_basketball_graph[0], float(athletics_office.initial_basketball_graph[0] + athletics_office.initial_basketball_graph[1]) * 100.0f),
    player_school.get_protagonist_info(30), -1,1 ); // -1=institutional level

if (( rc1 == 1) && (rc2 == 1)) {
SFRt = 0.03;
}
else
if ((rc1 == 1) || (rc2 == 1))
    SFRt = 0.01;

  // For institutional prestige
rc1 = player_school.think_protagonist_event( player_school.sub_score[S_PRESTIGE][HISTORY_MONTH_COUNT-1],
    player_school.sub_score[S_PRESTIGE][HISTORY_MONTH_COUNT-13],
    player_school.sub_score_start[S_PRESTIGE],
    player_school.get_protagonist_info(4), -1,1 );  // -1=institutional level

if ( rc1 == 1 )
    SFRt += 0.04;

                                                  // Capital Reserve
finance.this_year.asset_array[AC_CAPITAL_RESERVE] *= (1+SFRt);
// Facility Reserve Appropriation
finance.facility_reserve_appropriation[1] = SFRt;

// ********************** Part 7e) ************************* //
// Regulation of tuition increases

// get the result of tuition growth variable
rc1 = player_school.think_protagonist_event( finance.tuition_increase_history[THIS_YEAR],
    finance.tuition_increase_history[THIS_YEAR-1],
    finance.initial_tuition_increase,
    player_school.get_protagonist_info(27), -1,1 ); // -1=institutional level

// Base on the result above, set P
float P;
switch (rc1) {
case 4:                                       // case 4 & 7
case 7: P=0.65;break;
case 3: P=0.4;break;
default:  P=0.2;
}

float randomNo = math.get_random_float();

  // get a random variable and compare with P, to check there will be regulation
if ( randomNo <= P ) {                          // do regulation
// By different result, draw a normally-distributed random variable
float max;
switch (rc1) {
case 4: max = math.get_random_snd((float)2.0,0.2); break;
case 3: max = math.get_random_snd((float)1.5,0.3); break;
default:  max = math.get_random_snd((float)2.0,0.4); break;
}

// After getting the max, set the maximum of tuition growth = max
finance.revenue_policy_array[0].upper_bound = max;
}
}

//
// --- End of function Finance::update_appropiation --- //

//----- Begin of static function screen_intro -----//

static void screen_intro() {
//-------- message screen: greeting --------//
//	audio.play_wav("PHONE",audio.int_to_DsVolume(config.sound_effect_volume));

user_interface.bg_img(1, &vga_back);

String str;

str  = "End of the Academic/Fiscal Year ";
str += m.format( info.financial_year(), 4);     // end fiscal year 1 at year 2, etc.

font_charts.center_put(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2-50, str );

font_charts.center_put(ZOOM_X1, ZOOM_Y2-50, ZOOM_X2, ZOOM_Y2,
	"Click to Continue..." );

vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 1);
sys.blt_virtual_buf();
}

//----- End of static function screen_intro -----//

//----- Begin of static function screen_opt_intro -----//

static void screen_opt_intro() {
//------- message screen: begin resource allocation ----//

user_interface.bg_img(1, &vga_back);

String str;

str  = "Resource Allocation for the Fiscal Year ";
str += m.format( info.financial_year()+1, 4);   // end fiscal year 1 at year 2, etc.

font_charts.center_put(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2-50, str );

font_charts.center_put(ZOOM_X1, ZOOM_Y2-50, ZOOM_X2, ZOOM_Y2,
	"Click to Begin Resource Allocation..." );

vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 1);
sys.blt_virtual_buf();
}

//----- End of static function screen_opt_intro -----//

//----- Begin of static function screen_last_year_finance -----//

static void screen_last_year_finance(int reportOrder, int reportType, int reportYear, char* reportTitle, int refreshFlag) {
int saveReportType = finance.report_type;
int saveReportYear = finance.report_year;

finance.report_type = reportType;
finance.report_year = reportYear;

// 0-don't display buttons, we will display buttons by our own
finance.disp_report_main(refreshFlag, 0, reportTitle);

//----- paint buttons --------//

if( refreshFlag == INFO_REPAINT ) {
int x;

if( reportOrder==1 ) {                        // just the "Next" button
x = ( VGA_WIDTH - BUTTON_NEXT_WIDTH ) / 2;

button_back.reset();

button_next.paint_text( x, BUTTON_Y1,
        x+BUTTON_NEXT_WIDTH, BUTTON_Y2, "Next" );
}
else {                                        // the "Back" and "Next" buttons
x = ( VGA_WIDTH - (BUTTON_BACK_WIDTH + BUTTON_DISTANCE + BUTTON_NEXT_WIDTH) ) / 2;

button_back.paint_text( x, BUTTON_Y1,
        x+BUTTON_BACK_WIDTH, BUTTON_Y2, "Back" );

x += BUTTON_BACK_WIDTH + BUTTON_DISTANCE;

button_next.paint_text( x, BUTTON_Y1,
        x+BUTTON_NEXT_WIDTH, BUTTON_Y2, "Next" );
}
}

finance.report_type = saveReportType;
finance.report_year = saveReportYear;
}

//----- End of static function screen_last_year_finance -----//

//----- Begin of static function screen_last_year_finance_detect -----//

static void screen_last_year_finance_detect(int reportType, int reportYear) {
int saveReportType = finance.report_type;
int saveReportYear = finance.report_year;

finance.report_type = reportType;
finance.report_year = reportYear;

finance.report_detect(0);                       // 0-don't display buttons, we will display buttons by our own

//---------------------------------//

if( button_back.detect() )
    report_screen_id--;

if( button_next.detect() )
    report_screen_id++;

//---------------------------------//

finance.report_type = saveReportType;
finance.report_year = saveReportYear;
}

//----- End of static function screen_last_year_finance_detect -----//
