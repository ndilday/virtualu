//Filename		: OEnrolsl.cpp
//Description	: EnrollRes Class Definition
//						- handle enrollment of sl-2 to sl-5 (NONTRADTION to DISTANCE_LEARN)
//						- calc aid and talent for all levels
//Owner			: Fred

#include <stdio.h>                                // for DEBUG

// ES library header file
#include <ALL.H>
#include <OGAMESET.H>
#include <OMATH.H>                                // average_float, gammaCDF
#include <OSTR.h>                                 // strcmp
#include <OLINALG.H>                              // for this calc_enroll_offers

// CU header file
#include <GAMEDEF.H>                              // enum { NONMINORITY_MALE, NONMINORITY_FEMALE, MINORITY_MALE, MINORITY_FEMALE };
#include <OPSCHOOL.H>
#include <OPEERSCH.H>
#include <OSCHLRES.H>
#include <ODEPTRES.H>
#include <ODEPT.H>
#include <OLIBTECH.H>                             //## chea 270999

#include "OEnroll.h"

//----------- Define helper static functions ------//

static inline bool is_minority_group(char group) {
    return group == MINORITY_MALE || group == MINORITY_FEMALE;
}

static float multipler_special_ug(char sl, char subSeg) {
    err_when(sl==UG_TRADITION);
    float avg = is_minority_group(subSeg) ? 0.8f : 0.9f;

    return is_nontradition_ug_group(sl) ? math.get_random_snd(avg, PSCH_SD(0.05f)) : 1.0f;
}

