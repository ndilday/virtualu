//Filename   : OSYSMESS.CPP
//Description: Sys class functions for processing window messages

#include <OSYS.H>

//-------- Begin of function Sys::main_win_proc --------//
//! called by static_main_win_proc in sysinit.cpp
//!
long Sys::main_win_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch( message ) {
    case WM_CREATE:
	main_hwnd = hWnd;
	break;

    case WM_ACTIVATEAPP:
	active_flag = (BOOL)wParam && !IsIconic(hWnd);

	//--------------------------------------------------------------//
	// while we were not-active something bad happened that caused us
	// to pause, like a surface restore failing or we got a palette
	// changed, now that we are active try to fix things
	//--------------------------------------------------------------//

	if( active_flag ) {
	    unpause();
	    redraw_all_flag = 1;                      // for Sys::disp_frame to redraw the screen
	}
	else
	    pause();
	break;

    case WM_PAINT:
	redraw_all_flag = 1;
	break;

	//### begin zb 02/24/99
    case WM_CLOSE:                                // ALT+F4 ???
	//			deinit_directx();

	//### fred	2904
	//main_hwnd = NULL;
	//PostQuitMessage( 0 );
	//### fred	2904
	//			break;
	return 0;

    case WM_DESTROY:
	main_hwnd = NULL;
	//			 deinit_directx();
	PostQuitMessage( 0 );
	//			 break;
	return 0;
	//### end zb 02/24/99

    default:
	break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

//--------- End of function Sys::main_win_proc ---------//
