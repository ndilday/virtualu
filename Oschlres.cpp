//Filename    : OSCHLRES.CPP
//Description : PeerSchool resource object
//Owner       : Fred

#include <OBOX.H>
#include <OSYS.H>
#include <OGAMESET.H>
#include <ODYNARR.H>                              // for build_peer_school_array
#include <OSCHLREC.H>
#include <OSCHREC2.H>
#include <OSCHLRES.H>
#include <OPSCHOOL.H>
#include <OPEERSCH.H>
#include <OMATH.H>
#include <OINFO.H>                                // 1027 for next_day
#include <OFACURES.H>                             // 1027 for next_day
#include <OFINANCE.H>                             // 1027 for next_day

#include <OFIELD.H>                               //for IPED_FILED_COUNT

#ifdef DEBUG
#include <STDIO.H>
#include <OLOG.H>
#endif

#include <MATH.H>                                 // sqrtf()
#define EXPO2(x)      ((x)*(x))

//---------- #define constant ------------//

#define SCHOOL_DB1    DIR_RES"UMASTER1.RES"
#define SCHOOL_DB2    DIR_RES"UMASTER2.RES"
#define PEER_SCHOOL_DB  "PEERNAME"

//--------- define static function --------//

static int sort_school_distance_function( const void *a, const void *b );
static int sort_peerschool_prestige_function( const void *a, const void *b );

//------- Begin of function SchoolRes::init -------//
//!
void SchoolRes::init() {
}

//--------- End of function SchoolRes::init ---------//

//------- Begin of function SchoolRes::deinit -------//
//!
void SchoolRes::deinit() {
    mem_del(peer_school_array);                     // which already includes player_peer_school
    if ( desired_school_array != NULL ) {
	mem_del( desired_school_array );
	desired_school_array = NULL;
	desired_school_count = 0;
    }
}

//--------- End of function SchoolRes::deinit ---------//

//------- Begin of function SchoolRes::load_db_info -------//
//!
void SchoolRes::load_db_info() {

    load_db_1();

    load_db_2();

    load_db_peer();
}

//--------- End of function SchoolRes::load_db_info ---------//

//------- Begin of function SchoolRes::free_db_info -------//
//!
void SchoolRes::free_db_info() {
    if ( !db_school_array || !peer_info_array )
	return;

    mem_del(db_school_array);
    mem_del(peer_info_array);

    peer_info_count = db_school_count = 0;
    db_school_array = NULL;
    peer_info_array = NULL;
}

//--------- End of function SchoolRes::free_db_info ---------//

