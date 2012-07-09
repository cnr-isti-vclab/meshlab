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
    //parset.addParam(new RichString(CubeMapPathParam(), cubemapDirPath,"",""));
  }
  break;
  case DP_SHOW_GRID :
      parset.addParam(new RichFloat(BoxRatioParam(),2.0,"Box Ratio","The size of the grid around the object w.r.t. the bbox of the object"));
      parset.addParam(new RichFloat(GridMajorParam(),10,"Major Spacing",""));
      parset.addParam(new RichFloat(GridMinorParam(),1,"Minor Spacing",""));
      parset.addParam(new RichBool(GridSnapParam(),false,"Grid Snapping",""));
      parset.addParam(new RichBool(GridBackParam(),false,"Front grid culling",""));
      parset.addParam(new RichBool(ShowShadowParam(),false,"Show silhouette",""));
      parset.addParam(new RichColor(GridColorBackParam(),QColor(Color4b::Gray),"Back Grid Color",""));
      parset.addParam(new RichColor(GridColorFrontParam(),QColor(Color4b::Gray),"Front grid Color",""));

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
      float majorTick = parset->getFloat(GridMajorParam());
      float minorTick = parset->getFloat(GridMinorParam());
      bool gridSnap = parset->getBool(GridSnapParam());
      bool backFlag = parset->getBool(GridBackParam());
      bool shadowFlag = parset->getBool(ShowShadowParam());
      Color4b backColor = parset->getColor4b(GridColorBackParam());
      Color4b frontColor = parset->getColor4b(GridColorFrontParam());
      bb.Offset((bb.max-bb.min)*(scaleBB-1.0));
      DrawGriddedCube(*m.mm(),bb,majorTick,minorTick,gridSnap,backFlag,shadowFlag,backColor,frontColor,gla);
    } break;
  }
}

void DrawGridPlane(int axis,
                   int side, // 0 is min 1 is max
                   Point3f minP, Point3f maxP,
                   Point3f minG, Point3f maxG, // the box with vertex positions snapped to the grid (enlarging it).
                   float majorTick, float minorTick,
                   bool snappedBorder,
                   Color4b lineColor)
{
  int xAxis = (1+axis)%3;
  int yAxis = (2+axis)%3;
  int zAxis = (0+axis)%3; // the axis perpendicular to the plane we want to draw (e.g. if i want to draw plane xy I wrote '2')

  Color4b majorColor=lineColor;
  Color4b minorColor=lineColor;
  minorColor[3]=127;

  // We draw orizontal ande vertical lines onto the XY plane snapped with the major ticks
  Point3f p1,p2;
  if(snappedBorder)
      p1[zAxis]=p2[zAxis] = side ? maxG[zAxis] : minG[zAxis] ;
  else
      p1[zAxis]=p2[zAxis] = side ? maxP[zAxis] : minP[zAxis] ;

  float aMin,aMax;
  // first draw the vertical lines (e.g we iterate on the X with fixed Y extents)
  glBegin(GL_LINES);
  if(snappedBorder) {
      p1[yAxis]=minG[yAxis];
      p2[yAxis]=maxG[yAxis];
      aMin =minG[xAxis];
      aMax =maxG[xAxis];
  } else {
      p1[yAxis]=minP[yAxis];
      p2[yAxis]=maxP[yAxis];
      aMin =minG[xAxis]+majorTick;
      aMax =maxP[xAxis];
  }
  for(float alpha=aMin;alpha<=aMax;alpha+=majorTick)
  {
      p1[xAxis]=p2[xAxis]=alpha;
      glColor(majorColor);
      glVertex(p1); glVertex(p2);
      glColor(minorColor);
      for(float alpha2 = alpha+minorTick; alpha2<alpha+majorTick && alpha2<=aMax; alpha2+=minorTick) {
          p1[xAxis]=p2[xAxis]=alpha2;
          glVertex(p1); glVertex(p2);
      }
  }

    // then the horizontal lines  (e.g we iterate on the Y with fixed X extents)
  if(snappedBorder) {
      p1[xAxis]=minG[xAxis];
      p2[xAxis]=maxG[xAxis];
      aMin = minG[yAxis];
      aMax = maxG[yAxis];
  } else {
      p1[xAxis]=minP[xAxis];
      p2[xAxis]=maxP[xAxis];
      aMin = minG[yAxis]+majorTick;
      aMax = maxP[yAxis];
  }

  for(float alpha=aMin;alpha<=aMax;alpha+=majorTick)
    {
      p1[yAxis]=p2[yAxis]=alpha;
      glColor(majorColor);
      glVertex(p1); glVertex(p2);
      glColor(minorColor);
      for(float alpha2 = alpha+minorTick; alpha2<alpha+majorTick && alpha2<=aMax ;alpha2+=minorTick) {
          p1[yAxis]=p2[yAxis]=alpha2;
          glVertex(p1); glVertex(p2);
      }
    }

  glEnd();
  glColor(majorColor);

  // Draw the axis
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
void DrawFlatMesh(MeshModel &m, int axis, int side,
                                            Point3f minG, Point3f maxG)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    Point3f trans = side?maxG:minG;
    Point3f scale(1.0f,1.0f,1.0);
    trans[(axis+1)%3]=0;
    trans[(axis+2)%3]=0;
    scale[axis]=0;
    glTranslate(trans);
    glScale(scale);
    m.render(GLW::DMFlat,GLW::CMNone,GLW::TMNone);
    glPopMatrix();
    glPopAttrib();
}

