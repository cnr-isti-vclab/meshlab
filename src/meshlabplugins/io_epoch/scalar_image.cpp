/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include <Qt>
#include <QPixmap>
//#include <QtGui>
//#include <QtXml/QDomDocument>
//#include <QtXml/QDomElement>
//#include <QtXml/QDomNode>
//
//
//// temporaneamente prendo la versione corrente dalla cartella test
//#include<vcg/complex/trimesh/update/bounding.h>
//#include <wrap/io_trimesh/io_mask.h>
//#include <vcg/complex/trimesh/create/platonic.h>
//#include <vcg/complex/trimesh/update/bounding.h>
//#include <vcg/complex/trimesh/update/normal.h>
//#include <vcg/math/matrix33.h>
//#include<vcg/complex/trimesh/append.h>
//
//#include <QMessageBox>
//#include <QFileDialog>

//#include "epoch.h"
//#include "radial_distortion.h"
//#include "epoch_camera.h"
#include <bzlib.h>
#include <vector>
#include "scalar_image.h"



using namespace std;
/*

Images with bit-depths greater than 8bpp and up to 31bpp are supported by the JJ2000 codec if they are stored in PGX files
(one file per component). PGX is a custom monochrome file format invented specifically to simplify the use of JPEG 2000 
with images of different bit-depths in the range of 1 to 31 bits per pixel.

The file consists of a one line text header followed by the (raw) data.

Header: "PG"+ ws +<endianess>+ ws +[sign]+ws + <bit-depth>+" "+<width>+" "+<height>+[compressed_size]'\n'

where:

    * ws (white-spaces) is any combination of characters ' ' and '\t'.
    * endianess equals "LM" or "ML"(resp. little-endian or big-endian)
    * sign equals "+" or "-" (resp. unsigned or signed). If omited, values are supposed to be unsigned.
    * bit-depth that can be any number between 1 and 31. This number must take into account the eventual sign bit.
    * width and height are the image dimensions (in pixels).

Data: The image binary values appear one after the other (in raster order) immediately after the last header character ('\n')
and are byte-aligned (they are packed into 1,2 or 4 bytes per sample, depending upon the bit-depth value).
*/
template <>
bool ScalarImage<float>::Open(const char *filename)
{
  FILE *fp=fopen(filename,"rb");
  if(!fp) return false;
  char buf[256];
  fgets(buf,255,fp);
  qDebug("Header of %s is '%s'",filename,buf);
  float ll,lh;
  int depth;
	char mode;
	int compressed_size=0;
  sscanf(buf,"PG LM %i %i %i %c %f %f %i",&depth,&w,&h,&mode,&ll,&lh,&compressed_size);
  qDebug("image should be of %i x %i %i depth and with range in %f -- %f in mode %c",w,h,depth,ll,lh,mode);
  if(depth!=16) 
  {
    qDebug("Wrong depth of image 16 bit expected");
    return false;
  }
	if (mode != 'l' && mode != 'L')
	{
		qDebug("Wrong mode, expected l or L");
		return false;
	}

	if (mode == 'l')
	{
		vector<unsigned short> bb(w*h);
		fread(&*bb.begin(),w*h,sizeof(short),fp);
		v.resize(w*h);
		for(int i =0; i<w*h;++i)
			v[i]=ll+(lh-ll)*(float(bb[i])/65536.0f);
	}
	else
	{
		char* compressed_buffer = new char[compressed_size];
		fread(compressed_buffer,compressed_size,1,fp);
		// decompress!
		size_t size = w*h*sizeof(short);
		unsigned char * uncompressed_buffer = new unsigned char[size];
		unsigned int mysize = size;
		BZ2_bzBuffToBuffDecompress((char*)uncompressed_buffer, &mysize, compressed_buffer, compressed_size, 0, 0);
		if (mysize != size)
		{
			qDebug("This is very wrong. The uncompressed size is not the expected size");
			return false;
		}
		int imagesize = w*h;
		unsigned char *correct_buffer = new unsigned char[size];

		for (int i=0; i<imagesize; ++i)
		{
      for (size_t j=0; j<sizeof(short); ++j)
			{
				correct_buffer[sizeof(short)*i +j] = uncompressed_buffer[imagesize*j + i];
			}
		}
		v.resize(w*h);
		for (int i=0; i<imagesize; ++i)
		{
			float a = *((unsigned short*)&correct_buffer[i*sizeof(short)]);
			v[i] = ll+(lh-ll)*(a/65536.0f);
		}

		delete [] uncompressed_buffer;
		delete [] compressed_buffer;
		delete [] correct_buffer;
	}
  fclose(fp);
  return true;
}


template <>
bool ScalarImage<unsigned char>::Open(const char *filename)
{
  FILE *fp=fopen(filename,"rb");
  if(!fp) return false;
  char buf[256];
  fgets(buf,255,fp);
  qDebug("Header of %s is '%s'",filename,buf);
  int depth;
	char mode = ' ';
	int compressed_size=0;
  int nrscan = sscanf(buf,"PG LM %i %i %i %c %i",&depth,&w,&h,&mode,&compressed_size);
	if (nrscan == 3)
		qDebug("image should be of %i x %i %i depth ",w,h,depth);
	else
		qDebug("compressed image of %i x %i %i depth ",w,h,depth);
  if(depth!=8) 
  {
    qDebug("Wrong depth of image: 8 bit expected");
    return false;
  }
  
	if (mode != 'C')
	{
		v.resize(w*h);
		fread(&*v.begin(),w*h,sizeof(unsigned char),fp);
	}
	else
	{
		char* compressed_buffer = new char[compressed_size];
		fread(compressed_buffer,compressed_size,1,fp);
		// decompress!
		unsigned int mysize = w*h;
		v.resize(w*h);
		BZ2_bzBuffToBuffDecompress((char*)&*v.begin(), &mysize, compressed_buffer, compressed_size, 0, 0);
    if (mysize != (unsigned int)(w*h))
		{
			qDebug("This is very wrong. The uncompressed size is not the expected size");
			return false;
		}
	}
  
  fclose(fp);
  return true;
}


template <>
ScalarImage<unsigned char>::ScalarImage(QImage img)
{
  resize(img.width(),img.height());

   for(int y=0;y<h;++y)
      for(int x=0;x<w;++x)
        Val(x,y)=qGray(img.pixel(x,y));

}

template <class ScalarType>
bool ScalarImage<ScalarType>::Subsample(const int factor, ScalarImage<ScalarType> &fli)
{
 resize(fli.w/factor,fli.h/factor);

  for(int i=0;i<h;++i)
    for(int j=0;j<w;++j)
      Val(i,j)=fli.Val(i*factor,j*factor);
  return true;
};

