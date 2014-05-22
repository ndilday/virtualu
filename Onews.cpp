//Filename    : ONEWS_IF.CPP
//Description : Object NewsArray; including sys, font related functions.
//Owner			:Fred

#include <OFONT.H>
#include <OINFO.H>
#include <OFINANCE.H>

#include <ONEWS.H>

//------- define screen coordinations -------//
//--------- define static vars --------------//

//------ Begin of function NewsArray::NewsArray -----//

NewsArray::NewsArray() : DynArray(sizeof(News), 100) {
}

//-------- End of function NewsArray::NewsArray -----//

//------ Begin of function NewsArray::init -----//
//!
void NewsArray::init() {
    reset();
}

//------- End of function NewsArray::init -----//

//------ Begin of function NewsArray::deinit -----//
//!
void NewsArray::deinit() {
    reset();
}

//------- End of function NewsArray::deinit -----//

//------ Begin of function NewsArray::reset -----//
//!
//! Reset all news display options and clear all news in the log
//!
void NewsArray::reset() {
    zap();                                          // clear all news in the log

    last_clear_recno = 0;
    news_add_flag    = 1;

    default_setting();
}

//------- End of function NewsArray::reset -----//

//------ Begin of function NewsArray::default_setting -----//
//!
//! Reset all news display options
//!
void NewsArray::default_setting() {
    news_type_option[NEWS_NORMAL]=1;

    news_who_option = NEWS_DISP_ALL;                // default display news of groups controlled by the player
}

//------- End of function NewsArray::default_setting -----//

//------ Begin of function NewsArray::remove -----//
//!
//! Remove a specific news from the news_array.
//!
//! <int> newsId     = id. of the news
//! <int> shortPara1 = short_para1 of the news
//!
void NewsArray::remove(int newsId, int shortPara1) {
    News* newsPtr;

    for( int i=size() ; i>0 ; i-- ) {
	newsPtr = news_array[i];

	if( newsPtr->id == newsId &&
	    newsPtr->short_para1 == shortPara1 ) {
	    linkout(i);

	    //### begin alex 31/3 ###//
	    //if( i<last_clear_recno && last_clear_recno > 1 )
	    if( i<=last_clear_recno && last_clear_recno > 0 )
		last_clear_recno--;
	    //#### end alex 31/3 ####//

	    break;
	}
    }
}

//------- End of function NewsArray::remove -----//

//------- Begin of function NewsArray::operator[] -----//

News* NewsArray::operator[](int recNo) {
#ifdef DEBUG

    News* msgPtr;

    if( recNo )
	msgPtr = (News*) get(recNo);
    else
	msgPtr = NULL;                                // NULL will then cause error

    err_if( !msgPtr || msgPtr->news_date==0 )
	err_now( "NewsArray[] is deleted" );

    return msgPtr;

#else

    return (News*) get(recNo);
#endif
}

//--------- End of function NewsArray::operator[] ----//
