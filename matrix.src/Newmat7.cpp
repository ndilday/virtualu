//Owner: Fred
//$$ newmat7.cpp     Invert, solve, binary operations

// Copyright (C) 1991,2,3,4: R B Davies

#include "include.h"

#include "newmat.h"
#include "newmatrc.h"

#ifdef use_namespace
namespace NEWMAT {
#endif

#ifdef DO_REPORT
#define REPORT { static ExeCounter ExeCount(__LINE__,7); ++ExeCount; }
#else
#define REPORT {}
#endif

    /***************************** solve routines ******************************/

    GeneralMatrix* GeneralMatrix::MakeSolver() {
	REPORT
	    GeneralMatrix* gm = new CroutMatrix(*this);
	MatrixErrorNoSpace(gm); gm->ReleaseAndDelete(); return gm;
    }

    GeneralMatrix* Matrix::MakeSolver() {
	REPORT
	    GeneralMatrix* gm = new CroutMatrix(*this);
	MatrixErrorNoSpace(gm); gm->ReleaseAndDelete(); return gm;
    }

    void CroutMatrix::Solver(MatrixColX& mcout, const MatrixColX& mcin) {
	REPORT
	    int i = mcin.skip; Real* el = mcin.data-i; Real* el1 = el;
	    while (i--) *el++ = 0.0;
	    el += mcin.storage; i = nrows - mcin.skip - mcin.storage;
	    while (i--) *el++ = 0.0;
	    lubksb(el1, mcout.skip);
    }

    // Do we need check for entirely zero output?

    void UpperTriangularMatrix::Solver(MatrixColX& mcout,
				       const MatrixColX& mcin) {
	REPORT
	    int i = mcin.skip-mcout.skip; Real* elx = mcin.data-i;
	while (i-- > 0) *elx++ = 0.0;
	int nr = mcin.skip+mcin.storage;
	elx = mcin.data+mcin.storage; Real* el = elx;
	int j = mcout.skip+mcout.storage-nr; int nc = ncols-nr; i = nr-mcout.skip;
	while (j-- > 0) *elx++ = 0.0;
	Real* Ael = store + (nr*(2*ncols-nr+1))/2; j = 0;
	while (i-- > 0) {
	    elx = el; Real sum = 0.0; int jx = j++; Ael -= nc;
	    while (jx--) sum += *(--Ael) * *(--elx);
	    elx--; *elx = (*elx - sum) / *(--Ael);
	}
    }

    void LowerTriangularMatrix::Solver(MatrixColX& mcout,
				       const MatrixColX& mcin) {
	REPORT
	    int i = mcin.skip-mcout.skip; Real* elx = mcin.data-i;
	while (i-- > 0) *elx++ = 0.0;
	int nc = mcin.skip; i = nc+mcin.storage; elx = mcin.data+mcin.storage;
	int nr = mcout.skip+mcout.storage; int j = nr-i; i = nr-nc;
	while (j-- > 0) *elx++ = 0.0;
	Real* el = mcin.data; Real* Ael = store + (nc*(nc+1))/2; j = 0;
	while (i-- > 0) {
	    elx = el; Real sum = 0.0; int jx = j++; Ael += nc;
	    while (jx--) sum += *Ael++ * *elx++;
	    *elx = (*elx - sum) / *Ael++;
	}
    }

    /******************* carry out binary operations *************************/

    static GeneralMatrix*
    GeneralAdd(GeneralMatrix*,GeneralMatrix*,AddedMatrix*,MatrixType);
    static GeneralMatrix*
    GeneralSub(GeneralMatrix*,GeneralMatrix*,SubtractedMatrix*,MatrixType);
    static GeneralMatrix*
    GeneralMult(GeneralMatrix*,GeneralMatrix*,MultipliedMatrix*,MatrixType);
    static GeneralMatrix*
    GeneralSolv(GeneralMatrix*,GeneralMatrix*,BaseMatrix*,MatrixType);
    static GeneralMatrix*
    GeneralSP(GeneralMatrix*,GeneralMatrix*,SPMatrix*,MatrixType);
    static GeneralMatrix*
    GeneralElmDivide(GeneralMatrix*,GeneralMatrix*,ElmDivideMatrix*,MatrixType);

    GeneralMatrix* AddedMatrix::Evaluate(MatrixType mt) {
	REPORT
	    gm1=((BaseMatrix*&)bm1)->Evaluate();
	gm2=((BaseMatrix*&)bm2)->Evaluate();
#ifdef TEMPS_DESTROYED_QUICKLY
	GeneralMatrix* gmx;
	Try { gmx = GeneralAdd(gm1,gm2,this,mt); }
	CatchAll { delete this; ReThrow; }
	delete this; return gmx;
#else
	return GeneralAdd(gm1,gm2,this,mt);
#endif
    }

    GeneralMatrix* SubtractedMatrix::Evaluate(MatrixType mt) {
	REPORT
	    gm1=((BaseMatrix*&)bm1)->Evaluate();
	gm2=((BaseMatrix*&)bm2)->Evaluate();
#ifdef TEMPS_DESTROYED_QUICKLY
	GeneralMatrix* gmx;
	Try { gmx = GeneralSub(gm1,gm2,this,mt); }
	CatchAll { delete this; ReThrow; }
	delete this; return gmx;
#else
	return GeneralSub(gm1,gm2,this,mt);
#endif
    }

