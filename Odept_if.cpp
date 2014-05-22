//Filename    : ODEPT_IF.CPP
//Description : Modeling_academic (Modeling_academic Operation) class
// owner			: Kevin(Ho)
// Notes:	Steps to introduce a new class of user interface
//				1.	construct the new class
//				2.	in sys.h, add the new MODE, e.g. MODE_ADMISSIONS_AND_FINANCIAL_OFFICE
//				3.	include the new header file in cc.cpp, osysdisp.cpp, osysdet.cpp
//				4.	in cc.cpp, declare a global object of the new class, with the name matching the 'extern' statement in the header file
//				5.	in osysdisp.cpp, under switch(view_mode), add the redirection to the display function
//				6.	in osysdet.cpp, under Sys::detect_view, add the redirection to the detect function
//				7.	in osysdet.cpp, under Sys::process_key, add the new hot key (optional)
//				8.	in osysdet.cpp, under Sys::detect_button, modify if necessary (see the comments there)
//				9.	the deinit function (AdmOffice::deinit()) can be called when the view_mode in 'sys' object is switched
//					the function frees memory occupied by this object
//					the report function would re-allocate memory when it is called (note the init_flag)
//					notice the beginning section of Sys::disp_frame()
// Kevin 0811

#include <OSYS.H>
#include <OSTR.H>
#include <OMISC.H>
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
#include <OHELPIF.H>
#include <OGRPHYR.H>
#include <OGRPHMTH.H>
#include <OGRPHTRI.H>
#include <OGRPNFYR.H>
#include <OGRPNFM.H>
#include <OGRPNFT.H>
#include <OGRPH_PY.H>
#include <OGRPH_PT.H>
#include <OGRPH_PM.H>
#include <ORESTXT.H>
#include <OVBROW98.H>
#include <OVGALOCK.H>
#include <OPRINT.H>
#include <ODATE.H>
#include <OPSCHOOL.H>
#include <OAUDIO.H>
#include <OCONFIG.H>

//----------------- Departmental academic standing --------------------//
//-------------------- Main screen Configuration ----------------------//

enum {                                            // the bg image id
    BG_PIC_ID = 11
};

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-25
};

enum {
    BUTTON_WIDTH = 50,
    BUTTON_HEIGHT = 25,
    BUTTON_DIST = 230,
    BUTTON_VDIST = 170
};

enum {
    MODE_DEPT_GRAPH_WIDTH = 215,
    MODE_DEPT_GRAPH_HEIGHT = 120,

    MODE_DEPT_TAB0 = 15,
    MODE_DEPT_TAB1 = 245,
    MODE_DEPT_TAB2 = 475,
    MODE_DEPT_TAB3 = 705,

    MODE_DEPT_VTAB1 = 10,
    MODE_DEPT_VTAB2 = 163,
    MODE_DEPT_VTAB3 = 397,
};

enum {
    MODE_INSTITUTION  = 0,
    MODE_DEPARTMENT = 1,
    MODE_RANK     = 2,
};

static Button help_button_array[9];
static Button mode_dept_button[15];
static short *mode_dept_button_up_bitmap[15];
static short *mode_dept_button_down_bitmap[15];
/*
static GraphTrimester mode_dept_graph0;
static GraphYear mode_dept_graph1;
static GraphTrimester mode_dept_graph2;
static GraphMonth mode_dept_graph3;
static GraphMonth mode_dept_graph4;
static GraphYear mode_dept_graph5;
*/
static GraphNonFinancialTrimester mode_dept_graph0;
static GraphNonFinancialYear mode_dept_graph1;
static GraphNonFinancialTrimester mode_dept_graph2;
static GraphNonFinancialMonth mode_dept_graph3;
static GraphNonFinancialMonth mode_dept_graph4;
static GraphNonFinancialYear mode_dept_graph5;

static char *mode_dept_title = "Academic Operations";
static char *mode_dept_str[] = {
    "Educational quality",
    "Departmental prestige",
    "Student morale",
    "Faculty research performance",
    "Faculty morale",
    "Faculty diversity index",
};

static char report_init_flag = 0;
//------------------------------------------------------------------//

//-------------------- sub-windows Configuration ----------------------//

enum {
    MODE_DEPT_MAIN = 0,
    MODE_DEPT_EDUCATIONAL_QUALITY=1,
    MODE_DEPT_DEPARTMENTAL_ACADEMIC_STANDING=2,
    MODE_DEPT_STUDENT_MORALE=3,
    MODE_DEPT_FACULTY_RESEARCH_PERFORMANCE=4,
    MODE_DEPT_FACULTY_MORALE=5,
    MODE_DEPT_FACULTY_DIVERSITY_INDEX=6,
    MODE_DEPT_EDUCATIONAL_QUALITY2=7,
    MODE_DEPT_RANK=8,
};

//-------- define static functions ------//

// Notes:	the deinit_x functions should deallocate memory used by the
//				GraphA, ValueSlider, RadioButton, etc. objects
//				the functions are called when a mode-switching is triggered
//	Norris 0716

static void report_main(int);
static void report_educational_quality(int);
static void report_educational_quality2(int);
static void report_departmental_academic(int);
static void report_student_morale(int);
static void report_faculty_research_preformance(int);
static void report_faculty_morale(int);
static void report_diversity_index(int);
static void report_rank(int);

static int detect_main();
static int detect_main2();
static int detect_educational_quality();
static int detect_educational_quality2();
static int detect_departmental_academic();
static int detect_student_morale();
static int detect_faculty_research_preformance();
static int detect_faculty_morale();
static int detect_diversity_index();
static int detect_rank();

static void deinit_main();
static void deinit_educational_quality();
static void deinit_educational_quality2();
static void deinit_departmental_academic();
static void deinit_student_morale();
static void deinit_faculty_research_preformance();
static void deinit_faculty_morale();
static void deinit_diversity_index();
static void deinit_rank();

static void eq_disp_rep_row(int recNo, int x, int y, int refreshFlag);
static void da_disp_rep_row(int recNo, int x, int y, int refreshFlag);
static void sm_disp_rep_row(int recNo, int x, int y, int refreshFlag);
static void fp_disp_rep_row(int recNo, int x, int y, int refreshFlag);
static void fm_disp_rep_row(int recNo, int x, int y, int refreshFlag);
static void di_disp_rep_row(int recNo, int x, int y, int refreshFlag);
static void rk_disp_rep_row(int recNo, int x, int y, int refreshFlag);

typedef void (*ReportFP)(int refreshFlag);
typedef int (*DetectFP)();
typedef void (*DeinitFP)();

static ReportFP report_func_ptr[] = {
    &report_main,
    &report_educational_quality,
    &report_departmental_academic,
    &report_student_morale,
    &report_faculty_research_preformance,
    &report_faculty_morale,
    &report_diversity_index,
    &report_educational_quality2,
    &report_rank,
};

static DetectFP detect_func_ptr[] = {
    &detect_main,
    &detect_educational_quality,
    &detect_departmental_academic,
    &detect_student_morale,
    &detect_faculty_research_preformance,
    &detect_faculty_morale,
    &detect_diversity_index,
    &detect_educational_quality2,
    &detect_rank,
};

static DeinitFP deinit_func_ptr[] = {
    &deinit_main,
    &deinit_educational_quality,
    &deinit_departmental_academic,
    &deinit_student_morale,
    &deinit_faculty_research_preformance,
    &deinit_faculty_morale,
    &deinit_diversity_index,
    &deinit_educational_quality2,
    &deinit_rank,
};

void Department::set_report_init_flag(int flag) {
    report_init_flag=flag;
}

//-------- Begin of function Department::main_report ---------//
//!
void Department::main_report(int refreshFlag) {
    (*report_func_ptr[department_array.current_screen_mode])(refreshFlag);
}

void Department::main_report_deinit() {

    //	(*deinit_func_ptr[department_array.current_screen_mode])();

    for(int i=0;i<9;i++)
	(*deinit_func_ptr[i])();

    report_init_flag = 0;
}

int Department::main_report_detect() {
    return (*detect_func_ptr[department_array.current_screen_mode])();
}

//-------- End of function Department::main_report ---------//

//-------- Begin of function deinit_main ---------//
//!
void deinit_main() {
    if (report_init_flag==0)return;

    //--------- deallocate memory -------------//
    if (mode_dept_button_up_bitmap[0]) {
	for(int k=0;k<2;k++) {
	    // ##### begin Gilbert 25/04/2001 ######//
	    mode_dept_button[k].deinit();
	    // ##### end Gilbert 25/04/2001 ######//
	    mem_del(mode_dept_button_up_bitmap[k]);
	    mem_del(mode_dept_button_down_bitmap[k]);
	    mode_dept_button_up_bitmap[k] = NULL;
	    mode_dept_button_down_bitmap[k] = NULL;
	}
    }
    if (mode_dept_button_up_bitmap[6]) {
	for(int k=6;k<15;k++) {
	    // ##### begin Gilbert 25/04/2001 ######//
	    mode_dept_button[k].deinit();
	    // ##### end Gilbert 25/04/2001 ######//
	    mem_del(mode_dept_button_up_bitmap[k]);
	    mem_del(mode_dept_button_down_bitmap[k]);
	    mode_dept_button_up_bitmap[k] = NULL;
	    mode_dept_button_down_bitmap[k] = NULL;
	}
    }
    if (report_init_flag) {
	mode_dept_graph0.deinit();
	mode_dept_graph1.deinit();
	mode_dept_graph2.deinit();
	mode_dept_graph3.deinit();
	mode_dept_graph4.deinit();
	mode_dept_graph5.deinit();
    }

    if(department_array.current_screen_mode>0)
	(*deinit_func_ptr[department_array.current_screen_mode])();
    report_init_flag=0;
}

//
//-------- End of function deinit_main ---------//

