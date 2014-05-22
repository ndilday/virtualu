//Filename   : ODEPT_GS.CPP
//Description: functions for generating student records
//owner:			fred 0616

#include <OBOX.H>

#include <OMATH.H>
#include <OINFO.H>
#include <OMISC.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OSCHLRES.H>
#include <OPSCHOOL.H>
#include <OPEERSCH.H>                             //##### fred 0714 #####//
#include <OSYS.H>

#include <ALL.H>                                  //for debug_msg
#include <STDIO.H>

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

//-------- Define struct  ---------//
short NONTRA_COUNT = 0;
short MASTER_COUNT = 0;
short DOCTOR_COUNT = 0;

//!Structure used by the PlayerSchool class containing the student recno
//!and dept recno and trimester for each student.
struct StudentStruct {
    int stu_recno;
    int dept_recno;
    char  trimester_tag;
};

//!Array of StudentStructs maintained by the PlayerSchool class, broken
//!down by level.
class StuDynArray : public DynArray {
public:
    StuDynArray() : DynArray(sizeof(StudentStruct), 200) {};

    void deinit() {
	if( size()==0 )
	    return;

	for( int i=size() ; i>0 ; i-- ) {
	    //if( !is_deleted(i) )
	    linkout(i);
	}

	zap();                                      // zap the DynArrayB
    };
};

//------- Define static vars -----------//

static StuDynArray    stuArr[MAX_STUDENT_LEVEL];

