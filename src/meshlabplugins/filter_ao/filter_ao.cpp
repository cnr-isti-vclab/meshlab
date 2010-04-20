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

#include <GL/glew.h>
#include "AOGLWidget.h"
#include "filter_ao.h"

#include <vcg/math/gen_normal.h>

#include <wrap/qt/checkGLError.h>


#include <iostream>

#define AMBOCC_MAX_TEXTURE_SIZE 2048
#define AMBOCC_DEFAULT_TEXTURE_SIZE 512
#define AMBOCC_DEFAULT_NUM_VIEWS 128
#define AMBOCC_USEGPU_BY_DEFAULT false
#define AMBOCC_USEVBO_BY_DEFAULT false

using namespace std;
using namespace vcg;
static GLuint vs, fs, shdrID;

AmbientOcclusionPlugin::AmbientOcclusionPlugin() 
{ 
	typeList 
	<< FP_VERT_AMBIENT_OCCLUSION
	<< FP_FACE_AMBIENT_OCCLUSION;
	
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);

	useGPU = AMBOCC_USEGPU_BY_DEFAULT;
	numViews = AMBOCC_DEFAULT_NUM_VIEWS;
	depthTexSize = AMBOCC_DEFAULT_TEXTURE_SIZE;
	depthTexArea = depthTexSize*depthTexSize;
	maxTexSize = 16;

	colorFormat = GL_RGBA32F_ARB;
	dataTypeFP = GL_FLOAT;
}

AmbientOcclusionPlugin::~AmbientOcclusionPlugin()
{
}

QString AmbientOcclusionPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId)
	{
    case FP_VERT_AMBIENT_OCCLUSION :  return QString("Ambient Occlusion - Per Vertex");
    case FP_FACE_AMBIENT_OCCLUSION :  return QString("Ambient Occlusion - Per Face");
		default : assert(0); 
	}
	
	return QString("");
}

QString AmbientOcclusionPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId)
	{
		case FP_VERT_AMBIENT_OCCLUSION :  return QString("Generates environment occlusions values for the loaded mesh"); 
		case FP_FACE_AMBIENT_OCCLUSION :  return QString("Generates environment occlusions values for the loaded mesh"); 
		default : assert(0); 
	}

	return QString("");
}

int AmbientOcclusionPlugin::getRequirements(QAction *action)
{
	//no requirements needed
	return 0;
}

void AmbientOcclusionPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{
	switch(ID(action))
	{
		case FP_FACE_AMBIENT_OCCLUSION:
		case FP_VERT_AMBIENT_OCCLUSION:
			parlst.addParam(new RichFloat("dirBias",0,"Directional Bias [0..1]","The balance between a uniform and a directionally biased set of lighting direction<br>:"
											" - 0 means light came only uniformly from any direction<br>"
											" - 1 means that all the light cames from the specified cone of directions <br>"
											" - other values mix the two set of lighting directions "));
			parlst.addParam(new RichInt ("reqViews",AMBOCC_DEFAULT_NUM_VIEWS,"Requested views", "Number of different views uniformly placed around the mesh. More views means better accuracy at the cost of increased calculation time"));
			parlst.addParam(new RichPoint3f("coneDir",Point3f(0,1,0),"Lighting Direction", "Number of different views placed around the mesh. More views means better accuracy at the cost of increased calculation time"));
			parlst.addParam(new RichFloat("coneAngle",30,"Cone amplitude", "Number of different views uniformly placed around the mesh. More views means better accuracy at the cost of increased calculation time"));
			parlst.addParam(new RichBool("useGPU",AMBOCC_USEGPU_BY_DEFAULT,"Use GPU acceleration","In order to use GPU-Mode, your hardware must support FBOs, FP32 Textures and Shaders. Normally increases the performance by a factor of 4x-5x"));
			parlst.addParam(new RichBool("useVBO",AMBOCC_USEVBO_BY_DEFAULT,"Use VBO if supported","By using VBO, Meshlab loads all the vertex structure in the VRam, greatly increasing rendering speed (for both CPU and GPU mode). Disable it if problem occurs"));
			parlst.addParam(new RichInt ("depthTexSize",AMBOCC_DEFAULT_TEXTURE_SIZE,"Depth texture size(should be 2^n)", "Defines the depth texture size used to compute occlusion from each point of view. Higher values means better accuracy usually with low impact on performance"));
			break;
  default: break; // do not add any parameter for the other filters
  }
}
bool AmbientOcclusionPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
    MeshModel &m=*(md.mm());
    if(ID(filter)==FP_FACE_AMBIENT_OCCLUSION ) perFace=true;
	else perFace = false;
	
	useGPU = par.getBool("useGPU");
	useVBO = par.getBool("useVBO");
	depthTexSize = par.getInt("depthTexSize");
	depthTexArea = depthTexSize*depthTexSize;
	numViews = par.getInt("reqViews");
	errInit = false;
	float dirBias = par.getFloat("dirBias");
	Point3f coneDir = par.getPoint3f("coneDir");
	float coneAngle = par.getFloat("coneAngle");
	
	if(perFace) 
	  m.updateDataMask(MeshModel::MM_FACEQUALITY | MeshModel::MM_FACECOLOR);
	else 
		m.updateDataMask(MeshModel::MM_VERTQUALITY | MeshModel::MM_VERTCOLOR);

	
	std::vector<Point3f> unifDirVec;
	GenNormal<float>::Uniform(numViews,unifDirVec);
	
	std::vector<Point3f> coneDirVec;
	GenNormal<float>::UniformCone(numViews, coneDirVec, math::ToRad(coneAngle), coneDir);
	
	std::random_shuffle(unifDirVec.begin(),unifDirVec.end());
	std::random_shuffle(coneDirVec.begin(),coneDirVec.end());

	int unifNum = floor(unifDirVec.size() * (1.0 - dirBias ));
	int coneNum = floor(coneDirVec.size() * (dirBias ));
									
	
	viewDirVec.clear();
	viewDirVec.insert(viewDirVec.end(),unifDirVec.begin(),unifDirVec.begin()+unifNum);
	viewDirVec.insert(viewDirVec.end(),coneDirVec.begin(),coneDirVec.begin()+coneNum);
	
	numViews = viewDirVec.size();

	AOGLWidget *qWidget = new AOGLWidget(0,this);
	qWidget->cb = cb;
	qWidget->m = &m;
	qWidget->viewDirVec = &viewDirVec;
	qWidget->show();  //Ugly, but HAVE to be shown in order for it to work!
	
	return !errInit;
}
	
