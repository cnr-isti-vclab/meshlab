#include <iostream>

#include <GL/glew.h>

#include <QGLContext>
#include <QDomNode>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QGLFramebufferObject>


#include "alignset.h"

#include <wrap/gl/shot.h>
#include <wrap/gl/camera.h>
//#include <vcg/math/shot.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <wrap/io_trimesh/import_ply.h>

#include "shutils.h"

#define GL_STRINGFY(X) #X

using namespace std;

AlignSet::AlignSet()
	: mode(COMBINE)
	, target(NULL)
	, render(NULL)
	, vbo(0)
	, nbo(0)
	, cbo(0)
	, ibo(0)
	, error(0)
	, depthFB(0)
	, depthTex(0)
	, prjTex(0)
	, depthFB2(0)
	, depthTex2(0)
	, prjTex2(0)
	, depthFB3(0)
	, depthTex3(0)
	, prjTex3(0)
	, depthPrg(0)
	, depthW(1)
	, depthH(1)
{

  box.SetNull();
  correspList = new QList<PointCorrespondence*>();
  imageRatio=1;
}

AlignSet::~AlignSet() {
  if(target) delete []target;
  if(render) delete []render;
  delete correspList;
}

void AlignSet::initializeGL() {
  programs[COLOR] = createShaders("varying vec4 color; void main() { gl_Position = ftransform(); color = gl_Color; }",
    "varying vec4 color; void main() { gl_FragColor = color; }");
  programs[NORMALMAP] = createShaders("varying vec3 normal; void main() { normal = gl_NormalMatrix * gl_Normal;	gl_Position = ftransform(); }",
    "varying vec3 normal; void main() { "
    "vec3 color = normalize(normal); color = color * 0.5 + 0.5; gl_FragColor = vec4(color, 1.0); }");
  programs[COMBINE] = createShaders("varying vec3 normal; varying vec4 color; void main() { "
    "normal = gl_NormalMatrix * gl_Normal; gl_Position = ftransform(); color = gl_Color; }",
    "varying vec3 normal; varying vec4 color; void main() { "
    "vec3 ncolor = normalize(normal); ncolor = ncolor * 0.5 + 0.5; "
    "float t = color.x*color.x; gl_FragColor = (1-t)*color + t*(vec4(ncolor, 1.0)); }");
  programs[SPECULAR] = createShaders("varying vec3 reflection; void main() { "
    "vec3 normal = normalize(gl_NormalMatrix * gl_Normal); vec4 position = gl_ModelViewMatrix * gl_Vertex; "
    "reflection = reflect(position.xyz, normal); gl_Position = ftransform(); }",
    "varying vec3 reflection; varying vec4 color; void main() { "
    "vec4 ncolor; ncolor.xyz = normalize(reflection); ncolor.w = 1.0; gl_FragColor = ncolor * 0.5 + 0.5; }");
  programs[SILHOUETTE] = createShaders("varying vec4 color; void main() { gl_Position = ftransform(); color = gl_Color; }",
    "varying vec4 color; void main() { gl_FragColor = color; }");

  programs[SPECAMB] = createShaders("varying vec3 reflection; varying vec4 color; void main() { "
    "vec3 normal = normalize(gl_NormalMatrix * gl_Normal); vec4 position = gl_ModelViewMatrix * gl_Vertex; "
    "reflection = reflect(position.xyz, normal); gl_Position = ftransform(); color = gl_Color; }",
    "varying vec3 reflection; varying vec4 color; void main() { "
    "vec3 ncolor = normalize(reflection); ncolor = ncolor * 0.5 + 0.5; "
    "float t = color.x*color.x; gl_FragColor = (1-t)*color + t*(vec4(ncolor, 1.0)); }");


  // generate a new VBO and get the associated ID
  glGenBuffersARB(1, &vbo);
  glGenBuffersARB(1, &nbo);
  glGenBuffersARB(1, &cbo);
  glGenBuffersARB(1, &ibo);

  {
	glGenFramebuffers(1, &depthFB);
	glGenTextures(1, &depthTex);
	glGenTextures(1, &prjTex);

	glGenFramebuffers(1, &depthFB2);
	glGenTextures(1, &depthTex2);
	glGenTextures(1, &prjTex2);

	glGenFramebuffers(1, &depthFB3);
	glGenTextures(1, &depthTex3);
	glGenTextures(1, &prjTex3);

	// shadow map
	{
		const char * vsrc = GL_STRINGFY
		(
			void main(void)
			{
				gl_Position = ftransform();
			}
		);

		const char * fsrc = GL_STRINGFY
		(
			void main(void)
			{
				gl_FragColor = vec4(0.0);
			}
		);

		depthPrg = createShaders(vsrc, fsrc);
	}

	// projected texture
	{
		const char * vsrc = GL_STRINGFY
		(
			uniform mat4 uShadowMatrix;

			varying vec4 position;
			varying vec3 normal;
			varying vec4 color;

			void main(void)
			{
				position    = uShadowMatrix * gl_Vertex;
				normal      = gl_NormalMatrix * gl_Normal;
				color       = gl_Color;
				gl_Position = ftransform();
			}
		);

		const char * fsrc = GL_STRINGFY
		(
			uniform sampler2D uShadowMap;
			uniform sampler2D uProjectedImage;

			varying vec4 position;
			varying vec3 normal;
			varying vec4 color;

			void main(void)
			{
				vec4  clr;
				vec3  pos = (position.xyz / position.w) * 0.5 + 0.5;

				vec4  image = texture2D(uProjectedImage, pos.xy);
				float depth = texture2D(uShadowMap,      pos.xy);

				if ((pos.x >= 0.0) && (pos.x <= 1.0) && (pos.y >= 0.0) && (pos.y <= 1.0) && ((pos.z - depth) < 0.001))
				//if ((pos.x >= 0.0) && (pos.x <= 1.0) && (pos.y >= 0.0) && (pos.y <= 1.0) && ((pos.z - depth) > 0.0000001))
				//if ((pos.x >= 0.0) && (pos.x <= 1.0) && (pos.y >= 0.0) && (pos.y <= 1.0))
				{
					clr = image;
					
				}
				else
				{
					vec3  ncolor = normalize(normal) * 0.5 + 0.5;
					float t      = color.x * color.x;
					clr = mix(color, vec4(ncolor, 1.0), t);
					
				}

				gl_FragColor = clr;
			}
		);

		programs[PROJIMG] = createShaders(vsrc, fsrc);
	}

	// multi-projected texture
	{
		const char * vsrc = GL_STRINGFY
		(
			uniform mat4 uShadowMatrix;
			uniform mat4 uShadowMatrix2;
			uniform mat4 uShadowMatrix3;

			varying vec4 position;
			varying vec4 position2;
			varying vec4 position3;
			varying vec3 normal;
			varying vec4 color;

			void main(void)
			{
				position    = uShadowMatrix  * gl_Vertex;
				position2   = uShadowMatrix2 * gl_Vertex;
				position3   = uShadowMatrix3 * gl_Vertex;
				normal      = gl_NormalMatrix * gl_Normal;
				color       = gl_Color;
				gl_Position = ftransform();
			}
		);

		const char * fsrc = GL_STRINGFY
		(
			uniform sampler2D uShadowMap;
			uniform sampler2D uProjectedImage;
			uniform sampler2D uShadowMap2;
			uniform sampler2D uProjectedImage2;
			uniform sampler2D uShadowMap3;
			uniform sampler2D uProjectedImage3;

			varying vec4 position;
			varying vec4 position2;
			varying vec4 position3;
			varying vec3 normal;
			varying vec4 color;

			uniform float w1;
			uniform float w2;
			uniform float w3;

			void main(void)
			{
				vec4  clr  = vec4(0.0);
				vec3  pos  = (position.xyz  / position.w)  * 0.5 + 0.5;
				vec3  pos2 = (position2.xyz / position2.w) * 0.5 + 0.5;
				vec3  pos3 = (position3.xyz / position3.w) * 0.5 + 0.5;

				vec4  image = texture2D(uProjectedImage,   pos.xy);
				float depth = texture2D(uShadowMap,        pos.xy);

				vec4  image2 = texture2D(uProjectedImage2, pos2.xy);
				float depth2 = texture2D(uShadowMap2,      pos2.xy);

				vec4  image3 = texture2D(uProjectedImage3, pos3.xy);
				float depth3 = texture2D(uShadowMap3,      pos3.xy);

				float w = 0.0;


				if ((pos.x >= 0.0) && (pos.x <= 1.0) && (pos.y >= 0.0) && (pos.y <= 1.0) && ((pos.z - depth) < 0.001))
				{
					clr += (image*w1);
					w += w1;
				}
				if ((pos2.x >= 0.0) && (pos2.x <= 1.0) && (pos2.y >= 0.0) && (pos2.y <= 1.0) && ((pos2.z - depth2) < 0.001))
				{
					clr += (image2*w2);
					w += w2;					
				}
				if ((pos3.x >= 0.0) && (pos3.x <= 1.0) && (pos3.y >= 0.0) && (pos3.y <= 1.0) && ((pos3.z - depth3) < 0.001))
				{
					clr += (image3*w3);
					w += w3;					
				}

				if (w > 0.0)
				{
					clr = color * clr / w;
				}
				else
				{
					vec3  ncolor = normalize(normal) * 0.5 + 0.5;
					float t      = color.x * color.x;
					clr = mix(color, vec4(ncolor, 1.0), t);
				}

				gl_FragColor = clr;
			}
		);

		programs[PROJMULTIIMG] = createShaders(vsrc, fsrc);
	}





  }
}

