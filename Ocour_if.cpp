//Filename    : OCOUR_IF.CPP
//Description : Course report
// owner			: Kevin(Ho)
// Kevin 0824

#include <OSYS.H>
#include <OSTR.H>
#include <OMISC.H>
#include <OPSCHOOL.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <ODEPT.H>
#include <OVGA.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OBUTTON.H>
#include <OTEXT.H>
#include <OHELP.H>
#include <OGRPHYR.H>
#include <OPieChrt.H>
#include <OGRPH_PT.H>
#include <OGRPH_PM.H>
#include <OLIBTECH.H>
#include <ORADIOV.H>
#include <OVALSLDR.H>
#include <OGRPNFYR.H>
#include <OGRPNFT.H>

//----------------- Departmental academic standing --------------------//
//-------------------- Main screen Configuration ----------------------//

enum {                                            // the bg image id
    BG_PIC_ID = 14
};

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1,
    REPORT_Y1 = ZOOM_Y1+5,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-35
};

enum {
    BOTTOM_BUTTON_X1 = REPORT_X1+120,
    BOTTOM_BUTTON_Y1 = REPORT_Y2-20,
    BOTTOM_BUTTON_X2 = REPORT_X2,
    BOTTOM_BUTTON_Y2 = REPORT_Y2+10
};
enum {
    BOTTOM_BUTTON_WIDTH = 160,
    BOTTOM_BUTTON_HEIGHT = 22,
    BOTTOM_BUTTON_DISTANCE
    = (BOTTOM_BUTTON_X2-BOTTOM_BUTTON_X1-BOTTOM_BUTTON_WIDTH*3)/4
};

enum {
    TABLE_COLUMN_COUNT=7,
    TABLE_COLUMN_WIDTH=102,
    TABLE_X1 = REPORT_X1+10,
    TABLE_X2 = TABLE_X1 + TABLE_COLUMN_WIDTH*TABLE_COLUMN_COUNT
};

enum {
    MODE_STATISTICS_GRAPH_WIDTH = 210,
    MODE_STATISTICS_GRAPH_HEIGHT = 115,
    MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE = (REPORT_X2-REPORT_X1-MODE_STATISTICS_GRAPH_WIDTH*3)/3,
    MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE = 20,
};

static ButtonGroup type_button_group(6);
//static short *type_button_up_bitmap[4];
//static short *type_button_down_bitmap[4];

static char* reportTypeStrArray[]
= {
    "Overview","Class types","Technology",
    "Overview","Summer teaching","Technology",
};

//static short *text_refresh_bitmap;
static char *mode_course_title = "Course report";
static char *mode_course_label[] = {
    "Class type",
    "Number of sections",
    "Enrollment",
    "Average class\r\nsize (Norm)",
    "Class size deviation from norm",
    "Students denied entrance\r\nto courses",
    //	"Students failing courses",   //## chea 210999 change to % to make more sence
    "Percent failing courses\r\n(prior trimester)",
    "Overall average class size"
};
static char *mode_teaching_label[] = {
    "Small seminars",
    "Medium-sized classes",
    "Lecture courses with section meetings",
    "Distance learning courses",
    "Total",
};

enum { DISP_TEACHING_METHOD_COUNT = 4 };

static char mode_teaching_id[] = {
    SEMINAR,
    GENERAL,
    CLASS_WITH_BREAKOUT,
    DISTANCE_LEARN_COURSE,
    BREAKOUT_LAB,
    // teaching method

};

enum { REPORT_MODE_COUNT = 2 };
enum { REPORT_MODE_INSTITUTION, REPORT_MODE_DEPARTMENT};

enum {
    REPORT_TYPE_DEPARTMENT_OVERVIEW, REPORT_TYPE_DEPARTMENT_CLASSTYPES, REPORT_TYPE_DEPARTMENT_TECHNOLOGY,
    REPORT_TYPE_INSTITUTION_OVERVIEW, REPORT_TYPE_INSTITUTION_SUMMERTEACHING, REPORT_TYPE_INSTITUTION_TECHNOLOGY,
};

static char current_report_mode=REPORT_MODE_DEPARTMENT;

static char report_init_flag = 0;

// screen 1,3 setting
static GraphPerformanceIndicatorTrimester mode_course_graphicon;
static Text mode_course_text2;
static Text mode_course_text[MAX_TEACHING_METHOD_N_TOTAL][8];

// screen 2 setting
static Text mode_course_current_fraction1;
static Text mode_course_current_fraction2;
static Text mode_course_current_fraction3;
static PieChart mode_course_piechart1;
static PieChart mode_course_piechart2;
static ValueSlider mode_course_valueslider1;
static ValueSlider mode_course_valueslider2;
static ValueSlider mode_course_valueslider3;

// screen 4 setting
static RadioButtonVert mode_course_rbv;
static ValueSlider mode_course_valueslider4;
static GraphPerformanceIndicatorMonth mode_course_graphicon2;
static char* mode_course_rblabel[]= {
    "None",
    "Encourage summer enrollment",
    "Strongly encourage summer enrollment",
    "Require traditional undergraduates to enroll for at least two summer sessions before graduation.",
};

// screen 5 setting
static ValueSlider mode_course_technology_slider[3];
// define three graphs here
static GraphNonFinancialTrimester it_utilization_graph;
static GraphNonFinancialYear outsourcing_credits_graph;
static GraphNonFinancialTrimester distance_credits_graph;
static char* graphTypeStrArray[]
= {
    "Percentage of courses redesigned to make optimal use of information technology",
    "Outsourced credits",
    "Distance learning credits provided",
};
static char* sliderTypeStrArray[]
= {
    "Departmental incentives for faculty to optimize IT in teaching",
    "Ease of outsourcing credits for closed courses",
    "Priority for provision of distance learning credits",
};

enum {
    MODE_PRIORITY_BOX_HEIGHT = 165,
    MODE_PRIORITY_BOX_WIDTH = 650,
    MODE_PRIORITY_BOX_TAB1 = 40,
    MODE_PRIORITY_BOX_TAB2 = 240,
    MODE_PRIORITY_BOX_TAB3 = 360,
    MODE_PRIORITY_VALUE_SLIDER_HEIGHT = 18,
    MODE_PRIORITY_VALUE_SLIDER_WIDTH = 180,
};

//static short * mode_course_refresh1;

//----- Begin of function Department::course_report ------//
//!
//! <int> refreshFlag = INFO_REPAINT or INFO_UPDATE
//!
void Department::course_report(int refreshFlag) {
    if(department_array.current_report_type==REPORT_TYPE_INSTITUTION_OVERVIEW) {
	current_report_mode=REPORT_MODE_INSTITUTION;
	sys.is_institution_report=1;
	course_report_1(refreshFlag);
    }
    else if(department_array.current_report_type==REPORT_TYPE_DEPARTMENT_OVERVIEW) {
	current_report_mode=REPORT_MODE_DEPARTMENT;
	sys.is_institution_report=0;
	course_report_1(refreshFlag);
    }
    else if(department_array.current_report_type==REPORT_TYPE_INSTITUTION_SUMMERTEACHING) {
	current_report_mode=REPORT_MODE_INSTITUTION;
	sys.is_institution_report=1;
	course_report_4(refreshFlag);
    }
    else if(department_array.current_report_type==REPORT_TYPE_DEPARTMENT_CLASSTYPES) {
	current_report_mode=REPORT_MODE_DEPARTMENT;
	sys.is_institution_report=0;
	course_report_2(refreshFlag);
    }
    else if(department_array.current_report_type==REPORT_TYPE_INSTITUTION_TECHNOLOGY) {
	current_report_mode=REPORT_MODE_INSTITUTION;
	sys.is_institution_report=1;
	course_report_5(refreshFlag);
    }
    else if(department_array.current_report_type==REPORT_TYPE_DEPARTMENT_TECHNOLOGY) {
	current_report_mode=REPORT_MODE_DEPARTMENT;
	sys.is_institution_report=0;
	course_report_5(refreshFlag);
    }
}

