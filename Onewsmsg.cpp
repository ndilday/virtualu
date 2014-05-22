//Filename    : ONEWSMSG.CPP
//Description : Object news msg generating routines News::msg() and its helper
//Owner			: Fred

#include <OSTR.H>
#include <ONEWS.H>
#include <OFONT.H>
#include <ODATE.H>
#include <OINFO.H>
#include <OCONFIG.H>

#include <OSYS.H>
#include <OCHANCE.H>
#include <OPSCHOOL.H>
#include <OFINANCE.H>
#include <ODEPT.H>

//--------- Define variable type ----------//

typedef void (News::*NewsFP)();

//-------- Define struct NewsInfo -------//

//!Structure which wraps around News routine for generating message.
struct NewsInfo {
    NewsFP news_function_ptr;
};

//------ Define function pointers array ------//

static NewsInfo news_info_array[NEWS_ID_COUNT] = {
    { &News::alumni_die },
    { &News::rape_in_campus },

    { &News::retrofit_facilities },
    { &News::earth_quake },

    { &News::congress_research },
    { &News::stock_market },
    { &News::scandalous_fund_use },
    { &News::new_governor },
    { &News::court_limit_minority },
    { &News::risky_research },
    { &News::scandal_in_athletics },

    { &News::research_prize },
    { &News::technology_transfer },
    { &News::capital_campaign },
    { &News::student_loans },

    { &News::plagiarism_scandal },
    { &News::for_profit_competition },
    { &News::minority_student_percentage_falls },
    { &News::successful_grad },
    { &News::media_obtain_information },

    // protagonist message
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },

    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },

    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },

    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },
    { &News::protagonist_message },

    { &News::new_year_start },
    { &News::scenario_near_end },
    { &News::scenario_news },
    { &News::football_game },
    { &News::basketball_game },

    { &News::open_game_near_end },
    { &News::open_game_end },

    { &News::bankruptcy_message_lv1 },
    { &News::bankruptcy_message_lv2 },
    { &News::bankruptcy_message_lv3 },

    { &News::task_msg },
};

//------- Define static variables --------//

String News::str;

//------ Begin of function News::msg -----//
//!
//! Return the msg string of current news.
//!
char* News::msg() {
    NewsFP newsFP = news_info_array[id-1].news_function_ptr;

    (this->*newsFP)();                              // call the corrsponding function to return the news msg

    line_count=font_chartsm.count_line(400,str);

    return str;
}

//------- End of function News::msg -----//

//------ Begin of function News::set_newspaper_popup -----//
//!
void News::set_newspaper_popup() {
    sys.set_news_pop_up();
    News* newsPtr = this;
    chance_event.news_str = newsPtr->msg();
    chance_event.news_news_str=chance_event.news_str;

    //--- convert the year to 200? year format -----//

    int thisDate = date.julian( date.year(newsPtr->news_date),
				date.month(newsPtr->news_date),
				date.day(newsPtr->news_date) );

    // memcpy(chance_event.news_date_str, date.date_str(thisDate),55);
    strncpy( chance_event.news_date_str, info.date_str_julian(thisDate), 55);
    chance_event.news_date_str[55-1] = '\0';
    // chance_event.date_str = date.date_str(thisDate,1,0);
    chance_event.date_str = info.date_str_julian( thisDate, INFO_DATE_DEFAULT|INFO_DATE_SHORT_MONTH|INFO_DATE_NO_DAY);
}

//------- End of function News::set_newspaper_popup -----//

//------ Begin of function News::alumni_die -----//
//!
//! short_para1 = gender		// { GENDER_MALE, GENDER_FEMALE };
//! short_para2 = bequest
//!
void News::alumni_die() {
    //---------------- Text Format -----------------//
    //
    // An old <alumnus/alumna> dies andleaves a bequest of $<bequest> thousand which is transferred to Development Office.
    //
    //----------------------------------------------//

    str = "An old ";

    if( short_para1 == GENDER_MALE )
	str += "alumnus";
    else
	str += "alumna";

    err_when(short_para2<0);

    str += " dies and leaves a bequest of $";
    str += m.format(short_para2);
    str += " thousand which is transferred to the Development Office.";
}

//------- End of function News::alumni_die -----//

//------ Begin of function News::rape_in_campus -------//
//!
void News::rape_in_campus() {
    str = "Assaults occur on campus. Student and alumni morale plummet. ";
    str += "You need to spend more on \"Administration\" (which includes spending for Security).";
}

//------- End of function News::rape_in_campus --------//

//------ Begin of function News::retrofit_facilities -------//
//!
//! <int> short_para1 = dollarLost;
//!
void News::retrofit_facilities() {
    str = "More physically challenged students require modification of facilities. Lay out $";
    str += m.format(short_para1);
    str += " thousand.";
}

//------- End of function News::retrofit_facilities --------//

//------ Begin of function News::earth_quake -------//
//!
void News::earth_quake() {
    str = "Earthquake. Your available square footage drops dramatically. Faculty, staff, and students report that they have insufficient space.";
}

