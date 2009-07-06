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

#ifndef LM_REAL // not included by lmbc.c
#error This file should not be compiled directly!
#endif


/* precision-specific definitions */
#define FUNC_STATE LM_ADD_PREFIX(func_state)
#define LNSRCH LM_ADD_PREFIX(lnsrch)
#define BOXPROJECT LM_ADD_PREFIX(boxProject)
#define LEVMAR_BOX_CHECK LM_ADD_PREFIX(levmar_box_check)
#define LEVMAR_BC_DER LM_ADD_PREFIX(levmar_bc_der)
#define LEVMAR_BC_DIF LM_ADD_PREFIX(levmar_bc_dif) //CHECKME
#define LEVMAR_FDIF_FORW_JAC_APPROX LM_ADD_PREFIX(levmar_fdif_forw_jac_approx)
#define LEVMAR_FDIF_CENT_JAC_APPROX LM_ADD_PREFIX(levmar_fdif_cent_jac_approx)
#define LEVMAR_TRANS_MAT_MAT_MULT LM_ADD_PREFIX(levmar_trans_mat_mat_mult)
#define LEVMAR_L2NRMXMY LM_ADD_PREFIX(levmar_L2nrmxmy)
#define LEVMAR_COVAR LM_ADD_PREFIX(levmar_covar)
#define LMBC_DIF_DATA LM_ADD_PREFIX(lmbc_dif_data)
#define LMBC_DIF_FUNC LM_ADD_PREFIX(lmbc_dif_func)
#define LMBC_DIF_JACF LM_ADD_PREFIX(lmbc_dif_jacf)

#ifdef HAVE_LAPACK
#define AX_EQ_B_LU LM_ADD_PREFIX(Ax_eq_b_LU)
#define AX_EQ_B_CHOL LM_ADD_PREFIX(Ax_eq_b_Chol)
#define AX_EQ_B_QR LM_ADD_PREFIX(Ax_eq_b_QR)
#define AX_EQ_B_QRLS LM_ADD_PREFIX(Ax_eq_b_QRLS)
#define AX_EQ_B_SVD LM_ADD_PREFIX(Ax_eq_b_SVD)
#else
#define AX_EQ_B_LU LM_ADD_PREFIX(Ax_eq_b_LU_noLapack)
#endif /* HAVE_LAPACK */

/* find the median of 3 numbers */
#define __MEDIAN3(a, b, c) ( ((a) >= (b))?\
        ( ((c) >= (a))? (a) : ( ((c) <= (b))? (b) : (c) ) ) : \
        ( ((c) >= (b))? (b) : ( ((c) <= (a))? (a) : (c) ) ) )

#define _POW_ LM_CNST(2.1)

#define __LSITMAX   150 // max #iterations for line search

struct FUNC_STATE{
  int n, *nfev;
  LM_REAL *hx, *x;
  void *adata;
};

