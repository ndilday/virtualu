//Filename    : ODEPA_HI.CPP
//Description : Department resource object: for faculty hiring and salary setting
//Owner			: Fred

// To enable DEBUG_LOG,
// #define DEBUG_LOG_LOCAL 1

#include <ALL.H>
#include <OMATH.H>
#include <OLINALG.H>
#include <OLOG.H>

#include <OSYS.H>

#include <OSCHLRES.H>
#include <OPEERSCH.H>
#include <OFINANCE.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <ODATE.H>

// #define DEBUG_MATHCA		// use test data from excel
// #define DEBUG_VC

#ifdef DEBUG_VC
#include <stdio.h>
#endif

static void print_var_name( char *varName ) {
#ifdef DEBUG_VC
    DEBUG_LOG( varName );
#endif
}

static void print_vector2(const ColumnVector &xNames) {
#ifdef DEBUG_VC
    int count = xNames.Storage();
    char t[30];

    String s = "-> ";
    for (int i=1; i<=count; i++ ) {
	sprintf(t, "%.2f, ", xNames(i));
	s+= t;
	//s += m.format(xNames(i));
	//s += ", ";
    }
    DEBUG_LOG(s);
#endif
}

static void print_matrix(const Matrix &xNames) {
#ifdef DEBUG_VC
    int row = xNames.Nrows();
    int col = xNames.Ncols();
    char t[30];

    String s;

    DEBUG_LOG("[");

    for (int j=1; j<=row; j++ ) {
	s = "  ";
	for (int i=1; i<=col; i++ ) {
	    sprintf(t, "%.2f, ", xNames(j, i));
	    s+= t;

	    //s += m.format(xNames(j, i));
	    //s += ", ";
	}
	DEBUG_LOG(s);
    }

    DEBUG_LOG("]");
#endif
}

