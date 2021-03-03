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
#include <common/plugins/interfaces/edit_plugin.h>

class SampleEditPlugin : public QObject, public EditPlugin
{
	Q_OBJECT
	MESHLAB_EDIT_PLUGIN
	Q_INTERFACES(EditPlugin)
		
public:
    SampleEditPlugin();
    virtual ~SampleEditPlugin() {}

    static const QString Info();

    bool startEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);
    void endEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);
    void decorate(MeshModel &/*m*/, GLArea * /*parent*/, QPainter *p);
    void decorate (MeshModel &/*m*/, GLArea * ){};
    void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	void keyReleaseEvent(QKeyEvent *, MeshModel &, GLArea *);

    void drawFace(CMeshO::FacePointer fp,MeshModel &m, GLArea *gla, QPainter *p);
	void drawVert(CMeshO::VertexPointer vp, MeshModel &m, GLArea *gla, QPainter *p);

private:
    QPoint curGL;
	QFont qFont;
    bool haveToPick;
	int pickmode;
	CMeshO::FacePointer   curFacePtr;
	CMeshO::VertexPointer curVertPtr;
	std::vector<CMeshO::FacePointer>   NewFaceSel;
	std::vector<CMeshO::VertexPointer> NewVertSel;
	int pIndex;

signals:
	void suspendEditToggle();

};

#endif
