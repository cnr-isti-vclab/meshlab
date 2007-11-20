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
#include <gl/glew.h>

#include "filter_ao.h"

#include <iostream>

#define AMBOCC_DEFAULT_TEXT_SIZE 1024
#define AMBOCC_DEFAULT_NUM_VIEWS 250
#define AMBOCC_USEGPU_BY_DEFAULT false
#define AMBOCC_HV 3.85f

static GLuint vs, fs, shdrID, meshDL;

AmbientOcclusionPlugin::AmbientOcclusionPlugin() 
{ 
	typeList << FP_AMBIENT_OCCLUSION;
	
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);

	useGPU = AMBOCC_USEGPU_BY_DEFAULT;
	numViews = AMBOCC_DEFAULT_NUM_VIEWS;
	textSize = AMBOCC_DEFAULT_TEXT_SIZE;
	textArea = textSize*textSize;
}

AmbientOcclusionPlugin::~AmbientOcclusionPlugin()
{
}

const QString AmbientOcclusionPlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_AMBIENT_OCCLUSION :  return QString("Ambient Occlusion"); 
		default : assert(0); 
	}
}

const QString AmbientOcclusionPlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_AMBIENT_OCCLUSION :  return QString("Generates environment occlusions values for the loaded mesh"); 
		default : assert(0); 
	}
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
			parlst.addBool("gpuAcceleration",AMBOCC_USEGPU_BY_DEFAULT,"Use GPU acceleration");
			parlst.addInt("textSize",AMBOCC_DEFAULT_TEXT_SIZE,"Depth texture size(should be 2^n)");
			parlst.addInt("reqViews",AMBOCC_DEFAULT_NUM_VIEWS,"Requested views");
			break;
		default: assert(0);
	}
}
bool AmbientOcclusionPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	assert(filter->text() == filterName(FP_AMBIENT_OCCLUSION));
	useGPU = par.getBool("gpuAcceleration");
	textSize = par.getInt("textSize");
	textArea = textSize*textSize;
	numViews = par.getInt("reqViews");

	if ((unsigned int)m.cm.vn > textArea)
	{
		Log(0, "Too many vertices: up to %d are allowed", textArea);
		return false;
	}

	if (textSize < 15)
	{
		Log(0, "Texture size is too small, 16x16 used instead");
		textSize = 16;
		textArea = textSize*textSize;
	}
	if (textSize > 1024)
	{
		Log(0, "Texture size is too large, 1024x1024 used instead");
		textSize = 1024;
		textArea = textSize*textSize;
	}

	GLfloat *occlusion = new GLfloat[m.cm.vn];
	typedef std::vector<vcg::Point3f> vectP3f;
	vectP3f::iterator vi;
	vectP3f posVect;
	Point3f *vp = new Point3f[m.cm.vn],
	        *vn = new Point3f[m.cm.vn];
	QGLWidget qWidget;

	//Creates a new RC, initializes everything (glew, textures, FBO..)
	if (!initContext(qWidget))
	{
		Log(0,"Unable to create a new GL context");
		return false;
	}

	if(useGPU)
		vertexCoordsToTexture( m );
	else
	{
		//Yes, the loop down there is needed
		for (int j=0; j<m.cm.vn; ++j)
			occlusion[j] = 0.0f;

		vertexCoordsToArray ( m, vp, vn );
	}
	
	//Prepare mesh to be rendered
	vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
	vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(m.cm);
	meshBBox = m.cm.bbox;
	m.glw.Update();

	//Generates the views to be used
	GenNormal<float>::Uniform(numViews,posVect);
	numViews = posVect.size();

	glClearColor(0.0, 0.0, 0.0, 0.0);

	if (useGPU)
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fboDepthTest );

		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);  //final.rgba = min(2^31, src.rgba*1 + dest.rgba*1);
	}

	int c=0;
	for (vi = posVect.begin(); vi != posVect.end(); vi++)
	{
		glClear(GL_DEPTH_BUFFER_BIT );

		setCamera(*vi);

		glColorMask(0, 0, 0, 0);
		renderMesh(m);
		glColorMask(1, 1, 1, 1);
		
		if (useGPU)
			generateOcclusionHW();
		else
			generateOcclusionSW(vp, vn, occlusion, m.cm.vn);
		
		if ( c%5 == 0 )
			cb( 100*c/posVect.size() , "Calculating Ambient Occlusion...");
		c++;
	}

	if (useGPU)
	{
		applyOcclusionHW(m);
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	}
	else
		applyOcclusionSW(m,occlusion);


	Log(0,"Successfully calculated ambient occlusion after %i iterations",c );


	/********** Clean up the mess ************/
	if (GLEW_EXT_framebuffer_object)
	{
		glDetachObjectARB(fboDepthTest, resultBufferText);
		glDetachObjectARB(fboDepthTest, depthBufferText);
		glDeleteFramebuffersEXT(1, &fboDepthTest);
	}

	if (useGPU)
	{
		glDetachShader(shdrID, vs);
		glDetachShader(shdrID, fs);
		glDeleteShader(shdrID);
	}

	delete [] vp;
	delete [] vn;
	delete [] occlusion;

	qWidget.doneCurrent();

	return true;
}
void AmbientOcclusionPlugin::renderMesh(MeshModel &m)
{
	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.0, 1.0 );

	glPushMatrix();
		glMultMatrix(m.cm.Tr);
		m.glw.Draw(vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMNone);
	glPopMatrix();

	glDisable( GL_POLYGON_OFFSET_FILL );
}

