//Filename    : OLETTER.H
//Description : Help Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OLETTER_H
#define __OLETTER_H

//----------- Define class Development --------------//
//
//!Class controlling content and display of progress letters from the Board
//!of Trustees displayed to the player, including welcome letter and yearly
//!evaluation letter with statistics.
class Letter {
    int department_array_faculty_array_faculty_count[MAX_DEPARTMENT];
    int department_array_student_array_student_count[MAX_DEPARTMENT];
    int department_array_total_research_dollar[MAX_DEPARTMENT];
    int department_array_faculty_level_history_FT;
    int department_res_general_dept_student_level_historyMT;
    int enroll_res_total_student_count;

    float student_office_percent_demanding_places_sl1;
    float school_res_player_peer_school_percent_get_bacc_in_5_year;
    float enroll_res_applications_rate_UG_TRADITION;
    int enroll_res_student_count_UG_TRADITION;
    int enroll_res_student_count_UG_NONTRADITION;
    int enroll_res_student_count_DISTANCE_LEARN;
    int enroll_res_student_count_MASTER;
    int enroll_res_student_count_DOCTOR;
    int department_res_general_dept_student_array_student_count;
    int enroll_res_overall_percent_minority;
    int enroll_res_overall_percent_female;

    double department_array_faculty_count_on_tenure_line;
    double department_array_faculty_count_tenured;
    int department_array_faculty_gender_history_MINORITY_MALE_HISTORY_YEAR_COUNT_1;
    int department_array_faculty_gender_history_MINORITY_FEMALE_HISTORY_YEAR_COUNT_1;
    int department_array_faculty_gender_history_GENDER_ETHNIC_TYPE_COUNT_N_TOTAL_1_HISTORY_YEAR_COUNT_1;
    int department_array_faculty_gender_history_NONMINORITY_FEMALE_HISTORY_YEAR_COUNT_1;

    double player_school_athletic_program_intensity;

    double off_basketball_probability_win;
    double off_football_probability_win;
    double off_basketball_game_count_off_ncaa_level_input;
    double off_football_game_count_off_ncaa_level_input;
    double finance_total_expense_total;
    double finance_last_year_surplus_deficit;

    double finance_last_year_asset_array_AC_OPERATING_RESERVE;

    String eval_page_1;
    String eval_poor_areas;
    String eval_good_areas;
    String eval_mixed_areas;
    String eval_page_1_p2;

    long letter_date;

public:
    Letter();
    ~Letter();

    int write_file(File* filePtr);                //## chea 990519
    int read_file(File* filePtr);                 //## chea 990519

    char eval_page_number;
    char welcome_page_number;
    char scen_page_number;
    char state_page_number;

    char should_end_game;

    void init();
    void deinit();
    void scen_welcome_letter_report(int);
    void welcome_letter_report(int);
    void eval_letter_report(int);
    void state_report(int);

    void get_variables_score(char ascend);
    int scen_welcome_letter_detect();
    int welcome_letter_detect();
    int eval_letter_detect();
    int state_report_detect();

    void init_scen_welcome_letter();
    void init_welcome_letter();
    void init_eval_letter();
    void init_state_report();

private:
    int eval_letter_show_first_page(int generateText=0);
    int eval_letter_show_poor_area(int justCalc=0, int generateText=0);
    int eval_letter_show_good_area(int justCalc=0, int generateText=0);
    int eval_letter_show_mixed_area(int justCalc=0, int generateText=0);
};

extern Letter letter;

//-------------------------------------------------//
#endif
