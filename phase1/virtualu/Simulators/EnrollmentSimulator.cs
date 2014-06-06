using System;

using virtualu;
using virtualu.Utility;

namespace virtualu.Simulators
{
    public static class EnrollmentConstants
    {
        public const float MAX_TALENT_PRIORITY_LEVEL = 10.0f;
        public const short PRIORITY_LEVEL_COUNT = 3;
        public const short INSTITUTION_SEGMENT_COUNT = 7;
        public const short PREFERENCE_COUNT = 20;
        public const short NON_FINANCIAL_PREFERENCE_COUNT = 14;
        public const short PREFERENCE_COUNT2 = 21;
        public const short SCHOOL_CONTROL_COUNT = 2;
        public const short STUDENT_SEGMENT_COUNT = 7;
        public const short STUDENT_SEGMENT_ATHLETE = 3;
        public const int STUDENT_SUBSEGMENT_COUNT = Enum.GetNames(typeof(EthnicGroup)).Length * Enum.GetNames(typeof(Gender)).Length;

        public const short APP_YIELD_VAR_COUNT = 3;
        public const short MAX_TRADITION_COUNT = 5;
        public const short MAX_NONTRA_COUNT = 5;
        public const short MAX_MASTER_COUNT = 3;
        public const short MAX_DOCTOR_COUNT = 3;

        static const float[] yield_latency = { .9f, .5f, .5f, .5f, .5f };
        static const float[] apps_latency = { .4f, .4f, .5f, .6f, .7f };
        static const float[] aidMinMultipliers = { 1, 1.05f, 1.10f };
    }

    enum PublicPrivate
    {
        Private,
        Public
    }

    enum AdmittanceState
    {
        Application,
        Offer,
        Matriculation
    }

    enum EnrollmentReportType
    {
        YieldRate,
        OffersRate,
        PercentMinority,
        Academic,
        Extracurricular,
        Athletics,
        CreditsDemanded,
        CreditsProvided
    }

    enum RadioButtonLevel
    {
        Low,
        Medium,
        High
    }

    enum AidLevel
    {
        None,
        Medium,
        High
    }

    enum DistanceLearnerAcceptance
    {
        Prohibit,
        Accept,
        ActivelySeek
    }

    class PreferenceRec 
    {
        public PreferenceRec(string record);
        public string[] value = new string[EnrollmentConstants.INSTITUTION_SEGMENT_COUNT];
        public string weight;
    }

    class Preference {
        public float average;
        public float deviation;
        public float weight;
        public float tmp_fuzzy_value;
    }

    /// <summary>
    /// Structures containing modeling parameters governing enrollment of students are all levels.
    /// refer to [HE.STU.pref]sheet1, struct for all student levels
    /// </summary>
    class PreferenceRec2 
    {
        public float[] weight = new float[4];
    }

    class Preference2 
    {
        public float apps_ratio;
        public float yield_rate;
    }

    /// <summary>
    /// Structure for Enrollment Module. Refer to [HE.initialization]Apps_Yield.
    /// refer to [HE.initialization]Apps_Yield
    /// </summary>
    class AppYieldRec {
        public short[] count = new short[EnrollmentConstants.INSTITUTION_SEGMENT_COUNT];
    }

    /// <summary>
    /// Structure used by the EnrollRes class to load average student talent and
    /// its variance for all levels from the db.
    /// refer to [HE.STU.talent.xls]Sheet1
    /// </summary>
    class TalentRec {
        public string[] seg = new string[EnrollmentConstants.STUDENT_SEGMENT_COUNT];
    }

    /// <summary>
    /// Structure used by the EnrollRes class to store average student talent and its variance.
    /// </summary>
    class Talent {
        public float average;
        public float variance;
    }

    /// <summary>
    /// Structure for holding enrollment information.
    /// </summary>
    class EnrollData 
    {
        public float apps_ratio;
        public int base_apps;
        public float fraction_matrics;

        public int applications;
        public int matrics;

        public float student_pref;
        public float conversion_rate;
        public float base_conversion_rate;
        public Talent academic;
    }

    class ApplicationData
    {
        public int applications;
        public int offers;
        public int matriculations;
    }

    /// <summary>
    /// Structure for holding enrollment information.
    /// </summary>
    class EnrollDataSL1 
    {
        public float apps_ratio;
        public float yield_rate;
        public int base_apps;
        public float base_yield_rate;
        public float fraction_matrics;

        public ApplicationData application_data;

        public float util;
        public float util_variance;
    }

    class StudentTurnover
    {
        public int new_year_student_count;
        public int last_year_student_count;
        public int student_count;
        public float student_female_percent;
        public float student_minority_percent;
        public float student_minority_female_percent;
    }