//--------- Begin of function DepartmentArray::init ----------//
//!
//! refer to td 3.5 section 3
//!
void Department::init_hiring() {
    int i,j;

    //

    int countArray[MAX_RANK_AGE_GROUP_N_TOTAL][GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];

    Faculty * facPtr;

    memset(countArray, 0, sizeof(countArray));

    for (i=faculty_array.size(); i>0; i--) {
	if ( faculty_array.is_deleted(i) )
	    continue;

	facPtr = faculty_array[i];

	countArray[facPtr->rank_age_group()][facPtr->gender_ethnic_group] ++;
    }

    for ( i=0; i<MAX_RANK_AGE_GROUP; i++)
	for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++) {
	    countArray[i][GENDER_ETHNIC_TYPE_COUNT] += countArray[i][j];
	    countArray[MAX_RANK_AGE_GROUP][j] += countArray[i][j];
	}

    //-------- calculate faculty salary --------//

    DepartmentInfo* deptInfo = department_res[department_id];
    FacultyTemplate* facultyTemplate;

    float salaryArray[MAX_RANK_AGE_GROUP_N_TOTAL][GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];

    memset(salaryArray, 0, sizeof(salaryArray));

    for ( int rankAgeGroup=0; rankAgeGroup<MAX_RANK_AGE_GROUP; rankAgeGroup++ ) {
	facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_salary, rankAgeGroup );

	int facultySalary = int( deptInfo->salary_multiplier * facultyTemplate->overall_salary_multiplier *
				 PeerSchool::average_faculty_salary[rankAgeGroup] );

	for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++) {
	    int thisFacultySalary = facultySalary;      // not ($000)

	    if( j==MINORITY_FEMALE || j==NONMINORITY_FEMALE )
		thisFacultySalary = int( (float)thisFacultySalary * facultyTemplate->female_salary_multiplier );

	    if( j==MINORITY_MALE || j==MINORITY_FEMALE )
		thisFacultySalary = int( (float)thisFacultySalary * facultyTemplate->minority_salary_multiplier );

	    salaryArray[rankAgeGroup][j] = (float) thisFacultySalary;
	}
    }

    peer_numeraire_salary = salaryArray[FULL_PROF+1][NONMINORITY_MALE];

    err_when(peer_numeraire_salary <= 0);           // assume at least one white-male FULL_PROF-2

    //-------------------------------------------//

    memset(peer_salary_multiplier_rank, 0, sizeof(peer_salary_multiplier_rank));
    memset(peer_salary_multiplier_gender, 0, sizeof(peer_salary_multiplier_gender));

    for ( i=0; i<MAX_RANK_AGE_GROUP; i++) {
	for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++) {
	    peer_salary_multiplier_rank[i] += math.safe_log(math.safe_divide(salaryArray[i][j], salaryArray[FULL_PROF+1][j])) * countArray[i][j];
	}

	peer_salary_multiplier_rank[i] = (float) exp( math.safe_divide(peer_salary_multiplier_rank[i], (float) countArray[i][GENDER_ETHNIC_TYPE_COUNT]));

	err_when(peer_salary_multiplier_rank[i]>25 || peer_salary_multiplier_rank[i]<0 );
    }

    for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++) {

	for ( i=0; i<MAX_RANK_AGE_GROUP; i++) {
	    peer_salary_multiplier_gender[j] += math.safe_log(math.safe_divide(salaryArray[i][j], salaryArray[i][NONMINORITY_MALE])) * countArray[i][j];
	}

	peer_salary_multiplier_gender[j] = (float) exp( math.safe_divide(peer_salary_multiplier_gender[j], (float) countArray[MAX_RANK_AGE_GROUP][j]));

	err_when(peer_salary_multiplier_gender[j]>50 || peer_salary_multiplier_gender[j]<0 );
    }

    //-------------------------------------------//
    // init talent
    //-------------------------------------------//

    //float		peer_talent_rank[MAX_RANK_AGE_GROUP][TALENT_COUNT];
    //float		peer_talent_gender[GENDER_ETHNIC_TYPE_COUNT][TALENT_COUNT];

    memset(peer_talent_rank, 0, sizeof(peer_talent_rank));
    memset(peer_talent_gender, 0, sizeof(peer_talent_gender));

    // 990407
    float peerConstRankTalent[MAX_RANK_AGE_GROUP][TALENT_COUNT] = {
	{ 67.9f, 79.4f, 90, },
	{ 67.9f, 76.4f, 90, },
	{ 67.9f, 73.1f, 90, },
	{ 63.3f, 69.5f, 81.8f, },
	{ 60.5f, 65.7f, 34.4f, },
	{ 80.0f, 0, 0, },
	{ 80.0f, 0, 0, },
    };

    float peerConstGenderTalent[GENDER_ETHNIC_TYPE_COUNT][TALENT_COUNT] = {
	{ 67.9f, 79.4f, 90, },
	{ 67.9f, 76.4f, 87, },
	{ 67.9f, 73.1f, 50, },
	{ 70.0f, 75.0f, 60, },
    };

    memcpy(peer_talent_rank, peerConstRankTalent, sizeof(peer_talent_rank));
    memcpy(peer_talent_gender, peerConstGenderTalent, sizeof(peer_talent_gender));

    /* 990407
       for ( rankAgeGroup=0; rankAgeGroup<MAX_RANK_AGE_GROUP; rankAgeGroup++ )
       {

       facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_talent, rankAgeGroup );

       peer_talent_rank[rankAgeGroup][TALENT_TEACH] = deptInfo->talent_teaching * facultyTemplate->talent_teaching_multiplier;
       peer_talent_rank[rankAgeGroup][TALENT_SCHOLARSHIP] = deptInfo->talent_scholarship * facultyTemplate->talent_scholarship_multiplier;
       peer_talent_rank[rankAgeGroup][TALENT_RESEARCH] = deptInfo->talent_research * facultyTemplate->talent_research_multiplier;

       int rank = Faculty::rank_age_group_2_rank(rankAgeGroup);

       if ( rank == LONG_TERM_ADJUNCT || rank == SHORT_TERM_ADJUNCT )
       {
       peer_talent_rank[rankAgeGroup][TALENT_SCHOLARSHIP]
       = peer_talent_rank[rankAgeGroup][TALENT_RESEARCH] = 0;
       }
       }

       // ask bill how to do this initialization?

       for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++)
       {
       rankAgeGroup = FULL_PROF+1;					// full_prof-2

       peer_talent_gender[j][TALENT_TEACH] = peer_talent_rank[rankAgeGroup][TALENT_TEACH];
       peer_talent_gender[j][TALENT_SCHOLARSHIP] = peer_talent_rank[rankAgeGroup][TALENT_SCHOLARSHIP];
       peer_talent_gender[j][TALENT_RESEARCH] = peer_talent_rank[rankAgeGroup][TALENT_RESEARCH];
       }
    */
}