//----------- Begin of function EnrollRes::load_db_student_pref2 ------//
//!
//! refer to [HE.STU.pref]sheet1
//!
void EnrollRes::init_data_sls(char sl) {
    char arrSL = sl-1;                              // for array access
    err_when(UG_NONTRADITION != 1);

    //----------------------------------------------------//
    //-- initialization: td3.4 section 3.2					---//
    //----------------------------------------------------//

    //---- STEP 1. calc enroll_data_sls[][].student_pref -----//

    const float STU_PREF_SCALE = 4.0f;

    float prefVarsArray[PREFERECNE_COUNT2];         // column Q
    int i, j;

    PeerSchool* player = school_res.player_peer_school;
    memcpy(prefVarsArray, player->pref_vars_array, sizeof(prefVarsArray));

    for (i=0; i<PREFERECNE_COUNT2; i++) {
	if ( i < PREFERECNE_COUNT2 - 4 )
	    prefVarsArray[i] = math.safe_divide(prefVarsArray[i], PeerSchool::pref_vars_average_array[i]);
	else {
	    if (  ( sl == UG_NONTRADITION && fraction_sl2_offered_aid > 0 )
		  || ( sl == DISTANCE_LEARN && fraction_sl5_offered_aid > 0 )
		)
		prefVarsArray[i] = math.safe_divide(player->pref_vars_array_last[i], PeerSchool::pref_vars_average_array_last[i]);
	    else
		prefVarsArray[i] = 0;
	}
    }

    err_when(PUBLIC != 1);

    char  isPublic = player_school.control == PUBLIC;
    float finalRatio[MINORITY_GROUP_COUNT];         // stuPrefs
    float tmpRatio;
    float tmp, sign;

    for (j=0; j<MINORITY_GROUP_COUNT; j++) {
	finalRatio[j] = 1;

	for (i=0; i<PREFERECNE_COUNT2; i++) {
	    sign = (pref_special_case(i))? -1.0f: 1.0f;

	    // 1
	    tmp = enroll_res.preference_array2_sls[arrSL][isPublic][j][i];

	    if ( tmp <= 0.0 || prefVarsArray[i] == 0)
		tmpRatio = 1.0f;
	    else
		tmpRatio = math.safe_pow(prefVarsArray[i], sign * tmp);

	    if ( arrSL == 3 ) {                         // special setting for Distance Learning
		//				tmpRatio = math.safe_pow(		Kenneth
	    }

	    // 2
	    finalRatio[j] *= tmpRatio;
	    err_when( finalRatio[j] < 0 );
	}

	if ( !(1+ (float) exp(-s_slope*(finalRatio[j]-1))) )
	    finalRatio[j] = 1;
	else
	    finalRatio[j] = s_lower_bound+(s_upper_bound-s_lower_bound)/(1+ (float) exp(-s_slope*(finalRatio[j]-1)));

	//---- save the student preference for future use ----//

	if ( j == MINORITY ) {
	    enroll_data_sls[arrSL][MINORITY_MALE].student_pref = finalRatio[j];
	    enroll_data_sls[arrSL][MINORITY_FEMALE].student_pref = finalRatio[j];
	}
	else {
	    enroll_data_sls[arrSL][NONMINORITY_MALE].student_pref = finalRatio[j];
	    enroll_data_sls[arrSL][NONMINORITY_FEMALE].student_pref = finalRatio[j];
	}

	err_when( finalRatio[j] < 0 );
    }

    //-------//
    EnrollDataSL1* ed;
    EnrollData* edSL;
    char subSeg, seg;
    // 0.3 -> 30%
    float studentGenderPct[STUDENT_SUBSEGMENT_COUNT]; {

	err_when(GENDER_ETHNIC_TYPE_COUNT != STUDENT_SUBSEGMENT_COUNT);
	float femalePercent = student_female_percent[sl] / 100.0f;
	float minorityPercent = student_minority_percent[sl] / 100.0f;

	//askbill inital gender percent affect future enrollment?  BUG? = 0 when sl=5
	studentGenderPct[MINORITY_FEMALE] = femalePercent*minorityPercent;
	studentGenderPct[NONMINORITY_FEMALE] = femalePercent*(1.0f - minorityPercent);
	studentGenderPct[MINORITY_MALE] = (1.0f - femalePercent)*minorityPercent;
	studentGenderPct[NONMINORITY_MALE] = (1.0f - femalePercent)*(1.0f - minorityPercent);

	for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ )
	    enroll_data_sls[arrSL][subSeg].fraction_matrics = studentGenderPct[subSeg];
    }

    //---- STEP 2 -----//

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	float appTmp, totalFraction;
	Talent acaTmp;

	for ( seg=0, appTmp=0, acaTmp.average=0, totalFraction=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	    ed = &enroll_data_sl1[subSeg][seg];

	    appTmp += ed->apps_ratio * ed->fraction_matrics;
	    acaTmp.average += talent_array[subSeg][seg][ACADEMIC].average * ed->fraction_matrics;

	    totalFraction += ed->fraction_matrics;
	}

	//--------- calc enroll_data_sls[][].apps_ratio, applications and conversion_rate ---------//
	edSL = &enroll_data_sls[arrSL][subSeg];

	edSL->apps_ratio
	    //BUGHERE
	    = math.safe_pow(edSL->student_pref, 1.0f) * appTmp / totalFraction
	    //			= math.safe_pow(edSL->student_pref, STU_PREF_SCALE) * appTmp / totalFraction
	    ;                                           //+ math.get_random_snd(0, 0.1f * enroll_data_sls[sl][subSeg].apps_ratio);//askbill

	edSL->applications
	    = enroll_data_sls[arrSL][subSeg].base_apps
	    = int(edSL->apps_ratio
		  // 0.5: rounding
		  * studentGenderPct[subSeg] * target_student_intake[sl] + 0.5);

	edSL->conversion_rate
	    = enroll_data_sls[arrSL][subSeg].base_conversion_rate
	    //990514
	    = m.fmin(1.0f, math.safe_divide(1.0f, edSL->apps_ratio));

	if ( sl == DOCTOR )                           // 990514
	    edSL->conversion_rate =
		//min & max bug chea (max)
		m.fmin(m.fmax(0.0f,math.get_random_snd(0.5f,PSCH_SD(0.07f))), math.safe_divide(1.0f, edSL->apps_ratio));

	err_if ( sl == DOCTOR && edSL->conversion_rate > 0.99 )
	    err_here();                                 //int x=0;

	//--------- calc enroll_data_sls[][].academic ratings: average and variance ---------//

	// means[sL,geg],variances[sL,geg]

	edSL->academic.average
	    = ( multipler_special_ug(sl, subSeg) )
	    * math.safe_pow(edSL->student_pref, STU_PREF_SCALE) * acaTmp.average / totalFraction
	    ;                                           //+ math.get_random_snd(0, 0.1f * enroll_data_sls[sl][subSeg].academic.average);//askbill

	if ( sl == UG_NONTRADITION ) {
	    for ( seg=0, acaTmp.variance = 0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
		ed = &enroll_data_sl1[subSeg][seg];

		acaTmp.variance +=
		    ( talent_array[subSeg][seg][ACADEMIC].variance
		      + (float) math.safe_pow( multipler_special_ug(sl, subSeg) * talent_array[subSeg][seg][ACADEMIC].average
					       - enroll_data_sls[arrSL][subSeg].academic.average, 2)
			) * ed->fraction_matrics;
	    }

	    edSL->academic.variance
		= acaTmp.variance / totalFraction;
	}
	else
	    edSL->academic.variance = enroll_data_sls[UG_NONTRADITION-1][subSeg].academic.variance;

	err_when(edSL->applications < 0);
	err_when(edSL->academic.average < 0);
    }

    //-----------------//

    calc_enroll_aid_sls(sl);
}