//-------- Begin of function Department::main_report ---------//
//!
void report_main(int refreshFlag) {

    static int  numSeriesData36 = HISTORY_MONTH_COUNT;
    static int  numSeriesData24 = HISTORY_TRIMESTER_COUNT;
    static int  numSeriesData8 = HISTORY_YEAR_COUNT;

    static float xStart0,xEnd0;
    static int xStart1,xEnd1;
    static float xStart2,xEnd2;
    static float xStart3,xEnd3;
    static float xStart4,xEnd4;
    static int xStart5,xEnd5;

    static float *dataArray1;
    static float *dataArray2;
    static float *dataArray3;
    static float *dataArray4;
    static float *dataArray5;
    static float *dataArray0;

    int deptRecno = department_array.selected_recno;
    Department* cur_dept_ptr = department_array[deptRecno];

    if(department_array.current_data_mode==MODE_DEPARTMENT) {
	dataArray0=cur_dept_ptr->p_educational_quality_history;
	dataArray1=cur_dept_ptr->p_academic_standing_history;
	dataArray2=cur_dept_ptr->p_student_morale_history;
	dataArray3=cur_dept_ptr->p_faculty_research_history;
	dataArray4=cur_dept_ptr->p_faculty_morale_history;
	dataArray5=cur_dept_ptr->p_faculty_diversity_history;
	sys.is_institution_report=0;
    }
    else {
	dataArray0=department_array.p_educational_quality_history;
	dataArray1=department_array.p_academic_standing_history;
	dataArray2=department_array.p_student_morale_history;
	dataArray3=department_array.p_faculty_research_history;
	dataArray4=department_array.p_faculty_morale_history;
	dataArray5=department_array.p_faculty_diversity_history;
	sys.is_institution_report=1;

	// update institutional prestige as sub_score[] is updated monthly while p_academic_standing_history[] is updated yearly. We do this to sync the values.

	department_array.p_academic_standing_history[THIS_YEAR] = player_school.sub_score[S_PRESTIGE][THIS_MONTH];
    }
    //-----------initial data-----------//

    xEnd0 = float(info.game_year)+float(info.game_month)/12.0f;
    xStart0 = xEnd0-6.0f;
    xEnd1 = (info.game_year);
    xStart1 = (info.game_year-numSeriesData8+1);
    xEnd2 = float(info.game_year)+float(info.game_month)/12.0f;
    xStart2 = xEnd2-6.0f;
    xEnd3 = float(info.game_year)+float(info.game_month)/12.0f;
    xStart3 = xEnd3-3.0f;
    xEnd4 = float(info.game_year)+float(info.game_month)/12.0f;
    xStart4 = xEnd4-3.0f;
    xEnd5 = (info.game_year);
    xStart5 = (info.game_year-numSeriesData8+1);

    vga.use_back();
    if (refreshFlag == INFO_REPAINT) {
	if( report_init_flag )                        // deinit it first if it has already been initialized
	    deinit_main();

	//------------- paint the background ----------------//

	//		static short callTime=0;

	//		if( ++callTime%2==0 )
	user_interface.bg_img(BG_PIC_ID, &vga_back);
	//		else
	//			user_interface.bg_img(BG_PIC_ID-1, &vga_back);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1-4);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1-4,1);
	if(department_array.current_data_mode==MODE_DEPARTMENT)
	    font_charts.put(REPORT_X1+270,REPORT_Y1-19,"Departmental Performance");
	else
	    font_charts.put(REPORT_X1+270,REPORT_Y1-19,"Institutional Performance");

	user_interface.brighten(REPORT_X1,REPORT_Y1-2,REPORT_X2,REPORT_Y2-20);
	user_interface.brighten(100,REPORT_Y2-10,250,REPORT_Y2-10+BUTTON_HEIGHT);
	user_interface.brighten(330,REPORT_Y2-10,480,REPORT_Y2-10+BUTTON_HEIGHT);
	user_interface.brighten(550,REPORT_Y2-10,700,REPORT_Y2-10+BUTTON_HEIGHT);
	user_interface.rect(REPORT_X1,REPORT_Y1-2,REPORT_X2,REPORT_Y2-20,1);
	//		font_chartsm.put(REPORT_X2/2-70,REPORT_Y1-25,mode_dept_title);

	//----- initialize the buttons and the graph -----//
	if (!report_init_flag) {
	    for(int y = MODE_DEPT_VTAB1+REPORT_Y1+15,i=0,k=0;i<2;i++,y+=BUTTON_VDIST)
		for(int x = MODE_DEPT_TAB0+REPORT_X1,j=0;j<3;j++,x+=BUTTON_DIST,k++) {
		    if((k>=0)&&(k<=1)) {
			//## chea 200999 get rid of the "rank" and  "detail" same code but diff.pos. ORG.
			/*
			  // detail button
			  mode_dept_button_up_bitmap[k] = NULL;
			  mode_dept_button_down_bitmap[k] = NULL;

			  user_interface.create_button_bitmap(
			  x+MODE_DEPT_GRAPH_WIDTH-BUTTON_WIDTH,
			  y+MODE_DEPT_GRAPH_HEIGHT+7,
			  x+MODE_DEPT_GRAPH_WIDTH,
			  y+MODE_DEPT_GRAPH_HEIGHT+BUTTON_HEIGHT+2,
			  "Detail", &mode_dept_button_up_bitmap[k],
			  &mode_dept_button_down_bitmap[k], &vga_back);

			  mode_dept_button[k].create_bitmapW(
			  x+MODE_DEPT_GRAPH_WIDTH-BUTTON_WIDTH,
			  y+MODE_DEPT_GRAPH_HEIGHT+7,
			  x+MODE_DEPT_GRAPH_WIDTH,
			  y+MODE_DEPT_GRAPH_HEIGHT+BUTTON_HEIGHT+2,
			  mode_dept_button_down_bitmap[k]+4);

			  if(department_array.current_data_mode==MODE_DEPARTMENT)
			  mode_dept_button[k].paint();
			  else
			  {

			  vga.blt_buf(
			  x+MODE_DEPT_GRAPH_WIDTH-BUTTON_WIDTH,
			  y+MODE_DEPT_GRAPH_HEIGHT+7,
			  x+MODE_DEPT_GRAPH_WIDTH,
			  y+MODE_DEPT_GRAPH_HEIGHT+BUTTON_HEIGHT+7,0);
			  vga.blt_buf(
			  x+MODE_DEPT_GRAPH_WIDTH-BUTTON_WIDTH,
			  y+MODE_DEPT_GRAPH_HEIGHT+7,
			  x+MODE_DEPT_GRAPH_WIDTH,
			  y+MODE_DEPT_GRAPH_HEIGHT+BUTTON_HEIGHT+7,0);

			  }
			*/
			//## chea 200999 end get rid of the "rank" and  "detail" same code but diff.pos. ORG.

			// detail button
			mode_dept_button_up_bitmap[k] = NULL;
			mode_dept_button_down_bitmap[k] = NULL;

			user_interface.create_button_bitmap(
			    x+MODE_DEPT_GRAPH_WIDTH-5*BUTTON_WIDTH/2,
			    y+MODE_DEPT_GRAPH_HEIGHT+7,
			    x+MODE_DEPT_GRAPH_WIDTH-3*BUTTON_WIDTH/2,
			    y+MODE_DEPT_GRAPH_HEIGHT+BUTTON_HEIGHT+2,
			    "Detail", &mode_dept_button_up_bitmap[k],
			    &mode_dept_button_down_bitmap[k], &vga_back);

			mode_dept_button[k].create_bitmapW(
			    x+MODE_DEPT_GRAPH_WIDTH-5*BUTTON_WIDTH/2,
			    y+MODE_DEPT_GRAPH_HEIGHT+7,
			    x+MODE_DEPT_GRAPH_WIDTH-3*BUTTON_WIDTH/2,
			    y+MODE_DEPT_GRAPH_HEIGHT+BUTTON_HEIGHT+2,
			    mode_dept_button_down_bitmap[k]+4);

			if(department_array.current_data_mode==MODE_DEPARTMENT)
			    mode_dept_button[k].paint();
			else {

			    vga.blt_buf(
				x+MODE_DEPT_GRAPH_WIDTH-5*BUTTON_WIDTH/2,
				y+MODE_DEPT_GRAPH_HEIGHT+7,
				x+MODE_DEPT_GRAPH_WIDTH-3*BUTTON_WIDTH/2,
				y+MODE_DEPT_GRAPH_HEIGHT+BUTTON_HEIGHT+2,
				0);
			    vga.blt_buf(
				x+MODE_DEPT_GRAPH_WIDTH-5*BUTTON_WIDTH/2,
				y+MODE_DEPT_GRAPH_HEIGHT+7,
				x+MODE_DEPT_GRAPH_WIDTH-3*BUTTON_WIDTH/2,
				y+MODE_DEPT_GRAPH_HEIGHT+BUTTON_HEIGHT+2,
				0);

			}

		    }

		    // rank button
		    mode_dept_button_up_bitmap[k+6] = NULL;
		    mode_dept_button_down_bitmap[k+6] = NULL;

		    user_interface.create_button_bitmap(
			x+MODE_DEPT_GRAPH_WIDTH-5*BUTTON_WIDTH/2,
			y+MODE_DEPT_GRAPH_HEIGHT+7,
			x+MODE_DEPT_GRAPH_WIDTH-3*BUTTON_WIDTH/2,
			y+MODE_DEPT_GRAPH_HEIGHT+BUTTON_HEIGHT+2,
			"Rank", &mode_dept_button_up_bitmap[k+6],
			&mode_dept_button_down_bitmap[k+6], &vga_back);

		    mode_dept_button[k+6].create_bitmapW(
			x+MODE_DEPT_GRAPH_WIDTH-5*BUTTON_WIDTH/2,
			y+MODE_DEPT_GRAPH_HEIGHT+7,
			x+MODE_DEPT_GRAPH_WIDTH-3*BUTTON_WIDTH/2,
			y+MODE_DEPT_GRAPH_HEIGHT+BUTTON_HEIGHT+2,
			mode_dept_button_down_bitmap[k+6]+4);

		    //					mode_dept_button[k+6].paint();  //## chea 200999 get rid of the "rank" and  "detail"

		    if(k==0)
			mode_dept_graph0.init(
			    x, y,
			    x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			    1, &numSeriesData24, dataArray0, 0,
			    NULL, NULL, NULL, 1, 1, 1, 3);

		    if(k==1)
			mode_dept_graph1.init(
			    x, y,
			    x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			    1, &numSeriesData8, dataArray1, 0,
			    NULL, NULL, NULL, 1, 1, 1, 3);

		    if(k==2)
			mode_dept_graph2.init(
			    x, y,
			    x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			    1, &numSeriesData24, dataArray2, 0,
			    NULL, NULL, NULL, 1, 1, 1, 3);

		    if(k==3)
			mode_dept_graph3.init(
			    x, y,
			    x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			    1, &numSeriesData36, dataArray3,
			    0,
			    NULL, NULL, NULL, 1,
			    1, 1, 3);

		    if(k==4)
			mode_dept_graph4.init(
			    x, y,
			    x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			    1, &numSeriesData36, dataArray4,
			    0,
			    NULL, NULL, NULL, 1, 1, 1, 3);

		    if(k==5)
			mode_dept_graph5.init(
			    x, y,
			    x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			    1, &numSeriesData8, dataArray5,
			    0,
			    NULL, NULL, NULL, 1, 1, 1, 3);

		}
	    report_init_flag = 1;
	}

	for(int k=0;k<3;k++) {
	    mode_dept_button_up_bitmap[k+12] = NULL;
	    mode_dept_button_down_bitmap[k+12] = NULL;
	}

	user_interface.create_button_bitmap(
	    100,REPORT_Y2-10,
	    250,REPORT_Y2-10+BUTTON_HEIGHT,
	    "Institution", &mode_dept_button_up_bitmap[12],
	    &mode_dept_button_down_bitmap[12], &vga_back);

	mode_dept_button[12].create_bitmapW(
	    100,REPORT_Y2-10,
	    250,REPORT_Y2-10+BUTTON_HEIGHT,
	    mode_dept_button_down_bitmap[12]+4);

	user_interface.create_button_bitmap(
	    330,REPORT_Y2-10,
	    480,REPORT_Y2-10+BUTTON_HEIGHT,
	    "Department", &mode_dept_button_up_bitmap[13],
	    &mode_dept_button_down_bitmap[13], &vga_back);

	mode_dept_button[13].create_bitmapW(
	    330,REPORT_Y2-10,
	    480,REPORT_Y2-10+BUTTON_HEIGHT,
	    mode_dept_button_down_bitmap[13]+4);

	user_interface.create_button_bitmap(
	    550,REPORT_Y2-10,
	    700,REPORT_Y2-10+BUTTON_HEIGHT,
	    "Rank by department", &mode_dept_button_up_bitmap[14],
	    &mode_dept_button_down_bitmap[14], &vga_back);

	mode_dept_button[14].create_bitmapW(
	    550,REPORT_Y2-10,
	    700,REPORT_Y2-10+BUTTON_HEIGHT,
	    mode_dept_button_down_bitmap[14]+4);

	for(int k=12;k<15;k++) {
	    mode_dept_button[k].paint();
	}

	mode_dept_button[department_array.current_data_mode+12].body_ptr=(void*)(mode_dept_button_down_bitmap[department_array.current_data_mode+12]+4);
	mode_dept_button[department_array.current_data_mode+12].push();

	mode_dept_graph0.paint();
	mode_dept_graph1.paint();
	mode_dept_graph2.paint();
	mode_dept_graph3.paint();
	mode_dept_graph4.paint();
	mode_dept_graph5.paint();
    }
    else {
	//----repaint---///
	for(int k=0;k<15;k++) {
	    //------------- paint the background ----------------//

	    //## chea 200999 get rid of the "rank" and  "detail"
	    /*
              if((k>=2)&&(k<=5))continue;
              if(department_array.current_data_mode==MODE_DEPARTMENT)
	      mode_dept_button[k].paint();
              else
              {

	      vga.blt_buf(
	      mode_dept_button[0].x1,
	      mode_dept_button[0].y1,
	      mode_dept_button[0].x2,
	      mode_dept_button[0].y2+5,0);
	      vga.blt_buf(
	      mode_dept_button[1].x1,
	      mode_dept_button[1].y1,
	      mode_dept_button[1].x2,
	      mode_dept_button[1].y2+5,0);

	      }
	    */
	}

	mode_dept_button[department_array.current_data_mode+12].body_ptr=(void*)(mode_dept_button_down_bitmap[department_array.current_data_mode+12]+4);
	mode_dept_button[department_array.current_data_mode+12].push();

	mode_dept_graph0.refresh();
	mode_dept_graph1.refresh();
	mode_dept_graph2.refresh();
	mode_dept_graph3.refresh();
	mode_dept_graph4.refresh();
	mode_dept_graph5.refresh();
    }
    //---------------//
    for(int y = MODE_DEPT_VTAB1+REPORT_Y1+15,i=0,k=0;i<2;i++,y+=BUTTON_VDIST) {
	for(int x = MODE_DEPT_TAB0+REPORT_X1,j=0;j<3;j++,x+=BUTTON_DIST,k++) {
	    char* str = mode_dept_str[k];

	    // k==1 is Prestige display
	    if( k==1 && department_array.current_data_mode==MODE_INSTITUTION )
		str = "Institutional prestige";

	    font_chartsm.center_put(x,y-BUTTON_HEIGHT+7,x+MODE_DEPT_GRAPH_WIDTH,y-BUTTON_HEIGHT+23,str);
	}
    }

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

    //------- blt the back buffer to front buffer -------//
    vga.use_front();
}