//------- End of function News::earth_quake --------//

//------ Begin of function News::congress_research -------//
//!
//! short_para1 = adjust direction
//!
void News::congress_research() {
    str = "Congress adjusts research funding ";
    str += (short_para1>=0) ? "positively.":"negatively.";
}

//------- End of function News::congress_research --------//

//------ Begin of function News::stock_market -------//
//!
//! short_para1 = adjust direction
//!
void News::stock_market() {
    str = "Stock market ";
    str += (short_para1>=0) ? "soars.":"crashes.";
}

//------- End of function News::stock_market --------//

//------ Begin of function News::scandalous_fund_use -------//
//!
//! short_para1 = dollarLost
//!
void News::scandalous_fund_use() {
    str = "Auditors find scandalous use of funds. Senior officer of the University resigns. $";
    str += m.format(short_para1);
    str += " thousands disappear.  Fund-raising depressed.";
}

//------- End of function News::scandalous_fund_use --------//

//------ Begin of function News::new_governor-------//
//!
//! short_para1 = direction
//!
void News::new_governor() {
    str = "A new governor of your state is elected. ";

    if ( short_para1>=0 ) {
	str += "She likes the University and raises the appropriation.";
    }
    else {
	str += "He wants to spend on prisons, not universities.  Your appropriation drops.";
    }
}

//------- End of function News::new_governor --------//

//------ Begin of function News::court_limit_minority-------//
//!
void News::court_limit_minority() {
    str = "Courts ban favoring admission and financial aid on the basis of race or ethnicity.";
}

//------- End of function News::court_limit_minority --------//

//## chea 150699
//------ Begin of function News::risky_research-------//
//!
void News::risky_research() {

    str = "Risky genetic research on your campus attracts demostrators, who cause $";
    str += m.format(short_para1);
    str += " thousand in damage to the laboratory. The insurance company will not pay.";

}

//------- End of function News::risky_research --------//
//## chea 150699

//## chea 170699
//------ Begin of function News::scandal_in_athletics-------//
//!
void News::scandal_in_athletics() {
    str = "Your athletes took illegal drugs. Your athletic talent drops and you cannot recruit athletes for 3 years. Donations fall.";
}

//------- End of function News::scandal_in_athletics --------//
//## chea 170699

//## chwg 170699
//------ Begin of function News::plagiarism_scandal-------//
//!
void News::plagiarism_scandal() {
    str = "Plagiarism scandal rocks the University. ";
    str += m.format(short_para1,4);
    str += " students dismissed. Morale and donations fall.";
}

//------- End of function News::plagiarism_scandal --------//
//## chwg 170699

//## chwg 190699
//------ Begin of function News::for_profit_competition-------//
//!
void News::for_profit_competition() {
    str = "For-profit universities are growing fast and attracting students. The University cannot raise tuition for the next 3 years.";
    //		if((yield_for_profit_competition-1.0f)>0)
}

//------- End of function News::for_profit_competition --------//
//## chwg 190699

//## chwg 210699
//------ Begin of function News::minority_student_percentage_falls-------//
//!
void News::minority_student_percentage_falls() {
    str = "Courts ban favoring admission and financial aid on the basis of race or ethnicity.";
}

//------- End of function News::minority_student_percentage_falls --------//
//## chwg 210699

//## chwg 210699
//------ Begin of function News::successful_grad-------//
//!
void News::successful_grad() {
    if(m.random(2)>1)
	str = "A graduate wins the Oscar for Best Actress and credits the University. Applications rise.";
    else
	str = "A graduate is elected president of the United States and credits the University. Applications rise.";
}

//------- End of function News::successful_grad --------//
//## chwg 210699

//## chwg 210699
//------ Begin of function News::media_obtain_infomation-------//
//!
//! short_para1 = true cost education
//!
void News::media_obtain_information() {

    str = "An investigative reporter obtains information about 'the true cost of education' ";
    str += m.format(float_para1,2);

    str +=" relative to tuition income";            // (for private institutions)

    if(!player_school.is_public())
	str +=".";
    else
	str +=" plus state appropriation";            // (for public institutions).";
}

//------- End of function News::media_obtain_infomation --------//
//## chwg 210699

//------ Begin of function News::research_prize() -------------//
//!
//! short_para1 - the department recno
//! short_para2 - the faculty recno in the department.
//! short_para3 - first name id
//! short_para4 - middle name id
//! short_para5 - last name id
//!
void News::research_prize() {
    Department *dept = department_array[short_para1];

    str = "Professor ";
    if ( dept->faculty_array.is_deleted( short_para2 ) ) {
	str += Faculty::name( short_para3, short_para4, short_para5 );
    }
    else {
	str += dept->faculty_array[short_para2]->name();
    }
    str += " receives the Nobel Prize. Prestige soars.";
}

//------- End of function News::research_prize() --------------//

