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

#ifndef LM_REAL // not included by lmlec.c
#error This file should not be compiled directly!
#endif


/* precision-specific definitions */
#define LMLEC_DATA LM_ADD_PREFIX(lmlec_data)
#define LMLEC_ELIM LM_ADD_PREFIX(lmlec_elim)
#define LMLEC_FUNC LM_ADD_PREFIX(lmlec_func)
#define LMLEC_JACF LM_ADD_PREFIX(lmlec_jacf)
#define LEVMAR_LEC_DER LM_ADD_PREFIX(levmar_lec_der)
#define LEVMAR_LEC_DIF LM_ADD_PREFIX(levmar_lec_dif)
#define LEVMAR_DER LM_ADD_PREFIX(levmar_der)
#define LEVMAR_DIF LM_ADD_PREFIX(levmar_dif)
#define LEVMAR_TRANS_MAT_MAT_MULT LM_ADD_PREFIX(levmar_trans_mat_mat_mult)
#define LEVMAR_COVAR LM_ADD_PREFIX(levmar_covar)
#define LEVMAR_FDIF_FORW_JAC_APPROX LM_ADD_PREFIX(levmar_fdif_forw_jac_approx)

#define GEQP3 LM_ADD_PREFIX(geqp3_)
#define ORGQR LM_ADD_PREFIX(orgqr_)
#define TRTRI LM_ADD_PREFIX(trtri_)

struct LMLEC_DATA{
  LM_REAL *c, *Z, *p, *jac;
  int ncnstr;
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata);
  void (*jacf)(LM_REAL *p, LM_REAL *jac, int m, int n, void *adata);
  void *adata;
};

/* prototypes for LAPACK routines */
extern int GEQP3(int *m, int *n, LM_REAL *a, int *lda, int *jpvt,
                   LM_REAL *tau, LM_REAL *work, int *lwork, int *info);

extern int ORGQR(int *m, int *n, int *k, LM_REAL *a, int *lda, LM_REAL *tau,
                   LM_REAL *work, int *lwork, int *info);

extern int TRTRI(char *uplo, char *diag, int *n, LM_REAL *a, int *lda, int *info);

/*
 * This function implements an elimination strategy for linearly constrained
 * optimization problems. The strategy relies on QR decomposition to transform
 * an optimization problem constrained by Ax=b to an equivalent, unconstrained
 * one. Also referred to as "null space" or "reduced Hessian" method.
 * See pp. 430-433 (chap. 15) of "Numerical Optimization" by Nocedal-Wright
 * for details.
 *
 * A is mxn with m<=n and rank(A)=m
 * Two matrices Y and Z of dimensions nxm and nx(n-m) are computed from A^T so that
 * their columns are orthonormal and every x can be written as x=Y*b + Z*x_z=
 * c + Z*x_z, where c=Y*b is a fixed vector of dimension n and x_z is an
 * arbitrary vector of dimension n-m. Then, the problem of minimizing f(x)
 * subject to Ax=b is equivalent to minimizing f(c + Z*x_z) with no constraints.
 * The computed Y and Z are such that any solution of Ax=b can be written as
 * x=Y*x_y + Z*x_z for some x_y, x_z. Furthermore, A*Y is nonsingular, A*Z=0
 * and Z spans the null space of A.
 *
 * The function accepts A, b and computes c, Y, Z. If b or c is NULL, c is not
 * computed. Also, Y can be NULL in which case it is not referenced.
 * The function returns 0 in case of error, A's computed rank if successfull
 *
 */
