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

#ifndef LM_REAL // not included by misc.c
#error This file should not be compiled directly!
#endif


/* precision-specific definitions */
#define LEVMAR_CHKJAC LM_ADD_PREFIX(levmar_chkjac)
#define LEVMAR_FDIF_FORW_JAC_APPROX LM_ADD_PREFIX(levmar_fdif_forw_jac_approx)
#define LEVMAR_FDIF_CENT_JAC_APPROX LM_ADD_PREFIX(levmar_fdif_cent_jac_approx)
#define LEVMAR_TRANS_MAT_MAT_MULT LM_ADD_PREFIX(levmar_trans_mat_mat_mult)
#define LEVMAR_COVAR LM_ADD_PREFIX(levmar_covar)
#define LEVMAR_STDDEV LM_ADD_PREFIX(levmar_stddev)
#define LEVMAR_CORCOEF LM_ADD_PREFIX(levmar_corcoef)
#define LEVMAR_BOX_CHECK LM_ADD_PREFIX(levmar_box_check)
#define LEVMAR_L2NRMXMY LM_ADD_PREFIX(levmar_L2nrmxmy)

#ifdef HAVE_LAPACK
#define LEVMAR_PSEUDOINVERSE LM_ADD_PREFIX(levmar_pseudoinverse)
static int LEVMAR_PSEUDOINVERSE(LM_REAL *A, LM_REAL *B, int m);

/* BLAS matrix multiplication & LAPACK SVD routines */
#ifdef LM_BLAS_PREFIX
#define GEMM LM_CAT_(LM_BLAS_PREFIX, LM_ADD_PREFIX(LM_CAT_(gemm, LM_BLAS_SUFFIX)))
#else
#define GEMM LM_ADD_PREFIX(LM_CAT_(gemm, LM_BLAS_SUFFIX))
#endif
/* C := alpha*op( A )*op( B ) + beta*C */
extern void GEMM(char *transa, char *transb, int *m, int *n, int *k,
          LM_REAL *alpha, LM_REAL *a, int *lda, LM_REAL *b, int *ldb, LM_REAL *beta, LM_REAL *c, int *ldc);

#define GESVD LM_ADD_PREFIX(gesvd_)
#define GESDD LM_ADD_PREFIX(gesdd_)
extern int GESVD(char *jobu, char *jobvt, int *m, int *n, LM_REAL *a, int *lda, LM_REAL *s, LM_REAL *u, int *ldu,
                 LM_REAL *vt, int *ldvt, LM_REAL *work, int *lwork, int *info);

/* lapack 3.0 new SVD routine, faster than xgesvd() */
extern int GESDD(char *jobz, int *m, int *n, LM_REAL *a, int *lda, LM_REAL *s, LM_REAL *u, int *ldu, LM_REAL *vt, int *ldvt,
                 LM_REAL *work, int *lwork, int *iwork, int *info);

/* cholesky decomposition */
#define POTF2 LM_ADD_PREFIX(potf2_)
extern int POTF2(char *uplo, int *n, LM_REAL *a, int *lda, int *info);

#define LEVMAR_CHOLESKY LM_ADD_PREFIX(levmar_chol)

#else
#define LEVMAR_LUINVERSE LM_ADD_PREFIX(levmar_LUinverse_noLapack)

static int LEVMAR_LUINVERSE(LM_REAL *A, LM_REAL *B, int m);
#endif /* HAVE_LAPACK */

/* blocked multiplication of the transpose of the nxm matrix a with itself (i.e. a^T a)
 * using a block size of bsize. The product is returned in b.
 * Since a^T a is symmetric, its computation can be speeded up by computing only its
 * upper triangular part and copying it to the lower part.
 *
 * More details on blocking can be found at 
 * http://www-2.cs.cmu.edu/afs/cs/academic/class/15213-f02/www/R07/section_a/Recitation07-SectionA.pdf
 */
