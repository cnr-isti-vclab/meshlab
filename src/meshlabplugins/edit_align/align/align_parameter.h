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

#include <common/filterparameter.h>
#include <meshlabplugins/editalign/align/AlignPair.h>
#include <vcg/simplex/face/base.h>

class AlignParameter {

public:
	//translates the filter parameters into align parameters
	static void buildAlignParameters(RichParameterSet &fps, vcg::AlignPair::Param &app);

	//translates the align parameters into filter parameters
	static void buildRichParameterSet(vcg::AlignPair::Param &app, RichParameterSet &fps);

private:
	//no need to have an instance of this class
	AlignParameter();

};