//--------- Begin of function DepartmentArray::salary_setting ----------//
//!
//! refer to td 3.5 section 3
//!
void DepartmentArray::salary_setting() {
    int d, i, deptCount=0;
    Department *deptPtr;

    //------------- section 3.2 -----------------//

    // Overall salary increase budget (from RA model)
    float overalSalaryGrowth = float(finance.expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value + finance.inflation_rate) / 100.0f;

    // Overall average multiplier

    float avgGenderMultiplier[GENDER_ETHNIC_TYPE_COUNT];
    int count[GENDER_ETHNIC_TYPE_COUNT];

    memset(avgGenderMultiplier, 0, sizeof(avgGenderMultiplier));
    memset(count, 0, sizeof(count));

    for (d = size(); d>0; d--) {
	if ( is_deleted(d) )
	    continue;

	deptPtr = department_array[d];

	for (i=0; i<GENDER_ETHNIC_TYPE_COUNT; i++) {
	    avgGenderMultiplier[i] += math.safe_log(deptPtr->salary_multiplier[i]) * deptPtr->faculty_gender_history[i][THIS_YEAR];
	    count[i] = department_array.faculty_gender_history[i][THIS_YEAR];
	}

	deptCount++;
    }

    for (i=0; i<GENDER_ETHNIC_TYPE_COUNT; i++) {
	avgGenderMultiplier[i] = (count[i])?(float) exp( avgGenderMultiplier[i] / count[i]) : 1.0f;

	err_when(avgGenderMultiplier[i] < -15.0f || avgGenderMultiplier[i] > 90.0f);
    }

    //------------- section 3.3 -----------------//
    // STEP 1 ignored; implement outstanding promises, if any, for individual adjustment

    // STEP 2

    // Calculate the salary program multipler for gender-ethnic groups
    float factor = salary_weight_input_2_multiplier((int)gender_ethnic_equity_weight_input) / 6.0f;

    for (i=0; i<GENDER_ETHNIC_TYPE_COUNT; i++)
	avgGenderMultiplier[i] = 1 + factor * ( 1.0f / avgGenderMultiplier[i] - 1);

    // STEP 3 for each fac sim
    // Base 	Dept	Rank-exp	Gender-ethnic	Merit adjustments
    Faculty *facPtr;
    float base, dept, rankExp, genderEthnic, meritOverall;
    int facCount = 0, overallIncrease=0, totalSalary=0;
    int sumPriority = salary_weight_input_2_multiplier((int)teaching_weight_input)
	+ salary_weight_input_2_multiplier((int)scholarship_weight_input)
	+ salary_weight_input_2_multiplier((int)research_weight_input);

    float weight[3];
    weight[0] = math.safe_pow((float)salary_weight_input_2_multiplier((int)teaching_weight_input), 2) / sumPriority;
    weight[1] = math.safe_pow((float)salary_weight_input_2_multiplier((int)scholarship_weight_input), 2) / sumPriority;
    weight[2] = math.safe_pow((float)salary_weight_input_2_multiplier((int)research_weight_input), 2) / sumPriority;

    for ( d=size(); d>0; d-- ) {
	if ( is_deleted(d) )
	    continue;

	deptPtr = department_array[d];

	for ( i=deptPtr->faculty_array.faculty_count; i>0; i-- ) {
	    if ( deptPtr->faculty_array.is_deleted(i) )
		continue;

	    facPtr = deptPtr->faculty_array[i];

	    facCount++;

	    //----------------//
	    // update int	base, dept, rankExp, genderEthnic, meritOverall;
	    int sal = facPtr->salary;

	    base = sal * overalSalaryGrowth;
	    dept = sal * (deptPtr->dept_salary_multiplier - 1);
	    rankExp = sal * ( rank_age_group_salary_increase_multiplier[facPtr->rank_age_group(facPtr->rank_level, facPtr->age())] - 1);
	    //## chea 201099
	    //			genderEthnic = sal * ( avgGenderMultiplier[facPtr->gender_ethnic_group] - 1 );
	    genderEthnic = 3 * sal * ( avgGenderMultiplier[facPtr->gender_ethnic_group] - 1 );

	    meritOverall = 0;

	    err_when(sal>0 && rankExp<=-sal);

	    if ( ! facPtr->is_adjunct_faculty(facPtr->rank_level) ) {
		meritOverall += (facPtr-> performance_teaching / 100.0f) * base * weight[0];
		meritOverall += (facPtr-> performance_scholarship / 100.0f) * base * weight[1];
		meritOverall += (facPtr-> performance_research / 100.0f) * base * weight[2];
	    }
	    else {
		meritOverall += (facPtr-> performance_teaching / 100.0f) * base * salary_weight_input_2_multiplier((int)teaching_weight_input);
	    }

	    //----------------//
	    //min & max bug chea
	    facPtr->last_salary_increase = (int) max(0, (int)(base+dept+rankExp+genderEthnic+meritOverall));
	    overallIncrease += facPtr->last_salary_increase;

	    totalSalary += facPtr->salary;
	}
    }

    if ( facCount == 0 )                            // when initialization
	return;

    //----------------------//
    err_when(totalSalary<=0);
    if(totalSalary<=overallIncrease)
	d=0;                                          // setting breakpoint here to trace why (totalSalary<=overallIncrease)
    // req28_hiring: don't need to check: err_when( overallIncrease<0 || overalSalaryGrowth<0 );
    float rate = float(overallIncrease) / totalSalary;
    // overallIncrease may be zero
    float adjustRate = (overallIncrease != 0) ? (overalSalaryGrowth / rate) : 1.0f;

    for ( d=size(); d>0; d-- ) {
	if ( is_deleted(d) )
	    continue;

	deptPtr = department_array[d];

	for ( i=deptPtr->faculty_array.faculty_count; i>0; i-- ) {
	    if ( deptPtr->faculty_array.is_deleted(i) )
		continue;

	    facPtr = deptPtr->faculty_array[i];

	    facPtr->last_salary_increase = int(facPtr->last_salary_increase * adjustRate);
	    facPtr->salary += facPtr->last_salary_increase;

	    //## chea 201099
	    //			facPtr->salary = int(facPtr->salary * math.get_random_snd(1, 0.0015f));		// 0118
	}
    }

    //
    return;
}

