/* ////////////////////////////////////////////////////////////////////////////////
// 
//  Matlab MEX file for the Levenberg - Marquardt minimization algorithm
//  Copyright (C) 2007  Manolis Lourakis (lourakis at ics forth gr)
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
//////////////////////////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include <lm.h>

#include <mex.h>

/**
#define DEBUG
**/

#ifndef HAVE_LAPACK
#ifdef _MSC_VER
#pragma message("LAPACK not available, certain functionalities cannot be compiled!")
#else
#warning LAPACK not available, certain functionalities cannot be compiled
#endif /* _MSC_VER */
#endif /* HAVE_LAPACK */

#define __MAX__(A, B)     ((A)>=(B)? (A) : (B))

#define MIN_UNCONSTRAINED     0
#define MIN_CONSTRAINED_BC    1
#define MIN_CONSTRAINED_LEC   2
#define MIN_CONSTRAINED_BLEC  3

struct mexdata {
  /* matlab names of the fitting function & its Jacobian */
  char *fname, *jacname;

  /* binary flags specifying if input p0 is a row or column vector */
  int isrow_p0;

  /* rhs args to be passed to matlab. rhs[0] is reserved for
   * passing the parameter vector. If present, problem-specific
   * data are passed in rhs[1], rhs[2], etc
   */
  mxArray **rhs;
  int nrhs; /* >= 1 */
};

/* display printf-style error messages in matlab */
static void matlabFmtdErrMsgTxt(char *fmt, ...)
{
char  buf[256];
va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

  mexErrMsgTxt(buf);
}

/* display printf-style warning messages in matlab */
static void matlabFmtdWarnMsgTxt(char *fmt, ...)
{
char  buf[256];
va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

  mexWarnMsgTxt(buf);
}

static void func(double *p, double *hx, int m, int n, void *adata)
{
mxArray *lhs[1];
double *mp, *mx;
register int i;
struct mexdata *dat=(struct mexdata *)adata;
    
  /* prepare to call matlab */
  mp=mxGetPr(dat->rhs[0]);
  for(i=0; i<m; ++i)
    mp[i]=p[i];
    
  /* invoke matlab */
  mexCallMATLAB(1, lhs, dat->nrhs, dat->rhs, dat->fname);

  /* copy back results & cleanup */
  mx=mxGetPr(lhs[0]);
  for(i=0; i<n; ++i)
    hx[i]=mx[i];

  /* delete the matrix created by matlab */
  mxDestroyArray(lhs[0]);
}

static void jacfunc(double *p, double *j, int m, int n, void *adata)
{
mxArray *lhs[1];
double *mp;
double *mj;
register int i, k;
struct mexdata *dat=(struct mexdata *)adata;
    
  /* prepare to call matlab */
  mp=mxGetPr(dat->rhs[0]);
  for(i=0; i<m; ++i)
    mp[i]=p[i];

  /* invoke matlab */
  mexCallMATLAB(1, lhs, dat->nrhs, dat->rhs, dat->jacname);
    
  /* copy back results & cleanup. Note that the nxm Jacobian 
   * computed by matlab should be transposed so that
   * levmar gets it in row major, as expected
   */
  mj=mxGetPr(lhs[0]);
  for(i=0; i<n; ++i)
    for(k=0; k<m; ++k)
      j[i*m+k]=mj[i+k*n];

  /* delete the matrix created by matlab */
  mxDestroyArray(lhs[0]);
}

/* matlab matrices are in column-major, this routine converts them to row major for levmar */
static double *getTranspose(mxArray *Am)
{
int m, n;
double *At, *A;
register int i, j;

  m=mxGetM(Am);
  n=mxGetN(Am);
  A=mxGetPr(Am);
  At=mxMalloc(m*n*sizeof(double));

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      At[i*n+j]=A[i+j*m];
  
  return At;
}

