//Filename    : OFACULTY.cpp
//Description : FACULTY Class Definition

#include <OSYS.H>
#include <OMATH.H>
#include <ODEPT.H>
#include <OINFO.H>
#include <OCHANCE.H>
#include <ONAMERES.H>
#include <OFPHOTO.H>
#include <OPSCHOOL.H>
#include <OFACULTY.H>
#include <OFINANCE.H>
#include <OFACILIT.H>
#include <OPSCHOOL.H>                             // trimester_array
#include <OLIBTECH.H>                             // .faculty_incentive_using_it
#include <ODEPTRES.H>
#include <OPEERSCH.H>
#include <OSCHLRES.H>

//-------- declare static funciton ------------//

static int get_discretionary_time_pref(char* discretionaryTimePref, int index);

//---------- Begin of function Faculty::init -----------//
//!
void Faculty::init(int departmentRecno, int rankLevel, int genderEthnicGroup, int facultyAge, int startTeachingDate,
		   int facultySalary, int talentTeaching, int talentScholarship, int talentResearch) {
    memset( this, 0, sizeof(Faculty) );

    err_when( genderEthnicGroup < 0 || genderEthnicGroup >= GENDER_ETHNIC_TYPE_COUNT );

    //----------------------------------------------//

    department_recno      = departmentRecno;
    rank_level          = rankLevel;
    gender_ethnic_group     = genderEthnicGroup;

#ifdef DEBUG
    rank_age_group(rank_level, facultyAge);
#endif

    birthday            = info.game_date - int((float)(facultyAge)*365.25) - m.random(300);
    salary            = facultySalary;
    teaching_contact_hour = CONTACT_HOUR_PER_COURSE;//0112	// starts at zero, it is set in the course matching process
    start_teaching_date   = startTeachingDate;

    if ( age() != facultyAge && facultyAge == 41 )  // FULL PROFESSOR_1
	birthday -= 365;                              // fix bug on fly // 1102

  //--------- init name ----------//

    int loopCount=0;
    int curGender;

    if( gender_ethnic_group == NONMINORITY_MALE ||
	gender_ethnic_group == MINORITY_MALE ) {
	curGender = 'M';
    }
    //### fred 1023
    else if ( gender_ethnic_group == NONMINORITY_FEMALE ||
	      gender_ethnic_group == MINORITY_FEMALE )
	curGender = 'F';
    else
	err_here();
    //### fred 1023

    do {
	while(1) {
	    first_name_id = m.random(name_res.first_name_count)+1;

	    if( name_res.first_name_array[first_name_id-1].gender == curGender )
		break;
	    ++loopCount;
	    err_when( loopCount > 1000 );
	}

	middle_name_id = m.random(name_res.middle_name_count)+1;
	last_name_id   = m.random(name_res.last_name_count)+1;

	// skip Dick Head, also can skip combination here
    } while( strcmp( name_res.first_name_array[first_name_id-1].name, "Dick")==0
	     && strcmp( name_res.last_name_array[last_name_id-1].name, "Head")==0 );

    //---------- research proposals ----------//

    research_proposal_count = 0;
    research_month_expense  = 0;
    research_month_expense_direct = 0;              //## chea 151199 forget to init

    employ_status = 4;

    //-------------- trimester ---------------//

    off_duty_trimester = SUMMER;                    // it is default to SUMMER for all faculty
    is_third_trimester_teaching = 0;                // it is default to 0 for all faculty

  //----------------------------------------//
    reaction_summer_teaching = 0;

    // initial value may be larger than 100, so we clip it at 100 here
    talent_teaching    = (float) min(100, talentTeaching);
    talent_scholarship = (float) min(100, talentScholarship);
    talent_research  = (float) min(100, talentResearch);

    err_when(talent_teaching<0 || talent_scholarship<0 || talent_research<0);

  //------ pick a photo for the faculty -------//

    init_photo();
    this->name();

  //----------------------------------------//

  // 0108
    int checkSum = 0;
    int checkSum2 = 0;
    char tmplCode = 'A';

    FacultyTemplate *facultyTemplate = faculty_res.get_faculty_template( &tmplCode , rank_age_group(rank_level, age()));

    for( int j=0 ; j<DISCRETIONARY_TYPE_COUNT ; j++ ) {
	this->discretionary_hour_array[j] = NORMAL_DISCRETIONARY_HOUR * get_discretionary_time_pref( facultyTemplate->discretionary_time_pref, j ) / 100;
	checkSum += discretionary_hour_array[j];

	checkSum2 += get_discretionary_time_pref( facultyTemplate->discretionary_time_pref, j );
    }

    err_when(checkSum <= 0 || checkSum2 < 90 || checkSum2 > 105);

  //----------------------------------------//

    Faculty* facultyPtr = this;
    float qualityDriver = facultyPtr->get_quality_driver();
    float oRate = finance.get_research_overhead_rate() / 100.0f;

    //## chea 161099
    //	facultyPtr->proj_count_mult =
    //		float (0.8 + 0.004 * math.dual_response_func(1,50,100,29,33,20,76, qualityDriver));
    //	facultyPtr->proj_size_mult =
    //		float (0.7 + 0.006 * math.dual_response_func(1,50,100,29,33,20,76, qualityDriver) * math.safe_pow(1.0f+oRate, -0.5f) );
    //	facultyPtr->award_prob =
    //		float (0.01 + 0.005 * math.dual_response_func(1,50,100,29,33,20,76, qualityDriver) * math.safe_pow(1.0f+oRate, -1.5f) );

  //## chea 161099
  //	facultyPtr->proj_count_mult =
  //		float (0.8 + 0.4 * math.dual_response_func(1,50,100,29,33,20,76, qualityDriver)/100);
  //	facultyPtr->proj_size_mult =
  //		float (0.7 + 0.6 * math.dual_response_func(1,50,100,29,33,20,76, qualityDriver)/100 * math.safe_pow(1.0f+oRate, -0.5f) );
  //	facultyPtr->award_prob =
  //		float (0.01 + 0.5 * math.dual_response_func(1,50,100,29,33,20,76, qualityDriver)/100 * math.safe_pow(1.0f+oRate, -1.5f) );
  //		float (0.01 + 0.5 * math.dual_response_func(1,50,100,29,33,20,76, qualityDriver)/100 * math.safe_pow(1.0f+oRate, -1.5f) );//## chea 031199

  //## chea 191199
  //## chea 041199 new request
    facultyPtr->proj_count_mult = (0.8f + 0.4f * this->talent_research/100);
    facultyPtr->proj_size_mult = (0.7f + 0.6f * this->talent_research/100);
    //	if(facultyPtr->talent_research <= 20)
    //		facultyPtr->award_prob = 0.0f;
    //	else if(facultyPtr->talent_research <= 40)
    //		facultyPtr->award_prob = 0.05f;
    //	else if(facultyPtr->talent_research <= 60)
    //		facultyPtr->award_prob = 0.10f;
    //	else if(facultyPtr->talent_research <= 80)
    //		facultyPtr->award_prob = 0.15f;
    //	else
    //		facultyPtr->award_prob = (facultyPtr->talent_research/100);
    // this->award_prob = (float)(0.01f + 0.5 * this->talent_research/100);
    // change in 18/03/2002
    float awardProb = 2.0f * (qualityDriver * 0.01f) / math.safe_pow( (1.0f + oRate), 3.0f );
    this->award_prob = m.fmin( 0.85f, m.fmax(0.10f, awardProb) );

    Department* deptPtr = department_array[department_recno];
    char rankAgeGroup = rank_age_group(rank_level, age());
    deptPtr->init_proj_vars(this);

    //------ initialize faculty discretionary time ------//

    //	DepartmentInfo* deptInfo = department_res[deptPtr->department_id];

    //	facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_discretionary_time, rankAgeGroup );

    //	for( j=0 ; j<DISCRETIONARY_TYPE_COUNT ; j++ )
    //		facultyPtr->discretionary_hour_array[j] = NORMAL_DISCRETIONARY_HOUR * facultyTemplate->discretionary_time_pref[j] / 100;
}

