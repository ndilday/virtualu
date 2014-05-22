//Filename    : OVGA.CPP
//Description : VGA manipulation functions (Direct Draw version)

#define DEBUG_LOG_LOCAL 1

#include <windowsx.h>

#include <ALL.H>
#include <IMGFUN.H>
#include <COLCODE.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OCOLTBL.H>
#include <OFILE.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OLOG.H>
#include <OVGALOCK.H>
#include <OSYS.H>
//-------- Define constant --------//

#define UP_OPAQUE_COLOR       (VGA_GRAY+10)
#define DOWN_OPAQUE_COLOR     (VGA_GRAY+13)

//------ Define static class member vars ---------//

char    Vga::use_back_buf = 0;
char    Vga::opaque_flag  = 0;
VgaBuf* Vga::active_buf   = &vga_front;           // default: front buffer
// ##### begin Gilbert 26/04/2001 ######//
// static short * mouse_back_buffer_storage_bitmap;
// ##### end Gilbert 26/04/2001 ######//

char    low_video_memory_flag = 0;
extern "C" {
    short transparent_code_w;
}

//-------- Begin of function Vga::Vga ----------//

Vga::Vga() {
    memset( this, 0, sizeof(Vga) );
    // ##### begin Gilbert 26/04/2001 ######//
    // mouse_back_buffer_storage_bitmap=NULL;
    // ##### end Gilbert 26/04/2001 ######//
    vga_color_table = new ColorTable;
    dont_blt =0;                                    // ## chwg021999
}

//-------- End of function Vga::Vga ----------//

//-------- Begin of function Vga::~Vga ----------//

Vga::~Vga() {
    deinit();                                       // 1-is final

    delete vga_color_table;

    err_when( back_up_pal );                        // this must be free up immediately after its use
}

//-------- End of function Vga::~Vga ----------//

//-------- Begin of function Vga::init ----------//

BOOL Vga::init() {
    char* warnStr = "Warning: Due to the low memory of your display card, "
	"you may experience problems when you quit the game or "
	"switch tasks during the game. "
	"To avoid this problem, set your Windows display "
	"to 800x600 16-bit color mode before running the game.";

    //--------- Initialize DirectDraw object --------//

    if( !init_dd() )
	return FALSE;

    // get current display mode
    DDSURFACEDESC2 ddsd;
    DDSCAPS2  ddsCaps;
    DWORD    dwTotal;
    DWORD    dwFree;

    memset(&ddsd, 0, sizeof(ddsd) );
    ddsd.dwSize = sizeof(ddsd);
    ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;

    if( dd_obj->GetDisplayMode(&ddsd) == DD_OK &&
	dd_obj->GetAvailableVidMem(&ddsCaps, &dwTotal, &dwFree) == DD_OK ) {
	if( dwFree < (DWORD) VGA_WIDTH*VGA_HEIGHT*VGA_BPP/8 &&
	    !(ddsd.dwWidth==(DWORD)VGA_WIDTH && ddsd.dwHeight==(DWORD)VGA_HEIGHT && (ddsd.ddpfPixelFormat.dwRGBBitCount == (DWORD)VGA_BPP)) ) {
	    // not enough memory except same video mode

	    ShowCursor(TRUE);
	    // approximation of video memory required, actual video memory used should be calculated from vga_(true)_front->buf_pitch()

	    /*
	      extern char new_config_dat_flag;

	      if( new_config_dat_flag )
	      {
              MessageBox(sys.main_hwnd, warnStr,
	      WIN_TITLE, MB_OK | MB_ICONWARNING | MB_SETFOREGROUND );
	      }
	    */
	    low_video_memory_flag = 1;

	    ShowCursor(FALSE);
	}
    }

    if( !set_mode(VGA_WIDTH, VGA_HEIGHT) )
	return FALSE;

    return TRUE;
}

//-------- End of function Vga::init ----------//

//-------- Begin of function Vga::init_dd ----------//

