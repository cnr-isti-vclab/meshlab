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

SplatRendererPlugin::SplatRendererPlugin()
{
	mNormalTextureID = 0;
	mDepthTextureID = 0;
	mIsSupported = false;
	mRenderBuffer = 0;
	mWorkaroundATI = false;
	mBuggedAtiBlending = false;
	mDummyTexId = 0;

	mFlags = DEFERRED_SHADING_BIT | DEPTH_CORRECTION_BIT | FLOAT_BUFFER_BIT | OUTPUT_DEPTH_BIT;
	mCachedFlags = ~mFlags;
	// union of bits which controls the render buffer
	mRenderBufferMask = DEFERRED_SHADING_BIT | FLOAT_BUFFER_BIT;
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
	else qDebug("Succesfully loaded shader func '%s' in file '%s'",qPrintable(func),qPrintable(filename));
	QTextStream stream(&f);
	res = stream.readAll();
	f.close();
	res = QString("#define __%1__ 1\n").arg(func)
			+ QString("#define %1 main\n").arg(func)
			+ res;
	return res;
}

void SplatRendererPlugin::configureShaders()
{
	const char* passNames[3] = {"Visibility","Attribute","Finalization"};
	QString defines = "";
	if (mFlags & DEFERRED_SHADING_BIT)
		defines += "#define EXPE_DEFERRED_SHADING\n";
	if (mFlags & DEPTH_CORRECTION_BIT)
		defines += "#define EXPE_DEPTH_CORRECTION\n";
	if (mFlags & OUTPUT_DEPTH_BIT)
		defines += "#define EXPE_OUTPUT_DEPTH 1\n";
	if (mFlags & BACKFACE_SHADING_BIT)
		defines += "#define EXPE_BACKFACE_SHADING\n";
	if (mWorkaroundATI)
		defines += "#define EXPE_ATI_WORKAROUND\n";

	QString shading =
"vec4 meshlabLighting(vec4 color, vec3 eyePos, vec3 normal)"
"{"
"	normal = normalize(normal);"
"	vec3 lightVec = normalize(gl_LightSource[0].position.xyz);"
"	vec3 halfVec = normalize( lightVec - normalize(eyePos) );"
"	float aux_dot = dot(normal,lightVec);"
"	float diffuseCoeff = clamp(aux_dot, 0.0, 1.0);"
" float specularCoeff = aux_dot>0.0 ? clamp(pow(clamp(dot(halfVec, normal),0.0,1.0),gl_FrontMaterial.shininess), 0.0, 1.0) : 0.0;"
"	return vec4(color.rgb * ( gl_FrontLightProduct[0].ambient.rgb + diffuseCoeff * gl_FrontLightProduct[0].diffuse.rgb) + specularCoeff * gl_FrontLightProduct[0].specular.rgb, 1.0);"
"}\n";

	for (int k=0;k<3;++k)
	{
		QString vsrc = shading + defines + mShaderSrcs[k*2+0];
		QString fsrc = shading + defines + mShaderSrcs[k*2+1];
		mShaders[k].SetSources(mShaderSrcs[k*2+0]!="" ? vsrc.toAscii().data() : 0,
													 mShaderSrcs[k*2+1]!="" ? fsrc.toAscii().data() : 0);
		mShaders[k].prog.Link();
		if (mShaderSrcs[k*2+0]!="")
		{
			std::string compileinfo = mShaders[k].vshd.InfoLog();
			if (compileinfo.size()>0)
				std::cout << "Vertex shader info (" << passNames[k] << ":\n" << compileinfo << "\n";
		}
		if (mShaderSrcs[k*2+1]!="")
		{
			std::string compileinfo = mShaders[k].fshd.InfoLog();
			if (compileinfo.size()>0)
				std::cout << "Fragment shader info (" << passNames[k] << ":\n" << compileinfo << "\n";
		}
		std::string linkinfo = mShaders[k].prog.InfoLog();
		if (linkinfo.size()>0)
			std::cout << "Link info (" << passNames[k] << ":\n" << linkinfo << "\n";
	}
}

