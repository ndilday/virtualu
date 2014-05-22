//Filename   : OSCHLREC.H
//Description: Object SchoolRec Declaration and, PeerInfo and PeerRec
//Owner       : Fred

#ifndef __OSCHLREC_H
#define __OSCHLREC_H

#ifndef __GAMEDEF_H
#include <GAMEDEF.H>
#endif

#ifndef __OFACULTY_H
#include <OFACULTY.H>
#endif

//-------- Define struct SchoolFacultyRec --------//

//!Structures contained within the school record storing SchoolFaculty data
//!for each rank of professor.
struct SchoolFacultyRec {
    char  faculty_count[8];
    char  salary[8];
    char  female_percent[8];
    char  minority_percent[8];
};

//!Structures contained within the school record storing SchoolFaculty data
//!for adjunct professors - lowest two ranks.
struct SchoolFacultyRec2 {                        // for long & short term
    char  adjunct_count[8];
    char  female_percent[8];
    char  minority_percent[8];
};

//------ Define class SchoolRec1 ------//

//!Structure containing information to be loaded into SchoolEx from
//!UMASTER1.RES.
class SchoolRec1 {
public:
    //##### fred 0826 #####//
    enum { NAME_LEN = 35 };

    // field 1 in db
    char  name[NAME_LEN];
    //##### fred 0826 #####//
    char  fice[8];
    char  control[7];
    char  carnegie[7];
    char  cc_sort[5];
    char  market_score[6];
    char  state[8];
    char  usn_wr_ranking[6];
    char  ave_midpt_sat_act[8];
    char  freshmen_top_10_class[7];

    // field 11
    char  campus_environment[10];
    char  ncaa_football_division[8];
    char  ncaa_basketball_division[8];
    char  football_rating[6];
    char  basketball_rating[6];
    char  in_state_tuition[7];
    char  out_state_tuition[7];
    char  room_and_board_charges[8];
    char  undergrad_in_housing_percent[8];

    // field 20
    SchoolFacultyRec school_faculty_rec_array[FACULTY_RANK_LEVEL_COUNT-2];
    SchoolFacultyRec2 school_faculty_rec_array2[2];

    char  parttime_faculty_percent[10];           //BUG ? fred: char parttime_faculty_percent[8]

    //----------------------------------------//

    char  student_applications[6];
    char  student_accepted[6];
    char  student_enrolled[6];
    char  student_get_bacc_in_5_year_percent[6];

    // field 43
    char  freshmen_applied_for_aid[7];
    char  percent_freshmen_applied_or_aid[10];

    char  freshmen_with_need[8];
    char  percent_freshmen_with_need[9];

    char  freshmen_offered_aid[8];
    char  percent_freshmen_offered_aid[9];

    char  freshmen_offered_full_aid[8];
    char  percent_freshmen_offered_full_aid[8];

    char  percent_freshmen_with_fin_aid[8];
    char  percent_continuing_student_with_fin_aid[8];

    char  institutional_aid_per_fte[9];

    //----------- revenue data -------------//
    // field 54
    char  total_gross_tuition_revenue[8];
    char  total_financial_aid[8];
    char  total_sponsored_research[8];
    char  adjusted_total_sponsored_research[8];
    char  raw_endowment_spending[8];
    char  state_and_local_appropriations[8];
    char  total_gifts[8];
    char  gifts_to_operations[8];
    char  athletics_revenue[8];
    char  adjusted_other_operating_income[8];
    char  raw_other_operating_income[8];

    //----------- expense data -------------//

    char  academic_dept_total_salaries[9];
    char  academic_dept_faculty_salaries[9];
    char  academic_dept_other_expense[9];

    char  sponsored_research_total_salaries[9];
    char  sponsored_research_faculty_salaries[9];
    char  sponsored_research_other_expense[9];

    char  library_salaries[10];
    char  library_other_expense[10];

    char  student_life_salaries[10];
    char  student_life_other_expense[10];

    char  inst_support_salaries[10];
    char  inst_support_other_expense[10];

    char  inst_support_net_salaries[10];
    char  inst_support_net_other_expense[10];

    char  inst_advancement_salaries[10];
    char  inst_advancement_other_expense[10];

