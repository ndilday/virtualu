//Filename    : OCOURRES.CPP
//Description : course Resource object
//Owner			: Fred

// see req29b and HE.gdb.init.xls:course_template

#include <OGAMESET.H>
#include <OCOURRES.H>
#include <OCourse.H>
//#include <ODept.H>
#include <ODeptres.H>

//---------- #define constant ------------//

#define COURSE_TEMPLATE_DB    "COU_TMPL"
#define SUMMER_COURSE_DB    "SUMMER"

//---------- Begin of function CourseRes::init -----------//
//!
//! This function must be called after a map is generated.
//!
void CourseRes::init() {
    deinit();

    //------- load database information --------//

    memset( this, 0, sizeof(CourseRes) );

    load_template();
    load_summer_db();

    init_flag=1;
}

//---------- End of function CourseRes::init -----------//

//---------- Begin of function CourseRes::deinit -----------//

void CourseRes::deinit() {
    if( init_flag ) {
	if( course_template_count > 0 )
	    free_template();

	init_flag=0;
    }
}

//---------- End of function CourseRes::deinit -----------//

//------- Begin of function CourseRes::load_template -------//
//!

void CourseRes::change_course_templ_code(char* code) {
    switch (code[0]) {
    case 'A':
	code[0] = 'A';
	break;
    case 'B':
	code[0] = 'B';
	break;
    case 'E':
	code[0] = 'C';
	break;
    case 'F':
	code[0] = 'D';
	break;
    case 'H':
	code[0] = 'E';
	break;
    case 'S':
	if ( code[1] == 'C' )
	    code[0] = 'F';
	else
	    code[0] = 'H';
	break;
    case 'M':
	code[0] = 'G';
	break;
    default:
	err_here();
	break;
    }

    code[1] = '\0';
}

void CourseRes::load_template() {
    //course_template_count = 1;

    CourseTemplateRec   *templRec;
    CourseTemplate      *templPtr;
    Database        *dbTemplate = game_set.open_db(COURSE_TEMPLATE_DB);

    course_template_count = (short) dbTemplate->rec_count();

    err_when( course_template_array );              // to prevent double allocations
    err_when( course_template_count%3 != 0 || course_template_count == 0);

    course_template_count /= 3;                     // 3 = COURSE_PREFERENCE_COUNT

    course_template_array = (CourseTemplate*) mem_add( sizeof(CourseTemplate)*course_template_count );

    //------ read in course information array -------//

    memset( course_template_array, 0, sizeof(CourseTemplate) * course_template_count );

    templPtr = course_template_array;

    for( int i=0, type=0 ; i<course_template_count*3 ;) {
	templRec  = (CourseTemplateRec*) dbTemplate->read(i+1);

	change_course_templ_code(templRec->template_code);
	templPtr->template_code = templRec->template_code[0];

	templPtr->target_preference[type] = char(100 * m.atof( templRec->target_preference, 9 ));
	templPtr->normal_class_size[type] = m.atoi( templRec->normal_class_size, 9 );

	type = (type+1)%3;

	if ( ++i%3 == 0 )
	    templPtr++;

	//----------//
	/*
	  templatePtr->target_preference[P_SEMINAR] = 11;	// 99->99%
	  templatePtr->target_preference[P_CLASS_WITH_BREAKOUTS] = 27;
	  templatePtr->target_preference[P_OTHER] = 62;

	  //short	normal_class_size= (float) m.atof(templateRec->rank_age_multiplier, 9);
	  templatePtr->normal_class_size[P_SEMINAR] = 12;
	  templatePtr->normal_class_size[P_CLASS_WITH_BREAKOUTS] = 19;
	  templatePtr->normal_class_size[P_OTHER] = 20;
	*/
    }

    for( int i=0; i<course_template_count; i++) {
	templPtr = course_template_array + i;

	int tmp = templPtr->target_preference[0] + templPtr->target_preference[1];

	templPtr->target_preference[2] = 100 - tmp;
    }
}

