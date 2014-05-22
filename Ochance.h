//Filename    : OCHANCE.H
//Description : Object Random Events
//Owner			: Fred

#ifndef __OCHANCE_H
#define __OCHANCE_H

#include <OTASK.H>

//------- Define ----------//

typedef void (*EventFP)();
typedef void (EventF)();

//!Structure for random events. See ChanceEvent.
struct EventStruct {
    EventFP event_func;
    short   odd_chance;                             // relative probabiltiy		//BUGHERE define for future only
};

#include <ONEWS.H>

//--------- Define class ChanceEvent ----------//

//!The Random Event module. Used by Info::next_day() to process daily
//!operations.
class ChanceEvent {
public:
    //---- vars for storing the effects of the chance events ----//

    int alumni_die_gift;
    int alumni_die_financial_year;
    int plagiarism_scandal_n;
    float yield_for_profit_competition;
    int   minority_student_percentage_falls_consYrs;
    float true_cost_education;
    float research_adjust_multiplier;
    int football_game_bonus;                      // $(000)
    int basketball_game_bonus;                    // $(000)
    int   scandal_in_alt_drop_gift;               //how much do scandal_in_alt eff. gifts.  //## chea 160699
    int   athlete_special_ban_year;               //how many year ban from athlete_scandal
    float capital_campaign_lock_ins_pect;         //## chea 230699 //for capital_campaign only
    int  research_prize_dept_recno;
    int  research_prize_faculty_recno;
    int research_prize_happen_date;               //## chea 250699
    int research_prize_happen_month;              //## chea 250699
    int research_prize_happen_year;               //## chea 250699
    int dept_technology_transfer;                 //## chea 260699
    //## chea 260699
    float dept_technology_transfer_research_amount;
    int dept_technology_transfer_start_year;      //## chea 260699
    int dept_technology_transfer_start_month;     //## chea 260699
    int research_prize_flag;                      //## chea 091199 4 chance only
    int profit_competition_end_date;              //## chea 091199 4 chance only
    int profit_competition_act;                   //## chea 091199 4 chance only
    int stock_dir;                                //## chea 221199 stock market dir
    char  court_limit_minority_flag;

    float operating_reserve_modifier;
    float gift_modifier;
    float gift_growth_rate_multiplier;
    int capital_campaign_end_year;

    // these two event has no news associated, thus not randomly generated
    // see OPSCH_PO.CPP PlayerSchool::think_protagonist
    float alumni_budget_increase;                 // percent increase in alumni budget (100 for 100%)
    int alumni_budget_financial_year;
    float athletics_budget_amount;                // amount added to next year athetcis budget
    int athletics_budget_financial_year;

    //---- vars that affect other vars with lasting effect ----//

    float extra_effect_staff_morale;
    float extra_effect_media_att;
    float extra_effect_alumni_morale;

    /*
      int	scandalous_fund_use;
      int   scan_fund_use_flag;  //## chea 091199 equql to 1 if scandalous_fund_use() call

      int	media_att_count_down;  //## chea 091199 count how many time do the media_att drop
      int	media_att_count_up;  //## chea 091199 count how many time do the media_att drop
      int	alumni_morale_drop;  //## chea 091199 count how many time to do the alumni_morale_drop
      int	alumni_morale_rise;  //## chea 091199 count how many time to do the alumni_morale_rise
    */
    int test_chance;                              //## chea021000 for testing chance card only

    //-------------------------------------------//

    char * news_str;
    char * date_str;
    char news_date_str[55];
    String news_news_str;

    int any_event_last_happen_date;               // any events.
    int event_last_happen_date[NEWS_ID_COUNT];    // keep track of specific events.

    int any_task_last_happen_date;
    int   task_last_happen_date[TASK_COUNT];

public:
    void  init();
    void  deinit();

    int write_file(File* filePtr);
    int read_file(File* filePtr);

    void  next_day();

    // these two event has no news associated, thus not randomly generated
    // see OPSCH_PO.CPP PlayerSchool::think_protagonist
    // percent increase in alumni budget (100 for 100%)
    void  signal_alumni_budget_increase(float percentInc, int financialYear);
    // amount added to next year athetcis budget
    void  signal_athletics_budget_amount(float amount, int financialYear);

private:
    void  process_chance_event();
    void  process_objective_event();

    void  update_chance_vars();
    void  process_individual_chance();
};

extern ChanceEvent chance_event;

//---------------------------------------//
#endif