void LEVMAR_TRANS_MAT_MAT_MULT(LM_REAL *a, LM_REAL *b, int n, int m)
{
#ifdef HAVE_LAPACK /* use BLAS matrix multiply */

LM_REAL alpha=LM_CNST(1.0), beta=LM_CNST(0.0);
  /* Fool BLAS to compute a^T*a avoiding transposing a: a is equivalent to a^T in column major,
   * therefore BLAS computes a*a^T with a and a*a^T in column major, which is equivalent to
   * computing a^T*a in row major!
   */
  GEMM("N", "T", &m, &m, &n, &alpha, a, &m, a, &m, &beta, b, &m);

#else /* no LAPACK, use blocking-based multiply */

register int i, j, k, jj, kk;
register LM_REAL sum, *bim, *akm;
const int bsize=__BLOCKSZ__;

#define __MIN__(x, y) (((x)<=(y))? (x) : (y))
#define __MAX__(x, y) (((x)>=(y))? (x) : (y))

  /* compute upper triangular part using blocking */
  for(jj=0; jj<m; jj+=bsize){
    for(i=0; i<m; ++i){
      bim=b+i*m;
      for(j=__MAX__(jj, i); j<__MIN__(jj+bsize, m); ++j)
        bim[j]=0.0; //b[i*m+j]=0.0;
    }

    for(kk=0; kk<n; kk+=bsize){
      for(i=0; i<m; ++i){
        bim=b+i*m;
        for(j=__MAX__(jj, i); j<__MIN__(jj+bsize, m); ++j){
          sum=0.0;
          for(k=kk; k<__MIN__(kk+bsize, n); ++k){
            akm=a+k*m;
            sum+=akm[i]*akm[j]; //a[k*m+i]*a[k*m+j];
          }
          bim[j]+=sum; //b[i*m+j]+=sum;
        }
      }
    }
  }

  /* copy upper triangular part to the lower one */
  for(i=0; i<m; ++i)
    for(j=0; j<i; ++j)
      b[i*m+j]=b[j*m+i];

#undef __MIN__
#undef __MAX__

#endif /* HAVE_LAPACK */
}

/* forward finite difference approximation to the Jacobian of func */
void LEVMAR_FDIF_FORW_JAC_APPROX(
    void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata),
													   /* function to differentiate */
    LM_REAL *p,              /* I: current parameter estimate, mx1 */
    LM_REAL *hx,             /* I: func evaluated at p, i.e. hx=func(p), nx1 */
    LM_REAL *hxx,            /* W/O: work array for evaluating func(p+delta), nx1 */
    LM_REAL delta,           /* increment for computing the Jacobian */
    LM_REAL *jac,            /* O: array for storing approximated Jacobian, nxm */
    int m,
    int n,
    void *adata)
{
register int i, j;
LM_REAL tmp;
register LM_REAL d;

  for(j=0; j<m; ++j){
    /* determine d=max(1E-04*|p[j]|, delta), see HZ */
    d=LM_CNST(1E-04)*p[j]; // force evaluation
    d=FABS(d);
    if(d<delta)
      d=delta;

    tmp=p[j];
    p[j]+=d;

    (*func)(p, hxx, m, n, adata);

    p[j]=tmp; /* restore */

    d=LM_CNST(1.0)/d; /* invert so that divisions can be carried out faster as multiplications */
    for(i=0; i<n; ++i){
      jac[i*m+j]=(hxx[i]-hx[i])*d;
    }
  }
}

