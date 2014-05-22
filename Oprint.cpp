//Filename    : OPRINT.CPP
//Description : A collection of print objects

#include <windows.h>
#include <string.h>
#include <commdlg.h>
#include "oprint.h"
#include <stdio.h>
#include <OVGA.H>
#include <OCOLTBL.H>
#include <ALL.H>

//printint order:
//step1: use start_print(); to start printing doc.
//step2: use open_page(); to start a new page
//step3: set font by using dfont(); and put_? functions to add text or bmp
//setp4: use end_page();to end a page
//step5: use endprint(); to end printing doc

// ###### begin Gilbert 27/04/2001 ######//
Print::Print() {
    memset(this, 0, sizeof(*this) );
}

Print::~Print() {
    deinit();
}

void Print::init() {
    deinit();
}

void Print::deinit() {
    if( hImage )
	DeleteObject(hImage);
    hImage = NULL;

    if( printdlg.hDC )
	DeleteDC(printdlg.hDC);
    printdlg.hDC = NULL;

    if( memDC )
	DeleteDC(memDC);
    memDC = NULL;

    if( hdc && hwnd_of_hdc ) {
	ReleaseDC(hwnd_of_hdc, hdc);
	hdc = NULL;
    }

    if( hfont )
	DeleteObject(hfont);
    hfont = NULL;
}

// ###### end Gilbert 27/04/2001 ######//

//Initialize PRINTDLG structure.
// #### begin Gilbert 27/04/2001 #####//
// rename var printdlg to printDlg to avoid collision with member
void Print::print_init(PRINTDLG *printDlg, HWND hwnd,HINSTANCE hThisins) {
    printDlg->lStructSize = sizeof(PRINTDLG);
    printDlg->hwndOwner = hwnd;
    printDlg->hDevMode = NULL;
    printDlg->hDevNames =NULL;
    printDlg->hDC = NULL;
    printDlg->Flags = PD_RETURNDC|PD_RETURNDEFAULT|PD_PRINTSETUP|PD_HIDEPRINTTOFILE|PD_ENABLESETUPTEMPLATE;
    printDlg->nFromPage = 0;
    printDlg->nToPage = 0;
    printDlg->nMinPage = 0;
    printDlg->nMaxPage = 0;
    printDlg->nCopies = 1;
    printDlg->hInstance =hThisins;
    printDlg->lCustData = 0;
    printDlg->lpfnPrintHook = NULL;
    printDlg->lpfnSetupHook = NULL;
    printDlg->lpPrintTemplateName = "mydlg";
    printDlg->lpSetupTemplateName = "mydlg";
    printDlg->hPrintTemplate = NULL;
    printDlg->hSetupTemplate =NULL;
}

// #### end Gilbert 27/04/2001 #####//

int Print::start_print(HWND ohwnd,HINSTANCE hThis) {
    hdc = GetDC(ohwnd);
    // ##### begin Gilbert 27/04/2001 ######//
    hwnd_of_hdc = ohwnd;
    // ##### end Gilbert 27/04/2001 ######//
    memDC = CreateCompatibleDC(hdc);

    print_init(&printdlg, ohwnd, hThis);
    if(!PrintDlg(&printdlg))
	return 0;

    docinfo.cbSize = sizeof(DOCINFO);
    docinfo.lpszDocName = "Printing bitmaps";
    docinfo.lpszOutput = NULL;
    docinfo.lpszDatatype = NULL;
    docinfo.fwType = 0;

    if(!(GetDeviceCaps(printdlg.hDC, RASTERCAPS)  & (RC_BITBLT | RC_STRETCHBLT|RC_BITMAP64 |
						     RC_DI_BITMAP| RC_DIBTODEV |RC_PALETTE |RC_SCALING |RC_STRETCHBLT|RC_STRETCHDIB))) {
	// MessageBox(ohwnd, "Cannot Print Raster Images", "Error", MB_OK);
	return NULL;
    }

    StartDoc(printdlg.hDC, &docinfo);

    return 1;
}

void Print::open_page() {
    //obtain pixels-per-inch
    VidXPPI = GetDeviceCaps(memDC, LOGPIXELSX);
    VidYPPI = GetDeviceCaps(memDC, LOGPIXELSY);
    PrXPPI = GetDeviceCaps(printdlg.hDC, LOGPIXELSX);
    PrYPPI = GetDeviceCaps(printdlg.hDC, LOGPIXELSY);

    //get scaling ratios
    Xratio = (PrXPPI / VidXPPI)*2/3;
    Yratio = (PrYPPI / VidYPPI)*2/3;

    StartPage(printdlg.hDC);
}

