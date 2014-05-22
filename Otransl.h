//Filename    : OTRANSL.H
//Description : Multi-lingual Translation Class Header

#ifndef __OTRANSL_H
#define __OTRANSL_H

//--------- Define struct TranslateRec -------//

//!Structure containing the source text and the destination text as stored
//!in the translation table resource.
struct TranslateRec {
    char* from_text_ptr;
    char* to_text_ptr;
};

//---------- Define class Translate -----------//

//!Multi-lingual translation class used for looking up predefined strings
//!in a translation table resource.
class Translate {
private:
    char      init_flag;
    int     rec_count;                            // no. of translation records.

    char*   translate_text_buf;
    TranslateRec* translate_table;

    short   quick_seek_table[256];                // ascii from 0 to 255, total 256 records, storing record numbers pointing to translate_table

public:
    Translate();
    ~Translate()   { deinit(); }

    void init();
    void deinit();

    char* process(char*);
};

extern Translate translate;

//--------------------------------------------------//
#endif