static void
LNSRCH(int m, LM_REAL *x, LM_REAL f, LM_REAL *g, LM_REAL *p, LM_REAL alpha, LM_REAL *xpls,
       LM_REAL *ffpls, void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata), struct FUNC_STATE state,
       int *mxtake, int *iretcd, LM_REAL stepmx, LM_REAL steptl, LM_REAL *sx)
{
/* Find a next newton iterate by backtracking line search.
 * Specifically, finds a \lambda such that for a fixed alpha<0.5 (usually 1e-4),
 * f(x + \lambda*p) <= f(x) + alpha * \lambda * g^T*p
 *
 * Translated (with minor changes) from Schnabel, Koontz & Weiss uncmin.f,  v1.3

 * PARAMETERS :

 *	m       --> dimension of problem (i.e. number of variables)
 *	x(m)    --> old iterate:	x[k-1]
 *	f       --> function value at old iterate, f(x)
 *	g(m)    --> gradient at old iterate, g(x), or approximate
 *	p(m)    --> non-zero newton step
 *	alpha   --> fixed constant < 0.5 for line search (see above)
 *	xpls(m) <--	 new iterate x[k]
 *	ffpls   <--	 function value at new iterate, f(xpls)
 *	func    --> name of subroutine to evaluate function
 *	state   <--> information other than x and m that func requires.
 *			    state is not modified in xlnsrch (but can be modified by func).
 *	iretcd  <--	 return code
 *	mxtake  <--	 boolean flag indicating step of maximum length used
 *	stepmx  --> maximum allowable step size
 *	steptl  --> relative step size at which successive iterates
 *			    considered close enough to terminate algorithm
 *	sx(m)	  --> diagonal scaling matrix for x, can be NULL

 *	internal variables

 *	sln		 newton length
 *	rln		 relative length of newton step
*/

    register int i, j;
    int firstback = 1;
    LM_REAL disc;
    LM_REAL a3, b;
    LM_REAL t1, t2, t3, lambda, tlmbda, rmnlmb;
    LM_REAL scl, rln, sln, slp;
    LM_REAL tmp1, tmp2;
    LM_REAL fpls, pfpls = 0., plmbda = 0.; /* -Wall */

    f*=LM_CNST(0.5);
    *mxtake = 0;
    *iretcd = 2;
    tmp1 = 0.;
    if(!sx) /* no scaling */
      for (i = 0; i < m; ++i)
        tmp1 += p[i] * p[i];
    else
      for (i = 0; i < m; ++i)
        tmp1 += sx[i] * sx[i] * p[i] * p[i];
    sln = (LM_REAL)sqrt(tmp1);
    if (sln > stepmx) {
	  /*	newton step longer than maximum allowed */
	    scl = stepmx / sln;
      for(i=0; i<m; ++i) /* p * scl */
        p[i]*=scl;
	    sln = stepmx;
    }
    for(i=0, slp=0.; i<m; ++i) /* g^T * p */
      slp+=g[i]*p[i];
    rln = 0.;
    if(!sx) /* no scaling */
      for (i = 0; i < m; ++i) {
	      tmp1 = (FABS(x[i])>=LM_CNST(1.))? FABS(x[i]) : LM_CNST(1.);
	      tmp2 = FABS(p[i])/tmp1;
	      if(rln < tmp2) rln = tmp2;
      }
    else
      for (i = 0; i < m; ++i) {
	      tmp1 = (FABS(x[i])>=LM_CNST(1.)/sx[i])? FABS(x[i]) : LM_CNST(1.)/sx[i];
	      tmp2 = FABS(p[i])/tmp1;
	      if(rln < tmp2) rln = tmp2;
      }
    rmnlmb = steptl / rln;
    lambda = LM_CNST(1.0);

    /*	check if new iterate satisfactory.  generate new lambda if necessary. */

    for(j=__LSITMAX; j>=0; --j) {
	    for (i = 0; i < m; ++i)
	      xpls[i] = x[i] + lambda * p[i];

      /* evaluate function at new point */
      (*func)(xpls, state.hx, m, state.n, state.adata); ++(*(state.nfev));
      /* ### state.hx=state.x-state.hx, tmp1=||state.hx|| */
#if 1
       tmp1=LEVMAR_L2NRMXMY(state.hx, state.x, state.hx, state.n);
#else
      for(i=0, tmp1=0.0; i<state.n; ++i){
        state.hx[i]=tmp2=state.x[i]-state.hx[i];
        tmp1+=tmp2*tmp2;
      }
#endif
      fpls=LM_CNST(0.5)*tmp1; *ffpls=tmp1;

	    if (fpls <= f + slp * alpha * lambda) { /* solution found */
	      *iretcd = 0;
	      if (lambda == LM_CNST(1.) && sln > stepmx * LM_CNST(.99)) *mxtake = 1;
	      return;
	    }

	    /* else : solution not (yet) found */

      /* First find a point with a finite value */

	    if (lambda < rmnlmb) {
	      /* no satisfactory xpls found sufficiently distinct from x */

	      *iretcd = 1;
	      return;
	    }
	    else { /*	calculate new lambda */

	      /* modifications to cover non-finite values */
	      if (!LM_FINITE(fpls)) {
		      lambda *= LM_CNST(0.1);
		      firstback = 1;
	      }
	      else {
		      if (firstback) { /*	first backtrack: quadratic fit */
		        tlmbda = -lambda * slp / ((fpls - f - slp) * LM_CNST(2.));
		        firstback = 0;
		      }
		      else { /*	all subsequent backtracks: cubic fit */
		        t1 = fpls - f - lambda * slp;
		        t2 = pfpls - f - plmbda * slp;
		        t3 = LM_CNST(1.) / (lambda - plmbda);
		        a3 = LM_CNST(3.) * t3 * (t1 / (lambda * lambda)
				      - t2 / (plmbda * plmbda));
		        b = t3 * (t2 * lambda / (plmbda * plmbda)
			          - t1 * plmbda / (lambda * lambda));
		        disc = b * b - a3 * slp;
		        if (disc > b * b)
			      /* only one positive critical point, must be minimum */
			        tlmbda = (-b + ((a3 < 0)? -(LM_REAL)sqrt(disc): (LM_REAL)sqrt(disc))) /a3;
		        else
			      /* both critical points positive, first is minimum */
			        tlmbda = (-b + ((a3 < 0)? (LM_REAL)sqrt(disc): -(LM_REAL)sqrt(disc))) /a3;

		        if (tlmbda > lambda * LM_CNST(.5))
			        tlmbda = lambda * LM_CNST(.5);
		      }
		      plmbda = lambda;
		      pfpls = fpls;
		      if (tlmbda < lambda * LM_CNST(.1))
		        lambda *= LM_CNST(.1);
		      else
		        lambda = tlmbda;
        }
	    }
    }
    /* this point is reached when the iterations limit is exceeded */
	  *iretcd = 1; /* failed */
	  return;
} /* LNSRCH */

