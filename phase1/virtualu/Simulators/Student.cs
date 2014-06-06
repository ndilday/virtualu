using System;

using virtualu;
using virtualu.Utility;

namespace virtualu.Simulators
{
    public enum StudentLevel
    {
        Undergrad,
        NonTraditional,
        Masters,
        Doctoral,
        Distance
    }

    public enum DegreeType
    {
        Bachelors,
        Masters,
        Doctorate
    }

    public enum Talent
    {
        Academic,
        ExtraCurricular,
        Athletic,
        Income
    }

    static class StudentConstants
    {
        public const short MAX_STUDENT_LEVEL = (short)Enum.GetNames(typeof(StudentLevel)).Length;
        public const short MAX_STUDENT_LEVEL_INI = 4;   // for student database init; see student_par spreadsheet 4: ug, master and distance learn // 0107
        public const short MAX_GRADUATE_YEARS = 8;      // for calc transistion probabilities for student techdoc 3.3, section 4.2
        public const short MAX_STUDENT_LEVEL_N_TOTAL = (short)(MAX_STUDENT_LEVEL + 1);
        public const short MAX_STUDENT_LEVEL_TOTAL = MAX_STUDENT_LEVEL;
        public const short MAX_STUDENT_LEVEL_NO_SL1 = (short)(StudentConstants.MAX_STUDENT_LEVEL - 1);
        public const short BACHELOR_YEAR_COUNT = 4;
        public const short MASTER_YEAR_COUNT = 1;
        public const short DOCTOR_YEAR_COUNT = 2;

        public static bool is_nontradition_ug_group(StudentLevel sl)
        {
            return sl == StudentLevel.Distance || sl == StudentLevel.NonTraditional;
        }

        public static bool is_ug_group(StudentLevel sl)
        {
            return sl == StudentLevel.Undergrad || sl == StudentLevel.NonTraditional || sl == StudentLevel.Distance;
        }
    }

    /// <summary>
    /// Class for representing a member of the student body, including personal information,
    /// course history, and talents. Contains methods for computing satisfaction,
    /// selcting courses, and thinking/deciding on future educational pursuits.
    /// Responds to next_day() and read/write. See Tech doc 3.3 section 4.
    /// </summary>
    public class Student : Person
    {
        //--- vars that will remain constant once initialized ---//
        public readonly bool audit_flag;
        public readonly int file_course_id;
        public readonly int old_student_recno;
        public readonly int student_recno;                            //## chea 141099
        public readonly char student_segment;                        //#### fred 0917//

        public readonly int date_admitted;
        public readonly int year_admitted();

        //------- vars that will change over time ------//

        public char  year_in_program;

        public bool   division_status()     { return year_in_program <= 2; }
        public float academic_achievement;                   // a percentage: 0 to 100
        public int financial_aid;

        // FOR DESIGN1028_RESPONSE_FUNC.TXT
        public float initializationValueAC_LIBRARIES;        // ## CHWG290699	//func 29
        public float initializationValueAC_ATHLETICS;        //func 30
        //------- vars pertaining to courses ------//

        // the number of courses the student has to take in each trimester in a year, this array is set based on the summer teaching policy, the total of the values in the array should be 8 (STD_COURSES_PER_TRIMESTER*2)
        public char[]  course_per_trimester = new char[Constants.TRIMESTER_PER_YEAR];

        public char  total_course_all;                       // this is the total of all elements in course_to_take_array[]
        public char  total_elective_course_this_year;        // total number of elective courses taken and to take this school year

        // courses taken before, taking now and will be taking next trimester
        public char[,] course_taken_array = new char[Constants.MAX_DEPARTMENT, Constants.MAX_COURSE_DEPTH_WITHOUT_GR];
        public char[] course_taking_array = new char[Constants.MAX_DEPARTMENT];    // courses taking this trimester, taking now and will be taking next trimester

        public char  failed_course_count;                    // accumulated value from year 1 till graduate
        public char  course_denials_this_trimester;
        public char  course_failures_this_trimester;
        public char  off_quarter;                            // = AUTUMN, WINTER or SUMMER

        //---- talent and satisfaction/performance indices ----//

        //##### begin fred 980917 #####//

        public char  talent_academic;
        public char  talent_extracurricular;
        public char  talent_athletics;

        public char  satisfaction_academic;
        public char  satisfaction_student_life;
        public char  satisfaction_athletics;
        public char  satisfaction_overall;
        public char  faculty_workweek;

        // for calc satisfaction_overall
        public char  reaction_summer_course;
        public char reaction_summer_course_last;

        public float probability_change_major;
        public float multiplier_on_dropout_prob;

        //--- extra effect for chance card ---//
        public float extra_effect_satisfaction_academic;

        public void  init(char studentLevel, char yearInProgram, char genderEthnicGroup, char stuSeg, char major, int finAid, float* talentArr);

        public int write_file(File filePtr);
        public int read_file(File filePtr);

        public void  next_day();
        public void  next_trimester();
        public void  select_course(int ignoreFaculty=0, int initPrevYear=0);

        public int  change_major(int couCompleted);          //## chea 170699

        //------ functions for assigning courses -------//
        int   select_course2(int isRequiredCourse, int ignoreFaculty, int initPrevYear );
        int   assign_course(int selectedCourseDept, int selectedCourseDepth,
			    int selectedCourseFocus, int selectedTeachingMethod,
			    int isRequiredCourse, int ignoreFaculty, int initPrevYear );
        int   get_available_faculty(int departmentRecno);
        int select_required_course(int selectedCourseDept, int selectedCourseDepth, int selectedCourseFocus);
        void  select_elective_course(int selectedCourseDept, int selectedCourseDepth, int selectedCourseFocus);
        int   select_teaching_method(int selectedCourseDept);
        int   study_course(int studyDeptRecno);
        int   total_course_taken_dept(int departmentRecno);
        int has_selected_all_course();

