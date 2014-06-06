using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Widgets
{
    class Button
    {
        public static readonly short STR_BUG_LEN = 40;
        public static readonly short HELP_CODE_LEN = 8;

        public bool          is_pushed;
        public bool          enable_flag;                    // either 1(Yes) or 0(No)
        public short         x1;
        public short y1;
        public short x2;
        public short y2;                    // some function will need to access the button's coordination for area detection

        public bool         init_flag;
        public bool         button_type;
        public bool         use_texture_flag;
        public ushort button_key;                    // button is pressed when user press this key
        public bool        is_showing;

        public string          str_buf;
        public void*         body_ptr;                       // can be a text pointer, bitmap pointer or a pointer to user defined function
        public char          elastic;
        public Font         font_ptr;
        public short*    button_bk_bitmap;
        public short*    button_up_bitmap;
        public short*    button_dn_bitmap;
        public string        help_code;

        public Button();
        public ~Button();

        public void deinit();
        public void use_texture()             { use_texture_flag=1; }

        public void reset()                         { init_flag=0; }
        public void set_font(Font);
        public void set_key(unsigned keyCode)       { button_key = keyCode; }
        public void create(int,int,int,int,int,void*,char=1,char=0);
        public void set_body(void*);
        public void set_help_code(char* helpCode);

        //-------- text button ------------//

        public void create_text(int x1,int y1,int x2,int y2,char* textPtr,char elastic=1,char defIsPushed=0)
	    { create( BUTTON_TEXT,x1,y1,x2,y2,textPtr,elastic,defIsPushed); }

        public void paint_text(int x1,int y1,int x2,int y2,char* textPtr,char elastic=1,char defIsPushed=0)
	    { create( BUTTON_TEXT,x1,y1,x2,y2,textPtr,elastic,defIsPushed); paint(); }

        public void create_text(int x1,int y1,char* textPtr,char elastic=1,char defIsPushed=0);

        public void paint_text(int x1,int y1,char* textPtr,char elastic=1,char defIsPushed=0);

        //-------- bitmap button -------------//

        public void create_bitmap(int x1,int y1,int x2,int y2,char* iconPtr,char elastic=1,char defIsPushed=0)
	    { create( BUTTON_BITMAP,x1,y1,x2,y2,iconPtr,elastic,defIsPushed); }

        public void paint_bitmap(int x1,int y1,int x2,int y2,char* iconPtr,char elastic=1,char defIsPushed=0)
	    { create( BUTTON_BITMAP,x1,y1,x2,y2,iconPtr,elastic,defIsPushed); paint(); }

        //###### begin Norris 0707 ########//
        public void create_bitmapW(int x1,int y1,int x2,int y2,short* iconPtr,char elastic=1,char defIsPushed=0)
	    { create( BUTTON_BITMAPW,x1,y1,x2,y2,iconPtr,elastic,defIsPushed); }

        public void paint_bitmapW(int x1,int y1,int x2,int y2,short* iconPtr,char elastic=1,char defIsPushed=0)
	    { create( BUTTON_BITMAPW,x1,y1,x2,y2,iconPtr,elastic,defIsPushed); paint(); }
        //######## end Norris 0707 ########//

        //-------- user-defined function button ---------//

        public void create_udf(int x1,int y1,int x2,int y2,ButtonFP funcPtr,char elastic=1,char defIsPushed=0)
	    { create( BUTTON_UDF,x1,y1,x2,y2,funcPtr,elastic,defIsPushed); }

        public void paint_udf(int x1,int y1,int x2,int y2,ButtonFP funcPtr,char elastic=1,char defIsPushed=0)
	    { create( BUTTON_UDF,x1,y1,x2,y2,funcPtr,elastic,defIsPushed); paint(); }

        //--------------------------------------//

        public void paint(int= -1, int=1);

        public int  detect(unsigned=0,unsigned=0,int=0,int=0);

        public void hide(char);
        public void show();

        public void push()        { if(!is_pushed) paint(1); }
        public void pop()         { if(is_pushed)  paint(0);  }

        public void disable() {
	    if(enable_flag) {
	        enable_flag=0; paint();
	    }
        }
        public void enable() {
	    if(!enable_flag) {
	        enable_flag=1; paint();
	    }
        }

        public void wait_press(int inactiveTimeOut=0);
    }
}
