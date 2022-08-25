/**
	@file	predicates.h

			This module provides routines for Arbitrary Precision 
			Floating-point Arithmetic and Fast Robust Geometric Predicates.

	@note	This file and predicates.cpp were derived from the file called 
			predicates.c that was "placed in the public domain" by Jonathan 
			Richard Shewchuk.  The file predicates.c was downloaded from 
			http://www.cs.cmu.edu/~quake/robust.html and is dated May 18, 
			1996.  

	@note	The changes made included:  establishing a header file, enabling 
			the routines to be compiled from C++ and used with the Universal 
			3D code base; adding optimizations; and controlling which 
			functionality is compiled via preprocessor defines, as well as 
			commenting out totally unneeded functions.

	@note	For additional details and usage documentation please refer to 
			the comments in predicates.cpp, and the information provided at 
			http://www.cs.cmu.edu/~quake/robust.html.
*/


#ifndef _PREDICATES_H_
#define _PREDICATES_H_

#include "IFXResult.h"
#include "IFXDataTypes.h"

#define REAL F64                      /* float or double */


#ifdef __cplusplus
extern "C" {
#endif

REAL		orient2d(REAL *, REAL *, REAL *);
REAL		incircle(REAL *, REAL *, REAL *, REAL *); 
IFXRESULT	exactinit();
void		exactcleanup();

#ifdef __cplusplus
}
#endif


#endif