void SplatRendererPlugin::Init(QAction * a, MeshDocument & md, RenderMode &, QGLWidget *gla)
{
	if (!(md.mm()->hasDataMask(MeshModel::MM_VERTRADIUS)))
	{
		QMessageBox::warning(0, tr("Splat Render Failure"), QString("Warning the current mesh <font color=red>'" + md.mm()->fullName() + "'</font> cannot not be rendered.<br><i>Radius per vertex attribute</i> must be present."));
		return;
	}
	mIsSupported = true;
	gla->makeCurrent();
	// FIXME this should be done in meshlab !!! ??
	glewInit();

	const char* rs = (const char*)glGetString(GL_RENDERER);
	QString rendererString("");
	if(rs)
		rendererString = QString(rs);
	mWorkaroundATI = rendererString.startsWith("ATI") || rendererString.startsWith("AMD");
	// FIXME: maybe some recent HW correctly supports floating point blending...
	mBuggedAtiBlending = rendererString.startsWith("ATI") || rendererString.startsWith("AMD");

	if (mWorkaroundATI && mDummyTexId==0)
	{
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1,&mDummyTexId);
		glBindTexture(GL_TEXTURE_2D, mDummyTexId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 4, 4, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);
	}

	// let's check the GPU capabilities
	mSupportedMask = DEPTH_CORRECTION_BIT | BACKFACE_SHADING_BIT;
	if (!QGLFramebufferObject::hasOpenGLFramebufferObjects ())
	{
		mIsSupported = false;
		return;
	}
	if (GLEW_ARB_texture_float)
		mSupportedMask |= FLOAT_BUFFER_BIT;
	else
		std::cout << "Splatting: warning floating point textures are not supported.\n";

	if (GLEW_ARB_draw_buffers && (!mBuggedAtiBlending))
		mSupportedMask |= DEFERRED_SHADING_BIT;
	else
		std::cout << "Splatting: warning deferred shading is not supported.\n";

	if (GLEW_ARB_shadow)
		mSupportedMask |= OUTPUT_DEPTH_BIT;
	else
		std::cerr << "Splatting: warning copy of the depth buffer is not supported.\n";

	mFlags = mFlags & mSupportedMask;

	// load shader source
	mShaderSrcs[0] = loadSource("VisibilityVP","Raycasting.glsl");
	mShaderSrcs[1] = loadSource("VisibilityFP","Raycasting.glsl");
	mShaderSrcs[2] = loadSource("AttributeVP","Raycasting.glsl");
	mShaderSrcs[3] = loadSource("AttributeFP","Raycasting.glsl");
	mShaderSrcs[4] = "";
	mShaderSrcs[5] = loadSource("Finalization","Finalization.glsl");

	//mCurrentPass = 2;
	mBindedPass = -1;
	GL_TEST_ERR
}

void SplatRendererPlugin::updateRenderBuffer()
{
	if ( (!mRenderBuffer)
		|| (mRenderBuffer->width()!=mCachedVP[2])
		|| (mRenderBuffer->height()!=mCachedVP[3])
		|| ( (mCachedFlags & mRenderBufferMask) != (mFlags & mRenderBufferMask) ))
	{
		delete mRenderBuffer;
		GLenum fmt = (mFlags&FLOAT_BUFFER_BIT) ? GL_RGBA16F_ARB : GL_RGBA;
		mRenderBuffer = new QGLFramebufferObject(mCachedVP[2], mCachedVP[3],
				(mFlags&OUTPUT_DEPTH_BIT) ? QGLFramebufferObject::NoAttachment : QGLFramebufferObject::Depth,
				GL_TEXTURE_RECTANGLE_ARB, fmt);

		if (!mRenderBuffer->isValid())
		{
			std::cout << "SplatRenderer: invalid FBO\n";
		}

		GL_TEST_ERR
		if (mFlags&DEFERRED_SHADING_BIT)
		{
			// in deferred shading mode we need an additional buffer to accumulate the normals
			if (mNormalTextureID==0)
				glGenTextures(1,&mNormalTextureID);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mNormalTextureID);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, fmt, mCachedVP[2], mCachedVP[3], 0, GL_RGBA, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			mRenderBuffer->bind();
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_RECTANGLE_ARB, mNormalTextureID, 0);
			mRenderBuffer->release();
			GL_TEST_ERR
		}

		if (mFlags&OUTPUT_DEPTH_BIT)
		{
			// to output the depth values to the final depth buffer we need to
			// attach a depth buffer as a texture
			if (mDepthTextureID==0)
				glGenTextures(1,&mDepthTextureID);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mDepthTextureID);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT24_ARB, mCachedVP[2], mCachedVP[3], 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			mRenderBuffer->bind();
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_RECTANGLE_ARB, mDepthTextureID, 0);
			mRenderBuffer->release();
			GL_TEST_ERR
		}
	}
}