void Print::put_bitmap_center(char* name,int BMPWIDTH,int BMPHEIGHT,double pageprotion,int xcor,int ycor) {
    //Load Bitmap into hImage
    hImage = (HBITMAP)LoadImage(0, name, IMAGE_BITMAP, 0, 0,
				LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTCOLOR|LR_LOADMAP3DCOLORS );

    // put bitmap image into memory DC

    SelectObject(memDC, hImage); {
	// in some machine, StretchBlt cut area from bottom to top, may related to SetMapMode

	static int writeMapMode = 0;
	FILE *mapModeLog = NULL;
	int mapMode;
	SIZE windowExt, viewportExt;
	if( !writeMapMode )
	    mapModeLog = fopen("mapmode.log", "w");

	if( mapModeLog )
	    fprintf( mapModeLog, "memDC map mode : %x\n", (mapMode = GetMapMode(memDC)) );
	if( mapMode == MM_ANISOTROPIC || mapMode == MM_ISOTROPIC ) {
	    GetWindowExtEx(memDC, &windowExt);
	    GetViewportExtEx(memDC, &viewportExt);
	    if( mapModeLog ) {
		fprintf( mapModeLog, "memDC WindowExt : %d, %d\n", windowExt.cx, windowExt.cy );
		fprintf( mapModeLog, "memDC ViewportExt : %d, %d\n", viewportExt.cx, viewportExt.cy );
	    }
	}
	if( mapModeLog )
	    fprintf( mapModeLog, "memDC LOGPIXELSX, LOGPIXELSY: %d, %d\n", GetDeviceCaps(memDC,LOGPIXELSX), GetDeviceCaps(memDC,LOGPIXELSY) );

	if( mapModeLog )
	    fprintf( mapModeLog, "printdlg.hDC map mode : %x\n", (mapMode = GetMapMode(printdlg.hDC)) );
	if( mapMode == MM_ANISOTROPIC || mapMode == MM_ISOTROPIC ) {
	    GetWindowExtEx(printdlg.hDC, &windowExt);
	    GetViewportExtEx(printdlg.hDC, &viewportExt);
	    if( mapModeLog ) {
		fprintf( mapModeLog, "printdlg.hDC WindowExt : %d, %d\n", windowExt.cx, windowExt.cy );
		fprintf( mapModeLog, "printdlg.hDC ViewportExt : %d, %d\n", viewportExt.cx, viewportExt.cy );
	    }
	}
	if( mapModeLog )
	    fprintf( mapModeLog, "printdlg.hDC LOGPIXELSX, LOGPIXELSY: %d, %d\n", GetDeviceCaps(printdlg.hDC, LOGPIXELSX), GetDeviceCaps(printdlg.hDC, LOGPIXELSY) );

	if( mapModeLog )
	    fclose(mapModeLog);
	writeMapMode = 1;
    }

    // copy bitmap while maintaining perspective
    int xpagepos,ypagepos,pagew,pageh;

    pagew = GetDeviceCaps(printdlg.hDC,PHYSICALWIDTH);
    xpagepos = (int)((pagew-(BMPWIDTH*Xratio))/2);  /*1.3*/
    pageh = GetDeviceCaps(printdlg.hDC,PHYSICALHEIGHT);
    ypagepos = (int)(pageh*pageprotion);
    StretchBlt(printdlg.hDC,xpagepos,ypagepos,
	       (int) (BMPWIDTH*Xratio),                      //*1.3),
	       (int) (BMPHEIGHT*Yratio),                     //*1.2),
	       memDC, xcor, ycor,
	       BMPWIDTH, BMPHEIGHT,
	       SRCCOPY);

    // ##### begin Gilbert 24/04/2001
    // free hImage
    DeleteObject(hImage);
    hImage = NULL;
    // ##### end Gilbert 24/04/2001
}

