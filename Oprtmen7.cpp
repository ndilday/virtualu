//Filename    : OPRTMENU1.CPP
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
#include <OSCHLRES.H>
#include <OPEERSCH.H>
#include <ODEPT.H>
#include <ONEWS.H>
#include <OTASK.H>
#include <OLETTER.H>
#include <OSCEN_IF.H>
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

static int filename_count=0;
static String fn;

///////////////////////////////////////////////////
static int max_scn_page[SCENARIO_COUNT] = {
    2,2,3,3,3,3,3,3,2,3
};
static int scn_id[SCENARIO_COUNT] = {
    SCN_RAISE_SALARY,
    SCN_INVEST_ACADEMIC,
    SCN_TEACHING_QUALITY,                           //## fred0301
    SCN_RESEARCH_QUALITY,                           //## fred0301
    SCN_WINNING_ATHLETICS,
    SCN_LIMIT_TUITION_GROWTH,
    SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS,           //## fred0301
    SCN_STUDENT_DIVERSITY,
    SCN_FACULTY_DIVERSITY,
    SCN_BALANCE_BUDGET,
};

void PrintMenu::printing7() {
    box.tell("Now printing. \nPlease wait...");
    sys.no_true_output_flag=1;
    box.close();
    sys.no_true_output_flag=0;

    if(
	(print_view_mode==MODE_SCEN_WELCOME_LETTER)||
	(print_view_mode==MODE_WELCOME_LETTER)||
	(print_view_mode==MODE_EVALUATION_LETTER)||
	(print_view_mode==MODE_SCENARIO_LETTER)
	) { {
	    MouseDispCount mdc;
	    print.start_print(sys.main_hwnd,sys.app_hinstance);

	    if((print_option==1)||(print_option==3)) {
		print.open_page();
		print.dfont(18,false,false,700);
		if(player_school.scenario_id>0)
		    print.put_text("Virtual U Scenario Welcome Letters",17,1);
		else
		    print.put_text("Virtual U Welcome Letter",22,1);
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

		sys.no_true_output_flag=1;
		user_interface.backgd_flag=0;

		if(player_school.scenario_id>0) {
		    // SCENARIO WELCOME LETTER
		    for(int i=1;i<=max_scn_page[player_school.scenario_id-1];i++) {
			letter.scen_page_number=i;
			letter.deinit();
			letter.scen_welcome_letter_report(INFO_REPAINT);
			print.put_vga_back_center("prn001.bmp",
						  ZOOM_X2-ZOOM_X1-30,
						  ZOOM_Y2-ZOOM_Y1-134,
						  0.15+0.19*(i-1),
						  ZOOM_X1+7,
						  ZOOM_Y1+80
			    );
		    }
		}
		else {
		    // WELCOME LETTER
		    for(int i=1;i<=5;i++) {
			letter.welcome_page_number=i;
			letter.deinit();
			letter.welcome_letter_report(INFO_REPAINT);
			print.put_vga_back_center("prn001.bmp",
						  ZOOM_X2-ZOOM_X1-30,
						  ZOOM_Y2-ZOOM_Y1-104-((i>4)?40:0),
						  0.1+0.19*(i-1)-((i>4)?0.09:0),
						  ZOOM_X1+7,
						  ZOOM_Y1+50+((i>3)?40:0)
			    );
		    }
		}
		sys.no_true_output_flag=0;
		user_interface.backgd_flag=1;
		letter.deinit();
		print.end_page();
	    }

	    if((print_option==2)||(print_option==3)) {
		print.open_page();

		print.dfont(18,false,false,700);
		print.put_text("Virtual U Evaluation Letter",20,1);
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

		sys.no_true_output_flag=1;
		user_interface.backgd_flag=0;

		// EVALUATION LETTER
		enum {
		    SCENARIO_DESCRIPTION,
		    SCENARIO_ACHIEVEMENTS,
		    SCENARIO_EVALUATION
		};

		if(player_school.scenario_id>0) {
		    int scen_letter_page_backup;
		    scen_letter_page_backup=scenario_report.scn_current_state;

		    scenario_report.deinit();
		    scenario_report.scn_current_state=SCENARIO_DESCRIPTION;
		    scenario_report.scenario_report_report(INFO_REPAINT);
		    print.put_vga_back_center("prn001.bmp",
					      ZOOM_X2-ZOOM_X1-30,
					      ZOOM_Y2-ZOOM_Y1-114,
					      0.13,
					      ZOOM_X1+7,
					      ZOOM_Y1+60
			);

		    scenario_report.deinit();
		    scenario_report.scn_current_state=SCENARIO_ACHIEVEMENTS;
		    scenario_report.scenario_report_report(INFO_REPAINT);
		    print.put_vga_back_center("prn002.bmp",
					      ZOOM_X2-ZOOM_X1-70,
					      ZOOM_Y2-ZOOM_Y1-60,
					      0.33,
					      ZOOM_X1+27,
					      ZOOM_Y1+50
			);
		    scenario_report.scn_current_state=scen_letter_page_backup;
		}
		else {
		    for(int i=1;i<=5;i++) {
			letter.eval_page_number=i;
			letter.deinit();
			letter.eval_letter_report(INFO_REPAINT);
			print.put_vga_back_center("prn001.bmp",
						  ZOOM_X2-ZOOM_X1-30,
						  ZOOM_Y2-ZOOM_Y1-104-((i>4)?40:0),
						  0.1+0.19*(i-1)-((i>4)?0.09:0),
						  ZOOM_X1+7,
						  ZOOM_Y1+50+((i>3)?40:0)
			    );
		    }
		    /*
		      letter.eval_page_number=1;
		      letter.deinit();
		      letter.eval_letter_report(INFO_REPAINT);
		      print.put_vga_back_center("prn001.bmp",
                      ZOOM_X2-ZOOM_X1-30,
                      ZOOM_Y2-ZOOM_Y1-104,
                      0.2,
                      ZOOM_X1+7,
                      ZOOM_Y1+50
		      );
		    */
		}   
		sys.no_true_output_flag=0;
		user_interface.backgd_flag=1;
		letter.deinit();
		print.end_page();
	    }
	    print.endprint(sys.main_hwnd);
	}
    };

    /*
      case MODE_SCEN_WELCOME_LETTER:
      letter.scen_welcome_letter_report(refreshFlag);
      break;

      case MODE_WELCOME_LETTER:
      letter.welcome_letter_report(refreshFlag);
      break;

      case MODE_EVALUATION_LETTER:
      letter.eval_letter_report(refreshFlag);
      break;

      case MODE_BLACKBOARD_LOG:
      blackboard.black_log_report(refreshFlag);
      break;
    */

    if(print_view_mode==MODE_BLACKBOARD_LOG) { {
	MouseDispCount mdc;
	print.start_print(sys.main_hwnd,sys.app_hinstance);
	print.open_page();

	print.dfont(18,false,false,700);
	print.put_text("Virtual U News and Objective Log",17,1);
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

	if(news_array.size()>0)
	    if((print_option==1)||(print_option==3)) {
		user_interface.backgd_flag=0;
		sys.no_true_output_flag=1;

		sys.view_mode=MODE_BLACKBOARD_LOG;

		int vtab=12;
		print.dfont(9,false,true,700);
		print.put_text("News and Messages",25,vtab++);
		vtab++;

		print.dfont(9,false,false,0);
		for(int i=1;i<=news_array.size();i++) {
		    int line_count=font_chartsm.count_line(400,news_array[i]->msg());
		    // String str=date.date_str(news_array[i]->news_date,1,0);
		    String str=info.date_str_julian( news_array[i]->news_date, INFO_DATE_DEFAULT|INFO_DATE_SHORT_MONTH|INFO_DATE_NO_DAY );
		    print.put_text(str.str_buf,27,vtab);

		    for(int j=1;j<=line_count;j++) {
			str=font_chartsm.get_line(400,j,news_array[i]->msg());
			print.put_text(str.str_buf,40,vtab++);
			if(vtab>45) {
			    print.end_page();
			    print.open_page();
			    vtab=8;
			}
		    }
		    vtab++;
		    if(vtab>45) {
			print.end_page();
			print.open_page();
			vtab=8;
		    }
		}

		sys.no_true_output_flag=0;
		user_interface.backgd_flag=1;
		print.end_page();
	    }

	if(task_array.size()>0)
	    if((print_option==2)||(print_option==3)) {
		user_interface.backgd_flag=0;
		sys.no_true_output_flag=1;

		sys.view_mode=MODE_BLACKBOARD_LOG;

		int vtab=12;
		print.dfont(9,false,true,700);
		print.put_text("Objectives",25,vtab++);
		vtab++;

		print.dfont(9,false,false,0);
		for(int i=1;i<=task_array.size();i++) {
		    int line_count=font_chartsm.count_line(400,task_array[i]->msg());
		    // String str=date.date_str(task_array[i]->news_date,1,0);
		    String str=info.date_str_julian( news_array[i]->news_date, INFO_DATE_DEFAULT|INFO_DATE_SHORT_MONTH|INFO_DATE_NO_DAY );
		    print.put_text(str.str_buf,27,vtab);

		    for(int j=1;j<=line_count;j++) {
			str=font_chartsm.get_line(400,j,task_array[i]->msg());
			print.put_text(str.str_buf,40,vtab++);
			if(vtab>45) {
			    print.end_page();
			    print.open_page();
			    vtab=8;
			}
		    }
		    vtab++;
		    if(vtab>45) {
			print.end_page();
			print.open_page();
			vtab=8;
		    }
		}

		sys.no_true_output_flag=0;
		user_interface.backgd_flag=1;
		print.end_page();
	    }

	print.endprint(sys.main_hwnd);
    }
    }
    //////////////////////////////////////////////////////////////////////////////////
}

