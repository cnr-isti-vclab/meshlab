/////////////////////////////////////////////////////////////////////////////////
// 
//  Levenberg - Marquardt non-linear minimization algorithm
//  Copyright (C) 2004-05  Manolis Lourakis (lourakis at ics forth gr)
//  Institute of Computer Science, Foundation for Research & Technology - Hellas
//  Heraklion, Crete, Greece.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
/////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * Wrappers for linearly constrained Levenberg-Marquardt minimization. The same
 * core code is used with appropriate #defines to derive single and double
 * precision versions, see also lmlec_core.c
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lm.h"
#include "misc.h"


#ifndef HAVE_LAPACK

#ifdef _MSC_VER
#pragma message("Linearly constrained optimization requires LAPACK and was not compiled!")
#else
#warning Linearly constrained optimization requires LAPACK and was not compiled!
#endif // _MSC_VER

#else // LAPACK present

#if !defined(LM_DBL_PREC) && !defined(LM_SNGL_PREC)
#error At least one of LM_DBL_PREC, LM_SNGL_PREC should be defined!
#endif


#ifdef LM_SNGL_PREC
/* single precision (float) definitions */
#define LM_REAL float
#define LM_PREFIX s

#define __SUBCNST(x) x##F
#define LM_CNST(x) __SUBCNST(x) // force substitution

#include "lmlec_core.c" // read in core code

#undef LM_REAL
#undef LM_PREFIX
#undef __SUBCNST
#undef LM_CNST
#endif /* LM_SNGL_PREC */

#ifdef LM_DBL_PREC
/* double precision definitions */
#define LM_REAL double
#define LM_PREFIX d

#define LM_CNST(x) (x)

#include "lmlec_core.c" // read in core code

#undef LM_REAL
#undef LM_PREFIX
#undef LM_CNST
#endif /* LM_DBL_PREC */

#endif /* HAVE_LAPACK */

