/*********************************************************************/
/*                                                                   */
/*             Optimized BLAS libraries                              */
/*                     By Kazushige Goto <kgoto@tacc.utexas.edu>     */
/*                                                                   */
/* Copyright (c) The University of Texas, 2005. All rights reserved. */
/* UNIVERSITY EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING  */
/* THIS SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF      */
/* MERCHANTABILITY, FITNESS FOR ANY PARTICULAR PURPOSE,              */
/* NON-INFRINGEMENT AND WARRANTIES OF PERFORMANCE, AND ANY WARRANTY  */
/* THAT MIGHT OTHERWISE ARISE FROM COURSE OF DEALING OR USAGE OF     */
/* TRADE. NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH RESPECT TO   */
/* THE USE OF THE SOFTWARE OR DOCUMENTATION.                         */
/* Under no circumstances shall University be liable for incidental, */
/* special, indirect, direct or consequential damages or loss of     */
/* profits, interruption of business, or related expenses which may  */
/* arise from use of Software or Documentation, including but not    */
/* limited to those resulting from defects in Software and/or        */
/* Documentation, or loss or inaccuracy of data of any kind.         */
/*********************************************************************/

#ifndef COMMON_REF_H
#define COMMON_REF_H
#include "common.h"

#ifndef ASSEMBLER

#ifndef FLOATRET
#ifndef F2CCONV
#define FLOATRET	float
#else
#define FLOATRET	double
#endif
#endif

FLOATRET  BLASFUNC(sdotf)  (blasint *, float  *, blasint *, float  *, blasint *);
FLOATRET  BLASFUNC(sdsdotf)(blasint *, float  *,        float  *, blasint *, float  *, blasint *);

#ifdef RETURN_BY_STACK
void  BLASFUNC(cdotuf)  (float *,  blasint *, float  * , blasint *, float  *,  blasint *);
void  BLASFUNC(cdotcf)  (float *,  blasint *, float  *,  blasint *, float  *,  blasint *);
void  BLASFUNC(zdotuf)  (double *, blasint *, double  *, blasint *, double  *, blasint *);
void  BLASFUNC(zdotcf)  (double *, blasint *, double  *, blasint *, double  *, blasint *);
void  BLASFUNC(xdotuf)  (xdouble *, blasint *, xdouble  *, blasint *, xdouble  *, blasint *);
void  BLASFUNC(xdotcf)  (xdouble *, blasint *, xdouble  *, blasint *, xdouble  *, blasint *);
#else
float   _Complex BLASFUNC(cdotuf)  (blasint *, float  *, blasint *, float  *, blasint *);
float   _Complex BLASFUNC(cdotcf)  (blasint *, float  *, blasint *, float  *, blasint *);
double  _Complex BLASFUNC(zdotuf)  (blasint *, double  *, blasint *, double  *, blasint *);
double  _Complex BLASFUNC(zdotcf)  (blasint *, double  *, blasint *, double  *, blasint *);
xdouble _Complex BLASFUNC(xdotuf)  (blasint *, xdouble  *, blasint *, xdouble  *, blasint *);
xdouble _Complex BLASFUNC(xdotcf)  (blasint *, xdouble  *, blasint *, xdouble  *, blasint *);
#endif

double BLASFUNC(dsdotf) (blasint *, float  *, blasint *, float  *, blasint *);
double BLASFUNC(ddotf)  (blasint *, double *, blasint *, double *, blasint *);
xdouble BLASFUNC(qdotf)  (blasint *, xdouble *, blasint *, xdouble *, blasint *);

int    BLASFUNC(saxpyf) (blasint *, float  *, float  *, blasint *, float  *, blasint *);
int    BLASFUNC(daxpyf) (blasint *, double *, double *, blasint *, double *, blasint *);
int    BLASFUNC(qaxpyf) (blasint *, xdouble *, xdouble *, blasint *, xdouble *, blasint *);
int    BLASFUNC(caxpyf) (blasint *, float  *, float  *, blasint *, float  *, blasint *);
int    BLASFUNC(zaxpyf) (blasint *, double *, double *, blasint *, double *, blasint *);
int    BLASFUNC(xaxpyf) (blasint *, xdouble *, xdouble *, blasint *, xdouble *, blasint *);
int    BLASFUNC(caxpycf)(blasint *, float  *, float  *, blasint *, float  *, blasint *);
int    BLASFUNC(zaxpycf)(blasint *, double *, double *, blasint *, double *, blasint *);
int    BLASFUNC(xaxpycf)(blasint *, xdouble *, xdouble *, blasint *, xdouble *, blasint *);

int    BLASFUNC(scopyf) (blasint *, float  *, blasint *, float  *, blasint *);
int    BLASFUNC(dcopyf) (blasint *, double *, blasint *, double *, blasint *);
int    BLASFUNC(qcopyf) (blasint *, xdouble *, blasint *, xdouble *, blasint *);
int    BLASFUNC(ccopyf) (blasint *, float  *, blasint *, float  *, blasint *);
int    BLASFUNC(zcopyf) (blasint *, double *, blasint *, double *, blasint *);
int    BLASFUNC(xcopyf) (blasint *, xdouble *, blasint *, xdouble *, blasint *);

int    BLASFUNC(sswapf) (blasint *, float  *, blasint *, float  *, blasint *);
int    BLASFUNC(dswapf) (blasint *, double *, blasint *, double *, blasint *);
int    BLASFUNC(qswapf) (blasint *, xdouble *, blasint *, xdouble *, blasint *);
int    BLASFUNC(cswapf) (blasint *, float  *, blasint *, float  *, blasint *);
int    BLASFUNC(zswapf) (blasint *, double *, blasint *, double *, blasint *);
int    BLASFUNC(xswapf) (blasint *, xdouble *, blasint *, xdouble *, blasint *);

FLOATRET  BLASFUNC(sasumf) (blasint *, float  *, blasint *);
FLOATRET  BLASFUNC(scasumf)(blasint *, float  *, blasint *);
double BLASFUNC(dasumf) (blasint *, double *, blasint *);
xdouble BLASFUNC(qasumf) (blasint *, xdouble *, blasint *);
double BLASFUNC(dzasumf)(blasint *, double *, blasint *);
xdouble BLASFUNC(qxasumf)(blasint *, xdouble *, blasint *);