    GeneralMatrix* MultipliedMatrix::Evaluate(MatrixType mt) {
	REPORT
	    gm2 = ((BaseMatrix*&)bm2)->Evaluate();
	gm2 = gm2->Evaluate(gm2->Type().MultRHS());   // no symmetric on RHS
	gm1=((BaseMatrix*&)bm1)->Evaluate();
#ifdef TEMPS_DESTROYED_QUICKLY
	GeneralMatrix* gmx;
	Try { gmx = GeneralMult(gm1, gm2, this, mt); }
	CatchAll { delete this; ReThrow; }
	delete this; return gmx;
#else
	return GeneralMult(gm1, gm2, this, mt);
#endif
    }

    GeneralMatrix* SolvedMatrix::Evaluate(MatrixType mt) {
	REPORT
	    gm1=((BaseMatrix*&)bm1)->Evaluate();
	gm2=((BaseMatrix*&)bm2)->Evaluate();
#ifdef TEMPS_DESTROYED_QUICKLY
	GeneralMatrix* gmx;
	Try { gmx = GeneralSolv(gm1,gm2,this,mt); }
	CatchAll { delete this; ReThrow; }
	delete this; return gmx;
#else
	return GeneralSolv(gm1,gm2,this,mt);
#endif
    }

    GeneralMatrix* SPMatrix::Evaluate(MatrixType mt) {
	REPORT
	    gm1=((BaseMatrix*&)bm1)->Evaluate();
	gm2=((BaseMatrix*&)bm2)->Evaluate();
#ifdef TEMPS_DESTROYED_QUICKLY
	GeneralMatrix* gmx;
	Try { gmx = GeneralSP(gm1,gm2,this,mt); }
	CatchAll { delete this; ReThrow; }
	delete this; return gmx;
#else
	return GeneralSP(gm1,gm2,this,mt);
#endif
    }

    GeneralMatrix* ElmDivideMatrix::Evaluate(MatrixType mt) {
	REPORT
	    gm1=((BaseMatrix*&)bm1)->Evaluate();
	gm2=((BaseMatrix*&)bm2)->Evaluate();
#ifdef TEMPS_DESTROYED_QUICKLY
	GeneralMatrix* gmx;
	Try { gmx = GeneralElmDivide(gm1,gm2,this,mt); }
	CatchAll { delete this; ReThrow; }
	delete this; return gmx;
#else
	return GeneralElmDivide(gm1,gm2,this,mt);
#endif
    }

    // routines for adding or subtracting matrices of identical storage structure

    static void Add(GeneralMatrix* gm, GeneralMatrix* gm1, GeneralMatrix* gm2) {
	REPORT
	    Real* s1=gm1->Store(); Real* s2=gm2->Store();
	Real* s=gm->Store(); int i=gm->Storage() >> 2;
	while (i--) {
	    *s++ = *s1++ + *s2++; *s++ = *s1++ + *s2++;
	    *s++ = *s1++ + *s2++; *s++ = *s1++ + *s2++;
	}
	i=gm->Storage() & 3; while (i--) *s++ = *s1++ + *s2++;
    }

    static void Add(GeneralMatrix* gm, GeneralMatrix* gm2) {
	REPORT
	    Real* s2=gm2->Store(); Real* s=gm->Store(); int i=gm->Storage() >> 2;
	    while (i--)
	    { *s++ += *s2++; *s++ += *s2++; *s++ += *s2++; *s++ += *s2++; }
	    i=gm->Storage() & 3; while (i--) *s++ += *s2++;
    }

    static void Subtract(GeneralMatrix* gm, GeneralMatrix* gm1, GeneralMatrix* gm2) {
	REPORT
	    Real* s1=gm1->Store(); Real* s2=gm2->Store();
	Real* s=gm->Store(); int i=gm->Storage() >> 2;
	while (i--) {
	    *s++ = *s1++ - *s2++; *s++ = *s1++ - *s2++;
	    *s++ = *s1++ - *s2++; *s++ = *s1++ - *s2++;
	}
	i=gm->Storage() & 3; while (i--) *s++ = *s1++ - *s2++;
    }

    static void Subtract(GeneralMatrix* gm, GeneralMatrix* gm2) {
	REPORT
	    Real* s2=gm2->Store(); Real* s=gm->Store(); int i=gm->Storage() >> 2;
	    while (i--)
	    { *s++ -= *s2++; *s++ -= *s2++; *s++ -= *s2++; *s++ -= *s2++; }
	    i=gm->Storage() & 3; while (i--) *s++ -= *s2++;
    }

    static void ReverseSubtract(GeneralMatrix* gm, GeneralMatrix* gm2) {
	REPORT
	    Real* s2=gm2->Store(); Real* s=gm->Store(); int i=gm->Storage() >> 2;
	    while (i--) {
		*s = *s2++ - *s; s++; *s = *s2++ - *s; s++;
		*s = *s2++ - *s; s++; *s = *s2++ - *s; s++;
	    }
	    i=gm->Storage() & 3; while (i--) { *s = *s2++ - *s; s++; }
    }

    static void SP(GeneralMatrix* gm, GeneralMatrix* gm1, GeneralMatrix* gm2) {
	REPORT
	    Real* s1=gm1->Store(); Real* s2=gm2->Store();
	Real* s=gm->Store(); int i=gm->Storage() >> 2;
	while (i--) {
	    *s++ = *s1++ * *s2++; *s++ = *s1++ * *s2++;
	    *s++ = *s1++ * *s2++; *s++ = *s1++ * *s2++;
	}
	i=gm->Storage() & 3; while (i--) *s++ = *s1++ * *s2++;
    }

