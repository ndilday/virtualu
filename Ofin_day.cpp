//Filename    : OFinance.cpp
//Description : Finance Class Definition (functions support next_day())
//Owner       : Fred

// update values for interface; refer to "design doc June 5.doc" p.9 - p.11

// ES library header file
// CU header file
#include <OMATH.H>                                // get_random_snd()
#include <OCONFIG.H>
#include <OAUDIO.H>
#include <OSYS.H>                                 // .signal_exit_flag
#include <OBOX.H>
#include <OVGA.H>
#include <ODATE.H>
#include <OINFO.H>                                // info.game_day
#include <OOPT.H>
#include <ODEPT.H>                                // for ::update_projected_arrays_monthly()
#include <OFACULTY.H>                             // for ::update_projected_arrays_monthly()
#include <OPEERSCH.H>                             // for schoolres.player_peer_school
#include <OSCHLRES.H>
#include <OPSCHOOL.H>
#include <OFACILIT.H>                             // total_debt_capacity
#include <OINVEST.H>
#include <ODEVELOP.H>
#include <OFINANCE.H>
#include <OCHANCE.H>
#include <OSTUDENT.H>
#include <OSTUOFF.H>
#include <OLIBTECH.H>
#include <OLOG.H>

//define	DEBUG_REPORT_NOT

//----------- Define constants -------------//
//----------- Define static variables -------------//
static bool terminate_if_bad_finance = true;

//----------- Define static class member variables -------------//

//----------- Define static functions -------------//

static bool ask_quit_game(char *str) {
    String strOut(str);
    //strOut += "  Quit this game ?";

    //return box.ask(strOut) == 1;

    box.msg(strOut);

    return false;
}

static bool ask_quit(char *str, bool toQuit) {
    if ( sys.signal_exit_flag )
	return true;

    //playsound
    audio.play_wav("NEWS",audio.int_to_DsVolume(config.sound_effect_volume));

    // call interface to ask "end this game?"

    vga.use_front();                                // 1117
    toQuit = toQuit || ask_quit_game(str);
    vga.use_back();                                 // 1117

    if ( toQuit ) {
	sys.signal_exit_flag = 1;
	return true;
    }
    else {
	sys.redraw_all_flag = 1;
	return false;
    }
}

static char get_sign(double v) {
    if ( v == 0 )
	return NO_SIGN;
    else
	return (v > 0 ? POSITIVE : NEGATIVE );
}

//----------- End Define static functions -------------//

//----------- Begin of function Finance::optimization_input_reset -------------//
//!
void Finance::optimization_input_reset() {
    int i;

    for ( i=0; i<REVENUE_POLICY_COUNT; i++ )
	revenue_policy_array[i].reset_change();

    for ( i=0; i<EXPENSE_POLICY_COUNT; i++ )
	expense_policy_array[i].reset_change();

    for ( i=0; i<COST_RISE_POLICY_COUNT; i++ )
	cost_rise_policy_array[i].reset_change();

    FinancePolicy *policy;

    switch ( player_school.scenario_id ) {
    case ( SCN_LIMIT_TUITION_GROWTH ):
	policy = &(revenue_policy_array[PL_TUITION_RATE_GROWTH]);
	policy->upper_limit = 0;
	policy->upper_bound = 0;
	policy->target_value = 0;
	policy->bound_correction();
	break;
    }
}

//----------- End of function Finance::optimization_input_reset -------------//

/*
//----------- Begin of function Finance::calc_change -------------//
//!
//! for column "change" in reports
//!
double Finance::calc_change(double dest, double base)
{
  if ( base > 1E-5)
    return (dest - base) * 100 / base;
  else
    return 0;
}
//----------- Begin of function Finance::calc_change --------//
*/

//----------- Begin of function Finance::calc_change replace ABOVE -------------//
//!## chea 170899
//! for column "change" in reports
//!
double Finance::calc_change(double dest, double base) {
    //---- if both are negative, reverse them to both positive ---//

    if( dest < 0 && base < 0 ) {
	dest = -dest;
	base = -base;
    }

    //----------------------------------------------//

    double diff,locdest,locbase,result;
    locdest = dest;
    locbase = base;

    if(dest == 0 || dest == base)
	return 0;

    //----------------------------------------//

    diff = dest - base;

    if (diff <0)
	diff = -(diff);

    if (locbase <0)
	locbase =- (locbase);

    if(locbase == 0)                                //## chea avoid divided by 051099 0
	return 0;

    result = diff/locbase*100;

    if (dest<base)
	return -result;
    else
	return result;

    return 0;                                       //## chea if unknown value is passed

}

//----------- END of function Finance::calc_change_local --------//

//----------- Begin of function Finance::get_available_funds --------//
double Finance::get_available_funds() {
    float opReserve = (float) finance.this_year.asset_array[AC_OPERATING_RESERVE];

    float availableFunds = float( opReserve
				  + 0.5*( this_year.asset_array[AC_ENDOWMENT]
					  +this_year.asset_array[AC_CAPITAL_RESERVE] )
				  + 0.25 * this_year.asset_array[AC_BUILDINGS]
				  - this_year.liability_array[AC_LIABILITY_TOTAL]);

    return max(0.0f,availableFunds);                //min & max bug chea
}

//----------- Begin of function Finance::get_available_funds --------//

//----------- Begin of function Finance::next_day --------//
//!
void Finance::next_day() {
    //-------- Chance Card special handling ---------//

    if (chance_event.dept_technology_transfer_start_year == info.game_year &&
	chance_event.dept_technology_transfer_start_month == info.game_month && info.game_day==16) {
	//update screen
	budget_revenue_array[AC_OTHER_OPERATING_INCOME].total += chance_event.dept_technology_transfer_research_amount;
	projected_revenue_array[AC_OTHER_OPERATING_INCOME].this_year.total += chance_event.dept_technology_transfer_research_amount;
	sys.redraw_all_flag=1;
    }

    if (chance_event.dept_technology_transfer_start_year+3 == info.game_year &&
	chance_event.dept_technology_transfer_start_month == info.game_month && info.game_day==16) {
	budget_revenue_array[AC_OTHER_OPERATING_INCOME].total -= chance_event.dept_technology_transfer_research_amount;
	projected_revenue_array[AC_OTHER_OPERATING_INCOME].this_year.total -= chance_event.dept_technology_transfer_research_amount;

	box.msg("This is the last year of your sponsorship deal. ");
	chance_event.dept_technology_transfer_start_year=0;
    }

    if ( chance_event.research_prize_happen_date == info.game_day-1 && chance_event.research_prize_happen_month == info.game_month
	 //game_day-1 because ochance run first
	 && chance_event.research_prize_happen_year == info.game_year) {
	development_office.this_year.total *= m.fmin(1.10f, math.get_random_snd(1.15f, PSCH_SD(0.05f)));
    }

    // calculate the upper bound of indirect cost rate, for reset in 1st sept

    // info.game_day never 31
    if( date.day(info.game_date) == 30 && info.game_month == 8 ) {
	// calc the new value of the upper bound of indirect cost rate,
	// will be set at the first day of next year

	double costRatio = math.safe_divide( budget_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total,
					     budget_expense_array[AC_ACADEMIC_DEPARTMENTS].total + budget_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total );
	double costRecovery = (budget_expense_array[AC_IT_RESOURCES].total
			       + budget_expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].total
			       + budget_expense_array[AC_OPERATIONS_AND_MAINTENANCE].total) * costRatio;

	next_indirect_cost_rate_upper_bound = math.safe_divide( costRecovery, budget_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].total ) * 100.0;
    }

    // now reset upper_bound change

    if( !(info.prerun_year && info.game_year == 1)  // skip start of prerun year
	&& info.game_day == 1 && info.game_month == 9 ) {
	FinancePolicy* policy = &revenue_policy_array[PL_INDIRECT_COST_RATE];
	policy->upper_bound = next_indirect_cost_rate_upper_bound;
	double oldResultValue = policy->result_value; // don't change result value
	policy->bound_correction();
	policy->result_value = oldResultValue;
    }

    // reset required flag of surplus/deficit, and indirect cost rate

    if( info.prerun_year && info.game_year == 2
	// start of first year
	&& info.game_day == 1 && info.game_month == 9 ) {
	expense_policy_array[PL_SURPLUS_DEFICIT].required_flag = 0;
	revenue_policy_array[PL_INDIRECT_COST_RATE].required_flag = 0;
    }

    //---------- run finance monthly -----------//

    if ( info.game_day != 1 )
	return;

    month_left--;

    //--------- if it's not the start of a new financial year --------//

    if ( info.game_month != fiscal_year_start_month ) {
	// IMPORTANT asumption: the start date of a game is
	// equal to the start of financial year.

	err_when(month_left <= 0 && month_left >= 12);
	update_projected_arrays_monthly();

	// override budget to current projected value
	if( info.prerun_year && info.financial_year() == 1 && info.game_month == 8 ) {
	    projected_expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].this_year.staff *= 0.5;
	    projected_expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].this_year.other *= 0.5;

	    projected_expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].this_year.total =
		projected_expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].this_year.faculty
		+ projected_expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].this_year.staff
		+ projected_expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].this_year.other;

	    // calc total revenue
	    double totalRevenue = 0.0;
	    for( int i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
		if( i != AC_NET_TUITION_INCOME )
		    totalRevenue += projected_revenue_array[i].this_year.total;
	    }
	    double totalExpense = 0.0f;
	    for( int i=0 ; i<EXPENSE_ITEM_COUNT ; i++ )
		totalExpense += projected_expense_array[i].this_year.total;

	    projected_revenue_array[AC_OTHER_OPERATING_INCOME].this_year.total
		+= totalExpense * ( 1.0 + expense_policy_array[PL_SURPLUS_DEFICIT].target_value / 100.0 ) - totalRevenue;

	    for( int i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
		if( i == AC_SPONSORED_RESEARCH_REVENUE ) {
		    projected_revenue_array[i].last_month.direct = projected_revenue_array[i].this_year.direct / 12.0;
		    projected_revenue_array[i].last_month.indirect = projected_revenue_array[i].this_year.indirect / 12.0;
		}
		projected_revenue_array[i].last_month.total = projected_revenue_array[i].this_year.total / 12.0;
		if( i == AC_SPONSORED_RESEARCH_REVENUE ) {
		    // 11 months from September
		    projected_revenue_array[i].year_to_date.direct = projected_revenue_array[i].last_month.direct * 11;
		    projected_revenue_array[i].year_to_date.indirect = projected_revenue_array[i].last_month.indirect * 11;
		}
		projected_revenue_array[i].year_to_date.total = projected_revenue_array[i].last_month.total * 11;
		if( i == AC_SPONSORED_RESEARCH_REVENUE ) {
		    budget_revenue_array[i].direct = projected_revenue_array[i].this_year.direct;
		    budget_revenue_array[i].indirect = projected_revenue_array[i].this_year.indirect;
		}
		budget_revenue_array[i].total = projected_revenue_array[i].this_year.total;
	    }
	    for( int i = 0; i < EXPENSE_ITEM_COUNT; ++i ) {
		projected_expense_array[i].last_month.faculty = projected_expense_array[i].this_year.faculty / 12.0;
		projected_expense_array[i].last_month.staff = projected_expense_array[i].this_year.staff / 12.0;
		projected_expense_array[i].last_month.other = projected_expense_array[i].this_year.other / 12.0;
		projected_expense_array[i].last_month.total = projected_expense_array[i].this_year.total / 12.0;
		projected_expense_array[i].year_to_date.faculty = projected_expense_array[i].last_month.faculty * 11;
		projected_expense_array[i].year_to_date.staff = projected_expense_array[i].last_month.staff * 11;
		projected_expense_array[i].year_to_date.other = projected_expense_array[i].last_month.other * 11;
		projected_expense_array[i].year_to_date.total = projected_expense_array[i].last_month.total * 11;
		budget_expense_array[i].faculty = projected_expense_array[i].this_year.faculty;
		budget_expense_array[i].staff = projected_expense_array[i].this_year.staff;
		budget_expense_array[i].other = projected_expense_array[i].this_year.other;
		budget_expense_array[i].total = projected_expense_array[i].this_year.total;
	    }

	    // recalc to debug
	    totalRevenue = 0.0f;
	    for( int i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
		if( i != AC_NET_TUITION_INCOME )
		    totalRevenue += projected_revenue_array[i].this_year.total;
	    }

	    totalRevenue = 0.0f;

	    budget_calc_total();

