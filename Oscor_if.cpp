//Filename    : OSCOR_IF.CPP
//Description : Score Report

#include <KEY.H>
#include <OVGA.H>
#include <OFONT.H>
#include <OWORLD.H>
#include <OSYS.H>
#include <OSCHLRES.H>
#include <OPEERSCH.H>                             // enrollment
#include <OINFO.H>
#include <OGRPHYR.H>
#include <OIFACE.H>
#include <OBUTTON.H>
#include <ODEPT.H>
#include <OPSCHOOL.H>
#include <OSTUDENT.H>
#include <OTEXT.H>
#include <OLgTEXT.H>
#include <OGAMESTR.H>
#include <OPIECHRT.H>
#include <OGRPH_PM.H>
#include <OHELPIF.H>
#include <OVBROW98.H>
#include <OAUDIO.H>
#include <OCONFIG.H>
#include <OTASK.H>                                //## chea 200899 added task

//---------- Define constant ------------//

enum {
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+10,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-40,
};

enum {
    BOTTOM_BUTTON_WIDTH = 100,
    BOTTOM_BUTTON_WIDTH2 = 150,
    BOTTOM_BUTTON_WIDTH3 = 220,
    BOTTOM_BUTTON_HEIGHT = 25,
    BOTTOM_BUTTON_DISTANCE
    = (REPORT_X2-REPORT_X1-BOTTOM_BUTTON_WIDTH*6)/6+30  ,
    BOTTOM_BUTTON_DISTANCE2
    = (REPORT_X2-REPORT_X1-BOTTOM_BUTTON_WIDTH2*2)/24,
    BOTTOM_BUTTON_DISTANCE3
    = (REPORT_X2-REPORT_X1-BOTTOM_BUTTON_WIDTH3*3)/3-5
};

enum {
    WINDOW_X1 = REPORT_X1+10,
    WINDOW_Y1 = REPORT_Y1+16,
    WINDOW_X2 = REPORT_X2-10,
    //			WINDOW_Y2 = REPORT_Y2-44,
    WINDOW_Y2 = REPORT_Y2-52,
};

enum {                                            // the bg image id
    BG_PIC_ID = 10
};

enum {
    MODE_OVERALL=0,
    MODE_RANK=1,
    MODE_OUTPUTS=2,
    MODE_GENERAL_INSTITUTIONAL_PERFORMANCE=3,
    MODE_ATTITUDES_TOWARD_THE_INSTITUTION=4,
    MODE_FINANCE=5,
    MODE_MANAGEMENT=6,
    MODE_ADMISSION=7,
};

static char     report_init_flag=0;

static ButtonGroup type_button_group(8);

static Button help_button_array[9];

static short *type_button_up_bitmap[8];

static short *type_button_down_bitmap[8];

static char  *type_button_label[8]= {
    "Overall","Rank","Outputs",
    "Performance indicators",
    "Attitudes toward the institution",
    "Financial indicators","Management","Admission"
};

static void report_overall(int);
static void report_rank(int);
static void report_outputs(int);
static void report_general(int);
static void report_attitudes(int);
static void report_management(int);
static void report_finance(int);

static int detect_overall();
static int detect_rank();
static int detect_outputs();
static int detect_general();
static int detect_attitudes();
static int detect_management();
static int detect_finance();

static void deinit_overall();
static void deinit_rank();
static void deinit_outputs();
static void deinit_general();
static void deinit_attitudes();
static void deinit_management();
static void deinit_finance();

static void overall_disp_rep_row(int recNo, int x, int y, int refreshFlag);
static void rank_disp_rep_row(int recNo, int x, int y, int refreshFlag);

typedef void (*ReportFP)(int refreshFlag);
typedef int (*DetectFP)();
typedef void (*DeinitFP)();

static ReportFP report_func_ptr[] = {
    &report_overall,
    &report_rank,
    &report_outputs,
    &report_general,
    &report_attitudes,
    //	&report_management,
    &report_finance,
};

static DetectFP detect_func_ptr[] = {
    &detect_overall,
    &detect_rank,
    &detect_outputs,
    &detect_general,
    &detect_attitudes,
    //	&detect_management,
    &detect_finance,
};

static DeinitFP deinit_func_ptr[] = {
    &deinit_overall,
    &deinit_rank,
    &deinit_outputs,
    &deinit_general,
    &deinit_attitudes,
    //	&deinit_management,
    &deinit_finance,
};

