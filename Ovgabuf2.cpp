//Filename    : OVGABUF2.CPP
//Description : OVGABUF direct draw surface class - part 2

#include <ALL.H>
#include <IMGFUN.H>
#include <OMOUSE.H>
#include <OPOWER.H>
#include <OCOLTBL.H>
#include <OVGA.H>
#include <OVGABUF.H>
#include <OIMGRES.H>
#include <OSYS.H>
#include <OFONT.H>
#include <OBITMAP.H>
#include <OBITMAPW.H>

//------- Define static class member vars ---------//

short VgaBuf::color_scheme;
char  VgaBuf::color_light;
char  VgaBuf::color_dark;
char  VgaBuf::color_up;
char  VgaBuf::color_down;
char  VgaBuf::color_push;
char  VgaBuf::color_border;

//---------- Begin of function VgaBuf::set_color ---------//
//!
//! Set the color of the 3D panel
//!
//! <int> colorScheme - VGA_???
//!
void VgaBuf::set_color(int colorScheme) {
    color_scheme = colorScheme;

    color_light  = colorScheme;
    color_dark   = colorScheme+8;
    color_up     = colorScheme+4;                   // color_up   = colorScheme+2
    color_down   = colorScheme+2;                   // color_down = colorScheme
    color_push   = colorScheme+6;
    color_border = colorScheme+10;
}

//------------ End of function VgaBuf::set_color ---------//

//------------- Begin of function VgaBuf::bar --------------//
//!
//! Draw a bar without bliting
//!
//! Syntax : bar( x1, y1, x2, y2, color )
//!
//! <int> x1,y1       - the top left vertex of the bar
//! <int> x2,y2       - the bottom right vertex of the bar
//! <int> color index - the index of a 256 color palette
//!
void VgaBuf::bar(int x1,int y1,int x2,int y2,int colorCode) {
    err_when( !buf_locked );

    if(sys.no_true_output_flag)
	if( is_front )
	    return;

    if( is_front )
	mouse.hide_area(x1,y1,x2,y2);

    //	IMGbar(buf_ptr(), buf_true_pitch(), x1, y1, x2, y2, translate_color(colorCode) );

    for(int j=y1;j<=y2;j++)
	for(int i=x1;i<=x2;i++) {
	    short readpixel=(*(buf_ptr()+buf_pitch()*j+i));

	    if(((colorCode!=5)&&(colorCode!=Vga::active_buf->color_dark))
	       ||(readpixel!=translate_color(193)))
		draw_pixel(i,j,colorCode);
	}

    if( is_front )
	mouse.show_area();
}

//--------------- End of function VgaBuf::bar --------------//

/*
//------------- Begin of function VgaBuf::pixelize --------------//
//!
//! Put an area of evenly distributed pixels on the given area.
//!
//! <int> x1,y1     - the top left vertex of the bar
//! <int> x2,y2     - the bottom right vertex of the bar
//! <int> colorCode - colorCode of the pixels
//!
void VgaBuf::pixelize(int x1,int y1,int x2,int y2,int colorCode)
{
err_when( !buf_locked );

if( is_front )
mouse.hide_area(x1,y1,x2,y2);

//---------------------------------------//

// ###### begin Gilbert 7/7 #########//
int	bufPitch = buf_pitch();
int   x, y, lineRemainBytes = bufPitch - (x2-x1+1);
char* writePtr = buf_ptr() + bufPitch * y1 + x1;

for( y=y1 ; y<=y2 ; y++ )
{
if( y&1 )
{
writePtr+=bufPitch;
}
else
{
for( x=x1 ; x<=x2 ; x++, writePtr++ )
{
if( x&1 )
*writePtr = colorCode;
}

writePtr+=lineRemainBytes;
}
}
// ###### end Gilbert 7/7 #########//

//---------------------------------------//

if( is_front )
mouse.show_area();
}
//--------------- End of function VgaBuf::pixelize --------------//
*/