#ifdef DEBUG
	    DEBUG_LOG( "Modify projected and budget on 1 Aug of pre-run year" );
	    for( int i = 0; i < REVENUE_ITEM_COUNT; ++i ) {
		DEBUG_LOG( budget_revenue_array[i].total );
	    }
	    for( int i = 0; i < EXPENSE_ITEM_COUNT; ++i ) {
		DEBUG_LOG( budget_expense_array[i].total );
	    }
	    DEBUG_LOG( "End modify projected and budget" );
#endif

	}

	update_history(UPDATE_MONTH);
    }
    else {                                          //--------- if it's the start of a new financial year --------//
	// assume update_projected_arrays_pre_optimization() called.
	update_projected_arrays_post_optimization();

	if( !info.prerun_year )
	    update_hiring_policy();
	// deinit the dynamic array in each department
	for ( int i=department_array.size(); i>0; i-- ) {
	    if ( department_array.is_deleted(i) )
		continue;

	    Department* deptPtr = department_array[i];

	    // renew the display_faculty_array in this department
	    int arraySize = deptPtr->cur_faculty_array.size();

	    // save the data of faculty into history
	    deptPtr->departures_history[THIS_YEAR] = deptPtr->faculty_departures_number();
	    deptPtr->hires_history[THIS_YEAR]    = deptPtr->faculty_new_hires_number();
	    deptPtr->promotion_history[THIS_YEAR]  = deptPtr->faculty_promotions_number();
	    
	    shift_history( deptPtr->departures_history, HISTORY_YEAR_COUNT );
	    shift_history( deptPtr->hires_history, HISTORY_YEAR_COUNT );
	    shift_history( deptPtr->promotion_history, HISTORY_YEAR_COUNT );

	    deptPtr->display_faculty_array.zap();       // clear display_faculty_array

	    if ( arraySize != 0 ) {
		for ( int j=1; j<=arraySize; j++ ) {
		    Faculty* facOrg = (Faculty*) deptPtr->cur_faculty_array.get(j);
		    deptPtr->display_faculty_array.linkin( facOrg );
		}
		deptPtr->cur_faculty_array.zap();
	    }
	}

	update_history(UPDATE_ALL);

    }

    // ----- Begin ----- //
    // ----- 5. Bankruptcy Warning ----- //

    // set up variables for calculations
    double projectedYearEndSyrplusOrDeficit;
    int monthsToBankruptcy;
    const double maxStDebtPct = 10;                 // 99%
    const double maxStExpPct = 50;                  // 99%
    float shortTermDebtCap;
    double currentLoanValue;
    int monthsToYearEnd;

    // set projectedYearEndSyrplysOrDeficit
    projectedYearEndSyrplusOrDeficit = finance.this_year.surplus_deficit;

    // set shortTermDebtCap
    shortTermDebtCap = (float) max(maxStDebtPct * get_available_funds()/ 100,
				   maxStExpPct * finance.projected_total_operating_expense.this_year.total / 100);

    // set currentLoanValue
    if ( (finance.this_year.asset_array[0] + finance.this_year.asset_array[1]) < 0 )
	currentLoanValue = abs(finance.this_year.asset_array[0] + finance.this_year.asset_array[1]);
    else
	currentLoanValue = 0;

    // set monthsToYearEnd
    monthsToYearEnd = finance.fiscal_year_start_month - info.game_month;
    if ( monthsToYearEnd <= 0 )
	monthsToYearEnd += 12;

    // set monthsToBankruptcy
    if ( projectedYearEndSyrplusOrDeficit < 0 )
	monthsToBankruptcy = abs(( shortTermDebtCap - currentLoanValue)/(projectedYearEndSyrplusOrDeficit/12));
    else
	monthsToBankruptcy = 1000000;

    // Check the conditions for different warning message,
    // and add the message to the news.
    if ( monthsToBankruptcy <= monthsToYearEnd ) {
	last_warn_bankrupt_date = info.game_date;
	// add message
	news_array.bankruptcy_message_lv1();
    }
    else
	if (( monthsToBankruptcy <= monthsToYearEnd+12) && ( (info.game_date - last_warn_bankrupt_date) > 90 )) {
	    last_warn_bankrupt_date = info.game_date;
	    // add message
	    news_array.bankruptcy_message_lv2( monthsToBankruptcy );
	}
	else
	    if (( monthsToBankruptcy <= monthsToYearEnd+24) && ( (info.game_date - last_warn_bankrupt_date) > 180 )) {
		last_warn_bankrupt_date = info.game_date;
		// add message
		news_array.bankruptcy_message_lv3( monthsToBankruptcy );
	    }

    // ----- End ----- //

}

//------------- End of function Finance::next_day --------//

//----------- Begin of function Finance::update_hiring_policy --------//
//!
void Finance::update_hiring_policy() {
    float totalCost = 0;                            // not $000
    int  totalFacCount = 0;
    Department* deptPtr;

    for (int i=department_array.size(); i>0; i--) {
	if ( department_array.is_deleted(i) )
	    continue;

	int facCount=0;
	float total2FacultySalary = 0;                // not $000

	deptPtr = department_array[i];

	for ( int f=deptPtr->faculty_array.size(), totalFacultySalary=0; f>0; f-- ) {
	    if ( deptPtr->faculty_array.is_deleted(f) )
		continue;

	    totalFacCount++;

	    Faculty* facPtr = deptPtr->faculty_array[f]; {

		//0407  if ( deptPtr->faculty_array[f]->rank_age_group() == FULL_PROF+1 )		// fullProf2
		total2FacultySalary += facPtr->salary;
		facCount++;
	    }

	    totalCost += facPtr->salary;
	}

	// avg_salary: $000
	finance.hiring_policy_array[i-1].avg_salary = math.safe_divide(total2FacultySalary, facCount * 1000.0f);

	finance.hiring_policy_array[i-1].fac_count = deptPtr->faculty_array.faculty_count;
    }
}

//------------- End of function Finance::update_hiring_policy --------//

//----------- Begin of function Finance::calc_total_faculty_salary --------//
//!
double Finance::calc_total_faculty_salary() {
    int d, f, facCount=0;
    short deptCount = department_array.size();      //, facCount;
    Department *dept;
    total_faculty_salary = 0;

    for ( d=1; d<=deptCount; d++ ) {
	if ( department_array.is_deleted(d) )
	    continue;

	dept = department_array[d];

	//facCount = dept->faculty_array.size();
	for ( f=dept->faculty_array.size(); f>0; f-- ) {
	    if ( dept->faculty_array.is_deleted(f) )
		continue;

	    facCount++;

	    total_faculty_salary += dept->faculty_array[f]->salary;
	}
    }

    // facCount !=0 &&
    err_when( total_faculty_salary <=0 );

    total_faculty_salary /= 1000;

    //----- update the financial data --------//

    projected_expense_array[AC_ACADEMIC_DEPARTMENTS].this_year.faculty
	= total_faculty_salary;

    projected_expense_array[AC_ACADEMIC_DEPARTMENTS].this_year.total =
	projected_expense_array[AC_ACADEMIC_DEPARTMENTS].this_year.faculty +
	projected_expense_array[AC_ACADEMIC_DEPARTMENTS].this_year.staff   +
	projected_expense_array[AC_ACADEMIC_DEPARTMENTS].this_year.other;

    projected_expense_array[AC_ACADEMIC_DEPARTMENTS].last_month.faculty
	= total_faculty_salary/12.0f;

    projected_expense_array[AC_ACADEMIC_DEPARTMENTS].last_month.total =
	projected_expense_array[AC_ACADEMIC_DEPARTMENTS].last_month.faculty +
	projected_expense_array[AC_ACADEMIC_DEPARTMENTS].last_month.staff   +
	projected_expense_array[AC_ACADEMIC_DEPARTMENTS].last_month.other;

    calc_projected_total_n_percent(1);              // 1-recalc

    //---------- return the value now ---------//

    return total_faculty_salary;
}

//----------- Begin of function Finance::calc_total_faculty_salary --------//

//----------- Begin of function Finance::update_external_factors_yearly --------//
//!
void Finance::update_external_factors_yearly() {
    // yearly update some factors for update_projected_arrays_monthly()
    // assume called in pre_optimization

    //990528 calc_total_faculty_salary();

    //
    calc_interest(UPDATE_YEAR);
}

//----------- End of function Finance::update_external_factors_yearly --------//

