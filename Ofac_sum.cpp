//Filename    : OFAC_SUM.CPP
//Description : Faculty Report

#include <KEY.H>
#include <OVGA.H>
#include <OFONT.H>
#include <OWORLD.H>
#include <OSYS.H>
#include <OBOX.H>
#include <OHELP.H>
#include <OPSCHOOL.H>
#include <OINFO.H>
#include <OIFACE.H>
#include <OBUTTON.H>
#include <OTEXT.H>
#include <ODEPT.H>
#include <OFINANCE.H>
#include <OGAMESTR.H>
#include <OFACULTY.H>
#include <OFACURES.H>
#include <OGRPHYR.H>
#include <OGRPHMTH.H>
#include <OPIECHRT.H>
#include <OGRPH_PY.H>
#include <OGRPH_PM.H>
#include <OGRPH_PT.H>
#include <OGRPDYR.H>
#include <OGRPDMTH.H>
#include <OVALSLDR.H>
#include <ORADIO.H>
#include <OMOUSE.H>

//---------- Define constant ------------//

enum {
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+15,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-40,
};

enum { BUTTON_Y1=REPORT_Y2+10 };
enum { BUTTON_WIDTH=80, BUTTON_HEIGHT=20 };

enum { REPORT_TYPE_LIST, REPORT_TYPE_DETAIL, REPORT_RETIRE };
enum { FACULTY_REC_HEIGHT = 84, RESEARCH_REC_HEIGHT = 16 };

//--------- Define report modes ----------//

enum {                                            // the bg image id
    BG_PIC_ID = 13
};

enum { FAC_SUM_REPORT_TYPE_COUNT = 14 };

enum {
    REPORT_TYPE_DEPARTMENT_PROFILE1,
    REPORT_TYPE_DEPARTMENT_PROFILE2,
    REPORT_TYPE_DEPARTMENT_ACTIVITIES,
    REPORT_TYPE_DEPARTMENT_RESEARCH,
    REPORT_TYPE_DEPARTMENT_HIRING,
    REPORT_TYPE_DEPARTMENT_DIRECTORY,
    REPORT_TYPE_DEPARTMENT_TURNOVER,
    REPORT_TYPE_INSTITUTION_PROFILE1,
    REPORT_TYPE_INSTITUTION_PROFILE2,
    REPORT_TYPE_INSTITUTION_ACTIVITIES,
    REPORT_TYPE_INSTITUTION_RESEARCH,
    REPORT_TYPE_INSTITUTION_SALARY,
    REPORT_TYPE_INSTITUTION_PROMOTION,
    REPORT_TYPE_INSTITUTION_TURNOVER,
    REPORT_TYPE_DEPARTMENT_TURNOVER_DETAIL,
};

enum {
    FACULTY_LISTS,
    FACULTY_DETAILS,
};

enum { REPORT_MODE_COUNT = 2 };
enum { REPORT_MODE_INSTITUTION, REPORT_MODE_DEPARTMENT};

enum {
    BOTTOM_BUTTON_X1 = REPORT_X1+90,
    BOTTOM_BUTTON_Y1 = REPORT_Y2-20,
    BOTTOM_BUTTON_TAB1 = 400,
    BOTTOM_BUTTON_X2 = REPORT_X2-20,
    BOTTOM_BUTTON_Y2 = REPORT_Y2+10
};

enum {
    BOTTOM_BUTTON_WIDTH = 74,
    BOTTOM_BUTTON_WIDTH2 = 78,
    BOTTOM_BUTTON_HEIGHT = 22,
    BOTTOM_BUTTON_DISTANCE
    = (BOTTOM_BUTTON_X2-BOTTOM_BUTTON_X1-BOTTOM_BUTTON_WIDTH2*7)/8
};


static ButtonGroup type_button_group(FAC_SUM_REPORT_TYPE_COUNT);
//static short *type_button_up_bitmap[FAC_SUM_REPORT_TYPE_COUNT];
//static short *type_button_down_bitmap[FAC_SUM_REPORT_TYPE_COUNT];
enum {
    TAB1=20,
    TAB2=200,
    TAB3=210,
    TAB4=480,
    TAB5=500,
    TAB6=610,
    TAB7=80,
    VTAB1=22,
    VTAB2=160,
    VTAB3=180,
    VTAB4=310,
    VTAB5=420,
    VTAB6=22,
};


enum { MONTHLY=0,ANNUALLY=1 };

static short *text_refresh_bitmap1;

static Department* cur_dept_ptr;
// the department whose faculty we are looking at

static char* fac_avg_salary_label[] ={ "Avg. Salary", "# of", "% of", "initial" };

static char* reportModeStrArray[] = { "Summary 1", "Summary 2" };
static char* fac_sum_rank_label[] = {
    "Assistant professor",
    "Associate professor",
    "Full professor",
    "Long-term adjunct",
    "Short-term adjunct",
    "Total",
};

static char* fac_sum_ge_label[] = {
    "Male nonminority",
    "Female nonminority",
    "Male minority",
    "Female minority",
    "Total",
    "Total",
};
static char* fac_sum_title[] = {
    "Faculty Distribution by Rank",
    "Faculty Distribution by Gender and Ethnicity",
    "Faculty Changes & Adjustments",
};

static char* std_sum_title2[] = {
    "Talent\t\t\tPerformance",
    "The number of degrees awarded",
};

static char* std_sum_label2[] = {
    "Teaching","Scholarship","Research","Normal Teaching load","Course preparation",
    "Out-of-class student contact","Educational development",
    "Research","Scholarship","Institutional and public service","Average faculty work hours per week",
    "Morale","Percent and regular faculty who currently have active research projects"
};

static char* hire_report_label1[] = {
    "","Prior year","","","Promotions",
    "Current year","Click for",
};
static char* hire_report_label2[] = { "Department","total","Departures","New hires","to tenure","total","detail", };
static char* hire_report_label3[] = {  "", "Prior year", "", "", "Promotions", "", };
static char* hire_report_label4[] = {  "Year", "total", "Departures", "New hires", "to tenure", "Year total", };

static char current_report_mode=REPORT_MODE_DEPARTMENT;
static char current_graph_mode=ANNUALLY;

static GraphMonth mode_fac_sum_graphmonth1;
static GraphMonth mode_fac_sum_graphmonth2;
static GraphDoubleBarYear mode_fac_sum_doublebar_y;
static GraphDoubleBarMonth mode_fac_sum_doublebar_m;

static PieChart fac_sum_rank_piechart;
static PieChart fac_sum_gender_ethnic_piechart;
static char     report_init_flag=0;

static Button mode_fac_hire_button;
static Button mode_fac_next_button;
static Button mode_fac_detail_button[MAX_DEPARTMENT];
static char* reportTypeStrArray[]
= {
    "Ranks", "Profiles","Activities","Research","Hiring","Directory","Changes ",
    "Ranks", "Profiles","Activities","Research","Salary","Promotion","Changes",
};

// screen Hiring setting
static char *mode_fac_sum_rblabel[]= {
    "Low",
    "Medium",
    "High",
};
static char *mode_fac_sum_rblabel2[]= {
    "Don't hire",
    "Medium",
    "High",
};

static char *mode_fac_sum_rblabel3[]= {
    /*	"Don't do it",
	"Doesn't matter",
	"Important",
	"Very important",
  */
  /*
    "Nil",
    "Medium",
    "High",
    "Top",
  */
    "","","","",
};

static char *mode_fac_sum_rblabel4[]= {
    /*	"Not at all important",
	"Moderately important",
	"Important",
	"Very important",
	"Nil",                                          */
    "Medium",
    "High",
    "Top",
};

// radio button object
static RadioButton mode_fac_sum_rb1;
static RadioButton mode_fac_sum_rb2;
static RadioButton mode_fac_sum_rb3;
static RadioButton mode_fac_sum_rb4;
static RadioButton mode_fac_sum_rb5;
static RadioButton mode_fac_sum_rb6;
static RadioButton mode_fac_sum_rb7;
static RadioButton mode_fac_sum_rb8;
static RadioButton mode_fac_sum_rb9;
static RadioButton mode_fac_sum_rb10;

// Text Object
static Text mode_fac_sum_text1;

// Button Object
static Button mode_fac_sum_button;
static ButtonGroup mode_fac_sum_buttongroup(2);

// Graph Icon Object
static GraphPerformanceIndicatorYear mode_fac_sum_graphicon_y[6];
static GraphPerformanceIndicatorTrimester mode_fac_sum_graphicon_t[16];
static GraphPerformanceIndicatorMonth mode_fac_sum_graphicon_m[1];

// Data Text
static Text mode_fac_sum_data1[16];
static Text mode_fac_sum_data2[4];
static Text mode_fac_sum_data3[MAX_DEPARTMENT];
static Text mode_fac_sum_data4[7];

// ValueSlider
static ValueSlider mode_fac_sum_valueslider[MAX_DEPARTMENT];
static ValueSlider mode_fac_sum_valueslider2[MAX_DEPARTMENT];
static ValueSlider mode_fac_sum_valueslider3[MAX_DEPARTMENT];

// new graph
static GraphPerformanceIndicatorYear department_equity_graph;
static GraphPerformanceIndicatorYear gender_ethnic_equity_graph;
static GraphPerformanceIndicatorYear rank_equity_graph;
static Text department_equity_text;
static Text gender_ethnic_equity_text;
static Text rank_equity_text;

//----- Begin of Summary Report 1 ------//
void FacultyRes::summary_report_1(int refreshFlag) {
    static int percentDataArray[MAX_STUDENT_LEVEL_N_TOTAL];
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//

    int deptRecno=1;
    int rankLevelCount[MAX_STUDENT_LEVEL_N_TOTAL];

    // #####  Begin MArco ##### //
    int initialRankLevelCount[MAX_STUDENT_LEVEL_N_TOTAL];
    // ##### End MArco ##### //

    static int data_num=HISTORY_YEAR_COUNT;
    static int *dataArray[MAX_STUDENT_LEVEL_N_TOTAL];

    // ##### Begin Marco ##### //
    //	static int *initialDataArray[MAX_STUDENT_LEVEL_N_TOTAL];
    int initialDataArray[MAX_STUDENT_LEVEL_N_TOTAL];//NEW
    //	memset( initialRankLevelCount,0, sizeof(initialRankLevelCount) );
    // ##### End Marco ##### //

    memset( rankLevelCount,0, sizeof(rankLevelCount) );

    float avgSalary[MAX_STUDENT_LEVEL_N_TOTAL];
    float sumSalary[MAX_STUDENT_LEVEL_N_TOTAL];
    int totalStaff[MAX_STUDENT_LEVEL_N_TOTAL];

    for ( int j=0; j<MAX_STUDENT_LEVEL_N_TOTAL; j++ ) {
	avgSalary[j] = 0.0;
	sumSalary[j] = 0.0;
	totalStaff[j] = 0;
    }

    //---- determine the department for display the faculty records ----//
    if(current_report_mode==REPORT_MODE_DEPARTMENT) {
	if( department_array.selected_recno )
	    deptRecno = department_array.selected_recno;
	cur_dept_ptr = department_array[deptRecno];

	//-- department wide --//
	for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++) {
	    dataArray[i]=cur_dept_ptr->faculty_level_history[i];
	    rankLevelCount[i]=dataArray[i][data_num-1];

	    // ##### Begin Marco ##### //
	    initialDataArray[i]=cur_dept_ptr->initial_faculty_level[i];
	    //			initialRankLevelCount[i]=initialDataArray[i][data_num-1];
	    initialRankLevelCount[i]=initialDataArray[i];
	    // ##### End Marco ##### //
	}

	// calculate average salary
	for ( int i=1; i<=cur_dept_ptr->faculty_array.size(); i++ ) {
	    if ( cur_dept_ptr->faculty_array.is_deleted(i) )
		continue;

	    Faculty* facultyPtr = (Faculty*) cur_dept_ptr->faculty_array.get(i);

	    sumSalary[facultyPtr->rank_level] += facultyPtr->salary;
	}

	for ( int i=0; i<MAX_STUDENT_LEVEL_N_TOTAL-1; i++ ) {
	    if ( rankLevelCount[i] == 0 )
		avgSalary[i] = 0;
	    else
		avgSalary[i] = sumSalary[i] / rankLevelCount[i];

	    avgSalary[MAX_STUDENT_LEVEL_N_TOTAL-1] += sumSalary[i];
	}

	// calculate average in whole institution
	int sumLevelCount=0;
	for ( int i=0; i<MAX_STUDENT_LEVEL_N_TOTAL-1; i++ ) {
	    sumLevelCount += rankLevelCount[i];
	}
	avgSalary[MAX_STUDENT_LEVEL_N_TOTAL-1] /= sumLevelCount;
    }
    else {
	//	cur_dept_ptr->faculty_array.faculty_count;
	for ( int j=1; j<=department_array.size(); j++ ) {
	    Department* deptPtr = department_array[j];

	    if ( !deptPtr )
		continue;

	    for ( int k=1; k<=deptPtr->faculty_array.size(); k++ ) {
		Faculty* facultyPtr = deptPtr->faculty_array[k];

		if ( !facultyPtr )
		    continue;

		sumSalary[facultyPtr->rank_level] += facultyPtr->salary;
		totalStaff[facultyPtr->rank_level]++;
	    }
	}

	for ( int k=0; k<MAX_STUDENT_LEVEL_N_TOTAL; k++ ) {
	    dataArray[k]=department_array.faculty_level_history[k];
	    rankLevelCount[k]=dataArray[k][data_num-1];
	}

	for ( int k=0; k<MAX_STUDENT_LEVEL_N_TOTAL-1; k++ ) {
	    if ( totalStaff[k] == 0 )
		avgSalary[k] = 0;
	    else
		avgSalary[k] = sumSalary[k] / rankLevelCount[k];

	    avgSalary[MAX_STUDENT_LEVEL_N_TOTAL-1] += avgSalary[k]*rankLevelCount[k];
	    totalStaff[MAX_STUDENT_LEVEL_N_TOTAL-1] += rankLevelCount[k];
	}

	// calculate average salary in the institution
	avgSalary[MAX_STUDENT_LEVEL_N_TOTAL-1] /= totalStaff[MAX_STUDENT_LEVEL_N_TOTAL-1];

	//-- Institution wide --//
	for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++) {
	    dataArray[i]=department_array.faculty_level_history[i];
	    rankLevelCount[i]=dataArray[i][data_num-1];

	    // ##### Begin Marco ##### //
	    initialDataArray[i]=department_array.initial_faculty_level[i];
	    //			initialRankLevelCount[i]=initialDataArray[i][data_num-1];
	    initialRankLevelCount[i]=initialDataArray[i];
	    // ##### End Marco ##### //
	}
    }

    //	if(rankLevelCount[MAX_STUDENT_LEVEL_N_TOTAL-1]>1e-9)
    //		for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++)
    //				percentDataArray[i]=(rankLevelCount[i]*100)/rankLevelCount[MAX_STUDENT_LEVEL_N_TOTAL-1];

    int sum=0;
    for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL-1;i++)
	sum+=rankLevelCount[i];
    rankLevelCount[MAX_STUDENT_LEVEL_N_TOTAL-1]=sum;

    // ##### Begin Marco ##### //
    int initialSum=0;
    for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL-1;i++)
	initialSum+=initialRankLevelCount[i];
    initialRankLevelCount[MAX_STUDENT_LEVEL_N_TOTAL-1]=initialSum;
    // ##### End MArco ##### //

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
	    summary_report_deinit();
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
	font_charts.put(REPORT_X1+250,REPORT_Y1-7,fac_sum_title[0]);

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

	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");

	for (int i = 0; i < 7; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (int i = 7; i < 14; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
	/*
          user_interface.create_button_bitmap(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  reportTypeStrArray[i], &type_button_up_bitmap[i],
	  &type_button_down_bitmap[i], &vga_back);
          type_button_group[i].create_bitmapW(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  ((i == current_report_type) ? type_button_down_bitmap[i] : type_button_up_bitmap[i])+4);
	  }

	  {
	  user_interface.create_button_bitmap(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  reportTypeStrArray[i], &type_button_up_bitmap[i],
	  &type_button_down_bitmap[i], &vga_back);
	  type_button_group[i].create_bitmapW(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  ((i == current_report_type) ? type_button_down_bitmap[i] : type_button_up_bitmap[i])+4);
	  }
	*/
	/*		i=10;
			user_interface.create_button_bitmap(x, y,
			x+BOTTOM_BUTTON_WIDTH*2, y+BOTTOM_BUTTON_HEIGHT,
			reportTypeStrArray[i], &type_button_up_bitmap[i],
			&type_button_down_bitmap[i], &vga_back);
			type_button_group[i].create_bitmapW(x, y,
			x+BOTTOM_BUTTON_WIDTH*2, y+BOTTOM_BUTTON_HEIGHT,
			((i == current_report_type) ? type_button_down_bitmap[i] : type_button_up_bitmap[i])+4);
	*/
	report_init_flag = 1;

	fac_sum_rank_piechart.init(
	    REPORT_X1+TAB1,
	    REPORT_Y1+VTAB1+20,
	    REPORT_X1+TAB2,
	    REPORT_Y1+VTAB4,5, rankLevelCount, &xStart,&xEnd,
	    NULL, NULL, NULL, 1, 1, 0, 3);

	// print the labels on top
	font_chartsm.right_put( REPORT_X1+TAB4-20,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[0] );

	font_chartsm.right_put( REPORT_X1+TAB4+60-30,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[1] );

	font_chartsm.right_put( REPORT_X1+TAB5+57,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[2] );

	font_chartsm.right_put( REPORT_X1+TAB4+118,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[3] );

	//-------- draw the legend --------------//

	for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++) {
	    font_chartsm.put(
		REPORT_X1+TAB3+60,
		REPORT_Y1+VTAB1+i*43+43,
		fac_sum_rank_label[i]);

	    // print out average salary
	    font_chartsm.right_put( REPORT_X1+TAB4-20,
				    REPORT_Y1+VTAB1+i*43+43,
				    m.format( avgSalary[i], 2 ) );

	    /*****************Begin marco**********************/
	    // print out count
	    font_chartsm.right_put(
		REPORT_X1+TAB4+60-30,
		REPORT_Y1+VTAB1+i*43+43,
		m.format(rankLevelCount[i],4));

	    // print out %
	    font_chartsm.right_put(
		REPORT_X1+TAB5+60,
		REPORT_Y1+VTAB1+i*43+43,
		m.format(percentDataArray[i],29));

	    // print out initial value
	    font_chart_purple_sm.right_put(
		REPORT_X1+TAB4+113,
		REPORT_Y1+VTAB1+i*43+43,
		m.format(initialRankLevelCount[i],4));

	    /****************End Marco**********************/
	    mode_fac_sum_graphicon_y[i].init(
		REPORT_X1+TAB6,
		REPORT_Y1+VTAB1+i*43+30,
		REPORT_X1+TAB6+80,
		REPORT_Y1+VTAB1+i*43+69,
		1 ,&data_num ,dataArray[i] , 1, 0, 3);
	    mode_fac_sum_graphicon_y[i].paint();
	    if(i!=MAX_STUDENT_LEVEL_N_TOTAL-1) {
		user_interface.darken(
		    REPORT_X1+TAB3+30,
		    REPORT_Y1+VTAB1+43+i*43,
		    REPORT_X1+TAB3+43,
		    REPORT_Y1+VTAB1+57+i*43);

		user_interface.bar(
		    REPORT_X1+TAB3+30,
		    REPORT_Y1+VTAB1+43+i*43,
		    REPORT_X1+TAB3+41,
		    REPORT_Y1+VTAB1+55+i*43,
		    ((fac_sum_rank_piechart.series_color)[i]));
	    }
	}

	fac_sum_rank_piechart.refresh();
    }
    else {
	fac_sum_rank_piechart.refresh();
	vga_back.rest_area(text_refresh_bitmap1,0,0);

	// print the labels on top
	font_chartsm.right_put( REPORT_X1+TAB4-20,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[0] );

	font_chartsm.right_put( REPORT_X1+TAB4+60-30,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[1] );

	font_chartsm.right_put( REPORT_X1+TAB5+57,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[2] );

	font_chartsm.right_put( REPORT_X1+TAB4+118,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[3] );

	for(int i=0;i<MAX_STUDENT_LEVEL_N_TOTAL;i++) {
	    /*****************Begin Marco**********************/
	    // print out average salary
	    font_chartsm.right_put( REPORT_X1+TAB4-20,
				    REPORT_Y1+VTAB1+i*43+43,
				    m.format( avgSalary[i], 2 ) );

	    // print out count
	    font_chartsm.right_put(
		REPORT_X1+TAB4+60-30,
		REPORT_Y1+VTAB1+i*43+43,
		m.format(rankLevelCount[i],4));

	    // print out %
	    font_chartsm.right_put(
		REPORT_X1+TAB5+60,
		REPORT_Y1+VTAB1+i*43+43,
		m.format(percentDataArray[i],29));

	    // print out initial value
	    font_chart_purple_sm.right_put(
		REPORT_X1+TAB4+113,
		REPORT_Y1+VTAB1+i*43+43,
		m.format(initialRankLevelCount[i],4));

	    /*****************End Marco**********************/
	    mode_fac_sum_graphicon_y[i].refresh();
	}
    }

    //----------- paint the summary button group ---------//
    type_button_group.paint(current_report_type);

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    //##trevor 2807
    vga.use_front();
    //##trevor 2807
}