blasint    BLASFUNC(isamaxf)(blasint *, float  *, blasint *);
blasint    BLASFUNC(idamaxf)(blasint *, double *, blasint *);
blasint    BLASFUNC(iqamaxf)(blasint *, xdouble *, blasint *);
blasint    BLASFUNC(icamaxf)(blasint *, float  *, blasint *);
blasint    BLASFUNC(izamaxf)(blasint *, double *, blasint *);
blasint    BLASFUNC(ixamaxf)(blasint *, xdouble *, blasint *);

blasint    BLASFUNC(ismaxf) (blasint *, float  *, blasint *);
blasint    BLASFUNC(idmaxf) (blasint *, double *, blasint *);
blasint    BLASFUNC(iqmaxf) (blasint *, xdouble *, blasint *);
blasint    BLASFUNC(icmaxf) (blasint *, float  *, blasint *);
blasint    BLASFUNC(izmaxf) (blasint *, double *, blasint *);
blasint    BLASFUNC(ixmaxf) (blasint *, xdouble *, blasint *);

blasint    BLASFUNC(isaminf)(blasint *, float  *, blasint *);
blasint    BLASFUNC(idaminf)(blasint *, double *, blasint *);
blasint    BLASFUNC(iqaminf)(blasint *, xdouble *, blasint *);
blasint    BLASFUNC(icaminf)(blasint *, float  *, blasint *);
blasint    BLASFUNC(izaminf)(blasint *, double *, blasint *);
blasint    BLASFUNC(ixaminf)(blasint *, xdouble *, blasint *);

blasint    BLASFUNC(isminf)(blasint *, float  *, blasint *);
blasint    BLASFUNC(idminf)(blasint *, double *, blasint *);
blasint    BLASFUNC(iqminf)(blasint *, xdouble *, blasint *);
blasint    BLASFUNC(icminf)(blasint *, float  *, blasint *);
blasint    BLASFUNC(izminf)(blasint *, double *, blasint *);
blasint    BLASFUNC(ixminf)(blasint *, xdouble *, blasint *);

FLOATRET  BLASFUNC(samaxf) (blasint *, float  *, blasint *);
double BLASFUNC(damaxf) (blasint *, double *, blasint *);
xdouble BLASFUNC(qamaxf) (blasint *, xdouble *, blasint *);
FLOATRET  BLASFUNC(scamaxf)(blasint *, float  *, blasint *);
double BLASFUNC(dzamaxf)(blasint *, double *, blasint *);
xdouble BLASFUNC(qxamaxf)(blasint *, xdouble *, blasint *);

FLOATRET  BLASFUNC(saminf) (blasint *, float  *, blasint *);
double BLASFUNC(daminf) (blasint *, double *, blasint *);
xdouble BLASFUNC(qaminf) (blasint *, xdouble *, blasint *);
FLOATRET  BLASFUNC(scaminf)(blasint *, float  *, blasint *);
double BLASFUNC(dzaminf)(blasint *, double *, blasint *);
xdouble BLASFUNC(qxaminf)(blasint *, xdouble *, blasint *);

FLOATRET  BLASFUNC(smaxf) (blasint *, float  *, blasint *);
double BLASFUNC(dmaxf) (blasint *, double *, blasint *);
xdouble BLASFUNC(qmaxf) (blasint *, xdouble *, blasint *);
FLOATRET  BLASFUNC(scmaxf) (blasint *, float  *, blasint *);
double BLASFUNC(dzmaxf) (blasint *, double *, blasint *);
xdouble BLASFUNC(qxmaxf) (blasint *, xdouble *, blasint *);

FLOATRET  BLASFUNC(sminf) (blasint *, float  *, blasint *);
double BLASFUNC(dminf) (blasint *, double *, blasint *);
xdouble BLASFUNC(qminf) (blasint *, xdouble *, blasint *);
FLOATRET  BLASFUNC(scminf) (blasint *, float  *, blasint *);
double BLASFUNC(dzminf) (blasint *, double *, blasint *);
double BLASFUNC(qxminf) (blasint *, xdouble *, blasint *);

int    BLASFUNC(sscalf) (blasint *,  float  *, float  *, blasint *);
int    BLASFUNC(dscalf) (blasint *,  double *, double *, blasint *);
int    BLASFUNC(qscalf) (blasint *, xdouble *, xdouble *, blasint *);
int    BLASFUNC(cscalf) (blasint *,  float  *, float  *, blasint *);
int    BLASFUNC(zscalf) (blasint *,  double *, double *, blasint *);
int    BLASFUNC(xscalf) (blasint *, xdouble *, xdouble *, blasint *);
int    BLASFUNC(csscalf)(blasint *,  float  *, float  *, blasint *);
int    BLASFUNC(zdscalf)(blasint *,  double *, double *, blasint *);
int    BLASFUNC(xqscalf)(blasint *, xdouble *, xdouble *, blasint *);

FLOATRET  BLASFUNC(snrm2f) (blasint *, float  *, blasint *);
FLOATRET  BLASFUNC(scnrm2f)(blasint *, float  *, blasint *);
double BLASFUNC(dnrm2f) (blasint *, double *, blasint *);
xdouble BLASFUNC(qnrm2f) (blasint *, xdouble *, blasint *);
double BLASFUNC(dznrm2f)(blasint *, double *, blasint *);
double BLASFUNC(qxnrm2f)(blasint *, xdouble *, blasint *);

int    BLASFUNC(srotf)  (blasint *, float  *, blasint *, float  *, blasint *, float  *, float  *);
int    BLASFUNC(drotf)  (blasint *, double *, blasint *, double *, blasint *, double *, double *);
int    BLASFUNC(qrotf)  (blasint *, xdouble *, blasint *, xdouble *, blasint *, xdouble *, xdouble *);
int    BLASFUNC(csrotf) (blasint *, float  *, blasint *, float  *, blasint *, float  *, float  *);
int    BLASFUNC(zdrotf) (blasint *, double *, blasint *, double *, blasint *, double *, double *);
int    BLASFUNC(xqrotf) (blasint *, xdouble *, blasint *, xdouble *, blasint *, xdouble *, xdouble *);

