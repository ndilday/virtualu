//Filename    : OPRTMENU.CPP
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
#include <ODEPT.H>
#include <ODATE.H>

enum {  BG_PIC_ID = 7 };

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

PrintMenu::PrintMenu() {
    init();
}

PrintMenu::~PrintMenu() {
    deinit();
}

void PrintMenu::init() {
    init_flag = 0;
    in_print_mode = 0;
}

;

void PrintMenu::deinit() {
    init_flag = 0;
};

static String option_label[4];

void PrintMenu::report(int refreshFlag) {
    int tab1=REPORT_X1+110;
    int tab2=REPORT_X1+240;
    int vtab1=REPORT_Y1+10;
    int vtab2=REPORT_Y1+50;
    int vtab3=REPORT_Y1+90;
    int vtab4=REPORT_Y1+300;

    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	if(init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!init_flag) {
	    option_label[0]="";
	    option_label[1]="";
	    option_label[2]="";
	    option_label[3]="";

	    print_option=1;
	    user_interface.bg_img(BG_PIC_ID, &vga_back);
	    info.disp_column_bitmap(&vga_back);
	    init_flag = 1;
	    font_hstone.put(tab2+40,vtab1,"PRINT MENU");

	    if((print_view_mode==MODE_STUDENT_REPORT)||
	       (print_view_mode==MODE_FACULTY_SUMMARY)||
	       (print_view_mode==MODE_COURSE_REPORT)
		) {
		//				font_hstone.put(tab2-100,vtab2,"Student Report");
		option_label[0]="Current Screen";
		// ###### begin Gilbert 05/06/2001 ######//
		// option_label[1]="Departmental Report (Current Departments)";
		option_label[1]="Departmental Report (Current Department)";
		// ###### end Gilbert 05/06/2001 ######//
		option_label[2]="Departmental Report (All Departments)";
		option_label[3]="Institutional Report";
	    }

	    if(
		(print_view_mode==MODE_SCENARIO_LETTER)||
		(print_view_mode==MODE_SCEN_WELCOME_LETTER)||
		(print_view_mode==MODE_WELCOME_LETTER)||
		(print_view_mode==MODE_EVALUATION_LETTER)
		) {
		if((info.year_passed<=1)
		   &&(player_school.scenario_id==0)) {
		    option_label[0]="Welcome Letter";
		    option_label[1]="";
		    option_label[2]="";
		    option_label[3]="";
		}
		else {
		    option_label[0]="Welcome Letter";
		    option_label[1]="Evaluation Letter";
		    option_label[2]="Both";
		    option_label[3]="";
		}
	    }

	    if(print_view_mode==MODE_BLACKBOARD_LOG) {
		option_label[0]="News and Messages";
		option_label[1]="Objectives";
		option_label[2]="Both";
		option_label[3]="";
	    }

	    if((print_view_mode==MODE_SCORE_REPORT)) {
		option_label[0]="Current Screen";
		option_label[1]="All Reports";
		option_label[2]="";
		option_label[3]="";
	    }

	    if((print_view_mode==MODE_FINANCIAL_REPORT)) {
		option_label[0]="Current Screen";
		option_label[1]="All Reports";
		option_label[2]="All Reports (Expanded)";
		option_label[3]="";
	    }

	    if(print_view_mode==MODE_PERFORMANCE_REPORT) {
		option_label[0]="Current Screen";
		option_label[1]="Summary Report";
		// ##### begin Gilbert 05/06/2001 ######//
		// option_label[2]="Departmental Report (Current Departments)";
		option_label[2]="Departmental Report (Current Department)";
		// ##### end Gilbert 05/06/2001 ######//
		option_label[3]="Departmental Report (All Departments)";
	    }

	    if( print_view_mode >= MODE_FIRST_NONDEPARTMENTAL_FIRM &&
		print_view_mode <= MODE_LAST_NONDEPARTMENTAL_FIRM ) {

		switch( FIRST_NONDEPARTMENTAL_FIRM_ID + print_view_mode - MODE_FIRST_NONDEPARTMENTAL_FIRM ) {
		case FIRM_ADMISSIONS:
		case FIRM_STADIUM:
		case FIRM_PLAY_FIELD:
		case FIRM_INDOOR_SPORTS:
		case FIRM_DEVELOPMENT:
		case FIRM_FACILITIES:
		case FIRM_LIBRARY:
		case FIRM_OLD_MAIN: {
		    option_label[0]="Current Screen";
		    option_label[1]="All Reports";
		    option_label[2]="";
		    option_label[3]="";
		}
		break;
		case FIRM_PARK_LOTS:
		case FIRM_PARK_STRUCTURE:
		case FIRM_POLICE: {
		    option_label[0]="All Reports";
		    option_label[1]="";
		    option_label[2]="";
		    option_label[3]="";
		}
		break;

		}
	    }

	    if(in_print_mode)
		for(int j=0;j<4;j++) {
		    if(option_label[j].len()>0)
			image_interface.put_back(tab1,vtab3+j*40-5,(j==(print_option-1))?"PRTMENUT":"PRTMENUO");
		    font_stone.put(tab1+70,vtab3+j*40,option_label[j].str_buf);
		}
	    font_stone.put(tab1+120,vtab4,"Ok");
	    font_stone.put(tab1+360,vtab4,"Cancel");
	}
    }

    //------- blt the back buffer to front buffer -------//
    in_print_mode=1;
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

};

