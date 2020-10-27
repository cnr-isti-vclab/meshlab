/*******************************************************************************\
*                                                                               *
* This file contains routines for transforming between the different coordinate *
* systems used in Tsai's perspective projection camera model.  The routines     *
* are:                                                                          *
*                                                                               *
*       world_coord_to_image_coord ()                                           *
*       image_coord_to_world_coord ()                                           *
*       world_coord_to_camera_coord ()                                          *
*       camera_coord_to_world_coord ()                                          *
*       distorted_to_undistorted_sensor_coord ()                                *
*       undistorted_to_distorted_sensor_coord ()                                *
*       distorted_to_undistorted_image_coord ()                                 *
*       undistorted_to_distorted_image_coord ()                                 *
*                                                                               *
* The routines make use of the calibrated camera parameters and calibration     *
* constants contained in the two external data structures cp and cc.            *
*                                                                               *
* Notation                                                                      *
* --------                                                                      *
*                                                                               *
* The camera's X axis runs along increasing column coordinates in the           *
* image/frame.  The Y axis runs along increasing row coordinates.               *
* All 3D coordinates are right-handed.                                          *
*                                                                               *
* pix == image/frame grabber picture element                                    *
* sel == camera sensor element                                                  *
*                                                                               *
*                                                                               *
* History                                                                       *
* -------                                                                       *
*                                                                               *
* 18-Oct-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Added check in undistorted_to_distorted_sensor_coord () for situation   *
*       where an undistorted sensor point maps to the maximum barrel distortion *
*       radius.                                                                 *
*                                                                               *
* 18-May-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Split out from the cal_main.c file.                                     *
*       Fix the CBRT routine so it handles negative arguments properly.         *
*                                                                               *
\*******************************************************************************/
#include <stdio.h>
#include <math.h>
#include "cal_main.h"


extern struct camera_parameters cp;
extern struct calibration_constants cc;


#define SQRT(x) sqrt(fabs(x))


/************************************************************************/
/* This cube root routine handles negative arguments (unlike cbrt).     */

double    CBRT (x)
    double    x;
{
    double    pow ();

    if (x == 0)
	return (0);
    else if (x > 0)
	return (pow (x, (double) 1.0 / 3.0));
    else
	return (-pow (-x, (double) 1.0 / 3.0));
} 


/************************************************************************/
/*
       This routine converts from undistorted to distorted sensor coordinates.
       The technique involves algebraically solving the cubic polynomial

            Ru = Rd * (1 + kappa1 * Rd**2)

       using the Cardan method.

       Note: for kappa1 < 0 the distorted sensor plane extends out to a maximum
             barrel distortion radius of  Rd = sqrt (-1/(3 * kappa1)).

	     To see the logic used in this routine try graphing the above polynomial
	     for positive and negative kappa1's
*/

void      undistorted_to_distorted_sensor_coord (Xu, Yu, Xd, Yd)
    double    Xu,
              Yu,
             *Xd,
             *Yd;
{
#define SQRT3   1.732050807568877293527446341505872366943

    double    Ru,
              Rd,
              lambda,
              c,
              d,
              Q,
              R,
              D,
              S,
              T,
              sinT,
              cosT;

    if (((Xu == 0) && (Yu == 0)) || (cc.kappa1 == 0)) {
	*Xd = Xu;
	*Yd = Yu;
	return;
    }

    Ru = hypot (Xu, Yu);	/* SQRT(Xu*Xu+Yu*Yu) */

    c = 1 / cc.kappa1;
    d = -c * Ru;

    Q = c / 3;
    R = -d / 2;
    D = CUB (Q) + SQR (R);

    if (D >= 0) {		/* one real root */
	D = SQRT (D);
	S = CBRT (R + D);
	T = CBRT (R - D);
	Rd = S + T;

	if (Rd < 0) {
	    Rd = SQRT (-1 / (3 * cc.kappa1));
	    fprintf (stderr, "\nWarning: undistorted image point to distorted image point mapping limited by\n");
	    fprintf (stderr, "         maximum barrel distortion radius of %lf\n", Rd);
	    fprintf (stderr, "         (Xu = %lf, Yu = %lf) -> (Xd = %lf, Yd = %lf)\n\n",
		     Xu, Yu, Xu * Rd / Ru, Yu * Rd / Ru);
	}
    } else {			/* three real roots */
	D = SQRT (-D);
	S = CBRT (hypot (R, D));
	T = atan2 (D, R) / 3;
	SINCOS (T, sinT, cosT);

	/* the larger positive root is    2*S*cos(T)                   */
	/* the smaller positive root is   -S*cos(T) + SQRT(3)*S*sin(T) */
	/* the negative root is           -S*cos(T) - SQRT(3)*S*sin(T) */

	Rd = -S * cosT + SQRT3 * S * sinT;	/* use the smaller positive root */
    }

    lambda = Rd / Ru;

    *Xd = Xu * lambda;
    *Yd = Yu * lambda;
}