void AmbientOcclusionPlugin::writeTextureToFile(char* filename, GLuint textureID, int textureSize, const int channels, GLenum format, GLenum type, bool scale)
{
	GLfloat *data = new GLfloat[textureSize*channels];
	unsigned char *pixels = new unsigned char[textureSize*channels];

	glBindTexture(GL_TEXTURE_2D, textureID);
	glGetTexImage(GL_TEXTURE_2D, 0, format, type, data);
	
	if (scale)
	{
		float *fmin = new float[channels];
		float *fmax = new float[channels];
		float *fscale = new float[channels];

		for (int i=0; i<channels; ++i)
		{
			fmin[i] = 10.0f;
			fmax[i] = -10.0f;
		}

		for (int i=0; i<textureSize; ++i)
		{
			for (int j=0; j<channels; ++j)
			{
				fmin[j] = std::min(fmin[j], data[i*channels+j]);
				fmax[j] = std::max(fmax[j], data[i*channels+j]);
			}
		}
		
		for (int i=0; i<channels; ++i)
			fscale[i] = 1.0f / (fmax[i] - fmin[i]);

		for (int i=0; i<textureSize; ++i)
			for (int j=0; j<channels; ++j)
				data[i*channels+j] = (data[i*channels+j] - fmin[j]) * fscale[j];
		
		delete [] fmin;
		delete [] fmax;
		delete [] fscale;
	}

	for (int i=0; i<textureSize; ++i)
		for (int j=0; j<channels; ++j)
			pixels[i*channels+j] = (unsigned char)(data[i*channels+j] * 255.0f);

	FILE *f;
	
	fopen_s(&f, filename, "wb");
	fwrite(pixels, sizeof(unsigned char), textureSize*channels, f);
	fclose(f);

	delete [] pixels;
	delete [] data;
}

