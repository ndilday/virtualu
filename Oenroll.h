//Filename    : OEnroll.h
//Description : EnrollRes Class Declaration
//Owner       : Fred

#ifndef __OEnroll_H
#define __OEnroll_H

// ES library header file
// CU header file
#include <GAMEDEF.H>                              // HISTORY_YEAR_COUNT
#include <OSTUDENT.H>                             // MAX_STUDENT_LEVEL, ACADEMIC

//----------- Define constants -------------//
#define MAX_TALENT_PRIORITY_LEVEL 10.0f
#define PRIORITY_LEVEL_COUNT      3               // interface related constant

//----------- Define enum types -------------//
enum {
    INSTITU_SEGMENT_COUNT = 7,
    PREFERECNE_COUNT = 20,
    NON_FINANCIAL_PREFERECNE_COUNT = 14,
    //FINANCIAL_PREFERECNE_COUNT = 3,		// = (20-14)/2		// see D129:147
    PREFERECNE_COUNT2 = 21,
    SCHOOL_CONTROL_COUNT = 2                        // PUBLIC & PRIVATE in opschool.h
};

enum {
    MAX_STUDENT_LEVEL_NO_SL1 = MAX_STUDENT_LEVEL - 1,
    STUDENT_SEGMENT_COUNT = 7,
    STUDENT_SUBSEGMENT_COUNT = 4,                   // equal to GENDER_ETHNIC_TYPE implicitly

    STUDENT_SEGMENT_ATHLETE = 3,                    // index to array
};

enum {
    MINORITY_GROUP_COUNT = 2,

    NON_MINORITY = 0,
    MINORITY,
};

enum {
    APP_YIELD_VAR_COUNT = 3,                        // numbers of applications/offers/matriculations

    APPS = 0, OFFERS, MATRICS,
};

enum {
    ER_YIELD_RATE = 0 , ER_OFFERS_RATE, ER_PERCENT_MINORITY, ER_ACADEMIC,
    ER_EXTRACURRICULAR, ER_ATHLETICS, ER_CREDITS_DEMANDED, ER_CREDITS_PROVIDED,

    HISTORY_GRAPH_COUNT_SL25 = ER_ACADEMIC+1,       // 4 graphs only
    // 8 graphs
    HISTORY_GRAPH_COUNT_GENERAL = ER_CREDITS_PROVIDED+1,
};

// for radio button input
enum {
    SET_LOW = 0,
    SET_MID,
    SET_HIGH,
};

//----------- Define struct -------------//

// refer to HE.initialization[stu_pref]
//!
struct PreferenceRec {
    char  value[INSTITU_SEGMENT_COUNT][9];
    char  weight[9];
};

//!
struct Preference {
    float average;
    float deviation;
    float weight;
    float tmp_fuzzy_value;
};

// refer to [HE.STU.pref]sheet1, struct for all student levels
//!Structures containing modeling parameters governing enrollment of
//!students are all levels.
struct PreferenceRec2 {
    char  weight[4][9];
};

//!
struct Preference2 {
    float apps_ratio, yield_rate;
};

// refer to [HE.initialization]Apps_Yield
//!Structure for Enrollment Module. Refer to [HE.initialization]Apps_Yield.
struct AppYieldRec {
    char  count[INSTITU_SEGMENT_COUNT][9];
};

// refer to [HE.STU.talent.xls]Sheet1
//!Structure used by the EnrollRes class to load average student talent and
//!its variance for all levels form the db.
struct TalentRec {
    char  seg[STUDENT_SEGMENT_COUNT][9];
};

//!Structure used by the EnrollRes class to store average student talent
//!and its variance.
struct Talent {
    float average;
    float variance;
};

//------- Define struct EnrollDataSL1 -----//
//!Structure for holding enrollment information.
struct EnrollData {
    float apps_ratio;                               //, yield_rate;
    int base_apps;
    float fraction_matrics;

    int applications;
    //int	offers;
    int matrics;

    float   student_pref;
    float   conversion_rate, base_conversion_rate;
    Talent  academic;
};

//!Structure for holding enrollment information.
struct EnrollDataSL1 {                            //: public EnrollData
    float apps_ratio, yield_rate;
    int base_apps;
    float base_yield_rate, fraction_matrics;

    int applications;
    int offers;
    int matrics;

    float util, util_variance;
};

