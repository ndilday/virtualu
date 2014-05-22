//Filename    : OFACULTY.cpp
//Description : FACULTY Class Definition - Research!		// 990421

#include <OMATH.H>
//#include <OINFO.H>
//#include <OPSCHOOL.H>		// trimester_array

// game logic

#include <OFACULTY.H>
#include <OFINANCE.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OPEERSCH.H>
#include <OSCHLRES.H>
#include <OPSCHOOL.H>                             //## chea begin "special case" when spon.=none 070699 5.1.4

static bool is_calc_ini_statistic;

/*
//---------- Begin of function Faculty::think_research -----------//
//!
//! called monthly
//!
void Faculty::think_research()
{
  int i;

  //------------------------//
  if ( rank_level == LONG_TERM_ADJUNCT || rank_level == SHORT_TERM_ADJUNCT || ave_proj_size <=0 )
return;

//------------------------//

// 990414

float oRate = finance.get_research_overhead_rate() / 100.0f;
float qualityDriver = this->get_quality_driver();
this->award_prob =
float (0.01 + 0.005 * math.dual_response_func(1,50,100,29,33,20,76, qualityDriver) * math.safe_pow(1.0f+oRate, -1.5f) );

//------------------------//

Department* deptPtr = department_array[department_recno];
DepartmentInfo* deptInfo = department_res[deptPtr->department_id];
FacultyTemplate* templ = faculty_res.get_faculty_template(deptInfo->template_discretionary_time, rank_age_group() );
//	PeerSchool *playerSchool = school_res.player_peer_school;

//------------------------//

for (i=0; i<research_proposal_count; i++)
{
ResearchProposal* researchProposal = research_proposal_array+i;

if ( researchProposal->status == RESEARCH_PROPOSED )
{
// 990412 if ( math.get_random_float() < researchProposal->prob_proposal_funded )
if ( math.get_random_float() < award_prob )
{
// accepted
researchProposal->status = RESEARCH_ACTIVE;
researchProposal->date_to_next_status = info.game_date + (4 + m.random(MAX_ACTIVE_RESEARCH_MONTH-3)) * 30;		// draw a random number from 4 to 12 months

research_month_expense += researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH;
research_month_expense_direct += int((researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH ) / ( 1+researchProposal->overhead_rate));

deptPtr->research_m_history[R_ACCEPT][THIS_MONTH] += (researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH);
}
}
else if ( researchProposal->status == RESEARCH_ACTIVE )
{
}
else
err_here();

if ( info.game_date >= researchProposal->date_to_next_status )		// depends on game_date
{
// expired: proposal rejected or research finished

if ( researchProposal->status == RESEARCH_PROPOSED )
deptPtr->research_m_history[R_REJECT][THIS_MONTH] += (researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH);
else if ( researchProposal->status = RESEARCH_ACTIVE )
{
research_month_expense -= researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH;
research_month_expense_direct -= int((researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH ) / ( 1+researchProposal->overhead_rate));
}

for (int j=i+1; j<research_proposal_count; j++)
memcpy(research_proposal_array+j-1, research_proposal_array+j, sizeof(ResearchProposal));

research_proposal_count--;
i--;
}
}

//------------------------//	var 24

//990412

Faculty* facultyPtr = this;
int projectCount = 0;
float prob=0;
const float forCalcProb = (facultyPtr->ave_proj_count / facultyPtr->award_prob );

float randNum = math.get_random_float();
for ( i=0, prob=0; i<=MAX_RESEARCH_PROPOSAL; i++)
{
//		prob += math.double_poisson(i, forCalcProb);
if ( randNum < prob )
{
projectCount = i;
break;
}
}

//----- add research proposals -----//

for( ; projectCount > 0 && facultyPtr->research_proposal_count < MAX_RESEARCH_PROPOSAL;
projectCount--, facultyPtr->research_proposal_count++ )
{
ResearchProposal* researchProposal = facultyPtr->research_proposal_array + facultyPtr->research_proposal_count;

int tmp = (int) max(0, facultyPtr->ave_proj_size * math.get_random_snd(1.0f, 0.05f));

researchProposal->total_dollars = tmp;

researchProposal->overhead_rate = oRate;

researchProposal->status = RESEARCH_PROPOSED;
researchProposal->date_to_next_status = info.game_date + (2 + m.random(5)) * 30;		// draw a random number from 2 to 6 months

deptPtr->research_m_history[R_PROPOSAL][THIS_MONTH] += (researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH);
}

}
//---------- End of function Faculty::think_research -----------//
*/

