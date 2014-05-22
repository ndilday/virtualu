//Filename    : OMAOP.h
//Description : MAOP Class Declaration

#ifndef __OMAOP_H
#define __OMAOP_H

#include <OINFO.H>

#ifndef __ODYNARRB_H
#include <ODYNARRB.H>
#endif

//--------------------------------------------//

//---------- Define class MAOP ------------//

//!
class MAOP {

public:

    void  init(int departmentRecno, int rankLevel, int genderEthnicGroup, int MAOPAge, int startTeachingDate, int MAOPSalary,
	       int talentTeaching, int talentScholarship, int talentResearch);

    void  next_day();

    void  calc_all_performance();

    void  disp_summary_basic(int x, int y, int xColumn2, int refreshFlag);
    void  disp_summary_activity(int x, int y, int xColumn2, int refreshFlag);
    void  disp_summary_assess(int x, int y, int xColumn2, int refreshFlag);

    void  disp_detail_std(int refreshFlag);
    void  disp_research_proposal(int y, int refreshFlag);

private:
    void  init_photo();

    char* hour_str(int hourCount);

    void  calc_performance_teaching();
    void  calc_performance_scholarship();
    void  calc_performance_research();
    void  calc_satisfaction_index();
};

//----------- Define class MAOPArray -----//

//!
class MAOPArray : public DynArrayB {
public:
    int     MAOP_count;

    //------------------------------------------//

public:
    MAOPArray();
    ~MAOPArray();

    int     add(MAOP *newMAOP);
    int     add(int departmentRecno, int rankLevel, int genderEthnicGroup, int MAOPAge, int startTeachingDate, int MAOPSalary,
		int talentTeaching, int talentScholarship, int talentResearch);

    void      del(int recNo);

    int       avg_discretionary_hour(int discretionaryType);

    void      next_day();

    MAOP*   get_unpacked_MAOP(int recNo);

    int     is_deleted(int recNo);

#ifdef DEBUG
    MAOP*   operator[](int recNo);
#else
    MAOP*   operator[](int recNo) { return (MAOP*) get(recNo); }
#endif

private:
};

//------------------------------------------//
#endif
