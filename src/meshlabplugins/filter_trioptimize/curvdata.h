/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
#ifndef __CURVDATA
#define __CURVDATA

namespace vcg
{
class CurvData
{
public:
	CurvData()
	{
		A = 0; // area
		H = 0; // absolute mean curvature
		K = 0; // gaussian curvature
	}
	
	virtual ~CurvData() {};
	
	friend const CurvData operator+(const CurvData &lhs, const CurvData &rhs)
	{
		CurvData result;
		result.A = lhs.A + rhs.A;
		result.H = lhs.H + rhs.H;
		result.K = lhs.K + rhs.K;
		return result;
	}
	
	friend CurvData &operator+=(CurvData &lhs, const CurvData &rhs)
	{
		lhs.A += rhs.A;
		lhs.H += rhs.H;
		lhs.K += rhs.K;
		return lhs;
	}
	
	float A;
	float H;
	float K;
};


class NSMCEval
{
public:
	float operator() (const CurvData& c)
	{
		return (powf(c.H / 4.0, 2.0f) / c.A);
	}
};

class MeanCEval
{
public:
	float operator() (const CurvData& c)
	{
		return (c.H / 4.0);
	}
};

class AbsCEval
{
public:
	float operator() (const CurvData& c)
	{
		float k = 2 * M_PI - c.K;
		if(k > 0) return 2.0 * (c.H / 4.0);
		else return 2.0 * math::Sqrt(powf(c.H / 4.0, 2.0) - (c.A * k));
	}
};

} //end namespace vcg
#endif // __CURVDATA
