//Filename    : OTXTLIST.CPP
//Description : Object text list
// owner			: chwg 1015
#include <OVGA.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OINFO.H>
#include <OMOUSE.H>
#include <OVBROW98.H>
#include <OSCROLL.H>
#include <OTXTLIST.H>
#include <OIFACE.H>

//-------- Define macro constant ----------//

#define SCROLL_BAR_WIDTH 15

#define TEXT_LINE_SPACE   6
#define TEXT_MARGIN       20

//--------- Define static varaibles --------//

static ScrollBar scroll_bar;
static TxtList*  cur_txt_list_ptr;                // for static function
static VBrowse98 topic_browse;
// ##### begin Gilbert 19/04/2001 ###### //
static short * txtlist_refresh_bitmap = NULL;
// ##### end Gilbert 19/04/2001 ###### //
static void disp_topic_rec(int,int,int,int);
static void disp_idx_topic_rec(int,int,int,int);

//--------- Begin of function TxtList::TxtList ---------//

TxtList::TxtList() {
    init_flag      = 0;
    init_pict_flag = 0;
    init_list_flag = 0;
    history_pos = 0;
    can_back=0;
    top_line=1;
    memset( history_array, 0, sizeof(history_array[0]) * HISTORY_SIZE );
}

//----------- End of function TxtList::TxtList ---------//

//---------- Begin of function TxtList::init -------//
//!
//! The text list is optional, if it hasn't initialized,
//! it won't be displayed.
//!
//! <int>   x1, y1, x2, y2 = the position of the text list
//! <char*> resTxtName     = name of the text resource file
//!
void TxtList::init(int x1, int y1, int x2, int y2, char* resTxtName) {
    text_x1 = x1;
    text_y1 = y1;
    text_x2 = x2-SCROLL_BAR_WIDTH-4;                // space + scroll bar width
    text_y2 = y2;

    scroll_x1 = x2-SCROLL_BAR_WIDTH+1;
    scroll_y1 = y1;
    scroll_x2 = x2;
    scroll_y2 = y2;

    res_txt.init( resTxtName, 0 );                  // 0-don't always open the file, only open it during reading, save file handle

    if(topic_id < 1)
	topic_id=1;                                   // default topic id.

    init_flag = 1;
}

//---------- End of function TxtList::init -------//

//-------- Begin of function TxtList::deinit -------//
//!
void TxtList::deinit() {
    res_txt .deinit();

    res_pict.deinit();
    if(txtlist_refresh_bitmap)
	mem_del(txtlist_refresh_bitmap);
    txtlist_refresh_bitmap=NULL;
}

//---------- End of function TxtList::deinit -------//

//---------- Begin of function TxtList::init_pict -------//
//!
//! <int>   x1, y1, x2, y2 = the position of the text list
//! <char*> resPictName    = name of the picture resource file attached
//!                          to the text
//!
void TxtList::init_pict(int x1, int y1, int x2, int y2, char* resPictName) {
    pict_x1 = x1;
    pict_y1 = y1;
    pict_x2 = x2;
    pict_y2 = y2;

    res_pict.init( resPictName, 0 );

    init_pict_flag = 1;
}

//---------- End of function TxtList::init_pict -------//

//---------- Begin of function TxtList::init_list -------//
//!
//! <int> x1, y1, x2, y2 = the position of the topic list list
//!
void TxtList::init_list(int x1, int y1, int x2, int y2) {
    list_x1 = x1;
    list_y1 = y1;
    list_x2 = x2;
    list_y2 = y2;

    init_list_flag = 1;
    top_line=1;
}

//---------- End of function TxtList::init_list -------//

