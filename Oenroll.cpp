//Filename		: OEnroll.cpp
//Description	: EnrollRes Class Definition
//Owner			: Fred

#include <obox.h>                                 // for progress box
#include <stdio.h>                                // for DEBUG
#include <osys.h>                                 // for progress box

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
#include <OFINANCE.H>                             // for next_day
#include <OINFO.H>                                // for next_day
#include <ODEPT.H>                                // for calc_student_count
#include <OSTUDENT.H>                             // for calc_student_count
#include <OLIBTECH.H>
#include <OATHLETI.H>
#include <OSTUOFF.H>
#include "OEnroll.h"

// database come from data_to_db.xls
//
#define PREFERENCE_DB "STUPREF"
#define APP_YIELD_DB    "APPYIELD"
#define PREFERENCE_DB2  "STUPREF2"
#define TALENT_DB     "STTALENT"

//----------- Define constants -------------//
//----------- Define static variables -------------//
short TRADIT_COUNT = 0;

#define MAX_TRADITION_COUNT 5
#define MAX_NONTRA_COUNT    5
#define MAX_MASTER_COUNT    3
#define MAX_DOCTOR_COUNT    3

const int RECORD_SEPARATOR_LEN = 2;
static char *RECORD_SEPARATOR = "\r\n";
const int FIELD_SEPARATOR_LEN = 1;
static char *FIELD_SEPARATOR = "\t";

static char *export_num(int value) {
    static char num_str[20];
    return itoa( value, num_str, 10 );              // 10 is radix
}

static char *export_num(double value) {
    static char num_str[20];
    return gcvt( value, 7, num_str );               // a few more more than 7, for negative sign, decimal point and exponent
}

static char *export_percent(double value) {       // value already multiplied by 100
    static char num_str[20];
    return gcvt( value*0.01, 7, num_str );          // a few more more than 7, for negative sign, decimal point and exponent
}

#define WRITE_FIELD_SEPARATOR(f)      f->file_write(FIELD_SEPARATOR, FIELD_SEPARATOR_LEN)
#define WRITE_STR_FIELD(f,s)          { f->file_write(s, strlen(s)); }
#define WRITE_NUM_FIELD(f,n)          { char *s=export_num(n); f->file_write(s, strlen(s)); }
#define WRITE_PERCENT_FIELD(f,n)      { char *s=export_percent(n); f->file_write(s, strlen(s)); }
#define WRITE_RECORD_SEPARATOR(f)     f->file_write(RECORD_SEPARATOR, RECORD_SEPARATOR_LEN)


//----------- Define static class member variables -------------//
const float EnrollRes::apps_latency[MAX_STUDENT_LEVEL] = { .4f, .4f, .5f, .6f, .7f };
// 990408 const float EnrollRes::yield_latency[MAX_STUDENT_LEVEL] = { .6f, .5f, .5f, .5f, .5f };	// elm 1-4 are useless
// elm 1-4 are useless
const float EnrollRes::yield_latency[MAX_STUDENT_LEVEL] = {
    .9f, .5f, .5f, .5f, .5f
};

// (* given to minority scores *)
const float EnrollRes::aidMinMultipliers[PRIORITY_LEVEL_COUNT] = {
    1, 1.05f, 1.10f
};
//const float aidAthMultipliers		= {1, 1.25f, 1.50f}; // (* extra aid for athletes *)

EnrollRes::EnrollRes() {
}

//----------- Define static functions -------------//
static bool is_minority_group(char group) {
    return group == MINORITY_MALE || group == MINORITY_FEMALE;
}

static bool is_male_group(char group) {
    return group == MINORITY_MALE || group == NONMINORITY_MALE;
}

//----------- Begin of function EnrollRes::adjust_fraction_merit_aid --------//
//! adjust player input; refer to to "General:helper functions"
//!
void EnrollRes::adjust_fraction_merit_aid(float& fraction) {
    if ( fraction > 0 && fraction < fraction_merit_top - .005f )
	fraction = fraction_merit_top + .005f;
    else if ( fraction > 1 + .005f - fraction_merit_top )
	fraction = (1 - .005f - fraction_merit_top) ;
}

//----------- Begin of function EnrollRes::adjust_fraction_merit_aid --------//

//----------- Begin of function EnrollRes::init --------//
//!
//!
void EnrollRes::init() {
    memset(this, 0, sizeof(EnrollRes));

    load_db_student_pref();
    load_db_student_pref2();
    load_db_app_yield();
    load_db_student_talent();

    distance_learner_acceptance = 1;

    //-------- init constants ----------//
    s_lower_bound = 0.5f;
    s_upper_bound = 1.5f;
    s_slope     = 5.0f;

    chance_avg  = 1.0f;
    chance_sd = 1.0f;

    fraction_merit_top = 0.025f;                    // see section 2.4 last paragraph
}

//------------- End of function EnrollRes::init --------//

//----------- Begin of function EnrollRes::deinit --------//
//!
void EnrollRes::deinit() {
}

//------------- End of function EnrollRes::deinit --------//

//----------- Begin of function EnrollRes::load_db_student_pref ------//
//!
//! refer to HE.initialization[stu_pref]
//!
void EnrollRes::load_db_student_pref() {
    PreferenceRec  *prefRec;
    Database    *dbPreference = game_set.open_db(PREFERENCE_DB);

    int db_count = (short) dbPreference->rec_count();

    err_when( db_count != PREFERECNE_COUNT * 2 );   // 2: average and deviation

    //askbillok: how to handle "na in cells like E119-120
    //	segment2 = segment1

    //------ read in Preference information array -------//

    memset( preference_array, 0, sizeof(preference_array));

    for( int i=0 ; i<db_count ; i++ ) {
	//----- odd row/record store average -----//
	prefRec = (PreferenceRec*) dbPreference->read(i+1);

	for( int j=0 ; j<INSTITU_SEGMENT_COUNT; j++ ) {
	    preference_array[i/2][j].average = (float) m.atof( prefRec->value[j], 9 );
	    preference_array[i/2][j].weight = (float) m.atof( prefRec->weight, 9 );
	}

	i++;
	//-----  even row/record store deviation -----//
	prefRec = (PreferenceRec*) dbPreference->read(i+1);
	int j;
	for( j=0 ; j<INSTITU_SEGMENT_COUNT; j++ )
	    preference_array[i/2][j].deviation = (float) m.atof( prefRec->value[j], 9 );

	preference_array[i/2][j].tmp_fuzzy_value = 0.0f;
    }
}

//------------- End of function EnrollRes::load_db_student_pref ------//

