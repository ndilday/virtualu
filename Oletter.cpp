#include <OSYS.H>
#include <OCONFIG.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OGSET.H>
#include <OVGA.H>
#include <OMOUSE2.H>
#include <COLOR.H>
#include <OFINANCE.H>
#include <OPSCHOOL.H>
#include <OFONT.H>
#include <ODATE.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OLETTER.H>
#include <OVBROW98.H>
#include <OIFACE.H>
#include <OFACULTY.H>
#include <ODEPT.H>
#include <ONAMERES.H>
#include <ODEPTRES.H>
#include <OENROLL.H>
#include <OSTUOFF.H>
#include <OSCHLRES.H>
#include <OPEERSCH.H>
#include <OATHLETI.H>
#include <OINFO.H>
#include <OBUTTON.H>
#include <OMATH.H>
#include <OLOSEGAM.H>

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-55
};

enum {
    BOTTOM_BUTTON_X0 = REPORT_X1+70,
    BOTTOM_BUTTON_X1 = REPORT_X1+170,
    BOTTOM_BUTTON_Y1 = REPORT_Y2+15,
    BOTTOM_BUTTON_X2 = REPORT_X2-20,
    BOTTOM_BUTTON_Y2 = REPORT_Y2+40
};

enum {
    BOTTOM_BUTTON_WIDTH = 125,
    BOTTOM_BUTTON_HEIGHT = 25,
    HELP_BUTTON_COUNT = 3,
    BOTTOM_BUTTON_DISTANCE
    = (BOTTOM_BUTTON_X2-BOTTOM_BUTTON_X1-BOTTOM_BUTTON_WIDTH*2)/3
};

enum {                                            // the bg image id
    BG_PIC_ID = 6
};

static float protagonist_ck1 = 3.50f;
static float protagonist_ck2 = 1.75f;
static float protagonist_ck3 = 1.50f;

static short *bottom_button_up_bitmap[HELP_BUTTON_COUNT];
static short *bottom_button_down_bitmap[HELP_BUTTON_COUNT];
static Button button_return;
static Button button_back;
static Button button_pgup;
static Button button_pgdn;
static int letter_report_init_flag;
static void welcome_letter_disp_rep_row(int,int,int,int);
static int welcome_letter_detect_rep_row(int,int,int);
static void eval_letter_disp_rep_row(int,int,int,int);
static int eval_letter_detect_rep_row(int,int,int);

static VBrowse98 letter_browser;
static int name_id1,name_id2,name_id3;
static int staff2_name_id1,staff2_name_id2,staff2_name_id3;
static double surplusDeficit;

static int old_scen_page_number=-1;
static int max_scen_page_number=2;

static int old_welcome_page_number=-1;
static int max_welcome_page_number=5;

static int old_eval_page_number=-1;
static int max_eval_page_number=4;

static int old_state_page_number=-1;
static int max_state_page_number=3;

//--------- Define sort_data -----------//
//!Structure storing raw performance data that is sorted by the Letter
//!class and used to generate evaluation letters.
struct SortData {
    int idx;
    float strength;
    float consistency;
    float k1;
    char tag;
};

static struct SortData sort_data[SCORE_VAR_COUNT];

//---------- define text ----------------------//
static char *area_str[SCORE_VAR_COUNT]= {
    "Degrees granted                       ",
    "Faculty research performance          ",
    "Institutional prestige					  ",
    "Educational quality                  ",
    "Scholarship broadly defined          ",        // 1007
    "Student diversity                    ",
    "Faculty diversity                    ",
    "Alumni giving                 ",
    "Faculty morale                       ",
    "Student morale                       ",
    "Staff morale                         ",
    "Current surplus deficit percent      ",
    "Smoothed surplus deficit percent     ",
    "Endowment payout rate relative       ",        // 1007
    "Deferred maintenance backlog percent ",
};

//------- Begin of function Letter::Letter -----------//
//!
Letter::Letter() {
    //	init();
}

//--------- End of function Letter::Letter -----------//

//------- Begin of function Letter::~Letter -----------//
//!
Letter::~Letter() {
    deinit();
}

//--------- End of function Letter::~Letter -----------//

//------- Begin of function Letter::deinit ---------------//
//!
void Letter::deinit() {
    //--------- deallocate memory -------------//
    for (int i = 0; i < HELP_BUTTON_COUNT; i++) {
	if (bottom_button_up_bitmap[i]) {
	    mem_del(bottom_button_up_bitmap[i]);
	    mem_del(bottom_button_down_bitmap[i]);
	    bottom_button_up_bitmap[i] = NULL;
	    bottom_button_down_bitmap[i] = NULL;
	}
    }
    letter_report_init_flag = 0;
}

//--------- End of function Letter::deinit ---------------//

//------- Begin of function Letter::init -----------------//
//!
void Letter::init() {
    memset(this,0,sizeof(Letter));
    name_id1=m.random(name_res.first_name_count)+1;
    name_id2=m.random(name_res.middle_name_count)+1;
    name_id3=m.random(name_res.last_name_count)+1;
    staff2_name_id1=m.random(name_res.first_name_count);
    staff2_name_id2=m.random(name_res.middle_name_count);
    staff2_name_id3=m.random(name_res.last_name_count);
    eval_page_number=1;
    welcome_page_number=1;
    scen_page_number=1;
    state_page_number=1;
    for (int i = 0; i < HELP_BUTTON_COUNT; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }
    letter_report_init_flag = 0;
}

//--------- End of function Letter::init -----------------//

/*
enum{
  WELCOME_LETTER_PART1_BEGIN=1,
  WELCOME_LETTER_PART1_END=14,
  WELCOME_LETTER_PART2_BEGIN=15,
};

static void welcome_letter_disp_rep_row(int recno, int x, int y,int refreshFlag)
{
};

static int welcome_letter_detect_rep_row(int recno, int x, int y)
{
return 0;
};

static void eval_letter_disp_rep_row(int recno, int x, int y,int refreshFlag)
{
static char * eval_context[]={
"",
"",
"To   : ",
"          President, ",
"From:",
"          Chair, Board of Trustees",
"Date :   September 1,",
"",
"Dear President",
"        The Compensation Committee of the Board has completed your performance evaluation",

"for      -      . The Committee believes your performance has been weak. This",
"will be reflected in your salary for next year.",
"",

"The Committee judges the degree of improvement or degradation in each of the fifteen performance",
"areas listed below. While these do not provide a complete view of the University, we believe they",
"cover the most important areas. In many cases, the absolute level of the criterion is less",
"important than the trend. With this in mind, 8.0f estimated the percent increase or decrease",
"over last year's score and arrayed the indicators accordingly.",
"",
"The Committee and full Board would appreciate your continuing attention to these matters.",
"We would like to see positive trends for all the items during the next year.",

"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
};

int tab1=x+5;
int tab2=x+20;
String str;
String str2;

str=eval_context[recno];
font_san.put(tab2,y+2,str);

if(recno==2)
{
str2=player_school.player_first_name;
str2+=" ";
str2+=player_school.player_last_name;
font_blue_san.put(tab2+48,y+2,str2);
}
if(recno==3)
{
str2=player_school.school_name;
font_blue_san.put(tab2+120,y+2,str2);
}
if(recno==4)
{
str2=name_res.get_name_str(name_id1,name_id2,name_id3);
font_blue_san.put(tab2+48,y+2,str2);
}
if(recno==6)
{
if(info.game_month<9)
str2=m.format(info.game_year-1,16);
else
str2=m.format(info.game_year,16);
font_blue_san.put(tab2+150,y+2,str2);
}
if(recno==8)
{
str2=player_school.player_last_name;
str2+=" ,";
font_blue_san.put(tab2+100,y+2,str2);
}
if(recno==10)
{
if(info.game_month<9)
str2=m.format(info.game_year-2,16);
else
str2=m.format(info.game_year-1,16);
font_blue_san.put(tab2+20,y+2,str2);
if(info.game_month<9)
str2=m.format(info.game_year-1,16);
else
str2=m.format(info.game_year,16);
font_blue_san.put(tab2+60,y+2,str2);
}
if(recno==12)
{
if(finance.last_year.asset_array[AC_OPERATING_RESERVE] < 0)
{
str2="        Last year the University's operating reserve was in the red by ";
str2+=m.format(-finance.last_year.asset_array[AC_OPERATING_RESERVE],2);
str2+=" thousand.";
font_red_san.put(tab2,y+2,str2);
}
if(finance.last_year.asset_array[AC_OPERATING_RESERVE] > 0)
{
str2="        Last year the University's operating reserve was in the blue by ";
str2+=m.format(finance.last_year.asset_array[AC_OPERATING_RESERVE],2);
str2+=" thousand.";
font_blue_san.put(tab2,y+2,str2);
}
}
};

static int eval_letter_detect_rep_row(int recno, int x, int y)
{
return 0;
};
*/

///////////////////////////////////////////////////
enum{TMP_SCENARIO_COUNT = 10};
static int max_scn_page[TMP_SCENARIO_COUNT] = {
    1,3,3,3,3,3,3,2,3,3
};
static int scn_id[TMP_SCENARIO_COUNT] = {
    SCN_RAISE_SALARY,
    SCN_INVEST_ACADEMIC,
    SCN_TEACHING_QUALITY,                           //## fred0301
    SCN_RESEARCH_QUALITY,                           //## fred0301
    SCN_WINNING_ATHLETICS,
    SCN_LIMIT_TUITION_GROWTH,
    SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS,           //## fred0301
    SCN_STUDENT_DIVERSITY,
    SCN_FACULTY_DIVERSITY,
    SCN_BALANCE_BUDGET,
};
static char* scn_name[TMP_SCENARIO_COUNT] = {
    "Scenario 1  -- PAY BETTER",
    "Scenario 2  -- ALLOCATE NEW MONEY",
    "Scenario 3  -- TEACH BETTER",
    "Scenario 4  -- IMPROVE RESEARCH PERFORMANCE",
    "Scenario 5  -- WIN GAMES",
    "Scenario 6  -- REDUCE TUITION",
    "Scenario 7  -- RESPOND TO ENROLLMENT SHIFTS",
    "Scenario 8  -- ENROLL MORE MINORITY STUDENTS",
    "Scenario 9  -- HIRE MORE MINORITY FACULTY MEMBERS",
    "Scenario 10 -- BALANCE THE BUDGET",
};

