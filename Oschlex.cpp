//Filename    : OSCHEX.CPP
//Description : SchoolEx class functions -- also see Initial_Conds of HE.initialization.xls
//Owner			: Fred

#include <ODB.H>

#include <Oschrec2.h>                             // SchoolDegreeRec
#include <Oschlex.h>
#include <OSCHLRES.H>
#include <OPSCHOOL.H>

#ifdef DEBUG
#include <stdio.h>
#endif

#define SCHOOL_DB3   DIR_RES"UMASTER3.RES"

//--------- Begin of function SchoolEx::init --------//
//!
//! init object member variables
//! sdRec is temporarily useless
//!
void SchoolEx::init(int schoolRecno) {
    Database  dbSchool(SCHOOL_DB3, 0);              // 1-read the entire DB into buffer
    short db3Count = (short) dbSchool.rec_count();
    err_when(db3Count != school_res.db_school_count);

    db_school_recno = school_res.desired_school_array[schoolRecno].desired_recno;

    School* sPtr = &school_res.desired_school_array[schoolRecno];

    SchoolDegreeRec* degRec = (SchoolDegreeRec*) dbSchool.read(db_school_recno);

    memset( this, 0, sizeof(SchoolEx));

    //--- copy all vars from a School object in db_school_array[] to player_school_ex ---//

    memcpy( this, sPtr, sizeof(School) );           // SchoolEx is a derived class of School

    //-- field L-R of sheet "init_cond" of Excel file "initialization" --//

    operating_reserve   = sPtr->current_funds_balance;
    endowment_market    = sPtr->end_of_year_endowment_market_value;
    buildings       = sPtr->year_end_market_value_of_plant;
    capital_reserve   = sPtr->year_end_capital_reserve;
    general_plant_debt  = sPtr->year_end_general_plant_and_residence_hall_debt - sPtr->residence_hall_debt;
    residence_hall_debt = sPtr->residence_hall_debt;
    fund_balance      = endowment_market+buildings+capital_reserve-general_plant_debt-residence_hall_debt;

    //----- field S -----//

    gross_tuition_revenue = sPtr->total_gross_tuition_revenue;
    financial_aid       = sPtr->total_financial_aid;
    net_tuition_revenue   = gross_tuition_revenue-financial_aid;

    state_appropriations    = sPtr->state_and_local_appropriations;

    sponsored_research    = sPtr->adjusted_total_sponsored_research;
    gifts_to_operations   = sPtr->gifts_to_operations;
    endowment_spending    = int(endowment_market * sPtr->endowment_spending_rate/100.0);

    //---- field Z ------//

    athletics           = sPtr->athletics_revenue;
    other_operating_income    = sPtr->adjusted_other_operating_income;
    interest_on_operating_reserve   = int(sPtr->current_funds_balance * 0.08);
    total_sources_of_funds        = net_tuition_revenue
	+ state_appropriations + sponsored_research + gifts_to_operations + endowment_spending
	+ athletics + other_operating_income + interest_on_operating_reserve;

    // field AD
    dept_expense_faculty_salaries   = sPtr->academic_dept_faculty_salaries;
    dept_expense_staff_salaries   = sPtr->academic_dept_total_salaries - dept_expense_faculty_salaries;
    dept_expense_other          = sPtr->academic_dept_other_expense;
    sponsored_research_faculty_salaries   = sPtr->sponsored_research_faculty_salaries;
    sponsored_research_staff_salaries   = sPtr->sponsored_research_total_salaries - sponsored_research_faculty_salaries;
    sponsored_research_other          = sPtr->sponsored_research_other_expense;

    // field AJ
    library_staff_salaries    = sPtr->library_salaries;
    library_other_expense   = sPtr->library_other_expense;

    academic_it_staff_salaries    = int(sPtr->academic_support_salaries * 0.1);
    academic_it_other_expense   = int(sPtr->academic_support_other_expense * 0.3);

    // field AN
    student_life_staff_salaries = sPtr->student_life_salaries;
    student_life_other_expense    = sPtr->student_life_other_expense;

    // fields already defined in base class School
    //athletics_salaries				= sPtr->athletics_salaries;
    //athletics_other_expense				= sPtr->athletics_other_expense;

    // field AR
    inst_advancement_staff_salaries   = sPtr->inst_advancement_salaries;
    inst_advancement_other_expense    = sPtr->inst_advancement_other_expense;

    administration_staff_salaries   = sPtr->inst_support_net_salaries;
    administration_other_expense    = sPtr->inst_support_net_other_expense;

    o_and_m_staff_salaries    = sPtr->o_and_m_salaries;
    o_and_m_other_expense   = sPtr->o_and_m_other_expense;

    // field AX
    other_operating_expense_staff_salaries    = sPtr->academic_support_salaries - academic_it_staff_salaries;
    other_operating_expense_other_expense   = sPtr->academic_support_other_expense - academic_it_other_expense;

    total_operating_expenditure =                   // = sPtr->SUM(AD1244:AY1244)
	dept_expense_faculty_salaries +
	dept_expense_staff_salaries +
	dept_expense_other +
	sponsored_research_faculty_salaries +
	sponsored_research_staff_salaries +
	sponsored_research_other +
	library_staff_salaries +
	library_other_expense +
	academic_it_staff_salaries +
	academic_it_other_expense +
	student_life_staff_salaries +
	student_life_other_expense +
	athletics_salaries +
	athletics_other_expense +
	inst_advancement_staff_salaries +
	inst_advancement_other_expense +
	administration_staff_salaries +
	administration_other_expense +
	o_and_m_staff_salaries +
	o_and_m_other_expense +
	other_operating_expense_staff_salaries +
	other_operating_expense_other_expense;

    // field BA
    service_on_general_plant_debt = int((sPtr->year_end_general_plant_and_residence_hall_debt
					 - sPtr->residence_hall_debt)*0.065);
    transfer_to_capital_reserve = sPtr->transfer_to_plant;
    total_uses_of_funds       = total_operating_expenditure + service_on_general_plant_debt+transfer_to_capital_reserve;
    surplus_or_deficit        = total_sources_of_funds-total_uses_of_funds;

    // field BE
    tuition_rate        = int(sPtr->freshmen_from_within_the_state_percent/100.0*sPtr->in_state_tuition+(1-sPtr->freshmen_from_within_the_state_percent/100.0)*sPtr->out_state_tuition);
    room_and_board_rate   = sPtr->room_and_board_charges;
    //BUGHERE0 what that? "/ 1000 * 100"???
    percent_traditional_ug_in_residence_halls   = float(sPtr->ug_in_housing_percent/1000.0 * 100.0);
    percent_get_bacc_in_5_year              = float(sPtr->student_get_bacc_in_5_year_percent/1000.0 * 100.0);
    overhead_rate_on_sponsored_research       = sPtr->overhead_rate;
    endowment_spending_rate               = sPtr->endowment_spending_rate;

    // ----- field BK to CG ----- //

    // field BP - Assoc Profs
    memcpy(&(school_faculty_array_ex[ASSOC_PROF]), &(sPtr->school_faculty_array[ASSOC_PROF]), sizeof(SchoolFaculty));
    school_faculty_array_ex[ASSOC_PROF].active_research_dollars
	//## chea 061299 1.8.2
	//		= int(0.8*sPtr->sponsored_research_per_reg_faculty);
	= int(sPtr->sponsored_research_per_reg_faculty);

    // field BU - Asst Professors
    memcpy(&(school_faculty_array_ex[ASST_PROF]), &(sPtr->school_faculty_array[ASST_PROF]), sizeof(SchoolFaculty));
    school_faculty_array_ex[ASST_PROF].active_research_dollars
	//## chea 061299 1.8.2
	//		= int(0.5*sPtr->sponsored_research_per_reg_faculty);
	= int(0.6*sPtr->sponsored_research_per_reg_faculty);

    // field BK - Full Professors
    memcpy(&(school_faculty_array_ex[FULL_PROF]), &(sPtr->school_faculty_array[FULL_PROF]), sizeof(SchoolFaculty));

    if ( school_faculty_array_ex[FULL_PROF].faculty_count )
	school_faculty_array_ex[FULL_PROF].active_research_dollars
	    = int((sPtr->adjusted_total_sponsored_research
		   - school_faculty_array_ex[ASSOC_PROF].faculty_count*school_faculty_array_ex[ASSOC_PROF].active_research_dollars
		   - school_faculty_array_ex[ASST_PROF].faculty_count*school_faculty_array_ex[ASST_PROF].active_research_dollars
		) / school_faculty_array_ex[FULL_PROF].faculty_count);
    else
	school_faculty_array_ex[FULL_PROF].active_research_dollars = 0;
    // (??-BP1244*BR1244-BU1244*BW1244)/BK1244;

    // field BZ - long-term adjuncts
    memcpy(&(school_faculty_array_ex[LONG_TERM_ADJUNCT]), &(sPtr->school_faculty_array[LONG_TERM_ADJUNCT]), sizeof(SchoolFaculty));
    school_faculty_array_ex[LONG_TERM_ADJUNCT].salary
	= int(0.5*school_faculty_array_ex[FULL_PROF].salary
	      + 0.35*school_faculty_array_ex[ASSOC_PROF].salary
	      + 0.15*school_faculty_array_ex[ASST_PROF].salary);

    // field CD - short-term adjuncts
    memcpy(&(school_faculty_array_ex[SHORT_TERM_ADJUNCT]), &(sPtr->school_faculty_array[SHORT_TERM_ADJUNCT]), sizeof(SchoolFaculty));
    school_faculty_array_ex[SHORT_TERM_ADJUNCT].salary
	= int(0.15*school_faculty_array_ex[FULL_PROF].salary
	      + 0.40*school_faculty_array_ex[ASSOC_PROF].salary
	      + 0.45*school_faculty_array_ex[ASST_PROF].salary);

    // Enroll ft,pt_ug,etc (Enroll SL1-5)
    full_time_undergrad   = sPtr->full_time_undergrad;
    part_time_undergrad   = sPtr->part_time_undergrad;
    enrollment_masters    = sPtr->enrollment_masters;
    enrollment_doctoral   = sPtr->enrollment_doctoral;
    non_degree_seeking    = sPtr->non_degree_seeking;

    // field CM - Percent Female SL1-5
    percent_female_sl[0]    = sPtr->female_undergrad_percent;
    percent_female_sl[1]    = char(sPtr->female_undergrad_percent*1.1);
    percent_female_sl[2]    = char(sPtr->female_grad_and_prof_percent*1.1);
    percent_female_sl[3]    = char(sPtr->female_grad_and_prof_percent*0.75);
    percent_female_sl[4]    = char(sPtr->female_undergrad_percent*0.85);

    // field CR - Percent Minority SL1-5
    percent_minority_sl[0]  = char(sPtr->minority_undergrad_percent*0.85);
    percent_minority_sl[1]  = char(sPtr->minority_undergrad_percent*1.2);
    percent_minority_sl[2]  = sPtr->minority_grad_and_prof_percent;
    percent_minority_sl[3]  = char(sPtr->minority_grad_and_prof_percent*0.6);
    percent_minority_sl[4]  = char(sPtr->minority_undergrad_percent*1.1);

    percent_instate_freshmen        = sPtr->freshmen_from_within_the_state_percent;
    percent_non_resident_alien_doctoral = sPtr->non_resident_alien_grad_and_prof_percent;

    //##### begin fred0514 #####
    // field CY
    //memcpy(target_student_intake_sl, sPtr->target_student_intake_sl, sizeof(target_student_intake_sl));
    //memcpy(dropout_rate_sl, sPtr->dropout_rate_sl, sizeof(dropout_rate_sl));
    //memcpy(target_grad_rate_sl, sPtr->target_grad_rate_sl, sizeof(target_grad_rate_sl));
    //##### end fred0514 #####

    // field DN
    ug_applications   = sPtr->student_applications;
    ug_applications_rate  = float(ug_applications)/sPtr->student_accepted;
    ug_yield_rate     = 100-char(float(sPtr->student_enrolled)/sPtr->student_accepted*100.0);

    //TO int the 3nd to 5th vars, as in the excel sheet?
    // fields already defined in base class School
    //percent_ug_students_on_aid		= sPtr->percent_ug_students_on_aid;
    //percent_freshmen_with_need		= sPtr->percent_freshmen_with_need;
    //percent_freshmen_offered_aid			= sPtr->percent_freshmen_offered_aid;
    //percent_freshmen_offered_full_aid	= sPtr->percent_freshmen_offered_full_aid;
    //institutional_aid_per_fte		= sPtr->institutional_aid_per_fte;

    //----- init using degRec -----//
    // (from sheet degree_by_field of excel file master)
    // note: ** major_pref of sl1 = that of sl2 **
    // add degree fields here: student_ifield_pctercne

    //SchoolDegreeSLRec	student_level[3];		// for sl 1&2, 3, 4 // major_pref of sl1 = that of sl2
    //SchoolDegreeSLRec2 student_level5;		// for sl 5

    err_when(UG_TRADITION!=0);
    short sl;

    for (sl=UG_TRADITION; sl<=DOCTOR; sl++) {
	if ( sl != DOCTOR  )
	    init_student_ifield_pct(degRec, sl, 8);
	else
	    init_student_ifield_pct(degRec, sl, 9);     // special case in the database
    }

    // sheet degree_by_field store info in integer (number of student)
    // but spreadsheet init_condi change this to percentage
    short fd;
    float total;

    for (sl=0; sl<MAX_STUDENT_LEVEL_INI; sl++)
	for (fd = 0; fd < IPED_FIELD_COUNT; fd++) {
	    total = student_ifield_pct_total[sl];
	    if ( total <= 0.00001 )                       // total must >= 0
		student_ifield_pct[sl][fd] = 0.0;
	    else
		student_ifield_pct[sl][fd] /= total;
	}

#ifdef DEBUG_NO
    const short specialOrder[IPED_FIELD_COUNT] = {
	AGRICULTURE,INTER_DISCIPLINARY,BUSINESS,COMMUNICAIONS_LIBRARY_SCI,
	MATH_COMP_SCI,EDUCATION,ENGINEERING,FOREIGN_LANGUAGES,HEALTH_SOCIAL_SVCS,HUMANITIES,
	LIFE_PHYSICAL_SCI,SOCIAL_SCIENCES,VISUAL_PERFORMING_ARTS,BASIC_PERSONAL_PRACTICAL_SVCS
    };

    for (short fd=0; fd<IPED_FIELD_COUNT; fd++) {
	char s[300];
	sprintf(s, "schoolex: init: %f ", student_ifield_pct[0][specialOrder[fd]]);
	debug_msg(s);
    }
#endif
    // northwest sl1
    //	0.000	0.021	0.029	0.115	0.035	0.023	0.143	0.012	0.000	0.134	0.085	0.299	0.084	0.020
}