    /// <summary>
    /// Simulates enrollment, including applications, offers, financial aid, and
    /// matriculation. Used to get information for comparable schools.
    /// </summary>
    class EnrollRes
    {
#region Multi-dimensional arrays
        public Preference[,] preference_array = 
            new Preference[EnrollmentConstants.PREFERENCE_COUNT,EnrollmentConstants.INSTITUTION_SEGMENT_COUNT];
        public Preference2[,,] preference_array2 = 
            new Preference2[EnrollmentConstants.SCHOOL_CONTROL_COUNT,Enum.GetNames(typeof(EthnicGroup)).Length,EnrollmentConstants.PREFERENCE_COUNT2];
        public short[,,,] app_yield_array =
            new short[EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT,EnrollmentConstants.STUDENT_SEGMENT_COUNT,EnrollmentConstants.INSTITUTION_SEGMENT_COUNT,EnrollmentConstants.APP_YIELD_VAR_COUNT];
        public Talent[,,] talent_array =
            new Talent[EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT,EnrollmentConstants.STUDENT_SEGMENT_COUNT,Enum.GetNames(typeof(Talent)).Length];
        public StudentTurnover[] studentTurnover = new StudentTurnover[StudentConstants.MAX_STUDENT_LEVEL];
#endregion

#region Public Functions
        public void  init()
        {
            load_db_student_pref();
            load_db_student_pref2();
            load_db_app_yield();
            load_db_student_talent();

            distance_learner_acceptance = 1;

            //-------- init constants ----------//
            s_lower_bound = 0.5f;
            s_upper_bound = 1.5f;
            s_slope     = 5.0f;

            chance_avg  = 1.0f;
            chance_sd = 1.0f;

            fraction_merit_top = 0.025f;                    // see section 2.4 last paragraph
        }

        public int write_file(File filePtr);
        public int read_file(File filePtr);

        public void  init_data();
        public void  enroll_main(int initFlag=0);
        public void  next_day();
        public void  update_history(char update_flag);
#endregion

#region Private Functions
        void  load_db_student_pref()
        {
            PreferenceRec prefRec;
            Database dbPreference = Database.open_db(Constants.PREFERENCE_DB);

            int db_count = dbPreference.rec_count();

            if( db_count != EnrollmentConstants.PREFERENCE_COUNT * 2 )
            {
                // 2: average and deviation
                throw new Exception("EnrollmentSimulator.load_db_student_pref.db_count");
            };

            //askbillok: how to handle "na in cells like E119-120
            //	segment2 = segment1

            //------ read in Preference information array -------//
            for( int i = 0 ; i < db_count ; i++ ) 
            {
	            //----- odd row/record store average -----//
	            prefRec = new PreferenceRec( dbPreference.read(i+1));

                int j;
	            for( j = 0 ; j < EnrollmentConstants.INSTITUTION_SEGMENT_COUNT; j++ ) 
                {
	                preference_array[i/2,j].average = (float) Convert.ToDouble( prefRec.value[j] );
	                preference_array[i/2,j].weight = (float) Convert.ToDouble( prefRec.weight );
	            }

	            i++;
	            //-----  even row/record store deviation -----//
	            prefRec = new PreferenceRec( dbPreference.read(i+1));
	            
	            for( j = 0 ; j < EnrollmentConstants.INSTITUTION_SEGMENT_COUNT; j++ )
                {
	                preference_array[i/2,j].deviation = (float) Convert.ToDouble( prefRec.value[j] );
                }

	            preference_array[i/2,j].tmp_fuzzy_value = 0.0f;
            }
        }

