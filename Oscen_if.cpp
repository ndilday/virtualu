#include <OSYS.H>
#include <OCONFIG.H>
#include <OSCEN_IF.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OPSCHOOL.H>
#include <ORESTXT.H>
#include <OVGA.H>
#include <ODATE.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OATHLETI.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OMOUSE2.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <ODEPT.H>
#include <OBUTTON.H>
#include <OENROLL.H>
#include <OVALSLDR.H>
#include <OFINANCE.H>

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-55
};

enum {
    BUTTON_X0 = REPORT_X1+100,
    BOTTOM_Y1 = REPORT_Y2+25,
};

enum {
    BOTTOM_BUTTON_WIDTH = 80,
    BOTTOM_BUTTON_HEIGHT = 25,
    BOTTOM_BUTTON_DISTANCE = 10,
};

enum {                                            // the bg image id
    BG_PIC_ID = 15
};

enum {
    SCENARIO_DESCRIPTION,
    SCENARIO_ACHIEVEMENTS,
    SCENARIO_EVALUATION
};

#define PLAQUE_GOLD_STR "Summa Cum Laude"
#define PLAQUE_SILVER_STR "Magna Cum Laude"
#define PLAQUE_BRONZE_STR "Cum Laude"

static char scenario_report_init_flag;
static ValueSlider scn_slider;
static int scn_val=1999;

//------- Begin of function ScenarioReport::ScenarioReport -----------//
//!
ScenarioReport::ScenarioReport() {
    init();
}

//--------- end of function ScenarioReport::ScenarioReport -----------//

//------- Begin of function ScenarioReport::~ScenarioReport -----------//
//!
ScenarioReport::~ScenarioReport() {
    deinit();
}

//--------- end of function ScenarioReport::~ScenarioReport -----------//

//------- Begin of function ScenarioReport::deinit ---------------//
//!
void ScenarioReport::deinit() {
    scn_slider.deinit();
    scenario_report_init_flag = 0;
}

//--------- end of function ScenarioReport::deinit ---------------//

//------- Begin of function ScenarioReport::init -----------------//
//!
void ScenarioReport::init() {
    scenario_report_init_flag = 0;
    scn_current_state = SCENARIO_ACHIEVEMENTS;
}

//--------- end of function ScenarioReport::init -----------------//