//////////////////////////////////////////////////////

void Letter::scen_welcome_letter_report(int refreshFlag) {
    int tab1=REPORT_X1,tab2=REPORT_X1+70,
	tab3=REPORT_X1+100,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+20,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	if(letter_report_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!letter_report_init_flag) {
	    sys.set_speed(0);
	    max_scen_page_number=max_scn_page[player_school.scenario_id-1];
	    user_interface.bg_img(BG_PIC_ID, &vga_back);
	    letter_date = info.game_start_date;
	    font_letter.center_put(REPORT_X1+50,REPORT_Y1,REPORT_X1+680,REPORT_Y1+20,"Scenario Welcome Letter ");
	    font_letter.center_put(REPORT_X1+50,REPORT_Y1+30,REPORT_X1+680,REPORT_Y1+50,scn_name[player_school.scenario_id-1]);
	    letter_report_init_flag = 1;
	    button_back.create_text(350,vtab4+7,450,vtab4+28,"Proceed");
	    button_pgup.create_text(570,vtab4-10,660,vtab4+13,"Prev. page");
	    button_pgdn.create_text(570,vtab4+15,660,vtab4+38,"Next page");
	}
    }
    else {
	vga.use_back();
    }

    if((scen_page_number!=old_scen_page_number)||(refreshFlag == INFO_REPAINT)) {
	old_scen_page_number=scen_page_number;
	String context;

	//////////////////////////////
	// Scn 1 SCN_BALANCE_BUDGET //
	//////////////////////////////
	if(player_school.scenario_id==SCN_BALANCE_BUDGET) {
	    if(scen_page_number==1) {
		context = "To : ";
		context += player_school.player_first_name;
		context += " ";
		context += player_school.player_last_name;
		context += ", ";
		context += player_school.school_name;
		context += "           ";
		context += info.date_str_julian(letter_date);
		context +="\nFrom : ";
		context +=name_res.get_name_str(name_id1,name_id2,name_id3);
		context +=", Chair, Board of Trustees\n\n\n";
		context +="    The budget deficit you project alarms the Board. ";

		if(player_school.school_type==PRIVATE_SCHOOL)
		    context +="Unexpected shortfalls in research grants, gifts, and other income";
		if(player_school.school_type==LIBERAL_ARTS_SCHOOL)
		    context +="Unexpected shortfalls in gifts, tuition and other income";
		if(player_school.school_type==PUBLIC_SCHOOL)
		    context +="Unexpected shortfalls in research grants, state appropriations, tuition and other income";
		if(player_school.school_type==COMPREHENSIVE_SCHOOL)
		    context +="Unexpected shortfalls in state appropriations and other income";

		context +=" have thrown the budget ";
		context +=m.format(finance.this_year.surplus_deficit,2);
		context +=" thousands into the ";
		if(finance.this_year.surplus_deficit<0)
		    context+="red ";
		else
		    context+="black ";
		context +=".  Worse, hopes of more income anytime soon are vain.  At our recent meeting, you disappointed the Board by advancing no remedy to cope with the new circumstances promptly.\n";
	    }

	    if(scen_page_number==2) {
		context = "We are disturbed that you currently are targeting a 3 percent budget \
							  deficit for next year.\n\n";

		context +="    You must act immediately. ";
		context +="Although some actions may require more than one year to phase in, you must balance the budget within three years from now. ";
		context +="Furthermore, after achieving a balanced budget during years 4, 5, 6, you should run surpluses for years 7, 8, 9 or until reserves rise to their current level of ";
		context +=m.format(player_school.scenario_target[0],2);
		context +=" thousand.";
	    }

	    if(scen_page_number==3) {
		context = "You must accomplish the above without diminishing the average \"Performance ";
		context += "indicators\" and \"Attitudes toward the institution\" scores (listed on the \"Score\" screen.)";

	    }
	}

	////////////////////////////
	// Scn 2 SCN_RAISE_SALARY //
	////////////////////////////
	if(player_school.scenario_id==SCN_RAISE_SALARY) {
	    if(scen_page_number==1) {
		context = "To : ";
		context += player_school.player_first_name;
		context += " ";
		context += player_school.player_last_name;
		context += ", ";
		context += player_school.school_name;
		context += "           ";
		context += info.date_str_julian(letter_date);
		context +="\nFrom : ";
		context +=name_res.get_name_str(name_id1,name_id2,name_id3);
		context +=", Chair, Board of Trustees\n\n";
		context +="    The Board agrees with the concern you expressed about faculty salaries ";
		context +="at our last meeting.  At the meeting you convinced ";
		context +="us that the salaries we offer are 14% below the market. ";
		context +="Because competitive salaries are expected to remain constant, this means our salaries need to improve by 14%. We hope you can accomplish this within ten years, or in less time if possible.";
		/*
		  if(player_school.school_type==PRIVATE_SCHOOL || player_school.school_type==LIBERAL_ARTS_SCHOOL)
		  {
                  context +="Further, because salaries at competing institutions will grow about 20% during the next five years, ";
                  context +="our university must increase salaries 10 plus 20, or 30% within five years.\n";
		  }
		  else
		  {
                  context +="Further, because salaries at competing institutions will grow about 10% during the next five years, ";
                  context +="our university must increase salaries 10 plus 10, or 20% within five years.\n";
		  }
		*/
	    }       
	    /*
	      if(scen_page_number==2)
	      {

              if(player_school.school_type==PRIVATE_SCHOOL || player_school.school_type==LIBERAL_ARTS_SCHOOL)
              {
	      context ="    We ask you to pass from analysis to prescription. Because our faculty is the University, we must remedy our low salaries as soon as possible. How will you raise faculty \ 
	      salaries 30% and pay for the raises?  What combination of new money and reduced or eliminated programs will you use to finance the raises? Consider carefully \ 
	      how, beyond a more contented faculty, the higher salaries guarantee the continuity and ";
              }
	      else
	      {
	      context ="    We ask you to pass from analysis to prescription. Because our faculty is the University, we must remedy our low salaries as soon as possible. How will you raise faculty \ 
	      salaries 20% and pay for the raises?  What combination of new money and reduced or eliminated programs will you use to finance the raises? Consider carefully \ 
	      how, beyond a more contented faculty, the higher salaries guarantee the continuity and ";
	      }

	      context +="increase the worth of our university.";
	      }

	      if(scen_page_number==3)
	      {
	      }
	    */
	}   

	///////////////////////////////
	// Scn 3 SCN_INVEST_ACADEMIC //
	///////////////////////////////
	if(player_school.scenario_id==SCN_INVEST_ACADEMIC) {
	    if(scen_page_number==1) {
		context = "To : ";
		context += player_school.player_first_name;
		context += " ";
		context += player_school.player_last_name;
		context += ", ";
		context += player_school.school_name;
		context += "           ";
		context += info.date_str_julian(letter_date );
		context +="\nFrom : ";
		context +=name_res.get_name_str(name_id1,name_id2,name_id3);
		context +=", Chair, Board of Trustees\n\n";
		context +="    Congratulations. ";
		// find a female
		int staff_number=game_set.open_db("N_FIRST")->rec_count();
		while(name_res.is_male(staff2_name_id1)) {
		    staff2_name_id1++;
		    if(staff2_name_id1>staff_number)staff2_name_id1=1;
		}
		context +=name_res.get_name_str(staff2_name_id1,staff2_name_id2,staff2_name_id3);
		context +=" has just pledged ";
		context +=m.format(player_school.scenario_base1,2);
		context +=" thousands to enhance the ";

		if(player_school.school_type==PRIVATE_SCHOOL)
		    context +="sciences and allied fields";
		if(player_school.school_type==LIBERAL_ARTS_SCHOOL)
		    context +="humanities and allied fields";
		if(player_school.school_type==PUBLIC_SCHOOL)
		    context +="social sciences and allied fields";
		if(player_school.school_type==COMPREHENSIVE_SCHOOL)
		    context +="business, education, applied science and technology";

		context +=" at our university. ";
		context +="The pledge will raise next year's budget (for ";
		context += info.date_str_ymd( date.year(letter_date)+1, 0, 0 );
		context +=") by about ";
		context +=m.format(player_school.scenario_base2,2);
		context +=" thousand.\n\n";

		context +="    The donor gave to improve ";

		//-------- show the list of active departments related to the scenario -----//

		char* departmentPrivate[] = {
		    "biology", "chemistry", "physics", "mathematics", "computer science",
		    "engineering", "environmental science", NULL
		};

		char* departmentLiberalArts[] = {
		    "American studies", "classics", "English", "history", "philosophy",
		    "foreign languages", "art", "music", "drama", NULL
		};

		char* departmentPublic[] = {
		    "anthropology and sociology", "economics", "law", "political science",
		    "psychology", "gender/ethnic studies", "communication", NULL
		};

		char* departmentCompreh[] = {
		    "agricultural science", "technology services", "business", "education",
		    "health services", NULL
		};

		char** departmentList;
		int  hasDepartmentCount=0, thisCount=0;

		if(player_school.school_type==PRIVATE_SCHOOL)
		    departmentList = departmentPrivate;
		if(player_school.school_type==LIBERAL_ARTS_SCHOOL)
		    departmentList = departmentLiberalArts;
		if(player_school.school_type==PUBLIC_SCHOOL)
		    departmentList = departmentPublic;
		if(player_school.school_type==COMPREHENSIVE_SCHOOL)
		    departmentList = departmentCompreh;

		// count the number of available departments for later placement of the word "and"
		for( int i=0 ; i<12 && departmentList[i] ; i++ ) {
		    if( department_array.has_department(departmentList[i]) )
			hasDepartmentCount++;
		}

		// count the number of available departments for later placement of the word "and"
		for( int i=0 ; i<12 && departmentList[i] ; i++ ) {
		    if( department_array.has_department(departmentList[i]) ) {
			thisCount++;

			context += departmentList[i];

			if( thisCount == hasDepartmentCount ) // last one
			    context += ".";
			// second last one
			else if( thisCount == hasDepartmentCount-1 )
			    context += " and ";
			else
			    context += ", ";
		    }
		}
	    }

	    if(scen_page_number==2) {
		context ="How will you achieve the goal for which she gave so generously? \n\n\
								    You could concentrate big money on one or two departments or\n\
								spend a little on many. The number of students affected by an alternative represents one criterion for \
								selection. Another criterion is how your allocation of resources affects nonscience departments \
								and such important facets of university life as the quality of teaching.";
	    }
	    if(scen_page_number==3) {
		context ="    The Board will judge your \
								success first in meeting the donor's expectations, which may encourage other donors, \
								and then in lifting the prestige of our university.";
	    }
	}

	/////////////////////////////////
	// Scn 4 SCN_TEACHING_QUALITY //
	///////////////////////////////
	if(player_school.scenario_id==SCN_TEACHING_QUALITY) {
	    if(scen_page_number==1) {
		context = "To : ";
		context += player_school.player_first_name;
		context += " ";
		context += player_school.player_last_name;
		context += ", ";
		context += player_school.school_name;
		context += "           ";
		context += info.date_str_julian(letter_date);
		context += "\nFrom : ";
		context += name_res.get_name_str(name_id1,name_id2,name_id3);
		context += ", Chair, Board of Trustees\n\n\n";

		context +="    The recent report by the accreditation team alarms the Board. Our proud university \
					should not be cited for poor teaching. Something must be done.\n\n\
					    The team attributes our failure to inattention to education rather than to lack of \
					resources. They cited discontinued courses, student";
		// find a female
	    }
	    if(scen_page_number==2) {
		context ="demoralization, and insufficient \
					faculty time and attention devoted to students. They cited poor teaching in the \
					classroom. While some departments received a passing grade, most cry out for improvement.\n\n";
		context +="    In five rather than the usual ten years, the accreditation team will return. At that time, the Board \
				expects the University's educational quality will be rated good. ";
		//				context += m.format(player_school.sub_score[S_EDUCATIONAL_QUALITY][THIS_MONTH],1);			//BUGHERE
		context +="When they come in ten years for \
								our next regular examination, we expect our teaching will be rated excellent.";
		//				context += m.format(player_school.sub_score[S_EDUCATIONAL_QUALITY][THIS_MONTH]+15,1);;		//BUGHERE
		//				context +="excellent.";
	    }
	    if(scen_page_number==3) {
		context ="    Remembering the team's opinion that a lack of physical and financial \
					resources did not cause our poor teaching, the Board does not expect a \
					mere plea for more money to achieve accreditation. Rather, we expect a plan to put in our classrooms a \
					faculty that loves to teach.";
		//				How will you reach these goals?";
	    }
	}

	if(player_school.scenario_id==SCN_RESEARCH_QUALITY) {
	    if(scen_page_number==1) {
		context = "To : ";
		context += player_school.player_first_name;
		context += " ";
		context += player_school.player_last_name;
		context += ", ";
		context += player_school.school_name;
		context += "           ";
		context += info.date_str_julian(letter_date);
		context += "\nFrom : ";
		context += name_res.get_name_str(name_id1,name_id2,name_id3);
		context += ", Chair, Board of Trustees\n\n\n";
		context +="    At our last meeting you convinced the Board that faculty research and scholarship at our university are mediocre by national standards. \
					Something must be done.\n\n\
					    The Faculty Senate Research Committee attributes this condition to a lack of commitment \
					by both faculty and administrators rather than a";
	    }
	    if(scen_page_number==2) {
		context = "lack of resources. The Committee blames excessive teaching loads \
					and insufficient research support. \
					They blame low faculty morale. \
					Most departments require improvement.\n\n";
		context += "    In ten years, the Board and the Faculty Senate want the University's performance rating in scholarship to be strong. ";
		context += "The rating reflects the scholarship of integration, application, and \
					teaching as well as knowledge";
	    }
	    if(scen_page_number==3) {
		context ="creation. Remembering that the University's lackluster \
					research performance is not due to a lack of physical and financial resources, \
					the Board expects more than a \
					plea for money. Rather, we expect a plan to breathe life into our laboratories and \
					to reestablish the University's preeminence as a center of scholarship and discovery. How will you reach these goals?";
	    }
	}

	if(player_school.scenario_id==SCN_WINNING_ATHLETICS) {
	    if(scen_page_number==1) {
		context = "To : ";
		context += player_school.player_first_name;
		context += " ";
		context += player_school.player_last_name;
		context += ", ";
		context += player_school.school_name;
		context += "           ";
		context += info.date_str_julian(letter_date);
		context += "\nFrom : ";
		context += name_res.get_name_str(name_id1,name_id2,name_id3);
		context += ", Chair, Board of Trustees\n\n\n";
		context +="    The Alumni Association has impressed upon the Board the mediocrity of our athletes. \
					The Board agrees that our institution should win more games, thereby strengthening the loyalty \
					of alumni and attracting new students to all our departments.  Something must be done.";
	    }
	    if(scen_page_number==2) {
		context ="    The Alumni Association criticizes our admissions standards as too high and our athletics budget as too low. \
					We are faulted in particular for providing insufficient financial aid for athletes. \
					The Alumni expect our inattention to athletes to be corrected without damaging the academic \
					performance that, at bottom, represents the worth of our university. ";
		context +="    In four years, the Board and Alumni Association will review your progress";

		//				context +=player_school.goal_deadline[PLAQUE_SILVER];
		//				context +=" years,";

	    }
	    if(scen_page_number==3) {
		context ="as reflected in our teams' victories. By then, we want \
							our teams to win more than 60% of their games. We would \
							be especially pleased if they were \
							then winning more than 60% in a higher NCAA Division. \
							How will you achieve these goals and at what cost?";

		//					context ="we want our teams to win more than 60 percent of \ 
		//					their games. We would be especially pleased if they were then winning more than 60 \ 
		//					percent in a higher league.  How will you achieve these goals and at what cost?";
	    }
	}
	if(player_school.scenario_id==SCN_LIMIT_TUITION_GROWTH) {
	    if(scen_page_number==1) {
		context = "To : ";
		context += player_school.player_first_name;
		context += " ";
		context += player_school.player_last_name;
		context += ", ";
		context += player_school.school_name;
		context += "           ";
		context += info.date_str_julian(letter_date);
		context += "\nFrom : ";
		context += name_res.get_name_str(name_id1,name_id2,name_id3);
		context += ", Chair, Board of Trustees\n\n\n";
		context +="    The Board notes with alarm that our tuition has \
					escalated at a real rate of 3% per year. \
					Equal extravagance at other institutions does not relieve us \
					of our responsibility to lessen the burden on our students and \
					their parents. People will rebel if higher education continues to \
					be seen";
	    }
	    if(scen_page_number==2) {
		context ="as pushing aside their other needs.\n\n\
					    Believing the time has come to take a stand, the Board notifies you of its firm policy \
					that our tuition shall not rise. In fact, the Board would like tuition to go down.\n\n\
					    Until now the University has estimated the growth of nontuition revenue, funded new \
					programs, salary increases, and other costs, and then closed the gap by raising tuition. \
					You will now estimate tuition like ";
	    }
	    if(scen_page_number==3) {
		context ="other revenue and close the gap by disciplining expenditures. The new \
					budgeting will strengthen internal financial discipline by forcing trade-offs \
					among competing expenditures.  Most important, tuition \
					reductions will make us accessible to a larger pool of talented students, thus \
					ensuring the worth and continuity of our university.";
	    }
	}
	if(player_school.scenario_id==SCN_COMPENSATE_FOR_ENROLLMENT_SHIFTS) {
	    /*			int yy1,yy2,yy3;
				//BUGHERE
				yy1=(int)(player_school.scenario_target[0]/SCN3_TARGET_MULTIPLIER);
				yy2=(int)(player_school.scenario_target[1]*SCN3_TARGET_MULTIPLIER);
				yy3=(int)(player_school.scenario_target[2]/SCN3_TARGET_MULTIPLIER);
	    */
	    if(scen_page_number==1) {
		context = "To : ";
		context += player_school.player_first_name;
		context += " ";
		context += player_school.player_last_name;
		context += ", ";
		context += player_school.school_name;
		context += "           ";
		context += info.date_str_julian(letter_date);
		context += "\nFrom : ";
		context += name_res.get_name_str(name_id1,name_id2,name_id3);
		context += ", Chair, Board of Trustees\n\n\n";
		context +="    Shifts of enrollment among departments have distorted course listings, overloaded some \
					teachers and buildings, and left others idle. These shifts leave some courses half-filled while \
					students are foreclosed from desired, overfilled courses. Some departments have \
					upped the proportion of lecture courses to accommodate increased student numbers.";
	    }
	    if(scen_page_number==2) {

		context ="    Because the world has moved on and made a return to the previous pattern unlikely, the \
					Board shares the concern you expressed at our last meeting.  Going beyond describing \
					misalignments, however, how will you restructure \
					our academic programs and faculty resources to match today's needs?\
					\n\n    By the second semester of year ";

		context +=m.format(date.year(info.game_start_date)+player_school.goal_deadline[PLAQUE_BRONZE],4);

		context +=", the Board expects educational quality to increase";
		//				context +=m.format(yy1,1);
		//				context +=" to ";
		//				context +=m.format(player_school.scenario_target[0],1);
		context +=", course denials to decrease, and average";
	    }
	    if(scen_page_number==3) {
		//				context =" from ";
		//				context +=m.format(yy2,1);
		//				context +=" to ";
		//				context +=m.format(player_school.scenario_target[1],1);
		context ="teaching load hours to rise ";
		//				context +=m.format(yy3,1);
		//				context +=" to ";
		//				context +=m.format(player_school.scenario_target[2],1);
		context +="substantially. What is your plan?";
	    }
	}
	if(player_school.scenario_id==SCN_STUDENT_DIVERSITY) {
	    if(scen_page_number==1) {
		context = "To : ";
		context += player_school.player_first_name;
		context += " ";
		context += player_school.player_last_name;
		context += ", ";
		context += player_school.school_name;
		context += "           ";
		context += info.date_str_julian(letter_date);
		context += "\nFrom : ";
		context += name_res.get_name_str(name_id1,name_id2,name_id3);
		context += ", Chair, Board of Trustees\n\n\n";
		context += "    The Board shares your concern about the scarcity of minorities among our students. \
					Further, because minorities are equally scarce among freshmen as among seniors, we see no \
					evidence that time will cure the problem. Instead, you must act \
					promptly to increase the number of minorities.";

	    }
	    if(scen_page_number==2) {
		context += "    The Board sets your goal as boosting the number of minority students twofold in ten \
					years, that is, from ";
		context+=m.format(player_school.scenario_base,3);
		context+=" to ";
		context+=m.format(player_school.scenario_target[0],3);
		context += ". At the same time, we expect this will make us \
					accessible to new pools of talented students, guaranteeing the worth and thus continuity \
					of our university.";
	    }
	}
	if(player_school.scenario_id==SCN_FACULTY_DIVERSITY) {
	    if(scen_page_number==1) {
		context = "To : ";
		context += player_school.player_first_name;
		context += " ";
		context += player_school.player_last_name;
		context += ", ";
		context += player_school.school_name;
		context += "           ";
		context += info.date_str_julian(letter_date);
		context += "\nFrom : ";
		context += name_res.get_name_str(name_id1,name_id2,name_id3);
		context += ", Chair, Board of Trustees\n\n\n";
		context += "    At our last meeting, you told the Board your concern about the few minorities and \
					women on the faculty. We agree heartily and expect the shortfall to be redressed \
					promptly.\n\n";
		context += "    You told us the University has about the same underrepresentation";
	    }
	    if(scen_page_number==2) {
		context="in all \
					fields, and that minorities are even more underrepresented than women are. The scarcity of \
					minorities and women among young as well as old faculty shows the problem will not \
					correct itself.\n\n    The Board sets your goal as boosting minority representation by \
					50% in five years.";
		//				: that \ 
		//					is, from ";

		//				context+=m.format(player_school.scenario_target[0]/1.5f,3);
		//				context+=" to ";
		//				context+=m.format(player_school.scenario_target[0],3);
		context+=". For women, the goal is a 25% increase in five years.";
		//: from ";
		//				context+=m.format(player_school.scenario_target[1]/1.25f,3);
		//				context+=" to ";
		//				context+=m.format(player_school.scenario_target[1],3);

	    }
	    if(scen_page_number==3) {
		context="    We ask how you will accomplish these goals and with what change \
					in budget and staff size. At the same time, we expect the enhanced representation of women \
					and minorities on the faculty to enrich all aspects of university life.";
	    }
	}

	font_letter.put_paragraph(180,190,670,440,context.str_buf);
	String pagestr;
	pagestr = scen_page_number;
	pagestr += "/";
	pagestr += max_scen_page_number;
	font_letter.put(680,vtab4+20,pagestr.str_buf);

	button_back.paint();
	if(scen_page_number<=1)button_pgup.disable();
	if(scen_page_number>=max_scen_page_number)button_pgdn.disable();

	button_pgup.paint();
	button_pgdn.paint();

	if(refreshFlag != INFO_REPAINT)
	    vga.blt_buf(180,190,670,440);
    }
    //------- blt the back buffer to front buffer -------//

    if(refreshFlag == INFO_REPAINT)
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

    vga.use_front();
}