/************************************************************************/
void      distorted_to_undistorted_sensor_coord (Xd, Yd, Xu, Yu)
    double    Xd,
              Yd,
             *Xu,
             *Yu;
{
    double    distortion_factor;

    /* convert from distorted to undistorted sensor plane coordinates */
    distortion_factor = 1 + cc.kappa1 * (SQR (Xd) + SQR (Yd));
    *Xu = Xd * distortion_factor;
    *Yu = Yd * distortion_factor;
}


/************************************************************************/
void      undistorted_to_distorted_image_coord (Xfu, Yfu, Xfd, Yfd)
    double    Xfu,
              Yfu,
             *Xfd,
             *Yfd;
{
    double    Xu,
              Yu,
              Xd,
              Yd;

    /* convert from image to sensor coordinates */
    Xu = cp.dpx * (Xfu - cp.Cx) / cp.sx;
    Yu = cp.dpy * (Yfu - cp.Cy);

    /* convert from undistorted sensor to distorted sensor plane coordinates */
    undistorted_to_distorted_sensor_coord (Xu, Yu, &Xd, &Yd);

    /* convert from sensor to image coordinates */
    *Xfd = Xd * cp.sx / cp.dpx + cp.Cx;
    *Yfd = Yd / cp.dpy + cp.Cy;
}


/************************************************************************/
void      distorted_to_undistorted_image_coord (Xfd, Yfd, Xfu, Yfu)
    double    Xfd,
              Yfd,
             *Xfu,
             *Yfu;
{
    double    Xd,
              Yd,
              Xu,
              Yu;

    /* convert from image to sensor coordinates */
    Xd = cp.dpx * (Xfd - cp.Cx) / cp.sx;
    Yd = cp.dpy * (Yfd - cp.Cy);

    /* convert from distorted sensor to undistorted sensor plane coordinates */
    distorted_to_undistorted_sensor_coord (Xd, Yd, &Xu, &Yu);

    /* convert from sensor to image coordinates */
    *Xfu = Xu * cp.sx / cp.dpx + cp.Cx;
    *Yfu = Yu / cp.dpy + cp.Cy;
}


/***********************************************************************\
* This routine takes the position of a point in world coordinates [mm]	*
* and determines the position of its image in image coordinates [pix].	*
\***********************************************************************/
void      world_coord_to_image_coord (xw, yw, zw, Xf, Yf)
    double    xw,
              yw,
              zw,
             *Xf,
             *Yf;
{
    double    xc,
              yc,
              zc,
              Xu,
              Yu,
              Xd,
              Yd;

    /* convert from world coordinates to camera coordinates */
    xc = cc.r1 * xw + cc.r2 * yw + cc.r3 * zw + cc.Tx;
    yc = cc.r4 * xw + cc.r5 * yw + cc.r6 * zw + cc.Ty;
    zc = cc.r7 * xw + cc.r8 * yw + cc.r9 * zw + cc.Tz;

    /* convert from camera coordinates to undistorted sensor plane coordinates */
    Xu = cc.f * xc / zc;
    Yu = cc.f * yc / zc;

    /* convert from undistorted to distorted sensor plane coordinates */
    undistorted_to_distorted_sensor_coord (Xu, Yu, &Xd, &Yd);

    /* convert from distorted sensor plane coordinates to image coordinates */
    *Xf = Xd * cp.sx / cp.dpx + cp.Cx;
    *Yf = Yd / cp.dpy + cp.Cy;
}


