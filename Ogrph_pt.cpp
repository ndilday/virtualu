//Filename    : OGRPH_PT.CPP
//Description : Object GraphPerformanceIndicatorTrimester
//Owner			: Kevin(Ho)

#include <ALL.H>
#include <COLOR.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OGRPH_PT.H>
#include <OIFACE.H>

#include <math.h>

//--------- Define macro constant -----------//

#define MAX_LEN_LABEL "0%"

static int default_series_color[] = {
    V_BLUE,
    V_GREEN,
    V_RED,
    V_YELLOW,
    V_VIOLET,
    V_BROWN,
    V_ORANGE,
    V_PINK,
    V_BLACK,
    V_WHITE,
};                                                // 10 default series colors

enum {  DEFAULT_SERIES_COLOR_NUM = 10 };

enum {
    LEGEND_SIZE = 11,
    LEGEND_X_SPACING = 12,
    LEGEND_Y_SPACING = 2,
};

enum {  COMMON_OFFSET = 2         };

//-------------- Define static vars ------------//

//static char paint_flag = 0;

//-------- Begin of function GraphPerformanceIndicatorTrimester::GraphPerformanceIndicatorTrimester ----//
//!
GraphPerformanceIndicatorTrimester::GraphPerformanceIndicatorTrimester() {
    init_flag = 0;
    grapha_bitmap = NULL;
}

//---------- End of function GraphPerformanceIndicatorTrimester::GraphPerformanceIndicatorTrimester ----//

//-------- Begin of function GraphPerformanceIndicatorTrimester::~GraphPerformanceIndicatorTrimester -----//
//!
GraphPerformanceIndicatorTrimester::~GraphPerformanceIndicatorTrimester() {
    deinit();
}

//---------- End of function GraphPerformanceIndicatorTrimester::~GraphPerformanceIndicatorTrimester -----//

//-------- Begin of function GraphPerformanceIndicatorTrimester::init --------//
//!
//! <int>		x1, y1				- upper left corner
//! <int>		x2, y2				- lower right corner
//! <int>		seriesNum			- number of series in the graph
//! <int*>	dataNum				- ptr to number of data in a series
//! <void*>	dataArray			- ptr to the data array
//!										  ( format: Series1Data1, Series1Data2, ...,
//!														Series1DataN, Series2Data1, ...,
//!														SeriesNDataN )
//! <char>	dataTypeFlag		- data array type
//!										  1 - float, 2 - double, 3 - int, 4 - long
//! [char]	transparentFlag	- 1 - transparent graph, 0 - solid graph
//!										  ( default = 1 )
//!
//! [char]	valueFlag			- 1 - display the most updated value, 0 - not display
//!										  ( default = 1 )
//!
//! [int]		numFormat			- format of numbers (follows m.format())
//!										  ( default = 1 )
//!
//! [int*]	seriesColor			- array of color of the data series
//!										  NULL = use default color ( default = NULL )
//!
//! [int]		axisColor			- color the the axis
//!										  ( default = Vga::active_buf->color_dark )
//!
//! [int]    defaultUpperBound - default upper bound of the graph.
//!
void GraphPerformanceIndicatorTrimester::init(int x1, int y1, int x2, int y2,
					      int seriesNum, int *dataNum,
					      void *dataArray,
					      char dataTypeFlag,
					      char transparentFlag,
					      char valueFlag,
					      int numFormat, double defaultUpperBound, int *seriesColor, int axisColor
    ) {
    err_when(seriesNum > DEFAULT_SERIES_COLOR_NUM && !seriesColor);
    err_when(seriesNum <= 0);

    graph_width = x2 - x1 - 6;

    err_when(graph_width < 0);

    grapha_x1 = x1;
    grapha_y1 = y1;
    grapha_x2 = x2;
    grapha_y2 = y2;
    series_num = seriesNum;
    data_num = dataNum;
    data_array = dataArray;
    data_type_flag = dataTypeFlag;
    transparent_flag = transparentFlag;
    value_flag = valueFlag;
    num_format = numFormat;
    series_color = seriesColor ? seriesColor : default_series_color;
    axis_color = axisColor;
    default_upper_bound = defaultUpperBound;

    init_flag = 1;

    if(value_flag)
	y_label_max_len = font_chartsm.text_width(MAX_LEN_LABEL);
    else
	y_label_max_len = 0;
    set_font(&font_chartsm);

}

//------------- End of function GraphPerformanceIndicatorTrimester::init -----------//

//-------- Begin of function GraphPerformanceIndicatorTrimester::deinit ------------//
//!
void GraphPerformanceIndicatorTrimester::deinit() {
    if (grapha_bitmap) {
	mem_del(grapha_bitmap);
	grapha_bitmap = NULL;
    }
    init_flag = 0;
}

