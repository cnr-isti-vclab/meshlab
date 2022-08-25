/////////////////////////////////////////////////////////////////////////////////
// 
//  Demonstration driver program for the Levenberg - Marquardt minimization
//  algorithm
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

/******************************************************************************** 
 * Levenberg-Marquardt minimization demo driver. Only the double precision versions
 * are tested here. See the Meyer case for an example of verifying the Jacobian 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "lm.h"

#ifndef LM_DBL_PREC
#error Demo program assumes that levmar has been compiled with double precision, see LM_DBL_PREC!
#endif


/* Sample functions to be minimized with LM and their Jacobians.
 * More test functions at http://www.csit.fsu.edu/~burkardt/f_src/test_nls/test_nls.html
 * Check also the CUTE problems collection at ftp://ftp.numerical.rl.ac.uk/pub/cute/;
 * CUTE is searchable through http://numawww.mathematik.tu-darmstadt.de:8081/opti/select.html
 * CUTE problems can also be solved through the AMPL web interface at http://www.ampl.com/TRYAMPL/startup.html
 *
 * Nonlinear optimization models in AMPL can be found at http://www.princeton.edu/~rvdb/ampl/nlmodels/
 */

#define ROSD 105.0

/* Rosenbrock function, global minimum at (1, 1) */
void ros(double *p, double *x, int m, int n, void *data)
{
register int i;

  for(i=0; i<n; ++i)
    x[i]=((1.0-p[0])*(1.0-p[0]) + ROSD*(p[1]-p[0]*p[0])*(p[1]-p[0]*p[0]));
}

void jacros(double *p, double *jac, int m, int n, void *data)
{
register int i, j;

  for(i=j=0; i<n; ++i){
    jac[j++]=(-2 + 2*p[0]-4*ROSD*(p[1]-p[0]*p[0])*p[0]);
    jac[j++]=(2*ROSD*(p[1]-p[0]*p[0]));
  }
}


#define MODROSLAM 1E02
/* Modified Rosenbrock problem, global minimum at (1, 1) */
void modros(double *p, double *x, int m, int n, void *data)
{
register int i;

  for(i=0; i<n; i+=3){
    x[i]=10*(p[1]-p[0]*p[0]);
	  x[i+1]=1.0-p[0];
	  x[i+2]=MODROSLAM;
  }
}

void jacmodros(double *p, double *jac, int m, int n, void *data)
{
register int i, j;

  for(i=j=0; i<n; i+=3){
    jac[j++]=-20.0*p[0];
	  jac[j++]=10.0;

	  jac[j++]=-1.0;
	  jac[j++]=0.0;

	  jac[j++]=0.0;
	  jac[j++]=0.0;
  }
}


/* Powell's function, minimum at (0, 0) */
void powell(double *p, double *x, int m, int n, void *data)
{
register int i;

  for(i=0; i<n; i+=2){
    x[i]=p[0];
    x[i+1]=10.0*p[0]/(p[0]+0.1) + 2*p[1]*p[1];
  }
}

void jacpowell(double *p, double *jac, int m, int n, void *data)
{
register int i, j;

  for(i=j=0; i<n; i+=2){
    jac[j++]=1.0;
    jac[j++]=0.0;

    jac[j++]=1.0/((p[0]+0.1)*(p[0]+0.1));
    jac[j++]=4.0*p[1];
  }
}

/* Wood's function, minimum at (1, 1, 1, 1) */
void wood(double *p, double *x, int m, int n, void *data)
{
register int i;

  for(i=0; i<n; i+=6){
    x[i]=10.0*(p[1] - p[0]*p[0]);
    x[i+1]=1.0 - p[0];
    x[i+2]=sqrt(90.0)*(p[3] - p[2]*p[2]);
    x[i+3]=1.0 - p[2];
    x[i+4]=sqrt(10.0)*(p[1]+p[3] - 2.0);
    x[i+5]=(p[1] - p[3])/sqrt(10.0);
  }
}

/* Meyer's (reformulated) problem, minimum at (2.48, 6.18, 3.45) */
void meyer(double *p, double *x, int m, int n, void *data)
{
register int i;
double ui;

	for(i=0; i<n; ++i){
		ui=0.45+0.05*i;
		x[i]=p[0]*exp(10.0*p[1]/(ui+p[2]) - 13.0);
	}
}

void jacmeyer(double *p, double *jac, int m, int n, void *data)
{
register int i, j;
double ui, tmp;

  for(i=j=0; i<n; ++i){
	  ui=0.45+0.05*i;
	  tmp=exp(10.0*p[1]/(ui+p[2]) - 13.0);

	  jac[j++]=tmp;
	  jac[j++]=10.0*p[0]*tmp/(ui+p[2]);
	  jac[j++]=-10.0*p[0]*p[1]*tmp/((ui+p[2])*(ui+p[2]));
  }
}

