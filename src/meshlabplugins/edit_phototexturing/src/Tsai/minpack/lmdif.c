/* lmdif.f -- translated by f2c (version of 17 January 1992  0:17:58).
   You must link the resulting object file with the libraries:
	-lf77 -li77 -lm -lc   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int lmdif_(fcn, m, n, x, fvec, ftol, xtol, gtol, maxfev, 
	epsfcn, diag, mode, factor, nprint, info, nfev, fjac, ldfjac, ipvt, 
	qtf, wa1, wa2, wa3, wa4)
/* Subroutine */ int (*fcn) ();
integer *m, *n;
doublereal *x, *fvec, *ftol, *xtol, *gtol;
integer *maxfev;
doublereal *epsfcn, *diag;
integer *mode;
doublereal *factor;
integer *nprint, *info, *nfev;
doublereal *fjac;
integer *ldfjac, *ipvt;
doublereal *qtf, *wa1, *wa2, *wa3, *wa4;
{
    /* Initialized data */

    static doublereal one = 1.;
    static doublereal p1 = .1;
    static doublereal p5 = .5;
    static doublereal p25 = .25;
    static doublereal p75 = .75;
    static doublereal p0001 = 1e-4;
    static doublereal zero = 0.;

    /* System generated locals */
    integer fjac_dim1, fjac_offset, i__1, i__2;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    double sqrt();

    /* Local variables */
    static integer iter;
    static doublereal temp, temp1, temp2;
    static integer i, j, l, iflag;
    static doublereal delta;
    extern /* Subroutine */ int qrfac_(), lmpar_();
    static doublereal ratio;
    extern doublereal enorm_();
    static doublereal fnorm, gnorm;
    extern /* Subroutine */ int fdjac2_();
    static doublereal pnorm, xnorm, fnorm1, actred, dirder, epsmch, prered;
    extern doublereal dpmpar_();
    static doublereal par, sum;

/*     ********** */

/*     subroutine lmdif */

/*     the purpose of lmdif is to minimize the sum of the squares of */
/*     m nonlinear functions in n variables by a modification of */
/*     the levenberg-marquardt algorithm. the user must provide a */
/*     subroutine which calculates the functions. the jacobian is */
/*     then calculated by a forward-difference approximation. */

/*     the subroutine statement is */

/*       subroutine lmdif(fcn,m,n,x,fvec,ftol,xtol,gtol,maxfev,epsfcn, */
/*                        diag,mode,factor,nprint,info,nfev,fjac, */
/*                        ldfjac,ipvt,qtf,wa1,wa2,wa3,wa4) */

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
/*         the user wants to terminate execution of lmdif. */
/*         in this case set iflag to a negative integer. */

/*       m is a positive integer input variable set to the number */
/*         of functions. */

/*       n is a positive integer input variable set to the number */
/*         of variables. n must not exceed m. */

/*       x is an array of length n. on input x must contain */
/*         an initial estimate of the solution vector. on output x */
/*         contains the final estimate of the solution vector. */

/*       fvec is an output array of length m which contains */
/*         the functions evaluated at the output x. */

/*       ftol is a nonnegative input variable. termination */
/*         occurs when both the actual and predicted relative */
/*         reductions in the sum of squares are at most ftol. */
/*         therefore, ftol measures the relative error desired */
/*         in the sum of squares. */

/*       xtol is a nonnegative input variable. termination */
/*         occurs when the relative error between two consecutive */
/*         iterates is at most xtol. therefore, xtol measures the */
/*         relative error desired in the approximate solution. */

/*       gtol is a nonnegative input variable. termination */
/*         occurs when the cosine of the angle between fvec and */
/*         any column of the jacobian is at most gtol in absolute */
/*         value. therefore, gtol measures the orthogonality */
/*         desired between the function vector and the columns */
/*         of the jacobian. */

/*       maxfev is a positive integer input variable. termination */
/*         occurs when the number of calls to fcn is at least */
/*         maxfev by the end of an iteration. */

/*       epsfcn is an input variable used in determining a suitable */
/*         step length for the forward-difference approximation. this */
/*         approximation assumes that the relative errors in the */
/*         functions are of the order of epsfcn. if epsfcn is less */
/*         than the machine precision, it is assumed that the relative */
/*         errors in the functions are of the order of the machine */
/*         precision. */

/*       diag is an array of length n. if mode = 1 (see */
/*         below), diag is internally set. if mode = 2, diag */
/*         must contain positive entries that serve as */
/*         multiplicative scale factors for the variables. */

/*       mode is an integer input variable. if mode = 1, the */
/*         variables will be scaled internally. if mode = 2, */
/*         the scaling is specified by the input diag. other */
/*         values of mode are equivalent to mode = 1. */

/*       factor is a positive input variable used in determining the */
/*         initial step bound. this bound is set to the product of */
/*         factor and the euclidean norm of diag*x if nonzero, or else */
/*         to factor itself. in most cases factor should lie in the */
/*         interval (.1,100.). 100. is a generally recommended value. */

/*       nprint is an integer input variable that enables controlled */
/*         printing of iterates if it is positive. in this case, */
/*         fcn is called with iflag = 0 at the beginning of the first */
/*         iteration and every nprint iterations thereafter and */
/*         immediately prior to return, with x and fvec available */
/*         for printing. if nprint is not positive, no special calls */
/*         of fcn with iflag = 0 are made. */

/*       info is an integer output variable. if the user has */
/*         terminated execution, info is set to the (negative) */
/*         value of iflag. see description of fcn. otherwise, */
/*         info is set as follows. */

/*         info = 0  improper input parameters. */

/*         info = 1  both actual and predicted relative reductions */
/*                   in the sum of squares are at most ftol. */

/*         info = 2  relative error between two consecutive iterates */
/*                   is at most xtol. */

/*         info = 3  conditions for info = 1 and info = 2 both hold. */

/*         info = 4  the cosine of the angle between fvec and any */
/*                   column of the jacobian is at most gtol in */
/*                   absolute value. */

/*         info = 5  number of calls to fcn has reached or */
/*                   exceeded maxfev. */

/*         info = 6  ftol is too small. no further reduction in */
/*                   the sum of squares is possible. */

/*         info = 7  xtol is too small. no further improvement in */
/*                   the approximate solution x is possible. */

/*         info = 8  gtol is too small. fvec is orthogonal to the */
/*                   columns of the jacobian to machine precision. */

/*       nfev is an integer output variable set to the number of */
/*         calls to fcn. */

/*       fjac is an output m by n array. the upper n by n submatrix */
/*         of fjac contains an upper triangular matrix r with */
/*         diagonal elements of nonincreasing magnitude such that */

/*                t     t           t */
/*               p *(jac *jac)*p = r *r, */

/*         where p is a permutation matrix and jac is the final */
/*         calculated jacobian. column j of p is column ipvt(j) */
/*         (see below) of the identity matrix. the lower trapezoidal */
/*         part of fjac contains information generated during */
/*         the computation of r. */

/*       ldfjac is a positive integer input variable not less than m */
/*         which specifies the leading dimension of the array fjac. */

/*       ipvt is an integer output array of length n. ipvt */
/*         defines a permutation matrix p such that jac*p = q*r, */
/*         where jac is the final calculated jacobian, q is */
/*         orthogonal (not stored), and r is upper triangular */
/*         with diagonal elements of nonincreasing magnitude. */
/*         column j of p is column ipvt(j) of the identity matrix. */

/*       qtf is an output array of length n which contains */
/*         the first n elements of the vector (q transpose)*fvec. */

/*       wa1, wa2, and wa3 are work arrays of length n. */

/*       wa4 is a work array of length m. */

/*     subprograms called */

/*       user-supplied ...... fcn */

/*       minpack-supplied ... dpmpar,enorm,fdjac2,lmpar,qrfac */

/*       fortran-supplied ... dabs,dmax1,dmin1,dsqrt,mod */

/*     argonne national laboratory. minpack project. march 1980. */
/*     burton s. garbow, kenneth e. hillstrom, jorge j. more */

/*     ********** */
    /* Parameter adjustments */
    --wa4;
    --wa3;
    --wa2;
    --wa1;
    --qtf;
    --ipvt;
    fjac_dim1 = *ldfjac;
    fjac_offset = fjac_dim1 + 1;
    fjac -= fjac_offset;
    --diag;
    --fvec;
    --x;

    /* Function Body */

/*     epsmch is the machine precision. */

    epsmch = dpmpar_(&c__1);

    *info = 0;
    iflag = 0;
    *nfev = 0;

/*     check the input parameters for errors. */

    if (*n <= 0 || *m < *n || *ldfjac < *m || *ftol < zero || *xtol < zero || 
	    *gtol < zero || *maxfev <= 0 || *factor <= zero) {
	goto L300;
    }
    if (*mode != 2) {
	goto L20;
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (diag[j] <= zero) {
	    goto L300;
	}
/* L10: */
    }
L20:

/*     evaluate the function at the starting point */
/*     and calculate its norm. */

    iflag = 1;
    (*fcn)(m, n, &x[1], &fvec[1], &iflag);
    *nfev = 1;
    if (iflag < 0) {
	goto L300;
    }
    fnorm = enorm_(m, &fvec[1]);

/*     initialize levenberg-marquardt parameter and iteration counter. */

    par = zero;
    iter = 1;

/*     beginning of the outer loop. */

L30:

/*        calculate the jacobian matrix. */

    iflag = 2;
    fdjac2_(fcn, m, n, &x[1], &fvec[1], &fjac[fjac_offset], ldfjac, &iflag, 
	    epsfcn, &wa4[1]);
    *nfev += *n;
    if (iflag < 0) {
	goto L300;
    }

/*        if requested, call fcn to enable printing of iterates. */

    if (*nprint <= 0) {
	goto L40;
    }
    iflag = 0;
    if ((iter - 1) % *nprint == 0) {
	(*fcn)(m, n, &x[1], &fvec[1], &iflag);
    }
    if (iflag < 0) {
	goto L300;
    }
L40:

/*        compute the qr factorization of the jacobian. */

    qrfac_(m, n, &fjac[fjac_offset], ldfjac, &c__1, &ipvt[1], n, &wa1[1], &
	    wa2[1], &wa3[1]);

/*        on the first iteration and if mode is 1, scale according */
/*        to the norms of the columns of the initial jacobian. */

    if (iter != 1) {
	goto L80;
    }
    if (*mode == 2) {
	goto L60;
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	diag[j] = wa2[j];
	if (wa2[j] == zero) {
	    diag[j] = one;
	}
/* L50: */
    }
L60:

/*        on the first iteration, calculate the norm of the scaled x */
/*        and initialize the step bound delta. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	wa3[j] = diag[j] * x[j];
/* L70: */
    }
    xnorm = enorm_(n, &wa3[1]);
    delta = *factor * xnorm;
    if (delta == zero) {
	delta = *factor;
    }
