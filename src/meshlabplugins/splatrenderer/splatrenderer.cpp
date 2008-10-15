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
#include <iostream>
#include "splatrenderer.h"
#include <QGLWidget>
#include <QTextStream>
#include <wrap/gl/trimesh.h>
#include <QGLFramebufferObject>

using namespace std;
using namespace vcg;

#define GL_TEST_ERR\
        {\
            GLenum eCode;\
            if((eCode=glGetError())!=GL_NO_ERROR)\
                std::cerr << "OpenGL error : " <<  gluErrorString(eCode) << " in " <<  __FILE__ << " : " << __LINE__ << std::endl;\
        }
        
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
	res = QString("#define __%1__ 1\n").arg(func)
			+ QString("#define %1 main\n").arg(func)
			+ res;
// 	std::cout << func.toAscii().data() << " loaded : \n" << res.toAscii().data() << "\n";
	return res;
}

void SplatRendererPlugin::Init(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla)
{
	mIsSupported = true;
	gla->makeCurrent();
	// FIXME this should be done in meshlab !!!
	glewInit();

	mShaders[0].SetSources(loadSource("VisibilityVP","Raycasting.glsl").toAscii().data(),
												 loadSource("VisibilityFP","Raycasting.glsl").toAscii().data());
	mShaders[0].prog.Link();
	std::cout << "Linked visibility shader:\n"
// 		<< mShaders[0].vshd.InfoLog() << "\n"
// 		<< mShaders[0].fshd.InfoLog() << "\n"
		<< mShaders[0].prog.InfoLog() << "\n";

	mShaders[1].SetSources(loadSource("AttributeVP","Raycasting.glsl").toAscii().data(),
												 loadSource("AttributeFP","Raycasting.glsl").toAscii().data());
	mShaders[1].prog.Link();
	std::cout << "Linked attribute shader:\n" << mShaders[1].prog.InfoLog() << "\n";

	mShaders[2].SetSources(0,loadSource("Finalization","Finalization.glsl").toAscii().data());
	mShaders[2].prog.Link();
	std::cout << "Linked finalization shader:\n" << mShaders[2].prog.InfoLog() << "\n";

	// create a floating point FBO
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	if ( (!mRenderBuffer)
		|| (mRenderBuffer->width()!=vp[2])
		|| (mRenderBuffer->height()!=vp[3]))
	{
		delete mRenderBuffer;
		mRenderBuffer = new QGLFramebufferObject(vp[2], vp[3], QGLFramebufferObject::Depth, GL_TEXTURE_RECTANGLE_ARB, GL_RGBA16F_ARB);
		GL_TEST_ERR
		if (mDeferredShading)
		{
			// add a second floating point render target for the normals
			if (mNormalTextureID==0)
				glGenTextures(1,&mNormalTextureID);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mNormalTextureID);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16F_ARB, vp[2], vp[3], 0, GL_RGBA, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			mRenderBuffer->bind();
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_RECTANGLE_ARB, mNormalTextureID, 0);
			mRenderBuffer->release();
			GL_TEST_ERR
		}
	}

	mCurrentPass = 2;
	mBindedPass = -1;
}

void SplatRendererPlugin::Render(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget * /* gla */)
{
	mShaders[mCurrentPass].prog.Unbind();

	mCurrentPass = (mCurrentPass+1) % 3;

	if (mCurrentPass==0)
		mParams.update();

	if (mCurrentPass==2)
	{
		GL_TEST_ERR
		mRenderBuffer->release();
		if (mDeferredShading)
			glDrawBuffer(GL_BACK);
		//return;
		// finalization
		enablePass(mCurrentPass);
		float projmat[16];
		GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetFloatv(GL_PROJECTION_MATRIX,projmat);

		// switch to normalized 2D rendering mode
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		GL_TEST_ERR
		if (mDeferredShading)
		{
			mShaders[2].prog.Uniform("unproj", projmat[10], projmat[14]);
			mShaders[2].prog.Uniform("NormalWeight",1);
		}
		GL_TEST_ERR
		mShaders[2].prog.Uniform("viewport",float(viewport[0]),float(viewport[1]),float(viewport[2]),float(viewport[3]));
		GL_TEST_ERR
		mShaders[2].prog.Uniform("ColorWeight",0);
		
    // bind the FBO's textures
		// FIXME let's assume the unique texture is in GL_TEXTURE0
		GL_TEST_ERR
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mRenderBuffer->texture());
		if (mDeferredShading)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mNormalTextureID);
		}
// 		glBind
//     gpu.bindSampler(*(mSplattingBuffer->getSampler("ColorWeight")));


		// draw a quad covering the whole screen
    vcg::Point3f viewVec(1./projmat[0], 1./projmat[5], -1);
    if (!mOutputDepth)
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
		}
		GL_TEST_ERR
    glBegin(GL_QUADS);
			glColor3f(1, 0, 0);
			glTexCoord3f(viewVec.X(),viewVec.Y(),viewVec.Z());
			glMultiTexCoord2f(GL_TEXTURE1,1.,1.);
			glVertex3f(1,1,0);

			glColor3f(1, 1, 0);
			glTexCoord3f(-viewVec.X(),viewVec.Y(),viewVec.Z());
			glMultiTexCoord2f(GL_TEXTURE1,0.,1.);
			glVertex3f(-1,1,0);

			glColor3f(0, 1, 1);
			glTexCoord3f(-viewVec.X(),-viewVec.Y(),viewVec.Z());
			glMultiTexCoord2f(GL_TEXTURE1,0.,0.);
			glVertex3f(-1,-1,0);

			glColor3f(1, 0, 1);
			glTexCoord3f(viewVec.X(),-viewVec.Y(),viewVec.Z());
			glMultiTexCoord2f(GL_TEXTURE1,1.,0.);
			glVertex3f(1,-1,0);
    glEnd();
    GL_TEST_ERR
    if (!mOutputDepth)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }
    //gpu.unbindSampler(*(mSplattingBuffer->getSampler("ColorWeight")));
