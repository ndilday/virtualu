//Filename    : OFINDISP.cpp
//Description : Finance Class display method definition
//Owner       : Ho

#include <OBUTTON.H>                              // for this's disp_main
#include <OFONT.H>
#include <OSYS.H>                                 // sys.redraw_all_flag
#include <OWORLDMT.H>                             // ZOOM_??
#include <KEY.H>                                  // KEY_*
#include <OGAMESTR.H>                             // report_str()
#include <OFINANCE.H>
#include <OVGA.H>                                 // for this's disp_main
#include <OIFACE.H>
#include <OINFO.H>
#include <OSTR.H>
#include <OVBROW98.H>
#include <OOPT.H>
#include <OVGALOCK.H>
#include <OMATH.H>

//----- define constants for report -----//

#define FIN_REP_SHEET_COLOR   Vga::active_buf->color_down

enum {
    REPORT_X1 = ZOOM_X1+2,
    REPORT_Y1 = ZOOM_Y1+15,
    REPORT_X2 = ZOOM_X2-2,
    REPORT_Y2 = ZOOM_Y2-50,
};

enum { BG_PIC_ID  = 12};
enum { BUTTON_Y1 = REPORT_Y2+10 };
enum { BUTTON_WIDTH = 105, BUTTON_HEIGHT = 21 };
enum { FINANCIAL_REC_HEIGHT = 18 };
enum { BS_TAB1 = 220, BS_TAB2 = 550,BS_TAB3 = 600 };

//----- define variables for report -----//

static ButtonGroup  button_type(REPORT_TYPE_COUNT);
static ButtonGroup  button_year(REPORT_YEAR_COUNT);
static ButtonGroup  button_opt(REPORT_OPT_COUNT);
static Button     button_expand;
static Button     button_prev, button_next;
static VBrowse98    browse_financial;
static int        report_year = 1;                //temp
static void       disp_fin_rep_row(int,int,int,int);
static void       fin_rep_case1(int,int,int);
static void       fin_rep_case2(int,int,int);
static void       fin_rep_case3(int,int,int);
static void       fin_rep_case4(int,int,int);
static void       fin_rep_case8(int,int,int);
static void       fin_rep_case9(int,int,int);

// constant, macro, and function to exporting text file for import to spreadsheet
const int RECORD_SEPARATOR_LEN = 2;
static char *RECORD_SEPARATOR = "\r\n";
const int FIELD_SEPARATOR_LEN = 1;
static char *FIELD_SEPARATOR = "\t";

static char *export_num(int value) {
    static char num_str[20];
    return itoa( value, num_str, 10 );              // 10 is radix
}

static char *export_num(double value) {
    static char num_str[20];
    return gcvt( value, 7, num_str );               // a few more more than 7, for negative sign, decimal point and exponent
}

#define WRITE_FIELD_SEPARATOR(f)      f->file_write(FIELD_SEPARATOR, FIELD_SEPARATOR_LEN)
#define WRITE_STR_FIELD(f,s)          { f->file_write(s, strlen(s)); }
#define WRITE_NUM_FIELD(f,n)          { char *s=export_num(n); f->file_write(s, strlen(s)); }
#define WRITE_RECORD_SEPARATOR(f)     f->file_write(RECORD_SEPARATOR, RECORD_SEPARATOR_LEN)

// function to export finance report to text file
static void export_fin_rep_case1(File *);
static void export_fin_rep_case2(File *);
static void export_fin_rep_case3(File *);
static void export_fin_rep_case4(File *);
static void export_fin_rep_case5(File *);
static void export_fin_rep_case6(File *);
static void export_fin_rep_case7(File *);
static void export_fin_rep_case8(File *);
static void export_fin_rep_case9(File *);

static Button button_export;                      // button to export to (spread sheet) text file

//----- Begin of function Finance::report ------//
//!
//! Display Financial Report
//!
//! <int> refreshFlag   = INFO_REPAINT or INFO_UPDATE
//! [int] dispButton    = whether should display buttons or not (0 when called from finance.new_fiscal_year_report())
//!							    (default: 1)
//! [char*] reportTitle = report title (Report title)
//!
void Finance::disp_report_main(int refreshFlag, int dispButton, char* reportTitle) {
    int x=REPORT_X1, y=REPORT_Y1;
    vga.use_back();
    sys.is_institution_report=1;
    if (refreshFlag == INFO_REPAINT) {
	// ###### begin Gilbert 25/04/2001 ######//
	report_deinit();
	// ###### end Gilbert 25/04/2001 ######//

	//------------- paint the background ----------------//

	user_interface.bg_img(BG_PIC_ID, &vga_back);

	if( dispButton )
	    report_paint_button(refreshFlag);

	int reportY1=REPORT_Y1, reportY2=REPORT_Y2;

	if( reportTitle ) {
	    reportY1 += 15;
	    font_charts.center_put( REPORT_X1, ZOOM_Y1, REPORT_X2, reportY1-1, reportTitle );
	}

	if( !dispButton )
	    reportY2 += BUTTON_HEIGHT-8;

	// OPTIMIZATION
	if(report_type==REPORT_TYPE_OPTIMIZATION) {
	    if(finance.report_opt==REPORT_OPT_REVENUE)
		optimization.report_stage_1(refreshFlag);
	    if(finance.report_opt==REPORT_OPT_FUNCTION)
		optimization.report_stage_2(refreshFlag);
	    if(finance.report_opt==REPORT_OPT_FACULTY)
		optimization.report_stage_3(refreshFlag);
	    //				opt_stage3.report(refreshFlag);
	}
	else {
	    //			user_interface.bar( REPORT_X1, reportY1, REPORT_X2, reportY2,FIN_REP_SHEET_COLOR);

	    user_interface.brighten( REPORT_X1, reportY1, REPORT_X2, reportY2);
	    user_interface.rect( REPORT_X1, reportY1, REPORT_X2, reportY2);

	    //-------- set the number of rows for th browser --------//

	    int browseRecCount;
	    if(report_type==REPORT_TYPE_REVENUE||report_type==REPORT_TYPE_BUDGET_VS_ACTUAL) {
		if(report_year!=REPORT_YEAR_THREE_YEARS) {
		    browseRecCount=REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+9;
		}
		else {
		    if( report_is_expanded )
			browseRecCount=REVENUE_ITEM_COUNT+4*EXPENSE_ITEM_COUNT+25;
		    else
			browseRecCount=REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+9;
		}
	    }

	    if(report_type==REPORT_TYPE_DETAIL_REN)
		browseRecCount=REVENUE_ITEM_COUNT+4*EXPENSE_ITEM_COUNT+25;

	    if(report_type==REPORT_TYPE_BALANCE) {
		if(report_year==REPORT_YEAR_THREE_YEARS)
		    browseRecCount=ASSET_ITEM_COUNT+LIABILITY_ITEM_COUNT+8;
		else
		    browseRecCount=ASSET_ITEM_COUNT+LIABILITY_ITEM_COUNT+7;
	    }

	    //-------------------------------------------//

	    browse_financial.init( REPORT_X1, reportY1, REPORT_X2, reportY2,
				   0, FINANCIAL_REC_HEIGHT, browseRecCount,
				   disp_fin_rep_row, NULL ,0);

	    browse_financial.open(report_financial_recno);
	}

    }
    else {                                          // refreshFlag == INFO_REFRESH
	if(report_type==REPORT_TYPE_OPTIMIZATION) {
	    if(finance.report_opt==REPORT_OPT_REVENUE)
		optimization.report_stage_1(refreshFlag);
	    if(finance.report_opt==REPORT_OPT_FUNCTION)
		optimization.report_stage_2(refreshFlag);
	    if(finance.report_opt==REPORT_OPT_FACULTY)
		optimization.report_stage_3(refreshFlag);
	}
	else {
	    // update only
	    browse_financial.update();

	    if( dispButton )
		report_paint_button(refreshFlag);
	}
    }

    //---------------------------------------------//
    if(report_type==REPORT_TYPE_OPTIMIZATION) {
    }
    else {
	report_financial_recno=browse_financial.recno();
    }
    vga.blt_buf( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2 );
}

//----- End of function Finance::report ------//

void Finance::report_deinit() {
    /*		// OPTIMIZATION
		if(finance.report_type==REPORT_TYPE_OPTIMIZATION)
		{
		if(finance.report_opt==REPORT_OPT_REVENUE)
		optimization.deinit_stage_1(refreshFlag);
		if(finance.report_opt==REPORT_OPT_FUNCTION)
		optimization.deinit_stage_2(refreshFlag);
		if(finance.report_opt==REPORT_OPT_FACULTY)
		optimization.deinit_stage_3(refreshFlag);
		}
    */
    // ### begin Gilbert 25/04/2001 ######//
    optimization.deinit();                          // no need to check current stage, finance report didn't free memory properly

    button_type.deinit_buttons();
    button_year.deinit_buttons();
    button_opt.deinit_buttons();
    button_expand.deinit();
    button_prev.deinit();
    button_next.deinit();
    browse_financial.deinit();
    // ### end Gilbert 25/04/2001 ######//
    button_export.deinit();
}

//------- Begin of function Finance::report_detect -------//
//!
//! [int] detectButton  = whether should display buttons or not (0 when called from finance.new_fiscal_year_report())
//!							  (default: 1)
//!
void Finance::report_detect(int detectButton) {
    if( detectButton ) {
	//------- detect type buttons --------//
	if( button_type.detect() >= 0 ) {
	    report_type = button_type.button_pressed;
	    sys.redraw_all_flag = 1;
	    report_financial_recno=1;
	    report_is_expanded = 0;
	}

	if(report_type==REPORT_TYPE_OPTIMIZATION) {
	    if( button_opt.detect() >= 0 ) {
		report_opt = button_opt.button_pressed;
		sys.redraw_all_flag = 1;
		report_financial_recno=1;
		report_is_expanded = 0;
	    }
	}
	else if((report_type==REPORT_TYPE_REVENUE)||(report_type==REPORT_TYPE_BALANCE)) {
	    if( button_year.detect() >= 0 ) {
		report_year = button_year.button_pressed;
		sys.redraw_all_flag = 1;
		report_financial_recno=1;
		report_is_expanded = 0;
	    }

	}

	// detect export button

	if((report_type==REPORT_TYPE_REVENUE)||(report_type==REPORT_TYPE_BALANCE)||(report_type==REPORT_TYPE_DETAIL_REN)) {
	    if( button_export.detect() ) {
		// ask file name now

		MouseDispCount mouseDispCount;            // redisplay windows cursor

		char tempFilename[MAX_PATH];

		tempFilename[0] = '\0';                   // strcpy(tempFilename, "finance.txt");

		// ask filename now
		OPENFILENAME openFileName;
		memset( &openFileName, 0, sizeof(openFileName) );
		openFileName.lStructSize = sizeof(openFileName);
		openFileName.hwndOwner = sys.main_hwnd;
		openFileName.hInstance = sys.app_hinstance;
		// two '\0' at the end
		openFileName.lpstrFilter = "Text file(*.txt)\0*.txt\0All Files(*.*)\0*.*\0";
		openFileName.lpstrCustomFilter = NULL;
		openFileName.nMaxCustFilter = 0;
		openFileName.nFilterIndex = 0;
		openFileName.lpstrFile =  tempFilename;   // first char is already NULL
		openFileName.nMaxFile = sizeof(tempFilename);
		openFileName.lpstrFileTitle = NULL;       // result filename, without path
		openFileName.nMaxFileTitle = 0;
		openFileName.lpstrInitialDir = ".";       // current directory
		openFileName.lpstrTitle = NULL;
		openFileName.Flags = OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
		openFileName.nFileOffset = 0;
		openFileName.nFileExtension = 0;
		openFileName.lpstrDefExt = "txt";
		openFileName.lCustData = NULL;
		openFileName.lpfnHook = NULL;
		openFileName.lpTemplateName = NULL;

		if( GetSaveFileName(&openFileName) ) {
		    File exportFile;
		    exportFile.file_create( tempFilename );
		    if( report_type==REPORT_TYPE_REVENUE) {
			switch(report_year) {
			case 1: export_fin_rep_case1( &exportFile ); break;
			case 2: export_fin_rep_case2( &exportFile ); break;
			case 3: export_fin_rep_case3( &exportFile ); break;
			case 0:                             // fall through
			default: export_fin_rep_case4( &exportFile );
			}
		    }
		    else if(report_type==REPORT_TYPE_BALANCE) {
			// no matter which report_year, always print export_fin_rep_case8
			export_fin_rep_case8( &exportFile );
		    }
		    else if(report_type==REPORT_TYPE_DETAIL_REN) {
			export_fin_rep_case9( &exportFile );
		    }

		    exportFile.file_close();
		}
	    }
	}

	if( button_expand.detect() ) {
	    report_type = REPORT_TYPE_DETAIL_REN;
	    sys.redraw_all_flag = 1;
	    report_financial_recno=1;
	    //			report_is_expanded = !report_is_expanded;
	    //			sys.redraw_all_flag = 1;
	    //			report_financial_recno=1;
	}
    }
    if(report_type==REPORT_TYPE_OPTIMIZATION) {
	if(finance.report_opt==REPORT_OPT_REVENUE)
	    optimization.detect_stage_1();
	if(finance.report_opt==REPORT_OPT_FUNCTION)
	    optimization.detect_stage_2();
	if(finance.report_opt==REPORT_OPT_FACULTY)
	    optimization.detect_stage_3();
    }
    else
	browse_financial.detect();
}

//------- End of function Finance::report_detect -------//

//------- Begin of function Finance::report_paint_button ------//

void Finance::report_paint_button(int refreshFlag) {
    if( refreshFlag == INFO_REPAINT ) {
	//-------- paint type buttons ------------//

	int x=REPORT_X1+10, y=BUTTON_Y1-6;

	button_type[0].create_text( x, y, x+(int)(2.7*BUTTON_WIDTH-1), y+BUTTON_HEIGHT-1, "Budget vs Actual" );
	x+=(int)(2.7*BUTTON_WIDTH+15);

	button_type[1].create_text( x, y, x+(int)(1.2*BUTTON_WIDTH-1), y+BUTTON_HEIGHT-1, "Balance sheet" );
	x+=(int)(1.2*BUTTON_WIDTH+15);

	button_type[2].create_text( x, y, x+(int)(1.5*BUTTON_WIDTH-1), y+BUTTON_HEIGHT-1, "Budget plan" );
	x+=(int)(1.5*BUTTON_WIDTH+15);

	button_type[3].create_text( x, y, x+(int)(0.8*BUTTON_WIDTH-1), y+BUTTON_HEIGHT-1, "Detail", 0 );
	x=REPORT_X1+25, y=BUTTON_Y1+18;

	//---------show year buttons--------------------//

	if(report_type==REPORT_TYPE_OPTIMIZATION) {
	    button_opt[0].create_text( x, y, x+(int)(2*BUTTON_WIDTH-1), y+BUTTON_HEIGHT-1, "Revenue and expenditure" );

	    x+=(int)(2*BUTTON_WIDTH+20);
	    button_opt[1].create_text( x, y, x+(int)(2*BUTTON_WIDTH-1), y+BUTTON_HEIGHT-1, "Allocations");

	    x+=(int)(2*BUTTON_WIDTH+20);
	    button_opt[2].create_text( x, y, x+(int)(2*BUTTON_WIDTH-1), y+BUTTON_HEIGHT-1, "Faculty hiring");
	}

	if((report_type==REPORT_TYPE_REVENUE)
	   ||(report_type==REPORT_TYPE_BALANCE)) {
	    String s1;

	    int tyear=(int)(info.game_year+(info.game_month-9.0)/12);

	    s1="Standard";
	    button_year[0].create_text( x, y, x+BUTTON_WIDTH+40-1, y+BUTTON_HEIGHT-1, s1.str_buf );

	    x+=BUTTON_WIDTH+65;
	    s1="Year ";
	    s1+=(m.format(tyear-1,4));
	    button_year[1].create_text( x, y, x+BUTTON_WIDTH-1, y+BUTTON_HEIGHT-1, s1.str_buf );

	    x+=BUTTON_WIDTH+10;
	    s1="Year ";
	    s1+=(m.format(tyear,4));
	    button_year[2].create_text( x, y, x+BUTTON_WIDTH-1, y+BUTTON_HEIGHT-1, s1.str_buf );

	    x+=BUTTON_WIDTH+10;
	    s1="Year ";
	    s1+=(m.format(tyear+1,4));
	    button_year[3].create_text( x, y, x+BUTTON_WIDTH-1, y+BUTTON_HEIGHT-1, s1.str_buf );

	    x=REPORT_X2-BUTTON_WIDTH-25;
	    button_export.create_text( x, y, x+BUTTON_WIDTH-1, y+BUTTON_HEIGHT-1, "Export" );
	}

	if(report_type==REPORT_TYPE_DETAIL_REN) {
	    x=REPORT_X2-BUTTON_WIDTH-25;
	    button_export.create_text( x, y, x+BUTTON_WIDTH-1, y+BUTTON_HEIGHT-1, "Export" );
	}

	button_type.paint(report_type);

	if( report_type==REPORT_TYPE_BALANCE) {
	    //			for(int k=0;k<4;k++)
	    //				button_year[k].paint();
	    //			button_year[report_year].disable();
	    button_year.paint(report_year);
	    button_export.paint();
	}
	else if(report_type==REPORT_TYPE_OPTIMIZATION) {
	    if((report_opt<0)||(report_opt>3))
		report_opt=REPORT_OPT_REVENUE;
	    button_opt.paint(report_opt);
	}
	else if(report_type==REPORT_TYPE_REVENUE) {
	    button_year.paint(report_year);
	    button_export.paint();
	}
	else if(report_type==REPORT_TYPE_DETAIL_REN) {
	    button_export.paint();
	}
    }
}

