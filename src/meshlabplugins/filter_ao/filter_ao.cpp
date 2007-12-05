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



#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/space/box3.h>
#include <vcg/math/gen_normal.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include <wrap/gl/trimesh.h>

#include <QtGui>
#include <QGLContext>
#include <QGLFormat>
#include <QTextStream>
#include <QDataStream>
#include <QString>
#include <QFile>
#include <QTime>
#include <gl/glew.h>

#include "filter_ao.h"

#include <iostream>

#define AMBOCC_DEFAULT_TEXTURE_SIZE 1024
#define AMBOCC_DEFAULT_NUM_VIEWS 250
#define AMBOCC_USEGPU_BY_DEFAULT false

static GLuint vs, fs, shdrID, fxdShdrID, meshDL;

AmbientOcclusionPlugin::AmbientOcclusionPlugin() 
{ 
	typeList << FP_AMBIENT_OCCLUSION;
	
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);

	useGPU = AMBOCC_USEGPU_BY_DEFAULT;
	numViews = AMBOCC_DEFAULT_NUM_VIEWS;
	texSize = AMBOCC_DEFAULT_TEXTURE_SIZE;
	texArea = texSize*texSize;
}

AmbientOcclusionPlugin::~AmbientOcclusionPlugin()
{
}

const QString AmbientOcclusionPlugin::filterName(FilterIDType filterId) 
{
	switch(filterId)
	{
		case FP_AMBIENT_OCCLUSION :  return QString("Ambient Occlusion"); 
		default : assert(0); 
	}
	
	return QString("");
}

const QString AmbientOcclusionPlugin::filterInfo(FilterIDType filterId)
{
	switch(filterId)
	{
		case FP_AMBIENT_OCCLUSION :  return QString("Generates environment occlusions values for the loaded mesh"); 
		default : assert(0); 
	}

	return QString("");
}

const PluginInfo &AmbientOcclusionPlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("0.1");
	 ai.Author = ("Michele Mischitelli, Massimiliano Corsini");
   return ai;
 }

const int AmbientOcclusionPlugin::getRequirements(QAction *action)
{
	//no requirements needed
	return 0;
}