void SampleMeshDecoratePlugin::DrawGriddedCube(MeshModel &m, const Box3f &bb, float majorTick, float minorTick, bool snapFlag, bool backCullFlag, bool shadowFlag, Color4b frontColor, Color4b backColor, GLArea *gla)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    Point3f minP,maxP, minG,maxG;
    minP=bb.min;maxP=bb.max;

    // Make the box well rounded wrt to major tick
    for(int i=0;i<3;++i) // foreach axis
    {
        if(minP[i] > 0 ) minG[i] = minP[i] - fmod(minP[i],majorTick) - majorTick;
        if(minP[i] ==0 ) minG[i] = majorTick;
        if(minP[i] < 0 ) minG[i] = minP[i] + fmod(fabs(minP[i]),majorTick) - majorTick;
        if(maxP[i] > 0 ) maxG[i] = maxP[i] - fmod(maxP[i],majorTick) + majorTick;
        if(maxP[i] ==0 ) maxG[i] = majorTick;
        if(maxP[i] < 0 ) maxG[i] = maxP[i] + fmod(fabs(maxP[i]),majorTick);
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glColor3f(0.8f,0.8f,0.8f);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    Point3f viewPos = this->curShot.GetViewPoint();
//    qDebug("BG Grid MajorTick %4.2f MinorTick %4.2f  ## camera pos %7.3f %7.3f %7.3f", majorTick, minorTick, viewPos[0],viewPos[1],viewPos[2]);
//    qDebug("BG Grid boxF %7.3f %7.3f %7.3f # %7.3f %7.3f %7.3f",minP[0],minP[1],minP[2],maxP[0],maxP[1],maxP[2]);
//    qDebug("BG Grid boxG %7.3f %7.3f %7.3f # %7.3f %7.3f %7.3f",minG[0],minG[1],minG[2],maxG[0],maxG[1],maxG[2]);
    for (int ii=0;ii<3;++ii)
        for(int jj=0;jj<2;++jj)
        {
            bool front = FrontFacing(viewPos,ii,jj,minP,maxP);
            if( front || !backCullFlag)
            {
                DrawGridPlane(ii,jj,minP,maxP,minG,maxG,majorTick,minorTick,snapFlag,front?frontColor:backColor);
                if(shadowFlag) DrawFlatMesh(m, ii,jj,minG,maxG);
            }
        }

    glPopAttrib();
}

void  SampleMeshDecoratePlugin::setValue(QString name, vcg::Shotf val) {curShot=val;}

Q_EXPORT_PLUGIN(SampleMeshDecoratePlugin)
