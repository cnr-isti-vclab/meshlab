/*******************************************************************************\
*                                                                               *
* This file contains routines for measuring the accuracy of a calibrated        *
* camera model.  The routines are:                                              *
*                                                                               *
*       distorted_image_plane_error_stats ()                                    *
*       undistorted_image_plane_error_stats ()                                  *
*       object_space_error_stats ()                                             *
*       normalized_calibration_error ()                                         *
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
* History                                                                       *
* -------                                                                       *
*                                                                               *
* 18-May-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Split out from the cal_main.c file.                                     *
*       Simplified the statistical calculations.                                *
*                                                                               *
\*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include "cal_main.h"

extern struct camera_parameters cp;
extern struct calibration_constants cc;


/*******************************************************************************\
* This routine calculates the mean, standard deviation, max, and sum-of-squared *
* error of the magnitude of the error, in distorted image coordinates, between  *
* the measured location of a feature point in the image plane and the image of  *
* the 3D feature point as projected through the calibrated model.               *
* The calculation is for all of the points in the calibration data set.         *
\*******************************************************************************/
void      distorted_image_plane_error_stats (mean, stddev, max, sse)
    double   *mean,
             *stddev,
             *max,
             *sse;
{
    int       i;

    double    Xf,
              Yf,
              error,
              squared_error,
              max_error = 0,
              sum_error = 0,
              sum_squared_error = 0;

    if (cd.point_count < 1) {
	*mean = *stddev = *max = *sse = 0;
	return;
    }

    for (i = 0; i < cd.point_count; i++) {
	/* calculate the ideal location of the image of the data point */
	world_coord_to_image_coord (cd.xw[i], cd.yw[i], cd.zw[i], &Xf, &Yf);

	/* determine the error between the ideal and actual location of the data point	 */
	/* (in distorted image coordinates)						 */
	squared_error = SQR (Xf - cd.Xf[i]) + SQR (Yf - cd.Yf[i]);
	error = sqrt (squared_error);
	sum_error += error;
	sum_squared_error += squared_error;
	max_error = MAX (max_error, error);
    }

    *mean = sum_error / cd.point_count;
    *max = max_error;
    *sse = sum_squared_error;

    if (cd.point_count == 1)
	*stddev = 0;
    else
	*stddev = sqrt ((sum_squared_error - SQR (sum_error) / cd.point_count) / (cd.point_count - 1));
}


/*******************************************************************************\
* This routine calculates the mean, standard deviation, max, and sum-of-squared *
* error of the magnitude of the error, in undistorted image coordinates, between*
* the measured location of a feature point in the image plane and the image of  *
* the 3D feature point as projected through the calibrated model.               *
* The calculation is for all of the points in the calibration data set.         *
\*******************************************************************************/
void      undistorted_image_plane_error_stats (mean, stddev, max, sse)
    double   *mean,
             *stddev,
             *max,
             *sse;
{
    int       i;

    double    xc,
              yc,
              zc,
              Xu_1,
              Yu_1,
              Xu_2,
              Yu_2,
              Xd,
              Yd,
              distortion_factor,
              x_pixel_error,
              y_pixel_error,
              error,
              squared_error,
              max_error = 0,
              sum_error = 0,
              sum_squared_error = 0;

    if (cd.point_count < 1) {
	*mean = *stddev = *max = *sse = 0;
	return;
    }

    for (i = 0; i < cd.point_count; i++) {
	/* calculate the ideal location of the image of the data point */
	world_coord_to_camera_coord (cd.xw[i], cd.yw[i], cd.zw[i], &xc, &yc, &zc);

	/* convert from camera coordinates to undistorted sensor plane coordinates */
	Xu_1 = cc.f * xc / zc;
	Yu_1 = cc.f * yc / zc;

	/* convert from 2D image coordinates to distorted sensor coordinates */
	Xd = cp.dpx * (cd.Xf[i] - cp.Cx) / cp.sx;
	Yd = cp.dpy * (cd.Yf[i] - cp.Cy);

	/* convert from distorted sensor coordinates to undistorted sensor plane coordinates */
	distortion_factor = 1 + cc.kappa1 * (SQR (Xd) + SQR (Yd));
	Xu_2 = Xd * distortion_factor;
	Yu_2 = Yd * distortion_factor;

	/* determine the error between the ideal and actual location of the data point	 */
	/* (in undistorted image coordinates)						 */
	x_pixel_error = cp.sx * (Xu_1 - Xu_2) / cp.dpx;
	y_pixel_error = (Yu_1 - Yu_2) / cp.dpy;
	squared_error = SQR (x_pixel_error) + SQR (y_pixel_error);
	error = sqrt (squared_error);
	sum_error += error;
	sum_squared_error += squared_error;
	max_error = MAX (max_error, error);
    }

    *mean = sum_error / cd.point_count;
    *max = max_error;
    *sse = sum_squared_error;

    if (cd.point_count == 1)
	*stddev = 0;
    else
	*stddev = sqrt ((sum_squared_error - SQR (sum_error) / cd.point_count) / (cd.point_count - 1));
}


