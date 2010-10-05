/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include<GL/glew.h>
#include<vcg/space/point3.h>
#include<vcg/math/matrix44.h>
#include<wrap/gl/space.h>
#include<wrap/gl/math.h>

#include<string>

#include<QImage>
#include<QtOpenGL>
#include <wrap/qt/checkGLError.h>

using namespace vcg;
using namespace std;

#include "cubemap.h"

static Point3f cube_vertices[8] = {

  Point3f(-1.0,  1.0, -1.0), // l t f
  Point3f(-1.0, -1.0, -1.0), // l b f
  Point3f( 1.0, -1.0, -1.0), // r b f
  Point3f( 1.0,  1.0, -1.0), // r t f

  Point3f(-1.0,  1.0,  1.0), // l t b
  Point3f(-1.0, -1.0,  1.0), // l b b
  Point3f( 1.0, -1.0,  1.0), // r b b
  Point3f( 1.0,  1.0,  1.0), // r t b
};

static int cube_faces[6][4] = {

  {3, 2, 6, 7}, // right
  {4, 5, 1, 0}, // left
  {4, 0, 3, 7}, // top
  {1, 5, 6, 2}, // bottom
  {7, 6, 5, 4}, // back
  {0, 1, 2, 3}, // front
};

static float tex_vert[4][2] =
{
	{1,1},{1,0},{0,0},{0,1},
};

void CICubeMap::DrawEnvCube(Matrix44f &tr)
{
	if(!GLEW_ARB_texture_cube_map) 
				 DrawEnvCubeOld(tr);
	else  DrawEnvCubeExt(tr);
}

// Se l'hardware non supporta le cube map si disegna semplicemente un cubo con le sei
// texture caricate indipendemente. Si nota le giunzioni ai bordi

void CICubeMap::DrawEnvCubeOld(Matrix44f &tr)
{
	checkGLError::qDebug("DrawEnvCubeOld: start");
	glMatrixMode(GL_MODELVIEW);
	glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT | GL_TEXTURE_BIT); 
		
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_1D);
  glEnable(GL_TEXTURE_2D);
  glDepthMask(false);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPushMatrix();
	glLoadIdentity();
	glMultMatrix(tr);
	glScalef(radius,radius,radius);

  for (int i=0; i<6; ++i) 
	{
		glBindTexture(GL_TEXTURE_2D,oti[i]);
	
    glBegin(GL_QUADS);
		for (int j=0; j<4; ++j) {
			glTexCoord2f(tex_vert[j][0],tex_vert[j][1]);
 			glVertex(cube_vertices[cube_faces[i][j]]);
    }
    glEnd();
		
  }
	
	glDepthMask(true);
	glPopMatrix();
	glPopAttrib();
	checkGLError::qDebug("DrawEnvCubeOld: end");

}

void CICubeMap::DrawEnvCubeExt(Matrix44f &tr)
{
	Matrix44f ii=tr;
	ii=Invert(ii);
	
	glMatrixMode(GL_MODELVIEW);
	glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT | GL_TEXTURE_BIT ); // saves the current state of opengl 
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glScalef(radius,radius,radius);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	//CIMsg::checkGLError("glEnable(GL_TEXTURE_CUBE_MAP_ARB);");
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB,ti);
	//CIMsg::checkGLError("glBindTexture(GL_TEXTURE_CUBE_MAP_ARB,ti);");
	glDepthMask(false);
	
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//CIMsg::checkGLError("glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER)");
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//CIMsg::checkGLError("glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER)");

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();// Texture
	glLoadIdentity();
	glMultMatrix(ii);
	
 //	LQ.track.inverse().applyGL();

  for (int i=0; i<6; ++i) 
	{
    glBegin(GL_QUADS);
		for (int j=0; j<4; ++j) {
 			glNormal(-cube_vertices[cube_faces[i][j]]);
      glVertex(cube_vertices[cube_faces[i][j]]);
    }
    glEnd();
		
  }
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();// Texture
	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDepthMask(true);

glPopMatrix();
glPopAttrib();
}
bool CICubeMap::GetName(int i, QString basename, QString &filename)
{
	QString exts[6];
	
	exts[0]="_negx";
	exts[1]="_posx";
	exts[2]="_negy";
	exts[3]="_posy";
	exts[4]="_negz";
	exts[5]="_posz";
//	qDebug("filename = %s basename = %s", qPrintable(filename), qPrintable(basename));
  filename=basename;
	QString ext= basename.right(4);
	filename=filename.left(filename.length()-4);
	filename.append(exts[i]);
	filename.append(ext);
//	qDebug("filename = %s basename = %s", qPrintable(filename), qPrintable(basename));
	return true;
}

bool CICubeMap::Load(const char *basename)
{
  if(basename==0) return false;
	bool ret;
	if(!GLEW_ARB_texture_cube_map) 
		ret= LoadOld(basename);
	else ret =LoadExt(basename);
	if(!ret) SetInvalid();
	return ret;
}

bool CICubeMap::LoadOld(const char *basename)
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_1D);
  glEnable(GL_TEXTURE_2D);
	if(oti[0]==0) glGenTextures(6,&(oti[0]));
	QString filename;
		
	for(int i=0;i<6;++i)
		{
			glBindTexture(GL_TEXTURE_2D,oti[i]);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//CIMsg::checkGLError("glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER)");
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			//CIMsg::checkGLError("glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER)");
		
			QImage tt; 	
			
		  if(!GetName(i,basename,filename)) {
					glPopAttrib();
					return false;
				}
			if(!tt.load(filename)) {
					glPopAttrib();
					return false;
				}

			QImage tx = QGLWidget::convertToGLFormat ( tt);

			glBindTexture(GL_TEXTURE_2D, oti[i]);
			glTexImage2D( GL_TEXTURE_2D, 0, 3, tx.width(), tx.height(), 0,
													GL_RGBA, GL_UNSIGNED_BYTE, tx.bits() );
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tx.width(), tx.height(), GL_RGBA, GL_UNSIGNED_BYTE, tx.bits() );

			//CIMsg::checkGLError("glTexImage2D");		
		}

	glPopAttrib();
  return true;
}
bool CICubeMap::LoadExt(const char *basename)
{
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
	if(ti==0) 		glGenTextures(1,&ti);

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB,ti);
	GLenum targets[6];
	targets[0]=GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
	targets[1]=GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
	targets[2]=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
	targets[3]=GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
	targets[4]=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
	targets[5]=GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;

	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//CIMsg::checkGLError("glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER)");
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//CIMsg::checkGLError("glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER)");

		
	for(int i=0;i<6;++i)
		{
			QString filename;
			QImage tt;
			if(!GetName(i,basename,filename)) {
					glPopAttrib();
					return false;
				}

			if(!tt.load(filename)) {
					glPopAttrib();
					QMessageBox::warning(0,"Cubemapped background decoration","Warning unable to load cube map images: " + filename );
					return false;
				}

			QImage tx = QGLWidget::convertToGLFormat ( tt);

			glTexImage2D( targets[i], 0,  3, tx.width(), tx.height(), 0,	GL_RGBA, GL_UNSIGNED_BYTE, tx.bits() );
			gluBuild2DMipmaps(targets[i], 4, tx.width(), tx.height(),     GL_RGBA, GL_UNSIGNED_BYTE, tx.bits() );

			//CIMsg::checkGLError("glTexImage2D");
	
		}

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	
	glPopAttrib();
	return true;
}