//----------- Begin of function EnrollRes::load_db_student_pref2 ------//
//!
//! refer to [HE.STU.pref]sheet1
//!
void EnrollRes::load_db_student_pref2() {
    PreferenceRec2  *prefRec;
    Database    *dbPreference = game_set.open_db(PREFERENCE_DB2);
    String      tmpStr;

    int db_count = (short) dbPreference->rec_count();

    err_when( db_count != PREFERECNE_COUNT2 * (SCHOOL_CONTROL_COUNT + MAX_STUDENT_LEVEL - 1) );

    // make sure values of constants conformed to database column order
    err_when( NON_MINORITY != 0 );
    err_when( PUBLIC != 1 || PRIVATE != 0 );

    //------ read in Preference information array -------//

    memset( preference_array2, 0, sizeof(preference_array2));

    // for sl1

    int i, j, arrSL;
    char  control = PRIVATE;

    for( i=0; i<PREFERECNE_COUNT2*SCHOOL_CONTROL_COUNT; i++ ) {
	if ( i >= PREFERECNE_COUNT2 ) {
	    control = PUBLIC;
	    j = i - PREFERECNE_COUNT2;
	}
	else {
	    j = i;
	}

	prefRec = (PreferenceRec2*) dbPreference->read(i+1);

	m.rtrim_fld(tmpStr, prefRec->weight[0], 9);
	if ( strcmp(tmpStr, "") != 0 )
	    preference_array2[control][NON_MINORITY][j].apps_ratio  = (float) m.atof( prefRec->weight[0], 9 );
	else
	    preference_array2[control][NON_MINORITY][j].apps_ratio  = -1.0f;

	m.rtrim_fld(tmpStr, prefRec->weight[1], 9);
	if ( strcmp(tmpStr, "") != 0 )
	    preference_array2[control][MINORITY][j].apps_ratio      = (float) m.atof( prefRec->weight[1], 9 );
	else
	    preference_array2[control][MINORITY][j].apps_ratio      = -1.0f;

	m.rtrim_fld(tmpStr, prefRec->weight[2], 9);
	if ( strcmp(tmpStr, "") != 0 )
	    preference_array2[control][NON_MINORITY][j].yield_rate  = (float) m.atof( prefRec->weight[2], 9 );
	else
	    preference_array2[control][NON_MINORITY][j].yield_rate  = -1.0f;

	m.rtrim_fld(tmpStr, prefRec->weight[3], 9);
	if ( strcmp(tmpStr, "") != 0 )
	    preference_array2[control][MINORITY][j].yield_rate      = (float) m.atof( prefRec->weight[3], 9 );
	else
	    preference_array2[control][MINORITY][j].yield_rate      = -1.0f;

    }

    // for sl2-5;  arrSL=0 -> sl2

    for( i=0, arrSL=-1; i<PREFERECNE_COUNT2 * (MAX_STUDENT_LEVEL - 1); i++ ) {
	j = i % PREFERECNE_COUNT2;

	if ( j==0 ) {
	    arrSL++;    err_when(arrSL == MAX_STUDENT_LEVEL-1);
	}

	prefRec = (PreferenceRec2*) dbPreference->read(PREFERECNE_COUNT2*SCHOOL_CONTROL_COUNT + i+1);

	// for sl1
	if ( i == 63 ) {                              // start of SL5, Institutional Prestige
	    preference_array2_sls[arrSL][PRIVATE][NON_MINORITY][0]  = 0.1;
	    preference_array2_sls[arrSL][PRIVATE][MINORITY][0]      = 0.1;
	    preference_array2_sls[arrSL][PUBLIC][NON_MINORITY][0]   = 0.1;
	    preference_array2_sls[arrSL][PUBLIC][MINORITY][0]     = 0.1;
	}
	else
	    if ( i == 75 ) {                              // % of minority
		preference_array2_sls[arrSL][PRIVATE][NON_MINORITY][12] = -1.0f;
		preference_array2_sls[arrSL][PRIVATE][MINORITY][12]   = -1.0f;
		preference_array2_sls[arrSL][PUBLIC][NON_MINORITY][12]  = -1.0f;
		preference_array2_sls[arrSL][PUBLIC][MINORITY][12]      = -1.0f;
	    }
	    else
		if ( i == 78 ) {                              // Tuition: Private Institutions
		    preference_array2_sls[arrSL][PRIVATE][NON_MINORITY][15] = 0.5;
		    preference_array2_sls[arrSL][PRIVATE][MINORITY][15]   = 0.5;
		    preference_array2_sls[arrSL][PUBLIC][NON_MINORITY][15]  = -1.0f;
		    preference_array2_sls[arrSL][PUBLIC][MINORITY][15]      = -1.0f;
		}
		else
		    if ( i == 79 ) {                              // Blended Tuition: Public Institutions
			preference_array2_sls[arrSL][PRIVATE][NON_MINORITY][16] = -1.0f;
			preference_array2_sls[arrSL][PRIVATE][MINORITY][16]   = -1.0f;
			preference_array2_sls[arrSL][PUBLIC][NON_MINORITY][16]  = 0.3;
			preference_array2_sls[arrSL][PUBLIC][MINORITY][16]      = 0.3;
		    }
		    else {

			m.rtrim_fld(tmpStr, prefRec->weight[0], 9);
			preference_array2_sls[arrSL][PRIVATE][NON_MINORITY][j]
			    = ( strcmp(tmpStr, "") != 0 ) ? (float) m.atof( prefRec->weight[0], 9 ) : -1.0f;

			m.rtrim_fld(tmpStr, prefRec->weight[1], 9);
			preference_array2_sls[arrSL][PRIVATE][MINORITY][j]
			    = ( strcmp(tmpStr, "") != 0 ) ? (float) m.atof( prefRec->weight[1], 9 ) : -1.0f;

			m.rtrim_fld(tmpStr, prefRec->weight[2], 9);
			preference_array2_sls[arrSL][PUBLIC][NON_MINORITY][j]
			    = ( strcmp(tmpStr, "") != 0 ) ? (float) m.atof( prefRec->weight[2], 9 ) : -1.0f;

			m.rtrim_fld(tmpStr, prefRec->weight[3], 9);
			preference_array2_sls[arrSL][PUBLIC][MINORITY][j]
			    = ( strcmp(tmpStr, "") != 0 ) ? (float) m.atof( prefRec->weight[3], 9 ) : -1.0f;
		    }
    }
    // set new variable (Percent of courses redesigned for optimal IT)
    // just for SL-5 Distance Learning
    preference_array2_sls[3][PRIVATE][NON_MINORITY][21] = 0.4;
    preference_array2_sls[3][PRIVATE][MINORITY][21]     = 0.4;
    preference_array2_sls[3][PUBLIC][NON_MINORITY][21]    = 0.6;
    preference_array2_sls[3][PUBLIC][MINORITY][21]      = 0.6;
}

//------------- End of function EnrollRes::load_db_student_pref2 ------//

//----------- Begin of function EnrollRes::load_db_app_yield ------//
//!
//! refer to [HE.initialization.xls]Apps_Yield
//!
void EnrollRes::load_db_app_yield() {
    AppYieldRec   *appRec;
    Database    *dbAppYield = game_set.open_db(APP_YIELD_DB);
    int       varOffset, stuSubSeg, stuSeg, iSeg, i=0;

    int db_count = (short) dbAppYield->rec_count();

    err_when( db_count != STUDENT_SEGMENT_COUNT * STUDENT_SUBSEGMENT_COUNT * APP_YIELD_VAR_COUNT);

    //------ read in Preference information array -------//

    memset( app_yield_array, 0, sizeof(app_yield_array));

    for ( varOffset=0; varOffset<APP_YIELD_VAR_COUNT; varOffset++ )
	for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
	    for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
		appRec = (AppYieldRec*) dbAppYield->read(i+1);
		i++;
		for ( iSeg=0; iSeg<INSTITU_SEGMENT_COUNT; iSeg++ ) {
		    app_yield_array[stuSubSeg][stuSeg][iSeg][varOffset]
			= (short) m.atoi( appRec->count[iSeg], 9 );
		    err_when(app_yield_array[stuSubSeg][stuSeg][iSeg][varOffset] < 0);
		}
	    }

#ifdef DEBUG_
    debug_msg("----- begin enroll: app_yield -----");

    for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
	for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
	    char str[100] = "";
	    for ( iSeg=0; iSeg<INSTITU_SEGMENT_COUNT; iSeg++ ) {
		sprintf(str, "%s, %d", str, app_yield_array[stuSubSeg][stuSeg][iSeg].apps);
	    }
	    debug_msg(str);
	}
    debug_msg("----- end enroll: app_yield -----");
#endif
}

//------------- End of function EnrollRes::load_db_app_yield ------//

//----------- Begin of function EnrollRes::load_db_student_talent ------//
//!
//! refer to [HE.STU.talent.xls]Sheet1
//!

//------- static var for the 2 func below ------------//
static float income_growth[STUDENT_SUBSEGMENT_COUNT][STUDENT_SEGMENT_COUNT];
static float income_growth_last2[STUDENT_SUBSEGMENT_COUNT][STUDENT_SEGMENT_COUNT];

void EnrollRes::load_db_student_talent() {
#define TALENT_VAR_PROPERTIES_COUNT   3         // mean, sd, count

    TalentRec *talentRec;
    Database    *dbPreference = game_set.open_db(TALENT_DB);

    int db_count = (short) dbPreference->rec_count();

    err_when( db_count !=  STUDENT_SUBSEGMENT_COUNT * TALENT_INCOME_VAR_COUNT * TALENT_VAR_PROPERTIES_COUNT);

    //------ read in array -------//

    memset( talent_array, 0, sizeof(talent_array));

    int var, subSeg, seg, i=1;

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ )
	for ( var=0; var<TALENT_INCOME_VAR_COUNT; var++ ) {
	    // read average row
	    talentRec = (TalentRec*) dbPreference->read(i++);

	    for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
		talent_array[subSeg][seg][var].average
		    = (float) m.atof( talentRec->seg[seg], 9 );
	    }

	    // read standard deviation row
	    talentRec = (TalentRec*) dbPreference->read(i++);

	    for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
		talent_array[subSeg][seg][var].variance
		    = (float) m.atof( talentRec->seg[seg], 9 );
	    }

	    // skip the row "count" for each talent rating
	    //talentRec = (TalentRec*) dbPreference->read(i++);
	    i++;
	}

    // adjust the talent_array
    // refer to mathca cell:
    // "Traditional Undergraduates: sL-1: Test financial data"
    //
    // 10 years
    const float INCOME_UPDATE = math.safe_pow(1.04f, 10);

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	for ( var=0; var<TALENT_VAR_COUNT; var++ )
	    for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
		talent_array[subSeg][seg][var].variance
		    = math.safe_pow(talent_array[subSeg][seg][var].variance, 2);
	    }

	for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	    talent_array[subSeg][seg][INCOME].average
		*= INCOME_UPDATE;
	    talent_array[subSeg][seg][INCOME].variance
		// * INCOME_UPDATE
		= math.safe_pow(talent_array[subSeg][seg][var].variance / 10, 2);
	}
    }

    //--------------------------//

    // 1027 req26.txt

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	    income_growth[subSeg][seg] = math.get_random_snd(0.015f, PSCH_SD(.0025f));
	    income_growth_last2[subSeg][seg] = 0;
	}
    }
}

//------------- End of function EnrollRes::load_db_student_talent ------//

//----------- Begin of function EnrollRes::update_income ------//
//!
//! called yearly before enroll_main()
//!
void EnrollRes::update_income() {
    // adjust this var: Talent talent_array[STUDENT_SUBSEGMENT_COUNT][STUDENT_SEGMENT_COUNT][TALENT_INCOME_VAR_COUNT];
    int subSeg, seg;
    float inflationRate = (float) finance.inflation_rate / 100.0f;

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	    float lastValue = income_growth[subSeg][seg];

	    income_growth[subSeg][seg] = math.time_variation(lastValue, income_growth_last2[subSeg][seg], 0.9908f, -0.6314f, 0.003202690f);
	    income_growth_last2[subSeg][seg] = lastValue;

	    talent_array[subSeg][seg][INCOME].average *= (1+inflationRate+income_growth[subSeg][seg]);
	}
    }
}

//----------- End of function EnrollRes::update_income ------//

