//Filename    : OFACULTA.CPP
//Description : Faculty Array Class Definition

#include <OSYS.H>
#include <OFACULTY.H>

//##### begin fred 0819 #####//
#include <OPSCHOOL.H>
#include <OFINANCE.H>
#include <ODEPT.H>
//##### end fred 0819 #####//
#include <OMATH.H>                                //## chea 221199 init talent

//----------- Begin of function FacultyArray Constructor -----//
//!
FacultyArray::FacultyArray() : DynArrayB(sizeof(Faculty), 100, DEFAULT_REUSE_INTERVAL_DAYS) {
    faculty_count = 0;

    memset(talent_teaching,0,sizeof(talent_teaching));
    memset(talent_scholarship,0,sizeof(talent_teaching));
    memset(talent_research,0,sizeof(talent_teaching));

    memset(performance_teaching,0,sizeof(talent_teaching));
    memset(performance_scholarship,0,sizeof(talent_teaching));
    memset(performance_research,0,sizeof(talent_teaching));

    memset(satisfaction_index,0,sizeof(satisfaction_index));
    memset(percent_research,0,sizeof(percent_research));

    memset(teaching_contact_hour,0,sizeof(teaching_contact_hour));
    memset(discretionary_hour_array, 0, sizeof(discretionary_hour_array));

    time_shifted_scholarship = 0;
}

//------------- End of function FacultyArray Constructor -----//

//----------- Begin of function FacultyArray Destructor ------//
//!
FacultyArray::~FacultyArray() {
    //----------------------------------//

    for( int i=size() ; i>0 ; i-- ) {
	if( !is_deleted(i) )
	    del(i,1);
    }

    //----------------------------------//
}

//------------- End of function FacultyArray Destructor ------//

//--------- Begin of function FacultyArray::add ---------//

int FacultyArray::add(int departmentRecno, int rankLevel, int genderEthnicGroup, int facultyAge, int startTeachingDate,
		      int facultySalary, int talentTeaching, int talentScholarship, int talentResearch) {

    //## chea 221199 init all talent here
    //## chea 221099 init talentTeaching here
    //## chea 021299 modifiled from math.get_random_snd(1.0f, 0.05f)
    int talent_Teaching;
    talent_Teaching = (int)(talentTeaching * math.get_random_snd(1.0f, 0.10f));
    if(talent_Teaching>=100)
	talent_Teaching =100;
    int talent_Scholarship = (int)(talentScholarship * math.get_random_snd(1.0f, 0.10f));
    if(talent_Scholarship>=100)
	talent_Scholarship =100;
    int talent_Research    = (int)(talentResearch * math.get_random_snd(1.0f, 0.10f));
    if(talent_Research>=100)
	talent_Research =100;

    Faculty faculty;

    faculty.init(departmentRecno, rankLevel, genderEthnicGroup, facultyAge, startTeachingDate, facultySalary, talent_Teaching, talent_Scholarship, talent_Research);

    linkin(&faculty);

    ((Faculty*) get())->faculty_recno = recno();

    faculty_count++;

    // update display_faculty_array && display_faculty_count in department
    Department* deptPtr = department_array[departmentRecno];

    Faculty* orgPtr = this->operator [](recno());

    deptPtr->cur_faculty_array.linkin(orgPtr);

    Faculty* facPtr = (Faculty*) deptPtr->cur_faculty_array.get(deptPtr->cur_faculty_array.size());

    facPtr->employ_status = 0;                      // New Hire

    return recno();
}

//----------- End of function FacultyArray::add -----------//

//--------- Begin of function FacultyArray::del ---------//
void FacultyArray::del(int recNo, int isQuiting)
{
    Faculty*  orgPtr = operator[](recNo);

    // update display_faculty_array && display_faculty_count in department
    Department* deptPtr = department_array[orgPtr->department_recno];

    if (!isQuiting) {
	deptPtr->cur_faculty_array.linkin( orgPtr );

	int arraySize = deptPtr->cur_faculty_array.size();

	Faculty* facPtr = (Faculty*) deptPtr->cur_faculty_array.get(arraySize);

	facPtr->employ_status = 1;                    // Department
    }

    faculty_count--;

    linkout(recNo);
}