//---------- End of function DepartmentArray::salary_setting ----------//

//--------- Begin of function DepartmentArray::faculty_hiring ----------//
//!
void DepartmentArray::faculty_hiring() {
    // return;		//BUG

    //--------------------------------------------------//

    for ( int i=0; i<FACULTY_RANK_LEVEL_COUNT; i++ )
	school_res.player_peer_school->school_faculty_array_ex[i].salary = int(school_res.player_peer_school->school_faculty_array_ex[i].salary * 1.05f);

    //--------------------------------------------------//

    //finance.hiring_policy_array[size()-1].result_value = 20;		// 12 BUG

    for ( int d=size(); d>0; d-- ) {
	if ( is_deleted(d) )
	    continue;

	sys.yield();

	department_array[d]->faculty_hiring();
    }
}

//---------- End of function DepartmentArray::faculty_hiring ----------//

//--------- Begin of function Department::faculty_hiring ----------//
//!
//! refer to HE.Fac_Hiring.xls and email *_hire.txt
//!
void Department::faculty_hiring(bool hireNow) {
    int i, j, t;

    //------- update peer info -------//

    // assume faculty_hiring() called at the begining of the 2nd year.
    // then update peer_salary_multipliers

    for (i=0; i<MAX_RANK_AGE_GROUP; i++)
	peer_salary_multiplier_rank[i] *= (1+math.get_random_snd(0, 0.01f));

    for (i=0; i<GENDER_ETHNIC_TYPE_COUNT; i++)
	peer_salary_multiplier_gender[i] *= (1+math.get_random_snd(0, 0.01f));

    float inflationRate = (float) finance.inflation_rate / 100.0f;

    peer_numeraire_salary *= (1 + inflationRate + math.get_random_snd(0.015f, 0.01f));

    //-------//

    for (i=0; i<MAX_RANK_AGE_GROUP; i++)
	for ( t=0; t<TALENT_COUNT; t++ )
	    ;                                           //0407 peer_talent_rank[i][t] *= (1+math.get_random_snd(0, 0.025f));

    for (i=0; i<GENDER_ETHNIC_TYPE_COUNT; i++)
	for ( t=0; t<TALENT_COUNT; t++ )
	    ;                                           //0407 peer_talent_gender[i][t] *= (1+math.get_random_snd(0, 0.025f));

    //--------------//

    int totalHires = (int) finance.hiring_policy_array[department_recno-1].result_value;

    if ( hireNow )
	totalHires = 1;

    if ( totalHires <= 0 )
	return;

    //------- 1. Player input -------//

    // see dept.h

    //------- 2. Data from Elsewhere in the Game -------//

    int countArray[MAX_RANK_AGE_GROUP_N_TOTAL][GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    float salaryArray[MAX_RANK_AGE_GROUP_N_TOTAL][GENDER_ETHNIC_TYPE_COUNT_N_TOTAL];
    float salaryMultiplierRank[MAX_RANK_AGE_GROUP];
    float salaryMultiplierGender[GENDER_ETHNIC_TYPE_COUNT];

    Faculty * facPtr;

    memset(countArray, 0, sizeof(countArray));
    memset(salaryArray, 0, sizeof(salaryArray));
    memset(salaryMultiplierRank, 0, sizeof(salaryMultiplierRank));
    memset(salaryMultiplierGender, 0, sizeof(salaryMultiplierGender));

    for (i=faculty_array.size(); i>0; i--) {
	if ( faculty_array.is_deleted(i) )
	    continue;

	facPtr = faculty_array[i];

	countArray[facPtr->rank_age_group()][facPtr->gender_ethnic_group] ++;

	salaryArray[facPtr->rank_age_group()][facPtr->gender_ethnic_group] += facPtr->salary;
    }

    for ( i=0; i<MAX_RANK_AGE_GROUP; i++)
	for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++) {
	    countArray[i][GENDER_ETHNIC_TYPE_COUNT] += countArray[i][j];
	    countArray[MAX_RANK_AGE_GROUP][j] += countArray[i][j];
	}

    //-----//

    for ( i=0; i<MAX_RANK_AGE_GROUP; i++)
	for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++) {
	    int count = countArray[i][j];

	    if ( count > 0 )
		salaryArray[i][j] /= count;
	    else {
		salaryArray[i][j] = (float) school_res.player_peer_school->school_faculty_array_ex[Faculty::rank_age_group_2_rank(i)].salary;
		countArray[i][j] = 1;
	    }
	}

    float numeraireSalary = salaryArray[FULL_PROF+1][NONMINORITY_MALE];

    err_when(numeraireSalary <= 0);                 // assume at least one white-male FULL_PROF-2

    //if ( numeraireSalary <= 0 )
    //	salaryArray[FULL_PROF+1][NONMINORITY_MALE] = 15000;	//0301 fix bug on fly

    //-----//

    for ( i=0; i<MAX_RANK_AGE_GROUP; i++) {
	for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++) {
	    salaryMultiplierRank[i] += math.safe_log(math.safe_divide(salaryArray[i][j], salaryArray[FULL_PROF+1][j])) * countArray[i][j];
	}

	salaryMultiplierRank[i] = (float) exp( math.safe_divide(salaryMultiplierRank[i], (float) countArray[i][GENDER_ETHNIC_TYPE_COUNT]));

	err_when(salaryMultiplierRank[i]>1000 || salaryMultiplierRank[i]<0 );
    }

    for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++) {

	for ( i=0; i<MAX_RANK_AGE_GROUP; i++) {
	    salaryMultiplierGender[j] += math.safe_log(math.safe_divide(salaryArray[i][j], salaryArray[i][NONMINORITY_MALE])) * countArray[i][j];
	}

	salaryMultiplierGender[j] = (float) exp( math.safe_divide(salaryMultiplierGender[j], (float) countArray[MAX_RANK_AGE_GROUP][j]));

	err_when(salaryMultiplierGender[j]>1000 || salaryMultiplierGender[j]<0 );

	// data for next year's salary setting
	this->salary_multiplier[j] = salaryMultiplierGender[j];
    }

    buget_average_salary = int(numeraireSalary * salaryMultiplierRank[ASST_PROF]);

    //-----//

    //990518 int salaryPerNewHire = int(math.safe_divide(finance.cost_limit * 1000, (float) finance.total_hires));		// not ($000)
    // not ($000)
    int salaryPerNewHire = int(math.safe_divide(max(0.0f, finance.cost_limit) * 1000, (float) finance.total_hires));

