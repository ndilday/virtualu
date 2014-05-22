//Filename    : OSPRITEA.h
//Description : SpriteArray Class Declaration
//Owner       : Fred

#ifndef __OSPRITEA_H
#define __OSPRITEA_H

#include <ODYNARRB.H>

class Sprite;
//------- Define class SpriteArray ---------//
//!Collection of sprite objects. SpriteRes is now used instead.
class SpriteArray : public DynArrayB {
public:
    short   restart_recno;                        // indicate the unit's sprite_recno to process first in next process_cycle

    SpriteArray(int initArraySize=20);
    ~SpriteArray();

    void  init();
    void  deinit();

    void  add(Sprite*);
    void  add_sorted(Sprite*);
    void  del(int);

    virtual void process();

#ifdef DEBUG
    Sprite* operator[](int recNo);
#else
    Sprite* operator[](int recNo)   { return (Sprite*) get_ptr(recNo); }
#endif
};

extern SpriteArray sprite_array;
#endif