//----------- Begin of function Finance::update_external_factors_monthly --------//
//!
//! ----- some exogenous factors are changed monthly -----//
//!
void Finance::update_external_factors_monthly() {
    //-----------//
    inflation_rate_hist.shift_hist();

    static float growth=0, growthLast2=0;

    float old = growth;
    growth = math.time_variation(old, growthLast2, 1.8254f, -0.8659f, 0.000084545f);
    growthLast2 = old;

    inflation_rate_hist.set_cur(inflation_rate_hist.get_cur() + growth);
    inflation_rate = inflation_rate_hist.get_sum(); // set annual inflation rate

    inflation_rate_hist.set_cur(0);                 //0222
    inflation_rate = 0;                             //0222

    err_when(inflation_rate_hist.get_cur() != 0 );

    //-----------//
    calc_interest(UPDATE_MONTH);

    //-----------//
    PeerSchool *player = school_res.player_peer_school;
    int   totalFacResearchExpense = 0;              // monthly
    short   d;
    // double	last;

    for ( d=department_array.size(); d>0; d-- ) {
	if ( department_array.is_deleted(d) )
	    continue;
	// no divided by 1000 yet
	totalFacResearchExpense += department_array[d]->total_research_dollar;

    }

    // * 12 / 1000;
    projected_total_research_expense_this_year = totalFacResearchExpense * 3 / 250;

    //## chea 130899 try to lower the SPONSORED_RESEARCH change by lower the last_month.total

    //	projected_total_research_expense_this_year = (int)(projected_total_research_expense_this_year * 0.8);

    /*	990519
	last = budget_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].total
	= projected_total_research_expense_this_year;
	budget_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct
	= last / ( 1 + get_research_overhead_rate() / 100.0);
	budget_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].indirect
	= last - budget_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].direct;
    */

    //-----------//
    // academic support ratio = deptSupp$/(facSal+dirSponsRes); for pi calc of faculty
    float facExp, nonfacExp;
    nonfacExp = float(finance.projected_expense_array[AC_ACADEMIC_DEPARTMENTS].this_year.staff
		      + finance.projected_expense_array[AC_ACADEMIC_DEPARTMENTS].this_year.other);
    facExp = float(finance.total_faculty_salary + finance.projected_total_research_expense_this_year);
    academic_support_ratio = facExp>0? nonfacExp/facExp : 0;
}

//----------- End of function Finance::update_external_factors_monthly --------//

//----------- Begin of function Finance::calc_interest --------//
//!
//! "long term in WFM notes 0909" = "base long term in WFM notes 1909" * multiplier_response_func74;
//! calc base_short_term_debt_interest
//! calc base_long_term_debt_interest
//!
//! <char> default=UPDATE_YEAR
//!
void Finance::calc_interest(char updateFlag) {
    static HistoryData randomVarST;
    static HistoryData randomVarLT;
    float  multiplier, multiplierInput;

    double opReserve = last_year.asset_array[AC_OPERATING_RESERVE];

    float temp=(float)get_available_funds();

    switch (updateFlag) {
    case UPDATE_MONTH:
	randomVarST.shift_hist();

	//BUGHER found with Bill 0406: short_term_debt_interest decreases too fast...
	randomVarST.set_cur(math.time_variation(randomVarST.get(-1), randomVarST.get(-2), 1.8234f, -0.8909f, 0.00028114f));
	base_short_term_debt_interest = max(0.1, randomVarST.get_cur() + base_short_term_debt_interest);

	//---------------------------//

	//multiplier = Max[outstanding short term debt/prior year operating expenditure, outstanding short-term debt/availabole funds]

	// Start 06Mar Trev //

	float a,b;
	a=(float)math.safe_divide( (double)facility_office.total_debt_capacity[THIS_YEAR], total_operating_expense.total);
	b=(float)math.safe_divide( (double)facility_office.total_debt_capacity[THIS_YEAR], get_available_funds());

	multiplierInput = max(a, b);

	multiplierInput = max(0.0f, multiplierInput);
	multiplierInput = min(1.0f, multiplierInput);

	// End 06Mar Trev //

	//## chea try to slove the 0/0 problem

	multiplier = math.dual_response_func(1.000f, 1.776f, 4.000f, -1.045f, -0.253f, 0.575f, 0.222f, float(multiplierInput));

	//0406
	multiplier = (opReserve>0) ? 1.0f : multiplier;

	short_term_debt_interest = max(0.1, multiplier * base_short_term_debt_interest);
	break;

	//---------------------------//
    case UPDATE_YEAR:
	randomVarLT.shift_hist();

	randomVarLT.set_cur(math.time_variation(randomVarLT.get(-1), randomVarLT.get(-2), 0.5380f, -0.7579f, 0.006099140f));
	base_long_term_debt_interest = max(0.05, randomVarLT.get_cur() + base_short_term_debt_interest
					   // 0: latest/current data
					   + inflation_rate - 12 * inflation_rate_hist.get_cur());

	//---------------------------//

	// multiplier = (longTermTebtCapacity-outstandingLongTermDebt)/longTermTebtCapacity
	multiplier = (float) calc_change(this_year.liability_array[AC_LIABILITY_TOTAL], facility_office.total_debt_capacity[THIS_YEAR]) / -100;
	multiplier = math.dual_response_func(1.000f, 1.223f, 2.0f, -0.354f, -0.354f, 0.908f, 0.256f, float(multiplier));

	long_term_debt_interest = multiplier * base_long_term_debt_interest;
	break;

	//---------------------------//
    case UPDATE_ALL:
	calc_interest(UPDATE_YEAR);
	calc_interest(UPDATE_MONTH);
	break;

    default:
	err_here();
	break;
    }
}

//----------- End of function Finance::calc_interest --------//

//---------- Begin of function Finance::update_projected_arrays_pre_optimization -----------//
//!
//! called before optimization interface is shown
//!
//!
bool Finance::update_projected_arrays_pre_optimization() {
    err_when(month_left != 1);
    month_left = 0;                                 // a little trick herew
    update_external_factors_yearly();
    update_projected_arrays_monthly();

    //----------------------//
    // CHECK TERMINATION
    //----------------------//

    if ( terminate_if_bad_finance ) {
	//-------- A. check if we should terminate the game // 1016
	double reserve  = this_year.asset_array[AC_OPERATING_RESERVE];
	double limit  = 0.2 * investment_office.smoothed_endowment_value + 0.1 * this_year.asset_array[AC_BUILDINGS];
	const double maxStDebtPct = 10;               // 99%
	const double maxStExpPct = 50;                // 99%

	float shortTermDebtCap = (float) max(maxStDebtPct * get_available_funds()/ 100,
					     maxStExpPct * finance.projected_total_operating_expense.this_year.total / 100);

	String str;
	bool toQuit = false;

	if ( reserve < 0 && reserve < -shortTermDebtCap ) {
	    str = "Bankruptcy!  ";
	    str += player_school.school_name;
	    str += " has no cash and no credit.  Please click OK.";
	    toQuit = false;

	    sys.set_speed(0);
	    sys.save_speed=0;

	    //toQuit = ask_quit(str, toQuit);

	    //BUGHERE //addi
	    sys.set_staying_view_mode(MODE_BANKRUPTCY); // go to bankruptcy and then final score screen
	}
	//## chea 130899 stop the msg. display during prerun
	else if ( reserve < 0 && reserve < -shortTermDebtCap * 0.4 && info.prerun_year == 0) {
	    str = "Warning: Bankruptcy nears for ";
	    str += player_school.school_name;
	    str += ".  Having borrowed $";
	    str += long(-reserve);
	    str += ", you are dangerously close to the short-term debt limit of ";
	    str += long(shortTermDebtCap);
	    str += ".  You must act now!";
	    toQuit = false;

	    toQuit = ask_quit(str, toQuit);
	}
	else if ( reserve < -limit ) {
	    str = "Short of cash, ";
	    str += player_school.school_name;
	    str += " must borrow from the bank to pay its bills.  The most the bank will loan is $";
	    str += long(shortTermDebtCap);
	    str += ".";
	    toQuit = false;

	    toQuit = ask_quit(str, toQuit);
	}

	if ( toQuit )
	    return false;
    }

    //---------------------------------------------------------------------//
    // move data of current year to last year *before* optimization
    // and clear values for this and next years

    // 1027 special case
    //other_operating_income_last2 = revenue_array[AC_OTHER_OPERATING_INCOME].total;
    //state_appropriation_last2 = revenue_array[AC_STATE_APPROPRIATION].total;

    //## chea 250899 should init the last two item into the projected_expense_array[i].this_year instead of expense_array[i]
    if(info.prerun_year ==1) {
	memcpy( &(projected_expense_array[AC_SERVICE_ON_GENERAL_PLANT_DEBT].this_year),&(expense_array[AC_SERVICE_ON_GENERAL_PLANT_DEBT]), sizeof(ExpenseItem));
	memcpy( &(projected_expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].this_year),&(expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE]), sizeof(ExpenseItem));
    }

    int i;

    for ( i=0; i<REVENUE_ITEM_COUNT; i++) {
	memcpy( &(revenue_array[i]), &(projected_revenue_array[i].this_year), sizeof(RevenueItem));
    }

    for ( i=0; i<EXPENSE_ITEM_COUNT; i++) {
	memcpy( &(expense_array[i]), &(projected_expense_array[i].this_year), sizeof(ExpenseItem));
    }

    memcpy( &last_year, &this_year, sizeof(YearReport));
    memcpy( &total_revenue, &projected_total_revenue.this_year, sizeof(RevenueItem));
    memcpy( &total_operating_expense, &projected_total_operating_expense.this_year, sizeof(ExpenseItem));
    memcpy( &total_expense, &projected_total_expense.this_year, sizeof(ExpenseItem));

    return true;
}

//----------- End of function Finance::update_projected_arrays_pre_optimization ------------//

