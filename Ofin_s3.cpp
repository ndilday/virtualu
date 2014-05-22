//Filename    : OFINANCE.CPP
//Description : Finance class: stage3.  Algorithm from email Q32* and reQ32*
//Owner       : Fred

#include <STDIO.H>                                //temp for debug matrix
#include <OLOG.H>                                 // for debug matrix
#include <OMATH.H>

#include <ODEPT.H>
#include <ODEPTRES.H>
#include <OFACURES.H>

// CU header file
#include <OLinAlg.h>                              // for its quadratic_prog() and class Matrix
#include <OFINANCE.H>
#include <OSYS.H>                                 //## chea
#include <OBOX.H>                                 //## chea

//#define DEBUG_VC
//#define TEST_MATHCA

//---------- Begin of static function prt_vector -----------//
static void print_vector(const ColumnVector &xNames) {
#ifdef DEBUG_VC
    char s[300];
    sprintf(s, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f",
	    xNames(1),xNames(2),xNames(3),
	    xNames(4),xNames(5),xNames(6),
	    xNames(7),xNames(8),xNames(9),xNames(10));
    DEBUG_LOG(s);
#endif
}

static void print_vector2(const ColumnVector &xNames) {
#ifdef DEBUG_VC
    int count = xNames.Storage();
    String s = "->";
    for (int i=1; i<=count; i++ ) {
	s += ", ";
	s += m.format(xNames(i),0);
    }
    DEBUG_LOG(s);
#endif
}

//------ Begin of function sort_teaching_load_function ------//
//!
//! Sort ?  in ?_array by their teaching load in an decending order.
//!
static int sort_teaching_load_function( const void *a, const void *b ) {
    // 1st sort key

    float valueA = department_array[*((char*)a)]->faculty_array.teaching_contact_hour[THIS_TRIMESTER-1];
    float valueB = department_array[*((char*)b)]->faculty_array.teaching_contact_hour[THIS_TRIMESTER-1];

    float rc = valueA - valueB;

    if( rc < 0 )
	return 1;
    else if( rc > 0 )
	return -1;
    else
	return 0;
}

//------- End of function sort_teaching_load_function ------//

//---------- Begin of function Finance::calc_max_hires -----------//
//!
//!  calc_max_hires depend on real staff salary growth in oop1 && Faculty expense in oop2
//!
//!
void Finance::calc_max_hires() {
	// remove stage1_revenue, stage1_expense, stage2_expense
    double growthRate, newValue; {

	// = F203/100
	growthRate = cost_rise_policy_array[AC_ACADEMIC_DEPARTMENTS].result_value / 100;

	double researchNewValue = 0.0;                // will be canceled out, so no need to find stage1_revenue[S1_DIRECT_SPONSORED_RESEARCH].new_value
	double dept_faculty_new_price = prior_dept_fac_exp * (1+inflation_rate/100.0+expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value/100.0);
	//double change_value = dept_faculty_new_price * cost_rise_policy_array[AC_ACADEMIC_DEPARTMENTS].result_value + researchNewValue;
	double change_value = dept_faculty_new_price * cost_rise_policy_array[AC_ACADEMIC_DEPARTMENTS].result_value/100.0 + researchNewValue;
	newValue = dept_faculty_new_price + change_value - researchNewValue;
    }

    // growthAmount = newValue - oldValue
    double growthAmount = newValue - newValue / (1.0+growthRate);

    //------- calculate the average salary ---------//

    int totalSalary = 0;
    int facCount = 0;

    for (int i=department_array.department_count; i>0; i--) {
	Department* deptPtr = department_array[i];

	int facultyNo = deptPtr->faculty_array.faculty_count;

	// get the total salary among all of the faculty
	for ( int i=facultyNo; i>0; i-- ) {
	    if ( deptPtr->faculty_array.is_deleted(i) )
		continue;

	    Faculty* facPtr = deptPtr->faculty_array[i];

	    totalSalary += facPtr->salary;
	}

	facCount += deptPtr->faculty_array.faculty_count;
    }

    int avgSalary = totalSalary / facCount;

    //----- calc # of departures in the past year ------//

    int departureCount = max(0, department_array.faculty_level_history[FACULTY_RANK_LEVEL_COUNT][THIS_YEAR]
			     - facCount);

    //-------------------------------------//
    max_hires = int(growthAmount*1000) / avgSalary + departureCount;

    max_hires = max(0, max_hires);
    // 0116
    max_hires = min(MAX_S3_SLIDER * department_array.department_count, max_hires);

    /*
      //	maxHires = costLimit /
      //		( sum_over_depts{{E153*(1+F203)}[d]*target[d]}/sum_over_depts{TARGET[d]} )
      // where E153 is the average salary for fullProf2 in the department.

      //-------//

      int totalSalary = 0;
      int avgSalary = 0;
      int facCount = 0;
      max_hires = 0; //## chea to remove the pervious value

      for (int i=department_array.department_count; i>0; i--)
      {
      Department* deptPtr = department_array[i];

      totalSalary += (deptPtr->buget_average_salary>0) ? deptPtr->buget_average_salary: int(deptPtr->peer_numeraire_salary * deptPtr->peer_salary_multiplier_rank[ASST_PROF]) *
      deptPtr->faculty_array.faculty_count;

      facCount += deptPtr->faculty_array.faculty_count;
      }

      //-------//

      int departureCount=0;

      departureCount = max(0, department_array.faculty_level_history[FACULTY_RANK_LEVEL_COUNT][THIS_YEAR]
      - facCount);

      //-------//

      avgSalary = totalSalary / facCount;

      // 990524 max_hires = (int) max(*0, departureCount + math.safe_divide(cost_limit*1000, (float)avgSalary));//## chea old max_hires calc
      max_hires = (int) max(0.0f, math.safe_divide(cost_limit*1000, (float)avgSalary)); //min & max bug chea //## BUGHERE alg. has got A PROBLEMS

      max_hires = min(MAX_S3_SLIDER * department_array.department_count, max_hires);	// 0116
    */
}

