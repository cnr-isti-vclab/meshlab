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
Revision 1.32  2008/04/04 10:03:54  cignoni
Solved namespace ambiguities caused by the removal of a silly 'using namespace' in meshmodel.h

Revision 1.31  2008/02/12 14:23:11  cignoni
correct drawing selection bug

Revision 1.30  2008/02/04 09:32:56  cignoni
Added check to avoid drawing textures when mesh has no texture (and face color too)

Revision 1.29  2007/07/24 07:17:27  cignoni
moved matrix inside mesh class

Revision 1.28  2007/07/13 15:17:16  cignoni
Corrected per mesh color management (needed by align)

Revision 1.27  2007/07/10 07:19:29  cignoni
** Serious Changes **
again on the MeshDocument, the management of multiple meshes, layers, and per mesh transformation

Revision 1.26  2007/04/16 09:24:37  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing...

Revision 1.25  2007/03/20 16:22:34  cignoni
Big small change in accessing mesh interface. First step toward layers

Revision 1.24  2007/03/03 02:03:25  cignoni
Reformatted lower bar, added number of selected faces. Updated about dialog

Revision 1.23  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.22  2006/01/17 23:45:12  cignoni
Removed useless open function

Revision 1.21  2006/01/10 16:52:16  fmazzant
update ply::PlyMask -> io::Mask

Revision 1.20  2005/12/22 21:05:43  cignoni
Removed Optional Face Normal and added some initalization after opening

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