//---------- Begin of function ScenarioReport::eval_letter_report ------------//
//! You can set the value of ScenarioReport::topic_id before running this functions
void ScenarioReport::scenario_report_report(int refreshFlag) {
    int tab1=REPORT_X1+80,tab2=REPORT_X1+100,
	tab3=REPORT_X1+300,tab4=REPORT_X1+640;
    int vtab1=REPORT_Y1+40,vtab2=REPORT_Y1+90,
	vtab3=REPORT_Y1+255,vtab4=REPORT_Y1+350;
    vga.use_back();

    sys.is_institution_report=1;
    if (refreshFlag == INFO_REPAINT) {
	if(scenario_report_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!scenario_report_init_flag) {
	    user_interface.bg_img(BG_PIC_ID, &vga_back);
	    info.disp_column_bitmap(&vga_back);
	    scenario_report_init_flag=1;
	}
    }

    if(scn_current_state==SCENARIO_DESCRIPTION) {
	if (refreshFlag == INFO_REPAINT) {
	    String str;
	    str  = DIR_RES;
	    str += "SCENDESC";
	    ResTxt rs;
	    rs.init(str);

	    //	#ifdef DEBUG
	    //		if(player_school.scenario_id==0)player_school.scenario_id=6;  // for test ## chwg0225
	    //	#endif
	    int topic_id=rs.locate_topic(m.format(player_school.scenario_id,4));
	    char * textPtr = rs.get_body(topic_id);
	    font_charts.put(tab3,vtab1,"Scenario Description");
	    font_letter.put_paragraph(tab1,vtab2,tab4,vtab4,textPtr);
	}
    }

    if(scn_current_state==SCENARIO_ACHIEVEMENTS) {
	if (refreshFlag == INFO_REPAINT) {
	    font_charts.put(tab3-20,vtab1,"Scenario Achievements");

	    //-------------- SCN_BALANCE_BUDGET -----------------//

	    if(player_school.scenario_id==SCN_BALANCE_BUDGET) {
		String str;

		//--------- gold -------------//

		str= PLAQUE_GOLD_STR" objective : run surpluses of at least ";
		str+=m.format(player_school.scenario_target[0],2);
		str+=" thousands in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_GOLD],1);
		if(player_school.goal_deadline[PLAQUE_GOLD]>1)
		    str+=" years";
		else
		    str+=" year";
		str += " while maintaining a trustee evaluation of no less than ";
		str += player_school.scenario_base;
		str += ".";

		// #### begin Gilbert 09/07/2001 ####//
		// font_chartsm.put_paragraph(tab1,vtab2,tab4-100,vtab2+40,str);
		font_chartsm.put_paragraph(tab1,vtab2,tab4-50,vtab2+40,str);
		// #### end Gilbert 09/07/2001 ####//

		if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		    image_interface.put_back(tab4-110,vtab2-5,"ACHIEVED");

		//--------- silver -------------//

		str=PLAQUE_SILVER_STR" objective   : restore the operating reserve to its starting level of ";
		str+=m.format(player_school.scenario_target[1],2);
		str+=" thousands in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_SILVER],1);
		if(player_school.goal_deadline[PLAQUE_SILVER]>1)
		    str+=" years";
		else
		    str+=" year";
		str += " while maintaining a trustee evaluation of no less than ";
		str += player_school.scenario_base;
		str += ".";

		// #### begin Gilbert 09/07/2001 ####//
		// font_chartsm.put_paragraph(tab1,vtab2+40,tab4-100,vtab2+80,str);
		font_chartsm.put_paragraph(tab1,vtab2+40,tab4-50,vtab2+80,str);
		// #### end  Gilbert 09/07/2001 ####//
		if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		    image_interface.put_back(tab4-110,vtab2+35,"ACHIEVED");

		//---------- bronze --------------//

		str= PLAQUE_BRONZE_STR" objective : achieve a balanced budget in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_BRONZE],1);
		if(player_school.goal_deadline[PLAQUE_BRONZE]>1)
		    str+=" years";
		else
		    str+=" year";
		str += " while maintaining a trustee evaluation of no less than ";
		str += player_school.scenario_base;
		str += ".";

		// #### begin Gilbert 09/07/2001 ####//
		// font_chartsm.put_paragraph(tab1,vtab2+80,tab4-100,vtab2+120,str);
		font_chartsm.put_paragraph(tab1,vtab2+80,tab4-50,vtab2+120,str);
		// #### end Gilbert 09/07/2001 ####//
		if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		    image_interface.put_back(tab4-110,vtab2+75,"ACHIEVED");

		str="Current budget balance : ";
		str+=m.format(finance.this_year.surplus_deficit,2);
		font_chart_blue_sm.put(tab1,vtab2+130,str);

		str="Current operating reserve : ";
		str+=m.format(finance.this_year.asset_array[AC_OPERATING_RESERVE],2);
		font_chart_blue_sm.put(tab1,vtab2+150,str);

		str="Current trustee evaluation: ";
		str+=(int)player_school.scen_balance_budget_score();
		font_chart_blue_sm.put(tab1,vtab2+170,str);
	    }

	    //----------------- SCN_RAISE_SALARY ----------------//

	    if(player_school.scenario_id==SCN_RAISE_SALARY) {
		String str;
		str= PLAQUE_GOLD_STR" objective   : increase faculty salaries by ";
		str+=m.format(player_school.scenario_target[0],3);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_GOLD],1);
		if(player_school.goal_deadline[PLAQUE_GOLD]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2,tab4-100,vtab2+40,str);
		if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		    image_interface.put_back(tab4-110,vtab2-5,"ACHIEVED");

		str=PLAQUE_SILVER_STR" objective : increase faculty salaries by ";
		str+=m.format(player_school.scenario_target[1],3);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_SILVER],1);
		if(player_school.goal_deadline[PLAQUE_SILVER]>1)
		    str+=" years";
		else
		    str+=" year";
		font_chartsm.put_paragraph(tab1,vtab2+40,tab4-100,vtab2+80,str);
		if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		    image_interface.put_back(tab4-110,vtab2+35,"ACHIEVED");

		str= PLAQUE_BRONZE_STR" objective : increase faculty salaries by ";
		str+=m.format(player_school.scenario_target[2],3);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_BRONZE],1);
		if(player_school.goal_deadline[PLAQUE_BRONZE]>1)
		    str+=" years";
		else
		    str+=" year";
		font_chartsm.put_paragraph(tab1,vtab2+80,tab4-100,vtab2+120,str);
		if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		    image_interface.put_back(tab4-110,vtab2+75,"ACHIEVED");

		str="Percentage of faculty salary increase since the beginning of the game : ";
		str+=m.format(player_school.scenario_faculty_salary_increase,24);

		font_chart_blue_sm.put(tab1,vtab2+150,str);
	    }

	    //----------- SCN_INVEST_ACADEMIC ----------------//

	    if(player_school.scenario_id==SCN_INVEST_ACADEMIC) {
		String str;

		//------- Gold ---------//

		str  = PLAQUE_GOLD_STR" objective   : increase the weighted average prestige score of the listed departments by ";
		str += m.format(player_school.scenario_target[0],3);
		str += " points over ";
		str += m.format(player_school.goal_deadline[PLAQUE_GOLD],1);
		if(player_school.goal_deadline[PLAQUE_GOLD]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2,tab4-100,vtab2+40,str);
		if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		    image_interface.put_back(tab4-110,vtab2-5,"ACHIEVED");

		//-------- Silver ----------//

		str  = PLAQUE_SILVER_STR" objective   : increase the weighted average prestige score of the listed departments by ";
		str += m.format(player_school.scenario_target[1],3);
		str += " points over ";
		str += m.format(player_school.goal_deadline[PLAQUE_SILVER],1);
		if(player_school.goal_deadline[PLAQUE_SILVER]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2+40,tab4-100,vtab2+80,str);
		if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		    image_interface.put_back(tab4-110,vtab2+35,"ACHIEVED");

		//--------- Bronze -----------//

		str  = PLAQUE_BRONZE_STR" objective   : increase the weighted average prestige score of the listed departments by ";
		str += m.format(player_school.scenario_target[2],3);
		str += " points over ";
		str += m.format(player_school.goal_deadline[2],1);
		if(player_school.goal_deadline[2]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2+80,tab4-100,vtab2+120,str);
		if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		    image_interface.put_back(tab4-110,vtab2+75,"ACHIEVED");

		//------- current value -----------//

		int tryer=player_school.scenario_invest_academic_get_cur()-player_school.scenario_base;

		str="Current weighted average increase (departments are weighted by the number of students): ";
		str+=m.format(tryer,3);
		font_chartsm.put_paragraph(tab1,vtab2+135,tab4-105,vtab2+175,str);
	    }

	    //---------------- SCN_TEACHING_QUALITY -----------------//

	    if(player_school.scenario_id==SCN_TEACHING_QUALITY) {
		String str;
		str= PLAQUE_GOLD_STR" objective   : increase educational quality from ";
		str+=m.format(player_school.scenario_base,1);
		str+=" to ";
		str+=m.format(player_school.scenario_target[0],1);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_GOLD],1);
		if(player_school.goal_deadline[PLAQUE_GOLD]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2,tab4-100,vtab2+40,str);
		if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		    image_interface.put_back(tab4-110,vtab2-5,"ACHIEVED");

		str= PLAQUE_SILVER_STR" objective  : increase educational quality from ";
		str+=m.format(player_school.scenario_base,1);
		str+=" to ";
		str+=m.format(player_school.scenario_target[1],1);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_SILVER],1);
		if(player_school.goal_deadline[PLAQUE_SILVER]>1)
		    str+=" years";
		else
		    str+=" year";
		font_chartsm.put_paragraph(tab1,vtab2+40,tab4-100,vtab2+80,str);
		if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		    image_interface.put_back(tab4-110,vtab2+35,"ACHIEVED");

		str= PLAQUE_BRONZE_STR" objective : increase educational quality from ";
		str+=m.format(player_school.scenario_base,1);
		str+=" to ";
		str+=m.format(player_school.scenario_target[2],1);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_BRONZE],1);
		if(player_school.goal_deadline[PLAQUE_BRONZE]>1)
		    str+=" years";
		else
		    str+=" year";
		font_chartsm.put_paragraph(tab1,vtab2+80,tab4-100,vtab2+120,str);
		if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		    image_interface.put_back(tab4-110,vtab2+75,"ACHIEVED");

		str="Current educational quality : ";
		str+=m.format(player_school.sub_score[S_EDUCATIONAL_QUALITY][THIS_MONTH],1);
		font_chart_blue_sm.put(tab1,vtab2+150,str);
	    }

	    //------------------ SCN_RESEARCH_QUALITY -----------------//

	    if(player_school.scenario_id==SCN_RESEARCH_QUALITY) {
		String str;
		str=PLAQUE_GOLD_STR" objective   : increase the value of 'Scholarship, broadly defined' performance from ";
		str+=m.format(player_school.scenario_base,1);
		str+=" to ";
		str+=m.format(player_school.scenario_target[0],1);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_GOLD],1);
		if(player_school.goal_deadline[PLAQUE_GOLD]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2,tab4-100,vtab2+40,str);
		if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		    image_interface.put_back(tab4-110,vtab2-5,"ACHIEVED");

		str= PLAQUE_SILVER_STR" objective   : increase the value of 'Scholarship, broadly defined' performance from ";
		str+=m.format(player_school.scenario_base,1);
		str+=" to ";
		str+=m.format(player_school.scenario_target[1],1);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_SILVER],1);
		if(player_school.goal_deadline[PLAQUE_SILVER]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2+40,tab4-100,vtab2+80,str);
		if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		    image_interface.put_back(tab4-110,vtab2+35,"ACHIEVED");

		str= PLAQUE_BRONZE_STR" objective   : increase the value of 'Scholarship, broadly defined' performance from ";
		str+=m.format(player_school.scenario_base,1);
		str+=" to ";
		str+=m.format(player_school.scenario_target[2],1);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_BRONZE],1);
		if(player_school.goal_deadline[PLAQUE_BRONZE]>1)
		    str+=" years";
		else
		    str+=" year";
		font_chartsm.put_paragraph(tab1,vtab2+80,tab4-100,vtab2+120,str);
		if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		    image_interface.put_back(tab4-110,vtab2+75,"ACHIEVED");

		str="Scholarship, broadly defined : ";
		str+=m.format(player_school.sub_score[S_SCHOLARSHIP_BROADLY_DEFINED][THIS_MONTH],1);
		font_chart_blue_sm.put(tab1,vtab2+150,str);
	    }

	    //--------------- SCN_WINNING_ATHLETICS -----------------//

	    if(player_school.scenario_id==SCN_WINNING_ATHLETICS) {
		String str;

		//------------- Gold ---------------//

		str=PLAQUE_GOLD_STR" objective   : both athletics teams winning more than ";
		str+=m.format(player_school.scenario_target[0],3);
		str+=" of their games at a higher NCAA level in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_GOLD],1);
		if(player_school.goal_deadline[PLAQUE_GOLD]>1)
		    str+=" years";
		else
		    str+=" year";
		font_chartsm.put_paragraph(tab1,vtab2,tab4-100,vtab2+40,str);
		if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		    image_interface.put_back(tab4-110,vtab2-5,"ACHIEVED");

		//------------- Silver --------------//

		str= PLAQUE_SILVER_STR" objective   : both athletics teams winning more than ";
		str+=m.format(player_school.scenario_target[1],3);
		str+=" of their games in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_SILVER],1);
		if(player_school.goal_deadline[PLAQUE_SILVER]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2+40,tab4-100,vtab2+80,str);
		if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		    image_interface.put_back(tab4-110,vtab2+35,"ACHIEVED");

		//------------ Bronze -------------//

		str= PLAQUE_BRONZE_STR" objective   : both athletics teams winning more than ";
		str+=m.format(player_school.scenario_target[2],3);
		str+=" of their games in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_BRONZE],1);
		if(player_school.goal_deadline[PLAQUE_BRONZE]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2+80,tab4-100,vtab2+120,str);
		if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		    image_interface.put_back(tab4-110,vtab2+75,"ACHIEVED");

		//--------- display current value ------------//

		str="Football team wins : ";
		str+=m.format(athletics_office.football_percent_win*100,3);
		font_chart_blue_sm.put(tab1,vtab2+130,str);

		str="Basketball team wins : ";
		str+=m.format(athletics_office.basketball_percent_win*100,3);
		font_chart_blue_sm.put(tab1,vtab2+150,str);

		str="Level of competition has been increased? ";
		if( athletics_office.ncaa_level_input == INPUT_HIGH )
		    str += "Yes";
		else
		    str += "No";
		font_chart_blue_sm.put(tab1,vtab2+170,str);
	    }

	    //--------------- SCN_LIMIT_TUITION_GROWTH ----------------//

	    if(player_school.scenario_id==SCN_LIMIT_TUITION_GROWTH) {
		String str;

		//------------- Gold ---------------//

		str  = PLAQUE_GOLD_STR" objective   : reduce the tuition rate by ";
		str += m.format( player_school.scenario_target[0], 3 );
		str += " in ";
		str += player_school.goal_deadline[0];
		str += " years while maintaining a trustee evaluation of no less than ";
		str += player_school.scenario_target[1];
		str += " at the end of the period.";

		//## chea 270799
		font_chartsm.put_paragraph(tab1,vtab2,tab4-100,vtab2+39,str);
		if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		    image_interface.put_back(tab4-100,vtab2-5,"ACHIEVED");

		//------------- Silver -------------//

		str  = PLAQUE_SILVER_STR" objective   : reduce the tuition rate by ";
		str += m.format( player_school.scenario_target[0], 3 );
		str += " in ";
		str += player_school.goal_deadline[1];
		str += " years while maintaining a trustee evaluation of no less than ";
		str += player_school.scenario_target[1];
		str += " at the end of the period.";

		//## chea 270799
		font_chartsm.put_paragraph(tab1,vtab2+40,tab4-100,vtab2+79,str);
		if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		    image_interface.put_back(tab4-100,vtab2+35,"ACHIEVED");

		//------------- Bronze -------------//

		str  = PLAQUE_BRONZE_STR" objective   : reduce the tuition rate by ";
		str += m.format( player_school.scenario_target[0], 3 );
		str += " in ";
		str += player_school.goal_deadline[2];
		str += " years while maintaining a trustee evaluation of no less than ";
		str += player_school.scenario_target[1];
		str += " at the end of the period.";

		//## chea 270799
		font_chartsm.put_paragraph(tab1,vtab2+80,tab4-100,vtab2+119,str);
		if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		    image_interface.put_back(tab4-100,vtab2+75,"ACHIEVED");

		//----- display current value --------//

		str  = "Reduction in the tuition rate : ";
		str += m.format( 100.0f * (1.0f - finance.tuition_rate  / player_school.scenario_float_base), 24);
		font_chart_blue_sm.put(tab1,vtab2+130,str);

		str  = "Current trustee evaluation : ";
		str += m.format( player_school.cur_game_score, 1);
		font_chart_blue_sm.put(tab1,vtab2+150,str);
	    }

	    //--------------- SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS -------------//

	    if(player_school.scenario_id==SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS) {
		String str;

		//--------- Gold ----------//

		str=PLAQUE_GOLD_STR" objective : increase educational quality from ";
		str+=m.format(player_school.scenario_base,1);
		str+=" to ";
		str+=m.format(player_school.scenario_target[0],1);
		str+=" in ";
		str+=player_school.goal_deadline[PLAQUE_GOLD];
		if(player_school.goal_deadline[PLAQUE_GOLD]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put(tab1,vtab2,str);
		if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		    image_interface.put_back(tab4-110,vtab2-5,"ACHIEVED");

		//--------- Silver ----------//

		str= PLAQUE_SILVER_STR" objective : increase educational quality from ";
		str+=m.format(player_school.scenario_base,1);
		str+=" to ";
		str+=m.format(player_school.scenario_target[1],1);
		str+=" in ";
		str+=player_school.goal_deadline[PLAQUE_SILVER];
		if(player_school.goal_deadline[PLAQUE_SILVER]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2+40,tab4-100,vtab2+80,str);
		if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		    image_interface.put_back(tab4-110,vtab2+35,"ACHIEVED");

		//--------- Bronze ----------//

		str= PLAQUE_BRONZE_STR" objective : increase educational quality from ";
		str+=m.format(player_school.scenario_base,1);
		str+=" to ";
		str+=m.format(player_school.scenario_target[2],1);
		str+=" in ";
		str+=player_school.goal_deadline[PLAQUE_BRONZE];
		if(player_school.goal_deadline[PLAQUE_BRONZE]>1)
		    str+=" years";
		else
		    str+=" year";

		font_chartsm.put_paragraph(tab1,vtab2+80,tab4-100,vtab2+120,str);
		if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		    image_interface.put_back(tab4-110,vtab2+75,"ACHIEVED");

		//------- Current value ------------//

		str="Current educational quality : ";
		str+=m.format(player_school.sub_score[S_EDUCATIONAL_QUALITY][THIS_MONTH],1);
		font_chart_blue_sm.put(tab1,vtab2+155,str);
	    }

	    //--------------- SCN_STUDENT_DIVERSITY -------------------//

	    if(player_school.scenario_id==SCN_STUDENT_DIVERSITY) {
		String str;
		str=PLAQUE_GOLD_STR" objective   : increase the percent of minority students from ";
		str+=m.format(player_school.scenario_base,3);
		str+=" to ";
		str+=m.format(player_school.scenario_target[0],3);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_GOLD],1);
		if(player_school.goal_deadline[PLAQUE_GOLD]>1)
		    str+=" years";
		else
		    str+=" year";
		font_chartsm.put(tab1,vtab2+10,str);
		if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		    image_interface.put_back(tab4-100,vtab2,"ACHIEVED");

		str= PLAQUE_SILVER_STR" objective : increase the percent of minority students from ";
		str+=m.format(player_school.scenario_base,3);
		str+=" to ";
		str+=m.format(player_school.scenario_target[1],3);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_SILVER],1);
		if(player_school.goal_deadline[PLAQUE_SILVER]>1)
		    str+=" years";
		else
		    str+=" year";
		font_chartsm.put(tab1,vtab2+35,str);
		if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		    image_interface.put_back(tab4-100,vtab2+35,"ACHIEVED");

		str= PLAQUE_BRONZE_STR" objective : increase the percent of minority students from ";
		str+=m.format(player_school.scenario_base,3);
		str+=" to ";
		str+=m.format(player_school.scenario_target[2],3);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_BRONZE],1);
		if(player_school.goal_deadline[PLAQUE_BRONZE]>1)
		    str+=" years";
		else
		    str+=" year";
		font_chartsm.put(tab1,vtab2+60,str);
		if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		    image_interface.put_back(tab4-100,vtab2+70,"ACHIEVED");

		str="Current percent of minority students : ";
		str+=m.format(enroll_res.overall_percent_minority,3);
		font_chart_blue_sm.put(tab1,vtab2+90,str);

		//				str="Current operating reserve : ";
		//				str+=m.format(123,2);
		//				font_chartsm.put(tab1,vtab2+110,str);
	    }

	    //			SCN_FACULTY_DIVERSITY,
	    if(player_school.scenario_id==SCN_FACULTY_DIVERSITY) {

		int totalFacCount = department_array.faculty_gender_history[GENDER_ETHNIC_TYPE_COUNT][THIS_YEAR];
		int valueA = ( department_array.faculty_gender_history[MINORITY_MALE][THIS_YEAR] + department_array.faculty_gender_history[MINORITY_FEMALE][THIS_YEAR]) * 100 / totalFacCount;
		int valueB = ( department_array.faculty_gender_history[MINORITY_FEMALE][THIS_YEAR] + department_array.faculty_gender_history[NONMINORITY_FEMALE][THIS_YEAR]) * 100 / totalFacCount;

		String str;
		str= PLAQUE_GOLD_STR" objective   : increase minority faculty percentage from ";
		//str+=m.format(player_school.scenario_target[0]/1.5f,3); //## chea 280799
		str+=m.format(player_school.scenario_base1,3);
		str+=" to ";
		str+=m.format(player_school.scenario_target[0],3);
		str+=" and ";
		str+=" increase female faculty percentage from ";
		//str+=m.format(player_school.scenario_target[1]/1.25f,3); //## chea 280799
		str+=m.format(player_school.scenario_base2,3);
		str+=" to ";
		str+=m.format(player_school.scenario_target[1],3);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_GOLD],1);
		if(player_school.goal_deadline[PLAQUE_GOLD]>1)
		    str+=" years";
		else
		    str+=" year";
		// #### begin Gilbert 07/09/2001 #####//
		// font_chartsm.put_paragraph(tab1,vtab2-15,tab4-150,vtab2+25,str);
		font_chartsm.put_paragraph(tab1,vtab2-15,tab4-50,vtab2+25,str);
		// #### end Gilbert 07/09/2001 #####//
		if(player_school.plaque_granted[PLAQUE_GOLD]==1)
		    image_interface.put_back(tab4-100,vtab2-25,"ACHIEVED");

		str= PLAQUE_SILVER_STR" objective : increase minority faculty percentage from ";
		//str+=m.format(player_school.scenario_target[0]/1.5f,3); //## chea 280799
		str+=m.format(player_school.scenario_base1,3);
		str+=" to ";
		str+=m.format(player_school.scenario_target[0],3);
		str+=" and ";
		str+=" increase female faculty percentage from ";
		//str+=m.format(player_school.scenario_target[1]/1.25f,3); //## chea 280799
		str+=m.format(player_school.scenario_base2,3);
		str+=" to ";
		str+=m.format(player_school.scenario_target[1],3);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_SILVER],1);
		if(player_school.goal_deadline[PLAQUE_SILVER]>1)
		    str+=" years";
		else
		    str+=" year";
		// #### begin Gilbert 07/09/2001 #####//
		// font_chartsm.put_paragraph(tab1,vtab2+25,tab4-150,vtab2+65,str);
		font_chartsm.put_paragraph(tab1,vtab2+25,tab4-50,vtab2+65,str);
		// #### begin Gilbert 07/09/2001 #####//
		if(player_school.plaque_granted[PLAQUE_SILVER]==1)
		    image_interface.put_back(tab4-100,vtab2+25,"ACHIEVED");

		str= PLAQUE_BRONZE_STR" objective : increase minority faculty percentage from ";
		//str+=m.format(player_school.scenario_target[0]/8.0f,3); //## chea 280799
		str+=m.format(player_school.scenario_base1,3);
		str+=" to ";
		str+=m.format(player_school.scenario_target[0],3);
		str+=" and ";
		str+=" increase female faculty percentage from ";
		//str+=m.format(player_school.scenario_target[1]/1.25f,3); //## chea 280799
		str+=m.format(player_school.scenario_base2,3);
		str+=" to ";
		str+=m.format(player_school.scenario_target[1],3);
		str+=" in ";
		str+=m.format(player_school.goal_deadline[PLAQUE_BRONZE],1);
		if(player_school.goal_deadline[PLAQUE_BRONZE]>1)
		    str+=" years";
		else
		    str+=" year";
		// #### begin Gilbert 07/09/2001 #####//
		// font_chartsm.put_paragraph(tab1,vtab2+65,tab4-150,vtab2+105,str);
		font_chartsm.put_paragraph(tab1,vtab2+65,tab4-50,vtab2+105,str);
		// #### end Gilbert 07/09/2001 #####//
		if(player_school.plaque_granted[PLAQUE_BRONZE]==1)
		    image_interface.put_back(tab4-100,vtab2+75,"ACHIEVED");

		str="Current minority faculty percentage : ";
		str+=m.format(valueA,3);
		font_chart_blue_sm.put(tab1,vtab2+115,str);

		str="Current female faculty percentage : ";
		str+=m.format(valueB,3);
		font_chart_blue_sm.put(tab1,vtab2+135,str);

		//				str="Current operating reserve : ";
		//				str+=m.format(123,2);
		//				font_chartsm.put(tab1,vtab2+110,str);
	    }

	    font_chartsm.put(tab1,vtab3+48,m.format(date.year(info.game_start_date),16));
	    font_chartsm.put(tab1,vtab3+65,"Game started");

	    scn_slider.init(tab1+50,vtab3+25,400,18,
			    info.game_start_date,
			    date.julian( date.year(info.game_start_date)+player_school.scenario_max_game_years, date.month(info.game_start_date), date.day(info.game_start_date)),
			    &(info.game_date), 1, V_YELLOW, 0);       // suppress display of years

        String str;
        str="You have played ";

        int tmp=info.game_year - date.year(info.game_start_date);
        if ( info.game_month < date.month(info.game_start_date) )
	    tmp--;

	str+=tmp;
	if(tmp>1)
            str+=" years";
	else
	    str+=" year";
        font_chartsm.put(tab1+200,vtab3+55,str);
	font_chartsm.put(tab4-100,vtab3+48,m.format((int)(date.year(info.game_start_date)+player_school.scenario_max_game_years),16));
	font_chartsm.put(tab4-100,vtab3+65,"Game end");
	str="Total score : ";
	str+=player_school.ultimate_game_score;
	str+="(current score) + ";
	str+=player_school.bonus_point;
	str+="(scenario bonus)";
	font_chartsm.put(tab1,vtab3+95,str);
	str="=";
	str+=player_school.ultimate_game_score+player_school.bonus_point;
	font_chartsm.put(tab1,vtab3+115,str);
	scn_slider.refresh();

	// put year passed here, instead of by scn_slider itself
	scn_slider.font_ptr->center_put(scn_slider.value_slider_x1, scn_slider.value_slider_y1, scn_slider.value_slider_x2, scn_slider.value_slider_y2,
					info.date_str_julian(info.game_date, INFO_DATE_DEFAULT|INFO_DATE_NO_MONTH|INFO_DATE_NO_DAY) );
    }
}