//----------- Begin of function EnrollRes::calc_student_pref --------//
//!
//! Refer to [HE.initialization]stu_pref.
//!
//! This function will be run for the PGI and for peers at initialization and
//! as part of each year's admissions process. (from email req11_enroll.txt)
//!
void EnrollRes::calc_student_pref() {
    // column L
    // recalculate PeerSchool's properties to prefArray to speed up calculation
    // '10' is the additional field included in PREFERECNE_COUNT2 (pref_vars_array)
    // but not PREFERECNE_COUNT (prefArray)
    //
    PeerSchool* player = school_res.player_peer_school;
    float prefArray[PREFERECNE_COUNT];
    int i, j;

    for (j=0, i=0; j<PREFERECNE_COUNT; j++, i++) {
	if ( j == 10 )                                // skip in-state freshmen percent
	    i++;

	prefArray[j] = player->pref_vars_array[i];
    }

    err_when( PREFERECNE_COUNT != 20 );

    // E129-K147: Fuzzy Logic Indices for Undergraduate Applications Ratios and Yield Rates
    Preference *prefPtr;

    for (j=0; j<PREFERECNE_COUNT; j++)
	for (i=0; i<INSTITU_SEGMENT_COUNT; i++) {
	    prefPtr = (enroll_res.preference_array[j]) + i;
	    //prefPtr++;
	    prefPtr->tmp_fuzzy_value = (float) exp(-math.safe_pow((  math.safe_divide((prefPtr->average-prefArray[j]), prefPtr->deviation)  ), 2));
	}

    // E150-K150: weighted raw index value: update player's preference_array
    bool isPlayerSchoolPublic = player_school.is_public();
    float tmpSumOfSum = 0.0f;

    for (i=0; i<INSTITU_SEGMENT_COUNT; i++) {
	float tmpSum = 0.0f;

	for (j=0; j<NON_FINANCIAL_PREFERECNE_COUNT; j++) {
	    prefPtr = (enroll_res.preference_array[j]) + i;

	    tmpSum += prefPtr->tmp_fuzzy_value * prefPtr->weight;

	}

	err_when(tmpSum==0);

	for (j=NON_FINANCIAL_PREFERECNE_COUNT; j<PREFERECNE_COUNT; j+=2) {
	    prefPtr = (enroll_res.preference_array[j]) + i;

	    if ( isPlayerSchoolPublic )
		prefPtr++;

	    tmpSum += prefPtr->tmp_fuzzy_value * prefPtr->weight;
	}
	pref_weight_array[i] = tmpSum;
	tmpSumOfSum += tmpSum;

	err_when(tmpSumOfSum==0);
    }

    // E151-K151: calc segment weight
    for (i=0; i<INSTITU_SEGMENT_COUNT; i++)
	pref_weight_array[i] /= tmpSumOfSum;
}

//------------- End of function EnrollRes::calc_student_pref --------//

//----------- Begin of function EnrollRes::calc_app_yield --------//
//!
//! Refer to [HE.initialization]Apps_Yield.
//!
//! You can program a function to calculate columns BQ and BR of
//! Initialization:Apps_Yield. These "Base applications ratios" and "Base yield
//! rates"  represent the first step in calculating the final applications
//! ratios and yield rates. (from email req11_enroll.txt)
//!
//! this function is called during game initialization
//!
void EnrollRes::calc_app_yield() {
    float     appYield[APP_YIELD_VAR_COUNT], totalMatrics=0;
    int     varOffset, stuSubSeg, stuSeg, iSeg, i=0;
    // short				*shortPtr
    // short			*studenCountPtr;

    // refer to columns BM:QR
    for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
	for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
	    memset(appYield, 0, sizeof(appYield));
	    for ( varOffset=0; varOffset<APP_YIELD_VAR_COUNT; varOffset++ ) {
		//studenCountPtr = &( appYieldArray[varOffset] );
		for ( iSeg=0; iSeg<INSTITU_SEGMENT_COUNT; iSeg++ ) {
		    appYield[varOffset] += (enroll_res.app_yield_array[stuSubSeg][stuSeg][iSeg][varOffset] * pref_weight_array[iSeg]);

		    err_when(appYield[varOffset] < 0);
		}
	    }

	    err_when( appYield[MATRICS] < 0.000001f || appYield[OFFERS] < 0.000001f );
	    //err_when( appYield[OFFERS] > appYield[APPS] );	//1117
	    err_when( appYield[MATRICS] > appYield[OFFERS] );

	    //TO use pointer to speed it up
	    enroll_data_sl1[stuSubSeg][stuSeg].apps_ratio = float(appYield[APPS]) / appYield[MATRICS];
	    enroll_data_sl1[stuSubSeg][stuSeg].yield_rate = float(appYield[MATRICS]) / appYield[OFFERS];
	    enroll_data_sl1[stuSubSeg][stuSeg].fraction_matrics = appYield[MATRICS];

	    totalMatrics += appYield[MATRICS];
	}

    // note: apps_ratio in enroll_data is not updated yearly, it's just for init_enroll
    err_when(totalMatrics <= 0);

    for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
	for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
	    enroll_data_sl1[stuSubSeg][stuSeg].fraction_matrics /= totalMatrics;
	}
}

//------------- End of function EnrollRes::calc_app_yield --------//

//------------- End of function EnrollRes::set_app_yield --------//
//
void EnrollRes::set_app_yield(float appMultipler, float yieldMultiplier) {
    int stuSubSeg, stuSeg;

    for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
	for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
	    enroll_data_sl1[stuSubSeg][stuSeg].apps_ratio *= appMultipler;
	    enroll_data_sl1[stuSubSeg][stuSeg].yield_rate *= yieldMultiplier;
	}
}

//------------- End of function EnrollRes::set_app_yield --------//

//----------- static variables of class PeerSchool --------//
float PeerSchool::pref_vars_average_array[PREFERECNE_COUNT2];
float PeerSchool::pref_vars_average_array_last[PREFERECNE_COUNT2];

//----------- Begin of function PeerSchool::calc_student_pref_vars_array --------//
//!
//! Refer to column L of [Initialization.xls]stu_prefs
//! calc_student_pref_vars_array is designed for player school only
//!
//! Values will be used for calc of column Q of [HE.STU.pref.xls]Sheet1
//! Also see calc_student_pref_vars_average_array
//!
void PeerSchool::calc_student_pref_vars_array() {
    if ( this != school_res.player_peer_school ) {
	calc_student_pref_vars_array_peer();
	return;
    }

    memcpy( pref_vars_array_last, pref_vars_array, sizeof(pref_vars_array) );

    // calculate the column Q in [HE.STU.pref.xls]Sheet1
    //
    // recalculate PeerSchool's properties to prefArray to speed up calculation
    // (refer to column L of [HE.initialization]stu_pref)
    // local var: float pref_vars_array[PREFERECNE_COUNT2];

    // (minor bug) the following RHSs may NOT be updated during play

    // assume "this" is the player_peeer_school
    int studentEnrollCount[MAX_STUDENT_LEVEL];
    int i;

    for ( i=0; i<MAX_STUDENT_LEVEL; i++ ) {
	studentEnrollCount[i] = enroll_res.student_count[i];
	percent_female_sl[i] = (char) enroll_res.student_female_percent[i];
	percent_minority_sl[i] = (char) enroll_res.student_minority_percent[i];
    }

    pref_vars_array[0] = prestige;
    // by 1021 email
    pref_vars_array[1] = (float) athletics_office.ncaa_level_value;
    pref_vars_array[2] = float(student_life_staff_salaries+student_life_other_expense)/(full_time_undergrad+part_time_undergrad);
    pref_vars_array[3] = percent_traditional_ug_in_residence_halls / 100.0f;
    pref_vars_array[4] = percent_get_bacc_in_5_year / 100.0f;

    pref_vars_array[5] = enroll_res.applications_rate[UG_TRADITION];
    pref_vars_array[6] = enroll_res.cur_yield_rate[UG_TRADITION];
    pref_vars_array[7] = float(studentEnrollCount[1]) / float(studentEnrollCount[0]+studentEnrollCount[1]);

    float tmpCount = 0;

    pref_vars_array[8] = float(studentEnrollCount[0]+studentEnrollCount[1]) / enroll_res.total_student_count;
    pref_vars_array[9] = float(studentEnrollCount[4]) / enroll_res.total_student_count;

    // the additional field included in PREFERECNE_COUNT2 but not PREFERECNE_COUNT
    pref_vars_array[10] = float(percent_instate_freshmen) / 100;

    for (i = 0, tmpCount = 0; i<MAX_STUDENT_LEVEL; i++)
	tmpCount += float(studentEnrollCount[i]) * percent_female_sl[i] / 100;

    pref_vars_array[11] = float(tmpCount) / enroll_res.total_student_count;

    for (i = 0, tmpCount = 0; i<MAX_STUDENT_LEVEL; i++)
	tmpCount += studentEnrollCount[i] * percent_minority_sl[i] / 100;

    pref_vars_array[12] = float(tmpCount) /  enroll_res.total_student_count;
    pref_vars_array[13] = doc_time_to_degree;
    pref_vars_array[14] = sponsored_research_rating;
    pref_vars_array[15] = (float) finance.tuition_rate;

    pref_vars_array[16] = pref_vars_array[15];
    pref_vars_array[17] = percent_ug_students_on_aid / 100.0f;
    pref_vars_array[18] = pref_vars_array[17];
    pref_vars_array[19] = institutional_aid_per_fte;
    pref_vars_array[20] = institutional_aid_per_fte;

    err_when( PREFERECNE_COUNT2 != 21 );

    for (i=0; i<PREFERECNE_COUNT2; i++)
	err_when( pref_vars_array[i] < 0 );

}

//------------- End of function PeerSchool::calc_student_pref_vars_array --------//

