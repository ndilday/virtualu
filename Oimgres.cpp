//Filename    : OIMGES.CPP
//Description : Object ImageRes

#include <string.h>

#include <ALL.H>
#include <OVGA.H>
#include <OCOLTBL.H>
#include <OMOUSE.H>
#include <OIMGRES.H>
#include <OIFACE.H>
#include <OBITMAP.H>

//--------- Format of RES file ------------//
//
// In the resource file, contain many compressed images, each image
// has the following data
//
// <char[8]> = the name of the image
// <int>     = the widht of the image
// <int>     = the height of the image
// <char...> = the bitmap of the image
//
//--------------------------------------------//

//------- Start of function ImageRes::ImageRes -------//
//
// <char*> resName   = name of the resource file (e.g. "GIF.RES")
// [int]   readAll   = whether read all data into the buffer or read one each time
//                     (default:0)
// [int]   useCommonBuf = whether use the sys common buffer to store the data or not
//                     (default:0)
//
ImageRes::ImageRes(char* resFile, int readAll, int useCommonBuf) :
    ResourceIdx(resFile, readAll, useCommonBuf) {
}

//--------- End of function ImageRes::ImageRes -------//

//-------- Start of function ImageRes::put_front --------//
//
// int 	x,y       = the location of the image
// char* imageName = name of the image
// [int] compressFlag = compress flag
//								(default: 0)
//
void ImageRes::put_front(int x, int y, char* imageName, int compressFlag) {
    char* bitmapPtr = ResourceIdx::read(imageName);

    if(!bitmapPtr)
	return;

    mouse.hide_area( x, y, x+*((short*)bitmapPtr)-1, y+*(((short*)bitmapPtr)+1)-1 );

    if( compressFlag )
	vga_front.put_bitmap_trans_decompress( x, y, bitmapPtr );
    else
	vga_front.put_bitmap_trans( x, y, bitmapPtr );

    mouse.show_area();
}

//---------- End of function ImageRes::put_front --------//

//-------- Start of function ImageRes::put_back --------//
//
// int 	x,y       = the location of the image
// char* imageName = name of the image
// [int] compressFlag = compress flag
//								(default: 0)
//
void ImageRes::put_back(int x, int y, char* imageName, int compressFlag) {
    char* bitmapPtr = ResourceIdx::read(imageName);

    /*	if(user_interface.backgd_flag==0)
	{
	if(strcmp(imageName,"QUEST_UP")==0)
	{
        vga_back.barW_fast(x,y,x+*(short*)bitmapPtr,y+*((short*)bitmapPtr+1)+1,0xFFFF);
        return;
	}
	}
    */
    if( bitmapPtr ) {
	if( compressFlag )
	    vga_back.put_bitmap_trans_decompress( x, y, bitmapPtr );
	else
	    vga_back.put_bitmap_trans( x, y, bitmapPtr );
    }
}

//---------- End of function ImageRes::put_back --------//

//-------- Start of function ImageRes::put_front --------//
//
// int x,y      = the location of the image
// int bitmapId = id. of the bitmap
// [int] compressFlag = compress flag
//								(default: 0)
//
void ImageRes::put_front(int x, int y, int bitmapId, int compressFlag) {
    char* bitmapPtr = ResourceIdx::get_data(bitmapId);

    if( !bitmapPtr )
	return;

    mouse.hide_area( x, y, x+*((short*)bitmapPtr)-1, y+*(((short*)bitmapPtr)+1)-1 );

    if( compressFlag )
	vga_front.put_bitmap_trans_decompress( x, y, bitmapPtr );
    else
	vga_front.put_bitmap_trans( x, y, bitmapPtr );

    mouse.show_area();
}

//---------- End of function ImageRes::put_front --------//

//-------- Start of function ImageRes::put_back --------//
//
// int ,y       = the location of the image
// int bitmapId = id. of the bitmap
// [int] compressFlag = compress flag
//								(default: 0)
//
void ImageRes::put_back(int x, int y, int bitmapId, int compressFlag) {
    char* bitmapPtr = ResourceIdx::get_data(bitmapId);

    if( bitmapPtr ) {
	if( compressFlag )
	    vga_back.put_bitmap_trans_decompress( x, y, bitmapPtr );
	else
	    vga_back.put_bitmap_trans( x, y, bitmapPtr );
    }
}

