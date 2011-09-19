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
#include <limits>
#include <stdlib.h>
#include "decorate_background.h"
#include <QGLWidget>
#include <wrap/gl/addons.h>

#include <meshlab/glarea.h>
#include <common/pluginmanager.h>



using namespace vcg;

QString SampleMeshDecoratePlugin::decorationInfo(FilterIDType id) const
{
  switch(id)
	{
	case DP_SHOW_CUBEMAPPED_ENV :      return tr("Draws a customizable cube mapped background that is sync with trackball rotation");
  case DP_SHOW_GRID :      return tr("Draws a gridded background that can be used as a reference.");
	}
	assert(0);
	return QString();
}
QString SampleMeshDecoratePlugin::decorationName(FilterIDType id) const
{
	switch(id)
	{
	case DP_SHOW_CUBEMAPPED_ENV :      return tr("Cube mapped background");
  case DP_SHOW_GRID :      return tr("Background Grid");
	}
	assert(0);
	return QString();
}

void SampleMeshDecoratePlugin::initGlobalParameterSet(QAction *action, RichParameterSet &parset)
{
  switch(ID(action)){
  case DP_SHOW_CUBEMAPPED_ENV :
  if(!parset.hasParameter(CubeMapPathParam()))
      {
    QString cubemapDirPath = PluginManager::getBaseDirPath() + QString("/textures/cubemaps/uffizi.jpg");
    parset.addParam(new RichString(CubeMapPathParam(), cubemapDirPath,"",""));
  }
  break;
  case DP_SHOW_GRID :
      parset.addParam(new RichFloat(BoxRatioParam(),2.0,"Box Ratio","The size of the grid around the object w.r.t. the bbox of the object"));
      parset.addParam(new RichFloat(GridMajorParam(),1,"Major Spacing",""));
      parset.addParam(new RichFloat(GridMinorParam(),0.5,"Minor Spacing","Grid Size"));
    break;
  }
}		
		
bool SampleMeshDecoratePlugin::startDecorate( QAction * action, MeshDocument &/*m*/, RichParameterSet * parset, GLArea * gla)
{
  switch(ID(action)){
    case DP_SHOW_CUBEMAPPED_ENV :
    if(parset->findParameter(CubeMapPathParam())== NULL) qDebug("CubeMapPath was not setted!!!");
      cubemapFileName = parset->getString(CubeMapPathParam());
    break;
  case DP_SHOW_GRID:
    connect(gla,SIGNAL(transmitShot(QString,vcg::Shotf)),this,SLOT(setValue(QString,vcg::Shotf)));
    connect(this,SIGNAL(askViewerShot(QString)),gla,SLOT(sendViewerShot(QString)));
  break;
  }
	return true;
}

void SampleMeshDecoratePlugin::decorate(QAction *a, MeshDocument &m, RichParameterSet * parset,GLArea *gla, QPainter * )
{
  static QString lastname("unitialized");
	switch(ID(a))
	{
	  case DP_SHOW_CUBEMAPPED_ENV :
		{
      if(!cm.IsValid() || (lastname != cubemapFileName ) )
      {
        qDebug( "Current CubeMapPath Dir: %s ",qPrintable(cubemapFileName));
        glewInit();
        bool ret = cm.Load(qPrintable(cubemapFileName));
        lastname=cubemapFileName;
        if(! ret ) return;
        //QMessageBox::warning(gla,"Cubemapped background decoration","Warning unable to load cube map images: " + cubemapFileName );
        cm.radius=10;
      }
			if(!cm.IsValid()) return;

      Matrix44f tr;
			glGetv(GL_MODELVIEW_MATRIX,tr);			
			// Remove the translation from the current matrix by simply padding the last column of the matrix
      tr.SetColumn(3,Point4f(0,0,0,1.0));
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
    } break;
  case DP_SHOW_GRID :
    {
      emit this->askViewerShot("me");
      Box3f bb=m.bbox();
      float scaleBB = parset->getFloat(BoxRatioParam());
      bb.Offset((bb.max-bb.min)*(scaleBB-1.0));
        DrawGriddedCube(bb,10,1,gla);
    } break;
  }
}