L80:

/*        form (q transpose)*fvec and store the first n components in */
/*        qtf. */

    i__1 = *m;
    for (i = 1; i <= i__1; ++i) {
	wa4[i] = fvec[i];
/* L90: */
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (fjac[j + j * fjac_dim1] == zero) {
	    goto L120;
	}
	sum = zero;
	i__2 = *m;
	for (i = j; i <= i__2; ++i) {
	    sum += fjac[i + j * fjac_dim1] * wa4[i];
/* L100: */
	}
	temp = -sum / fjac[j + j * fjac_dim1];
	i__2 = *m;
	for (i = j; i <= i__2; ++i) {
	    wa4[i] += fjac[i + j * fjac_dim1] * temp;
/* L110: */
	}
L120:
	fjac[j + j * fjac_dim1] = wa1[j];
	qtf[j] = wa4[j];
/* L130: */
    }

/*        compute the norm of the scaled gradient. */

    gnorm = zero;
    if (fnorm == zero) {
	goto L170;
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	l = ipvt[j];
	if (wa2[l] == zero) {
	    goto L150;
	}
	sum = zero;
	i__2 = j;
	for (i = 1; i <= i__2; ++i) {
	    sum += fjac[i + j * fjac_dim1] * (qtf[i] / fnorm);
/* L140: */
	}
/* Computing MAX */
	d__2 = gnorm, d__3 = (d__1 = sum / wa2[l], abs(d__1));
	gnorm = max(d__2,d__3);
L150:
/* L160: */
	;
    }