//---------- End of function Faculty::init -----------//

//---------- Begin of function get_discretionary_time_pref -----------//
//!
//! It returns the normal discretionary time pref, unless in some scenarios.
//!
static int get_discretionary_time_pref(char* discretionaryTimePref, int index) {
    if( player_school.scenario_id == SCN_TEACHING_QUALITY || player_school.scenario_id == SCN_RESEARCH_QUALITY ) {
	//---- create a template pref array for storing the modified values -----//

	float tempPref[DISCRETIONARY_TYPE_COUNT];     // discretionary time preference // 0-100

	for( int i=0 ; i<DISCRETIONARY_TYPE_COUNT ; i++ ) {
	    tempPref[i] = (float) discretionaryTimePref[i];
	}

	//-------- process some special pref -----//

	if ( player_school.scenario_id == SCN_TEACHING_QUALITY ) {
	    tempPref[DT_OUT_OF_CLASS_STUDENT_CONTACT] *= 0.333f;
	    tempPref[DT_EDUCATIONAL_DEVELOPMENT] *= 0.333f;
	}
	else if( player_school.scenario_id == SCN_RESEARCH_QUALITY ) {
	    tempPref[DT_RESEARCH]  *= 0.333f;
	    tempPref[DT_SCHOLARSHIP]  *= 0.333f;
	}

	//--------- renormalize ---------//

	float totalPref=0;

	for( int i=0 ; i<DISCRETIONARY_TYPE_COUNT ; i++ ) {
	    totalPref += tempPref[i];
	}

	for( int i=0 ; i<DISCRETIONARY_TYPE_COUNT ; i++ ) {
	    tempPref[i] = tempPref[i] * 100 / totalPref;
	}

	int ret = (int) tempPref[index];

	if( tempPref[index] - (int) tempPref[index] > 0.5f )
	    ret++;

	return ret;
    }
    else {
	return discretionaryTimePref[index];
    }
}

