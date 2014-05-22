//Filename    : ONEWS2.CPP
//Description : News adding functions for NewsArray::add_news()
//Owner			: Fred

#include <OAUDIO.H>
#include <ODATE.H>
#include <OCONFIG.H>
#include <OSYS.H>
#include <OINFO.H>
#include <OMATH.H>
#include <ONEWS.H>
#include <OCHANCE.H>
#include <OFINANCE.H>
#include <OENROLL.H>
#include <OPSCHOOL.H>
#include <OATHLETI.H>
#include <ODEVELOP.H>
#include <ODEPT.H>

//------ Begin of function NewsArray::diplomacy -----//
//!
//! <int> gender = { GENDER_MALE, GENDER_FEMALE };
//! <int> bequest = dollar in million
//!
//! short_para1 = the recno of the TalkMsg in talk_res.talk_msg_array
//!
void NewsArray::alumni_die(int gender, int bequest) {
    //----------- add news --------------//

    News* newsPtr = add_news( NEWS_ALUMNI_DIE );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = gender;
    newsPtr->short_para2 = bequest;

    err_when(newsPtr->short_para1<0);
}

//------- End of function NewsArray::diplomacy -----//

//------ Begin of function NewsArray::rape_in_campus -----//
//!
//! <int>
//!
void NewsArray::rape_in_campus() {
    News* newsPtr = add_news( NEWS_RAPE_IN_CAMPUS );

    if( !newsPtr )
	return;
}

//------- End of function NewsArray::rape_in_campus -----//

//------ Begin of function NewsArray::retrofit_facilities -----//
//!
//! <int> short_para1 = dollarLost;
//!
void NewsArray::retrofit_facilities(int dollarLost) {
    News* newsPtr = add_news( NEWS_RETROFIT_FACILITIES );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = dollarLost;

    err_when(newsPtr->short_para1 <= 0 );
}

//------- End of function NewsArray::retrofit_facilities -----//

//------ Begin of function NewsArray::earth_quake -----//
//!
//! <int>
//!
void NewsArray::earth_quake() {
    News* newsPtr = add_news( NEWS_EARTH_QUAKE );

    if( !newsPtr )
	return;
}

//------- End of function NewsArray::earth_quake -----//

//------ Begin of function NewsArray::congress_research -----//
//!
//! short_para1 = adjust direction
//!
void NewsArray::congress_research(char dir) {
    News* newsPtr = add_news( NEWS_CONGRESS_RESEARCH );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = (dir >= 0) ? 1 : -1;
}

//------- End of function NewsArray::congress_research -----//

//## chea begin 160699
//------ Begin of function NewsArray::risky_research -----//
//!
//! short_para1 = inc //in operation reserve.
//!
void NewsArray::risky_research(int moneyLost) {
    News* newsPtr = add_news( NEWS_RISKY_RESEARCH );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = moneyLost;
}

//------- End of function NewsArray::risky_research -----//
//## chea end 160699

//## chea begin 170699
//------ Begin of function NewsArray::scandal_in_athletics -----//
//!
//! short_para1 = inc //in operation reserve.
//!
void NewsArray::scandal_in_athletics() {
    News* newsPtr = add_news( NEWS_SCANDAL_IN_ATHLETICS );

    if( !newsPtr )
	return;

}

//------- End of function NewsArray::scandal_in_athletics -----//
//## chea end 170699

//## chwg begin 170699
//------ Begin of function NewsArray::plagiarism_scandal -----//
//!
//! short_para1 = number of dismissed students.
//!
void NewsArray::plagiarism_scandal(int dismissedStudCount) {
    News* newsPtr = add_news( NEWS_PLAGIARISM_SCANDAL );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = dismissedStudCount;
}

//------- End of function NewsArray::plagiarism_scandal -----//
//## chwg end 170699

//## chwg begin 190699
//------ Begin of function NewsArray::for_profit_competition -----//
//!
//! short_para1 = inc //in operation reserve.
//!
void NewsArray::for_profit_competition() {
    News* newsPtr = add_news( NEWS_FOR_PROFIT_COMPETITION );

    if( !newsPtr )
	return;

}