    static void SP(GeneralMatrix* gm, GeneralMatrix* gm2) {
	REPORT
	    Real* s2=gm2->Store(); Real* s=gm->Store(); int i=gm->Storage() >> 2;
	    while (i--)
	    { *s++ *= *s2++; *s++ *= *s2++; *s++ *= *s2++; *s++ *= *s2++; }
	    i=gm->Storage() & 3; while (i--) *s++ *= *s2++;
    }

    static void ElmDivide(GeneralMatrix* gm, GeneralMatrix* gm1, GeneralMatrix* gm2) {
	REPORT
	    Real* s1=gm1->Store(); Real* s2=gm2->Store();
	Real* s=gm->Store(); int i=gm->Storage() >> 2;
	while (i--) {
	    *s++ = *s1++ / *s2++; *s++ = *s1++ / *s2++;
	    *s++ = *s1++ / *s2++; *s++ = *s1++ / *s2++;
	}
	i=gm->Storage() & 3; while (i--) *s++ = *s1++ / *s2++;
    }

    static void ElmDivide(GeneralMatrix* gm, GeneralMatrix* gm2) {
	REPORT
	    Real* s2=gm2->Store(); Real* s=gm->Store(); int i=gm->Storage() >> 2;
	    while (i--)
	    { *s++ /= *s2++; *s++ /= *s2++; *s++ /= *s2++; *s++ /= *s2++; }
	    i=gm->Storage() & 3; while (i--) *s++ /= *s2++;
    }

    // routines for adding or subtracting matrices of different storage structure

    static void AddDS(GeneralMatrix* gm, GeneralMatrix* gm1, GeneralMatrix* gm2) {
	int nr = gm->Nrows();
	MatrixRow mr1(gm1, LoadOnEntry); MatrixRow mr2(gm2, LoadOnEntry);
	MatrixRow mr(gm, StoreOnExit+DirectPart);
	while (nr--) { mr.Add(mr1,mr2); mr1.Next(); mr2.Next(); mr.Next(); }
    }

    static void AddDS(GeneralMatrix* gm, GeneralMatrix* gm2) {
	// Add into first argument
	int nr = gm->Nrows();
	MatrixRow mr(gm, StoreOnExit+LoadOnEntry+DirectPart);
	MatrixRow mr2(gm2, LoadOnEntry);
	while (nr--) { mr.Add(mr2); mr.Next(); mr2.Next(); }
    }

    static void SubtractDS
    (GeneralMatrix* gm, GeneralMatrix* gm1, GeneralMatrix* gm2) {
	int nr = gm->Nrows();
	MatrixRow mr1(gm1, LoadOnEntry); MatrixRow mr2(gm2, LoadOnEntry);
	MatrixRow mr(gm, StoreOnExit+DirectPart);
	while (nr--) { mr.Sub(mr1,mr2); mr1.Next(); mr2.Next(); mr.Next(); }
    }

    static void SubtractDS(GeneralMatrix* gm, GeneralMatrix* gm2) {
	int nr = gm->Nrows();
	MatrixRow mr(gm, LoadOnEntry+StoreOnExit+DirectPart);
	MatrixRow mr2(gm2, LoadOnEntry);
	while (nr--) { mr.Sub(mr2); mr.Next(); mr2.Next(); }
    }

    static void ReverseSubtractDS(GeneralMatrix* gm, GeneralMatrix* gm2) {
	int nr = gm->Nrows();
	MatrixRow mr(gm, LoadOnEntry+StoreOnExit+DirectPart);
	MatrixRow mr2(gm2, LoadOnEntry);
	while (nr--) { mr.RevSub(mr2); mr2.Next(); mr.Next(); }
    }

    static void SPDS(GeneralMatrix* gm, GeneralMatrix* gm1, GeneralMatrix* gm2) {
	int nr = gm->Nrows();
	MatrixRow mr1(gm1, LoadOnEntry); MatrixRow mr2(gm2, LoadOnEntry);
	MatrixRow mr(gm, StoreOnExit+DirectPart);
	while (nr--) { mr.Multiply(mr1,mr2); mr1.Next(); mr2.Next(); mr.Next(); }
    }

    static void SPDS(GeneralMatrix* gm, GeneralMatrix* gm2) {
	// SP into first argument
	int nr = gm->Nrows();
	MatrixRow mr(gm, StoreOnExit+LoadOnEntry+DirectPart);
	MatrixRow mr2(gm2, LoadOnEntry);
	while (nr--) { mr.Multiply(mr2); mr.Next(); mr2.Next(); }
    }

#ifdef __GNUG__
    void AddedMatrix::SelectVersion
    (MatrixType mtx, int& c1, int& c2) const
#else
	void AddedMatrix::SelectVersion
    (MatrixType mtx, bool& c1, bool& c2) const
#endif
    {
	// for determining version of add and subtract routines
	// will need to modify if further matrix structures are introduced
	MatrixBandWidth bw1 = gm1->BandWidth();
	MatrixBandWidth bw2 = gm2->BandWidth();
	MatrixBandWidth bwx(-1); if (mtx.IsBand()) bwx = bw1 + bw2;
	c1 = (mtx == gm1->Type()) && (bwx == bw1);
	c2 = (mtx == gm2->Type()) && (bwx == bw2);
    }