void PlayerSchool::score_report(int refreshFlag) {
    const static int tab1=10,tab2=150,tab3=320,tab4=420,tab5=490,tab6=620,tab7=680;
    const static int vtab1=22,vtab2=37,vtab3=200,vtab4=330,vtab5=320;

    vga.use_back();
    sys.is_institution_report=1;
    if( refreshFlag == INFO_REPAINT ) {

	if( report_init_flag )                        // deinit it first if it has already been initialized
	    score_report_deinit();
	report_init_flag = 1;

	//----- initialize the bottom sub-menus buttons -----//

	user_interface.bg_img(BG_PIC_ID, &vga_back);

	// top title
	user_interface.brighten(REPORT_X1+100,REPORT_Y1-10,REPORT_X1+630,REPORT_Y1+13);
	user_interface.rect(REPORT_X1+100,REPORT_Y1-10,REPORT_X1+630,REPORT_Y1+13,1);
	font_charts.put(REPORT_X1+300,REPORT_Y1-4,"Game Score");
	int i;
	for (i = 0; i < 6 ;i++) {
	    type_button_up_bitmap[i] = NULL;
	    type_button_down_bitmap[i] = NULL;
	}

	int x=REPORT_X1+tab1;
	int y=REPORT_Y1+vtab4;

	user_interface.create_button_bitmap(x, y,
					    x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					    type_button_label[0], &type_button_up_bitmap[0],
					    &type_button_down_bitmap[0], &vga_back);

	type_button_group[0].create_bitmapW(x, y,
					    x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					    ((i == current_score_report_type) ?
					     type_button_down_bitmap[0] :
					     type_button_up_bitmap[0])+4);

	y+=30;

	user_interface.create_button_bitmap(x, y,
					    x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					    type_button_label[1], &type_button_up_bitmap[1],
					    &type_button_down_bitmap[1], &vga_back);

	type_button_group[1].create_bitmapW(x, y,
					    x+BOTTOM_BUTTON_WIDTH, y+BOTTOM_BUTTON_HEIGHT,
					    ((i == current_score_report_type) ?
					     type_button_down_bitmap[1] :
					     type_button_up_bitmap[1])+4);

	x=REPORT_X1+tab2;
	y=REPORT_Y1+vtab4;

	user_interface.create_button_bitmap(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    type_button_label[2], &type_button_up_bitmap[2],
					    &type_button_down_bitmap[2], &vga_back);
	type_button_group[2].create_bitmapW(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    ((i == current_score_report_type) ?
					     type_button_down_bitmap[2] :
					     type_button_up_bitmap[2])+4);

	x+=BOTTOM_BUTTON_WIDTH3+BOTTOM_BUTTON_DISTANCE;

	user_interface.create_button_bitmap(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    type_button_label[3], &type_button_up_bitmap[3],
					    &type_button_down_bitmap[3], &vga_back);
	type_button_group[3].create_bitmapW(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    ((i == current_score_report_type) ?
					     type_button_down_bitmap[3] :
					     type_button_up_bitmap[3])+4);

	x=REPORT_X1+tab2;
	y=REPORT_Y1+vtab4+30;
	user_interface.create_button_bitmap(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    type_button_label[4], &type_button_up_bitmap[4],
					    &type_button_down_bitmap[4], &vga_back);

	type_button_group[4].create_bitmapW(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    ((current_score_report_type==4)?
					     type_button_down_bitmap[4] :
					     type_button_up_bitmap[4])+4);

	x+=BOTTOM_BUTTON_WIDTH3+BOTTOM_BUTTON_DISTANCE;

	user_interface.create_button_bitmap(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    type_button_label[5], &type_button_up_bitmap[5],
					    &type_button_down_bitmap[5], &vga_back);

	type_button_group[5].create_bitmapW(x, y,
					    x+BOTTOM_BUTTON_WIDTH3, y+BOTTOM_BUTTON_HEIGHT,
					    ((current_score_report_type==5)?
					     type_button_down_bitmap[5] :
					     type_button_up_bitmap[5])+4);

	if(current_score_report_type>1) {
	    user_interface.brighten(WINDOW_X1,WINDOW_Y1,WINDOW_X2,WINDOW_Y2);
	    user_interface.rect(WINDOW_X1,WINDOW_Y1,WINDOW_X2,WINDOW_Y2);
	    user_interface.rect(WINDOW_X1,WINDOW_Y1,WINDOW_X2,WINDOW_Y1+vtab1);
	    font_chartsm.put(WINDOW_X1+tab1,WINDOW_Y1+vtab1-17,"Contributing factors");
	    font_chartsm.put(WINDOW_X1+tab3-100,WINDOW_Y1+vtab1-17,"Current value");
	    font_chartsm.put(WINDOW_X1+tab4-95,WINDOW_Y1+vtab1-17,"Weight");
	    font_chartsm.right_put(WINDOW_X1+tab5, WINDOW_Y1+vtab1-17,"Change in last year");
	    font_chartsm.put(WINDOW_X1+tab5+10,WINDOW_Y1+vtab1-17,"Change since start");
	    font_chartsm.put(WINDOW_X1+tab6,WINDOW_Y1+vtab1-17,"Trend");
	}
	else {
	    user_interface.brighten(WINDOW_X1,WINDOW_Y1,WINDOW_X2,WINDOW_Y2);
	    user_interface.rect(WINDOW_X1,WINDOW_Y1,WINDOW_X2,WINDOW_Y2);
	}
    }
    type_button_group.paint(current_score_report_type);

    vga.use_back();
    (*report_func_ptr[current_score_report_type])(refreshFlag);

    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    //##trevor 2807
    vga.use_front();

}

void PlayerSchool::score_report_detect() {
    //------- detect type buttons --------//
    int summaryButtonGroupDetect;

    if((summaryButtonGroupDetect = type_button_group.detect()) >= 0) {
	type_button_group[current_score_report_type].body_ptr = (void *) (type_button_up_bitmap[current_score_report_type] + 4);
	current_score_report_type = summaryButtonGroupDetect;
	type_button_group[current_score_report_type].body_ptr = (void *) (type_button_down_bitmap[current_score_report_type] + 4);
	sys.redraw_all_flag = 1;
    }
    (*detect_func_ptr[current_score_report_type])();
}

void PlayerSchool::score_report_deinit() {
    //------- deinit type buttons --------//
    type_button_group.deinit_buttons();
    if (*type_button_up_bitmap) {
	for (int i = 0; i < 6; i++) {
	    mem_del(type_button_up_bitmap[i]);
	    mem_del(type_button_down_bitmap[i]);
	    type_button_up_bitmap[i] = NULL;
	    type_button_down_bitmap[i] = NULL;
	}
    }
    for(int i=0;i<6;i++)
	(*deinit_func_ptr[i])();

    report_init_flag = 0;
}

//--------------------- sub screen (7) ----------------------//

static int overall_report_init_flag=0;
//static LargeText overall_text[5];
static LargeText overall_text[6];                 //## chea 200899 to added a bouns pt
static GraphPerformanceIndicatorMonth overall_graphicon1;
static GraphPerformanceIndicatorMonth overall_graphicon2;

