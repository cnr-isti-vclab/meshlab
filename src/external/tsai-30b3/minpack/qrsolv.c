/* qrsolv.f -- translated by f2c (version of 17 January 1992  0:17:58).
   You must link the resulting object file with the libraries:
	-lf77 -li77 -lm -lc   (in that order)
*/

#include "f2c.h"

/* Subroutine */ int qrsolv_(n, r, ldr, ipvt, diag, qtb, x, sdiag, wa)
integer *n;
doublereal *r;
integer *ldr, *ipvt;
doublereal *diag, *qtb, *x, *sdiag, *wa;
{
    /* Initialized data */

    static doublereal p5 = .5;
    static doublereal p25 = .25;
    static doublereal zero = 0.;

    /* System generated locals */
    integer r_dim1, r_offset, i__1, i__2, i__3;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt();

    /* Local variables */
    static doublereal temp;
    static integer i, j, k, l;
    static doublereal cotan;
    static integer nsing;
    static doublereal qtbpj;
    static integer jp1, kp1;
    static doublereal tan_, cos_, sin_, sum;

/*     ********** */

/*     subroutine qrsolv */

/*     given an m by n matrix a, an n by n diagonal matrix d, */
/*     and an m-vector b, the problem is to determine an x which */
/*     solves the system */

/*           a*x = b ,     d*x = 0 , */

/*     in the least squares sense. */

/*     this subroutine completes the solution of the problem */
/*     if it is provided with the necessary information from the */
/*     qr factorization, with column pivoting, of a. that is, if */
/*     a*p = q*r, where p is a permutation matrix, q has orthogonal */
/*     columns, and r is an upper triangular matrix with diagonal */
/*     elements of nonincreasing magnitude, then qrsolv expects */
/*     the full upper triangle of r, the permutation matrix p, */
/*     and the first n components of (q transpose)*b. the system */
/*     a*x = b, d*x = 0, is then equivalent to */

/*                  t       t */
/*           r*z = q *b ,  p *d*p*z = 0 , */

/*     where x = p*z. if this system does not have full rank, */
/*     then a least squares solution is obtained. on output qrsolv */
/*     also provides an upper triangular matrix s such that */

/*            t   t               t */
/*           p *(a *a + d*d)*p = s *s . */

/*     s is computed within qrsolv and may be of separate interest. */

/*     the subroutine statement is */

/*       subroutine qrsolv(n,r,ldr,ipvt,diag,qtb,x,sdiag,wa) */

/*     where */

/*       n is a positive integer input variable set to the order of r. */

/*       r is an n by n array. on input the full upper triangle */
/*         must contain the full upper triangle of the matrix r. */
/*         on output the full upper triangle is unaltered, and the */
/*         strict lower triangle contains the strict upper triangle */
/*         (transposed) of the upper triangular matrix s. */

/*       ldr is a positive integer input variable not less than n */
/*         which specifies the leading dimension of the array r. */

/*       ipvt is an integer input array of length n which defines the */
/*         permutation matrix p such that a*p = q*r. column j of p */
/*         is column ipvt(j) of the identity matrix. */

/*       diag is an input array of length n which must contain the */
/*         diagonal elements of the matrix d. */

/*       qtb is an input array of length n which must contain the first */

/*         n elements of the vector (q transpose)*b. */

/*       x is an output array of length n which contains the least */
/*         squares solution of the system a*x = b, d*x = 0. */

/*       sdiag is an output array of length n which contains the */
/*         diagonal elements of the upper triangular matrix s. */

/*       wa is a work array of length n. */

/*     subprograms called */

/*       fortran-supplied ... dabs,dsqrt */

/*     argonne national laboratory. minpack project. march 1980. */
/*     burton s. garbow, kenneth e. hillstrom, jorge j. more */

/*     ********** */
    /* Parameter adjustments */
    --wa;
    --sdiag;
    --x;
    --qtb;
    --diag;
    --ipvt;
    r_dim1 = *ldr;
    r_offset = r_dim1 + 1;
    r -= r_offset;

    /* Function Body */

/*     copy r and (q transpose)*b to preserve input and initialize s. */
/*     in particular, save the diagonal elements of r in x. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *n;
	for (i = j; i <= i__2; ++i) {
	    r[i + j * r_dim1] = r[j + i * r_dim1];
/* L10: */
	}
	x[j] = r[j + j * r_dim1];
	wa[j] = qtb[j];