//---------- Begin of function PlayerSchoolgenerate_student -----------//
//!
void PlayerSchool::generate_student() {
    // refer to techdoc3.3 section 4.1
    generate_student_level_pct();

  // refer to techdoc3.3 section 4.2 & 4.3
    generate_student_transition_prob_n_year_in_program();

    // refer to techdoc3.3 section 4.4
    generate_student_gender_ethnic();

    // refer to techdoc3.3 section 4.5: talent indices
    // refer to techdoc3.3 section 4.6: financial aid
    // NOTE: see td3.4 section 2.7 for these two parts; see enrollment model

  // refer to techdoc3.3 section 4.7
    generate_student_major_preference();

    // refer to techdoc3.3 section 4.8: elective course preference
    // refer to techdoc3.3 section 4.9: courses taken

    // adjust the pct arrays for generate_student_random_get_group_index()
    generate_student_adjust_pct_arrays();

    // NOTE.1 980930
    // DL students will use the data in HE.GDB.init:Student_Parameters for non-traditional
    // undergraduates (SL-2).  The original data for SL-5, non-matriculated students, can
    // be abandoned.

    //
    //----- randomly get properties for generate student -----//
    //
    char studentLevel, yearInProgram, genderEthnicGroup, stuSeg, major;
    float talentArr[TALENT_VAR_COUNT];
    int i, finAid = 0, progressIndex = 50;

    talentArr[0]=talentArr[1]=talentArr[2] = 0.5f;

  //## chea 281099 you can lower the init student no. from below
    int stuCount = get_player_expected_student_count();

    //DynArrayB		stuArr(sizeof(Student*), 2000,DEFAULT_REUSE_INTERVAL_DAYS);

    int sl_count=0;

#ifdef DEBUG
    int year1Count[MAX_STUDENT_LEVEL];
    memset( year1Count, 0, sizeof(year1Count) );
#endif

    for( i=stuCount; i>0 ; i-- ) {

	//----- find studentLevel -----//

	studentLevel = generate_student_random_get_group_index
	    (student_level_pct, MAX_STUDENT_LEVEL-1);   // omit Distance Learning

	if ( studentLevel <= MASTER ) {
	    yearInProgram = 1 + generate_student_random_get_group_index
		(year_in_program_pct[studentLevel], MAX_GRADUATE_YEARS+1);
	}
	else if ( studentLevel == DOCTOR ) {
	    // special case: doctor's major is assigned here
	    //
	    err_when(studentLevel-1 != SL_DOCTOR);
	    // major is dept_recno to department_array
	    major = 1 + generate_student_random_get_group_index
		(adjusted_student_major_pref[SL_DOCTOR], department_array.department_count);

	    yearInProgram = 1 + generate_student_random_get_group_index
		(department_res[department_array[major]->department_id]->doctor_year_in_program_pct, MAX_GRADUATE_YEARS+1);
	}
	else if ( studentLevel == DISTANCE_LEARN ) {
	    yearInProgram = 1 + generate_student_random_get_group_index
		// see note.1 above
		(year_in_program_pct[UG_NONTRADITION], MAX_GRADUATE_YEARS+1);
	}
	else
	    err_here();                                 // unknown student level

	if(yearInProgram==1)
	    sl_count++;

	//----- find gender -----//

	genderEthnicGroup = generate_student_random_get_group_index
	    (student_gender_pct[studentLevel], GENDER_ETHNIC_TYPE_COUNT);

	//----- find major = deptRecno -----//

	if ( yearInProgram > 1 ||
	     ( !is_ug_group(studentLevel) )  ) {
	    if ( studentLevel != DOCTOR ) {             // doctor's major is assigned above
		short sl;
		if ( studentLevel == DISTANCE_LEARN )     // for sl5
		    sl = SL_UG;
		else if ( studentLevel > UG_TRADITION  )  // for sl2-3
		    sl = studentLevel - 1;
		else
		    sl = SL_UG;                             // for sl1

		err_when(MASTER != UG_TRADITION + 2);

		// major is dept_recno to department_array
		major = 1 + generate_student_random_get_group_index
		    // sl: 0107: this should fix the DL student in agricultre dept
		    (adjusted_student_major_pref[sl], department_array.department_count);
	    }
	}
	else if ( yearInProgram == 1 ) {
	    // 1st year UG student don't have major
	    major = 0;                                  // 0 -> UNDECIDED;
	}
	else
	    err_here();

	//----------- student segment ------------//

	if ( studentLevel == UG_TRADITION ) {
	    stuSeg = m.random(STUDENT_SEGMENT_COUNT);   //BUGHERE //TO calc subSeg
	}
	else {
	    stuSeg = -1;
	}

	//----- add student -----//

	// add old students here and, will add new students (yr1 student) in enrollment

	if ( yearInProgram > 1 ) {
	    // assign talent here // 990414

	    enroll_res.calc_a_talent(talentArr, studentLevel, genderEthnicGroup, stuSeg);

	    int stuRecno=0;
	    if ( major )
		stuRecno = department_array[major]->student_array.add( studentLevel, yearInProgram,
								       genderEthnicGroup, stuSeg, major, finAid, talentArr );
	    else
		err_here();
	    //department_res.general_dept.student_array.add( studentLevel, yearInProgram,
	    //	genderEthnicGroup, stuSeg, major, finAid, talentArr );

	    // 990413
	    // init var for course taking init
	    Student* stuPtr = department_array[major]->student_array[stuRecno];
	    StudentStruct stuStruct;

	    stuStruct.stu_recno = stuRecno;
	    stuStruct.dept_recno = major;
	    stuStruct.trimester_tag = -1;

	    // remember this sim
	    stuArr[studentLevel].linkin(&stuStruct);

	    //StudentStruct* stuStrPtr = (StudentStruct *) stuArr.get();
	    //err_when(stuPtr->student_level != studentLevel);
	    //err_when(stuStrPtr->stu_ptr != stuPtr);
	    //err_when(stuStrPtr->stu_ptr->student_level != studentLevel);
	}
#ifdef DEBUG
	else {
	    ++year1Count[studentLevel];
	}
#endif
    }

    if ( sys.debug_session ) {
	do {
	    Department* deptPtr = department_array[m.random(10)+1];

	    int stdRecno = m.random(deptPtr->student_array.size())+1;

	    Student* stdPtr = deptPtr->student_array[stdRecno];

	    if ( stdPtr->audit_flag )
		continue;

	    switch ( stdPtr->student_level ) {
	    case UG_TRADITION:    continue;
		break;
	    case UG_NONTRADITION: if ( NONTRA_COUNT >= MAX_NONTRA_COUNT )
		continue;
	    NONTRA_COUNT++;
	    break;
	    case MASTER:        if ( MASTER_COUNT >= MAX_MASTER_COUNT )
		continue;
	    MASTER_COUNT++;
	    break;
	    case DOCTOR:        if ( DOCTOR_COUNT >= MAX_DOCTOR_COUNT )
		continue;
	    DOCTOR_COUNT++;
	    break;
	    }

	    stdPtr->old_student_recno = stdRecno;

	    stdPtr->audit_flag = 1;

	    File file;

	    char fileName[123];

	    switch( stdPtr->student_level ) {
	    case UG_NONTRADITION: strcpy( fileName, "NONTRADITION" );
		break;
	    case MASTER:        strcpy( fileName, "MASTER" );
		break;
	    case DOCTOR:        strcpy( fileName, "DOCTOR" );
		break;
	    }

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
	} while ((NONTRA_COUNT != MAX_NONTRA_COUNT ) || (MASTER_COUNT != MAX_MASTER_COUNT) || (DOCTOR_COUNT != MAX_DOCTOR_COUNT));
    }

    //--- initialize previous course selection data for newly generated students. ---//
    generate_student_init_course(progressIndex);    // 990412

    for (char sl=0; sl<MAX_STUDENT_LEVEL; sl++)
	stuArr[sl].deinit();

}

//---------- End of function PlayerSchool::generate_student -----------//

//---------- Begin of function PlayerSchool::generate_student_level_pct -----------//
//!
//! refer to tecdoc 3.3 section 4.1
//!

int PlayerSchool::get_player_expected_student_count() {
    const int remap[INPUT_OPTION_COUNT] = {         //refer to notes 990114
	2000, 6000, 10000
    };

    //## BUGHER for newif
    err_when(student_count < 0 || student_count >= INPUT_OPTION_COUNT);

    return remap[student_count];
    //## BUGHER for newif
    return student_count;
}

int PlayerSchool::get_player_expected_sponsored_research() {
    switch (sponsored_research_intensity) {
    case 0:   return 1;
    case 5:   return 2;
    case 10:    return 4;
    default:
	err_here();
	return 1;
    }
}