//----- End of Summary Report 1 ------//

//----- Begin of Summary Report 2 ------//
void FacultyRes::summary_report_2(int refreshFlag) {
    static int percentDataArray2[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//

    int deptRecno=1;

    int genderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    static int data_num=HISTORY_YEAR_COUNT;
    static int *dataArray[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];

    memset( genderEthnicGroupCount,0, sizeof(genderEthnicGroupCount) );

    // ##### Begin MArco ##### //
    int initialGenderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    //	static int *initialDataArray[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    //NEW
    int initialDataArray[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];

    memset( initialGenderEthnicGroupCount,0, sizeof(initialGenderEthnicGroupCount) );
    // ##### End Marco ##### //

    float avgSalary[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    float sumSalary[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    int totalStaff[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];

    for ( int j=0; j<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; j++ ) {
	avgSalary[j] = 0.0;
	sumSalary[j] = 0.0;
	totalStaff[j] = 0;
    }

    //---- determine the department for display the faculty records ----//
    if(current_report_mode==REPORT_MODE_DEPARTMENT) {
	if(department_array.selected_recno)
	    deptRecno = department_array.selected_recno;
	cur_dept_ptr = department_array[deptRecno];

	// calculate the total salary
	for ( int k=1; k<=cur_dept_ptr->faculty_array.size(); k++ ) {
	    Faculty* facultyPtr = cur_dept_ptr->faculty_array[k];

	    if ( !facultyPtr )
		continue;

	    sumSalary[facultyPtr->gender_ethnic_group] += facultyPtr->salary;
	}

	//-- department wide --//
	for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++) {
	    dataArray[i]=cur_dept_ptr->faculty_gender_history[i];
	    genderEthnicGroupCount[i]=dataArray[i][data_num-1];

	    // ##### Begin Marco ##### //
	    initialDataArray[i]=cur_dept_ptr->initial_faculty_gender[i];
	    //			initialGenderEthnicGroupCount[i]=initialDataArray[i][data_num-1];
	    initialGenderEthnicGroupCount[i]=initialDataArray[i];
	    // ##### End MArco ##### //
	}

	// calculate average salary
	for ( int i=0; i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1; i++ ) {
	    if ( genderEthnicGroupCount[i] == 0 )
		avgSalary[i] = 0;
	    else
		avgSalary[i] = sumSalary[i] / genderEthnicGroupCount[i];

	    avgSalary[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1] += avgSalary[i]*genderEthnicGroupCount[i];
	}

	avgSalary[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1] /= genderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1];
    }
    else {
	//-- Institution wide --//
	for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++) {
	    dataArray[i]=department_array.faculty_gender_history[i];
	    genderEthnicGroupCount[i]=dataArray[i][data_num-1];

	    // ##### Begin Marco ##### //
	    initialDataArray[i]=department_array.initial_faculty_gender[i];
	    //			initialGenderEthnicGroupCount[i]=initialDataArray[i][data_num-1];
	    initialGenderEthnicGroupCount[i]=initialDataArray[i];
	    // ##### End Marco ##### //
	}

	// calculate total salary
	for ( int i=1; i<=department_array.size(); i++ ) {
	    Department* deptPtr = department_array[i];

	    if ( !deptPtr )
		continue;

	    for ( int j=1; j<=deptPtr->faculty_array.size(); j++ ) {
		Faculty* facultyPtr = (Faculty*) deptPtr->faculty_array[j];

		if ( !facultyPtr )
		    continue;

		sumSalary[facultyPtr->gender_ethnic_group] += facultyPtr->salary;
	    }
	}

	// calculate average salary
	for ( int i=0; i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1; i++ ) {
	    if ( genderEthnicGroupCount[i] == 0 )
		avgSalary[i] = 0;
	    else
		avgSalary[i] = sumSalary[i] / genderEthnicGroupCount[i];

	    avgSalary[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1] += avgSalary[i]*genderEthnicGroupCount[i];
	}

	// calculate average salary in whole institution
	avgSalary[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1] /=  genderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1];

    }

    //	if(genderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1]>1e-9)
    //		for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++)
    //			percentDataArray2[i]=(genderEthnicGroupCount[i]*100)/genderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1];

    int sum=0;
    for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1;i++)
	sum+=genderEthnicGroupCount[i];
    genderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1]=sum;

    // ##### Begin Marco ##### //
    int initialSum=0;
    for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1;i++)
	initialSum+=initialGenderEthnicGroupCount[i];
    initialGenderEthnicGroupCount[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1]=initialSum;
    // ##### End Marco ##### //

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
	    summary_report_deinit();

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
	  REPORT_X1+TAB2+375,
	  REPORT_Y1+VTAB1+8);

	  user_interface.rect(
	  REPORT_X1+TAB2+45,
	  REPORT_Y1+VTAB1-15,
	  REPORT_X1+TAB2+375,
	  REPORT_Y1+VTAB1+8);

	  font_charts.put(
	  REPORT_X1+TAB2+60,
	  REPORT_Y1+VTAB1-15,
	  fac_sum_title[1]);
	*/
	// top title
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	font_charts.put(REPORT_X1+230,REPORT_Y1-7,fac_sum_title[1]);

	//----- initialize the bottom sub-menus buttons -----//

	text_refresh_bitmap1=NULL;
	text_refresh_bitmap1=vga_back.save_area(
	    REPORT_X1+TAB4,
	    REPORT_Y1+VTAB1,
	    REPORT_X2-100,
	    REPORT_Y1+VTAB4,
	    text_refresh_bitmap1);

	fac_sum_gender_ethnic_piechart.init(
	    REPORT_X1+TAB1,
	    REPORT_Y1+VTAB1+20,
	    REPORT_X1+TAB2,
	    REPORT_Y1+VTAB4,
	    4, genderEthnicGroupCount, &xStart,&xEnd,
	    NULL, NULL, NULL, 1, 1, 0, 3);

	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;

	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");

	for (int i = 0; i < 7; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (int i = 7; i < 14; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
	/*
	  for (int i = 0; i < 6; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	  {
          user_interface.create_button_bitmap(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  reportTypeStrArray[i], &type_button_up_bitmap[i],
	  &type_button_down_bitmap[i], &vga_back);
          type_button_group[i].create_bitmapW(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  ((i == current_report_type) ? type_button_down_bitmap[i] : type_button_up_bitmap[i])+4);
	  }

	  x = BOTTOM_BUTTON_X1;
	  y = BOTTOM_BUTTON_Y1+28;

	  for (i = 6; i < 12; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	  {
	  user_interface.create_button_bitmap(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  reportTypeStrArray[i], &type_button_up_bitmap[i],
	  &type_button_down_bitmap[i], &vga_back);
	  type_button_group[i].create_bitmapW(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  ((i == current_report_type) ? type_button_down_bitmap[i] : type_button_up_bitmap[i])+4);
	  }
	*/
	report_init_flag = 1;

	// print the labels on top
	font_chartsm.right_put( REPORT_X1+TAB4-20,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[0] );

	font_chartsm.right_put( REPORT_X1+TAB4+60-30,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[1] );

	font_chartsm.right_put( REPORT_X1+TAB5+57,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[2] );

	font_chartsm.right_put( REPORT_X1+TAB4+118,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[3] );

	//-------- draw the legend --------------//

	for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++) {
	    font_chartsm.right_put( REPORT_X1+TAB4-20,
				    REPORT_Y1+VTAB1+i*47+43,
				    m.format( avgSalary[i], 2 ) );
	    font_chartsm.put(
		REPORT_X1+TAB3+60,
		REPORT_Y1+VTAB1+i*47+43,
		fac_sum_ge_label[i]);
	    // ##### Begin Marco ##### //
	    font_chartsm.right_put(
		REPORT_X1+TAB4+60-30,
		REPORT_Y1+VTAB1+i*47+43,
		m.format(genderEthnicGroupCount[i],4));

	    // print out %
	    font_chartsm.right_put(
		REPORT_X1+TAB5+60,
		REPORT_Y1+VTAB1+i*47+43,
		m.format(percentDataArray2[i],29));

	    // print out initial value
	    font_chart_purple_sm.right_put(
		REPORT_X1+TAB4+113,
		REPORT_Y1+VTAB1+i*47+43,
		m.format(initialGenderEthnicGroupCount[i],4));

	    // ##### End Marco ##### //
	    mode_fac_sum_graphicon_y[i].init(
		REPORT_X1+TAB6,
		REPORT_Y1+VTAB1+i*47+30,
		REPORT_X1+TAB6+80,
		REPORT_Y1+VTAB1+i*47+72,
		1 ,&data_num ,dataArray[i] , 1, 0, 3);
	    mode_fac_sum_graphicon_y[i].paint();
	    if(i!=GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1) {
		user_interface.darken(
		    REPORT_X1+TAB3+30,
		    REPORT_Y1+VTAB1+43+i*47,
		    REPORT_X1+TAB3+43,
		    REPORT_Y1+VTAB1+57+i*47
		    );
		user_interface.bar(
		    REPORT_X1+TAB3+30,
		    REPORT_Y1+VTAB1+43+i*47,
		    REPORT_X1+TAB3+41,
		    REPORT_Y1+VTAB1+55+i*47,
		    ((fac_sum_gender_ethnic_piechart.series_color)[i]));
	    }
	}
    }
    else {
	vga_back.rest_area(text_refresh_bitmap1,0,0);

	// print the labels on top
	font_chartsm.right_put( REPORT_X1+TAB4-20,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[0] );

	font_chartsm.right_put( REPORT_X1+TAB4+60-30,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[1] );

	font_chartsm.right_put( REPORT_X1+TAB5+57,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[2] );

	font_chartsm.right_put( REPORT_X1+TAB4+118,
				REPORT_Y1+VTAB1+23,
				fac_avg_salary_label[3] );

	for(int i=0;i<GENDER_ETHNIC_TYPE_COUNT_N_TOTAL;i++) {
	    // ##### Begin Marco ##### //
	    font_chartsm.right_put(
		REPORT_X1+TAB4+60-30,
		REPORT_Y1+VTAB1+i*47+43,
		m.format(genderEthnicGroupCount[i],4));
	    // print out %
	    font_chartsm.right_put(
		REPORT_X1+TAB5+60,
		REPORT_Y1+VTAB1+i*47+43,
		m.format(percentDataArray2[i],29));

	    // print out initial value
	    font_chart_purple_sm.right_put(
		REPORT_X1+TAB4+113,
		REPORT_Y1+VTAB1+i*47+43,
		m.format(initialGenderEthnicGroupCount[i],4));
	    // ##### End Marco ##### //
	    mode_fac_sum_graphicon_y[i].refresh();
	}
    }

    fac_sum_gender_ethnic_piechart.refresh();
    //----------- paint the summary button group ---------//
    type_button_group.paint(current_report_type);

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    vga.use_front();
}

