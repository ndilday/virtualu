//Filename    : OPIECHRT.CPP
//Description : Object PieChart
//Owner			: Kevin(Ho)

#include <ALL.H>
#include <COLOR.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OPIECHRT.H>
#include <OIFACE.H>

#include <math.h>

//--------- Define macro constant -----------//

#define MAX_LEN_LABEL "000"

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

//-------- Begin of function PieChart::PieChart ----//
//!
PieChart::PieChart() {
    init_flag = 0;
    piechart_bitmap = NULL;
}

//---------- End of function PieChart::PieChart ----//

//-------- Begin of function PieChart::~PieChart -----//
//!
PieChart::~PieChart() {
    deinit();
}

//---------- End of function PieChart::~PieChart -----//

//-------- Begin of function PieChart::init --------//
//!
//! <int>		x1, y1				- upper left corner
//! <int>		x2, y2				- lower right corner
//! <int>	dataNum					- number of data
//! <void*>	dataArray			- ptr to the data array
//!										  ( format: Series1Data1, Series1Data2, ...,
//!														Series1DataN, Series2Data1, ...,
//!														SeriesNDataN )
//! <char>	dataTypeFlag		- data array type
//!										  1 - float, 2 - double, 3 - int, 4 - long
//! <int*>	xStart				- x-axis start value
//! <int*>	xEnd					- x-axis end value
//! [char*]	xLabel				- x-axis label, NULL = no label
//!										  ( default = NULL )
//! [char*]	yLabel				- y-axis label, NULL = no label
//!										  ( default = NULL )
//! [char**]	legendArray			- ptr to array of legend strings, NULL = no legend
//!										  ( default = NULL )
//! [char]	transparentFlag	- 1 - transparent graph, 0 - solid graph
//!										  ( default = 1 )
//! [char]	orientationFlag	- y-axis label orientation
//!										  0 - horizontal, 1 - vertical
//!										  ( default = 1 )
//! [char]	valueFlag			- 1 - display the most updated value, 0 - not display
//!										  ( default = 1 )
//! [int]		numFormat			- format of numbers (follows m.format())
//!										  ( default = 1 )
//! [int*]	seriesColor			- array of color of the data series
//!										  NULL = use default color ( default = NULL )
//! [int]		axisColor			- color the the axis
//!										  ( default = Vga::active_buf->color_dark )
//!
void PieChart::init(int x1, int y1, int x2, int y2, int dataNum,
		    void *dataArray, char dataTypeFlag,
		    int *xStart, int *xEnd, char *xLabel, char *yLabel,
		    char **legendArray, char transparentFlag,
		    char orientationFlag, char valueFlag,
		    int numFormat, int *seriesColor, int axisColor) {
    err_when(dataNum > DEFAULT_SERIES_COLOR_NUM && !seriesColor);
    err_when(dataNum <= 0);
    err_when(*xEnd < *xStart);

    graph_width = x2 - x1 - 6;

    err_when(graph_width < 0);

    piechart_x1 = x1;
    piechart_y1 = y1;
    piechart_x2 = x2;
    piechart_y2 = y2;

    centerx = (piechart_x1+piechart_x2)/2;
    centery = (piechart_y1+piechart_y2)/2;
    int radius1 = (piechart_x2-piechart_x1)/2-3*COMMON_OFFSET;
    int radius2 = (piechart_y2-piechart_y1)/2-3*COMMON_OFFSET;
    radius = (radius1>radius2)?radius2:radius1;
    series_num = dataNum;
    data_num = dataNum;
    data_array = dataArray;
    data_type_flag = dataTypeFlag;
    x_start = xStart;
    x_end = xEnd;

    x_label = xLabel;
    y_label = yLabel;
    legend_array = legendArray;

    transparent_flag = transparentFlag;
    orientation_flag = orientationFlag;
    value_flag = valueFlag;
    num_format = num_format;

    series_color = seriesColor ? seriesColor : default_series_color;
    axis_color = axisColor;

    init_flag = 1;

    y_label_max_len = font_chartsm.text_width(MAX_LEN_LABEL);
    set_font(&font_chartsm);

}

//------------- End of function PieChart::init -----------//

