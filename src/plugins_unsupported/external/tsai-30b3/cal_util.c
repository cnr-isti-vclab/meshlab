/*******************************************************************************\
*                                                                               *
* This file contains utility routines for dumping, loading and printing         *
* the data structures used by the routines contained in the file                *
* cal_main.c.                                                                   *
*                                                                               *
* History                                                                       *
* -------                                                                       *
*                                                                               *
* 01-Apr-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Filename changes for DOS port.                                          *
*                                                                               *
* 04-Jun-94  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Added alternate macro definitions for less common math functions.       *
*                                                                               *
* 30-Nov-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*       Updated to use new calibration statistics routines.                     *
*                                                                               *
* 01-May-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*            Original implementation.                                           *
*                                                                               *
*                                                                               *
\*******************************************************************************/

#include <stdio.h>
#include "cal_main.h"

#define PI              3.14159265358979323846264338327950288419716939937511


void      load_cd_data (fp, cd)
    FILE     *fp;
    struct calibration_data *cd;
{
    cd->point_count = 0;
    while (fscanf (fp, "%lf %lf %lf %lf %lf",
		   &(cd->xw[cd->point_count]),
		   &(cd->yw[cd->point_count]),
		   &(cd->zw[cd->point_count]),
		   &(cd->Xf[cd->point_count]),
		   &(cd->Yf[cd->point_count])) != EOF)
	if (cd->point_count++ >= MAX_POINTS) {
	    fprintf (stderr, "load_cd_data: too many points, compiled in limit is %d\n", MAX_POINTS);
	    exit (-1);
	}
}


void      dump_cd_data (fp, cd)
    FILE     *fp;
    struct calibration_data *cd;
{
    int       i;

    for (i = 0; i < cd->point_count; i++)
	fprintf (fp, "%17.10le %17.10le %17.10le %17.10le %17.10le\n",
		 cd->xw[i],
		 cd->yw[i],
		 cd->zw[i],
		 cd->Xf[i],
		 cd->Yf[i]);
}


void      load_cp_cc_data (fp, cp, cc)
    FILE     *fp;
    struct camera_parameters *cp;
    struct calibration_constants *cc;
{
    double    sa,
              ca,
              sb,
              cb,
              sg,
              cg;

    fscanf (fp, "%lf", &(cp->Ncx));
    fscanf (fp, "%lf", &(cp->Nfx));
    fscanf (fp, "%lf", &(cp->dx));
    fscanf (fp, "%lf", &(cp->dy));
    fscanf (fp, "%lf", &(cp->dpx));
    fscanf (fp, "%lf", &(cp->dpy));
    fscanf (fp, "%lf", &(cp->Cx));
    fscanf (fp, "%lf", &(cp->Cy));
    fscanf (fp, "%lf", &(cp->sx));

    fscanf (fp, "%lf", &(cc->f));
    fscanf (fp, "%lf", &(cc->kappa1));
    fscanf (fp, "%lf", &(cc->Tx));
    fscanf (fp, "%lf", &(cc->Ty));
    fscanf (fp, "%lf", &(cc->Tz));
    fscanf (fp, "%lf", &(cc->Rx));
    fscanf (fp, "%lf", &(cc->Ry));
    fscanf (fp, "%lf", &(cc->Rz));

    SINCOS (cc->Rx, sa, ca);
    SINCOS (cc->Ry, sb, cb);
    SINCOS (cc->Rz, sg, cg);

    cc->r1 = cb * cg;
    cc->r2 = cg * sa * sb - ca * sg;
    cc->r3 = sa * sg + ca * cg * sb;
    cc->r4 = cb * sg;
    cc->r5 = sa * sb * sg + ca * cg;
    cc->r6 = ca * sb * sg - cg * sa;
    cc->r7 = -sb;
    cc->r8 = cb * sa;
    cc->r9 = ca * cb;

    fscanf (fp, "%lf", &(cc->p1));
    fscanf (fp, "%lf", &(cc->p2));
}