//
//-------- End of function Department::main_report ---------//

//-------- Begin of function dept_rep_detect ---------//
//!
int detect_main() {
    vga.use_back();

    int rc = detect_main2();

    vga.use_front();

    return rc;
}

//
//-------- End of function dept_rep_detect ---------//

//-------- Begin of function dept_rep_detect2 ---------//
//!
int detect_main2() {
    vga.use_front();

    for(int k=0;k<15;k++) {
	//		if(k>=2&&k<=5)continue;    //## chea 200999 org.
	if(k>=2&&k<=11)continue;                      //## chea 200999 get rid of the "rank" and  "detail"

	//## chea 200999 get rid of the "rank" and  datail
	if(department_array.current_data_mode != MODE_DEPARTMENT && (k>=0 && k<=1))continue;

	if(mode_dept_button[k].detect()) {

	    mode_dept_button[k].body_ptr=(void*)(mode_dept_button_down_bitmap[k]+4);
	    mode_dept_button[k].push();

	    (*deinit_func_ptr[department_array.current_screen_mode])();

	    if((k>=0)&&(k<2)) {
		department_array.current_screen_mode=k+1;
		department_array.current_pref_report_type=k+1;
	    }

	    if((k>=6)&&(k<12)) {
		department_array.current_screen_mode=MODE_DEPT_RANK;
		department_array.current_pref_report_type=k-5;
	    }
	    if(k==12||k==13) {
		department_array.current_screen_mode=MODE_DEPT_MAIN;
		department_array.current_data_mode=k-12;
	    }

	    if(k==14) {
		department_array.current_screen_mode=MODE_DEPT_RANK;
		department_array.current_pref_report_type=1;
		sys.is_institution_report = 1;
	    }

	    sys.redraw_all_flag = 1;
	    return 1;
	}
    }
    return 0;
}

//
//-------- End of function dept_rep_detect2 ---------//

///////////////////////////////////////////////////////////////////////
#define REC_HEIGHT 20
#define REC_COUNT 15
#define SHEET_COLOR Vga::active_buf->color_down

static Button eq_button_para,button_back;
static GraphPerformanceIndicatorTrimester eq_graphiconA;
static GraphPerformanceIndicatorTrimester eq_graphicon[EDUCATIONAL_QUALITY_VAR_COUNT];
static Text eq_text[EDUCATIONAL_QUALITY_VAR_COUNT],eq_textA;
static Text eq_text2[EDUCATIONAL_QUALITY_VAR_COUNT];
static Text eq_text_chg[EDUCATIONAL_QUALITY_VAR_COUNT],eq_text_chgA;

static GraphPerformanceIndicatorYear as_graphiconA;
static GraphPerformanceIndicatorYear as_graphicon[ACADEMIC_STANDING_VAR_COUNT];
static Text da_text[ACADEMIC_STANDING_VAR_COUNT],da_textA;
static Text da_text2[ACADEMIC_STANDING_VAR_COUNT];
static Text da_text_chg[ACADEMIC_STANDING_VAR_COUNT],da_text_chgA;

static GraphPerformanceIndicatorTrimester sm_graphiconA;
static GraphPerformanceIndicatorTrimester sm_graphicon[4];
static Text sm_text[4],sm_textA;
static Text sm_text_chg[4],sm_text_chgA;

static GraphPerformanceIndicatorMonth fp_graphiconA;
static GraphPerformanceIndicatorMonth fp_graphicon;
static Text fp_text,fp_textA;
static Text fp_text_chg,fp_text_chgA;

static GraphPerformanceIndicatorMonth fm_graphiconA;
static GraphPerformanceIndicatorMonth fm_graphicon;
static Text fm_text,fm_textA;
static Text fm_text_chg,fm_text_chgA;

static GraphPerformanceIndicatorYear di_graphiconA;
static GraphPerformanceIndicatorYear di_graphicon;
static Text di_text,di_textA;
static Text di_text_chg,di_text_chgA;

static VBrowse98  eq_browser;
static VBrowse98  rk_browser;

///////////////////////////////////////////////////////////////////////

//-------- Begin of functions report/detect/deinit educational_quality ---------//
//!
static char* educational_quality_label[]= {
    "Faculty teaching performance rating",
    "Faculty educational development time (hours per week)",
    "Faculty scholarship performance",
    "Faculty research performance",
    "Use of information technology in teaching",
    "Deviation of class size from its norm",
    "Deviation of teaching method mix from its norm",
};

void report_educational_quality(int refreshFlag) {

    const static int tab1=30,tab2=320,tab3=420,tab4=490;
    const static int tab5=620,tab6=680,tab7=660,tab8=760;
    const static int vtab1=32,vtab2=47,vtab3=210,vtab4=360,vtab5=320;
    static float v[EDUCATIONAL_QUALITY_VAR_COUNT];
    static float v2[EDUCATIONAL_QUALITY_VAR_COUNT];
    static float v3[EDUCATIONAL_QUALITY_VAR_COUNT];
    static int data_num=HISTORY_TRIMESTER_COUNT;
    static float * dataArray[EDUCATIONAL_QUALITY_VAR_COUNT];
    static float * dataArrayA;
    static float eq_chg;

    //-----communication with sim engine's Output-----//
    int deptRecno = department_array.selected_recno;
    Department* cur_dept_ptr = department_array[deptRecno];

    if(department_array.current_data_mode==MODE_DEPARTMENT) {
	sys.is_institution_report=0;
	dataArrayA=cur_dept_ptr->p_educational_quality_history;
	for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++) {
	    dataArray[i]=cur_dept_ptr->var_educational_quality_history[i];
	    v[i]=dataArray[i][data_num-1];
	    v3[i]=cur_dept_ptr->var_educational_quality_weight[i]*100;
	    v2[i]=cur_dept_ptr->var_change_educational_quality[i];
	}
    }
    else {
	sys.is_institution_report=1;
	dataArrayA=department_array.p_educational_quality_history;
	for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++) {
	    // BUGHERE -- wait for Fred's preparation
	    //			dataArray[i]=department_array.var_educational_quality_history[i];
	    dataArray[i]=cur_dept_ptr->var_educational_quality_history[i];
	    //			v[i]=(float)i;
	    v[i]=dataArray[i][data_num-1];
	    v3[i]=cur_dept_ptr->var_educational_quality_weight[i]*100;
	    v2[i]=cur_dept_ptr->var_change_educational_quality[i];
	}
    }

    if(info.graph_trimester_passed>1) {
	eq_chg=(float)(100.0*((double)dataArrayA[data_num-1]-dataArrayA[data_num-2])/dataArrayA[data_num-2]);
	if((eq_chg<0.0)&&(eq_chg>-0.01))
	    eq_chg=0.0;
    }
    else
	eq_chg=0.0;

    vga.use_back();
    if (refreshFlag == INFO_REPAINT) {
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1,1);
	font_charts.put(REPORT_X1+270,REPORT_Y1-19,"Departmental Performance");
	user_interface.brighten(REPORT_X1,REPORT_Y1+vtab1-30,REPORT_X2,REPORT_Y2-10);
	user_interface.rect(REPORT_X1,REPORT_Y1+vtab1-30,REPORT_X2,REPORT_Y1+vtab1-10);
	user_interface.rect(REPORT_X1,REPORT_Y1+vtab1-10,REPORT_X2,REPORT_Y1+vtab1+30);
	user_interface.rect( REPORT_X1,REPORT_Y1+vtab1+30,REPORT_X2,REPORT_Y2-10);

	//--------------Initialize buttons-------------------//
	button_back.create_text( REPORT_X1+tab1+250,
				 REPORT_Y2-1, REPORT_X1+tab1+440, REPORT_Y2+19, "Back" );
	//		button_back.set_key( 'B' );

	//--------------Initialize eq_graph-------------------//
	eq_graphiconA.init(
	    REPORT_X1+tab5-5,REPORT_Y1+vtab1-8,
	    REPORT_X1+tab6+5,REPORT_Y1+vtab1+28,
	    1 ,&data_num ,dataArrayA, 1, 0, 3);

	eq_textA.right_put(
	    REPORT_X1+tab2+80,REPORT_Y1+vtab1+5,&(dataArrayA[data_num-1]),4);

	eq_text_chgA.right_put(
	    REPORT_X1+tab4+85,REPORT_Y1+vtab1+5,
	    eq_chg,4);

	font_chartsm.put(REPORT_X1+tab1,REPORT_Y1+vtab1+5,"E d u c a t i o n a l   q u a l i t y");

	for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++) {
	    eq_graphicon[i].init(
		REPORT_X1+tab5,REPORT_Y1+vtab1+i*42+40,
		REPORT_X1+tab6,REPORT_Y1+vtab1+i*42+68,
		1 ,&data_num ,dataArray[i], 1, 0, 3);
	}

	for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++) {
	    font_chart_purple_sm.right_put( REPORT_X1+tab2+50, REPORT_Y1+vtab1+i*42+50, m.format(cur_dept_ptr->initial_education_equality[i],4) );

	    if(i==0||i>1&&i<5)
		eq_text[i].right_put(REPORT_X1+tab2+80,REPORT_Y1+vtab1+i*42+50,v+i,4);
	    else
		eq_text[i].right_put(REPORT_X1+tab2+80,REPORT_Y1+vtab1+i*42+50,v+i,4);

	    eq_text2[i].right_put(REPORT_X1+tab3+55,REPORT_Y1+vtab1+i*42+50,v3+i,3);
	    eq_text_chg[i].right_put(REPORT_X1+tab4+85,REPORT_Y1+vtab1+i*42+50,v2+i,4);

	    help_button_array[i].create_text(REPORT_X1+tab1-21,REPORT_Y1+vtab2+i*42+30,REPORT_X1+tab1-7,REPORT_Y1+vtab2+i*42+44,"?");
	    image_interface.put_back(REPORT_X1+tab1-21,REPORT_Y1+vtab2+i*42+30,"QUEST_UP");
	}

	//-------------- Show Text --------------//
	//		font_chartsm.put(REPORT_X1+tab1,REPORT_Y1+vtab1-27,"Contributing factors");
	font_chartsm.put(REPORT_X1+tab2+15,REPORT_Y1+vtab1-25,"Current value");
	font_chartsm.put(REPORT_X1+tab3+15,REPORT_Y1+vtab1-25,"Weight");
	font_chartsm.put(REPORT_X1+tab4+5,REPORT_Y1+vtab1-25,"Last trimester change");
	font_chartsm.put(REPORT_X1+tab5+20,REPORT_Y1+vtab1-25,"Trend");

	for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++) {
	    font_chartsm.put_paragraph(REPORT_X1+tab1+5,REPORT_Y1+vtab1+i*42+49,REPORT_X1+tab2+30,REPORT_Y1+vtab1+i*42+94,educational_quality_label[i]);
	}
	eq_graphiconA.paint();
	eq_textA.paint();

	for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++) {
	    eq_graphicon[i].paint();
	    eq_text[i].paint();
	    eq_text2[i].paint();
	    eq_text_chg[i].paint();
	}
    }
    else {
	eq_graphiconA.refresh();
	eq_textA.refresh();
	eq_text_chgA.refresh();
	for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++) {
	    eq_graphicon[i].refresh();
	    eq_text[i].refresh();
	    eq_text_chg[i].refresh();
	}
    }

    button_back.paint();

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    vga.use_front();
};