//-------- Begin of function PieChart::deinit ------------//
//!
void PieChart::deinit() {
    if (piechart_bitmap) {
	mem_del(piechart_bitmap);
	piechart_bitmap = NULL;
    }

    init_flag = 0;
}

//------------- End of function PieChart::deinit ---------//

//-------- Begin of function PieChart::set_font ------------//
//!
void PieChart::set_font(Font *fontPtr) {
    if (!init_flag)
	return;

    font_ptr = fontPtr;

    calc_pos();
}

//------------- End of function PieChart::set_font ---------//

//-------- Begin of function PieChart::calc_pos ------------//
//!
//! calculate screen positions
//! should be called after changing display settings
//!
void PieChart::calc_pos() {
    if (legend_array) {
	short textWidth = 0, textHeight = font_ptr->max_font_height;

	for (int i = 0; i < series_num; i++) {
	    if (font_ptr->text_width(legend_array[i]) > textWidth)
		textWidth = font_ptr->text_width(legend_array[i]);
	}

	err_when(textWidth + LEGEND_X_SPACING * 2 > graph_width);

	legend_x_num = (graph_width - LEGEND_X_SPACING*2) / (textWidth + LEGEND_X_SPACING*2);
	legend_y_num = (series_num / legend_x_num) + ((series_num % legend_x_num) ? 1 : 0);

	graph_height = piechart_y2 - piechart_y1 - legend_y_num * (textHeight + LEGEND_Y_SPACING) - 6;

	err_when(graph_height <= 0);

	legend_width = (graph_width - LEGEND_X_SPACING*2) / legend_x_num;
	legend_height = textHeight + LEGEND_Y_SPACING;
    }
    else {
	legend_width = legend_height = legend_x_num = legend_y_num = 0;
	graph_height = piechart_y2 - piechart_y1 - 6;
    }

    short YLabelWidth = y_label ? (orientation_flag ? font_ptr->max_font_height : font_ptr->text_width(y_label)) : -COMMON_OFFSET;
    short XLabelHeight = x_label ? (font_ptr->max_font_height) : -COMMON_OFFSET;
    series_x1 = piechart_x1 + 4 + COMMON_OFFSET*4 + YLabelWidth + y_label_max_len;
    series_y1 = piechart_y1 + 4 + COMMON_OFFSET*6;
    series_x2 = piechart_x2 - 4 - COMMON_OFFSET*12;
    series_y2 = piechart_y1 + graph_height - COMMON_OFFSET*3 - (font_ptr->max_font_height) - XLabelHeight;
}

//------------ End of function PieChart::calc_pos ---------//

//-------- Begin of function PieChart::paint --------------//
//!
void PieChart::paint() {
    if (transparent_flag) {
	user_interface.brighten(piechart_x1+4, piechart_y1+4, piechart_x2-2, piechart_y2-2);
	piechart_bitmap = vga_back.save_area(piechart_x1+4, piechart_y1+4, piechart_x2-2, piechart_y2-2, piechart_bitmap);
    }

    //	paint_flag = 1;
    refresh();
}

//------------- End of function PieChart::paint -----------//

//-------- Begin of function PieChart::refresh ------------//
//!
void PieChart::refresh() {
    //	vga.use_back();

    if (transparent_flag) {
	user_interface.rect(piechart_x1, piechart_y1, piechart_x2, piechart_y2, 1);
	vga_back.rest_area(piechart_bitmap, 0, 0);
    }
    else {
	user_interface.bar(piechart_x1, piechart_y1, piechart_x2, piechart_y2);
	user_interface.panel(piechart_x1+3, piechart_y1+3, piechart_x2-4, piechart_y2-4);
    }
    draw_series();

    //	if (paint_flag)
    //		paint_flag = 0;
    //		vga.blt_buf(piechart_x1, piechart_y1, piechart_x2+2, piechart_y2+2);
    //	else

}

//------------- End of function PieChart::refresh ---------//

