//Filename    : OSTD_SUM.CPP
//Description : Faculty Report

#include <KEY.H>
#include <OVGA.H>
#include <OMATH.H>
#include <OFONT.H>
#include <OWORLD.H>
#include <OSYS.H>
#include <OINFO.H>
#include <OFINANCE.H>
#include <OENROLL.H>
#include <OIFACE.H>
#include <OBUTTON.H>
#include <OVALSLDR.H>
#include <ODEPT.H>
#include <OPSCHOOL.H>
#include <OSTUOFF.H>
#include <OSTUDENT.H>
#include <OGAMESTR.H>
#include <OSPINNER.H>
#include <OGRPHYR.H>
#include <OGRPNFYR.H>                             // Large Performance Indicator
#include <OPIECHRT.H>
#include <OGRPH_PY.H>
#include <OGRPH_PT.H>
#include <OGRPH_PM.H>
#include <OGRPNFT.H>
#include <OTEXT.H>
#include <OHELP.H>

//---------- Define constant ------------//

enum {
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+15,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-40,
};

enum { FACULTY_REC_HEIGHT = 84, RESEARCH_REC_HEIGHT = 16 };

//--------- Define report modes ----------//

enum {                                            // the bg image id
    BG_PIC_ID = 8
};

enum { STD_SUM_REPORT_TYPE_COUNT = 10};
enum {
    REPORT_TYPE_DEPARTMENT_PROFILE1, REPORT_TYPE_DEPARTMENT_PROFILE2,
    REPORT_TYPE_DEPARTMENT_ACTIVITIES, REPORT_TYPE_TURNOVER,
    REPORT_TYPE_INSTITUTION_PROFILE1, REPORT_TYPE_INSTITUTION_PROFILE2,
    REPORT_TYPE_INSTITUTION_ACTIVITIES, REPORT_TYPE_INSTITUTION_UNDERGRADUATE,
    REPORT_TYPE_FINANCE,
    REPORT_TYPE_RESIDENCE_HALL,
};

enum { REPORT_MODE_COUNT = 2 };
enum { REPORT_MODE_INSTITUTION, REPORT_MODE_DEPARTMENT};

enum { BUTTON_Y1=REPORT_Y2+10 };

enum {
    BOTTOM_BUTTON_X1 = REPORT_X1+68,
    BOTTOM_BUTTON_Y1 = REPORT_Y2-20,
    BOTTOM_BUTTON_TAB1 = 400,
    BOTTOM_BUTTON_X2 = REPORT_X2,
    BOTTOM_BUTTON_Y2 = REPORT_Y2+10
};
enum {
    TAB1=20,
    TAB2=200,
    TAB3=210,
    TAB4=450,
    TAB5=500,
    TAB6=610,
    VTAB1=5,
    VTAB2=160,
    VTAB3=180,
    VTAB4=310,
    VTAB5=420,
};

enum {
    BOTTOM_BUTTON_WIDTH = 104,
    BOTTOM_BUTTON_WIDTH2 = 85,
    BOTTOM_BUTTON_HEIGHT = 22,
    BOTTOM_BUTTON_DISTANCE = 2
};

enum {
    MODE_FINANCIAL_BOX1_HEIGHT = 65,
    MODE_FINANCIAL_BOX2_HEIGHT = 260,
    MODE_FINANCIAL_GRAPH_WIDTH = 400,
    MODE_FINANCIAL_GRAPH_HEIGHT = 150,
    MODE_FINANCIAL_LEGEND_HEIGHT = 26,
    MODE_FINANCIAL_LEGEND_WIDTH = 400,

    MODE_FINANCIAL_TAB1 = 15,
    MODE_FINANCIAL_TAB2 = 280,
    MODE_FINANCIAL_TAB3 = 360,
    MODE_FINANCIAL_TAB4 = 400,
    MODE_FINANCIAL_TAB5 = 520,
    MODE_FINANCIAL_TAB6 = 600,
};

enum {
    FINANCIAL_BUTTON_X1 = REPORT_X1+MODE_FINANCIAL_TAB4+60,
    FINANCIAL_BUTTON_Y1 = REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+132,
    FINANCIAL_BUTTON_X2 = REPORT_X2,
    FINANCIAL_BUTTON_Y2 = REPORT_Y2-30
};

enum {
    FINANCIAL_BUTTON2_Y1 = REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+262,
    FINANCIAL_BUTTON2_Y2 = REPORT_Y2-30
};

enum {
    FINANCIAL_BUTTON_WIDTH = 200,
    FINANCIAL_BUTTON_HEIGHT = 20,
    FINANCIAL_BUTTON_DISTANCE = 3
};

enum {                                            // the 'sub' view-modes
    MODE_FINANCIAL_IMPLEMENT = 2,
    MODE_FINANCIAL_ANNOUNCE = 1,
    MODE_FINANCIAL_CONSIDER = 0,
};

static ButtonGroup type_button_group(10);
//static short *type_button_up_bitmap[9];
//static short *type_button_down_bitmap[9];
static Button mode_turnover_button;
static Button mode_turnover_next_button;

static short *text_refresh_bitmap1;

// Begin Financial Mode Setting
static char *mode_financial_str[] = {
    "Gross tuition income",
    "Student life spending",
    "Student life satisfaction",
    "",
    "Change",
    "Change in tuition rate",
    "Change in student life \r\nexpenditures"
};

static char *mode_financial_label[] = {
    "Revenue", "Expenditure","Profit", "Loss",
};

static char* mode_turnover_label1[] = {
    "Prior year's","Admissions","","","Current",
};

static char* mode_turnover_label2[] = {
    "enrollment","target","Graduations","Dropouts","enrollment",
};

static char* mode_turnover_row_label[] = {
    "Student level","Undergraduate traditional","Undergraduates nontraditional",
    "Graduate: Master's","Graduate: Doctoral","Total",
};

static short *mode_financial_stats_bitmap;
static Spinner financial_spinner_budget1;
static Spinner financial_spinner_budget2;
static ButtonGroup financial_button_group1(3);
static short *financial_button1_up_bitmap[3];
static short *financial_button1_down_bitmap[3];
static ButtonGroup financial_button_group2(3);
static short *financial_button2_up_bitmap[3];
static short *financial_button2_down_bitmap[3];
static char *financial_button_label[] = {
    "Consider for next year", "Promise for next year", "Implement now"
};
static char financial_current_mode = MODE_FINANCIAL_CONSIDER;

static GraphYear mode_financial_graph_profitloss1;
static GraphYear mode_financial_graph_profitloss2;
static GraphPerformanceIndicatorMonth mode_financial_graphicon;
static Text mode_financial_text;
static char *mode_financial_graph_title="";
static char *mode_financial_legend_label[] = {
    "Gross tuition revenue","Student life spending",
};

// End Financial Mode Setting

// Begin Residence Hall Mode Setting
static Text mode_hall_text;
static char *mode_hall_str[] = {
    "Occupied beds/ Available beds",
    "Percentage of traditional undergraduate students in residence halls"
};

static char *mode_hall_legend1[] = {
    "Available beds","Occupied beds"
};

static char *mode_hall_legend2[] = {
    "Tuition rate","Room & board rate"
};

static GraphNonFinancialYear mode_hall_graph1;
static GraphNonFinancialYear mode_hall_graph2;
static ValueSlider mode_hall_value_slider;
// End Residence Hall Mode Setting

static Department*  cur_dept_ptr;
// the department whose faculty we are looking at

//static char* reportModeStrArray[]
// = { "Institution", "Department" };

static char* reportTypeStrArray[]
= {
    "Levels", "Profiles","Activities",           "Turnover",
    "Levels", "Profiles","Activities","Undergraduate","Finance","Tuition &\nResidence halls",
};

static char* std_sum_rank_label[] = {
    "Undergraduate traditional",
    "Undergraduate nontraditional",
    "Graduate : master's",
    "Graduate : doctoral",
    "",
    "Total",
};

static char* std_sum_ge_label[] = {
    "Nonminority male",
    "Nonminority female",
    "Minority male",
    "Minority female",
    "Total",
};
static char* std_sum_title[] = {
    "Student Distribution by Student Level",
    "Student Distribution by Gender and Ethnicity",
};

static char* std_sum_title2[] = {
    "Academic talent",
    "Academic performance",
    "Number of degrees awarded",
    "Undergraduate talent",
    "Undergraduate satisfaction"
};

static char* std_sum_title3[] = {
    "Academic talent",
    "Academic performance",
    "Number of degrees awarded",
    "Time to degree (years)",
    "Number of dropouts",
    "Morale",
};

static char* std_sum_label2[] = {
    "Undergraduate students","Master's and professional students","Doctoral students",
    "Academic","Extracurricular","Athletics",
    "Academic","Student life","Athletics","Morale\r\n(overall satisfaction)"
};

static PieChart std_sum_rank_piechart;
static PieChart std_sum_gender_ethnic_piechart;
static GraphPerformanceIndicatorTrimester mode_std_sum_graphicon_t[14];
static GraphPerformanceIndicatorMonth mode_std_sum_graphicon_m[14];
static GraphPerformanceIndicatorYear mode_std_sum_graphicon_y[14];
static GraphPerformanceIndicatorTrimester mode_std_sum_student_morale;
static Text std_sum_text[15];
static Text std_sum_text2[14];
static char report_init_flag=0;

////////////////////////////////////////////////////////////////////////
//----- Begin of Summary Report 1 ------//