/* helical valley function, minimum at (1.0, 0.0, 0.0) */
#ifndef M_PI
#define M_PI   3.14159265358979323846  /* pi */
#endif

void helval(double *p, double *x, int m, int n, void *data)
{
double theta;

  if(p[0]<0.0)
     theta=atan(p[1]/p[0])/(2.0*M_PI) + 0.5;
  else if(0.0<p[0])
     theta=atan(p[1]/p[0])/(2.0*M_PI);
  else 
    theta=(p[1]>=0)? 0.25 : -0.25;

  x[0]=10.0*(p[2] - 10.0*theta);
  x[1]=10.0*(sqrt(p[0]*p[0] + p[1]*p[1]) - 1.0);
  x[2]=p[2];
}

void jachelval(double *p, double *jac, int m, int n, void *data)
{
register int i=0;
double tmp;

  tmp=p[0]*p[0] + p[1]*p[1];

  jac[i++]=50.0*p[1]/(M_PI*tmp);
  jac[i++]=-50.0*p[0]/(M_PI*tmp);
  jac[i++]=10.0;

  jac[i++]=10.0*p[0]/sqrt(tmp);
  jac[i++]=10.0*p[1]/sqrt(tmp);
  jac[i++]=0.0;

  jac[i++]=0.0;
  jac[i++]=0.0;
  jac[i++]=1.0;
}

/* Boggs - Tolle problem 3 (linearly constrained), minimum at (-0.76744, 0.25581, 0.62791, -0.11628, 0.25581)
 * constr1: p[0] + 3*p[1] = 0;
 * constr2: p[2] + p[3] - 2*p[4] = 0;
 * constr3: p[1] - p[4] = 0;
 */
void bt3(double *p, double *x, int m, int n, void *data)
{
register int i;
double t1, t2, t3, t4;

  t1=p[0]-p[1];
  t2=p[1]+p[2]-2.0;
  t3=p[3]-1.0;
  t4=p[4]-1.0;

  for(i=0; i<n; ++i)
    x[i]=t1*t1 + t2*t2 + t3*t3 + t4*t4;
}

void jacbt3(double *p, double *jac, int m, int n, void *data)
{
register int i, j;
double t1, t2, t3, t4;

  t1=p[0]-p[1];
  t2=p[1]+p[2]-2.0;
  t3=p[3]-1.0;
  t4=p[4]-1.0;

  for(i=j=0; i<n; ++i){
    jac[j++]=2.0*t1;
    jac[j++]=2.0*(t2-t1);
    jac[j++]=2.0*t2;
    jac[j++]=2.0*t3;
    jac[j++]=2.0*t4;
  }
}

/* Hock - Schittkowski problem 28 (linearly constrained), minimum at (0.5, -0.5, 0.5)
 * constr1: p[0] + 2*p[1] + 3*p[2] = 1;
 */
void hs28(double *p, double *x, int m, int n, void *data)
{
register int i;
double t1, t2;

  t1=p[0]+p[1];
  t2=p[1]+p[2];

  for(i=0; i<n; ++i)
    x[i]=t1*t1 + t2*t2;
}

void jachs28(double *p, double *jac, int m, int n, void *data)
{
register int i, j;
double t1, t2;

  t1=p[0]+p[1];
  t2=p[1]+p[2];

  for(i=j=0; i<n; ++i){
    jac[j++]=2.0*t1;
    jac[j++]=2.0*(t1+t2);
    jac[j++]=2.0*t2;
  }
}

/* Hock - Schittkowski problem 48 (linearly constrained), minimum at (1.0, 1.0, 1.0, 1.0, 1.0)
 * constr1: sum {i in 0..4} p[i] = 5;
 * constr2: p[2] - 2*(p[3]+p[4]) = -3;
 */
void hs48(double *p, double *x, int m, int n, void *data)
{
register int i;
double t1, t2, t3;

  t1=p[0]-1.0;
  t2=p[1]-p[2];
  t3=p[3]-p[4];

  for(i=0; i<n; ++i)
    x[i]=t1*t1 + t2*t2 + t3*t3;
}

void jachs48(double *p, double *jac, int m, int n, void *data)
{
register int i, j;
double t1, t2, t3;

  t1=p[0]-1.0;
  t2=p[1]-p[2];
  t3=p[3]-p[4];

  for(i=j=0; i<n; ++i){
    jac[j++]=2.0*t1;
    jac[j++]=2.0*t2;
    jac[j++]=-2.0*t2;
    jac[j++]=2.0*t3;
    jac[j++]=-2.0*t3;
  }
}