bool AlignSet::ProjectedImageChanged(const QImage & img)
{
	QImage tmp = QGLWidget::convertToGLFormat(img);
	tmp=tmp.scaled(wt,ht);
	//tmp.save("pippo.png");

	/*const GLsizei w = GLsizei(tmp.width());
	const GLsizei h = GLsizei(tmp.height());*/

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, wt, ht, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, prjTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, wt, ht, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp.bits()); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glPopAttrib();

	depthW = wt;
	depthH = ht;

	return true;
}

bool AlignSet::ProjectedMultiImageChanged()
{
	assert(glGetError() == 0);

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glActiveTexture(GL_TEXTURE0);

/////// Image 1
	//arcImages[0]->save("im0.jpg");

	QImage tmp = QGLWidget::convertToGLFormat(*arcImages[0]);
	tmp=tmp.scaled(wt,ht);

	//tmp.save("temp.jpg");

	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, wt, ht, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, prjTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, wt, ht, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp.bits()); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


/////// Image 2

	tmp = QGLWidget::convertToGLFormat(*arcImages[1]);
	tmp=tmp.scaled(wt,ht);
		
	//tmp.save("temp2.jpg");
	
	glBindTexture(GL_TEXTURE_2D, depthTex2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, wt, ht, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, prjTex2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, wt, ht, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp.bits()); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFB2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex2, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