//---------- End of function get_discretionary_time_pref -----------//

//---------- Begin of function Faculty::init_photo -----------//
//!
void Faculty::init_photo() {
    //------ pick a photo for the faculty -------//

    Department* deptPtr = department_array[department_recno];

    //---- pick a photo randomly --------//

    int photoId = m.random(faculty_photo_res.photo_count)+1;
    int tryCount=0;

    while(1) {
	if( ++photoId > faculty_photo_res.photo_count )
	    photoId = 1;

	if( faculty_photo_res[photoId]->gender_ethnic_group != this->gender_ethnic_group )
	    continue;

	//--- scan if this photo has already been used by a faculty in this department ---//

	int duplicatedFlag = 0;

	for( int i=deptPtr->faculty_array.size() ; i>0 ; i--) {
	    if( deptPtr->faculty_array.is_deleted(i) )
		continue;

	    if( deptPtr->faculty_array[i]->photo_id == photoId ) {
		duplicatedFlag = 1;
		break;
	    }
	}

	//--- if this photo hasn't been used in this department, then use it for this faculty ---//

	// or if the number of faculty in this department > total number of available photos
	if( !duplicatedFlag || tryCount > faculty_photo_res.photo_count ) {
	    photo_id = photoId;
	    return;
	}

	++tryCount;

	err_when( tryCount > 1000 );
    }
}

//---------- End of function Faculty::init_photo -----------//

//---------- Begin of function Faculty::next_day -----------//
//!
void Faculty::next_day() {
    //return;		//for interface debug //BUGHER

    // start of a trimester
    if ( info.game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	 && info.game_month == player_school.trimester_array[player_school.cur_trimester].start_month ) {
	update_history(UPDATE_TRIMESTER);
	if ( think_dismiss() )
	    return;
    }

    // start of a month
    if ( info.game_day == 1 ) {
	update_history(UPDATE_MONTH);

	sys.yield();
	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month )
	    update_history(UPDATE_YEAR);
    }
}

//---------- End of function Faculty::next_day -----------//

//---------- Begin of function Faculty::next_trimester -----------//
//!
void Faculty::next_trimester() {
    //----- reset vars -----//

    teaching_contact_hour = 0;                      // this will be calculated in couse selection model

    //----------------------//

    if ( think_departure() )                        // run this yearly would be OK too
	return;

    think_discretionary_time();                     // before promotion

    if ( think_promotion() )
	return;
}

//---------- End of function Faculty::next_trimester -----------//

//---------- Begin of function Faculty::think_departure --------//
//!
//! 1106 handle departure and retir procedure
//!
int Faculty::think_departure() {
    // faculty will retire if he is old
    if ( age() >= 72 ) {
	department_array[department_recno]->faculty_array.del(this->faculty_recno);
	return 1;
    }

    float multiplier = 1-(satisfaction_index-50)/100.0f;
    float multiplierLast = 1-(satisfaction_index_last-50)/100.0f;

    // var 16
    multiplier = player_school.latency_func(0.25f, multiplierLast, multiplier);

    // departure prob
    int departmentId = department_array[department_recno]->department_id;
    float departProb = department_res[departmentId]->faculty_departure_probability[rank_level][department_res.get_prob_school_type()];

    // * 0.1: added on 0107 *1.25: added on 1503
    if ( math.get_random_float() > multiplier * departProb *1.25 * 0.1)
	return 0;

    //----- remove the faculty from faculty_array -----//
    //return 0;		//BUGHER

    department_array[department_recno]->faculty_array.del(this->faculty_recno);

    return 1;
}

//---------- End of function Faculty::think_departure -----------//

//---------- Begin of function Faculty::think_dismiss --------//
//!
//! 1106 handle dismiss procedure
//!
int Faculty::think_dismiss() {
    if ( is_dismissed ) {                           // check if this faculty is dismissed
	if ( dismiss_trimester == 1 ) {
	    department_array[department_recno]->faculty_array.del(this->faculty_recno);

	    return 1;
	}
	else
	    dismiss_trimester--;
    }

    return 0;
}

//---------- End of function Faculty::think_dismiss -----------//