static void report_overall(int refreshFlag) {
    const static int tab1=10,tab2=450,tab3=560,tab4=680;
    const static int vtab1=40,vtab2=135,vtab3=110,vtab4=285;
    char *label[]= {
	"\tCurrent game score",
	"\tLast month change",
	"\tTrustee evaluation at the beginning of the game",
	"\tNumber of gaming years",
	// maintenance upgrade August 2001
	"\tCumulative game score",                    // "\tGame score",
	"\t=    (Current game score - Initial trustee evaluation) X 10",
	"\t      + Number of gaming years",           //## chea 200899 to added a bouns pt
	"\t      + Total bonus points",               //## chea 200899 to added a bouns pt
	"\t=   ",
    };

    static int data_num=HISTORY_MONTH_COUNT;
    static float *dataArray;
    static int *dataArray2;
    static float v[6];

    v[0]=player_school.cur_game_score;
    //### begin zb 02/22/99
    v[1]=(float)( (int)player_school.game_score_history[HISTORY_MONTH_COUNT-1]-
		  (int)player_school.game_score_history[HISTORY_MONTH_COUNT-2] );
    //### end zb 02/22/99
    v[2]=player_school.init_game_score;
    //	v[2]=(float)player_school.ultimate_game_score;
    v[3]=(float)info.year_passed-1;
    v[4]=(float)player_school.ultimate_game_score;
    static float totalBonusPoint;
    totalBonusPoint = task_array.total_bonus_pt + player_school.bonus_point + player_school.threshold_event_bonus;
    dataArray=player_school.game_score_history;
    dataArray2=player_school.ultimate_game_score_history;
    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( overall_report_init_flag )                // deinit it first if it has already been initialized
	    deinit_overall();
	overall_report_init_flag = 1;

	overall_graphicon1.init(
	    WINDOW_X1+tab3-30,WINDOW_Y1+50,
	    WINDOW_X1+tab4-30,WINDOW_Y1+123,
	    1 ,&data_num ,dataArray, 1, 0, 3);
	overall_graphicon1.paint();

	overall_graphicon2.init(
	    WINDOW_X1+tab3-30,WINDOW_Y1+170,
	    WINDOW_X1+tab4-30,WINDOW_Y1+243,
	    1 ,&data_num ,dataArray2, 1, 0, 3);
	overall_graphicon2.paint();

	font_chartsm.put( WINDOW_X1+tab3-123,WINDOW_Y1+5, "Change in last year" );
	font_chartsm.put( WINDOW_X1+tab3+5,WINDOW_Y1+5, "Change in since start" );

	float zn = player_school.get_protagonist_info(1)->normalizing_factor;
	// 1 year before
	float trend = (v[0] - player_school.game_score_history[THIS_MONTH-12]) / zn;
	float level = (v[0] - player_school.init_game_score) / zn;

	font_chartsm.put( WINDOW_X1+tab3-88, WINDOW_Y1+5+5+vtab1-25, m.format(trend,24) );
	font_chartsm.put( WINDOW_X1+tab3+45, WINDOW_Y1+5+5+vtab1-25, m.format(level,24) );

	for(int i=0;i<4;i++)
	    overall_text[i].right_put
		(WINDOW_X1+tab2-25,WINDOW_Y1+vtab1+(i-1)*25+5+5,(float*)(&v[i]),4);

	overall_text[4].right_put
	    (WINDOW_X1+tab2-25,WINDOW_Y1+vtab1+(3)*25+5,&totalBonusPoint,4);

	//		overall_text[4].put  //## chea 200899 to added a bouns pt
	//   		(WINDOW_X1+tab1+70,WINDOW_Y1+vtab2+140,(float*)(&v[4]),4);
	overall_text[5].put
	    (WINDOW_X1+tab1+70,WINDOW_Y1+vtab2+125,(float*)(&v[4]),4);

    }

    user_interface.bar(WINDOW_X1+tab1,WINDOW_Y1+vtab1+105,WINDOW_X1+tab4,WINDOW_Y1+vtab1+106,V_BROWN);

    for(int i=0;i<4;i++)
	//font_charts.put(WINDOW_X1+tab1,WINDOW_Y1+vtab1+(i-1)*35+5,label[i]);
	//## chea 200899 to added a bouns pt
	font_charts.put(WINDOW_X1+tab1,WINDOW_Y1+vtab1+(i-1)*25+5+5,label[i]);

    //## chea 200899 to added a bouns pt
    font_charts.put(WINDOW_X1+tab1,WINDOW_Y1+vtab1+(3)*25+5+5,"\tTotal bonus points");

    //	for(i=4;i<8;i++)
    for(int i=4;i<9;i++)                                //## chea 200899 to added a bouns pt
	font_charts.put(WINDOW_X1+tab1,WINDOW_Y1+vtab2+(i-3)*25,label[i]);

    overall_graphicon1.refresh();
    overall_graphicon2.refresh();
    // ##### begin Gilbert 25/04/2001 #######//
    for(int i=0;i<6;i++)                                // for(i=0;i<5;i++)
	overall_text[i].refresh();
    // ##### end Gilbert 25/04/2001 #######//
};

static int detect_overall() {
    //	overall_vbrowse.detect();
    return 0;
};

static void deinit_overall() {
    if(overall_report_init_flag==0)return;
    overall_graphicon1.deinit();
    overall_graphicon2.deinit();
    // ##### begin Gilbert 25/04/2001 #######//
    for(int i=0;i<6;i++)                            // for(int i=0;i<5;i++)
	overall_text[i].deinit();
    overall_report_init_flag=0;
    // ##### end Gilbert 25/04/2001 #######//
};

static void overall_disp_rep_row(int recNo, int x, int y, int refreshFlag) {
};

#define REC_HEIGHT 16
#define SHEET_COLOR Vga::active_buf->color_down

static int rank_report_init_flag=0;
static short* rank_text_refresh_bitmap1;
static GraphPerformanceIndicatorMonth rank_graphicon;
static VBrowse98 rank_vbrowse;

//### begin zb 02/22/99
static int sort_peer_school_score_function( const void *a, const void *b ) {
    float valueA = school_res.peer_school_array[(*((short*)a))].game_score;
    float valueB = school_res.peer_school_array[(*((short*)b))].game_score;

    float rc = valueA - valueB;

    if( rc > 0 )
	return -1;
    else if( rc <= 0 )
	return 1;
    else
	return 0;
}

//### end zb 02/22/99