//---------- Begin of function DepartmentArray::init_research -----------//
//!
//! called once for each game
//!
void DepartmentArray::init_research() {
    //	return;
    //	err_here();

    is_calc_ini_statistic = false;

    //----//

    int saveGameDate = info.game_date;

    //----//		run for a year; just to generate and update research projects

    const int oneMonth = 30;
    const int simMonths = 6;

    int i, j, m;

    for ( m=1; m<=simMonths; m++ ) {
	if ( m == simMonths )
	    is_calc_ini_statistic = true;

	info.game_date += oneMonth;

	for (i=size(); i>0; i--) {
	    if ( is_deleted(i) )
		continue;

	    Department* dPtr = department_array[i];

	    // for each faculty
	    for ( j=dPtr->faculty_array.size(); j>0; j-- ) {
		if ( dPtr->faculty_array.is_deleted(j) )
		    continue;

		Faculty * fPtr = dPtr->faculty_array[j];

		fPtr->init_research();

		//## chea 301199 since I have run the preru year so this may make the date_to_next_status neg.
		//				if ( m == simMonths )
		//				{
		//					for (int c=0; c<fPtr->research_proposal_count; c++)
		//						fPtr->research_proposal_array[c].date_to_next_status -= simMonths * oneMonth;
		//				}
	    }
	}
    }

    err_when(info.game_date - oneMonth * simMonths != saveGameDate );
    info.game_date = saveGameDate;
    is_calc_ini_statistic = false;                  //## chea BUGHERE after init this should set to true
    //	is_calc_ini_statistic = true;   //## chea BUGHERE after init this should set to true

}

//---------- Begin of function DepartmentArray::init_research -----------//

