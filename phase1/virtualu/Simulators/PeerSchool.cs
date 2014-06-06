using System;

namespace virtualu.Simulators
{
    /// <summary>
    /// An extended version of SchoolEx, used for representing peer schools.
    /// </summary>
    class PeerSchool : SchoolEx
    {
        SchoolRes school_res;
        EnrollRes enroll_res;

        #region Public Fields
        //TO: acorrding to email "re:q8: some easy questions", the following fields are temporiarily
        // and will be defined more clearerly later
        public short average_aid_per_student;

        public float student_faculty_ratio;
        public char long_term_adjunct_per_regular_facult_ratio;
        public char short_term_adjunct_per_regular_facult_ratio;
        public char tenure_ratio;
        public char liabilities_as_percent_of_assets;

        public float game_score;

        public float[] pref_vars_array = new float[EnrollmentConstants.PREFERENCE_COUNT2];
        public float[] pref_vars_array_last = new float[EnrollmentConstants.PREFERENCE_COUNT2];

        public static float[] pref_vars_average_array = new float[EnrollmentConstants.PREFERENCE_COUNT2];
        public static float[] pref_vars_average_array_last = new float[EnrollmentConstants.PREFERENCE_COUNT2];
        public static int[] average_faculty_salary = new int[FacultyTemplateConstants.MAX_RANK_AGE_GROUP];
        #endregion

        #region Public Functions
        public void init(int recno);    // calculate vars in PeerSchool based on vars in School

        public void calc_student_pref_vars_array()
        {
            if ( this != school_res.player_peer_school ) 
            {
	            calc_student_pref_vars_array_peer();
	            return;
            }

            Array.Copy( pref_vars_array_last, pref_vars_array, EnrollmentConstants.PREFERENCE_COUNT2);

            // calculate the column Q in [HE.STU.pref.xls]Sheet1
            //
            // recalculate PeerSchool's properties to prefArray to speed up calculation
            // (refer to column L of [HE.initialization]stu_pref)
            // local var: float pref_vars_array[PREFERECNE_COUNT2];

            // (minor bug) the following RHSs may NOT be updated during play

            // assume "this" is the player_peeer_school
            int[] studentEnrollCount= new int[StudentConstants.MAX_STUDENT_LEVEL];
            int i;

            for ( i=0; i<StudentConstants.MAX_STUDENT_LEVEL; i++ ) 
            {
	            studentEnrollCount[i] = enroll_res.student_count[i];
	            percent_female_sl[i] = (char) enroll_res.student_female_percent[i];
	            percent_minority_sl[i] = (char) enroll_res.student_minority_percent[i];
            }

            pref_vars_array[0] = prestige;
            // by 1021 email
            pref_vars_array[1] = (float) athletics_office.ncaa_level_value;
            pref_vars_array[2] = (float)(student_life_staff_salaries+student_life_other_expense)/(full_time_undergrad+part_time_undergrad);
            pref_vars_array[3] = percent_traditional_ug_in_residence_halls / 100.0f;
            pref_vars_array[4] = percent_get_bacc_in_5_year / 100.0f;

            pref_vars_array[5] = enroll_res.applications_rate[(short)StudentLevel.Undergrad];
            pref_vars_array[6] = enroll_res.cur_yield_rate[(short)StudentLevel.Undergrad];
            pref_vars_array[7] = (float)(studentEnrollCount[1]) / (float)(studentEnrollCount[0]+studentEnrollCount[1]);

            float tmpCount = 0;

            pref_vars_array[8] = (float)(studentEnrollCount[0]+studentEnrollCount[1]) / enroll_res.total_student_count;
            pref_vars_array[9] = (float)(studentEnrollCount[4]) / enroll_res.total_student_count;

            // the additional field included in PREFERENCE_COUNT2 but not PREFERENCE_COUNT
            pref_vars_array[10] = (float)(percent_instate_freshmen) / 100;

            for (i = 0, tmpCount = 0; i < StudentConstants.MAX_STUDENT_LEVEL; i++)
            {
                tmpCount += (float)(studentEnrollCount[i]) * percent_female_sl[i] / 100;
            }

            pref_vars_array[11] = (float)(tmpCount) / enroll_res.total_student_count;

            for (i = 0, tmpCount = 0; i < StudentConstants.MAX_STUDENT_LEVEL; i++)
            {
                tmpCount += studentEnrollCount[i] * percent_minority_sl[i] / 100;
            }

            pref_vars_array[12] = (float)(tmpCount) /  enroll_res.total_student_count;
            pref_vars_array[13] = doc_time_to_degree;
            pref_vars_array[14] = sponsored_research_rating;
            pref_vars_array[15] = (float) finance.tuition_rate;

            pref_vars_array[16] = pref_vars_array[15];
            pref_vars_array[17] = percent_ug_students_on_aid / 100.0f;
            pref_vars_array[18] = pref_vars_array[17];
            pref_vars_array[19] = institutional_aid_per_fte;
            pref_vars_array[20] = institutional_aid_per_fte;

            for (i=0; i < EnrollmentConstants.PREFERENCE_COUNT2; i++)
            {
	            if( pref_vars_array[i] < 0 )
                {
                    throw new ArgumentOutOfRangeException("prefs_var_array[" + i.ToString() + "]");
                }
            }
        }