//------- End of function NewsArray::for_profit_competition -----//
//## chwg end 190699

//## chwg begin 210699
//------ Begin of function NewsArray::minority_student_percentage_falls -----//
//!
//! short_para1 = inc //in operation reserve.
//!
void NewsArray::minority_student_percentage_falls() {
    News* newsPtr = add_news( NEWS_MINORITY_STUDENT_PERCENTAGE_FALLS );

    if( !newsPtr )
	return;

}

//------- End of function NewsArray::minority_student_percentage_falls -----//
//## chwg end 210699

//## chwg begin 210699
//------ Begin of function NewsArray::successful_grad -----//
//!
//! short_para1 = inc //in operation reserve.
//!
void NewsArray::successful_grad() {
    News* newsPtr = add_news( NEWS_SUCCESSFUL_GRAD );

    if( !newsPtr )
	return;

}

//------- End of function NewsArray::successful_grad -----//
//## chwg end 210699

//## chwg begin 210699
//------ Begin of function NewsArray::media_obtain_infomation -----//
//!
//! short_para1 = true cost education
//!
void NewsArray::media_obtain_information(float trueCostEducation) {
    News* newsPtr = add_news( NEWS_MEDIA_OBTAIN_INFORMATION );

    if( !newsPtr )
	return;

    newsPtr->float_para1 = trueCostEducation;
}

//------- End of function NewsArray::media_obtain_infomation -----//
//## chwg end 210699

//------ Begin of research_prize -------//
//
// <int> deptRecno    - the recno of the department that receives the prize.
// <int> facultyRecno - the recno of the faculty who receives the prize.
//
void NewsArray::research_prize(int deptRecno, int facultyRecno) {
    News* newsPtr = add_news( NEWS_RESEARCH_PRIZE );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = deptRecno;
    newsPtr->short_para2 = facultyRecno;

    Department* deptPtr = department_array[deptRecno];
    Faculty* facultyPtr = deptPtr->faculty_array[facultyRecno];
    newsPtr->short_para3 = facultyPtr->first_name_id;
    newsPtr->short_para4 = facultyPtr->middle_name_id;
    newsPtr->short_para5 = facultyPtr->last_name_id;
}

//------ End of research_prize------//

// ------ Begin of technology_transfer------------//
//
// <int>		the recno of the department
// <float>  the amount of technology transfer.
//
void NewsArray::technology_transfer(int deptRecno, float transferAmount) {
    News* newsPtr = add_news( NEWS_TECHNOLOGY_TRANSFER );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = deptRecno;
    newsPtr->float_para1 = transferAmount;
}

//------ End of technology_transfer------//

// ------ Begin of capital_campaign------------//
void NewsArray::capital_campaign() {
    News* newsPtr = add_news( NEWS_CAPITAL_CAMPAIGN );

    if( !newsPtr )
	return;
}

//------ End of capital_campaign------//

// ------ Begin of student_loans------------//
void NewsArray::student_loans(int is_increase) {
    News* newsPtr = add_news( NEWS_STUDENT_LOANS );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = is_increase;
}

//------ End of student_loans------//

//------ Begin of function NewsArray::stock_market -----//
//!
//! short_para1 = adjust direction
//!
void NewsArray::stock_market(char dir) {
    News* newsPtr = add_news( NEWS_STOCK_MARKET );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = (dir >= 0) ? 1 : -1;
}

//------- End of function NewsArray::stock_market -----//

//------ Begin of function NewsArray::scandalous_fund_use -----//
//!
//! short_para1 = dollarLost
//!
void NewsArray::scandalous_fund_use(int dollarLost) {
    News* newsPtr = add_news( NEWS_SCANDALOUS_FUND_USE );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = dollarLost;
}

//------- End of function NewsArray::scandalous_fund_use -----//

//------ Begin of function NewsArray::new_governor -----//
//!
//! short_para1 = direction
//!
void NewsArray::new_governor(char direction) {
    News* newsPtr = add_news( NEWS_NEW_GOVENOR );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = direction;
}

//------- End of function NewsArray::new_governor -----//

