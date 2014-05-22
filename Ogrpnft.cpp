//Filename    : OGRPNFT.CPP
//Description : Object GraphNonFinancialTrimester	(Trimester-based)
//Owner			: Kevin(Ho)

// The data is updated at 1st Sep of each year.
#include <ALL.H>
#include <COLOR.H>
#include <OINFO.H>
#include <OPSCHOOL.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OGRPNFT.H>
#include <OIFACE.H>
#include <math.h>

//--------- Define macro constant -----------//

#define MAX_LEN_LABEL "00,000"

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

//-------- Begin of function GraphNonFinancialTrimester::GraphNonFinancialTrimester ----//
//!
GraphNonFinancialTrimester::GraphNonFinancialTrimester() {
    init_flag = 0;
    graph_bitmap = NULL;
}

//---------- End of function GraphNonFinancialTrimester::GraphNonFinancialTrimester ----//

//-------- Begin of function GraphNonFinancialTrimester::~GraphNonFinancialTrimester -----//
//!
GraphNonFinancialTrimester::~GraphNonFinancialTrimester() {
    deinit();
}

//---------- End of function GraphNonFinancialTrimester::~GraphNonFinancialTrimester -----//

//-------- Begin of function GraphNonFinancialTrimester::init --------//
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
//! [int]		thisTrimesterIndicator	- position of the vertical line which indicates this year.
//!										- -1 : no line
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
void GraphNonFinancialTrimester::init(int x1, int y1, int x2, int y2,
				      int seriesNum, int *dataNum,
				      void *dataArray, char dataTypeFlag,
				      int thisTrimesterIndicator,
				      char *xLabel, char *yLabel,
				      char **legendArray, char transparentFlag,
				      char orientationFlag, char valueFlag,
				      int numFormat, int *seriesColor, int axisColor) {
    err_when(seriesNum > DEFAULT_SERIES_COLOR_NUM && !seriesColor);
    err_when(seriesNum <= 0);

    graph_width = x2 - x1 - 6;

    err_when(graph_width < 0);

    graph_x1 = x1;
    graph_y1 = y1;
    graph_x2 = x2;
    graph_y2 = y2;
    series_num = seriesNum;
    data_num = dataNum;
    data_array = dataArray;
    data_type_flag = dataTypeFlag;

    x_label = xLabel;
    y_label = yLabel;
    legend_array = legendArray;

    transparent_flag = transparentFlag;
    orientation_flag = orientationFlag;
    value_flag = valueFlag;
    num_format = numFormat;

    series_color = seriesColor ? seriesColor : default_series_color;
    axis_color = axisColor;

    init_flag = 1;
    this_trimester_indicator = thisTrimesterIndicator;
    y_label_max_len = font_chartsm.text_width(MAX_LEN_LABEL);
    set_font(&font_chartsm);
}

//------------- End of function GraphNonFinancialTrimester::init -----------//

//-------- Begin of function GraphNonFinancialTrimester::deinit ------------//
//!
void GraphNonFinancialTrimester::deinit() {
    if (graph_bitmap) {
	mem_del(graph_bitmap);
	graph_bitmap = NULL;
    }

    init_flag = 0;
}

//------------- End of function GraphNonFinancialTrimester::deinit ---------//

//-------- Begin of function GraphNonFinancialTrimester::set_font ------------//
//!
void GraphNonFinancialTrimester::set_font(Font *fontPtr) {
    if (!init_flag)
	return;

    font_ptr = fontPtr;

    calc_pos();
}

//------------- End of function GraphNonFinancialTrimester::set_font ---------//