bool AmbientOcclusionPlugin::processGL(AOGLWidget *aogl, MeshModel &m, vector<Point3f> &posVect)
{
	if (errInit)
		return false;

	checkGLError::qDebug("start");
 	int tInitElapsed = 0;
	QTime tInit, tAll;
	tInit.start();
	tAll.start();

	vector<vcg::Point3f>::iterator vi;

	vcg::tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
	vcg::tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m.cm);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	if (useVBO)
	{
		m.glw.SetHint(vcg::GLW::HNUseVBO);
		m.glw.Update();
	}
	
	tri::UpdateQuality<CMeshO>::VertexConstant(m.cm,0);
	
	if(useGPU)
	{	
		vertexCoordsToTexture( m );
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);  //final.rgba = min(2^31, src.rgba*1 + dest.rgba*1);
	}

	
	tInitElapsed = tInit.elapsed();
	vector<Point3f> faceCenterVec;
	
	for(int i=0;i<m.cm.fn;i++)
		faceCenterVec.push_back(Barycenter(m.cm.face[i]));
	
	for (vi = posVect.begin(); vi != posVect.end(); vi++)
	{
		setCamera(*vi, m.cm.bbox);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);

		if (useGPU)
		{
			glEnable(GL_DEPTH_TEST);

			// FIRST PASS - fill depth buffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboDepth);
			glViewport(0,0,depthTexSize,depthTexSize);
			glClear(GL_DEPTH_BUFFER_BIT);

			glColorMask(0, 0, 0, 0);
			m.glw.DrawFill<GLW::NMNone, GLW::CMNone, GLW::TMNone>();
			glColorMask(1, 1, 1, 1);

			glDisable(GL_POLYGON_OFFSET_FILL);
			
			// SECOND PASS - use depth buffer to check occlusion
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboResult);
			glViewport(0,0,maxTexSize,maxTexSize);
			generateOcclusionHW();
		}
		else
		{
			glDisable(GL_BLEND);
			glClear(GL_DEPTH_BUFFER_BIT);
			// FIRST PASS - fill depth buffer
			glColorMask(0, 0, 0, 0);
			m.glw.DrawFill<GLW::NMNone, GLW::CMNone, GLW::TMNone>();
			glColorMask(1, 1, 1, 1);

			glDisable(GL_POLYGON_OFFSET_FILL);

			// SECOND PASS - use depth buffer to check occlusion
			if(perFace) generateFaceOcclusionSW(m,faceCenterVec);
			else generateOcclusionSW(m);
		}

	}

	if (useGPU)
	{
		applyOcclusionHW(m);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	}

	if(perFace) 
		{
			tri::UpdateColor<CMeshO>::FaceQualityGray(m.cm);
			CMeshO::FaceIterator fi;
			for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD()) 
					(*fi).Q()=(*fi).Q()/numViews;
		} else {
			tri::UpdateColor<CMeshO>::VertexQualityGray(m.cm);
			CMeshO::VertexIterator vi;
			for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) if(!(*vi).IsD()) 
					(*vi).Q()=(*vi).Q()/numViews;
		}
	
	Log(0,"Successfully calculated A.O. after %3.2f sec, %3.2f of which is due to initialization", ((float)tAll.elapsed()/1000.0f), ((float)tInitElapsed/1000.0f) );


	/********** Clean up the mess ************/
	if (useGPU)
	{
		glDisable(GL_BLEND);

		glUseProgram(0);

		glDeleteTextures(1, &vertexCoordTex);
		glDeleteTextures(1, &vertexNormalsTex);
		glDeleteTextures(numTexPages, resultBufferTex);

		glDeleteFramebuffersEXT(1, &fboDepth);
		glDeleteFramebuffersEXT(1, &fboResult);

		glDetachShader(shdrID, vs);
		glDetachShader(shdrID, fs);
		glDeleteShader(shdrID);   //executes but gives INVALID_OPERATION ( ?!?!? )

		delete [] resultBufferTex;
		delete [] resultBufferMRT;
	}

	glDeleteTextures(1, &depthBufferTex);

	if (useVBO)
	m.glw.ClearHint(vcg::GLW::HNUseVBO);

	return true;
} 

