//Filename    : OSCHOOL.CPP
//Description : School class - calc_distance() - also see spreadsheet "indices" of HEinitialization
//Owner       : Fred

#include <OSYS.H>
#include <OGAMESET.H>
#include <OPSCHOOL.H>
#include <OSCHLRES.H>
#include <OSCHOOL.H>
#include <OATHLETI.H>

#define PRESTIGE_IMP    7
#define ATHLETIC_PROGRAM_INTENSITY_IMP    9
#define FULLTIME_UG_IN_HOUSING_PERCENT_IMP  6
#define UG_GET_DEGREE_IN_5YEAR_PERCENT_IMP  0
#define PERCENT_STUDENT_IN_STATE_IMP      3
#define STUDENT_COUNT_IMP             4
#define GRADUATE_STUDENT_PERCENT_IMP      6
#define PARTTIME_UNDERGRAD_PERCENT_IMP    2
#define NONDEGREE_STUDENT_PERCENT_IMP     2
#define DOCTORAL_PROGRAM_INTENSITY_IMP    7
#define RELATIVE_WEALTH_IMP           3

//------ defining static temp varialbes for calculation of distance_value -------//
// for L5
static float T_range,U_range,Y_range,Z_range;
static int   N_range,O_range,P_range,V_range;

// for M5 & J5
static float I_range, M_range;
static int   W_range, AJ_range, AC_range, AE_range,AF_range,AG_range,AH_range,AK_range;

// for I5, Z5, K5 respectively
static float K_min,K_max,AA_min,AA_max,L_range;

//----------- Define macro functions ---------//

#define IFZ(x,v)    (!(x)?(v):(x))
#define EXPO2(x)  ((x)*(x))

//-------- Begin of marco function FIND_AVG ------//

#define FIND_AVG(f,num) \
{ \
      sum=0;\
		for (i=sCount-1; i>=sCount-8; i--) \
						       sum+=(sArray+i)->f; \
									       sum /= num; \
											       }

//-------- End of marco function FIND_AVG --------//

//------------ Begin of marco function FIND_MIN_MAX ----------//

#define FIND_MIN_MAX(min,max,field,Type) \
{ \
      Type      tmp; \
			 min = max = sArray->field; \
							for (i=0; i<sCount; i++) \
							    { \
								  sPtr = sArray + i; \
											 tmp = sPtr->field; \
														if ( tmp < min ) \
																     min = tmp; \
																		    else if ( tmp > max ) \
																					      max = tmp; \
																							     } \
																								   }

//------------ End of marco function FIND_MIN_MAX ----------//

//------------ Begin of marco function FIND_RANGE ----------//

#define FIND_RANGE(field,range,Type) \
{ \
      Type  min, max; \
			  FIND_MIN_MAX(min,max,field,Type); \
								range = max - min; \
										       }

//------------ End of marco function FIND_RANGE ----------//

//---------- Begin of function School::calc_distance -----------//
//!
void School::calc_distance_NOPZ5() {
    float AA5 = doctoral_degrees_per_reg_faculty;

    short Q5 = usn_wr_ranking;                      // raw_usn_wr_ranking;
    short R5 = ave_midpt_sat_act;                   // raw_ave_midpt_sat_act;
    int S5 = freshmen_top_10_class;                 // raw_freshmen_top_10_percent_of_class/100;	//BUG? /100

    N5 = IFZ(Q5,300);
    O5 = IFZ(R5,800);
    P5 = IFZ(S5,5);
    Z5 = 9*(AA5-AA_min)/(AA_max-AA_min)+1;          // Doctoral degrees per faculty rating
}

//---------- Begin of function School::calc_distance 1 -----------//
//!
void School::calc_distance_L5() {
    // calc vars for L5
    //
    float T5 = applications_ratio;
    float U5 = float(yield_rate/100.0);
    int V5 = athletics_rating;
    float Y5 = sponsored_research_per_reg_faculty;

  // calc L5 (Raw prestige index)
  //
    L5 = (float)(
	(300-N5)*0.1/N_range
	+O5*0.1/O_range
	+P5*0.1/P_range
	+T5*0.15/T_range
	+U5*0.15/U_range
	+(V5+1)*0.00/V_range                          //BUG?	player_school!F630.00
	+Z5*0.15/Z_range);
}

//------------ End of function School::calc_distance 1 -----------//

//---------- Begin of function School::calc_distance 2 -----------//
//!
void School::calc_distance_K5() {
    // calc K5 (Prestige index/SD) for I5
    //
    K5 = L5/L_range;
}

//------------ End of function School::calc_distance 2 -----------//

//---------- Begin of function School::calc_distance 3 -----------//
//!
void School::calc_distance_prestige() {
    // calc I5 (Prestige) for M5
    //
    prestige = 9*(K5-K_min)/(K_max-K_min)+1;
}