//------------- End of function EnrollRes::load_db_student_pref2 ------//

//----------- Begin of function EnrollRes::calc_enroll_aid_sls ------//
//!
//!	(* Needs to be run whenever Player changes any aid parameter *)
//!
void EnrollRes::calc_enroll_aid_sls(char sl) {
    float fractionOfferedMeritAid = 0;

    if (  ( sl == UG_NONTRADITION ) ) {
	adjust_fraction_merit_aid(fraction_sl2_offered_aid);

	if ( fraction_sl2_offered_aid <= 0 ) {
	    merit_lower[sl] = 1;
	    merit_upper[sl] = 0;
	    return;
	}

	fractionOfferedMeritAid = fraction_sl2_offered_aid;
    }
    else if ( sl == DISTANCE_LEARN ) {
	adjust_fraction_merit_aid(fraction_sl5_offered_aid);

	if ( fraction_sl5_offered_aid <= 0 ) {
	    merit_lower[sl] = 1;
	    merit_upper[sl] = 0;
	    return;
	}

	fractionOfferedMeritAid = fraction_sl5_offered_aid;
    }
    else if ( sl == DOCTOR ) {
	merit_lower[sl] = max_aid;
	merit_upper[sl] = max_aid;
	return;
    }
    else {
	merit_lower[sl] = 1;
	merit_upper[sl] = 0;
	return;
    }

    //-------------------//
    //---- Merit Aid ----//
    //-------------------//

    /*

      mL2 = With[{tmp = frNonTradStudentsOfferedAid*
      Plus@@((conversionRate[sL2,#]*frPriorMatrics[sL2,#])&/@gegs)},
      FindMinimum[(meritFrObj[sL2,x]-tmp)^2,{x,5}][[2,1,2]]];
    */

    float adjustRatio=0;
    char subSeg, arrSL = sl-1;

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	adjustRatio
	    += enroll_data_sls[arrSL][subSeg].conversion_rate
	    * enroll_data_sls[arrSL][subSeg].fraction_matrics;
    }

    float x = 5, deviate, lastDeviate;
    float inc = x / 100.f;
    float offset = fractionOfferedMeritAid * adjustRatio;

    if ( calc_merit_aid_sls(sl, x+inc, offset)
	 > calc_merit_aid_sls(sl, x-inc, offset) )
	inc *= -1;

    deviate = calc_merit_aid_sls(sl, x, offset);

    do {
	x += inc;
	lastDeviate = deviate;
	deviate = calc_merit_aid_sls(sl, x, offset);
    }
    while ( deviate < lastDeviate && deviate > 0 );

    merit_lower[sl] = x;

    /*
      mH2 =  With[{tmp = frMeritTop*
      Plus@@((conversionRate[sL2,#]*frPriorMatrics[sL2,#])&/@gegs)},
      FindMinimum[(meritFrObj[sL2,x]-tmp)^2,{x,6}][[2,1,2]]],
    */

    x = 6;
    inc = x / 100.f;
    offset = fraction_merit_top * adjustRatio;

    if ( calc_merit_aid_sls(sl, x+inc, offset)
	 > calc_merit_aid_sls(sl, x-inc, offset) )
	inc *= -1;

    deviate = calc_merit_aid_sls(sl, x, offset);

    do {
	x += inc;
	lastDeviate = deviate;
	deviate = calc_merit_aid_sls(sl, x, offset);
    }
    while ( deviate < lastDeviate && deviate > 0 );

    merit_upper[sl] = x;
}