/***********************************************************************\
* This routine performs an inverse perspective projection to determine	*
* the position of a point in world coordinates that corresponds to a 	*
* given position in image coordinates.  To constrain the inverse	*
* projection to a single point the routine requires a Z world	 	*
* coordinate for the point in addition to the X and Y image coordinates.* 
\***********************************************************************/
void      image_coord_to_world_coord (Xfd, Yfd, zw, xw, yw)
    double    Xfd,
              Yfd, 
              zw,
             *xw,
             *yw;
{
    double    Xd,
              Yd,
              Xu,
              Yu,
              common_denominator;

    /* convert from image to distorted sensor coordinates */
    Xd = cp.dpx * (Xfd - cp.Cx) / cp.sx;
    Yd = cp.dpy * (Yfd - cp.Cy);

    /* convert from distorted sensor to undistorted sensor plane coordinates */
    distorted_to_undistorted_sensor_coord (Xd, Yd, &Xu, &Yu);

    /* calculate the corresponding xw and yw world coordinates	 */
    /* (these equations were derived by simply inverting	 */
    /* the perspective projection equations using Macsyma)	 */
    common_denominator = ((cc.r1 * cc.r8 - cc.r2 * cc.r7) * Yu +
			  (cc.r5 * cc.r7 - cc.r4 * cc.r8) * Xu -
			  cc.f * cc.r1 * cc.r5 + cc.f * cc.r2 * cc.r4);

    *xw = (((cc.r2 * cc.r9 - cc.r3 * cc.r8) * Yu +
	    (cc.r6 * cc.r8 - cc.r5 * cc.r9) * Xu -
	    cc.f * cc.r2 * cc.r6 + cc.f * cc.r3 * cc.r5) * zw +
	   (cc.r2 * cc.Tz - cc.r8 * cc.Tx) * Yu +
	   (cc.r8 * cc.Ty - cc.r5 * cc.Tz) * Xu -
	   cc.f * cc.r2 * cc.Ty + cc.f * cc.r5 * cc.Tx) / common_denominator;

    *yw = -(((cc.r1 * cc.r9 - cc.r3 * cc.r7) * Yu +
	     (cc.r6 * cc.r7 - cc.r4 * cc.r9) * Xu -
	     cc.f * cc.r1 * cc.r6 + cc.f * cc.r3 * cc.r4) * zw +
	    (cc.r1 * cc.Tz - cc.r7 * cc.Tx) * Yu +
	    (cc.r7 * cc.Ty - cc.r4 * cc.Tz) * Xu -
	    cc.f * cc.r1 * cc.Ty + cc.f * cc.r4 * cc.Tx) / common_denominator;
}


/***********************************************************************\
* This routine takes the position of a point in world coordinates [mm]	*
* and determines its position in camera coordinates [mm].		*
\***********************************************************************/
void      world_coord_to_camera_coord (xw, yw, zw, xc, yc, zc)
    double    xw,
              yw,
              zw,
             *xc,
             *yc,
	     *zc;
{
    *xc = cc.r1 * xw + cc.r2 * yw + cc.r3 * zw + cc.Tx;
    *yc = cc.r4 * xw + cc.r5 * yw + cc.r6 * zw + cc.Ty;
    *zc = cc.r7 * xw + cc.r8 * yw + cc.r9 * zw + cc.Tz;
}


/***********************************************************************\
* This routine takes the position of a point in camera coordinates [mm]	*
* and determines its position in world coordinates [mm].		*
\***********************************************************************/
void      camera_coord_to_world_coord (xc, yc, zc, xw, yw, zw)
    double    xc,
              yc,
              zc,
             *xw,
             *yw,
	     *zw;
{
    double    common_denominator;

    /* these equations were found by simply inverting the previous routine using Macsyma */

    common_denominator = ((cc.r1 * cc.r5 - cc.r2 * cc.r4) * cc.r9 +
			  (cc.r3 * cc.r4 - cc.r1 * cc.r6) * cc.r8 +
			  (cc.r2 * cc.r6 - cc.r3 * cc.r5) * cc.r7);

    *xw = ((cc.r2 * cc.r6 - cc.r3 * cc.r5) * zc +
	   (cc.r3 * cc.r8 - cc.r2 * cc.r9) * yc +
	   (cc.r5 * cc.r9 - cc.r6 * cc.r8) * xc +
	   (cc.r3 * cc.r5 - cc.r2 * cc.r6) * cc.Tz +
	   (cc.r2 * cc.r9 - cc.r3 * cc.r8) * cc.Ty +
	   (cc.r6 * cc.r8 - cc.r5 * cc.r9) * cc.Tx) / common_denominator;

    *yw = -((cc.r1 * cc.r6 - cc.r3 * cc.r4) * zc +
	    (cc.r3 * cc.r7 - cc.r1 * cc.r9) * yc +
	    (cc.r4 * cc.r9 - cc.r6 * cc.r7) * xc +
	    (cc.r3 * cc.r4 - cc.r1 * cc.r6) * cc.Tz +
	    (cc.r1 * cc.r9 - cc.r3 * cc.r7) * cc.Ty +
	    (cc.r6 * cc.r7 - cc.r4 * cc.r9) * cc.Tx) / common_denominator;

    *zw = ((cc.r1 * cc.r5 - cc.r2 * cc.r4) * zc +
	   (cc.r2 * cc.r7 - cc.r1 * cc.r8) * yc +
	   (cc.r4 * cc.r8 - cc.r5 * cc.r7) * xc +
	   (cc.r2 * cc.r4 - cc.r1 * cc.r5) * cc.Tz +
	   (cc.r1 * cc.r8 - cc.r2 * cc.r7) * cc.Ty +
	   (cc.r5 * cc.r7 - cc.r4 * cc.r8) * cc.Tx) / common_denominator;
}