BOOL Vga::init_dd() {
    if(dd_obj)                                      // the Direct Draw object has been initialized already
	return TRUE;

    //--------- Create direct draw object --------//

    DEBUG_LOG("Attempt DirectDrawCreate");
    LPDIRECTDRAW dd1Obj;
    int rc = DirectDrawCreate( NULL, &dd1Obj, NULL );
    DEBUG_LOG("DirectDrawCreate finish");

    if( rc != DD_OK ) {
#ifdef DEBUG
	debug_msg("DirectDrawCreate failed err=%d", rc);
#endif
	return FALSE;
    }

    //-------- Query DirectDraw2 interface --------//

    DEBUG_LOG("Attempt Query DirectDraw4");
    rc = dd1Obj->QueryInterface(IID_IDirectDraw4, (void **)&dd_obj);
    DEBUG_LOG("Query DirectDraw4 finish");
    if( rc != DD_OK ) {
#ifdef DEBUG
	debug_msg("Query DirectDraw4 failed err=%d", rc);
#endif
	dd1Obj->Release();
	return FALSE;
    }

    dd1Obj->Release();

    //-----------------------------------------------------------//
    // Convert it to a plain window
    //-----------------------------------------------------------//

    DWORD   dwStyle;
    dwStyle = GetWindowStyle(sys.main_hwnd);
    dwStyle |= WS_POPUP;
    dwStyle &= ~(WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX);
    SetWindowLong(sys.main_hwnd, GWL_STYLE, dwStyle);

    //-----------------------------------------------------------//
    // grab exclusive mode if we are going to run as fullscreen
    // otherwise grab normal mode.
    //-----------------------------------------------------------//

    DEBUG_LOG("Attempt DirectDraw SetCooperativeLevel");

    if( sys.use_true_front == 2 ) {                 // window mode
	rc = dd_obj->SetCooperativeLevel( sys.main_hwnd,
					  DDSCL_NORMAL );
    }
    else {
	rc = dd_obj->SetCooperativeLevel( sys.main_hwnd,
					  DDSCL_EXCLUSIVE |
					  DDSCL_FULLSCREEN );
    }

    DEBUG_LOG("DirectDraw SetCooperativeLevel finish");

    if( rc != DD_OK ) {
#ifdef DEBUG
	debug_msg("SetCooperativeLevel failed err=%d", rc);
#endif
	return FALSE;
    }

    return TRUE;
}

//-------- End of function Vga::init_dd ----------//

//-------- Begin of function Vga::set_mode ----------//

BOOL Vga::set_mode(int w, int h) {
    HRESULT rc;

    if( sys.use_true_front == 2 ) {                 // window mode
    }
    else {

	//-------------- set Direct Draw mode ---------------//

	DEBUG_LOG("Attempt DirectDraw SetDisplayMode");
	// IDirectDraw2::SetDisplayMode requires 5 parameters
	rc = dd_obj->SetDisplayMode( w, h, VGA_BPP, 0, 0);
	DEBUG_LOG("DirectDraw SetDisplayMode finish");

	if( rc != DD_OK ) {
#ifdef DEBUG
	    debug_msg("SetMode failed err=%d", rc);
#endif
	    return FALSE;
	}
    }

    //----------- get the pixel format flag -----------//

    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd) );
    ddsd.dwSize = sizeof(ddsd);

    pixel_format_flag = -1;

    if( dd_obj->GetDisplayMode(&ddsd) == DD_OK && ddsd.dwFlags & DDSD_PIXELFORMAT ) {
	if( ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB
	    && ddsd.ddpfPixelFormat.dwRGBBitCount == (DWORD)VGA_BPP ) {
	    if( ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL
		&& ddsd.ddpfPixelFormat.dwGBitMask == 0x001fL << 5
		&& ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL << 10 ) {
		pixel_format_flag = PIXFORM_RGB_555;
	    }
	    else if( ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL
		     && ddsd.ddpfPixelFormat.dwGBitMask == 0x003fL << 5
		     && ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL << 11 ) {
		pixel_format_flag = PIXFORM_RGB_565;
	    }
	    else if( ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL
		     && ddsd.ddpfPixelFormat.dwGBitMask == 0x001fL << 5
		     && ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL << 10 ) {
		pixel_format_flag = PIXFORM_BGR_555;
	    }
	    else if( ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL
		     && ddsd.ddpfPixelFormat.dwGBitMask == 0x003fL << 5
		     && ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL << 11 ) {
		pixel_format_flag = PIXFORM_BGR_565;
	    }
	}

	if( sys.use_true_front == 2 ) {               // window mode
	    if( ddsd.dwWidth < VGA_WIDTH || ddsd.dwHeight < VGA_HEIGHT ) {
		MessageBox( sys.main_hwnd, "Desktop resolution too small",
			    WIN_TITLE, MB_OK | MB_ICONWARNING | MB_SETFOREGROUND );
		return FALSE;
	    }
	}
    }

    if( pixel_format_flag == -1 ) {
	MessageBox(sys.main_hwnd, "Cannot determine the pixel format of this display mode.",
		   WIN_TITLE, MB_OK | MB_ICONWARNING | MB_SETFOREGROUND );

	if( sys.use_true_front == 2 ) {               // window mode
	    return FALSE;                               // require supported pixel format in window mode
	}

	pixel_format_flag = PIXFORM_BGR_565;
    }

    // assembly functions to initalize effect processing

    INITeffect(pixel_format_flag);
    INITbright(pixel_format_flag);

    //----------- display the system cursor -------------//

    SetCursor(NULL);

    return TRUE;
}