//------------- End of function EnrollRes::calc_enroll_aid_sls ------//

//----------- Begin of function EnrollRes::calc_merit_aid_sls --------//
//!
//! Refer to techdoc 3.4 section 2.4
//! and the mathca file HE.EnrMgt.rev.nb: "Traditional Undergraduates: sL-1: Financial Aid: Initialization"
//!
float EnrollRes::calc_merit_aid_sls(char sl, float aid, float base) {
    float deviate = 0, cdfTmp;

    float aidMinMultiplier;
    char  subSeg, arrSL=sl-1;
    EnrollData  *ed;

    // meritFrObj[sL2,mL_] := Plus@@Flatten[meritFrObj[sL2,mL,#]&/@gegs];
    // meritFrObj[sL2,mL_,geg_] := (1-cdf[acadR,sL2,geg,mL])*frPriorMatrics[sL2,geg];

    // cdf[acadR,sL_,geg_,x_] := gammaCDF[minMultF[aid,geg]*means[sL,geg],variances[sL,geg],x];

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	if ( !is_minority_group(subSeg) )
	    aidMinMultiplier = 1.0f;
	else
	    aidMinMultiplier = aidMinMultipliers[minority_aid_special];

	ed = &enroll_data_sls[arrSL][subSeg];

	cdfTmp = math.get_random_gamma_CDF( aidMinMultiplier * ed->academic.average,
					    PSCH_SD(ed->academic.variance), aid );

	deviate += (1-cdfTmp) * ed->fraction_matrics;
    }

    return math.safe_pow(deviate - base, 2);
}

//----------- End of function EnrollRes::calc_merit_aid_sls --------//

