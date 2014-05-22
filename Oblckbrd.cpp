#include <OSYS.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OGSET.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OFINANCE.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OVBROW98.H>
#include <OIFACE.H>
#include <OPSCHOOL.H>
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
#include <OBLCKBRD.H>
#include <OBUTTON.H>
#include <ONEWS.H>
#include <OTASK.H>
#include <ODATE.H>

enum {                                            // screen coordinates
    REPORT_X1 = ZOOM_X1+15,
    REPORT_Y1 = ZOOM_Y1+25,
    REPORT_X2 = ZOOM_X2-15,
    REPORT_Y2 = ZOOM_Y2-55
};

enum {                                            // the bg image id
    BG_PIC_ID = 5
};

static int blackboard_report_init_flag;

static VBrowse98 blackboard_browser;
static int browser_recno=1;
static int old_browser_rec_count=1;
static int new_browser_rec_count=0;
static void log_disp_rep_row(int,int,int,int);
static int log_detect_rep_row(int,int,int);

static int news_lines,tasks_lines,remained_news_lines,remained_tasks_lines,idx1,idx2;
static int space_for_news,space_for_task;

static ButtonGroup bottom_button_group(2);
static short *bottom_button_up_bitmap[2];
static short *bottom_button_down_bitmap[2];
static char *bottom_button_label[] = {
    "News and Messages", "Objectives"
};

static Button button_back;

static void log_disp_rep_row(int recno, int x, int y,int refreshFlag) {
    int tab1=x+5;
    int tab2=x+20;
    String str;

    if(blackboard.log_mode==NEWS_MODE) {
	if(recno==1) {
	    idx1=1;
	    idx2=1;
	}

	//		if(recno==2)
	//		{	str="News and Messages:";
	//			font_blue_san.put(tab2,y+2,str);
	//		}

	if((recno>=1)&&(recno<=space_for_news)) {
	    char * tmp;
	    if(space_for_news>0) {
		for(int i=1,idx1=1,idx2=1;i<recno;i++) {
		    int j=(news_array[idx1]->line_count);
		    if(idx2>=j) {
			idx2=1;
			idx1++;
		    }
		    else
			idx2++;
		    if(idx1>news_array.size())break;
		}

		if(idx2==0)idx2=1;
		if(idx1<=news_array.size()) {
		    tmp=news_array[idx1]->msg();
		    if(idx2==1) {
			// str=date.date_str(news_array[idx1]->news_date,1,0);
			str = info.date_str_julian( news_array[idx1]->news_date, INFO_DATE_DEFAULT|INFO_DATE_NO_DAY|INFO_DATE_SHORT_MONTH );
			font_chartsm.put(tab2,y+2,str);
		    }
		    str=font_chartsm.get_line(400,idx2,tmp);
		    font_chartsm.put(tab2+75,y+2,str);
		}
		else
		    font_chartsm.put(tab2+75,y+2,"...");
	    }
	}
    }
    else {
	if(recno==1) {
	    idx1=1;
	    idx2=1;
	}

	//		if(recno==2)
	//		{
	//			str="Objectives :";
	//			font_blue_san.put(tab2,y+2,str);
	//		}

	if((recno>=1)&&(recno<=space_for_task)) {
	    char * tmp;
	    if(space_for_task>0) {
		for(int i=1,idx1=1,idx2=1;i<recno;i++) {
		    if(idx2>=(task_array[idx1]->line_count)) {
			idx2=1;
			idx1++;
		    }
		    else
			idx2++;

		    if(idx1>task_array.size())break;
		}

		if(idx2==0)idx2=1;
		if(idx1<=task_array.size()) {
		    tmp=task_array[idx1]->get_task_msg();
		    if(idx2==1) {
			// str=date.date_str(task_array[idx1]->news_date,1,0);
			str=info.date_str_julian( task_array[idx1]->news_date, INFO_DATE_DEFAULT|INFO_DATE_NO_DAY|INFO_DATE_SHORT_MONTH );
			font_chartsm.put(tab2,y+2,str);
		    }
		    str=font_chartsm.get_line(400,idx2,tmp);
		    font_chartsm.put(tab2+75,y+2,str);
		}
		else
		    font_chartsm.put(tab2+75,y+2,"...");
	    }
	}
    }

};