void DrawGridPlane(int axis, int side,
                   Point3f minP, Point3f maxP,
                   Point3f minG, Point3f maxG, // the snapped position of the grids.
                   float majorTick, float minorTick)
{
  int xAxis = (1+axis)%3;
  int yAxis = (2+axis)%3;
  int zAxis = (0+axis)%3; // the axis perpendicular to the plane we want to draw (e.g. if i want to draw plane xy I wrote '2')

  Point3f p1,p2;

  p1[zAxis]=p2[zAxis] = side ? maxP[zAxis] : minP[zAxis] ;
  glLineWidth(0.5f);
  glBegin(GL_LINES);
  // first draw the vertical lines
  p1[yAxis]=minP[yAxis];
  p2[yAxis]=maxP[yAxis];
  for(float alpha=minG[xAxis];alpha<maxP[xAxis];alpha+=majorTick)
    {
      p1[xAxis]=alpha;
      p2[xAxis]=alpha;
      glVertex(p1);
      glVertex(p2);
    }

    // then the horizontal lines
  p1[xAxis]=minP[xAxis];
  p2[xAxis]=maxP[xAxis];
  for(float alpha=minG[yAxis];alpha<maxP[yAxis];alpha+=majorTick)
    {
      p1[yAxis]=alpha;
      p2[yAxis]=alpha;
      glVertex(p1);
      glVertex(p2);
    }
  glEnd();

  glLineWidth(1.0f);
  glBegin(GL_LINES);
  if(minP[xAxis]*maxP[xAxis] <0 )
  {
    p1[yAxis]=minP[yAxis];
    p2[yAxis]=maxP[yAxis];
    p1[xAxis]=p2[xAxis]=0;
    glVertex(p1);
    glVertex(p2);
  }
  if(minP[yAxis]*maxP[yAxis] <0 )
  {
    p1[xAxis]=minP[xAxis];
    p2[xAxis]=maxP[xAxis];
    p1[yAxis]=p2[yAxis]=0;
    glVertex(p1);
    glVertex(p2);
  }
  glEnd();

}
/* return true if the side of a box is front facing with respet of the give viewpoint.
   side 0, axis i == min on than i-th axis
   side 1, axis i == min on than i-th axis
   questo capita se il prodotto scalare tra il vettore normale entro della faccia
 */
bool FrontFacing(Point3f viewPos,
                 int axis, int side,
                 Point3f minP, Point3f maxP)
{
  assert (side==0 || side ==1);
  assert (axis>=0 && axis < 3);
  Point3f N(0,0,0);
  Point3f C = (minP+maxP)/2.0;

  if(side == 1) {
      C[axis] = maxP[axis];
      N[axis]=-1;
  }

  if(side == 0) {
    C[axis] = minP[axis];
    N[axis]=1;
  }
  Point3f vpc = viewPos-C;
//  qDebug("FaceCenter %f %f %f - %f %f %f",C[0],C[1],C[2],N[0],N[1],N[2]);
//  qDebug("VPC        %f %f %f",vpc[0],vpc[1],vpc[2]);
  return vpc*N > 0;
}

void SampleMeshDecoratePlugin::DrawGriddedCube(const Box3f &bb, float majorTick, float minorTick, GLArea *gla)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    Point3f minP,maxP, minG,maxG;
    minP=bb.min;maxP=bb.max;

    // Make the box well rounded wrt to major tick
    for(int i=0;i<3;++i) // foreach axis
    {
        if(minP[i] > 0 ) minG[i] = minP[i] - fmod(minP[i],majorTick) + majorTick;
        if(minP[i] ==0 ) minG[i] = majorTick;
        if(minP[i] < 0 ) minG[i] = minP[i] + fmod(fabs(minP[i]),majorTick);
    }

    glDisable(GL_LIGHTING);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    Point3f viewPos = this->curShot.GetViewPoint();
    qDebug("Current camera pos %f %f %f",viewPos[0],viewPos[1],viewPos[2]);
    for (int ii=0;ii<3;++ii)
        for(int jj=0;jj<2;++jj)
            if(!FrontFacing(viewPos,ii,jj,minP,maxP))
                DrawGridPlane(ii,jj,minP,maxP,minG,maxG,10,1);

    glPopAttrib();
}

void  SampleMeshDecoratePlugin::setValue(QString name, vcg::Shotf val) {curShot=val;}

Q_EXPORT_PLUGIN(SampleMeshDecoratePlugin)
