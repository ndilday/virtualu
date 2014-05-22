//Filename    : ODEPT.H
//Description : Department Class Declaration

#ifndef __ODEPT_H
#define __ODEPT_H

#ifndef __OFACULTY_H
#include <OFACULTY.H>
#endif

#ifndef __OSTUDENT_H
#include <OSTUDENT.H>
#endif

#ifndef __OCOURSE_H
#include <OCOURSE.H>
#endif

#ifndef __ODYNARRB_H
#include <ODYNARRB.H>
#endif

#include <ODEPTGEN.H>
#include <GAMEDEF.h>
#include <OFACURES.H>
#include <OCOURRES.H>

// for each year
#define DOCTORAL_AVERAGE_TEACHING_BREAKOUT_SECTIONS 1.5f

//----------- Define constant ---------------//
enum {
    EDUCATIONAL_QUALITY_VAR_COUNT = 7,
    ACADEMIC_STANDING_VAR_COUNT = 4,
    STUDENT_MORALE_VAR_COUNT = 1,
    FACULTY_RESEARCH_VAR_COUNT = 1,
    FACULTY_MORALE_VAR_COUNT = 1,
    FACULTY_DIVERSITY_VAR_COUNT = 1,
    TECH_UTILIZATION_VAR_COUNT = 1,
};

enum {
    TALENT_TEACH,
    TALENT_SCHOLARSHIP,
    TALENT_RESEARCH,

    TALENT_COUNT = 3,
};

enum {
    R_PROPOSAL,
    R_REJECT,
    R_ACCEPT,

    RESEARCH_STATUS_COUNT = 3,
};

//----------- Define class Department ---------------//

//!Subclass of GeneralDepartment. Contains methods for calculating
//!performance (financial as well as academic). As such, it responds to
//!Info::next_day(). Handles faculty generation, including hiring once the
//!games starts.
class Department : public GeneralDepartment {
public:
    FacultyArray  faculty_array;                  // faculty and students in this department
    CourseArray    course_array;

    DynArray    display_faculty_array;            // Use it for display faculty detail (new_hire, departures, promotions)
    DynArray    cur_faculty_array;                // Faculty data in current year

    char        first_write_offset;               // for file read write;

    char        department_recno;
    short       firm_recno;                       // the recno of the firm of this department

    //---------------// research related var

    int   total_research_dollar;                  // ($000) // sum of all faculty's research_month_expense in this department
    int   total_research_dollar_direct;           // ($000)
    int   research_dollar_norm;                   // ($000)

    char    percent_investigator;

    int   regular_faculty_count;

    //## fred 1030 ##//
    //---- priority discretionary time input by the player ----//

    // 0-10
    char    priority_discretionary_hour_array[DISCRETIONARY_TYPE_COUNT];
    // 0-10
    char    last_priority_discretionary_hour_array[DISCRETIONARY_TYPE_COUNT];
    char    pressure_to_change_teaching_load;     // =[-5,5]

    // 99->99%
    float   relative_priority_discretionary_hour_array[DISCRETIONARY_TYPE_COUNT];

    //---- player input for course ----//

    // 99->99%
    char    course_preference_input[COURSE_PREFERENCE_COUNT];
    // for sim & player ouput
    char    course_preference[COURSE_PREFERENCE_COUNT];

    //---- player input for faculty hiring and salary setting ----//

    float   dept_salary_multiplier;               // .99 -> 99% // as a fraction of the overall institutional percentage

    char    teaching_talent_priority;             // for new hires // = {Very important = 3; Important = 2; Moderately important = 1; Not important = 0}
    char    scholarship_talent_priority;
    char    research_talent_priority;
    char    cost_containment_priority;

    //	static int hiring_priority_input_2_multiplier(char in) { return IF(in==3,9,IF(in==2,4,in)); };   //## chea 131099
    static int hiring_priority_input_2_multiplier(char in) { return IF(in==3,20,IF(in==2,10,IF(in==1,5,in))); };

    char    assistant_professor_priority;         // labeled "New blood"   // = {Very important = 3; Important = 2; Doesn't matter = 1; Don't do it = 0}
    char    tenured_faculty_priority;             // labeled "Leadership"

    char    longterm_adjunct_faculty_priority;
    char    shortterm_adjunct_faculty_priority;
    char    female_faculty_priority;
    char    minority_faculty_priority;

    static int hiring_priority_level_input_2_multiplier(char in) { return IF(in==3,4,IF(in==2,1,IF(in==1,0,-100))); };