//---------- End of function ImageRes::put_back --------//

//-------- Start of function ImageRes::put_join --------//
//
// int 	x,y       = the location of the image
// char* imageName = name of the image
//
void ImageRes::put_join(int x, int y, char* imageName) {
    char* bitmapPtr = ResourceIdx::read(imageName);

    if( !bitmapPtr )
	return;

    mouse.hide_area( x, y, x+((Bitmap *)bitmapPtr)->get_width()-1, y+((Bitmap *)bitmapPtr)->get_height()-1 );

    if( bitmapPtr )
	vga_front.join_trans( &vga_back, x, y, bitmapPtr );

    mouse.show_area();
}

//---------- End of function ImageRes::put_join --------//

//-------- Start of function ImageRes::put_large --------//
//
// When a picture file is > 64K, which cannot be read into a single
// memory buffer.
//
// It will call vga.put_pict() which will continously read the file
// and put to the screen until completion.
//
// <VgaBuf*> vgaBuf 	  = the vga buffer for display
// <int>	    x,y       = the location of the image
// <char*>   imageName = name of the image
//
void ImageRes::put_large(VgaBuf* vgaBuf, int x, int y, char* imageName) {
    int dataSize;

    File* filePtr = get_file(imageName, dataSize);

    if (filePtr->file_get_short() != -1 ) {         // use common color palette
	filePtr->file_seek(filePtr->file_pos() - sizeof(short));
	vgaBuf->put_large_bitmap(x, y, filePtr);
    }
    else {                                          // use own color palette
	unsigned char palette[256 * 3];
	short *remapTable;
	filePtr->file_read(palette, 256 * 3);
	PalDesc palDesc(palette, 3, 256, 6);
	ColorTable colorTable;
	colorTable.generate_table_fast(MAX_BRIGHTNESS_ADJUST_DEGREE, palDesc, ColorTable::bright_func);
	remapTable = (short *) colorTable.get_table(0);
	vgaBuf->put_large_bitmap(x, y, filePtr, remapTable);
    }
}

//---------- End of function ImageRes::put_large --------//

//-------- Start of function ImageRes::put_large --------//
//
// When a picture file is > 64K, which cannot be read into a single
// memory buffer.
//
// It will call vga.put_pict() which will continously read the file
// and put to the screen until completion.
//
// <VgaBuf*> vgaBuf 	  = the vga buffer for display
// <int>	    x,y       = the location of the image
// <int>		 bitmapId  = id. of the bitmap in the bitmap resource file.
//
void ImageRes::put_large(VgaBuf* vgaBuf, int x, int y, int bitmapId) {
    int dataSize;

    vgaBuf->put_large_bitmap( x, y, ResourceIdx::get_file(bitmapId, dataSize) );
}

//---------- End of function ImageRes::put_large --------//

//-------- Start of function ImageRes::put_to_buf --------//
//
// Put the image to the specified Vga buffer.
//
// <VgaBuf*> vgaBufPtr = the pointer to the Vga buffer
// <char*>	 imageName = name of the image
//
void ImageRes::put_to_buf(VgaBuf* vgaBufPtr, char* imageName) {
    if( read_all ) {
	vgaBufPtr->put_bitmap(0, 0, get_ptr(imageName));
    }
    else {
	int dataSize;
	File* filePtr = get_file(imageName, dataSize);
	vgaBufPtr->put_large_bitmap(0, 0, filePtr);
    }
}

//---------- End of function ImageRes::put_to_buf --------//

//-------- Start of function ImageRes::put_to_buf --------//
//
// Put the image to the specified Vga buffer.
//
// <VgaBuf*> vgaBufPtr = the pointer to the Vga buffer
// <int>     bitmapId  = id. of the bitmap in the resource file.
//
void ImageRes::put_to_buf(VgaBuf* vgaBufPtr, int bitmapId) {
    if( read_all ) {
	vgaBufPtr->put_bitmap(0, 0, get_data(bitmapId));
    }
    else {
	int dataSize;
	File* filePtr = get_file(bitmapId, dataSize);
	vgaBufPtr->put_large_bitmap(0, 0, filePtr);
    }
}

//---------- End of function ImageRes::put_to_buf --------//