//## chea begin 260699
//------ Begin of function News::technology_transfer() -------------//
//!
//! short_para1 - the recno of the department
//! float_para1 - the amount of technology transfer.
//!
void News::technology_transfer() {
    str="Your ";
    str += department_array[short_para1]->name();
    str += " signs a deal with a company for a breakthrough that the company will market. For the next 5 years, the University receives $";
    str += m.format((int)float_para1);
    str += " thousand/year.";

}

//------- End of function News::technology_transfer() --------------//
//## chea end 260699

//------ Begin of function News::capital_campaign() -------------//
void News::capital_campaign() {
    str="You announce a capital campaign. Note that spending for the Development has increased.";
}

//------- End of function News::capital_campaign() --------------//

//------ Begin of function News::student_loans() -------------//
void News::student_loans() {
    if ( short_para1 == 0 )
	str="Student loans become harder to get. Applications rise at cheaper public institutions and fall at the more expensive privates.";
    else
	str="Student loans become easier to get. Applications rise at the more expensive privates and fall at cheaper public institutions.";
    //## chea (wfm: March 10,1999) 17b
}

//------- End of function News::student_loans() --------------//

//------ Begin of function News::football_game -------//
//!
//! short_para1 = dollar in K
//!
void News::football_game() {
    str = "Your football team wins the league title and goes to a New Year's Day Bowl Game. ";
    str += "You earn $";
    str += m.format(short_para1);
    str += " thousand, and donations and applications rise.";
}

//------- End of function News::football_game --------//

//------ Begin of function News::basketball_game -------//
//!
//! short_para1 = dollar in K
//!
void News::basketball_game() {
    str = "Your hoopsters win their league title. ";
    str += "You earn $";
    str += m.format(short_para1);
    str += " thousand, and donations and applications rise.";
}

//------- End of function News::basketball_game --------//

// --- Begin of function News::bankruptcy_message_lv1 --- //
//
void News::bankruptcy_message_lv1() {
    str = "The Financial Office projects bankruptcy at year-end. You must act now to avoid fiscal disaster.";
}

// --- End of function News::bankruptcy_message_lv1 --- //

// --- Begin of function News::bankruptcy_message_lv2 --- //
//
void News::bankruptcy_message_lv2() {
    str = "The Finance Office projects bankruptcy in warning in ";
    str += m.format(short_para1+12);
    str += " months. You should take remedial action very soon.";
}

// --- End of function News::bankruptcy_message_lv2 --- //

// --- Begin of function News::bankruptcy_message_lv3 --- //
//
void News::bankruptcy_message_lv3() {
    str = "The Finance Office projects bankruptcy in warning in ";
    str += m.format(short_para1+24);
    str += " months. You should change your budget policies as soon as possible.";
}

// --- End of function News::bankruptcy_message_lv3 --- //

//------ Begin of function News::new_year_start -------//
//!
void News::new_year_start() {
    str = "31 August deadline is approaching for next year's budget decisions.";
}

//------- End of function News::new_year_start --------//

//------ Begin of function News::scenario_near_end -------//
//!
//!
void News::scenario_near_end() {
    str = "Make haste! You have only one year left to achieve your goal(s).";
}

//------- End of function News::scenario_near_end --------//

//------ Begin of function News::open_game_near_end -------//
//!
//!
void News::open_game_near_end() {
    str = "This is a demo of the custom game and it will end at the end of the upcoming year.";

#ifdef DEMO
    str = "This demo game will end at the end of the upcoming year.";
#endif

#ifdef ADMIN
    str = "The simulation will complete at the end of the upcoming year.";
#endif
}

//------- End of function News::open_game_near_end --------//

//------ Begin of function News::open_game_end -------//
//!
void News::open_game_end() {
    str = "This is a demo of the custom game and it has reached its time limit. Please consider buying the Administrator version for fully functional custom games.";

#ifdef DEMO
    str = "This demo game has reached its time limit. Please consider buying the full version to extend play.";
#endif

#ifdef ADMIN
    str = "You have reached the end of the simulation. Congratulations.";
#endif
}

//------- End of function News::open_game_end --------//

//------ Begin of function News::scenario_news -------//
//!
//! newsPtr->short_para1 = type;			// PLAQUE_NEWS
//! newsPtr->short_para2 = subType;		// PLAQUE_GOLD
//! newsPtr->short_para3 = year;			// 1998
//!
void News::scenario_news() {
    if ( short_para1 == 0 ) {
	char *plaqueStr[] = { "Summa Cum Laude", "Magna Cum Laude", "Cum Laude" };

	str = "Congratulations! You have achieved the ";
	str += plaqueStr[short_para2];
	str += " scenario goal by September ";
	str += m.format(short_para3,4);
	str += ".  In honor of this occasion, the Board of Trustees is pleased to present you with this ";
	str += plaqueStr[short_para2];
	str += " plaque.";
    }
    else if ( short_para1 == 1 ) {
	str = "The time limit for this scenario has expired.";
    }
    else
	err_here();
}

//------- End of function News::scenario_news --------//

// News::protagonist_message moved to OPSCH_PO.CPP