    static GeneralMatrix* GeneralAdd(GeneralMatrix* gm1, GeneralMatrix* gm2,
				     AddedMatrix* am, MatrixType mtx) {
	Tracer tr("GeneralAdd");
	int nr=gm1->Nrows(); int nc=gm1->Ncols();
	if (nr!=gm2->Nrows() || nc!=gm2->Ncols())
	    Throw(IncompatibleDimensionsException(*gm1, *gm2));
	Compare(gm1->Type() + gm2->Type(),mtx);
#ifdef __GNUG__
	int c1,c2; am->SelectVersion(mtx,c1,c2);
#else
	bool c1,c2; am->SelectVersion(mtx,c1,c2);     // causes problems for g++
#endif
	if (c1 && c2) {
	    if (gm1->reuse()) { REPORT Add(gm1,gm2); gm2->tDelete(); return gm1; }
	    else if (gm2->reuse()) { REPORT Add(gm2,gm1); return gm2; }
	    else {
		REPORT GeneralMatrix* gmx = gm1->Type().New(nr,nc,am);
		gmx->ReleaseAndDelete(); Add(gmx,gm1,gm2); return gmx;
	    }
	}
	else {
	    if (c1 && gm1->reuse() )                    // must have type test first
	    { REPORT AddDS(gm1,gm2); gm2->tDelete(); return gm1; }
	    else if (c2 && gm2->reuse() )
	    { REPORT AddDS(gm2,gm1); if (!c1) gm1->tDelete(); return gm2; }
	    else {
		REPORT
		    GeneralMatrix* gmx = mtx.New(nr,nc,am); AddDS(gmx,gm1,gm2);
		if (!c1) gm1->tDelete(); if (!c2) gm2->tDelete();
		gmx->ReleaseAndDelete(); return gmx;
	    }
	}
    }

    static GeneralMatrix* GeneralSub(GeneralMatrix* gm1, GeneralMatrix* gm2,
				     SubtractedMatrix* sm, MatrixType mtx) {
	Tracer tr("GeneralSub");
	Compare(gm1->Type() + gm2->Type(),mtx);
	int nr=gm1->Nrows(); int nc=gm1->Ncols();
	if (nr!=gm2->Nrows() || nc!=gm2->Ncols())
	    Throw(IncompatibleDimensionsException(*gm1, *gm2));
#ifdef __GNUG__
	int c1,c2; sm->SelectVersion(mtx,c1,c2);
#else
	bool c1,c2; sm->SelectVersion(mtx,c1,c2);     // causes problems for g++
#endif
	if (c1 && c2) {
	    if (gm1->reuse())
	    { REPORT Subtract(gm1,gm2); gm2->tDelete(); return gm1; }
	    else if (gm2->reuse()) { REPORT ReverseSubtract(gm2,gm1); return gm2; }
	    else {
		REPORT
		    GeneralMatrix* gmx = gm1->Type().New(nr,nc,sm);
		gmx->ReleaseAndDelete(); Subtract(gmx,gm1,gm2); return gmx;
	    }
	}
	else {
	    if ( c1 && gm1->reuse() )
	    { REPORT  SubtractDS(gm1,gm2); gm2->tDelete(); return gm1; }
	    else if ( c2 && gm2->reuse() ) {
		REPORT
		    ReverseSubtractDS(gm2,gm1); if (!c1) gm1->tDelete(); return gm2;
	    }
	    else {
		REPORT
		    GeneralMatrix* gmx = mtx.New(nr,nc,sm); SubtractDS(gmx,gm1,gm2);
		if (!c1) gm1->tDelete(); if (!c2) gm2->tDelete();
		gmx->ReleaseAndDelete(); return gmx;
	    }
	}
    }

    static GeneralMatrix* GeneralMult1(GeneralMatrix* gm1, GeneralMatrix* gm2,
				       MultipliedMatrix* mm, MatrixType mtx) {
	REPORT
	    Tracer tr("GeneralMult1");
	int nr=gm1->Nrows(); int nc=gm2->Ncols();
	if (gm1->Ncols() !=gm2->Nrows())
	    Throw(IncompatibleDimensionsException(*gm1, *gm2));
	GeneralMatrix* gmx = mtx.New(nr,nc,mm);

	MatrixCol mcx(gmx, StoreOnExit+DirectPart);
	MatrixCol mc2(gm2, LoadOnEntry);
	while (nc--) {
	    MatrixRow mr1(gm1, LoadOnEntry, mcx.Skip());
	    Real* el = mcx.Data();                      // pointer to an element
	    int n = mcx.Storage();
	    while (n--) { *(el++) = DotProd(mr1,mc2); mr1.Next(); }
	    mc2.Next(); mcx.Next();
	}
	gmx->ReleaseAndDelete(); gm1->tDelete(); gm2->tDelete(); return gmx;
    }

