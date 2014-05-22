//Filename    : OPSCH_PI.CPP
//Description : School class
//Owner       : Fred

#include <All.h>
#include <OSYS.H>
#include <OMATH.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OPEERSCH.H>
#include <OSCHLRES.H>
#include <OCHANCE.H>
#include <OFINANCE.H>
#include <OATHLETI.H>
#include <ODEVELOP.H>
#include <OFACILIT.H>
#include <OENROLL.H>
#include <OINVEST.H>
#include <OCHANCE.H>

#include <OPSCHOOL.H>
#include <OTASK.H>                                //## chea 200899

static float default_sub_score_weight[SCORE_VAR_COUNT] = {
    0.120f,0.080f,
    0.070f,0.070f,0.070f,0.035f,0.035f,0.070f,
    0.075f,0.045f,0.030f,
    0.060f,0.090f,0.075f,0.075f,
};

//------ Begin of function sort_peer_school_score_function ------//
//!
//! Sort School in db_school_array by their distances in an ascending order.
//!
static int sort_peer_school_score_function( const void *a, const void *b ) {
    float valueA = school_res.peer_school_array[(*((short*)a))].game_score;
    float valueB = school_res.peer_school_array[(*((short*)b))].game_score;

    float rc = valueA - valueB;

    if( rc > 0 )
	return -1;
    else if( rc < 0 )
	return 1;
    else
	return 0;
}

inline char get_group_index(int i) {
    int grp;
    if ( i >= GROUP2_BEGIN_ID && i < GROUP3_BEGIN_ID )
	grp=1;
    else if ( i >= GROUP3_BEGIN_ID && i < GROUP4_BEGIN_ID )
	grp=2;
    else if ( i >= GROUP4_BEGIN_ID )
	grp=3;
    else
	grp=0;

    return grp;
}

//------- End of function sort_peer_school_score_function ------//

//---------- Begin of function PlayerSchool::init_pi -----------//
//!
//! intermediate vars for calc of performance indicators.
//!
void PlayerSchool::init_pi() {
    //-------------------------//
    // refer to section 7.1 of WFM notes 0919

    PeerSchool* sch = school_res.player_peer_school;
    float degreeGrantedInit[MAX_STUDENT_LEVEL];

    degreeGrantedInit[UG_TRADITION] = sch->total_bach_degrees;
    degreeGrantedInit[UG_NONTRADITION] = sch->total_bach_degrees / 2.0f;
    degreeGrantedInit[MASTER] = sch->total_masters_and_prof_degree;
    degreeGrantedInit[DOCTOR] = sch->total_doctoral_degrees;
    degreeGrantedInit[DISTANCE_LEARN] = degreeGrantedInit[UG_NONTRADITION]; {

	//990507

	float totalDegree=0;
	for ( int i=0; i<MAX_STUDENT_LEVEL; i++ )
	    totalDegree += degreeGrantedInit[i];

	float idealDegreeSL1 = enroll_res.student_count[UG_TRADITION] / 4.0f;
	float ratio = idealDegreeSL1 * totalDegree / degreeGrantedInit[UG_TRADITION];

	for ( int i=0; i<MAX_STUDENT_LEVEL; i++ )
	    degreeGrantedInit[i] *= ratio / totalDegree;
    }

    err_when(degreeGrantedInit[UG_TRADITION] <= 0);

    degree_granted_weight[UG_TRADITION] = (float) degreeGrantedInit[UG_TRADITION];

    float adjust[3] = { 2,4,15 };
    for (char sl=UG_NONTRADITION; sl<=DOCTOR; sl++)
	degree_granted_weight[sl] = ( degreeGrantedInit[sl] > 0 ) ? (float) degreeGrantedInit[sl] : float(degreeGrantedInit[UG_TRADITION]) / adjust[sl-UG_NONTRADITION];

    //990507 degree_granted_weight[UG_TRADITION] = (float) degreeGrantedInit[UG_TRADITION];
    degree_granted_weight[DISTANCE_LEARN] = (float) degreeGrantedInit[UG_NONTRADITION];

    for (int sl=0, degree_granted_ratio_init=0 ; sl<MAX_STUDENT_LEVEL; sl++) {
	degree_granted_weight[sl] = 1.0f / degree_granted_weight[sl];
	degree_granted_ratio_init += degree_granted_weight[sl] * degreeGrantedInit[sl];
    }

    err_when(degree_granted_ratio_init == 0);

    //-------------------------//
    init_research_dollar = -1;
    //grad_count = 0;
    //matrics_count = 0;

    //-------------------------//
    memset(score_group_weight, 0, sizeof(score_group_weight));

    for (int i=0; i<SCORE_VAR_COUNT; i++) {
	// sub_score_weight already initialized in game setting

	// calc group info for if
	//
	char grp = get_group_index(i);

	score_group_weight[grp] += sub_score_weight[i];
    }
}

//---------- End of function PlayerSchool::init_pi -----------//

//---------- Begin of function PlayerSchool::init_sub_score_weight -----------//
//!
//! subScoreGroup between 0 and SCORE_SUBGROUP_COUNT
//! or -1 means update all sub_score_weight
//!
void PlayerSchool::init_sub_score_weight(int subScoreGroup) {
    int i;
    if( subScoreGroup < 0 ) {
	for( i = 0; i < SCORE_VAR_COUNT; ++i )
	    sub_score_weight[i] = default_sub_score_weight[i] * 100;
    }
    else if( subScoreGroup == 0 ) {
	for( i = GROUP1_BEGIN_ID; i < GROUP2_BEGIN_ID; ++i )
	    sub_score_weight[i] = default_sub_score_weight[i] * 100;
    }
    else if( subScoreGroup == 1 ) {
	for( i = GROUP2_BEGIN_ID; i < GROUP3_BEGIN_ID; ++i )
	    sub_score_weight[i] = default_sub_score_weight[i] * 100;
    }
    else if( subScoreGroup == 2 ) {
	for( i = GROUP3_BEGIN_ID; i < GROUP4_BEGIN_ID; ++i )
	    sub_score_weight[i] = default_sub_score_weight[i] * 100;
    }
    else if( subScoreGroup == 3 ) {
	for( i = GROUP4_BEGIN_ID; i < SCORE_VAR_COUNT; ++i )
	    sub_score_weight[i] = default_sub_score_weight[i] * 100;
    }
    else {
	err_here();
    }
}

