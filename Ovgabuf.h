//Filename    : VGABUF.H
//Description : Header file for class VgaBuffer, Direct Draw Buffer.

#ifndef __VGABUF_H
#define __VGABUF_H

#include <IMGFUN.H>
#include <DDRAW.H>

//-------- Define class VgaBuf ----------------//

class File;
class Bitmap;
class BitmapW;

//!VGA direct draw buffer.
class VgaBuf {
public:
    LPDIRECTDRAWSURFACE4  dd_buf;
    DDSURFACEDESC2      buf_des;
    BOOL            buf_locked;                   // whether the and back buffers have been locked or not.
    short*         cur_buf_ptr;
    char            is_front;                     // whether it's the front buffer or not

    // ------- temp_unlock --------- //
    // char						save_locked_flag;
    UCHAR           lock_stack_count;
    WORD            lock_bit_stack;

    //--------- back buffer ----------//

    short* buf_ptr()          { return cur_buf_ptr; }
    short* buf_ptr(int x, int y)  { return (short *)((char *)cur_buf_ptr + buf_des.lPitch*y) + x; }

    // pitch in pixel
    int   buf_pitch() {                           // in no. of pixel
	return buf_des.lPitch >> 1;
    }

    // pitch in byte
    int   buf_true_pitch()      { return buf_des.lPitch; }

    int buf_size()          { return buf_des.dwWidth * buf_des.dwHeight * sizeof(short); }
    int   buf_width()         { return buf_des.dwWidth; }
    int   buf_height()            { return buf_des.dwHeight; }

    //---- GUI colors -----//

    static short color_scheme;                    // color scheme, set by set_color()

    static char color_light;                      // color of the light panel side
    static char color_dark;                       // color of the dark panel side
    static char color_up;                         // color of up_panel surface
    static char color_down;                       // color of down_panel surface
    static char color_push;                       // color of pushed button surface
    static char color_border;                     // color of color

    static short *default_remap_table;            // new for 16-bit

public:
    VgaBuf();
    ~VgaBuf();

    //---------- system functions ----------//

    void    init_front(LPDIRECTDRAW4 ddPtr);
    void    init_back(LPDIRECTDRAW4 ddPtr, DWORD =0, DWORD =0);
    void    deinit();

    void    set_color(int colorScheme);

    BOOL    is_buf_lost();
    BOOL    restore_buf();

    void    activate_pal(LPDIRECTDRAWPALETTE ddPalPtr);
    DWORD   color_match(COLORREF rgb);

    void    lock_buf();
    void    unlock_buf();

    void    temp_unlock();
    void    temp_restore_lock();
    void    temp_lock();
    void    temp_restore_unlock();

    void    set_buf_ptr(short* bufPtr)  { cur_buf_ptr = bufPtr; }
    void    set_default_buf_ptr()   { cur_buf_ptr = (short*)buf_des.lpSurface; }

    int     write_bmp_file(char* fileName);
    int     write_bmp_file_area(char* fileName, int x1, int y1, int x2, int y2);

    //---------- painting functions ----------//

    static int  translate_color(unsigned char c)  // calc 8-bit color to 16-bit color // new for 16 bit
	{ return default_remap_table[c]; }

    void    bar(int x1,int y1,int x2,int y2,int colorCode);
    void    bar_fast(int x1,int y1,int x2,int y2,int colorCode)
	{   IMGbar(buf_ptr(), buf_true_pitch(), x1, y1, x2, y2, translate_color(colorCode)); }
    void    barW_fast(int x1,int y1,int x2,int y2,int colorCode)
	{ IMGbar(buf_ptr(), buf_true_pitch(), x1, y1, x2, y2, colorCode); }

    void    bar_up(int x1,int y1,int x2,int y2)   { bar( x1, y1, x2, y2, color_up); }
    void    bar_down(int x1,int y1,int x2,int y2)   { bar( x1, y1, x2, y2, color_down); }

    void    draw_pixel(int x1,int y1,int colorCode)
	{ *(buf_ptr()+buf_pitch()*y1+x1) = translate_color(colorCode); }

    void    separator(int,int,int,int);
    void    line(int x1,int y1,int x2,int y2,int colorCode);
    void    thick_line(int x1,int y1,int x2,int y2,int colorCode);

    void    indicator(int,int,int,int,float,float,int,int= -1);
    void    indicator(int barType, int x,int y,float,float,int colorScheme);
    void    v_indicator(int,int,int,int,float,float,int,int= -1);