void Print::put_bitmap(char* name,int BMPWIDTH,int BMPHEIGHT,double xposition,double yposition,int xcor,int ycor) {
    //Load Bitmap into hImage
    //hImage = (HBITMAP)LoadImage(0, name, IMAGE_BITMAP, 0, 0,
    //	LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTCOLOR|LR_LOADMAP3DCOLORS );

    // put bitmap image into memory DC
    //SelectObject(memDC, hImage);

    // copy bitmap while maintaining perspective

    int xpagepos,ypagepos,pagew,pageh;

    // pagew= GetDeviceCaps(printdlg.hDC,ASPECTX);
    pagew= GetDeviceCaps(printdlg.hDC,PHYSICALWIDTH);
    xpagepos=(int)(xposition*pagew);
    // pageh= GetDeviceCaps(printdlg.hDC,ASPECTY);
    pageh= GetDeviceCaps(printdlg.hDC,PHYSICALHEIGHT);
    ypagepos=(int)(yposition*pageh);

    StretchBlt(printdlg.hDC,xpagepos,ypagepos,
	       (int) (BMPWIDTH*Xratio),                      //*1.3),
	       (int) (BMPHEIGHT*Yratio),                     //*1.2),
	       memDC, xcor, ycor,
	       BMPWIDTH, BMPHEIGHT,
	       SRCCOPY);

    // ##### begin Gilbert 24/04/2001 ######//
    // free hImage
    //DeleteObject(hImage);
    //hImage = NULL;
    // ##### end Gilbert 24/04/2001 ######//
}

/*
void Print::put_bitmap_bot(char* name,int BMPWIDTH,int BMPHEIGHT,double xprotion,double yprotion,int xcor,int ycor)
{
  //Load Bitmap into hImage
  hImage = LoadImage(0, name, IMAGE_BITMAP, 0, 0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
  // put bitmap image into memory DC
  SelectObject(memDC, hImage);

  // copy bitmap while maintaining perspective
  int xpagepos,ypagepos,pagew,pageh;

pagew = GetDeviceCaps(printdlg.hDC,PHYSICALWIDTH);
xpagepos = (int)(pagew-(pagew/xprotion));

pageh = GetDeviceCaps(printdlg.hDC,PHYSICALHEIGHT);
ypagepos = (int)(pageh-(pageh/yprotion));

StretchBlt(printdlg.hDC,xpagepos,ypagepos,
(int) (BMPWIDTH*Xratio*1.3),
(int) (BMPHEIGHT*Yratio*1.2),
memDC, xcor, ycor,
BMPWIDTH, BMPHEIGHT,
SRCCOPY);

// ###### begin Gilbert 27/04/2001 ######//
DeleteObject(hImage);
hImage = NULL;
// ###### end Gilbert 27/04/2001 ######//
}
*/

void Print::put_vga_back_center(char* name,int BMPWIDTH,int BMPHEIGHT,double pageprotion,int xcor,int ycor) {
    VgaBuf *vgaBuf = &vga_back;

#if(0)

    vgaBuf->write_bmp_file( name );                 // capture whole screen, but some computers have bug, capture from bottom to top in stretchBit
    put_bitmap_center(name, BMPWIDTH, BMPHEIGHT, pageprotion, xcor, ycor);

#elif(0)                                        // use capture screen, but need write to harddisk

    vgaBuf->write_bmp_file_area( name, xcor, ycor, xcor+BMPWIDTH-1, ycor+BMPHEIGHT-1 );
    // (0,0), not (xcor,ycor)
    put_bitmap_center(name, BMPWIDTH, BMPHEIGHT, pageprotion, 0, 0);

#elif(1)

    const int bytePerPixel = 3;
    // 3 byte per pixel, round-up to next DWORD boundary
    long bitmapPitch = (BMPWIDTH * bytePerPixel + 3) & ~3;
    BYTE *bitmapDataBuf = (BYTE *)mem_add( bitmapPitch * BMPHEIGHT );

    // BITMAPINFO *bitmapInfo = (BITMAPINFO *)mem_add( sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) ); // paletted bitmap
    BITMAPINFO *bitmapInfo = (BITMAPINFO *)mem_add( sizeof(BITMAPINFOHEADER) );

    memset( &bitmapInfo->bmiHeader, 0, sizeof(BITMAPINFOHEADER) );
    bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo->bmiHeader.biWidth = BMPWIDTH;
    bitmapInfo->bmiHeader.biHeight = BMPHEIGHT;
    bitmapInfo->bmiHeader.biPlanes = 1;
    bitmapInfo->bmiHeader.biBitCount = bytePerPixel * 8;
    bitmapInfo->bmiHeader.biCompression = BI_RGB;
    bitmapInfo->bmiHeader.biSizeImage = bitmapPitch * BMPHEIGHT;
    bitmapInfo->bmiHeader.biXPelsPerMeter = 0;
    bitmapInfo->bmiHeader.biYPelsPerMeter = 0;
    bitmapInfo->bmiHeader.biClrUsed = 0;
    bitmapInfo->bmiHeader.biClrImportant = 0;

    // set from bottom to top

    BYTE *rowPtr = (BYTE *)bitmapDataBuf;
    for( int cy = ycor+BMPHEIGHT-1; cy >= ycor; --cy, rowPtr+=bitmapPitch ) {
	short *readPtr = vgaBuf->buf_ptr( xcor, cy);
	BYTE *writePtr = rowPtr;
	for( int cx = xcor; cx < xcor+BMPWIDTH; ++cx, ++readPtr, writePtr+=bytePerPixel ) {
	    RGBColor rgbColor;
	    // only for bytePerPixel is 3
	    vga.decode_pixel( *readPtr, &rgbColor );
	    // to make the printing output clearer
	    writePtr[0] = rgbColor.blue>=0xf8 ? 0xff : rgbColor.blue;
	    writePtr[1] = rgbColor.green>=0xf8 ? 0xff : rgbColor.green;
	    writePtr[2] = rgbColor.red>=0xf8 ? 0xff : rgbColor.red;
	}
	// pad zero until bitmapPitch
	memset( writePtr, 0, rowPtr+bitmapPitch-writePtr);
    }

    // copy bitmap while maintaining perspective
    int xpagepos,ypagepos,pagew,pageh;

    pagew = GetDeviceCaps(printdlg.hDC,PHYSICALWIDTH);
    xpagepos = (int)((pagew-(BMPWIDTH*Xratio))/2);  /*1.3*/
    pageh = GetDeviceCaps(printdlg.hDC,PHYSICALHEIGHT);
    ypagepos = (int)(pageh*pageprotion);

    StretchDIBits(printdlg.hDC,xpagepos,ypagepos,
		  (int) (BMPWIDTH*Xratio),                      //*1.3),
		  (int) (BMPHEIGHT*Yratio),                     //*1.2),
		  0, 0,                                         // not xcor, ycor, as the (0,0)bitmapDataPtr is (xcor, ycor) of vgaBuf
		  BMPWIDTH, BMPHEIGHT,
		  bitmapDataBuf, bitmapInfo, DIB_RGB_COLORS,
		  SRCCOPY);

    // free memory
    mem_del( bitmapDataBuf );
    mem_del( bitmapInfo );
#else
#error
#endif
}

