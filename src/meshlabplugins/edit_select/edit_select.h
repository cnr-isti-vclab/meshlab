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

#include <common/interfaces.h>

class EditSelectPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)


public:
  enum { SELECT_FACE_MODE, SELECT_VERT_MODE, SELECT_CONN_MODE } ;

  EditSelectPlugin(int _ConnectedMode);

    virtual ~EditSelectPlugin() {}

    static QString Info();
    virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/){};
    virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseMoveEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
		virtual void keyReleaseEvent  (QKeyEvent *, MeshModel &/*m*/, GLArea *);
		virtual void keyPressEvent    (QKeyEvent *, MeshModel &/*m*/, GLArea *);

    QPoint start;
    QPoint cur;
    QPoint prev;
    bool isDragging;
    int selectionMode;
    std::vector<CMeshO::FacePointer> LastSelFace;
    std::vector<CMeshO::VertexPointer> LastSelVert;

signals:
	void setSelectionRendering(bool);

private:
  typedef enum {SMAdd, SMClear,SMSub} ComposingSelMode; // How the selection are composed
  ComposingSelMode composingSelMode;
	bool selectFrontFlag;
  void DrawXORRect(GLArea * gla, bool doubleDraw);
};

#endif