//----------- Begin of function EnrollRes::enroll_main_sls ------//
//!
//! called yearly for sl2-5
//!
void EnrollRes::enroll_main_sls(char sl) {
    //----------------------------------------------------//
    //-- main procedures: td3.4 section 3.3				---//
    //----------------------------------------------------//

    /*
      getApps[sL_] := ((applications[sL,#] =
      Round[(1-matsLatency[sL])*stuPrefs[sL,#]*baseApps[sL,#] +
      matsLatency[sL]*applications[sL,#]])&/@gegs;

      totalApps[sL] = Plus@@Flatten[applications[sL,#]&/@gegs]);
    */

    char  subSeg, arrSL=sl-1;
    EnrollData  *edSL;
    float idealMatrics = 0;
    float totalMatrics = 0;

    total_applications[sl] = 0;

    //## chea 110899 avoid 0 Matrics

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	if(enroll_data_sls[arrSL][subSeg].conversion_rate <= 0 && target_student_intake[sl] > 0) {
	    enroll_data_sls[arrSL][subSeg].conversion_rate=1;
	}
    }

    /*  // this is the org. idea
	for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ )
	{
	edSL = &enroll_data_sls[arrSL][subSeg];

	edSL->applications = int( 0.5 + math.get_random_snd(chance_avg, chance_sd, true)
        + (1-apps_latency[sl]) * app_multiplier[arrSL] * edSL->student_pref * edSL->base_apps
        + apps_latency[sl] * edSL->applications );

	total_applications[sl] += edSL->applications;
	idealMatrics += edSL->applications * edSL->conversion_rate;

	err_when(edSL->applications < 0);
	}
    */

    //## chea 270999 my idea
    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	edSL = &enroll_data_sls[arrSL][subSeg];

	if(sl ==2 || sl ==3 || sl ==1) {
	    edSL->applications = int( 0.5 + math.get_random_snd(chance_avg, PSCH_SD(chance_sd), true)
				      + (1-apps_latency[sl]) * app_multiplier[arrSL] * edSL->student_pref * edSL->base_apps
				      + apps_latency[sl] * edSL->applications );
	}
	/*
          else if(sl==4 && player_school.school_type == 1 && info.prerun_year == 1)
	  {
          edSL->applications = 10;
	  }
	*/
	else {
	    edSL->applications = int( 0.5 + math.get_random_snd(chance_avg, PSCH_SD(chance_sd), true)
				      + (1-apps_latency[sl]) * app_multiplier[arrSL] * edSL->student_pref * edSL->base_apps
				      + apps_latency[sl] * edSL->applications);
	}

	total_applications[sl] += edSL->applications;
	totalMatrics += edSL->applications * edSL->conversion_rate;

	err_when(edSL->applications < 0);
    }

    idealMatrics = math.safe_divide( (float) target_student_intake[sl], totalMatrics);

    // no need bound by 1

    //----------------------------------------------------//

    //if ( sl == DOCTOR && idealMatrics <= 0 )
    //	err_here();	//int x=0;

    total_matrics[sl] = 0;

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	edSL = &enroll_data_sls[arrSL][subSeg];

	edSL->matrics = int ( 0.5 + min(
	    //min & max bug chea
	    (float)(edSL->applications), idealMatrics * edSL->conversion_rate * edSL->applications ));

	total_matrics[sl] += edSL->matrics;

	err_when(edSL->matrics < 0);
	//if ( sl == DOCTOR && edSL->conversion_rate > 0.99 )
	//	err_here();	//int x=0;
    }

    // Item 8d i)
    if ( sl == 4 ) {
	int input1;
	switch ( distance_learner_acceptance ) {
	case 0: input1 = 0;             break;
	case 1: input1 = 8 * edSL->applications;  break;
	case 2: input1 = 16 * edSL->applications;
	}

	total_matrics[sl] = player_school.latency_func( 0.7f, total_matrics[sl], input1 );
    }

    //	if ( idealMatrics >= total_matrics[sl] )
    //		err_here();	//int x=0;

    //----------------------------------------------------//

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	edSL = &enroll_data_sls[arrSL][subSeg];

	edSL->fraction_matrics = math.safe_divide((edSL->matrics), total_matrics[sl]);
	edSL->conversion_rate = math.safe_divide((edSL->matrics), edSL->applications);

	err_when(edSL->conversion_rate < 0 || edSL->conversion_rate > 1);
    }

    //----------------------------------------------------//
    // calc getMinAcadRating

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	edSL = &enroll_data_sls[arrSL][subSeg];

	float x = 5, deviate, lastDeviate;
	float inc = x / 100.f;
	float offset = 1 - edSL->conversion_rate;

	if ( calc_academic_rating(sl, subSeg, x+inc, offset)
	     > calc_academic_rating(sl, subSeg, x-inc, offset) )
	    inc *= -1;

	deviate = calc_academic_rating(sl, subSeg, x, offset);

	do {
	    x += inc;
	    lastDeviate = deviate;
	    deviate = calc_academic_rating(sl, subSeg, x, offset);
	}
	while ( deviate < lastDeviate && deviate > 0 );

	min_academic_rating[arrSL] = min(x, 10.0f);   //min & max bug chea
    }

    /*
      getMinAcadRating[sL_,geg_,y_] := FindMinimum[(cdf[acadR,sL,geg,x]-(1-y))^2,{x,5}][[2,1,2]];

      getAcadRatings[sL_] := (
      (minAcadRating[sL,#] = Min[getMinAcadRating[sL,#,conversionRate[sL,#]],10])&/@gegs;
      minAcadRating[sL]   = Min[minAcadRating[sL,#]&/@gegs];	)
    */
}

//------------- End of function EnrollRes::enroll_main_sls ------//

//----------- Begin of function EnrollRes::calc_academic_rating --------//
//!
//! helper method for calc min_academic_rating[] above
//!
float EnrollRes::calc_academic_rating(char sl, char subSeg, float academicRating, float base) {
    float deviate = 0, cdfTmp, aidMinMultiplier;

    char  arrSL=sl-1;
    EnrollData  *ed;

    // cdf[acadR,sL_,geg_,x_] := gammaCDF[minMultF[aid,geg]*means[sL,geg],variances[sL,geg],x];

    if ( !is_minority_group(subSeg) )
	aidMinMultiplier = 1.0f;
    else
	aidMinMultiplier = aidMinMultipliers[minority_aid_special];

    ed = &enroll_data_sls[arrSL][subSeg];

    cdfTmp = math.get_random_gamma_CDF( aidMinMultiplier * ed->academic.average,
					PSCH_SD(ed->academic.variance), academicRating);

    deviate = cdfTmp;

    return math.safe_pow(deviate - base, 2);
}

