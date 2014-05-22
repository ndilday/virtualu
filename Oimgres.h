//Filename    : OIMAGERES.H
//Description : Object ImageRes

#ifndef __OIMAGERES_H
#define __OIMAGERES_H

#ifndef  __ORESX_H
#include <ORESX.H>
#endif

//----------------------------------------//

class VgaBuf;

//!Class for managing Image resources. The resource files contain many
//!compressed images, stored as follows: { <char[8]> = the name of the
//!image, <int> = the widht of the image, <int> = the height of the image,
//!<char...> = the bitmap of the image }.
class ImageRes : public ResourceIdx {
public:
    ImageRes()  {;}
    ImageRes(char* resFile, int readAll=0, int useVgaBuf=0);

    void  put_front(int,int,char*, int compressFlag=0);
    void  put_back(int,int,char*, int compressFlag=0);

    void  put_front(int,int,int, int compressFlag=0);
    void  put_back(int,int,int, int compressFlag=0);

    void  put_join(int,int,char*);

    char* get_ptr(char* imageName)   { return ResourceIdx::read(imageName); }

    void  put_large(VgaBuf*,int,int,char*);       // put a large image, over 64K
    void  put_large(VgaBuf*,int,int,int);

    void  put_to_buf(VgaBuf* vgaBufPtr, char* imageName);
    void  put_to_buf(VgaBuf* vgaBufPtr, int bitmapId);
};

extern ImageRes image_interface, image_sys, image_mascot, image_pict;

//--------------------------------------------//
#endif
