//Filename    : OPRINT.CPP
//Description : A collection of print objects

#include <windows.h>
#include <string.h>
#include <commdlg.h>
#include <osys.h>
#include "oprint.h"

//printint order:
//step1: use start_print(); to start printing doc.
//step2: use open_page(); to start a new page
//step3: set font by using dfont(); and put_? functions to add text or bmp
//setp4: use end_page();to end a page
//step5: use endprint(); to end printing doc

//Initialize PRINTDLG structure.
void Print::print_init(PRINTDLG *printdlg, HWND hwnd,HINSTANCE hThisins) {
    printdlg->lStructSize = sizeof(PRINTDLG);
    printdlg->hwndOwner = hwnd;
    printdlg->hDevMode = NULL;
    printdlg->hDevNames =NULL;
    printdlg->hDC = NULL;
    printdlg->Flags = PD_RETURNDC|PD_RETURNDEFAULT|PD_PRINTSETUP|PD_HIDEPRINTTOFILE|PD_ENABLESETUPTEMPLATE;
    printdlg->nFromPage = 0;
    printdlg->nToPage = 0;
    printdlg->nMinPage = 0;
    printdlg->nMaxPage = 0;
    printdlg->nCopies = 1;
    printdlg->hInstance =hThisins;
    printdlg->lCustData = 0;
    printdlg->lpfnPrintHook = NULL;
    printdlg->lpfnSetupHook = NULL;
    printdlg->lpPrintTemplateName = "mydlg";
    printdlg->lpSetupTemplateName = "mydlg";
    printdlg->hPrintTemplate = NULL;
    printdlg->hSetupTemplate =NULL;
}

int Print::start_print(HWND ohwnd,HINSTANCE hThis) {

    hdc = GetDC(ohwnd);
    memDC = CreateCompatibleDC(hdc);

    print_init(&printdlg, ohwnd, hThis);
    if(!PrintDlg(&printdlg)) return 0;

    docinfo.cbSize = sizeof(DOCINFO);
    docinfo.lpszDocName = "Printing bitmaps";
    docinfo.lpszOutput = NULL;
    docinfo.lpszDatatype = NULL;
    docinfo.fwType = 0;

    if(!(GetDeviceCaps(printdlg.hDC, RASTERCAPS)
	 & (RC_BITBLT | RC_STRETCHBLT|RC_BITMAP64 |
	    RC_DI_BITMAP| RC_DIBTODEV |RC_PALETTE |RC_SCALING |RC_STRETCHBLT|RC_STRETCHDIB))) {
	/*
	  MessageBox(ohwnd, "Cannot Print Raster Images",
	  "Error", MB_OK);
	*/
	return NULL;
    }

    StartDoc(printdlg.hDC, &docinfo);
    //##	begin	zhoubin	000406
    printer_mem_dc = CreateCompatibleDC(printdlg.hDC);
    //##	end		zhoubin	000406

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
				LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTCOLOR|LR_LOADMAP3DCOLORS
	);
    // put bitmap image into memory DC
    SelectObject(memDC, hImage);

    // copy bitmap while maintaining perspective
    int xpagepos,ypagepos,pagew,pageh;

    pagew= GetDeviceCaps(printdlg.hDC,PHYSICALWIDTH);
    xpagepos=(int)((pagew-(BMPWIDTH*Xratio))/2);    /*1.3*/
    pageh= GetDeviceCaps(printdlg.hDC,PHYSICALHEIGHT);
    ypagepos=(int)(pageh*pageprotion);

    //##	begin	zhoubin	000406
    /*	  StretchBlt(printdlg.hDC,xpagepos,ypagepos,
	  (int) (BMPWIDTH*Xratio),//*1.3),
				     (int) (BMPHEIGHT*Yratio),//*1.2),
								 memDC, xcor, ycor,
								 BMPWIDTH, BMPHEIGHT,
								 SRCCOPY);
    */
    int printer_width=(int)(BMPWIDTH*Xratio);
    int printer_height=(int)(BMPHEIGHT*Yratio);

    HBITMAP hBitmap = CreateCompatibleBitmap(printer_mem_dc,printer_width,
					     printer_height);

    SelectObject(printer_mem_dc,hBitmap);
    StretchBlt(printer_mem_dc,0,0,printer_width,printer_height,
	       memDC,xcor,ycor,BMPWIDTH,BMPHEIGHT,SRCCOPY);

    BitBlt(printdlg.hDC,xpagepos,ypagepos,
	   (int) printer_width,                          //*1.3),
	   (int) printer_height,                         //*1.2),
	   printer_mem_dc,0, 0,
	   SRCCOPY);
    DeleteObject(hBitmap);
    //##	end		zhoubin	000406

}

