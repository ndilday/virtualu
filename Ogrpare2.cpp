//Filename    : OGrpArea2.CPP
//Description : Object GraphArea2
//Owner			: Kevin(Ho) 0114

#include <ALL.H>
#include <COLOR.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OGRPARE2.H>
#include <OFINANCE.H>
#include <OIFACE.H>
#include <OMATH.H>

#include <math.h>

//--------- Define macro constant -----------//

#define MAX_LEN_LABEL "$10,000M"

static int default_series_color[] = {
    V_BLUE,
    V_RED,
    V_GREEN,
    V_ORANGE,
    V_VIOLET,
    V_BROWN,
    V_YELLOW,
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

//-------- Begin of function GraphArea2::GraphArea2 ----//
//!
GraphArea2::GraphArea2() {
    init_flag = 0;
    grapharea2_bitmap = NULL;
}

//---------- End of function GraphArea2::GraphArea2 ----//

//-------- Begin of function GraphArea2::~GraphArea2 -----//
//!
GraphArea2::~GraphArea2() {
    deinit();
}

//---------- End of function GraphArea2::~GraphArea2 -----//

//-------- Begin of function GraphArea2::init --------//
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
//! [int]		thisYearIndicator	- position of the vertical line which indicates this year.
//!										- -1 : no line
//! [int]    maxUpperLimit
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
void GraphArea2::init(int x1, int y1, int x2, int y2, int seriesNum,
		      int *dataNum,void *dataArray, char dataTypeFlag,char thisYearIndicator,int maxUpperLimit,
		      char *xLabel, char *yLabel,char **legendArray, char transparentFlag,  char orientationFlag,
		      char valueFlag,int numFormat, int *seriesColor, int axisColor) {
    err_when(seriesNum > DEFAULT_SERIES_COLOR_NUM && !seriesColor);
    err_when(seriesNum <= 0);

    //	err_when(*xEnd < *xStart);

    graph_width = x2 - x1 - 6;
    err_when(graph_width < 0);

    max_upper_limit=maxUpperLimit;
    // initialization	chwg0115
    grapharea2_x1 = x1;
    grapharea2_y1 = y1;
    grapharea2_x2 = x2;
    grapharea2_y2 = y2;

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

    y_label_max_len = font_chartsm.text_width(MAX_LEN_LABEL);
    set_font(&font_chartsm);

    //	paint();
}

//------------- End of function GraphArea2::init -----------//

//-------- Begin of function GraphArea2::deinit ------------//
//!
void GraphArea2::deinit() {
    if (grapharea2_bitmap) {
	mem_del(grapharea2_bitmap);
	grapharea2_bitmap = NULL;
    }

    init_flag = 0;
}

//------------- End of function GraphArea2::deinit ---------//

//-------- Begin of function GraphArea2::set_font ------------//
//!
void GraphArea2::set_font(Font *fontPtr) {
    if (!init_flag)
	return;

    font_ptr = fontPtr;

    calc_pos();
}

//------------- End of function GraphArea2::set_font ---------//

//-------- Begin of function GraphArea2::calc_pos ------------//
//!
//! calculate screen positions
//! should be called after changing display settings
//!
void GraphArea2::calc_pos() {
    if (legend_array) {
	short textWidth = 0, textHeight = font_ptr->max_font_height;

	for (int i = 0; i < 2+series_num; i++) {
	    if (font_ptr->text_width(legend_array[i]) > textWidth)
		textWidth = font_ptr->text_width(legend_array[i]);
	}

	err_when(textWidth + LEGEND_X_SPACING * 2 > graph_width);

	legend_x_num = (graph_width - LEGEND_X_SPACING*2) / (textWidth + LEGEND_X_SPACING*2);
	legend_y_num = ((2+series_num) / legend_x_num) + (((2+series_num) % legend_x_num) ? 1 : 0);

	graph_height = grapharea2_y2 - grapharea2_y1 - legend_y_num * (textHeight + LEGEND_Y_SPACING) - 6;

	err_when(graph_height <= 0);

	legend_width = (graph_width - LEGEND_X_SPACING*2) / legend_x_num;
	legend_height = textHeight + LEGEND_Y_SPACING;
    }
    else {
	legend_width = legend_height = legend_x_num = legend_y_num = 0;
	graph_height = grapharea2_y2 - grapharea2_y1 - 6;
    }

    short YLabelWidth = y_label ? (orientation_flag ? font_ptr->max_font_height : font_ptr->text_width(y_label)) : -COMMON_OFFSET;
    short XLabelHeight = x_label ? (font_ptr->max_font_height) : -COMMON_OFFSET;
    series_x1 = grapharea2_x1 + 4 + COMMON_OFFSET*4 + YLabelWidth + y_label_max_len;
    series_y1 = grapharea2_y1 + 4 + COMMON_OFFSET*6;
    series_x2 = grapharea2_x2 - 4 - COMMON_OFFSET*16;
    series_y2 = grapharea2_y1 + graph_height - COMMON_OFFSET*3 - (font_ptr->max_font_height) - XLabelHeight;
}

//------------ End of function GraphArea2::calc_pos ---------//

//-------- Begin of function GraphArea2::paint --------------//
//!
void GraphArea2::paint() {
    if (transparent_flag) {
	user_interface.brighten(grapharea2_x1+4, grapharea2_y1+4, grapharea2_x2-2, grapharea2_y2-2);

	//		vga.use_back();
	if (legend_array)
	    draw_legend();
	draw_axis_label();
	//		vga.use_front();
	grapharea2_bitmap = vga_back.save_area(grapharea2_x1+4, grapharea2_y1+4, grapharea2_x2-2, grapharea2_y2-2, grapharea2_bitmap);
    }

    //	paint_flag = 1;
    refresh();
}

//------------- End of function GraphArea2::paint -----------//

//-------- Begin of function GraphArea2::refresh ------------//
//!
void GraphArea2::refresh() {
    //	vga.use_back();

    if (transparent_flag) {
	user_interface.rect(grapharea2_x1, grapharea2_y1, grapharea2_x2, grapharea2_y2, 1);
	vga_back.rest_area(grapharea2_bitmap, 0, 0);
    }
    else {
	user_interface.bar(grapharea2_x1, grapharea2_y1, grapharea2_x2, grapharea2_y2);
	user_interface.panel(grapharea2_x1+3, grapharea2_y1+3, grapharea2_x2-4, grapharea2_y2-4);
    }
    if (legend_array)
	draw_legend();

    draw_scale();
    draw_series();
    draw_scale();

    //	vga.use_front();
    //	if (!paint_flag)
    //		vga.blt_buf(grapharea2_x1, grapharea2_y1, grapharea2_x2+2, grapharea2_y2+2);
    //	else
    //		paint_flag = 0;
}

//------------- End of function GraphArea2::refresh ---------//

//---------- Begin of function GraphArea2::draw_legend ------------//
//!
void GraphArea2::draw_legend() {
    for (int i = 0,c = 0; i < series_num+2; i++) {
	short legendX = grapharea2_x1 + 4 + (c % legend_x_num) * legend_width + LEGEND_X_SPACING;
	short legendY = grapharea2_y1 + 4 + graph_height + (c / legend_x_num) * legend_height;

	if(*(legend_array[i])=='\0')
	    continue;

	c++;
	if(i==2)
	    user_interface.bar( legendX+LEGEND_X_SPACING, legendY+LEGEND_Y_SPACING,
				legendX+LEGEND_X_SPACING+LEGEND_SIZE, legendY+LEGEND_Y_SPACING+LEGEND_SIZE,
				series_color[i] );
	else
	    user_interface.bar( legendX+LEGEND_X_SPACING, legendY+LEGEND_Y_SPACING+LEGEND_SIZE/2-1,
				legendX+LEGEND_X_SPACING+LEGEND_SIZE, legendY+LEGEND_Y_SPACING+LEGEND_SIZE/2+1,
				series_color[i] );
	font_ptr->put( legendX+LEGEND_X_SPACING*2+LEGEND_SIZE, legendY+LEGEND_Y_SPACING, legend_array[i] );
    }
}

//------------ End of function GraphArea2::draw_legend ------------//

//---------- Begin of function GraphArea2::draw_axis_label --------//
//!
void GraphArea2::draw_axis_label() {
    short textHeight = font_ptr->max_font_height;

    if (x_label) {
	font_ptr->center_put( series_x1, series_y2+COMMON_OFFSET*2+textHeight,
			      series_x2, grapharea2_y1+graph_height-COMMON_OFFSET, x_label );
    }

    if (y_label) {
	short textWidth = font_ptr->text_width(y_label) + 1;

	if (orientation_flag) {
	    short *fontBuf = (short *) sys.common_data_buf;
	    err_when(textWidth * textHeight * sizeof(short) > COMMON_DATA_BUF_SIZE);

	    for (int i = 0; i < textWidth * textHeight; i++)
		*fontBuf++ = transparent_code_w;

	    font_ptr->put_to_bufferW(fontBuf = (short *) sys.common_data_buf, textWidth * sizeof(short), 0, 0, y_label);

	    short xStart = grapharea2_x1 + 4 + COMMON_OFFSET * 2;
	    short yStart = (series_y1 + series_y2 - textWidth) / 2;

	    for (short x = 0; x < textHeight; x++) {
		for (short y = textWidth - 1; y >= 0; y--, fontBuf++) {
		    if (*fontBuf != transparent_code_w)
			*vga_back.buf_ptr(xStart+x, yStart+y) = *fontBuf;
		}
	    }
	}
	else {
	    font_ptr->center_put( grapharea2_x1+4+COMMON_OFFSET*2, series_y1,
				  grapharea2_x1+4+COMMON_OFFSET*2+textWidth, series_y2, y_label);
	}
    }
}

//------------ End of function GraphArea2::draw_axis_label --------//

//---------- Begin of function GraphArea2::find_scale -------------//
//!
void GraphArea2::find_scale() {
    double data;
    max_val = 0.0;
    min_val = 0.0;
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
	if (data > max_val)
	    max_val = fabs(data);
	if (data < min_val)
	    min_val = fabs(data);
	if (!x_axis_pos && data < 0.0)
	    x_axis_pos = 1;
    }

    if ( max_upper_limit>max_val )
	max_label = max_upper_limit;
    else
	max_label = max_val;

    if (min_val<0)
	min_label = -pow(10,floor(log10(fabs(min_val))));
    else
	min_label = 0;

    y_ratio=(series_y2-series_y1)/(max_label-min_label);
    // scr y-pos of x-axis
    x_axis_scr_pos=(int)(series_y1+max_label*y_ratio);
}

