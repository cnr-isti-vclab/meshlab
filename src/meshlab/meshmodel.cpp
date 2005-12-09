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
$Log$
Revision 1.18  2005/12/09 00:26:25  buzzelli
io importing mechanism adapted in order to be fully transparent towards the user

Revision 1.17  2005/12/07 00:56:40  fmazzant
added support for exporter generic obj file (level base)

Revision 1.16  2005/12/06 16:27:43  fmazzant
added obj file in generic open dialog

Revision 1.15  2005/12/04 00:22:46  cignoni
Switched from progresBar widget to progressbar dialog

Revision 1.14  2005/12/02 00:54:13  cignoni
Added TextureMode in render

Revision 1.13  2005/11/25 11:55:59  alemochi
Added function to Enable/Disable lighting (work in progress)

Revision 1.12  2005/11/24 01:45:28  cignoni
commented line 62. dangerous unuseful debug line.

Revision 1.11  2005/11/24 01:38:36  cignoni
Added new plugins intefaces, tested with shownormal render mode

Revision 1.10  2005/11/23 00:04:03  cignoni
added hint for better hiddenline

Revision 1.9  2005/11/22 11:40:14  glvertex
Now using a single method to compute normals (PerVertePerFace instead PerVertex then PerFace)

Revision 1.8  2005/11/21 22:09:35  cignoni
added missing enablenormal

Revision 1.7  2005/11/21 12:12:54  cignoni
Added copyright info

****************************************************************************/

#include "meshmodel.h"

#include <vcg/complex/trimesh/update/bounding.h>
#include "../test/io/import_obj.h"
#include "../test/io/export_obj.h"
#include <QString>
#include <QtGlobal>

bool MeshModel::Open(const char *filename, vcg::CallBackPos *cb)
{
  int mask;
  QString f = QString(filename);

	vcg::tri::io::ImporterPLY<CMeshO>::LoadMask(filename, mask); 
  
  if(mask&ply::PLYMask::PM_VERTQUALITY) qDebug("Has Vertex Quality\n");
  if(mask&ply::PLYMask::PM_FACEQUALITY) qDebug("Has Face Quality\n");
  if(mask&ply::PLYMask::PM_FACECOLOR) qDebug("Has Face Color\n");
  if(mask&ply::PLYMask::PM_VERTCOLOR) qDebug("Has Vertex Color\n");
  if(mask&ply::PLYMask::PM_WEDGTEXCOORD) 
  {
    qDebug("Has Wedge Text Coords\n");
    cm.face.EnableWedgeTex();
  }

	cm.face.EnableNormal();
	

	int ret;
	ret = vcg::tri::io::ImporterPLY<CMeshO>::Open(cm,filename,cb);
	
	//qDebug("Face 0 %f %f \n",cm.face[0].WT(0).u(),cm.face[0].WT(0).v());
	
	vcg::tri::UpdateBounding<CMeshO>::Box(cm);
	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(cm);
	
  return ret==::vcg::ply::E_NOERROR;
}

bool MeshModel::Save(const char *filename,CallBackPos *cb)
{
	QString fileName(filename);

	if(fileName.endsWith(".obj",Qt::CaseInsensitive))
		return vcg::tri::io::ExporterOBJ<CMeshO>::Save(this->cm,filename,cb);

	if(fileName.endsWith(".ply",Qt::CaseInsensitive))
		return vcg::tri::io::ExporterPLY<CMeshO>::Save(this->cm,filename,cb);

	return false;
}

bool MeshModel::Render(GLW::DrawMode dm, GLW::ColorMode cm, GLW::TextureMode tm)
{
  glw.SetHintParamf(GLW::HNPZTwist,0.0005f); //
  glColor3f(.8f,.8f,.8f);
  glw.Draw(dm,cm,tm);
  return true;
}