//------- End of function Finance::report_paint_button ------//

//------ Begin of static function fin_rep_case1 --------//
// Case 1: RENVENUE	and PREVIOUS YEAR
// recNo -- record no. (row no.) to be displayed
// x,y	-- left top position of the display region
//
static void fin_rep_case1(int recNo,int x, int y) {
    int i;
    //	int	xColumn1=x+305, xColumn2=440, xColumn3=510, xColumn4=590, xColumn5=680;
    int xColumn1=370, xColumn2=470, xColumn3=570, xColumn4=670, xColumn5=740;

    // Show title //
    if(recNo==1) {
	// change in verion 2
	// int tyear=(int)(info.game_year+(info.game_month-9.0)/12);
	int tyear = info.financial_year();
	///  BUGHERE ## chwg0301
	if(sys.view_mode==MODE_YEAR_END_REPORT)tyear++;

	String str;
	str="Previous Year : Year ";
	str+=m.format(tyear-1,4);

	font_charts.put(xColumn1-70,y+2,str.str_buf);
	return;
    }
    recNo--;

    if(recNo==1) {
	font_chart_red_sm.disp( x, y, "Revenue", x+200 );
	font_chart_red_sm.right_put( xColumn2, y+2, "Direct");
	font_chart_red_sm.right_put( xColumn3, y+2, "Indirect");
	font_chart_red_sm.right_put( xColumn4, y+2, "Total");
	font_chart_red_sm.right_put( xColumn5, y+2, "% sources");
    }
    else if((recNo>1) && (recNo<=REVENUE_ITEM_COUNT+1)) {
	i=recNo-2;
	if(recNo==5) {
	    //font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn2);
	    //font_chartsm.disp( xColumn2, y+2, finance.revenue_array[i].direct, 2, xColumn3);
	    //font_chartsm.disp( xColumn3, y+2, finance.revenue_array[i].indirect, 2, xColumn4);
	    //font_chartsm.disp( xColumn4, y+2, finance.revenue_array[i].total, 2, xColumn5 );
	    //font_chartsm.disp( xColumn5, y+2, finance.revenue_array[i].percent, 3, xColumn6 );

	    font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn2);
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.revenue_array[i].direct, 2));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.revenue_array[i].indirect, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.revenue_array[i].total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.revenue_array[i].percent, 24));
	}
	if((recNo==2)||(recNo==3)) {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn2);
	    //				font_chartsm.disp( xColumn4, y+2, finance.revenue_array[i].total, 2, xColumn5 );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.revenue_array[i].total, 2));
	}
	else {
	    if( recNo==4 )                              // use blue font for Net tuition income
		font_chart_blue_sm.disp( x, y, game_str_res.financial_report_str(i),  xColumn2);
	    else
		font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn2);

	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.revenue_array[i].total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.revenue_array[i].percent, 24));
	}
    }
    else if(recNo==REVENUE_ITEM_COUNT+2) {
	font_chart_blue_sm.disp( x, y,"  Total sources of funds", xColumn1);
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.total_revenue.total, 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.total_revenue.percent, 24));
    }
    else if(recNo==REVENUE_ITEM_COUNT+3) {
	font_chart_red_sm.right_put( xColumn1, y+2, "AY faculty");
	font_chart_red_sm.right_put( xColumn2, y+2, "Staff");
	font_chart_red_sm.right_put( xColumn3, y+2, "Other");
    }
    else if(recNo==REVENUE_ITEM_COUNT+4) {
	font_chart_red_sm.disp( x, y, "Expenditures", x+200 );
	font_chart_red_sm.right_put( xColumn1, y+2, "compensation");
	font_chart_red_sm.right_put( xColumn2, y+2, "compensation");
	font_chart_red_sm.right_put( xColumn3, y+2, "expenditures");
	font_chart_red_sm.right_put( xColumn4, y+2, "Total");
	font_chart_red_sm.right_put( xColumn5, y+2, "% uses");
    }
    else if((recNo>=REVENUE_ITEM_COUNT+5) && (recNo<REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3)) {
	i=recNo-5-REVENUE_ITEM_COUNT;
	if((recNo>=REVENUE_ITEM_COUNT+7) && (recNo<REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3)) {
	    font_chartsm.disp( x, y,game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),xColumn2);
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.expense_array[i].staff, 2));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.expense_array[i].other, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.expense_array[i].total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.expense_array[i].percent, 24));
	}
	else {
	    font_chartsm.disp( x, y,game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[i].faculty, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.expense_array[i].staff, 2));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.expense_array[i].other, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.expense_array[i].total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.expense_array[i].percent, 24));
	}
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3) {
	font_chart_blue_sm.disp( x, y,"  Total operating expenditures", xColumn1);
	font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.total_operating_expense.faculty, 2));
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.total_operating_expense.staff, 2));
	font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.total_operating_expense.other, 2));
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.total_operating_expense.total, 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.total_operating_expense.percent, 24));
    }
    else if((recNo>REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3) && (recNo<=REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+5)) {
	i=recNo-6-REVENUE_ITEM_COUNT;
	font_chartsm.disp( x, y,game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),xColumn1);
	font_chartsm.right_put( xColumn3, y+2, m.format(finance.expense_array[i].other, 2));
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.expense_array[i].total, 2));
	font_chartsm.right_put( xColumn5, y+2, m.format(finance.expense_array[i].percent, 24));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+6) {
	font_chart_blue_sm.disp( x, y,"  Total uses of funds", xColumn1);
	font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.total_expense.faculty, 2));
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.total_expense.staff, 2));
	font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.total_expense.other, 2));
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.total_expense.total, 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.total_expense.percent, 24));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+7) {
	double percent1,percent2,percent3,percent4;
	percent1=100*finance.total_expense.faculty/finance.total_expense.total;
	percent2=100*finance.total_expense.staff/finance.total_expense.total;
	percent3=100*finance.total_expense.other/finance.total_expense.total;
	percent4=100;

	font_chartsm.disp( x, y, "\tPercent of total uses",  xColumn1);
	font_chartsm.right_put( xColumn1, y+2, m.format(percent1, 24));
	font_chartsm.right_put( xColumn2, y+2, m.format(percent2, 24));
	font_chartsm.right_put( xColumn3, y+2, m.format(percent3, 24));
	font_chartsm.right_put( xColumn4, y+2, m.format(percent4, 24));
	//			font_chartsm.disp( xColumn5, y+2, "% uses", xColumn6 );
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+8) {
	font_chartsm.disp( x, y,"  Surplus (deficit)", xColumn1);

	if(finance.last_year.surplus_deficit>=0)
	    font_chart_green_sm.right_put( xColumn4, y+2, m.format(finance.last_year.surplus_deficit, 2) );
	else
	    font_chart_red_sm.right_put( xColumn4, y+2, m.format(finance.last_year.surplus_deficit, 2));
    }

}

//------ End of static function fin_rep_case1 --------//

//------ Begin of static function export_fin_rep_case1 --------//
//
// write fin_rep_case1 to a text file
//
void export_fin_rep_case1( File *filePtr ) {
    // year no
    int tyear = info.financial_year();
    WRITE_STR_FIELD(filePtr, "Previous Year : " ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, tyear-1); WRITE_RECORD_SEPARATOR(filePtr);
    // revenue label
    WRITE_STR_FIELD(filePtr, "Revenue"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank field
    WRITE_STR_FIELD(filePtr, "Direct"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Indirect"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Total"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "% sources"); WRITE_RECORD_SEPARATOR(filePtr);

    // revenue
    for( int i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD(filePtr, game_str_res.financial_report_str(i)); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_FIELD_SEPARATOR(filePtr);               // blank field
	// only AC_SPONSORED_RESEARCH_REVENUE display direct and indirect
	if( i == AC_SPONSORED_RESEARCH_REVENUE ) {
	    WRITE_NUM_FIELD(filePtr, finance.revenue_array[i].direct); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.revenue_array[i].indirect); WRITE_FIELD_SEPARATOR(filePtr);
	}
	else {
	    WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_FIELD_SEPARATOR(filePtr);
	}
	WRITE_NUM_FIELD(filePtr, finance.revenue_array[i].total); WRITE_FIELD_SEPARATOR(filePtr);
	// don't show % sources AC_GROSS_TUITION_INCOME,AC_FINANCIAL_AID
	if( i==AC_GROSS_TUITION_INCOME||i==AC_FINANCIAL_AID ) {
	    // empty
	}
	else {
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.revenue_array[i].percent);
	}
	WRITE_RECORD_SEPARATOR(filePtr);
    }
    // total revenue
    WRITE_STR_FIELD(filePtr, "  Total sources of funds"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank field
    WRITE_FIELD_SEPARATOR(filePtr);                 // direct field
    WRITE_FIELD_SEPARATOR(filePtr);                 // indirect field
    WRITE_NUM_FIELD(filePtr, finance.total_revenue.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_revenue.percent); WRITE_RECORD_SEPARATOR(filePtr);

  // expenditure label
    WRITE_STR_FIELD(filePtr, "Expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "AY faculty compensation"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Staff compensation"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Other expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Total"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "% uses"); WRITE_RECORD_SEPARATOR(filePtr);

  // expenditure item
    for( int i=0; i < EXPENSE_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD(filePtr, game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT)); WRITE_FIELD_SEPARATOR(filePtr);
	if( i==AC_ACADEMIC_DEPARTMENTS || i==AC_SPONSORED_RESEARCH_EXPENSE ) {
	    WRITE_NUM_FIELD(filePtr, finance.expense_array[i].faculty);
	}
	WRITE_FIELD_SEPARATOR(filePtr);
	if( i < OPERATING_EXPENSE_ITEM_COUNT ) {
	    WRITE_NUM_FIELD(filePtr, finance.expense_array[i].staff);
	}
	WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.expense_array[i].other); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.expense_array[i].total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.expense_array[i].percent); WRITE_RECORD_SEPARATOR(filePtr);

	// total operatring expenditure
	if( i==OPERATING_EXPENSE_ITEM_COUNT-1 ) {
	    WRITE_STR_FIELD(filePtr, "  Total operating expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.total_operating_expense.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.total_operating_expense.staff); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.total_operating_expense.other); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.total_operating_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.total_operating_expense.percent); WRITE_RECORD_SEPARATOR(filePtr);
	}
    }

    // total use of fund
    WRITE_STR_FIELD(filePtr, "  Total uses of funds" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_expense.faculty ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_expense.staff); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_expense.other); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.total_expense.percent); WRITE_RECORD_SEPARATOR(filePtr);

  // percentage of total uses
    double percent1,percent2,percent3,percent4;
    percent1=finance.total_expense.faculty/finance.total_expense.total;
    percent2=finance.total_expense.staff/finance.total_expense.total;
    percent3=finance.total_expense.other/finance.total_expense.total;
    percent4=1.0;

    WRITE_STR_FIELD(filePtr, "    Percent of total uses" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent1); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent2); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent3); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent4); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_RECORD_SEPARATOR(filePtr);                // blank on % uses field

    // surplus
    WRITE_STR_FIELD(filePtr, "  Surplus (deficit)"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // AY faculty compensation field
    WRITE_FIELD_SEPARATOR(filePtr);                 // staff compensation field
    WRITE_FIELD_SEPARATOR(filePtr);                 // other expenditure field
    WRITE_NUM_FIELD(filePtr, finance.last_year.surplus_deficit); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_RECORD_SEPARATOR(filePtr);                // blank on % uses field
}

//------ End of static function export_fin_rep_case1 --------//

