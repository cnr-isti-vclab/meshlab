/////////////////////////////////////////////////////////////////////////////////
// 
//  Solution of linear systems involved in the Levenberg - Marquardt
//  minimization algorithm
//  Copyright (C) 2004  Manolis Lourakis (lourakis at ics forth gr)
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


/* Solvers for the linear systems Ax=b. Solvers should NOT modify their A & B arguments! */


#ifndef LM_REAL // not included by Axb.c
#error This file should not be compiled directly!
#endif


#ifdef LINSOLVERS_RETAIN_MEMORY
#define __STATIC__ static
#else
#define __STATIC__ // empty
#endif /* LINSOLVERS_RETAIN_MEMORY */

#ifdef HAVE_LAPACK

/* prototypes of LAPACK routines */

#define GEQRF LM_ADD_PREFIX(geqrf_)
#define ORGQR LM_ADD_PREFIX(orgqr_)
#define TRTRS LM_ADD_PREFIX(trtrs_)
#define POTF2 LM_ADD_PREFIX(potf2_)
#define POTRF LM_ADD_PREFIX(potrf_)
#define GETRF LM_ADD_PREFIX(getrf_)
#define GETRS LM_ADD_PREFIX(getrs_)
#define GESVD LM_ADD_PREFIX(gesvd_)
#define GESDD LM_ADD_PREFIX(gesdd_)

/* QR decomposition */
extern int GEQRF(int *m, int *n, LM_REAL *a, int *lda, LM_REAL *tau, LM_REAL *work, int *lwork, int *info);
extern int ORGQR(int *m, int *n, int *k, LM_REAL *a, int *lda, LM_REAL *tau, LM_REAL *work, int *lwork, int *info);

/* solution of triangular systems */
extern int TRTRS(char *uplo, char *trans, char *diag, int *n, int *nrhs, LM_REAL *a, int *lda, LM_REAL *b, int *ldb, int *info);

/* cholesky decomposition */
extern int POTF2(char *uplo, int *n, LM_REAL *a, int *lda, int *info);
extern int POTRF(char *uplo, int *n, LM_REAL *a, int *lda, int *info); /* block version of dpotf2 */

/* LU decomposition and systems solution */
extern int GETRF(int *m, int *n, LM_REAL *a, int *lda, int *ipiv, int *info);
extern int GETRS(char *trans, int *n, int *nrhs, LM_REAL *a, int *lda, int *ipiv, LM_REAL *b, int *ldb, int *info);

/* Singular Value Decomposition (SVD) */
extern int GESVD(char *jobu, char *jobvt, int *m, int *n, LM_REAL *a, int *lda, LM_REAL *s, LM_REAL *u, int *ldu,
                   LM_REAL *vt, int *ldvt, LM_REAL *work, int *lwork, int *info);

/* lapack 3.0 new SVD routine, faster than xgesvd().
 * In case that your version of LAPACK does not include them, use the above two older routines
 */
extern int GESDD(char *jobz, int *m, int *n, LM_REAL *a, int *lda, LM_REAL *s, LM_REAL *u, int *ldu, LM_REAL *vt, int *ldvt,
                   LM_REAL *work, int *lwork, int *iwork, int *info);

/* precision-specific definitions */
#define AX_EQ_B_QR LM_ADD_PREFIX(Ax_eq_b_QR)
#define AX_EQ_B_QRLS LM_ADD_PREFIX(Ax_eq_b_QRLS)
#define AX_EQ_B_CHOL LM_ADD_PREFIX(Ax_eq_b_Chol)
#define AX_EQ_B_LU LM_ADD_PREFIX(Ax_eq_b_LU)
#define AX_EQ_B_SVD LM_ADD_PREFIX(Ax_eq_b_SVD)

/*
 * This function returns the solution of Ax = b
 *
 * The function is based on QR decomposition with explicit computation of Q:
 * If A=Q R with Q orthogonal and R upper triangular, the linear system becomes
 * Q R x = b or R x = Q^T b.
 * The last equation can be solved directly.
 *
 * A is mxm, b is mx1
 *
 * The function returns 0 in case of error, 1 if successfull
 *
 * This function is often called repetitively to solve problems of identical
 * dimensions. To avoid repetitive malloc's and free's, allocated memory is
 * retained between calls and free'd-malloc'ed when not of the appropriate size.
 * A call with NULL as the first argument forces this memory to be released.
 */