//-------- Begin of function GraphNonFinancialTrimester::calc_pos ------------//
//!
//! calculate screen positions
//! should be called after changing display settings
//!
void GraphNonFinancialTrimester::calc_pos() {
    if (legend_array) {
	short textWidth = 0, textHeight = font_ptr->max_font_height;

	for (int i = 0; i < series_num; i++) {
	    if (font_ptr->text_width(legend_array[i]) > textWidth)
		textWidth = font_ptr->text_width(legend_array[i]);
	}

	err_when(textWidth + LEGEND_X_SPACING * 2 > graph_width);

	legend_x_num = (graph_width - LEGEND_X_SPACING*2) / (textWidth + LEGEND_X_SPACING*2);
	legend_y_num = (series_num / legend_x_num) + ((series_num % legend_x_num) ? 1 : 0);

	graph_height = graph_y2 - graph_y1 - legend_y_num * (textHeight + LEGEND_Y_SPACING) - 6;

	err_when(graph_height <= 0);

	legend_width = (graph_width - LEGEND_X_SPACING*2) / legend_x_num;
	legend_height = textHeight + LEGEND_Y_SPACING;
    }
    else {
	legend_width = legend_height = legend_x_num = legend_y_num = 0;
	graph_height = graph_y2 - graph_y1 - 6;
    }

    short YLabelWidth = y_label ? (orientation_flag ? font_ptr->max_font_height : font_ptr->text_width(y_label)) : -COMMON_OFFSET;
    short XLabelHeight = x_label ? (font_ptr->max_font_height) : -COMMON_OFFSET;
    series_x1 = graph_x1 + 4 + COMMON_OFFSET*4 + YLabelWidth + y_label_max_len;
    series_y1 = graph_y1 + 4 + COMMON_OFFSET*6;
    series_x2 = graph_x2 - 4 - COMMON_OFFSET*16;
    series_y2 = graph_y1 + graph_height - COMMON_OFFSET*3 - (font_ptr->max_font_height);
}

//------------ End of function GraphNonFinancialTrimester::calc_pos ---------//

//-------- Begin of function GraphNonFinancialTrimester::paint --------------//
//!
void GraphNonFinancialTrimester::paint() {
    if (transparent_flag) {
	user_interface.brighten(graph_x1+4, graph_y1+4, graph_x2-2, graph_y2-2);
	if (legend_array)
	    draw_legend();
	draw_axis_label();
	graph_bitmap = vga_back.save_area(graph_x1+4, graph_y1+4, graph_x2-2, graph_y2-2, graph_bitmap);
    }

    //	paint_flag = 1;
    refresh();
}

//------------- End of function GraphNonFinancialTrimester::paint -----------//

//-------- Begin of function GraphNonFinancialTrimester::refresh ------------//
//!
void GraphNonFinancialTrimester::refresh() {
    if (transparent_flag) {
	user_interface.rect(graph_x1, graph_y1, graph_x2, graph_y2, 1);
	vga_back.rest_area(graph_bitmap, 0, 0);
    }
    else {
	user_interface.bar(graph_x1, graph_y1, graph_x2, graph_y2);
	user_interface.panel(graph_x1+3, graph_y1+3, graph_x2-4, graph_y2-4);
    }
    if (legend_array)
	draw_legend();
    draw_scale();
    draw_series();

    //	if (!paint_flag)
    //		vga.blt_buf(graph_x1, graph_y1, graph_x2+2, graph_y2+2);
    //	else
    //		paint_flag = 0;
}

//------------- End of function GraphNonFinancialTrimester::refresh ---------//

//---------- Begin of function GraphNonFinancialTrimester::draw_legend ------------//
//!
void GraphNonFinancialTrimester::draw_legend() {
    for (int i = 0; i < series_num; i++) {
	short legendX = graph_x1 + 4 + (i % legend_x_num) * legend_width + LEGEND_X_SPACING;
	short legendY = graph_y1 + 4 + graph_height + (i / legend_x_num) * legend_height;
	user_interface.bar( legendX+LEGEND_X_SPACING, legendY+LEGEND_Y_SPACING+LEGEND_SIZE/2-1,
			    legendX+LEGEND_X_SPACING+LEGEND_SIZE, legendY+LEGEND_Y_SPACING+LEGEND_SIZE/2+1,
			    series_color[i] );
	font_ptr->put( legendX+LEGEND_X_SPACING*2+LEGEND_SIZE, legendY+LEGEND_Y_SPACING, legend_array[i] );
    }
}

//------------ End of function GraphNonFinancialTrimester::draw_legend ------------//

