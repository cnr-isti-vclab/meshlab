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
Revision 1.4  2005/12/12 22:48:42  cignoni
Added plugin info methods

Revision 1.3  2005/12/12 11:19:41  cignoni
Added bbox corners and axis,
cleaned up the identification between by string of decorations

****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "meshdecorate.h"

using namespace vcg;
const ActionInfo &ExtraMeshDecoratePlugin::Info(QAction *) 
 {
   ActionInfo ai; 
   ai.Help=tr("Generic Help for an action");
   return ai;
 }

 const PluginInfo &ExtraMeshDecoratePlugin::Info() 
{
   PluginInfo ai; 
   ai.Date=tr("__DATE__");
   return ai;
 }
 
const QString ExtraMeshDecoratePlugin::ST(int id) const
{
  switch(id)
  {
    case DP_SHOW_NORMALS      : return QString("Show Normals");
    case DP_SHOW_BOX_CORNERS  : return QString("Show Box Corners");
    case DP_SHOW_AXIS         : return QString("Show Axis");
    default: assert(0);
  }
  return QString("error!");
}

void ExtraMeshDecoratePlugin::Decorate(QAction *a, MeshModel &m, RenderMode &/*rm*/, GLArea * /*gla*/) 
{
	if(a->text() == ST(DP_SHOW_NORMALS))
	{
    glPushAttrib(GL_ENABLE_BIT );
    float LineLen = m.cm.bbox.Diag()/20.0;
    CMeshO::VertexIterator vi;
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
    {
      glVertex((*vi).P());
      glVertex((*vi).P()+(*vi).N()*LineLen);
    }
    glEnd();
   glPopAttrib();
  }
  if(a->text() == ST(DP_SHOW_BOX_CORNERS)) DrawBBoxCorner(m);
  if(a->text() == ST(DP_SHOW_AXIS)) DrawAxis(m);
}
 
void ExtraMeshDecoratePlugin::DrawBBoxCorner(MeshModel &m)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0);
	glColor(Color4b::Cyan);
	Box3f b=m.cm.bbox;
	Point3f mi=b.min;
	Point3f ma=b.max;
	Point3f d3=(b.max-b.min)/4.0;
	Point3f zz(0,0,0);
	glBegin(GL_LINES);
	glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+d3[0],mi[1]+zz[1],mi[2]+zz[2]); 
	glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+zz[0],mi[1]+d3[1],mi[2]+zz[2]);
	glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+zz[0],mi[1]+zz[1],mi[2]+d3[2]);
																																		
	glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]-d3[0],mi[1]+zz[1],mi[2]+zz[2]);
	glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]+zz[0],mi[1]+d3[1],mi[2]+zz[2]);
	glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]+zz[0],mi[1]+zz[1],mi[2]+d3[2]);
																																		
  glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+d3[0],ma[1]+zz[1],mi[2]+zz[2]);
	glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+zz[0],ma[1]-d3[1],mi[2]+zz[2]);
	glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+zz[0],ma[1]+zz[1],mi[2]+d3[2]);
																																		
	glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]-d3[0],ma[1]+zz[1],mi[2]+zz[2]);
	glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]+zz[0],ma[1]-d3[1],mi[2]+zz[2]);
	glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]+zz[0],ma[1]+zz[1],mi[2]+d3[2]);
																																		
	glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+d3[0],mi[1]+zz[1],ma[2]+zz[2]);
	glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+zz[0],mi[1]+d3[1],ma[2]+zz[2]);
	glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+zz[0],mi[1]+zz[1],ma[2]-d3[2]);
																																		
	glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]-d3[0],mi[1]+zz[1],ma[2]+zz[2]);
	glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]+zz[0],mi[1]+d3[1],ma[2]+zz[2]);
	glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]+zz[0],mi[1]+zz[1],ma[2]-d3[2]);
																																		
	glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+d3[0],ma[1]+zz[1],ma[2]+zz[2]);
	glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+zz[0],ma[1]-d3[1],ma[2]+zz[2]);
	glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+zz[0],ma[1]+zz[1],ma[2]-d3[2]);
																																		
	glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]-d3[0],ma[1]+zz[1],ma[2]+zz[2]);
	glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]+zz[0],ma[1]-d3[1],ma[2]+zz[2]);
	glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]+zz[0],ma[1]+zz[1],ma[2]-d3[2]);
	
	glEnd();
	glPopAttrib();
}


void ExtraMeshDecoratePlugin::DrawAxis(MeshModel &m)
{
	float hw=m.cm.bbox.Diag()/2.0;
		glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2.0);
	glBegin(GL_LINES);
		glColor(Color4b::Red);
		glVertex3f(-hw,0,0);
		glVertex3f(+hw,0,0);
		glColor(Color4b::Green);
		glVertex3f(0,-hw,0);
		glVertex3f(0,+hw,0);
		glColor(Color4b::Blue);
		glVertex3f(0,0,-hw);
		glVertex3f(0,0,+hw);
	glEnd();
	glDepthRange(0.0f,1.0f);
	glPopAttrib();

}

Q_EXPORT_PLUGIN(ExtraMeshDecoratePlugin)
