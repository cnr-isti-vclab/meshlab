/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#ifndef MESHLAB_EDIT_PLUGIN_INTERFACE_H
#define MESHLAB_EDIT_PLUGIN_INTERFACE_H

#include <QTabletEvent>

#include "meshlab_plugin.h"
#include "../../ml_document/mesh_document.h"


class GLArea;

/*
EditPluginInterface
Used to provide tools that needs some kind of interaction with the mesh.
Editing tools are exclusive (only one at a time) and can grab the mouse events and customize the rendering process.
*/

class EditPluginInterface : public MeshLabPlugin
{
public:
	EditPluginInterface() : MeshLabPlugin() {}
	virtual ~EditPluginInterface() {}

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
	
private:
	virtual QString pluginName() const final {return QString();};
};


/** MeshEditInterfaceFactory
\short The MeshEditInterfaceFactory class is a <i>factory</i> is used to generate a object for each starting of an editing filter.

This is needed because editing filters have a internal state, so if you want to have an editing tool for two different documents you have to instance two objects.
This class is used by the framework to generate an independent MeshEditInterface for each document.
*/
class EditPluginInterfaceFactory : public MeshLabPluginFile
{
public:
	EditPluginInterfaceFactory() {}
	virtual ~EditPluginInterfaceFactory() {}

	//gets a list of actions available from this plugin
	virtual QList<QAction *> actions() const = 0;

	//get the edit tool for the given action
	virtual EditPluginInterface* getMeshEditInterface(const QAction *) = 0;

	//get the description for the given action
	virtual QString getEditToolDescription(const QAction *) = 0;
};

#define MESHLAB_EDIT_PLUGIN \
	public: \
		virtual std::pair<std::string, bool> getMLVersion() const { \
			return std::make_pair(meshlab::meshlabVersion(), meshlab::builtWithDoublePrecision()); \
		} \
	private:

#define EDIT_PLUGIN_INTERFACE_IID  "vcg.meshlab.EditPluginInterface/1.0"
#define EDIT_PLUGIN_INTERFACE_FACTORY_IID  "vcg.meshlab.EditPluginInterfaceFactory/1.0"

Q_DECLARE_INTERFACE(EditPluginInterface, EDIT_PLUGIN_INTERFACE_IID)
Q_DECLARE_INTERFACE(EditPluginInterfaceFactory, EDIT_PLUGIN_INTERFACE_FACTORY_IID)


#endif // MESHLAB_EDIT_PLUGIN_INTERFACE_H
