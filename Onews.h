//Filename    : ONEWS.H
//Description : Object News
//Owner			: Fred

#ifndef __ONEWS_H
#define __ONEWS_H

#ifndef __ODYNARR_H
#include <ODYNARR.H>
#endif

#include <OSTR.H>

//----- Maximum no. of news ---------//

enum { MAX_NEWS=50 };

//--------- Define constant --------------//

#define DISP_NEWS_DAYS  300                       // how long a news should stay on the screen before it disappears
#define DISP_NEWS_COUNT 5                         //## maximum no. of news message displayed on the screen at a time

#define NEWS_STR_PARA_LEN 20

//-------- Define news type ---------//

enum { NEWS_TYPE_NUM=1 };
enum { NEWS_NORMAL=0 };

enum { NEWS_WHO_NUM=1 };
enum {                                            //, NEWS_DISP_FRIENDLY, NEWS_DISP_PLAYER, NEWS_DISP_NONE };
    NEWS_DISP_ALL=0
};

//------- Define other constant -------//

enum {
    TASK_MSG_SUBTYPE_COUNT = 3,
    TASK_STARTED=0, TASK_EXPIRED, TASK_COMPLETED
};

//--------- Define news id. ----------//

enum {
    NEWS_ALUMNI_DIE = 1,

    NEWS_RAPE_IN_CAMPUS,

    NEWS_RETROFIT_FACILITIES,
    NEWS_EARTH_QUAKE,

    NEWS_CONGRESS_RESEARCH,
    NEWS_STOCK_MARKET,
    NEWS_SCANDALOUS_FUND_USE,
    NEWS_NEW_GOVENOR,
    NEWS_COURT_LIMIT_MINORITY,
    NEWS_RISKY_RESEARCH,                            //## chea 160699
    NEWS_SCANDAL_IN_ATHLETICS,                      //## chea 170699
    NEWS_RESEARCH_PRIZE,
    NEWS_TECHNOLOGY_TRANSFER,
    NEWS_CAPITAL_CAMPAIGN,
    NEWS_STUDENT_LOANS,

    NEWS_PLAGIARISM_SCANDAL,                        // ## chwg 180699
    NEWS_FOR_PROFIT_COMPETITION,                    // ## chwg 190699
    NEWS_MINORITY_STUDENT_PERCENTAGE_FALLS,         // ## chwg 210699
    NEWS_SUCCESSFUL_GRAD,                           // ## chwg 210699
    NEWS_MEDIA_OBTAIN_INFORMATION,                  // ## chwg 210699

    NEWS_ID_NEXT_OF_LAST_CHANCE,                    // must after chance news message
    // only chance card event
    NEWS_ID_COUNT_ONLY_CHANCE = NEWS_ID_NEXT_OF_LAST_CHANCE-NEWS_ALUMNI_DIE,

    // protagonist message
    NEWS_PROTAGONIST_TRUSTEE_EVALUATION = NEWS_ID_NEXT_OF_LAST_CHANCE,
    NEWS_PROTAGONIST_DEGREES_GRANTED,
    NEWS_PROTAGONIST_SPONSORED_PERFORMACE,
    NEWS_PROTAGONIST_INSTITUTIONAL_PRESTIGE,
    NEWS_PROTAGONIST_EDUCATIONAL_QUALITY,
    NEWS_PROTAGONIST_SCHOLARSHIP,
    NEWS_PROTAGONIST_STUDENT_DIVERSITY,
    NEWS_PROTAGONIST_FACULTY_DIVERSITY,
    NEWS_PROTAGONIST_ALUMNI_FIVE_YEARS,
    NEWS_PROTAGONIST_FACULTY_MORALE,

    NEWS_PROTAGONIST_STUDENT_MORALE,
    NEWS_PROTAGONIST_STAFF_MORALE,
    NEWS_PROTAGONIST_CURRENT_SURPLUS,
    NEWS_PROTAGONIST_SMOOTHED_SURPLUS,
    NEWS_PROTAGONIST_ENDOWMENT_PAYOUT,
    NEWS_PROTAGONIST_MAINTENANCE_BACKLOG,
    NEWS_PROTAGONIST_UG_SELECTIVITY,
    NEWS_PROTAGONIST_ALUMNI_MORALE,
    NEWS_PROTAGONIST_ALUMNI_DONATION,
    NEWS_PROTAGONIST_ENDOWMENT_MARKET_VALUE,