//------------- Begin of function PeerSchool::calc_student_pref_vars_array_peer --------//
//!
//! calc_student_pref_vars_array_peer is designed for peer school only, not including player school
//!
//!
//! Refer to column L of [Initialization.xls]stu_prefs
//!
//! Values will be used for calc of column Q of [HE.STU.pref.xls]Sheet1
//! Also see calc_student_pref_vars_average_array
//!
void PeerSchool::calc_student_pref_vars_array_peer() {
    memcpy( pref_vars_array_last, pref_vars_array, sizeof(pref_vars_array) );

    // calculate the column Q in [HE.STU.pref.xls]Sheet1
    //
    // recalculate PeerSchool's properties to prefArray to speed up calculation
    // (refer to column L of [HE.initialization]stu_pref)
    // local var: float pref_vars_array[PREFERECNE_COUNT2];

    // the following RHSs may NOT be updated during play

    pref_vars_array[0] = prestige;
    pref_vars_array[1] = (float) athletics_office.ncaa_level_value;
    pref_vars_array[2] = math.safe_divide( float(student_life_staff_salaries+student_life_other_expense), (float)full_time_undergrad+part_time_undergrad );
    pref_vars_array[3] = percent_traditional_ug_in_residence_halls / 100.0f;
    pref_vars_array[4] = percent_get_bacc_in_5_year / 100.0f;

    pref_vars_array[5] = ug_applications_rate;
    pref_vars_array[6] = ug_yield_rate / 100.0f;
    pref_vars_array[7] = math.safe_divide( (float)part_time_undergrad, (float)full_time_undergrad+part_time_undergrad );

    int studentEnrollCount[MAX_STUDENT_LEVEL];
    studentEnrollCount[0] = full_time_undergrad;
    studentEnrollCount[1] = part_time_undergrad;
    studentEnrollCount[2] = enrollment_masters;
    studentEnrollCount[3] = enrollment_doctoral;
    studentEnrollCount[4] = non_degree_seeking;

    float studentCount = 0.0f;
    int i, tmpCount=0;
    for (i=0; i<MAX_STUDENT_LEVEL; i++)
	studentCount += (float) studentEnrollCount[i];

    pref_vars_array[8] = math.safe_divide( (int)(full_time_undergrad+part_time_undergrad), (int)studentCount );
    pref_vars_array[9] = math.safe_divide( (int)non_degree_seeking, (int)studentCount );

    // the additional field included in PREFERECNE_COUNT2 but not PREFERECNE_COUNT
    pref_vars_array[10] = percent_instate_freshmen;

    for (i=0; i<MAX_STUDENT_LEVEL; i++)
	tmpCount += studentEnrollCount[i] * percent_female_sl[i] / 100;

    pref_vars_array[11] = math.safe_divide( (int)tmpCount, (int)studentCount );

    for (i=0, tmpCount = 0; i<MAX_STUDENT_LEVEL; i++)
	tmpCount += studentEnrollCount[i] * percent_minority_sl[i] / 100;

    pref_vars_array[12] = math.safe_divide( (int)tmpCount, (int)studentCount );
    pref_vars_array[13] = doc_time_to_degree;
    pref_vars_array[14] = sponsored_research_rating;
    pref_vars_array[15] = (float) tuition_rate;

    pref_vars_array[16] = pref_vars_array[15];
    pref_vars_array[17] = percent_ug_students_on_aid / 100.0f;
    pref_vars_array[18] = pref_vars_array[17];
    pref_vars_array[19] = institutional_aid_per_fte;
    pref_vars_array[20] = institutional_aid_per_fte;

    err_when( PREFERECNE_COUNT2 != 21 );

    for (i=0; i<PREFERECNE_COUNT2; i++)
	err_when( pref_vars_array[i] < 0 );
}

//------------- End of function PeerSchool::calc_student_pref_vars_array_peer --------//

//----------- Begin of static function PeerSchool::calc_student_pref_vars_average_array --------//
//
// Refer to [HE.STU.pref.xls]Sheet1
//
// Calc values for column Q for PGI and peer schools
//
void PeerSchool::calc_student_pref_vars_average_array() {
    memcpy( pref_vars_average_array_last, pref_vars_average_array, sizeof(pref_vars_average_array) );

    float sum;
    short psCount = school_res.peer_school_count;

    // pref_vars_average_array is static
    //
    for (int var=0; var<PREFERECNE_COUNT2; var++) {
	sum = 0.0f;

	for (int i=0; i<psCount; i++) {
	    PeerSchool *ps = &(school_res.peer_school_array[i]);

	    if ( ps != school_res.player_peer_school )

		sum += ps->pref_vars_array[var];
	}

	pref_vars_average_array[var] = sum / psCount;
    }
}

//------------- End of static function PeerSchool::calc_student_pref_vars_average_array --------//

//----------- Begin of function EnrollRes::calc_enroll_applications --------//
//!----------- Begin of function EnrollRes::calc_enroll_matriculations --------//
//!
//! Refer to [HE.STU.pref.xls]Sheet1 and techdoc 3.4 section 2.2 & 2.5
//! and the mathca file HE.EnrMgt.rev.nb: "Traditional Undergraduates: sL-1: Applications: function"
//! and the mathca file HE.EnrMgt.rev.nb: "Traditional Undergraduates: sL-1: Matriculations: function"
//!
//! The result, in cell R29, will be multiplied by the the base figure
//! calculated per (6) to get the final apps ratio and yield rate for the
//! appropriate gender-ethnic group and institutional type.
//! (from email req11_enroll.txt)
//!
//!
void EnrollRes::calc_enroll_applications() {
    const int sl = 0;
    float prefVarsArray[PREFERECNE_COUNT2];         // column Q
    int i, j;

    // calc values for column Q
    // The data in column Q represent
    // the "ratio of the variable for the PGI to the average of the same variable
    // for the peer institutions." (from email req11_enroll.txt)
    //
    PeerSchool* player = school_res.player_peer_school;
    memcpy(prefVarsArray, player->pref_vars_array, sizeof(prefVarsArray));

    for (i=0; i<PREFERECNE_COUNT2; i++) {
	// (* uses lagged financial aid variables; current values for all else *)
	//
	if ( i < PREFERECNE_COUNT2 - 4 )
	    prefVarsArray[i] = math.safe_divide(prefVarsArray[i], PeerSchool::pref_vars_average_array[i]);
	else
	    prefVarsArray[i] = math.safe_divide(player->pref_vars_array_last[i], PeerSchool::pref_vars_average_array_last[i]);
    }

    // calc values for column R
    //
    bool isPublic = player_school.is_public();
    float finalRatio[MINORITY_GROUP_COUNT];
    float tmpRatio;
    float tmp, sign;

    for (j=0; j<MINORITY_GROUP_COUNT; j++) {
	finalRatio[j] = 1;

	for (i=0; i<PREFERECNE_COUNT2; i++) {
	    sign = (pref_special_case(i))? -1.0f: 1.0f;

	    // 1
	    tmp = enroll_res.preference_array2[isPublic][j][i].apps_ratio;

	    if ( tmp <= 0.0 || prefVarsArray[i] == 0)
		tmpRatio = 1.0f;
	    else
		tmpRatio = math.safe_pow(prefVarsArray[i], sign * tmp);

	    // 2
	    finalRatio[j] *= tmpRatio;

	}

	if ( !(1+ (float) exp(-s_slope*(finalRatio[j]-1))) )
	    finalRatio[j] = 1;
	else
	    finalRatio[j] = s_lower_bound+(s_upper_bound-s_lower_bound)/(1+ (float) exp(-s_slope*(finalRatio[j]-1)));

	err_when( finalRatio[j] < 0 );
    }

    // refer to mathca code only this part
    //
    int minorGroup, stuSubSeg, stuSeg;
    EnrollDataSL1* ed;

    total_applications[sl] = 0;

    finalRatio[MINORITY] *= app_multiplier[sl];
    finalRatio[NON_MINORITY] *= app_multiplier[sl];

    //----------------//

    //	total_applications[sl]=0;	// ## CHWG062199

    for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ ) {
	if ( is_minority_group(stuSubSeg) )
	    minorGroup = MINORITY;
	else
	    minorGroup = NON_MINORITY;

	for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
	    ed = &enroll_data_sl1[stuSubSeg][stuSeg];

	    ed->applications
		= int( (1.0f-apps_latency[sl]) * math.get_random_snd(chance_avg, PSCH_SD(chance_sd), true)
		       * finalRatio[minorGroup] * ed->base_apps
		       // + 0.5 for float-int rounding
		       + apps_latency[sl] * ed->applications + 0.5);

	    err_when(ed->applications < 0);
	    total_applications[sl] += ed->applications;
	}
    }
}