void PlayerSchool::generate_student_level_pct() {
    PeerSchool *playerEx = school_res.player_peer_school;
    int studentCount=0, i;

    err_when(UG_TRADITION != 0 || UG_NONTRADITION != 1);

    float sl2Multiplier[3] = { 1.5f, 1.0f, 0.5f };
    err_when(URBAN != 0);

    int realStuCount = get_player_expected_student_count();

    student_level_pct[UG_TRADITION]   = playerEx->full_time_undergrad;
    // 1009 + campus location
    student_level_pct[UG_NONTRADITION]  = playerEx->part_time_undergrad * sl2Multiplier[player_school.campus_environment] * player_school.parttime_undergrad_percent / 100;
    // player_input
    student_level_pct[DISTANCE_LEARN] = (float) ( realStuCount * sl5_student_percent / 100);

    int gradStudCount = realStuCount * graduate_student_percent / 100;

    student_level_pct[DOCTOR] = gradStudCount * player_school.doctoral_program_intensity / 100;
    student_level_pct[MASTER] = gradStudCount - student_level_pct[DOCTOR];

    //-----------------------------------------------//

    for (i=0; i<MAX_STUDENT_LEVEL; i++)
	studentCount += (int) student_level_pct[i];

    //-----------------------------------------------//

    float scaleFactor = float(realStuCount) / studentCount;

    for (i=0; i<MAX_STUDENT_LEVEL; i++) {
	student_level_pct[i]
	    = (student_level_pct[i] * scaleFactor) / realStuCount;
    }
}

//---------- End of function PlayerSchool::generate_student_level_pct -----------//

//---------- Begin of function PlayerSchool::generate_student_transition_probabilities -----------//
//!
//! refer to tecdoc 3.3 section 4.2 & 4.3
//!
//! In email req12:
//! Re Q4:
//! Treat all transition probabilities out of year 9 and later as equal to the
//! year-8 probability. There's no maximum time for studies, but there will be
//! few "survivors" beyond year 8 in any case.
//!
void PlayerSchool::generate_student_transition_prob_n_year_in_program() {
    PeerSchool *playerEx = school_res.player_peer_school;
    int yr, sl, i;

    //----- transition probabilities for UG_TRADITION & UG_NONTRADITION & MASTER-----//
    // refer to techdoc3.3 section 4.2
    //temporarily place the following procedures in this function

    // read these two arrays from dbase
    const float dropouts_fraction[MASTER_ARRAY_SIZE][MAX_GRADUATE_YEARS] = {
	{ 0.29f, 0.19f, 0.26f,  0.16f, 0.07f, 0.02f, 0.01f, 0.00f },
	{ 0.25f, 0.20f, 0.15f, 0.10f, 0.08f, 0.08f, 0.07f, 0.07f  },
	{ 0.70f, 0.30f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f  }

    };

    const float graduates_fraction[MASTER_ARRAY_SIZE][MAX_GRADUATE_YEARS] = {
	{ 0.00f,  0.00f,  0.06f,  0.53f,  0.31f,  0.08f,  0.02f,  0.00f },
	{ 0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.10f,  0.30f,  0.50f },
	{ 0.95f,  0.05f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f }
    };

    err_when(UG_TRADITION!=0);
    err_when(MASTER!=2);

    // step 1
    //
    float fractionWhoGradOnTime[UG_NONTRADITION+1];
    float gradAnytimePct[MASTER_ARRAY_SIZE];

    memset(fractionWhoGradOnTime, 0, sizeof(fractionWhoGradOnTime));

    for (i=0; i<MAX_GRADUATE_YEARS; i++)
	fractionWhoGradOnTime[UG_TRADITION] += graduates_fraction[UG_TRADITION][i];

    for (i=0; i<MAX_GRADUATE_YEARS; i++)
	fractionWhoGradOnTime[UG_NONTRADITION] += graduates_fraction[UG_NONTRADITION][i];

    for (sl= 0; sl<=UG_NONTRADITION; sl++) {
	gradAnytimePct[sl]
	    = playerEx->target_grad_rate_sl[sl] / 100.0f
	    + ( 1-playerEx->target_grad_rate_sl[sl] / 100.0f )
	    * ( 1-fractionWhoGradOnTime[sl] );
    }
    // special case for MASTER
    gradAnytimePct[MASTER] = playerEx->target_grad_rate_sl[MASTER] / 100.0f;

    // step 2
    //
    float fractionSurvivors[MAX_GRADUATE_YEARS+1], popDivisor[MASTER_ARRAY_SIZE];
    float gradTransProb[MASTER_ARRAY_SIZE][MAX_GRADUATE_YEARS];

    for (sl=0; sl<MASTER_ARRAY_SIZE; sl++) {
	for (yr=0; yr<MAX_GRADUATE_YEARS; yr++) {
	    if ( yr > 0 ) {
		fractionSurvivors[yr]  = fractionSurvivors[yr-1] * (1.0f - gradTransProb[sl][yr-1]    - dropout_trans_prob[sl][yr-1]);
		popDivisor[sl] += fractionSurvivors[yr];
	    }
	    else {
		fractionSurvivors[yr] = 1.0f;
		popDivisor[sl] = fractionSurvivors[yr];
	    }

	    // BUG FIX if divided by zero
	    dropout_trans_prob[sl][yr] = math.safe_divide( dropouts_fraction[sl][yr] * ( 1.0f - gradAnytimePct[sl] ), fractionSurvivors[yr] );

	    gradTransProb[sl][yr] = math.safe_divide( graduates_fraction[sl][yr] * gradAnytimePct[sl], fractionSurvivors[yr] );
	}

	//----- calc year_in_program_pct -----//
	fractionSurvivors[yr]  = fractionSurvivors[yr-1] * (1.0f - gradTransProb[sl][yr-1] - dropout_trans_prob[sl][yr-1]);
	popDivisor[sl] += fractionSurvivors[yr];

	for (yr=0; yr<MAX_GRADUATE_YEARS+1; yr++) {
	    year_in_program_pct[sl][yr] = math.safe_divide( fractionSurvivors[yr], popDivisor[sl] );
	    //## chea 120200 just checking
	    if(year_in_program_pct[sl][yr]<=0)
		year_in_program_pct[sl][yr]=0;
	}
    }

    memcpy(grad_trans_prob, gradTransProb, sizeof(gradTransProb));

    //----- transition probabilities for DOCTOR -----//
    // calculated for each department;
    // see DepartmentInfo::generate_student_transition_prob_n_year_in_program_doctor()
    short deptCount = department_res.department_count;
    for( i=1 ; i<=deptCount ; i++ ) {
	DepartmentInfo *deptInfo = department_res[i];
	deptInfo->generate_student_transition_prob_n_year_in_program_doctor();
    }

    //----- transition probabilities for SL-5 are not necessary -----//
}

