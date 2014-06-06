using System;

namespace virtualu.Simulators
{
    enum Range
    {
        Local,
        Regional,
        National
    }

    enum Location
    {
        Northeast,
        MidAtlantic,
        Southeast,
        SouthCentral,
        Midwest,
        Pacific
    }

    enum Wealth
    {
        Rich,
        InBetween,
        Struggling
    }

    enum PlaqueColor
    {
        Gold,
        Silver,
        Bronze
    }

    enum Scenario
    {
        None,
        RaiseSalary,
        InvestAcademic,
        TeachingQuality,
        ResearchQuality,
        WinningAthletics,
        LimitTuitionGrowth,
        CompensateForEnrollmentShifts,
        StudentDiversity,
        FacultyDiversity,
        BalanceBudget
    }

    enum PlayerSchoolConstants
    {
        // order of constants in ec
        // Outputs
        S_DEGREES_GRANTED = 0,
        S_SPONSORED_RESEARCH_ACTIVITY,

        // Institutional performance indicators
        S_PRESTIGE,
        S_EDUCATIONAL_QUALITY,
        S_SCHOLARSHIP_BROADLY_DEFINED,                  // 1007
        S_STUDENT_DIVERSITY,
        S_FACULTY_DIVERSITY,
        S_PERCENT_ALUMNI_GIVEN,

        // Attitudes toward the institution
        S_FACULTY_MORALE,
        S_STUDENT_MORALE,
        S_STAFF_MORALE,

        // Financial indicators
        S_CURRENT_SURPLUS_DEFICIT_PERCENT,
        S_SMOOTHED_SURPLUS_DEFICIT_PERCENT,
        S_ENDOWMENT_PAYOUT_RATE_RELATIVE,               // 1007
        S_DEFERRED_MAINTENANCE_BACKLOG_PERCENT,

        SCORE_VAR_COUNT = 15,

        SCORE_SUBGROUP_COUNT = 4,
        GROUP1_BEGIN_ID = S_DEGREES_GRANTED,
        GROUP2_BEGIN_ID = S_PRESTIGE,
        GROUP3_BEGIN_ID = S_FACULTY_MORALE,
        GROUP4_BEGIN_ID = S_CURRENT_SURPLUS_DEFICIT_PERCENT,
    }

    class ProtagonistInfo 
    {
        public char protagonist_id;
        public char departmental_level;
        public char exist_departmental_level;                  // institutional level message having a similar departmental message
        public char cause_termination_flag;
        public enum ProtagonistFrequency event_freq;
        public short event_month;
        public short event_day;
        enum ProtagonistMsgSource msg_source;
        public float relative_trend_threshold;                 // RM
        public float normalizing_factor;                       // Zn
        public float r1_bonus_inst, r4_penalty_inst;           // r4_penalty_inst in negative
        public float r1_bonus_dept, r4_penalty_dept;           // r4_penalty_inst in negative
        public string insert_text;

        public int is_event_date();
        public string source_str(int departmentRecno, int srcPara1, short srcPara2, short srcPara3, short srcPara4 );
        // rating (R1-R6) and department
        public string rating_str(int rating, int departmentRecno );
        public string target_str(int departmentRecno);
    }

    /// <summary>
    /// Structure defining the start date, end date, and total number of days in a trimester.
    /// Used by the PlayerSchool class.
    /// </summary>
    class Trimester 
    {
        public char start_month;
        public char start_day;
        public char end_month;
        public char end_day;
        public char total_days;                                // total number of days in the trimester
        public void calc_total_days();
    }

    /// <summary>
    /// Class for representing the player's school, including student and faculty body,
    /// prestige, satisfaction and morale levels, institution evironment, mode, and profile.
    /// Used to calculate performance metrics and generate student reports.
    /// </summary>
    class PlayerSchool 
    {
        //---------- config vars -----------//
        public float spinner_var1;
        public float spinner_var2;
        public int mascot;
        public string school_name;
        public string player_first_name;
        public string player_last_name;