//---------- Begin of function Faculty::think_promotion -----------//
//!
int Faculty::think_promotion() {
    // var 17 in response func
    // Multiplies the promotion probabilities given in Faculaty_TransProbs of HE.GDB.init.

    float prob;
    DepartmentInfo *deptInfo = department_res[department_array[department_recno]->department_id];

    if ( rank_level == ASSOC_PROF && age() >= 41 )
	prob = deptInfo->associate_professor_promotion_probability[department_res.get_prob_school_type()];
    else if ( rank_level == ASST_PROF )
	prob = deptInfo->assistant_professor_promotion_probability[department_res.get_prob_school_type()];
    else
	return 0;

  //--------------//

  // Enlight: don't do the amendment below as the has been programmed already.
  // May also be used in place of paragraphs 5 and 6 of Section 3.3 of Td_3.5. (It
  // accomplishes the same thing.) If so used, paragraph 5 is deleted and paragraph 6 becomes:
  // Overall merit adjustment = base increase * merit priority * this multiplier

  // The weight formulas are given in the "Implied institutional performance ratings"
  // column on the Faculty_Incentives sheet.
  // Transform the weighted average to the multiplier as follows:
  // multiplier = 1+(wgtdAve-50)/100

    Department* deptPtr = department_array[department_recno];

    float multiplier = 1, sum = 0;
    float weight[3];
    float myTalent[3] = { talent_teaching, talent_scholarship, talent_research };

    weight[0] = (deptPtr->priority_discretionary_hour_array[0]+deptPtr->priority_discretionary_hour_array[1]+deptPtr->priority_discretionary_hour_array[2]) / 3.0f;
    weight[1] = deptPtr->priority_discretionary_hour_array[DT_SCHOLARSHIP];
    weight[2] = deptPtr->priority_discretionary_hour_array[DT_RESEARCH];

    for (int i=0; i<3; i++)
	sum += weight[i];

    for (int i=0; i<3; i++)
	weight[i] = math.safe_divide( weight[i], sum );
    //		weight[i] /= sum;

    for (int i=0, sum=0; i<3; i++)
	sum += weight[i] * myTalent[i];

    multiplier = 1+(sum-50.0f)/100;

    //--------------//

    if ( rank_level == ASST_PROF ) {
	const int remap[INPUT_OPTION_COUNT] = { 25, 50, 74 };

	multiplier *= 1-(remap[player_school.faculty_promotion_difficulity]-50)/100.0f;
	//## chea 201099
	//		multiplier = math.single_response_func(0.6f, 1.4f, 100.0f, 50.0f, multiplier);	// var 18
	// var 18
	multiplier = math.single_response_func(0.1f, 1.9f, 100.0f, 50.0f, multiplier);
    }

    if ( math.get_random_float() > multiplier * prob )
	return 0;

    rank_level++;

    // update display_faculty_array && display_faculty_count in department

    Faculty* orgPtr = this;

    deptPtr->cur_faculty_array.linkin(orgPtr);

    int arraySize = deptPtr->cur_faculty_array.size();

    Faculty* facPtr = (Faculty*) deptPtr->cur_faculty_array.get(arraySize);

    facPtr->employ_status = 2;                      // Promotion

    return 1;
}

//---------- End of function Faculty::think_promotion -----------//

//---------- Begin of function Faculty::think_accept_retire --------//
//!
//! 1106 var 20 in response_func.xls; called directly by interface
//!
int Faculty::think_accept_retire(int retireOffer) {
    const int VAR_COUNT = 2;
    float input[VAR_COUNT];

    err_when(retireOffer<0);

    if ( rank_level == LONG_TERM_ADJUNCT  )
	input[0] = float(retireOffer) / salary;
    else
	input[0] = float(retireOffer) / salary * 0.5;

    input[1] = float(100 - satisfaction_index) / 100;

    //-------------//
    input[0] = math.dual_response_func(0, 0.340f, 1.000f, 0.668f, 3.015f, 0.505f, 2.476f, input[0]);

    float prob = 0;
    float weight[VAR_COUNT] = { 0.5f, 0.5f };

    for (int i=0; i<VAR_COUNT; i++)
	prob += weight[i] * input[i];

    //-------------//

    if ( math.get_random_float() > prob )
	return 0;

    department_array[department_recno]->faculty_array.del(faculty_recno);

    return 1;
}

//---------- End of function Faculty::think_accept_retire -----------//