//------------- End of function GraphPerformanceIndicatorTrimester::deinit ---------//

//-------- Begin of function GraphPerformanceIndicatorTrimester::set_font ------------//
//!
void GraphPerformanceIndicatorTrimester::set_font(Font *fontPtr) {
    if (!init_flag)
	return;

    font_ptr = fontPtr;

    calc_pos();
}

//------------- End of function GraphPerformanceIndicatorTrimester::set_font ---------//

//-------- Begin of function GraphPerformanceIndicatorTrimester::calc_pos ------------//
//!
//! calculate screen positions
//! should be called after changing display settings
//!
void GraphPerformanceIndicatorTrimester::calc_pos() {
    graph_height = grapha_y2 - grapha_y1 ;
    short YLabelWidth =  -COMMON_OFFSET;
    short XLabelHeight = -COMMON_OFFSET;
    series_x1 = grapha_x1 + 4 + COMMON_OFFSET*2;
    series_y1 = grapha_y1 + 4 + COMMON_OFFSET*4;
    series_x2 = grapha_x2 - 4 - COMMON_OFFSET*2;
    series_y2 = grapha_y1 + graph_height - COMMON_OFFSET*2;
}

//------------ End of function GraphPerformanceIndicatorTrimester::calc_pos ---------//

//-------- Begin of function GraphPerformanceIndicatorTrimester::paint --------------//
//!
void GraphPerformanceIndicatorTrimester::paint() {
    if (transparent_flag) {
	vga.use_back();

	user_interface.brighten(grapha_x1+4, grapha_y1+4, grapha_x2-2, grapha_y2-2);
	grapha_bitmap = vga_back.save_area(grapha_x1+4, grapha_y1+4, grapha_x2-2, grapha_y2-2, grapha_bitmap);
    }
    //	paint_flag = 1;
    refresh();
}

//------------- End of function GraphPerformanceIndicatorTrimester::paint -----------//

//-------- Begin of function GraphPerformanceIndicatorTrimester::refresh ------------//
//!
void GraphPerformanceIndicatorTrimester::refresh() {
    //	vga.use_back();

    if (transparent_flag) {
	user_interface.rect(grapha_x1, grapha_y1, grapha_x2, grapha_y2, 1);
	vga_back.rest_area(grapha_bitmap, 0, 0);
    }
    else {
	user_interface.bar(grapha_x1, grapha_y1, grapha_x2, grapha_y2);
	user_interface.panel(grapha_x1+3, grapha_y1+3, grapha_x2-4, grapha_y2-4);
    }
    draw_scale();
    draw_series();

    //	if (!paint_flag)
    //		vga.blt_buf(grapha_x1, grapha_y1, grapha_x2+2, grapha_y2+2);
    //	else
    //		paint_flag = 0;
}

//------------- End of function GraphPerformanceIndicatorTrimester::refresh ---------//

//---------- Begin of function GraphPerformanceIndicatorTrimester::find_scale -------------//
//!
void GraphPerformanceIndicatorTrimester::find_scale() {
    double data, maxVal = 0.0;
    x_axis_pos = 0;

    for (int i = 0; i < series_num * *data_num; i++) {
	switch (data_type_flag) {
	case DATA_DOUBLE:
	    data = ((double*)data_array)[i];
	    break;
	case DATA_FLOAT:
	    data = double(((float*)data_array)[i]);
	    break;
	case DATA_INT:
	    data = double(((int*)data_array)[i]);
	    break;
	case DATA_LONG:
	    data = double(((long*)data_array)[i]);
	    break;
	}
	if (fabs(data) > maxVal)
	    maxVal = fabs(data);
	if (!x_axis_pos && data < 0.0)
	    x_axis_pos = 1;
    }
    if (maxVal > default_upper_bound)
	max_scale = maxVal;
    else
	max_scale = default_upper_bound;
}

//------------ End of function GraphPerformanceIndicatorTrimester::find_scale -------------//

//---------- Begin of function GraphPerformanceIndicatorTrimester::draw_scale -------------//
//!
void GraphPerformanceIndicatorTrimester::draw_scale() {
    find_scale();
    //------ Draw y-axis -------//

    if (x_axis_pos)
	vga_back.bar(series_x1, series_y1-COMMON_OFFSET*2, series_x1+1, series_y2+COMMON_OFFSET*2, axis_color);
    else
	vga_back.bar(series_x1, series_y1-COMMON_OFFSET*2, series_x1+1, series_y2, axis_color);

    if (x_axis_pos)
	vga_back.bar(series_x1, series_y2+COMMON_OFFSET*2, series_x2-COMMON_OFFSET, series_y2+COMMON_OFFSET*2, axis_color);
    else
	vga_back.bar(series_x1, series_y2, series_x2-COMMON_OFFSET, series_y2, axis_color);

    //------ Draw x-axis -------//
    x_axis_step = (double)(series_x2-series_x1) / (*data_num-1);
}