/*******************************************************************************\
* This routine calculates the mean, standard deviation, max, and sum-of-squared *
* error of the distance of closest approach (i.e. 3D error) between the point   *
* in object space and the line of sight formed by back projecting the measured  *
* 2D coordinates out through the camera model.                                  *
* The calculation is for all of the points in the calibration data set.         *
\*******************************************************************************/
void      object_space_error_stats (mean, stddev, max, sse)
    double   *mean,
             *stddev,
             *max,
             *sse;
{
    int       i;

    double    xc,
              yc,
              zc,
              Xu,
              Yu,
              Xd,
              Yd,
              t,
              distortion_factor,
              error,
              squared_error,
              max_error = 0,
              sum_error = 0,
              sum_squared_error = 0;

    if (cd.point_count < 1) {
	*mean = *stddev = *max = *sse = 0;
	return;
    }

    for (i = 0; i < cd.point_count; i++) {
	/* determine the position of the 3D object space point in camera coordinates */
	world_coord_to_camera_coord (cd.xw[i], cd.yw[i], cd.zw[i], &xc, &yc, &zc);

	/* convert the measured 2D image coordinates into distorted sensor coordinates */
	Xd = cp.dpx * (cd.Xf[i] - cp.Cx) / cp.sx;
	Yd = cp.dpy * (cd.Yf[i] - cp.Cy);

	/* convert from distorted sensor coordinates into undistorted sensor plane coordinates */
	distortion_factor = 1 + cc.kappa1 * (SQR (Xd) + SQR (Yd));
	Xu = Xd * distortion_factor;
	Yu = Yd * distortion_factor;

	/* find the magnitude of the distance (error) of closest approach */
	/* between the undistorted line of sight and the point in 3 space */
	t = (xc * Xu + yc * Yu + zc * cc.f) / (SQR (Xu) + SQR (Yu) + SQR (cc.f));
	squared_error = SQR (xc - Xu * t) + SQR (yc - Yu * t) + SQR (zc - cc.f * t);
	error = sqrt (squared_error);
	sum_error += error;
	sum_squared_error += squared_error;
	max_error = MAX (max_error, error);
    }

    *mean = sum_error / cd.point_count;
    *max = max_error;
    *sse = sum_squared_error;

    if (cd.point_count == 1)
	*stddev = 0;
    else
	*stddev = sqrt ((sum_squared_error - SQR (sum_error) / cd.point_count) / (cd.point_count - 1));
}


/*******************************************************************************\
* This routine performs an error measure proposed by Weng in IEEE PAMI,         *
* October 1992.                                                                 *
\*******************************************************************************/
void      normalized_calibration_error (mean, stddev)
    double   *mean,
             *stddev;
{
    int       i;

    double    xc,
              yc,
              zc,
              xc_est,
              yc_est,
              zc_est,
              fu,
              fv,
              Xd,
              Yd,
              Xu,
              Yu,
              distortion_factor,
              squared_error,
              sum_error = 0,
              sum_squared_error = 0;

    if (cd.point_count < 1) {
	*mean = *stddev = 0;
	return;
    }

    /* This error metric is taken from						 */
    /* "Camera Calibration with Distortion Models and Accuracy Evaluation"	 */
    /* J. Weng, P. Cohen, and M. Herniou					 */
    /* IEEE Transactions on PAMI, Vol. 14, No. 10, October 1992, pp965-980	 */

    for (i = 0; i < cd.point_count; i++) {
	/* estimate the 3D coordinates of the calibration data point by back 	 */
	/* projecting its measured image location through the model to the	 */
	/* plane formed by the original z world component.			 */

	/* calculate the location of the data point in camera coordinates */
	world_coord_to_camera_coord (cd.xw[i], cd.yw[i], cd.zw[i], &xc, &yc, &zc);

	/* convert from 2D image coordinates to distorted sensor coordinates */
	Xd = cp.dpx * (cd.Xf[i] - cp.Cx) / cp.sx;
	Yd = cp.dpy * (cd.Yf[i] - cp.Cy);

	/* convert from distorted sensor coordinates to undistorted sensor plane coordinates */
	distortion_factor = 1 + cc.kappa1 * (SQR (Xd) + SQR (Yd));
	Xu = Xd * distortion_factor;
	Yu = Yd * distortion_factor;

	/* estimate the location of the data point by back projecting the image position */
	zc_est = zc;
	xc_est = zc_est * Xu / cc.f;
	yc_est = zc_est * Yu / cc.f;

	fu = cp.sx * cc.f / cp.dpx;
	fv = cc.f / cp.dpy;

	squared_error = (SQR (xc_est - xc) + SQR (yc_est - yc)) /
	 (SQR (zc_est) * (1 / SQR (fu) + 1 / SQR (fv)) / 12);
	sum_error += sqrt (squared_error);
	sum_squared_error += squared_error;
    }

    *mean = sum_error / cd.point_count;

    if (cd.point_count == 1)
	*stddev = 0;
    else
	*stddev = sqrt ((sum_squared_error - SQR (sum_error) / cd.point_count) / (cd.point_count - 1));
}