//------------ End of function GraphArea2::find_scale -------------//

//---------- Begin of function GraphArea2::draw_scale -------------//
//!
void GraphArea2::draw_scale() {
    find_scale();

    //	short xAxisPos;

    //------ Draw y-axis -------//

    //	if (x_axis_pos)
    //		vga_back.bar(series_x1, series_y1-COMMON_OFFSET*2, series_x1+1, series_y2+COMMON_OFFSET*2, axis_color);
    //	else

    vga_back.bar(series_x1, series_y1-COMMON_OFFSET*2, series_x1+1, series_y2, axis_color);

    //	short scaleNum = (series_y2-series_y1-font_ptr->max_font_height) / (font_ptr->max_font_height*3/2);
    //	scaleNum = short(pow(2,floor(log10(double(scaleNum))/log10(2.0))));
    //	short scaleStep = (series_y2-series_y1) / scaleNum;
    //	double scaleValue;

    // Draw +ve index
    short scaleNum =(int)((x_axis_scr_pos-series_y1)/ (font_ptr->max_font_height*3/2));
    if ( scaleNum > 0 )
	scaleNum = short(pow(2,floor(log10(double(scaleNum))/log10(2.0))));
    else
	scaleNum = 0;
    double indexStep = max_label/scaleNum;

    //------ Draw y-axis -------//
    for (int i = 0; i <= scaleNum; i++) {
	int yPos=(int)(x_axis_scr_pos-y_ratio*indexStep*i);
	vga_back.bar( series_x1-COMMON_OFFSET,yPos,
		      series_x1+COMMON_OFFSET,yPos+1, axis_color);

	//		if ( (x_axis_pos && i == scaleNum / 2) || !(x_axis_pos || i) )
	//			vga_back.bar(series_x1, xAxisPos=series_y2-i*scaleStep-1, series_x2+COMMON_OFFSET*8, series_y2-i*scaleStep, axis_color);
	//		else
	//			vga_back.bar(series_x1, series_y2-i*scaleStep, series_x1+COMMON_OFFSET*2, series_y2-i*scaleStep, axis_color);
	//		if (x_axis_pos)
	//			scaleValue = max_scale*2/scaleNum*i - max_scale;
	//		else
	//			scaleValue = max_scale/scaleNum*i;

	switch (data_type_flag) {
	case DATA_FLOAT:
	case DATA_DOUBLE:
	    font_ptr->right_put(series_x1-COMMON_OFFSET, yPos-font_ptr->max_font_height/2, m.format(indexStep*i,num_format));
	    break;
	case DATA_INT:
	case DATA_LONG:
	    font_ptr->right_put(series_x1-COMMON_OFFSET, yPos-font_ptr->max_font_height/2, m.format(long(indexStep*i),num_format));
	}
    }

    // Draw -ve index
    scaleNum =(int)((series_y2-x_axis_scr_pos)/ (font_ptr->max_font_height*3/2));
    if ( scaleNum > 0 )
	scaleNum = short(pow(2,floor(log10(double(scaleNum))/log10(2.0))));
    else
	scaleNum = 0;
    indexStep = fabs(math.safe_divide(min_label,(double)scaleNum));

    //------ Draw y-axis -------//
    for (int i = 1; i <= scaleNum; i++) {
	int yPos=(int)(x_axis_scr_pos+y_ratio*indexStep*i);
	vga_back.bar( series_x1-COMMON_OFFSET,yPos,
		      series_x1+COMMON_OFFSET,yPos+1, axis_color);

	//		if ( (x_axis_pos && i == scaleNum / 2) || !(x_axis_pos || i) )
	//			vga_back.bar(series_x1, xAxisPos=series_y2-i*scaleStep-1, series_x2+COMMON_OFFSET*8, series_y2-i*scaleStep, axis_color);
	//		else
	//			vga_back.bar(series_x1, series_y2-i*scaleStep, series_x1+COMMON_OFFSET*2, series_y2-i*scaleStep, axis_color);
	//		if (x_axis_pos)
	//			scaleValue = max_scale*2/scaleNum*i - max_scale;
	//		else
	//			scaleValue = max_scale/scaleNum*i;

	switch (data_type_flag) {
	case DATA_FLOAT:
	case DATA_DOUBLE:
	    font_ptr->right_put(series_x1-COMMON_OFFSET, yPos-font_ptr->max_font_height/2, m.format(-indexStep*i,num_format));
	    break;
	case DATA_INT:
	case DATA_LONG:
	    font_ptr->right_put(series_x1-COMMON_OFFSET, yPos-font_ptr->max_font_height/2, m.format(-long(indexStep*i),num_format));
	}
    }

    // show x-axis scale

    // At max, how much data points shown for each series
    int data_year_shown=8;

    // The scales within how much years should be shown
    int scale_year_shown=8;

    // pesudo year = which year the indicator pointing to
    double pesudo_end;
    // ##### patch begin Gilbert 07/09/2001 ######//
    pesudo_end = info.financial_year();
    //	if(info.game_month<finance.fiscal_year_start_month)
    //		pesudo_end=info.game_year-1;
    //	else
    //		pesudo_end=info.game_year;
    // ##### patch end Gilbert 07/09/2001 ######//

    if(info.graph_year_passed<data_year_shown)
	data_year_shown=info.graph_year_passed;

    short maxLabelWidth = font_ptr->text_width(m.format(pesudo_end, 16));
    double scaleYearRatio =1.0;
    x_axis_step = (series_x2-series_x1) / (scale_year_shown);

    short displayInc = 1;
    while (displayInc * x_axis_step < maxLabelWidth ) displayInc++;

    double printing_offset=0.0;

    int start_printing_year=(int)(pesudo_end+
				  ((this_year_indicator>=0)?this_year_indicator:0)
				  +1-data_year_shown);

    //	double actual_end_year=(double)info.game_month/12.0+info.game_year+1;
    //	int show_end_year=(int)(actual_end_year-finance.fiscal_year_start_month/12.0);
    //	int show_start_year=show_end_year-year_shown+1;

    //------ Draw x-axis -------//
    vga_back.bar(series_x1,x_axis_scr_pos-1,series_x2,x_axis_scr_pos,axis_color);
    for (int i = 0; i < scale_year_shown ; i += displayInc) {
	vga_back.bar(
	    (int)(series_x1+(printing_offset+i)*x_axis_step),
	    x_axis_scr_pos-COMMON_OFFSET*2,
	    (int)(series_x1+(printing_offset+i)*x_axis_step),
	    x_axis_scr_pos+COMMON_OFFSET*2, axis_color);
	font_ptr->center_put(
	    (int)(series_x1+(printing_offset+i)*x_axis_step)-maxLabelWidth/2-COMMON_OFFSET,
	    x_axis_scr_pos+COMMON_OFFSET,
	    (int)(series_x1+(printing_offset+i)*x_axis_step)+maxLabelWidth/2+COMMON_OFFSET,
	    x_axis_scr_pos+COMMON_OFFSET+font_ptr->max_font_height,
	    m.format((int)(start_printing_year+i/scaleYearRatio), 16));
    }

    //---- Draw this year indicators ----//
    if((this_year_indicator>=0)&&(this_year_indicator<HISTORY_YEAR_COUNT))
	for(int j=series_y1;j<series_y2+COMMON_OFFSET;j+=8)
	    vga_back.bar(
		(int)(series_x1+(data_year_shown-this_year_indicator-1)*x_axis_step),
		j,
		(int)(series_x1+(data_year_shown-this_year_indicator-1)*x_axis_step),
		j+4,
		axis_color);
}

