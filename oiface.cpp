//Filename    : OIFACE.CPP
//Description : UserInterface class

// Comments:	general functions for user interface manipulation
//					refer to "admissions and financial aid office", <Oadm_off>
// Norris 0714

#include <ALL.H>
#include <OSYS.H>
#include <OWORLDMT.H>
#include <OIFACE.H>
#include <OVGA.H>
#include <COLOR.H>
#include <OCOLTBL.H>
#include <OIMGRES.H>
#include <OFONT.H>
#include <OINFO.H>
#include <OFILE.H>
#include <ORESX.H>
#include <OBITMAPW.H>

//--------- define constant ---------//

#define BG_FILENAME "IFACE"                       // background images filename prefix
#define NUM_BG_IMG  19                            // number of background images

enum {
    BRIGHTENING_IDX = 6,
    DARKENING_IDX = -2
};

//-------- Begin of function UserInterface::UserInterface ------//
//!
UserInterface::UserInterface() {
    memset(this, 0, sizeof(UserInterface));
    color_table = new ColorTable;
    backgd_flag = 1;
    cur_img_id = 0;
    // ##### begin Gilbert 26/04/2001 ######//
    save_area_buffer = NULL;
    save_area_buffer_size = 0;;
    // ##### end Gilbert 26/04/2001 ######//
}

//---------- End of function UserInterface::UserInterface ------//

//-------- Begin of function UserInterface::~UserInterface -----//
//!
UserInterface::~UserInterface() {
    // ##### begin Gilbert 26/04/2001 ######//
    if( save_area_buffer )
	mem_del(save_area_buffer);
    save_area_buffer = NULL;
    save_area_buffer_size = 0;
    // ##### end Gilbert 26/04/2001 ######//
    delete color_table;
}

//---------- End of function UserInterface::~UserInterface -----//

//-------- Begin of function UserInterface::bg_img ------------//
//!
//!	<int>			imgId			- image id to be drawn
//! [VgaBuf*]	vgaBuf		- ptr to VgaBuf
//!									  (default = Vga::active_buf)
//!
void UserInterface::bg_img(int imgId, VgaBuf *vgaBufPtr) {
    err_when(imgId <= 0 || imgId > NUM_BG_IMG);

    if(backgd_flag==0) {
	//		vgaBufPtr->bar(ZOOM_X1, ZOOM_Y1,ZOOM_X2, ZOOM_Y2, V_WHITE);
	vgaBufPtr->barW_fast(ZOOM_X1, ZOOM_Y1,ZOOM_X2, ZOOM_Y2,0xFFFF);
	//		info.disp_column_bitmap(vgaBufPtr);
	return;
    }

    String imgName = BG_FILENAME;
    imgName += long(imgId);

    int dataSize;
    File* filePtr = image_interface.get_file(imgName, dataSize);

    if (imgId != cur_img_id) {
	short header = filePtr->file_get_short();

	err_when( header != -1 );                     // must use own color palette

	filePtr->file_read(palette, 256 * 3);

	PalDesc palDesc(palette, 3, 256, 6);
	color_table->generate_table_fast(MAX_BRIGHTNESS_ADJUST_DEGREE, palDesc, ColorTable::bright_func);
	remap_table = (short *) color_table->get_table(0);

	cur_img_id = imgId;
    }
    else {
	filePtr->file_seek(filePtr->file_pos() + 256 * 3 + sizeof(short));
    }

    vgaBufPtr->put_large_bitmap(ZOOM_X1, ZOOM_Y1, filePtr, remap_table);

    info.disp_column_bitmap(vgaBufPtr);
}

//---------- End of function UserInterface::bg_img ------------//