        void  load_db_student_pref2()
        {
            PreferenceRec2 prefRec;
            Database dbPreference = Database.open_db(Constants.PREFERENCE_DB);
            string tmpStr;

            int db_count = (short) dbPreference.rec_count();

            //------ read in Preference information array -------//
            Array.Clear(preference_array2, 0, preference_array2.Length);

            // for sl1

            int i, j, arrSL;
            byte isPublic = 0;

            for( i = 0; i < EnrollmentConstants.PREFERENCE_COUNT2 * EnrollmentConstants.SCHOOL_CONTROL_COUNT; i++ )
            {
                if (i >= EnrollmentConstants.PREFERENCE_COUNT2) 
                {
                    isPublic = 1;
                    j = i - EnrollmentConstants.PREFERENCE_COUNT2;
	            }
	            else 
                {
	                j = i;
	            }

	            prefRec = (PreferenceRec2) dbPreference.read(i+1);

                if (strcmp(tmpStr, "") != 0)
                {
                    preference_array2[isPublic, (short)EthnicGroup.NonMinority, j].apps_ratio = prefRec.weight[0];
                }
                else
                {
                    preference_array2[isPublic, (short)EthnicGroup.NonMinority, j].apps_ratio = -1.0f;
                }

                if (strcmp(tmpStr, "") != 0)
                {
                    preference_array2[isPublic, (short)EthnicGroup.Minority, j].apps_ratio = prefRec.weight[1];
                }
                else
                {
                    preference_array2[isPublic, (short)EthnicGroup.Minority, j].apps_ratio = -1.0f;
                }

                if (strcmp(tmpStr, "") != 0)
                {
                    preference_array2[isPublic, (short)EthnicGroup.NonMinority, j].yield_rate = prefRec.weight[2];
                }
                else
                {
                    preference_array2[isPublic, (short)EthnicGroup.NonMinority, j].yield_rate = -1.0f;
                }

                if (strcmp(tmpStr, "") != 0)
                {
                    preference_array2[isPublic, (short)EthnicGroup.Minority, j].yield_rate = prefRec.weight[3];
                }
                else
                {
                    preference_array2[isPublic, (short)EthnicGroup.Minority, j].yield_rate = -1.0f;
                }
            }

            // for sl2-5;  arrSL=0 -> sl2

            for (i = 0, arrSL = -1; i < EnrollmentConstants.PREFERENCE_COUNT2 * (StudentConstants.MAX_STUDENT_LEVEL - 1); i++)
            {
                j = i % EnrollmentConstants.PREFERENCE_COUNT2;

	            if ( j==0 ) 
                {
	                arrSL++;
	            }

                prefRec = (PreferenceRec2)dbPreference.read(EnrollmentConstants.PREFERENCE_COUNT2 * EnrollmentConstants.SCHOOL_CONTROL_COUNT + i + 1);

	            // for sl1
	            if ( i == 63 ) 
                {                              // start of SL5, Institutional Prestige
                    preference_array2_sls[arrSL, (short)PublicPrivate.Private, (short)EthnicGroup.NonMinority, 0] = 0.1f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Private, (short)EthnicGroup.Minority, 0] = 0.1f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Public, (short)EthnicGroup.NonMinority, 0] = 0.1f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Public, (short)EthnicGroup.Minority, 0] = 0.1f;
	            }
	            else if ( i == 75 ) 
                {                              // % of minority
                    preference_array2_sls[arrSL, (short)PublicPrivate.Private, (short)EthnicGroup.NonMinority, 12] = -1.0f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Private, (short)EthnicGroup.Minority, 12] = -1.0f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Public, (short)EthnicGroup.NonMinority, 12] = -1.0f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Public, (short)EthnicGroup.NonMinority, 12] = -1.0f;
	            }
	            else if ( i == 78 ) 
                {                              // Tuition: Private Institutions
                    preference_array2_sls[arrSL, (short)PublicPrivate.Private, (short)EthnicGroup.NonMinority, 15] = 0.5f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Private, (short)EthnicGroup.Minority, 15] = 0.5f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Public, (short)EthnicGroup.NonMinority, 15] = -1.0f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Public, (short)EthnicGroup.Minority, 15] = -1.0f;
		        }
                else if (i == 79)
                {                              // Blended Tuition: Public Institutions
                    preference_array2_sls[arrSL, (short)PublicPrivate.Private, (short)EthnicGroup.NonMinority, 16] = -1.0f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Private, (short)EthnicGroup.Minority, 16] = -1.0f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Public, (short)EthnicGroup.NonMinority, 16] = 0.3f;
                    preference_array2_sls[arrSL, (short)PublicPrivate.Public, (short)EthnicGroup.Minority, 16] = 0.3f;
                }
                else
                {
                    m.rtrim_fld(tmpStr, prefRec.weight[0], 9);
                    preference_array2_sls[arrSL, (short)PublicPrivate.Private, (short)EthnicGroup.NonMinority, j]
                        = (strcmp(tmpStr, "") != 0) ? prefRec.weight[0] : -1.0f;

                    m.rtrim_fld(tmpStr, prefRec.weight[1], 9);
                    preference_array2_sls[arrSL, (short)PublicPrivate.Private, (short)EthnicGroup.Minority, j]
                        = (strcmp(tmpStr, "") != 0) ? prefRec.weight[1] : -1.0f;

                    m.rtrim_fld(tmpStr, prefRec.weight[2], 9);
                    preference_array2_sls[arrSL, (short)PublicPrivate.Public, (short)EthnicGroup.NonMinority, j]
                        = (strcmp(tmpStr, "") != 0) ? prefRec.weight[2] : -1.0f;

                    m.rtrim_fld(tmpStr, prefRec.weight[3], 9);
                    preference_array2_sls[arrSL, (short)PublicPrivate.Public, (short)EthnicGroup.Minority, j]
                        = (strcmp(tmpStr, "") != 0) ? prefRec.weight[3] : -1.0f;
                }
            }
            // set new variable (Percent of courses redesigned for optimal IT)
            // just for SL-5 Distance Learning
            preference_array2_sls[3, (short)PublicPrivate.Private, (short)EthnicGroup.NonMinority, 21] = 0.4f;
            preference_array2_sls[3, (short)PublicPrivate.Private, (short)EthnicGroup.Minority, 21] = 0.4f;
            preference_array2_sls[3, (short)PublicPrivate.Public, (short)EthnicGroup.NonMinority, 21] = 0.6f;
            preference_array2_sls[3, (short)PublicPrivate.Public, (short)EthnicGroup.Minority, 21] = 0.6f;
        }

        void  load_db_student_talent()
        {
            TalentRec talentRec;
            Database dbPreference = game_set.open_db(Constants.TALENT_DB);

            int db_count = (short) dbPreference.rec_count();
            if (db_count != EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT * Enum.GetNames(typeof(Talent)).Length * 3)
            {
                throw new FormatException("TalentDB file has incorrect number of rows!");
            }

            //------ read in array -------//
            Array.Clear(talent_array, 0, talent_array.Length);
            int var, subSeg, seg, i=1;

            for (subSeg = 0; subSeg < EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT; subSeg++)
            {
                for (var = 0; var < Enum.GetNames(typeof(Talent)).Length; var++)
                {
                    // read average row
                    talentRec = (TalentRec)dbPreference.read(i++);

                    for (seg = 0; seg < EnrollmentConstants.STUDENT_SEGMENT_COUNT; seg++)
                    {
                        talent_array[subSeg, seg, var].average
                            = (float)m.atof(talentRec.seg[seg]);
                    }

                    // read standard deviation row
                    talentRec = (TalentRec)dbPreference.read(i++);

                    for (seg = 0; seg < EnrollmentConstants.STUDENT_SEGMENT_COUNT; seg++)
                    {
                        talent_array[subSeg, seg, var].variance
                            = (float)m.atof(talentRec.seg[seg]);
                    }

                    // skip the row "count" for each talent rating
                    //talentRec = (TalentRec*) dbPreference->read(i++);
                    i++;
                }
            }
            // adjust the talent_array
            // refer to mathca cell:
            // "Traditional Undergraduates: sL-1: Test financial data"
            //
            // 10 years
            const float INCOME_UPDATE = math.safe_pow(1.04f, 10);

            for (subSeg = 0; subSeg < EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT; subSeg++)
            {
	        for ( var = 0; var < Enum.GetNames(typeof(Talent)).Length - 1; var++ )
            {
                for (seg = 0; seg < EnrollmentConstants.STUDENT_SEGMENT_COUNT; seg++)
                {
		        talent_array[subSeg,seg,var].variance
		            = math.safe_pow(talent_array[subSeg,seg,var].variance, 2);
	            }

	        for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	            talent_array[subSeg,seg,INCOME].average
		        *= INCOME_UPDATE;
	            talent_array[subSeg,seg,INCOME].variance
		        // * INCOME_UPDATE
		        = math.safe_pow(talent_array[subSeg,seg,var].variance / 10, 2);
	        }
            }

            //--------------------------//

            // 1027 req26.txt

            for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	        for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	            income_growth[subSeg,seg] = math.get_random_snd(0.015f, PSCH_SD(.0025f));
	            income_growth_last2[subSeg,seg] = 0;
	        }
            }
        }

        void  load_db_app_yield()
        {
            AppYieldRec appRec;
            Database dbAppYield = game_set.open_db(Constants.APP_YIELD_DB);
            int varOffset, stuSubSeg, stuSeg, iSeg, i=0;

            int db_count = (short) dbAppYield.rec_count();
            if (db_count != EnrollmentConstants.STUDENT_SEGMENT_COUNT * EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT * Enum.GetNames(typeof(AdmittanceState)).Length)
            {
                throw new FormatException("Yield DB had an incorrect number of rows");
            }

            //------ read in Preference information array -------//
            Array.Clear(app_yield_array, 0, app_yield_array.Length);

            for (varOffset = 0; varOffset < Enum.GetNames(typeof(AdmittanceState)).Length; varOffset++)
            {
                for (stuSubSeg = 0; stuSubSeg < EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT; stuSubSeg++)
                {
                    for (stuSeg = 0; stuSeg < EnrollmentConstants.STUDENT_SEGMENT_COUNT; stuSeg++)
                    {
                        appRec = (AppYieldRec)dbAppYield.read(i + 1);
                        i++;
                        for (iSeg = 0; iSeg < EnrollmentConstants.INSTITUTION_SEGMENT_COUNT; iSeg++)
                        {
                            app_yield_array[stuSubSeg, stuSeg, iSeg, varOffset] = appRec.count[iSeg];
                        }
                    }
                }
            }
        }

        void  update_income()
        {
            // adjust this var: Talent talent_array[STUDENT_SUBSEGMENT_COUNT,STUDENT_SEGMENT_COUNT,TALENT_INCOME_VAR_COUNT];
            int subSeg, seg;
            float inflationRate = (float) finance.inflation_rate / 100.0f;

            for ( subSeg=0; subSeg<STUDENT_SUBSEGMENT_COUNT; subSeg++ ) {
	        for ( seg=0; seg<STUDENT_SEGMENT_COUNT; seg++ ) {
	            float lastValue = income_growth[subSeg,seg];

	            income_growth[subSeg,seg] = math.time_variation(lastValue, income_growth_last2[subSeg,seg], 0.9908f, -0.6314f, 0.003202690f);
	            income_growth_last2[subSeg,seg] = lastValue;

	            talent_array[subSeg,seg,INCOME].average *= (1+inflationRate+income_growth[subSeg,seg]);
	        }
            }
        }