/////// Image 3

	tmp = QGLWidget::convertToGLFormat(*arcImages[2]);
	tmp=tmp.scaled(wt,ht);
		
	//tmp.save("temp3.jpg");
	
	glBindTexture(GL_TEXTURE_2D, depthTex3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, wt, ht, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, prjTex3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, wt, ht, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp.bits()); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFB3);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex3, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glPopAttrib();

	assert(glGetError() == 0);

	depthW = wt;
	depthH = ht;

	return true;
}

bool AlignSet::RenderShadowMap(void)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	assert(glGetError() == 0);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFB);
	assert(glGetError() == 0);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	assert(glGetError() == 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	assert(glGetError() == 0);
	glViewport(0, 0, depthW, depthH);
	glEnable(GL_DEPTH_TEST);

	assert(glGetError() == 0);
	glUseProgram(depthPrg);
	
	assert(glGetError() == 0);
	float _near, _far;
  _near=0.1;
  _far=10000;

  GlShot< vcg::Shot<float> >::GetNearFarPlanes(shotPro, mesh->bbox, _near, _far);
  //assert(_near <= _far);
  if(_near <= 0) _near = 0.1;
  if(_far < _near) _far = 1000;


  assert(glGetError() == 0);

  //render to FBO
  //fbo.bind();
  

  //glViewport(0, 0, wt, ht);
  //glClear(GL_DEPTH_BUFFER_BIT);
  GlShot< vcg::Shot<float> >::SetView(shotPro, 0.5*_near, 2*_far);

  vcg::Matrix44f proj_matrix;
  glGetFloatv(GL_PROJECTION_MATRIX , proj_matrix.V());
  proj_matrix.transposeInPlace();

  vcg::Matrix44f view_matrix;
  glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.V());
  view_matrix.transposeInPlace();

  vcg::Matrix44f shot_matrix =  proj_matrix * view_matrix;
  shot_matrix.transposeInPlace();

	
   shadPro = shot_matrix ;
  
  //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_RECTANGLE_ARB, depthTex, 0);
  glDisable(GL_LIGHTING);
  //bind indices
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);

  //bind vertices
  glEnable(GL_COLOR_MATERIAL);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
  glEnableClientState(GL_VERTEX_ARRAY);                 // activate vertex coords array
  glVertexPointer(3, GL_FLOAT, 0, 0);                   // last param is offset, not ptr

