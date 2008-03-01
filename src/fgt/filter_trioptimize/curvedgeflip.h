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

#include <meshlab/meshmodel.h>
#include <vcg/complex/local_optimization/tri_edge_flip.h>


class CurvEdgeFlip; // forward declaration


/* This flip happens only if decreases the curvature of the surface */
class CurvEdgeFlip : public vcg::tri::TriEdgeFlip<CMeshO, CurvEdgeFlip > {
	protected:
		typedef vcg::tri::TriEdgeFlip<CMeshO, CurvEdgeFlip > Parent;
	
	public:
		CurvEdgeFlip() {}
		
		CurvEdgeFlip(CurvEdgeFlip &par)
		{
			this->_pos = par.GetPos();
			this->_localMark = par.GetMark();
			this->_priority = par.Priority();
		}
		
		CurvEdgeFlip(const PosType pos, int mark) : Parent(pos, mark) {}
		//static void Finalize(CMeshO &m, HeapType&h_ret) {}
};