void Print::put_bitmap(char* name,int BMPWIDTH,int BMPHEIGHT,double xposition,double yposition,int xcor,int ycor) {
    //Load Bitmap into hImage
    hImage = (HBITMAP)LoadImage(0, name, IMAGE_BITMAP, 0, 0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
    // put bitmap image into memory DC
    SelectObject(memDC, hImage);

  // copy bitmap while maintaining perspective

    int xpagepos,ypagepos,pagew,pageh;

    //	  pagew= GetDeviceCaps(printdlg.hDC,ASPECTX);
    pagew= GetDeviceCaps(printdlg.hDC,PHYSICALWIDTH);
    xpagepos=(int)(xposition*pagew);
    //   pageh= GetDeviceCaps(printdlg.hDC,ASPECTY);
    pageh= GetDeviceCaps(printdlg.hDC,PHYSICALHEIGHT);
    ypagepos=(int)(yposition*pageh);
    //##	begin	zhoubin	000406
    /*	  StretchBlt(printdlg.hDC,xpagepos,ypagepos,
	  (int) (BMPWIDTH*Xratio),//*1.3),
				     (int) (BMPHEIGHT*Yratio),//*1.2),
								 memDC, xcor, ycor,
								 BMPWIDTH, BMPHEIGHT,
								 SRCCOPY);
							  */
	       int printer_width=(int)(BMPWIDTH*Xratio);
	       int printer_height=(int)(BMPHEIGHT*Yratio);

	       HBITMAP hBitmap = CreateCompatibleBitmap(printer_mem_dc,printer_width,
							printer_height);

	       SelectObject(printer_mem_dc,hBitmap);
	       StretchBlt(printer_mem_dc,0,0,printer_width,printer_height,
			  memDC,xcor,ycor,BMPWIDTH,BMPHEIGHT,SRCCOPY);

	       BitBlt(printdlg.hDC,xpagepos,ypagepos,
		      (int) printer_width,                          //*1.3),
		      (int) printer_height,                         //*1.2),
		      printer_mem_dc,0, 0,
		      SRCCOPY);
	       DeleteObject(hBitmap);
	       //##	end		zhoubin	000406

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

pagew= GetDeviceCaps(printdlg.hDC,PHYSICALWIDTH);
xpagepos=(int)(pagew-(pagew/xprotion));

pageh= GetDeviceCaps(printdlg.hDC,PHYSICALHEIGHT);
ypagepos=(int)(pageh-(pageh/yprotion));

StretchBlt(printdlg.hDC,xpagepos,ypagepos,
(int) (BMPWIDTH*Xratio*1.3),
(int) (BMPHEIGHT*Yratio*1.2),
memDC, xcor, ycor,
BMPWIDTH, BMPHEIGHT,
SRCCOPY);
}
*/

void Print::put_text(char* string,int xpos,int ypos) {

    SelectObject(printdlg.hDC,hfont);
    GetTextMetrics(printdlg.hDC, &tm);
    SetMapMode(printdlg.hDC,MM_TEXT);
    xpos=xpos*tm.tmAveCharWidth;
    ypos=ypos*(tm.tmHeight + tm.tmExternalLeading);

    // output to printer
    TextOut(printdlg.hDC,xpos, ypos, string, strlen(string));

}

//create font

                                                  /*bold=700*/
void Print::dfont(int PointSize,bool italic,bool under,int weight) {

    hfont=CreateFont(
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
    DeleteDC(memDC);
    DeleteDC(printer_mem_dc);
    DeleteDC(printdlg.hDC);
    ReleaseDC(ohwnd, hdc);
}
