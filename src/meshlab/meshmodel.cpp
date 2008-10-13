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


#include <QString>
#include <QtGlobal>
#include <QFileInfo>
#include "meshmodel.h"
#include <wrap/gl/math.h>

using namespace vcg;

MeshModel *MeshDocument::getMesh(const char *name)
{ 
	foreach(MeshModel *mmp, meshList)
			{
				QString shortName( QFileInfo(mmp->fileName.c_str()).fileName() );
				if(shortName == name) return mmp;
			}
	assert(0);
	return 0;
}

void MeshDocument::setCurrentMesh(unsigned int i)
{
  assert(i < (unsigned int)meshList.size());
	currentMesh=meshList.at(i);
	emit currentMeshChanged(i);
}

MeshModel *MeshDocument::addNewMesh(const char *meshName,MeshModel *newMesh)
{
	QString newName=meshName;
	
	for(QList<MeshModel*>::iterator mmi=meshList.begin();mmi!=meshList.end();++mmi)
	{
		QString shortName( (*mmi)->fileName.c_str() );
		if(shortName == newName) 
			newName = newName+"_copy";
	}
	
	if(newMesh==0) 
		newMesh=new MeshModel(qPrintable(newName));
	else 
		newMesh->fileName = qPrintable(newName);
	
	meshList.push_back(newMesh);
	currentMesh=meshList.back();
	emit currentMeshChanged(meshList.size()-1);
	
	return newMesh;
}

bool MeshDocument::delMesh(MeshModel *mmToDel)
{
	if(meshList.size()==1) return false; 

	QMutableListIterator<MeshModel *> i(meshList);
	
	while (i.hasNext())
	{
		MeshModel *md = i.next();
		
		if (md==mmToDel) 
		{
			i.remove();
			delete mmToDel;
		}
	}
	
	if(currentMesh == mmToDel)
	{
		setCurrentMesh(0);
		emit currentMeshChanged(0);
	}

	return true;
}

bool MeshModel::Render(GLW::DrawMode _dm, GLW::ColorMode _cm, GLW::TextureMode _tm)
{
  glPushMatrix();
	glMultMatrix(cm.Tr);
	if( (_cm == GLW::CMPerFace)  && (!tri::HasPerFaceColor(cm)) ) _cm=GLW::CMNone;
	if( (_tm == GLW::TMPerWedge )&& (!tri::HasPerWedgeTexCoord(cm)) ) _tm=GLW::TMNone;
	if( (_tm == GLW::TMPerWedgeMulti )&& (!tri::HasPerWedgeTexCoord(cm)) ) _tm=GLW::TMNone;
  glw.Draw(_dm,_cm,_tm);
	glPopMatrix();
  return true;
}

bool MeshModel::RenderSelectedFaces()
{
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
  glColor4f(1.0f,0.0,0.0,.3f);
  glPolygonOffset(-1.0, -1);
  CMeshO::FaceIterator fi;
	glPushMatrix();
	glMultMatrix(cm.Tr);glBegin(GL_TRIANGLES);
	cm.sfn=0;
	for(fi=cm.face.begin();fi!=cm.face.end();++fi)
    if(!(*fi).IsD() && (*fi).IsS())
    {
  		glVertex((*fi).cP(0));
  		glVertex((*fi).cP(1));
  		glVertex((*fi).cP(2));
			++cm.sfn;
    }
  glEnd();
	glPopMatrix();
	glPopAttrib();
  return true;
}