        // departments_list in game setting
        int tmp_department_pool_flag[DEPARTMENT_TYPE_COUNT];
        int right_bws_pool[DEPARTMENT_TYPE_COUNT];
        int left_bws_pool[DEPARTMENT_TYPE_COUNT];
        int right_bws_pool_size;
        int left_bws_pool_size;
        int pool_selected_size;
        int just_add,just_remove;

        //--------- Game settings -----------//

        int   student_market;                         // value: 0 to 2
        int   location;

        int   prestige;                               // value: 1 to 10
        int student_count;                            // = INPUT_LOW, INPUT_MEDIUM, INPUT_HIGH
        int sl5_student_percent;                      // = 0-100
        int get_player_expected_student_count();
        int get_player_expected_sponsored_research();

        int   parttime_undergrad_percent;
        int   ug_get_degree_in_5year_percent;

        int   fulltime_ug_in_housing_percent;
        int   athletic_program_intensity;             // value: 0 (low) to 2(high)	//## fred 1021

        int   graduate_student_percent;
        int   nondegree_student_percent;

        int   doctoral_program_intensity;
        int   sponsored_research_intensity;           // value from 0 to 10. Low=0, medium=5, high=10

        int   relative_wealth;
        int   control;                                // either 0:PRIVATE or 1:PUBLIC
        int   campus_environment;                     // = SUBURBAN, etc.
        bool  is_public()       { return control == PUBLIC; };
        float initializationValueAC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE;
        int   school_type;                            // set in ogammain.cpp; can be enum { LIBERAL_ARTS_SCHOOL, COMPREHENSIVE_SCHOOL, PUBLIC_SCHOOL, PRIVATE_SCHOOL };

        char  is_unacceptable_at_year_end;

        //--------- Scenario variables -----------//

        int scenario_id;                              // = 1 to SCENARIO_COUNT

        int scenario_base;                            // i.e. $300 thousand
        int scenario_base_at_start;                   // i.e. $300 thousand

        int scenario_target[3];                       // i.e. $330 thousand

        int scenario_base1;
        int scenario_base2;
        float scenario_float_base;

        float scenario_faculty_salary_increase;       // vars for specific scenario SCN_RAISE_SALARY
        float scenario_endowment_increase;

        int scenario_review_game_year;                // i.e. 3 year from gameStartYear
        int scenario_max_game_years;                  // i.e. 5 years
        int scenario_start_score;
        int scen_quit_game_flag;
        // if the flag of a department in this array is 1, the department is selected for processing in the current scenario.
        char  scen_dept_selected_array[MAX_DEPARTMENT];

        char  plaque_granted[TOTAL_PLAQUE_COUNT];     // 1: achieved
        char  goal_deadline[TOTAL_PLAQUE_COUNT];      //
        char  plaque_last;                            // = PLAQUE_* above

        void  scenario_check_target();                // check if the target of scenario is met
        int scenario_invest_academic_get_cur();
        //## chea 060899 to init in info.next_day()
        void  init_scen_dept_selected(int selectPercent);
        float scen_balance_budget_score();

        //---------------------------------//

        // store department_res's index; = 0 if invalid
        int selected_department_list[DEPARTMENT_TYPE_COUNT];

        // put in oconfig.h
        int chance_event_frequency;                   // = 	INPUT_LOW, INPUT_MEDIUM, INPUT_HIGH,
        int chance_objective_frequency;               // = 	INPUT_LOW, INPUT_MEDIUM, INPUT_HIGH,
        int chance_perturbations;
        int behavioral_response_speed;
        int threshold_event_flag;                     // for enable think_protagonist
        float difficulty_multiplier;

        Trimester trimester_array[TRIMESTER_PER_YEAR];

        // more user input	//fred 0505
        //
        char percent_student_in_state;                // depends on student_market
        char relative_wealth_value;                   // depends on relative_wealth; importance value