/*
//-------- Begin of function UserInterface::construct_palette_brighten ------//
//!
void UserInterface::construct_palette_brighten()
{
  double rgbR, rgbG, rgbB;
  double yiqY, yiqI, yiqQ;
  unsigned char *palettePtr = palette, *paletteBrightenPtr = palette_brighten;
  for (int i = 0; i < 256; i++)
  {
    yiqY = 0.2990*(rgbR=double(*palettePtr++)/63.0);
yiqY += 0.5870*(rgbG=double(*palettePtr++)/63.0);
yiqY += 0.1140*(rgbB=double(*palettePtr++)/63.0) + BRIGHTEN_IDX;
yiqI = 0.5960*rgbR - 0.2750*rgbG - 0.3210*rgbB;
yiqQ = 0.2120*rgbR - 0.5230*rgbG + 0.3110*rgbB;
rgbR = yiqY + 0.9557*yiqI + 0.6199*yiqQ;
rgbG = yiqY - 0.2716*yiqI - 0.6469*yiqQ;
rgbB = yiqY - 1.1082*yiqI + 1.7051*yiqQ;
(*paletteBrightenPtr++) = (rgbR > 1.0) ? 63 : short(rgbR*63.0);
(*paletteBrightenPtr++) = (rgbG > 1.0) ? 63 : short(rgbG*63.0);
(*paletteBrightenPtr++) = (rgbB > 1.0) ? 63 : short(rgbB*63.0);
}
PalDesc palDesc(palette_brighten, 3, 256, 6);
color_table_brighten->generate_table_fast(MAX_BRIGHTNESS_ADJUST_DEGREE, palDesc, ColorTable::bright_func);
remap_table_brighten = (short *) color_table_brighten->get_table(0);
}
//---------- End of function UserInterface::construct_palette_brighten ------//
*/

