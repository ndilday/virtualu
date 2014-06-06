using System;

namespace virtualu.Simulators
{
    static class DepartmentConstants
    {
        public const short DEPARTMENT_TYPE_COUNT = 28;
        public const short COURSE_GAME_FIELD_COUNT = Enum.GetNames(typeof(GameField)).Length + 1;
    }

    class DepartmentRec 
    {
        public string name;
        public string field_code;
        public string total_courses_taught;
        public string  max_dl_class_size;                     //1102 add this?

        //---- multipliers & talent indices -----//
        public string salary_multiplier;
        public string teaching_load_multiplier;
        public string research_per_faculty_multiplier;
        public string female_multiplier;
        public string minority_multiplier;
        public string talent_teaching;
        public string talent_scholarship;
        public string talent_research;

        public string percent_faculty_pi;
        public string research_project_size;

        public string doctoral_graduation_rate_multiplier;
        public string doctoral_time_to_dropout_multiplier;
        public string doctoral_time_to_degree_multiplier;
        public string graduate_tuition_rate_multiplier;

        //-------- templates -------//
        public string template_rank_and_age;
        public string template_gender_ethnic;
        public string template_salary;
        public string template_talent;
        public string template_course_load;                  // Normal teaching load

        public string faculty_discretionary_time;
        public string course_type;
        public string base_fac_discretionary_time;
    }

    /// <summary>
    /// for dept2.dbf
    /// </summary>
    struct DepartmentRec2 
    {
        public string name;
        public string iped_field;
        public string field_fraction_pct_sl1n2;
        public string field_fraction_pct_sl3;
        public string field_fraction_pct_sl4;
        public string field_fraction_pct_sl5;
    }

    /// <summary>
    /// for facility.dbf
    /// Structures for holding department data.
    /// </summary>
    struct DepartmentRec3 
    {
        public string replacement_cost;
        public string  normal_onm_cost;
        public string months_to_construct;

        public string sf_per_course_enrollment;
        public string sf_per_faculty;
        public string sf_per_research_dollar;
        public string sf_per_dollar_of_central_exp;
        public string fixed_sf_per_dept;
    }

    /// <summary>
    /// for elecpref.dbf
    /// Elective preference record?
    /// </summary>
    class ElecPrefRec 
    {
        public string[] pref = new string[DepartmentConstants.DEPARTMENT_TYPE_COUNT];  // 0.999 => 99.9%
    }

    /// <summary>
    /// Structure for holding faculty information.
    /// </summary>
    class DepartmentFacultyRec 
    {
        public string[] trans_prob = new string[Enum.GetNames(typeof(Range)).Length];
    }

    /// <summary>
    /// Structure for holding course information.
    /// </summary>
    class DepartmentCourseRec 
    {
        public string[] required = new string[DepartmentConstants.COURSE_GAME_FIELD_COUNT];
    }

    class DepartmentUGCourseRec : DepartmentCourseRec 
    {
        public string[] elective = new string[StudentConstants.BACHELOR_YEAR_COUNT];
    }

    /// <summary>
    /// "extract the same fields from DepartmentInfo for general education
    /// can be a base class of DepartmentInfo if necessary"
    /// 
    /// Base class of DepartmentInfo. Needed for handling general department for
    /// "undecided" student, and university general facilities for facilities model.
    /// </summary>
    class GeneralDepartmentInfo 
    {
        //----- curriculum requirment vars -------//
        public char[] bachelor_depth1 = new char[DepartmentConstants.COURSE_GAME_FIELD_COUNT];  // bachelor_depth1[0] --> own department
        public char[] bachelor_depth2 = new char[DepartmentConstants.COURSE_GAME_FIELD_COUNT];  // each element contains the number of courses a student must take in a department
        public char[] bachelor_depth3 = new char[DepartmentConstants.COURSE_GAME_FIELD_COUNT];

        // it contain the number courses a student must take over the years
        public char[] bachelor_elective_year_count = new char[StudentConstants.BACHELOR_YEAR_COUNT];
        // the total of preference weight across all department is 100, it is
        public float[] bachelor_elective_course_pref = new float[DepartmentConstants.DEPARTMENT_TYPE_COUNT];
        float total_bachelor_elective_course_pref;      // the total of pref weights for existing departments