//---------- Begin of function PieChart::draw_legend ------------//
//!
void PieChart::draw_legend() {
    for (int i = 0; i < series_num; i++) {
	short legendX = piechart_x1 + 4 + (i % legend_x_num) * legend_width + LEGEND_X_SPACING;
	short legendY = piechart_y1 + 4 + graph_height + (i / legend_x_num) * legend_height;
	user_interface.bar( legendX+LEGEND_X_SPACING, legendY+LEGEND_Y_SPACING,
			    legendX+LEGEND_X_SPACING+LEGEND_SIZE, legendY+LEGEND_Y_SPACING+LEGEND_SIZE,
			    series_color[i] );
	font_ptr->put( legendX+LEGEND_X_SPACING*2+LEGEND_SIZE, legendY+LEGEND_Y_SPACING, legend_array[i] );
    }
}

//------------ End of function PieChart::draw_legend ------------//

//---------- Begin of function PieChart::draw_axis_label --------//
//!
void PieChart::draw_axis_label() {
    short textHeight = font_ptr->max_font_height;

    if (x_label) {
	font_ptr->center_put( series_x1, series_y2+COMMON_OFFSET*2+textHeight,
			      series_x2, piechart_y1+graph_height-COMMON_OFFSET, x_label );
    }

    if (y_label) {
	short textWidth = font_ptr->text_width(y_label) + 1;

	if (orientation_flag) {
	    short *fontBuf = (short *) sys.common_data_buf;
	    err_when(textWidth * textHeight * sizeof(short) > COMMON_DATA_BUF_SIZE);

	    for (int i = 0; i < textWidth * textHeight; i++)
		*fontBuf++ = transparent_code_w;

	    font_ptr->put_to_bufferW(fontBuf = (short *) sys.common_data_buf, textWidth * sizeof(short), 0, 0, y_label);

	    short xStart = piechart_x1 + 4 + COMMON_OFFSET * 2;
	    short yStart = (series_y1 + series_y2 - textWidth) / 2;

	    for (short x = 0; x < textHeight; x++) {
		for (short y = textWidth - 1; y >= 0; y--, fontBuf++) {
		    if (*fontBuf != transparent_code_w)
			*vga_back.buf_ptr(xStart+x, yStart+y) = *fontBuf;
		}
	    }
	}
	else {
	    font_ptr->center_put( piechart_x1+4+COMMON_OFFSET*2, series_y1,
				  piechart_x1+4+COMMON_OFFSET*2+textWidth, series_y2, y_label);
	}
    }
}

//------------ End of function PieChart::draw_axis_label --------//