//---------- Begin of function GraphNonFinancialTrimester::draw_axis_label --------//
//!
void GraphNonFinancialTrimester::draw_axis_label() {
    short textHeight = font_ptr->max_font_height;

    if (x_label) {
	font_ptr->center_put( series_x1, series_y2+COMMON_OFFSET*2+textHeight,
			      series_x2, graph_y1+graph_height-COMMON_OFFSET, x_label );
    }

    if (y_label) {
	short textWidth = font_ptr->text_width(y_label) + 1;

	if (orientation_flag) {
	    short *fontBuf = (short *) sys.common_data_buf;
	    err_when(textWidth * textHeight * sizeof(short) > COMMON_DATA_BUF_SIZE);

	    for (int i = 0; i < textWidth * textHeight; i++)
		*fontBuf++ = transparent_code_w;

	    font_ptr->put_to_bufferW(fontBuf = (short *) sys.common_data_buf, textWidth * sizeof(short), 0, 0, y_label);

	    short xStart = graph_x1 + 4 + COMMON_OFFSET * 2;
	    short yStart = (series_y1 + series_y2 - textWidth) / 2;

	    for (short x = 0; x < textHeight; x++) {
		for (short y = textWidth - 1; y >= 0; y--, fontBuf++) {
		    if (*fontBuf != transparent_code_w)
			*vga_back.buf_ptr(xStart+x, yStart+y) = *fontBuf;
		}
	    }
	}
	else {
	    font_ptr->center_put( graph_x1+4+COMMON_OFFSET*2, series_y1,
				  graph_x1+4+COMMON_OFFSET*2+textWidth, series_y2, y_label);
	}
    }
}

//------------ End of function GraphNonFinancialTrimester::draw_axis_label --------//

//---------- Begin of function GraphNonFinancialTrimester::find_scale -------------//
//!
void GraphNonFinancialTrimester::find_scale() {
    double data, maxVal = 0.0,minVal = 0.0;

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
	if (data > maxVal)
	    maxVal = data;
	if (data < minVal)
	    minVal = data;
    }
    if (maxVal>0)
	max_scale = maxVal;
    else
	max_scale = 10;

    if (minVal<0)
	min_scale = minVal;
    else
	min_scale = 0;

}

//------------ End of function GraphNonFinancialTrimester::find_scale -------------//

