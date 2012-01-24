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
    EditPointPlugin();
    virtual ~EditPointPlugin() {}

    static const QString Info();

    bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    void Decorate(MeshModel &/*m*/, GLArea * /*parent*/, QPainter *p);
    void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * );
    void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * );
    void mouseReleaseEvent(QMouseEvent *, MeshModel &/*m*/, GLArea * );
    void keyPressEvent(QKeyEvent *, MeshModel &/*m*/, GLArea *);


private:    
        bool isMousePressed;
        bool haveToPick;

        CMeshO::VertexPointer startingVertex;
        vcg::Point2f startingClick;

        float dist;
        float maxHop;

        std::vector<CMeshO::VertexPointer> ComponentVector;
        std::vector<CMeshO::VertexPointer> BorderVector;
        std::vector<CMeshO::VertexPointer> NotReachableVector;

        QPoint cur;
};

#endif
