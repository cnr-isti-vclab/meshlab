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
#include <limits>
#include <stdlib.h>
#include "decorate_background.h"
#include <wrap/gl/addons.h>

#include <meshlab/glarea.h>
#include <common/pluginmanager.h>


using namespace vcg;

QString DecorateBackgroundPlugin::decorationName(FilterIDType id) const
{
    switch (id)
    {
    case DP_SHOW_CUBEMAPPED_ENV:  return tr("Cube mapped background");
    case DP_SHOW_GRID:            return tr("Background Grid");
    }
    assert(0);
    return QString();
}

QString DecorateBackgroundPlugin::decorationInfo(FilterIDType id) const
{
    switch(id)
    {
    case DP_SHOW_CUBEMAPPED_ENV :  return tr("Draws a customizable cube mapped background that is sync with trackball rotation");
    case DP_SHOW_GRID :            return tr("Draws a gridded background that can be used as a reference.");
    }
    assert(0);
    return QString();
}

void DecorateBackgroundPlugin::initGlobalParameterSet(QAction *action, RichParameterSet &parset)
{
    switch(ID(action))
    {
    case DP_SHOW_CUBEMAPPED_ENV :
        if(!parset.hasParameter(CubeMapPathParam()))
        {
            QString cubemapDirPath = PluginManager::getBaseDirPath() + QString("/textures/cubemaps/uffizi.jpg");
            //parset.addParam(new RichString(CubeMapPathParam(), cubemapDirPath,"",""));
        }
        break;
    case DP_SHOW_GRID :
        parset.addParam(new RichFloat(BoxRatioParam(),1.2f,"Box Ratio","The size of the grid around the object w.r.t. the bbox of the object"));
        parset.addParam(new RichFloat(GridMajorParam(),10.0f,"Major Spacing",""));
        parset.addParam(new RichFloat(GridMinorParam(),1.0f,"Minor Spacing",""));
        parset.addParam(new RichBool(GridBackParam(),true,"Front grid culling",""));
        parset.addParam(new RichBool(ShowShadowParam(),false,"Show silhouette",""));
        parset.addParam(new RichColor(GridColorBackParam(), QColor(163,116,35,255), "Back Grid Color", ""));
        parset.addParam(new RichColor(GridColorFrontParam(),QColor(22,139,119,255),"Front grid Color",""));
        break;
    }
}

bool DecorateBackgroundPlugin::startDecorate( QAction * action, MeshDocument &/*m*/, RichParameterSet * parset, GLArea * gla)
{
    GLenum res = glewInit();
    if (res != GLEW_OK)
        return false;
    switch(ID(action))
    {
    case DP_SHOW_CUBEMAPPED_ENV :
        if(parset->findParameter(CubeMapPathParam())== NULL) qDebug("CubeMapPath was not setted!!!");
        cubemapFileName = parset->getString(CubeMapPathParam());
        break;
    case DP_SHOW_GRID:
        /*QMetaObject::Connection aaa =*/ connect(gla, SIGNAL(transmitShot(QString, Shotm)), this, SLOT(setValue(QString, Shotm)));
        /*QMetaObject::Connection bbb =*/ connect(this, SIGNAL(askViewerShot(QString)), gla, SLOT(sendViewerShot(QString)));
        break;
    }
    return true;
}