void AmbientOcclusionPlugin::initTextures(GLenum colorFormat, GLenum depthFormat)
{
	//*******SETS DEFAULT OPENGL STUFF**********/
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );

	vertexCoordText = 20;
	vertexNrmlsText = 30;
	resultBufferText= 40;

	if (useGPU)
	{
		//*******INIT VERTEX COORDINATES TEXTURE*********/
		glGenTextures (1, &vertexCoordText);
		glBindTexture(GL_TEXTURE_2D, vertexCoordText);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, 
					 textSize, textSize, 0, GL_RGBA, GL_FLOAT, 0);

		//*******INIT NORMAL VECTORS TEXTURE*********/
		glGenTextures (1, &vertexNrmlsText);
		glBindTexture(GL_TEXTURE_2D, vertexNrmlsText);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, 
					 textSize, textSize, 0, GL_RGBA, GL_FLOAT, 0);
	}

	//*******INIT RESULT TEXTURE*********/
	glGenTextures (1, &resultBufferText);
	glBindTexture(GL_TEXTURE_2D, resultBufferText);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, 
				 textSize, textSize, 0, GL_RGBA, GL_FLOAT, 0);

	//*******INIT DEPTH TEXTURE*********/
	glGenTextures(1, &depthBufferText);
	glBindTexture(GL_TEXTURE_2D, depthBufferText);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,           GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,           GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,       GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,       GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB,   GL_LUMINANCE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

	glTexImage2D (GL_TEXTURE_2D, 0, depthFormat,
	              textSize, textSize, 0, 
				  GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

bool AmbientOcclusionPlugin::initContext(QGLWidget &qWidget)
{
	QGLFormat qFormat = QGLFormat::defaultFormat();
	qFormat.setAlpha(true);
	qFormat.setDepth(true);
	qWidget.setFormat(qFormat);

	if(!qWidget.isValid())
	{
		Log(0,"Error: Unable to create a new QGLWidget");
		return false;
	}

	qWidget.setFixedSize(textSize,textSize);
	qWidget.makeCurrent();

	//*******INIT GLEW********/
	GLint glewError = glewInit();
	if (glewError)
	{
		Log(0,(const char*)glewGetErrorString(glewError));
		return false;
	}

	if (useGPU && !(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
	{
		useGPU = false;
		Log(0, "Shaders are not supported, using Software mode");
		return false;
	}

	//*******LOADS AMBIENT OCCLUSION SHADER*********/
	if (useGPU)
		set_shaders("ambient_occlusion",vs,fs,shdrID);

	//GL_RGBA32F_ARB works on nv40+(GeForce6 or newer) and ATI hardware
	initTextures(GL_RGBA8, GL_DEPTH_COMPONENT24);

	//*******INIT FBO*********/
	if (GLEW_EXT_framebuffer_object)
	{
		fboDepthTest = 10;
		glGenFramebuffersEXT ( 1, &fboDepthTest);
		glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT, fboDepthTest);
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, resultBufferText, 0);
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBufferText, 0 );

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
		
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0);
	}
	else
	{
		Log(0,"Error: Framebuffers are not supported");
		return false;
	}

	glViewport(0.0, 0.0, textSize, textSize);

	return true;
}

void AmbientOcclusionPlugin::vertexCoordsToTexture(MeshModel &m)
{
	GLfloat *vertexPosition= new GLfloat[textArea*4];
	GLfloat *vertexNormals = new GLfloat[textArea*4];

	//Copies each vertex's position and normal in new vectors
	for (int i=0; i < m.cm.vn; ++i)
	{
		//Vertex position
		vertexPosition[i*4+0] = m.cm.vert[i].P().X();
		vertexPosition[i*4+1] = m.cm.vert[i].P().Y();
		vertexPosition[i*4+2] = m.cm.vert[i].P().Z();
		vertexPosition[i*4+3] = 1.0;

		//Normal vector for each vertex
		vertexNormals[i*4+0] = m.cm.vert[i].N().X();
		vertexNormals[i*4+1] = m.cm.vert[i].N().Y();
		vertexNormals[i*4+2] = m.cm.vert[i].N().Z();
		vertexNormals[i*4+3] = 1.0;
	}

	//The aforementioned vectors are used to encode a texture that stores each vertex's position & normal
	//Those texture are then used to perform a GPU occlusion test with each view's depth buffer
	
	//Write vertex coordinates
	glBindTexture(GL_TEXTURE_2D, vertexCoordText);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textSize, textSize, GL_RGBA, GL_FLOAT, vertexPosition);	
	delete [] vertexPosition;

	//Write normal directions
	glBindTexture(GL_TEXTURE_2D, vertexNrmlsText);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textSize, textSize, GL_RGBA, GL_FLOAT, vertexNormals);
	delete [] vertexNormals;

	glBindTexture(GL_TEXTURE_2D, NULL);
}


