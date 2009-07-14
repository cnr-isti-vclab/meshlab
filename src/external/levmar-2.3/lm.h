/* 
////////////////////////////////////////////////////////////////////////////////////
// 
//  Prototypes and definitions for the Levenberg - Marquardt minimization algorithm
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
////////////////////////////////////////////////////////////////////////////////////
*/

#ifndef _LM_H_
#define _LM_H_


/************************************* Start of configuration options *************************************/

/* specify whether to use LAPACK or not. The first option is strongly recommended */
 #define HAVE_LAPACK  /* use LAPACK */
 #undef HAVE_LAPACK   /* uncomment this to force not using LAPACK */

/* to avoid the overhead of repeated mallocs(), routines in Axb.c can be instructed to
 * retain working memory between calls. Such a choice, however, renders these routines
 * non-reentrant and is not safe in a shared memory multiprocessing environment.
 * Bellow, this option is turned on only when not compiling with OpenMP.
 */
#if !defined(_OPENMP) 
#define LINSOLVERS_RETAIN_MEMORY /* comment this if you don't want routines in Axb.c retain working memory between calls */
#endif

/* determine the precision variants to be build. Default settings build
 * both the single and double precision routines
 */
#define LM_DBL_PREC  /* comment this if you don't want the double precision routines to be compiled */
#define LM_SNGL_PREC /* comment this if you don't want the single precision routines to be compiled */

/****************** End of configuration options, no changes necessary beyond this point ******************/


