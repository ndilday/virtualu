//Filename   : OPEERSCH.H
//Description: Class PeerSchool Declaration
//Owner:			Fred
// An extended version of SchoolEx, with variables in the "Peer_Insts" spreadsheet.
// 0601: variables of peer school to be reported have not decided yet.

#ifndef __OPEERSCH_H
#define __OPEERSCH_H

#include <OSTUDENT.H>                             // MAX_STUDENT_LEVEL
#include <OSCHLEX.H>

#include <OENROLL.H>                              // INSTITU_SEGMENT_COUNT, AppYield
#include <OFACURES.H>                             // MAX_RANK_AGE_GROUP

//---------- Define class PeerSchool ---------//

//!An extended version of SchoolEx, used for representing peer schools.
class PeerSchool : public SchoolEx {
public:
    //--- fields defined in School are commented, uncomment these all if it's not derived from School ---//
    //--- fields not found in School or master dbase are uncommented ---//

    /*
      float	applications_ratio;
      char	yield_rate;
      char	ncaa_basketball_division[8];		// the data of this column in Peer_Insts have problems
      short	in_state_tuition;						// most of in_state_tuition = out_state_tuition in Master dbase
      short room_and_board_charges;

      // Enroll ft,pt_ug,etc (Enroll SL1-5)
      short full_time_undergrad;
      short part_time_undergrad;
      short enrollment_masters;
      short enrollment_doctoral;
      short non_degree_seeking;

      char	student_get_bacc_in_5_year_percent;
      char	percent_ug_students_on_aid;
    */

    //TO: acorrding to email "re:q8: some easy questions", the following fields are temporiarily
    // and will be defined more clearerly later
    short average_aid_per_student;

    float student_faculty_ratio;
    char  long_term_adjunct_per_regular_facult_ratio;
    char  short_term_adjunct_per_regular_facult_ratio;
    char  tenure_ratio;
    // short	sponsored_research_per_reg_faculty;			// = 1000 * sPtr->sponsored_research_per_reg_faculty;

    // char	net_tuition_revenue_as_percent_of_e_and_g_expense;		// the data of this column in Peer_Insts don't match master dbase
    // float	endowment_per_student;
    char  liabilities_as_percent_of_assets;

    float game_score;

public:
    void  init(int recno);                        // calculate vars in PeerSchool based on vars in School

    //-------------------------------------------//
    //----- data and methods for enrollment -----//
    //-------------------------------------------//

public:
    float pref_vars_array[PREFERECNE_COUNT2];
    float pref_vars_array_last[PREFERECNE_COUNT2];

    static float pref_vars_average_array[PREFERECNE_COUNT2];
    static float pref_vars_average_array_last[PREFERECNE_COUNT2];

public:
    void  calc_student_pref_vars_array();
    void  calc_student_pref_vars_array_peer();
    static void calc_student_pref_vars_average_array();

    //-------------------------------------------//
    //----- data and methods for faculty salary -----//
    //-------------------------------------------//
    // not $000
    static int  average_faculty_salary[MAX_RANK_AGE_GROUP];

public:
    static void calc_average_faculty_salary();
};

//--------------------------------------//
#endif
