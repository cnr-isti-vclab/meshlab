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
#ifndef EDITPLUGIN_H
#define EDITPLUGIN_H

#include <common/plugins/interfaces/edit_plugin.h>

class EditSelectPlugin : public QObject, public EditPlugin
{
	Q_OBJECT
	MESHLAB_EDIT_PLUGIN
	Q_INTERFACES(EditPlugin)


public:
	enum { SELECT_FACE_MODE, SELECT_VERT_MODE, SELECT_CONN_MODE, SELECT_AREA_MODE };

	EditSelectPlugin(int _ConnectedMode);

	virtual ~EditSelectPlugin() {}

	static QString Info();
	void suggestedRenderingData(MeshModel & m, MLRenderingData& dt);
	bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);
	void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/) {}
	void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
	void mousePressEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *);
	void mouseMoveEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *);
	void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *);
	void keyReleaseEvent(QKeyEvent *, MeshModel &/*m*/, GLArea *);
	void keyPressEvent(QKeyEvent *, MeshModel &/*m*/, GLArea *);

	vcg::Point2f start;
	vcg::Point2f cur;
	vcg::Point2f prev;
	bool isDragging;
	int selectionMode;
	std::vector<CMeshO::FacePointer> LastSelFace;
	std::vector<CMeshO::VertexPointer> LastSelVert;

	// for area selection
	std::vector<vcg::Point2f> selPolyLine;
	int areaMode = 0;
	int multiMode = 0;
	// proj data
	GLdouble mvMatrix_f[16];
	GLdouble prMatrix_f[16];
	GLint viewpSize[4];
	Eigen::Matrix<Scalarm,4,4> SelMatrix;
	Scalarm SelViewport[4];

signals:
	void setDecorator(QString, bool);

private:
	typedef enum { SMAdd, SMClear, SMSub } ComposingSelMode; // How the selection are composed
	ComposingSelMode composingSelMode;
	bool selectFrontFlag;
	void DrawXORRect(GLArea * gla, bool doubleDraw);
	void DrawXORPolyLine(GLArea * gla);
	void doSelection(MeshModel &m, GLArea *gla, int mode);
};

#endif
