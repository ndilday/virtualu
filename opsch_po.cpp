// Filename    : OPSCH_PO.CPP
// Description : PlayerSchool player protagonist

#include <OPSCHOOL.H>
#include <ALL.H>
#include <OSYS.H>
#include <OGAMESTR.H>
#include <OMATH.H>
#include <ODEPT.H>
#include <ONEWS.H>
#include <OCHANCE.H>
#include <OENROLL.H>
#include <ODEVELOP.H>
#include <OINVEST.H>
#include <OFACILIT.H>
#include <OATHLETI.H>
#include <OLOSEGAM.H>
#include <OFINANCE.H>
#include <OBOX.H>

static float score_function( float x, float y, float mx, float my, bool test ) {
    double theta = atan2(mx, my);
    double score;
    if( test )
	// strength
	return float( x * cos(theta) + y * sin(theta));
    else
	// consistency
	return float( -x * sin(theta) + y * cos(theta));
    return (float)score;                            // returns strength if test is true; otherwise returns consistency
}

// moved to opschool.h
// return value of PlayerSchool::think_protagonist_event
// enum { COMMENT_NONE=0, COMMENT_R1, COMMENT_R2, COMMENT_R3, COMMENT_R4, COMMENT_R5, COMMENT_R6, COMMENT_TERMINATE };

static float protagonist_ck1 = 5.00f;             // 4.00f;
static float protagonist_ck2 = 2.00f;
static float protagonist_ck3 = 1.75f;
// if a student leaves he brings 10 more students to leave
#define STUDENT_LEAVE_COUNT 10
#define STUDENT_LEAVE_COUNT_STR "ten"

static void post_protagonist_message( int protagonistId, int rating, int departmentRecno, int srcDepartmentRecno, int srcPara1, short srcPara2, short srcPara3, short srcPara4 ) {
    news_array.protagonist_message(protagonistId, rating,
				   // -ve means institutional level, and magnitude indicate the departmentRecno of protagonist
				   departmentRecno == -1 ? -srcDepartmentRecno : departmentRecno,
				   srcPara1, srcPara2, srcPara3, srcPara4 );
}

static int choose_arbitary_department() {
    if( department_array.size() <= 0 )
	return 0;

    int departmentRecno = m.random( department_array.size() );
    for( int i = department_array.size(); i > 0 && department_array.is_deleted(departmentRecno); --i ) {
	if( ++departmentRecno > department_array.size() )
	    departmentRecno = 1;                        // increase and loop back
    }

    if( department_array.is_deleted(departmentRecno) )
	return 0;
    return departmentRecno;
}

enum ProtagonistFrequency {
    PROTAGONIST_YEAR,
    PROTAGONIST_TRIMESTER,
    PROTAGONIST_MONTH,
};

enum ProtagonistMsgSource {
    PROTAGONIST_NULL,
    PROTAGONIST_COMMITTEE,                          // chairman of trustee committee
    PROTAGONIST_TRUSTEE,
    PROTAGONIST_DEPARTMENT,                         // chair of xx department
    PROTAGONIST_SENATE,                             // faculty senate
    PROTAGONIST_PROFESSOR,
    PROTAGONIST_FACULTY,                            // faculty member
    PROTAGONIST_FINANCE,
    PROTAGONIST_FACILITIES,
    PROTAGONIST_ADMISSIONS,
    PROTAGONIST_POLICE,
    PROTAGONIST_STAFF,
    PROTAGONIST_STUDENT,
    PROTAGONIST_ALUMNUS,
    PROTAGONIST_ALUMNA,
};

// Month start        X03)xx.02 (04)xx.04 X06)xx.06
// Month middle
// Month end          (10)xx.24 (11)xx.26 (20)xx.28 (22)xx.29

// trimester start
// trimester end      (31)00.11 (12)00.14 X05)00.17 X02)00.21

// Year start         (27)09.10 (07)09.15 (17)09.20 (08)10.05 (23)10.10 (24)10.15 (25)11.05 (26)11.10 (28)11.15
// Year middle        (13)02.15 X14)02.20 X15)02.25 (16)03.05 (18)03.10 (19)03.15
// Year end           (21)07.11 X01)07.20 X09)07.30 (32)08.20
// Year sport season  (29)12.28 (30)05.10

                                                  //                           (institue)   (dept)