//------------ End of function GraphArea2::draw_scale -------------//

//---------- Begin of function GraphArea2::draw_series ------------//
//!
void GraphArea2::draw_series() {
    switch (data_type_flag) {
    case DATA_FLOAT: draw_series_float(); break;
    case DATA_DOUBLE: draw_series_double(); break;
    case DATA_INT: draw_series_int(); break;
    case DATA_LONG: draw_series_long(); break;
    }
}

//------------ End of function GraphArea2::draw_series ------------//

//---------- Begin of function GraphArea2::draw_series_float ------------//
//!
void GraphArea2::draw_series_float() {
    float *dataArray = (float *) data_array;
    short prevX, prevY, currX, currY;
    short prevY2, currY2;
    short fillY, fillY2;
    double m,m2;

    //Filling region
    prevX = series_x1;

    dataArray-=info.graph_year_passed;
    dataArray+=*data_num;

    // Find Y by dataArray
    //	if (x_axis_pos)
    //		prevY = (short)(series_y2 - ((*dataArray+float(max_scale)) * (series_y2-series_y1) / float(max_scale*2)));
    //	else
    //		prevY = (short)(series_y2 - *dataArray * (series_y2-series_y1) / float(max_scale));
    //	if (x_axis_pos)
    //		prevY2 = (short)(series_y2 - ((*(dataArray+(*data_num))+float(max_scale)) * (series_y2-series_y1) / float(max_scale*2)));
    //	else
    //		prevY2 = (short)(series_y2 - *(dataArray+(*data_num)) * (series_y2-series_y1) / float(max_scale));

    prevY = (short)(x_axis_scr_pos-*dataArray*y_ratio);
    prevY2 = (short)(x_axis_scr_pos-*(dataArray+(*data_num))*y_ratio);
    dataArray++;
    vga_back.thick_line(prevX, prevY, prevX+1, prevY, vga_back.translate_color(series_color[0]));
    vga_back.thick_line(prevX, prevY2, prevX+1, prevY2, vga_back.translate_color(series_color[1]));

    for (int j = 1; j < info.graph_year_passed; j++) {
	currX = series_x1 + j * x_axis_step;
	//		if (x_axis_pos)
	//			currY = (short)(series_y2 - ((*dataArray+float(max_scale)) * (series_y2-series_y1) / float(max_scale*2)));
	//		else
	//			currY = (short)(series_y2 - *dataArray * (series_y2-series_y1) / float(max_scale));
	//		if (x_axis_pos)
	//			currY2 = (short)(series_y2 - ((*(dataArray+(*data_num))+float(max_scale)) * (series_y2-series_y1) / float(max_scale*2)));
	//		else
	//			currY2 = (short)(series_y2 - *(dataArray+(*data_num)) * (series_y2-series_y1) / float(max_scale));

	currY = (short)(x_axis_scr_pos-*dataArray*y_ratio);
	currY2 = (short)(x_axis_scr_pos-*(dataArray+(*data_num))*y_ratio);

	// filling region
	m=((double)currY-prevY)/x_axis_step;
	m2=((double)currY2-prevY2)/x_axis_step;

	for(int i=0;i<x_axis_step;i++) {
	    fillY=(int)(m*i+prevY);
	    fillY2=(int)(m2*i+prevY2);
	    if(fillY>fillY2) {
		if(*(legend_array[3])!='\0')
		    vga_back.line(prevX+i, fillY, prevX+i, fillY2, vga_back.translate_color(series_color[3]));
	    }
	    else
		vga_back.line(prevX+i, fillY, prevX+i, fillY2, vga_back.translate_color(series_color[2]));
	}

	// draw lines
	vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[0]));
	vga_back.thick_line(prevX, prevY2, currX, currY2, vga_back.translate_color(series_color[1]));

	prevX = currX;
	prevY = currY;
	prevY2 = currY2;
	dataArray++;
    }

    if (value_flag) {
	draw_value(dataArray-1, vga_back.translate_color(series_color[0]), prevX+COMMON_OFFSET, prevY-font_news.max_font_height/2, DATA_FLOAT);
	draw_value(dataArray+(*data_num-1), vga_back.translate_color(series_color[1]), prevX+COMMON_OFFSET, prevY2, DATA_FLOAT);
    }

    //---------------Draw the remain lines if they exist-----------//
    if(series_num>2) {
	dataArray+=(*data_num);

	for (int i = 2; i < series_num; i++) {
	    dataArray-=(info.graph_year_passed);
	    dataArray+=(*data_num);
	    prevX = series_x1;
	    //			if (x_axis_pos)
	    //				prevY = series_y2 - short(((*dataArray+float(max_scale)) * float(series_y2-series_y1) / float(max_scale*2)));
	    //			else
	    //				prevY = series_y2 - short(*dataArray * float(series_y2-series_y1) / float(max_scale));
	    prevY = (short)(x_axis_scr_pos-*dataArray*y_ratio);

	    dataArray++;

	    for (int j = 1; j < (info.graph_year_passed); j++) {
		currX = series_x1 + j * x_axis_step;
		//				if (x_axis_pos)
		//					currY = series_y2 - short(((*dataArray+float(max_scale)) * float(series_y2-series_y1) / float(max_scale*2)));
		//				else
		//					currY = series_y2 - short(*dataArray * float(series_y2-series_y1) / float(max_scale));
		currY = (short)(x_axis_scr_pos-*dataArray*y_ratio);

		vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i+2]));

		prevX = currX;
		prevY = currY;
		dataArray++;
	    }
	    if (value_flag)
		draw_value(dataArray-1, vga_back.translate_color(series_color[i+2]), prevX+COMMON_OFFSET, prevY-font_news.max_font_height/2, DATA_FLOAT);
	}
    }
}

