//Filename    : OPRTMENU2.CPP
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
#include <OADM_OFF.H>
#include <OATHLETI.H>
#include <ODEVELOP.H>
#include <OFACILIT.H>
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

void PrintMenu::printing2() {
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

      case FIRM_ADMISSIONS:
        enum {                                    // the 'sub' view-modes
          MODE_ADMISSION = 0,
          MODE_STATISTICS = 1,
          MODE_PRIORITY = 2,
          MODE_FINANCIAL_AID = 3 
        };
		{
          MouseDispCount mdc;
          print.start_print(sys.main_hwnd,sys.app_hinstance);
          print.open_page();

          print.dfont(18,false,false,700);
          print.put_text("Virtual U Admissions and Financial Aid Report",10,1);
          print.dfont(10,false,false,0);

          print.put_text("Insitution : ",30,4);
                                                  //Player Institution
          print.put_text(player_school.school_name,44,4);
          print.put_text("President : ",30,5);
          String str;
          str =  player_school.player_first_name;
          str += " ";
          str += player_school.player_last_name;
          print.put_text(str.str_buf,44,5);       //President
          print.put_text("Date : ",99,5);
          // print.put_text(date.date_str(info.game_date),107,5);                   //date
          print.put_text(info.date_str_julian(info.game_date), 107, 5 );

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
            user_interface.backgd_flag=0;
            sys.no_true_output_flag=1;

            sys.view_mode=FIRM_ADMISSIONS;
            adm_office.deinit();
            adm_office.report(INFO_REPAINT);

            //First page printing
            //*********************start printing********************

            //this will print the bmp in the center of the paper and 1/7 down the page
            print.put_vga_back_center("prn001.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-((adm_office.current_mode==MODE_STATISTICS)?100:70),
              0.2,ZOOM_X1,ZOOM_Y1
            /*upper left cor. of bitmap*/
              );

            sys.no_true_output_flag=0;
            user_interface.backgd_flag=1;
            sys.view_mode=print_view_mode;
            adm_office.deinit();
            // ##### begin Gilbert 28/04/2001 ######//
            // save memory
            // adm_office.report(INFO_REPAINT);
            // ##### end Gilbert 28/04/2001 ######//
          }

          if(print_option==2) {
            /*						int deptRecno = department_array.selected_recno;
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
            user_interface.backgd_flag=0;
            sys.no_true_output_flag=1;

            int report_type_backup=adm_office.current_mode;
            int report_submode_backup=adm_office.sub_current_mode;

            sys.view_mode=FIRM_ADMISSIONS;
            adm_office.deinit();
            adm_office.current_mode=MODE_ADMISSION;
            adm_office.report(INFO_REPAINT);
            print.put_vga_back_center("prn001.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
              0.15,ZOOM_X1,ZOOM_Y1
              );

            adm_office.deinit();
            adm_office.current_mode=MODE_STATISTICS;
            adm_office.sub_current_mode=0;
            adm_office.report(INFO_REPAINT);
            print.put_vga_back_center("prn002.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-100,
              0.4,ZOOM_X1,ZOOM_Y1
              );

            adm_office.deinit();
            adm_office.current_mode=MODE_STATISTICS;
            adm_office.sub_current_mode=1;
            adm_office.report(INFO_REPAINT);
            print.put_vga_back_center("prn003.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-100,
              0.65,ZOOM_X1,ZOOM_Y1
              );

            print.end_page();
            print.open_page();

            adm_office.deinit();
            adm_office.current_mode=MODE_STATISTICS;
            adm_office.sub_current_mode=2;
            adm_office.report(INFO_REPAINT);
            print.put_vga_back_center("prn001.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-100,
              0.15,ZOOM_X1,ZOOM_Y1
              );

            adm_office.deinit();
            adm_office.current_mode=MODE_STATISTICS;
            adm_office.sub_current_mode=3;
            adm_office.report(INFO_REPAINT);
            print.put_vga_back_center("prn002.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-100,
              0.4,ZOOM_X1,ZOOM_Y1
              );

            adm_office.deinit();
            adm_office.current_mode=MODE_STATISTICS;
            adm_office.sub_current_mode=4;
            adm_office.report(INFO_REPAINT);
            print.put_vga_back_center("prn003.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-100,
              0.65,ZOOM_X1,ZOOM_Y1
              );

            print.end_page();
            print.open_page();

            adm_office.deinit();
            adm_office.current_mode=MODE_PRIORITY;
            adm_office.report(INFO_REPAINT);
            adm_office.report(INFO_UPDATE);
            print.put_vga_back_center("prn001.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
              0.15,ZOOM_X1,ZOOM_Y1
              );

            adm_office.deinit();
            adm_office.current_mode=MODE_FINANCIAL_AID;
            adm_office.sub_current_mode=0;
            adm_office.report(INFO_REPAINT);
            adm_office.report(INFO_UPDATE);
            print.put_vga_back_center("prn002.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-57,
              0.4,ZOOM_X1,ZOOM_Y1
              );

            adm_office.deinit();
            adm_office.current_mode=report_type_backup;
            adm_office.sub_current_mode=report_submode_backup;
            sys.no_true_output_flag=0;
            user_interface.backgd_flag=1;
            sys.view_mode=print_view_mode;
            // ##### begin Gilbert 28/04/2001 ######//
            // save memory
            // adm_office.report(INFO_REPAINT);
            // ##### end Gilbert 28/04/2001 ######//
          }

          print.end_page();
          print.endprint(sys.main_hwnd);
        }
        break;

        ///////////////////////////////////////////////////////////////////////////////////////////
      case FIRM_STADIUM:
      case FIRM_PLAY_FIELD:
      case FIRM_INDOOR_SPORTS: {
        enum {                                    // the 'sub' view-modes
          MODE_ATHLETICS = 0,
          MODE_FINANCIAL = 1,
        };

        MouseDispCount mdc;
        print.start_print(sys.main_hwnd,sys.app_hinstance);
        print.open_page();

        print.dfont(18,false,false,700);
        print.put_text("Virtual U Athletics Report",22,1);
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
        print.put_text(info.date_str_julian(info.game_date),107,5);

        if(player_school.scenario_id>0) {
          print.dfont(10,false,false,700);
          print.put_text(scn_name[player_school.scenario_id-1],50,6);
        }

        if(print_option==1) {
          user_interface.backgd_flag=0;
          sys.no_true_output_flag=1;

          sys.view_mode=FIRM_INDOOR_SPORTS;

          athletics_office.deinit();
          athletics_office.report(INFO_REPAINT);

          print.put_vga_back_center("prn001.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
            0.2,ZOOM_X1,ZOOM_Y1
            );

          sys.no_true_output_flag=0;
          user_interface.backgd_flag=1;
          sys.view_mode=print_view_mode;

          athletics_office.deinit();
          // ##### begin Gilbert 28/04/2001 #####//
          // save memory
          // athletics_office.report(INFO_REPAINT);
          // ##### end Gilbert 28/04/2001 #####//
        }

        if(print_option==2) {
          user_interface.backgd_flag=0;
          sys.no_true_output_flag=1;

          int report_type_backup=athletics_office.current_mode;

          sys.view_mode=FIRM_INDOOR_SPORTS;
          athletics_office.deinit();

          athletics_office.current_mode=MODE_ATHLETICS;
          athletics_office.report(INFO_REPAINT);
          print.put_vga_back_center("prn001.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
            0.15,ZOOM_X1,ZOOM_Y1
            );

          athletics_office.deinit();
          athletics_office.current_mode=MODE_FINANCIAL;
          athletics_office.report(INFO_REPAINT);
          print.put_vga_back_center("prn002.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
            0.4,ZOOM_X1,ZOOM_Y1
            );

          athletics_office.deinit();
          athletics_office.current_mode=report_type_backup;

          sys.no_true_output_flag=0;
          user_interface.backgd_flag=1;
          sys.view_mode=print_view_mode;
          // ##### begin Gilbert 28/04/2001 #####//
          // save memory
          // athletics_office.report(INFO_REPAINT);
          // ##### end Gilbert 28/04/2001 #####//
        }

        print.end_page();
        print.endprint(sys.main_hwnd);
      }
      break;

      ///////////////////////////////////////////////////////////////////////////////////////////
      case FIRM_DEVELOPMENT: {
        enum {                                    // the 'sub' view-modes
          MODE_DEVELOPMENT = 0,
          MODE_FINANCIAL = 1,
        };

        MouseDispCount mdc;
        print.start_print(sys.main_hwnd,sys.app_hinstance);
        print.open_page();

        print.dfont(18,false,false,700);
        print.put_text("Virtual U Development and Alumni Affairs Report",10,1);
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

          sys.view_mode=FIRM_INDOOR_SPORTS;

          development_office.deinit();
          development_office.report(INFO_REPAINT);

          print.put_vga_back_center("prn001.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
            0.2,ZOOM_X1,ZOOM_Y1
            );

          sys.no_true_output_flag=0;
          user_interface.backgd_flag=1;
          sys.view_mode=print_view_mode;

          development_office.deinit();
          // ##### begin Gilbert 28/04/2001 #####//
          // save memory
          // development_office.report(INFO_REPAINT);
          // ##### end Gilbert 28/04/2001 #####//
        }

        if(print_option==2) {
          user_interface.backgd_flag=0;
          sys.no_true_output_flag=1;

          int report_type_backup=development_office.current_mode;

          sys.view_mode=FIRM_INDOOR_SPORTS;
          development_office.deinit();

          development_office.current_mode=MODE_DEVELOPMENT;
          development_office.report(INFO_REPAINT);
          print.put_vga_back_center("prn001.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
            0.15,ZOOM_X1,ZOOM_Y1
            );

          development_office.deinit();
          development_office.current_mode=MODE_FINANCIAL;
          development_office.report(INFO_REPAINT);
          print.put_vga_back_center("prn002.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
            0.4,ZOOM_X1,ZOOM_Y1
            );

          development_office.deinit();
          development_office.current_mode=report_type_backup;

          sys.no_true_output_flag=0;
          user_interface.backgd_flag=1;
          sys.view_mode=print_view_mode;
          // ##### begin Gilbert 28/04/2001 #####//
          // save memory
          // development_office.report(INFO_REPAINT);
          // ##### end Gilbert 28/04/2001 #####//
        }

        print.end_page();
        print.endprint(sys.main_hwnd);
      }
      break;

      case FIRM_FACILITIES: {

        enum {                                    // the 'sub' view-modes
          MODE_FINANCIAL = 0,
          MODE_SPACEUSAGE = 1,
          MODE_CAPITAL = 2,
          MODE_DEBT = 3, 
        };
		{
          MouseDispCount mdc;
          print.start_print(sys.main_hwnd,sys.app_hinstance);
          print.open_page();

          print.dfont(18,false,false,700);
          print.put_text("Virtual U Facilities Management Report",17,1);
          print.dfont(10,false,false,0);

          print.put_text("Insitution : ",30,4);
                                                  //Player Institution
          print.put_text(player_school.school_name,44,4);
          print.put_text("President : ",30,5);
          String str;
          str =  player_school.player_first_name;
          str += " ";
          str += player_school.player_last_name;
          print.put_text(str.str_buf,44,5);       //President
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

            sys.view_mode=FIRM_INDOOR_SPORTS;

            facility_office.deinit();
            facility_office.report(INFO_REPAINT);

            print.put_vga_back_center("prn001.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
              0.2,ZOOM_X1,ZOOM_Y1
              );

            sys.no_true_output_flag=0;
            user_interface.backgd_flag=1;
            sys.view_mode=print_view_mode;

            facility_office.deinit();
            // ##### begin Gilbert 28/04/2001 #####//
            // save memory
            // facility_office.report(INFO_REPAINT);
            // ##### end Gilbert 28/04/2001 #####//
          }

          if(print_option==2) {
            user_interface.backgd_flag=0;
            sys.no_true_output_flag=1;

            int report_type_backup=facility_office.current_mode;

            sys.view_mode=FIRM_INDOOR_SPORTS;
            facility_office.deinit();

            facility_office.current_mode= MODE_FINANCIAL;
            facility_office.report(INFO_REPAINT);
            print.put_vga_back_center("prn001.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
              0.15,ZOOM_X1,ZOOM_Y1
              );

            facility_office.deinit();
            facility_office.current_mode= MODE_SPACEUSAGE;
            facility_office.report(INFO_REPAINT);
            print.put_vga_back_center("prn002.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
              0.55,ZOOM_X1,ZOOM_Y1
              );

            print.end_page();
            print.open_page();
            facility_office.deinit();
            facility_office.current_mode= MODE_CAPITAL;
            facility_office.report(INFO_REPAINT);
            print.put_vga_back_center("prn003.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
              0.15,ZOOM_X1,ZOOM_Y1
              );
            facility_office.deinit();
            facility_office.current_mode= MODE_DEBT;
            facility_office.report(INFO_REPAINT);
            print.put_vga_back_center("prn004.bmp",
              ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-50,
              0.55,ZOOM_X1,ZOOM_Y1
              );

            facility_office.deinit();
            facility_office.current_mode=report_type_backup;

            sys.no_true_output_flag=0;
            user_interface.backgd_flag=1;
            sys.view_mode=print_view_mode;
            // ##### begin Gilbert 28/04/2001 #####//
            // save memory
            // facility_office.report(INFO_REPAINT);
            // ##### end Gilbert 28/04/2001 #####//
          }

          print.end_page();
          print.endprint(sys.main_hwnd);
        }

      }
      break;

    }
  }
  //////////////////////////////////////////////////////////////////////////////////

}