#ifdef __cplusplus
extern "C" {
#endif


#define FABS(x) (((x)>=0.0)? (x) : -(x))

/* work arrays size for ?levmar_der and ?levmar_dif functions.
 * should be multiplied by sizeof(double) or sizeof(float) to be converted to bytes
 */
#define LM_DER_WORKSZ(npar, nmeas) (2*(nmeas) + 4*(npar) + (nmeas)*(npar) + (npar)*(npar))
#define LM_DIF_WORKSZ(npar, nmeas) (4*(nmeas) + 4*(npar) + (nmeas)*(npar) + (npar)*(npar))

/* work arrays size for ?levmar_bc_der and ?levmar_bc_dif functions.
 * should be multiplied by sizeof(double) or sizeof(float) to be converted to bytes
 */
#define LM_BC_DER_WORKSZ(npar, nmeas) (2*(nmeas) + 4*(npar) + (nmeas)*(npar) + (npar)*(npar))
#define LM_BC_DIF_WORKSZ(npar, nmeas) LM_BC_DER_WORKSZ((npar), (nmeas)) /* LEVMAR_BC_DIF currently implemented using LEVMAR_BC_DER()! */

/* work arrays size for ?levmar_lec_der and ?levmar_lec_dif functions.
 * should be multiplied by sizeof(double) or sizeof(float) to be converted to bytes
 */
#define LM_LEC_DER_WORKSZ(npar, nmeas, nconstr) LM_DER_WORKSZ((npar)-(nconstr), (nmeas))
#define LM_LEC_DIF_WORKSZ(npar, nmeas, nconstr) LM_DIF_WORKSZ((npar)-(nconstr), (nmeas))

/* work arrays size for ?levmar_blec_der and ?levmar_blec_dif functions.
 * should be multiplied by sizeof(double) or sizeof(float) to be converted to bytes
 */
#define LM_BLEC_DER_WORKSZ(npar, nmeas, nconstr) LM_LEC_DER_WORKSZ((npar), (nmeas)+(npar), (nconstr))
#define LM_BLEC_DIF_WORKSZ(npar, nmeas, nconstr) LM_LEC_DIF_WORKSZ((npar), (nmeas)+(npar), (nconstr))

#define LM_OPTS_SZ    	 5 /* max(4, 5) */
#define LM_INFO_SZ    	 9
#define LM_ERROR         -1
#define LM_INIT_MU    	 1E-03
#define LM_STOP_THRESH	 1E-17
#define LM_DIFF_DELTA    1E-06
#define LM_VERSION       "2.3 (May 2008)"

#ifdef LM_DBL_PREC
/* double precision LM, with & without Jacobian */
/* unconstrained minimization */
extern int dlevmar_der(
      void (*func)(double *p, double *hx, int m, int n, void *adata),
      void (*jacf)(double *p, double *j, int m, int n, void *adata),
      double *p, double *x, int m, int n, int itmax, double *opts,
      double *info, double *work, double *covar, void *adata);

extern int dlevmar_dif(
      void (*func)(double *p, double *hx, int m, int n, void *adata),
      double *p, double *x, int m, int n, int itmax, double *opts,
      double *info, double *work, double *covar, void *adata);

/* box-constrained minimization */
extern int dlevmar_bc_der(
       void (*func)(double *p, double *hx, int m, int n, void *adata),
       void (*jacf)(double *p, double *j, int m, int n, void *adata),  
       double *p, double *x, int m, int n, double *lb, double *ub,
       int itmax, double *opts, double *info, double *work, double *covar, void *adata);

extern int dlevmar_bc_dif(
       void (*func)(double *p, double *hx, int m, int n, void *adata),
       double *p, double *x, int m, int n, double *lb, double *ub,
       int itmax, double *opts, double *info, double *work, double *covar, void *adata);

#ifdef HAVE_LAPACK
/* linear equation constrained minimization */
extern int dlevmar_lec_der(
      void (*func)(double *p, double *hx, int m, int n, void *adata),
      void (*jacf)(double *p, double *j, int m, int n, void *adata),
      double *p, double *x, int m, int n, double *A, double *b, int k,
      int itmax, double *opts, double *info, double *work, double *covar, void *adata);

extern int dlevmar_lec_dif(
      void (*func)(double *p, double *hx, int m, int n, void *adata),
      double *p, double *x, int m, int n, double *A, double *b, int k,
      int itmax, double *opts, double *info, double *work, double *covar, void *adata);

/* box & linear equation constrained minimization */
extern int dlevmar_blec_der(
      void (*func)(double *p, double *hx, int m, int n, void *adata),
      void (*jacf)(double *p, double *j, int m, int n, void *adata),
      double *p, double *x, int m, int n, double *lb, double *ub, double *A, double *b, int k, double *wghts,
      int itmax, double *opts, double *info, double *work, double *covar, void *adata);

extern int dlevmar_blec_dif(
      void (*func)(double *p, double *hx, int m, int n, void *adata),
      double *p, double *x, int m, int n, double *lb, double *ub, double *A, double *b, int k, double *wghts,
      int itmax, double *opts, double *info, double *work, double *covar, void *adata);
#endif /* HAVE_LAPACK */

#endif /* LM_DBL_PREC */


#ifdef LM_SNGL_PREC
/* single precision LM, with & without Jacobian */
/* unconstrained minimization */
extern int slevmar_der(
      void (*func)(float *p, float *hx, int m, int n, void *adata),
      void (*jacf)(float *p, float *j, int m, int n, void *adata),
      float *p, float *x, int m, int n, int itmax, float *opts,
      float *info, float *work, float *covar, void *adata);

extern int slevmar_dif(
      void (*func)(float *p, float *hx, int m, int n, void *adata),
      float *p, float *x, int m, int n, int itmax, float *opts,
      float *info, float *work, float *covar, void *adata);

/* box-constrained minimization */
extern int slevmar_bc_der(
       void (*func)(float *p, float *hx, int m, int n, void *adata),
       void (*jacf)(float *p, float *j, int m, int n, void *adata),  
       float *p, float *x, int m, int n, float *lb, float *ub,
       int itmax, float *opts, float *info, float *work, float *covar, void *adata);

extern int slevmar_bc_dif(
       void (*func)(float *p, float *hx, int m, int n, void *adata),
       float *p, float *x, int m, int n, float *lb, float *ub,
       int itmax, float *opts, float *info, float *work, float *covar, void *adata);

#ifdef HAVE_LAPACK
/* linear equation constrained minimization */
extern int slevmar_lec_der(
      void (*func)(float *p, float *hx, int m, int n, void *adata),
      void (*jacf)(float *p, float *j, int m, int n, void *adata),
      float *p, float *x, int m, int n, float *A, float *b, int k,
      int itmax, float *opts, float *info, float *work, float *covar, void *adata);

extern int slevmar_lec_dif(
      void (*func)(float *p, float *hx, int m, int n, void *adata),
      float *p, float *x, int m, int n, float *A, float *b, int k,
      int itmax, float *opts, float *info, float *work, float *covar, void *adata);

/* box & linear equation constrained minimization */
extern int slevmar_blec_der(
      void (*func)(float *p, float *hx, int m, int n, void *adata),
      void (*jacf)(float *p, float *j, int m, int n, void *adata),
      float *p, float *x, int m, int n, float *lb, float *ub, float *A, float *b, int k, float *wghts,
      int itmax, float *opts, float *info, float *work, float *covar, void *adata);

extern int slevmar_blec_dif(
      void (*func)(float *p, float *hx, int m, int n, void *adata),
      float *p, float *x, int m, int n, float *lb, float *ub, float *A, float *b, int k, float *wghts,
      int itmax, float *opts, float *info, float *work, float *covar, void *adata);
#endif /* HAVE_LAPACK */

#endif /* LM_SNGL_PREC */

/* linear system solvers */
#ifdef HAVE_LAPACK

#ifdef LM_DBL_PREC
extern int dAx_eq_b_QR(double *A, double *B, double *x, int m);
extern int dAx_eq_b_QRLS(double *A, double *B, double *x, int m, int n);
extern int dAx_eq_b_Chol(double *A, double *B, double *x, int m);
extern int dAx_eq_b_LU(double *A, double *B, double *x, int m);
extern int dAx_eq_b_SVD(double *A, double *B, double *x, int m);
#endif /* LM_DBL_PREC */

#ifdef LM_SNGL_PREC
extern int sAx_eq_b_QR(float *A, float *B, float *x, int m);
extern int sAx_eq_b_QRLS(float *A, float *B, float *x, int m, int n);
extern int sAx_eq_b_Chol(float *A, float *B, float *x, int m);
extern int sAx_eq_b_LU(float *A, float *B, float *x, int m);
extern int sAx_eq_b_SVD(float *A, float *B, float *x, int m);
#endif /* LM_SNGL_PREC */

#else /* no LAPACK */

#ifdef LM_DBL_PREC
extern int dAx_eq_b_LU_noLapack(double *A, double *B, double *x, int n);
#endif /* LM_DBL_PREC */

#ifdef LM_SNGL_PREC
extern int sAx_eq_b_LU_noLapack(float *A, float *B, float *x, int n);
#endif /* LM_SNGL_PREC */

#endif /* HAVE_LAPACK */

/* Jacobian verification, double & single precision */
#ifdef LM_DBL_PREC
extern void dlevmar_chkjac(
    void (*func)(double *p, double *hx, int m, int n, void *adata),
    void (*jacf)(double *p, double *j, int m, int n, void *adata),
    double *p, int m, int n, void *adata, double *err);
#endif /* LM_DBL_PREC */

#ifdef LM_SNGL_PREC
extern void slevmar_chkjac(
    void (*func)(float *p, float *hx, int m, int n, void *adata),
    void (*jacf)(float *p, float *j, int m, int n, void *adata),
    float *p, int m, int n, void *adata, float *err);
#endif /* LM_SNGL_PREC */

/* standard deviation & Pearson's correlation coefficient for best-fit parameters */
#ifdef LM_DBL_PREC
extern double dlevmar_stddev( double *covar, int m, int i);
extern double dlevmar_corcoef(double *covar, int m, int i, int j);
#endif /* LM_DBL_PREC */

#ifdef LM_SNGL_PREC
extern float slevmar_stddev( float *covar, int m, int i);
extern float slevmar_corcoef(float *covar, int m, int i, int j);
#endif /* LM_SNGL_PREC */

#ifdef __cplusplus
}
#endif

#endif /* _LM_H_ */