//---------- Begin of function Faculty::think_discretionary_time -----------//
//!
void Faculty::think_discretionary_time() {
    int i;

    float researchdisTimeOffset;                    // 0-100

    // "Faculty incentive" worksheet in response func	// 1116

    Department* deptPtr = department_array[department_recno];
    DepartmentInfo* deptInfo = department_res[deptPtr->department_id];
    FacultyTemplate* templ = faculty_res.get_faculty_template(deptInfo->template_discretionary_time, rank_age_group() );

    researchdisTimeOffset = math.safe_divide(research_month_expense_direct, deptPtr->research_dollar_norm);
    researchdisTimeOffset = math.dual_response_func(0,10.09f, 20, 0.802f, 0.690f, 0.403f, 2.189f, researchdisTimeOffset);

    err_when(researchdisTimeOffset < 0);

    // 99->99%
    float adjustedBaseDisTime[DISCRETIONARY_TYPE_COUNT];

    for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ )
	adjustedBaseDisTime[i] = (float) get_discretionary_time_pref( templ->discretionary_time_pref, i);

    float base = 1 + (researchdisTimeOffset/100.0f) * (1.0f-adjustedBaseDisTime[DT_RESEARCH]/100);

    err_when(base == 0);

    for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ )
	adjustedBaseDisTime[i] /= base;

    adjustedBaseDisTime[DT_RESEARCH] += (researchdisTimeOffset/100.0f) * (1.0f-adjustedBaseDisTime[DT_RESEARCH]/100) * 100.0f / base;

    //----------------//
    /*
      float total = 0;

      for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ )
      total += adjustedBaseDisTime[i];

      err_when(total < 96 || total > 104);
    */
    //----------------//  // column F-G

    //----//
    // 0129 scenario(4): rescale templ->discretionary_time_pref[]

    //----//

    // 99->99%
    float normalizedRawTimePercent[DISCRETIONARY_TYPE_COUNT];

    const int remap[INPUT_OPTION_COUNT] = {25,50,75};
    int ratio = remap[player_school.faculty_degree_to_which_priorities_reflected_in_promotion];

    i = DT_COURSE_PREPARATION;
    normalizedRawTimePercent[i] = (ratio*deptPtr->relative_priority_discretionary_hour_array[i] / 100.0f
				   + talent_teaching * get_discretionary_time_pref( templ->discretionary_time_pref, i) / 100.0f) ;

    i = DT_OUT_OF_CLASS_STUDENT_CONTACT;
    normalizedRawTimePercent[i] = (ratio*deptPtr->relative_priority_discretionary_hour_array[i] / 100.0f
				   + talent_teaching * get_discretionary_time_pref( templ->discretionary_time_pref,i) / 100.0f) ;

    i = DT_EDUCATIONAL_DEVELOPMENT;
    normalizedRawTimePercent[i] = (ratio*deptPtr->relative_priority_discretionary_hour_array[i] / 100.0f
				   + (talent_teaching + talent_scholarship) * get_discretionary_time_pref( templ->discretionary_time_pref,i) / 200.0f) ;

    i = DT_RESEARCH;
    normalizedRawTimePercent[i] = (ratio*deptPtr->relative_priority_discretionary_hour_array[i] / 100.0f
				   + talent_research * adjustedBaseDisTime[i] / 100.0f) ;

    i = DT_SCHOLARSHIP;
    normalizedRawTimePercent[i] = (ratio*deptPtr->relative_priority_discretionary_hour_array[i] / 100.0f
				   + talent_scholarship * get_discretionary_time_pref( templ->discretionary_time_pref,i) / 100.0f);

    i = DT_INSTITUTIONAL_AND_PUBLIC_SERVICE;
    normalizedRawTimePercent[i] = (ratio*deptPtr->relative_priority_discretionary_hour_array[i] / 100.0f
				   + 50 * get_discretionary_time_pref( templ->discretionary_time_pref, i) / 100.0f) ;

    float total;

    for ( total=0, i=0; i < DISCRETIONARY_TYPE_COUNT; i++ )
	total += normalizedRawTimePercent[i];

    err_when(total < 0);

    for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ )
	normalizedRawTimePercent[i] *= 100.0f / total;

    //----------------//	// column H

    float priorPercent[DISCRETIONARY_TYPE_COUNT];

    for ( total=0, i=0; i < DISCRETIONARY_TYPE_COUNT; i++ ) {
	priorPercent[i] = this->discretionary_hour_array[i];
	total += priorPercent[i];
    }

    err_when(total < 0);

    if ( total > 0 )
	for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ )
	    priorPercent[i] *= 100.0f / total;

    //----------------//	// column I

    float checkSum = 0, checkSum2;

    for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ ) {
	priorPercent[i] = player_school.latency_func(0.67f, normalizedRawTimePercent[i], priorPercent[i]);

	// fix in version 2, long and short term adjunct don't do research and scholarship
	if( (rank_level == LONG_TERM_ADJUNCT || rank_level == SHORT_TERM_ADJUNCT)
	    && (i == DT_RESEARCH || i == DT_SCHOLARSHIP) )
	    priorPercent[i] = 0.0f;

	checkSum += priorPercent[i];
    }

    err_when( checkSum <=0);

    for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ ) {
	priorPercent[i] *= 100 / checkSum;
    }
    /*
      int check_rank_level=0;
      check_rank_level = this->rank_level;

      for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ )
      {
      if(check_rank_level ==3 || check_rank_level ==4)
      priorPercent[check_rank_level] = 0.0f;
      else
      priorPercent[i] *= 100 / checkSum;
      }
    */
    //----------------//

    checkSum = 0; checkSum2 = 0;

    int check_rank_level=0;

    check_rank_level = this->rank_level;            //## chea 221099

    for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ ) {
	// 0108:  + 0.5
	this->discretionary_hour_array[i] = (float)(total * priorPercent[i] / 100);

	checkSum += priorPercent[i];
	checkSum2 += discretionary_hour_array[i];
    }

    err_when(total<=0 || checkSum < 90 || checkSum > 105);
    err_when(checkSum2 <=0);

    //----------------//

    float strainPriority=0;

    base = 3*talent_teaching+talent_research+talent_scholarship+50;

    for ( total=0, i=0; i < DISCRETIONARY_TYPE_COUNT; i++ ) {
	total += 100 * talent_teaching * math.safe_pow((priorPercent[i] - adjustedBaseDisTime[i])/100.0f, 2) / base;

	// * 10: since input is  [0,10]
	strainPriority += math.safe_pow((deptPtr->priority_discretionary_hour_array[i] - deptPtr->last_priority_discretionary_hour_array[i]) * 10.0f, 2) / 100.0f;
    }

    //----------------//

    // char pressure_to_change_teaching_load;		// =[-50,50]
    float strainValue = 5 - deptPtr->pressure_to_change_teaching_load / 10.0f;

    strain_priority = player_school.latency_func(0.5f, strain_priority, strainPriority);

    const float weight[2] = {0.4f, 0.35f};

    strain_on_discretionary_time =  weight[0]* total + weight[1] * strainValue + (1-weight[0]-weight[1]) * strain_priority;

    //----------------//

    for ( total=0, i=0; i < DISCRETIONARY_TYPE_COUNT; i++ ) {
	//## 990609 BUGHERE each faculty in this department will run this and hence is WRONG!
	deptPtr->last_priority_discretionary_hour_array[i] = deptPtr->priority_discretionary_hour_array[i];
    }

    memcpy(last_discretionary_hour_array, discretionary_hour_array, sizeof(last_discretionary_hour_array));
}

