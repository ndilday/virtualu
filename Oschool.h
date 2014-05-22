//Filename   : OSCHOOL.H
//Description: Object School Declaration
//Owner       : Fred

#ifndef __OSCHOOL_H
#define __OSCHOOL_H

#ifndef __OFACULTY_H
#include <OFACULTY.H>                             // FACULTY_RANK_LEVEL_COUNT
#endif

//-------- Define struct SchoolFaculty -------//

//!Simple structure for storing faculty information pertaining to expense
//!calculations aggregated by rank. Includes faculty count, average salary,
//!and male/female percentages for each ranking.
struct SchoolFaculty {
    short faculty_count;
    int salary;
    float female_percent;
    float minority_percent;
};

//------ Define class School -------//

//!Class representing other non-peer schools in the game, used for
//!calculating prestige, relative wealth, athletic success, and
//!percentages.
class School {
public:
    //---- institutional characteristics ----//

    char  name[48];                               //##### fred 0826 #####//
    short fice;
    char  control;
    char  carnegie;
    char  cc_sort;
    char  market_score;

    //--------------------------------------//

    char  state[2];
    short usn_wr_ranking;                         // Raw USN&WR ranking
    short ave_midpt_sat_act;
    char  freshmen_top_10_class;

    char  campus_environment;                     // = URBAN, SUBURBAN, or RURAL

    char  ncaa_football_division[8];
    char  ncaa_basketball_division[8];
    char  football_rating;
    char  basketball_rating;

    short in_state_tuition;
    short out_state_tuition;
    short room_and_board_charges;
    char  ug_in_housing_percent;

    SchoolFaculty school_faculty_array[FACULTY_RANK_LEVEL_COUNT];

    float parttime_faculty_percent;

    //----------------------------------------//
    // fred 0501

    int student_applications;
    short student_accepted;
    short student_enrolled;
    char  student_get_bacc_in_5_year_percent;

    // field 43
    short freshmen_applied_for_aid;
    float percent_freshmen_applied_or_aid;

    short freshmen_with_need;
    float percent_freshmen_with_need;

    short freshmen_offered_aid;
    float percent_freshmen_offered_aid;

    short freshmen_offered_full_aid;
    float percent_freshmen_offered_full_aid;

    char  percent_freshmen_with_fin_aid;
    char  percent_continuing_student_with_fin_aid;

    short institutional_aid_per_fte;

    //----------- revenue data -------------//
    // field 54
    int total_gross_tuition_revenue;
    int total_financial_aid;
    int total_sponsored_research;
    int adjusted_total_sponsored_research;
    int raw_endowment_spending;
    int state_and_local_appropriations;
    int total_gifts;
    int gifts_to_operations;
    short athletics_revenue;
    int adjusted_other_operating_income;
    int raw_other_operating_income;

    //----------- field 65: expense data -------------//

    int academic_dept_total_salaries;
    int academic_dept_faculty_salaries;
    int academic_dept_other_expense;

    int sponsored_research_total_salaries;
    int sponsored_research_faculty_salaries;
    int sponsored_research_other_expense;

    int library_salaries;
    int library_other_expense;

    int student_life_salaries;
    int student_life_other_expense;

    int inst_support_salaries;
    int inst_support_other_expense;

    int inst_support_net_salaries;
    int inst_support_net_other_expense;

    int inst_advancement_salaries;
    int inst_advancement_other_expense;

    int o_and_m_salaries;
    int o_and_m_other_expense;

    int athletics_salaries;
    int athletics_other_expense;

    int academic_support_salaries;
    int academic_support_other_expense;

    // field 87	// fred 0501
    int end_of_year_endowment_market_value;
    int current_funds_balance;
    int year_end_book_value_of_plant;
    int year_end_market_value_of_plant;
    int year_end_capital_reserve;
    int year_end_general_plant_and_residence_hall_debt;
    int residence_hall_debt;

    int raw_surplus;
    int adjusted_surplus;
    int transfer_to_plant;
    int amount_added_to_other_operating_income;

    // field 98
    short full_time_undergrad;
    short part_time_undergrad;
    short master_and_professional;
    short non_degree_seeking;
    char  female_undergrad_percent;
    char  female_grad_and_prof_percent;
    char  minority_undergrad_percent;
    char  minority_grad_and_prof_percent;
    char  non_resident_alien_grad_and_prof_percent;

    // field 107
    short full_time_freshmen;
    char  freshmen_from_within_the_state_percent;
    short total_bach_degrees;
    short total_masters_and_prof_degree;
    short total_doctoral_degrees;
    short total_certificates;

    // field 113
    int enrollment_fte;
    char  ug_percent;
    char  part_time_ug_percent;
    char  non_degree_seeking_percent;

    float applications_ratio;
    char  yield_rate;
    char  athletics_rating;

    int total_e_and_g_expenditure;
    char  net_tutition_reenue_as_percent_of_e_and_g_expese;

    short total_tenureline_faculty;
    float sponsored_research_per_reg_faculty;     // ($000)
    float doctoral_degrees_per_reg_faculty;

    char  overhead_rate;
    float endowment_spending_rate;

    short enrollment_masters;
    short enrollment_doctoral;

    // ---------- field 1 of umaster2 -------------

    short target_student_intake_sl[5];
    float dropout_rate_sl[5];
    float target_grad_rate_sl[5];

    // field 16
    char  percent_ug_students_on_aid;
    float endowment_per_student;
    float endowment_per_tenureline_faculty_member;
    float ratio_of_current_funds_balance_to_total_operating_expenditure;
    char  financial_condition;
    short number_of_incoming_ug_students;
    char  female_student_percent;
    char  minority_student_percent;
    char  sponsored_research_rating;
    char  doctoral_degres_per_faculty_rating;
    char  adj_freshmen_top_ten_percent_of_class;

    float apps_ratio2;
    float yield_rate2;
    char  athletics_rating2;
    char  net_tuition_revenue_as_percent_of_e_and_g_expense;
    float doc_time_to_degree;

    //---- vars for screening school records -----//

    float prestige;
    float distance_value;

    short desired_recno;

    //---- temp vars for calu distance_value -----//

    int N5,O5,P5;
    float Z5, L5, K5, M5;
    char  AC5, AH5;

private:
    //---- helper methods for calc_distance_all_school ----//

    void      calc_distance_NOPZ5();
    void      calc_distance_L5();
    void      calc_distance_K5();
    void      calc_distance_ACAH5(short sCount, School* sArray);
    void      calc_distance_prestige();
    void      calc_distance_M5();
    void      calc_distance_value();

public:
    void  calc_distance_first();
    static void calc_distance_all_school();       // for calculate prestige and distance_value
};

//----------------------------------------------//
#endif