static int LMLEC_ELIM(LM_REAL *A, LM_REAL *b, LM_REAL *c, LM_REAL *Y, LM_REAL *Z, int m, int n)
{
static LM_REAL eps=LM_CNST(-1.0);

LM_REAL *buf=NULL;
LM_REAL *a, *tau, *work, *r, aux;
register LM_REAL tmp;
int a_sz, jpvt_sz, tau_sz, r_sz, Y_sz, worksz;
int info, rank, *jpvt, tot_sz, mintmn, tm, tn;
register int i, j, k;

  if(m>n){
    fprintf(stderr, RCAT("matrix of constraints cannot have more rows than columns in", LMLEC_ELIM) "()!\n");
    return LM_ERROR;
  }

  tm=n; tn=m; // transpose dimensions
  mintmn=m;

  /* calculate required memory size */
  worksz=-1; // workspace query. Optimal work size is returned in aux
  //ORGQR((int *)&tm, (int *)&tm, (int *)&mintmn, NULL, (int *)&tm, NULL, (LM_REAL *)&aux, &worksz, &info);
  GEQP3((int *)&tm, (int *)&tn, NULL, (int *)&tm, NULL, NULL, (LM_REAL *)&aux, (int *)&worksz, &info);
  worksz=(int)aux;
  a_sz=tm*tm; // tm*tn is enough for xgeqp3()
  jpvt_sz=tn;
  tau_sz=mintmn;
  r_sz=mintmn*mintmn; // actually smaller if a is not of full row rank
  Y_sz=(Y)? 0 : tm*tn;

  tot_sz=jpvt_sz*sizeof(int) + (a_sz + tau_sz + r_sz + worksz + Y_sz)*sizeof(LM_REAL);
  buf=(LM_REAL *)malloc(tot_sz); /* allocate a "big" memory chunk at once */
  if(!buf){
    fprintf(stderr, RCAT("Memory allocation request failed in ", LMLEC_ELIM) "()\n");
    exit(1);
  }

  a=(LM_REAL *)buf;
  jpvt=(int *)(a+a_sz);
  tau=(LM_REAL *)(jpvt + jpvt_sz);
  r=tau+tau_sz;
  work=r+r_sz;
  if(!Y) Y=work+worksz;

  /* copy input array so that LAPACK won't destroy it. Note that copying is
   * done in row-major order, which equals A^T in column-major
   */
  for(i=0; i<tm*tn; ++i)
      a[i]=A[i];

  /* clear jpvt */
  for(i=0; i<jpvt_sz; ++i) jpvt[i]=0;

  /* rank revealing QR decomposition of A^T*/
  GEQP3((int *)&tm, (int *)&tn, a, (int *)&tm, jpvt, tau, work, (int *)&worksz, &info);
  //dgeqpf_((int *)&tm, (int *)&tn, a, (int *)&tm, jpvt, tau, work, &info);
  /* error checking */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", GEQP3) " in ", LMLEC_ELIM) "()\n", -info);
      exit(1);
    }
    else if(info>0){
      fprintf(stderr, RCAT(RCAT("unknown LAPACK error (%d) for ", GEQP3) " in ", LMLEC_ELIM) "()\n", info);
      free(buf);
      return 0;
    }
  }
  /* the upper triangular part of a now contains the upper triangle of the unpermuted R */

  if(eps<0.0){
    LM_REAL aux;

    /* compute machine epsilon. DBL_EPSILON should do also */
    for(eps=LM_CNST(1.0); aux=eps+LM_CNST(1.0), aux-LM_CNST(1.0)>0.0; eps*=LM_CNST(0.5))
                              ;
    eps*=LM_CNST(2.0);
  }

  tmp=tm*LM_CNST(10.0)*eps*FABS(a[0]); // threshold. tm is max(tm, tn)
  tmp=(tmp>LM_CNST(1E-12))? tmp : LM_CNST(1E-12); // ensure that threshold is not too small
  /* compute A^T's numerical rank by counting the non-zeros in R's diagonal */
  for(i=rank=0; i<mintmn; ++i)
    if(a[i*(tm+1)]>tmp || a[i*(tm+1)]<-tmp) ++rank; /* loop across R's diagonal elements */
    else break; /* diagonal is arranged in absolute decreasing order */

  if(rank<tn){
    fprintf(stderr, RCAT("\nConstraints matrix in ",  LMLEC_ELIM) "() is not of full row rank (i.e. %d < %d)!\n"
            "Make sure that you do not specify redundant or inconsistent constraints.\n\n", rank, tn);
    //exit(1);
    free(buf);
    return LM_ERROR;
  }

  /* compute the permuted inverse transpose of R */
  /* first, copy R from the upper triangular part of a to r. R is rank x rank */
  for(j=0; j<rank; ++j){
    for(i=0; i<=j; ++i)
      r[i+j*rank]=a[i+j*tm];
    for(i=j+1; i<rank; ++i)
      r[i+j*rank]=0.0; // lower part is zero
  }

  /* compute the inverse */
  TRTRI("U", "N", (int *)&rank, r, (int *)&rank, &info);
  /* error checking */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", TRTRI) " in ", LMLEC_ELIM) "()\n", -info);
      exit(1);
    }
    else if(info>0){
      fprintf(stderr, RCAT(RCAT("A(%d, %d) is exactly zero for ", TRTRI) " (singular matrix) in ", LMLEC_ELIM) "()\n", info, info);
      free(buf);
      return 0;
    }
  }
  /* then, transpose r in place */
  for(i=0; i<rank; ++i)
    for(j=i+1; j<rank; ++j){
      tmp=r[i+j*rank];
      k=j+i*rank;
      r[i+j*rank]=r[k];
      r[k]=tmp;
  }

  /* finally, permute R^-T using Y as intermediate storage */
  for(j=0; j<rank; ++j)
    for(i=0, k=jpvt[j]-1; i<rank; ++i)
      Y[i+k*rank]=r[i+j*rank];

  for(i=0; i<rank*rank; ++i) // copy back to r
    r[i]=Y[i];

  /* resize a to be tm x tm, filling with zeroes */
  for(i=tm*tn; i<tm*tm; ++i)
    a[i]=0.0;

  /* compute Q in a as the product of elementary reflectors. Q is tm x tm */
  ORGQR((int *)&tm, (int *)&tm, (int *)&mintmn, a, (int *)&tm, tau, work, &worksz, &info);
  /* error checking */
  if(info!=0){
    if(info<0){
      fprintf(stderr, RCAT(RCAT("LAPACK error: illegal value for argument %d of ", ORGQR) " in ", LMLEC_ELIM) "()\n", -info);
      exit(1);
    }
    else if(info>0){
      fprintf(stderr, RCAT(RCAT("unknown LAPACK error (%d) for ", ORGQR) " in ", LMLEC_ELIM) "()\n", info);
      free(buf);
      return 0;
    }
  }

  /* compute Y=Q_1*R^-T*P^T. Y is tm x rank */
  for(i=0; i<tm; ++i)
    for(j=0; j<rank; ++j){
      for(k=0, tmp=0.0; k<rank; ++k)
        tmp+=a[i+k*tm]*r[k+j*rank];
      Y[i*rank+j]=tmp;
    }

  if(b && c){
    /* compute c=Y*b */
    for(i=0; i<tm; ++i){
      for(j=0, tmp=0.0; j<rank; ++j)
        tmp+=Y[i*rank+j]*b[j];

      c[i]=tmp;
    }
  }

  /* copy Q_2 into Z. Z is tm x (tm-rank) */
  for(j=0; j<tm-rank; ++j)
    for(i=0, k=j+rank; i<tm; ++i)
      Z[i*(tm-rank)+j]=a[i+k*tm];

  free(buf);

  return rank;
}