int AX_EQ_B_QR(LM_REAL *A, LM_REAL *B, LM_REAL *x, int m)
{
__STATIC__ LM_REAL *buf=NULL;
__STATIC__ int buf_sz=0;

LM_REAL *a, *qtb, *tau, *r, *work;
int a_sz, qtb_sz, tau_sz, r_sz, tot_sz;
register int i, j;
int info, worksz, nrhs=1;
register LM_REAL sum;

    if(!A)
#ifdef LINSOLVERS_RETAIN_MEMORY
    {
      if(buf) free(buf);
      buf=NULL;
      buf_sz=0;

      return 1;
    }
#else
      return 1; /* NOP */
#endif /* LINSOLVERS_RETAIN_MEMORY */
   
    /* calculate required memory size */
    a_sz=m*m;
    qtb_sz=m;
    tau_sz=m;
    r_sz=m*m; /* only the upper triangular part really needed */
    worksz=3*m; /* this is probably too much */
    tot_sz=a_sz + qtb_sz + tau_sz + r_sz + worksz;

#ifdef LINSOLVERS_RETAIN_MEMORY
    if(tot_sz>buf_sz){ /* insufficient memory, allocate a "big" memory chunk at once */
      if(buf) free(buf); /* free previously allocated memory */

      buf_sz=tot_sz;
      buf=(LM_REAL *)malloc(buf_sz*sizeof(LM_REAL));
      if(!buf){
        fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_QR) "() failed!\n");
        exit(1);
      }
    }
#else
      buf_sz=tot_sz;
      buf=(LM_REAL *)malloc(buf_sz*sizeof(LM_REAL));
      if(!buf){
        fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_QR) "() failed!\n");
        exit(1);
      }
#endif /* LINSOLVERS_RETAIN_MEMORY */

    a=buf;
    qtb=a+a_sz;
    tau=qtb+qtb_sz;
    r=tau+tau_sz;
    work=r+r_sz;

  /* store A (column major!) into a */
	for(i=0; i<m; i++)
		for(j=0; j<m; j++)
			a[i+j*m]=A[i*m+j];

  /* QR decomposition of A */
  GEQRF((int *)&m, (int *)&m, a, (int *)&m, tau, work, (int *)&worksz, (int *)&info);
  /* error treatment */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", GEQRF) " in ", AX_EQ_B_QR) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT(RCAT("Unknown LAPACK error %d for ", GEQRF) " in ", AX_EQ_B_QR) "()\n", info);
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

      return 0;
    }
  }

  /* R is stored in the upper triangular part of a; copy it in r so that ORGQR() below won't destroy it */ 
  for(i=0; i<r_sz; i++)
    r[i]=a[i];

  /* compute Q using the elementary reflectors computed by the above decomposition */
  ORGQR((int *)&m, (int *)&m, (int *)&m, a, (int *)&m, tau, work, (int *)&worksz, (int *)&info);
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", ORGQR) " in ", AX_EQ_B_QR) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT("Unknown LAPACK error (%d) in ", AX_EQ_B_QR) "()\n", info);
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

      return 0;
    }
  }

  /* Q is now in a; compute Q^T b in qtb */
  for(i=0; i<m; i++){
    for(j=0, sum=0.0; j<m; j++)
      sum+=a[i*m+j]*B[j];
    qtb[i]=sum;
  }

  /* solve the linear system R x = Q^t b */
  TRTRS("U", "N", "N", (int *)&m, (int *)&nrhs, r, (int *)&m, qtb, (int *)&m, &info);
  /* error treatment */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", TRTRS) " in ", AX_EQ_B_QR) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT("LAPACK error: the %d-th diagonal element of A is zero (singular matrix) in ", AX_EQ_B_QR) "()\n", info);
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

      return 0;
    }
  }

	/* copy the result in x */
	for(i=0; i<m; i++)
    x[i]=qtb[i];

