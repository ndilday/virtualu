#include <ALL.H>
#include <OSYS.H>
#include  <OFONT.H>
#include  <OSTR.H>
#include <OTEXT.H>

//-------- Begin of function Text::Text ----//
//!
Text::Text() {
    text_refresh_bitmap = NULL;
    init_flag = 0;
    pointer_flag=NIL;
    String disp_string;
}

//---------- End of function Text::Text ----//

//-------- Begin of function Text::~Text ---//
//!
Text::~Text() {
    deinit();
}

//---------- End of function Text::~Text ---//

//-------- Begin of function Text::deinit ------------//
//!
void Text::deinit() {

    if (text_refresh_bitmap) {
	mem_del(text_refresh_bitmap);
	text_refresh_bitmap = NULL;
    }
    //	if (disp_string)
    //	{
    //		mem_del(disp_string.str_buf);
    //	}
    init_flag = 0;
}

//------------- End of function Text::deinit ---------//

void Text::init(int x,int y,char* str) {
    text_x1=x;
    text_y1=y;

    pointer_flag=CHAR_TYPE;
    char_data_ptr=str;

    for(int i=0;str[i]!=0;i++);
    //	disp_string=mem_add(sizeof(char)*i);
    //	strcpy(disp_string.str_buf,str);
    disp_string=str;
    font_type=&font_chartsm;
}

void Text::init(int x,int y,int* valptr,int fmt) {
    text_x1=x;
    text_y1=y;

    pointer_flag=INT_TYPE;
    int_data_ptr=valptr;
    data_fmt=fmt;

    disp_string=m.format(*valptr,fmt);

    if(*valptr>=0)
	font_type=&font_chartsm;
    else
	font_type=&font_chart_red_sm;
}

void Text::init(int x,int y,long val,int fmt) {
    text_x1=x;
    text_y1=y;

    disp_string=m.format(val,fmt);

    if(val>=0)
	font_type=&font_chartsm;
    else
	font_type=&font_chart_red_sm;
}

void Text::init(int x,int y,long* valptr,int fmt) {
    text_x1=x;
    text_y1=y;

    pointer_flag=LONG_TYPE;
    long_data_ptr=valptr;
    data_fmt=fmt;
    disp_string=m.format(*valptr,fmt);

    if(*valptr>=0)
	font_type=&font_chartsm;
    else
	font_type=&font_chart_red_sm;
}

void Text::init(int x,int y,int val,int fmt) {
    text_x1=x;
    text_y1=y;

    disp_string=m.format(val,fmt);

    if(val>=0)
	font_type=&font_chartsm;
    else
	font_type=&font_chart_red_sm;
}

void Text::init(int x,int y,float* valptr,int fmt) {
    text_x1=x;
    text_y1=y;

    pointer_flag=FLOAT_TYPE;
    float_data_ptr=valptr;
    data_fmt=fmt;

    disp_string=m.format(*valptr,fmt);

    if(*valptr>=0)
	font_type=&font_chartsm;
    else
	font_type=&font_chart_red_sm;
}

void Text::init(int x,int y,float val,int fmt) {
    text_x1=x;
    text_y1=y;

    disp_string=m.format(val,fmt);

    if(val>=0)
	font_type=&font_chartsm;
    else
	font_type=&font_chart_red_sm;
}

void Text::init(int x,int y,double* valptr,int fmt) {
    text_x1=x;
    text_y1=y;

    pointer_flag=DOUBLE_TYPE;
    double_data_ptr=valptr;
    data_fmt=fmt;

    disp_string=m.format(*valptr,fmt);
    if(*valptr>=0)
	font_type=&font_chartsm;
    else
	font_type=&font_chart_red_sm;
}

void Text::init(int x,int y,double val,int fmt) {
    text_x1=x;
    text_x1=y;

    disp_string=m.format(val,fmt);

    if(val>=0)
	font_type=&font_chartsm;
    else
	font_type=&font_chart_red_sm;
}

void Text::paint() {
    if(init_flag != 1) {
	if(text_refresh_bitmap)deinit();
	int text_space=50;
	if (font_type->text_width(disp_string)>text_space)
	    text_space=font_type->text_width(disp_string);
	text_refresh_bitmap = vga.active_buf->save_area(
	    text_x1-((right_flag==1)?text_space:0),
	    text_y1,
	    text_x1+((right_flag==1)?0:text_space),
	    text_y1+font_type->height(),
	    text_refresh_bitmap);
	init_flag = 1;
    }
    refresh();
}

void Text::refresh() {
    if(init_flag == 0)return;

    // update disp_string

    if(pointer_flag==CHAR_TYPE)
	disp_string=char_data_ptr;

    if(pointer_flag==INT_TYPE) {
	disp_string=m.format(*int_data_ptr,data_fmt);
	if(*int_data_ptr>=0)
	    font_type=&font_chartsm;
	else
	    font_type=&font_chart_red_sm;
    }

    if(pointer_flag==LONG_TYPE) {
	disp_string=m.format(*long_data_ptr,data_fmt);
	if(*long_data_ptr>=0)
	    font_type=&font_chartsm;
	else
	    font_type=&font_chart_red_sm;
    }

    if(pointer_flag==FLOAT_TYPE) {
	if((*float_data_ptr<0.0)&&(*float_data_ptr>-0.01))
	    *float_data_ptr=0.0;
	disp_string=m.format(*float_data_ptr,data_fmt);
	if(*float_data_ptr>=0)
	    font_type=&font_chartsm;
	else
	    font_type=&font_chart_red_sm;
    }

    if(pointer_flag==DOUBLE_TYPE) {
	if((*double_data_ptr<0.0)&&(*double_data_ptr>-0.01))
	    *double_data_ptr=0.0;
	disp_string=m.format(*double_data_ptr,data_fmt);
	if(*double_data_ptr>=0)
	    font_type=&font_chartsm;
	else
	    font_type=&font_chart_red_sm;
    }

    vga.active_buf->rest_area(text_refresh_bitmap,0,0);

    if(right_flag==1)
	font_type->right_put(text_x1,text_y1,disp_string);
    else
	font_type->put(text_x1,text_y1,disp_string);
    /*	int text_space=45;
	if ((font_type->max_font_width)*disp_string.len()>30)
	text_space=(font_type->max_font_width)*disp_string.len();
	vga.blt_buf(
	text_x1,
	text_y1,
	text_x1+text_space,
	text_y1+font_type->height());
    */
}
