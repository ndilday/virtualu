//Filename   : OSCHLRES.H
//Description: Object SchoolRes Declaration
//Owner       : Fred

#ifndef __OSCHLRES_H
#define __OSCHLRES_H

#ifndef __OSCHLEX_H
#include <OSCHLEX.h>
#endif

//----------- Define constant ----------//

#define   MAX_SCHOOL_EX_FOR_INIT_CONDITION  10    // the number of school used for generating initial conditions
#define MAX_PEER_SCHOOL_COUNT       12
#define MAX_FIRST_SCHOOL            30

class PeerSchool;
class PeerInfo;

//----------- Define class SchoolRes -----------//

//!School resourse class which loads all SchoolRecords (schools, peers, and
//!the players) from db. Responds to init_game() and read/write.
class SchoolRes {
public:
    short           db_school_count;
    School*         db_school_array;              // an array of schools read from the database

    School*       desired_school_array;
    short         desired_school_count;

    short           peer_info_count;
    PeerInfo*       peer_info_array;              // an array of peer schools read from the database

    SchoolEx        school_ex_array[MAX_SCHOOL_EX_FOR_INIT_CONDITION];
    PeerSchool*     player_peer_school;

    short         peer_school_count;
    PeerSchool*     peer_school_array;            // includes player_peer_school after battle.create_object()

    // peer_school_rank[] store the index of peer_school_array; index 0 refer to highest score
    short         peer_school_rank[MAX_PEER_SCHOOL_COUNT];

public:
    void          init();
    void          deinit();

    int         write_file(File* filePtr);
    int         read_file(File* filePtr);

    void            init_game();
    void          next_day();                     //### fred 1027 ###//

#ifdef DEBUG
    School*       get_db_school(int recno);
#else
    School*       get_db_school(int recno) { return db_school_array+recno-1; }
#endif

    void          get_desired_school_array();
    void          get_50_school_array();

private:
    void            load_db_info();
    void          free_db_info();

    void          build_school_ex_array();
    void          init_player_school_ex();
    void          build_peer_school_array();

    void          load_db_1();
    void          load_db_2();
    void          load_db_peer();
};

extern SchoolRes school_res;

//---------------------------------------------//
#endif