//------- End of function FacultySummary::report ------//

//-------- Begin of function Department::course_report1 ---------//
//!
void Department::course_report_1(int refreshFlag) {

    const static int vtab1=19;
    const static int vtab2=63;
    const static int vtab3=67;
    const static int vtab4=302;
    const static int vtab5=305;
    const static int vtab6=350;

    static int data_num=HISTORY_TRIMESTER_COUNT;
    static int *dataArray;
    static int v[MAX_TEACHING_METHOD_N_TOTAL][7];
    static int v2;

    // ##### Begin Marco ##### //
    static int initialv[MAX_TEACHING_METHOD_N_TOTAL][7];
    static int initialv2;

    memset(initialv,0,sizeof(initialv));
    // ##### End Marco ##### //

    //-----communication with sim engine's Output-----//

    memset(v,0,sizeof(v));

    if(current_report_mode==REPORT_MODE_INSTITUTION) {
	for(int i=0;i<MAX_TEACHING_METHOD_N_TOTAL;i++) {
	    v[i][0]=department_array.class_section_count[i];
	    v[i][1]=department_array.convention_student_count[i];
	    v[i][2]=department_array.average_class_size[i];
	    v[i][3]=department_array.size_deviation[i];
	    v[i][4]=department_array.admission_denied_count[i];
	    //			v[i][5]=department_array.failure_count[i];   //## chea 210999 change to % to make more sence
	    //## chea 210999 begin change to % to make more sence
	    if(department_array.convention_student_count[i]<1)
		v[i][5]=0;
	    else
		//## chea 210999 change to % to make more sence
		v[i][5]=(int)((float)department_array.failure_count[i] / (float)department_array.convention_student_count[i] * 100.0f);
	    //## chea 210999 end change to % to make more sence
	    v[i][6]=department_array.normal_class_size[i];

	    // ##### Begin Marco ##### //
	    initialv[i][0]=department_array.initial_class_section_count[i];
	    initialv[i][1]=department_array.initial_convention_student_count[i];
	    initialv[i][2]=department_array.initial_average_class_size[i];
	    initialv[i][3]=department_array.initial_size_deviation[i];
	    initialv[i][4]=department_array.initial_admission_denied_count[i];
	    if(department_array.initial_convention_student_count[i]<1)
		initialv[i][5]=0;
	    else
		initialv[i][5]=(int)((float)department_array.initial_failure_count[i] / (float)department_array.initial_convention_student_count[i] * 100.0f);
	    initialv[i][6]=department_array.initial_normal_class_size[i];
	    // ##### End Marco ##### //

	}
	dataArray=department_array.experienced_class_size;
	v2=dataArray[data_num-1];

	// ##### Begin Marco ##### //
	initialv2=department_array.initial_experienced_class_size;
	// ##### End Marco ##### //

    }
    else {
	int deptRecno = department_array.selected_recno;
	Department* cur_dept_ptr = department_array[deptRecno];
	for(int i=0;i<MAX_TEACHING_METHOD_N_TOTAL;i++) {
	    v[i][0]=cur_dept_ptr->course_array.class_section_count[i];
	    v[i][1]=cur_dept_ptr->course_array.convention_student_count[i];
	    v[i][2]=cur_dept_ptr->course_array.average_class_size[i];
	    v[i][3]=cur_dept_ptr->course_array.size_deviation[i];
	    v[i][4]=cur_dept_ptr->course_array.admission_denied_count[i];
	    //## chea 210999 begin change to % to make more sence
	    //			v[i][5]=cur_dept_ptr->course_array.failure_count[i];
	    if(cur_dept_ptr->course_array.convention_student_count[i]<1)
		v[i][5]=0;
	    else
		//## chea 210999 change to % to make more sence
		v[i][5]=(int)((float)cur_dept_ptr->course_array.failure_count[i] / (float)cur_dept_ptr->course_array.convention_student_count[i] * 100.0f);
	    //## chea 210999 begin change to % to make more sence
	    v[i][6]=cur_dept_ptr->course_array.normal_class_size[i];

	    // ##### Begin Marco ##### //
	    initialv[i][0]=cur_dept_ptr->course_array.initial_class_section_count[i];
	    initialv[i][1]=cur_dept_ptr->course_array.initial_convention_student_count[i];
	    initialv[i][2]=cur_dept_ptr->course_array.initial_average_class_size[i];
	    initialv[i][3]=cur_dept_ptr->course_array.initial_size_deviation[i];
	    initialv[i][4]=cur_dept_ptr->course_array.initial_admission_denied_count[i];
	    if(cur_dept_ptr->course_array.initial_convention_student_count[i]<1)
		initialv[i][5]=0;
	    else
		initialv[i][5]=(int)((float)cur_dept_ptr->course_array.initial_failure_count[i] / (float)cur_dept_ptr->course_array.initial_convention_student_count[i] * 100.0f);
	    initialv[i][6]=cur_dept_ptr->course_array.initial_normal_class_size[i];
	    // ##### End Marco #### //

	}
	dataArray=cur_dept_ptr->course_array.experienced_class_size;
	v2=dataArray[data_num-1];

	// #### begin Marco ##### //
	initialv2=cur_dept_ptr->course_array.initial_experienced_class_size;
	// ###### end marco ##### //
    }

    /*	int divideCourseCount[MAX_TEACHING_METHOD];
	memset(divideCourseCount,0,sizeof(divideCourseCount));
	for( int i=cur_dept_ptr->course_array.size() ; i>=1 ; i-- )
	{
	Course* coursePtr = cur_dept_ptr->course_array[i];

	v[(int)coursePtr->teaching_method][1]+=(float)coursePtr->class_section_count;
	v[(int)coursePtr->teaching_method][2]+=(float)coursePtr->convention_student_count;
	if( coursePtr->class_section_count > 0 )
	{
        v[(int)coursePtr->teaching_method][3]+=(float)coursePtr->convention_student_count / (float)coursePtr->class_section_count;
	divideCourseCount[coursePtr->teaching_method]++;
	}
	v[(int)coursePtr->teaching_method][5]+=(float)coursePtr->admission_denied_count;
	v[(int)coursePtr->teaching_method][6]+=(float)coursePtr->failure_count;
	}
	for(i=0;i<MAX_TEACHING_METHOD;i++)
	{
	if( divideCourseCount[i] )
	v[i][3]=v[i][3]/divideCourseCount[i];
	}
    */

    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-5,REPORT_X1+630,REPORT_Y1+14);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-5,REPORT_X1+630,REPORT_Y1+14,1);
	font_charts.put(REPORT_X1+320,REPORT_Y1-2,"Courses");

	user_interface.brighten(TABLE_X1,REPORT_Y1+vtab1,TABLE_X2,REPORT_Y1+vtab6);

	user_interface.rect(
	    TABLE_X1,REPORT_Y1+vtab5,
	    TABLE_X2+1,REPORT_Y1+vtab6,1);

	//-------------- Show Text --------------//
	user_interface.rect(
	    TABLE_X1,REPORT_Y1+vtab1,
	    TABLE_X1+TABLE_COLUMN_WIDTH+17,REPORT_Y1+vtab2+4,1);

	user_interface.rect(
	    TABLE_X1,REPORT_Y1+vtab3,
	    TABLE_X1+TABLE_COLUMN_WIDTH+17,REPORT_Y1+vtab4+2,1);

	font_chartsm.put_paragraph(
	    TABLE_X1+7,REPORT_Y1+vtab1+3,
	    (int)(TABLE_X1+(1.1)*TABLE_COLUMN_WIDTH),REPORT_Y1+vtab2,
	    mode_course_label[0],-1);

	user_interface.rect(
	    TABLE_X1+TABLE_COLUMN_WIDTH+15,REPORT_Y1+vtab1,
	    TABLE_X1+TABLE_COLUMN_WIDTH+TABLE_COLUMN_WIDTH+2,REPORT_Y1+vtab2+4,1);

	user_interface.rect(
	    TABLE_X1+TABLE_COLUMN_WIDTH+15,REPORT_Y1+vtab3,
	    TABLE_X1+TABLE_COLUMN_WIDTH+TABLE_COLUMN_WIDTH+2,REPORT_Y1+vtab4+2,1);

	font_chartsm.put_paragraph(
	    TABLE_X1+TABLE_COLUMN_WIDTH+25,REPORT_Y1+vtab1+3,
	    (int)(TABLE_X1+(2.1)*TABLE_COLUMN_WIDTH),REPORT_Y1+vtab2,
	    mode_course_label[1],-1);

	for(int i=2;i<TABLE_COLUMN_COUNT;i++) {
	    user_interface.rect(
		TABLE_X1+i*TABLE_COLUMN_WIDTH,REPORT_Y1+vtab1,
		TABLE_X1+i*TABLE_COLUMN_WIDTH+TABLE_COLUMN_WIDTH+2,REPORT_Y1+vtab2+4,1);

	    user_interface.rect(
		TABLE_X1+i*TABLE_COLUMN_WIDTH,REPORT_Y1+vtab3,
		TABLE_X1+i*TABLE_COLUMN_WIDTH+TABLE_COLUMN_WIDTH+2,REPORT_Y1+vtab4+2,1);

	    font_chartsm.put_paragraph(
		TABLE_X1+i*TABLE_COLUMN_WIDTH+7,REPORT_Y1+vtab1+3,
		(int)(TABLE_X1+(i+1.1)*TABLE_COLUMN_WIDTH),REPORT_Y1+vtab2+10,
		mode_course_label[i],-1);
	}
	for(int i=0;i<3;i++) {
	    font_chartsm.put_paragraph(
		TABLE_X1+7,REPORT_Y1+vtab3+45*i+15,
		TABLE_X1+10+TABLE_COLUMN_WIDTH,REPORT_Y1+vtab3+45*i+70,
		mode_teaching_label[i],-1);
	}

	for(int i=3;i<DISP_TEACHING_METHOD_COUNT+1;i++) {
	    // ##### Begin Marco ##### //

	    if ( i == 3 )                               // skip to print out Distance Learning courses
		continue;

	    if (i == 4) {
		font_chartsm.put_paragraph(
		    TABLE_X1+7,REPORT_Y1+vtab3+50*i,
		    TABLE_X1+10+TABLE_COLUMN_WIDTH,REPORT_Y1+vtab3+50*i+70,
		    mode_teaching_label[i],-1);

		font_chart_purple_sm.put(TABLE_X1+7,REPORT_Y1+vtab3+50*4+20, "Initial");
		vga_back.line(TABLE_X1+7,REPORT_Y1+vtab3+50*4+18, font_chart_purple_sm.text_width("Initial")+TABLE_X1+7,REPORT_Y1+vtab3+50*4+18,V_BLACK);
	    }
	    else {
		/* orginal */
		font_chartsm.put_paragraph(
		    TABLE_X1+7,REPORT_Y1+vtab3+50*i+15,
		    TABLE_X1+10+TABLE_COLUMN_WIDTH,REPORT_Y1+vtab3+50*i+70,
		    mode_teaching_label[i],-1);
		/* orginal */
	    }
	    // ##### End Marco ##### //
	}

	if(report_init_flag)
	    course_report_deinit();

	report_init_flag=1;

	/*		text_refresh_bitmap=
			vga_back.save_area(
			TABLE_X1+TABLE_COLUMN_WIDTH,
			REPORT_Y1+vtab3,
			TABLE_X2,
			REPORT_Y1+vtab4,
			text_refresh_bitmap);
	*/
	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;
	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");
	for (int i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
	}
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (int i = 3; i < 6; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
	}

	/*
	  short x = BOTTOM_BUTTON_X1;
	  short y = BOTTOM_BUTTON_Y1;

	  font_chartsm.put(REPORT_X1,y+2,"    Department :");
	  font_chartsm.put(REPORT_X1,y+30,"      Institution :");

	  for (i = 0; i < 2; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	  {
          user_interface.create_button_bitmap(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  reportTypeStrArray[i], &type_button_up_bitmap[i],
	  &type_button_down_bitmap[i], &vga_back);
	  type_button_group[i].create_bitmapW(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  ((i == department_array.current_report_type) ? type_button_down_bitmap[i] : type_button_up_bitmap[i])+4);
	  }

	  x = BOTTOM_BUTTON_X1;
	  y = BOTTOM_BUTTON_Y1+28;

	  for (i = 2; i < 4; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	  {
	  user_interface.create_button_bitmap(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  reportTypeStrArray[i], &type_button_up_bitmap[i],
	  &type_button_down_bitmap[i], &vga_back);
	  type_button_group[i].create_bitmapW(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  ((i == department_array.current_report_type) ? type_button_down_bitmap[i] : type_button_up_bitmap[i])+4);
	  }
	*/
	font_chartsm.put(TABLE_X1+110,REPORT_Y1+vtab5+15,mode_course_label[7]);

	mode_course_text2.right_put(TABLE_X1+400,REPORT_Y1+vtab5+15,&v2,4);
	mode_course_graphicon.init(TABLE_X1+500,REPORT_Y1+vtab5+5,
				   TABLE_X1+600,REPORT_Y1+vtab5+40,
				   1 ,&data_num ,dataArray, 1, 0, 3);
	mode_course_graphicon.paint();
	int j;
	for(int i=0;i<MAX_TEACHING_METHOD-1;i++) {
	    int classId = mode_teaching_id[i];
	    if ( classId == 1 )                         // skip to print Distance Learning Courses Data
		continue;
	    for(j=0;j<6;j++) {
		//## chea 210999 change failing to % (it print hoz)
		if(j==5) {
		    mode_course_text[i][j].right_put
			(
			    (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)-((j==2)?20:0)+((j==3)?10:0)),
			    REPORT_Y1+vtab3+50*i+15,
			    &(v[classId][j]),
			    3
			    );
		}
		else {
		    mode_course_text[i][j].right_put
			(
			    (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)-((j==2)?20:0)+((j==3)?10:0)),
			    REPORT_Y1+vtab3+50*i+15,
			    &(v[classId][j]),
			    (j==3)?3:4
			    );
		}
	    }
	    // (norm)
	    mode_course_text[i][6].right_put
		( (int)(TABLE_X1+3.4*TABLE_COLUMN_WIDTH+25+((j==0)?45:30)),
		  REPORT_Y1+vtab3+50*i+15,
		  &(v[classId][6]),
		  13
		    );

	}
	for(int j=0;j<6;j++) {
	    //## chea 210999 change failing to % (it print hoz)
	    if(j==5) {
		mode_course_text[MAX_TEACHING_METHOD-1][j].right_put
		    ( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
		      //REPORT_Y1+vtab3+50*4+15,
		      REPORT_Y1+vtab3+50*4,
		      &(v[MAX_TEACHING_METHOD_N_TOTAL-1][j]),
		      3
			);

		// ##### Begin Marco ##### //
		font_chart_purple_sm.right_put
		    ( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
		      REPORT_Y1+vtab3+50*4+20,
		      m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],3));

		if ( font_chart_purple_sm.text_width(m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],3)) > font_chartsm.text_width(m.format(v[MAX_TEACHING_METHOD_N_TOTAL-1][j],3)) )
		    vga_back.line
			( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0))-font_chart_purple_sm.text_width(m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],3)),
			  REPORT_Y1+vtab3+50*4+17,
			  (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
			  REPORT_Y1+vtab3+50*4+17,
			  V_BLACK);
		else
		    vga_back.line
			( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0))-font_chartsm.text_width(m.format(v[MAX_TEACHING_METHOD_N_TOTAL-1][j],3)),
			  REPORT_Y1+vtab3+50*4+17,
			  (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
			  REPORT_Y1+vtab3+50*4+17,
			  V_BLACK);
		// ##### End Marco ##### //

	    }
	    else {

		mode_course_text[MAX_TEACHING_METHOD-1][j].right_put
		    ( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
		      //REPORT_Y1+vtab3+50*4+15,
		      REPORT_Y1+vtab3+50*4,
		      &(v[MAX_TEACHING_METHOD_N_TOTAL-1][j]),
		      (j==3)?3:4
			);

		// ##### Begin Marco ##### //
		font_chart_purple_sm.right_put
		    ( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
		      REPORT_Y1+vtab3+50*4+20,
		      m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4));
		if (font_chart_purple_sm.text_width(m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4)) > font_chartsm.text_width(m.format(v[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4)) )
		    vga_back.line
			( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0))-font_chart_purple_sm.text_width(m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4)),
			  REPORT_Y1+vtab3+50*4+17,
			  (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
			  REPORT_Y1+vtab3+50*4+17,
			  V_BLACK);
		else
		    vga_back.line
			( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0))-font_chartsm.text_width(m.format(v[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4)),
			  REPORT_Y1+vtab3+50*4+17,
			  (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
			  REPORT_Y1+vtab3+50*4+17,
			  V_BLACK);
		// ##### End Marco ##### //

	    }

	}

    }
    else {
	// refresh screen
	for(int i=0;i<MAX_TEACHING_METHOD;i++)
	    for(int j=0;j<7;j++)
		mode_course_text[i][j].refresh();
	mode_course_text2.refresh();
	mode_course_graphicon.refresh();

	// ##### Begin Marco ##### //
	for(int j=0;j<6;j++) {
	    if(j==5) {
		font_chart_purple_sm.right_put
		    ( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
		      REPORT_Y1+vtab3+50*4+20,
		      m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],3));
		if (font_chart_purple_sm.text_width(m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],3)) > font_chartsm.text_width(m.format(v[MAX_TEACHING_METHOD_N_TOTAL-1][j],3)) )
		    vga_back.line
			( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0))-font_chart_purple_sm.text_width(m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4)),
			  REPORT_Y1+vtab3+50*4+17,
			  (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
			  REPORT_Y1+vtab3+50*4+17,
			  V_BLACK);
		else
		    vga_back.line
			( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0))-font_chartsm.text_width(m.format(v[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4)),
			  REPORT_Y1+vtab3+50*4+17,
			  (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
			  REPORT_Y1+vtab3+50*4+17,
			  V_BLACK);
	    }
	    else {
		font_chart_purple_sm.right_put
		    ( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
		      REPORT_Y1+vtab3+50*4+20,
		      m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4));
		if (font_chart_purple_sm.text_width(m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4)) > font_chartsm.text_width(m.format(v[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4)) )
		    vga_back.line
			( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0))-font_chart_purple_sm.text_width(m.format(initialv[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4)),
			  REPORT_Y1+vtab3+50*4+17,
			  (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
			  REPORT_Y1+vtab3+50*4+17,
			  V_BLACK);
		else
		    vga_back.line
			( (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0))-font_chartsm.text_width(m.format(v[MAX_TEACHING_METHOD_N_TOTAL-1][j],(j==3)?3:4)),
			  REPORT_Y1+vtab3+50*4+17,
			  (int)(TABLE_X1+(j+1.4)*TABLE_COLUMN_WIDTH+((j==0)?35:20)+((j==3)?10:0)),
			  REPORT_Y1+vtab3+50*4+17,
			  V_BLACK);
	    }
	}
	// ##### End Marco ##### //

    }

    type_button_group.paint(department_array.current_report_type);

    //	vga_back.rest_area(text_refresh_bitmap,0,0);

    /*	for(int i=1;i<TABLE_COLUMN_COUNT;i++)
	{
	for(int j=0;j<DISP_TEACHING_METHOD_COUNT;j++)
	{
        int classId = mode_teaching_id[j];
        int normClassSize = player_school.normal_class_size[classId];

        if(i==4)		// % class size deviation
        {
	float deviation;

	if( v[classId][1] > 0 )
	deviation = (float) 100 * (v[classId][3]-(float)normClassSize) / (float)normClassSize;
	else
	deviation = 0;		// no deviation if there is no section.

	Font *fontPtr = &font_chartsm;

	if( deviation < 0 )
	fontPtr = &font_red_san;

	fontPtr->center_put(
	TABLE_X1+i*TABLE_COLUMN_WIDTH,REPORT_Y1+vtab3+45*j+15,
	TABLE_X1+(i+1)*TABLE_COLUMN_WIDTH,REPORT_Y1+vtab3+45*j+15+font_chartsm.height(),
	m.format(deviation,3) );
	}
	else
	{
	font_chartsm.center_put(
	TABLE_X1+i*TABLE_COLUMN_WIDTH,REPORT_Y1+vtab3+45*j+15,
	TABLE_X1+(i+1)*TABLE_COLUMN_WIDTH,REPORT_Y1+vtab3+45*j+15+font_chartsm.height(),
	m.format((int)v[classId][i]) );

	if( i==3 )		// average class size, also display the norm
	{
	font_chartsm.center_put(
	TABLE_X1+i*TABLE_COLUMN_WIDTH+50,REPORT_Y1+vtab3+45*j+15,
	TABLE_X1+(i+1)*TABLE_COLUMN_WIDTH,REPORT_Y1+vtab3+45*j+15+font_chartsm.height(),
	m.format(-normClassSize) );		// use the negative sign to produce bracket display
	}
	}
	}
	}
    */
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

}

