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
#include <vcg/space/fitting3.h>

#include <vcg/complex/algorithms/create/platonic.h>

#include "connectedComponent.h"


/* defining the numbers of neighbours in the graph. Six seems to be good enough for our purpose */
#define K 6

using namespace std;
using namespace vcg;

EditPointPlugin::EditPointPlugin(int _editType) : editType(_editType) {}

const QString EditPointPlugin::Info() {
    return tr("Select a region of the point cloud thought to be in the same connected component.");
}
  
void EditPointPlugin::Decorate(MeshModel &m, GLArea * gla, QPainter *p)
{
  this->RealTimeLog("Point Selection",
                    "<table>"
                    "<tr><td width=50> Hop Thr:</td><td width=100 align=right><b >%8.3f </b></td><td><i> (Wheel to change it)</i> </td></tr>"
                    "<tr><td>          Radius: </td><td width=70 align=right><b> %8.3f </b></td><td><i> (Drag or Alt+Wheel to change it)</i></td></tr>"
                    "</table>",this->maxHop,this->dist);

    /* When the user first click we have to find the point under the mouse pointer.
       At the same time we need to compute the Dijkstra algorithm over the knn-graph in order
       to find the distance between the selected point and the others. */
    if(haveToPick)
    {
        glPushMatrix();
        glMultMatrix(m.cm.Tr);
        vector<CMeshO::VertexPointer> NewSel;
        GLPickTri<CMeshO>::PickVert(cur.x(), gla->height() - cur.y(), m.cm, NewSel);
        if(NewSel.size() > 0) {
            startingVertex = NewSel.front();

            tri::ComponentFinder<CMeshO>::Dijkstra(m.cm, *startingVertex, K, this->maxHop, this->NotReachableVector);

            ComponentVector.push_back(startingVertex);
        }

        haveToPick = false;
        glPopMatrix();
    }

    /* When at least a point is selected we need to draw the selection */
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

        tri::UpdateSelection<CMeshO>::VertexClear(m.cm);

        /* In OldComponentVector we find all the points selected until the last click of the mouse.
           The other points are saved in ComponentVector. With two different structures for old and new
           selection we can add or subtract one from each other. */
        switch (composingSelMode) {
        case SMSub:
            for(vector<CMeshO::VertexPointer>::iterator vi = OldComponentVector.begin(); vi != OldComponentVector.end(); ++vi) {
                (*vi)->SetS();
            }
            for(vector<CMeshO::VertexPointer>::iterator vi = ComponentVector.begin(); vi != ComponentVector.end(); ++vi) {
                (*vi)->ClearS();
            }
            break;

        case SMAdd:
            for(vector<CMeshO::VertexPointer>::iterator vi = OldComponentVector.begin(); vi != OldComponentVector.end(); ++vi) {
                (*vi)->SetS();
            }
            for(vector<CMeshO::VertexPointer>::iterator vi = ComponentVector.begin(); vi != ComponentVector.end(); ++vi) {
                (*vi)->SetS();
            }
            break;

        case SMClear:
            for(vector<CMeshO::VertexPointer>::iterator vi = ComponentVector.begin(); vi != ComponentVector.end(); ++vi) {
                (*vi)->SetS();
            }
            break;
        }

        /* The actual selection is drawn in red (instead of the automatic drawing of selected vertex
           of MeshLab) */
        glBegin(GL_POINTS);
        glColor4f(1,0,0,.5f);

        for (CMeshO::VertexIterator vi = m.cm.vert.begin(); vi != m.cm.vert.end(); vi++) {
            if (vi->IsS()) glVertex(vi->cP());
        }

        glEnd();

        /* Borders points are drawn in yellow. */
        glBegin(GL_POINTS);
        glColor4f(1,1,0,.5f);

        for(vector<CMeshO::VertexPointer>::iterator vi = BorderVector.begin(); vi != BorderVector.end(); ++vi)
        {
            if ((*vi)->IsS()) glVertex((*vi)->cP());
        }

        glEnd();

        /* If the "fitting plane" plugin is selected we draw a light blue circle to visualize the
           actual plane found by the algorithm (and the fitted points). */
        if (editType == SELECT_FITTING_PLANE_MODE) {
            fittingCircle.Clear();
            vcg::tri::OrientedDisk<CMeshO>(fittingCircle, 192, fittingPlane.Projection(startingVertex->cP()), fittingPlane.Direction(), this->fittingRadius);

            glBegin(GL_TRIANGLE_FAN);
            glColor4f(0.69,0.93,0.93,.7f);

            CMeshO::VertexIterator vi;
            for (vi = fittingCircle.vert.begin(); vi != fittingCircle.vert.end(); vi++) {
                glVertex(vi->cP());
            }
            vi = fittingCircle.vert.begin();
            vi++;
            glVertex(vi->cP());

            glEnd();
        }

        glPopAttrib();
        glPopMatrix();
    }
}

