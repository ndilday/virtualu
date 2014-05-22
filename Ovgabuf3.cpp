
//----------- Begin of function VgaBuf::d3_panel_up ------------//
//!
//! Draw a Ms windows style 3D panel
//!
//! thickness can be 1-4
//!
//! <int> x1,y1,x2,y2 = the four vertex of the panel
//! [int] thick       = thickness of the border
//!                     (default:2)
//! [int] paintCentre = paint the centre area of the plane or not
//!                     pass 0 if the area has just been painted with bar()
//!                     (default:1)
//! [char*] tileName  = name of the texture tile if this panel should
//!							  be filled with tile texture (default: NULL)
//!
void VgaBuf::d3_panel_up(int x1,int y1,int x2,int y2,int t,int paintCentre, char* tileName) {
    int i,x,y;

    if( !image_init_flag ) {
	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	if( d3_has_shadow )
	    mouse.hide_area( max(0,x1-VGA_SHADOW_WIDTH), y1, x2, min(VGA_HEIGHT-1,y2+VGA_SHADOW_HEIGHT) );
	else
	    mouse.hide_area( x1,y1,x2,y2 );
    }
    else {
	err_if( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=image_width|| y2>=image_height )
	    err_here();
    }

    //------------------------------------------------//

    if( d3_has_border ) {
	if( paintCentre ) {
	    if( tileName )
		tile( x1+t+1, y1+t+1, x2-t-1, y2-t-1, tileName);
	    else
		// centre
		(*vga_bar_fp)( x1+t+1, y1+t+1, x2-t-1, y2-t-1, color_up );
	}

	for( i=t, y=y1+t ; y>y1 ; y--,i-- )
	    // top side
	    (*vga_bar_fp)( x1+i,y,x2-i,y, i==0?V_WHITE:color_light );

	for( i=t, x=x1+t ; x>x1 ; x--,i-- )           // right side
	    // left side
	    (*vga_bar_fp)( x,y1+i,x,y2-i, i==0?V_WHITE:color_light );

	for( i=t, y=y2-t ; y<y2 ; y++,i-- )
	    // bottom side
	    (*vga_bar_fp)( x1+i,y,x2-i,y, i==0?color_dark+2:color_dark-2 );

	for( i=t, x=x2-t ; x<x2 ; x++,i-- )           // right side
	    (*vga_bar_fp)( x,y1+i,x,y2-i, i==0?color_dark+2:color_dark-2 );

	(*vga_bar_fp)( x1,y1,x2,y1, color_border );   // border
	(*vga_bar_fp)( x1,y2,x2,y2, color_border );
	(*vga_bar_fp)( x1,y1,x1,y2, color_border );
	(*vga_bar_fp)( x2,y1,x2,y2, color_border );
    }
    else {
	if( paintCentre ) {
	    if( tileName )
		tile( x1+t, y1+t, x2-t, y2-t, tileName);
	    else
		// centre
		(*vga_bar_fp)( x1+t, y1+t, x2-t, y2-t, color_up );
	}

	for( i=t-1, y=y1+t-1 ; y>=y1 ; y--,i-- )
	    // top side
	    (*vga_bar_fp)( x1+i,y,x2-i,y, i==0?V_WHITE:color_light );

	for( i=t-1, x=x1+t-1 ; x>=x1 ; x--,i-- )
	    // left side
	    (*vga_bar_fp)( x,y1+i,x,y2-i, i==0?V_WHITE:color_light );

	for( i=t-1, y=y2-t+1 ; y<=y2 ; y++,i-- )
	    // bottom side
	    (*vga_bar_fp)( x1+i,y,x2-i,y, i==0?color_dark+2:color_dark-2 );

	for( i=t-1, x=x2-t+1 ; x<=x2 ; x++,i-- )      // right side
	    (*vga_bar_fp)( x,y1+i,x,y2-i, i==0?color_dark+2:color_dark-2 );
    }

    //-------- Display shadow ------------//

    if( d3_has_shadow && !image_init_flag ) {
	if( x1>0 )                                    // left vertical shadow
	    darken( max(0,x1-VGA_SHADOW_WIDTH), y1+VGA_SHADOW_HEIGHT, x1-1, y2, VGA_SHADOW_DEGREE );

	if( y2 < VGA_HEIGHT-1 )                       // bottom horizontal shadow
	    darken( max(0,x1-VGA_SHADOW_WIDTH), y2+1, x2-VGA_SHADOW_WIDTH, min(VGA_HEIGHT-1, y2+VGA_SHADOW_HEIGHT), VGA_SHADOW_DEGREE );
    }

    //-------------------------------------------//

    if( !image_init_flag )
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
//! [int] paintCentre = paint the centre area of the plane or not
//!                     pass 0 if the area has just been painted with bar()
//!                     (default:1)
//! [char*] tileName  = name of the texture tile if this panel should
//!							  be filled with tile texture (default:NULL)
//!
void VgaBuf::d3_panel_down(int x1,int y1,int x2,int y2,int t,int paintCentre, char* tileName) {
    int i,x,y;

    if( !image_init_flag ) {
	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	mouse.hide_area( x1,y1,x2,y2 );               // if the mouse cursor is in that area, hide it
    }
    else
	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=image_width|| y2>=image_height );

    //---------- main centre area -----------//

    if( paintCentre ) {
	if( d3_has_border ) {
	    if( tileName )
		tile( x1+t+1, y1+t+1, x2-t-1, y2-t-1, tileName);
	    else
		// centre
		(*vga_bar_fp)( x1+t+1, y1+t+1, x2-t-1, y2-t-1, color_down );
	}
	else {
	    if( tileName )
		tile( x1+t, y1+t, x2-t, y2-t, tileName );
	    else
		(*vga_bar_fp)( x1+t, y1+t, x2-t, y2-t, color_down );
	}
    }

    //----------- panel sides -----------//

    if( t == 2 ) {                                  // when thickness==2, use four different color for the down panel
	// outer lines

	// top
	(*vga_bar_fp)( x1,y1,x2-1,y1,color_scheme+8  );
	// bottom
	(*vga_bar_fp)( x1+1,y2,x2,y2,V_WHITE         );
	// left
	(*vga_bar_fp)( x1,y1,x1,y2-1,color_scheme+8  );
	// right
	(*vga_bar_fp)( x2,y1+1,x2,y2,V_WHITE         );

	// inner lines

	// bottom
	(*vga_bar_fp)( x1+1,y1+1,x2-1,y1+1,color_scheme+15 );
	// top
	(*vga_bar_fp)( x1+1,y2-1,x2-1,y2-1,color_scheme+1  );
	// left
	(*vga_bar_fp)( x1+1,y1+1,x1+1,y2-1,color_scheme+15 );
	// right
	(*vga_bar_fp)( x2-1,y1+1,x2-1,y2-1,color_scheme+1  );
    }
    else {                                          // one unit thickness
	for( i=t-1, y=y2-t+1 ; y<=y2 ; y++,i-- )
	    // bottom
	    (*vga_bar_fp)( x1+i,y,x2-i,y,i==0?V_WHITE:color_light );

	for( i=t-1, x=x2-t+1 ; x<=x2 ; x++,i-- )      // left
	    // right
	    (*vga_bar_fp)( x,y1+i,x2,y2-i,i==0?V_WHITE:color_light );

	for( i=t-1, y=y1+t-1 ; y>=y1 ; y--,i-- )      // top
	    (*vga_bar_fp)( x1+i,y,x2-i,y,i==0?color_dark+2:color_dark-2 );

	for( i=t-1, x=x1+t-1 ; x>=x1 ; x--,i-- )      // left
	    (*vga_bar_fp)( x,y1+i,x,y2-i,i==0?color_dark+2:color_dark-2 );
    }

    //---------- dark border ------------//

    if( d3_has_border ) {
	x1+=t;
	x2-=t;
	y1+=t;
	y2-=t;

	(*vga_bar_fp)( x1,y1,x2,y1, color_border );
	(*vga_bar_fp)( x1,y2,x2,y2, color_border );
	(*vga_bar_fp)( x1,y1,x1,y2, color_border );
	(*vga_bar_fp)( x2,y1,x2,y2, color_border );
    }

    if( !image_init_flag )
	mouse.show_area();
}

