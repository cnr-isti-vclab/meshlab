#ifndef _CONST_TYPES_H_
#define _CONST_TYPES_H_

//These 3 types define unsigned int of 8, 16 and 32 bits
typedef unsigned char		UINT8;
typedef unsigned short int	UINT16;
typedef unsigned int		UINT32;

#define UINT8size  sizeof(UINT8)
#define	UINT16size sizeof(UINT16)
#define	UINT32size sizeof(UINT32)

#define COLOR_BAND_SIZE	1024

#define CANVAS_BORDER_DISTANCE	10.0f

//da eliminare!!! MAL
#define Y_SCALE_STEP			5

#define MARKERS_RADIUS			2.0f

#define REMOVE_TF_HANDLE	0x00000001
#define REMOVE_TF_BG		0x00000010
#define REMOVE_TF_LINES		0x00000100
#define REMOVE_TF_ALL		(REMOVE_TF_HANDLE | REMOVE_TF_BG | REMOVE_TF_LINES)
#define REMOVE_EQ_HANDLE	0x00001000
#define REMOVE_HISTOGRAM	0x00010000
#define DELETE_REMOVED_ITEMS	0x00100000
#define DO_NOT_DELETE_REMOVED_ITEMS	0x11011111
#define REMOVE_ALL_ITEMS	(REMOVE_TF_HANDLE | REMOVE_EQ_HANDLE | REMOVE_TF_BG | REMOVE_TF_LINES | REMOVE_HISTOGRAM)

#endif