void Department::course_report_2(int refreshFlag) {

    const static int vtab1=21;
    const static int vtab2=51;
    const static int vtab3=51;
    const static int vtab4=201;
    const static int vtab5=211;
    const static int vtab6=350;
    const static int tab1=TABLE_X1;
    const static int tab2=TABLE_X1+250;
    const static int tab3=TABLE_X1+250;
    const static int tab4=TABLE_X1+470;
    const static int tab5=TABLE_X1+470;
    const static int tab6=TABLE_X2-10;

    static int u[3]= {
	11,22,23
    }
    ,v[3]= {
	40,25,35
    };
    static int xStart=1991,xEnd=1998;
    //-----communication with sim engine's Output-----//

    int deptRecno = department_array.selected_recno;
    Department* cur_dept_ptr = department_array[deptRecno];
    cur_dept_ptr->course_array;

    u[0]=cur_dept_ptr->course_preference[0];
    u[1]=cur_dept_ptr->course_preference[1];
    u[2]=cur_dept_ptr->course_preference[2];
    v[0]=cur_dept_ptr->course_preference_input[0];
    v[1]=cur_dept_ptr->course_preference_input[1];
    v[2]=cur_dept_ptr->course_preference_input[2];

    // normal teaching method percentage
    CourseTemplate* templ = course_res.get_template(department_id);

    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	user_interface.brighten(tab3,REPORT_Y1+vtab1,tab6,REPORT_Y1+vtab2);
	user_interface.brighten(tab1,REPORT_Y1+vtab3,tab6,REPORT_Y1+vtab6);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-5,REPORT_X1+630,REPORT_Y1+14);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-5,REPORT_X1+630,REPORT_Y1+14,1);
	font_charts.put(REPORT_X1+320,REPORT_Y1-2,"Courses");

	user_interface.rect(tab1,REPORT_Y1+vtab3,tab6,REPORT_Y1+vtab4,1);
	user_interface.rect(tab1,REPORT_Y1+vtab4,tab6,REPORT_Y1+vtab6,1);
	user_interface.rect(tab3,REPORT_Y1+vtab1,tab4,REPORT_Y1+vtab6,1);
	user_interface.rect(tab5-3,REPORT_Y1+vtab1,tab6,REPORT_Y1+vtab6,1);

	if(report_init_flag)
	    course_report_deinit();
	report_init_flag=1;

	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;
	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");
	for (int i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
	}
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (int i = 3; i < 6; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
	}

	mode_course_current_fraction1.right_put(tab3+90,REPORT_Y1+vtab5+15,u,3);
	mode_course_current_fraction2.right_put(tab3+90,REPORT_Y1+vtab5+65,u+1,3);
	mode_course_current_fraction3.right_put(tab3+90,REPORT_Y1+vtab5+115,u+2,3);

	for( int i=0 ; i<3 ; i++ ) {                      // display normal teaching method percentage
	    //marco
	    font_chart_purple_sm.right_put( tab3+150, REPORT_Y1+vtab5+15+i*50, m.format(cur_dept_ptr->initial_course_preference[i],3) );
	}

	mode_course_piechart1.init(tab3,REPORT_Y1+vtab3,tab4,REPORT_Y1+vtab4,3,u,&xStart,&xEnd,
				   NULL, NULL, NULL, 1, 1, 0, 3);
	mode_course_piechart2.init(tab5-3,REPORT_Y1+vtab3,tab6,REPORT_Y1+vtab4,3,v,&xStart,&xEnd,
				   NULL, NULL, NULL, 1, 1, 0, 3);
	mode_course_piechart1.paint();
	mode_course_piechart2.paint();

	//-------- draw the legend --------------//
	font_chartsm.put(tab1+75,REPORT_Y1+vtab5+15,"Seminar");
	font_chartsm.put(tab1+75,REPORT_Y1+vtab5+65,"Medium-sized classes");
	font_chartsm.put(tab1+75,REPORT_Y1+vtab5+115,"Lectures with breakouts");
	user_interface.bar(
	    tab1+54,REPORT_Y1+vtab5+15,tab1+66,REPORT_Y1+vtab5+27,
	    ((mode_course_piechart1.series_color)[0]));
	user_interface.bar(
	    tab1+54,REPORT_Y1+vtab5+65,tab1+66,REPORT_Y1+vtab5+77,
	    ((mode_course_piechart1.series_color)[1]));
	user_interface.bar(
	    tab1+54,REPORT_Y1+vtab5+115,tab1+66,REPORT_Y1+vtab5+127,
	    ((mode_course_piechart1.series_color)[2]));

	font_chartsm.put(tab3+45,REPORT_Y1+vtab1+10,"Current fraction");
	font_chartsm.put(tab5+45,REPORT_Y1+vtab1+10,"Target fraction");
	mode_course_valueslider1.init(tab5+15,REPORT_Y1+vtab5+10,200,20,0,100,&(v[0]),1,((mode_course_piechart1.series_color)[0]),3);
	mode_course_valueslider2.init(tab5+15,REPORT_Y1+vtab5+60,200,20,0,100,&(v[1]),1,((mode_course_piechart1.series_color)[1]),3);
	mode_course_valueslider3.init(tab5+15,REPORT_Y1+vtab5+110,200,20,0,100,&(v[2]),1,((mode_course_piechart1.series_color)[2]),3);
    }
    else {
	// refresh screen
	mode_course_current_fraction1.refresh();
	mode_course_current_fraction2.refresh();
	mode_course_current_fraction3.refresh();
	mode_course_piechart1.refresh();
	mode_course_piechart2.refresh();
    }

    mode_course_valueslider1.refresh();
    mode_course_valueslider2.refresh();
    mode_course_valueslider3.refresh();
    type_button_group.paint(department_array.current_report_type);

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

}

