/****************************************************************************
* MeshLab                                                           o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2021                                           \/)\/    *
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

#include "mesh_model.h"

#include <wrap/gl/math.h>

#include <QDir>
#include <utility>

using namespace vcg;

MeshModel::MeshModel(unsigned int id, const QString& fullFileName, const QString& labelName) :
	idInsideFile(-1)
{
	/*glw.m = &(cm);*/
	clear();
	_id=id;
	if(!fullFileName.isEmpty())   this->fullPathFileName=fullFileName;
	if(!labelName.isEmpty())	 this->_label=labelName;
}

void MeshModel::clear()
{
	setMeshModified(false);
	// These data are always active on the mesh
	currentDataMask = MM_NONE;
	currentDataMask |= MM_VERTCOORD | MM_VERTNORMAL | MM_VERTFLAG ;
	currentDataMask |= MM_FACEVERT  | MM_FACENORMAL | MM_FACEFLAG ;

	visible=true;
	cm.Tr.SetIdentity();
	cm.sfn=0;
	cm.svn=0;
}

void MeshModel::updateBoxAndNormals()
{
	tri::UpdateBounding<CMeshO>::Box(cm);
	if(cm.fn>0) {
		tri::UpdateNormal<CMeshO>::PerFaceNormalized(cm);
		tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(cm);
	}
}

QString MeshModel::relativePathName(const QString& path) const
{
	QDir documentDir (path);
	QString relPath=documentDir.relativeFilePath(this->fullPathFileName);

	//if(relPath.size()>1 && relPath[0]=='.' &&  relPath[1]=='.')
	//	qDebug("Error we have a mesh that is not in the same folder of the project: %s ", qUtf8Printable(relPath));

	return relPath;
}

int MeshModel::io2mm(int single_iobit)
{
	switch(single_iobit)
	{
	case tri::io::Mask::IOM_NONE         : return  MM_NONE;
	case tri::io::Mask::IOM_VERTCOORD    : return  MM_VERTCOORD;
	case tri::io::Mask::IOM_VERTCOLOR    : return  MM_VERTCOLOR;
	case tri::io::Mask::IOM_VERTFLAGS    : return  MM_VERTFLAG;
	case tri::io::Mask::IOM_VERTQUALITY  : return  MM_VERTQUALITY;
	case tri::io::Mask::IOM_VERTNORMAL   : return  MM_VERTNORMAL;
	case tri::io::Mask::IOM_VERTTEXCOORD : return  MM_VERTTEXCOORD;
	case tri::io::Mask::IOM_VERTRADIUS   : return  MM_VERTRADIUS;

	case tri::io::Mask::IOM_FACEINDEX    : return  MM_FACEVERT  ;
	case tri::io::Mask::IOM_FACEFLAGS    : return  MM_FACEFLAG  ;
	case tri::io::Mask::IOM_FACECOLOR    : return  MM_FACECOLOR  ;
	case tri::io::Mask::IOM_FACEQUALITY  : return  MM_FACEQUALITY;
	case tri::io::Mask::IOM_FACENORMAL   : return  MM_FACENORMAL ;

	case tri::io::Mask::IOM_WEDGTEXCOORD : return  MM_WEDGTEXCOORD;
	case tri::io::Mask::IOM_WEDGCOLOR    : return  MM_WEDGCOLOR;
	case tri::io::Mask::IOM_WEDGNORMAL   : return  MM_WEDGNORMAL  ;

	case tri::io::Mask::IOM_BITPOLYGONAL : return  MM_POLYGONAL  ;

	default:
		assert(0);
		return MM_NONE;  // FIXME: Returning this is not the best solution (!)
		break;
	} ;
}


/**** DATAMASK STUFF ****/

bool MeshModel::hasDataMask(const int maskToBeTested) const
{
	return ((currentDataMask & maskToBeTested)!= 0);
}

void MeshModel::updateDataMask()
{
	currentDataMask = MM_NONE;
	currentDataMask |=
			MM_VERTCOORD | MM_VERTNORMAL | MM_VERTFLAG |
			MM_VERTQUALITY | MM_VERTCOLOR;
	currentDataMask |=
			MM_FACEVERT  | MM_FACENORMAL | MM_FACEFLAG ;
	if (cm.vert.IsVFAdjacencyEnabled())
		currentDataMask |= MM_VERTFACETOPO;
	if (cm.vert.IsMarkEnabled())
		currentDataMask |= MM_VERTMARK;
	if (cm.vert.IsTexCoordEnabled())
		currentDataMask |= MM_VERTTEXCOORD;
	if (cm.vert.IsCurvatureEnabled())
		currentDataMask |= MM_VERTCURV;
	if (cm.vert.IsCurvatureDirEnabled())
		currentDataMask |= MM_VERTCURVDIR;
	if (cm.vert.IsRadiusEnabled())
		currentDataMask |= MM_VERTRADIUS;
	if (cm.face.IsQualityEnabled())
		currentDataMask |= MM_FACEQUALITY;
	if (cm.face.IsMarkEnabled())
		currentDataMask |= MM_FACEMARK;
	if (cm.face.IsColorEnabled())
		currentDataMask |= MM_FACECOLOR;
	if (cm.face.IsFFAdjacencyEnabled())
		currentDataMask |= MM_FACEFACETOPO;
	if (cm.face.IsVFAdjacencyEnabled())
		currentDataMask |= MM_VERTFACETOPO;
	if (cm.face.IsCurvatureDirEnabled())
		currentDataMask |= MM_FACECURVDIR;
	if (cm.face.IsWedgeTexCoordEnabled())
		currentDataMask |= MM_WEDGTEXCOORD;
}

