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
#ifndef FLOATBUFFER_H
#define FLOATBUFFER_H

#include<vcg\math\shot.h>
#include<vcg\math\histogram.h>

#include<QString>
#include<QImage>
#include<queue>

using namespace std;
using namespace vcg;

//----------------------------------  BMP headers for cross platform compilation
 
/* Note: the magic number has been removed from the bmpfile_header structure
   since it causes alignment problems
     struct bmpfile_magic should be written/read first
   followed by the
     struct bmpfile_header
   [this avoids compiler-specific alignment pragmas etc.]
*/
 
typedef unsigned int uint16_t; 
typedef signed long int int32_t; 
typedef unsigned long int uint32_t;

typedef struct {
  unsigned char magic[2];
} BITMAPMAGICHEADER_X;
 
typedef struct {
  uint32_t filesz;
  uint16_t creator1;
  uint16_t creator2;
  uint32_t bmp_offset;
} BITMAPFILEHEADER_X;

typedef struct {
  uint32_t header_sz;
  int32_t width;
  int32_t height;
  uint16_t nplanes;
  uint16_t bitspp;
  uint32_t compress_type;
  uint32_t bmp_bytesz;
  int32_t hres;
  int32_t vres;
  uint32_t ncolors;
  uint32_t nimpcolors;
} BITMAPINFOHEADER_X;

//------------------------------------------------------------------------------

class colorp
{
 public:

	float r;
	float g;
	float b;

	float w;
	int im;
};

class floatbuffer
{
 public:

	float* data;

	int sx,sy;

	int loaded;	// -1 not created
				      //  1 loaded
				      //  0 unloaded

	QString filename; // when unloaded, offcore filename

	floatbuffer(void);
	floatbuffer(floatbuffer *from);
	~floatbuffer(void);

	int init(int sizex, int sizey);
	int destroy();

	float getval(int xx, int yy);
	int   setval(int xx, int yy, float val);

	int fillwith(float val);

	int applysobel(floatbuffer *from);
	int initborder(floatbuffer* zerofrom);
	int distancefield();

	int dump(QString filename);
	int dumpbmp(QString filename, bool asitis = false);
	int dumpbmp2(QString filename);
	int dumppfm(QString filename);
};


#endif // FLOATBUFFER_H