//----------- Begin of function Finance::update_projected_arrays_post_optimization --------//
//!
//! update projected_revenue_array *after* optimization;
//! refer to email req13d_if_finance.txt and req13_if_finance.txt
//!
void Finance::update_projected_arrays_post_optimization() {
    //budget_player_report();		//BUGHERE0 double-check the call order!

    //
    investment_office.calc_smoothed_endowment_value();

    //
    // update this and next year report
    //
    err_when(month_left != -1);                     // good check here

    int i;

    month_left = 12;
    //990520 memset(projected_revenue_array, 0, sizeof(projected_revenue_array));
    //990520 memset(projected_expense_array, 0, sizeof(projected_expense_array));

    if(info.game_day==1 && info.prerun_year==0)
	int breajef=0;

    float ratioMonth = (12-month_left) / 12.0f;

    for( i=0 ; i<REVENUE_ITEM_COUNT; i++ ) {
	projected_revenue_array[i].year_to_date.direct = ratioMonth * revenue_array[i].direct;
	projected_revenue_array[i].year_to_date.indirect = ratioMonth * revenue_array[i].indirect;
	projected_revenue_array[i].year_to_date.total = ratioMonth * revenue_array[i].total;

	projected_revenue_array[i].last_month.direct = budget_revenue_array[i].direct / 12.0;
	projected_revenue_array[i].last_month.indirect = budget_revenue_array[i].indirect / 12.0;
	projected_revenue_array[i].last_month.total = budget_revenue_array[i].total / 12.0;
    }

    for( i=0 ; i<EXPENSE_ITEM_COUNT; i++ ) {
	//---- sponsored research is handled differently ----//

	if( i==AC_SPONSORED_RESEARCH_EXPENSE ) {
	    double totalFacResearchExpense = projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].this_year.direct / 12.0;

	    double totalAYFacultySalaryOffsets = totalFacResearchExpense * 0.1;

	    double lastMonthFaculty = totalAYFacultySalaryOffsets;
	    double lastMonthOther = totalAYFacultySalaryOffsets * 0.05;

	    double lastMonthStaff
		= projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].this_year.direct / 12.0
		- totalAYFacultySalaryOffsets
		- lastMonthOther;

	    projected_expense_array[i].last_month.faculty = lastMonthFaculty;
	    projected_expense_array[i].last_month.staff = lastMonthStaff;
	    projected_expense_array[i].last_month.other = lastMonthOther;
	    projected_expense_array[i].last_month.total = lastMonthFaculty + lastMonthStaff + lastMonthOther;
	}
	else {
	    projected_expense_array[i].last_month.faculty = budget_expense_array[i].faculty / 12.0;
	    projected_expense_array[i].last_month.staff = budget_expense_array[i].staff / 12.0;
	    projected_expense_array[i].last_month.other = budget_expense_array[i].other / 12.0;
	    projected_expense_array[i].last_month.total = budget_expense_array[i].total / 12.0;
	}

	projected_expense_array[i].year_to_date.faculty = ratioMonth * expense_array[i].faculty;
	projected_expense_array[i].year_to_date.staff = ratioMonth * expense_array[i].staff;
	projected_expense_array[i].year_to_date.other = ratioMonth * expense_array[i].other;
	projected_expense_array[i].year_to_date.total = ratioMonth * expense_array[i].total;
    }

    finance.this_year.asset_array[AC_CAPITAL_RESERVE]
	// 0222
	+= projected_expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].this_year.total;

    //	update_projected_arrays_monthly_sub();  //## chea 250899 these make the 2nd & 3rd col diff.
    update_projected_arrays_monthly_sub();          //## chea 250899 these make the 2nd & 3rd col diff.

    optimization_input_reset();

    //-----------------------------------//

    department_array.faculty_hiring();

    calc_total_faculty_salary();

    //	update_projected_arrays_monthly(); //## chea 250899 these make the 2nd & 3rd col diff.

    optimization.refresh_optimization_screen = 1;

    //----------- scenario special handling -----------//

    if( player_school.scenario_id == SCN_RAISE_SALARY ) {
	player_school.scenario_faculty_salary_increase =
	    100.0f * ( (1.0f + player_school.scenario_faculty_salary_increase / 100.0f)
		       *(1.0f + (float) expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value / 100.0f) - 1.0f);
    }

    //------- update salary ---------//

    department_array.salary_setting();
}

//----------- End of function Finance::update_projected_arrays_post_optimization --------//

//----------- Begin of function Finance::update_projected_arrays_monthly --------//
//!
//! refer to email req13d_if_finance.txt and req13_if_finance.txt
//!
void Finance::update_projected_arrays_monthly() {
    update_external_factors_monthly();
    sys.yield();

    /*

      From Email req13d_if_finance.txt:

      During the year:
      1. Projected values for the current year (1998-99) are calculated each
      month as described above.
      2. "Change" (third column) equals the projected year-end value minus the
      prior-year value.
      3. Projected following-year values (1999-00) equal the current-year
      projected value accelerated by the current inflation value plus the growth
      policy factors that came out of the current year's RA procedure (C136:c144
      of HE.ResAlloc.ex). In other words, the projected values for the following
      year represent what would happen if current policies were applied without
      change to the latest current year-end budget projections.
      4. "Change" (last column) equals the difference between the projected
      year-end numbers for 1999-00 and 1998-99.

      General Equation:
      projected_var[i] = year_to_date + (budget/12) * month_left;

    */

    int i;
    double  fTmp;
    PeerSchool *player = school_res.player_peer_school;

    //
    // -------------- Step 1. --------------
    //

    // 1a. Update last_month actual value and hence year_to_date in projected_revenue_array
    const float perMonth = 1.0f/12.0f;

    // ------- REVENUE YTD -------------------------- //

    projected_revenue_array[AC_GROSS_TUITION_INCOME].last_month.total
	= perMonth * budget_revenue_array[AC_GROSS_TUITION_INCOME].total;

    projected_revenue_array[AC_FINANCIAL_AID].last_month.total
	= perMonth * budget_revenue_array[AC_FINANCIAL_AID].total;

    projected_revenue_array[AC_NET_TUITION_INCOME].last_month.total
	= projected_revenue_array[AC_GROSS_TUITION_INCOME].last_month.total
	+ projected_revenue_array[AC_FINANCIAL_AID].last_month.total;

    projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.total
	// 990519	// budget_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].total;
	= perMonth * projected_total_research_expense_this_year;

    if ( development_office.this_month_total_gift <= 0 )
	development_office.this_month_total_gift = (float) finance.revenue_array[AC_GIFTS].total / 12;
    else
	development_office.this_month_total_gift *= float(1+finance.inflation_rate_hist.get_cur()/100+development_office.gift_growth_rate);

    projected_revenue_array[AC_GIFTS].last_month.total
	= development_office.this_month_total_gift;   // 0115;

    projected_revenue_array[AC_ENDOWMENT_SPENDING].last_month.total
	= perMonth * budget_revenue_array[AC_ENDOWMENT_SPENDING].total;

    fTmp = perMonth * budget_revenue_array[AC_INTERCOLLEGIATE_ATHLETICS].total;
    projected_revenue_array[AC_INTERCOLLEGIATE_ATHLETICS].last_month.total
	= fTmp + math.get_random_snd(0, PSCH_SD(float(0.025f * fTmp)));

    fTmp = perMonth * budget_revenue_array[AC_OTHER_OPERATING_INCOME].total;
    projected_revenue_array[AC_OTHER_OPERATING_INCOME].last_month.total
	// 0405
	= fTmp + math.get_random_snd(0, PSCH_SD(float(0.025f * fTmp)));

    double opReserve = this_year.asset_array[AC_OPERATING_RESERVE];
    projected_revenue_array[AC_INTEREST_EARNED_OR_PAID].last_month.total
	= (opReserve > 0) ? (opReserve * ( inflation_rate_hist.get_cur() * 12 + short_term_debt_interest ) / 1200.0f)
	: (opReserve * ( bank_debt_interest + short_term_debt_interest ) / 1200.0f );

    // begin trevor 8 Dec 00

    //	projected_revenue_array[AC_STATE_APPROPRIATION].last_month.total
    //		= perMonth * budget_revenue_array[AC_STATE_APPROPRIATION].total;

    // end trevor 8 Dec 00

    //---------//
    // special case
    projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.direct
	= projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.total / ( 1 + get_research_overhead_rate() / 100.0);

    projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.indirect
	= projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.total - projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.direct;

    //---------//

    for ( i=0; i<REVENUE_ITEM_COUNT; i++) {
	projected_revenue_array[i].year_to_date.direct
	    += projected_revenue_array[i].last_month.direct;
	projected_revenue_array[i].year_to_date.indirect
	    += projected_revenue_array[i].last_month.indirect;
	projected_revenue_array[i].year_to_date.total
	    += projected_revenue_array[i].last_month.total;
    }

    // ------- EXPENDITURE YTD -------------------------- //

    projected_expense_array[AC_ACADEMIC_DEPARTMENTS].last_month.faculty
	= perMonth * total_faculty_salary;

    fTmp = perMonth * budget_expense_array[AC_ACADEMIC_DEPARTMENTS].staff;
    projected_expense_array[AC_ACADEMIC_DEPARTMENTS].last_month.staff
	= fTmp + math.get_random_snd(0, PSCH_SD(float(0.005f * fTmp)));

    fTmp = perMonth * budget_expense_array[AC_ACADEMIC_DEPARTMENTS].other;
    projected_expense_array[AC_ACADEMIC_DEPARTMENTS].last_month.other
	= fTmp + math.get_random_snd(0, PSCH_SD(float(0.01f * fTmp))); {

	//	projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.direct

	// 0406 the result of this code section will be over-written or will not be used later

	double totalFacResearchExpense = projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].this_year.direct / 12.0;
	double totalAYFacultySalaryOffsets = totalFacResearchExpense * 0.1;
	projected_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].last_month.faculty
	    = totalAYFacultySalaryOffsets;

	projected_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].last_month.other = totalAYFacultySalaryOffsets * 0.05;
	projected_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].last_month.staff
	    = projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].this_year.direct / 12.0
	    - totalAYFacultySalaryOffsets
	    - projected_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].last_month.other;
    }

    for (i=AC_LIBRARIES; i<=AC_ENROLLMENT_MANAGEMENT; i++) {
	float devFactor;

	if ( i == AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE )
	    devFactor = 0.015f;
	else
	    devFactor = 0.015f;

	fTmp = perMonth * budget_expense_array[i].staff;
	projected_expense_array[i].last_month.staff
	    = fTmp + math.get_random_snd(0, PSCH_SD(float(devFactor * fTmp)));

	fTmp = perMonth * budget_expense_array[i].other;
	projected_expense_array[i].last_month.other
	    = fTmp + math.get_random_snd(0, PSCH_SD(float(devFactor * fTmp)));
    }

    projected_expense_array[AC_SERVICE_ON_GENERAL_PLANT_DEBT].last_month.total
	= this_year.liability_array[AC_GENERAL_PLANT_DEBT] * long_term_debt_interest / 1200;
    // 990419 = perMonth * budget_expense_array[AC_SERVICE_ON_GENERAL_PLANT_DEBT].total;

    projected_expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].last_month.total
	= perMonth * budget_expense_array[AC_TRANSFER_TO_CAPITAL_RESERVE].total;

    //---------//

    for ( i=0; i<EXPENSE_ITEM_COUNT; i++) {
	projected_expense_array[i].year_to_date.faculty
	    += projected_expense_array[i].last_month.faculty;
	projected_expense_array[i].year_to_date.staff
	    += projected_expense_array[i].last_month.staff;
	projected_expense_array[i].year_to_date.other
	    += projected_expense_array[i].last_month.other;
	projected_expense_array[i].year_to_date.total
	    += projected_expense_array[i].last_month.total;
    }

    sys.yield();

    //---------////---------////---------//
    //---- Step 3, 2 and 4 ----//
    update_projected_arrays_monthly_sub();

    sys.yield();
}

//------------- End of function Finance::update_projected_arrays_monthly --------//