/* Hock - Schittkowski problem 51 (linearly constrained), minimum at (1.0, 1.0, 1.0, 1.0, 1.0)
 * constr1: p[0] + 3*p[1] = 4;
 * constr2: p[2] + p[3] - 2*p[4] = 0;
 * constr3: p[1] - p[4] = 0;
 */
void hs51(double *p, double *x, int m, int n, void *data)
{
register int i;
double t1, t2, t3, t4;

  t1=p[0]-p[1];
  t2=p[1]+p[2]-2.0;
  t3=p[3]-1.0;
  t4=p[4]-1.0;

  for(i=0; i<n; ++i)
    x[i]=t1*t1 + t2*t2 + t3*t3 + t4*t4;
}

void jachs51(double *p, double *jac, int m, int n, void *data)
{
register int i, j;
double t1, t2, t3, t4;

  t1=p[0]-p[1];
  t2=p[1]+p[2]-2.0;
  t3=p[3]-1.0;
  t4=p[4]-1.0;

  for(i=j=0; i<n; ++i){
    jac[j++]=2.0*t1;
    jac[j++]=2.0*(t2-t1);
    jac[j++]=2.0*t2;
    jac[j++]=2.0*t3;
    jac[j++]=2.0*t4;
  }
}

/* Hock - Schittkowski problem 01 (box constrained), minimum at (1.0, 1.0)
 * constr1: p[1]>=-1.5;
 */
void hs01(double *p, double *x, int m, int n, void *data)
{
double t;

  t=p[0]*p[0];
  x[0]=10.0*(p[1]-t);
  x[1]=1.0-p[0];
}

void jachs01(double *p, double *jac, int m, int n, void *data)
{
register int j=0;

  jac[j++]=-20.0*p[0];
  jac[j++]=10.0;

  jac[j++]=-1.0;
  jac[j++]=0.0;
}

/* Hock - Schittkowski MODIFIED problem 21 (box constrained), minimum at (2.0, 0.0)
 * constr1: 2 <= p[0] <=50;
 * constr2: -50 <= p[1] <=50;
 *
 * Original HS21 has the additional constraint 10*p[0] - p[1] >= 10; which is inactive
 * at the solution, so it is dropped here.
 */
void hs21(double *p, double *x, int m, int n, void *data)
{
  x[0]=p[0]/10.0;
  x[1]=p[1];
}

void jachs21(double *p, double *jac, int m, int n, void *data)
{
register int j=0;

  jac[j++]=0.1;
  jac[j++]=0.0;

  jac[j++]=0.0;
  jac[j++]=1.0;
}

/* Problem hatfldb (box constrained), minimum at (0.947214, 0.8, 0.64, 0.4096)
 * constri: p[i]>=0.0; (i=1..4)
 * constr5: p[1]<=0.8;
 */
void hatfldb(double *p, double *x, int m, int n, void *data)
{
register int i;

  x[0]=p[0]-1.0;

  for(i=1; i<m; ++i)
     x[i]=p[i-1]-sqrt(p[i]);
}

void jachatfldb(double *p, double *jac, int m, int n, void *data)
{
register int j=0;

  jac[j++]=1.0;
  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=0.0;

  jac[j++]=1.0;
  jac[j++]=-0.5/sqrt(p[1]);
  jac[j++]=0.0;
  jac[j++]=0.0;

  jac[j++]=0.0;
  jac[j++]=1.0;
  jac[j++]=-0.5/sqrt(p[2]);
  jac[j++]=0.0;

  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=1.0;
  jac[j++]=-0.5/sqrt(p[3]);
}

/* Problem hatfldc (box constrained), minimum at (1.0, 1.0, 1.0, 1.0)
 * constri: p[i]>=0.0; (i=1..4)
 * constri+4: p[i]<=10.0; (i=1..4)
 */
void hatfldc(double *p, double *x, int m, int n, void *data)
{
register int i;

  x[0]=p[0]-1.0;

  for(i=1; i<m-1; ++i)
     x[i]=p[i-1]-sqrt(p[i]);

  x[m-1]=p[m-1]-1.0;
}

void jachatfldc(double *p, double *jac, int m, int n, void *data)
{
register int j=0;

  jac[j++]=1.0;
  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=0.0;

  jac[j++]=1.0;
  jac[j++]=-0.5/sqrt(p[1]);
  jac[j++]=0.0;
  jac[j++]=0.0;

  jac[j++]=0.0;
  jac[j++]=1.0;
  jac[j++]=-0.5/sqrt(p[2]);
  jac[j++]=0.0;

  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=1.0;
}