#ifndef LINSOLVERS_RETAIN_MEMORY
  free(buf);
#endif

	return 1;
}

/*
 * This function returns the solution of min_x ||Ax - b||
 *
 * || . || is the second order (i.e. L2) norm. This is a least squares technique that
 * is based on QR decomposition:
 * If A=Q R with Q orthogonal and R upper triangular, the normal equations become
 * (A^T A) x = A^T b  or (R^T Q^T Q R) x = A^T b or (R^T R) x = A^T b.
 * This amounts to solving R^T y = A^T b for y and then R x = y for x
 * Note that Q does not need to be explicitly computed
 *
 * A is mxn, b is mx1
 *
 * The function returns 0 in case of error, 1 if successfull
 *
 * This function is often called repetitively to solve problems of identical
 * dimensions. To avoid repetitive malloc's and free's, allocated memory is
 * retained between calls and free'd-malloc'ed when not of the appropriate size.
 * A call with NULL as the first argument forces this memory to be released.
 */
int AX_EQ_B_QRLS(LM_REAL *A, LM_REAL *B, LM_REAL *x, int m, int n)
{
__STATIC__ LM_REAL *buf=NULL;
__STATIC__ int buf_sz=0;

LM_REAL *a, *atb, *tau, *r, *work;
int a_sz, atb_sz, tau_sz, r_sz, tot_sz;
register int i, j;
int info, worksz, nrhs=1;
register LM_REAL sum;
   
    if(!A)
#ifdef LINSOLVERS_RETAIN_MEMORY
    {
      if(buf) free(buf);
      buf=NULL;
      buf_sz=0;

      return 1;
    }
#else
      return 1; /* NOP */
#endif /* LINSOLVERS_RETAIN_MEMORY */
   
    if(m<n){
		  fprintf(stderr, RCAT("Normal equations require that the number of rows is greater than number of columns in ", AX_EQ_B_QRLS) "() [%d x %d]! -- try transposing\n", m, n);
		  exit(1);
	  }
      
    /* calculate required memory size */
    a_sz=m*n;
    atb_sz=n;
    tau_sz=n;
    r_sz=n*n;
    worksz=3*n; /* this is probably too much */
    tot_sz=a_sz + atb_sz + tau_sz + r_sz + worksz;

#ifdef LINSOLVERS_RETAIN_MEMORY
    if(tot_sz>buf_sz){ /* insufficient memory, allocate a "big" memory chunk at once */
      if(buf) free(buf); /* free previously allocated memory */

      buf_sz=tot_sz;
      buf=(LM_REAL *)malloc(buf_sz*sizeof(LM_REAL));
      if(!buf){
        fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_QRLS) "() failed!\n");
        exit(1);
      }
    }
#else
      buf_sz=tot_sz;
      buf=(LM_REAL *)malloc(buf_sz*sizeof(LM_REAL));
      if(!buf){
        fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_QRLS) "() failed!\n");
        exit(1);
      }
