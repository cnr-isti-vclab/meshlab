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

#ifndef __IO_FILL_AREA_INC_
#define __IO_FILL_AREA_INC_

#include <deque>
#include <algorithm>
#include <QtGui/QImage>

namespace ui
{
	struct myGSImage
	{
		unsigned char* data;
		size_t w, h;
	
		myGSImage()
		{
			data = 0;
		}

		myGSImage(const myGSImage& image)
		{
			w = image.w;
			h = image.h;
			if (data)
				delete [] data;
			data = new unsigned char[w*h];
			memcpy(data, image.data, w*h);
		}

		myGSImage & operator = (const myGSImage& image)
		{
			w = image.w;
			h = image.h;
			if (data)
				delete [] data;
			data = new unsigned char[w*h];
			memcpy(data, image.data, w*h);			
			return *this;
		}
	
		myGSImage(size_t width, size_t height): w(width), h(height)
		{
			data = new unsigned char[width*height];
		};

		~myGSImage()
		{
			if (data)
				delete [] data;
		}
		
		unsigned char get(size_t i, size_t j) const
		{
			return data[j*w + i];
		}

		void put(size_t i, size_t j, unsigned char a)
		{
			data[j*w + i] = a;
		}

		void fill(unsigned char a)
		{
			for (size_t i=0; i<w; ++i)
				for (size_t j=0; j<h; ++j)
					put(i,j,a);
		}
	};

	class fillImage
	{
	public:
		fillImage();
		virtual ~fillImage();
		void Compute(const QImage& input, int x, int y, int threshold_gradient, 
								 int threshold_fixed, QImage& output);
		
	protected:
		
		bool ShouldWeCompute(int x, int y);

		void ComputeGradient(const QImage&, myGSImage& output);
		
		void DealWithPixel(const std::pair<int,int>&, QImage& output);
		QImage input_, computed_;
		myGSImage gradient_;
		int threshold_gradient_, threshold_fixed_;
		int W,H;
		int xo, yo;
		std::deque<std::pair<int,int> > pixels_to_do_;
	};
}

#endif