#endregion
 
#region Single-Dimension Arrays
        // for interface on student turnover
        public int[] new_year_student_count = new int[StudentConstants.MAX_STUDENT_LEVEL];
        public int[] last_year_student_count = new int[StudentConstants.MAX_STUDENT_LEVEL];

        // updated per trimester:
        public int total_student_count;
        public int[] student_count = new int[StudentConstants.MAX_STUDENT_LEVEL];
        public float[] student_female_percent = new float[StudentConstants.MAX_STUDENT_LEVEL];
        public float[] student_minority_percent = new float[StudentConstants.MAX_STUDENT_LEVEL];
        // for new formula of diversity
        public float[] student_minority_female_percent = new float[StudentConstants.MAX_STUDENT_LEVEL];
#endregion

        public void  calc_student_count(int initFlag=0);

        // 990427
        public void  set_app_yield(float appMultipler, float yieldMultiplier)
        {
            int stuSubSeg, stuSeg;

            for (stuSubSeg = 0; stuSubSeg < EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT; stuSubSeg++)
            {
                for (stuSeg = 0; stuSeg < EnrollmentConstants.STUDENT_SEGMENT_COUNT; stuSeg++)
                {
                    enroll_data_sl1[stuSubSeg, stuSeg].apps_ratio *= appMultipler;
                    enroll_data_sl1[stuSubSeg, stuSeg].yield_rate *= yieldMultiplier;
                }
            }
        }

        //----------------------------------------------------//
        //-- data and methods for PGI enrollment only		---//
        //----------------------------------------------------//
        //----- for athletic office -----//
        public int male_matrics_top_athletes;                // for all student level
        public char  avg_athletic_talent_male_matrics;
        public char  avg_athletic_talent_female_matrics;

        //----- for interface -----//
        public int matrics_top_athletes;                     // for all student level
        public float total_aid;                              // for all student level // ($000)
        public ApplicationData[] total_applications = new ApplicationData[StudentConstants.MAX_STUDENT_LEVEL];

        // rates in 0.999 for 99.9% format;  ug_applications_rate & ug_yield_rate defined in School will not be updated
        public float[] applications_rate = new float[StudentConstants.MAX_STUDENT_LEVEL];
        public float[] cur_yield_rate = new float[StudentConstants.MAX_STUDENT_LEVEL];
        public float[] offers_rate = new float[StudentConstants.MAX_STUDENT_LEVEL];

        public int[,] talent_average = new int[StudentConstants.MAX_STUDENT_LEVEL_N_TOTAL,Enum.GetNames(typeof(Talent)).Length];
        //99 -> 99%
        public float overall_yield_rate;
        public float overall_offers_rate;
        public float overall_apps_rate;

        public float[] initial_offers_rate = new float[StudentConstants.MAX_STUDENT_LEVEL];
        public float[] initial_cur_yield_rate = new float[StudentConstants.MAX_STUDENT_LEVEL];
        public int initial_matrics_top_athletes;

        // adjusted to thousands of dollars
        float max_aid;
        float need_lower;
        float need_upper;
        float[] merit_lower = new float[StudentConstants.MAX_STUDENT_LEVEL];
        float[] merit_upper = new float[StudentConstants.MAX_STUDENT_LEVEL];

        float[] fraction_on_aid = new float[Enum.GetNames(typeof(EthnicGroup)).Length];
        int[] average_aid = new int[Enum.GetNames(typeof(EthnicGroup)).Length];

        // for storing result of calc_student_pref()
        float[] pref_weight_array = new float[EnrollmentConstants.INSTITUTION_SEGMENT_COUNT];
    
        public EnrollDataSL1[,] enroll_data_sl1 = new EnrollDataSL1[EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT,EnrollmentConstants.STUDENT_SEGMENT_COUNT];
        public EnrollData[,] enroll_data_sls = new EnrollData[StudentConstants.MAX_STUDENT_LEVEL_NO_SL1,EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT];

        //-------- define constants for the calc_applications and matriculations ----------//
        // s-function parameters from R33:35 in HE.STU.pref.xls
        float s_lower_bound;
        float s_upper_bound;
        float s_slope;

        // see R31 in HE.STU.pref.xls
        bool  pref_special_case(int x)  
        { 
            return ( x==5 || x==7 || x==9 || x==15 || x==16 );  
        }

        float chance_avg;
        float chance_sd;
        float fraction_merit_top;

        void  init_data_sl1();
        void  calc_talent_student_db();
        
        void  calc_student_pref(SchoolRes school_res, EnrollRes enroll_res, PlayerSchool player_school)
        {
            // column L
            // recalculate PeerSchool's properties to prefArray to speed up calculation
            // '10' is the additional field included in PREFERECNE_COUNT2 (pref_vars_array)
            // but not PREFERECNE_COUNT (prefArray)
            //
            PeerSchool player = school_res.player_peer_school;
            float[] prefArray = new float[EnrollmentConstants.PREFERENCE_COUNT];
            int i, j;

            for (j = 0, i = 0; j < EnrollmentConstants.PREFERENCE_COUNT; j++, i++) 
            {
	            if ( j == 10 )                                // skip in-state freshmen percent
                {
	                i++;
                }

	            prefArray[j] = player.pref_vars_array[i];
            }

            // E129-K147: Fuzzy Logic Indices for Undergraduate Applications Ratios and Yield Rates
            Preference prefPtr;

            for (j = 0; j < EnrollmentConstants.PREFERENCE_COUNT; j++)
            {
	            for (i = 0; i < EnrollmentConstants.INSTITUTION_SEGMENT_COUNT; i++) 
                {
	                prefPtr = enroll_res.preference_array[j,i];
                    if(prefPtr.deviation == 0)
                    {
                        prefPtr.tmp_fuzzy_value = 0;
                    }
                    else
                    {
                        prefPtr.tmp_fuzzy_value = (float)Math.Exp(-Math.Pow((prefPtr.average - prefArray[j])/prefPtr.deviation,2));
                    }
	            }
            }

            // E150-K150: weighted raw index value: update player's preference_array
            bool isPlayerSchoolPublic = player_school.is_public();
            float tmpSumOfSum = 0.0f;

            for (i = 0; i < EnrollmentConstants.INSTITUTION_SEGMENT_COUNT; i++) 
            {
	            float tmpSum = 0.0f;

	            for (j = 0; j < EnrollmentConstants.NON_FINANCIAL_PREFERENCE_COUNT; j++) 
                {
	                prefPtr = enroll_res.preference_array[j,i];
	                tmpSum += prefPtr.tmp_fuzzy_value * prefPtr.weight;
	            }

	            if(tmpSum == 0)
                {
                    throw new InvalidOperationException("tmpSum is zero");
                }

	            for (j = EnrollmentConstants.NON_FINANCIAL_PREFERENCE_COUNT; j < EnrollmentConstants.PREFERENCE_COUNT; j += 2) 
                {
	                prefPtr = isPlayerSchoolPublic ? enroll_res.preference_array[j, i+1] : enroll_res.preference_array[j,i];
	                tmpSum += prefPtr.tmp_fuzzy_value * prefPtr.weight;
	            }

	            pref_weight_array[i] = tmpSum;
	            tmpSumOfSum += tmpSum;

	            if(tmpSum == 0)
                {
                    throw new InvalidOperationException("tmpSum is zero");
                }
            }

            // E151-K151: calc segment weight
            for (i = 0; i < EnrollmentConstants.INSTITUTION_SEGMENT_COUNT; i++)
            {
	            pref_weight_array[i] /= tmpSumOfSum;
            }
        }

        void  calc_app_yield(EnrollRes enroll_res)
        {
            float[] appYield = new float[Enum.GetNames(typeof(AdmittanceState)).Length];
            float totalMatrics = 0;
            int varOffset, stuSubSeg, stuSeg, iSeg, i=0;

            // refer to columns BM:QR
            for ( stuSubSeg = 0; stuSubSeg < EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
            {
                for (stuSeg = 0; stuSeg < EnrollmentConstants.STUDENT_SEGMENT_COUNT; stuSeg++) 
                {
                    Array.Clear(appYield, 0, appYield.Length);
	                for ( varOffset = 0; varOffset < Enum.GetNames(typeof(AdmittanceState)).Length; varOffset++ ) 
                    {
		                for ( iSeg = 0; iSeg < EnrollmentConstants.INSTITUTION_SEGMENT_COUNT; iSeg++ ) 
                        {
		                    appYield[varOffset] += (enroll_res.app_yield_array[stuSubSeg,stuSeg,iSeg,varOffset] * pref_weight_array[iSeg]);
                            if(appYield[varOffset] < 0)
                            {
                                throw new InvalidOperationException("appYield[varOffset] < 0");
                            }
		                }
	                }
                }

                if(appYield[(short)AdmittanceState.Matriculation] < 0.000001f)
                {
                    throw new InvalidOperationException("appYield[(short)AdmittanceState.Matriculation] < 0.000001f");
                }
                if(appYield[(short)AdmittanceState.Offer] < 0.000001f)
                {
                    throw new InvalidOperationException("appYield[(short)AdmittanceState.Offer] < 0.000001f");
                }
                if(appYield[(short)AdmittanceState.Matriculation] > appYield[(short)AdmittanceState.Application])
                {
                    throw new InvalidOperationException("appYield[(short)AdmittanceState.Matriculation] > appYield[(short)AdmittanceState.Application]");
                }

	            //TO use pointer to speed it up
	            enroll_data_sl1[stuSubSeg,stuSeg].apps_ratio = (float)(appYield[(short)AdmittanceState.Application]) / appYield[(short)AdmittanceState.Matriculation];
	            enroll_data_sl1[stuSubSeg,stuSeg].yield_rate = (float)(appYield[(short)AdmittanceState.Matriculation]) / appYield[(short)AdmittanceState.Offer];
	            enroll_data_sl1[stuSubSeg,stuSeg].fraction_matrics = appYield[(short)AdmittanceState.Matriculation];

	            totalMatrics += appYield[(short)AdmittanceState.Matriculation];
	        }

            // note: apps_ratio in enroll_data is not updated yearly, it's just for init_enroll
            if(totalMatrics <= 0)
            {
                throw new InvalidOperationException("totalMatrics <= 0");
            }

            for ( stuSubSeg = 0; stuSubSeg < EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ )
            {
	            for ( stuSeg = 0; stuSeg < EnrollmentConstants.STUDENT_SEGMENT_COUNT; stuSeg++ ) 
                {
	                enroll_data_sl1[stuSubSeg,stuSeg].fraction_matrics /= totalMatrics;
	            }
            }
        }

        /// <summary>
        /// Refer to [HE.STU.pref.xls]Sheet1 and techdoc 3.4 section 2.2 & 2.5
        /// and the mathca file HE.EnrMgt.rev.nb: "Traditional Undergraduates: sL-1: Applications: function"
        /// and the mathca file HE.EnrMgt.rev.nb: "Traditional Undergraduates: sL-1: Matriculations: function"
        /// 
        /// The result, in cell R29, will be multiplied by the the base figure
        /// calculated per (6) to get the final apps ratio and yield rate for the
        /// appropriate gender-ethnic group and institutional type.
        /// (from email req11_enroll.txt)
        /// </summary>
        public void  calc_enroll_applications()
        {
            const int sl = 0;
            float[] prefVarsArray = new float[EnrollmentConstants.PREFERENCE_COUNT2];         // column Q
            int i, j;

            // calc values for column Q
            // The data in column Q represent
            // the "ratio of the variable for the PGI to the average of the same variable
            // for the peer institutions." (from email req11_enroll.txt)
            //
            PeerSchool player = school_res.player_peer_school;
            Array.Copy(prefVarsArray, player.pref_vars_array, EnrollmentConstants.PREFERENCE_COUNT2);

            for (i = 0; i < EnrollmentConstants.PREFERENCE_COUNT2; i++) 
            {
	            // (* uses lagged financial aid variables; current values for all else *)
	            if ( i < EnrollmentConstants.PREFERENCE_COUNT2 - 4 )
                {
	                prefVarsArray[i] = math.safe_divide(prefVarsArray[i], PeerSchool.pref_vars_average_array[i]);
                }
	            else
                {
	                prefVarsArray[i] = math.safe_divide(player.pref_vars_array_last[i], PeerSchool.pref_vars_average_array_last[i]);
                }
            }

            // calc values for column R
            //
            bool isPublic = player_school.is_public();
            float[] finalRatio = new float[Enum.GetNames(typeof(EthnicGroup)).Length];
            float tmpRatio;
            float tmp, sign;

            for (j = 0; j < Enum.GetNames(typeof(EthnicGroup)).Length; j++) 
            {
	            finalRatio[j] = 1;

	            for (i=0; i<EnrollmentConstants.PREFERENCE_COUNT2; i++) 
                {
	                sign = (pref_special_case(i)) ? -1.0f: 1.0f;

	                // 1
	                tmp = enroll_res.preference_array2[isPublic,j,i].apps_ratio;

	                if ( tmp <= 0.0 || prefVarsArray[i] == 0)
                    {
		                tmpRatio = 1.0f;
                    }
	                else
                    {
		                tmpRatio = math.safe_pow(prefVarsArray[i], sign * tmp);
                    }

	                // 2
	                finalRatio[j] *= tmpRatio;
	            }

	            if ( !(1+ (float) exp(-s_slope*(finalRatio[j]-1))) )
                {
	                finalRatio[j] = 1;
                }
	            else
                {
	                finalRatio[j] = s_lower_bound+(s_upper_bound-s_lower_bound)/(1+ (float) exp(-s_slope*(finalRatio[j]-1)));
                }
	            err_when( finalRatio[j] < 0 );
            }

            // refer to mathca code only this part
            //
            int minorGroup, stuSubSeg, stuSeg;
            EnrollDataSL1 ed;

            total_applications[sl] = 0;

            finalRatio[MINORITY] *= app_multiplier[sl];
            finalRatio[NON_MINORITY] *= app_multiplier[sl];

            //----------------//

            //	total_applications[sl]=0;	// ## CHWG062199

            for ( stuSubSeg=0; stuSubSeg<STUDENT_SUBSEGMENT_COUNT; stuSubSeg++ ) 
            {
	            if ( is_minority_group(stuSubSeg) )
                {
	                minorGroup = MINORITY;
	            }
                else
                {
	                minorGroup = NON_MINORITY;
                }
	            for ( stuSeg=0; stuSeg<STUDENT_SEGMENT_COUNT; stuSeg++ ) 
                {
	                ed = enroll_data_sl1[stuSubSeg,stuSeg];

	                ed.applications = 
                        (int)( (1.0f-apps_latency[sl]) * math.get_random_snd(chance_avg, PSCH_SD(chance_sd), true)
		                   * finalRatio[minorGroup] * ed->base_apps
		                   // + 0.5 for float-int rounding
		                   + apps_latency[sl] * ed->applications + 0.5);

	                err_when(ed->applications < 0);
	                total_applications[sl] += ed->applications;
	            }
            }
        }

        public void  calc_enroll_offers();
        public void  calc_enroll_matriculations();

        void  calc_post_enroll();
        void  calc_total_aid();
        void  calc_post_enroll_adjust_aid_all_sls();

        public void  update_enroll_history(char noshift=0);

        public float calc_an_aid(Student stuPtr);
        public void  calc_a_talent(float[] talentArr, char sl, char subSeg, char seg=0);

        void save_initial_data();

        //----------------------------------------------------//
        //-- data and methods for PGI enrollment sl2-5		---//
        //----------------------------------------------------//

        //intermediate vars
        // a array read from db
        float[,,,] preference_array2_sls = 
            new float[StudentConstants.MAX_STUDENT_LEVEL_NO_SL1,
                      EnrollmentConstants.SCHOOL_CONTROL_COUNT,
                      Enum.GetNames(typeof(EthnicGroup)).Length,
                      EnrollmentConstants.PREFERENCE_COUNT2+1];
        float[] min_academic_rating = new float[StudentConstants.MAX_STUDENT_LEVEL_NO_SL1];

        public float[] app_multiplier = new float[StudentConstants.MAX_STUDENT_LEVEL];
        public float[] yield_multiplier = new float[StudentConstants.MAX_STUDENT_LEVEL];

        void  init_data_sls(char sl);
        void  calc_app_multiplier(char sl);

        public void  enroll_main_sls(char sl);
        public void  update_student_intake();

        void  calc_post_enroll_sls(char sl);
        void  calc_enroll_aid_sls(char sl);

        //----------------------------------------------------//
        //-- data and methods for player's input and output --//
        //----------------------------------------------------//
        // 1. player input for calc_*_aid
        public float fraction_sl1_offered_merit_aid;
        public float fraction_sl2_offered_aid;
        public float fraction_sl5_offered_aid;               // askbill to add if
        public AidLevel  minority_aid_special;                   // (* 2 = high; 1=medium; 0 = none *)	// see SET_HIGH
        public AidLevel  athlete_aid_special;                    // (* 2 = high; 1=medium; 0 = none *)

        // 2. player input for calc_enroll_offers (admissions)
        public byte[] offers_priority = new byte[Enum.GetNames(typeof(Talent)).Length];      // = [0,MAX_TALENT_PRIORITY_LEVEL]
        public AidLevel  minority_offers_special;                // (* 2 = high; 1=medium; 0 = none *)
        public AidLevel  athlete_offers_special;                 // (* 2 = high; 1=medium; 0 = none *)
        public DistanceLearnerAcceptance  distance_learner_acceptance;            // (* 2 = actively seek; 1 = accept; 0 = prohibit *)

        // 3. player input for generate student sims
        public float fraction_need_covered;
        public bool  aid_budget_enforced;                    //temp not used

        // 4. for screen Overview
        public int[] target_student_intake = new int[StudentConstants.MAX_STUDENT_LEVEL];
        public double[] target_student_intake_ratio = new double[StudentConstants.MAX_STUDENT_LEVEL];
        //## chea 040999 store the value for the last year target ribbon
        public int[] last_year_target_student_intake = new int[StudentConstants.MAX_STUDENT_LEVEL];

        public int[] target_enrollment = new int[StudentConstants.MAX_STUDENT_LEVEL];

        // values for interface output
        public int overall_percent_minority;
        public int overall_percent_female;
        public int overall_percent_minority_female;          // for new formula of diversity
        public int[] overall_gender_ethnic_group_student_count = new int[EnrollmentConstants.STUDENT_SUBSEGMENT_COUNT];
        public int overall_percent_minority_matric;
        public int[,,] matric_history = new int[StudentConstants.MAX_STUDENT_LEVEL_N_TOTAL,Enum.GetNames(typeof(EnrollmentReportType)).Length,Constants.HISTORY_YEAR_COUNT];
        public int[] distance_credits_provided_history = new int[Constants.HISTORY_TRIMESTER_COUNT];

        void  calc_enroll_aid();
        void  player_change_offers_priority();
        void  calc_doctoral_intake_target();

        float calc_need_aid(float x);
        float calc_merit_aid(float x, float base1);
        float calc_merit_aid_sls(char sl, float x, float base1);
        
        /// <summary>
        /// adjust player input; refer to to "General:helper functions"
        /// </summary>
        /// <param name="fraction"></param>
        void  adjust_fraction_merit_aid(float fraction)
        {
            if ( fraction > 0 && fraction < fraction_merit_top - .005f )
            {
	            fraction = fraction_merit_top + .005f;
            }
            else if ( fraction > 1 + .005f - fraction_merit_top )
            {
	            fraction = (1 - .005f - fraction_merit_top) ;
            }
        }

        float calc_academic_rating(char sl, char subSeg, float academicRating, float base1);
    }
}