//---------- End of function PlayerSchool::generate_student_transition_probabilities -----------//

//-------- Begin of function PlayerSchool::generate_student_transition_probabilities -------//
//!
//! depends on which department student major in
//!
void DepartmentInfo::generate_student_transition_prob_n_year_in_program_doctor() {
    //----- transition probabilities for DOCTOR -----//
    // refer to techdo3.3, section 4.2, p.12-p.13
    //
    short sl = DOCTOR;
    short yr;

    PeerSchool *playerEx = school_res.player_peer_school;
    float targetSchoolGradRate = playerEx->target_grad_rate_sl[sl] / 100.0f;
    float avgTimeToDegree = playerEx->doc_time_to_degree;

    //temp vars for debug
    //targetSchoolGradRate = 0.465f;
    //avgTimeToDegree = 4.21f;

    float avgTimeToDropout = 0.5f * avgTimeToDegree;// B102

    // step1: F106:H106 of student_par spreadsheet
    //
#define MAX_GRADUATE_RATE 0.8f

    DepartmentInfo* deptInfo = this;                // department_res[department_id];

    float cummGradRate = targetSchoolGradRate * deptInfo->doctoral_grad_rate_multiplier;
    if ( cummGradRate > MAX_GRADUATE_RATE )
	cummGradRate = MAX_GRADUATE_RATE;

    float aveTimeToDropout  = avgTimeToDropout * deptInfo->doctoral_time_to_dropout_multiplier;
    float aveTimeToDegree = avgTimeToDegree * deptInfo->doctoral_time_to_degree_multiplier;

    // step2: I106:P106

    float graduateFraction, dropoutFraction;
    float fractionSurvivors[MAX_GRADUATE_YEARS], popDivisor;

    for (yr=0; yr<MAX_GRADUATE_YEARS; yr++) {
	if ( yr > 0 ) {
	    fractionSurvivors[yr]  = fractionSurvivors[yr-1] * (1.0f - doctor_dropout_trans_prob[yr-1] - doctor_graduate_trans_prob[yr-1]);
	    popDivisor += fractionSurvivors[yr];
	}
	else {
	    fractionSurvivors[yr] = 1.0f;
	    popDivisor = fractionSurvivors[yr];
	}

	//askbillok: 1-yr or yr-1:
	yr++;                                         // see equation (3) in p.13 techdoc 3.3
	dropoutFraction
	    = float( exp((1-yr)/aveTimeToDropout) - exp(-yr/aveTimeToDropout));
	graduateFraction
	    = float( exp((3-yr)/(aveTimeToDegree-2)) - exp((2-yr)/(aveTimeToDegree-2)));
	yr--;

	//----- calc trans_prob -----//

	doctor_dropout_trans_prob[yr] = dropoutFraction*(1-cummGradRate)/fractionSurvivors[yr];

	if ( yr==0 || yr==1 )
	    doctor_graduate_trans_prob[yr]  = 0.0f;
	else
	    doctor_graduate_trans_prob[yr]  = graduateFraction*cummGradRate/fractionSurvivors[yr];
    }

    fractionSurvivors[yr]  = fractionSurvivors[yr-1] * (1.0f - doctor_dropout_trans_prob[yr-1] - doctor_graduate_trans_prob[yr-1]);
    popDivisor += fractionSurvivors[yr];

    for (yr=0; yr<MAX_GRADUATE_YEARS+1; yr++) {     //askbillok: take the easiest approaach; why yr=0to8 (why+1)
	doctor_year_in_program_pct[yr] = fractionSurvivors[yr] / popDivisor;
    }
}

//---------- End of function PlayerSchool::generate_faculty_calc_vars -----------//

//-------- Begin of function PlayerSchool::generate_student_gender_ethnic -------//
//!
//! refer to techdoc3.3 section 4.4
//!
void PlayerSchool::generate_student_gender_ethnic() {
    //----- initial percent for each gender-ethic group ----//

    PeerSchool *playerEx = school_res.player_peer_school;

    for ( int sl=0; sl<MAX_STUDENT_LEVEL; sl++ ) {
	//----- get female and minority percentages from player_school_ex -----//

	float femalePercent = playerEx->percent_female_sl[sl] / 100.0f;
	float minorityPercent = playerEx->percent_minority_sl[sl] / 100.0f;

	//--------- special case: scenario ---------//

	if ( info.year_passed == 1 && player_school.scenario_id == SCN_STUDENT_DIVERSITY ) {
	    minorityPercent *= 0.571f;
	}

	// calc what we want

	student_gender_pct[sl][MINORITY_FEMALE] = femalePercent*minorityPercent;
	student_gender_pct[sl][NONMINORITY_FEMALE] = femalePercent*(1.0f - minorityPercent);
	student_gender_pct[sl][MINORITY_MALE] = (1.0f - femalePercent)*minorityPercent;
	student_gender_pct[sl][NONMINORITY_MALE] = (1.0f - femalePercent)*(1.0f - minorityPercent);
    }
}