void      dump_cp_cc_data (fp, cp, cc)
    FILE     *fp;
    struct camera_parameters *cp;
    struct calibration_constants *cc;
{
    fprintf (fp, "%17.10le\n", cp->Ncx);
    fprintf (fp, "%17.10le\n", cp->Nfx);
    fprintf (fp, "%17.10le\n", cp->dx);
    fprintf (fp, "%17.10le\n", cp->dy);
    fprintf (fp, "%17.10le\n", cp->dpx);
    fprintf (fp, "%17.10le\n", cp->dpy);
    fprintf (fp, "%17.10le\n", cp->Cx);
    fprintf (fp, "%17.10le\n", cp->Cy);
    fprintf (fp, "%17.10le\n", cp->sx);

    fprintf (fp, "%17.10le\n", cc->f);
    fprintf (fp, "%17.10le\n", cc->kappa1);
    fprintf (fp, "%17.10le\n", cc->Tx);
    fprintf (fp, "%17.10le\n", cc->Ty);
    fprintf (fp, "%17.10le\n", cc->Tz);
    fprintf (fp, "%17.10le\n", cc->Rx);
    fprintf (fp, "%17.10le\n", cc->Ry);
    fprintf (fp, "%17.10le\n", cc->Rz);
    fprintf (fp, "%17.10le\n", cc->p1);
    fprintf (fp, "%17.10le\n", cc->p2);
}


void      print_cp_cc_data (fp, cp, cc)
    FILE     *fp;
    struct camera_parameters *cp;
    struct calibration_constants *cc;
{
    fprintf (fp, "       f = %.6lf  [mm]\n\n", cc->f);

    fprintf (fp, "       kappa1 = %.6le  [1/mm^2]\n\n", cc->kappa1);

    fprintf (fp, "       Tx = %.6lf,  Ty = %.6lf,  Tz = %.6lf  [mm]\n\n", cc->Tx, cc->Ty, cc->Tz);

    fprintf (fp, "       Rx = %.6lf,  Ry = %.6lf,  Rz = %.6lf  [deg]\n\n",
	     cc->Rx * 180 / PI, cc->Ry * 180 / PI, cc->Rz * 180 / PI);

    fprintf (fp, "       R\n");
    fprintf (fp, "       %9.6lf  %9.6lf  %9.6lf\n", cc->r1, cc->r2, cc->r3);
    fprintf (fp, "       %9.6lf  %9.6lf  %9.6lf\n", cc->r4, cc->r5, cc->r6);
    fprintf (fp, "       %9.6lf  %9.6lf  %9.6lf\n\n", cc->r7, cc->r8, cc->r9);

    fprintf (fp, "       sx = %.6lf\n", cp->sx);
    fprintf (fp, "       Cx = %.6lf,  Cy = %.6lf  [pixels]\n\n", cp->Cx, cp->Cy);

    fprintf (fp, "       Tz / f = %.6lf\n\n", cc->Tz / cc->f);
}


void      print_error_stats (fp)
    FILE     *fp;
{
    double    mean,
              stddev,
              max,
              sse;

    /* calculate the distorted image plane error statistics for the data set */
    distorted_image_plane_error_stats (&(mean), &(stddev), &(max), &(sse));
    fprintf (fp, "       distorted image plane error:\n");
    fprintf (fp,
	     "         mean = %.6lf,  stddev = %.6lf,  max = %.6lf  [pix],  sse = %.6lf  [pix^2]\n\n",
	     mean, stddev, max, sse);

    /* calculate the undistorted image plane error statistics for the data set */
    undistorted_image_plane_error_stats (&(mean), &(stddev), &(max), &(sse));
    fprintf (fp, "       undistorted image plane error:\n");
    fprintf (fp,
	     "         mean = %.6lf,  stddev = %.6lf,  max = %.6lf  [pix],  sse = %.6lf  [pix^2]\n\n",
	     mean, stddev, max, sse);

    /* calculate the undistorted image plane error statistics for the data set */
    object_space_error_stats (&(mean), &(stddev), &(max), &(sse));
    fprintf (fp, "       object space error:\n");
    fprintf (fp,
	     "         mean = %.6lf,  stddev = %.6lf,  max = %.6lf  [mm],  sse = %.6lf  [mm^2]\n\n",
	     mean, stddev, max, sse);

    /* calculate the error statistics for the data set */
    normalized_calibration_error (&(mean), &(stddev));

    fprintf (fp, "       normalized calibration error:  %.6lf\n\n", mean);
}