//------ Begin of static function fin_rep_case2 --------//
// Case 2: RENVENUE	and LAST YEAR
// recNo -- record no. (row no.) to be displayed
// x,y	-- left top position of the display region
//
static void fin_rep_case2(int recNo,int x, int y) {
    int i;
    int xColumn1=370, xColumn2=470, xColumn3=570, xColumn4=670, xColumn5=740;

    // Show title //
    if(recNo==1) {
	// change in version 2
	// int tyear=(int)(info.game_year+(info.game_month-9.0)/12);
	int tyear = info.financial_year();

	String str;
	str="Present Year : Year ";
	//			str+=m.format(tyear,16);    //## chea 061199 2.5.1
	str+=m.format(tyear,4);

	//			str+="-";
	//			str+=m.format(tyear+1,16);  //## chea 061199 2.5.1

	font_charts.put(xColumn1-70,y+2,str.str_buf);
	return;
    }
    recNo--;

    if(recNo==1) {
	font_chart_red_sm.disp( x, y, "Revenue", x+200 );
	font_chart_red_sm.right_put( xColumn2, y+2, "Direct");
	font_chart_red_sm.right_put( xColumn3, y+2, "Indirect");
	font_chart_red_sm.right_put( xColumn4, y+2, "Total");
	font_chart_red_sm.right_put( xColumn5, y+2, "% sources");
    }
    else if((recNo>1) && (recNo<=REVENUE_ITEM_COUNT+1)) {
	i=recNo-2;
	if(recNo==5) {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn2);
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].this_year.direct, 2));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_revenue_array[i].this_year.indirect, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].this_year.total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].this_year.percent, 24));
	}
	if((recNo==2)||(recNo==3)) {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn2);
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].this_year.total, 2));
	}
	else {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn2);
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].this_year.total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].this_year.percent, 24));
	}
    }
    else if(recNo==REVENUE_ITEM_COUNT+2) {
	font_chart_blue_sm.disp( x, y,"  Total sources of funds", xColumn1);
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_revenue.this_year.total, 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.projected_total_revenue.this_year.percent, 24));
    }
    else if(recNo==REVENUE_ITEM_COUNT+3) {
	font_chart_red_sm.right_put( xColumn1, y+2, "AY faculty");
	font_chart_red_sm.right_put( xColumn2, y+2, "Staff");
	font_chart_red_sm.right_put( xColumn3, y+2, "Other");
    }
    else if(recNo==REVENUE_ITEM_COUNT+4) {
	font_chart_red_sm.disp( x, y, "Expenditures", x+200 );
	font_chart_red_sm.right_put( xColumn1, y+2, "compensation");
	font_chart_red_sm.right_put( xColumn2, y+2, "compensation");
	font_chart_red_sm.right_put( xColumn3, y+2, "expenditures");
	font_chart_red_sm.right_put( xColumn4, y+2, "Total");
	font_chart_red_sm.right_put( xColumn5, y+2, "% uses");
    }
    else if((recNo>=REVENUE_ITEM_COUNT+5) && (recNo<REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3)) {
	i=recNo-5-REVENUE_ITEM_COUNT;
	if((recNo>=REVENUE_ITEM_COUNT+7) && (recNo<REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3)) {
	    font_chartsm.disp( x, y,game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),xColumn2);
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[i].this_year.staff, 2));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[i].this_year.other, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].this_year.total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[i].this_year.percent, 24));
	}
	else {
	    font_chartsm.disp( x, y,game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),xColumn2);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.projected_expense_array[i].this_year.faculty, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[i].this_year.staff, 2));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[i].this_year.other, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].this_year.total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[i].this_year.percent, 24));
	}
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3) {
	font_chart_blue_sm.disp( x, y,"  Total operating expenditures", xColumn1);
	font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.projected_total_operating_expense.this_year.faculty, 2));
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_operating_expense.this_year.staff, 2));
	font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.projected_total_operating_expense.this_year.other, 2));
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_operating_expense.this_year.total, 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.projected_total_operating_expense.this_year.percent, 24));
    }
    else if((recNo>REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3) && (recNo<=REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+5)) {
	i=recNo-6-REVENUE_ITEM_COUNT;
	font_chartsm.disp( x, y,game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),xColumn2);
	font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[i].this_year.other, 2));
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].this_year.total, 2));
	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[i].this_year.percent, 24));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+6) {
	font_chart_blue_sm.disp( x, y,"  Total uses of funds", xColumn1);
	font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.projected_total_expense.this_year.faculty, 2));
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_expense.this_year.staff, 2));
	font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.projected_total_expense.this_year.other, 2));
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_expense.this_year.total, 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.projected_total_expense.this_year.percent, 24));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+7) {
	double percent1,percent2,percent3,percent4;
	// fix in version 2
	//percent1=100*finance.total_expense.faculty/finance.total_expense.total;
	//percent2=100*finance.total_expense.staff/finance.total_expense.total;
	//percent3=100*finance.total_expense.other/finance.total_expense.total;
	percent1=100*finance.projected_total_expense.this_year.faculty/finance.projected_total_expense.this_year.total;
	percent2=100*finance.projected_total_expense.this_year.staff/finance.projected_total_expense.this_year.total;
	percent3=100*finance.projected_total_expense.this_year.other/finance.projected_total_expense.this_year.total;
	percent4=100;

	font_chartsm.disp( x, y, "\tPercent of total uses",  xColumn1);
	font_chartsm.right_put( xColumn1, y+2, m.format(percent1, 24));
	font_chartsm.right_put( xColumn2, y+2, m.format(percent2, 24));
	font_chartsm.right_put( xColumn3, y+2, m.format(percent3, 24));
	font_chartsm.right_put( xColumn4, y+2, m.format(percent4, 24));
	//			font_chartsm.disp( xColumn5, y+2, "% uses", xColumn6 );
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+8) {
	font_chartsm.disp( x, y,"  Surplus (deficit)", xColumn1);
	if(finance.this_year.surplus_deficit>0)
	    font_chart_green_sm.right_put( xColumn4, y+2, m.format(finance.this_year.surplus_deficit, 2));
	else
	    font_chart_red_sm.right_put( xColumn4, y+2, m.format(finance.this_year.surplus_deficit, 2));
    }
}

//------ End of static function fin_rep_case2 --------//

//------ Begin of static function export_fin_rep_case2 --------//
//
// write fin_rep_case2 to a text file
//
void export_fin_rep_case2( File *filePtr ) {
    // year no
    int tyear = info.financial_year();
    WRITE_STR_FIELD(filePtr, "Present Year : " ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, tyear); WRITE_RECORD_SEPARATOR(filePtr);
    // revenue label
    WRITE_STR_FIELD(filePtr, "Revenue"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank field
    WRITE_STR_FIELD(filePtr, "Direct"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Indirect"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Total"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "% sources"); WRITE_RECORD_SEPARATOR(filePtr);

    // revenue
    for( int i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD(filePtr, game_str_res.financial_report_str(i)); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_FIELD_SEPARATOR(filePtr);               // blank field
	// only AC_SPONSORED_RESEARCH_REVENUE display direct and indirect
	if( i == AC_SPONSORED_RESEARCH_REVENUE ) {
	    WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].this_year.direct); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].this_year.indirect); WRITE_FIELD_SEPARATOR(filePtr);
	}
	else {
	    WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_FIELD_SEPARATOR(filePtr);
	}
	WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].this_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	// don't show % sources AC_GROSS_TUITION_INCOME,AC_FINANCIAL_AID
	if( i==AC_GROSS_TUITION_INCOME||i==AC_FINANCIAL_AID ) {
	    // empty
	}
	else {
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_revenue_array[i].this_year.percent);
	}
	WRITE_RECORD_SEPARATOR(filePtr);
    }
    // total revenue
    WRITE_STR_FIELD(filePtr, "  Total sources of funds"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank field
    WRITE_FIELD_SEPARATOR(filePtr);                 // direct field
    WRITE_FIELD_SEPARATOR(filePtr);                 // indirect field
    WRITE_NUM_FIELD(filePtr, finance.projected_total_revenue.this_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_revenue.this_year.percent); WRITE_RECORD_SEPARATOR(filePtr);

  // expenditure label
    WRITE_STR_FIELD(filePtr, "Expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "AY faculty compensation"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Staff compensation"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Other expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Total"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "% uses"); WRITE_RECORD_SEPARATOR(filePtr);

  // expenditure item
    for( int i=0; i < EXPENSE_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD(filePtr, game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT)); WRITE_FIELD_SEPARATOR(filePtr);
	if( i==AC_ACADEMIC_DEPARTMENTS || i==AC_SPONSORED_RESEARCH_EXPENSE ) {
	    WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].this_year.faculty);
	}
	WRITE_FIELD_SEPARATOR(filePtr);
	if( i < OPERATING_EXPENSE_ITEM_COUNT ) {
	    WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].this_year.staff);
	}
	WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].this_year.other); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].this_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].this_year.percent); WRITE_RECORD_SEPARATOR(filePtr);

	if( i==OPERATING_EXPENSE_ITEM_COUNT-1 ) {
	    // total operatring expenditure
	    WRITE_STR_FIELD(filePtr, "  Total operating expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.this_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.this_year.staff); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.this_year.other); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.this_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.this_year.percent); WRITE_RECORD_SEPARATOR(filePtr);
	}
    }

    // total use of fund
    WRITE_STR_FIELD(filePtr, "  Total uses of funds" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.this_year.faculty ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.this_year.staff); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.this_year.other); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.this_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.this_year.percent); WRITE_RECORD_SEPARATOR(filePtr);

  // percentage of total uses
    double percent1,percent2,percent3,percent4;
    percent1=finance.projected_total_expense.this_year.faculty/finance.projected_total_expense.this_year.total;
    percent2=finance.projected_total_expense.this_year.staff/finance.projected_total_expense.this_year.total;
    percent3=finance.projected_total_expense.this_year.other/finance.projected_total_expense.this_year.total;
    percent4=1.0;

    WRITE_STR_FIELD(filePtr, "    Percent of total uses" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent1); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent2); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent3); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent4); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_RECORD_SEPARATOR(filePtr);                // blank on % uses field

    // surplus
    WRITE_STR_FIELD(filePtr, "  Surplus (deficit)"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // AY faculty compensation field
    WRITE_FIELD_SEPARATOR(filePtr);                 // staff compensation field
    WRITE_FIELD_SEPARATOR(filePtr);                 // other expenditure field
    WRITE_NUM_FIELD(filePtr, finance.this_year.surplus_deficit); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_RECORD_SEPARATOR(filePtr);                // blank on % uses field
}

//------ End of static function export_fin_rep_case2 --------//

//------ Begin of static function fin_rep_case3 --------//
// Case 3: RENVENUE	and NEXT YEAR
// recNo -- record no. (row no.) to be displayed
// x,y	-- left top position of the display region
//
static void fin_rep_case3(int recNo,int x, int y) {
    int i;
    //	int	xColumn1=x+305, xColumn2=440, xColumn3=510, xColumn4=590, xColumn5=680;
    int xColumn1=370, xColumn2=470, xColumn3=570, xColumn4=670, xColumn5=740;

    // Show title //
    if(recNo==1) {
	// fix in version 2
	// int tyear=(int)(info.game_year+(info.game_month-9.0)/12);
	int tyear=info.financial_year();

	String str;
	str="Next Year : Year ";
	//			str+=m.format(tyear+1,16);     //## chea 061199 2.5.1
	str+=m.format(tyear+1,4);

	//			str+="-";
	//			str+=m.format(tyear+2,16);     //## chea 061199 2.5.1

	font_charts.put(xColumn1-70,y+2,str.str_buf);
	return;
    }
    recNo--;

    if(recNo==1) {
	font_chart_red_sm.disp( x, y, "Revenue", x+200 );
	font_chart_red_sm.right_put( xColumn2, y+2, "Direct");
	font_chart_red_sm.right_put( xColumn3, y+2, "Indirect");
	font_chart_red_sm.right_put( xColumn4, y+2, "Total");
	font_chart_red_sm.right_put( xColumn5, y+2, "% sources");
    }
    else if((recNo>1) && (recNo<=REVENUE_ITEM_COUNT+1)) {
	i=recNo-2;
	if(recNo==5) {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn1);
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].next_year.direct, 2));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_revenue_array[i].next_year.indirect, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].next_year.total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].next_year.percent, 24));
	}
	if((recNo==2)||(recNo==3)) {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn2);
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].next_year.total, 2));
	}
	else {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn2);
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].next_year.total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].next_year.percent, 24));
	}
    }
    else if(recNo==REVENUE_ITEM_COUNT+2) {
	font_chart_blue_sm.disp( x, y,"  Total sources of funds", xColumn1);
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_revenue.next_year.total, 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.projected_total_revenue.next_year.percent, 24));
    }
    else if(recNo==REVENUE_ITEM_COUNT+3) {
	font_chart_red_sm.right_put( xColumn1, y+2, "AY faculty");
	font_chart_red_sm.right_put( xColumn2, y+2, "Staff");
	font_chart_red_sm.right_put( xColumn3, y+2, "Other");
    }
    else if(recNo==REVENUE_ITEM_COUNT+4) {
	font_chart_red_sm.disp( x, y, "Expenditures", x+200 );
	font_chart_red_sm.right_put( xColumn1, y+2, "compensation");
	font_chart_red_sm.right_put( xColumn2, y+2, "compensation");
	font_chart_red_sm.right_put( xColumn3, y+2, "expenditures");
	font_chart_red_sm.right_put( xColumn4, y+2, "Total");
	font_chart_red_sm.right_put( xColumn5, y+2, "% uses");
    }
    else if((recNo>=REVENUE_ITEM_COUNT+5) && (recNo<REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3)) {
	i=recNo-5-REVENUE_ITEM_COUNT;
	if((recNo>=REVENUE_ITEM_COUNT+7) && (recNo<REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3)) {
	    font_chartsm.disp( x, y,game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),xColumn1);
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[i].next_year.staff, 2));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[i].next_year.other, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].next_year.total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[i].next_year.percent, 24));
	}
	else {
	    font_chartsm.disp( x, y,game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.projected_expense_array[i].next_year.faculty, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[i].next_year.staff, 2));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[i].next_year.other, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].next_year.total, 2));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[i].next_year.percent, 24));
	}
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3) {
	font_chart_blue_sm.disp( x, y,"  Total operating expenditures", xColumn1);
	font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.projected_total_operating_expense.next_year.faculty, 2));
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_operating_expense.next_year.staff, 2));
	font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.projected_total_operating_expense.next_year.other, 2));
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_operating_expense.next_year.total, 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.projected_total_operating_expense.next_year.percent, 24));
    }
    else if((recNo>REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+3) && (recNo<=REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+5)) {
	i=recNo-6-REVENUE_ITEM_COUNT;
	font_chartsm.disp( x, y,game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),xColumn1);
	font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[i].next_year.other, 2));
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].next_year.total, 2));
	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[i].next_year.percent, 24));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+6) {
	font_chart_blue_sm.disp( x, y,"  Total uses of funds", xColumn1);
	font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.projected_total_expense.next_year.faculty, 2));
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_expense.next_year.staff, 2));
	font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.projected_total_expense.next_year.other, 2));
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_expense.next_year.total, 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.projected_total_expense.next_year.percent, 24));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+7) {
	double percent1,percent2,percent3,percent4;
	// fix in version 2
	//percent1=100*finance.total_expense.faculty/finance.total_expense.total;
	//percent2=100*finance.total_expense.staff/finance.total_expense.total;
	//percent3=100*finance.total_expense.other/finance.total_expense.total;
	percent1=100*finance.projected_total_expense.next_year.faculty/finance.projected_total_expense.next_year.total;
	percent2=100*finance.projected_total_expense.next_year.staff/finance.projected_total_expense.next_year.total;
	percent3=100*finance.projected_total_expense.next_year.other/finance.projected_total_expense.next_year.total;
	percent4=100;

	font_chartsm.disp( x, y, "\tPercent of total uses",  xColumn1);
	font_chartsm.right_put( xColumn1, y+2, m.format(percent1, 24));
	font_chartsm.right_put( xColumn2, y+2, m.format(percent2, 24));
	font_chartsm.right_put( xColumn3, y+2, m.format(percent3, 24));
	font_chartsm.right_put( xColumn4, y+2, m.format(percent4, 24));
	//			font_chartsm.disp( xColumn5, y+2, "% uses", xColumn6 );
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+8) {
	font_chartsm.disp( x, y,"  Surplus (deficit)", xColumn1);
	if(finance.next_year.surplus_deficit>0)
	    font_chart_green_sm.right_put( xColumn4, y+2, m.format(finance.next_year.surplus_deficit, 2));
	else
	    font_chart_red_sm.right_put( xColumn4, y+2, m.format(finance.next_year.surplus_deficit, 2) );
    }
}

//------ End of static function fin_rep_case3 --------//