//---------------------////---------------------////---------------------//
void EnrollRes::calc_enroll_matriculations() {
    const int sl = 0;                               //## chea 260899 try to use loop below

    float prefVarsArray[PREFERECNE_COUNT2];         // column Q
    int i, j;

    // calc values for column Q
    // The data in column Q represent
    // the "ratio of the variable for the PGI to the average of the same variable
    // for the peer institutions." (from email req11_enroll.txt)
    //
    PeerSchool* player = school_res.player_peer_school;
    memcpy(prefVarsArray, player->pref_vars_array, sizeof(prefVarsArray));

    for (i=0; i<PREFERECNE_COUNT2; i++) {
	// (* uses current values for all variables *)
	//
	prefVarsArray[i] = math.safe_divide(prefVarsArray[i], PeerSchool::pref_vars_average_array[i]);
    }

    // calc values for column R
    //
    bool isPublic = player_school.is_public();
    float finalRatio[MINORITY_GROUP_COUNT];
    float tmpRatio;
    float tmp, sign;

    for (j=0; j<MINORITY_GROUP_COUNT; j++) {
	finalRatio[j] = 1;

	for (i=0; i<PREFERECNE_COUNT2; i++) {
	    sign = (pref_special_case(i))? -1.0f: 1.0f;

	    // 1
	    //tmp = enroll_res.preference_array2[isPublic][j][i].apps_ratio;
	    tmp = enroll_res.preference_array2[isPublic][j][i].yield_rate;

	    if ( tmp <= 0.0 )                           // empty entry in database
		tmpRatio = 1;
	    else if ( prefVarsArray[i] == 0 )           // 0.75: from mathca code
		tmpRatio = 0.75f;
	    else {
		if ( i < PREFERECNE_COUNT2 - 4 )
		    tmpRatio = (float) math.safe_pow(prefVarsArray[i], sign * tmp);
		else if ( i < PREFERECNE_COUNT2 - 2 )
		    tmpRatio = (float) math.safe_pow(math.safe_divide(fraction_on_aid[j], prefVarsArray[i]), sign * tmp);
		else
		    tmpRatio = (float) math.safe_pow(math.safe_divide(float(average_aid[j]), prefVarsArray[i]), sign * tmp);
	    }

	    if ( tmpRatio < -100 || tmpRatio > 100 )    // fix bug on fly
		tmpRatio = 1;

	    // 2
	    finalRatio[j] *= tmpRatio;
	}

	if ( !(1+ (float) exp(-s_slope*(finalRatio[j]-1))) )
	    finalRatio[j] = 1;
	else
	    finalRatio[j] = s_lower_bound+(s_upper_bound-s_lower_bound)/(1+ (float) exp(-s_slope*(finalRatio[j]-1)));
    }

    // refer to mathca code only for this part
    //
    int minorGroup, stuSubSeg, stuSeg;
    EnrollDataSL1* ed; {

	// BUG_HERE
	// for statement is missing in the original version..//## chwg0628	..
	//		for(sl=0;sl<MAX_STUDENT_LEVEL;sl++)
	//
	//	for(int sl=0;sl<MAX_STUDENT_LEVEL;sl++)  //## chea 260899 try adding
	total_matrics[sl] = 0;
	matrics_top_athletes = 0; male_matrics_top_athletes = 0;

	for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ ) {
	    if ( is_minority_group(stuSubSeg) )
		minorGroup = MINORITY;
	    else
		minorGroup = NON_MINORITY;

	    for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
		ed = &enroll_data_sl1[stuSubSeg][stuSeg];

		ed->yield_rate
		    = (1.0f-yield_latency[sl]) * math.get_random_snd(chance_avg, PSCH_SD(chance_sd), true)
		    * yield_multiplier[sl] * finalRatio[minorGroup] * ed->base_yield_rate
		    + yield_latency[sl] * ed->yield_rate;

		// make yield_rate within [1:0]
		//min & max bug chea
		ed->yield_rate = max(0.0f, min(1.0f, ed->yield_rate));

		// calc matriculations
		// + 0.5 for float-int rounding
		ed->matrics =  int(ed->yield_rate * ed->offers+ 0.5);

		// calc total matriculations
		total_matrics[sl] += ed->matrics;
	    }

	    // calc number of top athletes *newly* recruited
	    // which is the number of matriculants in stdent segment 4 (athlete)
	    matrics_top_athletes += enroll_data_sl1[stuSubSeg][3].matrics;

	    if(is_male_group(stuSubSeg))
		male_matrics_top_athletes += enroll_data_sl1[stuSubSeg][3].matrics;
	}
    }

    // do again to adjust total_matrics to target_enrollment
    if( (!info.prerun_year || info.prerun_year && info.game_year > 1 ) && total_matrics[sl] >= 10 ) {
	int oldTotalMatrics = total_matrics[sl];
	int targetIntake = target_student_intake[sl];

	total_matrics[sl] = 0;
	matrics_top_athletes = 0; male_matrics_top_athletes = 0;

	for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ ) {
	    if ( is_minority_group(stuSubSeg) )
		minorGroup = MINORITY;
	    else
		minorGroup = NON_MINORITY;

	    for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
		ed = &enroll_data_sl1[stuSubSeg][stuSeg];

		ed->yield_rate = ed->yield_rate * targetIntake / oldTotalMatrics;

		// make yield_rate within [1:0]
		//min & max bug chea
		ed->yield_rate = max(0.0f, min(1.0f, ed->yield_rate));

		// calc matriculations
		// + 0.5 for float-int rounding
		ed->matrics =  int(ed->yield_rate * ed->offers+ 0.5);

		// calc total matriculations
		total_matrics[sl] += ed->matrics;
	    }

	    // calc number of top athletes *newly* recruited
	    // which is the number of matriculants in stdent segment 4 (athlete)
	    matrics_top_athletes += enroll_data_sl1[stuSubSeg][3].matrics;

	    if(is_male_group(stuSubSeg))
		male_matrics_top_athletes += enroll_data_sl1[stuSubSeg][3].matrics;
	}
    }

    for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ ) {
	for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
	    ed = &enroll_data_sl1[stuSubSeg][stuSeg];

	    if ( total_matrics[sl] > 0 )
		ed->fraction_matrics = float(ed->matrics) / total_matrics[sl];
	    else
		ed->fraction_matrics = 0;
	}
    }
}

//------------- End of function EnrollRes::calc_enroll_matriculations --------//
//------------- End of function EnrollRes::calc_enroll_applications --------//

//----------- Begin of function EnrollRes::calc_enroll_offers --------//
//!
//! Refer to techdoc 3.4 section 2.3
//! and the mathca file HE.EnrMgt.rev.nb: "Traditional Undergraduates: sL-1: Admissions"
//!
void EnrollRes::calc_enroll_offers() {
    // for sl1 temporarily
    const int sl = 0;
    int stuSubSeg, stuSeg;
    EnrollDataSL1* ed;
    float sum=0;
    total_offers[sl] = 0;

    //PeerSchool* player = school_res.player_peer_school;
    int targetIntake = target_student_intake[sl];

    // calc sum
    for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
	for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
	    ed = &enroll_data_sl1[stuSubSeg][stuSeg];
	    sum += ed->yield_rate * ed->applications;
	}

    // check the calculated sum: (* If applications allow selectivity do QP, else admit all applicants. *)
    if ( sum <= targetIntake ) {
	// admit all applicants
	//
	for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
	    for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
		ed = &enroll_data_sl1[stuSubSeg][stuSeg];
		ed->offers = ed->applications;
		total_offers[sl] += ed->offers;
	    }
    }
    else {
	// use quadratic program to solve
	//
	const int size = STUDENT_SUBSEGMENT_COUNT*STUDENT_SEGMENT_COUNT;

	DiagonalMatrix  mQ(size);
	ColumnVector  mQdiag(size);
	DiagonalMatrix I(size);       I = 1;
	Matrix mA = I;
	RowVector vA(size);
	ColumnVector  vc(size);
	ColumnVector  vb(size+1);                     //bug
	ColumnVector  vtmp(1);        vtmp(1) = targetIntake;
	ColumnVector  vOffers(size);

	const int K_FACTOR = 5;
	int i=1;

	// init mQdiag, vA, vc
	for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
	    for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
		ed = &enroll_data_sl1[stuSubSeg][stuSeg];
		mQdiag(i) = ed->applications;
		vA(i) = ed->yield_rate;
		vc(i) = - ed->util - K_FACTOR;
		i++;
	    }

	// init mQ, mA, vb
	vb = mQdiag & vtmp;
	mA &= vA;
	mA *= -1; vb *= -1;

	for (i=1; i<=size; i++)
	    mQdiag(i) = 2 * K_FACTOR / max(mQdiag(i), 0.001);
	mQ.set_diagonal(mQdiag);

	// here the main call
	if ( !LinearAlgebra::quadratic_prog(vc,mQ,mA,vb,vOffers) ) {
	    vOffers = 0;
	}

	// "Partition" vOffers (a list) to ed->offers (list in list)
	int offs;
	i =1;

	for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
	    for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
		offs = enroll_data_sl1[stuSubSeg][stuSeg].offers = int( vOffers(i++) +0.5);
		total_offers[sl] += offs;
	    }
    }
    /*
      y as prior  year's yield rates
      z as number of applications
      eU is expected (average)  utility
    */
}

//------------- End of function EnrollRes::calc_enroll_offers --------//

//----------- Begin of function PeerSchool::calc_enroll_aid --------//
//!
//! Refer to techdoc 3.4 section 2.4
//! and the mathca file HE.EnrMgt.rev.nb: "Traditional Undergraduates: sL-1: Financial Aid: Initialization"
//! calc nL, nH and mL, mH for student generation sims later
//! That is, lower and upper bounds for need and merit need
//! (* Needs to be run whenever Player changes any aid parameter *)
//!
//!
void EnrollRes::calc_enroll_aid() {
    // for sl1 temporarily
    const int sl = 0;

    //-------------------//
    //---- Need Aid -----//
    //-------------------//

    float x = 60, lastDeviate, deviate;
    float inc = x / 100.f;

    // nH = FindMinimum[(needFrObj[sL1,x]-frOfTradUGsWithNeed)^2,{x,60}][[2,1,2]];
    // FindMinimum is used in lieu of Golden Section
    //

    if ( calc_need_aid(x+inc) > calc_need_aid(x-inc) )
	inc *= -1;

    deviate = calc_need_aid(x);

    do {
	x += inc;
	lastDeviate = deviate;
	deviate = calc_need_aid(x);
    }
    while ( deviate < lastDeviate && deviate > 0 );

    need_upper = x;
    need_lower = need_upper - max_aid;

    //-------------------//
    //---- Merit Aid ----//
    //-------------------//

    adjust_fraction_merit_aid(fraction_sl1_offered_merit_aid);

    // mL = FindMinimum[(meritFrObj[sL1,x]-frStudentsOfferedMeritAid)^2,{x,5}][[2,1,2]];
    x = 5;
    inc = x / 100.f;

    if ( calc_merit_aid(x+inc, fraction_sl1_offered_merit_aid)
	 > calc_merit_aid(x-inc, fraction_sl1_offered_merit_aid) )
	inc *= -1;

    deviate = calc_merit_aid(x, fraction_sl1_offered_merit_aid);

    do {
	x += inc;
	lastDeviate = deviate;
	deviate = calc_merit_aid(x, fraction_sl1_offered_merit_aid);
    }
    while ( deviate < lastDeviate && deviate > 0 );

    merit_lower[sl] = x;

    // mH = FindMinimum[(meritFrObj[sL1,x]-frMeritTop)^2,{x,6}][[2,1,2]];
    x = 6;
    inc = x / 100.f;

    if ( calc_merit_aid(x+inc, fraction_merit_top)
	 > calc_merit_aid(x-inc, fraction_merit_top) )
	inc *= -1;

    deviate = calc_merit_aid(x, fraction_merit_top);

    do {
	x += inc;
	lastDeviate = deviate;
	deviate = calc_merit_aid(x, fraction_merit_top);
    }
    while ( deviate < lastDeviate && deviate > 0 );

    merit_upper[sl] = x;
}