//--------- End of function CourseRes::load_template ---------//

//------- Begin of function CourseRes::free_template -------//
//!
void CourseRes::free_template() {
    err_when( course_template_count == 0 );

    mem_del(course_template_array);
    course_template_count = 0;
}

//--------- End of function CourseRes::free_template ---------//

//------- Begin of function CourseRes::get_course_template -------//
//!
CourseTemplate* CourseRes::get_template(char* templateCode) {
    int offset = templateCode[0] - FIRST_COURSE_TEMPLATE_CODE;

    err_when(offset >= course_template_count);
    return course_template_array + offset;
}

//--------- End of function CourseRes::get_course_template ---------//

//------- Begin of function CourseRes::get_course_template -------//
//!
CourseTemplate* CourseRes::get_template(int deptId) {
    return get_template(department_res[deptId]->template_course_type);
}

//--------- End of function CourseRes::get_course_template ---------//

//------- Begin of function CourseRes::get_normal_class_size -------//
//!
int CourseRes::get_normal_class_size(char* templateCode, char teachingMethod) {
    err_when(teachingMethod >= MAX_TEACHING_METHOD);

    //-------------------------------------------//

    // map TM in course.h to TM in courres.h
    // c.h	SEMINAR, DISTANCE_LEARN_COURSE, CLASS_WITH_BREAKOUT, BREAKOUT_LAB, GENERAL,
    // cr.h	P_SEMINAR=0, P_CLASS_WITH_BREAKOUTS, P_OTHER,

    static const char remap[MAX_TEACHING_METHOD]
	= { P_SEMINAR, -1, P_CLASS_WITH_BREAKOUTS, P_SEMINAR, P_OTHER };

    teachingMethod = remap[teachingMethod];
    err_when(teachingMethod == -1);                 // this case handled at the beginning

    CourseTemplate* templ = get_template(templateCode);

    return templ->normal_class_size[teachingMethod];
}

//--------- End of function CourseRes::get_normal_class_size ---------//

//------- Begin of function CourseRes::get_course_pref -------//
//!
void CourseRes::get_course_pref(char* templateCode, char* pref) {
    err_here();

    CourseTemplate* templ = get_template(templateCode);

    const char remap[MAX_TEACHING_METHOD]
	= { templ->target_preference[P_SEMINAR], 0, templ->target_preference[P_CLASS_WITH_BREAKOUTS], 0, templ->target_preference[P_OTHER] };

    for( int i=0 ; i<MAX_TEACHING_METHOD; i++ ) {
	pref[i] = remap[i];
    }
}

//--------- End of function CourseRes::get_course_pref ---------//

//------- Begin of function CourseRes::load_summer_db -------//
//!
void CourseRes::load_summer_db() {
    SummerRec *sumRec;
    Database    *dbsummer = game_set.open_db(SUMMER_COURSE_DB);

    //------ read in course information array -------//

    memset( summer_course_pref, 0, sizeof(summer_course_pref) );

    // summer_course_pref[MAX_STUDENT_LEVEL][SUMMER_PERF_COUNT][SUMMER_COURSE_COUNT];				// 99->99%

    for( int sl=0, x=1; sl<MAX_STUDENT_LEVEL; sl++ ) {
	for( int e=0; e<SUMMER_PERF_COUNT; e++ ) {
	    sumRec  = (SummerRec*) dbsummer->read(x);

	    int prob=0;
	    for( int c=0; c<SUMMER_COURSE_COUNT; c++ ) {
		summer_course_pref[sl][e][c] = int( 100 * m.atof(sumRec->course_pref[c], 9) );

		prob += summer_course_pref[sl][e][c];

		err_when(summer_course_pref[sl][e][c] > 100 || summer_course_pref[sl][e][c] < 0);
	    }
	    x++;

	    err_when(prob<90 || prob>110);
	}
	x++;
    }
}

//--------- End of function CourseRes::load_summer_db ---------//
