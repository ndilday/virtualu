// owner Kevin
// OText -- Class of display text
///////////////////////////////////////

#include <ALL.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OSTR.H>
#include <OMISC.H>

//-------------- Define static vars ------------//

#define TEXT_HEIGHT font_san.height()

enum{NIL,CHAR_TYPE,INT_TYPE,LONG_TYPE,FLOAT_TYPE,DOUBLE_TYPE};

//!Class which handles the display of text on the screen.
class Text {

public:
    char    init_flag;
    char    paint_flag;
    char    right_flag;
    short * text_refresh_bitmap;
    int   text_x1,text_y1;
    char    pointer_flag;
    char *  char_data_ptr;
    int  *  int_data_ptr;
    long *  long_data_ptr;
    float * float_data_ptr;
    double *  double_data_ptr;
    char    data_fmt;
    String  disp_string;
    Font *  font_type;
public:

    Text();
    ~Text();

    void init(int x,int y,char* str);
    void init(int x,int y,int* valptr,int fmt=4);
    void init(int x,int y,int val,int fmt=4);
    void init(int x,int y,long* valptr,int fmt=4);
    void init(int x,int y,long val,int fmt=4);
    void init(int x,int y,float* valptr,int fmt=4);
    void init(int x,int y,float val,int fmt=4);
    void init(int x,int y,double* valptr,int fmt=4);
    void init(int x,int y,double val,int fmt=4);

    void put(int x, int y,int val,int fmt =4)
	{ right_flag=0;init(x,y,val,fmt);paint();}
    void put(int x, int y,float val,int fmt =4)
	{ right_flag=0;init(x,y,val,fmt);paint();}
    void put(int x, int y,double val,int fmt =4)
	{ right_flag=0;init(x,y,val,fmt);paint();}

    void put(int x, int y,int* valptr,int fmt =4)
	{ right_flag=0;init(x,y,valptr,fmt);paint();}
    void put(int x, int y,float* valptr,int fmt =4)
	{ right_flag=0;init(x,y,valptr,fmt);paint();}
    void put(int x, int y,double* valptr,int fmt =4)
	{ right_flag=0;init(x,y,valptr,fmt);paint();}

    void right_put(int x, int y,int val,int fmt =4)
	{ right_flag=1;init(x,y,val,fmt);paint();}
    void right_put(int x, int y,float val,int fmt =4)
	{ right_flag=1;init(x,y,val,fmt);paint();}
    void right_put(int x, int y,double val,int fmt =4)
	{ right_flag=1;init(x,y,val,fmt);paint();}

    void right_put(int x, int y,int* valptr,int fmt =4)
	{ right_flag=1;init(x,y,valptr,fmt);paint();}
    void right_put(int x, int y,float* valptr,int fmt =4)
	{ right_flag=1;init(x,y,valptr,fmt);paint();}
    void right_put(int x, int y,double* valptr,int fmt =4)
	{ right_flag=1;init(x,y,valptr,fmt);paint();}

    void put(int x, int y,char* str)
	{ init(x,y,str);paint();}

    void paint();
    void refresh(void);
    void deinit();
};