//------- Begin of function SchoolRes::load_db_1 -------//
//!
void SchoolRes::load_db_1() {
    SchoolRec1   *sRec;
    School      *sPtr;
    Database  dbSchool(SCHOOL_DB1, 1);              // 1-read the entire DB into buffer

    db_school_count = (short) dbSchool.rec_count();
    db_school_array = (School*) mem_add( sizeof(School) * db_school_count );

    //------ read in department information array -------//

    memset( db_school_array, 0, sizeof(School) * db_school_count );

    for( int i=0 ; i<db_school_count ; i++ ) {
	sRec = (SchoolRec1*) dbSchool.read(i+1);
	sPtr = db_school_array+i;

	m.rtrim_fld( sPtr->name, sRec->name, sRec->NAME_LEN );
	sPtr->fice = m.atoi( sRec->fice, 8 );

	if( sRec->control[1] == 'r')                  // 'r' in "Private"
	    sPtr->control = PRIVATE;
	else
	    sPtr->control = PUBLIC;

	switch( sRec->carnegie[0] ) {                 // BUGHERE0 - to be further defined
	case 'R':                                   // "R1"
	    sPtr->carnegie = PRIVATE_SCHOOL;
	    break;

	case 'L':                                   // LA1
	    sPtr->carnegie = LIBERAL_ARTS_SCHOOL;
	    break;

	default:
	    sPtr->carnegie = COMPREHENSIVE_SCHOOL;
	    break;
	}

	sPtr->cc_sort     = m.atoi( sRec->cc_sort, 5 );
	sPtr->market_score = m.atoi( sRec->market_score, 6 );

	m.rtrim_fld( sPtr->state, sRec->state, 8 );

	sPtr->usn_wr_ranking    = m.atoi( sRec->usn_wr_ranking, 6 );
	sPtr->ave_midpt_sat_act = m.atoi( sRec->ave_midpt_sat_act, 8 );
	sPtr->freshmen_top_10_class = char(m.atof( sRec->freshmen_top_10_class, 7 ) * 100);

	switch( sRec->campus_environment[0] ) {
	case 'U':
	    sPtr->campus_environment = URBAN;
	    break;

	case 'S':
	    sPtr->campus_environment = SUBURBAN;
	    break;

	case 'R':
	    sPtr->campus_environment = RURAL;
	    break;
	}

	//BUG ? m.rtrim!?
	m.rtrim_fld( sPtr->ncaa_football_division, sRec->ncaa_football_division, 8 );
	m.rtrim_fld( sPtr->ncaa_basketball_division, sRec->ncaa_basketball_division, 8 );
	sPtr->football_rating = m.atoi( sRec->football_rating, 6 );
	sPtr->basketball_rating = m.atoi( sRec->basketball_rating, 6 );

	sPtr->in_state_tuition = m.atoi( sRec->in_state_tuition, 7 );
	sPtr->out_state_tuition = m.atoi( sRec->out_state_tuition, 7 );

	sPtr->room_and_board_charges = m.atoi( sRec->room_and_board_charges, 8 );
	sPtr->ug_in_housing_percent = m.atoi( sRec->undergrad_in_housing_percent, 8 );

	//---------------------------------------//

	// PeerFacultyRec *pfRec = sRec->peer_faculty_rec_array;
	// PeerFaculty    *pfPtr = sPtr->peer_faculty_array;
	SchoolFacultyRec *pfRec = sRec->school_faculty_rec_array;
	SchoolFaculty    *pfPtr = sPtr->school_faculty_array;

	for( int j=0 ; j<FACULTY_RANK_LEVEL_COUNT-2 ; j++, pfRec++, pfPtr++ ) {
	    pfPtr->faculty_count    = m.atoi( pfRec->faculty_count, 8 );
	    pfPtr->salary           = m.atoi( pfRec->salary, 8 );
	    pfPtr->female_percent   = (float)m.atof( pfRec->female_percent, 8 ) * 100;
	    pfPtr->minority_percent = (float)m.atof( pfRec->minority_percent, 8 ) * 100;
	}

	SchoolFacultyRec2 *pfRec2 = sRec->school_faculty_rec_array2;

	for( int j=0; j<2 ; j++, pfRec2++, pfPtr++ ) {
	    pfPtr->faculty_count    = m.atoi( pfRec2->adjunct_count, 8 );
	    pfPtr->salary        = 0;                   // dummy field
	    pfPtr->female_percent   = (float) m.atof( pfRec2->female_percent, 8 ) * 100;
	    pfPtr->minority_percent = (float) m.atof( pfRec2->minority_percent, 8 ) * 100; {
	    }

	    //##### begin fred 0523 #####//
	    // swap the two 'struct' since we use the following order
	    // in faculty.h which is no the same with the orderin database
	    //enum { ASST_PROF, ASSOC_PROF, FULL_PROF, LONG_TERM_ADJUNCT, SHORT_TERM_ADJUNCT };	// 3 professers & 2 adjuncts
	    SchoolFaculty tmpFac;
	    memcpy(&tmpFac, &(sPtr->school_faculty_array[FULL_PROF]), sizeof(SchoolFaculty));
	    memcpy(&(sPtr->school_faculty_array[FULL_PROF]), &(sPtr->school_faculty_array[ASST_PROF]), sizeof(SchoolFaculty));
	    memcpy(&(sPtr->school_faculty_array[ASST_PROF]), &tmpFac, sizeof(SchoolFaculty));
	}
	//##### end fred 0523 #####//

	sPtr->parttime_faculty_percent = (float) m.atof( sRec->parttime_faculty_percent, 8 ) * 100;

	//--------------------------------------//

	//----------------------------------------//
	// fred 0501
	sPtr->student_applications = m.atoi( sRec->student_applications, 6 );
	sPtr->student_accepted = m.atoi( sRec->student_accepted, 6 );
	sPtr->student_enrolled = m.atoi( sRec->student_enrolled, 6 );

	//sPtr->student_get_bacc_in_5_year_percent = m.atoi( sRec->student_get_bacc_in_5_year_percent, 6 );

	//## chea 130899 try to lower the grads rate
	char getfromrec = m.atoi( sRec->student_get_bacc_in_5_year_percent, 6 );
	float final_adj_result = (float)getfromrec * (player_school.ug_get_degree_in_5year_percent /100.0f);
	sPtr->student_get_bacc_in_5_year_percent = (char)final_adj_result;

	sPtr->freshmen_applied_for_aid = m.atoi( sRec->freshmen_applied_for_aid, 7 );
	sPtr->percent_freshmen_applied_or_aid = (float) m.atof( sRec->percent_freshmen_applied_or_aid, 10 ) * 100;
	sPtr->freshmen_with_need = m.atoi( sRec->freshmen_with_need, 8 );
	sPtr->percent_freshmen_with_need = (float) m.atof( sRec->percent_freshmen_with_need, 9 ) * 100;
	sPtr->freshmen_offered_aid = m.atoi( sRec->freshmen_offered_aid, 8 );
	sPtr->percent_freshmen_offered_aid = (float) m.atof( sRec->percent_freshmen_offered_aid, 9 ) * 100;

	sPtr->freshmen_offered_full_aid = m.atoi( sRec->freshmen_offered_full_aid, 8 );
	sPtr->percent_freshmen_offered_full_aid = (float) m.atof( sRec->percent_freshmen_offered_full_aid, 8 ) * 100;

	sPtr->percent_freshmen_with_fin_aid = m.atoi( sRec->percent_freshmen_with_fin_aid, 8 );
	sPtr->percent_continuing_student_with_fin_aid = m.atoi( sRec->percent_continuing_student_with_fin_aid, 8 );

	sPtr->institutional_aid_per_fte = m.atoi( sRec->institutional_aid_per_fte, 9 );

	//----------- revenue data -------------//
	sPtr->total_gross_tuition_revenue = m.atoi( sRec->total_gross_tuition_revenue, 8 );
	sPtr->total_financial_aid = m.atoi( sRec->total_financial_aid, 8 );
	sPtr->total_sponsored_research = m.atoi( sRec->total_sponsored_research, 8 );
	sPtr->adjusted_total_sponsored_research = m.atoi( sRec->adjusted_total_sponsored_research, 8 );
	sPtr->raw_endowment_spending = m.atoi( sRec->raw_endowment_spending, 8 );
	sPtr->state_and_local_appropriations = m.atoi( sRec->state_and_local_appropriations, 8 );
	sPtr->total_gifts = m.atoi( sRec->total_gifts, 8 );
	sPtr->gifts_to_operations = m.atoi( sRec->gifts_to_operations, 8 );
	sPtr->athletics_revenue = m.atoi( sRec->athletics_revenue, 8 );

	sPtr->adjusted_other_operating_income = m.atoi( sRec->adjusted_other_operating_income, 8 );
	sPtr->raw_other_operating_income = m.atoi( sRec->raw_other_operating_income, 8 );

	//----------- expense data -------------//
	sPtr->academic_dept_total_salaries = m.atoi( sRec->academic_dept_total_salaries, 9 );
	sPtr->academic_dept_faculty_salaries = m.atoi( sRec->academic_dept_faculty_salaries, 9 );
	sPtr->academic_dept_other_expense = m.atoi( sRec->academic_dept_other_expense, 9 );

	sPtr->sponsored_research_total_salaries = m.atoi( sRec->sponsored_research_total_salaries, 9 );
	sPtr->sponsored_research_faculty_salaries = m.atoi( sRec->sponsored_research_faculty_salaries, 9 );
	sPtr->sponsored_research_other_expense = m.atoi( sRec->sponsored_research_other_expense, 9 );

	sPtr->library_salaries = m.atoi( sRec->library_salaries, 10 );
	sPtr->library_other_expense = m.atoi( sRec->library_other_expense, 10 );

	sPtr->student_life_salaries = m.atoi( sRec->student_life_salaries, 10 );
	sPtr->student_life_other_expense = m.atoi( sRec->student_life_other_expense, 10 );

	sPtr->inst_support_salaries = m.atoi( sRec->inst_support_salaries, 10 );
	sPtr->inst_support_other_expense = m.atoi( sRec->inst_support_other_expense, 10 );

	sPtr->inst_support_net_salaries = m.atoi( sRec->inst_support_net_salaries, 10 );
	sPtr->inst_support_net_other_expense = m.atoi( sRec->inst_support_net_other_expense, 10 );
	sPtr->inst_advancement_salaries = m.atoi( sRec->inst_advancement_salaries, 10 );
	sPtr->inst_advancement_other_expense = m.atoi( sRec->inst_advancement_other_expense, 10 );
	sPtr->o_and_m_salaries = m.atoi( sRec->o_and_m_salaries, 8 );
	sPtr->o_and_m_other_expense = m.atoi( sRec->o_and_m_other_expense, 8 );
	sPtr->athletics_salaries = m.atoi( sRec->athletics_salaries, 8 );
	sPtr->athletics_other_expense = m.atoi( sRec->athletics_other_expense, 8 );

	sPtr->academic_support_salaries = m.atoi( sRec->academic_support_salaries, 10 );
	sPtr->academic_support_other_expense = m.atoi( sRec->academic_support_other_expense, 10 );

	// field 87	// fred 0501
	sPtr->end_of_year_endowment_market_value = m.atoi( sRec->end_of_year_endowment_market_value, 8 );
	sPtr->current_funds_balance = m.atoi( sRec->current_funds_balance, 8 );
	sPtr->year_end_book_value_of_plant = m.atoi( sRec->year_end_book_value_of_plant, 8 );
	sPtr->year_end_market_value_of_plant = m.atoi( sRec->year_end_market_value_of_plant, 8 );
	sPtr->year_end_capital_reserve = m.atoi( sRec->year_end_capital_reserve, 8 );
	sPtr->year_end_general_plant_and_residence_hall_debt = m.atoi( sRec->year_end_general_plant_and_residence_hall_debt, 8 );
	sPtr->residence_hall_debt = m.atoi( sRec->residence_hall_debt, 8 );

	sPtr->raw_surplus = m.atoi( sRec->raw_surplus, 8 );
	sPtr->adjusted_surplus = m.atoi( sRec->adjusted_surplus, 8 );
	sPtr->transfer_to_plant = m.atoi( sRec->transfer_to_plant, 8 );
	sPtr->amount_added_to_other_operating_income = m.atoi( sRec->amount_added_to_other_operating_income, 8 );

	// field 98
	sPtr->full_time_undergrad = m.atoi( sRec->full_time_undergrad, 8 );
	sPtr->part_time_undergrad = m.atoi( sRec->part_time_undergrad, 8 );
	sPtr->master_and_professional = m.atoi( sRec->master_and_professional, 8 );
	sPtr->non_degree_seeking = m.atoi( sRec->non_degree_seeking, 8 );
	sPtr->female_undergrad_percent = char(100 * m.atof( sRec->female_undergrad_percent, 7 ));
	sPtr->female_grad_and_prof_percent = char(100 * m.atof( sRec->female_grad_and_prof_percent, 7 ));

	sPtr->minority_undergrad_percent = char(100 * m.atof( sRec->minority_undergrad_percent, 7 ));
	sPtr->minority_grad_and_prof_percent = char(100 * m.atof( sRec->minority_grad_and_prof_percent, 7 ));
	sPtr->non_resident_alien_grad_and_prof_percent = char(100 * m.atof( sRec->non_resident_alien_grad_and_prof_percent, 7 ));
	sPtr->full_time_freshmen = m.atoi( sRec->full_time_freshmen, 9 );
	sPtr->freshmen_from_within_the_state_percent = char(100 * m.atof( sRec->freshmen_from_within_the_state_percent, 9 ));
	sPtr->total_bach_degrees = m.atoi( sRec->total_bach_degrees, 9 );
	sPtr->total_masters_and_prof_degree = m.atoi( sRec->total_masters_and_prof_degree, 9 );
	sPtr->total_doctoral_degrees = m.atoi( sRec->total_doctoral_degrees, 9 );
	sPtr->total_certificates = m.atoi( sRec->total_certificates, 9 );

	// field 113
	sPtr->enrollment_fte = m.atoi( sRec->enrollment_fte, 7 );
	sPtr->ug_percent = char(100 * m.atof( sRec->undergrad_percent, 7 ));
	sPtr->part_time_ug_percent = char(100 * m.atof( sRec->part_time_undergrad_percent, 7 ));
	sPtr->non_degree_seeking_percent = char(100 * m.atof( sRec->non_degree_seeking_percent, 7 ));

	sPtr->applications_ratio = (float) m.atof( sRec->applications_ratio, 7 );
	sPtr->yield_rate = char(100 * m.atof( sRec->yield_rate, 7 ));
	sPtr->athletics_rating = m.atoi( sRec->athletics_rating, 9 );

	sPtr->total_e_and_g_expenditure = m.atoi( sRec->total_e_and_g_expenditure, 9 );
	sPtr->net_tutition_reenue_as_percent_of_e_and_g_expese = char(100 * m.atof( sRec->net_tutition_reenue_as_percent_of_e_and_g_expese, 9 ));
	sPtr->total_tenureline_faculty = m.atoi( sRec->total_tenureline_faculty, 9 );
	sPtr->sponsored_research_per_reg_faculty = (float) m.atof( sRec->sponsored_research_per_reg_faculty, 9 );
	sPtr->doctoral_degrees_per_reg_faculty = (float) m.atof( sRec->doctoral_degrees_per_reg_faculty, 9 );

	sPtr->overhead_rate = char(100 * m.atof( sRec->overhead_rate, 7 ));
	sPtr->endowment_spending_rate = 100 * (float) m.atof( sRec->endowment_spending_rate, 7 );
	sPtr->enrollment_masters = m.atoi( sRec->enrollment_masters, 8 );
	sPtr->enrollment_doctoral = m.atoi( sRec->enrollment_doctoral, 8 );
    }
}

