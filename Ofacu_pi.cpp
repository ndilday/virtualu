//Filename    : OFACULTY.cpp
//Description : FACULTY Class Definition
//Own:			 Fred.

#include <OMATH.H>
#include <ODEPT.H>
#include <OINFO.H>
#include <ONAMERES.H>
#include <OFPHOTO.H>
#include <OFACULTY.H>
#include <OFINANCE.H>
#include <OPSCHOOL.H>                             // trimester_array
#include <OPEERSCH.H>
#include <OLIBTECH.H>                             // .faculty_incentive_using_it
#include <OCHANCE.H>                              //## chea 091199

//---------- Begin of function Faculty::update_history -----------//
//!
//! update history arrays
//!
//! update_flag		= UPDATE_TRIMESTER, UPDATE_MONTH, UPDATE_YEAR; (ooffice.h)
//!
void Faculty::update_history(char update_flag) {
    switch (update_flag) {
    case UPDATE_MONTH:
	satisfaction_index_last = satisfaction_index;
	calc_satisfaction_index();
	calc_performance_research();
	think_research();                           // 1118
	break;
    case UPDATE_TRIMESTER:
	satisfaction_index_last_tri = satisfaction_index;
	calc_performance_teaching();
	calc_performance_scholarship();
	calc_discretionary_hour_it();
	break;
    case UPDATE_YEAR:
	reaction_summer_teaching_last = reaction_summer_teaching;
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

//---------- End of function Faculty::update_history -----------//

//------ Begin of function Faculty::calc_all_performance -------//
//!
void Faculty::calc_all_performance() {
    err_here();

    calc_performance_teaching();
    calc_performance_scholarship();
    calc_performance_research();

    calc_satisfaction_index();
}

//------- End of function Faculty::calc_all_performance -------//

//---------- Begin of function Faculty::calc_performance_teaching -----------//
//!
void Faculty::calc_performance_teaching() {
    float input7=0;

    //-----------//
    input7 = finance.academic_support_ratio;

    //-----------//
    float talentTeaching = math.single_response_func(1, 100, 64.99f, 51.96f, talent_teaching);
    float performanceScholarship = math.dual_response_func(1, 50, 100, 21.46f, 21.95f, 20.43f, 80, performance_scholarship);
    float coursePrepareHour = math.single_response_func(1, 100, 13.05f, 6.85f, discretionary_hour_array[DT_COURSE_PREPARATION]);
    float outOfClassContactHour = math.single_response_func(1, 100, 13.05f, 6.85f, discretionary_hour_array[DT_OUT_OF_CLASS_STUDENT_CONTACT]);
    float contactHourRatio = math.dual_response_func(1, 50, 100, -0.456f, -0.461f, 1.544f, 0.452f, float(teaching_contact_hour)/NORMAL_TEACHING_CONTACT_HOUR);
    input7 = math.dual_response_func(1, 47.34f, 100, 0.216f, 0.431f, 0.119f, 0.657f, input7);

    float overallValue = talentTeaching * 0.2f +
	performanceScholarship * 0.25f +
	coursePrepareHour * 0.15f +
	outOfClassContactHour * 0.1f +
	contactHourRatio * 0.1f +
	satisfaction_index_last * 0.1f +
	input7 * 0.1f;

    //	performance_teaching = math.latency_func( 0.79f, performance_teaching, overallValue);
    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    performance_teaching = player_school.latency_func( 0.65f, performance_teaching, overallValue);

}

//---------- End of function Faculty::calc_performance_teaching -----------//

//---------- Begin of function Faculty::calc_performance_scholarship -----------//
//!
void Faculty::calc_performance_scholarship() {
    float input3 = finance.academic_support_ratio;

    //----------------//
    float talentScholarship = math.single_response_func(1, 100, 64.99f, 51.96f, talent_scholarship);
    float performanceResearch = math.dual_response_func(1, 50, 100, 21.02f, 34.78f, 19.75f, 75.56f, performance_research);
    float scholarshipHour = math.single_response_func(1, 100, 13.23f, 6.40f, discretionary_hour_array[DT_SCHOLARSHIP]);

    input3 = math.dual_response_func(1, 49.62f, 100, 0.243f, 0.303f, 0.124f, 0.634f, input3);

    //## chea 221099
    if(this->rank_level ==3 || this->rank_level ==4) {
	//		talentScholarship = 0.0f;
	//		performanceResearch = 0.0f;
	scholarshipHour = 0.0f;

    }

    float overallValue = talentScholarship * 0.3f +
	performanceResearch * 0.1f +
	scholarshipHour * 0.3f +
	satisfaction_index_last * 0.1f +
	input3 *0.2f;

    //	performance_scholarship = math.latency_func( 0.94f, performance_scholarship, overallValue);
    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    performance_scholarship = player_school.latency_func( 0.70f, performance_scholarship, overallValue);

    //## chea 221099
    //	if(this->rank_level ==3 || this->rank_level ==4)
    //		performance_scholarship = 0.0f;

}

//---------- End of function Faculty::calc_performance_scholarship -----------//

//---------- Begin of function Faculty::calc_performance_research -----------//
//!
void Faculty::calc_performance_research() {
    float input4 = finance.academic_support_ratio;

    //----------------//
    float talentResearch = math.single_response_func(1, 100, 29.02f, 60.37f, talent_research);
    float performanceScholarship = math.single_response_func(1, 100, 33.39f, 31.36f, performance_scholarship);
    float researchHour  = math.single_response_func(1, 100, 24.67f, 13.69f, discretionary_hour_array[DT_RESEARCH]);
    float researchRatio;

    float researchNorm = (float) department_array[department_recno]->research_dollar_norm;

    if( researchNorm )
	researchRatio = math.single_response_func(1, 100, 6.57f, 1.79f, research_month_expense / researchNorm );
    else
	researchRatio = 0;

    //## chea 221099
    if(rank_level ==3 || rank_level ==4) {
	researchHour = 0.0f;
	//		talentResearch = 0.0f;
	//		performanceScholarship = 0.0f;
	researchRatio = 0.0f;
    }

    input4 = math.dual_response_func(1, 49.81f, 100, 0.295f, 0.510f, 0.150f, 0.860f, input4);

    //----------------//

    float newValue = talentResearch * 0.3f +
	performanceScholarship * 0.1f +
	researchHour * 0.2f +
	researchRatio * 0.35f +
	input4 * 0.05f;

    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    performance_research = player_school.latency_func( 0.75f, performance_research, newValue);

    //---------- extra effect brought the chance cards ------------//

    if( extra_effect_performance_research ) {
	performance_research *= extra_effect_performance_research;

	extra_effect_performance_research += (1.0f - extra_effect_performance_research) * 0.3f;

	performance_research = max(0, min(100,performance_research ) );
    }
}

//---------- End of function Faculty::calc_performance_research -----------//

//---------- Begin of function Faculty::calc_discretionary_hour_it -----------//
//!
void Faculty::calc_discretionary_hour_it() {
    float input[2];

    //--------------//
    input[0] = discretionary_hour_array[DT_EDUCATIONAL_DEVELOPMENT]
	* float( float(finance.expense_array[AC_IT_RESOURCES].total) / finance.total_operating_expense.total );

    input[0] *= (float) finance.cost_rise_policy_array[PL_INFO_TECH_RESOURCES].penalty_multiplier2;

    //--------------//
    input[0] = math.dual_response_func(0,60.23f,100,0.076f,0.112f,0.033f,0.150f, input[0]);
    input[1] = math.dual_response_func(0,51.1f,75,32.26f,26.33f,19.66f,68.16f, library_tech_office.faculty_incentive_using_it);

    float newValue = 0.5f * input[0] + 0.5f * input[1];

    p_discretionary_hour_it_percent = player_school.latency_func( 0.8f, p_discretionary_hour_it, newValue);

    //--------------//
    int totalDTTime = 0;
    for (int i=0; i<DISCRETIONARY_TYPE_COUNT; i++)
	totalDTTime += discretionary_hour_array[i];

    p_discretionary_hour_it = totalDTTime * p_discretionary_hour_it_percent / 100;

}

//---------- End of function Faculty::calc_discretionary_hour_it -----------//

//---------- Begin of function Faculty::calc_satisfaction_index -----------//
//!
void Faculty::calc_satisfaction_index() {
#define VAR_COUNT   8

    float input[VAR_COUNT];
    memset(input, 0, sizeof(input));

    int i;

    //--------------//

    float peerAvg = (float) PeerSchool::average_faculty_salary[ rank_age_group() ];

    if ( peerAvg > 0 )
	input[0] = salary / peerAvg;

    //--------------//
    input[1] = float(teaching_contact_hour) / NORMAL_TEACHING_CONTACT_HOUR;

    //--------------//
    input[2] = player_school.sub_score[S_FACULTY_DIVERSITY][THIS_MONTH];

    //--------------//
    input[3] = player_school.sub_score[S_STUDENT_DIVERSITY][THIS_MONTH];

    //--------------//
    /*	Equals:

	50 + sum over i of:
	weight[i] * (performance[i] - talent[i])/
	sum of the weights,
	truncated to be between 0 and 100.

	where
	weight[i] = institutional priority[i] amd "i" ranges over teaching, scholarship, and research.

	The institutional priorities for T, S, and R are on the Faculty_Incentives sheet.
    */
    const char V_COUNT = 3;
    float subInput[V_COUNT], weight[V_COUNT];

    //	a percentage from 0 to 100
    subInput[0] = performance_teaching - talent_teaching;
    subInput[1] = performance_scholarship - talent_scholarship;
    subInput[2] = performance_research - talent_research;

    weight[0] = department_array.weight_teaching;
    weight[1] = department_array.weight_scholarship;
    weight[2] = department_array.weight_research;

    for (i=0; i<V_COUNT; i++)
	input[4] += weight[i] * subInput[i];

    //min & max bug chea
    input[4] = max(0.0f,min(100.0f, 50 + input[4] / (weight[0]+weight[1]+weight[2])));

    err_when((weight[0]+weight[1]+weight[2])==0);

    //--------------//
    // Strain on discretionary time due to institutional priorities
    input[5] = strain_on_discretionary_time;

    //--------------//
    // Reaction to emphasis on summer teaching
    // Let x[t] = the value of the emphasis variable (0,1,2, or 3) at month t, TIMES
    // (percent of summer teaching as part of regular faculty load)/100 ; . Then:

    //float emphasis_var, emphasis_var_last;
    //emphasis_var = emphasis_var_last = 1;		// changed monthly // to put it to "student Union"?

    const float lamda = 0.98f;

    reaction_summer_teaching = max(0.0f,
				   (1-lamda)*(-10)*(player_school.summer_emphasis*player_school.summer_emphasis - player_school.summer_emphasis_last*player_school.summer_emphasis_last)
				   + lamda*reaction_summer_teaching_last);       //min & max bug chea

    input[6] = reaction_summer_teaching;

    //--------------//
    // -1:last year
    input[7] = department_array[department_recno]->p_academic_standing_history[THIS_YEAR-1];

    //--------------//
    input[0] = math.dual_response_func(1, 50, 100, 0.123f, 0.123f, 0.863f, 1.136f, input[0]);
    input[1] = math.dual_response_func(1, 50, 100, -0.214f, -0.132f, 1.075f, 0.688f, input[1]);
    input[2] = math.dual_response_func(1,49.48f,100,20.74f,29.48f,20.30f,75.12f, input[2]);
    input[3] = math.dual_response_func(1,49.48f,100,20.74f,29.48f,20.30f,75.12f, input[3]);
    input[5] = math.single_response_func(100,0,-100.5f,49.07f, input[5]);

    //--------------//
    float weightWhiteMale[VAR_COUNT] = { 0.25f,0.20f,0.05f,0.05f,0.15f,0.15f,0.05f,0.10f,};
    float weightOther[VAR_COUNT] = {0.25f,0.20f,0.10f,0.10f,0.10f,0.10f,0.05f,0.10f,};

    float newValue = 0;

    if ( gender_ethnic_group == NONMINORITY_MALE ) {
	for (i=0; i<VAR_COUNT; i++)
	    newValue += weightWhiteMale[i] * input[i];
    }
    else {
	for (i=0; i<VAR_COUNT; i++)
	    newValue += weightOther[i] * input[i];
    }

    if ( is_dismissed )
	newValue *= 0.1;

    //	satisfaction_index = math.latency_func( 0.80f, satisfaction_index, newValue);
    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    satisfaction_index = player_school.latency_func( 0.20f, satisfaction_index, newValue);

    // var 12 in respone_func.xls
    if ( rank_level == ASST_PROF ) {
	// scale to =[0,100]
	int promoDiff = (player_school.faculty_promotion_difficulity - INPUT_LOW ) * 100 / (INPUT_HIGH - INPUT_LOW);

	satisfaction_index *= (1 - 0.2f * (promoDiff - 50) / 100 );
	//min & max bug chea
	satisfaction_index = max(0.0f, min(100.0f, satisfaction_index));
    }

    //----------------------------//
    // 1020 add penalty, if any; // 1118

    satisfaction_index *= (float) finance.revenue_policy_array[PL_ENDOWMENT_SPENDING_RATE].penalty_multiplier1;
    satisfaction_index *= (float) finance.revenue_policy_array[PL_INDIRECT_COST_RATE].penalty_multiplier1;

    satisfaction_index *= (float) finance.expense_policy_array[PL_FACULTY_SALARY_INCREASES].penalty_multiplier1;
    //satisfaction_index *= (float) finance.expense_policy_array[PL_REAL_TRANSFER_TO_PLANT_GROWTH].penalty_multiplier1;
    satisfaction_index *= (float) finance.expense_policy_array[PL_BUDGET_ADJUSTMENT].penalty_multiplier1;

    //---//
    satisfaction_index *= (float) finance.cost_rise_policy_array[PL_FACULTY_FTE].penalty_multiplier1;
    satisfaction_index *= (float) finance.cost_rise_policy_array[PL_NON_FACULTY_DEPARTMENTAL_EXPENSE].penalty_multiplier1;
    satisfaction_index *= (float) finance.cost_rise_policy_array[PL_LIBRARIES].penalty_multiplier1;

    //---- chance card extra effect ------//

    if( extra_effect_satisfaction_index ) {
	satisfaction_index *= extra_effect_satisfaction_index;

	extra_effect_satisfaction_index += (1.0f - extra_effect_satisfaction_index) * 0.3f;
    }

    //--------- cap the value ------------//

    //min & max bug chea
    satisfaction_index = max(0.0f,min(100.0f,satisfaction_index));

}

//---------- End of function Faculty::calc_satisfaction_index -----------//
