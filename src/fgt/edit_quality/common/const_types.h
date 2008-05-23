/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
/****************************************************************************
History
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#ifndef _CONST_TYPES_H_
#define _CONST_TYPES_H_

//These 3 types define unsigned int of 8, 16 and 32 bits
typedef unsigned char		UINT8;
typedef unsigned short int	UINT16;
typedef unsigned int		UINT32;

#define UINT8size  sizeof(UINT8)
#define	UINT16size sizeof(UINT16)
#define	UINT32size sizeof(UINT32)

//Size of Transfer Function color band
#define COLOR_BAND_SIZE	1024

//Default distance of graphics items from view borders in the GUI
#define CANVAS_BORDER_DISTANCE	10.0f

//minimum step used to round float values in histogram
#define Y_SCALE_STEP			5

#define NUMBER_OF_HISTOGRAM_BINS 50000
#define NUMBER_OF_HISTOGRAM_BARS 100

// The defalut percentile for clamping equalizer histogram
#define CLAMP_PERCENTILE 90


//set of macros used to tell to clearItems method what to remove and\or delete
//each macro refers to a particular type of graphic item
#define REMOVE_TF_HANDLE	0x00000001
#define REMOVE_TF_BG		0x00000010
#define REMOVE_TF_LINES		0x00000100
#define REMOVE_TF_ALL		(REMOVE_TF_HANDLE | /*REMOVE_TF_BG |*/ REMOVE_TF_LINES)
#define REMOVE_EQ_HANDLE	0x00001000
#define REMOVE_EQ_HISTOGRAM	0x00010000
#define DELETE_REMOVED_ITEMS	0x00100000
#define DO_NOT_DELETE_REMOVED_ITEMS	0x11011111
#define REMOVE_ALL_ITEMS	(REMOVE_TF_HANDLE | REMOVE_EQ_HANDLE | REMOVE_TF_BG | REMOVE_TF_LINES | REMOVE_EQ_HISTOGRAM)

//default size of Equalizer and Transfer Function graphical handles
#define DEFAULT_HANDLE_SIZE	6

//struct used to pack info about equalizer state
//this struct is used when a CSV file is saved or loaded
struct EQUALIZER_INFO
{
	float minQualityVal;
	float midQualityPercentage;
	float maxQualityVal;
	float brightness;
};

#define QUALITY_LABEL_DIGITS_NUM	8


#endif