static ProtagonistInfo protagonist_info_array[MAX_PROTAGONIST_ID] = { 
    // current trustee evaluation
    //  id,dept,ex,fire, freq,                  month,day  source,                   Rm,     Zn,   +R1,  -R4,  +R1,  -R4, insert text
    {  1,   0, 0,   0, PROTAGONIST_YEAR,       7,20,     PROTAGONIST_NULL,      1.00f, 1.000f,     0,    0,    0,    0, NULL },
    // degree granted
  
    {   2,   0, 0,   0, PROTAGONIST_TRIMESTER,  0,21,     PROTAGONIST_NULL,      1.00f, 1.000f,     0,    0,    0,    0, NULL },
    // sponsored performance
  
    {   3,   0, 0,   0, PROTAGONIST_MONTH,      0, 2,     PROTAGONIST_NULL,      1.00f, 1.000f,     0,    0,    0,    0, NULL },
    // institutional prestige
  
    {	4,   0, 0,   1, PROTAGONIST_MONTH,      0, 4,     PROTAGONIST_TRUSTEE,   1.00f, 1.000f,    40,  -40,    0,    0, "institutional prestige" },
    // educational quality
  
    { 5,   0, 1,   0, PROTAGONIST_TRIMESTER,  0,17,     PROTAGONIST_NULL,      1.00f, 1.000f,     0,    0,    0,    0, NULL },
    // scholarship, broadly defined
  
    { 6,   0, 0,   0, PROTAGONIST_MONTH,      0, 6,     PROTAGONIST_NULL,      1.00f, 1.000f,     0,    0,    0,    0, NULL },
    // student diversity
  
    { 7,   0, 0,   0, PROTAGONIST_YEAR,       9,15,     PROTAGONIST_STUDENT,   1.00f, 1.000f,     0,    0,    0,    0, "student diversity" },
    // faculty diversity
  
    { 8,   1, 0,   0, PROTAGONIST_YEAR,      10, 5,     PROTAGONIST_FACULTY,   1.00f, 1.000f,     0,    0,    0,    0, "faculty diversity" },
    // percentage of alumni .. five years
  
    { 9,   0, 0,   0, PROTAGONIST_YEAR,       7,30,     PROTAGONIST_NULL,      1.00f, 1.000f,     0,    0,    0,    0, NULL },
    // faculty morale
  
    { 10,   1, 0,   1, PROTAGONIST_MONTH,      0,24,     PROTAGONIST_FACULTY,   0.25f, 5.000f,    30,  -30,   10,  -10, "faculty morale" },
    // student morale
  
    { 11,   1, 0,   1, PROTAGONIST_MONTH,      0,26,     PROTAGONIST_STUDENT,   0.25f, 5.000f,    30,  -30,   10,  -10, "student morale" },
    // staff morale
  
    { 12,   0, 0,   0, PROTAGONIST_TRIMESTER,  0,14,     PROTAGONIST_STAFF,     0.25f, 5.000f,    10,  -10,    0,    0, "staff morale" },
    // current surplus (deficit)...
  
    { 13,   0, 0,   0, PROTAGONIST_YEAR,       2,15,     PROTAGONIST_FACILITIES,2.00f, 5.000f,     0,    0,    0,    0, NULL },
    // smooth surplus (deficit)...
  
    { 14,   0, 0,   0, PROTAGONIST_YEAR,       2,20,     PROTAGONIST_NULL,      1.00f, 5.000f,     0,    0,    0,    0, NULL },
    // endowment payout rate
  
    { 15,   0, 0,   0, PROTAGONIST_YEAR,       2,25,     PROTAGONIST_NULL,      1.00f, 1.000f,     0,    0,    0,    0, NULL },
    // deferred maintenance backlog
  
    { 16,   0, 0,   0, PROTAGONIST_YEAR,       3, 5,     PROTAGONIST_FACILITIES,1.00f,-2.500f,    10,  -10,    0,    0, "the deferred maintenance backlog" },
    // undergraduate selectivity
  
    { 17,   0, 0,   1, PROTAGONIST_YEAR,       9,20,     PROTAGONIST_ADMISSIONS,1.00f, 1.000f,    40,  -40,    0,    0, "undergraduate selectivity" },
    // alumni morale
  
    { 18,   0, 0,   1, PROTAGONIST_YEAR,       3,10,     PROTAGONIST_COMMITTEE, 1.00f, 1.000f,    30,  -30,    0,    0, "alumni morale" },
    // percent alumni who donated last year
  
    { 19,   0, 0,   0, PROTAGONIST_YEAR,       3,15,     PROTAGONIST_COMMITTEE, 1.00f, 1.000f,    10,  -10,    0,    0, "the percentage of alumni who donated last year" },
    // index of endowment market value
  
    { 20,   0, 0,   0, PROTAGONIST_MONTH,      0,28,     PROTAGONIST_FINANCE,   2.00f, 0.010f,    10,  -10,    0,    0, "the investment performance of the endowment" },
    // crime index
  
    { 21,   0, 0,   0, PROTAGONIST_YEAR,       7,11,     PROTAGONIST_POLICE,    1.00f,-1.000f,    10,  -10,    0,    0, "the university's crime index" },
    // faculty research performance
  
    { 22,   1, 0,   1, PROTAGONIST_MONTH,      0,29,     PROTAGONIST_FACULTY,   0.25f,15.000f,    40,  -40,   10,  -10, "faculty research performance" },
    // faculty salaries:annual increase %
  
    { 23,   0, 0,   0, PROTAGONIST_YEAR,      10,10,     PROTAGONIST_FACULTY,   2.00f, 0.005f,    20,  -20,    0,    0, "last year's increase in faculty salaries" },
    // faculty salaries:gender-ethnic equity
  
    { 24,   0, 0,   0, PROTAGONIST_YEAR,      10,15,     PROTAGONIST_SENATE,    1.00f,-1.000f,    10,  -10,    0,    0, "gender-ethnic equity in faculty salaries" },
    // faculty salaries:departmental equity
  
    { 25,   0, 0,   0, PROTAGONIST_YEAR,      11, 5,     PROTAGONIST_DEPARTMENT,1.00f,-0.700f,    10,  -10,    0,    0, "faculty salary equity among departments" },
    // faculty rank equity
  
    { 26,   0, 0,   0, PROTAGONIST_YEAR,      11,10,     PROTAGONIST_SENATE,    1.00f,-0.500f,    10,  -10,    0,    0, "salary equity among faculty ranks" },
    // tuition:annual incrase(%)
  
    { 27,   0, 0,   0, PROTAGONIST_YEAR,       9,10,     PROTAGONIST_TRUSTEE,   2.00f, 1.000f,    20,  -20,    0,    0, "tuition increases" },
    // space shortfall
  
    { 28,   0, 0,   1, PROTAGONIST_YEAR,      11,15,     PROTAGONIST_FACILITIES,1.00f,-2.000f,    30,  -30,    0,    0, "space shortfall" },
    // football performance
 
    { 29,   0, 0,   0, PROTAGONIST_YEAR,      12,28,     PROTAGONIST_ALUMNUS,   1.00f, 1.000f,    10,  -10,    0,    0, "performance in football" },
    // basketball performance
    { 30,   0, 0,   0, PROTAGONIST_YEAR,       5,10,     PROTAGONIST_ALUMNA,    1.00f, 1.000f,    10,  -10,    0,    0, "performance in basketball" },
    // inst/dept education quality
  
    { 31,   1, 0,   1, PROTAGONIST_TRIMESTER,  0,11,     PROTAGONIST_STUDENT,   1.00f, 2.000f,    40,  -40,   10,  -10, "quality of education" },
    // inst/dept prestige
  
    { 32,   1, 0,   1, PROTAGONIST_YEAR,       8,20,     PROTAGONIST_FACULTY,   1.00f, 1.000f,    30,  -30,   10,  -10, "academic standing" },
    // course denials (% of enrollments)
  
    { 33,   1, 0,   0, PROTAGONIST_TRIMESTER,  2, 1,     PROTAGONIST_STUDENT,   1.00f,-0.250f,    20,  -20,   10,  -10, "course denials" },
    // class size deviation
  
    { 34,   1, 0,   0, PROTAGONIST_TRIMESTER,  2, 7,     PROTAGONIST_STUDENT,   1.00f,-6.000f,    20,  -20,   10,  -10, "class size deviations" },
};

// changes from ver2 spec :
// 11 (student morale) frequency change to per trimester
// 18 (alumni morale) freq change to per year
#ifdef DEBUG
void PlayerSchool::set_cks( int index, float change ) {
    switch (index) {
    case 1: protagonist_ck1 += change; break;
    case 2: protagonist_ck2 += change; break;
    case 3: protagonist_ck3 += change; break;
    }
    sys.redraw_all_flag = 1;
}

void PlayerSchool::get_cks( float &ck1, float &ck2, float &ck3 ) {
    ck1 = protagonist_ck1;
    ck2 = protagonist_ck2;
    ck3 = protagonist_ck3;
}
#endif