void AmbientOcclusionPlugin::vertexCoordsToArray(MeshModel &m, Point3f *vp, Point3f *vn)
{
	if (vp == NULL || vn == NULL)
	{
		Log(0, "Internal error: vertex and normal arrays are NULL");
		return;
	}

	//Copies each vertex's position and normal in new vectors
	for (int i=0; i < m.cm.vn; ++i)
	{
		vp[i] = m.cm.vert[i].P();
		vn[i] = m.cm.vert[i].N();
	}
}
void AmbientOcclusionPlugin::setCamera(Point3f camDir)
{
	cameraDir = camDir;
	GLfloat d = (meshBBox.Diag()/2.0) * 1.1,
	        k = 0.1f;
	Point3f eye = meshBBox.Center() + camDir * (d+k);

	glViewport(0.0, 0.0, textSize, textSize);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-d, d, -d, d, k, k+(2.0*d) );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.X(), eye.Y(), eye.Z(),
			  meshBBox.Center().X(), meshBBox.Center().Y(), meshBBox.Center().Z(),
			  0.0, 1.0, 0.0);
}


void AmbientOcclusionPlugin::generateOcclusionHW(void)
{
	GLfloat *mvMatrix_f = new GLfloat[16];
	GLfloat *prMatrix_f = new GLfloat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mvMatrix_f);
	glGetFloatv(GL_PROJECTION_MATRIX, prMatrix_f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	

	glUseProgram(shdrID);

	//Vertex position texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, vertexCoordText);
	glUniform1i(glGetUniformLocation(shdrID, "vTexture"), 0);

	//Normal direction texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, vertexNrmlsText);
	glUniform1i(glGetUniformLocation(shdrID, "nTexture"), 1);

	//Depthmap
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthBufferText);
	glUniform1i(glGetUniformLocation(shdrID, "dTexture"), 2);

	//View Direction
	glUniform3f(glGetUniformLocation(shdrID, "viewDirection"), cameraDir.X(), cameraDir.Y(), cameraDir.Z());

	//ModelView Matrix
	glUniformMatrix4fv(glGetUniformLocation(shdrID, "mvMatrix"), 1, GL_FALSE, (const GLfloat*)mvMatrix_f);

	//Projection Matrix
	glUniformMatrix4fv(glGetUniformLocation(shdrID, "prMatrix"), 1, GL_FALSE, (const GLfloat*)prMatrix_f);

	//Texture Size
	glUniform1i(glGetUniformLocation(shdrID, "texSize"), textSize);

	//Need to clear the depthBuffer if we don't
	//want a mesh-shaped hole in the middle of the S.A.Q. :)
	glClear(GL_DEPTH_BUFFER_BIT);

	//Screen aligned Quad
	glBegin(GL_QUADS);
		glVertex3f(-1.0f, -1.0f, 0.0f); //L-L
		glVertex3f( 1.0f, -1.0f, 0.0f); //L-R
		glVertex3f( 1.0f,  1.0f, 0.0f); //U-R
		glVertex3f(-1.0f,  1.0f, 0.0f); //U-L
	glEnd();

	glUseProgram(0);

	delete [] mvMatrix_f;
	delete [] prMatrix_f;
}

