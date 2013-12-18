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
