//Filename    : OFIELD.H
//Description : Field Class Declaration

#ifndef __OFIELD_H
#define __OFIELD_H

#ifndef __OCOURSE_H
#include <OCOURSE.H>
#endif

//--------- Define IPED fields ---------//

enum { IPED_FIELD_COUNT = 14 };
//TO SORT this according to student_par spreadsheet
enum {
    AGRICULTURE,
    BASIC_PERSONAL_PRACTICAL_SVCS,
    BUSINESS,
    COMMUNICAIONS_LIBRARY_SCI,
    EDUCATION,
    ENGINEERING,
    FOREIGN_LANGUAGES,
    HEALTH_SOCIAL_SVCS,
    HUMANITIES,
    INTER_DISCIPLINARY,
    LIFE_PHYSICAL_SCI,
    MATH_COMP_SCI,
    SOCIAL_SCIENCES,
    VISUAL_PERFORMING_ARTS
};

//--------- Define fields used in the game ------//

enum { GAME_FIELD_COUNT = 9 };

enum {
    FIELD_APPLIED_SCIENCE=1,
    FIELD_BUSINESS,
    FIELD_CREATIVE_ARTS,
    FIELD_ENGINEERING,
    FIELD_HUMANITIES,
    FIELD_LANGUAGE_STUDIES,
    FIELD_MATH_AND_COMPUTER_SCIENCE,
    FIELD_NATURAL_SCIENCE,
    FIELD_SOCIAL_SCIENCE,
};

//------ Define struct FieldRec -------//

//!Structure containing information associated with major fields as stored
//!in the database.
struct FieldRec {
    enum { NAME_LEN=30 };

    char  code;
    char  name[NAME_LEN];

    char  elective_course_focus_pref[MAX_COURSE_DEPTH_WITHOUT_GR][2];
};

//----- Define struct FieldInfo -------//

//!Structure containing information associated with the major fields in the
//!simulation, including name, code number, and elective fulfillment.
struct FieldInfo {
    enum { NAME_LEN=30 };

    char  code;
    char  name[NAME_LEN+1];

    // the total of preference weight across different focus categories is 100
    short elective_course_focus_pref[MAX_COURSE_DEPTH_WITHOUT_GR];
};

//----------- Define class FieldRes ---------------//

//!
class FieldRes {
public:
    char            init_flag;

    short           field_count;
    FieldInfo*      info_array;

public:
    FieldRes();

    void        init();
    void        deinit();

    int       look_up_code(int fieldCode);

    int         write_file(File*);
    int         read_file(File*);

#ifdef DEBUG
    FieldInfo*   operator[](int fieldId);
#else
    FieldInfo*   operator[](int fieldId) { return info_array + fieldId - 1; }
#endif

private:
    void        load_info();
};

extern FieldRes field_res;

//---------------------------------------------//
#endif
