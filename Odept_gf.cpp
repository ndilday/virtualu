//Filename   : ODEPT_GF.CPP
//Description: functions for generating faculty records in the department class

#include <omath.h>
#include <OINFO.H>
#include <OFACURES.H>
#include <ODEPTRES.H>
#include <OFACULTY.H>                             //##### fred 0919 #####//
#include <OSCHLRES.H>
#include <OPSCHOOL.H>
#include <OPEERSCH.H>                             //##### fred 0714 #####//
#include <OFINANCE.H>
#include <ODEPT.H>
#include <ODATE.H>

#define DEBUG_VC
#ifdef DEBUG_VC
#include <OLOG.H>                                 // for debug matrix
#include <STDIO.H>                                //temp for debug matrix
#endif

//-------- Define struct RankAgeGroup ---------//

//!Structure containing the age ranges corresponding to a particular
//!faculty rank for each department.
struct RankAgeGroup {
    char rank_id;
    char min_age;
    char max_age;

    //---- vars for storing intermediate results ---//

    int  research_per_faculty;
};

//------- Define static vars -----------//

static RankAgeGroup rank_age_group_array[MAX_RANK_AGE_GROUP] = {
    { ASST_PROF , 28, 33 },
    { ASSOC_PROF, 34, 40 }, {
	//## chea 221199 use the old rank
	FULL_PROF , 41, 50
    },
    { FULL_PROF , 51, 60 },
    { FULL_PROF , 61, 70 },
    { LONG_TERM_ADJUNCT, 34, 70 },
    { SHORT_TERM_ADJUNCT, 34, 50 },

    //	{ ASST_PROF , 27, 32 },
    //	{ ASSOC_PROF, 33, 39 },
    //	{ FULL_PROF , 40, 49 },
    //	{ FULL_PROF , 50, 59 },
    //	{ FULL_PROF , 60, 70 },
    //	{ LONG_TERM_ADJUNCT, 33, 70 },
    //	{ SHORT_TERM_ADJUNCT, 33, 49 },
};

static int   average_research_project_size;
static float total_faculty_to_be_added;           // the total number of faculty ought to be in this department
static float k1=1, k2=1;

static bool is_minority_group(char group) {
    return group == MINORITY_MALE || group == MINORITY_FEMALE;
}

//-------- Begin of function Faculty::rank_age_group -------//
//!
//! temporarily place this func here
//!
int Faculty::rank_age_group(char rank, int aage) {
    int rankAgeGroup;

    switch (rank) {
    case ASST_PROF:
	rankAgeGroup = 0;
	break;
    case ASSOC_PROF:
	rankAgeGroup = 1;
	break;
    case FULL_PROF:
	rankAgeGroup = 2;

	// Comment to avoid bug here
	//		if ( aage < rank_age_group_array[rankAgeGroup].min_age )
	//			err_here();

	if ( aage > rank_age_group_array[rankAgeGroup].max_age )
	    rankAgeGroup++;

	if ( aage > rank_age_group_array[rankAgeGroup].max_age )
	    rankAgeGroup++;

	err_when(rankAgeGroup > 4);                 // above may change
	break;
    case LONG_TERM_ADJUNCT:
	rankAgeGroup = 5;
	break;
    case SHORT_TERM_ADJUNCT:
	rankAgeGroup = 6;
	break;
    default:
	err_here();
	break;
    }

    return rankAgeGroup;
}

int Faculty::rank_age_group_2_rank(int rag) {
    return rank_age_group_array[rag].rank_id;
}

int Faculty::get_min_age(int rankAgeGroup) {
    return rank_age_group_array[rankAgeGroup].min_age;
}

static int is_regular_faculty(char rank) {
    return (rank==ASST_PROF || rank==ASSOC_PROF || rank==FULL_PROF);
}

bool Faculty::is_regular() {
    return is_regular_faculty(rank_level) == 1;
}

//---------- End of function Faculty::rank_age_group -----------//

//-------- Begin of function Department::generate_faculty -------//
//!
void Department::generate_faculty() {
    err_when(faculty_array.faculty_count != 0);

    generate_faculty_calc_vars();

    if ( total_faculty_to_be_added < 5 )            // 990414
	total_faculty_to_be_added += 5;

#ifdef DEBUG_VC

    float facultyCount[MAX_RANK_AGE_GROUP];
    float sum=0;

    for( int i=0 ; i<MAX_RANK_AGE_GROUP ; i++ ) {
	facultyCount[i] = (float) faculty_to_be_added(i);
	sum += facultyCount[i];

	err_when(facultyCount[i] < 0);
    }

    err_when(sum <= 0);

    for( int i=0 ; i<MAX_RANK_AGE_GROUP ; i++ ) {
	facultyCount[i] = (facultyCount[i]*total_faculty_to_be_added / sum);

	facultyCount[i] += 1.0f;                      // round up // 990414
    }

    for( int i=0 ; i<MAX_RANK_AGE_GROUP ; i++ )
	generate_faculty_one_group(i, (int) (facultyCount[i]) );

    //----------------------------------//
    // calc statistics

    // 1203
    /* Subject: Re: Q34. letter and hiring

       "Number of faculty on tenure line" equals the total number of assist,
       associate, and full professors (all three groups); i.e., it excludes the
       two adjunct categories.

       "number of faculty tenured" equals the total of associate and full
       professors: i.e., the above minus the assistant professors.

    */

    //## chea 221099 for testing only

    //	int test =0;
    //	for (i = faculty_array.size(); i>0; i--)
    //	{
    //		if (faculty_array.is_deleted(i))
    //			continue;

    //		test++;

    //		switch(faculty_array[i]->rank_level)
    //		{
    //		case ASSOC_PROF:
    //		case FULL_PROF:
    //			department_array.faculty_count_tenured++;
    //		case ASST_PROF:
    //			department_array.faculty_count_on_tenure_line++;
    //			break;
    //		}
    //	}

    int facCount = faculty_array.faculty_count;

    err_when(total_faculty_to_be_added > 0 && facCount <= 0);
    err_when(facCount >= 10.0 * total_faculty_to_be_added);

    char s[300];

    sprintf(s, "===> dept #%d has %d faculty instead of %f.",
	    department_recno, facCount, total_faculty_to_be_added );
    DEBUG_LOG(s);
#endif

}

