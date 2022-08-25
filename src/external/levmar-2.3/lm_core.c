/////////////////////////////////////////////////////////////////////////////////
// 
//  Levenberg - Marquardt non-linear minimization algorithm
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

#ifndef LM_REAL // not included by lm.c
#error This file should not be compiled directly!
#endif


/* precision-specific definitions */
#define LEVMAR_DER LM_ADD_PREFIX(levmar_der)
#define LEVMAR_DIF LM_ADD_PREFIX(levmar_dif)
#define LEVMAR_FDIF_FORW_JAC_APPROX LM_ADD_PREFIX(levmar_fdif_forw_jac_approx)
#define LEVMAR_FDIF_CENT_JAC_APPROX LM_ADD_PREFIX(levmar_fdif_cent_jac_approx)
#define LEVMAR_TRANS_MAT_MAT_MULT LM_ADD_PREFIX(levmar_trans_mat_mat_mult)
#define LEVMAR_L2NRMXMY LM_ADD_PREFIX(levmar_L2nrmxmy)
#define LEVMAR_COVAR LM_ADD_PREFIX(levmar_covar)

#ifdef HAVE_LAPACK
#define AX_EQ_B_LU LM_ADD_PREFIX(Ax_eq_b_LU)
#define AX_EQ_B_CHOL LM_ADD_PREFIX(Ax_eq_b_Chol)
#define AX_EQ_B_QR LM_ADD_PREFIX(Ax_eq_b_QR)
#define AX_EQ_B_QRLS LM_ADD_PREFIX(Ax_eq_b_QRLS)
#define AX_EQ_B_SVD LM_ADD_PREFIX(Ax_eq_b_SVD)
#else
#define AX_EQ_B_LU LM_ADD_PREFIX(Ax_eq_b_LU_noLapack)
#endif /* HAVE_LAPACK */

/* 
 * This function seeks the parameter vector p that best describes the measurements vector x.
 * More precisely, given a vector function  func : R^m --> R^n with n>=m,
 * it finds p s.t. func(p) ~= x, i.e. the squared second order (i.e. L2) norm of
 * e=x-func(p) is minimized.
 *
 * This function requires an analytic Jacobian. In case the latter is unavailable,
 * use LEVMAR_DIF() bellow
 *
 * Returns the number of iterations (>=0) if successfull, LM_ERROR if failed
 *
 * For more details, see K. Madsen, H.B. Nielsen and O. Tingleff's lecture notes on 
 * non-linear least squares at http://www.imm.dtu.dk/pubdb/views/edoc_download.php/3215/pdf/imm3215.pdf
 */

