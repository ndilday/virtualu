//Filename    : OPRTMENU4.CPP
//Description : Printing Menu Class Implementation
//Owner		  : Kevin(ho)

#include <OPRINT.H>
#include <OVGALOCK.H>
#include <OPRTMENU.H>
#include <OWORLDMT.H>
#include <OSYS.H>
#include <OBOX.H>
#include <OVGA.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OMOUSE2.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <ODEPT.H>
#include <OPSCHOOL.H>
#include <OFINANCE.H>
#include <ODATE.H>

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-55
};

enum {
    PRINT_OPT1=1,
    PRINT_OPT2=2,
    PRINT_OPT3=3,
    PRINT_OPT4=4,
};

void PrintMenu::printing4() {
    box.tell("Now printing. \nPlease wait...");
    sys.no_true_output_flag=1;
    box.close();
    sys.no_true_output_flag=0;

    //////////////////////////////////////////////////////////////////////////////////
    /// ----- FINANCIAL REPORT ------- //
    //////////////////////////////////////////////////////////////////////////////////
    if(print_view_mode==MODE_FINANCIAL_REPORT) { {
	MouseDispCount mdc;
	print.start_print(sys.main_hwnd,sys.app_hinstance);
	print.open_page();

	print.dfont(18,false,false,700);
	print.put_text("Virtual U Financial Report",22,1);
	print.dfont(10,false,false,0);

	print.put_text("Insitution : ",30,4);
	//Player Institution
	print.put_text(player_school.school_name,44,4);
	print.put_text("President : ",30,5);
	String str;
	str =  player_school.player_first_name;
	str += " ";
	str += player_school.player_last_name;
	print.put_text(str.str_buf,44,5);           //President
	print.put_text("Date : ",99,5);
	// print.put_text(date.date_str(info.game_date),107,5);                   //date
	print.put_text(info.date_str_julian(info.game_date), 107,5 );

	if(player_school.scenario_id>0) {
	    print.dfont(10,false,false,700);
	    print.put_text(scn_name[player_school.scenario_id-1],50,6);
	}

	if(print_option==1) {
	    user_interface.backgd_flag=0;
	    sys.no_true_output_flag=1;

	    finance.report_deinit();
	    finance.disp_report_main(INFO_REPAINT);

	    /*				if(finance.report_type!=REPORT_TYPE_REVENUE)
					{
					print.put_vga_back_center("prn001.bmp",
					ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-63,
					0.2,ZOOM_X1,ZOOM_Y1
					);
					}
	    */
	    //---------------------------------------------//

	    // Print Context
	    if((finance.report_type==REPORT_TYPE_REVENUE)&&
	       (finance.report_year==REPORT_YEAR_THREE_YEARS)&&
	       (finance.report_is_expanded==0)) {
		finance.report_deinit();
		finance.report_financial_recno=1;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-82,
					  0.21,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=32;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn002.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-90,
					  0.41,ZOOM_X1+7,ZOOM_Y1+37
		    );
	    }

	    //---------------------------------------------//

	    if( finance.report_type==REPORT_TYPE_DETAIL_REN ||
		( finance.report_type==REPORT_TYPE_REVENUE &&
		  finance.report_year==REPORT_YEAR_THREE_YEARS &&
		  finance.report_is_expanded==1 ) ) {
		finance.report_deinit();
		finance.report_financial_recno=1;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.11,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=32;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn002.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.31,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=48;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn003.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.51,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=64;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn004.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.71,ZOOM_X1+7,ZOOM_Y1+20
		    );

		print.end_page();
		print.open_page();

		finance.report_deinit();
		finance.report_financial_recno=80;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn005.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.11,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=96;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn006.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-350,
					  0.31,ZOOM_X1+7,ZOOM_Y1+290
		    );

	    }

	    //---------------------------------------------//

	    // Print Context
	    if((finance.report_type==REPORT_TYPE_REVENUE)&&
	       ((finance.report_year==REPORT_YEAR_PREV)||
		(finance.report_year==REPORT_YEAR_THIS)||
		(finance.report_year==REPORT_YEAR_NEXT))
		) {
		finance.report_deinit();
		finance.report_financial_recno=1;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.22,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=32;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn002.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.405,ZOOM_X1+7,ZOOM_Y1+20
		    );
	    }

	    //---------------------------------------------//

	    // Print Context
	    if((finance.report_type==REPORT_TYPE_BALANCE)&&
	       (finance.report_year==REPORT_YEAR_THREE_YEARS)) {
		finance.report_deinit();
		finance.report_financial_recno=1;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.21,ZOOM_X1+7,ZOOM_Y1+20
		    );
	    }

	    //---------------------------------------------//

	    if((finance.report_type==REPORT_TYPE_OPTIMIZATION)&&
	       (finance.report_opt==REPORT_OPT_REVENUE)) {
		finance.report_deinit();
		finance.report_financial_recno=1;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-20,ZOOM_Y2-ZOOM_Y1-50,
					  0.30,ZOOM_X1+7,ZOOM_Y1+3
		    );
	    }

	    //---------------------------------------------//

	    if((finance.report_type==REPORT_TYPE_OPTIMIZATION)&&
	       (finance.report_opt==REPORT_OPT_FUNCTION)) {
		finance.report_deinit();
		finance.report_financial_recno=1;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-20,ZOOM_Y2-ZOOM_Y1-50,
					  0.30,ZOOM_X1+7,ZOOM_Y1+3
		    );
	    }

	    //---------------------------------------------//

	    if((finance.report_type==REPORT_TYPE_OPTIMIZATION)&&
	       (finance.report_opt==REPORT_OPT_FACULTY)) {
		finance.report_deinit();
		finance.report_financial_recno=1;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-20,ZOOM_Y2-ZOOM_Y1-50,
					  0.30,ZOOM_X1+7,ZOOM_Y1+3
		    );
	    }

	    //---------------------------------------------//

	    user_interface.backgd_flag=1;
	    finance.report_deinit();
	    // ###### begin Gilbert 28/04/2001 ######//
	    // save memory
	    // finance.disp_report_main(INFO_REPAINT);
	    // ###### end Gilbert 28/04/2001 ######//
	    sys.no_true_output_flag=0;
	}

	if(print_option==2||print_option==3) {
	    user_interface.backgd_flag=0;
	    sys.no_true_output_flag=1;
	    sys.view_mode=MODE_FINANCIAL_REPORT;
	    int report_type_backup=finance.report_type;
	    int report_year_backup=finance.report_year;
	    int report_expand_backup=finance.report_is_expanded;
	    int report_opt_backup=finance.report_opt;

	    if(print_option==2)
		finance.report_is_expanded=0;
	    if(print_option==3)
		finance.report_is_expanded=1;

	    if(print_option==2) {
		// Print Context
		finance.report_type=REPORT_TYPE_REVENUE;
		finance.report_year=REPORT_YEAR_THREE_YEARS;

		finance.report_deinit();
		finance.report_financial_recno=1;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-82,
					  0.11,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=32;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn002.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-90,
					  0.31,ZOOM_X1+7,ZOOM_Y1+37
		    );
	    }

	    if(print_option==3) {
		// Print Context
		finance.report_type=REPORT_TYPE_REVENUE;
		finance.report_year=REPORT_YEAR_THREE_YEARS;

		finance.report_deinit();
		finance.report_financial_recno=1;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.11,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=32;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn002.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.31,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=48;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn003.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.51,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=64;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn004.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.71,ZOOM_X1+7,ZOOM_Y1+20
		    );

		print.end_page();
		print.open_page();

		finance.report_deinit();
		finance.report_financial_recno=80;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn005.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
					  0.11,ZOOM_X1+7,ZOOM_Y1+20
		    );

		finance.report_deinit();
		finance.report_financial_recno=96;
		finance.disp_report_main(INFO_REPAINT);
		print.put_vga_back_center("prn006.bmp",
					  ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-350,
					  0.31,ZOOM_X1+7,ZOOM_Y1+290
		    );
	    }

	    // Print Context
	    finance.report_type=REPORT_TYPE_REVENUE;
	    finance.report_year=REPORT_YEAR_PREV;

	    finance.report_deinit();
	    finance.report_financial_recno=1;
	    finance.disp_report_main(INFO_REPAINT);
	    print.put_vga_back_center("prn001.bmp",
				      ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
				      0.52,ZOOM_X1+7,ZOOM_Y1+20
		);

	    finance.report_deinit();
	    finance.report_financial_recno=32;
	    finance.disp_report_main(INFO_REPAINT);
	    print.put_vga_back_center("prn002.bmp",
				      ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
				      0.705,ZOOM_X1+7,ZOOM_Y1+20
		);

	    print.end_page();
	    print.open_page();

	    // Print Context
	    finance.report_type=REPORT_TYPE_REVENUE;
	    finance.report_year=REPORT_YEAR_THIS;

	    finance.report_deinit();
	    finance.report_financial_recno=1;
	    finance.disp_report_main(INFO_REPAINT);
	    print.put_vga_back_center("prn001.bmp",
				      ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
				      0.05,ZOOM_X1+7,ZOOM_Y1+20
		);

	    finance.report_deinit();
	    finance.report_financial_recno=32;
	    finance.disp_report_main(INFO_REPAINT);
	    print.put_vga_back_center("prn002.bmp",
				      ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
				      0.236,ZOOM_X1+7,ZOOM_Y1+20
		);

	    // Print Context
	    finance.report_type=REPORT_TYPE_REVENUE;
	    finance.report_year=REPORT_YEAR_NEXT;

	    finance.report_deinit();
	    finance.report_financial_recno=1;
	    finance.disp_report_main(INFO_REPAINT);
	    print.put_vga_back_center("prn003.bmp",
				      ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
				      0.51,ZOOM_X1+7,ZOOM_Y1+20
		);

	    finance.report_deinit();
	    finance.report_financial_recno=32;
	    finance.disp_report_main(INFO_REPAINT);
	    print.put_vga_back_center("prn004.bmp",
				      ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
				      0.696,ZOOM_X1+7,ZOOM_Y1+20
		);

	    print.end_page();
	    print.open_page();

	    // Print Context
	    finance.report_type=REPORT_TYPE_BALANCE;
	    finance.report_year=REPORT_YEAR_THREE_YEARS;

	    finance.report_deinit();
	    finance.report_financial_recno=1;
	    finance.disp_report_main(INFO_REPAINT);
	    print.put_vga_back_center("prn005.bmp",
				      ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
				      0.11,ZOOM_X1+7,ZOOM_Y1+20
		);

	    // Print Context
	    finance.report_type=REPORT_TYPE_BALANCE;
	    finance.report_year=REPORT_YEAR_PREV;

	    finance.report_deinit();
	    finance.report_financial_recno=1;
	    finance.disp_report_main(INFO_REPAINT);
	    print.put_vga_back_center("prn006.bmp",
				      ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
				      0.52,ZOOM_X1+7,ZOOM_Y1+20
		);

	    print.end_page();
	    print.open_page();

	    // Print Context
	    finance.report_type=REPORT_TYPE_BALANCE;
	    finance.report_year=REPORT_YEAR_THIS;

	    finance.report_deinit();
	    finance.report_financial_recno=1;
	    finance.disp_report_main(INFO_REPAINT);
	    print.put_vga_back_center("prn001.bmp",
				      ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
				      0.11,ZOOM_X1+7,ZOOM_Y1+20
		);

	    // Print Context
	    finance.report_type=REPORT_TYPE_BALANCE;
	    finance.report_year=REPORT_YEAR_NEXT;

	    finance.report_deinit();
	    finance.report_financial_recno=1;
	    finance.disp_report_main(INFO_REPAINT);
	    print.put_vga_back_center("prn003.bmp",
				      ZOOM_X2-ZOOM_X1-30,ZOOM_Y2-ZOOM_Y1-80,
				      0.52,ZOOM_X1+7,ZOOM_Y1+20
		);

	    print.end_page();
	    print.open_page();

	    finance.report_type=REPORT_TYPE_OPTIMIZATION;
	    finance.report_opt=REPORT_OPT_REVENUE;

	    finance.report_deinit();
	    finance.report_financial_recno=1;
	    finance.disp_report_main(INFO_REPAINT);
	    finance.disp_report_main(INFO_UPDATE);
	    print.put_vga_back_center("prn005.bmp",
				      ZOOM_X2-ZOOM_X1-20,ZOOM_Y2-ZOOM_Y1-50,
				      0.10,ZOOM_X1+7,ZOOM_Y1+3
		);

	    // Print Context
	    finance.report_type=REPORT_TYPE_OPTIMIZATION;
	    finance.report_opt=REPORT_OPT_FUNCTION;

	    finance.report_deinit();
	    finance.report_financial_recno=1;
	    finance.disp_report_main(INFO_REPAINT);
	    finance.disp_report_main(INFO_UPDATE);
	    print.put_vga_back_center("prn006.bmp",
				      ZOOM_X2-ZOOM_X1-20,ZOOM_Y2-ZOOM_Y1-50,
				      0.40,ZOOM_X1+7,ZOOM_Y1+3
		);

	    // Print Context
	    finance.report_type=REPORT_TYPE_OPTIMIZATION;
	    finance.report_opt=REPORT_OPT_FACULTY;

	    finance.report_deinit();
	    finance.report_financial_recno=1;
	    finance.disp_report_main(INFO_REPAINT);
	    finance.disp_report_main(INFO_UPDATE);
	    print.put_vga_back_center("prn007.bmp",
				      ZOOM_X2-ZOOM_X1-20,ZOOM_Y2-ZOOM_Y1-50,
				      0.70,ZOOM_X1+7,ZOOM_Y1+3
		);

	    // Deinit //
	    user_interface.backgd_flag=1;
	    finance.report_deinit();
	    finance.report_type=report_type_backup;
	    finance.report_year=report_year_backup;
	    finance.report_opt=report_opt_backup;
	    finance.report_is_expanded=report_expand_backup;
	    // ###### begin Gilbert 28/04/2001 ######//
	    // save memory
	    // finance.disp_report_main(INFO_REPAINT);
	    // ###### end Gilbert 28/04/2001 ######//
	    sys.no_true_output_flag=0;
	}

	print.end_page();
	print.endprint(sys.main_hwnd);
    }
    }
}
