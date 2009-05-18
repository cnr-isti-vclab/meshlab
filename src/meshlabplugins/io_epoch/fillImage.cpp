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

#include "fillImage.h"
#include <cmath>
#include <limits>

namespace ui
{

	fillImage::fillImage()
	{
	}

	fillImage::~fillImage()
	{
	}

	void fillImage::Compute(const QImage& input, int x, int y, int threshold_gradient, int threshold_fixed, QImage& output)
	{
		threshold_gradient_ = threshold_gradient;
		threshold_fixed_ = threshold_fixed;
		input_ = input;
		W = input.width();
		H = input.height();
		xo = x;
		yo = y;

		output = QImage(W, H, QImage::Format_Mono);
		computed_ = QImage(W, H, QImage::Format_Mono);
		output.fill(0);
		computed_.fill(0);

		ComputeGradient(input, gradient_);
		
		pixels_to_do_.push_back(std::make_pair(x,y));
		while(!pixels_to_do_.empty())
		{
			DealWithPixel(pixels_to_do_.front(),output);
			pixels_to_do_.pop_front();
		}

	}

	bool fillImage::ShouldWeCompute(int x, int y)
	{
		if (input_.isGrayscale())
			return (gradient_.get(x,y) < threshold_gradient_ && computed_.pixelIndex(x,y) == 0 && std::abs(qGray(input_.pixel(x,y)) - qGray(input_.pixel(xo,yo))) < threshold_fixed_);
		else
			return (gradient_.get(x,y) < threshold_gradient_ && computed_.pixelIndex(x,y) == 0 && std::abs(qRed(input_.pixel(x,y)) - qRed(input_.pixel(xo,yo))) < threshold_fixed_ && std::abs(qGreen(input_.pixel(x,y)) - qGreen(input_.pixel(xo,yo))) < threshold_fixed_ && std::abs(qBlue(input_.pixel(x,y)) - qBlue(input_.pixel(xo,yo))) < threshold_fixed_);
	}

	void fillImage::DealWithPixel(const std::pair<int,int>& xy, QImage& output)
	{
		int x = xy.first;
		int y = xy.second;
		if (computed_.pixelIndex(x,y) == 1)
			return;
		output.setPixel(x,y,1);
		computed_.setPixel(x,y,1);

		if (x>0 && ShouldWeCompute(x-1,y))
			pixels_to_do_.push_back(std::make_pair(x-1,y));
		if (x<W-1 && ShouldWeCompute(x+1, y))
			pixels_to_do_.push_back(std::make_pair(x+1,y));
		if (y>0 && ShouldWeCompute(x, y-1))
			pixels_to_do_.push_back(std::make_pair(x,y-1));
		if (y<H-1 && ShouldWeCompute(x, y+1))
			pixels_to_do_.push_back(std::make_pair(x,y+1));
	}

	void fillImage::ComputeGradient(const QImage& input, myGSImage& output)
	{
		input.save("input.jpg", "jpg");
		size_t W = input.width();
		size_t H = input.height();

		float* temp = new float[W*H];
		for (size_t i=0; i<W*H; ++i)
			temp[i] = 0.0;

		float dx, dy;
		float min = std::numeric_limits<float>::max();
		float max = -std::numeric_limits<float>::max();

		for (size_t i=1; i<W; ++i)
			for (size_t j=1; j<H; ++j)
			{
				dx = qGray(input.pixel(i,j)) - qGray(input.pixel(i-1,j));
				dy = qGray(input.pixel(i,j)) - qGray(input.pixel(i,j-1));

				dx = std::sqrt(dx*dx + dy*dy);
				temp[j*W+i] = dx;

				if (dx > max)
					max = dx;
				if (dx < min)
					min = dx;
			}
		output = myGSImage(W, H);

		float range = 255./(max-min);
		for (size_t i=0; i<W; ++i)
			for (size_t j=0; j<H; ++j)
				output.put(i, j, (unsigned char)((temp[j*W+i]-min)*range));		
	}
}
