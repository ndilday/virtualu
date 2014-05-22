#include <OSYS.H>
#include <OCONFIG.H>
#include <ODEPTLST.H>
#include <OGAMESET.H>
#include <OWORLDMT.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OFONT.H>
#include <OGAMESTR.H>
#include <OMOUSE.H>
#include <OIFACE.H>
#include <OINFO.H>
#include <ODEPT.H>
#include <OBUTTON.H>

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
    BG_PIC_ID = 7
};

static char dept_list_init_flag;
static int line_spacing;
//------- Begin of function DeptList::DeptList -----------//
//!
DeptList::DeptList() {
    init();
}

//--------- End of function DeptList::DeptList -----------//

//------- Begin of function DeptList::~DeptList -----------//
//!
DeptList::~DeptList() {
    deinit();
}

//--------- End of function DeptList::~DeptList -----------//

//------- Begin of function DeptList::deinit ---------------//
//!
void DeptList::deinit() {
    dept_list_init_flag = 0;
}

//--------- End of function DeptList::deinit ---------------//

//------- Begin of function DeptList::init -----------------//
//!
void DeptList::init() {
    dept_list_init_flag = 0;
}

//--------- End of function DeptList::init -----------------//

/*	
static char *dept_list_labels[10]=
{
"ADMISSIONS AND FINANCIAL AID",
  "ATHLETICS",
  "DEVELOPMENT AND ALUMNI AFFAIRS",
  "FACILITIES MANAGEMENT",
  "LIBRARY AND INFORMANTION TECHNOLOGY",
  "INVESTMENT",
  "CRIME",
  "PARKING",
"ENERGY",
"OTHER MINOR REPORTS",
};
*/

//---------- Begin of function DeptList::eval_letter_report ------------//
//! You can set the value of DeptList::topic_id before running this functions
void DeptList::dept_list_report(int refreshFlag) {
    int tab1=REPORT_X1,tab2=REPORT_X1+160,
	tab3=REPORT_X1+200,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+5,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;

    line_spacing = 340/department_array.size();

    vga.use_back();
    sys.is_institution_report=0;
    if (refreshFlag == INFO_REPAINT) {
	if(dept_list_init_flag )
	    deinit();

	//------------- paint the background ----------------//
	if (!dept_list_init_flag) {
	    user_interface.bg_img(BG_PIC_ID, &vga_back);
	    info.disp_column_bitmap(&vga_back);
	    dept_list_init_flag = 1;
	    font_hstone.put(tab3,REPORT_Y1,"SELECT THE ACTIVE DEPARTMENT : ");
	    for(int i=1;i<=department_array.size();i++) {
		//				image_interface.put_back( tab2,vtab1+line_spacing*i, "DSPB_UP" );
		//				vga.blt_buf(tab2,vtab1+line_spacing*i,tab2+70,vtab1+line_spacing*i+22);
		//				font_yellow_diamond.put(tab3,vtab1+line_spacing*i+2,dept_list_labels[i]);
		if( department_array.is_deleted(i) )
		    continue;
		String deptStr;
		deptStr = department_array[i]->name();
		String deptStrTmp=deptStr.substr(0,deptStr.len()-11);

		//				font_stone.put(tab2,vtab1+line_spacing*i+2,m.format(i,1));
		//				font_stone.put(tab2+30,vtab1+line_spacing*i+2,deptStr);

		font_hstone.put
		    (tab2,vtab1+line_spacing*i+2,m.format(i,1));
		font_hstone.put
		    (tab2+30,vtab1+line_spacing*i+2,deptStrTmp);
	    }
	}
    }
    //------- blt the back buffer to front buffer -------//
    vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
}

//---------- End of function Development::eval_letter_report ------------//

////////////////////////////////////////////////////////////////////////////////////////////

int DeptList::dept_list_detect() {
    int tab1=REPORT_X1,tab2=REPORT_X1+160,
	tab3=REPORT_X1+200,tab4=REPORT_X1+700;
    int vtab1=REPORT_Y1+5,vtab2=REPORT_Y1+70,
	vtab3=REPORT_Y1+100,vtab4=REPORT_Y1+350;
    if (!dept_list_init_flag)
	return 0;

    for(int i=1;i<=department_array.size();i++) {
	if( department_array.is_deleted(i) )
	    continue;
	String deptStr;
	deptStr = department_array[i]->name();
	String deptStrTmp=deptStr.substr(0,deptStr.len()-11);

	font_stone.put(tab2,vtab1+line_spacing*i+2,m.format(i,1));
	font_stone.put(tab2+30,vtab1+line_spacing*i+2,deptStrTmp);

    }

    for(int i=1;i<=department_array.size();i++) {
	if( department_array.is_deleted(i) )
	    continue;
	if(mouse.in_area(tab3,vtab1+line_spacing*i,tab4,vtab1+line_spacing*i+22)) {
	    String deptStr;
	    deptStr = department_array[i]->name();
	    String deptStrTmp=deptStr.substr(0,deptStr.len()-11);

	    font_hstone.put(tab2,vtab1+line_spacing*i+2,m.format(i,1));
	    font_hstone.put(tab2+30,vtab1+line_spacing*i+2,deptStrTmp);
	}
	if(mouse.single_click(tab3,vtab1+line_spacing*i,tab4,vtab1+line_spacing*i+22)) {
	    department_array.selected_recno=i;
	    sys.set_view_mode(sys.report_before_morerpt_or_deptlist);
	    sys.redraw_all_flag=1;
	}
    }
    return 0;
}
