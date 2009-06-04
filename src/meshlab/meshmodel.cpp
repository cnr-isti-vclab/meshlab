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


int MeshModel::io2mm(int single_iobit)
{
	switch(single_iobit) 
	{
		case tri::io::Mask::IOM_NONE					: return  MM_NONE;
		case tri::io::Mask::IOM_VERTCOORD		: return  MM_VERTCOORD;
		case tri::io::Mask::IOM_VERTCOLOR		: return  MM_VERTCOLOR;
		case tri::io::Mask::IOM_VERTFLAGS		: return  MM_VERTFLAG;
		case tri::io::Mask::IOM_VERTQUALITY	: return  MM_VERTQUALITY;
		case tri::io::Mask::IOM_VERTNORMAL		: return  MM_VERTNORMAL;
		case tri::io::Mask::IOM_VERTTEXCOORD : return  MM_VERTTEXCOORD;
		case tri::io::Mask::IOM_VERTRADIUS		: return  MM_VERTRADIUS;
		
		case tri::io::Mask::IOM_FACEINDEX   		: return  MM_FACEVERT  ;
		case tri::io::Mask::IOM_FACEFLAGS   		: return  MM_FACEFLAG  ;
		case tri::io::Mask::IOM_FACECOLOR   		: return  MM_FACECOLOR  ;
		case tri::io::Mask::IOM_FACEQUALITY 		: return  MM_FACEQUALITY;
		case tri::io::Mask::IOM_FACENORMAL  		: return  MM_FACENORMAL ;
		
		case tri::io::Mask::IOM_WEDGTEXCOORD 		: return  MM_WEDGTEXCOORD;
		case tri::io::Mask::IOM_WEDGCOLOR				: return  MM_WEDGCOLOR;
		case tri::io::Mask::IOM_WEDGNORMAL   		: return  MM_WEDGNORMAL  ;

		default:
			assert(0);
			return MM_NONE;  // FIXME: Returning this is not the best solution (!)
			break;
	} ;
}

const QStringList MeshModel::getStringListFromEnumMask( const int mask )
{
	QStringList lst;
	if (mask & MM_VERTCOORD)
		lst.append("MM_VERTCOORD");
	
	if (mask & MM_VERTNORMAL)
		lst.append("MM_VERTNORMAL");

	if (mask & MM_VERTFLAG)
		lst.append("MM_VERTFLAG");

	if (mask & MM_VERTCOLOR)
		lst.append("MM_VERTCOLOR");

	if (mask & MM_VERTFLAG)
		lst.append("MM_VERTQUALITY");

	if (mask & MM_VERTMARK)
		lst.append("MM_VERTMARK");
	
	if (mask & MM_VERTFLAG)
		lst.append("MM_VERTFACETOPO");

	if (mask & MM_VERTCURV)
		lst.append("MM_VERTCURV");
	
	if (mask & MM_VERTCURVDIR)
		lst.append("MM_VERTCURVDIR");

	if (mask & MM_VERTRADIUS)
		lst.append("MM_VERTRADIUS");
		
	if (mask & MM_VERTTEXCOORD)
		lst.append("MM_VERTTEXCOORD");

	if (mask & MM_FACEVERT)
		lst.append("MM_FACEVERT");

	if (mask & MM_FACENORMAL)
		lst.append("MM_FACENORMAL");

	if (mask & MM_FACENORMAL)
		lst.append("MM_FACENORMAL");

	if (mask & MM_FACEFLAG)
		lst.append("MM_FACEFLAG");
		
	if (mask & MM_FACECOLOR)
		lst.append("MM_FACECOLOR");

	if (mask & MM_FACEQUALITY)
		lst.append("MM_FACEQUALITY");

	if (mask & MM_FACEMARK)
		lst.append("MM_FACEMARK");

	if (mask & MM_FACEFACETOPO)
		lst.append("MM_FACEFACETOPO");

	if (mask & MM_WEDGTEXCOORD)
		lst.append("MM_WEDGTEXCOORD");

	if (mask & MM_WEDGNORMAL)
		lst.append("MM_WEDGNORMAL");

	if (mask & MM_WEDGCOLOR)
		lst.append("MM_WEDGCOLOR");
		
	if (mask & MM_VERTFLAGSELECT)
		lst.append("MM_VERTFLAGSELECT");

	if (mask & MM_FACEFLAGSELECT)
		lst.append("MM_FACEFLAGSELECT");

	return lst;
}