//------------- Begin of function VgaBuf::rect --------------//
//!
//! Draw a rect on VGA screen
//!
//! Syntax : rect( x1, y1, x2, y2, lineWidth, color )
//!
//! int x1,y1       - the top left vertex of the rect
//! int x2,y2       - the bottom right vertex of the rect
//! int lineThick   - thickness of the lines of the rect
//! int color       - the color of the rect
//!
void VgaBuf::rect(int x1,int y1,int x2,int y2,int lt, int color) {
    lt--;

    bar(x1,y1,x2,y1+lt,color);
    bar(x1,y1,x1+lt,y2,color);
    bar(x1,y2-lt,x2,y2,color);
    bar(x2-lt,y1,x2,y2,color);
}

//--------------- End of function VgaBuf::rect --------------//

//------------- Start of function VgaBuf::d3_rect --------------//
//
// Draw a d3 rect on VGA screen
//
// Syntax : d3_rect( x1, y1, x2, y2, lineWidth, color )
//
// int x1,y1       - the top left vertex of the d3_rect
// int x2,y2       - the bottom right vertex of the d3_rect

void VgaBuf::d3_rect(int x1,int y1,int x2,int y2) {
    rect( x1+1, y1+1, x2, y2, 1, V_WHITE );
    rect( x1, y1, x2-1, y2-1, 1, VGA_GRAY+8 );
}

//--------------- End of function VgaBuf::d3_rect --------------//

//------------- Start of function VgaBuf::tile --------------//
//
// Fill an area with a specific tile
//
// <int>   x1,y1    - the top left vertex of the tile
// <int>   x2,y2    - the bottom right vertex of the tile
// <char*> tileName - the name of the tile
//
void VgaBuf::tile(int x1,int y1,int x2,int y2,char* tileName) {
    err_if( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT )
	err_now( "VgaBuf::tile" );
    /*
      if( is_front )
      mouse.hide_area( x1,y1,x2,y2 );

      char* tilePtr = image_tile.get_ptr(tileName);

      if( tilePtr )
      VGAtile(x1,y1,x2,y2,tilePtr);

      if( is_front )
      mouse.show_area();
    */
}

//--------------- End of function VgaBuf::tile --------------//

//------------- Start of function VgaBuf::separator --------------//
//
// Draw a VGA separator line
//
// Syntax : separator( x1, y1, x2, y2 )
//
// int x1,y1       - the top left vertex of the separator
// int x2,y2       - the bottom right vertex of the separator
//
void VgaBuf::separator(int x1, int y1, int x2, int y2) {
    if( y1+1==y2 ) {                                // horizontal line
	bar( x1, y1, x2, y1, V_WHITE        );
	bar( x1, y2, x2, y2, color_dark );
    }
    else {
	bar( x1, y1, x1, y2, V_WHITE        );
	bar( x2, y1, x2, y2, color_dark );
    }
}

//--------------- End of function VgaBuf::separator --------------//

//------------- Start of function VgaBuf::indicator --------------//
//
// <int>   x1, y1, x2, y2 = coordination of the indicator
// <float> curValue       = the value of the bar
// <float> maxValue       = max value, the bar width = maxBarWidth * curValue / maxValue
// <int>   indiColor      = color of the indicator
// [int]   backColor      = background color
//                          (default : vga.color_down)
//			    (-2 if don't paint background color)
//
void VgaBuf::indicator(int x1, int y1, int x2, int y2, float curValue,
		       float maxValue, int indiColor, int backColor) {
    if(sys.no_true_output_flag)
	if( is_front )
	    return;

    if( backColor == -1 )
	backColor = color_down;

    if( curValue > maxValue )
	curValue = maxValue;

    if( curValue > 0 ) {
	int barWidth = (int) ((float)(x2-x1) * curValue / maxValue);

	int halfHeight = (y2-y1+1)/2-1;
	int tx2        = x1+barWidth;
	int y;
	double ic=indiColor;

	ic+=halfHeight;

	bar( x1-1, y1-1, x1+barWidth+1, y2+1, V_BLACK );

	for( y=y1 ; y<y1+(halfHeight/2) ; y++, ic-=2.0 )
	    bar( x1, y, tx2, y, (int)ic);

	for( ; y<=y2 ; y++, ic+=0.75 )
	    bar( x1, y, tx2, y, (int)ic );

	if( backColor != -2 ) {                       // -2 if don't paint background color
	    if( x1+barWidth < x2 )
		bar( x1+barWidth+1, y1, x2, y2, backColor );
	}
    }
    else {
	if( backColor != -2 )                         // -2 if don't paint background color
	    bar( x1, y1, x2, y2, backColor );
    }

}

