#ifndef _LALSIMIMREOBSPINPRECAUXFUNCS_C
#define _LALSIMIMREOBSPINPRECAUXFUNCS_C

/* Calculate the kronecker delta */
static REAL8 
XLALKronecker(const INT4 i, const INT4 j)
{
	REAL8		d = ((i == j) ? 1. : 0.);
	return d;
}

#endif				/* _LALSIMIMREOBSPINPRECAUXFUNCS_C */