/* check the supplied matlab function and its Jacobian. Returns 1 on error, 0 otherwise */
static int checkFuncAndJacobian(double *p, int  m, int n, int havejac, struct mexdata *dat)
{
mxArray *lhs[1];
register int i;
int ret=0;
double *mp;

  mexSetTrapFlag(1); /* handle errors in the MEX-file */

  mp=mxGetPr(dat->rhs[0]);
  for(i=0; i<m; ++i)
    mp[i]=p[i];

  /* attempt to call the supplied func */
  i=mexCallMATLAB(1, lhs, dat->nrhs, dat->rhs, dat->fname);
  if(i){
    fprintf(stderr, "levmar: error calling '%s'.\n", dat->fname);
    ret=1;
  }
  else if(!mxIsDouble(lhs[0]) || mxIsComplex(lhs[0]) || !(mxGetM(lhs[0])==1 || mxGetN(lhs[0])==1) ||
      __MAX__(mxGetM(lhs[0]), mxGetN(lhs[0]))!=n){
    fprintf(stderr, "levmar: '%s' should produce a real vector with %d elements (got %d).\n",
                    dat->fname, m, __MAX__(mxGetM(lhs[0]), mxGetN(lhs[0])));
    ret=1;
  }
  /* delete the matrix created by matlab */
  mxDestroyArray(lhs[0]);

  if(havejac){
    /* attempt to call the supplied jac  */
    i=mexCallMATLAB(1, lhs, dat->nrhs, dat->rhs, dat->jacname);
    if(i){
      fprintf(stderr, "levmar: error calling '%s'.\n", dat->jacname);
      ret=1;
    }
    else if(!mxIsDouble(lhs[0]) || mxIsComplex(lhs[0]) || mxGetM(lhs[0])!=n || mxGetN(lhs[0])!=m){
      fprintf(stderr, "levmar: '%s' should produce a real %dx%d matrix (got %dx%d).\n",
                      dat->jacname, n, m, mxGetM(lhs[0]), mxGetN(lhs[0]));
      ret=1;
    }
    else if(mxIsSparse(lhs[0])){
      fprintf(stderr, "levmar: '%s' should produce a real dense matrix (got a sparse one).\n", dat->jacname);
      ret=1;
    }
    /* delete the matrix created by matlab */
    mxDestroyArray(lhs[0]);
  }

  mexSetTrapFlag(0); /* on error terminate the MEX-file and return control to the MATLAB prompt */

  return ret;
}