void AmbientOcclusionPlugin::initGL(vcg::CallBackPos *cb, unsigned int numVertices)
{
	//******* INIT GLEW ********/
	cb(0, "Initializing: Glew and Hardware Capabilities");
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		Log(0,(const char*)glewGetErrorString(err));
		errInit = true;
		return;
	}

	//******* QUERY HARDWARE FOR: MAX TEX SIZE ********/
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<GLint*>(&maxTexSize) );
	maxTexSize = std::min(maxTexSize, (unsigned int)AMBOCC_MAX_TEXTURE_SIZE);


	if (depthTexSize < 16)
	{
		Log(0, "Texture size is too small, 16x16 used instead");
		depthTexSize = 16;
		depthTexArea = depthTexSize*depthTexSize;
	}
	if (depthTexSize > maxTexSize)
	{
		Log(0, "Texture size is too large, %dx%d used instead",maxTexSize,maxTexSize);
		depthTexSize = maxTexSize;
		depthTexArea = depthTexSize*depthTexSize;
	}
	
	//******* SET DEFAULT OPENGL STUFF **********/
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_TEXTURE_3D_EXT );


	//******* CHECK THAT EVERYTHING IS SUPPORTED **********/
	if (useGPU)
	{
		if (!glewIsSupported("GL_ARB_vertex_shader GL_ARB_fragment_shader"))
		{
			if (!glewIsSupported("GL_EXT_vertex_shader GL_EXT_fragment_shader"))
			{
				Log(0, "Your hardware doesn't support Shaders, which are required for hw occlusion");
				errInit = true;
				return;
			}
		}
		if ( !glewIsSupported("GL_EXT_framebuffer_object") )
		{
			Log(0, "Your hardware doesn't support FBOs, which are required for hw occlusion");
			errInit = true;
			return;
		}

		if ( glewIsSupported("GL_ARB_texture_float") )
		{
			if ( !glewIsSupported("GL_EXT_gpu_shader4") )   //Only DX10-grade cards support FP32 blending
			{
				//colorFormat = GL_RGB16F_ARB;
				//dataTypeFP = GL_HALF_FLOAT_ARB;

				Log(0,"Your hardware can't do FP32 blending, and currently the FP16 version is not yet implemented.");
				errInit = true;
				return;
			}

			colorFormat = GL_RGB32F_ARB;
			dataTypeFP = GL_FLOAT;
		}
		else
		{
			Log(0,"Your hardware doesn't support floating point textures, which are required for hw occlusion");
			errInit = true;
			return;
		}

		unsigned int maxTexPages=1;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, reinterpret_cast<GLint*>(&maxTexPages) );

		//******* CHECK MODEL SIZE ***********/
		if ((maxTexSize*maxTexSize*maxTexPages) < numVertices && useGPU)
		{
			Log(0, "That's a really huge model, I can't handle it in hardware, sorry..");
			errInit = true;
			return;
		}

		//******* FIND BEST COMPROMISE BETWEEN TEX SIZE AND MRTs ********/
		unsigned int smartTexSize;
		for (smartTexSize=64; (smartTexSize*smartTexSize) < (numVertices/maxTexPages); smartTexSize*=2 );
		
		if (smartTexSize > maxTexSize)
		{
			//should ever enter this point, just exit with error
			Log(0,"There was an error while determining best texture size, unable to continue");
			errInit = true;
			return;
		}

		//******* LOAD SHADERS *******/
		cb(30, "Initializing: Shaders and Textures");

		if (maxTexPages == 4)
			set_shaders("ambient_occlusion4",vs,fs,shdrID);
		else
			set_shaders("ambient_occlusion8",vs,fs,shdrID);  //geforce 8+


		maxTexSize = smartTexSize;
		numTexPages = std::min( (numVertices / (smartTexSize*smartTexSize))+1, maxTexPages);
		resultBufferTex = new GLuint[numTexPages];
		resultBufferMRT = new GLenum[numTexPages];

		//******* INIT TEXTURES **********/
		initTextures();

		//*******INIT FBO*********/
		cb(60, "Initializing: Framebuffer Objects");
		
		fboDepth = 0;
		glGenFramebuffersEXT(1, &fboDepth);   // FBO for first pass (1 depth attachment)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboDepth);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBufferTex, 0);

		// only in this way it is possible to read back the depth texture correctly(!!)
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (!checkFramebuffer())
		{
			errInit = true;
			return;
		}
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		

		fboResult = 0;
		glGenFramebuffersEXT(1, &fboResult);   // FBO for second pass (1 color attachment)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboResult);
		for (unsigned int i=0; i<numTexPages; ++i)
		{
			resultBufferMRT[i] = GL_COLOR_ATTACHMENT0_EXT+i;
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, GL_TEXTURE_2D, resultBufferTex[i], 0);
		}

		glDrawBuffers(numTexPages, resultBufferMRT);

		if (!checkFramebuffer())
		{
			errInit = true;
			return;
		}
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	glViewport(0.0, 0.0, depthTexSize, depthTexSize);

	cb(100, "Initializing: Done.");
}

void AmbientOcclusionPlugin::initTextures()
{
	unsigned int potTexSize = 0;

	vertexCoordTex   = 0;
	vertexNormalsTex = 0;
	resultBufferTex  = new GLuint[numTexPages];

	//**** find nearest POT size for numTexPages in order to use it as depth size in 3D Textures ****/
	for (potTexSize=1; potTexSize<numTexPages; potTexSize*=2);

	glGenTextures (1, &depthBufferTex);
	glGenTextures (1, &vertexCoordTex);
	glGenTextures (1, &vertexNormalsTex);
	glGenTextures (numTexPages, resultBufferTex);

	
	//*******INIT DEPTH TEXTURE - 2D *********/
	glBindTexture(GL_TEXTURE_2D, depthBufferTex);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,           GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,           GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,       GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,       GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB,   GL_LUMINANCE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthTexSize, depthTexSize, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);


	//*******INIT VERTEX COORDINATES TEXTURE - 3D *********/
	glBindTexture(GL_TEXTURE_3D, vertexCoordTex);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage3D(GL_TEXTURE_3D, 0, colorFormat, maxTexSize, maxTexSize, potTexSize, 0, GL_RGBA, dataTypeFP, 0);


	//*******INIT NORMAL VECTORS TEXTURE - 3D *********/
	glBindTexture(GL_TEXTURE_3D, vertexNormalsTex);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage3D(GL_TEXTURE_3D, 0, colorFormat, maxTexSize, maxTexSize, potTexSize, 0, GL_RGBA, dataTypeFP, 0);


	//*******INIT RESULT TEXTURE - 2D Array *********/
	for (unsigned int i=0; i<numTexPages; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, resultBufferTex[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, maxTexSize, maxTexSize, 0, GL_RGBA, dataTypeFP, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
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
			assert(0); 
		}

		return false;
	}

	return true;
}

