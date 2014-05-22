//Filename    : COLOR.H
//Description : Game Color Definition

#ifndef __COLOR_H
#define __COLOR_H

//--------- Define color codes, code from cap ----------//

#define VGA_BROWN             0x01                // array of color
#define VGA_YELLOW            0x10
#define VGA_RED               0x20
#define VGA_SEA_GREEN         0x30
#define VGA_LAND_GRASS      0x50
#define VGA_LAND_HILL       0x60
#define VGA_GRAY              0x70
#define VGA_OLIVE             0x80
#define VGA_PINK              0x90
#define VGA_VIOLET            0xA0
#define VGA_SOIL              0xB0
#define VGA_ORANGE            0xC0
#define VGA_GRASS_GREEN       0xD0
#define VGA_IF_COLOR        0xE0
#define VGA_SEA_BLUE          0xF0

#define V_BLACK               0x00                // single color only
#define V_GRAY                122
#define V_WHITE               VGA_GRAY
#define V_ORANGE              VGA_ORANGE+6
#define V_RED                 VGA_RED+4
#define V_GREEN               VGA_SEA_GREEN+5
#define V_BLUE                VGA_SEA_BLUE+5
#define V_VIOLET              VGA_VIOLET+4
#define V_BROWN               VGA_BROWN+5
#define V_PINK                VGA_PINK+4
#define V_YELLOW              VGA_YELLOW+2

#define V_BACKGROUND        0xFF                  // background color, pixels of this color are not put in VGAputIcon
#define V_TRANS         0x00                      //temp for axoview

//---------- Define Game Colors --------------//

#define OWN_SELECT_FRAME_COLOR    V_YELLOW        // for mousescr.cpp only!?

//---------------------------------------------//
#endif