//---------- End of function Faculty::think_discretionary_time -----------//

//---------- Begin of function Faculty::think_research -----------//
//!
//! called monthly
//!
void Faculty::think_research() {

    // 990604
    if ( player_school.sponsored_research_intensity <= 0 ) {

	err_when(research_proposal_count>0);          //5.1.4
	return;
    }

    int i;

    //------------------------//
    if ( rank_level == LONG_TERM_ADJUNCT || rank_level == SHORT_TERM_ADJUNCT || ave_proj_size <=0 )
	return;

    //------------------------//

    // 990414

    float oRate = finance.get_research_overhead_rate() / 100.0f;
    float qualityDriver = this->get_quality_driver();
    //	this->award_prob =
    //## 251099
    //		float (0.01 + 0.005 * math.dual_response_func(1,50,100,29,33,20,76, qualityDriver) * math.safe_pow(1.0f+oRate, -1.5f) );
    //		float (0.01 + 0.5 * math.dual_response_func(1,50,100,29,33,20,76, qualityDriver)/100 * math.safe_pow(1.0f+oRate, -1.5f) );

    //## chea 041199 new request
    //## chea 191199
    this->proj_count_mult = (0.8f + 0.4f * this->talent_research/100);
    this->proj_size_mult = (0.7f + 0.6f * this->talent_research/100);
    // this->award_prob = (float)(0.01f + 0.5 * this->talent_research/100);
    // change in 18/03/2002
    float awardProb = 2.0f * (qualityDriver * 0.01f) / math.safe_pow( (1.0f + oRate), 3.0f );
    this->award_prob = m.fmin( 0.85f, m.fmax(0.10f, awardProb) );

    //	if(this->talent_research <= 20)
    //		this->award_prob = 0.0f;
    //	else if(this->talent_research <= 40)
    //		this->award_prob = 0.05f;
    //	else if(this->talent_research <= 60)
    //		this->award_prob = 0.10f;
    //	else if(this->talent_research <= 80)
    //		this->award_prob = 0.15f;
    //	else
    //		this->award_prob = (this->talent_research/100);

    //------------------------//

    Department* deptPtr = department_array[department_recno];
    DepartmentInfo* deptInfo = department_res[deptPtr->department_id];
    FacultyTemplate* templ = faculty_res.get_faculty_template(deptInfo->template_discretionary_time, rank_age_group() );
    PeerSchool *playerSchool = school_res.player_peer_school;

    //------------------------//

    for (i=0; i<research_proposal_count; i++) {
	ResearchProposal* researchProposal = research_proposal_array+i;

	if ( researchProposal->status == RESEARCH_PROPOSED ) {
	    // 990412 if ( math.get_random_float() < researchProposal->prob_proposal_funded )
	    if ( math.get_random_float() < award_prob ) {
		// accepted
		researchProposal->status = RESEARCH_ACTIVE;

		//				info.debug_enroll++;

		//				researchProposal->date_to_next_status = info.game_date + (4 + m.random(MAX_ACTIVE_RESEARCH_MONTH-3)) * 30;		// draw a random number from 4 to 12 months
		//## chea 221199 make the act pro.month=12
		//				researchProposal->date_to_next_status = info.game_date + (m.random(7)) * 30;		//## chea 240899 draw a random number from 0 to 6 months
		//## chea 240899 draw a random number from 0 to 6 months
		researchProposal->date_to_next_status = info.game_date + (1+m.random(12)) * 30;
		//				researchProposal->date_to_next_status = info.game_date + (4 + m.random(MAX_ACTIVE_RESEARCH_MONTH-3)) * 30;		//## chea 240899 draw a random number from 0 to 6 months
		//				researchProposal->date_to_next_status = info.game_date + (1) * 30;		//## chea 251099 test

		//## chea 151199 try my cal. method 1.4.1
		research_month_expense += researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH;
		research_month_expense_direct += int((researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH ) / ( 1+researchProposal->overhead_rate));
		deptPtr->research_m_history[R_ACCEPT][THIS_MONTH] += (float)researchProposal->total_dollars / (MAX_ACTIVE_RESEARCH_MONTH * 1000);
	    }
	}
	else if ( researchProposal->status == RESEARCH_ACTIVE ) {
	}
	else
	    err_here();

	//## chea 081199 old alg.BEGIN here this has not consider the prerun year I will -1 year in oinfo.cpp
	// depends on game_date  //## chea 031199
	if ( info.game_date >= researchProposal->date_to_next_status) {
	    // expired: proposal rejected or research finished
	    if ( researchProposal->status == RESEARCH_PROPOSED ) {
		deptPtr->research_m_history[R_REJECT][THIS_MONTH] += (float)researchProposal->total_dollars / (MAX_ACTIVE_RESEARCH_MONTH * 1000);
	    }
	    // fix in version 2
	    // else if ( researchProposal->status = RESEARCH_ACTIVE )
	    else if ( researchProposal->status == RESEARCH_ACTIVE ) {
		//## chea 151199 try my cal. method 1.4.1
		research_month_expense -= researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH;
		research_month_expense_direct -= int((researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH ) / ( 1+researchProposal->overhead_rate));

	    }

	    for (int j=i+1; j<research_proposal_count; j++) {
		memcpy(research_proposal_array+j-1, research_proposal_array+j, sizeof(ResearchProposal));
	    }
	    research_proposal_count--;
	    i--;

	}
	//## chea 081199 old alg.END here this has not consider the prerun year
    }

    //------------------------//	var 24

    /*
      = 0.001 * (% fac in dept who are PIs) +
      Sqrt[expProjs/actProjs] +
      Sqrt[fac's res. quality drivers/100] +
      (faculty's research discretionary time, per week),

      where

      % faculty who are PIs is defined in C18;C22 of HE.GDB.init:Faculty_Initialization;
      actProjs = number of active proposals and awards for this professor
      expProjs = expected number of projects per principal investigator in this department and age/rank category: in D18:D22 of Fac_Init.
    */

    /*

      990412

      6. For each faculty sim, determine the new proposals for the month (all variables refer to the current month) :
      prob (n, ave_proj_count/ award_prob) for n = 0 to max_n
      Number of outstanding proposals is determined according to this probability vector (many faculty will have no proposals). Then, for each proposal:

      proposal_size = ave_proj_size *random_normal_deviate(1.0, 0.12) and
      direct = project_size/(1.0+ind_cost_rate); indirect = project_size - direct; and
      number of months remaining = 2 + rand_int[0,4]] (as now).

      (The indirect cost rate is determined once and for all when the proposal is generated.)

    */

    //990412

    Faculty* facultyPtr = this;
    int projectCount = 0;
    float prob=0;

    //	const float forCalcProb = (facultyPtr->ave_proj_count / facultyPtr->award_prob ); //tryttt
    //	const float forCalcProb = (facultyPtr->ave_proj_count * deptInfo->percent_pi_faculty/100.0f / facultyPtr->award_prob );  //## chea 230899
    //	const float forCalcProb = (facultyPtr->proj_size_mult * facultyPtr->ave_proj_count / DCON*facultyPtr->award_prob ); 	//## CHEA 251099 supose had k1 in there
    //	const float forCalcProb = (facultyPtr->ave_proj_count / facultyPtr->award_prob ); 	//## CHEA 251099 supose had k1 in there

    float normalSpaceArea = facility_office.normal_area[HISTORY_MONTH_COUNT-1];
    float actualSpaceArea = facility_office.actual_area[HISTORY_MONTH_COUNT-1];
    float shortFallRatio = max(0, math.safe_divide((normalSpaceArea-actualSpaceArea),normalSpaceArea));

    float forCalcProb = (facultyPtr->award_prob);

    forCalcProb *= (1-shortFallRatio/0.15);

    float randNum = math.get_random_float();

    for ( i=0, prob=0; i<=MAX_RESEARCH_PROPOSAL; i++) {
	prob += math.double_poisson(i, forCalcProb);

	if ( randNum < prob ) {
	    projectCount = i;
	    break;
	}
    }

    //----- add research proposals -----//

    for( ; projectCount > 0 && facultyPtr->research_proposal_count < MAX_RESEARCH_PROPOSAL;
	 projectCount--, facultyPtr->research_proposal_count++ ) {
	ResearchProposal* researchProposal = facultyPtr->research_proposal_array + facultyPtr->research_proposal_count;

	//		int tmp = (int) max(0.0f, facultyPtr->ave_proj_size * math.get_random_snd(1.0f, 0.05f)); //min & max bug chea
	//		int tmp = (int) max(0.2f, facultyPtr->proj_size_mult * facultyPtr->ave_proj_size * math.get_random_snd(1.0f, 0.20f)); //## chea 251099
	//## chea 221199
	//## chea 061299 test
	//		int tmp = (int) max(0.0f, facultyPtr->ave_proj_size * 1000 * math.get_random_snd(1.0f, 0.05f)); //min & max bug chea

	//## chea 021299 1.8.4
	//min & max bug chea
	int tmp = (int) m.fmax(0.0f, facultyPtr->proj_size_mult * facultyPtr->ave_proj_size * 1000 * math.get_random_snd(1.0f, 0.10f));

	//## chea 151199 try to make the research projet non 0
	//		if(tmp >=0 && tmp <= 1)
	//			tmp =1;

	//## chea 191199 1.8.3
	//		if(tmp >=50)
	//			tmp = 50;

	researchProposal->total_dollars = tmp ;

	researchProposal->overhead_rate = oRate;

	researchProposal->status = RESEARCH_PROPOSED;

	//    info.debug_enroll3++;

	//## chea 221199 make the pro. month=3
	//		researchProposal->date_to_next_status = info.game_date + (2 + m.random(5) ) * 30;		// draw a random number from 2 to 6 months
	// draw a random number from 1 to 3 months
	researchProposal->date_to_next_status = info.game_date + (1 + m.random(3) ) * 30;

	deptPtr->research_m_history[R_PROPOSAL][THIS_MONTH] += (float)researchProposal->total_dollars / (MAX_ACTIVE_RESEARCH_MONTH * 1000);
    }

}