    //---- vars for faculty hiring ----//
    float   salary_multiplier[GENDER_ETHNIC_TYPE_COUNT];

    float   peer_salary_multiplier_rank[MAX_RANK_AGE_GROUP];
    float   peer_salary_multiplier_gender[GENDER_ETHNIC_TYPE_COUNT];
    float   peer_numeraire_salary;                // not ($000)

    float   peer_talent_rank[MAX_RANK_AGE_GROUP][TALENT_COUNT];
    float   peer_talent_gender[GENDER_ETHNIC_TYPE_COUNT][TALENT_COUNT];

    int   buget_average_salary;                   // 0116: for calc_max_hires; not ($000)

    //---- vars for salary setting ----//

    //---- vars for doctoral admission target ----//	// 1106 // Notes 0830 section 3
    int   init_doctor_count;
    int   init_breakout_section_count;
    float   doctors_per_research_dollar;
    float   breakout_section_by_doctor;

    //## fred 1030 ##//

    //------ performance indicators ------//

    float p_faculty_research;
    float p_academic_standing;                    // dept prestige
    float p_academic_standing_no_latency;         // dept prestige
    float p_tech_utilization;                     // course using it
    float p_educational_quality;
    float p_faculty_morale;
    float p_student_morale;                       // 980923: Student satisfaction with the department's academic program
    float p_faculty_diversity;

    // --- for store the sliders' value in course technology screen
    int department_incentives;
    int ease_outsourcing_credits;
    int distance_learning_credits;

    int distance_learning_enrollment;
    int total_distance_learning_place;
    int distance_credits_provided;

    // ##### Begin Marco ##### //
    float initial_p_faculty_morale;
    float initial_p_faculty_diversity;
    float initial_p_faculty_research;
    float initial_p_student_morale;
    float initial_p_academic_standing;
    float initial_p_educational_quality;

    // for sim & player ouput
    char  initial_course_preference[COURSE_PREFERENCE_COUNT];

    // Student
    //	int	initial_dept_student_level_history[MAX_STUDENT_LEVEL_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    //new
    int initial_dept_student_level_history[MAX_STUDENT_LEVEL_N_TOTAL];
    //	int	initial_dept_gender_student_level_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    //new
    int initial_dept_gender_student_level_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];

    char    initial_pressure_to_change_teaching_load;
    char    initial_priority_discretionary_hour_array[DISCRETIONARY_TYPE_COUNT];
    // #### End Marco ##### //

    int departures_history[HISTORY_YEAR_COUNT];
    int hires_history[HISTORY_YEAR_COUNT];
    int promotion_history[HISTORY_YEAR_COUNT];

    float cummulative_tech_utilization;           // course using it // 1104 // response_func var 64

    //## chea 270999 to update the student no. report.
    int   dept_student_level_no[MAX_STUDENT_LEVEL_N_TOTAL];
    int dept_student_level_history[MAX_STUDENT_LEVEL_N_TOTAL][HISTORY_TRIMESTER_COUNT];

    //## chea 290999 to update the student no. report.
    int   dept_gender_student_level_no[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    int dept_gender_student_level_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL][HISTORY_TRIMESTER_COUNT];

public:
    Department();
    void  init(int departmentId);

    int write_file(File* filePtr);
    int read_file(File* filePtr);
    static int  write_log_header(File *);
    int write_log(File *);

    char* name();

    void  main_report(int);
    void  main_report_deinit();
    int main_report_detect();

    // ## begin chwg 0909
    char  current_report_type;
    void  course_report(int);
    void  course_report_1(int);
    void  course_report_2(int);
    void  course_report_3(int);
    void  course_report_4(int);
    void  course_report_5(int);
    void  course_report_deinit();
    int course_report_detect();
    // ## end chwg 0909

    // ## begin chwg 031599
    void set_report_init_flag(int flag);
    void print_report();
    // ## end chwg 031599

    void  generate_faculty();
    void  generate_course();

    void  next_day();

    void  calc_all_performance_indicator();

    int   create_department_firm();

    void  init_hiring();
    void  faculty_hiring(bool hireNow=false);

    int faculty_new_hires_number();
    int faculty_departures_number();
    int faculty_promotions_number();

    // ##### Begin MArco ##### //
    void save_initial_data();
    // ##### End Marco ##### //