//---------- End of function Department::generate_faculty -----------//

//-------- Begin of function Department::generate_faculty_one_group -------//
//!
void Department::generate_faculty_one_group(int rankAgeGroup, int facultyCount ) {
    int totalFacAdded = 0;
    int facutlyRecnoArray[300];                     // should use mem_add...

    err_when(facultyCount > 300 );

    // = faculty_to_be_added(rankAgeGroup);		// the number of faculty of this rank age group needed to be generated.

    //----- calculate the percent of minority female -----//

    DepartmentInfo* deptInfo = department_res[department_id];

    FacultyTemplate* facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_gender_ethnic, rankAgeGroup );

    //----- get female and minority percentages from player_school_ex -----//

    int curRank = rank_age_group_array[rankAgeGroup].rank_id;

    PeerSchool *plSch = school_res.player_peer_school;

    float femalePercent = plSch->school_faculty_array_ex[curRank].female_percent;
    float minorityPercent = plSch->school_faculty_array_ex[curRank].minority_percent ;

    float percent_faculty_array[GENDER_ETHNIC_TYPE_COUNT];

    //----- calculate percentage of minority female faculty -----//

    percent_faculty_array[MINORITY_FEMALE] = deptInfo->female_multiplier * deptInfo->minority_multiplier
	* facultyTemplate->female_multiplier * facultyTemplate->minority_multiplier
	* femalePercent * minorityPercent / 100
	/1.5f;                                        //## chea 221099 to pull down the minority rate

    //----- calculate percentage of minority male faculty -----//

    percent_faculty_array[MINORITY_MALE] = deptInfo->minority_multiplier
	* facultyTemplate->minority_multiplier
	* minorityPercent
	/3;                                           //## chea 221099 to pull down the minority rate

    //----- calculate percentage of non-minority female faculty -----//

    //## chea 061299 BUGHERE
    //	percent_faculty_array[NONMINORITY_FEMALE] = femalePercent;
    percent_faculty_array[NONMINORITY_FEMALE] = femalePercent - percent_faculty_array[MINORITY_FEMALE];

    //----- calculate percentage of non-minority male faculty -----//

    //## chea 061299 just for check sum
    percent_faculty_array[NONMINORITY_MALE] = 100-percent_faculty_array[NONMINORITY_FEMALE]
	-percent_faculty_array[MINORITY_MALE]-percent_faculty_array[MINORITY_FEMALE];

    //-------- calculate faculty salary --------//

    facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_salary, rankAgeGroup );

    int facultySalary = int( deptInfo->salary_multiplier * facultyTemplate->overall_salary_multiplier *
			     plSch->school_faculty_array_ex[curRank].salary );

    //------- calculate faculty talent indices ------//

    facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_talent, rankAgeGroup );

    float talentTeaching    = deptInfo->talent_teaching * facultyTemplate->talent_teaching_multiplier;
    //## chea 221099 talentTeaching had not been properly init yet! but if added here will only gen a group of fact.with the same teaching talent
    //   talentTeaching = 20.0f + m.random(79);
    //	if(talentTeaching>=100)
    //		talentTeaching =100;

    float talentScholarship = deptInfo->talent_scholarship * facultyTemplate->talent_scholarship_multiplier;
    float talentResearch    = deptInfo->talent_research * facultyTemplate->talent_research_multiplier;

    //	if (talentResearch==0)
    //	{
    //		int dfhd=0;
    //		talentResearch=13;
    //	}
    //------ calculate the number of minority female faculty -----//

    float addCount;                                 // number of faculty needed to be added
    int genderEth;

    for( int i=0 ; i<GENDER_ETHNIC_TYPE_COUNT ; i++ ) {

	//## chea 201299 unused_faculty_count_array not use here
	addCount = (float) facultyCount * percent_faculty_array[i] / 100 + faculty_res.unused_faculty_count_array[i];
	//		addCount = (float) facultyCount * percent_faculty_array[i] / 100;

	for( ; addCount>=1 ; addCount-- ) {
	    if ( totalFacAdded > 1.8 * facultyCount )   // 990417
		break;

	    //----- determine the age of the new faculty -----//

	    //## fred ##//
	    int facultyAge = rank_age_group_array[rankAgeGroup].min_age +
		m.random( rank_age_group_array[rankAgeGroup].max_age
			  //								  - rank_age_group_array[rankAgeGroup].min_age + 1 ); //## chea 221199 can't +1 because it may exceed the age group
			  - rank_age_group_array[rankAgeGroup].min_age );
	    //## fred ##//

	    //------ determine the starting teaching date -----//

	    //## chea old idea totally wrong
	    //		int startTeachingDate = info.game_start_date
	    //										- m.random( facultyAge - rank_age_group_array[rankAgeGroup].min_age ) * 365
	    //										- m.random( 365 );

	    //## chea 221199 code 1.8.1  wfm review: 20 Nov 99
	    int startTeachingDate = 0;
	    if(rankAgeGroup==0)
		startTeachingDate = info.game_start_date - (1+m.random(6))*365;
	    else if(rankAgeGroup==1)
		startTeachingDate = info.game_start_date - (1+m.random(10))*365;
	    else if(rankAgeGroup==2)
		startTeachingDate = info.game_start_date - (1+m.random(10))*365;
	    else if(rankAgeGroup==3)
		startTeachingDate = info.game_start_date - (1+m.random(20))*365;
	    else if(rankAgeGroup==4)
		startTeachingDate = info.game_start_date - (1+m.random(30))*365;
	    else if(rankAgeGroup==5)
		startTeachingDate = info.game_start_date - (1+m.random(20))*365;
	    else
		//SHORT_TERM_ADJUNCT
		startTeachingDate = info.game_start_date - (1+m.random(5))*365;

	    //---- if the faculty is a female or a minority, apply the needed multiplier ----//

	    int thisFacultySalary = facultySalary;      // not ($000)

	    if( i==MINORITY_FEMALE || i==NONMINORITY_FEMALE )
		thisFacultySalary = int( (float)thisFacultySalary * facultyTemplate->female_salary_multiplier );

	    if( i==MINORITY_MALE || i==MINORITY_FEMALE )
		thisFacultySalary = int( (float)thisFacultySalary * facultyTemplate->minority_salary_multiplier );

	    thisFacultySalary = int(thisFacultySalary * math.get_random_snd(1, 0.015f));

	    err_when(thisFacultySalary<=0);

	    //--------- add a new faculty now ---------//

	    genderEth = i;

	    //askbill: do this for new fac sims OR(?) for ALL fac sim generated in a game battle
	    if ( player_school.scenario_id == SCN_FACULTY_DIVERSITY ) {
		if ( is_minority_group(genderEth) && math.get_random_float () > 0.667f ) {
		    if (genderEth == MINORITY_MALE )
			genderEth = NONMINORITY_MALE;
		    else
			genderEth = NONMINORITY_FEMALE;
		}
		if ( genderEth == NONMINORITY_FEMALE && math.get_random_float () > 0.80f )
		    genderEth = NONMINORITY_MALE;
		else if ( genderEth == MINORITY_FEMALE && math.get_random_float () > 0.80f )
		    genderEth = MINORITY_MALE;
	    }

	    int facultyRecno = faculty_array.add( department_recno, curRank, genderEth,
						  facultyAge, startTeachingDate, thisFacultySalary,
						  (int) talentTeaching, (int) talentScholarship, (int) talentResearch );

	    //--------- initialize pi (principal investigator) for this faculty -------//

	    Faculty* facultyPtr = faculty_array[facultyRecno];

	    facultyPtr->is_investigator = (math.get_random_float() < deptInfo->percent_pi_faculty * deptInfo->talent_research / 10000.0f);

	    //## chea 041199 new request
	    facultyPtr->proj_count_mult = (0.8f+0.4f*facultyPtr->talent_research/100);
	    facultyPtr->proj_size_mult = (0.7f+0.6f*facultyPtr->talent_research/100);
	    //facultyPtr->award_prob = (0.01f+0.5f*facultyPtr->talent_research/100);
	    // change in 18/03/2002
	    float oRate = finance.get_research_overhead_rate() / 100.0f;
	    float qualityDriver = facultyPtr->get_quality_driver();
	    float awardProb = 2.0f * (qualityDriver * 0.01f) / math.safe_pow( (1.0f + oRate), 3.0f );
	    facultyPtr->award_prob = m.fmin( 0.85f, m.fmax(0.10f, awardProb) );
	    //----------//

	    // 990412
	    facutlyRecnoArray[totalFacAdded] = facultyRecno;
	    totalFacAdded++;
	}

	// store the unused non-integer count for later use
	faculty_res.unused_faculty_count_array[i] = addCount;
    }

    //---------- 990412 ----------//

    // Revision to procedures for handling research projects and proposals (WFM: 6 April 1999); replaces procedures in HE.RespF.
    // k1 = 1/(sum over sims in k of proj_count_mult)
    // k2 = 1/(sum over sims in k of (proj_size_mult*k1* proj_count_mult))

    k1=0, k2=0;

    for ( int j=0; j<totalFacAdded; j++ ) {
	Faculty* facultyPtr = faculty_array[ facutlyRecnoArray[j] ];

	k1 += facultyPtr->proj_count_mult;
    }

    float test = (float)totalFacAdded;

    //## chea 301199 new way to do k1&k2
    k1 = math.safe_divide(1.0f, k1);
    //	k1 = math.safe_divide(test, k1);
    //	k1 = math.safe_divide(1.0f, k1);

    //## chea 161099
    department_array.k1con = k1;

    for ( int j=0; j<totalFacAdded; j++ ) {
	Faculty* facultyPtr = faculty_array[ facutlyRecnoArray[j] ];

	//## chea 161099
	//		k2 += facultyPtr->proj_size_mult * k1 * facultyPtr->proj_count_mult;
	k2 += facultyPtr->proj_size_mult * facultyPtr->proj_count_mult;
	//## chea 301199 guess how the proj_size_mult works
	//		k2 += facultyPtr->proj_size_mult;
    }

    //## chea 301199 new way to do k1&k2
    k2 = math.safe_divide(1.0f, k2);
    //	k2 = math.safe_divide(test, k2);
    //	k2 = math.safe_divide(1.0f, k2);

    //## chea 251099
    //   k1 = 1;
    //   k2 = 2;

    //## chea 161099
    department_array.k2con = k2;

    //-----------//

    for ( int j=0; j<totalFacAdded; j++ )
	generate_faculty_research_project(facutlyRecnoArray[j], rankAgeGroup);
}

