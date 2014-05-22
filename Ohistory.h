//Filename		: OHistory.h
//Description	: Constants for Finance Class Declaration;
//						For inflation rate, interest rate, etc, in HE.response_func->Time_Variation
//Ower		   : Fred

#ifndef __OHISTORY_H
#define __OHISTORY_H

enum {
    FA_HISTORY_YEAR = 12,
    //FA_THIS_MONTH = FA_HISTORY_YEAR-1,
};

//!Class which keeps historical contants (inflation rate, interest rate,
//!etc) used for finance class declaration .
struct HistoryData {
    // init(char type=FA_HISTORY_YEAR);
    int   length;
    float   data[FA_HISTORY_YEAR];                  //temp

    HistoryData() {
	memset(this, 0, sizeof(HistoryData));
	length = FA_HISTORY_YEAR;
    }

    void    init(int len) {
	memset(this, 0, sizeof(HistoryData));
	length = FA_HISTORY_YEAR;
    };

    void    init(int len, float value) {
	init(len);
	for ( int h=0; h<length; h++)
	    data[h] = value;
    };

    void    set_cur(float value)        { data[length-1] = value; };

    float   get_cur()             { return data[length-1]; };
    float   oldest_value()            { return data[0]; };

    float   get(int relativeIndex) {                // 0: latest/current data
	return data[length-1+relativeIndex];
    };

    void    shift_hist() {
	for ( int h=0; h<length-1; h++) {
	    data[h] = data[h+1];
	}
	data[length-1] = 0;
    };

    float   get_sum() {
	float sum;  int h;
	for ( sum=0, h=0; h<length; h++) {
	    sum += data[h];
	}
	return sum;
    }
};
#endif