void AmbientOcclusionPlugin::generateOcclusionSW(Point3f *vp, Point3f *vn, GLfloat *occlusion, GLuint numVert)
{
	if (vp == NULL || vn == NULL)
	{
		Log(0, "Internal error: vertex and normal arrays are NULL");
		return;
	}

	GLdouble *resCoords  = new GLdouble[3];
	GLdouble *mvMatrix_f = new GLdouble[16];
	GLdouble *prMatrix_f = new GLdouble[16];
	GLint    *viewpSize  = new GLint[4];
	GLfloat  *dFloat     = new GLfloat[textArea];

	glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix_f);
	glGetDoublev(GL_PROJECTION_MATRIX, prMatrix_f);
	glGetIntegerv(GL_VIEWPORT, viewpSize);

	glReadBuffer(GL_DEPTH_ATTACHMENT_EXT);
	glReadPixels(0, 0, textSize, textSize, GL_DEPTH_COMPONENT, GL_FLOAT, dFloat);

	for (unsigned int i=0; i<numVert; ++i)
	{
		gluProject( vp[i].X(), vp[i].Y(), vp[i].Z(),
				   (const GLdouble *) mvMatrix_f, (const GLdouble *) prMatrix_f, (const GLint *) viewpSize,
				   &resCoords[0], &resCoords[1], &resCoords[2] );

		int x = floor(resCoords[0]);
		int y = floor(resCoords[1]);
		
		if ( resCoords[2] <= (GLdouble)dFloat[textSize*y+x] )
		{
			vn[i].Normalize();
			cameraDir.Normalize();
			occlusion[i] += max( vn[i]*cameraDir, 0.0f);
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
	const float k = AMBOCC_HV / (numViews);

	GLdouble *result = new GLdouble[textArea*4];
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadPixels(0, 0, textSize, textSize, GL_RGBA, GL_FLOAT, result);
	
	/*
	float fmin = 100.0, fmax = -100.0, fscale = 0.0;
	for (int i=0; i<m.cm.vn; ++i)
	{
		fmin = std::min(fmin, (float)(result[i*4+0]));
		fmax = std::max(fmax, (float)(result[i*4+0]));
	}
	fscale = 1.0f / (fmax - fmin);
	/**/

	for (int i = 0; i < m.cm.vn; i++)
	{
		m.cm.vert[i].Q() = result[i*4+0] * k;
		m.cm.vert[i].C()[0] = m.cm.vert[i].Q() * 255.0f;
		m.cm.vert[i].C()[1] = m.cm.vert[i].Q() * 255.0f;
		m.cm.vert[i].C()[2] = m.cm.vert[i].Q() * 255.0f;
	}

	delete [] result;
}

void AmbientOcclusionPlugin::applyOcclusionSW(MeshModel &m, GLfloat *aoValues)
{
	const float k = AMBOCC_HV / (numViews);

	/*
	float fmin = 100.0, fmax = -100.0, fscale = 0.0;
	for (int i=0; i<m.cm.vn; ++i)
	{
		fmin = std::min(fmin, (float)(aoValues[i]));
		fmax = std::max(fmax, (float)(aoValues[i]));
	}
	fscale = 1.0f / (fmax - fmin);
	*/

	for (int i = 0; i < m.cm.vn; i++)
	{
		m.cm.vert[i].Q() = aoValues[i] * k;
		m.cm.vert[i].C()[0] = m.cm.vert[i].Q() * 255.0f;
		m.cm.vert[i].C()[1] = m.cm.vert[i].Q() * 255.0f;
		m.cm.vert[i].C()[2] = m.cm.vert[i].Q() * 255.0f;
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
		int errV;
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
		int errF;
		glGetShaderiv(f,GL_COMPILE_STATUS,&errF);
		assert(errF==GL_TRUE);
		file.close();
	}

	pr = glCreateProgram();

	glAttachShader(pr,v);
	glAttachShader(pr,f);

	glLinkProgram(pr);
}


Q_EXPORT_PLUGIN(AmbientOcclusionPlugin)
