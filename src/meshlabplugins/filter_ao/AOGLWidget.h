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

#ifndef __VCG_AOGLWIDGET
#define __VCG_AOGLWIDGET

#include <GL/glew.h>
#include <QGLWidget>

#include <common/interfaces.h>

class AmbientOcclusionPlugin;

class AOGLWidget: public QGLWidget
{
	Q_OBJECT 
public:
	AOGLWidget (QWidget * parent, AmbientOcclusionPlugin *_plugin);
	AmbientOcclusionPlugin * plugin;
	vcg::CallBackPos *cb;
	MeshModel *m;
	std::vector<vcg::Point3f> *viewDirVec;
	
public slots:
	protected:
		/// opengl initialization and drawing calls
		void initializeGL ();
		void paintGL ();
	
};

#endif