void AmbientOcclusionPlugin::vertexCoordsToTexture(MeshModel &m)
{
	unsigned int texSize = maxTexSize*maxTexSize*numTexPages*4;

	GLfloat *vertexPosition= new GLfloat[texSize];
	GLfloat *vertexNormals = new GLfloat[texSize];
	vcg::Point3<CMeshO::ScalarType> vn;

	//Copies each vertex's position and normal in new vectors
	for (int i=0; i < m.cm.vn; ++i)
	{
		//Vertex position
		vertexPosition[i*4+0] = m.cm.vert[i].P().X();
		vertexPosition[i*4+1] = m.cm.vert[i].P().Y();
		vertexPosition[i*4+2] = m.cm.vert[i].P().Z();
		vertexPosition[i*4+3] = 1.0;

		//Normal vector for each vertex
		vn = m.cm.vert[i].N();
		vertexNormals[i*4+0] = vn.X();
		vertexNormals[i*4+1] = vn.Y();
		vertexNormals[i*4+2] = vn.Z();
		vertexNormals[i*4+3] = 1.0;
	}

	//Write vertex coordinates
	glBindTexture(GL_TEXTURE_3D_EXT, vertexCoordTex);
	glTexSubImage3D(GL_TEXTURE_3D_EXT, 0, 0, 0, 0, maxTexSize, maxTexSize, numTexPages, GL_RGBA, dataTypeFP, vertexPosition);

	//Write normal directions
	glBindTexture(GL_TEXTURE_3D_EXT, vertexNormalsTex);
	glTexSubImage3D(GL_TEXTURE_3D_EXT, 0, 0, 0, 0, maxTexSize, maxTexSize, numTexPages, GL_RGBA, dataTypeFP, vertexNormals);

	delete [] vertexNormals;
	delete [] vertexPosition;
}

void AmbientOcclusionPlugin::setCamera(Point3f camDir, Box3f &meshBBox)
{
	cameraDir = camDir;
	GLfloat d = (meshBBox.Diag()/2.0) * 1.1,
	        k = 0.1f;
	Point3f eye = meshBBox.Center() + camDir * (d+k);

	glViewport(0.0, 0.0, depthTexSize, depthTexSize);

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
	glBindTexture(GL_TEXTURE_3D_EXT, vertexCoordTex);
	glUniform1i(glGetUniformLocation(shdrID, "vTexture"), 1);

	// Set vertex normal texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D_EXT, vertexNormalsTex);
	glUniform1i(glGetUniformLocation(shdrID, "nTexture"), 2);

	// Set view direction
	glUniform3f(glGetUniformLocation(shdrID, "viewDirection"), cameraDir.X(), cameraDir.Y(), cameraDir.Z());

	// Set ModelView-Projection Matrix
	glUniformMatrix4fv(glGetUniformLocation(shdrID, "mvprMatrix"), 1, GL_FALSE, (const GLfloat*)mv_pr_Matrix_f);

	// Set total number of texture pages
	glUniform1f(glGetUniformLocation(shdrID, "numTexPages"), static_cast<float>(numTexPages) );

	// Set texture Size
	glUniform1f(glGetUniformLocation(shdrID, "texSize"), static_cast<float>(depthTexSize) );

	// Set viewport Size
	glUniform1f(glGetUniformLocation(shdrID, "viewpSize"), static_cast<float>(maxTexSize) );

	// Screen-aligned Quad
	glBegin(GL_QUADS);
		glVertex3f(-1.0f, -1.0f, 0.0f); //L-L
		glVertex3f( 1.0f, -1.0f, 0.0f); //L-R
		glVertex3f( 1.0f,  1.0f, 0.0f); //U-R
		glVertex3f(-1.0f,  1.0f, 0.0f); //U-L
	glEnd();

	glUseProgram(0);
}