void AmbientOcclusionPlugin::initParameterSet(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
	switch(ID(action))
	{
		case FP_AMBIENT_OCCLUSION:
			parlst.addBool("gpuAcceleration",AMBOCC_USEGPU_BY_DEFAULT,"Use GPU acceleration","In order to use GPU-Mode, your hardware must support FBOs, FP32 Textures and Shaders. Normally increases the performance by a factor of 4x-5x");
			parlst.addInt ("texSize",AMBOCC_DEFAULT_TEXTURE_SIZE,"Depth texture size(should be 2^n)", "Defines the depth texture size used to compute occlusion from each point of view. Higher values means better accuracy usually with low impact on performance");
			parlst.addInt ("reqViews",AMBOCC_DEFAULT_NUM_VIEWS,"Requested views", "Number of different views uniformly placed around the mesh. More views means better accuracy at the cost of increased calculation time");
			break;
		default: assert(0);
	}
}
bool AmbientOcclusionPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	int tInitElapsed = 0;
	QTime tInit, tAll;
	tInit.start();
	tAll.start();

	assert(filter->text() == filterName(FP_AMBIENT_OCCLUSION));
	useGPU = par.getBool("gpuAcceleration");
	texSize = par.getInt("texSize");
	texArea = texSize*texSize;
	numViews = par.getInt("reqViews");

	if ( useGPU && ((unsigned int)m.cm.vn > texArea) )
	{
		Log(0, "Too many vertices: up to %d are allowed", texArea);
		return false;
	}

	GLfloat *occlusion = new GLfloat[m.cm.vn];
	typedef std::vector<vcg::Point3f> vectP3f;
	vectP3f::iterator vi;
	static vectP3f posVect;
	QGLWidget qWidget;

	//Creates a new RC, initializes everything (glew, textures, FBO..)
	if (!initContext(qWidget, cb))
		return false;
	
	//Prepare mesh to be rendered
	vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m.cm);
	if (GLEW_ARB_vertex_buffer_object)
	{
		m.glw.SetHint(vcg::GLW::HNUseVBO);
		cb(0, "Generating vertex data for VBO...");
	}
	m.glw.Update();

	if (GLEW_ARB_vertex_buffer_object)
		cb(100, "Generating vertex data for VBO... Done.");

	if(useGPU)
		vertexCoordsToTexture( m );
	else
	{
		//Yes, the loop down there is needed
		for (int j=0; j<m.cm.vn; ++j)
			occlusion[j] = 0.0f;
	}

	//Generates the views to be used just the first time
	//(to keep view directions always the same)
	//or if the user changed the number of requested views
	static bool first = true;
	static int lastViews = numViews;
	if (first || (lastViews!=numViews))
	{
		first = false;
		lastViews = numViews;
		GenNormal<float>::Uniform(numViews,posVect);
	}

	numViews = posVect.size();

	glClearColor(0.0, 0.0, 0.0, 0.0);

	tInitElapsed = tInit.elapsed();
	
	int c=0;
	for (vi = posVect.begin(); vi != posVect.end(); vi++)
	{
		setCamera(*vi, m.cm.bbox);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);

		if (useGPU)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);

			// FIRST PASS - fill depth buffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboDepth);
			glBindTexture(GL_TEXTURE_2D, depthBufferTex);
			glClear(GL_DEPTH_BUFFER_BIT);

			glColorMask(0, 0, 0, 0);
				renderMesh(m);
			glColorMask(1, 1, 1, 1);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboResult);

			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);  //final.rgba = min(2^31, src.rgba*1 + dest.rgba*1);

			// SECOND PASS - use depth buffer to check occlusion
			generateOcclusionHW();
			
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
		else
		{
			glClear(GL_DEPTH_BUFFER_BIT);

			// FIRST PASS - fill depth buffer
			glColorMask(0, 0, 0, 0);
				renderMesh(m);
			glColorMask(1, 1, 1, 1);

			glDisable(GL_POLYGON_OFFSET_FILL);

			// SECOND PASS - use depth buffer to check occlusion
			generateOcclusionSW(m, occlusion);
		}

		cb( 100*c/posVect.size() , "Calculating Ambient Occlusion...");
		c++;
	}

	if (useGPU)
	{
		applyOcclusionHW(m);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	}
	else
		applyOcclusionSW(m,occlusion);

	Log(0,"Successfully calculated A.O. after %3.2f sec, %3.2f of which is due to initialization", ((float)tAll.elapsed()/1000.0f), ((float)tInitElapsed/1000.0f) );


	/********** Clean up the mess ************/
	if (useGPU)
	{
		glDisable(GL_BLEND);

		glDetachObjectARB(fboDepth, depthBufferTex);
		glDetachObjectARB(fboResult, resultBufferTex);
		glDeleteFramebuffersEXT(1, &fboDepth);
		glDeleteFramebuffersEXT(1, &fboResult);

		glDetachShader(shdrID, vs);
		glDetachShader(shdrID, fs);
		glDeleteShader(shdrID);
	}

	m.glw.ClearHint(vcg::GLW::HNUseVBO);

	delete [] occlusion;

	qWidget.doneCurrent();

	return true;
}
void AmbientOcclusionPlugin::renderMesh(MeshModel &m)
{
	m.glw.DrawFill<vcg::GLW::NMNone, vcg::GLW::CMNone, vcg::GLW::TMNone>();
}

