using System;

using virtualu.Utility;

namespace virtualu.Simulators
{
    class FacultyTemplateConstants
    {
        public const short MAX_RANK_AGE_GROUP = 7;
        public const char FIRST_TEMPLATE_CODE = 'A';
    }

    /// <summary>
    /// Structure containing the faculty template data as stored in the database.
    /// </summary>
    class FacultyTemplateRec 
    {
        public string template_code;
        public string rank_age_group_id;

        //----- multipliers -----//
        public string rank_age_multiplier;
        public string female_multiplier;
        public string minority_multiplier;

        public string overall_salary_multiplier;
        public string female_salary_multiplier;
        public string minority_salary_multiplier;

        public string talent_teaching_multiplier;
        public string talent_scholarship_multiplier;
        public string talent_research_multiplier;

        public string normal_teaching_load_multiplier;

        public string[] discretionary_time_pref = new string[Enum.GetNames(typeof(DiscretionaryType)).Length];
    }

    /// <summary>
    /// Structure containing the faculty template attributes. The faculty
    /// templates elaborate the department-level parameters to the faculty
    /// age/rank categories. (See Tech Doc 3.3, section 3.3)
    /// </summary>
    class FacultyTemplate {
        public char  template_code;
        public char  rank_age_group_id;

        //----- multipliers -----//
        public float rank_age_multiplier;
        public float female_multiplier;
        public float minority_multiplier;

        public float overall_salary_multiplier;
        public float female_salary_multiplier;
        public float minority_salary_multiplier;

        public float talent_teaching_multiplier;
        public float talent_scholarship_multiplier;
        public float talent_research_multiplier;

        public float normal_teaching_load_multiplier;          // NTL multiplier

        // discretionary time preference // 0-100
        public char[]  discretionary_time_pref = new char[Enum.GetNames(typeof(DiscretionaryType)).Length];
    }

    /// <summary>
    /// Collection Class of all Faculty templates and methods for delegating
    /// requests to generate repots to the respective templates.
    /// </summary>
    class FacultyRes 
    {
        public char init_flag;
        public int report_type;
        public int report_list_mode;
        public int report_det_mode;
        public int report_faculty_recno;
        public int report_research_recno;

        public short faculty_template_count;
        public FacultyTemplate faculty_template_array;

        public short show_department_detail_count;           // This is just used in Faculty report
        public char is_year_end_report;                     // This is just used in Faculty turnover report

        //--------------------------------------------------//
        // The following vars are for storing percentages
        // that are not realized because the percentage
        // are too small to produce one integer faculty.
        // They are accumulated here, so that they will
        // be added to the following calculations to
        // produce a overall population that reflects
        // the percentages calculated. They are set to zero
        // in init().
        //--------------------------------------------------//

        // since it's a fraction of an integer, so the var type is float
        public float[] unused_faculty_count_array = new float[Enum.GetNames(typeof(Gender)).Length * Enum.GetNames(typeof(EthnicGroup)).Length];

        public FacultyRes();

        public void init();
        public void deinit();

        public int     write_file(File filePtr);
        public int     read_file(File filePtr);

        public void report(int refreshFlag);
        public void report_faculty_hired(int refreshFlag);
        public int report_hired_detect();
        public void report_detect();
        public int current_report_type;
        public int current_year_report;
        public int last_report_type;
        public void summary_report(int refreshFlag);
        public void summary_report_1(int refreshFlag);
        public void summary_report_2(int refreshFlag);
        public void summary_report_3(int refreshFlag);
        public void summary_report_4(int refreshFlag);
        public void summary_report_5(int refreshFlag);
        public void summary_report_6(int refreshFlag);
        public void summary_report_7(int refreshFlag);
        public void summary_report_8(int refreshFlag);
        public void summary_report_9(int refreshFlag);
        public void summary_report_10(int refreshFlag);
        public int summary_report_detect();
        public void summary_report_deinit();

        public FacultyTemplate  get_faculty_template(string templateCode, int rankAgeGroup);

        void load_template();
        void free_template();
        void report_paint_button(int refreshFlag);
    }
}