int detect_educational_quality() {
    const static int tab1=30,tab2=320,tab3=420,tab4=490;
    const static int tab5=620,tab6=680,tab7=660,tab8=760;
    const static int vtab1=22,vtab2=37,vtab3=200,vtab4=350,vtab5=320;
    for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++) {
	if(help_button_array[i].detect()) {
	    // ## begin chwg1022  create the global ResTxt help_resource later //
	    String str;
	    str  = DIR_RES;
	    str += "SCORE";

	    ResTxt rs;
	    rs.init(str);
	    // ## chwg1022 end ////////////////////////

	    image_interface.put_back(REPORT_X1+tab1-21,REPORT_Y1+vtab2+i*42+40,"QUEST_DN");
	    vga.blt_buf(REPORT_X1+tab1-21,REPORT_Y1+vtab2+i*42+40,REPORT_X1+tab1-3,REPORT_Y1+vtab2+i*42+58);
	    sys.back_view_mode=sys.view_mode;
	    help_if.help_textlist.topic_id=rs.locate_topic(educational_quality_label[i]);
	    if(help_if.help_textlist.topic_id>0) {
		help_if.help_textlist.add_history(help_if.help_textlist.topic_id);
		sys.set_view_mode(MODE_HELP_DESC);
		sys.redraw_all_flag = 1;
		audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    }
	    return 1;

	}
    }

    if(button_back.detect()) {
	(*deinit_func_ptr[department_array.current_screen_mode])();
	//-------- post display redraw flag -------//
	sys.redraw_all_flag = 1;
	department_array.current_screen_mode=MODE_DEPT_MAIN;
	return 1;
    }
    return 0;
};

void deinit_educational_quality() {
    for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++)
	eq_graphicon[i].deinit();
    eq_graphiconA.deinit();
    eq_textA.deinit();
    eq_text_chgA.deinit();
    for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++) {
	eq_text[i].deinit();
	eq_text2[i].deinit();
	eq_text_chg[i].deinit();
	// ##### begin Gilbert 28/04/2001 ######//
	help_button_array[i].deinit();
	// ##### end Gilbert 28/04/2001 ######//
    }
    // ##### begin Gilbert 28/04/2001 ######//
    button_back.deinit();
    // ##### end Gilbert 28/04/2001 ######//
};
//
//-------- End of functions report/detect/deinit educational_quality ---------//

//-------- Begin of functions report/detect/deinit educational_quality2 page 2---------//
//!

void report_educational_quality2(int refreshFlag) {
    const static int tab1=10;
    const static int tab2=600;
    const static int tab3=650;
    const static int tab4=700;
    const static int vtab1=-20;
    const static int vtab2=200;
    const static int vtab3=225;
    const static int vtab4=360;
    int report_recno=1;
    static int reccount=REC_COUNT;
    vga.use_back();
    if (refreshFlag == INFO_REPAINT) {
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);
	user_interface.rect(REPORT_X1,REPORT_Y1+vtab1,REPORT_X2,REPORT_Y1+vtab4);
	user_interface.brighten(REPORT_X1,REPORT_Y1+vtab1,REPORT_X2,REPORT_Y1+vtab4);

	//--------------Initialize buttons-------------------//
	eq_button_para.create_text( REPORT_X1+tab1+200,
				    REPORT_Y2+2, REPORT_X1+tab1+350, REPORT_Y2+22, "Variables" );
	//		eq_button_para.set_key( 'V' );

	button_back.create_text( REPORT_X1+tab1+450,
				 REPORT_Y2+2, REPORT_X1+tab1+500, REPORT_Y2+22, "Back" );

	//		button_back.set_key( 'B' );

	//--------------Initialize Vbroswer-------------------//
	eq_browser.init(REPORT_X1,REPORT_Y1+vtab1,REPORT_X2,REPORT_Y1+vtab4,
			0, REC_HEIGHT, reccount, eq_disp_rep_row,NULL,0);
	eq_browser.open(report_recno);

	//-------------- Show Text --------------//
    }
    else {
    }

    report_recno=eq_browser.recno();

    vga.use_back();
    eq_button_para.paint();
    button_back.paint();

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    eq_browser.update();
    vga.use_front();
};

int detect_educational_quality2() {

    vga.use_front();
    eq_browser.detect();

    if(eq_button_para.detect()) {
	(*deinit_func_ptr[department_array.current_screen_mode])();
	//-------- post display redraw flag -------//
	sys.redraw_all_flag = 1;
	department_array.current_screen_mode=MODE_DEPT_EDUCATIONAL_QUALITY;
	return 1;
    }

    if(button_back.detect()) {
	(*deinit_func_ptr[department_array.current_screen_mode])();
	//-------- post display redraw flag -------//
	sys.redraw_all_flag = 1;
	department_array.current_screen_mode=MODE_DEPT_MAIN;
	return 1;
    }

    return 0;
};

void deinit_educational_quality2() {
    // #### begin Gilbert 28/04/2001 #####//
    eq_browser.deinit();
    button_back.deinit();
    eq_button_para.deinit();
    // #### end Gilbert 28/04/2001 #####//
};

static void eq_disp_rep_row(int recNo, int x, int y, int refreshFlag) {
    //----- display financial report -----//
    if( refreshFlag == INFO_REPAINT ) {
	vga.use_back();
	//		user_interface.bar(x, y, REPORT_X2-40, y+REC_HEIGHT,SHEET_COLOR);
	//		vga.blt_buf( x, y, REPORT_X2-45, y+REC_HEIGHT-2, 0 );
    }

    if(recNo==1) {
	font_chartsm.put_paragraph
	    (
		x, y, REPORT_X2-45, y+REC_HEIGHT-2,
		"Text here..."
		);
    }

};

//
//-------- End of functions report/detect/deinit educational_quality2 page 2 ---------//

//-------- Begin of functions report/detect/deinit departmental_academic ---------//
//!
static char* departmental_academic_label[]= {
    "Faculty research performance",
    "Sponsored research",
    "Quality rating for doctoral students",
    "Doctoral students per regular faculty member"
};

void report_departmental_academic(int refreshFlag) {
    const static int tab1=30,tab2=320,tab3=420,tab4=490;
    const static int tab5=620,tab6=680,tab7=660,tab8=760;
    const static int vtab1=32,vtab2=47,vtab3=210,vtab4=360,vtab5=320;
    static float v[ACADEMIC_STANDING_VAR_COUNT];
    static float v2[ACADEMIC_STANDING_VAR_COUNT];
    static float v3[ACADEMIC_STANDING_VAR_COUNT];
    static int data_num=HISTORY_YEAR_COUNT;
    static float *dataArray[ACADEMIC_STANDING_VAR_COUNT];
    static float *dataArrayA;
    static float da_chg;

    //-----communication with sim engine's Output-----//
    int deptRecno = department_array.selected_recno;
    Department* cur_dept_ptr = department_array[deptRecno];

    if(department_array.current_data_mode==MODE_DEPARTMENT) {
	sys.is_institution_report=0;
	dataArrayA=cur_dept_ptr->p_academic_standing_history;
	for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++) {
	    dataArray[i]=cur_dept_ptr->var_academic_standing_history[i];
	    v[i]=dataArray[i][data_num-1];
	    v2[i]=cur_dept_ptr->var_change_academic_standing[i];
	    v3[i]=cur_dept_ptr->var_academic_standing_weight[i]*100;
	}
    }
    else {
	sys.is_institution_report=1;
	dataArrayA=department_array.p_academic_standing_history;
	for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++) {
	    // BUGHERE -- wait for Fred's preparation
	    //			dataArray[i]=department_array.var_educational_quality_history[i];
	    //			v[i]=dataArray[i][data_num-1];
	    dataArray[i]=cur_dept_ptr->var_academic_standing_history[i];
	    //			v[i]=(float)i;
	    //			v2[i]=cur_dept_ptr->var_change_academic_standing[i];
	    v[i]=dataArray[i][data_num-1];
	    v2[i]=cur_dept_ptr->var_change_academic_standing[i];
	    v3[i]=cur_dept_ptr->var_academic_standing_weight[i]*100;
	}
    }

    if(info.graph_trimester_passed>1) {
	da_chg=(float)(100.0*((double)dataArrayA[data_num-1]-dataArrayA[data_num-2])/dataArrayA[data_num-2]);
	if((da_chg<0.0)&&(da_chg>-0.01))
	    da_chg=0.0;
    }
    else
	da_chg=0.0;

    vga.use_back();
    if (refreshFlag == INFO_REPAINT) {
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1-4);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1-4,1);
	font_charts.put(REPORT_X1+270,REPORT_Y1-19,"Departmental Performance");

	user_interface.brighten(REPORT_X1,REPORT_Y1+vtab1-30,REPORT_X2,REPORT_Y2-10);

	user_interface.rect(REPORT_X1,REPORT_Y1+vtab1-30,REPORT_X2,REPORT_Y1+vtab1-10);
	user_interface.rect(REPORT_X1,REPORT_Y1+vtab1-10,REPORT_X2,REPORT_Y1+vtab1+30);
	user_interface.rect(REPORT_X1,REPORT_Y1+vtab1+30,REPORT_X2,REPORT_Y2-10);

	//--------------Initialize buttons-------------------//
	button_back.create_text( REPORT_X1+tab1+250,
				 REPORT_Y2-1, REPORT_X1+tab1+440, REPORT_Y2+19, "Back" );

	//		button_back.set_key( 'B' );

	//--------------Initialize da_graph-------------------//
	as_graphiconA.init(
	    REPORT_X1+tab5,REPORT_Y1+vtab1-8,
	    REPORT_X1+tab6,REPORT_Y1+vtab1+28,
	    1 ,&data_num ,dataArrayA, 1, 0, 3);
	da_textA.right_put(
	    REPORT_X1+tab2+80,REPORT_Y1+vtab1+5,&(dataArrayA[data_num-1]),4);

	da_text_chgA.right_put(
	    REPORT_X1+tab4+85,REPORT_Y1+vtab1+5,
	    &da_chg,4);

	font_chartsm.put(REPORT_X1+tab1,REPORT_Y1+vtab1+5,
			 "D e p a r t m e n t a l   P r e s t i g e");

	for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++)
	    as_graphicon[i].init(
		REPORT_X1+tab5-5,REPORT_Y1+vtab1+i*56+40,
		REPORT_X1+tab6+5,REPORT_Y1+vtab1+i*56+68,
		1 ,&data_num ,dataArray[i], 1, 0, 3);
	for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++) {
	    font_chart_purple_sm.right_put( REPORT_X1+tab2+20, REPORT_Y1+vtab1+i*56+50, m.format(cur_dept_ptr->initial_academic_standing[i],4) );

	    if(i==0||i>1&&i<5)
		da_text[i].right_put(REPORT_X1+tab2+80,REPORT_Y1+vtab1+i*56+50,v+i,4);
	    else
		da_text[i].right_put(REPORT_X1+tab2+80,REPORT_Y1+vtab1+i*56+50,v+i,4);

	    da_text2[i].right_put(REPORT_X1+tab3+55,REPORT_Y1+vtab1+i*56+50,v3+i,3);
	    da_text_chg[i].right_put(REPORT_X1+tab4+85,REPORT_Y1+vtab1+i*56+50,v2+i,4);
	    help_button_array[i].create_text(REPORT_X1+tab1-23,REPORT_Y1+vtab2+i*56+30,
					     REPORT_X1+tab1-9,REPORT_Y1+vtab2+i*56+44,"?");
	    image_interface.put_back(REPORT_X1+tab1-21,REPORT_Y1+vtab2+i*56+30,"QUEST_UP");
	}

	//-------------- Show Text --------------//
	//		font_chartsm.put(REPORT_X1+tab1,REPORT_Y1+vtab1-27,"Contributing factors");
	font_chartsm.put(REPORT_X1+tab2+15,REPORT_Y1+vtab1-25,"Current value");
	font_chartsm.put(REPORT_X1+tab3+15,REPORT_Y1+vtab1-25,"Weight");
	font_chartsm.put(REPORT_X1+tab4+5,REPORT_Y1+vtab1-25,"Last year change");
	font_chartsm.put(REPORT_X1+tab5+20,REPORT_Y1+vtab1-25,"Trend");

	for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++) {
	    font_chartsm.put_paragraph(REPORT_X1+tab1+5,REPORT_Y1+vtab1+i*56+48,REPORT_X1+tab2+30,REPORT_Y1+vtab1+i*56+94,departmental_academic_label[i]);
	}

	as_graphiconA.paint();
	da_textA.paint();

	for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++) {
	    as_graphicon[i].paint();
	    da_text[i].paint();
	    da_text2[i].paint();
	    da_text_chg[i].paint();
	}
    }
    else {
	as_graphiconA.refresh();
	da_textA.refresh();
	da_text_chgA.refresh();
	for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++) {
	    as_graphicon[i].refresh();
	    da_text[i].refresh();
	    da_text_chg[i].refresh();
	}
    }

    vga.use_back();
    button_back.paint();

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    vga.use_front();

};