//------------ End of function GraphArea2::draw_series_float ------------//

//---------- Begin of function GraphArea2::draw_series_double ------------//
//!
void GraphArea2::draw_series_double() {
    double *dataArray = (double *) data_array;
    short prevX, prevY, currX, currY;
    short prevY2, currY2;
    short fillY, fillY2;
    double m,m2;

    //Filling region
    prevX = series_x1;
    dataArray-=(info.graph_year_passed);
    dataArray+=*data_num;

    // Find Y by dataArray
    //	if (x_axis_pos)
    //		prevY = (short)(series_y2 - ((*dataArray+double(max_scale)) * (series_y2-series_y1) / double(max_scale*2)));
    //	else
    //		prevY = (short)(series_y2 - *dataArray * (series_y2-series_y1) / double(max_scale));
    //	if (x_axis_pos)
    //		prevY2 = (short)(series_y2 - ((*(dataArray+(*data_num))+double(max_scale)) * (series_y2-series_y1) / double(max_scale*2)));
    //	else
    //		prevY2 = (short)(series_y2 - *(dataArray+(*data_num)) * (series_y2-series_y1) / double(max_scale));
    prevY = (short)(x_axis_scr_pos-*dataArray*y_ratio);
    prevY2 = (short)(x_axis_scr_pos-*(dataArray+(*data_num))*y_ratio);

    dataArray++;
    vga_back.thick_line(prevX, prevY, prevX+1, prevY, vga_back.translate_color(series_color[0]));
    vga_back.thick_line(prevX, prevY2, prevX+1, prevY2, vga_back.translate_color(series_color[1]));

    for (int j = 1; j < (info.graph_year_passed); j++) {
	currX = series_x1 + j * x_axis_step;
	//		if (x_axis_pos)
	//			currY = (short)(series_y2 - ((*dataArray+double(max_scale)) * (series_y2-series_y1) / double(max_scale*2)));
	//		else
	//			currY = (short)(series_y2 - *dataArray * (series_y2-series_y1) / double(max_scale));

	//		if (x_axis_pos)
	//			currY2 = (short)(series_y2 - ((*(dataArray+(*data_num))+double(max_scale)) * (series_y2-series_y1) / double(max_scale*2)));
	//		else
	//			currY2 = (short)(series_y2 - *(dataArray+(*data_num)) * (series_y2-series_y1) / double(max_scale));
	currY = (short)(x_axis_scr_pos-*dataArray*y_ratio);
	currY2 = (short)(x_axis_scr_pos-*(dataArray+(*data_num))*y_ratio);

	// filling region
	m=((double)currY-prevY)/x_axis_step;
	m2=((double)currY2-prevY2)/x_axis_step;

	for(int i=0;i<x_axis_step;i++) {
	    fillY=(int)(m*i+prevY);
	    fillY2=(int)(m2*i+prevY2);
	    if(fillY>fillY2) {
		if(*(legend_array[3])!='\0')
		    vga_back.line(prevX+i, fillY, prevX+i, fillY2, vga_back.translate_color(series_color[3]));
	    }
	    else
		vga_back.line(prevX+i, fillY, prevX+i, fillY2, vga_back.translate_color(series_color[2]));
	}

	// draw lines
	vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[0]));
	vga_back.thick_line(prevX, prevY2, currX, currY2, vga_back.translate_color(series_color[1]));

	prevX = currX;
	prevY = currY;
	prevY2 = currY2;
	dataArray++;
    }

    if (value_flag) {
	draw_value(dataArray-1, vga_back.translate_color(series_color[0]), prevX+COMMON_OFFSET, prevY-font_news.max_font_height/2, DATA_DOUBLE);
	draw_value(dataArray+(*data_num-1), vga_back.translate_color(series_color[1]), prevX+COMMON_OFFSET, prevY2, DATA_DOUBLE);
    }

    //---------------Draw the remain lines if they exist-----------//

    if(series_num>2) {
	dataArray+=(*data_num);

	for (int i = 2; i < series_num; i++) {
	    dataArray-=(info.graph_year_passed);
	    dataArray+=*data_num;
	    prevX = series_x1;
	    //			if (x_axis_pos)
	    //				prevY = series_y2 - short(((*dataArray+double(max_scale)) * double(series_y2-series_y1) / double(max_scale*2)));
	    //			else
	    //				prevY = series_y2 - short(*dataArray * double(series_y2-series_y1) / double(max_scale));
	    //			prevY	= (short)(x_axis_scr_pos-*dataArray*y_ratio);
	    prevY = (short)(x_axis_scr_pos-*dataArray*y_ratio);

	    dataArray++;

	    for (int j = 1; j < (info.graph_year_passed); j++) {
		currX = series_x1 + j * x_axis_step;
		//				if (x_axis_pos)
		//					currY = series_y2 - short(((*dataArray+double(max_scale)) * double(series_y2-series_y1) / double(max_scale*2)));
		//				else
		//					currY = series_y2 - short(*dataArray * double(series_y2-series_y1) / double(max_scale));
		//				currY	= (short)(x_axis_scr_pos-*dataArray*y_ratio);
		currY = (short)(x_axis_scr_pos-*dataArray*y_ratio);

		vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i+2]));

		prevX = currX;
		prevY = currY;
		dataArray++;
	    }
	    if (value_flag)
		draw_value(dataArray-1, vga_back.translate_color(series_color[i+2]), prevX+COMMON_OFFSET, prevY-font_news.max_font_height/2, DATA_DOUBLE);
	}
    }

}

