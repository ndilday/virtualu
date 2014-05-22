//Filename		: OEnroll.cpp
//Description	: EnrollRes Class Definition
//Owner			: Fred

// ES library header file
#include <ALL.H>
#include <OGAMESET.H>
#include <OBOX.H>
#include <OSYS.H>

// CU header file
#include <GAMEDEF.H>                              // enum { NONMINORITY_MALE, NONMINORITY_FEMALE, MINORITY_MALE, MINORITY_FEMALE };
#include <ODEPTRES.H>
#include <OSCHLRES.H>
#include <OPEERSCH.H>

#include <OSTUDENT.H>
#include <OPSCHOOL.H>
#include <OFINANCE.H>
#include <ODEPT.H>
#include <OCHANCE.H>
#include <OMATH.H>

#include "OEnroll.h"

//----------- Define static functions -------------//
static bool is_minority_group(char group) {
    return group == MINORITY_MALE || group == MINORITY_FEMALE;
}

static bool is_male_group(char group) {
    return group == MINORITY_MALE || group == NONMINORITY_MALE;
}

//----------- Begin of function EnrollRes::update_enroll_history --------//
//!
//! update time-series graphs for interface display
//!
void EnrollRes::update_enroll_history(char noShift) {
    //-------------------------------//
    //		 calc vars for interface	//
    //-------------------------------//

    int j;

    int apps=0, offers=0, matrics=0;
    for (j=0; j<MAX_STUDENT_LEVEL; j++) {
	applications_rate[j] = ( total_matrics[j] ) ? float(total_applications[j]) / total_matrics[j] : 0;

	//## chea 081099 old idea
	//		cur_yield_rate[j] = ( total_offers[j] ) ? 1.0f-(float(total_matrics[j]) / total_offers[j]) : 0;
	//		cur_yield_rate[j] = ( total_offers[j] ) ? (float(total_matrics[j]) / total_applications[j]) : 0;
	//## chea 221099
	cur_yield_rate[j] = ( total_offers[j] ) ? (float(total_matrics[j]) / total_offers[j]) : 0;

	offers_rate[j] = ( total_applications[j] ) ? float(total_offers[j]) / total_applications[j] : 0;

	apps += total_applications[j];
	offers += total_offers[j];
	matrics += total_matrics[j];

	err_when(total_applications[j] < 0 || total_offers[j] < 0 || total_matrics[j] < 0);
    }

    // * 100 for interface display
    //

    overall_yield_rate = (offers) ? float(matrics)/offers * 100: 0;
    overall_offers_rate = (apps) ? float(offers)/apps * 100: 0;
    overall_apps_rate = (matrics) ? float(apps)/matrics * 100: 0;

  //------------ calc percent_minority ------------//

    char  subSeg, seg;
    int newStudentCount, totalMatrics = 0;
    char  sl = 0;
    int percentMinority[MAX_STUDENT_LEVEL];

    overall_percent_minority_matric = 0;

    memset(percentMinority, 0, sizeof(percentMinority));

  // sl1
    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	    newStudentCount = enroll_data_sl1[subSeg][seg].matrics;

	    // update vars for calc average later
	    if ( is_minority_group(subSeg) ) {
		overall_percent_minority_matric += newStudentCount;
		percentMinority[sl] += newStudentCount;
	    }

	    totalMatrics += newStudentCount;
	}
    }

    //sl25
    for ( sl=UG_NONTRADITION; sl<MAX_STUDENT_LEVEL; sl++) {
	char arrSL = sl-1;
	for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	    newStudentCount = enroll_data_sls[arrSL][subSeg].matrics;

	    // update vars for calc average later
	    if ( is_minority_group(subSeg) ) {
		overall_percent_minority_matric += newStudentCount;
		percentMinority[arrSL] += newStudentCount;
	    }

	    totalMatrics += newStudentCount;
	}
    }

    // calc vars for interface excusively
    overall_percent_minority_matric = (totalMatrics > 0) ? overall_percent_minority_matric * 100 / totalMatrics : 0;

    for ( sl=0; sl<MAX_STUDENT_LEVEL; sl++)
	percentMinority[sl] = (total_matrics[sl] > 0) ? percentMinority[sl] * 100 / total_matrics[sl] : 0;

  //------------ calculate talent ------------//

  // sl1-5 talent

    if( !noShift ) {                                // do not calculate if noShift is 1, when noShift is one, it is not a normall call, but an event call and we may not have the appropriate inputs to calculate it at this time
	talent_average[MAX_STUDENT_LEVEL_TOTAL][ACADEMIC] = 0;

	for ( sl=0; sl<MAX_STUDENT_LEVEL; sl++) {
	    talent_average[MAX_STUDENT_LEVEL_TOTAL][ACADEMIC]  += talent_average[sl][ACADEMIC];
	    talent_average[sl][ACADEMIC] = (total_matrics[sl] > 0) ? talent_average[sl][ACADEMIC] / total_matrics[sl] :0;
	}

	// sl1 talent

	sl = 0;

	if ( total_matrics[sl] > 0 ) {
	    talent_average[sl][EXTRA_CURRI] /= total_matrics[sl];
	    talent_average[sl][ATHLETIC] /= total_matrics[sl];
	}

	talent_average[MAX_STUDENT_LEVEL_TOTAL][ACADEMIC] = (totalMatrics > 0) ? talent_average[MAX_STUDENT_LEVEL_TOTAL][ACADEMIC] / totalMatrics : 0;
	talent_average[MAX_STUDENT_LEVEL_TOTAL][EXTRA_CURRI] = talent_average[sl][EXTRA_CURRI];
	talent_average[MAX_STUDENT_LEVEL_TOTAL][ATHLETIC] = talent_average[sl][ATHLETIC];
    }

    //-------------------------------//
    // calc vars for graph interface	//
    //-------------------------------//

    int h, var;

    if(!noShift) {
	// shift history to "left"
	//
	for ( sl=0; sl<MAX_STUDENT_LEVEL_N_TOTAL; sl++)
	    for ( var=0; var<HISTORY_GRAPH_COUNT_GENERAL; var++) {
		shift_history( matric_history[sl][var], HISTORY_YEAR_COUNT );
	    }
    }
    // update current year values
    //
    h = THIS_YEAR;
    sl = MAX_STUDENT_LEVEL;

    // history_value is int array
    matric_history[sl][ER_CREDITS_DEMANDED][h]  = (int) enroll_res.total_applications[DISTANCE_LEARN]*8;
    int totalDLProvided = 0;
    for ( int i=department_array.size(); i>0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;
	totalDLProvided += department_array[i]->distance_credits_provided;
    }
    matric_history[sl][ER_CREDITS_PROVIDED][h]  = (int) totalDLProvided;
    matric_history[sl][ER_YIELD_RATE][h]    = (int) overall_yield_rate;
    matric_history[sl][ER_OFFERS_RATE][h]   = (int) overall_offers_rate;
    matric_history[sl][ER_PERCENT_MINORITY][h]= (int) overall_percent_minority_matric;

    if(!noShift) {
	matric_history[sl][ER_ACADEMIC][h]      = talent_average[sl][ACADEMIC];
	matric_history[sl][ER_EXTRACURRICULAR][h] = talent_average[sl][EXTRA_CURRI];
	matric_history[sl][ER_ATHLETICS][h]     = talent_average[sl][ATHLETIC];
    }

    //----------------------------------//
    //		 calc vars for graph: sl1-5	//
    //----------------------------------//

    for ( sl=0; sl<MAX_STUDENT_LEVEL_TOTAL; sl++) {
	matric_history[sl][ER_CREDITS_DEMANDED][h]    = int(enroll_res.total_applications[DISTANCE_LEARN] * 8);
	matric_history[sl][ER_CREDITS_PROVIDED][h]    = int(totalDLProvided);
	matric_history[sl][ER_YIELD_RATE][h]    = int(cur_yield_rate[sl] * 100);
	matric_history[sl][ER_OFFERS_RATE][h]   = int(offers_rate[sl] * 100);
	matric_history[sl][ER_PERCENT_MINORITY][h]= percentMinority[sl];

	if(!noShift) {
	    matric_history[sl][ER_ACADEMIC][h]      = talent_average[sl][ACADEMIC];
	    matric_history[sl][ER_EXTRACURRICULAR][h] = talent_average[sl][EXTRA_CURRI];
	    matric_history[sl][ER_ATHLETICS][h]     = talent_average[sl][ATHLETIC];
	}
    }
}