        //--------- dynamic vars ----------//

        char  cur_trimester;                          // AUTUMN, WINTER or SUMMER
        int   cur_trimester_start_date;

        char  is_trimester_off;
        char  trimester_off_day;                      // the nth day since the trimester is off

        char* trimester_str();                        // Winter '97

        //------ course related vars --------//

        char  teaching_method_pref[MAX_TEACHING_METHOD];
        char  course_match_order[MAX_STUDENT_LEVEL];  // the order in which the different levels of students are processed

        short cur_select_course_student_level_order;  // the student level which should be selecting courses now, it's an id. to PlayerSchool::course_match_order[MAX_STUDENT_LEVEL]
        short cur_select_course_student_level;        // the student level which should be selecting courses now. It's a student level id.

        //------ faculty pomotion policies screen - player input related vars --------//
        char  faculty_promotion_difficulity;          // use enum { INPUT_LOW, INPUT_MEDIUM, INPUT_HIGHT };
        // use enum { INPUT_LOW, INPUT_MEDIUM, INPUT_HIGHT };
        char  faculty_degree_to_which_priorities_reflected_in_promotion;

        char  summer_emphasis, summer_emphasis_last;  // (=0 for None, 1, 2, or 3)
        char  percent_summer_teaching_load;           // 99->99%; percent of summer teaching by regular faculty as part of normal teaching load

        //----- for playershool's all students generate_student() -----//

        float student_level_pct[MAX_STUDENT_LEVEL];
        float year_in_program_pct[MASTER_ARRAY_SIZE][MAX_GRADUATE_YEARS+1];
        float student_gender_pct[MAX_STUDENT_LEVEL][GENDER_ETHNIC_TYPE_COUNT];
        float dropout_trans_prob[MASTER_ARRAY_SIZE][MAX_GRADUATE_YEARS];
        float adjusted_student_major_pref[MAX_STUDENT_LEVEL_INI][MAX_DEPARTMENT];
        float major_pref_multiplier[MAX_DEPARTMENT];
        //990416
        float grad_trans_prob[MASTER_ARRAY_SIZE][MAX_GRADUATE_YEARS];

        float delay_rate[MAX_STUDENT_LEVEL];

        //----- for generate_student() -----//
    public:
        int get_random_major(char sl);                // using adjusted_student_major_pref
        char  generate_student_random_get_group_index(float *arr, int size);

        //### begin trevor 1908 ##//
    public:
        void  init();
        void  deinit();

        void  init_setting_scenario(int scenId);
        void  init_setting_non_scenario();
        void  init_scenario();

        int write_file(File* filePtr);
        int read_file(File* filePtr);
        int write_log_header(File *);
        int write_log(File *);

        // ## begin chwg0408
        char current_report_type;
        char current_report_mode;
        // ## end chwg0408

        void  student_report(int refreshFlag);
        int student_report_detect();

        void  summary_student_report(int refreshFlag);
        void  summary_student_report_1(int refreshFlag);
        void  summary_student_report_2(int refreshFlag);
        void  summary_student_report_3(int refreshFlag);
        void  summary_student_report_4(int refreshFlag);
        void  summary_student_report_5(int refreshFlag);
        void  summary_student_report_6(int refreshFlag);
        void  summary_student_report_7(int refreshFlag);
        void  summary_student_report_call_residence();
        int summary_student_report_detect();
        void  summary_student_report_deinit();
        void  summary_student_report_printing(int printOption);

        char  current_score_report_type;
        void  score_report(int refreshFlag);
        void  score_report_printing(int printOption);
        void  score_report_detect();
        void  score_report_deinit();

        void  next_day();
        //#### begin fred 0922 ####//
        void  next_trimester();
        //#### end fred 0922 ####//

        void  generate_student();

        void  new_trimester(int trimesterId);

        //## chea 190699
        void  generate_student_adjust_pct_array(float *arr, int size);
        //## chea 190699
        void  generate_student_readjust_pct_array(float *arr, int size);

