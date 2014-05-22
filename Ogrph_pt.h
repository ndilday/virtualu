//Filename    : OGRPH_PT.H
//Description : Header of object Performance Indicator of Trimester

#ifndef __OGRPH_PT_H
#define __OGRPH_PT_H

#include <OVGA.H>
#include <OFONT.H>

//--------- Define macro constant -----------//

#ifndef __OGRPHYR_H
enum {
    DATA_FLOAT = 1, DATA_DOUBLE = 2,
    DATA_INT = 3, DATA_LONG = 4
};
#endif

//--------- Define class GraphPerformanceIndicatorTrimester --------//
//
//!
class GraphPerformanceIndicatorTrimester {
public:
    short grapha_x1, grapha_y1, grapha_x2, grapha_y2;
    short series_x1, series_y1, series_x2, series_y2;
    short graph_height, graph_width;
    short legend_width, legend_height, legend_x_num, legend_y_num;
    char **legend_array, *x_label, *y_label;
    int *series_color, axis_color;
    char init_flag, transparent_flag, orientation_flag;
    char data_type_flag, value_flag;
    int num_format;
    int series_num, *data_num;
    void *data_array;
    int *x_start, *x_end;
    short y_label_max_len;
    double default_upper_bound;

protected:
    char x_axis_pos;                              // 0-bottom, 1-center
    double max_scale;                             // max abs value of the graph
    double x_axis_step;                           // step size on x-axis

    Font *font_ptr;
    short *grapha_bitmap;

public:
    GraphPerformanceIndicatorTrimester();
    ~GraphPerformanceIndicatorTrimester();

    // 14 parameters
    void init(int,int,int,int,int,int*,void*,char,char=1,char=1,int=1,double=10.0,int* =NULL,int=Vga::active_buf->color_dark);

    // 14 parameters
    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,
	      float* dataArray,
	      char transparentFlag=1,char valueFlag=1,
	      int numFormat=1,double defaultUpperBound=10,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark) {
	init(x1, y1, x2, y2, seriesNum, dataNum,
	     (void*) dataArray,
	     DATA_FLOAT,
	     transparentFlag, valueFlag,
	     numFormat, defaultUpperBound, seriesColor, axisColor);
    }

    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,
	      double* dataArray,
	      char transparentFlag=1,char valueFlag=1,
	      int numFormat=1,double defaultUpperBound=10.0,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark) {
	init(x1, y1, x2, y2, seriesNum, dataNum,
	     (void*) dataArray,
	     DATA_DOUBLE,
	     transparentFlag, valueFlag,
	     numFormat, defaultUpperBound, seriesColor, axisColor);
    }

    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,
	      int* dataArray,
	      char transparentFlag=1,char valueFlag=1,
	      int numFormat=1,double defaultUpperBound=10.0,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark) {
	init(x1, y1, x2, y2, seriesNum, dataNum,
	     (void*) dataArray,
	     DATA_INT,
	     transparentFlag, valueFlag,
	     numFormat, defaultUpperBound, seriesColor, axisColor);
    }

    void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,
	      long* dataArray,
	      char transparentFlag=1,char valueFlag=1,
	      int numFormat=1,double defaultUpperBound=10.0,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark) {
	init(x1, y1, x2, y2, seriesNum, dataNum,
	     (void*) dataArray,
	     DATA_LONG,
	     transparentFlag, valueFlag,
	     numFormat, defaultUpperBound, seriesColor, axisColor);
    }

    void deinit();
    void paint();
    void refresh();
    void set_font(Font *);
    void calc_pos();
    void set_y_label_max_len(short yLabelMaxLen) { y_label_max_len = yLabelMaxLen; }

protected:
    void find_scale();
    void draw_scale();
    void draw_series();
    void draw_series_float();
    void draw_series_double();
    void draw_series_int();
    void draw_series_long();
    void draw_value(void *, int, short, short, int);
};
#endif