// generate opinion from people
void PlayerSchool::think_protagonist() {
    if( info.prerun_year || !threshold_event_flag )
	return;

    ProtagonistInfo *protagonistInfo;
    int rc = 0;

    // Current trustee evaluation (Dept:no|Freq:Year|Protagonist:na|Inserted text:na|mx:1.00|Zn:1.000|+R1:na|-R4:na|Fired?:na)

    protagonistInfo = protagonist_info_array;
    err_when( protagonistInfo->protagonist_id != 1 );

    if( protagonistInfo->is_event_date() )
	rc = think_protagonist_event( cur_game_score, game_score_history[HISTORY_MONTH_COUNT-1-12], init_game_score,
				      protagonistInfo, -1 );                      // -1=institutional level

    if( rc == COMMENT_TERMINATE )
	return;

    // S_DEGREES_GRANTED to S_DEFERRED_MAINTENANCE_BACKLOG_PERCENT
    err_when( SCORE_VAR_COUNT != 15 );
    for( int subScoreType = 0; subScoreType < SCORE_VAR_COUNT; ++subScoreType ) {
	++protagonistInfo;

	if( protagonistInfo->exist_departmental_level )
	    continue;                                   // duplicated, skip

	if( protagonistInfo->is_event_date() ) {
	    rc = think_protagonist_event( sub_score[subScoreType][HISTORY_MONTH_COUNT-1], sub_score[subScoreType][HISTORY_MONTH_COUNT-13], sub_score_start[subScoreType],
					  protagonistInfo, -1 );                    // -1=institutional level

	    if( rc == COMMENT_TERMINATE )
		return;

	    if( protagonistInfo->departmental_level ) {
		// scan each department (skip general department?)
		for( int deptRecno = 1; deptRecno <= department_array.size(); ++deptRecno ) {
		    if( department_array.is_deleted(deptRecno) )
			continue;

		    Department *deptPtr = department_array[deptRecno];

		    switch( subScoreType ) {
		    case S_FACULTY_DIVERSITY:
			rc = think_protagonist_event( deptPtr->p_faculty_diversity_history[HISTORY_YEAR_COUNT-1], deptPtr->p_faculty_diversity_history[HISTORY_YEAR_COUNT-2], deptPtr->initial_p_faculty_diversity,
						      protagonistInfo, deptRecno );     // departmental level
			break;
		    case S_FACULTY_MORALE:
			rc = think_protagonist_event( deptPtr->p_faculty_morale_history[HISTORY_MONTH_COUNT-1], deptPtr->p_faculty_morale_history[HISTORY_MONTH_COUNT-13], deptPtr->initial_p_faculty_morale,
						      protagonistInfo, deptRecno );     // departmental level
			break;
		    case S_STUDENT_MORALE:
			rc = think_protagonist_event( deptPtr->p_student_morale_history[HISTORY_TRIMESTER_COUNT-1], deptPtr->p_student_morale_history[HISTORY_TRIMESTER_COUNT-1-TRIMESTER_PER_YEAR], deptPtr->initial_p_student_morale,
						      protagonistInfo, deptRecno );     // departmental level
			break;
		    default:
			err_here();
		    }
		}
	    }
	}
    }

    // undergraduate selectivity (UG traditional)
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 17 );

    if( protagonistInfo->is_event_date() ) {
	// use formula : (1 - admitRate/100 * yieldRate/100) * 100
	rc = think_protagonist_event(
	    float(10000 - enroll_res.matric_history[UG_TRADITION][ER_OFFERS_RATE][THIS_YEAR] * enroll_res.matric_history[UG_TRADITION][ER_YIELD_RATE][THIS_YEAR]) / 100.0f,
	    float(10000 - enroll_res.matric_history[UG_TRADITION][ER_OFFERS_RATE][THIS_YEAR-1] * enroll_res.matric_history[UG_TRADITION][ER_YIELD_RATE][THIS_YEAR-1]) / 100.0f,
	    (1.0f - enroll_res.initial_offers_rate[UG_TRADITION] * enroll_res.initial_cur_yield_rate[UG_TRADITION]) * 100.0f,
	    protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // alumni morale
    ++protagonistInfo;                              // protagonistInfo->id is 18
    err_when( protagonistInfo->protagonist_id != 18 );

    if( protagonistInfo->is_event_date() ) {
	rc = think_protagonist_event( development_office.alumni_morale_history[HISTORY_YEAR_COUNT-1], development_office.alumni_morale_history[HISTORY_YEAR_COUNT-2], development_office.initial_alumni_morale,
				      protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // Percent alumni who donated ... last year
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 19 );

    if( protagonistInfo->is_event_date() ) {
	rc = think_protagonist_event( development_office.percent_alumni_gave_gift_history[HISTORY_YEAR_COUNT-1], development_office.percent_alumni_gave_gift_history[HISTORY_YEAR_COUNT-2], development_office.initial_percent_alumni_gave_gift,
				      protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // Index of endowment market value
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 20 );

    if( protagonistInfo->is_event_date() ) {
	rc = think_protagonist_event( investment_office.endowment_performance_history[THIS_MONTH],
				      investment_office.endowment_performance_history[THIS_MONTH-12],
				      investment_office.initial_endowment_performance,
				      protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // Crime Index
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 21 );

    if( protagonistInfo->is_event_date() ) {
	rc = think_protagonist_event( (float)facility_office.crime_index[THIS_MONTH], (float)facility_office.crime_index[THIS_MONTH-12], (float)facility_office.initial_crime_index,
				      protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // faculty research performance
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 22 );

    if( protagonistInfo->is_event_date() ) {
	rc = think_protagonist_event( department_array.p_faculty_research_history[THIS_MONTH], department_array.p_faculty_research_history[THIS_MONTH-12], department_array.initial_p_faculty_research,
				      protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;

	if( protagonistInfo->departmental_level ) {
	    // scan each department (skip general department?)
	    for( int deptRecno = 1; deptRecno <= department_array.size(); ++deptRecno ) {
		if( department_array.is_deleted(deptRecno) )
		    continue;

		Department *deptPtr = department_array[deptRecno];

		rc = think_protagonist_event( deptPtr->p_faculty_research_history[THIS_MONTH], deptPtr->p_faculty_research_history[THIS_MONTH-1], deptPtr->initial_p_faculty_diversity,
					      protagonistInfo, deptRecno );           // departmental level
	    }
	}
    }

    // faculty salaries:annual increase
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 23 );

    if( protagonistInfo->is_event_date() ) {
	rc = think_protagonist_event( finance.faculty_salary_increase_history[THIS_YEAR],
				      finance.faculty_salary_increase_history[THIS_YEAR-1],
				      finance.initial_faculty_salary_increase,
				      protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // faculty salaries:gender-ethnic equity
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 24 );

    if( protagonistInfo->is_event_date() ) {
	rc = think_protagonist_event( department_array.salary_gender_ethnic_equity_history[THIS_YEAR],
				      department_array.salary_gender_ethnic_equity_history[THIS_YEAR-1],
				      department_array.initial_salary_gender_ethnic_equity,
				      protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // faculty salaries:departmental equity
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 25 );

    if( protagonistInfo->is_event_date() ) {
	rc = think_protagonist_event( department_array.salary_departmental_equity_history[THIS_YEAR],
				      department_array.salary_departmental_equity_history[THIS_YEAR-1],
				      department_array.initial_salary_departmental_equity,
				      protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // faculty salaries rank equity
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 26 );

    if( protagonistInfo->is_event_date() ) {
	rc = think_protagonist_event( department_array.salary_rank_equity_history[THIS_YEAR],
				      department_array.salary_rank_equity_history[THIS_YEAR-1],
				      department_array.initial_salary_rank_equity,
				      protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // tutition:annual increase
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 27 );

    if( protagonistInfo->is_event_date() ) {
	rc = think_protagonist_event( finance.tuition_increase_history[THIS_YEAR],
				      finance.tuition_increase_history[THIS_YEAR-1],
				      finance.initial_tuition_increase,
				      protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // space shortfall
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 28 );

    if( protagonistInfo->is_event_date() ) {
	// use (projected - norm) / norm * 100
	rc = think_protagonist_event(
	    float(facility_office.projected_area[THIS_MONTH] - facility_office.normal_area[THIS_MONTH]) * 100.0f / (float)facility_office.normal_area[THIS_MONTH],
	    float(facility_office.projected_area[THIS_MONTH-12] - facility_office.normal_area[THIS_MONTH-12]) * 100.0f / (float)facility_office.normal_area[THIS_MONTH-12],
	    float(facility_office.initial_projected_area - facility_office.initial_normal_area) * 100.0f / (float)facility_office.initial_normal_area,
	    protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // football performance
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 29 );

    if( protagonistInfo->is_event_date() ) {
	// win/(win+loss)*100
	rc = think_protagonist_event(
	    (float)athletics_office.football_graph[THIS_YEAR] / float(athletics_office.football_graph[THIS_YEAR] + athletics_office.football_graph[HISTORY_YEAR_COUNT+THIS_YEAR]) * 100.0f,
	    (float)athletics_office.football_graph[THIS_YEAR-1] / float(athletics_office.football_graph[THIS_YEAR-1] + athletics_office.football_graph[HISTORY_YEAR_COUNT+THIS_YEAR-1]) * 100.0f,
	    (float)athletics_office.initial_football_graph[0] / float(athletics_office.initial_football_graph[0] + athletics_office.initial_football_graph[1]) * 100.0f,
	    protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // basketball performance
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 30 );

    if( protagonistInfo->is_event_date() ) {
	// win/(win+loss)*100
	rc = think_protagonist_event(
	    (float)athletics_office.basketball_graph[THIS_YEAR] / float(athletics_office.basketball_graph[THIS_YEAR] + athletics_office.basketball_graph[HISTORY_YEAR_COUNT+THIS_YEAR]) * 100.0f,
	    (float)athletics_office.basketball_graph[THIS_YEAR-1] / float(athletics_office.basketball_graph[THIS_YEAR-1] + athletics_office.basketball_graph[HISTORY_YEAR_COUNT+THIS_YEAR-1]) * 100.0f,
	    (float)athletics_office.initial_basketball_graph[0] / float(athletics_office.initial_basketball_graph[0] + athletics_office.initial_basketball_graph[1]) * 100.0f,
	    protagonistInfo, -1 );                      // -1=institutional level

	if( rc == COMMENT_TERMINATE )
	    return;
    }

    // educational quality
    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 31 );

    if( protagonistInfo->is_event_date() ) {
	// institutional
	rc = think_protagonist_event(
	    department_array.p_educational_quality_history[THIS_TRIMESTER],
	    department_array.p_educational_quality_history[THIS_TRIMESTER-TRIMESTER_PER_YEAR],
	    department_array.initial_p_educational_quality,
	    protagonistInfo, -1 );

	if( rc == COMMENT_TERMINATE )
	    return;

	if( protagonistInfo->departmental_level ) {
	    // scan each department (skip general department?)
	    for( int deptRecno = 1; deptRecno <= department_array.size(); ++deptRecno ) {
		if( department_array.is_deleted(deptRecno) )
		    continue;

		Department *deptPtr = department_array[deptRecno];

		rc = think_protagonist_event(
		    deptPtr->p_educational_quality_history[THIS_TRIMESTER],
		    deptPtr->p_educational_quality_history[THIS_TRIMESTER-TRIMESTER_PER_YEAR],
		    deptPtr->initial_p_educational_quality,
		    protagonistInfo, deptRecno );           // departmental level
	    }
	}
    }

    // prestige

    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 32 );

    if( protagonistInfo->is_event_date() ) {
	// institutional
	rc = think_protagonist_event(
	    department_array.p_academic_standing_history[THIS_YEAR],
	    department_array.p_academic_standing_history[THIS_YEAR-1],
	    department_array.initial_p_academic_standing,
	    protagonistInfo, -1 );

	if( rc == COMMENT_TERMINATE )
	    return;

	if( protagonistInfo->departmental_level ) {
	    // scan each department (skip general department?)
	    for( int deptRecno = 1; deptRecno <= department_array.size(); ++deptRecno ) {
		if( department_array.is_deleted(deptRecno) )
		    continue;

		Department *deptPtr = department_array[deptRecno];

		rc = think_protagonist_event(
		    deptPtr->p_academic_standing_history[THIS_YEAR],
		    deptPtr->p_academic_standing_history[THIS_YEAR-1],
		    deptPtr->initial_p_academic_standing,
		    protagonistInfo, deptRecno );           // departmental level
	    }
	}
    }

    // course denial (% of enrollments)

    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 33 );

    if( protagonistInfo->is_event_date() ) {
	// institutional

	// total admission_denied_count / total course enroll (convention_student_count?)

	// avoid divided by zero
	if( department_array.convention_student_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER] != 0
	    && department_array.convention_student_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER-TRIMESTER_PER_YEAR] != 0
	    && department_array.initial_convention_student_count[MAX_TEACHING_METHOD] != 0 ) {
	    rc = think_protagonist_event(
		(float)department_array.admission_denied_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER] * 100.0f / (float)department_array.convention_student_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER],
		(float)department_array.admission_denied_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER-TRIMESTER_PER_YEAR] * 100.0f / (float)department_array.convention_student_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER-TRIMESTER_PER_YEAR],
		(float)department_array.initial_admission_denied_count[MAX_TEACHING_METHOD] * 100.0f / (float)department_array.initial_convention_student_count[MAX_TEACHING_METHOD],
		protagonistInfo, -1 );

	    if( rc == COMMENT_TERMINATE )
		return;
	}

	if( protagonistInfo->departmental_level ) {
	    // scan each department (skip general department?)
	    for( int deptRecno = 1; deptRecno <= department_array.size(); ++deptRecno ) {
		if( department_array.is_deleted(deptRecno) )
		    continue;

		CourseArray *coursePtr = &department_array[deptRecno]->course_array;

		// avoid divided by zero
		if( coursePtr->convention_student_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER] != 0
		    && coursePtr->convention_student_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER-TRIMESTER_PER_YEAR] != 0
		    && coursePtr->initial_convention_student_count[MAX_TEACHING_METHOD] != 0 ) {
		    rc = think_protagonist_event(
			(float)coursePtr->admission_denied_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER] * 100.0f / (float)coursePtr->convention_student_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER],
			(float)coursePtr->admission_denied_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER-TRIMESTER_PER_YEAR] * 100.0f / (float)coursePtr->convention_student_count_history[MAX_TEACHING_METHOD][THIS_TRIMESTER-TRIMESTER_PER_YEAR],
			(float)coursePtr->initial_admission_denied_count[MAX_TEACHING_METHOD] * 100.0f / (float)coursePtr->initial_convention_student_count[MAX_TEACHING_METHOD],
			protagonistInfo, deptRecno );         // departmental level
		}
	    }
	}
    }

    // class size deviation

    ++protagonistInfo;
    err_when( protagonistInfo->protagonist_id != 34 );

    if( protagonistInfo->is_event_date() ) {
	// skip institutional, since department_array.size_deviation_history is not correct

	// scan each department (skip general department?)
	for( int deptRecno = 1; deptRecno <= department_array.size(); ++deptRecno ) {
	    if( department_array.is_deleted(deptRecno) )
		continue;

	    CourseArray *coursePtr = &department_array[deptRecno]->course_array;
	    for( int t = 0; t < MAX_TEACHING_METHOD; ++t ) {
		if( t == DISTANCE_LEARN_COURSE )
		    continue;                               // skip distance learning

		rc = think_protagonist_event(
		    (float)coursePtr->size_deviation_history[t][THIS_TRIMESTER],
		    (float)coursePtr->size_deviation_history[t][THIS_TRIMESTER-TRIMESTER_PER_YEAR],
		    (float)coursePtr->initial_size_deviation[t],
		    protagonistInfo, deptRecno );           // departmental level
	    }
	}
    }

}

