//Filename    : OFACULTY.h
//Description : Faculty Class Declaration

#ifndef __OFACULTY_H
#define __OFACULTY_H

#include <OINFO.H>
#include <GAMEDEF.H>

#ifndef __ODYNARRB_H
#include <ODYNARRB.H>
#endif

//--------------------------------------------//

#define NORMAL_TEACHING_CONTACT_HOUR    6         // A faculty normally spend 6 hours a week in teaching
#define NORMAL_FACULTY_TOTAL_HOUR   44            // A faculty normally spend 44 hours in teaching and other activities in discretionary time
#define NORMAL_DISCRETIONARY_HOUR   (NORMAL_FACULTY_TOTAL_HOUR-NORMAL_TEACHING_CONTACT_HOUR)
#define GCON 3                                    //## chea 161099
#define DCON 12                                   //## chea 161099

//--------------------------------------------//

enum { FACULTY_RANK_LEVEL_COUNT = 5 };

enum {                                            // 3 professers & 2 adjuncts
    ASST_PROF, ASSOC_PROF, FULL_PROF, LONG_TERM_ADJUNCT, SHORT_TERM_ADJUNCT
};

enum { DISCRETIONARY_TYPE_COUNT = 6 };

enum {                                            // discretionary time
    DT_COURSE_PREPARATION,
    DT_OUT_OF_CLASS_STUDENT_CONTACT,
    DT_EDUCATIONAL_DEVELOPMENT,
    DT_RESEARCH,
    DT_SCHOLARSHIP,
    DT_INSTITUTIONAL_AND_PUBLIC_SERVICE
};

//##### fred 0928 #####//
// plus TOTAL; for interface
enum {
    DISCRETIONARY_TYPE_COUNT_N_TOTAL = DISCRETIONARY_TYPE_COUNT + 1,

    FACULTY_RANK_LEVEL_COUNT_N_TOTAL = FACULTY_RANK_LEVEL_COUNT + 1,
    FACULTY_RANK_LEVEL_TOTAL = FACULTY_RANK_LEVEL_COUNT,
};
//##### fred 0928 #####//

#define MAX_RESEARCH_PROPOSAL     12
#define MAX_ACTIVE_RESEARCH_MONTH   12            // maximum number of months for a research
#define  MAX_PROPOSED_RESEARCH_MONTH  6

//------- Define class ResearchProposal ------//

enum {                                            // for ResearchProposal:status
    RESEARCH_PROPOSED, RESEARCH_ACTIVE
};

//!Structure describing costs and milestone dates for active and proposed
//!faculty research projects.
struct ResearchProposal {

    int total_dollars;                              // ($000)

    float overhead_rate;

    char  status;
    int date_to_next_status;                        // if active, it will be the date when the proposal is supposed to be complete
    // if proposed, it will be the date when a decision is made on whether it is accept or not
    // Notes 0830: add reseaerch overhead rate at the time each proposal is generated

    float prob_proposal_funded;
};

//---------- Define class Faculty ------------//

//!Class for representing a faculty member, including personal information,
//!salary, ongoing research, teaching, talent and performance. Responds to
//!Info::next_day()
class Faculty {
public:
    short faculty_recno;
    short department_recno;

    short photo_id;

    char* name();
    static char *name(short firstNameId, short middleNameId, short lastNameId);
    short  first_name_id;
    short middle_name_id;
    short last_name_id;

    char  rank_level;
    char  gender_ethnic_group;                    // constant values for this is defined in GAMEDEF.H
    int birthday;
    int age()     { return int( float(info.game_date - birthday) / 365.25 ); }

    // will be used by PeerSchool also
    static int  rank_age_group(char rank, int age);
    int     rank_age_group() { return rank_age_group(rank_level, age()); };

    static int  rank_age_group_2_rank(int rag);
    bool      is_regular();
    static bool is_adjunct_faculty(char rank) { return rank == LONG_TERM_ADJUNCT || rank == SHORT_TERM_ADJUNCT; };

    static int  get_min_age(int rankAgeGroup);

    int start_teaching_date;
    int   last_promotion_date;
    int   salary;                                 // NOT ($000) // annual income
    int last_salary_increase;                     //### fred 1030
    char* employed_period();

    char  teaching_contact_hour;                  // unit: hour	// also called teaching load
    float discretionary_hour_array[DISCRETIONARY_TYPE_COUNT];
    // var to be removed // discretionary time in the last trimester, for computation purposes
    char  last_discretionary_hour_array[DISCRETIONARY_TYPE_COUNT];
    float  total_hour();

    char  employ_status;
    char  is_dismissed;
    char  dismiss_trimester;

    float strain_priority;                        // for faculty incentive
    float strain_on_discretionary_time;           // result of faculty incentive

    //---------- research related var ---------------//

    // Notes 0830:
    // The effects of research on doctoral requirements, faculty discretionary time, faculty
    // research performance, and any other faculty activity variables are all based on direct
    // research dollars. The financial statements are based on total research dollars, so
    // they now can be calculated by summing the "total research" variable over faculty sims..

    ResearchProposal research_proposal_array[MAX_RESEARCH_PROPOSAL];

    bool  is_investigator;

    char  research_proposal_count;
    int research_month_expense;                   // ($000)  total monthly expenditures on sponsored rsrch prj
    int research_month_expense_direct;            // ($000)  // 1105

