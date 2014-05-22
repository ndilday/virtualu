//Filename    : OFACILIT.H
//Description : Facility Office Class Declaration
//              Refer to HE.Facilities.xls and techdoc 2.4
//Owner		  : Fred

#ifndef __OFacilit_H
#define __OFacilit_H

#include <GAMEDEF.H>

//----------- Define constants -------------//

//----------- Define class Facility --------------//
//
//!Class for simulating a facility office, including financing, parking and
//!crime models. See Tech Doc 2.4
//!(http://virtual-u.org/documentation/hetd_24.html)
class Facility {
public:
    //##### begin ho 980813 #####//
public:
    Facility();
    ~Facility();
    void init();
    void deinit();
    void report(int);
    int detect();
    int detect2();

    char current_mode;
    //##### end ho 980813 #####//

    int     write_file(File* filePtr);
    int     read_file(File* filePtr);

    //----- vars and methods related to simulation and statistics

public:
    //----- constants or other factors -----//
    // Debt capacity parameters
    //### begin zhoubin 02/25/99
    float spinner_var1,spinner_var3,debt_spinner_var1,debt_spinner_var2;
    float spinner_var4;
    //### end zhoubin 02/25/99
    // = 0.5f;
    float maximum_debt_balance_as_a_percent_of_funds_balances;
    // = 0.08f;
    float max_interest_payments_as_a_percent_of_operating_expenditures;

    // = 0.1f;
    float percent_gap_required_before_new_construction_is_considered;
    float escalation_factor_for_deferrd_maintenance_per_year;

    //----- for intermediate calculation -----//
    int total_normal_onm;
    float total_actual_new_debt;
    // G63
    float construction_project_spending[HISTORY_YEAR_COUNT];

    //-----------------------------------//

public:
    //----- player_input -----//
    int cur_debt_limit;
    float percent_project_funded_with_debt;       // 33.3 => 33.3%

    int min_capital_reserve_balance;              // new input

    //----- interface_output -----//

    int begin_backlog;                            // amount needed to keep up
    int cur_change_backlog;
    float facility_staff_morale;

    // this old var can be removed
    // G68
    float ratio_of_square_footage_to_benchmark[HISTORY_YEAR_COUNT];

    // for screen Space Usage
    float total_invest_close_gap;                 // $000		// updated monthly in calc_facility_staff_morale
    int normal_area[HISTORY_MONTH_COUNT],
	actual_area[HISTORY_MONTH_COUNT],
	projected_area[HISTORY_MONTH_COUNT];        // unit: square foot

    // for other screens
    int total_debt_capacity[HISTORY_YEAR_COUNT];  // E42 & E44
    // E44
    int outstanding_debt_balance[HISTORY_YEAR_COUNT];
    int debt_limit[HISTORY_YEAR_COUNT];           // is input also

    // O&M expense and backlog history with projected values
    int onm_n_backlog_history[HISTORY_YEAR_COUNT2];

    // three history in a graph
    int transfer_history[HISTORY_YEAR_COUNT];     // transfer to capital reserve
    int gifts_to_facility[HISTORY_YEAR_COUNT];
    float captial_reserve_expense[HISTORY_YEAR_COUNT];

    // ##### Begin MArco ##### //
    float initial_total_invest_close_gap;
    //	float	initial_captial_reserve_expense[HISTORY_YEAR_COUNT];
    float initial_captial_reserve_expense;
    int initial_crime_index;                      // 0-100
    int initial_normal_area;
    int initial_actual_area;
    int initial_projected_area;                   // unit: square foot
    // ##### End MArco ##### //

    //-----------------------------------//

public:
    void  init_data_pre_finance();
    void  init_data();
    void  update_history_yearly();
    void  next_day();
    void  run_new_construction();

    // ##### Begin Marco ##### //
    void  save_initial_data();
    void  begin_space_inventory();
    // ##### End Marco ##### //

private:
    void  update_history_sub();
    void  calc_facility_staff_morale();
    void  update_vars_monthly();

    //------------------------------------------//
    // Parking and Crime model
    //------------------------------------------//
public:
    int cur_parking_supply;
    int projected_parking_supply;
    int farout_capacity;

    int parking_demand[HISTORY_YEAR_COUNT];
    int parking_supply[HISTORY_YEAR_COUNT];
    char  crime_index[HISTORY_MONTH_COUNT];       // 0-100

    void  update_parking(bool init=false);
    void  update_crime(bool init=false);
};

extern Facility facility_office;

//-------------------------------------------------//
#endif