int Letter::scen_welcome_letter_detect() {
    if (!letter_report_init_flag)
	return 0;
    vga.use_front();
    if(scen_page_number<max_scen_page_number)
	if(button_pgdn.detect()) {
	    if(scen_page_number<max_scen_page_number)
		scen_page_number++;
	    sys.redraw_all_flag=1;
	};

    if(scen_page_number>1)
	if(button_pgup.detect()) {
	    if(scen_page_number>1)
		scen_page_number--;
	    sys.redraw_all_flag=1;
	};

    if(button_back.detect()   ||
       (m.upper(mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_UNIQUE_KEY))=='W')) {
	scen_page_number=1;
	old_scen_page_number=-1;
	if(sys.save_view_mode!=MODE_YEAR_END_REPORT) {
	    sys.set_staying_view_mode(0);
	    sys.set_view_mode(MODE_NORMAL);
	}
	else {
	    sys.set_staying_view_mode(MODE_YEAR_END_REPORT);
	}
	return 1;
    }
    return 0;
}

//////////////////////////////////////////////////////

int compare_ascend(const void *arg1,const void *arg2) {
    float p,q;
    p=((struct SortData*)arg1)->strength;
    q=((struct SortData*)arg2)->strength;

    if(p<q)return -1;
    if(p>q)return 1;
    return 0;
};