//------ Begin of function NewsArray::court_limit_minority -----//
//!
void NewsArray::court_limit_minority() {
    News* newsPtr = add_news( NEWS_COURT_LIMIT_MINORITY );

    if( !newsPtr )
	return;
}

//------- End of function NewsArray::court_limit_minority -----//
// short_para1 - opinion rating (1-6:R1-R6)
// short_para2 - departmentRecno (-1 for institutional)
// short_para3 - source of message(srcPara1), gender-ethnic group of student, faculty first_name_id for faculty
// short_para4 - srcPara2, gender-ethnic group of student, faculty middle_name_id for faculty
// short_para5 - srcPara3, gender-ethnic group of student, faculty last_name_id for faculty
// short_para6 - srcPara4, gender-ethnic group of student, faculty rank_level
void NewsArray::protagonist_message( int protagonistId, int rating, int departmentRecno, int srcPara1, short srcPara2, short srcPara3, short srcPara4 ) {
    int newsId = NEWS_PROTAGONIST_TRUSTEE_EVALUATION+protagonistId-1;
    err_when( newsId < NEWS_PROTAGONIST_TRUSTEE_EVALUATION || newsId > NEWS_PROTAGONIST_CLASS_SIZE_DEVIATION );

    News *newsPtr = add_news( newsId );
    if( !newsPtr )
	return;
    newsPtr->short_para1 = rating;
    newsPtr->short_para2 = departmentRecno;
    newsPtr->short_para3 = srcPara1;
    newsPtr->short_para4 = srcPara2;
    newsPtr->short_para5 = srcPara3;
    newsPtr->short_para6 = srcPara4;
}

// --- Begin of function NewsArray::bankruptcy_message_lv1 --- //
//
void NewsArray::bankruptcy_message_lv1() {
    News *newsPtr = add_news( NEWS_BANKRUPTCY_LV1 );

    if ( !newsPtr )
	return;
}

//
// --- End of function NewsArray::bankruptcy_message_lv1 --- //

// --- Begin of function NewsArray::bankruptcy_message_lv2 --- //
//
void NewsArray::bankruptcy_message_lv2( int monthsToBankruptcy ) {
    News *newsPtr = add_news( NEWS_BANKRUPTCY_LV2 );

    if ( !newsPtr )
	return;

    newsPtr->short_para1 = monthsToBankruptcy;
}

//
// --- End of function NewsArray::bankruptcy_message_lv2 --- //

// --- Begin of function NewsArray::bankruptcy_message_lv3 --- //
//
void NewsArray::bankruptcy_message_lv3( int monthsToBankruptcy ) {
    News *newsPtr = add_news( NEWS_BANKRUPTCY_LV3 );

    if ( !newsPtr )
	return;
    newsPtr->short_para1 = monthsToBankruptcy;
}

//
// --- End of function NewsArray::bankruptcy_message_lv3 --- //

//------ Begin of function NewsArray::football_game -----//
//!
//! <int>
//!
void NewsArray::football_game(int numWins) {
    const int numWinsRange = 3, numWinsMin = 9;

    if ( numWins < numWinsMin)
	return;

    if( chance_event.test_chance != NEWS_FOOTBALL_GAME ) {
	if ( player_school.chance_event_frequency == INPUT_LOW || chance_event.football_game_bonus > 0 )
	    return;

	//	if (numWins >= numWinsMin+numWinsRange)
	//		numWins = numWinsMin+numWinsRange-1;  //old idea this don't stop the pree after player win the title

	//## chea 141099 stop the pree after player win the title
	if ( numWins > numWinsMin+numWinsRange )
	    return;
    }
    else
	chance_event.test_chance = 0;

    //---//

    News* newsPtr = add_news( NEWS_FOOTBALL_GAME );

    if( !newsPtr )
	return;

    //!
    struct DollarSet {
	float min, avg, sd;
    };

    DollarSet dollarSet[numWinsRange] = { {         // $ in million
	1, 2, 0.5f
    }
					  , {
					      2, 4, 1
					  }
					  , {
					      5, 10, 2.5f
					  }
					  ,
    };
    char idx = numWins - numWinsMin;
    float factor = 1.0f / math.safe_pow(athletics_office.ncaa_level_value, 1.2f);

    float dollarValue
	= factor  * min( dollarSet[idx].min,
			 math.get_random_snd(dollarSet[idx].avg, dollarSet[idx].sd));

    dollarValue *= 1000;                            // $ unit in thousand

    // to be added to athletic revenue
    chance_event.football_game_bonus = (short) dollarValue;

    newsPtr->short_para1 = (short) dollarValue;

    //---//

    enroll_res.set_app_yield(
	factor * m.fmin(1.1f, math.get_random_snd(1.25f, 0.05f)),
	factor * m.fmin(1.05f, math.get_random_snd(1.15f, 0.04f))
	);

    development_office.alumni_morale *= factor * m.fmin(1.05f, math.get_random_snd(1.15f, 0.04f));
    player_school.media_attitudes = (char) int( player_school.media_attitudes * factor * m.fmin(1.05f, math.get_random_snd(1.15f, 0.04f)));

    //---//

    newsPtr->set_newspaper_popup();
}