void Print::put_vga_back(char* name,int BMPWIDTH,int BMPHEIGHT,double xposition,double yposition,int xcor,int ycor) {
    VgaBuf *vgaBuf = &vga_back;

#if(0)

    vgaBuf->write_bmp_file( name );
    put_bitmap(name, BMPWIDTH, BMPHEIGHT, xposition, yposition, xcor, ycor);

#elif(0)                                        // use capture screen, but need write to harddisk

    vgaBuf->write_bmp_file_area( name, xcor, ycor, xcor+BMPWIDTH-1, ycor+BMPHEIGHT-1 );
    // (0,0) not (xcor,ycor)
    put_bitmap(name, BMPWIDTH, BMPHEIGHT, xposition, yposition, 0, 0);

#elif(1)

    const int bytePerPixel = 3;
    // 3 byte per pixel, round-up to next DWORD boundary
    long bitmapPitch = (BMPWIDTH * bytePerPixel + 3) & ~3;
    BYTE *bitmapDataBuf = (BYTE *)mem_add( bitmapPitch * BMPHEIGHT );

    // BITMAPINFO *bitmapInfo = (BITMAPINFO *)mem_add( sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) ); // paletted bitmap
    BITMAPINFO *bitmapInfo = (BITMAPINFO *)mem_add( sizeof(BITMAPINFOHEADER) );

    memset( &bitmapInfo->bmiHeader, 0, sizeof(BITMAPINFOHEADER) );
    bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo->bmiHeader.biWidth = BMPWIDTH;
    bitmapInfo->bmiHeader.biHeight = BMPHEIGHT;
    bitmapInfo->bmiHeader.biPlanes = 1;
    bitmapInfo->bmiHeader.biBitCount = bytePerPixel * 8;
    bitmapInfo->bmiHeader.biCompression = BI_RGB;
    bitmapInfo->bmiHeader.biSizeImage = bitmapPitch * BMPHEIGHT;
    bitmapInfo->bmiHeader.biXPelsPerMeter = 0;
    bitmapInfo->bmiHeader.biYPelsPerMeter = 0;
    bitmapInfo->bmiHeader.biClrUsed = 0;
    bitmapInfo->bmiHeader.biClrImportant = 0;

    // set from bottom to top

    BYTE *rowPtr = (BYTE *)bitmapDataBuf;
    for( int cy = ycor+BMPHEIGHT-1; cy >= ycor; --cy, rowPtr+=bitmapPitch ) {
	short *readPtr = vgaBuf->buf_ptr( xcor, cy);
	BYTE *writePtr = rowPtr;
	for( int cx = xcor; cx < xcor+BMPWIDTH; ++cx, ++readPtr, writePtr+=bytePerPixel ) {
	    RGBColor rgbColor;
	    // only for bytePerPixel is 3
	    vga.decode_pixel( *readPtr, &rgbColor );
	    // to make the printing output clearer
	    writePtr[0] = rgbColor.blue>=0xf8 ? 0xff : rgbColor.blue;
	    writePtr[1] = rgbColor.green>=0xf8 ? 0xff : rgbColor.green;
	    writePtr[2] = rgbColor.red>=0xf8 ? 0xff : rgbColor.red;
	}
	// pad zero until bitmapPitch
	memset( writePtr, 0, rowPtr+bitmapPitch-writePtr);
    }

    // copy bitmap while maintaining perspective

    int xpagepos,ypagepos,pagew,pageh;

    // pagew= GetDeviceCaps(printdlg.hDC,ASPECTX);
    pagew= GetDeviceCaps(printdlg.hDC,PHYSICALWIDTH);
    xpagepos=(int)(xposition*pagew);
    // pageh= GetDeviceCaps(printdlg.hDC,ASPECTY);
    pageh= GetDeviceCaps(printdlg.hDC,PHYSICALHEIGHT);
    ypagepos=(int)(yposition*pageh);

    StretchDIBits(printdlg.hDC,xpagepos,ypagepos,
		  (int) (BMPWIDTH*Xratio),                      //*1.3),
		  (int) (BMPHEIGHT*Yratio),                     //*1.2),
		  0, 0,                                         // not xcor, ycor, as the (0,0)bitmapDataPtr is (xcor, ycor) of vgaBuf
		  BMPWIDTH, BMPHEIGHT,
		  bitmapDataBuf, bitmapInfo, DIB_RGB_COLORS,
		  SRCCOPY);

    // free memory
    mem_del( bitmapDataBuf );
    mem_del( bitmapInfo );
#else
#error
#endif
}