void Department::course_report_4(int refreshFlag) {

    const static int vtab1=20;
    const static int vtab2=78;
    const static int vtab3=82;
    const static int vtab4=280;
    const static int vtab5=285;
    const static int vtab6=350;

    static int data_num=HISTORY_MONTH_COUNT;
    static float *dataArray;

    static int v1=0;
    static double v2=12.345;
    static int v3=23;
    //-----communication with sim engine's Output-----//

    int deptRecno = department_array.selected_recno;
    Department* cur_dept_ptr = department_array[deptRecno];
    cur_dept_ptr->course_array;

    v1=player_school.summer_emphasis;

    // BUGHERE
    dataArray=library_tech_office.tech_utilization_history;
    v2=library_tech_office.tech_utilization_history[HISTORY_MONTH_COUNT-1];
    /////////

    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-5,REPORT_X1+630,REPORT_Y1+14);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-5,REPORT_X1+630,REPORT_Y1+14,1);
	font_charts.put(REPORT_X1+320,REPORT_Y1-2,"Courses");

	user_interface.brighten(TABLE_X1,REPORT_Y1+vtab1,TABLE_X2,REPORT_Y1+vtab2);
	user_interface.brighten(TABLE_X1,REPORT_Y1+vtab3,TABLE_X2,REPORT_Y1+vtab6);

	user_interface.rect(
	    TABLE_X1,REPORT_Y1+vtab1,
	    TABLE_X2,REPORT_Y1+vtab2+3,1);

	user_interface.rect(
	    TABLE_X1,REPORT_Y1+vtab3,
	    TABLE_X2,REPORT_Y1+vtab6,1);

	//-------------- Show Text --------------//
	font_chartsm.put_paragraph(
	    TABLE_X1+50,REPORT_Y1+vtab3+15,
	    TABLE_X2,REPORT_Y1+vtab3+45,
	    "Emphasis on summer course-taking");

	if(report_init_flag)
	    course_report_deinit();
	report_init_flag=1;

	/*
	  short x = BOTTOM_BUTTON_X1;
	  short y = BOTTOM_BUTTON_Y1;

	  font_chartsm.put(REPORT_X1,y+2,"    Department :");
	  font_chartsm.put(REPORT_X1,y+30,"      Institution :");

	  for (int i = 0; i < 2; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	  {
          user_interface.create_button_bitmap(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  reportTypeStrArray[i], &type_button_up_bitmap[i],
	  &type_button_down_bitmap[i], &vga_back);
	  type_button_group[i].create_bitmapW(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  ((i == department_array.current_report_type) ? type_button_down_bitmap[i] : type_button_up_bitmap[i])+4);
	  }

	  x = BOTTOM_BUTTON_X1;
	  y = BOTTOM_BUTTON_Y1+28;

	  for (i = 2; i < 4; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE)
	  {
	  user_interface.create_button_bitmap(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  reportTypeStrArray[i], &type_button_up_bitmap[i],
	  &type_button_down_bitmap[i], &vga_back);
	  type_button_group[i].create_bitmapW(x, y,
	  x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
	  ((i == department_array.current_report_type) ? type_button_down_bitmap[i] : type_button_up_bitmap[i])+4);
	  }
	*/
	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;
	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");
	for (int i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
	}
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (int i = 3; i < 6; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
	}

	font_chartsm.put(REPORT_X1+50,REPORT_Y1+vtab1+25,"Percentage of courses using information technology");

	// Initialize the objected to be displayed
	//		if(mode_course_refresh1)
	//			mem_del(mode_course_refresh1);
	//		mode_course_refresh1=NULL;
	//		mode_course_refresh=vga_back.save_area(REPORT_X1,REPORT_Y1+vtab5,REPORT_X2,REPORT_Y1+vtab6,mode_course_refresh);
	mode_course_text2.right_put(REPORT_X1+450,REPORT_Y1+vtab1+25,&v2,3);
	mode_course_graphicon2.init(REPORT_X1+550,REPORT_Y1+vtab1+10,
				    REPORT_X1+650,REPORT_Y1+vtab1+50,
				    1 ,&data_num ,dataArray, 1, 0, 3);

	mode_course_rbv.init(
	    REPORT_X1+100,
	    REPORT_Y1+vtab3+60,
	    REPORT_X2,
	    REPORT_Y1+vtab6,
	    4,
	    &v1,
	    mode_course_rblabel,
	    NULL
	    );

	//		mode_course_valueslider4.init(REPORT_X1+250,REPORT_Y1+vtab5+26,200,20,0,100,&v3,1,V_YELLOW,3);
	// Pre-paint the graph here
	mode_course_graphicon2.paint();
	//		vga_back.rest_area(mode_course_refresh1,0,0);
    }
    else {
	// refresh screen
	mode_course_text2.refresh();
	mode_course_graphicon2.refresh();
	mode_course_rbv.refresh();

	/*		if(((*mode_course_rbv.sel_num)==1)||
			((*mode_course_rbv.sel_num)==2)||
			((*mode_course_rbv.sel_num)==3))
			{
			font_chartsm.put(TABLE_X1+50,REPORT_Y1+vtab5,
			"Percent of summer teaching by regular faculty as part of normal teaching load");
			}
			else
			{
			vga_back.rest_area(mode_course_refresh1,0,0);
			}
	*/
    }
    /*	if(
	((*mode_course_rbv.sel_num)==1)||
	((*mode_course_rbv.sel_num)==2)||
	((*mode_course_rbv.sel_num)==3)
	)
	{
	mode_course_valueslider4.refresh();
	}
    */
    type_button_group.paint(department_array.current_report_type);

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

}

