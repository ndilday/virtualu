//Filename    : OGCompon.h
//Description : GComponent Class Declaration  - Graphical Component
//					 As base class for all GUI Componenet

#ifndef __OGCompon_H
#define __OGCompon_H

// ES library header file
// CU header file
#include <ORect.h>

//----------- Define constants -------------//
//----------- Define enum types -------------//
//----------- Define struct -------------//

//----------- Define class GComponent -----//
class VgaBuf;
//!
class GComponent {
protected:
    enum {                                        // temp for axoview
	MOUSE_LEFT=0, MOUSE_RIGHT
    };

    // this component's coordinate (w.r.t. this's parent component which should be whole game window);  const
    //
    Rect  clip;

protected:
    bool  is_active;                              //temp no use
public:
    bool  is_shown;                               // is this shown or hidden

    //------------------------------------------//

public:
    void  repaint(VgaBuf *vgaBuf);
    void  on_event();
    bool  is_inside(int x, int y);                // wrt this->clip

    //------------------------------------------//

protected:
    GComponent();
    ~GComponent();
    void  init();
    void  deinit();
    void  set_bounds(int x1, int y1, int x2, int y2);

    //------------------------------------------//

    virtual void paint(VgaBuf *vgaBuf){};
    // for on_event
    virtual void on_key(unsigned scanCode, unsigned skeyState) {
    };
    // for on_event
    virtual void on_mouse_click(int x, int y, char button=MOUSE_LEFT) {
    };
};
#endif                                            //ifndef
