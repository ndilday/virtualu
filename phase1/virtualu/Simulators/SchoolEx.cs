using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Simulators
{
    /// <summary>
    /// Major preferences of students as defined by initial conditions spreadsheet.
    /// </summary>
    class SchoolDegreeSLRec
    {
        public string agriculture_forestry;
        public string  inter_disciplinary;
        public string  business;
        public string  communicaions_library_sci;
        public string  math_comp_sci;
        public string  education;
        public string  engineering;
        public string  foreign_languages;
        public string  health_social_svcs;
        public string  humanities;
        public string  life_physical_sci;
        public string  social_sciences;
        public string  visual_Performing_arts;
        public string  basic_Personal_practical_svcs;
        public string  total;
    }

    /// <summary>
    /// Container class containing information pertaining to degree programs
    /// broken down by student levels. SchoolDegreeRec is used in school expense
    /// calculations (SchoolEx).
    /// </summary>
    class SchoolDegreeRec 
    {
        string  name;
        string  fice;
        string  control;
        string  carnegie;
        string  cc_sort;
        string  market_score;

        SchoolDegreeSLRec student_level;            // for sl 1&2, 3, 4
    }

    /// <summary>
    /// Extension of SchoolFaculty including active_research_dollars per faculty.
    /// </summary>
    class SchoolFacultyEx : SchoolFaculty 
    {
        public int active_research_dollars;                  // ($000) _per_faculty
    };

    /// <summary>
    /// Class for calculating and tracking school expenses. An extended version
    /// of School, with SchoolDegree variables in the "Initial Conditions" spreadsheet.
    /// </summary>
    class SchoolEx : School
    {
        public int db_school_recno;                          // recno to db_school_array;

        // field L-R
        public int operating_reserve;                        // current_funds_balance
        public int endowment_market;                         // end_of_year_endowment_market_value
        public int buildings;                                // year_end_market_value_of_plant
        public int capital_reserve;                          // year_end_capital_reserve
        public int general_plant_debt;                       // year_end_general_plant_and_residence_hall_debt
        public int residence_hall_debt;                      // residence_hall_debt
        public int fund_balance;                             // R=M1244+N1244+O1244-P1244-Q1244

        // field S
        public int gross_tuition_revenue;                    // total_gross_tuition_revenue
        public int financial_aid;                            // total_financial_aid
        public int net_tuition_revenue;                      // =S-T

        public int state_appropriations;                     // state_and_local_appropriations
        public int sponsored_research;                       // adjusted_total_sponsored_research
        public int gifts_to_operations;                      // gifts_to_operations
        public int endowment_spending;                       //

        // field Z
        public short athletics;                              // athletics_revenue
        public int other_operating_income;                   // adjusted_other_operating_income;
        public int interest_on_operating_reserve;            // = current_funds_balance * 0.08
        public int total_sources_of_funds;                   // =AC=SUM(U1244:AB1244)

        // field AD
        public int dept_expense_faculty_salaries;           // academic_dept_faculty_salaries
        public int dept_expense_staff_salaries;             // = academic_dept_total_salaries - AD
        public int dept_expense_other;                      // academic_dept_other_expense
        public int sponsored_research_faculty_salaries;     // sponsored_research_faculty_salaries
        public int sponsored_research_staff_salaries;       // = sponsored_research_total_salaries - AG
        public int sponsored_research_other;                // sponsored_research_other

        // field AJ
        public int library_staff_salaries;                  // library_salaries
        public int library_other_expense;                   // library_other_expense

        public int academic_it_staff_salaries;              // = academic_support_salaries * 0.1
        public int academic_it_other_expense;               // = academic_support_other_expense * 0.3

        // field AN
        public int student_life_staff_salaries;             // student_life_salaries
        public int student_life_other_expense;              // student_life_other_expense

        // field AR
        public int inst_advancement_staff_salaries;         // inst_advancement_salaries
        public int inst_advancement_other_expense;          // inst_advancement_other_expense

        public int administration_staff_salaries;           // inst_support_net_salaries;
        public int administration_other_expense;            // inst_support_net_other_expense;

        public int o_and_m_staff_salaries;                  // o_and_m_salaries
        public int o_and_m_other_expense;                   // o_and_m_other_expense

        // field AX
        public int other_operating_expense_staff_salaries;  // academic_support_salaries -AL
        public int other_operating_expense_other_expense;   // academic_support_other_expense -AM

        public int total_operating_expenditure;             // = SUM(AD1244:AY1244)

        // field BA
        public int service_on_general_plant_debt;           // (year_end_general_plant_and_residence_hall_debt
        // -residence_hall_debt)*0.065
        public int transfer_to_capital_reserve;             // transfer_to_plant
        public int total_uses_of_funds;                     // = AZ1244+BA1244+BB1244
        public int surplus_or_deficit;                      // = AC1244-BC1244

        // field BE
        public int tuition_rate;                            // = freshmen_from_within_the_state_percent/100.0*in_state_tuition+(1-freshmen_from_within_the_state_percent/100.0)*out_state_tuition
        public short room_and_board_rate;                   // room_and_board_charges
        // ug_in_housing_percent/1000.0
        public float percent_traditional_ug_in_residence_halls;
        public float percent_get_bacc_in_5_year;            // student_get_bacc_in_5_year_percent/1000.0
        public char overhead_rate_on_sponsored_research;    // overhead_rate
        public float endowment_spending_rate;               // float	endowment_spending_rate;

        // field BK
        // replaced super class's school_faculty_array and will be updated
        public SchoolFacultyEx[] school_faculty_array_ex= new SchoolFacultyEx[Enum.GetNames(typeof(FacultyLevel)).Length];

        // Enroll ft,pt_ug,etc (Enroll SL1-5)
        public short full_time_undergrad;                    // = sPtr->full_time_undergrad;
        public short part_time_undergrad;                    // = sPtr->part_time_undergrad;
        public short enrollment_masters;                     // = sPtr->enrollment_masters;
        public short enrollment_doctoral;                    // = sPtr->enrollment_doctoral;
        public short non_degree_seeking;                     // = sPtr->non_degree_seeking

        // field CM - Percent Female SL1-5
        public char[] percent_female_sl = new char[StudentConstants.MAX_STUDENT_LEVEL];   // = female_undergrad_percent

        // field CR - Percent Minority SL1-5
        public char[] percent_minority_sl = new char[StudentConstants.MAX_STUDENT_LEVEL]; // = minority_undergrad_percent*0.85;

        public char  percent_instate_freshmen;               // = freshmen_from_within_the_state_percent
        public char  percent_non_resident_alien_doctoral;    // = non_resident_alien_grad_and_prof_percent;

        // field DN
        public int ug_applications;                          // = student_applications;
        public float ug_applications_rate;                   // = ug_applications/student_accepted;
        public char  ug_yield_rate;                          // = float(student_enrolled)/student_accepted*100;

        // major_pref of sl1 = that of sl2
        float[,] student_ifield_pct = new float[StudentConstants.MAX_STUDENT_LEVEL - 1,Enum.GetNames(typeof(FieldType)).Length];
        float[] student_ifield_pct_total = new float[StudentConstants.MAX_STUDENT_LEVEL-1];

        public void init(int schoolRecno);                  // calculate vars in SchoolEx based on vars in School

        void  init_student_ifield_pct(SchoolDegreeRec degRec, short sl, short len);
    }
}
