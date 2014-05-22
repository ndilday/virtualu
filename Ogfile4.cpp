//Filename    : OGFILE4.CPP
//Description : Object Game file, save game and restore game, part 3.2

//=================================================================================//
// for write_file_3

#include <ODEPT.H>

//=================================================================================//
#include <OGFILE.H>

//=================================================================================//
//============================ section 3/3 ========================================//
//=================================================================================//

//------------------------------------
// continue section 3/3 in ogfile3.cpp

//--------- Begin of function DepartmentArray::write_file ---------------//
//!
int DepartmentArray::write_file(File* filePtr) {
    //------ write info in DepartmentArray ------//

    if( !filePtr->file_write( (char*) this + sizeof(DynArrayB), sizeof(DepartmentArray)-sizeof(DynArrayB) ) )
	return 0;

    err_when(department_count == 0);

    //---------- write Departments --------------//

    int    i;
    Department *deptPtr;

    int deptSize = size();

    filePtr->file_put_short( deptSize );            // no. of nations in nation_array

    for( i=1; i<=deptSize; i++) {
	deptPtr = (Department*) get_ptr(i);

	//----- write nationId or 0 if the nation is deleted -----//

	if( !deptPtr ) {                              // the object is deleted
	    filePtr->file_put_short(0);
	}
	else {
	    filePtr->file_put_short(1);                 // there is a object in this record

	    //------ write data in the base class ------//

	    if( !deptPtr->write_file(filePtr) )
		return 0;

	    err_when(i != deptPtr->department_recno);   // assume no object deleted
	}
    }

    //------- write empty room array --------//

    write_empty_room(filePtr);

    return 1;
}

int DepartmentArray::read_file(File* filePtr) {
    if( !filePtr->file_read( (char*) this + sizeof(DynArrayB), sizeof(DepartmentArray)-sizeof(DynArrayB) ) )
	return 0;

    err_when(department_count == 0);

    //---------- read Depts --------------//

    int     i, deptRecno, deptSize;
    Department* deptPtr;

    deptSize = filePtr->file_get_short();           // get no. of depts from file

    // err_when(deptCount != department_count);

    for( i=1; i<=deptSize; i++) {
	if( filePtr->file_get_short() == 0 ) {
	    add_blank(1);                               // it's a DynArrayB function
	}
	else {
	    //----- create nation object -----------//

	    deptRecno = create_dept();

	    deptPtr   = department_array[deptRecno];

	    //----- read data in base class --------//

	    if( !deptPtr->read_file( filePtr ) )
		return 0;

	    deptPtr->create_department_firm();          // in order to call create_department_firm(); since we don't save firm_array

	    err_when(deptRecno != deptPtr->department_recno);
	}
    }

    //-------- linkout() those record added by add_blank() ----------//
    //-- So they will be marked deleted in DynArrayB and can be -----//
    //-- undeleted and used when a new record is going to be added --//

    for( i=size() ; i>0 ; i-- ) {
	DynArrayB::go(i);                             // since NationArray has its own go() which will call GroupArray::go()

	if( get_ptr() == NULL )                       // add_blank() record
	    linkout();
    }

    //------- read empty room array --------//

    read_empty_room(filePtr);

    err_when(packed_size() != department_count || department_count == 0);
    return 1;
}

//--------- End of function DepartmentArray::write_file ---------------//

//--------- Begin of function Department::write_file ---------------//
//!
int Department::write_file(File* filePtr) {
    first_write_offset = 1;

    if ( !GeneralDepartment::write_file(filePtr) )
	return 0;

    //---------------------//

    if ( !faculty_array.write_file(filePtr) )
	return 0;

    if ( !course_array.write_file(filePtr) )
	return 0;

    if ( !display_faculty_array.write_file(filePtr) )
	return 0;

    if ( !cur_faculty_array.write_file(filePtr) )
	return 0;

    //---------------------//

    int writeSize = sizeof(Department) - ((char*)(&first_write_offset) - (char*)(this));

    return filePtr->file_write( (&first_write_offset), writeSize);
}

int Department::read_file(File* filePtr) {
    first_write_offset = 0;

    if ( !GeneralDepartment::read_file(filePtr) )
	return 0;

    //---------------------//

    if ( !faculty_array.read_file(filePtr) )
	return 0;

    if ( !course_array.read_file(filePtr) )
	return 0;

    if ( !display_faculty_array.read_file(filePtr) )
	return 0;

    if ( !cur_faculty_array.read_file(filePtr) )
	return 0;

    //---------------------//

    int readSize = sizeof(Department) - ((char*)(&first_write_offset) - (char*)(this));

    if (! filePtr->file_read( (&first_write_offset), readSize) )
	return 0;

    err_when(first_write_offset != 1);

    return 1;
}

