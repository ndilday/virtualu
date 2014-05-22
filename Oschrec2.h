//Filename   : OSCHREC2.H
//Description: Object SchoolDegreeRecDeclaration
//Owner       : Fred

#ifndef __OSCHREC2_H
#define __OSCHREC2_H

//-------- Define struct SchoolDegreeSLRec --------//

//!
struct SchoolDegreeSLRec {
    char  agriculture_forestry[8];
    char  inter_disciplinary[8];
    char  business[8];
    char  communicaions_library_sci[8];
    char  math_comp_sci[8];
    char  education[8];

    char  engineering[8];
    char  foreign_languages[8];
    char  health_social_svcs[8];
    char  humanities[8];
    char  life_physical_sci[8];

    char  social_sciences[8];
    char  visual_Performing_arts[8];
    char  basic_Personal_practical_svcs[8];
    char  total[8];
};

//-------- Define struct SchoolDegreeSLRec2 --------//

//!Major preferences of students as defined by initial conditions
//!spreadsheet.
struct SchoolDegreeSLRec2 {
    char  agriculture_forestry[9];
    char  inter_disciplinary[9];
    char  business[9];
    char  communicaions_library_sci[9];
    char  math_comp_sci[9];
    char  education[9];

    char  engineering[9];
    char  foreign_languages[9];
    char  health_social_svcs[9];
    char  humanities[9];
    char  life_physical_sci[9];

    char  social_sciences[9];
    char  visual_Performing_arts[9];
    char  basic_Personal_practical_svcs[9];
    char  total[11];
};

//-------- Define class SchoolDegreeRec ---------//

class SchoolEx;

//!Container class containing information pertaining to degree programs
//!broken down by student levels. SchoolDegreeRec is used in school expense
//!calculations (SchoolEx).
class SchoolDegreeRec {
    friend SchoolEx;

    char  name[28];
    char  fice[8];
    char  control[7];
    char  carnegie[7];
    char  cc_sort[5];
    char  market_score[11];

    SchoolDegreeSLRec student_level[3];             // for sl 1&2, 3, 4
    SchoolDegreeSLRec2 student_level5;              // for sl 5
};

//------------------------------------------------//
#endif