private:

    //---- functions for generating faculty ----//

    void  generate_faculty_one_group(int rankAgeGroup, int facultyCount);
    void  generate_faculty_calc_vars();
    void  generate_faculty_research_project(int facultyRecno, int rankAgeGroup);
    int   research_per_faculty(int rankAgeGroup);
    float weighted_adjusted_teaching_load(int rankAgeGroup);
    int   faculty_to_be_added(int rankAgeGroup);

    //----- functions for calculating performance indicators or graphs-----//

    void  calc_faculty_research();
    void  calc_academic_standing();
    void  calc_tech_utilization();
public:
    void  calc_educational_quality();
private:
    void  calc_faculty_morale();
    void  calc_student_morale();
    void  calc_faculty_diversity();

    void  calc_faculty_history();
    void  calc_research_history();

    void  calc_research_norm();
    // for calc research norm
    void  calc_faculty_count_by_rank_age(int *facultyCountArray);
    int get_regular_faculty_count();

public:
    void  init_proj_vars(Faculty* facPtr);        // 990413; for init faculty research
    void  calc_research_dollar();                 // 0118: public for finance
    void  update_history(char update_flag);

    //-------------------------------//
    // code for interface display		//
    //-------------------------------//
public:
    float p_educational_quality_history[HISTORY_TRIMESTER_COUNT];
    float p_academic_standing_history[HISTORY_YEAR_COUNT];
    float p_student_morale_history[HISTORY_TRIMESTER_COUNT];
    float p_faculty_research_history[HISTORY_MONTH_COUNT];
    float p_faculty_morale_history[HISTORY_MONTH_COUNT];
    float p_faculty_diversity_history[HISTORY_YEAR_COUNT];
    float p_tech_utilization_history[HISTORY_TRIMESTER_COUNT];
    float distance_credits_provided_history[HISTORY_TRIMESTER_COUNT];

    float var_educational_quality_history[EDUCATIONAL_QUALITY_VAR_COUNT][HISTORY_TRIMESTER_COUNT];
    float var_academic_standing_history[ACADEMIC_STANDING_VAR_COUNT][HISTORY_YEAR_COUNT];
    //rmif: to remove this var
    float var_student_morale_history[STUDENT_MORALE_VAR_COUNT][HISTORY_TRIMESTER_COUNT];

    float var_change_educational_quality[EDUCATIONAL_QUALITY_VAR_COUNT];
    float var_change_academic_standing[ACADEMIC_STANDING_VAR_COUNT];
    //rmif: to remove this var
    float var_change_student_morale[STUDENT_MORALE_VAR_COUNT];

    //----- history arrays below which can be placed in faculty_array and student_array; we place here for easy access
    int faculty_level_history[FACULTY_RANK_LEVEL_COUNT_N_TOTAL][HISTORY_YEAR_COUNT];
    int faculty_gender_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL][HISTORY_YEAR_COUNT];

    //----- history for research 1217 -----//
    //## chea 221199
    //	int	research_m_history[RESEARCH_STATUS_COUNT][HISTORY_MONTH_COUNT];		// ($000)
    //	int	research_y_history[RESEARCH_STATUS_COUNT][HISTORY_YEAR_COUNT];			// ($000)
    // ($000)
    float research_m_history[RESEARCH_STATUS_COUNT][HISTORY_MONTH_COUNT];
    // ($000)
    float research_y_history[RESEARCH_STATUS_COUNT][HISTORY_YEAR_COUNT];

    // ($000)
    int research_total_history[HISTORY_MONTH_COUNT];
    // ($000)
    int research_direct_history[HISTORY_MONTH_COUNT];

    // 99->99%
    int regular_faculty_with_research[HISTORY_MONTH_COUNT];

    // ##### begin Marco ##### //
    int initial_faculty_level[MAX_STUDENT_LEVEL_N_TOTAL];
    int initial_faculty_gender[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];

    // ($000)
    float initial_research_m[RESEARCH_STATUS_COUNT];
    // ($000)
    float initial_research_y[RESEARCH_STATUS_COUNT];
    int initial_research_total;                   // ($000)
    int initial_research_direct;                  // ($000)
    int initial_regular_faculty_with_research;    // 99->99%

    int initial_education_equality[7];
    int initial_academic_standing[4];
    // ##### End Marco ##### //

    //--------------//

    static const float var_educational_quality_weight[EDUCATIONAL_QUALITY_VAR_COUNT];
    static const float var_academic_standing_weight[ACADEMIC_STANDING_VAR_COUNT];
};

