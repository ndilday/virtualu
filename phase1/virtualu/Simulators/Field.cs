using System;

using virtualu.Utility;

namespace virtualu.Simulators
{
    enum FieldType
    {
        Agriculture,
        PersonalPracticalServices,
        Business,
        CommunicationsLibraryScience,
        Education,
        Engineering,
        ForeignLanguages,
        HealthSocialServices,
        Humanities,
        InterdisciplinaryStudies,
        PhysicalScience,
        ComputerScience,
        SocialScience,
        FineArts
    }

    enum GameField
    {
        AppliedScience,
        Business,
        CreativeArts,
        Engineering,
        Humanities,
        LanguageStudies,
        MathComputerScience,
        NaturalScience,
        SocialScience
    }

    /// <summary>
    /// Structure containing information associated with major fields as stored in the database.
    /// </summary>
    class FieldRec {
        public char  code;
        public string  name;

        public char[,] elective_course_focus_pref = new char[Enum.GetNames(typeof(CourseDepth)).Length - 1,2];
    }

    /// <summary>
    /// Structure containing information associated with the major fields in the
    /// simulation, including name, code number, and elective fulfillment.
    /// </summary>
    class FieldInfo 
    {
        char code;
        string name;

        // the total of preference weight across different focus categories is 100
        short[] elective_course_focus_pref = new short[Enum.GetNames(typeof(CourseDepth)).Length - 1];
    }

    class FieldRes 
    {
        public char init_flag;
        public short field_count;
        public FieldInfo info_array;

        public FieldRes();

        public void init();
        public void deinit();

        public int look_up_code(int fieldCode);

        public int write_file(File);
        public int read_file(File);

        void        load_info();
    }
}
