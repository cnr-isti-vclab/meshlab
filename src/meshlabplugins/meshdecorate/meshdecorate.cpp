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
Revision 1.10  2006/01/19 23:56:44  glvertex
Starting quoted box (simply draws xyz axes)

Revision 1.9  2006/01/17 10:43:48  cignoni
removed '' from DATE macro

Revision 1.8  2006/01/07 11:49:00  glvertex
Added actions descriptions

Revision 1.7  2006/01/07 11:32:05  glvertex
Disabled textures when drawing corners,axis and normals lines
Enhanced blending normals

Revision 1.6  2006/01/02 16:44:38  glvertex
Blending normals

Revision 1.5  2005/12/13 11:02:56  cignoni
made info structs static

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
const ActionInfo &ExtraMeshDecoratePlugin::Info(QAction *action) 
 {
   static ActionInfo ai;

	 if(action->text() == ST(DP_SHOW_AXIS))
	 {
		 ai.Help = tr("Draws XYZ axes in world coordinates");
		 ai.ShortHelp = tr("Draws XYZ axes in world coordinates");
	 }

	 if(action->text() == ST(DP_SHOW_BOX_CORNERS))
	 {
		 ai.Help = tr("Draws object's bounding box corners");
		 ai.ShortHelp = tr("Draws object's bounding box corners");
	 }

	 if(action->text() == ST(DP_SHOW_NORMALS))
	 {
		 ai.Help = tr("Draws object vertex normals");
		 ai.ShortHelp = tr("Draws object vertex normals");
	 }

	 if(action->text() == ST(DP_SHOW_QUOTED_BOX))
	 {
		 ai.Help = tr("Draws quoted box");
		 ai.ShortHelp = tr("Draws quoted box");
	 }

   return ai;
 }

 const PluginInfo &ExtraMeshDecoratePlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr("January 2006");
   return ai;
 }
 
const QString ExtraMeshDecoratePlugin::ST(int id) const
{
  switch(id)
  {
    case DP_SHOW_NORMALS      : return QString("Show Normals");
    case DP_SHOW_BOX_CORNERS  : return QString("Show Box Corners");
    case DP_SHOW_AXIS         : return QString("Show Axis");
		case DP_SHOW_QUOTED_BOX		:	return QString("Show Quoted Box");
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
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_LINES);
		glColor4f(.4f,.4f,1.f,.3f);
    for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
    {
      glVertex((*vi).P());
      glVertex((*vi).P()+(*vi).N()*LineLen);
    }
    glEnd();
   glPopAttrib();
  }
  if(a->text() == ST(DP_SHOW_BOX_CORNERS))	DrawBBoxCorner(m);
  if(a->text() == ST(DP_SHOW_AXIS))					DrawAxis(m);
	if(a->text() == ST(DP_SHOW_QUOTED_BOX))		DrawQuotedBox(m);
}

void ExtraMeshDecoratePlugin::DrawQuotedBox(MeshModel &m)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0);

	// Mesh boundingBox
	Box3f b=m.cm.bbox;
	float spanX = b.DimX()*.5f;
	float spanY = b.DimY()*.5f;
	float spanZ = b.DimZ()*.5f;

	// bbox center
	Point3f c = b.Center();

	glColor4f(.5f,1.f,.5f,.8f);
	glBegin(GL_LINES);
		glVertex3f(c[0]-spanX,c[1],c[2]);	glVertex3f(c[0]+spanX,c[1],c[2]);
		glVertex3f(c[0],c[1]-spanY,c[2]);	glVertex3f(c[0],c[1]+spanY,c[2]);
		glVertex3f(c[0],c[1],c[2]-spanZ);	glVertex3f(c[0],c[1],c[2]+spanZ);
	glEnd();

	glPopAttrib();

}
 
void ExtraMeshDecoratePlugin::DrawBBoxCorner(MeshModel &m)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
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
		glDisable(GL_TEXTURE_2D);
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
