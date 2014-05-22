//Filename    : OGFILE3.CPP
//Description : Object Game file, save game and restore game, part 3.1

//=================================================================================//
// for write_file_1

#include <ODEPTRES.H>
#include <OPEERSCH.H>
#include <OSCHLRES.H>
#include <OENROLL.H>
#include <OLETTER.H>                              //## chea 990519
#include <OFACURES.H>                             //## chea 090699

//=================================================================================//
// for write_file_2

#include <OCONFIG.H>
#include <OGAME.H>
#include <OINFO.H>
#include <OFINANCE.H>
#include <OPSCHOOL.H>

//=================================================================================//
// for write_file_3

#include <ODEPT.H>
#include <ONEWS.H>
#include <OTASK.H>
#include <OCHANCE.H>

#include <OATHLETI.H>
#include <ODEVELOP.H>
#include <OFACILIT.H>
#include <OLIBTECH.H>
#include <OINVEST.H>
#include <OSTUOFF.H>

//=================================================================================//
#include <OGFILE.H>

//=================================================================================//
//============================ section 1/3 ========================================//
//=================================================================================//

//--------- Begin of function DepartmentRes::write_file ---------------//
int DepartmentRes::write_file(File* filePtr) {
    //char        		init_flag;

    //short       		department_count;
    //DepartmentInfo*   info_array;

    //DepartmentInfo		general_dept_info;	// = general_education_department, although we just need GeneralDepartmentInfo, we still use DepartmentInfo for ease of programming
    //GeneralDepartment	general_dept;			// year 1 ug student without major

    int rc;

    rc = filePtr->file_put_short(init_flag);
    rc &= filePtr->file_put_short(department_count);

    if ( !rc )
	return 0;

    err_when(!info_array);

    if( !filePtr->file_write( info_array, department_count*sizeof(DepartmentInfo) ) )
	return 0;

    if( !filePtr->file_write(&general_dept_info, sizeof(general_dept_info)) )
	return 0;

    return general_dept.write_file(filePtr);
}

int DepartmentRes::read_file(File* filePtr) {
    init_flag = (char) filePtr->file_get_short();
    department_count = filePtr->file_get_short();

    err_when(!info_array);

    if( !filePtr->file_read( info_array, department_count*sizeof(DepartmentInfo) ) )
	return 0;

    if( !filePtr->file_read(&general_dept_info, sizeof(general_dept_info)) )
	return 0;

    return general_dept.read_file(filePtr);
}

//--------- End of function DepartmentRes::write_file ---------------//

//## chea 090699 begin try to save the FacultyRes()
//--------- Begin of function FacultyRes::write_file ---------------//
int FacultyRes::write_file(File* filePtr) {

    //char	          init_flag;
    //short       		 faculty_template_count;
    //FacultyTemplate* faculty_template_array;

    int rc;

    rc = filePtr->file_put_short(init_flag);
    rc &= filePtr->file_put_short(faculty_template_count);

    if ( !rc )
	return 0;

    err_when(!faculty_template_array);

    if( !filePtr->file_write( faculty_template_array, faculty_template_count*sizeof(FacultyTemplate) ) )
	return 0;

    return 1;
}

int FacultyRes::read_file(File* filePtr) {
    init_flag = (char) filePtr->file_get_short();
    faculty_template_count = filePtr->file_get_short();

    err_when(!faculty_template_array);

    if( !filePtr->file_read( faculty_template_array, faculty_template_count*sizeof(FacultyTemplate) ) )
	return 0;

    return 1;
}

//--------- End of function FacultyRes::write_file ---------------//
//## chea 090699 end try to save the FacultyRes()

//--------- Begin of function SchoolRes::write_file ---------------//
int SchoolRes::write_file(File* filePtr) {
    err_when(db_school_array);
    err_when(peer_info_array);

    //PeerSchool*			player_peer_school;
    //short					peer_school_count;
    //PeerSchool*			peer_school_array;	// includes player_peer_school after battle.create_object()

    if ( !filePtr->file_write(this, sizeof(SchoolRes)) )
	return 0;

    //-------------------//

    err_when(!peer_school_array);

    if( !filePtr->file_write( peer_school_array, peer_school_count*sizeof(PeerSchool) ) )
	return 0;

    //-------------------//

    short playerPeerIndex = -1;

    for (int i=0; i<peer_school_count; i++)
	if ( peer_school_array+i == player_peer_school ) {
	    playerPeerIndex = i;
	    break;
	}

    err_when(playerPeerIndex <0);

    filePtr->file_put_short(playerPeerIndex);

    //-------------------//
    // write PeerSchool::
    // static float pref_vars_average_array[PREFERECNE_COUNT2];
    // static float pref_vars_average_array_last[PREFERECNE_COUNT2];
    // static int	average_faculty_salary[MAX_RANK_AGE_GROUP];		// not $000

    if( !filePtr->file_write(PeerSchool::pref_vars_average_array, sizeof(PeerSchool::pref_vars_average_array) ) )
	return 0;

    if( !filePtr->file_write(PeerSchool::pref_vars_average_array_last, sizeof(PeerSchool::pref_vars_average_array_last) ) )
	return 0;

    if( !filePtr->file_write(PeerSchool::average_faculty_salary, sizeof(PeerSchool::average_faculty_salary) ) )
	return 0;

    return 1;
}