static int log_detect_rep_row(int recno, int x, int y) {
    return 0;
};

//------- Begin of function BlackBoard::BlackBoard -----------//
//!
BlackBoard::BlackBoard() {
    init();
}

//--------- End of function BlackBoard::BlackBoard -----------//

//------- Begin of function BlackBoard::~BlackBoard -----------//
//!
BlackBoard::~BlackBoard() {
    deinit();
}

//--------- End of function BlackBoard::~BlackBoard -----------//

//------- Begin of function BlackBoard::deinit ---------------//
//!
void BlackBoard::deinit() {
    if (*bottom_button_up_bitmap) {
	for (int i = 0; i < 2; i++) {
	    mem_del(bottom_button_up_bitmap[i]);
	    mem_del(bottom_button_down_bitmap[i]);
	    bottom_button_up_bitmap[i] = NULL;
	    bottom_button_down_bitmap[i] = NULL;
	}
    }

    button_back.deinit();
    //--------- deallocate memory -------------//
    blackboard_report_init_flag = 0;
}

//--------- End of function BlackBoard::deinit ---------------//

//------- Begin of function BlackBoard::init -----------------//
//!
void BlackBoard::init() {
    memset(this,0,sizeof(BlackBoard));
    for (int i = 0; i < 2; i++) {
	bottom_button_up_bitmap[i] = NULL;
	bottom_button_down_bitmap[i] = NULL;
    }
    blackboard_report_init_flag = 0;
    log_mode=NEWS_MODE;
}

//--------- End of function BlackBoard::init -----------------//

void BlackBoard::black_message_report(int refreshFlag) {
    int i,j,k;
    vga.use_back();
    vga_back.bar(260, 515, 620, 590, V_BLACK );

    for(i=3,j=news_array.size();i>=0;j--) {
	if(j<=0){i=-1;continue;}
	news_array[j]->line_count=font_fblack.count_line(260,news_array[j]->msg());
	for(k=news_array[j]->line_count;k>0;k--) {
	    char *print_str;
	    if(i>=0) {
		//					print_str=font_fblack.get_line(260,k,news_array[j]->msg());
		//## chea 240999
		print_str=font_fblack.get_line(255,k,news_array[j]->msg());
		font_fblack.put(320,520+i*18,print_str);
		if(k==1) {
		    // print_str=date.date_str(news_array[j]->news_date,1,0);
		    print_str=info.date_str_julian( news_array[j]->news_date, INFO_DATE_DEFAULT|INFO_DATE_NO_DAY|INFO_DATE_SHORT_MONTH );
		    font_fblack.put(265,520+i*18,print_str);
		}
		i--;
	    }
	}
    }
    //		log_mode=NEWS_MODE;
    vga.blt_buf(260, 515, 620, 590);
};

void BlackBoard::black_objective_report(int refreshFlag) {
    int i,j,k=0;
    vga.use_back();
    vga_back.bar(260, 515, 620, 590, V_BLACK );

    for(i=3,j=task_array.size();i>=0;j--) {
	if(j<=0){i=-1;continue;}
	task_array[j]->line_count=font_fblack.count_line(260,task_array[j]->get_task_msg());
	for(k=task_array[j]->line_count;k>0;k--) {
	    char *print_str;
	    if(i>=0) {
		print_str=font_fblack.get_line(260,k,task_array[j]->get_task_msg());
		font_fblack.put(320,520+i*18,print_str);
		if(k==1) {
		    // print_str=date.date_str(task_array[j]->news_date,1,0);
		    print_str=info.date_str_julian( task_array[j]->news_date, INFO_DATE_DEFAULT|INFO_DATE_NO_DAY|INFO_DATE_SHORT_MONTH );
		    font_fblack.put(265,520+i*18,print_str);
		}
		i--;
	    }
	}
    }
    //		log_mode=OBJECTIVE_MODE;
    vga.blt_buf(260, 515, 620, 590);
};