//-------- Begin of function UserInterface::rect ------------//
//!
//! <int>     x1,y1,x2,y2		- coordinates of corners of rectangle
//! [int]     width				- width of rectangle (default: 3)
//! [int]     borderColor		- border color (default: Vga::active_buf->color_up)
//! [int]     shadowColor		- shadow color (default: Vga::active_buf->color_dark)
//! [VgaBuf*] vgaBuf				- ptr to VgaBuf (default = Vga::active_buf)
//!
void UserInterface::rect(int x1, int y1, int x2, int y2, int width, int borderColor, int shadowColor, VgaBuf *vgaBufPtr) {
    //	vgaBufPtr->bar(x1+width, y1+width, x2-width, y1+width+1, shadowColor);
    //	vgaBufPtr->bar(x1+width, y1+width+2, x1+width+1, y2-width, shadowColor);
    //	vgaBufPtr->bar(x1+1, y2+1, x2+2, y2+2, shadowColor);
    //	vgaBufPtr->bar(x2+1, y1+1, x2+2, y2, shadowColor);
    //	vgaBufPtr->bar(x1, y1, x2, y1+(--width), borderColor);
    //	vgaBufPtr->bar(x1, y1+width+1, x1+width, y2, borderColor);
    //	vgaBufPtr->bar(x1+width+1, y2-width, x2, y2, borderColor);
    //	vgaBufPtr->bar(x2-width, y1+width+1, x2, y2-width-1, borderColor);
    int w=width+1;
    if(w<2)w=2;
    // - 1
    vgaBufPtr->bar(x1+w, y1+w, x2-w, y1+w, shadowColor);
    // | 1
    vgaBufPtr->bar(x1+w, y1+w+1, x1+w, y2-w+2, shadowColor);
    vgaBufPtr->bar(x1, y2+2, x2, y2+2, shadowColor);// - 2
    vgaBufPtr->bar(x2, y1, x2, y2+2, shadowColor);  // | 2
    w--;
    // RGB = 75,60,31, Idx =5 (187,150,75)
    // ref to : /ccres/pal/pal.std
    vgaBufPtr->bar(x1+w, y1+w, x2-w, y1+w, 5);      // - 1
    vgaBufPtr->bar(x1+w, y1+w+1, x1+w, y2-w+1, 5);  // | 1
    vgaBufPtr->bar(x1, y2+1, x2-1, y2+1, 5);        // - 2
    vgaBufPtr->bar(x2-1, y1, x2-1, y2+1, 5);        // | 2
    w--;

    if ( borderColor == Vga::active_buf->color_up ) {
	// RGB = 94,87,69 , Idx = 193 (250,217,173)
	vgaBufPtr->bar(x1, y1, x2-2, y1+w, 193);      // - 1
	vgaBufPtr->bar(x1, y1+w, x1+w, y2-w, 193);    // | 1
	vgaBufPtr->bar(x1, y2-w, x2-2, y2, 193);      // - 2
	vgaBufPtr->bar(x2-w-2, y1, x2-2, y2, 193);    // | 2
    }
    else {
	// RGB = 94,87,69 , Idx = 193 (250,217,173)
	// - 1
	vgaBufPtr->bar(x1, y1, x2-2, y1+w, borderColor);
	// | 1
	vgaBufPtr->bar(x1, y1+w, x1+w, y2-w, borderColor);
	// - 2
	vgaBufPtr->bar(x1, y2-w, x2-2, y2, borderColor);
	// | 2
	vgaBufPtr->bar(x2-w-2, y1, x2-2, y2, borderColor);
    }
    //	vgaBufPtr->bar(x1+w, y1+w, x2-1, y1+w, V_BROWN);    // - 1
    //	vgaBufPtr->bar(x1+w, y1+w+1, x1+w, y2-1, V_BROWN);	 // | 1
    //	vgaBufPtr->bar(x1, y2+w, x2+w+1, y2+w, V_BROWN);		 // - 2
    //	vgaBufPtr->bar(x2+w, y1, x2+w, y2+w-1, V_BROWN);	 // | 2
    //	w--;
    //	vgaBufPtr->bar(x1, y1, x2, y1+w, V_ORANGE);			// - 1
    //	vgaBufPtr->bar(x1, y1+w, x1+w, y2, V_ORANGE);		// | 1
    //	vgaBufPtr->bar(x1, y2, x2+w, y2+w, V_ORANGE);		// - 2
    //	vgaBufPtr->bar(x2, y1, x2+w, y2, V_ORANGE);		// | 2

    //	vgaBufPtr->bar(x1+width, y1+width, x2-width, y1+width+1, shadowColor);
    //	vgaBufPtr->bar(x1+width, y1+width+2, x1+width+1, y2-width, shadowColor);
    //	vgaBufPtr->bar(x1+1, y2+1, x2+2, y2+2, shadowColor);
    //	vgaBufPtr->bar(x2+1, y1+1, x2+2, y2, shadowColor);
    //	vgaBufPtr->bar(x1, y1, x2, y1+(--width), V_BROWN);
    //	vgaBufPtr->bar(x1, y1+width+1, x1+width, y2, V_BROWN);
    //	vgaBufPtr->bar(x1+width+1, y2-width, x2, y2, V_BROWN);
    //	vgaBufPtr->bar(x2-width, y1+width+1, x2, y2-width-1, V_BROWN);
    //	vgaBufPtr->bar(x1, y1, x2, y1+(--width), V_ORANGE);
    //	vgaBufPtr->bar(x1, y1+width+1, x1+width, y2, V_ORANGE);
    //	vgaBufPtr->bar(x1+width+1, y2-width, x2, y2, V_ORANGE);
    //	vgaBufPtr->bar(x2-width, y1+width+1, x2, y2-width-1, V_ORANGE);
}

//---------- End of function UserInterface::rect ------------//

//-------- Begin of function UserInterface::h_line ------------//
//!
//! <int>     x1,y1				- upper left coordinate of the horizontal line
//! <int>		 length				- length of line
//! [int]		 width				- width of line (default: 3)
//! [int]     borderColor		- border color (default: Vga::active_buf->color_up)
//! [int]     shadowColor		- shadow color (default: Vga::active_buf->color_dark)
//! [VgaBuf*] vgaBuf				- ptr to VgaBuf (default = Vga::active_buf)
//!
void UserInterface::h_line(int x1, int y1, int length, int width, int borderColor, int shadowColor, VgaBuf *vgaBufPtr) {
    //borderColor);
    vgaBufPtr->bar(x1, y1, x1+length-1, y1+width-1, 193);
    // shadowColor);
    vgaBufPtr->bar(x1+1, y1+width, x1+length+1, y1+width+1,5);
    // shadowColor);
    vgaBufPtr->bar(x1+length+1, y1+1, x1+length+2, y1+width,5);
}