#endif /* LINSOLVERS_RETAIN_MEMORY */

    a=buf;
    atb=a+a_sz;
    tau=atb+atb_sz;
    r=tau+tau_sz;
    work=r+r_sz;

  /* store A (column major!) into a */
	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
			a[i+j*m]=A[i*n+j];

  /* compute A^T b in atb */
  for(i=0; i<n; i++){
    for(j=0, sum=0.0; j<m; j++)
      sum+=A[j*n+i]*B[j];
    atb[i]=sum;
  }

  /* QR decomposition of A */
  GEQRF((int *)&m, (int *)&n, a, (int *)&m, tau, work, (int *)&worksz, (int *)&info);
  /* error treatment */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", GEQRF) " in ", AX_EQ_B_QRLS) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT(RCAT("Unknown LAPACK error %d for ", GEQRF) " in ", AX_EQ_B_QRLS) "()\n", info);
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

      return 0;
    }
  }

  /* R is stored in the upper triangular part of a. Note that a is mxn while r nxn */
  for(j=0; j<n; j++){
    for(i=0; i<=j; i++)
      r[i+j*n]=a[i+j*m];

    /* lower part is zero */
    for(i=j+1; i<n; i++)
      r[i+j*n]=0.0;
  }

  /* solve the linear system R^T y = A^t b */
  TRTRS("U", "T", "N", (int *)&n, (int *)&nrhs, r, (int *)&n, atb, (int *)&n, &info);
  /* error treatment */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", TRTRS) " in ", AX_EQ_B_QRLS) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT("LAPACK error: the %d-th diagonal element of A is zero (singular matrix) in ", AX_EQ_B_QRLS) "()\n", info);
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

      return 0;
    }
  }

  /* solve the linear system R x = y */
  TRTRS("U", "N", "N", (int *)&n, (int *)&nrhs, r, (int *)&n, atb, (int *)&n, &info);
  /* error treatment */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", TRTRS) " in ", AX_EQ_B_QRLS) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT("LAPACK error: the %d-th diagonal element of A is zero (singular matrix) in ", AX_EQ_B_QRLS) "()\n", info);
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

      return 0;
    }
  }

	/* copy the result in x */
	for(i=0; i<n; i++)
    x[i]=atb[i];

#ifndef LINSOLVERS_RETAIN_MEMORY
  free(buf);
#endif

	return 1;
}

/*
 * This function returns the solution of Ax=b
 *
 * The function assumes that A is symmetric & postive definite and employs
 * the Cholesky decomposition:
 * If A=U^T U with U upper triangular, the system to be solved becomes
 * (U^T U) x = b
 * This amount to solving U^T y = b for y and then U x = y for x
 *
 * A is mxm, b is mx1
 *
 * The function returns 0 in case of error, 1 if successfull
 *
 * This function is often called repetitively to solve problems of identical
 * dimensions. To avoid repetitive malloc's and free's, allocated memory is
 * retained between calls and free'd-malloc'ed when not of the appropriate size.
 * A call with NULL as the first argument forces this memory to be released.
 */
int AX_EQ_B_CHOL(LM_REAL *A, LM_REAL *B, LM_REAL *x, int m)
{
__STATIC__ LM_REAL *buf=NULL;
__STATIC__ int buf_sz=0;

LM_REAL *a, *b;
int a_sz, b_sz, tot_sz;
register int i, j;
int info, nrhs=1;
   
    if(!A)
#ifdef LINSOLVERS_RETAIN_MEMORY
    {
      if(buf) free(buf);
      buf=NULL;
      buf_sz=0;

      return 1;
    }
#else
      return 1; /* NOP */
#endif /* LINSOLVERS_RETAIN_MEMORY */
   
    /* calculate required memory size */
    a_sz=m*m;
    b_sz=m;
    tot_sz=a_sz + b_sz;

#ifdef LINSOLVERS_RETAIN_MEMORY
    if(tot_sz>buf_sz){ /* insufficient memory, allocate a "big" memory chunk at once */
      if(buf) free(buf); /* free previously allocated memory */

      buf_sz=tot_sz;
      buf=(LM_REAL *)malloc(buf_sz*sizeof(LM_REAL));
      if(!buf){
        fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_CHOL) "() failed!\n");
        exit(1);
      }
    }
#else
      buf_sz=tot_sz;
      buf=(LM_REAL *)malloc(buf_sz*sizeof(LM_REAL));
      if(!buf){
        fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_CHOL) "() failed!\n");
        exit(1);
      }