void PlayerSchool::summary_student_report_1(int refreshFlag) {
    static int percentDataArray[MAX_STUDENT_LEVEL+1];
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//

    static int data_num=HISTORY_YEAR_COUNT;
    static int data_numT=HISTORY_TRIMESTER_COUNT;
    //	static int *dataArray[MAX_STUDENT_LEVEL];   //## chea 270999 change this and try org.
    static int *dataArray[MAX_STUDENT_LEVEL+1];     //## chea 270999 change this and try org.
    //	static int *initialDataArray[MAX_STUDENT_LEVEL+1];   //Marco
    int initialDataArray[MAX_STUDENT_LEVEL+1];      //Marco new

    int rankLevelCount[MAX_STUDENT_LEVEL+1];
    int initialRankLevelCount[MAX_STUDENT_LEVEL+1]; //marco
    int deptRecno;

    memset( rankLevelCount,0, sizeof(rankLevelCount) );
    //marco
    memset( initialRankLevelCount,0, sizeof(initialRankLevelCount) );

    //---- determine the department for display the faculty records ----//
    if(current_report_mode==REPORT_MODE_DEPARTMENT) {
	if( department_array.selected_recno )
	    deptRecno = department_array.selected_recno;
	cur_dept_ptr = department_array[deptRecno];
	//-- department wide --//
	for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++) {
	    //			dataArray[i] = cur_dept_ptr->student_level_history[i];  //## chea 270999 org
	    dataArray[i] = cur_dept_ptr->dept_student_level_history[i];
	    //marco
	    initialDataArray[i] = cur_dept_ptr->initial_dept_student_level_history[i];

	    rankLevelCount[i]=dataArray[i][data_numT-1];//## chea 270999  org
	    //			rankLevelCount[i]=dataArray[i];
	    //initialRankLevelCount[i]=initialDataArray[i][data_numT-1];			//marco
	    //marco	new
	    initialRankLevelCount[i]=initialDataArray[i];
	}
    }
    else {
	//-- Institution wide --//
	for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++) {
	    //## chea 270999 update tri.
	    //			dataArray[i]=department_array.student_level_history[i];  //## chea 270999//org.
	    //			rankLevelCount[i]=dataArray[i][data_num-1];              //## chea 270999//org.

	    //## chea 270999//org.
	    dataArray[i]=player_school.ins_student_level_history[i];
	    //## chea added the gen_dept_student back in  //## chea 270999//org.
	    rankLevelCount[i]=dataArray[i][data_numT-1] + department_res.general_dept.dept_student_level_no[i];

	    // ##### Begin Marco ##### //
	    initialDataArray[i]=player_school.initial_ins_student_level_history[i];
	    //			initialRankLevelCount[i]=initialDataArray[i][data_numT-1] + department_res.general_dept.initial_dept_student_level_no[i];
	    //NEW
	    initialRankLevelCount[i]=initialDataArray[i] + department_res.general_dept.initial_dept_student_level_no[i];
	    // ##### End Marco ##### //

	}
    }

    //	if(rankLevelCount[MAX_STUDENT_LEVEL_N_TOTAL-1]>1e-9)
    //		for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++)
    //				percentDataArray[i]=(rankLevelCount[i]*100)/rankLevelCount[MAX_STUDENT_LEVEL_N_TOTAL-1];

    int sum=0;
    for(int i=0;i<MAX_STUDENT_LEVEL;i++) {
	if ( i == DISTANCE_LEARN )                    // don't count distance learning in ver2
	    continue;
	sum+=rankLevelCount[i];
    }
    rankLevelCount[MAX_STUDENT_LEVEL_N_TOTAL-1]=sum;

    // ##### Begin Marco ##### //
    int initialSum=0;
    for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL-1;i++) {
	if ( i == 4 )
	    continue;
	initialSum+=initialRankLevelCount[i];
    }
    initialRankLevelCount[MAX_STUDENT_LEVEL_N_TOTAL-1]=initialSum;
    // ##### End Marco ##### //
    if(sum!=0) {
	int diff=200;
	for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++) {
	    percentDataArray[i]=(rankLevelCount[i]*100)/sum;
	    diff-=percentDataArray[i];
	}
	int maxpercent=percentDataArray[0],maxi=0;
	for(int i=1;i<MAX_STUDENT_LEVEL_N_TOTAL-1;i++) {
	    if(percentDataArray[i]>maxpercent) {
		maxpercent=percentDataArray[i];
		maxi=i;
	    }
	}
	percentDataArray[maxi]+=diff;
    }
    else
	for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++)
	    percentDataArray[i]=0;

    //------ paint the background -------//
    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_student_report_deinit();

	report_init_flag = 1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);
	user_interface.brighten(
	    REPORT_X1+TAB1,
	    REPORT_Y1+VTAB1+20,
	    REPORT_X1+TAB2,
	    REPORT_Y1+VTAB4
	    );
	user_interface.brighten(
	    REPORT_X1+TAB2,
	    REPORT_Y1+VTAB1+20,
	    REPORT_X2,
	    REPORT_Y1+VTAB4);

	user_interface.rect(
	    REPORT_X1+TAB2-2,
	    REPORT_Y1+VTAB1+20,
	    REPORT_X2,
	    REPORT_Y1+VTAB4,1);
	/*
	  user_interface.brighten(
	  REPORT_X1+TAB2+45,
	  REPORT_Y1+VTAB1-15,
	  REPORT_X1+TAB2+325,
	  REPORT_Y1+VTAB1+8);

	  user_interface.rect(
	  REPORT_X1+TAB2+45,
	  REPORT_Y1+VTAB1-15,
	  REPORT_X1+TAB2+325,
	  REPORT_Y1+VTAB1+8);

	  font_charts.put(
	  REPORT_X1+TAB2+60,
	  REPORT_Y1+VTAB1-15,
	  std_sum_title[0]);
	*/
	// top title
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	font_charts.put(REPORT_X1+230,REPORT_Y1-8,std_sum_title[0]);

	//----- initialize the bottom sub-menus buttons -----//

	text_refresh_bitmap1=NULL;
	text_refresh_bitmap1=vga_back.save_area(
	    REPORT_X1+TAB4,
	    REPORT_Y1+VTAB1,
	    REPORT_X2-100,
	    REPORT_Y1+VTAB4,
	    text_refresh_bitmap1);

	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;
	font_chartsm.put(REPORT_X1-10,y+2,"Department :");
	font_chartsm.put(REPORT_X1-10,y+30,"  Institution :");
	int i;
	for (i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
	type_button_group[i].create_text(x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y,
					 x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y+BOTTOM_BUTTON_HEIGHT,
					 reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (i = 4; i < 9; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,
						 x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
						 reportTypeStrArray[i]);
	y = BOTTOM_BUTTON_Y1;
	type_button_group[i].create_text(x, y,
					 x+BOTTOM_BUTTON_WIDTH+10, y+BOTTOM_BUTTON_HEIGHT+28,
					 reportTypeStrArray[i]);

	std_sum_rank_piechart.init(
	    REPORT_X1+TAB1,
	    REPORT_Y1+VTAB1+20,
	    REPORT_X1+TAB2,
	    REPORT_Y1+VTAB4,
	    4, rankLevelCount, &xStart,&xEnd,
	    NULL, NULL, NULL, 1, 1, 0, 3);

	std_sum_rank_piechart.refresh();

	//-------- draw the legend --------------//

	for(i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++) {
	    if (i == 4)
		continue;
	    font_chartsm.put_paragraph(
		REPORT_X1+TAB3+60,
		REPORT_Y1+VTAB1+i*43+43,
		REPORT_X1+TAB4+20,
		REPORT_Y1+VTAB1+i*43+83,
		std_sum_rank_label[i]);
	    font_chartsm.right_put(
		REPORT_X1+TAB4+60,
		REPORT_Y1+VTAB1+i*43+43,
		m.format(rankLevelCount[i],4));
	    // ##### Begin Marco ##### //
	    font_chart_purple_sm.right_put(
		REPORT_X1+TAB5+60,
		REPORT_Y1+VTAB1+i*43+43,
		m.format(initialRankLevelCount[i],4));
	    // ##### End Marco ##### //
	    //			mode_std_sum_graphicon_y[i].init(              //## chea 270999
	    mode_std_sum_graphicon_t[i].init(
		REPORT_X1+TAB6,
		REPORT_Y1+VTAB1+i*43+30,
		REPORT_X1+TAB6+80,
		REPORT_Y1+VTAB1+i*43+68,
		//					1 ,&data_num ,dataArray[i] , 1, 0, 3);   //## chea 270999
		1 ,&data_numT ,dataArray[i] , 1, 0, 3);

	    //			mode_std_sum_graphicon_y[i].paint();           //## chea 270999
	    mode_std_sum_graphicon_t[i].paint();

	    if(i!=MAX_STUDENT_LEVEL_N_TOTAL-1)
		user_interface.bar(
		    REPORT_X1+TAB3+30,
		    REPORT_Y1+VTAB1+43+i*43,
		    REPORT_X1+TAB3+41,
		    REPORT_Y1+VTAB1+55+i*43,
		    ((std_sum_rank_piechart.series_color)[i]));
	}
    }
    else {
	std_sum_rank_piechart.refresh();
	vga_back.rest_area(text_refresh_bitmap1,0,0);
	for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++) {

	    if ( i==4 )
		continue;
	    font_chartsm.right_put(
		REPORT_X1+TAB4+60,
		REPORT_Y1+VTAB1+i*43+43,
		m.format(rankLevelCount[i],4));

	    // ##### Begin Marco ##### //
	    font_chart_purple_sm.right_put(
		REPORT_X1+TAB5+60,
		REPORT_Y1+VTAB1+i*43+43,
		m.format(initialRankLevelCount[i],4));
	    // ##### End Marco ##### //
	}
	for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++) {
	    //			mode_std_sum_graphicon_y[i].refresh();   //## chea 270999
	    if ( i==4 )
		continue;
	    mode_std_sum_graphicon_t[i].refresh();
	}
    }

    //----------------- show objects ----------------//
    type_button_group.paint(current_report_type);
    // draw the buttons into different color
    int x1, x2, y1, y2;

    for ( int z=3; z<10; z++ ) {
	x1 = type_button_group[z].x1+1;
	x2 = type_button_group[z].x2+1;
	y1 = type_button_group[z].y1+1;
	y2 = type_button_group[z].y2-1;

	user_interface.rect(x1,y1,x2,y2,1,70);
    }

    //------ 9Mar Trevor			//BUGHERE

    int totalCourse=0, totalStudent=0;

    for( int j=department_array.size() ; j>0 ; j-- ) {
	Department* deptPtr = department_array[j];

	for( int i=deptPtr->student_array.size() ; i>0 ; i-- ) {
	    if( deptPtr->student_array.is_deleted(i) )
		continue;

	    totalStudent ++;
	    totalCourse += deptPtr->student_array[i]->total_course_all;
	}
    }

    //----------- paint the summary button group ---------//

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    vga.use_front();
}

//----- End of Summary Report 1 ------//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//----- Begin of Summary Report 2 ------//

void PlayerSchool::summary_student_report_2(int refreshFlag) {
    static int percentDataArray2[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//

    int deptRecno;
    static int data_num=HISTORY_YEAR_COUNT;
    static int data_numT=HISTORY_TRIMESTER_COUNT;   //## chea 290999 update this report
    static int *dataArray[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    int genderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    memset( genderEthnicGroupCount,0, sizeof(genderEthnicGroupCount) );

    // ##### Begin Marco ##### //
    //	static int *initialDataArray[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    //NEW
    int initialDataArray[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    int initialGenderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    memset( initialGenderEthnicGroupCount,0, sizeof(initialGenderEthnicGroupCount) );
    // ##### End Marco ##### //

    //---- determine the department for display the faculty records ----//
    if(current_report_mode==REPORT_MODE_DEPARTMENT) {
	if( department_array.selected_recno )
	    deptRecno = department_array.selected_recno;
	cur_dept_ptr = department_array[deptRecno];

	//-- department wide --//
	for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++) {
	    //## chea 290999 update this report
	    //			dataArray[i]=cur_dept_ptr->student_gender_history[i];
	    dataArray[i]=cur_dept_ptr->dept_gender_student_level_history[i];
	    genderEthnicGroupCount[i]=dataArray[i][data_numT-1];

	    // ##### begin Marco ###### //
	    initialDataArray[i]=cur_dept_ptr->initial_dept_gender_student_level_history[i];
	    //			initialGenderEthnicGroupCount[i]=initialDataArray[i][data_numT-1];
	    //new
	    initialGenderEthnicGroupCount[i]=initialDataArray[i];
	    // ##### End Marco ###### //
	}
    }
    else {
	//-- Institution wide --//
	for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++) {
	    //## chea 290999 update this report
	    //			dataArray[i]=department_array.student_gender_history[i];
	    dataArray[i]=player_school.ins_gender_student_level_history[i];
	    genderEthnicGroupCount[i]=dataArray[i][data_numT-1];

	    // ##### Begin Marco ##### //
	    initialDataArray[i]=player_school.initial_ins_gender_student_level_history[i];
	    //			initialGenderEthnicGroupCount[i]=initialDataArray[i][data_numT-1];
	    //marco new
	    initialGenderEthnicGroupCount[i]=initialDataArray[i];
	    // ##### End Marco ##### //
	}
    }

    int sum=0;
    for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1;i++)
	sum+=genderEthnicGroupCount[i];
    genderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1]=sum;
    // ##### Begin Marco ##### //
    int initialSum=0;
    for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1;i++)
	initialSum+=initialGenderEthnicGroupCount[i];
    initialGenderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1]=initialSum;
    // ##### End MArco ##### //

    if(sum!=0) {
	int diff=200;
	for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++) {
	    percentDataArray2[i]=(genderEthnicGroupCount[i]*100)/sum;
	    diff-=percentDataArray2[i];
	}
	int maxpercent=percentDataArray2[0],maxi=0;
	for(int i=1;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1;i++) {
	    if(percentDataArray2[i]>maxpercent) {
		maxpercent=percentDataArray2[i];
		maxi=i;
	    }
	}
	percentDataArray2[maxi]+=diff;
    }
    else
	for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++)
	    percentDataArray2[i]=0;

    //------ paint the background -------//
    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_student_report_deinit();

	report_init_flag = 1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);
	user_interface.brighten(
	    REPORT_X1+TAB1,
	    REPORT_Y1+VTAB1+20,
	    REPORT_X1+TAB2,
	    REPORT_Y1+VTAB4
	    );

	user_interface.brighten(
	    REPORT_X1+TAB2,
	    REPORT_Y1+VTAB1+20,
	    REPORT_X2,
	    REPORT_Y1+VTAB4);
	user_interface.rect(
	    REPORT_X1+TAB2-2,
	    REPORT_Y1+VTAB1+20,
	    REPORT_X2,
	    REPORT_Y1+VTAB4,1);

	// top title
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	font_charts.put(REPORT_X1+180,REPORT_Y1-8,std_sum_title[1]);

	//----- initialize the bottom sub-menus buttons -----//

	text_refresh_bitmap1=NULL;
	text_refresh_bitmap1=vga_back.save_area(
	    REPORT_X1+TAB4,
	    REPORT_Y1+VTAB1,
	    REPORT_X2-100,
	    REPORT_Y1+VTAB4,
	    text_refresh_bitmap1);

	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;
	font_chartsm.put(REPORT_X1-10,y+2,"Department :");
	font_chartsm.put(REPORT_X1-10,y+30,"  Institution :");
	int i;
	for (i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);                      //  Turnover
	type_button_group[i].create_text(x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y,
					 x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y+BOTTOM_BUTTON_HEIGHT,
					 reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (i = 4; i < 9; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,
						 x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
						 reportTypeStrArray[i]);

	y = BOTTOM_BUTTON_Y1;
	type_button_group[i].create_text(x, y,
					 x+BOTTOM_BUTTON_WIDTH+10, y+BOTTOM_BUTTON_HEIGHT+28,
					 reportTypeStrArray[i]);

	std_sum_gender_ethnic_piechart.init(
	    REPORT_X1+TAB1,
	    REPORT_Y1+VTAB1+20,
	    REPORT_X1+TAB2,
	    REPORT_Y1+VTAB4,
	    4, genderEthnicGroupCount, &xStart,&xEnd,
	    NULL, NULL, NULL, 1, 1, 0, 3);

	std_sum_gender_ethnic_piechart.refresh();

	//-------- draw the legend --------------//

	for(i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++) {
	    font_chartsm.put(
		REPORT_X1+TAB3+60,
		REPORT_Y1+VTAB1+i*47+43,
		std_sum_ge_label[i]);
	    font_chartsm.right_put(
		REPORT_X1+TAB4+60,
		REPORT_Y1+VTAB1+i*47+43,
		m.format(genderEthnicGroupCount[i],4));
	    // ##### Begin Marco ##### //
	    font_chart_purple_sm.right_put(
		REPORT_X1+TAB5+60,
		REPORT_Y1+VTAB1+i*47+43,
		m.format(initialGenderEthnicGroupCount[i],4));

	    // ##### End Marco ##### //
	    //			mode_std_sum_graphicon_y[i].init(            //## chea 290999 update this report
	    mode_std_sum_graphicon_t[i].init(           //## chea 290999 update this report
		REPORT_X1+TAB6,
		REPORT_Y1+VTAB1+i*47+30,
		REPORT_X1+TAB6+80,
		REPORT_Y1+VTAB1+i*47+72,
		//				1 ,&data_num ,dataArray[i] , 1, 0, 3);    //## chea 290999 update this report
		1 ,&data_numT ,dataArray[i] , 1, 0, 3);

	    //			mode_std_sum_graphicon_y[i].paint();         //## chea 290999 update this report
	    mode_std_sum_graphicon_t[i].paint();

	    if(i!=GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1)
		user_interface.bar(
		    REPORT_X1+TAB3+30,
		    REPORT_Y1+VTAB1+43+i*47,
		    REPORT_X1+TAB3+41,
		    REPORT_Y1+VTAB1+55+i*47,
		    ((std_sum_gender_ethnic_piechart.series_color)[i]));
	}
    }
    else {
	std_sum_gender_ethnic_piechart.refresh();
	vga_back.rest_area(text_refresh_bitmap1,0,0);
	for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++) {
	    font_chartsm.right_put(
		REPORT_X1+TAB4+60,
		REPORT_Y1+VTAB1+i*47+43,
		m.format(genderEthnicGroupCount[i],4));
	    // ##### Begin Marco ##### //
	    font_chart_purple_sm.right_put(
		REPORT_X1+TAB5+60,
		REPORT_Y1+VTAB1+i*47+43,
		m.format(initialGenderEthnicGroupCount[i],4));

	    // ##### End Marco ##### //
	    //			mode_std_sum_graphicon_y[i].refresh();
	    mode_std_sum_graphicon_t[i].refresh();      //## chea 290999
	}
    }

    //----------------- show objects ----------------//
    type_button_group.paint(current_report_type);
    // draw the buttons into different color
    int x1, x2, y1, y2;

    for ( int z=3; z<10; z++ ) {
	x1 = type_button_group[z].x1+1;
	x2 = type_button_group[z].x2+1;
	y1 = type_button_group[z].y1+1;
	y2 = type_button_group[z].y2-1;

	user_interface.rect(x1,y1,x2,y2,1,70);
    }
    //----------- paint the summary button group ---------//
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    vga.use_front();
}

