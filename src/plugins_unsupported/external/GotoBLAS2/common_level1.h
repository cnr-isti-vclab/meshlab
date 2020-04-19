/*********************************************************************/
/*                                                                   */
/*             Optimized BLAS libraries                              */
/*                     By Kazushige Goto <kgoto@tacc.utexas.edu>     */
/*                                                                   */
/* Copyright (c) The University of Texas, 2009. All rights reserved. */
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

#ifndef ASSEMBLER

#ifdef __CUDACC__
extern "C" {
#endif

float   sdot_k(BLASLONG, float   *, BLASLONG, float   *, BLASLONG);
double dsdot_k(BLASLONG, float   *, BLASLONG, float *, BLASLONG);
double  ddot_k(BLASLONG, double  *, BLASLONG, double  *, BLASLONG);
xdouble qdot_k(BLASLONG, xdouble *, BLASLONG, xdouble *, BLASLONG);

float   _Complex cdotc_k (BLASLONG, float  *, BLASLONG, float  *, BLASLONG);
float   _Complex cdotu_k (BLASLONG, float  *, BLASLONG, float  *, BLASLONG);
double  _Complex zdotc_k (BLASLONG, double *, BLASLONG, double *, BLASLONG);
double  _Complex zdotu_k (BLASLONG, double *, BLASLONG, double *, BLASLONG);
xdouble _Complex xdotc_k (BLASLONG, xdouble *, BLASLONG, xdouble *, BLASLONG);
xdouble _Complex xdotu_k (BLASLONG, xdouble *, BLASLONG, xdouble *, BLASLONG);

int    saxpy_k (BLASLONG, BLASLONG, BLASLONG, float, 
	       float  *, BLASLONG, float  *, BLASLONG, float  *, BLASLONG);
int    daxpy_k (BLASLONG, BLASLONG, BLASLONG, double, 
	       double *, BLASLONG, double *, BLASLONG, double *, BLASLONG);
int    qaxpy_k (BLASLONG, BLASLONG, BLASLONG, xdouble, 
	       xdouble *, BLASLONG, xdouble *, BLASLONG, xdouble *, BLASLONG);
int    caxpy_k (BLASLONG, BLASLONG, BLASLONG, float,  float,
	       float  *, BLASLONG, float  *, BLASLONG, float  *, BLASLONG);
int    zaxpy_k (BLASLONG, BLASLONG, BLASLONG, double, double,
	       double *, BLASLONG, double *, BLASLONG, double *, BLASLONG);
int    xaxpy_k (BLASLONG, BLASLONG, BLASLONG, xdouble, xdouble,
	       xdouble *, BLASLONG, xdouble *, BLASLONG, xdouble *, BLASLONG);
int    caxpyc_k (BLASLONG, BLASLONG, BLASLONG, float,  float,
	       float  *, BLASLONG, float  *, BLASLONG, float  *, BLASLONG);
int    zaxpyc_k (BLASLONG, BLASLONG, BLASLONG, double, double,
	       double *, BLASLONG, double *, BLASLONG, double *, BLASLONG);
int    xaxpyc_k (BLASLONG, BLASLONG, BLASLONG, xdouble, xdouble,
	       xdouble *, BLASLONG, xdouble *, BLASLONG, xdouble *, BLASLONG);

int    scopy_k(BLASLONG, float  *, BLASLONG, float  *, BLASLONG);
int    dcopy_k(BLASLONG, double *, BLASLONG, double *, BLASLONG);
int    qcopy_k(BLASLONG, xdouble *, BLASLONG, xdouble *, BLASLONG);
int    ccopy_k(BLASLONG, float  *, BLASLONG, float  *, BLASLONG);
int    zcopy_k(BLASLONG, double *, BLASLONG, double *, BLASLONG);
int    xcopy_k(BLASLONG, xdouble *, BLASLONG, xdouble *, BLASLONG);

int    sswap_k (BLASLONG, BLASLONG, BLASLONG, float, 
	       float  *, BLASLONG, float  *, BLASLONG, float *, BLASLONG);
int    dswap_k (BLASLONG, BLASLONG, BLASLONG, double, 
	       double *, BLASLONG, double *, BLASLONG, double*, BLASLONG);
int    qswap_k (BLASLONG, BLASLONG, BLASLONG, xdouble, 
	       xdouble *, BLASLONG, xdouble *, BLASLONG, xdouble*, BLASLONG);
int    cswap_k (BLASLONG, BLASLONG, BLASLONG, float,  float,
	       float  *, BLASLONG, float  *, BLASLONG, float *, BLASLONG);
int    zswap_k (BLASLONG, BLASLONG, BLASLONG, double, double,
	       double *, BLASLONG, double *, BLASLONG, double*, BLASLONG);
int    xswap_k (BLASLONG, BLASLONG, BLASLONG, xdouble, xdouble,
	       xdouble *, BLASLONG, xdouble *, BLASLONG, xdouble*, BLASLONG);

float   sasum_k (BLASLONG, float  *, BLASLONG);
double  dasum_k (BLASLONG, double *, BLASLONG);
xdouble qasum_k (BLASLONG, xdouble *, BLASLONG);
float   casum_k (BLASLONG, float  *, BLASLONG);
double  zasum_k (BLASLONG, double *, BLASLONG);
xdouble xasum_k (BLASLONG, xdouble *, BLASLONG);

float   samax_k (BLASLONG, float  *, BLASLONG);
double  damax_k (BLASLONG, double *, BLASLONG);
xdouble qamax_k (BLASLONG, xdouble *, BLASLONG);
float   camax_k (BLASLONG, float  *, BLASLONG);
double  zamax_k (BLASLONG, double *, BLASLONG);
xdouble xamax_k (BLASLONG, xdouble *, BLASLONG);

float   samin_k (BLASLONG, float  *, BLASLONG);
double  damin_k (BLASLONG, double *, BLASLONG);
xdouble qamin_k (BLASLONG, xdouble *, BLASLONG);
float   camin_k (BLASLONG, float  *, BLASLONG);
double  zamin_k (BLASLONG, double *, BLASLONG);
xdouble xamin_k (BLASLONG, xdouble *, BLASLONG);

BLASLONG   isamax_k(BLASLONG, float  *, BLASLONG);
BLASLONG   idamax_k(BLASLONG, double *, BLASLONG);
BLASLONG   iqamax_k(BLASLONG, xdouble *, BLASLONG);
BLASLONG   icamax_k(BLASLONG, float  *, BLASLONG);
BLASLONG   izamax_k(BLASLONG, double *, BLASLONG);
BLASLONG   ixamax_k(BLASLONG, xdouble *, BLASLONG);

BLASLONG   isamin_k(BLASLONG, float  *, BLASLONG);
BLASLONG   idamin_k(BLASLONG, double *, BLASLONG);
BLASLONG   iqamin_k(BLASLONG, xdouble *, BLASLONG);
BLASLONG   icamin_k(BLASLONG, float  *, BLASLONG);
BLASLONG   izamin_k(BLASLONG, double *, BLASLONG);
BLASLONG   ixamin_k(BLASLONG, xdouble *, BLASLONG);

float   smax_k (BLASLONG, float  *, BLASLONG);
double  dmax_k (BLASLONG, double *, BLASLONG);
xdouble qmax_k (BLASLONG, xdouble *, BLASLONG);
float   cmax_k (BLASLONG, float  *, BLASLONG);
double  zmax_k (BLASLONG, double *, BLASLONG);
xdouble xmax_k (BLASLONG, xdouble *, BLASLONG);

float   smin_k (BLASLONG, float  *, BLASLONG);
double  dmin_k (BLASLONG, double *, BLASLONG);
xdouble qmin_k (BLASLONG, xdouble *, BLASLONG);
float   cmin_k (BLASLONG, float  *, BLASLONG);
double  zmin_k (BLASLONG, double *, BLASLONG);
xdouble xmin_k (BLASLONG, xdouble *, BLASLONG);

BLASLONG   ismax_k(BLASLONG, float  *, BLASLONG);
BLASLONG   idmax_k(BLASLONG, double *, BLASLONG);
BLASLONG   iqmax_k(BLASLONG, xdouble *, BLASLONG);
BLASLONG   icmax_k(BLASLONG, float  *, BLASLONG);
BLASLONG   izmax_k(BLASLONG, double *, BLASLONG);
BLASLONG   ixmax_k(BLASLONG, xdouble *, BLASLONG);

BLASLONG   ismin_k(BLASLONG, float  *, BLASLONG);
BLASLONG   idmin_k(BLASLONG, double *, BLASLONG);
BLASLONG   iqmin_k(BLASLONG, xdouble *, BLASLONG);
BLASLONG   icmin_k(BLASLONG, float  *, BLASLONG);
BLASLONG   izmin_k(BLASLONG, double *, BLASLONG);
BLASLONG   ixmin_k(BLASLONG, xdouble *, BLASLONG);

int    sscal_k(BLASLONG, BLASLONG, BLASLONG, float, 
	      float *, BLASLONG, float *, BLASLONG,  float  *, BLASLONG);
int    dscal_k(BLASLONG, BLASLONG, BLASLONG, double, 
	      double *, BLASLONG, double *, BLASLONG,  double  *, BLASLONG);
int    qscal_k(BLASLONG, BLASLONG, BLASLONG, xdouble, 
	      xdouble *, BLASLONG, xdouble *, BLASLONG,  xdouble  *, BLASLONG);
int    cscal_k(BLASLONG, BLASLONG, BLASLONG, float, float,
	      float *, BLASLONG, float *, BLASLONG,  float  *, BLASLONG);
int    zscal_k(BLASLONG, BLASLONG, BLASLONG, double, double,
	      double *, BLASLONG, double *, BLASLONG,  double  *, BLASLONG);
int    xscal_k(BLASLONG, BLASLONG, BLASLONG, xdouble, xdouble,
	      xdouble *, BLASLONG, xdouble *, BLASLONG,  xdouble  *, BLASLONG);
int    csscal_k(BLASLONG, BLASLONG, BLASLONG, float, float,
	       float *, BLASLONG, float *, BLASLONG,  float  *, BLASLONG);
int    zdscal_k(BLASLONG, BLASLONG, BLASLONG, double, double,
	       double *, BLASLONG, double *, BLASLONG,  double  *, BLASLONG);
int    xqscal_k(BLASLONG, BLASLONG, BLASLONG, xdouble, xdouble,
	       xdouble *, BLASLONG, xdouble *, BLASLONG,  xdouble  *, BLASLONG);

float   snrm2_k(BLASLONG, float   *, BLASLONG);
double  dnrm2_k(BLASLONG, double  *, BLASLONG);
xdouble qnrm2_k(BLASLONG, xdouble *, BLASLONG);
float   cnrm2_k(BLASLONG, float   *, BLASLONG);
double  znrm2_k(BLASLONG, double  *, BLASLONG);
xdouble xnrm2_k(BLASLONG, xdouble *, BLASLONG);

int    srot_k (BLASLONG, float  *, BLASLONG, float  *, BLASLONG, float , float );
int    drot_k (BLASLONG, double *, BLASLONG, double *, BLASLONG, double, double);
int    qrot_k (BLASLONG, xdouble *, BLASLONG, xdouble *, BLASLONG, xdouble, xdouble);
int    csrot_k(BLASLONG, float  *, BLASLONG, float  *, BLASLONG, float , float );
int    zdrot_k(BLASLONG, double *, BLASLONG, double *, BLASLONG, double, double);
int    xqrot_k(BLASLONG, xdouble *, BLASLONG, xdouble *, BLASLONG, xdouble, xdouble);

int    srotg_k(float  *, float  *, float  *, float  *);
int    drotg_k(double *, double *, double *, double *);
int    qrotg_k(xdouble *, xdouble *, xdouble *, xdouble *);
int    csrotg_k(float  *, float  *, float  *, float  *);
int    zdrotg_k(double *, double *, double *, double *);
int    xqrotg_k(xdouble *, xdouble *, xdouble *, xdouble *);

int    srotmg_k(float  *, float  *, float  *, float  *, float  *);
int    drotmg_k(double *, double *, double *, double *, double *);
int    qrotmg_k(xdouble *, xdouble *, xdouble *, xdouble *, xdouble *);

int    srotm_k (BLASLONG, float,  BLASLONG, float,  BLASLONG, float);
int    drotm_k (BLASLONG, double, BLASLONG, double, BLASLONG, double);
int    qrotm_k (BLASLONG, xdouble, BLASLONG, xdouble, BLASLONG, xdouble);

#ifdef __CUDACC__
}
#endif

#endif

