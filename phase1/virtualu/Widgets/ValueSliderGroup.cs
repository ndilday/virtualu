using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Widgets
{
    class ValueSliderGroup
    {
        public int value_slider_num;
        public ValueSlider[] value_slider_array;

        public ValueSliderGroup(int);
        public void paint();
        public int detect();
        ValueSlider operator[](int recNo)  { if (recNo>value_slider_num) recNo=0; return value_slider_array[recNo]; }
    }
}
