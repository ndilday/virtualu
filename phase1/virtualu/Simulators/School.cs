using System;

using virtualu.Utility;

namespace virtualu.Simulators
{
    class SchoolConstants
    {
        public const short MAX_SCHOOL_EX_FOR_INIT_CONDITION = 10;    // the number of school used for generating initial conditions
        public const short MAX_PEER_SCHOOL_COUNT = 12;
        public const short MAX_FIRST_SCHOOL = 30;
    }

    /// <summary>
    /// Simple structure for storing faculty information pertaining to expense
    /// calculations aggregated by rank. Includes faculty count, average salary,
    /// and male/female percentages for each ranking.
    /// </summary>
    class SchoolFaculty
    {
        public short faculty_count;
        public int salary;
        public float female_percent;
        public float minority_percent;
    }

    enum CampusEnvironment
    {
        Urban,
        Suburban,
        Rural
    }

    //!School resourse class which loads all SchoolRecords (schools, peers, and
//!the players) from db. Responds to init_game() and read/write.
class SchoolRes 
{
        public short db_school_count;
        public School db_school_array;              // an array of schools read from the database

        public School desired_school_array;
        public short desired_school_count;

        public short peer_info_count;
        public string peer_info_array;              // an array of peer schools read from the database

        public SchoolEx[] school_ex_array = new SchoolEx[SchoolConstants.MAX_SCHOOL_EX_FOR_INIT_CONDITION];
        public PeerSchool player_peer_school;

        public short peer_school_count;
        public PeerSchool[] peer_school_array;            // includes player_peer_school after battle.create_object()

        // peer_school_rank[] store the index of peer_school_array; index 0 refer to highest score
        public short[] peer_school_rank = new short[SchoolConstants.MAX_PEER_SCHOOL_COUNT];

        public void init();
        public void deinit();

        public int write_file(File filePtr);
        public int read_file(File filePtr);

        public void init_game();
        public void next_day();                     //### fred 1027 ###//

    #if DEBUG
        School       get_db_school(int recno);
    #else
        School       get_db_school(int recno) { return db_school_array+recno-1; }
    #endif

        public void get_desired_school_array();
        public void get_50_school_array();

        void load_db_info();
        void free_db_info();

        void build_school_ex_array();
        void init_player_school_ex();
        void build_peer_school_array();

        void load_db_1();
        void load_db_2();
        void load_db_peer();
}

    /// <summary>
    /// Class representing other non-peer schools in the game, used for
    /// calculating prestige, relative wealth, athletic success, and percentages.
    /// </summary>
    class School
    {
        //---- institutional characteristics ----//
        public string  name;                               //##### fred 0826 #####//
        public short fice;
        public char  control;
        public char  carnegie;
        public char  cc_sort;
        public char  market_score;

        //--------------------------------------//

        public string state;
        public short usn_wr_ranking;                         // Raw USN&WR ranking
        public short ave_midpt_sat_act;
        public char  freshmen_top_10_class;

        public CampusEnvironment  campus_environment;                     // = URBAN, SUBURBAN, or RURAL

        public string  ncaa_football_division;
        public string  ncaa_basketball_division;
        public char  football_rating;
        public char  basketball_rating;

        public short in_state_tuition;
        public short out_state_tuition;
        public short room_and_board_charges;
        public char  ug_in_housing_percent;

        public SchoolFaculty[] school_faculty_array = 
            new SchoolFaculty[Enum.GetNames(typeof(FacultyLevel)).Length];

        public float parttime_faculty_percent;

        //----------------------------------------//
        // fred 0501

        public int student_applications;
        public short student_accepted;
        public short student_enrolled;
        public char  student_get_bacc_in_5_year_percent;

        // field 43
        public short freshmen_applied_for_aid;
        public float percent_freshmen_applied_or_aid;

        public short freshmen_with_need;
        public float percent_freshmen_with_need;

        public short freshmen_offered_aid;
        public float percent_freshmen_offered_aid;

        public short freshmen_offered_full_aid;
        public float percent_freshmen_offered_full_aid;

        public char percent_freshmen_with_fin_aid;
        public char percent_continuing_student_with_fin_aid;

        public short institutional_aid_per_fte;

        //----------- revenue data -------------//
        // field 54
        public int total_gross_tuition_revenue;
        public int total_financial_aid;
        public int total_sponsored_research;
        public int adjusted_total_sponsored_research;
        public int raw_endowment_spending;
        public int state_and_local_appropriations;
        public int total_gifts;
        public int gifts_to_operations;
        public short athletics_revenue;
        public int adjusted_other_operating_income;
        public int raw_other_operating_income;

