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
#ifndef PHOTOTEXTURING_H
#define PHOTOTEXTURING_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <PhotoTexturingWidget.h>
#include <src/PhotoTexturer.h>

class PhotoTexturingPlugin : public QObject, public MeshEditInterface {
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)

public:
	PhotoTexturingPlugin();
	virtual ~PhotoTexturingPlugin() {
	}

	static const QString Info();

	virtual void StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/){};
	
	virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea *);
	virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea *);
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *);

	void drawFace(CMeshO::FacePointer fp, MeshModel &m, GLArea * gla);
	
	GLArea *glArea;
	PhotoTexturingWidget *ptDialog;
	PhotoTexturer *photoTexturer;
	QFont qFont;
private: 
	signals:
	
	void suspendEditToggle();
};

#endif
