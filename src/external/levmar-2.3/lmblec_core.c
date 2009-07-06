/////////////////////////////////////////////////////////////////////////////////
// 
//  Levenberg - Marquardt non-linear minimization algorithm
//  Copyright (C) 2004-06  Manolis Lourakis (lourakis at ics forth gr)
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
 * This file implements combined box and linear equation constraints.
 *
 * Note that the algorithm implementing linearly constrained minimization does
 * so by a change in parameters that transforms the original program into an
 * unconstrained one. To employ the same idea for implementing box & linear
 * constraints would require the transformation of box constraints on the
 * original parameters to box constraints for the new parameter set. This
 * being impossible, a different approach is used here for finding the minimum.
 * The trick is to remove the box constraints by augmenting the function to
 * be fitted with penalty terms and then solve the resulting problem (which
 * involves linear constrains only) with the functions in lmlec.c
 *
 * More specifically, for the constraint a<=x[i]<=b to hold, the term C[i]=
 * (2*x[i]-(a+b))/(b-a) should be within [-1, 1]. This is enforced by adding
 * the penalty term w[i]*max((C[i])^2-1, 0) to the objective function, where
 * w[i] is a large weight. In the case of constraints of the form a<=x[i],
 * the term C[i]=a-x[i] has to be non positive, thus the penalty term is
 * w[i]*max(C[i], 0). If x[i]<=b, C[i]=x[i]-b has to be non negative and
 * the penalty is w[i]*max(C[i], 0). The derivatives needed for the Jacobian
 * are as follows:
 * For the constraint a<=x[i]<=b: 4*(2*x[i]-(a+b))/(b-a)^2 if x[i] not in [a, b],
 *                                0 otherwise
 * For the constraint a<=x[i]: -1 if x[i]<=a, 0 otherwise
 * For the constraint x[i]<=b: 1 if b<=x[i], 0 otherwise
 *
 * Note that for the above to work, the weights w[i] should be large enough;
 * depending on your minimization problem, the default values might need some
 * tweaking (see arg "wghts" below).
 *******************************************************************************/

#ifndef LM_REAL // not included by lmblec.c
#error This file should not be compiled directly!
#endif


#define __MAX__(x, y)   (((x)>=(y))? (x) : (y))
#define __BC_WEIGHT__   LM_CNST(1E+04)

#define __BC_INTERVAL__ 0
#define __BC_LOW__      1
#define __BC_HIGH__     2

/* precision-specific definitions */
#define LEVMAR_BOX_CHECK LM_ADD_PREFIX(levmar_box_check)
#define LMBLEC_DATA LM_ADD_PREFIX(lmblec_data)
#define LMBLEC_FUNC LM_ADD_PREFIX(lmblec_func)
#define LMBLEC_JACF LM_ADD_PREFIX(lmblec_jacf)
#define LEVMAR_LEC_DER LM_ADD_PREFIX(levmar_lec_der)
#define LEVMAR_LEC_DIF LM_ADD_PREFIX(levmar_lec_dif)
#define LEVMAR_BLEC_DER LM_ADD_PREFIX(levmar_blec_der)
#define LEVMAR_BLEC_DIF LM_ADD_PREFIX(levmar_blec_dif)
#define LEVMAR_COVAR LM_ADD_PREFIX(levmar_covar)
#define LEVMAR_FDIF_FORW_JAC_APPROX LM_ADD_PREFIX(levmar_fdif_forw_jac_approx)

struct LMBLEC_DATA{
  LM_REAL *x, *lb, *ub, *w;
  int *bctype;
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata);
  void (*jacf)(LM_REAL *p, LM_REAL *jac, int m, int n, void *adata);
  void *adata;
};

/* augmented measurements */
static void LMBLEC_FUNC(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata)
{
struct LMBLEC_DATA *data=(struct LMBLEC_DATA *)adata;
int nn;
register int i, j, *typ;
register LM_REAL *lb, *ub, *w, tmp;

  nn=n-m;
  lb=data->lb;
  ub=data->ub;
  w=data->w;
  typ=data->bctype;
  (*(data->func))(p, hx, m, nn, data->adata);

  for(i=nn, j=0; i<n; ++i, ++j){
    switch(typ[j]){
      case __BC_INTERVAL__:
        tmp=(LM_CNST(2.0)*p[j]-(lb[j]+ub[j]))/(ub[j]-lb[j]);
        hx[i]=w[j]*__MAX__(tmp*tmp-LM_CNST(1.0), LM_CNST(0.0));
      break;

      case __BC_LOW__:
        hx[i]=w[j]*__MAX__(lb[j]-p[j], LM_CNST(0.0));
      break;

      case __BC_HIGH__:
        hx[i]=w[j]*__MAX__(p[j]-ub[j], LM_CNST(0.0));
      break;
    }
  }
}

