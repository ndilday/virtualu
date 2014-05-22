//Filename    : ODEPTLIST.H
//Description : department report Class Declaration
//Owner		  : Kevin(ho)

#ifndef __ODEPTLIST_H
#define __ODEPTLIST_H

//----------- Define class Development --------------//
//
//!? ? ?
class DeptList {
public:
    DeptList();
    ~DeptList();
    void init();
    void deinit();
    void dept_list_report(int);
    int dept_list_detect();
};

extern DeptList dept_list;

//-------------------------------------------------//
#endif