// departmentRecno may be -1 if departmentLevel is true, meaning the whole university
// [int]testResign - if testResign is true, no action processed, just return which case belongs to
//                   if testResign is false, return which case if the action is processed (if filtered by random return 0)
//
// return 0 or which of the case (1-6 and 7 for termination)
// COMMENT_NONE=0, COMMENT_R1, COMMENT_R2, COMMENT_R3, COMMENT_R4, COMMENT_R5, COMMENT_R6, COMMENT_TERMINATE

//
int PlayerSchool::think_protagonist_event(float curValue, float lastYearValue, float initialValue, struct ProtagonistInfo *protagonistInfo, int departmentRecno, int testResign) {
    if( info.financial_year() <= 1 )                // needed for first year
	lastYearValue = initialValue;

    int departmentLevel = protagonistInfo->departmental_level;
    // institutional level message having a similar departmental message
    int existDepartmentLevel = protagonistInfo->exist_departmental_level;
    char *insertText = protagonistInfo->insert_text;
    // may cause termination (fire!)
    int causeTerminateFlag = protagonistInfo->cause_termination_flag & 1;
    float zn = protagonistInfo->normalizing_factor;
    float trend = (curValue - lastYearValue) / zn;  // 1 year before
    float level = (curValue - initialValue) / zn;
    float rm = protagonistInfo->relative_trend_threshold;
    float r1Bonus = departmentRecno == -1 ? protagonistInfo->r1_bonus_inst : protagonistInfo->r1_bonus_dept;
    float r4Penalty = departmentRecno == -1 ? protagonistInfo->r4_penalty_inst : protagonistInfo->r4_penalty_dept;

    float mx = rm * protagonist_ck1;
    float my = float( protagonist_ck1 * sqrt(5.0 / info.financial_year()) );
    float strength;
    float consistency;
    float k1;

    calc_strength_consistency_k1(curValue, lastYearValue, initialValue, protagonistInfo,strength, consistency, k1);

    float k2 = protagonist_ck2 * k1;
    float k3 = protagonist_ck3 * k2;

    //   Message from <gender-ethnic group and student level> [department]:
    // eg : Message from a non-minority female traditional undergraduate student majoring in Chemistry
    // | Message from Professor <faculty name> in the <department>:
    // | Message from Chair of Engineering Department:
    // | Message from Chair of the Faculty Senate:
    // | Message from a trustee:
    // | Message from an alumna|alumnus:

    // select srcPara
    int srcFacultyRecno = 0;
    int srcDepartmentRecno = 0;
    int srcStudentRecno = 0;
    int  srcPara1 = 0;
    short srcPara2 = 0;
    short srcPara3 = 0;
    short srcPara4 = 0;

    if( !testResign ) {                             // no need to know msg source if testing
	switch( protagonistInfo->msg_source ) {
	case PROTAGONIST_FACULTY:
	    if( departmentRecno == -1 ) {
		// random select a department
		srcDepartmentRecno = choose_arbitary_department();
	    }
	    else {
		srcDepartmentRecno = departmentRecno; {
		}

		// randomly select a faculty member in department

		if( department_array.is_deleted(srcDepartmentRecno) )
		    return 0;                             // no suitable department

		Department *deptPtr = department_array[srcDepartmentRecno];

		int facultyCount = deptPtr->faculty_array.size();
		int facultyRecno = m.random(facultyCount) + 1;
		int i;
		for( i=facultyCount; i > 0; --i ) {
		    if( !deptPtr->faculty_array.is_deleted(facultyRecno) )
			break;
		    if( ++facultyRecno > facultyCount )
			facultyRecno = 1;
		}
		if( !i )
		    return COMMENT_NONE;                  // can't find faculty

		srcFacultyRecno = facultyRecno;
		Faculty *facultyPtr = deptPtr->faculty_array[facultyRecno];
		srcPara1 = facultyPtr->first_name_id;
		srcPara2 = facultyPtr->middle_name_id;
		srcPara3 = facultyPtr->last_name_id;
		srcPara4 = facultyPtr->rank_level;
	    }
	    break;

	case PROTAGONIST_STUDENT:
	    if( departmentRecno == -1 ) {
		// random select a department
		srcDepartmentRecno = choose_arbitary_department();
	    }
	    else {
		srcDepartmentRecno = departmentRecno; {
		}

		// randomly select a student in department

		if( department_array.is_deleted(srcDepartmentRecno)
		    || department_array[srcDepartmentRecno]->student_array.size() <= 0 )
		    return COMMENT_NONE;                  // no suitable department

		StudentArray &studentArray = department_array[srcDepartmentRecno]->student_array;
		int studentCount = studentArray.size();
		int studentRecno = m.random(studentCount) + 1;
		int i;
		for( i=studentCount; i > 0; --i ) {
		    if( !studentArray.is_deleted(studentRecno) )
			break;
		    if( ++studentRecno > studentCount )
			studentRecno = 1;
		}
		if( !i )
		    return COMMENT_NONE;                  // can't find faculty

		srcStudentRecno = studentRecno;
		Student *studentPtr = studentArray[studentRecno];
		srcPara1 = studentPtr->student_level;
		srcPara2 = studentPtr->gender_ethnic_group;
		srcPara3 = studentPtr->year_in_program;
		srcPara4 = 0;                           // reserved to store number of students quit the university
	    }
	    break;

	case PROTAGONIST_DEPARTMENT:
	    if( departmentRecno == -1 ) {
		// random select a department
		srcDepartmentRecno = choose_arbitary_department();
	    }
	    else {
		srcDepartmentRecno = departmentRecno;
	    }
	    break;
	}
    }

    if( departmentRecno == -1 && causeTerminateFlag && strength < -k3 ) {
	// terminate (only for checking institutional level)

	if( testResign )
	    return COMMENT_TERMINATE;

	lose_game.set_reason(LOSE_REASON_PROTAGONIST, protagonistInfo->protagonist_id );

	// go to lost screen and then final score screen
	sys.set_staying_view_mode(MODE_LOSE_GAME);
	return COMMENT_TERMINATE;
    }
    else if( strength > k2 ) {
	if( testResign )
	    return COMMENT_R1;

	// R1, 1/4 chance for departmental level, 1.0 chance for instituation level
	if( is_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno)
	    && (departmentRecno == -1 || math.get_random_float() < 0.25f) ) {
	    // bonus_point += r1Bonus; // add bonus
	    threshold_event_bonus += (float)r1Bonus / 100.0f;

	    post_protagonist_message( protagonistInfo->protagonist_id, 1, departmentRecno, srcDepartmentRecno, srcPara1, srcPara2, srcPara3, srcPara4 );
	    News *newsPtr = news_array[news_array.recno()];
	    box.msg( newsPtr->msg() );
	    mark_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno);
	    return COMMENT_R1;
	}
    }
    else if( strength > k1 ) {
	if( testResign )
	    return COMMENT_R2;

	// R2, 1/8 chance for department and 1/2 chance for institutional level
	if( is_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno)
	    && (departmentRecno == -1 && math.get_random_float() < 0.50f
		|| departmentRecno >= 0 && math.get_random_float() < 0.125f) ) {
	    post_protagonist_message( protagonistInfo->protagonist_id, 2, departmentRecno, srcDepartmentRecno, srcPara1, srcPara2, srcPara3, srcPara4 );
	    mark_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno);
	    return COMMENT_R2;
	}
    }
    else if( strength < -k2 ) {
	if( testResign )
	    return COMMENT_R4;

	// R4 (note the order, 1/4 chance for departmental level, 1.0 chance for instituation level
	if( is_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno)
	    && (departmentRecno == -1 || math.get_random_float() < 0.25f) ) {
	    // bonus_point += r4Penalty; // r4Penalty in negative
	    threshold_event_bonus += (float)r4Penalty / 100.0f;

	    // process outcome
	    switch( protagonistInfo->protagonist_id ) {
	    case 10:                                  // faculty morale, NEWS_PROTAGONIST_FACULTY_MORALE:
	    case 22:                                  // faculty research performance, NEWS_PROTAGONIST_RESEARCH_PERFORMANCE
	    case 23:                                  // faculty salaries : annual increase, NEWS_PROTAGONIST_SALARIES_INCREASE
	    case 32:                                  // course denials, NEWS_PROTAGONIST_DEPT_PRESTIGE
		// the faculty resign
		if( srcDepartmentRecno && srcFacultyRecno ) {
		    department_array[srcDepartmentRecno]->faculty_array.del( srcFacultyRecno );
		}
		break;

	    case 11:                                  // student morale NEWS_PROTAGONIST_STUDENT_MORALE
	    case 31:                                  // education quality NEWS_PROTAGONIST_DEPT_EDUCATIONAL_QUALITY
		if( srcDepartmentRecno ) {
		    StudentArray &studentArray = department_array[srcDepartmentRecno]->student_array;

		    // this student leaves
		    studentArray.del(srcStudentRecno);

		    // delete 10 student leave
		    int deletedCount = 0;
		    for( int pass = 0; pass <= 3 && deletedCount < STUDENT_LEAVE_COUNT; ++pass ) {
			int studentCount = studentArray.size();
			int studentRecno = srcStudentRecno; // m.random(studentCount) + 1;
			for( int i=studentCount; i > 0 && deletedCount < STUDENT_LEAVE_COUNT; --i, (studentRecno=studentRecno<studentCount?studentRecno+1:1) ) {
			    if( !studentArray.is_deleted(studentRecno) ) {
				Student *studentPtr = studentArray[studentRecno];
				// check student level in pass 0, 1, 2
				if( (pass>2 || studentPtr->student_level == srcPara1 )
				    // check year in pass 0, 1
				    && (pass>1 || studentPtr->year_in_program == srcPara3 )
				    // check gender-ethnic in pass 0
				    && (pass>0 || studentPtr->gender_ethnic_group == srcPara2) ) {
				    studentArray.del(studentRecno);
				    ++deletedCount;
				}
			    }
			}
		    }

		    srcPara4 = deletedCount;
		}
		break;

	    case 18:                                  // NEWS_PROTAGONIST_ALUMNI_MORALE
	    case 19:                                  // NEWS_PROTAGONIST_ALUMNI_DONATION
		// double budget immediately
		chance_event.signal_alumni_budget_increase( 1.0f, info.financial_year());
		break;

	    case 29:                                  // NEWS_PROTAGONIST_FOOTBALL_PERFORMANCE
	    case 30:                                  // NEWS_PROTAGONIST_BASKETBALL_PERFORMANCE
		// add 200,000 to athletics budget immediately
		chance_event.signal_athletics_budget_amount( 200.0f, info.financial_year());
		break;

	    }                                           // end switch

	    post_protagonist_message( protagonistInfo->protagonist_id, 4, departmentRecno, srcDepartmentRecno, srcPara1, srcPara2, srcPara3, srcPara4 );

	    News *newsPtr = news_array[news_array.recno()];
	    box.msg( newsPtr->msg() );
	    mark_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno);

	    return COMMENT_R4;
	}
    }
    else if( strength < -k1 ) {
	if( testResign )
	    return COMMENT_R3;

	// R3, 1/8 chance for department and 1/2 chance for institutional level
	if( is_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno)
	    && (departmentRecno == -1 && math.get_random_float() < 0.50f
		|| departmentRecno >= 0 && math.get_random_float() < 0.125f) ) {
	    post_protagonist_message( protagonistInfo->protagonist_id, 3, departmentRecno, srcDepartmentRecno, srcPara1, srcPara2, srcPara3, srcPara4 );
	    mark_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno);
	    return COMMENT_R3;
	}
    }
    else if( consistency > k1 ) {
	if( testResign )
	    return COMMENT_R5;

	// R5
	if( mark_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno)
	    && (departmentRecno == -1 && math.get_random_float() < (1.0f/3.0f)
		|| departmentRecno >= 0 && math.get_random_float() < (1.0f/12.0f)) ) {
	    post_protagonist_message( protagonistInfo->protagonist_id, 5, departmentRecno, srcDepartmentRecno, srcPara1, srcPara2, srcPara3, srcPara4 );
	    mark_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno);
	    return COMMENT_R5;
	}
    }
    else if( consistency < -k1 ) {
	if( testResign )
	    return COMMENT_R6;

	// R6
	if( mark_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno)
	    && (departmentRecno == -1 && math.get_random_float() < (1.0f/3.0f)
		|| departmentRecno >= 0 && math.get_random_float() < (1.0f/12.0f)) ) {
	    post_protagonist_message( protagonistInfo->protagonist_id, 6, departmentRecno, srcDepartmentRecno, srcPara1, srcPara2, srcPara3, srcPara4 );
	    mark_protagonist_event_date(protagonistInfo->protagonist_id, departmentRecno);
	    return COMMENT_R6;
	}
    }

    return COMMENT_NONE;
}

