//************** Temporary *****************//
#include <OSYS.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OTEST.H>
#include <OGSET.H>
#include <ORADIO.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OFINANCE.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OTABMENU.H>
#include <OSLIDER.H>
#include <OVALSLDR.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <OSPINNER.H>
#include <OGRPHYR.H>
#include <OGRPH_PY.H>
#include <OBNDSLDR.H>
#include <OGRPNFYR.H>

enum {
    REPORT_X1 = ZOOM_X1 + 15,
    REPORT_Y1 = ZOOM_Y1 + 15,
    REPORT_X2 = ZOOM_X2 - 15,
    REPORT_Y2 = ZOOM_Y2 - 70
};

static BoundedSlider bounded_slider;
static double bounded_slider_var, bounded_slider_lo, bounded_slider_up;

static GraphNonFinancialYear graph_a;
static GraphPerformanceIndicatorYear graph_b;
static char *legend[] = {
    "Legend 1", "Legend 2", "Legend 3", "Legend 4"
};
static int graph_a_start, graph_a_end, graph_data_num, graph_data_num1;
static int graph_data[] = {
    24, 37, 50, 18, 24, 37, 50, 46,
    24, 37, 50, 18, 80, 60, 90, 50,
    24, 37, 50, 7, 13, 20, 20, 30,
};

char *x_lab = "X-axis label";
char *y_lab = "Y-axis label";

static Spinner spinner, spinner2;
static float spinner_val, spinner_val2;

static RadioButton radio_button;
static int radio_sel;
static char *radio_desc[] = {
    "Selection 1", "Selection 2", "Selection 3"
};

static TabMenu tab_menu;
static int tab_sel;
static char *tab_short_desc[] = {
    "S1", "S2", "S3"
};
static char *tab_long_desc[] = {
    "Long Description 1", "Long Description 2", "Long Description 3"
};

static int callback_disp(int selNum, int refreshFlag) {
    vga_back.d3_panel_down(200+selNum*100, 200, 600, 300, 5);
    vga.blt_buf( 100, 150, 700, 350 );
    return 0;
}

static int callback_detect(int selNum) {
    return 0;
}

//static SliderGroup slider_group(3);
//static int slider_var[3];

static ValueSlider value_slider;
static int value_slider_var;

static char init_flag;
static int pic_id;

Test::Test() {
    radio_sel = 1;
    tab_sel = 1;
    //	slider_var[0] = 7;
    //	slider_var[1] = 8;
    //	slider_var[2] = 9;
    value_slider_var = 11;
    init_flag = 0;
    pic_id = 1;
    spinner_val = 2.5;
    spinner_val2 = 3.5;
    graph_a_start = 1993;
    graph_a_end = 1997;
    graph_data_num = 5;
    graph_data_num1 = 1;
    bounded_slider_lo = -5.0;
    bounded_slider_up = 5.0;
    bounded_slider_var = 1.0;
}

void Test::report(int refreshFlag) {
    vga.use_back();
    /*
      if (refreshFlag == INFO_REPAINT)
      vga_back.d3_panel_down(REPORT_X1, REPORT_Y1, REPORT_X2, REPORT_Y2, 2);

      if (!init_flag)
      {

      tab_menu.init(50, 100, 750, 400, 3, &tab_sel,
      80, 200, tab_short_desc, tab_long_desc,
      callback_disp, callback_detect);

      //		slider_group[0].init(200, 450, 400, 16, V_GREEN, &slider_var[0],
      //									20, 15, V_RED);
      //		slider_group[1].init(200, 470, 400, 16, V_GREEN, &slider_var[1],
      //									20, 15, V_RED);
      //		slider_group[2].init(200, 490, 400, 16, V_GREEN, &slider_var[2],
      //									20, 15, V_RED);

      value_slider.init(200, 420, 400, 20, 0, 100, &value_slider_var, vga_back.color_border, 3);

      init_flag = 1;
      }
    */

    //	vga.use_front();

    if (refreshFlag == INFO_REPAINT) {
	user_interface.bg_img(pic_id, &vga_back);
	user_interface.rect(100, 140, 700, 450, &vga_back);

	if (!init_flag) {
	    radio_button.init(140, 80, 450, 120, 3, &radio_sel, radio_desc);
	    spinner.init(200, 200, 300, 240, &spinner_val, 0.0, 5.0, 0.5);
	    //			spinner2.init(400, 200, 500, 240, &spinner_val2, 0.0, 5.0, 0.5, 0);
	    value_slider.init(200, 450, 400, 20, 0, 100, &value_slider_var);
	    graph_a.init(300, 200, 650, 400, 1, &graph_data_num, graph_data, 2, x_lab, y_lab, legend);
	    graph_b.init(100, 100, 175, 160, 1, &graph_data_num, graph_data, 1, 0, 3);
	    bounded_slider.init(500, 80, 200, 60, &bounded_slider_var, &bounded_slider_lo, &bounded_slider_up, -10.0, 10.0);
	    init_flag = 1;
	}
	else {
	    radio_button.paint();
	    spinner.paint();
	    //			spinner2.paint();
	    graph_a.paint();
	    graph_b.paint();
	    value_slider.paint();
	    bounded_slider.paint();
	}
	if (++pic_id == 6) pic_id = 1;

	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    }
    else {
	radio_button.refresh();
	spinner.refresh();
	//		spinner2.refresh();
	graph_a.refresh();
	graph_b.refresh();
	value_slider.refresh();
	bounded_slider.refresh();
    }

    vga.use_front();
}

void Test::detect() {
    vga.use_back();
    /*
      radio_button.detect();
      tab_menu.detect();
      //	slider_group.detect();
      value_slider.detect();
    */

    radio_button.detect();
    spinner.detect();
    //	spinner2.detect();
    value_slider.detect();
    bounded_slider.detect();

    vga.use_front();
}