int detect_departmental_academic() {
    const static int tab1=30,tab2=320,tab3=420,tab4=490;
    const static int tab5=620,tab6=680,tab7=660,tab8=760;
    const static int vtab1=22,vtab2=37,vtab3=200,vtab4=350,vtab5=320;
    for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++) {
	if(help_button_array[i].detect()) {
	    // ## begin chwg1022  create the global ResTxt help_resource later //
	    String str;
	    str  = DIR_RES;
	    str += "SCORE";

	    ResTxt rs;
	    rs.init(str);
	    // ## chwg1022 end ////////////////////////
	    image_interface.put_back(REPORT_X1+tab1-21,REPORT_Y1+vtab2+i*56+40,"QUEST_DN");
	    vga.blt_buf(REPORT_X1+tab1-21,REPORT_Y1+vtab2+i*56+40,REPORT_X1+tab1-3,REPORT_Y1+vtab2+i*56+58);
	    sys.back_view_mode=sys.view_mode;
	    help_if.help_textlist.topic_id=rs.locate_topic(departmental_academic_label[i]);
	    if(help_if.help_textlist.topic_id>0) {
		help_if.help_textlist.add_history(help_if.help_textlist.topic_id);
		sys.set_view_mode(MODE_HELP_DESC);
		sys.redraw_all_flag = 1;
		audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    }
	    return 1;
	}
    }
    if(button_back.detect()) {
	(*deinit_func_ptr[department_array.current_screen_mode])();
	//-------- post display redraw flag -------//
	sys.redraw_all_flag = 1;
	department_array.current_screen_mode=MODE_DEPT_MAIN;
	return 1;
    }
    return 0;
};

void deinit_departmental_academic() {
    for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++)
	as_graphicon[i].deinit();
    as_graphiconA.deinit();
    da_textA.deinit();
    da_text_chgA.deinit();
    for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++) {
	da_text[i].deinit();
	da_text2[i].deinit();
	da_text_chg[i].deinit();
	help_button_array[i].deinit();
    }
    // ###### begin Gilbert 28/04/2001 #####//
    button_back.deinit();
    // ###### end Gilbert 28/04/2001 #####//
}

static void da_disp_rep_row(int recNo, int x, int y, int refreshFlag) {
    //----- display financial report -----//
    if( refreshFlag == INFO_REPAINT ) {
	vga.use_back();
	//		user_interface.bar(x, y, REPORT_X2-40, y+REC_HEIGHT,SHEET_COLOR);
	//		vga.blt_buf( x, y, REPORT_X2-45, y+REC_HEIGHT-2, 0 );
    }

    if(recNo==1) {
	font_chartsm.put_paragraph
	    (
		x, y, REPORT_X2-45, y+REC_HEIGHT-2,
		"This is a test ............ ...Hello!?"
		);
    }

};

//
//-------- End of functions report/detect/deinit departmental_academic ---------//

//-------- Begin of functions report/detect/deinit student_morale ---------//
//!

void report_student_morale(int refreshFlag) {
    static char* label[]= {
	"Educational quality",
	"Course denials as a percent of total course enrollment",
	"Average factor out-of-class student contact time",
	"Average over teaching methods of (course enrollment/course sections)/normal class size",
    };

    const static int tab1=10;
    const static int tab2=545;
    const static int tab3=580;
    const static int tab4=600;
    const static int tab5=650;
    const static int tab6=700;
    const static int vtab1=40;
    const static int vtab2=200;
    const static int vtab3=225;
    const static int vtab4=360;
    static float v[STUDENT_MORALE_VAR_COUNT];
    static float v2[STUDENT_MORALE_VAR_COUNT];
    static int data_num=HISTORY_TRIMESTER_COUNT;
    static float *dataArray[STUDENT_MORALE_VAR_COUNT];
    static float *dataArrayA;

    //-----communication with sim engine's Output-----//
    int deptRecno = department_array.selected_recno;
    Department* cur_dept_ptr = department_array[deptRecno];

    if(department_array.current_data_mode==MODE_DEPARTMENT) {
	sys.is_institution_report=0;
	dataArrayA=cur_dept_ptr->p_student_morale_history;
	for(int i=0;i<STUDENT_MORALE_VAR_COUNT;i++) {
	    dataArray[i]=cur_dept_ptr->var_student_morale_history[i];
	    v[i]=dataArray[i][data_num-1];
	    v2[i]=cur_dept_ptr->var_change_student_morale[i];
	}
    }
    else {
	sys.is_institution_report=1;
	dataArrayA=department_array.p_student_morale_history;
	for(int i=0;i<STUDENT_MORALE_VAR_COUNT;i++) {
	    // BUGHERE -- wait for Fred's preparation
	    dataArray[i]=cur_dept_ptr->var_student_morale_history[i];
	    v[i]=(float)i;
	    v2[i]=cur_dept_ptr->var_change_student_morale[i];
	}
    }

    vga.use_back();
    if (refreshFlag == INFO_REPAINT) {
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);
	user_interface.brighten(REPORT_X1,REPORT_Y1+vtab1-50,REPORT_X2,REPORT_Y1+vtab1-20);
	user_interface.brighten(REPORT_X1,REPORT_Y1+vtab1-20,REPORT_X2,REPORT_Y1+vtab4+3);
	user_interface.rect(REPORT_X1,REPORT_Y1+vtab1-50,REPORT_X2,REPORT_Y1+vtab1-20);
	user_interface.rect(REPORT_X1,REPORT_Y1+vtab1-20,REPORT_X2,REPORT_Y1+vtab4+3);

	//--------------Initialize buttons-------------------//
	button_back.create_text( REPORT_X1+tab1+300,
				 REPORT_Y2+2, REPORT_X1+tab1+450, REPORT_Y2+22, "Back" );
	//		button_back.set_key( 'B' );

	for(int i=0;i<STUDENT_MORALE_VAR_COUNT;i++) {
	    sm_text[i].put(REPORT_X1+tab2,REPORT_Y1+vtab1+i*38+25,v[i],4);
	    sm_text_chg[i].put(REPORT_X1+tab4,REPORT_Y1+vtab1+i*38+25,v[i],4);
	}

	sm_textA.put(
	    REPORT_X1+tab2,REPORT_Y1+vtab1-40,dataArrayA[data_num-1],4);
	if(info.graph_trimester_passed>4) {
	    sm_text_chgA.put(
		REPORT_X1+tab4,REPORT_Y1+vtab1-40,
		100.0*(dataArrayA[data_num-1]-dataArrayA[data_num-4])/dataArrayA[data_num-4],4);
	    sm_text_chgA.paint();
	}
	//--------------Initialize sm_graph-------------------//
	sm_graphiconA.init(
	    REPORT_X1+tab5,REPORT_Y1+vtab1-50,
	    REPORT_X1+tab6,REPORT_Y1+vtab1-20,
	    1 ,&data_num ,dataArrayA, 1, 0, 3);
	for(int i=0;i<STUDENT_MORALE_VAR_COUNT;i++)
	    sm_graphicon[i].init(
		REPORT_X1+tab5,REPORT_Y1+vtab1+i*38+12,
		REPORT_X1+tab6,REPORT_Y1+vtab1+i*38+47,
		1 ,&data_num ,dataArray[i],1, 0, 3);

	//-------------- Show Text --------------//
	font_chartsm.put_paragraph(REPORT_X1+tab1,REPORT_Y1+vtab1-40,
				   REPORT_X1+tab2,REPORT_Y1+vtab1-20,"S T U D E N T   M O R A L E");
	font_chartsm.put_paragraph(REPORT_X1+tab1,REPORT_Y1+vtab1-15,
				   REPORT_X1+tab2,REPORT_Y1+vtab1+5,"Contributing factors");
	font_chartsm.put_paragraph( REPORT_X1+tab4-30,REPORT_Y1+vtab1-60,
				    REPORT_X1+tab4+100,REPORT_Y1+vtab1-32," ");
	//-------------- Show Text --------------//
	for( int i=0;i<STUDENT_MORALE_VAR_COUNT;i++) {
	    font_chartsm.put_paragraph(REPORT_X1+tab1,REPORT_Y1+vtab1+i*38+20,REPORT_X1+tab2,REPORT_Y1+vtab1+i*38+55,label[i]);
	}

	sm_graphiconA.paint();
	sm_textA.paint();
	for(int i=0;i<STUDENT_MORALE_VAR_COUNT;i++) {
	    sm_graphicon[i].paint();
	    sm_text[i].paint();
	    sm_text_chg[i].paint();
	}
    }
    else {
	sm_graphiconA.refresh();
	sm_textA.refresh();
	sm_text_chgA.refresh();
	for(int i=0;i<STUDENT_MORALE_VAR_COUNT;i++) {
	    sm_graphicon[i].refresh();
	    sm_text[i].refresh();
	    sm_text_chg[i].refresh();
	}
    }

    vga.use_back();
    button_back.paint();

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    vga.use_front();
};

int detect_student_morale() {
    if(button_back.detect()) {
	(*deinit_func_ptr[department_array.current_screen_mode])();
	//-------- post display redraw flag -------//
	sys.redraw_all_flag = 1;
	department_array.current_screen_mode=MODE_DEPT_MAIN;
	return 1;
    }
    return 0;
};

void deinit_student_morale() {
    // ##### begin Gilbert 28/04/2001 #####//
    sm_graphiconA.deinit();
    sm_textA.deinit();
    sm_text_chgA.deinit();
    for(int i=0;i<STUDENT_MORALE_VAR_COUNT;i++) {
	sm_graphicon[i].deinit();
	sm_text_chg[i].deinit();
	sm_text[i].deinit();
    }
    button_back.deinit();
    // ##### end Gilbert 28/04/2001 #####//
}

static void sm_disp_rep_row(int recNo, int x, int y, int refreshFlag) {
    //----- display financial report -----//
    if( refreshFlag == INFO_REPAINT ) {
	vga.use_back();
	//		user_interface.bar(x, y, REPORT_X2-40, y+REC_HEIGHT,SHEET_COLOR);
	//		vga.blt_buf( x, y, REPORT_X2-45, y+REC_HEIGHT-2);
    }

    if(recNo==1) {
	font_chartsm.put_paragraph
	    (
		x, y, REPORT_X2-45, y+REC_HEIGHT-2,
		"This is a test ............ ...Hello!?"
		);
    }

};

//
//-------- End of functions report/detect/deinit student_morale ---------//

//-------- Begin of functions report/detect/deinit faculty_publications ---------//
//!
void report_faculty_research_preformance(int refreshFlag)
{};

int detect_faculty_research_preformance()
{ return 0;};

void deinit_faculty_research_preformance()
{};

static void fp_disp_rep_row(int recNo, int x, int y, int refreshFlag)
{};
//
//-------- End of functions report/detect/deinit FACULTY_RESEARCH_PERFORMANCE ---------//

//-------- Begin of functions report/detect/deinit faculty_morale ---------//
//!
void report_faculty_morale(int refreshFlag)
{};

int detect_faculty_morale() {
    return 0;
};

void deinit_faculty_morale() {

}

static void fm_disp_rep_row(int recNo, int x, int y, int refreshFlag) {
};

//
//-------- End of functions report/detect/deinit faculty_morale ---------//

//-------- Begin of functions report/detect/deinit diversity_index ---------//
//!

void report_diversity_index(int refreshFlag) {
};

int detect_diversity_index() {
    return 0;
};

void deinit_diversity_index()
{};

static void di_disp_rep_row(int recNo, int x, int y, int refreshFlag)
{};

//
//-------- End of functions report/detect/deinit diversity_index ---------//

//-------- Begin of functions report/detect/deinit rank ---------//
//!