//    err = glGetError();

  //glUseProgram(depthPrg);


  int start = 0;
  int tot = 30000;
  if (mesh->fn>0)
  {
    while(start < mesh->fn) {
      glDrawElements(GL_TRIANGLES, tot*3, GL_UNSIGNED_INT, (void *)(start*3*sizeof(int)));
      start += tot;
      if(start + tot > mesh->fn)
        tot = mesh->fn - start;
    }
  }
  else glDrawArrays(GL_POINTS, 0, mesh->vn);


  glDisableClientState(GL_VERTEX_ARRAY);                // deactivate vertex array
 
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  
  glUseProgram(0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GlShot< vcg::Shot<float> >::UnsetView();

	glPopAttrib();
	//glFinish();

	return true;


}

bool AlignSet::RenderMultiShadowMap(void)
{

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	///// Image 1

	assert(glGetError() == 0);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFB);
	assert(glGetError() == 0);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	assert(glGetError() == 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	assert(glGetError() == 0);
	glViewport(0, 0, depthW, depthH);
	glEnable(GL_DEPTH_TEST);

	assert(glGetError() == 0);
	glUseProgram(depthPrg);

	assert(glGetError() == 0);
	float _near, _far;
	_near=0.1;
	_far=10000;

	GlShot< vcg::Shot<float> >::GetNearFarPlanes(*arcShots[0], mesh->bbox, _near, _far);
	//assert(_near <= _far);
	if(_near <= 0) _near = 0.1;
	if(_far < _near) _far = 1000;


	assert(glGetError() == 0);

	//render to FBO
	//fbo.bind();


	//glViewport(0, 0, wt, ht);
	//glClear(GL_DEPTH_BUFFER_BIT);
	GlShot< vcg::Shot<float> >::SetView(*arcShots[0], 0.5*_near, 2*_far);

	vcg::Matrix44f proj_matrix;
	glGetFloatv(GL_PROJECTION_MATRIX , proj_matrix.V());
	proj_matrix.transposeInPlace();

	vcg::Matrix44f view_matrix;
	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.V());
	view_matrix.transposeInPlace();

	vcg::Matrix44f shot_matrix =  proj_matrix * view_matrix;
	shot_matrix.transposeInPlace();


	prjMats.push_back(shot_matrix);

	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_RECTANGLE_ARB, depthTex, 0);
	glDisable(GL_LIGHTING);
	//bind indices
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);

	//bind vertices
	glEnable(GL_COLOR_MATERIAL);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	glEnableClientState(GL_VERTEX_ARRAY);                 // activate vertex coords array
	glVertexPointer(3, GL_FLOAT, 0, 0);                   // last param is offset, not ptr

	//    err = glGetError();

	//glUseProgram(depthPrg);


	int start = 0;
	int tot = 30000;
	if (mesh->fn>0)
	{
		while(start < mesh->fn) {
			glDrawElements(GL_TRIANGLES, tot*3, GL_UNSIGNED_INT, (void *)(start*3*sizeof(int)));
			start += tot;
			if(start + tot > mesh->fn)
				tot = mesh->fn - start;
		}
	}
	else glDrawArrays(GL_POINTS, 0, mesh->vn);


	glDisableClientState(GL_VERTEX_ARRAY);                // deactivate vertex array

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	glUseProgram(0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GlShot< vcg::Shot<float> >::UnsetView();

	glPopAttrib();
	//glFinish();


	///// Image 2

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	assert(glGetError() == 0);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFB2);
	assert(glGetError() == 0);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	assert(glGetError() == 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	assert(glGetError() == 0);
	glViewport(0, 0, depthW, depthH);
	glEnable(GL_DEPTH_TEST);

	assert(glGetError() == 0);
	glUseProgram(depthPrg);

	assert(glGetError() == 0);

	_near=0.1;
	_far=10000;

	GlShot< vcg::Shot<float> >::GetNearFarPlanes(*arcShots[1], mesh->bbox, _near, _far);
	//assert(_near <= _far);
	if(_near <= 0) _near = 0.1;
	if(_far < _near) _far = 1000;


	assert(glGetError() == 0);

	//render to FBO
	//fbo.bind();


	//glViewport(0, 0, wt, ht);
	//glClear(GL_DEPTH_BUFFER_BIT);
	GlShot< vcg::Shot<float> >::SetView(*arcShots[1], 0.5*_near, 2*_far);

	glGetFloatv(GL_PROJECTION_MATRIX , proj_matrix.V());
	proj_matrix.transposeInPlace();

	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.V());
	view_matrix.transposeInPlace();

	shot_matrix =  proj_matrix * view_matrix;
	shot_matrix.transposeInPlace();


	prjMats.push_back(shot_matrix) ;

	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_RECTANGLE_ARB, depthTex, 0);
	glDisable(GL_LIGHTING);
	//bind indices
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);

	//bind vertices
	glEnable(GL_COLOR_MATERIAL);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	glEnableClientState(GL_VERTEX_ARRAY);                 // activate vertex coords array
	glVertexPointer(3, GL_FLOAT, 0, 0);                   // last param is offset, not ptr

	//    err = glGetError();

	//glUseProgram(depthPrg);


	start = 0;
	tot = 30000;
	if (mesh->fn>0)
	{
		while(start < mesh->fn) {
			glDrawElements(GL_TRIANGLES, tot*3, GL_UNSIGNED_INT, (void *)(start*3*sizeof(int)));
			start += tot;
			if(start + tot > mesh->fn)
				tot = mesh->fn - start;
		}
	}
	else glDrawArrays(GL_POINTS, 0, mesh->vn);


	glDisableClientState(GL_VERTEX_ARRAY);                // deactivate vertex array

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	glUseProgram(0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GlShot< vcg::Shot<float> >::UnsetView();

	glPopAttrib();
	//glFinish();

	///// Image 3

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	assert(glGetError() == 0);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFB3);
	assert(glGetError() == 0);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	assert(glGetError() == 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	assert(glGetError() == 0);
	glViewport(0, 0, depthW, depthH);
	glEnable(GL_DEPTH_TEST);

	assert(glGetError() == 0);
	glUseProgram(depthPrg);

	assert(glGetError() == 0);

	_near=0.1;
	_far=10000;

	GlShot< vcg::Shot<float> >::GetNearFarPlanes(*arcShots[2], mesh->bbox, _near, _far);
	//assert(_near <= _far);
	if(_near <= 0) _near = 0.1;
	if(_far < _near) _far = 1000;


	assert(glGetError() == 0);

	//render to FBO
	//fbo.bind();


	//glViewport(0, 0, wt, ht);
	//glClear(GL_DEPTH_BUFFER_BIT);
	GlShot< vcg::Shot<float> >::SetView(*arcShots[2], 0.5*_near, 2*_far);


	glGetFloatv(GL_PROJECTION_MATRIX , proj_matrix.V());
	proj_matrix.transposeInPlace();


	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.V());
	view_matrix.transposeInPlace();

	shot_matrix =  proj_matrix * view_matrix;
	shot_matrix.transposeInPlace();


	prjMats.push_back(shot_matrix) ;

	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_RECTANGLE_ARB, depthTex, 0);
	glDisable(GL_LIGHTING);
	//bind indices
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);

	//bind vertices
	glEnable(GL_COLOR_MATERIAL);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	glEnableClientState(GL_VERTEX_ARRAY);                 // activate vertex coords array
	glVertexPointer(3, GL_FLOAT, 0, 0);                   // last param is offset, not ptr

	//    err = glGetError();

	//glUseProgram(depthPrg);


	start = 0;
	tot = 30000;
	if (mesh->fn>0)
	{
		while(start < mesh->fn) {
			glDrawElements(GL_TRIANGLES, tot*3, GL_UNSIGNED_INT, (void *)(start*3*sizeof(int)));
			start += tot;
			if(start + tot > mesh->fn)
				tot = mesh->fn - start;
		}
	}
	else glDrawArrays(GL_POINTS, 0, mesh->vn);


	glDisableClientState(GL_VERTEX_ARRAY);                // deactivate vertex array

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	glUseProgram(0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GlShot< vcg::Shot<float> >::UnsetView();

	glPopAttrib();
	//glFinish();

	

	return true;


}