int    BLASFUNC(srotgf) (float  *, float  *, float  *, float  *);
int    BLASFUNC(drotgf) (double *, double *, double *, double *);
int    BLASFUNC(qrotgf) (xdouble *, xdouble *, xdouble *, xdouble *);
int    BLASFUNC(crotgf) (float  *, float  *, float  *, float  *);
int    BLASFUNC(zrotgf) (double *, double *, double *, double *);
int    BLASFUNC(xrotgf) (xdouble *, xdouble *, xdouble *, xdouble *);

int    BLASFUNC(srotmgf)(float  *, float  *, float  *, float  *, float  *);
int    BLASFUNC(drotmgf)(double *, double *, double *, double *, double *);
int    BLASFUNC(qrotmgf)(xdouble *, xdouble *, xdouble *, xdouble *, xdouble *);

int    BLASFUNC(srotmf) (blasint *, float  *, blasint *, float  *, blasint *, float  *);
int    BLASFUNC(drotmf) (blasint *, double *, blasint *, double *, blasint *, double *);
int    BLASFUNC(qrotmf) (blasint *, xdouble *, blasint *, xdouble *, blasint *, xdouble *);

/* Level 2 routines */

int BLASFUNC(sgerf)(blasint *,    blasint *, float *,  float *, blasint *,
		   float *,  blasint *, float *,  blasint *);
int BLASFUNC(dgerf)(blasint *,    blasint *, double *, double *, blasint *,
		   double *, blasint *, double *, blasint *);
int BLASFUNC(qgerf)(blasint *,    blasint *, xdouble *, xdouble *, blasint *,
		   xdouble *, blasint *, xdouble *, blasint *);
int BLASFUNC(cgeruf)(blasint *,    blasint *, float *,  float *, blasint *,
		    float *,  blasint *, float *,  blasint *);
int BLASFUNC(cgercf)(blasint *,    blasint *, float *,  float *, blasint *,
		    float *,  blasint *, float *,  blasint *);
int BLASFUNC(zgeruf)(blasint *,    blasint *, double *, double *, blasint *,
		    double *, blasint *, double *, blasint *);
int BLASFUNC(zgercf)(blasint *,    blasint *, double *, double *, blasint *,
		    double *, blasint *, double *, blasint *);
int BLASFUNC(xgeruf)(blasint *,    blasint *, xdouble *, xdouble *, blasint *,
		    xdouble *, blasint *, xdouble *, blasint *);
int BLASFUNC(xgercf)(blasint *,    blasint *, xdouble *, xdouble *, blasint *,
		    xdouble *, blasint *, xdouble *, blasint *);