//--------- End of function SchoolEx::init --------//

//--------- Begin of function SchoolEx::init --------//
//!
//! note: ** major_pref of sl1 = that of sl2 **
//!
void SchoolEx::init_student_ifield_pct(SchoolDegreeRec* degRec, short sl, short len) {
    if ( sl != DISTANCE_LEARN ) {
	SchoolDegreeSLRec* slRec = &(degRec->student_level[sl]);

	student_ifield_pct[sl][AGRICULTURE] = (float)  m.atoi( slRec->  agriculture_forestry, len );
	student_ifield_pct[sl][INTER_DISCIPLINARY] = (float)  m.atoi( slRec-> inter_disciplinary, len );
	student_ifield_pct[sl][BUSINESS] = (float)  m.atoi( slRec-> business, len);
	student_ifield_pct[sl][COMMUNICAIONS_LIBRARY_SCI] = (float)  m.atoi( slRec->  communicaions_library_sci, len );
	student_ifield_pct[sl][MATH_COMP_SCI] = (float)  m.atoi( slRec->  math_comp_sci, len );
	student_ifield_pct[sl][EDUCATION] = (float)  m.atoi( slRec->  education, len );

	student_ifield_pct[sl][ENGINEERING] = (float)  m.atoi( slRec->  engineering, len );
	student_ifield_pct[sl][FOREIGN_LANGUAGES] = (float)  m.atoi( slRec->  foreign_languages, len );
	student_ifield_pct[sl][HEALTH_SOCIAL_SVCS] = (float)  m.atoi( slRec-> health_social_svcs, len );
	student_ifield_pct[sl][HUMANITIES] = (float)  m.atoi( slRec-> humanities, len );
	student_ifield_pct[sl][LIFE_PHYSICAL_SCI] = (float)  m.atoi( slRec->  life_physical_sci, len );

	student_ifield_pct[sl][SOCIAL_SCIENCES] = (float)  m.atoi( slRec->  social_sciences, len );
	student_ifield_pct[sl][VISUAL_PERFORMING_ARTS] = (float)  m.atoi( slRec-> visual_Performing_arts, len );
	student_ifield_pct[sl][BASIC_PERSONAL_PRACTICAL_SVCS] = (float)  m.atoi( slRec->  basic_Personal_practical_svcs, len );

	student_ifield_pct_total[sl] = (float)  m.atoi( slRec-> total, len );
    }
    else {
	SchoolDegreeSLRec2* slRec = &(degRec->student_level5);

	student_ifield_pct[sl][AGRICULTURE] = (float)  m.atoi( slRec->  agriculture_forestry, len );
	student_ifield_pct[sl][INTER_DISCIPLINARY] = (float)  m.atoi( slRec-> inter_disciplinary, len );
	student_ifield_pct[sl][BUSINESS] = (float)  m.atoi( slRec-> business, len);
	student_ifield_pct[sl][COMMUNICAIONS_LIBRARY_SCI] = (float)  m.atoi( slRec->  communicaions_library_sci, len );
	student_ifield_pct[sl][MATH_COMP_SCI] = (float)  m.atoi( slRec->  math_comp_sci, len );
	student_ifield_pct[sl][EDUCATION] = (float)  m.atoi( slRec->  education, len );

	student_ifield_pct[sl][ENGINEERING] = (float)  m.atoi( slRec->  engineering, len );
	student_ifield_pct[sl][FOREIGN_LANGUAGES] = (float)  m.atoi( slRec->  foreign_languages, len );
	student_ifield_pct[sl][HEALTH_SOCIAL_SVCS] = (float)  m.atoi( slRec-> health_social_svcs, len );
	student_ifield_pct[sl][HUMANITIES] = (float)  m.atoi( slRec-> humanities, len );
	student_ifield_pct[sl][LIFE_PHYSICAL_SCI] = (float)  m.atoi( slRec->  life_physical_sci, len );

	student_ifield_pct[sl][SOCIAL_SCIENCES] = (float)  m.atoi( slRec->  social_sciences, len );
	student_ifield_pct[sl][VISUAL_PERFORMING_ARTS] = (float)  m.atoi( slRec-> visual_Performing_arts, len );
	student_ifield_pct[sl][BASIC_PERSONAL_PRACTICAL_SVCS] = (float)  m.atoi( slRec->  basic_Personal_practical_svcs, len );

	student_ifield_pct_total[sl] = (float)  m.atoi( slRec-> total, 11 );
    }
}

//--------- End of function SchoolEx::init --------//
