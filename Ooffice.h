//Filename    : OOffice.h
//Description : Office Class Declaration
//Owner       : Fred

#ifndef __OOffice_H
#define __OOffice_H

// ES library header file
// CU header file

//----------- Define constants -------------//

//----------- Define enum types -------------//
// for yearly time-series graph
enum {
    HISTORY_YEAR_COUNT  = 8,
    HISTORY_YEAR_COUNT2 = HISTORY_YEAR_COUNT * 2,

    THIS_YEAR = HISTORY_YEAR_COUNT-1,

    OFFICE_REVENUE = HISTORY_YEAR_COUNT - 1,
    OFFICE_EXPENSE = HISTORY_YEAR_COUNT2 -1,

    FIRST_HISTORY = OFFICE_REVENUE,
    SECOND_HISTORY = OFFICE_EXPENSE,
};

enum {
    HISTORY_MONTH_COUNT = 36,

    THIS_MONTH = HISTORY_MONTH_COUNT-1,
};

enum {
    HISTORY_TRIMESTER_COUNT = 24,

    THIS_TRIMESTER = HISTORY_TRIMESTER_COUNT-1,
};

enum {
    UPDATE_MONTH, UPDATE_TRIMESTER, UPDATE_YEAR, UPDATE_ALL,
};

//----------- Define struct -------------//
#define shift_history(hArr, hCount) \ 
{ \
      int h;\
		for ( h=0; h<hCount-1; h++)\ 
		    { \
			  hArr[h] = hArr[h+1];\
						  }\
						       }\
//

/*
//----------- Define class Office -----//
//!
class Office
{
private:

  //------------------------------------------//

public:
  Office();
~Office();
void	init();
void	deinit();
int	getId();
//------------------------------------------//

private:

};
*/
#endif                                            //ifndef
