/*******************************************************************************\
*                                                                               *
* This program generates a synthetic set of calibration data for use            *
* with Tsai's camera calibration algorithm.                                     *
*                                                                               *
* History                                                                       *
* -------                                                                       *
*                                                                               *
* 01-Apr-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Filename changes for DOS port.                                          *
*                                                                               *
* 06-Jun-94  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*       Fixed bug in call to print_cp_cc_data ().  The second and third         *
*       arguments should be pointers to the data structures.                    *
*       Bug reported by Chien-Ping Lu <cplu@NEBULA.SYSTEMSZ.CS.YALE.EDU>        *
*                                                                               *
* 30-Nov-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*       Updated to use new calibration statistics routines.                     *
*                                                                               *
* 01-May-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*       Modified to use utility routines.                                       *
*                                                                               *
* 07-Feb-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*       Original implementation.                                                *
*                                                                               *
\*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include "cal_main.h"

#define PI              3.14159265358979323846264338327950288419716939937511

#define TARGET_STDEV	0.04	/* 0.04 standard deviation of target dimensions in [mm] */
#define SENSOR_STDEV	0.02	/* 0.02 standard deviation of sensor measurements in [pixels] */

double    gasdev ();		/* random number generator */

int       idum = -1362;		/* initial seed for gasdev */


/* reasonable calibration constants for coplanar calibration */
void      cc_generate_calibration_constants ()
{
    cc.f = 270.0;		/* [mm] (focal length) */
    cc.kappa1 = -0.0006;	/* [1/mm^2] (distortion coefficient) */

    cc.Tx = -100.0;		/* [mm] (x translation) */
    cc.Ty = -85.0;		/* [mm] (y translation) */
    cc.Tz = 2000.0;		/* [mm] (z translation) */

    cc.Rx = 30.0 * PI / 180;	/* [rad] (x rotation) */
    cc.Ry = 1.0 * PI / 180;	/* [rad] (y rotation) */
    cc.Rz = 2.0 * PI / 180;	/* [rad] (z rotation) */
    apply_RPY_transform ();

    cc.Tx = 150.0;		/* [mm] (x translation) */
    cc.Ty = -75.0;		/* [mm] (y translation) */
    cc.Tz = 6600.0;		/* [mm] (z translation) */

    cc.Rx = 20.0 * PI / 180;	/* [rad] (x rotation) */
    cc.Ry = 5.0 * PI / 180;	/* [rad] (y rotation) */
    cc.Rz = 88.0 * PI / 180;	/* [rad] (z rotation) */
    apply_RPY_transform ();
}


/* reasonable calibration constants for noncoplanar calibration */
void      nc_generate_calibration_constants ()
{
    cc.f = 70.0;		/* [mm] (focal length) */
    cc.kappa1 = -0.0006;	/* [1/mm^2] (distortion coefficient) */

    cc.Tx = -100.0;		/* [mm] (x translation) */
    cc.Ty = -85.0;		/* [mm] (y translation) */
    cc.Tz = 2000.0;		/* [mm] (z translation) */

    cc.Rx = 30.0 * PI / 180;	/* [rad] (x rotation) */
    cc.Ry = 1.0 * PI / 180;	/* [rad] (y rotation) */
    cc.Rz = 2.0 * PI / 180;	/* [rad] (z rotation) */
    apply_RPY_transform ();
}



void      generate_calibration_data ()
{
#define X_COUNT		10	/* number of columns */
#define Y_COUNT		10	/* number of rows */
#define Z_COUNT		 1	/* number of planes */

#define X_ORIGIN        10	/* [mm] */
#define Y_ORIGIN        10	/* [mm] */
#define Z_ORIGIN	 0	/* [mm] */

#define X_SPACING       20	/* inter plane spacing [mm] */
#define Y_SPACING	20	/* inter plane spacing [mm] */
#define Z_SPACING	20	/* intra plane spacing [mm] */

    int       i,
              j,
              k;

    cd.point_count = 0;
    for (k = 0; k < Z_COUNT; k++)
	for (i = 0; i < X_COUNT; i++)
	    for (j = 0; j < Y_COUNT; j++) {
		/* generate a point in 3D world coordinates */
		cd.xw[cd.point_count] = i * X_SPACING + X_ORIGIN;
		cd.yw[cd.point_count] = j * Y_SPACING + Y_ORIGIN;
		cd.zw[cd.point_count] = k * Z_SPACING + Z_ORIGIN;

		/* generate the corresponding 2D image coordinates, including dimensional */
		/* noise - gaussian, zero mean, TARGET_STDEV standard deviation */
		world_coord_to_image_coord (cd.xw[cd.point_count] + gasdev (&idum) * TARGET_STDEV,
					    cd.yw[cd.point_count] + gasdev (&idum) * TARGET_STDEV,
					    cd.zw[cd.point_count] + gasdev (&idum) * TARGET_STDEV,
					    &(cd.Xf[cd.point_count]),
					    &(cd.Yf[cd.point_count]));

		/* add in measurement noise - gaussian, zero mean, SENSOR_STDEV standard deviation */
		cd.Xf[cd.point_count] += gasdev (&idum) * SENSOR_STDEV;
		cd.Yf[cd.point_count] += gasdev (&idum) * SENSOR_STDEV;

		cd.point_count += 1;
	    }
}


main ()
{
#define CX_OFFSET       4	/* x offset for center of radial lens distortion */
#define CY_OFFSET       8	/* y offset for center of radial lens distortion */
#define SX_FACTOR	1	/* perturbation factor for x scale factor */

    /* initialize the camera parameters (cp) with the photometrics constants */
    initialize_photometrics_parms ();

    /* perturb the image center a bit */
    cp.Cx += CX_OFFSET;
    cp.Cy += CY_OFFSET;

    /* perturb the scaling factor a bit */
    cp.sx *= SX_FACTOR;

    /* generate a synthetic set of calibration constants (cc) */
    cc_generate_calibration_constants ();

    /* generate a synthetic set of calibration data (cd) */
    generate_calibration_data ();

    fprintf (stderr, "\nCoplanar synthetic calibration data generation:\n");

    fprintf (stderr, "\ncamera type: %s\n\n", camera_type);

    print_cp_cc_data (stderr, &cp, &cc);

    print_error_stats (stderr);

    dump_cd_data (stdout, &cd);

    return 0;
}