//------------ End of function School::calc_distance 3 -----------//

//---------- Begin of function School::calc_distance 4 -----------//
void School::calc_distance_ACAH5(short  sCount, School* sArray) {
    int i, sum;

    // find AC5
    //
    char  AD5 = freshmen_from_within_the_state_percent;

    // if (this/me >=10)  AC5 = IFZ(AD5, ave_of_the_last_8_ACs)	else AC5 = AD5;	//BUG special case!!!
    if ( sCount >= 10 ) {
	if ( AD5 )
	    AC5 = AD5;
	else {
	    FIND_AVG(AC5,8);
	    AC5 = sum;
	}
    }
    else
	AC5 = AD5;

    //
    //
    char  AI5 = non_degree_seeking_percent;         // excel column name = non_matric
    // if (this/me >=10)  AH5 = IFZ(AI5, ave_of_the_last_8_AHs)	else AH5 = AI5		//BUG special case!!!
    if ( sCount >= 10 ) {
	if ( AD5 )
	    AH5 = AI5;
	else {
	    FIND_AVG(AH5,8);
	    AH5 = sum;
	}
    }
    else
	AH5 = AI5;
}

//------------ End of function School::calc_distance 4 -----------//

//---------- Begin of function School::calc_distance 5 -----------//
//!
void School::calc_distance_M5() {
    // calc M5 (Raw target inst'l distance) for J5
    //
    float I5    = prestige;                         // see calc_distance_prestige
    short V5    = athletics_rating;
    char  AJ5 = ug_in_housing_percent;
    char  W5    = student_get_bacc_in_5_year_percent;
    int AE5 = enrollment_fte;                       //total_student (headcount)
    int AF5 = ug_percent;
    int AG5 = part_time_ug_percent;
    int AK5 = financial_condition;
    // Z5: see above calc_distance_NOPZ5()

    float tmp[11];

    memset( tmp, 0, sizeof(tmp) );

    /* //## chea 180999 org.idea
       tmp[0] = PRESTIGE_IMP *EXPO2((I5-player_school.prestige)/I_range);
       tmp[1] = ATHLETIC_PROGRAM_INTENSITY_IMP *EXPO2(float(V5-athletics_office.ncaa_level_value)/V_range);
       tmp[2] = FULLTIME_UG_IN_HOUSING_PERCENT_IMP *EXPO2(float(AJ5-player_school.fulltime_ug_in_housing_percent)/AJ_range);
       tmp[3] = UG_GET_DEGREE_IN_5YEAR_PERCENT_IMP *EXPO2(float(W5-player_school.ug_get_degree_in_5year_percent)/W_range);
       tmp[4] = PERCENT_STUDENT_IN_STATE_IMP *EXPO2(float(AC5-player_school.percent_student_in_state)/AC_range);
       tmp[5] = STUDENT_COUNT_IMP *EXPO2(float(AE5-player_school.get_player_expected_student_count())/AE_range);

       tmp[6] = GRADUATE_STUDENT_PERCENT_IMP *EXPO2(float(100-AF5-player_school.graduate_student_percent)/AF_range);
       tmp[7] = PARTTIME_UNDERGRAD_PERCENT_IMP *EXPO2(float(AG5-player_school.parttime_undergrad_percent)/AG_range);
       tmp[8] = NONDEGREE_STUDENT_PERCENT_IMP *EXPO2(float(AH5-player_school.nondegree_student_percent)/AH_range);
       tmp[9] = DOCTORAL_PROGRAM_INTENSITY_IMP *EXPO2(float(Z5-player_school.doctoral_program_intensity)/Z_range);
       tmp[10] = RELATIVE_WEALTH_IMP *EXPO2(float(AK5-player_school.relative_wealth_value)/AK_range);
    */

    //## chea 180999 suggest by Bill Massy
    tmp[0] = (PRESTIGE_IMP *EXPO2((I5-player_school.prestige)/I_range)*10);
    if(player_school.prestige == 1 || player_school.prestige == 10)
	tmp[0] = (PRESTIGE_IMP *EXPO2((I5-player_school.prestige)/I_range)*10);

    tmp[1] = ATHLETIC_PROGRAM_INTENSITY_IMP *EXPO2(float(V5-athletics_office.ncaa_level_value)/V_range);
    if(athletics_office.ncaa_level_value == 1.5 || athletics_office.ncaa_level_value == 4.5)
	tmp[1] = ATHLETIC_PROGRAM_INTENSITY_IMP *EXPO2(float(V5-athletics_office.ncaa_level_value)/V_range)*10;

    tmp[2] = FULLTIME_UG_IN_HOUSING_PERCENT_IMP *EXPO2(float(AJ5-player_school.fulltime_ug_in_housing_percent)/AJ_range);
    if(player_school.fulltime_ug_in_housing_percent == 0 || player_school.fulltime_ug_in_housing_percent == 50)
	tmp[2] = (FULLTIME_UG_IN_HOUSING_PERCENT_IMP *EXPO2(float(AJ5-player_school.fulltime_ug_in_housing_percent)/AJ_range))*10;

    tmp[3] = UG_GET_DEGREE_IN_5YEAR_PERCENT_IMP *EXPO2(float(W5-player_school.ug_get_degree_in_5year_percent)/W_range);
    if(player_school.ug_get_degree_in_5year_percent == 50 || player_school.ug_get_degree_in_5year_percent == 100)
	tmp[3] = (UG_GET_DEGREE_IN_5YEAR_PERCENT_IMP *EXPO2(float(W5-player_school.ug_get_degree_in_5year_percent)/W_range))*10;

    tmp[4] = PERCENT_STUDENT_IN_STATE_IMP *EXPO2(float(AC5-player_school.percent_student_in_state)/AC_range);
    if(player_school.percent_student_in_state == 30 || player_school.percent_student_in_state == 70)
	tmp[4] = PERCENT_STUDENT_IN_STATE_IMP *EXPO2(float(AC5-player_school.percent_student_in_state)/AC_range)*10;

    tmp[6] = GRADUATE_STUDENT_PERCENT_IMP *EXPO2(float(100-AF5-player_school.graduate_student_percent)/AF_range);
    if(player_school.graduate_student_percent == 5 || player_school.graduate_student_percent == 35)
	tmp[6] = GRADUATE_STUDENT_PERCENT_IMP *EXPO2(float(100-AF5-player_school.graduate_student_percent)/AF_range)*10;

    tmp[7] = PARTTIME_UNDERGRAD_PERCENT_IMP *EXPO2(float(AG5-player_school.parttime_undergrad_percent)/AG_range);
    if(player_school.parttime_undergrad_percent == 0 || player_school.parttime_undergrad_percent == 50)
	tmp[7] = PARTTIME_UNDERGRAD_PERCENT_IMP *EXPO2(float(AG5-player_school.parttime_undergrad_percent)/AG_range)*10;

    // not from setting
    if(player_school.nondegree_student_percent == 0 || player_school.nondegree_student_percent == 50)

	tmp[9] = DOCTORAL_PROGRAM_INTENSITY_IMP *EXPO2(float(Z5-player_school.doctoral_program_intensity)/Z_range);
    if(player_school.doctoral_program_intensity == 0 || player_school.doctoral_program_intensity == 10)
	tmp[9] = DOCTORAL_PROGRAM_INTENSITY_IMP *EXPO2(float(Z5-player_school.doctoral_program_intensity)/Z_range)*10;

    //	player_school.relative_wealth=2;   //## chea 081099 testing
    //	player_school.relative_wealth_value=2;   //## chea 081099 testing

    /*#if(GAME_VERSION>=200)

      float rich = 5000;
      float middle = 2400;
      float struggle = 1000;

      float wealth[3] = {5000,2400,1000};

      tmp[10] = abs((year_end_capital_reserve - wealth[player_school.relative_wealth])/wealth[player_school.relative_wealth]);

      #else */

    if(player_school.relative_wealth_value == 1)
	tmp[10] = (RELATIVE_WEALTH_IMP *EXPO2(float(AK5-player_school.relative_wealth_value)/AK_range));
    else if(player_school.relative_wealth_value == 2)
	tmp[10] = (RELATIVE_WEALTH_IMP *EXPO2(float(AK5-player_school.relative_wealth_value)/AK_range)) * 4;
    else
	tmp[10] = (RELATIVE_WEALTH_IMP *EXPO2(float(AK5-player_school.relative_wealth_value)/AK_range)) * 10;
    //#endif

    for (int i=0; i<11; i++)
	M5 += tmp[i];
}