void report_rank(int refreshFlag) {
    const static int tab1=REPORT_X1+10;
    const static int tab2=REPORT_X1+450;
    const static int tab3=REPORT_X1+455;
    const static int tab4=REPORT_X1+720;
    const static int vtab1=REPORT_Y1;
    const static int vtab2=REPORT_Y1+335;
    static int report_recno=1;
    static int reccount=department_array.department_count;

    //-----communication with sim engine's Output-----//
    int deptRecno = department_array.selected_recno;
    Department* cur_dept_ptr = department_array[deptRecno];

    //	dataArray=cur_dept_ptr->p_faculty_diversity_history;
    //	v[0]=dataArray[7];

    vga.use_back();
    if (refreshFlag == INFO_REPAINT) {
	//------------- paint the background ----------------//
	user_interface.bg_img(BG_PIC_ID, &vga_back);

	user_interface.brighten(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1-4);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-25,REPORT_X1+630,REPORT_Y1-4,1);
	font_charts.put(REPORT_X1+270,REPORT_Y1-19,"Departmental Performance");

	user_interface.brighten(tab1,vtab1,tab2,vtab2);
	user_interface.brighten(tab3,vtab1,tab4,vtab2);
	user_interface.rect(tab1,vtab1,tab2,vtab2);
	user_interface.rect(tab3,vtab1,tab4,vtab2);
	user_interface.brighten(
	    100,REPORT_Y2-10,
	    250,REPORT_Y2-10+BUTTON_HEIGHT
	    );
	user_interface.brighten(
	    330,REPORT_Y2-10,
	    480,REPORT_Y2-10+BUTTON_HEIGHT
	    );
	user_interface.brighten(
	    550,REPORT_Y2-10,
	    700,REPORT_Y2-10+BUTTON_HEIGHT
	    );

	//--------------Initialize buttons-------------------//

	//		button_back.create_text( tab1+250,
	//			REPORT_Y2+2, tab1+450, REPORT_Y2+22, "Back" );
	//		button_back.set_key( 'B' );

	for(int k=0;k<3;k++) {
	    if(mode_dept_button_up_bitmap[k+12]) {
		mem_del(mode_dept_button_up_bitmap[k+12]);
		mode_dept_button_up_bitmap[k+12] = NULL;
	    }
	    if(mode_dept_button_down_bitmap[k+12]) {
		mem_del(mode_dept_button_down_bitmap[k+12]);
		mode_dept_button_down_bitmap[k+12] = NULL;
	    }
	}

	user_interface.create_button_bitmap(
	    100,REPORT_Y2-10,
	    250,REPORT_Y2-10+BUTTON_HEIGHT,
	    "Institution", &mode_dept_button_up_bitmap[12],
	    &mode_dept_button_down_bitmap[12], &vga_back);

	mode_dept_button[12].create_bitmapW(
	    100,REPORT_Y2-10,
	    250,REPORT_Y2-10+BUTTON_HEIGHT,
	    mode_dept_button_down_bitmap[12]+4);

	user_interface.create_button_bitmap(
	    330,REPORT_Y2-10,
	    480,REPORT_Y2-10+BUTTON_HEIGHT,
	    "Department", &mode_dept_button_up_bitmap[13],
	    &mode_dept_button_down_bitmap[13], &vga_back);

	mode_dept_button[13].create_bitmapW(
	    330,REPORT_Y2-10,
	    480,REPORT_Y2-10+BUTTON_HEIGHT,
	    mode_dept_button_down_bitmap[13]+4);

	user_interface.create_button_bitmap(
	    550,REPORT_Y2-10,
	    700,REPORT_Y2-10+BUTTON_HEIGHT,
	    "Rank by department", &mode_dept_button_up_bitmap[14],
	    &mode_dept_button_down_bitmap[14], &vga_back);

	mode_dept_button[14].create_bitmapW(
	    550,REPORT_Y2-10,
	    700,REPORT_Y2-10+BUTTON_HEIGHT,
	    mode_dept_button_down_bitmap[14]+4);

	int  y;
	int k;
	for(y = vtab1+15,k=0;k<6;k++,y+=BUTTON_HEIGHT+25) {
	    // detail button
	    if(mode_dept_button_up_bitmap[k]) {
		mem_del(mode_dept_button_up_bitmap[k]);
		mode_dept_button_up_bitmap[k] = NULL;
	    }
	    if(mode_dept_button_down_bitmap[k]) {
		mem_del(mode_dept_button_down_bitmap[k]);
		mode_dept_button_down_bitmap[k] = NULL;
	    }

	    user_interface.create_button_bitmap(
		tab3+10,y,tab4-10,y+BUTTON_HEIGHT+10,
		mode_dept_str[k], &mode_dept_button_up_bitmap[k],
		&mode_dept_button_down_bitmap[k], &vga_back);

	    mode_dept_button[k].create_bitmapW(
		tab3+10,y,tab4-10,y+BUTTON_HEIGHT+10,
		((k == department_array.current_pref_report_type-1) ?
		 mode_dept_button_down_bitmap[k] :
		 mode_dept_button_up_bitmap[k])+4);
	}

	//--------------Initialize Vbroswer-------------------//
	reccount=department_array.department_count+1;
	rk_browser.init(tab1,vtab1,tab2,vtab2,
			0, REC_HEIGHT, reccount, rk_disp_rep_row,NULL,0);
	rk_browser.open(report_recno);

	for(k=0;k<3;k++) {
	    mode_dept_button[12+k].paint();
	}

	for(k=0;k<6;k++)
	    mode_dept_button[k].paint();
	mode_dept_button[department_array.current_screen_mode+6].push();
	mode_dept_button[department_array.current_pref_report_type-1].push();

    }
    else {
    }

    report_recno=rk_browser.recno();

    vga.use_back();
    //	button_back.paint();
    rk_browser.update();

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

};

int detect_rank() {
    vga.use_front();
    rk_browser.detect();
    vga.use_back();

    for(int k=0;k<6;k++)
	if(mode_dept_button[k].detect()) {
	    mode_dept_button[k].body_ptr=(void*)(mode_dept_button_down_bitmap[k]+4);
	    mode_dept_button[k].push();

	    (*deinit_func_ptr[department_array.current_screen_mode])();
	    sys.redraw_all_flag = 1;
	    department_array.current_pref_report_type=k+1;
	    return 1;
	}

    for(int k=0;k<3;k++)
	if(mode_dept_button[12+k].detect()) {
	    mode_dept_button[12+k].body_ptr=(void*)(mode_dept_button_down_bitmap[12+k]+4);
	    mode_dept_button[12+k].push();

	    (*deinit_func_ptr[department_array.current_screen_mode])();
	    sys.redraw_all_flag = 1;
	    if((k==0)||(k==1))
		department_array.current_screen_mode=MODE_DEPT_MAIN;
	    department_array.current_data_mode=k;
	    return 1;
	}

    //	if(button_back.detect())
    //	{
    //	  (*deinit_func_ptr[department_array.current_screen_mode])();
    //	  //-------- post display redraw flag -------//
    //	  sys.redraw_all_flag = 1;

    //   return 1;
    //	}
    return 0;
};

void deinit_rank() {
    //--------- deallocate memory -------------//

    // #### begin Gilbert 28/04/2001 ######//
    rk_browser.deinit();
    // #### end Gilbert 28/04/2001 ######//

    if (mode_dept_button_up_bitmap[0]) {
	for(int k=0;k<6;k++) {
	    // ##### begin Gilbert 25/04/2001 #####//
	    mode_dept_button[k].deinit();
	    // ##### end Gilbert 25/04/2001 #####//
	    mem_del(mode_dept_button_up_bitmap[k]);
	    mem_del(mode_dept_button_down_bitmap[k]);
	    mode_dept_button_up_bitmap[k] = NULL;
	    mode_dept_button_down_bitmap[k] = NULL;
	}
    }

    if (mode_dept_button_up_bitmap[6]) {
	for(int k=6;k<12;k++) {
	    // ##### begin Gilbert 25/04/2001 #####//
	    mode_dept_button[k].deinit();
	    // ##### end Gilbert 25/04/2001 #####//
	    mem_del(mode_dept_button_up_bitmap[k]);
	    mem_del(mode_dept_button_down_bitmap[k]);
	    mode_dept_button_up_bitmap[k] = NULL;
	    mode_dept_button_down_bitmap[k] = NULL;
	}
    }

    if (mode_dept_button_up_bitmap[12]) {
	for(int k=12;k<15;k++) {
	    // ##### begin Gilbert 25/04/2001 #####//
	    mode_dept_button[k].deinit();
	    // ##### end Gilbert 25/04/2001 #####//
	    mem_del(mode_dept_button_up_bitmap[k]);
	    mem_del(mode_dept_button_down_bitmap[k]);
	    mode_dept_button_up_bitmap[k] = NULL;
	    mode_dept_button_down_bitmap[k] = NULL;
	}
    }
}

static void rk_disp_rep_row(int recNo, int x, int y, int refreshFlag) {
    //----- display financial report -----//
    if( refreshFlag == INFO_REPAINT ) {
	vga.use_back();

	//		vga.blt_buf( x, y, REPORT_X2-45, y+REC_HEIGHT-2);
    }

    int tab1=35;

  // ###### Begin MArco ##### //
    int tab2=300;
    int tab3=370;
    // ###### End MArco ##### //

  //	user_interface.bar(x, y, x+tab1+tab2+50, y+REC_HEIGHT+2,SHEET_COLOR);

    if(recNo!=1) {
	font_chartsm.put(x+tab1-20, y,recNo-1);
    }

    if(department_array.current_pref_report_type==MODE_DEPT_FACULTY_MORALE) {
	if(recNo==1) {
	    font_charts.put
		( x+tab1, y, "Department");
	    font_charts.put
		( x+tab2-80, y,"Faculty morale");

	    // #### Begin MArco ##### //
	    font_chart_purple_sm.put
		( x+tab3-10, y,"Initial");
	    // ### End MArco ##### //
	}
	else {
	    int this_index=department_array.p_faculty_morale_sort[recNo-2];
	    String tmp=department_array[this_index]->name();
	    tmp.str_buf[tmp.len()-10]='\0';
	    font_chartsm.put_paragraph
		( x+tab1, y, x+tab2,  y+REC_HEIGHT-2,tmp.str_buf);
	    font_chartsm.put_paragraph
		( x+tab2, y, x+tab2+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->p_faculty_morale,4));

	    //##### Begin Marco ##### //
	    font_chart_purple_sm.put_paragraph
		( x+tab3, y, x+tab3+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->initial_p_faculty_morale,4));
	    //##### End MArco ##### //

	}
    }
    if(department_array.current_pref_report_type==MODE_DEPT_FACULTY_DIVERSITY_INDEX) {
	if(recNo==1) {
	    font_charts.put
		( x+tab1, y, "Department");
	    font_charts.put
		( x+tab2-130, y,"Faculty diversity index");

	    // #### Begin MArco ##### //
	    font_chart_purple_sm.put
		( x+tab3-10, y,"Initial");
	    // ### End MArco ##### //
	}
	else {
	    int this_index=department_array.p_faculty_diversity_sort[recNo-2];
	    String tmp=department_array[this_index]->name();
	    tmp.str_buf[tmp.len()-10]='\0';
	    font_chartsm.put_paragraph
		( x+tab1, y, x+tab2,  y+REC_HEIGHT-2,tmp.str_buf);
	    font_chartsm.put_paragraph
		( x+tab2, y, x+tab2+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->p_faculty_diversity,4));

	    // ##### Begin Marco ##### //
	    font_chart_purple_sm.put_paragraph
		( x+tab3, y, x+tab3+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->initial_p_faculty_diversity,4));
	    // ##### End MArco ##### //

	}
    }
    if(department_array.current_pref_report_type==MODE_DEPT_FACULTY_RESEARCH_PERFORMANCE) {
	if(recNo==1) {
	    font_charts.put
		( x+tab1, y, "Department");
	    font_charts.put
		( x+tab2-165, y,"Faculty research performance");

	    // #### Begin MArco ##### //
	    font_chart_purple_sm.put
		( x+tab3-10, y,"Initial");
	    // ### End MArco ##### //

	}
	else {
	    int this_index=department_array.p_faculty_research_sort[recNo-2];
	    String tmp=department_array[this_index]->name();
	    tmp.str_buf[tmp.len()-10]='\0';

	    font_chartsm.put_paragraph
		( x+tab1, y, x+tab2,  y+REC_HEIGHT-2,tmp.str_buf);
	    font_chartsm.put_paragraph
		( x+tab2, y, x+tab2+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->p_faculty_research,4));

	    // ##### Begin Marco ##### //
	    font_chart_purple_sm.put_paragraph
		( x+tab3, y, x+tab3+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->initial_p_faculty_research,4));
	    // ##### End MArco ##### //

	}
    }
    if(department_array.current_pref_report_type==MODE_DEPT_STUDENT_MORALE) {
	if(recNo==1) {
	    font_charts.put
		( x+tab1, y, "Department");
	    font_charts.put
		( x+tab2-80, y,"Student morale");

	    // #### Begin MArco ##### //
	    font_chart_purple_sm.put
		( x+tab3-10, y,"Initial");
	    // ### End MArco ##### //

	}
	else {
	    int this_index=department_array.p_student_morale_sort[recNo-2];
	    String tmp=department_array[this_index]->name();
	    tmp.str_buf[tmp.len()-10]='\0';
	    font_chartsm.put_paragraph
		( x+tab1, y, x+tab2,  y+REC_HEIGHT-2,tmp.str_buf);
	    font_chartsm.put_paragraph
		( x+tab2, y, x+tab2+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->p_student_morale,4));

	    // ##### Begin Marco ##### //
	    font_chart_purple_sm.put_paragraph
		( x+tab3, y, x+tab3+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->initial_p_student_morale,4));
	    // ##### End Marco ##### //

	}
    }
    if(department_array.current_pref_report_type==MODE_DEPT_DEPARTMENTAL_ACADEMIC_STANDING) {
	if(recNo==1) {
	    font_charts.put
		( x+tab1, y, "Department");
	    font_charts.put
		( x+tab2-110, y,"Departmental Prestige");

	    // #### Begin MArco ##### //
	    font_chart_purple_sm.put
		( x+tab3-10, y,"Initial");
	    // ### End MArco ##### //
	}
	else {
	    int this_index=department_array.p_academic_standing_sort[recNo-2];
	    String tmp=department_array[this_index]->name();
	    tmp.str_buf[tmp.len()-10]='\0';
	    font_chartsm.put_paragraph
		( x+tab1, y, x+tab2,  y+REC_HEIGHT-2,tmp.str_buf);
	    font_chartsm.put_paragraph
		( x+tab2, y, x+tab2+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->p_academic_standing,4));

	    // ##### Begin Marco ##### //
	    font_chart_purple_sm.put_paragraph
		( x+tab3, y, x+tab3+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->initial_p_academic_standing,4));
	    // ##### End MArco #### //

	}
    }
    if(department_array.current_pref_report_type==MODE_DEPT_EDUCATIONAL_QUALITY) {
	if(recNo==1) {
	    font_charts.put
		( x+tab1, y, "Department");

	    font_charts.put
		( x+tab2-110, y,"Educational quality");

	    // #### Begin MArco ##### //
	    font_chart_purple_sm.put
		( x+tab3-10, y,"Initial");
	    // ### End MArco ##### //
	}
	else {
	    int this_index=department_array.p_educational_quality_sort[recNo-2];
	    String tmp=department_array[this_index]->name();
	    tmp.str_buf[tmp.len()-10]='\0';
	    font_chartsm.put_paragraph
		( x+tab1, y, x+tab2,  y+REC_HEIGHT-2,tmp.str_buf);
	    font_chartsm.put_paragraph
		( x+tab2, y, x+tab2+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->p_educational_quality,4));

	    // ##### Begin MArco ##### //
	    font_chart_purple_sm.put_paragraph
		( x+tab3, y, x+tab3+50, y+REC_HEIGHT-2,
		  m.format(department_array[this_index]->initial_p_educational_quality,4));
	    // ##### End MArco ##### //
	}
    }
};