#endif /* LINSOLVERS_RETAIN_MEMORY */

    a=buf;
    b=a+a_sz;

  /* store A (column major!) into a anb B into b */
	for(i=0; i<m; i++){
		for(j=0; j<m; j++)
			a[i+j*m]=A[i*m+j];

    b[i]=B[i];
  }

  /* Cholesky decomposition of A */
  POTF2("U", (int *)&m, a, (int *)&m, (int *)&info);
  /* error treatment */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", POTF2) " in ", AX_EQ_B_CHOL) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT(RCAT("LAPACK error: the leading minor of order %d is not positive definite,\nthe factorization could not be completed for ", POTF2) " in ", AX_EQ_B_CHOL) "()\n", info);
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

      return 0;
    }
  }

  /* solve the linear system U^T y = b */
  TRTRS("U", "T", "N", (int *)&m, (int *)&nrhs, a, (int *)&m, b, (int *)&m, &info);
  /* error treatment */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", TRTRS) " in ", AX_EQ_B_CHOL) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT("LAPACK error: the %d-th diagonal element of A is zero (singular matrix) in ", AX_EQ_B_CHOL) "()\n", info);
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

      return 0;
    }
  }

  /* solve the linear system U x = y */
  TRTRS("U", "N", "N", (int *)&m, (int *)&nrhs, a, (int *)&m, b, (int *)&m, &info);
  /* error treatment */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", TRTRS) "in ", AX_EQ_B_CHOL) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT("LAPACK error: the %d-th diagonal element of A is zero (singular matrix) in ", AX_EQ_B_CHOL) "()\n", info);
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

      return 0;
    }
  }

	/* copy the result in x */
	for(i=0; i<m; i++)
    x[i]=b[i];

#ifndef LINSOLVERS_RETAIN_MEMORY
  free(buf);
#endif

	return 1;
}

/*
 * This function returns the solution of Ax = b
 *
 * The function employs LU decomposition:
 * If A=L U with L lower and U upper triangular, then the original system
 * amounts to solving
 * L y = b, U x = y
 *
 * A is mxm, b is mx1
 *
 * The function returns 0 in case of error,
 * 1 if successfull
 *
 * This function is often called repetitively to solve problems of identical
 * dimensions. To avoid repetitive malloc's and free's, allocated memory is
 * retained between calls and free'd-malloc'ed when not of the appropriate size.
 * A call with NULL as the first argument forces this memory to be released.
 */
int AX_EQ_B_LU(LM_REAL *A, LM_REAL *B, LM_REAL *x, int m)
{
__STATIC__ LM_REAL *buf=NULL;
__STATIC__ int buf_sz=0;

int a_sz, ipiv_sz, b_sz, work_sz, tot_sz;
register int i, j;
int info, *ipiv, nrhs=1;
LM_REAL *a, *b, *work;
   
    if(!A)
#ifdef LINSOLVERS_RETAIN_MEMORY
    {
      if(buf) free(buf);
      buf=NULL;
      buf_sz=0;

      return 1;
    }
#else
      return 1; /* NOP */
#endif /* LINSOLVERS_RETAIN_MEMORY */
   
    /* calculate required memory size */
    ipiv_sz=m;
    a_sz=m*m;
    b_sz=m;
    work_sz=100*m; /* this is probably too much */
    tot_sz=ipiv_sz + a_sz + b_sz + work_sz; // ipiv_sz counted as LM_REAL here, no harm is done though

#ifdef LINSOLVERS_RETAIN_MEMORY
    if(tot_sz>buf_sz){ /* insufficient memory, allocate a "big" memory chunk at once */
      if(buf) free(buf); /* free previously allocated memory */

      buf_sz=tot_sz;
      buf=(LM_REAL *)malloc(buf_sz*sizeof(LM_REAL));
      if(!buf){
        fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_LU) "() failed!\n");
        exit(1);
      }
    }
#else
      buf_sz=tot_sz;
      buf=(LM_REAL *)malloc(buf_sz*sizeof(LM_REAL));
      if(!buf){
        fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_LU) "() failed!\n");
        exit(1);
      }