//------------ End of function GraphPerformanceIndicatorTrimester::draw_scale -------------//

//---------- Begin of function GraphPerformanceIndicatorTrimester::draw_series ------------//
//!
void GraphPerformanceIndicatorTrimester::draw_series() {
    switch (data_type_flag) {
    case DATA_FLOAT: draw_series_float(); break;
    case DATA_DOUBLE: draw_series_double(); break;
    case DATA_INT: draw_series_int(); break;
    case DATA_LONG: draw_series_long(); break;
    }
}

//------------ End of function GraphPerformanceIndicatorTrimester::draw_series ------------//

//---------- Begin of function GraphPerformanceIndicatorTrimester::draw_series_float ------------//
//!
void GraphPerformanceIndicatorTrimester::draw_series_float() {
    float *dataArray = (float *) data_array;
    short prevX, prevY, currX, currY;

    for (int i = 0; i < series_num; i++) {
	dataArray-=info.graph_trimester_passed;
	dataArray+=HISTORY_TRIMESTER_COUNT;
	prevX = series_x1;
	if (x_axis_pos)
	    prevY = series_y2 - short(((*dataArray+float(max_scale)) * float(series_y2-series_y1) / float(max_scale*2)));
	else
	    prevY = series_y2 - short(*dataArray * float(series_y2-series_y1) / float(max_scale));

	dataArray++;
	vga_back.thick_line(prevX-1, prevY, prevX+1, prevY, vga_back.translate_color(series_color[i]));

	for (int j = 1; j < info.graph_trimester_passed; j++) {
	    currX = series_x1 + (int)(x_axis_step*j);
	    if (x_axis_pos)
		currY = series_y2 - short(((*dataArray+float(max_scale)) * float(series_y2-series_y1) / float(max_scale*2)));
	    else
		currY = series_y2 - short(*dataArray * float(series_y2-series_y1) / float(max_scale));

	    vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i]));

	    prevX = currX;
	    prevY = currY;
	    dataArray++;
	}

	if (value_flag)
	    draw_value(dataArray-1, vga_back.translate_color(V_BLACK), prevX+COMMON_OFFSET-(int)(0.3f*x_axis_step), prevY-font_ptr->max_font_height/2, DATA_FLOAT);
    }
}

//------------ End of function GraphPerformanceIndicatorTrimester::draw_series_float ------------//

//---------- Begin of function GraphPerformanceIndicatorTrimester::draw_series_double ------------//
//!
void GraphPerformanceIndicatorTrimester::draw_series_double() {
    double *dataArray = (double *) data_array;
    short prevX, prevY, currX, currY;

    for (int i = 0; i < series_num; i++) {
	dataArray-=info.graph_trimester_passed;
	dataArray+=HISTORY_TRIMESTER_COUNT;
	prevX = series_x1;
	if (x_axis_pos)
	    prevY = series_y2 - short(((*dataArray+max_scale) * double(series_y2-series_y1) / (max_scale*2)));
	else
	    prevY = series_y2 - short(*dataArray * double(series_y2-series_y1) / max_scale);

	dataArray++;
	vga_back.thick_line(prevX-1, prevY, prevX+1, prevY, vga_back.translate_color(series_color[i]));

	for (int j = 1; j < info.graph_trimester_passed; j++) {
	    currX = series_x1 + (int)(x_axis_step*j);
	    if (x_axis_pos)
		currY = series_y2 - short(((*dataArray+max_scale) * double(series_y2-series_y1) / (max_scale*2)));
	    else
		currY = series_y2 - short(*dataArray * double(series_y2-series_y1) / max_scale);

	    vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i]));

	    prevX = currX;
	    prevY = currY;
	    dataArray++;
	}

	if (value_flag)
	    draw_value(dataArray-1, vga_back.translate_color(V_BLACK), prevX+COMMON_OFFSET-(int)(0.3f*x_axis_step), prevY-font_ptr->max_font_height/2, DATA_DOUBLE);
    }
}

//------------ End of function GraphPerformanceIndicatorTrimester::draw_series_double ------------//