//---------- End of function Finance::calc_max_hires -----------//

//---------- Begin of function Finance::pre_optimization_stage_3 -----------//
//!
//! called after departure and just before optimization interface
//!
bool Finance::pre_optimization_stage_3(bool init) {

    // 990507
    //	if ( !init )
    //## chea 081099 this can't be called during opt. because it alter tuition_rate but may cause bug.BUGHERE
    //		finance.calc_budget_report();		//BUGHER double-check the call order!

    //---- init ----//

    Department *deptPtr;
    int i, openToHire = 10, max = 0;

    total_hires = 0;
    /*
      for (i=department_array.department_count-1; i>=0; i--)
      {
      //finance.hiring_policy_array[i].upper_bound = 0;
      //finance.hiring_policy_array[i].target_value = 0;
      //finance.hiring_policy_array[i].result_value = 0;
      //finance.hiring_policy_array[i].weight = OPT_MID_WEIGHT;
      }
    */

    //---------------------------------//

    // 990507 department_array.salary_setting();		// bug if it runs more than once each year.

    //---------------------------------//

    /*
      In email 990525:

      5. The basic theory of this part of the model is fairly simple. Here it is
      in my terms, not the variable names in the code. It's possible I made a
      mistake somewhere in the specs (perhaps involving sponsored research), so
      please check that the code does this job and change it if it doesn't.

      prior_dept_fac_exp = prior year's departmental faculty salary actual

      budget_for_old_billets_at_new_salaries =
      prior_dept_fac_exp*(1+S1.fac_sal_gr%/100) [from S1]

      adj_for_change_in_billets_at_new_salaries =
      prior_dept_fac_exp*(S2.pct_change_in_fac/100)*(1+S1.fac_sal_gr%/100)

      (note that S2.pct_change_in_fac can be positive or negative).

      new_deptl_fac_sal_budget =
      budget_for_old_billets_at_new_prices + adj_for_change_in_billets_at_new_salaries

      S3.costLimit = new_deptl_fac_sal_budget -
      new_salaries_of_continuing_faculty

      (Continuing faculty are those remaining after attrition.)

    */

    const double& s1Change = expense_policy_array[PL_FACULTY_SALARY_INCREASES].result_value;
    const double& s2Change = cost_rise_policy_array[PL_FACULTY_FTE].result_value;

    double budgetForOldBilletsAtNewSalaries =
	prior_dept_fac_exp * (1 + s1Change/100);

    double adjForChangeInBilletsAtNewSalaries =
	prior_dept_fac_exp * (s2Change/100) * (1 + s1Change/100);

    //---------------------------------//
    // update for optimize_policy_3

    float totalCost = 0;                            // not $000
    int totalFacCount = 0;

    for (i=department_array.size(); i>0; i--) {
	if ( department_array.is_deleted(i) )
	    continue;

	int facCount=0;
	float total2FacultySalary = 0;                // not $000

	deptPtr = department_array[i];

	for ( int f=deptPtr->faculty_array.size(), totalFacultySalary=0; f>0; f-- ) {
	    if ( deptPtr->faculty_array.is_deleted(f) )
		continue;

	    totalFacCount++;

	    Faculty* facPtr = deptPtr->faculty_array[f]; {

		//0407  if ( deptPtr->faculty_array[f]->rank_age_group() == FULL_PROF+1 )		// fullProf2
		total2FacultySalary += facPtr->salary;
		facCount++;
	    }

	    totalCost += facPtr->salary;
	}

	// avg_salary: $000
	finance.hiring_policy_array[i-1].avg_salary = math.safe_divide(total2FacultySalary, facCount * 1000.0f);

	finance.hiring_policy_array[i-1].fac_count = deptPtr->faculty_array.faculty_count;

	//finance.hiring_policy_array[i-1].target_value = facCount;
    }

    // totalFacCount != 0
    err_when(totalCost<=0);

    totalCost /= 1000.0f;
    //990518 cost_limit = max(0, float(budget_expense_array[AC_ACADEMIC_DEPARTMENTS].faculty - totalCost ));
    //990525 cost_limit = float(budget_expense_array[AC_ACADEMIC_DEPARTMENTS].faculty - totalCost );
    cost_limit = float(budgetForOldBilletsAtNewSalaries + adjForChangeInBilletsAtNewSalaries
		       -totalCost);

    //-------------------------//
    calc_max_hires();

    total_hires = max_hires;                        // 0116

    //---------// 0423 0507
    /*
      for (i=department_array.size(); i>0; i--)
      {
      if ( department_array.is_deleted(i) )
      continue;

      int* targetV = &(finance.hiring_policy_array[i-1].target_value);
      *targetV = int(0.99 + max_hires * (*targetV) / totalFacCount);
      }
    */
    //-------------// 990507

    //## chea set upper bound to max
    //  	for (int r=0; r<department_array.department_count; r++ )	// give new hire to the dept(s) with largest teaching-load
    //	{
    //		HiringPolicy* finPr2 = &(finance.hiring_policy_array[r]);
    //		finPr2 -> upper_bound = 10;
    //	}

    const int minPerDept = max_hires / department_array.department_count;
    const int remainedHires = max_hires % department_array.department_count;

    /*
      for (i=department_array.size(); i>0; i--)
      {
      if ( department_array.is_deleted(i) )
      continue;

      int* targetV = &(finance.hiring_policy_array[i-1].target_value);
      *targetV = minPerDept;
      }
    */

    if ( remainedHires >= 0 ) {
	char* deptSortArray;

	deptSortArray = (char*) mem_add(sizeof(char) * department_array.department_count);

	for (i=department_array.department_count; i>0; i--)
	    deptSortArray[i-1] = i;
	qsort( deptSortArray, department_array.department_count, sizeof(deptSortArray[0]), sort_teaching_load_function );

	i=0;

	//## ho 290799 BUGHERE

	/*
	  for (int j=remainedHires; j>0 && i<department_array.department_count; j--, i++ )	// give new hire to the dept(s) with largest teaching-load
	  {
          HiringPolicy* finP = &(finance.hiring_policy_array[ deptSortArray[i] - 1 ]);

          if ( finP->target_value >= finP->upper_bound )
          {
	  j++;
          }
          else
	  finP->target_value++;

	  }
	*/

	//## chea set upper bound to target
	// give new hire to the dept(s) with largest teaching-load
	for (int r=0; r<department_array.department_count; r++ ) {
	    HiringPolicy* finPr2 = &(finance.hiring_policy_array[r]);
	    if(finPr2 -> target_value >= finPr2 -> upper_bound)
		finPr2 -> target_value = finPr2 -> upper_bound;
	    finPr2 -> result_value = finPr2 -> target_value;
	}

	mem_del(deptSortArray);
    }

    return true;
}

