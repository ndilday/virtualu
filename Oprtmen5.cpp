//Filename    : OPRTMENU5.CPP
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

void PrintMenu::printing5() {
  box.tell("Now printing. \nPlease wait...");
  sys.no_true_output_flag=1;
  box.close();
  sys.no_true_output_flag=0;

  //////////////////////////////////////////////////////////////////////////////////
  /// ----- PERFORMANCE REPORT ------- //
  //////////////////////////////////////////////////////////////////////////////////
  if(print_view_mode==MODE_PERFORMANCE_REPORT) {
    if(print_option==3) {
      int deptRecno = department_array.selected_recno;
      Department* cur_dept_ptr = department_array[deptRecno];
      cur_dept_ptr->print_report();
      return;
    }

    if(print_option==4) {
      int tmp_backup=department_array.selected_recno;
      for(int i=1;i<=department_array.size();i++) {
        if(department_array.is_deleted(i))
          continue;
        department_array.selected_recno=i;
        Department* cur_dept_ptr = department_array[i];
        cur_dept_ptr->print_report();
      }
      department_array.selected_recno=tmp_backup;
      return; {
    }

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
        enum {
          MODE_INSTITUTION  = 0,
          MODE_DEPARTMENT = 1,
          MODE_RANK     = 2,
        };

        int recno_backup=department_array.selected_recno;
        int scn_backup=department_array.current_screen_mode;
        int report_type_backup=department_array.current_pref_report_type;
        int data_mode_backup=department_array.current_data_mode;

        int deptRecno = department_array.selected_recno;

        print.dfont(18,false,false,700);
        print.put_text("Virtual U Performance Report",17,1);

        Department* cur_dept_ptr = department_array[deptRecno];
        String str1;
        str1 = "Department : ";
        str1 += cur_dept_ptr->name();
        str1 += "                                                                                                                              ";
        print.dfont(12,false,false,0);
        print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
        print.dfont(12,false,true,0);
        print.put_text(str1.str_buf,20,7);

        user_interface.backgd_flag=0;
        sys.no_true_output_flag=1;
        sys.view_mode=MODE_PERFORMANCE_REPORT; {
        //				if(department_array.current_data_mode!=MODE_RANK)
                                                  // chwg072699
          department_array[department_array.selected_recno]->set_report_init_flag(1);
          department_array[department_array.selected_recno]->main_report_deinit();
          department_array[department_array.selected_recno]->main_report(INFO_REPAINT);
        }

        //First page printing
        //*********************start printing********************
        //this will print the bmp in the center of the paper and 1/7 down the page
        print.put_vga_back_center("prn001.bmp",
          ZOOM_X2-ZOOM_X1,ZOOM_Y2-ZOOM_Y1-33,
          0.2,ZOOM_X1,ZOOM_Y1
        /*upper left cor. of bitmap*/
          );

        sys.no_true_output_flag=0;
        user_interface.backgd_flag=1; {

        //				if(department_array.current_data_mode!=MODE_RANK)
                                                  // chwg072699
          department_array[department_array.selected_recno]->set_report_init_flag(1);
          department_array[department_array.selected_recno]->main_report_deinit();
        }
        department_array.selected_recno=recno_backup;
        department_array.current_screen_mode=scn_backup;
        department_array.current_pref_report_type=report_type_backup;
        department_array.current_data_mode=data_mode_backup;

      }

      if(print_option==2) {
        /*				String str1;
                str1 = "Department : ";
                str1 += cur_dept_ptr->name();
                str1 += "                                                                                                                              ";
                print.dfont(12,false,false,0);
                print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",20,6);
                print.dfont(12,false,true,0);
                print.put_text(str1.str_buf,20,7);
        */

        print.dfont(18,false,false,700);
        print.put_text("Virtual U Departmental Performance Summary Report",7,1);

        print.dfont(12,false,false,0);
        print.put_text("________________________________________________________________________________________________________________________________________________________________________________________________",15,10);

        print.dfont(10,false,false,0);

        print.put_text("Faculty",100,10);
        print.put_text("Faculty",130,10);

        print.put_text("Educational",55,11);
        print.put_text("Departmental",70,11);
        print.put_text("Student",85,11);
        print.put_text("Research",100,11);
        print.put_text("Faculty",115,11);
        print.put_text("Diversity",130,11);

        print.put_text("Department",20,12);
        print.put_text("Quality",55,12);
        print.put_text("Prestige",70,12);
        print.put_text("Morale",85,12);
        print.put_text("Performance",100,12);
        print.put_text("Morale",115,12);
        print.put_text("Index",130,12);

        print.dfont(10,false,false,0);

        for(int i=1;i<=department_array.size();i++) {
          if(department_array.is_deleted(i))
            continue;
          Department* cur_dept_ptr = department_array[i];

          print.put_text(cur_dept_ptr->name(),18,13+i);
          print.put_text(m.format(cur_dept_ptr->p_educational_quality_history[HISTORY_TRIMESTER_COUNT-1],1),58,13+i);
          print.put_text(m.format(cur_dept_ptr->p_academic_standing_history[HISTORY_YEAR_COUNT-1],1),73,13+i);
          print.put_text(m.format(cur_dept_ptr->p_student_morale_history[HISTORY_TRIMESTER_COUNT-1],1),86,13+i);
          print.put_text(m.format(cur_dept_ptr->p_faculty_research_history[HISTORY_MONTH_COUNT-1],1),102,13+i);
          print.put_text(m.format(cur_dept_ptr->p_faculty_morale_history[HISTORY_MONTH_COUNT-1],1),117,13+i);
          print.put_text(m.format(cur_dept_ptr->p_faculty_diversity_history[HISTORY_YEAR_COUNT-1],1),132,13+i);
        }
      }
      print.end_page();
      print.endprint(sys.main_hwnd);
    }
  }
}