//     if (mDeferredShading)
//         gpu.unbindSampler(*(mSplattingBuffer->getSampler("NormalWeight")));
//     if ((mDeferredShading && mDepthInterpolationMode==DIM_None) || mOutputDepth)
//         gpu.unbindSampler(*(mSplattingBuffer->getSampler("Depth")));

		// restore matrices
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		GL_TEST_ERR
	}
	else
	{
		GL_TEST_ERR
		mParams.loadTo(mShaders[mCurrentPass].prog);
		if (mCurrentPass==0)
		{
			GLint vp[4];
    	glGetIntegerv(GL_VIEWPORT, vp);
			mRenderBuffer->bind();
			if (mDeferredShading)
			{
				GLenum buf[2] = {GL_COLOR_ATTACHMENT0_EXT,GL_COLOR_ATTACHMENT1_EXT};
				glDrawBuffersATI(2, buf);
			}
			glViewport(vp[0],vp[1],vp[2],vp[3]);
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		}
		GL_TEST_ERR
		enablePass(mCurrentPass);
		GL_TEST_ERR
	}
}

void SplatRendererPlugin::Draw(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget * gla)
{
	if (m.cm.vert.RadiusEnabled)
	{
		if (mCurrentPass==2)
			return;

	GL_TEST_ERR
		enablePass(mCurrentPass);
// 		if (mCurrentPass==0)
		GL_TEST_ERR
			drawSplats(m, rm);
			GL_TEST_ERR
	}
	else if (mCurrentPass==2)
	{
//     mOtherMesh.push_back(&m);
		// restore states
// 		mBindedPass = -1;
		MeshRenderInterface::Draw(a, m, rm, gla);
	}
}

void SplatRendererPlugin::enablePass(int n)
{
	if (mBindedPass!=n)
	{
		if (mBindedPass>=0)
			mShaders[mBindedPass].prog.Unbind();
		mShaders[n].prog.Bind();
		mBindedPass = n;

		// set GL states
		if (n==0)
		{
			glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
			glEnable(GL_POINT_SPRITE_ARB);
			glDisable(GL_POINT_SMOOTH);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			
			glAlphaFunc(GL_LESS,1);
			glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			glEnable(GL_DEPTH_TEST);
		}
		if (n==1)
		{
			glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
			glEnable(GL_POINT_SPRITE_ARB);
			glDisable(GL_POINT_SMOOTH);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			
			glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE,GL_ONE);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_ALPHA_TEST);
		}
		if (n==2)
		{
			glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
			glDepthMask(GL_TRUE);
			glDisable(GL_LIGHTING);
			glDisable(GL_BLEND);
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

void SplatRendererPlugin::UniformParameters::update()
{
	float mv[16];
	float proj[16];
	int vp[4];
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	glGetFloatv(GL_PROJECTION_MATRIX, proj);
	glGetIntegerv(GL_VIEWPORT, vp);

	// get the scale
	float scale = vcg::Point3f(mv[0],mv[1],mv[2]).Norm();
	glScalef(1./scale, 1./scale, 1./scale);
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	scale = vcg::Point3f(mv[0],mv[1],mv[2]).Norm();
// 	std::cout << scale << "\n";

	radiusScale = 1.0;
	preComputeRadius = - scale * std::max(proj[0]*vp[2], proj[5]*vp[3]);
	depthOffset = 2.0;
	oneOverEwaRadius = 0.70710678118654;
	halfVp = Point2f(0.5*vp[2], 0.5*vp[3]);
	rayCastParameter1 = Point3f(2./(proj[0]*vp[2]), 2./(proj[5]*vp[3]), 0.0);
	rayCastParameter2 = Point3f(-1./proj[0], -1./proj[5], -1.0);
	depthParameterCast = Point2f(0.5*proj[14], 0.5-0.5*proj[10]);
}

void SplatRendererPlugin::UniformParameters::loadTo(Program& prg)
{
	prg.Bind();
	prg.Uniform("expeRadiusScale",radiusScale);
	prg.Uniform("expePreComputeRadius",preComputeRadius);
	prg.Uniform("expeDepthOffset",depthOffset);
	prg.Uniform("oneOverEwaRadius",oneOverEwaRadius);
	prg.Uniform("halfVp",halfVp);
	prg.Uniform("rayCastParameter1",rayCastParameter1);
	prg.Uniform("rayCastParameter2",rayCastParameter2);
	prg.Uniform("depthParameterCast",depthParameterCast);
}

Q_EXPORT_PLUGIN(SplatRendererPlugin)