//----------- Define class EnrollRes -----//
//!Simulates enrollment, including applications, offers, financial aid, and
//!matriculation. Used to get information for comparable schools.
class EnrollRes {
    // arrays read from dbs for enrollment
public:
    Preference  preference_array[PREFERECNE_COUNT][INSTITU_SEGMENT_COUNT];
    Preference2 preference_array2[SCHOOL_CONTROL_COUNT][MINORITY_GROUP_COUNT][PREFERECNE_COUNT2];
    short     app_yield_array[STUDENT_SUBSEGMENT_COUNT][STUDENT_SEGMENT_COUNT][INSTITU_SEGMENT_COUNT][APP_YIELD_VAR_COUNT];
    Talent    talent_array[STUDENT_SUBSEGMENT_COUNT][STUDENT_SEGMENT_COUNT][TALENT_INCOME_VAR_COUNT];

    //------------------------------------------//

public:
    EnrollRes();
    void  init();
    void  deinit();

    int write_file(File* filePtr);
    int read_file(File* filePtr);

    void  init_data();
    void  enroll_main(int initFlag=0);
    void  next_day();
    void  update_history(char update_flag);

    //------------------------------------------//

private:
    void  load_db_student_pref();
    void  load_db_student_pref2();
    void  load_db_student_talent();
    void  load_db_app_yield();
    void  update_income();

    //----------------------------------------------------//
    //-- data and methods for enrollment and other model -//
    //----------------------------------------------------//

public:
    // for interface on student turnover
    int   new_year_student_count[MAX_STUDENT_LEVEL];
    int last_year_student_count[MAX_STUDENT_LEVEL];
    // updated per trimester:
    int total_student_count;
    int student_count[MAX_STUDENT_LEVEL];
    float student_female_percent[MAX_STUDENT_LEVEL];
    float student_minority_percent[MAX_STUDENT_LEVEL];
    // for new formula of diversity
    float student_minority_female_percent[MAX_STUDENT_LEVEL];

public:
    void  calc_student_count(int initFlag=0);
    // 990427
    void  set_app_yield(float appMultipler, float yieldMultiplier);

    //----------------------------------------------------//
    //-- data and methods for PGI enrollment only		---//
    //----------------------------------------------------//
public:
    //----- for athletic office -----//
    int male_matrics_top_athletes;                // for all student level
    char  avg_athletic_talent_male_matrics;
    char  avg_athletic_talent_female_matrics;

    //----- for interface -----//
    int matrics_top_athletes;                     // for all student level
    float total_aid;                              // for all student level // ($000)
    int total_applications[MAX_STUDENT_LEVEL], total_offers[MAX_STUDENT_LEVEL], total_matrics[MAX_STUDENT_LEVEL];

    // rates in 0.999 for 99.9% format;  ug_applications_rate & ug_yield_rate defined in School will not be updated
    float applications_rate[MAX_STUDENT_LEVEL], cur_yield_rate[MAX_STUDENT_LEVEL];
    float offers_rate[MAX_STUDENT_LEVEL];

    int talent_average[MAX_STUDENT_LEVEL_N_TOTAL][TALENT_VAR_COUNT];
    //99 -> 99%
    float overall_yield_rate, overall_offers_rate, overall_apps_rate;

    // ##### Begin MArco ##### //
    float initial_offers_rate[MAX_STUDENT_LEVEL];
    float initial_cur_yield_rate[MAX_STUDENT_LEVEL];
    int initial_matrics_top_athletes;
    // ##### End MArco ##### //

    //----- intermediate vars -----//
private:
    // adjusted to thousands of dollars
    float max_aid;
    float need_lower, need_upper;
    float merit_lower[MAX_STUDENT_LEVEL], merit_upper[MAX_STUDENT_LEVEL];

    float fraction_on_aid[MINORITY_GROUP_COUNT];
    int average_aid[MINORITY_GROUP_COUNT];

    // for storing result of calc_student_pref()
    float pref_weight_array[INSTITU_SEGMENT_COUNT];
public:
    EnrollDataSL1 enroll_data_sl1[STUDENT_SUBSEGMENT_COUNT][STUDENT_SEGMENT_COUNT];
    EnrollData    enroll_data_sls[MAX_STUDENT_LEVEL_NO_SL1][STUDENT_SUBSEGMENT_COUNT];

private:

    //----- constants -----//

    static const float  yield_latency[MAX_STUDENT_LEVEL];
    static const float  apps_latency[MAX_STUDENT_LEVEL];
    static const float  aidMinMultipliers[PRIORITY_LEVEL_COUNT];

    //-------- define constants for the calc_applications and matriculations ----------//
    // s-function parameters from R33:35 in HE.STU.pref.xls
    float s_lower_bound;
    float s_upper_bound;
    float s_slope;