//---------- End of function Faculty::think_research -----------//

//---------- Begin of function Faculty::employed_period -----------//
//!
char* Faculty::employed_period() {
    static String str;

    int yearCount  = (info.game_date-start_teaching_date) / 365;
    int monthCount = ((info.game_date-start_teaching_date) % 365) / 30;

    //-------- number of years ----------//

    if( yearCount > 0 ) {
	str = yearCount;

	if( yearCount > 1 )
	    str += " years";
	else
	    str += " year";
    }
    else
	str = "";

    str += " ";

    //-------- number of months ----------//

    if( monthCount > 0 ) {
	str += monthCount;

	if( monthCount > 1 )
	    str += " months";
	else
	    str += " month";
    }

    return str;
}

//---------- End of function Faculty::employed_period -----------//

//---------- Begin of function Faculty::total_hour -----------//
//!
float Faculty::total_hour() {
    float totalHour=teaching_contact_hour;

    for( int i=0 ; i<DISCRETIONARY_TYPE_COUNT ; i++ )
	totalHour += discretionary_hour_array[i];

    return totalHour;
}

//---------- End of function Faculty::total_hour -----------//

//------ Begin of function Faculty::hour_str -------//
//!
char* Faculty::hour_str(float hourCount) {
    static String str;

    str = hourCount;

    if( hourCount > 1 )
	str += " hours";
    else
	str += " hour";

    return str;
}