        //--------- other functions --------//
        int   think_graduate();
        int think_dropout();
        int think_change_major(char courseCompleted=0);
        void  think_summer_course();

        //------ functions for performance indicators -------//
        public void  update_history(char update_flag);
        public void  calc_satisfaction_academic();

        void  calc_satisfaction_student_life();
        void  calc_satisfaction_athletics();
        void  calc_satisfaction_overall();
        void  calc_faculty_workweek();

        void  calc_probability_change_major();
        void  calc_multiplier_on_dropout_prob();
    }

    //----------- Define class StudentArray -----//

    /// <summary>
    /// Collection class containing all of the students in the school as well as
    /// methods for summarizing student data. Delegates calls to next_day() and
    /// read/write to each student.
    /// </summary>
    class StudentArray : DynArrayB {
        public int department_recno;
        public int student_count;

        public StudentArray();
        public ~StudentArray();
        public int write_file(File filePtr);
        public int read_file(File filePtr);
        public int add(char studentLevel, char yearInProgram, char genderEthnicGroup, char stuSeg, char major, int finAid, float* talentArr);
        public void del(int recNo);
        public void next_day();
        public void next_trimester();
        public void init_course(int processYearInProgram);
        public int is_deleted(int recNo);

        //---- functions for course selection ---//

        public void select_course(int ignoreFaculty=0);

        public char[,] initial_talent_academic_all = new char[Enum.GetNames(typeof(DegreeType)).Length,Constants.HISTORY_MONTH_COUNT];
        public char[]  initial_performance_academic_ug = new char[Constants.HISTORY_MONTH_COUNT];

        public int[] initial_bachelor_degree = new int[Constants.HISTORY_YEAR_COUNT];
        public int[] initial_master_degree = new int[Constants.HISTORY_YEAR_COUNT];
        public int[] initial_doctor_degree = new int[Constants.HISTORY_YEAR_COUNT];
        public float[,] initial_fake_disp_time_to_degree = new float[Enum.GetNames(typeof(DegreeType)).Length,Constants.HISTORY_YEAR_COUNT];

        public float[,] initial_time_to_degree = new float[Enum.GetNames(typeof(DegreeType)).Length,Constants.HISTORY_YEAR_COUNT];
        public float[,] initial_dropout_rate = new float[Enum.GetNames(typeof(DegreeType)).Length,Constants.HISTORY_YEAR_COUNT];
        // student summary for sl1-5
        public char[,]  talent_academic_all = new char[Enum.GetNames(typeof(DegreeType)).Length,Constants.HISTORY_MONTH_COUNT];
        public char[]  performance_academic_ug = new char[Constants.HISTORY_MONTH_COUNT];

        // student summary for sl1
        public char[] talent_academic = new char[Constants.HISTORY_MONTH_COUNT];
        public char[] talent_extracurricular = new char[Constants.HISTORY_MONTH_COUNT];
        public char[] talent_athletics = new char[Constants.HISTORY_MONTH_COUNT];

        public char[] satisfaction_academic = new char[Constants.HISTORY_MONTH_COUNT];
        public char[] satisfaction_student_life = new char[Constants.HISTORY_MONTH_COUNT];
        public char[] satisfaction_athletics = new char[Constants.HISTORY_MONTH_COUNT];
        // for ultimate score
        public char[] satisfaction_overall = new char[Constants.HISTORY_MONTH_COUNT];

        public float[,] time_to_degree = new float[Enum.GetNames(typeof(DegreeType)).Length,Constants.HISTORY_YEAR_COUNT];
        public float[,] dropout_rate = new float[Enum.GetNames(typeof(DegreeType)).Length,Constants.HISTORY_YEAR_COUNT];

        // special case handling: fake_disp_time_to_degree() for smoothing graph
        public float[,] fake_disp_time_to_degree = new float[Enum.GetNames(typeof(DegreeType)).Length,Constants.HISTORY_YEAR_COUNT];

        public int[] bachelor_degree = new int[Constants.HISTORY_YEAR_COUNT];
        public int[] non_ug_bachelor_degree = new int[Constants.HISTORY_YEAR_COUNT];
        public int[] master_degree = new int[Constants.HISTORY_YEAR_COUNT];
        public int[] doctor_degree = new int[Constants.HISTORY_YEAR_COUNT];

        public int[] time_to_degree_cumm = new int[Enum.GetNames(typeof(DegreeType)).Length];
        public int[] cur_dropout = new int[Enum.GetNames(typeof(DegreeType)).Length];
        public float[] ave_entering_class = new float[Enum.GetNames(typeof(DegreeType)).Length];

        public int cur_bachelor_degree;
        public int cur_non_ug_bachelor_degree;               // store the graduations for nontraditional ug
        public int cur_master_degree;
        public int cur_doctor_degree;

        public int[] last_year_degree = new int[4];                      // for year end report student turnover
        public int[] last_year_dropout = new int[4];

        public int ave_time_to_graduation_for_ug;

        public char[] ave_satisfaction_student_life = new char[Enum.GetNames(typeof(StudentLevel)).Length];

        public void  update_history(char update_flag);
        public void  calc_student_performance();
        public void  calc_degree_awarded();

        public void  save_initial_data();
    }
}
