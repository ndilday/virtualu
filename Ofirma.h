//Filename    : OFIRMA.H
//Description : Object Firm Array
//owner		: ho

#ifndef __OFIRMA_H
#define __OFIRMA_H

#ifndef __ODYNARRB_H
#include <ODYNARRB.H>
#endif

//--------- Define class FirmArray ----------//

class Firm;

//!Collection Class for managing all of the visible buildings. Responds to
//!click events by playing associated sound effect, and processes all firms
//!for action and movement for next frame.
class FirmArray : public DynArrayB {
public:
    int  selected_recno;                          // the firm current being selected

    int  touched_recno;                           // the firm which the mouse cursor currently touches
    int  touched_frame_count;

public:
    FirmArray();
    ~FirmArray();

    void  init();
    void  deinit();

    int add_firm(int xLoc, int yLoc, int firmId);
    int create_firm(int firmId);
    void  select_firm(int firmRecno);

    void  del_firm(int);
    int   firm_class_size(int);

    int   process();
    void  next_day();
    void  next_month();
    void  next_year();

    void  skip(int);

    int   write_file(File*);
    int   read_file(File*);

#ifdef DEBUG
    Firm* operator()();                           // reference to current Firm record
    Firm* operator[](int recNo);
#else
    Firm* operator()()          { return (Firm*) get_ptr(); }
    Firm* operator[](int recNo)   { return (Firm*) get_ptr(recNo); }
#endif

    int   is_deleted(int recNo)                   // whether the item is deleted or not
	{ return get_ptr(recNo) == NULL; }

};

//---------------------------------------------//

extern FirmArray firm_array;

//---------------------------------------------//
#endif
