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

//-------- Define scenario name for printing ----------//

char* PrintMenu::scn_name[SCENARIO_COUNT] = {
  "Scenario 1.  -- PAY BETTER",
  "Scenario 2.  -- ALLOCATE NEW MONEY",
  "Scenario 3.  -- TEACH BETTER",
  "Scenario 4.  -- IMPROVE RESEARCH PERFORMANCE",
  "Scenario 5.  -- WIN GAMES",
  "Scenario 6   -- REDUCE TUITION",
  "Scenario 7.  -- RESPOND TO ENROLLMENT SHIFTS",
  "Scenario 8.  -- ENROLL MORE MINORITY STUDENTS",
  "Scenario 9.  -- HIRE MORE MINORITY FACULTY MEMBERS",
  "Scenario 10. -- BALANCE THE BUDGET",
};

//------- Define static vars -----------//

static int filename_count=0;
static String fn;

//------- Begin of function PrintMenu::printing1 ------//

void PrintMenu::printing1() {
  box.tell("Now printing. \nPlease wait...");
  sys.no_true_output_flag=1;
  box.close();
  sys.no_true_output_flag=0;

  //////////////////////////////////////////////////////////////////////////////////
  /// ----- STUDENT REPORT ------- //
  //////////////////////////////////////////////////////////////////////////////////
  if(print_view_mode==MODE_STUDENT_REPORT) {
    enum {
      REPORT_TYPE_DEPARTMENT_PROFILE1, REPORT_TYPE_DEPARTMENT_PROFILE2,
      REPORT_TYPE_DEPARTMENT_ACTIVITIES,
      REPORT_TYPE_TURNOVER,
      REPORT_TYPE_INSTITUTION_PROFILE1, REPORT_TYPE_INSTITUTION_PROFILE2,
      REPORT_TYPE_INSTITUTION_ACTIVITIES, REPORT_TYPE_INSTITUTION_UNDERGRADUATE,
      REPORT_TYPE_FINANCE,
      REPORT_TYPE_RESIDENCE_HALL,
    };
    enum { REPORT_MODE_INSTITUTION, REPORT_MODE_DEPARTMENT}; {
      MouseDispCount mdc;
      print.start_print(sys.main_hwnd,sys.app_hinstance);
      print.open_page();

      print.dfont(18,false,false,700);
      print.put_text("Virtual U Department Student Report",17,1);
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
                                                  //date
      print.put_text(info.date_str_julian(info.game_date),107,5);

      if(player_school.scenario_id>0) {
        print.dfont(10,false,false,700);
        print.put_text(scn_name[player_school.scenario_id-1],50,6);
      }

      if(print_option==1) {
        int deptRecno = department_array.selected_recno;
        Department* cur_dept_ptr = department_array[deptRecno];
        String str1;
        str1 ="Department : ";
        str1 +=cur_dept_ptr->name();
        str1 +="                                                                                                                              ";
        print.dfont(12,false,false,0);
        print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
        print.dfont(12,false,true,0);
        print.put_text(str1.str_buf,20,7);

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;
        sys.view_mode=MODE_STUDENT_REPORT;
        player_school.summary_student_report_deinit();
        player_school.summary_student_report(INFO_REPAINT);

        // ##### begin Gilbert 07/07/2001 #####//
        //First page printing
        //*********************start printing********************

        //this will print the bmp in the center of the paper and 1/7 down the page
        print.put_vga_back_center("prn001.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-63,
          0.2,ZOOM_X1,ZOOM_Y1
        /*upper left cor. of bitmap*/
          );
        // ##### end Gilbert 07/07/2001 #####//

        // ####### begin Gilbert 27/04/2001 #####//
        // save memory
        player_school.summary_student_report_deinit();
        // ####### end Gilbert 27/04/2001 #####//

        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
      }

      if(print_option==2) {
        int deptRecno = department_array.selected_recno;
        Department* cur_dept_ptr = department_array[deptRecno];
        String str1;
        str1 ="Department : ";
        str1 +=cur_dept_ptr->name();
        str1 +="                                                                                                                              ";
        print.dfont(12,false,false,0);
        print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
        print.dfont(12,false,true,0);
        print.put_text(str1.str_buf,20,7);

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;

        sys.view_mode=MODE_STUDENT_REPORT;

        int report_type_backup=player_school.current_report_type;
        int report_mode_backup=player_school.current_report_mode;
        player_school.current_report_mode=REPORT_MODE_DEPARTMENT;

        player_school.summary_student_report_deinit();
        player_school.current_report_type=REPORT_TYPE_DEPARTMENT_PROFILE1;
        player_school.summary_student_report(INFO_REPAINT);
        print.put_vga_back_center("prn001.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
          0.15,ZOOM_X1,ZOOM_Y1
          );

        player_school.summary_student_report_deinit();
        player_school.current_report_type=REPORT_TYPE_DEPARTMENT_PROFILE2;
        player_school.summary_student_report(INFO_REPAINT);
        print.put_vga_back_center("prn002.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
          0.4,ZOOM_X1,ZOOM_Y1
          );

        player_school.summary_student_report_deinit();
        player_school.current_report_type=REPORT_TYPE_DEPARTMENT_ACTIVITIES;
        player_school.summary_student_report(INFO_REPAINT);
        print.put_vga_back_center("prn003.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.65,ZOOM_X1,ZOOM_Y1
          );

        // ####### begin Gilbert 27/04/2001 #####//
        // save memory
        player_school.summary_student_report_deinit();
        // ####### end Gilbert 27/04/2001 #####//

        player_school.current_report_type=report_type_backup;
        player_school.current_report_mode=report_mode_backup;
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
        // ####### begin Gilbert 27/04/2001 #####//
        // save memory
        // player_school.summary_student_report(INFO_REPAINT);
        // ####### end Gilbert 27/04/2001 #####//
      }

      if(print_option==3) {
        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;

        sys.view_mode=MODE_STUDENT_REPORT;
        int report_type_backup=player_school.current_report_type;
        int report_mode_backup=player_school.current_report_mode;
        int report_dept_backup=department_array.selected_recno;
        player_school.current_report_mode=REPORT_MODE_DEPARTMENT;

        filename_count=0;
        for(int deptRecno=1;deptRecno <= department_array.size();deptRecno++) {
          if( department_array.is_deleted(deptRecno))
            continue;

          Department* cur_dept_ptr = department_array[deptRecno];
          String str1;
          str1 ="Department : ";
          str1 +=cur_dept_ptr->name();
          str1 +="                                                                                                                              ";
          print.dfont(12,false,false,0);
          print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
          print.dfont(12,false,true,0);
          print.put_text(str1.str_buf,20,7);

          player_school.summary_student_report_deinit();
          department_array.selected_recno=deptRecno;
          player_school.current_report_type=REPORT_TYPE_DEPARTMENT_PROFILE1;
          player_school.summary_student_report(INFO_REPAINT);

          //					fn="prn";
          //					fn+=++filename_count;
          //					fn+=".bmp";
          //					print.put_vga_back_center(fn.str_buf,
          print.put_vga_back_center("prn001.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
            0.15,ZOOM_X1,ZOOM_Y1
            );

          player_school.summary_student_report_deinit();
          department_array.selected_recno=deptRecno;
          player_school.current_report_type=REPORT_TYPE_DEPARTMENT_PROFILE2;
          player_school.summary_student_report(INFO_REPAINT);

          /*					fn="prn";
                    fn+=++filename_count;
                    fn+=".bmp";
                    print.put_vga_back_center(fn.str_buf,
          */
          print.put_vga_back_center("prn002.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
            0.4,ZOOM_X1,ZOOM_Y1
            );

          player_school.summary_student_report_deinit();
          department_array.selected_recno=deptRecno;
          player_school.current_report_type=REPORT_TYPE_DEPARTMENT_ACTIVITIES;
          player_school.summary_student_report(INFO_REPAINT);

          /*					fn="prn";
                    fn+=++filename_count;
                    fn+=".bmp";
                    print.put_vga_back_center(fn.str_buf,
          */
          print.put_vga_back_center("prn003.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
            0.65,ZOOM_X1,ZOOM_Y1
            );
          if(deptRecno < department_array.size()) {
            print.end_page();
            print.open_page();
          }
        }

        // ####### begin Gilbert 27/04/2001 #####//
        // save memory
        player_school.summary_student_report_deinit();
        // ####### end Gilbert 27/04/2001 #####//

        player_school.current_report_type=report_type_backup;
        player_school.current_report_mode=report_mode_backup;
        department_array.selected_recno=report_dept_backup;
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
        // ####### begin Gilbert 27/04/2001 #####//
        // save memory
        // player_school.summary_student_report(INFO_REPAINT);
        // ####### end Gilbert 27/04/2001 #####//
      }

      if(print_option==4) {
        int deptRecno = department_array.selected_recno;
        Department* cur_dept_ptr = department_array[deptRecno];
        String str1;
        //				str1 ="Department : ";
        //				str1 +=cur_dept_ptr->name();
        //				str1 +="                                                                                                                              ";
        //				print.dfont(12,false,false,0);
        //				print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
        //				print.dfont(12,false,true,0);
        //				print.put_text(str1.str_buf,20,7);

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;

        sys.view_mode=MODE_STUDENT_REPORT;
        int report_type_backup=player_school.current_report_type;

        player_school.summary_student_report_deinit();
        player_school.current_report_type=REPORT_TYPE_INSTITUTION_PROFILE1;
        player_school.summary_student_report(INFO_REPAINT);
        print.put_vga_back_center("prn001.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
          0.15,ZOOM_X1,ZOOM_Y1
          );

        player_school.summary_student_report_deinit();
        player_school.current_report_type=REPORT_TYPE_INSTITUTION_PROFILE2;
        player_school.summary_student_report(INFO_REPAINT);
        print.put_vga_back_center("prn002.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
          0.4,ZOOM_X1,ZOOM_Y1
          );

        player_school.summary_student_report_deinit();
        player_school.current_report_type=REPORT_TYPE_INSTITUTION_ACTIVITIES;
        player_school.summary_student_report(INFO_REPAINT);
        print.put_vga_back_center("prn003.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.65,ZOOM_X1,ZOOM_Y1
          );

        print.end_page();
        print.open_page();

        player_school.summary_student_report_deinit();
        player_school.current_report_type=REPORT_TYPE_INSTITUTION_UNDERGRADUATE;
        player_school.summary_student_report(INFO_REPAINT);
        print.put_vga_back_center("prn004.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
          0.05,ZOOM_X1,ZOOM_Y1
          );

        player_school.summary_student_report_deinit();
        player_school.current_report_type=REPORT_TYPE_FINANCE;
        player_school.summary_student_report(INFO_REPAINT);
        print.put_vga_back_center("prn005.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.3,ZOOM_X1,ZOOM_Y1
          );

        player_school.summary_student_report_deinit();
        player_school.current_report_type=REPORT_TYPE_RESIDENCE_HALL;
        player_school.summary_student_report(INFO_REPAINT);
        print.put_vga_back_center("prn006.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.55,ZOOM_X1,ZOOM_Y1
          );

        print.end_page();
        print.open_page();

        player_school.summary_student_report_deinit();
        player_school.current_report_type=REPORT_TYPE_TURNOVER;
        player_school.summary_student_report(INFO_REPAINT);
        print.put_vga_back_center("prn007.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.05,ZOOM_X1,ZOOM_Y1
          );

        // ####### begin Gilbert 27/04/2001 #####//
        // save memory
        player_school.summary_student_report_deinit();
        // ####### end Gilbert 27/04/2001 #####//

        player_school.current_report_type=report_type_backup;
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;

        // ####### begin Gilbert 27/04/2001 #####//
        // save memory
        // player_school.summary_student_report(INFO_REPAINT);
        // ####### end Gilbert 27/04/2001 #####//
      }

      print.end_page();
      print.endprint(sys.main_hwnd);
    }
  }
  //////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////////
  /// ----- FACULTY SUMMARY-- //
  //////////////////////////////////////////////////////////////////////////////////
  if(print_view_mode==MODE_FACULTY_SUMMARY) {
    enum {
      REPORT_TYPE_DEPARTMENT_PROFILE1,
      REPORT_TYPE_DEPARTMENT_PROFILE2,
      REPORT_TYPE_DEPARTMENT_ACTIVITIES,
      REPORT_TYPE_DEPARTMENT_RESEARCH,
      REPORT_TYPE_DEPARTMENT_HIRING,
      REPORT_TYPE_DEPARTMENT_DIRECTORY,
      REPORT_TYPE_DEPARTMENT_TURNOVER,
      REPORT_TYPE_INSTITUTION_PROFILE1,
      REPORT_TYPE_INSTITUTION_PROFILE2,
      REPORT_TYPE_INSTITUTION_ACTIVITIES,
      REPORT_TYPE_INSTITUTION_RESEARCH,
      REPORT_TYPE_INSTITUTION_SALARY,
      REPORT_TYPE_INSTITUTION_PROMOTION,
      REPORT_TYPE_INSTITUTION_TURNOVER,
      REPORT_TYPE_DEPARTMENT_TURNOVER_DETAIL,
    };
	{
      MouseDispCount mdc;
      print.start_print(sys.main_hwnd,sys.app_hinstance);
      print.open_page();

      print.dfont(18,false,false,700);
      print.put_text("Virtual U Department Faculty Report",17,1);
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
      print.put_text(info.date_str_julian(info.game_date), 107,5);

      if(player_school.scenario_id>0) {
        print.dfont(10,false,false,700);
        print.put_text(scn_name[player_school.scenario_id-1],50,6);
      }

      if(print_option==1) {
        int deptRecno = department_array.selected_recno;
        Department* cur_dept_ptr = department_array[deptRecno];
        String str1;
        str1 ="Department : ";
        str1 +=cur_dept_ptr->name();
        str1 +="                                                                                                                              ";
        print.dfont(12,false,false,0);
        print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
        print.dfont(12,false,true,0);
        print.put_text(str1.str_buf,20,7);

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;
        sys.view_mode=MODE_FACULTY_SUMMARY;
        faculty_res.summary_report_deinit();
        faculty_res.summary_report(INFO_REPAINT);

        //First page printing
        //*********************start printing********************
        //this will print the bmp in the center of the paper and 1/7 down the page
        print.put_vga_back_center("prn001.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-63,
          0.2,ZOOM_X1,ZOOM_Y1
        /*upper left cor. of bitmap*/
          );

        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call summary_report_deinit instead
        faculty_res.summary_report_deinit();
        // ##### end Gilbert 27/04/2001 ######//

        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
      }

      if(print_option==2) {
        int deptRecno = department_array.selected_recno;
        Department* cur_dept_ptr = department_array[deptRecno];
        String str1;
        str1 ="Department : ";
        str1 +=cur_dept_ptr->name();
        str1 +="                                                                                                                              ";
        print.dfont(12,false,false,0);
        print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
        print.dfont(12,false,true,0);
        print.put_text(str1.str_buf,20,7);

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;

        sys.view_mode=MODE_FACULTY_SUMMARY;
        int report_type_backup=faculty_res.current_report_type;

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_PROFILE1;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn002.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
          0.15,ZOOM_X1,ZOOM_Y1
          );

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_PROFILE2;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn002.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
          0.4,ZOOM_X1,ZOOM_Y1
          );

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_ACTIVITIES;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn003.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.65,ZOOM_X1,ZOOM_Y1
          );

        print.end_page();
        print.open_page();

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_RESEARCH;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn004.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.15,ZOOM_X1,ZOOM_Y1
          );

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_HIRING;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn005.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.45,ZOOM_X1,ZOOM_Y1
          );

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_TURNOVER;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn006.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.7,ZOOM_X1,ZOOM_Y1
          );

        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call summary_report_deinit instead
        faculty_res.summary_report_deinit();
        // ##### end Gilbert 27/04/2001 ######//

        faculty_res.current_report_type=report_type_backup;
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
        // ##### begin Gilbert 20/04/2001 ######//
        // have memory not freed, call summary_report_deinit instead
        // faculty_res.summary_report(INFO_REPAINT);
        // ##### end Gilbert 20/04/2001 ######//
      }

      if(print_option==3) {
        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;

        sys.view_mode=MODE_FACULTY_SUMMARY;
        int report_type_backup=faculty_res.current_report_type;
        int report_dept_backup=department_array.selected_recno;
        filename_count=1;
        for(int deptRecno=1;deptRecno <= department_array.size();deptRecno++) {
          if( department_array.is_deleted(deptRecno))
            continue;

          department_array.selected_recno = deptRecno;
          Department* cur_dept_ptr = department_array[deptRecno];
          String str1;
          str1 ="Department : ";
          str1 +=cur_dept_ptr->name();
          str1 +="                                                                                                                              ";
          print.dfont(12,false,false,0);
          print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
          print.dfont(12,false,true,0);
          print.put_text(str1.str_buf,20,7);

          faculty_res.summary_report_deinit();
          faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_PROFILE1;
          faculty_res.summary_report(INFO_REPAINT);
          fn="prn";
          fn+=++filename_count;
          fn+=".bmp";
          print.put_vga_back_center(fn.str_buf,
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
            0.15,ZOOM_X1,ZOOM_Y1
            );

          faculty_res.summary_report_deinit();
          faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_PROFILE2;
          faculty_res.summary_report(INFO_REPAINT);
          fn="prn";
          fn+=++filename_count;
          fn+=".bmp";
          print.put_vga_back_center(fn.str_buf,
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
            0.4,ZOOM_X1,ZOOM_Y1
            );

          faculty_res.summary_report_deinit();
          faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_ACTIVITIES;
          faculty_res.summary_report(INFO_REPAINT);
          fn="prn";
          fn+=++filename_count;
          fn+=".bmp";
          print.put_vga_back_center(fn.str_buf,
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
            0.65,ZOOM_X1,ZOOM_Y1
            );

          print.end_page();
          print.open_page();

          faculty_res.summary_report_deinit();
          faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_RESEARCH;
          faculty_res.summary_report(INFO_REPAINT);
          fn="prn";
          fn+=++filename_count;
          fn+=".bmp";
          print.put_vga_back_center(fn.str_buf,
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
            0.15,ZOOM_X1,ZOOM_Y1
            );

          faculty_res.summary_report_deinit();
          faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_HIRING;
          faculty_res.summary_report(INFO_REPAINT);
          fn="prn";
          fn+=++filename_count;
          fn+=".bmp";
          print.put_vga_back_center(fn.str_buf,
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
            0.45,ZOOM_X1,ZOOM_Y1
            );

          faculty_res.summary_report_deinit();
          faculty_res.current_report_type=REPORT_TYPE_DEPARTMENT_TURNOVER;
          faculty_res.summary_report(INFO_REPAINT);
          fn="prn";
          fn+=++filename_count;
          fn+=".bmp";
          print.put_vga_back_center(fn.str_buf,
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
            0.7,ZOOM_X1,ZOOM_Y1
            );

          if(deptRecno < department_array.size()) {
            print.end_page();
            print.open_page();
          }
        }

        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call summary_report_deinit instead
        faculty_res.summary_report_deinit();
        // ##### end Gilbert 27/04/2001 ######//

        faculty_res.current_report_type=report_type_backup;
        department_array.selected_recno=report_dept_backup;
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
        // ##### begin Gilbert 20/04/2001 ######//
        // have memory not freed, call summary_report_deinit instead
        // faculty_res.summary_report(INFO_REPAINT);
        // ##### end Gilbert 20/04/2001 ######//
      }

      if(print_option==4) {
        int deptRecno = department_array.selected_recno;
        Department* cur_dept_ptr = department_array[deptRecno];
        String str1;
        // ###### begin Gilbert 25/04/2001 #######//
        // no department name for instituion report
        //str1 ="Department : ";
        //str1 +=cur_dept_ptr->name();
        //str1 +="                                                                                                                              ";
        //print.dfont(12,false,false,0);
        //print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
        //print.dfont(12,false,true,0);
        //print.put_text(str1.str_buf,20,7);
        // ###### end Gilbert 25/04/2001 #######//

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;

        sys.view_mode=MODE_FACULTY_SUMMARY;
        int report_type_backup=faculty_res.current_report_type;

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_INSTITUTION_PROFILE1;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn001.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
          0.15,ZOOM_X1,ZOOM_Y1
          );

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_INSTITUTION_PROFILE2;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn002.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-70,
          0.4,ZOOM_X1,ZOOM_Y1
          );

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_INSTITUTION_ACTIVITIES;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn003.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.65,ZOOM_X1,ZOOM_Y1
          );

        print.end_page();
        print.open_page();

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_INSTITUTION_RESEARCH;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn004.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.15,ZOOM_X1,ZOOM_Y1
          );

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_INSTITUTION_SALARY;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn005.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.4,ZOOM_X1,ZOOM_Y1
          );

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_INSTITUTION_PROMOTION;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn006.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.65,ZOOM_X1,ZOOM_Y1
          );

        print.end_page();
        print.open_page();

        faculty_res.summary_report_deinit();
        faculty_res.current_report_type=REPORT_TYPE_INSTITUTION_TURNOVER;
        faculty_res.summary_report(INFO_REPAINT);
        print.put_vga_back_center("prn007.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.05,ZOOM_X1,ZOOM_Y1
          );

        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call summary_report_deinit instead
        faculty_res.summary_report_deinit();
        // ##### end Gilbert 27/04/2001 ######//

        faculty_res.current_report_type=report_type_backup;
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call summary_report_deinit instead
        // faculty_res.summary_report(INFO_REPAINT);
        // ##### end Gilbert 27/04/2001 ######//
      }

      print.end_page();
      print.endprint(sys.main_hwnd);
    }
  }
  //////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  /// ----- COURSE REPORT-- //
  //////////////////////////////////////////////////////////////////////////////////
  if(print_view_mode==MODE_COURSE_REPORT) {
    enum {
      REPORT_TYPE_DEPARTMENT_OVERVIEW, REPORT_TYPE_DEPARTMENT_CLASSTYPES,
      REPORT_TYPE_DEPARTMENT_TECHNOLOGY,
      REPORT_TYPE_INSTITUTION_OVERVIEW, REPORT_TYPE_INSTITUTION_SUMMERTEACHING,
      REPORT_TYPE_INSTITUTION_TECHNOLOGY,
	};
    
	{
      MouseDispCount mdc;

      //			print_header();

      if(print_option==1) {
        print.dfont(18,false,false,700);
        print.put_text("Virtual U Department Course Report",17,1);
        print.dfont(10,false,false,0);

        int deptRecno = department_array.selected_recno;
        Department* cur_dept_ptr = department_array[deptRecno];
        String str1;
        str1 ="Department : ";
        str1 +=cur_dept_ptr->name();
        str1 +="                                                                                                                              ";
        print.dfont(12,false,false,0);
        print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
        print.dfont(12,false,true,0);
        print.put_text(str1.str_buf,20,7);

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;

        sys.view_mode=MODE_COURSE_REPORT;

        cur_dept_ptr->course_report_deinit();
        cur_dept_ptr->course_report(INFO_REPAINT);

        //First page printing
        //*********************start printing********************

        //this will print the bmp in the center of the paper and 1/7 down the page
        print.put_vga_back_center("prn001.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.2,ZOOM_X1,ZOOM_Y1
        /*upper left cor. of bitmap*/
          );

        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call course_report_deinit instead
        cur_dept_ptr->course_report_deinit();
        // ##### end Gilbert 27/04/2001 ######//

        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
      }

      if(print_option==2) {
        print.dfont(18,false,false,700);
        print.put_text("Virtual U Department Course Report",17,1);
        print.dfont(10,false,false,0);

        int deptRecno = department_array.selected_recno;
        Department* cur_dept_ptr = department_array[deptRecno];
        String str1;
        str1 ="Department : ";
        str1 +=cur_dept_ptr->name();
        str1 +="                                                                                                                              ";
        print.dfont(12,false,false,0);
        print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
        print.dfont(12,false,true,0);
        print.put_text(str1.str_buf,20,7);

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;

        sys.view_mode=MODE_COURSE_REPORT;
        int report_type_backup=department_array.current_report_type;

        cur_dept_ptr->course_report_deinit();
        department_array.current_report_type=REPORT_TYPE_DEPARTMENT_OVERVIEW;
        cur_dept_ptr->course_report(INFO_REPAINT);
        print.put_vga_back_center("prn002.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.2,ZOOM_X1,ZOOM_Y1
          );

        cur_dept_ptr->course_report_deinit();
        department_array.current_report_type=REPORT_TYPE_DEPARTMENT_CLASSTYPES;
        cur_dept_ptr->course_report(INFO_REPAINT);
        print.put_vga_back_center("prn002.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.65,ZOOM_X1,ZOOM_Y1
          );

        print.end_page();
        print.open_page();

        cur_dept_ptr->course_report_deinit();
        department_array.current_report_type=REPORT_TYPE_DEPARTMENT_TECHNOLOGY;
        cur_dept_ptr->course_report(INFO_REPAINT);
        print.put_vga_back_center("prn003.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.05,ZOOM_X1,ZOOM_Y1
          );

        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call course_report_deinit instead
        cur_dept_ptr->course_report_deinit();
        // ##### end Gilbert 27/04/2001 ######//

        department_array.current_report_type=report_type_backup;
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call course_report_deinit instead
        // cur_dept_ptr->course_report(INFO_REPAINT);
        // ##### end Gilbert 27/04/2001 ######//
      }

      if(print_option==3) {
        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;

        print.dfont(18,false,false,700);
        print.put_text("Virtual U Department Course Report",17,1);
        print.dfont(10,false,false,0);

        int deptRecno = department_array.selected_recno;
        Department* cur_dept_ptr = department_array[deptRecno];
        sys.view_mode=MODE_COURSE_REPORT;
        int report_type_backup=department_array.current_report_type;
        int report_dept_backup=department_array.selected_recno;

        filename_count=1;

        for(deptRecno=1;deptRecno <= department_array.size();deptRecno++) {
          if( department_array.is_deleted(deptRecno))
            continue;

          department_array.selected_recno=deptRecno;
          cur_dept_ptr = department_array[deptRecno];
          String str1;
          str1 ="Department : ";
          str1 +=cur_dept_ptr->name();
          str1 +="                                                                                                                              ";
          print.dfont(12,false,false,0);
          print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
          print.dfont(12,false,true,0);
          print.put_text(str1.str_buf,20,7);

          cur_dept_ptr->course_report_deinit();
          department_array.current_report_type=REPORT_TYPE_DEPARTMENT_OVERVIEW;
          cur_dept_ptr->course_report(INFO_REPAINT);
          /*					fn="prn";
                    fn+=++filename_count;
                    fn+=".bmp";
                    print.put_vga_back_center(fn.str_buf,
          */
          print.put_vga_back_center("prn001.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
            0.2,ZOOM_X1,ZOOM_Y1
            );

          cur_dept_ptr->course_report_deinit();
          department_array.current_report_type=REPORT_TYPE_DEPARTMENT_CLASSTYPES;
          cur_dept_ptr->course_report(INFO_REPAINT);
          /*					fn="prn";
                    fn+=++filename_count;
                    fn+=".bmp";
          */
          print.put_vga_back_center("prn002.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
            0.65,ZOOM_X1,ZOOM_Y1
            );

          print.end_page();
          print.open_page();

          cur_dept_ptr->course_report_deinit();
          department_array.current_report_type=REPORT_TYPE_DEPARTMENT_TECHNOLOGY;
          cur_dept_ptr->course_report(INFO_REPAINT);

          print.put_vga_back_center("prn003.bmp",
            ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
            0.05,ZOOM_X1,ZOOM_Y1
            );

          if(deptRecno < department_array.size()) {
            print.end_page();
            print.open_page();
          }
        }

        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call course_report_deinit instead
        cur_dept_ptr->course_report_deinit();
        // ##### end Gilbert 27/04/2001 ######//

        cur_dept_ptr = department_array[department_array.selected_recno];
        department_array.current_report_type=report_type_backup;
        department_array.selected_recno=report_dept_backup;
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call course_report_deinit instead
        // cur_dept_ptr->course_report(INFO_REPAINT);
        // ##### end Gilbert 27/04/2001 ######//
      }

      if(print_option==4) {
        print.dfont(18,false,false,700);
        print.put_text("Virtual U Institutional Course Report",17,1);
        print.dfont(10,false,false,0);

        int deptRecno = department_array.selected_recno;
        Department* cur_dept_ptr = department_array[deptRecno];
        /*				String str1;
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

        sys.view_mode=MODE_COURSE_REPORT;
        int report_type_backup=department_array.current_report_type;

        cur_dept_ptr->course_report_deinit();
        department_array.current_report_type=REPORT_TYPE_INSTITUTION_OVERVIEW;
        cur_dept_ptr->course_report(INFO_REPAINT);
        print.put_vga_back_center("prn003.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.2,ZOOM_X1,ZOOM_Y1
          );

        cur_dept_ptr->course_report_deinit();
        department_array.current_report_type=REPORT_TYPE_INSTITUTION_SUMMERTEACHING;
        cur_dept_ptr->course_report(INFO_REPAINT);

        print.put_vga_back_center("prn004.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.65,ZOOM_X1,ZOOM_Y1
          );

        print.end_page();
        print.open_page();

        cur_dept_ptr->course_report_deinit();
        department_array.current_report_type=REPORT_TYPE_INSTITUTION_TECHNOLOGY;
        cur_dept_ptr->course_report(INFO_REPAINT);

        print.put_vga_back_center("prn005.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-60,
          0.05,ZOOM_X1,ZOOM_Y1
          );

        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call course_report_deinit instead
        cur_dept_ptr->course_report_deinit();
        // ##### end Gilbert 27/04/2001 ######//

        department_array.current_report_type=report_type_backup;
        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1;
        // ##### begin Gilbert 27/04/2001 ######//
        // have memory not freed, call course_report_deinit instead
        // cur_dept_ptr->course_report(INFO_REPAINT);
        // ##### end Gilbert 27/04/2001 ######//
      }
      print.end_page();
      print.endprint(sys.main_hwnd);
    }
  }
  //////////////////////////////////////////////////////////////////////////////////
}