//---------- End of function UserInterface::h_line ------------//

//-------- Begin of function UserInterface::v_line ------------//
//!
//! <int>     x1,y1				- upper left coordinate of the vertical line
//! <int>		 height				- height of line
//! [int]		 width				- width of line (default: 3)
//! [int]     borderColor		- border color (default: Vga::active_buf->color_up)
//! [int]     shadowColor		- shadow color (default: Vga::active_buf->color_dark)
//! [VgaBuf*] vgaBuf				- ptr to VgaBuf (default = Vga::active_buf)
//!
void UserInterface::v_line(int x1, int y1, int height, int width, int borderColor, int shadowColor, VgaBuf *vgaBufPtr) {
    if (borderColor >0 || shadowColor>0) {
	vgaBufPtr->bar(x1, y1, x1+width-1, y1+height-1, borderColor);
	vgaBufPtr->bar(x1+width+1, y1+1, x1+width+2, y1+height+1, shadowColor);
	vgaBufPtr->bar(x1+1, y1+height, x1+width, y1+height+1, shadowColor);
    }
    else {
	//borderColor);
	vgaBufPtr->bar(x1, y1, x1+width-1, y1+height-1, 193);
	// shadowColor);
	vgaBufPtr->bar(x1+width+1, y1+1, x1+width+2, y1+height+1, 5);
	//shadowColor);
	vgaBufPtr->bar(x1+1, y1+height, x1+width, y1+height+1, 5);
    }
}

//---------- End of function UserInterface::v_line ------------//

//-------- Begin of function UserInterface::bar ------------//
//!
//! <int>     x1,y1				- upper left coordinate of the horizontal line
//! <int>     x2,y2				- lower right coordinate of the horizontal line
//! [int]     barColor			- bar color (default: Vga::active_buf->color_up)
//! [int]     shadowColor		- shadow color (default: Vga::active_buf->color_dark)
//! [VgaBuf*] vgaBuf				- ptr to VgaBuf (default = Vga::active_buf)
//!
void UserInterface::bar(int x1, int y1, int x2, int y2, int barColor, int shadowColor, VgaBuf *vgaBufPtr) {
    vgaBufPtr->bar(x1, y1, x2, y2, barColor);
    vgaBufPtr->bar(x1, y2+1, x2, y2+2, shadowColor);
    vgaBufPtr->bar(x2+1, y1, x2+2, y2+2, shadowColor);
}

//---------- End of function UserInterface::bar ------------//

//-------- Begin of function UserInterface::panel ------------//
//!
//! <int>     x1,y1,x2,y2		- coordinates of corners of panel
//! [int]     panelColor			- panel color (default: Vga::active_buf->color_up)
//! [int]     lightColor			- light color (default: Vga::active_buf->color_light)
//! [int]     shadowColor		- shadow color (default: Vga::active_buf->color_dark)
//! [VgaBuf*] vgaBuf				- ptr to VgaBuf (default = Vga::active_buf)
//!
void UserInterface::panel(int x1, int y1, int x2, int y2, int panelColor, int lightColor, int shadowColor, VgaBuf *vgaBufPtr) {
    vgaBufPtr->bar(x1, y1, x2, y1, shadowColor);
    vgaBufPtr->bar(x1, y1+1, x1, y2, shadowColor);
    vgaBufPtr->bar(x1+1, y2, x2, y2, lightColor);
    vgaBufPtr->bar(x2, y1+1, x2, y2-1, lightColor);
    vgaBufPtr->bar(x1+1, y1+1, x2-1, y2-1, panelColor);
}

//---------- End of function UserInterface::panel ------------//

//-------- Begin of function UserInterface::paint ------------//
//!
//! <int>     x1,y1,x2,y2		- coordinates of corners of panel
//! [int]     paintColor			- paint color (default: Vga::active_buf->color_up)
//! [VgaBuf*] vgaBuf				- ptr to VgaBuf (default = Vga::active_buf)
//!
void UserInterface::paint(int x1, int y1, int x2, int y2, int paintColor, VgaBuf *vgaBufPtr) {
    vgaBufPtr->bar(x1, y1, x2, y2, paintColor);
}