int compare_descend(const void *arg1,const void *arg2) {
    float p,q;
    p=((struct SortData*)arg1)->strength;
    q=((struct SortData*)arg2)->strength;

    if(p>q)return -1;
    if(p<q)return 1;
    return 0;
};

int sort_consistency(const void *arg1, const void *arg2) {
    float p,q;

    p=((struct SortData*)arg1)->consistency;
    q=((struct SortData*)arg2)->consistency;

    if(p>q)return -1;
    if(p<q)return 1;
    return 0;
}

//---------- Begin of function Letter::welcome_letter_report ------------//
void Letter::welcome_letter_report(int refreshFlag) {
    int tab1=REPORT_X1,tab2=REPORT_X1+70,
	tab3=REPORT_X1+100,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+20,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	if(letter_report_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!letter_report_init_flag) {
	    sys.set_speed(0);
	    user_interface.bg_img(BG_PIC_ID, &vga_back);

	    font_letter.center_put(REPORT_X1+50,REPORT_Y1,REPORT_X1+680,REPORT_Y1+20,"Welcome Letter ");

	    letter_report_init_flag = 1;
	    button_back.create_text(350,vtab4+7,450,vtab4+28,"Proceed");
	    button_pgup.create_text(570,vtab4-10,660,vtab4+13,"Prev. page");
	    button_pgdn.create_text(570,vtab4+15,660,vtab4+38,"Next page");
	}
    }
    else {
	vga.use_back();
    }

    if((welcome_page_number!=old_welcome_page_number)||(refreshFlag == INFO_REPAINT)) {
	old_welcome_page_number=welcome_page_number;
	String context;

	if(welcome_page_number==1) {
	    context = "To      :    ";
	    context += player_school.player_first_name;
	    context += " ";
	    context += player_school.player_last_name;
	    context += "\n";

	    context += "              President, ";
	    context += player_school.school_name;
	    context += "\n";

	    context += "From  :    ";
	    context += name_res.get_name_str(name_id1,name_id2,name_id3);
	    context += "\n";

	    context += "              Chair, Board of Trustees",
		context += "\n";

	    context += "Date   :    ";
	    context += info.date_str_julian(letter_date);
	    context += "\n";
	    context += "\n";

	    context +="Dear President ";
	    context +=player_school.player_last_name;
	    font_letter.put_paragraph(185,120,670,250,context.str_buf);

	    context ="    Welcome to ";
	    context +=player_school.school_name;
	    context +=". The Board is pleased to have as distinguished a person as yourself as our new president.\n\n";
	    context +="    The University has ";
	    context +=m.format(department_array.department_count,4);
	    context +=" departments with ";
	    context +=m.format(department_array_faculty_level_history_FT,4);
	    context +=" faculty members. We enroll ";
	    context +=m.format(enroll_res_total_student_count,4);
	    context +=" students and have ";
		int totalDollar, i;
	    for (i=department_array.size(), totalDollar=0; i>0; i--) {
		if (department_array.is_deleted(i))
		    continue;
		// fix in ver 2
		//totalDollar += department_array_total_research_dollar[i];
		totalDollar += department_array_total_research_dollar[i-1];
	    }
	    context +=m.format(totalDollar,2);
	    context +=" thousands of sponsored research expenditures monthly. The breakdown by departments is as follows :";
	    //			context +=m.format(department_res_general_dept_student_level_historyMT,4);
	    //			context +=" students have yet to declare a major)";
	    //			context +="\n";
	    font_letter.put_paragraph(185,250,670,420,context.str_buf);
	}
	if(welcome_page_number==2) {
	    int list_tab1=185;
	    int list_tab2=500;
	    int list_tab3=580;
	    int list_tab4=660;
	    int list_start_y=130;
	    int list_line_spacing=290/(1+department_array.size());

	    font_letter.put(list_tab2-50,list_start_y,"Faculty");
	    font_letter.put(list_tab3-50,list_start_y,"Students");
	    font_letter.put(list_tab4-50,list_start_y,"Research");

	    for(int i=1;i<=department_array.size();i++) {
		if(!department_array.is_deleted(i)) {
		    int y=list_start_y+i*list_line_spacing;
		    String tmp;
		    tmp=department_array[i]->name();
		    tmp.str_buf[tmp.len()-10]='\0';
		    font_letter.put(list_tab1,y,tmp.str_buf);
		    // fix in ver 2
		    //font_letter.right_put(list_tab2,y,m.format(department_array_faculty_array_faculty_count[i],4));
		    //font_letter.right_put(list_tab3,y,m.format(department_array_student_array_student_count[i],4));
		    //font_letter.right_put(list_tab4,y,m.format(department_array_total_research_dollar[i],2));
		    font_letter.right_put(list_tab2,y,m.format(department_array_faculty_array_faculty_count[i-1],4));
		    font_letter.right_put(list_tab3,y,m.format(department_array_student_array_student_count[i-1],4));
		    font_letter.right_put(list_tab4,y,m.format(department_array_total_research_dollar[i-1],2));
		}
	    }
	}
	if(welcome_page_number==3) {
	    context ="    Traditional undergraduates number ";
	    context +=m.format(enroll_res_student_count_UG_TRADITION,4);
	    context +=", of which ";
	    context +=m.format(student_office_percent_demanding_places_sl1,3);
	    context +=" live in on-campus residence halls";
	    context +=" and ";
	    context +=m.format(department_res_general_dept_student_array_student_count,4);
	    context +=" have yet to declare a major.";

	    context +=" About ";
	    context +=m.format(10 * school_res_player_peer_school_percent_get_bacc_in_5_year,3);
	    context +=" graduate within five years. The University receives ";
	    //			context +=m.format(enroll_res_applications_rate_UG_TRADITION,4);		//### fred 1127  //## chea081099
	    //### fred 1127  //## chea081099
	    context +=m.format(enroll_res_applications_rate_UG_TRADITION,26);
	    context +=" applications for every traditional undergraduate space. An additional ";
	    context +=m.format(enroll_res_student_count_UG_NONTRADITION,4);
	    //			context +=m.format(enroll_res_student_count_UG_TRADITION,4);
	    context +=" students attend part-time ";
	    context +=" (nontraditional undergraduates) and ";
	    context +=m.format(enroll_res_student_count_DISTANCE_LEARN,4);
	    context +=" students enroll as distance learners.";
	    context +=" About ";
	    context +=m.format(enroll_res_overall_percent_minority,3);
	    context +=" of students are minorities and ";
	    context +=m.format(enroll_res_overall_percent_female,3);
	    context +=" are women.";
	    context +=" We have ",
		context +=m.format(enroll_res_student_count_MASTER,4);
	    context +=" master's students and ";
	    context +=m.format(enroll_res_student_count_DOCTOR,4);
	    context +=" doctoral students. ",
		font_letter.put_paragraph(185,160,670,340,context.str_buf);

	    //			font_letter.put_paragraph(185,340,670,400,context.str_buf);
	}
	if(welcome_page_number==4) {
	    context ="    Of our ";
	    context +=m.format(department_array_faculty_level_history_FT,4);
	    context +=" faculty members, ";
	    //	      context +=m.format(department_array_faculty_count_on_tenure_line,1);
	    //## chea 110809 change by b.massy
	    context +=m.format(department_array_faculty_count_on_tenure_line,0);

	    context +=" are on the tenure line and ";
	    //			context +=m.format(department_array_faculty_count_tenured,1);
	    //## chea 110809 change by b.massy
	    context +=m.format(department_array_faculty_count_tenured,0);

	    context +=" are tenured. ";
	    /*
	      context +=" We believe the tenure-line faculty scores at the ";
	      context +=m.format(department_array.faculty_rank_in_research_talent,1);;
	      context +="th";
	      context +=" percentile nationally in research talent and the ";
	      context +=m.format(department_array.faculty_rank_in_scholarship_talent,1);
	      context +="th";
	      context +=" percentile in scholarly talent, and that our whole faculty scores at the ";
	      context +=m.format(department_array.faculty_rank_in_teaching_talent,1);
	      context +="th";
	      context +=" percentile in teaching talent. ";
	    */
	    context +="About ";
	    int percentMinor = (department_array_faculty_gender_history_MINORITY_MALE_HISTORY_YEAR_COUNT_1+
				department_array_faculty_gender_history_MINORITY_FEMALE_HISTORY_YEAR_COUNT_1) * 100
		/ department_array_faculty_gender_history_GENDER_ETHNIC_TYPE_COUNT_N_TOTAL_1_HISTORY_YEAR_COUNT_1;

	    int percentFemale = (department_array_faculty_gender_history_MINORITY_FEMALE_HISTORY_YEAR_COUNT_1+
				 department_array_faculty_gender_history_NONMINORITY_FEMALE_HISTORY_YEAR_COUNT_1) * 100
		/ department_array_faculty_gender_history_GENDER_ETHNIC_TYPE_COUNT_N_TOTAL_1_HISTORY_YEAR_COUNT_1;

	    context +=m.format(percentMinor,3);
	    context +=" of faculty are minorities and ";
	    context +=m.format(percentFemale,3);
	    context +=" are women.";
	    context +="\n\n";
	    context +="  The University's men's football and women's basketball teams compete at the Division level ";

	    if(player_school_athletic_program_intensity==INPUT_HIGH)
		context +="I";
	    if(player_school_athletic_program_intensity==INPUT_MEDIUM)
		context +="II";
	    if(player_school_athletic_program_intensity==INPUT_LOW)
		context +="III";

	    context +=". Last year's win/loss records were ";

	    // Athletics *off = &athletics_office;

	    int winCount, lossCount;

	    winCount = int(off_football_probability_win * off_football_game_count_off_ncaa_level_input);
	    lossCount = int(off_football_game_count_off_ncaa_level_input - winCount);
	    context +=m.format(winCount,1);
	    context +="-";
	    context +=m.format(lossCount,1);
	    context +=" and ";

	    winCount = int(off_basketball_probability_win * off_basketball_game_count_off_ncaa_level_input);
	    lossCount = int(off_basketball_game_count_off_ncaa_level_input - winCount);
	    context +=m.format(winCount,1);
	    context +="-";
	    context +=m.format(lossCount,1);
	    context +=", respectively.";
	    font_letter.put_paragraph(185,180,670,420,context.str_buf);
	}

	if(welcome_page_number==5) {
	    /*	
		"",
		"",
		"",
		"We will be watching these areas with special care during the years ahead.",

		"The Board is counting on you and we wish you every success.",
		"",
		"We also want you to consider the following",
	    */
	    //			context="";

	    int surplusDeficit = (int)finance_last_year_surplus_deficit;

	    if(surplusDeficit!=0) {
		context= "    Last year the University's budget was in the ";
		if(finance_last_year_surplus_deficit<0)
		    context+="red ";
		else
		    context+="black ";

		context+="by ";

		if(surplusDeficit<0) {
		    context+=m.format(-surplusDeficit,2);
		}
		else {
		    context+=m.format(surplusDeficit,2);
		}

		context += " thousand (";

		if(surplusDeficit<0)                      //## chea 110899 change by b.massy
		    context+=m.format((float)-surplusDeficit * 100 / finance_total_expense_total,24);
		else
		    context+=m.format((float)surplusDeficit * 100 / finance_total_expense_total,24);

		context += " of expenditures). ";
	    }

	    double test_surplus=finance_total_expense_total/finance_last_year_surplus_deficit;
	    if(test_surplus==0.0)return;
	    if(test_surplus<-0.25)
		context+="The Board hopes you will correct this weak financial performance.";
	    if(test_surplus>0.0)
		context+="The Board hopes you will maintain this kind of strong financial performance.";
	    else
		context+="";

	    font_letter.put_paragraph(185,170,670,240,context.str_buf);

	    context="    The Board is counting on you and we wish you every success.";
	    font_letter.put_paragraph(185,250,670,420,context.str_buf);
	}

	String pagestr;
	pagestr = welcome_page_number;
	pagestr += "/";
	pagestr += max_welcome_page_number;
	font_letter.put(680,vtab4+20,pagestr.str_buf);

	button_back.paint();
	if(welcome_page_number<=1)button_pgup.disable();
	if(welcome_page_number>=max_welcome_page_number)button_pgdn.disable();

	button_pgup.paint();
	button_pgdn.paint();

	if(refreshFlag != INFO_REPAINT)
	    vga.blt_buf(185,140,670,420);
	//		vga.blt_buf(350,vtab4+10,450,vtab4+28);
	//		vga.blt_buf(570,vtab4-10,660,vtab4+13);
	//		vga.blt_buf(570,vtab4+15,660,vtab4+38);
    }
    //------- blt the back buffer to front buffer -------//

    if(refreshFlag == INFO_REPAINT)
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

    vga.use_front();
}