//-------------------//
int SchoolRes::read_file(File* filePtr) {
    free_db_info();

    School* schoolPtr = desired_school_array;
    int   schoolCount = desired_school_count;

    //-------------------//

    if ( !filePtr->file_read(this, sizeof(SchoolRes)) )
	return 0;

    desired_school_array = schoolPtr;
    desired_school_count = schoolCount;

    //-------------------//

    peer_school_array = (PeerSchool *) mem_add(peer_school_count*sizeof(PeerSchool));

    if( !filePtr->file_read( peer_school_array, peer_school_count*sizeof(PeerSchool) ) )
	return 0;

    //-------------------//

    short playerPeerIndex = -1;

    playerPeerIndex = filePtr->file_get_short();

    err_when(playerPeerIndex < 0 || playerPeerIndex >= peer_school_count);

    player_peer_school = peer_school_array+playerPeerIndex;

    //-------------------//

    if( !filePtr->file_read(PeerSchool::pref_vars_average_array, sizeof(PeerSchool::pref_vars_average_array) ) )
	return 0;

    if( !filePtr->file_read(PeerSchool::pref_vars_average_array_last, sizeof(PeerSchool::pref_vars_average_array_last) ) )
	return 0;

    if( !filePtr->file_read(PeerSchool::average_faculty_salary, sizeof(PeerSchool::average_faculty_salary) ) )
	return 0;

    return 1;
}

//--------- End of function SchoolRes::write_file ---------------//

//--------- Begin of function EnrollRes::write_file ---------------//
int EnrollRes::write_file(File* filePtr) {
    return filePtr->file_write(this, sizeof(EnrollRes));
}

int EnrollRes::read_file(File* filePtr) {
    return filePtr->file_read(this, sizeof(EnrollRes));
}

//--------- End of function EnrollRes::write_file ---------------//

//--------- Begin of function EnrollRes::write_file ---------------//  //## chea begin 990519
int Letter::write_file(File* filePtr) {
    return filePtr->file_write(this, sizeof(Letter));
}

int Letter::read_file(File* filePtr) {
    return filePtr->file_read(this, sizeof(Letter));
}

//--------- End of function EnrollRes::write_file ---------------//    //## chea end 990519

//=================================================================================//
//============================ section 2/3 ========================================//
//=================================================================================//

//--------- Begin of function Config::write_file ---------------//
int Config::write_file(File* filePtr) {
    return filePtr->file_write(this, sizeof(Config));
}

// keepSysSettings:	default = 1
//
int Config::read_file(File* filePtr, int keepSavedSysSettings) {
    if ( keepSavedSysSettings )
	return filePtr->file_read(this, sizeof(Config));
    else {
	default_game_setting();
	return 1;
    }
}

//--------- End of function Config::write_file ---------------//

//--------- Begin of function Info::write_file ---------------//
int Info::write_file(File* filePtr) {

    random_seed_4_save = m.random_seed;             //## chea 210899 to save the random seed before save

    int writeSize = (char*)(&last_write_offset) - (char*)(this);

    //---------- write the info data -----------//

    return filePtr->file_write( this, writeSize );
}

int Info::read_file(File* filePtr) {
    int readSize = (char*)(&last_write_offset) - (char*)(this);

    //------- read the info data ----------//

    return filePtr->file_read( this, readSize );
}

//--------- End of function Info::write_file ---------------//

//--------- Begin of function Info::write_file ---------------//
int Game::write_file(File* filePtr) {
    return filePtr->file_write( this, sizeof(Game) );
}

int Game::read_file(File* filePtr) {
    return filePtr->file_read( this, sizeof(Game) );
}