void SplatRendererPlugin::Render(QAction *, MeshDocument &md, RenderMode &rm, QGLWidget * /* gla */)
{
	GL_TEST_ERR

		/*************** First Pass ***********/
		// this is the first pass of the frame, so let's update the shaders, buffers, etc...
		glGetIntegerv(GL_VIEWPORT, mCachedVP);
		glGetFloatv(GL_MODELVIEW_MATRIX, mCachedMV);
    glGetFloatv(GL_PROJECTION_MATRIX, mCachedProj);

		updateRenderBuffer();
		if (mCachedFlags != mFlags)
			configureShaders();

		mCachedFlags = mFlags;

		mParams.update(mCachedMV, mCachedProj, mCachedVP);
		float s = md.mm()->glw.GetHintParamf(GLW::HNPPointSize);
		if (s>1)
			s = pow(s,0.3f);
		mParams.radiusScale *= s;

		// FIXME since meshlab does not set any material properties, let's define some here
		glDisable(GL_COLOR_MATERIAL);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, vcg::Point4f(0.3, 0.3, 0.3, 1.).V());
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, vcg::Point4f(0.6, 0.6, 0.6, 1.).V());
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, vcg::Point4f(0.5, 0.5, 0.5, 1.).V());

		mRenderBuffer->bind();
		if (mFlags&DEFERRED_SHADING_BIT)
		{
			GLenum buf[2] = {GL_COLOR_ATTACHMENT0_EXT,GL_COLOR_ATTACHMENT1_EXT};
			glDrawBuffersARB(2, buf);
		}
		glViewport(mCachedVP[0],mCachedVP[1],mCachedVP[2],mCachedVP[3]);
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		//* End Setup of first Pass Now a simple rendering of all the involved meshes.*/ 
		mParams.loadTo(mShaders[0].prog);
		enablePass(0);
		foreach(MeshModel * mp, md.meshList)
			{
				if(mp->visible)  drawSplats (*mp,rm);
			}

		// begin second pass 
		mParams.loadTo(mShaders[1].prog);
		enablePass(1);

		foreach(MeshModel * mp, md.meshList)
			{
				if(mp->visible)  drawSplats (*mp,rm);
			}
		
		//* Start third Pass Setup */ 

		// this is the last pass: normalization by the sum of weights + deferred shading
		mRenderBuffer->release();
		if (mFlags&DEFERRED_SHADING_BIT)
			glDrawBuffer(GL_BACK);

		enablePass(2);

		// switch to normalized 2D rendering mode
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		mShaders[2].prog.Uniform("viewport",float(mCachedVP[0]),float(mCachedVP[1]),float(mCachedVP[2]),float(mCachedVP[3]));
		mShaders[2].prog.Uniform("ColorWeight",GLint(0)); // this is a texture unit
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mRenderBuffer->texture());

		if (mFlags&DEFERRED_SHADING_BIT)
		{
			mShaders[2].prog.Uniform("unproj", mCachedProj[10], mCachedProj[14]);
			mShaders[2].prog.Uniform("NormalWeight",GLint(1)); // this is a texture unit
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mNormalTextureID);
			GL_TEST_ERR
		}

		if (mFlags&OUTPUT_DEPTH_BIT)
		{
			mShaders[2].prog.Uniform("Depth",GLint(2)); // this is a texture unit
			glActiveTexture(GL_TEXTURE2);GL_TEST_ERR
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mDepthTextureID);GL_TEST_ERR
			GL_TEST_ERR
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
		}

		// draw a quad covering the whole screen
    vcg::Point3f viewVec(1./mCachedProj[0], 1./mCachedProj[5], -1);

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
    if (!(mFlags&OUTPUT_DEPTH_BIT))
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }

		glUseProgram(0);

		// restore matrices
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		
	  GL_TEST_ERR

}
#if 0
void SplatRendererPlugin::Draw(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget * gla)
{
	if (m.cm.vert.RadiusEnabled)
	{
		if (mCurrentPass==2)
			return;

		enablePass(mCurrentPass);
		/*if (mCurrentPass==1)*/ drawSplats(m, rm);
	}
	else if (mCurrentPass==2)
	{
		MeshRenderInterface::Draw(a, m, rm, gla);
	}
}
#endif
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
			glDisable(GL_LIGHTING);
// 			glDisable(GL_POINT_SMOOTH);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

			glAlphaFunc(GL_LESS,1);
			glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			glEnable(GL_DEPTH_TEST);

// 			glActiveTexture(GL_TEXTURE0);
// 			glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
// 			glEnable(GL_POINT_SPRITE_ARB);
		}
		if (n==1)
		{
			glDisable(GL_LIGHTING);
			glEnable(GL_POINT_SMOOTH);
			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

			glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE,GL_ONE);
// 			//glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE,GL_ZERO);
// 			glBlendFunc(GL_ONE,GL_ZERO);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_ALPHA_TEST);

// 			glActiveTexture(GL_TEXTURE0);

		}
		if ( (n==0) || (n==1) )
		{
			// enable point sprite rendering mode
			glActiveTexture(GL_TEXTURE0);
			if (mWorkaroundATI)
			{
				glBindTexture(GL_TEXTURE_2D, mDummyTexId);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 2, 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);
				glPointParameterf(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
				// hm... ^^^^
			}
			glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
			glEnable(GL_POINT_SPRITE_ARB);
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
		// manual rendering
		int cm = rm.colorMode;
		if( (cm == GLW::CMPerFace)  && (!tri::HasPerFaceColor(m.cm)) )
			cm=GLW::CMNone;
		glPushMatrix();
		glMultMatrix(m.cm.Tr);
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

	// draw the vertices
	m.Render(vcg::GLW::DMPoints,rm.colorMode,rm.textureMode);

	glClientActiveTexture(GL_TEXTURE2);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
}


void SplatRendererPlugin::UniformParameters::update(float* mv, float* proj, GLint* vp)
{
	// extract the uniform scale
	float scale = vcg::Point3f(mv[0],mv[1],mv[2]).Norm();

	radiusScale = scale;
	preComputeRadius = - std::max(proj[0]*vp[2], proj[5]*vp[3]);
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