//resample image IF too big.
void AlignSet::resize(int max_side) {
  int w = image->width();
  int h = image->height();
  if(image->isNull()) {
    w =  1024;
    h = 768;
  }

  if(w > max_side) {
    h = h*max_side/w;
    w = max_side;
  }
  if(h > max_side) {
    w = w*max_side/h;
    h = max_side;
  }

  wt=w;
  ht=h;

  if(target) delete []target;
  if(render) delete []render;
  target = new unsigned char[w*h];
  render = new unsigned char[w*h];


  if(image->isNull()) return;
  //resize image and store values into render
  QImage im;
  if(w != image->width() || h != image->height())
    im = image->scaled(w, h, Qt::IgnoreAspectRatio); //Qt::KeepAspectRatio);
  else im = *image;
  //im.save("image.jpg");
  assert(w == im.width());
  assert(h == im.height());
  //im.save("cambi.jpg");
  QColor color;
  int offset = 0;
  //equalize image
  int histo[256];
  memset(histo, 0, 256*sizeof(int));
  for (int y = h-1; y >= 0; y--) {
    for (int x = 0; x < w; x++) {
      color.setRgb(im.pixel(x, y));
      unsigned char c = (unsigned char)(color.red() * 0.3f + color.green() * 0.59f + color.blue() * 0.11f);
      target[offset] = c;
      histo[c]++;
      offset++;
    }
  }
#ifdef RESCALE_HISTO
  int cumulative[256];
  cumulative[0] = histo[0];
  for(int i = 1; i < 256; i++)
    cumulative[i] = cumulative[i-1] + histo[i];

  int min = 0;
  int max = 255;
  for(int i = 0; i < 256; i++) {
    if(cumulative[i] > 20) break;
    min = i;
  }

  //invert cumulative..
  cumulative[255] = histo[255];
  for(int i = 254; i >= 0; i--)
    cumulative[i] = cumulative[i+1] + histo[i];

  for(int i = 255; i >= 0; i--) {
    if(cumulative[i] > 20) break;
    max = i;
  }
  assert(max > min);
  //rescale between min and max (should use bresenham but I am lazy
  unsigned char equa[256];
  for(int i = 0; i < 256; i++) {
    if(i < min) equa[i] = 0;
    if(i > max) equa[i] = 255;
    equa[i] = (255*(i - min))/(max - min);
  }
  for(int i = 0; i < w*h; i++)
    target[i] = equa[target[i]];
#endif
}

