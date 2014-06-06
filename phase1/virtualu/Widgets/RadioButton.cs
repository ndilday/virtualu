using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Widgets
{
    class RadioButton
    {
        public short radio_button_x1;
        public short radio_button_y1;
        public short radio_button_x2;
        public short radio_button_y2;
        public short radio_button_y;
        public short radio_button_width;
        public short radio_button_height;
        public int option_num;
        public int *sel_num;
        public int button_color;
        public string[] option_desc;
        public short *desc_x;
        public short *radio_button_bitmap;
        public Font font_ptr;
        public bool init_flag;

        void fill(short,short,int);
        void circle(short,short,int,int,char);

        public RadioButton();
        public ~RadioButton();
        public void init(int,int,int,int,int,int*,char**,int=vga_back.color_border);
        public void deinit();
        public void paint();
        public void refresh();
        public int detect();
        public void set_font(Font fontPtr) { font_ptr = fontPtr; }
    }
}