//--------- End of function Department::write_file ---------------//

//--------- Begin of function GeneralDepartment::write_file ---------------//
//!
int GeneralDepartment::write_file(File* filePtr) {
    //---------------------//

    if ( !student_array.write_file(filePtr) )
	return 0;

    //---------------------//

    int writeSize = sizeof(GeneralDepartment) - ((char*)(&first_write_offset) - (char*)(this));

    return filePtr->file_write( (&first_write_offset), writeSize);
}

int GeneralDepartment::read_file(File* filePtr) {
    //---------------------//

    if ( !student_array.read_file(filePtr) )
	return 0;

  //---------------------//

    int readSize = sizeof(GeneralDepartment) - ((char*)(&first_write_offset) - (char*)(this));

    return filePtr->file_read( (&first_write_offset), readSize);
}

//--------- End of function GeneralDepartment::write_file ---------------//

//--------- Begin of function StudentArray::write_file ---------------//
//!
int StudentArray::write_file(File* filePtr) {
    if( !filePtr->file_write( (char*) this + sizeof(DynArrayB), sizeof(StudentArray)-sizeof(DynArrayB) ) )
	return 0;

    return DynArrayB::write_file( filePtr );
}

int StudentArray::read_file(File* filePtr) {
    if( !filePtr->file_read( (char*) this + sizeof(DynArrayB), sizeof(StudentArray)-sizeof(DynArrayB) ) )
	return 0;

    if ( !DynArrayB::read_file( filePtr ) )
	return 0;

    err_when(packed_size() != student_count);

    return 1;
}

//--------- End of function StudentArray::write_file ---------------//

/*
//--------- Begin of function Student::write_file ---------------//
//!
int Student::write_file(File* filePtr)
{
  return filePtr->file_write(this, sizeof(Student));
}

int Student::read_file(File* filePtr)
{
  return filePtr->file_read(this, sizeof(Student));
}
//--------- End of function Student::write_file ---------------//
*/

//--------- Begin of function FacultyArray::write_file ---------------//
//!
int FacultyArray::write_file(File* filePtr) {
    if( !filePtr->file_write( (char*) this + sizeof(DynArrayB), sizeof(FacultyArray)-sizeof(DynArrayB) ) )
	return 0;

    return DynArrayB::write_file( filePtr );
}

int FacultyArray::read_file(File* filePtr) {
    if( !filePtr->file_read( (char*) this + sizeof(DynArrayB), sizeof(FacultyArray)-sizeof(DynArrayB) ) )
	return 0;

    return DynArrayB::read_file( filePtr );
}

//--------- End of function FacultyArray::write_file ---------------//

/*
//--------- Begin of function Faculty::write_file ---------------//
//!
int Faculty::write_file(File* filePtr)
{
  //return filePtr->file_write(this, sizeof(Faculty));
  return 1;
}

int Faculty::read_file(File* filePtr)
{
//return filePtr->file_read(this, sizeof(Faculty));
return 1;
}
//--------- End of function Student::Faculty ---------------//
*/

//--------- Begin of function CourseArray::write_file ---------------//
//!
int CourseArray::write_file(File* filePtr) {
    if( !filePtr->file_write( (char*) this + sizeof(DynArrayB), sizeof(CourseArray)-sizeof(DynArrayB) ) )
	return 0;

    return DynArrayB::write_file( filePtr );
}

int CourseArray::read_file(File* filePtr) {
    if( !filePtr->file_read( (char*) this + sizeof(DynArrayB), sizeof(CourseArray)-sizeof(DynArrayB) ) )
	return 0;

    return DynArrayB::read_file( filePtr );
}

//--------- End of function CourseArray::write_file ---------------//

/*
//--------- Begin of function Course::write_file ---------------//
//!
int Course::write_file(File* filePtr)
{
  return 1;	//return filePtr->file_write(this, sizeof(Faculty));
}

int Course::read_file(File* filePtr)
{
    return 1;	//return filePtr->file_read(this, sizeof(Faculty));
}
//--------- End of function Course::write_file ---------------//
*/