void DecorateBackgroundPlugin::decorateDoc(QAction *a, MeshDocument &m, RichParameterSet * parset,GLArea * gla, QPainter *, GLLogStream &)
{
    static QString lastname("unitialized");
    switch(ID(a))
    {
    case DP_SHOW_CUBEMAPPED_ENV :
        {
            if(!cm.IsValid() || (lastname != cubemapFileName ) )
            {
                qDebug( "Current CubeMapPath Dir: %s ",qUtf8Printable(cubemapFileName));
                glewInit();
                bool ret = cm.Load(qUtf8Printable(cubemapFileName));
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
        } 
        break;
    case DP_SHOW_GRID :
        {
            emit this->askViewerShot("backGrid");

            float scaleBB = parset->getFloat(BoxRatioParam());
            float majorTick = fabs(parset->getFloat(GridMajorParam()));
            float minorTick = fabs(parset->getFloat(GridMinorParam()));
            bool backFlag = parset->getBool(GridBackParam());
            bool shadowFlag = parset->getBool(ShowShadowParam());
            Color4b backColor = parset->getColor4b(GridColorBackParam());
            Color4b frontColor = parset->getColor4b(GridColorFrontParam());
            Box3m bb = m.bbox();
            float scalefactor = std::max(0.1, (scaleBB - 1.0));
            bb.Offset((bb.max - bb.min)*(scalefactor/2.0));

			// minortick should never be more than majortick
			if (minorTick > majorTick)
				minorTick = majorTick;

			// check if user asked for a grid that is too dense
			// if more than 100-200k ticks, the rendering will slow down too much and crash on some drivers
			int ticks = ((bb.Dim()[0] + bb.Dim()[1] + bb.Dim()[2]) / minorTick) + ((bb.Dim()[0] + bb.Dim()[1] + bb.Dim()[2]) / majorTick);
			if (ticks > 200000)
			{
				if (log10(bb.Diag()) > 0.0)
					majorTick = pow(10, floor(log10(bb.Diag()) -1));
				else
					majorTick = pow(10, floor(log10(bb.Diag()) + 1));
				minorTick = majorTick / 2.0;
			}

            MLSceneGLSharedDataContext* shared = NULL;
            QGLContext* cont = NULL;
            if ((gla != NULL) && (gla->mvc() != NULL))
            {
                cont = gla->context();
                shared = gla->mvc()->sharedDataContext();
            }

            DrawGriddedCube(shared,cont,*m.mm(), bb, majorTick, minorTick, backFlag, shadowFlag, backColor, frontColor);
        } 
        break;
    }
}


// Note minG/maxG is wider than minP/maxP.
void DrawGridPlane(int axis,
    int side, // 0 is min 1 is max
    Point3m minP, Point3m maxP,
    Point3m minG, Point3m maxG, // the box with vertex positions snapped to the grid (enlarging it).
    float majorTick, float minorTick,
    Color4b lineColor)
{
    int xAxis = (1+axis)%3;
    int yAxis = (2+axis)%3;
    int zAxis = (0+axis)%3; // the axis perpendicular to the plane we want to draw (e.g. if i want to draw plane xy I write '2')

    Color4b majorColor=lineColor;
    Color4b minorColor;
    Color4b axisColor;
    minorColor[0] = std::min(255.0, lineColor[0] * 2.0); // minorcolor is 2 times brighter and half solid w.r.t. majorcolor
    minorColor[1] = std::min(255.0, lineColor[1] * 2.0);
    minorColor[2] = std::min(255.0, lineColor[2] * 2.0);
    minorColor[3] = std::min(127.0, lineColor[3] / 2.0);
    axisColor[0] = lineColor[0] * 0.66; // axiscolor is 2/3 darker w.r.t. majorcolor
    axisColor[1] = lineColor[1] * 0.66;
    axisColor[2] = lineColor[2] * 0.66;
    axisColor[3] = 255;

    // We draw orizontal and vertical lines onto the XY plane snapped with the major ticks
    Point3m p1,p2,p3,p4;
    p1[zAxis] = p2[zAxis] = p3[zAxis] = p4[zAxis] = side ? maxG[zAxis] : minG[zAxis];

    float aMin,aMax,bMin,bMax;

    p1[yAxis] = minG[yAxis];
    p2[yAxis] = maxG[yAxis];
    p3[xAxis] = minG[xAxis];
    p4[xAxis] = maxG[xAxis];
    aMin = minG[xAxis];
    aMax = maxG[xAxis];
    bMin = minG[yAxis];
    bMax = maxG[yAxis];

    glLineWidth(0.5f);
    glColor(minorColor);
    glBegin(GL_LINES);
    for (float alpha = aMin; alpha <= aMax; alpha += minorTick)
    {
        p1[xAxis] = p2[xAxis] = alpha;
        glVertex(p1); glVertex(p2);
    }
    for (float alpha = bMin; alpha <= bMax; alpha += minorTick)
    {
        p3[yAxis] = p4[yAxis] = alpha;
        glVertex(p3); glVertex(p4);
    }
    glEnd();

    glLineWidth(1.0f);
    glColor(majorColor);
    glBegin(GL_LINES);
    for (float alpha = aMin; alpha <= aMax; alpha += majorTick)
    {
        p1[xAxis] = p2[xAxis] = alpha;
        glVertex(p1); glVertex(p2);
    }
    for (float alpha = bMin; alpha <= bMax; alpha += majorTick)
    {
        p3[yAxis] = p4[yAxis] = alpha;
        glVertex(p3); glVertex(p4);
    }
    glEnd();

    // Draw the axis
    glColor(axisColor);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    if(minP[xAxis]*maxP[xAxis] <0 )
    {
        p1[yAxis]=minG[yAxis];
        p2[yAxis]=maxG[yAxis];
        p1[xAxis]=p2[xAxis]=0;
        glVertex(p1);
        glVertex(p2);
    }
    if(minP[yAxis]*maxP[yAxis] <0 )
    {
        p1[xAxis]=minG[xAxis];
        p2[xAxis]=maxG[xAxis];
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
bool FrontFacing(Point3m viewPos,
    int axis,
    int side,
    Point3m minP, 
    Point3m maxP)
{
    assert (side==0 || side ==1);
    assert (axis>=0 && axis < 3);
    Point3m N(0,0,0);
    Point3m C = (minP+maxP)/2.0;

    if(side == 1) {
        C[axis] = maxP[axis];
        N[axis]=-1;
    }

    if(side == 0) {
        C[axis] = minP[axis];
        N[axis]=1;
    }
    Point3m vpc = viewPos-C;
    //  qDebug("FaceCenter %f %f %f - %f %f %f",C[0],C[1],C[2],N[0],N[1],N[2]);
    //  qDebug("VPC        %f %f %f",vpc[0],vpc[1],vpc[2]);
    return vpc*N > 0;
}

void DrawFlatMesh(MLSceneGLSharedDataContext* shared,QGLContext* cont,MeshModel &m, int axis, int side, Point3m minG, Point3m maxG)
{
    if ((shared == NULL) || (cont == NULL))
        return;
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    Point3m trans = side?maxG:minG;
    Point3m scale(1.0f,1.0f,1.0);
    trans[(axis+1)%3]=0;
    trans[(axis+2)%3]=0;
    scale[axis]=0;
    glTranslate(trans);
    glScale(scale);
        shared->draw(m.id(),cont);
    //    m.render(GLW::DMFlat,GLW::CMNone,GLW::TMNone);
    glPopMatrix();
    glPopAttrib();
}

void DecorateBackgroundPlugin::DrawGriddedCube(MLSceneGLSharedDataContext* shared,QGLContext* cont,MeshModel &m, const Box3m &bb, Scalarm majorTick, Scalarm minorTick, bool backCullFlag, bool shadowFlag, Color4b frontColor, Color4b backColor)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    Point3m minP,maxP, minG,maxG;
    minP=bb.min;maxP=bb.max;

    // Make the box well rounded wrt to major tick (only increase)
    for(int i=0;i<3;++i) // foreach axis
    {
        if (minP[i] == 0) 
            minG[i] = -majorTick;
        else
            minG[i] = minP[i] + fmod(fabs(minP[i]), majorTick) - majorTick;

        if (maxP[i] == 0) 
            maxG[i] =  majorTick;
        else
            maxG[i] = maxP[i] - fmod(fabs(maxP[i]), majorTick) + majorTick;
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glColor3f(0.8f,0.8f,0.8f);
    glEnable(GL_LINE_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    Point3m viewPos = Point3m::Construct(this->curShot.GetViewPoint());
    //    qDebug("BG Grid MajorTick %4.2f MinorTick %4.2f  ## camera pos %7.3f %7.3f %7.3f", majorTick, minorTick, viewPos[0],viewPos[1],viewPos[2]);
    //    qDebug("BG Grid boxF %7.3f %7.3f %7.3f # %7.3f %7.3f %7.3f",minP[0],minP[1],minP[2],maxP[0],maxP[1],maxP[2]);
    //    qDebug("BG Grid boxG %7.3f %7.3f %7.3f # %7.3f %7.3f %7.3f",minG[0],minG[1],minG[2],maxG[0],maxG[1],maxG[2]);
    for (int ii=0;ii<3;++ii)
        for(int jj=0;jj<2;++jj)
        {
            bool front = FrontFacing(viewPos,ii,jj,minP,maxP);
            if( front || !backCullFlag)
            {
                DrawGridPlane(ii,jj,minP,maxP,minG,maxG,majorTick,minorTick,front?frontColor:backColor);
                if(shadowFlag) 
                {
                    glPushAttrib(GL_COLOR_BUFFER_BIT);
                    glBlendColor(1.0f,1.0f,1.0f,0.4f);
                    glBlendFunc(GL_CONSTANT_COLOR, GL_ONE);
                    DrawFlatMesh(shared,cont,m,ii,jj,minG,maxG);
                    glPopAttrib();
                }
            }
        }

        glDisable(GL_BLEND);
        glPopAttrib();
}

void  DecorateBackgroundPlugin::setValue(QString name, Shotm val) 
{
    curShot=val;
}

MESHLAB_PLUGIN_NAME_EXPORTER(DecorateBackgroundPlugin)
