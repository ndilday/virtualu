//Filename    : ODATE.H
//Description : Header file of object date information

#ifndef __ODATE_H
#define __ODATE_H

//#ifndef __ALL_H
//#include <ALL.H>
//#endif
#ifndef __WINDOWS_
#include <windows.h>
#endif

//!Utility class with functions for getting day, month, and year
//!information.
class DateInfo {
public:

    long  julian(int,int,int);
    long  julian(char*);

    char* date_str(long,int=0,int=1);
    char* month_str(int);

    int   year(long julianDate)  { return get_date(julianDate,'Y'); }
    int   month(long julianDate) { return get_date(julianDate,'M'); }
    int   day(long julianDate)   { return get_date(julianDate,'D'); }

    char* time_str(int);
    int   days_in_month(int);
    int   day_year(int,int,int);

    int   add_months(int inDate, int addMonth);

    int file_time_to_julian(FILETIME& fileTime);
    int  get_date(long,char);

private:
    long ytoj(int);
    int  month_day(int,int,int&,int&);
    //   int  get_date(long,char);
};

extern DateInfo date;
#endif
