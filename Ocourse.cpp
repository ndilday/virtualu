//Filename    : OCOURSE.CPP
//Description : Course Class Definition

#include <OPSCHOOL.H>
#include <OCOURSE.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OCOURRES.H>

//------ Begin of function Course::normal_class_size -------//

int Course::normal_class_size() {
    int deptId = department_array[department_recno]->department_id;

    if ( teaching_method == DISTANCE_LEARN_COURSE )
	return department_res[deptId]->max_dl_class_size;
    else
	return course_res.get_normal_class_size(department_res[deptId]->template_course_type, teaching_method);
}

//------ End function Course::normal_class_size -------//

//------ Begin of function Course::next_trimester -------//

void Course::next_trimester() {
    reset_vars();
}

//------ End function Course::next_trimester -------//

//------ Begin of function Course::reset_vars -------//

void Course::reset_vars() {
    is_closed = 0;
    convention_student_count = 0;
    //convention_student_distance_learner_count = 0;
    distance_student_count   = 0;

    admission_denied_count   = 0;
    failure_count        = 0;

    outsourcing_credits    = 0;

    class_section_count      = 0;
    cur_section_student_count= 0;
}

//------ End function Course::reset_vars -------//