        void update_stu_sum();                        //## chea 270999 for update stu_sum only
        void update_gender();                         //## chea 290999
        void calc_setting_vars();                     //## chea 091099 getting result of option buttons

        // new in GAME_VERSION>=200, eqv to math.latency_func when GAME_VERSION is 100
        float latency_func(float latencyPara, float curValue, float inputValue);

    private:
        void  init_config();
        void  default_setting();
        //	void 	calc_setting_vars();

        //----- for generate_student() -----//
        void  generate_student_level_pct();
        void  generate_student_transition_prob_n_year_in_program();
        void  generate_student_gender_ethnic();
        void  generate_student_major_preference();
        void  generate_student_adjust_pct_arrays();
        //	void	generate_student_adjust_pct_array(float *arr, int size);   //## chea 190699 change to public
        //	void	generate_student_readjust_pct_array(float *arr, int size); //## chea 190699 change to public

        void  generate_student_init_course(int progressIndex);
        //### end trevor 1908 ##//

        //##### begin fred 1013 #####//

        //-------------------------------------------------------------------------//
        //----- performance indicators -----//
        //-------------------------------------------------------------------------//

    public:
        // alumni_morale in odevelop.h
        // prestige in opeersch.h
        char  staff_morale;
        char  media_attitudes;
        char  administrative_performance;
        char  scholarship_broadly_defined;

        float sub_score[SCORE_VAR_COUNT][HISTORY_MONTH_COUNT];
        float sub_score_change[SCORE_VAR_COUNT];
        float sub_score_start[SCORE_VAR_COUNT];       // the value of the sub score at the beginning of the game. (or the end of the pre-year )
        float game_score_history[HISTORY_MONTH_COUNT];
        int ultimate_game_score_history[HISTORY_MONTH_COUNT];

        // 0125
        float score_sub_total[SCORE_SUBGROUP_COUNT][HISTORY_MONTH_COUNT];
        float score_group_change[SCORE_SUBGROUP_COUNT];
        float score_group_weight[SCORE_SUBGROUP_COUNT];

        float sub_score_weight[SCORE_VAR_COUNT];
        float init_game_score;
        float cur_game_score;
        int ultimate_game_score;
        int bonus_point;                              // 1013 for new "Objective"  //## chea 200899 use task_array.total_bonus_pt
        float threshold_event_bonus;

        //----------------//
        float time_to_degree[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];
        float dropout_rate[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];

        // special case handling: fake_disp_time_to_degree() for smoothing graph
        float fake_disp_time_to_degree[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];

        //----------------//
        int bachelor_degree[HISTORY_YEAR_COUNT];      // 980921
        int non_ug_bachelor_degree[HISTORY_YEAR_COUNT];
        int master_degree[HISTORY_YEAR_COUNT];
        int doctor_degree[HISTORY_YEAR_COUNT];

        //----------------//

        // student summary for sl1-5

        char  talent_academic_all[DEGREE_TYPE_COUNT][HISTORY_MONTH_COUNT];
        char  performance_academic_ug[HISTORY_MONTH_COUNT];

        // student summary for sl-1

        char  talent_academic[HISTORY_MONTH_COUNT];
        char  talent_extracurricular[HISTORY_MONTH_COUNT];
        char  talent_athletics[HISTORY_MONTH_COUNT];

        char  satisfaction_academic[HISTORY_MONTH_COUNT];
        char  satisfaction_student_life[HISTORY_MONTH_COUNT];
        char  satisfaction_athletics[HISTORY_MONTH_COUNT];
        char  satisfaction_overall[HISTORY_MONTH_COUNT];

        //## chea 270999 to update the student no. report.
        int   ins_student_level_no[MAX_STUDENT_LEVEL_N_TOTAL];
        int ins_student_level_history[MAX_STUDENT_LEVEL_N_TOTAL][HISTORY_TRIMESTER_COUNT];

