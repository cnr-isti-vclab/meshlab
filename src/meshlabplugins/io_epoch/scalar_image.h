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

#ifndef VCG_SCALARIMAGE_H
#define VCG_SCALARIMAGE_H
#include <algorithm>
#include <assert.h>
#include <vcg/space/color4.h>
/*
Very simple class to store a bitmap of floating point values.
*/
template <class ScalarType> 
class ScalarImage
{
public:
  std::vector<ScalarType> v;
  int w,h;
  void resize(int _w, int _h) { w=_w; h=_h; v.resize(w*h);}
  bool Open(const char *filename);
  ScalarType &Val(int x,int y) {
    assert(x>=0 && x<w);
    assert(y>=0 && y<h);
    return v[y*w+x];
  };

  ScalarType Val(int x,int y) const {
    assert(x>=0 && x<w);
    assert(y>=0 && y<h);
    return v[y*w+x];
  };

  void operator = ( const ScalarImage & img )			
	{
    w=img.w;
    h=img.h;
    v=img.v;
  } 

  ScalarType MinVal()  {   return *std::min_element(v.begin(),v.end());  }
  ScalarType MaxVal()  {   return *std::max_element(v.begin(),v.end());  }

	inline QImage convertToQImage()
	{
		QImage img(w,h,QImage::Format_RGB32);

		float maxV = MaxVal();
		float minV = MinVal();

    float scale = 1.0f / (maxV-minV);
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++)
			{
				float value = (Val(x, y) - minV) * scale;
				value *= 255.0f;
				img.setPixel(x,y,qRgb(value,value,value));
			}

		return img;
	}

  void Erode(ScalarImage &Eroded, const int wsize=1) const
  {
    Eroded.resize(w,h);
    // erosion filter (3 x 3)
		  float minimum;
		  for (int y = wsize; y < h-wsize; y++)
			  for (int x = wsize; x < w-wsize; x++)
			  {
				  minimum = Val(x, y);
				  for (int yy = y - wsize; yy <= y + wsize; yy++)
					  for (int xx = x - wsize; xx <= x + wsize; xx++)
						  if (Val(xx, yy) < minimum)
							  minimum = Val(xx, yy);

				  Eroded.Val(x, y) = minimum;
			  }
  }

  void Dilate(ScalarImage &Dilated, int wsize=1)
  {
    Dilated.resize(w,h);
    // dilation filter (3 x 3)
		  float maximum;
		  for (int y = wsize; y < h-wsize; y++)
			  for (int x = wsize; x < w-wsize; x++)
			  {
				  maximum = Val(x, y);
				  for (int yy = y - wsize; yy <= y + wsize; yy++)
					  for (int xx = x - wsize; xx <= x + wsize; xx++)
						  if (Val(xx, yy) > maximum)
							  maximum = Val(xx, yy);

				  Dilated.Val(x, y) = maximum;
			  }
  }

  ScalarImage(QImage img);
  ScalarImage(){};
  bool Subsample(const int factor, ScalarImage<ScalarType> &fli);
  static QPixmap colorizedScaledToHeight(const int desiredH, ScalarImage<ScalarType> &fli, float colormax=10)
  {
    assert(fli.h>desiredH);
    int factor =  fli.h / desiredH;
     
    int newW=fli.w/factor -1;
    int newH=fli.h/factor -1;
    QImage newImage(newW,newH,QImage::Format_RGB32);
     
    for(int i=0;i<newImage.height();++i)
        for(int j=0;j<newImage.width();++j)
        {
          float sum=0;
          for(int si=0;si<factor;++si)
            for(int sj=0;sj<factor;++sj)
                sum+=  fli.Val(j*factor+sj,i*factor+si);
          sum/=factor*factor;
					sum=std::min(sum,colormax);
					vcg::Color4b avgcolor; avgcolor.ColorRamp(0,colormax,sum);
          newImage.setPixel(j,i,qRgb(avgcolor.V(0),avgcolor.V(1),avgcolor.V(2)));
        }
        return QPixmap::fromImage(newImage);
};

};

typedef ScalarImage<float> FloatImage;
typedef ScalarImage<unsigned char> CharImage;


#endif