int PrintMenu::detect() {
    int tab1=REPORT_X1+110;
    int tab2=REPORT_X1+210;
    int vtab1=REPORT_Y1+10;
    int vtab2=REPORT_Y1+50;
    int vtab3=REPORT_Y1+90;
    int vtab4=REPORT_Y1+300;
    int j;
    /*
      for(j=0;j<4;j++)
      {
      if(mouse.in_area(tab1+70,vtab3+j*40,tab1+670,vtab3+(j+1)*40))
      {
      for(int i=0;i<4;i++)
      font_stone.put(tab1+70,vtab3+i*40,print_menu_labels[i]);
      font_hstone.put
      (tab1+70,vtab3+j*40,print_menu_labels[j]);
      }
      }
    */
    if(mouse.single_click(tab1+120,vtab4,tab1+170,vtab4+30)) {
	font_hstone.put(tab1+120,vtab4,"Ok");
	font_stone.put(tab1+360,vtab4,"Cancel");
	printing();
	sys.set_staying_view_mode(0);
	sys.set_view_mode(MODE_NORMAL);
    }

    if(mouse.single_click(tab1+360,vtab4,tab1+420,vtab4+30)) {
	font_stone.put(tab1+120,vtab4,"Ok");
	font_hstone.put(tab1+360,vtab4,"Cancel");

	if( (print_menu.print_view_mode==MODE_WELCOME_LETTER)
	    ||  (print_menu.print_view_mode==MODE_EVALUATION_LETTER)
	    ||  (print_menu.print_view_mode==MODE_SCEN_WELCOME_LETTER)
	    ||  (print_menu.print_view_mode==MODE_SCENARIO_LETTER))
	    sys.set_view_mode(sys.save_view_mode);
	else
	    sys.set_staying_view_mode(0);
    }

    if(in_print_mode) {
	for(j=0;j<4;j++) {
	    font_stone.put(tab1+70,vtab3+j*40,option_label[j].str_buf);

	    if(mouse.single_click(tab1,vtab3+j*40,tab1+400,vtab3+j*40+33)) {
		print_option=j+1;
	    }
	}

	if(print_option>0) {
	    for(j=0;j<4;j++) {
		if(option_label[j].len()>0)
		    image_interface.put_back(tab1,vtab3+j*40-5,(j==(print_option-1))?"PRTMENUT":"PRTMENUO");
		if(print_option==j+1)
		    font_hstone.put(tab1+70,vtab3+j*40,option_label[j].str_buf);
	    }
	}
    }

    return 0;
};

void PrintMenu::printing() {
    // change font for printing
    font_chartsm.init("CHRP", 1);
    font_chart_red_sm.init("RCSP", 1);
    font_chart_green_sm.init("GCSP", 1);
    font_chart_blue_sm.init("BCSP", 1);
    font_chart_purple_sm.init("PCSP", 1);

    printing1();
    printing2();
    printing3();
    printing4();
    printing5();
    printing6();
    printing7();
    in_print_mode=0;

    // restore font for printing
    font_chartsm.init("CHRM", 1);
    font_chart_red_sm.init("RCSM", 1);
    font_chart_green_sm.init("GCSM", 1);
    font_chart_blue_sm.init("BCSM", 1);
    font_chart_purple_sm.init("PCSM", 1);
};

//----- Begin of function PrintMenu::print_header ------//

void PrintMenu::print_header() {
    print.start_print(sys.main_hwnd,sys.app_hinstance);
    print.open_page();

    print.dfont(18,false,false,700);
    print.put_text("Virtual U Facilities Management Report",17,1);
    print.dfont(10,false,false,0);

    print.put_text("Insitution : ",30,4);
    print.put_text(player_school.school_name,44,4); //Player Institution
    print.put_text("President : ",30,5);
    String str;
    str =  player_school.player_first_name;
    str += " ";
    str += player_school.player_last_name;
    print.put_text(str.str_buf,44,5);               //President
    print.put_text("Date : ",99,5);
    // print.put_text(date.date_str(info.game_date),107,5);                   //date
    print.put_text(info.date_str_julian(info.game_date), 107,5 );

    if(player_school.scenario_id>0) {
	print.dfont(10,false,false,700);
	print.put_text(scn_name[player_school.scenario_id-1],50,6);
    }

    /*
      print.start_print(sys.main_hwnd,sys.app_hinstance);
      print.open_page();

      print.dfont(18,false,false,700);
      print.put_text("Virtual U Department Faculty Report",17,1);
      print.dfont(10,false,false,0);

      print.put_text("Insitution : ",20,4);
      print.put_text(player_school.school_name,33,4);               //Player Institution
      print.put_text("President : ",20,5);
      String str;
      str =  player_school.player_first_name;
      str += " ";
      str += player_school.player_last_name;
      print.put_text(str.str_buf,33,5);                           //President
      print.put_text("Date : ",70,5);
      // print.put_text(date.date_str(info.game_date),77,5);                   //date
      print.put_text(info.date_str_julian(info.game_date), 77,5 );

      if(player_school.scenario_id>0)
      {
      print.dfont(10,false,false,700);
      print.put_text("Scenario : ",20,6);
      print.put_text(scn_name[player_school.scenario_id-1],31,6);
      }
    */
}

//----- End of function PrintMenu::print_header ------//