        //----------- field 65: expense data -------------//
        public int academic_dept_total_salaries;
        public int academic_dept_faculty_salaries;
        public int academic_dept_other_expense;

        public int sponsored_research_total_salaries;
        public int sponsored_research_faculty_salaries;
        public int sponsored_research_other_expense;

        public int library_salaries;
        public int library_other_expense;

        public int student_life_salaries;
        public int student_life_other_expense;

        public int inst_support_salaries;
        public int inst_support_other_expense;

        public int inst_support_net_salaries;
        public int inst_support_net_other_expense;

        public int inst_advancement_salaries;
        public int inst_advancement_other_expense;

        public int o_and_m_salaries;
        public int o_and_m_other_expense;

        public int athletics_salaries;
        public int athletics_other_expense;

        public int academic_support_salaries;
        public int academic_support_other_expense;

        // field 87	// fred 0501
        public int end_of_year_endowment_market_value;
        public int current_funds_balance;
        public int year_end_book_value_of_plant;
        public int year_end_market_value_of_plant;
        public int year_end_capital_reserve;
        public int year_end_general_plant_and_residence_hall_debt;
        public int residence_hall_debt;

        public int raw_surplus;
        public int adjusted_surplus;
        public int transfer_to_plant;
        public int amount_added_to_other_operating_income;

        // field 98
        public short full_time_undergrad;
        public short part_time_undergrad;
        public short master_and_professional;
        public short non_degree_seeking;
        public char female_undergrad_percent;
        public char female_grad_and_prof_percent;
        public char minority_undergrad_percent;
        public char minority_grad_and_prof_percent;
        public char non_resident_alien_grad_and_prof_percent;

        // field 107
        public short full_time_freshmen;
        public char  freshmen_from_within_the_state_percent;
        public short total_bach_degrees;
        public short total_masters_and_prof_degree;
        public short total_doctoral_degrees;
        public short total_certificates;

        // field 113
        public int enrollment_fte;
        public char ug_percent;
        public char part_time_ug_percent;
        public char non_degree_seeking_percent;

        public float applications_ratio;
        public char yield_rate;
        public char athletics_rating;

        public int total_e_and_g_expenditure;
        public char net_tutition_reenue_as_percent_of_e_and_g_expese;

        public short total_tenureline_faculty;
        public float sponsored_research_per_reg_faculty;     // ($000)
        public float doctoral_degrees_per_reg_faculty;

        public char overhead_rate;
        public float endowment_spending_rate;

        short enrollment_masters;
        short enrollment_doctoral;

        // ---------- field 1 of umaster2 -------------

        public short[] target_student_intake_sl = new short[StudentConstants.MAX_STUDENT_LEVEL];
        public float[] dropout_rate_sl = new float[StudentConstants.MAX_STUDENT_LEVEL];
        public float[] target_grad_rate_sl = new float[StudentConstants.MAX_STUDENT_LEVEL];

        // field 16
        public char  percent_ug_students_on_aid;
        public float endowment_per_student;
        public float endowment_per_tenureline_faculty_member;
        public float ratio_of_current_funds_balance_to_total_operating_expenditure;
        public char  financial_condition;
        public short number_of_incoming_ug_students;
        public char  female_student_percent;
        public char  minority_student_percent;
        public char  sponsored_research_rating;
        public char  doctoral_degres_per_faculty_rating;
        public char  adj_freshmen_top_ten_percent_of_class;

        public float apps_ratio2;
        public float yield_rate2;
        public char athletics_rating2;
        public char net_tuition_revenue_as_percent_of_e_and_g_expense;
        public float doc_time_to_degree;

        //---- vars for screening school records -----//

        public float prestige;
        public float distance_value;
        public short desired_recno;

        //---- temp vars for calu distance_value -----//
        public int N5;
        public int O5;
        public int P5;
        public float Z5;
        public float L5;
        public float K5;
        public float M5;
        public char AC5;
        public char AH5;

        //---- helper methods for calc_distance_all_school ----//

        void calc_distance_NOPZ5();
        void calc_distance_L5();
        void calc_distance_K5();
        void calc_distance_ACAH5(short sCount, School[] sArray);
        void calc_distance_prestige();
        void calc_distance_M5();
        void calc_distance_value();

        public void  calc_distance_first();
        public static void calc_distance_all_school();       // for calculate prestige and distance_value
    }
}
