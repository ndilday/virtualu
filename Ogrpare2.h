//Filename    : OGrpAre2.H
//Description : Header of object GraphArea2

#ifndef __Ographarea2_H
#define __Ographarea2_H

#include <OVGA.H>
#include <OFONT.H>

//--------- Define macro constant -----------//

#ifndef __OGRPHYR_H
#ifndef __OGRAPHA_H
enum {
    DATA_FLOAT = 1, DATA_DOUBLE = 2,
    DATA_INT = 3, DATA_LONG = 4
};
#endif
#endif

//--------- Define class GraphArea2 --------//
//
//!
class GraphArea2 {
public:

    short grapharea2_x1, grapharea2_y1, grapharea2_x2, grapharea2_y2;
    short series_x1, series_y1, series_x2, series_y2;
    short graph_height, graph_width;
    short legend_width, legend_height, legend_x_num, legend_y_num;
    char **legend_array, *x_label, *y_label;
    int *series_color, axis_color;
    int max_upper_limit;
    char init_flag, transparent_flag, orientation_flag;
    char data_type_flag, value_flag;
    int num_format;
    int series_num, *data_num;
    int this_year_indicator;
    void *data_array;
    int *x_start, *x_end;
    short y_label_max_len;

protected:
    char x_axis_pos;                              // 0-bottom, 1-center
    int x_axis_scr_pos;                           // scr y-pos of x-axis
    double max_label;                             // max label value of the graph //chwg0115
    double min_label;                             // max label value of the graph //chwg0115
    double max_val;                               // max abs value of the graph	//chwg0115
    double min_val;                               // max abs value of the graph //chwg0115
    double y_ratio;
    short x_axis_step;                            // step size on x-axis
    Font *font_ptr;
    short *grapharea2_bitmap;

public:
    GraphArea2();
    ~GraphArea2();

    //19
    void init(int,int,int,int,int,
	      int*,void*,char,char,int = 1000000,
	      char* =NULL,char* =NULL,char** =NULL,char=1,char=1,
	      char=1,int=1,int* =NULL,int=Vga::active_buf->color_dark);

    // 18
    void init(int x1,int y1,int x2,int y2,int seriesNum,
	      int *dataNum,float* dataArray,int thisyear,int maxUpperLimit,char* xLabel,
	      char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,
	      int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark) {
	// 19
	init(x1, y1, x2, y2, seriesNum,
	     dataNum, (void*)dataArray, DATA_FLOAT, thisyear, maxUpperLimit,
	     xLabel, yLabel, legendArray, transparentFlag, orientationFlag,
	     valueFlag, numFormat, seriesColor, axisColor);
    }

    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,double* dataArray,int thisyear,int maxUpperLimit,char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	{ init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_DOUBLE, thisyear, maxUpperLimit, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }

    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,int* dataArray,int thisyear, int maxUpperLimit, char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	{ init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_INT, thisyear, maxUpperLimit, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }

    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,long* dataArray,int thisyear, int maxUpperLimit, char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	{ init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_LONG, thisyear, maxUpperLimit, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }

    void deinit();
    void paint();
    void refresh();
    void set_font(Font *);
    void calc_pos();

    void set_y_label_max_len(short yLabelMaxLen) { y_label_max_len = yLabelMaxLen; }

protected:
    void draw_legend();
    void draw_axis_label();
    void find_scale();
    void draw_scale();
    void draw_series();
    void draw_series_float();
    void draw_series_double();
    void draw_series_int();
    void draw_series_long();
    void draw_value(void *, int, short, short, int);

};

//--------------------------//
#endif