void AlignSet::renderScene(vcg::Shot<float> &view, int component, bool save) {
  QSize fbosize(wt,ht);
  QGLFramebufferObjectFormat frmt;
  frmt.setInternalTextureFormat(GL_RGBA);
  frmt.setAttachment(QGLFramebufferObject::Depth);
  QGLFramebufferObject fbo(fbosize,frmt);

  float _near, _far;
  _near=0.1;
  _far=10000;

  GlShot< vcg::Shot<float> >::GetNearFarPlanes(view, mesh->bbox, _near, _far);
  //assert(_near <= _far);
  if(_near <= 0) _near = 0.1;
  if(_far < _near) _far = 1000;


//GLenum err = glGetError();

  //render to FBO
  fbo.bind();

  glViewport(0, 0, wt, ht);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GlShot< vcg::Shot<float> >::SetView(view, 0.5*_near, 2*_far);

//	err = glGetError();

  //mode=PROJIMG;

  bool use_colors = false;
  bool use_normals = false;
  bool use_prjimg = false;
  bool use_prjmultiimg = false;
  int program = programs[mode]; //standard pipeline
  switch(mode){
   case COLOR:
     use_colors = true;
     break;
   case NORMALMAP:
   case SPECULAR:
     use_normals = true;
     break;
   case COMBINE:
   case SPECAMB:
     use_colors = true;
     use_normals = true;
     break;
   case SILHOUETTE:
     break;
   case PROJIMG:
	     use_colors  = true;
		 use_normals = true;
		 use_prjimg  = true;
		 //RenderShadowMap();
	break;
	case PROJMULTIIMG:
	     use_colors  = true;
		 use_normals = true;
		 use_prjmultiimg  = true;
		 //RenderShadowMap();
	break;
   default: assert(0);
  }
  glDisable(GL_LIGHTING);
  //bind indices
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);

  //bind vertices
  glEnable(GL_COLOR_MATERIAL);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
  glEnableClientState(GL_VERTEX_ARRAY);                 // activate vertex coords array
  glVertexPointer(3, GL_FLOAT, 0, 0);                   // last param is offset, not ptr