//------------- End of function EnrollRes::calc_enroll_aid --------//

//----------- Begin of function EnrollRes::calc_need_aid --------//
//!
//! Refer to techdoc 3.4 section 2.4
//! and the mathca file HE.EnrMgt.rev.nb: "Traditional Undergraduates: sL-1: Financial Aid: Initialization"
//!
float EnrollRes::calc_need_aid(float aid) {
    // for sl1 temporarily
    const int sl = 0;
    PeerSchool* player = school_res.player_peer_school;

    // (* Equals the max of % stu with need and % with need given need aid (cols DR & DS);
    //	taking the max is and approximation to mitigate data problems *)

    //min & max bug chea
    const float frOfTradUGsWithNeed = max(player->percent_freshmen_with_need, (float)(player->freshmen_offered_aid))  / 100.0f;

    // constant mulitpliers
    // refer also to "Genral: helper function: Minority, athletics, and non-traditional multipliers"

    float deviate = 0, cdfTmp;

    float aidMinMultiplier;
    int subSeg, seg;
    //EnrollDataSL1	*ed;
    Talent    *talentPtr;

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	if ( subSeg == NONMINORITY_MALE || subSeg == NONMINORITY_FEMALE )
	    aidMinMultiplier = 1.0f;
	else
	    aidMinMultiplier = aidMinMultipliers[minority_aid_special];

	for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	    //ed = &enroll_data_sl1[subSeg][seg];
	    talentPtr = &(enroll_res.talent_array[subSeg][seg][INCOME]);

	    cdfTmp = math.get_random_gamma_CDF( aidMinMultiplier * talentPtr->average,
						PSCH_SD(talentPtr->variance), aid );
	    deviate += cdfTmp * enroll_data_sl1[subSeg][seg].fraction_matrics;
	}
    }

    return math.safe_pow(deviate - frOfTradUGsWithNeed, 2);

    /*
      nH = FindMinimum[(needFrObj[sL1,x]-frOfTradUGsWithNeed)^2,{x,60}][[2,1,2]];

      nL = nH-maxAid;

      needFrObj[sL1,nH_] := Plus@@Flatten[needFrObj[sL1,nH,#]&/@gegs];
      needFrObj[sL1,nH_,geg_] := With[{cdfTmp = cdf[inc,geg,nH]},
      cdfTmp[[#]]*frPriorMatrics[geg][[#]]&/@Range[7]];

      cdf[var_,geg_,x_] :=
      gammaCDF[minMultF[aid,geg]*means[var,geg][[#]],
      variances[var,geg][[#]],
      x]&/@Range[7];
    */
}

//------------- End of function EnrollRes::calc_need_aid --------//

//----------- Begin of function EnrollRes::calc_merit_aid --------//
//!
//! Refer to techdoc 3.4 section 2.4
//! and the mathca file HE.EnrMgt.rev.nb: "Traditional Undergraduates: sL-1: Financial Aid: Initialization"
//!
float EnrollRes::calc_merit_aid(float aid, float base) {
    // for sl1 temporarily
    const int sl = 0;

    // constant mulitpliers
    // refer also to "Genral: helper function: Minority, athletics, and non-traditional multipliers"

    float deviate = 0, cdfTmp;

    float aidMinMultiplier;
    int subSeg, seg;
    EnrollDataSL1 *ed;
    //Talent		*talentPtr;

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	if ( subSeg == NONMINORITY_MALE || subSeg == NONMINORITY_FEMALE )
	    aidMinMultiplier = 1.0f;
	else
	    aidMinMultiplier = aidMinMultipliers[minority_aid_special];

	for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	    ed = &enroll_data_sl1[subSeg][seg];

	    cdfTmp = 1 - math.get_random_gamma_CDF( aidMinMultiplier * ed->util,
						    PSCH_SD(ed->util_variance), aid );

	    deviate += cdfTmp * enroll_data_sl1[subSeg][seg].fraction_matrics;
	}
    }

    return math.safe_pow(deviate - base, 2);

    /*	
	meritFrObj[sL1,mL_] := Plus@@Flatten[meritFrObj[sL1,mL,#]&/@gegs];
	meritFrObj[sL1,mL_,geg_] := With[{cdfTmp = 1-cdf[util,geg,mL]},
	cdfTmp[[#]]*frPriorMatrics[geg][[#]]&/@Range[7]];
    */
}

//------------- End of function EnrollRes::calc_merit_aid --------//

//----------- Begin of function EnrollRes::player_change_offers_priority --------//
//!
//! Refer to the mathca file HE.EnrMgt.rev.nb: "Genral: helper function: utility function"
//! Called from interface after player_change_offers_priority
//! Recalculate util and util_variance for subseg and seg
//!
void EnrollRes::player_change_offers_priority() {
    // for sl1 temporarily
    const int sl = 0;

    // constant mulitpliers
    // refer also to "Genral: helper function: Minority, athletics, and non-traditional multipliers"
    //
    // (* these respresent the "boost" *)
    const float admMinMultipliers[PRIORITY_LEVEL_COUNT] = {
	1.0f, 2.0f, 3.0f
    };

    int subSeg, seg, var;
    float admMinMultiplier;
    EnrollDataSL1 *ed;

    // update utility on player input
    //
    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	if ( subSeg == NONMINORITY_MALE || subSeg == NONMINORITY_FEMALE )
	    admMinMultiplier = 1.0f;
	else
	    admMinMultiplier = admMinMultipliers[minority_offers_special];

	for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	    ed = &enroll_data_sl1[subSeg][seg];
	    ed->util = ed->util_variance = 0.0f;

	    for ( var=0; var<TALENT_VAR_COUNT; var++ ) {
		ed->util += float(offers_priority[var]) / MAX_TALENT_PRIORITY_LEVEL * admMinMultiplier * enroll_res.talent_array[subSeg][seg][var].average;
		ed->util_variance += math.safe_pow(offers_priority[var] / MAX_TALENT_PRIORITY_LEVEL,2) * enroll_res.talent_array[subSeg][seg][var].variance;
	    }
	}
    }
}

//------------- End of function EnrollRes::player_change_offers_priority --------//

//----------- Begin of function EnrollRes::calc_doctoral_intake_target --------//
//!
void EnrollRes::calc_doctoral_intake_target() {
    // 0113
    if ( player_school.doctoral_program_intensity == 0 ) {
	target_student_intake[DOCTOR] = 0;
	return;
    }

    int i,j;
    int targetTotal = 0;
    CourseArray* courArr;
    Department* deptPtr;

    for ( i=department_array.size(); i>0; i-- ) {
	if ( department_array.is_deleted(i) )
	    continue;

	deptPtr = department_array[i];

	int totalBreakoutSections = 0;
	courArr = &(department_array[i]->course_array);

	for (j=courArr->size(); j>0; j--) {
	    if ( courArr->operator[](j)->teaching_method == BREAKOUT_LAB )
		totalBreakoutSections++;
	}

	targetTotal += (int) max(deptPtr->doctors_per_research_dollar * deptPtr->total_research_dollar_direct * 1000,
				 deptPtr->init_doctor_count + deptPtr->breakout_section_by_doctor * (totalBreakoutSections - deptPtr->init_breakout_section_count));
    }

    target_student_intake[DOCTOR] = max(0, targetTotal - student_count[DOCTOR]);

    if ( target_student_intake[DOCTOR] > 3000 )
	target_student_intake[DOCTOR] = 3000;         // possibly above has bug
    //	else if ( target_student_intake[DOCTOR] <= 0 )
    //		target_student_intake[DOCTOR] = target_student_intake[MASTER];

    err_when(targetTotal < 0);

    //-------- fix: use the updated target_student_intake[] to recalculate base_apps --------//

    if( info.prerun_year==1 ) {
	EnrollData* edSL;
	int sl=3;

	// 0.3 -> 30%
	float studentGenderPct[STUDENT_SUBSEGMENT_COUNT];

	err_when(GENDER_ETHNIC_TYPE_COUNT != STUDENT_SUBSEGMENT_COUNT);
	float femalePercent = student_female_percent[sl] / 100.0f;
	float minorityPercent = student_minority_percent[sl] / 100.0f;

	//askbill inital gender percent affect future enrollment?  BUG? = 0 when sl=5
	studentGenderPct[MINORITY_FEMALE] = femalePercent*minorityPercent;
	studentGenderPct[NONMINORITY_FEMALE] = femalePercent*(1.0f - minorityPercent);
	studentGenderPct[MINORITY_MALE] = (1.0f - femalePercent)*minorityPercent;
	studentGenderPct[NONMINORITY_MALE] = (1.0f - femalePercent)*(1.0f - minorityPercent);

	for ( int subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	    //--------- calc enroll_data_sls[][].apps_ratio, applications and conversion_rate ---------//
	    edSL = &enroll_data_sls[sl-1][subSeg];

	    edSL->base_apps
		// 0.5: rounding
		= int(edSL->apps_ratio * studentGenderPct[subSeg] * target_student_intake[sl] + 0.5);
	}
    }
}

//------------- End of function EnrollRes::calc_doctoral_intake_target --------//