L170:

/*        test for convergence of the gradient norm. */

    if (gnorm <= *gtol) {
	*info = 4;
    }
    if (*info != 0) {
	goto L300;
    }

/*        rescale if necessary. */

    if (*mode == 2) {
	goto L190;
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	d__1 = diag[j], d__2 = wa2[j];
	diag[j] = max(d__1,d__2);
/* L180: */
    }
L190:

/*        beginning of the inner loop. */

L200:

/*           determine the levenberg-marquardt parameter. */

    lmpar_(n, &fjac[fjac_offset], ldfjac, &ipvt[1], &diag[1], &qtf[1], &delta,
	     &par, &wa1[1], &wa2[1], &wa3[1], &wa4[1]);

/*           store the direction p and x + p. calculate the norm of p. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	wa1[j] = -wa1[j];
	wa2[j] = x[j] + wa1[j];
	wa3[j] = diag[j] * wa1[j];
/* L210: */
    }
    pnorm = enorm_(n, &wa3[1]);

/*           on the first iteration, adjust the initial step bound. */

    if (iter == 1) {
	delta = min(delta,pnorm);
    }

/*           evaluate the function at x + p and calculate its norm. */

    iflag = 1;
    (*fcn)(m, n, &wa2[1], &wa4[1], &iflag);
    ++(*nfev);
    if (iflag < 0) {
	goto L300;
    }
    fnorm1 = enorm_(m, &wa4[1]);