int LEVMAR_DER(
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata), /* functional relation describing measurements. A p \in R^m yields a \hat{x} \in  R^n */
  void (*jacf)(LM_REAL *p, LM_REAL *j, int m, int n, void *adata),  /* function to evaluate the Jacobian \part x / \part p */ 
  LM_REAL *p,         /* I/O: initial parameter estimates. On output has the estimated solution */
  LM_REAL *x,         /* I: measurement vector. NULL implies a zero vector */
  int m,              /* I: parameter vector dimension (i.e. #unknowns) */
  int n,              /* I: measurement vector dimension */
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
  LM_REAL *work,     /* working memory at least LM_DER_WORKSZ() reals large, allocated if NULL */
  LM_REAL *covar,    /* O: Covariance matrix corresponding to LS solution; mxm. Set to NULL if not needed. */
  void *adata)       /* pointer to possibly additional data, passed uninterpreted to func & jacf.
                      * Set to NULL if not needed
                      */
{
register int i, j, k, l;
int worksz, freework=0, issolved;
/* temp work arrays */
LM_REAL *e,          /* nx1 */
       *hx,         /* \hat{x}_i, nx1 */
       *jacTe,      /* J^T e_i mx1 */
       *jac,        /* nxm */
       *jacTjac,    /* mxm */
       *Dp,         /* mx1 */
   *diag_jacTjac,   /* diagonal of J^T J, mx1 */
       *pDp;        /* p + Dp, mx1 */

register LM_REAL mu,  /* damping constant */
                tmp; /* mainly used in matrix & vector multiplications */
LM_REAL p_eL2, jacTe_inf, pDp_eL2; /* ||e(p)||_2, ||J^T e||_inf, ||e(p+Dp)||_2 */
LM_REAL p_L2, Dp_L2=LM_REAL_MAX, dF, dL;
LM_REAL tau, eps1, eps2, eps2_sq, eps3;
LM_REAL init_p_eL2;
int nu=2, nu2, stop=0, nfev, njev=0;
const int nm=n*m;
int (*linsolver)(LM_REAL *A, LM_REAL *B, LM_REAL *x, int m)=NULL;

  mu=jacTe_inf=0.0; /* -Wall */

  if(n<m){
    fprintf(stderr, LCAT(LEVMAR_DER, "(): cannot solve a problem with fewer measurements [%d] than unknowns [%d]\n"), n, m);
    return LM_ERROR;
  }

  if(!jacf){
    fprintf(stderr, RCAT("No function specified for computing the Jacobian in ", LEVMAR_DER)
        RCAT("().\nIf no such function is available, use ", LEVMAR_DIF) RCAT("() rather than ", LEVMAR_DER) "()\n");
    return LM_ERROR;
  }

  if(opts){
	  tau=opts[0];
	  eps1=opts[1];
	  eps2=opts[2];
	  eps2_sq=opts[2]*opts[2];
    eps3=opts[3];
  }
  else{ // use default values
	  tau=LM_CNST(LM_INIT_MU);
	  eps1=LM_CNST(LM_STOP_THRESH);
	  eps2=LM_CNST(LM_STOP_THRESH);
	  eps2_sq=LM_CNST(LM_STOP_THRESH)*LM_CNST(LM_STOP_THRESH);
    eps3=LM_CNST(LM_STOP_THRESH);
  }

  if(!work){
    worksz=LM_DER_WORKSZ(m, n); //2*n+4*m + n*m + m*m;
    work=(LM_REAL *)malloc(worksz*sizeof(LM_REAL)); /* allocate a big chunk in one step */
    if(!work){
      fprintf(stderr, LCAT(LEVMAR_DER, "(): memory allocation request failed\n"));
      exit(1);
    }
    freework=1;
  }

  /* set up work arrays */
  e=work;
  hx=e + n;
  jacTe=hx + n;
  jac=jacTe + m;
  jacTjac=jac + nm;
  Dp=jacTjac + m*m;
  diag_jacTjac=Dp + m;
  pDp=diag_jacTjac + m;

  /* compute e=x - f(p) and its L2 norm */
  (*func)(p, hx, m, n, adata); nfev=1;
  /* ### e=x-hx, p_eL2=||e|| */
#if 1
  p_eL2=LEVMAR_L2NRMXMY(e, x, hx, n);  
#else
  for(i=0, p_eL2=0.0; i<n; ++i){
    e[i]=tmp=x[i]-hx[i];
    p_eL2+=tmp*tmp;
  }
#endif
  init_p_eL2=p_eL2;
  if(!LM_FINITE(p_eL2)) stop=7;

  for(k=0; k<itmax && !stop; ++k){
    /* Note that p and e have been updated at a previous iteration */

    if(p_eL2<=eps3){ /* error is small */
      stop=6;
      break;
    }

    /* Compute the Jacobian J at p,  J^T J,  J^T e,  ||J^T e||_inf and ||p||^2.
     * Since J^T J is symmetric, its computation can be speeded up by computing
     * only its upper triangular part and copying it to the lower part
     */

    (*jacf)(p, jac, m, n, adata); ++njev;

    /* J^T J, J^T e */
    if(nm<__BLOCKSZ__SQ){ // this is a small problem
      /* This is the straightforward way to compute J^T J, J^T e. However, due to
       * its noncontinuous memory access pattern, it incures many cache misses when
       * applied to large minimization problems (i.e. problems involving a large
       * number of free variables and measurements), in which J is too large to
       * fit in the L1 cache. For such problems, a cache-efficient blocking scheme
       * is preferable.
       *
       * Thanks to John Nitao of Lawrence Livermore Lab for pointing out this
       * performance problem.
       *
       * On the other hand, the straightforward algorithm is faster on small
       * problems since in this case it avoids the overheads of blocking. 
       */

      for(i=0; i<m; ++i){
        for(j=i; j<m; ++j){
          int lm;

          for(l=0, tmp=0.0; l<n; ++l){
            lm=l*m;
            tmp+=jac[lm+i]*jac[lm+j];
          }

		      /* store tmp in the corresponding upper and lower part elements */
          jacTjac[i*m+j]=jacTjac[j*m+i]=tmp;
        }

        /* J^T e */
        for(l=0, tmp=0.0; l<n; ++l)
          tmp+=jac[l*m+i]*e[l];
        jacTe[i]=tmp;
      }
    }
    else{ // this is a large problem
      /* Cache efficient computation of J^T J based on blocking
       */
      LEVMAR_TRANS_MAT_MAT_MULT(jac, jacTjac, n, m);

      /* cache efficient computation of J^T e */
      for(i=0; i<m; ++i)
        jacTe[i]=0.0;

      for(i=0; i<n; ++i){
        register LM_REAL *jacrow;

        for(l=0, jacrow=jac+i*m, tmp=e[i]; l<m; ++l)
          jacTe[l]+=jacrow[l]*tmp;
      }
    }

	  /* Compute ||J^T e||_inf and ||p||^2 */
    for(i=0, p_L2=jacTe_inf=0.0; i<m; ++i){
      if(jacTe_inf < (tmp=FABS(jacTe[i]))) jacTe_inf=tmp;

      diag_jacTjac[i]=jacTjac[i*m+i]; /* save diagonal entries so that augmentation can be later canceled */
      p_L2+=p[i]*p[i];
    }
    //p_L2=sqrt(p_L2);

#if 0
if(!(k%100)){
  printf("Current estimate: ");
  for(i=0; i<m; ++i)
    printf("%.9g ", p[i]);
  printf("-- errors %.9g %0.9g\n", jacTe_inf, p_eL2);
}
#endif

    /* check for convergence */
    if((jacTe_inf <= eps1)){
      Dp_L2=0.0; /* no increment for p in this case */
      stop=1;
      break;
    }

   /* compute initial damping factor */
    if(k==0){
      for(i=0, tmp=LM_REAL_MIN; i<m; ++i)
        if(diag_jacTjac[i]>tmp) tmp=diag_jacTjac[i]; /* find max diagonal element */
      mu=tau*tmp;
    }

    /* determine increment using adaptive damping */
    while(1){
      /* augment normal equations */
      for(i=0; i<m; ++i)
        jacTjac[i*m+i]+=mu;

      /* solve augmented equations */
#ifdef HAVE_LAPACK
      /* 5 alternatives are available: LU, Cholesky, 2 variants of QR decomposition and SVD.
       * Cholesky is the fastest but might be inaccurate; QR is slower but more accurate;
       * SVD is the slowest but most accurate; LU offers a tradeoff between accuracy and speed
       */

      issolved=AX_EQ_B_LU(jacTjac, jacTe, Dp, m); linsolver=AX_EQ_B_LU;
      //issolved=AX_EQ_B_CHOL(jacTjac, jacTe, Dp, m); linsolver=AX_EQ_B_CHOL;
      //issolved=AX_EQ_B_QR(jacTjac, jacTe, Dp, m); linsolver=AX_EQ_B_QR;
      //issolved=AX_EQ_B_QRLS(jacTjac, jacTe, Dp, m, m); linsolver=AX_EQ_B_QRLS;
      //issolved=AX_EQ_B_SVD(jacTjac, jacTe, Dp, m); linsolver=AX_EQ_B_SVD;

#else
      /* use the LU included with levmar */
      issolved=AX_EQ_B_LU(jacTjac, jacTe, Dp, m); linsolver=AX_EQ_B_LU;
#endif /* HAVE_LAPACK */

      if(issolved){
        /* compute p's new estimate and ||Dp||^2 */
        for(i=0, Dp_L2=0.0; i<m; ++i){
          pDp[i]=p[i] + (tmp=Dp[i]);
          Dp_L2+=tmp*tmp;
        }
        //Dp_L2=sqrt(Dp_L2);

        if(Dp_L2<=eps2_sq*p_L2){ /* relative change in p is small, stop */
        //if(Dp_L2<=eps2*(p_L2 + eps2)){ /* relative change in p is small, stop */
          stop=2;
          break;
        }

       if(Dp_L2>=(p_L2+eps2)/(LM_CNST(EPSILON)*LM_CNST(EPSILON))){ /* almost singular */
       //if(Dp_L2>=(p_L2+eps2)/LM_CNST(EPSILON)){ /* almost singular */
         stop=4;
         break;
       }

        (*func)(pDp, hx, m, n, adata); ++nfev; /* evaluate function at p + Dp */
        /* compute ||e(pDp)||_2 */
        /* ### hx=x-hx, pDp_eL2=||hx|| */
#if 1
        pDp_eL2=LEVMAR_L2NRMXMY(hx, x, hx, n);
#else
        for(i=0, pDp_eL2=0.0; i<n; ++i){
          hx[i]=tmp=x[i]-hx[i];
          pDp_eL2+=tmp*tmp;
        }
#endif
        if(!LM_FINITE(pDp_eL2)){ /* sum of squares is not finite, most probably due to a user error.
                                  * This check makes sure that the inner loop does not run indefinitely.
                                  * Thanks to Steve Danauskas for reporting such cases
                                  */
          stop=7;
          break;
        }

        for(i=0, dL=0.0; i<m; ++i)
          dL+=Dp[i]*(mu*Dp[i]+jacTe[i]);

        dF=p_eL2-pDp_eL2;

        if(dL>0.0 && dF>0.0){ /* reduction in error, increment is accepted */
          tmp=(LM_CNST(2.0)*dF/dL-LM_CNST(1.0));
          tmp=LM_CNST(1.0)-tmp*tmp*tmp;
          mu=mu*( (tmp>=LM_CNST(ONE_THIRD))? tmp : LM_CNST(ONE_THIRD) );
          nu=2;

          for(i=0 ; i<m; ++i) /* update p's estimate */
            p[i]=pDp[i];

          for(i=0; i<n; ++i) /* update e and ||e||_2 */
            e[i]=hx[i];
          p_eL2=pDp_eL2;
          break;
        }
      }

      /* if this point is reached, either the linear system could not be solved or
       * the error did not reduce; in any case, the increment must be rejected
       */

      mu*=nu;
      nu2=nu<<1; // 2*nu;
      if(nu2<=nu){ /* nu has wrapped around (overflown). Thanks to Frank Jordan for spotting this case */
        stop=5;
        break;
      }
      nu=nu2;

      for(i=0; i<m; ++i) /* restore diagonal J^T J entries */
        jacTjac[i*m+i]=diag_jacTjac[i];
    } /* inner loop */
  }

  if(k>=itmax) stop=3;

  for(i=0; i<m; ++i) /* restore diagonal J^T J entries */
    jacTjac[i*m+i]=diag_jacTjac[i];

  if(info){
    info[0]=init_p_eL2;
    info[1]=p_eL2;
    info[2]=jacTe_inf;
    info[3]=Dp_L2;
    for(i=0, tmp=LM_REAL_MIN; i<m; ++i)
      if(tmp<jacTjac[i*m+i]) tmp=jacTjac[i*m+i];
    info[4]=mu/tmp;
    info[5]=(LM_REAL)k;
    info[6]=(LM_REAL)stop;
    info[7]=(LM_REAL)nfev;
    info[8]=(LM_REAL)njev;
  }

  /* covariance matrix */
  if(covar){
    LEVMAR_COVAR(jacTjac, covar, p_eL2, m, n);
  }

  if(freework) free(work);

#ifdef LINSOLVERS_RETAIN_MEMORY
  if(linsolver) (*linsolver)(NULL, NULL, NULL, 0);
#endif

  return (stop!=4 && stop!=7)?  k : LM_ERROR;
}


