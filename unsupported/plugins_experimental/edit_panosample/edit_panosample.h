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

#ifndef EDITPAINT_H
#define EDITPAINT_H

#include <GL/glew.h>
#include <QObject>
#include <QDockWidget>

#include <meshlab/glarea.h>
#include <common/interfaces.h>
#include <wrap/gl/pick.h>

#include "qualitychecker.h"




/**
 * EditPanosample plugin main class (MeshEditing plugin)
 */
class EditPanosamplePlugin : public QObject, public MeshEditInterface {
	Q_OBJECT
		Q_INTERFACES(MeshEditInterface)

public:
	EditPanosamplePlugin();
	virtual ~EditPanosamplePlugin();

	static const QString Info();

	bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);
	void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);
	void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
	void mousePressEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *);
	void mouseMoveEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *);
	void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *);


signals:
	void setSelectionRendering(bool);

public slots:
	void update();
	void on_createImageSpace();
	void on_layerChosenChanged();

	//void runPanoSample();
	//void setSamplePoints();
private:
	bool createImageSpaceTrigger;


	void panoRender(GLArea * gla);
	void drawScene(GLArea * gla);

	std::vector<QImage> panoramas;
	QDockWidget* dock1;
	QualityChecker * qualitychecker;
	MeshModel * meshmodel;

	GLArea * glarea;
};


#endif