//----------- Begin of function Finance::update_projected_arrays_monthly_sub ------//
//!
//! called by update_projected_arrays_monthly() above
//!
void Finance::update_projected_arrays_monthly_sub() {
    int i;
    double temp;

    // 1b. Calc this year projected value in projected_revenue_array

    //float ratioMonthLeft = month_left / 12.0f;

    if(info.game_day ==1 && info.game_month ==9) {
	int fhdh=0;
    }

    if(info.game_day==1)                            // && info.prerun_year==0)
	int breajef=0;

    if(info.game_day==1 && info.game_month == 8) {
	last_month_SR_D =0;
	last_month_SR_ID =0;

	if(info.game_year==1) {
	    last_month_SR_D = projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.direct * 1.1f;
	    last_month_SR_ID = projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.indirect* 1.1f;
	}
	else {
	    last_month_SR_D = projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.direct;
	    last_month_SR_ID = projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.indirect;
	}
    }

    //--------------- Calculate Revenue ---------------------//

    for ( i=0; i<REVENUE_ITEM_COUNT; i++) {
	if ( i==AC_GROSS_TUITION_INCOME ||
	     i==AC_GIFTS ) {                               // Use the amounts directly. //
	    continue;
	}

	//---- for Financial Aid, only update it once a year ----//

	if( i==AC_FINANCIAL_AID && info.game_month != 9 ) {
	    continue;
	}

	//-------------------------------------------------//

	if ( i != AC_SPONSORED_RESEARCH_REVENUE ) {
	    if ( info.prerun_year || i != AC_STATE_APPROPRIATION ) {
		projected_revenue_array[i].this_year.total
		    = projected_revenue_array[i].year_to_date.total
		    + month_left * projected_revenue_array[i].last_month.total;
	    }
	}
	else {                                        //--- for AC_SPONSORED_RESEARCH_REVENUE ----//

	    //## chea 021199 don't do in 1/9
	    // > 1722030 so that it does not run in the first year of the initialization (initialization has 2 years)
	    if(info.game_day ==1 && info.game_month ==9 && info.game_date > 1722030 ) {
		//only [3] will do here anyway
		projected_revenue_array[3].last_month.direct = last_month_SR_D;
		projected_revenue_array[3].year_to_date.indirect = last_month_SR_ID;

		projected_revenue_array[i].this_year.direct
		    = projected_revenue_array[i].year_to_date.direct
		    + month_left * projected_revenue_array[i].last_month.direct;

		projected_revenue_array[i].this_year.indirect
		    = projected_revenue_array[i].year_to_date.indirect
		    + month_left * projected_revenue_array[i].last_month.indirect;

		projected_revenue_array[i].this_year.total
		    = projected_revenue_array[i].this_year.direct + projected_revenue_array[i].this_year.indirect;

		/* can't change budget after optimization otherwise the balance will be off.
		   //---- special case handling: set sponsored research budget to its projected actual ----//

		   budget_revenue_array[i].direct = projected_revenue_array[i].this_year.direct;
		   budget_revenue_array[i].indirect = projected_revenue_array[i].this_year.indirect;
		   budget_revenue_array[i].total = projected_revenue_array[i].this_year.total;
		*/
	    }                                           
	    else {
		//special case AC_SPONSORED_RESEARCH_REVENUE
		projected_revenue_array[i].this_year.direct
		    = projected_revenue_array[i].year_to_date.direct
		    + month_left * projected_revenue_array[i].last_month.direct;

		projected_revenue_array[i].this_year.indirect
		    = projected_revenue_array[i].year_to_date.indirect
		    + month_left * projected_revenue_array[i].last_month.indirect;
		projected_revenue_array[i].this_year.total
		    = projected_revenue_array[i].this_year.direct + projected_revenue_array[i].this_year.indirect;
	    }

	}

    }

    // 0118: special case

    if ( month_left == 12 ) {                       // 0405
	projected_revenue_array[AC_GROSS_TUITION_INCOME].this_year.total = tuition_rate * enroll_res.total_student_count / 1000;

    }

    projected_revenue_array[AC_NET_TUITION_INCOME].this_year.total =
	projected_revenue_array[AC_FINANCIAL_AID].this_year.total + projected_revenue_array[AC_GROSS_TUITION_INCOME].this_year.total;

    //------------------ Calculate Expenses ---------------------------//

    //------------------ Calculate Expenses: phase 1 ---------------------------//

    for ( i=0; i<EXPENSE_ITEM_COUNT; i++) {
	projected_expense_array[i].this_year.faculty
	    = projected_expense_array[i].year_to_date.faculty
	    + month_left * projected_expense_array[i].last_month.faculty;
	projected_expense_array[i].this_year.staff
	    = projected_expense_array[i].year_to_date.staff
	    + month_left * projected_expense_array[i].last_month.staff;
	projected_expense_array[i].this_year.other
	    = projected_expense_array[i].year_to_date.other
	    + month_left * projected_expense_array[i].last_month.other;

	if ( i== AC_SPONSORED_RESEARCH_EXPENSE ) {    //1228 by WFM notes 1222
	    double total = projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].this_year.direct;

	    projected_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].this_year.faculty
		= total * research_expense_fac_ratio;

	    projected_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].this_year.staff
		= total * research_expense_staff_ratio;

	    projected_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].this_year.other
		= total * ( 1-research_expense_fac_ratio-research_expense_staff_ratio);
	}

	//----- calculate the total of one row ------//

	projected_expense_array[i].this_year.total = projected_expense_array[i].this_year.faculty +
	    projected_expense_array[i].this_year.staff + projected_expense_array[i].this_year.other;

	//---- special case handling: set sponsored research expense budget to its projected actual ----//

	/* can't change budget after optimization otherwise the balance will be off.

	   if( i== AC_SPONSORED_RESEARCH_EXPENSE && info.game_day ==1 && info.game_month ==9 && info.game_date > 1722030 )		// > 1722030 so that it does not run in the first year of the initialization (initialization has 2 years)
	   {
	   budget_expense_array[i].faculty = projected_expense_array[i].this_year.faculty;
	   budget_expense_array[i].staff  = projected_expense_array[i].this_year.staff;
	   budget_expense_array[i].other  = projected_expense_array[i].this_year.other;
	   budget_expense_array[i].total = projected_expense_array[i].this_year.total;
	   }
	*/
    }   

    //0301
    i=AC_TRANSFER_TO_CAPITAL_RESERVE;
    if ( projected_expense_array[i].this_year.total < 0 ) {
	projected_expense_array[i].this_year.total = 0;
	projected_expense_array[i].this_year.other = 0;
    }

    // -------------- Step 3. -------------- projected next year
    //
    // this step is switchable with Step 2.

    // 3b. update projected *next_year* value from budget year 980902

    for ( i=AC_NET_TUITION_INCOME; i<REVENUE_ITEM_COUNT; i++) {
	if ( i != AC_SPONSORED_RESEARCH_REVENUE ) {
	    projected_revenue_array[i].next_year.total
		= projected_revenue_array[i].this_year.total// budget_revenue_array[i].total
		* (1 + inflation_rate / 100 + projected_revenue_array[i].change_budget_year.total / 100);
	}
	else {
	    //special case AC_SPONSORED_RESEARCH_REVENUE
	    projected_revenue_array[i].next_year.direct
		// budget_revenue_array[i].direct
		= projected_revenue_array[i].this_year.direct
		* (1 + inflation_rate / 100 + projected_revenue_array[i].change_budget_year.direct / 100);
	    projected_revenue_array[i].next_year.indirect
		// budget_revenue_array[i].indirect
		= projected_revenue_array[i].this_year.indirect
		* (1 + inflation_rate / 100 + projected_revenue_array[i].change_budget_year.indirect / 100);
	    projected_revenue_array[i].next_year.total
		= projected_revenue_array[i].next_year.direct + projected_revenue_array[i].next_year.indirect;
	}
    }

    //special case
    //0222

    for ( i=AC_GROSS_TUITION_INCOME; i<=AC_NET_TUITION_INCOME; i++) {
	projected_revenue_array[i].next_year.total
	    = (1 + inflation_rate ) * projected_revenue_array[i].this_year.total;
    }

    //special case  ## chea 170899
    projected_revenue_array[AC_NET_TUITION_INCOME].next_year.total
	= projected_revenue_array[AC_GROSS_TUITION_INCOME].next_year.total
	+ projected_revenue_array[AC_FINANCIAL_AID].next_year.total;

    //## chea 241199 make the 3rd row the same 12.2.2 d)
    finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total = finance.budget_revenue_array[AC_ENDOWMENT_SPENDING].total;
    finance.projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.percent = finance.budget_revenue_array[AC_ENDOWMENT_SPENDING].percent;

    //------------------ Calculate Expenses: phase 2 ---------------------------//

    for ( i=0; i<EXPENSE_ITEM_COUNT; i++) {
	projected_expense_array[i].next_year.faculty
	    = projected_expense_array[i].this_year.faculty// budget_expense_array[i].faculty
	    * (1 + inflation_rate / 100 + projected_expense_array[i].change_budget_year.faculty / 100);
	projected_expense_array[i].next_year.staff
	    = projected_expense_array[i].this_year.staff// budget_expense_array[i].staff
	    * (1 + inflation_rate / 100 + projected_expense_array[i].change_budget_year.staff / 100);
	projected_expense_array[i].next_year.other
	    = projected_expense_array[i].this_year.other// budget_expense_array[i].other
	    * (1 + inflation_rate / 100 + projected_expense_array[i].change_budget_year.other/ 100);

	if ( i== AC_SPONSORED_RESEARCH_EXPENSE ) {    //1228
	    // 0119

	    double totalAYFacultySalaryOffsets = projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].next_year.direct;
	    projected_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].next_year.faculty
		= research_expense_fac_ratio * totalAYFacultySalaryOffsets;
	    projected_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].next_year.staff
		= research_expense_staff_ratio * totalAYFacultySalaryOffsets;
	    projected_expense_array[AC_SPONSORED_RESEARCH_EXPENSE].next_year.other
		= totalAYFacultySalaryOffsets * ( 1-research_expense_fac_ratio-research_expense_staff_ratio);
	}

	//----- calculate the total of one row ------//

	projected_expense_array[i].next_year.total = projected_expense_array[i].next_year.faculty +
	    projected_expense_array[i].next_year.staff + projected_expense_array[i].next_year.other;
    }

    i = AC_INTERCOLLEGIATE_ATHLETICS;
    projected_revenue_array[i].this_year.total += chance_event.football_game_bonus + chance_event.basketball_game_bonus;

    //-------------- Step 2. change_last_year: last_year VS projected this_year ----------//

    RevenueItemChange* projRev;
    ExpenseItemChange* projExp;

    //## BUGHERE
    //## chea 170899 calc_change have bug it can't calc. change with two - no.
    //   Since this function is called by other cpp. So may have bug
    //

    for ( i=0; i<REVENUE_ITEM_COUNT; i++) {
	//----- calculate change ------//

	temp = revenue_array[i].direct;
	projRev = &projected_revenue_array[i];
	projRev->change_last_year.direct
	    = calc_change( projRev->this_year.direct, temp );

	temp = revenue_array[i].indirect;
	projRev = &projected_revenue_array[i];
	projRev->change_last_year.indirect
	    = calc_change( projRev->this_year.indirect, temp );

	temp = revenue_array[i].total;
	projRev = &projected_revenue_array[i];

	projRev->change_last_year.total = calc_change( projRev->this_year.total, temp );
    }

    for ( i=0; i<EXPENSE_ITEM_COUNT; i++) {
	//----- calculate change ------//

	temp = expense_array[i].faculty;
	projExp = &projected_expense_array[i];
	projExp->change_last_year.faculty
	    = calc_change( projExp->this_year.faculty, temp );

	temp = expense_array[i].staff;
	projExp = &projected_expense_array[i];
	projExp->change_last_year.staff
	    = calc_change( projExp->this_year.staff, temp );

	temp = expense_array[i].other;
	projExp = &projected_expense_array[i];
	projExp->change_last_year.other
	    = calc_change( projExp->this_year.other, temp );

	temp = expense_array[i].total;
	projExp = &projected_expense_array[i];
	projExp->change_last_year.total
	    = calc_change( projExp->this_year.total, temp );
    }

    //
    // -------------- Step 4. -------------- change_next_year
    //

    for ( i=0; i<REVENUE_ITEM_COUNT; i++) {
	projRev = &projected_revenue_array[i];
	temp = projRev->this_year.direct;
	projRev->change_next_year.direct
	    = calc_change( projRev->next_year.direct, temp );

	projRev = &projected_revenue_array[i];
	temp = projRev->this_year.indirect;
	projRev->change_next_year.indirect
	    = calc_change( projRev->next_year.indirect, temp );

	projRev = &projected_revenue_array[i];
	temp = projRev->this_year.total;
	projRev->change_next_year.total
	    = calc_change( projRev->next_year.total, temp );

    }

    for ( i=0; i<EXPENSE_ITEM_COUNT; i++) {
	projExp = &projected_expense_array[i];
	temp = projExp->this_year.faculty;
	projExp->change_next_year.faculty
	    = calc_change( projExp->next_year.faculty, temp );

	projExp = &projected_expense_array[i];
	temp = projExp->this_year.staff;
	projExp->change_next_year.staff
	    = calc_change( projExp->next_year.staff, temp );

	projExp = &projected_expense_array[i];
	temp = projExp->this_year.other;
	projExp->change_next_year.other
	    = calc_change( projExp->next_year.other, temp );

	projExp = &projected_expense_array[i];
	temp = projExp->this_year.total;
	projExp->change_next_year.total
	    = calc_change( projExp->next_year.total, temp );
    }

    //---- Last Step for update monthly ----//
    calc_projected_total_n_percent();
    sys.yield();

    //------ update budget change percent and individual budget item's percent of budget total as in special case handling, sponsored research budgets were set to projected sponsored research revenue and expenses

    budget_calc_total();                            // re-calculate budget total
    budget_calc_percent();
    budget_calc_change();
}