//----- End of Summary Report 2 ------//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//----- Begin of Summary Report 3 ------//
void PlayerSchool::summary_student_report_3(int refreshFlag) {
    int tab1=10;
    int tab2=230;
    int tab3=300;
    int tab4=380;
    int tab5=450;
    int tab6=530;
    int tab7=610;
    int vtab1=15;
    int vtab2=160;
    int vtab3=193;
    int vtab4=338;
    static int data_num1=HISTORY_MONTH_COUNT;
    static int data_num2=HISTORY_YEAR_COUNT;
    static int numSeriesData2 = HISTORY_TRIMESTER_COUNT;
    static int v1[3];
    static int v2[3];
    static int v3[3];
    static float v4[3];
    static float v5[3];
    static float v6;
    static int dataArray1[8][HISTORY_MONTH_COUNT];
    static int dataArray2[3][HISTORY_YEAR_COUNT];
    static int dataArray3[8][HISTORY_MONTH_COUNT];
    static float dataArray4[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];
    static float dataArray5[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];
    static float *dataArray6;

    // ##### Begin MArco ##### //
    static int initialv1[3];
    static int initialv2[3];
    static int initialv3[3];
    static float initialv4[3];
    static float initialv5[3];
    static float initialv6;
    int initialDataArray1[8][HISTORY_MONTH_COUNT];
    int initialDataArray2[3][HISTORY_YEAR_COUNT];
    // int initialDataArray3[8][HISTORY_MONTH_COUNT];
    float initialDataArray4[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];
    float initialDataArray5[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];

    memset(initialDataArray1,0,sizeof(initialDataArray1));
    memset(initialDataArray2,0,sizeof(initialDataArray2));
    // ##### End MArco ##### //
    int deptRecno=1;

    memset(dataArray1,0,sizeof(dataArray1));
    memset(dataArray2,0,sizeof(dataArray2));

    //---- determine the department for display the faculty records ----//
    if(current_report_mode==REPORT_MODE_DEPARTMENT) {
	if( department_array.selected_recno )
	    deptRecno = department_array.selected_recno;

	cur_dept_ptr = department_array[deptRecno];

	dataArray6=cur_dept_ptr->p_student_morale_history;

	//-- department wide --//
	for(int j=0;j<HISTORY_MONTH_COUNT;j++) {
	    dataArray1[0][j]=cur_dept_ptr->student_array.talent_academic_all[BACHELOR_DEGREE][j];
	    dataArray1[1][j]=cur_dept_ptr->student_array.talent_academic_all[MASTER_DEGREE][j];
	    dataArray1[2][j]=cur_dept_ptr->student_array.talent_academic_all[DOCTOR_DEGREE][j];
	    dataArray1[3][j]=cur_dept_ptr->student_array.performance_academic_ug[j];

	    // ##### Begin Marco ##### //
	    initialDataArray1[0][j]=cur_dept_ptr->student_array.initial_talent_academic_all[BACHELOR_DEGREE][j];
	    initialDataArray1[1][j]=cur_dept_ptr->student_array.initial_talent_academic_all[MASTER_DEGREE][j];
	    initialDataArray1[2][j]=cur_dept_ptr->student_array.initial_talent_academic_all[DOCTOR_DEGREE][j];
	    initialDataArray1[3][j]=cur_dept_ptr->student_array.initial_performance_academic_ug[j];
	    // ##### End MArco ##### //
	}

	for(int j=0;j<HISTORY_YEAR_COUNT;j++) {
	    dataArray2[0][j]=cur_dept_ptr->student_array.bachelor_degree[j]+
		cur_dept_ptr->student_array.non_ug_bachelor_degree[j];

	    dataArray2[1][j]=cur_dept_ptr->student_array.master_degree[j];
	    dataArray2[2][j]=cur_dept_ptr->student_array.doctor_degree[j];
	    dataArray4[0][j]=cur_dept_ptr->student_array.fake_disp_time_to_degree[BACHELOR_DEGREE][j];
	    dataArray4[1][j]=cur_dept_ptr->student_array.time_to_degree[MASTER_DEGREE][j];
	    dataArray4[2][j]=cur_dept_ptr->student_array.time_to_degree[DOCTOR_DEGREE][j];
	    dataArray5[0][j]=cur_dept_ptr->student_array.dropout_rate[BACHELOR_DEGREE][j];
	    dataArray5[1][j]=cur_dept_ptr->student_array.dropout_rate[MASTER_DEGREE][j];
	    dataArray5[2][j]=cur_dept_ptr->student_array.dropout_rate[DOCTOR_DEGREE][j];

	    // #### Begin Marco ##### //
	    initialDataArray2[0][j]=cur_dept_ptr->student_array.initial_bachelor_degree[j];
	    initialDataArray2[1][j]=cur_dept_ptr->student_array.initial_master_degree[j];
	    initialDataArray2[2][j]=cur_dept_ptr->student_array.initial_doctor_degree[j];
	    initialDataArray4[0][j]=cur_dept_ptr->student_array.initial_fake_disp_time_to_degree[BACHELOR_DEGREE][j];
	    initialDataArray4[1][j]=cur_dept_ptr->student_array.initial_time_to_degree[MASTER_DEGREE][j];
	    initialDataArray4[2][j]=cur_dept_ptr->student_array.initial_time_to_degree[DOCTOR_DEGREE][j];
	    initialDataArray5[0][j]=cur_dept_ptr->student_array.initial_dropout_rate[BACHELOR_DEGREE][j];
	    initialDataArray5[1][j]=cur_dept_ptr->student_array.initial_dropout_rate[MASTER_DEGREE][j];
	    initialDataArray5[2][j]=cur_dept_ptr->student_array.initial_dropout_rate[DOCTOR_DEGREE][j];
	    // ##### End MArco ##### //
	}
	for(int j=0;j<3;j++) {
	    v1[j]=dataArray1[j][HISTORY_MONTH_COUNT-1];
	    v3[j]=dataArray2[j][HISTORY_YEAR_COUNT-1];
	    v4[j]=dataArray4[j][HISTORY_YEAR_COUNT-1];
	    v5[j]=dataArray5[j][HISTORY_YEAR_COUNT-1];

	    // #### Begin MArco #### //
	    initialv1[j]=initialDataArray1[j][HISTORY_MONTH_COUNT-1];
	    initialv3[j]=initialDataArray2[j][HISTORY_YEAR_COUNT-1];
	    initialv4[j]=initialDataArray4[j][HISTORY_YEAR_COUNT-1];
	    initialv5[j]=initialDataArray5[j][HISTORY_YEAR_COUNT-1];
	    // ##### End MArco #### //
	}
	v2[0]=dataArray1[3][HISTORY_MONTH_COUNT-1];
	v6 = dataArray6[HISTORY_TRIMESTER_COUNT-1];
	initialv6 = cur_dept_ptr->initial_p_student_morale;

	// ##### Begin MArco #####//
	initialv2[0]=initialDataArray1[3][HISTORY_MONTH_COUNT-1];
	// ##### End Marco ##### //
    }
    else {
	//-- Institution wide --//
	dataArray6=department_array.p_student_morale_history;

	for(int j=0;j<HISTORY_MONTH_COUNT;j++) {
	    dataArray1[0][j]=player_school.talent_academic_all[BACHELOR_DEGREE][j];
	    dataArray1[1][j]=player_school.talent_academic_all[MASTER_DEGREE][j];
	    dataArray1[2][j]=player_school.talent_academic_all[DOCTOR_DEGREE][j];
	    dataArray1[3][j]=player_school.performance_academic_ug[j];

	    // ##### Begin Marco ##### //
	    initialDataArray1[0][j]=player_school.initial_talent_academic_all[BACHELOR_DEGREE][j];
	    initialDataArray1[1][j]=player_school.initial_talent_academic_all[MASTER_DEGREE][j];
	    initialDataArray1[2][j]=player_school.initial_talent_academic_all[DOCTOR_DEGREE][j];
	    initialDataArray1[3][j]=player_school.initial_performance_academic_ug[j];
	    // ##### End Marco ##### //
	}

	for(int j=0;j<HISTORY_YEAR_COUNT;j++) {
	    dataArray2[0][j]=player_school.bachelor_degree[j]+
		player_school.non_ug_bachelor_degree[j];

	    dataArray2[1][j]=player_school.master_degree[j];
	    dataArray2[2][j]=player_school.doctor_degree[j];
	    dataArray4[0][j]=player_school.fake_disp_time_to_degree[BACHELOR_DEGREE][j];
	    dataArray4[1][j]=player_school.time_to_degree[MASTER_DEGREE][j];
	    dataArray4[2][j]=player_school.time_to_degree[DOCTOR_DEGREE][j];
	    dataArray5[0][j]=player_school.dropout_rate[BACHELOR_DEGREE][j];
	    dataArray5[1][j]=player_school.dropout_rate[MASTER_DEGREE][j];
	    dataArray5[2][j]=player_school.dropout_rate[DOCTOR_DEGREE][j];

	    // ##### begin MArco ##### //
	    initialDataArray2[0][j]=player_school.initial_bachelor_degree[j];
	    initialDataArray2[1][j]=player_school.initial_master_degree[j];
	    initialDataArray2[2][j]=player_school.initial_doctor_degree[j];
	    //NEW
	    initialDataArray4[0][j]=player_school.initial_fake_disp_time_to_degree[j];
	    initialDataArray4[1][j]=player_school.initial_time_to_degree[MASTER_DEGREE][j];
	    initialDataArray4[2][j]=player_school.initial_time_to_degree[DOCTOR_DEGREE][j];
	    initialDataArray5[0][j]=player_school.initial_dropout_rate[BACHELOR_DEGREE][j];
	    initialDataArray5[1][j]=player_school.initial_dropout_rate[MASTER_DEGREE][j];
	    initialDataArray5[2][j]=player_school.initial_dropout_rate[DOCTOR_DEGREE][j];
	    // ##### End Marco ##### //
	}

	for(int j=0;j<3;j++) {
	    v1[j]=dataArray1[j][HISTORY_MONTH_COUNT-1];
	    v3[j]=dataArray2[j][HISTORY_YEAR_COUNT-1];
	    v4[j]=dataArray4[j][HISTORY_YEAR_COUNT-1];
	    v5[j]=dataArray5[j][HISTORY_YEAR_COUNT-1];

	    // ##### begin Marco ##### //
	    initialv1[j]=initialDataArray1[j][HISTORY_MONTH_COUNT-1];
	    initialv3[j]=initialDataArray2[j][HISTORY_YEAR_COUNT-1];
	    initialv4[j]=initialDataArray4[j][HISTORY_YEAR_COUNT-1];
	    initialv5[j]=initialDataArray5[j][HISTORY_YEAR_COUNT-1];
	    // ##### /End Marco ##### //
	}
	v2[0]=dataArray1[3][HISTORY_MONTH_COUNT-1];
	v6 = dataArray6[HISTORY_TRIMESTER_COUNT-1];
	initialv6 = player_school.sub_score_start[S_STUDENT_MORALE];
	// marco
	initialv2[0]=initialDataArray1[3][HISTORY_MONTH_COUNT-1];
    }

    //------ paint the background -------//
    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {

	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_student_report_deinit();
	report_init_flag =1;

	// paint the background image
	user_interface.bg_img(BG_PIC_ID, &vga_back);
	// the big lower rectangle of table 1
	user_interface.brighten(REPORT_X1+tab1-5,REPORT_Y1+vtab1+4,
				REPORT_X2-tab1,REPORT_Y1+vtab2);
	// the small upper rectangle of table 1
	user_interface.brighten(REPORT_X1+tab2,REPORT_Y1+vtab1-13,
				REPORT_X2-tab1,REPORT_Y1+vtab1+4);

	// the big lower rectangle of table 2
	user_interface.brighten(REPORT_X1+tab1-5,REPORT_Y1+vtab3+4,
				REPORT_X2-tab1,REPORT_Y1+vtab4);
	// the small upper rectangle of table 2
	user_interface.brighten(REPORT_X1+tab2,REPORT_Y1+vtab3-30,
				REPORT_X2-tab1,REPORT_Y1+vtab3+4);

	user_interface.rect(  REPORT_X1+tab2,REPORT_Y1+vtab1-15,
			      REPORT_X1+tab4+3,REPORT_Y1+vtab2,1);
	user_interface.rect(  REPORT_X1+tab4,REPORT_Y1+vtab1-15,
			      REPORT_X1+tab6+3,REPORT_Y1+vtab2,1);
	user_interface.rect(  REPORT_X1+tab6,REPORT_Y1+vtab1-15,
			      REPORT_X2-tab1,REPORT_Y1+vtab2,1);
	user_interface.rect(  REPORT_X1+tab1-5,REPORT_Y1+vtab1+4,
			      REPORT_X2-tab1,REPORT_Y1+vtab2,1);

	user_interface.rect(  REPORT_X1+tab2,REPORT_Y1+vtab3-30,
			      REPORT_X1+tab4+3,REPORT_Y1+vtab4,1);
	user_interface.rect(  REPORT_X1+tab4,REPORT_Y1+vtab3-30,
			      REPORT_X1+tab6+3,REPORT_Y1+vtab4,1);
	user_interface.rect(  REPORT_X1+tab6,REPORT_Y1+vtab3-30,
			      REPORT_X2-tab1,REPORT_Y1+vtab4,1);
	user_interface.rect(  REPORT_X1+tab1-5,REPORT_Y1+vtab3+4,
			      REPORT_X2-tab1,REPORT_Y1+vtab4,1);

	font_chartsm.center_put(REPORT_X1+tab2,REPORT_Y1+vtab1-11,
				REPORT_X1+tab4,REPORT_Y1+vtab1+4,
				std_sum_title3[0]);
	font_chartsm.center_put(REPORT_X1+tab4+8,REPORT_Y1+vtab1-11,
				REPORT_X1+tab6,REPORT_Y1+vtab1+4,
				std_sum_title3[1]);
	font_chartsm.put_paragraph( REPORT_X1+tab2+10,REPORT_Y1+vtab3-26,
				    REPORT_X1+tab4,REPORT_Y1+vtab4+5,
				    std_sum_title3[2],-2);

	font_chartsm.center_put(REPORT_X1+tab6,REPORT_Y1+vtab3-19,
				REPORT_X2-1,REPORT_Y1+vtab3,
				std_sum_title3[4]);
	font_chartsm.center_put(REPORT_X1+tab6, REPORT_Y1+vtab1-11,
				REPORT_X2-1,REPORT_Y1+vtab1+4,
				std_sum_title3[5]);

	font_chartsm.put(REPORT_X1+tab1+5,REPORT_Y1+vtab1+20,std_sum_label2[0]);
	font_chartsm.put(REPORT_X1+tab1+5,REPORT_Y1+vtab1+70,std_sum_label2[1]);
	font_chartsm.put(REPORT_X1+tab1+5,REPORT_Y1+vtab1+120,std_sum_label2[2]);
	font_chartsm.put(REPORT_X1+tab1+5,REPORT_Y1+vtab3+20,std_sum_label2[0]);
	font_chartsm.put(REPORT_X1+tab1+5,REPORT_Y1+vtab3+70,std_sum_label2[1]);
	font_chartsm.put(REPORT_X1+tab1+5,REPORT_Y1+vtab3+120,std_sum_label2[2]);

	user_interface.brighten(REPORT_X1+tab1,REPORT_Y1-13,REPORT_X1+tab2-4,REPORT_Y1+10);
	user_interface.rect(REPORT_X1+tab1,REPORT_Y1-13,REPORT_X1+tab2-4,REPORT_Y1+10,1);
	font_charts.put(REPORT_X1+60,REPORT_Y1-8,"Student Activities");

	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;
	font_chartsm.put(REPORT_X1-10,y+2,"Department :");
	font_chartsm.put(REPORT_X1-10,y+30,"  Institution :");
	int i;
	for (i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);                      //  Turnover
	type_button_group[i].create_text(x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y,
					 x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y+BOTTOM_BUTTON_HEIGHT,
					 reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (i = 4; i < 9; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,
						 x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
						 reportTypeStrArray[i]);

	y = BOTTOM_BUTTON_Y1;
	type_button_group[i].create_text(x, y,
					 x+BOTTOM_BUTTON_WIDTH+10, y+BOTTOM_BUTTON_HEIGHT+28,
					 reportTypeStrArray[i]);

	mode_std_sum_graphicon_m[0].init(REPORT_X1+tab3-5,REPORT_Y1+vtab1+9,
					 REPORT_X1+tab3+75,REPORT_Y1+vtab1+49,
					 1 ,&data_num1 ,dataArray1[0] , 1, 0, 3);
	mode_std_sum_graphicon_m[1].init(REPORT_X1+tab3-5,REPORT_Y1+vtab1+54,
					 REPORT_X1+tab3+75,REPORT_Y1+vtab1+94,
					 1 ,&data_num1 ,dataArray1[1] , 1, 0, 3);
	mode_std_sum_graphicon_m[2].init(REPORT_X1+tab3-5,REPORT_Y1+vtab1+100,
					 REPORT_X1+tab3+75,REPORT_Y1+vtab1+140,
					 1 ,&data_num1 ,dataArray1[2] , 1, 0, 3);

	mode_std_sum_graphicon_m[3].init(REPORT_X1+tab5-5,REPORT_Y1+vtab1+9,
					 REPORT_X1+tab5+75,REPORT_Y1+vtab1+49,
					 1 ,&data_num1 ,dataArray1[3] , 1, 0, 3);

	mode_std_sum_graphicon_y[0].init(REPORT_X1+tab3-5,REPORT_Y1+vtab3+9,
					 REPORT_X1+tab3+75,REPORT_Y1+vtab3+49,
					 1 ,&data_num2 ,dataArray2[0] , 1, 0, 3);
	mode_std_sum_graphicon_y[1].init(REPORT_X1+tab3-5,REPORT_Y1+vtab3+54,
					 REPORT_X1+tab3+75,REPORT_Y1+vtab3+94,
					 1 ,&data_num2 ,dataArray2[1] , 1, 0, 3);
	mode_std_sum_graphicon_y[2].init(REPORT_X1+tab3-5,REPORT_Y1+vtab3+100,
					 REPORT_X1+tab3+75,REPORT_Y1+vtab3+140,
					 1 ,&data_num2 ,dataArray2[2] , 1, 0, 3);

	mode_std_sum_graphicon_y[6].init(REPORT_X2-95,REPORT_Y1+vtab3+9,
					 REPORT_X2-15,REPORT_Y1+vtab3+49,
					 1 ,&data_num2 ,dataArray5[0] , 1, 0, 3);

	mode_std_sum_student_morale.init(
	    REPORT_X2-95,REPORT_Y1+vtab1+9,
	    REPORT_X2-15,REPORT_Y1+vtab1+49,
	    1, &numSeriesData2, dataArray6, 1,
	    0, 3);

	//----------------- show objects ----------------//
	type_button_group.paint(current_report_type);
	// draw the buttons into different color
	int x1, x2, y1, y2;

	for ( int z=3; z<10; z++ ) {
	    x1 = type_button_group[z].x1+1;
	    x2 = type_button_group[z].x2+1;
	    y1 = type_button_group[z].y1+1;
	    y2 = type_button_group[z].y2-1;

	    user_interface.rect(x1,y1,x2,y2,1,70);
	}

	for(i=0;i<4;i++)
	    mode_std_sum_graphicon_m[i].paint();

	for(i=0;i<9;i++) {
	    if( i==4 || i==5 || i==7 || i==8 )          // these graphs have been removed.
		continue;

	    if ( i== 3 )
		continue;

	    mode_std_sum_graphicon_y[i].paint();
	}

	mode_std_sum_student_morale.paint();

	for(i=0;i<3;i++) {
	    // ##### Begin Marco ##### //
	    std_sum_text[i].right_put(
		REPORT_X1+tab2+30,REPORT_Y1+vtab1+i*50+20,&v1[i],3);
	    std_sum_text[i+4].right_put(
		REPORT_X1+tab2+30,REPORT_Y1+vtab3+i*50+20,&v3[i],4);
	    font_chart_purple_sm.right_put(
		REPORT_X1+tab2+60,REPORT_Y1+vtab1+i*50+20,m.format(initialv1[i],3));

	    font_chart_purple_sm.right_put(
		REPORT_X1+tab2+60,REPORT_Y1+vtab3+i*50+20,m.format(initialv3[i],4));
	    // ##### End Marco ##### //

	    if( i==0 ) {                                // only show 0, the others have been removed
		// ##### Begin Marco ##### //
		//				std_sum_text[i+7].right_put(
		//					REPORT_X1+tab4+30,REPORT_Y1+vtab3+i*50+20,&v4[i],26);

		std_sum_text[i+10].right_put(
		    REPORT_X1+tab6+40,REPORT_Y1+vtab3+i*50+20,&v5[i],4);

		std_sum_text[i+14].right_put(
		    REPORT_X1+tab6+40,REPORT_Y1+vtab1+20,&v6,3);

		//				font_chart_purple_sm.right_put(
		//					REPORT_X1+tab4+60,REPORT_Y1+vtab3+i*50+20,m.format(initialv4[i],26));

		font_chart_purple_sm.right_put(
		    REPORT_X1+tab6+70,REPORT_Y1+vtab3+i*50+20,m.format(initialv5[i],4));

		font_chart_purple_sm.right_put(
		    REPORT_X1+tab6+70,REPORT_Y1+vtab1+20,m.format(initialv6,3));

		// ##### End Marco ##### //
	    }
	}

	std_sum_text[3].right_put(REPORT_X1+tab4+30,REPORT_Y1+vtab1+20,&v2[0],3);

	// ##### Begin Marco ##### //
	font_chart_purple_sm.right_put(REPORT_X1+tab4+60,REPORT_Y1+vtab1+20,m.format(initialv2[0],3));

	// ##### End Marco ##### //

	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

    }
    else {
	for(int i=0;i<4;i++)
	    mode_std_sum_graphicon_m[i].refresh();

	for(int i=0;i<9;i++) {
	    if( i==4 || i==5 || i==7 || i==8 )          // these graphs have been removed.
		continue;

	    if ( i == 3 )
		continue;

	    mode_std_sum_graphicon_y[i].refresh();
	}

	for(int i=0;i<3;i++) {
	    std_sum_text[i].refresh();
	    std_sum_text[i+4].refresh();

	    // ##### Begin Marco ##### //
	    font_chart_purple_sm.right_put(
		REPORT_X1+tab2+60,REPORT_Y1+vtab1+i*50+20,m.format(initialv1[i],3));

	    font_chart_purple_sm.right_put(
		REPORT_X1+tab2+60,REPORT_Y1+vtab3+i*50+20,m.format(initialv3[i],4));
	    // ##### End Marco ##### //

	    if( i==0 ) {                                // only show 0, the others have been removed
		std_sum_text[i+7].refresh();
		std_sum_text[i+10].refresh();

		// ##### Begin Marco ##### //
		//				font_chart_purple_sm.right_put(
		//						REPORT_X1+tab4+60,REPORT_Y1+vtab3+i*50+20,m.format(initialv4[i],26));

		font_chart_purple_sm.right_put(
		    REPORT_X1+tab6+70,REPORT_Y1+vtab3+i*50+20,m.format(initialv5[i],4));
		// ##### End Marco ##### //
	    }
	}
	std_sum_text[3].refresh();
	// ##### Begin Marco ##### //
	font_chart_purple_sm.right_put(REPORT_X1+tab4+60,REPORT_Y1+vtab1+20,m.format(initialv2[0],3));
	// ##### End Marco ##### //

	for(int i=0;i<3;i++) {
	    vga.blt_buf(
		REPORT_X1+tab2,REPORT_Y1+vtab1+i*50+20,
		REPORT_X1+tab2+60,REPORT_Y1+vtab1+i*50+32);
	    vga.blt_buf(
		REPORT_X1+tab2,REPORT_Y1+vtab3+i*50+20,
		REPORT_X1+tab2+60,REPORT_Y1+vtab3+i*50+32
		);
	    vga.blt_buf(
		REPORT_X1+tab4,REPORT_Y1+vtab3+i*50+20,
		REPORT_X1+tab4+60,REPORT_Y1+vtab3+i*50+32
		);
	    vga.blt_buf(
		REPORT_X1+tab6,REPORT_Y1+vtab3+i*50+20,
		REPORT_X1+tab6+70,REPORT_Y1+vtab3+i*50+32
		);
	}
	vga.blt_buf(REPORT_X1+tab4,REPORT_Y1+vtab1+20,
		    REPORT_X1+tab4+60,REPORT_Y1+vtab1+32);

	// refresh the graphs
	vga.blt_buf(REPORT_X1+tab3-5,REPORT_Y1+vtab1+9,
		    REPORT_X1+tab3+75,REPORT_Y1+vtab1+140);
	vga.blt_buf(REPORT_X1+tab5-5,REPORT_Y1+vtab1+9,
		    REPORT_X1+tab5+75,REPORT_Y1+vtab1+140);

	vga.blt_buf(REPORT_X1+tab3-5,REPORT_Y1+vtab3+9,
		    REPORT_X1+tab3+75,REPORT_Y1+vtab3+140);
	vga.blt_buf(REPORT_X1+tab5-5,REPORT_Y1+vtab3+9,
		    REPORT_X1+tab5+75,REPORT_Y1+vtab3+140);
	vga.blt_buf(REPORT_X2-95,REPORT_Y1+vtab3+9,
		    REPORT_X2-15,REPORT_Y1+vtab3+140);
    }

    vga.use_front();
}

//----- End of Summary Report 3 ------//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//----- Begin of Summary Report 4 ------//
void PlayerSchool::summary_student_report_4(int refreshFlag) {
    int tab1=10;
    int tab2=230;
    int tab3=300;
    int tab4=380;
    int tab5=450;
    int tab6=530;
    int tab7=610;
    int vtab1=15;
    int vtab2=160;
    int vtab3=82;
    int vtab4=258;
    static int data_num1=HISTORY_MONTH_COUNT;
    static int data_num2=HISTORY_YEAR_COUNT;
    static int v1[3];
    static int v2[3];
    static int v3[3];
    static int v4[7];

    static int dataArray1[8][HISTORY_MONTH_COUNT];
    static int dataArray2[3][HISTORY_YEAR_COUNT];
    static int dataArray3[8][HISTORY_MONTH_COUNT];
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//
    int deptRecno=1;

    memset(dataArray1,0,sizeof(dataArray1));
    memset(dataArray2,0,sizeof(dataArray2));

    // ##### Begin Marco ###### //
    static int initialv1[3];
    static int initialv2[3];
    static int initialv3[3];
    static int initialv4[7];

    static int initialDataArray1[8][HISTORY_MONTH_COUNT];
    static int initialDataArray2[3][HISTORY_YEAR_COUNT];
    static int initialDataArray3[8][HISTORY_MONTH_COUNT];

    memset(initialDataArray1,0,sizeof(initialDataArray1));
    memset(initialDataArray2,0,sizeof(initialDataArray2));
    // #####  End Marco ##### //

    //-- Institution wide --//
    for(int j=0;j<HISTORY_MONTH_COUNT;j++) {
	//## 071299 chea 1.12.1
	//		dataArray3[0][j]=player_school.talent_academic[j];
	//		dataArray3[1][j]=player_school.talent_extracurricular[j];
	//		dataArray3[2][j]=player_school.talent_athletics[j];
	dataArray3[0][j]= max(0,min(100,player_school.talent_academic[j]));
	dataArray3[1][j]= max(0,min(100,player_school.talent_extracurricular[j]));
	dataArray3[2][j]= max(0,min(100,player_school.talent_athletics[j]));

	dataArray3[3][j]=player_school.satisfaction_academic[j];
	dataArray3[4][j]=player_school.satisfaction_student_life[j];
	dataArray3[5][j]=player_school.satisfaction_athletics[j];
	dataArray3[6][j]=player_school.satisfaction_overall[j];

	// ##### Begin Marco ##### //
	initialDataArray3[0][j]= max(0,min(100,player_school.initial_talent_academic[j]));
	initialDataArray3[1][j]= max(0,min(100,player_school.initial_talent_extracurricular[j]));
	initialDataArray3[2][j]= max(0,min(100,player_school.initial_talent_athletics[j]));

	initialDataArray3[3][j]=player_school.initial_satisfaction_academic[j];
	initialDataArray3[4][j]=player_school.initial_satisfaction_student_life[j];
	initialDataArray3[5][j]=player_school.initial_satisfaction_athletics[j];
	initialDataArray3[6][j]=player_school.initial_satisfaction_overall[j];
	// #### End Marco ##### //
    }

    for(int j=0;j<7;j++) {
	// #### Begin MArco #### //
	v4[j]=dataArray3[j][HISTORY_MONTH_COUNT-1];
	initialv4[j]=initialDataArray3[j][HISTORY_MONTH_COUNT-1];
    }
    // ##### End Marco ##### //

    //------ paint the background -------//
    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {

	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_student_report_deinit();
	report_init_flag =1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+7);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+7,1);
	font_charts.put(REPORT_X1+290,REPORT_Y1-8,"Undergraduate Profile");

	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;
	font_chartsm.put(REPORT_X1-10,y+2,"Department :");
	font_chartsm.put(REPORT_X1-10,y+30,"  Institution :");
	int i;
	for (i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
    //  Turnover
	type_button_group[i].create_text(x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y,
					 x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y+BOTTOM_BUTTON_HEIGHT,
					 reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (i = 4; i < 9; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,
						 x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
						 reportTypeStrArray[i]);

	y = BOTTOM_BUTTON_Y1;
	type_button_group[i].create_text(x, y,
					 x+BOTTOM_BUTTON_WIDTH+10, y+BOTTOM_BUTTON_HEIGHT+28,
					 reportTypeStrArray[i]);

	//----------------- show objects ----------------//
	type_button_group.paint(current_report_type);
	// draw the buttons into different color
	int x1, x2, y1, y2;

	for ( int z=3; z<10; z++ ) {
	    x1 = type_button_group[z].x1+1;
	    x2 = type_button_group[z].x2+1;
	    y1 = type_button_group[z].y1+1;
	    y2 = type_button_group[z].y2-1;

	    user_interface.rect(x1,y1,x2,y2,1,70);
	}

	user_interface.brighten(
	    REPORT_X1+tab1-5,REPORT_Y1+vtab3,
	    REPORT_X2-tab1,REPORT_Y1+vtab4+2);

	user_interface.rect(  REPORT_X1+tab1-5,REPORT_Y1+vtab3,
			      REPORT_X2-tab1,REPORT_Y1+vtab4+2,1);
	user_interface.rect(  REPORT_X1+tab4-2,REPORT_Y1+vtab3,
			      REPORT_X2-tab1,REPORT_Y1+vtab4+2,1);
	user_interface.rect(  REPORT_X1+tab1-5,REPORT_Y1+vtab3,
			      REPORT_X2-tab1,REPORT_Y1+vtab3+23,1);

	font_chartsm.put_paragraph( REPORT_X1+tab1+5,REPORT_Y1+vtab3+5,
				    REPORT_X1+tab2+100,REPORT_Y1+vtab3+35,
				    std_sum_title2[3]);
	font_chartsm.put_paragraph( REPORT_X1+tab4+5,REPORT_Y1+vtab3+5,
				    REPORT_X2,REPORT_Y1+vtab3+35,
				    std_sum_title2[4]);

	font_chartsm.put(REPORT_X1+tab1+50,REPORT_Y1+vtab3+40,std_sum_label2[3]);
	font_chartsm.put(REPORT_X1+tab1+50,REPORT_Y1+vtab3+85,std_sum_label2[4]);
	font_chartsm.put(REPORT_X1+tab1+50,REPORT_Y1+vtab3+130,std_sum_label2[5]);
	font_chartsm.put(REPORT_X1+tab4+25,REPORT_Y1+vtab3+40,std_sum_label2[6]);
	font_chartsm.put(REPORT_X1+tab4+25,REPORT_Y1+vtab3+75,std_sum_label2[7]);
	font_chartsm.put(REPORT_X1+tab4+25,REPORT_Y1+vtab3+110,std_sum_label2[8]);

	font_chartsm.put_paragraph(REPORT_X1+tab4+25,REPORT_Y1+vtab3+134,
				   REPORT_X1+tab4+250,REPORT_Y1+vtab3+167,std_sum_label2[9],-1);

	mode_std_sum_graphicon_m[0].init(REPORT_X1+tab3-5,REPORT_Y1+vtab3+30,
					 REPORT_X1+tab3+75,REPORT_Y1+vtab3+70,
					 1 ,&data_num1 ,dataArray3[0] , 1, 0, 3);
	mode_std_sum_graphicon_m[1].init(REPORT_X1+tab3-5,REPORT_Y1+vtab3+77,
					 REPORT_X1+tab3+75,REPORT_Y1+vtab3+117,
					 1 ,&data_num1 ,dataArray3[1] , 1, 0, 3);
	mode_std_sum_graphicon_m[2].init(REPORT_X1+tab3-5,REPORT_Y1+vtab3+124,
					 REPORT_X1+tab3+75,REPORT_Y1+vtab3+164,
					 1 ,&data_num1 ,dataArray3[2] , 1, 0, 3);
	//## chea 071299 1.12.2
	mode_std_sum_graphicon_m[3].init(REPORT_X1+tab7-5,REPORT_Y1+vtab3+26,
					 REPORT_X1+tab7+80,REPORT_Y1+vtab3+58,
					 //						1 ,&data_num1 ,dataArray3[0] , 1, 0, 3);
					 1 ,&data_num1 ,dataArray3[3] , 1, 0, 3);
	mode_std_sum_graphicon_m[4].init(REPORT_X1+tab7-5,REPORT_Y1+vtab3+62,
					 REPORT_X1+tab7+80,REPORT_Y1+vtab3+94,
					 //						1 ,&data_num1 ,dataArray3[1] , 1, 0, 3);
					 1 ,&data_num1 ,dataArray3[4] , 1, 0, 3);
	mode_std_sum_graphicon_m[5].init(REPORT_X1+tab7-5,REPORT_Y1+vtab3+98,
					 REPORT_X1+tab7+80,REPORT_Y1+vtab3+130,
					 //						1 ,&data_num1 ,dataArray3[2] , 1, 0, 3);
					 1 ,&data_num1 ,dataArray3[5] , 1, 0, 3);
	mode_std_sum_graphicon_m[6].init(REPORT_X1+tab7-5,REPORT_Y1+vtab3+134,
					 REPORT_X1+tab7+80,REPORT_Y1+vtab3+166,
					 //						1 ,&data_num1 ,dataArray3[3] , 1, 0, 3);
					 1 ,&data_num1 ,dataArray3[6] , 1, 0, 3);

	for(i=0;i<7;i++)
	    mode_std_sum_graphicon_m[i].paint();

	for(i=0;i<3;i++) {
	    // ##### Begin Marco ##### //
	    std_sum_text2[i].right_put(
		REPORT_X1+tab2+20,
		REPORT_Y1+vtab3+i*45+40,&v4[i],
		3);
	    std_sum_text2[i+3].right_put(
		REPORT_X1+tab6+30,
		REPORT_Y1+vtab3+i*35+40,
		&v4[i+3],
		4);
	    font_chart_purple_sm.right_put(
		REPORT_X1+tab2+60,
		REPORT_Y1+vtab3+i*45+40,
		m.format(initialv4[i],3));
	    font_chart_purple_sm.right_put(
		REPORT_X1+tab6+60,
		REPORT_Y1+vtab3+i*35+40,
		m.format(initialv4[i+3],4));

	    // ##### End Marco ##### //
	}

	// ##### Begin Marco ##### //
	std_sum_text2[6].right_put(
	    REPORT_X1+tab6+30,
	    REPORT_Y1+vtab3+145,
	    &v4[6],
	    4);
	font_chart_purple_sm.right_put(
	    REPORT_X1+tab6+60,REPORT_Y1+vtab3+145,
	    m.format(initialv4[6],4));
	// ##### End Marco ##### //
    }
    else {
	for(int i=0;i<7;i++)
	    mode_std_sum_graphicon_m[i].refresh();
	for(int i=0;i<3;i++) {
	    std_sum_text2[i].refresh();
	    std_sum_text2[i+3].refresh();

	    // ##### Begin Marco ##### //
	    font_chart_purple_sm.right_put(
		REPORT_X1+tab2+60,
		REPORT_Y1+vtab3+i*45+40,
		m.format(initialv4[i],3));
	    font_chart_purple_sm.right_put(
		REPORT_X1+tab6+60,
		REPORT_Y1+vtab3+i*35+40,
		m.format(initialv4[i+3],4));
	    // ##### End Marco ##### //
	}
	std_sum_text2[6].refresh();

	// ##### Begin Marco ##### //
	font_chart_purple_sm.right_put(
	    REPORT_X1+tab6+60,REPORT_Y1+vtab3+145,
	    m.format(initialv4[6],4));
	// ##### End Marco ##### //
    }

    //----------- paint the summary button group ---------//

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

    vga.use_front();
}

//----- End of Summary Report 4 ------//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//----- Begin of Summary Report 5 ------//
void PlayerSchool::summary_student_report_5(int refreshFlag) {

    static int v11=1000,v12=1999,v13=8888;
    static double v21=663.23,v22=123.23,v23=17.23,v24=723.23,v25=13.23;
    static double v31=663.23,v32=123.23,v33=17.23,v34=723.23,v35=13.23;
    static double v3=0.00;
    static int sliderVar1=0;
    //### begin zhoubin 02/25/99
    //	static float spinnerVar1=0.0;
    //	static float spinnerVar2=0.0;
    //### end zhoubin 02/25/99
    static int numSeriesData1 = HISTORY_YEAR_COUNT;
    static int numSeriesData2 = HISTORY_MONTH_COUNT;
    static int *dataArray1,*dataArray2;
    static int dataArray3[HISTORY_MONTH_COUNT];

    //	Communicate with Sim Engine's Output
    dataArray1=student_office.tuition_income_history;
    dataArray2=student_office.student_life_history;

    for(int i=0;i<HISTORY_MONTH_COUNT;i++)
	dataArray3[i]=player_school.satisfaction_student_life[i];

    v12=info.financial_year();
    v11=v12-1;
    v13=v12+1;
    v21=finance.revenue_array[AC_GROSS_TUITION_INCOME].total;
    v22=finance.projected_revenue_array[AC_GROSS_TUITION_INCOME].change_last_year.total;
    v23=finance.projected_revenue_array[AC_GROSS_TUITION_INCOME].this_year.total;
    v24=finance.projected_revenue_array[AC_GROSS_TUITION_INCOME].change_next_year.total;
    v25=finance.projected_revenue_array[AC_GROSS_TUITION_INCOME].next_year.total;
    v31=finance.expense_array[AC_STUDENT_LIFE].total;
    v32=finance.projected_expense_array[AC_STUDENT_LIFE].change_last_year.total;
    v33=finance.projected_expense_array[AC_STUDENT_LIFE].this_year.total;
    v34=finance.projected_expense_array[AC_STUDENT_LIFE].change_next_year.total;
    v35=finance.projected_expense_array[AC_STUDENT_LIFE].next_year.total;
    ///////////////
    v3=dataArray3[HISTORY_MONTH_COUNT-1];

    // ##### Begin Marco  ##### //
    static double initialv21=663.23;
    static double initialv31=663.23;
    initialv21=finance.initial_revenue_array[AC_GROSS_TUITION_INCOME].total;
    initialv31=finance.initial_expense_array[AC_STUDENT_LIFE].total;
    // ##### end MArco ##### //

    int field_space=8*font_chartsm.max_font_width;

    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	// Update during repaint only
	//		spinnerVar1=(float)finance.get_policy_var(true, AC_GROSS_TUITION_INCOME)->target_value;
	//		spinnerVar2=(float)finance.get_policy_var(false, AC_STUDENT_LIFE)->target_value;

	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_student_report_deinit();
	report_init_flag =1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);
	//--------- draw the brightened boxes ------------//
	user_interface.brighten(REPORT_X1, REPORT_Y1+15-5, REPORT_X2, REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT);
	user_interface.brighten(REPORT_X1,REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT,
				REPORT_X1+MODE_FINANCIAL_TAB4-2,REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+MODE_FINANCIAL_BOX2_HEIGHT);
	user_interface.brighten(REPORT_X1+MODE_FINANCIAL_TAB4+3,REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT,
				REPORT_X2,REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+MODE_FINANCIAL_BOX2_HEIGHT);

	user_interface.rect(REPORT_X1, REPORT_Y1+15-5, REPORT_X2, REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT,1);
	user_interface.rect(REPORT_X1,
			    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT,
			    REPORT_X1+MODE_FINANCIAL_TAB4+3,
			    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+MODE_FINANCIAL_BOX2_HEIGHT/2,1);
	user_interface.rect(REPORT_X1,
			    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+MODE_FINANCIAL_BOX2_HEIGHT/2,
			    REPORT_X1+MODE_FINANCIAL_TAB4+3,
			    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+MODE_FINANCIAL_BOX2_HEIGHT,1);

	user_interface.rect(
	    REPORT_X1+MODE_FINANCIAL_TAB4,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X2,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+MODE_FINANCIAL_BOX2_HEIGHT,1);

	user_interface.rect(
	    REPORT_X1+MODE_FINANCIAL_TAB4,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X2,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+38,1);

	short y = FINANCIAL_BUTTON_Y1-40;
	for (int i = 0; i < 3; i++, y+=FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE) {
	    user_interface.create_button_bitmap(FINANCIAL_BUTTON_X1,y,
						FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT,
						financial_button_label[i], &(financial_button1_up_bitmap[i]),
						&(financial_button1_down_bitmap[i]), &vga_back);

	    financial_button_group1[i].create_bitmapW(FINANCIAL_BUTTON_X1,y,
						      FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT,
						      (financial_button1_up_bitmap[i])+4,0);
	}

	y = FINANCIAL_BUTTON2_Y1-60;
	for (int i = 0; i < 3; i++, y+=FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE) {
	    user_interface.create_button_bitmap(FINANCIAL_BUTTON_X1,y,
						FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT,
						financial_button_label[i], &(financial_button2_up_bitmap[i]),
						&(financial_button2_down_bitmap[i]), &vga_back);

	    financial_button_group2[i].create_bitmapW(FINANCIAL_BUTTON_X1,y,
						      FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,y+FINANCIAL_BUTTON_HEIGHT,
						      (financial_button2_up_bitmap[i])+4,0);
	}

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+7);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+7,1);
	font_charts.put(REPORT_X1+330,REPORT_Y1-8,"Tuition");

	short x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1;
	font_chartsm.put(REPORT_X1-10,y+2,"Department :");
	font_chartsm.put(REPORT_X1-10,y+30,"  Institution :");
	int i;
	for (i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
	//  Turnover
	type_button_group[i].create_text(x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y,
					 x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y+BOTTOM_BUTTON_HEIGHT,
					 reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (i = 4; i < 9; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,
						 x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
						 reportTypeStrArray[i]);

	y = BOTTOM_BUTTON_Y1;
	type_button_group[i].create_text(x, y,
					 x+BOTTOM_BUTTON_WIDTH+10, y+BOTTOM_BUTTON_HEIGHT+28,
					 reportTypeStrArray[i]);

	financial_spinner_budget1.init(
	    REPORT_X1+MODE_FINANCIAL_TAB5+75,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+42,
	    REPORT_X1+MODE_FINANCIAL_TAB5+175,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+72,
	    //### begin zhoubin 02/25/99
	    &spinner_var1,
	    //### end zhoubin 02/25/99
	    //				-100.0f,100.0f,0.1f,0,NULL,23);
	    -5.0f,5.0f,0.1f,0,NULL,25);                 //## chea 170999

	financial_spinner_budget2.init(
	    REPORT_X1+MODE_FINANCIAL_TAB5+75,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+150,
	    REPORT_X1+MODE_FINANCIAL_TAB5+175,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+180,
	    //### begin zhoubin 02/25/99
	    &spinner_var2,
	    //### end zhoubin 02/25/99
	    //				-100.0f,100.0f,0.1f,0,NULL,23);
	    -5.0f,5.0f,0.1f,0,NULL,25);                 //## chea 170999

	mode_financial_stats_bitmap=vga_back.save_area(
	    REPORT_X1+MODE_FINANCIAL_TAB2, REPORT_Y1+15,
	    REPORT_X2, REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT,
	    mode_financial_stats_bitmap);

	text_refresh_bitmap1=vga_back.save_area(
	    REPORT_X1+TAB4,
	    REPORT_Y1+15+VTAB1,
	    REPORT_X2,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT,
	    text_refresh_bitmap1);

	//-------- graph title --------------//
	mode_financial_graph_profitloss1.init(
	    REPORT_X1, REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT,
	    REPORT_X1+MODE_FINANCIAL_TAB4+3,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+MODE_FINANCIAL_BOX2_HEIGHT/2,
	    1, &numSeriesData1, dataArray1, 1,
	    NULL, NULL, &(mode_financial_legend_label[0]), 1, 1, 1, 1);

	mode_financial_graph_profitloss2.init(
	    REPORT_X1,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+MODE_FINANCIAL_BOX2_HEIGHT/2,
	    REPORT_X1+MODE_FINANCIAL_TAB4+3,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+MODE_FINANCIAL_BOX2_HEIGHT,
	    1, &numSeriesData1, dataArray2, 1,
	    NULL, NULL, &(mode_financial_legend_label[1]), 1, 1, 1, 1);

	mode_financial_graph_profitloss1.paint();
	mode_financial_graph_profitloss2.paint();

	mode_financial_graphicon.init(
	    REPORT_X1+MODE_FINANCIAL_TAB5+110,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+3,
	    REPORT_X1+MODE_FINANCIAL_TAB5+180,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+36,
	    1, &numSeriesData2, dataArray3, 1, 0 , 5);

	mode_financial_graphicon.paint();

	mode_financial_text.init(
	    REPORT_X1+MODE_FINANCIAL_TAB5+60,
	    REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+13,&v3,3);
	mode_financial_text.paint();

	type_button_group.paint(current_report_type);
	// draw the buttons into different color
	int x1, x2, y1, y2;

	for ( int z=3; z<10; z++ ) {
	    x1 = type_button_group[z].x1+1;
	    x2 = type_button_group[z].x2+1;
	    y1 = type_button_group[z].y1+1;
	    y2 = type_button_group[z].y2-1;

	    user_interface.rect(x1,y1,x2,y2,1,70);
	}
    }
    else {
	vga_back.rest_area(mode_financial_stats_bitmap,0,0);
	vga_back.rest_area(text_refresh_bitmap1,0,0);

	mode_financial_graph_profitloss1.refresh();
	mode_financial_graph_profitloss2.refresh();
	mode_financial_graphicon.refresh();

	mode_financial_text.refresh();
    }

    //-------- paint the buttons -----------//
    int pressed1=P_NONE;
    int read1 =finance.get_policy_var(true, AC_GROSS_TUITION_INCOME)->applied_flag;
    if(read1!=P_NONE)pressed1=read1;
    if(pressed1==P_NONE) {
	for(int i=0;i<3;i++)
	    financial_button_group1[i].paint();
    }
    else {
	financial_button_group1.paint(pressed1-1);
    }

    int pressed2=P_NONE;
    int read2 =finance.get_policy_var(false, AC_STUDENT_LIFE)->applied_flag;
    if(read2!=P_NONE)pressed2=read2;
    if(pressed2==P_NONE) {
	for(int i=0; i<3; i++)
	    financial_button_group2[i].paint();
    }
    else {
	financial_button_group2.paint(pressed2-1);
    }

    //-------- display static strings -----------//
    font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+15+25, mode_financial_str[0]);
    font_chartsm.put(REPORT_X1+MODE_FINANCIAL_TAB1, REPORT_Y1+15+46, mode_financial_str[1]);

    font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB3,
			     REPORT_Y1+15+5,
			     REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
			     REPORT_Y1+15+5+font_chartsm.max_font_height, mode_financial_str[4]);
    font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB5,
			     REPORT_Y1+15+5,
			     REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
			     REPORT_Y1+15+5+font_chartsm.max_font_height, mode_financial_str[4]);

    font_chartsm.put_paragraph(
	REPORT_X1+MODE_FINANCIAL_TAB4+10,
	REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+10,
	REPORT_X1+MODE_FINANCIAL_TAB5+160,
	REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+35,
	mode_financial_str[2]);
    font_chartsm.put_paragraph(
	REPORT_X1+MODE_FINANCIAL_TAB4+10,
	REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+50,
	REPORT_X1+MODE_FINANCIAL_TAB5+160,
	REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+80,
	mode_financial_str[5]);
    font_chartsm.put_paragraph(
	REPORT_X1+MODE_FINANCIAL_TAB4+10,
	REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+150,
	REPORT_X1+MODE_FINANCIAL_TAB5+160,
	REPORT_Y1+15+MODE_FINANCIAL_BOX1_HEIGHT+190,
	mode_financial_str[6]);

    financial_spinner_budget1.refresh();
    financial_spinner_budget2.refresh();

    String *s1,*s2;

    //show years

    s1=new String(m.format(v11,16));
    //	s2=new String(m.format(v11+1,16));   //## chea 061199
    //	(*s1)+="-";
    //	(*s1)+=(*s2);
    font_chartsm.center_put(
	REPORT_X1+MODE_FINANCIAL_TAB2+10,
	REPORT_Y1+15+5,
	REPORT_X1+MODE_FINANCIAL_TAB2+field_space+10,
	REPORT_Y1+15+5+font_chartsm.max_font_height,
	s1->str_buf);

    (*s1)=m.format(v12,16);
    //	(*s2)=m.format(v12+1,16);            //## chea 061199
    //	(*s1)+="-";
    //	(*s1)+=(*s2);
    font_chartsm.center_put(
	REPORT_X1+MODE_FINANCIAL_TAB4+40+10,
	REPORT_Y1+15+5,
	REPORT_X1+MODE_FINANCIAL_TAB4+field_space+10,
	REPORT_Y1+15+5+font_chartsm.max_font_height,
	s1->str_buf);

    (*s1)=m.format(v13,16);
    //	(*s2)=m.format(v13+1,16);            //## chea 061199
    //	(*s1)+="-";
    //	(*s1)+=(*s2);
    font_chartsm.center_put(
	REPORT_X1+MODE_FINANCIAL_TAB6+10,
	REPORT_Y1+15+5,
	REPORT_X1+MODE_FINANCIAL_TAB6+field_space+10,
	REPORT_Y1+15+5+font_chartsm.max_font_height,
	s1->str_buf);
    delete s1,s2;

    /*
      font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB2,
      REPORT_Y1+15+25,
      REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
      REPORT_Y1+15+25+font_chartsm.max_font_height,
      m.format(v21, 2));
      font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB3,
      REPORT_Y1+15+25,
      REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
      REPORT_Y1+15+25+font_chartsm.max_font_height,
      m.format(v22, 3));
      font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB4+40,
      REPORT_Y1+15+25,
      REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
      REPORT_Y1+15+25+font_chartsm.max_font_height,
      m.format(v23, 2));
      font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB5,
      REPORT_Y1+15+25,
      REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
      REPORT_Y1+15+25+font_chartsm.max_font_height,
      m.format(v24, 3));
      font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB6,
      REPORT_Y1+15+25,
      REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
      REPORT_Y1+15+25+font_chartsm.max_font_height,
      m.format(v25, 2));

      font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB2,
      REPORT_Y1+15+46,
      REPORT_X1+MODE_FINANCIAL_TAB2+field_space,
      REPORT_Y1+15+46+font_chartsm.max_font_height,
      m.format(v31, 2));
      font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB3,
      REPORT_Y1+15+46,
      REPORT_X1+MODE_FINANCIAL_TAB3+field_space,
      REPORT_Y1+15+46+font_chartsm.max_font_height,
      m.format(v32, 3));
      font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB4+40,
      REPORT_Y1+15+46,
      REPORT_X1+MODE_FINANCIAL_TAB4+field_space,
      REPORT_Y1+15+46+font_chartsm.max_font_height,
      m.format(v33, 2));
      font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB5,
      REPORT_Y1+15+46,
      REPORT_X1+MODE_FINANCIAL_TAB5+field_space,
      REPORT_Y1+15+46+font_chartsm.max_font_height,
      m.format(v34, 3));
      font_chartsm.center_put( REPORT_X1+MODE_FINANCIAL_TAB6,
      REPORT_Y1+15+46,
      REPORT_X1+MODE_FINANCIAL_TAB6+field_space,
      REPORT_Y1+15+46+font_chartsm.max_font_height,
      m.format(v35, 2));
    */
    font_chartsm.right_put(
	REPORT_X1+MODE_FINANCIAL_TAB2+field_space-40,
	REPORT_Y1+15+25,
	m.format(v21, 2));
    font_chartsm.right_put(
	REPORT_X1+MODE_FINANCIAL_TAB3+field_space-45,
	REPORT_Y1+15+25,
	m.format(v22, 24));
    font_chartsm.right_put(
	REPORT_X1+MODE_FINANCIAL_TAB4+field_space-20,
	REPORT_Y1+15+25,
	m.format(v23, 2));
    font_chartsm.right_put(
	REPORT_X1+MODE_FINANCIAL_TAB5+field_space-50,
	REPORT_Y1+15+25,
	m.format(v24, 24));
    font_chartsm.right_put(
	REPORT_X1+MODE_FINANCIAL_TAB6+field_space-40,
	REPORT_Y1+15+25,
	m.format(v25, 2));

    font_chartsm.right_put(
	REPORT_X1+MODE_FINANCIAL_TAB2+field_space-40,
	REPORT_Y1+15+46,
	m.format(v31, 2));
    font_chartsm.right_put(
	REPORT_X1+MODE_FINANCIAL_TAB3+field_space-45,
	REPORT_Y1+15+46,
	m.format(v32, 24));
    font_chartsm.right_put(
	REPORT_X1+MODE_FINANCIAL_TAB4+field_space-20,
	REPORT_Y1+15+46,
	m.format(v33, 2));
    font_chartsm.right_put(
	REPORT_X1+MODE_FINANCIAL_TAB5+field_space-50,
	REPORT_Y1+15+46,
	m.format(v34, 24));
    font_chartsm.right_put(
	REPORT_X1+MODE_FINANCIAL_TAB6+field_space-40,
	REPORT_Y1+15+46,
	m.format(v35, 2));

    // #### Begin Marco #### //
    if (info.financial_year() > 1 ) {
	font_chart_purple_sm.right_put(
	    REPORT_X1+MODE_FINANCIAL_TAB2,
	    REPORT_Y1+15+5,
	    "Yr. 0 (Initial)");
	font_chart_purple_sm.right_put(
	    REPORT_X1+MODE_FINANCIAL_TAB2,
	    REPORT_Y1+15+25,
	    m.format(initialv21, 2));
	font_chart_purple_sm.right_put(
	    REPORT_X1+MODE_FINANCIAL_TAB2,
	    REPORT_Y1+15+46,
	    m.format(initialv31, 2));
    }
    // #### End Marco #### //
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

    // Pop up all buttons
    for(int i=0;i<3;i++) {
	financial_button_group1[i].pop();
	financial_button_group1[i].body_ptr=
	    (void*)(financial_button1_up_bitmap[i]+4);
	financial_button_group2[i].pop();
	financial_button_group2[i].body_ptr=
	    (void*)(financial_button2_up_bitmap[i]+4);
    }
}

