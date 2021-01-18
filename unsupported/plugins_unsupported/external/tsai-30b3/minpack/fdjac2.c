/* fdjac2.f -- translated by f2c (version of 17 January 1992  0:17:58).
   You must link the resulting object file with the libraries:
	-lf77 -li77 -lm -lc   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int fdjac2_(fcn, m, n, x, fvec, fjac, ldfjac, iflag, epsfcn, 
	wa)
/* Subroutine */ int (*fcn) ();
integer *m, *n;
doublereal *x, *fvec, *fjac;
integer *ldfjac, *iflag;
doublereal *epsfcn, *wa;
{
    /* Initialized data */

    static doublereal zero = 0.;

    /* System generated locals */
    integer fjac_dim1, fjac_offset, i__1, i__2;

    /* Builtin functions */
    double sqrt();

    /* Local variables */
    static doublereal temp, h;
    static integer i, j;
    static doublereal epsmch;
    extern doublereal dpmpar_();
    static doublereal eps;

/*     ********** */

/*     subroutine fdjac2 */

/*     this subroutine computes a forward-difference approximation */
/*     to the m by n jacobian matrix associated with a specified */
/*     problem of m functions in n variables. */

/*     the subroutine statement is */

/*       subroutine fdjac2(fcn,m,n,x,fvec,fjac,ldfjac,iflag,epsfcn,wa) */

/*     where */

/*       fcn is the name of the user-supplied subroutine which */
/*         calculates the functions. fcn must be declared */
/*         in an external statement in the user calling */
/*         program, and should be written as follows. */

/*         subroutine fcn(m,n,x,fvec,iflag) */
/*         integer m,n,iflag */
/*         double precision x(n),fvec(m) */
/*         ---------- */
/*         calculate the functions at x and */
/*         return this vector in fvec. */
/*         ---------- */
/*         return */
/*         end */

/*         the value of iflag should not be changed by fcn unless */
/*         the user wants to terminate execution of fdjac2. */
/*         in this case set iflag to a negative integer. */

/*       m is a positive integer input variable set to the number */
/*         of functions. */

/*       n is a positive integer input variable set to the number */
/*         of variables. n must not exceed m. */

/*       x is an input array of length n. */

/*       fvec is an input array of length m which must contain the */
/*         functions evaluated at x. */

/*       fjac is an output m by n array which contains the */
/*         approximation to the jacobian matrix evaluated at x. */

/*       ldfjac is a positive integer input variable not less than m */
/*         which specifies the leading dimension of the array fjac. */

/*       iflag is an integer variable which can be used to terminate */
/*         the execution of fdjac2. see description of fcn. */

/*       epsfcn is an input variable used in determining a suitable */
/*         step length for the forward-difference approximation. this */
/*         approximation assumes that the relative errors in the */
/*         functions are of the order of epsfcn. if epsfcn is less */
/*         than the machine precision, it is assumed that the relative */
/*         errors in the functions are of the order of the machine */
/*         precision. */

/*       wa is a work array of length m. */

/*     subprograms called */

/*       user-supplied ...... fcn */

/*       minpack-supplied ... dpmpar */

/*       fortran-supplied ... dabs,dmax1,dsqrt */

/*     argonne national laboratory. minpack project. march 1980. */
/*     burton s. garbow, kenneth e. hillstrom, jorge j. more */

/*     ********** */
    /* Parameter adjustments */
    --wa;
    fjac_dim1 = *ldfjac;
    fjac_offset = fjac_dim1 + 1;
    fjac -= fjac_offset;
    --fvec;
    --x;

    /* Function Body */

/*     epsmch is the machine precision. */

    epsmch = dpmpar_(&c__1);

    eps = sqrt((max(*epsfcn,epsmch)));
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	temp = x[j];
	h = eps * abs(temp);
	if (h == zero) {
	    h = eps;
	}
	x[j] = temp + h;
	(*fcn)(m, n, &x[1], &wa[1], iflag);
	if (*iflag < 0) {
	    goto L30;
	}
	x[j] = temp;
	i__2 = *m;
	for (i = 1; i <= i__2; ++i) {
	    fjac[i + j * fjac_dim1] = (wa[i] - fvec[i]) / h;
/* L10: */
	}
/* L20: */
    }
L30:
    return 0;

/*     last card of subroutine fdjac2. */

} /* fdjac2_ */