#endif /* LINSOLVERS_RETAIN_MEMORY */

    ipiv=(int *)buf;
    a=(LM_REAL *)(ipiv + ipiv_sz);
    b=a+a_sz;
    work=b+b_sz;

    /* store A (column major!) into a and B into b */
	  for(i=0; i<m; i++){
		  for(j=0; j<m; j++)
        a[i+j*m]=A[i*m+j];

      b[i]=B[i];
    }

  /* LU decomposition for A */
	GETRF((int *)&m, (int *)&m, a, (int *)&m, ipiv, (int *)&info);  
	if(info!=0){
		if(info<0){
      fprintf(stderr, RCAT(RCAT("argument %d of ", GETRF) " illegal in ", AX_EQ_B_LU) "()\n", -info);
			exit(1);
		}
		else{
      fprintf(stderr, RCAT(RCAT("singular matrix A for ", GETRF) " in ", AX_EQ_B_LU) "()\n");
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

			return 0;
		}
	}

  /* solve the system with the computed LU */
  GETRS("N", (int *)&m, (int *)&nrhs, a, (int *)&m, ipiv, b, (int *)&m, (int *)&info);
	if(info!=0){
		if(info<0){
			fprintf(stderr, RCAT(RCAT("argument %d of ", GETRS) " illegal in ", AX_EQ_B_LU) "()\n", -info);
			exit(1);
		}
		else{
			fprintf(stderr, RCAT(RCAT("unknown error for ", GETRS) " in ", AX_EQ_B_LU) "()\n");
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

			return 0;
		}
	}

	/* copy the result in x */
	for(i=0; i<m; i++){
		x[i]=b[i];
	}

#ifndef LINSOLVERS_RETAIN_MEMORY
  free(buf);
#endif

	return 1;
}

/*
 * This function returns the solution of Ax = b
 *
 * The function is based on SVD decomposition:
 * If A=U D V^T with U, V orthogonal and D diagonal, the linear system becomes
 * (U D V^T) x = b or x=V D^{-1} U^T b
 * Note that V D^{-1} U^T is the pseudoinverse A^+
 *
 * A is mxm, b is mx1.
 *
 * The function returns 0 in case of error, 1 if successfull
 *
 * This function is often called repetitively to solve problems of identical
 * dimensions. To avoid repetitive malloc's and free's, allocated memory is
 * retained between calls and free'd-malloc'ed when not of the appropriate size.
 * A call with NULL as the first argument forces this memory to be released.
 */
int AX_EQ_B_SVD(LM_REAL *A, LM_REAL *B, LM_REAL *x, int m)
{
__STATIC__ LM_REAL *buf=NULL;
__STATIC__ int buf_sz=0;
static LM_REAL eps=LM_CNST(-1.0);

register int i, j;
LM_REAL *a, *u, *s, *vt, *work;
int a_sz, u_sz, s_sz, vt_sz, tot_sz;
LM_REAL thresh, one_over_denom;
register LM_REAL sum;
int info, rank, worksz, *iwork, iworksz;
   
    if(!A)
#ifdef LINSOLVERS_RETAIN_MEMORY
    {
      if(buf) free(buf);
      buf=NULL;
      buf_sz=0;

      return 1;
    }
#else
      return 1; /* NOP */
#endif /* LINSOLVERS_RETAIN_MEMORY */
   
  /* calculate required memory size */
  worksz=16*m; /* more than needed */
  iworksz=8*m;
  a_sz=m*m;
  u_sz=m*m; s_sz=m; vt_sz=m*m;

  tot_sz=iworksz*sizeof(int) + (a_sz + u_sz + s_sz + vt_sz + worksz)*sizeof(LM_REAL);

#ifdef LINSOLVERS_RETAIN_MEMORY
  if(tot_sz>buf_sz){ /* insufficient memory, allocate a "big" memory chunk at once */
    if(buf) free(buf); /* free previously allocated memory */

    buf_sz=tot_sz;
    buf=(LM_REAL *)malloc(buf_sz);
    if(!buf){
      fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_SVD) "() failed!\n");
      exit(1);
    }
  }
#else
    buf_sz=tot_sz;
    buf=(LM_REAL *)malloc(buf_sz);
    if(!buf){
      fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_SVD) "() failed!\n");
      exit(1);
    }