//---------- Begin of function PieChart::find_scale -------------//
//!
void PieChart::find_scale() {
    double data, maxVal = 0.0;
    x_axis_pos = 0;

    for (int i = 0; i < series_num * data_num; i++) {
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
    if (maxVal>0)
	max_scale = pow(10,ceil(log10(maxVal)));
    else
	max_scale = 5;
}

//------------ End of function PieChart::find_scale -------------//

//---------- Begin of function PieChart::draw_scale -------------//
//!
void PieChart::draw_scale() {
    find_scale();

    short xAxisPos;

    //------ Draw y-axis -------//

    if (x_axis_pos)
	vga_back.bar(series_x1, series_y1-COMMON_OFFSET*2, series_x1+1, series_y2+COMMON_OFFSET*2, axis_color);
    else
	vga_back.bar(series_x1, series_y1-COMMON_OFFSET*2, series_x1+1, series_y2, axis_color);

    short scaleNum = (series_y2-series_y1-font_ptr->max_font_height) / (font_ptr->max_font_height*3/2);
    if ( scaleNum > 0 )
	scaleNum = short(pow(2,floor(log10(double(scaleNum))/log10(2.0))));
    else
	scaleNum = 0;
    short scaleStep = (series_y2-series_y1) / scaleNum;
    double scaleValue;
    for (int i = 0; i <= scaleNum; i++) {
	if ( (x_axis_pos && i == scaleNum / 2) || !(x_axis_pos || i) )
	    vga_back.bar(series_x1, xAxisPos=series_y2-i*scaleStep-1, series_x2+COMMON_OFFSET*8, series_y2-i*scaleStep, axis_color);
	else
	    vga_back.bar(series_x1, series_y2-i*scaleStep, series_x1+COMMON_OFFSET*2, series_y2-i*scaleStep, axis_color);

	if (x_axis_pos)
	    scaleValue = max_scale*2/scaleNum*i - max_scale;
	else
	    scaleValue = max_scale/scaleNum*i;

	switch (data_type_flag) {
	case DATA_FLOAT:
	case DATA_DOUBLE:
	    font_ptr->right_put(series_x1-COMMON_OFFSET, series_y2-i*scaleStep-font_ptr->max_font_height/2, m.format(scaleValue,num_format));
	    break;
	case DATA_INT:
	case DATA_LONG:
	    font_ptr->right_put(series_x1-COMMON_OFFSET, series_y2-i*scaleStep-font_ptr->max_font_height/2, m.format(long(scaleValue),num_format));
	}
    }

    //------ Draw x-axis -------//

    short maxLabelWidth = font_ptr->text_width(m.format(*x_end, 4))+COMMON_OFFSET*2;
    x_axis_step = (series_x2-series_x1) / (data_num-1);
    short displayInc = 1;
    while (displayInc * x_axis_step / maxLabelWidth < 1) displayInc++;
    int xAxisStep = (*x_end - *x_start) / (data_num - 1);
    for (int i = 0; i < data_num-1; i += displayInc) {
	vga_back.bar(series_x2-i*x_axis_step, xAxisPos-COMMON_OFFSET*2, series_x2-i*x_axis_step, xAxisPos, axis_color);
	font_ptr->center_put((series_x2-i*x_axis_step)-maxLabelWidth/2-COMMON_OFFSET, xAxisPos+COMMON_OFFSET,
			     (series_x2-i*x_axis_step)+maxLabelWidth/2+COMMON_OFFSET, xAxisPos+COMMON_OFFSET+font_ptr->max_font_height, m.format(*x_end-xAxisStep*i, 4));
    }
}

//------------ End of function PieChart::draw_scale -------------//

//---------- Begin of function PieChart::draw_series ------------//
//!
void PieChart::draw_series() {
    switch (data_type_flag) {
    case DATA_FLOAT: draw_series_float(); break;
    case DATA_DOUBLE: draw_series_double(); break;
    case DATA_INT: draw_series_int(); break;
    case DATA_LONG: draw_series_long(); break;
    }
}

//------------ End of function PieChart::draw_series ------------//

//---------- Begin of function PieChart::draw_pie ------------//
//!
void PieChart::draw_pie(double theta1, double theta2, int piecolor) {
    //assume theta2 > theta 1
    double t1=theta1, t2=theta2;
    double sint3,cost3,t3,t4,t5,x,y;
    double skip=1.0;

    if(theta1==theta2)return;

    t3=0.5*(t1+t2);
    sint3=sin(t3);
    cost3=cos(t3);
    t4=2*(skip/(t2-t1));
    t5=skip/radius;
    x=centerx;
    y=centery;
    while(t1<=t2) {
	vga_back.thick_line(
	    (int)x, (int)y, centerx+(int)(radius*cos(t1)), centery+(int)(radius*sin(t1)),
	    vga_back.translate_color(series_color[piecolor]));
	vga_back.thick_line(
	    (int)x, (int)y, centerx+(int)(radius*cos(t2)), centery+(int)(radius*sin(t2)),
	    vga_back.translate_color(series_color[piecolor]));

	x+=t4*cost3;
	y+=t4*sint3;
	t1+=t5;
	t2-=t5;
    }
    t1=theta1;
    t2=theta2;
    while(t1<=t2) {
	vga_back.thick_line(
	    centerx+(int)(radius*cos(t2)), centery+(int)(radius*sin(t2)),
	    centerx+(int)(radius*cos(t2-=t5)), centery+(int)(radius*sin(t2)),
	    vga_back.translate_color(V_GRAY-5));
	vga_back.thick_line(
	    centerx+(int)(radius*cos(t1)), centery+(int)(radius*sin(t1)),
	    centerx+(int)(radius*cos(t1+=t5)), centery+(int)(radius*sin(t1)),
	    vga_back.translate_color(V_GRAY-5));
	x+=t4*cost3;
	y+=t4*sint3;
    }

}

//---------- End of function PieChart::draw_pie ------------//

//---------- Begin of function PieChart::draw_series_int ------------//
//!
void PieChart::draw_series_int() {
    int *dataArray = (int *) data_array;
    long total=0;
    double theta1,theta2;

    for (int i = 0; i < data_num; i++)
	total+=(long)(dataArray[i]);

    if(total<1E-9)total=1;
    theta2=0.0;
    for (int i = 0; i < data_num; i++) {
	theta1=theta2;
	theta2+=6.2832*(double)dataArray[i]/total;
	draw_pie(theta1,theta2,i);

	if((theta1!=theta2)
	   &&(fabs(theta2-theta1-6.2832)>1e-9)) {
	    vga_back.thick_line(
		centerx,
		centery,
		centerx+(int)(radius*cos(theta1)),
		centery+(int)(radius*sin(theta1)),
		vga_back.translate_color(V_GRAY-5));
	    vga_back.thick_line(
		centerx,
		centery,
		centerx+(int)(radius*cos(theta2)),
		centery+(int)(radius*sin(theta2)),
		vga_back.translate_color(V_GRAY-5));
	}
    }

}

//------------ End of function PieChart::draw_series_long ------------//

void PieChart::draw_series_long() {
    long *dataArray = (long *) data_array;
    long total=0;
    double theta1,theta2;

    for (int i = 0; i < data_num; i++)
	total+=(long)(dataArray[i]);
    if(total<1E-9)total=1;

    theta2=0.0;
    for (int i = 0; i < data_num; i++) {
	theta1=theta2;
	theta2+=6.2832*(double)dataArray[i]/total;
	draw_pie(theta1,theta2,i);
    }
    theta1=0.0;
    for (int i = 0; i <= data_num; i++) {
	vga_back.thick_line(
	    centerx, centery,
	    centerx+(int)(radius*cos(theta1)), centery+(int)(radius*sin(theta1)),
	    vga_back.translate_color(V_GRAY-5));
	theta1+=6.2832*(double)dataArray[i]/total;
    }
}

//------------ End of function PieChart::draw_series_long ------------//

//------------ End of function PieChart::draw_series_float ------------//

void PieChart::draw_series_float() {
    float *dataArray = (float *) data_array;
    float total=0;
    double theta1,theta2;

    for (int i = 0; i < data_num; i++)
	total+=(float)(dataArray[i]);

    if(total<1E-9)total=1;

    theta2=0.0;
    for (int i = 0; i < data_num; i++) {
	theta1=theta2;
	theta2+=6.2832*(double)dataArray[i]/total;
	draw_pie(theta1,theta2,i);
    }
    theta1=0.0;
    for (int i = 0; i <= data_num; i++) {
	vga_back.thick_line(
	    centerx, centery,
	    centerx+(int)(radius*cos(theta1)), centery+(int)(radius*sin(theta1)),
	    vga_back.translate_color(V_GRAY-5));
	theta1+=6.2832*(double)dataArray[i]/total;
    }

}

//------------ End of function PieChart::draw_series_float ------------//

//------------ End of function PieChart::draw_series_long ------------//

void PieChart::draw_series_double() {
    double *dataArray = (double *) data_array;
    double total=0;
    double theta1,theta2;

    for (int i = 0; i < data_num; i++)
	total+=(double)(dataArray[i]);

    if(total<1E-9)total=1;

    theta2=0.0;
    for (int i = 0; i < data_num; i++) {
	theta1=theta2;
	theta2+=6.2832*(double)dataArray[i]/total;
	draw_pie(theta1,theta2,i);
    }

    theta1=0.0;
    for (int i = 0; i <= data_num; i++) {
	vga_back.thick_line(
	    centerx, centery,
	    centerx+(int)(radius*cos(theta1)), centery+(int)(radius*sin(theta1)),
	    vga_back.translate_color(V_GRAY-5));
	theta1+=6.2832*(double)dataArray[i]/total;
    }

}

//------------ End of function PieChart::draw_series_float ------------//

//---------- Begin of function PieChart::draw_value -------------------//
//!
void PieChart::draw_value(void *value, int color, short x, short y, int dataType) {
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

//---------- Begin of function PieChart::draw_value -------------------//