//------ Begin of static function export_fin_rep_case3 --------//
//
// write fin_rep_case3 to a text file
//
void export_fin_rep_case3( File *filePtr ) {
    // year no
    int tyear = info.financial_year();
    WRITE_STR_FIELD(filePtr, "Next Year : " ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, tyear+1); WRITE_RECORD_SEPARATOR(filePtr);
    // revenue label
    WRITE_STR_FIELD(filePtr, "Revenue"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank field
    WRITE_STR_FIELD(filePtr, "Direct"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Indirect"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Total"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "% sources"); WRITE_RECORD_SEPARATOR(filePtr);

    // revenue
    for( int i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD(filePtr, game_str_res.financial_report_str(i)); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_FIELD_SEPARATOR(filePtr);               // blank field
	// only AC_SPONSORED_RESEARCH_REVENUE display direct and indirect
	if( i == AC_SPONSORED_RESEARCH_REVENUE ) {
	    WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].next_year.direct); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].next_year.indirect); WRITE_FIELD_SEPARATOR(filePtr);
	}
	else {
	    WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_FIELD_SEPARATOR(filePtr);
	}
	WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].next_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	// don't show % sources AC_GROSS_TUITION_INCOME,AC_FINANCIAL_AID
	if( i==AC_GROSS_TUITION_INCOME||i==AC_FINANCIAL_AID ) {
	    // empty
	}
	else {
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_revenue_array[i].next_year.percent);
	}
	WRITE_RECORD_SEPARATOR(filePtr);
    }
    // total revenue
    WRITE_STR_FIELD(filePtr, "  Total sources of funds"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank field
    WRITE_FIELD_SEPARATOR(filePtr);                 // direct field
    WRITE_FIELD_SEPARATOR(filePtr);                 // indirect field
    WRITE_NUM_FIELD(filePtr, finance.projected_total_revenue.next_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_revenue.next_year.percent); WRITE_RECORD_SEPARATOR(filePtr);

  // expenditure label
    WRITE_STR_FIELD(filePtr, "Expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "AY faculty compensation"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Staff compensation"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Other expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Total"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "% uses"); WRITE_RECORD_SEPARATOR(filePtr);

  // expenditure item
    for( int i=0; i < EXPENSE_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD(filePtr, game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT)); WRITE_FIELD_SEPARATOR(filePtr);
	if( i==AC_ACADEMIC_DEPARTMENTS || i==AC_SPONSORED_RESEARCH_EXPENSE ) {
	    WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].next_year.faculty);
	}
	WRITE_FIELD_SEPARATOR(filePtr);
	if( i < OPERATING_EXPENSE_ITEM_COUNT ) {
	    WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].next_year.staff);
	}
	WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].next_year.other); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].next_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].next_year.percent); WRITE_RECORD_SEPARATOR(filePtr);

	if( i==OPERATING_EXPENSE_ITEM_COUNT-1 ) {
	    // total operatring expenditure
	    WRITE_STR_FIELD(filePtr, "  Total operating expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.next_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.next_year.staff); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.next_year.other); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.next_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.next_year.percent); WRITE_RECORD_SEPARATOR(filePtr);
	}
    }

    // total use of fund
    WRITE_STR_FIELD(filePtr, "  Total uses of funds" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.next_year.faculty ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.next_year.staff); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.next_year.other); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.next_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.next_year.percent); WRITE_RECORD_SEPARATOR(filePtr);

  // percentage of total uses
    double percent1,percent2,percent3,percent4;
    percent1=finance.projected_total_expense.next_year.faculty/finance.projected_total_expense.next_year.total;
    percent2=finance.projected_total_expense.next_year.staff/finance.projected_total_expense.next_year.total;
    percent3=finance.projected_total_expense.next_year.other/finance.projected_total_expense.next_year.total;
    percent4=1.0;

    WRITE_STR_FIELD(filePtr, "    Percent of total uses" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent1); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent2); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent3); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent4); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_RECORD_SEPARATOR(filePtr);                // blank on % uses field

    // surplus
    WRITE_STR_FIELD(filePtr, "  Surplus (deficit)"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // AY faculty compensation field
    WRITE_FIELD_SEPARATOR(filePtr);                 // staff compensation field
    WRITE_FIELD_SEPARATOR(filePtr);                 // other expenditure field
    WRITE_NUM_FIELD(filePtr, finance.next_year.surplus_deficit); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_RECORD_SEPARATOR(filePtr);                // blank on % uses field
}

//------ End of static function export_fin_rep_case3 --------//

//------ Begin of static function fin_rep_case4 --------//
// Case 4: RENVENUE	and ACTUAL AND BUDGET
// recNo -- record no. (row no.) to be displayed
// x,y	-- left top position of the display region
//
static void fin_rep_case4(int recNo,int x, int y) {
    int i;
    int xColumn1=390, xColumn2=460, xColumn3=560, xColumn4=630, xColumn5=730, xColumn6=745;

    // ##### Begin Marco ##### //
    int xColumn0 = 300;
    // ##### End Marco ##### //

    // Show title //

    if(recNo==1) {
	font_charts.disp( xColumn1-40, y+2, "Budget vs Actual", xColumn4+140 );
	return;
    }
    recNo--;

    if(recNo==1) {
	font_chart_red_sm.disp( xColumn2-40, y+2, "Change", xColumn3 );
	font_chart_red_sm.disp( xColumn4-40, y+2, "Change", xColumn5 );
	int tyear=(int)(info.game_year+((float)info.game_month-9.0)/12);

	String s1, s2 ,s3;

	s1 += m.format(tyear-1,16);
	s1 += " actual";
	font_chart_red_sm.disp( xColumn1-60, y+2, s1.str_buf, xColumn2-10);

	s2 += m.format(tyear,16);
	s2 += " budget";
	font_chart_red_sm.disp( xColumn3-65, y+2, s2.str_buf, xColumn4-10 );

	s3 = m.format(tyear,16);
	s3 += " actual";
	font_chart_red_sm.disp( xColumn5-60, y+2, s3.str_buf, xColumn6 );

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.disp(xColumn0-30, y+2, "Initial", xColumn1-10);
	// ##### End Marco ##### //
    }

    if(recNo==2) {
	font_chart_red_sm.disp( x, y, "Revenue", x+200 );
    }

    //	else if((recNo>2) && (recNo<=REVENUE_ITEM_COUNT+2))
    // Marco
    else if((recNo>2) && (recNo<=REVENUE_ITEM_COUNT)) {
	i=recNo-3;

	font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn1);

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_revenue_array[i].total, 2));
	// ##### End Marco ##### //

	font_chartsm.right_put( xColumn1, y+2, m.format(finance.revenue_array[i].total, 2));
	//		font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].change_budget_year.total, 3));
	//## chea 250899
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].change_budget_year.total, 24));

	font_chartsm.right_put( xColumn3, y+2, m.format(finance.budget_revenue_array[i].total, 2));
	//			font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].change_last_year.total, 3));
	//## chea 250899
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].change_last_year.total, 24));

	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].this_year.total, 2));

    }
    // ##### Begin Marco  ##### //
    else if((recNo==REVENUE_ITEM_COUNT+1)) {        // Marco
	i=recNo-3;

	font_chartsm.disp( x, y, "  Interest earned or paid on",  xColumn1);

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_revenue_array[i].total, 2));
	// ##### End Marco ##### //

	font_chartsm.right_put( xColumn1, y+2, m.format(finance.revenue_array[i].total, 2));
	//		font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].change_budget_year.total, 3));
	//## chea 250899
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].change_budget_year.total, 24));

	font_chartsm.right_put( xColumn3, y+2, m.format(finance.budget_revenue_array[i].total, 2));
	//			font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].change_last_year.total, 3));
	//## chea 250899
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].change_last_year.total, 24));

	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].this_year.total, 2));

    }
    else if((recNo==REVENUE_ITEM_COUNT+2)) {        // Marco
	font_chartsm.disp( x + 30, y, "operating reserve",  xColumn1);
    }
    else if(recNo==REVENUE_ITEM_COUNT+3) {
	i=recNo-4;

	font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn1);

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_revenue_array[i].total, 2));
	// ##### End Marco ##### //

	font_chartsm.right_put( xColumn1, y+2, m.format(finance.revenue_array[i].total, 2));
	//		font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].change_budget_year.total, 3));
	//## chea 250899
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].change_budget_year.total, 24));

	font_chartsm.right_put( xColumn3, y+2, m.format(finance.budget_revenue_array[i].total, 2));
	//			font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].change_last_year.total, 3));
	//## chea 250899
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].change_last_year.total, 24));

	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].this_year.total, 2));

    }
    // #### End Marco ##### //
    //	else if(recNo==REVENUE_ITEM_COUNT+3)
    else if(recNo==REVENUE_ITEM_COUNT+4) {          //Marco
	font_chart_blue_sm.disp( x, y, "  Total sources of funds",  xColumn1);

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    //			font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_revenue.total, 2));
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_revenue_total, 2));
	// ##### End Marco ##### //

	font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.total_revenue.total, 2));
	//			font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_revenue.change_budget_year.total, 3));
	//## chea 250899
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_revenue.change_budget_year.total, 24));

	font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.budget_total_revenue.total, 2));
	//			font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_revenue.change_last_year.total, 3));
	//## chea 250899
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_revenue.change_last_year.total, 24));

	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.projected_total_revenue.this_year.total, 2));
    }

    // ##### End Marco ##### //

    else if(recNo==REVENUE_ITEM_COUNT+5) {
	font_chart_red_sm.disp( x, y, "Expenditures", x+200 );
    }
    else if((recNo>=REVENUE_ITEM_COUNT+6) && (recNo<REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+4)) {
	i=recNo-6-REVENUE_ITEM_COUNT;
	font_chartsm.disp( x, y, game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),  xColumn1);

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[i].total, 2));
	// ##### End Marco ##### //

	font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[i].total, 2));
	//			font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[i].change_budget_year.total, 3));
	//## chea 250899
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[i].change_budget_year.total, 24));

	font_chartsm.right_put( xColumn3, y+2, m.format(finance.budget_expense_array[i].total, 2));
	//			font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].change_last_year.total, 3));  //## chea 250899
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].change_last_year.total, 24));

	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[i].this_year.total, 2));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+4) {
	font_chart_blue_sm.disp( x, y,"  Total operating expenditures", xColumn1);

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_operating_expense.total, 2));
	// ##### End Marco ##### //

	font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.total_operating_expense.total, 2));
	//			font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_operating_expense.change_budget_year.total, 3));
	//## chea 250899
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_operating_expense.change_budget_year.total, 24));

	font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.budget_total_operating_expense.total, 2));
	//			font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_operating_expense.change_last_year.total, 3));
	//## chea 250899
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_operating_expense.change_last_year.total, 24));

	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.projected_total_operating_expense.this_year.total, 2));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+5) {
	i=recNo-7-REVENUE_ITEM_COUNT;
	font_chartsm.disp( x, y, game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),  xColumn1);

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[i].total, 2));
	// ##### End Marco ##### //

	font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[i].total, 2));
	//			font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[i].change_budget_year.total, 3));
	//## chea 250899
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[i].change_budget_year.total, 24));

	font_chartsm.right_put( xColumn3, y+2, m.format(finance.budget_expense_array[i].total, 2));
	//			font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].change_last_year.total, 3));
	//## chea 250899
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].change_last_year.total, 24));

	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[i].this_year.total, 2));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+6) {
	i=recNo-7-REVENUE_ITEM_COUNT;
	font_chartsm.disp( x, y, game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT),  xColumn1);

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[i].total, 2));
	// ##### End Marco ##### //

	font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[i].total, 2));
	//			font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[i].change_budget_year.total, 3));
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[i].change_budget_year.total, 24));

	font_chartsm.right_put( xColumn3, y+2, m.format(finance.budget_expense_array[i].total, 2));
	//			font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].change_last_year.total, 3));
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[i].change_last_year.total, 24));

	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[i].this_year.total, 2));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+7) {
	font_chart_blue_sm.disp( x, y,"  Total uses of funds", xColumn1);

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_expense.total, 2));
	// ##### End Marco ##### //

	font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.total_expense.total, 2));
	//			font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_expense.change_budget_year.total, 3));
	//## chea 250899
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_expense.change_budget_year.total, 24));

	font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.budget_total_expense.total, 2));
	//			font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_expense.change_last_year.total, 3));
	//## chea 250899
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_expense.change_last_year.total, 24));

	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.projected_total_expense.this_year.total, 2));
    }
    else if(recNo==REVENUE_ITEM_COUNT+EXPENSE_ITEM_COUNT+8) {
	font_chartsm.disp( x, y,"  Surplus (deficit)", xColumn2);

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_last_year.surplus_deficit, 2));
	// ##### End Marco ##### //

	if(finance.last_year.surplus_deficit>0)
	    font_chart_green_sm.right_put( xColumn1, y+2, m.format(finance.last_year.surplus_deficit, 2));
	else
	    font_chart_red_sm.right_put( xColumn1, y+2, m.format(finance.last_year.surplus_deficit, 2));

	if(finance.budget_year.surplus_deficit>0)
	    font_chart_green_sm.right_put( xColumn3, y+2, m.format(finance.budget_year.surplus_deficit, 2));
	else
	    font_chart_red_sm.right_put( xColumn3, y+2, m.format(finance.budget_year.surplus_deficit, 2));

	if(finance.this_year.surplus_deficit>0)
	    font_chart_green_sm.right_put( xColumn5, y+2, m.format(finance.this_year.surplus_deficit, 2));
	else
	    font_chart_red_sm.right_put( xColumn5, y+2, m.format(finance.this_year.surplus_deficit, 2));
    }
    //## fred 990519
}

//------ End of static function fin_rep_case4 --------//

//------ Begin of static function export_fin_rep_case4 --------//
//
// write fin_rep_case4 to a text file
//
void export_fin_rep_case4( File *filePtr ) {
    // year no
    int tyear = info.financial_year();
    WRITE_STR_FIELD(filePtr, "Budget vs Actual" ); WRITE_RECORD_SEPARATOR(filePtr);
    // write year label
    WRITE_FIELD_SEPARATOR(filePtr);                 // item revenue/expenditure item field
    WRITE_STR_FIELD(filePtr, "Initial"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, m.format(tyear-1, 16)); WRITE_STR_FIELD(filePtr, " actual"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Change"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, m.format(tyear, 16)); WRITE_STR_FIELD(filePtr, " budget"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, "Change"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_STR_FIELD(filePtr, m.format(tyear, 16)); WRITE_STR_FIELD(filePtr, " actual"); WRITE_RECORD_SEPARATOR(filePtr);
    // write revenue label
    WRITE_STR_FIELD(filePtr, "Revenue"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank initial
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank last year actual
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank change
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank this year budget
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank change
    WRITE_RECORD_SEPARATOR(filePtr);                // blank this year actual

    // revenue
    for( int i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD(filePtr, game_str_res.financial_report_str(i)); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.initial_revenue_array[i].total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.revenue_array[i].total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_revenue_array[i].change_budget_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.budget_revenue_array[i].total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_revenue_array[i].change_last_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].this_year.total); WRITE_RECORD_SEPARATOR(filePtr);
    }
    // total revenue
    WRITE_STR_FIELD(filePtr, "  Total sources of funds"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.initial_total_revenue_total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_revenue.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_revenue.change_budget_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.budget_total_revenue.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_revenue.change_last_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_revenue.this_year.total); WRITE_RECORD_SEPARATOR(filePtr);

    // expenditure label
    WRITE_STR_FIELD(filePtr, "Expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank initial
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank last year actual
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank change
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank this year budget
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank change
    WRITE_RECORD_SEPARATOR(filePtr);                // blank this year actual

    // expenditure item
    for( int i=0; i < EXPENSE_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD(filePtr, game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT)); WRITE_FIELD_SEPARATOR(filePtr);

	WRITE_NUM_FIELD(filePtr, finance.initial_expense_array[i].total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.expense_array[i].total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].change_budget_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.budget_expense_array[i].total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].change_last_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].this_year.total); WRITE_RECORD_SEPARATOR(filePtr);

	if( i==OPERATING_EXPENSE_ITEM_COUNT-1 ) {
	    // total operatring expenditure
	    WRITE_STR_FIELD(filePtr, "  Total operating expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.initial_total_operating_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.total_operating_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.change_budget_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.budget_total_operating_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.change_last_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.this_year.total); WRITE_RECORD_SEPARATOR(filePtr);
	}
    }

    // total use of fund
    WRITE_STR_FIELD(filePtr, "  Total uses of funds" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.initial_total_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.change_budget_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.budget_total_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.change_last_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.this_year.total); WRITE_RECORD_SEPARATOR(filePtr);

    // surplus
    WRITE_STR_FIELD(filePtr, "  Surplus (deficit)"); WRITE_FIELD_SEPARATOR(filePtr);
    // blank initial
    WRITE_NUM_FIELD(filePtr, finance.initial_last_year.surplus_deficit); WRITE_FIELD_SEPARATOR(filePtr);
    // blank last year actual
    WRITE_NUM_FIELD(filePtr, finance.last_year.surplus_deficit); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank change
    // blank this year budget
    WRITE_NUM_FIELD(filePtr, finance.budget_year.surplus_deficit); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);                 // blank change
    // blank this year actual
    WRITE_NUM_FIELD(filePtr, finance.this_year.surplus_deficit); WRITE_RECORD_SEPARATOR(filePtr);
}

//------ End of static function export_fin_rep_case4 --------//