//--------- End of function SchoolRes::load_db_1 ---------//

//------- Begin of function SchoolRes::load_db_2 -------//
//!
void SchoolRes::load_db_2() {
    SchoolRec2   *sRec;
    School      *sPtr;
    Database  dbSchool(SCHOOL_DB2, 1);              // 1-read the entire DB into buffer

    for( int i=0 ; i<db_school_count ; i++ ) {
	sRec = (SchoolRec2*) dbSchool.read(i+1);
	sPtr = db_school_array+i;

	// ---------- umaster2 ------------- //
	sPtr->target_student_intake_sl[0] = m.atoi( sRec->target_student_intake_sl1, 7 );
	sPtr->target_student_intake_sl[1] = m.atoi( sRec->target_student_intake_sl2, 7 );
	sPtr->target_student_intake_sl[2] = m.atoi( sRec->target_student_intake_sl3, 7 );
	sPtr->target_student_intake_sl[3] = m.atoi( sRec->target_student_intake_sl4, 7 );
	sPtr->target_student_intake_sl[4] = m.atoi( sRec->target_student_intake_sl5, 7 );

	//BUG ? 9
	sPtr->dropout_rate_sl[0] = 100 * (float) m.atof( sRec->dropout_rate_sl1, 9 );
	sPtr->dropout_rate_sl[1] = 100 * (float) m.atof( sRec->dropout_rate_sl2, 6 );
	sPtr->dropout_rate_sl[2] = 100 * (float) m.atof( sRec->dropout_rate_sl3, 6 );
	sPtr->dropout_rate_sl[3] = 100 * (float) m.atof( sRec->dropout_rate_sl4, 6 );
	sPtr->dropout_rate_sl[4] = 100 * (float) m.atof( sRec->dropout_rate_sl5, 6 );

	sPtr->target_grad_rate_sl[0] = 100 * (float) m.atof( sRec->target_grad_rate_sl1, 6 );
	sPtr->target_grad_rate_sl[1] = 100 * (float) m.atof( sRec->target_grad_rate_sl2, 6 );
	sPtr->target_grad_rate_sl[2] = 100 * (float) m.atof( sRec->target_grad_rate_sl3, 6 );
	sPtr->target_grad_rate_sl[3] = 100 * (float) m.atof( sRec->target_grad_rate_sl4, 6 );
	sPtr->target_grad_rate_sl[4] = 100 * (float) m.atof( sRec->target_grad_rate_sl5, 6 );

	// field 16
	sPtr->percent_ug_students_on_aid = char(100 * m.atof( sRec->pct_ug_students_on_aid, 7 ));
	sPtr->endowment_per_student = (float) m.atof( sRec->endowment_per_student, 10 );
	sPtr->endowment_per_tenureline_faculty_member = (float) m.atof( sRec->endowment_per_tenureline_faculty_member, 11 );
	sPtr->ratio_of_current_funds_balance_to_total_operating_expenditure = (float) m.atof( sRec->ratio_of_current_funds_balance_to_total_operating_expenditure, 10 );
	sPtr->financial_condition = m.atoi( sRec->financial_condition, 10 );
	sPtr->number_of_incoming_ug_students = m.atoi( sRec->number_of_incoming_ug_students, 10 );
	sPtr->female_student_percent = char(100 * m.atof( sRec->female_student_percent, 9 ));
	sPtr->minority_student_percent = char(100 * m.atof( sRec->minority_student_percent, 9 ));

	//## chea 160799 begin sponsored_research
	//		if(player_school.sponsored_research_intensity==5)
	sPtr->sponsored_research_rating = m.atoi( sRec->sponsored_research_rating, 9 );
	//		if(player_school.sponsored_research_intensity==10)
	//			sPtr->sponsored_research_rating = 10;
	//## chea 160799 end sponsored_research

	sPtr->doctoral_degres_per_faculty_rating = m.atoi( sRec->doctoral_degres_per_faculty_rating, 9 );
	sPtr->adj_freshmen_top_ten_percent_of_class = char(100 * m.atof( sRec->adj_freshmen_top_ten_percent_of_class, 9 ));
	sPtr->apps_ratio2 = (float) m.atof( sRec->apps_ratio2, 8 );
	sPtr->yield_rate2 = (float) m.atof( sRec->yield_rate2, 8 );
	sPtr->athletics_rating2 = m.atoi( sRec->athletics_rating2, 8 );
	sPtr->net_tuition_revenue_as_percent_of_e_and_g_expense = char(100 * m.atof( sRec->net_tuition_revenue_as_percent_of_e_and_g_expense, 8 ));
	sPtr->doc_time_to_degree = (float) m.atof( sRec->doc_time_to_degree, 11 );
    }
}

//--------- End of function SchoolRes::load_db_2 ---------//

//------- Begin of function SchoolRes::load_db_peer -------//
//!
void SchoolRes::load_db_peer() {
    PeerRec   *pRec;
    PeerInfo    *pPtr;
    // dbSchool(PEER_SCHOOL_DB, 1);		// 1-read the entire DB into buffer
    Database    *dbSchool = game_set.open_db(PEER_SCHOOL_DB);

    peer_info_count = (short) dbSchool->rec_count();
    peer_info_array = (PeerInfo*) mem_add( sizeof(PeerInfo) * peer_info_count );

    //------ read in school array -------//

    memset( peer_info_array, 0, sizeof(PeerInfo) * peer_info_count );

    for( int i=0 ; i<peer_info_count; i++ ) {
	pRec = (PeerRec*) dbSchool->read(i+1);
	pPtr = peer_info_array+i;

	m.rtrim_fld( pPtr->name, pRec->name, pRec->NAME_LEN );
    }
}

//------- End of function SchoolRes::load_db_peer -------//

//--------- Begin of function SchoolRes::init_game -----------//
//!
//! This function initializes:
//!
//! - SchoolRes::player_school_ex
//! - SchoolRes::peer_school_array[]
//!
void SchoolRes::init_game() {
    //-----------------------------------------------------------------//
    // Read UMASTER1.DBF and UMASTER2.DBF to SchoolRes::db_school_array[]
    // using SchoolRec1 and SchoolRec2 as the header for reading.
    // Refer to the "Main" sheet in the "Master" Excel file.
    //-----------------------------------------------------------------//

    load_db_info();

    // Item 7a)

    // --------------------------------------------------------------- //
    // Get the records in db_school_array[] with the desired type
    // Public or Private. And store it into
    // SchoolRes::desired_school_array[]
    // ---------------------------------------------------------------- //

    get_desired_school_array();

    // ---------------------------------------------------------------- //
    // Calculate a Distance value for all records in
    // SchoolRes::desired_school_array[],
    // The varaibles need to compare are just Number of on-campus students
    // and Sponsored research intensity.
    // ---------------------------------------------------------------- //

    for ( int i=0; i<desired_school_count; i++ )
	desired_school_array[i].calc_distance_first();

    // --------------------------------------------------------------- //
    // Get 50 records in desired_school_array[] with the smallest
    // distance value. And they will store into desired_school_array again
    // --------------------------------------------------------------- //

    get_50_school_array();

    //-----------------------------------------------------------------//
    // Calculate a Distance value for all records in
    // SchoolRes::db_school_array[].
    // Refer to the "Indices" sheet in the "Initialization" Excel file.
    //-----------------------------------------------------------------//

    School::calc_distance_all_school();

    //-----------------------------------------------------------------//
    // Get 10 records in db_school_array[] with the smallest distance values
    // and store their record number in db_school_array[] in school_ex_array[].
    //-----------------------------------------------------------------//

    build_school_ex_array();

    //---- initialize player_school & player_school_ex ----//

    init_player_school_ex();

    //----- select and build peer schools in peer_school_array from db_school_array -----//

    build_peer_school_array();

    PeerSchool::calc_average_faculty_salary();

    //-----------------------------------//

    free_db_info();
}

