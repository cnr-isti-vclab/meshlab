/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005 - 2020                                          \/)\/   *
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

#include "cmesh.h"

CMeshO::CMeshO() :
	vcgTriMesh(),
	sfn(0), svn(0), pvn(0), pfn(0)
{
}

CMeshO::CMeshO(const CMeshO& oth) :
	vcgTriMesh(), sfn(oth.sfn), svn(oth.svn), 
	pvn(oth.pvn), pfn(oth.pfn), Tr(oth.Tr)
{
	vcg::tri::Append<vcgTriMesh, vcgTriMesh>::MeshAppendConst(*this, oth);
}

/// TODO: make a proper implementation of a move constructor.
/// Even if almost never used, this is very inefficient.
CMeshO::CMeshO(CMeshO&& oth): 
	vcgTriMesh(), sfn(oth.sfn), svn(oth.svn), 
	pvn(oth.pvn), pfn(oth.pfn), Tr(oth.Tr)
{
	//I could take everything from oth and place it in
	//this mesh
	vcg::tri::Append<vcgTriMesh, vcgTriMesh>::Mesh(*this, oth);
}

/// TODO: change this and use the copy&swap idiom
CMeshO& CMeshO::operator=(const CMeshO& oth)
{
	Clear();
	vcg::tri::Append<vcgTriMesh, vcgTriMesh>::MeshCopyConst(*this, oth);
	sfn = oth.sfn;
	svn = oth.svn;
	pvn = oth.pvn;
	pfn = oth.pfn;
	Tr = oth.Tr;
	return *this;
}

Box3m CMeshO::trBB() const
{
	Box3m bb;
	bb.Add(Tr,bbox);
	return bb;
}