    //----------------------------------//

    char  off_duty_trimester;                     // either AUTUMN, WINTER, or SUMMER
    char  is_third_trimester_teaching;            // 0 or 1

    float reaction_summer_teaching, reaction_summer_teaching_last;

    //----------------------------------//

    float talent_teaching;                        //	a percentage from 0 to 100
    float talent_scholarship;
    float talent_research;

    float performance_teaching;                   //	a percentage from 0 to 100
    float performance_scholarship;
    float performance_research;
    float p_discretionary_hour_it_percent;
    float p_discretionary_hour_it;

    float time_shifted_scholarship;

    float satisfaction_index;                     //	a percentage from 0 to 100
    float satisfaction_index_last;
    float satisfaction_index_last_tri;

    //--- extra effect for chance card ---//

    float extra_effect_satisfaction_index;
    float extra_effect_performance_research;

    //-----------------------------------//

    // 990412 research project generation related vars

    float proj_count_mult;
    float proj_size_mult;
    float award_prob;

    float ave_proj_count;
    float ave_proj_size;

    //-----------------------------------//

public:

    void  init(int departmentRecno, int rankLevel, int genderEthnicGroup, int facultyAge, int startTeachingDate, int facultySalary,
	       int talentTeaching, int talentScholarship, int talentResearch);

    void  init_research();

    int write_file(File* filePtr);
    int read_file(File* filePtr);

    void  next_day();
    void next_trimester();

    void  calc_all_performance();

    void  disp_summary_basic(int x, int y, int xColumn2, int refreshFlag, int displayModeFlag);
    void  disp_summary_activity(int x, int y, int xColumn2, int refreshFlag);
    void  disp_summary_assess(int x, int y, int xColumn2, int refreshFlag);

    void  disp_detail_std(int refreshFlag);
    void  disp_research_proposal(int y, int refreshFlag);

private:
    void  init_photo();

    char* hour_str(float hourCount);

    int think_departure();
    int think_dismiss();
    int think_promotion();
    void  think_discretionary_time();
    void  think_research();

    void  calc_performance_teaching();
public:
    void  calc_performance_scholarship();
    void  calc_performance_research();
    void  calc_satisfaction_index();

private:
    void  calc_discretionary_hour_it();

public:
    int think_accept_retire(int retireOffer);
    void  update_history(char update_flag);
    float get_quality_driver();                   // 990412
};

//-------------------------------------------//
//----------- Define class FacultyArray -----//
//-------------------------------------------//

//!Collection Class for managing all of the members of the faculty.
//!Delegates events to each member of the faculty.
class  FacultyArray: public DynArrayB {
public:
    int     department_recno;
    int     faculty_count;

    //------------------------------------------//

public:
    FacultyArray();
    ~FacultyArray();

    int     write_file(File* filePtr);
    int     read_file(File* filePtr);

    int     add(int departmentRecno, int rankLevel, int genderEthnicGroup, int facultyAge, int startTeachingDate, int facultySalary,
		int talentTeaching, int talentScholarship, int talentResearch);

    void      del(int recNo, int isQuiting=0);

    int       avg_discretionary_hour(int discretionaryType);

    void      next_day();
    void      next_trimester();

    Faculty*  get_unpacked_faculty(int recNo);

    int     is_deleted(int recNo);

#ifdef DEBUG
    Faculty*  operator[](int recNo);
#else
    Faculty*  operator[](int recNo) { return (Faculty*) get(recNo); }
#endif

    //------------------------------------------//
    // performance indicators
public:
    //	a percentage from 0 to 100
    float talent_teaching[HISTORY_TRIMESTER_COUNT];
    float talent_scholarship[HISTORY_TRIMESTER_COUNT];
    float talent_research[HISTORY_TRIMESTER_COUNT];

    //	a percentage from 0 to 100
    float performance_teaching[HISTORY_TRIMESTER_COUNT];
    float performance_scholarship[HISTORY_TRIMESTER_COUNT];
    // BUGHRE change to monthly as in Faculty::update_history()
    float performance_research[HISTORY_TRIMESTER_COUNT];

    float satisfaction_index[HISTORY_MONTH_COUNT];//	a percentage from 0 to 100
    // percent faculty have research project
    char  percent_research[HISTORY_TRIMESTER_COUNT];

    // unit: hour
    float teaching_contact_hour[HISTORY_TRIMESTER_COUNT];
    float discretionary_hour_array[DISCRETIONARY_TYPE_COUNT_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    float time_shifted_scholarship;

    // ##### Begin Marco ##### //
    float initial_teaching_contact_hour;
    float initial_discretionary_hour_array[DISCRETIONARY_TYPE_COUNT_N_TOTAL];

    float initial_talent_teaching;                //	a percentage from 0 to 100
    float initial_talent_scholarship;
    float initial_talent_research;

    float initial_performance_teaching;           //	a percentage from 0 to 100
    float initial_performance_scholarship;
    float initial_performance_research;           // BUGHRE change to monthly as in Faculty::update_history()

    float initial_time_shifted_scholarship;
    // ##### End Marco ##### //

public:
    void  update_history(char update_flag);
    // ##### Begin MArco ##### //
    void  save_initial_data();
    // ##### End MArco ##### //

private:
    void  think_summer_teaching();
    void  calc_history_monthly();
    void  calc_history_trimester();
};

//------------------------------------------//
#endif