/* central finite difference approximation to the Jacobian of func */
void LEVMAR_FDIF_CENT_JAC_APPROX(
    void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata),
													   /* function to differentiate */
    LM_REAL *p,              /* I: current parameter estimate, mx1 */
    LM_REAL *hxm,            /* W/O: work array for evaluating func(p-delta), nx1 */
    LM_REAL *hxp,            /* W/O: work array for evaluating func(p+delta), nx1 */
    LM_REAL delta,           /* increment for computing the Jacobian */
    LM_REAL *jac,            /* O: array for storing approximated Jacobian, nxm */
    int m,
    int n,
    void *adata)
{
register int i, j;
LM_REAL tmp;
register LM_REAL d;

  for(j=0; j<m; ++j){
    /* determine d=max(1E-04*|p[j]|, delta), see HZ */
    d=LM_CNST(1E-04)*p[j]; // force evaluation
    d=FABS(d);
    if(d<delta)
      d=delta;

    tmp=p[j];
    p[j]-=d;
    (*func)(p, hxm, m, n, adata);

    p[j]=tmp+d;
    (*func)(p, hxp, m, n, adata);
    p[j]=tmp; /* restore */

    d=LM_CNST(0.5)/d; /* invert so that divisions can be carried out faster as multiplications */
    for(i=0; i<n; ++i){
      jac[i*m+j]=(hxp[i]-hxm[i])*d;
    }
  }
}

/* 
 * Check the Jacobian of a n-valued nonlinear function in m variables
 * evaluated at a point p, for consistency with the function itself.
 *
 * Based on fortran77 subroutine CHKDER by
 * Burton S. Garbow, Kenneth E. Hillstrom, Jorge J. More
 * Argonne National Laboratory. MINPACK project. March 1980.
 *
 *
 * func points to a function from R^m --> R^n: Given a p in R^m it yields hx in R^n
 * jacf points to a function implementing the Jacobian of func, whose correctness
 *     is to be tested. Given a p in R^m, jacf computes into the nxm matrix j the
 *     Jacobian of func at p. Note that row i of j corresponds to the gradient of
 *     the i-th component of func, evaluated at p.
 * p is an input array of length m containing the point of evaluation.
 * m is the number of variables
 * n is the number of functions
 * adata points to possible additional data and is passed uninterpreted
 *     to func, jacf.
 * err is an array of length n. On output, err contains measures
 *     of correctness of the respective gradients. if there is
 *     no severe loss of significance, then if err[i] is 1.0 the
 *     i-th gradient is correct, while if err[i] is 0.0 the i-th
 *     gradient is incorrect. For values of err between 0.0 and 1.0,
 *     the categorization is less certain. In general, a value of
 *     err[i] greater than 0.5 indicates that the i-th gradient is
 *     probably correct, while a value of err[i] less than 0.5
 *     indicates that the i-th gradient is probably incorrect.
 *
 *
 * The function does not perform reliably if cancellation or
 * rounding errors cause a severe loss of significance in the
 * evaluation of a function. therefore, none of the components
 * of p should be unusually small (in particular, zero) or any
 * other value which may cause loss of significance.
 */

void LEVMAR_CHKJAC(
    void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata),
    void (*jacf)(LM_REAL *p, LM_REAL *j, int m, int n, void *adata),
    LM_REAL *p, int m, int n, void *adata, LM_REAL *err)
{
LM_REAL factor=LM_CNST(100.0);
LM_REAL one=LM_CNST(1.0);
LM_REAL zero=LM_CNST(0.0);
LM_REAL *fvec, *fjac, *pp, *fvecp, *buf;

register int i, j;
LM_REAL eps, epsf, temp, epsmch;
LM_REAL epslog;
int fvec_sz=n, fjac_sz=n*m, pp_sz=m, fvecp_sz=n;

  epsmch=LM_REAL_EPSILON;
  eps=(LM_REAL)sqrt(epsmch);

  buf=(LM_REAL *)malloc((fvec_sz + fjac_sz + pp_sz + fvecp_sz)*sizeof(LM_REAL));
  if(!buf){
    fprintf(stderr, LCAT(LEVMAR_CHKJAC, "(): memory allocation request failed\n"));
    exit(1);
  }
  fvec=buf;
  fjac=fvec+fvec_sz;
  pp=fjac+fjac_sz;
  fvecp=pp+pp_sz;

  /* compute fvec=func(p) */
  (*func)(p, fvec, m, n, adata);

  /* compute the Jacobian at p */
  (*jacf)(p, fjac, m, n, adata);

  /* compute pp */
  for(j=0; j<m; ++j){
    temp=eps*FABS(p[j]);
    if(temp==zero) temp=eps;
    pp[j]=p[j]+temp;
  }

  /* compute fvecp=func(pp) */
  (*func)(pp, fvecp, m, n, adata);

  epsf=factor*epsmch;
  epslog=(LM_REAL)log10(eps);

  for(i=0; i<n; ++i)
    err[i]=zero;

  for(j=0; j<m; ++j){
    temp=FABS(p[j]);
    if(temp==zero) temp=one;

    for(i=0; i<n; ++i)
      err[i]+=temp*fjac[i*m+j];
  }

  for(i=0; i<n; ++i){
    temp=one;
    if(fvec[i]!=zero && fvecp[i]!=zero && FABS(fvecp[i]-fvec[i])>=epsf*FABS(fvec[i]))
        temp=eps*FABS((fvecp[i]-fvec[i])/eps - err[i])/(FABS(fvec[i])+FABS(fvecp[i]));
    err[i]=one;
    if(temp>epsmch && temp<eps)
        err[i]=((LM_REAL)log10(temp) - epslog)/epslog;
    if(temp>=eps) err[i]=zero;
  }

  free(buf);

  return;
}

