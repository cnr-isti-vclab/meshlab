/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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

#ifndef MESHLAB_INTERFACES_H
#define MESHLAB_INTERFACES_H
//#include <GL/glew.h>

#include "interfaces/plugin_interface.h"
#include "GLLogStream.h"
#include "meshmodel.h"

#include <QtCore>
#include <QApplication>
#include <QAction>
#include <QGLContext>
#include <QGLFormat>
#include <QMessageBox>
#include <QTabletEvent>
#include <QDebug>


class QWidget;
class QGLWidget;
class QString;
class QVariant;
class QMouseEvent;
class QTreeWidgetItem;
class MeshModel;
class RenderMode;
class GLArea;
class GLAreaReg;

class MeshModel;



/*
Editing Interface
Used to provide tools that needs some kind of interaction with the mesh.
Editing tools are exclusive (only one at a time) and can grab the mouse events and customize the rendering process.
*/

class MeshEditInterface : public PluginInterface
{
public:
	MeshEditInterface() : PluginInterface() {}
	virtual ~MeshEditInterface() {}

	//should return a sentence describing what the editing tool does
	static const QString Info();

	virtual void suggestedRenderingData(MeshModel &/*m*/, MLRenderingData& /*dt*/) {}

	// Called when the user press the first time the button
	virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/) { return true; }
	virtual bool StartEdit(MeshDocument &md, GLArea *parent, MLSceneGLSharedDataContext* cont)
	{
		//assert(NULL != md.mm());
		if (md.mm() != NULL)
			return (StartEdit(*(md.mm()), parent, cont));
		else return false;
	}
	// Called when the user press the second time the button
	virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/) {}
	virtual void EndEdit(MeshDocument &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/) {}

	// There are two classes of editing tools, the one that works on a single layer at a time
	// and the ones that works on all layers and have to manage in a correct way the action of changing the current layer.
	// For the edit tools that works ona single layer changing the layer means the restart of the edit tool.
	virtual bool isSingleMeshEdit() const { return true; }

	// Called when the user changes the selected layer
	//by default it calls end edit with the layer that was selected and start with the new layer that is
	//selected.  This ensures that plugins who don't support layers do not get sent pointers to meshes
	//they are not expecting.
	// If your editing plugins is not singleMesh you MUST reimplement this to correctly handle the change of layer.
	virtual void LayerChanged(MeshDocument &md, MeshModel &oldMeshModel, GLArea *parent, MLSceneGLSharedDataContext* cont)
	{
		assert(this->isSingleMeshEdit());
		EndEdit(oldMeshModel, parent, cont);
		StartEdit(md, parent, cont);
	}

	virtual void Decorate(MeshModel &m, GLArea *parent, QPainter * /*p*/) { Decorate(m, parent); }
	virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/) {}

	virtual void mousePressEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *) = 0;
	virtual void mouseMoveEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *) = 0;
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *) = 0;
	virtual void keyReleaseEvent(QKeyEvent *, MeshModel &/*m*/, GLArea *) {}
	virtual void keyPressEvent(QKeyEvent *, MeshModel &/*m*/, GLArea *) {}
	virtual void wheelEvent(QWheelEvent*, MeshModel &/*m*/, GLArea *) {}
	virtual void tabletEvent(QTabletEvent * e, MeshModel &/*m*/, GLArea *) { e->ignore(); }
};


/** MeshEditInterfaceFactory
\short The MeshEditInterfaceFactory class is a <i>factory</i> is used to generate a object for each starting of an editing filter.

This is needed because editing filters have a internal state, so if you want to have an editing tool for two different documents you have to instance two objects.
This class is used by the framework to generate an independent MeshEditInterface for each document.
*/
class MeshEditInterfaceFactory
{
public:
	virtual ~MeshEditInterfaceFactory() {}

	//gets a list of actions available from this plugin
	virtual QList<QAction *> actions() const = 0;

	//get the edit tool for the given action
	virtual MeshEditInterface* getMeshEditInterface(QAction *) = 0;

	//get the description for the given action
	virtual QString getEditToolDescription(QAction *) = 0;

};

#define MESHLAB_PLUGIN_IID_EXPORTER(x) Q_PLUGIN_METADATA(IID x)
#define MESHLAB_PLUGIN_NAME_EXPORTER(x)

#define MESH_EDIT_INTERFACE_IID  "vcg.meshlab.MeshEditInterface/1.0"
#define MESH_EDIT_INTERFACE_FACTORY_IID  "vcg.meshlab.MeshEditInterfaceFactory/1.0"

Q_DECLARE_INTERFACE(MeshEditInterface, MESH_EDIT_INTERFACE_IID)
Q_DECLARE_INTERFACE(MeshEditInterfaceFactory, MESH_EDIT_INTERFACE_FACTORY_IID)

#endif
