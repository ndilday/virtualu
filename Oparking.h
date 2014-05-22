//Filename    : OPARKING.H
//Description : More report Class Declaration
//Owner		  : Kevin(ho)

#ifndef __OPARKING_H
#define __OPARKING_H

//----------- Define class Development --------------//
//
//!
class Parking {
public:
    Parking();
    ~Parking();
    void init();
    void deinit();
    void parking_report(int);
    int parking_detect();
};

extern Parking parking;

//-------------------------------------------------//
#endif