//------------ End of function GraphArea2::draw_series_double ------------//

//---------- Begin of function GraphArea2::draw_series_int ------------//
//!
void GraphArea2::draw_series_int() {
    int *dataArray = (int *) data_array;
    short prevX, prevY, currX, currY;
    short prevY2, currY2;
    short fillY, fillY2;
    double m,m2;

    //Filling region
    prevX = series_x1;
    dataArray-=(info.graph_year_passed);
    dataArray+=*data_num;

    // Find Y by dataArray
    //	if (x_axis_pos)
    //		prevY = series_y2 - ((*dataArray+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
    //	else
    //		prevY = series_y2 - *dataArray * (series_y2-series_y1) / int(max_scale);
    //	if (x_axis_pos)
    //		prevY2 = series_y2 - ((*(dataArray+(*data_num))+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
    //	else
    //		prevY2 = series_y2 - *(dataArray+(*data_num)) * (series_y2-series_y1) / int(max_scale);

    prevY = (short)(x_axis_scr_pos-*dataArray*y_ratio);
    prevY2 = (short)(x_axis_scr_pos-*(dataArray+(*data_num))*y_ratio);

    dataArray++;

    vga_back.thick_line(prevX, prevY, prevX+1, prevY, vga_back.translate_color(series_color[0]));
    vga_back.thick_line(prevX, prevY2, prevX+1, prevY2, vga_back.translate_color(series_color[1]));

    for (int j = 1; j < (info.graph_year_passed); j++) {
	currX = series_x1 + j * x_axis_step;
	//		if (x_axis_pos)
	//			currY = series_y2 - ((*dataArray+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
	//		else
	//			currY = series_y2 - *dataArray * (series_y2-series_y1) / int(max_scale);

	//		if (x_axis_pos)
	//			currY2 = series_y2 - ((*(dataArray+(*data_num))+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
	//		else
	//			currY2 = series_y2 - *(dataArray+(*data_num)) * (series_y2-series_y1) / int(max_scale);

	currY = (short)(x_axis_scr_pos-*dataArray*y_ratio);
	currY2 = (short)(x_axis_scr_pos-*(dataArray+(*data_num))*y_ratio);

	// filling region
	m=((double)currY-prevY)/x_axis_step;
	m2=((double)currY2-prevY2)/x_axis_step;

	for(int i=0;i<x_axis_step;i++) {
	    fillY=(int)(m*i+prevY);
	    fillY2=(int)(m2*i+prevY2);
	    if(fillY>fillY2) {
		if(*(legend_array[3])!='\0')
		    vga_back.line(prevX+i, fillY, prevX+i, fillY2, vga_back.translate_color(series_color[3]));
	    }
	    else
		vga_back.line(prevX+i, fillY, prevX+i, fillY2, vga_back.translate_color(series_color[2]));
	}

	// draw lines
	vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[0]));
	vga_back.thick_line(prevX, prevY2, currX, currY2, vga_back.translate_color(series_color[1]));

	prevX = currX;
	prevY = currY;
	prevY2 = currY2;
	dataArray++;
    }

    if (value_flag) {
	draw_value(dataArray-1, vga_back.translate_color(series_color[0]), prevX+COMMON_OFFSET, prevY-font_ptr->max_font_height/2, DATA_INT);
	draw_value(dataArray+(*data_num)-1, vga_back.translate_color(series_color[1]), prevX+COMMON_OFFSET, prevY2-font_ptr->max_font_height/2, DATA_INT);
    }

    if(series_num>2) {
	dataArray+=(*data_num);
	for (int i = 2; i < series_num; i++) {
	    prevX = series_x1;
	    dataArray-=(info.graph_year_passed);
	    dataArray+=*data_num;
	    // Find Y by dataArray
	    //			if (x_axis_pos)
	    //				prevY = series_y2 - ((*dataArray+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
	    //			else
	    //				prevY = series_y2 - *dataArray * (series_y2-series_y1) / int(max_scale);
	    prevY = (short)(x_axis_scr_pos-*dataArray*y_ratio);

	    dataArray++;

	    for (int j = 1; j < (info.graph_year_passed); j++) {
		currX = series_x1 + j * x_axis_step;
		//				if (x_axis_pos)
		//					currY = series_y2 - ((*dataArray+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
		//				else
		//					currY = series_y2 - *dataArray * (series_y2-series_y1) / int(max_scale);
		currY = (short)(x_axis_scr_pos-*dataArray*y_ratio);

		vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i+2]));

		prevX = currX;
		prevY = currY;
		dataArray++;
	    }
	    if (value_flag)
		draw_value(dataArray-1, vga_back.translate_color(series_color[i+2]), prevX+COMMON_OFFSET, prevY-font_news.max_font_height/2, DATA_INT);
	}

    }
}