    static GeneralMatrix* GeneralMult2(GeneralMatrix* gm1, GeneralMatrix* gm2,
				       MultipliedMatrix* mm, MatrixType mtx) {
	// version that accesses by row only - not good for thin matrices
	// or column vectors in right hand term. Needs fixing
	REPORT
	    Tracer tr("GeneralMult2");
	int nr=gm1->Nrows(); int nc=gm2->Ncols();
	if (gm1->Ncols() !=gm2->Nrows())
	    Throw(IncompatibleDimensionsException(*gm1, *gm2));
	GeneralMatrix* gmx = mtx.New(nr,nc,mm);

	Real* el = gmx->Store(); int n = gmx->Storage();
	while (n--) *el++ = 0.0;
	MatrixRow mrx(gmx, LoadOnEntry+StoreOnExit+DirectPart);
	MatrixRow mr1(gm1, LoadOnEntry);
	while (nr--) {
	    MatrixRow mr2(gm2, LoadOnEntry, mr1.Skip());
	    el = mr1.Data();                            // pointer to an element
	    n = mr1.Storage();
	    while (n--) { mrx.AddScaled(mr2, *el++); mr2.Next(); }
	    mr1.Next(); mrx.Next();
	}
	gmx->ReleaseAndDelete(); gm1->tDelete(); gm2->tDelete(); return gmx;
    }

    static GeneralMatrix* mmMult(GeneralMatrix* gm1, GeneralMatrix* gm2) {
	// matrix multiplication for type Matrix only
	REPORT
	    Tracer tr("MatrixMult");

	int nr=gm1->Nrows(); int ncr=gm1->Ncols(); int nc=gm2->Ncols();
	if (ncr != gm2->Nrows()) Throw(IncompatibleDimensionsException(*gm1,*gm2));

	Matrix* gm = new Matrix(nr,nc); MatrixErrorNoSpace(gm);

	Real* s1=gm1->Store(); Real* s2=gm2->Store(); Real* s=gm->Store();

	if (ncr) {
	    while (nr--) {
		Real* s2x = s2; int j = ncr;
		Real* sx = s; Real f = *s1++; int k = nc;
		while (k--) *sx++ = f * *s2x++;
		while (--j)
		{ sx = s; f = *s1++; k = nc; while (k--) *sx++ += f * *s2x++; }
		s = sx;
	    }
	}
	else *gm = 0.0;

	gm->ReleaseAndDelete(); gm1->tDelete(); gm2->tDelete(); return gm;
    }

    static GeneralMatrix* GeneralMult(GeneralMatrix* gm1, GeneralMatrix* gm2,
				      MultipliedMatrix* mm, MatrixType mtx) {
	if ( Rectangular(gm1->Type(), gm2->Type(), mtx)) return mmMult(gm1, gm2);
	else {
	    Compare(gm1->Type() * gm2->Type(),mtx);
	    int nr = gm2->Nrows(); int nc = gm2->Ncols();
	    if (nc <= 5 && nr > nc) return GeneralMult1(gm1, gm2, mm, mtx);
	    else return GeneralMult2(gm1, gm2, mm, mtx);
	}
    }

#ifdef __GNUG__
    void SPMatrix::SelectVersion
    (MatrixType mtx, int& c1, int& c2) const
#else
	void SPMatrix::SelectVersion
    (MatrixType mtx, bool& c1, bool& c2) const
#endif
    {
	// for determining version of SP routines
	// will need to modify if further matrix structures are introduced
	MatrixBandWidth bw1 = gm1->BandWidth();
	MatrixBandWidth bw2 = gm2->BandWidth();
	MatrixBandWidth bwx(-1);  if (mtx.IsBand()) bwx = bw1.minimum(bw2);
	c1 = (mtx == gm1->Type()) && (bwx == bw1);
	c2 = (mtx == gm2->Type()) && (bwx == bw2);
    }

    static GeneralMatrix* GeneralSP(GeneralMatrix* gm1, GeneralMatrix* gm2,
				    SPMatrix* am, MatrixType mtx) {
	Tracer tr("GeneralSP");
	int nr=gm1->Nrows(); int nc=gm1->Ncols();
	if (nr!=gm2->Nrows() || nc!=gm2->Ncols())
	    Throw(IncompatibleDimensionsException(*gm1, *gm2));
	Compare(gm1->Type().SP(gm2->Type()),mtx);
#ifdef __GNUG__
	int c1,c2; am->SelectVersion(mtx,c1,c2);
#else
	bool c1,c2; am->SelectVersion(mtx,c1,c2);     // causes problems for g++
#endif
	if (c1 && c2) {
	    if (gm1->reuse()) { REPORT SP(gm1,gm2); gm2->tDelete(); return gm1; }
	    else if (gm2->reuse()) { REPORT SP(gm2,gm1); return gm2; }
	    else {
		REPORT GeneralMatrix* gmx = gm1->Type().New(nr,nc,am);
		gmx->ReleaseAndDelete(); SP(gmx,gm1,gm2); return gmx;
	    }
	}
	else {
	    if (c1 && gm1->reuse() )                    // must have type test first
	    { REPORT SPDS(gm1,gm2); gm2->tDelete(); return gm1; }
	    else if (c2 && gm2->reuse() )
	    { REPORT SPDS(gm2,gm1); if (!c1) gm1->tDelete(); return gm2; }
	    else {
		REPORT
		    GeneralMatrix* gmx = mtx.New(nr,nc,am); SPDS(gmx,gm1,gm2);
		if (!c1) gm1->tDelete(); if (!c2) gm2->tDelete();
		gmx->ReleaseAndDelete(); return gmx;
	    }
	}
    }