//---------- End of function PlayerSchool::generate_student_gender_ethnic -----------//

//-------- Begin of function PlayerSchool::generate_student_major_preference -------//
//!
//! refer to techdoc3.3 section 4.7
//!
void PlayerSchool::generate_student_major_preference() {
    short i, sl, deptCount, deptId;
    DepartmentInfo *deptInfo;

    deptCount = department_res.department_count;
    for( i=1 ; i<=deptCount ; i++ ) {
	deptInfo = department_res[i];
	deptInfo->generate_student_major_preference();
    }

    //----- adjust the preferecne percentages for the included dept -----//
    //
    // please note that if a department is not included in department_array,
    // it won't have an entry in adjusted_student_major_pref
    //

    deptCount = department_array.department_count;
    float totalPctIncludedDept = 0.0f;

    err_when(!deptCount);
    memset(adjusted_student_major_pref, 0, sizeof(adjusted_student_major_pref));

    for (sl=0; sl<MAX_STUDENT_LEVEL_INI; sl++) {
	totalPctIncludedDept = 0.0f;

	for (i=1; i<=deptCount; i++) {
	    deptId = department_array[i]->department_id;
	    deptInfo = department_res[deptId];
	    totalPctIncludedDept += deptInfo->student_major_pref[sl];
	}

	for (i=1; i<=deptCount; i++) {
	    deptId = department_array[i]->department_id;
	    deptInfo = department_res[deptId];

	    adjusted_student_major_pref[sl][i-1] = math.safe_divide(deptInfo->student_major_pref[sl], totalPctIncludedDept);
	    // another alternative:
	    // adjusted_student_major_pref[sl][deptId] /= totalPctIncludedDept;
	}

	// 0304 change upon Jesse's latest email
	float minPct = 1.0f / deptCount;
	totalPctIncludedDept = 0;

	for (i=1; i<=deptCount; i++) {
	    // if pref < 0.1 (10%)
	    if ( adjusted_student_major_pref[sl][i-1] < minPct )
		// + 1% pref to this dept
		adjusted_student_major_pref[sl][i-1] += 0.03f;

	    totalPctIncludedDept += adjusted_student_major_pref[sl][i-1];
	}

	for (i=1; i<=deptCount; i++)
	    adjusted_student_major_pref[sl][i-1] /= totalPctIncludedDept;

	// end 0304
    }
}

//---------- End of function PlayerSchool::generate_student_major_preference -----------//

//-------- Begin of function DepartmentInfo::generate_student_major_preference -------//
//!
//! refer to techdoc3.3 section 4.7
//!
void DepartmentInfo::generate_student_major_preference() {
    short sl;
    PeerSchool *playerEx = school_res.player_peer_school;

    for (sl=0; sl<MAX_STUDENT_LEVEL_INI; sl++) {
	// refer to spreadsheet: student_par: E24 = C24*D24:
	// Preference for major = IPEDS %UG degrees * Fraction of field pct

	student_major_pref[sl]
	    = playerEx->student_ifield_pct[sl][iped_field]
	    * field_fraction_pct[sl];
    }

    //TO debug the values of student_major_pref?
}

//---------- Begin of function DeptInfo::generate_student_ajdust_arrays -----------//

//---------- Begin of function PlayerSchool::generate_student_ajdust_arrays -----------//
//!
//!
void PlayerSchool::generate_student_adjust_pct_arrays() {
    int sl, i;

    generate_student_adjust_pct_array
	(student_level_pct, MAX_STUDENT_LEVEL);

    for (sl=0; sl<MASTER_ARRAY_SIZE; sl++) {
	generate_student_adjust_pct_array
	    (year_in_program_pct[sl], MAX_GRADUATE_YEARS+1);
    }

    short deptCount = department_res.department_count;
    for( i=1 ; i<=deptCount ; i++ ) {
	generate_student_adjust_pct_array
	    (department_res[i]->doctor_year_in_program_pct, MAX_GRADUATE_YEARS+1);
    }

    for (sl=0; sl<MAX_STUDENT_LEVEL; sl++) {
	generate_student_adjust_pct_array
	    (student_gender_pct[sl], GENDER_ETHNIC_TYPE_COUNT);
    }

    for (sl=0; sl<MAX_STUDENT_LEVEL_INI; sl++) {
	generate_student_adjust_pct_array
	    (adjusted_student_major_pref[sl], department_array.department_count);
    }
}

void PlayerSchool::generate_student_adjust_pct_array(float *arr, int size) {
    int i;
    for (i=1; i<size; i++)
	arr[i] += arr[i-1];

    if ( arr[size-1] > 0.95)
	arr[size-1] = 1.0f;
    else {
	if (arr[0] != 0 )                             //1120 bug from special game_setting
	    err_here();                                 // debug_msg("PlayerSchool::generate_student_adjust_pct_array: Array Data Incorrect");
    }
}

void PlayerSchool::generate_student_readjust_pct_array(float *arr, int size) {
    int i;
    for (i=size-1; i>0; i--)
	arr[i] -= arr[i-1];

    err_when(arr[0]<0);
}