//------------ End of function GraphArea2::draw_series_int ------------//

//---------- Begin of function GraphArea2::draw_series_long ------------//
//!
void GraphArea2::draw_series_long() {
    long *dataArray = (long *) data_array;
    short prevX, prevY, currX, currY;
    short prevY2, currY2;
    short fillY, fillY2;
    double m,m2;

    //Filling region
    prevX = series_x1;
    dataArray-=(info.graph_year_passed);
    dataArray+=*data_num;

    // Find Y by dataArray
    //	if (x_axis_pos)
    //		prevY = series_y2 - ((*dataArray+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
    //	else
    //		prevY = series_y2 - *dataArray * (series_y2-series_y1) / int(max_scale);

    //	if (x_axis_pos)
    //		prevY2 = series_y2 - ((*(dataArray+(*data_num))+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
    //	else
    //		prevY2 = series_y2 - *(dataArray+(*data_num)) * (series_y2-series_y1) / int(max_scale);

    prevY = (short)(x_axis_scr_pos-*dataArray*y_ratio);
    prevY2 = (short)(x_axis_scr_pos-*(dataArray+(*data_num))*y_ratio);

    dataArray++;

    vga_back.thick_line(prevX, prevY, prevX+1, prevY, vga_back.translate_color(series_color[0]));
    vga_back.thick_line(prevX, prevY2, prevX+1, prevY2, vga_back.translate_color(series_color[1]));

    for (int j = 1; j < (info.graph_year_passed); j++) {
	currX = series_x1 + j * x_axis_step;
	//		if (x_axis_pos)
	//			currY = series_y2 - ((*dataArray+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
	//		else
	//			currY = series_y2 - *dataArray * (series_y2-series_y1) / int(max_scale);
	//		if (x_axis_pos)
	//			currY2 = series_y2 - ((*(dataArray+(*data_num))+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
	//		else
	//			currY2 = series_y2 - *(dataArray+(*data_num)) * (series_y2-series_y1) / int(max_scale);

	currY = (short)(x_axis_scr_pos-*dataArray*y_ratio);
	currY2 = (short)(x_axis_scr_pos-*(dataArray+(*data_num))*y_ratio);
	// filling region
	m=((double)currY-prevY)/x_axis_step;
	m2=((double)currY2-prevY2)/x_axis_step;

	for(int i=0;i<x_axis_step;i++) {
	    fillY=(int)(m*i+prevY);
	    fillY2=(int)(m2*i+prevY2);
	    if(fillY>fillY2) {
		if(*(legend_array[3])!='\0')
		    vga_back.line(prevX+i, fillY, prevX+i, fillY2, vga_back.translate_color(series_color[3]));
	    }
	    else
		vga_back.line(prevX+i, fillY, prevX+i, fillY2, vga_back.translate_color(series_color[2]));
	}

	// draw lines
	vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[0]));
	vga_back.thick_line(prevX, prevY2, currX, currY2, vga_back.translate_color(series_color[1]));

	prevX = currX;
	prevY = currY;
	prevY2 = currY2;
	dataArray++;
    }

    if (value_flag) {
	draw_value(dataArray-1, vga_back.translate_color(series_color[0]), prevX+COMMON_OFFSET, prevY-font_ptr->max_font_height/2, DATA_LONG);
	draw_value(dataArray+(*data_num)-1, vga_back.translate_color(series_color[1]), prevX+COMMON_OFFSET, prevY2-font_ptr->max_font_height/2, DATA_LONG);
    }

    if(series_num>2) {
	dataArray+=(*data_num);
	for (int i = 2; i < series_num; i++) {
	    prevX = series_x1;
	    dataArray-=(info.graph_year_passed);
	    dataArray+=*data_num;
	    // Find Y by dataArray
	    //			if (x_axis_pos)
	    //				prevY = series_y2 - ((*dataArray+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
	    //			else
	    //				prevY = series_y2 - *dataArray * (series_y2-series_y1) / int(max_scale);
	    prevY = (short)(x_axis_scr_pos-*dataArray*y_ratio);

	    dataArray++;

	    for (int j = 1; j < (info.graph_year_passed); j++) {
		currX = series_x1 + j * x_axis_step;
		//				if (x_axis_pos)
		//					currY = series_y2 - ((*dataArray+int(max_scale)) * (series_y2-series_y1) / int(max_scale*2));
		//				else
		//					currY = series_y2 - *dataArray * (series_y2-series_y1) / int(max_scale);
		currY = (short)(x_axis_scr_pos-*dataArray*y_ratio);

		vga_back.thick_line(prevX, prevY, currX, currY, vga_back.translate_color(series_color[i+2]));

		prevX = currX;
		prevY = currY;
		dataArray++;
	    }
	    if (value_flag)
		draw_value(dataArray-1, vga_back.translate_color(series_color[i+2]), prevX+COMMON_OFFSET, prevY-font_news.max_font_height/2, DATA_LONG);
	}

    }
}

//------------ End of function GraphArea2::draw_series_float ------------//

//---------- Begin of function GraphArea2::draw_value -------------------//
//!
void GraphArea2::draw_value(void *value, int color, short x, short y, int dataType) {
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

//---------- Begin of function GraphArea2::draw_value -------------------//
