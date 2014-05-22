//Filename    : OGAMESTR.CPP
//Description : Class GameStr

#include <OGAMESTR.H>
#include <GAMEDEF.H>

//---------- Begin of function GameStrRes::rank_str -----------//
//!
char* GameStrRes::rank_str(int rankId) {
    char* strArray[] = {
	"Assistant professor",
	"Associate professor",
	"Full professor",
	"Long-term adjunct",
	"Short-term adjunct",
    };

    return strArray[rankId];
}

//---------- End of function GameStrRes::rank_str -----------//

//---------- Begin of function GameStrRes::gender_ethnic_str -----------//
//!
char* GameStrRes::gender_ethnic_str(int genderEthnicGroup) {
    char* strArray[] = {
	"Nonminority male",
	"Nonminority female",
	"Minority male",
	"Minority female",
    };

    return strArray[genderEthnicGroup];
}

//---------- End of function GameStrRes::gender_ethnic_str -----------//

//---------- Begin of function GameStrRes::trimester_str -----------//
//!
char* GameStrRes::trimester_str(int trimesterId) {
    char* strArray[] = {
	"Fall",
	"Winter",
	"Summer",
    };

    return strArray[trimesterId];
}

//---------- End of function GameStrRes::trimester_str -----------//

//---------- Begin of function GameStrRes::financial_report_str -----------//
char * GameStrRes::balance_sheet_str(int rowId) {
    // order follows to ofinance.h
    char* strArray[] = {
	"  Operating reserve",
	"  Capital reserve",                          //### fred ###//
	"  Endowment at market value",
	"  Academic plant replacement value",
	"  Residence halls",
	"  Quasi endowment at market value",
	"Total assets",
	"  General plant debt",
	"  Residence hall debt",
	"Total liabilities",
	"",
	"",
	};

	return strArray[rowId];
    }
    //---------- End of function GameStrRes::financial_report_str -----------//

  //---------- Begin of function GameStrRes::financial_report_str -----------//
    char * GameStrRes::financial_report_str(int rowId) {
	char* strArray[] = {
	    "  Gross tuition income",
	    "    Financial aid",
	    "  Net tuition income",
	    "  Sponsored research",
	    "  Gifts",
	    "  Endowment spending",
	    "  Intercollegiate athletics",
	    "  Other operating inc. - auxilary losses",
	    "  Interest earned or paid on operating reserve",
	    "  State appropriation",

	    //		"Total sources of funds",

	    "  Academic departments",
	    "  Sponsored research",
	    "  Library resources",                      //## chea 180899 change by b.massy 5.1.2
	    "  Information technology resources",
	    "  Student life",
	    "  Athletics",

	    //		"  Institutional advancement",
	    "  Development and alumni affairs",
	    "  Administration",
	    "  Operations and maintenance",
	    "  Enrollment management",

	    //		"Total operating expenditures",

	    "  Service on general plant debt",
	    "  Transfer to capital reserve",

	    //		"Total uses of funds",
	};

	return strArray[rowId];
    }
    //---------- End of function GameStrRes::financial_report_str -----------//

  //########## being Norris 0625 ############//

  //-------- Begin of function GameStrRes::opt_stage1_str ------------//
  //
    char *GameStrRes::opt_stage1_str(int rowId) {
	char *strArray[] = {
	    // Revenue policies

	    "Real growth of tuition rate",
	    "Real financial aid growth",                //"Policy change to financial aid budget",
	    "Endowment spending rate",
	    "Indirect cost rate",

	    // Cost rise and net growth

	    "Real faculty salary growth",
	    "Real staff salary growth",
	    //		"Other budget growth",
	    "Real operating budget growth",
	    //		"Other cost-rise provision",
	    "Transfer to plant",                        //"Real transfer to plant growth",
	    "Surplus or deficit",
	};

	return strArray[rowId];
    }
    //------- End of function GameStrRes::opt_stage1_str ----------//

  //-------- Begin of function GameStrRes::opt_stage2_str ------------//
  //
    char *GameStrRes::opt_stage2_str(int rowId) {
	char *strArray[] = {
	    // Change net of cost-rise

	    "Faculty numbers",
	    "Nonfaculty departmental expense",
	    "Libraries",                                //" resources",
	    "Information technology",                   //" resources",
	    //		"Libraries & information resources",
	    "Student life",
	    "Athletics",
	    //		"Institutional advancement",
	    "Development and alumni affairs",           //## chea 180899 change by b.massy 5.1.1
	    "Administration (G&A)",
	    "Operations and maintenance (O&M)",
	    "Enrollment management",
	};

	return strArray[rowId];
    }
    //------- End of function GameStrRes::opt_stage2_str ----------//

  //########### end Norris 0625 ##########//

  // #### begin Gilbert 10/10/2001 #####//
  // UG_TRADITION, UG_NONTRADITION, MASTER, DOCTOR, DISTANCE_LEARN
    char* GameStrRes::student_level_str(int studentLevel) {
	char *strArray[] = {
	    "traditional undergraduate",                // UG_TRADITION
	    "nontraditional undergraduate",             // UG_NONTRADITION
	    "Master's",                                 // MASTER
	    "Doctoral",                                 // DOCTOR
	    "distance-learning",                        // DISTANCE_LEARN
	};

	return strArray[studentLevel];
    }
    // #### end Gilbert 10/10/2001 #####//
