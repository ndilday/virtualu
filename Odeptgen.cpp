//Filename    : ODEPT.H
//Description : Department Class Definition
//Owner       : Fred

#include <ALL.H>
#include <ODEPT.H>
#include <ODEPTRES.H>
#include <ODEPTGEN.H>

//----------- Begin of function GeneralDepartment Constructor -----//
//!
GeneralDepartment::GeneralDepartment() {
    init();
}

//------------- End of function GeneralDepartment Constructor -----//

//----------- Begin of function GeneralDepartment init -----//
//!
void GeneralDepartment::init() {
    //don't do this!!!:	memset(this, 0, sizeof(GeneralDepartment));
    err_when(student_array.student_count != 0);
    // fix in version 2
    student_array.zap();                            // clear empty_room_array in last game

    //
    department_id = 0;

    //
    memset(project_array, 0, sizeof(project_array));
    project_count = 0;
    project_added_this_year = false;

}

//------------- End of function GeneralDepartment init -----//

//----------- Begin of function GeneralDepartment next_day -----//
//!
void GeneralDepartment::next_day() {
    student_array.next_day();
}

//------------- End of function GeneralDepartment next_day -----//

//----------- Begin of function GeneralDepartment next_month_construction -----//
//!
//! <newSF>		new square foot of completed projects
//! <expense>	expense of completed projects
//!
void GeneralDepartment::next_month_construction(int &newSF, int &expense) {
    newSF = 0;
    expense = 0;

    for( int i=project_count-1 ; i>=0 ; i-- ) {
	if( --(project_array[i].month_left) <= 0 ) {
	    newSF += project_array[i].new_sf;
	    expense += project_array[i].expense;

	    m.del_array_rec(project_array, project_count, sizeof(project_array[0]), i+1 );
	    project_count--;
	}
    }
}

//------------- End of function GeneralDepartment next_month_construction -----//

//----------- Begin of function GeneralDepartment:add_construction -----//
//!
//! The general department can also have construction, it represents
//! all constructions of all non-departmental buildings (including
//! admission office, development office, and etc.)
//!
//! <newSF>		new square foot of added project
//! <expense>	expense of added project
//!
void GeneralDepartment::add_construction(int newSF, int expense) {
    err_when( newSF <= 0 || expense <=0 );

    project_array[project_count].new_sf  = newSF;
    project_array[project_count].expense = expense;
    project_array[project_count].month_left = 12;

    project_count++;
    err_when(project_count > MAX_PROJECT_COUNT);

    project_added_this_year = true;
}

//------------- End of function GeneralDepartment:add_construction -----//

//----------- Begin of function GeneralDepartment:next_month_construction -----//
//!
//! empty function
//!
void GeneralDepartment::confirm_construction_yearly() {
    //TO sort project_array by month_left (incrementaly)
    // IF genDeptInfo->months_to_construct will change

    // project_added_this_year = false;
}

//------------- End of function GeneralDepartment:next_month_construction -----//

//----------- Begin of function GeneralDepartment:next_month_construction -----//
//!
int GeneralDepartment::get_constructing_sf() {
    int total = 0;

    for ( int i=0; i<project_count; i++) {
	total += project_array[i].new_sf;
    }

    return total;
}

//------------- End of function GeneralDepartment:next_month_construction -----//

//----- Begin of function GeneralDepartment::get_course_requirement ----//
//!
//! Get the number of required courses of the specific department that
//! a student majoring in this department has to take.
//!
//! <int> studentLevel 	 - the student level
//! <int> targetDeptRecno - the target department of the course that the student may take
//! <int> courseDepth 	 - the course depth of the course
//!
int GeneralDepartment::get_course_requirement(int studentLevel, int targetDeptRecno, int courseDepth) {
    DepartmentInfo* thisDeptInfo = department_res[department_id];

    int targetDeptId  = department_array[targetDeptRecno]->department_id;

    //## chea 291199 added to fix 2.1.1
    int targetFieldId=0;

    if( targetDeptId > 0 )
	targetFieldId = department_res[targetDeptId]->field_id;

    if( targetDeptId == department_id )             // if the target department is this departemnt, set targetFieldId to 0, as 0 means own department
	targetFieldId = 0;

    //-- if targetDeptId is 0, the first element in the depth array will be used. for instance, bachelor_depth1[0] will be used if the department is a general education department

    switch( studentLevel ) {
    case UG_TRADITION:
    case UG_NONTRADITION:
    case DISTANCE_LEARN: {
	switch(courseDepth) {
        case DEPTH_D1:
	    // [0] is own department
	    return thisDeptInfo->bachelor_depth1[targetFieldId];

        case DEPTH_D2:
	    return thisDeptInfo->bachelor_depth2[targetFieldId];

        case DEPTH_D3:
	    return thisDeptInfo->bachelor_depth3[targetFieldId];
	}
	break;
    }

    case MASTER: {
	switch(courseDepth) {
        case DEPTH_D3:
	    return thisDeptInfo->master_depth3[targetFieldId];

        case DEPTH_GR:
	    return thisDeptInfo->master_graduate[targetFieldId];
	}
	break;
    }

    case DOCTOR:
	return thisDeptInfo->doctor_graduate[targetFieldId];
	break;
    }

    err_here();
    return 0;
}

//------ End of function GeneralDepartment::get_course_requirement -----//
