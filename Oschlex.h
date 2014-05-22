//Filename   : OSCHLEX.H
//Description: Class SchoolEx Declaration
//Owner       : Fred

#ifndef __OSCHLEX_H
#define __OSCHLEX_H

#ifndef __OSCHOOL_H
#include <OSCHOOL.H>
#endif

#include <OSTUDENT.H>                             // MAX_STUDENT_LEVEL
#include <OFIELD.H>                               // IPED_FIELD_COUNT

//------- Define class SchoolFacultyEx -----//

//!Extension of SchoolFaculty including active_research_dollars per
//!faculty.
class SchoolFacultyEx : public SchoolFaculty {
public:
    int active_research_dollars;                  // ($000) _per_faculty
};

//---------- Define class SchoolEx ---------//

class SchoolDegreeRec;

//!Class for calculating and tracking school expenses. An extended version
//!of School, with SchoolDegree variables in the "Initial Conditions"
//!spreadsheet.
class SchoolEx : public School {                  // an extended version of School, with variables in the "Initial Conditions" spreadsheet.
public:
    int db_school_recno;                          // recno to db_school_array;

    // field L-R
    int operating_reserve;                        // current_funds_balance
    int endowment_market;                         // end_of_year_endowment_market_value
    int buildings;                                // year_end_market_value_of_plant
    int capital_reserve;                          // year_end_capital_reserve
    int general_plant_debt;                       // year_end_general_plant_and_residence_hall_debt
    int residence_hall_debt;                      // residence_hall_debt
    int fund_balance;                             // R=M1244+N1244+O1244-P1244-Q1244

    // field S
    int gross_tuition_revenue;                    // total_gross_tuition_revenue
    int financial_aid;                            // total_financial_aid
    int net_tuition_revenue;                      // =S-T

    int state_appropriations;                     // state_and_local_appropriations
    int sponsored_research;                       // adjusted_total_sponsored_research
    int gifts_to_operations;                      // gifts_to_operations
    int endowment_spending;                       //

    // field Z
    short athletics;                              // athletics_revenue
    int other_operating_income;                   // adjusted_other_operating_income;
    int interest_on_operating_reserve;            // = current_funds_balance * 0.08
    int total_sources_of_funds;                   // =AC=SUM(U1244:AB1244)

    // field AD
    int dept_expense_faculty_salaries;            // academic_dept_faculty_salaries
    int dept_expense_staff_salaries;              // = academic_dept_total_salaries - AD
    int dept_expense_other;                       // academic_dept_other_expense
    int sponsored_research_faculty_salaries;      // sponsored_research_faculty_salaries
    int sponsored_research_staff_salaries;        // = sponsored_research_total_salaries - AG
    int sponsored_research_other;                 // sponsored_research_other

    // field AJ
    int library_staff_salaries;                   // library_salaries
    int library_other_expense;                    // library_other_expense

    int academic_it_staff_salaries;               // = academic_support_salaries * 0.1
    int academic_it_other_expense;                // = academic_support_other_expense * 0.3

    // field AN
    int student_life_staff_salaries;              // student_life_salaries
    int student_life_other_expense;               // student_life_other_expense

    // fields already defined in base class School
    // Athletics: staff salaries	Athletics: other expense
    // int athletics_salaries;
    // int athletics_other_expense;

    // field AR
    int inst_advancement_staff_salaries;          // inst_advancement_salaries
    int inst_advancement_other_expense;           // inst_advancement_other_expense

    int administration_staff_salaries;            // inst_support_net_salaries;
    int administration_other_expense;             // inst_support_net_other_expense;

    int o_and_m_staff_salaries;                   // o_and_m_salaries
    int o_and_m_other_expense;                    // o_and_m_other_expense

    // field AX
    int other_operating_expense_staff_salaries;   // academic_support_salaries -AL
    int other_operating_expense_other_expense;    // academic_support_other_expense -AM

    int total_operating_expenditure;              // = SUM(AD1244:AY1244)

    // field BA
    int service_on_general_plant_debt;            // (year_end_general_plant_and_residence_hall_debt
    // -residence_hall_debt)*0.065
    int transfer_to_capital_reserve;              // transfer_to_plant
    int total_uses_of_funds;                      // = AZ1244+BA1244+BB1244
    int surplus_or_deficit;                       // = AC1244-BC1244

    // field BE
    int tuition_rate;                             // = freshmen_from_within_the_state_percent/100.0*in_state_tuition+(1-freshmen_from_within_the_state_percent/100.0)*out_state_tuition
    short room_and_board_rate;                    // room_and_board_charges
    // ug_in_housing_percent/1000.0
    float percent_traditional_ug_in_residence_halls;
    float percent_get_bacc_in_5_year;             // student_get_bacc_in_5_year_percent/1000.0
    char  overhead_rate_on_sponsored_research;    // overhead_rate
    float endowment_spending_rate;                // float	endowment_spending_rate;

    // field BK
    // replaced super class's school_faculty_array and will be updated
    SchoolFacultyEx school_faculty_array_ex[FACULTY_RANK_LEVEL_COUNT];

    // Enroll ft,pt_ug,etc (Enroll SL1-5)
    short full_time_undergrad;                    // = sPtr->full_time_undergrad;
    short part_time_undergrad;                    // = sPtr->part_time_undergrad;
    short enrollment_masters;                     // = sPtr->enrollment_masters;
    short enrollment_doctoral;                    // = sPtr->enrollment_doctoral;
    short non_degree_seeking;                     // = sPtr->non_degree_seeking

    // field CM - Percent Female SL1-5
    char  percent_female_sl[MAX_STUDENT_LEVEL];   // = female_undergrad_percent

    // field CR - Percent Minority SL1-5
    char  percent_minority_sl[MAX_STUDENT_LEVEL]; // = minority_undergrad_percent*0.85;

    char  percent_instate_freshmen;               // = freshmen_from_within_the_state_percent
    char  percent_non_resident_alien_doctoral;    // = non_resident_alien_grad_and_prof_percent;

    /*
      // field defined in base class School
      // field CY
      short	target_student_intake_sl[5];
      float	dropout_rate_sl[5];
      float	target_grad_rate_sl[5];
    */

    // field DN
    int ug_applications;                          // = student_applications;
    float ug_applications_rate;                   // = ug_applications/student_accepted;
    char  ug_yield_rate;                          // = float(student_enrolled)/student_accepted*100;

    //TO int the 3nd to 5th vars?
    // fields already defined in base class School
    //char	percent_ug_students_on_aid;	// DQ= percent_ug_students_on_aid;
    //float	percent_freshmen_with_need;	// = percent_freshmen_with_need;
    //float	freshmen_offered_aid;			// = freshmen_offered_aid;
    //float	percent_freshmen_offered_full_aid;	// = percent_freshmen_offered_full_aid;
    //short	institutional_aid_per_fte;		// DU

    // major_pref of sl1 = that of sl2
    float   student_ifield_pct[MAX_STUDENT_LEVEL-1][IPED_FIELD_COUNT];
    float   student_ifield_pct_total[MAX_STUDENT_LEVEL-1];

    // fields already defined in base class School
    //float		doctoral_degrees_per_faculty_rating;
    //float		sponsored_research_rating;					// it is integer (char) in class School

private:
    void  init_student_ifield_pct(SchoolDegreeRec* degRec, short sl, short len);
public:
    void  init(int schoolRecno);                  // calculate vars in SchoolEx based on vars in School
};

//--------------------------------------//
#endif
