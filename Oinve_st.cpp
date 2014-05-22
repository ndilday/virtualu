//Filename    : OStat.cpp
//Description : Investment Class Definition
//Owner       : Fred

// ES library header file
#include <OMATH.H>

// CU header file
#include <OINFO.H>
#include <OPSCHOOL.H>
#include <OFINANCE.H>
#include <ODEVELOP.H>
#include <OCHANCE.H>

#include "OINVEST.H"

//----------- Define constants -------------//
// no smoothing                   (=>lam = 0)
// 3-year smoothing               (=>lam = 0.45)
// 5-year smoothing               (=>lam = 0.60)

//## chea 241199 12.2.2 b)
//static float endowment_lambda[INPUT_OPTION_COUNT] = { 0, 0.45f, 0.60f };
static float endowment_lambda[INPUT_OPTION_COUNT] = { 0, 0.398f, 0.541f };

static float tmp_max = 1;

//----------- Begin of function Investment::init_data --------//
//!
void Investment::init_data() {
    // assume called: memset(this, 0, sizeof(Investment));

    //----------------------------//
    // data from excel sheet: response_func:investments

    //LC 50%	0.060	0.220
    //SC 20%	0.095	0.260
    //BD 30%	0.025	0.180

  /*
    ASSET_TYPE_COUNT = 3,

    AS_LC_STOCK=0,		// large-cap
    AS_SC_STOCK,		// small-cap
    AS_BONDS,

    int	allocation_percent;		// 33%
    float	return_mean;
    float	return_deviation;
  */

    asset_array[AS_LC_STOCK].allocation_percent = 50;
    asset_array[AS_SC_STOCK].allocation_percent = 20;
    asset_array[AS_BONDS].allocation_percent = 30;

    asset_array[AS_LC_STOCK].return_mean = 0.06f;
    asset_array[AS_SC_STOCK].return_mean = 0.095f;
    asset_array[AS_BONDS].return_mean = 0.025f;

    asset_array[AS_LC_STOCK].return_deviation = 0.22f;
    asset_array[AS_SC_STOCK].return_deviation = 0.26f;
    asset_array[AS_BONDS].return_deviation = 0.18f;

    //-----------------------------------------//

    int i, j;

    for (i=0; i < ASSET_TYPE_COUNT; i++)
	for (j=0; j < ASSET_TYPE_COUNT; j++) {
	    // should use a matrix of multipliers!!??
	    float multiplier;

	    if ( i == j)
		multiplier = 1;
	    else if ( ( i == AS_SC_STOCK && j == AS_LC_STOCK )  || ( j == AS_SC_STOCK && i == AS_LC_STOCK ) )
		multiplier = 0.9f;
	    else if ( ( i == AS_SC_STOCK && j == AS_BONDS ) || ( j == AS_SC_STOCK && i == AS_BONDS ) )
		multiplier = 0.3f;
	    else if ( ( i == AS_LC_STOCK && j == AS_BONDS ) || ( j == AS_LC_STOCK && i == AS_BONDS ) )
		multiplier = 0.4f;

	    asset_covariance_array[i][j] = multiplier * asset_array[j].return_deviation * asset_array[i].return_deviation;
	}

    s_n_p_history[THIS_MONTH] = 100;
    endowment_performance_history[THIS_MONTH] = 100;

    //s_n_p_history[THIS_MONTH] = 0;
    //endowment_performance_history[THIS_MONTH] = 0;

    //----------------------------//

    degree_smoothing_for_endowment_payout = 0;
    //	smoothed_endowment_value = finance.last_year.asset_array[AC_ENDOWMENT];  // inited in finance.init_data()

  //----------------------------//

    endowment_ratio = float(100*development_office.last_year.endowment / finance.last_year.asset_array[AC_ENDOWMENT]);

    endowment_change_year_to_date = 0;              // 990527

  //----------------------------//

  // expected_real_return.init(FA_HISTORY_YEAR, 0);

    float realReturn = 0;

    for (i=0; i < ASSET_TYPE_COUNT; i++)
	realReturn += asset_array[i].allocation_percent * ( asset_array[i].return_mean );

    expected_real_return.init(FA_HISTORY_YEAR, realReturn / 12.0f );

    update_history(UPDATE_MONTH);

                                                  // min & max bug chea
    tmp_max = max(1.0f, max(s_n_p_history[THIS_MONTH], endowment_performance_history[THIS_MONTH]));
    s_n_p_history[THIS_MONTH] /= tmp_max;
    endowment_performance_history[THIS_MONTH] /= tmp_max;

    int m = THIS_MONTH;
    s_n_p_history[m-1] = s_n_p_history[m-2] = s_n_p_history[THIS_MONTH];
    endowment_performance_history[m-1] = endowment_performance_history[m-2] = endowment_performance_history[THIS_MONTH];

  //----------------------------//

  //update_history(UPDATE_YEAR);		// equilibrium_payout_rate calced here
    calc_equilibrium_payout_rate();

    //----------------------------//

    err_when(! finance.last_year.asset_array[AC_ENDOWMENT]);
}