static void report_rank(int refreshFlag) {
    const static int tab1=10,tab2=450,tab3=560,tab4=680;
    const static int vtab1=20,vtab2=105,vtab3=110,vtab4=285;
    static int report_recno=1;

    vga.use_back();

    //### begin zb 02/22/99
    if((int)school_res.player_peer_school->game_score!=(int)player_school.cur_game_score) {
	school_res.player_peer_school->game_score=player_school.cur_game_score;
	qsort( school_res.peer_school_rank, school_res.peer_school_count, sizeof(school_res.peer_school_rank[0]), sort_peer_school_score_function );
    }
    //### end zb 02/22/99

    if( refreshFlag == INFO_REPAINT ) {
	if( rank_report_init_flag )                   // deinit it first if it has already been initialized
	    deinit_rank();
	rank_report_init_flag = 1;

	rank_vbrowse.init(
	    WINDOW_X1+tab1,WINDOW_Y1+vtab1+10,
	    WINDOW_X2-tab1,WINDOW_Y1+vtab4,
	    0, REC_HEIGHT, school_res.peer_school_count,
	    rank_disp_rep_row,NULL,0);

	rank_vbrowse.open(report_recno);
    }
    font_chartsm.put(WINDOW_X1+200,WINDOW_Y1+vtab1+10,"");
    rank_vbrowse.update();
    report_recno=rank_vbrowse.recno();
};

static int detect_rank() {
    rank_vbrowse.detect();
    return 0;
};

static void deinit_rank() {
    if(rank_report_init_flag==0)return;
    // ####### begin Gilbert 25/04/2001 ######//
    rank_vbrowse.deinit();
    rank_report_init_flag = 0;
    // ####### end Gilbert 25/04/2001 ######//
};

static void rank_disp_rep_row(int recNo, int x, int y, int refreshFlag) {
    int tab1=10,tab2=60,tab3=400,tab4=600;

    //	vga_back.bar_down(x,y,x+tab4,y+20);

    if(recNo==1) {
	font_chartsm.put(x+tab1,y,"Rank");
	font_chartsm.put(x+tab2,y,"University");
	font_chartsm.put(x+tab3,y,"Current trustee evaluation");
	return;
    }
    else {
	int idx = school_res.peer_school_rank[recNo-2];

	PeerSchool *ps = &(school_res.peer_school_array[idx]);
	if(ps==school_res.player_peer_school) {
	    font_chart_blue_sm.right_put(x+tab1+20,y,m.format(recNo-1,4));
	    font_chart_blue_sm.put(x+tab2,y,ps->name);
	    font_chart_blue_sm.right_put(x+tab3+70,y,m.format((int)ps->game_score,4));
	}
	else {
	    font_chartsm.right_put(x+tab1+20,y,m.format(recNo-1,4));
	    font_chartsm.put(x+tab2,y,ps->name);
	    font_chartsm.right_put(x+tab3+70,y,m.format((int)ps->game_score,4));
	}
    }

};

//-- screen 2 : outputs ---/
#define OUTPUTS_ITEMS_NUM 2
static int outputs_report_init_flag=0;
static GraphPerformanceIndicatorMonth outputs_graphicon[OUTPUTS_ITEMS_NUM];
static Text outputs_text1[OUTPUTS_ITEMS_NUM];
static Text outputs_text2[OUTPUTS_ITEMS_NUM];
static Text outputs_text3[OUTPUTS_ITEMS_NUM];
static Text outputs_text4[OUTPUTS_ITEMS_NUM];

static char * outputs_help_title[OUTPUTS_ITEMS_NUM]= {
    "Degrees granted",
    // maintenance upgrade August 2001
    // "Sponsored performance"
    "Faculty research performance"
};

static void report_outputs(int refreshFlag) {
    const static int tab1=30,tab2=320,tab3=420,tab4=490;
    const static int tab5=620,tab6=680,tab7=660,tab8=760;
    const static int vtab1=22,vtab2=37,vtab3=200,vtab4=330,vtab5=320;

    static int data_num=HISTORY_MONTH_COUNT;
    static float *dataArray[OUTPUTS_ITEMS_NUM];
    static float v1[OUTPUTS_ITEMS_NUM];
    static float v2[OUTPUTS_ITEMS_NUM];
    static float v3[OUTPUTS_ITEMS_NUM];
    static float v4[OUTPUTS_ITEMS_NUM];

    int i,j;
    for(i=0,j=S_DEGREES_GRANTED;i<OUTPUTS_ITEMS_NUM;i++,j++) {
	dataArray[i]=player_school.sub_score[j];
	v1[i]=dataArray[i][HISTORY_MONTH_COUNT-1];
	v2[i]=player_school.sub_score_weight[j];
	v3[i]=player_school.sub_score_change[j];
	float zn = player_school.get_protagonist_info(j+2)->normalizing_factor;
	// 1 year before
	float trend = (v1[i] - dataArray[i][HISTORY_MONTH_COUNT-1-12]) / zn;
	float level = (v1[i] - player_school.sub_score_start[j]) / zn;
	v4[i] = trend;
	v3[i] = level;
    }

    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( outputs_report_init_flag )                // deinit it first if it has already been initialized
	    deinit_outputs();
	outputs_report_init_flag = 1;

	for(int i=0;i<OUTPUTS_ITEMS_NUM;i++) {
	    outputs_graphicon[i].init(
		WINDOW_X1+tab5,WINDOW_Y1+vtab2+i*32-10,
		WINDOW_X1+tab6,WINDOW_Y1+vtab2+i*32+20,
		1 ,&data_num ,dataArray[i], 1, 0, 3);
	    outputs_graphicon[i].paint();
	    outputs_text1[i].right_put(WINDOW_X1+tab2-40,WINDOW_Y1+vtab2+i*32,v1+i,4);
	    outputs_text2[i].right_put(WINDOW_X1+tab3-60,WINDOW_Y1+vtab2+i*32,v2+i,24);
	    outputs_text4[i].right_put(WINDOW_X1+tab3+30, WINDOW_Y1+vtab2+i*32, v4+i, 24);
	    //## chea 110899
	    outputs_text3[i].right_put(WINDOW_X1+tab4+95,WINDOW_Y1+vtab2+i*32,v3+i,24);

	    if(user_interface.backgd_flag==1) {
		image_interface.put_back(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*32-2,"QUEST_UP");
		help_button_array[i].create_text(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*32-2,WINDOW_X1+tab1-5,WINDOW_Y1+vtab2+i*32+16,"?");
	    }
	}
    }

    for(i=0;i<OUTPUTS_ITEMS_NUM;i++) {
	outputs_text1[i].refresh();
	outputs_text2[i].refresh();
	outputs_text3[i].refresh();
	outputs_text4[i].refresh();
	outputs_graphicon[i].refresh();
	font_chartsm.put(WINDOW_X1+tab1,WINDOW_Y1+vtab2+i*32,outputs_help_title[i]);
    }

    vga.blt_buf(WINDOW_X1, WINDOW_Y1, WINDOW_X2, WINDOW_Y2);

    vga.use_front();
};