//---------- End of function Finance::pre_optimization_stage_3 -----------//

//---------- Begin of function Finance::optimize_policy_3 -----------//
//!
bool Finance::optimize_policy_3() {
    Department *deptPtr;
    int i;
    int deptCount = department_array.department_count;

#ifdef TEST_MATHCA
    deptCount = 10;
#endif

    const double weight_intrinsic = 0.45f;          // c148 Weight on intrinsic value (v. TL)
    //const double weight_response = 0.7f;	// c149 Weight on responsiveness (v. stability)

    err_when(deptCount<=0);

    //----------------------------//

    //ColumnVector avgSalary(deptCount);		// column E 153:162		// $000
    //ColumnVector facCount(deptCount);		// column I 153:162

    //----------------------------// check player input

    //## chea check the total_hires & totaltarget
    int totalupper = 0;
    for (int r=0; r<department_array.department_count; r++ ) {
	HiringPolicy* finPr2 = &(finance.hiring_policy_array[r]);
	totalupper += finPr2 -> upper_bound;
    }

    if (totalupper < total_hires) {
	box.msg("The total of your upper bound values is less than the total faculty you wanted to be hired. ");
	return false;
    }
    //## chea check the total_hires & totaltarget

    ColumnVector targetHire(deptCount), weightsIV3(deptCount), upperBound(deptCount);

    for (i=deptCount; i>0; i--) {
	err_if ( department_array.is_deleted(i) )
	    err_here();

	deptPtr = department_array[i];

	DepartmentInfo* deptInfo = department_res[deptPtr->department_id];

	weightsIV3(i) = (double)1/(double)max( 1,deptPtr->faculty_array.size() );

	//## chea try to avoid the bug below
	if (finance.hiring_policy_array[i-1].upper_bound< finance.hiring_policy_array[i-1].target_value) {
	    finance.hiring_policy_array[i-1].upper_bound = finance.hiring_policy_array[i-1].target_value;
	}

	upperBound(i) = finance.hiring_policy_array[i-1].upper_bound;

	targetHire(i) = finance.hiring_policy_array[i-1].target_value;

	err_when(upperBound(i) < targetHire(i));
    }

#ifdef TEST_MATHCA
    double dummyArr[] = {3,7,13,2,2, 1,5,0,1,4};
    double dummyW[] = {1.000,1.135,1.450,1.000,0.955,1.000,1.000,0.775,1.000,1.000};

    targetHire << dummyArr;
    upperBound << dummyArr;
    weightsIV3 << dummyW;
#endif

    //----------------------------// construct quad_program input matrices PART A

    ColumnVector c(deptCount);
    DiagonalMatrix Q(deptCount);

    c = -SP(targetHire, weightsIV3);
    Q.set_diagonal(weightsIV3);

    //----------------------------//	PART B

    //	for (i=1; i <=deptCount; i++)
    //	{
    //		surplusRHS(1) += avgSalary(i) * facCount(i)  * (1+growthRate);
    //		surplusCoefs3(i) = avgSalary(i) * (1+growthRate);
    //	}

    //	{
    //		double maxSavingAtNewSalary = 0;		// J177

    //		for (i=1; i <=deptCount; i++)
    //		{
    //			maxSavingAtNewSalary += (lowerBound3(i) - facCount(i)) * avgSalary(i) * (1+growthRate);
    //		}

    //		double C136 = finance.cost_rise_policy_array[PL_FACULTY_FTE].result_value / 100;
    //		double C203 = finance.stage1_expense[S1_FACULTY_SALARY_INCREASES].base_value;
    //
    //		#ifdef TEST_MATHCA
    //		surplusRHS(1) += -520.327f;
    //		#else
    //		surplusRHS(1) += max(C136*C203,maxSavingAtNewSalary);	// += i178 where i178=MAX(C136*C203,J177)
    //		#endif
    //	}

    DiagonalMatrix I(deptCount);  I = 1;
    Matrix A = -I;
    ColumnVector b = -upperBound;

    err_when(total_hires<0);                        // check player input

    RowVector tmpList(deptCount); tmpList = 1;
    ColumnVector totalHires(1); totalHires(1) = total_hires;

#ifdef TEST_MATHCA
    err_here();
    totalHires(1) = -10;
    for (i=1;i<=deptCount;i++)
	totalHires(1) += targetHire(i);
#endif

    A = A & -tmpList;
    b = b & -totalHires;

    //----------------------------//	Go run it!

    ColumnVector xNames(deptCount);
    if (!LinearAlgebra::quadratic_prog(c,Q,A,b,xNames))
	return false;

    print_vector2(xNames);

    //----------------------------//

    //int deptCount = department_array.size();
    int resultTotal = 0;                            // 990423
    //int resultArr[MAX_DEPARTMENT];

    // use following procedures :
    // i) find the largest (remaining) value in xNames and round it to the nearest integer.
    //    For ties, just use the department that comes up first in the search
    // ii) if that integer >= 1 put it into the Results vector ELSE put 1 in the result vector
    // iii) repeat from (i) until sum of results equals maxHires. If the value in the (ii)
    // is greate than the remaining hires, use the remaining hires instead of (ii)

    int deptRank;
    double lastXNames;
    int lastDept=0;

    for(deptRank=0; deptRank<deptCount; ++deptRank) {
	int curDept=0;
	double curXNames;
	for( i=1; i<=deptCount; ++i ) {
	    double xNamesValue = xNames(i);

	    // this department has been searched
	    if( lastDept != 0
		&& (xNamesValue > lastXNames || xNamesValue == lastXNames && i <= lastDept) )
		continue;

	    // find the department with maximum xNames
	    if( curDept != 0 && xNamesValue <= curXNames )
		continue;

	    curDept = i;
	    curXNames = xNamesValue;
	}

	// find a department, mark it so next for(deptRank) won't scan it again
	i = lastDept = curDept;
	lastXNames = curXNames;

	int xNamesInt = int( curXNames + 0.5 );       //round to integer

	// at least get 1 place
	// or use ceil((total_hires-resultTotal)/(deptCount-deptRank)), if 1 can't fill up all the vacancies
	xNamesInt = max( xNamesInt, 1 );

	// check remaining
	err_when( resultTotal > total_hires );
	if( xNamesInt > total_hires - resultTotal )
	    xNamesInt = total_hires - resultTotal;

	// place the result and add the total

	finance.hiring_policy_array[curDept-1].result_value = xNamesInt;
	resultTotal += xNamesInt;
    }


    //## chea fix the rounding error

    int counter =0;

    if (resultTotal > total_hires) {
	counter = resultTotal - total_hires;

	//## chea set upper bound to target
	for (int q=0; q<department_array.department_count; q++ ) {
	    HiringPolicy* finPr5 = &(finance.hiring_policy_array[q]);

	    if (finPr5 -> result_value >=1) {
		finPr5 -> result_value --;
		counter--;
	    }

	    if (counter == 0) break;

	}

	resultTotal = total_hires;
    }

    // readjust result_value may cause bug

    int diff = total_hires - resultTotal;
    int deptRecno = 0;

    for (i=0; i<diff; i++) {

	finance.hiring_policy_array[deptRecno].result_value ++;

	if(finance.hiring_policy_array[deptRecno].result_value > 10) {
	    finance.hiring_policy_array[deptRecno].result_value --;
	    diff++;
	}

	if ( ++deptRecno == deptCount )
	    deptRecno = 0;
    }

    //## chea set upper bound to target
    for (int cq=0; cq<department_array.department_count; cq++ ) {
	HiringPolicy* finPr3 = &(finance.hiring_policy_array[cq]);
	//		finPr3 -> target_value = finPr3 -> result_value;				//BUGHERE
	if (finPr3 -> upper_bound < finPr3 -> result_value)
	    finPr3 -> upper_bound = finPr3 -> result_value;
    }

    return true;
}

//---------- End of function Finance::optimize_policy_3 -----------//