//----- End of Summary Report 2 ------//

//----- Begin of Summary Report 3 ------//  (activities)
void FacultyRes::summary_report_3(int refreshFlag) {
    int tab1=REPORT_X1+0;
    int tab2=REPORT_X1+150;
    int tab3=REPORT_X1+190;
    int tab4=REPORT_X1+280;
    int tab5=REPORT_X1+320;
    int tab6=REPORT_X1+410;
    int tab7=REPORT_X1+450;
    int tab8=REPORT_X1+540;
    int tab9=REPORT_X1+710;
    int tab11=REPORT_X1+5;
    int tab12=REPORT_X1+210;
    int tab13=REPORT_X1+260;
    int tab14=REPORT_X1+350;
    int tab15=REPORT_X1+580;
    int tab16=REPORT_X1+620;
    int tab17=REPORT_X1+710;

    int vtab1=REPORT_Y1+10;
    int vtab2=REPORT_Y1+25;
    int vtab3=REPORT_Y1+100;
    int vtab4=REPORT_Y1+105;
    int vtab5=REPORT_Y1+340;

    static int data_num1=HISTORY_TRIMESTER_COUNT;
    static int data_num2=HISTORY_MONTH_COUNT;
    /*	static int v1[3]={12,32,21};
	static int v2[3]={14,42,26};
	static float v3[4]={52,33,23,31};
	static float v4[4]={52,33,12,31};
	static float v5;
	//	static int v6=21;

	static int v7[8]={5,5,5,5,5,5,5,5};
    */
    // ##### Begin Marco ####### //
    static float v1[3]={12,32,21};
    static float v2[3]={14,42,26};
    static float v3[4]={52,33,23,31};
    static float v4[5]={52,33,12,31,31};
    static float v5;
    //	static int v6=21;

    static int v7[8]={5,5,5,5,5,5,5,5};
    // ##### End Marco ####### //
    static float dataArray1[7][HISTORY_TRIMESTER_COUNT];

    static float dataArray2[1][HISTORY_MONTH_COUNT];
    static float dataArray3[DISCRETIONARY_TYPE_COUNT_N_TOTAL+1][HISTORY_TRIMESTER_COUNT];
    static int xStart = 1991, xEnd = 1998;          //*** temporary ***//

    // ##### Begin Marco ##### //
    //	static int initialv1[3]={12,32,21};
    //	static int initialv2[3]={14,42,26};
    static float initialv1[3]={12,32,21};
    static float initialv2[3]={14,42,26};
    static float initialv3[4]={52,33,23,31};
    static float initialv4[5]={52,33,12,31,31};
    static float initialv5;
    //	static int v6=21;
    //	static int initialv7[8]={5,5,5,5,5,5,5,5};
    static float initialv7[8]={5,5,5,5,5,5,5,5};
    //	static float initialDataArray3[DISCRETIONARY_TYPE_COUNT_N_TOTAL+1][HISTORY_TRIMESTER_COUNT];
    // ##### End Marco ##### //

    int deptRecno=1;

    memset( dataArray1,0, sizeof(dataArray1) );
    memset( dataArray2,0, sizeof(dataArray2) );
    memset( dataArray3,0, sizeof(dataArray3) );

    // ##### Begin Marco ##### //
    //	memset( initialDataArray3,0, sizeof(initialDataArray3) );
    // ##### End Marco ##### //

    //---- determine the department for display the faculty records ----//
    if(current_report_mode==REPORT_MODE_DEPARTMENT) {
	if( department_array.selected_recno )
	    deptRecno = department_array.selected_recno;

	cur_dept_ptr = department_array[deptRecno];

	//-- department wide --//
	for(int i=0;i<HISTORY_TRIMESTER_COUNT;i++) {
	    dataArray1[0][i]=(int)cur_dept_ptr->faculty_array.talent_teaching[i];
	    dataArray1[1][i]=(int)cur_dept_ptr->faculty_array.talent_scholarship[i];
	    dataArray1[2][i]=(int)cur_dept_ptr->faculty_array.talent_research[i];
	    dataArray1[3][i]=(int)cur_dept_ptr->faculty_array.performance_teaching[i];
	    dataArray1[4][i]=(int)cur_dept_ptr->faculty_array.performance_scholarship[i];
	    dataArray1[5][i]=(int)cur_dept_ptr->faculty_array.performance_research[i];
	}
	initialv1[0]=cur_dept_ptr->faculty_array.initial_talent_teaching;
	initialv1[1]=cur_dept_ptr->faculty_array.initial_talent_scholarship;
	initialv1[2]=cur_dept_ptr->faculty_array.initial_talent_research;
	initialv2[0]=cur_dept_ptr->faculty_array.initial_performance_teaching;
	initialv2[1]=cur_dept_ptr->faculty_array.initial_performance_scholarship;
	initialv2[2]=cur_dept_ptr->faculty_array.initial_performance_research;

	for(int i=0;i<HISTORY_MONTH_COUNT;i++) {
	    dataArray2[0][i]=cur_dept_ptr->p_faculty_morale_history[i];
	}
	initialv5=cur_dept_ptr->initial_p_faculty_morale;

	/*
	  for(i=0;i<HISTORY_TRIMESTER_COUNT;i++)
	  {
          dataArray3[0][i]=(int)cur_dept_ptr->faculty_array.teaching_contact_hour[i];
          for(int j=0;j<DISCRETIONARY_TYPE_COUNT_N_TOTAL;j++)
	  dataArray3[j+1][i]=(int)cur_dept_ptr->faculty_array.discretionary_hour_array[j][i];
	  }
	*/
	for(int i=0;i<HISTORY_TRIMESTER_COUNT;i++) {
	    dataArray3[0][i]=cur_dept_ptr->faculty_array.teaching_contact_hour[i];
	    for(int j=0;j<DISCRETIONARY_TYPE_COUNT_N_TOTAL;j++)
		dataArray3[j+1][i]=cur_dept_ptr->faculty_array.discretionary_hour_array[j][i];
	}
	v7[0]=cur_dept_ptr->pressure_to_change_teaching_load;
	// ##### Begin MArco ##### //
	initialv7[0]=cur_dept_ptr->initial_pressure_to_change_teaching_load;

	initialv3[0]=cur_dept_ptr->faculty_array.initial_teaching_contact_hour;
	// size of initial_discretionary_hour_array
	err_when( DISCRETIONARY_TYPE_COUNT_N_TOTAL < 7 );
	initialv3[1]=cur_dept_ptr->faculty_array.initial_discretionary_hour_array[0];
	initialv3[2]=cur_dept_ptr->faculty_array.initial_discretionary_hour_array[1];
	initialv3[3]=cur_dept_ptr->faculty_array.initial_discretionary_hour_array[2];
	initialv4[0]=cur_dept_ptr->faculty_array.initial_discretionary_hour_array[3];
	initialv4[1]=cur_dept_ptr->faculty_array.initial_discretionary_hour_array[4];
	initialv4[2]=cur_dept_ptr->faculty_array.initial_discretionary_hour_array[5];
	initialv4[3]=cur_dept_ptr->faculty_array.initial_discretionary_hour_array[6];
	initialv4[4]=cur_dept_ptr->faculty_array.initial_time_shifted_scholarship;

	v4[4] = cur_dept_ptr->faculty_array.time_shifted_scholarship;
	// ##### End Marco ##### //
	for(int i=0;i<6;i++) {
	    v7[i+1]=cur_dept_ptr->priority_discretionary_hour_array[i];
	    // ##### Begin MArco ##### //
	    initialv7[i+1]=cur_dept_ptr->initial_priority_discretionary_hour_array[i];
	}
	// ##### End Marco ##### //
    }
    else {
	//-- institution wide chwg0116 --//
	for(int i=0;i<HISTORY_TRIMESTER_COUNT;i++) {
	    dataArray1[0][i]=(int)department_array.talent_teaching[i];
	    dataArray1[1][i]=(int)department_array.talent_scholarship[i];
	    dataArray1[2][i]=(int)department_array.talent_research[i];
	    dataArray1[3][i]=(int)department_array.performance_teaching[i];
	    dataArray1[4][i]=(int)department_array.performance_scholarship[i];
	    dataArray1[5][i]=(int)department_array.performance_research[i];
	}
	initialv1[0]=department_array.initial_talent_teaching;
	initialv1[1]=department_array.initial_talent_scholarship;
	initialv1[2]=department_array.initial_talent_research;
	initialv2[0]=department_array.initial_performance_teaching;
	initialv2[1]=department_array.initial_performance_scholarship;
	initialv2[2]=department_array.initial_performance_research;

	v4[4] = department_array.time_shifted_scholarship;

	for(int i=0;i<HISTORY_MONTH_COUNT;i++) {
	    dataArray2[0][i]=department_array.p_faculty_morale_history[i];
	}
	initialv5=department_array.initial_p_faculty_morale;

	for(int i=0;i<HISTORY_TRIMESTER_COUNT;i++) {
	    //			dataArray3[0][i]=(int)department_array.teaching_contact_hour[i];
	    //			for(int j=0;j<DISCRETIONARY_TYPE_COUNT_N_TOTAL;j++)
	    //				dataArray3[j+1][i]=(int)department_array.discretionary_hour_array[j][i];

	    // ##### begin Marco ##### //
	    dataArray3[0][i]=department_array.teaching_contact_hour[i];
	    for(int j=0;j<DISCRETIONARY_TYPE_COUNT_N_TOTAL;j++)
		dataArray3[j+1][i]=department_array.discretionary_hour_array[j][i];
	    // #### End Marco #### //
	}
	initialv3[0]=department_array.initial_teaching_contact_hour;
	// size of initial_discretionary_hour_array
	err_when( DISCRETIONARY_TYPE_COUNT_N_TOTAL < 7 );
	initialv3[1]=department_array.initial_discretionary_hour_array[0];
	initialv3[2]=department_array.initial_discretionary_hour_array[1];
	initialv3[3]=department_array.initial_discretionary_hour_array[2];
	initialv4[0]=department_array.initial_discretionary_hour_array[3];
	initialv4[1]=department_array.initial_discretionary_hour_array[4];
	initialv4[2]=department_array.initial_discretionary_hour_array[5];
	initialv4[3]=department_array.initial_discretionary_hour_array[6];
	initialv4[4]=department_array.initial_time_shifted_scholarship;
    }

    for(int i=0;i<3;i++) {
	v1[i]=dataArray1[i][HISTORY_TRIMESTER_COUNT-1];
	v2[i]=dataArray1[i+3][HISTORY_TRIMESTER_COUNT-1];
    }
    for(int i=0;i<4;i++) {
	v3[i]=dataArray3[i][HISTORY_TRIMESTER_COUNT-1];
	v4[i]=dataArray3[i+4][HISTORY_TRIMESTER_COUNT-1];
    }

    v5=dataArray2[0][HISTORY_MONTH_COUNT-1];
    // ##### Begin Marco #### //
    // initialv5 already set to department or department_array.initial_p_faculty_morale

    // round the value to 1 dec. place and then calculate the total
    for (int i =0; i <4; i++) {
	v3[i]=m.round_one_dec(v3[i]);
	v4[i]=m.round_one_dec(v4[i]);
	initialv3[i]=m.round_one_dec(initialv3[i]);
	initialv4[i]=m.round_one_dec(initialv4[i]);
    }

    v4[3] = 0;
    initialv4[3] = 0;

    for (int i =0; i <3; i++) {
	v4[3]+=v4[i]+v3[i];
	initialv4[3]+=initialv4[i]+initialv3[i];
    }
    v4[3]+=v3[3];
    initialv4[3]+=initialv3[3];

    v4[3]+=v4[4];
    initialv4[3]+=initialv4[4];
    // ##### End Marco #### //

    //------ paint the background -------//
    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	// deinit it first if it has already been initialized
	if( report_init_flag )
	    summary_report_deinit();
	report_init_flag = 1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);
	user_interface.brighten(tab1,vtab2,tab9,vtab3+3);
	//		user_interface.brighten(tab1,vtab4,tab9,vtab5);
	user_interface.rect(tab1,vtab2,tab8,vtab3+3,1);
	user_interface.rect(tab8-3,vtab2,tab9-1,vtab3+3,1);

	user_interface.brighten(tab11-5,vtab4-1,tab11+345,vtab4+58);
	user_interface.brighten(tab14,vtab4-1,tab14+358,vtab4+58);
	user_interface.brighten(tab11-5,vtab4+60,tab11+345,vtab4+118);
	user_interface.brighten(tab14,vtab4+60,tab14+358,vtab4+118);
	user_interface.brighten(tab11-5,vtab4+120,tab11+345,vtab4+178);
	user_interface.brighten(tab14,vtab4+120,tab14+358,vtab4+178);
	user_interface.brighten(tab11-5,vtab4+180,tab11+345,vtab4+234);
	user_interface.brighten(tab14,vtab4+180,tab14+358,vtab4+234);

	user_interface.rect(tab11-5,vtab4-1,tab11+346,vtab4+59,1);
	user_interface.rect(tab14,vtab4-1,tab14+359,vtab4+59,1);
	user_interface.rect(tab11-5,vtab4+60,tab11+346,vtab4+119,1);
	user_interface.rect(tab14,vtab4+60,tab14+359,vtab4+119,1);
	user_interface.rect(tab11-5,vtab4+120,tab11+346,vtab4+179,1);
	user_interface.rect(tab14,vtab4+120,tab14+359,vtab4+179,1);
	user_interface.rect(tab11-5,vtab4+180,tab11+346,vtab4+235,1);
	user_interface.rect(tab14,vtab4+180,tab14+359,vtab4+235,1);

	//		user_interface.rect(tab1,vtab4,tab14,vtab5);
	//		user_interface.rect(tab14,vtab4,tab9,vtab5);
	//		user_interface.rect(tab1,vtab4+60,tab9,vtab4+120);
	//		user_interface.rect(tab1,vtab4+120,tab9,vtab4+180);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+8);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+8,1);
	font_charts.put(REPORT_X1+300,REPORT_Y1-8,"Faculty Activities");

	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;

	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");

	for (int i = 0; i < 7; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)

		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (int i = 7; i < 14; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)

		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);

	//----------Print labels ----------//
	font_chartsm.put(tab1+20,vtab2+15,"Talent");
	font_chartsm.put(tab1+20,vtab2+45,"Performance");
	font_chartsm.put(tab2,vtab1,"   Teaching");
	font_chartsm.put(tab4,vtab1,"Scholarship");
	font_chartsm.put(tab6,vtab1,"   Research");
	font_chartsm.put(tab8,vtab2+10,"  Faculty morale");

	// font_chartsm.put(tab11+10,vtab4+10,"Normal teaching load");
	// display actual teaching load
	font_chartsm.put(tab11+10,vtab4+10,"Actual teaching load");
	font_chartsm.put(tab11+10,vtab4+70,"Course preparation");
	// ##### Begin Marco ##### //
	font_chartsm.put(tab11+10,vtab4+130,"Out-of-class student");
	font_chartsm.put(tab11+10,vtab4+146,"contact");
	font_chartsm.put(tab11+10,vtab4+190,"Educational");
	font_chartsm.put(tab11+10,vtab4+206, "development");

	// ##### End Marco ##### //

	font_chartsm.put(tab14+20,vtab4+10,"Research");
	font_chartsm.put(tab14+20,vtab4+70,"Scholarship");
	// ##### Begin Marco ##### //
	font_chartsm.put(tab14+20,vtab4+130,"Institutional and");
	font_chartsm.put(tab14+20,vtab4+146, "public service");

	font_chartsm.put(tab14+20,vtab4+190, "Technology development time shifted");
	font_chartsm.put(tab14+20,vtab4+206, "from scholarship");
	font_chartsm.put(tab14+20,vtab4+220,"Total");

	int tab11=REPORT_X1+5;

	//---------- data text -----------//		(upper block percentage)
	// ##### Begin Marco ##### //
	// upper block current
	font_chartsm.put(tab2-30,vtab2+15,m.format(v1[0],3));
	font_chartsm.put(tab4-30,vtab2+15,m.format(v1[1],3));
	font_chartsm.put(tab6-30,vtab2+15,m.format(v1[2],3));
	font_chartsm.put(tab2-30,vtab2+45,m.format(v2[0],3));
	font_chartsm.put(tab4-30,vtab2+45,m.format(v2[1],3));
	font_chartsm.put(tab6-30,vtab2+45,m.format(v2[2],3));
	font_chartsm.put(tab9-70,vtab2+10,m.format(v5,3));

	// upper block initial
	font_chart_purple_sm.put(tab2+5, vtab2+15, m.format(initialv1[0],3));
	font_chart_purple_sm.put(tab4+5, vtab2+15, m.format(initialv1[1],3));
	font_chart_purple_sm.put(tab6+5, vtab2+15, m.format(initialv1[2],3));
	font_chart_purple_sm.put(tab2+5, vtab2+45, m.format(initialv2[0],3));
	font_chart_purple_sm.put(tab4+5, vtab2+45, m.format(initialv2[1],3));
	font_chart_purple_sm.put(tab6+5, vtab2+45, m.format(initialv2[2],3));
	font_chart_purple_sm.put(tab9-40, vtab2+10, m.format(initialv5,3));

	//	lower block left current
	font_chartsm.put(tab12-70, vtab4+10, m.format(v3[0],26));
	font_chartsm.put(tab12-70, vtab4+70, m.format(v3[1],26));
	font_chartsm.put(tab12-70, vtab4+130, m.format(v3[2],26));
	font_chartsm.put(tab12-70, vtab4+190, m.format(v3[3],26));

	// lower lock left initial
	font_chart_purple_sm.put(tab12-40,vtab4+10,m.format(initialv3[0],13));
	font_chart_purple_sm.put(tab12-40,vtab4+70,m.format(initialv3[1],13));
	font_chart_purple_sm.put(tab12-40,vtab4+130,m.format(initialv3[2],13));
	font_chart_purple_sm.put(tab12-40,vtab4+190,m.format(initialv3[3],13));

	// "hrs/week"
	font_chartsm.put(tab12, vtab4+10, "hrs/week");
	font_chartsm.put(tab12, vtab4+70, "hrs/week");
	font_chartsm.put(tab12, vtab4+130, "hrs/week");
	font_chartsm.put(tab12, vtab4+190, "hrs/week");

	// lower block right current
	font_chartsm.put(tab15-95, vtab4+10, m.format(v4[0],26));
	font_chartsm.put(tab15-95, vtab4+70, m.format(v4[1],26));
	font_chartsm.put(tab15-95, vtab4+130, m.format(v4[2],26));
	font_chartsm.put(tab15-95, vtab4+220, m.format(v4[3],26));
	font_chartsm.put(tab15-95, vtab4+206, m.format(v4[4],26));

	// lower block right initial
	font_chart_purple_sm.put(tab15-50,vtab4+10,m.format(initialv4[0],13));
	font_chart_purple_sm.put(tab15-50,vtab4+70,m.format(initialv4[1],13));
	font_chart_purple_sm.put(tab15-50,vtab4+130,m.format(initialv4[2],13));
	font_chart_purple_sm.put(tab15-50,vtab4+220,m.format(initialv4[3],13));
	font_chart_purple_sm.put(tab15-50,vtab4+206,m.format(initialv4[4],13));

	font_chartsm.put(tab15-10, vtab4+10, "hrs/week");
	font_chartsm.put(tab15-10, vtab4+70, "hrs/week");
	font_chartsm.put(tab15-10, vtab4+130, "hrs/week");
	font_chartsm.put(tab15-10, vtab4+206, "hrs/week");
	font_chartsm.put(tab15-10, vtab4+220, "hrs/week");
	// ##### End Marco ##### //

	//---------- Graph Icon -----------//
	mode_fac_sum_graphicon_t[0].init(
	    tab3,vtab2+5,tab3+50,vtab2+35,
	    1 ,&data_num1 ,dataArray1[0] , 1, 0, 3);
	mode_fac_sum_graphicon_t[1].init(
	    tab5,vtab2+5,tab5+50,vtab2+35,
	    1 ,&data_num1 ,dataArray1[1] , 1, 0, 3);
	mode_fac_sum_graphicon_t[2].init(
	    tab7,vtab2+5,tab7+50,vtab2+35,
	    1 ,&data_num1 ,dataArray1[2] , 1, 0, 3);

	mode_fac_sum_graphicon_t[3].init(
	    tab3,vtab2+40,tab3+50,vtab2+70,
	    1 ,&data_num1 ,dataArray1[3] , 1, 0, 3);
	mode_fac_sum_graphicon_t[4].init(
	    tab5,vtab2+40,tab5+50,vtab2+70,
	    1 ,&data_num1 ,dataArray1[4] , 1, 0, 3);
	mode_fac_sum_graphicon_t[5].init(
	    tab7,vtab2+40,tab7+50,vtab2+70,
	    1 ,&data_num1 ,dataArray1[5] , 1, 0, 3);

	mode_fac_sum_graphicon_m[0].init(
	    tab8+30,vtab3-45,tab9-30,vtab3-5,
	    1 ,&data_num2,dataArray2[0] , 1, 0, 3);

	double maxUpperBound=10.0;
	for(int i=0;i<7;i++)
	    for(int j=0;j<HISTORY_TRIMESTER_COUNT;j++) {
		if(dataArray3[i][j]>maxUpperBound)
		    maxUpperBound=dataArray3[i][j];
	    }
	mode_fac_sum_graphicon_t[7].init(
	    tab13+10,vtab4+10,tab14-10,vtab4+50,
	    1 ,&data_num1 ,dataArray3[0],
	    1, 0, 3, maxUpperBound);
	mode_fac_sum_graphicon_t[8].init(tab13+10,vtab4+70,tab14-10,vtab4+110,
					 1 ,&data_num1 ,dataArray3[1] ,
					 1, 0, 3, maxUpperBound);
	mode_fac_sum_graphicon_t[9].init(tab13+10,vtab4+130,tab14-10,vtab4+170,
					 1 ,&data_num1 ,dataArray3[2] ,
					 1, 0, 3, maxUpperBound);
	mode_fac_sum_graphicon_t[10].init(tab13+10,vtab4+190,tab14-10,vtab4+230,
					  1 ,&data_num1 ,dataArray3[3] ,
					  1, 0, 3, maxUpperBound);
	mode_fac_sum_graphicon_t[11].init(tab16+10,vtab4+10,tab17-10,vtab4+50,
					  1 ,&data_num1 ,dataArray3[4] ,
					  1, 0, 3, maxUpperBound);
	mode_fac_sum_graphicon_t[12].init(tab16+10,vtab4+70,tab17-10,vtab4+110,
					  1 ,&data_num1 ,dataArray3[5] ,
					  1, 0, 3, maxUpperBound);
	mode_fac_sum_graphicon_t[13].init(tab16+10,vtab4+130,tab17-10,vtab4+170,
					  1 ,&data_num1 ,dataArray3[6] ,
					  1, 0, 3, maxUpperBound);
	mode_fac_sum_graphicon_t[14].init(tab16+10,vtab4+190,tab17-10,vtab4+230,
					  1 ,&data_num1 ,dataArray3[7] , 1, 0, 3);

	type_button_group.paint(current_report_type);

	for(int i=0;i<6;i++)
	    mode_fac_sum_graphicon_t[i].paint();
	for(int i=7;i<15;i++)
	    mode_fac_sum_graphicon_t[i].paint();
	mode_fac_sum_graphicon_m[0].paint();

	//----------------------------------------//

	//------------- Priority Setting --------------------//
	if(current_report_mode == REPORT_MODE_DEPARTMENT) {
	    // ##### Begin Marco ##### //
	    font_chart_blue_sm.put_paragraph(tab11+40,vtab4+27,tab11+150,vtab4+61,"Pressure to change teaching load",-1);
	    font_chart_blue_sm.put_paragraph(tab11+95,vtab4+95,tab11+150,vtab4+125,"Priority");
	    font_chart_blue_sm.put_paragraph(tab11+95,vtab4+155,tab11+150,vtab4+185,"Priority");
	    font_chart_blue_sm.put_paragraph(tab11+95,vtab4+215,tab11+150,vtab4+245,"Priority");
	    font_chart_blue_sm.put_paragraph(tab14+100,vtab4+35,tab14+150,vtab4+65,"Priority");
	    font_chart_blue_sm.put_paragraph(tab14+100,vtab4+95,tab14+150,vtab4+125,"Priority");
	    font_chart_blue_sm.put_paragraph(tab14+100,vtab4+155,tab14+150,vtab4+185 ,"Priority");
	    // ##### End Marco ##### //

	    mode_fac_sum_valueslider[0].init(
		tab11+150,
		vtab4+32,
		100,18,-5,5,&(v7[0]),1,V_YELLOW,4);
	    for(int i=1;i<4;i++)
		mode_fac_sum_valueslider[i].init(
		    tab11+150,
		    vtab4+32+60*i,
		    100,18,0,10,&(v7[i]),1,V_YELLOW,4);
	    for(int i=0;i<3;i++)
		mode_fac_sum_valueslider[i+4].init(
		    tab14+150,
		    vtab4+32+60*i,
		    100,18,0,10,&(v7[4+i]),1,V_YELLOW,4);
	}
    }

    for(int i=0;i<15;i++)
	mode_fac_sum_data1[i].refresh();
    for(int i=0;i<6;i++)
	mode_fac_sum_graphicon_t[i].refresh();
    for(int i=7;i<15;i++)
	mode_fac_sum_graphicon_t[i].refresh();
    mode_fac_sum_graphicon_m[0].refresh();

    if(current_report_mode == REPORT_MODE_DEPARTMENT) {
	for(int i=0;i<7;i++)
	    mode_fac_sum_valueslider[i].refresh();
    }
    type_button_group.paint(current_report_type);

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//----- End of Summary Report 3 ------//