#ifdef DEBUG_MATHCA
    salaryPerNewHire = 100000;
#endif

    if (!salaryPerNewHire)
	salaryPerNewHire = 100000;

    //------- 3. Parameters -------//

    const float salary_talent_elasticities[TALENT_COUNT] = {0.7f, 1.0f, 1.2f};

    //------- 4. Calculations -------//

    // calc "Available talent by rank-experience group and gender"

    float   talentRank[MAX_RANK_AGE_GROUP][TALENT_COUNT];
    float   talentGender[GENDER_ETHNIC_TYPE_COUNT][TALENT_COUNT];

    for ( i=0; i<MAX_RANK_AGE_GROUP; i++) {
	for (t=0; t<TALENT_COUNT; t++)
	    //min & max bug chea
	    talentRank[i][t] = max(1.0f,min(100.0f, peer_talent_rank[i][t]*math.safe_pow(math.safe_divide(numeraireSalary * salaryMultiplierRank[i], peer_numeraire_salary*peer_salary_multiplier_rank[i]), salary_talent_elasticities[t] )));

	int rank = Faculty::rank_age_group_2_rank(i);

	if ( rank == LONG_TERM_ADJUNCT || rank == SHORT_TERM_ADJUNCT ) {
	    talentRank[i][TALENT_SCHOLARSHIP]=0;
	    talentRank[i][TALENT_RESEARCH]=0;
	}
    }

    for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++) {
	for (t=0; t<TALENT_COUNT; t++)
	    //min & max bug chea
	    talentGender[j][t] = max(1.0f,min(100.0f, peer_talent_gender[j][t]*math.safe_pow(math.safe_divide(numeraireSalary * salaryMultiplierGender[j], peer_numeraire_salary*peer_salary_multiplier_gender[j]), salary_talent_elasticities[t] )));
    }

    //-----//

    const char varLength = 13;

    ColumnVector c(varLength);    c=0;

    int tester =hiring_priority_input_2_multiplier(research_talent_priority);

    int talentPrior[TALENT_COUNT] = {
	hiring_priority_input_2_multiplier(teaching_talent_priority),
	hiring_priority_input_2_multiplier(scholarship_talent_priority),
	hiring_priority_input_2_multiplier(research_talent_priority)
    };
    // modify talentPrior if sum are zero
    err_when( TALENT_COUNT != 3 );
    if( talentPrior[0] + talentPrior[1] + talentPrior[2] <= 0 ) {
	talentPrior[0] = talentPrior[1] = talentPrior[2] = 1;
    }

    // C85:95

    enum {ASST=1,ASSOC,FULL1,FULL2,FULL3,LTADJ,STADJ,WHMA,WHFE,MINAM,MINFE,BSAVE,BOVER};

    ColumnVector weightedTalent(MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT);

    for ( i=0; i<MAX_RANK_AGE_GROUP; i++)
	weightedTalent(i+1) = talentPrior[TALENT_TEACH]*talentRank[i][TALENT_TEACH]+(2*talentPrior[TALENT_SCHOLARSHIP]*talentRank[i][TALENT_SCHOLARSHIP]+4*talentPrior[TALENT_RESEARCH]*talentRank[i][TALENT_RESEARCH])/7.0f;

    for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++)
	weightedTalent(MAX_RANK_AGE_GROUP+j+1) = talentPrior[TALENT_TEACH]*talentGender[j][TALENT_TEACH]+(2*talentPrior[TALENT_SCHOLARSHIP]*talentGender[j][TALENT_SCHOLARSHIP]+4*talentPrior[TALENT_RESEARCH]*talentGender[j][TALENT_RESEARCH])/7.0f;

    // D85:95

    float F7 = ( talentPrior[0] + talentPrior[1] + talentPrior[2] ) / 3.0f;

    float playerPrior[MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT] = {
	1.0f * hiring_priority_level_input_2_multiplier(assistant_professor_priority),
	0.5f * hiring_priority_level_input_2_multiplier(tenured_faculty_priority)/4,
	1.0f * hiring_priority_level_input_2_multiplier(tenured_faculty_priority)/4,
	0.75f * hiring_priority_level_input_2_multiplier(tenured_faculty_priority)/4,
	0.4f * hiring_priority_level_input_2_multiplier(tenured_faculty_priority)/4,
	1.0f * hiring_priority_level_input_2_multiplier(longterm_adjunct_faculty_priority),
	1.0f * hiring_priority_level_input_2_multiplier(shortterm_adjunct_faculty_priority),

	0,
	1.0f * hiring_priority_level_input_2_multiplier(female_faculty_priority),
	1.0f * hiring_priority_level_input_2_multiplier(minority_faculty_priority),
	0.5f * (hiring_priority_level_input_2_multiplier(female_faculty_priority) + hiring_priority_level_input_2_multiplier(minority_faculty_priority))
    };

    float C99 = 0;

    for ( i=1; i<=MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT; i++) {
	c(i) = weightedTalent(i) *  playerPrior[i-1] / F7;
	C99 += (float) weightedTalent(i);
    }

    C99 /= MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT;

    // E85:95	//------//

    ColumnVector salaryIncr(MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT);
    float E99 = 0;

    for ( i=0; i<MAX_RANK_AGE_GROUP; i++) {
	// 0 = Salary payments in relation to current scale
	salaryIncr(i+1) = salaryMultiplierRank[i]*numeraireSalary*(1+0) / 100000.0f;
	E99 += (float) salaryIncr(i+1);
    }

    for ( j=0; j<GENDER_ETHNIC_TYPE_COUNT; j++) {
	salaryIncr(MAX_RANK_AGE_GROUP+j+1) = salaryMultiplierGender[j]*numeraireSalary*(1+0) / 100000.0f;
	E99 += (float) salaryIncr(MAX_RANK_AGE_GROUP+j+1);
    }

    E99 = ((E99/(MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT)) + numeraireSalary ) / 1000;

    //-----//

    ColumnVector b(4);

    // construct vectors b and c

    // E85:97	//------//

    const float C100 = 1;
    const float E100 = 1;

    for ( i=1; i<=MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT; i++)
	c(i) = math.safe_divide( -C100 * ( weightedTalent(i)/3.0f + c(i) ), (double)C99);

    c(BOVER) = math.safe_divide(E100*(math.safe_divide((float)hiring_priority_input_2_multiplier(cost_containment_priority),F7)), E99);
    c(BSAVE) = -0.2 * c(BOVER);

    float E102 = salaryPerNewHire / 100000.0f;

    b(1) = -1;
    b(2) = -1;
    b(3) = -E102;
    b(4) = E102;

    // print_vector2(c);

    err_when(b(3) == 0);

    //-----//

    DiagonalMatrix Q(varLength);  Q = 0;
    Matrix A(4, varLength);                         // row x column

    double inputA[] = {
	-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,
	0,0,0,0,0,0,0,-1,-1,-1,-1,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,-1,
	0,0,0,0,0,0,0,0,0,0,0,-1,1
    };

    A << inputA;

    for ( i=1; i<=MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT; i++) {
	A(3,i) = -salaryIncr(i);
	A(4,i) = salaryIncr(i);
    }

    //-----//