//--------------- End of function VgaBuf::indicator --------------//

//------------- Start of function VgaBuf::v_indicator --------------//
//
// <int>   x1, y1, x2, y2 = coordination of the indicator
// <float> curValue       = the value of the bar
// <float> maxValue       = max value, the bar width = maxBarWidth * curValue / maxValue
// <int>   indiColor      = color of the indicator
// [int]   backColor      = background color
//                          (default : vga.color_down)
//			    (-2 if don't paint background color)
//
void VgaBuf::v_indicator(int x1, int y1, int x2, int y2, float curValue,
			 float maxValue, int indiColor, int backColor) {
    if(sys.no_true_output_flag)
	if( is_front )
	    return;

    if( backColor == -1 )
	backColor = color_down;

    if( curValue > maxValue )
	curValue = maxValue;

    if( curValue > 0 ) {
	int barHeight = (int) ((float)(y2-y1) * curValue / maxValue);

	int halfWidth = (x2-x1+1)/2-1;
	int ty2        = y1+barHeight;
	int x;

	indiColor+=halfWidth;

	for( x=x1 ; x<x1+halfWidth; x++, indiColor-- )
	    bar( x, y1, x, ty2, indiColor );

	for( ; x<=x2 ; x++, indiColor++ )
	    bar( x, y1, x, ty2, indiColor );

	if( backColor != -2 ) {                       // -2 if don't paint background color
	    if( y1+barHeight < y2 )
		bar( x1, y1+barHeight+1, x2, y2, backColor );
	}
    }
    else {
	if( backColor != -2 )                         // -2 if don't paint background color
	    bar( x1, y1, x2, y2, backColor );
    }
}

//--------------- End of function VgaBuf::v_indicator --------------//