    // fred 0512
    static GeneralMatrix* GeneralElmDivide(GeneralMatrix* gm1, GeneralMatrix* gm2,
					   ElmDivideMatrix* am, MatrixType mtx) {
	Tracer tr("GeneralElmDivide");
	int nr=gm1->Nrows(); int nc=gm1->Ncols();
	if (nr!=gm2->Nrows() || nc!=gm2->Ncols())
	    Throw(IncompatibleDimensionsException(*gm1, *gm2));
	Compare(gm1->Type().SP(gm2->Type()),mtx);     // BUG keep using SP's?
#ifdef __GNUG__
	int c1,c2; am->SelectVersion(mtx,c1,c2);
#else
	bool c1,c2; am->SelectVersion(mtx,c1,c2);     // causes problems for g++
#endif
	if (c1 && c2) {
	    if (gm1->reuse()) { REPORT ElmDivide(gm1,gm2); gm2->tDelete(); return gm1; }
	    else if (gm2->reuse()) { REPORT ElmDivide(gm2,gm1); return gm2; }
	    else {
		REPORT GeneralMatrix* gmx = gm1->Type().New(nr,nc,am);
		gmx->ReleaseAndDelete(); ElmDivide(gmx,gm1,gm2); return gmx;
	    }
	}
	else {
	    return gm1;                                 // BUG here
	    /*
	      if (c1 && gm1->reuse() )               // must have type test first
	      {
	      //REPORT SPDS(gm1,gm2); gm2->tDelete(); return gm1;
	      }
	      else if (c2 && gm2->reuse() )
	      {
	      //REPORT SPDS(gm2,gm1); if (!c1) gm1->tDelete(); return gm2;
	      }
	      else
	      {
	      REPORT
	      GeneralMatrix* gmx = mtx.New(nr,nc,am); SPDS(gmx,gm1,gm2);
	      if (!c1) gm1->tDelete(); if (!c2) gm2->tDelete();
	      gmx->ReleaseAndDelete(); return gmx;
	      }*/
	}
    }

    static GeneralMatrix* GeneralSolv(GeneralMatrix* gm1, GeneralMatrix* gm2,
				      BaseMatrix* sm, MatrixType mtx) {
	REPORT
	    Tracer tr("GeneralSolv");
	Compare(gm1->Type().i() * gm2->Type(),mtx);
	int nr=gm1->Nrows(); int nc=gm2->Ncols();
	if (gm1->Ncols() != gm2->Nrows())
	    Throw(IncompatibleDimensionsException(*gm1, *gm2));
	GeneralMatrix* gmx = mtx.New(nr,nc,sm); MatrixErrorNoSpace(gmx);
	Real* r = new Real [nr]; MatrixErrorNoSpace(r);
#ifndef ATandT
	MONITOR_REAL_NEW("Make   (GenSolv)",nr,r)
	    // deleted for ATandT, to balance deletion below
#endif
	    GeneralMatrix* gms = gm1->MakeSolver();
	Try {
	    // copy to and from r
	    MatrixColX mcx(gmx, r, StoreOnExit+DirectPart);
	    // this must be inside Try so mcx is destroyed before gmx
	    MatrixColX mc2(gm2, r, LoadOnEntry);
	    while (nc--) { gms->Solver(mcx, mc2); mcx.Next(); mc2.Next(); }
	}
	CatchAll {
	    gms->tDelete();
	    delete gmx;                                 // <--------------------
	    gm2->tDelete();
#ifndef ATandT
	    MONITOR_REAL_DELETE("Delete (GenSolv)",nr,r)
		// ATandT version 2.1 gives an internal error
#endif
		delete [] r;
	    ReThrow;
	}
	gms->tDelete(); gmx->ReleaseAndDelete(); gm2->tDelete();
#ifndef ATandT
	MONITOR_REAL_DELETE("Delete (GenSolv)",nr,r)
	    // ATandT version 2.1 gives an internal error
#endif
	    delete [] r;
	return gmx;
    }

    GeneralMatrix* InvertedMatrix::Evaluate(MatrixType mtx) {
	// Matrix Inversion - use solve routines
	Tracer tr("InvertedMatrix::Evaluate");
	REPORT
	    gm=((BaseMatrix*&)bm)->Evaluate();
	int n = gm->Nrows(); DiagonalMatrix I(n); I=1.0;
#ifdef TEMPS_DESTROYED_QUICKLY
	GeneralMatrix* gmx;
	Try {
	    Compare(gm->Type().i(),mtx);
	    SkipConversionCheck SCC;                    // otherwise inverting
	    // symmetric causes a problem
	    gmx = GeneralSolv(gm,&I,this,mtx);
	}
	CatchAll { delete this; ReThrow; }
	delete this; return gmx;
#else
	Compare(gm->Type().i(),mtx);
	SkipConversionCheck SCC;                      // otherwise inverting
	// symmetric causes a problem
	return GeneralSolv(gm,&I,this,mtx);
#endif
    }

    /*************************** norm functions ****************************/

    Real BaseMatrix::Norm1() const {
	// maximum of sum of absolute values of a column
	REPORT
	    GeneralMatrix* gm = ((BaseMatrix&)*this).Evaluate();
	int nc = gm->Ncols(); Real value = 0.0;
	MatrixCol mc(gm, LoadOnEntry);
	while (nc--)
	{ Real v = mc.SumAbsoluteValue(); if (value < v) value = v; mc.Next(); }
	gm->tDelete(); return value;
    }

    Real BaseMatrix::NormInfinity() const {
	// maximum of sum of absolute values of a row
	REPORT
	    GeneralMatrix* gm = ((BaseMatrix&)*this).Evaluate();
	int nr = gm->Nrows(); Real value = 0.0;
	MatrixRow mr(gm, LoadOnEntry);
	while (nr--)
	{ Real v = mr.SumAbsoluteValue(); if (value < v) value = v; mr.Next(); }
	gm->tDelete(); return value;
    }

