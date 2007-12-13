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
sampledecorate.cpp,v
Revision 1.1  2007/12/13 00:33:55  cignoni
New small samples

****************************************************************************/

#include <QtGui>

#include <math.h>
#include <limits>
#include <stdlib.h>
#include "sampledecorate.h"
#include <QGLWidget>
#include <wrap/gl/addons.h>



using namespace vcg;

const QString SampleMeshDecoratePlugin::Info(QAction *action)
 {
  switch(ID(action))
  {
    case DP_SHOW_CUBEMAPPED_ENV :      return tr("Draws a cube mapped background");
 	 }
  assert(0);
  return QString();
 }

 const PluginInfo &SampleMeshDecoratePlugin::Info()
{
   static PluginInfo ai;
   ai.Date=tr("Dec 2007");
	 ai.Author=tr("Paolo Cignoni");
	 ai.Version=tr("1.0");
   return ai;
 }

const QString SampleMeshDecoratePlugin::ST(FilterIDType filter) const
{
  switch(filter)
  {
    case DP_SHOW_CUBEMAPPED_ENV      : return QString("Cubemapped Environment");
    default: assert(0);
  }
  return QString("error!");
}

void SampleMeshDecoratePlugin::Decorate(QAction *a, MeshModel &m, RenderMode &/*rm*/, QGLWidget *gla, QFont qf)
{
  if(a->text() != ST(DP_SHOW_CUBEMAPPED_ENV))	assert(0);
	if(!cm.IsValid())
	{
		glewInit();
		cm.Load("/Users/cignoni/devel/meshlab/src/sampleplugins/sampledecoration/cubemaps/uffizi.jpg");
		cm.radius= m.cm.bbox.Diag()/2;
	}
	Matrix44f tr;
	glGetv(GL_MODELVIEW_MATRIX,tr);
	
	tr[0][3]=0;
	tr[1][3]=0;
	tr[2][3]=0;
	tr[3][3]=1;
	float scale = 1.0/pow(tr.Determinant(),1.0/3.0);
	Matrix44f Scale; 
	Scale.SetDiagonal(scale);
	tr=tr*Scale;
	glPushMatrix();
		cm.DrawEnvCube(tr);
	glPopMatrix();
}

Q_EXPORT_PLUGIN(SampleMeshDecoratePlugin)