void BlackBoard::black_log_report(int refreshFlag) {
    int tab1=REPORT_X1,tab2=REPORT_X1+70,
	tab3=REPORT_X1+100,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+20,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;
    vga.use_back();

    new_browser_rec_count=2;
    space_for_news=0;
    space_for_task=0;

    int i;

    if(log_mode==NEWS_MODE) {
	for(i=1;i<=news_array.size();i++) {
	    news_array[i]->line_count=font_chartsm.count_line(400,news_array[i]->msg());
	    new_browser_rec_count+=news_array[i]->line_count;
	    space_for_news+=news_array[i]->line_count;
	}
    }
    else {
	for(i=1;i<=task_array.size();i++) {
	    task_array[i]->line_count=font_chartsm.count_line(400,task_array[i]->get_task_msg());
	    new_browser_rec_count+=task_array[i]->line_count;
	    space_for_task+=task_array[i]->line_count;
	}
    }

    if(new_browser_rec_count!=old_browser_rec_count)
	refreshFlag = INFO_REPAINT;
    old_browser_rec_count=new_browser_rec_count;

    if (refreshFlag == INFO_REPAINT) {
	if(blackboard_report_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!blackboard_report_init_flag) {
	    user_interface.bg_img(BG_PIC_ID, &vga_back);

	    user_interface.brighten(tab1,vtab1,tab4,vtab4);
	    user_interface.brighten(tab1,vtab1,tab4,vtab4);
	    user_interface.rect(tab1,vtab1,tab4,vtab4);

	    user_interface.brighten(REPORT_X1+250,REPORT_Y1-18,REPORT_X1+480,REPORT_Y1+13);
	    user_interface.brighten(REPORT_X1+250,REPORT_Y1-18,REPORT_X1+480,REPORT_Y1+13);
	    user_interface.rect(REPORT_X1+250,REPORT_Y1-18,REPORT_X1+480,REPORT_Y1+13);
	    font_charts.put(REPORT_X1+285,REPORT_Y1-8,"News and Objective Log");

	    int browseRecHeight=20;

	    blackboard_browser.init(
		tab1,vtab1,tab4,vtab4,
		0,  browseRecHeight,
		new_browser_rec_count,
		log_disp_rep_row,
		log_detect_rep_row,0);

	    blackboard_browser.open(new_browser_rec_count);

	    short x = tab1+120;
	    for (int i = 0; i < 2; i++, x+=270) {
		user_interface.create_button_bitmap(x, vtab4+13, x+200, vtab4+33,
						    bottom_button_label[i], &bottom_button_up_bitmap[i],
						    &bottom_button_down_bitmap[i], &vga_back);

		bottom_button_group[i].create_bitmapW(x, vtab4+13, x+200, vtab4+33,
						      ((i == log_mode) ? bottom_button_down_bitmap[i] : bottom_button_up_bitmap[i])+4);
	    }

	    button_back.create_text( tab4-80, vtab4+13, tab4-10, vtab4+33, "Back" );

	    blackboard_report_init_flag = 1;
	}
	bottom_button_group.paint(log_mode);
	button_back.paint();
    }
    else {
	vga.use_back();
	blackboard_browser.update();
    }

    //------- blt the back buffer to front buffer -------//
    vga.blt_buf(260, 515, 620, 590);
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
    vga.use_front();
    //---------- End of function blackboard::welcome_blackboard_report ------------//
};

int BlackBoard::black_log_detect() {
    if (!blackboard_report_init_flag)
	return 0;
    vga.use_front();
    blackboard_browser.detect();
    int bottomButtonGroupDetect;
    if ((bottomButtonGroupDetect = bottom_button_group.detect()) >= 0) {
	//---- deinit the modes to free memory ----//
	//		(*deinit_func_ptr[current_mode])();

	//-------- change display mode ------------//
	bottom_button_group[log_mode].body_ptr = (void *) (bottom_button_up_bitmap[log_mode] + 4);
	log_mode = bottomButtonGroupDetect;
	bottom_button_group[log_mode].body_ptr = (void *) (bottom_button_down_bitmap[log_mode] + 4);

	//-------- post display redraw flag -------//
	sys.redraw_all_flag = 1;

	return 1;
    }

    if( button_back.detect() ) {
	sys.set_view_mode( sys.back_view_mode );
	return 1;
    }

    return 0;
}

void BlackBoard::black_log_deinit() {

}
