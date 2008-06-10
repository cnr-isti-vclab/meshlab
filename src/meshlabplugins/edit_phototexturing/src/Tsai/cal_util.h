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

#ifndef CAL_UTIL_H_
#define CAL_UTIL_H_

#include <stdio.h>

void      load_cd_data (FILE *fp,struct calibration_data *cd);

void      dump_cd_data (FILE     *fp,struct calibration_data *cd);


void  load_cp_cc_data (FILE     *fp,struct camera_parameters *cp, struct calibration_constants *cc);


void      dump_cp_cc_data (FILE *fp,struct camera_parameters *cp,struct calibration_constants *cc);


void      print_cp_cc_data (FILE     *fp,struct camera_parameters *cp,struct calibration_constants *cc);


void      print_error_stats (FILE     *fp);

#endif /*CAL_UTIL_H_*/
