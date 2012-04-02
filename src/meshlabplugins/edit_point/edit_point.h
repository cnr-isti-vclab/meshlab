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

#ifndef EDITPOINTPLUGIN_H
#define EDITPOINTPLUGIN_H

#include <QObject>
#include <common/interfaces.h>

class EditPointPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
		
public:
    enum {SELECT_DEFAULT_MODE, SELECT_FITTING_PLANE_MODE};

    EditPointPlugin(int _editType);
    virtual ~EditPointPlugin() {}

    static const QString Info();

    bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    void Decorate(MeshModel &/*m*/, GLArea * /*parent*/, QPainter *p);
    void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * );
    void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * );
    void mouseReleaseEvent(QMouseEvent *, MeshModel &/*m*/, GLArea * );
    void keyPressEvent(QKeyEvent *, MeshModel &/*m*/, GLArea *);
    void wheelEvent(QWheelEvent*, MeshModel &/*m*/, GLArea * );

signals:
    void setSelectionRendering(bool);

private:
        // How the selections are composed
        typedef enum {SMAdd, SMClear,SMSub} ComposingSelMode;
        ComposingSelMode composingSelMode;

        // Could be SELECT_DEFAULT_MODE or SELECT_FITTING_PLANE_MODE
        int editType;

        bool isMousePressed;
        bool haveToPick;

        CMeshO::VertexPointer startingVertex;
        vcg::Point2f startingClick;

        float dist;                 // the selection's radius
        float maxHop;               // the maximus hop distance allowed between 2 adjacent points
        float fittingRadiusPerc;    // the percentage of the selection's radius used to interpolate points to find the plane
        float fittingRadius;        // fittingRadiusPerc * dist
        float planeDist;            // maximus distance that a point has from the plane in order to be selected

        vcg::Plane3<CMeshO::ScalarType> fittingPlane;

        // Used to draw the circle that shows how the plane is inclinated
        CMeshO fittingCircle;

        // ComponentVector: the points in the actual selection (red colored)
        // BorderVector: the points in the actual border (yellow colored)
        // NotReachableVector: points not reachable from the actual startingVertex (used to find the BorderVector)
        // OldComponentVector: the previous selection: used to compose different selections
        std::vector<CMeshO::VertexPointer> ComponentVector;
        std::vector<CMeshO::VertexPointer> BorderVector;
        std::vector<CMeshO::VertexPointer> NotReachableVector;
        std::vector<CMeshO::VertexPointer> OldComponentVector;

        QPoint cur;
};

#endif