/* Hock - Schittkowski (modified) problem 52 (box/linearly constrained), minimum at (-0.09, 0.03, 0.25, -0.19, 0.03)
 * constr1: p[0] + 3*p[1] = 0;
 * constr2: p[2] +   p[3] - 2*p[4] = 0;
 * constr3: p[1] -   p[4] = 0;
 *
 * To the above 3 constraints, we add the following 5:
 * constr4: -0.09 <= p[0];
 * constr5:   0.0 <= p[1] <= 0.3;
 * constr6:          p[2] <= 0.25;
 * constr7:  -0.2 <= p[3] <= 0.3;
 * constr8:   0.0 <= p[4] <= 0.3;
 *
 */
void modhs52(double *p, double *x, int m, int n, void *data)
{
  x[0]=4.0*p[0]-p[1];
  x[1]=p[1]+p[2]-2.0;
  x[2]=p[3]-1.0;
  x[3]=p[4]-1.0;
}

void jacmodhs52(double *p, double *jac, int m, int n, void *data)
{
register int j=0;

  jac[j++]=4.0;
  jac[j++]=-1.0;
  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=0.0;

  jac[j++]=0.0;
  jac[j++]=1.0;
  jac[j++]=1.0;
  jac[j++]=0.0;
  jac[j++]=0.0;

  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=1.0;
  jac[j++]=0.0;

  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=0.0;
  jac[j++]=1.0;
}

/* Schittkowski (modified) problem 235 (box/linearly constrained), minimum at (-1.725, 2.9, 0.725)
 * constr1: p[0] + p[2] = -1.0;
 *
 * To the above constraint, we add the following 2:
 * constr2: p[1] - 4*p[2] = 0;
 * constr3: 0.1 <= p[1] <= 2.9;
 * constr4: 0.7 <= p[2];
 *
 */
void mods235(double *p, double *x, int m, int n, void *data)
{
  x[0]=0.1*(p[0]-1.0);
  x[1]=p[1]-p[0]*p[0];
}

void jacmods235(double *p, double *jac, int m, int n, void *data)
{
register int j=0;

  jac[j++]=0.1;
  jac[j++]=0.0;
  jac[j++]=0.0;

  jac[j++]=-2.0*p[0];
  jac[j++]=1.0;
  jac[j++]=0.0;
}

/* Boggs and Tolle modified problem 7 (box/linearly constrained), minimum at (0.7, 0.49, 0.19, 1.19, -0.2)
 * We keep the original objective function & starting point and use the following constraints:
 *
 * subject to cons1:
 *  x[1]+x[2] - x[3] = 1.0;
 * subject to cons2:
 *   x[2] - x[4] + x[1] = 0.0;
 * subject to cons3:
 *   x[5] + x[1] = 0.5;
 * subject to cons4:
 *   x[5]>=-0.3;
 * subject to cons5:
 *    x[1]<=0.7;
 *
 */
void modbt7(double *p, double *x, int m, int n, void *data)
{
register int i;

  for(i=0; i<n; ++i)
    x[i]=100.0*(p[1]-p[0]*p[0])*(p[1]-p[0]*p[0]) + (p[0]-1.0)*(p[0]-1.0);
}

void jacmodbt7(double *p, double *jac, int m, int n, void *data)
{
register int i, j;

  for(i=j=0; i<m; ++i){
    jac[j++]=-400.0*(p[1]-p[0]*p[0])*p[0] + 2.0*p[0] - 2.0;
    jac[j++]=200.0*(p[1]-p[0]*p[0]);
    jac[j++]=0.0;
    jac[j++]=0.0;
    jac[j++]=0.0;
  }
}

/* Equilibrium combustion problem, constrained nonlinear equation from the book by Floudas et al.
 * Minimum at (0.0034, 31.3265, 0.0684, 0.8595, 0.0370)
 * constri: p[i]>=0.0001; (i=1..5)
 * constri+5: p[i]<=100.0; (i=1..5)
 */
void combust(double *p, double *x, int m, int n, void *data)
{
  double R, R5, R6, R7, R8, R9, R10;

  R=10;
  R5=0.193;
  R6=4.10622*1e-4;
  R7=5.45177*1e-4;
  R8=4.4975*1e-7;
  R9=3.40735*1e-5;
  R10=9.615*1e-7;

  x[0]=p[0]*p[1]+p[0]-3*p[4];
  x[1]=2*p[0]*p[1]+p[0]+3*R10*p[1]*p[1]+p[1]*p[2]*p[2]+R7*p[1]*p[2]+R9*p[1]*p[3]+R8*p[1]-R*p[4];
  x[2]=2*p[1]*p[2]*p[2]+R7*p[1]*p[2]+2*R5*p[2]*p[2]+R6*p[2]-8*p[4];
  x[3]=R9*p[1]*p[3]+2*p[3]*p[3]-4*R*p[4];
  x[4]=p[0]*p[1]+p[0]+R10*p[1]*p[1]+p[1]*p[2]*p[2]+R7*p[1]*p[2]+R9*p[1]*p[3]+R8*p[1]+R5*p[2]*p[2]+R6*p[2]+p[3]*p[3]-1.0;
}

