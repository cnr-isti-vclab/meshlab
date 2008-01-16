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
/****************************************************************************
  History
$Log: meshedit.cpp,v $
****************************************************************************/

#ifndef EDITALIGN_MESHTREE_H
#define EDITALIGN_MESHTREE_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "align/AlignPair.h" 
#include "align/OccupancyGrid.h" 
#include <wrap/gui/trackball.h>

class MeshNode
{
public:
	MeshNode(MeshModel *_m, int _id)
	{
		m=_m;
		id = _id;
		glued=false;
	}
	MeshNode() { m=0;id=-1;}
	bool glued;
	int id;
  MeshModel *m;
	const Matrix44f &tr() const {return m->cm.Tr;}
	const Box3f &bbox() const {return m->cm.bbox;}
};

class MeshTree
{
	public:
	MeshTree();
	
  QList<MeshNode *> nodeList;
	vcg::OccupancyGrid OG;
	std::vector<AlignPair::Result> ResVec;
	std::vector<AlignPair::Result *> ResVecPtr;
	vcg::CallBackPos * cb;
	
  MeshModel *MM(unsigned int i) {return nodeList.value(i)->m;}
	
	void clear()
	{
		foreach(MeshNode *mp, nodeList) 
				delete mp;
		nodeList.clear();
		ResVec.clear();
		ResVecPtr.clear();
	}

	void MeshTree::resetID();

	MeshNode *find(int id)
		{
		foreach(MeshNode *mp, nodeList) 
		if(mp->id==id) return mp;
		assert("You are trying to find an unexistent mesh");
		return 0;
		}

	MeshNode *find(MeshModel *m)
		{
		foreach(MeshNode *mp, nodeList) 
		if(mp->m==m) return mp;
		assert("You are trying to find an unexistent mesh");
		return 0;
		}
	int gluedNum();

	
	void Process(AlignPair::Param ap);
	inline Box3f bbox() {
		Box3f FullBBox;
		foreach(MeshNode *mp, nodeList) 
			FullBBox.Add(Matrix44f::Construct(mp->tr()),mp->bbox());
		return FullBBox;
 }

	inline Box3f gluedBBox() {
		Box3f FullBBox;
		foreach(MeshNode *mp, nodeList) 
			if(mp->glued)
					FullBBox.Add(Matrix44f::Construct(mp->tr()),mp->bbox());
		return FullBBox;
	}


};
#endif