/* augmented Jacobian */
static void LMBLEC_JACF(LM_REAL *p, LM_REAL *jac, int m, int n, void *adata)
{
struct LMBLEC_DATA *data=(struct LMBLEC_DATA *)adata;
int nn, *typ;
register int i, j;
register LM_REAL *lb, *ub, *w, tmp;

  nn=n-m;
  lb=data->lb;
  ub=data->ub;
  w=data->w;
  typ=data->bctype;
  (*(data->jacf))(p, jac, m, nn, data->adata);

  /* clear all extra rows */
  for(i=nn*m; i<n*m; ++i)
    jac[i]=0.0;

  for(i=nn, j=0; i<n; ++i, ++j){
    switch(typ[j]){
      case __BC_INTERVAL__:
        if(lb[j]<=p[j] && p[j]<=ub[j])
          continue; // corresp. jac element already 0

        /* out of interval */
        tmp=ub[j]-lb[j];
        tmp=LM_CNST(4.0)*(LM_CNST(2.0)*p[j]-(lb[j]+ub[j]))/(tmp*tmp);
        jac[i*m+j]=w[j]*tmp;
      break;

      case __BC_LOW__: // (lb[j]<=p[j])? 0.0 : -1.0;
        if(lb[j]<=p[j])
          continue; // corresp. jac element already 0

        /* smaller than lower bound */
        jac[i*m+j]=-w[j];
      break;

      case __BC_HIGH__: // (p[j]<=ub[j])? 0.0 : 1.0;
        if(p[j]<=ub[j])
          continue; // corresp. jac element already 0

        /* greater than upper bound */
        jac[i*m+j]=w[j];
      break;
    }
  }
}

/* 
 * This function seeks the parameter vector p that best describes the measurements
 * vector x under box & linear constraints.
 * More precisely, given a vector function  func : R^m --> R^n with n>=m,
 * it finds p s.t. func(p) ~= x, i.e. the squared second order (i.e. L2) norm of
 * e=x-func(p) is minimized under the constraints lb[i]<=p[i]<=ub[i] and A p=b;
 * A is kxm, b kx1. Note that this function DOES NOT check the satisfiability of
 * the specified box and linear equation constraints.
 * If no lower bound constraint applies for p[i], use -DBL_MAX/-FLT_MAX for lb[i];
 * If no upper bound constraint applies for p[i], use DBL_MAX/FLT_MAX for ub[i].
 *
 * This function requires an analytic Jacobian. In case the latter is unavailable,
 * use LEVMAR_BLEC_DIF() bellow
 *
 * Returns the number of iterations (>=0) if successfull, LM_ERROR if failed
 *
 * For more details on the algorithm implemented by this function, please refer to
 * the comments in the top of this file.
 *
 */
