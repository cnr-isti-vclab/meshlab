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

#include <common/parameters/rich_parameter_list.h>
#include <vcglib/vcg/complex/algorithms/align_pair.h>

class FilterIcpAlignParameter {

public:
	static void RichParameterSetToAlignPairParam(const RichParameterList &rps, vcg::AlignPair::Param &app);
	static void AlignPairParamToRichParameterSet(const vcg::AlignPair::Param &app, RichParameterList &rps);
private:
	/* No need to have an instance of this class */
	FilterIcpAlignParameter();
};