//----------- End of function EnrollRes::calc_academic_rating --------//

//----------- Begin of function EnrollRes::calc_post_enroll_sls --------//
//!
//! refer to td3.4 section 3.5
//!
void EnrollRes::calc_post_enroll_sls(char sl) {
    //-------------------------//
    //		 generate student		//
    //-------------------------//

    EnrollData *edSL;
    char  subSeg, seg=-1;
    int newStudentCount, i, arrSL=sl-1;

    // temp vars storing new sim properties
    float talentArr[TALENT_VAR_COUNT];
    int aid=0;

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	edSL = &enroll_data_sls[arrSL][subSeg];
	newStudentCount = edSL->matrics;

	for (i=0; i<newStudentCount; i++) {
	    char subSegStu = subSeg;

	    if ( info.year_passed == 1 && player_school.scenario_id == SCN_STUDENT_DIVERSITY ) {
		if ( is_minority_group(subSeg) && math.get_random_float () > 0.571f ) {
		    if (subSegStu == MINORITY_MALE )
			subSegStu = NONMINORITY_MALE;
		    else
			subSegStu = NONMINORITY_FEMALE;
		}
	    }

	    //------- calc 3 talent values and aid

	    calc_a_talent(talentArr, sl, subSegStu);

	    // 990408
	    /*if ( sl == DOCTOR && talentArr[ACADEMIC] < min_academic_rating[arrSL] )
	      {
	      // if acadRating < minAcadRating, then discard this applicant
	      edSL->matrics--;
	      continue;
	      }*/

	    //-------  Generate student; see also PlayerSchool::generate_student()
	    //

	    if ( is_nontradition_ug_group(sl) ) {
		// all year 1 ug students start without major
		department_res.general_dept.student_array.add( sl, 1, subSegStu, seg, 0, int(aid), talentArr );
	    }
	    else {
		int major = player_school.get_random_major(sl);
		department_array[major]->student_array.add( sl, 1, subSegStu, seg, major, int(aid), talentArr );
	    }

	    //-------  update vars for calc average later

	    talent_average[sl][ACADEMIC] += int( 10 * talentArr[ACADEMIC]);
	}
    }

    //----------------------------------------------------//

    // recalc matrics since some are discarded above

    //----------------------------------------------------//

    if ( sl != 4 ) {                                // Not distance learning
	total_matrics[sl] = 0;

	for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	    edSL = &enroll_data_sls[arrSL][subSeg];

	    total_matrics[sl] += edSL->matrics;

	    err_when(edSL->matrics < 0);
	}
    }
    //----------------------------------------------------//

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	edSL = &enroll_data_sls[arrSL][subSeg];

	edSL->fraction_matrics = math.safe_divide((edSL->matrics), total_matrics[sl]);
	edSL->conversion_rate = math.safe_divide((edSL->matrics), edSL->applications);

	err_when(edSL->conversion_rate < 0 || edSL->conversion_rate > 1);
    }

    //----------------------------------------------------//
    //990408

    int subOffer = 0;

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	edSL = &enroll_data_sls[arrSL][subSeg];

	subOffer += int( edSL->matrics + (edSL->applications - edSL->matrics ) *
			 m.fmin(1.0f, m.fmax(0.0f,math.get_random_snd(edSL->conversion_rate,
								      PSCH_SD(edSL->conversion_rate * 0.1f)))) ); //min & max bug chea
    }

    //total_offers[sl] = int(total_matrics[sl] * 1.4f);
    total_offers[sl] = subOffer;
}

//----------- End of function EnrollRes::calc_post_enroll_sls --------//