//------------ End of function School::calc_distance 5 -----------//

//---------- Begin of function School::calc_distance final -----------//
//!
void School::calc_distance_value() {
    // calc J5 (distance)
    //
    distance_value = M5/(M_range);

    //temp for debug
    //	if ( distance_value <= 0.01 )
    if ( distance_value < 0 ) {
	distance_value = 10.0;
	err_here();
    }
}

//------------ End of function School::calc_distance final -----------//

//---------- Begin of static function School::calc_distance_all_school -----------//
void School::calc_distance_all_school() {
    short     sCount = school_res.desired_school_count;
    School*   sArray = school_res.desired_school_array;

    err_when(sCount < 10 || !sArray);
    // update class/global temp vars for Z5
    AA_min = AA_max = sArray->doctoral_degrees_per_reg_faculty;

    School      *sPtr;
    short     i;
    float     tmp;

    for (i=0; i<sCount; i++) {
	sPtr = sArray + i;
	tmp = sPtr->doctoral_degrees_per_reg_faculty;
	if ( tmp < AA_min)
	    AA_min = tmp ;
	else if ( tmp > AA_max)
	    AA_max = tmp;
    }

    // update object temp vars for L5
    for (i=0; i<sCount; i++) {
	sPtr = sArray + i;
	sPtr->calc_distance_NOPZ5();
    }

    // update class/global temp vars for L5
    FIND_RANGE(N5,N_range,int);
    FIND_RANGE(O5,O_range,int);
    FIND_RANGE(P5,P_range,int);
    FIND_RANGE(Z5,Z_range,float);

    FIND_RANGE(applications_ratio,T_range,float);
    FIND_RANGE(yield_rate,U_range,float);
    U_range /= 100;                                 // floating point percent
    FIND_RANGE(athletics_rating,V_range,int);
    // BUG in data:
    FIND_RANGE(sponsored_research_per_reg_faculty,Y_range,float);
    //TEMP set to value in column Y in "Indices"
    //Y_range = float(476.52);

    // update object temp vars for K5
    for (i=0; i<sCount; i++) {
	sPtr = sArray + i;
	sPtr->calc_distance_L5();
    }

    // update class/global temp vars for K5
    FIND_RANGE(L5,L_range,float);

    // update object temp vars for I5 (prestige)
    for (i=0; i<sCount; i++) {
	sPtr = sArray + i;
	sPtr->calc_distance_K5();
    }

    // update class/global temp vars for I5 (prestige)
    FIND_MIN_MAX(K_min, K_max, K5, float);

    // update object vars for M5
    for (i=0; i<sCount; i++) {
	sPtr = sArray + i;
	sPtr->calc_distance_prestige();
	sPtr->calc_distance_ACAH5(i, sArray);
    }

    // update class/global temp vars for M5
    FIND_RANGE(prestige, I_range, float);
    //OR char
    FIND_RANGE(ug_in_housing_percent, AJ_range, int);
    FIND_RANGE(student_get_bacc_in_5_year_percent, W_range, int);
    FIND_RANGE(AC5,AC_range,int);
    FIND_RANGE(enrollment_fte,AE_range,int);
    FIND_RANGE(ug_percent,AF_range,int);
    FIND_RANGE(part_time_ug_percent,AG_range,int);
    FIND_RANGE(AH5,AH_range,int);
    FIND_RANGE(financial_condition,AK_range, int);

    // update object vars M5 for distance_value
    for (i=0; i<sCount; i++) {
	sPtr = sArray + i;
	sPtr->calc_distance_M5();
    }

    // update class/global temp vars for distance_value
    FIND_RANGE(M5,M_range,float);

    // update object vars: distance_value
    for (i=0; i<sCount; i++) {
	sPtr = sArray + i;
	sPtr->calc_distance_value();
    }
}