/*
//------------- Start of function VgaBuf::indicator --------------//
//
// <int>   barType        = bar style, bit 0 = disp curValue, bit 1 =  disp '/' and maxValue, bit 2 = longer bar, bit 3 = use another back buffer
// <int>   x1, y1         = coordination of the indicator
// <float> curValue       = the value of the bar
// <float> maxValue       = max value, the bar width = maxBarWidth * curValue / maxValue
// <int>   colorScheme    = color of the indicator // not used
//
void VgaBuf::indicator(int barType, int x1, int y1, float curValue, float maxValue, int colorScheme)
{
// -------- get indicator bar background ------------//

const TEMP_BUFFER_SIZE = 0x2000;
static short tempBuffer[TEMP_BUFFER_SIZE];

char *bgPtr = image_button.get_ptr(barType & 4 ? "V_HP-BX" : "V_PW-BX");
if( !bgPtr )
{
err_here();
return;
}

// -------- calculate the boundary or the indicator ---------//
int width = ((Bitmap *)bgPtr)->get_width();
int height = ((Bitmap *)bgPtr)->get_height();

int bufferX1 = x1;
int bufferY1 = y1;

short *destPtr = buf_ptr();
int destPitch = buf_true_pitch();

if(barType & 8)
{
if( ((Bitmap *)bgPtr)->size() < TEMP_BUFFER_SIZE )
{
bufferX1 = 0;
bufferY1 = 0;
((BitmapW *)tempBuffer)->init(width, height);
destPtr = ((BitmapW *)tempBuffer)->bitmap;
destPitch = ((BitmapW *)tempBuffer)->get_true_pitch();
}
else
{
barType &= ~8;		// indicator too big, directly write to the buffer
}
}

int x2 = x1 + width -1;
int y2 = y1 + height -1;
int bufferX2 = bufferX1 + width -1;
int bufferY2 = bufferY1 + height -1;

// -------- draw background ---------//

if( is_front )
mouse.hide_area(x1, y1, x2, y2 );

IMGbltRemap( destPtr, destPitch, bufferX1, bufferY1, bgPtr, default_remap_table);

const barLeftMargin = 5;
const barRightMargin = 5;
const barTopMargin = 5;
const barBottomMargin = 5;

// ---------- draw energy bar ----------//

if( curValue > maxValue )
curValue = maxValue;
int barWidth = int( (width - barLeftMargin - barRightMargin ) * curValue / maxValue);
int barX1 = bufferX1 + barLeftMargin;
int barX2 = barX1 + barWidth - 1;
int barHeight = height - (barTopMargin + barBottomMargin);
int barY1 = bufferY1 + barTopMargin;

unsigned int lc = 0;
static unsigned char lineColor[8] =
{ VGA_LIGHT_BLUE, VGA_LIGHT_BLUE+1, VGA_DARK_BLUE, VGA_LIGHT_BLUE+2,
VGA_DARK_BLUE+1, VGA_LIGHT_BLUE+3, VGA_DARK_BLUE+2, VGA_DARK_BLUE+3 };

if( barWidth > 0 )
{
if(barHeight >= 8)
{
int bY1 = barY1;
for( int c = 0; c < 8; ++c)
{
int bY2 = bY1 + (lc+ barHeight -1)/8;
IMGbar(destPtr, destPitch, barX1, bY1, barX2, bY2, translate_color(lineColor[c]) );
lc += barHeight;
bY1 += lc / 8;
lc %= 8;
}
}
else
{
for( int y = 0; y < barHeight; ++y)
{
int c = lc/barHeight;
IMGbar(destPtr, destPitch, barX1, barY1+y, barX2, barY1+y, translate_color(lineColor[c]));
lc += 8;
}
}
}

// ---------- display value ----------//

if( barType & 3)
{
String str;

if( barType & 1)
str  = (int) curValue;
if( barType & 2)
{
str += "/";
str += (int) maxValue;
}

font_hitpoint.center_put_to_bufferW( destPtr, destPitch, bufferX1, bufferY1, bufferX2, bufferY2, str);
}

if(barType & 8)
{
put_bitmapW(x1, y1, tempBuffer);
}
// ---------------------------------- //

if( is_front )
mouse.show_area();

}
//------------- End of function VgaBuf::indicator --------------//

//------------- Start of function VgaBuf::v_indicator --------------//
//
// Vertical indicator, all parameters are same as indicator() except
// it is vertical.
//
// <int>   x1, y1, x2, y2 = coordination of the indicator
// <float> curValue       = the value of the bar
// <float> maxValue       = max value, the bar width = maxBarWidth * curValue / maxValue
// <int>   indiColor      = color of the indicator
// [int]   backColor      = background color
//                          (default : vga.color_down)
//
void VgaBuf::v_indicator(int x1, int y1, int x2, int y2, float curValue,
float maxValue, int indiColor, int backColor)
{
if( backColor == -1 )
backColor = color_down;

if( curValue > 0 )
{
int barHeight = (int) ((float)(y2-y1) * curValue / maxValue);

int halfWidth = (x2-x1+1)/2-1;
int ty1        = max(y2-barHeight,y1); // when curValue>0, even the actual bar width < 1, one pixel will also be painted
int x;

indiColor+=halfWidth;

for( x=x1 ; x<x1+halfWidth ; x++, indiColor-- )
bar( x, ty1, x, y2, indiColor );

for( ; x<=x2 ; x++, indiColor++ )
bar( x, ty1, x, y2, indiColor );

if( y1 < y2-barHeight )
bar( x1, y1, x2, y2-barHeight-1, backColor );
}
else
{
bar( x1, y1, x2, y2, backColor );
}
}
//--------------- End of function VgaBuf::v_indicator --------------//
*/