//---------- End of function Department::generate_faculty_one_group -----------//

//-------- Begin of function Department::generate_faculty_calc_vars -------//
//!
//! Calculate vars needed for subsequent calculations and store them in static vars.
//!
void Department::generate_faculty_calc_vars() {
    //	1230; by email req42_misc_course.txt

    PeerSchool *player = school_res.player_peer_school;

    float normalTeachingLoad = 3.5f-1.75f / (1+(float)exp(0.5f*
							  (10-player->doctoral_degres_per_faculty_rating-1.5f*player->sponsored_research_rating)));

    //---- calculate research per faculty for all rank age group ---//

    for( int i=0 ; i<MAX_RANK_AGE_GROUP ; i++ )
	rank_age_group_array[i].research_per_faculty = research_per_faculty(i);

    //---- calculate avg research project size, which equals one-half weighted average res/faculty of full prof group 1 and 2 ---//

    average_research_project_size = ( rank_age_group_array[FIRST_RANK_AGE_GROUP+2].research_per_faculty
				      + rank_age_group_array[FIRST_RANK_AGE_GROUP+3].research_per_faculty ) / 4;

    //---- calculate the total weighted adjusted teaching load ----//

    //	err_when( average_research_project_size == 0 ); // cause error in weighted_adjusted_teaching_load

    float totalWeightedAdjustedTeachingLoad = 0;
	int i;
    for( i=0 ; i<MAX_RANK_AGE_GROUP ; i++ )
	totalWeightedAdjustedTeachingLoad += weighted_adjusted_teaching_load(i);

    totalWeightedAdjustedTeachingLoad /= MAX_RANK_AGE_GROUP;

    //err_when(totalWeightedAdjustedTeachingLoad <= 0);

    //## chea 061299 don't know what bug happen here
    if ( totalWeightedAdjustedTeachingLoad < 2 )    // fix bug on fly; error when "add" some depts on game setting
	totalWeightedAdjustedTeachingLoad = 2.0;

    //---- calculate the total number of faculty ought to be in this department ----//

    DepartmentInfo *deptInfo = department_res[department_id];
    int total=0;

    for (i=course_array.size(); i>0; i--) {

	//-----------------------------------------------------------------------//
	//
	// Battle::init_course_all_student() will produce a bug in student graudation drop
	// as it resets variables in course inproperly. When the function is commented out,
	// the bug disappear.
	//
	// The bugs with the teaching load and total faculty number is due to the initialization
	// of the course. The program call student course selection function many times to process
	// course selection in 4 year in just one function. The drawback is that without properly
	// updating variables in course during the process (in fact it should do each trimester
	// during the 4 year process), the class section count will increase many folds. Simply
	// put, this is like 4 times the students trying to attend courses in a single year, the
	// course section count will increase abnormally.
	//
	// To fix the problem, in dept_gf.cpp's Department::generate_faculty_calc_vars() function,
	// the input course_array[i]->class_section_count is divided by 4 before it is used in the
	// calculation.
	//
	//-----------------------------------------------------------------------//

	// assume course enroll
	total += course_array[i]->class_section_count/4;
    }

    //990406 total = max(total, deptInfo->total_courses_taught);
    //err_when(totalWeightedAdjustedTeachingLoad >= 3);

    total_faculty_to_be_added = total
	/ (totalWeightedAdjustedTeachingLoad );       // 1230: comment out (2*total)

    //fred0615
    if(total_faculty_to_be_added==0)total_faculty_to_be_added=1;

    err_when(total_faculty_to_be_added<=0);
}

