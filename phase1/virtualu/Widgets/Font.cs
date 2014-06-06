using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Widgets
{
    /// <summary>
    /// Structure used to represent a hypertext field. A maximum of MAX_HYPER_FIELD are allowed per page. 
    /// The display of HyperText is handled by the TxtList class.
    /// </summary>
    struct HyperField
    {
        short x1, y1, x2, y2;
        char* text_ptr;                                 // pointer to the hyper-field name in the text
        short text_len;                                 // length of the hyper-field name
    };

    struct FontInfo
    {
    }

    class Font
    {
        public bool    init_flag;

        public string   next_text_ptr;                        // these 3 vars are used for storing
        public short   next_text_y;                          // the result parameters after calling
        public short   line_count;                           // put_paragraph(). Refer to put_paragraph()

        public short   font_height;                          // height of a character
        public short   std_font_height;
        public short   max_font_height;
        public short   max_font_width;                       // width of the widest character in the font
        public short   space_width;                          // width of the space character
        public short   inter_char_space;                     // inter-character space

        public ushort first_char;                    // the starting available character
        public ushort last_char;                     // the ending available character

        public FontInfo[] font_info_array;
        public string    font_bitmap_buf;                     // pointer to the buffer of the font

        public static short      hyper_field_count;
        public static HyperField hyper_field_array[MAX_HYPER_FIELD];

        public Font(string = null);
        public ~Font();

        public void init(char* fontName, int interCharSpace=1, int italicShift=0);
        public void deinit();

        public int  height()                    { return font_height; }
        public int  width()                     { return space_width; }

        public void use_std_height()   { font_height = std_font_height; }
        public void use_max_height()   { font_height = max_font_height; }

        public int  text_width(string, int= -1, int=0);
        public int  text_height(int=DEFAULT_LINE_SPACE);

        public int  put(int,int,string,char=0,int= -1);

        public int  put(int x, int y, int value, char clearBack=0, int x2= -1)
	    { return put( x, y, m.format(value), clearBack, x2 ); }

        public void put_char(int,int,ushort);
        public void right_put(int,int,string);

        public int  d3_put(int,int,string);
        public void d3_put(int,int,int,int,string);
        public int  center_put(int,int,int,int,string,char clearBack=0);

        public void put_paragraph(int,int,int,int,string,int=DEFAULT_LINE_SPACE,int=1,char=1);

        public string get_line(int area_width, int line, char *textPtr);
        public int  find_splitter(int,int,string);
        public void count_line(int x1, int y1, int x2, int y2, string text, int lineSpace, int& totalLines, int& dispLines);
        public int count_line(int area_width, string textPtr);
        public void put_char_to_buffer(string dest, int destPitch, int x1, int y1, ushort text);
        public void put_to_buffer(string dest, int destPitch, int x1, int y1, string text);
        public void center_put_to_buffer(string dest, int destPitch, int x1, int y1, int x2, int y2, char *text);

        public void put_char_to_bufferW(short* dest, int destPitch, int x1, int y1, unsigned short text);
        public void put_to_bufferW(short* dest, int destPitch, int x1, int y1, char *text);
        public void center_put_to_bufferW(short* dest, int destPitch, int x1, int y1, int x2, int y2, char *text);

        public short translate_german_char(short textChar);

        //----------- <int> version -------------//

        public void put_field(int,int,char*,int,int,int=1);
        public void update_field(int,int,int,int,int);
        public void field(int,int,char*,int,int,int,int,int,char* helpCode=NULL);
        public int  disp(int,int,int,int,int);

        //----- <short> version, call <int> version directly -----//

        public void put_field(int a,int b,char* c,int d,short e,int f=1)
	    { put_field(a,b,c,d,(int)e,f); }

        public void update_field(int a,int b,short c,int d,int e)
	    { update_field(a,b,(int)c,d,e); }

        public void field(int a,int b,char* c,int d,short e,int f,int g,int h,char* helpCode=NULL)
	    { field(a,b,c,d,(int)e,f,g,h); }

        public int  disp(int a,int b,short c,int d,int e)
	    { return disp(a,b,(int)c,d,e); }

        //---- <long> version, call <int> version directly ------//

        public void put_field(int a,int b,char* c,int d,long e,int f=1)
	    { put_field(a,b,c,d,(int)e,f); }

        public void update_field(int a,int b,long c,int d,int e)
	    { update_field(a,b,(int)c,d,e); }

        public void field(int a,int b,char* c,int d,long e,int f,int g,int h,char* helpCode=NULL)
	    { field(a,b,c,d,(int)e,f,g,h); }

        public int  disp(int a,int b,long c,int d,int e)
	    { return disp(a,b,(int)c,d,e); }

        //------------- <double> version ---------------//

        public void put_field(int,int,char*,int,double,int=1);
        public void update_field(int,int,double,int,int);
        public void field(int,int,char*,int,double,int,int,int,char* helpCode=NULL);
        public int  disp(int,int,double,int,int);

        //-------------- <char*> version ---------------//

        public void put_field(int,int,char*,int,char*);
        public void update_field(int,int,char*,int);
        public void field(int,int,char*,int,char*,int,int,char* helpCode=NULL);
        public int  disp(int,int,char*,int);
    }
}