//    err = glGetError();

  glUseProgram(program);

//    err = glGetError();

  if(use_colors) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, cbo);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
  }
  if(use_normals) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, nbo);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, 0);
  }

  if(use_prjimg) {

    //RenderShadowMap();
	GLint loc = -1;

	loc = glGetUniformLocation(program, "uShadowMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, shadPro.V());

	loc = glGetUniformLocation(program, "uShadowMap");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(program, "uProjectedImage");
	glUniform1i(loc, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, prjTex);

	glActiveTexture(GL_TEXTURE0);

	
  }

  if(use_prjmultiimg) {

    //RenderShadowMap();
	GLint loc = -1;

	//// Image 1

	loc = glGetUniformLocation(program, "uShadowMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, prjMats[0].V());

	loc = glGetUniformLocation(program, "w1");
	glUniform1f(loc, arcMI[0]);

	loc = glGetUniformLocation(program, "w2");
	glUniform1f(loc, arcMI[1]);

	loc = glGetUniformLocation(program, "w3");
	glUniform1f(loc, arcMI[2]);

	loc = glGetUniformLocation(program, "uShadowMap");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(program, "uProjectedImage");
	glUniform1i(loc, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, prjTex);

	//// Image 2

	loc = glGetUniformLocation(program, "uShadowMatrix2");
	glUniformMatrix4fv(loc, 1, GL_FALSE, prjMats[1].V());

	loc = glGetUniformLocation(program, "uShadowMap2");
	glUniform1i(loc, 2);

	loc = glGetUniformLocation(program, "uProjectedImage2");
	glUniform1i(loc, 3);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthTex2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, prjTex2);

	//// Image 3

	loc = glGetUniformLocation(program, "uShadowMatrix3");
	glUniformMatrix4fv(loc, 1, GL_FALSE, prjMats[2].V());

	loc = glGetUniformLocation(program, "uShadowMap3");
	glUniform1i(loc, 4);

	loc = glGetUniformLocation(program, "uProjectedImage3");
	glUniform1i(loc, 5);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, depthTex3);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, prjTex3);


	glActiveTexture(GL_TEXTURE0);

	
  }

