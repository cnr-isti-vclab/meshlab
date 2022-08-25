/*******************************************************************************\
*                                                                               *
* This program reads in a file containing coplanar calibration data             *
* and then uses the routines in cal_main.c to perform Tsai's                    *
* coplanar camera calibration.                                                  *
*                                                                               *
* If additional calibration data files are included on the command line         *
* they are tested using the model calibrated from the first set of              *
* calibration data.                                                             *
*                                                                               *
* At the end of the program the calibrated camera model is dumped in a format   *
* that can be loaded in by other programs.                                      *
*                                                                               *
* History                                                                       *
* -------                                                                       *
*                                                                               *
* 01-Apr-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Filename changes for DOS port.                                          *
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
#include "cal_main.h"

main (argc, argv)
    int       argc;
    char    **argv;
{
    FILE     *data_fd;

    int       i;

    if (argc < 2) {
	fprintf (stderr, "syntax: %s cd.data_file1 [ cd.data_file2 ..... ]\n", argv[0]);
	exit (-1);
    }

    /* initialize the camera parameters (cp) with the appropriate camera constants */
    initialize_photometrics_parms ();

    fprintf (stderr, "\nCoplanar calibration (Tz, f, kappa1 optimization) \n");

    fprintf (stderr, "\ncamera type: %s\n", camera_type);

    /* run through all of the files on the command line */
    for (i = 1; i < argc; i++) {
	if ((data_fd = fopen (argv[i], "r")) == NULL) {
	    fprintf (stderr, "%s: unable to open file \"%s\"\n", argv[0], argv[i]);
	    exit (-1);
	}
	/* load up the calibration data (cd) from the given data file */
	load_cd_data (data_fd, &cd);
	fclose (data_fd);

        fprintf (stderr, "\ndata file: %s  (%d points)\n\n", argv[i], cd.point_count);

	if (i == 1) {
	    /* determine the calibration constants from the 1st data file */
	    coplanar_calibration ();

	    print_cp_cc_data (stderr, &cp, &cc);
	}

	print_error_stats (stderr);
    }

    dump_cp_cc_data (stdout, &cp, &cc);

    return 0;
}