//-------- End of function Vga::set_mode ----------//

//-------- Begin of function Vga::deinit ----------//

void Vga::deinit() {
    // ##### begin Gilbert 26/04/2001 ######//
    // %% Begin Ho0909
    //	if(mouse_back_buffer_storage_bitmap)
    //	{	mem_del(mouse_back_buffer_storage_bitmap);
    //		mouse_back_buffer_storage_bitmap=NULL;
    //	}
    // %% End Ho0909
    // ##### end Gilbert 26/04/2001 ######//

    release_pal();

    if( dd_obj ) {
	//DEBUG_LOG("Attempt vga.dd_obj->RestoreDisplayMode()");
	// dd_obj->RestoreDisplayMode();
	//DEBUG_LOG("vga.dd_obj->RestoreDisplayMode() finish");

	DEBUG_LOG("Attempt vga.dd_obj->Release()");
	dd_obj->Release();
	DEBUG_LOG("vga.dd_obj->Release() finish");
	dd_obj = NULL;
    }

}

//-------- End of function Vga::deinit ----------//

//--------- Start of function Vga::load_pal ----------//
//
// Load the palette from a file and set it to the front buf.
//
BOOL Vga::load_pal(char* fileName) {
    char palBuf[256][3];
    File palFile;

    palFile.file_open(fileName);
    palFile.file_seek(8);                           // bypass the header info
    palFile.file_read(palBuf, 256*3);
    palFile.file_close();

    //--- Create a Direct Draw Palette and associate it with the front buffer ---//

    if( dd_pal == NULL ) {
	for(int i=0; i<256; i++) {
	    pal_entry_buf[i].peRed   = palBuf[i][0];
	    pal_entry_buf[i].peGreen = palBuf[i][1];
	    pal_entry_buf[i].peBlue  = palBuf[i][2];
	}

	HRESULT rc = dd_obj->CreatePalette( DDPCAPS_8BIT, pal_entry_buf, &dd_pal, NULL );

	if( rc != DD_OK )
	    return FALSE;
    }

    init_color_table();
    init_gray_remap_table();

    // set global variable
    transparent_code_w = translate_color(TRANSPARENT_CODE);

    return TRUE;
}

//----------- End of function Vga::load_pal ----------//

//--------- Start of function Vga::init_color_table ----------//

void Vga::init_color_table() {
    //----- initialize interface color table -----//

    PalDesc palDesc( (unsigned char*) pal_entry_buf, sizeof(PALETTEENTRY), 256, 8);
    vga_color_table->generate_table_fast( MAX_BRIGHTNESS_ADJUST_DEGREE, palDesc, ColorTable::bright_func );

    default_remap_table = (short *)vga_color_table->get_table(0);
}

//----------- End of function Vga::init_color_table ----------//

//--------- Start of function Vga::release_pal ----------//

void Vga::release_pal() {
    // ##### begin Gilbert 16/9 #######//
    if( dd_pal ) {
	while( dd_pal->Release() );
	dd_pal = NULL;
    }
    // ##### end Gilbert 16/9 #######//
}

//----------- End of function Vga::release_pal ----------//

//-------- Begin of function Vga::activate_pal ----------//
//!
//! we are getting the palette focus, select our palette
//!
void Vga::activate_pal(VgaBuf* vgaBufPtr) {
    vgaBufPtr->activate_pal(dd_pal);
}

//--------- End of function Vga::activate_pal ----------//