//------- Define class DepartmentArray ---------//

//!Collection class for Department. Delegates next_day() calls to the
//!departments, and handles aggregate academic information and interactions
//!with students (i.e., course selection).
class DepartmentArray : public DynArrayB {
public:
    int     department_count;

    int     selected_recno;
    int     last_selected_recno;                  // display the report of last_selected_recno department if there is no department currently selected
    char      current_screen_mode;
    char      current_pref_report_type;
    char      current_data_mode;

public:
    DepartmentArray();
    ~DepartmentArray();

    void      init();
    void      deinit();
    void      init_data();                        //### fred 0919

    int     write_file(File* filePtr);
    int     read_file(File* filePtr);
    int     write_log_header(File *);
    int     write_log(File *);

    int       add(int departmentId);
    void      del(int recNo);
    int     create_dept();

    void      next_day();

    int     is_deleted(int recNo);
    int     has_department(char* departmentName);

#ifdef DEBUG
    Department*   operator[](int departmentRecno);
#else
    //## fred
    Department*   operator[](int departmentRecno) {
	return (Department*) get_ptr(departmentRecno);
    }
#endif

    char current_report_type;

    //------------------------------------------//
    // player input or related directly
    // check 0830 design notes, section 2.3, for these 3 inputs.
    float weight_teaching;                        // = [0,100]
    float weight_scholarship;
    float weight_research;

    //---- the player inupt for **faculty hiring and salary setting** ----//
    // 0.99 -> 99% as a fraction of the overall institutional percentage
    float rank_age_group_salary_increase_multiplier[MAX_RANK_AGE_GROUP];

    float teaching_weight_input;                  // {Very important = 3; Important = 2; Moderately important = 1; Not at all important = 0}
    float scholarship_weight_input;
    float research_weight_input;
    float gender_ethnic_equity_weight_input;
    //## chea 280999 try to higher thr weight
    //	static int salary_weight_input_2_multiplier(char weightInput) { return IF(weightInput=3,6,IF(weightInput=2,3,weightInput)); };  org. idea
    static int salary_weight_input_2_multiplier(char weightInput) { return IF(weightInput=3,9,IF(weightInput=2,6,weightInput*3)); };

    //## chea 161099
    float k1con;
    float k2con;

    // for welcome letter
    int faculty_count_on_tenure_line;
    int faculty_count_tenured;

    int faculty_rank_in_teaching_talent;          // these 3 to be deleted
    int faculty_rank_in_scholarship_talent;
    int faculty_rank_in_research_talent;

    //------------------------------------------//
    void  salary_setting();
    void  faculty_hiring();
    void  calc_course_pref();
    void  calc_faculty_student_history();         // 0406

    //------------------------------------------//
    // performance indicators
public:
    float p_educational_quality_history[HISTORY_TRIMESTER_COUNT];
    float p_academic_standing_history[HISTORY_YEAR_COUNT];
    float p_student_morale_history[HISTORY_TRIMESTER_COUNT];
    float p_faculty_research_history[HISTORY_MONTH_COUNT];
    float p_faculty_morale_history[HISTORY_MONTH_COUNT];
    float p_faculty_diversity_history[HISTORY_YEAR_COUNT];

    int p_educational_quality_sort[MAX_DEPARTMENT];
    int p_academic_standing_sort[MAX_DEPARTMENT];
    int p_student_morale_sort[MAX_DEPARTMENT];
    int p_faculty_research_sort[MAX_DEPARTMENT];
    int p_faculty_morale_sort[MAX_DEPARTMENT];
    int p_faculty_diversity_sort[MAX_DEPARTMENT];