//------- End of function NewsArray::football_game -----//

//------ Begin of function NewsArray::basketball_game -----//
//!
//! <int>
//!
void NewsArray::basketball_game(int numWins) {
    const int numWinsRange = 3, numWinsMin = 25;

    if ( numWins < numWinsMin )
	return;

    if( chance_event.test_chance != NEWS_BASKETBALL_GAME ) {
	if ( player_school.chance_event_frequency == INPUT_LOW || chance_event.basketball_game_bonus > 0 )
	    return;

	//	if ( numWins >= numWinsMin+numWinsRange )
	//		numWins = numWinsMin+numWinsRange-1;   //old idea this don't stop the pree after player win the title

	//## chea 141099 stop the pree after player win the title
	if ( numWins > numWinsMin+numWinsRange )
	    return;
    }
    else
	chance_event.test_chance = 0;

    //---//

    News* newsPtr = add_news( NEWS_BASKETBALL_GAME );

    if( !newsPtr )
	return;

    //!
    struct DollarSet {
	float min, avg, sd;
    };

    DollarSet dollarSet[numWinsRange] = { {         // $ in million
	.1f, .25f, .1f
    }
					  , {
					      .3f, .5f, .15f
					  }
					  , {
					      1, 2.5f, .5f
					  }
					  ,
    };
    char idx = numWins - numWinsMin;
    float factor = 1.0f / math.safe_pow(athletics_office.ncaa_level_value, 2);

    float dollarValue
	= factor  * m.fmin( dollarSet[idx].min,
			    math.get_random_snd(dollarSet[idx].avg, dollarSet[idx].sd));

    dollarValue *= 1000;                            // $ unit in thousand

    // to be added to athletic revenue
    chance_event.basketball_game_bonus = (short) dollarValue;

    newsPtr->short_para1 = (short) dollarValue;

    //---//

    enroll_res.set_app_yield(
	factor * m.fmin(1.1f, math.get_random_snd(1.25f, 0.05f)),
	factor * m.fmin(1.05f, math.get_random_snd(1.15f, 0.04f))
	);

    //---//

    newsPtr->set_newspaper_popup();
}

//------- End of function NewsArray::basketball_game -----//

//------ Begin of function NewsArray::new_task -----//
//!
//! <int> taskPtr - task info
//! [int] subtype=TASK_STARTED by default
//!
void NewsArray::new_task(News *taskPtr, char subtype) {
    News *newsPtr = add_news(NEWS_NEW_TASK, NEWS_NORMAL);

    newsPtr->task_id = taskPtr->id;
    newsPtr->task_msg_subtype = subtype;            // enum { TASK_STARTED=0, TASK_EXPIRED, TASK_COMPLETED };
    newsPtr->short_para1 = taskPtr->short_para1;
    newsPtr->short_para2 = taskPtr->short_para2;
    newsPtr->short_para3 = taskPtr->short_para3;
    newsPtr->short_para4 = taskPtr->short_para4;
    newsPtr->short_para5 = taskPtr->short_para5;
    newsPtr->short_para6 = taskPtr->short_para6;
    newsPtr->short_para7 = taskPtr->short_para7;
}

//------- End of function NewsArray::new_task -----//