//-------- Begin of function Vga::adjust_brightness ----------//
//!
//! <int> changeValue - the value to add to the RGB values of
//!                     all the colors in the palette.
//!                     the value can be from -255 to 255.
//!
//! <int> preserveContrast - whether preserve the constrast or not
//!
void Vga::adjust_brightness(int changeValue) {
    //---- find out the maximum rgb value can change without affecting the contrast ---//

    int          i;
    int          newRed, newGreen, newBlue;
    PALETTEENTRY palBuf[256];

    //------------ change palette now -------------//

    for( i=0 ; i<256 ; i++ ) {
	newRed   = (int)pal_entry_buf[i].peRed   + changeValue;
	newGreen = (int)pal_entry_buf[i].peGreen + changeValue;
	newBlue  = (int)pal_entry_buf[i].peBlue  + changeValue;

	palBuf[i].peRed   = min(255, max(newRed,0) );
	palBuf[i].peGreen = min(255, max(newGreen,0) );
	palBuf[i].peBlue  = min(255, max(newBlue,0) );
    }

    //------------ set palette ------------//

    vga_front.temp_unlock();

    dd_pal->SetEntries( 0, 0, 256, palBuf );

    vga_front.temp_restore_lock();
}

//--------- End of function Vga::adjust_brightness ----------//

//--------- Begin of function Vga::blt_buf ----------//
//!
//! Blt the back buffer to the front buffer.
//!
//! <int> x1, y1, x2, y2 - the coordinations of the area to be blit
//! [int] putBackCursor  - whether put a mouse cursor onto the back buffer
//!                        before blitting.
//!                        (default: 0)
//!

BOOL Vga::blt_buf(int x1, int y1, int x2, int y2, int putBackCursor) {
    // ## Begin chwg 021999:: do nothing while using on-line help or pop-up news.
    if (dont_blt) return FALSE;
    if (sys.no_true_output_flag) return FALSE;
    // ## End chwg 021999

    int restoreCursor = 0;
    short mouseBackBufferStorageBitmap[2+64*64];    // size for 64x64 bitmap

    if( putBackCursor ) {
	// ##### begin Gilbert 24/04/2001 ######//
	// reduce save area here
	if( !mouse_cursor.hide_all_flag && mouse_cursor.cur_icon
	    && m.is_touch(x1,y1,x2,y2, mouse_cursor.cur_x1,mouse_cursor.cur_y1,mouse_cursor.cur_x2,mouse_cursor.cur_y2) ) {
	    mouse_cursor.hide_area_flag = 0;            // do not call mouse.hide_area() which will double paint the cursor

	    mouse_cursor.hide_x1 = x1;
	    mouse_cursor.hide_y1 = y1;
	    mouse_cursor.hide_x2 = x2;
	    mouse_cursor.hide_y2 = y2;

	    //-------- put mouse cursor ---------//
	    // ## Begin chwg 0909:: save area
	    //if(mouse_back_buffer_storage_bitmap)
	    //{	mem_del(mouse_back_buffer_storage_bitmap);
	    //mouse_back_buffer_storage_bitmap=NULL;
	    //}
	    // mouse_back_buffer_storage_bitmap=vga_back.save_area(x1, y1, x2, y2, mouse_back_buffer_storage_bitmap);
	    // backup the overlap area
	    err_when( BitmapW::size(min(x2,mouse_cursor.cur_x2)-max(x1,mouse_cursor.cur_x1)+1,min(y2,mouse_cursor.cur_y2)-max(y1,mouse_cursor.cur_y1)) > sizeof(mouseBackBufferStorageBitmap) );
	    vga_back.read_bitmapW( max(x1,mouse_cursor.cur_x1), max(y1,mouse_cursor.cur_y1),
				   min(x2,mouse_cursor.cur_x2), min(y2,mouse_cursor.cur_y2), mouseBackBufferStorageBitmap );
	    // ## End chwg 0909
	    mouse_cursor.disp_back_buf(x1, y1, x2, y2);
	    restoreCursor = 1;                          // rest_area mouse_back_buffer_storage_bitmap later
	}
	// ##### end Gilbert 24/04/2001 ######//
    }
    else {
	mouse.hide_area(x1, y1, x2, y2);
    }

    //--------------------------------------//
    // ###### begin Gilbert 12/9 #######//
    //	vga_front.blt_buf_fast( &vga_back, x1, y1, x2, y2 );
    // use directx blt function
    vga_front.temp_unlock();
    vga_back.temp_unlock();
    RECT rect = { x1, y1, x2+1, y2+1 };
    vga_front.dd_buf->BltFast( x1, y1, vga_back.dd_buf, &rect, DDBLTFAST_NOCOLORKEY );
    vga_back.temp_restore_lock();
    vga_front.temp_restore_lock();
    // ###### end Gilbert 12/10 #######//
    //--------------------------------------//

    if( putBackCursor ) {
	// ##### begin Gilbert 24/04/2001 #####//
	if( restoreCursor ) {
	    // %% Begin Ho 0909:: restore the damaged back buffer
	    // vga_back.rest_area(mouse_back_buffer_storage_bitmap,0,0);
	    // %%End Ho 0909
	    vga_back.put_bitmapW( max(x1,mouse_cursor.cur_x1), max(y1,mouse_cursor.cur_y1),
				  mouseBackBufferStorageBitmap );
	    mouse_cursor.hide_area_flag = 0;            // do not call mouse.show_area() which will double paint the cursor
	}
	// ##### end Gilbert 24/04/2001 #####//
    }
    else {
	mouse.show_area();
    }
    return TRUE;
}