//---------- End of function Department::generate_faculty_calc_vars -----------//

//-------- Begin of function Department::init_proj_vars -------//
//!
void Department::init_proj_vars(Faculty* facultyPtr) {
    DepartmentInfo* deptInfo = department_res[this->department_id];
    FacultyTemplate* templ = faculty_res.get_faculty_template(deptInfo->template_discretionary_time, facultyPtr->rank_age_group() );

    PeerSchool *playerSchool = school_res.player_peer_school;

    //Test deptInfo->research_per_faculty_multiplier
    //	float researchPerFaculty = ( (float) playerSchool->school_faculty_array_ex[facultyPtr->rank_level].active_research_dollars * templ->talent_research_multiplier );

    //## chea 021299 1.8.3
    //	float researchPerFaculty = ( (float) playerSchool->school_faculty_array_ex[facultyPtr->rank_level].active_research_dollars * deptInfo->research_per_faculty_multiplier *
    //								 templ->talent_research_multiplier );
    float researchPerFaculty = ( (float) playerSchool->school_faculty_array_ex[facultyPtr->rank_level].active_research_dollars * deptInfo->research_per_faculty_multiplier);

    //## chea 171199 try to higher thje SR
    //	if(player_school.sponsored_research_intensity > 5)
    //		researchPerFaculty = researchPerFaculty * player_school.sponsored_research_intensity/5;
    //		researchPerFaculty = researchPerFaculty * player_school.sponsored_research_intensity/7;

    if(player_school.sponsored_research_intensity == 5)
	researchPerFaculty = (float)(researchPerFaculty );
    else if(player_school.sponsored_research_intensity == 6)
	researchPerFaculty = (float)(researchPerFaculty * 1.1);
    else if(player_school.sponsored_research_intensity == 7)
	researchPerFaculty = (float)(researchPerFaculty * 1.2);
    else if(player_school.sponsored_research_intensity == 8)
	researchPerFaculty = (float)(researchPerFaculty * 1.3);
    else if(player_school.sponsored_research_intensity == 9)
	researchPerFaculty = (float)(researchPerFaculty * 1.4);
    else if(player_school.sponsored_research_intensity == 10)
	researchPerFaculty = (float)(researchPerFaculty * 1.5);

    //int expProjs = (int) max(0.0f,min(4.0f,  //## chea 140899  should be wrong min max

    float test = (float)(researchPerFaculty/(deptInfo->percent_pi_faculty/100.0f))/deptInfo->normal_research_project_size;

    float expProjs = (float) max(0.0f,min(4.0f,
					  //min & max bug chea//## chea 140899 change to float cal
					  (float)(researchPerFaculty/(deptInfo->percent_pi_faculty/100.0f))/deptInfo->normal_research_project_size));

    if ( department_id == 3 )                       // business dept
	int prob = 0;

    //## chea 161099 help on changing ave_proj_count & facultyPtr->ave_proj_size
    //	exp_no_projs_per_PI[k]       = expProjs
    //	adj_res_per_fac[k]           = researchPerFaculty
    //	(pct_fac_who_are_PIs[k]/100) = (deptInfo->percent_pi_faculty/100.0f);

    //	facultyPtr->ave_proj_count = min(MAX_RESEARCH_PROPOSAL, k1 * facultyPtr->proj_count_mult * researchPerFaculty);   //tryttt
    //	facultyPtr->ave_proj_count = min((float)(MAX_RESEARCH_PROPOSAL), (float)(k1 * facultyPtr->proj_count_mult * expProjs)); // min & max bug
    //	facultyPtr->ave_proj_count = min((float)(MAX_RESEARCH_PROPOSAL), (float)(k1 * facultyPtr->proj_count_mult * expProjs * (deptInfo->percent_pi_faculty/100.0f))); //## chea 161099
    //## chea 191199
    facultyPtr->ave_proj_count = min((float)(MAX_RESEARCH_PROPOSAL), (float)(k1 * facultyPtr->proj_count_mult * expProjs / (deptInfo->percent_pi_faculty/100.0f)));

    //	facultyPtr->ave_proj_size = k2* facultyPtr->proj_size_mult * researchPerFaculty / (deptInfo->percent_pi_faculty/100.0f);
    //	facultyPtr->ave_proj_size = k2* facultyPtr->proj_size_mult * researchPerFaculty;  //## chea240899
    //## chea 161099
    facultyPtr->ave_proj_size = k2* facultyPtr->proj_size_mult * researchPerFaculty;
    //## chea 301199 test new idea
    //	facultyPtr->ave_proj_size = k2* facultyPtr->proj_size_mult * deptInfo->normal_research_project_size;

    if ( facultyPtr->ave_proj_size <= 0 ) {
	float base = math.safe_divide(researchPerFaculty, ((deptInfo->percent_pi_faculty/100.0f) * expProjs));
	float tmp = m.fmax(0.0f,(0.5f + 2*math.safe_pow(facultyPtr->talent_research/100.0f,2)) * base
			   //min & max bug chea
			   + m.fmax(0.25f*base, (float)(math.get_random_snd(0, 0.2f*base))));

	facultyPtr->ave_proj_size = tmp;
    }

    //------//

    facultyPtr->research_proposal_count = 0;
}