//----- Begin of Summary Report 4 ------// (research)
void FacultyRes::summary_report_4(int refreshFlag) {
    int tab1=REPORT_X1;

    // ##### Begin Marco ##### //
    int tab2=REPORT_X1+250;
    int tab3=REPORT_X1+475;
    // ##### End Marco ##### //
    int tab4=REPORT_X1+530;
    int tab5=REPORT_X1+600;
    int tab6=REPORT_X1+700;
    int vtab1=REPORT_Y1+15;
    int vtab2=REPORT_Y1+135;
    int vtab3=REPORT_Y1+140;
    int vtab4=REPORT_Y1+235;
    int vtab5=REPORT_Y1+240;
    int vtab6=REPORT_Y1+335;
    static int data_num1=HISTORY_MONTH_COUNT/4;
    static int data_num2=HISTORY_YEAR_COUNT;
    static int data_num3=HISTORY_MONTH_COUNT;
    static int dataArray1[RESEARCH_STATUS_COUNT*HISTORY_MONTH_COUNT];
    static int dataArray2[RESEARCH_STATUS_COUNT*HISTORY_YEAR_COUNT];
    static int dataArray3[2*HISTORY_MONTH_COUNT];
    static int dataArray4[HISTORY_MONTH_COUNT];
    static double v1=11,v2=22,v3=33;
    int i;

    // ##### begin MArco ##### //
    static double initialv1=11,initialv2=22,initialv3=33;
    // ##### End MArco ###### //

    if(current_report_mode == REPORT_MODE_DEPARTMENT) {
	int deptRecno = department_array.selected_recno;
	Department* cur_dept_ptr = department_array[deptRecno];

	if(current_graph_mode==MONTHLY) {
	    for(i=0;i<HISTORY_MONTH_COUNT;i++) {
		dataArray1[i]=(int)(cur_dept_ptr->research_m_history[0][i]);
		dataArray1[HISTORY_MONTH_COUNT+i]=cur_dept_ptr->research_m_history[1][i];
		dataArray1[2*HISTORY_MONTH_COUNT+i]=cur_dept_ptr->research_m_history[2][i];
	    }
	}
	else {
	    for(i=0;i<HISTORY_YEAR_COUNT;i++) {
		dataArray2[i]=cur_dept_ptr->research_y_history[0][i];
		dataArray2[HISTORY_YEAR_COUNT+i]=cur_dept_ptr->research_y_history[1][i];
		dataArray2[2*HISTORY_YEAR_COUNT+i]=cur_dept_ptr->research_y_history[2][i];
	    }
	}

	for(i=0;i<HISTORY_MONTH_COUNT;i++) {
	    dataArray3[i]=cur_dept_ptr->research_total_history[i];
	    dataArray3[HISTORY_MONTH_COUNT+i]=cur_dept_ptr->research_direct_history[i];
	    dataArray4[i]=cur_dept_ptr->regular_faculty_with_research[i];
	}
	initialv1=cur_dept_ptr->initial_research_total;
	initialv2=cur_dept_ptr->initial_research_direct;
	initialv3=cur_dept_ptr->initial_regular_faculty_with_research;

    }
    else {
	if(current_graph_mode==MONTHLY) {
	    for(i=0;i<HISTORY_MONTH_COUNT;i++) {
		dataArray1[i]=department_array.research_m_history[0][i];
		dataArray1[HISTORY_MONTH_COUNT+i]=department_array.research_m_history[1][i];
		dataArray1[2*HISTORY_MONTH_COUNT+i]=department_array.research_m_history[2][i];
	    }
	}
	else {
	    for(i=0;i<HISTORY_YEAR_COUNT;i++) {
		dataArray2[i]=department_array.research_y_history[0][i];
		dataArray2[HISTORY_YEAR_COUNT+i]=department_array.research_y_history[1][i];
		dataArray2[2*HISTORY_YEAR_COUNT+i]=department_array.research_y_history[2][i];
	    }
	}

	for(i=0;i<HISTORY_MONTH_COUNT;i++) {
	    dataArray3[i]=department_array.research_total_history[i];
	    dataArray3[HISTORY_MONTH_COUNT+i]=department_array.research_direct_history[i];
	    dataArray4[i]=department_array.regular_faculty_with_research[i];
	}
	initialv1=department_array.initial_research_total;
	initialv2=department_array.initial_research_direct;
	initialv3=department_array.initial_regular_faculty_with_research;
    }
    v1=dataArray3[HISTORY_MONTH_COUNT-1];
    v2=dataArray3[2*HISTORY_MONTH_COUNT-1];
    v3=dataArray4[HISTORY_MONTH_COUNT-1];

    //------ paint the background -------//
    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {

	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_report_deinit();

	user_interface.bg_img(BG_PIC_ID, &vga_back);

	user_interface.brighten(tab1,vtab1,tab6,vtab2);
	user_interface.brighten(tab1,vtab3,tab3,vtab4);
	user_interface.brighten(tab1,vtab5,tab3,vtab6);
	// ##### Begin Marco ##### //
	user_interface.brighten(tab3,vtab3,tab6,vtab4);
	user_interface.brighten(tab3,vtab5,tab6,vtab6);
	// ##### End Marco ##### //

	user_interface.rect(tab5-2,vtab1,tab6,vtab2,1);

	user_interface.rect(tab2-1,vtab3,tab3,vtab4,1);
	user_interface.rect(tab2-1,vtab5,tab3,vtab6,1);

	// ##### Begin Marco ##### //
	user_interface.rect(tab3,vtab3,tab6,vtab4,1);
	user_interface.rect(tab3,vtab5,tab6,vtab6,1);
	// ##### End Marco ##### //

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+8);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+8,1);

	font_charts.put(REPORT_X1+300,REPORT_Y1-8,"Sponsored Research");

	if(current_graph_mode==MONTHLY) {
	    mode_fac_sum_doublebar_m.init(tab1,vtab1,tab5,vtab2,
					  3, &data_num1, dataArray1, 1,
					  NULL, NULL, NULL, 1, 1, 0, 2);
	    mode_fac_sum_doublebar_m.paint();
	}
	else {
	    mode_fac_sum_doublebar_y.init(tab1,vtab1,tab5,vtab2,
					  3, &data_num2, dataArray2, 1,
					  NULL, NULL, NULL, 1, 1, 0, 2);
	    mode_fac_sum_doublebar_y.paint();
	}

	mode_fac_sum_graphmonth1.init(tab1,vtab3,tab2,vtab4,
				      2, &data_num3, dataArray3, 1,
				      NULL, NULL, NULL, 1, 1, 0, 2);
	mode_fac_sum_graphmonth2.init(tab1,vtab5,tab2,vtab6,
				      1, &data_num3, dataArray4, 1,
				      NULL, NULL, NULL, 1, 1, 0, 3);
	mode_fac_sum_graphmonth1.paint();
	mode_fac_sum_graphmonth2.paint();

	//-------- draw the legend --------------//
	font_chartsm.put_paragraph(
	    tab2+20,vtab3+5,tab3,vtab3+50,"Total sponsored research expenditures this month");

	font_chartsm.put_paragraph(
	    tab2+20,vtab3+50,tab3,vtab4,"Total direct sponsored research expenditures this month");

	font_chartsm.put_paragraph(
	    tab2+20,vtab5+35,tab3,vtab5+80,"Regular faculty with sponsored research");

	// ##### Begin Marco ##### //
	font_chart_purple_sm.put_paragraph(
	    tab3+10,vtab3+5,tab6,vtab3+50,"Total sponsored research expenditures initially");

	font_chart_purple_sm.put_paragraph(
	    tab3+10,vtab3+50,tab6,vtab4,"Total direct sponsored research expenditures initially");

	font_chart_purple_sm.put_paragraph(
	    tab3+10,vtab5+35,tab6,vtab5+80,"Regular faculty with sponsored research initially");

	font_chart_purple_sm.put(tab3+172,vtab3+23,m.format(initialv1,2));
	font_chart_purple_sm.put(tab3+172,vtab3+68,m.format(initialv2,2));
	font_chart_purple_sm.put(tab3+172,vtab5+52,m.format(initialv3,3));
	// ##### End Marco ##### //

	//data here
	mode_fac_sum_data2[0].put(tab2+162,vtab3+23,&v1,2);
	mode_fac_sum_data2[1].put(tab2+162,vtab3+68,&v2,2);
	mode_fac_sum_data2[2].put(tab2+162,vtab5+52,&v3,3);

	if(current_graph_mode==MONTHLY) {
	    vga_back.bar(
		tab2+8,vtab5+40,tab2+15,vtab5+47,
		(mode_fac_sum_doublebar_m.series_color)[0]);
	    vga_back.bar(
		tab2+8,vtab3+10,tab2+15,vtab3+17,
		(mode_fac_sum_doublebar_m.series_color)[0]);
	    vga_back.bar(
		tab2+8,vtab3+55,tab2+15,vtab3+62,
		(mode_fac_sum_doublebar_m.series_color)[1]);

	}
	else {
	    vga_back.bar(
		tab2+8,vtab5+40,tab2+15,vtab5+47,
		(mode_fac_sum_doublebar_y.series_color)[0]);
	    vga_back.bar(
		tab2+8,vtab3+10,tab2+15,vtab3+17,
		(mode_fac_sum_doublebar_y.series_color)[0]);
	    vga_back.bar(
		tab2+8,vtab3+55,tab2+15,vtab3+62,
		(mode_fac_sum_doublebar_y.series_color)[1]);
	}

	font_chartsm.put_paragraph(
	    tab5+18,vtab1+5,tab6,vtab1+50,"$ proposal outstanding",-2);
	font_chartsm.put(
	    tab5+18,vtab1+40,"$ rejected");
	font_chartsm.put(
	    tab5+18,vtab1+56,"$ accepted");

	if(current_graph_mode==MONTHLY) {
	    vga_back.bar(
		tab5+8,vtab1+9,tab5+15,vtab1+17,
		(mode_fac_sum_doublebar_m.series_color)[0]);
	    vga_back.bar(
		tab5+8,vtab1+44,tab5+15,vtab1+52,
		(mode_fac_sum_doublebar_m.series_color)[1]);
	    vga_back.bar(
		tab5+8,vtab1+60,tab5+15,vtab1+68,
		(mode_fac_sum_doublebar_m.series_color)[2]);
	}
	else {
	    vga_back.bar(
		tab5+8,vtab1+9,tab5+15,vtab1+17,
		(mode_fac_sum_doublebar_y.series_color)[0]);
	    vga_back.bar(
		tab5+8,vtab1+44,tab5+15,vtab1+52,
		(mode_fac_sum_doublebar_y.series_color)[1]);
	    vga_back.bar(
		tab5+8,vtab1+60,tab5+15,vtab1+68,
		(mode_fac_sum_doublebar_y.series_color)[2]);
	}

	mode_fac_sum_buttongroup[0].create_text(
	    tab5+15,vtab1+74,tab5+90,vtab1+93,"monthly",1);
	mode_fac_sum_buttongroup[1].create_text(
	    tab5+15,vtab1+96,tab5+90,vtab1+115,"annually",1);

	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;

	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");

	for (int i = 0; i < 7; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (i = 7; i < 14; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
	type_button_group.paint(current_report_type);
	mode_fac_sum_buttongroup.paint(current_graph_mode);

	report_init_flag = 1;
    }
    else {
	for(int i=0;i<3;i++)
	    mode_fac_sum_data2[i].refresh();
	if(current_graph_mode==MONTHLY)
	    mode_fac_sum_doublebar_m.refresh();
	else
	    mode_fac_sum_doublebar_y.refresh();
	mode_fac_sum_graphmonth1.refresh();
	mode_fac_sum_graphmonth2.refresh();
	type_button_group.paint(current_report_type);
	mode_fac_sum_buttongroup.paint(current_graph_mode);
    }

    //----------- paint the summary button group ---------//

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    vga.use_front();
}

