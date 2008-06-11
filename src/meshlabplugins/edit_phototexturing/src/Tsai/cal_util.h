/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2008                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/* 
  This file derives from the public domain tsai lib. 
  The original author of the Tsai lib released it saying that it could be 
  modified and re-distributed as needed, 
*/

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
