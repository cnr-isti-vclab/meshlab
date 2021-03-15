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

#ifndef MESHLAB_EDIT_PLUGIN_H
#define MESHLAB_EDIT_PLUGIN_H

#include <QTabletEvent>

#include "meshlab_plugin.h"
#include "meshlab_plugin_file.h"
#include "../../ml_document/mesh_document.h"


class GLArea;

/**
 * @brief The EditTool class is used to provide tools that needs some kind of
 * interaction with the mesh. Editing tools are exclusive (only one at a time) 
 * and can grab the mouse events and customize the rendering process.
 **/

class EditTool : public MeshLabPlugin
{
public:
	EditTool() : MeshLabPlugin() {}
	virtual ~EditTool() {}

	//should return a sentence describing what the editing tool does
	static const QString info();

	virtual void suggestedRenderingData(MeshModel &/*m*/, MLRenderingData& /*dt*/) {}

	// Called when the user press the first time the button
	virtual bool startEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/) { return true; }
	virtual bool startEdit(MeshDocument &md, GLArea *parent, MLSceneGLSharedDataContext* cont)
	{
		//assert(NULL != md.mm());
		if (md.mm() != NULL)
			return (startEdit(*(md.mm()), parent, cont));
		else return false;
	}
	// Called when the user press the second time the button
	virtual void endEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/) {}
	virtual void endEdit(MeshDocument &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/) {}

	// There are two classes of editing tools, the one that works on a single layer at a time
	// and the ones that works on all layers and have to manage in a correct way the action of changing the current layer.
	// For the edit tools that works ona single layer changing the layer means the restart of the edit tool.
	virtual bool isSingleMeshEdit() const { return true; }

	// Called when the user changes the selected layer
	//by default it calls end edit with the layer that was selected and start with the new layer that is
	//selected.  This ensures that plugins who don't support layers do not get sent pointers to meshes
	//they are not expecting.
	// If your editing plugins is not singleMesh you MUST reimplement this to correctly handle the change of layer.
	virtual void layerChanged(MeshDocument &md, MeshModel &oldMeshModel, GLArea *parent, MLSceneGLSharedDataContext* cont)
	{
		assert(this->isSingleMeshEdit());
		endEdit(oldMeshModel, parent, cont);
		startEdit(md, parent, cont);
	}

	virtual void decorate(MeshModel &m, GLArea *parent, QPainter * /*p*/) { decorate(m, parent); }
	virtual void decorate(MeshModel &/*m*/, GLArea * /*parent*/) {}

	virtual void mousePressEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *) = 0;
	virtual void mouseMoveEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *) = 0;
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *) = 0;
	virtual void keyReleaseEvent(QKeyEvent *, MeshModel &/*m*/, GLArea *) {}
	virtual void keyPressEvent(QKeyEvent *, MeshModel &/*m*/, GLArea *) {}
	virtual void wheelEvent(QWheelEvent*, MeshModel &/*m*/, GLArea *) {}
	virtual void tabletEvent(QTabletEvent * e, MeshModel &/*m*/, GLArea *) { e->ignore(); }
};


/** 
 * @brief The EditPluginFactory class  is used to generate an action for each 
 * starting of an editing filter.
 *
 * This is needed because editing filters have a internal state, so if you want 
 * to have an editing tool for two different documents you have to instance 
 * two objects. This class is used by the framework to generate an independent 
 * EditPlugin for each document.
 */
class EditPluginFactory : public MeshLabPluginFile
{
public:
	EditPluginFactory() {}
	virtual ~EditPluginFactory() {}

	//gets a list of actions available from this plugin
	virtual std::list<QAction *> actions() const {return actionList;};

	//get the edit tool for the given action
	virtual EditTool* getEditTool(const QAction *) = 0;

	//get the description for the given action
	virtual QString getEditToolDescription(const QAction *) = 0;

protected:
	std::list<QAction*> actionList;
};

#define MESHLAB_EDIT_PLUGIN \
	public: \
		virtual std::pair<std::string, bool> getMLVersion() const { \
			return std::make_pair(meshlab::meshlabVersion(), meshlab::builtWithDoublePrecision()); \
		} \
	private:

#define EDIT_TOOL_IID  "vcg.meshlab.EditTool/1.0"
#define EDIT_PLUGIN_FACTORY_IID  "vcg.meshlab.EditPluginFactory/1.0"

Q_DECLARE_INTERFACE(EditTool, EDIT_TOOL_IID)
Q_DECLARE_INTERFACE(EditPluginFactory, EDIT_PLUGIN_FACTORY_IID)


#endif // MESHLAB_EDIT_PLUGIN_H
