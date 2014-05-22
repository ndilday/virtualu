//Filename    : OIFACE.H
//Description : Header of object UserInterface

#ifndef __OIFACE_H
#define __OIFACE_H

#include <OVGA.H>
#include <OVGABUF.H>
#include <OCOLTBL.H>
#include <OFONT.H>

//--------- Define class UserInterface --------//
//
//!General functions for user interface manipulation, such as brightness
//!and color palette.
class UserInterface {
public:
    UserInterface();
    ~UserInterface();

    void bg_img(int imgId, VgaBuf *vgaBuf=Vga::active_buf);

    void rect(int, int, int, int, int=3, int=Vga::active_buf->color_up, int=Vga::active_buf->color_dark, VgaBuf* =Vga::active_buf);
    void h_line(int, int, int, int=3, int=Vga::active_buf->color_up, int=Vga::active_buf->color_dark, VgaBuf* = Vga::active_buf);
    void v_line(int, int, int, int=3, int=Vga::active_buf->color_up, int=Vga::active_buf->color_dark, VgaBuf* = Vga::active_buf);
    void bar(int, int, int, int, int=Vga::active_buf->color_up, int=Vga::active_buf->color_dark, VgaBuf* = Vga::active_buf);
    void panel(int, int, int, int, int=Vga::active_buf->color_up, int=Vga::active_buf->color_light, int=Vga::active_buf->color_dark, VgaBuf* = Vga::active_buf);
    void paint(int, int, int, int, int=Vga::active_buf->color_up, VgaBuf* = Vga::active_buf);

    void rect(int x1, int y1, int x2, int y2, VgaBuf* vgaBufPtr)
	{ rect(x1, y1, x2, y2, 3, vgaBufPtr->color_up, vgaBufPtr->color_dark, vgaBufPtr); }
    void h_line(int x1, int y1, int length, VgaBuf* vgaBufPtr)
	{ h_line(x1, y1, length, 3, vgaBufPtr->color_up, vgaBufPtr->color_dark, vgaBufPtr); }
    void v_line(int x1, int y1, int height, VgaBuf* vgaBufPtr)
	{ v_line(x1, y1, height, 3, vgaBufPtr->color_up, vgaBufPtr->color_dark, vgaBufPtr); }
    void bar(int x1, int y1, int x2, int y2, VgaBuf* vgaBufPtr)
	{ bar(x1, y1, x2, y2, vgaBufPtr->color_up, vgaBufPtr->color_dark, vgaBufPtr); }
    void panel(int x1, int y1, int x2, int y2, VgaBuf* vgaBufPtr)
	{ panel(x1, y1, x2, y2, vgaBufPtr->color_up, vgaBufPtr->color_light, vgaBufPtr->color_dark, vgaBufPtr); }
    void paint(int x1, int y1, int x2, int y2, VgaBuf* vgaBufPtr)
	{ paint(x1, y1, x2, y2, vgaBufPtr->color_up, vgaBufPtr); }

    void adjust_brightness(int, int, int, int, int, VgaBuf* =Vga::active_buf);
    void brighten(int, int, int, int, VgaBuf* =Vga::active_buf);
    void darken(int, int, int, int, VgaBuf* =Vga::active_buf);

    void create_button_bitmap(int, int, int, int, char *, short**, short**, VgaBuf* =Vga::active_buf, Font* = &font_charts);

public:
    int cur_img_id;
    char backgd_flag;

private:
    unsigned char palette[256 * 3];
    short *remap_table;
    ColorTable *color_table;
    // ##### begin Gilbert 26/04/2001 ######//
    short *save_area_buffer;
    int save_area_buffer_size;
    // ##### end Gilbert 26/04/2001 ######//
};
//--------------------------------------------//

extern UserInterface user_interface;
#endif
