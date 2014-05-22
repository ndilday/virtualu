// Filename   : OGFILE5.CPP
// Decription : write log

#include <OGFILE.H>
#include <ALL.H>
#include <ODATE.H>
#include <OPSCHOOL.H>
#include <ODEPT.H>
#include <OCOURSE.H>
#include <ODEVELOP.H>
#include <OFACILIT.H>
#include <OFINANCE.H>
#include <OINVEST.H>
#include <OATHLETI.H>
#include <OENROLL.H>
#include <OLIBTECH.H>
#include <OMATH.H>

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
    //return gcvt( value*0.01, 7, num_str ); // a few more more than 7, for negative sign, decimal point and exponent
    gcvt( value, 7, num_str );                      // a few more more than 7, for negative sign, decimal point and exponent
    strcat( num_str, "%" );
    return num_str;
}

#define WRITE_FIELD_SEPARATOR(f)      f->file_write(FIELD_SEPARATOR, FIELD_SEPARATOR_LEN)
#define WRITE_STR_FIELD(f,s)          { f->file_write(s, strlen(s)); }
#define WRITE_NUM_FIELD(f,n)          { char *s=export_num(n); f->file_write(s, strlen(s)); }
#define WRITE_PERCENT_FIELD(f,n)      { char *s=export_percent(n); f->file_write(s, strlen(s)); }
#define WRITE_RECORD_SEPARATOR(f)     f->file_write(RECORD_SEPARATOR, RECORD_SEPARATOR_LEN)

int GameFile::create_game_log(char *fileName) {
    File file;

    file.file_create(fileName);
    if( !write_log_header(&file) ) {
	file.file_close();
	return 0;
    }

    file.file_close();
    return 1;
}

int GameFile::append_game_log(char *fileName) {
    File file;

    file.file_append(fileName);
    file.file_seek(0, FILE_END);

    if( !write_log(&file) ) {
	file.file_close();
	return 0;
    }

    file.file_close();
    return 1;
}

int GameFile::write_log_header(File* filePtr) {
    // write date field

    WRITE_STR_FIELD(filePtr, "year" );

    WRITE_FIELD_SEPARATOR(filePtr);

    WRITE_STR_FIELD(filePtr, "year/month" );

    WRITE_FIELD_SEPARATOR(filePtr);

    if( !player_school.write_log_header(filePtr) )
	return 0;

    WRITE_FIELD_SEPARATOR(filePtr);

    if( !department_array.write_log_header(filePtr) )
	return 0;

    WRITE_RECORD_SEPARATOR(filePtr);

    return 1;
}

int GameFile::write_log(File* filePtr) {
    // write date field
    // use the different between info.game_date and info.game_start_date

    int startYear = date.year(info.game_start_date);
    int startMonth = date.month(info.game_start_date);
    int curYear = info.game_year;
    int curMonth = info.game_month;
    // add 1 prerun year
    int monthDiff = (curYear-startYear+1) * 12 + curMonth - startMonth;
    WRITE_NUM_FIELD(filePtr, (float)monthDiff/12.0f );

    WRITE_FIELD_SEPARATOR(filePtr);

    WRITE_STR_FIELD(filePtr, info.date_str_ymd(curYear, curMonth, 1, INFO_DATE_DEFAULT|INFO_DATE_NO_DAY|INFO_DATE_SHORT_MONTH ) );

    WRITE_FIELD_SEPARATOR(filePtr);

    if( !player_school.write_log(filePtr) )
	return 0;

    WRITE_FIELD_SEPARATOR(filePtr);

    if( !department_array.write_log(filePtr) )
	return 0;

    WRITE_RECORD_SEPARATOR(filePtr);

    return 1;
}

int PlayerSchool::write_log_header(File *filePtr) {
    WRITE_STR_FIELD(filePtr, "GameScore");          // cummulative game score
    // Current trustee evaluation
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "TrusteeEval");

    // sub_score array
    static char *scoreFieldName[SCORE_VAR_COUNT] = {
	"Degrees", "Research$",
	"Prestige", "EdQuality", "Scholarship", "StuDivers", "FacDivers", "AlumGift%",
	"FacMorale", "StuMorale", "StaffMorale",
	"Surplus", "SmSurplus", "EndPoRate", "DefMaint",
    };

    for( int scoreType = 0; scoreType < SCORE_VAR_COUNT; ++scoreType ) {
	WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, scoreFieldName[scoreType]);
    }

    // Alumni morale
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "AlumMorale");
    // Index of endowment market value
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "EndMV");
    // Crime Index
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "Crime");
    // Space shortfall
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "SpaceShort");
    // Football performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "Football%");
    // Basketball performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "BBall%");

    // UG applications
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "UgApps");
    // UG Yield
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "UgYield");
    // UG academic talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "UgAcadTal");
    // UG extra curricular talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "UgXcurTal");
    // UG athletic talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "UgAthTal");
    // UG academic performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "UgAcadPerf");
    // UG academic satisfaction
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "UgAcadSat");
    // UG student life satisfaction
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "UgStLifeSat");
    // UG athletics satisfaction
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "UgAthdSat");
    // DL demand
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "DL_Demand");
    // DL provided
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "DL_Provide");
    // Quality of IT infra. & training
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "IT-infra");
    // Percent of courses opt.d for IT
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "IT_course%");

    return 1;
}

