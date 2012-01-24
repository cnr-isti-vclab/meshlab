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
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "edit_point.h"
#include <wrap/gl/pick.h>
#include <wrap/qt/gl_label.h>

#include "connectedComponent.h"


using namespace std;
using namespace vcg;

EditPointPlugin::EditPointPlugin() {}

const QString EditPointPlugin::Info() {
    return tr("Select a region of the mesh");
}
  
void EditPointPlugin::Decorate(MeshModel &m, GLArea * gla, QPainter *p)
{
    if(haveToPick)
    {
        glPushMatrix();
        glMultMatrix(m.cm.Tr);
        vector<CMeshO::VertexPointer> NewSel;
        GLPickTri<CMeshO>::PickVert(cur.x(), gla->height() - cur.y(), m.cm, NewSel);
        if(NewSel.size() > 0) {
            startingVertex = NewSel.front();

            ComponentFinder<CMeshO, CVertexO>::Dijkstra(m.cm, *startingVertex, 6, this->maxHop, this->NotReachableVector);
            ComponentVector.push_back(startingVertex);
        }

        haveToPick = false;
        glPopMatrix();
    }

    if (startingVertex != NULL) {
        glPushMatrix();
        glMultMatrix(m.cm.Tr);

        glPushAttrib(GL_ENABLE_BIT );
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glDepthRange (0.0, 0.9999);
        glDepthFunc(GL_LEQUAL);
        glPointSize(6.f);

        glBegin(GL_POINTS);
        glColor4f(1,0,0,.6f);

        for(vector<CMeshO::VertexPointer>::iterator vi = ComponentVector.begin(); vi != ComponentVector.end(); ++vi)
        {
            glVertex((*vi)->cP());
        }
        glEnd();

        glBegin(GL_POINTS);
        glColor4f(1,1,0,.6f);

        for(vector<CMeshO::VertexPointer>::iterator vi = BorderVector.begin(); vi != BorderVector.end(); ++vi)
        {
            glVertex((*vi)->cP());
        }

        glEnd();
        glPopAttrib();
        glPopMatrix();
    }
}

bool EditPointPlugin::StartEdit(MeshModel &m, GLArea *gla ) {
    BorderVector = std::vector<CVertexO*>();
    NotReachableVector = std::vector<CVertexO*>();
    this->maxHop = m.cm.bbox.Diag() / 100.0;

    return true;
}

void EditPointPlugin::EndEdit(MeshModel &m, GLArea *gla) {
    ComponentFinder<CMeshO, CVertexO>::DeletePerVertexAttribute(m.cm);
}


void EditPointPlugin::mousePressEvent(QMouseEvent *ev, MeshModel &m, GLArea *gla ) {
    startingVertex = NULL;

    cur = ev->pos();
    haveToPick = true;

    this->isMousePressed = true;
    this->dist = 0.0;
    this->startingClick = vcg::Point2f(ev->x(), ev->y());

    BorderVector.clear();
    ComponentVector.clear();
}

void EditPointPlugin::mouseMoveEvent(QMouseEvent *ev, MeshModel &m, GLArea *gla ) {
    if (this->isMousePressed && startingVertex != NULL) {
        float distFromCenter = math::Sqrt((startingClick.X() - ev->x())*(startingClick.X() - ev->x()) + (startingClick.Y() - ev->y()) * (startingClick.Y() - ev->y()));

        float perim = 2 * m.cm.bbox.DimX() + 2 * m.cm.bbox.DimY();
        perim = (perim < 2 * m.cm.bbox.DimY() + 2 * m.cm.bbox.DimZ()) ? 2 * m.cm.bbox.DimY() + 2 * m.cm.bbox.DimZ() : perim;
        perim = (perim < 2 * m.cm.bbox.DimX() + 2 * m.cm.bbox.DimZ()) ? 2 * m.cm.bbox.DimX() + 2 * m.cm.bbox.DimZ() : perim;

        this->dist = (perim * distFromCenter) / gla->width();

        BorderVector.clear();
        ComponentVector = ComponentFinder<CMeshO, CVertexO>::FindComponent(m.cm, *startingVertex, 6, this->dist, this->maxHop, BorderVector, NotReachableVector);
    }    
}

void EditPointPlugin::mouseReleaseEvent(QMouseEvent *ev, MeshModel &m, GLArea *gla) {
    gla->update();
}

void EditPointPlugin::keyPressEvent(QKeyEvent *ev, MeshModel &m, GLArea *gla) {
    bool hopDistModified = false;
    int keyPressed = ev->key();

    if (startingVertex != NULL) {
        switch (keyPressed) {
            case Qt::Key_A:
                this->maxHop *= 1.2;

                hopDistModified = true;

                break;

            case Qt::Key_Z:
                this->maxHop /= 1.2;

                hopDistModified = true;

                break;

            default: break;
        }
    }

    if (hopDistModified) {
        ComponentFinder<CMeshO, CVertexO>::Dijkstra(m.cm, *startingVertex, 6, this->maxHop, this->NotReachableVector);

        BorderVector.clear();
        ComponentVector = ComponentFinder<CMeshO, CVertexO>::FindComponent(m.cm, *startingVertex, 6, this->dist, this->maxHop, BorderVector, NotReachableVector);
    }

    gla->update();
    return;
}