bool EditPointPlugin::StartEdit(MeshModel &m, GLArea *gla) {
    // Needed by MeshLab to colorize the selected points (for istance when we exit the plugin)
    connect(this, SIGNAL(setSelectionRendering(bool)), gla, SLOT(setSelectVertRendering(bool)));
    setSelectionRendering(true);

    for (CMeshO::VertexIterator vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi) {
        if (vi->IsS()) OldComponentVector.push_back(&*vi);
    }

    startingVertex = NULL;

    ComponentVector.clear();
    BorderVector.clear();
    NotReachableVector.clear();

    this->maxHop = m.cm.bbox.Diag() / 100.0;
    this->planeDist = m.cm.bbox.Diag() / 100.0;
    this->fittingRadiusPerc = 0.1;
    this->dist = 0;

    composingSelMode = SMClear;

    return true;
}

void EditPointPlugin::EndEdit(MeshModel &m, GLArea *gla) {
    //delete the circle if present.
    fittingCircle.Clear();
    tri::ComponentFinder<CMeshO>::DeletePerVertexAttribute(m.cm);
}


void EditPointPlugin::mousePressEvent(QMouseEvent *ev, MeshModel &m, GLArea *gla) {

    cur = ev->pos();

    this->isMousePressed = true;
    if(!(ev->modifiers() & Qt::AltModifier) || startingVertex == NULL)
    {
      this->startingClick = vcg::Point2f(ev->x(), ev->y());
      startingVertex = NULL;
      this->dist = 0.0;
      haveToPick = true;
    }
    this->fittingRadius = 0.0;

    OldComponentVector.clear();

    if (ev->modifiers() & Qt::ControlModifier || ev->modifiers() & Qt::ShiftModifier)
    {
        for (CMeshO::VertexIterator vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi) {
            if (vi->IsS()) OldComponentVector.push_back(&*vi);
        }
    }

    composingSelMode = SMClear;
    if(ev->modifiers() & Qt::ControlModifier) composingSelMode = SMAdd;
    if(ev->modifiers() & Qt::ShiftModifier) composingSelMode = SMSub;

    BorderVector.clear();
    ComponentVector.clear();
}

void EditPointPlugin::mouseMoveEvent(QMouseEvent *ev, MeshModel &m, GLArea *gla ) {
    if (this->isMousePressed && startingVertex != NULL) {
        float distFromCenter = math::Sqrt((startingClick.X() - ev->x())*(startingClick.X() - ev->x()) + (startingClick.Y() - ev->y()) * (startingClick.Y() - ev->y()));

        // TO DO: find a better approximation for the dist parameter
        float perim = 2 * m.cm.bbox.DimX() + 2 * m.cm.bbox.DimY();
        perim = (perim < 2 * m.cm.bbox.DimY() + 2 * m.cm.bbox.DimZ()) ? 2 * m.cm.bbox.DimY() + 2 * m.cm.bbox.DimZ() : perim;
        perim = (perim < 2 * m.cm.bbox.DimX() + 2 * m.cm.bbox.DimZ()) ? 2 * m.cm.bbox.DimX() + 2 * m.cm.bbox.DimZ() : perim;

        this->dist = (perim * distFromCenter) / gla->width();

        BorderVector.clear();

        if (editType == SELECT_DEFAULT_MODE)
            ComponentVector = tri::ComponentFinder<CMeshO>::FindComponent(m.cm, this->dist, BorderVector, NotReachableVector);
        else if (editType == SELECT_FITTING_PLANE_MODE) {
            this->fittingRadius = dist * fittingRadiusPerc;
            ComponentVector = tri::ComponentFinder<CMeshO>::FindComponent(m.cm, this->dist, BorderVector, NotReachableVector, true, fittingRadius, planeDist, &fittingPlane);
        }

        gla->update();
    }
}