//---------- End of function Letter::welcome_letter_report ------------//

//---------- End of function Letter::welcome_letter_detect ------------//
int Letter::welcome_letter_detect() {
    if (!letter_report_init_flag)
	return 0;
    vga.use_front();
    //	letter_browser.detect();
    if(welcome_page_number<max_welcome_page_number)
	if(button_pgdn.detect()) {
	    if(welcome_page_number<max_welcome_page_number)
		welcome_page_number++;
	    sys.redraw_all_flag=1;
	};

    if(welcome_page_number>1)
	if(button_pgup.detect()) {
	    if(welcome_page_number>1)
		welcome_page_number--;
	    sys.redraw_all_flag=1;
	};

    if(button_back.detect()) {
	welcome_page_number=1;
	old_welcome_page_number=-1;
	if(sys.save_view_mode!=MODE_YEAR_END_REPORT) {
	    sys.set_staying_view_mode(0);
	    sys.set_speed(sys.save_speed);
	    sys.set_view_mode(MODE_NORMAL);
	    sys.save_view_mode=MODE_NORMAL;
	}
	else
	    sys.set_staying_view_mode(MODE_YEAR_END_REPORT);
	return 1;
    }

    if(m.upper(mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_UNIQUE_KEY))=='W') {
	welcome_page_number=1;
	old_welcome_page_number=-1;
	if(sys.save_view_mode!=MODE_YEAR_END_REPORT) {
	    sys.set_staying_view_mode(0);
	    sys.view_mode=MODE_WELCOME_LETTER;
	    sys.save_view_mode=MODE_NORMAL;
	}
	else
	    sys.set_staying_view_mode(MODE_YEAR_END_REPORT);
	return 1;
    }
    return 0;
}

//---------- End of function Letter::welcome_letter_detect ------------//

//////////////////////////////////////////////////////

//---------- Begin of function Letter::eval_letter_report ------------//
void Letter::eval_letter_report(int refreshFlag) {
    int tab1=REPORT_X1,tab2=REPORT_X1+70,
	tab3=REPORT_X1+100,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+20,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    vga.use_back();

    if (refreshFlag == INFO_REPAINT) {
	if(letter_report_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!letter_report_init_flag) {
	    sys.set_speed(0);
	    user_interface.bg_img(BG_PIC_ID, &vga_back);

	    font_letter.center_put(REPORT_X1+50,REPORT_Y1,REPORT_X1+680,REPORT_Y1+20,"Evaluation Letter ");

	    letter_report_init_flag = 1;
	    button_back.create_text(350,vtab4+7,450,vtab4+28,"Proceed");
	    button_pgup.create_text(570,vtab4-10,660,vtab4+13,"Prev. page");
	    button_pgdn.create_text(570,vtab4+15,660,vtab4+38,"Next page");
	}
    }
    else {
	vga.use_back();
    }

    if( eval_page_number > max_eval_page_number )
	eval_page_number = max_eval_page_number;

    //----------------------------------------------//

    if((eval_page_number!=old_eval_page_number)||(refreshFlag == INFO_REPAINT)) {
	//--- recalculate the # of pages it has ----//

	max_eval_page_number = 1 + eval_letter_show_poor_area(1) + eval_letter_show_good_area(1) + eval_letter_show_mixed_area(1);

	old_eval_page_number=eval_page_number;

	//--------------------------------------------//

	if(eval_page_number==1) {
	    eval_letter_show_first_page();
	}

	//---------------------------------------------------------//

	if( eval_page_number==2 ) {
	    if( !eval_letter_show_poor_area() ) {
		eval_letter_show_good_area();
	    }
	}

	//---------------------------------------------------------//

	if( eval_page_number==3 ) {
	    eval_letter_show_good_area();
	}

	if ( eval_page_number == 4 ) {
	    eval_letter_show_mixed_area();
	}

	//-------- show letter now ------------//

	String pagestr;
	pagestr = eval_page_number;
	pagestr += "/";
	pagestr += max_eval_page_number;
	font_letter.put(680,vtab4+20,pagestr.str_buf);

	button_back.paint();
	if(eval_page_number<=1)button_pgup.disable();
	if(eval_page_number>=max_eval_page_number)button_pgdn.disable();

	button_pgup.paint();
	button_pgdn.paint();

	if(refreshFlag != INFO_REPAINT)
	    vga.blt_buf(185,140,670,420);
    }

    //------- blt the back buffer to front buffer -------//

    if(refreshFlag == INFO_REPAINT)
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

    vga.use_front();
}