//------- End of function Faculty::hour_str -------//

//------ Begin of function Faculty::name -------//
//!
char* Faculty::name() {
    // 990521

    if ( ! name_res.is_male(first_name_id) ) {      // is female
	String str = name_res.first_name_array[first_name_id-1].name;

	if ( str == "Catherine" &&
	     // is male
	     ( gender_ethnic_group == NONMINORITY_MALE || gender_ethnic_group == MINORITY_MALE ) ) {
	    err_now("ofaculty.cpp: line 864: Faculty::name():");
	    //err_here();
	}
    }

    err_if( faculty_photo_res[photo_id]->gender_ethnic_group != this->gender_ethnic_group )
	err_here();

    return name_res.get_name_str( first_name_id, middle_name_id, last_name_id );
}

//------- End of function Faculty::name -------//

//------ Begin of function Faculty::name -------//
//!
char *Faculty::name(short firstNameId, short middleNameId, short lastNameId) {
    return name_res.get_name_str( firstNameId, middleNameId, lastNameId );
}

//------ End of function Faculty::name -------//

//------ Begin of function Faculty::get_quality_driver -------//
//!
float Faculty::get_quality_driver() {
    // 990412

    int i;
    const int VAR_COUNT = 4;
    float input[VAR_COUNT];

    float qualityDriver=0;
    Department* deptPtr = department_array[department_recno];

    input[0] = talent_research;
    input[1] = performance_research;
    input[2] = deptPtr->p_academic_standing;
    input[3] = school_res.player_peer_school->prestige;

    input[0] = math.dual_response_func(0, 48.9f, 100, 65.52f, 32.19f, 29.67f, 79.24f, input[0]);
    input[1] = math.dual_response_func(0, 49.3f, 100, 71.18f, 23.22f, 30.35f, 62.56f, input[1]);
    input[2] = math.dual_response_func(0, 49.72f, 100, 72.29f, 30.76f, 39.41f, 83.34f, input[2]);
    input[3] = math.dual_response_func(0, 49.8f, 100, 58.89f, 18.19f, 29.91f, 89.79f, input[3]);

    const float weight[VAR_COUNT] = { 0.3f, 0.3f, 0.25f, 0.15f };

    for (i=0; i<VAR_COUNT; i++)
	qualityDriver += weight[i] * input[i];

    return chance_event.research_adjust_multiplier * qualityDriver;
}

//------- End of function Faculty::get_quality_driver -------//