/* constrained measurements: given pp, compute the measurements at c + Z*pp */
static void LMLEC_FUNC(LM_REAL *pp, LM_REAL *hx, int mm, int n, void *adata)
{
struct LMLEC_DATA *data=(struct LMLEC_DATA *)adata;
int m;
register int i, j;
register LM_REAL sum;
LM_REAL *c, *Z, *p, *Zimm;

  m=mm+data->ncnstr;
  c=data->c;
  Z=data->Z;
  p=data->p;
  /* p=c + Z*pp */
  for(i=0; i<m; ++i){
    Zimm=Z+i*mm;
    for(j=0, sum=c[i]; j<mm; ++j)
      sum+=Zimm[j]*pp[j]; // sum+=Z[i*mm+j]*pp[j];
    p[i]=sum;
  }

  (*(data->func))(p, hx, m, n, data->adata);
}

/* constrained Jacobian: given pp, compute the Jacobian at c + Z*pp
 * Using the chain rule, the Jacobian with respect to pp equals the
 * product of the Jacobian with respect to p (at c + Z*pp) times Z
 */
static void LMLEC_JACF(LM_REAL *pp, LM_REAL *jacjac, int mm, int n, void *adata)
{
struct LMLEC_DATA *data=(struct LMLEC_DATA *)adata;
int m;
register int i, j, l;
register LM_REAL sum, *aux1, *aux2;
LM_REAL *c, *Z, *p, *jac; 

  m=mm+data->ncnstr;
  c=data->c;
  Z=data->Z;
  p=data->p;
  jac=data->jac;
  /* p=c + Z*pp */
  for(i=0; i<m; ++i){
    aux1=Z+i*mm;
    for(j=0, sum=c[i]; j<mm; ++j)
      sum+=aux1[j]*pp[j]; // sum+=Z[i*mm+j]*pp[j];
    p[i]=sum;
  }

  (*(data->jacf))(p, jac, m, n, data->adata);

  /* compute jac*Z in jacjac */
  if(n*m<=__BLOCKSZ__SQ){ // this is a small problem
    /* This is the straightforward way to compute jac*Z. However, due to
     * its noncontinuous memory access pattern, it incures many cache misses when
     * applied to large minimization problems (i.e. problems involving a large
     * number of free variables and measurements), in which jac is too large to
     * fit in the L1 cache. For such problems, a cache-efficient blocking scheme
     * is preferable. On the other hand, the straightforward algorithm is faster
     * on small problems since in this case it avoids the overheads of blocking.
     */

    for(i=0; i<n; ++i){
      aux1=jac+i*m;
      aux2=jacjac+i*mm;
      for(j=0; j<mm; ++j){
        for(l=0, sum=0.0; l<m; ++l)
          sum+=aux1[l]*Z[l*mm+j]; // sum+=jac[i*m+l]*Z[l*mm+j];

        aux2[j]=sum; // jacjac[i*mm+j]=sum;
      }
    }
  }
  else{ // this is a large problem
    /* Cache efficient computation of jac*Z based on blocking
     */
#define __MIN__(x, y) (((x)<=(y))? (x) : (y))
    register int jj, ll;

    for(jj=0; jj<mm; jj+=__BLOCKSZ__){
      for(i=0; i<n; ++i){
        aux1=jacjac+i*mm;
        for(j=jj; j<__MIN__(jj+__BLOCKSZ__, mm); ++j)
          aux1[j]=0.0; //jacjac[i*mm+j]=0.0;
      }

      for(ll=0; ll<m; ll+=__BLOCKSZ__){
        for(i=0; i<n; ++i){
          aux1=jacjac+i*mm; aux2=jac+i*m;
          for(j=jj; j<__MIN__(jj+__BLOCKSZ__, mm); ++j){
            sum=0.0;
            for(l=ll; l<__MIN__(ll+__BLOCKSZ__, m); ++l)
              sum+=aux2[l]*Z[l*mm+j]; //jac[i*m+l]*Z[l*mm+j];
            aux1[j]+=sum; //jacjac[i*mm+j]+=sum;
          }
        }
      }
    }
  }
}
#undef __MIN__