//---------- End of function Letter::eval_letter_report ------------//

//---------- End of function Letter::eval_letter_show_first_page ------------//

int Letter::eval_letter_show_first_page(int generateText) {
    if( info.prerun_year )
	return 0;

    if( generateText ) {                            // only set the text when the letter is being generated.
	eval_page_1 = "To      :    ";
	eval_page_1 += player_school.player_first_name;
	eval_page_1 += " ";
	eval_page_1 += player_school.player_last_name;
	eval_page_1 += "\n";

	eval_page_1 += "              President, ";
	eval_page_1 += player_school.school_name;
	eval_page_1 += "\n";

	eval_page_1 += "From  :    ";
	eval_page_1 += name_res.get_name_str(name_id1,name_id2,name_id3);
	eval_page_1 += "\n";

	eval_page_1 += "              Chair, Board of Trustees",
	    eval_page_1 += "\n";

	eval_page_1 += "Date   :    ";
	eval_page_1 += info.date_str_julian(letter_date);
	eval_page_1 += "\n";

	eval_page_1 +="Dear President ";
	eval_page_1 +=player_school.player_last_name;
	eval_page_1 += " :\n";

	eval_page_1 +="        The Compensation Committee of the Board has completed your performance evaluation for year ";
	// eval_page_1 += info.date_str_julian(letter_date, INFO_DATE_DEFAULT|INFO_DATE_NO_DAY|INFO_DATE_NO_MONTH);
	// no need to add Yr. because previous word is year
	eval_page_1 += info.date_str_julian(letter_date, INFO_DATE_DEFAULT&~INFO_DATE_ADD_YR|INFO_DATE_NO_DAY|INFO_DATE_NO_MONTH);

	eval_page_1 +=". The Committee believes your performance has been ";

	//----- description word for "your performance" ------//

	// Item 15b)
	// ####### begin Gilbert 09/11/2001 ######//
	//int result = player_school.think_protagonist_event( player_school.cur_game_score, player_school.game_score_history[35],
	//																		player_school.init_game_score, player_school.get_protagonist_info(1),-1);
	int result = player_school.think_protagonist_event( player_school.cur_game_score, player_school.game_score_history[THIS_MONTH-12],
							    player_school.init_game_score, player_school.get_protagonist_info(1),-1, 1);
	// ####### end Gilbert 09/11/2001 ######//

	switch (result) {
	case 1: eval_page_1 += "outstanding. "; break;
	case 2: eval_page_1 += "strong. ";      break;
	case 3: eval_page_1 += "weak. ";      break;
	case 4:                                     // case 4 and case 7
	case 7: eval_page_1 += "unacceptable. ";  break;
	case 5:
	case 6: eval_page_1 += "mixed. ";     break;
	default:  eval_page_1 += "satisfactory. ";  break;
	}

	if ( result == 4 || result == 7 ) {           // unacceptable
	    // second consecutive "unacceptable"
	    if ( player_school.is_unacceptable_at_year_end ) {
		// Fired screen
		should_end_game = 1;
	    }
	    else
		player_school.is_unacceptable_at_year_end = 1;
	}
	else
	    player_school.is_unacceptable_at_year_end = 0;

	// Second Paragraph
	eval_page_1_p2 = "          This evaluation considers both the change during the last year and the cumulative change since the start of the game. ";
	if ( player_school.scenario_id > 0 )          // if it is a scenario game
	    eval_page_1_p2 += "It does not take progress toward the scenario goal into account.";
	eval_page_1_p2 += "\n";

	// Third Paragraph
	if ( result == 4 || result == 7 )
	    eval_page_1_p2 += "       The Board wishes to inform you that failure to improve your performance during the next year will result in termination.";

    }
    else {
	font_letter.put_paragraph(175,120,670,420,eval_page_1.str_buf);
	font_letter.put_paragraph(175,320,670,520, eval_page_1_p2.str_buf);
    }
    return 1;
}

//---------- End of function Letter::eval_letter_show_first_page ------------//

//---------- End of function Letter::eval_letter_show_poor_area ------------//
//
// [int] justCalc  - whether this function is called just for calculation.
//							(default: 0)
// [int] generateText - whether this is the first time showing this letter or repeated showing
//						   (this only matters when justCalc is 0. Default: 0)
//
int Letter::eval_letter_show_poor_area(int justCalc, int generateText) {
    //------ set the performance value of individual areas and then sort them ----//
    if( info.prerun_year )
	return 0;

    int i;

    get_variables_score(1);                         // 1 -- sort it into acscending order

    float s5, s6, s7;

    s5 = sort_data[4].strength;
    s6 = sort_data[5].strength;
    s7 = sort_data[6].strength;

    int displayNumber;

    if ( (abs(s5-s6) > abs(s6-s7)) && (abs(s6-s7) < abs(1.05*s6)) )
	displayNumber = 7;
    else
	if ( (abs(s5-s6) < abs(s6-s7)) && (abs(s5-s6) < abs(1.05*s6)) )
	    displayNumber = 5;
	else
	    displayNumber = 6;

    if( justCalc )
	return 1;

    if( generateText ) {
	// reset the sort_data
	for ( i=0; i<SCORE_VAR_COUNT; i++ )
	    sort_data[i].tag = 0;

	int addedCount=0;

	eval_poor_areas="        The Board believes the following areas are most in need of improvement. Entries near the top of the list need the most attention.\n\n";

	// display poor items
	for ( i=0; i<displayNumber; i++ ) {
	    eval_poor_areas+="         -  ";
	    eval_poor_areas+=area_str[sort_data[i].idx];
	    sort_data[i].tag = 1;
	    eval_poor_areas+="\n";
	}
    }
    else {
	font_letter.put_paragraph(185,150,670,420,eval_poor_areas.str_buf);
    }

    return 1;
}

//---------- End of function Letter::eval_letter_show_poor_area ------------//

//---------- End of function Letter::eval_letter_show_good_area ------------//
//
// [int] justCalc  - whether this function is called just for calculation.
//							(default: 0)
// [int] generateText - whether this is the first time showing this letter or repeated showing
//						   (this only matters when justCalc is 0. Default: 0)
//
int Letter::eval_letter_show_good_area(int justCalc, int generateText) {
    //------ set the performance value of individual areas and then sort them ----//
    if( info.prerun_year )
	return 1;

    qsort(sort_data,(size_t)SCORE_VAR_COUNT,sizeof(struct SortData),compare_descend);

    int i;

    float s5, s6, s7;
    int displayNumber;

    s5 = sort_data[4].strength;
    s6 = sort_data[5].strength;
    s7 = sort_data[6].strength;

    if ( (abs(s5-s6) > abs(s6-s7)) && (abs(s6-s7) < abs(1.05*s6)) )
	displayNumber = 7;
    else
	if ( (abs(s5-s6) < abs(s6-s7)) && (abs(s5-s6) < abs(1.05*s6)) )
	    displayNumber = 5;
	else
	    displayNumber = 6;

    if( justCalc )
	return 1;
    //--------------------------------------------//

    if( generateText ) {
	eval_good_areas="        The Board is pleased with your performance in these areas. Entries at the top of the list represent the best performance.\n\n";

	for ( i=0; i<displayNumber; i++ ) {
	    eval_good_areas+="         -  ";
	    eval_good_areas+=area_str[sort_data[i].idx];
	    sort_data[i].tag = 1;
	    eval_good_areas+="\n";
	}
    }
    else {
	font_letter.put_paragraph(185,150,670,420,eval_good_areas.str_buf);
    }
    return 1;
}

//---------- End of function Letter::eval_letter_show_good_area ------------//