// Kenneth
// --- Begin of PlayerSchool::get_protagonist_info --- //
//
ProtagonistInfo* PlayerSchool::get_protagonist_info(int recno) {
    err_when( recno <= 0 || recno > MAX_PROTAGONIST_ID );
    return &protagonist_info_array[recno-1];
}

//
// --- End of PlayerSchool::get_protagonist_info --- //

// --- Begin of PlayerSchool::calc_strength_consistency_k1 --- //
//
void PlayerSchool::calc_strength_consistency_k1(float curValue, float lastYearValue, float initialValue, struct ProtagonistInfo *protagonistInfo, float &strength, float &consistency, float &k1) {
    float zn = protagonistInfo->normalizing_factor;
    float trend = (curValue - lastYearValue) / zn;  // 1 year before
    float level = (curValue - initialValue) / zn;
    float rm = protagonistInfo->relative_trend_threshold;

    float mx = rm * protagonist_ck1;
    float my = float( protagonist_ck1 * sqrt(5.0 / info.financial_year()) );

    strength = score_function( trend, level, mx, my, true );
    consistency = score_function( trend, level, mx, my, false );
    k1 = score_function( mx, my, mx, my, true );

    return;
}

//
// --- End of PlayerSchool::calc_strength_consistency_k1 --- //