//------ Begin of static function fin_rep_case5 --------//
// Case 5: BALANCE SHEET && LAST YEAR
// recNo -- record no. (row no.) to be displayed
// x,y	-- left top position of the display region
//
static void fin_rep_case5(int recNo,int x, int y) {
    int i;
    int xColumn1=x+305, xColumn2=440, xColumn3=510, xColumn4=590, xColumn5=680;
    //		if(recNo==1)
    //		{
    //			font_chart_blue_sm.disp( BS_TAB1, y, "Balance sheet", BS_TAB1+200 );
    //		}

    // Show title //
    if(recNo==1) {
	String str;
	// fix in version 2
	// int tyear=(int)(info.game_year+(info.game_month-9.0)/12);
	int tyear=info.financial_year();
	///  BUGHERE ## chwg0301
	if(sys.view_mode==MODE_YEAR_END_REPORT)tyear++;

	str="Previous Year : Year ";
	str+=m.format(tyear-1,4);

	font_charts.put( xColumn1-50, y+2, str.str_buf);
	return;
    }
    recNo--;

    // Shift the page a row upper.
    recNo++;
    if(recNo==2) {
	font_chart_red_sm.disp( BS_TAB1, y+2, "Assets", BS_TAB1+200 );
    }
    else if((recNo>2)&&(recNo<=ASSET_ITEM_COUNT+2)) {
	i=recNo-3;
	if ( i == 3 )
	    i = 5;
	else
	    if ( i>3 && i<6 )
		i -= 1;
	font_chartsm.disp( BS_TAB1, y+2, game_str_res.balance_sheet_str(i),  BS_TAB2);
	font_chartsm.right_put( BS_TAB2, y+2, m.format(finance.last_year.asset_array[i], 2));
    }
    else if(recNo==ASSET_ITEM_COUNT+4) {
	font_chart_red_sm.disp( BS_TAB1, y+2, "Liabilities", BS_TAB1+200 );
    }
    else if((recNo>=ASSET_ITEM_COUNT+5)&&(recNo<ASSET_ITEM_COUNT+5+LIABILITY_ITEM_COUNT)) {
	i=recNo-5-ASSET_ITEM_COUNT;
	font_chartsm.disp( BS_TAB1, y+2, game_str_res.balance_sheet_str(i+ASSET_ITEM_COUNT),  BS_TAB2);
	font_chartsm.right_put( BS_TAB2, y+2, m.format(finance.last_year.liability_array[i], 2));
    }
    else if(recNo==ASSET_ITEM_COUNT+LIABILITY_ITEM_COUNT+6) {
	font_chart_blue_sm.disp( BS_TAB1, y+2, "Fund balances", BS_TAB1+200 );
	font_chart_blue_sm.right_put( BS_TAB2, y+2,
				      m.format(finance.last_year.asset_array[ASSET_ITEM_COUNT-1]-
					       finance.last_year.liability_array[LIABILITY_ITEM_COUNT-1], 2));

    }
    else if(recNo==ASSET_ITEM_COUNT+LIABILITY_ITEM_COUNT+7) {
	font_chart_blue_sm.disp( BS_TAB1, y+2, "Total liabilities and fund balance", BS_TAB1+220);
	font_chart_blue_sm.right_put( BS_TAB2, y+2,
				      m.format(finance.last_year.asset_array[ASSET_ITEM_COUNT-1], 2));
    }
}

//------ End of static function fin_rep_case5 --------//

//------ Begin of static function fin_rep_case6 --------//
// Case 6: BALANCE SHEET && THIS YEAR
// recNo -- record no. (row no.) to be displayed
// x,y	-- left top position of the display region
//
static void fin_rep_case6(int recNo,int x, int y) {
    int i;
    int xColumn1=x+305, xColumn2=440, xColumn3=510, xColumn4=590, xColumn5=680;

    // Show title //
    if(recNo==1) {
	String str;
	// fix in version 2
	// int tyear=(int)(info.game_year+(info.game_month-9.0)/12);
	int tyear=info.financial_year();
	str="Present Year : Year ";
	//			str+=m.format(tyear,16);   //## chea 061199
	str+=m.format(tyear,4);

	//			str+="-";
	//			str+=m.format(tyear+1,16); //## chea 061199

	font_charts.put( xColumn1-50, y+2, str.str_buf);
	return;
    }
    if(recNo==2) {
	//
	font_chart_red_sm.disp( BS_TAB1, y+2, "Assets", BS_TAB1+200 );
    }
    else if((recNo>2)&&(recNo<=ASSET_ITEM_COUNT+2)) {
	i=recNo-3;
	if ( i == 3 )
	    i = 5;
	else
	    if ( i>3 && i<6 )
		i -= 1;
	font_chartsm.disp( BS_TAB1, y+2, game_str_res.balance_sheet_str(i),  BS_TAB2);
	//
	font_chartsm.right_put( BS_TAB2, y+2, m.format(finance.this_year.asset_array[i], 2));
    }
    else if(recNo==ASSET_ITEM_COUNT+4) {
	//
	font_chart_red_sm.disp( BS_TAB1, y+2, "Liabilities", BS_TAB1+200 );
    }
    else if((recNo>=ASSET_ITEM_COUNT+5)&&(recNo<ASSET_ITEM_COUNT+5+LIABILITY_ITEM_COUNT)) {
	i=recNo-5-ASSET_ITEM_COUNT;
	font_chartsm.disp( BS_TAB1, y+2, game_str_res.balance_sheet_str(i+ASSET_ITEM_COUNT),  BS_TAB2);
	font_chartsm.right_put( BS_TAB2, y+2, m.format(finance.this_year.liability_array[i], 2));
    }
    else if(recNo==ASSET_ITEM_COUNT+LIABILITY_ITEM_COUNT+6) {
	font_chart_blue_sm.disp( BS_TAB1, y+2, "Fund balances", BS_TAB1+200 );
	font_chart_blue_sm.right_put( BS_TAB2, y+2, m.format(finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-
							     finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1], 2));

    }
    else if(recNo==ASSET_ITEM_COUNT+LIABILITY_ITEM_COUNT+7) {
	font_chart_blue_sm.disp( BS_TAB1, y+2, "Total liabilities and fund balance", BS_TAB1+220);
	font_chart_blue_sm.right_put( BS_TAB2, y+2, m.format(finance.this_year.asset_array[ASSET_ITEM_COUNT-1], 2));
    }
}

//------ End of static function fin_rep_case6 --------//

//------ Begin of static function fin_rep_case7 --------//
// Case 7: BALANCE SHEET && NEXT YEAR
// recNo -- record no. (row no.) to be displayed
// x,y	-- left top position of the display region
//
static void fin_rep_case7(int recNo,int x, int y) {
    int i;
    int xColumn1=x+305, xColumn2=440, xColumn3=510, xColumn4=590, xColumn5=680;

    // Show title //
    if(recNo==1) {
	String str;
	// fix in version2
	// int tyear=(int)(info.game_year+(info.game_month-9.0)/12);
	int tyear=info.financial_year();
	str="Next Year : Year ";
	//			str+=m.format(tyear+1,16);  //## chea 061199
	str+=m.format(tyear+1,4);

	//			str+="-";
	//			str+=m.format(tyear+2,16);  //## chea 061199

	font_charts.put( xColumn1-50, y+2, str.str_buf);
	return;
    }
    if(recNo==2) {
	font_chart_red_sm.disp( BS_TAB1, y+2, "Assets", BS_TAB1+200 );
    }
    else if((recNo>2)&&(recNo<=ASSET_ITEM_COUNT+2)) {
	i=recNo-3;
	if ( i == 3 )
	    i = 5;
	else
	    if ( i > 3 && i < 6 )
		i -= 1;
	font_chartsm.disp( BS_TAB1, y+2, game_str_res.balance_sheet_str(i),  BS_TAB2);
	font_chartsm.right_put( BS_TAB2, y+2, m.format(finance.next_year.asset_array[i], 2));
    }
    else if(recNo==ASSET_ITEM_COUNT+4) {
	font_chart_red_sm.disp( BS_TAB1, y+2, "Liabilities", BS_TAB1+200 );
    }
    else if((recNo>=ASSET_ITEM_COUNT+5)&&(recNo<ASSET_ITEM_COUNT+5+LIABILITY_ITEM_COUNT)) {
	i=recNo-5-ASSET_ITEM_COUNT;
	font_chartsm.disp( BS_TAB1, y+2, game_str_res.balance_sheet_str(i+ASSET_ITEM_COUNT),  BS_TAB2);
	font_chartsm.right_put( BS_TAB2, y+2, m.format(finance.next_year.liability_array[i], 2));
    }
    else if(recNo==ASSET_ITEM_COUNT+LIABILITY_ITEM_COUNT+6) {
	font_chart_blue_sm.disp( BS_TAB1, y+2, "Fund balances", BS_TAB1+200 );
	font_chart_blue_sm.right_put( BS_TAB2, y+2, m.format(finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-
							     finance.next_year.liability_array[LIABILITY_ITEM_COUNT-1], 2));

    }
    else if(recNo==ASSET_ITEM_COUNT+LIABILITY_ITEM_COUNT+7) {
	font_chart_blue_sm.disp( BS_TAB1, y+2, "Total liabilities and fund balance", BS_TAB1+220 );
	font_chart_blue_sm.right_put( BS_TAB2, y+2, m.format(finance.next_year.asset_array[ASSET_ITEM_COUNT-1], 2));
    }
}

//------ End of static function fin_rep_case7 --------//

//------ Begin of static function fin_rep_case8 --------//
// Case 8: BALANCE SHEET && THREE YEAR
// recNo -- record no. (row no.) to be displayed
// x,y	-- left top position of the display region
//
static void fin_rep_case8(int recNo,int x, int y) {
    int i;
    int xColumn1=395, xColumn2=465, xColumn3=545, xColumn4=615, xColumn5=695,xColumn6=745;

    // ##### Begin MArco ##### //
    int xColumn0 = 315;
    // ##### End Marco ##### //

    // Show title //
    if(recNo==1) {
	font_charts.disp( xColumn1-60, y+2, "Three-Year Balance Sheets", xColumn4+90 );
	return;
    }
    //		recNo--;

    if(recNo==2) {
	font_chart_blue_sm.disp( x, y, "", x+200 );
	// fix in version 2
	// int tyear=(int)(info.game_year+(info.game_month-9.0)/12);
	int tyear=info.financial_year();
	/*
          String s1="Yr. ";             //## chea 061199
          s1+=m.format(tyear-1,16);
          s1+="-";
          s1+=m.format(tyear,16);
          font_chart_red_sm.disp( xColumn1-50, y+2, s1.str_buf, xColumn1+60 );

          s1="Yr. ";                    //## chea 061199
          s1+=m.format(tyear,16);
          s1+="-";
          s1+=m.format(tyear+1,16);
	  font_chart_red_sm.disp( xColumn3-50, y+2, s1.str_buf, xColumn3+60 );

	  s1="Yr. ";                    //## chea 061199
	  s1+=m.format(tyear+1,16);
	  s1+="-";
	  s1+=m.format(tyear+2,16);
	  font_chart_red_sm.disp( xColumn5-50, y+2, s1.str_buf, xColumn6 );
	*/

	String s1,s2,s3;                              //## chea 061199 2.5.1
	s1+=m.format(tyear-1,16);
	//font_chart_red_sm.disp( xColumn1-35, y+2, s1.str_buf, xColumn1+60 );
	font_chart_red_sm.disp( xColumn0-35, y+2, s1.str_buf, xColumn0+60 );

	s2+=m.format(tyear,16);
	//font_chart_red_sm.disp( xColumn3-35, y+2, s2.str_buf, xColumn3+60 );
	font_chart_red_sm.disp( xColumn2-35, y+2, s2.str_buf, xColumn2+60 );

	s3+=m.format(tyear+1,16);
	font_chart_red_sm.disp( xColumn5-35, y+2, s3.str_buf, xColumn6 );

	// ##### Begin Marco ##### //
	font_chart_purple_sm.disp( xColumn3-35, y+2, "Initial", xColumn4+60 );
	// #### End Marco #####//

    }
    else if(recNo==3) {
	font_chart_red_sm.disp( x, y+2, "Assets", x+200 );

	// ##### Begin Marco ##### //
	font_chart_red_sm.disp( xColumn2-45, y+2, "(Current)", xColumn2+60 );
	// ##### End Marco ##### //
    }
    else if((recNo>3)&&(recNo<=ASSET_ITEM_COUNT+3)) {
	i=recNo-4;
	if ( i == 3 )                                 // should display value of quasi endowment
	    i = 5;
	else
	    if ( i > 3 && i < 6 )
		i -= 1;
	font_chartsm.disp( x, y+2, game_str_res.balance_sheet_str(i),  xColumn1);
	//			font_chartsm.right_put( xColumn1, y+2, m.format(finance.last_year.asset_array[i], 2));
	//			font_chartsm.right_put( xColumn3, y+2, m.format(finance.this_year.asset_array[i], 2));
	font_chartsm.right_put( xColumn0, y+2, m.format(finance.last_year.asset_array[i], 2));
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.this_year.asset_array[i], 2));
	font_chartsm.right_put( xColumn5, y+2, m.format(finance.next_year.asset_array[i], 2));

	// ##### Begin Marco ##### //
	font_chart_purple_sm.right_put( xColumn3, y+2, m.format(finance.initial_last_year.asset_array[i], 2));
	// ##### End Marco ##### //
	/* //## chea 250899
	   font_chartsm.right_put( xColumn2, y+2,
	   m.format(100*(finance.this_year.asset_array[i]-finance.last_year.asset_array[i])
	   /finance.last_year.asset_array[i],
	   3));
	   font_chartsm.right_put( xColumn4, y+2,
	   m.format(100*(finance.next_year.asset_array[i]-finance.this_year.asset_array[i])
	   /finance.this_year.asset_array[i],
	   3));
	*/

	//			font_chartsm.right_put( xColumn2, y+2,
	font_chartsm.right_put( xColumn1, y+2,
				m.format(100*math.safe_divide((finance.this_year.asset_array[i]-finance.last_year.asset_array[i])
							      ,finance.last_year.asset_array[i]),
					 24));                                       //## chea 250899
	font_chartsm.right_put( xColumn4, y+2,
				m.format(100*math.safe_divide((finance.next_year.asset_array[i]-finance.last_year.asset_array[i])
							      ,finance.last_year.asset_array[i]),
					 24));

    }
    else if(recNo==ASSET_ITEM_COUNT+4) {
	font_chart_red_sm.disp( x, y+2, "Liabilities", x+200 );
    }
    else if((recNo>=ASSET_ITEM_COUNT+5)&&(recNo<ASSET_ITEM_COUNT+5+LIABILITY_ITEM_COUNT)) {
	i=recNo-5-ASSET_ITEM_COUNT;
	font_chartsm.disp( x, y+2, game_str_res.balance_sheet_str(i+ASSET_ITEM_COUNT),  xColumn1);
	//			font_chartsm.right_put( xColumn1, y+2, m.format(finance.last_year.liability_array[i], 2));
	//			font_chartsm.right_put( xColumn3, y+2, m.format(finance.this_year.liability_array[i], 2));
	font_chartsm.right_put( xColumn0, y+2, m.format(finance.last_year.liability_array[i], 2));
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.this_year.liability_array[i], 2));
	font_chartsm.right_put( xColumn5, y+2, m.format(finance.next_year.liability_array[i], 2));

	// ##### Begin Marco ##### //
	font_chart_purple_sm.right_put( xColumn3, y+2, m.format(finance.initial_last_year.liability_array[i], 2));
	// ##### End Marco ##### //

	/*  //## chea 250899
	    font_chartsm.right_put( xColumn2, y+2,
	    m.format(100*(finance.this_year.liability_array[i]-finance.last_year.liability_array[i])
	    /finance.last_year.liability_array[i],
	    3));
	    font_chartsm.right_put( xColumn4, y+2,
	    m.format(100*(finance.next_year.liability_array[i]-finance.this_year.liability_array[i])
	    /finance.this_year.liability_array[i],
	    3));
	*/

	//			font_chartsm.right_put( xColumn2, y+2,
	font_chartsm.right_put( xColumn1, y+2,
				m.format(100*(finance.this_year.liability_array[i]-finance.last_year.liability_array[i])
					 /finance.last_year.liability_array[i],
					 24));                                       //## chea 250899
	font_chartsm.right_put( xColumn4, y+2,
				m.format(100*(finance.next_year.liability_array[i]-finance.this_year.liability_array[i])
					 /finance.this_year.liability_array[i],
					 24));                                       //## chea 250899

    }
    else if(recNo==ASSET_ITEM_COUNT+LIABILITY_ITEM_COUNT+6) {
	font_chart_blue_sm.disp( x, y+2, "Fund balances", x+200 );
	//			font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.last_year.asset_array[ASSET_ITEM_COUNT-1]-
	font_chart_blue_sm.right_put( xColumn0, y+2, m.format(finance.last_year.asset_array[ASSET_ITEM_COUNT-1]-
							      finance.last_year.liability_array[LIABILITY_ITEM_COUNT-1], 2));
	//			font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-
							      finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1], 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-
							      finance.next_year.liability_array[LIABILITY_ITEM_COUNT-1], 2));

	// ##### Begin Marco ##### //
	font_chart_purple_sm.right_put( xColumn3, y+2, m.format(finance.initial_last_year.asset_array[ASSET_ITEM_COUNT-1]-
								finance.initial_last_year.liability_array[LIABILITY_ITEM_COUNT-1], 2));
	// ##### End Marco ##### //

	/*  //## chea 250899
	    font_chart_blue_sm.right_put( xColumn2, y+2,
            m.format(100*(
            finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1]-
            finance.last_year.asset_array[ASSET_ITEM_COUNT-1]+finance.last_year.liability_array[LIABILITY_ITEM_COUNT-1])
            /(finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1]),
	    //				finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1]-finance.last_year.liability_array[LIABILITY_ITEM_COUNT-1])
	    //				/finance.last_year.liability_array[LIABILITY_ITEM_COUNT-1],
            3));
	    font_chart_blue_sm.right_put( xColumn4, y+2,
            m.format(100*(
	    finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-finance.next_year.liability_array[LIABILITY_ITEM_COUNT-1]-
	    finance.this_year.asset_array[ASSET_ITEM_COUNT-1]+finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1])
	    /(finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-finance.next_year.liability_array[LIABILITY_ITEM_COUNT-1]),
	    //				finance.next_year.liability_array[LIABILITY_ITEM_COUNT-1]-finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1])
	    //				/finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1],
	    3));
	*/

	//			font_chart_blue_sm.right_put( xColumn2, y+2,
	font_chart_blue_sm.right_put( xColumn1, y+2,
				      m.format(100*(
					  finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1]-
					  finance.last_year.asset_array[ASSET_ITEM_COUNT-1]+finance.last_year.liability_array[LIABILITY_ITEM_COUNT-1])
					       /(finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1]),
					       24));                                       //## chea 250899
	font_chart_blue_sm.right_put( xColumn4, y+2,
				      m.format(100*(
					  finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-finance.next_year.liability_array[LIABILITY_ITEM_COUNT-1]-
					  finance.this_year.asset_array[ASSET_ITEM_COUNT-1]+finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1])
					       /(finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-finance.next_year.liability_array[LIABILITY_ITEM_COUNT-1]),
					       24));                                       //## chea 250899

    }
    else if(recNo==ASSET_ITEM_COUNT+LIABILITY_ITEM_COUNT+7) {
	font_chart_blue_sm.disp( x, y+2, "Total liabilities and fund balance", BS_TAB1+220);
	//			font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.last_year.asset_array[ASSET_ITEM_COUNT-1], 2));
	//			font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.this_year.asset_array[ASSET_ITEM_COUNT-1], 2));
	font_chart_blue_sm.right_put( xColumn0, y+2, m.format(finance.last_year.asset_array[ASSET_ITEM_COUNT-1], 2));
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.this_year.asset_array[ASSET_ITEM_COUNT-1], 2));
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.next_year.asset_array[ASSET_ITEM_COUNT-1], 2));
	//			font_chart_blue_sm.right_put( xColumn2, y+2,
	//				m.format(100*(finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-finance.last_year.asset_array[ASSET_ITEM_COUNT-1])
	//				/finance.last_year.asset_array[ASSET_ITEM_COUNT-1],
	//				3));

	// ##### Begin Marco ##### //
	font_chart_purple_sm.right_put( xColumn3, y+2, m.format(finance.initial_last_year.asset_array[ASSET_ITEM_COUNT-1], 2));
	// ##### End Marco ##### //

	//			font_chart_blue_sm.right_put( xColumn2, y+2,
	font_chart_blue_sm.right_put( xColumn1, y+2,
				      m.format(100*(finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-finance.last_year.asset_array[ASSET_ITEM_COUNT-1])
					       /finance.last_year.asset_array[ASSET_ITEM_COUNT-1],
					       24));                                       //## chea 250899

	//			font_chart_blue_sm.right_put( xColumn4, y+2,
	//				m.format(100*(finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-finance.this_year.asset_array[ASSET_ITEM_COUNT-1])
	//				/finance.this_year.asset_array[ASSET_ITEM_COUNT-1],
	//				3));

	font_chart_blue_sm.right_put( xColumn4, y+2,
				      m.format(100*(finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-finance.this_year.asset_array[ASSET_ITEM_COUNT-1])
					       /finance.this_year.asset_array[ASSET_ITEM_COUNT-1],
					       24));                                       //## chea 250899

    }
}