#ifdef HAVE_LAPACK
/*
 * This function computes the pseudoinverse of a square matrix A
 * into B using SVD. A and B can coincide
 * 
 * The function returns 0 in case of error (e.g. A is singular),
 * the rank of A if successfull
 *
 * A, B are mxm
 *
 */
static int LEVMAR_PSEUDOINVERSE(LM_REAL *A, LM_REAL *B, int m)
{
LM_REAL *buf=NULL;
int buf_sz=0;
static LM_REAL eps=LM_CNST(-1.0);

register int i, j;
LM_REAL *a, *u, *s, *vt, *work;
int a_sz, u_sz, s_sz, vt_sz, tot_sz;
LM_REAL thresh, one_over_denom;
int info, rank, worksz, *iwork, iworksz;
   
  /* calculate required memory size */
  worksz=16*m; /* more than needed */
  iworksz=8*m;
  a_sz=m*m;
  u_sz=m*m; s_sz=m; vt_sz=m*m;

  tot_sz=iworksz*sizeof(int) + (a_sz + u_sz + s_sz + vt_sz + worksz)*sizeof(LM_REAL);

    buf_sz=tot_sz;
    buf=(LM_REAL *)malloc(buf_sz);
    if(!buf){
      fprintf(stderr, RCAT("memory allocation in ", LEVMAR_PSEUDOINVERSE) "() failed!\n");
      exit(1);
    }

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
      fprintf(stderr, RCAT(RCAT(RCAT("LAPACK error: illegal value for argument %d of ", GESVD), "/" GESDD) " in ", LEVMAR_PSEUDOINVERSE) "()\n", -info);
      exit(1);
    }
    else{
      fprintf(stderr, RCAT("LAPACK error: dgesdd (dbdsdc)/dgesvd (dbdsqr) failed to converge in ", LEVMAR_PSEUDOINVERSE) "() [info=%d]\n", info);
      free(buf);

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

  /* compute the pseudoinverse in B */
	for(i=0; i<a_sz; i++) B[i]=0.0; /* initialize to zero */
  for(rank=0, thresh=eps*s[0]; rank<m && s[rank]>thresh; rank++){
    one_over_denom=LM_CNST(1.0)/s[rank];

    for(j=0; j<m; j++)
      for(i=0; i<m; i++)
        B[i*m+j]+=vt[rank+i*m]*u[j+rank*m]*one_over_denom;
  }

  free(buf);

	return rank;
}
#else // no LAPACK

