/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2020                                           \/)\/    *
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
#ifndef MESHLAB_COMPILE_TIME_VERSION_H
#define MESHLAB_COMPILE_TIME_VERSION_H

// Example of __DATE__ string: "Jul 27 2012"
#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')

#define COMPUTE_BUILD_MONTH \
	( \
		(BUILD_MONTH_IS_JAN) ?  1 : \
		(BUILD_MONTH_IS_FEB) ?  2 : \
		(BUILD_MONTH_IS_MAR) ?  3 : \
		(BUILD_MONTH_IS_APR) ?  4 : \
		(BUILD_MONTH_IS_MAY) ?  5 : \
		(BUILD_MONTH_IS_JUN) ?  6 : \
		(BUILD_MONTH_IS_JUL) ?  7 : \
		(BUILD_MONTH_IS_AUG) ?  8 : \
		(BUILD_MONTH_IS_SEP) ?  9 : \
		(BUILD_MONTH_IS_OCT) ? 10 : \
		(BUILD_MONTH_IS_NOV) ? 11 : \
		(BUILD_MONTH_IS_DEC) ? 12 : \
		/* error default */  99 \
	)

constexpr unsigned int compileTimeYear(){
	return ((__DATE__)[7] - '0') * 1000 + ((__DATE__)[8] - '0') * 100 + ((__DATE__)[9] - '0') * 10 + ((__DATE__)[10] - '0');
}

constexpr unsigned int compileTimeMonth() {
	return COMPUTE_BUILD_MONTH;
}


#endif // MESHLAB_COMPILE_TIME_VERSION_H