int PlayerSchool::write_log(File *filePtr) {
    WRITE_NUM_FIELD(filePtr, ultimate_game_score);  // "GameScore" cummulative game score
    // "TrusteeEval" Current trustee evaluation
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, cur_game_score);

    // sub_score array

    for( int scoreType = 0; scoreType < SCORE_VAR_COUNT; ++scoreType ) {
	WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, sub_score[scoreType][HISTORY_MONTH_COUNT-1]);
    }

    // "AlumMorale" Alumni morale
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, development_office.alumni_morale);
    // "EndMV" Index of endowment market value
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, investment_office.endowment_performance_history[HISTORY_MONTH_COUNT-1]);
    // "Crime" Crime Index
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, facility_office.crime_index[HISTORY_MONTH_COUNT-1]);
    // "SpaceShort" Space shortfall
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, facility_office.normal_area[HISTORY_MONTH_COUNT-1] - facility_office.projected_area[HISTORY_MONTH_COUNT-1] );
    // "Football%" Football performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, athletics_office.football_percent_win * 100.0);
    // "BBall%" Basketball performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, athletics_office.basketball_percent_win * 100.0 );

    // "UgApps" UG applications
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, enroll_res.total_applications[UG_TRADITION]+enroll_res.total_applications[UG_NONTRADITION] );
    float ugYield;
    if( enroll_res.total_offers[UG_TRADITION]+enroll_res.total_offers[UG_NONTRADITION] < 1 )
	ugYield = 0.0f;
    else
	ugYield = float(enroll_res.total_matrics[UG_TRADITION]+enroll_res.total_matrics[UG_NONTRADITION])
	    / float(enroll_res.total_offers[UG_TRADITION]+enroll_res.total_offers[UG_NONTRADITION]) * 100.0f;
    // "UgYield" UG Yield
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, ugYield );
    // "UgAcadTal" UG academic talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, talent_academic[HISTORY_MONTH_COUNT-1] );
    // "UgXcurTal" UG extra curricular talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, talent_extracurricular[HISTORY_MONTH_COUNT-1] );
    // "UgAthTal" UG athletic talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, talent_athletics[HISTORY_MONTH_COUNT-1] );
    // "UgAcadPerf" UG academic performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, performance_academic_ug[HISTORY_MONTH_COUNT-1] );
    // "UgAcadSat" UG academic satisfaction
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, satisfaction_academic[HISTORY_MONTH_COUNT-1] );
    // "UgStLifeSat" UG student life satisfaction
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, satisfaction_student_life[HISTORY_MONTH_COUNT-1] );
    // "UgAthdSat" UG athletics satisfaction
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, satisfaction_athletics[HISTORY_MONTH_COUNT-1] );

    //	WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, enroll_res.total_matrics[DISTANCE_LEARN] ); // "DL_Demand" DL Demand
    // "DL_Demand" DL Demand
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, enroll_res.matric_history[DISTANCE_LEARN][ER_CREDITS_DEMANDED][THIS_YEAR] );

    // sum of distance_credits_provided of every department
    //int sumCreditProvided = 0;
    //for( int deptRecno = 1; deptRecno <= department_array.size(); ++deptRecno )
    //{
    //	if( department_array.is_deleted(deptRecno) )
    //		continue;
    //	sumCreditProvided += department_array[deptRecno]->distance_credits_provided;
    //}
    //WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, sumCreditProvided ); // "DL_Provide" DL provided
    // "DL_Provide" DL provided
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, enroll_res.distance_credits_provided_history[THIS_TRIMESTER] );

    // "IT-infra" Quality of IT infra. & training
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, library_tech_office.internet_quality );
    // "IT_course%" Percent of courses opt.d for IT
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, library_tech_office.tech_utilization );

    return 1;
}

int DepartmentArray::write_log_header(File *filePtr) {

    WRITE_STR_FIELD(filePtr, "ResPerf");            // Faculty research performance
    // Faculty salaries:annual incr.(%)
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "FacSals%");
    // Faculty salaries:gender-ethnic equity
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "FacSals:GE");
    // Faculty salaries:departmental equity
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "FacSals:D");
    // Faculty salaries:rank equity
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "FacSals:R");
    // Tuition:annual increase
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "Tuition%");
    // Course denials (% of enrollments)
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "CseDenials");

    // department may not be created
    for( int departRecno = 1; departRecno <= MAX_DEPARTMENT; ++departRecno ) {
	WRITE_FIELD_SEPARATOR(filePtr);

	if( !Department::write_log_header(filePtr) )
	    return 0;
    }

    return 1;
}