// --- Begin of PlayerSchool::is_protagonist_event_date --- //
//
int PlayerSchool::is_protagonist_event_date( int protagonistId, int deptRecno ) {
    err_when( protagonistId <= 0 || protagonistId > MAX_PROTAGONIST_ID );

    if( get_protagonist_info(protagonistId)->is_event_date() ) {
	int checkDate = info.game_date;
	err_when( deptRecno != -1 && (deptRecno <= 0 || deptRecno > MAX_DEPARTMENT) );

	if( deptRecno == -1 ) {
	    // institutional level message within 120 days
	    return !last_protagonist_event_date[protagonistId-1][0] || (checkDate-last_protagonist_event_date[protagonistId-1][0]>=120);
	}
	else {
	    err_when( deptRecno <= 0 || deptRecno > MAX_DEPARTMENT );
	    // departmental level message within 365 days
	    return !last_protagonist_event_date[protagonistId-1][deptRecno] || (checkDate-last_protagonist_event_date[protagonistId-1][deptRecno]>=365);
	}
    }
    return 0;
}

// --- End of PlayerSchool::is_protagonist_event_date --- //

// --- Begin of PlayerSchool::mark_protagonist_event_date --- //
//
int PlayerSchool::mark_protagonist_event_date( int protagonistId, int deptRecno ) {
    int checkDate = info.game_date;

    err_when( protagonistId <= 0 || protagonistId > MAX_PROTAGONIST_ID );

    if( deptRecno == -1 ) {
	// institutional level
	last_protagonist_event_date[protagonistId-1][0] = checkDate;
    }
    else {
	err_when( deptRecno <= 0 || deptRecno > MAX_DEPARTMENT );
	// departmental level
	last_protagonist_event_date[protagonistId-1][deptRecno] = checkDate;
    }
    return 0;
}