//------------- End of function Finance::update_projected_arrays_monthly_sub ------//

//---------- Begin of function Finance::calc_projected_total_n_percent -----------//
//!
//! [int] reCalc - if this is 1, then it is a re-calculation as oppsite to the first calculation.
//!						(default:0)
//!
void Finance::calc_projected_total_n_percent(int reCalc) {
    //-----------------//
    //--- total		----//
    //-----------------//

    //------- calculate the total revenue ---------//

    memset( &projected_total_revenue, 0, sizeof(projected_total_revenue) );

    for( int i=AC_NET_TUITION_INCOME ; i<REVENUE_ITEM_COUNT ; i++ ) {
	//------ calculate totals of all rows -------//

	projected_total_revenue.this_year.direct   += projected_revenue_array[i].this_year.direct;
	projected_total_revenue.this_year.indirect += projected_revenue_array[i].this_year.indirect;
	projected_total_revenue.this_year.total    += projected_revenue_array[i].this_year.total;

	projected_total_revenue.next_year.direct   += projected_revenue_array[i].next_year.direct;
	projected_total_revenue.next_year.indirect += projected_revenue_array[i].next_year.indirect;
	projected_total_revenue.next_year.total    += projected_revenue_array[i].next_year.total; {
	}

	// BUG FIX if divided by zero
	projected_total_revenue.change_next_year.direct
	    = math.safe_divide((projected_total_revenue.next_year.direct - projected_total_revenue.this_year.direct )*100, projected_total_revenue.this_year.direct );
	projected_total_revenue.change_next_year.total
	    = math.safe_divide((projected_total_revenue.next_year.indirect - projected_total_revenue.this_year.indirect )*100, projected_total_revenue.this_year.indirect );
	projected_total_revenue.change_next_year.total
	    = math.safe_divide((projected_total_revenue.next_year.total - projected_total_revenue.this_year.total )*100, projected_total_revenue.this_year.total );

	projected_total_revenue.change_last_year.direct
	    = math.safe_divide((projected_total_revenue.this_year.direct - total_revenue.direct )*100, total_revenue.direct);
	projected_total_revenue.change_last_year.indirect
	    = math.safe_divide((projected_total_revenue.this_year.indirect - total_revenue.indirect )*100, total_revenue.indirect);
	projected_total_revenue.change_last_year.total
	    = math.safe_divide((projected_total_revenue.this_year.total - total_revenue.total )*100, total_revenue.total);

	projected_total_revenue.change_budget_year.direct
	    = calc_change(budget_total_revenue.direct, total_revenue.direct);
	projected_total_revenue.change_budget_year.indirect
	    = calc_change(budget_total_revenue.indirect, total_revenue.indirect);
	projected_total_revenue.change_budget_year.total
	    = calc_change(budget_total_revenue.total, total_revenue.total);
    }

    //------- calculate the total expense ---------//

    memset( &projected_total_expense, 0, sizeof(projected_total_expense) );

    for( int i=0 ; i<OPERATING_EXPENSE_ITEM_COUNT ; i++ ) {
	//------ calculate totals of all rows -------//

	projected_total_expense.this_year.faculty += projected_expense_array[i].this_year.faculty;
	projected_total_expense.this_year.staff   += projected_expense_array[i].this_year.staff;
	projected_total_expense.this_year.other   += projected_expense_array[i].this_year.other;
	projected_total_expense.this_year.total   += projected_expense_array[i].this_year.total;

	projected_total_expense.next_year.faculty += projected_expense_array[i].next_year.faculty;
	projected_total_expense.next_year.staff   += projected_expense_array[i].next_year.staff;
	projected_total_expense.next_year.other   += projected_expense_array[i].next_year.other;
	projected_total_expense.next_year.total   += projected_expense_array[i].next_year.total; {
	}

	// update struct projected_total_expense.change_xxxx_year
	// BUG FIX if divided by zero
	projected_total_expense.change_next_year.faculty
	    = math.safe_divide((projected_total_expense.next_year.faculty - projected_total_expense.this_year.faculty )*100, projected_total_expense.this_year.faculty);
	projected_total_expense.change_next_year.staff
	    = math.safe_divide((projected_total_expense.next_year.staff - projected_total_expense.this_year.staff )*100, projected_total_expense.this_year.staff);
	projected_total_expense.change_next_year.other
	    = math.safe_divide((projected_total_expense.next_year.other - projected_total_expense.this_year.other )*100, projected_total_expense.this_year.other);
	projected_total_expense.change_next_year.total
	    = math.safe_divide((projected_total_expense.next_year.total - projected_total_expense.this_year.total )*100, projected_total_expense.this_year.total);

	projected_total_expense.change_last_year.faculty
	    = math.safe_divide((projected_total_expense.this_year.faculty - total_operating_expense.faculty )*100, total_operating_expense.faculty);
	projected_total_expense.change_last_year.staff
	    = math.safe_divide((projected_total_expense.this_year.staff - total_operating_expense.staff )*100, total_operating_expense.staff);
	projected_total_expense.change_last_year.other
	    = math.safe_divide((projected_total_expense.this_year.other - total_operating_expense.other )*100, total_operating_expense.other);
	projected_total_expense.change_last_year.total
	    = math.safe_divide((projected_total_expense.this_year.total - total_operating_expense.total )*100, total_operating_expense.total);

	projected_total_expense.change_budget_year.faculty
	    = calc_change(budget_total_operating_expense.faculty, total_operating_expense.faculty);
	projected_total_expense.change_budget_year.staff
	    = calc_change(budget_total_operating_expense.staff, total_operating_expense.staff);
	projected_total_expense.change_budget_year.other
	    = calc_change(budget_total_operating_expense.other, total_operating_expense.other);
	projected_total_expense.change_budget_year.total
	    = calc_change(budget_total_operating_expense.total, total_operating_expense.total);
    }

    //------- calculate the total operating expense ---------//
    memcpy( &projected_total_operating_expense, &projected_total_expense, sizeof(projected_total_expense) );

    for( int i=OPERATING_EXPENSE_ITEM_COUNT ; i<TOTAL_EXPENSE_ITEM_COUNT ; i++ ) {
#define MONEY_MAX 99999999999.0

	err_when(projected_expense_array[10].this_year.total > MONEY_MAX);
	err_when(projected_expense_array[10].next_year.other > MONEY_MAX);
	err_when(projected_expense_array[10].next_year.total > MONEY_MAX);

	err_when(projected_expense_array[10].change_last_year.total > MONEY_MAX);
	err_when(projected_expense_array[10].change_next_year.other > MONEY_MAX);
	err_when(projected_expense_array[10].change_next_year.total > MONEY_MAX);

	//------ calculate totals of all rows -------//

	projected_total_expense.this_year.faculty += projected_expense_array[i].this_year.faculty;
	projected_total_expense.this_year.staff   += projected_expense_array[i].this_year.staff;
	projected_total_expense.this_year.other   += projected_expense_array[i].this_year.other;
	projected_total_expense.this_year.total   += projected_expense_array[i].this_year.total;

	projected_total_expense.next_year.faculty += projected_expense_array[i].next_year.faculty;
	projected_total_expense.next_year.staff   += projected_expense_array[i].next_year.staff;
	projected_total_expense.next_year.other   += projected_expense_array[i].next_year.other;
	projected_total_expense.next_year.total   += projected_expense_array[i].next_year.total; {
	}

	// update struct projected_total_expense.change_xxxx_year
	projected_total_expense.change_next_year.faculty
	    = math.safe_divide((projected_total_expense.next_year.faculty - projected_total_expense.this_year.faculty )*100, projected_total_expense.this_year.faculty);
	projected_total_expense.change_next_year.staff
	    = math.safe_divide((projected_total_expense.next_year.staff - projected_total_expense.this_year.staff )*100, projected_total_expense.this_year.staff);
	projected_total_expense.change_next_year.other
	    = math.safe_divide((projected_total_expense.next_year.other - projected_total_expense.this_year.other )*100, projected_total_expense.this_year.other);
	projected_total_expense.change_next_year.total
	    = math.safe_divide((projected_total_expense.next_year.total - projected_total_expense.this_year.total )*100, projected_total_expense.this_year.total);

	projected_total_expense.change_last_year.faculty
	    = math.safe_divide((projected_total_expense.this_year.faculty - total_expense.faculty )*100, total_expense.faculty);
	projected_total_expense.change_last_year.staff
	    = math.safe_divide((projected_total_expense.this_year.staff - total_expense.staff )*100, total_expense.staff);
	projected_total_expense.change_last_year.other
	    = math.safe_divide((projected_total_expense.this_year.other - total_expense.other )*100, total_expense.other);
	projected_total_expense.change_last_year.total
	    = math.safe_divide((projected_total_expense.this_year.total - total_expense.total )*100, total_expense.total);

	projected_total_expense.change_budget_year.faculty
	    = calc_change(budget_total_expense.faculty, total_expense.faculty);
	projected_total_expense.change_budget_year.staff
	    = calc_change(budget_total_expense.staff, total_expense.staff);
	projected_total_expense.change_budget_year.other
	    = calc_change(budget_total_expense.other, total_expense.other);
	projected_total_expense.change_budget_year.total
	    = calc_change(budget_total_expense.total, total_expense.total);
    }

    sys.yield();

    //-----------------//
    //--- percent	----//
    //-----------------//

    //------- calculate the total revenue ---------//

    for( int i=AC_NET_TUITION_INCOME ; i<REVENUE_ITEM_COUNT ; i++ ) {
	// BUG FIXED if divided by zero
	projected_revenue_array[i].this_year.percent = 100.0f * math.safe_divide(projected_revenue_array[i].this_year.total,
										 projected_total_revenue.this_year.total);
	projected_revenue_array[i].next_year.percent = 100.0f * math.safe_divide(projected_revenue_array[i].next_year.total,
										 projected_total_revenue.next_year.total);
    }

    //------- calculate the total expense ---------//
    projected_total_operating_expense.this_year.percent = 0;
    projected_total_operating_expense.next_year.percent = 0;

    for( int i=0 ; i<EXPENSE_ITEM_COUNT ; i++ ) {
	projected_expense_array[i].this_year.percent = 100.0f * math.safe_divide(projected_expense_array[i].this_year.total,
										 projected_total_expense.this_year.total);
	projected_expense_array[i].next_year.percent = 100.0f * math.safe_divide(projected_expense_array[i].next_year.total,
										 projected_total_expense.next_year.total);

	if ( i < OPERATING_EXPENSE_ITEM_COUNT ) {
	    projected_total_operating_expense.this_year.percent += projected_expense_array[i].this_year.percent;
	    projected_total_operating_expense.next_year.percent += projected_expense_array[i].next_year.percent;
	}
    }

    projected_total_revenue.this_year.percent = 100;
    projected_total_expense.this_year.percent = 100;
    projected_total_revenue.next_year.percent = 100;
    projected_total_expense.next_year.percent = 100;

    sys.yield();

    //----- balance sheet -----//

    this_year.surplus_deficit = projected_total_revenue.this_year.total - projected_total_expense.this_year.total;

    double cons2 = 1.0 + inflation_rate/100.0 + investment_office.expected_real_annual_return_rate/100.0;
    double cons1 = 1.0 + inflation_rate/100.0 + short_term_debt_interest/100.0;

    // 1022
    double bank_deposit_rate_per_month = finance.inflation_rate + .005;

    if ( !reCalc )
	this_year.asset_array[AC_OPERATING_RESERVE]   //=D50*(1+C72+C73)+F263
	    = this_year.asset_array[AC_OPERATING_RESERVE]*(1+bank_deposit_rate_per_month)
	    + chance_event.operating_reserve_modifier;

    //-------- Chance Event --------//
    /*
      if( !reCalc )
      {
      if(chance_event.scandalous_fund_use > 0)
      {
      this_year.asset_array[AC_OPERATING_RESERVE] -= chance_event.scandalous_fund_use;
      chance_event.scandalous_fund_use = 0;
      }
      }
    */
    //------ recalc Endowment ----------//

    double bank_interest_rate = finance.inflation_rate + .01;

    this_year.asset_array[AC_ENDOWMENT] =
	this_year.asset_array[AC_ENDOWMENT]*(1+bank_interest_rate)-
	(float)(revenue_array[AC_ENDOWMENT_SPENDING].total*(1-endowment_quasi_ratio)/12);

    this_year.asset_array[AC_QUASI] =
	this_year.asset_array[AC_QUASI]*(1+bank_interest_rate)-
	(float)(revenue_array[AC_ENDOWMENT_SPENDING].total*endowment_quasi_ratio/12);;

    /*
      this_year.asset_array[AC_ENDOWMENT]	//=D51*(1+C72+G72)-F241		// = D51-F241 by fred 1023
      = last_year.asset_array[AC_ENDOWMENT]
      + investment_office.endowment_change_year_to_date	// 990527
      + development_office.this_year.endowment;				// 990608

      //--- scenario special handling ------//

      if( player_school.scenario_id == SCN_INVEST_ACADEMIC &&
      info.prerun_year==0 && !(info.game_year >= 2 && info.game_month >= 9) )		// only do this for the first game year, in the second year, last_year.asset_array[] will be updated correctly already.
      {
      this_year.asset_array[AC_ENDOWMENT]	+= player_school.scenario_endowment_increase;
      }

      if ( this_year.asset_array[AC_QUASI] != 0 && info.prerun_year != 1 )
      this_year.asset_array[AC_QUASI] = (last_year.asset_array[AC_QUASI]+transfer_quasi) * this_year.asset_array[AC_ENDOWMENT] / last_year.asset_array[AC_ENDOWMENT];
      else
      this_year.asset_array[AC_QUASI] = last_year.asset_array[AC_QUASI] * this_year.asset_array[AC_ENDOWMENT] / last_year.asset_array[AC_ENDOWMENT];

      float ratio = this_year.asset_array[AC_QUASI] / (this_year.asset_array[AC_QUASI]+this_year.asset_array[AC_ENDOWMENT]);

      this_year.asset_array[AC_ENDOWMENT] = last_year.asset_array[AC_ENDOWMENT] + (investment_office.endowment_change_year_to_date + development_office.this_year.endowment
      - projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total)*(1-ratio);

      this_year.asset_array[AC_QUASI] = last_year.asset_array[AC_QUASI] + (investment_office.endowment_change_year_to_date + development_office.this_year.endowment
      - projected_revenue_array[AC_ENDOWMENT_SPENDING].this_year.total)*ratio;
    */

    //------- calculate asset totals --------//

    calc_asset_total();

    //------------- next_year ---------------//

    memcpy(&next_year, &this_year, sizeof(YearReport));
    next_year.surplus_deficit = projected_total_revenue.next_year.total - projected_total_expense.next_year.total;
}