//------------- End of function VgaBuf::d3_panel_down ------------//

//----------- Begin of function VgaBuf::d3_panel_up_clear ------------//
//!
//! clear the centre of the either up or down panel
//!
//! <int> x1,y1,x2,y2 = the four vertex of the panel
//! [int] thick       = thickness of the border
//!                     (default:2)
//! [char*] tileName  = name of the texture tile if this panel should
//!							  be filled with tile texture (default:NULL)
//!
void VgaBuf::d3_panel_up_clear(int x1,int y1,int x2,int y2,int t, char* tileName) {
    t += d3_has_border;

    if( tileName )
	tile( x1+t, y1+t, x2-t, y2-t, tileName );
    else
	bar( x1+t, y1+t, x2-t, y2-t, color_up );
}

//------------- End of function VgaBuf::d3_panel_up_clear ------------//

//----------- Begin of function VgaBuf::d3_panel_down_clear ------------//
//!
//! clear the centre of the either up or down panel
//!
//! <int> x1,y1,x2,y2 = the four vertex of the panel
//! [int] thick       = thickness of the border
//!                     (default:2)
//! [char*] tileName  = name of the texture tile if this panel should
//!							  be filled with tile texture (default:NULL)
//!
void VgaBuf::d3_panel_down_clear(int x1,int y1,int x2,int y2,int t, char* tileName) {
    t += d3_has_border;

    if( tileName )
	tile( x1+t, y1+t, x2-t, y2-t, tileName );
    else
	bar( x1+t, y1+t, x2-t, y2-t, color_down );
}

//------------- End of function VgaBuf::d3_panel_down_clear ------------//
