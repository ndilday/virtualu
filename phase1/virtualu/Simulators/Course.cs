using System;

using virtualu.Utility;

namespace virtualu.Simulators
{
    class CourseConstants
    {
        public const short STD_COURSE_PER_TRIMESTER = 4;            // total number of courses in a trimester
        public const short STD_COURSE_NUG_PER_TRIMESTER = 2;
        public const short STD_COURSE_PER_YEAR = (STD_COURSE_PER_TRIMESTER * 2);
        public const short TOTAL_REQUIRED_COURSE = 4;
        public const short TOTAL_ELECTIVE_COURSE = 2;
        public const short CONTACT_HOUR_PER_COURSE = 3;             // needed contact hours of a faculty for teaching a section of the selected course
        public const short MAX_YEAR_REQUIRE_ELECTIVE_COURSE = 4;    // maximum number of years in program that require taking elective courses, for defining the size of Department::min_elective_course_by_year[MAX_YEAR_REQUIRE_ELECTIVE_COURSE]
        
        //----- Define number of courses required for different types of degree -----//
        public const short COURSE_COUNT_REQUIRED_FOR_BACHELOR = 32;
        public const short COURSE_COUNT_REQUIRED_FOR_MASTER = 8;
        public const short COURSE_COUNT_REQUIRED_FOR_DOCTOR = 16;
    }

    enum CourseDepth
    {
        First,
        Second,
        Third,
        Graduate
    }

    enum TeachingMethod
    {
        Seminar, 
        DistanceLearning,
        Class,
        Breakout,
        General
    }

    class Course
    {
        public short course_recno;
        public short department_recno;

        public char depth_category;
        public char focus_category;

        public char teaching_method;
        public char is_closed;                              // 1 if this course is closed. This happens when there is no faculty member available for creating new course section.

        public int distance_learning_place;
        public int actual_distance_learning_enrollment;

        //-------- counter variables ---------//

        public short convention_student_count;               // the total number of students in all sections of this course
        public short distance_student_count;                 // not used yet

        public int normal_class_size();                      // should be read "normal section size for any course with the corresponding teaching method (column A) for each template in course_template"

        public short admission_denied_count;
        public short failure_count;

        public short outsourcing_credits;

        public short class_section_count;                    // the total number of sections in this course
        public short cur_section_student_count;              // number of students in the current active section

        public int write_file(File filePtr);
        public int read_file(File filePtr);

        public void  next_trimester();
        public void  reset_vars();
    }

    class CourseArray : DynArrayB 
    {
        public int department_recno;
        public int[] course_enrollments = new int[Enum.GetNames(typeof(TeachingMethod)).Length];  // for facility office

        //---------- data for interface -------------//
        public int[] class_section_count = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] convention_student_count = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] average_class_size = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] normal_class_size = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];

        public int[] size_deviation = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] admission_denied_count = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] failure_count = new int[Enum.GetNames(typeof(TeachingMethod)).Length];

        public int[] outsourcing_credits = new int[Enum.GetNames(typeof(TeachingMethod)).Length];

        public int[] experienced_class_size = new int[Constants.HISTORY_TRIMESTER_COUNT];

        public int[,] class_section_count_history = 
            new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1,Constants.HISTORY_TRIMESTER_COUNT];
        public int[,] convention_student_count_history = 
            new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1,Constants.HISTORY_TRIMESTER_COUNT];
        public int[,] size_deviation_history = 
            new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1,Constants.HISTORY_TRIMESTER_COUNT];
        public int[,] admission_denied_count_history = 
            new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1,Constants.HISTORY_TRIMESTER_COUNT];
        public int[,] failure_count_history = 
            new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1,Constants.HISTORY_TRIMESTER_COUNT];
        public int[,] outsourcing_credits_history = 
            new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1,Constants.HISTORY_TRIMESTER_COUNT];

        public int[] initial_class_section_count = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] initial_convention_student_count = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] initial_average_class_size = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] initial_size_deviation = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] initial_admission_denied_count = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] initial_failure_count = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];
        public int[] initial_normal_class_size = new int[Enum.GetNames(typeof(TeachingMethod)).Length + 1];

        public int initial_experienced_class_size;

        public CourseArray();
        public ~CourseArray();

        public int write_file(File filePtr);
        public int read_file(File filePtr);

        public void add(Course newCourse);
        public void del(int recNo);

        public void next_day(int initial=0);
        public void next_trimester();
        public void update_history();

        public Course get_course(int courseDepth, int courseFocus, int teachingMethod);

        public int get_depth(int depthFocusId);
        public int get_focus(int depthFocusId);

        public int is_deleted(int recNo);
        public void save_initial_data();
    }
}