// --- End of PlayerSchool::mark_protagonist_event_date --- //

// parameter usage (short_para1...) for protagonist message:
// short_para1 - opinion rating (1-6:R1-R6)
// short_para2 - departmentRecno (-ve for institutional, magnitude is departmentRecno of faculty)
// short_para3 - source of message(srcPara1), gender-ethnic group of student, faculty first_name_id for faculty
// short_para4 - srcPara2, student level, faculty middle_name_id for faculty
// short_para5 - srcPara3, student gender-ethnic group, faculty last_name_id for faculty
// short_para6 - srcPara4, year_in_program of student, faculty rank_level
//
void News::protagonist_message() {
    err_when( id < NEWS_PROTAGONIST_TRUSTEE_EVALUATION || id > NEWS_PROTAGONIST_CLASS_SIZE_DEVIATION );

    ProtagonistInfo *protogonistInfo = protagonist_info_array + id - NEWS_PROTAGONIST_TRUSTEE_EVALUATION;

    // protagonist source department and faculty name res id
    str  = protogonistInfo->source_str( abs(short_para2), short_para3, short_para4, short_para5, short_para6 );
    // rating and target department
    str += protogonistInfo->rating_str( short_para1, short_para2 );

    if( short_para1 == COMMENT_R4 ) {               // case R4, may have addition outcome for some event
	switch( id ) {
	case NEWS_PROTAGONIST_FACULTY_MORALE:
	case NEWS_PROTAGONIST_RESEARCH_PERFORMANCE:
	case NEWS_PROTAGONIST_SALARIES_INCREASE:
	case NEWS_PROTAGONIST_DEPT_PRESTIGE:
	    str += " I resign!";
	    break;

	case NEWS_PROTAGONIST_STUDENT_MORALE:
	case NEWS_PROTAGONIST_DEPT_EDUCATIONAL_QUALITY:
	    if( short_para6 > 0 ) {                   // srcPara4 = number of student leave
		str += " I am leaving the university";
		if( short_para6 > 1 ) {                 // bring some friends to leave
		    str += " along with ";
		    if( short_para6 == STUDENT_LEAVE_COUNT )
			str += STUDENT_LEAVE_COUNT_STR;
		    else
			str += short_para6;
		    str += " of my friends";
		}
		str += "!";
	    }
	    break;

	case NEWS_PROTAGONIST_ALUMNI_MORALE:
	case NEWS_PROTAGONIST_ALUMNI_DONATION:
	    str += " I understand the Board has directed you to double spending on development and alumni affairs. I applaud that, but urge you to consider other actions as well.";
	    break;

	case NEWS_PROTAGONIST_FOOTBALL_PERFORMANCE:
	case NEWS_PROTAGONIST_BASKETBALL_PERFORMANCE:
	    str += " The alumni have prevailed on the Board to hire a big-time coach. This will add $200,000 a year to the athletics budget.";
	    break;
	}
    }
}

// for reasonId == LOSE_REASON_PROTAGONIST
//	reason_para0 is protagonist_id
char *LoseGameReport::get_reason_str_protagonist() {
    ProtagonistInfo *protagonistInfo = protagonist_info_array + reason_para0 - 1;
    err_when( protagonistInfo->protagonist_id != reason_para0 );

    static String str;
    err_when( protagonistInfo->insert_text == NULL );
    str = protagonistInfo->insert_text;
    char *strPtr = str;
    strPtr[0] = m.upper(strPtr[0]);
    if( protagonistInfo->normalizing_factor > 0 )
	str += " is too low!";
    else
	str += " is too high!";

    return str;
}

int ProtagonistInfo::is_event_date() {
    const int MONTH_PER_TRIMESTER = 12 / TRIMESTER_PER_YEAR;

    if( msg_source == PROTAGONIST_NULL )
	return 0;

    switch( event_freq ) {
    case PROTAGONIST_YEAR:
	if( info.game_month == event_month && info.game_day == event_day )
	    return 1;
	break;
    case PROTAGONIST_TRIMESTER:
	err_when( event_month >= MONTH_PER_TRIMESTER );
	if( info.game_month % MONTH_PER_TRIMESTER == event_month && info.game_day == event_day )
	    return 1;
	break;
    case PROTAGONIST_MONTH:
	if( info.game_day == event_day )
	    return 1;
	break;
    default: {
	    err_here();
	}
    }
    return 0;
}

