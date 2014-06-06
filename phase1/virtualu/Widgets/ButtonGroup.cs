using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Widgets
{
    class ButtonGroup
    {
        public int     button_num;
        public int     button_pressed;
        public Button[] button_array;

        public ButtonGroup(int);
        public ~ButtonGroup();
        public void  deinit_buttons();                       //call deinit to each button

        public void paint(int= -1);
        public int  detect();
        public void push(int);

        public int     operator()()          { return button_pressed; }
    }
}