//---------- End of function PlayerSchool::generate_student_adjust_arrays -----------//

//---------- End of function PlayerSchool::generate_student_random_get_group_index -----------//
//
// <size> = number of group
// return the group index
//
char PlayerSchool::generate_student_random_get_group_index(float *arr, int size) {
    const int randomPrecesion = 10000;
    float rand = float(m.random(randomPrecesion+1)) / randomPrecesion;

    err_when(size >127);                            // since return type is char

    for ( int i=0; i<size; i++ ) {
	if ( rand <= arr[i] )
	    return (char) i;
    }
    return 0;
}

//---------- End of function PlayerSchool::generate_student_random_get_group_index -----------//

//---------- End of function PlayerSchool::get_random_major -----------//
//
// <sl> = student level
// return the dept_recno index
//
int PlayerSchool::get_random_major(char sl) {
    if ( sl == DISTANCE_LEARN)                      // 0107
	sl = UG_TRADITION;
    if ( sl >= UG_NONTRADITION )
	sl--;

    err_when(sl<0 || sl>= MAX_STUDENT_LEVEL_INI);

    int returnValue = 1 + generate_student_random_get_group_index( adjusted_student_major_pref[sl], department_array.department_count );

    if ( returnValue > department_array.size() )
	returnValue = department_array.size();

    return returnValue;                             // major is dept_recno to department_array

}

//---------- End of function PlayerSchool::get_random_major -----------//

//### begin trevor 1908 ##//

//---------- End of function PlayerSchool::generate_student_init_course -----------//
//
// Initialize previous course selection data for newly generated students.
//
/*
void PlayerSchool::generate_student_init_course(int progressIndex)
{
  //err_here();		// 990413

  for( int processYearInProgram=4 ; processYearInProgram>=2 ; processYearInProgram-- )		// only process students whose year in program equal to this
  {
    for( int deptRecno=department_array.size() ; deptRecno>0 ; deptRecno-- )
    {
      if ( deptRecno==3 || deptRecno==6 || deptRecno==9 )

if( department_array.is_deleted(deptRecno) )
continue;

department_array[deptRecno]->student_array.init_course(processYearInProgram);
}
}
}
//---------- End of function PlayerSchool::generate_student_init_course -----------//
*/
//### end trevor 1908 ##//

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//

static char cur_student_level_for_sorting = 0;

//------ Begin of function sort_stu_talent_function ------//
//!
//! Sort School in db_school_array by their distances in an ascending order.
//!
static int sort_stu_talent_function( const void *a, const void *b ) {
    // 1st sort key: trimester_tag
    // 2nd sort key: talent

    StudentStruct* stuStrPtrA, *stuStrPtrB;
    Student* stuPtrA, *stuPtrB;

    stuStrPtrA = (StudentStruct *) stuArr[cur_student_level_for_sorting].get(*((short*)a));
    stuStrPtrB = (StudentStruct *) stuArr[cur_student_level_for_sorting].get(*((short*)b));

    float rc = float(stuStrPtrA->trimester_tag - stuStrPtrB->trimester_tag);

    if( rc > 0 )
	return 1;
    else if( rc < 0 )
	return -1;

    stuPtrA = department_array[stuStrPtrA->dept_recno]->student_array[stuStrPtrA->stu_recno];
    stuPtrB = department_array[stuStrPtrB->dept_recno]->student_array[stuStrPtrB->stu_recno];

    float valueA = stuPtrA->talent_academic;
    float valueB = stuPtrB->talent_academic;

    rc = valueA - valueB;

    if( rc > 0 )
	return 1;
    else if( rc < 0 )
	return -1;
    else
	return 0;
}

//------- End of function sort_stu_talent_function ------//

//---------- Begin of function PlayerSchool::generate_student_init_course -----------//
//!
//! Initialize previous course selection data for generated students.
//! ref: CC: Revised Initialization Procedure for Student Sims (rev.11 March 1999)
//! ## fred 990602 Ccenrol_new.doc
/*
  for SL-1,2,4,5
  {
       set tag = 0

       for eachTriGroup (note: i.e. there're (8-2 = 6) TriGroups for SL-1)
       {
          generate sims for this triGroup
          assign the current tag to these sims
          for each sim generated for this triGroup AND sims created SO FAR
          {
run A-2
}
increment tag!!!
}

run B for SL-1,2,3,4,5
}

*/