//---------- End of function SchoolRes::init_game -----------//

//-------- Begin of function SchoolRes::build_school_ex_array --------//
//!
//! Build school_ex_array[] for calculating player_school_ex later.
//!
void SchoolRes::build_school_ex_array() {
    //-------------------------------------------------//
    //---- create an array for sorting on distance ----//
    short* schoolSortArray = (short*) mem_add( desired_school_count * sizeof(short) );
    for( int i=0 ; i<desired_school_count ; i++ )
	schoolSortArray[i] = i;

    //---- sort School in db_school_array[] by distance ----//

    qsort( schoolSortArray, desired_school_count, sizeof(schoolSortArray[0]), sort_school_distance_function );

    //-----------------------------------------------------------------//
    // Get 10 records in db_school_array[] with the smallest distance values
    // and store their record number in db_school_array[] in school_ex_array[].
    //
    // Call SchoolEx::init() to calculate a set of new variables in
    // school_ex_array[] based on vars in db_school_array[]. Refer to
    // the "Init Cond" sheet in the "Initialization" Excel file.
    //-----------------------------------------------------------------//

    //##### begin 0825 fred #####//
    //BUGHERE for debug
    /*
      schoolSortArray[0]=(18);		//18
      schoolSortArray[1]=(132);		//132
      schoolSortArray[2]=(58);
      schoolSortArray[3]=(140);
      schoolSortArray[4]=(64);

      schoolSortArray[5]=(4);		// harvard
      schoolSortArray[6]=(130);
      schoolSortArray[7]=(53);
      schoolSortArray[8]=(6);
      schoolSortArray[9]=(50);
    */
    //##### end 0825 fred #####//

    //---- init school_ex_array -----//

    // read database for "degree_by_field" sheet of excel file "HE.Data_xfm.Master"
    //
    //Database 	dbSchool(SCHOOL_DB3, 0);	// 1-read the entire DB into buffer
    //short db3Count = (short) dbSchool.rec_count();
    //err_when(db3Count != db_school_count);

    for( int i=0 ; i<MAX_SCHOOL_EX_FOR_INIT_CONDITION ; i++ ) {
	//SchoolDegreeRec* sRec = (SchoolDegreeRec*) dbSchool.read(schoolSortArray[i]);
	school_ex_array[i].init( schoolSortArray[i] );
    }

    mem_del( schoolSortArray );
}

//--------- End of function SchoolRes::build_school_ex_array ---------//