        //## 290999 to update the gender report
        int   ins_gender_student_level_no[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
        int ins_gender_student_level_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL][HISTORY_TRIMESTER_COUNT];

        float old_pre;

        char  is_year_end_report;

        // ##### Begin MArco ##### //
        //	int	initial_ins_student_level_history[MAX_STUDENT_LEVEL_N_TOTAL][HISTORY_TRIMESTER_COUNT];
        //NEW
        int initial_ins_student_level_history[MAX_STUDENT_LEVEL_N_TOTAL];
        //	int	initial_ins_gender_student_level_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL][HISTORY_TRIMESTER_COUNT];
        //NEW
        int initial_ins_gender_student_level_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
        char  initial_talent_academic_all[DEGREE_TYPE_COUNT][HISTORY_MONTH_COUNT];
        char  initial_performance_academic_ug[HISTORY_MONTH_COUNT];

        char  initial_talent_academic[HISTORY_MONTH_COUNT];
        char  initial_talent_extracurricular[HISTORY_MONTH_COUNT];
        char  initial_talent_athletics[HISTORY_MONTH_COUNT];

        char  initial_satisfaction_academic[HISTORY_MONTH_COUNT];
        char  initial_satisfaction_student_life[HISTORY_MONTH_COUNT];
        char  initial_satisfaction_athletics[HISTORY_MONTH_COUNT];
        char  initial_satisfaction_overall[HISTORY_MONTH_COUNT];

        int initial_bachelor_degree[HISTORY_YEAR_COUNT];
        int initial_master_degree[HISTORY_YEAR_COUNT];
        int initial_doctor_degree[HISTORY_YEAR_COUNT];

        float initial_time_to_degree[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];
        float initial_dropout_rate[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];

        //	float	initial_fake_disp_time_to_degree[DEGREE_TYPE_COUNT][HISTORY_YEAR_COUNT];
        float initial_fake_disp_time_to_degree[HISTORY_YEAR_COUNT];

        // [protagonist_id-1][departmentRecno] or [protagonist_id-1][0] for instituional level
        int last_protagonist_event_date[MAX_PROTAGONIST_ID][MAX_DEPARTMENT+1];
        // ##### End MArco ##### //

        //----------------//
    private:
        // intermediate vars for calc of performance indicators.
        float degree_granted_ratio_init;
        float degree_granted_weight[MAX_STUDENT_LEVEL];
        int init_research_dollar;

    public:
        void  init_pi();
        void  init_score();
        // set sub_score_weight to default value
        void  init_sub_score_weight(int subScoreGroup=-1);
        void  normalize_sub_score_weight();           // make sub of sub_score_weight to 100%
        float get_default_sub_score_weight(int);
        void  add_bonus(int value);
        void  update_history(char flag=UPDATE_ALL);
        void  calc_ultimate_score();
        void  calc_staff_morale();
        void  calc_media_attitudes();

        void  save_initial_data();
        int is_hidden_mascot(int mascotId);
        void  think_protagonist();                    // generate opinion from people
        int think_protagonist_event(float curValue, float lastYearValue, float initialValue, struct ProtagonistInfo *protagonistInfo, int departmentRecno, int testResign=0 );
    #ifdef DEBUG
        void  set_cks( int index, float change );
        void  get_cks( float &ck1, float &ck2, float &ck3 );
    #endif
        ProtagonistInfo*  get_protagonist_info(int recno);
        void  calc_strength_consistency_k1(float curValue, float lastYearValue, float initialValue, struct ProtagonistInfo *protagonistInfo, float &strength, float &consistency, float &k1);
        int is_protagonist_event_date( int protagonistId, int departmentRecno );
        int mark_protagonist_event_date( int protagonistId, int departmentRecno );

    private:
        void  calc_performance_yearly();
        void  calc_performance_monthly();
        void  calc_prestige();
        void  calc_administrative_performance_etc();

        void  calc_student_performance();
    }
}