    void    rect(int,int,int,int,int,int);
    void    d3_rect(int,int,int,int);

    void    d3_panel_up(int,int,int,int,int=2,int=1, char* =0);
    void    d3_panel_down(int,int,int,int,int=2,int=1, char* =0);
    void    d3_panel_up_clear(int,int,int,int,int=2, char* =0);
    void    d3_panel_down_clear(int,int,int,int,int=2, char* =0);

    void    tile(int,int,int,int,char*);
    void    adjust_brightness(int x1,int y1,int x2,int y2,int adjustDegree);
    void    draw_d3_up_border(int x1,int y1,int x2,int y2);
    void    draw_d3_down_border(int x1,int y1,int x2,int y2);

    void    convert_gray(int x1, int y1, int x2, int y2);

    //-------- buffer saving functions --------//

    short*  save_area(int,int,int,int,short* =0L);
    // void  	rest_area(char*,int=1 );
    void    rest_area(short*,int=1,int=0 );
    void    rest_area_with_clipping(short*,int,int,int,int,int=1,int=0 );

    void    save_area_common_buf(int,int,int,int);
    void    rest_area_common_buf();

    //------- large bitmap displaying functions -------//

    void    put_large_bitmap(int x1, int y1, File* filePtr, short *colorRemapTable=0);
    void    put_large_bitmapW(int x1, int y1, File* filePtr);

    //---------- assembly bitmap manipulation functions ----------//

    // 8->16 blt

    void    put_bitmap(int x,int y,char* bitmapBuf);

