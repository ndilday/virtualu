//Filename    : ONAMERES.h
//Description : NameRes Class Declaration
//Owner			: Fred

#ifndef __ONAMERES_H
#define __ONAMERES_H

#ifndef __ORESDB_H
#include <ORESDB.H>
#endif

//------- Define NameRec --------//

//!Structure used to contain the middle or last as stored in the database
struct NameRec {
    enum { NAME_LEN=9 };
    char  name[NAME_LEN];
};

//------- Define Name --------//

//!Structure used to contain the middle or last.
struct NameInfo {
    enum { NAME_LEN=9 };
    char  name[NAME_LEN+1];
};

//------- Define FirstNameRec --------//

//!Structure containing the first name and gender as stored in the
//!database.
struct FirstNameRec {
    enum { NAME_LEN=9 };
    char  name[NAME_LEN];
    char  gender;
};

//------- Define FirstName --------//

//!Structure containing the first name and gender.
struct FirstNameInfo {
    enum { NAME_LEN=9 };
    char  name[NAME_LEN+1];
    char  gender;
};

//------- Define class NameRes ---------//

//!Resource class for loading and managing full name and gender as a
//!function of nameId.
class NameRes {
public:
    bool          init_flag;

    short         first_name_count;
    short         middle_name_count;
    short         last_name_count;

    FirstNameInfo*    first_name_array;
    NameInfo*         middle_name_array;
    NameInfo*       last_name_array;

public:
    NameRes();
    ~NameRes();

    void          init();
    void          deinit();

    char*         get_name_str(int firstNameId, int middleNameId, int lastNameId);
    bool          is_male(int firstNameId);

private:
    void          load_info(char* dbFileName, NameInfo** nameArrayPtr, short& nameCount);
    void          load_first_name();
};

extern NameRes name_res;

//-----------------------------------------------//
#endif