int BLASFUNC(sgemvf)(char *, blasint *, blasint *, float  *, float  *, blasint *, 
		    float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(dgemvf)(char *, blasint *, blasint *, double *, double *, blasint *, 
		    double *, blasint *, double *, double *, blasint *);
int BLASFUNC(qgemvf)(char *, blasint *, blasint *, xdouble *, xdouble *, blasint *, 
		    xdouble *, blasint *, xdouble *, xdouble *, blasint *);
int BLASFUNC(cgemvf)(char *, blasint *, blasint *, float  *, float  *, blasint *, 
		    float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(zgemvf)(char *, blasint *, blasint *, double *, double *, blasint *, 
		    double *, blasint *, double *, double *, blasint *);
int BLASFUNC(xgemvf)(char *, blasint *, blasint *, xdouble *, xdouble *, blasint *, 
		    xdouble *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(strsvf)(char *, char *, char *, blasint *, float  *, blasint *,
		     float  *, blasint *);
int BLASFUNC(dtrsvf)(char *, char *, char *, blasint *, double *, blasint *,
		     double *, blasint *);
int BLASFUNC(qtrsvf)(char *, char *, char *, blasint *, xdouble *, blasint *,
		     xdouble *, blasint *);
int BLASFUNC(ctrsvf)(char *, char *, char *, blasint *, float  *, blasint *, 
		     float  *, blasint *);
int BLASFUNC(ztrsvf)(char *, char *, char *, blasint *, double *, blasint *, 
		     double *, blasint *);
int BLASFUNC(xtrsvf)(char *, char *, char *, blasint *, xdouble *, blasint *, 
		     xdouble *, blasint *);

int BLASFUNC(stpsvf)(char *, char *, char *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(dtpsvf)(char *, char *, char *, blasint *, double *, double *, blasint *);
int BLASFUNC(qtpsvf)(char *, char *, char *, blasint *, xdouble *, xdouble *, blasint *);
int BLASFUNC(ctpsvf)(char *, char *, char *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(ztpsvf)(char *, char *, char *, blasint *, double *, double *, blasint *);
int BLASFUNC(xtpsvf)(char *, char *, char *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(strmvf)(char *, char *, char *, blasint *, float  *, blasint *,
		     float  *, blasint *);
int BLASFUNC(dtrmvf)(char *, char *, char *, blasint *, double *, blasint *, 
		     double *, blasint *);
int BLASFUNC(qtrmvf)(char *, char *, char *, blasint *, xdouble *, blasint *, 
		     xdouble *, blasint *);
int BLASFUNC(ctrmvf)(char *, char *, char *, blasint *, float  *, blasint *, 
		     float  *, blasint *);
int BLASFUNC(ztrmvf)(char *, char *, char *, blasint *, double *, blasint *, 
		     double *, blasint *);
int BLASFUNC(xtrmvf)(char *, char *, char *, blasint *, xdouble *, blasint *, 
		     xdouble *, blasint *);

int BLASFUNC(stpmvf)(char *, char *, char *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(dtpmvf)(char *, char *, char *, blasint *, double *, double *, blasint *);
int BLASFUNC(qtpmvf)(char *, char *, char *, blasint *, xdouble *, xdouble *, blasint *);
int BLASFUNC(ctpmvf)(char *, char *, char *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(ztpmvf)(char *, char *, char *, blasint *, double *, double *, blasint *);
int BLASFUNC(xtpmvf)(char *, char *, char *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(stbmvf)(char *, char *, char *, blasint *, blasint *, float  *, blasint *, float  *, blasint *);
int BLASFUNC(dtbmvf)(char *, char *, char *, blasint *, blasint *, double *, blasint *, double *, blasint *);
int BLASFUNC(qtbmvf)(char *, char *, char *, blasint *, blasint *, xdouble *, blasint *, xdouble *, blasint *);
int BLASFUNC(ctbmvf)(char *, char *, char *, blasint *, blasint *, float  *, blasint *, float  *, blasint *);
int BLASFUNC(ztbmvf)(char *, char *, char *, blasint *, blasint *, double *, blasint *, double *, blasint *);
int BLASFUNC(xtbmvf)(char *, char *, char *, blasint *, blasint *, xdouble *, blasint *, xdouble *, blasint *);

int BLASFUNC(stbsvf)(char *, char *, char *, blasint *, blasint *, float  *, blasint *, float  *, blasint *);
int BLASFUNC(dtbsvf)(char *, char *, char *, blasint *, blasint *, double *, blasint *, double *, blasint *);
int BLASFUNC(qtbsvf)(char *, char *, char *, blasint *, blasint *, xdouble *, blasint *, xdouble *, blasint *);
int BLASFUNC(ctbsvf)(char *, char *, char *, blasint *, blasint *, float  *, blasint *, float  *, blasint *);
int BLASFUNC(ztbsvf)(char *, char *, char *, blasint *, blasint *, double *, blasint *, double *, blasint *);
int BLASFUNC(xtbsvf)(char *, char *, char *, blasint *, blasint *, xdouble *, blasint *, xdouble *, blasint *);

int BLASFUNC(ssymvf)(char *, blasint *, float  *, float *, blasint *, 
		     float  *, blasint *, float *, float *, blasint *);
int BLASFUNC(dsymvf)(char *, blasint *, double  *, double *, blasint *, 
		     double  *, blasint *, double *, double *, blasint *);
int BLASFUNC(qsymvf)(char *, blasint *, xdouble  *, xdouble *, blasint *, 
		     xdouble  *, blasint *, xdouble *, xdouble *, blasint *);
int BLASFUNC(csymvf)(char *, blasint *, float  *, float *, blasint *, 
		     float  *, blasint *, float *, float *, blasint *);
int BLASFUNC(zsymvf)(char *, blasint *, double  *, double *, blasint *, 
		     double  *, blasint *, double *, double *, blasint *);
int BLASFUNC(xsymvf)(char *, blasint *, xdouble  *, xdouble *, blasint *, 
		     xdouble  *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(sspmvf)(char *, blasint *, float  *, float *,
		     float  *, blasint *, float *, float *, blasint *);
int BLASFUNC(dspmvf)(char *, blasint *, double  *, double *, 
		     double  *, blasint *, double *, double *, blasint *);
int BLASFUNC(qspmvf)(char *, blasint *, xdouble  *, xdouble *, 
		     xdouble  *, blasint *, xdouble *, xdouble *, blasint *);
int BLASFUNC(cspmvf)(char *, blasint *, float  *, float *,
		     float  *, blasint *, float *, float *, blasint *);
int BLASFUNC(zspmvf)(char *, blasint *, double  *, double *,
		     double  *, blasint *, double *, double *, blasint *);
int BLASFUNC(xspmvf)(char *, blasint *, xdouble  *, xdouble *,
		     xdouble  *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(ssyrf)(char *, blasint *, float   *, float  *, blasint *,
		    float  *, blasint *);
int BLASFUNC(dsyrf)(char *, blasint *, double  *, double *, blasint *,
		    double *, blasint *);
int BLASFUNC(qsyrf)(char *, blasint *, xdouble  *, xdouble *, blasint *,
		    xdouble *, blasint *);
int BLASFUNC(csyrf)(char *, blasint *, float   *, float  *, blasint *,
		    float  *, blasint *);
int BLASFUNC(zsyrf)(char *, blasint *, double  *, double *, blasint *,
		    double *, blasint *);
int BLASFUNC(xsyrf)(char *, blasint *, xdouble  *, xdouble *, blasint *,
		    xdouble *, blasint *);

int BLASFUNC(ssyr2f)(char *, blasint *, float   *, 
		     float  *, blasint *, float  *, blasint *, float  *, blasint *);
int BLASFUNC(dsyr2f)(char *, blasint *, double  *, 
		     double *, blasint *, double *, blasint *, double *, blasint *);
int BLASFUNC(qsyr2f)(char *, blasint *, xdouble  *, 
		     xdouble *, blasint *, xdouble *, blasint *, xdouble *, blasint *);
int BLASFUNC(csyr2f)(char *, blasint *, float   *, 
		     float  *, blasint *, float  *, blasint *, float  *, blasint *);
int BLASFUNC(zsyr2f)(char *, blasint *, double  *, 
		     double *, blasint *, double *, blasint *, double *, blasint *);
int BLASFUNC(xsyr2f)(char *, blasint *, xdouble  *, 
		     xdouble *, blasint *, xdouble *, blasint *, xdouble *, blasint *);

int BLASFUNC(ssprf)(char *, blasint *, float   *, float  *, blasint *,
		    float  *);
int BLASFUNC(dsprf)(char *, blasint *, double  *, double *, blasint *,
		    double *);
int BLASFUNC(qsprf)(char *, blasint *, xdouble  *, xdouble *, blasint *,
		    xdouble *);
int BLASFUNC(csprf)(char *, blasint *, float   *, float  *, blasint *,
		    float  *);
int BLASFUNC(zsprf)(char *, blasint *, double  *, double *, blasint *,
		    double *);
int BLASFUNC(xsprf)(char *, blasint *, xdouble  *, xdouble *, blasint *,
		    xdouble *);

int BLASFUNC(sspr2f)(char *, blasint *, float   *, 
		     float  *, blasint *, float  *, blasint *, float  *);
int BLASFUNC(dspr2f)(char *, blasint *, double  *, 
		     double *, blasint *, double *, blasint *, double *);
int BLASFUNC(qspr2f)(char *, blasint *, xdouble  *, 
		     xdouble *, blasint *, xdouble *, blasint *, xdouble *);
int BLASFUNC(cspr2f)(char *, blasint *, float   *, 
		     float  *, blasint *, float  *, blasint *, float  *);
int BLASFUNC(zspr2f)(char *, blasint *, double  *, 
		     double *, blasint *, double *, blasint *, double *);
int BLASFUNC(xspr2f)(char *, blasint *, xdouble  *, 
		     xdouble *, blasint *, xdouble *, blasint *, xdouble *);

int BLASFUNC(cherf)(char *, blasint *, float   *, float  *, blasint *,
		    float  *, blasint *);
int BLASFUNC(zherf)(char *, blasint *, double  *, double *, blasint *,
		    double *, blasint *);
int BLASFUNC(xherf)(char *, blasint *, xdouble  *, xdouble *, blasint *,
		    xdouble *, blasint *);

int BLASFUNC(chprf)(char *, blasint *, float   *, float  *, blasint *, float  *);
int BLASFUNC(zhprf)(char *, blasint *, double  *, double *, blasint *, double *);
int BLASFUNC(xhprf)(char *, blasint *, xdouble  *, xdouble *, blasint *, xdouble *);

int BLASFUNC(cher2f)(char *, blasint *, float   *, 
		     float  *, blasint *, float  *, blasint *, float  *, blasint *);
int BLASFUNC(zher2f)(char *, blasint *, double  *, 
		     double *, blasint *, double *, blasint *, double *, blasint *);
int BLASFUNC(xher2f)(char *, blasint *, xdouble  *, 
		     xdouble *, blasint *, xdouble *, blasint *, xdouble *, blasint *);

int BLASFUNC(chpr2f)(char *, blasint *, float   *, 
		     float  *, blasint *, float  *, blasint *, float  *);
int BLASFUNC(zhpr2f)(char *, blasint *, double  *, 
		     double *, blasint *, double *, blasint *, double *);
int BLASFUNC(xhpr2f)(char *, blasint *, xdouble  *, 
		     xdouble *, blasint *, xdouble *, blasint *, xdouble *);

int BLASFUNC(chemvf)(char *, blasint *, float  *, float *, blasint *, 
		     float  *, blasint *, float *, float *, blasint *);
int BLASFUNC(zhemvf)(char *, blasint *, double  *, double *, blasint *, 
		     double  *, blasint *, double *, double *, blasint *);
int BLASFUNC(xhemvf)(char *, blasint *, xdouble  *, xdouble *, blasint *, 
		     xdouble  *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(chpmvf)(char *, blasint *, float  *, float *,
		     float  *, blasint *, float *, float *, blasint *);
int BLASFUNC(zhpmvf)(char *, blasint *, double  *, double *,
		     double  *, blasint *, double *, double *, blasint *);
int BLASFUNC(xhpmvf)(char *, blasint *, xdouble  *, xdouble *,
		     xdouble  *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(snormf)(char *, blasint *, blasint *, float  *, blasint *);
int BLASFUNC(dnormf)(char *, blasint *, blasint *, double *, blasint *);
int BLASFUNC(cnormf)(char *, blasint *, blasint *, float  *, blasint *);
int BLASFUNC(znormf)(char *, blasint *, blasint *, double *, blasint *);

int BLASFUNC(sgbmvf)(char *, blasint *, blasint *, blasint *, blasint *, float  *, float  *, blasint *, 
		    float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(dgbmvf)(char *, blasint *, blasint *, blasint *, blasint *, double *, double *, blasint *, 
		    double *, blasint *, double *, double *, blasint *);
int BLASFUNC(qgbmvf)(char *, blasint *, blasint *, blasint *, blasint *, xdouble *, xdouble *, blasint *, 
		    xdouble *, blasint *, xdouble *, xdouble *, blasint *);
int BLASFUNC(cgbmvf)(char *, blasint *, blasint *, blasint *, blasint *, float  *, float  *, blasint *, 
		    float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(zgbmvf)(char *, blasint *, blasint *, blasint *, blasint *, double *, double *, blasint *, 
		    double *, blasint *, double *, double *, blasint *);
int BLASFUNC(xgbmvf)(char *, blasint *, blasint *, blasint *, blasint *, xdouble *, xdouble *, blasint *, 
		    xdouble *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(ssbmvf)(char *, blasint *, blasint *, float  *, float  *, blasint *, 
		    float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(dsbmvf)(char *, blasint *, blasint *, double *, double *, blasint *, 
		    double *, blasint *, double *, double *, blasint *);
int BLASFUNC(qsbmvf)(char *, blasint *, blasint *, xdouble *, xdouble *, blasint *, 
		    xdouble *, blasint *, xdouble *, xdouble *, blasint *);
int BLASFUNC(csbmvf)(char *, blasint *, blasint *, float  *, float  *, blasint *, 
		    float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(zsbmvf)(char *, blasint *, blasint *, double *, double *, blasint *, 
		    double *, blasint *, double *, double *, blasint *);
int BLASFUNC(xsbmvf)(char *, blasint *, blasint *, xdouble *, xdouble *, blasint *, 
		    xdouble *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(chbmvf)(char *, blasint *, blasint *, float  *, float  *, blasint *, 
		    float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(zhbmvf)(char *, blasint *, blasint *, double *, double *, blasint *, 
		    double *, blasint *, double *, double *, blasint *);
int BLASFUNC(xhbmvf)(char *, blasint *, blasint *, xdouble *, xdouble *, blasint *, 
		    xdouble *, blasint *, xdouble *, xdouble *, blasint *);

/* Level 3 routines */

GEMMRETTYPE BLASFUNC(sgemmf)(char *, char *, blasint *, blasint *, blasint *, float *,
	   float  *, blasint *, float  *, blasint *, float  *, float  *, blasint *);
GEMMRETTYPE BLASFUNC(dgemmf)(char *, char *, blasint *, blasint *, blasint *, double *,
	   double *, blasint *, double *, blasint *, double *, double *, blasint *);
GEMMRETTYPE BLASFUNC(qgemmf)(char *, char *, blasint *, blasint *, blasint *, xdouble *,
	   xdouble *, blasint *, xdouble *, blasint *, xdouble *, xdouble *, blasint *);
GEMMRETTYPE BLASFUNC(cgemmf)(char *, char *, blasint *, blasint *, blasint *, float *,
	   float  *, blasint *, float  *, blasint *, float  *, float  *, blasint *);
GEMMRETTYPE BLASFUNC(zgemmf)(char *, char *, blasint *, blasint *, blasint *, double *,
	   double *, blasint *, double *, blasint *, double *, double *, blasint *);
GEMMRETTYPE BLASFUNC(xgemmf)(char *, char *, blasint *, blasint *, blasint *, xdouble *,
	   xdouble *, blasint *, xdouble *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(sge2mmf)(char *, char *, char *, blasint *, blasint *,
		     float *, float  *, blasint *, float  *, blasint *,
		     float *, float  *, blasint *);
int BLASFUNC(dge2mmf)(char *, char *, char *, blasint *, blasint *,
		     double *, double  *, blasint *, double  *, blasint *,
		     double *, double  *, blasint *);
int BLASFUNC(cge2mmf)(char *, char *, char *, blasint *, blasint *,
		     float *, float  *, blasint *, float  *, blasint *,
		     float *, float  *, blasint *);
int BLASFUNC(zge2mmf)(char *, char *, char *, blasint *, blasint *,
		     double *, double  *, blasint *, double  *, blasint *,
		     double *, double  *, blasint *);

int BLASFUNC(strsmf)(char *, char *, char *, char *, blasint *, blasint *,
	   float *,  float *, blasint *, float *, blasint *);
int BLASFUNC(dtrsmf)(char *, char *, char *, char *, blasint *, blasint *,
	   double *,  double *, blasint *, double *, blasint *);
int BLASFUNC(qtrsmf)(char *, char *, char *, char *, blasint *, blasint *,
	   xdouble *,  xdouble *, blasint *, xdouble *, blasint *);
int BLASFUNC(ctrsmf)(char *, char *, char *, char *, blasint *, blasint *,
	   float *,  float *, blasint *, float *, blasint *);
int BLASFUNC(ztrsmf)(char *, char *, char *, char *, blasint *, blasint *,
	   double *,  double *, blasint *, double *, blasint *);
int BLASFUNC(xtrsmf)(char *, char *, char *, char *, blasint *, blasint *,
	   xdouble *,  xdouble *, blasint *, xdouble *, blasint *);

int BLASFUNC(strmmf)(char *, char *, char *, char *, blasint *, blasint *,
	   float *,  float *, blasint *, float *, blasint *);
int BLASFUNC(dtrmmf)(char *, char *, char *, char *, blasint *, blasint *,
	   double *,  double *, blasint *, double *, blasint *);
int BLASFUNC(qtrmmf)(char *, char *, char *, char *, blasint *, blasint *,
	   xdouble *,  xdouble *, blasint *, xdouble *, blasint *);
int BLASFUNC(ctrmmf)(char *, char *, char *, char *, blasint *, blasint *,
	   float *,  float *, blasint *, float *, blasint *);
int BLASFUNC(ztrmmf)(char *, char *, char *, char *, blasint *, blasint *,
	   double *,  double *, blasint *, double *, blasint *);
int BLASFUNC(xtrmmf)(char *, char *, char *, char *, blasint *, blasint *,
	   xdouble *,  xdouble *, blasint *, xdouble *, blasint *);

int BLASFUNC(ssymmf)(char *, char *, blasint *, blasint *, float  *, float  *, blasint *,
	   float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(dsymmf)(char *, char *, blasint *, blasint *, double *, double *, blasint *,
	   double *, blasint *, double *, double *, blasint *);
int BLASFUNC(qsymmf)(char *, char *, blasint *, blasint *, xdouble *, xdouble *, blasint *,
	   xdouble *, blasint *, xdouble *, xdouble *, blasint *);
int BLASFUNC(csymmf)(char *, char *, blasint *, blasint *, float  *, float  *, blasint *,
	   float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(zsymmf)(char *, char *, blasint *, blasint *, double *, double *, blasint *,
	   double *, blasint *, double *, double *, blasint *);
int BLASFUNC(xsymmf)(char *, char *, blasint *, blasint *, xdouble *, xdouble *, blasint *,
	   xdouble *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(ssyrkf)(char *, char *, blasint *, blasint *, float  *, float  *, blasint *,
	   float  *, float  *, blasint *);
int BLASFUNC(dsyrkf)(char *, char *, blasint *, blasint *, double *, double *, blasint *,
	   double *, double *, blasint *);
int BLASFUNC(qsyrkf)(char *, char *, blasint *, blasint *, xdouble *, xdouble *, blasint *,
	   xdouble *, xdouble *, blasint *);
int BLASFUNC(csyrkf)(char *, char *, blasint *, blasint *, float  *, float  *, blasint *,
	   float  *, float  *, blasint *);
int BLASFUNC(zsyrkf)(char *, char *, blasint *, blasint *, double *, double *, blasint *,
	   double *, double *, blasint *);
int BLASFUNC(xsyrkf)(char *, char *, blasint *, blasint *, xdouble *, xdouble *, blasint *,
	   xdouble *, xdouble *, blasint *);

int BLASFUNC(ssyr2kf)(char *, char *, blasint *, blasint *, float  *, float  *, blasint *,
	   float *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(dsyr2kf)(char *, char *, blasint *, blasint *, double *, double *, blasint *,
	   double*, blasint *, double *, double *, blasint *);
int BLASFUNC(qsyr2kf)(char *, char *, blasint *, blasint *, xdouble *, xdouble *, blasint *,
	   xdouble*, blasint *, xdouble *, xdouble *, blasint *);
int BLASFUNC(csyr2kf)(char *, char *, blasint *, blasint *, float  *, float  *, blasint *,
	   float *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(zsyr2kf)(char *, char *, blasint *, blasint *, double *, double *, blasint *,
	   double*, blasint *, double *, double *, blasint *);
int BLASFUNC(xsyr2kf)(char *, char *, blasint *, blasint *, xdouble *, xdouble *, blasint *,
	   xdouble*, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(chemmf)(char *, char *, blasint *, blasint *, float  *, float  *, blasint *,
	   float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(zhemmf)(char *, char *, blasint *, blasint *, double *, double *, blasint *,
	   double *, blasint *, double *, double *, blasint *);
int BLASFUNC(xhemmf)(char *, char *, blasint *, blasint *, xdouble *, xdouble *, blasint *,
	   xdouble *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(cherkf)(char *, char *, blasint *, blasint *, float  *, float  *, blasint *,
	   float  *, float  *, blasint *);
int BLASFUNC(zherkf)(char *, char *, blasint *, blasint *, double *, double *, blasint *,
	   double *, double *, blasint *);
int BLASFUNC(xherkf)(char *, char *, blasint *, blasint *, xdouble *, xdouble *, blasint *,
	   xdouble *, xdouble *, blasint *);

int BLASFUNC(cher2kf)(char *, char *, blasint *, blasint *, float  *, float  *, blasint *,
	   float *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(zher2kf)(char *, char *, blasint *, blasint *, double *, double *, blasint *,
	   double*, blasint *, double *, double *, blasint *);
int BLASFUNC(xher2kf)(char *, char *, blasint *, blasint *, xdouble *, xdouble *, blasint *,
	   xdouble*, blasint *, xdouble *, xdouble *, blasint *);
int BLASFUNC(cher2mf)(char *, char *, char *, blasint *, blasint *, float  *, float  *, blasint *,
	   float *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(zher2mf)(char *, char *, char *, blasint *, blasint *, double *, double *, blasint *,
	   double*, blasint *, double *, double *, blasint *);
int BLASFUNC(xher2mf)(char *, char *, char *, blasint *, blasint *, xdouble *, xdouble *, blasint *,
	   xdouble*, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(sgemtf)(char *, blasint *, blasint *, float  *, float  *, blasint *,
		    float  *, blasint *);
int BLASFUNC(dgemtf)(char *, blasint *, blasint *, double *, double *, blasint *,
		    double *, blasint *);
int BLASFUNC(cgemtf)(char *, blasint *, blasint *, float  *, float  *, blasint *, 
		    float  *, blasint *);
int BLASFUNC(zgemtf)(char *, blasint *, blasint *, double *, double *, blasint *,
		    double *, blasint *);

int BLASFUNC(sgemaf)(char *, char *, blasint *, blasint *, float  *, 
		    float  *, blasint *, float *, float  *, blasint *, float *, blasint *);
int BLASFUNC(dgemaf)(char *, char *, blasint *, blasint *, double *,
		    double *, blasint *, double*, double *, blasint *, double*, blasint *);
int BLASFUNC(cgemaf)(char *, char *, blasint *, blasint *, float  *,
		    float  *, blasint *, float *, float  *, blasint *, float *, blasint *);
int BLASFUNC(zgemaf)(char *, char *, blasint *, blasint *, double *, 
		    double *, blasint *, double*, double *, blasint *, double*, blasint *);

int BLASFUNC(sgemsf)(char *, char *, blasint *, blasint *, float  *,
		    float  *, blasint *, float *, float  *, blasint *, float *, blasint *);
int BLASFUNC(dgemsf)(char *, char *, blasint *, blasint *, double *,
		    double *, blasint *, double*, double *, blasint *, double*, blasint *);
int BLASFUNC(cgemsf)(char *, char *, blasint *, blasint *, float  *,
		    float  *, blasint *, float *, float  *, blasint *, float *, blasint *);
int BLASFUNC(zgemsf)(char *, char *, blasint *, blasint *, double *,
		    double *, blasint *, double*, double *, blasint *, double*, blasint *);

int BLASFUNC(sgemcf)(char *, char *, blasint *, blasint *, blasint *, float *,
	   float  *, blasint *, float  *, blasint *, float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(dgemcf)(char *, char *, blasint *, blasint *, blasint *, double *,
	   double *, blasint *, double *, blasint *, double *, blasint *, double *, double *, blasint *);
int BLASFUNC(qgemcf)(char *, char *, blasint *, blasint *, blasint *, xdouble *,
	   xdouble *, blasint *, xdouble *, blasint *, xdouble *, blasint *,  xdouble *, xdouble *, blasint *);
int BLASFUNC(cgemcf)(char *, char *, blasint *, blasint *, blasint *, float *,
	   float  *, blasint *, float  *, blasint *, float  *, blasint *, float  *, float  *, blasint *);
int BLASFUNC(zgemcf)(char *, char *, blasint *, blasint *, blasint *, double *,
	   double *, blasint *, double *, blasint *, double *, blasint *, double *, double *, blasint *);
int BLASFUNC(xgemcf)(char *, char *, blasint *, blasint *, blasint *, xdouble *,
	   xdouble *, blasint *, xdouble *, blasint *, xdouble *, blasint *, xdouble *, xdouble *, blasint *);

int BLASFUNC(sgetf2f)(blasint *, blasint *, float  *, blasint *, blasint *, blasint *);
int BLASFUNC(dgetf2f)(blasint *, blasint *, double *, blasint *, blasint *, blasint *);
int BLASFUNC(qgetf2f)(blasint *, blasint *, xdouble *, blasint *, blasint *, blasint *);
int BLASFUNC(cgetf2f)(blasint *, blasint *, float  *, blasint *, blasint *, blasint *);
int BLASFUNC(zgetf2f)(blasint *, blasint *, double *, blasint *, blasint *, blasint *);
int BLASFUNC(xgetf2f)(blasint *, blasint *, xdouble *, blasint *, blasint *, blasint *);

int BLASFUNC(sgetrff)(blasint *, blasint *, float  *, blasint *, blasint *, blasint *);
int BLASFUNC(dgetrff)(blasint *, blasint *, double *, blasint *, blasint *, blasint *);
int BLASFUNC(qgetrff)(blasint *, blasint *, xdouble *, blasint *, blasint *, blasint *);
int BLASFUNC(cgetrff)(blasint *, blasint *, float  *, blasint *, blasint *, blasint *);
int BLASFUNC(zgetrff)(blasint *, blasint *, double *, blasint *, blasint *, blasint *);
int BLASFUNC(xgetrff)(blasint *, blasint *, xdouble *, blasint *, blasint *, blasint *);

int BLASFUNC(slaswpf)(blasint *, float  *, blasint *, blasint *, blasint *, blasint *, blasint *);
int BLASFUNC(dlaswpf)(blasint *, double *, blasint *, blasint *, blasint *, blasint *, blasint *);
int BLASFUNC(qlaswpf)(blasint *, xdouble *, blasint *, blasint *, blasint *, blasint *, blasint *);
int BLASFUNC(claswpf)(blasint *, float  *, blasint *, blasint *, blasint *, blasint *, blasint *);
int BLASFUNC(zlaswpf)(blasint *, double *, blasint *, blasint *, blasint *, blasint *, blasint *);
int BLASFUNC(xlaswpf)(blasint *, xdouble *, blasint *, blasint *, blasint *, blasint *, blasint *);

int BLASFUNC(sgetrsf)(char *, blasint *, blasint *, float  *, blasint *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(dgetrsf)(char *, blasint *, blasint *, double *, blasint *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(qgetrsf)(char *, blasint *, blasint *, xdouble *, blasint *, blasint *, xdouble *, blasint *, blasint *);
int BLASFUNC(cgetrsf)(char *, blasint *, blasint *, float  *, blasint *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(zgetrsf)(char *, blasint *, blasint *, double *, blasint *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(xgetrsf)(char *, blasint *, blasint *, xdouble *, blasint *, blasint *, xdouble *, blasint *, blasint *);

int BLASFUNC(sgesvf)(blasint *, blasint *, float  *, blasint *, blasint *, float *, blasint *, blasint *);
int BLASFUNC(dgesvf)(blasint *, blasint *, double *, blasint *, blasint *, double*, blasint *, blasint *);
int BLASFUNC(qgesvf)(blasint *, blasint *, xdouble *, blasint *, blasint *, xdouble*, blasint *, blasint *);
int BLASFUNC(cgesvf)(blasint *, blasint *, float  *, blasint *, blasint *, float *, blasint *, blasint *);
int BLASFUNC(zgesvf)(blasint *, blasint *, double *, blasint *, blasint *, double*, blasint *, blasint *);
int BLASFUNC(xgesvf)(blasint *, blasint *, xdouble *, blasint *, blasint *, xdouble*, blasint *, blasint *);

int BLASFUNC(spotf2f)(char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(dpotf2f)(char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(qpotf2f)(char *, blasint *, xdouble *, blasint *, blasint *);
int BLASFUNC(cpotf2f)(char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(zpotf2f)(char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(xpotf2f)(char *, blasint *, xdouble *, blasint *, blasint *);

int BLASFUNC(spotrff)(char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(dpotrff)(char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(qpotrff)(char *, blasint *, xdouble *, blasint *, blasint *);
int BLASFUNC(cpotrff)(char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(zpotrff)(char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(xpotrff)(char *, blasint *, xdouble *, blasint *, blasint *);

int BLASFUNC(slauu2f)(char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(dlauu2f)(char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(qlauu2f)(char *, blasint *, xdouble *, blasint *, blasint *);
int BLASFUNC(clauu2f)(char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(zlauu2f)(char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(xlauu2f)(char *, blasint *, xdouble *, blasint *, blasint *);

int BLASFUNC(slauumf)(char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(dlauumf)(char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(qlauumf)(char *, blasint *, xdouble *, blasint *, blasint *);
int BLASFUNC(clauumf)(char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(zlauumf)(char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(xlauumf)(char *, blasint *, xdouble *, blasint *, blasint *);

int BLASFUNC(strti2f)(char *, char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(dtrti2f)(char *, char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(qtrti2f)(char *, char *, blasint *, xdouble *, blasint *, blasint *);
int BLASFUNC(ctrti2f)(char *, char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(ztrti2f)(char *, char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(xtrti2f)(char *, char *, blasint *, xdouble *, blasint *, blasint *);

int BLASFUNC(strtrif)(char *, char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(dtrtrif)(char *, char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(qtrtrif)(char *, char *, blasint *, xdouble *, blasint *, blasint *);
int BLASFUNC(ctrtrif)(char *, char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(ztrtrif)(char *, char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(xtrtrif)(char *, char *, blasint *, xdouble *, blasint *, blasint *);

int BLASFUNC(spotrif)(char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(dpotrif)(char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(qpotrif)(char *, blasint *, xdouble *, blasint *, blasint *);
int BLASFUNC(cpotrif)(char *, blasint *, float  *, blasint *, blasint *);
int BLASFUNC(zpotrif)(char *, blasint *, double *, blasint *, blasint *);
int BLASFUNC(xpotrif)(char *, blasint *, xdouble *, blasint *, blasint *);

FLOATRET  BLASFUNC(slamchf)(char *);
double    BLASFUNC(dlamchf)(char *);
xdouble   BLASFUNC(qlamchf)(char *);

FLOATRET  BLASFUNC(slamc3f)(float *, float *);
double    BLASFUNC(dlamc3f)(double *, double *);
xdouble   BLASFUNC(qlamc3f)(xdouble *, xdouble *);

int BLASFUNC(slarff)(char *, blasint *, blasint *, float *, blasint *, float *, float *, blasint *, float *);
int BLASFUNC(dlarff)(char *, blasint *, blasint *, double *, blasint *, double *, double *, blasint *, double *);
int BLASFUNC(qlarff)(char *, blasint *, blasint *, xdouble *, blasint *, xdouble *, xdouble *, blasint *, xdouble *);
int BLASFUNC(clarff)(char *, blasint *, blasint *, float *, blasint *, float *, float *, blasint *, float *);
int BLASFUNC(zlarff)(char *, blasint *, blasint *, double *, blasint *, double *, double *, blasint *, double *);
int BLASFUNC(xlarff)(char *, blasint *, blasint *, xdouble *, blasint *, xdouble *, xdouble *, blasint *, xdouble *);

#endif
#endif