/*
 * This function computes the inverse of A in B. A and B can coincide
 *
 * The function employs LAPACK-free LU decomposition of A to solve m linear
 * systems A*B_i=I_i, where B_i and I_i are the i-th columns of B and I.
 *
 * A and B are mxm
 *
 * The function returns 0 in case of error,
 * 1 if successfull
 *
 */
static int LEVMAR_LUINVERSE(LM_REAL *A, LM_REAL *B, int m)
{
void *buf=NULL;
int buf_sz=0;

register int i, j, k, l;
int *idx, maxi=-1, idx_sz, a_sz, x_sz, work_sz, tot_sz;
LM_REAL *a, *x, *work, max, sum, tmp;

  /* calculate required memory size */
  idx_sz=m;
  a_sz=m*m;
  x_sz=m;
  work_sz=m;
  tot_sz=idx_sz*sizeof(int) + (a_sz+x_sz+work_sz)*sizeof(LM_REAL);

  buf_sz=tot_sz;
  buf=(void *)malloc(tot_sz);
  if(!buf){
    fprintf(stderr, RCAT("memory allocation in ", LEVMAR_LUINVERSE) "() failed!\n");
    exit(1);
  }

  idx=(int *)buf;
  a=(LM_REAL *)(idx + idx_sz);
  x=a + a_sz;
  work=x + x_sz;

  /* avoid destroying A by copying it to a */
  for(i=0; i<a_sz; ++i) a[i]=A[i];

  /* compute the LU decomposition of a row permutation of matrix a; the permutation itself is saved in idx[] */
	for(i=0; i<m; ++i){
		max=0.0;
		for(j=0; j<m; ++j)
			if((tmp=FABS(a[i*m+j]))>max)
        max=tmp;
		  if(max==0.0){
        fprintf(stderr, RCAT("Singular matrix A in ", LEVMAR_LUINVERSE) "()!\n");
        free(buf);

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

  /* The decomposition has now replaced a. Solve the m linear systems using
   * forward and back substitution
   */
  for(l=0; l<m; ++l){
    for(i=0; i<m; ++i) x[i]=0.0;
    x[l]=LM_CNST(1.0);

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

    for(i=0; i<m; ++i)
      B[i*m+l]=x[i];
  }

  free(buf);

  return 1;
}
#endif /* HAVE_LAPACK */

/*
 * This function computes in C the covariance matrix corresponding to a least
 * squares fit. JtJ is the approximate Hessian at the solution (i.e. J^T*J, where
 * J is the Jacobian at the solution), sumsq is the sum of squared residuals
 * (i.e. goodnes of fit) at the solution, m is the number of parameters (variables)
 * and n the number of observations. JtJ can coincide with C.
 * 
 * if JtJ is of full rank, C is computed as sumsq/(n-m)*(JtJ)^-1
 * otherwise and if LAPACK is available, C=sumsq/(n-r)*(JtJ)^+
 * where r is JtJ's rank and ^+ denotes the pseudoinverse
 * The diagonal of C is made up from the estimates of the variances
 * of the estimated regression coefficients.
 * See the documentation of routine E04YCF from the NAG fortran lib
 *
 * The function returns the rank of JtJ if successful, 0 on error
 *
 * A and C are mxm
 *
 */
int LEVMAR_COVAR(LM_REAL *JtJ, LM_REAL *C, LM_REAL sumsq, int m, int n)
{
register int i;
int rnk;
LM_REAL fact;

#ifdef HAVE_LAPACK
   rnk=LEVMAR_PSEUDOINVERSE(JtJ, C, m);
   if(!rnk) return 0;
#else
#ifdef _MSC_VER
#pragma message("LAPACK not available, LU will be used for matrix inversion when computing the covariance; this might be unstable at times")
#else
#warning LAPACK not available, LU will be used for matrix inversion when computing the covariance; this might be unstable at times
#endif // _MSC_VER

   rnk=LEVMAR_LUINVERSE(JtJ, C, m);
   if(!rnk) return 0;

   rnk=m; /* assume full rank */
#endif /* HAVE_LAPACK */

   fact=sumsq/(LM_REAL)(n-rnk);
   for(i=0; i<m*m; ++i)
     C[i]*=fact;

   return rnk;
}

/*  standard deviation of the best-fit parameter i.
 *  covar is the mxm covariance matrix of the best-fit parameters (see also LEVMAR_COVAR()).
 *
 *  The standard deviation is computed as \sigma_{i} = \sqrt{C_{ii}} 
 */
LM_REAL LEVMAR_STDDEV(LM_REAL *covar, int m, int i)
{
   return (LM_REAL)sqrt(covar[i*m+i]);
}

/* Pearson's correlation coefficient of the best-fit parameters i and j.
 * covar is the mxm covariance matrix of the best-fit parameters (see also LEVMAR_COVAR()).
 *
 * The coefficient is computed as \rho_{ij} = C_{ij} / sqrt(C_{ii} C_{jj})
 */
LM_REAL LEVMAR_CORCOEF(LM_REAL *covar, int m, int i, int j)
{
   return (LM_REAL)(covar[i*m+j]/sqrt(covar[i*m+i]*covar[j*m+j]));
}

/* check box constraints for consistency */
int LEVMAR_BOX_CHECK(LM_REAL *lb, LM_REAL *ub, int m)
{
register int i;

  if(!lb || !ub) return 1;

  for(i=0; i<m; ++i)
    if(lb[i]>ub[i]) return 0;

  return 1;
}

#ifdef HAVE_LAPACK

/* compute the Cholesky decompostion of C in W, s.t. C=W^t W and W is upper triangular */
int LEVMAR_CHOLESKY(LM_REAL *C, LM_REAL *W, int m)
{
register int i, j;
int info;

  /* copy weights array C to W (in column-major order!) so that LAPACK won't destroy it */
  for(i=0; i<m; i++)
    for(j=0; j<m; j++)
      W[i+j*m]=C[i*m+j];

  /* cholesky decomposition */
  POTF2("U", (int *)&m, W, (int *)&m, (int *)&info);
  /* error treatment */
  if(info!=0){
		if(info<0){
      fprintf(stderr, "LAPACK error: illegal value for argument %d of dpotf2 in %s\n", -info, LCAT(LEVMAR_DER, "()"));
		  exit(1);
		}
		else{
			fprintf(stderr, "LAPACK error: the leading minor of order %d is not positive definite,\n%s()\n", info,
						RCAT("and the cholesky factorization could not be completed in ", LEVMAR_CHOLESKY));
			return LM_ERROR;
		}
  }

  /* the decomposition is in the upper part of W (in column-major order!).
   * copying it to the lower part and zeroing the upper transposes
   * W in row-major order
   */
  for(i=0; i<m; i++)
    for(j=0; j<i; j++){
      W[i+j*m]=W[j+i*m];
      W[j+i*m]=0.0;
    }

  return 0;
}
#endif /* HAVE_LAPACK */


/* Compute e=x-y for two n-vectors x and y and return the squared L2 norm of e.
 * e can coincide with either x or y; x can be NULL, in which case it is assumed
 * to be equal to the zero vector.
 * Uses loop unrolling and blocking to reduce bookkeeping overhead & pipeline
 * stalls and increase instruction-level parallelism; see http://www.abarnett.demon.co.uk/tutorial.html
 */

LM_REAL LEVMAR_L2NRMXMY(LM_REAL *e, LM_REAL *x, LM_REAL *y, int n)
{
const int blocksize=8, bpwr=3; /* 8=2^3 */
register int i;
int j1, j2, j3, j4, j5, j6, j7;
int blockn;
register LM_REAL sum0=0.0, sum1=0.0, sum2=0.0, sum3=0.0;

  /* n may not be divisible by blocksize, 
   * go as near as we can first, then tidy up.
   */ 
  blockn = (n>>bpwr)<<bpwr; /* (n / blocksize) * blocksize; */

  if(x){
    /* unroll the loop in blocks of `blocksize' */
    for(i=0; i<blockn; i+=blocksize){
              e[i ]=x[i ]-y[i ]; sum0+=e[i ]*e[i ];
      j1=i+1; e[j1]=x[j1]-y[j1]; sum1+=e[j1]*e[j1];
      j2=i+2; e[j2]=x[j2]-y[j2]; sum2+=e[j2]*e[j2];
      j3=i+3; e[j3]=x[j3]-y[j3]; sum3+=e[j3]*e[j3];
      j4=i+4; e[j4]=x[j4]-y[j4]; sum0+=e[j4]*e[j4];
      j5=i+5; e[j5]=x[j5]-y[j5]; sum1+=e[j5]*e[j5];
      j6=i+6; e[j6]=x[j6]-y[j6]; sum2+=e[j6]*e[j6];
      j7=i+7; e[j7]=x[j7]-y[j7]; sum3+=e[j7]*e[j7];
    }

   /*
    * There may be some left to do.
    * This could be done as a simple for() loop, 
    * but a switch is faster (and more interesting) 
    */ 

    if(i<n){ 
      /* Jump into the case at the place that will allow
       * us to finish off the appropriate number of items. 
       */ 

      switch(n - i){ 
        case 7 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
        case 6 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
        case 5 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
        case 4 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
        case 3 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
        case 2 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
        case 1 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
      }
    }
  }
  else{ /* x==0 */
    /* unroll the loop in blocks of `blocksize' */
    for(i=0; i<blockn; i+=blocksize){
              e[i ]=-y[i ]; sum0+=e[i ]*e[i ];
      j1=i+1; e[j1]=-y[j1]; sum1+=e[j1]*e[j1];
      j2=i+2; e[j2]=-y[j2]; sum2+=e[j2]*e[j2];
      j3=i+3; e[j3]=-y[j3]; sum3+=e[j3]*e[j3];
      j4=i+4; e[j4]=-y[j4]; sum0+=e[j4]*e[j4];
      j5=i+5; e[j5]=-y[j5]; sum1+=e[j5]*e[j5];
      j6=i+6; e[j6]=-y[j6]; sum2+=e[j6]*e[j6];
      j7=i+7; e[j7]=-y[j7]; sum3+=e[j7]*e[j7];
    }

   /*
    * There may be some left to do.
    * This could be done as a simple for() loop, 
    * but a switch is faster (and more interesting) 
    */ 

    if(i<n){ 
      /* Jump into the case at the place that will allow
       * us to finish off the appropriate number of items. 
       */ 

      switch(n - i){ 
        case 7 : e[i]=-y[i]; sum0+=e[i]*e[i]; ++i;
        case 6 : e[i]=-y[i]; sum0+=e[i]*e[i]; ++i;
        case 5 : e[i]=-y[i]; sum0+=e[i]*e[i]; ++i;
        case 4 : e[i]=-y[i]; sum0+=e[i]*e[i]; ++i;
        case 3 : e[i]=-y[i]; sum0+=e[i]*e[i]; ++i;
        case 2 : e[i]=-y[i]; sum0+=e[i]*e[i]; ++i;
        case 1 : e[i]=-y[i]; sum0+=e[i]*e[i]; ++i;
      }
    }
  }

  return sum0+sum1+sum2+sum3;
}

/* undefine everything. THIS MUST REMAIN AT THE END OF THE FILE */
#undef LEVMAR_PSEUDOINVERSE
#undef LEVMAR_LUINVERSE
#undef LEVMAR_BOX_CHECK
#undef LEVMAR_CHOLESKY
#undef LEVMAR_COVAR
#undef LEVMAR_STDDEV
#undef LEVMAR_CORCOEF
#undef LEVMAR_CHKJAC
#undef LEVMAR_FDIF_FORW_JAC_APPROX
#undef LEVMAR_FDIF_CENT_JAC_APPROX
#undef LEVMAR_TRANS_MAT_MAT_MULT
#undef LEVMAR_L2NRMXMY