static int detect_outputs() {
    const static int tab1=30,vtab2=37;
    vga.use_back();
    for(int i=0;i<OUTPUTS_ITEMS_NUM;i++)
	if(help_button_array[i].detect()) {
	    // ## begin chwg1022  create the global ResTxt help_resource later //
	    String str;
	    str  = DIR_RES;
	    str += "SCORE";

	    ResTxt rs;
	    rs.init(str);
	    // ## chwg1022 end ////////////////////////

	    image_interface.put_back(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*32-2,"QUEST_DN");
	    vga.blt_buf(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*32-2,WINDOW_X1+tab1-2,WINDOW_Y1+vtab2+i*32+16);
	    sys.back_view_mode=sys.view_mode;
	    help_if.help_textlist.topic_id=rs.locate_topic(outputs_help_title[i]);
	    if(help_if.help_textlist.topic_id>0) {
		help_if.help_textlist.add_history(help_if.help_textlist.topic_id);
		sys.set_view_mode(MODE_HELP_DESC);
		sys.redraw_all_flag = 1;
		audio.play_wav("BEEPS-1",audio.int_to_DsVolume(config.sound_effect_volume));
	    }
	    return 1;
	}
    return 0;
};

static void deinit_outputs() {

    if(outputs_report_init_flag==0)return;

    for(int i=0;i<OUTPUTS_ITEMS_NUM;i++) {
	outputs_graphicon[i].deinit();
	outputs_text1[i].deinit();
	outputs_text2[i].deinit();
	outputs_text3[i].deinit();
	// ##### begin Gilbert 28/04/2001 #####//
	help_button_array[i].deinit();
	// ##### end Gilbert 28/04/2001 #####//
    }
    // ###### begin Gilbert 25/04/2001 #######//
    outputs_report_init_flag = 0;
    // ###### end Gilbert 25/04/2001 #######//

};

//-- screen 3 : general ---/
#define GENERAL_ITEMS_NUM 6
static char * general_help_title[GENERAL_ITEMS_NUM]= {
    "Institutional prestige",
    "Educational quality",
    "Scholarship, broadly defined",
    "Student diversity",
    "Faculty diversity",
    "Percentage of alumni who have donated during the last five years",
};

static int general_report_init_flag=0;
static GraphPerformanceIndicatorMonth general_graphicon[GENERAL_ITEMS_NUM];
static Text general_text1[GENERAL_ITEMS_NUM];
static Text general_text2[GENERAL_ITEMS_NUM];
static Text general_text3[GENERAL_ITEMS_NUM];
static Text general_text4[GENERAL_ITEMS_NUM];

static void report_general(int refreshFlag) {
    const static int tab1=30,tab2=320,tab3=420,tab4=490;
    const static int tab5=620,tab6=680,tab7=660,tab8=760;
    const static int vtab1=22,vtab2=37,vtab3=200,vtab4=330,vtab5=320;

    static int data_num=HISTORY_MONTH_COUNT;

    static float *dataArray[GENERAL_ITEMS_NUM];
    static float v1[GENERAL_ITEMS_NUM];
    static float v2[GENERAL_ITEMS_NUM];
    static float v3[GENERAL_ITEMS_NUM];
    static float v4[GENERAL_ITEMS_NUM];

    int i,j;

    for(i=0,j=S_PRESTIGE;i<GENERAL_ITEMS_NUM;i++,j++) {
	dataArray[i]=player_school.sub_score[j];
	v1[i]=dataArray[i][HISTORY_MONTH_COUNT-1];
	v2[i]=player_school.sub_score_weight[j];
	v3[i]=player_school.sub_score_change[j];
	float zn = player_school.get_protagonist_info(j+2)->normalizing_factor;
	// 1 year before
	float trend = (v1[i] - dataArray[i][HISTORY_MONTH_COUNT-1-12]) / zn;
	float level = (v1[i] - player_school.sub_score_start[j]) / zn;
	v4[i] = trend;
	v3[i] = level;
    }

    //## chea 201099 I have do this just after the prerun year
    if(info.game_date < 1721699)
	v3[0] = task_array.rand_init;

    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( general_report_init_flag )                // deinit it first if it has already been initialized
	    deinit_general();
	general_report_init_flag = 1;

	for(int i=0;i<GENERAL_ITEMS_NUM;i++) {
	    general_graphicon[i].init(
		WINDOW_X1+tab5,WINDOW_Y1+vtab2+i*40-10,
		WINDOW_X1+tab6,WINDOW_Y1+vtab2+i*40+20,
		1 ,&data_num ,dataArray[i], 1, 0, 3);
	    general_graphicon[i].paint();

	    if(user_interface.backgd_flag==1) {
		image_interface.put_back(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*40-2,"QUEST_UP");
		help_button_array[i].create_text(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*40-2,WINDOW_X1+tab1-5,WINDOW_Y1+vtab2+i*40+16,"?");
	    }
	    general_text1[i].right_put(WINDOW_X1+tab2-40,WINDOW_Y1+vtab2+i*40,v1+i,4);
	    general_text2[i].right_put(WINDOW_X1+tab3-60,WINDOW_Y1+vtab2+i*40,v2+i,24);
	    general_text4[i].right_put(WINDOW_X1+tab3+30, WINDOW_Y1+vtab2+i*40, v4+i, 24);
	    //## chea 110899
	    general_text3[i].right_put(WINDOW_X1+tab4+95,WINDOW_Y1+vtab2+i*40,v3+i,24);
	}
    }

    for(i=0;i<GENERAL_ITEMS_NUM;i++) {
	font_chartsm.put_paragraph(WINDOW_X1+tab1,WINDOW_Y1+vtab2+i*40,
				   WINDOW_X1+tab2-90,WINDOW_Y1+vtab2+i*40+40,
				   general_help_title[i]);
	general_graphicon[i].refresh();
	general_text1[i].refresh();
	general_text2[i].refresh();
	general_text3[i].refresh();
    }

    vga.blt_buf(WINDOW_X1, WINDOW_Y1, WINDOW_X2, WINDOW_Y2);
    vga.use_front();
};