//-------- End of function Department::init_proj_vars -------//

//-------- Begin of function Department::generate_faculty_research_project -------//
//!
//! <int> facultyRecno - generate research projects for this faculty.
//!
void Department::generate_faculty_research_project(int facultyRecno, int rankAgeGroup) {
    // 990604
    if ( player_school.sponsored_research_intensity <= 0 )
	return;

    Faculty* facultyPtr = faculty_array[facultyRecno];
    int i;
    //## chea 230899
    DepartmentInfo* deptInfo = department_res[this->department_id];

    /*

      // 990412

      3. For each faculty sim, calculate:
      ave_proj_count 	= k1* proj_count_mult*exp_no_projs_per_PI[k]
      ave_proj_size 	= k2* proj_size_mult*adj_res_per_fac[k]/(pct_fac_who_are_PIs[k]/100)

      4. For each faculty sim, determine the active projects at initialization:
      prob (n, ave_proj_count) for n = 0 to max_n
      Number of active projects is determined according to this probability vector (many faculty will have no projects). Then, for each project:
      project_size = ave_proj_size *random_normal_deviate(1.0, 0.12),
      direct = project_size/(1.0+ind_cost_rate); indirect = project_size - direct; and
      number of months remaining = rand_int[1, 4 + rand_int[0,8]]. ("1" means the project will expire at the end of the first month of play.)

      5. For each faculty sim, determine the outstanding proposals at initialization:
      prob (n, (ave_prop_dur/ave_proj_dur)*(ave_proj_count/ award_prob)) for n = 0 to max_n
      Number of outstanding proposals is determined according to this probability vector (many faculty will have no proposals). Then, for each proposal:
      proposal_size = ave_proj_size *random_normal_deviate(1.0, 0.12) and
      direct = project_size/(1.0+ind_cost_rate); indirect = project_size - direct; and
      number of months remaining = rand_int[1, 2 + rand_int[0,4]].

    */

    init_proj_vars(facultyPtr);

    //return;	//BUGHERE 990422

    if ( facultyPtr->ave_proj_size <= 0 )           //990414
	return;

    float prob=0;
    int projectCount = MAX_RESEARCH_PROPOSAL;

    if ( department_id == 3 )                       // business dept
	prob = 0;

    float randNum = math.get_random_float();
    for ( i=0; i<=MAX_RESEARCH_PROPOSAL; i++) {
	//## chea 251099 change back to original
	//		prob += math.double_poisson(i, facultyPtr->ave_proj_count); // suppose HAVE K1 in it as well //tryttt
	//		prob += math.double_poisson(i, (facultyPtr->ave_proj_count * deptInfo->percent_pi_faculty/100.0f));     //## chea 230899
	// suppose HAVE K1 in it as well //tryttt
	prob += math.double_poisson(i, facultyPtr->award_prob);

	if ( randNum < prob ) {
	    projectCount = i;
	    break;
	}
    }

    //----- add active projects -----//

    float oRate = finance.get_research_overhead_rate() / 100.0f;

    for( ; projectCount > 0;
	 projectCount--, facultyPtr->research_proposal_count++ ) {
	// project_size = ave_proj_size *random_normal_deviate(1.0, 0.12),
	// direct = project_size/(1.0+ind_cost_rate); indirect = project_size - direct; and
	// number of months remaining = rand_int[1, 4 + rand_int[0,8]]. ("1" means the project will expire at the end of the first month of play.)

	ResearchProposal* researchProposal = facultyPtr->research_proposal_array + facultyPtr->research_proposal_count;

	//## chea 251099
	//		int tmp = (int) max(0.0f, facultyPtr->ave_proj_size * math.get_random_snd(1.0f, 0.05f));  //min & max bug chea
	//		int tmp = (int) max(0.2f, facultyPtr->ave_proj_size * math.get_random_snd(1.0f, 0.20f));  //min & max bug chea
	//## chea 221199
	//min & max bug chea
	int tmp = (int) m.fmax(0.0f, facultyPtr->ave_proj_size * 1000 * math.get_random_snd(1.0f, 0.05f));

	//## chea 151199 try to make the research projet non 0
	//		if(tmp >=0 && tmp <= 1)
	//			tmp =1;

	//## chea 191199 1.8.3
	//		if(tmp >=50)
	//			tmp = 50;

	researchProposal->total_dollars = tmp;

	researchProposal->overhead_rate = oRate;

	researchProposal->status = RESEARCH_ACTIVE;
	//		researchProposal->date_to_next_status = info.game_date + (4 + m.random(9)) * 30;		// draw a random number from 4 to 12 months
	//		researchProposal->date_to_next_status = info.game_date + (m.random(6)) * 30;	//## chea 240899	// draw a random number from 0 to 6 months //## 251099 shoild draw a bigger no. since init
	//		researchProposal->date_to_next_status = info.game_date + (1) * 30;		//## chea 251099 test
	// draw a random number from 2 to 6 months
	researchProposal->date_to_next_status = info.game_date + (2 + m.random(5)) * 30;

	//## chea 151199 try my cal. method 1.4.1
	facultyPtr->research_month_expense += tmp / MAX_ACTIVE_RESEARCH_MONTH;
	facultyPtr->research_month_expense_direct += int( ( tmp / MAX_ACTIVE_RESEARCH_MONTH ) / ( 1+oRate ));

    }

    // 5. For each faculty sim, determine the outstanding proposals at initialization:

#define AVE_PROP_DUR    3.5f
#define AVE_PROJ_DUR    8.5f

    //	const float forCalcProb = (AVE_PROP_DUR / AVE_PROJ_DUR) * (facultyPtr->ave_proj_count / facultyPtr->award_prob );  //tryttt
    //	const float forCalcProb = (AVE_PROP_DUR / AVE_PROJ_DUR) * (facultyPtr->ave_proj_count * deptInfo->percent_pi_faculty/100.0f / facultyPtr->award_prob );  //## chea 230899
    //tryttt
    const float forCalcProb = (facultyPtr->award_prob);

    randNum = math.get_random_float();
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
	// proposal_size = ave_proj_size *random_normal_deviate(1.0, 0.12) and
	// direct = project_size/(1.0+ind_cost_rate); indirect = project_size - direct; and
	// number of months remaining = rand_int[1, 2 + rand_int[0,4]].

	ResearchProposal* researchProposal = facultyPtr->research_proposal_array + facultyPtr->research_proposal_count;

	//## chea 251099
	//		int tmp = (int) max(0.0f, facultyPtr->ave_proj_size * math.get_random_snd(1.0f, 0.05f)); //min & max bug chea
	//		int tmp = (int) max(0.2f, facultyPtr->ave_proj_size * math.get_random_snd(1.0f, 0.20f)); //min & max bug chea
	//		int tmp = (int) max(0.2f, facultyPtr->ave_proj_size ); //## chea 251099 test
	//## chea 221199
	//min & max bug chea
	int tmp = (int) m.fmax(0.0f, facultyPtr->ave_proj_size * 1000 * math.get_random_snd(1.0f, 0.05f));

	//## chea 151199 try to make the research projet non 0
	//		if(tmp >=0 && tmp <= 1)
	//			tmp =1;

	//## chea 191199 1.8.3
	//		if(tmp >=50)
	//			tmp = 50;

	researchProposal->total_dollars = tmp;

	researchProposal->overhead_rate = oRate;

	researchProposal->status = RESEARCH_PROPOSED;

	// draw a random number from 2 to 6 months //## not include in old alg.
	researchProposal->date_to_next_status = info.game_date + (2 + m.random(5)) * 30;
	//		researchProposal->date_to_next_status = info.game_date + (m.random(7)) * 30;		//## 251099 since this is init // draw a random number from 2 to 6 months
	//		researchProposal->date_to_next_status = info.game_date + (1) * 30;		//## chea 251099 test

    }

    /*
      if ( math.get_random_float() > deptInfo->percent_pi_faculty/100.0f )
      return;

      //----- add active projects -----//

      float oRate = finance.get_research_overhead_rate() / 100.0f;
      float projectCount = (float) rank_age_group_array[rankAgeGroup].research_per_faculty / average_research_project_size;

      projectCount *= 1.5;		// 0219

      //----

      float researchPerFaculty = ( (float) playerSchool->school_faculty_array_ex[facultyPtr->rank_level].active_research_dollars * deptInfo->research_per_faculty_multiplier *
      templ->talent_research_multiplier );

      int expProjs = (int) max(0,min(4,(researchPerFaculty/(deptInfo->percent_pi_faculty/100.0f))/deptInfo->normal_research_project_size));

      float base = math.safe_divide(researchPerFaculty, ((deptInfo->percent_pi_faculty/100.0f) * expProjs));

      for( ; projectCount > 0 && facultyPtr->research_proposal_count < MAX_RESEARCH_PROPOSAL  ;
      projectCount--, facultyPtr->research_proposal_count++ )		// number of active projects = random integer, based on research per faculty / average project size
      {
      //--- if projectCount > 0 && projectCount < 1, randomly decide whether a new project needs to be added or not ---//

      if( projectCount < 1 && m.random(10) > projectCount*10 )
      break;

      //-----------------------------------------------//

      ResearchProposal* researchProposal = facultyPtr->research_proposal_array + facultyPtr->research_proposal_count;

      //0405 researchProposal->total_dollars = average_research_project_size * (100+m.random(10)-5) / 100;		// -5 to +5 randomness

      float tmp = max(0,(0.5f + 2*math.safe_pow(facultyPtr->talent_research/100.0f,2)) * base
      + max(0.25f*base, math.get_random_snd(0, 0.2f*base)));
      researchProposal->total_dollars = (int) tmp;

      researchProposal->overhead_rate = oRate;

      researchProposal->status = RESEARCH_ACTIVE;
      researchProposal->date_to_next_status = info.game_date + (4 + m.random(9)) * 30;		// draw a random number from 4 to 12 months

      facultyPtr->research_month_expense += researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH;
      facultyPtr->research_month_expense_direct = int(facultyPtr->research_month_expense / ( 1+oRate));
      }

      //----- add proposed projects -----//

      projectCount = (float) rank_age_group_array[rankAgeGroup].research_per_faculty / average_research_project_size / 2;

      projectCount *= 1.5;		// 0219

      for( ; projectCount > 0 && facultyPtr->research_proposal_count < MAX_RESEARCH_PROPOSAL  ;
      projectCount--, facultyPtr->research_proposal_count++ )
      {
      //--- if projectCount > 0 && projectCount < 1, randomly decide whether a new project needs to be added or not ---//

      if( projectCount < 1 && m.random(10) > projectCount*10 )
      break;

      //-----------------------------------------------//

      ResearchProposal* researchProposal = facultyPtr->research_proposal_array + facultyPtr->research_proposal_count;

      //researchProposal->total_dollars = average_research_project_size * (100+m.random(10)-5) / 100;		// -5 to +5 randomness

      float tmp = max(0,(0.5f + 2*math.safe_pow(facultyPtr->talent_research/100.0f,2)) * base
      + max(0.25f*base, math.get_random_snd(0, 0.2f*base)));
      researchProposal->total_dollars = (int) tmp;

      researchProposal->overhead_rate = oRate;

      researchProposal->status = RESEARCH_PROPOSED;
      researchProposal->date_to_next_status = info.game_date + (2 + m.random(5)) * 30;		// draw a random number from 2 to 6 months
      }
    */
}

