//Filename   : OPEERSCH.CPP
//Description: PeerSchool class methods -- also see Peer_Insts of HE.initialization.xls
//Owner:			Fred

#include <OSCHOOL.H>
#include <OSCHLRES.H>
#include <OPEERSCH.H>

//--------- Begin of function PeerSchool::init --------//
//!
//! init object member variables
//!
void PeerSchool::init(int recno) {
    memset( this, 0, sizeof(PeerSchool));

    if ( recno > 0 ) {                              // for handling palyerschool
	//--- copy all vars from a School object in db_school_array[] to player_school_ex ---//
	School* sPtr = school_res.desired_school_array+recno-1;

	memcpy( this, sPtr, sizeof(School) );         // PeerSchool is a derived class of School

	SchoolEx::init(recno);
    }
}

//--------- End of function PeerSchool::init --------//
