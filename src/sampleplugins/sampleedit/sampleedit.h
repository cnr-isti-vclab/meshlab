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

#ifndef SAMPLEEDITPLUGIN_H
#define SAMPLEEDITPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

class SampleEditPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
  QList <QAction *> actionList;
		
public:
    SampleEditPlugin();
    virtual ~SampleEditPlugin() {}

    virtual const QString Info(QAction *);
    virtual const PluginInfo &Info();

    virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
    virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent    (QAction *, QMouseEvent *, MeshModel &, GLArea * ) {};
    virtual void mouseMoveEvent     (QAction *, QMouseEvent *, MeshModel &, GLArea * ) {};
    virtual void mouseReleaseEvent  (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );

		virtual QList<QAction *> actions() const ;
		
		void drawFace(CMeshO::FacePointer fp,MeshModel &m, GLArea * gla);

    QPoint cur;
		QFont qFont;
    bool haveToPick;
		CMeshO::FacePointer curFacePtr;

};

#endif
