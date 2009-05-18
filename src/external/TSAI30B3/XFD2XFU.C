/*******************************************************************************\
*                                                                               *
* This program reads in data for a calibrated camera model and then prompts     *
* the user for distorted image coordinates that they would like to convert      *
* into undistorted image coordinates using the model.                           *
*                                                                               *
* History                                                                       *
* -------                                                                       *
*                                                                               *
* 01-Apr-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Filename changes for DOS port.                                          *
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

#define MIN_X	0
#define MAX_X	575

#define MIN_Y	0
#define MAX_Y	383

main (argc, argv)
    int       argc;
    char    **argv;
{
    FILE     *data_fd;

    double    Xfd,
              Yfd,
              Xfu,
              Yfu,
              atof ();

    char      temp[256];

    if (argc != 2) {
	fprintf (stderr, "syntax: %s cp.cc.data_file\n", argv[0]);
	exit (-1);
    }

    /* load up the camera parameters and calibration constants from the given data file */
    if ((data_fd = fopen (argv[1], "r")) == NULL) {
	fprintf (stderr, "%s: unable to open file \"%s\"\n", argv[0], argv[1]);
	exit (-1);
    }
    load_cp_cc_data (data_fd, &cp, &cc);
    fclose (data_fd);

    fprintf (stdout, "\n Input file: %s\n\n", argv[1]);

    print_cp_cc_data (stdout, &cp, &cc);

    while (1) {
	/* prompt for distorted image coordinates */
	fprintf (stdout, "\n Enter Xfd [%d:%d] : ", MIN_X, MAX_X);

	if (gets (temp) == NULL)
	    break;
	Xfd = atof (temp);
	if ((Xfd < MIN_X) || (Xfd > MAX_X))
	    break;

	fprintf (stdout, "\n Enter Yfd [%d:%d] : ", MIN_Y, MAX_Y);

	if (gets (temp) == NULL)
	    break;
	Yfd = atof (temp);
	if ((Yfd < MIN_Y) || (Yfd > MAX_Y))
	    break;

	/* determine the corresponding undistorted image coordinates */
	distorted_to_undistorted_image_coord (Xfd, Yfd, &Xfu, &Yfu);

	fprintf (stdout,
		 "\n    [Xfd,Yfd] = [%.2lf, %.2lf]  -->  [Xfu,Yfu] = [%.2lf, %.2lf]\n",
		 Xfd, Yfd, Xfu, Yfu);
    }

    fprintf (stdout, "\n\n");

    return 0;
}