//	err = glGetError();

  int start = 0;
  int tot = 30000;
  if (mesh->fn>0)
  {
    while(start < mesh->fn) {
      glDrawElements(GL_TRIANGLES, tot*3, GL_UNSIGNED_INT, (void *)(start*3*sizeof(int)));
      start += tot;
      if(start + tot > mesh->fn)
        tot = mesh->fn - start;
    }
  }
  else glDrawArrays(GL_POINTS, 0, mesh->vn);


  if(use_prjimg) {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
  }

  if(use_prjmultiimg) {
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	  
	  glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
  }



  delete [] render;
  render = new unsigned char[wt*ht];

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  switch(component) {
    case 0: glReadPixels( 0, 0, wt, ht, GL_RED, GL_UNSIGNED_BYTE, render); break;
    case 1: glReadPixels( 0, 0, wt, ht, GL_GREEN, GL_UNSIGNED_BYTE, render); break;
    case 2: glReadPixels( 0, 0, wt, ht, GL_BLUE, GL_UNSIGNED_BYTE, render); break;
    case 3: glReadPixels( 0, 0, wt, ht, GL_ALPHA, GL_UNSIGNED_BYTE, render); break;
    case 4: break;
  }

  //err = glGetError();

  glDisableClientState(GL_VERTEX_ARRAY);                // deactivate vertex array
  if(use_colors) glDisableClientState(GL_COLOR_ARRAY);
  if(use_normals) glDisableClientState(GL_NORMAL_ARRAY);

  //err = glGetError();

  // bind with 0, so, switch back to normal pointer operation
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

  switch(mode) {
    case SILHOUETTE:
    case COLOR:
    case COMBINE:
    case NORMALMAP: glEnable(GL_LIGHTING); break;
    default: break;
  }

  // standard opengl pipeline is re-activated
  glUseProgram(0);

  GlShot< vcg::Shot<float> >::UnsetView();

  //save=false;
  //glFinish();
  if (save)
  {
	 QImage l=fbo.toImage();
     rend=l;
	 l.save("rendering.jpg");
  }
  else
	  rend=fbo.toImage();

  fbo.release();

}

void AlignSet::readRender(int component) {
  QSize fbosize(wt,ht);
  QGLFramebufferObjectFormat frmt;
  frmt.setInternalTextureFormat(GL_RGBA);
  frmt.setAttachment(QGLFramebufferObject::Depth);
  QGLFramebufferObject fbo(fbosize,frmt);

  fbo.bind();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  switch(component) {
  case 0: glReadPixels( 0, 0, width(), height(), GL_RED, GL_UNSIGNED_BYTE, render); break;
  case 1: glReadPixels( 0, 0, width(), height(), GL_GREEN, GL_UNSIGNED_BYTE, render); break;
  case 2: glReadPixels( 0, 0, width(), height(), GL_BLUE, GL_UNSIGNED_BYTE, render); break;
  case 3: glReadPixels( 0, 0, width(), height(), GL_ALPHA, GL_UNSIGNED_BYTE, render); break;
  }
  QImage l=fbo.toImage();
  //l.save("puppo.jpg");
  fbo.release();
}

GLuint AlignSet::createShaderFromFiles(QString name) {
  QString vert = "shaders/" + name + ".vert";
  QString frag = "shaders/" + name + ".frag";

  const char *vs_src = ShaderUtils::importShaders(vert.toLocal8Bit().data());
  if(!vs_src) {
    cerr << "Could not load shader: " << qPrintable(vert) << endl;
    return 0;
  }

  const char *fs_src = ShaderUtils::importShaders(frag.toLocal8Bit().data());
  if(!fs_src) {
    cerr << "Could not load shader: " << qPrintable(frag) << endl;
    return 0;
  }

  return createShaders(vs_src, fs_src);
}

GLuint AlignSet::createShaders(const char *vert, const char *frag) {
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vert, NULL);
  ShaderUtils::compileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &frag, NULL);
  ShaderUtils::compileShader(fs);

  GLuint prog = glCreateProgram();
  glAttachShader(prog, vs);
  glAttachShader(prog, fs);

  ShaderUtils::linkShaderProgram(prog);
  return prog;
}