    int total_student_count;
    int total_faculty_count;
    int faculty_level_history[FACULTY_RANK_LEVEL_COUNT_N_TOTAL][HISTORY_YEAR_COUNT];
    int faculty_gender_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL][HISTORY_YEAR_COUNT];
    int student_level_history[MAX_STUDENT_LEVEL_N_TOTAL][HISTORY_YEAR_COUNT];
    int student_gender_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL][HISTORY_YEAR_COUNT];

    // unit: hour
    float teaching_contact_hour[HISTORY_TRIMESTER_COUNT];
    float discretionary_hour_array[DISCRETIONARY_TYPE_COUNT_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    float time_shifted_scholarship;

    int average_research_dollars[FACULTY_RANK_LEVEL_COUNT];

    //----- history for research 1217 -----//
    //## chea 221199
    //	int	research_m_history[RESEARCH_STATUS_COUNT][HISTORY_MONTH_COUNT];		// ($000)
    //	int	research_y_history[RESEARCH_STATUS_COUNT][HISTORY_YEAR_COUNT];			// ($000)
    // ($000)
    float research_m_history[RESEARCH_STATUS_COUNT][HISTORY_MONTH_COUNT];
    // ($000)
    float research_y_history[RESEARCH_STATUS_COUNT][HISTORY_YEAR_COUNT];

    // ($000)
    int research_total_history[HISTORY_MONTH_COUNT];
    // ($000)
    int research_direct_history[HISTORY_MONTH_COUNT];

    // 99->99%
    int regular_faculty_with_research[HISTORY_MONTH_COUNT];

    //----- pi 980929 --------------------//
    //	a percentage from 0 to 100
    float talent_teaching[HISTORY_TRIMESTER_COUNT];
    float talent_scholarship[HISTORY_TRIMESTER_COUNT];
    float talent_research[HISTORY_TRIMESTER_COUNT];

    //	a percentage from 0 to 100
    float performance_teaching[HISTORY_TRIMESTER_COUNT];
    float performance_scholarship[HISTORY_TRIMESTER_COUNT];
    // BUGHRE change to monthly as in Faculty::update_history()
    float performance_research[HISTORY_TRIMESTER_COUNT];

    //---------- data for interface -------------//
    int class_section_count[MAX_TEACHING_METHOD_N_TOTAL];
    int convention_student_count[MAX_TEACHING_METHOD_N_TOTAL];
    int average_class_size[MAX_TEACHING_METHOD_N_TOTAL];
    int normal_class_size[MAX_TEACHING_METHOD_N_TOTAL];

    int size_deviation[MAX_TEACHING_METHOD_N_TOTAL];
    int admission_denied_count[MAX_TEACHING_METHOD_N_TOTAL];
    int failure_count[MAX_TEACHING_METHOD_N_TOTAL];
    int outsourcing_credits[MAX_TEACHING_METHOD_N_TOTAL];
    int experienced_class_size[HISTORY_TRIMESTER_COUNT];

    int class_section_count_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    int convention_student_count_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    int size_deviation_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    int admission_denied_count_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    int failure_count_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];
    int outsourcing_credits_history[MAX_TEACHING_METHOD_N_TOTAL][HISTORY_TRIMESTER_COUNT];

    float salary_gender_ethnic_equity_history[HISTORY_YEAR_COUNT];
    float salary_departmental_equity_history[HISTORY_YEAR_COUNT];
    float salary_rank_equity_history[HISTORY_YEAR_COUNT];

    int initial_class_section_count[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_convention_student_count[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_average_class_size[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_size_deviation[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_admission_denied_count[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_failure_count[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_normal_class_size[MAX_TEACHING_METHOD_N_TOTAL];
    int initial_experienced_class_size;

    int initial_faculty_level[MAX_STUDENT_LEVEL_N_TOTAL];
    int initial_faculty_gender[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];

    float initial_talent_teaching;                //	a percentage from 0 to 100
    float initial_talent_scholarship;
    float initial_talent_research;

    float initial_performance_teaching;           //	a percentage from 0 to 100
    float initial_performance_scholarship;
    float initial_performance_research;           // BUGHRE change to monthly as in Faculty::update_history()

    // ($000)
    float initial_research_m[RESEARCH_STATUS_COUNT];

    float initial_p_educational_quality;
    float initial_p_academic_standing;
    float initial_p_faculty_research;
    float initial_p_faculty_morale;
    float initial_teaching_contact_hour;          // unit: hour
    float initial_discretionary_hour_array[DISCRETIONARY_TYPE_COUNT_N_TOTAL];
    float initial_time_shifted_scholarship;

    // ($000)
    float initial_research_y[RESEARCH_STATUS_COUNT];
    int initial_research_total;                   // ($000)
    int initial_research_direct;                  // ($000)

    int initial_regular_faculty_with_research;    // 99->99%

    float initial_salary_gender_ethnic_equity;
    float initial_salary_departmental_equity;
    float initial_salary_rank_equity;

    //-------------------------//

public:
    void  update_history(char update_flag);
    void  init_research();
    void  p_sort_performance();
    void  calc_course_history();
    void  calc_research_history();

    void  save_initial_data();
    void  calc_salary_equity();
};

extern DepartmentArray department_array;

//---------------------------------------------//
#endif