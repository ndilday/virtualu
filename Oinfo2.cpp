//Filename: OINFO2.CPP
//Description : Info class

#include <ALL.H>
#include <OVGA.H>
#include <OFONT.H>
#include <OSYS.H>
#include <OINFO.H>
#include <OIFACE.H>
#include <OSTR.H>
#include <OMISC.H>
#include <OGRPHYR.H>
#include <OGRPH_PY.H>
#include <OGRPH_PT.H>
#include <OGRPH_PM.H>
#include <OFINANCE.H>
#include <OPSCHOOL.H>
#include <ODEPT.H>

static GraphPerformanceIndicatorTrimester graph_t[6];
static GraphPerformanceIndicatorMonth graph_m[6];
static GraphPerformanceIndicatorYear graph_y[6];

int graph_series_color[2] = { V_BLUE, V_ORANGE };

//-------- Begin of function Info::disp_6_graphs() --------//
void Info::disp_6_graphs() {
    if(!disp_6_graphs_flag) {
	image_interface.put_back(MIN_MAX_X1,MIN_MAX_Y1,"MAX");
	return;
    }
    image_interface.put_back(MIN_MAX_X1,MIN_MAX_Y1,"MIN");
    user_interface.brighten(GRAPH1_X1,GRAPH1_Y1,GRAPH1_X2,GRAPH1_Y2);
    user_interface.rect(GRAPH1_X1,GRAPH1_Y1,GRAPH1_X2,GRAPH1_Y2,2);
    user_interface.bar(GRAPH1_X1+5,GRAPH1_Y1+10,GRAPH1_X1+10,GRAPH1_Y1+15,V_BLUE);
    //### begin zb 02/25/99
    font_chartsm.put_paragraph(GRAPH1_X1+18,GRAPH1_Y1+5,GRAPH1_X1+150,GRAPH1_Y2,"Surplus or deficit");
    //### end zb 02/25/99

    user_interface.brighten(GRAPH2_X1,GRAPH2_Y1,GRAPH2_X2,GRAPH2_Y2);
    user_interface.rect(GRAPH2_X1,GRAPH2_Y1,GRAPH2_X2,GRAPH2_Y2,2);
    user_interface.bar(GRAPH2_X1+5,GRAPH2_Y1+10,GRAPH2_X1+10,GRAPH2_Y1+15,V_BLUE);
    //### begin zb 02/25/99
    font_chartsm.put_paragraph(GRAPH2_X1+18,GRAPH2_Y1+5,GRAPH2_X1+150,GRAPH2_Y2,"Projected reserves");
    //### end zb 02/25/99

    user_interface.brighten(GRAPH3_X1,GRAPH3_Y1,GRAPH3_X2,GRAPH3_Y2);
    user_interface.rect(GRAPH3_X1,GRAPH3_Y1,GRAPH3_X2,GRAPH3_Y2,2);

    //	user_interface.bar(GRAPH3_X1+5,GRAPH3_Y1+10,GRAPH3_X1+10,GRAPH3_Y1+15,V_BLUE);         //## chea 051199 swap position request
    //	font_chartsm.put_paragraph(GRAPH3_X1+18,GRAPH3_Y1+5,GRAPH3_X2,GRAPH3_Y2,"Research");   //## chea 051199 swap position request
    user_interface.bar(GRAPH3_X1+5,GRAPH3_Y1+10,GRAPH3_X1+10,GRAPH3_Y1+15,V_ORANGE);
    font_chartsm.put_paragraph(GRAPH3_X1+18,GRAPH3_Y1+5,GRAPH3_X2,GRAPH3_Y2,"Net tuition");

    //	user_interface.bar(GRAPH3_X1+5,GRAPH3_Y1+27,GRAPH3_X1+10,GRAPH3_Y1+32,V_ORANGE);            //## chea 051199 swap position request
    //	font_chartsm.put_paragraph(GRAPH3_X1+18,GRAPH3_Y1+23,GRAPH3_X2,GRAPH3_Y2+18,"Net Tuition"); //## chea 051199 swap position request
    user_interface.bar(GRAPH3_X1+5,GRAPH3_Y1+27,GRAPH3_X1+10,GRAPH3_Y1+32,V_BLUE);
    font_chartsm.put_paragraph(GRAPH3_X1+18,GRAPH3_Y1+23,GRAPH3_X2,GRAPH3_Y2+18,"Research");

    user_interface.brighten(GRAPH4_X1,GRAPH4_Y1,GRAPH4_X2,GRAPH4_Y2);
    user_interface.rect(GRAPH4_X1,GRAPH4_Y1,GRAPH4_X2,GRAPH4_Y2,2);
    user_interface.bar(GRAPH4_X1+5,GRAPH4_Y1+10,GRAPH4_X1+10,GRAPH4_Y1+15, graph_series_color[0] );
    // version 2 display both Institutional prestige and Undergraduate selectivity
    font_chartsm.put_paragraph(GRAPH4_X1+18,GRAPH4_Y1+5,GRAPH4_X1+120,GRAPH4_Y2+22,"Prestige");

    user_interface.bar(GRAPH4_X1+5,GRAPH4_Y1+27,GRAPH4_X1+10,GRAPH4_Y1+32, graph_series_color[1] );
    font_chartsm.put_paragraph(GRAPH4_X1+18,GRAPH4_Y1+23,GRAPH4_X2-5,GRAPH4_Y1+40,"Selectivity");

    user_interface.brighten(GRAPH5_X1,GRAPH5_Y1,GRAPH5_X2,GRAPH5_Y2);
    user_interface.rect(GRAPH5_X1,GRAPH5_Y1,GRAPH5_X2,GRAPH5_Y2,2);
    user_interface.bar(GRAPH5_X1+5,GRAPH5_Y1+10,GRAPH5_X1+10,GRAPH5_Y1+15,graph_series_color[0] );
    // version 2 display number of student and number of faculty
    font_chartsm.put_paragraph(GRAPH5_X1+18,GRAPH5_Y1+5,GRAPH5_X1+120,GRAPH5_Y2,"Total students");

    user_interface.bar(GRAPH5_X1+5,GRAPH5_Y1+27,GRAPH5_X1+10,GRAPH5_Y1+32,graph_series_color[1] );
    font_chartsm.put_paragraph(GRAPH5_X1+18,GRAPH5_Y1+23,GRAPH5_X2-5,GRAPH5_Y1+40,"Total faculty");

    user_interface.brighten(GRAPH6_X1,GRAPH6_Y1,GRAPH6_X2,GRAPH6_Y2);
    user_interface.rect(GRAPH6_X1,GRAPH6_Y1,GRAPH6_X2,GRAPH6_Y2,2);

    user_interface.bar(GRAPH6_X1+5,GRAPH6_Y1+10,GRAPH6_X1+10,GRAPH6_Y1+15, graph_series_color[0] );
    font_chartsm.put_paragraph(GRAPH6_X1+18,GRAPH6_Y1+5,GRAPH6_X2-5,GRAPH6_Y1+22,"Faculty morale");

    user_interface.bar(GRAPH6_X1+5,GRAPH6_Y1+27,GRAPH6_X1+10,GRAPH6_Y1+32, graph_series_color[1] );
    font_chartsm.put_paragraph(GRAPH6_X1+18,GRAPH6_Y1+23,GRAPH6_X2-5,GRAPH6_Y1+40,"Student morale");

    static int data_num1=HISTORY_MONTH_COUNT;
    static int data_num2=HISTORY_YEAR_COUNT;
    // #### begin Gilbert 02/05/2001 ####//
    static float dataArray1[2*HISTORY_MONTH_COUNT];
    // #### end Gilbert 02/05/2001 ####//

    // Graph 1
    for(int i=0;i<HISTORY_MONTH_COUNT;i++) {
	dataArray1[i]=(float)(finance.surplus_deficit_history[i]);
    }
    graph_m[0].init(GRAPH1_X1,GRAPH1_Y1+40,GRAPH1_X2,GRAPH1_Y2,
		    1 ,&data_num1 ,dataArray1, 1, 0, 3);
    graph_m[0].refresh();
    graph_m[0].deinit();
    //### begin zb 02/25/99
    if(dataArray1[HISTORY_MONTH_COUNT-1]<0)
	font_chart_red_sm.put_paragraph(GRAPH1_X1+18,GRAPH1_Y1+23,GRAPH1_X2,GRAPH1_Y1+40,m.format(dataArray1[HISTORY_MONTH_COUNT-1],2));
    else
	font_chartsm.put_paragraph(GRAPH1_X1+18,GRAPH1_Y1+23,GRAPH1_X2,GRAPH1_Y1+40,m.format(dataArray1[HISTORY_MONTH_COUNT-1],2));
    //### end zb 02/25/99

    // Graph 2
    for(int i=0;i<HISTORY_MONTH_COUNT;i++) {
	dataArray1[i]=(float)(finance.operating_sub_surplus_history[i]);
    }
    graph_m[1].init(GRAPH2_X1,GRAPH2_Y1+40,GRAPH2_X2,GRAPH2_Y2,
		    1 ,&data_num1 ,dataArray1, 1, 0, 3);
    graph_m[1].refresh();
    graph_m[1].deinit();
    //### begin zb 02/25/99
    if(dataArray1[HISTORY_MONTH_COUNT-1]<0)
	font_chart_red_sm.put_paragraph(GRAPH2_X1+18,GRAPH2_Y1+23,GRAPH2_X2,GRAPH2_Y1+40,m.format(dataArray1[HISTORY_MONTH_COUNT-1],2));
    else
	font_chartsm.put_paragraph(GRAPH2_X1+18,GRAPH2_Y1+23,GRAPH2_X2,GRAPH2_Y1+40,m.format(dataArray1[HISTORY_MONTH_COUNT-1],2));
    //### end zb 02/25/99

    // Graph 3
    //net_tuition_history

    for(int i=0;i<HISTORY_MONTH_COUNT;i++) {
	dataArray1[i+HISTORY_MONTH_COUNT]=(float)(finance.net_tuition_history[i]);
	dataArray1[i]=(float)(finance.sponsored_research_history[i]);
    }
    //	graph_m[2].init(GRAPH3_X1,GRAPH3_Y1+40,GRAPH3_X2,GRAPH3_Y2,
    //						1 ,&data_num1 ,dataArray1, 1, 0, 3);

    graph_m[2].init(GRAPH3_X1,GRAPH3_Y1+40,GRAPH3_X2,GRAPH3_Y2,
		    2 ,&data_num1 ,dataArray1, 1, 0, 3);

    graph_m[2].refresh();
    graph_m[2].deinit();
    //	if(dataArray1[HISTORY_MONTH_COUNT-1]<0)
    //		font_chart_red_sm.put_paragraph(GRAPH3_X1+73,GRAPH3_Y1+23,GRAPH3_X2,GRAPH3_Y1+40,m.format(dataArray1[HISTORY_MONTH_COUNT-1],2));
    //	else
    //		font_chartsm.put_paragraph(GRAPH3_X1+73,GRAPH3_Y1+23,GRAPH3_X2,GRAPH3_Y1+40,m.format(dataArray1[HISTORY_MONTH_COUNT-1],2));

    //	font_chartsm.put_paragraph(GRAPH3_X1+90,GRAPH3_Y1+5,GRAPH3_X2,GRAPH3_Y1+22,m.format((int)dataArray1[HISTORY_MONTH_COUNT-1],2));    //## chea 051199 swap position request
    //	font_chartsm.put_paragraph(GRAPH3_X1+90,GRAPH3_Y1+23,GRAPH3_X2,GRAPH3_Y1+40,m.format((int)dataArray1[2*HISTORY_MONTH_COUNT-1],2)); //## chea 051199 swap position request
    //### begin zb 02/25/99
    //## chea 051199 swap position request
    font_chartsm.put_paragraph(GRAPH3_X1+85,GRAPH3_Y1+5,GRAPH3_X2,GRAPH3_Y1+22,m.format((int)dataArray1[2*HISTORY_MONTH_COUNT-1],2));
    //## chea 051199 swap position request
    font_chartsm.put_paragraph(GRAPH3_X1+85,GRAPH3_Y1+23,GRAPH3_X2,GRAPH3_Y1+40,m.format((int)dataArray1[HISTORY_MONTH_COUNT-1],2));
    //### end zb 02/25/99

    // Graph 4
    for(int i=0;i<HISTORY_MONTH_COUNT;i++) {
	dataArray1[i]=(float)(player_school.sub_score[S_PRESTIGE][i]);
    }
    graph_m[3].init(GRAPH4_X1,GRAPH4_Y1+40,GRAPH4_X2,GRAPH4_Y2,
		    1 ,&data_num1 ,dataArray1, 1, 0, 3, graph_series_color );
    graph_m[3].refresh();
    graph_m[3].deinit();
    if(dataArray1[HISTORY_MONTH_COUNT-1]<0)         // verion 2 has two data
	font_chart_red_sm.put_paragraph(GRAPH4_X1+110,GRAPH4_Y1+5,GRAPH4_X2-5,GRAPH4_Y1+22,m.format((int)dataArray1[HISTORY_MONTH_COUNT-1],3));
    else
	font_chartsm.put_paragraph(GRAPH4_X1+110,GRAPH4_Y1+5,GRAPH4_X2-5,GRAPH4_Y1+22,m.format((int)dataArray1[HISTORY_MONTH_COUNT-1],3));

    // undergraudate selectivity draw in graph 4
    // int selHistoryColor = V_RED;

    for(int i=0;i<HISTORY_YEAR_COUNT;i++) {
	dataArray1[i]=(float)(finance.sl1_sel_history[i]*100);
    }
    graph_y[3].init(GRAPH4_X1,GRAPH4_Y1+40,GRAPH4_X2,GRAPH4_Y2,
		    1 ,&data_num2 ,dataArray1, 1, 0, 3, graph_series_color+1 );
    graph_y[3].refresh();
    graph_y[3].deinit();
    if(dataArray1[HISTORY_MONTH_COUNT-1]<0)
	font_chart_red_sm.put_paragraph(GRAPH4_X1+110,GRAPH4_Y1+23,GRAPH4_X2-5,GRAPH4_Y1+40,m.format((int)dataArray1[HISTORY_YEAR_COUNT-1],3));
    else
	font_chartsm.put_paragraph(GRAPH4_X1+110,GRAPH4_Y1+23,GRAPH4_X2-5,GRAPH4_Y1+40,m.format((int)dataArray1[HISTORY_YEAR_COUNT-1],3));

    // Graph 5
    // version 2 display number of students and number of faculty
    for(int i=0;i<HISTORY_YEAR_COUNT;i++) {
	dataArray1[i] = department_array.student_level_history[MAX_STUDENT_LEVEL_TOTAL][i] / 10;
	dataArray1[i+HISTORY_YEAR_COUNT] = department_array.faculty_level_history[FACULTY_RANK_LEVEL_TOTAL][i];
    }
    graph_y[4].init(GRAPH5_X1,GRAPH5_Y1+40,GRAPH5_X2,GRAPH5_Y2,
		    2 ,&data_num2 ,dataArray1, 1, 0, 3, graph_series_color );
    graph_y[4].refresh();
    graph_y[4].deinit();
    if ( (int)department_array.student_level_history[MAX_STUDENT_LEVEL_TOTAL][HISTORY_YEAR_COUNT-1] >= 10000 ) {
	font_chartsm.right_put(GRAPH5_X2-15,GRAPH5_Y1+5,
			       // don't use dataArray because divied by 10
			       m.format((int)department_array.student_level_history[MAX_STUDENT_LEVEL_TOTAL][HISTORY_YEAR_COUNT-1],30));
    }
    else {
	font_chartsm.right_put(GRAPH5_X2-15,GRAPH5_Y1+5,
			       // don't use dataArray because divied by 10
			       m.format((int)department_array.student_level_history[MAX_STUDENT_LEVEL_TOTAL][HISTORY_YEAR_COUNT-1],1));
    }
    font_chartsm.put_paragraph(GRAPH5_X1+110,GRAPH5_Y1+23,GRAPH5_X2-5,GRAPH5_Y1+40,m.format((int)dataArray1[2*HISTORY_YEAR_COUNT-1],1));

    // Graph 6
    for(int i=0;i<HISTORY_MONTH_COUNT;i++) {
	dataArray1[i] = player_school.sub_score[S_FACULTY_MORALE][i];
	dataArray1[i+HISTORY_MONTH_COUNT] = player_school.sub_score[S_STUDENT_MORALE][i];

	//		dataArray1[i]=(float)(department_array.p_faculty_morale_history[i]);
    }
    graph_m[5].init(GRAPH6_X1,GRAPH6_Y1+40,GRAPH6_X2,GRAPH6_Y2,
		    2 ,&data_num1 ,dataArray1, 1, 0, 3, graph_series_color );
    graph_m[5].refresh();
    graph_m[5].deinit();
    font_chartsm.put_paragraph(GRAPH6_X1+110,GRAPH6_Y1+5,GRAPH6_X2-5,GRAPH6_Y1+22,m.format((int)dataArray1[HISTORY_MONTH_COUNT-1],3));
    font_chartsm.put_paragraph(GRAPH6_X1+110,GRAPH6_Y1+23,GRAPH6_X2-5,GRAPH6_Y1+40,m.format((int)dataArray1[2*HISTORY_MONTH_COUNT-1],3));
}

//-------- End of function Info::disp_6_graphs() --------//