int LEVMAR_BLEC_DER(
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata), /* functional relation describing measurements. A p \in R^m yields a \hat{x} \in  R^n */
  void (*jacf)(LM_REAL *p, LM_REAL *j, int m, int n, void *adata),  /* function to evaluate the Jacobian \part x / \part p */ 
  LM_REAL *p,         /* I/O: initial parameter estimates. On output has the estimated solution */
  LM_REAL *x,         /* I: measurement vector. NULL implies a zero vector */
  int m,              /* I: parameter vector dimension (i.e. #unknowns) */
  int n,              /* I: measurement vector dimension */
  LM_REAL *lb,        /* I: vector of lower bounds. If NULL, no lower bounds apply */
  LM_REAL *ub,        /* I: vector of upper bounds. If NULL, no upper bounds apply */
  LM_REAL *A,         /* I: constraints matrix, kxm */
  LM_REAL *b,         /* I: right hand constraints vector, kx1 */
  int k,              /* I: number of constraints (i.e. A's #rows) */
  LM_REAL *wghts,     /* mx1 weights for penalty terms, defaults used if NULL */
  int itmax,          /* I: maximum number of iterations */
  LM_REAL opts[4],    /* I: minim. options [\mu, \epsilon1, \epsilon2, \epsilon3]. Respectively the scale factor for initial \mu,
                       * stopping thresholds for ||J^T e||_inf, ||Dp||_2 and ||e||_2. Set to NULL for defaults to be used
                       */
  LM_REAL info[LM_INFO_SZ],
					           /* O: information regarding the minimization. Set to NULL if don't care
                      * info[0]= ||e||_2 at initial p.
                      * info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||Dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
                      * info[5]= # iterations,
                      * info[6]=reason for terminating: 1 - stopped by small gradient J^T e
                      *                                 2 - stopped by small Dp
                      *                                 3 - stopped by itmax
                      *                                 4 - singular matrix. Restart from current p with increased mu 
                      *                                 5 - no further error reduction is possible. Restart with increased mu
                      *                                 6 - stopped by small ||e||_2
                      *                                 7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
                      * info[7]= # function evaluations
                      * info[8]= # Jacobian evaluations
                      */
  LM_REAL *work,     /* working memory at least LM_BLEC_DER_WORKSZ() reals large, allocated if NULL */
  LM_REAL *covar,    /* O: Covariance matrix corresponding to LS solution; mxm. Set to NULL if not needed. */
  void *adata)       /* pointer to possibly additional data, passed uninterpreted to func & jacf.
                      * Set to NULL if not needed
                      */
{
  struct LMBLEC_DATA data;
  int ret;
  LM_REAL locinfo[LM_INFO_SZ];
  register int i;

  if(!jacf){
    fprintf(stderr, RCAT("No function specified for computing the Jacobian in ", LEVMAR_BLEC_DER)
      RCAT("().\nIf no such function is available, use ", LEVMAR_BLEC_DIF) RCAT("() rather than ", LEVMAR_BLEC_DER) "()\n");
    return LM_ERROR;
  }

  if(!LEVMAR_BOX_CHECK(lb, ub, m)){
    fprintf(stderr, LCAT(LEVMAR_BLEC_DER, "(): at least one lower bound exceeds the upper one\n"));
    return LM_ERROR;
  }

  /* measurement vector needs to be extended by m */
  if(x){ /* nonzero x */
    data.x=(LM_REAL *)malloc((n+m)*sizeof(LM_REAL));
    if(!data.x){
      fprintf(stderr, LCAT(LEVMAR_BLEC_DER, "(): memory allocation request #1 failed\n"));
      exit(1);
    }

    for(i=0; i<n; ++i)
      data.x[i]=x[i];
    for(i=n; i<n+m; ++i)
      data.x[i]=0.0;
  }
  else
    data.x=NULL;

  data.w=(LM_REAL *)malloc(m*sizeof(LM_REAL) + m*sizeof(int));
  if(!data.w){
    fprintf(stderr, LCAT(LEVMAR_BLEC_DER, "(): memory allocation request #2 failed\n"));
    exit(1);
  }
  data.bctype=(int *)(data.w+m);

  for(i=0; i<m; ++i){
    data.w[i]=(!wghts)? __BC_WEIGHT__ : wghts[i];
    if(ub[i]!=LM_REAL_MAX && lb[i]!=LM_REAL_MIN) data.bctype[i]=__BC_INTERVAL__;
    else if(lb[i]!=LM_REAL_MIN) data.bctype[i]=__BC_LOW__;
    else data.bctype[i]=__BC_HIGH__;
  }

  data.lb=lb;
  data.ub=ub;
  data.func=func;
  data.jacf=jacf;
  data.adata=adata;

  if(!info) info=locinfo; /* make sure that LEVMAR_LEC_DER() is called with non-null info */
  ret=LEVMAR_LEC_DER(LMBLEC_FUNC, LMBLEC_JACF, p, data.x, m, n+m, A, b, k, itmax, opts, info, work, covar, (void *)&data);

  if(data.x) free(data.x);
  free(data.w);

  return ret;
}

/* Similar to the LEVMAR_BLEC_DER() function above, except that the Jacobian is approximated
 * with the aid of finite differences (forward or central, see the comment for the opts argument)
 */