static int detect_general() {
    const static int tab1=30,vtab2=37;

    vga.use_back();
    for(int i=0;i<GENERAL_ITEMS_NUM;i++) {
	if(help_button_array[i].detect()) {
	    // ## begin chwg1022  create the global ResTxt help_resource later //
	    String str;
	    str  = DIR_RES;
	    str += "SCORE";

	    ResTxt rs;
	    rs.init(str);
	    // ## chwg1022 end ////////////////////////

	    if(user_interface.backgd_flag==1) {
		image_interface.put_back(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*40-2,"QUEST_DN");
		vga.blt_buf(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*40-2,WINDOW_X1+tab1-2,WINDOW_Y1+vtab2+i*40+16);
	    }
	    sys.back_view_mode=sys.view_mode;
	    help_if.help_textlist.topic_id=rs.locate_topic(general_help_title[i]);
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
};

static void deinit_general() {
    if(general_report_init_flag==0)return;

    for(int i=0;i<GENERAL_ITEMS_NUM;i++) {
	general_graphicon[i].deinit();
	general_text1[i].deinit();
	general_text2[i].deinit();
	general_text3[i].deinit();
	// ###### begin Gilbert 28/04/2001 ######//
	help_button_array[i].deinit();
	// ###### end Gilbert 28/04/2001 ######//
    }

    // ###### begin Gilbert 25/04/2001 ######//
    general_report_init_flag = 0;
    // ###### end Gilbert 25/04/2001 ######//
};

//-- screen 4 : attitudes ---/
#define ATTITUDES_ITEMS_NUM 3
static int attitudes_report_init_flag=0;
static char *attitudes_help_title[]= {
    "Faculty morale",
    "Student morale",
    "Staff morale",
};
static GraphPerformanceIndicatorMonth attitudes_graphicon[ATTITUDES_ITEMS_NUM];
static Text attitudes_text1[ATTITUDES_ITEMS_NUM];
static Text attitudes_text2[ATTITUDES_ITEMS_NUM];
static Text attitudes_text3[ATTITUDES_ITEMS_NUM];
static Text attitudes_text4[ATTITUDES_ITEMS_NUM];

static void report_attitudes(int refreshFlag) {
    const static int tab1=30,tab2=320,tab3=420,tab4=490;
    const static int tab5=620,tab6=680,tab7=660,tab8=760;
    const static int vtab1=22,vtab2=37,vtab3=200,vtab4=330,vtab5=320;

    static int data_num=HISTORY_MONTH_COUNT;

    static float *dataArray[ATTITUDES_ITEMS_NUM];
    static float v1[ATTITUDES_ITEMS_NUM];
    static float v2[ATTITUDES_ITEMS_NUM];
    static float v3[ATTITUDES_ITEMS_NUM];
    static float v4[ATTITUDES_ITEMS_NUM];

    int i,j;
    for(i=0,j=S_FACULTY_MORALE;i<ATTITUDES_ITEMS_NUM;i++,j++) {
	dataArray[i]=player_school.sub_score[j];
	v1[i]=dataArray[i][HISTORY_MONTH_COUNT-1];
	v2[i]=player_school.sub_score_weight[j];
	v3[i]=player_school.sub_score_change[j];
	float zn = player_school.get_protagonist_info(j+2)->normalizing_factor;
	// 1 year before
	float trend = (v1[i] - dataArray[i][HISTORY_MONTH_COUNT-1-12]) / zn;
	float level = (v1[i] - player_school.sub_score_start[j]) / zn;
	v4[i] = trend;
	v3[i] = level;
    }

    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( attitudes_report_init_flag )              // deinit it first if it has already been initialized
	    deinit_attitudes();
	attitudes_report_init_flag = 1;

	for(int i=0;i<ATTITUDES_ITEMS_NUM;i++) {
	    attitudes_graphicon[i].init(
		WINDOW_X1+tab5,WINDOW_Y1+vtab2+i*32-10,
		WINDOW_X1+tab6,WINDOW_Y1+vtab2+i*32+20,
		1 ,&data_num ,dataArray[i], 1, 0, 3);
	    attitudes_graphicon[i].paint();

	    attitudes_text1[i].right_put(WINDOW_X1+tab2-40,WINDOW_Y1+vtab2+i*32,v1+i,4);
	    attitudes_text2[i].right_put(WINDOW_X1+tab3-60,WINDOW_Y1+vtab2+i*32,v2+i,24);
	    attitudes_text4[i].right_put(WINDOW_X1+tab3+30, WINDOW_Y1+vtab2+i*32, v4+i, 24);
	    //## chea 110899
	    attitudes_text3[i].right_put(WINDOW_X1+tab4+95,WINDOW_Y1+vtab2+i*32,v3+i,24);
	    if(user_interface.backgd_flag==1) {
		image_interface.put_back(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*32-2,"QUEST_UP");
		help_button_array[i].create_text(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*32-2,WINDOW_X1+tab1-5,WINDOW_Y1+vtab2+i*32+16,"?");
	    }
	}
    }

    for(i=0;i<ATTITUDES_ITEMS_NUM;i++) {
	font_chartsm.put(WINDOW_X1+tab1,WINDOW_Y1+vtab2+i*32,attitudes_help_title[i]);
	attitudes_graphicon[i].refresh();
	attitudes_text1[i].refresh();
	attitudes_text2[i].refresh();
	attitudes_text3[i].refresh();
    }

    vga.blt_buf(WINDOW_X1, WINDOW_Y1, WINDOW_X2, WINDOW_Y2);
    vga.use_front();
};

static int detect_attitudes() {
    const static int tab1=30,vtab2=37;

    vga.use_back();
    for(int i=0;i<ATTITUDES_ITEMS_NUM;i++) {
	if(help_button_array[i].detect()) {
	    // ## begin chwg1022  create the global ResTxt help_resource later //
	    String str;
	    str  = DIR_RES;
	    str += "SCORE";

	    ResTxt rs;
	    rs.init(str);
	    // ## chwg1022 end ////////////////////////

	    image_interface.put_back(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*32-2,"QUEST_DN");
	    vga.blt_buf(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*32-2,WINDOW_X1+tab1-2,WINDOW_Y1+vtab2+i*32+16);
	    sys.back_view_mode=sys.view_mode;
	    help_if.help_textlist.topic_id=rs.locate_topic(attitudes_help_title[i]);
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
};

static void deinit_attitudes() {
    if(attitudes_report_init_flag==0)return;

    for(int i=0;i<ATTITUDES_ITEMS_NUM;i++) {
	attitudes_graphicon[i].deinit();
	attitudes_text1[i].deinit();
	attitudes_text2[i].deinit();
	attitudes_text3[i].deinit();
	// ##### begin Gilbert 28/04/2001 #####//
	help_button_array[i].deinit();
	// ##### end Gilbert 28/04/2001 #####//
    }
    // ##### begin Gilbert 25/04/2001 ######//
    attitudes_report_init_flag = 0;
    // ##### end Gilbert 25/04/2001 ######//
};

//-- screen 5 : management ---/
#define MANAGEMENT_ITEMS_NUM 5
static char *management_help_title[]= {
    "Average of class sizes as a percent normal class sizes",
    "Library volumes per FTE student",
    "Percent of TSUs using IT in course",
    "Administrative performance",
    "Percentage of alumni who have donated during the last five years",
};
static int management_report_init_flag=0;
static GraphPerformanceIndicatorMonth management_graphicon[MANAGEMENT_ITEMS_NUM];
static Text management_text1[MANAGEMENT_ITEMS_NUM];
static Text management_text2[MANAGEMENT_ITEMS_NUM];
static Text management_text3[MANAGEMENT_ITEMS_NUM];
static Text management_text4[MANAGEMENT_ITEMS_NUM];

static void report_management(int refreshFlag) {
    const static int tab1=30,tab2=320,tab3=420,tab4=490;
    const static int tab5=620,tab6=680,tab7=660,tab8=760;
    const static int vtab1=22,vtab2=37,vtab3=200,vtab4=330,vtab5=480;

    static int data_num=HISTORY_MONTH_COUNT;

    static float *dataArray[MANAGEMENT_ITEMS_NUM];
    static float v1[MANAGEMENT_ITEMS_NUM];
    static float v2[MANAGEMENT_ITEMS_NUM];
    static float v3[MANAGEMENT_ITEMS_NUM];

    int i,j;
    //#### fred 1007 for compilation ####//
    //for(i=0,j=S_AVERAGE_OF_CLASS_SIZES_PERCENT;i<MANAGEMENT_ITEMS_NUM;i++,j++)
    for(i=0,j=7;i<MANAGEMENT_ITEMS_NUM;i++,j++) {
	//#### fred 1007 ####//
	dataArray[i]=player_school.sub_score[j];
	v1[i]=dataArray[i][HISTORY_MONTH_COUNT-1];
	v2[i]=player_school.sub_score_weight[j];
	v3[i]=player_school.sub_score_change[j];
    }

    vga.use_back();
    if( refreshFlag == INFO_REPAINT ) {
	if( management_report_init_flag )             // deinit it first if it has already been initialized
	    deinit_management();
	management_report_init_flag = 1;

	for(int i=0;i<MANAGEMENT_ITEMS_NUM;i++) {
	    management_graphicon[i].init(
		WINDOW_X1+tab5,WINDOW_Y1+vtab2+i*48-10,
		WINDOW_X1+tab6,WINDOW_Y1+vtab2+i*48+20,
		1 ,&data_num ,dataArray[i], 1, 0, 3);
	    management_graphicon[i].paint();

	    management_text1[i].right_put(WINDOW_X1+tab2+40,WINDOW_Y1+vtab2+i*48,v1+i,4);
	    management_text2[i].right_put(WINDOW_X1+tab3+20,WINDOW_Y1+vtab2+i*48,v2+i,24);
	    //management_text3[i].right_put(WINDOW_X1+tab4+60,WINDOW_Y1+vtab2+i*48,v3+i,4);
	    //## chea 110899
	    management_text3[i].right_put(WINDOW_X1+tab4+60,WINDOW_Y1+vtab2+i*48,v3+i,24);

	    if(user_interface.backgd_flag==1) {
		image_interface.put_back(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*48-2,"QUEST_UP");
		help_button_array[i].create_text(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*48-2,WINDOW_X1+tab1-5,WINDOW_Y1+vtab2+i*48+16,"?");
	    }
	}
    }

    for(i=0;i<MANAGEMENT_ITEMS_NUM;i++) {
	font_chartsm.put_paragraph( WINDOW_X1+tab1,WINDOW_Y1+vtab2+i*48,
				    WINDOW_X1+tab2,WINDOW_Y1+vtab2+(i+1)*48,
				    management_help_title[i]);
	management_graphicon[i].refresh();
	management_text1[i].refresh();
	management_text2[i].refresh();
	management_text3[i].refresh();
    }

    vga.blt_buf(WINDOW_X1, WINDOW_Y1, WINDOW_X2, WINDOW_Y2);
    vga.use_front();
};

static int detect_management() {
    const static int tab1=30,vtab2=37;

    vga.use_back();
    for(int i=0;i<GENERAL_ITEMS_NUM;i++) {
	if(help_button_array[i].detect()) {
	    // ## begin chwg1022  create the global ResTxt help_resource later //
	    String str;
	    str  = DIR_RES;
	    str += "SCORE";

	    ResTxt rs;
	    rs.init(str);
	    // ## chwg1022 end ////////////////////////

	    image_interface.put_back(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*48-2,"QUEST_DN");
	    vga.blt_buf(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*48-2,WINDOW_X1+tab1-2,WINDOW_Y1+vtab2+i*48+16);
	    sys.back_view_mode=sys.view_mode;
	    help_if.help_textlist.topic_id=rs.locate_topic(management_help_title[i]);
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

static void deinit_management() {
    if(management_report_init_flag==0)return;

    for(int i=0;i<MANAGEMENT_ITEMS_NUM;i++) {
	management_graphicon[i].deinit();
	management_text1[i].deinit();
	management_text2[i].deinit();
	management_text3[i].deinit();
	// ###### begin Gilbert 25/04/2001 ######//
	help_button_array[i].deinit();
	// ###### end Gilbert 25/04/2001 ######//
    }
    // ###### begin Gilbert 25/04/2001 ######//
    management_report_init_flag = 0;
    // ###### end Gilbert 25/04/2001 ######//
};

//-- screen 6 : finance ---/
#define FINANCE_ITEMS_NUM 4
static int finance_report_init_flag=0;
static char *finance_help_title[FINANCE_ITEMS_NUM]= {
    "Current surplus (deficit) as a percentage of expenditure",
    "Smoothed surplus (deficit) as a percentage of expenditure",
    "Endowment payout rate relative to the equilibrium payout rate",
    "Deferred maintenance backlog",
};
static GraphPerformanceIndicatorMonth finance_graphicon[FINANCE_ITEMS_NUM];
static Text finance_text1[FINANCE_ITEMS_NUM];
static Text finance_text2[FINANCE_ITEMS_NUM];
static Text finance_text3[FINANCE_ITEMS_NUM];
static Text finance_text4[FINANCE_ITEMS_NUM];

static void report_finance(int refreshFlag) {
    const static int tab1=30,tab2=320,tab3=420,tab4=490;
    const static int tab5=620,tab6=680,tab7=660,tab8=760;
    const static int vtab1=22,vtab2=37,vtab3=200,vtab4=330,vtab5=480;

    static int data_num=HISTORY_MONTH_COUNT;

    static float *dataArray[FINANCE_ITEMS_NUM];
    static float v1[FINANCE_ITEMS_NUM];
    static float v2[FINANCE_ITEMS_NUM];
    static float v3[FINANCE_ITEMS_NUM];
    static float v4[FINANCE_ITEMS_NUM];

    int i,j;
    for(i=0,j=S_CURRENT_SURPLUS_DEFICIT_PERCENT;i<FINANCE_ITEMS_NUM;i++,j++) {
	dataArray[i]=player_school.sub_score[j];
	v1[i]=dataArray[i][HISTORY_MONTH_COUNT-1];
	v2[i]=player_school.sub_score_weight[j];
	v3[i]=player_school.sub_score_change[j];
	float zn = player_school.get_protagonist_info(j+2)->normalizing_factor;
	// 1 year before
	float trend = (v1[i] - dataArray[i][HISTORY_MONTH_COUNT-1-12]) / zn;
	float level = (v1[i] - player_school.sub_score_start[j]) / zn;
	v4[i] = trend;
	v3[i] = level;
    }

    vga.use_back();
    if(refreshFlag == INFO_REPAINT) {
	if(finance_report_init_flag)                  // deinit it first if it has already been initialized
	    deinit_finance();
	finance_report_init_flag = 1;

	for(int i=0;i<FINANCE_ITEMS_NUM;i++) {
	    finance_graphicon[i].init(
		WINDOW_X1+tab5,WINDOW_Y1+vtab2+i*40-10,
		WINDOW_X1+tab6,WINDOW_Y1+vtab2+i*40+20,
		1 ,&data_num ,dataArray[i], 1, 0, 3);
	    finance_graphicon[i].paint();
	    if(user_interface.backgd_flag==1) {
		image_interface.put_back(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*40-2,"QUEST_UP");
		help_button_array[i].create_text(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*40-2,WINDOW_X1+tab1-5,WINDOW_Y1+vtab2+i*40+16,"?");
	    }
	    finance_text1[i].right_put(WINDOW_X1+tab2-40,WINDOW_Y1+vtab2+i*40,v1+i,4);
	    finance_text2[i].right_put(WINDOW_X1+tab3-60,WINDOW_Y1+vtab2+i*40,v2+i,24);
	    finance_text4[i].right_put(WINDOW_X1+tab3+30, WINDOW_Y1+vtab2+i*40, v4+i, 24);
	    //## chea 110899
	    finance_text3[i].right_put(WINDOW_X1+tab4+95,WINDOW_Y1+vtab2+i*40,v3+i,24);
	}
    }

    for(i=0;i<FINANCE_ITEMS_NUM;i++) {
	font_chartsm.put_paragraph( WINDOW_X1+tab1,WINDOW_Y1+vtab2+i*40,
				    WINDOW_X1+tab2-95,WINDOW_Y1+vtab2+(i+1)*40,
				    finance_help_title[i]);
	finance_graphicon[i].refresh();
	finance_text1[i].refresh();
	finance_text2[i].refresh();
	finance_text3[i].refresh();
    }

    vga.blt_buf(WINDOW_X1, WINDOW_Y1, WINDOW_X2, WINDOW_Y2);
    vga.use_front();
};

static int detect_finance() {
    const static int tab1=30,vtab2=37;

    vga.use_back();
    for(int i=0;i<GENERAL_ITEMS_NUM;i++) {
	if(help_button_array[i].detect()) {
	    // ## begin chwg1022  create the global ResTxt help_resource later //
	    String str;
	    str  = DIR_RES;
	    str += "SCORE";

	    ResTxt rs;
	    rs.init(str);
	    // ## chwg1022 end ////////////////////////

	    image_interface.put_back(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*40-2,"QUEST_DN");
	    vga.blt_buf(WINDOW_X1+tab1-23,WINDOW_Y1+vtab2+i*40-2,WINDOW_X1+tab1-2,WINDOW_Y1+vtab2+i*40+16);
	    sys.back_view_mode=sys.view_mode;
	    help_if.help_textlist.topic_id=rs.locate_topic(finance_help_title[i]);
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

static void deinit_finance() {
    if(finance_report_init_flag==0)return;

    for(int i=0;i<FINANCE_ITEMS_NUM;i++) {
	finance_graphicon[i].deinit();
	finance_text1[i].deinit();
	finance_text2[i].deinit();
	finance_text3[i].deinit();
	// ##### begin Gilbert 28/04/2001 ######//
	help_button_array[i].deinit();
	// ##### end Gilbert 28/04/2001 ######//
    }
    // ###### begin Gilbert 25/04/2001 ######//
    finance_report_init_flag = 0;
    // ###### end Gilbert 25/04/2001 ######//
};