//----- End of function report_FINANCIAL ---------//
//----- End of Summary Report 5 ------//
////////////////////////////////////////////////////////////////////////

void PlayerSchool::summary_student_report_call_residence() {
    current_report_type=REPORT_TYPE_RESIDENCE_HALL;
    current_report_mode=REPORT_MODE_INSTITUTION;
}

////////////////////////////////////////////////////////////////////////
//----- Begin of Summary Report 6 ------//
void PlayerSchool::summary_student_report_6(int refreshFlag) {

    int tab1=10;
    int tab2=230;
    int tab3=300;
    int tab4=350;
    int tab5=450;
    int tab6=530;
    int tab7=610;
    int vtab1=15;
    int vtab2=265;
    int vtab3=270;
    int vtab4=330;
    static int v11=1000,v12=1999,v13=8888;
    static double v21=663.23,v22=123.23,v23=17.23,v24=723.23,v25=13.23;
    static int v3=0;
    static float v4=0;
    static int sliderVar1=0;
    static float spinnerVar1=0.0;
    static float spinnerVar2=0.0;
    static int numSeriesData = HISTORY_YEAR_COUNT;

    static int dataArray1[2*HISTORY_YEAR_COUNT],
	dataArray2[2*HISTORY_YEAR_COUNT];
    static int xStart, xEnd;

    //	Communicate with Sim Engine's Output
    for(int j=0;j<HISTORY_YEAR_COUNT;j++) {
	dataArray1[j]=student_office.available_bed_count[j];
	dataArray1[j+HISTORY_YEAR_COUNT]=student_office.occupied_bed_count[j];
	dataArray2[j]=student_office.tuition_rate_history[j];
	dataArray2[j+HISTORY_YEAR_COUNT]=student_office.room_n_board_rate_history[j];
    }

    v12=info.financial_year();
    v11=v12-1;
    v13=v12+1;
    v21=finance.expense_array[2].total;
    v22=finance.projected_expense_array[2].change_last_year.total;
    v23=finance.projected_expense_array[2].this_year.total;
    v24=finance.projected_expense_array[2].change_next_year.total;
    v25=finance.projected_expense_array[2].next_year.total;
    //### fred 1112###//
    v3= (int) student_office.percent_demanding_places_sl1;
    //	v4=(float)(100.0*student_office.occupied_bed_count[THIS_YEAR]/enroll_res.student_count[UG_TRADITION]);
    v4=(float)(100.0*student_office.occupied_bed_count[THIS_YEAR]/student_office.available_bed_count[THIS_YEAR]);
    ///////////////

    int field_space=8*font_chartsm.max_font_width;

    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_student_report_deinit();
	report_init_flag =1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);
	//--------- draw the brightened boxes ------------//

	user_interface.brighten(REPORT_X1, REPORT_Y1+vtab1, REPORT_X2, REPORT_Y1+vtab4);

	/*		user_interface.rect(
			REPORT_X1, REPORT_Y1+vtab1,
			REPORT_X1+tab4+1, REPORT_Y1+vtab2+4);
			user_interface.rect(
			REPORT_X1+tab4+2, REPORT_Y1+vtab1,
			REPORT_X2-1, REPORT_Y1+vtab2+4);
	*/
	user_interface.rect(REPORT_X1, REPORT_Y1+vtab3-1, REPORT_X2-1, REPORT_Y1+vtab4,1);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+7);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+7,1);
	font_charts.put(REPORT_X1+300,REPORT_Y1-8,"Student Housing");

	//-------- display static strings -----------//
	font_chartsm.put(REPORT_X1+tab1+25, REPORT_Y1+vtab3+10, mode_hall_str[0]);
	font_chartsm.put(REPORT_X1+tab1+25, REPORT_Y1+vtab3+36, mode_hall_str[1]);

	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;
	font_chartsm.put(REPORT_X1-10,y+2,"Department :");
	font_chartsm.put(REPORT_X1-10,y+30,"  Institution :");
	int i;
	for (i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);                       //  Turnover
	type_button_group[i].create_text(x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y,
					 x+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE, y+BOTTOM_BUTTON_HEIGHT,
					 reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (i = 4; i < 9; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,
						 x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
						 reportTypeStrArray[i]);

	y = BOTTOM_BUTTON_Y1;
	type_button_group[i].create_text(x, y,
					 x+BOTTOM_BUTTON_WIDTH+10, y+BOTTOM_BUTTON_HEIGHT+28,
					 reportTypeStrArray[i]);

	//-------- graph title --------------//
	mode_hall_graph1.init(
	    REPORT_X1, REPORT_Y1+vtab1,
	    REPORT_X1+tab4+1, REPORT_Y1+vtab2+4,
	    2, &numSeriesData, dataArray1, 0,
	    NULL, NULL, mode_hall_legend1, 1, 1, 1, 1);

	mode_hall_graph2.init(
	    REPORT_X1+tab4-1, REPORT_Y1+vtab1,
	    REPORT_X2-1, REPORT_Y1+vtab2+4,
	    2, &numSeriesData, dataArray2, 0,
	    NULL, NULL, mode_hall_legend2, 1, 1, 1, 2); //## chea200899 2.6.1

	mode_hall_text.init(
	    REPORT_X1+tab1+500, REPORT_Y1+vtab3+10,
	    &v4,3);
	mode_hall_text.paint();

	mode_hall_graph1.paint();
	mode_hall_graph2.paint();

	type_button_group.paint(current_report_type);
	// draw the buttons into different color
	int x1, x2, y1, y2;

	for ( int z=3; z<10; z++ ) {
	    x1 = type_button_group[z].x1+1;
	    x2 = type_button_group[z].x2+1;
	    y1 = type_button_group[z].y1+1;
	    y2 = type_button_group[z].y2-1;

	    user_interface.rect(x1,y1,x2,y2,1,70);
	}

	mode_hall_value_slider.init(
	    REPORT_X1+tab1+500, REPORT_Y1+vtab3+30,
	    180,20,
	    0, 100, &v3, 1, V_YELLOW, 3);
	mode_hall_value_slider.refresh();
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    }
    else {
	mode_hall_graph1.refresh();
	mode_hall_graph2.refresh();
	mode_hall_text.refresh();
	vga.blt_buf(
	    REPORT_X1, REPORT_Y1+vtab1,
	    REPORT_X1+tab4-3, REPORT_Y1+vtab2);
	vga.blt_buf(
	    REPORT_X1+tab4+2, REPORT_Y1+vtab1,
	    REPORT_X2-1, REPORT_Y1+vtab2);
	vga.blt_buf(
	    REPORT_X1+tab1+500, REPORT_Y1+vtab3+10,
	    REPORT_X1+tab1+550, REPORT_Y1+vtab3+28);
    }

    //	mode_hall_value_slider.refresh();

}