//------ End of static function fin_rep_case8 --------//

//------ Begin of static function export_fin_rep_case8 --------//
//
// export to text file
//
static void export_fin_rep_case8(File *filePtr) {
    int i;
    int tyear=info.financial_year();
    WRITE_STR_FIELD( filePtr, "Year" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, tyear-1); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change field
    WRITE_NUM_FIELD( filePtr, tyear); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_STR_FIELD( filePtr, "Initial"); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change field
    WRITE_NUM_FIELD( filePtr, tyear+1); WRITE_RECORD_SEPARATOR( filePtr );

    // write asset label
    WRITE_STR_FIELD( filePtr, "Assets" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_FIELD_SEPARATOR( filePtr );               // blank last year field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change field
    WRITE_STR_FIELD( filePtr, "(Current)" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_FIELD_SEPARATOR( filePtr );               // blank initial field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank next year
    WRITE_RECORD_SEPARATOR( filePtr );              // blank on value field

    // write asset
    for( i=0; i<ASSET_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD( filePtr, game_str_res.balance_sheet_str(i) ); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, finance.last_year.asset_array[i]); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, (finance.this_year.asset_array[i]-finance.last_year.asset_array[i])/finance.last_year.asset_array[i] ); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, finance.this_year.asset_array[i]); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, finance.initial_last_year.asset_array[i]); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, (finance.next_year.asset_array[i]-finance.last_year.asset_array[i])/finance.last_year.asset_array[i]); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, finance.next_year.asset_array[i]); WRITE_RECORD_SEPARATOR( filePtr );
    }

    // write liabilities label
    WRITE_STR_FIELD( filePtr, "Liabilities" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_FIELD_SEPARATOR( filePtr );               // blank last year field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank current year field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank initial field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank next year
    WRITE_RECORD_SEPARATOR( filePtr );              // blank on value field

    // write liabilities
    for( i=0; i<LIABILITY_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD( filePtr, game_str_res.balance_sheet_str(i+ASSET_ITEM_COUNT) ); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, finance.last_year.liability_array[i]); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, (finance.this_year.liability_array[i]-finance.last_year.liability_array[i])/finance.last_year.liability_array[i]); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, finance.this_year.liability_array[i]); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, finance.initial_last_year.liability_array[i], ); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, (finance.next_year.liability_array[i]-finance.this_year.liability_array[i])/finance.this_year.liability_array[i]); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_NUM_FIELD( filePtr, finance.next_year.liability_array[i]); WRITE_RECORD_SEPARATOR( filePtr );
    }

    // write fund balance
    WRITE_STR_FIELD( filePtr, "Fund balances" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, finance.last_year.asset_array[ASSET_ITEM_COUNT-1]-finance.last_year.liability_array[LIABILITY_ITEM_COUNT-1]); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, (finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1]-
			       finance.last_year.asset_array[ASSET_ITEM_COUNT-1]+finance.last_year.liability_array[LIABILITY_ITEM_COUNT-1])
		     /(finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1]) );
    WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1]); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, finance.initial_last_year.asset_array[ASSET_ITEM_COUNT-1]-finance.initial_last_year.liability_array[LIABILITY_ITEM_COUNT-1]); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, (finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-finance.next_year.liability_array[LIABILITY_ITEM_COUNT-1]-
			       finance.this_year.asset_array[ASSET_ITEM_COUNT-1]+finance.this_year.liability_array[LIABILITY_ITEM_COUNT-1])
		     /(finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-finance.next_year.liability_array[LIABILITY_ITEM_COUNT-1]) );
    WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-finance.next_year.liability_array[LIABILITY_ITEM_COUNT-1]); WRITE_RECORD_SEPARATOR( filePtr );

    // write total liabilites and fund balance
    WRITE_STR_FIELD( filePtr, "Total liabilities and fund balance" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, finance.last_year.asset_array[ASSET_ITEM_COUNT-1]); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, (finance.this_year.asset_array[ASSET_ITEM_COUNT-1]-finance.last_year.asset_array[ASSET_ITEM_COUNT-1])/finance.last_year.asset_array[ASSET_ITEM_COUNT-1] ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, finance.this_year.asset_array[ASSET_ITEM_COUNT-1]); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, finance.initial_last_year.asset_array[ASSET_ITEM_COUNT-1]); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, (finance.next_year.asset_array[ASSET_ITEM_COUNT-1]-finance.this_year.asset_array[ASSET_ITEM_COUNT-1])/finance.last_year.asset_array[ASSET_ITEM_COUNT-1]); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, finance.next_year.asset_array[ASSET_ITEM_COUNT-1]); WRITE_RECORD_SEPARATOR( filePtr );
}

//------ End of static function export_fin_rep_case8 --------//