      //----- for facility office
      // sf: square foot

      // Parameters for replacement and normal operations and maintenance cost
        public int replacement_cost;                           // unit:	$ per sf		(not $000)
        public float cost_growth, cost_growth_last2;           //## fred // 1026 for calc time variation // 0.99%
        public float normal_onm_cost;                          // unit: $ per sf    (not $000)
        public char months_to_construct;

      // Parameters for calculating normal square footage
        public float sf_per_course_enrollment_in_seminars;
        public float sf_per_course_enrollment_in_general_courses;
        public float sf_per_course_enrollment_in_lecture_courses;

        public float sf_per_faculty;
        public float sf_per_research_dollar;                   // per thousand dollars
        public float sf_per_dollar_of_central_exp;             // per thousand dollars
        public float fixed_sf_per_dept;

        // initermediate variables; not from db!!!
        public int normal_sf, actual_sf;
        public int projected_actual_sf;
        // $000
        public float actual_new_debt, required_draw_on_capital_reserve, invest_close_gap, additional_debt;
    }

    /// <summary>
    /// Contains information about a department, including its students and faculty.
    /// Calculates the number of majors in the department, 
    /// including the number that switch to other departments.
    /// </summary>
    class DepartmentInfo : GeneralDepartmentInfo 
    {
        string  name;
        short field_id;

        // short total_courses_taught;	// 0406 it's a place holder in excel file
        short max_dl_class_size;                        // 1102

        //------ multipliers ------//

        float salary_multiplier;
        float teaching_load_multiplier;
        float research_per_faculty_multiplier;
        float female_multiplier;
        float minority_multiplier;

        //------ multipliers 0616 fred; for init student database ------//

        float doctoral_grad_rate_multiplier;
        float doctoral_time_to_dropout_multiplier;
        float doctoral_time_to_degree_multiplier;
        float graduate_tuition_rate_multiplier;

        //---- faculyt parameter-----//

        char  talent_teaching;
        char  talent_scholarship;
        char  talent_research;

        char  percent_pi_faculty;                       // 1117	// 99->99%
        int normal_research_project_size;

        //-------- templates -------//

        char  template_rank_and_age[1];
        char  template_gender_ethnic[1];
        char  template_salary[1];
        char  template_talent[1];
        char  template_course_load[1];

        char  template_discretionary_time[1];           //1116

        char  template_course_type[2];                  //1105

        char  base_fac_discretionary_time[1];           //1119

        //----- for database dept2 -----//

        char  iped_field;
        float field_fraction_pct[4];

        //----- for generate_student database -----//

        float doctor_dropout_trans_prob[MAX_GRADUATE_YEARS];
        float doctor_graduate_trans_prob[MAX_GRADUATE_YEARS];

        float doctor_year_in_program_pct[MAX_GRADUATE_YEARS+1];
        float student_major_pref[MAX_STUDENT_LEVEL_INI];

        //------ faculty promotion and departure probability ------//

        // 0.99->99%
        float faculty_departure_probability[FACULTY_RANK_LEVEL_COUNT][PROB_SCHOOL_TYPE_COUNT];
        float associate_professor_promotion_probability[PROB_SCHOOL_TYPE_COUNT];
        float assistant_professor_promotion_probability[PROB_SCHOOL_TYPE_COUNT];

        //------ required and elective courses tables ------//

        char master_depth3[COURSE_GAME_FIELD_COUNT];
        char master_graduate[COURSE_GAME_FIELD_COUNT];
        char master_elective_year_count;                // it contain the number courses a student must take over the years, master program only has one year

        // the total of preference weight across all department is 100, it is
        float master_elective_course_pref[DEPARTMENT_TYPE_COUNT];
        float total_master_elective_course_pref;

        char doctor_graduate[COURSE_GAME_FIELD_COUNT];

    public:
        //---- functions for generating student ----//

        void  generate_student_transition_prob_n_year_in_program_doctor();
        void  generate_student_major_preference();
    }

    class Department
    {
    }
}