//---------- Begin of function VgaBuf::line -------------//
//!
//! Draw a line
//!
//! <int> x1,y1,x2,y2 = the coordination of the line
//! <int> lineColor   = color of the line
//!
void VgaBuf::line(int x1,int y1,int x2,int y2,int lineColor) {
    if( is_front )
	mouse.hide_area( x1,y1,x2,y2 );               // if the mouse cursor is in that area, hide it

    IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1, y1, x2, y2, lineColor);

    if( is_front )
	mouse.show_area();
}

//------------ End of function VgaBuf::line -------------//

//---------- Begin of function VgaBuf::thick_line -------------//
//!
//! Draw a thick line
//!
//! <int> x1,y1,x2,y2 = the coordination of the line
//! <int> lineColor   = color of the line
//!
void VgaBuf::thick_line(int x1,int y1,int x2,int y2,int lineColor) {
    err_when( x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    if( is_front )
	mouse.hide_area( x1,y1,x2,y2 );               // if the mouse cursor is in that area, hide it

    if( y1-y2 > abs(x2-x1) ) {                      // keep thickness of the line to 3
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1  , y1-1, x2  , y2-1, lineColor );
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1  , y1  , x2  , y2  , lineColor );
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1  , y1+1, x2  , y2+1, lineColor );
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1+1, y1+1, x2+1, y2+1, lineColor );
    }

    else if( y2-y1 > abs(x2-x1) ) {
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1+1, y1-1, x2+1, y2-1, lineColor );
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1  , y1-1, x2  , y2-1, lineColor );
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1  , y1  , x2  , y2  , lineColor );
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1  , y1+1, x2  , y2+1, lineColor );
    }

    else {
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1, y1-1, x2, y2-1, lineColor );
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1, y1  , x2, y2  , lineColor );
	IMGline(buf_ptr(), buf_true_pitch(), buf_width(), buf_height(), x1, y1+1, x2, y2+1, lineColor );
    }

    if( is_front )
	mouse.show_area();
}

//------------ End of function VgaBuf::thick_line -------------//

//----------- Begin of function VgaBuf::d3_panel_up ------------//
//!
//! Draw a Ms windows style 3D panel
//!
//! thickness can be 1-4
//!
//! <int> x1,y1,x2,y2 = the four vertex of the panel
//! [int] thick       = thickness of the border
//!                     (default:2)
//! [int] paintCentre = paint the center area of the plane or not
//!                     pass 0 if the area has just been painted with bar()
//!                     (default:1)
//! [char*] tileName  = name of the texture tile if this panel should
//!							  be filled with tile texture (default: NULL)
//!
void VgaBuf::d3_panel_up(int x1,int y1,int x2,int y2,int t,int paintCentre, char* tileName) {
    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    if(sys.no_true_output_flag)
	if( is_front )
	    return;
    if( is_front )
	mouse.hide_area( x1,y1,x2,y2 );

    //------------------------------------------------//

    if( paintCentre )
    {
	if( tileName )
	    tile( x1+t, y1+t, x2-t, y2-t, tileName);
	else
	    bar_fast(x1+t, y1+t, x2-t, y2-t, 193/*color_up*/ );  // center
    }

    int i, x, y;

    for( i=t-1, y=y1+t-1 ; y>=y1 ; y--,i-- )
	bar_fast( x1+i,y,x2-i,y, i==0?V_WHITE:193//color_light 
	    );     // top side

    for( i=t-1, x=x1+t-1 ; x>=x1 ; x--,i-- )
	bar_fast( x,y1+i,x,y2-i, i==0?V_WHITE:193 );     // left side

    for( i=t-1, y=y2-t+1 ; y<=y2 ; y++,i-- )
	bar_fast( x1+i,y,x2-i,y, i==0?color_dark+2:color_dark-2 );   // bottom side

    for( i=t-1, x=x2-t+1 ; x<=x2 ; x++,i-- )                          // right side
	bar_fast( x,y1+i,x,y2-i, i==0?color_dark+2:color_dark-2 );

    //-------------------------------------------//

    if( is_front )
	mouse.show_area();
}
//------------- End of function VgaBuf::d3_panel_up ------------//