        public void calc_student_pref_vars_array_peer()
        {
            Array.Copy(pref_vars_array_last, pref_vars_array, EnrollmentConstants.PREFERENCE_COUNT2);

            // calculate the column Q in [HE.STU.pref.xls]Sheet1
            //
            // recalculate PeerSchool's properties to prefArray to speed up calculation
            // (refer to column L of [HE.initialization]stu_pref)
            // local var: float pref_vars_array[PREFERECNE_COUNT2];

            // the following RHSs may NOT be updated during play
            pref_vars_array[0] = prestige;
            pref_vars_array[1] = (float) athletics_office.ncaa_level_value;
            pref_vars_array[2] = math.safe_divide( (float)(student_life_staff_salaries+student_life_other_expense), (float)full_time_undergrad+part_time_undergrad );
            pref_vars_array[3] = percent_traditional_ug_in_residence_halls / 100.0f;
            pref_vars_array[4] = percent_get_bacc_in_5_year / 100.0f;

            pref_vars_array[5] = ug_applications_rate;
            pref_vars_array[6] = ug_yield_rate / 100.0f;
            pref_vars_array[7] = math.safe_divide( (float)part_time_undergrad, (float)full_time_undergrad+part_time_undergrad );

            int[] studentEnrollCount = new int[StudentConstants.MAX_STUDENT_LEVEL];
            studentEnrollCount[0] = full_time_undergrad;
            studentEnrollCount[1] = part_time_undergrad;
            studentEnrollCount[2] = enrollment_masters;
            studentEnrollCount[3] = enrollment_doctoral;
            studentEnrollCount[4] = non_degree_seeking;

            float studentCount = 0.0f;
            int i, tmpCount=0;
            for (i = 0; i < StudentConstants.MAX_STUDENT_LEVEL; i++)
            {
                studentCount += (float)studentEnrollCount[i];
            }

            pref_vars_array[8] = math.safe_divide( (int)(full_time_undergrad+part_time_undergrad), (int)studentCount );
            pref_vars_array[9] = math.safe_divide( (int)non_degree_seeking, (int)studentCount );

            // the additional field included in PREFERECNE_COUNT2 but not PREFERECNE_COUNT
            pref_vars_array[10] = percent_instate_freshmen;

            for (i = 0; i < StudentConstants.MAX_STUDENT_LEVEL; i++)
            {
                tmpCount += studentEnrollCount[i] * percent_female_sl[i] / 100;
            }

            pref_vars_array[11] = math.safe_divide( (int)tmpCount, (int)studentCount );

            for (i = 0, tmpCount = 0; i < StudentConstants.MAX_STUDENT_LEVEL; i++)
	        tmpCount += studentEnrollCount[i] * percent_minority_sl[i] / 100;

            pref_vars_array[12] = math.safe_divide( (int)tmpCount, (int)studentCount );
            pref_vars_array[13] = doc_time_to_degree;
            pref_vars_array[14] = sponsored_research_rating;
            pref_vars_array[15] = (float) tuition_rate;

            pref_vars_array[16] = pref_vars_array[15];
            pref_vars_array[17] = percent_ug_students_on_aid / 100.0f;
            pref_vars_array[18] = pref_vars_array[17];
            pref_vars_array[19] = institutional_aid_per_fte;
            pref_vars_array[20] = institutional_aid_per_fte;

            for (i = 0; i < EnrollmentConstants.PREFERENCE_COUNT2; i++)
            {
                if (pref_vars_array[i] < 0)
                {
                    throw new ArgumentOutOfRangeException("prefs_var_array[" + i.ToString() + "]");
                }
            }
        }

        public void calc_student_pref_vars_average_array()
        {
            Array.Copy(pref_vars_average_array_last, pref_vars_average_array, EnrollmentConstants.PREFERENCE_COUNT2);

            float sum;
            short psCount = school_res.peer_school_count;

            // pref_vars_average_array is static
            //
            for (int var = 0; var < EnrollmentConstants.PREFERENCE_COUNT2; var++)
            {
                sum = 0.0f;

                for (int i = 0; i < psCount; i++)
                {
                    PeerSchool ps = school_res.peer_school_array[i];

                    if (ps != school_res.player_peer_school)
                    {
                        sum += ps.pref_vars_array[var];
                    }
                }

                pref_vars_average_array[var] = sum / psCount;
            }
        }

        public static void calc_average_faculty_salary();
        #endregion
    }
}