#endif /* LINSOLVERS_RETAIN_MEMORY */

  iwork=(int *)buf;
  a=(LM_REAL *)(iwork+iworksz);
  /* store A (column major!) into a */
  for(i=0; i<m; i++)
    for(j=0; j<m; j++)
      a[i+j*m]=A[i*m+j];

  u=a + a_sz;
  s=u+u_sz;
  vt=s+s_sz;
  work=vt+vt_sz;

  /* SVD decomposition of A */
  GESVD("A", "A", (int *)&m, (int *)&m, a, (int *)&m, s, u, (int *)&m, vt, (int *)&m, work, (int *)&worksz, &info);
  //GESDD("A", (int *)&m, (int *)&m, a, (int *)&m, s, u, (int *)&m, vt, (int *)&m, work, (int *)&worksz, iwork, &info);

  /* error treatment */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT(RCAT("LAPACK error: illegal value for argument %d of ", GESVD), "/" GESDD) " in ", AX_EQ_B_SVD) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT("LAPACK error: dgesdd (dbdsdc)/dgesvd (dbdsqr) failed to converge in ", AX_EQ_B_SVD) "() [info=%d]\n", info);
#ifndef LINSOLVERS_RETAIN_MEMORY
      free(buf);
#endif

      return 0;
    }
  }

  if(eps<0.0){
    LM_REAL aux;

    /* compute machine epsilon */
    for(eps=LM_CNST(1.0); aux=eps+LM_CNST(1.0), aux-LM_CNST(1.0)>0.0; eps*=LM_CNST(0.5))
                                          ;
    eps*=LM_CNST(2.0);
  }

  /* compute the pseudoinverse in a */
	for(i=0; i<a_sz; i++) a[i]=0.0; /* initialize to zero */
  for(rank=0, thresh=eps*s[0]; rank<m && s[rank]>thresh; rank++){
    one_over_denom=LM_CNST(1.0)/s[rank];

    for(j=0; j<m; j++)
      for(i=0; i<m; i++)
        a[i*m+j]+=vt[rank+i*m]*u[j+rank*m]*one_over_denom;
  }

	/* compute A^+ b in x */
	for(i=0; i<m; i++){
	  for(j=0, sum=0.0; j<m; j++)
      sum+=a[i*m+j]*B[j];
    x[i]=sum;
  }

#ifndef LINSOLVERS_RETAIN_MEMORY
  free(buf);
#endif

	return 1;
}

/* undefine all. IT MUST REMAIN IN THIS POSITION IN FILE */
#undef AX_EQ_B_QR
#undef AX_EQ_B_QRLS
#undef AX_EQ_B_CHOL
#undef AX_EQ_B_LU
#undef AX_EQ_B_SVD

#undef GEQRF
#undef ORGQR
#undef TRTRS
#undef POTF2
#undef POTRF
#undef GETRF
#undef GETRS
#undef GESVD
#undef GESDD

#else // no LAPACK

/* precision-specific definitions */
#define AX_EQ_B_LU LM_ADD_PREFIX(Ax_eq_b_LU_noLapack)

/*
 * This function returns the solution of Ax = b
 *
 * The function employs LU decomposition followed by forward/back substitution (see 
 * also the LAPACK-based LU solver above)
 *
 * A is mxm, b is mx1
 *
 * The function returns 0 in case of error,
 * 1 if successfull
 *
 * This function is often called repetitively to solve problems of identical
 * dimensions. To avoid repetitive malloc's and free's, allocated memory is
 * retained between calls and free'd-malloc'ed when not of the appropriate size.
 * A call with NULL as the first argument forces this memory to be released.
 */
int AX_EQ_B_LU(LM_REAL *A, LM_REAL *B, LM_REAL *x, int m)
{
__STATIC__ void *buf=NULL;
__STATIC__ int buf_sz=0;

register int i, j, k;
int *idx, maxi=-1, idx_sz, a_sz, work_sz, tot_sz;
LM_REAL *a, *work, max, sum, tmp;

    if(!A)
#ifdef LINSOLVERS_RETAIN_MEMORY
    {
      if(buf) free(buf);
      buf=NULL;
      buf_sz=0;

      return 1;
    }
#else
    return 1; /* NOP */
#endif /* LINSOLVERS_RETAIN_MEMORY */
   
  /* calculate required memory size */
  idx_sz=m;
  a_sz=m*m;
  work_sz=m;
  tot_sz=idx_sz*sizeof(int) + (a_sz+work_sz)*sizeof(LM_REAL);

#ifdef LINSOLVERS_RETAIN_MEMORY
  if(tot_sz>buf_sz){ /* insufficient memory, allocate a "big" memory chunk at once */
    if(buf) free(buf); /* free previously allocated memory */

    buf_sz=tot_sz;
    buf=(void *)malloc(tot_sz);
    if(!buf){
      fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_LU) "() failed!\n");
      exit(1);
    }
  }