//---------- Begin of function GraphNonFinancialTrimester::draw_scale -------------//
//!
void GraphNonFinancialTrimester::draw_scale() {
    find_scale();

    //------ Draw y-axis -------//
    vga_back.bar(series_x1, series_y1-COMMON_OFFSET*2, series_x1+1, series_y2, axis_color);

    xAxisPos=(short)(series_y1+(series_y2-series_y1)*(max_scale)/(max_scale-min_scale));

    posScaleNum = (xAxisPos-series_y1-font_ptr->max_font_height) / (font_ptr->max_font_height*3/2);
    if ( posScaleNum > 0 )
	posScaleNum = short(pow(2,floor(log10(double(posScaleNum))/log10(2.0))));
    else
	posScaleNum = 0;
    negScaleNum = (series_y2-xAxisPos-font_ptr->max_font_height) / (font_ptr->max_font_height*3/2);
    if ( negScaleNum > 0 )
	negScaleNum = short(pow(2,ceil(log10(double(negScaleNum))/log10(2.0))));
    else
	negScaleNum = 0;

    if ( posScaleNum + negScaleNum <= 0) {
	posScaleNum = 1;
	negScaleNum = 0;
    }

    short posScaleStep;
    short negScaleStep;

    if(posScaleNum<1) {
	posScaleStep = 0;
	posScaleInc = 0;
    }
    else {
	posScaleStep  = (xAxisPos-series_y1) / posScaleNum;
	posScaleInc   = max_scale / posScaleNum;
	if( data_type_flag==DATA_INT || data_type_flag==DATA_LONG )
	    posScaleInc = ceil(posScaleInc);            // don't like integer type to have non-integral posScaleInc
    }

    if(negScaleNum<1) {
	negScaleStep = 0;
	negScaleInc = 0;
    }
    else {
	negScaleStep = (series_y2-xAxisPos) / negScaleNum;
	negScaleInc   = -min_scale / negScaleNum;
	if( data_type_flag==DATA_INT || data_type_flag==DATA_LONG )
	    negScaleInc = ceil(negScaleInc);            // don't like integer type to have non-integral negScaleInc
    }

    if(posScaleNum+negScaleNum<1) {
	scaleStep = 0;
	scaleInc   = 0;
    }
    else {
	scaleStep = (short)((series_y2-series_y1)/(posScaleNum+negScaleNum));
	scaleInc = ((max_scale-min_scale)/(posScaleNum+negScaleNum));
	if( data_type_flag==DATA_INT || data_type_flag==DATA_LONG )
	    scaleInc = max(posScaleInc, negScaleInc);
    }

    double scaleValue;

    for (int i = -negScaleNum,ycheck=xAxisPos-i*scaleStep;
	 ycheck>graph_y1;
	 i++,ycheck-=scaleStep) {

	vga_back.bar(series_x1, ycheck, series_x1+COMMON_OFFSET*2, ycheck, axis_color);

	scaleValue = i*scaleInc;

	switch (data_type_flag) {
	case DATA_FLOAT:
	case DATA_DOUBLE:
	    font_ptr->right_put(series_x1-COMMON_OFFSET, ycheck-font_ptr->max_font_height/2, m.format(scaleValue,num_format));
	    break;
	case DATA_INT:
	case DATA_LONG:
	    font_ptr->right_put(series_x1-COMMON_OFFSET, ycheck-font_ptr->max_font_height/2, m.format(long(scaleValue),num_format));
	}
    }

    //------ Draw x-axis -------//
    vga_back.bar(series_x1, xAxisPos-COMMON_OFFSET, series_x2+1, xAxisPos, axis_color);

    // show x-axis scale

    // At max, how much data points shown for each series
    int data_trimester_shown=24;

    // The scales within how much years should be shown
    int scale_trimester_shown=24;

    if(info.graph_trimester_passed<data_trimester_shown)
	data_trimester_shown=info.graph_trimester_passed;

    // show x-axis scale
    double pesudo_end,pesudo_game_day_template[4];
    double printing_offset;
    int start_printing_year;
    pesudo_game_day_template[0]=
	(double)player_school.trimester_array[WINTER].start_month/12.0
	+(double)player_school.trimester_array[WINTER].start_day/365.25;
    pesudo_game_day_template[1]=(double)player_school.trimester_array[SUMMER].start_month/12.0
	+(double)player_school.trimester_array[SUMMER].start_day/365.25;
    pesudo_game_day_template[2]=(double)player_school.trimester_array[AUTUMN].start_month/12.0
	+(double)player_school.trimester_array[AUTUMN].start_day/365.25;
    double current_day=(double)info.game_month/12+(double)info.game_day/365.25;
    if(current_day>pesudo_game_day_template[2]) {
	pesudo_end=pesudo_game_day_template[2];
	start_printing_year=(int)((info.game_year)+pesudo_end+(double)this_trimester_indicator/3+1-(double)data_trimester_shown/3-1.0/12);
	printing_offset=2/3;
    }
    else if(current_day>pesudo_game_day_template[1]) {
	pesudo_end=pesudo_game_day_template[1];
	start_printing_year=(int)((info.game_year)+pesudo_end+(double)this_trimester_indicator/3+1-(double)data_trimester_shown/3-1.0/12);
	printing_offset=1/3;
    }
    else if(current_day>pesudo_game_day_template[0]) {
	pesudo_end=pesudo_game_day_template[0];
	start_printing_year=(int)((info.game_year)+pesudo_end+(double)this_trimester_indicator/3+1-(double)data_trimester_shown/3-1.0/12);
	printing_offset=0;
    }
    else {
	pesudo_end=pesudo_game_day_template[2];
	start_printing_year=(int)((info.game_year-1)+pesudo_end+(double)this_trimester_indicator/3+1-(double)data_trimester_shown/3-1.0/12);
	printing_offset=2/3;
    }

    printing_offset+=1.0;
    short maxLabelWidth = font_ptr->text_width(m.format((int)start_printing_year, 16))+COMMON_OFFSET;
    double scaleYearRatio =3;
    x_axis_step = ((float)series_x2-series_x1) / (scale_trimester_shown);

    short displayInc = 1;
    while (displayInc * x_axis_step < maxLabelWidth ) displayInc++;

    //	double actual_end=(double)(pesudo_game_day+this_trimester_indicator/3)+info.game_year;
    //	int show_end=(int)(actual_end);

    //	double actual_start = actual_end-(double)(info.graph_trimester_passed)/3.0;
    //	double show_year_offset=1.0-(actual_start-(int)actual_start);
    //	int show_start=(int)(actual_start)+1;

    //	show_end=show_start+(trimester_shown/3);

    //	short displayInc = 0;
    //	while (displayInc * x_axis_step * 12.0< maxLabelWidth) displayInc++;

    //------ Draw x-axis -------//
    for (int i = 0; i < scale_trimester_shown ; i += displayInc) {
	int this_x;
	if((this_x = (int)(series_x1+((double)(i/3+printing_offset)*x_axis_step*3)))<series_x2) {
	    vga_back.bar(
		this_x,
		xAxisPos-COMMON_OFFSET*2,
		this_x+1,
		xAxisPos, axis_color);
	    font_ptr->center_put(
		this_x-maxLabelWidth/2-COMMON_OFFSET,
		xAxisPos+COMMON_OFFSET,
		this_x+maxLabelWidth/2-COMMON_OFFSET,
		xAxisPos+COMMON_OFFSET+font_ptr->max_font_height,
		m.format((int)(start_printing_year+i/scaleYearRatio),16));
	}
    }

    //---- Draw this year indicators ----//
    if((this_trimester_indicator>=0)&&(this_trimester_indicator<HISTORY_TRIMESTER_COUNT))
	for(int j=series_y1;j<series_y2+COMMON_OFFSET;j+=8)
	    vga_back.bar(
		(int)(series_x1+(info.graph_trimester_passed-this_trimester_indicator-1)*x_axis_step),
		j,
		(int)(series_x1+(info.graph_trimester_passed-this_trimester_indicator-1)*x_axis_step),
		j+4,
		axis_color);
}