//------ Begin of static function fin_rep_case9 --------//
// Case 9: EXPANDING THREE YEARS REVENUE
// recNo -- record no. (row no.) to be displayed
// x,y	-- left top position of the display region
//
static void fin_rep_case9(int recNo,int x, int y) {
    int i;
    int xColumn1=395, xColumn2=460, xColumn3=545, xColumn4=610, xColumn5=695,xColumn6=745;

    // ##### Begin Marco ##### //
    int xColumn0 = 315;
    // ##### End Marco ##### //

    // Show title //
    if(recNo==1) {
	font_charts.disp( xColumn1-75, y+2, "Three-Year Projection (Expanded)", xColumn4+120 );
	return;
    }

    if(recNo==2) {
	font_chart_red_sm.disp( xColumn2-35, y+2, "Change", xColumn3 );
	font_chart_red_sm.disp( xColumn4-35, y+2, "Change", xColumn5 );

	String *s1,*s2,*s3;
	int tyear=(int)(info.game_year+(info.game_month-9.0)/12);
	/*
          s1=new String(m.format(tyear-1,16));
          s2=new String(m.format(tyear,16));
          (*s1)+="-";
          (*s1)+=(*s2);
          font_chart_red_sm.disp( xColumn1-50, y+2, s1->str_buf, xColumn2-10 );

          (*s1)=String(m.format(tyear,16));
          (*s2)=String(m.format(tyear+1,16));
          (*s1)+=   "-";
          (*s1)+=(*s2);
	  font_chart_red_sm.disp( xColumn3-50, y+2, s1->str_buf, xColumn4-10 );

	  (*s1)=String(m.format(tyear+1,16));
	  (*s2)=String(m.format(tyear+2,16));
	  (*s1)+="-";
	  (*s1)+=(*s2);
	  font_chart_red_sm.disp( xColumn5-50, y+2, s1->str_buf, xColumn6 );
	  delete s1,s2;
	*/

	//## chea 061199 2.5.1
	s1=new String(m.format(tyear-1,16));
	s2=new String(m.format(tyear,16));
	s3=new String(m.format(tyear+1,16));

	font_chart_red_sm.disp( xColumn1-35, y+2, s1->str_buf, xColumn2-10 );

	font_chart_red_sm.disp( xColumn3-35, y+2, s2->str_buf, xColumn4-10 );

	font_chart_red_sm.disp( xColumn5-35, y+2, s3->str_buf, xColumn6 );

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.disp( xColumn0-35, y+2, "Initial", xColumn1-10 );
	// ##### End Marco ##### //

	delete s1,s2,s3;

    }

    else if(recNo==3) {
	font_chart_red_sm.disp( x, y, "Revenue", x+200 );
    }
    else if((recNo>=4) && (recNo<=6)) {
	i=recNo-4;
	font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn1);
	font_chartsm.right_put( xColumn1, y+2, m.format(finance.revenue_array[i].total, 2));
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].change_last_year.total, 24) );
	font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_revenue_array[i].this_year.total, 2) );
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].change_next_year.total, 24) );
	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].next_year.total, 2) );

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_revenue_array[i].total, 2));
	// ##### End Marco ##### //
    }
    else if((recNo>=7) && (recNo<=10)) {
	if(recNo==7) {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(3),  xColumn1);
	}
	else if(recNo==8) {
	    font_chart_red_sm.disp( x, y, "\tDirect",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.revenue_array[3].direct, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[3].change_last_year.direct, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_revenue_array[3].this_year.direct, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[3].change_next_year.direct, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[3].next_year.direct, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_revenue_array[3].direct, 2));
	    // ##### End Marco ##### //
	}
	else if(recNo==9) {
	    font_chart_red_sm.disp( x, y, "\tIndirect",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.revenue_array[3].indirect, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[3].change_last_year.indirect, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_revenue_array[3].this_year.indirect, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[3].change_next_year.indirect, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[3].next_year.indirect, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_revenue_array[3].indirect, 2));
	    // ##### End Marco ##### //
	}
	else if(recNo==10) {
	    font_chart_red_sm.disp( x, y, "\tTotal",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.revenue_array[3].total, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[3].change_last_year.total, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_revenue_array[3].this_year.total, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[3].change_next_year.total, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[3].next_year.total, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_revenue_array[3].total, 2));
	    // ##### End Marco ##### //
	}
    }
    //		else if((recNo>=11) && (recNo<=16))
    else if((recNo>=11) && (recNo<=14)) {
	i=recNo-7;
	font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn1);
	font_chartsm.right_put( xColumn1, y+2, m.format(finance.revenue_array[i].total, 2));
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].change_last_year.total, 24) );
	font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_revenue_array[i].this_year.total, 2) );
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].change_next_year.total, 24) );
	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].next_year.total, 2) );

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_revenue_array[i].total, 2));
	// ##### End Marco ##### //
    }
    // ##### Begin Marco ##### //
    else if (recNo == 15) {
	i=recNo-7;
	font_chartsm.disp( x, y, "  Interest earned or paid on",  xColumn1);
	font_chartsm.right_put( xColumn1, y+2, m.format(finance.revenue_array[i].total, 2));
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].change_last_year.total, 24) );
	font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_revenue_array[i].this_year.total, 2) );
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].change_next_year.total, 24) );
	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].next_year.total, 2) );

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_revenue_array[i].total, 2));
	// ##### End Marco ##### //
    }
    else if (recNo == 16) {
	font_chartsm.disp( x + 30, y, "operating reserve",  xColumn1);
    }
    else if (recNo == 17) {
	i=recNo-8;
	font_chartsm.disp( x, y, game_str_res.financial_report_str(i),  xColumn1);
	font_chartsm.right_put( xColumn1, y+2, m.format(finance.revenue_array[i].total, 2));
	font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_revenue_array[i].change_last_year.total, 24) );
	font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_revenue_array[i].this_year.total, 2) );
	font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_revenue_array[i].change_next_year.total, 24) );
	font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_revenue_array[i].next_year.total, 2) );

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_revenue_array[i].total, 2));
	// ##### End Marco ##### //
    }
    // ##### End Marco ##### //
    //		else if(recNo==17)
    else if(recNo==18) {                            //marco
	font_chart_blue_sm.disp( x, y, "  Total sources of funds",  xColumn1);
	font_chart_blue_sm.right_put( xColumn1, y+2, m.format(finance.total_revenue.total, 2));
	font_chart_blue_sm.right_put( xColumn2, y+2, m.format(finance.projected_total_revenue.change_last_year.total, 24) );
	font_chart_blue_sm.right_put( xColumn3, y+2, m.format(finance.projected_total_revenue.this_year.total, 2) );
	font_chart_blue_sm.right_put( xColumn4, y+2, m.format(finance.projected_total_revenue.change_next_year.total, 24) );
	font_chart_blue_sm.right_put( xColumn5, y+2, m.format(finance.projected_total_revenue.next_year.total, 2) );

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    //				font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_revenue.total, 2));
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_revenue_total, 2));
	// ##### End Marco ##### //
    }
    //		else if(recNo==18)
    else if(recNo==19) {                            //marco
	font_chart_red_sm.disp( x, y, "Expenditures", x+200 );
    }

    // Calculate the sub-index p,q form recNo
    // recNo = p*5 + q +19
    int p,q;

    //		p=(recNo-19)/5;
    //		q=(recNo-19)-p*5;
    p=(recNo-20)/5;                                 //marco
    q=(recNo-20)-p*5;                               //marco

    if((p>=0)&&(p<2)) {
	if (q==0) {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(p+REVENUE_ITEM_COUNT),  xColumn1);
	}
	else if(q==1) {
	    font_chart_red_sm.disp( x, y, "\tAY faculty compensation",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[p].faculty, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[p].change_last_year.faculty, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[p].this_year.faculty, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[p].change_next_year.faculty, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[p].next_year.faculty, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[p].faculty, 2));
	    // ##### End Marco ##### //
	}
	else if(q==2) {
	    font_chart_red_sm.disp( x, y, "\tStaff compensation",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[p].staff, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[p].change_last_year.staff, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[p].this_year.staff, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[p].change_next_year.staff, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[p].next_year.staff, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[p].staff, 2));
	    // ##### End Marco ##### //
	}
	else if(q==3) {
	    font_chart_red_sm.disp( x, y, "\tOther expenditures",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[p].other, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[p].change_last_year.other, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[p].this_year.other, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[p].change_next_year.other, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[p].next_year.other, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[p].other, 2));
	    // ##### End Marco ##### //
	}
	else if(q==4) {
	    font_chart_red_sm.disp( x, y, "\tTotal",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[p].total, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[p].change_last_year.total, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[p].this_year.total, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[p].change_next_year.total, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[p].next_year.total, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[p].total, 2));
	    // ##### End Marco ##### //
	}
    }

    // Calculate the sub-index p,q form recNo
    // recNo = p*4 + q +29

    //		p=(recNo-29)/4;		//0--6
    //		q=(recNo-29)-p*4;
    p=(recNo-30)/4;                                 //marco
    q=(recNo-30)-p*4;                               //marco

    if((p>=0)&&(p<=EXPENSE_ITEM_COUNT-5)) {
	// BUGHERE EXPENSE_ITEM_COUNT should be  minus 3
	p+=2;
	if (q==0) {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(p+REVENUE_ITEM_COUNT),  xColumn1);
	}
	else if(q==1) {
	    font_chart_red_sm.disp( x, y, "\tStaff compensation",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[p].staff, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[p].change_last_year.staff, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[p].this_year.staff, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[p].change_next_year.staff, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[p].next_year.staff, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[p].staff, 2));
	    // ##### End Marco ##### //
	}
	else if(q==2) {
	    font_chart_red_sm.disp( x, y, "\tOther expenditures",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[p].other, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[p].change_last_year.other, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[p].this_year.other, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[p].change_next_year.other, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[p].next_year.other, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[p].other, 2));
	    // ##### End Marco ##### //
	}
	else if(q==3) {
	    font_chart_red_sm.disp( x, y, "\tTotal",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[p].total, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[p].change_last_year.total, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[p].this_year.total, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[p].change_next_year.total, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[p].next_year.total, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[p].total, 2));
	    // ##### End Marco ##### //
	}
    }

    //		if((recNo>=61)&&(recNo<=65))
    //		{	if (recNo==61)
    if((recNo>=62)&&(recNo<=66)) {                  //marco
	//marco
	if (recNo==62) {
	    font_chart_blue_sm.disp( x, y,"  Total operating expenditures", xColumn1);
	}
	//			else if(recNo==62)
	else if(recNo==63) {                          //marco
	    font_chart_red_sm.disp( x, y, "\tAY faculty compensation",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.total_operating_expense.faculty, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_total_operating_expense.change_last_year.faculty, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_total_operating_expense.this_year.faculty, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_total_operating_expense.change_next_year.faculty, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_total_operating_expense.next_year.faculty, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_operating_expense.faculty, 2));
	    // ##### End Marco ##### //
	}
	//			else if(recNo==63)
	else if(recNo==64) {                          //marco
	    font_chart_red_sm.disp( x, y, "\tStaff compensation",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.total_operating_expense.staff, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_total_operating_expense.change_last_year.staff, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_total_operating_expense.this_year.staff, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_total_operating_expense.change_next_year.staff, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_total_operating_expense.next_year.staff, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_operating_expense.staff, 2));
	    // ##### End Marco ##### //
	}
	//			else if(recNo==64)
	else if(recNo==65) {                          //marco
	    font_chart_red_sm.disp( x, y, "\tOther expenditures",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.total_operating_expense.other, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_total_operating_expense.change_last_year.other, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_total_operating_expense.this_year.other, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_total_operating_expense.change_next_year.other, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_total_operating_expense.next_year.other, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_operating_expense.other, 2));
	    // ##### End Marco ##### //
	}
	//			else if(recNo==65)
	else if(recNo==66) {                          //marco
	    font_chart_red_sm.disp( x, y, "\tTotal",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.total_operating_expense.total, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_total_operating_expense.change_last_year.total, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_total_operating_expense.this_year.total, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_total_operating_expense.change_next_year.total, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_total_operating_expense.next_year.total, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_operating_expense.total, 2));
	    // ##### End Marco ##### //
	}
    }

    // Calculate the sub-index p,q form recNo
    // recNo = p*3 + q +62

    //		p=(recNo-66)/3;		//0--6
    //		q=(recNo-66)-p*3;
    p=(recNo-67)/3;                                 //marco
    q=(recNo-67)-p*3;                               //marco

    if((p>=0)&&(p<2)) {
	// BUGHERE EXPENSE_ITEM_COUNT should be  minus 3
	p+=10;
	if (q==0) {
	    font_chartsm.disp( x, y, game_str_res.financial_report_str(p+REVENUE_ITEM_COUNT),  xColumn1);
	}
	//			else if(q==1)
	//			{
	//				font_chart_red_sm.disp( x, y, "\tStaff compensation",  xColumn1);
	//				font_chartsm.disp( xColumn1, y+2, finance.expense_array[p].staff, 2));
	//				font_chartsm.disp( xColumn2, y+2, finance.projected_expense_array[p].change_last_year.staff, 3) );
	//				font_chartsm.disp( xColumn3, y+2, finance.projected_expense_array[p].this_year.staff, 2) );
	//				font_chartsm.disp( xColumn4, y+2, finance.projected_expense_array[p].change_next_year.staff, 3) );
	//				font_chartsm.disp( xColumn5, y+2, finance.projected_expense_array[p].next_year.staff, 2) );
	//			}
	else if(q==1) {
	    font_chart_red_sm.disp( x, y, "\tOther expenditures",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[p].other, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[p].change_last_year.other, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[p].this_year.other, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[p].change_next_year.other, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[p].next_year.other, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[p].other, 2));
	    // ##### End Marco ##### //
	}
	else if(q==2) {
	    font_chart_red_sm.disp( x, y, "\tTotal",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.expense_array[p].total, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_expense_array[p].change_last_year.total, 24) );
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_expense_array[p].this_year.total, 2) );
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_expense_array[p].change_next_year.total, 24) );
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_expense_array[p].next_year.total, 2) );

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_expense_array[p].total, 2));
	    // ##### End Marco ##### //
	}
    }
    //		else if(recNo==70)
    //		{
    //			font_chartsm.disp( x, y,"  Service on general plant debt", xColumn1);
    //			//BUGHERE Service on generatl plant debt, Transfer to capital reserve are missing
    //			//			font_chartsm.disp( x, y,"Surplus (deficit)", xColumn1);
    //			//			font_chartsm.disp( xColumn4, y+2, finance.projected_surplus_deficit, 2) );
    //		}
    //		else if(recNo==71)
    //		{
    //			font_chartsm.disp( x, y,"  Transfer to capital reserve", xColumn1);
    //		}
    //	else if((recNo>=72)&&(recNo<=76))
    else if((recNo>=73)&&(recNo<=77)) {             //Marco
	//		if (recNo==72)
	if (recNo==73) {                              //Marco
	    font_chart_blue_sm.disp( x, y,"  Total uses of funds", xColumn1);
	}
	//		else if(recNo==73)
	else if(recNo==74) {                          //Marco
	    font_chart_red_sm.disp( x, y, "\tAY faculty compensation",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.total_expense.faculty, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_total_expense.change_last_year.faculty, 24));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_total_expense.this_year.faculty, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_total_expense.change_next_year.faculty, 24));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_total_expense.next_year.faculty, 2));

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_expense.faculty, 2));
	    // ##### End Marco ##### //
	}
	//		else if(recNo==74)
	else if(recNo==75) {                          //Marco
	    font_chart_red_sm.disp( x, y, "\tStaff compensation",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.total_expense.staff, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_total_expense.change_last_year.staff, 24));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_total_expense.this_year.staff, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_total_expense.change_next_year.staff, 24));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_total_expense.next_year.staff, 2));

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_expense.staff, 2));
	    // ##### End Marco ##### //
	}
	//		else if(recNo==75)
	else if(recNo==76) {                          //Marco
	    font_chart_red_sm.disp( x, y, "\tOther expenditures",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.total_expense.other, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_total_expense.change_last_year.other, 24));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_total_expense.this_year.other, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_total_expense.change_next_year.other, 24));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_total_expense.next_year.other, 2));

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_expense.other, 2));
	    // ##### End Marco ##### //
	}
	//		else if(recNo==76)
	else if(recNo==77) {                          //Marco
	    font_chart_red_sm.disp( x, y, "\tTotal",  xColumn1);
	    font_chartsm.right_put( xColumn1, y+2, m.format(finance.total_expense.total, 2));
	    font_chartsm.right_put( xColumn2, y+2, m.format(finance.projected_total_expense.change_last_year.total, 24));
	    font_chartsm.right_put( xColumn3, y+2, m.format(finance.projected_total_expense.this_year.total, 2));
	    font_chartsm.right_put( xColumn4, y+2, m.format(finance.projected_total_expense.change_next_year.total, 24));
	    font_chartsm.right_put( xColumn5, y+2, m.format(finance.projected_total_expense.next_year.total, 2));

	    // ##### Begin Marco ##### //
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_total_expense.total, 2));
	    // ##### End Marco ##### //
	}
    }
    //	else if((recNo>=77)&&(recNo<=81))
    else if((recNo>=78)&&(recNo<=82)) {             //Marco
	//		if (recNo==77)
	if (recNo==78) {
	    font_chartsm.disp( x, y, "  Percent of total uses",  xColumn1);
	}
	//		else if(recNo==78)
	else if(recNo==79) {                          //Marco
	    font_chart_red_sm.disp( x, y, "\tAY faculty compensation",  xColumn1);
	    double percent1,percent2,percent3;
	    percent1=100*finance.total_expense.faculty/finance.total_expense.total;
	    percent2=100*finance.projected_total_expense.this_year.faculty/
		finance.projected_total_expense.this_year.total;
	    percent3=100*finance.projected_total_expense.next_year.faculty/
		finance.projected_total_expense.next_year.total;

	    font_chartsm.right_put( xColumn1, y+2, m.format(percent1, 24));
	    font_chartsm.right_put( xColumn3, y+2, m.format(percent2, 24));
	    font_chartsm.right_put( xColumn5, y+2, m.format(percent3, 24));

	    // ##### Begin Marco ##### //
	    double percent4=100*finance.initial_total_expense.faculty/finance.initial_total_expense.total;
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(percent4, 24));
	    // ##### End Marco ##### //
	}
	//		else if(recNo==79)
	else if(recNo==80) {                          //Marco
	    font_chart_red_sm.disp( x, y, "\tStaff compensation",  xColumn1);
	    double percent1,percent2,percent3;
	    percent1=100*finance.total_expense.staff/finance.total_expense.total;
	    percent2=100*finance.projected_total_expense.this_year.staff/
		finance.projected_total_expense.this_year.total;
	    percent3=100*finance.projected_total_expense.next_year.staff/
		finance.projected_total_expense.next_year.total;

	    font_chartsm.right_put( xColumn1, y+2, m.format(percent1, 24));
	    font_chartsm.right_put( xColumn3, y+2, m.format(percent2, 24));
	    font_chartsm.right_put( xColumn5, y+2, m.format(percent3, 24));

	    // ##### Begin Marco ##### //
	    double percent4=100*finance.initial_total_expense.staff/finance.initial_total_expense.total;
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(percent4, 24));
	    // ##### End Marco ##### //
	}
	//		else if(recNo==80)
	else if(recNo==81) {                          //Marco
	    font_chart_red_sm.disp( x, y, "\tOther expenditures",  xColumn1);
	    double percent1,percent2,percent3;
	    percent1=100*finance.total_expense.other/finance.total_expense.total;
	    percent2=100*finance.projected_total_expense.this_year.other/
		finance.projected_total_expense.this_year.total;
	    percent3=100*finance.projected_total_expense.next_year.other/
		finance.projected_total_expense.next_year.total;

	    font_chartsm.right_put( xColumn1, y+2, m.format(percent1, 24));
	    font_chartsm.right_put( xColumn3, y+2, m.format(percent2, 24));
	    font_chartsm.right_put( xColumn5, y+2, m.format(percent3, 24));

	    // ##### Begin Marco ##### //
	    double percent4=100*finance.initial_total_expense.other/finance.initial_total_expense.total;
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(percent4, 24));
	    // ##### End Marco ##### //
	}
	//		else if(recNo==81)
	else if(recNo==82) {                          //Marco
	    font_chart_red_sm.disp( x, y, "\tTotal",  xColumn1);
	    double percent1,percent2,percent3;
	    percent1=100;
	    percent2=100;
	    percent3=100;

	    font_chartsm.right_put( xColumn1, y+2, m.format(percent1, 24));
	    font_chartsm.right_put( xColumn3, y+2, m.format(percent2, 24));
	    font_chartsm.right_put( xColumn5, y+2, m.format(percent3, 24));

	    // ##### Begin Marco ##### //
	    double percent4 = 100;
	    if (info.financial_year() > 1)
		font_chart_purple_sm.right_put( xColumn0, y+2, m.format(percent4, 24));
	    // ##### End Marco ##### //
	}
    }
    //		else if(recNo==82)
    else if (recNo == 83) {                         // Marco
	font_chartsm.disp( x, y,"  Surplus (deficit)", xColumn2);
	if(finance.last_year.surplus_deficit>0)
	    font_chart_green_sm.right_put( xColumn1, y+2, m.format(finance.last_year.surplus_deficit, 2));
	else
	    font_chart_red_sm.right_put( xColumn1, y+2, m.format(finance.last_year.surplus_deficit, 2));

	if(finance.this_year.surplus_deficit>0)
	    font_chart_green_sm.right_put( xColumn3, y+2, m.format(finance.this_year.surplus_deficit, 2));
	else
	    font_chart_red_sm.right_put( xColumn3, y+2, m.format(finance.this_year.surplus_deficit, 2));

	if(finance.next_year.surplus_deficit>0)
	    font_chart_green_sm.right_put( xColumn5, y+2, m.format(finance.next_year.surplus_deficit, 2));
	else
	    font_chart_red_sm.right_put( xColumn5, y+2, m.format(finance.next_year.surplus_deficit, 2));

	// ##### Begin Marco ##### //
	if (info.financial_year() > 1)
	    font_chart_purple_sm.right_put( xColumn0, y+2, m.format(finance.initial_last_year.surplus_deficit, 2));
	// ##### End Marco ##### //
    }

}