// --- Begin of Department::course_report_5 --- //
//
void Department::course_report_5(int refreshFlag) {
    const static int vtab1=21;
    const static int vtab2=51;
    const static int vtab3=51;
    const static int vtab4=201;
    const static int vtab5=211;
    const static int vtab6=350;
    const static int tab1=TABLE_X1;
    const static int tab2=TABLE_X1+250;
    const static int tab3=TABLE_X1+250;
    const static int tab4=TABLE_X1+470;
    const static int tab5=TABLE_X1+470;
    const static int tab6=TABLE_X2-10;

    int deptRecno = department_array.selected_recno;
    Department* deptPtr = department_array[deptRecno];
    int sliderVar1=deptPtr->department_incentives;
    int sliderVar2=deptPtr->ease_outsourcing_credits;
    int sliderVar3=deptPtr->distance_learning_credits;

    vga.use_back();

    if ( (refreshFlag == INFO_REPAINT) || (info.is_begin_trimester()) ) {
	if(report_init_flag)
	    course_report_deinit();
	report_init_flag=1;

	// used to store the three graphs data
	static float  dataArray[HISTORY_TRIMESTER_COUNT];
	static int  dataArray2[HISTORY_YEAR_COUNT];
	// used to store the three graphs data
	static int  dataArray3[HISTORY_TRIMESTER_COUNT];
	static int  numSeriesData = HISTORY_TRIMESTER_COUNT;
	static int  numSeriesData2 = HISTORY_YEAR_COUNT;

	memset( dataArray, 0, sizeof(dataArray) );
	memset( dataArray2, 0, sizeof(dataArray2) );
	memset( dataArray3, 0, sizeof(dataArray3) );

	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	user_interface.brighten(tab1,REPORT_Y1+vtab3,tab6,REPORT_Y1+vtab6);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-5,REPORT_X1+630,REPORT_Y1+14);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-5,REPORT_X1+630,REPORT_Y1+14,1);
	font_charts.put(REPORT_X1+180,REPORT_Y1-2,"Information Technology and Distance Learning Credits");

	int shiftNumber;
	if ( player_school.is_trimester_off ) {
	    if ( player_school.cur_trimester != 0 )
		shiftNumber = player_school.cur_trimester - 1;
	    else
		shiftNumber = 2;
	}
	else
	    shiftNumber = player_school.cur_trimester;

	if ( current_report_mode==REPORT_MODE_INSTITUTION ) {
	    for ( int i=0; i<HISTORY_TRIMESTER_COUNT; i++ ) {
		for ( int j=department_array.size(); j>0; j-- ) {
		    if ( department_array.is_deleted(j) )
			continue;

		    dataArray[i] += department_array[j]->p_tech_utilization_history[i];
		    dataArray3[i] += department_array[j]->distance_credits_provided_history[i];
		}
		dataArray[i] /= department_array.size();
		dataArray[i] = (int) dataArray[i];
	    }

	    for ( int i=0; i<HISTORY_YEAR_COUNT; i++ ) {
		for ( int j=department_array.size(); j>0; j-- ) {
		    if ( department_array.is_deleted(j) )
			continue;

		    int index = HISTORY_TRIMESTER_COUNT - shiftNumber - (HISTORY_YEAR_COUNT-1-i)*3 - 1;
		    dataArray2[i] += department_array[j]->course_array.outsourcing_credits_history[MAX_TEACHING_METHOD][index];
		}
	    }

	}
	else {
	    for ( int i=0; i<HISTORY_TRIMESTER_COUNT; i++ ) {
		dataArray[i] = (int)deptPtr->p_tech_utilization_history[i];
		dataArray3[i] = (int)deptPtr->distance_credits_provided_history[i];
	    }
	    for ( int i=0; i<HISTORY_YEAR_COUNT; i++ ) {
		int index = HISTORY_TRIMESTER_COUNT - shiftNumber - (HISTORY_YEAR_COUNT-1-i)*3 - 1;
		dataArray2[i] = (int)deptPtr->course_array.outsourcing_credits_history[MAX_TEACHING_METHOD][index];
	    }
	}

	// Three graph	--- Kenneth
	it_utilization_graph.init(
	    REPORT_X1+20,
	    REPORT_Y1+70,
	    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH+20,
	    REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+70,
	    1, &numSeriesData, dataArray, 0,
	    NULL, NULL, NULL, 1, 1, 1, 3);
	outsourcing_credits_graph.init(
	    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE+20,
	    REPORT_Y1+70,
	    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*2+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE+20,
	    REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+70,
	    1, &numSeriesData2, dataArray2, 0,
	    NULL, NULL, NULL, 1, 1, 1);
	distance_credits_graph.init(
	    REPORT_X1+(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)*2+20,
	    REPORT_Y1+70,
	    REPORT_X1+MODE_STATISTICS_GRAPH_WIDTH*3+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE*2+20,
	    REPORT_Y1+MODE_STATISTICS_GRAPH_HEIGHT+70,
	    1, &numSeriesData, dataArray3, 0,
	    NULL, NULL, NULL, 1, 1, 1);

	it_utilization_graph.paint();
	outsourcing_credits_graph.paint();
	distance_credits_graph.paint();

	// Three Texts
	for (int i = 0; i < 3; i++) {
	    font_chartsm.put_paragraph(
		REPORT_X1+(i%3)*(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)+20,
		REPORT_Y1+(i/3)*(MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE)+MODE_STATISTICS_GRAPH_HEIGHT+72,
		REPORT_X1+(i%3)*(MODE_STATISTICS_GRAPH_WIDTH+MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE)+MODE_STATISTICS_GRAPH_WIDTH+20,
		REPORT_Y1+(i/3)*(MODE_STATISTICS_GRAPH_HEIGHT+MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE)+MODE_STATISTICS_GRAPH_HEIGHT+72+4*font_chartsm.max_font_height,
		graphTypeStrArray[i]);
	}

	// Draw sliders if in department mode
	// need to draw it lower in Version 2.0
	if(current_report_mode==REPORT_MODE_DEPARTMENT) {
	    // Three Sliders
	    mode_course_technology_slider[0].init(
		REPORT_X1+MODE_PRIORITY_BOX_TAB2+200,
		REPORT_Y1+42+190+15,
		MODE_PRIORITY_VALUE_SLIDER_WIDTH,
		MODE_PRIORITY_VALUE_SLIDER_HEIGHT,
		0, 100, &sliderVar1, 1, V_YELLOW);

	    mode_course_technology_slider[1].init(
		REPORT_X1+MODE_PRIORITY_BOX_TAB2+200,
		REPORT_Y1+72+190+15,
		MODE_PRIORITY_VALUE_SLIDER_WIDTH,
		MODE_PRIORITY_VALUE_SLIDER_HEIGHT,
		0, 100, &sliderVar2, 1, V_YELLOW);

	    mode_course_technology_slider[2].init(
		REPORT_X1+MODE_PRIORITY_BOX_TAB2+200,
		REPORT_Y1+102+190+15,
		MODE_PRIORITY_VALUE_SLIDER_WIDTH,
		MODE_PRIORITY_VALUE_SLIDER_HEIGHT,
		0, 100, &sliderVar3, 1, V_YELLOW);

	    mode_course_technology_slider[0].refresh();
	    mode_course_technology_slider[1].refresh();
	    mode_course_technology_slider[2].refresh();

	    //  Display Text

	    font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y1+42+190+15,sliderTypeStrArray[0]);
	    font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y1+72+190+15,sliderTypeStrArray[1]);
	    font_chartsm.put(REPORT_X1+MODE_PRIORITY_BOX_TAB1,REPORT_Y1+102+190+15,sliderTypeStrArray[2]);
	}

	short x = BOTTOM_BUTTON_X1;
	short y = BOTTOM_BUTTON_Y1;
	font_chartsm.put(REPORT_X1,y+2,"    Department :");
	font_chartsm.put(REPORT_X1,y+30,"      Institution :");

	for ( int i = 0; i < 3; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
	}
	x = BOTTOM_BUTTON_X1;
	y = BOTTOM_BUTTON_Y1+28;
	for (int i = 3; i < 6; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) {
	    type_button_group[i].create_text(x, y,
					     x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					     reportTypeStrArray[i]);
	}
	type_button_group.paint(department_array.current_report_type);
    }

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//
// --- End of Department::course_report_5 --- //

