/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
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
	MeshNode(MeshModel *_m)
	{
		m=_m;
		glued=false;
	}
	MeshNode() { m=0;}
	bool glued;
  MeshModel *m;
	const Matrix44f &tr() const {return m->cm.Tr;}
	const Box3f &bbox() const {return m->cm.bbox;}
};

class MeshTree
{
	public:
  QList<MeshNode *> nodeList;
	vcg::OccupancyGrid OG;
	std::vector<AlignPair::Result> ResVec;
	std::vector<AlignPair::Result *> ResVecPtr;
  MeshModel *MM(unsigned int i) {return nodeList.value(i)->m;}
	void clear()
	{
		foreach(MeshNode *mp, nodeList) 
				delete mp;
		nodeList.clear();
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