    NEWS_PROTAGONIST_CRIME_INDEX,
    NEWS_PROTAGONIST_RESEARCH_PERFORMANCE,
    NEWS_PROTAGONIST_SALARIES_INCREASE,
    NEWS_PROTAGONIST_SALARIES_GENDER_EQ,
    NEWS_PROTAGONIST_SALARIES_DEPT_EQ,
    NEWS_PROTAGONIST_SALARIES_RANK_EQ,
    NEWS_PROTAGONIST_TUITION_INCREASE,
    NEWS_PROTAGONIST_SPACE_SHORTFALL,
    NEWS_PROTAGONIST_FOOTBALL_PERFORMANCE,
    NEWS_PROTAGONIST_BASKETBALL_PERFORMANCE,

    NEWS_PROTAGONIST_DEPT_EDUCATIONAL_QUALITY,
    NEWS_PROTAGONIST_DEPT_PRESTIGE,
    NEWS_PROTAGONIST_COURSE_DENIALS,
    NEWS_PROTAGONIST_CLASS_SIZE_DEVIATION,

    // non-chance event
    NEWS_NEW_YEAR_START,
    NEWS_SCENARIO_NEAR_END,
    NEWS_SCENARIO_NEWS,
    NEWS_FOOTBALL_GAME,
    NEWS_BASKETBALL_GAME,

    NEWS_OPEN_GAME_NEAR_END,
    NEWS_OPEN_GAME_END,

    NEWS_BANKRUPTCY_LV1,
    NEWS_BANKRUPTCY_LV2,
    NEWS_BANKRUPTCY_LV3,

    NEWS_NEW_TASK,                                  // must be the last id

    NEWS_ID_NEXT_OF_LAST_NEWS,
    NEWS_ID_COUNT = NEWS_ID_NEXT_OF_LAST_NEWS-1,
};

//------------------------------------//
// Task array/list properties:
//
// - shown on both message (normal news) and objective sections
// - will be removed from the objective section after the task offer expires or the task is completed
// - checked monthly the success of the task; add bonus if task is completed
//

//------- Define struct News ---------//

//!Class which defines news events which can occur during the game with
//!routines for generating their associated messages. (i.e. n old
//!<alumnus/alumna> dies and leaves a bequest of $<bequest> thousand which
//!is transferred to Development Office.)
struct News {
public:
    char  id;                                     // id. of the news, NEWS_???
    char  task_id;                                // the corresponding task id, TASK_???, if this is a new task messsage

    char  type;                                   // news type   // type may be > NEWS_TYPE_NUM, for indicating that the news has been displayed in the stock window, do display it on the newspaper again
    char  news_type()     { return type%NEWS_TYPE_NUM; }
    char  task_msg_subtype;                       // = TASK_* above

    long  news_date;                              // date of the news

    //int   nation_name_id1;   // nation res. id of the nation that generate the news
    //int   nation_name_id2;   // if the news is related to two nations (e.g. one nation buys the stock of another nation)

    //char* u_name1();
    //char* player_name1();

    int short_para1;
    int short_para2;
    int short_para3;
    short short_para4;
    short short_para5;
    short short_para6;
    short short_para7;
    float float_para1;
    char  str_para1[NEWS_STR_PARA_LEN+1];

    static  String str;                           // for msg
    int   line_count;

public:
    char* msg();                                  // return the news msg

    void  task_msg();
    char* get_task_msg();

    //---- functions for return news string for msg() ----//

    void  alumni_die();
    void  rape_in_campus();
    void  retrofit_facilities();
    void  earth_quake();
    void  risky_research();                       //## chea 160699
    void  scandal_in_athletics();                 //## chea 170699
    void  research_prize();
    void  technology_transfer();
    void  capital_campaign();
    void  student_loans();

