//Filename    : OFIELD.CPP
//Description : Field object

#include <ALL.H>
#include <OSYS.H>
#include <OGAMESET.H>
#include <OFIELD.H>

//---------- #define constant ------------//

#define FIELD_DB   "FIELD"

//------- Begin of function FieldRes::FieldRes -----------//

FieldRes::FieldRes() {
    init_flag=0;
}

//--------- End of function FieldRes::FieldRes -----------//

//---------- Begin of function FieldRes::init -----------//
//!
//! This function must be called after a map is generated.
//!
void FieldRes::init() {
    deinit();

    //------- load database information --------//

    load_info();

    init_flag=1;
}

//---------- End of function FieldRes::init -----------//

//---------- Begin of function FieldRes::deinit -----------//

void FieldRes::deinit() {
    if( init_flag ) {
	mem_del(info_array);
	info_array = 0;
	init_flag=0;
    }
}

//---------- End of function FieldRes::deinit -----------//

//------- Begin of function FieldRes::load_info -------//
//!
//! Read in information of RACE.DBF into memory array
//!
void FieldRes::load_info() {
    FieldRec  *fieldRec;
    FieldInfo *fieldInfo;
    Database  *dbField = game_set.open_db(FIELD_DB);

    field_count = (short) dbField->rec_count();
    info_array  = (FieldInfo*) mem_add( sizeof(FieldInfo)*field_count );

    //------ read in field information array -------//

    memset( info_array, 0, sizeof(FieldInfo) * field_count );

    for( int i=0 ; i<field_count ; i++ ) {
	fieldRec  = (FieldRec*) dbField->read(i+1);
	fieldInfo = info_array+i;

	fieldInfo->code = fieldRec->code;

	m.rtrim_fld( fieldInfo->name, fieldRec->name, fieldRec->NAME_LEN );

	for( int j=0 ; j<MAX_COURSE_DEPTH_WITHOUT_GR ; j++ )
	    fieldInfo->elective_course_focus_pref[j] = m.atoi( fieldRec->elective_course_focus_pref[j], 2 );
    }
}

//--------- End of function FieldRes::load_info ---------//

//---------- Begin of function FieldRes::look_up_code -----------//
//!
//! Look up the given field code and return the id. of the field.
//!
int FieldRes::look_up_code(int fieldCode) {
    for( int i=0 ; i<field_count ; i++ ) {
	if( info_array[i].code == fieldCode )
	    return i+1;
    }

    return 0;
}

//---------- End of function FieldRes::look_up_code -----------//

#ifdef DEBUG
//---------- Begin of function FieldRes::operator[] -----------//
//!
FieldInfo* FieldRes::operator[](int fieldId) {
    err_when( fieldId < 1 || fieldId > field_count );

    return info_array + fieldId - 1;
}

//---------- End of function FieldRes::operator[] -----------//
#endif