//------------- End of function EnrollRes::update_enroll_history --------//

//------------- Begin of function EnrollRes::next_day --------//
void EnrollRes::next_day() {
    //---- update student intake once a year -----//

    if(info.game_day ==1 && info.game_month ==9)
	update_student_intake();

    //----  start of a trimester -------/

    if ( info.game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	 && info.game_month == player_school.trimester_array[player_school.cur_trimester].start_month ) {
	update_history(UPDATE_TRIMESTER);
    }

    //-------  start of a month ---------//

    if ( info.game_day == 1 ) {
	update_history(UPDATE_MONTH);

	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month ) {
	    update_history(UPDATE_YEAR);
	    //## chea 160699 for checking end of ban
	    if(chance_event.athlete_special_ban_year>0) chance_event.athlete_special_ban_year --;
	}
    }
}

//------------- End of function EnrollRes::next_day --------//

//------------- Begin of function EnrollRes::update_student_intake --------//
void EnrollRes::update_student_intake() {
    //---- for initialization -----//
    if(info.prerun_year == 1 && info.game_year == 1 )
	{
	    //## chea 040999 store the value for the last year target ribbon
	    for(int i=0; i<5 ; i++) {
		if( i !=3)
		    enroll_res.last_year_target_student_intake[i] = enroll_res.target_student_intake[i];
	    }

	    //## chea 060999 begin make the cal target_student_intake = lastyear student no - grad and drop
	}
	else {                                          //--- for normal running -------------//
	    //begin chea make actural student count
	    int i,j;
	    int thisYearArray[5] = {0,0,0,0,0};

	    StudentArray* stuArr;
	    Student *stuPtr;

	    err_when( department_array.size() == 0 );

	    for ( i=department_array.size(); i>=0; i-- ) {
		if (i) {
		    if ( department_array.is_deleted(i) )
			continue;

		    stuArr = &(department_array[i]->student_array);

		}
		else
		    stuArr = &(department_res.general_dept.student_array);

		// loop student

		for (j=stuArr->size(); j>0; j--) {
		    if ( stuArr->is_deleted(j) )
			continue;

		    sys.yield();
		    stuPtr = stuArr->operator[](j);

		    if(stuPtr->student_level == 0)
			thisYearArray[0]++;
		    if(stuPtr->student_level == 1)
			thisYearArray[1]++;
		    if(stuPtr->student_level == 2)
			thisYearArray[2]++;
		    //			if(stuPtr->student_level == 3)
		    //				thisYearArray[3]++;
		    if(stuPtr->student_level == 4)
			thisYearArray[4]++;

		}                                           // end for j
	    }
	    //end chea make actural student count

	    // BEGIN adjust the intake no.because of the enroll_res.total_applications
	    int adjustStuCount[5] = {0,0,0,0,0};
	    int count_over =0;
	    int t;

	    // calculate data for further calculation
	    int bachelorDegree=0, nonUgDegree=0, masterDegree=0, doctorDegree=0;
	    int dropoutLocal[DEGREE_TYPE_COUNT+1]={0,0,0,0};
	    int nonUgDropouts=0;

	    for ( i=1; i<=department_array.size(); i++ ) {
		if ( department_array.is_deleted(i) )
		    continue;

		Department* deptPtr = department_array[i];

		// get total degree numbers
		bachelorDegree += deptPtr->student_array.last_year_degree[0];
		nonUgDegree   += deptPtr->student_array.last_year_degree[1];
		masterDegree  += deptPtr->student_array.last_year_degree[2];
		doctorDegree  += deptPtr->student_array.last_year_degree[3];

		// get total dropout numbers
		for ( int j=0; j<4; j++ )
		    dropoutLocal[j] += deptPtr->student_array.last_year_dropout[j];
	    }

	    GeneralDepartment* genDept = &department_res.general_dept;

	    for ( j=0; j<4; j++ )
		dropoutLocal[j] += genDept->student_array.last_year_dropout[j];

	    // Target_intake(t) = target_enrollment - (actual_enrollment(t-1)-Grad(t)-Dropouts(t))
	    for (t=0; t<4; t++) {
		adjustStuCount[t] = enroll_res.target_enrollment[t];

		adjustStuCount[t] -= department_array.student_level_history[t][THIS_YEAR];

		switch (t) {
		case 0: adjustStuCount[t] += bachelorDegree;  break;
		case 1: adjustStuCount[t] += nonUgDegree;   break;
		case 2: adjustStuCount[t] += masterDegree;    break;
		case 3: adjustStuCount[t] += doctorDegree;    break;
		}

		adjustStuCount[t] += dropoutLocal[t];
	    }

	    // set it to 0 if negative
	    for (t=0; t<5; t++) {
		if ( adjustStuCount[t] <= 0 ) {
		    adjustStuCount[t] = 0;
		}
	    }

	    // END adjust the intake no.because of the enroll_res.total_applications

	    //## chea make actural student count
	    for (int h=0; h<5 ;h++) {
		enroll_res.target_student_intake[h] = adjustStuCount[h];
	    }

	    //## chea 040999 store the value for the last year target ribbon

	    for(int u=0; u<5 ; u++) {
		if( u !=3)
		    enroll_res.last_year_target_student_intake[u] = enroll_res.target_student_intake[u];
	    }
	    //## chea 060999 begin make the cal target_student_intake = lastyear student no - grad and drop
	}
}

