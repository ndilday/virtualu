//Filename   : OGAMESTR.H
//Description: Object SchoolRes Declaration

#ifndef __OGAMESTR_H
#define __OGAMESTR_H

//----------- Define class GameStrRes -----------//

//!Constant string resource, broken down by category. Methods return
//!appropriate string as a function of id/code.
class GameStrRes {
public:
    char * financial_report_str(int rowId);
    char * balance_sheet_str(int rowId);
    char*   rank_str(int rankId);
    char*   gender_ethnic_str(int genderEthnicGroup);
    char*    trimester_str(int trimesterId);
    // #### begin Gilbert 10/10/2001 #####//
    char*   student_level_str(int studentLevel);  // UG_TRADITION, UG_NONTRADITION, MASTER, DOCTOR, DISTANCE_LEARN
    // #### end Gilbert 10/10/2001 #####//

    //######### begin Norris 0625 #############//
    char *opt_stage1_str(int rowId);
    char *opt_stage2_str(int rowId);
    //########### end Norris 0625 #############//
};

extern GameStrRes game_str_res;

//----------------------------------------------//
#endif
