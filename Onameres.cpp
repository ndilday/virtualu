//Filename    : ONAMERES.CPP
//Description : NameRes Class Definition
//Owner			: Fred

#include <All.h>
#include <OVGA.H>
#include <OSYS.H>
#include <OSTR.h>
#include <OMISC.h>
#include <OGAMESET.H>
#include <ONAMERES.h>

//----------- Define constants -------------//

#define FIRST_NAME_DB     "N_FIRST"
#define MIDDLE_NAME_DB      "N_MID"
#define LAST_NAME_DB        "N_LAST"

//----------- Begin of function NameRes Constructor -----//
//!
NameRes::NameRes() {
    init_flag=0;
}

//------------- End of function NameRes Constructor -----//

//----------- Begin of function NameRes Destructor ------//
//!

NameRes::~NameRes() {
    deinit();
}

//------------- End of function NameRes Destructor ------//

//----------- Begin of function NameRes::init --------//
//!
void NameRes::init() {
    if( init_flag )
	return;

    load_first_name();
    load_info(MIDDLE_NAME_DB, &middle_name_array, middle_name_count );
    load_info(LAST_NAME_DB  , &last_name_array  , last_name_count   );

    //----------------------------------------//

    init_flag=1;
}

//------------- End of function NameRes::init --------//

//----------- Begin of function NameRes::deinit --------//
//!
void NameRes::deinit() {
    if( init_flag ) {
	mem_del(first_name_array);
	mem_del(middle_name_array);
	mem_del(last_name_array);

	init_flag=0;
    }
}

//------------- End of function NameRes::deinit --------//

//----------- Begin of function NameRes::load_first_name -------//
//!
void NameRes::load_first_name() {
    FirstNameRec  *firstNameRec;
    FirstNameInfo *firstNameInfo;
    int         i;
    Database    *dbFirstName = game_set.open_db(FIRST_NAME_DB);

    first_name_count = (short) dbFirstName->rec_count();
    first_name_array = (FirstNameInfo*) mem_add( sizeof(FirstNameInfo)*first_name_count );

    //------ read in firstName bitmap info array -------//

    memset( first_name_array, 0, sizeof(FirstNameInfo) * first_name_count );

    for( i=0 ; i<first_name_count ; i++ ) {
	firstNameRec  = (FirstNameRec*) dbFirstName->read(i+1);
	firstNameInfo = first_name_array+i;

	m.rtrim_fld( firstNameInfo->name, firstNameRec->name, firstNameRec->NAME_LEN );

	if( firstNameRec->gender=='F' )
	    firstNameInfo->gender = 'F';
	else
	    firstNameInfo->gender = 'M';
    }
}

//----------- End of function NameRes::load_first_name -------//

//----------- Begin of function NameRes::load_info -------//
//!
void NameRes::load_info(char* dbFileName, NameInfo** nameArrayPtr, short& nameCount) {
    NameRec  *nameRec;
    NameInfo *nameInfo;
    Database *dbName = game_set.open_db(dbFileName);

    nameCount     = (short) dbName->rec_count();
    *nameArrayPtr = (NameInfo*) mem_add( sizeof(NameInfo)*nameCount );

    //------ read in name bitmap info array -------//

    memset( *nameArrayPtr, 0, sizeof(NameInfo) * nameCount );

    for( int i=0 ; i<nameCount ; i++ ) {
	nameRec  = (NameRec*) dbName->read(i+1);
	nameInfo = *nameArrayPtr + i;

	m.rtrim_fld( nameInfo->name, nameRec->name, nameRec->NAME_LEN );
    }
}

//----------- End of function NameRes::load_info -------//

//----------- Begin of function NameRes::get_name_str -------//
//!
char* NameRes::get_name_str(int firstNameId, int middleNameId, int lastNameId) {
    static String str;

    str  = first_name_array[firstNameId-1].name;
    //str += " ";
    //str += middle_name_array[middleNameId-1].name;
    str += " ";
    str += last_name_array[lastNameId-1].name;

    return str;
}

//----------- End of function NameRes::get_name_str -------//

//----------- Begin of function NameRes::is_male -------//
//!
bool NameRes::is_male(int firstNameId) {
    return first_name_array[firstNameId-1].gender == 'M';
}

//----------- End of function NameRes::is_male -------//