    // see R31 in HE.STU.pref.xls
    bool  pref_special_case(int x)  { return ( x==5 || x==7 || x==9 || x==15 || x==16 );  };

    float chance_avg;
    float chance_sd;
    float fraction_merit_top;

private:
    void  init_data_sl1();
    void  calc_talent_student_db();
    void  calc_student_pref();                    // for init_data_sl1
    void  calc_app_yield();                       // for init_data_sl1

public:
    void  calc_enroll_applications();
    void  calc_enroll_offers();
    void  calc_enroll_matriculations();
private:
    void  calc_post_enroll();
    void  calc_total_aid();
    void  calc_post_enroll_adjust_aid_all_sls();
public:
    void  update_enroll_history(char noshift=0);

    float calc_an_aid(Student *stuPtr);
    void  calc_a_talent(float talentArr[TALENT_VAR_COUNT], char sl, char subSeg, char seg=0);

    // ##### begin Marco ##### //
    void save_initial_data();
    // ##### End Marco ##### //

    //----------------------------------------------------//
    //-- data and methods for PGI enrollment sl2-5		---//
    //----------------------------------------------------//

    //intermediate vars
private:
    // a array read from db
    float preference_array2_sls[MAX_STUDENT_LEVEL_NO_SL1][SCHOOL_CONTROL_COUNT][MINORITY_GROUP_COUNT][PREFERECNE_COUNT2+1];
    float min_academic_rating[MAX_STUDENT_LEVEL_NO_SL1];

public:                                         // chwg0621
    float app_multiplier[MAX_STUDENT_LEVEL], yield_multiplier[MAX_STUDENT_LEVEL];

private:
    void  init_data_sls(char sl);
    void  calc_app_multiplier(char sl);

public:                                         // chwg0621
    void  enroll_main_sls(char sl);
    void  update_student_intake();
private:
    void  calc_post_enroll_sls(char sl);
    void  calc_enroll_aid_sls(char sl);

    //----------------------------------------------------//
    //-- data and methods for player's input and output --//
    //----------------------------------------------------//
public:
    // 1. player input for calc_*_aid
    float fraction_sl1_offered_merit_aid;
    float fraction_sl2_offered_aid;
    float fraction_sl5_offered_aid;               // askbill to add if
    char  minority_aid_special;                   // (* 2 = high; 1=medium; 0 = none *)	// see SET_HIGH
    char  athlete_aid_special;                    // (* 2 = high; 1=medium; 0 = none *)

    // 2. player input for calc_enroll_offers (admissions)
    char  offers_priority[TALENT_VAR_COUNT];      // = [0,MAX_TALENT_PRIORITY_LEVEL]
    char  minority_offers_special;                // (* 2 = high; 1=medium; 0 = none *)
    char  athlete_offers_special;                 // (* 2 = high; 1=medium; 0 = none *)
    char  distance_learner_acceptance;            // (* 2 = actively seek; 1 = accept; 0 = prohibit *)

    // 3. player input for generate student sims
    float fraction_need_covered;
    bool  aid_budget_enforced;                    //temp not used

    // 4. for screen Overview
    int target_student_intake[MAX_STUDENT_LEVEL];
    // chwg0810
    double target_student_intake_ratio[MAX_STUDENT_LEVEL];
    //## chea 040999 store the value for the last year target ribbon
    int last_year_target_student_intake[MAX_STUDENT_LEVEL];

    int target_enrollment[MAX_STUDENT_LEVEL];

    // values for interface output
    int overall_percent_minority;
    int overall_percent_female;
    int overall_percent_minority_female;          // for new formula of diversity
    int overall_gender_ethnic_group_student_count[GENDER_ETHNIC_TYPE_COUNT];
    int overall_percent_minority_matric;
    int matric_history[MAX_STUDENT_LEVEL_N_TOTAL][HISTORY_GRAPH_COUNT_GENERAL][HISTORY_YEAR_COUNT];
    int distance_credits_provided_history[HISTORY_TRIMESTER_COUNT];

private:
    void  calc_enroll_aid();
    void  player_change_offers_priority();
    void  calc_doctoral_intake_target();

    float calc_need_aid(float x);
    float calc_merit_aid(float x, float base);
    float calc_merit_aid_sls(char sl, float x, float base);
    void  adjust_fraction_merit_aid(float& fraction);
    float calc_academic_rating(char sl, char subSeg, float academicRating, float base);
};

extern EnrollRes enroll_res;
#endif                                            //ifndef