//----- End of function report_FINANCIAL ---------//
//----- End of Summary Report 6 ------//
////////////////////////////////////////////////////////////////////////

// --- Begin of function PlayerSchol::summer_student_report_7 --- //
//
void PlayerSchool::summary_student_report_7(int refreshFlag) {
    vga.use_back();

    if( refreshFlag == INFO_REPAINT ) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_student_report_deinit();
	report_init_flag =1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);

	int VTAB6 = 30;
	int TAB7  = 80;
	int TAB8   = 70;
	int TAB9  = 250;

	// top title
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	font_charts.center_put(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,"Student Admissions, Graduations, and Dropouts");

	if ( is_year_end_report ) {
	    // Paint "Back" Button
	    mode_turnover_button.create_text(REPORT_X1+270, BUTTON_Y1,
					     REPORT_X1+390, BUTTON_Y1+20,"Back",1); {

		// Paint "Next" Button
		mode_turnover_next_button.create_text(REPORT_X1+410,BUTTON_Y1,
						      REPORT_X1+530, BUTTON_Y1+20,"Next",1);
		mode_turnover_next_button.paint();
	    }
	}
	else {
	    // Paint "Back" Button
	    mode_turnover_button.create_text(REPORT_X1+320, BUTTON_Y1,
					     REPORT_X1+440, BUTTON_Y1+20,"Back",1);
	}

	mode_turnover_button.paint();

	// Display Table

	// Show Brighten Part
	user_interface.brighten(
	    REPORT_X1+TAB8,
	    REPORT_Y1+VTAB1+80,
	    REPORT_X1+TAB9+TAB7*5,
	    REPORT_Y1+VTAB1+80+5*VTAB6+50
	    );

	// The leftmost Column
	user_interface.rect(
	    REPORT_X1+TAB8,
	    REPORT_Y1+VTAB1+80,
	    REPORT_X1+TAB9,
	    REPORT_Y1+VTAB1+80+5*VTAB6+50,1
	    );

	// Columns
	int i;
	for ( i=0; i<5; i++)
	    user_interface.rect(
		REPORT_X1+TAB9-2+i*TAB7,
		REPORT_Y1+VTAB1+80,
		REPORT_X1+TAB9+TAB7*(i+1),
		REPORT_Y1+VTAB1+80+5*VTAB6+50,1);

	// Rows
	for( i=0;i<5; i++)
	    user_interface.rect(
		REPORT_X1+TAB8,
		REPORT_Y1+VTAB1+80+i*VTAB6+50,
		REPORT_X1+TAB9+TAB7*5,
		REPORT_Y1+VTAB1+80+(i+1)*VTAB6+50,1
		);

	// Print the titles on the table
	// Print row labels
	for ( i=0; i<6; i++ )
	    font_chartsm.put( REPORT_X1+TAB8+8,
			      REPORT_Y1+VTAB1+80+i*VTAB6+50-22,
			      mode_turnover_row_label[i] );

	// Print upper column labels
	for ( i=0; i<5; i++ ) {
	    if ( is_year_end_report || i!=1 ) {
		if ( is_year_end_report && i==4 )
		    font_chartsm.center_put(REPORT_X1+TAB9-2+i*TAB7,
					    REPORT_Y1+VTAB1+80+50-40,
					    REPORT_X1+TAB9+TAB7*(i+1),
					    REPORT_Y1+VTAB1+80+50-23,
					    "Target");
		else
		    font_chartsm.center_put(REPORT_X1+TAB9-2+i*TAB7,
					    REPORT_Y1+VTAB1+80+50-40,
					    REPORT_X1+TAB9+TAB7*(i+1),
					    REPORT_Y1+VTAB1+80+50-23,
					    mode_turnover_label1[i]);
	    }
	}

	// Print lower column labels
	for ( i=0; i<5; i++ ) {
	    if ( is_year_end_report || i!=1 ) {
		font_chartsm.center_put(REPORT_X1+TAB9-2+i*TAB7,
					REPORT_Y1+VTAB1+80+50-22,
					REPORT_X1+TAB9+TAB7*(i+1),
					REPORT_Y1+VTAB1+80+50,
					mode_turnover_label2[i]);
	    }
	    else {
		font_chartsm.center_put(REPORT_X1+TAB9-2+i*TAB7,
					REPORT_Y1+VTAB1+80+50-22,
					REPORT_X1+TAB9+TAB7*(i+1),
					REPORT_Y1+VTAB1+80+50,
					"Admissions" );
	    }
	}

	// Calculate data for year end report
	int bachelorDegree=0, nonUgDegree=0, masterDegree=0, doctorDegree=0;
	int dropoutLocal[DEGREE_TYPE_COUNT+1]={0,0,0,0};
	int nonUgDropouts=0;
	int totalPriorNo, totalAdmissionNo, totalDegreeNo, totalDropouts, totalNewNo;

	if ( is_year_end_report ) {
	    for ( int i=1; i<=department_array.size(); i++ ) {
		if ( department_array.is_deleted(i) )
		    continue;

		Department* deptPtr = department_array[i];

		// get total degree numbers
		bachelorDegree += deptPtr->student_array.last_year_degree[0];
		nonUgDegree   += deptPtr->student_array.last_year_degree[1];
		masterDegree  += deptPtr->student_array.last_year_degree[2];
		doctorDegree  += deptPtr->student_array.last_year_degree[3];

		// get total dropout numbers
		for ( int j=0; j<4; j++ )
		    dropoutLocal[j] += deptPtr->student_array.last_year_dropout[j];
	    }

	    GeneralDepartment* genDept = &department_res.general_dept;

	    for ( int j=0; j<4; j++ )
		dropoutLocal[j] += genDept->student_array.last_year_dropout[j];

	    // Calculate totals
	    totalPriorNo = enroll_res.new_year_student_count[UG_TRADITION]+
		enroll_res.new_year_student_count[UG_NONTRADITION]+
		enroll_res.new_year_student_count[MASTER]+
		enroll_res.new_year_student_count[DOCTOR];

	    totalDegreeNo = bachelorDegree + nonUgDegree + masterDegree + doctorDegree;

	    totalDropouts = dropoutLocal[0] + dropoutLocal[1] + dropoutLocal[2] + dropoutLocal[3];

	    totalNewNo = enroll_res.target_enrollment[0]+enroll_res.target_enrollment[1]+enroll_res.target_enrollment[2]
		+ enroll_res.target_enrollment[3];
	}
	else {
	    // Calculate dropouts of Nontraditional Undergraduates
	    nonUgDropouts = enroll_res.last_year_student_count[UG_NONTRADITION]+
		enroll_res.total_matrics[UG_NONTRADITION]-
		player_school.non_ug_bachelor_degree[7]-
		enroll_res.new_year_student_count[UG_NONTRADITION];

	    // not sure need modify. if need, not modify here
	    // BUGHERE, setting of initial value of last_year_student_count for MASTER and DOCTOR
	    //			enroll_res.last_year_student_count[MASTER] =
	    //						enroll_res.new_year_student_count[MASTER]-
	    //						enroll_res.total_matrics[MASTER]+
	    //						player_school.master_degree[7]+
	    //						player_school.dropout_rate[MASTER_DEGREE][7];
	    //
	    //			enroll_res.last_year_student_count[DOCTOR] =
	    //						enroll_res.new_year_student_count[DOCTOR]-
	    //						enroll_res.total_matrics[DOCTOR]+
	    //						player_school.doctor_degree[7]+
	    //						player_school.dropout_rate[DOCTOR_DEGREE][7];

	    // Calculate total in each column
	    totalPriorNo = enroll_res.last_year_student_count[UG_TRADITION]+
		enroll_res.last_year_student_count[UG_NONTRADITION]+
		enroll_res.last_year_student_count[MASTER]+
		enroll_res.last_year_student_count[DOCTOR];

	    totalAdmissionNo = enroll_res.total_matrics[UG_TRADITION]+
		enroll_res.total_matrics[UG_NONTRADITION]+
		enroll_res.total_matrics[MASTER]+
		enroll_res.total_matrics[DOCTOR];

	    totalDegreeNo = player_school.bachelor_degree[7]+
		player_school.non_ug_bachelor_degree[7]+
		player_school.master_degree[7]+
		player_school.doctor_degree[7];

	    totalDropouts = player_school.dropout_rate[BACHELOR_DEGREE][7]+
		player_school.dropout_rate[MASTER_DEGREE][7]+
		player_school.dropout_rate[DOCTOR_DEGREE][7];

	    totalNewNo = enroll_res.new_year_student_count[UG_TRADITION]+
		enroll_res.new_year_student_count[UG_NONTRADITION]+
		enroll_res.new_year_student_count[MASTER]+
		enroll_res.new_year_student_count[DOCTOR];
	}

	// Print out data on the table
	for ( i=0; i<5; i++ ) {
	    // Prior year's enrollment
	    int priorNo;
	    if ( is_year_end_report ) {
		switch (i) {
		case 0: priorNo = enroll_res.new_year_student_count[UG_TRADITION]; break;
		case 1: priorNo = enroll_res.new_year_student_count[UG_NONTRADITION]; break;
		case 2: priorNo = enroll_res.new_year_student_count[MASTER]; break;
		case 3: priorNo = enroll_res.new_year_student_count[DOCTOR]; break;
		case 4: priorNo = totalPriorNo;
		}
	    }
	    else {
		switch (i) {
		case 0: priorNo = enroll_res.last_year_student_count[UG_TRADITION]; break;
		case 1:   priorNo = enroll_res.last_year_student_count[UG_NONTRADITION]; break;
		case 2: priorNo = enroll_res.last_year_student_count[MASTER]; break;
		case 3: priorNo = enroll_res.last_year_student_count[DOCTOR]; break;
		case 4: priorNo = totalPriorNo;
		}
	    }
	    font_chartsm.center_put(REPORT_X1+TAB9-2,
				    REPORT_Y1+VTAB1+80+i*VTAB6+50,
				    REPORT_X1+TAB9+TAB7,
				    REPORT_Y1+VTAB1+80+(i+1)*VTAB6+50,
				    m.format(priorNo));

	    // Admissions
	    int admissionNo;
	    if ( !is_year_end_report ) {
		switch (i) {
		case 0: admissionNo = enroll_res.total_matrics[UG_TRADITION]; break;
		case 1: admissionNo = enroll_res.total_matrics[UG_NONTRADITION]; break;
		case 2: admissionNo = enroll_res.total_matrics[MASTER]; break;
		case 3: admissionNo = enroll_res.total_matrics[DOCTOR]; break;
		case 4: admissionNo = totalAdmissionNo;
		}

		font_chartsm.center_put(REPORT_X1+TAB9-2+TAB7,
					REPORT_Y1+VTAB1+80+i*VTAB6+50,
					REPORT_X1+TAB9+TAB7*2,
					REPORT_Y1+VTAB1+80+(i+1)*VTAB6+50,
					m.format(admissionNo));
	    }
	    else {
		if ( i == 4 ) {                           // total
		    int totalTarget=0;

		    for ( int z=0; z<4; z++ )
			totalTarget += enroll_res.target_student_intake[z];

		    font_chartsm.center_put(REPORT_X1+TAB9-2+TAB7,
					    REPORT_Y1+VTAB1+80+i*VTAB6+50,
					    REPORT_X1+TAB9+TAB7*2,
					    REPORT_Y1+VTAB1+80+(i+1)*VTAB6+50,
					    m.format(totalTarget) );
		}
		else {
		    font_chartsm.center_put(REPORT_X1+TAB9-2+TAB7,
					    REPORT_Y1+VTAB1+80+i*VTAB6+50,
					    REPORT_X1+TAB9+TAB7*2,
					    REPORT_Y1+VTAB1+80+(i+1)*VTAB6+50,
					    m.format(enroll_res.target_student_intake[i]));
		}
	    }

	    // Graduations
	    int degreeNo;
	    if ( is_year_end_report ) {
		switch (i) {
		case 0:   degreeNo = bachelorDegree; break;
		case 1:   degreeNo = nonUgDegree;   break;
		case 2:   degreeNo = masterDegree;  break;
		case 3:   degreeNo = doctorDegree;  break;
		case 4:   degreeNo = totalDegreeNo;
		}
	    }
	    else {
		switch (i) {
		case 0:   degreeNo = player_school.bachelor_degree[7]; break;
		case 1:   degreeNo = player_school.non_ug_bachelor_degree[7]; break;
		case 2:   degreeNo = player_school.master_degree[7]; break;
		case 3:   degreeNo = player_school.doctor_degree[7]; break;
		case 4:   degreeNo = totalDegreeNo;
		}
	    }
	    font_chartsm.center_put(REPORT_X1+TAB9-2+TAB7*2,
				    REPORT_Y1+VTAB1+80+i*VTAB6+50,
				    REPORT_X1+TAB9+TAB7*3,
				    REPORT_Y1+VTAB1+80+(i+1)*VTAB6+50,
				    m.format(degreeNo));

	    // Dropouts
	    int dropoutNo;
	    if ( is_year_end_report ) {
		switch (i) {
		case 0:   dropoutNo = dropoutLocal[0]; break;
		case 1:   dropoutNo = dropoutLocal[1];    break;
		case 2:   dropoutNo = dropoutLocal[2];  break;
		case 3:   dropoutNo = dropoutLocal[3];  break;
		case 4:   dropoutNo = totalDropouts;
		}
	    }
	    else {
		switch (i) {
		case 0:   dropoutNo = player_school.dropout_rate[BACHELOR_DEGREE][7]-nonUgDropouts; break;
		case 1:   dropoutNo = nonUgDropouts; break;
		case 2:   dropoutNo = player_school.dropout_rate[MASTER_DEGREE][7]; break;
		case 3:   dropoutNo = player_school.dropout_rate[DOCTOR_DEGREE][7]; break;
		case 4:   dropoutNo = totalDropouts;
		}
	    }
	    font_chartsm.center_put(REPORT_X1+TAB9-2+TAB7*3,
				    REPORT_Y1+VTAB1+80+i*VTAB6+50,
				    REPORT_X1+TAB9+TAB7*4,
				    REPORT_Y1+VTAB1+80+(i+1)*VTAB6+50,
				    m.format(dropoutNo));

	    // New enrollment
	    int newNo;
	    switch (i) {
	    case 0: newNo = enroll_res.new_year_student_count[UG_TRADITION]; break;
	    case 1:   newNo = enroll_res.new_year_student_count[UG_NONTRADITION]; break;
	    case 2: newNo = enroll_res.new_year_student_count[MASTER]; break;
	    case 3: newNo = enroll_res.new_year_student_count[DOCTOR]; break;
	    case 4:  newNo = totalNewNo;
	    }

	    if ( is_year_end_report ) {
		switch (i) {
		case 0: newNo -= bachelorDegree+dropoutLocal[0]; break;
		case 1: newNo -= nonUgDegree+dropoutLocal[1]; break;
		case 2: newNo -= masterDegree+dropoutLocal[2];  break;
		case 3: newNo -= doctorDegree+dropoutLocal[3];  break;
		}
		newNo += enroll_res.target_student_intake[i];
	    }

	    font_chartsm.center_put(REPORT_X1+TAB9-2+TAB7*4,
				    REPORT_Y1+VTAB1+80+i*VTAB6+50,
				    REPORT_X1+TAB9+TAB7*5,
				    REPORT_Y1+VTAB1+80+(i+1)*VTAB6+50,
				    m.format(newNo));
	}
    }

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    vga.use_front();
}