void Print::put_text(char* string,int xpos,int ypos) {
    SelectObject(printdlg.hDC,hfont);
    GetTextMetrics(printdlg.hDC, &tm);
    SetMapMode(printdlg.hDC,MM_TEXT);
    xpos = xpos*tm.tmAveCharWidth;
    ypos = ypos*(tm.tmHeight + tm.tmExternalLeading);

    // output to printer
    TextOut(printdlg.hDC,xpos, ypos, string, strlen(string));
}

//create font

                                                  /*bold=700*/
void Print::dfont(int PointSize,bool italic,bool under,int weight) {
    // ##### begin Gilbert 27/04/2001 ######//
    // free previous font
    if( hfont ) {
	DeleteObject(hfont);
	hfont = NULL;
    }
    // ##### emd Gilbert 27/04/2001 ######//
    hfont = CreateFont(
	(-MulDiv(PointSize, GetDeviceCaps(printdlg.hDC, LOGPIXELSY), 72)),
	0,                                            // logical average character width
	0,                                            // angle of escapement
	0,                                            // base-line orientation angle
	weight,                                       // font weight
	italic,                                       // italic attribute flag
	under,                                        // underline attribute flag
	false,                                        // strikeout attribute flag
	DEFAULT_CHARSET,                              // character set identifier
	OUT_CHARACTER_PRECIS,                         // output precision
	CLIP_DEFAULT_PRECIS,                          // clipping precision
	DEFAULT_QUALITY,                              // output quality
	FF_ROMAN,                                     //DEFAULT_PITCH, // pitch and family
	NULL                                          // pointer to typeface name string
	);
};

void Print::end_page() {
    EndPage(printdlg.hDC);
}

void Print::endprint(HWND ohwnd) {                //put the hwnd here
    EndDoc(printdlg.hDC);
    // ##### begin Gilbert 27/04/2001 #######//
    DeleteDC(memDC);
    memDC = NULL;
    DeleteDC(printdlg.hDC);
    printdlg.hDC = NULL;
    // err_when( hwnd_of_hdc != ohwnd );
    ReleaseDC(ohwnd, hdc);
    hdc = NULL;
    // ##### end Gilbert 27/04/2001 #######//
}