/* L20: */
    }

/*     eliminate the diagonal matrix d using a givens rotation. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {

/*        prepare the row of d to be eliminated, locating the */
/*        diagonal element using p from the qr factorization. */

	l = ipvt[j];
	if (diag[l] == zero) {
	    goto L90;
	}
	i__2 = *n;
	for (k = j; k <= i__2; ++k) {
	    sdiag[k] = zero;
/* L30: */
	}
	sdiag[j] = diag[l];

/*        the transformations to eliminate the row of d */
/*        modify only a single element of (q transpose)*b */
/*        beyond the first n, which is initially zero. */

	qtbpj = zero;
	i__2 = *n;
	for (k = j; k <= i__2; ++k) {

/*           determine a givens rotation which eliminates the */
/*           appropriate element in the current row of d. */

	    if (sdiag[k] == zero) {
		goto L70;
	    }
	    if ((d__1 = r[k + k * r_dim1], abs(d__1)) >= (d__2 = sdiag[k], 
		    abs(d__2))) {
		goto L40;
	    }
	    cotan = r[k + k * r_dim1] / sdiag[k];
/* Computing 2nd power */
	    d__1 = cotan;
	    sin_ = p5 / sqrt(p25 + p25 * (d__1 * d__1));
	    cos_ = sin_ * cotan;
	    goto L50;
L40:
	    tan_ = sdiag[k] / r[k + k * r_dim1];
/* Computing 2nd power */
	    d__1 = tan_;
	    cos_ = p5 / sqrt(p25 + p25 * (d__1 * d__1));
	    sin_ = cos_ * tan_;
L50:

/*           compute the modified diagonal element of r and */
/*           the modified element of ((q transpose)*b,0). */

	    r[k + k * r_dim1] = cos_ * r[k + k * r_dim1] + sin_ * sdiag[k];
	    temp = cos_ * wa[k] + sin_ * qtbpj;
	    qtbpj = -sin_ * wa[k] + cos_ * qtbpj;
	    wa[k] = temp;

/*           accumulate the tranformation in the row of s. */

	    kp1 = k + 1;
	    if (*n < kp1) {
		goto L70;
	    }
	    i__3 = *n;
	    for (i = kp1; i <= i__3; ++i) {
		temp = cos_ * r[i + k * r_dim1] + sin_ * sdiag[i];
		sdiag[i] = -sin_ * r[i + k * r_dim1] + cos_ * sdiag[i];
		r[i + k * r_dim1] = temp;
/* L60: */
	    }
L70:
/* L80: */
	    ;
	}
L90:

/*        store the diagonal element of s and restore */
/*        the corresponding diagonal element of r. */

	sdiag[j] = r[j + j * r_dim1];
	r[j + j * r_dim1] = x[j];
/* L100: */
    }

/*     solve the triangular system for z. if the system is */
/*     singular, then obtain a least squares solution. */

    nsing = *n;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (sdiag[j] == zero && nsing == *n) {
	    nsing = j - 1;
	}
	if (nsing < *n) {
	    wa[j] = zero;
	}
/* L110: */
    }
    if (nsing < 1) {
	goto L150;
    }
    i__1 = nsing;
    for (k = 1; k <= i__1; ++k) {
	j = nsing - k + 1;
	sum = zero;
	jp1 = j + 1;
	if (nsing < jp1) {
	    goto L130;
	}
	i__2 = nsing;
	for (i = jp1; i <= i__2; ++i) {
	    sum += r[i + j * r_dim1] * wa[i];
/* L120: */
	}
L130:
	wa[j] = (wa[j] - sum) / sdiag[j];
/* L140: */
    }
L150:

/*     permute the components of z back to components of x. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	l = ipvt[j];
	x[l] = wa[j];
/* L160: */
    }
    return 0;

/*     last card of subroutine qrsolv. */

} /* qrsolv_ */