//----------- Begin of function EnrollRes::calc_app_multiplier --------//
//!
void EnrollRes::calc_app_multiplier(char sl) {
    const int VAR_COUNT = 3;
    float input[VAR_COUNT];
    int i;

    err_when(UG_TRADITION != 0);
    err_when(sl>=MAX_STUDENT_LEVEL);

    if ( sl == DISTANCE_LEARN ) {
	float in=0;

	for (int m=THIS_MONTH; m>=THIS_MONTH-info.graph_month_passed+1; m--)
	    in += library_tech_office.tech_utilization_history[m];

	//## org.
	in = math.single_response_func(0.0f, 1.5f, 10.0f, 5.0f, in/100);

	//		app_multiplier[sl] = math.latency_func(0.3f, app_multiplier[sl], in);
	//## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
	app_multiplier[sl] = player_school.latency_func(0.25f, app_multiplier[sl], in);

    }
    else {
	// Applications multiplier: on-campus students // 1009 response_func
	//

	memset(input, 0, sizeof(input));

	//-------------------//
	input[0] = school_res.player_peer_school->prestige;

	//-------------------//
	// (minor bug) should depend on [sl]
	input[1] = player_school.satisfaction_overall[THIS_MONTH];

	//-------------------//
	int totalTargetIntake = 0;

	for (i=0; i<MAX_STUDENT_LEVEL; i++)
	    totalTargetIntake += target_student_intake[i];

	//min & max bug chea
	input[2] = max(1000.0f, float(finance.projected_expense_array[AC_ENROLLMENT_MANAGEMENT].this_year.total)) / totalTargetIntake;

	if ( input[2] == 0 )
	    input[2] = 1;

	input[2] *= (float) finance.cost_rise_policy_array[PL_ENROLLMENT_MANAGEMENT].penalty_multiplier1;

	//-------------------//
	float weight[VAR_COUNT] = { 0.35f, 0.3f, 0.35f };
	float newValue = 0;
	for (i=0; i<VAR_COUNT; i++)
	    newValue += input[i] * weight[i];

	//-------------------//

	//## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
	app_multiplier[sl] = max(0.7f,min(1.3f, player_school.latency_func(0.8f, app_multiplier[sl], newValue)));
    }

    //-------------------//
    // calc yield_multiplier
    //-------------------//
    float weight[VAR_COUNT] = { 0.35f, 0.5f, 0.15f};
    float newValue = 0;

    for (i=0; i<VAR_COUNT; i++)
	newValue += input[i] * weight[i];

    //-------------------//

    //## chea 300899 change latency_func(HE.RespF_Defs.Rev 230899 E-mail)
    yield_multiplier[sl] = max(0.7f,min(1.3f, player_school.latency_func(0.80f, yield_multiplier[sl], newValue)));

}

//----------- End of function EnrollRes::calc_app_multiplier --------//

//----------- Begin of function EnrollRes::calc_post_enroll --------//
//!
//! ----- generate student update values for if display or next enrollment -----//
//!
void EnrollRes::calc_post_enroll() {
    // for sl1 temporarily
    const int sl = UG_TRADITION;                    // 0305

    //-------------------------//
    //		 generate student		//
    //-------------------------//
    // refer to td3.4 section 2.7

    char subSeg, seg;
    int newStudentCount, i, t, aid=0;

    // temp vars storing new sim properties
    float talentArr[TALENT_VAR_COUNT];
    int maleCount=0, femaleCount=0;

    avg_athletic_talent_male_matrics = 0;
    avg_athletic_talent_female_matrics = 0;

    for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	    newStudentCount = enroll_data_sl1[subSeg][seg].matrics;

	    for (i=0; i<newStudentCount; i++) {
		//---- Generate student; see also PlayerSchool::generate_student()
		//
		// year 1 ug student without major

		char subSegStu = subSeg;

		if ( info.year_passed == 1 && player_school.scenario_id == SCN_STUDENT_DIVERSITY ) {
		    if ( is_minority_group(subSeg) && math.get_random_float () > 0.571f ) {
			if (subSegStu == MINORITY_MALE )
			    subSegStu = NONMINORITY_MALE;
			else
			    subSegStu = NONMINORITY_FEMALE;
		    }
		}

		calc_a_talent(talentArr, sl,subSegStu,seg);

		department_res.general_dept.student_array.add( sl, 1, subSegStu, seg, 0, aid, talentArr );
		//				enroll_res.total_matrics++;

		// update vars for calc average later
		//
		for (t=0; t<TALENT_VAR_COUNT; t++)
		    talent_average[sl][t] += int( 10 * talentArr[t] );

		if ( is_male_group(subSegStu) ) {
		    avg_athletic_talent_male_matrics = (char) talentArr[ATHLETIC];
		    maleCount++;
		}
		else {
		    avg_athletic_talent_female_matrics = (char) talentArr[ATHLETIC];
		    femaleCount++;
		}

	    }                                           // for studentCount
	}
    }

    if ( maleCount )
	avg_athletic_talent_male_matrics /= maleCount;

    if ( femaleCount )
	avg_athletic_talent_female_matrics /= femaleCount;

    if ( sys.debug_session ) {
	// audit_flag
	for ( ;; ) {
	    if ( TRADIT_COUNT == MAX_TRADITION_COUNT )
		return;

	    GeneralDepartment* deptPtr;

	    deptPtr = &department_res.general_dept;

	    int stuRecno = m.random(deptPtr->student_array.size())+1;

	    if ( deptPtr->student_array.is_deleted( stuRecno ) )
		continue;

	    Student* stdPtr = deptPtr->student_array[stuRecno];

	    if ( stdPtr->audit_flag )
		continue;

	    TRADIT_COUNT++;

	    stdPtr->audit_flag = 1;

	    stdPtr->old_student_recno = stdPtr->student_recno;

	    File file;

	    char fileName[123];

	    strcpy( fileName, "TRADITION" );
	    strcat( fileName, m.format(stdPtr->old_student_recno) );
	    strcat( fileName, ".txt" );

	    file.file_create(fileName);

	    WRITE_STR_FIELD( (&file), "Cohort" );
	    WRITE_FIELD_SEPARATOR( (&file) );
	    WRITE_STR_FIELD( (&file), "Trimester" );
	    WRITE_FIELD_SEPARATOR( (&file) );
	    WRITE_STR_FIELD( (&file), "ID" );
	    WRITE_FIELD_SEPARATOR( (&file) );
	    WRITE_STR_FIELD( (&file), "Variable" );
	    WRITE_FIELD_SEPARATOR( (&file) );
	    WRITE_STR_FIELD( (&file), "Trimester" );
	    WRITE_RECORD_SEPARATOR( (&file) );

	    for ( int i=0; i<4; i++ ) {
		WRITE_NUM_FIELD( (&file), -1 );
		WRITE_FIELD_SEPARATOR( (&file) );
		WRITE_STR_FIELD( (&file), "A" );
		WRITE_FIELD_SEPARATOR( (&file) );
		WRITE_FIELD_SEPARATOR( (&file) );
		WRITE_STR_FIELD( (&file), "S" );
		WRITE_FIELD_SEPARATOR( (&file) );

		switch (i) {
		case 0: WRITE_STR_FIELD( (&file), "Courses" );
		    WRITE_FIELD_SEPARATOR( (&file) );
		    WRITE_FIELD_SEPARATOR( (&file) );
		    WRITE_NUM_FIELD( (&file), stdPtr->course_per_trimester[0] );
		    WRITE_NUM_FIELD( (&file), stdPtr->course_per_trimester[1] );
		    WRITE_NUM_FIELD( (&file), stdPtr->course_per_trimester[2] );
		    break;

		case 1: WRITE_STR_FIELD( (&file), "Status" );
		    WRITE_FIELD_SEPARATOR( (&file) );
		    WRITE_FIELD_SEPARATOR( (&file) );
		    if ( stdPtr->department_recno ) {
			WRITE_STR_FIELD( (&file), department_res[department_array[stdPtr->department_recno]->department_id]->name );
		    }
		    else
			WRITE_STR_FIELD( (&file), "Undec" );
		    break;

		case 2: WRITE_STR_FIELD( (&file), "AcadTalent" );
		    WRITE_FIELD_SEPARATOR( (&file) );
		    WRITE_NUM_FIELD( (&file), stdPtr->talent_academic );
		    break;
		case 3: WRITE_STR_FIELD( (&file), "GenEnth" );
		    WRITE_FIELD_SEPARATOR( (&file) );
		    WRITE_FIELD_SEPARATOR( (&file) );
		    switch( stdPtr->gender_ethnic_group ) {
		    case NONMINORITY_MALE:    WRITE_STR_FIELD( (&file), "NM" ); break;
		    case NONMINORITY_FEMALE:  WRITE_STR_FIELD( (&file), "NF" ); break;
		    case MINORITY_MALE:     WRITE_STR_FIELD( (&file), "MM" ); break;
		    case MINORITY_FEMALE:   WRITE_STR_FIELD( (&file), "MF" ); break;
		    }
		    break;
		}
		WRITE_RECORD_SEPARATOR( (&file) );
	    }

	    file.file_close();
	}
    }

}

//------------- End of function EnrollRes::calc_post_enroll --------//

//------------- End of function EnrollRes::calc_post_enroll_adjust_aid_all_sls --------//
//
//
void EnrollRes::calc_post_enroll_adjust_aid_all_sls() {

}

//------------- End of function EnrollRes::calc_post_enroll_adjust_aid_all_sls --------//

