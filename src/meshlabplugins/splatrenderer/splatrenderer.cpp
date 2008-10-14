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

#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "splatrenderer.h"
#include <QGLWidget>
#include <QTextStream>
#include <wrap/gl/trimesh.h>
#include <iostream>

using namespace std;
using namespace vcg;

void SplatRendererPlugin::initActionList()
{
	actionList << new QAction("Splatting", this);
}

QString SplatRendererPlugin::loadSource(const QString& func,const QString& filename)
{
	QString res;
	QFile f(":/SplatRenderer/shaders/" + filename);
	if (!f.open(QFile::ReadOnly))
	{
		std::cerr << "failed to load shader file " << filename.toAscii().data() << "\n";
		return res;
	}
	QTextStream stream(&f);
	res = stream.readAll();
	f.close();
	res = QString("#define __%1__\n").arg(func) + res;
	std::cout << func.toAscii().data() << " loaded : \n" << res.toAscii().data() << "\n";
	return res;
}

void SplatRendererPlugin::Init(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla)
{
	gla->makeCurrent();
	// FIXME this should be done in meshlab !!!
	glewInit();

	mShaders[0].SetSources(loadSource("VisibilityVP","Raycasting.glsl").toAscii().data(),
												 loadSource("VisibilityFP","Raycasting.glsl").toAscii().data());
	mShaders[1].SetSources(loadSource("AttributeVP","Raycasting.glsl").toAscii().data(),
												 loadSource("AttributeFP","Raycasting.glsl").toAscii().data());
// 	mShaders[2].SetSources(loadSource("FinalizationVP","Raycasting.glsl").toAscii().data(),
// 												 loadSource("FinalizationFP","Raycasting.glsl").toAscii().data());

	// create a floating point FBO
	//QGLFramebufferObject
	
	mCurrentPass = 2;
	mBindedPass = -1;
}

void SplatRendererPlugin::Render(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget * /* gla */)
{
	mCurrentPass = (mCurrentPass++) % 3;
}

void SplatRendererPlugin::Draw(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget * gla)
{
	if (mCurrentPass==2)
		return;

	if (m.cm.vert.RadiusEnabled)
	{
		//enablePass(mCurrentPass);
		drawSplats(m, rm);
	}
	else
	{
		mBindedPass = -1;
		MeshRenderInterface::Draw(a, m, rm, gla);
	}
}

void SplatRendererPlugin::enablePass(int n)
{
	if (mBindedPass!=n)
	{
		mShaders[n].Bind();
		mBindedPass = n;

		// set GL states
		if (n==0)
		{
		}
		if (n==1)
		{
		}
		if (n==2)
		{
		}
	}
}

void SplatRendererPlugin::drawSplats(MeshModel &m, RenderMode &rm)
{
	if(m.cm.vn!=(int)m.cm.vert.size())
	{
		int cm = rm.colorMode;

		if( (cm == GLW::CMPerFace)  && (!tri::HasPerFaceColor(m.cm)) ) cm=GLW::CMNone;

		glPushMatrix();
		glMultMatrix(m.cm.Tr);

		// manual rendering
		CMeshO::VertexIterator vi;
		glBegin(GL_POINTS);
			if(cm==GLW::CMPerMesh)
				glColor(m.cm.C());

			for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
				if(!(*vi).IsD())
				{
					glMultiTexCoord1f(GL_TEXTURE2, (*vi).cR());
					glNormal((*vi).cN());
					if (cm==GLW::CMPerVert) glColor((*vi).C());
					glVertex((*vi).P());
				}
		glEnd();

		glPopMatrix();

		return;
	}
	
	// bind the radius
	glClientActiveTexture(GL_TEXTURE2);
	glTexCoordPointer(
		1,
		GL_FLOAT,
		size_t(m.cm.vert[1].cR())-size_t(m.cm.vert[0].cR()),
		&m.cm.vert[0].cR()
	);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);

	// draw
	m.Render(rm.drawMode,rm.colorMode,rm.textureMode);

	glClientActiveTexture(GL_TEXTURE2);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
}
	
Q_EXPORT_PLUGIN(SplatRendererPlugin)
