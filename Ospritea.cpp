//Filename    : OSPRITEA.CPP
//Description : Object SpriteArray
//Owner       : Fred

//#include <ALL.H>
//#include <OVGA.H>
//#include <OSTR.H>
//#include <OSYS.H>
//#include <OSPRTRES.H>
#include <OSPRITE.H>
#include <OSPRITEA.H>
//#include <OUNIT.H>

/*#ifdef DEBUG
#include <OBULLET.H>
#include <OTORNADO.H>

static int num_of_unit;
#endif*/

//--------- Begin of function SpriteArray::SpriteArray ---------//
//! <int> initArraySize - the initial size of this array.
//!
SpriteArray::SpriteArray(int initArraySize) : DynArrayB(sizeof(Sprite*),initArraySize, DEFAULT_REUSE_INTERVAL_DAYS) {
}

//----------- End of function SpriteArray::SpriteArray ---------//

//--------- Begin of function SpriteArray::~SpriteArray ---------//
SpriteArray::~SpriteArray() {
    deinit();
}

//----------- End of function SpriteArray::~SpriteArray ---------//

//--------- Begin of function SpriteArray::init ---------//
void SpriteArray::init() {
    restart_recno = 1;
}

//----------- End of function SpriteArray::init ---------//

//--------- Begin of function SpriteArray::deinit ---------//
//! All firms should be deleted when the system terminated
//!
//! Chain : delete nation --> delete firm --> delete job --> delete item
//!
//! Some data is corrupted when the some firms still exist when
//! the system terminated
//!
void SpriteArray::deinit() {
    if( size()==0 )
	return;

    //----- delete sprite objects ------//

    Sprite* emptyPtr = NULL;
    Sprite* spritePtr;

    for( int i=1 ; i<=size() ; i++ ) {
	spritePtr = (Sprite*) get_ptr(i);

	if( spritePtr )
	    delete spritePtr;

	update( &emptyPtr, i );                       // set the pointer in SpriteArray to NULL, so is_deleted() can return correct result, this is needed as Unit::deinit() will call is_deleted()
    }

    //-------- zap the array -----------//

    zap();
}

//----------- End of function SpriteArray::deinit ---------//

//--------- Begin of function SpriteArray::add ---------//
void SpriteArray::add(Sprite *newSprite) {
    linkin(&newSprite);

    newSprite->sprite_recno = recno();
}

//----------- End of function SpriteArray::add -----------//

//--------- Begin of function SpriteArray::add_sorted ---------//
//! Add the sprite into the array in a sorted order.
//!
//! <Sprite*> spritePtr - pointer to the sprite to be added
//!
//! Note: it does not call Sprite::init_recno() as it is supposed to be used by disp_sprite_array
//!		   only and sprites to be added are existing sprites only.
//!
//! return : <int> - the recno of the newly added sprite in SpriteArray.
//!
void SpriteArray::add_sorted(Sprite *newSprite) {
    /*
      int l=0, r=size(), x=0;
      int addY  = newSprite->abs_y2;
      int testY = addY + 1;

      //---------------------------------------------------------------------//
      // Use a binary search to find the right location to add the new sprite.
      //---------------------------------------------------------------------//

      while (r > l)
      {
      x = (l + r) / 2;

      testY = operator[](x+1)->abs_y2;   // the one to be compared with the adding one.

      if (addY < testY)
      r = x;
      else
      l = x + 1;

      if (addY == testY)
      break;
      }

      if (addY >= testY)
      x++;

      insert_at(x+1, &newSprite);
    */
}

//----------- End of function SpriteArray::add_sorted -----------//

//--------- Begin of function SpriteArray::del ---------//
void SpriteArray::del(int recNo) {
    Sprite* spritePtr = (Sprite*) get_ptr(recNo);

    err_when( !spritePtr );

    delete spritePtr;

    linkout(recNo);
}

//----------- End of function SpriteArray::del -----------//

//--------- Begin of function SpriteArray::process ---------//
void SpriteArray::process() {
    Sprite* spritePtr;
    int i;
    int arraySize = size();

    for ( i=1; i<=arraySize; i++) {
	spritePtr = (Sprite *) get_ptr(i);
	spritePtr->process();
    }
}

//----------- End of function SpriteArray::process -----------//

//------- Begin of function SpriteArray::operator[] -----//
#ifdef DEBUG
Sprite* SpriteArray::operator[](int recNo) {
    Sprite* spritePtr = (Sprite*) get_ptr(recNo);

    if( !spritePtr )
	err.run( "SpriteArray[] is deleted" );

    return spritePtr;
}
#endif
//--------- End of function SpriteArray::operator[] ----//