//------------ End of function GraphNonFinancialTrimester::draw_scale -------------//

//---------- Begin of function GraphNonFinancialTrimester::draw_series ------------//
//!
void GraphNonFinancialTrimester::draw_series() {
    switch (data_type_flag) {
    case DATA_FLOAT: draw_series_float(); break;
    case DATA_DOUBLE: draw_series_double(); break;
    case DATA_INT: draw_series_int(); break;
    case DATA_LONG: draw_series_long(); break;
    }
}

//------------ End of function GraphNonFinancialTrimester::draw_series ------------//

//---------- Begin of function GraphNonFinancialTrimester::draw_series_float ------------//
//!
void GraphNonFinancialTrimester::draw_series_float() {
    float *dataArray = (float *) data_array;
    short prevX, prevY, currX, currY;

    for (int i = 0; i < series_num; i++) {
	dataArray-=info.graph_trimester_passed;
	dataArray+=HISTORY_TRIMESTER_COUNT;
	prevX = series_x1;
	prevY = xAxisPos-(short)(*dataArray * scaleStep/scaleInc) ;

	dataArray++;
	vga_back.thick_line(prevX-1, prevY, prevX+1, prevY, vga_back.translate_color(series_color[i]));

	for (int j = 1; j < info.graph_trimester_passed; j++) {
	    currX = series_x1 + (short)(j * x_axis_step);
	    currY = xAxisPos-(short)(*dataArray * scaleStep/scaleInc) ;

	    vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i]));

	    prevX = currX;
	    prevY = currY;
	    dataArray++;
	}

	if (value_flag)
	    draw_value(dataArray-1, vga_back.translate_color(series_color[i]), prevX+COMMON_OFFSET-(int)(0.3f*x_axis_step), prevY-font_news.max_font_height/2, DATA_FLOAT);
    }
}

//------------ End of function GraphNonFinancialTrimester::draw_series_float ------------//

//---------- Begin of function GraphNonFinancialTrimester::draw_series_double ------------//
//!
void GraphNonFinancialTrimester::draw_series_double() {
    double *dataArray = (double *) data_array;
    short prevX, prevY, currX, currY;

    for (int i = 0; i < series_num; i++) {
	dataArray-=info.graph_trimester_passed;
	dataArray+=HISTORY_TRIMESTER_COUNT;
	prevX = series_x1;
	prevY = xAxisPos-(short)(*dataArray * (double)scaleStep/scaleInc);

	dataArray++;
	vga_back.thick_line(prevX-1, prevY, prevX+1, prevY, vga_back.translate_color(series_color[i]));

	for (int j = 1; j < info.graph_trimester_passed; j++) {
	    currX = series_x1 + (short)(j * x_axis_step);
	    currY = xAxisPos-(short)(*dataArray * (double)scaleStep/scaleInc);

	    vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i]));

	    prevX = currX;
	    prevY = currY;
	    dataArray++;
	}

	if (value_flag)
	    draw_value(dataArray-1, vga_back.translate_color(series_color[i]), prevX+COMMON_OFFSET-(int)(0.3f*x_axis_step), prevY-font_news.max_font_height/2, DATA_DOUBLE);
    }
}