//------------- End of function Investment::init_data --------//

//----------- Begin of function Investment::calc_endowment_growth ------//
//!
//! Please refer to CC: WFM Design Notes, 19 September, 1998 section 2
//! to find out the algorithm proposed.
//!
void Investment::calc_endowment_growth() {
    int i,j;

    // nominal return // 33%
    float monthlyEndowmentGrowthRate = finance.inflation_rate_hist.get_cur();
    float realReturn = 0;

    for (i=0; i < ASSET_TYPE_COUNT; i++)
	realReturn += asset_array[i].allocation_percent * ( asset_array[i].return_mean );

    monthlyEndowmentGrowthRate += realReturn / 12.0f / 100.0f;

    //----------------------------//
    expected_real_return.shift_hist();
    expected_real_return.set_cur(realReturn / 12.0f);
    expected_real_annual_return_rate = expected_real_return.get_sum();

    //----------------------------//
    endowment_growthrate_deviation = 0;             // annual	// 0.99 -> 99%
    for (i=0; i < ASSET_TYPE_COUNT; i++)
	for (j=0; j < ASSET_TYPE_COUNT; j++)
	    endowment_growthrate_deviation += (asset_array[i].allocation_percent / 100.0f) * (asset_array[j].allocation_percent / 100.0f) * asset_covariance_array[i][j];

    endowment_growthrate_deviation = (float) sqrt(endowment_growthrate_deviation);

    // check mean = 5.7%, sd = 18.6%

    /* //## old idea
       float lamda = 0.5f;				// value from email req25.txtf
       float sd = 0.000084545f;		// for calc inflation_rate_hist in ofin_day.cpp
       float endowmentGrowthrateDeviationMonthly = 0;	// 0.99 (99%)
       endowmentGrowthrateDeviationMonthly = float( sqrt(endowment_growthrate_deviation - 12 * sd * sd / (1-lamda)) / sqrt(12) );

       float chance = 0;
       float endRand = math.get_random_snd(monthlyEndowmentGrowthRate/100.0f, endowmentGrowthrateDeviationMonthly*0.01f);
       float oldEndowment = (float) finance.this_year.asset_array[AC_ENDOWMENT];
       // 990525 finance.this_year.asset_array[AC_ENDOWMENT] = oldEndowment * ( 1 + endRand + chance );
       endowment_change_year_to_date += oldEndowment * ( endRand + chance );

       endowment_performance_history[THIS_MONTH] = float( (1+endRand) * endowment_performance_history[THIS_MONTH-1]);

       // endowment_performance_history[THIS_MONTH] /= tmp_max;
    */
    //## chea 231199 11.1.1
    float endowmentGrowthrateDeviationMonthly = 0;  // 0.99 (99%)
    endowmentGrowthrateDeviationMonthly = math.get_random_snd(monthlyEndowmentGrowthRate, endowment_growthrate_deviation/12);

    //chose up or down //## chea 031299 Bill only want it to go up
    int dir = m.random(2) ? 1:0 ;
    if(dir == 1)
	endowmentGrowthrateDeviationMonthly = -(endowmentGrowthrateDeviationMonthly);

    // add chance card here

    float endRand = math.get_random_snd(monthlyEndowmentGrowthRate, endowmentGrowthrateDeviationMonthly);

    //------ chance card handling ----------//

    if(chance_event.stock_dir == -1) {
	endRand = -float(100+m.random(400)) / 1000.f;
	chance_event.stock_dir = 0;
    }
    else if(chance_event.stock_dir == 1) {
	endRand = float(100+m.random(400)) / 1000.f;
	chance_event.stock_dir = 0;
    }

    //-------------------------------------//

    float oldEndowment = (float) finance.this_year.asset_array[AC_ENDOWMENT];

    endowment_change_year_to_date += oldEndowment * endRand;
    endowment_performance_history[THIS_MONTH] = float( (1+endRand) * endowment_performance_history[THIS_MONTH-1]);

    /*
      //------------------------------//
      // calc S&P 500

      float sapRand = math.get_random_snd(
      finance.inflation_rate_hist.get_cur() / 100.0f + asset_array[AS_LC_STOCK].return_mean - expected_real_return.get_cur() / 100.0f,
      //990507 asset_array[AS_LC_STOCK].return_deviation - endowmentGrowthrateDeviationMonthly);
      asset_array[AS_LC_STOCK].return_deviation / 12 - endowmentGrowthrateDeviationMonthly);

      //	s_n_p_history[THIS_MONTH] = float(( 1.0 + endRand + sapRand ) * s_n_p_history[THIS_MONTH-1]);
      // chwg0604
      double ir=0.6*sapRand+0.4*asset_array[AS_BONDS].return_mean;
      s_n_p_history[THIS_MONTH] = s_n_p_history[THIS_MONTH-1]*(float)(1.0f+(ir));

      // s_n_p_history[THIS_MONTH] /= tmp_max;
    */
}