#ifdef DEBUG_MATHCA

    totalHires = 2;

    double inputC[] = {-2.13263,0.09157,1.54078,0.69731,-0.12454,-0.22504,-0.12303,-1.17628,-1.28269,1.54429,0.08191,-0.00056229,0.00281145};

    c << inputC;

    //double inputB[] = {-1.00000,-1.00000,9.68585,-9.68585};
    double inputB[] = {-1.00000,-1.00000,0,0};

    b << inputB;
#endif

    //------- 5. Main -------//

    //-------------------------------------//
    //-------------------------------------//
    //					main loop
    //-------------------------------------//
    //-------------------------------------//

    // not ($000)
    int totalBugetDept = salaryPerNewHire * totalHires;

    int facCountBeforeNewHires = faculty_array.faculty_count;

    for ( i=0; i<totalHires; i++ ) {
#ifdef DEBUG_VC
	char s[300];
	sprintf(s, "\nZZZZZZZ  Department %d: hire the %d th faculty this year ZZZZZZZ", department_recno, i+1);
	DEBUG_LOG(s);
#endif

	int j;

	//----------------------------//-
	//----------------------------//	Go run it!

	print_var_name("Before quadratric_prog");
	print_var_name( "c" );
	print_vector2(c);
	print_var_name( "Q" );
	print_matrix(Q);
	print_var_name( "A" );
	print_matrix(A);
	print_var_name( "b" );
	print_vector2(b);

	ColumnVector xNames(varLength);
	if (!LinearAlgebra::quadratic_prog(c,Q,A,b,xNames))
	    break;                                      // temp do nothing

	print_var_name("After quadratic_prog" );
	print_var_name("xNames" );
	print_vector2(xNames);

	// mathca xNames = {1.,0,0,0,0,0,0,0,1.,0,0}

	/*// 0105section of code still have bug

	  // see email Re: Q40. hiring

	  for (j=1; j<=varLength-2; j++)
	  xNames(j) = max(0,min(1,(xNames(j) + 0.5)));

	  int count=0, group=0;

	  for (j=1; j<=varLength-2; j++)
	  if ( xNames(j) == 1 )
	  {
	  if ( j > MAX_RANK_AGE_GROUP )
	  group = 1;

	  count++;
	  }
    */

	int count=0, group=0;

	for (group=0; group<=1; group++) {
	    int maxIdx = 0;
	    REAL maxValue = 0;
	    int start,end;

	    if ( group == 1 ) {
		start = MAX_RANK_AGE_GROUP+1;
		end = varLength-2;
	    }
	    else {
		start = 1;
		end = MAX_RANK_AGE_GROUP;
	    }

	    // find the max within the group
	    for (j=start; j<=end; j++) {
		if ( xNames(j) > maxValue ) {
		    //err_when( xNames(j) == 1 );

		    maxValue = xNames(j);
		    maxIdx = j;
		}
	    }

	    err_when(maxIdx <= 0);

	    xNames(maxIdx) = 1;
	    count++;
	}

	for (j=1; j<=varLength-2; j++)
	    if ( xNames(j) != 1 )
		xNames(j) =0;

	xNames(varLength-1) = 0;
	xNames(varLength) = 0;

	print_var_name("After changing xNames");
	print_vector2(xNames);

	err_when( count != 2 );

	//----------------------------//
	//----------------------------//	generate faculty

	char  rank, age, gender;
	int talentTeaching, talentScholarship, talentResearch;
	int salary, rankAgeGroup;

	for (j=1; j<=MAX_RANK_AGE_GROUP; j++) {
	    if ( xNames(j) == 1 ) {
		rankAgeGroup = j-1;
		rank = Faculty::rank_age_group_2_rank(j-1);

		talentTeaching = (int) talentRank[j-1][TALENT_TEACH];
		talentScholarship = (int) talentRank[j-1][TALENT_SCHOLARSHIP];
		talentResearch = (int) talentRank[j-1][TALENT_RESEARCH];
		salary = (int) salaryIncr(j);

		break;
	    }
	}

	err_when(j==MAX_RANK_AGE_GROUP+1);

	for (j=MAX_RANK_AGE_GROUP+1; j<=MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT; j++) {
	    if ( xNames(j) == 1 ) {
		gender = j-MAX_RANK_AGE_GROUP-1;

		talentTeaching = (int) (talentTeaching + talentGender[gender][TALENT_TEACH]) / 2;
		talentScholarship = (int) (talentScholarship + talentGender[gender][TALENT_SCHOLARSHIP]) / 2;
		talentResearch = (int) (talentResearch + talentGender[gender][TALENT_RESEARCH]) / 2;
		//min & max bug chea
		salary = (int) max(0, (int)(salary + salaryIncr(j) + numeraireSalary));

		break;
	    }

	}

	err_when(j==MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT+1);

	age = Faculty::get_min_age(rankAgeGroup) + m.random(5);

	int startTeachingDate = 0;

	startTeachingDate = date.julian(info.game_year,info.game_month,info.game_day);

	/*
          // for testing only
	  int errorwhen = m.random( age - Faculty::get_min_age(rankAgeGroup) ) * 365 + m.random( 365 );
	  int start_game = info.game_start_date;
	  int syear = date.get_date(start_game,'Y');
	  int smonth = date.get_date(start_game,'M');
	  int sday = date.get_date(start_game,'D');
	  int test_day = date.julian(2,9,1);
	  int test_daysdg = date.julian(-2,9,1);
	  int year = date.get_date(startTeachingDate,'Y');
	  int month = date.get_date(startTeachingDate,'M');
	  int day = date.get_date(startTeachingDate,'D');
	*/

	//------- for setting faculty salary ------------//
	//
	// Since there are some problems with above code calculating the salary,
	// we are using this new code from faculty initializating to override the calculation.
	//
	//-----------------------------------------------//

	DepartmentInfo* deptInfo = department_res[department_id];

	FacultyTemplate* facultyTemplate = faculty_res.get_faculty_template( deptInfo->template_salary, rankAgeGroup );
	PeerSchool *plSch = school_res.player_peer_school;

	salary = int( deptInfo->salary_multiplier * facultyTemplate->overall_salary_multiplier *
		      plSch->school_faculty_array_ex[rank].salary );

	//------------ add the new faculty ------------//

	int facultyRecno = faculty_array.add(department_recno, rank, gender, age, startTeachingDate,
					     salary, talentTeaching, talentScholarship, talentResearch);

	//0415 this->generate_faculty_research_project(facultyRecno, rankAgeGroup);	// 0412

	//calc_faculty_history();			// 0407

	//----------------------------//
	//----------------------------// prepare data for next loop

	if ( i == 0 ) {
	    // append a new row to A
	    RowVector dummy(varLength);     dummy = 0;

	    A &= dummy;

	    ColumnVector dum(1);

	    b &= dum;

	    b(5) = 0;
	}

	// update the fifth row of A
	// see subject: Re: Q38a. misc

	ColumnVector test(MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT);
	int rCount=0, gCount=0;

	for (j=1; j<=MAX_RANK_AGE_GROUP+GENDER_ETHNIC_TYPE_COUNT; j++) {
	    A(5,j) -= ( xNames(j) == 1 && math.get_random_float() > 0.25 ) ? 1 : 0;
	    test(j) = A(5,j);

	    if ( A(5,j) == -1 ) {
		if ( j <= MAX_RANK_AGE_GROUP )
		    rCount++;
		else
		    gCount++;
	    }
	}

	if ( rCount >= MAX_RANK_AGE_GROUP ) {
	    for (j=1; j<=MAX_RANK_AGE_GROUP; j++)
		A(5,j) = 0;
	}

	if ( gCount >= GENDER_ETHNIC_TYPE_COUNT ) {
	    for (j=1; j<=GENDER_ETHNIC_TYPE_COUNT; j++)
		A(5,j+MAX_RANK_AGE_GROUP) = 0;
	}

	print_var_name("test");
	print_vector2(test);

	//------//

	totalBugetDept += -salary;                    // not ($000)

	if ( totalHires-1 != i )
	    salaryPerNewHire = totalBugetDept / (totalHires-i-1);

	E102 = salaryPerNewHire / 100000.0f;

	b(3) = -E102;
	b(4) = E102;

	sys.yield();
    }

    err_when( faculty_array.faculty_count != facCountBeforeNewHires + totalHires );

}

//---------- End of function Department::faculty_hiring ----------//

/*
67.9		79.4	90
67.9	76.4	90
67.9	73.1	90
63.3	69.5	81.8
60.5	65.7	34.4
80.0	0	0
80.0	0	0

  float peerConstGenderTalent[GENDER_ETHNIC_TYPE_COUNT][TALENT_COUNT] =
{
67.9	79.4	90
67.9	76.4	87
67.9	73.1	50
70.0	75.0	60
*/