//
// --- End of function PlayerSchool::summer_student_report_7 --- //

//----- Begin of function FacultySummary::student_report ------//
//!
//! <int> refreshFlag = INFO_REPAINT or INFO_UPDATE
//!
void PlayerSchool::summary_student_report(int refreshFlag) {
    if(current_report_type==REPORT_TYPE_DEPARTMENT_PROFILE1) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_student_report_1(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_DEPARTMENT_PROFILE2) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_student_report_2(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_DEPARTMENT_ACTIVITIES) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_student_report_3(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_INSTITUTION_PROFILE1) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_student_report_1(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_INSTITUTION_PROFILE2) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_student_report_2(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_INSTITUTION_ACTIVITIES) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_student_report_3(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_INSTITUTION_UNDERGRADUATE) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_student_report_4(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_FINANCE) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_student_report_5(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_RESIDENCE_HALL) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_student_report_6(refreshFlag);
    }
    else if (current_report_type==REPORT_TYPE_TURNOVER) {
	sys.is_institution_report = 1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_student_report_7(refreshFlag);
    }
}

//------- End of function FacultySummary::report ------//

//----- Begin of function FacultySummary::student_report_detect ------//
//!
int PlayerSchool::summary_student_report_detect() {
    vga.use_front();

    if ( current_report_type == REPORT_TYPE_TURNOVER ) {
	if ( is_year_end_report ) {
	    if ( mode_turnover_button.detect() )
		return 1;
	    else
		if ( mode_turnover_next_button.detect() )
		    return 2;
		else
		    return 0;
	}

	if (mode_turnover_button.detect()) {
	    current_report_type = REPORT_TYPE_INSTITUTION_PROFILE1;
	    sys.redraw_all_flag=1;
	}
	return 1;
    }

    //------- detect type buttons --------//
    int summaryButtonGroupDetect;

    if((summaryButtonGroupDetect = type_button_group.detect()) >= 0) {
	//		type_button_group[current_report_type].body_ptr = (void *) (type_button_up_bitmap[current_report_type] + 4);
	current_report_type = summaryButtonGroupDetect;
	//		type_button_group[current_report_type].body_ptr = (void *) (type_button_down_bitmap[current_report_type] + 4);
	sys.redraw_all_flag = 1;
	return 1;
    }

    if(current_report_type==REPORT_TYPE_FINANCE) {
	help.set_help(
	    REPORT_X1+MODE_FINANCIAL_TAB4,
	    REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+65,
	    REPORT_X1+MODE_FINANCIAL_TAB5+30,
	    REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+95,
	    "H2.1.1.");
	help.set_help(
	    REPORT_X1+MODE_FINANCIAL_TAB4,
	    REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+175,
	    REPORT_X1+MODE_FINANCIAL_TAB5+30,
	    REPORT_Y1+MODE_FINANCIAL_BOX1_HEIGHT+205,
	    "H2.1.2.");
	help.set_help(
	    FINANCIAL_BUTTON_X1,FINANCIAL_BUTTON_Y1-40,
	    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,FINANCIAL_BUTTON_Y1-40+FINANCIAL_BUTTON_HEIGHT,
	    "H2.1.3.");
	help.set_help(
	    FINANCIAL_BUTTON_X1,FINANCIAL_BUTTON_Y1-40+FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE,
	    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,FINANCIAL_BUTTON_Y1-40+2*FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE,
	    "H2.1.4.");
	help.set_help(
	    FINANCIAL_BUTTON_X1,FINANCIAL_BUTTON_Y1-40+2*FINANCIAL_BUTTON_HEIGHT+2*FINANCIAL_BUTTON_DISTANCE,
	    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,FINANCIAL_BUTTON_Y1-40+3*FINANCIAL_BUTTON_HEIGHT+2*FINANCIAL_BUTTON_DISTANCE,
	    "H2.1.5.");
	help.set_help(
	    FINANCIAL_BUTTON_X1,FINANCIAL_BUTTON2_Y1-60,
	    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,FINANCIAL_BUTTON2_Y1-60+FINANCIAL_BUTTON_HEIGHT,
	    "H2.1.3.");
	help.set_help(
	    FINANCIAL_BUTTON_X1,FINANCIAL_BUTTON2_Y1-60+FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE,
	    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,FINANCIAL_BUTTON2_Y1-60+2*FINANCIAL_BUTTON_HEIGHT+FINANCIAL_BUTTON_DISTANCE,
	    "H2.1.4.");
	help.set_help(
	    FINANCIAL_BUTTON_X1,FINANCIAL_BUTTON2_Y1-60+2*FINANCIAL_BUTTON_HEIGHT+2*FINANCIAL_BUTTON_DISTANCE,
	    FINANCIAL_BUTTON_X1+FINANCIAL_BUTTON_WIDTH,FINANCIAL_BUTTON2_Y1-60+3*FINANCIAL_BUTTON_HEIGHT+2*FINANCIAL_BUTTON_DISTANCE,
	    "H2.1.5.");

	if (financial_spinner_budget1.detect()) {
	    financial_spinner_budget1.paint();
	    return 1;
	}
	if (financial_spinner_budget2.detect()) {
	    financial_spinner_budget2.paint();
	    return 1;
	}
	for(int i=0;i<3;i++) {
	    if(financial_button_group1[i].detect()) {
		financial_button_group1[i].body_ptr =
		    (void *) (financial_button1_down_bitmap[i] + 4);
		//	Communicate with Sim Engine
		finance.change_budget(i,true, AC_GROSS_TUITION_INCOME,(*financial_spinner_budget1.var_ptr));
		//##	begin	zhoubin	000329
		*financial_spinner_budget1.var_ptr=0;
		//##	end	zhoubin	000329
	    }
	    if(financial_button_group2[i].detect()) {
		financial_button_group2[i].body_ptr =
		    (void *) (financial_button2_down_bitmap[i] + 4);
		//	Communicate with Sim Engine
		finance.change_budget(i,false, AC_STUDENT_LIFE,(*financial_spinner_budget2.var_ptr));
		//##	begin	zhoubin	000329
		*financial_spinner_budget2.var_ptr=0;
		//##	end	zhoubin	000329
	    }
	}
    }

    if(current_report_type==REPORT_TYPE_RESIDENCE_HALL) {
	help.set_help(
	    REPORT_X1+35, REPORT_Y1+300,
	    REPORT_X1+690, REPORT_Y1+320,
	    "H2.2.1.");
	vga.use_back();
	if (mode_hall_value_slider.detect()) {
	    //student_office.targe_sl1_in_hall_percent=*(mode_hall_value_slider.var_ptr);		//### fred ###//
	    //### fred 1009###//
	    student_office.percent_demanding_places_sl1= (float) *(mode_hall_value_slider.var_ptr);
	    return 1;
	}
    }

    return 0;
}

