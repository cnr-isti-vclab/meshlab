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
Revision 1.7  2005/11/21 12:12:54  cignoni
Added copyright info

****************************************************************************/

#include "meshmodel.h"
#include<vcg/complex/trimesh/update/bounding.h>
#include <QtGlobal>
bool MeshModel::Open(const char *filename)
{
  int mask;
  vcg::tri::io::ImporterPLY<CMeshO>::LoadMask(filename, mask);
  if(mask&ply::PLYMask::PM_VERTQUALITY) printf("Has Vertex Quality\n");
  if(mask&ply::PLYMask::PM_FACEQUALITY) printf("Has Face Quality\n");
  if(mask&ply::PLYMask::PM_FACECOLOR) qDebug("Has Face Color\n");
  if(mask&ply::PLYMask::PM_VERTCOLOR) qDebug("Has Vertex Color\n");
  if(mask&ply::PLYMask::PM_WEDGTEXCOORD) 
  {
    qDebug("Has Wedge Text Coords\n");
    cm.face.EnableWedgeTex();
  }
  int ret=vcg::tri::io::ImporterPLY<CMeshO>::Open(cm,filename);
  qDebug("FAce 0 %f %f \n",cm.face[0].WT(0).u(),cm.face[0].WT(0).v());
	vcg::tri::UpdateBounding<CMeshO>::Box(cm);
	vcg::tri::UpdateNormals<CMeshO>::PerVertex(cm);
	vcg::tri::UpdateNormals<CMeshO>::PerFace(cm);
	
  

  return ret==::vcg::ply::E_NOERROR;
}


bool MeshModel::Render(GLW::DrawMode dm, GLW::ColorMode cm)
{
  glw.Draw(dm,cm,GLW::TMNone);
  return true;
}
