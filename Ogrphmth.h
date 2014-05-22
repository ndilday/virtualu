//Filename    : Ogrphmth.H
//Description : Header of object GraphMonth
//owner		  : chwg (Ho)

#ifndef __OGRPHMTH_H
#define __OGRPHMTH_H

#include <OVGA.H>
#include <OFONT.H>

//--------- Define macro constant -----------//
#ifndef __OGRPHYR_H
enum {
    DATA_FLOAT = 1, DATA_DOUBLE = 2,
    DATA_INT = 3, DATA_LONG = 4
};
#endif __OGRPHYR_H
//--------- Define class GraphMonth --------//
//
//!
class GraphMonth {
public:
    short xAxisPos;
    short graph_x1, graph_y1, graph_x2, graph_y2;
    short series_x1, series_y1, series_x2, series_y2;
    short graph_height, graph_width, legend_height, legend_x_num, legend_y_num;
    char **legend_array, *x_label, *y_label;
    int *series_color, axis_color;
    int this_month_indicator;
    char init_flag;
    short legend_width, transparent_flag, orientation_flag;
    char data_type_flag, value_flag;
    int num_format;
    int series_num, *data_num;
    void *data_array;
    int *x_start, *x_end;
    short y_label_max_len;

protected:
    char x_axis_pos;                              // 0-bottom, 1-center
    float x_axis_step;                            // step size on x-axis
    short scaleStep;
    Font *font_ptr;
    short *graph_bitmap;
    double max_scale;                             // max abs value of the graph
    double min_scale;                             // max abs value of the graph
    short posScaleNum,negScaleNum;
    double scaleInc;
    double posScaleInc;
    double negScaleInc;

public:
    GraphMonth();
    ~GraphMonth();

    void init(int,int,int,int,int,
	      int*,void*,char,int=-1,
	      char* =NULL,char* =NULL,char** =NULL,char=1,char=1,
	      char=1,int=1,int* =NULL,int=Vga::active_buf->color_dark);

    void init(int x1,int y1,int x2,int y2,int seriesNum,
	      int *dataNum,float* dataArray,int thisMonth=-1,
	      char* xLabel=NULL,char* yLabel=NULL,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,
	      char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark) {
	init(x1, y1, x2, y2, seriesNum,
	     dataNum, (void*) dataArray, DATA_FLOAT,thisMonth,
	     xLabel, yLabel, legendArray, transparentFlag, orientationFlag,
	     valueFlag, numFormat, seriesColor, axisColor);
    }
    void init(int x1,int y1,int x2,int y2,int seriesNum,
	      int *dataNum,double* dataArray,int thisMonth=-1,
	      char* xLabel=NULL,char* yLabel=NULL,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,
	      char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark) {
	init(x1, y1, x2, y2, seriesNum,
	     dataNum, (void*) dataArray, DATA_DOUBLE,thisMonth,
	     xLabel, yLabel, legendArray, transparentFlag, orientationFlag,
	     valueFlag, numFormat, seriesColor, axisColor);
    }
    void init(int x1,int y1,int x2,int y2,int seriesNum,
	      int *dataNum,int* dataArray,int thisMonth=-1,
	      char* xLabel=NULL,char* yLabel=NULL,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,
	      char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark) {
	init(x1, y1, x2, y2, seriesNum,
	     dataNum, (void*) dataArray, DATA_INT,thisMonth,
	     xLabel, yLabel, legendArray, transparentFlag, orientationFlag,
	     valueFlag, numFormat, seriesColor, axisColor);
    }
    void init(int x1,int y1,int x2,int y2,int seriesNum,
	      int *dataNum,long* dataArray,int thisMonth=-1,
	      char* xLabel=NULL,char* yLabel=NULL,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,
	      char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark) {
	init(x1, y1, x2, y2, seriesNum,
	     dataNum, (void*) dataArray, DATA_LONG,thisMonth,
	     xLabel, yLabel, legendArray, transparentFlag, orientationFlag,
	     valueFlag, numFormat, seriesColor, axisColor);
    }

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