int DepartmentArray::write_log(File *filePtr) {
    // "ResPerf" Faculty research performance
    WRITE_PERCENT_FIELD(filePtr, p_faculty_research_history[HISTORY_MONTH_COUNT-1]);
    // "FacSals%" Faculty saleries:annual incr.(%)
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, finance.expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value);
    // "FacSals:GE" Faculty salaries:gender-ethnic equity
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, salary_gender_ethnic_equity_history[THIS_YEAR]);
    // "FacSals:D" Faculty salaries:departmental equity
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, salary_departmental_equity_history[THIS_YEAR]);
    // "FacSals:R" Faculty salaries:rank equity
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, salary_rank_equity_history[THIS_YEAR]);
    // "Tuition%" Tuition:annual increase
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, finance.revenue_policy_array[PL_TUITION_RATE_GROWTH].result_value);
    // "CseDenials" Course denials (% of enrollments)
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, 100.0*math.safe_divide(admission_denied_count[MAX_TEACHING_METHOD],convention_student_count[MAX_TEACHING_METHOD]));

    for( int departRecno = 1; departRecno <= size(); ++departRecno ) {
	if( is_deleted(departRecno) )
	    continue;

	WRITE_FIELD_SEPARATOR(filePtr);

	if( !operator[](departRecno)->write_log(filePtr) )
	    return 0;
    }

    return 1;
}

int Department::write_log_header(File *filePtr) {
    WRITE_STR_FIELD(filePtr, "DeptID" );            // department Id
    // Faculty teaching talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_FacTchTal" );
    // Faculty scholarship talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_FacSchTal" );
    // Faculty research talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_FacResTal" );
    // Faculty teaching performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_FacTchPerf" );
    // Faculty scholarship performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_FacSchPerf" );
    // Faculty research performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_FacResPerf" );
    // Faculty morale
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_FacMorale" );
    // Pct faculty with spons. research
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_SponsRes%" );

    // Class size deviation from norm
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_Class_Dev%" );
    // Students denied entrance to cses
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_CseDenials" );
    // Percent failures
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_Failures%" );
    // Percent of courses opt. for IT
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_IT_course%" );
    // Outsourced credits
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_Outsource_Cr" );
    // Distance learning credits
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_STR_FIELD(filePtr, "D_DL_Credits" );

    return 1;
}

int Department::write_log(File *filePtr) {
    //	                                WRITE_NUM_FIELD(filePtr, department_id ); // "DeptID" department Id
    WRITE_STR_FIELD(filePtr, name() );              // "DeptID" department Id
    // "D_FacTchTal" Faculty teaching talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, faculty_array.talent_teaching[HISTORY_TRIMESTER_COUNT-1] );
    // "D_FacSchTal" Faculty scholarship talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, faculty_array.talent_scholarship[HISTORY_TRIMESTER_COUNT-1] );
    // "D_FacResTal" Faculty research talent
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, faculty_array.talent_research[HISTORY_TRIMESTER_COUNT-1] );
    // "D_FacTchPerf" Faculty teaching performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, faculty_array.performance_teaching[HISTORY_TRIMESTER_COUNT-1] );
    // "D_FacSchPerf" Faculty scholarship performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, faculty_array.performance_scholarship[HISTORY_TRIMESTER_COUNT-1] );
    // "D_FacResPerf" Faculty research performance
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, faculty_array.performance_research[HISTORY_TRIMESTER_COUNT-1] );
    // "D_FacMorale" Faculty morale
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, p_faculty_morale );
    // "D_SponsRes%" Pct faculty with spons. research
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, regular_faculty_with_research[HISTORY_MONTH_COUNT-1] );

                                                  // "D_Class_Dev%" Class size deviation from norm
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, course_array.size_deviation[MAX_TEACHING_METHOD] );
    float percentFailure = 0.0f;
    float courseDenial = 0.0f;
    if( course_array.convention_student_count[MAX_TEACHING_METHOD] > 0 ) {
	percentFailure = (float)course_array.failure_count[MAX_TEACHING_METHOD] / (float)course_array.convention_student_count[MAX_TEACHING_METHOD] * 100.0f;
	courseDenial = (float)course_array.admission_denied_count[MAX_TEACHING_METHOD] / (float)course_array.convention_student_count[MAX_TEACHING_METHOD] * 100.0f;
    }
    // "D_CseDenials" Students denied entrance to cses
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, courseDenial );
    // "D_Failures%" Percent failures
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, percentFailure );
    // "D_IT_course%" Percent of courses opt. for IT
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_PERCENT_FIELD(filePtr, p_tech_utilization );
    // "D_Outsource_Cr" Outsourced credits
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, course_array.outsourcing_credits[MAX_TEACHING_METHOD] );
    // WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, distance_learning_credits ); // "D_DL_Credits" Distance learning credits
    // "D_DL_Credits" Distance learning credits
    WRITE_FIELD_SEPARATOR(filePtr); WRITE_NUM_FIELD(filePtr, distance_credits_provided );

    return 1;
}