//---------- End of function Department::generate_faculty_research_project -----------//

//-------- Begin of function Department::research_per_faculty -------//
//!
//! This function is called by weighted_adjusted_teaching_load() and generate_faculty_calc_vars().
//!
int Department::research_per_faculty(int rankAgeGroup) {
    //---- calculate research per faculty -----//

    PeerSchool *playerSchoolEx = school_res.player_peer_school;

    DepartmentInfo* deptInfo = department_res[department_id];

    // get the talent template
    FacultyTemplate* facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_talent, rankAgeGroup );

    int facultyRank = rank_age_group_array[rankAgeGroup].rank_id;

    int researchPerFaculty = (int) ( (float) playerSchoolEx->school_faculty_array_ex[facultyRank].active_research_dollars * deptInfo->research_per_faculty_multiplier *
				     facultyTemplate->talent_research_multiplier );// ($000)

    return researchPerFaculty;
}

//---------- End of function Department::research_per_faculty -----------//

//-------- Begin of function Department::weighted_adjusted_teaching_load -------//
//!
//! This function is called by Department::generate_faculty_calc_vars() to determine
//! total number of faculty.
//!
float Department::weighted_adjusted_teaching_load(int rankAgeGroup) {
    DepartmentInfo* deptInfo = department_res[department_id];

    //---- get normal course load of the current department ----//

    FacultyTemplate* facultyTemplate;

    facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_course_load, rankAgeGroup );

    //----//

    PeerSchool *player = school_res.player_peer_school;

    // unit: courses/semester
    //	float normalTeachingLoad = float(3.5-1.75/(1+exp(0.5*(10-player->doctoral_degres_per_faculty_rating-1.5*player->sponsored_research_rating ))));
    float normalTeachingLoad = float(3.5-3.5/(1+exp(0.5*(10-player->doctoral_degres_per_faculty_rating-1.5*player->sponsored_research_rating ))));

    err_when( normalTeachingLoad <=0 );

    normalTeachingLoad *= facultyTemplate->normal_teaching_load_multiplier
	* deptInfo->teaching_load_multiplier;

    //----- Scenario special handling ---------//

    if(player_school.scenario_id==SCN_TEACHING_QUALITY) {
	if( player_school.scen_dept_selected_array[department_recno-1] )
	    normalTeachingLoad *=0.333f;
    }

    if(player_school.scenario_id==SCN_RESEARCH_QUALITY) {
	if( player_school.scen_dept_selected_array[department_recno-1] )
	    normalTeachingLoad *=2.0f;
    }

    //---- calculate research per faculty -----//

    int researchPerFaculty = rank_age_group_array[rankAgeGroup].research_per_faculty;

    //-------- calculate adjusted teaching load --------//

    //1230 note: this part is not the same in faculty_initialization
    // adjustedTeachingLoad = 4 roughly.
    float adjustedTeachingLoad = (float)( 0.6 * normalTeachingLoad + 0.4 * normalTeachingLoad
					  / ( 1 + exp(-4.39 + 2.2 * researchPerFaculty
						      / average_research_project_size) ));

    facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_rank_and_age, rankAgeGroup );

    //---- calculate and return weighted adjusted teaching load -----//

    // 990415 return adjustedTeachingLoad * facultyTemplate->rank_age_multiplier;

    return adjustedTeachingLoad * facultyTemplate->rank_age_multiplier * 0.9f;
}