//----- End of Summary Report 4 ------//

//----- Begin of Summary Report 5 ------//
void FacultyRes::summary_report_5(int refreshFlag) {
    int tab1=REPORT_X1+242;
    int vtab1=REPORT_Y1+33;

    report(refreshFlag);
    if( refreshFlag == INFO_REPAINT ) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_report_deinit();
	report_init_flag = 1;
	mode_fac_sum_button.create_text(  REPORT_X1+260, REPORT_Y2+10,
					  REPORT_X1+350, REPORT_Y2+30,"Back",1);
    }

    //	vga.use_back();
    mode_fac_sum_button.paint();
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//----- Begin of Summary Report 6 ------//
void FacultyRes::summary_report_6(int refreshFlag) {
    int tab1=REPORT_X1-10;
    int tab2=REPORT_X1;
    int tab3=REPORT_X1+135;
    int tab4=REPORT_X1+720;
    int vtab1=REPORT_Y1;
    int vtab2=REPORT_Y1+250;
    int vtab3=REPORT_Y1+255;
    int vtab4=REPORT_Y1+320;
    static int v1,v2;
    static int v3,v4;
    static int v5,v6;
    static int v7,v8;
    static int v9,v10;
    int deptRecno=1;

    if(department_array.selected_recno)
	deptRecno = department_array.selected_recno;
    cur_dept_ptr = department_array[deptRecno];

    v1 = cur_dept_ptr->assistant_professor_priority;
    v2 = cur_dept_ptr->tenured_faculty_priority;
    v3 = cur_dept_ptr->cost_containment_priority;
    v4 = cur_dept_ptr->longterm_adjunct_faculty_priority;
    v5 = cur_dept_ptr->shortterm_adjunct_faculty_priority;
    v6 = cur_dept_ptr->female_faculty_priority;
    v7 = cur_dept_ptr->minority_faculty_priority;
    v8 = cur_dept_ptr->teaching_talent_priority;
    v9 = cur_dept_ptr->scholarship_talent_priority;
    v10 = cur_dept_ptr->research_talent_priority;
    //------ paint the background -------//
    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_report_deinit();
	report_init_flag = 1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);

	user_interface.brighten(tab1,vtab1+12,tab4,vtab4);
	//		user_interface.brighten(tab1,vtab3,tab4,vtab4);
	user_interface.rect(tab1,vtab1+12,tab4,vtab4,1);
	//		user_interface.rect(tab1,vtab3,tab4,vtab4);

	// top title
	user_interface.brighten(REPORT_X1+130,REPORT_Y1-13,REPORT_X1+600,REPORT_Y1+10);
	user_interface.rect(REPORT_X1+130,REPORT_Y1-13,REPORT_X1+600,REPORT_Y1+10,1);
	font_charts.put(REPORT_X1+290,REPORT_Y1-8,"Faculty Hiring Priorities");

	font_charts.put(tab2+135+25,vtab1+22,"Nil");
	font_charts.put(tab2+270+25,vtab1+22,"Medium");
	font_charts.put(tab2+425+25,vtab1+22,"High");
	font_charts.put(tab2+565+25,vtab1+22,"Top");
	font_chart_blue_sm.put(tab2,vtab1+22+25,"New blood");
	font_chart_blue_sm.put(tab2,vtab1+47+25,"Leadership");
	font_chart_blue_sm.put(tab2,vtab1+72+25,"Cost containment");
	font_chart_blue_sm.put(tab2,vtab1+102+25,"Long-term adjunct");
	font_chart_blue_sm.put(tab2,vtab1+127+25,"Short-term adjunct");
	font_chart_blue_sm.put(tab2,vtab1+157+25,"Minority");
	font_chart_blue_sm.put(tab2,vtab1+182+25,"Female");
	font_chart_blue_sm.put(tab2,vtab1+212+25,"Teaching talent");
	font_chart_blue_sm.put(tab2,vtab1+237+25,"Scholarship talent");
	font_chart_blue_sm.put(tab2,vtab1+262+25,"Research talent");
	//		font_chartsm.put(tab2,vtab1+270,"The number of open faculty positions");
	mode_fac_sum_rb1.init(tab3+25,vtab1+20+25,tab4+25,vtab1+40+25,4,&v1,mode_fac_sum_rblabel3,NULL);
	mode_fac_sum_rb2.init(tab3+25,vtab1+45+25,tab4+25,vtab1+65+25,4,&v2,mode_fac_sum_rblabel3,NULL);
	mode_fac_sum_rb3.init(tab3+25,vtab1+70+25,tab4+25,vtab1+90+25,4,&v3,mode_fac_sum_rblabel3,NULL);
	mode_fac_sum_rb4.init(tab3+25,vtab1+100+25,tab4+25,vtab1+120+25,4,&v4,mode_fac_sum_rblabel3,NULL);
	mode_fac_sum_rb5.init(tab3+25,vtab1+125+25,tab4+25,vtab1+145+25,4,&v5,mode_fac_sum_rblabel3,NULL);
	mode_fac_sum_rb6.init(tab3+25,vtab1+155+25,tab4+25,vtab1+175+25,4,&v6,mode_fac_sum_rblabel3,NULL);
	mode_fac_sum_rb7.init(tab3+25,vtab1+180+25,tab4+25,vtab1+200+25,4,&v7,mode_fac_sum_rblabel3,NULL);
	mode_fac_sum_rb8.init(tab3+25,vtab1+210+25,tab4+25,vtab1+230+25,4,&v8,mode_fac_sum_rblabel3,NULL);
	mode_fac_sum_rb9.init(tab3+25,vtab1+235+25,tab4+25,vtab1+255+25,4,&v9,mode_fac_sum_rblabel3,NULL);
	mode_fac_sum_rb10.init(tab3+25,vtab1+260+25,tab4+25,vtab1+280+25,4,&v10,mode_fac_sum_rblabel3,NULL);
	//		mode_fac_sum_text1.put(tab2+250,vtab1+270,v8);

	//mode_fac_sum_button.create_text(tab3+150,vtab1+290,tab3+350,vtab1+310,"Hire a new faculty member",1);

	//----- initialize the bottom sub-menus buttons -----//
	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;

	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");

	for (int i = 0; i < 7; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (int i = 7; i < 14; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
    }
    else {
	mode_fac_sum_rb1.refresh();
	mode_fac_sum_rb2.refresh();
	mode_fac_sum_rb3.refresh();
	mode_fac_sum_rb4.refresh();
	mode_fac_sum_rb5.refresh();
	mode_fac_sum_rb6.refresh();
	mode_fac_sum_rb7.refresh();
	mode_fac_sum_rb8.refresh();
	mode_fac_sum_rb9.refresh();
	mode_fac_sum_rb10.refresh();
    }
    //----------- paint the summary button group ---------//
    type_button_group.paint(current_report_type);
    //mode_fac_sum_button.paint();
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//----- End of Summary Report 6 ------//

//----- Begin of Summary Report 7 ------//
void FacultyRes::summary_report_7(int refreshFlag) {
    int tab1=REPORT_X1+5;
    int tab2=REPORT_X1+15;
    int tab3=REPORT_X1+90;
    int tab4=REPORT_X1+270;
    int tab5=REPORT_X1+290;
    int tab6=REPORT_X1+555;
    // int tab7=
    int tab8=REPORT_X1+695;
    int ptab2=REPORT_X1+15;                         // tab for Teaching performance and Scholarly performance
    int ptab3=REPORT_X1+90;                         // tab for Teaching performance and Scholarly performance
    int ptab4=REPORT_X1+350;                        // tab for Research performance and Gender and ethnic equity
    int ptab5=REPORT_X1+430;                        // tab for Research performance and Gender and ethnic equity

    int vtab1=REPORT_Y1+15;
    int vtab2=REPORT_Y1+285;
    int vtab3=REPORT_Y1+290;
    int vtab4=REPORT_Y1+340;
    static int v1,v2,v3,v4;
    static int data1[MAX_DEPARTMENT];
    static int data2[MAX_DEPARTMENT];
    int i;

    //	v1=player_school.faculty_promotion_difficulity;
    //	v2=player_school.faculty_degree_to_which_priorities_reflected_in_promotion;
    v1=(int)(department_array.teaching_weight_input*100);
    v2=(int)(department_array.scholarship_weight_input*100);
    v3=(int)(department_array.research_weight_input*100);
    v4=(int)(department_array.gender_ethnic_equity_weight_input*100);
    //------ paint the background -------//
    vga.use_back();

    for(i=0;i<MAX_DEPARTMENT;i++) {
	if( department_array.is_deleted(i+1)) {
	    data1[i]=0;
	    continue;
	}
	Department* dept_ptr = department_array[i+1];
	data1[i]=(int)(dept_ptr->dept_salary_multiplier*100);
    }
    for(i=0;i<7;i++)
	data2[i]=(int)(department_array.rank_age_group_salary_increase_multiplier[i]*100);

    static int data_num=HISTORY_YEAR_COUNT;
    static float dataArray1[HISTORY_YEAR_COUNT];
    static float dataArray2[HISTORY_YEAR_COUNT];
    static float dataArray3[HISTORY_YEAR_COUNT];
    for(i=0;i<HISTORY_YEAR_COUNT;++i) {
	dataArray1[i] = department_array.salary_departmental_equity_history[i] * 100.0f;
	dataArray2[i] = department_array.salary_gender_ethnic_equity_history[i] * 100.0f;
	dataArray3[i] = department_array.salary_rank_equity_history[i] * 100.0f;
    }

    if( refreshFlag == INFO_REPAINT ) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_report_deinit();
	report_init_flag = 1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);
	user_interface.brighten(tab1,vtab1,tab8,vtab2+3);
	user_interface.rect(tab1,vtab1,tab8,vtab2+3,1);
	//		user_interface.brighten(tab5,vtab1,tab6,vtab4);
	//		user_interface.rect(tab5,vtab1,tab6,vtab4);
	user_interface.brighten(tab1,vtab3-2,tab8,vtab4);
	user_interface.rect(tab1,vtab3-2,tab8,vtab4,1);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	font_charts.put(REPORT_X1+300,REPORT_Y1-7,"Faculty Salary Program");

	font_chartsm.put(ptab2,vtab3+7,"Teaching performance");
	font_chartsm.put(ptab2,vtab3+29,"Scholarly performance");
	font_chartsm.put(ptab4,vtab3+7,"Research performance");
	font_chartsm.put(ptab4,vtab3+29,"Gender and ethnic equity");

	//		mode_fac_sum_rb1.init(tab3,vtab3+19,tab4,vtab3+37,3,&v1,mode_fac_sum_rblabel,NULL);
	//		mode_fac_sum_rb2.init(tab3,vtab3+49,tab4,vtab3+67,3,&v2,mode_fac_sum_rblabel,NULL);
	//		mode_fac_sum_rb3.init(tab5,vtab3+19,tab6,vtab3+37,3,&v3,mode_fac_sum_rblabel,NULL);
	//		mode_fac_sum_rb4.init(tab5,vtab3+49,tab6,vtab3+67,3,&v4,mode_fac_sum_rblabel,NULL);

	mode_fac_sum_valueslider3[0].init(
	    ptab3+90,vtab3+5,
	    80,15,
	    50,350,&v1,1,V_YELLOW,10);
	mode_fac_sum_valueslider3[1].init(
	    ptab3+90,vtab3+27,
	    80,15,
	    50,350,&v2,1,V_YELLOW,10);
	mode_fac_sum_valueslider3[2].init(
	    ptab5+90,vtab3+5,
	    80,15,
	    50,350,&v3,1,V_YELLOW,10);
	mode_fac_sum_valueslider3[3].init(
	    ptab5+90,vtab3+27,
	    80,15,
	    50,350,&v4,1,V_YELLOW,10);

	//----- initialize the bottom sub-menus buttons -----//
	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;

	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");

	for (i = 0; i < 7; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (i = 7; i < 14; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);

	// show all departments
	user_interface.rect(tab1,vtab1,tab4+3,vtab1+40,1);
	user_interface.rect(tab1,vtab1,tab4+3,vtab2+3,1);
	user_interface.rect(tab4,vtab1,tab6,vtab1+40,1);
	user_interface.rect(tab4,vtab1,tab6,vtab2+3,1);
	user_interface.rect(tab6-3,vtab1,tab8,vtab1+40,1);
	user_interface.rect(tab6-3,vtab1,tab8,vtab2+3,1);

	font_charts.put(tab1+15,vtab1+12,"By department");
	font_chartsm.put_paragraph(tab4-100,vtab1+5,tab4,vtab1+45,"Departmental salary increase percentage",1);
	int line_spacing=(vtab2-vtab1-60)/department_array.size();
	for(int l=department_array.size() ; l>0 ; l-- ) {
	    if( department_array.is_deleted(l) )
		continue;
	    Department* dept_ptr = department_array[l];
	    String tmp;
	    tmp=dept_ptr->name();
	    tmp.str_buf[tmp.len()-10]='\0';
	    font_chartsm.put(tab1+15,vtab1+30+(int)(line_spacing*(0.2+l)),tmp.str_buf);
	    //			mode_fac_sum_data3[l-1].put(tab1+280,vtab1+32+line_spacing*l,&(data1[l-1]));
	    //			mode_fac_sum_valueslider[l-1].init(
	    //					tab1+245,vtab1+29+(int)(line_spacing*(0.2+l)),
	    //					90,line_spacing-6,
	    //					50,250,data1+l-1,1,V_YELLOW,10);
	    // changed in ver2
	    mode_fac_sum_valueslider[l-1].init(
		tab4-100,vtab1+29+(int)(line_spacing*(0.2+l)),
		90,line_spacing-6,
		50,250,data1+l-1,1,V_YELLOW,10);
	}
	font_charts.put_paragraph(tab4+15,vtab1+5,tab6-125,vtab1+45,"By rank and experience");
	font_chartsm.put_paragraph(tab6-125,vtab1+5,tab6,vtab1+45,"Rank/experience group salary increase percentage",1);
	font_chartsm.put(tab4+15,vtab1+52,"Assistant professor");
	font_chartsm.put(tab4+15,vtab1+82,"Associate professor");
	font_chartsm.put(tab4+15,vtab1+112,"Full professor-1");
	font_chartsm.put(tab4+15,vtab1+142,"Full professor-2");
	font_chartsm.put(tab4+15,vtab1+172,"Full professor-3");
	font_chartsm.put(tab4+15,vtab1+202,"Long-term adjunct");
	font_chartsm.put(tab4+15,vtab1+232,"Short-term adjunct");
	for(i=0;i<7;i++)
	    //			mode_fac_sum_valueslider2[i].init(
	    //					tab4+235,vtab1+51+30*i,
	    //					90,18,
	    //					50,250,data2+i,1,V_YELLOW,10);
	    mode_fac_sum_valueslider2[i].init(
		tab6-100,vtab1+51+30*i,
		90,18,
		50,250,data2+i,1,V_YELLOW,10);

	font_charts.put_paragraph(tab6+12,vtab1+5,tab8-3, vtab1+45, "Salary variation indices");
	font_chartsm.put(tab6+12, vtab1+52, "Departments" );
	department_equity_graph.init(
	    tab6+52,vtab1+52+16,tab8-15,vtab1+123-6,
	    1 ,&data_num , dataArray1, 1, 0, 3);
	department_equity_graph.paint();
	font_chart_purple_sm.put(tab6+12,vtab1+52+32,m.format(department_array.initial_salary_departmental_equity,23) );

	font_chartsm.put(tab6+12, vtab1+123, "Gender/ethnic groups" );
	gender_ethnic_equity_graph.init(
	    tab6+52,vtab1+123+16,tab8-15,vtab1+194-6,
	    1 ,&data_num , dataArray2, 1, 0, 3);
	gender_ethnic_equity_graph.paint();
	font_chart_purple_sm.put(tab6+12,vtab1+123+32,m.format(department_array.initial_salary_gender_ethnic_equity,23) );

	font_chartsm.put(tab6+12, vtab1+194, "Ranks" );
	rank_equity_graph.init(
	    tab6+52,vtab1+194+16,tab8-15,vtab1+285-20,
	    1 ,&data_num , dataArray3, 1, 0, 3);
	rank_equity_graph.paint();
	font_chart_purple_sm.put(tab6+12,vtab1+194+32,m.format(department_array.initial_salary_rank_equity,23) );
    }

    //--------------------------------------------//

    for(i=0;i<department_array.size();i++)
	mode_fac_sum_valueslider[i].refresh();

    for(i=0;i<7;i++)
	mode_fac_sum_valueslider2[i].refresh();

    for(i=0;i<4;i++)
	mode_fac_sum_valueslider3[i].refresh();

    department_equity_graph.refresh();
    gender_ethnic_equity_graph.refresh();
    rank_equity_graph.refresh();

    department_equity_text.put( tab6+12, vtab1+52+16, department_array.salary_departmental_equity_history[THIS_YEAR], 23 );
    gender_ethnic_equity_text.put( tab6+12, vtab1+123+16, department_array.salary_gender_ethnic_equity_history[THIS_YEAR], 23 );
    rank_equity_text.put( tab6+12, vtab1+194+16, department_array.salary_rank_equity_history[THIS_YEAR], 23 );

    //----------- paint the summary button group ---------//
    type_button_group.paint(current_report_type);

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

}