/* Projections to feasible set \Omega: P_{\Omega}(y) := arg min { ||x - y|| : x \in \Omega},  y \in R^m */

/* project vector p to a box shaped feasible set. p is a mx1 vector.
 * Either lb, ub can be NULL. If not NULL, they are mx1 vectors
 */
static void BOXPROJECT(LM_REAL *p, LM_REAL *lb, LM_REAL *ub, int m)
{
register int i;

  if(!lb){ /* no lower bounds */
    if(!ub) /* no upper bounds */
      return;
    else{ /* upper bounds only */
      for(i=0; i<m; ++i)
        if(p[i]>ub[i]) p[i]=ub[i];
    }
  }
  else
    if(!ub){ /* lower bounds only */
      for(i=0; i<m; ++i)
        if(p[i]<lb[i]) p[i]=lb[i];
    }
    else /* box bounds */
      for(i=0; i<m; ++i)
        p[i]=__MEDIAN3(lb[i], p[i], ub[i]);
}

/* 
 * This function seeks the parameter vector p that best describes the measurements
 * vector x under box constraints.
 * More precisely, given a vector function  func : R^m --> R^n with n>=m,
 * it finds p s.t. func(p) ~= x, i.e. the squared second order (i.e. L2) norm of
 * e=x-func(p) is minimized under the constraints lb[i]<=p[i]<=ub[i].
 * If no lower bound constraint applies for p[i], use -DBL_MAX/-FLT_MAX for lb[i];
 * If no upper bound constraint applies for p[i], use DBL_MAX/FLT_MAX for ub[i].
 *
 * This function requires an analytic Jacobian. In case the latter is unavailable,
 * use LEVMAR_BC_DIF() bellow
 *
 * Returns the number of iterations (>=0) if successfull, LM_ERROR if failed
 *
 * For details, see C. Kanzow, N. Yamashita and M. Fukushima: "Levenberg-Marquardt
 * methods for constrained nonlinear equations with strong local convergence properties",
 * Journal of Computational and Applied Mathematics 172, 2004, pp. 375-397.
 * Also, see K. Madsen, H.B. Nielsen and O. Tingleff's lecture notes on 
 * unconstrained Levenberg-Marquardt at http://www.imm.dtu.dk/pubdb/views/edoc_download.php/3215/pdf/imm3215.pdf
 */

int LEVMAR_BC_DER(
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata), /* functional relation describing measurements. A p \in R^m yields a \hat{x} \in  R^n */
  void (*jacf)(LM_REAL *p, LM_REAL *j, int m, int n, void *adata),  /* function to evaluate the Jacobian \part x / \part p */ 
  LM_REAL *p,         /* I/O: initial parameter estimates. On output has the estimated solution */
  LM_REAL *x,         /* I: measurement vector. NULL implies a zero vector */
  int m,              /* I: parameter vector dimension (i.e. #unknowns) */
  int n,              /* I: measurement vector dimension */
  LM_REAL *lb,        /* I: vector of lower bounds. If NULL, no lower bounds apply */
  LM_REAL *ub,        /* I: vector of upper bounds. If NULL, no upper bounds apply */
  int itmax,          /* I: maximum number of iterations */
  LM_REAL opts[4],    /* I: minim. options [\mu, \epsilon1, \epsilon2, \epsilon3]. Respectively the scale factor for initial \mu,
                       * stopping thresholds for ||J^T e||_inf, ||Dp||_2 and ||e||_2. Set to NULL for defaults to be used.
                       * Note that ||J^T e||_inf is computed on free (not equal to lb[i] or ub[i]) variables only.
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
  LM_REAL *work,     /* working memory at least LM_BC_DER_WORKSZ() reals large, allocated if NULL */
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

