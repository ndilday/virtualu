
namespace virtualu
{
    class GraphNonFinancial
    {
        public short xAxisPos;
        public short graph_x1;
        public short graph_y1;
        public short graph_x2;
        public short graph_y2;
        public short series_x1;
        public short series_y1;
        public short series_x2;
        public short series_y2;
        public short graph_height;
        public short graph_width;
        public short legend_height;
        public short legend_x_num;
        public short legend_y_num;
        public string[] legend_array;
        public string x_label;
        public string y_label;
        public int *series_color;
        public int axis_color;
        public bool isCurrent;
        public bool init_flag;
        public bool data_type_flag;
        public bool value_flag;
        public short legend_width;
        public short transparent_flag;
        public short orientation_flag;
        public int num_format;
        public int series_num;
        public int* data_num;
        public void *data_array;
        public int *x_start;
        public int *x_end;
        public short y_label_max_len;

        protected bool x_axis_pos;      // 0-bottom, 1-center
        protected short x_axis_step;              // step size on x-axis
        protected short scaleStep;
        protected Font *font_ptr;
        protected short *graph_bitmap;
        protected double max_scale;               // max abs value of the graph
        protected double min_scale;               // max abs value of the graph
        protected short posScaleNum;
        protected short negScaleNum;
        protected double scaleInc;
        protected double posScaleInc;
        protected double negScaleInc;

        public GraphNonFinancial();
        public ~GraphNonFinancial();

        public void init(int,int,int,int,int,int*,void*,char,int=-1,char* =NULL,char* =NULL,char** =NULL,char=1,char=1,char=1,int=1,int* =NULL,int=Vga::active_buf->color_dark);
        public void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,float* dataArray,int thisYear,char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	    { init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_FLOAT,thisYear, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }
        public void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,double* dataArray,int thisYear,char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	    { init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_DOUBLE,thisYear, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }
        public void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,int* dataArray,int thisYear,char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	    { init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_INT,thisYear, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }
        public void init(int x1,int y1,int x2,int y2,int seriesNum,int *dataNum,long* dataArray,int thisYear,char* xLabel,char* yLabel,char** legendArray=NULL,char transparentFlag=1,char orientationFlag=1,char valueFlag=1,int numFormat=1,int* seriesColor=NULL,int axisColor=Vga::active_buf->color_dark)
	    { init(x1, y1, x2, y2, seriesNum, dataNum, (void*) dataArray, DATA_LONG, thisYear, xLabel, yLabel, legendArray, transparentFlag, orientationFlag, valueFlag, numFormat, seriesColor, axisColor); }

        public void deinit();
        public void paint();
        public void refresh();
        public void set_font(Font *);
        public void calc_pos();

        public void set_y_label_max_len(short yLabelMaxLen) { y_label_max_len = yLabelMaxLen; }

        protected void draw_legend();
        protected void draw_axis_label();
        protected void find_scale();
        protected void draw_scale();
        protected void draw_series();
        protected void draw_series_float();
        protected void draw_series_double();
        protected void draw_series_int();
        protected void draw_series_long();
        protected void draw_value(void *, int, short, short, int);
    }

    class GraphNonFinancialYear : GraphNonFinancial
    {
    }

    class GraphNonFinancialTrimester : GraphNonFinancial
    {
    }
}