void Department::course_report_deinit() {
    if(report_init_flag==0)return;

    report_init_flag = 0;

    /*	if(text_refresh_bitmap)
	{	mem_del(text_refresh_bitmap);
	text_refresh_bitmap=NULL;
	}
    */
    //	if(mode_course_refresh1)
    //	{
    //		mem_del(mode_course_refresh1);
    //		mode_course_refresh1 = NULL;
    //	}

    //------- deinit type buttons --------//
    /*	if (*type_button_up_bitmap)
	{
	for (int i = 0; i < 4; i++)
	{
        mem_del(type_button_up_bitmap[i]);
        mem_del(type_button_down_bitmap[i]);
        type_button_up_bitmap[i] = NULL;
        type_button_down_bitmap[i] = NULL;
	}
	}
    */

    // ##### begin Gilbert 27/04/2001 ######//
    type_button_group.deinit_buttons();
    // ##### end Gilbert 27/04/2001 ######//

    for(int i=0;i<MAX_TEACHING_METHOD;i++)
	for(int j=0;j<7;j++)
	    mode_course_text[i][j].deinit();

    mode_course_graphicon.deinit();
    mode_course_graphicon2.deinit();
    mode_course_text2.deinit();
    mode_course_rbv.deinit();
    mode_course_valueslider4.deinit();
    mode_course_current_fraction1.deinit();
    mode_course_current_fraction2.deinit();
    mode_course_current_fraction3.deinit();
    mode_course_valueslider1.deinit();
    mode_course_valueslider2.deinit();
    mode_course_valueslider3.deinit();
    mode_course_piechart1.deinit();
    mode_course_piechart2.deinit();
    mode_course_technology_slider[0].deinit();
    mode_course_technology_slider[1].deinit();
    mode_course_technology_slider[2].deinit();
}

