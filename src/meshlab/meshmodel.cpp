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

#include "meshmodel.h"
#include <QString>
#include <QtGlobal>

bool MeshModel::Render(GLW::DrawMode dm, GLW::ColorMode cm, GLW::TextureMode tm)
{
  glColor3f(.8f,.8f,.8f);
  glw.Draw(dm,cm,tm);
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
  glBegin(GL_TRIANGLES);
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
	glPopAttrib();
  return true;
}