/* variables for constrained LM */
struct FUNC_STATE fstate;
LM_REAL alpha=LM_CNST(1e-4), beta=LM_CNST(0.9), gamma=LM_CNST(0.99995), gamma_sq=gamma*gamma, rho=LM_CNST(1e-8);
LM_REAL t, t0;
LM_REAL steptl=LM_CNST(1e3)*(LM_REAL)sqrt(LM_REAL_EPSILON), jacTeDp;
LM_REAL tmin=LM_CNST(1e-12), tming=LM_CNST(1e-18); /* minimum step length for LS and PG steps */
const LM_REAL tini=LM_CNST(1.0); /* initial step length for LS and PG steps */
int nLMsteps=0, nLSsteps=0, nPGsteps=0, gprevtaken=0;
int numactive;
int (*linsolver)(LM_REAL *A, LM_REAL *B, LM_REAL *x, int m)=NULL;

  mu=jacTe_inf=t=0.0;  tmin=tmin; /* -Wall */

  if(n<m){
    fprintf(stderr, LCAT(LEVMAR_BC_DER, "(): cannot solve a problem with fewer measurements [%d] than unknowns [%d]\n"), n, m);
    return LM_ERROR;
  }

  if(!jacf){
    fprintf(stderr, RCAT("No function specified for computing the Jacobian in ", LEVMAR_BC_DER)
        RCAT("().\nIf no such function is available, use ", LEVMAR_BC_DIF) RCAT("() rather than ", LEVMAR_BC_DER) "()\n");
    return LM_ERROR;
  }

  if(!LEVMAR_BOX_CHECK(lb, ub, m)){
    fprintf(stderr, LCAT(LEVMAR_BC_DER, "(): at least one lower bound exceeds the upper one\n"));
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
    worksz=LM_BC_DER_WORKSZ(m, n); //2*n+4*m + n*m + m*m;
    work=(LM_REAL *)malloc(worksz*sizeof(LM_REAL)); /* allocate a big chunk in one step */
    if(!work){
      fprintf(stderr, LCAT(LEVMAR_BC_DER, "(): memory allocation request failed\n"));
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

  fstate.n=n;
  fstate.hx=hx;
  fstate.x=x;
  fstate.adata=adata;
  fstate.nfev=&nfev;
  
  /* see if starting point is within the feasile set */
  for(i=0; i<m; ++i)
    pDp[i]=p[i];
  BOXPROJECT(p, lb, ub, m); /* project to feasible set */
  for(i=0; i<m; ++i)
    if(pDp[i]!=p[i])
      fprintf(stderr, RCAT("Warning: component %d of starting point not feasible in ", LEVMAR_BC_DER) "()! [%g projected to %g]\n",
                      i, pDp[i], p[i]);

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

	  /* Compute ||J^T e||_inf and ||p||^2. Note that ||J^T e||_inf
     * is computed for free (i.e. inactive) variables only. 
     * At a local minimum, if p[i]==ub[i] then g[i]>0;
     * if p[i]==lb[i] g[i]<0; otherwise g[i]=0 
     */
    for(i=j=numactive=0, p_L2=jacTe_inf=0.0; i<m; ++i){
      if(ub && p[i]==ub[i]){ ++numactive; if(jacTe[i]>0.0) ++j; }
      else if(lb && p[i]==lb[i]){ ++numactive; if(jacTe[i]<0.0) ++j; }
      else if(jacTe_inf < (tmp=FABS(jacTe[i]))) jacTe_inf=tmp;

      diag_jacTjac[i]=jacTjac[i*m+i]; /* save diagonal entries so that augmentation can be later canceled */
      p_L2+=p[i]*p[i];
    }
    //p_L2=sqrt(p_L2);

#if 0
if(!(k%100)){
  printf("Current estimate: ");
  for(i=0; i<m; ++i)
    printf("%.9g ", p[i]);
  printf("-- errors %.9g %0.9g, #active %d [%d]\n", jacTe_inf, p_eL2, numactive, j);
}
#endif

    /* check for convergence */
    if(j==numactive && (jacTe_inf <= eps1)){
      Dp_L2=0.0; /* no increment for p in this case */
      stop=1;
      break;
    }

   /* compute initial damping factor */
    if(k==0){
      if(!lb && !ub){ /* no bounds */
        for(i=0, tmp=LM_REAL_MIN; i<m; ++i)
          if(diag_jacTjac[i]>tmp) tmp=diag_jacTjac[i]; /* find max diagonal element */
        mu=tau*tmp;
      }
      else 
        mu=LM_CNST(0.5)*tau*p_eL2; /* use Kanzow's starting mu */
    }

    /* determine increment using a combination of adaptive damping, line search and projected gradient search */
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
        for(i=0; i<m; ++i)
          pDp[i]=p[i] + Dp[i];

        /* compute p's new estimate and ||Dp||^2 */
        BOXPROJECT(pDp, lb, ub, m); /* project to feasible set */
        for(i=0, Dp_L2=0.0; i<m; ++i){
          Dp[i]=tmp=pDp[i]-p[i];
          Dp_L2+=tmp*tmp;
        }
        //Dp_L2=sqrt(Dp_L2);

        if(Dp_L2<=eps2_sq*p_L2){ /* relative change in p is small, stop */
          stop=2;
          break;
        }

        if(Dp_L2>=(p_L2+eps2)/(LM_CNST(EPSILON)*LM_CNST(EPSILON))){ /* almost singular */
          stop=4;
          break;
        }

        (*func)(pDp, hx, m, n, adata); ++nfev; /* evaluate function at p + Dp */
        /* ### hx=x-hx, pDp_eL2=||hx|| */
#if 1
        pDp_eL2=LEVMAR_L2NRMXMY(hx, x, hx, n);
#else
        for(i=0, pDp_eL2=0.0; i<n; ++i){ /* compute ||e(pDp)||_2 */
          hx[i]=tmp=x[i]-hx[i];
          pDp_eL2+=tmp*tmp;
        }
#endif
        if(!LM_FINITE(pDp_eL2)){
          stop=7;
          break;
        }

        if(pDp_eL2<=gamma_sq*p_eL2){
          for(i=0, dL=0.0; i<m; ++i)
            dL+=Dp[i]*(mu*Dp[i]+jacTe[i]);

#if 1
          if(dL>0.0){
            dF=p_eL2-pDp_eL2;
            tmp=(LM_CNST(2.0)*dF/dL-LM_CNST(1.0));
            tmp=LM_CNST(1.0)-tmp*tmp*tmp;
            mu=mu*( (tmp>=LM_CNST(ONE_THIRD))? tmp : LM_CNST(ONE_THIRD) );
          }
          else
            mu=(mu>=pDp_eL2)? pDp_eL2 : mu; /* pDp_eL2 is the new pDp_eL2 */
#else

          mu=(mu>=pDp_eL2)? pDp_eL2 : mu; /* pDp_eL2 is the new pDp_eL2 */
#endif

          nu=2;

          for(i=0 ; i<m; ++i) /* update p's estimate */
            p[i]=pDp[i];

          for(i=0; i<n; ++i) /* update e and ||e||_2 */
            e[i]=hx[i];
          p_eL2=pDp_eL2;
          ++nLMsteps;
          gprevtaken=0;
          break;
        }
      }
      else{

      /* the augmented linear system could not be solved, increase mu */

        mu*=nu;
        nu2=nu<<1; // 2*nu;
        if(nu2<=nu){ /* nu has wrapped around (overflown). Thanks to Frank Jordan for spotting this case */
          stop=5;
          break;
        }
        nu=nu2;

        for(i=0; i<m; ++i) /* restore diagonal J^T J entries */
          jacTjac[i*m+i]=diag_jacTjac[i];

        continue; /* solve again with increased nu */
      }

      /* if this point is reached, the LM step did not reduce the error;
       * see if it is a descent direction
       */

      /* negate jacTe (i.e. g) & compute g^T * Dp */
      for(i=0, jacTeDp=0.0; i<m; ++i){
        jacTe[i]=-jacTe[i];
        jacTeDp+=jacTe[i]*Dp[i];
      }

      if(jacTeDp<=-rho*pow(Dp_L2, _POW_/LM_CNST(2.0))){
        /* Dp is a descent direction; do a line search along it */
        int mxtake, iretcd;
        LM_REAL stepmx;

        tmp=(LM_REAL)sqrt(p_L2); stepmx=LM_CNST(1e3)*( (tmp>=LM_CNST(1.0))? tmp : LM_CNST(1.0) );

#if 1
        /* use Schnabel's backtracking line search; it requires fewer "func" evaluations */
        LNSRCH(m, p, p_eL2, jacTe, Dp, alpha, pDp, &pDp_eL2, func, fstate,
               &mxtake, &iretcd, stepmx, steptl, NULL); /* NOTE: LNSRCH() updates hx */
        if(iretcd!=0) goto gradproj; /* rather inelegant but effective way to handle LNSRCH() failures... */
#else
        /* use the simpler (but slower!) line search described by Kanzow et al */
        for(t=tini; t>tmin; t*=beta){
          for(i=0; i<m; ++i){
            pDp[i]=p[i] + t*Dp[i];
            //pDp[i]=__MEDIAN3(lb[i], pDp[i], ub[i]); /* project to feasible set */
          }

          (*func)(pDp, hx, m, n, adata); ++nfev; /* evaluate function at p + t*Dp */
          for(i=0, pDp_eL2=0.0; i<n; ++i){ /* compute ||e(pDp)||_2 */
            hx[i]=tmp=x[i]-hx[i];
            pDp_eL2+=tmp*tmp;
          }
          if(!LM_FINITE(pDp_eL2)) goto gradproj; /* treat as line search failure */

          //if(LM_CNST(0.5)*pDp_eL2<=LM_CNST(0.5)*p_eL2 + t*alpha*jacTeDp) break;
          if(pDp_eL2<=p_eL2 + LM_CNST(2.0)*t*alpha*jacTeDp) break;
        }
#endif
        ++nLSsteps;
        gprevtaken=0;

        /* NOTE: new estimate for p is in pDp, associated error in hx and its norm in pDp_eL2.
         * These values are used below to update their corresponding variables 
         */
      }
      else{
gradproj: /* Note that this point can also be reached via a goto when LNSRCH() fails */

        /* jacTe is a descent direction; make a projected gradient step */

        /* if the previous step was along the gradient descent, try to use the t employed in that step */
        /* compute ||g|| */
        for(i=0, tmp=0.0; i<m; ++i)
          tmp+=jacTe[i]*jacTe[i];
        tmp=(LM_REAL)sqrt(tmp);
        tmp=LM_CNST(100.0)/(LM_CNST(1.0)+tmp);
        t0=(tmp<=tini)? tmp : tini; /* guard against poor scaling & large steps; see (3.50) in C.T. Kelley's book */

        for(t=(gprevtaken)? t : t0; t>tming; t*=beta){
          for(i=0; i<m; ++i)
            pDp[i]=p[i] - t*jacTe[i];
          BOXPROJECT(pDp, lb, ub, m); /* project to feasible set */
          for(i=0; i<m; ++i)
            Dp[i]=pDp[i]-p[i];

          (*func)(pDp, hx, m, n, adata); ++nfev; /* evaluate function at p - t*g */
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
          if(!LM_FINITE(pDp_eL2)){
            stop=7;
            goto breaknested;
          }

          for(i=0, tmp=0.0; i<m; ++i) /* compute ||g^T * Dp|| */
            tmp+=jacTe[i]*Dp[i];

          if(gprevtaken && pDp_eL2<=p_eL2 + LM_CNST(2.0)*LM_CNST(0.99999)*tmp){ /* starting t too small */
            t=t0;
            gprevtaken=0;
            continue;
          }
          //if(LM_CNST(0.5)*pDp_eL2<=LM_CNST(0.5)*p_eL2 + alpha*tmp) break;
          if(pDp_eL2<=p_eL2 + LM_CNST(2.0)*alpha*tmp) break;
        }

        ++nPGsteps;
        gprevtaken=1;
        /* NOTE: new estimate for p is in pDp, associated error in hx and its norm in pDp_eL2 */
      }

      /* update using computed values */

      for(i=0, Dp_L2=0.0; i<m; ++i){
        tmp=pDp[i]-p[i];
        Dp_L2+=tmp*tmp;
      }
      //Dp_L2=sqrt(Dp_L2);

      if(Dp_L2<=eps2_sq*p_L2){ /* relative change in p is small, stop */
        stop=2;
        break;
      }

      for(i=0 ; i<m; ++i) /* update p's estimate */
        p[i]=pDp[i];

      for(i=0; i<n; ++i) /* update e and ||e||_2 */
        e[i]=hx[i];
      p_eL2=pDp_eL2;
      break;
    } /* inner loop */
  }

