//Filename    : OVGABUF.CPP
//Description : OVGABUF direct draw surface class

#include <OVGABUF.H>
#include <OVGA.H>
#include <OCOLTBL.H>
#include <ALL.H>
#include <OMOUSE.H>
#include <IMGFUN.H>
#include <OSYS.H>
#include <OWORLD.H>
#include <OBITMAP.H>
#include <OBITMAPW.H>

short *VgaBuf::default_remap_table;

//-------- Begin of function VgaBuf::VgaBuf ----------//

VgaBuf::VgaBuf() {
    memset( this, 0, sizeof(VgaBuf) );
    set_color( VGA_GRAY );
}

//-------- End of function VgaBuf::VgaBuf ----------//

//-------- Begin of function VgaBuf::~VgaBuf ----------//

VgaBuf::~VgaBuf() {
    deinit();
}

//-------- End of function VgaBuf::~VgaBuf ----------//

//-------- Begin of function VgaBuf::init_front ----------//
//!
//! Create a direct draw front buffer.
//!
void VgaBuf::init_front(LPDIRECTDRAW4 ddPtr) {
    DDSURFACEDESC2       ddsd;
    HRESULT             rc;
    DDCAPS              ddcaps;

    //------ Get Direct Draw capacity info --------//

    ddcaps.dwSize = sizeof( ddcaps );

    if( ddPtr->GetCaps( &ddcaps, NULL ) != DD_OK )
	err.run( "Error creating Direct Draw front surface!" );

    //---------------------------------------------//
    // Create the Front Buffer
    //---------------------------------------------//

    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize = sizeof( ddsd );

    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    //	LPDIRECTDRAWSURFACE dd1Buf;
    rc = ddPtr->CreateSurface( &ddsd, &dd_buf, NULL );
    if( rc != DD_OK )
	err.run( "Error creating Direct Draw front surface!" );

    //	rc = dd1Buf->QueryInterface(IID_IDirectDrawSurface2, (void **)&dd_buf);
    //	if( rc != DD_OK )
    //	{
    //		dd1Buf->Release();
    //		err.run ( "Error creating Direct Draw front surface!!" );
    //	}

    //	dd1Buf->Release();

    lock_bit_stack = 0;
    lock_stack_count = 0;

    default_remap_table = vga.default_remap_table;  // new for 16-bit

    is_front = 1;
}

//-------- End of function VgaBuf::init_front ----------//