//----------- End of function Finance::calc_projected_total_n_percent ------------//

//---------- Begin of function Finance::calc_asset_total -----------//
//!
void Finance::calc_asset_total() {
    // calc asset_total
    finance.this_year.asset_array[AC_ASSET_TOTAL] = 0 ;

    for( int i=0 ; i<ASSET_ITEM_COUNT-1 ; i++ ) {
	//err_when(finance.this_year.asset_array[i] < 0);
	finance.this_year.asset_array[AC_ASSET_TOTAL] += finance.this_year.asset_array[i];
    }

    finance.this_year.liability_array[AC_LIABILITY_TOTAL] = 0;

    for( int i=0 ; i<LIABILITY_ITEM_COUNT-1 ; i++ ) {
	finance.this_year.liability_array[AC_LIABILITY_TOTAL] += finance.this_year.liability_array[i];
    }
}

//----------- End of function Finance::calc_asset_total ------------//

//----------- Begin of function Finance::get_policy_var ------------//
//!
FinancePolicy* Finance::get_policy_var(bool isRevenue, char index) {
    FinancePolicy* policy;
    if ( isRevenue ) {
	// translate from AC_ to PL_
	if ( index == AC_GROSS_TUITION_INCOME )
	    index = PL_TUITION_RATE_GROWTH;
	else if (index == AC_ENDOWMENT_SPENDING )
	    index = PL_ENDOWMENT_SPENDING_RATE;
	else
	    err_here();

	policy =  &(revenue_policy_array[index]);

    }
    //BUGHER?, should(?) handle more case in expense_policy_array?
    else if ( index == AC_TRANSFER_TO_CAPITAL_RESERVE ) {
	index = PL_REAL_TRANSFER_TO_PLANT_GROWTH;
	policy = &(expense_policy_array[index]);
    }
    else {
	// translate from AC_ to PL_
	err_when(PL_LIBRARIES != AC_LIBRARIES);
	err_when(index < 0 || index > PL_ENROLLMENT_MANAGEMENT);

	policy = &(cost_rise_policy_array[index]);
    }

    return policy;
}

//---------- End of function Finance::get_policy_var -----------//