//-------- Begin of function SchoolRes::init_player_school_ex --------//
//!
//! Initialize player_school_ex by averaging school_ex_array[]
//!
void SchoolRes::init_player_school_ex() {
    player_peer_school = (PeerSchool *)mem_add(sizeof(PeerSchool));
    memset(player_peer_school, 0 ,sizeof(PeerSchool));

    strcpy(player_peer_school->name, player_school.school_name);
    // some members of SchoolEx have the same name as members of School

    //-------------------------------------//

#define AVERAGE_SCHOOL_EX_VAR_FLOAT(varName) \
    player_peer_school->varName = math.average_float( (char*) school_ex_array, sizeof(SchoolEx), MAX_SCHOOL_EX_FOR_INIT_CONDITION, \
      (char*) &(school_ex_array[0].varName) - (char*) &(school_ex_array[0]) )

#define AVERAGE_SCHOOL_EX_VAR_INT(varName) \
      player_peer_school->varName = math.average_int( (char*) school_ex_array, sizeof(SchoolEx), MAX_SCHOOL_EX_FOR_INIT_CONDITION, \
      (char*) &(school_ex_array[0].varName) - (char*) &(school_ex_array[0]) )

#define AVERAGE_SCHOOL_EX_VAR_SHORT(varName) \
      player_peer_school->varName = math.average_short( (char*) school_ex_array, sizeof(SchoolEx), MAX_SCHOOL_EX_FOR_INIT_CONDITION, \
      (char*) &(school_ex_array[0].varName) - (char*) &(school_ex_array[0]) )

#define AVERAGE_SCHOOL_EX_VAR_CHAR(varName) \
      player_peer_school->varName = math.average_char( (char*) school_ex_array, sizeof(SchoolEx), MAX_SCHOOL_EX_FOR_INIT_CONDITION, \
      (char*) &(school_ex_array[0].varName) - (char*) &(school_ex_array[0]) )

#define AVERAGE_SCHOOL_EX_VAR_FLOAT_SKIP_ZERO(varName) \
      player_peer_school->varName = math.average_float( (char*) school_ex_array, sizeof(SchoolEx), MAX_SCHOOL_EX_FOR_INIT_CONDITION, \
      (char*) &(school_ex_array[0].varName) - (char*) &(school_ex_array[0]), true )

#define AVERAGE_SCHOOL_EX_VAR_INT_SKIP_ZERO(varName) \
      player_peer_school->varName = math.average_int( (char*) school_ex_array, sizeof(SchoolEx), MAX_SCHOOL_EX_FOR_INIT_CONDITION, \
      (char*) &(school_ex_array[0].varName) - (char*) &(school_ex_array[0]), true )

#define AVERAGE_SCHOOL_EX_VAR_SHORT_SKIP_ZERO(varName) \
      player_peer_school->varName = math.average_short( (char*) school_ex_array, sizeof(SchoolEx), MAX_SCHOOL_EX_FOR_INIT_CONDITION, \
      (char*) &(school_ex_array[0].varName) - (char*) &(school_ex_array[0]), true )

#define AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(varName) \
      player_peer_school->varName = math.average_char( (char*) school_ex_array, sizeof(SchoolEx), MAX_SCHOOL_EX_FOR_INIT_CONDITION, \
      (char*) &(school_ex_array[0].varName) - (char*) &(school_ex_array[0]), true )

    int i;

    //--------------------------------------//
    //-------- average vars in School ------//
    //--------------------------------------//
    // School
    AVERAGE_SCHOOL_EX_VAR_SHORT(fice);
    AVERAGE_SCHOOL_EX_VAR_CHAR(control);
    AVERAGE_SCHOOL_EX_VAR_CHAR(carnegie);
    AVERAGE_SCHOOL_EX_VAR_CHAR(cc_sort);
    AVERAGE_SCHOOL_EX_VAR_CHAR(market_score);

    //--------------------------------------//

    AVERAGE_SCHOOL_EX_VAR_SHORT(usn_wr_ranking);    // Raw USN&WR ranking
    AVERAGE_SCHOOL_EX_VAR_SHORT(ave_midpt_sat_act);
    AVERAGE_SCHOOL_EX_VAR_CHAR( freshmen_top_10_class);

    AVERAGE_SCHOOL_EX_VAR_CHAR(campus_environment); // = URBAN, SUBURBAN, or RURAL

    AVERAGE_SCHOOL_EX_VAR_CHAR(football_rating);
    AVERAGE_SCHOOL_EX_VAR_CHAR(basketball_rating);

    AVERAGE_SCHOOL_EX_VAR_SHORT(in_state_tuition);
    AVERAGE_SCHOOL_EX_VAR_SHORT(out_state_tuition);
    AVERAGE_SCHOOL_EX_VAR_SHORT(room_and_board_charges);
    AVERAGE_SCHOOL_EX_VAR_CHAR(ug_in_housing_percent);

    for (i=0; i<FACULTY_RANK_LEVEL_COUNT; i++) {
	AVERAGE_SCHOOL_EX_VAR_SHORT(school_faculty_array[i].faculty_count);
	AVERAGE_SCHOOL_EX_VAR_INT(school_faculty_array[i].salary);
	AVERAGE_SCHOOL_EX_VAR_FLOAT(school_faculty_array[i].female_percent);
	AVERAGE_SCHOOL_EX_VAR_FLOAT(school_faculty_array[i].minority_percent);
    }
    AVERAGE_SCHOOL_EX_VAR_FLOAT(parttime_faculty_percent);

    //----------------------------------------//
    // fred 0501

    AVERAGE_SCHOOL_EX_VAR_INT(student_applications);
    AVERAGE_SCHOOL_EX_VAR_SHORT(student_accepted);
    AVERAGE_SCHOOL_EX_VAR_SHORT(student_enrolled);
    AVERAGE_SCHOOL_EX_VAR_CHAR(student_get_bacc_in_5_year_percent);

    // field 43
    AVERAGE_SCHOOL_EX_VAR_SHORT(freshmen_applied_for_aid);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(percent_freshmen_applied_or_aid);

    AVERAGE_SCHOOL_EX_VAR_SHORT(freshmen_with_need);
    AVERAGE_SCHOOL_EX_VAR_FLOAT_SKIP_ZERO(percent_freshmen_with_need);

    AVERAGE_SCHOOL_EX_VAR_SHORT_SKIP_ZERO(freshmen_offered_aid);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(percent_freshmen_offered_aid);

    AVERAGE_SCHOOL_EX_VAR_SHORT(freshmen_offered_full_aid);
    AVERAGE_SCHOOL_EX_VAR_FLOAT_SKIP_ZERO(percent_freshmen_offered_full_aid);

    AVERAGE_SCHOOL_EX_VAR_CHAR( percent_freshmen_with_fin_aid);
    AVERAGE_SCHOOL_EX_VAR_CHAR( percent_continuing_student_with_fin_aid);

    AVERAGE_SCHOOL_EX_VAR_SHORT_SKIP_ZERO(institutional_aid_per_fte);

    //----------- revenue data -------------//
    // field 54
    AVERAGE_SCHOOL_EX_VAR_INT(total_gross_tuition_revenue);
    AVERAGE_SCHOOL_EX_VAR_INT(total_financial_aid);
    AVERAGE_SCHOOL_EX_VAR_INT(total_sponsored_research);
    AVERAGE_SCHOOL_EX_VAR_INT(adjusted_total_sponsored_research);
    AVERAGE_SCHOOL_EX_VAR_INT(raw_endowment_spending);
    AVERAGE_SCHOOL_EX_VAR_INT(state_and_local_appropriations);
    AVERAGE_SCHOOL_EX_VAR_INT(total_gifts);
    AVERAGE_SCHOOL_EX_VAR_INT(gifts_to_operations);
    AVERAGE_SCHOOL_EX_VAR_SHORT(athletics_revenue);
    AVERAGE_SCHOOL_EX_VAR_INT(adjusted_other_operating_income);
    AVERAGE_SCHOOL_EX_VAR_INT(raw_other_operating_income);

    //----------- field 65: expense data -------------//

    AVERAGE_SCHOOL_EX_VAR_INT(academic_dept_total_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(academic_dept_faculty_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(academic_dept_other_expense);

    AVERAGE_SCHOOL_EX_VAR_INT(sponsored_research_total_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(sponsored_research_faculty_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(sponsored_research_other_expense);

    AVERAGE_SCHOOL_EX_VAR_INT(library_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(library_other_expense);

    AVERAGE_SCHOOL_EX_VAR_INT(student_life_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(student_life_other_expense);

    AVERAGE_SCHOOL_EX_VAR_INT(inst_support_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(inst_support_other_expense);

    AVERAGE_SCHOOL_EX_VAR_INT(inst_support_net_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(inst_support_net_other_expense);

    AVERAGE_SCHOOL_EX_VAR_INT(inst_advancement_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(inst_advancement_other_expense);

    AVERAGE_SCHOOL_EX_VAR_INT(o_and_m_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(o_and_m_other_expense);

    AVERAGE_SCHOOL_EX_VAR_INT(athletics_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(athletics_other_expense);

    AVERAGE_SCHOOL_EX_VAR_INT(academic_support_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(academic_support_other_expense);

    // field 87	// fred 0501
    AVERAGE_SCHOOL_EX_VAR_INT(end_of_year_endowment_market_value);
    AVERAGE_SCHOOL_EX_VAR_INT(current_funds_balance);
    AVERAGE_SCHOOL_EX_VAR_INT(year_end_book_value_of_plant);
    AVERAGE_SCHOOL_EX_VAR_INT(year_end_market_value_of_plant);
    AVERAGE_SCHOOL_EX_VAR_INT(year_end_capital_reserve);
    AVERAGE_SCHOOL_EX_VAR_INT(year_end_general_plant_and_residence_hall_debt);
    AVERAGE_SCHOOL_EX_VAR_INT(residence_hall_debt);

    AVERAGE_SCHOOL_EX_VAR_INT(raw_surplus);
    AVERAGE_SCHOOL_EX_VAR_INT(adjusted_surplus);
    AVERAGE_SCHOOL_EX_VAR_INT(transfer_to_plant);
    AVERAGE_SCHOOL_EX_VAR_INT(amount_added_to_other_operating_income);

    // field 98
    AVERAGE_SCHOOL_EX_VAR_SHORT(full_time_undergrad);
    AVERAGE_SCHOOL_EX_VAR_SHORT(part_time_undergrad);
    AVERAGE_SCHOOL_EX_VAR_SHORT(master_and_professional);
    AVERAGE_SCHOOL_EX_VAR_SHORT(non_degree_seeking);
    AVERAGE_SCHOOL_EX_VAR_CHAR(female_undergrad_percent);
    AVERAGE_SCHOOL_EX_VAR_CHAR(female_grad_and_prof_percent);
    AVERAGE_SCHOOL_EX_VAR_CHAR(minority_undergrad_percent);
    AVERAGE_SCHOOL_EX_VAR_CHAR(minority_grad_and_prof_percent);
    AVERAGE_SCHOOL_EX_VAR_CHAR(non_resident_alien_grad_and_prof_percent);

    // field 107
    AVERAGE_SCHOOL_EX_VAR_SHORT(full_time_freshmen);
    AVERAGE_SCHOOL_EX_VAR_CHAR(freshmen_from_within_the_state_percent);
    AVERAGE_SCHOOL_EX_VAR_SHORT(total_bach_degrees);
    AVERAGE_SCHOOL_EX_VAR_SHORT(total_masters_and_prof_degree);
    AVERAGE_SCHOOL_EX_VAR_SHORT(total_doctoral_degrees);
    AVERAGE_SCHOOL_EX_VAR_SHORT(total_certificates);

    // field 113
    AVERAGE_SCHOOL_EX_VAR_INT(enrollment_fte);
    AVERAGE_SCHOOL_EX_VAR_CHAR(ug_percent);
    AVERAGE_SCHOOL_EX_VAR_CHAR(part_time_ug_percent);
    AVERAGE_SCHOOL_EX_VAR_CHAR(non_degree_seeking_percent);

    AVERAGE_SCHOOL_EX_VAR_FLOAT(applications_ratio);
    AVERAGE_SCHOOL_EX_VAR_CHAR(yield_rate);
    AVERAGE_SCHOOL_EX_VAR_CHAR(athletics_rating);

    AVERAGE_SCHOOL_EX_VAR_INT(total_e_and_g_expenditure);
    AVERAGE_SCHOOL_EX_VAR_CHAR(net_tutition_reenue_as_percent_of_e_and_g_expese);

    AVERAGE_SCHOOL_EX_VAR_SHORT(total_tenureline_faculty);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(sponsored_research_per_reg_faculty);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(doctoral_degrees_per_reg_faculty);

    AVERAGE_SCHOOL_EX_VAR_CHAR(overhead_rate);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(endowment_spending_rate);

    AVERAGE_SCHOOL_EX_VAR_SHORT(enrollment_masters);
    AVERAGE_SCHOOL_EX_VAR_SHORT(enrollment_doctoral);

    // ---------- field 1 of umaster2 -------------

    for(i=0;i<5;i++) {
	AVERAGE_SCHOOL_EX_VAR_SHORT(target_student_intake_sl[i]);
	AVERAGE_SCHOOL_EX_VAR_FLOAT_SKIP_ZERO(dropout_rate_sl[i]);
	AVERAGE_SCHOOL_EX_VAR_FLOAT_SKIP_ZERO(target_grad_rate_sl[i]);
    }

    // field 16
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_ug_students_on_aid);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(endowment_per_student);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(endowment_per_tenureline_faculty_member);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(ratio_of_current_funds_balance_to_total_operating_expenditure);
    AVERAGE_SCHOOL_EX_VAR_CHAR(financial_condition);
    AVERAGE_SCHOOL_EX_VAR_SHORT(number_of_incoming_ug_students);
    AVERAGE_SCHOOL_EX_VAR_CHAR(female_student_percent);
    AVERAGE_SCHOOL_EX_VAR_CHAR(minority_student_percent);
    AVERAGE_SCHOOL_EX_VAR_CHAR(sponsored_research_rating);
    AVERAGE_SCHOOL_EX_VAR_CHAR(doctoral_degres_per_faculty_rating);
    AVERAGE_SCHOOL_EX_VAR_CHAR(adj_freshmen_top_ten_percent_of_class);

    AVERAGE_SCHOOL_EX_VAR_FLOAT(apps_ratio2);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(yield_rate2);
    AVERAGE_SCHOOL_EX_VAR_CHAR(athletics_rating2);
    AVERAGE_SCHOOL_EX_VAR_CHAR(net_tuition_revenue_as_percent_of_e_and_g_expense);
    // = 4.21 for debug
    AVERAGE_SCHOOL_EX_VAR_FLOAT_SKIP_ZERO(doc_time_to_degree);

    //---- vars for screening school records -----//
    AVERAGE_SCHOOL_EX_VAR_FLOAT(prestige);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(distance_value);

    //--------------------------------------//
    //------ average vars in SchoolEx ------//
    //--------------------------------------//
    // field L-R
    AVERAGE_SCHOOL_EX_VAR_INT(operating_reserve);   // current_funds_balance
    AVERAGE_SCHOOL_EX_VAR_INT(endowment_market);    // end_of_year_endowment_market_value
    AVERAGE_SCHOOL_EX_VAR_INT(buildings);           // year_end_market_value_of_plant
    AVERAGE_SCHOOL_EX_VAR_INT(capital_reserve);     // year_end_capital_reserve
    AVERAGE_SCHOOL_EX_VAR_INT(general_plant_debt);  // year_end_general_plant_and_residence_hall_debt
    AVERAGE_SCHOOL_EX_VAR_INT(residence_hall_debt); // residence_hall_debt
    AVERAGE_SCHOOL_EX_VAR_INT(fund_balance);        // R=M1244+N1244+O1244-P1244-Q1244

    // field S
    // total_gross_tuition_revenue
    AVERAGE_SCHOOL_EX_VAR_INT(gross_tuition_revenue);
    AVERAGE_SCHOOL_EX_VAR_INT(financial_aid);       // total_financial_aid
    AVERAGE_SCHOOL_EX_VAR_INT(net_tuition_revenue); // =S-T

    AVERAGE_SCHOOL_EX_VAR_INT(state_appropriations);// state_and_local_appropriations
    AVERAGE_SCHOOL_EX_VAR_INT(sponsored_research);  // adjusted_total_sponsored_research
    AVERAGE_SCHOOL_EX_VAR_INT(gifts_to_operations); // gifts_to_operations
    AVERAGE_SCHOOL_EX_VAR_INT(endowment_spending);  //BUG in excel data!!! = M*B = endowment_market * fice

    // field Z
    AVERAGE_SCHOOL_EX_VAR_SHORT(athletics);         // athletics_revenue
    // adjusted_other_operating_income);
    AVERAGE_SCHOOL_EX_VAR_INT(other_operating_income);
    // = current_funds_balance * 0.08
    AVERAGE_SCHOOL_EX_VAR_INT(interest_on_operating_reserve);
    // =AC=SUM(U1244:AB1244)
    AVERAGE_SCHOOL_EX_VAR_INT(total_sources_of_funds);

    // field AD
    // academic_dept_faculty_salaries
    AVERAGE_SCHOOL_EX_VAR_INT(dept_expense_faculty_salaries);
    // = academic_dept_total_salaries - AD
    AVERAGE_SCHOOL_EX_VAR_INT(dept_expense_staff_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(dept_expense_other);  // academic_dept_other_expense
    // sponsored_research_faculty_salaries
    AVERAGE_SCHOOL_EX_VAR_INT(sponsored_research_faculty_salaries);
    // = sponsored_research_total_salaries - AG
    AVERAGE_SCHOOL_EX_VAR_INT(sponsored_research_staff_salaries);
    // sponsored_research_other
    AVERAGE_SCHOOL_EX_VAR_INT(sponsored_research_other);

    // field AJ
    // library_salaries
    AVERAGE_SCHOOL_EX_VAR_INT(library_staff_salaries);
    // library_other_expense
    AVERAGE_SCHOOL_EX_VAR_INT(library_other_expense);

    // = academic_support_salaries * 0.1
    AVERAGE_SCHOOL_EX_VAR_INT(academic_it_staff_salaries);
    // = academic_support_other_expense * 0.3
    AVERAGE_SCHOOL_EX_VAR_INT(academic_it_other_expense);

    // field AN
    // student_life_salaries
    AVERAGE_SCHOOL_EX_VAR_INT(student_life_staff_salaries);
    // student_life_other_expense
    AVERAGE_SCHOOL_EX_VAR_INT(student_life_other_expense);

    // fields already defined in base class School
    //AVERAGE_SCHOOL_EX_VAR_INT(athletics_salaries);			// academic_support_salaries
    //AVERAGE_SCHOOL_EX_VAR_INT(athletics_other_expense);				// academic_support_other_expense

    // field AR
    // inst_advancement_salaries
    AVERAGE_SCHOOL_EX_VAR_INT(inst_advancement_staff_salaries);
    // inst_advancement_other_expense
    AVERAGE_SCHOOL_EX_VAR_INT(inst_advancement_other_expense);

    // inst_support_net_salaries);
    AVERAGE_SCHOOL_EX_VAR_INT(administration_staff_salaries);
    // inst_support_net_other_expense);
    AVERAGE_SCHOOL_EX_VAR_INT(administration_other_expense);

    // o_and_m_salaries
    AVERAGE_SCHOOL_EX_VAR_INT(o_and_m_staff_salaries);
    // o_and_m_other_expense
    AVERAGE_SCHOOL_EX_VAR_INT(o_and_m_other_expense);

    // field AX
    // academic_support_salaries -AL
    AVERAGE_SCHOOL_EX_VAR_INT(other_operating_expense_staff_salaries);
    // academic_support_other_expense -AM
    AVERAGE_SCHOOL_EX_VAR_INT(other_operating_expense_other_expense);

    // = SUM(AD1244:AY1244)
    AVERAGE_SCHOOL_EX_VAR_INT(total_operating_expenditure);

    // field BA
    // (year_end_general_plant_and_residence_hall_debt
    AVERAGE_SCHOOL_EX_VAR_INT(service_on_general_plant_debt);
    // -residence_hall_debt)*0.065
    // transfer_to_plant
    AVERAGE_SCHOOL_EX_VAR_INT(transfer_to_capital_reserve);
    AVERAGE_SCHOOL_EX_VAR_INT(total_uses_of_funds); // = AZ1244+BA1244+BB1244
    AVERAGE_SCHOOL_EX_VAR_INT(surplus_or_deficit);  // = AC1244-BC1244

    // field BE
    // = freshmen_from_within_the_state_percent/100.0*in_state_tuition+(1-freshmen_from_within_the_state_percent/100.0)*out_state_tuition
    AVERAGE_SCHOOL_EX_VAR_INT_SKIP_ZERO(tuition_rate);
    // room_and_board_charges
    AVERAGE_SCHOOL_EX_VAR_SHORT_SKIP_ZERO(room_and_board_rate);
    // ug_in_housing_percent/1000.0
    AVERAGE_SCHOOL_EX_VAR_FLOAT(percent_traditional_ug_in_residence_halls);
    // student_get_bacc_in_5_year_percent/1000.0
    AVERAGE_SCHOOL_EX_VAR_FLOAT(percent_get_bacc_in_5_year);
    // overhead_rate
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(overhead_rate_on_sponsored_research);
    // float	endowment_spending_rate);
    AVERAGE_SCHOOL_EX_VAR_FLOAT_SKIP_ZERO(endowment_spending_rate);

    // field BK
    for (i=0; i<FACULTY_RANK_LEVEL_COUNT; i++) {
	AVERAGE_SCHOOL_EX_VAR_SHORT(school_faculty_array_ex[i].faculty_count);
	AVERAGE_SCHOOL_EX_VAR_INT_SKIP_ZERO(school_faculty_array_ex[i].salary);
	AVERAGE_SCHOOL_EX_VAR_FLOAT_SKIP_ZERO(school_faculty_array_ex[i].female_percent);
	AVERAGE_SCHOOL_EX_VAR_FLOAT_SKIP_ZERO(school_faculty_array_ex[i].minority_percent);
	AVERAGE_SCHOOL_EX_VAR_INT(school_faculty_array_ex[i].active_research_dollars);
    }

    // Enroll ft,pt_ug,etc (Enroll SL1-5);// Enroll ft,pt_ug,etc (Enroll SL1-5);  will be "scaled" later in this function
    // = sPtr->full_time_undergrad);
    AVERAGE_SCHOOL_EX_VAR_SHORT(full_time_undergrad);
    // = sPtr->part_time_undergrad);
    AVERAGE_SCHOOL_EX_VAR_SHORT(part_time_undergrad);
    AVERAGE_SCHOOL_EX_VAR_SHORT(enrollment_masters);// = sPtr->enrollment_masters);
    // = sPtr->enrollment_doctoral);
    AVERAGE_SCHOOL_EX_VAR_SHORT(enrollment_doctoral);
    AVERAGE_SCHOOL_EX_VAR_SHORT(non_degree_seeking);// = sPtr->non_degree_seeking

    // field CM - Percent Female SL1-5
    // = female_undergrad_percent
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_female_sl[0]);
    // = female_undergrad_percent*1.1
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_female_sl[1]);
    // = female_grad_and_prof_percent*1.1
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_female_sl[2]);
    // = female_grad_and_prof_percent*0.75
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_female_sl[3]);
    // = female_undergrad_percent*0.85
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_female_sl[4]);

    // field CR - Percent Minority SL1-5
    // = minority_undergrad_percent*0.85);
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_minority_sl[0]);
    // = minority_undergrad_percent*1.2);
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_minority_sl[1]);
    // = minority_grad_and_prof_percent);
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_minority_sl[2]);
    // = minority_grad_and_prof_percent*0.6);
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_minority_sl[3]);
    // = minority_undergrad_percent*1.1);
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_minority_sl[4]);

    // = freshmen_from_within_the_state_percent
    AVERAGE_SCHOOL_EX_VAR_CHAR(percent_instate_freshmen);
    // = non_resident_alien_grad_and_prof_percent);
    AVERAGE_SCHOOL_EX_VAR_CHAR_SKIP_ZERO(percent_non_resident_alien_doctoral);

    //##### begin fred0514 #####
    // fields already defined in base class School
    // field CY
    //for(i=0;i<5;i++)
    //{
    //	AVERAGE_SCHOOL_EX_VAR_SHORT(target_student_intake_sl[i]);
    //	AVERAGE_SCHOOL_EX_VAR_FLOAT(dropout_rate_sl[i]);
    //	AVERAGE_SCHOOL_EX_VAR_FLOAT(target_grad_rate_sl[i]);
    //}
    //##### end fred0514 #####

    // field DN
    AVERAGE_SCHOOL_EX_VAR_INT(ug_applications);     // = student_applications);
    // = ug_applications/student_accepted);
    AVERAGE_SCHOOL_EX_VAR_FLOAT(ug_applications_rate);
    AVERAGE_SCHOOL_EX_VAR_CHAR(ug_yield_rate);      // = float(student_enrolled)/student_accepted*100);

    // fields already defined in base class School
    //AVERAGE_SCHOOL_EX_VAR_CHAR(percent_ug_students_on_aid);	// = percent_ug_students_on_aid);
    //AVERAGE_SCHOOL_EX_VAR_FLOAT(percent_freshmen_with_need);	// = percent_freshmen_with_need);
    //AVERAGE_SCHOOL_EX_VAR_FLOAT(freshmen_offered_aid);			// = freshmen_offered_aid);
    //AVERAGE_SCHOOL_EX_VAR_FLOAT(percent_freshmen_offered_full_aid);	// = percent_freshmen_offered_full_aid);
    //AVERAGE_SCHOOL_EX_VAR_SHORT(institutional_aid_per_fte);

    // err_when(AGRICULTURE);

    for (short sl=0; sl<MAX_STUDENT_LEVEL_INI; sl++)
	for (short fd=0; fd < IPED_FIELD_COUNT; fd++) {
	    // 1020 in latest initial_conds dated 980711, it is AVERAGE!
	    AVERAGE_SCHOOL_EX_VAR_FLOAT(student_ifield_pct[sl][fd]);
	}

#ifdef DEBUG_
    const short specialOrder[IPED_FIELD_COUNT] = {
	AGRICULTURE,INTER_DISCIPLINARY,BUSINESS,COMMUNICAIONS_LIBRARY_SCI,
	MATH_COMP_SCI,EDUCATION,ENGINEERING,FOREIGN_LANGUAGES,HEALTH_SOCIAL_SVCS,HUMANITIES,
	LIFE_PHYSICAL_SCI,SOCIAL_SCIENCES,VISUAL_PERFORMING_ARTS,BASIC_PERSONAL_PRACTICAL_SVCS
    };
    for (short fd=0; fd<IPED_FIELD_COUNT; fd++) {
	char s[300];
	sprintf(s, "schoolex: average: %f ", player_peer_school->student_ifield_pct[0][specialOrder[fd]]);
	debug_msg(s);
    }
#endif

    // 0.0 4.7%	11.7%	9.6%	4.2%	2.9%	11.4%	2.4%	5.0%	10.4%	11.5%	33.4%	5.3%	4.0%
}

//--------- End of function SchoolRes::init_player_school_ex ---------//

//------ Begin of function sort_school_distance_function ------//
//!
//! Sort School in db_school_array by their distances in an ascending order.
//!
static int sort_school_distance_function( const void *a, const void *b ) {
    float distanceA = school_res.desired_school_array[*((short*)a)].distance_value;
    float distanceB = school_res.desired_school_array[*((short*)b)].distance_value;

    float rc = distanceA-distanceB;

    if( rc > 0 )
	return 1;
    else if( rc < 0 )
	return -1;
    else
	return 0;
}

//------- End of function sort_school_distance_function ------//

//------ Begin of function sort_peerschool_prestige_function ------//
//!
//! Sort School in db_school_array by their distances in an ascending order.
//!
static int sort_peerschool_prestige_function( const void *a, const void *b ) {
    float prestigeA = school_res.desired_school_array[*((short*)a)].prestige;
    float prestigeB = school_res.desired_school_array[*((short*)b)].prestige;

    float rc = prestigeA - prestigeB;

    if( rc > 0 )
	return 1;
    else if( rc < 0 )
	return -1;
    else
	return 0;
}

//------- End of function sort_peerschool_prestige_function ------//

//------- Begin of function SchoolRes::get_db_school -------//
//!
#ifdef DEBUG
School* SchoolRes::get_db_school(int recno) {
    err_when( recno < 1 || recno > db_school_count );

    return db_school_array+recno-1;
}
#endif
//--------- End of function SchoolRes::get_db_school ---------//

//-------- Begin of function SchoolRes::build_peer_school_array --------//
//!
//! select and build peer schools in peer_school_array from db_school_array
//!
void SchoolRes::build_peer_school_array() {
    int i, recno;
    float distanceDeviation = 0.0f;
    float distanceAverage = 0.0f;

    School *sPtr = desired_school_array;

    //
    //--- calculate standard deviation of distance in db_school_array ---//
    //
    for( recno=1; recno<=desired_school_count; recno++ ) {
	distanceAverage += desired_school_array[recno-1].distance_value;
    }
    distanceAverage /= desired_school_count;

    for( recno=1; recno<=desired_school_count; recno++ )
	distanceDeviation += EXPO2(desired_school_array[recno-1].distance_value - distanceAverage);
    distanceDeviation /= desired_school_count;
    distanceDeviation = sqrtf(distanceDeviation);

    //
    //--- select school in db_school_array ---//
    //
    DynArray    peerRecnoArr(sizeof(int), 20);

    // C = 0.1 is proposed by Bill in email on 0531
    const float C = 0.1f;                           // number of deviation in screening range
    const float screenRange = player_peer_school->distance_value + C * distanceDeviation;
    int count = 0;

    sPtr = desired_school_array;
    for( recno=0; recno<desired_school_count ; recno++, sPtr++ ) {
	if ( sPtr->distance_value < screenRange ) {
	    peerRecnoArr.linkin((void *) &recno);

	    if( ++count == MAX_PEER_SCHOOL_COUNT )
		break;
	}
    }

    peer_school_count = peerRecnoArr.size();

    //
    //---sort the peer_school_array---//
    //

    //---- create an array for sorting on prestige ----//

    short* schoolSortArray = (short*) mem_add( peer_school_count * sizeof(short) );

    for( i=1; i<=peer_school_count; i++ ) {
	schoolSortArray[i-1] = *((int *) peerRecnoArr.get(i));

#ifdef DEBUG_PEER
	char s[100];
	recno = *((int *) peerRecnoArr.get(i));
	sprintf(s, "b4 sort: recno/prestige/distance #%d = %d   %f   %f", i, recno, desired_school_array[recno-1].prestige, desired_school_array[recno-1].distance_value);
	DEBUG_LOG(s);
#endif
    }

    //---- sort School in array[] by prestige ----//
    qsort( schoolSortArray, peer_school_count, sizeof(schoolSortArray[0]), sort_peerschool_prestige_function );

    //
    //---create PeerSchool records in peer_school_array---//
    //
    peer_school_array = (PeerSchool*) mem_add( sizeof(PeerSchool) * (peer_school_count+1) );
    PeerSchool *psPtr = peer_school_array;
    bool isPlayerAdded = false;

    // loop through schoolSortArray
    for ( i=0; i<peer_school_count; i++, psPtr++) {
	recno = schoolSortArray[i];

	if ( !isPlayerAdded && desired_school_array[recno].prestige > player_peer_school->prestige )
	    {
		// insert player_school to the peer_school_array
		// then deallocate memory pointed by player_peer_school

		// if player_peer_school->prestige is the largest, this segment is not run, see segment after for loop
		psPtr->init(-1);

		memcpy(psPtr, player_peer_school, sizeof(PeerSchool));
		mem_del(player_peer_school);
		player_peer_school = psPtr;

		psPtr++;
		isPlayerAdded = true;
		//psPtr->init(recno);		// 1008
		//continue;
	    }

	//psPtr->init(db_school_array+recno-1);
	psPtr->init(recno);

	//--------- assign a new name from made-up names database PEERNAME.DBF  ---//

	int loopCount=0;

	while(1) {
	    err_when( loopCount++ > 10000 );

	    int peerId = m.random(peer_info_count);

	    //--- filter out duplication -----//

	    char* newName = peer_info_array[peerId].name;
		int j;
	    for ( j=0; j<peer_school_count; j++ ) {
		if( strcmp( peer_school_array[j].name, newName )==0 )
		    break;
	    }

	    if( j<peer_school_count )                   // duplicated.
		continue;

	    //-------- set the peer name now -------------//

	    strcpy(psPtr->name, newName);
	    break;
	}

#ifdef DEBUG_PEER
	char s[100];
	sprintf(s, "af sort: recno/prestige/distance %d = %d   %f   %f", i+1, recno, psPtr->prestige, psPtr->distance_value);
	DEBUG_LOG(s);
#endif
    }

    // ##### begin Gilbert 16/01/2002 #####//
    // fix in version 2 but also apply for version 1
    // if player_peer_school has the largest prestige, it was not added to peer_school_array
    if( !isPlayerAdded ) {
	psPtr->init(-1);

	memcpy(psPtr, player_peer_school, sizeof(PeerSchool));
	mem_del(player_peer_school);
	player_peer_school = psPtr;

	psPtr++;
	isPlayerAdded = true;
    }
    // ##### end Gilbert 16/01/2002 #####//

    peer_school_count++;                            // include the player school
    mem_del( schoolSortArray );                     // sorting finished

    //----------------------------//
    // 1029 // see WFM Notes, 19 Sept, section5.4

    memset(PeerSchool::average_faculty_salary, 0, sizeof(PeerSchool::average_faculty_salary));

    psPtr = peer_school_array;

    for ( i=0; i<peer_school_count; i++, psPtr++) {
	for (int r=0; r<FACULTY_RANK_LEVEL_COUNT; r++) {
	    // 51: full professor: middle age group
	    PeerSchool::average_faculty_salary[Faculty::rank_age_group(r, 51)] += psPtr->school_faculty_array_ex[r].salary;
	}
    }

    for (int r=0; r<MAX_RANK_AGE_GROUP; r++)
	PeerSchool::average_faculty_salary[r] /= peer_school_count;

    //------------------//

    int lowFullProf = Faculty::rank_age_group(FULL_PROF, 41);
    int midFullProf = Faculty::rank_age_group(FULL_PROF, 51);
    int highFullProf = Faculty::rank_age_group(FULL_PROF, 61);

    char templateCode[1] = { FIRST_TEMPLATE_CODE };
    FacultyTemplate* facultyTemplate = faculty_res.get_faculty_template( templateCode, lowFullProf );
    FacultyTemplate* facultyTemplate2 = faculty_res.get_faculty_template( templateCode, midFullProf );
    FacultyTemplate* facultyTemplate3 = faculty_res.get_faculty_template( templateCode, highFullProf );

    float lowRatio = float(facultyTemplate->overall_salary_multiplier) / facultyTemplate2->overall_salary_multiplier;
    float highRatio = float(facultyTemplate3->overall_salary_multiplier) / facultyTemplate2->overall_salary_multiplier;

    PeerSchool::average_faculty_salary[lowFullProf] = int(lowRatio * PeerSchool::average_faculty_salary[midFullProf]);
    PeerSchool::average_faculty_salary[highFullProf] = int(highRatio * PeerSchool::average_faculty_salary[midFullProf]);

    //-----------------//
    // 0130 scenario(2)

    if ( player_school.scenario_id == SCN_RAISE_SALARY ) {
	for (int r=0; r<MAX_RANK_AGE_GROUP; r++)
	    PeerSchool::average_faculty_salary[r] = int(PeerSchool::average_faculty_salary[r] * 1.1);
    }
}

//-------- End of function SchoolRes::build_peer_school_array --------//

//-------- Begin of function SchoolRes::next_day --------//
//!
//!
//!
void  SchoolRes::next_day() {
    // per year
    if ( info.game_day == 1  && info.game_month == finance.fiscal_year_start_month )
	PeerSchool::calc_average_faculty_salary();
}

//-------- End of function SchoolRes::next_day --------//

// --- Begin of function SchoolRes::get_desired_school_array --- //
//
void SchoolRes::get_desired_school_array() {
    short numCount=0;

    // get the number count of school are in desired type
    for ( int i=0; i<db_school_count; i++ ) {
	// they are in desired type
	if ( get_db_school(i+1)->control == player_school.control ) {
	    numCount++;
	}
    }

    desired_school_count = numCount;
    desired_school_array = (School*) mem_add( sizeof(School) * desired_school_count );

    numCount = 0;
    // copy the desired school into desired_school_array
    for ( int i=0; i<db_school_count; i++ ) {
	// if they are in desired type
	if ( get_db_school(i+1)->control == player_school.control ) {
	    memcpy( &desired_school_array[numCount], get_db_school(i+1), sizeof(School) );
	    desired_school_array[numCount].desired_recno = i+1;
	    numCount++;
	}
    }
}

//
// --- End of function SchoolRes::get_desired_school_array --- //

// --- Begin of function SchoolRes::get_50_school_array --- //
//
void SchoolRes::get_50_school_array() {
    // save the  of desired_school_array into schoolSortArray for sorting function
    short*  schoolSortArray = (short*) mem_add( desired_school_count * sizeof(short) );

    for ( int i=0; i<desired_school_count; i++ )
	schoolSortArray[i] = i;

  // quick sort the desired array
    qsort( schoolSortArray, desired_school_count, sizeof(schoolSortArray[0]), sort_school_distance_function );

    // copy the first 50 schools into desired_school_array
    short*  tempSchoolRecnoArray = (short*) mem_add( MAX_FIRST_SCHOOL * sizeof(short) );
    for ( int i=0; i<MAX_FIRST_SCHOOL; i++ )
	tempSchoolRecnoArray[i] = desired_school_array[schoolSortArray[i]].desired_recno;
    for ( int i=0; i<MAX_FIRST_SCHOOL; i++ ) {
	memcpy( &desired_school_array[i], get_db_school(tempSchoolRecnoArray[i]), sizeof(School) );
	desired_school_array[i].desired_recno = tempSchoolRecnoArray[i];
    }

    // resize the desired_school_count and also the desired_school_array
    desired_school_count = MAX_FIRST_SCHOOL;

    desired_school_array = (School*) mem_resize( desired_school_array, sizeof(School) * desired_school_count );

    mem_del(schoolSortArray);
    mem_del(tempSchoolRecnoArray);
}

//
// --- End of function SchoolRes::get_50_school_array --- //

//-------- Begin of static function PeerSchool::calc_average_faculty_salary--------//
//
// see WFM Notes, 10 or 19 Sept, section5.4
//
int PeerSchool::average_faculty_salary[MAX_RANK_AGE_GROUP];

void  PeerSchool::calc_average_faculty_salary() {
    // req28a_time_variation.txt
    float       initValue = math.get_random_snd(.01f, .0025f);
    static float gen_rand[MAX_RANK_AGE_GROUP] = {initValue,initValue,initValue,initValue,initValue,initValue,initValue};
    static float gen_rand_last2[MAX_RANK_AGE_GROUP] = {0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f};

    initValue = math.get_random_snd(.01f, .005f);   // req28a_time_variation.txt
    static float spec_rand[MAX_RANK_AGE_GROUP] = {initValue,initValue,initValue,initValue,initValue,initValue,initValue};
    static float spec_rand_last2[MAX_RANK_AGE_GROUP] = {0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f};

    // initialize static var: int average_faculty_salary[MAX_RANK_AGE_GROUP];	// not $000
    float inflationRate = float(finance.inflation_rate) / 100.0f;

    for (int i=0; i<MAX_RANK_AGE_GROUP; i++) {
	float lastGen = gen_rand[i];
	float lastSpec = spec_rand[i];

	gen_rand[i] = math.time_variation(lastGen, gen_rand_last2[i], 1.5147f, -0.7647f, 0.001250360f);
	spec_rand[i] = math.time_variation(lastSpec, spec_rand_last2[i], 1.0643f, -0.7285f, 0.001660440f);

	gen_rand_last2[i] = lastGen;
	spec_rand_last2[i] = lastSpec;

	average_faculty_salary[i] = int( average_faculty_salary[i] * (1+inflationRate+gen_rand[i]+spec_rand[i]) );
    }
}

//-------- End of static function PeerSchool::calc_average_faculty_salary--------//