void AmbientOcclusionPlugin::initTextures(GLenum colorFormat, GLenum depthFormat)
{
	//*******SETS DEFAULT OPENGL STUFF**********/
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );

	vertexCoordTex = 0;
	vertexNormalsTex = 0;
	resultBufferTex= 0;

	if (useGPU)
	{
		//*******INIT VERTEX COORDINATES TEXTURE*********/
		glGenTextures (1, &vertexCoordTex);
		glBindTexture(GL_TEXTURE_2D, vertexCoordTex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, 
					 texSize, texSize, 0, GL_RGBA, GL_FLOAT, 0);

		//*******INIT NORMAL VECTORS TEXTURE*********/
		glGenTextures (1, &vertexNormalsTex);
		glBindTexture(GL_TEXTURE_2D, vertexNormalsTex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, 
					 texSize, texSize, 0, GL_RGBA, GL_FLOAT, 0);
	}

	//*******INIT RESULT TEXTURE*********/
	glGenTextures (1, &resultBufferTex);
	glBindTexture(GL_TEXTURE_2D, resultBufferTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, 
				 texSize, texSize, 0, GL_RGBA, GL_FLOAT, 0);

	//*******INIT DEPTH TEXTURE*********/
	glGenTextures(1, &depthBufferTex);
	glBindTexture(GL_TEXTURE_2D, depthBufferTex);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,           GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,           GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,       GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,       GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB,   GL_LUMINANCE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

	glTexImage2D (GL_TEXTURE_2D, 0, depthFormat,
	              texSize, texSize, 0, 
				  GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

bool AmbientOcclusionPlugin::initContext(QGLWidget &qWidget, vcg::CallBackPos *cb)
{
	cb(0, "Initializing: Context");

	QGLFormat qFormat = QGLFormat::defaultFormat();
	qFormat.setAlpha(true);
	qFormat.setDepth(true);
	qWidget.setFormat(qFormat);

	if(!qWidget.isValid())
	{
		Log(0,"Error: Unable to create a new QGLWidget");
		return false;
	}

	qWidget.setFixedSize(texSize,texSize);
	qWidget.makeCurrent();

	cb(15, "Initializing: Glew");

	//*******INIT GLEW********/
	GLint glewError = glewInit();
	if (glewError)
	{
		Log(0,(const char*)glewGetErrorString(glewError));
		return false;
	}

	//*******CHECK TEX SIZE********/
	if (texSize < 15)
	{
		Log(0, "Texture size is too small, 16x16 used instead");
		texSize = 16;
		texArea = texSize*texSize;
	}
	if (texSize > 1024)
	{
		Log(0, "Texture size is too large, 1024x1024 used instead");
		texSize = 1024;
		texArea = texSize*texSize;
	}

	//*******CHECK THAT EVERYTHING IS SUPPORTED**********/
	if (useGPU)
	{
		if ( !GLEW_ARB_vertex_shader || !GLEW_ARB_fragment_shader )
		{
			Log(0, "Your hardware doesn't support Shaders, which are required for hw occlusion");
			return false;
		}
		if ( !GLEW_EXT_framebuffer_object )
		{
			Log(0, "Your hardware doesn't support FBOs, which are required for hw occlusion");
			return false;
		}
	}

	if (!GLEW_ARB_texture_float)
	{
		Log(0,"Your hardware doesn't support FP16/32 textures, which are required for hw occlusion");
		return false;
	}

	cb(30, "Initializing: Textures");
	//GL_RGBA32/16F_ARB works on nv40+(GeForce6 or newer) and ATI hardware
	initTextures(GL_RGBA32F_ARB, GL_DEPTH_COMPONENT24);
	
	if (useGPU)
	{
		//*******LOAD SHADER*******/
		cb(45, "Initializing: Shaders");
		set_shaders("ambient_occlusion",vs,fs,shdrID);


		//*******INIT FBO*********/

		cb(65, "Initializing: FBOs");
		// FBO for first pass (1 depth attachment)
		fboDepth = 0;
		glGenFramebuffersEXT(1, &fboDepth);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboDepth);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBufferTex, 0);

		// only in this way it is possible to read back the depth texture correctly(!!)
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (!checkFramebuffer())
			return false;
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		// FBO for second pass (1 color attachment)
		fboResult = 0;
		glGenFramebuffersEXT(1, &fboResult);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboResult);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, resultBufferTex, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		
		if (!checkFramebuffer())
			return false;
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	glViewport(0.0, 0.0, texSize, texSize);

	cb(100, "Initializing: Done.");

	return true;
}

