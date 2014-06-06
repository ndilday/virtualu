using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Widgets
{
    class ValueSlider
    {
        public short value_slider_x1;
        public short value_slider_y1;
        public short value_slider_x2;
        public short value_slider_y2;
        public int format_type;
        public int min_val;
        public int max_val;
        public int *var_ptr;
        public int bar_color;
        public int bar_width;
        public bool init_flag;
        public bool transparent_flag;
        public short *value_slider_bitmap;
        public Font font_ptr;

        public ValueSlider();
        public ~ValueSlider();
        public void init(int,int,int,int,int,int,int*,int=1,int=Vga::active_buf->color_up,int=1);
        public void init_target(int,int,int,int,int,int);
        public void set_target(int,int,int,int,int,int);
        public void deinit();
        public void paint();
        public void refresh();
        public int detect();
        public void set_font(Font fontPtr) { font_ptr = fontPtr; }

        //## chea 060999 for adding a target bar on the value slider
        int target_flat;                              //1 is on default=0
        int tar_x_pos;                                //x_pos of the target bar
        int tar_y_pos;                                //y_pos of the target bar
        int tar_height;                               //height of the target bar
        int tar_width;                                //width of the target bar
        int tar_borderColor;                          //border color the target bar
        int tar_shadowColor;                          //shadow color the target bar
    }
}