    char  o_and_m_salaries[8];
    char  o_and_m_other_expense[8];

    char  athletics_salaries[8];
    char  athletics_other_expense[8];

    char  academic_support_salaries[10];
    char  academic_support_other_expense[10];

    // field 87	// fred 0501
    char  end_of_year_endowment_market_value[8];
    char  current_funds_balance[8];
    char  year_end_book_value_of_plant[8];
    char  year_end_market_value_of_plant[8];
    char  year_end_capital_reserve[8];
    char  year_end_general_plant_and_residence_hall_debt[8];
    char  residence_hall_debt[8];

    char  raw_surplus[8];
    char  adjusted_surplus[8];
    char  transfer_to_plant[8];
    char  amount_added_to_other_operating_income[8];

    // field 98
    char  full_time_undergrad[8];
    char  part_time_undergrad[8];
    char  master_and_professional[8];
    char  non_degree_seeking[8];
    char  female_undergrad_percent[7];
    char  female_grad_and_prof_percent[7];
    char  minority_undergrad_percent[7];
    char  minority_grad_and_prof_percent[7];
    char  non_resident_alien_grad_and_prof_percent[7];
    char  full_time_freshmen[9];
    char  freshmen_from_within_the_state_percent[9];
    char  total_bach_degrees[9];
    char  total_masters_and_prof_degree[9];
    char  total_doctoral_degrees[9];
    char  total_certificates[9];

    // field 113
    char  enrollment_fte[7];
    char  undergrad_percent[7];
    char  part_time_undergrad_percent[7];
    char  non_degree_seeking_percent[7];

    char  applications_ratio[7];
    char  yield_rate[7];
    char  athletics_rating[9];

    char  total_e_and_g_expenditure[9];
    char  net_tutition_reenue_as_percent_of_e_and_g_expese[9];

    char  total_tenureline_faculty[9];
    char  sponsored_research_per_reg_faculty[9];
    char  doctoral_degrees_per_reg_faculty[9];

    char  overhead_rate[7];
    char  endowment_spending_rate[7];

    char  enrollment_masters[8];
    char  enrollment_doctoral[8];
};

//------ Define class SchoolRec2 ------//

//!Structure containing information to be loaded into SchoolEx from
//!UMASTER2.RES.
class SchoolRec2 {
public:
    //field 1
    char  target_student_intake_sl1[7];
    char  target_student_intake_sl2[7];
    char  target_student_intake_sl3[7];
    char  target_student_intake_sl4[7];
    char  target_student_intake_sl5[7];

    char  dropout_rate_sl1[9];                    //BUG ?
    char  dropout_rate_sl2[6];
    char  dropout_rate_sl3[6];
    char  dropout_rate_sl4[6];
    char  dropout_rate_sl5[6];

    char  target_grad_rate_sl1[6];
    char  target_grad_rate_sl2[6];
    char  target_grad_rate_sl3[6];
    char  target_grad_rate_sl4[6];
    char  target_grad_rate_sl5[6];

    // field 16
    char  pct_ug_students_on_aid[7];
    char  endowment_per_student[10];
    char  endowment_per_tenureline_faculty_member[11];
    char  ratio_of_current_funds_balance_to_total_operating_expenditure[10];
    char  financial_condition[10];
    char  number_of_incoming_ug_students[10];
    char  female_student_percent[9];
    char  minority_student_percent[9];
    char  sponsored_research_rating[9];
    char  doctoral_degres_per_faculty_rating[9];
    char  adj_freshmen_top_ten_percent_of_class[9];

    char  apps_ratio2[8];
    char  yield_rate2[8];
    char  athletics_rating2[8];
    char  net_tuition_revenue_as_percent_of_e_and_g_expense[8];
    char  doc_time_to_degree[11];                 // new field added on 980623
};

//-----------------------------------------------//
//!Sturucture containing the name of the peer school as stored in the db.
struct PeerRec {
    enum { NAME_LEN = 47 };

    char  name[NAME_LEN];
};

//!Sturucture containing the name of the peer school.
class PeerInfo {
public:
    char  name[PeerRec::NAME_LEN+1];
};

//-----------------------------------------------//
#endif