//------- End of function FacultySummary::student_report_detect ------//

void PlayerSchool::summary_student_report_deinit() {
    if(report_init_flag==0)return;

    if (text_refresh_bitmap1) {
	mem_del(text_refresh_bitmap1);
	text_refresh_bitmap1=NULL;
    }
    //------- deinit type buttons --------//
    /*	if (*type_button_up_bitmap)
	{
	for (int i = 0; i < 9; i++)
	{
        mem_del(type_button_up_bitmap[i]);
        mem_del(type_button_down_bitmap[i]);
        type_button_up_bitmap[i] = NULL;
        type_button_down_bitmap[i] = NULL;
	}
	}
    */
    std_sum_rank_piechart.deinit();
    std_sum_gender_ethnic_piechart.deinit();

    for(int i=0;i<14;i++) {
	mode_std_sum_graphicon_t[i].deinit();

	if ( i != 3 )

	    if( !(i==4 || i==5 || i==7 || i==8) )       // these graphs have been removed.
		mode_std_sum_graphicon_y[i].deinit();

	mode_std_sum_graphicon_m[i].deinit();
	std_sum_text[i].deinit();
	std_sum_text2[i].deinit();
    }

    // ##### begin Gilbert 27/04/2001 #####//
    financial_button_group1.deinit_buttons();
    financial_button_group2.deinit_buttons();
    // ##### end Gilbert 27/04/2001 #####//

    if (*financial_button1_up_bitmap)
	for (int k = 0; k < 3; k++) {
	    mem_del(financial_button1_up_bitmap[k]);
	    financial_button1_up_bitmap[k] = NULL;
	}

    if (*financial_button1_down_bitmap)
	for (int k = 0; k < 3; k++) {
	    mem_del(financial_button1_down_bitmap[k]);
	    financial_button1_down_bitmap[k] = NULL;
	}

    if (*financial_button2_up_bitmap)
	for (int k = 0; k < 3; k++) {
	    mem_del(financial_button2_up_bitmap[k]);
	    financial_button2_up_bitmap[k] = NULL;
	}

    if (*financial_button2_down_bitmap)
	for (int k = 0; k < 3; k++) {
	    mem_del(financial_button2_down_bitmap[k]);
	    financial_button2_down_bitmap[k] = NULL;
	}

    if (mode_financial_stats_bitmap) {
	mem_del(mode_financial_stats_bitmap);
	mode_financial_stats_bitmap = NULL;
    }

    mode_financial_text.deinit();
    mode_financial_graph_profitloss1.deinit();
    mode_financial_graph_profitloss2.deinit();
    mode_financial_graphicon.deinit();

    mode_hall_graph1.deinit();
    mode_hall_graph2.deinit();
    mode_hall_text.deinit();

    //### begin zb 02/23/99
    financial_spinner_budget1.deinit();
    financial_spinner_budget2.deinit();
    //### end zb 02/23/99

    // ##### begin Gilbert 27/04/2001 #####//
    mode_hall_value_slider.deinit();
    type_button_group.deinit_buttons();
    // ##### end Gilbert 27/04/2001 #####//

    report_init_flag=0;
}

//------- End of function FacultySummary::student_report_deinit ------//