//------------ End of function School::calc_distance MAIN -----------//

// --- Begin of function School::calc_distance_first --- //
//
void School::calc_distance_first() {
    int AE5 = enrollment_fte;                       //total_student (headcount)
    School*     sArray = school_res.desired_school_array;
    short     sCount = school_res.desired_school_count;
    float max = 0;
    float min = 9999;
    float sumSquare=0;
    float sumSingle=0;

    for ( int i=0; i<sCount; i++ ) {
	if ( sArray[i].enrollment_fte> max ) {
	    max = sArray[i].enrollment_fte;
	}
	if ( sArray[i].enrollment_fte < min ) {
	    min = sArray[i].enrollment_fte;
	}
	sumSquare += EXPO2( sArray[i].enrollment_fte);
	sumSingle += sArray[i].enrollment_fte;
    }

    float sd;

    sd = m.sqrt(( sumSquare - EXPO2( sumSingle )/sCount )/sCount);

    float range = sd;

    //	if ( desired_recno == 494 || desired_recno == 53 )
    //		range = sd;

    float distanceValueStudentCount;

    distanceValueStudentCount = EXPO2(float(AE5-player_school.get_player_expected_student_count())/range);

    float Y5 = sponsored_research_per_reg_faculty;

    max = 0;
    min = 9999;

    for ( int i=0; i<sCount; i++ ) {
	if ( sArray[i].sponsored_research_per_reg_faculty > max )
	    max = sArray[i].sponsored_research_per_reg_faculty;
	if ( sArray[i].sponsored_research_per_reg_faculty < min )
	    min = sArray[i].sponsored_research_per_reg_faculty;
    }

    range = max - min;

    float distanceValueSponsoredResearch;

    distanceValueSponsoredResearch = EXPO2(float(player_school.get_player_expected_sponsored_research()) - 9*Y5/range);

    distance_value = 0.67*distanceValueStudentCount + 0.33*distanceValueSponsoredResearch;
}

//
// --- End of function School::calc_distance_first --- //
