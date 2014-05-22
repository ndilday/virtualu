//Filename    : OPIE.H
//Description : Object Pie Chart

#ifndef __OPIE_H
#define __OPIE_H

#ifndef __ALL_H
#include <ALL.H>
#endif

enum { MAX_DEGREE = 360 };

//-------------------------------------//

//!Class for calculating and drawing a pie chart.
class PieChart {
private:
    //!
    struct PiePoint {
	int x, y;
    };

    PiePoint pie_point_table[MAX_DEGREE];         // points of the pie, 0-360, total 361 points

    char* slot_icon_ptr;

    int   pie_radius;
    int   box_width;

    float last_degree;                            // degree of the last drawn slice

public:
    void init(char* =NULL);

    void init_image(int= -1);
    void put_image(int,int);
    void draw_slice(float,int);
};

extern PieChart pie_chart;

//-------------------------------------//
#endif
