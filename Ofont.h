//Filename    : OFONT.H
//Description : Header file of Object Font

#ifndef __OFONT_H
#define __OFONT_H

#ifndef __ALL_H
#include <ALL.H>
#endif

#ifndef __OVGABUF_H
#include <OVGABUF.H>
#endif

#ifndef __OTRANSL_H
#include <OTRANSL.H>
#endif

//------- Define constant ------------//

#define DEFAULT_LINE_SPACE  2                     // 2 pixels space between lines

//#define FIRST_NATION_COLOR_CODE_IN_TEXT  0x80
//#define LAST_NATION_COLOR_CODE_IN_TEXT   0x8F
#define NATION_COLOR_BAR_WIDTH        13
#define NATION_COLOR_BAR_HEIGHT       13

//------ Define struct HyperField ------//

#define MAX_HYPER_FIELD 30                        // Maximum no. of hyper-text fields in a single page

//!Structure used to represent a hypertext field. A maximum of
//!MAX_HYPER_FIELD are allowed per page. The display of HyperText is
//!handled by the TxtList class.
struct HyperField {
    short x1, y1, x2, y2;
    char* text_ptr;                                 // pointer to the hyper-field name in the text
    short text_len;                                 // length of the hyper-field name
};

//--------- Define class Font ----------//

struct FontInfo;

//!UI class for displaying multiple fonts within the application. A font
//!resource file is composed of three parts : one FontHeader, one FontInfo
//!for each character, Bitmap data.
class Font {
public:
    char    init_flag;

    char*   next_text_ptr;                        // these 3 vars are used for storing
    short   next_text_y;                          // the result parameters after calling
    short   line_count;                           // put_paragraph(). Refer to put_paragraph()

    short   font_height;                          // height of a character
    short   std_font_height;
    short   max_font_height;
    short   max_font_width;                       // width of the widest character in the font
    short   space_width;                          // width of the space character
    short   inter_char_space;                     // inter-character space

    unsigned short first_char;                    // the starting available character
    unsigned short last_char;                     // the ending available character

    FontInfo* font_info_array;
    char*    font_bitmap_buf;                     // pointer to the buffer of the font

public:
    static short      hyper_field_count;
    static HyperField hyper_field_array[MAX_HYPER_FIELD];

    //---------------------------------------//

public:
    Font(char* =NULL);
    ~Font();

    void init(char* fontName, int interCharSpace=1, int italicShift=0);
    void deinit();

    int  height()                    { return font_height; }
    int  width()                     { return space_width; }

    void use_std_height()   { font_height = std_font_height; }
    void use_max_height()   { font_height = max_font_height; }

    int  text_width(char*, int= -1, int=0);
    int  text_height(int=DEFAULT_LINE_SPACE);

    int  put(int,int,char*,char=0,int= -1);

    int  put(int x, int y, int value, char clearBack=0, int x2= -1)
	{ return put( x, y, m.format(value), clearBack, x2 ); }

    void put_char(int,int,unsigned short);
    void right_put(int,int,char*);

    int  d3_put(int,int,char*);
    void d3_put(int,int,int,int,char*);
    int  center_put(int,int,int,int,char*,char clearBack=0);

    void put_paragraph(int,int,int,int,char*,int=DEFAULT_LINE_SPACE,int=1,char=1);

    char* Font::get_line(int area_width, int line, char *textPtr);
    int  find_splitter(int,int,char*);
    void count_line(int x1, int y1, int x2, int y2, char *text,
		    int lineSpace, int& totalLines, int& dispLines);
    int count_line(int area_width, char *textPtr);
    void put_char_to_buffer(char* dest, int destPitch, int x1, int y1, unsigned short text);
    void put_to_buffer(char* dest, int destPitch, int x1, int y1, char *text);
    void center_put_to_buffer(char* dest, int destPitch, int x1, int y1, int x2, int y2, char *text);

    void put_char_to_bufferW(short* dest, int destPitch, int x1, int y1, unsigned short text);
    void put_to_bufferW(short* dest, int destPitch, int x1, int y1, char *text);
    void center_put_to_bufferW(short* dest, int destPitch, int x1, int y1, int x2, int y2, char *text);

    short translate_german_char(short textChar);

    //----------- <int> version -------------//

    void put_field(int,int,char*,int,int,int=1);
    void update_field(int,int,int,int,int);
    void field(int,int,char*,int,int,int,int,int,char* helpCode=NULL);
    int  disp(int,int,int,int,int);

    //----- <short> version, call <int> version directly -----//

    void put_field(int a,int b,char* c,int d,short e,int f=1)
	{ put_field(a,b,c,d,(int)e,f); }

    void update_field(int a,int b,short c,int d,int e)
	{ update_field(a,b,(int)c,d,e); }

    void field(int a,int b,char* c,int d,short e,int f,int g,int h,char* helpCode=NULL)
	{ field(a,b,c,d,(int)e,f,g,h); }

    int  disp(int a,int b,short c,int d,int e)
	{ return disp(a,b,(int)c,d,e); }

    //---- <long> version, call <int> version directly ------//

    void put_field(int a,int b,char* c,int d,long e,int f=1)
	{ put_field(a,b,c,d,(int)e,f); }

    void update_field(int a,int b,long c,int d,int e)
	{ update_field(a,b,(int)c,d,e); }

    void field(int a,int b,char* c,int d,long e,int f,int g,int h,char* helpCode=NULL)
	{ field(a,b,c,d,(int)e,f,g,h); }

    int  disp(int a,int b,long c,int d,int e)
	{ return disp(a,b,(int)c,d,e); }

    //------------- <double> version ---------------//

    void put_field(int,int,char*,int,double,int=1);
    void update_field(int,int,double,int,int);
    void field(int,int,char*,int,double,int,int,int,char* helpCode=NULL);
    int  disp(int,int,double,int,int);

    //-------------- <char*> version ---------------//

    void put_field(int,int,char*,int,char*);
    void update_field(int,int,char*,int);
    void field(int,int,char*,int,char*,int,int,char* helpCode=NULL);
    int  disp(int,int,char*,int);
};

extern Font font_san, font_std, font_small, font_mid, font_news, font_casa;
extern Font font_red_san;
extern Font font_green_san;
extern Font font_blue_san;
extern Font font_small_san;
extern Font font_fblack;
extern Font font_fdiamond;
extern Font font_stone;
extern Font font_charts;
extern Font font_chartsm;

extern Font font_chart_blue_sm;
extern Font font_chart_orange_sm;
extern Font font_chart_red_sm;
extern Font font_chart_green_sm;
extern Font font_chart_purple_sm;

extern Font font_numbers;
extern Font font_redcharts;
extern Font font_barcharts;
extern Font font_hstone;
extern Font font_yellow_diamond;
extern Font font_letter;
extern Font font_fscore;
extern Font font_pgold;
extern Font font_psilver;
extern Font font_pbronze;

//------------------------------------------------//
#endif