//------- Begin of function TxtList::disp -------//
//!
//! <int> refreshFlag = INFO_REPAINT or INFO_UPDATE
//! [int] topicId     = the id. of the topic requested to display
//!                     (default : current topic id.)
//!
void TxtList::disp(int refreshFlag, int topicId) {
    vga.use_back();
    err_when( !init_flag );

    //-------- display a new topic ----------//

    char* textPtr;

    if( topicId )
	topic_id = topicId;

    //------------ paint panel --------------//
    if(init_list_flag) {
	if( refreshFlag==INFO_REPAINT ) {
	    user_interface.brighten( list_x1, list_y1, list_x2, list_y2 );
	    user_interface.rect( list_x1, list_y1, list_x2, list_y2 );
	    //			if( init_pict_flag )
	    //				vga.active_buf->d3_panel_down( pict_x1, pict_y1, pict_x2, pict_y2, 2, 0 );
	    cur_txt_list_ptr = this;
	    // for static rec display function's reference
	    topic_browse.init( list_x1, list_y1, list_x2, list_y2,
			       0, 20, res_txt.rec_count, disp_topic_rec );
	    topic_browse.open(topic_id);
	}
	//		topic_browse.update();
    }

    if(!init_list_flag) {
	if( refreshFlag==INFO_REPAINT ) {
	    user_interface.rect( text_x1, text_y1, text_x2, text_y2 );
	    user_interface.brighten( text_x1, text_y1, text_x2, text_y2 );

	    if( init_pict_flag )
		vga.active_buf->d3_panel_down( pict_x1, pict_y1, pict_x2, pict_y2, 2, 0 );
	    cur_txt_list_ptr = this;                    // for static rec display function's reference

	    txtlist_refresh_bitmap=vga_back.save_area(
		text_x1,text_y1,text_x2,text_y2,
		txtlist_refresh_bitmap);
	}
	//------------ retrieve topic ------------//
	vga_back.rest_area(txtlist_refresh_bitmap,0,1);

	if(topicId && refreshFlag==INFO_REPAINT ) {   // topicId can be NULL, while INFO_REPAINT
	    //			top_line=1;
	    textPtr = res_txt.get_body(topic_id);

	    //-------- count the no. of lines of the text --------//

	    int dispLines;                              // no. of lines can be displayed on the area
	    int totalLines;                             // total no. of lines of the text

	    font_letter.count_line( text_x1+TEXT_MARGIN, text_y1+TEXT_MARGIN,
				    text_x2-TEXT_MARGIN+2, text_y2-TEXT_MARGIN,
				    textPtr, TEXT_LINE_SPACE, dispLines, totalLines );

	    //-------- display scroll bar ------//

	    scroll_bar.init( 1, scroll_x1, scroll_y1, scroll_x2, scroll_y2,
			     dispLines, dispLines, totalLines );

	    scroll_bar.paint();
	    scroll_bar.refresh(top_line,1);
	}
	else {                                        // redisplay current topic, only scroll to a different part of the text
	    textPtr = res_txt.get_body(topic_id);
	}
	font_letter.put_paragraph( text_x1+TEXT_MARGIN, text_y1+TEXT_MARGIN,
				   text_x2-TEXT_MARGIN+2, text_y2-TEXT_MARGIN,
				   textPtr, TEXT_LINE_SPACE, top_line );

	if( init_pict_flag ) {
	    char* pictName = res_txt.get_pict_name(topic_id);

	    if( pictName[0] ) {
		char* pictPtr = res_pict.read(pictName);

		if( pictPtr )
		    vga.active_buf->put_bitmap( pict_x1+2, pict_y1+2, pictPtr );
	    }
	}

	vga.blt_buf( text_x1, text_y1, text_x2, text_y2 );
    }

    //--------- display text body ----------//
}

//-------- End of function TxtList::disp -------//

//-------- Begin of function TxtList::add_history -----------//
//!
//! Add a topic id. to the topic history
//!
void TxtList::add_history(int topicId) {
    /*
      can_back=(history_array[history_pos]>0);
      if( ++history_pos >= HISTORY_SIZE )
      history_pos = 0;
      history_array[history_pos] = topicId;
    */

    int next_pos=((history_pos+1)>=HISTORY_SIZE)?0:history_pos+1;
    int prev_pos=((history_pos-1)<0)?HISTORY_SIZE-1:history_pos-1;

    // do nothing if same history idx is added.
    // chwg ##1215
    if(history_array[prev_pos]==topicId)
	return;

    if(history_array[history_pos]!=0) {
	history_array[next_pos]=0;
    }

    history_array[history_pos] = topicId;
    can_back=(history_array[(((prev_pos)<0)?HISTORY_SIZE-1:prev_pos)]>0);;
    history_pos = next_pos;
}

//----------- End of function TxtList::add_history -----------//