void jaccombust(double *p, double *jac, int m, int n, void *data)
{
register int j=0;
  double R, R5, R6, R7, R8, R9, R10;

  R=10;
  R5=0.193;
  R6=4.10622*1e-4;
  R7=5.45177*1e-4;
  R8=4.4975*1e-7;
  R9=3.40735*1e-5;
  R10=9.615*1e-7;

  for(j=0; j<m*n; ++j) jac[j]=0.0;

  j=0;
  jac[j]=p[1]+1;
  jac[j+1]=p[0];
  jac[j+4]=-3;

  j+=m;
  jac[j]=2*p[1]+1;
  jac[j+1]=2*p[0]+6*R10*p[1]+p[2]*p[2]+R7*p[2]+R9*p[3]+R8;
  jac[j+2]=2*p[1]*p[2]+R7*p[1];
  jac[j+3]=R9*p[1];
  jac[j+4]=-R;

  j+=m;
  jac[j+1]=2*p[2]*p[2]+R7*p[2];
  jac[j+2]=4*p[1]*p[2]+R7*p[1]+4*R5*p[2]+R6;
  jac[j+4]=-8;

  j+=m;
  jac[j+1]=R9*p[3];
  jac[j+3]=R9*p[1]+4*p[3];
  jac[j+4]=-4*R;

  j+=m;
  jac[j]=p[1]+1;
  jac[j+1]=p[0]+2*R10*p[1]+p[2]*p[2]+R7*p[2]+R9*p[3]+R8;
  jac[j+2]=2*p[1]*p[2]+R7*p[1]+2*R5*p[2]+R6;
  jac[j+3]=R9*p[1]+2*p[3];
}