//---------- Begin of function GraphPerformanceIndicatorTrimester::draw_series_int ------------//
//!
void GraphPerformanceIndicatorTrimester::draw_series_int() {
    int *dataArray = (int *) data_array;

    short prevX, prevY, currX, currY;

    for (int i = 0; i < series_num; i++) {
	dataArray-=info.graph_trimester_passed;
	dataArray+=HISTORY_TRIMESTER_COUNT;
	prevX = series_x1;
	if (x_axis_pos)
	    prevY = series_y2 - ((*dataArray+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
	else
	    prevY = series_y2 - *dataArray * (series_y2-series_y1) / int(max_scale);

	dataArray++;
	vga_back.thick_line(prevX-1, prevY, prevX+1, prevY, vga_back.translate_color(series_color[i]));

	for (int j = 1; j < info.graph_trimester_passed; j++) {
	    currX = series_x1 + (int)(x_axis_step*j);
	    if (x_axis_pos)
		currY = series_y2 - ((*dataArray+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
	    else
		currY = series_y2 - *dataArray * (series_y2-series_y1) / int(max_scale);

	    vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i]));

	    prevX = currX;
	    prevY = currY;
	    dataArray++;
	}

	if (value_flag)
	    draw_value(dataArray-1, vga_back.translate_color(V_BLACK), prevX+COMMON_OFFSET-(int)(0.3f*x_axis_step), prevY-font_ptr->max_font_height/2, DATA_INT);
    }
}

//------------ End of function GraphPerformanceIndicatorTrimester::draw_series_int ------------//

//---------- Begin of function GraphPerformanceIndicatorTrimester::draw_series_long ------------//
//!
void GraphPerformanceIndicatorTrimester::draw_series_long() {
    long *dataArray = (long *) data_array;
    short prevX, prevY, currX, currY;

    for (int i = 0; i < series_num; i++) {
	dataArray-=info.graph_trimester_passed;
	dataArray+=HISTORY_TRIMESTER_COUNT;
	prevX = series_x1;
	if (x_axis_pos)
	    prevY = series_y2 - short(((*dataArray+long(max_scale)) * long(series_y2-series_y1) / long(max_scale*2)));
	else
	    prevY = series_y2 - short(*dataArray * long(series_y2-series_y1) / long(max_scale));

	dataArray++;
	vga_back.thick_line(prevX-1, prevY, prevX+1, prevY, vga_back.translate_color(series_color[i]));

	for (int j = 1; j < info.graph_trimester_passed; j++) {
	    currX = series_x1 + (int)(x_axis_step*j);
	    if (x_axis_pos)
		currY = series_y2 - short(((*dataArray+long(max_scale)) * long(series_y2-series_y1) / long(max_scale*2)));
	    else
		currY = series_y2 - short(*dataArray * long(series_y2-series_y1) / long(max_scale));

	    vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i]));

	    prevX = currX;
	    prevY = currY;
	    dataArray++;
	}

	if (value_flag)
	    draw_value(dataArray-1, vga_back.translate_color(V_BLACK), prevX+COMMON_OFFSET-(int)(0.3f*x_axis_step), prevY-font_ptr->max_font_height/2, DATA_LONG);
    }
}

//------------ End of function GraphPerformanceIndicatorTrimester::draw_series_float ------------//

//---------- Begin of function GraphPerformanceIndicatorTrimester::draw_value -------------------//
//!
void GraphPerformanceIndicatorTrimester::draw_value(void *value, int color, short x, short y, int dataType) {
    char *valueString;
    switch (data_type_flag) {
    case DATA_FLOAT:
	valueString = m.format(*((float *) value), num_format); break;
    case DATA_DOUBLE:
	valueString = m.format(*((double *) value), num_format); break;
    case DATA_INT:
	valueString = m.format(*((int *) value), num_format); break;
    case DATA_LONG:
	valueString = m.format(*((long *) value), num_format); break;
    }

    short textHeight = font_ptr->max_font_height;
    short textWidth = font_ptr->text_width(valueString) + 1;
    /*	
	short *bitmap = (short *) sys.common_data_buf;
	err_when( (2 + textWidth * textHeight) * sizeof(short) > COMMON_DATA_BUF_SIZE );
	*bitmap++ = textWidth;
	*bitmap++ = textHeight;

	short *shortPtr = bitmap;
	for (int i = 0; i < textWidth * textHeight; i++)
	*shortPtr++ = transparent_code_w;

	font_ptr->put_to_bufferW(bitmap, textWidth * sizeof(short), 0, 0, valueString);

	shortPtr = bitmap;
	for (i = 0; i < textWidth * textHeight; i++, shortPtr++)
	if (*shortPtr != transparent_code_w)
	*shortPtr = color;

	vga_back.put_bitmapW_trans(x, y, (short *) sys.common_data_buf);
    */
    font_chartsm.put(x+3,y,valueString);
}

//---------- End of function GraphPerformanceIndicatorTrimester::draw_value -------------------//