//---------- End of function PlayerSchool::init_sub_score_weight -----------//

//---------- Begin of function PlayerSchool::normalize_sub_score_weight -----------//
//!
//! make sub of sub_score_weight to 100%
//!
void PlayerSchool::normalize_sub_score_weight() {
    float sumWeight = 0.0f;
    for( int i = 0; i < SCORE_VAR_COUNT; ++i )
	sumWeight += sub_score_weight[i];

    if( sumWeight <= 0.0f ) {
	init_sub_score_weight(-1);                    // avoid divided by zero, restore default
	return;
    }

    for( int i = 0; i < SCORE_VAR_COUNT; ++i )
	sub_score_weight[i] *= 100.0f / sumWeight;
}

//---------- End of function PlayerSchool::init_sub_score_weight -----------//

//---------- Begin of function PlayerSchool::get_default_sub_score_weight -----------//
//!
float PlayerSchool::get_default_sub_score_weight(int i) {
    return default_sub_score_weight[i] * 100;
}

//---------- End of function PlayerSchool::get_default_sub_score_weight -----------//

//---------- Begin of function PlayerSchool::update_history -----------//
//!
//! update history arrays
//!
//! update_flag		= UPDATE_TRIMESTER, UPDATE_MONTH, UPDATE_YEAR; (ooffice.h)
//!
void PlayerSchool::update_history(char update_flag) {
    switch (update_flag) {
    case UPDATE_MONTH:
	//## chea 071299 1.12.1 this is done in department_array.next_day
	calc_performance_monthly();                 // calc score

	summer_emphasis_last = summer_emphasis;     //1125
	break;
    case UPDATE_TRIMESTER:
	break;
    case UPDATE_YEAR:
	calc_performance_yearly();                  // include *_degree[HISTORY_YEAR_COUNT]//##chea 290100 try to loca the bug
	break;
    case UPDATE_ALL:
	update_history(UPDATE_YEAR);
	update_history(UPDATE_MONTH);
	update_history(UPDATE_TRIMESTER);
	break;
    default:
	err_here();
	break;
    }
}

//---------- End of function PlayerSchool::update_history -----------//

//---------- Begin of function PlayerSchool::calc_performance_yearly -----------//
void PlayerSchool::calc_performance_yearly() {

    shift_history(bachelor_degree, HISTORY_YEAR_COUNT);
    shift_history(non_ug_bachelor_degree, HISTORY_YEAR_COUNT);
    shift_history(master_degree, HISTORY_YEAR_COUNT);
    shift_history(doctor_degree, HISTORY_YEAR_COUNT);

    for (int y=0; y<DEGREE_TYPE_COUNT; y++) {
	shift_history(time_to_degree[y], HISTORY_YEAR_COUNT);
	shift_history(fake_disp_time_to_degree[y], HISTORY_YEAR_COUNT);
	shift_history(dropout_rate[y], HISTORY_YEAR_COUNT);
    }

    int i, deptCount = 0;
    Department* deptPtr;

    for ( i=department_array.size(); i>0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;

	deptPtr = department_array[i];

	bachelor_degree[THIS_YEAR] += deptPtr->student_array.bachelor_degree[THIS_YEAR];
	non_ug_bachelor_degree[THIS_YEAR] += deptPtr->student_array.non_ug_bachelor_degree[THIS_YEAR];
	master_degree[THIS_YEAR] += deptPtr->student_array.master_degree[THIS_YEAR];
	//0223
	doctor_degree[THIS_YEAR] += deptPtr->student_array.doctor_degree[THIS_YEAR];

	for (int y=0; y<DEGREE_TYPE_COUNT; y++) {
	    time_to_degree[y][THIS_YEAR] += deptPtr->student_array.time_to_degree[y][THIS_YEAR];
	    dropout_rate[y][THIS_YEAR] += deptPtr->student_array.dropout_rate[y][THIS_YEAR];
	}

	deptCount++;
    }

    //------------------------//

    for (int y=0; y<DEGREE_TYPE_COUNT; y++)
	time_to_degree[y][THIS_YEAR] /= deptCount;

    //--- special case handling: smooth the graph for bachelor Time to Degree ---//

    fake_disp_time_to_degree[BACHELOR_DEGREE][THIS_YEAR] = m.fmax( time_to_degree[BACHELOR_DEGREE][THIS_YEAR], math.get_random_snd(4.3f, 0.07f) );

    //------------------------//

    GeneralDepartment* genDept = &department_res.general_dept;

    for (int y=0; y<DEGREE_TYPE_COUNT; y++) {
	dropout_rate[y][THIS_YEAR] += genDept->student_array.dropout_rate[y][THIS_YEAR];
    }

    //990408 for (y=0; y<DEGREE_TYPE_COUNT; y++)
    //990408	dropout_rate[y][THIS_YEAR] /= deptCount + 1;
}

//---------- Begin of function PlayerSchool::calc_performance_yearly -----------//

//---------- Begin of function PlayerSchool::calc_performance_monthly -----------//
void PlayerSchool::calc_performance_monthly() {
    calc_prestige();
    //sys.yield();
    calc_staff_morale();
    calc_media_attitudes();
    calc_administrative_performance_etc();
    //sys.yield();
    calc_student_performance();
    //sys.yield();

    for (int i=0; i<SCORE_VAR_COUNT; i++)
	shift_history(sub_score[i], HISTORY_MONTH_COUNT);
    shift_history(game_score_history,HISTORY_MONTH_COUNT);
    shift_history(ultimate_game_score_history,HISTORY_MONTH_COUNT);

    calc_ultimate_score();
}

//---------- Begin of function PlayerSchool::calc_performance_monthly -----------//