//----------- End of function Investment::calc_endowment_growth ------//

//----------- Begin of function Investment::update_history_sub ------//
//! desc
//!
void Investment::update_history_sub() {

    /*

      refer to email req25b.txt

      >---------
      >And, will there be any special projection method used for the
      >two outyear projections?  Or just apply the related-algorithm
      >immediately after the player makes changes on interface?

      just apply the new payout rate, new smoothing-rule lambda-value, and
      current expected annual endowment total return (based on the current
      monthly endowment growthrate, which in turn depends on the asset
      allocations from overview-1 and the monthly inflation rate) to the existing
      formulas.

      i now remember the other item i talked with trevor about, which may have
      led tot he confusion about how may lines to include on the graph. what i'd
      like to do is project three values for each of the two outyears (next and
      second next), as follows:

      based on expected total return plus one standard deviation (annual)
      based on expected total return:
      based on expected total return minus one standard deviation (annual)

      the three lines will start at the same value (in the current year), then
      diverge with the first one being at the top and the last one at the bottom.
      note that the standard deviation depends on the current asset allocation.

    */

    //------- P_THIS_YEAR ----------//  //##chea 191099------- should be P_LAST_YEAR ----------//

    //	endowment_spending_history[PROJECTED_VALUE][P_THIS_YEAR] = (int)finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total;
    //	endowment_spending_history[PROJECTED_MAX_VALUE][P_THIS_YEAR] = (int)finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total;
    //	endowment_spending_history[PROJECTED_MIN_VALUE][P_THIS_YEAR] = (int)finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total;

    //## chea 191099
    //	endowment_spending_history[PROJECTED_VALUE][P_THIS_YEAR] = (int)finance.revenue_array[AC_ENDOWMENT_SPENDING].total;
    //	endowment_spending_history[PROJECTED_MAX_VALUE][P_THIS_YEAR] = (int)(int)finance.revenue_array[AC_ENDOWMENT_SPENDING].total;
    //	endowment_spending_history[PROJECTED_MIN_VALUE][P_THIS_YEAR] = (int)(int)finance.revenue_array[AC_ENDOWMENT_SPENDING].total;
    endowment_spending_history[PROJECTED_VALUE][P_THIS_YEAR] = (int)finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total;
    endowment_spending_history[PROJECTED_MAX_VALUE][P_THIS_YEAR] = (int)finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total;
    endowment_spending_history[PROJECTED_MIN_VALUE][P_THIS_YEAR] = (int)finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total;

    //------- init data ----------//

    const float annualReturnRate = expected_real_annual_return_rate;
    const float annualReturnDeviation = endowment_growthrate_deviation;
    const float payoutRate = float(finance.revenue_policy_array[PL_ENDOWMENT_SPENDING_RATE].result_value) / 100;
    float curEndowmentValue = (float) finance.this_year.asset_array[AC_ENDOWMENT]+(float) finance.this_year.asset_array[AC_QUASI];

    //------- P_NEXT_YEAR ----------//  //##chea 191099------- should be P_THIS_YEAR ----------//
    float   lam = endowment_lambda[degree_smoothing_for_endowment_payout];
    lam = 1.0f - lam;
    err_when(degree_smoothing_for_endowment_payout>INPUT_HIGH);

    curEndowmentValue *= (1+annualReturnRate/100.0f);
    float smoothedEndowmentValue = math.latency_func(lam, curEndowmentValue, smoothed_endowment_value);

    endowment_spending_history[PROJECTED_VALUE][P_NEXT_YEAR] = int( smoothedEndowmentValue * payoutRate);
    endowment_spending_history[PROJECTED_MAX_VALUE][P_NEXT_YEAR] = int( math.latency_func(lam, curEndowmentValue * (1+annualReturnDeviation), smoothed_endowment_value) * payoutRate );
    endowment_spending_history[PROJECTED_MIN_VALUE][P_NEXT_YEAR] = int( math.latency_func(lam, curEndowmentValue * (1-annualReturnDeviation), smoothed_endowment_value) * payoutRate );

    //## chea 191099 to update endowment_spending
    int temp1 = int( smoothedEndowmentValue * payoutRate);
    int temp2 = (int)finance.revenue_array[AC_ENDOWMENT_SPENDING].total;

    //## chea 241199 make the 3rd row the same 12.2.2 d)
    //	finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total = temp1;
    //	finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].change_last_year.total = finance.calc_change(temp1 , temp2);

    //------- P_2ND_NEXT_YEAR ----------//  //##chea 191099------- should be P_NEXT_YEAR ----------//
    curEndowmentValue *= (1+annualReturnRate/100.0f);
    smoothedEndowmentValue = math.latency_func(lam, curEndowmentValue, smoothed_endowment_value);

    endowment_spending_history[PROJECTED_VALUE][P_2ND_NEXT_YEAR] = int( smoothedEndowmentValue * payoutRate);
    endowment_spending_history[PROJECTED_MAX_VALUE][P_2ND_NEXT_YEAR] = int( math.latency_func(lam, curEndowmentValue * (1+annualReturnDeviation), smoothed_endowment_value) * payoutRate );
    endowment_spending_history[PROJECTED_MIN_VALUE][P_2ND_NEXT_YEAR] = int( math.latency_func(lam, curEndowmentValue * (1-annualReturnDeviation), smoothed_endowment_value) * payoutRate );

    //## chea 191099 to update endowment_spending

    int temp3 = int( smoothedEndowmentValue * payoutRate);
    int temp4 = (int)finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total;
    finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].next_year.total = temp3;
    finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].change_next_year.total = finance.calc_change(temp3 , temp4);

}