/* 
 * This function is similar to LEVMAR_DER except that the minimization
 * is performed subject to the linear constraints A p=b, A is kxm, b kx1
 *
 * This function requires an analytic Jacobian. In case the latter is unavailable,
 * use LEVMAR_LEC_DIF() bellow
 *
 */
int LEVMAR_LEC_DER(
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata), /* functional relation describing measurements. A p \in R^m yields a \hat{x} \in  R^n */
  void (*jacf)(LM_REAL *p, LM_REAL *j, int m, int n, void *adata),  /* function to evaluate the Jacobian \part x / \part p */ 
  LM_REAL *p,         /* I/O: initial parameter estimates. On output has the estimated solution */
  LM_REAL *x,         /* I: measurement vector. NULL implies a zero vector */
  int m,              /* I: parameter vector dimension (i.e. #unknowns) */
  int n,              /* I: measurement vector dimension */
  LM_REAL *A,         /* I: constraints matrix, kxm */
  LM_REAL *b,         /* I: right hand constraints vector, kx1 */
  int k,              /* I: number of constraints (i.e. A's #rows) */
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
  LM_REAL *work,     /* working memory at least LM_LEC_DER_WORKSZ() reals large, allocated if NULL */
  LM_REAL *covar,    /* O: Covariance matrix corresponding to LS solution; mxm. Set to NULL if not needed. */
  void *adata)       /* pointer to possibly additional data, passed uninterpreted to func & jacf.
                      * Set to NULL if not needed
                      */
{
  struct LMLEC_DATA data;
  LM_REAL *ptr, *Z, *pp, *p0, *Zimm; /* Z is mxmm */
  int mm, ret;
  register int i, j;
  register LM_REAL tmp;
  LM_REAL locinfo[LM_INFO_SZ];

  if(!jacf){
    fprintf(stderr, RCAT("No function specified for computing the Jacobian in ", LEVMAR_LEC_DER)
      RCAT("().\nIf no such function is available, use ", LEVMAR_LEC_DIF) RCAT("() rather than ", LEVMAR_LEC_DER) "()\n");
    return LM_ERROR;
  }

  mm=m-k;

  if(n<mm){
    fprintf(stderr, LCAT(LEVMAR_LEC_DER, "(): cannot solve a problem with fewer measurements + constraints [%d + %d] than unknowns [%d]\n"), n, k, m);
    return LM_ERROR;
  }

  ptr=(LM_REAL *)malloc((2*m + m*mm + n*m + mm)*sizeof(LM_REAL));
  if(!ptr){
    fprintf(stderr, LCAT(LEVMAR_LEC_DER, "(): memory allocation request failed\n"));
    exit(1);
  }
  data.p=p;
  p0=ptr;
  data.c=p0+m;
  data.Z=Z=data.c+m;
  data.jac=data.Z+m*mm;
  pp=data.jac+n*m;
  data.ncnstr=k;
  data.func=func;
  data.jacf=jacf;
  data.adata=adata;

  ret=LMLEC_ELIM(A, b, data.c, NULL, Z, k, m); // compute c, Z
  if(ret==LM_ERROR){
    free(ptr);
    return LM_ERROR;
  }

  /* compute pp s.t. p = c + Z*pp or (Z^T Z)*pp=Z^T*(p-c)
   * Due to orthogonality, Z^T Z = I and the last equation
   * becomes pp=Z^T*(p-c). Also, save the starting p in p0
   */
  for(i=0; i<m; ++i){
    p0[i]=p[i];
    p[i]-=data.c[i];
  }

  /* Z^T*(p-c) */
  for(i=0; i<mm; ++i){
    for(j=0, tmp=0.0; j<m; ++j)
      tmp+=Z[j*mm+i]*p[j];
    pp[i]=tmp;
  }

  /* compute the p corresponding to pp (i.e. c + Z*pp) and compare with p0 */
  for(i=0; i<m; ++i){
    Zimm=Z+i*mm;
    for(j=0, tmp=data.c[i]; j<mm; ++j)
      tmp+=Zimm[j]*pp[j]; // tmp+=Z[i*mm+j]*pp[j];
    if(FABS(tmp-p0[i])>LM_CNST(1E-03))
      fprintf(stderr, RCAT("Warning: component %d of starting point not feasible in ", LEVMAR_LEC_DER) "()! [%.10g reset to %.10g]\n",
                      i, p0[i], tmp);
  }

  if(!info) info=locinfo; /* make sure that LEVMAR_DER() is called with non-null info */
  /* note that covariance computation is not requested from LEVMAR_DER() */
  ret=LEVMAR_DER(LMLEC_FUNC, LMLEC_JACF, pp, x, mm, n, itmax, opts, info, work, NULL, (void *)&data);

  /* p=c + Z*pp */
  for(i=0; i<m; ++i){
    Zimm=Z+i*mm;
    for(j=0, tmp=data.c[i]; j<mm; ++j)
      tmp+=Zimm[j]*pp[j]; // tmp+=Z[i*mm+j]*pp[j];
    p[i]=tmp;
  }

  /* compute the covariance from the Jacobian in data.jac */
  if(covar){
    LEVMAR_TRANS_MAT_MAT_MULT(data.jac, covar, n, m); /* covar = J^T J */
    LEVMAR_COVAR(covar, covar, info[1], m, n);
  }

  free(ptr);

  return ret;
}