//---------- Begin of function PlayerSchool::calc_prestige -----------//
void PlayerSchool::calc_prestige() {
    const char P_VAR_COUNT=4;
    int i;
    float diff;
    float input[P_VAR_COUNT];

    //## chea 201099
    diff = 0.0f;
    if(info.prerun_year == 1 && info.game_day ==1 && info.game_month==9) {
	if(player_school.prestige == 1)
	    school_res.player_peer_school->prestige = (float)(20 + m.random(4));
	else if(player_school.prestige == 3)
	    school_res.player_peer_school->prestige = (float)(30 + m.random(4));
	else
	    school_res.player_peer_school->prestige = (float)(40 + m.random(4));

	old_pre = 0.0f;
    }

    memset(input, 0, sizeof(input));

    //--------------//
    int totalFacCount = 0;
    Department* deptPtr;

    for ( i=department_array.size(); i>0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;

	deptPtr = department_array[i];

	int facCount = deptPtr->faculty_array.faculty_count;

	input[0] += deptPtr->p_academic_standing * facCount;

	totalFacCount += facCount;
    }

    err_when(totalFacCount == 0);
    input[0] /= totalFacCount;

    //--------------//
    input[1] = enroll_res.cur_yield_rate[UG_TRADITION] * 100;

    //--------------//
    input[2] = math.safe_divide(1.0f, enroll_res.offers_rate[0]);

    //--------------//
    input[3] = athletics_office.avg_percent_win;

    //--------------//
    input[1] = math.single_response_func(0.0f, 100.0f, 40.0f, 40.0f, input[1]);
    input[2] = math.single_response_func(0.0f, 100.0f, 4.0f, 3.0f, input[2]);
    input[3] = math.dual_response_func(0.0f, 49.9f,100.0f, 0.225f, 0.178f, 0.301f, 0.685f, input[3]);

    //--------------//

    float curPrestige = school_res.player_peer_school->prestige;
    float weight[P_VAR_COUNT] = { 0.5f, 0.2f, 0.2f, 0.1f };
    float newValue = 0;

    for(i=0; i<P_VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    curPrestige = player_school.latency_func( 0.91f, curPrestige, newValue);

    if(info.prerun_year == 1 && info.game_day ==1 && info.game_month==9)
	diff = 0.0f ;
    else
	diff = curPrestige - old_pre;

    old_pre = curPrestige;
    err_when(old_pre <= 0);                         //prestige can't be 0
    school_res.player_peer_school->prestige += diff;

    if(school_res.player_peer_school->prestige <= 0)
	school_res.player_peer_school->prestige = 0.0f;
    else if(school_res.player_peer_school->prestige >= 100)
	school_res.player_peer_school->prestige = 100.0f;

}

//---------- End of function PlayerSchool::calc_prestige -----------//

//---------- Begin of function PlayerSchool::calc_staff_morale -----------//
void PlayerSchool::calc_staff_morale() {
    const char M_VAR_COUNT=4;
    int i;
    float input[M_VAR_COUNT];
    memset(input, 0, sizeof(input));

    //--------------//
    input[0] = (float) finance.expense_policy_array[PL_STAFF_SALARY_INCREASES].result_value;

    //--------------//
    input[1] = (float) finance.revenue_policy_array[PL_INDIRECT_COST_RATE].result_value;

    //--------------//
    input[2] = (float) finance.expense_policy_array[PL_BUDGET_ADJUSTMENT].result_value;

    //--------------//
    input[3] = administrative_performance;

    //--------------//
    input[0] = math.dual_response_func(0,60.00f,100,0.711f,0.722f,0.355f,2.500f, input[0]);
    input[1] = math.dual_response_func(0, 60.00f, 100, 2.362f, 2.0f, -3.819f, 4.0f, input[1]);
    input[2] = math.dual_response_func(0, 60.00f, 100, 1.450f, 3.010f, -0.866f, 1.495f, input[2]);

    // delete references to input[1]
    float weight[M_VAR_COUNT] = {0.50f, 0.00f, 0.3f, 0.2f};

    float newValue = 0;
    for(i=0; i<M_VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    //-------- pass throught the latency function ---------//

    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    staff_morale = (char) player_school.latency_func( 0.88f, staff_morale, newValue);

    //---------- apply extra effect brought the chance cards ------------//

    double finalValue = staff_morale;

    if( chance_event.extra_effect_staff_morale ) {
	finalValue *= chance_event.extra_effect_staff_morale;

	chance_event.extra_effect_staff_morale += (1.0f - chance_event.extra_effect_staff_morale) * 0.3f;
    }

    //------- apply penalty due to change of promised policies ------//

    finalValue *= finance.expense_policy_array[PL_STAFF_SALARY_INCREASES].penalty_multiplier1;
    finalValue *= finance.expense_policy_array[PL_BUDGET_ADJUSTMENT].penalty_multiplier2;
    finalValue *= finance.cost_rise_policy_array[PL_ADMINISTRATION].penalty_multiplier1;

    staff_morale = (char) max(0, min(100, finalValue) );
}

//---------- End of function PlayerSchool::calc_staff_morale -----------//

//---------- Begin of function PlayerSchool::calc_media_attitudes -----------//

void PlayerSchool::calc_media_attitudes() {
    const char M_VAR_COUNT=3;
    int i;
    float input[M_VAR_COUNT];
    memset(input, 0, sizeof(input));

    //--------------//
    input[0] = float( school_res.player_peer_school->prestige );

    //--------------//
    input[1] = float(finance.this_year.surplus_deficit) / float(finance.total_operating_expense.total);
    err_when(finance.total_operating_expense.total<= 0);

    //--------------//
    input[2] = athletics_office.avg_percent_win;    //BUGHERE should *100

    //--------------//
    input[1] = math.dual_response_func(0,60,100,0.486f,0.782f,-0.931f,1.000f, input[1]);
    input[2] = math.dual_response_func(0,40,100,19.97f,30.00f,20.00f,75.00f, input[2]);

    float weight[M_VAR_COUNT] = { 0.60f, 0.20f, 0.20f, };

    float newValue = 0;
    for(i=0; i<M_VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    //-------- pass throught the latency function ---------//

    //## chea 091199 handling chance card value
    media_attitudes = (char)player_school.latency_func( 0.5f, media_attitudes, newValue);

    //---------- extra effect brought the chance cards ------------//

    if( chance_event.extra_effect_media_att ) {
	media_attitudes = char( (float)media_attitudes * chance_event.extra_effect_media_att );

	chance_event.extra_effect_media_att += (1.0f - chance_event.extra_effect_media_att) * 0.3f;

	media_attitudes = max(0, min(100,media_attitudes) );
    }
}

//---------- End of function PlayerSchool::calc_media_attitudes -----------//

//---------- Begin of function PlayerSchool::calc_administrative_performance_etc -----------//
void PlayerSchool::calc_administrative_performance_etc() {
    const char M_VAR_COUNT=3;
    int i;
    float input[M_VAR_COUNT];
    memset(input, 0, sizeof(input));

    //--------------//
    input[0] = (float) staff_morale;

    //--------------//
    //	input[1] = (float) finance.revenue_policy_array[PL_INDIRECT_COST_RATE].result_value;
    // chwg ##290699
    if(info.game_year==1)
	if(info.game_month==9)
	    initializationValueAC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE=(float)finance.expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].total;
    input[1] = float(finance.expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].total)/
	(initializationValueAC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE*(1+math.get_random_snd(0.0f, PSCH_SD(0.05f))));

    //--------------//
    //	input[2] = float(finance.projected_expense_array[AC_ADMINISTRATION_N_OTHER_OPERATING_EXPENSE].this_year.total) / float(finance.projected_total_operating_expense.this_year.total);
    // chwg ##290699
    input[2] = (float) finance.revenue_policy_array[PL_INDIRECT_COST_RATE].result_value;

    err_when(finance.budget_total_operating_expense.total <= 0);

    //--------------//
    //	input[1] = math.dual_response_func(0,60.00f,100,8.09f,23.52f,15.88f,29.96f, input[1]);
    // chwg290699
    input[1] = math.dual_response_func(0,50.00f,100,1.68f,0.28f,0.45f,1.60f, input[1]);
    input[2] = math.dual_response_func(0, 60.10f, 100, 3.546f, 2.996f, -3.226f, 3.503f, input[2]);

    // delete references to input[2]
    float weight[M_VAR_COUNT] = {0.5f, 0.5f, 0.0f};

    float newValue = 0;
    for(i=0; i<M_VAR_COUNT; i++)
	newValue += weight[i] * input[i];

    //	administrative_performance = (char) math.latency_func( 0.95f, administrative_performance, newValue);
    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    administrative_performance = (char) player_school.latency_func( 0.90f, administrative_performance, newValue);

    // req26b.txt:	//Single-Logistic; floor = 0.75; ceiling = 1; b = .998; b = 1.514
    administrative_performance = (char) math.single_response_func(0.75f, 1, .998f, 1.514f, administrative_performance); {

	//--------------//
	//scholarship_broadly_defined
	//--------------//

	const char SB_VAR_COUNT=5;
	float input[SB_VAR_COUNT];
	memset(input, 0, sizeof(input));

	//--------------//
	// Average regular faculty scholarship performance
	// Average regular faculty research performance
	// Average faculty educational development time (all faculty)
	// Average faculty public service time (all faculty)
	// Technology utilization in teaching

	input[0] = department_array.performance_scholarship[THIS_TRIMESTER];
	input[1] = department_array.performance_research[THIS_TRIMESTER];

	input[2] = (float) department_array.discretionary_hour_array[DT_EDUCATIONAL_DEVELOPMENT][THIS_TRIMESTER];
	input[3] = (float) department_array.discretionary_hour_array[DT_INSTITUTIONAL_AND_PUBLIC_SERVICE][THIS_TRIMESTER];

	//--------------//
	Department* deptPtr;
	int totalFacCount = 0;

	for ( i=department_array.size(); i>0; i-- ) {
	    if ( department_array.is_deleted(i) )
		continue;

	    deptPtr = department_array[i];

	    int facCount = deptPtr->faculty_array.faculty_count;

	    input[4] += deptPtr->p_tech_utilization * facCount;

	    totalFacCount += facCount;
	}

	err_when(!totalFacCount);
	input[4] /= totalFacCount;

	//--------------//
	input[0] = math.dual_response_func(1,50,100,28.35f,29.00f,22.31f,77.45f, input[0]);
	input[1] = math.dual_response_func(1,100,90,28.91f,20.78f,21.85f,80.03f, input[1]);
	input[2] = math.single_response_func(1,100,6.32f,7.58f, input[2]);
	input[3] = math.single_response_func(1,100,6.32f,7.58f, input[3]);
	input[4] = math.single_response_func(1,100,48.82f,40.98f, input[4]);

	float weight[SB_VAR_COUNT] = {0.15f, 0.05f, 0.10f, 0.10f, 0.15f};

	float newValue = 0;
	for(i=0; i<SB_VAR_COUNT; i++)
	    newValue += weight[i] * input[i];

	//	scholarship_broadly_defined = (char) math.latency_func( 0.93f, scholarship_broadly_defined, newValue);
	//## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
	scholarship_broadly_defined = (char) player_school.latency_func( 0.80f, scholarship_broadly_defined, newValue);

    }
}

//---------- End of function PlayerSchool::calc_administrative_performance_etc -----------//

//---------- Begin of function PlayerSchool::calc_ultimate_score -----------//
//!
//! refer to section8 of td2.5 AND [HE.RespF_Definitions.xls]TrusteeEvel
//!
void PlayerSchool::calc_ultimate_score() {
    int i, j;

    //------ Step 1: calculate the inputs -------//

    Department *deptPtr;
    float subScore[SCORE_VAR_COUNT];

    memset(subScore, 0, sizeof(subScore));

    //----------- Calculate S_DEGREES_GRANTED ----------//
    //
    // Degrees granted: Min[100, 10*Max[0, X-15]]
    // where X = 100*(SL-1_degrees/SL-1_enrollments)
    // i.e. traditional undergraduates only. (X ranges from 15 [score=0] to 25 [score=100].)
    //
    //-----------------------------------------------------//

    float x = 100.0f * (float) bachelor_degree[THIS_YEAR] / (float) enroll_res.student_count[UG_TRADITION];

    // Item 15i) -- Change formula for Score of Degree Granted in order to prevent zero value
    subScore[S_DEGREES_GRANTED] = min(100, min(x,15)+8.5*max(0,x-15));

    //---------- Calculate S_SPONSORED_RESEARCH_ACTIVITY --------------//
    //
    // use research performance from the performance screen.
    //
    //--------------------------------------------------------//

    subScore[S_SPONSORED_RESEARCH_ACTIVITY] = department_array.p_faculty_research_history[THIS_MONTH];

    //---------- Calculate S_PRESTIGE ----------------//

    subScore[S_PRESTIGE] = school_res.player_peer_school->prestige;

    //----------- Calculate S_EDUCATIONAL_QUALITY -----------//
    //
    // Educational quality: institution-level educational quality
    //	from the performance screen (already 0-100).
    //
    //--------------------------------------------------------//

    subScore[S_EDUCATIONAL_QUALITY] = department_array.p_educational_quality_history[THIS_TRIMESTER];

    //--------- Calculate S_SCHOLARSHIP_BROADLY_DEFINED ----------//
    //
    //	weighted average of the department-level scholarship performance figures
    // from the performance:detail screens (already 0-100), with the number of
    // regular faculty (i.e., excluding the two adjunct categories) as weights..
    //
    //-------------------------------------------------------------//

    float totalScholarship=0;
    int totalFaculty = 0;

    for ( i=department_array.size(); i>0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;

	deptPtr = department_array[i];

	for( j=deptPtr->faculty_array.size() ; j>0 ; j-- ) {
	    if( deptPtr->faculty_array.is_deleted(j) )
		continue;

	    totalScholarship += deptPtr->faculty_array[j]->performance_scholarship;
	    totalFaculty++;
	}
    }

    err_when (!totalFaculty);

    subScore[S_SCHOLARSHIP_BROADLY_DEFINED] = totalScholarship / totalFaculty;

    sys.yield();

    //----------- S_FACULTY_MORALE -----------//
    //
    // institution-level faculty morale from the performance screen (already 0-100).
    // Average performance ratings over faculty sims.
    //
    //----------------------------------------//

    subScore[S_FACULTY_MORALE] = department_array.p_faculty_morale_history[THIS_MONTH];

    //-------------------------------------------//

    subScore[S_STUDENT_MORALE] = department_array.p_student_morale_history[THIS_TRIMESTER];

    //------------S_STAFF_MORALE -------------//

    subScore[S_STAFF_MORALE] = staff_morale;

    //---------- S_STUDENT_DIVERSITY -------------//

    // Formula used by the U.S. Census Bureau
    // diversity = 1 - {(%NonMinMale/100)^2 + (%NonMinFemale/100)^2 + (%MinMale/100)^2 + (%MinFemale/100)^2}

    //	int nonMinFemale = enroll_res.overall_percent_female - enroll_res.overall_percent_minority_female;
    //	int minMale = enroll_res.overall_percent_minority - enroll_res.overall_percent_minority_female;
    //	int minFemale = enroll_res.overall_percent_minority_female;
    //	int nonMinMale = 100 - nonMinFemale - minMale - minFemale;
    //	subScore[S_STUDENT_DIVERSITY] = 100.0f * (1.0f -
    //		float(nonMinMale*nonMinMale + nonMinFemale*nonMinFemale + minMale*minMale + minFemale*minFemale)
    //		/ float(100*100));

    // use count instead of interger percentage for accuracy
    {
	int gender, totalCount;
	subScore[S_STUDENT_DIVERSITY] = 0.0f;

	for( gender = 0, totalCount = 0; gender < GENDER_ETHNIC_TYPE_COUNT; ++gender ) {
	    totalCount += enroll_res.overall_gender_ethnic_group_student_count[gender];
	    subScore[S_STUDENT_DIVERSITY] += (float)enroll_res.overall_gender_ethnic_group_student_count[gender]
		* (float)enroll_res.overall_gender_ethnic_group_student_count[gender];
	}

	if( totalCount > 0) {
	    subScore[S_STUDENT_DIVERSITY] = 100.0f * (1.0f - subScore[S_STUDENT_DIVERSITY]/(float)totalCount/(float)totalCount);
	}
    }

    //------------ S_FACULTY_DIVERSITY ----------//

    totalFaculty = 0;
    float totalMaleMinority=0;
    float totalFemaleMinority=0;
    float totalFemaleMajority=0;

    for ( i=department_array.size(); i>0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;

	deptPtr = department_array[i];

	for( j=deptPtr->faculty_array.size() ; j>0 ; j-- ) {
	    if( deptPtr->faculty_array.is_deleted(j) )
		continue;

	    switch( deptPtr->faculty_array[j]->gender_ethnic_group ) {
	    case MINORITY_MALE:
		totalMaleMinority++;
		break;

	    case MINORITY_FEMALE:
		totalFemaleMinority++;
		break;

	    case NONMINORITY_FEMALE:
		totalFemaleMajority++;
		break;
	    }

	    totalFaculty++;
	}
    }

    err_when (!totalFaculty);

    //--- convert to percentage ---//

    totalMaleMinority = 100.0f * (float) totalMaleMinority / (float) totalFaculty;
    totalFemaleMinority = 100.0f * (float) totalFemaleMinority / (float) totalFaculty;
    totalFemaleMajority = 100.0f * (float) totalFemaleMajority / (float) totalFaculty;

    // Formula used by the U.S. Census Bureau
    // diversity = 1 - {(%NonMinMale/100)^2 + (%NonMinFemale/100)^2 + (%MinMale/100)^2 + (%MinFemale/100)^2}

    float totalMaleMajority = 100.0f - totalMaleMinority - totalFemaleMinority - totalFemaleMajority;

    subScore[S_FACULTY_DIVERSITY] = 100.0f * (1.0f -
					      float(totalMaleMajority*totalMaleMajority + totalFemaleMajority*totalFemaleMajority + totalMaleMinority*totalMaleMinority + totalFemaleMinority*totalFemaleMinority)
					      / float(100*100));

    sys.yield();

    //-----------S_PERCENT_ALUMNI_GIVEN ----------//

    x = development_office.percent_alumni_gave_gift;
    subScore[S_PERCENT_ALUMNI_GIVEN] = min(0.62f * math.safe_pow(x,1.3f), 100.0f);

    //--------- S_CURRENT_SURPLUS_DEFICIT_PERCENT ------------------//
    //
    // Current surplus-deficit: 50 + Sign[X]*Min[50, 10*Abs[ X]^1.5] where
    //	X = 100*(Current-year surplus or deficit)/(Current-year total expenditures).
    //	(X ranges from -3 [score=0] to +3 [score=100].)
    //
    //---------------------------------------------------------------//

    x = 100.0f * (float)finance.this_year.surplus_deficit / (float)finance.projected_total_expense.this_year.total;

    subScore[S_CURRENT_SURPLUS_DEFICIT_PERCENT] = 50.0f + ((x>0) ? 1.0f : -1.0f) *
	min( 50.0f, 10.0f * math.safe_pow( (float) fabs(x), 1.5f ) );

    //--------- S_SMOOTHED_SURPLUS_DEFICIT_PERCENT ------------//
    //
    // Smoothed surplus-deficit: Current surplus-deficit run through a latency
    // function with parameter = 0.5 (initialized at the end of the pre-year).
    //
    //---------------------------------------------------------------//

    float latency= 0.5f;

    if( info.prerun_year == 1 )
	subScore[S_SMOOTHED_SURPLUS_DEFICIT_PERCENT] = subScore[S_CURRENT_SURPLUS_DEFICIT_PERCENT];
    else
	subScore[S_SMOOTHED_SURPLUS_DEFICIT_PERCENT] = math.latency_func(latency, sub_score[S_SMOOTHED_SURPLUS_DEFICIT_PERCENT][THIS_MONTH-1], subScore[S_CURRENT_SURPLUS_DEFICIT_PERCENT]);
    // don't change math.latency_func to player_school.latency_func

    //---------  S_DEFERRED_MAINTENANCE_BACKLOG_PERCENT --------//
    //
    // Deferred maintenance backlog: 100 - Min[100, 30.0*X^1.1] where
    // X = 100* (deferred maintenance backlog/replacement value of plant).
    // (X ranges from 0 [score=100] to 3 [score=0].)
    //
    //---------------------------------------------------------------//

    x = 100.0f * math.safe_divide((float) facility_office.onm_n_backlog_history[SECOND_H_THIS_YEAR],
				  (float)finance.this_year.asset_array[AC_BUILDINGS]);

    subScore[S_DEFERRED_MAINTENANCE_BACKLOG_PERCENT] = 100.0f - min(100.0f, 30.0f * math.safe_pow(x, 1.1f) );

    //--------- S_ENDOWMENT_PAYOUT_RATE_RELATIVE --------//
    //
    // Endowment payout rate: 100 - Min[100, 25.0*Abs[X -4.5]^1.2] where
    // X = endowment payout rate (in percentage terms).
    // (X ranges from about 4.5 [score=0] to 7.5 or 1.5 [score=100].)
    //
    //---------------------------------------------------------------//

    x = (float)finance.revenue_policy_array[PL_ENDOWMENT_SPENDING_RATE].result_value;

    subScore[S_ENDOWMENT_PAYOUT_RATE_RELATIVE] =
	100.0f - min( 100.0f, 25.0f * math.safe_pow( (float) fabs(x-4.5f), 1.2f) );

    //----- limit all inputs to the range from 0 to 1000 ------//

    for (i=0; i<SCORE_VAR_COUNT; i++) {
	if( subScore[i] < 0 )
	    subScore[i] = 0;

	if( subScore[i] > 100 )
	    subScore[i] = 100;
    }

    //--------------------------------------------------------------//
    //	 calc cur_game_score by adding up the inputs with weights
    //--------------------------------------------------------------//

    float rawCurrentGameScore = 0;

    for (i=0; i<SCORE_VAR_COUNT; i++)
	rawCurrentGameScore += subScore[i] * sub_score_weight[i] / 100;

    cur_game_score = rawCurrentGameScore;

    //------- calculate the utlimate score ---------//

    ultimate_game_score = int((info.year_passed - 1) + (int(cur_game_score) - int(init_game_score)) * 10);

    // 1013 for new "Objctive"  //## chea 200899
    ultimate_game_score += task_array.total_bonus_pt;
    ultimate_game_score += bonus_point;             // scenario bonus
    ultimate_game_score += (int)threshold_event_bonus;

    if ( cur_game_score <= 0 )
	cur_game_score = 0;

    sys.yield();

    // ----- for interface graph plotting -----//

    for (i=0; i<SCORE_VAR_COUNT; i++) {
	if ( subScore[i] < 0 )
	    subScore[i] = 0;
    }

    for (i=0; i<SCORE_VAR_COUNT; i++) {
	sub_score[i][THIS_MONTH] = subScore[i];
	sub_score_change[i] = subScore[i] - sub_score_start[i];
    }

    //----- save game score into historical array -----//

    game_score_history[THIS_MONTH] = cur_game_score;

    ultimate_game_score_history[THIS_MONTH] = ultimate_game_score;

    PeerSchool *playerSch = school_res.player_peer_school;
    playerSch->game_score = (float) cur_game_score;

    //----- calculate input group sub-total --------//

    for (i=0; i<SCORE_SUBGROUP_COUNT; i++)
	score_sub_total[i][THIS_MONTH] = 0;

    for (i=0; i<SCORE_VAR_COUNT; i++) {
	char grp = get_group_index(i);

	score_sub_total[grp][THIS_MONTH] += subScore[i] * sub_score_weight[i] / 100;
    }

    for (i=0; i<SCORE_SUBGROUP_COUNT; i++)
	score_group_change[i] = score_sub_total[i][THIS_MONTH] - score_sub_total[i][THIS_MONTH-1];

    //------ calc peer school score ------//

    for (i=0; i<school_res.peer_school_count; i++) {
	// min & max bug chea
	school_res.peer_school_array[i].game_score = m.fmax(5.0f, school_res.peer_school_array[i].game_score + math.get_random_snd(0,5));
    }

    // since it is overrided in the loop above
    school_res.player_peer_school->game_score = (float) cur_game_score;

    //------ sort peer_school_rank[] --------//		// peer_school_rank[] store the index of peer_school_array; index 0 refer to highest score

    for( i=0; i<school_res.peer_school_count; i++ ) {
	school_res.peer_school_rank[i] = i;
    }

    qsort( school_res.peer_school_rank, school_res.peer_school_count, sizeof(school_res.peer_school_rank[0]), sort_peer_school_score_function );
}

//---------- End of function PlayerSchool::calc_ultimate_score -----------//

//---------- Begin of function PlayerSchool::init_score -----------//

void PlayerSchool::init_score() {
    //------- update ultimate score -------//

    calc_ultimate_score();

    //------ update sub score -----------//

    for (int i=0; i<SCORE_VAR_COUNT; i++) {
	sub_score_start[i] = sub_score[i][THIS_MONTH];
	sub_score_change[i] = 0;
    }

    //------ calculate game score ------------//

    // set initial game score
    player_school.init_game_score = player_school.cur_game_score;

    player_school.game_score_history[HISTORY_MONTH_COUNT-1]=player_school.cur_game_score;
    player_school.game_score_history[HISTORY_MONTH_COUNT-2]=player_school.cur_game_score;

    if ( player_school.scenario_id > 0 )
	player_school.scenario_start_score = (int) player_school.cur_game_score;

    //------ re-calc it as init_game_score has been updated ----//

    calc_ultimate_score();
}

//---------- End of function PlayerSchool::init_score -----------//

//---------- Begin of function PlayerSchool::add_bonus -----------//

void PlayerSchool::add_bonus(int value) {
    bonus_point += value;
    calc_ultimate_score();
}

//---------- End of function PlayerSchool::add_bonus -----------//

//---------- Begin of function PlayerSchool::calc_student_performance -----------//
//!
void PlayerSchool::calc_student_performance() {
    shift_history(talent_academic, HISTORY_MONTH_COUNT);
    shift_history(talent_extracurricular, HISTORY_MONTH_COUNT);
    shift_history(talent_athletics, HISTORY_MONTH_COUNT);

    shift_history(satisfaction_academic, HISTORY_MONTH_COUNT);
    shift_history(satisfaction_student_life, HISTORY_MONTH_COUNT);
    shift_history(satisfaction_athletics, HISTORY_MONTH_COUNT);
    shift_history(satisfaction_overall, HISTORY_MONTH_COUNT);

    for (int d=0; d<DEGREE_TYPE_COUNT; d++)
	shift_history(talent_academic_all[d], HISTORY_MONTH_COUNT);

    shift_history(performance_academic_ug, HISTORY_MONTH_COUNT);

    StudentArray *stuPtr;
    const char  AVG_VAR_COUNT = 11;
    float total[AVG_VAR_COUNT];
    int i, totalStuCount = 0, totalUgtStuCount=0;
    int totalDoctorCount=0, totalMasterCount=0;

    memset(total, 0, sizeof(total));

    for( i=department_array.size() ; i>0 ; i-- ) {
	if( department_array.is_deleted(i) )
	    continue;

	Department *dept = department_array[i];
	stuPtr = &(department_array[i]->student_array);

	int stuCount = stuPtr->student_count;
	int ugtCount = dept->student_level_history[UG_TRADITION][THIS_YEAR];
	//int ugAllCount = ugtCount + dept->student_level_history[UG_NONTRADITION][THIS_YEAR] + dept->student_level_history[DISTANCE_LEARN][THIS_YEAR];
	int masCount = dept->student_level_history[MASTER][THIS_YEAR];
	int docCount = dept->student_level_history[DOCTOR][THIS_YEAR];

	//		test[i] = ugtCount;

	//		test1 = stuPtr->talent_academic[THIS_MONTH] * info.debug[i];

	//		test2++;

	total[0] += stuPtr->talent_academic[THIS_MONTH] * ugtCount;
	//		total[0] += stuPtr->talent_academic[THIS_MONTH-1] * info.debug[i];
	total[1] += stuPtr->talent_extracurricular[THIS_MONTH] * ugtCount;
	total[2] += stuPtr->talent_athletics[THIS_MONTH] * ugtCount;

	total[3] += stuPtr->satisfaction_academic[THIS_MONTH] * ugtCount;
	total[4] += stuPtr->satisfaction_student_life[THIS_MONTH] * ugtCount;
	total[5] += stuPtr->satisfaction_athletics[THIS_MONTH] * ugtCount;
	total[6] += stuPtr->satisfaction_overall[THIS_MONTH] * ugtCount;

	//ugAllCount;
	total[7] += stuPtr->performance_academic_ug[THIS_MONTH] * ugtCount;
	// ugAllCount;
	total[8] += stuPtr->talent_academic_all[BACHELOR_DEGREE][THIS_MONTH] * ugtCount;
	total[9] += stuPtr->talent_academic_all[MASTER_DEGREE][THIS_MONTH] * masCount;
	total[10] += stuPtr->talent_academic_all[DOCTOR_DEGREE][THIS_MONTH] * docCount;

	totalUgtStuCount += ugtCount;
	totalStuCount += stuCount;

	//totalUgAllCount += ugAllCount;
	totalMasterCount += masCount;
	totalDoctorCount += docCount;
    }

    //	if (!totalStuCount )
    //		return;

    if ( totalUgtStuCount ) {
	//## 071299 chea 1.12.1
	talent_academic[THIS_MONTH] = char(total[0] / totalUgtStuCount);
	//		talent_academic[THIS_MONTH] = 33; //## 200199 chea 4 testing only

	//		talent_academic[THIS_MONTH] = char(total[0] / info.debug[11]);
	talent_extracurricular [THIS_MONTH] = char(total[1] / totalUgtStuCount);
	talent_athletics [THIS_MONTH] = char(total[2] / totalUgtStuCount);

	satisfaction_academic [THIS_MONTH] = char(total[3] / totalUgtStuCount);
	satisfaction_student_life [THIS_MONTH] = char(total[4] / totalUgtStuCount);
	satisfaction_athletics [THIS_MONTH] = char(total[5] / totalUgtStuCount);
	satisfaction_overall [THIS_MONTH] = char(total[6] / totalUgtStuCount);

	//totalUgAllCount);
	talent_academic_all[BACHELOR_DEGREE][THIS_MONTH] = char(total[8] / totalUgtStuCount);

	performance_academic_ug[THIS_MONTH] = char(total[7] / totalUgtStuCount);
    }

    if ( totalMasterCount )
	talent_academic_all[MASTER_DEGREE][THIS_MONTH] = char(total[9] / totalMasterCount);

    if ( totalDoctorCount )
	talent_academic_all[DOCTOR_DEGREE][THIS_MONTH] = char(total[10] / totalDoctorCount);
}

//---------- End of function PlayerSchool::calc_student_performance -----------//

//---------- End of function PlayerSchool::next_trimester -----------//
//
// update the array adjusted_student_major_pref
//
void PlayerSchool::next_trimester() {
    short i, sl, deptCount;
    float totalPctIncludedDept = 0.0f;
    Department *deptPtr;

    deptCount = department_array.department_count;

    if ( deptCount == 0 )                           // it's true when called through init();
	return;

    err_when(!deptCount || deptCount > MAX_DEPARTMENT);

    //---------- adjust the array which was adjusted in PlayerSchool::generate_student_adjust_one_pct_array(float *arr, int size)

    for (sl=0; sl<MAX_STUDENT_LEVEL_INI; sl++) {
	generate_student_readjust_pct_array
	    (adjusted_student_major_pref[sl], department_array.department_count);
    }

    sys.yield();

    //---------- calc major multiplier and hence major ------------//

    for (sl=0; sl<MAX_STUDENT_LEVEL_INI; sl++) {
	totalPctIncludedDept = 0.0f;

	for (i=1; i<=deptCount; i++) {
	    deptPtr = department_array[i];

	    float pref = 0.5f * deptPtr->p_educational_quality + 0.5f * deptPtr->p_student_morale;

	    major_pref_multiplier[i-1] = player_school.latency_func( 0.5f, major_pref_multiplier[i-1], pref );

	    //float last1 = adjusted_student_major_pref[sl][i-1];
	    //float rand = time_variation(last1, last2, 1.8531f, -0.9151f ,difficulty_multiplier*0.003099490f)
	    float rand = math.time_variation(0, 0, 1.8531f, -0.9151f, difficulty_multiplier * 0.003099490f );

	    adjusted_student_major_pref[sl][i-1]
		*= major_pref_multiplier[i-1] * (1 + rand);// 1007: random added WFM note 0919

	    //---------- Scenario special handling ----------//

	    if( scenario_id == SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS &&
		info.prerun_year && info.game_year==1 ) {
		if( player_school.scen_dept_selected_array[i-1] )
		    adjusted_student_major_pref[sl][i-1] *= 1.67f;
		else
		    adjusted_student_major_pref[sl][i-1] *= 0.33f;
	    }

	    //----------------------------------------------------//

	    totalPctIncludedDept += adjusted_student_major_pref[sl][i-1];

	    sys.yield();
	}

	//--------- normalize the preference ----------//

	for (i=1; i<=deptCount; i++) {
	    adjusted_student_major_pref[sl][i-1] = math.safe_divide(adjusted_student_major_pref[sl][i-1] ,totalPctIncludedDept);
	}
    }

    // generate_student_adjust_pct_arrays(); for probability of selection of major
    //
    for (sl=0; sl<MAX_STUDENT_LEVEL_INI; sl++) {
	generate_student_adjust_pct_array
	    (adjusted_student_major_pref[sl], department_array.department_count);
    }

    sys.yield();

    //------------------------------------------------------------//
    //---------- 1103 calc elective course multiplier ------------//
    //------------------------------------------------------------//

    for( i=0 ; i<=department_res.department_count ; i++ ) {
	// 0 points to the general department
	DepartmentInfo* deptInfo = department_res.operator[](i);

	float total_bachelor_pref = 0;
	float total_master_pref = 0;

	for (int d=department_array.size(); d>0; d--) {
	    if( department_array.is_deleted(d) )
		continue;

	    int deptId = department_array[d]->department_id;

	    deptInfo->bachelor_elective_course_pref[deptId-1] *=
		department_array[d]->p_educational_quality;

	    deptInfo->master_elective_course_pref[deptId-1] *=
		department_array[d]->p_educational_quality;

	    total_bachelor_pref += deptInfo->bachelor_elective_course_pref[deptId-1];
	    total_master_pref += deptInfo->master_elective_course_pref[deptId-1];
	}

	total_bachelor_pref /= department_array.department_count;
	total_master_pref /= department_array.department_count;

	sys.yield();

	for (int d=department_array.size(); d>0; d--) {
	    if( department_array.is_deleted(d) )
		continue;

	    int deptId = department_array[d]->department_id;

	    deptInfo->bachelor_elective_course_pref[deptId-1] = math.safe_divide(deptInfo->bachelor_elective_course_pref[deptId-1], total_bachelor_pref);
	    deptInfo->master_elective_course_pref[deptId-1] = math.safe_divide(deptInfo->master_elective_course_pref[deptId-1], total_master_pref);
	}

	sys.yield();
    }

    department_res.calc_total_elective_course_pref();
}

//---------- End of function PlayerSchool::next_trimester -----------//

// ##### Begin Marco ##### //
// -----------Begin of function PlayerSchool::save_initial_data ------ //
void PlayerSchool::save_initial_data() {
    for (int i = 0; i < MAX_STUDENT_LEVEL_N_TOTAL; i++) {
	//		for (int j = 0; j < HISTORY_TRIMESTER_COUNT; j++)
	//			initial_ins_student_level_history[i][j] = ins_student_level_history[i][j];
	initial_ins_student_level_history[i] = ins_student_level_history[i][HISTORY_TRIMESTER_COUNT - 1];
    }

    for (int i = 0; i < GENDER_ETHNIC_TYPE_COUNT_N_TOTAL; i++) {
	//		for (int j = 0; j < HISTORY_TRIMESTER_COUNT; j++)
	//			initial_ins_gender_student_level_history[i][j] = ins_gender_student_level_history[i][j];
	initial_ins_gender_student_level_history[i] = ins_gender_student_level_history[i][HISTORY_TRIMESTER_COUNT - 1];
    }

    for (int i = 0; i < HISTORY_MONTH_COUNT; i ++)
	initial_performance_academic_ug[i] = performance_academic_ug[i];

    for (int j = 0; j < DEGREE_TYPE_COUNT; j++) {
	for (int i = 0; i < HISTORY_MONTH_COUNT; i ++)
	    initial_talent_academic_all[j][i] = talent_academic_all[j][i];
    }

    for (int i = 0; i < HISTORY_MONTH_COUNT; i++) {
	initial_talent_academic[i] = talent_academic[i];
	initial_talent_extracurricular[i] = talent_extracurricular[i];
	initial_talent_athletics[i] = talent_athletics[i];

	initial_satisfaction_academic[i] = satisfaction_academic[i];
	initial_satisfaction_student_life[i] = satisfaction_student_life[i];
	initial_satisfaction_athletics[i] = satisfaction_athletics[i];
	initial_satisfaction_overall[i] = satisfaction_overall[i];
    }

    for (int i = 0; i < HISTORY_YEAR_COUNT; i++) {
	initial_bachelor_degree[i] = bachelor_degree[i]+non_ug_bachelor_degree[i];
	initial_master_degree[i] = master_degree[i];
	initial_doctor_degree[i] = doctor_degree[i];
    }

    for (int j = 0; j < DEGREE_TYPE_COUNT; j++) {
	for (int i = 0; i < HISTORY_YEAR_COUNT; i++) {
	    initial_time_to_degree[j][i] = time_to_degree[j][i];
	    initial_dropout_rate[j][i] = dropout_rate[j][i];
	    //			initial_fake_disp_time_to_degree[j][i] = fake_disp_time_to_degree[j][i];
	    initial_fake_disp_time_to_degree[i] = fake_disp_time_to_degree[BACHELOR_DEGREE][i];
	}
    }
}

// ----------- End of function PlayerSchool::save_initial_data ------ //
// ##### End Marco ##### //