//---------- End of function UserInterface::paint ------------//

/*
//-------- Begin of function UserInterface::brighten ----------//
//!
//! <short *> bufPtr				- ptr to image buffer
//!										  format <short> - width
//!													<short> - height
//!													<short> ...
//!
void UserInterface::brighten(short *bufPtr)
{
  int bufSize = int(*bufPtr++);
bufSize *= int(*bufPtr++);

if (cur_img_id <= 0 || cur_img_id > NUM_BG_IMG)
return;

if (!palette_flag)
{
construct_palette_brighten();
palette_flag = 1;
}

for (int i = 0; i < bufSize; i++, bufPtr++)
{
for (int j = 0; j < 256; j++)
{
if (*bufPtr == remap_table[j])
{
*bufPtr = remap_table_brighten[j];
break;
}
}
}
}
//---------- End of function UserInterface::brighten ---------//
*/

//--------- Begin of function UserInterface::adjust_brigheness -------//
//!
void UserInterface::adjust_brightness(int x1, int y1, int x2, int y2, int brightness, VgaBuf *vgaBufPtr) {
#if( MAX_BRIGHTNESS_ADJUST_DEGREE > 10 )
    brightness *= MAX_BRIGHTNESS_ADJUST_DEGREE / 10;
#endif

    err_when( brightness < -MAX_BRIGHTNESS_ADJUST_DEGREE ||
	      brightness >  MAX_BRIGHTNESS_ADJUST_DEGREE );

    IMGbrightBar( vgaBufPtr->buf_ptr(), vgaBufPtr->buf_true_pitch(), x1, y1, x2, y2, brightness);

    /*
      if ((x2 - x1 + 1) * (y2 - y1 + 1) > COMMON_DATA_BUF_SIZE)
      {
      for (short i = y1; i <= y2; i++)
      {
      vgaBufPtr->save_area_common_buf(x1, i, x2, i);
      IMGcopyWbright( vgaBufPtr->buf_ptr(), vgaBufPtr->buf_true_pitch(),
      ((short*)sys.common_data_buf)+6, *((short*)sys.common_data_buf),
      x1, i, x2, i, brightness);
      }
      }
      else
      {
      vgaBufPtr->save_area_common_buf(x1, y1, x2, y2);
      IMGcopyWbright( vgaBufPtr->buf_ptr(), vgaBufPtr->buf_true_pitch(),
      ((short*)sys.common_data_buf)+6, *((short*)sys.common_data_buf),
      x1, y1, x2, y2, brightness);
      }
    */
}

//----------- End of function UserInterface::adjust_brightness -------//

//--------- Begin of function UserInterface::brighten -------//
//!
void UserInterface::brighten(int x1, int y1, int x2, int y2, VgaBuf *vgaBufPtr) {
    if(backgd_flag==0)return;
    adjust_brightness(x1, y1, x2, y2, BRIGHTENING_IDX, vgaBufPtr);
}

//----------- End of function UserInterface::brighten -------//

//--------- Begin of function UserInterface::darken -------//
//!
void UserInterface::darken(int x1, int y1, int x2, int y2, VgaBuf *vgaBufPtr) {
    adjust_brightness(x1, y1, x2, y2, DARKENING_IDX, vgaBufPtr);
}

//----------- End of function UserInterface::darken -------//