//------ Begin of function NewsArray::new_year_start -----//
//!
//! <int>
//!
void NewsArray::new_year_start() {
    News* newsPtr = add_news( NEWS_NEW_YEAR_START, NEWS_NORMAL );

    if( !newsPtr )
	return;
}

//------- End of function NewsArray::new_year_start -----//

//------ Begin of function NewsArray::scenario_near_end -----//
//!
//! <int>
//!
void NewsArray::scenario_near_end() {
    News* newsPtr = add_news( NEWS_SCENARIO_NEAR_END, NEWS_NORMAL );

    if( !newsPtr )
	return;
}

//------- End of function NewsArray::scenario_near_end -----//

//------ Begin of function NewsArray::open_game_near_end -----//
//!
void NewsArray::open_game_near_end() {
    News* newsPtr = add_news( NEWS_OPEN_GAME_NEAR_END, NEWS_NORMAL );

    if( !newsPtr )
	return;

    //--- do the following to make the news pop up ----//

    sys.set_news_pop_up();

    chance_event.news_str=newsPtr->msg();
    chance_event.news_news_str = chance_event.news_str;

    // memcpy(chance_event.news_date_str, date.date_str(info.game_date),55);
    strncpy( chance_event.news_date_str, info.date_str_julian(info.game_date), 55 );
    chance_event.news_date_str[55-1] = '\0';
}

//------- End of function NewsArray::open_game_near_end -----//

//------ Begin of function NewsArray::open_game_end -----//
//!
void NewsArray::open_game_end() {
    News* newsPtr = add_news( NEWS_OPEN_GAME_END, NEWS_NORMAL );

    if( !newsPtr )
	return;

    //--- do the following to make the news pop up ----//

    sys.set_news_pop_up();

    chance_event.news_str=newsPtr->msg();
    chance_event.news_news_str = chance_event.news_str;

    // memcpy(chance_event.news_date_str, date.date_str(info.game_date),55);
    strncpy( chance_event.news_date_str, info.date_str_julian(info.game_date), 55 );
    chance_event.news_date_str[55-1] = '\0';
}

//------- End of function NewsArray::open_game_end -----//

//------ Begin of function NewsArray::scenario_news -----//
//!
//! <int>
//!
void NewsArray::scenario_news(char type, char subType, int year) {
    News* newsPtr = add_news( NEWS_SCENARIO_NEWS, NEWS_NORMAL );

    if( !newsPtr )
	return;

    newsPtr->short_para1 = type;                    // i.e. PLAQUE_NEWS
    newsPtr->short_para2 = subType;                 // i.e. PLAQUE_GOLD
    newsPtr->short_para3 = year;                    // 1998

    newsPtr->set_newspaper_popup();
}

//------- End of function NewsArray::scenario_news -----//

//------ Begin of function NewsArray::add_news -----//
//!
//! Called by news processing function to set news parameters
//!
//! <int> newsId       = the id. of the news
//! <int> newsType     = news type
//! [int] nationRecno  = nation recno of the news
//! [int] nationRecno2 = recno of the 2nd nation related to the news
//! [int] forceAdd		 = add this news anyway, regardless of whether
//!								the nation has contact with the player or not
//!                      (default: 0)
//!
//! return : <News*> return the pointer of the News
//!						  NULL - the nation of the news does not have contact with the player
//!
News* NewsArray::add_news(int newsId, int newsType) {
    //playsound
    audio.play_wav("NEWS",audio.int_to_DsVolume(config.sound_effect_volume));

    //----------------------------------------------//

    static News news;

    news.id   = newsId;
    news.type = newsType;
    news.news_date = info.game_date;

    //--- if the news adding flag is turned off, don't add the news ---//

    if( news_add_flag ) {
	//--- if no. of news reaches max., delete the oldest one ---//

	if( size() >= MAX_NEWS ) {
	    start();
	    linkout();                                  // note: this is a packed DynArray!

	    if( last_clear_recno > 1 )
		last_clear_recno--;
	}

	//--------- link in a new news ---------//

	linkin(&news);

	return (News*) get();
    }
    else {
	return &news;
    }
}

//------- End of function NewsArray::add_news -----//