breaknested: /* NOTE: this point is also reached via an explicit goto! */

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

#if 0
printf("%d LM steps, %d line search, %d projected gradient\n", nLMsteps, nLSsteps, nPGsteps);
#endif

  return (stop!=4 && stop!=7)?  k : LM_ERROR;
}

/* following struct & LMBC_DIF_XXX functions won't be necessary if a true secant
 * version of LEVMAR_BC_DIF() is implemented...
 */
struct LMBC_DIF_DATA{
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata);
  LM_REAL *hx, *hxx;
  void *adata;
  LM_REAL delta;
};

void LMBC_DIF_FUNC(LM_REAL *p, LM_REAL *hx, int m, int n, void *data)
{
struct LMBC_DIF_DATA *dta=(struct LMBC_DIF_DATA *)data;

  /* call user-supplied function passing it the user-supplied data */
  (*(dta->func))(p, hx, m, n, dta->adata);
}

void LMBC_DIF_JACF(LM_REAL *p, LM_REAL *jac, int m, int n, void *data)
{
struct LMBC_DIF_DATA *dta=(struct LMBC_DIF_DATA *)data;

  /* evaluate user-supplied function at p */
  (*(dta->func))(p, dta->hx, m, n, dta->adata);
  LEVMAR_FDIF_FORW_JAC_APPROX(dta->func, p, dta->hx, dta->hxx, dta->delta, jac, m, n, dta->adata);
}