//------------- End of function Investment::update_history_sub ------//

//----------- Begin of function Investment::next_day ------//
//! desc
//!
void Investment::next_day() {
    // start of a month
    if(info.game_day == 29 && info.game_month == 8) {
	int asdfasdfasd=7;
    }

    if ( info.game_day == 1 ) {
	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month )
	    update_history(UPDATE_YEAR);

	update_history(UPDATE_MONTH);
    }

    // start of a trimester
    if ( info.game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	 && info.game_month == player_school.trimester_array[player_school.cur_trimester].start_month ) {
	update_history(UPDATE_TRIMESTER);
    }
}

//------------- End of function Investment::next_day ------//

//----------- Begin of function Athletics::update_history ------//
//!
//! update time-series graphs for interface display
//! A) win/lost; B) revenue history
//!
void Investment::update_history(char updateFlag) {
    //	float input;
    int i;

    switch (updateFlag) {
    case UPDATE_MONTH:
	shift_history(s_n_p_history, HISTORY_MONTH_COUNT);
	shift_history(endowment_performance_history, HISTORY_MONTH_COUNT);
	calc_endowment_growth();
	update_history_sub();
	break;

    case UPDATE_TRIMESTER:
	break;

    case UPDATE_YEAR:
	for (i=0; i<PROJECTION_TYPE_COUNT; i++) {
	    shift_history(endowment_spending_history[i], P_HISTORY_YEAR_COUNT);
	}

	calc_equilibrium_payout_rate();
	endowment_change_year_to_date = 0;          // 990527
	break;

    case UPDATE_ALL:
	//		update_history(UPDATE_MONTH);
	//		update_history(UPDATE_TRIMESTER);
	//		update_history(UPDATE_YEAR);
	//		break;

    default:
	err_here();
	break;
    }
}