//--------- End of function Info::write_file ---------------//

//--------- Begin of function Finance::write_file ---------------//
int Finance::write_file(File* filePtr) {
    return filePtr->file_write( this, sizeof(Finance) );
}

int Finance::read_file(File* filePtr) {
    return filePtr->file_read( this, sizeof(Finance) );
}

//--------- End of function Finance::write_file ---------------//

//--------- Begin of function PlayerSchool::write_file ---------------//
int PlayerSchool::write_file(File* filePtr) {
    return filePtr->file_write( this, sizeof(PlayerSchool) );
}

int PlayerSchool::read_file(File* filePtr) {
    return filePtr->file_read( this, sizeof(PlayerSchool) );
}

//--------- End of function PlayerSchool::write_file ---------------//

//=================================================================================//
//============================ section 3/3 ========================================//
//=================================================================================//

//--------- Begin of function NewsArray::write_file ---------------//
int NewsArray::write_file(File* filePtr) {
    //------ write info in NewsArray ------//

    if( !filePtr->file_write( (char*) this + sizeof(DynArray), sizeof(NewsArray)-sizeof(DynArray) ) )
	return 0;

    return DynArray::write_file( filePtr );
}

int NewsArray::read_file(File* filePtr) {
    if( !filePtr->file_read( (char*) this + sizeof(DynArray), sizeof(NewsArray)-sizeof(DynArray) ) )
	return 0;

    return DynArray::read_file( filePtr );
}

//--------- End of function NewsArray::write_file ---------------//

//--------- Begin of function TaskArray::write_file ---------------//
int TaskArray::write_file(File* filePtr) {
    if( !filePtr->file_write( (char*) this + sizeof(DynArray), sizeof(TaskArray)-sizeof(DynArray) ) )
	return 0;

    return DynArray::write_file( filePtr );
}

int TaskArray::read_file(File* filePtr) {
    if( !filePtr->file_read( (char*) this + sizeof(DynArray), sizeof(TaskArray)-sizeof(DynArray) ) )
	return 0;

    return DynArray::read_file( filePtr );
}

//--------- End of function TaskArray::write_file ---------------//

//--------- Begin of function ChanceEvent::write_file ---------------//
int ChanceEvent::write_file(File* filePtr) {
    return filePtr->file_write(this, sizeof(ChanceEvent));
}

int ChanceEvent::read_file(File* filePtr) {
    return filePtr->file_read(this, sizeof(ChanceEvent));
}

//--------- End of function ChanceEvent::write_file ---------------//

//--------- Begin of function Development::write_file ---------------//
int Development::write_file(File* filePtr) {
    return filePtr->file_write(this, sizeof(Development));
}

int Development::read_file(File* filePtr) {
    return filePtr->file_read(this, sizeof(Development));
}

//--------- End of function Development::write_file ---------------//

//--------- Begin of function Athletics::write_file ---------------//
int Athletics::write_file(File* filePtr) {
    return filePtr->file_write(this, sizeof(Athletics));
}

int Athletics::read_file(File* filePtr) {
    return filePtr->file_read(this, sizeof(Athletics));
}

//--------- End of function Athletics::write_file ---------------//

//--------- Begin of function LibTech::write_file ---------------//
int LibTech::write_file(File* filePtr) {
    return filePtr->file_write(this, sizeof(LibTech));
}

int LibTech::read_file(File* filePtr) {
    return filePtr->file_read(this, sizeof(LibTech));
}

//--------- End of function LibTech::write_file ---------------//

//--------- Begin of function Facility::write_file ---------------//
int Facility::write_file(File* filePtr) {
    return filePtr->file_write(this, sizeof(Facility));
}

int Facility::read_file(File* filePtr) {
    return filePtr->file_read(this, sizeof(Facility));
}

//--------- End of function Facility::write_file ---------------//

//--------- Begin of function Facility::write_file ---------------//
int Investment::write_file(File* filePtr) {
    return filePtr->file_write(this, sizeof(Investment));
}

int Investment::read_file(File* filePtr) {
    return filePtr->file_read(this, sizeof(Investment));
}

//--------- End of function Facility::write_file ---------------//

//--------- Begin of function Facility::write_file ---------------//
int StudentOffice::write_file(File* filePtr) {
    return filePtr->file_write(this, sizeof(StudentOffice));
}

int StudentOffice::read_file(File* filePtr) {
    return filePtr->file_read(this, sizeof(StudentOffice));
}

//--------- End of function Facility::write_file ---------------//