//----------- End of function FacultyArray::del -----------//

//------ Begin of function FacultyArray::avg_discretionary_hour -----//
//!
//! Return the average discretionary hours of all faculty in this array
//! of a specific discretionary time category.
//!
int FacultyArray::avg_discretionary_hour(int discretionaryType) {
    Faculty* facultyPtr;

    int totalHour=0;

    for( int i=size() ; i>0 ; i-- ) {
	if( is_deleted(i) )
	    continue;

	facultyPtr = operator[](i);

	totalHour += facultyPtr->discretionary_hour_array[discretionaryType];
    }

    //##### begin fred 980813 #####//
    if ( faculty_count )
	return totalHour / faculty_count;
    else
	return 0;
    //##### end fred 980813 #####//
}

//--------- End of function FacultyArray::avg_discretionary_hour -----//

//--------- Begin of function FacultyArray::next_day ---------//

void FacultyArray::next_day() {
    //----- call individual faculty's next_day() function ----//

    if(info.prerun_year == 1 && info.game_date == 1722034)
	int fgdfg=0;

    for( int i=size() ; i>0 ; i-- ) {
	if( is_deleted(i) )
	    continue;

	operator[](i)->next_day();
    }

    //##### begin fred 0819 #####//
    // start of a trimester
    if ( info.game_day == player_school.trimester_array[player_school.cur_trimester].start_day
	 && info.game_month == player_school.trimester_array[player_school.cur_trimester].start_month ) {
	update_history(UPDATE_TRIMESTER);
	think_summer_teaching();
    }

    // start of a month
    if ( info.game_day == 1 ) {
	update_history(UPDATE_MONTH);
	sys.yield();

	// start of a year
	if ( info.game_month == finance.fiscal_year_start_month )
	    update_history(UPDATE_YEAR);
    }
    //##### end fred 0819 #####//
}

//----------- End of function FacultyArray::next_day -----------//

//--------- Begin of function FacultyArray::next_trimester ---------//

void FacultyArray::next_trimester() {
    int i;

    // for Faculty::think_discretionary_time()

    Department* deptPtr = department_array[department_recno];
    int total=0;                                    // = 0;	990419

    for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ )
	total += deptPtr->priority_discretionary_hour_array[i];

    if ( total > 0 ) {                              // 990531
	for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ ) {
	    deptPtr->relative_priority_discretionary_hour_array[i]
		= deptPtr->priority_discretionary_hour_array[i] * 100.0f / total;
	    err_when(deptPtr->relative_priority_discretionary_hour_array[i] < 0);
	}
    }
    else
	for ( i=0; i < DISCRETIONARY_TYPE_COUNT; i++ )
	    deptPtr->relative_priority_discretionary_hour_array[i] = 0;

    err_if ( total < 0 )                            // 990531
	err_here();

    //----- call individual faculty's next_trimester() function ----//

    for( i=size() ; i>0 ; i-- ) {
	if( is_deleted(i) )
	    continue;

	//err_when(operator[](i)->faculty_recno != i);

	operator[](i)->next_trimester();

	sys.yield();
    }
}

//----------- End of function FacultyArray::next_trimester -----------//

//--------- Begin of function FacultyArray::is_deleted ---------//

int FacultyArray::is_deleted(int recNo) {
    Faculty* facultyPtr = (Faculty*) get(recNo);

    if( facultyPtr == NULL )
	return 1;

    return facultyPtr->faculty_recno == 0;
}

//----------- End of function FacultyArray::is_deleted -----------//