int main()
{
register int i, j;
int problem, ret;
double p[5], // 6 is max(2, 3, 5)
	   x[16]; // 16 is max(2, 3, 5, 6, 16)
int m, n;
double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
char *probname[]={
    "Rosenbrock function",
    "modified Rosenbrock problem",
    "Powell's function",
    "Wood's function",
    "Meyer's (reformulated) problem",
    "helical valley function",
    "Boggs & Tolle's problem #3",
    "Hock - Schittkowski problem #28",
    "Hock - Schittkowski problem #48",
    "Hock - Schittkowski problem #51",
    "Hock - Schittkowski problem #01",
    "Hock - Schittkowski modified problem #21",
    "hatfldb problem",
    "hatfldc problem",
    "equilibrium combustion problem",
    "Hock - Schittkowski modified problem #52",
    "Schittkowski modified problem #235",
    "Boggs & Tolle modified problem #7",
};

  opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20;
  opts[4]=LM_DIFF_DELTA; // relevant only if the finite difference Jacobian version is used 

  /* uncomment the appropriate line below to select a minimization problem */
  problem=
		  //0; // Rosenbrock function
		  //1; // modified Rosenbrock problem
		  //2; // Powell's function
      //3; // Wood's function
		  4; // Meyer's (reformulated) problem
      //5; // helical valley function
#ifdef HAVE_LAPACK
      //6; // Boggs & Tolle's problem 3
      //7; // Hock - Schittkowski problem 28
      //8; // Hock - Schittkowski problem 48
      //9; // Hock - Schittkowski problem 51
#else // no LAPACK
#ifdef _MSC_VER
#pragma message("LAPACK not available, some test problems cannot be used")
#else
#warning LAPACK not available, some test problems cannot be used
#endif // _MSC_VER

#endif /* HAVE_LAPACK */
      //10; // Hock - Schittkowski problem 01
      //11; // Hock - Schittkowski modified problem 21
      //12; // hatfldb problem
      //13; // hatfldc problem
      //14; // equilibrium combustion problem
#ifdef HAVE_LAPACK
      //15; // Hock - Schittkowski modified problem 52
      //16; // Schittkowski modified problem 235
      //17; // Boggs & Tolle modified problem #7
#endif /* HAVE_LAPACK */
				
  switch(problem){
  default: fprintf(stderr, "unknown problem specified (#%d)! Note that some minimization problems require LAPACK.\n", problem);
           exit(1);
    break;
  case 0:
  /* Rosenbrock function */
    m=2; n=2;
    p[0]=-1.2; p[1]=1.0;
    for(i=0; i<n; i++) x[i]=0.0;
    ret=dlevmar_der(ros, jacros, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    //ret=dlevmar_dif(ros, p, x, m, n, 1000, opts, info, NULL, NULL, NULL);  // no Jacobian
  break;

  case 1:
  /* modified Rosenbrock problem */
    m=2; n=3;
    p[0]=-1.2; p[1]=1.0;
    for(i=0; i<n; i++) x[i]=0.0;
    ret=dlevmar_der(modros, jacmodros, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    //ret=dlevmar_dif(modros, p, x, m, n, 1000, opts, info, NULL, NULL, NULL);  // no Jacobian
  break;

  case 2:
  /* Powell's function */
    m=2; n=2;
    p[0]=3.0; p[1]=1.0;
    for(i=0; i<n; i++) x[i]=0.0;
    ret=dlevmar_der(powell, jacpowell, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    //ret=dlevmar_dif(powell, p, x, m, n, 1000, opts, info, NULL, NULL, NULL);		// no Jacobian
  break;

  case 3:
  /* Wood's function */
    m=4; n=6;
    p[0]=-3.0; p[1]=-1.0; p[2]=-3.0; p[3]=-1.0;
    for(i=0; i<n; i++) x[i]=0.0;
    ret=dlevmar_dif(wood, p, x, m, n, 1000, opts, info, NULL, NULL, NULL);  // no Jacobian
  break;

  case 4:
  /* Meyer's data fitting problem */
    m=3; n=16;
    p[0]=8.85; p[1]=4.0; p[2]=2.5;
    x[0]=34.780;	x[1]=28.610; x[2]=23.650; x[3]=19.630;
    x[4]=16.370;	x[5]=13.720; x[6]=11.540; x[7]=9.744;
    x[8]=8.261;	x[9]=7.030; x[10]=6.005; x[11]=5.147;
    x[12]=4.427;	x[13]=3.820; x[14]=3.307; x[15]=2.872;
    //ret=dlevmar_der(meyer, jacmeyer, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian

   { double *work, *covar;
    work=malloc((LM_DIF_WORKSZ(m, n)+m*m)*sizeof(double));
    if(!work){
    	fprintf(stderr, "memory allocation request failed in main()\n");
      exit(1);
    }
    covar=work+LM_DIF_WORKSZ(m, n);

    ret=dlevmar_dif(meyer, p, x, m, n, 1000, opts, info, work, covar, NULL); // no Jacobian, caller allocates work memory, covariance estimated

    printf("Covariance of the fit:\n");
    for(i=0; i<m; ++i){
      for(j=0; j<m; ++j)
        printf("%g ", covar[i*m+j]);
      printf("\n");
    }
    printf("\n");

    free(work);
   }

/* uncomment the following block to verify Jacobian */
/*
   {
    double err[16];
    dlevmar_chkjac(meyer, jacmeyer, p, m, n, NULL, err); 
    for(i=0; i<n; ++i) printf("gradient %d, err %g\n", i, err[i]);
   }
*/

  break;

  case 5:
  /* helical valley function */
    m=3; n=3;
    p[0]=-1.0; p[1]=0.0; p[2]=0.0;
    for(i=0; i<n; i++) x[i]=0.0;
    ret=dlevmar_der(helval, jachelval, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    //ret=dlevmar_dif(helval, p, x, m, n, 1000, opts, info, NULL, NULL, NULL);  // no Jacobian
  break;

#ifdef HAVE_LAPACK
  case 6:
  /* Boggs-Tolle problem 3 */
    m=5; n=5;
    p[0]=2.0; p[1]=2.0; p[2]=2.0;
    p[3]=2.0; p[4]=2.0;
    for(i=0; i<n; i++) x[i]=0.0;

    {
      double A[3*5]={1.0, 3.0, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0, 1.0, -2.0,  0.0, 1.0, 0.0, 0.0, -1.0},
             b[3]={0.0, 0.0, 0.0};

    ret=dlevmar_lec_der(bt3, jacbt3, p, x, m, n, A, b, 3, 1000, opts, info, NULL, NULL, NULL); // lin. constraints, analytic Jacobian
    //ret=dlevmar_lec_dif(bt3, p, x, m, n, A, b, 3, 1000, opts, info, NULL, NULL, NULL); // lin. constraints, no Jacobian
    }
  break;
  case 7:
  /* Hock - Schittkowski problem 28 */
    m=3; n=3;
    p[0]=-4.0; p[1]=1.0; p[2]=1.0;
    for(i=0; i<n; i++) x[i]=0.0;

    {
      double A[1*3]={1.0, 2.0, 3.0},
             b[1]={1.0};

    ret=dlevmar_lec_der(hs28, jachs28, p, x, m, n, A, b, 1, 1000, opts, info, NULL, NULL, NULL); // lin. constraints, analytic Jacobian
    //ret=dlevmar_lec_dif(hs28, p, x, m, n, A, b, 1, 1000, opts, info, NULL, NULL, NULL); // lin. constraints, no Jacobian
    }
  break;
  case 8:
  /* Hock - Schittkowski problem 48 */
    m=5; n=5;
    p[0]=3.0; p[1]=5.0; p[2]=-3.0;
    p[3]=2.0; p[4]=-2.0;
    for(i=0; i<n; i++) x[i]=0.0;

    {
      double A[2*5]={1.0, 1.0, 1.0, 1.0, 1.0,  0.0, 0.0, 1.0, -2.0, -2.0},
             b[2]={5.0, -3.0};

    ret=dlevmar_lec_der(hs48, jachs48, p, x, m, n, A, b, 2, 1000, opts, info, NULL, NULL, NULL); // lin. constraints, analytic Jacobian
    //ret=dlevmar_lec_dif(hs48, p, x, m, n, A, b, 2, 1000, opts, info, NULL, NULL, NULL); // lin. constraints, no Jacobian
    }
  break;
  case 9:
  /* Hock - Schittkowski problem 51 */
    m=5; n=5;
    p[0]=2.5; p[1]=0.5; p[2]=2.0;
    p[3]=-1.0; p[4]=0.5;
    for(i=0; i<n; i++) x[i]=0.0;

    {
      double A[3*5]={1.0, 3.0, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0, 1.0, -2.0,  0.0, 1.0, 0.0, 0.0, -1.0},
             b[3]={4.0, 0.0, 0.0};

    ret=dlevmar_lec_der(hs51, jachs51, p, x, m, n, A, b, 3, 1000, opts, info, NULL, NULL, NULL); // lin. constraints, analytic Jacobian
    //ret=dlevmar_lec_dif(hs51, p, x, m, n, A, b, 3, 1000, opts, info, NULL, NULL, NULL); // lin. constraints, no Jacobian
    }
  break;
#endif /* HAVE_LAPACK */

  case 10:
  /* Hock - Schittkowski problem 01 */
    m=2; n=2;
    p[0]=-2.0; p[1]=1.0;
    for(i=0; i<n; i++) x[i]=0.0;
    //ret=dlevmar_der(hs01, jachs01, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    {
      double lb[2], ub[2];

      lb[0]=-DBL_MAX; lb[1]=-1.5;
      ub[0]=ub[1]=DBL_MAX;

      ret=dlevmar_bc_der(hs01, jachs01, p, x, m, n, lb, ub, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    }
    break;
  case 11:
  /* Hock - Schittkowski (modified) problem 21 */
    m=2; n=2;
    p[0]=-1.0; p[1]=-1.0;
    for(i=0; i<n; i++) x[i]=0.0;
    //ret=dlevmar_der(hs21, jachs21, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    {
      double lb[2], ub[2];

      lb[0]=2.0; lb[1]=-50.0;
      ub[0]=50.0; ub[1]=50.0;

      ret=dlevmar_bc_der(hs21, jachs21, p, x, m, n, lb, ub, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    }
    break;
  case 12:
  /* hatfldb problem */
    m=4; n=4;
    p[0]=p[1]=p[2]=p[3]=0.1;
    for(i=0; i<n; i++) x[i]=0.0;
    //ret=dlevmar_der(hatfldb, jachatfldb, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    {
      double lb[4], ub[4];

      lb[0]=lb[1]=lb[2]=lb[3]=0.0;

      ub[0]=ub[2]=ub[3]=DBL_MAX;
      ub[1]=0.8;

      ret=dlevmar_bc_der(hatfldb, jachatfldb, p, x, m, n, lb, ub, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    }
    break;
  case 13:
  /* hatfldc problem */
    m=4; n=4;
    p[0]=p[1]=p[2]=p[3]=0.9;
    for(i=0; i<n; i++) x[i]=0.0;
    //ret=dlevmar_der(hatfldc, jachatfldc, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    {
      double lb[4], ub[4];

      lb[0]=lb[1]=lb[2]=lb[3]=0.0;

      ub[0]=ub[1]=ub[2]=ub[3]=10.0;

      ret=dlevmar_bc_der(hatfldc, jachatfldc, p, x, m, n, lb, ub, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    }
    break;
  case 14:
  /* equilibrium combustion problem */
    m=5; n=5;
    p[0]=p[1]=p[2]=p[3]=p[4]=0.0001;
    for(i=0; i<n; i++) x[i]=0.0;
    //ret=dlevmar_der(combust, jaccombust, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    {
      double lb[5], ub[5];

      lb[0]=lb[1]=lb[2]=lb[3]=lb[4]=0.0001;

      ub[0]=ub[1]=ub[2]=ub[3]=ub[4]=100.0;

      ret=dlevmar_bc_der(combust, jaccombust, p, x, m, n, lb, ub, 5000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
    }
    break;
#ifdef HAVE_LAPACK
  case 15:
  /* Hock - Schittkowski modified problem 52 */
    m=5; n=4;
    p[0]=2.0; p[1]=2.0; p[2]=2.0;
    p[3]=2.0; p[4]=2.0;
    for(i=0; i<n; i++) x[i]=0.0;

    {
      double A[3*5]={1.0, 3.0, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0, 1.0, -2.0,  0.0, 1.0, 0.0, 0.0, -1.0},
             b[3]={0.0, 0.0, 0.0};

      double lb[5], ub[5];

      double weights[5]={2000.0, 2000.0, 2000.0, 2000.0, 2000.0}; // penalty terms weights

      lb[0]=-0.09; lb[1]=0.0; lb[2]=-DBL_MAX; lb[3]=-0.2; lb[4]=0.0;
      ub[0]=DBL_MAX; ub[1]=0.3; ub[2]=0.25; ub[3]=0.3; ub[4]=0.3;

      ret=dlevmar_blec_der(modhs52, jacmodhs52, p, x, m, n, lb, ub, A, b, 3, weights, 1000, opts, info, NULL, NULL, NULL); // box & lin. constraints, analytic Jacobian
      //ret=dlevmar_blec_dif(modhs52, p, x, m, n, lb, ub, A, b, 3, weights, 1000, opts, info, NULL, NULL, NULL); // box & lin. constraints, no Jacobian
    }
    break;
  case 16:
  /* Schittkowski modified problem 235 */
    m=3; n=2;
    p[0]=-2.0; p[1]=3.0; p[2]=1.0;
    for(i=0; i<n; i++) x[i]=0.0;

    {
      double A[2*3]={1.0, 0.0, 1.0,  0.0, 1.0, -4.0},
             b[2]={-1.0, 0.0};

      double lb[3], ub[3];

      lb[0]=-DBL_MAX; lb[1]=0.1; lb[2]=0.7;
      ub[0]=DBL_MAX; ub[1]=2.9; ub[2]=DBL_MAX;

      ret=dlevmar_blec_der(mods235, jacmods235, p, x, m, n, lb, ub, A, b, 2, NULL, 1000, opts, info, NULL, NULL, NULL); // box & lin. constraints, analytic Jacobian
      //ret=dlevmar_blec_dif(mods235, p, x, m, n, lb, ub, A, b, 2, NULL, 1000, opts, info, NULL, NULL, NULL); // box & lin. constraints, no Jacobian
    }
    break;
  case 17:
  /* Boggs & Tolle modified problem 7 */
    m=5; n=5;
    p[0]=-2.0; p[1]=1.0; p[2]=1.0; p[3]=1.0; p[4]=1.0;
    for(i=0; i<n; i++) x[i]=0.0;

    {
      double A[3*5]={1.0, 1.0, -1.0, 0.0, 0.0,   1.0, 1.0, 0.0, -1.0, 0.0,   1.0, 0.0, 0.0, 0.0, 1.0},
             b[3]={1.0, 0.0, 0.5};

      double lb[5], ub[5];

      lb[0]=-DBL_MAX; lb[1]=-DBL_MAX; lb[2]=-DBL_MAX; lb[3]=-DBL_MAX; lb[4]=-0.3;
      ub[0]=0.7;      ub[1]= DBL_MAX; ub[2]= DBL_MAX; ub[3]= DBL_MAX; ub[4]=DBL_MAX;

      ret=dlevmar_blec_der(modbt7, jacmodbt7, p, x, m, n, lb, ub, A, b, 3, NULL, 1000, opts, info, NULL, NULL, NULL); // box & lin. constraints, analytic Jacobian
      //ret=dlevmar_blec_dif(modbt7, p, x, m, n, lb, ub, A, b, 3, NULL, 10000, opts, info, NULL, NULL, NULL); // box & lin. constraints, no Jacobian
    }
    break;
#endif /* HAVE_LAPACK */
  } /* switch */
  
  printf("Results for %s:\n", probname[problem]);
  printf("Levenberg-Marquardt returned %d in %g iter, reason %g\nSolution: ", ret, info[5], info[6]);
  for(i=0; i<m; ++i)
    printf("%.7g ", p[i]);
  printf("\n\nMinimization info:\n");
  for(i=0; i<LM_INFO_SZ; ++i)
    printf("%g ", info[i]);
  printf("\n");

  return 0;
}
