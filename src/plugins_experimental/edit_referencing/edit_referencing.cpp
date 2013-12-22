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
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "edit_referencing.h"
#include <wrap/gl/pick.h>
#include <wrap/qt/gl_label.h>

using namespace std;
using namespace vcg;

EditReferencingPlugin::EditReferencingPlugin() {
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(12);    
}

const QString EditReferencingPlugin::Info()
{
    return tr("Reference layers using fiducial points.");
}
 
void EditReferencingPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
    gla->update();
    cur=event->pos();
    haveToPick=true;

    // DEBUG DEBUG

    vector<vcg::Point3d> FixP;
    vector<vcg::Point3d> MovP;
    Matrix44d Mtrasf;
    vcg::Point3d newp;

    FixP.clear();
    newp = Point3d(-25.19000, 89.52376, 0.0);
    FixP.push_back(newp);
    newp = Point3d(35.97340, 21.47749, 1.0);
    FixP.push_back(newp);
    newp = Point3d(0.00000, 0.00000, 5.0);
    FixP.push_back(newp);
    newp = Point3d(-58.22760, 68.19291, 2.0);
    FixP.push_back(newp);
    newp = Point3d(-24.87419, 29.90908, 1.0);
    FixP.push_back(newp);
    newp = Point3d(8.98064, 46.20274, 2.0);
    FixP.push_back(newp);
    newp = Point3d(-34.08825, 59.46381, -3.0);
    FixP.push_back(newp);
    newp = Point3d(14.83746, 28.60058, -1.0);
    FixP.push_back(newp);

    newp = Point3d(38.97, -45.4012, 0.0);
    MovP.push_back(newp);
    newp = Point3d(-52.9811, -39.7094, 1.0);
    MovP.push_back(newp);
    newp = Point3d(-42.0136, 0.259092, 5.0);
    MovP.push_back(newp);
    newp = Point3d(47.047, -7.04106, 2.0);
    MovP.push_back(newp);
    newp = Point3d(-3.1936, -3.69573, 1.0);
    MovP.push_back(newp);
    newp = Point3d(-16.494, -38.3969, 2.0);
    MovP.push_back(newp);
    newp = Point3d(23.9654, -17.6361, -3.0);
    MovP.push_back(newp);
    newp = Point3d(-32.6295, -29.9274, -1.0);
    MovP.push_back(newp);

    ComputeRigidMatchMatrix(FixP, MovP, Mtrasf);

    this->Log(GLLogStream::FILTER, "MATRIX:");
    this->Log(GLLogStream::FILTER, "%f, %f, %f, %f",Mtrasf[0][0],Mtrasf[0][1],Mtrasf[0][2],Mtrasf[0][3]);
    this->Log(GLLogStream::FILTER, "%f, %f, %f, %f",Mtrasf[1][0],Mtrasf[1][1],Mtrasf[1][2],Mtrasf[1][3]);
    this->Log(GLLogStream::FILTER, "%f, %f, %f, %f",Mtrasf[2][0],Mtrasf[2][1],Mtrasf[2][2],Mtrasf[2][3]);
    this->Log(GLLogStream::FILTER, "%f, %f, %f, %f",Mtrasf[3][0],Mtrasf[3][1],Mtrasf[3][2],Mtrasf[3][3]);

    this->Log(GLLogStream::FILTER, "  ");
    this->Log(GLLogStream::FILTER, "errors:");

    float TrError=0;
    for(int Pind=0; Pind<MovP.size(); Pind++)
    {
      TrError = (FixP[Pind] - (Mtrasf * MovP[Pind])).Norm();
      this->Log(GLLogStream::FILTER, "%d: %f",Pind,TrError);
    }

    //this->Log(GLLogStream::FILTER, "Distance: %f",Distance(a,b));
}
  
void EditReferencingPlugin::Decorate(MeshModel &m, GLArea * gla, QPainter *p)
{
    if(haveToPick)
    {
        glPushMatrix();
        glMultMatrix(m.cm.Tr);
        vector<CMeshO::FacePointer> NewSel;
        GLPickTri<CMeshO>::PickFace(cur.x(), gla->height() - cur.y(), m.cm, NewSel);
        if(NewSel.size()>0)
            curFacePtr=NewSel.front();
        haveToPick=false;
        glPopMatrix();
	}   
 if(curFacePtr)
 {
    glPushMatrix();
    glMultMatrix(m.cm.Tr);
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
    glLineWidth(2.0f);
    glDepthFunc(GL_ALWAYS);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
	
    glDisable(GL_LIGHTING);
    glColor(Color4b::DarkRed);
    glBegin(GL_LINE_LOOP);
        glVertex(curFacePtr->P(0));
        glVertex(curFacePtr->P(1));
        glVertex(curFacePtr->P(2));
    glEnd();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glColor(Color4b::Red);
    glBegin(GL_LINE_LOOP);
        glVertex(curFacePtr->P(0));
        glVertex(curFacePtr->P(1));
        glVertex(curFacePtr->P(2));
    glEnd();

    drawFace(curFacePtr,m,gla,p);

    glPopAttrib();
    glPopMatrix();
 }
}

void EditReferencingPlugin::drawFace(CMeshO::FacePointer fp, MeshModel &m, GLArea *gla, QPainter *p)
{
  QString buf = QString("f%1\n (%3 %4 %5)").arg(tri::Index(m.cm,fp)).arg(tri::Index(m.cm,fp->V(0))).arg(tri::Index(m.cm,fp->V(1))).arg(tri::Index(m.cm,fp->V(2)));
  Point3f c=Barycenter(*fp);
  vcg::glLabel::render(p,c,buf);
  for(int i=0;i<3;++i)
    {
       buf =QString("\nv%1:%2 (%3 %4 %5)").arg(i).arg(fp->V(i) - &m.cm.vert[0]).arg(fp->P(i)[0]).arg(fp->P(i)[1]).arg(fp->P(i)[2]);
      if( m.hasDataMask(MeshModel::MM_VERTQUALITY) )
        buf +=QString(" - Q(%1)").arg(fp->V(i)->Q());
      if( m.hasDataMask(MeshModel::MM_WEDGTEXCOORD) )
          buf +=QString("- uv(%1 %2) id:%3").arg(fp->WT(i).U()).arg(fp->WT(i).V()).arg(fp->WT(i).N());
      if( m.hasDataMask(MeshModel::MM_VERTTEXCOORD) )
          buf +=QString("- uv(%1 %2) id:%3").arg(fp->V(i)->T().U()).arg(fp->V(i)->T().V()).arg(fp->V(i)->T().N());
    vcg::glLabel::render(p,fp->V(i)->P(),buf);
    }
}

bool EditReferencingPlugin::StartEdit(MeshModel &/*m*/, GLArea *gla )
{
    curFacePtr=0;
    gla->setCursor(QCursor(QPixmap(":/images/cur_referencing.png"),1,1));
    return true;
}