//-------- Begin of function FacultyArray::get_unpacked_person --------//
//!
//! Given the packed record (with deleted facultys compressed, in broswer),
//! and return the the unpacked record ( recno passed to faculty_array[] )
//!
Faculty* FacultyArray::get_unpacked_faculty(int recNo) {
    //----- filter out deleted facultys --------//

    int i, facultyCount=0;

    for( i=1 ; i<=size() ; i++ ) {
	if( !is_deleted(i) ) {
	    if( ++facultyCount == recNo )
		return operator[](i);
	}
    }

    // ## begin chwg 0911
    return operator[](size());
    // ## end chwg 0911

    err_now( "FacultyArray::get_unpacked_faculty" );
    return NULL;
}

//----------- End of static function get_unpacked_faculty -----------//

//##### begin fred 0819 #####//
//---------- Begin of function FacultyArray::update_history -----------//
void FacultyArray::update_history(char update_flag) {
    switch (update_flag) {
    case UPDATE_MONTH:
	calc_history_monthly();
	break;
    case UPDATE_TRIMESTER:
	calc_history_trimester();
	break;
    case UPDATE_YEAR:
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

//---------- End of function FacultyArray::update_history -----------//

//---------- Begin of function FacultyArray::calc_history_trimester -----------//
void FacultyArray::calc_history_trimester() {
    int i,j;

    shift_history(talent_teaching,HISTORY_TRIMESTER_COUNT);
    shift_history(talent_scholarship,HISTORY_TRIMESTER_COUNT);
    shift_history(talent_research,HISTORY_TRIMESTER_COUNT);

    shift_history(performance_teaching,HISTORY_TRIMESTER_COUNT);
    shift_history(performance_scholarship,HISTORY_TRIMESTER_COUNT);
    shift_history(performance_research,HISTORY_TRIMESTER_COUNT);
    shift_history(percent_research,HISTORY_TRIMESTER_COUNT);

    shift_history(teaching_contact_hour,HISTORY_TRIMESTER_COUNT);
    for ( j=0; j<DISCRETIONARY_TYPE_COUNT_N_TOTAL; j++ )
	shift_history(discretionary_hour_array[j], HISTORY_TRIMESTER_COUNT);

    Faculty* facultyPtr;
    float total[8];
    // use int, not char
    int discretionaryHourTotal[DISCRETIONARY_TYPE_COUNT];
    int facCount = 0;

    memset(total, 0, sizeof(total));
    memset(discretionaryHourTotal, 0, sizeof(discretionaryHourTotal));
    time_shifted_scholarship = 0;
    int regularFacCount = 0;

    for( i=size() ; i>0 ; i-- ) {
	if( is_deleted(i) )
	    continue;

	facultyPtr = operator[](i); facCount++;

	// don't count adjunct in ver2
	if( facultyPtr->is_regular() ) {
	    total[0] += facultyPtr->talent_teaching;
	    total[1] += facultyPtr->talent_scholarship;
	    total[2] += facultyPtr->talent_research;
	    total[3] += facultyPtr->performance_teaching;
	    total[4] += facultyPtr->performance_scholarship;
	    total[5] += facultyPtr->performance_research;
	    ++regularFacCount;
	}

	total[6] += (facultyPtr->research_proposal_count) ? 1 : 0;
	total[7] += facultyPtr->teaching_contact_hour;

	for ( j=0; j<DISCRETIONARY_TYPE_COUNT; j++ )
	    discretionaryHourTotal[j] += facultyPtr->discretionary_hour_array[j];

	time_shifted_scholarship += facultyPtr->time_shifted_scholarship;
    }

    if ( !facCount )
	return;

    talent_teaching[THIS_TRIMESTER] = math.safe_divide(total[0], (float)regularFacCount);
    talent_scholarship[THIS_TRIMESTER] = math.safe_divide(total[1], (float)regularFacCount);
    talent_research[THIS_TRIMESTER] = math.safe_divide(total[2], (float)regularFacCount);
    performance_teaching[THIS_TRIMESTER] = math.safe_divide(total[3], (float)regularFacCount);
    performance_scholarship[THIS_TRIMESTER] = math.safe_divide(total[4], (float)regularFacCount);
    performance_research[THIS_TRIMESTER] = math.safe_divide(total[5], (float)regularFacCount);
    percent_research[THIS_TRIMESTER] = char(total[6] * 100 / facCount);
    teaching_contact_hour[THIS_TRIMESTER] = (total[7] / facCount);

    // 990518
    j = THIS_TRIMESTER;
    if((player_school.cur_trimester == SUMMER) &&
       // TL_summer < TL_spring
       (teaching_contact_hour[j] < teaching_contact_hour[j-1]) )
	teaching_contact_hour[j] = teaching_contact_hour[j-1];

    // include teaching_contact_hour in TOTAL
    discretionary_hour_array[DISCRETIONARY_TYPE_COUNT_N_TOTAL-1][THIS_TRIMESTER] = (total[7] / facCount);

    time_shifted_scholarship /= facCount;
    // include time shifted from scholarship
    discretionary_hour_array[DISCRETIONARY_TYPE_COUNT_N_TOTAL-1][THIS_TRIMESTER] += time_shifted_scholarship;

    for ( j=0; j<DISCRETIONARY_TYPE_COUNT; j++ ) {
	discretionary_hour_array[j][THIS_TRIMESTER] = float(discretionaryHourTotal[j]) / facCount;
	discretionary_hour_array[DISCRETIONARY_TYPE_COUNT_N_TOTAL-1][THIS_TRIMESTER] += discretionary_hour_array[j][THIS_TRIMESTER];
    }

}

//---------- End of function FacultyArray::calc_history_trimester -----------//

//---------- Begin of function FacultyArray::calc_history_monthly -----------//
void FacultyArray::calc_history_monthly() {
    shift_history(satisfaction_index,HISTORY_MONTH_COUNT);

    Faculty *facultyPtr;
    float total = 0;
    int i, facCount = 0;

    for( i=size() ; i>0 ; i-- ) {
	if( is_deleted(i) )
	    continue;

	facultyPtr = operator[](i); facCount++;

	total += facultyPtr->satisfaction_index;

    }

    if ( !facCount )
	return;

    satisfaction_index[THIS_MONTH] = total / facCount;
}

//---------- End of function FacultyArray::calc_history_monthly -----------//
//##### end fred 0819 #####//

// ##### begin Marco#### //
//---------- Begin of function FacultyArray::save_initial_data -----------//
void FacultyArray::save_initial_data() {
    int i;

    initial_teaching_contact_hour = teaching_contact_hour[HISTORY_TRIMESTER_COUNT-1];
    //	a percentage from 0 to 100
    initial_talent_teaching = talent_teaching[HISTORY_TRIMESTER_COUNT-1];
    initial_talent_scholarship = talent_scholarship[HISTORY_TRIMESTER_COUNT-1];
    initial_talent_research = talent_research[HISTORY_TRIMESTER_COUNT-1];
    //	a percentage from 0 to 100
    initial_performance_teaching = performance_teaching[HISTORY_TRIMESTER_COUNT-1];
    initial_performance_scholarship = performance_scholarship[HISTORY_TRIMESTER_COUNT-1];
    // BUGHRE change to monthly as in Faculty::update_history()
    initial_performance_research = performance_research[HISTORY_TRIMESTER_COUNT-1];

    for (i = 0; i < DISCRETIONARY_TYPE_COUNT_N_TOTAL;i++) {
	initial_discretionary_hour_array[i] = discretionary_hour_array[i][HISTORY_TRIMESTER_COUNT-1];
    }

    initial_time_shifted_scholarship = time_shifted_scholarship;
}

//---------- End of function FacultyArray::save_initial_data -----------//

// ###### End Marco ###### //

#ifdef DEBUG

//------- Begin of function FacultyArray::operator[] -----//

Faculty* FacultyArray::operator[](int recNo) {
    Faculty* FacultyPtr = (Faculty*) get(recNo);

    if( !FacultyPtr )
	err.run( "FacultyArray[] is deleted" );

    return FacultyPtr;
}

//--------- End of function FacultyArray::operator[] ----//
#endif