// --- Begin of function Letter::eval_letter_show_mixed_area --- //
//
// Version 2.0 Item 15f)
int Letter::eval_letter_show_mixed_area(int justCalc, int generateText) {
    if ( justCalc )
	return 1;

    if( info.prerun_year )
	return 1;

    // Item 15f ii)
    // first get the items which have not yet displayed
    int i;
    SortData tempSortData[SCORE_VAR_COUNT];
    int count = 0;
    for ( i=0; i<SCORE_VAR_COUNT; i++ ) {
	if ( sort_data[i].tag == 0 ) {                // has not yet displayed
	    tempSortData[count].idx = sort_data[i].idx;
	    tempSortData[count].consistency = sort_data[i].consistency;
	    tempSortData[count].k1 = sort_data[i].k1;
	    tempSortData[count].tag = 1;
	    count++;
	}
    }

    // sort the tempSortData by consistency (descending)
    qsort(tempSortData,count,sizeof(struct SortData),sort_consistency);

    if ( generateText ) {
	eval_mixed_areas="        The Board believes your performance in these areas to be mixed.\n\n";
	// Item 15f iii)
	int printedNumber1=0;
	for ( i=0; i<count; i++ ) {
	    if ( tempSortData[i].consistency >= tempSortData[i].k1 )
		printedNumber1++;
	}

	// If have something to display
	if ( printedNumber1 > 0 )
	    eval_mixed_areas+="        Trend is up but cumulative performance remains negative:\n";

	printedNumber1=0;
	for ( i=0; i<count; i++ ) {
	    if ( printedNumber1 == 3 )
		break;
	    if ( tempSortData[i].consistency >= tempSortData[i].k1 ) {
		eval_mixed_areas+="         -  ";
		eval_mixed_areas += area_str[tempSortData[i].idx];
		eval_mixed_areas += "\n";
		printedNumber1++;
	    }
	}

	// Item 15f iv)
	int printedNumber2 = 0;
	for ( i=count-1; i>=0; i-- ) {
	    if ( tempSortData[i].consistency <= -tempSortData[i].k1 )
		printedNumber2++;
	}

	// if have something to print
	if ( printedNumber2 > 0 )
	    eval_mixed_areas+="\n        Trend is down but cumulative performance remains positive:\n";

	printedNumber2=0;
	for ( i=count-1; i>=0; i-- ) {
	    if ( printedNumber2 == 3 )
		break;
	    if ( tempSortData[i].consistency <= -tempSortData[i].k1 ) {
		eval_mixed_areas+="         -  ";
		eval_mixed_areas += area_str[tempSortData[i].idx];
		eval_mixed_areas += "\n";
		printedNumber2++;
	    }
	}

	// if nothing has printed out
	if ( (printedNumber1 + printedNumber2) == 0 )
	    eval_mixed_areas += "        No items meet this criterion.";

    }
    else {
	font_letter.put_paragraph(185,150,670,420,eval_mixed_areas.str_buf);
    }
    return 1;

}

//
// --- End of function Letter::eval_letter_show_mixed_area --- //

//---------- Begin of function Letter::eval_letter_detect ------------//
int Letter::eval_letter_detect() {
    if (!letter_report_init_flag)
	return 0;
    vga.use_front();

    //--- recalculate the number of pages it has ----//

    max_eval_page_number = 1 + eval_letter_show_poor_area(1) + eval_letter_show_good_area(1) + eval_letter_show_mixed_area(1);

    if(eval_page_number<max_eval_page_number)
	if(button_pgdn.detect()) {
	    if(eval_page_number<max_eval_page_number)
		eval_page_number++;
	    sys.redraw_all_flag=1;
	};

    if(eval_page_number>1)
	if(button_pgup.detect()) {
	    if(eval_page_number>1)
		eval_page_number--;
	    sys.redraw_all_flag=1;
	};

    // for mail button
    if(button_back.detect()) {
	eval_page_number=1;
	old_eval_page_number=-1;
	welcome_page_number=1;
	old_welcome_page_number=-1;

	if ( should_end_game ) {                      // lose game by consecutive unacceptable performance
	    lose_game.set_reason( LOSE_REASON_UNACCEPTABLE );
	    sys.set_staying_view_mode(MODE_LOSE_GAME);
	}
	else {
	    if ( player_school.is_public() ) {
		if ( (sys.save_view_mode != MODE_YEAR_END_REPORT) && (sys.save_view_mode != MODE_STATE_REPORT) )
		    {
			sys.set_staying_view_mode(0);
			sys.set_speed(sys.save_speed);
			sys.set_view_mode(MODE_NORMAL);
			sys.save_view_mode=MODE_NORMAL;
		    }
		    else {
			if ( player_school.is_public() )
			    sys.set_view_mode( MODE_STATE_REPORT );
			else
			    sys.set_view_mode( MODE_YEAR_END_REPORT );

			player_school.is_year_end_report = 1;
			player_school.current_report_type = 3;  // (REPORT_TYPE_TURNOVER)

		    }
	    }
	    else {
		if ( sys.save_view_mode!=MODE_YEAR_END_REPORT) {
		    sys.set_staying_view_mode(0);
		    sys.set_speed(sys.save_speed);
		    sys.set_view_mode(MODE_NORMAL);
		    sys.save_view_mode=MODE_NORMAL;
		}
		else {
		    sys.set_view_mode( MODE_YEAR_END_REPORT );
		    sys.staying_view_mode = MODE_YEAR_END_REPORT ;
		    player_school.is_year_end_report = 1;
		    player_school.current_report_type = 3;  // (REPORT_TYPE_TURNOVER)
		}
	    }
	    return 2;
	}
	return 1;
    }

    if(m.upper(mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_UNIQUE_KEY))=='W') {
	welcome_page_number=1;
	old_welcome_page_number=-1;
	if(sys.save_view_mode!=MODE_YEAR_END_REPORT) {
	    sys.set_staying_view_mode(0);
	    sys.view_mode=MODE_EVALUATION_LETTER;
	    sys.save_view_mode=MODE_NORMAL;
	}
	else
	    sys.set_staying_view_mode(MODE_YEAR_END_REPORT);
	return 1;
    }

    return 0;
}

//---------- End of function Letter::eval_letter_detect ------------//

//--------- Begin of function Letter::init_eval_letter -----------//
void Letter::init_eval_letter() {
    letter_date = info.game_date;

    eval_letter_show_first_page(1);                 // 1=generate text
    eval_letter_show_poor_area(0, 1);
    eval_letter_show_good_area(0, 1);
    eval_letter_show_mixed_area(0,1);
}

//--------- End of function Letter::init_eval_letter -----------//

//--------- Begin of function Letter::init_welcome_letter -----------//
void Letter::init_welcome_letter() {
    letter_date = info.game_date;

    // page 1,2
    for(int i=1;i<=department_array.size();i++) {
	if(!department_array.is_deleted(i)) {
	    // fix in ver 2
	    //department_array_faculty_array_faculty_count[i]=department_array[i]->faculty_array.faculty_count;
	    //department_array_student_array_student_count[i]=department_array[i]->student_array.student_count;
	    //## chea 221199
	    //			department_array_total_research_dollar[i]=department_array[i]->total_research_dollar;
	    //department_array_total_research_dollar[i]=department_array[i]->total_research_dollar/1000;

	    department_array_faculty_array_faculty_count[i-1]=department_array[i]->faculty_array.faculty_count;
	    department_array_student_array_student_count[i-1]=department_array[i]->student_array.student_count;
	    department_array_total_research_dollar[i-1]=department_array[i]->total_research_dollar/1000;
	}
    }
    department_array_faculty_level_history_FT=department_array.faculty_level_history[FACULTY_RANK_LEVEL_COUNT][THIS_YEAR];
    department_res_general_dept_student_level_historyMT=department_res.general_dept.student_level_history[MAX_STUDENT_LEVEL][THIS_YEAR];
    enroll_res_total_student_count=enroll_res.total_student_count;

    // page 3
    student_office_percent_demanding_places_sl1=student_office.percent_demanding_places_sl1;
    school_res_player_peer_school_percent_get_bacc_in_5_year=school_res.player_peer_school->percent_get_bacc_in_5_year;
    enroll_res_applications_rate_UG_TRADITION=enroll_res.applications_rate[UG_TRADITION];
    enroll_res_student_count_UG_TRADITION=enroll_res.student_count[UG_TRADITION];
    enroll_res_student_count_UG_NONTRADITION=enroll_res.student_count[UG_NONTRADITION];
    enroll_res_student_count_DISTANCE_LEARN=enroll_res.student_count[DISTANCE_LEARN];
    enroll_res_student_count_MASTER=enroll_res.student_count[MASTER];
    enroll_res_student_count_DOCTOR=enroll_res.student_count[DOCTOR];
    department_res_general_dept_student_array_student_count=department_res.general_dept.student_array.student_count;
    enroll_res_overall_percent_minority=enroll_res.overall_percent_minority;
    enroll_res_overall_percent_female=enroll_res.overall_percent_female;

    // page 4,5
    department_array_faculty_count_on_tenure_line=department_array.faculty_count_on_tenure_line;
    department_array_faculty_count_tenured=department_array.faculty_count_tenured;

    department_array_faculty_gender_history_MINORITY_MALE_HISTORY_YEAR_COUNT_1
	=department_array.faculty_gender_history[MINORITY_MALE][HISTORY_YEAR_COUNT-1];

    department_array_faculty_gender_history_MINORITY_FEMALE_HISTORY_YEAR_COUNT_1
	=department_array.faculty_gender_history[MINORITY_FEMALE][HISTORY_YEAR_COUNT-1];

    department_array_faculty_gender_history_GENDER_ETHNIC_TYPE_COUNT_N_TOTAL_1_HISTORY_YEAR_COUNT_1
	=department_array.faculty_gender_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1][HISTORY_YEAR_COUNT-1];

    department_array_faculty_gender_history_NONMINORITY_FEMALE_HISTORY_YEAR_COUNT_1
	=department_array.faculty_gender_history[NONMINORITY_FEMALE][HISTORY_YEAR_COUNT-1];

    department_array_faculty_gender_history_GENDER_ETHNIC_TYPE_COUNT_N_TOTAL_1_HISTORY_YEAR_COUNT_1
	=department_array.faculty_gender_history[GENDER_ETHNIC_TYPE_COUNT_N_TOTAL-1][HISTORY_YEAR_COUNT-1];

    player_school_athletic_program_intensity
	=player_school.athletic_program_intensity;

    Athletics *off = &athletics_office;
    off_basketball_probability_win=off->basketball_probability_win;
    off_basketball_game_count_off_ncaa_level_input=off->basketball_game_count[off->ncaa_level_input];
    off_football_probability_win=off->football_probability_win;
    off_football_game_count_off_ncaa_level_input=off->football_game_count[off->ncaa_level_input];
    finance_total_expense_total=finance.total_expense.total;
    finance_last_year_surplus_deficit=finance.last_year.surplus_deficit;
    //finance_last_year_surplus_deficit=finance.this_year.surplus_deficit;  //## chea 160899 show this year surplus
}

//--------- End of function Letter::init_welcome_letter -----------//

// --- Begin of function Letter::init_state_report --- //
//
void Letter::init_state_report() {
    if ( finance.revenue_policy_array[0].upper_bound == 5 )
	max_state_page_number = 2;
    else
	max_state_page_number = 3;
}

// --- End of function Letter::init_state_report --- //