bool AmbientOcclusionPlugin::checkFramebuffer()
{
	GLenum fboStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	if ( fboStatus != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		switch (fboStatus)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			Log(0, "FBO Incomplete: Attachment");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			Log(0, "FBO Incomplete: Missing Attachment");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			Log(0, "FBO Incomplete: Dimensions");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			Log(0, "FBO Incomplete: Formats");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			Log(0, "FBO Incomplete: Draw Buffer");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			Log(0, "FBO Incomplete: Read Buffer");
			break;
		default:
			Log(0, "Undefined FBO error");
		}

		return false;
	}

	return true;
}

void AmbientOcclusionPlugin::vertexCoordsToTexture(MeshModel &m)
{
	GLfloat *vertexPosition= new GLfloat[texArea*4];
	GLfloat *vertexNormals = new GLfloat[texArea*4];

	//Copies each vertex's position and normal in new vectors
	for (int i=0; i < m.cm.vn; ++i)
	{
		//Vertex position
		vertexPosition[i*4+0] = m.cm.vert[i].P().X();
		vertexPosition[i*4+1] = m.cm.vert[i].P().Y();
		vertexPosition[i*4+2] = m.cm.vert[i].P().Z();
		vertexPosition[i*4+3] = 1.0;

		//Normal vector for each vertex
		vcg::Point3<CMeshO::ScalarType> n = m.cm.vert[i].N();
		vertexNormals[i*4+0] = n.X();
		vertexNormals[i*4+1] = n.Y();
		vertexNormals[i*4+2] = n.Z();
		vertexNormals[i*4+3] = 1.0;
	}

	//The aforementioned vectors are used to encode a texture that stores each vertex's position & normal
	//Those texture are then used to perform a GPU occlusion test with each view's depth buffer
	
	//Write vertex coordinates
	glBindTexture(GL_TEXTURE_2D, vertexCoordTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texSize, texSize, GL_RGBA, GL_FLOAT, vertexPosition);	
	delete [] vertexPosition;

	//Write normal directions
	glBindTexture(GL_TEXTURE_2D, vertexNormalsTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texSize, texSize, GL_RGBA, GL_FLOAT, vertexNormals);
	delete [] vertexNormals;
}

void AmbientOcclusionPlugin::setCamera(Point3f camDir, Box3f &meshBBox)
{
	cameraDir = camDir;
	GLfloat d = (meshBBox.Diag()/2.0) * 1.1,
	        k = 0.1f;
	Point3f eye = meshBBox.Center() + camDir * (d+k);

	glViewport(0.0, 0.0, texSize, texSize);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-d, d, -d, d, k, k+(2.0*d) );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.X(), eye.Y(), eye.Z(),
			  meshBBox.Center().X(), meshBBox.Center().Y(), meshBBox.Center().Z(),
			  0.0, 1.0, 0.0);
}


