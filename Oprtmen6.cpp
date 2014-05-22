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

void PrintMenu::printing6() {
  box.tell("Now printing. \nPlease wait...");
  sys.no_true_output_flag=1;
  box.close();
  sys.no_true_output_flag=0;

  //////////////////////////////////////////////////////////////////////////////////
  /// ----- STUDENT REPORT ------- //
  //////////////////////////////////////////////////////////////////////////////////
  if(print_view_mode==MODE_SCORE_REPORT) {
    enum {
      MODE_OVERALL=0,
      MODE_RANK=1,
      MODE_OUTPUTS=2,
      MODE_GENERAL_INSTITUTIONAL_PERFORMANCE=3,
      MODE_ATTITUDES_TOWARD_THE_INSTITUTION=4,
      MODE_FINANCE=5,
      MODE_MANAGEMENT=6,
      MODE_ADMISSION=7, 
    };
	{
      MouseDispCount mdc;
      print.start_print(sys.main_hwnd,sys.app_hinstance);
      print.open_page();

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
        /*				int deptRecno = department_array.selected_recno;
                Department* cur_dept_ptr = department_array[deptRecno];
                String str1;
                str1 ="Department : ";
                str1 +=cur_dept_ptr->name();
                str1 +="                                                                                                                              ";
                print.dfont(12,false,false,0);
                print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
                print.dfont(12,false,true,0);
                print.put_text(str1.str_buf,20,7);
        */
        print.dfont(18,false,false,700);
        print.put_text("Virtual U Score Report",24,1);
        print.dfont(10,false,false,0);

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;
        sys.view_mode=MODE_SCORE_REPORT;
        player_school.score_report_deinit();
        player_school.score_report(INFO_REPAINT);
        //First page printing
        //*********************start printing********************

        //this will print the bmp in the center of the paper and 1/7 down the page
        print.put_vga_back_center("prn001.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-80,
          0.2,ZOOM_X1,ZOOM_Y1
        /*upper left cor. of bitmap*/
          );
        // ##### begin Gilbert 28/04/2001 ######?/
        // save memory
        player_school.score_report_deinit();
        // ##### end Gilbert 28/04/2001 ######?/
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
      }

      if(print_option==2) {
        /*				int deptRecno = department_array.selected_recno;
                Department* cur_dept_ptr = department_array[deptRecno];
                String str1;
                str1 ="Department : ";
                str1 +=cur_dept_ptr->name();
                str1 +="                                                                                                                              ";
                print.dfont(12,false,false,0);
                print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
                print.dfont(12,false,true,0);
                print.put_text(str1.str_buf,20,7);
        */
        print.dfont(18,false,false,700);
        print.put_text("Virtual U Score Report (Detail)",17,1);
        print.dfont(10,false,false,0);

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;

        sys.view_mode=MODE_SCORE_REPORT;
        int report_type_backup=player_school.current_score_report_type;

        player_school.current_score_report_type=MODE_OVERALL;
        player_school.score_report_deinit();
        player_school.score_report(INFO_REPAINT);
        print.put_vga_back_center("prn001.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-80,
          0.17,ZOOM_X1,ZOOM_Y1
          );

        print.dfont(12,false,true,0);
        print.put_text("Overall",25,9);

        player_school.current_score_report_type=MODE_OUTPUTS;
        player_school.score_report_deinit();
        player_school.score_report(INFO_REPAINT);
        print.put_vga_back_center("prn002.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-80,
          0.4,ZOOM_X1,ZOOM_Y1
          );

        print.dfont(12,false,true,0);
        print.put_text("Output",25,23);

        player_school.current_score_report_type=MODE_GENERAL_INSTITUTIONAL_PERFORMANCE;
        player_school.score_report_deinit();
        player_school.score_report(INFO_REPAINT);
        print.put_vga_back_center("prn003.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-80,
          0.65,ZOOM_X1,ZOOM_Y1
          );

        print.dfont(12,false,true,0);
        print.put_text("Performance Indicators",25,37);

        print.end_page();
        print.open_page();

        player_school.current_score_report_type=MODE_ATTITUDES_TOWARD_THE_INSTITUTION;
        player_school.score_report_deinit();
        player_school.score_report(INFO_REPAINT);
        print.put_vga_back_center("prn004.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-80,
          0.05,ZOOM_X1,ZOOM_Y1
          );

        print.dfont(12,false,true,0);
        print.put_text("Attitudes toward the institution",25,2);

        player_school.current_score_report_type=MODE_FINANCE;
        player_school.score_report_deinit();
        player_school.score_report(INFO_REPAINT);
        print.put_vga_back_center("prn005.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-80,
          0.3,ZOOM_X1,ZOOM_Y1
          );

        print.dfont(12,false,true,0);
        print.put_text("Financial Indicators",25,17);

        print.dfont(9,false,false,0);
        print.put_text("________________________________________________________________________________________________________________________________",15,48);
        print.put_text("Rank",35,47);
        print.put_text("University",45,47);
        print.put_text("Current trustee evaluation",90,47);

        for(int i=0;i<school_res.peer_school_count;i++) {
          int idx = school_res.peer_school_rank[i];

          PeerSchool *ps = &(school_res.peer_school_array[idx]);
          print.put_text(m.format(i+1,4),35,49+i);
          print.put_text(ps->name,45,49+i);
          print.put_text(m.format((int)ps->game_score,4),100,49+i);
        }

        print.dfont(12,false,true,0);
        print.put_text("Rank",25,34);

        player_school.current_score_report_type=report_type_backup;
        // ##### begin Gilbert 28/04/2001 ######?/
        // save memory
        player_school.score_report_deinit();
        // ##### end Gilbert 28/04/2001 ######?/
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
        // #### begin Gilbert 24/04/2001 #####//
        // repaint after MouseDispCount mdc
        // player_school.score_report(INFO_REPAINT);
        // #### end Gilbert 24/04/2001 #####//
      }

      print.end_page();
      print.endprint(sys.main_hwnd);
    }
  }
  //////////////////////////////////////////////////////////////////////////////////

}
