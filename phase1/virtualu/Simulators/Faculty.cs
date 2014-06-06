using System;

using virtualu;
using virtualu.Utility;

namespace virtualu.Simulators
{
    class FacultyConstants
    {
        public const short NORMAL_TEACHING_CONTACT_HOUR = 6;    // A faculty normally spend 6 hours a week in teaching
        public const short NORMAL_FACULTY_TOTAL_HOUR = 44;      // A faculty normally spend 44 hours in teaching and other activities in discretionary time
        public const short NORMAL_DISCRETIONARY_HOUR = (NORMAL_FACULTY_TOTAL_HOUR - NORMAL_TEACHING_CONTACT_HOUR);
        public const short GCON = 3;                            //## chea 161099
        public const short DCON = 12;                           //## chea 161099
        public const short MAX_RESEARCH_PROPOSAL = 12;
        public const short MAX_ACTIVE_RESEARCH_MONTH = 12;      // maximum number of months for a research
        public const short MAX_PROPOSED_RESEARCH_MONTH = 6;
    }

    enum FacultyLevel
    {
        AssistantProfessor, 
        AssociateProfessor, 
        FullProfessor, 
        LongTermAdjunct, 
        ShortTermAdjunct
    }

    enum DiscretionaryType
    {                                            // discretionary time
        CoursePreperation,
        OfficeHours,
        EducationalDevelopment,
        Research,
        Scholarship,
        PublicService
    }

    /// <summary>
    /// Structure describing costs and milestone dates for active and proposed faculty research projects.
    /// </summary>
    class ResearchProposal
    {
        public int total_dollars;                              // ($000)
        public float overhead_rate;
        public char status;
        public float prob_proposal_funded;
        // if active, it will be the date when the proposal is supposed to be complete
        // if proposed, it will be the date when a decision is made on whether it is accept or not
        // Notes 0830: add reseaerch overhead rate at the time each proposal is generated
        public int date_to_next_status;
    }

    class WorkAreas
    {
        public float teaching;
        public float scholarship;
        public float research;
    }

    /// <summary>
    /// Class for representing a faculty member, including personal information,
    /// salary, ongoing research, teaching, talent and performance. Responds to
    /// Info::next_day()
    /// </summary>
    class Faculty
    {
        public short faculty_recno;
        public short department_recno;

        public short photo_id;

        public string name();
        public static string name(short firstNameId, short middleNameId, short lastNameId);
        public short first_name_id;
        public short middle_name_id;
        public short last_name_id;

        public FacultyLevel rank_level;
        public char gender_ethnic_group;                    // constant values for this is defined in GAMEDEF.H
        public int birthday;
        public int age()     { return int( float(info.game_date - birthday) / 365.25 ); }

        // will be used by PeerSchool also
        public static int rank_age_group(FacultyLevel rank, int age);
        public int rank_age_group() { return rank_age_group(rank_level, age()); };

        public static int rank_age_group_2_rank(int rag);
        public bool is_regular();
        public static bool is_adjunct_faculty(FacultyLevel rank) { return rank == FacultyLevel.LongTermAdjunct || rank == FacultyLevel.ShortTermAdjunct; };

        public static int get_min_age(int rankAgeGroup);

        public int start_teaching_date;
        public int last_promotion_date;
        public int salary;                          // NOT ($000) // annual income
        public int last_salary_increase;                   //### fred 1030
        public string employed_period();

        public char  teaching_contact_hour;                // unit: hour	// also called teaching load
        public float[] discretionary_hour_array = new float[Enum.GetNames(typeof(DiscretionaryType)).Length];
        // var to be removed // discretionary time in the last trimester, for computation purposes
        public char[] last_discretionary_hour_array = new char[Enum.GetNames(typeof(DiscretionaryType)).Length];
        public float  total_hour();

        public char  employ_status;
        public char  is_dismissed;
        public char  dismiss_trimester;

        public float strain_priority;                        // for faculty incentive
        public float strain_on_discretionary_time;           // result of faculty incentive

        //---------- research related var ---------------//

        // Notes 0830:
        // The effects of research on doctoral requirements, faculty discretionary time, faculty
        // research performance, and any other faculty activity variables are all based on direct
        // research dollars. The financial statements are based on total research dollars, so
        // they now can be calculated by summing the "total research" variable over faculty sims..

        public ResearchProposal[] research_proposal_array = new ResearchProposal[FacultyConstants.MAX_RESEARCH_PROPOSAL];

        public bool  is_investigator;