void AmbientOcclusionPlugin::generateOcclusionHW()
{
	GLfloat mv_pr_Matrix_f[16];  // modelview-projection matrix
	
	glGetFloatv(GL_MODELVIEW_MATRIX, mv_pr_Matrix_f);
	glMatrixMode(GL_PROJECTION);
	glMultMatrixf(mv_pr_Matrix_f);
	glGetFloatv(GL_PROJECTION_MATRIX, mv_pr_Matrix_f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Need to clear the depthBuffer if we don't
	// want a mesh-shaped hole in the middle of the S.A.Q. :)
	glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);


	glUseProgram(shdrID);

	// Set depthmap
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthBufferTex);
	glUniform1i(glGetUniformLocation(shdrID, "dTexture"), 0);

	// Set vertex position texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, vertexCoordTex);
	glUniform1i(glGetUniformLocation(shdrID, "vTexture"), 1);

	// Set vertex normal texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, vertexNormalsTex);
	glUniform1i(glGetUniformLocation(shdrID, "nTexture"), 2);

	// Set view direction
	glUniform3f(glGetUniformLocation(shdrID, "viewDirection"), cameraDir.X(), cameraDir.Y(), cameraDir.Z());

	// Set ModelView-Projection Matrix
	glUniformMatrix4fv(glGetUniformLocation(shdrID, "mvprMatrix"), 1, GL_FALSE, (const GLfloat*)mv_pr_Matrix_f);

	// Set texture Size
	glUniform1i(glGetUniformLocation(shdrID, "texSize"), texSize);

	// Screen-aligned Quad
	glBegin(GL_QUADS);
		glVertex3f(-1.0f, -1.0f, 0.0f); //L-L
		glVertex3f( 1.0f, -1.0f, 0.0f); //L-R
		glVertex3f( 1.0f,  1.0f, 0.0f); //U-R
		glVertex3f(-1.0f,  1.0f, 0.0f); //U-L
	glEnd();

	glUseProgram(0);
}

void AmbientOcclusionPlugin::generateOcclusionSW(MeshModel &m, GLfloat *occlusion)
{
	GLdouble *resCoords  = new GLdouble[3];
	GLdouble *mvMatrix_f = new GLdouble[16];
	GLdouble *prMatrix_f = new GLdouble[16];
	GLint    *viewpSize  = new GLint[4];
	GLfloat  *dFloat     = new GLfloat[texArea];

	glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix_f);
	glGetDoublev(GL_PROJECTION_MATRIX, prMatrix_f);
	glGetIntegerv(GL_VIEWPORT, viewpSize);

	glReadPixels(0, 0, texSize, texSize, GL_DEPTH_COMPONENT, GL_FLOAT, dFloat);

	cameraDir.Normalize();

	Point3<CMeshO::ScalarType> vp;
	Point3<CMeshO::ScalarType> vn;
	for (int i=0; i<m.cm.vn; ++i)
	{
		vp = m.cm.vert[i].P();
		gluProject(vp.X(), vp.Y(), vp.Z(),
				   (const GLdouble *) mvMatrix_f, (const GLdouble *) prMatrix_f, (const GLint *) viewpSize,
				   &resCoords[0], &resCoords[1], &resCoords[2] );

		int x = floor(resCoords[0]);
		int y = floor(resCoords[1]);
		
		if (resCoords[2] <= (GLdouble)dFloat[texSize*y+x])
		{
			vn = m.cm.vert[i].N();
			occlusion[i] += max(vn*cameraDir, 0.0f);
		}
	}

	delete [] mvMatrix_f;
	delete [] prMatrix_f;
	delete [] viewpSize;
	delete [] resCoords;
	delete [] dFloat;
}
void AmbientOcclusionPlugin::applyOcclusionHW(MeshModel &m)
{
	GLfloat *result = new GLfloat[texArea*4];
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadPixels(0, 0, texSize, texSize, GL_RGBA, GL_FLOAT, result);

	float maxvalue = 0.0f, minvalue = 100000.0f;
	for (int i=0; i < m.cm.vn; i++)
	{
		if (result[i*4] < minvalue)
			minvalue = result[i*4];

		if (result[i*4] > maxvalue)
			maxvalue = result[i*4];
	}

	float scale = 255.0f / (maxvalue - minvalue);

	for (int i = 0; i < m.cm.vn; i++)
	{
		m.cm.vert[i].Q() = result[i*4+0];
		m.cm.vert[i].C()[0] = (m.cm.vert[i].Q() - minvalue) * scale;
		m.cm.vert[i].C()[1] = (m.cm.vert[i].Q() - minvalue) * scale;
		m.cm.vert[i].C()[2] = (m.cm.vert[i].Q() - minvalue) * scale;
	}

	delete [] result;
}