//----- End of Summary Report 7 ------//

//----- Begin of Summary Report 8 ------//
void FacultyRes::summary_report_8(int refreshFlag) {
    int tab1=REPORT_X1+100;
    int tab2=REPORT_X1+130;
    int tab3=REPORT_X1+300;
    int tab4=REPORT_X1+640;
    int vtab1=REPORT_Y1+100;
    int vtab2=REPORT_Y1+250;
    static int v1,v2;

    v1=player_school.faculty_promotion_difficulity;
    v2=player_school.faculty_degree_to_which_priorities_reflected_in_promotion;
    //------ paint the background -------//
    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_report_deinit();
	report_init_flag = 1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);
	user_interface.brighten(tab1,vtab1,tab4,vtab2);
	user_interface.rect(tab1,vtab1,tab4,vtab2,1);
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	font_charts.put(REPORT_X1+290,REPORT_Y1-7,"Promotion and Tenure");

	font_chartsm.center_put(tab2+10,vtab1+30,tab4,vtab1+48,"Degree to which salary-setting priorities are reflected in promotion");
	font_chartsm.center_put(tab2+30,vtab1+85,tab4,vtab1+103,"Difficulty of promotion to tenure");
	mode_fac_sum_rb1.init(tab2+100,vtab1+45,tab4,vtab1+72,3,&v1,mode_fac_sum_rblabel,NULL);
	mode_fac_sum_rb2.init(tab2+100,vtab1+100,tab4,vtab1+127,3,&v2,mode_fac_sum_rblabel,NULL);

	//----- initialize the bottom sub-menus buttons -----//
	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;

	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");

	for (int i = 0; i < 7; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (int i = 7; i < 14; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
		type_button_group[i].create_text(x, y,x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,reportTypeStrArray[i]);
    }
    else {
	mode_fac_sum_rb1.refresh();
	mode_fac_sum_rb2.refresh();
    }
    //----------- paint the summary button group ---------//
    type_button_group.paint(current_report_type);

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

}

//----- End of Summary Report 8 ------//