//---------- Begin of function Faculty::init_research -----------//
//!
//! called once for each game
//!
void Faculty::init_research() {

    //## chea begin "special case" when spon.=none 070699 5.1.4
    if ( player_school.sponsored_research_intensity <= 0 )
	return;
    //## chea end "special case" when spon.=none 070699 5.1.4

    int i;

    //------------------------//
    if ( rank_level == LONG_TERM_ADJUNCT || rank_level == SHORT_TERM_ADJUNCT || ave_proj_size <=0 )
	return;

    //------------------------//

    // 990414

    float oRate = finance.get_research_overhead_rate() / 100.0f;

    float qualityDriver = this->get_quality_driver();
    // change in 18/03/2002
    float awardProb = 2.0f * (qualityDriver * 0.01f) / math.safe_pow( (1.0f + oRate), 3.0f );
    this->award_prob = m.fmin( 0.85f, m.fmax(0.10f, awardProb) );

    //------------------------//

    Department* deptPtr = department_array[department_recno];
    DepartmentInfo* deptInfo = department_res[deptPtr->department_id];
    FacultyTemplate* templ = faculty_res.get_faculty_template(deptInfo->template_discretionary_time, rank_age_group() );
    // PeerSchool *playerSchool = school_res.player_peer_school;

    //------------------------//

    for (i=0; i<research_proposal_count; i++) {

	ResearchProposal* researchProposal = research_proposal_array+i;

	if ( researchProposal->status == RESEARCH_PROPOSED ) {
	    if ( math.get_random_float() < award_prob ) {
		// accepted
		researchProposal->status = RESEARCH_ACTIVE;

		// TRICK #1

		//				researchProposal->date_to_next_status = info.game_date + (4 + m.random(MAX_ACTIVE_RESEARCH_MONTH-3)) * 30;		// draw a random number from 4 to 12 months
		//## chea240899 draw a random number from 0 to 6 months  //## chea 251099
		researchProposal->date_to_next_status = info.game_date + (m.random(6)) * 30;
		//				researchProposal->date_to_next_status = info.game_date + (1) * 30;		//## chea 251099 test

		//## chea 151199 try my cal. method 1.4.1
		research_month_expense += researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH;
		research_month_expense_direct += int((researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH ) / ( 1+researchProposal->overhead_rate));

		if ( is_calc_ini_statistic ) {
		    // not sure why divide after add
		    deptPtr->research_m_history[R_ACCEPT][THIS_MONTH] += (float)researchProposal->total_dollars / (MAX_ACTIVE_RESEARCH_MONTH*1000);
		}
	    }
	}
	else if ( researchProposal->status == RESEARCH_ACTIVE ) {
	}
	else
	    err_here();

	// TRICK #2
	// depends on game_date  //## chea 031199
	if ( info.game_date>= researchProposal->date_to_next_status ) {
	    // expired: proposal rejected or research finished

	    if ( researchProposal->status == RESEARCH_PROPOSED ) {
		if ( is_calc_ini_statistic ) {
		    deptPtr->research_m_history[R_REJECT][THIS_MONTH] += (float)researchProposal->total_dollars / (MAX_ACTIVE_RESEARCH_MONTH * 1000);
		}
	    }
	    // fix in ver 2
	    // else if ( researchProposal->status = RESEARCH_ACTIVE )
	    else if ( researchProposal->status == RESEARCH_ACTIVE ) {
		//## chea 151199 try my cal. method 1.4.1
		research_month_expense -= researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH;
		research_month_expense_direct -= int((researchProposal->total_dollars / MAX_ACTIVE_RESEARCH_MONTH ) / ( 1+researchProposal->overhead_rate));

	    }
	    else
		err_here();

	    for (int j=i+1; j<research_proposal_count; j++) {
		memcpy(research_proposal_array+j-1, research_proposal_array+j, sizeof(ResearchProposal));
	    }
	    research_proposal_count--;

	    i--;
	}
    }

    //----------------------------------//

    Faculty* facultyPtr = this;
    int projectCount = 0;
    float prob=0;

    //## chea 161099
    //	const float forCalcProb = (facultyPtr->ave_proj_count / facultyPtr->award_prob ); //tryttt
    //   const float forCalcProb = (facultyPtr->ave_proj_count * deptInfo->percent_pi_faculty/100.0f / facultyPtr->award_prob );  //## chea 230899
    //tryttt
    const float forCalcProb = (facultyPtr->award_prob);

    //## chea 161099  it is impossiable to added k1 in the alg.
    //	const float forCalcProb = (k1*GCON*facultyPtr->ave_proj_count / (DCON*facultyPtr->award_prob ));  //## chea 161099

    //## 251099 use k1&k2  =1
    //	const float forCalcProb = (k1*GCON*facultyPtr->ave_proj_count / (DCON*facultyPtr->award_prob ));  //## chea 161099
    // const float forCalcProb = (GCON*facultyPtr->ave_proj_count / (DCON*facultyPtr->award_prob ));  //## chea 161099

    float randNum = math.get_random_float();
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
	ResearchProposal* researchProposal = facultyPtr->research_proposal_array + facultyPtr->research_proposal_count;

	//## chea 251099
	//		int tmp = (int) max(0.0f, facultyPtr->ave_proj_size * math.get_random_snd(1.0f, 0.05f)); //min & max bug chea
	//		int tmp = (int) max(0.2f, facultyPtr->ave_proj_size * math.get_random_snd(1.0f, 0.20f)); //min & max bug chea
	//		int tmp = (int) max(0.2f, facultyPtr->ave_proj_size ); //min & max bug chea //for testing
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

	//TRICK #3
	// draw a random number from 2 to 6 months
	researchProposal->date_to_next_status = info.game_date + (2 + m.random(5)) * 30;
	//		researchProposal->date_to_next_status = info.game_date + (1) * 30;		//## chea 251099 test
	//		researchProposal->date_to_next_status = info.game_date + (m.random(7)) * 30;		//## chea 240899 draw a random number from 2 to 6 months //## chea 251099

	if ( is_calc_ini_statistic ) {
	    deptPtr->research_m_history[R_PROPOSAL][THIS_MONTH] += (float)researchProposal->total_dollars / (MAX_ACTIVE_RESEARCH_MONTH * 1000);
	}
    }

    for (int c=0; c<research_proposal_count; c++)
	//## chea 031199
	err_when( research_proposal_array[c].date_to_next_status > info.game_date+30*18 );
}

//---------- End of function Faculty::init_research -----------//