//-------- Begin of function VgaBuf::init_back ----------//
//!
//! Create a direct draw back buffer.
//!
//! [DWORD] w      : width of the surface [default 0 : VGA_WIDTH]
//! [DWORD] h      : height of the surface [default 0 : VGA_HEIGHT]
//!
void VgaBuf::init_back( LPDIRECTDRAW4 ddPtr, DWORD w, DWORD h ) {
    DDSURFACEDESC2       ddsd;
    HRESULT             rc;

    //--------- fill in surface desc -----------//

    memset( &ddsd, 0, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;

    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

    ddsd.dwWidth  = w ? w : VGA_WIDTH;
    ddsd.dwHeight = h ? h : VGA_HEIGHT;

    //	LPDIRECTDRAWSURFACE dd1Buf;
    rc = ddPtr->CreateSurface( &ddsd, &dd_buf, NULL );
    if( rc != DD_OK )
	err.run( "Error creating direct draw back surface!" );

    //	rc = dd1Buf->QueryInterface(IID_IDirectDrawSurface2, (void **)&dd_buf);
    //	if( rc != DD_OK )
    //	{
    //		dd1Buf->Release();
    //		err.run( "Error creating direct draw back surface!!" );
    //	}

    //	dd1Buf->Release();

    lock_bit_stack = 0;
    lock_stack_count = 0;

    default_remap_table = vga.default_remap_table;  // new for 16-bit
}

//-------- End of function VgaBuf::init_back ----------//

//------ Begin of function VgaBuf::deinit --------//

void VgaBuf::deinit() {
    if( dd_buf ) {
	dd_buf->Release();
	dd_buf = NULL;
    }
}

//-------- End of function VgaBuf::deinit ----------//

//-------- Begin of function VgaBuf::activate_pal ----------//
//!
//! Activate a palette to the current direct draw surface buffer.
//!
void VgaBuf::activate_pal(LPDIRECTDRAWPALETTE ddPalPtr) {
    return;                                         // no need for 16-bit

    err_when(!ddPalPtr || !dd_buf);

    HRESULT rc = dd_buf->SetPalette(ddPalPtr);

    if( rc == DDERR_SURFACELOST ) {
	dd_buf->Restore();
	rc = dd_buf->SetPalette(ddPalPtr);
    }

#ifdef DEBUG
    if( rc != DD_OK )
	debug_msg( "VgaBuf::activate_pal(), failed activating the palette" );
#endif
}

//--------- End of function VgaBuf::activate_pal ----------//

//-------- Begin of function VgaBuf::color_match ----------//

DWORD VgaBuf::color_match(COLORREF rgb) {
    COLORREF    rgbT;
    HDC         hdc;
    DWORD       dw = CLR_INVALID;
    DDSURFACEDESC2  ddsd;
    HRESULT       hres;

    if( dd_buf->GetDC(&hdc) == DD_OK ) {
	rgbT = GetPixel(hdc, 0, 0);
	SetPixel(hdc, 0, 0, rgb);
	dd_buf->ReleaseDC(hdc);
    }

    ddsd.dwSize = sizeof(ddsd);
    hres = dd_buf->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

    if (hres == DD_OK) {
	dw  = *(DWORD *)ddsd.lpSurface;
	dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1;
	dd_buf->Unlock(NULL);
    }

    if (dd_buf->GetDC(&hdc) == DD_OK) {
	SetPixel(hdc, 0, 0, rgbT);
	dd_buf->ReleaseDC(hdc);
    }

    return dw;
}

//-------- End Endof function VgaBuf::color_match ----------//

//-------- Begin of function VgaBuf::is_buf_lost ----------//
//!
BOOL VgaBuf::is_buf_lost() {
    return dd_buf && dd_buf->IsLost() == DDERR_SURFACELOST;
}

//--------- End of function VgaBuf::is_buf_lost ----------//

//-------- Begin of function VgaBuf::restore_buf ----------//
//!
//! Restore buffers that have been lost.
//!
BOOL VgaBuf::restore_buf() {
    if( dd_buf == NULL || dd_buf->Restore() != DD_OK ) {
#ifdef DEBUG
	debug_msg("Error restoring direct draw buffer");
#endif
	return FALSE;
    }

    return TRUE;
}

//--------- End of function VgaBuf::restore_buf ----------//

//------------- Begin of function VgaBuf::lock_buf --------------//

void VgaBuf::lock_buf() {
    err_if( buf_locked )
	err_now( "VgaBuf::lock_buf() error, buffer already locked." );

    memset( &buf_des, 0, sizeof(buf_des) );

    buf_des.dwSize = sizeof(buf_des);

    int rc = dd_buf->Lock(NULL, &buf_des, DDLOCK_WAIT, NULL);

    cur_buf_ptr = (short *) buf_des.lpSurface;

    //--------------------------------------//

    if( rc==DD_OK )
	buf_locked = TRUE;
    else {
	if( is_front )
	    err_now( "VgaBuf::lock_buf() locking front buffer failed." );
	else
	    err_now( "VgaBuf::lock_buf() locking back buffer failed." );

#ifdef DEBUG
	debug_msg( "Failed to lock the buffer." );
#endif
    }
}

//--------------- End of function VgaBuf::lock_buf --------------//

//------------- Begin of function VgaBuf::unlock_buf --------------//

void VgaBuf::unlock_buf() {
    // ####### begin Gilbert 16/9 #####//
    if( !dd_buf )
	return;
    // ####### end Gilbert 16/9 #####//

    err_when( !buf_locked );

    int rc = dd_buf->Unlock(NULL);

    if( rc==DD_OK )
	buf_locked = FALSE;
    else {
	switch(rc) {
	case DDERR_INVALIDOBJECT:
	    err_now( "VgaBuf::unlock_buf error: DDERR_INVALIDOBJECT" );

	case DDERR_INVALIDPARAMS:
	    err_now( "VgaBuf::unlock_buf error: DDERR_INVALIDPARAMS" );

	case DDERR_INVALIDRECT:
	    err_now( "VgaBuf::unlock_buf error: DDERR_INVALIDRECT" );

	case DDERR_NOTLOCKED:
	    err_now( "VgaBuf::unlock_buf error: DDERR_NOTLOCKED" );

	case DDERR_SURFACELOST:
	    err_now( "VgaBuf::unlock_buf error: DDERR_SURFACELOST" );
	}

	if( is_front )
	    err_now( "VgaBuf::unlock_buf() unlocking front buffer failed." );
	else
	    err_now( "VgaBuf::unlock_buf() unlocking back buffer failed." );

#ifdef DEBUG
	debug_msg( "Failed to unlock the buffer." );
#endif
    }
}

//--------------- End of function VgaBuf::unlock_buf --------------//

//------------- Begin of function VgaBuf::temp_unlock --------------//
//!
//! Unlock the Vga buffer temporarily.
//!
void VgaBuf::temp_unlock() {
    // push buf_locked
    err_when(lock_stack_count >= sizeof(lock_bit_stack)*8);
    err_when(buf_locked != 0 && buf_locked != 1);   // 0 or 1
    lock_bit_stack = (lock_bit_stack << 1) | buf_locked;
    ++lock_stack_count;

    if( buf_locked )
	unlock_buf();

    err_when( buf_locked );
}

//--------------- End of function VgaBuf::temp_unlock --------------//

//------------- Begin of function VgaBuf::temp_restore_lock --------------//
//!
//! Restore the previous lock stage.
//! paired with temp_unlock()
//!
void VgaBuf::temp_restore_lock() {
    // pop buf_locked
    err_when(lock_stack_count==0);
    DWORD newBufLocked = lock_bit_stack & 1;
    lock_bit_stack >>= 1;
    lock_stack_count--;

    if( newBufLocked )
	lock_buf();
}

//--------------- End of function VgaBuf::temp_restore_lock --------------//

//------------- Begin of function VgaBuf::temp_lock --------------//
//!
//! Lock the Vga buffer temporarily.
//!
void VgaBuf::temp_lock() {
    // push buf_locked
    err_when(lock_stack_count >= sizeof(lock_bit_stack)*8);
    err_when(buf_locked != 0 && buf_locked != 1);   // 0 or 1
    lock_bit_stack = (lock_bit_stack << 1) | buf_locked;
    ++lock_stack_count;

    if( !buf_locked )
	lock_buf();
    err_when( !buf_locked );
}

//------------- End of function VgaBuf::temp_lock --------------//

//------------- Begin of function VgaBuf::temp_restore_unlock --------------//
//!
//! Restore the previous unlock stage.
//! paired with temp_lock()
//!
void VgaBuf::temp_restore_unlock() {
    // pop buf_locked
    err_when(lock_stack_count==0);
    DWORD newBufLocked = lock_bit_stack & 1;
    lock_bit_stack >>= 1;
    lock_stack_count--;

    if( !newBufLocked )
	unlock_buf();
}

//------------- End of function VgaBuf::temp_restore_unlock --------------//

//------------- Begin of function VgaBuf::put_bitmap --------------//
//!
//! Put a bitmap on the surface buffer
//!
void VgaBuf::put_bitmap(int x,int y,char* bitmapPtr) {
    err_when( !buf_locked );

    if( is_front )
	mouse.hide_area( x, y, x+*((short*)bitmapPtr)-1, y+*(((short*)bitmapPtr)+1)-1 );

    //	##chwg1130
    if((bitmapPtr[0]==-1)&&(bitmapPtr[1]==-1)) 
	{
		//create new 16bit palette
		short newPalette[256];
		int p;
		for(int i=0,p=2;i<256;i++,p+=sizeof(RGBColor))
		{
			newPalette[i] = (short)vga.make_pixel(
				(BYTE)bitmapPtr[p] << 2,
				(BYTE)bitmapPtr[p+1] << 2,
				(BYTE)bitmapPtr[p+2] << 2);
		}
		//		  newPalette[i] = (short)vga.make_pixel( (RGBColor *)(bitmapPtr + p) );
		put_bitmap_remap(x,y,bitmapPtr+p,newPalette);
    }
    else
	IMGbltRemap(buf_ptr(), buf_true_pitch(), x, y, bitmapPtr, default_remap_table);

    if( is_front )
	mouse.show_area();
}

//--------------- End of function VgaBuf::put_bitmap --------------//

//------- Begin of function VgaBuf::put_bitmap_trans --------//
//!
//! Put a bitmap on the surface buffer and hide the mouse cursor while displaying
//!
void VgaBuf::put_bitmap_trans(int x,int y,char* bitmapPtr) {
    err_when( !buf_locked );

    if( is_front )
	mouse.hide_area( x, y, x+*((short*)bitmapPtr)-1, y+*(((short*)bitmapPtr)+1)-1 );

    //	##chwg1130
    if((bitmapPtr[0]==-1)&&(bitmapPtr[1]==-1)) {
	//create new 16bit palette
	short newPalette[256];
	int p;
	for(int i=0,p=2;i<256;i++,p+=sizeof(RGBColor))
	    newPalette[i] = (short)vga.make_pixel(
		(BYTE)bitmapPtr[p] << 2,
		(BYTE)bitmapPtr[p+1] << 2,
		(BYTE)bitmapPtr[p+2] << 2);
	IMGbltTransRemap(buf_ptr(), buf_true_pitch(), x, y, bitmapPtr+p,newPalette);
    }
    else
	IMGbltTransRemap(buf_ptr(), buf_true_pitch(), x, y, bitmapPtr, default_remap_table);

    if( is_front )
	mouse.show_area();
}

//--------- End of function VgaBuf::put_bitmap_trans --------//

//------- Begin of function VgaBuf::put_bitmap_remap --------//
//!
//! Put a bitmap on the surface buffer and hide the mouse cursor while displaying
//!
void VgaBuf::put_bitmap_remap(int x,int y,char* bitmapPtr,short *colorTable) {
    err_when( !buf_locked );

    if( is_front )
	mouse.hide_area( x, y, x+((Bitmap *)bitmapPtr)->get_width()-1, y+((Bitmap*)bitmapPtr)->get_height()-1 );

    IMGbltRemap(buf_ptr(), buf_true_pitch(), x, y, bitmapPtr, colorTable);

    if( is_front )
	mouse.show_area();
}

//--------- End of function VgaBuf::put_bitmap_remap --------//

//---------- Begin of function VgaBuf::save_area_common_buf ----------//
//!
//! Save screen area to sys.common_data_buf.
//!
void VgaBuf::save_area_common_buf(int x1, int y1, int x2, int y2) {
    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    long saveSize = sizeof(short)*4 + BitmapW::size(x2-x1+1, y2-y1+1);

    err_if( saveSize > COMMON_DATA_BUF_SIZE )
	err_now( "VgaBuf::save_area_common_buf()" );

    short* shortPtr = (short*) sys.common_data_buf;

    *shortPtr++ = x1;
    *shortPtr++ = y1;
    *shortPtr++ = x2;
    *shortPtr++ = y2;

    //-------- read screen ---------//

    if( is_front )
	mouse.hide_area( x1,y1,x2,y2 );               // if the mouse cursor is in that area, hide it

    read_bitmapW( x1,y1,x2,y2, shortPtr );

    if( is_front )
	mouse.show_area();
}

//------------ End of function VgaBuf::save_area_common_buf ----------//

//---------- Begin of function VgaBuf::rest_area_common_buf ----------//
//!
//! Restore screen area to the screen from Vga image buffer.
//! This screen should be previously saved by save_area()
//!
void VgaBuf::rest_area_common_buf() {
    short* shortPtr = (short*) sys.common_data_buf;

    int x1 = *shortPtr++;
    int y1 = *shortPtr++;
    int x2 = *shortPtr++;
    int y2 = *shortPtr++;

    put_bitmapW( x1, y1, shortPtr );
}

//------------ End of function VgaBuf::rest_area_common_buf ----------//

//---------- Begin of function VgaBuf::save_area ---------//
//!
//! save_area() differs from save_area() as :
//!
//! save_area() save the screen to a user-defined buffer.
//! save_area()  save the screen to the global screen saving buffer in Vga object
//!
//! <int>   x1,y1,x2,y2 = the area of the screen
//! [char*] saveScr     = the pointer to the previous saved buffer
//!                       (default : initialize a new buffer)
//!
short* VgaBuf::save_area(int x1, int y1, int x2, int y2, short* saveScr ) {
    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    long newSize = sizeof(short)*4 + BitmapW::size((x2-x1+1),(y2-y1+1));

    saveScr = (short *)mem_resize( saveScr, newSize );

    short* shortPtr = (short*) saveScr;

    *shortPtr++ = x1;
    *shortPtr++ = y1;
    *shortPtr++ = x2;
    *shortPtr++ = y2;

    if( is_front )
	mouse.hide_area( x1,y1,x2,y2 );               // if the mouse cursor is in that area, hide it

    read_bitmapW( x1,y1,x2,y2, shortPtr );

    if( is_front )
	mouse.show_area();

    return saveScr;
}

//------------ End of function VgaBuf::save_area ---------//

// ---  begin  ##chwg1021 --- //
//----------- Begin of function VgaBuf::rest_area_with_clipping --------//
//!
//! Restore previously saved screen
//!
//! short* saveScr     = the previously saved screen
//! short cx1
//! short cy1
//! short cx2
//! short cy2
//! [int] releaseFlag = whether release the buffer of the saved screen or not
//!                     (default : 1)
//! [int] transparentFlag = whether put with transparent processing (default:0)
//!
void VgaBuf::rest_area_with_clipping(short* saveScr,
				     int cx1, int cy1,
				     int cx2, int cy2,
				     int releaseFlag, int transparentFlag) {
    int x1,y1,x2,y2;

    if( saveScr == NULL )
	return;

    short* shortPtr = (short*) saveScr;

    x1 = *shortPtr++;
    y1 = *shortPtr++;
    x2 = *shortPtr++;
    y2 = *shortPtr++;

    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    if( transparentFlag )
	put_bitmapW_area_trans( x1, y1, shortPtr, cx1, cy1, cx2, cy2);
    else
	put_bitmapW_area( x1, y1, shortPtr, cx1, cy1, cx2, cy2);

    if( releaseFlag )
	mem_del( saveScr );
}

//------------ End of function VgaBuf::rest_area_with_clipping ----------//
// ---  end  ##chwg1021 --- //

//----------- Begin of function VgaBuf::rest_area --------//
//!
//! Restore previously saved screen
//!
//! char* saveScr     = the previously saved screen
//! [int] releaseFlag = whether release the buffer of the saved screen or not
//!                     (default : 1)
//! [int] transparentFlag = whether put with transparent processing (default:0)
//!
void VgaBuf::rest_area(short* saveScr, int releaseFlag, int transparentFlag) {
    int  x1,y1,x2,y2;

    if( saveScr == NULL )
	return;

    short* shortPtr = (short*) saveScr;

    x1 = *shortPtr++;
    y1 = *shortPtr++;
    x2 = *shortPtr++;
    y2 = *shortPtr++;

    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    if( transparentFlag )
	put_bitmapW_trans( x1, y1, shortPtr );
    else
	put_bitmapW( x1, y1, shortPtr );

    if( releaseFlag )
	mem_del( saveScr );
}

//------------ End of function VgaBuf::rest_area ----------//

///////////////////////
// old version 8 bit //
///////////////////////
//------------ Begin of function VgaBuf::write_bmp_file --------------//
//!
//! Load a BMP file into the current VgaBuf DIB object.
//!
//! <char*> fileName - the name of the BMP file.
//!
//! return : <int> 1-succeeded, 0-failed.
//!
/*
int VgaBuf::write_bmp_file(char* fileName)
{
   File				bmpFile;
   BITMAPINFO*	bmpInfoPtr = NULL;
   char*			bitmapPtr = NULL;

   bmpFile.file_create(fileName, 1, 0);		// 1-handle error, 0-disable variable file size

   //------------ Write the file header ------------//

BITMAPFILEHEADER bmpFileHdr;

bmpFileHdr.bfType 		= 0x4D42;			// set the type to "BM"
bmpFileHdr.bfSize 		= buf_size();
bmpFileHdr.bfReserved1 = 0;
bmpFileHdr.bfReserved2 = 0;
bmpFileHdr.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;

bmpFile.file_write(&bmpFileHdr, sizeof(bmpFileHdr));

//------------ Write in the info header -----------//

BITMAPINFOHEADER bmpInfoHdr;

bmpInfoHdr.biSize			 = sizeof(BITMAPINFOHEADER);
bmpInfoHdr.biWidth			 = buf_des.dwWidth;
bmpInfoHdr.biHeight			 = buf_des.dwHeight;
bmpInfoHdr.biPlanes			 = 1;
bmpInfoHdr.biBitCount		 = 8;
bmpInfoHdr.biCompression	 = BI_RGB;
bmpInfoHdr.biSizeImage	    = buf_size();
bmpInfoHdr.biXPelsPerMeter = 0;
bmpInfoHdr.biYPelsPerMeter = 0;
bmpInfoHdr.biClrUsed		 = 0;
bmpInfoHdr.biClrImportant  = 0;

bmpFile.file_write(&bmpInfoHdr, sizeof(bmpInfoHdr));

//------------ write the color table -----------//

LPDIRECTDRAWPALETTE ddPalettePtr;				// get the direct draw surface's palette
dd_buf->GetPalette(&ddPalettePtr);

PALETTEENTRY *palEntries = (PALETTEENTRY*) mem_add( sizeof(PALETTEENTRY)*256 );
ddPalettePtr->GetEntries(0, 0, 256, palEntries);

RGBQUAD *colorTable = (RGBQUAD*) mem_add( sizeof(RGBQUAD)*256 );		// allocate a color table with 256 entries

for( int i=0 ; i<256 ; i++ )
{
colorTable[i].rgbBlue  = palEntries[i].peBlue;
colorTable[i].rgbGreen = palEntries[i].peGreen;
colorTable[i].rgbRed   = palEntries[i].peRed;
colorTable[i].rgbReserved = 0;
}

bmpFile.file_write(colorTable, sizeof(RGBQUAD)*256);

mem_del(palEntries);
mem_del(colorTable);

//----------- write the bitmap ----------//

for( int y=buf_height()-1 ; y>=0 ; y-- )					// write in reversed order as DIB's vertical order is reversed
bmpFile.file_write(buf_ptr(0,y), buf_width());

//------------ close the file -----------//

bmpFile.file_close();

return 1;
}
//------------ End of function VgaBuf::write_bmp_file --------------//
*/

//------------ Begin of function VgaBuf::write_bmp_file --------------//
//!
//! Load a BMP file into the current VgaBuf DIB object.
//!
//! <char*> fileName - the name of the BMP file.
//!
//! return : <int> 1-succeeded, 0-failed.
//!
int VgaBuf::write_bmp_file(char* fileName) {
    File       bmpFile;
    BITMAPINFO*  bmpInfoPtr = NULL;
    char*      bitmapPtr = NULL;

    int        hasPaletteFlag = 0;

    bmpFile.file_create(fileName, 1, 0);            // 1-handle error, 0-disable variable file size

    //------------ Write the file header ------------//

    BITMAPFILEHEADER bmpFileHdr;

    bmpFileHdr.bfType    = 0x4D42;                  // set the type to "BM"
    bmpFileHdr.bfSize    = buf_size();
    bmpFileHdr.bfReserved1 = 0;
    bmpFileHdr.bfReserved2 = 0;
    bmpFileHdr.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    if( hasPaletteFlag )
	bmpFileHdr.bfOffBits += sizeof(RGBQUAD)*256;

    bmpFile.file_write(&bmpFileHdr, sizeof(bmpFileHdr));

    //------------ Write in the info header -----------//

    BITMAPINFOHEADER bmpInfoHdr;

    bmpInfoHdr.biSize       = sizeof(BITMAPINFOHEADER);
    bmpInfoHdr.biWidth      = buf_des.dwWidth;
    bmpInfoHdr.biHeight       = buf_des.dwHeight;
    bmpInfoHdr.biPlanes       = 1;
    bmpInfoHdr.biBitCount     = 24;
    bmpInfoHdr.biCompression   = BI_RGB;
    bmpInfoHdr.biSizeImage     = bmpInfoHdr.biWidth * bmpInfoHdr.biHeight * bmpInfoHdr.biBitCount / 8;
    bmpInfoHdr.biXPelsPerMeter = 0;
    bmpInfoHdr.biYPelsPerMeter = 0;
    bmpInfoHdr.biClrUsed    = 0;
    bmpInfoHdr.biClrImportant  = 0;

    bmpFile.file_write(&bmpInfoHdr, sizeof(bmpInfoHdr));

    //------------ write the color table -----------//

    if( hasPaletteFlag ) {
	LPDIRECTDRAWPALETTE ddPalettePtr;             // get the direct draw surface's palette
	dd_buf->GetPalette(&ddPalettePtr);

	PALETTEENTRY *palEntries = (PALETTEENTRY*) mem_add( sizeof(PALETTEENTRY)*256 );
	ddPalettePtr->GetEntries(0, 0, 256, palEntries);

	// allocate a color table with 256 entries
	RGBQUAD *colorTable = (RGBQUAD*) mem_add( sizeof(RGBQUAD)*256 );

	for( int i=0 ; i<256 ; i++ ) {
	    colorTable[i].rgbBlue  = palEntries[i].peBlue;
	    colorTable[i].rgbGreen = palEntries[i].peGreen;
	    colorTable[i].rgbRed   = palEntries[i].peRed;
	    colorTable[i].rgbReserved = 0;
	}

	bmpFile.file_write(colorTable, sizeof(RGBQUAD)*256);

	mem_del(palEntries);
	mem_del(colorTable);
    }

    //----------- write the bitmap ----------//

    if( bmpInfoHdr.biBitCount == 8 ) {
	for( int y=buf_height()-1 ; y>=0 ; y-- )      // write in reversed order as DIB's vertical order is reversed
	    bmpFile.file_write(buf_ptr(0,y), buf_width());
    }
    else if( bmpInfoHdr.biBitCount == 24 ) {
	int lineBufferSize = sizeof(RGBColor) * bmpInfoHdr.biWidth;
	RGBColor *lineBuffer = (RGBColor *)mem_add( lineBufferSize );
	for( int y = buf_height()-1; y>=0 ; --y ) {
	    register short *pixelPtr = buf_ptr( 0, y );
	    register RGBColor *lineBufPtr = lineBuffer;
	    for( int x = buf_width()-1; x >= 0; --x, ++pixelPtr, ++lineBufPtr) {
		vga.decode_pixel( *pixelPtr, lineBufPtr );
		if(lineBufPtr->blue>=0xf8)                // to make the printing output clearer
		    lineBufPtr->blue=0xff;                  // chwg0410
		if(lineBufPtr->green>=0xf8)               // to make the printing output clearer
		    lineBufPtr->green=0xff;                 // chwg0410
		if(lineBufPtr->red>=0xf8)                 // to make the printing output clearer
		    lineBufPtr->red=0xff;                   // chwg0410
		// exchange Red and blue
		BYTE r = lineBufPtr->red;
		lineBufPtr->red = lineBufPtr->blue;
		lineBufPtr->blue = r;
	    }
	    bmpFile.file_write(lineBuffer, lineBufferSize );
	}
	mem_del(lineBuffer);
    }
    else {
	err_here();
    }

    //------------ close the file -----------//

    bmpFile.file_close();

    return 1;
}

//------------ End of function VgaBuf::write_bmp_file --------------//

//------------ Begin of function VgaBuf::write_bmp_file_area --------------//
//!
//! Load a BMP file into the current VgaBuf DIB object.
//!
//! <char*> fileName - the name of the BMP file.
//!
//! return : <int> 1-succeeded, 0-failed.
//!
int VgaBuf::write_bmp_file_area(char* fileName, int x1, int y1, int x2, int y2) {
    File       bmpFile;
    BITMAPINFO*  bmpInfoPtr = NULL;
    char*      bitmapPtr = NULL;

    int        hasPaletteFlag = 0;
    int        bytePerPixel = sizeof(RGBColor);

    bmpFile.file_create(fileName, 1, 0);            // 1-handle error, 0-disable variable file size

    //------------ Write the file header ------------//

    BITMAPFILEHEADER bmpFileHdr;

    bmpFileHdr.bfType    = 0x4D42;                  // set the type to "BM"
    // buf_size();
    bmpFileHdr.bfSize    = (x2-x1+1)*(y2-y1+1)*bytePerPixel;
    bmpFileHdr.bfReserved1 = 0;
    bmpFileHdr.bfReserved2 = 0;
    bmpFileHdr.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    if( hasPaletteFlag )
	bmpFileHdr.bfOffBits += sizeof(RGBQUAD)*256;

    bmpFile.file_write(&bmpFileHdr, sizeof(bmpFileHdr));

    //------------ Write in the info header -----------//

    BITMAPINFOHEADER bmpInfoHdr;

    bmpInfoHdr.biSize       = sizeof(BITMAPINFOHEADER);
    bmpInfoHdr.biWidth      = (x2-x1+1);            // buf_des.dwWidth;
    bmpInfoHdr.biHeight       = (y2-y1+1);          // buf_des.dwHeight;
    bmpInfoHdr.biPlanes       = 1;
    bmpInfoHdr.biBitCount     = bytePerPixel*8;
    bmpInfoHdr.biCompression   = BI_RGB;
    bmpInfoHdr.biSizeImage     = bmpInfoHdr.biWidth * bmpInfoHdr.biHeight * bmpInfoHdr.biBitCount / 8;
    bmpInfoHdr.biXPelsPerMeter = 0;
    bmpInfoHdr.biYPelsPerMeter = 0;
    bmpInfoHdr.biClrUsed    = 0;
    bmpInfoHdr.biClrImportant  = 0;

    bmpFile.file_write(&bmpInfoHdr, sizeof(bmpInfoHdr));

    //------------ write the color table -----------//

    if( hasPaletteFlag ) {
	LPDIRECTDRAWPALETTE ddPalettePtr;             // get the direct draw surface's palette
	dd_buf->GetPalette(&ddPalettePtr);

	PALETTEENTRY *palEntries = (PALETTEENTRY*) mem_add( sizeof(PALETTEENTRY)*256 );
	ddPalettePtr->GetEntries(0, 0, 256, palEntries);

	// allocate a color table with 256 entries
	RGBQUAD *colorTable = (RGBQUAD*) mem_add( sizeof(RGBQUAD)*256 );

	for( int i=0 ; i<256 ; i++ ) {
	    colorTable[i].rgbBlue  = palEntries[i].peBlue;
	    colorTable[i].rgbGreen = palEntries[i].peGreen;
	    colorTable[i].rgbRed   = palEntries[i].peRed;
	    colorTable[i].rgbReserved = 0;
	}

	bmpFile.file_write(colorTable, sizeof(RGBQUAD)*256);

	mem_del(palEntries);
	mem_del(colorTable);
    }

    //----------- write the bitmap ----------//

    if( bmpInfoHdr.biBitCount == 8 ) {
	for( int y=buf_height()-1 ; y>=0 ; y-- )      // write in reversed order as DIB's vertical order is reversed
	    bmpFile.file_write(buf_ptr(0,y), buf_width());
    }
    else if( bmpInfoHdr.biBitCount == 24 ) {
	int lineBufferSize = sizeof(RGBColor) * bmpInfoHdr.biWidth;
	RGBColor *lineBuffer = (RGBColor *)mem_add( lineBufferSize );
	for( int y = y2; y>=y1 ; --y ) {
	    short *pixelPtr = buf_ptr( x1, y );
	    RGBColor *lineBufPtr = lineBuffer;
	    for( int x = x1; x <= x2; ++x, ++pixelPtr, ++lineBufPtr) {
		vga.decode_pixel( *pixelPtr, lineBufPtr );
		if(lineBufPtr->blue>=0xf8)                // to make the printing output clearer
		    lineBufPtr->blue=0xff;                  // chwg0410
		if(lineBufPtr->green>=0xf8)               // to make the printing output clearer
		    lineBufPtr->green=0xff;                 // chwg0410
		if(lineBufPtr->red>=0xf8)                 // to make the printing output clearer
		    lineBufPtr->red=0xff;                   // chwg0410
		// exchange Red and blue
		BYTE r = lineBufPtr->red;
		lineBufPtr->red = lineBufPtr->blue;
		lineBufPtr->blue = r;
	    }
	    bmpFile.file_write(lineBuffer, lineBufferSize );
	}
	mem_del(lineBuffer);
    }
    else {
	err_here();
    }

    //------------ close the file -----------//

    bmpFile.file_close();

    return 1;
}

//------------ End of function VgaBuf::write_bmp_file --------------//

//---------- Begin of function VgaBuf::put_large_bitmap ---------//
//!
//! Put a picture on the screen, when a picture's size is > 64K
//! Pass a file pointer to put_large_bitmap() for continously reading the file
//!
//! Syntax
//!
//! <int>   x, y    = the location of the picture on the screen
//! <FILE*> filePtr = pointer to the
//!
//!--------- Format of Picture buffer -------//
//!
//! <int> = picture width
//! <int> = picture height
//!
//! char[...]    = non-compressed flat picture bitmap
//!
//!---------------------------------------------//

void VgaBuf::put_large_bitmap(int x1, int y1, File* filePtr, short *colorRemapTable) {
    if( filePtr == NULL )
	return;

    if( !colorRemapTable )
	colorRemapTable = default_remap_table;

    int pictWidth = filePtr->file_get_short();
    int hasPalette=0;

    //------ read in bitmap and display it --------//

    int pictHeight = filePtr->file_get_short();
    int x2 = x1 + pictWidth  - 1;
    int y2 = y1 + pictHeight - 1;

    long pictSize = (long) pictWidth * pictHeight;

    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    //---- if pict size less than 64K, read in the picture in one step ----//

    if( Bitmap::size(pictWidth, pictHeight) <= COMMON_DATA_BUF_SIZE ) {
	((Bitmap *)sys.common_data_buf)->init(pictWidth, pictHeight);

	filePtr->file_read( ((Bitmap *)sys.common_data_buf)->bitmap, pictSize );

	if( is_front )
	    mouse.hide_area( x1,y1,x2,y2 );             // if the mouse cursor is in that area, hide it

	put_bitmap_remap_fast( x1, y1, (char *)sys.common_data_buf, colorRemapTable );

	if( is_front )
	    mouse.show_area();
    }
    else {                                          //----- if the picture size > 64K, read in line by line -----//
	// max. no. of lines can be in the buffer
	int bufferLine = (COMMON_DATA_BUF_SIZE - 2*sizeof(short) )/ pictWidth;
	int ty=y1+bufferLine-1;

	if( ty> y2 )
	    ty=y2;

	while( y1<=y2 ) {
	    ((Bitmap *)sys.common_data_buf)->init( pictWidth, (ty-y1+1) );
	    filePtr->file_read( ((Bitmap *)sys.common_data_buf)->bitmap, (unsigned)pictWidth * (ty-y1+1) );

	    if( is_front )
		mouse.hide_area( x1,y1,x2,ty );           // if the mouse cursor is in that area, hide it

	    put_bitmap_remap_fast( x1, y1, sys.common_data_buf, colorRemapTable );

	    if( is_front )
		mouse.show_area();

	    y1 += bufferLine;

	    if( (ty+=bufferLine) > y2 )
		ty=y2;
	}
    }
}

//----------- End of function VgaBuf::put_large_bitmap --------//

//---------- Begin of function VgaBuf::put_large_bitmapW ---------//
//!
//! Put a picture on the screen, when a picture's size is > 64K
//! Pass a file pointer to put_large_bitmap() for continously reading the file
//!
//! Syntax
//!
//! <int>   x, y    = the location of the picture on the screen
//! <FILE*> filePtr = pointer to the
//!
//!--------- Format of Picture buffer -------//
//!
//! <int> = picture width
//! <int> = picture height
//!
//! short[...]    = non-compressed flat picture bitmap
//!
//!---------------------------------------------//

void VgaBuf::put_large_bitmapW(int x1, int y1, File* filePtr) {
    if( filePtr == NULL )
	return;

    int pictWidth = filePtr->file_get_short();

    //------ read in bitmap and display it --------//

    int pictHeight = filePtr->file_get_short();
    int x2 = x1 + pictWidth  - 1;
    int y2 = y1 + pictHeight - 1;

    long pictSize = (long) pictWidth * pictHeight * sizeof(short);

    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    //---- if pict size less than 64K, read in the picture in one step ----//

    if( BitmapW::size(pictWidth, pictHeight) <= COMMON_DATA_BUF_SIZE ) {
	((BitmapW *)sys.common_data_buf)->init(pictWidth, pictHeight);

	filePtr->file_read( ((BitmapW *)sys.common_data_buf)->bitmap, pictSize );

	if( is_front )
	    mouse.hide_area( x1,y1,x2,y2 );             // if the mouse cursor is in that area, hide it

	put_bitmapW( x1, y1, (short *)sys.common_data_buf );

	if( is_front )
	    mouse.show_area();
    }
    else {                                          //----- if the picture size > 64K, read in line by line -----//
	// max. no. of lines can be in the buffer
	int bufferLine = (COMMON_DATA_BUF_SIZE - 2*sizeof(short) )/ pictWidth / sizeof(short);
	int ty=y1+bufferLine-1;

	if( ty> y2 )
	    ty=y2;

	while( y1<=y2 ) {
	    ((BitmapW *)sys.common_data_buf)->init( pictWidth, (ty-y1+1) );
	    filePtr->file_read( ((BitmapW *)sys.common_data_buf)->bitmap, (unsigned)pictWidth * (ty-y1+1) * sizeof(short) );

	    if( is_front )
		mouse.hide_area( x1,y1,x2,ty );           // if the mouse cursor is in that area, hide it

	    put_bitmapW( x1, y1, (short *)sys.common_data_buf);

	    if( is_front )
		mouse.show_area();

	    y1 += bufferLine;

	    if( (ty+=bufferLine) > y2 )
		ty=y2;
	}
    }
}

//----------- End of function VgaBuf::put_large_bitmapW --------//

//----------- Begin of function VgaBuf::convert_gray ----------//
//!
//! Convert an specified area of the bitmap from color to gray-scale.
//!
void VgaBuf::convert_gray(int x1, int y1, int x2, int y2) {
    // remap_bar(x1, y1, x2, y2, vga.gray_remap_table);
}

//--------- End of function VgaBuf::convert_gray -----------//
