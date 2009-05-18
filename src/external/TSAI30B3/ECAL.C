/*******************************************************************************\
*                                                                               *
* This program reads in a file containing the cp/cc data for a camera model     *
* and a file containing the cd data obtained using the same camera but with a   *
* potentially different set of extrinsic parameters (i.e. Rx,Ry,Rz,Tz,Ty,Tz).   *
* The program then reestimates the extrinsic parameters to produce a new        *
* camera model.  The algorithm is based in part on Tsai's camera calibration    *
* algorithm.                                                                    *
*                                                                               *
* At the end of the program the new camera model is dumped in a format          *
* that can be loaded in by other programs.                                      *
*                                                                               *
* History                                                                       *
* -------                                                                       *
*                                                                               *
* 01-Apr-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Filename changes for DOS port.                                          *
*                                                                               *
* 11-Nov-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*       Original implementation.                                                *
*                                                                               *
\*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include "cal_main.h"

main (argc, argv)
    int       argc;
    char    **argv;
{
    FILE     *data_fd;

    double    max_zw,
              min_zw;

    int       i;

    if (argc != 3) {
	fprintf (stderr, "syntax: %s  cc.cp.data_file  cd.data_file\n", argv[0]);
	exit (-1);
    }

    fprintf (stdout, "\nExtrinsic Parameter Calibration\n");

    if ((data_fd = fopen (argv[1], "r")) == NULL) {
	fprintf (stderr, "%s: unable to open file \"%s\"\n", argv[0], argv[1]);
	exit (-1);
    }

    /* load up the calibration and camera data (cc and cp) from the given data file */
    load_cp_cc_data (data_fd, &cp, &cc);
    fclose (data_fd);

    if ((data_fd = fopen (argv[2], "r")) == NULL) {
	fprintf (stderr, "%s: unable to open file \"%s\"\n", argv[0], argv[2]);
	exit (-1);
    }
    /* load up the calibration and camera data (cc and cp) from the given data file */
    load_cd_data (data_fd, &cd);
    fclose (data_fd);

    fprintf (stderr, "\n   cp/cc data file: %s", argv[1]);

    /* find out if this data is coplanar in Z or not */
    max_zw = min_zw = cd.zw[0];
    for (i = 1; i < cd.point_count; i++)
	if (max_zw < cd.zw[i])
	    max_zw = cd.zw[i];
	else if (min_zw > cd.zw[i])
	    min_zw = cd.zw[i];

    if (max_zw == min_zw) {
	fprintf (stderr, "\n   cd data file:    %s   (%d data points, coplanar in Z)\n",
		 argv[2], cd.point_count);
	coplanar_extrinsic_parameter_estimation ();
    } else {
	fprintf (stderr, "\n   cd data file:    %s   (%d data points, noncoplanar in Z)\n",
		 argv[2], cd.point_count);
	noncoplanar_extrinsic_parameter_estimation ();
    }

    fprintf (stderr, "\n   new camera parameters:\n\n");

    print_cp_cc_data (stderr, &cp, &cc);

    print_error_stats (stderr);

    dump_cp_cc_data (stdout, &cp, &cc);

    return 0;
}