/* No Jacobian version of the LEVMAR_BC_DER() function above: the Jacobian is approximated with 
 * the aid of finite differences (forward or central, see the comment for the opts argument)
 * Ideally, this function should be implemented with a secant approach. Currently, it just calls
 * LEVMAR_BC_DER()
 */
int LEVMAR_BC_DIF(
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata), /* functional relation describing measurements. A p \in R^m yields a \hat{x} \in  R^n */
  LM_REAL *p,         /* I/O: initial parameter estimates. On output has the estimated solution */
  LM_REAL *x,         /* I: measurement vector. NULL implies a zero vector */
  int m,              /* I: parameter vector dimension (i.e. #unknowns) */
  int n,              /* I: measurement vector dimension */
  LM_REAL *lb,        /* I: vector of lower bounds. If NULL, no lower bounds apply */
  LM_REAL *ub,        /* I: vector of upper bounds. If NULL, no upper bounds apply */
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
  LM_REAL *work,     /* working memory at least LM_BC_DIF_WORKSZ() reals large, allocated if NULL */
  LM_REAL *covar,    /* O: Covariance matrix corresponding to LS solution; mxm. Set to NULL if not needed. */
  void *adata)       /* pointer to possibly additional data, passed uninterpreted to func.
                      * Set to NULL if not needed
                      */
{
struct LMBC_DIF_DATA data;
int ret;

    //fprintf(stderr, RCAT("\nWarning: current implementation of ", LEVMAR_BC_DIF) "() does not use a secant approach!\n\n");

    data.func=func;
    data.hx=(LM_REAL *)malloc(2*n*sizeof(LM_REAL)); /* allocate a big chunk in one step */
    if(!data.hx){
      fprintf(stderr, LCAT(LEVMAR_BC_DIF, "(): memory allocation request failed\n"));
      exit(1);
    }
    data.hxx=data.hx+n;
    data.adata=adata;
    data.delta=(opts)? FABS(opts[4]) : (LM_REAL)LM_DIFF_DELTA; // no central differences here...

    ret=LEVMAR_BC_DER(LMBC_DIF_FUNC, LMBC_DIF_JACF, p, x, m, n, lb, ub, itmax, opts, info, work, covar, (void *)&data);

    if(info) /* correct the number of function calls */
      info[7]+=info[8]*(m+1); /* each Jacobian evaluation costs m+1 function calls */

    free(data.hx);

    return ret;
}
/* undefine everything. THIS MUST REMAIN AT THE END OF THE FILE */
#undef FUNC_STATE
#undef LNSRCH
#undef BOXPROJECT
#undef LEVMAR_BOX_CHECK
#undef LEVMAR_BC_DER
#undef LMBC_DIF_DATA
#undef LMBC_DIF_FUNC
#undef LMBC_DIF_JACF
#undef LEVMAR_BC_DIF
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