#else
    buf_sz=tot_sz;
    buf=(void *)malloc(tot_sz);
    if(!buf){
      fprintf(stderr, RCAT("memory allocation in ", AX_EQ_B_LU) "() failed!\n");
      exit(1);
    }
#endif /* LINSOLVERS_RETAIN_MEMORY */

  idx=(int *)buf;
  a=(LM_REAL *)(idx + idx_sz);
  work=a + a_sz;

  /* avoid destroying A, B by copying them to a, x resp. */
  for(i=0; i<m; ++i){ // B & 1st row of A
    a[i]=A[i];
    x[i]=B[i];
  }
  for(  ; i<a_sz; ++i) a[i]=A[i]; // copy A's remaining rows
  /****
  for(i=0; i<m; ++i){
    for(j=0; j<m; ++j)
      a[i*m+j]=A[i*m+j];
    x[i]=B[i];
  }
  ****/

  /* compute the LU decomposition of a row permutation of matrix a; the permutation itself is saved in idx[] */
	for(i=0; i<m; ++i){
		max=0.0;
		for(j=0; j<m; ++j)
			if((tmp=FABS(a[i*m+j]))>max)
        max=tmp;
		  if(max==0.0){
        fprintf(stderr, RCAT("Singular matrix A in ", AX_EQ_B_LU) "()!\n");
#ifndef LINSOLVERS_RETAIN_MEMORY
        free(buf);
#endif

        return 0;
      }
		  work[i]=LM_CNST(1.0)/max;
	}

	for(j=0; j<m; ++j){
		for(i=0; i<j; ++i){
			sum=a[i*m+j];
			for(k=0; k<i; ++k)
        sum-=a[i*m+k]*a[k*m+j];
			a[i*m+j]=sum;
		}
		max=0.0;
		for(i=j; i<m; ++i){
			sum=a[i*m+j];
			for(k=0; k<j; ++k)
        sum-=a[i*m+k]*a[k*m+j];
			a[i*m+j]=sum;
			if((tmp=work[i]*FABS(sum))>=max){
				max=tmp;
				maxi=i;
			}
		}
		if(j!=maxi){
			for(k=0; k<m; ++k){
				tmp=a[maxi*m+k];
				a[maxi*m+k]=a[j*m+k];
				a[j*m+k]=tmp;
			}
			work[maxi]=work[j];
		}
		idx[j]=maxi;
		if(a[j*m+j]==0.0)
      a[j*m+j]=LM_REAL_EPSILON;
		if(j!=m-1){
			tmp=LM_CNST(1.0)/(a[j*m+j]);
			for(i=j+1; i<m; ++i)
        a[i*m+j]*=tmp;
		}
	}

  /* The decomposition has now replaced a. Solve the linear system using
   * forward and back substitution
   */
	for(i=k=0; i<m; ++i){
		j=idx[i];
		sum=x[j];
		x[j]=x[i];
		if(k!=0)
			for(j=k-1; j<i; ++j)
        sum-=a[i*m+j]*x[j];
		else
      if(sum!=0.0)
			  k=i+1;
		x[i]=sum;
	}

	for(i=m-1; i>=0; --i){
		sum=x[i];
		for(j=i+1; j<m; ++j)
      sum-=a[i*m+j]*x[j];
		x[i]=sum/a[i*m+i];
	}

#ifndef LINSOLVERS_RETAIN_MEMORY
  free(buf);
#endif

  return 1;
}

/* undefine all. IT MUST REMAIN IN THIS POSITION IN FILE */
#undef AX_EQ_B_LU

#endif /* HAVE_LAPACK */