/* Similar to the LEVMAR_LEC_DER() function above, except that the Jacobian is approximated
 * with the aid of finite differences (forward or central, see the comment for the opts argument)
 */
int LEVMAR_LEC_DIF(
  void (*func)(LM_REAL *p, LM_REAL *hx, int m, int n, void *adata), /* functional relation describing measurements. A p \in R^m yields a \hat{x} \in  R^n */
  LM_REAL *p,         /* I/O: initial parameter estimates. On output has the estimated solution */
  LM_REAL *x,         /* I: measurement vector. NULL implies a zero vector */
  int m,              /* I: parameter vector dimension (i.e. #unknowns) */
  int n,              /* I: measurement vector dimension */
  LM_REAL *A,         /* I: constraints matrix, kxm */
  LM_REAL *b,         /* I: right hand constraints vector, kx1 */
  int k,              /* I: number of constraints (i.e. A's #rows) */
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
  LM_REAL *work,     /* working memory at least LM_LEC_DIF_WORKSZ() reals large, allocated if NULL */
  LM_REAL *covar,    /* O: Covariance matrix corresponding to LS solution; mxm. Set to NULL if not needed. */
  void *adata)       /* pointer to possibly additional data, passed uninterpreted to func.
                      * Set to NULL if not needed
                      */
{
  struct LMLEC_DATA data;
  LM_REAL *ptr, *Z, *pp, *p0, *Zimm; /* Z is mxmm */
  int mm, ret;
  register int i, j;
  register LM_REAL tmp;
  LM_REAL locinfo[LM_INFO_SZ];

  mm=m-k;

  if(n<mm){
    fprintf(stderr, LCAT(LEVMAR_LEC_DIF, "(): cannot solve a problem with fewer measurements + constraints [%d + %d] than unknowns [%d]\n"), n, k, m);
    return LM_ERROR;
  }

  ptr=(LM_REAL *)malloc((2*m + m*mm + mm)*sizeof(LM_REAL));
  if(!ptr){
    fprintf(stderr, LCAT(LEVMAR_LEC_DIF, "(): memory allocation request failed\n"));
    exit(1);
  }
  data.p=p;
  p0=ptr;
  data.c=p0+m;
  data.Z=Z=data.c+m;
  data.jac=NULL;
  pp=data.Z+m*mm;
  data.ncnstr=k;
  data.func=func;
  data.jacf=NULL;
  data.adata=adata;

  ret=LMLEC_ELIM(A, b, data.c, NULL, Z, k, m); // compute c, Z
  if(ret==LM_ERROR){
    free(ptr);
    return LM_ERROR;
  }

  /* compute pp s.t. p = c + Z*pp or (Z^T Z)*pp=Z^T*(p-c)
   * Due to orthogonality, Z^T Z = I and the last equation
   * becomes pp=Z^T*(p-c). Also, save the starting p in p0
   */
  for(i=0; i<m; ++i){
    p0[i]=p[i];
    p[i]-=data.c[i];
  }

  /* Z^T*(p-c) */
  for(i=0; i<mm; ++i){
    for(j=0, tmp=0.0; j<m; ++j)
      tmp+=Z[j*mm+i]*p[j];
    pp[i]=tmp;
  }

  /* compute the p corresponding to pp (i.e. c + Z*pp) and compare with p0 */
  for(i=0; i<m; ++i){
    Zimm=Z+i*mm;
    for(j=0, tmp=data.c[i]; j<mm; ++j)
      tmp+=Zimm[j]*pp[j]; // tmp+=Z[i*mm+j]*pp[j];
    if(FABS(tmp-p0[i])>LM_CNST(1E-03))
      fprintf(stderr, RCAT("Warning: component %d of starting point not feasible in ", LEVMAR_LEC_DIF) "()! [%.10g reset to %.10g]\n",
                      i, p0[i], tmp);
  }

  if(!info) info=locinfo; /* make sure that LEVMAR_DIF() is called with non-null info */
  /* note that covariance computation is not requested from LEVMAR_DIF() */
  ret=LEVMAR_DIF(LMLEC_FUNC, pp, x, mm, n, itmax, opts, info, work, NULL, (void *)&data);

  /* p=c + Z*pp */
  for(i=0; i<m; ++i){
    Zimm=Z+i*mm;
    for(j=0, tmp=data.c[i]; j<mm; ++j)
      tmp+=Zimm[j]*pp[j]; // tmp+=Z[i*mm+j]*pp[j];
    p[i]=tmp;
  }

  /* compute the Jacobian with finite differences and use it to estimate the covariance */
  if(covar){
    LM_REAL *hx, *wrk, *jac;

    hx=(LM_REAL *)malloc((2*n+n*m)*sizeof(LM_REAL));
    if(!hx){
      fprintf(stderr, LCAT(LEVMAR_LEC_DIF, "(): memory allocation request failed\n"));
      exit(1);
    }

    wrk=hx+n;
    jac=wrk+n;

    (*func)(p, hx, m, n, adata); /* evaluate function at p */
    LEVMAR_FDIF_FORW_JAC_APPROX(func, p, hx, wrk, (LM_REAL)LM_DIFF_DELTA, jac, m, n, adata); /* compute the Jacobian at p */
    LEVMAR_TRANS_MAT_MAT_MULT(jac, covar, n, m); /* covar = J^T J */
    LEVMAR_COVAR(covar, covar, info[1], m, n);
    free(hx);
  }

  free(ptr);

  return ret;
}

/* undefine all. THIS MUST REMAIN AT THE END OF THE FILE */
#undef LMLEC_DATA
#undef LMLEC_ELIM
#undef LMLEC_FUNC
#undef LMLEC_JACF
#undef LEVMAR_FDIF_FORW_JAC_APPROX
#undef LEVMAR_COVAR
#undef LEVMAR_TRANS_MAT_MAT_MULT
#undef LEVMAR_LEC_DER
#undef LEVMAR_LEC_DIF
#undef LEVMAR_DER
#undef LEVMAR_DIF

#undef GEQP3
#undef ORGQR
#undef TRTRI