void AmbientOcclusionPlugin::generateOcclusionSW(MeshModel &m)
{
	GLdouble resCoords[3];
	GLdouble mvMatrix_f[16];
	GLdouble prMatrix_f[16];
	GLint viewpSize[4];
	GLfloat *dFloat = new GLfloat[depthTexArea];

	glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix_f);
	glGetDoublev(GL_PROJECTION_MATRIX, prMatrix_f);
	glGetIntegerv(GL_VIEWPORT, viewpSize);

	glReadPixels(0, 0, depthTexSize, depthTexSize, GL_DEPTH_COMPONENT, GL_FLOAT, dFloat);

	cameraDir.Normalize();
	for (int i=0; i<m.cm.vn; ++i)
	{
		Point3<CMeshO::ScalarType> &vp = m.cm.vert[i].P();
		gluProject(vp.X(), vp.Y(), vp.Z(),
				   (const GLdouble *) mvMatrix_f, (const GLdouble *) prMatrix_f, (const GLint *) viewpSize,
				   &resCoords[0], &resCoords[1], &resCoords[2] );

		int x = floor(resCoords[0]);
		int y = floor(resCoords[1]);
		
		if (resCoords[2] <= (GLdouble)dFloat[depthTexSize*y+x])
		{
			 m.cm.vert[i].Q() += max(m.cm.vert[i].cN().dot(cameraDir), 0.0f);
		}
	}

	delete [] dFloat;
}

void AmbientOcclusionPlugin::generateFaceOcclusionSW(MeshModel &m, vector<Point3f> & faceCenterVec)
{
	GLdouble resCoords[3];
	GLdouble mvMatrix_f[16];
	GLdouble prMatrix_f[16];
	GLint viewpSize[4];
	GLfloat *dFloat = new GLfloat[depthTexArea];
	glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix_f);
	glGetDoublev(GL_PROJECTION_MATRIX, prMatrix_f);
	glGetIntegerv(GL_VIEWPORT, viewpSize);

	glReadPixels(0, 0, depthTexSize, depthTexSize, GL_DEPTH_COMPONENT, GL_FLOAT, dFloat);

	cameraDir.Normalize();
	for (uint i=0; i<faceCenterVec.size(); ++i)
	{
		Point3<CMeshO::ScalarType> &vp = faceCenterVec[i];
		gluProject(vp.X(), vp.Y(), vp.Z(),
							 (const GLdouble *) mvMatrix_f, (const GLdouble *) prMatrix_f, (const GLint *) viewpSize,
							 &resCoords[0], &resCoords[1], &resCoords[2] );
		
		int x = floor(resCoords[0]);
		int y = floor(resCoords[1]);
		
		if (resCoords[2] <= (GLdouble)dFloat[depthTexSize*y+x])
		{
			m.cm.face[i].Q() += max(m.cm.face[i].cN().dot(cameraDir), 0.0f);
		}
	}

delete [] dFloat;
}

void AmbientOcclusionPlugin::applyOcclusionHW(MeshModel &m)
{
	const unsigned int texelNum = maxTexSize*maxTexSize;

	GLfloat *result = new GLfloat[texelNum*4];

	unsigned int nVert=0;

	for (unsigned int n=0; n<numTexPages; ++n)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT+n);
		glReadPixels(0, 0, maxTexSize, maxTexSize, GL_RGBA, GL_FLOAT, result);   //usare GL_RED

		nVert = ( n+1 == numTexPages) ? (m.cm.vn % texelNum) : texelNum;

		for (unsigned int i = 0; i < nVert; i++)
			m.cm.vert[texelNum*n+i].Q() = result[i*4];
	}
	
	delete [] result;
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
	QChar nMRT;
	QByteArray ba;
	QFile file;
	char *data;

	nMRT = fileName.at(fileName.size()-1);
	
	fileName = fileName.left(fileName.size()-1);
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

	fileName = fileName.left(fileName.size()-5);
	fileName.append(nMRT);
	fileName.append(".frag");
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
	unsigned char *cdata = new unsigned char[elems];

	for (int i=0; i<elems; ++i)
		cdata[i] = (unsigned char)(texdata[i]*255.0);

	FILE *f;
	f=fopen(qPrintable(filename) ,"wb+");
	fwrite(cdata,sizeof(unsigned char),elems,f);
	fclose(f);

	delete [] cdata;
}

Q_EXPORT_PLUGIN(AmbientOcclusionPlugin)