//---------- Begin of function Finance::change_budget -----------//
//!
//! called by interface directly
//! example call: change_budget(CONSIDER_NEXT_YEAR, false, AC_ATHLETICS, 10.0f)
//!
//! <changeType>		: type of change = CONSIDER_NEXT_YEAR, ANNOUNCE_NEXT_YEAR, IMPLEMENT_NOW
//! <isRevenue>			: is revenue item or expense
//! <index>				: index for budget_*_array but not *_policy_array
//! <percentChange>	: range from [-100,100]
//! [char] showMsg		: whether display message or not (default:1)
//!
void Finance::change_budget(char changeType, bool isRevenue, char index, float percentChange, char showMsg) {
    // designed for three types of financial decisions:
    // Implemented, Announced and Proposed

    err_when( percentChange > 100 && percentChange < -100 );
    err_when( index < 0 );
    String str1,str2;

    FinancePolicy *policy;
    double target,lower,upper, lastTarget;

    policy = get_policy_var(isRevenue, index);

    //--- if percentChange is 0, do nothing but shows the current target value ---//

    int mappedChangeType;

    if( changeType==CONSIDER_NEXT_YEAR )            // map change type to P_??? scale.
	mappedChangeType = P_CONSIDER;
    else if( changeType==ANNOUNCE_NEXT_YEAR )
	mappedChangeType = P_PROMISE;
    else
	mappedChangeType = P_IMPLEMENT;

    // and no change in policy type
    if( percentChange==0 && policy->applied_flag == mappedChangeType ) {
	if( policy->applied_flag == P_CONSIDER )
	    str1="Current target value is ";
	else
	    str1="The target value is fixed at ";

	str1+=m.format(policy->target_value,23);
	str1+=".";

	str1+="\r\nIt should be lie between ";
	str1+=m.format((float)policy->lower_limit,23);
	str1+=" and ";
	str1+=m.format((float)policy->upper_limit,23);
	str1+=".";

	if( policy->applied_flag == P_CONSIDER ) {
	    str1+="\r\n'Consider' is selected or preference '3' has been pressed \r\n in Finance Budget Plan report.";
	}
	else if( policy->applied_flag == P_PROMISE ) {
	    str1+="\r\n'Promise' is selected or preference 'R' has been pressed \r\n in Finance Budget Plan report.";
	}
	else if( policy->applied_flag == P_IMPLEMENT ) {
	    str1+="\r\n'Implement now' is selected or preference 'R' has been pressed \r\n in Finance Budget Plan report.";
	    str1+="\r\nThe operation is implemented with this target value now.";
	}

	if( showMsg )
	    box.msg(str1);
	return;
    }

    //--- the user is not allowed to change the action type or value once the user has applied Promise or Implement ---//

    if( policy->applied_flag == P_PROMISE ) {
	if( showMsg )
	    box.msg( "Sorry, you have already made a promise for this policy and cannot withdraw it." );
	return;
    }

    if( policy->applied_flag == P_IMPLEMENT ) {
	if( showMsg )
	    box.msg( "Sorry, you have already implemented this policy and cannot change it." );
	return;
    }

    //----- save values for restore next year ------//

    //----- if first time consider or promise this year, save last target value ------//

    if ( policy->applied_flag == P_NONE ) {         // if it has set it before, keep last_target_value and do not change it.
	policy->last_target_value = policy->target_value;
	policy->last_upper_bound = policy->upper_bound;
	policy->last_lower_bound = policy->lower_bound;
	policy->last_import_weight = policy->import_weight;
	policy->last_required_flag = policy->required_flag;
    }

    //-- But the user can change from Consider to Promise or Implement Now ---//

    switch(changeType) {
    case CONSIDER_NEXT_YEAR:
    case ANNOUNCE_NEXT_YEAR:

	lastTarget = policy->target_value;
	target = policy->target_value + percentChange;

	//--- adjust the lower & upper bounds if the target exceed the current bounds ---//

	lower = max(policy->lower_limit, min(target, policy->lower_bound));
	upper = min(policy->upper_limit, max(target, policy->upper_bound));

	policy->target_value = min( max(target, lower), upper);
	policy->lower_bound = lower;
	policy->upper_bound = upper;
	policy->import_weight = OPT_HIGH_WEIGHT;

	str1="Target value changes from ";
	str1+=m.format((float)lastTarget,23);
	str1+=" to ";
	str1+=m.format((float)policy->target_value,23);
	str1+=".";

	str1+="\r\nIt should be lie between ";
	str1+=m.format((float)policy->lower_limit,23);
	str1+=" and ";
	str1+=m.format((float)policy->upper_limit,23);
	str1+=".";

	if( changeType==CONSIDER_NEXT_YEAR ) {
	    str1+="\r\n'Consider' is selected or preference '3' has been pressed \r\n in Finance Budget Plan report.";
	}
	else {
	    str1+="\r\n'Promise' is selected or preference 'R' has been pressed \r\n in Finance Budget Plan report.";
	}

	if( showMsg )
	    box.msg(str1);

	//---------- penalty: check if promise or commitment applied before -------------//
	// add small un-implemented penalty for CONSIDER and big penalty for ANNOUNCE    //

	if ( policy->applied_flag > P_NONE && get_sign(policy->target_value - lastTarget) == policy->penalty_direction ) {
	    double value = pow( double(policy->target_value - lastTarget)/(policy->upper_limit - policy->lower_limit),2);

	    value = math.dual_response_func(0, 0.451f, 1.000f, -0.358f, -0.361f, 0.819f, 0.213f, float(value));

	    policy->penalty_multiplier1 = 1 - policy->max_penalty1 * value;
	    policy->penalty_multiplier2 = 1 - policy->max_penalty2 * value;

	    // adjustment

	    if ( changeType == CONSIDER_NEXT_YEAR ) {
		policy->penalty_multiplier1 *= 0.333333f;
		policy->penalty_multiplier2 *= 0.333333f;
	    }

	    err_when(value < 0 || value > 1);
	}

	//--------- set the effect now ---------//

	if ( changeType == CONSIDER_NEXT_YEAR )
	    policy->applied_flag = P_CONSIDER;
	else {
	    policy->required_flag = true;
	    policy->applied_flag = P_PROMISE;
	}

	break;

	//-------- Click Implement Now button ----------//

    case IMPLEMENT_NOW:

	//----- if the user has set Consider previously, restore target_value before calling set_constraint_policy() so that last_target_value will not be overwritten twice

	lastTarget = policy->target_value;          // the last target regardless of what the one before the previous value is

	//---- trancate out-of-limit values  -----------//

	policy->target_value += percentChange;
	policy->target_value = min( max(policy->target_value, policy->lower_limit), policy->upper_limit);

	policy->set_constraint_policy(0);           // don't save last as we already did in this function

	//---------- display message --------------//

	str1="Target value changes from ";
	str1+=m.format((float)lastTarget,23);
	str1+=" to ";
	str1+=m.format((float)policy->target_value,23);
	str1+=" .";

	str1+="\r\nIt should be lie between ";
	str1+=m.format((float)policy->lower_limit,23);
	str1+=" and ";
	str1+=m.format((float)policy->upper_limit,23);
	str1+=".";

	str1+="\r\n'Implement now' is selected or preference 'R' has been pressed \r\n in Finance Budget Plan report.";
	str1+="\r\nThe operation is implemented with this target value now.";

	if( showMsg )
	    box.msg(str1);

	//------- apply the effect now -------//

	policy->required_flag = true;
	policy->applied_flag = P_IMPLEMENT;

	policy->import_weight = OPT_HIGH_WEIGHT;

	err_when(policy->target_value < policy->lower_bound);
	break;

	//------------------//

    default:
	err_here();
	break;
    }

    optimization.refresh_optimization_screen = 1;
}

//----------- End of function Finance::change_budget ------------//

//## chea begin 240699 for(WFM comments;3/19/99 15.3)
//---------- Begin of function Finance::update_history -----------//
//!
//! update history arrays
//!
//! update_flag		= UPDATE_TRIMESTER, UPDATE_MONTH, UPDATE_YEAR; (ooffice.h)
//!
void Finance::update_history(char update_flag) {

    double operating_sub_surplus = last_year.asset_array[AC_OPERATING_RESERVE] + finance.this_year.surplus_deficit;
    double sponsored_research =                     //projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].last_month.total + projected_revenue_array[AC_SPONSORED_RESEARCH_EXPENSE].last_month.total;
	//finance.revenue_array[AC_SPONSORED_RESEARCH_REVENUE].total;
	projected_revenue_array[AC_SPONSORED_RESEARCH_REVENUE].this_year.total;

    double net_tuition = projected_revenue_array[AC_NET_TUITION_INCOME].this_year.total;

    double bank_deposit_rate_per_month ;

    float test1 = 0.00f;
    float test2 = 0.00f;
    float result = 0.00f;
    int i;

    switch (update_flag) {
    case UPDATE_MONTH:
	shift_history(surplus_deficit_history, HISTORY_MONTH_COUNT);
	surplus_deficit_history[THIS_MONTH] = finance.this_year.surplus_deficit;
	shift_history(operating_sub_surplus_history, HISTORY_MONTH_COUNT);
	operating_sub_surplus_history[THIS_MONTH] = operating_sub_surplus;
	shift_history(sponsored_research_history, HISTORY_MONTH_COUNT);
	sponsored_research_history[THIS_MONTH] = sponsored_research;
	shift_history(net_tuition_history, HISTORY_MONTH_COUNT);
	net_tuition_history[THIS_MONTH] = net_tuition;
	shift_history(short_term_debt_interest_history, HISTORY_MONTH_COUNT);
	short_term_debt_interest_history[THIS_MONTH] = short_term_debt_interest;

	// 1022
	bank_deposit_rate_per_month = (finance.inflation_rate + .005);
	this_year.asset_array[AC_CAPITAL_RESERVE] += bank_deposit_rate_per_month * this_year.asset_array[AC_CAPITAL_RESERVE];

	// copy value from this_year to month_start
	for( i = 0; i < REVENUE_ITEM_COUNT; ++i )
	    projected_revenue_array[i].month_start = projected_revenue_array[i].this_year;
	for( i = 0; i < EXPENSE_ITEM_COUNT; ++i )
	    projected_expense_array[i].month_start = projected_expense_array[i].this_year;

	projected_net_distance_learning_expense.month_start = projected_net_distance_learning_expense.this_year;
	projected_total_revenue.month_start = projected_total_revenue.this_year;
	projected_total_operating_expense.month_start = projected_total_operating_expense.this_year;
	projected_total_expense.month_start = projected_total_expense.this_year;

	break;

    case UPDATE_TRIMESTER:

	break;

    case UPDATE_YEAR:
	test1 = (float) enroll_res.total_matrics[UG_TRADITION];
	test2 = (float) enroll_res.total_applications[UG_TRADITION];
	result =  test1/test2;

	shift_history(sl1_sel_history, HISTORY_YEAR_COUNT);
	sl1_sel_history[THIS_YEAR] = result;
	shift_history(faculty_salary_increase_history, HISTORY_YEAR_COUNT);
	faculty_salary_increase_history[THIS_YEAR] = expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value;

	shift_history(tuition_increase_history, HISTORY_YEAR_COUNT);
	tuition_increase_history[THIS_YEAR] = revenue_policy_array[PL_TUITION_RATE_GROWTH].result_value;

	shift_history( long_term_debt_interest_history, HISTORY_YEAR_COUNT );
	long_term_debt_interest_history[THIS_YEAR] = long_term_debt_interest;

	this_year.surplus_deficit = projected_total_revenue.this_year.total - projected_total_expense.this_year.total;

	this_year.asset_array[AC_OPERATING_RESERVE] += this_year.surplus_deficit;

	this_year.asset_array[AC_ENDOWMENT] += development_office.this_year.endowment;
	break;

    case UPDATE_ALL:
	update_history(UPDATE_MONTH);
	update_history(UPDATE_TRIMESTER);
	update_history(UPDATE_YEAR);
	break;

    default:
	err_here();
	break;

    }
}

//---------- End of function Finance::update_history -----------//
//## chea begin 240699