//----------- Begin of function EnrollRes::calc_an_aid --------//
//!
//! note: all income, utility, lower and upper aids in enrollment are adjusted to $1000.
//! The returned value is also in $1000 format!
//!
float EnrollRes::calc_an_aid(Student *stuPtr) {
    char sl = stuPtr->student_level;
    char subSeg = stuPtr->gender_ethnic_group;
    char seg = stuPtr->student_segment;

    //------- calc need and merit aid
    EnrollDataSL1 *ed;
    Talent *talentPtr;
    float utility, needAid, meritAid, income;
    float aid;

    if ( sl == 0 ) {
	err_when(seg < 0 || seg >= STUDENT_SEGMENT_COUNT);

	ed = &enroll_data_sl1[subSeg][seg];
	talentPtr = &(talent_array[subSeg][seg][ACADEMIC]);

	// ## chea 100899 use small variance to change aid
	//		income = math.get_random_snd(talentPtr[INCOME].average, float(sqrt(talentPtr[INCOME].variance)), true );
	//		utility = math.get_random_snd(ed->util, float(sqrt(ed->util_variance)), true );

	income = math.get_random_snd(talentPtr[INCOME].average, PSCH_SD(float(sqrt(talentPtr[INCOME].variance))/4.0f), true );
	utility = math.get_random_snd(ed->util, PSCH_SD(float(sqrt(ed->util_variance))/4.0f), true );

	// adjust utility

	if ( seg == STUDENT_SEGMENT_ATHLETE ) {
	    switch (athlete_offers_special) {
	    case SET_HIGH:
		utility = max(utility,10.0f);           // min & max bug chea
		break;
	    case SET_MID:
		utility = ( 10 + utility ) / 2;
		break;
	    default:
		break;
	    }
	}

	//---- calc need and merit aid ---------//

	if ( income < need_lower )
	    needAid = fraction_need_covered * max_aid;
	else if ( income < need_upper )
	    needAid = fraction_need_covered * max_aid * (need_upper-income) / (need_upper - need_lower);
	else
	    needAid = 0;

	if ( fraction_sl1_offered_merit_aid <= 0 )
	    merit_upper[sl] = 10;

	if ( utility > merit_upper[sl] )
	    meritAid = max_aid - needAid;
	else if ( utility > merit_lower[sl] )
	    // min & max bug chea
	    meritAid = max( max_aid * float(utility - merit_lower[sl]) / (merit_upper[sl] - merit_lower[sl]) - needAid, 0.0f );
	else
	    meritAid = 0;

	//meritAid = 0;		//990408
	err_when(need_upper == need_lower);
	err_when(merit_upper[sl] == merit_lower[sl]);

	//------------------ calc final aid

	err_when(needAid + meritAid > max_aid + 0.15);

	aid = ((needAid + meritAid));

	if ( seg == STUDENT_SEGMENT_ATHLETE ) {
	    switch (athlete_aid_special) {
	    case SET_HIGH:
		aid = ( max_aid);
		break;
	    case SET_MID:
		aid = ( (max_aid+aid) / 2.0f);
		break;
	    default:
		break;
	    }
	}
    }

    //-------- sl2-5 -----------//

    else {
	//--- mertiAid only
	// utility = academic rating in sl2-5
	utility = float(stuPtr->talent_academic) / 10;

	if ( sl == DOCTOR )
	    meritAid = max_aid;
	else if ( merit_upper[sl] < merit_lower[sl] )
	    meritAid = 0;
	else if ( utility > merit_upper[sl] )
	    meritAid = max_aid;                         //0223
	else if ( utility > merit_lower[sl] )
	    // min & max bug chea
	    meritAid = max( max_aid * float(utility - merit_lower[sl]) / (merit_upper[sl] - merit_lower[sl]), 0.0f );
	else
	    meritAid = 0;

	aid = (meritAid);
    }

    err_when(aid > (max_aid + 0.10));
    total_aid += aid;

    return aid;
}

//----------- End of function EnrollRes::calc_an_aid --------//