//---------- End of function Vga::blt_buf ----------//

//----------- Begin of function Vga::d3_panel_up ------------//
//!
//! <int> x1,y1,x2,y2  = the four vertex of the panel
//! [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//!                      (default: 0)
//! [int] drawBorderOnly = draw border only, do not brighten the center area
//!                        (default: 0)
//!
void Vga::d3_panel_up(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly) {
    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    VgaBuf* vgaBuf;

    if( vgaFrontOnly )
	vgaBuf = &vga_front;
    else
	vgaBuf = &vga_back;

    if(sys.no_true_output_flag)
	vgaBuf = &vga_back;

    if( !drawBorderOnly ) {
	if( Vga::opaque_flag )
	    vgaBuf->bar(x1+1, y1+1, x2-1, y2-1, UP_OPAQUE_COLOR);
	else
	    vgaBuf->adjust_brightness(x1+1, y1+1, x2-1, y2-1, IF_UP_BRIGHTNESS_ADJUST);
    }

    mouse.hide_area( x1,y1,x2,y2 );

    //--------- white border on top and left sides -----------//

    // top side
    vgaBuf->bar_fast( x1+1,y1,x2,y1, IF_LIGHT_BORDER_COLOR );
    // left side
    vgaBuf->bar_fast( x1,y1,x1,y2  , IF_LIGHT_BORDER_COLOR );

    //--------- black border on bottom and right sides -----------//

    // bottom side
    vgaBuf->bar_fast( x1+1,y2,x2,y2, IF_DARK_BORDER_COLOR );
    // right side
    vgaBuf->bar_fast( x2,y1+1,x2,y2, IF_DARK_BORDER_COLOR );

    //-------------------------------------------//

    mouse.show_area();

    //----- blt the area from the back buffer to the front buffer ------//

    if( !vgaFrontOnly && !use_back_buf )            // only blt the back to the front is the active buffer is the front
	vga.blt_buf(x1, y1, x2, y2, 0);
}

//------------- End of function Vga::d3_panel_up ------------//

//----------- Begin of function Vga::d3_panel_down ------------//
//!
//! <int> x1,y1,x2,y2  = the four vertex of the panel
//! [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//!                      (default: 0)
//! [int] drawBorderOnly = draw border only, do not brighten the center area
//!                        (default: 0)
//!
void Vga::d3_panel_down(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly) {
    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    VgaBuf* vgaBuf;

    if( vgaFrontOnly )
	vgaBuf = &vga_front;
    else
	vgaBuf = &vga_back;

    if(sys.no_true_output_flag)
	vgaBuf = &vga_back;

    if( !drawBorderOnly ) {
	if( Vga::opaque_flag )
	    vgaBuf->bar(x1+1, y1+1, x2-1, y2-1, DOWN_OPAQUE_COLOR);
	else
	    vgaBuf->adjust_brightness(x1+1, y1+1, x2-1, y2-1, IF_DOWN_BRIGHTNESS_ADJUST);
    }

    mouse.hide_area( x1,y1,x2,y2 );

    //--------- white border on top and left sides -----------//

    // top side
    vgaBuf->bar_fast( x1+1,y1,x2,y1, IF_DARK_BORDER_COLOR );
    // left side
    vgaBuf->bar_fast( x1,y1,x1,y2  , IF_DARK_BORDER_COLOR );

    //--------- black border on bottom and right sides -----------//

    // bottom side
    vgaBuf->bar_fast( x1+1,y2,x2,y2, IF_LIGHT_BORDER_COLOR );
    // right side
    vgaBuf->bar_fast( x2,y1+1,x2,y2, IF_LIGHT_BORDER_COLOR );

    //-------------------------------------------//

    mouse.show_area();

    //----- blt the area from the back buffer to the front buffer ------//

    if( !vgaFrontOnly && !use_back_buf )            // only blt the back to the front is the active buffer is the front
	vga.blt_buf(x1, y1, x2, y2, 0);
}