//---------- Begin of funcion UserInterface::create_button_bitmap -------//
//
//	<int>			x1, y1, x2, y2			coordinates of the button
// <char*>		label						text label of the button
// <short**>	upBitmapPtr				ptr to button-up bitmap (to use: upBitmapPtr+4)
// <short**>	downBitmapPtr			ptr to button-down bitmap (to use: downBitmapPtr+4)
//	[VgaBuf*]	vgaBufPtr				ptr to VgaBuf (default = Vga::active_buf)
// [Font*]		fontPtr					ptr to font to be used (default = &font_san)
//
void UserInterface::create_button_bitmap(int x1, int y1, int x2, int y2, char *label, short **upBitmapPtr, short **downBitmapPtr, VgaBuf *vgaBufPtr, Font *fontPtr) {
    x1++;
    y1++;
    x2--;
    y2--;

    // ##### begin Gilbert 26/04/2001 ######//
    // reduce mem_add/mem_del
    // short *saveAreaBuf = vgaBufPtr->save_area(x1, y1, x2+2, y2+2);  //## chea 070999 BUGHERE if the button is @ the bottom of the screen
    int saveAreaSize = BitmapW::size(x2+2-x1+1, y2+2-y1+1);
    if( saveAreaSize > save_area_buffer_size ) {
	save_area_buffer_size = saveAreaSize + 0x1000;// allocate 1K byte more
	save_area_buffer = (short *)mem_resize( save_area_buffer, save_area_buffer_size );
    }
    vgaBufPtr->read_bitmapW(x1, y1, x2+2, y2+2, save_area_buffer );
    // ##### end Gilbert 26/04/2001 ######//

    rect(x1, y1, x2+2, y2,1);
    short lineCount = 1;
    char labelLocal[123];
    strcpy( labelLocal, label );
    short breakPos[2]= {                            // Assume can divide into 3 rows
	strlen(label),strlen(label)
    };
    for ( int i=0; i<strlen(label)-1; i++ ) {
	if ( label[i] == '\n' ) {                     // there should be a line break
	    breakPos[lineCount-1] = i-1;                // will copy strings till i
	    lineCount++;
	}
    }
    char oneRowLabel[123]="\0";
    char* tail = "\0";
    int  startY,endY;
    for ( int i=0; i<lineCount; i++ ) {
	strcpy ( oneRowLabel, "" );
	// copy the text need to display
	strncpy( oneRowLabel, labelLocal, breakPos[i]+1 );
	strcpy( oneRowLabel+breakPos[i]+1, tail );
	startY = ((y2+1-(y1+2))/lineCount)*i+(y1+2);
	endY   = ((y2+1-(y1+2))/lineCount)*(i+1)+(y1+2);
	fontPtr->center_put( x1+2, startY, x2, endY, oneRowLabel );
	// Shorten the label
	strcpy( labelLocal, label+breakPos[i]+1 );
    }
    *downBitmapPtr = vgaBufPtr->save_area(x1-1, y1-1, x2+2, y2+2, *downBitmapPtr);
    // ##### begin Gilbert 26/04/2001 ######//
    // vgaBufPtr->rest_area(saveAreaBuf, 0);
    vgaBufPtr->put_bitmapW( x1,y1, save_area_buffer );
    // ##### end Gilbert 26/04/2001 ######//
    brighten(x1, y1, x2+2, y2);
    rect(x1, y1, x2+2, y2,1);
    strcpy( labelLocal, label );
    strcpy(oneRowLabel,"\0");
    tail = "\0";
    startY,endY;
    for ( int i=0; i<lineCount; i++ ) {
	strcpy( oneRowLabel, "" );
	// copy the text need to display
	strncpy( oneRowLabel, labelLocal, breakPos[i]+1 );
	strcpy( oneRowLabel+breakPos[i]+1, tail );
	startY = ((y2+1-(y1+2))/lineCount)*i+(y1+2);
	endY   = ((y2+1-(y1+2))/lineCount)*(i+1)+(y1+2);
	fontPtr->center_put( x1+2, startY, x2, endY, oneRowLabel );
	// Shorten the label
	strcpy( labelLocal, label+breakPos[i]+1 );
    }
    *upBitmapPtr = vgaBufPtr->save_area(x1-1, y1-1, x2+2, y2+2, *upBitmapPtr);
    // ##### begin Gilbert 26/04/2001 ######//
    // vgaBufPtr->rest_area(saveAreaBuf, 1);
    vgaBufPtr->put_bitmapW( x1,y1, save_area_buffer );
    // ##### end Gilbert 26/04/2001 ######//
}

//----------- End of function UserInterface::create_button_bitmap -------//
