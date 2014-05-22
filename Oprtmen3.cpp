//Filename    : OPRTMENU3.CPP
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
#include <OPSCHOOL.H>
#include <OLIBTECH.H>
#include <OINVEST.H>
#include <OPARKING.H>
#include <OCRIME.H>
#include <ODEPT.H>
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

void PrintMenu::printing3() {
    box.tell("Now printing. \nPlease wait...");
    sys.no_true_output_flag=1;
    box.close();
    sys.no_true_output_flag=0;

    //////////////////////////////////////////////////////////////////////////////////
    /// ----- FIRM REPORT ------- //
    //////////////////////////////////////////////////////////////////////////////////
    if( print_view_mode >= MODE_FIRST_NONDEPARTMENTAL_FIRM &&
	print_view_mode <= MODE_LAST_NONDEPARTMENTAL_FIRM ) {
	switch( FIRST_NONDEPARTMENTAL_FIRM_ID + print_view_mode - MODE_FIRST_NONDEPARTMENTAL_FIRM ) {
	case FIRM_PARK_LOTS:
	case FIRM_PARK_STRUCTURE: {
	    MouseDispCount mdc;
	    print.start_print(sys.main_hwnd,sys.app_hinstance);
	    print.open_page();

	    print.dfont(18,false,false,700);
	    print.put_text("Virtual U Parking Report",20,1);
	    print.dfont(10,false,false,0);

	    print.put_text("Insitution : ",30,4);
	    //Player Institution
	    print.put_text(player_school.school_name,44,4);
	    print.put_text("President : ",30,5);
	    String str;
	    str =  player_school.player_first_name;
	    str += " ";
	    str += player_school.player_last_name;
	    print.put_text(str.str_buf,44,5);         //President
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

		sys.view_mode=FIRM_OLD_MAIN;

		parking.deinit();
		parking.parking_report(INFO_REPAINT);

		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-80,ZOOM_Y2-ZOOM_Y1-50,
					  0.2,ZOOM_X1+40,ZOOM_Y1
		    );

		sys.no_true_output_flag=0;
		user_interface.backgd_flag=1;
		sys.view_mode=print_view_mode;

		parking.deinit();
		// ###### begin Gilbert 28/04/2001 ######//
		// save memory
		// parking.parking_report(INFO_REPAINT);
		// ###### end Gilbert 28/04/2001 ######//
	    }

	    print.end_page();
	    print.endprint(sys.main_hwnd);
	}
	break;

	case FIRM_POLICE: {
	    MouseDispCount mdc;
	    print.start_print(sys.main_hwnd,sys.app_hinstance);
	    print.open_page();

	    print.dfont(18,false,false,700);
	    print.put_text("Virtual U Crime Report",23,1);
	    print.dfont(10,false,false,0);

	    print.put_text("Insitution : ",30,4);
	    //Player Institution
	    print.put_text(player_school.school_name,44,4);
	    print.put_text("President : ",30,5);
	    String str;
	    str =  player_school.player_first_name;
	    str += " ";
	    str += player_school.player_last_name;
	    print.put_text(str.str_buf,44,5);         //President
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

		sys.view_mode=FIRM_OLD_MAIN;

		crime.deinit();
		crime.crime_report(INFO_REPAINT);

		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1-80,ZOOM_Y2-ZOOM_Y1-50,
					  0.2,ZOOM_X1+40,ZOOM_Y1
		    );

		sys.no_true_output_flag=0;
		user_interface.backgd_flag=1;
		sys.view_mode=print_view_mode;

		crime.deinit();
		// ###### begin Gilbert 28/04/2001 ######//
		// save memory
		// crime.crime_report(INFO_REPAINT);
		// ###### end Gilbert 28/04/2001 ######//
	    }

	    print.end_page();
	    print.endprint(sys.main_hwnd);
	}
	break;

	case FIRM_OLD_MAIN: {
	    enum {                                    // the 'sub' view-modes
		MODE_INVESTMENT = 0,
		MODE_FINANCIAL = 1,
	    };

	    MouseDispCount mdc;
	    print.start_print(sys.main_hwnd,sys.app_hinstance);
	    print.open_page();

	    print.dfont(18,false,false,700);
	    print.put_text("Virtual U Investment Report",21,1);
	    print.dfont(10,false,false,0);

	    print.put_text("Insitution : ",30,4);
	    //Player Institution
	    print.put_text(player_school.school_name,44,4);
	    print.put_text("President : ",30,5);
	    String str;
	    str =  player_school.player_first_name;
	    str += " ";
	    str += player_school.player_last_name;
	    print.put_text(str.str_buf,44,5);         //President
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

		sys.view_mode=FIRM_OLD_MAIN;

		investment_office.deinit();
		investment_office.report(INFO_REPAINT);

		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
					  0.2,ZOOM_X1,ZOOM_Y1
		    );

		sys.no_true_output_flag=0;
		user_interface.backgd_flag=1;
		sys.view_mode=print_view_mode;

		investment_office.deinit();
		// ###### begin Gilbert 28/04/2001 ########//
		// save memory
		// investment_office.report(INFO_REPAINT);
		// ###### end Gilbert 28/04/2001 ########//
	    }

	    if(print_option==2) {
		user_interface.backgd_flag=0;
		sys.no_true_output_flag=1;

		int report_type_backup=library_tech_office.current_mode;

		sys.view_mode=FIRM_OLD_MAIN;
		investment_office.deinit();

		investment_office.current_mode=MODE_INVESTMENT;
		investment_office.report(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
					  0.15,ZOOM_X1,ZOOM_Y1
		    );

		investment_office.deinit();
		investment_office.current_mode=MODE_FINANCIAL;
		investment_office.report(INFO_REPAINT);
		print.put_vga_back_center("prn002.bmp",
					  ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
					  0.4,ZOOM_X1,ZOOM_Y1
		    );

		investment_office.deinit();
		investment_office.current_mode=report_type_backup;

		sys.no_true_output_flag=0;
		user_interface.backgd_flag=1;
		sys.view_mode=print_view_mode;
		// ###### begin Gilbert 28/04/2001 ########//
		// save memory
		// investment_office.report(INFO_REPAINT);
		// ###### end Gilbert 28/04/2001 ########//
	    }

	    print.end_page();
	    print.endprint(sys.main_hwnd);
	}
	break;

	case FIRM_LIBRARY: {
	    enum {                                    // the 'sub' view-modes
		MODE_OVERVIEW = 0,
		MODE_FINANCIAL = 1,
	    };

	    MouseDispCount mdc;
	    print.start_print(sys.main_hwnd,sys.app_hinstance);
	    print.open_page();

	    print.dfont(18,false,false,700);
	    print.put_text("Virtual U Library and Information Technology Report",6,1);
	    print.dfont(10,false,false,0);

	    print.put_text("Insitution : ",30,4);
	    //Player Institution
	    print.put_text(player_school.school_name,44,4);
	    print.put_text("President : ",30,5);
	    String str;
	    str =  player_school.player_first_name;
	    str += " ";
	    str += player_school.player_last_name;
	    print.put_text(str.str_buf,44,5);         //President
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

		sys.view_mode=FIRM_LIBRARY;

		library_tech_office.deinit();
		library_tech_office.report(INFO_REPAINT);

		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
					  0.2,ZOOM_X1,ZOOM_Y1
		    );

		sys.no_true_output_flag=0;
		user_interface.backgd_flag=1;
		sys.view_mode=print_view_mode;

		library_tech_office.deinit();
		// #### begin Gilbert 28/04/2001 #####//
		// save memory
		// library_tech_office.report(INFO_REPAINT);
		// #### end Gilbert 28/04/2001 #####//
	    }

	    if(print_option==2) {
		user_interface.backgd_flag=0;
		sys.no_true_output_flag=1;

		int report_type_backup=library_tech_office.current_mode;

		sys.view_mode=FIRM_LIBRARY;
		library_tech_office.deinit();

		library_tech_office.current_mode=MODE_OVERVIEW;
		library_tech_office.report(INFO_REPAINT);
		print.put_vga_back_center("prn001.bmp",
					  ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
					  0.15,ZOOM_X1,ZOOM_Y1
		    );

		library_tech_office.deinit();
		library_tech_office.current_mode=MODE_FINANCIAL;
		library_tech_office.report(INFO_REPAINT);
		print.put_vga_back_center("prn002.bmp",
					  ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
					  0.4,ZOOM_X1,ZOOM_Y1
		    );

		library_tech_office.deinit();
		library_tech_office.current_mode=report_type_backup;

		sys.no_true_output_flag=0;
		user_interface.backgd_flag=1;
		sys.view_mode=print_view_mode;
		// #### begin Gilbert 28/04/2001 #####//
		// save memory
		// library_tech_office.report(INFO_REPAINT);
		// #### end Gilbert 28/04/2001 #####//
	    }

	    print.end_page();
	    print.endprint(sys.main_hwnd);
	}
	break;

	}
    }
    //////////////////////////////////////////////////////////////////////////////////

}