//
//-------- End of functions report/detect/deinit rank ---------//

enum{TMP_SCENARIO_COUNT=10};
static char* scn_name[TMP_SCENARIO_COUNT] = {
    "Scenario 1. PAY BETTER",
    "Scenario 2. ALLOCATE NEW MONEY",
    "Scenario 3. TEACH BETTER",
    "Scenario 4. IMPROVE RESEARCH PERFORMANCE",
    "Scenario 5. WIN GAMES",
    "Scenario 6. REDUCE TUITION",
    "Scenario 7. RESPOND TO ENROLLMENT SHIFTS",
    "Scenario 8. ENROLL MORE MINORITY STUDENTS",
    "Scenario 9. HIRE MORE MINORITY FACULTY MEMBERS",
    "Scenario 10. BALANCE THE BUDGET",
};

// begin ## chwg031599
// ##### begin Gilbert 19/04/2001 ###### //
//static short * back_buf_backup;
static short * back_buf_backup = NULL;
// ##### end Gilbert 19/04/2001 ###### //
void Department::print_report() {
    if(back_buf_backup) {
	mem_del(back_buf_backup);
	back_buf_backup = NULL;
    }
    back_buf_backup = vga_back.save_area(
	0,0,VGA_WIDTH-1,VGA_HEIGHT-1,
	back_buf_backup);
    ///////////////////////////////////////////////////////////////////////////////////
    // Part1 Draw six graphs on the back buffer
    int numSeriesData36 = HISTORY_MONTH_COUNT;
    int numSeriesData24 = HISTORY_TRIMESTER_COUNT;
    int numSeriesData8 = HISTORY_YEAR_COUNT;

    float xStart0,xEnd0;
    int xStart1,xEnd1;
    float xStart2,xEnd2;
    float xStart3,xEnd3;
    float xStart4,xEnd4;
    int xStart5,xEnd5;

    float *dataArray1;
    float *dataArray2;
    float *dataArray3;
    float *dataArray4;
    float *dataArray5;
    float *dataArray0;

    int deptRecno = department_array.selected_recno;
    Department* cur_dept_ptr = department_array[deptRecno];

    vga.use_back();
    vga_back.bar(260, 515, 620, 590, V_BLACK );
    font_fblack.put(265,520,"Report printing....");
    vga.blt_buf(260, 515, 620, 590);
    vga_back.bar(260, 515, 620, 590, V_BLACK );

    dataArray0=cur_dept_ptr->p_educational_quality_history;
    dataArray1=cur_dept_ptr->p_academic_standing_history;
    dataArray2=cur_dept_ptr->p_student_morale_history;
    dataArray3=cur_dept_ptr->p_faculty_research_history;
    dataArray4=cur_dept_ptr->p_faculty_morale_history;
    dataArray5=cur_dept_ptr->p_faculty_diversity_history;
    //-----------initial data-----------//

    xEnd0 = float(info.game_year)+float(info.game_month)/12.0f;
    xStart0 = xEnd0-6.0f;
    xEnd1 = (info.game_year);
    xStart1 = (info.game_year-numSeriesData8+1);
    xEnd2 = float(info.game_year)+float(info.game_month)/12.0f;
    xStart2 = xEnd2-6.0f;
    xEnd3 = float(info.game_year)+float(info.game_month)/12.0f;
    xStart3 = xEnd3-3.0f;
    xEnd4 = float(info.game_year)+float(info.game_month)/12.0f;
    xStart4 = xEnd4-3.0f;
    xEnd5 = (info.game_year);
    xStart5 = (info.game_year-numSeriesData8+1);

    vga_back.bar_fast(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,V_WHITE);
    if(!report_init_flag) {
	for(int y = MODE_DEPT_VTAB1+REPORT_Y1+15,i=0,k=0;i<2;i++,y+=BUTTON_VDIST)
	    for(int x = MODE_DEPT_TAB0+REPORT_X1,j=0;j<3;j++,x+=BUTTON_DIST,k++) {
		if(k==0)
		    mode_dept_graph0.init(
			x, y,
			x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			1, &numSeriesData24, dataArray0, 0,
			NULL, NULL, NULL, 1, 1, 1, 3);

		if(k==1)
		    mode_dept_graph1.init(
			x, y,
			x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			1, &numSeriesData8, dataArray1, 0,
			NULL, NULL, NULL, 1, 1, 1, 3);

		if(k==2)
		    mode_dept_graph2.init(
			x, y,
			x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			1, &numSeriesData24, dataArray2, 0,
			NULL, NULL, NULL, 1, 1, 1, 3);

		if(k==3)
		    mode_dept_graph3.init(
			x, y,
			x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			1, &numSeriesData36, dataArray3,
			0,
			NULL, NULL, NULL, 1,
			1, 1, 3);

		if(k==4)
		    mode_dept_graph4.init(
			x, y,
			x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			1, &numSeriesData36, dataArray4,
			0,
			NULL, NULL, NULL, 1, 1, 1, 3);
		if(k==5)
		    mode_dept_graph5.init(
			x, y,
			x+MODE_DEPT_GRAPH_WIDTH, y+MODE_DEPT_GRAPH_HEIGHT,
			1, &numSeriesData8, dataArray5,
			0,
			NULL, NULL, NULL, 1, 1, 1, 3);
	    }
    }

    mode_dept_graph0.paint();
    mode_dept_graph1.paint();
    mode_dept_graph2.paint();
    mode_dept_graph3.paint();
    mode_dept_graph4.paint();
    mode_dept_graph5.paint();
    //---------------//
    for(int y = MODE_DEPT_VTAB1+REPORT_Y1+15,i=0,k=0;i<2;i++,y+=BUTTON_VDIST)
	for(int x = MODE_DEPT_TAB0+REPORT_X1,j=0;j<3;j++,x+=BUTTON_DIST,k++) {
	    font_chartsm.center_put(x,y-BUTTON_HEIGHT+7,x+MODE_DEPT_GRAPH_WIDTH,y-BUTTON_HEIGHT+23,mode_dept_str[k]);
	}
    if(!report_init_flag)
	deinit_main();
    // #### begin Gilbert 28/04/2001 ######//
    mode_dept_graph0.deinit();
    mode_dept_graph1.deinit();
    mode_dept_graph2.deinit();
    mode_dept_graph3.deinit();
    mode_dept_graph4.deinit();
    mode_dept_graph5.deinit();
    // #### end Gilbert 28/04/2001 ######//
    // end of creating bitmap

    // #### begin Gilbert 07/07/2001 ######//
    // vga_back.write_bmp_file("prn001.bmp");
    short *prn001 = vga_back.save_area(
	0,0,VGA_WIDTH-1,VGA_HEIGHT-1,
	NULL);
    // #### end Gilbert 07/07/2001 ######//

    ///////////////////////////////////////////////////////////////////////////////////
    // Part2 Generate trend bitmaps
    // Initization
    int tab1=30,tab2=320,tab3=420,tab4=490;
    int tab5=620,tab6=680,tab7=660,tab8=760;
    int vtab1=22,vtab2=37,vtab3=200,vtab4=350,vtab5=320;
    int data_num=HISTORY_TRIMESTER_COUNT;
    float * dataArray[EDUCATIONAL_QUALITY_VAR_COUNT];
    float * dataArrayA;

    // Read graphicons data
    dataArrayA=cur_dept_ptr->p_educational_quality_history;
    for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++)
	dataArray[i]=cur_dept_ptr->var_educational_quality_history[i];

    // Init graphicons
    if(department_array.current_screen_mode!=MODE_DEPT_EDUCATIONAL_QUALITY) {
	eq_graphiconA.init(
	    REPORT_X1+tab5-5,REPORT_Y1+vtab1-8,
	    REPORT_X1+tab6+5,REPORT_Y1+vtab1+28,
	    1 ,&data_num ,dataArrayA, 1, 0, 3);

	for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++)
	    eq_graphicon[i].init(
		REPORT_X1+tab5,REPORT_Y1+vtab1+i*42+40,
		REPORT_X1+tab6,REPORT_Y1+vtab1+i*42+68,
		1 ,&data_num ,dataArray[i], 1, 0, 3);
    }

    // Drawing bitmap
    vga_back.bar_fast(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,V_WHITE);
    eq_graphiconA.paint();
    for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++)
	eq_graphicon[i].paint();

    // Deinit graphicons
    if(department_array.current_screen_mode!=MODE_DEPT_EDUCATIONAL_QUALITY) {
	eq_graphiconA.deinit();
	for(int i=0;i<EDUCATIONAL_QUALITY_VAR_COUNT;i++)
	    eq_graphicon[i].deinit();
    }
    // Write bitmap
    // #### begin Gilbert 07/07/2001 ######//
    // vga_back.write_bmp_file("prn002.bmp");
    short *prn002 = vga_back.save_area(
	0,0,VGA_WIDTH-1,VGA_HEIGHT-1,
	NULL);
    // #### end Gilbert 07/07/2001 ######//
    ///////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////
    // Part3 Generate trend bitmaps
    // Initization
    data_num=HISTORY_YEAR_COUNT;
    float *as_dataArray[ACADEMIC_STANDING_VAR_COUNT];
    float *as_dataArrayA;

    // Read graphicons data
    as_dataArrayA=cur_dept_ptr->p_academic_standing_history;
    for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++)
	as_dataArray[i]=cur_dept_ptr->var_academic_standing_history[i];

    // Init graphicons
    if(department_array.current_screen_mode!=ACADEMIC_STANDING_VAR_COUNT) {
	as_graphiconA.init(
	    REPORT_X1+tab5,REPORT_Y1+vtab1-8,
	    REPORT_X1+tab6,REPORT_Y1+vtab1+28,
	    1 ,&data_num ,as_dataArrayA, 1, 0, 3);

	for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++)
	    as_graphicon[i].init(
		REPORT_X1+tab5-5,REPORT_Y1+vtab1+i*56+40,
		REPORT_X1+tab6+5,REPORT_Y1+vtab1+i*56+68,
		1 ,&data_num ,as_dataArray[i], 1, 0, 3);
    }

    // Drawing bitmap
    vga_back.bar_fast(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,V_WHITE);
    as_graphiconA.paint();
    for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++)
	as_graphicon[i].paint();

    // Deinit graphicons
    if(department_array.current_screen_mode!=ACADEMIC_STANDING_VAR_COUNT) {
	as_graphiconA.deinit();
	for(int i=0;i<ACADEMIC_STANDING_VAR_COUNT;i++)
	    as_graphicon[i].deinit();
    }
    // Write bitmap
    // #### begin Gilbert 07/07/2001 ######//
    // vga_back.write_bmp_file("prn003.bmp");
    short *prn003 = vga_back.save_area(
	0,0,VGA_WIDTH-1,VGA_HEIGHT-1,
	NULL);
    // #### end Gilbert 07/07/2001 ######//

    ///////////////////////////////////////////////////////////////////////////////////

    // ##### begin Gilbert 07/07/2001 ###### //
    // restore later after printing
    //vga_back.rest_area(back_buf_backup,1,0);
    //back_buf_backup = NULL;
    // ##### end Gilbert 07/07/2001 ###### //

    // Print here
    {
	MouseDispCount mdc;
	//First page printing
	//*********************start printing********************

	print.start_print(sys.main_hwnd,sys.app_hinstance);
	print.open_page();

	//this will print the bmp in the center of the paper and 1/7 down the page
	// ##### begin Gilbert 07/07/2001 ######//
	vga_back.rest_area( prn001, 0 );
	/*BMPWIDTH*/
	print.put_vga_back("prn001.bmp",696,342 /*BMPHEIGHT*/,
			   0.12,0.142,
			   54,71);                                     /*upper left cor. of bitmap*/
	// ##### end Gilbert 07/07/2001 ######//

	//useage of dfont
	//dfont(int PointSize,bool italic,bool under,int weight [bold=700,normal=0,max=1000])
	//have to set the font before each print

	print.dfont(18,false,true,700);
	print.put_text("Virtual U Department Performance Detail Report",6,1);

	String str1;
	str1 = "Department : ";
	str1 += cur_dept_ptr->name();
	str1 += "                                                                                                                              ";
	print.dfont(12,false,false,0);
	print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",10,6);
	print.dfont(12,false,true,0);
	print.put_text(str1.str_buf,10,7);

	print.dfont(9,false,false,0);
	print.put_text("Insitution : ",20,5);
	//Player Institution
	print.put_text(player_school.school_name,34,5);
	print.put_text("President : ",20,6);
	String str;
	str =  player_school.player_first_name;
	str += " ";
	str += player_school.player_last_name;
	print.put_text(str.str_buf,34,6);             //President
	print.put_text("Date:",79,6);
	// print.put_text(date.date_str(info.game_date),86,6);                   //date
	//date
	print.put_text( info.date_str_julian(info.game_date),86,6);

	//		print.put_text("Department:",10,10);
	//		print.put_text(cur_dept_ptr->name(),23,10);        //Department

	print.dfont(9,false,false,700);
	if(player_school.scenario_id>0)
	    print.put_text(scn_name[player_school.scenario_id-1],35,8);
	//		print.put_text("Balance the Budget",44,8);                //scenario

	print.dfont(9,false,true,0);
	print.put_text("Contributors                                                                                                                                                                        ",10,34);
	print.put_text("Contributors                                                                                                                                                                       ",10,54);

	print.dfont(9,false,false,0);
	print.put_text("                                                                                       Current value   Weight   Last trimester change   Trend",10,32);
	print.put_text("                                                                                       Current value   Weight   Last year change          Trend",10,52);

	//educational Current value
	print.put_text(m.format(dataArrayA[HISTORY_TRIMESTER_COUNT-1],1),69,33);
	float eq_chg;
	if(info.graph_trimester_passed>1) {
	    eq_chg=(float)(100.0*((double)dataArrayA[HISTORY_TRIMESTER_COUNT-1]
				  -dataArrayA[HISTORY_TRIMESTER_COUNT-2])/
			   dataArrayA[HISTORY_TRIMESTER_COUNT-2]);
	    if((eq_chg<0.0)&&(eq_chg>-0.01))
		eq_chg=0.0;
	}
	else
	    eq_chg=0.0;
	print.put_text(m.format(eq_chg,1),95,33);     //educational last trimester change trend

	//department Current value
	print.put_text(m.format(as_dataArrayA[HISTORY_YEAR_COUNT-1],1),69,53);

	float da_chg;
	if(info.graph_trimester_passed>1) {
	    da_chg=(float)
		(100.0*((double)as_dataArrayA[HISTORY_YEAR_COUNT-1]
			-as_dataArrayA[HISTORY_YEAR_COUNT-2])
		 /as_dataArrayA[HISTORY_YEAR_COUNT-2]);
	    if((da_chg<0.0)&&(da_chg>-0.01))
		da_chg=0.0;
	}
	else
	    da_chg=0.0;
	print.put_text(m.format(da_chg,1),95,53);     //department last year change

	print.dfont(9,false,false,0);
	print.put_text("Faculty teaching performance rating",10,36);
	//educational Current value
	print.put_text(m.format(dataArray[0][HISTORY_TRIMESTER_COUNT-1],1),69,36);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_educational_quality_weight[0],1),82,36);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_educational_quality[0],1),95,36);

	print.put_text("Faculty education development time (hours per week)",10,38);
	//educational Current value
	print.put_text(m.format(dataArray[1][HISTORY_TRIMESTER_COUNT-1],1),69,38);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_educational_quality_weight[1],1),82,38);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_educational_quality[1],1),95,38);

	print.put_text("Faculty scholarship performance rating",10,40);
	//educational Current value
	print.put_text(m.format(dataArray[2][HISTORY_TRIMESTER_COUNT-1],1),69,40);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_educational_quality_weight[2],1),82,40);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_educational_quality[2],1),95,40);

	print.put_text("Faculty research performance",10,42);
	//educational Current value
	print.put_text(m.format(dataArray[3][HISTORY_TRIMESTER_COUNT-1],1),69,42);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_educational_quality_weight[3],1),82,42);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_educational_quality[3],1),95,42);

	print.put_text("Use of information in teaching",10,44);
	//educational Current value
	print.put_text(m.format(dataArray[4][HISTORY_TRIMESTER_COUNT-1],1),69,44);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_educational_quality_weight[4],1),82,44);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_educational_quality[4],1),95,44);

	print.put_text("Deviation of class size from its norm",10,46);
	//educational Current value
	print.put_text(m.format(dataArray[5][HISTORY_TRIMESTER_COUNT-1],1),69,46);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_educational_quality_weight[5],1),82,46);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_educational_quality[5],1),95,46);

	print.put_text("Deviation of teaching method mix from its norm",10,48);
	//educational Current value
	print.put_text(m.format(dataArray[6][HISTORY_TRIMESTER_COUNT-1],1),69,48);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_educational_quality_weight[6],1),82,48);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_educational_quality[6],1),95,48);

	print.dfont(9,false,false,0);
	print.put_text("Faculty research performance",10,56);
	//educational Current value
	print.put_text(m.format(as_dataArray[0][HISTORY_YEAR_COUNT-1],1),69,56);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_academic_standing_weight[0],1),82,56);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_academic_standing[0],1),95,56);

	print.put_text("Sponsored research",10,58);
	//educational Current value
	print.put_text(m.format(as_dataArray[1][HISTORY_YEAR_COUNT-1],1),69,58);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_academic_standing_weight[1],1),82,58);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_academic_standing[1],1),95,58);

	print.put_text("Quality rating for doctoral students",10,60);
	//educational Current value
	print.put_text(m.format(as_dataArray[2][HISTORY_YEAR_COUNT-1],1),69,60);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_academic_standing_weight[2],1),82,60);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_academic_standing[2],1),95,60);

	print.put_text("Number of doctoral students per regular factulty",10,62);
	//educational Current value
	print.put_text(m.format(as_dataArray[3][HISTORY_YEAR_COUNT-1],1),69,62);
	//educational weight
	print.put_text(m.format(cur_dept_ptr->var_academic_standing_weight[3],1),82,62);
	//educational last trimester change trend
	print.put_text(m.format(cur_dept_ptr->var_change_academic_standing[3],1),95,62);

	// ##### begin Gilbert 07/07/2001 ######//
	vga_back.rest_area( prn002, 0 );
	//trend1 bmp  education
	print.put_vga_back("prn002.bmp",56,25,0.685,0.418,671,106);
	//trend2 bmp
	print.put_vga_back("prn002.bmp",56,25,0.685,0.448,671,148);
	//trend3 bmp
	print.put_vga_back("prn002.bmp",56,25,0.685,0.473,671,190);
	//trend4 bmp
	print.put_vga_back("prn002.bmp",56,25,0.685,0.498,671,232);
	//trend5 bmp
	print.put_vga_back("prn002.bmp",56,25,0.685,0.523,671,274);
	//trend6 bmp
	print.put_vga_back("prn002.bmp",56,25,0.685,0.548,671,316);
	//trend7 bmp
	print.put_vga_back("prn002.bmp",56,25,0.685,0.573,671,358);
	//trend8 bmp
	print.put_vga_back("prn002.bmp",56,25,0.685,0.598,671,400);

	vga_back.rest_area( prn003, 0 );
	//trend9 bmp   departmental
	print.put_vga_back("prn003.bmp",56,25,0.685,0.668,671,105);
	//trend10 bmp
	print.put_vga_back("prn003.bmp",56,25,0.685,0.698,671,148);
	//trend11 bmp
	print.put_vga_back("prn003.bmp",56,25,0.685,0.723,671,204);
	//trend12 bmp
	print.put_vga_back("prn003.bmp",56,25,0.685,0.748,671,260);
	//trend13 bmp
	print.put_vga_back("prn003.bmp",56,25,0.685,0.773,671,316);
	// ##### end Gilbert 07/07/2001 ######//

	print.dfont(10,false,false,700);
	print.put_text("Educational Quality",8,29);
	print.put_text("Departmental Quality",8,47);

	/*		print.dfont(9,false,false,0);
			print.put_text(m.format(as_dataArray[3][HISTORY_YEAR_COUNT-1],1),69,62);             //educational Current value
			print.put_text(m.format(cur_dept_ptr->var_change_academic_standing[3],1),95,62);     //educational last trimester change trend
			print.put_text(m.format(dataArray[6][HISTORY_TRIMESTER_COUNT-1],1),69,48);                               //educational Current value
			print.put_text(m.format(cur_dept_ptr->var_change_educational_quality[6],1),95,48);                               //educational last trimester change trend
	*/
	print.end_page();

	print.endprint(sys.main_hwnd);

	//***********************end printing***********************
	//		print.PrintBmp(sys.main_hwnd,256,256);
    }

    // ##### begin Gilbert 07/07/2001 ###### //
    mem_del(prn003);
    mem_del(prn002);
    mem_del(prn001);

    vga_back.rest_area(back_buf_backup,1,0);
    back_buf_backup = NULL;
    // ##### end Gilbert 07/07/2001 ###### //

    sys.redraw_all_flag=1;
}

// end ## chwg031599
