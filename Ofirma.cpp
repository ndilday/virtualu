//Filename    : OFirmArray.cpp
//Description : FirmArray Class Definition
//owner		: ho

#include <OAUDIO.H>
#include <OCONFIG.H>
#include <ODEPT.H>
#include <OFIRMRES.H>
#include <OFIRM.H>
#include <OSYS.H>

//----------- Begin of function FirmArray Constructor -----//
//! desc
//! <int> -
//! [int] -
//!
FirmArray::FirmArray() : DynArrayB(sizeof(Firm*), 50, DEFAULT_REUSE_INTERVAL_DAYS) {

}

//------------- End of function FirmArray Constructor -----//

//----------- Begin of function FirmArray Destructor ------//
//!
FirmArray::~FirmArray() {

}

//------------- End of function FirmArray Destructor ------//

//----------- Begin of function FirmArray::init --------//
//!
void FirmArray::init() {
    selected_recno = 0;
    touched_recno  = 0;
}

//------------- End of function FirmArray::init --------//

//----------- Begin of function FirmArray::deinit --------//
//!
void FirmArray::deinit() {
    //#### fred 1113

    if( size()==0 )
	return;

    //----------------------------------//

    for( int i=size() ; i>0 ; i-- ) {
	if( !is_deleted(i) )
	    del_firm(i);
    }

    //----------------------------------//

    zap();                                          // zap the DynArrayB
}

//------------- End of function FirmArray::deinit --------//

//----------- Begin of function FirmArray::process ------//
//!
//! Process all firm in firm_array for action and movement for next frame
//!
//! Return : 1 - all firm in the FirmArray has been processed
//!          0 - only some has been processed, not all
//!
int FirmArray::process() {
    return 1;
}

//------------- End of function FirmArray::process ------//

//--------- Begin of function FirmArray::add_firm ---------//
//!
//! <int> xLoc        = the x location of the firm to be built
//! <int> yLoc        = the y location of the firm to be built
//! <int> firmId      = firm type id.
//!
//! Return : <int> the record no. of the newly added firm;  index for firm_array
//!
int FirmArray::add_firm(int xLoc, int yLoc, int firmId) {
    if( !firm_res[firmId]->is_available() )         // if the firm is not available in the current university type, don't add it
	return 0;

    //	if( !world.can_build_firm(xLoc, yLoc, firmId) )
    //		return 0;

    //---------- create the firm -------------//

    int firmRecno = create_firm(firmId);

    firm_array[firmRecno]->init(xLoc, yLoc, firmId);

    return firmRecno;
}

//---------- End of function FirmArray::add_firm ---------//

//--------- Begin of function FirmArray::create_firm ---------//
//! called by :
//!
//! 1. FirmArray::add_firm()  for setting up a new firm
//! 2. FirmArray::read_file() when loading game.
//!
//! <int> firmId = firm type id
//!
//! return : <int> the record no. of the newly added firm
//!
int FirmArray::create_firm(int firmId) {
    Firm* firmPtr;

    switch(firmId) {
    case 1:
    default:
	firmPtr = new Firm;                         //TO change it to NAT_SCI ??
	break;

	//temp default:
	//err_now("FirmArray::create_firm()");
    }

    //----------------------------------------//

    linkin(&firmPtr);
    firmPtr->firm_recno = recno();

    return firmPtr->firm_recno;
}

//----------- End of function FirmArray::create_firm ---------//

//--------- Begin of function FirmArray::del_firm ---------//

void FirmArray::del_firm(int recNo) {
    Firm* firmPtr = firm_array[recNo];

    delete firmPtr;

    linkout(recNo);

    //--------------------------------//

    if( recNo == selected_recno )
	selected_recno = 0;

    //--------------------------------//

    if( recNo == touched_recno )
	touched_recno = 0;
}

//----------- End of function FirmArray::del_firm -----------//

//--------- Begin of function FirmArray::select_firm ---------//

void FirmArray::select_firm(int selectedRecno) {
    selected_recno = selectedRecno;

    if( !selectedRecno )
	return;

    Firm* firmPtr = operator[](selectedRecno);

    if( firmPtr->department_recno ) {               // only change the selected department recno if the newly selected firm is a departmental building, otherwise keep the current selected department
	department_array.selected_recno = firmPtr->department_recno;
	sys.bDept=true;
    }
    else {
	sys.bDept=false;
	sys.old_Dept=-1;

	//--------- play sound --------------//

	switch( firmPtr->firm_id ) {
	case FIRM_ADMISSIONS:
	    audio.play_wav("ADMISSN",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case FIRM_INDOOR_SPORTS:
	    audio.play_wav("BBALL",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case FIRM_STADIUM:
	    audio.play_wav("FOOTBALL",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case FIRM_DEVELOPMENT:
	    audio.play_wav("DEVALUMN",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case FIRM_FACILITIES:
	    audio.play_wav("FACMAN",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case FIRM_LIBRARY:
	    audio.play_wav("INFOTECH",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case FIRM_OLD_MAIN:
	    audio.play_wav("INVEST",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case FIRM_POLICE:
	    audio.play_wav("CRIME",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;

	case FIRM_PARK_LOTS:
	case FIRM_PARK_STRUCTURE:
	    audio.play_wav("PARKING",audio.int_to_DsVolume(config.sound_effect_volume));
	    break;
	}
    }
}

//----------- End of function FirmArray::select_firm -----------//

//======================================================//

#ifdef DEBUG

//------- Begin of function FirmArray::operator() -----//
//!
Firm* FirmArray::operator()() {
    Firm* firmPtr = (Firm*) get_ptr();              // if recno()==0, get_ptr() returns NULL

    err_if( !firmPtr )
	err_now( "FirmArray[recno()] is deleted" );

    return firmPtr;
}

//--------- End of function FirmArray::operator() ----//

//------- Begin of function FirmArray::operator[] -----//
//!
Firm* FirmArray::operator[](int recNo) {
    Firm* firmPtr = (Firm*) get_ptr(recNo);

    if( !firmPtr )
	err.run( "FirmArray[] is deleted" );

    return firmPtr;
}

//--------- End of function FirmArray::operator[] ----//
#endif