void AmbientOcclusionPlugin::applyOcclusionSW(MeshModel &m, GLfloat *aoValues)
{
	float maxvalue = 0.0f, minvalue = 100000.0f;
	for (int i=0; i < m.cm.vn; i++)
	{
		if (aoValues[i] < minvalue)
			minvalue = aoValues[i];

		if (aoValues[i] > maxvalue)
			maxvalue = aoValues[i];
	}

	float scale = 255.0f / (maxvalue - minvalue);

	for (int i = 0; i < m.cm.vn; i++)
	{
		m.cm.vert[i].Q() = aoValues[i];
		m.cm.vert[i].C()[0] = (m.cm.vert[i].Q() - minvalue) * scale;
		m.cm.vert[i].C()[1] = (m.cm.vert[i].Q() - minvalue) * scale;
		m.cm.vert[i].C()[2] = (m.cm.vert[i].Q() - minvalue) * scale;
	}
}

void AmbientOcclusionPlugin::set_shaders(char *shaderName, GLuint &v, GLuint &f, GLuint &pr)
{	
	QDir shadersDir = QDir(qApp->applicationDirPath());


#if defined(Q_OS_WIN)
	if (shadersDir.dirName() == "debug" || shadersDir.dirName() == "release" || shadersDir.dirName() == "plugins"  )
		shadersDir.cdUp();
#elif defined(Q_OS_MAC)
	if (shadersDir.dirName() == "MacOS") {
		for(int i=0;i<4;++i)
		{
			if(shadersDir.exists("shaders"))
				break;
			shadersDir.cdUp();
		}
	}
#endif

	bool ret=shadersDir.cd("shaders");
	if(!ret) 
	{
		QMessageBox::information(0, "Ambient Occlusion Plugin","Unable to find the shaders directory.\nNo shaders will be loaded.");
		return;
	}

	f = glCreateShader(GL_FRAGMENT_SHADER);
	v = glCreateShader(GL_VERTEX_SHADER);

	QString fileName(shaderName);
	QByteArray ba;
	QFile file;
	char *data;

	fileName.append(".vert");
	file.setFileName(shadersDir.absoluteFilePath(fileName));
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream ts(&file);
		ba = ts.readAll().toLocal8Bit();
		data = ba.data();
		glShaderSource(v, 1, (const GLchar**)&data,NULL);
		glCompileShader(v);
		GLint errV;
		glGetShaderiv(v,GL_COMPILE_STATUS,&errV);
		assert(errV==GL_TRUE);
		file.close();
	}

	fileName.replace(QString(".vert"), QString(".frag"));
	file.setFileName(shadersDir.absoluteFilePath(fileName));
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream ts(&file);
		ba = ts.readAll().toLocal8Bit();
		data = ba.data();
		glShaderSource(f, 1, (const GLchar**)&data,NULL);
		glCompileShader(f);
		GLint errF;
		glGetShaderiv(f,GL_COMPILE_STATUS,&errF);
		assert(errF==GL_TRUE);
		file.close();
	}

	pr = glCreateProgram();

	glAttachShader(pr,v);
	glAttachShader(pr,f);

	glLinkProgram(pr);
}
void AmbientOcclusionPlugin::dumpFloatTexture(QString filename, float *texdata, int elems)
{
	QFile f(filename);
	f.open(QFile::WriteOnly);
	f.write(reinterpret_cast<const char *>(texdata), elems * sizeof(float));
	f.close();
}

Q_EXPORT_PLUGIN(AmbientOcclusionPlugin)