    void    put_bitmap_hmirror( int x, int y, char *bitmapBuf )
	{ IMGbltRemapHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, default_remap_table ); }

    void    put_bitmap_area(int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltAreaRemap( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

    void    put_bitmap_area_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2)
	{ IMGbltAreaRemapHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

    void    put_bitmap_remap(int desX, int desY, char* bitmapBuf, short *colorRemapTable);
    void    put_bitmap_remap_fast( int x, int y, char *bitmapBuf, short *colorRemapTable )
	{ IMGbltRemap( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, colorRemapTable ); }

    void    put_bitmap_remap_hmirror( int x, int y, char *bitmapBuf, short *colorRemapTable )
	{ IMGbltRemapHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, colorRemapTable ); }

    void    put_bitmap_area_remap(int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable )
	{ IMGbltAreaRemap( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

    void    put_bitmap_area_remap_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
	{ IMGbltAreaRemapHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

    // functions with transparent color keying

    void    put_bitmap_trans(int x,int y,char* bitmapBuf);

    void    put_bitmap_trans_fast(int x,int y,char* bitmapBuf)
	{ IMGbltTransRemap( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, default_remap_table ); }

    void    put_bitmap_trans_hmirror( int x, int y, char *bitmapBuf )
	{ IMGbltTransRemapHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, default_remap_table ); }

    void    put_bitmap_area_trans( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltAreaTransRemap( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

    void    put_bitmap_area_trans_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltAreaTransRemapHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

    void    put_bitmap_trans_remap( int x, int y, char *bitmapBuf, short *colorRemapTable )
	{ IMGbltTransRemap( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, colorRemapTable ); }

    void    put_bitmap_trans_remap_hmirror( int x, int y, char *bitmapBuf, short *colorRemapTable )
	{ IMGbltTransRemapHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, colorRemapTable ); }

    void    put_bitmap_area_trans_remap( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable )
	{ IMGbltAreaTransRemap( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

    void    put_bitmap_area_trans_remap_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
	{ IMGbltAreaTransRemapHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

    // functions with run-length transparent key decompression

    void    put_bitmap_trans_decompress( int x, int y, char *bitmapBuf )
	{ IMGbltTransRemapDecompress( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, default_remap_table ); }

    void    put_bitmap_trans_decompress_hmirror( int x, int y, char *bitmapBuf )
	{ IMGbltTransRemapDecompressHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, default_remap_table ); }

    void    put_bitmap_area_trans_decompress( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltAreaTransRemapDecompress( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

    void    put_bitmap_area_trans_decompress_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltAreaTransRemapDecompressHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

    void    put_bitmap_trans_remap_decompress( int x, int y, char *bitmapBuf, short *colorRemapTable)
	{ IMGbltTransRemapDecompress( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, colorRemapTable ); }

    void    put_bitmap_trans_remap_decompress_hmirror( int x, int y, char *bitmapBuf, short *colorRemapTable)
	{ IMGbltTransRemapDecompressHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, colorRemapTable ); }

    void    put_bitmap_area_trans_remap_decompress( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
	{ IMGbltAreaTransRemapDecompress( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

    void    put_bitmap_area_trans_remap_decompress_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
	{ IMGbltAreaTransRemapDecompressHMirror( buf_ptr(), buf_true_pitch(), x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

    // 16->16 blt

    void    read_bitmapW(int x1,int y1,int x2,int y2, short* bitmapWBuf)
	{ IMGreadW(buf_ptr(), buf_true_pitch(), x1, y1, x2, y2, bitmapWBuf); }
    void    read_bitmapW(int x1,int y1,int x2,int y2, BitmapW* bitmapWBuf)
	{ IMGreadW(buf_ptr(), buf_true_pitch(), x1, y1, x2, y2, (short *)bitmapWBuf); }

    void    put_bitmapW(int x, int y, short *bitmapWBuf )
	{ IMGbltW( buf_ptr(), buf_true_pitch(), x, y, bitmapWBuf ); }
    void    put_bitmapW(int x, int y, BitmapW *bitmapWBuf )
	{ IMGbltW( buf_ptr(), buf_true_pitch(), x, y, (short *)bitmapWBuf ); }

    void    put_bitmapW_area( int x, int y, short *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltWArea( buf_ptr(), buf_true_pitch(), x, y, bitmapWBuf, srcX1, srcY1, srcX2, srcY2 ); }
    void    put_bitmapW_area( int x, int y, BitmapW *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltWArea( buf_ptr(), buf_true_pitch(), x, y, (short *)bitmapWBuf, srcX1, srcY1, srcX2, srcY2 ); }

    // functions with transparent color keying

    void    put_bitmapW_trans( int x, int y, short *bitmapWBuf )
	{ IMGbltWTrans( buf_ptr(), buf_true_pitch(), x, y, bitmapWBuf ); }
    void    put_bitmapW_trans( int x, int y, BitmapW *bitmapWBuf )
	{ IMGbltWTrans( buf_ptr(), buf_true_pitch(), x, y, (short *)bitmapWBuf ); }

    void    put_bitmapW_area_trans( int x, int y, short *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltWAreaTrans( buf_ptr(), buf_true_pitch(), x, y, bitmapWBuf, srcX1, srcY1, srcX2, srcY2 ); }
    void    put_bitmapW_area_trans( int x, int y, BitmapW *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltWAreaTrans( buf_ptr(), buf_true_pitch(), x, y, (short *)bitmapWBuf, srcX1, srcY1, srcX2, srcY2 ); }

    // --------- VgaBuf to VgaBuf copy ------------ //

    void    blt_buf( VgaBuf *srcBuf, int x1, int y1 );
    // put whole srcBuf at (x1, y1) of this Vgabuf

    void    blt_buf_fast( VgaBuf *srcBuf, int srcX1, int srcY1, int srcX2, int srcY2)
	{ IMGcopyW( buf_ptr(), buf_true_pitch(), srcBuf->buf_ptr(), srcBuf->buf_true_pitch(), srcX1, srcY1, srcX2, srcY2); }
    // cut (srcX1,srcY1)-(srcX2,srcY2) of srcBuf and put at (srcX1,srcY1) of this VgaBuf

    void    blt_buf_bright( VgaBuf *srcBuf, int srcX1, int srcY1, int srcX2, int srcY2, int brightness);

    void    join_trans_remap( VgaBuf *srcBuf, int x, int y, char *bitmapBuf, short *colorRemapTable)
	{ IMGjoinTransRemap( buf_ptr(), buf_true_pitch(), srcBuf->buf_ptr(), srcBuf->buf_true_pitch(), x, y, bitmapBuf, colorRemapTable ); }

    void    join_trans( VgaBuf *srcBuf, int x, int y, char *bitmapBuf )
	{ IMGjoinTransRemap( buf_ptr(), buf_true_pitch(), srcBuf->buf_ptr(), srcBuf->buf_true_pitch(), x, y, bitmapBuf, default_remap_table ); }
};

extern VgaBuf vga_front, vga_back, vga_true_front;

//--------------------------------------------//
#endif