//------------ End of function GraphNonFinancialTrimester::draw_series_double ------------//

//---------- Begin of function GraphNonFinancialTrimester::draw_series_int ------------//
//!
void GraphNonFinancialTrimester::draw_series_int() {
    int *dataArray = (int *) data_array;
    short prevX, prevY, currX, currY;

    for (int i = 0; i < series_num; i++) {
	dataArray-=info.graph_trimester_passed;
	dataArray+=HISTORY_TRIMESTER_COUNT;
	prevX = series_x1;
	prevY = xAxisPos-(short)(*dataArray * scaleStep/scaleInc);

	dataArray++;
	vga_back.thick_line(prevX-1, prevY, prevX+1, prevY, vga_back.translate_color(series_color[i]));

	for (int j = 1; j < info.graph_trimester_passed; j++) {
	    currX = series_x1 + (short)(j * x_axis_step);
	    currY = (short)(xAxisPos-(int)(*dataArray * scaleStep/scaleInc));

	    vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i]));

	    prevX = currX;
	    prevY = currY;
	    dataArray++;
	}

	if (value_flag)
	    draw_value(dataArray-1, vga_back.translate_color(series_color[i]), prevX+COMMON_OFFSET-(int)(0.3f*x_axis_step), prevY-font_news.max_font_height/2, DATA_INT);
    }
}

//------------ End of function GraphNonFinancialTrimester::draw_series_int ------------//

//---------- Begin of function GraphNonFinancialTrimester::draw_series_long ------------//
//!
void GraphNonFinancialTrimester::draw_series_long() {
    long *dataArray = (long *) data_array;
    short prevX, prevY, currX, currY;

    for (int i = 0; i < series_num; i++) {
	dataArray-=info.graph_trimester_passed;
	dataArray+=HISTORY_TRIMESTER_COUNT;
	prevX = series_x1;
	prevY = xAxisPos-(short)(*dataArray * scaleStep/scaleInc);

	dataArray++;
	vga_back.thick_line(prevX-1, prevY, prevX+1, prevY, vga_back.translate_color(series_color[i]));

	for (int j = 1; j < info.graph_trimester_passed; j++) {
	    currX = series_x1 + (short)(j * x_axis_step);
	    currY = xAxisPos-(short)(*dataArray * scaleStep/scaleInc);

	    vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i]));

	    prevX = currX;
	    prevY = currY;
	    dataArray++;
	}

	if (value_flag)
	    draw_value(dataArray-1, vga_back.translate_color(series_color[i]), prevX+COMMON_OFFSET-(int)(0.3f*x_axis_step), prevY-font_news.max_font_height/2, DATA_LONG);
    }
}

//------------ End of function GraphNonFinancialTrimester::draw_series_float ------------//

//---------- Begin of function GraphNonFinancialTrimester::draw_value -------------------//
//!
void GraphNonFinancialTrimester::draw_value(void *value, int color, short x, short y, int dataType) {
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

    short textHeight = font_news.max_font_height;
    short textWidth = font_news.text_width(valueString) + 1;

    /*
      short *bitmap = (short *) sys.common_data_buf;
      err_when( (2 + textWidth * textHeight) * sizeof(short) > COMMON_DATA_BUF_SIZE );
      *bitmap++ = textWidth;
      *bitmap++ = textHeight;

      short *shortPtr = bitmap;
      for (int i = 0; i < textWidth * textHeight; i++)
      *shortPtr++ = transparent_code_w;

      font_news.put_to_bufferW(bitmap, textWidth * sizeof(short), 0, 0, valueString);

      shortPtr = bitmap;
      for (i = 0; i < textWidth * textHeight; i++, shortPtr++)
      if (*shortPtr != transparent_code_w)
      *shortPtr = color;

      vga_back.put_bitmapW_trans(x, y, (short *) sys.common_data_buf);
    */
    font_chartsm.put(x+3,y,valueString);
}

//---------- End of function GraphNonFinancialTrimester::draw_value -------------------//