/* Secant version of the LEVMAR_DER() function above: the Jacobian is approximated with 
 * the aid of finite differences (forward or central, see the comment for the opts argument)
 */
int LEVMAR_DIF(
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata), /* functional relation describing measurements. A p \in R^m yields a \hat{x} \in  R^n */
  LM_REAL *p,         /* I/O: initial parameter estimates. On output has the estimated solution */
  LM_REAL *x,         /* I: measurement vector. NULL implies a zero vector */
  int m,              /* I: parameter vector dimension (i.e. #unknowns) */
  int n,              /* I: measurement vector dimension */
  int itmax,          /* I: maximum number of iterations */
  LM_REAL opts[5],    /* I: opts[0-4] = minim. options [\mu, \epsilon1, \epsilon2, \epsilon3, \delta]. Respectively the
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
  LM_REAL *work,     /* working memory at least LM_DIF_WORKSZ() reals large, allocated if NULL */
  LM_REAL *covar,    /* O: Covariance matrix corresponding to LS solution; mxm. Set to NULL if not needed. */
  void *adata)       /* pointer to possibly additional data, passed uninterpreted to func.
                      * Set to NULL if not needed
                      */
{
register int i, j, k, l;
int worksz, freework=0, issolved;
/* temp work arrays */
LM_REAL *e,          /* nx1 */
       *hx,         /* \hat{x}_i, nx1 */
       *jacTe,      /* J^T e_i mx1 */
       *jac,        /* nxm */
       *jacTjac,    /* mxm */
       *Dp,         /* mx1 */
   *diag_jacTjac,   /* diagonal of J^T J, mx1 */
       *pDp,        /* p + Dp, mx1 */
       *wrk,        /* nx1 */
       *wrk2;       /* nx1, used only for holding a temporary e vector and when differentiating with central differences */

int using_ffdif=1;

register LM_REAL mu,  /* damping constant */
                tmp; /* mainly used in matrix & vector multiplications */
LM_REAL p_eL2, jacTe_inf, pDp_eL2; /* ||e(p)||_2, ||J^T e||_inf, ||e(p+Dp)||_2 */
LM_REAL p_L2, Dp_L2=LM_REAL_MAX, dF, dL;
LM_REAL tau, eps1, eps2, eps2_sq, eps3, delta;
LM_REAL init_p_eL2;
int nu, nu2, stop=0, nfev, njap=0, K=(m>=10)? m: 10, updjac, updp=1, newjac;
const int nm=n*m;
int (*linsolver)(LM_REAL *A, LM_REAL *B, LM_REAL *x, int m)=NULL;

  mu=jacTe_inf=p_L2=0.0; /* -Wall */
  updjac=newjac=0; /* -Wall */

  if(n<m){
    fprintf(stderr, LCAT(LEVMAR_DIF, "(): cannot solve a problem with fewer measurements [%d] than unknowns [%d]\n"), n, m);
    return LM_ERROR;
  }

  if(opts){
	  tau=opts[0];
	  eps1=opts[1];
	  eps2=opts[2];
	  eps2_sq=opts[2]*opts[2];
    eps3=opts[3];
	  delta=opts[4];
    if(delta<0.0){
      delta=-delta; /* make positive */
      using_ffdif=0; /* use central differencing */
    }
  }
  else{ // use default values
	  tau=LM_CNST(LM_INIT_MU);
	  eps1=LM_CNST(LM_STOP_THRESH);
	  eps2=LM_CNST(LM_STOP_THRESH);
	  eps2_sq=LM_CNST(LM_STOP_THRESH)*LM_CNST(LM_STOP_THRESH);
    eps3=LM_CNST(LM_STOP_THRESH);
	  delta=LM_CNST(LM_DIFF_DELTA);
  }

  if(!work){
    worksz=LM_DIF_WORKSZ(m, n); //4*n+4*m + n*m + m*m;
    work=(LM_REAL *)malloc(worksz*sizeof(LM_REAL)); /* allocate a big chunk in one step */
    if(!work){
      fprintf(stderr, LCAT(LEVMAR_DIF, "(): memory allocation request failed\n"));
      exit(1);
    }
    freework=1;
  }

  /* set up work arrays */
  e=work;
  hx=e + n;
  jacTe=hx + n;
  jac=jacTe + m;
  jacTjac=jac + nm;
  Dp=jacTjac + m*m;
  diag_jacTjac=Dp + m;
  pDp=diag_jacTjac + m;
  wrk=pDp + m;
  wrk2=wrk + n;

  /* compute e=x - f(p) and its L2 norm */
  (*func)(p, hx, m, n, adata); nfev=1;
  /* ### e=x-hx, p_eL2=||e|| */
#if 1
  p_eL2=LEVMAR_L2NRMXMY(e, x, hx, n);
#else
  for(i=0, p_eL2=0.0; i<n; ++i){
    e[i]=tmp=x[i]-hx[i];
    p_eL2+=tmp*tmp;
  }
#endif
  init_p_eL2=p_eL2;
  if(!LM_FINITE(p_eL2)) stop=7;

  nu=20; /* force computation of J */

  for(k=0; k<itmax && !stop; ++k){
    /* Note that p and e have been updated at a previous iteration */

    if(p_eL2<=eps3){ /* error is small */
      stop=6;
      break;
    }

    /* Compute the Jacobian J at p,  J^T J,  J^T e,  ||J^T e||_inf and ||p||^2.
     * The symmetry of J^T J is again exploited for speed
     */

    if((updp && nu>16) || updjac==K){ /* compute difference approximation to J */
      if(using_ffdif){ /* use forward differences */
        LEVMAR_FDIF_FORW_JAC_APPROX(func, p, hx, wrk, delta, jac, m, n, adata);
        ++njap; nfev+=m;
      }
      else{ /* use central differences */
        LEVMAR_FDIF_CENT_JAC_APPROX(func, p, wrk, wrk2, delta, jac, m, n, adata);
        ++njap; nfev+=2*m;
      }
      nu=2; updjac=0; updp=0; newjac=1;
    }

    if(newjac){ /* Jacobian has changed, recompute J^T J, J^t e, etc */
      newjac=0;

      /* J^T J, J^T e */
      if(nm<=__BLOCKSZ__SQ){ // this is a small problem
        /* This is the straightforward way to compute J^T J, J^T e. However, due to
         * its noncontinuous memory access pattern, it incures many cache misses when
         * applied to large minimization problems (i.e. problems involving a large
         * number of free variables and measurements), in which J is too large to
         * fit in the L1 cache. For such problems, a cache-efficient blocking scheme
         * is preferable.
         *
         * Thanks to John Nitao of Lawrence Livermore Lab for pointing out this
         * performance problem.
         *
         * On the other hand, the straightforward algorithm is faster on small
         * problems since in this case it avoids the overheads of blocking. 
         */
      
        for(i=0; i<m; ++i){
          for(j=i; j<m; ++j){
            int lm;

            for(l=0, tmp=0.0; l<n; ++l){
              lm=l*m;
              tmp+=jac[lm+i]*jac[lm+j];
            }

            jacTjac[i*m+j]=jacTjac[j*m+i]=tmp;
          }

          /* J^T e */
          for(l=0, tmp=0.0; l<n; ++l)
            tmp+=jac[l*m+i]*e[l];
          jacTe[i]=tmp;
        }
      }
      else{ // this is a large problem
        /* Cache efficient computation of J^T J based on blocking
         */
        LEVMAR_TRANS_MAT_MAT_MULT(jac, jacTjac, n, m);

        /* cache efficient computation of J^T e */
        for(i=0; i<m; ++i)
          jacTe[i]=0.0;

        for(i=0; i<n; ++i){
          register LM_REAL *jacrow;

          for(l=0, jacrow=jac+i*m, tmp=e[i]; l<m; ++l)
            jacTe[l]+=jacrow[l]*tmp;
        }
      }
      
      /* Compute ||J^T e||_inf and ||p||^2 */
      for(i=0, p_L2=jacTe_inf=0.0; i<m; ++i){
        if(jacTe_inf < (tmp=FABS(jacTe[i]))) jacTe_inf=tmp;

        diag_jacTjac[i]=jacTjac[i*m+i]; /* save diagonal entries so that augmentation can be later canceled */
        p_L2+=p[i]*p[i];
      }
      //p_L2=sqrt(p_L2);
    }

#if 0
if(!(k%100)){
  printf("Current estimate: ");
  for(i=0; i<m; ++i)
    printf("%.9g ", p[i]);
  printf("-- errors %.9g %0.9g\n", jacTe_inf, p_eL2);
}
#endif

    /* check for convergence */
    if((jacTe_inf <= eps1)){
      Dp_L2=0.0; /* no increment for p in this case */
      stop=1;
      break;
    }

   /* compute initial damping factor */
    if(k==0){
      for(i=0, tmp=LM_REAL_MIN; i<m; ++i)
        if(diag_jacTjac[i]>tmp) tmp=diag_jacTjac[i]; /* find max diagonal element */
      mu=tau*tmp;
    }

    /* determine increment using adaptive damping */

    /* augment normal equations */
    for(i=0; i<m; ++i)
      jacTjac[i*m+i]+=mu;

    /* solve augmented equations */
#ifdef HAVE_LAPACK
    /* 5 alternatives are available: LU, Cholesky, 2 variants of QR decomposition and SVD.
     * Cholesky is the fastest but might be inaccurate; QR is slower but more accurate;
     * SVD is the slowest but most accurate; LU offers a tradeoff between accuracy and speed
     */

    issolved=AX_EQ_B_LU(jacTjac, jacTe, Dp, m); linsolver=AX_EQ_B_LU;
    //issolved=AX_EQ_B_CHOL(jacTjac, jacTe, Dp, m); linsolver=AX_EQ_B_CHOL;
    //issolved=AX_EQ_B_QR(jacTjac, jacTe, Dp, m); linsolver=AX_EQ_B_QR;
    //issolved=AX_EQ_B_QRLS(jacTjac, jacTe, Dp, m, m); linsolver=AX_EQ_B_QRLS;
    //issolved=AX_EQ_B_SVD(jacTjac, jacTe, Dp, m); linsolver=AX_EQ_B_SVD;
#else
    /* use the LU included with levmar */
    issolved=AX_EQ_B_LU(jacTjac, jacTe, Dp, m); linsolver=AX_EQ_B_LU;
#endif /* HAVE_LAPACK */

    if(issolved){
    /* compute p's new estimate and ||Dp||^2 */
      for(i=0, Dp_L2=0.0; i<m; ++i){
        pDp[i]=p[i] + (tmp=Dp[i]);
        Dp_L2+=tmp*tmp;
      }
      //Dp_L2=sqrt(Dp_L2);

      if(Dp_L2<=eps2_sq*p_L2){ /* relative change in p is small, stop */
      //if(Dp_L2<=eps2*(p_L2 + eps2)){ /* relative change in p is small, stop */
        stop=2;
        break;
      }

      if(Dp_L2>=(p_L2+eps2)/(LM_CNST(EPSILON)*LM_CNST(EPSILON))){ /* almost singular */
      //if(Dp_L2>=(p_L2+eps2)/LM_CNST(EPSILON)){ /* almost singular */
        stop=4;
        break;
      }

      (*func)(pDp, wrk, m, n, adata); ++nfev; /* evaluate function at p + Dp */
      /* compute ||e(pDp)||_2 */
      /* ### wrk2=x-wrk, pDp_eL2=||wrk2|| */
#if 1
      pDp_eL2=LEVMAR_L2NRMXMY(wrk2, x, wrk, n);
#else
      for(i=0, pDp_eL2=0.0; i<n; ++i){
        wrk2[i]=tmp=x[i]-wrk[i];
        pDp_eL2+=tmp*tmp;
      }
#endif
      if(!LM_FINITE(pDp_eL2)){ /* sum of squares is not finite, most probably due to a user error.
                                * This check makes sure that the loop terminates early in the case
                                * of invalid input. Thanks to Steve Danauskas for suggesting it
                                */

        stop=7;
        break;
      }

      dF=p_eL2-pDp_eL2;
      if(updp || dF>0){ /* update jac */
        for(i=0; i<n; ++i){
          for(l=0, tmp=0.0; l<m; ++l)
            tmp+=jac[i*m+l]*Dp[l]; /* (J * Dp)[i] */
          tmp=(wrk[i] - hx[i] - tmp)/Dp_L2; /* (f(p+dp)[i] - f(p)[i] - (J * Dp)[i])/(dp^T*dp) */
          for(j=0; j<m; ++j)
            jac[i*m+j]+=tmp*Dp[j];
        }
        ++updjac;
        newjac=1;
      }

      for(i=0, dL=0.0; i<m; ++i)
        dL+=Dp[i]*(mu*Dp[i]+jacTe[i]);

      if(dL>0.0 && dF>0.0){ /* reduction in error, increment is accepted */
        tmp=(LM_CNST(2.0)*dF/dL-LM_CNST(1.0));
        tmp=LM_CNST(1.0)-tmp*tmp*tmp;
        mu=mu*( (tmp>=LM_CNST(ONE_THIRD))? tmp : LM_CNST(ONE_THIRD) );
        nu=2;

        for(i=0 ; i<m; ++i) /* update p's estimate */
          p[i]=pDp[i];

        for(i=0; i<n; ++i){ /* update e, hx and ||e||_2 */
          e[i]=wrk2[i]; //x[i]-wrk[i];
          hx[i]=wrk[i];
        }
        p_eL2=pDp_eL2;
        updp=1;
        continue;
      }
    }

    /* if this point is reached, either the linear system could not be solved or
     * the error did not reduce; in any case, the increment must be rejected
     */

    mu*=nu;
    nu2=nu<<1; // 2*nu;
    if(nu2<=nu){ /* nu has wrapped around (overflown). Thanks to Frank Jordan for spotting this case */
      stop=5;
      break;
    }
    nu=nu2;

    for(i=0; i<m; ++i) /* restore diagonal J^T J entries */
      jacTjac[i*m+i]=diag_jacTjac[i];
  }

  if(k>=itmax) stop=3;

  for(i=0; i<m; ++i) /* restore diagonal J^T J entries */
    jacTjac[i*m+i]=diag_jacTjac[i];

  if(info){
    info[0]=init_p_eL2;
    info[1]=p_eL2;
    info[2]=jacTe_inf;
    info[3]=Dp_L2;
    for(i=0, tmp=LM_REAL_MIN; i<m; ++i)
      if(tmp<jacTjac[i*m+i]) tmp=jacTjac[i*m+i];
    info[4]=mu/tmp;
    info[5]=(LM_REAL)k;
    info[6]=(LM_REAL)stop;
    info[7]=(LM_REAL)nfev;
    info[8]=(LM_REAL)njap;
  }

  /* covariance matrix */
  if(covar){
    LEVMAR_COVAR(jacTjac, covar, p_eL2, m, n);
  }

                                                               
  if(freework) free(work);

#ifdef LINSOLVERS_RETAIN_MEMORY
  if(linsolver) (*linsolver)(NULL, NULL, NULL, 0);
#endif

  return (stop!=4 && stop!=7)?  k : LM_ERROR;
}

/* undefine everything. THIS MUST REMAIN AT THE END OF THE FILE */
#undef LEVMAR_DER
#undef LEVMAR_DIF
#undef LEVMAR_FDIF_FORW_JAC_APPROX
#undef LEVMAR_FDIF_CENT_JAC_APPROX
#undef LEVMAR_COVAR
#undef LEVMAR_TRANS_MAT_MAT_MULT
#undef LEVMAR_L2NRMXMY
#undef AX_EQ_B_LU
#undef AX_EQ_B_CHOL
#undef AX_EQ_B_QR
#undef AX_EQ_B_QRLS
#undef AX_EQ_B_SVD