int Department::course_report_detect() {
    //------- detect type buttons --------//
    int summaryButtonGroupDetect;

    if((summaryButtonGroupDetect = type_button_group.detect()) >= 0) {
	//		type_button_group[department_array.current_report_type].body_ptr = (void *) (type_button_up_bitmap[department_array.current_report_type] + 4);
	department_array.current_report_type = summaryButtonGroupDetect;
	//		type_button_group[department_array.current_report_type].body_ptr = (void *) (type_button_down_bitmap[department_array.current_report_type] + 4);
	sys.redraw_all_flag = 1;
	return 1;
    }

    /*	if(department_array.current_report_type == REPORT_TYPE_INSTITUTION_SUMMERTEACHING)
      if(
        ((*mode_course_rbv.sel_num)==1)||
        ((*mode_course_rbv.sel_num)==2)||
        ((*mode_course_rbv.sel_num)==3)
        )
      {
        if(mode_course_valueslider4.detect())
        {	player_school.percent_summer_teaching_load=*(mode_course_valueslider4.var_ptr);
          return 1;
        }
  }
  */
    if(department_array.current_report_type == REPORT_TYPE_INSTITUTION_SUMMERTEACHING) {
	help.set_help(TABLE_X1,REPORT_Y1+82,
		      TABLE_X2,REPORT_Y1+450,"H3.2.1.");

	if (mode_course_rbv.detect()) {
	    player_school.summer_emphasis=*(mode_course_rbv.sel_num);
	    return 1;
	}
    }

    if ( department_array.current_report_type == REPORT_TYPE_DEPARTMENT_TECHNOLOGY ) {
	if ( mode_course_technology_slider[0].detect() ) {
	    // change the value for the departmental incentives for fculty to optimize IT in teaching
	    int deptRecno = department_array.selected_recno;
	    Department* deptPtr = department_array[deptRecno];
	    deptPtr->department_incentives = *(mode_course_technology_slider[0].var_ptr);
	}
	if ( mode_course_technology_slider[1].detect() ) {
	    // change the value for the ease of outsourcing credits for closed courses
	    int deptRecno = department_array.selected_recno;
	    Department* deptPtr = department_array[deptRecno];
	    deptPtr->ease_outsourcing_credits = *(mode_course_technology_slider[1].var_ptr);
	}
	if ( mode_course_technology_slider[2].detect() ) {
	    // change the value for priority for provision of distance learning credits
	    int deptRecno = department_array.selected_recno;
	    Department* deptPtr = department_array[deptRecno];
	    deptPtr->distance_learning_credits = *(mode_course_technology_slider[2].var_ptr);
	}
    }

    if(department_array.current_report_type == REPORT_TYPE_DEPARTMENT_CLASSTYPES) {
	help.set_help(TABLE_X1+470,REPORT_Y1+200,TABLE_X1+690,REPORT_Y1+340,"H3.1.1.");
	if(mode_course_valueslider1.detect()) {
	    double sliderVar1,sliderVar2,sliderVar3,total;
	    int deptRecno = department_array.selected_recno;
	    Department* cur_dept_ptr = department_array[deptRecno];
	    sliderVar1=*(mode_course_valueslider1.var_ptr);
	    sliderVar2=*(mode_course_valueslider2.var_ptr);
	    sliderVar3=*(mode_course_valueslider3.var_ptr);
	    total=sliderVar2+sliderVar3;
	    if(total>1E-9) {
		*(mode_course_valueslider2.var_ptr)=
		    (int)((100.0-sliderVar1)*(sliderVar2/total));
	    }
	    else {
		*(mode_course_valueslider2.var_ptr)=
		    (int)(50-sliderVar1/2);
	    }
	    *(mode_course_valueslider3.var_ptr)=
		100-*(mode_course_valueslider2.var_ptr)
		-*(mode_course_valueslider1.var_ptr);

	    cur_dept_ptr->course_preference_input[0]=*(mode_course_valueslider1.var_ptr);
	    cur_dept_ptr->course_preference_input[1]=*(mode_course_valueslider2.var_ptr);
	    cur_dept_ptr->course_preference_input[2]=*(mode_course_valueslider3.var_ptr);

	    return 1;
	}
	if(mode_course_valueslider2.detect()) {
	    double sliderVar1,sliderVar2,sliderVar3,total;
	    int deptRecno = department_array.selected_recno;
	    Department* cur_dept_ptr = department_array[deptRecno];
	    sliderVar1=*(mode_course_valueslider1.var_ptr);
	    sliderVar2=*(mode_course_valueslider2.var_ptr);
	    sliderVar3=*(mode_course_valueslider3.var_ptr);
	    total=sliderVar1+sliderVar3;
	    if(total>1E-9) {
		*(mode_course_valueslider1.var_ptr)=
		    (int)((100.0-sliderVar2)*(sliderVar1/total));
	    }
	    else {
		*(mode_course_valueslider1.var_ptr)=
		    (int)(50-sliderVar2/2);
	    }
	    *(mode_course_valueslider3.var_ptr)=
		100-*(mode_course_valueslider2.var_ptr)
		-*(mode_course_valueslider1.var_ptr);

	    cur_dept_ptr->course_preference_input[0]=*(mode_course_valueslider1.var_ptr);
	    cur_dept_ptr->course_preference_input[1]=*(mode_course_valueslider2.var_ptr);
	    cur_dept_ptr->course_preference_input[2]=*(mode_course_valueslider3.var_ptr);
	    return 1;
	}
	if(mode_course_valueslider3.detect()) {
	    double sliderVar1,sliderVar2,sliderVar3,total;
	    int deptRecno = department_array.selected_recno;
	    Department* cur_dept_ptr = department_array[deptRecno];
	    sliderVar1=*(mode_course_valueslider1.var_ptr);
	    sliderVar2=*(mode_course_valueslider2.var_ptr);
	    sliderVar3=*(mode_course_valueslider3.var_ptr);
	    total=sliderVar1+sliderVar2;
	    if(total>1E-9) {
		*(mode_course_valueslider1.var_ptr)=
		    (int)((100.0-sliderVar3)*(sliderVar1/total));
	    }
	    else {
		*(mode_course_valueslider1.var_ptr)=
		    (int)(50-sliderVar3/2);
	    }
	    *(mode_course_valueslider2.var_ptr)=
		100-*(mode_course_valueslider3.var_ptr)
		-*(mode_course_valueslider1.var_ptr);
	    cur_dept_ptr->course_preference_input[0]=*(mode_course_valueslider1.var_ptr);
	    cur_dept_ptr->course_preference_input[1]=*(mode_course_valueslider2.var_ptr);
	    cur_dept_ptr->course_preference_input[2]=*(mode_course_valueslider3.var_ptr);
	    return 1;
	}
    }

    return 0;
}

//-------- End of function Department::main_report ---------//