char *ProtagonistInfo::source_str(int srcDepartmentRecno, int srcPara1, short srcPara2, short srcPara3, short srcPara4 ) {
    static String str;
    str = "";

    switch( msg_source ) {
    case PROTAGONIST_NULL:
	// str = "Message from an anonymous: ";
	break;
    case PROTAGONIST_COMMITTEE:
	str = "Message from Chair of trustee Development Committee: ";
	break;
    case PROTAGONIST_TRUSTEE:
	str = "Message from a trustee: ";
	break;
    case PROTAGONIST_DEPARTMENT:
	str = "Message from Chair of ";
	if( srcDepartmentRecno == 0 )
	    str += "General Department";
	else
	    str += department_array[srcDepartmentRecno]->name();
	str += ": ";
	break;
    case PROTAGONIST_SENATE:
	str = "Message from Chair of the Faculty Senate: ";
	break;
    case PROTAGONIST_PROFESSOR:
	err_here();                                 // not used, use PROTAGONIST_FACULTY instead
	break;
    case PROTAGONIST_FACULTY: {
	// srcPara1 = facultyPtr->first_name_id;
	// srcPara2 = facultyPtr->middle_name_id;
	// srcPara3 = facultyPtr->last_name_id;
	// srcPara4 = facultyPtr->rank_level;

	err_when( department_array.is_deleted(srcDepartmentRecno) );
	Department *deptPtr = department_array[srcDepartmentRecno];

	str  = "Message from ";
	//str += game_str_res.rank_str( srcPara4 );
	// str += " ";
	if( srcPara4 == FULL_PROF )
	    str += "Professor ";
	str += Faculty::name( srcPara1, srcPara2, srcPara3 );
	str += " in the ";
	str += deptPtr->name();
	str += ": ";
    }
    // randomly select a faculty member in department
    break;
    case PROTAGONIST_FINANCE:
	str = "Message from VP for Finance: ";
	break;
    case PROTAGONIST_FACILITIES:
	str = "Message from Director of Facilities: ";
	break;
    case PROTAGONIST_ADMISSIONS:
	str = "Message from Dean of Admissions: ";
	break;
    case PROTAGONIST_POLICE:
	str = "Message from Police Chief: ";
	break;
    case PROTAGONIST_STAFF:
	str = "Message from staff member: ";
	break;
    case PROTAGONIST_STUDENT:
	// randomly select a studnet in department
	// eg : Message from a non-minority female traditional undergraduate student majoring in Chemistry
	if( !srcDepartmentRecno ) {
	    str += "Message from a student: ";
	}
	else {
	    //srcPara1 = studentPtr->student_level;
	    //srcPara2 = studentPtr->gender_ethnic_group;
	    //srcPara3 = studentPtr->year_in_program;
	    //srcPara4 = (reserved to store number of students quit the university)

	    err_when( department_array.is_deleted(srcDepartmentRecno) );
	    str += "Message from a";
	    str += " ";
	    String genderStr=game_str_res.gender_ethnic_str(srcPara2);
	    str += genderStr.lower();
	    str += " ";
	    str += game_str_res.student_level_str(srcPara1);
	    str += " student majoring in ";
	    str += department_res[department_array[srcDepartmentRecno]->department_id]->name;
	    // don't use department_array[srcDepartmentRecno]->name(), we don't want department
	    str += ": ";
	}
	break;
    case PROTAGONIST_ALUMNUS:
	str = "Message from an alumnus: ";
	break;
    case PROTAGONIST_ALUMNA:
	str = "Message from an alumna: ";
	break;
    default:
	err_here();
    }

    return str;
}

// rating (R1-R6) and department

char *ProtagonistInfo::rating_str(int rating, int departmentRecno ) {
    static String str;

    str = "";

    int departmentLevel =  departmental_level;
    int existDepartmentLevel = exist_departmental_level;
    char *insertText = insert_text;
    char *targetStr = NULL;

    switch( rating ) {
    case COMMENT_R1:                              // R1, 1/4 chance for departmental level, 1.0 chance for instituation level
	str += "Congratulations";
	if( (targetStr=target_str(departmentRecno))
	    && targetStr[0] != '\0' ) {                 // not empty string
	    str += " on";
	    str += targetStr;
	}
	str += ".";
	str += " This outstanding performance reflects positively on your administration.";
	break;

    case COMMENT_R2:                              // R2, 1/8 chance for department and 1/2 chance for institutional level
	str += "I'm pleased";
	if( (targetStr=target_str(departmentRecno))
	    && targetStr[0] != '\0' ) {                 // not empty string
	    str += " with";
	    str += targetStr;
	}
	str += ".";
	str += " I hope you can do even better in the future.";
	break;

    case COMMENT_R4:                              // R4 (note the order, 1/4 chance for departmental level, 1.0 chance for instituation level
	str += "I'm bitterly disappointed";
	if( (targetStr=target_str(departmentRecno) )
	    && targetStr[0] != '\0' ) {                 // not empty string
	    str += " about";
	    str += targetStr;
	}
	str += ".";
	str += " Such unacceptable performance threatens the viability of your administration.";
	break;

    case COMMENT_R3:                              // R3, 1/8 chance for department and 1/2 chance for institutional level
	str += "I'm concerned";
	if( (targetStr=target_str(departmentRecno) )
	    && targetStr[0] != '\0' ) {                 // not empty string
	    str += " about";
	    str += targetStr;
	}
	str += ".";
	str += " You can expect complaints to intensify unless performance in this area improves.";
	break;

    case COMMENT_R5:                              // R5
	str += "My compliments on the favorable trend";
	if( (targetStr=target_str(departmentRecno) )
	    && targetStr[0] != '\0' ) {                 // not empty string
	    str += " in";
	    str += targetStr;
	}
	str += ".";
	str += " However, the level of performance remains weak.";
	break;

    case COMMENT_R6:                              // R6
	str += "I'm concerned about the unfavorable trend";
	if( (targetStr=target_str(departmentRecno) )
	    && targetStr[0] != '\0' ) {                 // not empty string
	    str += " in";
	    str += targetStr;
	}
	str += ".";
	str += " However, the level of performance remains strong.";
	break;

    default:
	err_here();
    }

    return str;
}

char *ProtagonistInfo::target_str(int departmentRecno) {
    static String str;
    str = "";

    int departmentLevel = departmental_level;
    int existDepartmentLevel = exist_departmental_level;
    char *insertText = insert_text;

    if( (departmentLevel || existDepartmentLevel) && departmentRecno < 0 )
	str += " the university's";
    if( insertText ) {
	str += " ";
	str += insertText;
    }
    if( departmentLevel && departmentRecno >= 0 ) {
	str += " in the ";
	if( departmentRecno == 0 )
	    // general department
	    str += department_res.general_dept_info.name;
	else
	    str += department_array[departmentRecno]->name();
    }

    return str;
}
