//Filename    : OGFILE.H
//Description : Oject Game file, save and restore game
//

#ifndef __OGFILE_H
#define __OGFILE_H

#include <OFILE.H>
#include <ODYNARR.H>
#include <OPSCHOOL.H>

#define MIN_FREE_SPACE 1000

//-------- Define struct HallFame ----------//

enum {                                            // No. of Hall of Fame entries
    HALL_FAME_NUM = 5
};

//!Class for displaying and recording the information displayed in the hall
//!of fame.
struct HallFame {                                 // Hall of Fame
    char school_name[PlayerSchool::SCHOOL_NAME_LEN+1];
    char  player_first_name[PlayerSchool::FIRST_NAME_LEN+1];
    char  player_last_name[PlayerSchool::LAST_NAME_LEN+1];

    int   score;                                    // it's the sorting key of hall_fame_array
    int   trust_score;

    short start_year;
    short end_year;
    short scenario_id;                              // 0: invalid

    //short difficulty_rating;

public:
    void  disp_info(int,int,int);
    void  record_data(int);
};

//-------- Define class GameFile -----------//

//!Class for saving/retrieving complete game state to/from a file. Games
//!are saved to individual files with a .sav extension in the current
//!directory.
class GameFile {
    enum { PLAYER_NAME_LEN = 41};

public:
    size_t   class_size;                          // for version compare
    char     file_name[MAX_PATH+1];
    char*   get_save_name();

    char     player_name[PLAYER_NAME_LEN];

    int      game_date;                           // the game date of the saved game
    FILETIME file_date;                           // saving game date
    int   school_type;

    static   File* file_ptr;
    static   char  last_read_success_flag;

    //### begin zb 02/25/99
    int   game_speed;
    //### end zb 02/25/99

public:
    int   save_game(char* =NULL);
    int   load_game(char* =NULL);

    int   save_express_game(char* =NULL);
    int   load_express_game(char* =NULL);

    void  set_file_name();
    //void  disp_info(int x, int y);
    int   validate_header();

    // write log file
    int create_game_log(char *);
    int append_game_log(char *);

private:
    void  save_process();
    void  load_process();
    int   write_game_header(File* filePtr);

    int   write_file(File*);
    int   write_file_1(File*);
    int   write_file_2(File*);
    int   write_file_3(File*);

    int   read_file(File*);
    int   read_file_1(File*);
    int   read_file_2(File*);
    int   read_file_3(File*);

    void  write_book_mark(short bookMark);
    int   read_book_mark(short bookMark);

    // write log
    int   write_log_header(File*);
    int   write_log(File*);
};

//------- Define class GameFileArray --------//

//!Collection class containing all of the GameFiles in the hall of fame.
//!Also used to load the headers of the saved games for menu selection.
class GameFileArray : public DynArray {
    //-----------------------------------------//
public:
    //int  save_game()    { return menu(1); }
    //int  load_game()    { return menu(2); }
    //int  menu(int, int *recno=NULL);

private:
    //void disp_browse();
    //int  process_action(int=0);

    //-----------------------------------------//
public:
    bool  demo_format;
    char  has_read_hall_of_fame;
    char last_file_name[MAX_PATH+1];

    HallFame hall_fame_array[HALL_FAME_NUM];

public:
    GameFileArray();

    void init(char *extStr);
    void deinit();

    void save_new_game(char* =NULL);              // save a new game immediately without prompting menu
    void del_game(int recNo);
    void del_express_game(int recNo);

    int  read_hall_of_fame();
    int  write_hall_of_fame();                    // it may be called by group_res.gen_group() in writting default name

    //void disp_hall_of_fame();
    int  add_hall_of_fame(int);

    GameFile* operator[](int recNo);

private:
    void load_all_game_header(char *extStr, int readAutoSave=1);
};

extern GameFileArray game_file_array;
extern GameFile      game_file;                   // for saving autosave...

//-----------------------------------------
#endif