    void  plagiarism_scandal();                   // ## chwg 170699
    void  for_profit_competition();               // ## chwg 190699
    void  minority_student_percentage_falls();    // ## chwg 210699
    void  successful_grad();                      // ## chwg 210699
    void  media_obtain_information();             // ## chwg 210699

    void  congress_research();
    void  stock_market();
    void  scandalous_fund_use();
    void  new_governor();
    void  court_limit_minority();

    void  football_game();
    void  basketball_game();

    // protagonist message
    void  protagonist_message();                  // for all id beginning with NEWS_PROTAGONIST_
    void  bankruptcy_message_lv1();
    void  bankruptcy_message_lv2();
    void  bankruptcy_message_lv3();

    //-------------------------------//

    void  new_year_start();
    void  scenario_near_end();
    void  scenario_news();

    void  open_game_near_end();
    void  open_game_end();

    //---- news messages for tasks ----//

    void  new_task();

    void  news_rise_performance();
    void  news_educational_quality();             //## chea 220999
    void  news_student_morale();
    void  news_department_morale();
    void  news_rise_faculty_research_Per();
    void  news_rise_faculty_diversity_index();
    void  news_rise_use_of_IT();
    void  news_rise_staff_morale();

    //--------------------------------------------//
    // interface related section

    void  set_newspaper_popup();
    //int 	put(int y, int detectAction, int& newsHeight);
};

//-------- Define class NewsArray ----------//

class Font;

//!Collection class of all possible news events that have occurred during
//!the history of the game. Also defines interface for generating new news
//!events.
class NewsArray : public DynArray {
public:
    //------ display options ------//

    char  news_type_option[NEWS_TYPE_NUM];
    char  news_who_option;
    char  news_add_flag;

    int last_clear_recno;

    //--------------------------------/
    //##
public:
    NewsArray();

    void  init();                                 // called in ogame.cpp
    void  deinit();                               // called in ogame.cpp

    int write_file(File* filePtr);
    int read_file(File* filePtr);

    void  enable()        { news_add_flag = 1; }
    void  disable()   { news_add_flag = 0; }

    void  reset();
    void  default_setting();

    void  clear_news_disp();

    News* add_news(int newsId, int newsType=NEWS_NORMAL);
    void  remove(int newsId, int shortPara1);
    //##

    //------ functions for adding news -------//

    void  alumni_die(int gender, int bequest);
    void  rape_in_campus();
    void  retrofit_facilities(int dollarLost);
    void  earth_quake();
    void  risky_research(int moneyLost);          //## chea 160699
    void  scandal_in_athletics();                 //## chea 170699
    void  research_prize(int deptRecno, int facultyRecno);
    void  technology_transfer(int deptRecno, float transferAmount);
    void  capital_campaign();
    void  student_loans(int is_increase);         //## chea 230699

    // ## chwg 170699
    void  plagiarism_scandal(int dismissedStudCount);
    void  for_profit_competition();               // ## chwg 170699
    void  minority_student_percentage_falls();    // ## chwg 210699
    void  successful_grad();                      // ## chwg 210699
    // ## chwg 210699
    void  media_obtain_information(float trueCostEducation);

    void  congress_research(char direction);
    void  stock_market(char direction);
    void  scandalous_fund_use(int dollarLost);
    void  new_governor(char direction);
    void  court_limit_minority();

    // protagonist message
    // for all id beginning with NEWS_PROTAGONIST_
    void  protagonist_message(int protagonistId, int rating, int departmentRecno, int srcPara1, short srcPara2, short srcPara3, short srcPara4 );

    // bankruptcy message
    void  bankruptcy_message_lv1();
    void  bankruptcy_message_lv2( int monthsToBankruptcy );
    void  bankruptcy_message_lv3( int monthsToBankruptcy );

    void  new_year_start();
    void  scenario_near_end();
    void  scenario_news(char type, char subType=0, int year=0);
    void  football_game(int numWins);
    void  basketball_game(int numWins);

    void  open_game_near_end();
    void  open_game_end();

    void  new_task(News *taskPtr, char subtype=TASK_STARTED);

    //--------------------------------------------//

    News* operator[](int recNo);
};

extern NewsArray news_array;

//-------------------------------------------//
#endif
