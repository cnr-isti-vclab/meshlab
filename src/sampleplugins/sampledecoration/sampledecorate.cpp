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

#include <meshlab/glarea.h>



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
 
void SampleMeshDecoratePlugin::initGlobalParameterSet(QAction *, FilterParameterSet *parset) 
{
	if(parset->findParameter("CubeMapPath")!= NULL) 
	{
		qDebug("CubeMapPath already setted. Doing nothing");
		return;
	}
	QString cubemapDirPath = MainWindowInterface::getBaseDirPath() + QString("/textures/cubemaps/uffizi.jpg");
	
	//parset->addString("CubeMapPath", "/Users/cignoni/devel/meshlab/src/meshlab/textures/cubemaps/uffizi.jpg");
	parset->addString("CubeMapPath", cubemapDirPath);
	
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

void SampleMeshDecoratePlugin::Decorate(QAction *a, MeshModel &m, FilterParameterSet *par, GLArea *gla, QFont /*qf*/)
{
 assert(par);
	static QString lastname("unitialized");
  if(a->text() != ST(DP_SHOW_CUBEMAPPED_ENV))	assert(0);
	
	QString basename;
	if(!cm.IsValid())
	{
		assert(par->findParameter("CubeMapPath")); 
		basename=par->getString("CubeMapPath");
		if(lastname != basename ) 
		{
			qDebug( "Current CubeMapPath Dir: %s ",qPrintable(basename)); 

			glewInit();
			bool ret = cm.Load(qPrintable(basename));
			lastname=basename;
			if(! ret ) 
				QMessageBox::warning(gla,"Cubemapped background decoration","Warning unable to load cube map images: " + basename );
		//	cm.radius= m.cm.bbox.Diag()/2;
			cm.radius=10;
		}
	}
	if(!cm.IsValid()) return;
	Matrix44f tr;
	glGetv(GL_MODELVIEW_MATRIX,tr);
	
	// Remove the translation from the current matrix by simply padding the last column of the matrix
 	tr[0][3]=0;
	tr[1][3]=0;
	tr[2][3]=0;
	tr[3][3]=1;
	
	//Remove the scaling from the the current matrix by adding an inverse scaling matrix
	float scale = 1.0/pow(tr.Determinant(),1.0f/3.0f);
	Matrix44f Scale; 
	Scale.SetDiagonal(scale);
	tr=tr*Scale;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	GLfloat fAspect = (GLfloat)gla->width()/ gla->height();
	float ratio= gla->ratio;
	if(gla->fov==5)		glOrtho(-ratio*fAspect,ratio*fAspect,-ratio,ratio,0, 15);
	   else    		gluPerspective(gla->fov, fAspect, .1, 20);

  glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		cm.DrawEnvCube(tr);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

Q_EXPORT_PLUGIN(SampleMeshDecoratePlugin)