//---------- End of function Department::weighted_adjusted_teaching_load -----------//

//-------- Begin of function Department::faculty_to_be_added -------//
//!
//! Return the number of faculty of this rank age group needed to be generated.
//!
int Department::faculty_to_be_added(int rankAgeGroup) {
    DepartmentInfo* deptInfo = department_res[department_id];
    FacultyTemplate* facultyTemplate;

    //----- calculate raw % faculty from database (player_peer_school) -----//

    int totalProfCount = 0;

    PeerSchool *playerSchoolEx = school_res.player_peer_school;

    for( int i=0 ; i<FACULTY_RANK_LEVEL_COUNT ; i++ )
	totalProfCount += playerSchoolEx->school_faculty_array_ex[i].faculty_count;

    int curRank = rank_age_group_array[rankAgeGroup].rank_id;

    float rawPercentFaculty = (float) playerSchoolEx->school_faculty_array_ex[curRank].faculty_count / totalProfCount;

    //---- calculate adjusted % faculty -------//

    //1230 note: this part is not the same in faculty_initialization

    float templateWeight = 2;                       // define template weight relative to db weight

    facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_rank_and_age, rankAgeGroup );

    float adjustedPercentFaculty = (templateWeight * facultyTemplate->rank_age_multiplier + rawPercentFaculty) / (templateWeight + 1);

    //----- calculate the number of faculty of this rank age group -----//

    err_when(total_faculty_to_be_added <= 0 || adjustedPercentFaculty > 1.5);

    return int( total_faculty_to_be_added * adjustedPercentFaculty );
}