/*
[ret, p, info, covar]=levmar_der (f, j, p0, x, itmax, opts, 'unc'                        ...)
[ret, p, info, covar]=levmar_bc  (f, j, p0, x, itmax, opts, 'bc',   lb, ub,              ...)
[ret, p, info, covar]=levmar_lec (f, j, p0, x, itmax, opts, 'lec',          A, b,        ...)
[ret, p, info, covar]=levmar_blec(f, j, p0, x, itmax, opts, 'blec', lb, ub, A, b, wghts, ...)
*/

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *Prhs[])
{
register int i;
register double *pdbl;
mxArray **prhs=(mxArray **)&Prhs[0], *At;
struct mexdata mdata;
int len, status;
double *p, *p0, *ret, *x;
int m, n, havejac, Arows, itmax, nopts, mintype, nextra;
double opts[LM_OPTS_SZ]={LM_INIT_MU, LM_STOP_THRESH, LM_STOP_THRESH, LM_STOP_THRESH, LM_DIFF_DELTA};
double info[LM_INFO_SZ];
double *lb=NULL, *ub=NULL, *A=NULL, *b=NULL, *wghts=NULL, *covar=NULL;

  /* parse input args; start by checking their number */
  if((nrhs<5))
    matlabFmtdErrMsgTxt("levmar: at least 5 input arguments required (got %d).", nrhs);
  if(nlhs>4)
    matlabFmtdErrMsgTxt("levmar: too many output arguments (max. 4, got %d).", nlhs);
  else if(nlhs<2)
    matlabFmtdErrMsgTxt("levmar: too few output arguments (min. 2, got %d).", nlhs);
    
  /* note that in order to accommodate optional args, prhs & nrhs are adjusted accordingly below */

  /** func **/
  /* first argument must be a string , i.e. a char row vector */
  if(mxIsChar(prhs[0])!=1)
    mexErrMsgTxt("levmar: first argument must be a string.");
  if(mxGetM(prhs[0])!=1)
    mexErrMsgTxt("levmar: first argument must be a string (i.e. char row vector).");
  /* store supplied name */
  len=mxGetN(prhs[0])+1;
  mdata.fname=mxCalloc(len, sizeof(char));
  status=mxGetString(prhs[0], mdata.fname, len);
  if(status!=0)
    mexErrMsgTxt("levmar: not enough space. String is truncated.");

  /** jac (optional) **/
  /* check whether second argument is a string */
  if(mxIsChar(prhs[1])==1){
    if(mxGetM(prhs[1])!=1)
      mexErrMsgTxt("levmar: second argument must be a string (i.e. row vector).");
    /* store supplied name */
    len=mxGetN(prhs[1])+1;
    mdata.jacname=mxCalloc(len, sizeof(char));
    status=mxGetString(prhs[1], mdata.jacname, len);
    if(status!=0)
      mexErrMsgTxt("levmar: not enough space. String is truncated.");
    havejac=1;

    ++prhs;
    --nrhs;
  }
  else{
    mdata.jacname=NULL;
    havejac=0;
  }

#ifdef DEBUG
  fflush(stderr);
  fprintf(stderr, "LEVMAR: %s analytic Jacobian\n", havejac? "with" : "no");
#endif /* DEBUG */

/* CHECK 
if(!mxIsDouble(prhs[1]) || mxIsComplex(prhs[1]) || !(mxGetM(prhs[1])==1 && mxGetN(prhs[1])==1))
*/

  /** p0 **/
  /* the second required argument must be a real row or column vector */
  if(!mxIsDouble(prhs[1]) || mxIsComplex(prhs[1]) || !(mxGetM(prhs[1])==1 || mxGetN(prhs[1])==1))
    mexErrMsgTxt("levmar: p0 must be a real vector.");
  p0=mxGetPr(prhs[1]);
  /* determine if we have a row or column vector and retrieve its 
   * size, i.e. the number of parameters
   */
  if(mxGetM(prhs[1])==1){
    m=mxGetN(prhs[1]);
    mdata.isrow_p0=1;
  }
  else{
    m=mxGetM(prhs[1]);
    mdata.isrow_p0=0;
  }
  /* copy input parameter vector to avoid destroying it */
  p=mxMalloc(m*sizeof(double));
  for(i=0; i<m; ++i)
    p[i]=p0[i];
    
  /** x **/
  /* the third required argument must be a real row or column vector */
  if(!mxIsDouble(prhs[2]) || mxIsComplex(prhs[2]) || !(mxGetM(prhs[2])==1 || mxGetN(prhs[2])==1))
    mexErrMsgTxt("levmar: x must be a real vector.");
  x=mxGetPr(prhs[2]);
  n=__MAX__(mxGetM(prhs[2]), mxGetN(prhs[2]));

  /** itmax **/
  /* the fourth required argument must be a scalar */
  if(!mxIsDouble(prhs[3]) || mxIsComplex(prhs[3]) || mxGetM(prhs[3])!=1 || mxGetN(prhs[3])!=1)
    mexErrMsgTxt("levmar: itmax must be a scalar.");
  itmax=(int)mxGetScalar(prhs[3]);
    
  /** opts **/
  /* the fifth required argument must be a real row or column vector */
  if(!mxIsDouble(prhs[4]) || mxIsComplex(prhs[4]) || (!(mxGetM(prhs[4])==1 || mxGetN(prhs[4])==1) &&
                                                      !(mxGetM(prhs[4])==0 && mxGetN(prhs[4])==0)))
    mexErrMsgTxt("levmar: opts must be a real vector.");
  pdbl=mxGetPr(prhs[4]);
  nopts=__MAX__(mxGetM(prhs[4]), mxGetN(prhs[4]));
  if(nopts!=0){ /* if opts==[], nothing needs to be done and the defaults are used */
    if(nopts>LM_OPTS_SZ)
      matlabFmtdErrMsgTxt("levmar: opts must have at most %d elements, got %d.", LM_OPTS_SZ, nopts);
    else if(nopts<((havejac)? LM_OPTS_SZ-1 : LM_OPTS_SZ))
      matlabFmtdWarnMsgTxt("levmar: only the %d first elements of opts specified, remaining set to defaults.", nopts);
    for(i=0; i<nopts; ++i)
      opts[i]=pdbl[i];
  }
#ifdef DEBUG
  else{
    fflush(stderr);
    fprintf(stderr, "LEVMAR: empty options vector, using defaults\n");
  }
#endif /* DEBUG */

  /** mintype (optional) **/
  /* check whether sixth argument is a string */
  if(nrhs>=6 && mxIsChar(prhs[5])==1 && mxGetM(prhs[5])==1){
    char *minhowto;

    /* examine supplied name */
    len=mxGetN(prhs[5])+1;
    minhowto=mxCalloc(len, sizeof(char));
    status=mxGetString(prhs[5], minhowto, len);
    if(status!=0)
      mexErrMsgTxt("levmar: not enough space. String is truncated.");

    for(i=0; minhowto[i]; ++i)
      minhowto[i]=tolower(minhowto[i]);
    if(!strncmp(minhowto, "unc", 3)) mintype=MIN_UNCONSTRAINED;
    else if(!strncmp(minhowto, "bc", 2)) mintype=MIN_CONSTRAINED_BC;
    else if(!strncmp(minhowto, "lec", 3)) mintype=MIN_CONSTRAINED_LEC;
    else if(!strncmp(minhowto, "blec", 4)) mintype=MIN_CONSTRAINED_BLEC;
    else matlabFmtdErrMsgTxt("levmar: unknown minimization type '%s'.", minhowto);

    mxFree(minhowto);

    ++prhs;
    --nrhs;
  }
  else
    mintype=MIN_UNCONSTRAINED;

  if(mintype==MIN_UNCONSTRAINED) goto extraargs;

  /* arguments below this point are optional and their presence depends
   * upon the minimization type determined above
   */
  /** lb, ub **/
  if(nrhs>=7 && (mintype==MIN_CONSTRAINED_BC || mintype==MIN_CONSTRAINED_BLEC)){
    /* check if the next two arguments are real row or column vectors */
    if(mxIsDouble(prhs[5]) && !mxIsComplex(prhs[5]) && (mxGetM(prhs[5])==1 || mxGetN(prhs[5])==1)){
      if(mxIsDouble(prhs[6]) && !mxIsComplex(prhs[6]) && (mxGetM(prhs[6])==1 || mxGetN(prhs[6])==1)){
        if((i=__MAX__(mxGetM(prhs[5]), mxGetN(prhs[5])))!=m)
          matlabFmtdErrMsgTxt("levmar: lb must have %d elements, got %d.", m, i);
        if((i=__MAX__(mxGetM(prhs[6]), mxGetN(prhs[6])))!=m)
          matlabFmtdErrMsgTxt("levmar: ub must have %d elements, got %d.", m, i);

        lb=mxGetPr(prhs[5]);
        ub=mxGetPr(prhs[6]);

        prhs+=2;
        nrhs-=2;
      }
    }
  }

  /** A, b **/
  if(nrhs>=7 && (mintype==MIN_CONSTRAINED_LEC || mintype==MIN_CONSTRAINED_BLEC)){
    /* check if the next two arguments are a real matrix and a real row or column vector */
    if(mxIsDouble(prhs[5]) && !mxIsComplex(prhs[5]) && mxGetM(prhs[5])>=1 && mxGetN(prhs[5])>=1){
      if(mxIsDouble(prhs[6]) && !mxIsComplex(prhs[6]) && (mxGetM(prhs[6])==1 || mxGetN(prhs[6])==1)){
        if((i=mxGetN(prhs[5]))!=m)
          matlabFmtdErrMsgTxt("levmar: A must have %d columns, got %d.", m, i);
        if((i=__MAX__(mxGetM(prhs[6]), mxGetN(prhs[6])))!=(Arows=mxGetM(prhs[5])))
          matlabFmtdErrMsgTxt("levmar: b must have %d elements, got %d.", Arows, i);

        At=prhs[5];
        b=mxGetPr(prhs[6]);
        A=getTranspose(At);

        prhs+=2;
        nrhs-=2;
      }
    }
  }

  /* wghts */
  /* check if we have a weights vector */
  if(nrhs>=6 && mintype==MIN_CONSTRAINED_BLEC){ /* only check if we have seen both box & linear constraints */
    if(mxIsDouble(prhs[5]) && !mxIsComplex(prhs[5]) && (mxGetM(prhs[5])==1 || mxGetN(prhs[5])==1)){
      if(__MAX__(mxGetM(prhs[5]), mxGetN(prhs[5]))==m){
        wghts=mxGetPr(prhs[5]);

        ++prhs;
        --nrhs;
      }
    }
  }
  /* arguments below this point are assumed to be extra arguments passed
   * to every invocation of the fitting function and its Jacobian
   */

extraargs:
  /* handle any extra args and allocate memory for
   * passing the current parameter estimate to matlab
   */
  nextra=nrhs-5;
  mdata.nrhs=nextra+1;
  mdata.rhs=(mxArray **)mxMalloc(mdata.nrhs*sizeof(mxArray *));
  for(i=0; i<nextra; ++i)
    mdata.rhs[i+1]=(mxArray *)prhs[nrhs-nextra+i]; /* discard 'const' modifier */
#ifdef DEBUG
  fflush(stderr);
  fprintf(stderr, "LEVMAR: %d extra args\n", nextra);
#endif /* DEBUG */

  if(mdata.isrow_p0){ /* row vector */
    mdata.rhs[0]=mxCreateDoubleMatrix(1, m, mxREAL);
    /*
    mxSetM(mdata.rhs[0], 1);
    mxSetN(mdata.rhs[0], m);
    */
  }
  else{ /* column vector */
    mdata.rhs[0]=mxCreateDoubleMatrix(m, 1, mxREAL);
    /*
    mxSetM(mdata.rhs[0], m);
    mxSetN(mdata.rhs[0], 1);
    */
  }

  /* ensure that the supplied function & Jacobian are as expected */
  if(checkFuncAndJacobian(p, m, n, havejac, &mdata)){
    status=LM_ERROR;
    goto cleanup;
  }

  if(nlhs>3) /* covariance output required */
    covar=mxMalloc(m*m*sizeof(double));

  /* invoke levmar */
  if(!lb && !ub){
    if(!A && !b){ /* no constraints */
      if(havejac)
        status=dlevmar_der(func, jacfunc, p, x, m, n, itmax, opts, info, NULL, covar, (void *)&mdata);
      else
        status=dlevmar_dif(func,          p, x, m, n, itmax, opts, info, NULL, covar, (void *)&mdata);
#ifdef DEBUG
  fflush(stderr);
  fprintf(stderr, "LEVMAR: calling dlevmar_der()/dlevmar_dif()\n");
#endif /* DEBUG */
    }
    else{ /* linear constraints */
#ifdef HAVE_LAPACK
      if(havejac)
        status=dlevmar_lec_der(func, jacfunc, p, x, m, n, A, b, Arows, itmax, opts, info, NULL, covar, (void *)&mdata);
      else
        status=dlevmar_lec_dif(func,          p, x, m, n, A, b, Arows, itmax, opts, info, NULL, covar, (void *)&mdata);
#else
      mexErrMsgTxt("levmar: no linear constraints support, HAVE_LAPACK was not defined during MEX-file compilation.");
#endif /* HAVE_LAPACK */

#ifdef DEBUG
  fflush(stderr);
  fprintf(stderr, "LEVMAR: calling dlevmar_lec_der()/dlevmar_lec_dif()\n");
#endif /* DEBUG */
    }
  }
  else{
    if(!A && !b){ /* box constraints */
      if(havejac)
        status=dlevmar_bc_der(func, jacfunc, p, x, m, n, lb, ub, itmax, opts, info, NULL, covar, (void *)&mdata);
      else
        status=dlevmar_bc_dif(func,          p, x, m, n, lb, ub, itmax, opts, info, NULL, covar, (void *)&mdata);
#ifdef DEBUG
  fflush(stderr);
  fprintf(stderr, "LEVMAR: calling dlevmar_bc_der()/dlevmar_bc_dif()\n");
#endif /* DEBUG */
    }
    else{ /* box & linear constraints */
#ifdef HAVE_LAPACK
      if(havejac)
        status=dlevmar_blec_der(func, jacfunc, p, x, m, n, lb, ub, A, b, Arows, wghts, itmax, opts, info, NULL, covar, (void *)&mdata);
      else
        status=dlevmar_blec_dif(func,          p, x, m, n, lb, ub, A, b, Arows, wghts, itmax, opts, info, NULL, covar, (void *)&mdata);
#else
      mexErrMsgTxt("levmar: no box & linear constraints support, HAVE_LAPACK was not defined during MEX-file compilation.");
#endif /* HAVE_LAPACK */

#ifdef DEBUG
  fflush(stderr);
  fprintf(stderr, "LEVMAR: calling dlevmar_blec_der()/dlevmar_blec_dif()\n");
#endif /* DEBUG */
    }
  }
#ifdef DEBUG
  fflush(stderr);
  printf("LEVMAR: minimization returned %d in %g iter, reason %g\n\tSolution: ", status, info[5], info[6]);
  for(i=0; i<m; ++i)
    printf("%.7g ", p[i]);
  printf("\n\n\tMinimization info:\n\t");
  for(i=0; i<LM_INFO_SZ; ++i)
    printf("%g ", info[i]);
  printf("\n");
#endif /* DEBUG */

  /* copy back return results */
  /** ret **/
  plhs[0]=mxCreateDoubleMatrix(1, 1, mxREAL);
  ret=mxGetPr(plhs[0]);
  ret[0]=(double)status;

  /** popt **/
  plhs[1]=(mdata.isrow_p0==1)? mxCreateDoubleMatrix(1, m, mxREAL) : mxCreateDoubleMatrix(m, 1, mxREAL);
  pdbl=mxGetPr(plhs[1]);
  for(i=0; i<m; ++i)
    pdbl[i]=p[i];

  /** info **/
  if(nlhs>2){
    plhs[2]=mxCreateDoubleMatrix(1, LM_INFO_SZ, mxREAL);
    pdbl=mxGetPr(plhs[2]);
    for(i=0; i<LM_INFO_SZ; ++i)
      pdbl[i]=info[i];
  }

  /** covar **/
  if(nlhs>3){
    plhs[3]=mxCreateDoubleMatrix(m, m, mxREAL);
    pdbl=mxGetPr(plhs[3]);
    for(i=0; i<m*m; ++i) /* covariance matrices are symmetric, thus no need to transpose! */
      pdbl[i]=covar[i];
  }

cleanup:
  /* cleanup */
  mxDestroyArray(mdata.rhs[0]);
  if(A) mxFree(A);

  mxFree(mdata.fname);
  if(havejac) mxFree(mdata.jacname);
  mxFree(p);
  mxFree(mdata.rhs);
  if(covar) mxFree(covar);

  if(status==LM_ERROR)
    mexWarnMsgTxt("levmar: optimization returned with an error!");
}