//----------- Begin of function VgaBuf::d3_panel_down ------------//
//!
//! Draw a Ms windows style 3D panel with panel pushed downwards
//!
//! <int> x1,y1,x2,y2 = the four vertex of the panel
//! [int] thick       = thickness of the border
//!                     (default:2)
//! [int] paintCentre = paint the center area of the plane or not
//!                     pass 0 if the area has just been painted with bar()
//!                     (default:1)
//! [char*] tileName  = name of the texture tile if this panel should
//!							  be filled with tile texture (default: NULL)
//!
void VgaBuf::d3_panel_down(int x1,int y1,int x2,int y2,int t,int paintCentre, char* tileName)
{
    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    if(sys.no_true_output_flag)
	if( is_front )
	    return;

    if( is_front )
	mouse.hide_area( x1,y1,x2,y2 );

    //---------- main center area -----------//

    // ####### begin Gilbert 12/7 ##########//
    if( paintCentre )
    {
	if( tileName )
	    tile( x1+t, y1+t, x2-t, y2-t, tileName );
	else
	    bar_fast(x1+t, y1+t, x2-t, y2-t, color_down );  // center
    }

    if( t == 2 ) // when thickness==2, use four different color for the down panel
    {
	// outer lines

	bar_fast( x1,y1,x2-1,y1,color_scheme+8  );     // top
	bar_fast( x1+1,y2,x2,y2,V_WHITE         );     // bottom
	bar_fast( x1,y1,x1,y2-1,color_scheme+8  );     // left
	bar_fast( x2,y1+1,x2,y2,V_WHITE         );     // right

	// inner lines

	bar_fast( x1+1,y1+1,x2-1,y1+1,color_scheme+15 );  // bottom
	bar_fast( x1+1,y2-1,x2-1,y2-1,color_scheme+1  );  // top
	bar_fast( x1+1,y1+1,x1+1,y2-1,color_scheme+15 );  // left
	bar_fast( x2-1,y1+1,x2-1,y2-1,color_scheme+1  );  // right
    }
    else         // one unit thickness
    {
	int i, x, y;

	for( i=t-1, y=y2-t+1 ; y<=y2 ; y++,i-- )
	    bar_fast( x1+i,y,x2-i,y,i==0?V_WHITE:color_light );     // bottom

	for( i=t-1, x=x2-t+1 ; x<=x2 ; x++,i-- )          // left
	    bar_fast( x,y1+i,x2,y2-i,i==0?V_WHITE:color_light );     // right

	for( i=t-1, y=y1+t-1 ; y>=y1 ; y--,i-- )          // top
	    bar_fast( x1+i,y,x2-i,y,i==0?color_dark+2:color_dark-2 );

	for( i=t-1, x=x1+t-1 ; x>=x1 ; x--,i-- )          // left
	    bar_fast( x,y1+i,x,y2-i,i==0?color_dark+2:color_dark-2 );
    }

    //----------- show mouse ----------//

    if( is_front )
	mouse.show_area();
}
//------------- End of function VgaBuf::d3_panel_down ------------//


//----------- Begin of function VgaBuf::d3_panel_up_clear ------------//
//!
//! clear the center of the either up or down panel
//!
//! <int> x1,y1,x2,y2 = the four vertex of the panel
//! [int] thick       = thickness of the border
//!                     (default:2)
//! [char*] tileName  = name of the texture tile if this panel should
//!							  be filled with tile texture (default: NULL)
//!
void VgaBuf::d3_panel_up_clear(int x1,int y1,int x2,int y2,int t, char *tileName)
{
    if(sys.no_true_output_flag)
	if( is_front )
	    return;

    if( tileName )
	tile( x1+t, y1+t, x2-t, y2-t, tileName );
    else
	bar(x1+t, y1+t, x2-t, y2-t, color_up );
}
//------------- End of function VgaBuf::d3_panel_up_clear ------------//