// --- Begin of FacultyRes::summary_report_9 --- //
//
void FacultyRes::summary_report_9(int refreshFlag) {
    //------ paint the background -------//
    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_report_deinit();
	report_init_flag = 1;

	user_interface.bg_img(BG_PIC_ID, &vga_back);

	// top title
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,1);
	if ( is_year_end_report ) {
	    font_charts.center_put(REPORT_X1+100,REPORT_Y1-13,REPORT_X1+630,REPORT_Y1+10,
				   "Faculty Changes & Adjustments Report" );

	    int totalDept = 0;
	    for (int i=1;i<=department_array.size();i++)
		if (!department_array.is_deleted(i))
		    totalDept++;

	    // Paint the text "New hires will not be determined until the new budget has been set."
	    font_charts.put( REPORT_X1+TAB1, BUTTON_Y1-16,
			     "New hires wil not be determined until the new budget has been set." );
	}
	else
	    font_charts.put(REPORT_X1+240,REPORT_Y1-7,fac_sum_title[2]);

	if ( is_year_end_report ) {
	    // Paint "Back" Button
	    mode_fac_hire_button.create_text(REPORT_X1+270, BUTTON_Y1,
					     REPORT_X1+390, BUTTON_Y1+BUTTON_HEIGHT,"Back",1);

	    // Paint "Next" Button
	    char buttonStr[123];
	    strcpy( buttonStr, "Next" );
	    mode_fac_next_button.create_text(REPORT_X1+410, BUTTON_Y1,
					     REPORT_X1+530, BUTTON_Y1+BUTTON_HEIGHT,buttonStr,1);
	}
	else {
	    // Paint "Back" Button
	    mode_fac_hire_button.create_text(REPORT_X1+320, BUTTON_Y1,
					     REPORT_X1+440, BUTTON_Y1+BUTTON_HEIGHT,"Back",1);
	}

	// Display Table
	if (current_report_mode!=REPORT_MODE_INSTITUTION && !is_year_end_report) {
	    // Show Brighten Part
	    user_interface.brighten(
		REPORT_X1+TAB1+50,
		REPORT_Y1+VTAB1+10,
		REPORT_X1+TAB2+TAB7*5+50,
		REPORT_Y1+VTAB1+10+8*VTAB6+50
		);

	    // Leftmost column
	    user_interface.rect(
		REPORT_X1+TAB1+50,
		REPORT_Y1+VTAB1+10,
		REPORT_X1+TAB2+50,
		REPORT_Y1+VTAB1+10+8*VTAB6+50,1
		);

	    // Draw Columns
	    for ( int i=0; i<5; i++)
		user_interface.rect(
		    REPORT_X1+TAB2-2+i*TAB7+50,
		    REPORT_Y1+VTAB1+10,
		    REPORT_X1+TAB2+TAB7*(i+1)+50,
		    REPORT_Y1+VTAB1+10+8*VTAB6+50,1);

	    // Draw Rows
	    for( int i=0;i<8; i++)
		user_interface.rect(
		    REPORT_X1+TAB1+50,
		    REPORT_Y1+VTAB1+10+i*VTAB6+50,
		    REPORT_X1+TAB2+TAB7*5+50,
		    REPORT_Y1+VTAB1+10+(i+1)*VTAB6+50,1
		    );

	    // Print titles of Table
	    font_chartsm.center_put(
		REPORT_X1+TAB1+50,
		REPORT_Y1+VTAB1+10+50-20,
		REPORT_X1+TAB2-2+50,
		REPORT_Y1+VTAB1+10+50,
		hire_report_label4[0]);

	    for ( int i=0; i<5; i++)
		font_chartsm.center_put(
		    REPORT_X1+TAB2+i*TAB7+50,
		    REPORT_Y1+VTAB1+10+15,
		    REPORT_X1+TAB2-2+(i+1)*TAB7+50,
		    REPORT_Y1+VTAB1+10+30,
		    hire_report_label3[i+1]);

	    for ( int i=0; i<5; i++)
		font_chartsm.center_put(
		    REPORT_X1+TAB2+i*TAB7+50,
		    REPORT_Y1+VTAB1+10+30,
		    REPORT_X1+TAB2-2+(i+1)*TAB7+50,
		    REPORT_Y1+VTAB1+10+50,
		    hire_report_label4[i+1]);
	}
	else {
	    int totalDept = 0;
	    for (int i=1;i<=department_array.size();i++) {
		if (!department_array.is_deleted(i))
		    totalDept++;
	    }

	    // Show Brighten Part
	    user_interface.brighten(
		REPORT_X1+TAB1,
		REPORT_Y1+VTAB1+10,
		REPORT_X1+TAB2+TAB7*6,
		REPORT_Y1+VTAB1+10+totalDept*VTAB6+50
		);

	    user_interface.rect(
		REPORT_X1+TAB1,
		REPORT_Y1+VTAB1+10,
		REPORT_X1+TAB2,
		REPORT_Y1+VTAB1+10+totalDept*VTAB6+50,1
		);

	    for ( int i=0; i<6; i++)
		user_interface.rect(
		    REPORT_X1+TAB2-2+i*TAB7,
		    REPORT_Y1+VTAB1+10,
		    REPORT_X1+TAB2+TAB7*(i+1),
		    REPORT_Y1+VTAB1+10+totalDept*VTAB6+50,1);

	    for( int i=0;i<totalDept; i++)
		user_interface.rect(
		    REPORT_X1+TAB1,
		    REPORT_Y1+VTAB1+10+i*VTAB6+50,
		    REPORT_X1+TAB2+TAB7*6,
		    REPORT_Y1+VTAB1+10+(i+1)*VTAB6+50,1
		    );

	    // Print titles of Table
	    font_chartsm.center_put(
		REPORT_X1+TAB1,
		REPORT_Y1+VTAB1+10+50-20,
		REPORT_X1+TAB2-2,
		REPORT_Y1+VTAB1+10+50,
		hire_report_label2[0]);

	    for ( int i=0; i<6; i++)
		font_chartsm.center_put(
		    REPORT_X1+TAB2+i*TAB7,
		    REPORT_Y1+VTAB1+10+15,
		    REPORT_X1+TAB2-2+(i+1)*TAB7,
		    REPORT_Y1+VTAB1+10+30,
		    hire_report_label1[i+1]);

	    for ( int i=0; i<6; i++)
		font_chartsm.center_put(
		    REPORT_X1+TAB2+i*TAB7,
		    REPORT_Y1+VTAB1+10+30,
		    REPORT_X1+TAB2-2+(i+1)*TAB7,
		    REPORT_Y1+VTAB1+10+50,
		    hire_report_label2[i+1]);
	}

	if ( (current_report_mode==REPORT_MODE_INSTITUTION) || (is_year_end_report) ) {
	    int row = 1;
	    show_department_detail_count = 0;
	    for ( int j=1; j<=department_array.size(); j++ ) {
		show_department_detail_count++;
		if ( department_array.is_deleted(j) )
		    continue;
		String deptStr;
		cur_dept_ptr = department_array[j];
		deptStr = cur_dept_ptr->name();
		String deptStrTmp=deptStr.substr(0,deptStr.len()-11);

		// Display details
		// Department Name
		font_chartsm.center_put(
		    REPORT_X1+TAB1,
		    REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
		    REPORT_X1+TAB2-2,
		    REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
		    deptStrTmp);

		// Prior year total
		if ( is_year_end_report )
		    font_chartsm.center_put(
			REPORT_X1+TAB2,
			REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
			REPORT_X1+TAB2+TAB7,
			REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
			m.format(cur_dept_ptr->faculty_level_history[5][7]));
		else
		    font_chartsm.center_put(
			REPORT_X1+TAB2,
			REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
			REPORT_X1+TAB2+TAB7,
			REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
			m.format(cur_dept_ptr->faculty_level_history[5][6]));

		// Departures
		font_chartsm.center_put(
		    REPORT_X1+TAB2+TAB7,
		    REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
		    REPORT_X1+TAB2+TAB7*2-2,
		    REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
		    m.format(cur_dept_ptr->faculty_departures_number()));

		// New hires
		if ( is_year_end_report ) {
		    font_chartsm.center_put(
			REPORT_X1+TAB2+TAB7*2,
			REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
			REPORT_X1+TAB2+TAB7*3-2,
			REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
			"na");
		}
		else {
		    font_chartsm.center_put(
			REPORT_X1+TAB2+TAB7*2,
			REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
			REPORT_X1+TAB2+TAB7*3-2,
			REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
			m.format(cur_dept_ptr->faculty_new_hires_number()));
		}

		// Promotion to tenure
		font_chartsm.center_put(
		    REPORT_X1+TAB2+TAB7*3,
		    REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
		    REPORT_X1+TAB2+TAB7*4-2,
		    REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
		    m.format(cur_dept_ptr->faculty_promotions_number()));

		// Current year total
		if ( is_year_end_report ) {
		    font_chartsm.center_put(
			REPORT_X1+TAB2+TAB7*4,
			REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
			REPORT_X1+TAB2+TAB7*5-2,
			REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
			m.format(cur_dept_ptr->faculty_level_history[5][7]-cur_dept_ptr->faculty_departures_number()) );
		}
		else {
		    font_chartsm.center_put(
			REPORT_X1+TAB2+TAB7*4,
			REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
			REPORT_X1+TAB2+TAB7*5-2,
			REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
			m.format(cur_dept_ptr->faculty_level_history[5][7]));
		}

		// Click for details
		mode_fac_detail_button[row-1].create_text(
		    REPORT_X1+TAB2+TAB7*5-3,
		    REPORT_Y1+VTAB1+10+50+(row-1)*VTAB6-1,
		    REPORT_X1+TAB2+TAB7*6-1,
		    REPORT_Y1+VTAB1+10+50+VTAB6+(row-1)*VTAB6+1,
		    "Detail",1);

		row++;
	    }
	}
	else {                                        // show just one department detail
	    show_department_detail_count = 1;
	    int deptRecno = 1;
	    if( department_array.selected_recno )
		deptRecno = department_array.selected_recno;
	    cur_dept_ptr = department_array[deptRecno];

	    // Display details
	    // Department Year
	    /*			String deptName="\0";
				deptName = cur_dept_ptr->name();
				deptName = deptName.substr(0,deptName.len()-11);
				strcat(deptName, "\0");
				font_chartsm.center_put(
				REPORT_X1+TAB1,
				REPORT_Y1+VTAB1+30+50+5,
				REPORT_X1+TAB2-2,
				REPORT_Y1+VTAB1+30+50+VTAB6-2,
				deptName);
	    */

	    // find the start year
	    int startYear;

	    if ( info.financial_year() > HISTORY_YEAR_COUNT ) {
		startYear = info.financial_year() - HISTORY_YEAR_COUNT + 1;
	    }
	    else
		startYear = 1;

	    // find the end year
	    int endYear = info.financial_year();

	    int row=1;

	    for ( int i=startYear; i<=endYear; i++ ) {
		font_chartsm.center_put(
		    REPORT_X1+TAB1+50,
		    REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
		    REPORT_X1+TAB2-2+50,
		    REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
		    m.format( i ) );

		int yearNo = (THIS_YEAR-endYear%HISTORY_YEAR_COUNT+1+i-1)%HISTORY_YEAR_COUNT;

		// get the display departures, hires, promotion numbers
		int priorNo, departureNo, hireNo, promotionNo;
		if ( yearNo == THIS_YEAR ) {
		    departureNo = cur_dept_ptr->faculty_departures_number();
		    hireNo = cur_dept_ptr->faculty_new_hires_number();
		    promotionNo = cur_dept_ptr->faculty_promotions_number();
		}
		else {
		    departureNo = cur_dept_ptr->departures_history[yearNo];
		    hireNo      = cur_dept_ptr->hires_history[yearNo];
		    promotionNo = cur_dept_ptr->promotion_history[yearNo];
		}

		if ( yearNo == 0 )
		    priorNo = cur_dept_ptr->faculty_level_history[5][yearNo] - hireNo + departureNo;
		else
		    priorNo = cur_dept_ptr->faculty_level_history[5][yearNo-1];

		// Prior year total
		font_chartsm.center_put(
		    REPORT_X1+TAB2+50,
		    REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
		    REPORT_X1+TAB2+TAB7+50,
		    REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
		    m.format( priorNo ));

		// Departures
		font_chartsm.center_put(
		    REPORT_X1+TAB2+TAB7+50,
		    REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
		    REPORT_X1+TAB2+TAB7*2-2+50,
		    REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
		    m.format(departureNo));

		// New hires
		font_chartsm.center_put(
		    REPORT_X1+TAB2+TAB7*2+50,
		    REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
		    REPORT_X1+TAB2+TAB7*3-2+50,
		    REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
		    m.format(hireNo));

		// Promotion to tenure
		font_chartsm.center_put(
		    REPORT_X1+TAB2+TAB7*3+50,
		    REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
		    REPORT_X1+TAB2+TAB7*4-2+50,
		    REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
		    m.format(promotionNo));

		// Current year total
		font_chartsm.center_put(
		    REPORT_X1+TAB2+TAB7*4+50,
		    REPORT_Y1+VTAB1+10+50+5+(row-1)*VTAB6,
		    REPORT_X1+TAB2+TAB7*5-2+50,
		    REPORT_Y1+VTAB1+10+50+VTAB6-2+(row-1)*VTAB6,
		    m.format(cur_dept_ptr->faculty_level_history[5][yearNo]));

		// Click to detail
		mode_fac_detail_button[0].create_text(
		    REPORT_X1+TAB2+TAB7*5-3+50,
		    REPORT_Y1+VTAB1+10+50-1+(row-1)*VTAB6,
		    REPORT_X1+TAB2+TAB7*6-1+35,
		    REPORT_Y1+VTAB1+10+50+VTAB6+1+(row-1)*VTAB6,
		    "Detail",1);

		row ++;
	    }
	}

	report_init_flag = 1;
    }

    //----------- paint the summary button group ---------//
    mode_fac_hire_button.paint();
    if (is_year_end_report)
	mode_fac_next_button.paint();
    for (int i=0; i<show_department_detail_count; i++)
	mode_fac_detail_button[i].paint();

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    //##trevor 2807
    vga.use_front();
    //##trevor 2807

}

//
// --- End of FacultyRes::summer_report_9 --- //

// --- Begin of FacultyRes::summary_report_10 --- //
//
// It is used to show details of Faculty Hires, Promotions and Departures
void FacultyRes::summary_report_10(int refreshFlag) {
    int tab1=REPORT_X1+242;
    int vtab1=REPORT_Y1+33;

    report_faculty_hired(refreshFlag);
    if( refreshFlag == INFO_REPAINT ) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    summary_report_deinit();
	report_init_flag = 1;
	mode_fac_sum_button.create_text(  REPORT_X1+260, REPORT_Y2+10,
					  REPORT_X1+350, REPORT_Y2+30,"Back",1);
    }
}

//
// --- End of FacultyRes::summary_report_10 --- //

//----- Begin of function FacultySummary::report ------//
//!
//! <int> refreshFlag = INFO_REPAINT or INFO_UPDATE
//!
void FacultyRes::summary_report(int refreshFlag) {
    if(current_report_type==REPORT_TYPE_DEPARTMENT_PROFILE1) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_report_1(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_DEPARTMENT_PROFILE2) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_report_2(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_DEPARTMENT_ACTIVITIES) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_report_3(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_DEPARTMENT_RESEARCH) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_report_4(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_DEPARTMENT_DIRECTORY) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_report_5(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_DEPARTMENT_HIRING) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_report_6(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_INSTITUTION_PROFILE1) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_report_1(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_INSTITUTION_PROFILE2) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_report_2(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_INSTITUTION_ACTIVITIES) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_report_3(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_INSTITUTION_RESEARCH) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_report_4(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_INSTITUTION_PROMOTION) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_report_8(refreshFlag);
    }
    else if(current_report_type==REPORT_TYPE_INSTITUTION_SALARY) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_report_7(refreshFlag);
    }
    else if (current_report_type==REPORT_TYPE_DEPARTMENT_TURNOVER) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_report_9(refreshFlag);
    }
    else if (current_report_type==REPORT_TYPE_INSTITUTION_TURNOVER) {
	sys.is_institution_report=1;
	current_report_mode=REPORT_MODE_INSTITUTION;
	summary_report_9(refreshFlag);
    }
    else if (current_report_type==REPORT_TYPE_DEPARTMENT_TURNOVER_DETAIL) {
	sys.is_institution_report=0;
	current_report_mode=REPORT_MODE_DEPARTMENT;
	summary_report_10(refreshFlag);
    }
}

//------- End of function FacultySummary::report ------//