//------- blt the back buffer to front buffer -------//
vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//---------- end of function Development::eval_letter_report ------------//

////////////////////////////////////////////////////////////////////////////////////////////

int ScenarioReport::scenario_report_detect() {
int tab1=REPORT_X1,tab2=REPORT_X1+100,
    tab3=REPORT_X1+200,tab4=REPORT_X1+600;
int vtab1=REPORT_Y1+15,vtab2=REPORT_Y1+70,
    vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

if (!scenario_report_init_flag)
    return 0;
if (mouse.single_click(200,50,400,140)) {
scn_current_state=SCENARIO_ACHIEVEMENTS;
sys.redraw_all_flag=1;
return 1;
}
if (mouse.single_click(400,50,600,140)) {
scn_current_state=SCENARIO_DESCRIPTION;
sys.redraw_all_flag=1;
return 1;
}

if(mouse.single_click(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2,2)) {
sys.set_staying_view_mode(0);
sys.set_view_mode(MODE_NORMAL);
return 1;
}

// for mail button
if(mouse.single_click(8, 562, 76, 599, 2)
    ||
(m.upper(mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_UNIQUE_KEY))=='W')) {
sys.set_staying_view_mode(0);
sys.set_view_mode(MODE_NORMAL);
return 1;
}
return 0;
}