    /********************** Concatenation and stacking *************************/

    GeneralMatrix* ConcatenatedMatrix::Evaluate(MatrixType mtx) {
	REPORT
	    Tracer tr("Concatenate");
#ifdef TEMPS_DESTROYED_QUICKLY
	Try {
	    gm2 = ((BaseMatrix*&)bm2)->Evaluate();
	    gm1 = ((BaseMatrix*&)bm1)->Evaluate();
	    Compare(gm1->Type() | gm2->Type(),mtx);
	    int nr=gm1->Nrows(); int nc = gm1->Ncols() + gm2->Ncols();
	    if (nr != gm2->Nrows())
		Throw(IncompatibleDimensionsException(*gm1, *gm2));
	    GeneralMatrix* gmx = mtx.New(nr,nc,this);
	    MatrixRow mr1(gm1, LoadOnEntry); MatrixRow mr2(gm2, LoadOnEntry);
	    MatrixRow mr(gmx, StoreOnExit+DirectPart);
	    while (nr--) { mr.ConCat(mr1,mr2); mr1.Next(); mr2.Next(); mr.Next(); }
	    gmx->ReleaseAndDelete(); gm1->tDelete(); gm2->tDelete(); delete this;
	    return gmx;
	}
	CatchAll  { delete this; ReThrow; }
#ifndef UseExceptions
	return 0;
#endif
#else
	gm2 = ((BaseMatrix*&)bm2)->Evaluate();
	gm1 = ((BaseMatrix*&)bm1)->Evaluate();
	Compare(gm1->Type() | gm2->Type(),mtx);
	int nr=gm1->Nrows(); int nc = gm1->Ncols() + gm2->Ncols();
	if (nr != gm2->Nrows())
	    Throw(IncompatibleDimensionsException(*gm1, *gm2));
	GeneralMatrix* gmx = mtx.New(nr,nc,this);
	MatrixRow mr1(gm1, LoadOnEntry); MatrixRow mr2(gm2, LoadOnEntry);
	MatrixRow mr(gmx, StoreOnExit+DirectPart);
	while (nr--) { mr.ConCat(mr1,mr2); mr1.Next(); mr2.Next(); mr.Next(); }
	gmx->ReleaseAndDelete(); gm1->tDelete(); gm2->tDelete(); return gmx;
#endif
    }

    GeneralMatrix* StackedMatrix::Evaluate(MatrixType mtx) {
	REPORT
	    Tracer tr("Stack");
#ifdef TEMPS_DESTROYED_QUICKLY
	Try {
	    gm2 = ((BaseMatrix*&)bm2)->Evaluate();
	    gm1 = ((BaseMatrix*&)bm1)->Evaluate();
	    Compare(gm1->Type() & gm2->Type(),mtx);
	    int nc=gm1->Ncols();
	    int nr1 = gm1->Nrows(); int nr2 = gm2->Nrows();
	    if (nc != gm2->Ncols())
		Throw(IncompatibleDimensionsException(*gm1, *gm2));
	    GeneralMatrix* gmx = mtx.New(nr1+nr2,nc,this);
	    MatrixRow mr1(gm1, LoadOnEntry); MatrixRow mr2(gm2, LoadOnEntry);
	    MatrixRow mr(gmx, StoreOnExit+DirectPart);
	    while (nr1--) { mr.Copy(mr1); mr1.Next(); mr.Next(); }
	    while (nr2--) { mr.Copy(mr2); mr2.Next(); mr.Next(); }
	    gmx->ReleaseAndDelete(); gm1->tDelete(); gm2->tDelete(); delete this;
	    return gmx;
	}
	CatchAll  { delete this; ReThrow; }
#ifndef UseExceptions
	return 0;
#endif
#else
	gm2 = ((BaseMatrix*&)bm2)->Evaluate();
	gm1 = ((BaseMatrix*&)bm1)->Evaluate();
	Compare(gm1->Type() & gm2->Type(),mtx);
	int nc=gm1->Ncols();
	int nr1 = gm1->Nrows(); int nr2 = gm2->Nrows();
	if (nc != gm2->Ncols())
	    Throw(IncompatibleDimensionsException(*gm1, *gm2));
	GeneralMatrix* gmx = mtx.New(nr1+nr2,nc,this);
	MatrixRow mr1(gm1, LoadOnEntry); MatrixRow mr2(gm2, LoadOnEntry);
	MatrixRow mr(gmx, StoreOnExit+DirectPart);
	while (nr1--) { mr.Copy(mr1); mr1.Next(); mr.Next(); }
	while (nr2--) { mr.Copy(mr2); mr2.Next(); mr.Next(); }
	gmx->ReleaseAndDelete(); gm1->tDelete(); gm2->tDelete(); return gmx;
#endif
    }

    // ************************* equality of matrices ******************** //

    static bool RealEqual(Real* s1, Real* s2, int n) {
	int i = n >> 2;
	while (i--) {
	    if (*s1++ != *s2++) return false; if (*s1++ != *s2++) return false;
	    if (*s1++ != *s2++) return false; if (*s1++ != *s2++) return false;
	}
	i = n & 3; while (i--) if (*s1++ != *s2++) return false;
	return true;
    }