//----- Begin of function FacultySummary::report_detect ------//
//!
int FacultyRes::summary_report_detect() {
    vga.use_front();

    if ( is_year_end_report ) {
	if ( current_year_report==FACULTY_LISTS ) {
	    if ( mode_fac_hire_button.detect() ) {
		mode_fac_hire_button.push();
		sys.redraw_all_flag = 1;
		return 2;                                 // BACK
	    }
	    else
		if ( mode_fac_next_button.detect() ) {
		    mode_fac_next_button.push();
		    sys.redraw_all_flag = 1;
		    return 3;                                 // NEXT
		}
		else {
		    for ( int i=0; i<show_department_detail_count; i++ ) {
			if ( mode_fac_detail_button[i].detect() ) {
			    // call detail report
			    mode_fac_detail_button[i].push();
			    sys.redraw_all_flag = 1;
			    department_array.selected_recno=i+1;
			    return 1;
			}
		    }
		}
	}
	else
	    if ( report_hired_detect() ) {
		return 1;
	    }

	return 0;
    }

    if((current_report_type==REPORT_TYPE_DEPARTMENT_TURNOVER)||(current_report_type==REPORT_TYPE_INSTITUTION_TURNOVER)) {
	if ( mode_fac_hire_button.detect() ) {
	    mode_fac_hire_button.push();
	    current_report_type = REPORT_TYPE_INSTITUTION_PROFILE1;
	    current_report_mode = REPORT_MODE_INSTITUTION;
	    sys.redraw_all_flag = 1;
	    return 1;
	}
	else {
	    for ( int i=0; i<show_department_detail_count; i++ ) {
		if ( mode_fac_detail_button[i].detect() ) {
		    // call detail report
		    mode_fac_detail_button[i].push();
		    if( current_report_type == REPORT_TYPE_INSTITUTION_TURNOVER )
			department_array.selected_recno = i+1;
		    last_report_type = current_report_type;
		    current_report_type = REPORT_TYPE_DEPARTMENT_TURNOVER_DETAIL;
		    current_report_mode = REPORT_MODE_DEPARTMENT;
		    sys.redraw_all_flag = 1;
		    //					department_array.selected_recno=i+1;
		    return 1;
		}
	    }
	}
	vga.use_back();
    }

    if((current_report_type!=REPORT_TYPE_DEPARTMENT_TURNOVER)&&(current_report_type!=REPORT_TYPE_INSTITUTION_TURNOVER)
       &&(current_report_type!=REPORT_TYPE_DEPARTMENT_TURNOVER_DETAIL))

	if(current_report_type != REPORT_TYPE_DEPARTMENT_DIRECTORY) {
	    // pre-set the fac_rep 's page to the list
	    report_type=0;                                // REPORT_TYPE_LIST;

	    //------- detect type buttons --------//
	    int summaryButtonGroupDetect;

	    if((summaryButtonGroupDetect = type_button_group.detect()) >= 0) {
		//			type_button_group[current_report_type].body_ptr = (void *) (type_button_up_bitmap[current_report_type] + 4);
		current_report_type = summaryButtonGroupDetect;
		//			type_button_group[current_report_type].body_ptr = (void *) (type_button_down_bitmap[current_report_type] + 4);
		sys.redraw_all_flag = 1;
		return 1;
	    }
	}

    if ( current_report_type == REPORT_TYPE_DEPARTMENT_TURNOVER_DETAIL )
	report_hired_detect();

    if(current_report_type == REPORT_TYPE_DEPARTMENT_DIRECTORY) {
	report_detect();
	if(faculty_res.report_type!=REPORT_RETIRE)
	    if(mode_fac_sum_button.detect()) {
		mode_fac_sum_button.push();
		current_report_type = REPORT_TYPE_INSTITUTION_PROFILE1;
		current_report_mode = REPORT_MODE_INSTITUTION;
		sys.redraw_all_flag = 1;
		return 1;
	    }
	vga.use_back();
    }

    if(current_report_type == REPORT_TYPE_INSTITUTION_PROMOTION) {
	help.set_help(REPORT_X1+130,REPORT_Y1+130,REPORT_X1+640,REPORT_Y1+170,"H1.4.1.");
	help.set_help(REPORT_X1+130,REPORT_Y1+180,REPORT_X1+640,REPORT_Y1+220,"H1.4.2.");
	if (mode_fac_sum_rb1.detect()) {
	    player_school.faculty_promotion_difficulity
		=*(mode_fac_sum_rb1.sel_num);             //BUGHERE
	    return 1;
	}
	if (mode_fac_sum_rb2.detect()) {
	    player_school.faculty_degree_to_which_priorities_reflected_in_promotion
		=*(mode_fac_sum_rb2.sel_num);             //BUGHERE
	    return 1;
	}
    }

    if(current_report_type == REPORT_TYPE_INSTITUTION_SALARY) {
	help.set_help(REPORT_X1+270-100,REPORT_Y1+20,REPORT_X1+270-10,REPORT_Y1+290,"H1.2.1.");
	help.set_help(REPORT_X1+555-100,REPORT_Y1+20,REPORT_X1+555-10,REPORT_Y1+290,"H1.2.2.");
	help.set_help(REPORT_X1+15,REPORT_Y1+295,REPORT_X1+260,REPORT_Y1+318,"H1.2.3.");
	help.set_help(REPORT_X1+15,REPORT_Y1+318,REPORT_X1+260,REPORT_Y1+341,"H1.2.4.");
	help.set_help(REPORT_X1+350,REPORT_Y1+295,REPORT_X1+600,REPORT_Y1+318,"H1.2.5.");
	help.set_help(REPORT_X1+350,REPORT_Y1+318,REPORT_X1+600,REPORT_Y1+341,"H1.2.6.");

	/*		if (mode_fac_sum_rb1.detect())
			{	department_array.teaching_weight_input
			=*(mode_fac_sum_rb1.sel_num);		//BUGHERE
			return 1;
			}
			if (mode_fac_sum_rb2.detect())
			{	department_array.scholarship_weight_input
			=*(mode_fac_sum_rb2.sel_num);		//BUGHERE
			return 1;
			}
			if (mode_fac_sum_rb3.detect())
			{	department_array.research_weight_input
			=*(mode_fac_sum_rb3.sel_num);		//BUGHERE
			return 1;
			}
			if (mode_fac_sum_rb4.detect())
			{	department_array.gender_ethnic_equity_weight_input
			=*(mode_fac_sum_rb4.sel_num);		//BUGHERE
			return 1;
			}
	*/

	vga.use_back();
	for(int i=0;i<department_array.size();i++)
	    if(mode_fac_sum_valueslider[i].detect()) {
		if( department_array.is_deleted(i+1)) {
		    continue;
		}
		Department* dept_ptr = department_array[i+1];
		dept_ptr->dept_salary_multiplier=(float)(((double)*(mode_fac_sum_valueslider[i].var_ptr))/100.0);
		return 1;
	    }
	for(int i=0;i<7;i++)
	    if(mode_fac_sum_valueslider2[i].detect()) {
		department_array.rank_age_group_salary_increase_multiplier[i]=(float)(((double)*(mode_fac_sum_valueslider2[i].var_ptr))/100.0);
		return 1;
	    }
	if(mode_fac_sum_valueslider3[0].detect()) {
	    department_array.teaching_weight_input=((float)*(mode_fac_sum_valueslider3[0].var_ptr))/100;
	    return 1;
	}
	if(mode_fac_sum_valueslider3[1].detect()) {
	    department_array.scholarship_weight_input=((float)*(mode_fac_sum_valueslider3[1].var_ptr))/100;
	    return 1;
	}
	if(mode_fac_sum_valueslider3[2].detect()) {
	    department_array.research_weight_input=((float)*(mode_fac_sum_valueslider3[2].var_ptr))/100;
	    return 1;
	}
	if(mode_fac_sum_valueslider3[3].detect()) {
	    department_array.gender_ethnic_equity_weight_input=((float)*(mode_fac_sum_valueslider3[3].var_ptr))/100;
	    return 1;
	}
    }

    if(current_report_type == REPORT_TYPE_INSTITUTION_ACTIVITIES) {
	help.set_help(REPORT_X1,REPORT_Y1+105,REPORT_X1+320,REPORT_Y1+160,"H1.1.1.");
	help.set_help(REPORT_X1,REPORT_Y1+165,REPORT_X1+320,REPORT_Y1+220,"H1.1.2.");
	help.set_help(REPORT_X1,REPORT_Y1+225,REPORT_X1+320,REPORT_Y1+280,"H1.1.3.");
	help.set_help(REPORT_X1,REPORT_Y1+285,REPORT_X1+320,REPORT_Y1+330,"H1.1.4.");
	help.set_help(REPORT_X1+340,REPORT_Y1+105,REPORT_X1+660,REPORT_Y1+160,"H1.1.5.");
	help.set_help(REPORT_X1+340,REPORT_Y1+165,REPORT_X1+660,REPORT_Y1+220,"H1.1.6.");
	help.set_help(REPORT_X1+340,REPORT_Y1+225,REPORT_X1+660,REPORT_Y1+280,"H1.1.7.");
	help.set_help(REPORT_X1+340,REPORT_Y1+285,REPORT_X1+660,REPORT_Y1+330,"H1.1.8.");
    }

    if(current_report_type == REPORT_TYPE_DEPARTMENT_ACTIVITIES) {
	help.set_help(REPORT_X1,REPORT_Y1+105,REPORT_X1+320,REPORT_Y1+160,"H1.1.1.");
	help.set_help(REPORT_X1,REPORT_Y1+165,REPORT_X1+320,REPORT_Y1+220,"H1.1.2.");
	help.set_help(REPORT_X1,REPORT_Y1+225,REPORT_X1+320,REPORT_Y1+280,"H1.1.3.");
	help.set_help(REPORT_X1,REPORT_Y1+285,REPORT_X1+320,REPORT_Y1+330,"H1.1.4.");
	help.set_help(REPORT_X1+340,REPORT_Y1+105,REPORT_X1+660,REPORT_Y1+160,"H1.1.5.");
	help.set_help(REPORT_X1+340,REPORT_Y1+165,REPORT_X1+660,REPORT_Y1+220,"H1.1.6.");
	help.set_help(REPORT_X1+340,REPORT_Y1+225,REPORT_X1+660,REPORT_Y1+280,"H1.1.7.");
	help.set_help(REPORT_X1+340,REPORT_Y1+285,REPORT_X1+660,REPORT_Y1+330,"H1.1.8.");

	vga.use_back();
	if(mode_fac_sum_valueslider[0].detect()) {
	    int deptRecno;
	    if( department_array.selected_recno )
		deptRecno = department_array.selected_recno;
	    department_array[deptRecno]->pressure_to_change_teaching_load
		=(*(mode_fac_sum_valueslider[0].var_ptr));
	    return 1;
	}

	for(int i=0;i<6;i++)
	    if(mode_fac_sum_valueslider[i+1].detect()) {
		int deptRecno;
		if( department_array.selected_recno )
		    deptRecno = department_array.selected_recno;
		department_array[deptRecno]->priority_discretionary_hour_array[i]
		    =(*(mode_fac_sum_valueslider[i+1].var_ptr));
		return 1;
	    }
    }
    if(current_report_type==REPORT_TYPE_DEPARTMENT_RESEARCH) {
	for(int i=0;i<2;i++)
	    if(mode_fac_sum_buttongroup[i].detect()) {
		current_graph_mode=i;
		mode_fac_sum_buttongroup[i].push();
		mode_fac_sum_buttongroup[1-i].pop();
		sys.redraw_all_flag = 1;
		return 1;
	    }
    }
    if(current_report_type==REPORT_TYPE_INSTITUTION_RESEARCH) {
	for(int i=0;i<2;i++)
	    if(mode_fac_sum_buttongroup[i].detect()) {
		current_graph_mode=i;
		mode_fac_sum_buttongroup[i].push();
		mode_fac_sum_buttongroup[1-i].pop();
		sys.redraw_all_flag = 1;
		return 1;
	    }
    }
    if(current_report_type == REPORT_TYPE_DEPARTMENT_HIRING) {
	help.set_help(REPORT_X1,REPORT_Y1+22+25,REPORT_X1+720,REPORT_Y1+42+25,"H1.3.1.");
	help.set_help(REPORT_X1,REPORT_Y1+47+25,REPORT_X1+720,REPORT_Y1+67+25,"H1.3.2.");
	help.set_help(REPORT_X1,REPORT_Y1+72+25,REPORT_X1+720,REPORT_Y1+92+25,"H1.3.3.");
	help.set_help(REPORT_X1,REPORT_Y1+102+25,REPORT_X1+720,REPORT_Y1+122+25,"H1.3.4.");
	help.set_help(REPORT_X1,REPORT_Y1+127+25,REPORT_X1+720,REPORT_Y1+147+25,"H1.3.5.");
	help.set_help(REPORT_X1,REPORT_Y1+157+25,REPORT_X1+720,REPORT_Y1+177+25,"H1.3.6.");
	help.set_help(REPORT_X1,REPORT_Y1+182+25,REPORT_X1+720,REPORT_Y1+202+25,"H1.3.7.");
	help.set_help(REPORT_X1,REPORT_Y1+212+25,REPORT_X1+720,REPORT_Y1+232+25,"H1.3.8.");
	help.set_help(REPORT_X1,REPORT_Y1+237+25,REPORT_X1+720,REPORT_Y1+257+25,"H1.3.9.");
	help.set_help(REPORT_X1,REPORT_Y1+262+25,REPORT_X1+720,REPORT_Y1+282+25,"H1.3.10.");
	//		help.set_help(REPORT_X1+285,REPORT_Y1+290,REPORT_X1+485,REPORT_Y1+310,"H1.3.11.");
	if (mode_fac_sum_rb1.detect()) {
	    //BUGHERE
	    cur_dept_ptr->assistant_professor_priority=(char)*(mode_fac_sum_rb1.sel_num);
	    return 1;
	}
	if (mode_fac_sum_rb2.detect()) {
	    //BUGHERE
	    cur_dept_ptr->tenured_faculty_priority=(char)*(mode_fac_sum_rb2.sel_num);
	    return 1;
	}
	if (mode_fac_sum_rb3.detect()) {
	    //BUGHERE
	    cur_dept_ptr->cost_containment_priority=(char)*(mode_fac_sum_rb3.sel_num);
	    return 1;
	}
	if (mode_fac_sum_rb4.detect()) {
	    //BUGHERE
	    cur_dept_ptr->longterm_adjunct_faculty_priority=(char)*(mode_fac_sum_rb4.sel_num);
	    return 1;
	}
	if (mode_fac_sum_rb5.detect()) {
	    //BUGHERE
	    cur_dept_ptr->shortterm_adjunct_faculty_priority=(char)*(mode_fac_sum_rb5.sel_num);
	    return 1;
	}
	if (mode_fac_sum_rb6.detect()) {
	    //BUGHERE
	    cur_dept_ptr->female_faculty_priority=(char)*(mode_fac_sum_rb6.sel_num);
	    return 1;
	}
	if (mode_fac_sum_rb7.detect()) {
	    //BUGHERE
	    cur_dept_ptr->minority_faculty_priority=(char)*(mode_fac_sum_rb7.sel_num);
	    return 1;
	}

	if (mode_fac_sum_rb8.detect()) {
	    //BUGHERE
	    cur_dept_ptr->teaching_talent_priority=(char)*(mode_fac_sum_rb8.sel_num);
	    return 1;
	}
	if (mode_fac_sum_rb9.detect()) {
	    //BUGHERE
	    cur_dept_ptr->scholarship_talent_priority=(char)*(mode_fac_sum_rb9.sel_num);
	    return 1;
	}
	if (mode_fac_sum_rb10.detect()) {
	    //BUGHERE
	    cur_dept_ptr->research_talent_priority=(char)*(mode_fac_sum_rb10.sel_num);
	    return 1;
	}
	vga.use_front();
	/*
	  if (mode_fac_sum_button.detect())
	  {
          String str;
          str="Are you sure? This action will commit $";
          float xxx=0;
          xxx=(finance.total_hires>0)?finance.cost_limit/finance.total_hires:0;	//## fred 0406
          xxx = (xxx<=0) ? cur_dept_ptr->peer_numeraire_salary : 0;					//## fred 0406
          str+=m.format(xxx,4);
          str+=" from this year's budget. \n";
          str+="Salary expenditure and any research revenue would start at the \n\ 
	  beginning of the following month, and the person would be \n\ 
	  available for teaching during the next semester. ";
	  int rc = box.ask(str.str_buf);
	  if(rc==1)
	  {
	  //				box.msg("Salary expenditure and any research revenue would start \n\ 
	  //							at the beginning of the following month, and the person \n\ 
	  //							would be available for teaching during the next semester. ");
	  cur_dept_ptr->faculty_hiring(true);
	  }
	  return 1;
	  }
	*/
    }
    return 0;
}

//------- End of function FacultySummary::report_detect ------//

void FacultyRes::summary_report_deinit() {
    if (text_refresh_bitmap1) {
	mem_del(text_refresh_bitmap1);
	text_refresh_bitmap1=NULL;
    }

    /*
      //------- deinit type buttons --------//
      if (*type_button_up_bitmap)
      {
      for (int i = 0; i < FAC_SUM_REPORT_TYPE_COUNT; i++)
      {
      mem_del(type_button_up_bitmap[i]);
      mem_del(type_button_down_bitmap[i]);
      type_button_up_bitmap[i] = NULL;
      type_button_down_bitmap[i] = NULL;
      }
      }
    */

    //	if (*mode_button_up_bitmap)
    //	{
    //		for (int i = 0; i < 2; i++)
    //		{
    //			mem_del(mode_button_up_bitmap[i]);
    //			mem_del(mode_button_down_bitmap[i]);
    //			mode_button_up_bitmap[i] = NULL;
    //			mode_button_down_bitmap[i] = NULL;
    //		}
    //	}

    fac_sum_rank_piechart.deinit();
    fac_sum_gender_ethnic_piechart.deinit();
    mode_fac_sum_rb1.deinit();
    mode_fac_sum_rb2.deinit();
    mode_fac_sum_rb3.deinit();
    mode_fac_sum_rb4.deinit();
    mode_fac_sum_rb5.deinit();
    mode_fac_sum_rb6.deinit();
    mode_fac_sum_rb7.deinit();
    mode_fac_sum_rb8.deinit();
    mode_fac_sum_rb9.deinit();
    mode_fac_sum_rb10.deinit();
	int i;
    for(i=0;i<16;i++)
	mode_fac_sum_graphicon_t[i].deinit();
    for(i=0;i<MAX_DEPARTMENT;i++)
	mode_fac_sum_valueslider[i].deinit();
    for(i=0;i<MAX_DEPARTMENT;i++)
	mode_fac_sum_valueslider2[i].deinit();
    for(i=0;i<MAX_DEPARTMENT;i++)
	mode_fac_sum_valueslider3[i].deinit();

    department_equity_graph.deinit();
    gender_ethnic_equity_graph.deinit();
    rank_equity_graph.deinit();

    // ##### begin Gilbert 25/04/2001 ######//
    mode_fac_sum_graphmonth1.deinit();
    mode_fac_sum_graphmonth2.deinit();
    mode_fac_sum_doublebar_y.deinit();
    mode_fac_sum_doublebar_m.deinit();

    for(i=0;i<16;++i)
	mode_fac_sum_data1[i].deinit();
    for(i=0;i<4;++i)
	mode_fac_sum_data2[i].deinit();
    for(i=0;i<MAX_DEPARTMENT;++i)
	mode_fac_sum_data3[i].deinit();
    for(i=0;i<7;++i)
	mode_fac_sum_data4[i].deinit();

    for(i=0;i<6;++i)
	mode_fac_sum_graphicon_y[i].deinit();
    for(i=0;i<16;++i)
	mode_fac_sum_graphicon_t[i].deinit();
    for(i=0;i<1;++i)
	mode_fac_sum_graphicon_m[i].deinit();

    mode_fac_sum_button.deinit();
    mode_fac_sum_buttongroup.deinit_buttons();
    mode_fac_sum_text1.deinit();

    type_button_group.deinit_buttons();             // but init again soon if just switching report
    // ##### end Gilbert 25/04/2001 ######//

    report_init_flag=0;
}

//------- End of function FacultySummary::report_deinit ------//
