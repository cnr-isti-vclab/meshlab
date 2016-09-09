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

#ifndef EDITALIGN_MESHTREE_H
#define EDITALIGN_MESHTREE_H

#include <QObject>

#include <common/interfaces.h>
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
//  MeshNode() { m=0;id=-1;}
  bool glued;
  MeshModel *m;
  Matrix44m &tr() {return m->cm.Tr;}
  const Box3m &bbox() const {return m->cm.bbox;}
  int Id() {return m->id();}
};

class MeshTree
{
public:
  MeshTree();

  class Param
  {
  public:
    int OGSize;
    float arcThreshold;
    float recalcThreshold;
    Param()
    {
      OGSize = 50000;
      arcThreshold = 0.3f;
      recalcThreshold = 0.1f;
    }
  };

  QList<MeshNode *> nodeList;
  QList<vcg::AlignPair::Result> resultList;

  vcg::OccupancyGrid OG;
//  std::vector<vcg::AlignPair::Result> ResVec;
//  std::vector<vcg::AlignPair::Result *> ResVecPtr;
  vcg::CallBackPos * cb;

  MeshModel *MM(unsigned int i) {return nodeList.value(i)->m;}

  void clear()
  {
    foreach(MeshNode *mp, nodeList)
      delete mp;
    nodeList.clear();
    resultList.clear();
//    ResVecPtr.clear();
  }

  vcg::AlignPair::Result * findResult(int id1,int id2)
  {
    for(QList<vcg::AlignPair::Result>::iterator li=resultList.begin();li!=resultList.end();++li)
      if((li->MovName==id1 && li->FixName==id2) ||
         (li->MovName==id2 && li->FixName==id1) ) return &*li;
    return 0;
  }

  void deleteResult(MeshNode *mp)
  {
    QList<vcg::AlignPair::Result>::iterator li=resultList.begin();
    while(li!=resultList.end())
    {
      if(li->MovName==mp->Id() || li->FixName==mp->Id())
        li=resultList.erase(li);
      else ++li;
    }
  }

  MeshNode *find(int id)
  {
    foreach(MeshNode *mp, nodeList)
      if(mp->Id()==id) return mp;
    assert("You are trying to find an unexistent mesh"==0);
    return 0;
  }

  MeshNode *find(MeshModel *m)
  {
    foreach(MeshNode *mp, nodeList)
      if(mp->m==m) return mp;
    assert("You are trying to find an unexistent mesh"==0);
    return 0;
  }
  int gluedNum();


  void Process(vcg::AlignPair::Param &ap, Param &mtp);
  void ProcessGlobal(vcg::AlignPair::Param &ap);
  void ProcessArc(int fixId, int movId, vcg::AlignPair::Result &result, vcg::AlignPair::Param ap);
  void ProcessArc(int fixId, int movId, vcg::Matrix44d &MovToFix, vcg::AlignPair::Result &result, vcg::AlignPair::Param ap);

  inline Box3m bbox() {
    Box3m FullBBox;
    foreach(MeshNode *mp, nodeList)
      FullBBox.Add(Matrix44m::Construct(mp->tr()),mp->bbox());
    return FullBBox;
  }

  inline Box3m gluedBBox() {
    Box3m FullBBox;
    foreach(MeshNode *mp, nodeList)
      if(mp->glued)
        FullBBox.Add(Matrix44m::Construct(mp->tr()),mp->bbox());
    return FullBBox;
  }
};
#endif