//----------- Begin of function VgaBuf::d3_panel_down_clear ------------//
//!
//! clear the center of the either up or down panel
//!
//! <int> x1,y1,x2,y2 = the four vertex of the panel
//! [int] thick       = thickness of the border
//!                     (default:2)
//! [char*] tileName  = name of the texture tile if this panel should
//!							  be filled with tile texture (default: NULL)
//!
void VgaBuf::d3_panel_down_clear(int x1,int y1,int x2,int y2,int t, char *tileName)
{
    if(sys.no_true_output_flag)
	if( is_front )
	    return;
    if( tileName )
	tile( x1+t, y1+t, x2-t, y2-t, tileName );
    else
	bar(x1+t, y1+t, x2-t, y2-t, color_down );
}
//------------- End of function VgaBuf::d3_panel_down_clear ------------//


//----------- Begin of function VgaBuf::adjust_brightness ------------//
//!
//! clear the center of the either up or down panel
//!
//! <int> x1,y1,x2,y2  = the four vertex of the panel
//! <int> adjustDegree = the degree of brightness to adjust
//!							   (a value from -10 to 10)
//!
void VgaBuf::adjust_brightness(int x1,int y1,int x2,int y2,int adjustDegree)
{
    if( is_front )
	mouse.hide_area( x1,y1,x2,y2 );

#if( MAX_BRIGHTNESS_ADJUST_DEGREE > 10 )
    adjustDegree *= MAX_BRIGHTNESS_ADJUST_DEGREE / 10;
#endif

    err_when( adjustDegree < -MAX_BRIGHTNESS_ADJUST_DEGREE ||
	      adjustDegree >  MAX_BRIGHTNESS_ADJUST_DEGREE );

//	unsigned char* colorRemapTable = vga.vga_color_table->get_table(adjustDegree);
//	remap_bar(x1, y1, x2, y2, colorRemapTable);
    IMGbrightBar( buf_ptr(), buf_true_pitch(), x1, y1, x2, y2, adjustDegree);

    if( is_front )
	mouse.show_area();
}
//------------- End of function VgaBuf::adjust_brightness ------------//


//----------- Begin of function VgaBuf::blt_buf_bright ----------//
//!
//! copy and change brightness of an area
//!
void VgaBuf::blt_buf_bright( VgaBuf *srcBuf, int srcX1, int srcY1, int srcX2, int srcY2, int adjustDegree)
{
#if( MAX_BRIGHTNESS_ADJUST_DEGREE > 10 )
    adjustDegree *= MAX_BRIGHTNESS_ADJUST_DEGREE / 10;
#endif

    err_when( adjustDegree < -MAX_BRIGHTNESS_ADJUST_DEGREE ||
	      adjustDegree >  MAX_BRIGHTNESS_ADJUST_DEGREE );

    IMGcopyWbright( buf_ptr(), buf_true_pitch(), srcBuf->buf_ptr(), srcBuf->buf_true_pitch(), srcX1, srcY1, srcX2, srcY2, adjustDegree); 
}
//----------- End of function VgaBuf::blt_buf_bright ----------//


//------------- Begin of function VgaBuf::blt_buf ------------//
//! copy put whole part of a vgaBuf to (x1,y1) of this VgaBuf
void VgaBuf::blt_buf( VgaBuf *srcBuf, int x1, int y1 )
{
    short *srcPtr = srcBuf->buf_ptr();
    int srcWidth = srcBuf->buf_width();
    int srcPitch = srcBuf->buf_true_pitch();
    int srcHeight = srcBuf->buf_height();
    short *destPtr = buf_ptr();
    int destPitch = buf_true_pitch();

    _asm
	{
	    mov	eax, y1
		imul	destPitch
		add	eax, x1
		add	eax, x1
		mov	edi, eax
		add	edi, destPtr

		mov	esi, srcPtr

		mov	ecx, srcHeight
		blt_buf_1:
	    push	ecx
		push	esi
		push	edi
		mov	ecx, srcWidth
		rep movsw
		pop	edi
		pop	esi
		pop	ecx
		add	edi, destPitch
		add	esi, srcPitch
		loop	blt_buf_1
		}
}
//------------- End of function VgaBuf::blt_buf ------------//
