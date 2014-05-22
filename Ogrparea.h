//Filename    : OGraphArea.H
//Description : Header of object GraphArea

#ifndef __OGRAPHAREA_H
#define __OGRAPHAREA_H

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

//--------- Define class GraphArea --------//
//
//!
class GraphArea {
public:
    short GraphArea_x1, GraphArea_y1, GraphArea_x2, GraphArea_y2;
    short series_x1, series_y1, series_x2, series_y2;
    short graph_height, graph_width;
    short legend_width, legend_height, legend_x_num, legend_y_num;
    char **legend_array, *x_label, *y_label;
    int *series_color, axis_color;
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
    double max_scale;                             // max abs value of the graph
    short x_axis_step;                            // step size on x-axis
    Font *font_ptr;
    short *GraphArea_bitmap;

public:
    GraphArea();
    ~GraphArea();

    void init(int,int,int,int,int,int*,void*,char,int,char* =NULL,char* =NULL,char** =NULL,char=1,char=1,char=1,int=1,int* =NULL,int=Vga::active_buf->color_dark);
    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,float* dataArray,int thisyear,char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	{ init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_FLOAT,thisyear, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }
    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,double* dataArray,int thisyear,char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	{ init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_DOUBLE, thisyear, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }
    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,int* dataArray,int thisyear,char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	{ init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_INT, thisyear, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }
    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,long* dataArray,int thisyear,char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	{ init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_LONG, thisyear, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }

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