//------------- End of function Athletics::update_history ------//

//----------- Begin of function Investment::deinit --------//
//!
//! refer to WFM notes 0919 section 3
//!
void Investment::calc_equilibrium_payout_rate() {
    float road, r, i, g, E, B, I;                   // 99%

    // assume called just after stage1 resource allocation, by opsch_pi.cpp for game score calculation

    // road: This is the "expected real annual return" described in Section 2.1 of the same document.

    road = (float) expected_real_annual_return_rate;
    r = float(Finance::calc_change(finance.projected_total_expense.this_year.total, finance.total_expense.total)
	      - finance.inflation_rate);

    float thisRev = float(finance.projected_total_revenue.this_year.total - finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total);
    float lastRev = float(finance.total_revenue.total - finance.revenue_array[AC_ENDOWMENT_SPENDING].total);

    i = float(Finance::calc_change(thisRev, lastRev) - finance.inflation_rate);
    g = endowment_ratio = math.latency_func(0.6f, endowment_ratio,
					    float(100*development_office.this_year.endowment / finance.this_year.asset_array[AC_ENDOWMENT]));
    E = float(finance.this_year.asset_array[AC_ENDOWMENT]);
    B = float(finance.projected_total_expense.this_year.total);
    I = thisRev;

    // solution of quadratic equation

    // min & max bug chea
    equilibrium_payout_rate = (float) sqrt(max(0.0f, math.safe_pow(road+g, 2) - 4 * (r*B-i*I) /E));
    // min & max bug chea
    equilibrium_payout_rate = (float) min(100.0f, max(0.04f, 0.5f * (road+g + equilibrium_payout_rate)));

    // return equilibrium_payout_rate;
}

//------------- End of function Investment::deinit --------//

//----------- Begin of function Investment::calc_smoothed_endowment_value --------//
//!
//! refer to email req25.txt
//!
void Investment::calc_smoothed_endowment_value() {

    /*
      Finally, having gotten into this area again, I'd like to suggest one more
      change while you're working on the Overview 2 screen and related
      procedures.

      **First, I'd like to define a new variable:

      smoothedEndowmentMV[t] =
      (1-lam)endowmentMarketValue[t]+lam*smoothedEndowmentMV[t-1]
      smoothedEndowmentMV[0] = endowmentMarketValue[0]

      **The new variable should be used in place of "endowmentMarketValue[t]"
      wherever it's multiplied by the payout rate: that would be in recourse
      allocation Stage I and in the projections discussed above. The original
      "endowmentMarketValue" should continue to be used in all financial
      statements.

      **The value of lambda should determined by Player input on the Overview 2
      screen via the following (new) 3-way radio button:

      float		lam = endowment_lambda[degree_smoothing_for_endowment_payout];

      err_when(degree_smoothing_for_endowment_payout>INPUT_HIGH);

      smoothed_endowment_value = math.latency_func(lam,
      float(finance.this_year.asset_array[AC_ENDOWMENT]), smoothed_endowment_value);

    */

    //## 241199 new idea from Bill
    //b)
    float   lam = endowment_lambda[degree_smoothing_for_endowment_payout];
    //a)
    smoothed_endowment_value = (float)((1.0f-lam) * finance.this_year.asset_array[AC_ENDOWMENT] + lam * smoothed_endowment_value);
    smoothed_quasi_endowment_value = (float)((1.0f-lam) * finance.this_year.asset_array[AC_QUASI] + lam * smoothed_quasi_endowment_value);
    //	smoothed_endowment_value = math.latency_func(lam,
    //		float(finance.this_year.asset_array[AC_ENDOWMENT]), smoothed_endowment_value);

}

//----------- End of function Investment::calc_smoothed_endowment_value --------//

// ##### Begin MArco ##### //
//----------- Begin of function Investment::save_initial_data --------//
void Investment::save_initial_data() {
    //	for (int i = 0; i < ASSET_TYPE_COUNT; i++)
    //		initial_asset_array[i] = asset_array[i];
    initial_asset_array0_allocation_percent = asset_array[0].allocation_percent;
    initial_asset_array1_allocation_percent = asset_array[1].allocation_percent;
    initial_asset_array2_allocation_percent = asset_array[2].allocation_percent;
    initial_endowment_performance = endowment_performance_history[THIS_MONTH];
}

//----------- End of function Investment::save_initial_data --------//
// ##### End MArco ##### //