//------------- End of function EnrollRes::update_student_intake --------//

//---------- Begin of function Student::update_history -----------//
//!
//! update history arrays
//!
//! update_flag		= UPDATE_TRIMESTER, UPDATE_MONTH, UPDATE_YEAR; (ooffice.h)
//!
void EnrollRes::update_history(char update_flag) {
    switch (update_flag) {
    case UPDATE_MONTH:
	break;

    case UPDATE_TRIMESTER:
	calc_student_count();
	break;

    case UPDATE_YEAR:
	update_income();
	sys.yield();
	enroll_main();                              // ***** // which will call update_enroll_history()
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

//---------- End of function Student::update_history -----------//

//------------- Begin of function EnrollRes::calc_student_count --------//
void EnrollRes::calc_student_count(int initFlag) {
    total_student_count = 0;
    memset(student_count,0,sizeof(student_count));
    memset(student_female_percent,0,sizeof(student_female_percent));
    memset(student_minority_percent,0,sizeof(student_minority_percent));
    memset(student_minority_female_percent, 0, sizeof(student_minority_female_percent));
    memset(overall_gender_ethnic_group_student_count, 0, sizeof(overall_gender_ethnic_group_student_count));

    int i,j;
    StudentArray* stuArr;
    Student *stuPtr;
    char gender, sl;

    err_when( department_array.size() == 0 );
    for ( i=department_array.size(); i>=0; i-- ) {

	if (i) {
	    if ( department_array.is_deleted(i) )
		continue;

	    stuArr = &(department_array[i]->student_array);
	}
	else
	    stuArr = &(department_res.general_dept.student_array);

	// loop student
	for (j=stuArr->size(); j>0; j--) {
	    if ( stuArr->is_deleted(j) )
		continue;

	    sys.yield();

	    stuPtr = stuArr->operator[](j);

	    gender = stuPtr->gender_ethnic_group;
	    sl = stuPtr->student_level;

	    err_when(UG_TRADITION!=0 || sl < UG_TRADITION || sl > DISTANCE_LEARN);

	    student_count[sl]++;

	    switch ( gender ) {
	    case NONMINORITY_MALE:
		break;
	    case NONMINORITY_FEMALE:
		student_female_percent[sl] ++;
		break;
	    case MINORITY_MALE:
		student_minority_percent[sl] ++;
		break;
	    case MINORITY_FEMALE:
		student_female_percent[sl] ++;
		student_minority_percent[sl] ++;
		student_minority_female_percent[sl]++;
		break;
	    default:
		err_here();
		break;
	    }

	    overall_gender_ethnic_group_student_count[gender]++;
	}                                             // end for j
    }

    for (i=0; i<MAX_STUDENT_LEVEL; i++)
	total_student_count += student_count[i];

    err_when(total_student_count == 0);

    //--------//
    overall_percent_female = 0;
    overall_percent_minority = 0;
    overall_percent_minority_female = 0;

    for (i=0; i<MAX_STUDENT_LEVEL; i++) {
	overall_percent_female += int(student_female_percent[i]);
	overall_percent_minority += int(student_minority_percent[i]);
	overall_percent_minority_female += int(student_minority_female_percent[i]);
    }

    overall_percent_female = int(overall_percent_female * 100.0f / total_student_count);
    overall_percent_minority = int(overall_percent_minority * 100.0f / total_student_count);
    overall_percent_minority_female = int(overall_percent_minority_female * 100.0f / total_student_count);

    //--------//
    // Since version 2.0 no Distance Learning
    for (i=0; i<MAX_STUDENT_LEVEL-1; i++)
	{
	    student_female_percent[i] = math.safe_divide( 100.0f * student_female_percent[i], (float)student_count[i] );
	    student_minority_percent[i] = math.safe_divide( 100.0f * student_minority_percent[i], (float)student_count[i] );
	    student_minority_female_percent[i] = math.safe_divide( 100.0f * student_minority_female_percent[i], (float)student_count[i] );
	}
}

//------------- End of function EnrollRes::calc_student_count --------//

//------------- Begin of function EnrollRes::calc_total_aid --------//
//!
void EnrollRes:: calc_total_aid() {
    //-------------- fin aid for old student------------------------//

    int i,j, totalAidNewStu=0;
    StudentArray* stuArr;
    Student *stuPtr;

    err_when( department_array.size() == 0 );
    for ( i=department_array.size(); i>=0; i-- ) {
	if (i) {
	    if ( department_array.is_deleted(i) )
		continue;

	    stuArr = &(department_array[i]->student_array);
	}
	else
	    stuArr = &(department_res.general_dept.student_array);

	// loop student
	for (j=stuArr->size(); j>0; j--) {
	    if ( stuArr->is_deleted(j) )
		continue;

	    stuPtr = stuArr->operator[](j);

	    stuPtr->financial_aid = int(1000*calc_an_aid(stuPtr));

	    if ( stuPtr->year_in_program == 1 )
		totalAidNewStu += stuPtr->financial_aid / 1000;
	}
    }

    err_when( total_aid == totalAidNewStu );        // adjustRatio == 0

    //--------------------------------------//

    // recalc the aids of all *old* student sims => recalc total_aid
    // refer to last paragraphs in td3.4 section 2.7

    // have to adjust for returning but not newly-entered sims
    // adjust aid for all sims; refer to td3.4 section 4

    float budget = (float) school_res.player_peer_school->financial_aid;

    if ( !aid_budget_enforced || budget > total_aid )
	return;

    float adjustRatio = float( budget - totalAidNewStu ) / (total_aid - totalAidNewStu);

    total_aid = budget;

    if ( adjustRatio < 0 )
	adjustRatio = 0;                              // aid = 0 for all returning sims //askbill if budget - totalAidNewStu < 0

    err_when( adjustRatio > 1 );

    //--------------------------------------//

    memset(fraction_on_aid,0,sizeof(fraction_on_aid));
    memset(average_aid,0,sizeof(average_aid));
    int studentGotAid = 0;

    for ( i=department_array.size(); i>=0; i-- ) {
	if (i) {
	    if ( department_array.is_deleted(i) )
		continue;

	    stuArr = &(department_array[i]->student_array);
	}
	else
	    stuArr = &(department_res.general_dept.student_array);

	// loop student
	for (j=stuArr->size(); j>0; j--) {
	    if ( stuArr->is_deleted(j) )
		continue;

	    stuPtr = stuArr->operator[](j);

	    // job 1 financial_aid

	    if ( stuPtr->year_in_program != 1 )
		//TO add some penalty for morale also!?
		stuPtr->financial_aid = int(stuPtr->financial_aid*adjustRatio);

	    // job 2 financial_aid_statistics

	    if ( stuPtr->financial_aid > 0 ) {
		char minor;

		minor = ( is_minority_group(stuPtr->gender_ethnic_group) ) ? MINORITY : NON_MINORITY;

		fraction_on_aid[minor]++;
		average_aid[minor]++;
		studentGotAid++;
	    }
	}
    }

    //--------------------------------------//

    for (i=0; i<MINORITY_GROUP_COUNT; i++) {
	fraction_on_aid[i] /= total_student_count;
	if ( studentGotAid)
	    average_aid[i] /= studentGotAid;
    }
}

//------------- End of function EnrollRes::calc_total_aid --------//