//----------- Begin of function EnrollRes::init_data_sl1 --------//
//!
//! Refer to [HE.initialization]Apps_Yield and techdoc 3.4 section 2.1
//! and the mathca file HE.EnrMgt.rev.nb: "Traditional Undergraduates: sL-1: Data: Test admissions data"
//! Init member enroll_data
//!
void EnrollRes::init_data_sl1() {
    // for sl1 temporarily
    const int sl = 0;

    // -------- update target_student_intake -------- //
    // init target_student_intake for interface
    //
    // "The intake target is a player decision"
    // the *initialization* value can be obtained from "he.gdb.init:student_pars" by
    // multiplying the percent of students in the first year times the total
    // number of students for the student level. eg, for traditional
    // undergraduates (sl-1), it's ab19*c8.

    //optional // 0304
    memset(target_student_intake, 0, sizeof(target_student_intake));

    for (char s=0; s<=DOCTOR; s++) {
	/*		if( s <= UG_NONTRADITION )
			{
			target_student_intake[s]
			= int(student_count[s] * player_school.year_in_program_pct[s][0]);	// 0: year 1
			}
			else */

	if ( s <= MASTER ) {
	    //--- the current student_count[] is the total number of student excluding year 0 student.We divide it by (1 - percent) to restore the total number including year 0 student.

	    target_student_intake[s] = int(
		(float) student_count[s] / (1.0f - player_school.year_in_program_pct[s][0])
		* player_school.year_in_program_pct[s][0] );
	}
	else if ( s == DOCTOR ) {
	    float total = 0;

	    for ( int d=department_array.size(); d>0; d--) {
		total += department_res[department_array[d]->department_id]->doctor_year_in_program_pct[0];
	    }

	    total /= department_array.size();

	    target_student_intake[s] = int( student_count[s] * total );
	}
    }

    target_student_intake[DISTANCE_LEARN] = target_student_intake[UG_NONTRADITION];

    int initial_intake = int(1.05 * target_student_intake[sl] + 0.5);

    err_when(initial_intake==0);
    /*
      // 990525
      for (s=0; s<MAX_STUDENT_LEVEL; s++)
      target_student_intake[s] = min( target_student_intake[s], student_count[s] );
    */
    //----------------------------------//

    PeerSchool* player = school_res.player_peer_school;
    int stuSubSeg, stuSeg, i=0;

    // update vars before calc_student_pref_vars_array()
    applications_rate[sl] = player->ug_applications_rate;
    cur_yield_rate[sl] = player->ug_yield_rate / 100.0f;
    //offers_rate[sl]

    // call twice in order to update the array pref_vars_array_last
    player->calc_student_pref_vars_array();
    player->calc_student_pref_vars_array();

    calc_student_pref();
    calc_app_yield();                               // calc enroll_data_sl1's members apps_ratio, yield_rate;

    // calc enroll_data's memebers
    //
    for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ ) {
	for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) {
	    EnrollDataSL1 * ed = &enroll_data_sl1[stuSubSeg][stuSeg];

	    ed->applications = int(ed->apps_ratio * ed->fraction_matrics * initial_intake + 0.5);
	    ed->base_apps = ed->applications;

	    ed->base_yield_rate = ed->yield_rate;
	    ed->offers = ed->matrics = 0;

	    err_when(ed->base_apps < 0);
	}
    }

    //---------------------------------------------------------------//
    // set member variables for default player settings
    //
    // init player input here

    // for calc_offers
    switch ( player_school.athletic_program_intensity ) {
    case INPUT_LOW:   offers_priority[2] = 3; break;
    case INPUT_MEDIUM:  offers_priority[2] = 5; break;
    case INPUT_HIGH:    offers_priority[2] = 7; break;
    }
    switch( player_school.prestige ) {
    case 1: offers_priority[0] = offers_priority[1] = 3; break;
    case 3: offers_priority[0] = offers_priority[1] = 5; break;
    case 10: offers_priority[0] = offers_priority[1] = 7;break;
    }

    minority_offers_special = SET_LOW;
    athlete_offers_special = SET_LOW;
    player_change_offers_priority();                // called before calc_enroll_aid() below

    // for calc_aid
    fraction_sl1_offered_merit_aid = 0.25f;         //
    fraction_sl2_offered_aid = 0.0f;                // 990407 0.2f
    minority_aid_special = SET_LOW;
    athlete_aid_special = SET_LOW;
    calc_enroll_aid();

    // for gen sims
    fraction_need_covered = 0.10f;                  // 990407 0.5f
    aid_budget_enforced = false;                    // 0405 = true
    // aidBudget					= 8000; (* from financial module; $000 *)

    //---------------------------------------------------------------//

    //-------- for calc_matrics ----------//
    // init this from base class School/SchoolEx
    // frOnAid[init]				= 0.63;  (* cell DQ1238 in 'Initial_Conditions' *)
    // aveAid[init]				= 4089 * (1.04)^5; (* cell DU updated from 1994 *)
    // PeerSchool* player = school_res.player_peer_school;

    // cell DQ
    float overAllFraction = player->percent_ug_students_on_aid;
    // cell DU
    float overAllAverage  = (float) player->institutional_aid_per_fte;

    fraction_on_aid[NON_MINORITY] = overAllFraction;// 0.63f;
    fraction_on_aid[MINORITY] = overAllFraction;    //0.2f;

    // 5000;
    average_aid[NON_MINORITY] = (int) overAllAverage;
    average_aid[MINORITY] = (int) overAllAverage;

    //---------------------------------------------------------------//

    switch ( player_school.scenario_id ) {
    case ( SCN_WINNING_ATHLETICS ):
	offers_priority[ATHLETIC] = math.get_random_snd(2, PSCH_SD(0.5f));
	athlete_offers_special = SET_LOW;
	athlete_aid_special = SET_LOW;
	break;
    }
}

//------------- End of function EnrollRes::init_data_sl1 --------//

//----------- Begin of function EnrollRes::init_data --------//
//!
//! must run after school_res.build_peer_school_array()
//!
//! player_school.generate_student() will calc student count and fin aid
//! so that method should be called BEFORE this one
//!
void EnrollRes::init_data() {
    int progressIndex = 70;

    calc_student_count();

    int schoolCount = school_res.peer_school_count;

    // array includes PGI and peer schools
    PeerSchool *peerSch = school_res.peer_school_array;

    for (int i=0; i<schoolCount; i++, peerSch++) {
	peerSch->calc_student_pref_vars_array();
	peerSch->calc_student_pref_vars_array();
    }

    // peerSch = school_res.player_peer_school;

    // call twice to update the array pref_vars_average_array_last
    PeerSchool::calc_student_pref_vars_average_array();
    PeerSchool::calc_student_pref_vars_average_array();

    // simulate player school ***ONLY***

    //askbill do? td3.4 section 3.4: Model Initialization for all sl

    PeerSchool *playerSchool = school_res.player_peer_school;
    // 0118 for calc_aid
    student_office.room_n_board_rate = playerSchool->room_and_board_rate;

    init_data_sl1();

    for (char sl=UG_NONTRADITION; sl<MAX_STUDENT_LEVEL; sl++) {
	init_data_sls(sl);
    }

    calc_talent_student_db();
}

//------------- End of function EnrollRes::init_data --------//

//----------- Begin of function EnrollRes::enroll_main --------//
//!
//! enrollment management model: run at the begining of each academic year
//!
//! [int] initFlag=0 by default
//!
void EnrollRes::enroll_main(int initFlag) {
    // assume PGI's PeerSchool (school_res.player_peer_school) is pointing
    // to peer_school_array in school_res.build_peer_school
    //
    int i, schoolCount = school_res.peer_school_count;
    PeerSchool *peerSch = school_res.peer_school_array;

    // pre-calculation for peerSch->calc_enroll_*
    //
    for (i=0; i<schoolCount; i++, peerSch++)        // PGI and/or peer schools
	peerSch->calc_student_pref_vars_array();

    // pref_vars_average_array is useful for calc_enroll_apps&matrics()
    PeerSchool::calc_student_pref_vars_average_array();

    //------- main calls --------//

    // initialize intermediate variables

    //peerSch = school_res.player_peer_school;
    //peerSch->calc_student_pref_vars_array();	// called above

    // (* Equals tuition rate + room and board rate (cols BE & BF) + an
    // allowance for incidentials (=$2,000), adjusted to thousands of dollars. *)
    PeerSchool* player = school_res.player_peer_school;

    //990408 max_aid = ( finance.tuition_rate + student_office.room_n_board_rate + 2000 ) / 1000.0f;		//
    max_aid = ( finance.tuition_rate ) / 1000.0f;
    total_aid = 0;                                  // this will be calculated in this function

    // for IF
    // percent_minority = 0;
    memset(talent_average, 0, sizeof(talent_average));

    //-------- update for player input ---------//

    calc_doctoral_intake_target();
    player_change_offers_priority();
    calc_enroll_aid();

    for (char sl=UG_NONTRADITION; sl<MAX_STUDENT_LEVEL; sl++) {
	calc_enroll_aid_sls(sl);
    }

    //-------- update pi for enroll ---------//

    for (int sl=0; sl<MAX_STUDENT_LEVEL; sl++)
	calc_app_multiplier(sl);

    //------------- SL1 -------------//
    // main procedures of enrollment
    calc_enroll_applications();
    calc_enroll_offers();
    calc_enroll_matriculations();
    calc_post_enroll();

    //------------- SL2-5 -------------//

    for ( int sl=UG_NONTRADITION; sl<MAX_STUDENT_LEVEL; sl++) {
	if ( sl==DISTANCE_LEARN )
	    continue;
	enroll_main_sls(sl);
	calc_post_enroll_sls(sl);
    }

    calc_student_count();
    // will update this two once a year
    if ( (info.game_month == 9) && (info.game_day == 1) ) {
	for ( int i=0; i<MAX_STUDENT_LEVEL-1; i++ ) {
	    last_year_student_count[i] = new_year_student_count[i];
	    new_year_student_count[i] = student_count[i];

	    if ( info.prerun_year && info.game_year == 1 )
		target_enrollment[i] = new_year_student_count[i];
	    // for year 1 only copy doctor
	    if ( info.prerun_year && info.game_year == 2 && i == DOCTOR )
		target_enrollment[i] = new_year_student_count[i];
	}
    }
    calc_total_aid();
    //calc_post_enroll_adjust_aid_all_sls();

    err_when(total_aid > max_aid * total_student_count);
    update_enroll_history();

    //---------------------------------//

    if ( initFlag > 0 && player_school.scenario_id == SCN_STUDENT_DIVERSITY ) {
	player_school.scenario_base = overall_percent_minority;
	player_school.scenario_target[0] = min(100, int(5.0f * overall_percent_minority));
	player_school.scenario_max_game_years = 7;
    }
}

//------------- End of function EnrollRes::enroll_main --------//