void PlayerSchool::generate_student_init_course(int progressIndex) {
    int try_to_count = 0;
    const char norm_ft_trimesters[MAX_STUDENT_LEVEL] = { 8,8,2,4,8 };
    //## chea 281099
    const char no_of_time_sel_cour[MAX_STUDENT_LEVEL] = {
	2,2,1,1,2
    };

    int i;

    StudentStruct* stuStrPtr;
    Student* stuPtr;

    //---- get the maximum # of students in all student levels ----//

    int stuCount = 0;

    for( i=0 ; i<MAX_STUDENT_LEVEL ; i++ ) {
	if( stuArr[i].size() > stuCount )
	    stuCount = stuArr[i].size();
    }

    err_when( stuCount <= 0 );
    err_when( stuCount > 10000000 );

    //---- allocate the array ---------//

    short* stuSortArray = (short*) mem_add( stuCount * sizeof(short) );

    for ( char sl=0; sl<MAX_STUDENT_LEVEL; sl++ ) {
	/*
	  int tag = 1;
	  int stuCountThisLevel = stuArr[sl].size();
	  int stuCountPerTrimester = stuCountThisLevel / norm_ft_trimesters[sl];

	  // ------ Part A ------

	  for ( int t=0; t<norm_ft_trimesters[sl]-2; t++ )  //## chea 281099 for testing
	  {
          for ( i=1; i<=stuCountPerTrimester*(1+t); i++ )
          {
	  stuStrPtr = (StudentStruct *) stuArr[sl].get(i);
	  stuPtr = department_array[stuStrPtr->dept_recno]->student_array[stuStrPtr->stu_recno];

	  if ( i > stuCountPerTrimester * t )			// shoudn't be ">="
	  stuStrPtr->trimester_tag = tag;

	  err_when(stuPtr->student_level != sl);		//BUGHERE
	  stuPtr->select_course(1);		// 1: ignoreFaculty
	  }

	  tag++;
	  }
	*/

	//## chea 281099 new idea since the old alg. ignor year_in_program
	int tag = 1;
	int time_in_school;
	int stuCountThisLevel = stuArr[sl].size();

	int stuCountPerTrimester = stuCountThisLevel / norm_ft_trimesters[sl];

#ifdef DEBUG
	int studentYearCount[MAX_GRADUATE_YEARS+1];
	memset( studentYearCount, 0, sizeof(studentYearCount) );
	int studentCredit[32+1];
	memset( studentCredit, 0, sizeof(studentCredit) );
#endif

	for ( i=1; i<=stuCountThisLevel; i++ ) {

	    stuStrPtr = (StudentStruct *) stuArr[sl].get(i);
	    stuPtr = department_array[stuStrPtr->dept_recno]->student_array[stuStrPtr->stu_recno];

	    err_when(stuPtr->year_in_program == 1);
	    time_in_school = stuPtr->year_in_program-1;

#ifdef DEBUG
	    if( time_in_school > MAX_GRADUATE_YEARS )
		studentYearCount[MAX_GRADUATE_YEARS]++;
	    else if( time_in_school >= 0 )
		studentYearCount[time_in_school]++;
#endif

	    for ( int t=0; t<no_of_time_sel_cour[sl]*time_in_school; t++ ) {
		if ( stuPtr->audit_flag ) {
		    File file;

		    char fileName[123];

		    switch( stuPtr->student_level ) {
		    case UG_TRADITION:    strcpy( fileName, "TRADITION" );
			break;
		    case UG_NONTRADITION: strcpy( fileName, "NONTRADITION" );
			break;
		    case MASTER:        strcpy( fileName, "MASTER" );
			break;
		    case DOCTOR:        strcpy( fileName, "DOCTOR" );
			break;
		    }

		    strcat( fileName, m.format(stuPtr->old_student_recno) );
		    strcat( fileName, ".txt" );

		    file.file_append( fileName );
		    file.file_seek(0, FILE_END);

		    char textDisplay[128];
		    sprintf( textDisplay, "****** This is the No.%d trimester in the history ******", t );
		    WRITE_STR_FIELD( (&file), textDisplay );

		    WRITE_RECORD_SEPARATOR( (&file) );

		    file.file_close();
		}

		stuPtr->select_course(1, 1);              // 1: ignoreFaculty, 1: ignore course student count, denial count
	    }

	    //		if ( i > stuCountPerTrimester * t )			// shoudn't be ">="
	    //				stuStrPtr->trimester_tag = tag;

#ifdef DEBUG
	    if( stuPtr->total_course_all >= 32 )
		++studentCredit[32];
	    else if( stuPtr->total_course_all >= 0 )
		++studentCredit[stuPtr->total_course_all];
#endif
	    tag++;
	}

	// ------ Part B ------

	int t = norm_ft_trimesters[sl]-2;
	int tmpBound = stuCountPerTrimester * t;

	for ( i=1; i<=stuCountThisLevel; i++ ) {
	    stuStrPtr = (StudentStruct *) stuArr[sl].get(i);
	    stuPtr = department_array[stuStrPtr->dept_recno]->student_array[stuStrPtr->stu_recno];

	    if ( i > tmpBound )
		stuStrPtr->trimester_tag = tag;
	    try_to_count++;
	    if ( stuPtr->audit_flag ) {
		File file;

		char fileName[123];

		switch( stuPtr->student_level ) {
		case UG_TRADITION:    strcpy( fileName, "TRADITION" );
		    break;
		case UG_NONTRADITION: strcpy( fileName, "NONTRADITION" );
		    break;
		case MASTER:        strcpy( fileName, "MASTER" );
		    break;
		case DOCTOR:        strcpy( fileName, "DOCTOR" );
		    break;
		}

		strcat( fileName, m.format(stuPtr->old_student_recno) );
		strcat( fileName, ".txt" );

		file.file_append( fileName );
		file.file_seek(0, FILE_END);

		WRITE_STR_FIELD( (&file), "********** It is the start of the prerun year **********" );

		WRITE_RECORD_SEPARATOR( (&file) );

		file.file_close();
	    }
	    stuPtr->select_course(1);                   // 1: ignoreFaculty
	}

	// ------ Part C-1 ------ // sort tag and talent

	for( i=1; i<=stuCountThisLevel; i++ )
	    stuSortArray[i-1] = i;                      // stuArr[sl].get();

	cur_student_level_for_sorting = sl;
	qsort( stuSortArray, stuCountThisLevel, sizeof(stuSortArray[0]), sort_stu_talent_function );

	// ------ Part C-2 ------

	float *yearInProgramPctArr;

	if ( sl <= MASTER ) {
	    yearInProgramPctArr = year_in_program_pct[sl];
	}
	else if ( sl == DOCTOR ) {
	    //major = 1 + generate_student_random_get_group_index					// major is dept_recno to department_array
	    //	(adjusted_student_major_pref[SL_DOCTOR], department_array.department_count);

	    //yearInProgram = 1 + generate_student_random_get_group_index
	    int major = 1+m.random(department_array.department_count);
	    yearInProgramPctArr =
		department_res[department_array[major]->department_id]->doctor_year_in_program_pct;
	}
	else if ( sl == DISTANCE_LEARN ) {
	    yearInProgramPctArr = year_in_program_pct[UG_NONTRADITION];
	}

	// to find stuCountByYear[]
	int stuCountByYear[MAX_GRADUATE_YEARS+1];
	memset(stuCountByYear,0,sizeof(stuCountByYear));

	int chkCount = 0;
	float pct = yearInProgramPctArr[0];

	for (i=0; i<MAX_GRADUATE_YEARS+1 && pct<=1; i++) {
	    stuCountByYear[i] = int(stuCountThisLevel * yearInProgramPctArr[i]);

	    chkCount += stuCountByYear[i];
	    pct += yearInProgramPctArr[i+1];
	}

	// to eliminate the difference

	int diff = stuCountThisLevel - chkCount;
	err_when(diff < 0);

	if ( diff > 0 ) {
	    i = min(MAX_GRADUATE_YEARS, i);
	    stuCountByYear[i] += diff;
	}

	/*		for (i=MAX_GRADUATE_YEARS; i>=0 && diff>0; i--)
			{
			if ( stuCountByYear[i] >= diff )
			{
			stuCountByYear[i] -= diff;
			diff = 0;
			}
			else
			{
			diff -= stuCountByYear[i];
			stuCountByYear[i] = 0;
			}
			}*/

	// ------ Part C-3 ------

	int year = MAX_GRADUATE_YEARS+1;

	for (i=0; ; ) {
	    if ( stuCountByYear[year-1] > 0 )
		stuCountByYear[year-1]--;
	    else if ( year > 2 ) {
		year--;
		continue;
	    }
	    else
		break;

	    err_when(stuSortArray[i] < 1 || stuSortArray[i] > stuCountThisLevel);

	    stuStrPtr = (StudentStruct *) stuArr[sl].get(stuSortArray[i]);
	    stuPtr = department_array[stuStrPtr->dept_recno]->student_array[stuStrPtr->stu_recno];

	    stuPtr->year_in_program = year;

	    i++;
	}

	int finalStuCountThisLevel = i;

	// discard sims
	if ( finalStuCountThisLevel != stuCountThisLevel ) {
	    for (int j=finalStuCountThisLevel; j<stuCountThisLevel; j++) {
		stuStrPtr = (StudentStruct *) stuArr[sl].get(stuSortArray[j]);
		department_array[stuStrPtr->dept_recno]->student_array.del(stuStrPtr->stu_recno);
		stuStrPtr->dept_recno = 0;
		stuStrPtr->stu_recno = 0;
	    }
	}

	// ------ Part D ------ // admission procedure will be run in enrollment model

	// ------ Part E-1 ------

	float failRate = 0;

	for (int j=0; j<finalStuCountThisLevel; j++) {
	    stuStrPtr = (StudentStruct *) stuArr[sl].get(stuSortArray[j]);

	    // Last bug : sl=1, j=6 -> stuStrPtr = NULL
	    // stuSortArray seems corruptted
	    // chwg 072199 // avoid the bug of stuStrPtr become NULL
	    if(stuStrPtr==NULL)
		continue;
	    stuPtr = department_array[stuStrPtr->dept_recno]->student_array[stuStrPtr->stu_recno];

	    float newValue= 40+(stuPtr->talent_academic)*(.2f + .8f * ((50)-50)/100);
	    newValue = math.single_response_func(0.0f, 0.1f, -70.11f, 53.65f, newValue);

	    failRate += newValue;
	}

	// BUG FIX if divided by zero
	failRate = math.safe_divide( failRate, (float)finalStuCountThisLevel );

	// ------ Part E-2 ------

	PeerSchool* player = school_res.player_peer_school;
	const float expectedYearsToDegree[MAX_STUDENT_LEVEL] = { 4.47f, 7.65f, 1.05f, player->doc_time_to_degree-2, 7.65f };
	const char normYearsToDegree[MAX_STUDENT_LEVEL] = { 4,8,1,2,8 };

	//## chea 151099
	float lagRate = expectedYearsToDegree[sl] / normYearsToDegree[sl];
	//		float lagRate = normYearsToDegree[sl] / expectedYearsToDegree[sl];    //## chea 151099  want to change to this but I am lost

	// ------ Part E-3 ------

	//## chea 151099
	this->delay_rate[sl] = 1 - (1-lagRate) / (1-failRate);
	//		this->delay_rate[sl] = min(0.975f,max(0.8f,1 - (1-lagRate) / (1-failRate)));   //## chea 151099 want to change to this but I am lost

    }                                               // end for

    mem_del( stuSortArray );                        // sorting finished
}

//---------- End of function PlayerSchool::generate_student_init_course -----------//
