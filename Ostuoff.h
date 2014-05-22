//Filename    : OStuOff.h
//Description : StudentOffice Class Declaration: includes Residence Hall
//Owner       : Fred

#ifndef __OStuOff_H
#define __OStuOff_H

// ES library header file

// CU header file
#include <GAMEDEF.H>

//----------- Define constants -------------//
//----------- Define enum types -------------//
//----------- Define struct -------------//

//----------- Define class Statistics -----//
//!Class which is responsible for managing student Residence hall, keeping
//!track of available and occupied beds, room and board rate, and how to
//!accommodate excess demand. Responds to new_day and read/write.
class StudentOffice {
public:
    float room_n_board_rate;                      // not ($000)

    //---------------- for interface in/out
public:
    // user input
    float percent_demanding_places_sl1;           // 33->33%; Percent of trad. UGs demanding places // = targe_sl1_in_hall_percent on screen

    // residence hall
    int available_bed_count[HISTORY_YEAR_COUNT];
    int occupied_bed_count[HISTORY_YEAR_COUNT];
    int tuition_rate_history[HISTORY_YEAR_COUNT]; // not ($000)
    int room_n_board_rate_history[HISTORY_YEAR_COUNT];

    // finance
    // ($000)
    int tuition_income_history[HISTORY_YEAR_COUNT];
    int student_life_history[HISTORY_YEAR_COUNT];

private:
    bool  accomodate_excess_demand;               // Do you wish to build residence halls to accomodate excess demand? (1=Y)
    //33%
    float percent_excess_demand_before_capacity_increased;
    int replacement_cost_per_student_space;       // not ($000)
    int replacement_cost_last2;
    float raw_initial_onm_cost_per_student_space;
    float raw_initial_variable_cost_per_occupied_space;

    int current_places;
    int demand_places;
    int filled_places;
    int new_places;

    float new_places_expense_last_year;           //($000)

public:

    //------------------------------------------//

public:
    StudentOffice();
    ~StudentOffice();

    int     write_file(File* filePtr);
    int     read_file(File* filePtr);

    void  init_data();

    void  next_day();

    //------------------------------------------//

private:
    void  update_history(char updateFlag=UPDATE_ALL);
    void  update_history_sub();
    void  run_residence_hall_yearly();
};

extern StudentOffice student_office;
#endif                                            //ifndef