// --- Begin of function Letter::state_report --- //
//
void Letter::state_report(int refreshFlag) {
    int tab1=REPORT_X1,tab2=REPORT_X1+70,
	tab3=REPORT_X1+100,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+20,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    int list_tab1=185;
    int list_tab2=230;
    int list_tab3=240;
    int list_tab4=250;
    int list_tab5=580;
    int list_tab6=660;
    int list_start_y=230;
    int list_line_space=25;

    vga.use_back();

    init_state_report();

    if (refreshFlag == INFO_REPAINT) {
	if(letter_report_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!letter_report_init_flag) {
	    sys.set_speed(0);
	    user_interface.bg_img(BG_PIC_ID, &vga_back);

	    font_letter.center_put(REPORT_X1+50,REPORT_Y1,REPORT_X1+680,REPORT_Y1+20,"State Appropriation Report");

	    letter_report_init_flag = 1;

	    button_return.create_text( 180, vtab4+7,280,vtab4+28,"Back" );
	    button_back.create_text(350,vtab4+7,450,vtab4+28,"Proceed");
	    button_pgup.create_text(570,vtab4-10,660,vtab4+13,"Prev. page");
	    button_pgdn.create_text(570,vtab4+15,660,vtab4+38,"Next page");
	}
    }
    else {
	vga.use_back();
    }

    if((state_page_number!=old_state_page_number)||(refreshFlag == INFO_REPAINT)) {
	old_state_page_number=state_page_number;
	String context;

	short x1 = list_tab1;
	short x2 = list_tab2;
	short x3 = list_tab3;
	short x4 = list_tab4;
	short x5 = list_tab5;
	short x6 = list_tab6;
	short y = list_start_y;

	if(state_page_number==1) {
	    context  = "To      :      President ";
	    context += player_school.player_last_name;
	    context += "\n";

	    context += "From   :        ";
	    context += "State Higher Education Coordinating Board";
	    context += "\n";

	    context += "\n";
	    context += "The Coordinating Board is pleased to inform  you of its funding decisions for the coming year.";
	    context += "\n";

	    font_letter.put_paragraph(185,120,670,420,context);

	    font_letter.right_put(x5,y,"Current");
	    font_letter.right_put(x6,y,"Coming");

	    y += 15;

	    font_letter.right_put(x5,y,"year");
	    Vga::active_buf->line(x5-26,y+17,x5,y+17,V_BLACK);
	    font_letter.right_put(x6,y,"year");
	    Vga::active_buf->line(x6-26,y+17,x6,y+17,V_BLACK);

	    y += list_line_space;

	    font_letter.put(x1,y,"Operating Funds");

	    y += list_line_space;

	    font_letter.put(x2,y,"Base appropriation");
	    font_letter.right_put(x5,y,m.format(int(finance.base_appropriation[0])));
	    font_letter.right_put(x6,y,m.format(int(finance.base_appropriation[1])));

	    y += list_line_space+12;

	    font_letter.put(x2,y,"Performance funding based on:");

	    y += list_line_space;

	    font_letter.put(x3,y,"Enrollment");
	    font_letter.right_put(x5,y,m.format(int(finance.enroll_appropriation[0])));
	    font_letter.right_put(x6,y,m.format(int(finance.enroll_appropriation[1])));
	}
	else
	    if ( state_page_number == 2 ) {
		// Page 2
		y = 140;
		font_letter.put(x3,y,"Performance indicators");
		font_letter.right_put(x5,y,m.format(int(finance.performance_appropriation[0])));
		font_letter.right_put(x6,y,m.format(int(finance.performance_appropriation[1])));

		y += list_line_space;

		font_letter.put(x4,y,"(degrees granted, student diversity,");
		y += list_line_space;
		font_letter.put(x4,y," course denials, and class size)");

		y += list_line_space;

		font_letter.put(x3,y,"Education Quality Audit*");
		font_letter.right_put(x5,y,m.format(int(finance.education_quality_appropriation[0])));
		font_letter.right_put(x6,y,m.format(int(finance.education_quality_appropriation[1])));

		y += list_line_space+8;

		font_letter.put(x2,y,"Total state appropriation");
		font_letter.right_put(x5,y,m.format(int(finance.total_appropriation[0])));
		font_letter.right_put(x6,y,m.format(int(finance.total_appropriation[1])));

		y += list_line_space;

		context = "*The Education Quality Audit gives you a grade of ";
		int result = player_school.think_protagonist_event(
		    player_school.sub_score[S_EDUCATIONAL_QUALITY][HISTORY_MONTH_COUNT-1],
		    player_school.sub_score[S_EDUCATIONAL_QUALITY][HISTORY_MONTH_COUNT-1-12],
		    player_school.sub_score_start[S_EDUCATIONAL_QUALITY],
		    player_school.get_protagonist_info(5),
		    -1, 1 );

		switch(result) {
		case 1:   context+="Outstanding"; break;
		case 2:   context+="Strong";    break;
		case 3:   context+="Weak";      break;
		case 4:   context+="Unacceptable";break;
		case 5:
		case 6:   context+="Mixed";     break;
		default:    context+="Satisfactory";
		}

		context += ".";
		font_letter.put(x1,y,context);

		y += list_line_space+10;

		font_letter.put(x1,y,"Supplementary Capital Funds");

		y += list_line_space;

		font_letter.put(x2,y,"Additions to the Facilities Reserve");
		font_letter.right_put(x5,y,m.format(int(finance.facility_reserve_appropriation[0]),3));
		font_letter.right_put(x6,y,m.format(int(finance.facility_reserve_appropriation[1]),3));
	    }
	    else {
		// Page 3 ( Just show if there is Tuition Rate Limit )

		y = 140;

		font_letter.put(x1,y,"Tuition rate limits");

		y += list_line_space;

		context = "In addition, we wish to inform you that the State Legislature has limited tuition growth for the coming year to ";
		context += m.format(finance.revenue_policy_array[0].upper_bound);
		context += " percent. Your university's tuition increase will not be allowed to exceed this figure.";

		font_letter.put_paragraph(x1,y,x6,y+100,context);
	    }

	String pagestr;
	pagestr = state_page_number;
	pagestr += "/";
	pagestr += max_state_page_number;
	font_letter.put(680,vtab4+20,pagestr.str_buf);

	button_back.paint();
	button_return.paint();
	if(state_page_number<=1)button_pgup.disable();
	if(state_page_number>=max_state_page_number)button_pgdn.disable();

	button_pgup.paint();
	button_pgdn.paint();

	if(refreshFlag != INFO_REPAINT)
	    vga.blt_buf(185,140,670,420);
    }
    //------- blt the back buffer to front buffer -------//

    if(refreshFlag == INFO_REPAINT)
	vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

    vga.use_front();
}

//
// --- End of function Letter::state_report --- //

// --- Begin of function Letter::state_report_detect --- //
//
int Letter::state_report_detect() {
    if (!letter_report_init_flag)
	return 0;
    vga.use_front();

    //--- recalculate the number of pages it has ----//

    if(state_page_number<max_state_page_number)
	if(button_pgdn.detect()) {
	    if(state_page_number<max_state_page_number)
		state_page_number++;
	    sys.redraw_all_flag=1;
	};

    if(state_page_number>1)
	if(button_pgup.detect()) {
	    if(state_page_number>1)
		state_page_number--;
	    sys.redraw_all_flag=1;
	};

    // for mail button
    if(button_back.detect()) {
	state_page_number=1;
	old_state_page_number=-1;
	welcome_page_number=1;
	old_welcome_page_number=-1;
	if(sys.save_view_mode!=MODE_YEAR_END_REPORT) {
	    player_school.is_year_end_report = 1;
	    player_school.current_report_type = 3;      // (REPORT_TYPE_TURNOVER)
	    sys.set_staying_view_mode(0);
	    sys.set_speed(sys.save_speed);
	    sys.set_view_mode(MODE_NORMAL);
	    sys.save_view_mode=MODE_NORMAL;
	}
	else {
	    player_school.is_year_end_report = 1;
	    player_school.current_report_type = 3;      // (REPORT_TYPE_TURNOVER)
	    sys.set_view_mode(MODE_YEAR_END_REPORT);
	    //			sys.set_staying_view_mode(MODE_YEAR_END_REPORT);
	}
	return 2;
    }

    if ( button_return.detect() ) {
	sys.set_view_mode( MODE_EVALUATION_LETTER );
	//		sys.save_view_mode = MODE_YEAR_END_REPORT;
	return 1;
    }

    if(m.upper(mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_UNIQUE_KEY))=='W') {
	state_page_number=1;
	old_state_page_number=-1;
	if(sys.save_view_mode!=MODE_YEAR_END_REPORT) {
	    sys.set_staying_view_mode(0);
	    sys.view_mode=MODE_EVALUATION_LETTER;
	    sys.save_view_mode=MODE_NORMAL;
	}
	else
	    sys.set_staying_view_mode(MODE_YEAR_END_REPORT);
	return 0;
    }

    return 0;
}

//
// --- End of function Letter::state_report_detect --- //

// --- Begin of function Letter::get_variable_score --- //
//
void Letter::get_variables_score(char ascend) {

    float strength;
    float consistency;
    float k1;

    for ( int subScoreType=0; subScoreType<SCORE_VAR_COUNT; subScoreType++ ) {
	float curValue = player_school.sub_score[subScoreType][HISTORY_MONTH_COUNT-1];
	float lastYearValue = player_school.sub_score[subScoreType][HISTORY_MONTH_COUNT-13];
	float initialValue = player_school.sub_score_start[subScoreType];

	player_school.calc_strength_consistency_k1(curValue, lastYearValue,initialValue,
						   player_school.get_protagonist_info(2+subScoreType),strength, consistency, k1);

	sort_data[subScoreType].idx = subScoreType;
	sort_data[subScoreType].strength = strength;
	sort_data[subScoreType].consistency = consistency;
	sort_data[subScoreType].k1 = k1;

	//		sort_data[subScoreType].tag = 0;
    }

    if ( ascend )
	qsort(sort_data,(size_t)SCORE_VAR_COUNT,sizeof(struct SortData),compare_ascend);
    else
	qsort(sort_data,(size_t)SCORE_VAR_COUNT,sizeof(struct SortData),compare_descend);
}

//
// --- End of function Letter::get_variable_score --- //