//-------- Begin of function TxtList::back_history -----------//
//!
//! Backward to previous topic in the history
//!
void TxtList::back_history() {
    /*
      history_array[history_pos]=0;	// the history just added is useless
      if((history_pos-1) < 0 )
      history_pos = HISTORY_SIZE;

      int topicId = history_array[--history_pos];

      if( topicId>0 )        // if topicId==0, means no history
      {
      history_array[history_pos] = 0;
      can_back=(history_array[(((history_pos-1)<0)?HISTORY_SIZE-1:history_pos-1)]>0);
      disp( INFO_UPDATE, topicId );

      if( init_list_flag )
      topic_browse.refresh( topicId );
      }
    */
    int prev_pos=history_pos;

    do {
	prev_pos--;
	prev_pos=(prev_pos<0)?HISTORY_SIZE-1:prev_pos;
    }
    while(history_array[prev_pos]==topic_id && history_array[prev_pos]!=0);

    int topicId = history_array[prev_pos];
    if( topicId>0 ) {                               // if topicId==0, means no history
	history_array[prev_pos] = 0;
	history_pos = prev_pos;
	disp( INFO_UPDATE, topicId );
	if( init_list_flag )
	    topic_browse.refresh( topicId );
	can_back=(history_array[(((prev_pos-1)<0)?HISTORY_SIZE-1:prev_pos-1)]>0);
    }
}

//----------- End of function TxtList::back_history -----------//

//------- Begin of function TxtList::detect -------//
//!
//! Return : <int> 1 - detected
//!                0 - not detected
//!
int TxtList::detect() {
    int rc;

    //------- detect topic list list ----------//
    //	vga.use_front();
    // select new topic
    if( init_list_flag && (rc=topic_browse.detect()) > 0 ) {
	double_clicked=topic_browse.double_click;
	topic_id=rc;
	sys.redraw_all_flag=1;
	//		disp(INFO_UPDATE, rc);
	return 1;
    }

    if(!init_list_flag) {
	if( (rc=scroll_bar.detect()) > 0 ) {
	    //-------- detect text scroll bar ----------//
	    top_line = rc;
	    sys.redraw_all_flag=1;
	    disp(INFO_UPDATE);
	    return 1;
	    //------- detect on text body ---------//
	}
	if( detect_text() )
	    return 1;
    }
    return 0;

}

//------- Begin of function TxtList::detect_text -------//
//!
//! Return : <int> 1 - detected
//!                0 - not detected
//!
int TxtList::detect_text() {
    //------- if mouse pressed on text area ------//
    //-- can pressed on hyper-texted field or just fast scrolling --//

    if( !mouse.single_click( text_x1, text_y1, text_x2, text_y2 ) )
	return 0;

    //---- detect hyper-texted field ------//

    int         i, topicId;
    char        saveChar;
    HyperField* hyperField = font_letter.hyper_field_array;

    for( i=0 ; i<font_letter.hyper_field_count ; i++, hyperField++ ) {
	if( mouse.click_x() >= hyperField->x1 &&
	    mouse.click_x() <= hyperField->x2 &&
	    mouse.click_y() >= hyperField->y1 &&
	    mouse.click_y() <= hyperField->y2 ) {
	    err_when( hyperField->text_len > TxtIndex::TITLE_LEN );

	    saveChar = hyperField->text_ptr[hyperField->text_len];
	    hyperField->text_ptr[hyperField->text_len] = NULL;

	    topicId = res_txt.locate_topic(hyperField->text_ptr);

	    hyperField->text_ptr[hyperField->text_len] = saveChar;

	    if( topicId ) {
		add_history(topic_id);                    // add current topic id to the history

		disp(INFO_UPDATE, topicId);
		sys.redraw_all_flag=1;
		if( init_list_flag )
		    topic_browse.refresh( topicId );

		return 1;
	    }
	}
    }

    //------ detect fast scrolling --------//

    if( mouse.click_y() < text_y1 + (text_y2-text_y1) / 2 )
	top_line = scroll_bar.page_up(1);             // page up, 1 = skip less one line, so when scrolling text, want the last line of previous page be the first line of current page, then pass 1 as skipLess
    else
	top_line = scroll_bar.page_down(1);           // page down

    disp(INFO_UPDATE);

    return 1;
}

//-------- Begin of function TxtList::detect_text -------//

//-------- Begin of static function disp_topic_rec -----------//
//
static void disp_topic_rec(int recNo, int x, int y, int refreshFlag) {
    if( refreshFlag == INFO_REPAINT )
	font_letter.put( x+10, y, cur_txt_list_ptr->res_txt.get_title(recNo) );
}

//----------- End of static function disp_topic_rec -----------//
