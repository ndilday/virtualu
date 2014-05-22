//Filename    : OINVEST.H
//Description : Athletics Office Class Declaration
//Owner		  : Kevin(chwg)

#ifndef __OINVESTMENT_H
#define __OINVESTMENT_H

#include <GAMEDEF.H>                              //### fred
#include <OHISTORY.H>                             //### fred 1023

//------- define constants for the special 3 yrs projected graph on screen ------//
enum {
    P_HISTORY_YEAR_COUNT = 8,
    P_HISTORY_PROJECTED_YEAR_COUNT = 2,             // this, next and second next year
    P_HISTORY_PASSED_YEAR_COUNT = P_HISTORY_YEAR_COUNT - P_HISTORY_PROJECTED_YEAR_COUNT,

    P_THIS_YEAR = P_HISTORY_PASSED_YEAR_COUNT - 1,
    P_NEXT_YEAR,
    P_2ND_NEXT_YEAR,
};

enum {
    PROJECTION_TYPE_COUNT=3,

    PROJECTED_VALUE = 0,
    PROJECTED_MIN_VALUE,
    PROJECTED_MAX_VALUE,
};

enum {
    ASSET_TYPE_COUNT = 3,

    AS_LC_STOCK=0,                                  // large-cap
    AS_SC_STOCK,                                    // small-cap
    AS_BONDS,
};

//----------------------//

//!Structure for Investment module.
struct AssetItem {
    int allocation_percent;                         // 33%
    float return_mean;                              // 0.33
    float return_deviation;                         // 0.33
};

//----------- Define class Investment --------------//
//
//!Class used to simulate investments as specified in the financial model.
//!Includes methods for tracking performance, growth, history,
//!expectations, and supports reporting and loading/saving.
class Investment {

public:
    Investment();
    ~Investment();
    void init();
    void deinit();
    void report(int);
    int detect();
    int detect2();

    //####### fred 981022 #########//
    char current_mode;
    int     write_file(File* filePtr);
    int     read_file(File* filePtr);

    //--------------------------------------//
    // simulation-related vars and methods

public:
    //-------- for screen "overview1" ----------//

    AssetItem asset_array[ASSET_TYPE_COUNT];

    //### begin zhoubin 02/28/99
    float spinner_var1;
    //### end zhoubin 02/28/99
    float endowment_performance_history[HISTORY_MONTH_COUNT];
    float s_n_p_history[HISTORY_MONTH_COUNT];

    //-------- for screen "overview2" ----------//

    // number of valid entries = min(P_HISTORY_YEAR_COUNT, info.year_passed+3);
    int endowment_spending_history[PROJECTION_TYPE_COUNT][P_HISTORY_YEAR_COUNT];
    float equilibrium_payout_rate;

    // "Degree of smoothing for endowment payout":
    // no smoothing
    // 3-year smoothing
    // 5-year smoothing
    char  degree_smoothing_for_endowment_payout;  // INPUT_LOW, INPUT_MEDIUM, INPUT_HIGH

    //-------- not for interface ----------//

    float smoothed_endowment_value;
    float endowment_change_year_to_date;          // 990527

    float smoothed_quasi_endowment_value;

    // ##### Begin Marco ##### //
    //	AssetItem	initial_asset_array[ASSET_TYPE_COUNT];
    int initial_asset_array0_allocation_percent;
    int initial_asset_array1_allocation_percent;
    int initial_asset_array2_allocation_percent;
    float initial_endowment_performance;
    // ##### End Marco ##### //

private:
    float asset_covariance_array[ASSET_TYPE_COUNT][ASSET_TYPE_COUNT];

    HistoryData expected_real_return;             // 99%
    float endowment_growthrate_deviation;         // 0.99 (99%)
    float endowment_ratio;

public:
    float expected_real_annual_return_rate;       // 99%			// nominal total return = inflation plus real return

    void  init_data();
    void  next_day();
    void  calc_smoothed_endowment_value();

    // ##### Begin Marco ##### //
    void  save_initial_data();
    // ##### End Marco ##### //
    //------------------------------------------//

private:
    void  calc_endowment_growth();
    void  calc_equilibrium_payout_rate();
    void  update_history_sub();
    void  update_history(char updateFlag=UPDATE_ALL);
    //####### fred 981022 #########//
};

extern Investment investment_office;

//-------------------------------------------------//
#endif