void MeshModel::updateDataMask(const MeshModel *m)
{
	updateDataMask(m->currentDataMask);
}

void MeshModel::updateDataMask(int neededDataMask)
{
	if((neededDataMask & MM_FACEFACETOPO)!=0)
	{
		cm.face.EnableFFAdjacency();
		tri::UpdateTopology<CMeshO>::FaceFace(cm);
	}
	if((neededDataMask & MM_VERTFACETOPO)!=0)
	{
		cm.vert.EnableVFAdjacency();
		cm.face.EnableVFAdjacency();
		tri::UpdateTopology<CMeshO>::VertexFace(cm);
	}

	if((neededDataMask & MM_WEDGTEXCOORD)!=0)
		cm.face.EnableWedgeTexCoord();
	if((neededDataMask & MM_FACECOLOR)!=0)
		cm.face.EnableColor();
	if((neededDataMask & MM_FACEQUALITY)!=0)
		cm.face.EnableQuality();
	if((neededDataMask & MM_FACECURVDIR)!=0)
		cm.face.EnableCurvatureDir();
	if((neededDataMask & MM_FACEMARK)!=0)
		cm.face.EnableMark();
	if((neededDataMask & MM_VERTMARK)!=0)
		cm.vert.EnableMark();
	if((neededDataMask & MM_VERTCURV)!=0)
		cm.vert.EnableCurvature();
	if((neededDataMask & MM_VERTCURVDIR)!=0)
		cm.vert.EnableCurvatureDir();
	if((neededDataMask & MM_VERTRADIUS)!=0)
		cm.vert.EnableRadius();
	if((neededDataMask & MM_VERTTEXCOORD)!=0)
		cm.vert.EnableTexCoord();

	currentDataMask |= neededDataMask;
}

void MeshModel::clearDataMask(int unneededDataMask)
{
	if( ( (unneededDataMask & MM_VERTFACETOPO)!=0)	&& hasDataMask(MM_VERTFACETOPO)) {cm.face.DisableVFAdjacency();
	cm.vert.DisableVFAdjacency(); }
	if( ( (unneededDataMask & MM_FACEFACETOPO)!=0)	&& hasDataMask(MM_FACEFACETOPO))	cm.face.DisableFFAdjacency();

	if( ( (unneededDataMask & MM_WEDGTEXCOORD)!=0)	&& hasDataMask(MM_WEDGTEXCOORD)) 	cm.face.DisableWedgeTexCoord();
	if( ( (unneededDataMask & MM_FACECOLOR)!=0)			&& hasDataMask(MM_FACECOLOR))			cm.face.DisableColor();
	if( ( (unneededDataMask & MM_FACEQUALITY)!=0)		&& hasDataMask(MM_FACEQUALITY))		cm.face.DisableQuality();
	if( ( (unneededDataMask & MM_FACEMARK)!=0)			&& hasDataMask(MM_FACEMARK))			cm.face.DisableMark();
	if( ( (unneededDataMask & MM_VERTMARK)!=0)			&& hasDataMask(MM_VERTMARK))			cm.vert.DisableMark();
	if( ( (unneededDataMask & MM_VERTCURV)!=0)			&& hasDataMask(MM_VERTCURV))			cm.vert.DisableCurvature();
	if( ( (unneededDataMask & MM_VERTCURVDIR)!=0)		&& hasDataMask(MM_VERTCURVDIR))		cm.vert.DisableCurvatureDir();
	if( ( (unneededDataMask & MM_VERTRADIUS)!=0)		&& hasDataMask(MM_VERTRADIUS))		cm.vert.DisableRadius();
	if( ( (unneededDataMask & MM_VERTTEXCOORD)!=0)	&& hasDataMask(MM_VERTTEXCOORD))	cm.vert.DisableTexCoord();

	currentDataMask = currentDataMask & (~unneededDataMask);
}

void MeshModel::enable(int openingFileMask)
{
	if( openingFileMask & tri::io::Mask::IOM_VERTTEXCOORD )
		updateDataMask(MM_VERTTEXCOORD);
	if( openingFileMask & tri::io::Mask::IOM_WEDGTEXCOORD )
		updateDataMask(MM_WEDGTEXCOORD);
	if( openingFileMask & tri::io::Mask::IOM_VERTCOLOR	)
		updateDataMask(MM_VERTCOLOR);
	if( openingFileMask & tri::io::Mask::IOM_FACECOLOR	)
		updateDataMask(MM_FACECOLOR);
	if( openingFileMask & tri::io::Mask::IOM_VERTRADIUS   ) updateDataMask(MM_VERTRADIUS);
	if( openingFileMask & tri::io::Mask::IOM_CAMERA	   ) updateDataMask(MM_CAMERA);
	if( openingFileMask & tri::io::Mask::IOM_VERTQUALITY  ) updateDataMask(MM_VERTQUALITY);
	if( openingFileMask & tri::io::Mask::IOM_FACEQUALITY  ) updateDataMask(MM_FACEQUALITY);
	if( openingFileMask & tri::io::Mask::IOM_BITPOLYGONAL ) updateDataMask(MM_POLYGONAL);
}

bool MeshModel::meshModified() const
{
	return modified;
}

void MeshModel::setMeshModified(bool b)
{
	modified = b;
}

int MeshModel::dataMask() const
{
	return currentDataMask;
}