/*           compute the scaled actual reduction. */

    actred = -one;
    if (p1 * fnorm1 < fnorm) {
/* Computing 2nd power */
	d__1 = fnorm1 / fnorm;
	actred = one - d__1 * d__1;
    }

/*           compute the scaled predicted reduction and */
/*           the scaled directional derivative. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	wa3[j] = zero;
	l = ipvt[j];
	temp = wa1[l];
	i__2 = j;
	for (i = 1; i <= i__2; ++i) {
	    wa3[i] += fjac[i + j * fjac_dim1] * temp;
/* L220: */
	}
/* L230: */
    }
    temp1 = enorm_(n, &wa3[1]) / fnorm;
    temp2 = sqrt(par) * pnorm / fnorm;
/* Computing 2nd power */
    d__1 = temp1;
/* Computing 2nd power */
    d__2 = temp2;
    prered = d__1 * d__1 + d__2 * d__2 / p5;
/* Computing 2nd power */
    d__1 = temp1;
/* Computing 2nd power */
    d__2 = temp2;
    dirder = -(d__1 * d__1 + d__2 * d__2);

/*           compute the ratio of the actual to the predicted */
/*           reduction. */

    ratio = zero;
    if (prered != zero) {
	ratio = actred / prered;
    }

/*           update the step bound. */

    if (ratio > p25) {
	goto L240;
    }
    if (actred >= zero) {
	temp = p5;
    }
    if (actred < zero) {
	temp = p5 * dirder / (dirder + p5 * actred);
    }
    if (p1 * fnorm1 >= fnorm || temp < p1) {
	temp = p1;
    }
/* Computing MIN */
    d__1 = delta, d__2 = pnorm / p1;
    delta = temp * min(d__1,d__2);
    par /= temp;
    goto L260;
L240:
    if (par != zero && ratio < p75) {
	goto L250;
    }
    delta = pnorm / p5;
    par = p5 * par;
L250:
L260:

/*           test for successful iteration. */

    if (ratio < p0001) {
	goto L290;
    }

/*           successful iteration. update x, fvec, and their norms. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	x[j] = wa2[j];
	wa2[j] = diag[j] * x[j];
/* L270: */
    }
    i__1 = *m;
    for (i = 1; i <= i__1; ++i) {
	fvec[i] = wa4[i];
/* L280: */
    }
    xnorm = enorm_(n, &wa2[1]);
    fnorm = fnorm1;
    ++iter;
L290:

/*           tests for convergence. */

    if (abs(actred) <= *ftol && prered <= *ftol && p5 * ratio <= one) {
	*info = 1;
    }
    if (delta <= *xtol * xnorm) {
	*info = 2;
    }
    if (abs(actred) <= *ftol && prered <= *ftol && p5 * ratio <= one && *info 
	    == 2) {
	*info = 3;
    }
    if (*info != 0) {
	goto L300;
    }

/*           tests for termination and stringent tolerances. */

    if (*nfev >= *maxfev) {
	*info = 5;
    }
    if (abs(actred) <= epsmch && prered <= epsmch && p5 * ratio <= one) {
	*info = 6;
    }
    if (delta <= epsmch * xnorm) {
	*info = 7;
    }
    if (gnorm <= epsmch) {
	*info = 8;
    }
    if (*info != 0) {
	goto L300;
    }

/*           end of the inner loop. repeat if iteration unsuccessful. */

    if (ratio < p0001) {
	goto L200;
    }

/*        end of the outer loop. */

    goto L30;
L300:

/*     termination, either normal or user imposed. */

    if (iflag < 0) {
	*info = iflag;
    }
    iflag = 0;
    if (*nprint > 0) {
	(*fcn)(m, n, &x[1], &fvec[1], &iflag);
    }
    return 0;

/*     last card of subroutine lmdif. */

} /* lmdif_ */