    static bool intEqual(int* s1, int* s2, int n) {
	int i = n >> 2;
	while (i--) {
	    if (*s1++ != *s2++) return false; if (*s1++ != *s2++) return false;
	    if (*s1++ != *s2++) return false; if (*s1++ != *s2++) return false;
	}
	i = n & 3; while (i--) if (*s1++ != *s2++) return false;
	return true;
    }

    bool operator==(const BaseMatrix& A, const BaseMatrix& B) {
	Tracer tr("BaseMatrix ==");
	REPORT
	    GeneralMatrix* gmA = ((BaseMatrix&)A).Evaluate();
	GeneralMatrix* gmB = ((BaseMatrix&)B).Evaluate();

	if (gmA == gmB)                               // same matrix
	{ REPORT gmA->tDelete(); return true; }

	if ( gmA->Nrows() != gmB->Nrows() || gmA->Ncols() != gmB->Ncols() )
	    // different dimensions
        { REPORT gmA->tDelete(); gmB->tDelete(); return false; }

        // check for CroutMatrix or BandLUMatrix
        MatrixType AType = gmA->Type(); MatrixType BType = gmB->Type();
	if (AType.CannotConvert() || BType.CannotConvert() ) {
	    REPORT
		bool bx = gmA->IsEqual(*gmB);
	    gmA->tDelete(); gmB->tDelete();
	    return bx;
	}

	// is matrix storage the same
	// will need to modify if further matrix structures are introduced
	if (AType == BType && gmA->BandWidth() == gmB->BandWidth()) {
	    // compare store
	    REPORT
		bool bx = RealEqual(gmA->Store(),gmB->Store(),gmA->Storage());
	    gmA->tDelete(); gmB->tDelete();
	    return bx;
	}

	// matrix storage different - just subtract
	REPORT  return IsZero(*gmA-*gmB);
    }

    bool operator==(const GeneralMatrix& A, const GeneralMatrix& B) {
	Tracer tr("GeneralMatrix ==");
	// May or may not call tDeletes
	REPORT

	    if (&A == &B)                               // same matrix
	    { REPORT return true; }

	if ( A.Nrows() != B.Nrows() || A.Ncols() != B.Ncols() ) {
	    // different dimensions
	    REPORT return false;
	}

	// check for CroutMatrix or BandLUMatrix
	MatrixType AType = A.Type(); MatrixType BType = B.Type();
	if (AType.CannotConvert() || BType.CannotConvert() )
	{ REPORT  return A.IsEqual(B); }

	// is matrix storage the same
	// will need to modify if further matrix structures are introduced
	if (AType == BType && A.BandWidth() == B.BandWidth())
        { REPORT return RealEqual(A.Store(),B.Store(),A.Storage()); }

        // matrix storage different - just subtract
        REPORT  return IsZero(A-B);
    }

    bool GeneralMatrix::IsZero() const {
	REPORT
	    Real* s=store; int i = storage >> 2;
	while (i--) {
	    if (*s++) return false; if (*s++) return false;
	    if (*s++) return false; if (*s++) return false;
	}
	i = storage & 3; while (i--) if (*s++) return false;
	return true;
    }

    bool IsZero(const BaseMatrix& A) {
	Tracer tr("BaseMatrix::IsZero");
	REPORT
	    GeneralMatrix* gm1 = 0; bool bx;
	Try { gm1=((BaseMatrix&)A).Evaluate(); bx = gm1->IsZero(); }
	CatchAll { if (gm1) gm1->tDelete(); ReThrow; }
	gm1->tDelete();
	return bx;
    }

    // IsEqual functions - insist matrices are of same type
    // as well as equal values to be equal

    bool GeneralMatrix::IsEqual(const GeneralMatrix& A) const {
	Tracer tr("GeneralMatrix IsEqual");
	if (A.Type() != Type())                       // not same types
	{ REPORT return false; }
	if (&A == this)                             // same matrix
        { REPORT  return true; }
        if (A.nrows != nrows || A.ncols != ncols)
	    // different dimensions
	{ REPORT return false; }
	// is matrix storage the same - compare store
	REPORT
	    return RealEqual(A.store,store,storage);
    }

    bool CroutMatrix::IsEqual(const GeneralMatrix& A) const {
	Tracer tr("CroutMatrix IsEqual");
	if (A.Type() != Type())                       // not same types
	{ REPORT return false; }
	if (&A == this)                             // same matrix
        { REPORT  return true; }
        if (A.nrows != nrows || A.ncols != ncols)
	    // different dimensions
	{ REPORT return false; }
	// is matrix storage the same - compare store
	REPORT
	    return RealEqual(A.store,store,storage)
	    && intEqual(((CroutMatrix&)A).indx, indx, nrows);
    }

    bool BandLUMatrix::IsEqual(const GeneralMatrix& A) const {
	Tracer tr("BandLUMatrix IsEqual");
	if (A.Type() != Type())                       // not same types
	{ REPORT  return false; }
	if (&A == this)                             // same matrix
        { REPORT  return true; }
        if ( A.Nrows() != nrows || A.Ncols() != ncols
	     || ((BandLUMatrix&)A).m1 != m1 || ((BandLUMatrix&)A).m2 != m2 )
	    // different dimensions
	{ REPORT  return false; }

	// matrix storage the same - compare store
	REPORT
	    return RealEqual(A.Store(),store,storage)
	    && RealEqual(((BandLUMatrix&)A).store2,store2,storage2)
	    && intEqual(((BandLUMatrix&)A).indx, indx, nrows);
    }

#ifdef use_namespace
}
#endif