//------------- End of function Vga::d3_panel_down ------------//

//----------- Begin of function Vga::d3_panel2_up ------------//
//!
//! <int> x1,y1,x2,y2  = the four vertex of the panel
//! [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//!                      (default: 0)
//! [int] drawBorderOnly = draw border only, do not brighten the center area
//!                        (default: 0)
//!
void Vga::d3_panel2_up(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly) {
    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    VgaBuf* vgaBuf;

    if( vgaFrontOnly )
	vgaBuf = &vga_front;
    else
	vgaBuf = &vga_back;

    if(sys.no_true_output_flag)
	vgaBuf = &vga_back;

    if( !drawBorderOnly )
	vgaBuf->adjust_brightness(x1+2, y1+2, x2-3, y2-3, IF_UP_BRIGHTNESS_ADJUST);

    mouse.hide_area( x1,y1,x2,y2 );

    //--------- white border on top and left sides -----------//

    vgaBuf->bar_fast( x1,y1,x2-3,y1+1,0x9a );
    vgaBuf->draw_pixel(x2-2, y1, 0x9a);
    vgaBuf->bar_fast( x1,y1+2,x1+1,y2-3, 0x9a );    // left side
    vgaBuf->draw_pixel(x1, y2-2, 0x9a);

    //--------- black border on bottom and right sides -----------//

    vgaBuf->bar_fast( x2-2,y1+2,x2-1,y2-1, 0x90 );  // bottom side
    vgaBuf->draw_pixel(x2-1, y1+1, 0x90);
    vgaBuf->bar_fast( x1+2,y2-2,x2-3,y2-1, 0x90 );  // right side
    vgaBuf->draw_pixel(x1+1, y2-1, 0x90);

    //--------- junction between white and black border --------//
    vgaBuf->draw_pixel(x2-1, y1, 0x97);
    vgaBuf->draw_pixel(x2-2, y1+1, 0x97);
    vgaBuf->draw_pixel(x1, y2-1, 0x97);
    vgaBuf->draw_pixel(x1+1, y2-2, 0x97);

    //--------- gray shadow on bottom and right sides -----------//
    vgaBuf->bar_fast( x2, y1+1, x2, y2, 0x97);
    vgaBuf->bar_fast( x1+1, y2, x2-1, y2, 0x97);

    //-------------------------------------------//

    mouse.show_area();

    //----- blt the area from the back buffer to the front buffer ------//

    if( !vgaFrontOnly && !use_back_buf )            // only blt the back to the front is the active buffer is the front
	vga.blt_buf(x1, y1, x2, y2, 0);
}

//------------- End of function Vga::d3_panel_up ------------//