//------ End of static function fin_rep_case9 --------//

//------ Begin of static function fin_rep_case9 --------//
//
// export the content of export_fin_rep_case9 to file
//
void export_fin_rep_case9( File *filePtr ) {
    WRITE_STR_FIELD( filePtr, "Three-Year Projection (Expanded)" ); WRITE_RECORD_SEPARATOR( filePtr );
    int tyear=info.financial_year();
    int i;
    // write year label
    WRITE_STR_FIELD( filePtr, "Year" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_STR_FIELD( filePtr, "Initial" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, tyear-1 ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_STR_FIELD( filePtr, "Change" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, tyear ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_STR_FIELD( filePtr, "Change" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_NUM_FIELD( filePtr, tyear+1 ); WRITE_RECORD_SEPARATOR( filePtr );
    // revenue
    WRITE_STR_FIELD( filePtr, "Revenue" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_FIELD_SEPARATOR( filePtr );               // blank initial field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank last year
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change
    WRITE_FIELD_SEPARATOR( filePtr );               // blank this year
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change
    WRITE_RECORD_SEPARATOR( filePtr );              // blank next year

    // revenue records
    for( i=0; i < REVENUE_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD(filePtr, game_str_res.financial_report_str(i) ); WRITE_FIELD_SEPARATOR(filePtr);

	if(i==AC_SPONSORED_RESEARCH_REVENUE) {
	    // insert Direct and inDirect, total
	    WRITE_FIELD_SEPARATOR( filePtr );           // blank initial field
	    WRITE_FIELD_SEPARATOR( filePtr );           // blank last year
	    WRITE_FIELD_SEPARATOR( filePtr );           // blank change
	    WRITE_FIELD_SEPARATOR( filePtr );           // blank this year
	    WRITE_FIELD_SEPARATOR( filePtr );           // blank change
	    WRITE_RECORD_SEPARATOR( filePtr );          // blank next year

	    // direct
	    WRITE_STR_FIELD(filePtr, "     Direct" ); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.initial_revenue_array[i].direct); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.revenue_array[i].direct); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_revenue_array[i].change_last_year.direct); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].this_year.direct); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_revenue_array[i].change_next_year.direct); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].next_year.direct); WRITE_RECORD_SEPARATOR(filePtr);

	    // in-direct
	    WRITE_STR_FIELD(filePtr, "     Indirect" ); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.initial_revenue_array[i].indirect); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.revenue_array[i].indirect); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_revenue_array[i].change_last_year.indirect); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].this_year.indirect); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_revenue_array[i].change_next_year.indirect); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].next_year.indirect); WRITE_RECORD_SEPARATOR(filePtr);

	    // write total label
	    WRITE_STR_FIELD(filePtr, "     Total" ); WRITE_FIELD_SEPARATOR(filePtr);
	}

	WRITE_NUM_FIELD(filePtr, finance.initial_revenue_array[i].total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.revenue_array[i].total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_revenue_array[i].change_last_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].this_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_revenue_array[i].change_next_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_revenue_array[i].next_year.total); WRITE_RECORD_SEPARATOR(filePtr);
    }

    WRITE_STR_FIELD(filePtr, "  Total sources of funds" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.initial_total_revenue_total ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_revenue.total ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_revenue.change_last_year.total ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_revenue.this_year.total ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_revenue.change_next_year.total ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_revenue.next_year.total ); WRITE_RECORD_SEPARATOR(filePtr);

    // expenditure
    WRITE_STR_FIELD( filePtr, "Expenditures" ); WRITE_FIELD_SEPARATOR( filePtr );
    WRITE_FIELD_SEPARATOR( filePtr );               // blank initial field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank last year
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change
    WRITE_FIELD_SEPARATOR( filePtr );               // blank this year
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change
    WRITE_RECORD_SEPARATOR( filePtr );              // blank next year

    for( i=0; i < EXPENSE_ITEM_COUNT; ++i ) {
	WRITE_STR_FIELD( filePtr, game_str_res.financial_report_str(i+REVENUE_ITEM_COUNT) ); WRITE_FIELD_SEPARATOR( filePtr );
	WRITE_FIELD_SEPARATOR( filePtr );             // blank initial field
	WRITE_FIELD_SEPARATOR( filePtr );             // blank last year
	WRITE_FIELD_SEPARATOR( filePtr );             // blank change
	WRITE_FIELD_SEPARATOR( filePtr );             // blank this year
	WRITE_FIELD_SEPARATOR( filePtr );             // blank change
	WRITE_RECORD_SEPARATOR( filePtr );            // blank next year

	// write AY faculty compensation
	if( i==AC_ACADEMIC_DEPARTMENTS||i==AC_SPONSORED_RESEARCH_EXPENSE) {
	    WRITE_STR_FIELD(filePtr, "     AY faculty compensation" ); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.initial_expense_array[i].faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.expense_array[i].faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].change_last_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].this_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].change_next_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].next_year.faculty); WRITE_RECORD_SEPARATOR(filePtr);
	}

	if(i<OPERATING_EXPENSE_ITEM_COUNT) {          //only for operating expenditures
	    // write staff compensation
	    WRITE_STR_FIELD(filePtr, "     Staff compensation" ); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.initial_expense_array[i].staff ); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.expense_array[i].staff ); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].change_last_year.staff ); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].this_year.staff ); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].change_next_year.staff ); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].next_year.staff ); WRITE_RECORD_SEPARATOR(filePtr);
	}

	// write other expenditures
	WRITE_STR_FIELD(filePtr, "     Other expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.initial_expense_array[i].other ); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.expense_array[i].other ); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].change_last_year.other ); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].this_year.other ); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].change_next_year.other ); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].next_year.other ); WRITE_RECORD_SEPARATOR(filePtr);

	// write total
	WRITE_STR_FIELD(filePtr, "     Total"); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.initial_expense_array[i].total ); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.expense_array[i].total ); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].change_last_year.total ); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].this_year.total ); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_expense_array[i].change_next_year.total ); WRITE_FIELD_SEPARATOR(filePtr);
	WRITE_NUM_FIELD(filePtr, finance.projected_expense_array[i].next_year.total ); WRITE_RECORD_SEPARATOR(filePtr);

	// write total operating expenditures
	if( i==OPERATING_EXPENSE_ITEM_COUNT-1 ) {
	    WRITE_STR_FIELD( filePtr, "  Total operating expenditures" );WRITE_FIELD_SEPARATOR( filePtr );
	    WRITE_FIELD_SEPARATOR( filePtr );           // blank initial field
	    WRITE_FIELD_SEPARATOR( filePtr );           // blank last year
	    WRITE_FIELD_SEPARATOR( filePtr );           // blank change
	    WRITE_FIELD_SEPARATOR( filePtr );           // blank this year
	    WRITE_FIELD_SEPARATOR( filePtr );           // blank change
	    WRITE_RECORD_SEPARATOR( filePtr );          // blank next year

	    WRITE_STR_FIELD(filePtr, "     AY faculty compensation"); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.initial_total_operating_expense.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.total_operating_expense.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.change_last_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.this_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.change_next_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.next_year.faculty); WRITE_RECORD_SEPARATOR(filePtr);

	    WRITE_STR_FIELD(filePtr, "     Staff faculty compensation"); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.initial_total_operating_expense.staff); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.total_operating_expense.staff); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.change_last_year.staff); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.this_year.staff); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.change_next_year.staff); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.next_year.staff); WRITE_RECORD_SEPARATOR(filePtr);

	    WRITE_STR_FIELD(filePtr, "     Other expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.initial_total_operating_expense.other); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.total_operating_expense.other); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.change_last_year.other); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.this_year.other); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.change_next_year.other); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.next_year.other); WRITE_RECORD_SEPARATOR(filePtr);

	    WRITE_STR_FIELD(filePtr, "     Total"); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.initial_total_operating_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.total_operating_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.change_last_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.this_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_operating_expense.change_next_year.total); WRITE_FIELD_SEPARATOR(filePtr);
	    WRITE_NUM_FIELD(filePtr, finance.projected_total_operating_expense.next_year.total); WRITE_RECORD_SEPARATOR(filePtr);
	}
    }

    WRITE_STR_FIELD(filePtr, "  Total uses of funds" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR( filePtr );               // blank initial field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank last year
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change
    WRITE_FIELD_SEPARATOR( filePtr );               // blank this year
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change
    WRITE_RECORD_SEPARATOR( filePtr );              // blank next year

    WRITE_STR_FIELD(filePtr, "     AY faculty compensation" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.initial_total_expense.faculty ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_expense.faculty); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.change_last_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.this_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.change_next_year.faculty); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.next_year.faculty); WRITE_RECORD_SEPARATOR(filePtr);

    WRITE_STR_FIELD(filePtr, "     Staff compensation" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.initial_total_expense.staff ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_expense.staff); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.change_last_year.staff); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.this_year.staff); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.change_next_year.staff); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.next_year.staff); WRITE_RECORD_SEPARATOR(filePtr);

    WRITE_STR_FIELD(filePtr, "     Other expenditures" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.initial_total_expense.other ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_expense.other); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.change_last_year.other); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.this_year.other); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.change_next_year.other); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.next_year.other); WRITE_RECORD_SEPARATOR(filePtr);

    WRITE_STR_FIELD(filePtr, "     Total" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.initial_total_expense.total ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.total_expense.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.change_last_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.this_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, 0.01*finance.projected_total_expense.change_next_year.total); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.projected_total_expense.next_year.total); WRITE_RECORD_SEPARATOR(filePtr);

    // write percent of total use
    WRITE_STR_FIELD(filePtr, "  Percent of total uses" ); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR( filePtr );               // blank initial field
    WRITE_FIELD_SEPARATOR( filePtr );               // blank last year
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change
    WRITE_FIELD_SEPARATOR( filePtr );               // blank this year
    WRITE_FIELD_SEPARATOR( filePtr );               // blank change
    WRITE_RECORD_SEPARATOR( filePtr );              // blank next year

    double percent1,percent2,percent3,percent4;

    WRITE_STR_FIELD(filePtr, "     AY faculty compensation"); WRITE_FIELD_SEPARATOR(filePtr);
    percent1=finance.total_expense.faculty/finance.total_expense.total;
    percent2=finance.projected_total_expense.this_year.faculty/
	finance.projected_total_expense.this_year.total;
    percent3=finance.projected_total_expense.next_year.faculty/
	finance.projected_total_expense.next_year.total;
    percent4=finance.initial_total_expense.faculty/finance.initial_total_expense.total;
    WRITE_NUM_FIELD(filePtr, percent4); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent1); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent2); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent3); WRITE_RECORD_SEPARATOR(filePtr);

    WRITE_STR_FIELD(filePtr, "     Staff compensation"); WRITE_FIELD_SEPARATOR(filePtr);
    percent1=finance.total_expense.staff/finance.total_expense.total;
    percent2=finance.projected_total_expense.this_year.staff/
	finance.projected_total_expense.this_year.total;
    percent3=finance.projected_total_expense.next_year.staff/
	finance.projected_total_expense.next_year.total;
    percent4=finance.initial_total_expense.staff/finance.initial_total_expense.total;
    WRITE_NUM_FIELD(filePtr, percent4); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent1); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent2); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent3); WRITE_RECORD_SEPARATOR(filePtr);

    WRITE_STR_FIELD(filePtr, "     Other expenditures"); WRITE_FIELD_SEPARATOR(filePtr);
    percent1=finance.total_expense.other/finance.total_expense.total;
    percent2=finance.projected_total_expense.this_year.other/
	finance.projected_total_expense.this_year.total;
    percent3=finance.projected_total_expense.next_year.other/
	finance.projected_total_expense.next_year.total;
    percent4=finance.initial_total_expense.other/finance.initial_total_expense.total;
    WRITE_NUM_FIELD(filePtr, percent4); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent1); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent2); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent3); WRITE_RECORD_SEPARATOR(filePtr);

    WRITE_STR_FIELD(filePtr, "     Total"); WRITE_FIELD_SEPARATOR(filePtr);
    percent1=1.0;
    percent2=1.0;
    percent3=1.0;
    percent4=1.0;
    WRITE_NUM_FIELD(filePtr, percent4); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent1); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent2); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, percent3); WRITE_RECORD_SEPARATOR(filePtr);

    WRITE_STR_FIELD(filePtr, "  Surplus (deficit)"); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.initial_last_year.surplus_deficit); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.last_year.surplus_deficit); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.this_year.surplus_deficit); WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_FIELD_SEPARATOR(filePtr);
    WRITE_NUM_FIELD(filePtr, finance.next_year.surplus_deficit); WRITE_RECORD_SEPARATOR(filePtr);
}

//------ End of static function export_fin_rep_case9 --------//

//------ Begin of static function disp_fin_rep_row --------//

static void disp_fin_rep_row(int recNo, int x, int y, int refreshFlag) {
    //----- display financial report -----//
    //	user_interface.bar(x, y, REPORT_X2-10, y+browse_financial.rec_height*2-5,FIN_REP_SHEET_COLOR);

    // Case 1: RENVENUE	and PREVIOUS YEAR
    if(finance.report_type==REPORT_TYPE_REVENUE &&
       finance.report_year==REPORT_YEAR_PREV) {
	fin_rep_case1(recNo,x,y);
    }

    // Case 2: RENVENUE and THIS YEAR
    if(finance.report_type==REPORT_TYPE_REVENUE &&
       finance.report_year==REPORT_YEAR_THIS) {
	fin_rep_case2(recNo,x,y);
    }

    // Case 3: RENVENUE and NEXT YEAR
    if(finance.report_type==REPORT_TYPE_REVENUE &&
       finance.report_year==REPORT_YEAR_NEXT) {
	fin_rep_case3(recNo,x,y);
    }

    // Case 4: RENVENUE and THREE YEARS
    if(finance.report_type==REPORT_TYPE_REVENUE &&
       finance.report_year==REPORT_YEAR_THREE_YEARS  //&&
	) {                                             /*!finance.report_is_expanded */
	fin_rep_case4(recNo,x,y);
    }

    // Case 5: BALANCE SHEET && LAST YEAR
    if(finance.report_type==REPORT_TYPE_BALANCE &&
       finance.report_year==REPORT_YEAR_PREV) {
	fin_rep_case5(recNo,x,y);
    }

    // Case 6: BALANCE SHEET && THIS YEAR
    if(finance.report_type==REPORT_TYPE_BALANCE &&
       finance.report_year==REPORT_YEAR_THIS) {
	fin_rep_case6(recNo,x,y);
    }

    // Case 7: BALANCE SHEET && NEXT YEAR
    if(finance.report_type==REPORT_TYPE_BALANCE &&
       finance.report_year==REPORT_YEAR_NEXT) {
	fin_rep_case7(recNo,x,y);
    }

    // Case 8: BALANCE SHEET && THREE YEAR

    if(finance.report_type==REPORT_TYPE_BALANCE &&
       finance.report_year==REPORT_YEAR_THREE_YEARS) {
	fin_rep_case8(recNo,x,y);
    }

    // Case 9: EXPANDING THREE YEARS REVENUE
    /*	if( finance.report_is_expanded &&
	finance.report_type==REPORT_TYPE_REVENUE &&
	finance.report_year==REPORT_YEAR_THREE_YEARS )
    */
    if(finance.report_type==REPORT_TYPE_DETAIL_REN) {
	finance.report_year=REPORT_YEAR_THREE_YEARS;
	fin_rep_case9(recNo,x,y);
    }
    //	vga.blt_buf( x, y, REPORT_X2-10, y+browse_financial.rec_height*2-5);

}

//------ End of static function disp_fin_rep_row --------//