        public char  research_proposal_count;
        public int research_month_expense;                   // ($000)  total monthly expenditures on sponsored rsrch prj
        public int research_month_expense_direct;            // ($000)  // 1105

        //----------------------------------//

        public Season  off_duty_trimester;
        public bool  is_third_trimester_teaching;            // 0 or 1

        public float reaction_summer_teaching, reaction_summer_teaching_last;

        //----------------------------------//
        public WorkAreas talent;
        public WorkAreas performance;
        public float p_discretionary_hour_it_percent;
        public float p_discretionary_hour_it;

        public float time_shifted_scholarship;

        public float satisfaction_index;                     //	a percentage from 0 to 100
        public float satisfaction_index_last;
        public float satisfaction_index_last_tri;

        //--- extra effect for chance card ---//

        public float extra_effect_satisfaction_index;
        public float extra_effect_performance_research;

        //-----------------------------------//

        // 990412 research project generation related vars

        public float proj_count_mult;
        public float proj_size_mult;
        public float award_prob;

        public float ave_proj_count;
        public float ave_proj_size;

        //-----------------------------------//
        public void init(int departmentRecno, int rankLevel, int genderEthnicGroup, int facultyAge, 
                          int startTeachingDate, int facultySalary, WorkAreas talent);

        public void init_research();

        public int write_file(File filePtr);
        public int read_file(File filePtr);

        public void next_day();
        public void next_trimester();

        public void calc_all_performance();

        public void disp_summary_basic(int x, int y, int xColumn2, int refreshFlag, int displayModeFlag);
        public void disp_summary_activity(int x, int y, int xColumn2, int refreshFlag);
        public void disp_summary_assess(int x, int y, int xColumn2, int refreshFlag);

        public void disp_detail_std(int refreshFlag);
        public void disp_research_proposal(int y, int refreshFlag);

        void  init_photo();

        string hour_str(float hourCount);

        int think_departure();
        int think_dismiss();
        int think_promotion();
        void  think_discretionary_time();
        void  think_research();
        void  calc_discretionary_hour_it();
        void  calc_performance_teaching();

        public void calc_performance_scholarship();
        public void calc_performance_research();
        public void calc_satisfaction_index();
        public int think_accept_retire(int retireOffer);
        public void update_history(char update_flag);
        public float get_quality_driver();                   // 990412
    }

    /// <summary>
    /// Collection Class for managing all of the members of the faculty.
    /// Delegates events to each member of the faculty.
    /// </summary>
    class  FacultyArray: DynArrayB 
    {
        public int department_recno;
        public int faculty_count;

        //------------------------------------------//
        public FacultyArray();
        public ~FacultyArray();

        public int write_file(File filePtr);
        public int read_file(File filePtr);

        public int add(int departmentRecno, int rankLevel, int genderEthnicGroup, 
                       int facultyAge, int startTeachingDate, int facultySalary,
		               WorkAreas talent);

        public void del(int recNo, int isQuiting=0);

        public int avg_discretionary_hour(int discretionaryType);

        public void next_day();
        public void next_trimester();

        public Faculty get_unpacked_faculty(int recNo);

        public int is_deleted(int recNo);

        //------------------------------------------//
        // performance indicators

        // percentages from 0 to 100
        public WorkAreas[] talent = new WorkAreas[Constants.HISTORY_TRIMESTER_COUNT];
        public WorkAreas[] performance = new WorkAreas[Constants.HISTORY_TRIMESTER_COUNT];
        public WorkAreas initial_talent;
        public WorkAreas initial_performance;
        public float initial_time_shifted_scholarship;
        public float satisfaction_index = new float[Constants.HISTORY_TRIMESTER_COUNT];
        // percent faculty have research project
        public char percent_research = new char[Constants.HISTORY_TRIMESTER_COUNT];

        // unit: hour
        public float[] teaching_contact_hour = new float[Constants.HISTORY_TRIMESTER_COUNT];
        public float[,] discretionary_hour_array = new float[Enum.GetNames(typeof(DiscretionaryTypes)).Length + 1,Constants.HISTORY_TRIMESTER_COUNT];
        public float time_shifted_scholarship;

        public float initial_teaching_contact_hour;
        public float initial_discretionary_hour_array = new float[Enum.GetNames(typeof(DiscretionaryTypes)).Length + 1];

        public void  update_history(char update_flag);
        public void  save_initial_data();

        void  think_summer_teaching();
        void  calc_history_monthly();
        void  calc_history_trimester();
    }
}