//----------- Begin of function Vga::d3_panel2_down ------------//
//!
//! <int> x1,y1,x2,y2  = the four vertex of the panel
//! [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//!                      (default: 0)
//! [int] drawBorderOnly = draw border only, do not brighten the center area
//!                        (default: 0)
//!
void Vga::d3_panel2_down(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly) {
    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    VgaBuf* vgaBuf;

    if( vgaFrontOnly )
	vgaBuf = &vga_front;
    else
	vgaBuf = &vga_back;

    if(sys.no_true_output_flag)
	vgaBuf = &vga_back;

    if( !drawBorderOnly )
	vgaBuf->adjust_brightness(x1+2, y1+2, x2-3, y2-3, IF_DOWN_BRIGHTNESS_ADJUST);

    mouse.hide_area( x1,y1,x2,y2 );

    //--------- black border on top and left sides -----------//

    vgaBuf->bar_fast( x1,y1,x2-3,y1+1,0x90 );
    vgaBuf->draw_pixel(x2-2, y1, 0x90);
    vgaBuf->bar_fast( x1,y1+2,x1+1,y2-3, 0x90 );    // left side
    vgaBuf->draw_pixel(x1, y2-2, 0x90);

    //--------- while border on bottom and right sides -----------//

    vgaBuf->bar_fast( x2-2,y1+2,x2-1,y2-1, 0x9a );  // bottom side
    vgaBuf->draw_pixel(x2-1, y1+1, 0x9a);
    vgaBuf->bar_fast( x1+2,y2-2,x2-3,y2-1, 0x9a );  // right side
    vgaBuf->draw_pixel(x1+1, y2-1, 0x9a);

    //--------- junction between white and black border --------//
    vgaBuf->draw_pixel(x2-1, y1, 0x97);
    vgaBuf->draw_pixel(x2-2, y1+1, 0x97);
    vgaBuf->draw_pixel(x1, y2-1, 0x97);
    vgaBuf->draw_pixel(x1+1, y2-2, 0x97);

    //--------- remove shadow, copy from back  -----------//
    vgaBuf->bar_fast( x2, y1+1, x2, y2, 0x9c);
    vgaBuf->bar_fast( x1+1, y2, x2-1, y2, 0x9c);

    mouse.show_area();

    //----- blt the area from the back buffer to the front buffer ------//

    if( !vgaFrontOnly && !use_back_buf )            // only blt the back to the front is the active buffer is the front
	vga.blt_buf(x1, y1, x2, y2, 0);
}

//------------- End of function Vga::d3_panel2_down ------------//

//------------- Start of function Vga::separator --------------//
//
// Draw a VGA separator line
//
// Syntax : separator( x1, y1, x2, y2 )
//
// int x1,y1       - the top left vertex of the separator
// int x2,y2       - the bottom right vertex of the separator
//
void Vga::separator(int x1, int y1, int x2, int y2) {
    err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

    if( y1+1==y2 ) {                                // horizontal line
	vga_front.adjust_brightness(x1, y1, x2, y1, IF_UP_BRIGHTNESS_ADJUST);
	vga_front.adjust_brightness(x1, y2, x2, y2, IF_DOWN_BRIGHTNESS_ADJUST);
    }
    else {
	vga_front.adjust_brightness(x1, y1, x1, y2, IF_UP_BRIGHTNESS_ADJUST);
	vga_front.adjust_brightness(x2, y1, x2, y2, IF_DOWN_BRIGHTNESS_ADJUST);
    }
}

//--------------- End of function Vga::separator --------------//

//----------- Begin of function Vga::init_gray_remap_table ----------//
//!
//! Initialize a gray remap table for VgaBuf::convert_gray to use.
//!
void Vga::init_gray_remap_table() {
    //------ create a color to gray-scale remap table ------//

#define FIRST_GRAY_COLOR   0x90
#define GRAY_SCALE_COUNT   16                   // no. of gray colors

    // #define FIRST_GRAY_COLOR   0x96
    // #define GRAY_SCALE_COUNT   10    // no. of gray colors

    PALETTEENTRY* palEntry = vga.pal_entry_buf;
    int i, grayIndex;

    for( i=0 ; i<256 ; i++, palEntry++ ) {
	//--------------------------------------------------------//
	//
	// How to calculate the gray index (0-31)
	//
	// formula is : grey = red * 0.3 + green * 0.59 + blue * 0.11
	//              the range of the result value is 0-255
	//              this value is then divided by 8 to 0-31
	//
	//--------------------------------------------------------//

	grayIndex = ((int)palEntry->peRed * 30 + (int)palEntry->peGreen * 59 +
		     (int)palEntry->peBlue * 11) / 100 / (256/GRAY_SCALE_COUNT);

	gray_remap_table[i] = FIRST_GRAY_COLOR + grayIndex;
    }
}

//--------- End of function Vga::init_gray_remap_table -----------//

int Vga::make_pixel(BYTE red, BYTE green, BYTE blue) {
    return IMGmakePixel((blue << 16) + (green << 8) + red);
}

int Vga::make_pixel(RGBColor *rgb) {
    int u;
    memcpy(&u, rgb, sizeof(RGBColor));
    return IMGmakePixel(u);
}

void Vga::decode_pixel(int p, RGBColor *rgb) {
    int u = IMGdecodePixel(p);
    memcpy(rgb, &u, sizeof(RGBColor));
}
