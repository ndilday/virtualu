//Filename    : GAMEDEF.H
//Description : Maximum game constant

#ifndef __GAMEDEF_H
#define __GAMEDEF_H

//----------- define game version ------------//

extern char *GAME_VERSION_STR;                    // change from #define to extern
#define GAME_VERSION       201                    // Version 1.00, don't change it unless the format of save game files has been changed

//-------- Define constant ------------//

#define APP_NAME           "CCampus"              // application name
#define WIN_CLASS_NAME     "Virtual U"            // for registering window class
#define WIN_TITLE          "Virtual U"            // window title

//---------- Define constant ---------//

enum { INFO_REPAINT=10, INFO_UPDATE, INFO_SPECIAL };

//------- Data sub-directories -------//

#define DIR_RES         "RESOURCE\\"
#define DIR_SOUND       "SOUND\\"
#define DIR_MOVIE       sys.dir_movie
#define DIR_TUTORIAL    sys.dir_tutorial
#define DIR_SCENARIO    sys.dir_scenario

//-------- define constant --------//

#define MAX_ZOOM_LEVEL  3

enum { ZOOM_SMALL=0, ZOOM_MEDIUM, ZOOM_LARGE };

//------- Define game constant -------//

#define MAX_DEPARTMENT          12
#define STUDENT_TO_FACULTY_RATIO    14

#define CAN_BE_ANY            -1                  // for global use

#define IF(cond,x,y)    ((cond)?(x):(y))

//------- Define game constant -------//

//##### begin fred 1013 #####//

enum {
    INPUT_LOW, INPUT_MEDIUM, INPUT_HIGH,

    INPUT_OPTION_COUNT,
};                                                // for radio button input

enum { GENDER_ETHNIC_TYPE_COUNT = 4 };

enum { NONMINORITY_MALE, NONMINORITY_FEMALE, MINORITY_MALE, MINORITY_FEMALE };
enum { GENDER_MALE, GENDER_FEMALE };

enum {
    GENDER_ETHNIC_TYPE_COUNT_N_TOTAL = GENDER_ETHNIC_TYPE_COUNT + 1,
    GENDER_ETHNIC_TYPE_TOTAL = GENDER_ETHNIC_TYPE_COUNT,
};
//##### end fred 1013 #####//

enum { TRIMESTER_PER_YEAR = 3 };

enum { AUTUMN, WINTER, SUMMER };

enum {                                            // The first and the last trimesters in a year
    FIRST_TRIMESTER=AUTUMN,
    LAST_TRIMESTER=SUMMER
};

enum {                                            // the order is VERY important
    PRIVATE_SCHOOL, LIBERAL_ARTS_SCHOOL, PUBLIC_SCHOOL, COMPREHENSIVE_SCHOOL,
    INSTITUTIONAL_TYPE_COUNT, DUMMY_SCHOOL,
};

//##### begin fred 0820 #####//
//----------- Define enum types -------------//
// for yearly time-series graph
enum {
    HISTORY_YEAR_COUNT  = 8,

    //------------ for projected graph
    H_PREV_YEAR = HISTORY_YEAR_COUNT-3,
    H_THIS_YEAR = HISTORY_YEAR_COUNT-2,
    H_NEXT_YEAR = HISTORY_YEAR_COUNT-1,

    //------------ for performance indicators graph
    THIS_YEAR = HISTORY_YEAR_COUNT-1,

    //------------ for two-line graph
    HISTORY_YEAR_COUNT2 = HISTORY_YEAR_COUNT * 2,

    OFFICE_REVENUE = HISTORY_YEAR_COUNT - 2,
    OFFICE_EXPENSE = HISTORY_YEAR_COUNT2 - 2,

    //FIRST_HISTORY = OFFICE_REVENUE,			//TO remove these two lines
    //SECOND_HISTORY = OFFICE_EXPENSE,

    FIRST_H_THIS_YEAR = OFFICE_REVENUE,
    SECOND_H_THIS_YEAR = OFFICE_EXPENSE,

};

enum {
    HISTORY_MONTH_COUNT = 48,

    // for projected graph
    H_PREV_MONTH = HISTORY_MONTH_COUNT-3,
    H_THIS_MONTH = HISTORY_MONTH_COUNT-2,
    H_NEXT_MONTH = HISTORY_MONTH_COUNT-1,

    // for performance indicators graph
    THIS_MONTH = HISTORY_MONTH_COUNT-1,
};

enum {
    HISTORY_TRIMESTER_COUNT = 24,

    H_PREV_TRIMESTER = HISTORY_TRIMESTER_COUNT-3,
    H_THIS_TRIMESTER = HISTORY_TRIMESTER_COUNT-2,
    H_NEXT_TRIMESTER = HISTORY_TRIMESTER_COUNT-1,

    // for performance indicators graph
    THIS_TRIMESTER = HISTORY_TRIMESTER_COUNT-1,
};

enum {
    UPDATE_MONTH, UPDATE_TRIMESTER, UPDATE_YEAR, UPDATE_ALL,
};

//----------- Define struct -------------// //## Chea 140799    if (info.prerun_year==0)

#define shift_history(hArr, hCount) \
{ \
      \
    { \
	  \
	  int _h;\
		     for ( _h=0; _h<hCount-1; _h++)\
			 { \
			       hArr[_h] = hArr[_h+1];\
							 }\
							      hArr[hCount-1] = 0;\
										     }\
											  \
											  }

//##### end fred 0820 #####//
#endif