int LEVMAR_BLEC_DIF(
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata), /* functional relation describing measurements. A p \in R^m yields a \hat{x} \in  R^n */
  LM_REAL *p,         /* I/O: initial parameter estimates. On output has the estimated solution */
  LM_REAL *x,         /* I: measurement vector. NULL implies a zero vector */
  int m,              /* I: parameter vector dimension (i.e. #unknowns) */
  int n,              /* I: measurement vector dimension */
  LM_REAL *lb,        /* I: vector of lower bounds. If NULL, no lower bounds apply */
  LM_REAL *ub,        /* I: vector of upper bounds. If NULL, no upper bounds apply */
  LM_REAL *A,         /* I: constraints matrix, kxm */
  LM_REAL *b,         /* I: right hand constraints vector, kx1 */
  int k,              /* I: number of constraints (i.e. A's #rows) */
  LM_REAL *wghts,     /* mx1 weights for penalty terms, defaults used if NULL */
  int itmax,          /* I: maximum number of iterations */
  LM_REAL opts[5],    /* I: opts[0-3] = minim. options [\mu, \epsilon1, \epsilon2, \epsilon3, \delta]. Respectively the
                       * scale factor for initial \mu, stopping thresholds for ||J^T e||_inf, ||Dp||_2 and ||e||_2 and
                       * the step used in difference approximation to the Jacobian. Set to NULL for defaults to be used.
                       * If \delta<0, the Jacobian is approximated with central differences which are more accurate
                       * (but slower!) compared to the forward differences employed by default. 
                       */
  LM_REAL info[LM_INFO_SZ],
					           /* O: information regarding the minimization. Set to NULL if don't care
                      * info[0]= ||e||_2 at initial p.
                      * info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||Dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
                      * info[5]= # iterations,
                      * info[6]=reason for terminating: 1 - stopped by small gradient J^T e
                      *                                 2 - stopped by small Dp
                      *                                 3 - stopped by itmax
                      *                                 4 - singular matrix. Restart from current p with increased mu 
                      *                                 5 - no further error reduction is possible. Restart with increased mu
                      *                                 6 - stopped by small ||e||_2
                      *                                 7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
                      * info[7]= # function evaluations
                      * info[8]= # Jacobian evaluations
                      */
  LM_REAL *work,     /* working memory at least LM_BLEC_DIF_WORKSZ() reals large, allocated if NULL */
  LM_REAL *covar,    /* O: Covariance matrix corresponding to LS solution; mxm. Set to NULL if not needed. */
  void *adata)       /* pointer to possibly additional data, passed uninterpreted to func.
                      * Set to NULL if not needed
                      */
{
  struct LMBLEC_DATA data;
  int ret;
  register int i;
  LM_REAL locinfo[LM_INFO_SZ];

  if(!LEVMAR_BOX_CHECK(lb, ub, m)){
    fprintf(stderr, LCAT(LEVMAR_BLEC_DER, "(): at least one lower bound exceeds the upper one\n"));
    return LM_ERROR;
  }

  /* measurement vector needs to be extended by m */
  if(x){ /* nonzero x */
    data.x=(LM_REAL *)malloc((n+m)*sizeof(LM_REAL));
    if(!data.x){
      fprintf(stderr, LCAT(LEVMAR_BLEC_DER, "(): memory allocation request #1 failed\n"));
      exit(1);
    }

    for(i=0; i<n; ++i)
      data.x[i]=x[i];
    for(i=n; i<n+m; ++i)
      data.x[i]=0.0;
  }
  else
    data.x=NULL;

  data.w=(LM_REAL *)malloc(m*sizeof(LM_REAL) + m*sizeof(int));
  if(!data.w){
    fprintf(stderr, LCAT(LEVMAR_BLEC_DER, "(): memory allocation request #2 failed\n"));
    exit(1);
  }
  data.bctype=(int *)(data.w+m);

  for(i=0; i<m; ++i){
    data.w[i]=(!wghts)? __BC_WEIGHT__ : wghts[i];
    if(ub[i]!=LM_REAL_MAX && lb[i]!=LM_REAL_MIN) data.bctype[i]=__BC_INTERVAL__;
    else if(lb[i]!=LM_REAL_MIN) data.bctype[i]=__BC_LOW__;
    else data.bctype[i]=__BC_HIGH__;
  }

  data.lb=lb;
  data.ub=ub;
  data.func=func;
  data.jacf=NULL;
  data.adata=adata;

  if(!info) info=locinfo; /* make sure that LEVMAR_LEC_DIF() is called with non-null info */
  ret=LEVMAR_LEC_DIF(LMBLEC_FUNC, p, data.x, m, n+m, A, b, k, itmax, opts, info, work, covar, (void *)&data);

  if(data.x) free(data.x);
  free(data.w);

  return ret;
}

/* undefine all. THIS MUST REMAIN AT THE END OF THE FILE */
#undef LEVMAR_BOX_CHECK
#undef LMBLEC_DATA
#undef LMBLEC_FUNC
#undef LMBLEC_JACF
#undef LEVMAR_FDIF_FORW_JAC_APPROX
#undef LEVMAR_COVAR
#undef LEVMAR_LEC_DER
#undef LEVMAR_LEC_DIF
#undef LEVMAR_BLEC_DER
#undef LEVMAR_BLEC_DIF