void EditPointPlugin::mouseReleaseEvent(QMouseEvent *ev, MeshModel &m, GLArea *gla) {
    this->isMousePressed = false;

    gla->update();
}

void EditPointPlugin::keyPressEvent(QKeyEvent *ev, MeshModel &m, GLArea *gla) {
    bool hopDistModified = false;
    bool parameterModified = false;

    int keyPressed = ev->key();

    switch (keyPressed) {
    case Qt::Key_A:
        if (startingVertex != NULL) {
            this->maxHop *= 1.2;

            hopDistModified = true;
            parameterModified = true;
        }

        break;

    case Qt::Key_Z:
        if (startingVertex != NULL) {
            this->maxHop /= 1.2;

            hopDistModified = true;
            parameterModified = true;
        }

        break;

    case Qt::Key_S:
        if (startingVertex != NULL && editType == SELECT_FITTING_PLANE_MODE) {
            this->fittingRadiusPerc *= 1.2;
            this->fittingRadius = (dist * fittingRadiusPerc);

            parameterModified = true;
        }

        break;

    case Qt::Key_X:
        if (startingVertex != NULL && editType == SELECT_FITTING_PLANE_MODE) {
            this->fittingRadiusPerc /= 1.2;
            this->fittingRadius = (dist * fittingRadiusPerc);

            parameterModified = true;
        }

        break;

    case Qt::Key_D:
        if (startingVertex != NULL && editType == SELECT_FITTING_PLANE_MODE) {
            this->planeDist *= 1.2;

            parameterModified = true;
        }

        break;

    case Qt::Key_C:
        if (startingVertex != NULL && editType == SELECT_FITTING_PLANE_MODE) {
            this->planeDist /= 1.2;

            parameterModified = true;
        }

        break;

    default: break;

    }


    /* If the hop dist has been modified we need to recompute the distances because we may have some
       new arcs to consider in the Dijkstra algorithm.
       If we modified other parameters we need only to find the new selected component. */
    if (hopDistModified) {
        tri::ComponentFinder<CMeshO>::Dijkstra(m.cm, *startingVertex, 6, this->maxHop, this->NotReachableVector);
    }
    if (parameterModified) {
        BorderVector.clear();
        if (editType == SELECT_DEFAULT_MODE)
            ComponentVector = tri::ComponentFinder<CMeshO>::FindComponent(m.cm, this->dist, BorderVector, NotReachableVector);
        else if (editType == SELECT_FITTING_PLANE_MODE)
            ComponentVector = tri::ComponentFinder<CMeshO>::FindComponent(m.cm, this->dist, BorderVector, NotReachableVector, true, fittingRadius, planeDist, &fittingPlane);
    }

    gla->update();
    return;
}

void EditPointPlugin::wheelEvent(QWheelEvent* ev, MeshModel &m, GLArea *gla)
{
  bool hopDistModified = false;
  bool distModified = false;

  int wheelDirection = ev->delta();
  if (startingVertex != NULL && (ev->modifiers() & Qt::AltModifier))
  {
    this->dist *= pow(1.1f, wheelDirection / 120.f);
    distModified = true;
  }

  if (!(ev->modifiers() & Qt::AltModifier))
  {
    this->maxHop *= pow(1.1f, wheelDirection / 120.f);
    hopDistModified = true;
  }

  if (hopDistModified && (startingVertex != NULL)) {
    tri::ComponentFinder<CMeshO>::Dijkstra(m.cm, *startingVertex, K, this->maxHop, this->NotReachableVector);
  }

  if(startingVertex != NULL)
  {
    BorderVector.clear();

    if (editType == SELECT_DEFAULT_MODE)
      ComponentVector = tri::ComponentFinder<CMeshO>::FindComponent(m.cm, this->dist, BorderVector, NotReachableVector);
    else if (editType == SELECT_FITTING_PLANE_MODE)
      ComponentVector = tri::ComponentFinder<CMeshO>::FindComponent(m.cm, this->dist, BorderVector, NotReachableVector, true, fittingRadius, planeDist, &fittingPlane);
  }

  gla->update();
  return;

}
