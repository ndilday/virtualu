//Filename    : OPRINT.H
//Description : Header file of print object
#include <windows.h>

#ifndef __OPRINT_H
#define __OPRINT_H

#define NUMLINES 68

//!Used for printing out game to a peripheral printer. (1) use
//!start_print(); to start printing doc (2) use open_page(); to start a new
//!page (3) set font by using dfont(); and put_? functions to add text or
//!bmp (4) use end_page();to end a page (5) use endprint(); to end printing
//!doc.
class Print {

private:                                        //private var
    HDC memDC,hdc;
    //##	begin	zhoubin	000406
    HDC printer_mem_dc;
    //##	end		zhoubin	000406
    double VidXPPI, VidYPPI, PrXPPI, PrYPPI;
    double Xratio, Yratio;
    HBITMAP hImage;
    PRINTDLG printdlg;
    DOCINFO docinfo;
    TEXTMETRIC tm;
    HFONT hfont;

public:
    Print() {                                     //con
    };
    ~Print() {                                    //dis
    };
    int start_print(HWND,HINSTANCE);
    void open_page();
    /*BMPWIDTH*/
    void put_bitmap_center(char*,int,int /*BMPHEIGHT*/,double,int,int);
    void put_bitmap(char*,int,int,double,double,int,int);
    void put_text(char*,int,int);
    void dfont(int,bool,bool,int);
    void end_page();
    void endprint(HWND);

private:                                        //private function
    void print_init(PRINTDLG*,HWND,HINSTANCE);

};

extern Print print;

//----------------------------------------------//
#endif