//---------- End of function Department::faculty_to_be_added -----------//

// --- Begin of Department::faculty_new_hires_number --- //
//
int Department::faculty_new_hires_number() {
    int hiresCount = 0;
    for ( int i=1; i<=display_faculty_array.size(); i++) {
	Faculty* facPtr = (Faculty*) display_faculty_array.get(i);
	if ( facPtr->employ_status == 0 )
	    hiresCount++;
    }
    return hiresCount;
}

//
// --- End of Department::faculty_new_hires_number --- //

// --- Begin of Department::faculty_departures_number --- //
//
int Department::faculty_departures_number() {
    int departureCount = 0;
    if ( faculty_res.is_year_end_report ) {
	for ( int i=1; i<=cur_faculty_array.size(); i++ ) {
	    Faculty* facPtr = (Faculty*) cur_faculty_array.get(i);
	    if ( facPtr->employ_status == 1 )
		departureCount++;
	}
    }
    else {
	for ( int i=1; i<=display_faculty_array.size(); i++ ) {
	    Faculty* facPtr = (Faculty*) display_faculty_array.get(i);
	    if ( facPtr->employ_status == 1 )
		departureCount++;
	}
    }

    return departureCount;
}

//
// --- End of Department::faculty_departures_number --- //

// --- Begin of Department::faculty_promotions_number --- //
//
int Department::faculty_promotions_number() {
    int promotionCount = 0;
    if ( faculty_res.is_year_end_report ) {
	for ( int i=1; i<=cur_faculty_array.size(); i++ ) {
	    Faculty* facPtr = (Faculty*) cur_faculty_array.get(i);
	    if ( facPtr->employ_status == 2 )
		promotionCount++;
	}
    }
    else {
	for ( int i=1; i<=display_faculty_array.size(); i++ ) {
	    Faculty* facPtr = (Faculty*) display_faculty_array.get(i);
	    if ( facPtr->employ_status == 2 )
		promotionCount++;
	}
    }

    return promotionCount;
}

//#### begin fred 0919 ###//
//---------- Begin of function Department::calc_faculty_count_by_rank_age -----------//
//!
void Department::calc_faculty_count_by_rank_age(int *facultyCountArray) {
    int totalCount=0;

    memset(facultyCountArray,0,sizeof(int) * MAX_RANK_AGE_GROUP );

    for( int i=faculty_array.size() ; i>0 ; i-- ) {
	if( faculty_array.is_deleted(i) )
	    continue;

	Faculty *facPtr = faculty_array[i];
	int rankAgeGroup = Faculty::rank_age_group(facPtr->rank_level, facPtr->age());

	facultyCountArray[rankAgeGroup]++;
	totalCount++;
    }

    err_when(totalCount != faculty_array.faculty_count);
}

//---------- End of function Department::calc_faculty_count_by_rank_age -----------//

//------ Begin of function Department::calc_research_dollar -------//
//!
void Department::calc_research_dollar() {
    total_research_dollar = 0;    total_research_dollar_direct = 0;

    for( int i=faculty_array.size() ; i>0 ; i-- ) {
	if( faculty_array.is_deleted(i) )
	    continue;

	total_research_dollar += faculty_array[i]->research_month_expense;
	total_research_dollar_direct += faculty_array[i]->research_month_expense_direct;
    }
}

//------ End of function Department::calc_research_dollar -------//

//------ Begin of function Department::calc_research_norm -------//
//!
void Department::calc_research_norm() {
    //---------------------//
    research_dollar_norm = 0;

    // research_dollar_norm -
    // "Global ave. research per faculty" is calculated by a formula like
    // the one in cells B18:B24 of HE.GDB.init:Faculty_Initialization.
    //
    // To make the calculation replace the qualtity defined in Column BW1238
    // of the initialization database (the first term in the formula) with the
    // average of this quantity over all the institutions in the
    // database (all 1100 institutions, not just the initialization or peer institutions).
    //
    // The figure is determined once and for all at initialization. It is age-rank
    // as well as department-specific. At the beginning of each year (after
    // departures and hires), calculate a weighted average over the regular faculty
    // age-rank categories using the number of faculty as weights.

    //PeerSchool *playerSchoolEx = school_res.player_peer_school;
    DepartmentInfo* deptInfo = department_res[department_id];
    int facultyCount[MAX_RANK_AGE_GROUP];
    int totalRegFacCount=0;

    calc_faculty_count_by_rank_age(facultyCount);

    for( int rankAgeGroup=0 ; rankAgeGroup<MAX_RANK_AGE_GROUP ; rankAgeGroup++ ) {
	// get the talent template
	FacultyTemplate* facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_talent, rankAgeGroup );

	int facultyRank = rank_age_group_array[rankAgeGroup].rank_id;

	if ( !is_regular_faculty(facultyRank) )
	    continue;

	int researchPerFaculty = (int) ( (float) department_array.average_research_dollars[facultyRank]
					 * facultyTemplate->talent_research_multiplier * deptInfo->research_per_faculty_multiplier );
	/*
	  int researchPerFaculty = (int) ( (PeerSchool::average_faculty_salary[facultyRank] / 1000.0)
	  * facultyTemplate->talent_research_multiplier * deptInfo->research_per_faculty_multiplier );

	  int researchPerFaculty = (int) ( (PeerSchool::average_faculty_salary[facultyRank] / 1000.0)
	  * facultyTemplate->talent_research_multiplier * deptInfo->research_per_faculty_multiplier );
	  */
	int count = facultyCount[rankAgeGroup];

	research_dollar_norm += researchPerFaculty * count;
	totalRegFacCount += count;
    }

    err_when(totalRegFacCount != get_regular_faculty_count());

    if ( research_dollar_norm > 0 )
	research_dollar_norm /= totalRegFacCount;
    else
	research_dollar_norm = 0;
}

//------- End of function Department::calc_research_norm --------//
//#### end fred 0919 ###//
