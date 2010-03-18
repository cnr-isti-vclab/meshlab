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
#include <common/pluginmanager.h>



using namespace vcg;

QString SampleMeshDecoratePlugin::filterInfo(QAction *action) const
{
	switch(ID(action))
	{
	case DP_SHOW_CUBEMAPPED_ENV :      return tr("Draws a customizable cube mapped background that is sync with trackball rotation");
	}
	assert(0);
	return QString();
}
QString SampleMeshDecoratePlugin::filterName(FilterIDType id) const
{
	switch(id)
	{
	case DP_SHOW_CUBEMAPPED_ENV :      return tr("Cube mapped background");
	}
	assert(0);
	return QString();
}

void SampleMeshDecoratePlugin::initGlobalParameterSet(QAction *, RichParameterSet &parset) 
{
	assert(!parset.hasParameter(CubeMapPathParam()));
	QString cubemapDirPath = PluginManager::getBaseDirPath() + QString("/textures/cubemaps/uffizi.jpg");
	parset.addParam(new RichString(CubeMapPathParam(), cubemapDirPath,"",""));
}		
		
bool SampleMeshDecoratePlugin::StartDecorate( QAction * /*mode*/, MeshModel &/*m*/, RichParameterSet * parset, GLArea * /*parent*/ )
{
	assert(parset->hasParameter(CubeMapPathParam()));
	basename = parset->getString(CubeMapPathParam());
	if(parset->findParameter(CubeMapPathParam())== NULL) 	
		qDebug("CubeMapPath was not setted!!!");

	return true;
}

void SampleMeshDecoratePlugin::Decorate(QAction *a, MeshModel &/*m*/, GLArea *gla, QFont /*qf*/)
{
	static QString lastname("unitialized");
	switch(ID(a))
	{
	  case DP_SHOW_CUBEMAPPED_ENV :
		{
			if(a->text() != filterName(DP_SHOW_CUBEMAPPED_ENV))	assert(0);
			if(!cm.IsValid())
			{
				if(lastname != basename ) 
				{
					qDebug( "Current CubeMapPath Dir: %s ",qPrintable(basename)); 
					glewInit();
					bool ret = cm.Load(qPrintable(basename));
					lastname=basename;
					if(! ret ) QMessageBox::warning(gla,"Cubemapped background decoration","Warning unable to load cube map images: " + basename );
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
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
				cm.DrawEnvCube(tr);
			glPopMatrix();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			}
	}
}

Q_EXPORT_PLUGIN(SampleMeshDecoratePlugin)