//----------- Begin of function EnrollRes::calc_a_talent --------//
//!
//!
void EnrollRes::calc_a_talent(float talentArr[TALENT_VAR_COUNT], char sl, char subSeg, char seg) {
    err_when(subSeg < 0 || subSeg >= STUDENT_SUBSEGMENT_COUNT);

    if ( sl == UG_TRADITION ) {
	err_when(seg < 0 || seg >= STUDENT_SEGMENT_COUNT);

	Talent *talentPtr;
	talentPtr = &(talent_array[subSeg][seg][ACADEMIC]);

	for (char t=0; t<TALENT_VAR_COUNT; t++)
	    talentArr[t] = math.get_random_snd(talentPtr[t].average, PSCH_SD(float(sqrt(talentPtr[t].variance))), true );
    }
    else {
	char arrSL = sl-1;
	EnrollData *edSL;

	edSL = &enroll_data_sls[arrSL][subSeg];       // enroll_data_sls[MAX_STUDENT_LEVEL_NO_SL1][STUDENT_SUBSEGMENT_COUNT];

	if ( sl == DOCTOR )
	    // min & max bug chea
	    talentArr[ACADEMIC] = m.fmin(100.0f, math.get_random_snd(math.single_response_func(0.7f, 1.3f, 100, 50, edSL->academic.average), PSCH_SD(float(sqrt(edSL->academic.variance))), true ));
	else
	    talentArr[ACADEMIC] = math.get_random_snd(edSL->academic.average, PSCH_SD(float(sqrt(edSL->academic.variance))), true );
	talentArr[EXTRA_CURRI] = talentArr[ATHLETIC] = 0;

    }

    //## chea 161299 testing only
    //	talentArr[0]=talentArr[1]=talentArr[2] = 3.3f;

}

//----------- End of function EnrollRes::calc_a_talent --------//

//------------- End of function EnrollRes::calc_talent_student_db --------//
//
// calc talent of students in db: returning student
//
void EnrollRes::calc_talent_student_db() {
    StudentArray* stuArr;
    Student *stuPtr;
    float talentArr[TALENT_VAR_COUNT];
    int stuCount=0, i, j;

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

	    char studentLevel = stuPtr->student_level;
	    char stuSeg = stuPtr->student_segment;
	    char genderEthnicGroup = stuPtr->gender_ethnic_group;

	    if ( studentLevel == UG_TRADITION ) {
		//-------- assume: enroll_res.init() (dbf init) called --------//
		calc_a_talent(talentArr, studentLevel, genderEthnicGroup, stuSeg);
	    }
	    else {
		//-------- assume: value will be assigned after enroll_res.init_data() called
		calc_a_talent(talentArr, studentLevel, genderEthnicGroup);
	    }
	    //## 071299 chea 1.12.1
	    // min & max bug chea
	    stuPtr->talent_academic       = (char) max(0.0f,min(100.0f, talentArr[ACADEMIC] * 10));
	    //			stuPtr->talent_academic			 	= 33; //##200100 cHEA

	    // min & max bug chea
	    stuPtr->talent_extracurricular  = (char) max(0.0f,min(100.0f, talentArr[EXTRA_CURRI] * 10));
	    // min & max bug chea
	    stuPtr->talent_athletics      = (char) max(0.0f,min(100.0f, talentArr[ATHLETIC] * 10));

	    //			stuPtr->talent_academic			 	= (char) max(0.0f,min(100.0f, talentArr[ACADEMIC])); // min & max bug chea
	    //			stuPtr->talent_extracurricular 	= (char) max(0.0f,min(100.0f, talentArr[EXTRA_CURRI])); // min & max bug chea
	    //			stuPtr->talent_athletics		 	= (char) max(0.0f,min(100.0f, talentArr[ATHLETIC])); // min & max bug chea

	    stuCount++;
	}
    }

    err_when(stuCount != total_student_count);
}

//------------- End of function EnrollRes::calc_talent_student_db--------//

// ##### Begin Marco #### //
// ----------- Begin of function EnrollRes::save_initial_data --------//
void EnrollRes::save_initial_data() {
    for (int i = 0; i < MAX_STUDENT_LEVEL; i++) {
	initial_offers_rate[i] = offers_rate[i];
	initial_cur_yield_rate[i] = cur_yield_rate[i];
    }

    initial_matrics_top_athletes = matrics_top_athletes;
}

// ----------- End of function EnrollRes::save_initial_data --------//
// ##### End Marco #### //
