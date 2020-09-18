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

/**
Used to customized the rendering process.
Rendering plugins are now responsible of the rendering of the whole MeshDocument and not only of a single MeshModel.

The Render function is called in with the ModelView and Projection Matrices already set up, screen cleared and background drawn.
After the Render call the MeshLab frawework draw on the opengl context other decorations and the trackball, so it there is the
requirement for a rendering plugin is that it should leave the z-buffer in a coherent state.

The typical rendering loop of a Render plugin is something like, :

<your own opengl setup>

foreach(MeshModel * mp, meshDoc.meshList)
{
if(mp->visible) mp->Render(rm.drawMode,rm.colorMode,rm.textureMode);
}

*/

class MeshRenderInterface : public PluginInterface
{
public:
	MeshRenderInterface() :PluginInterface() {}
	virtual ~MeshRenderInterface() {}

	virtual void Init(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& /*mp*/, GLArea *) {}
	virtual void Render(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& mp, GLArea *) = 0;
	virtual void Finalize(QAction *, MeshDocument *, GLArea *) {}
	virtual bool isSupported() = 0;
	virtual QList<QAction *> actions() = 0;
};
/**
MeshDecorateInterface is the base class of all <b> decorators </b>
Decorators are 'read-only' visualization aids that helps to show some data about a document.
Decorators can make some permesh precomputation but the rendering has to be efficient.
Decorators should save the additional data into per-mesh attribute.


There are two classes of Decorations
- PerMesh
- PerDocument

PerMesh Decorators are associated to each mesh/view
Some example of PerDocument Decorations
- backgrounds
- trackball icon
- axis
- shadows
- screen space Ambient occlusion (think it as a generic 'darkner')

Some example of PerMesh Decorations
- coloring of selected vertex/face
- displaying of normals/curvature directions
- display of specific tagging
*/

class MeshDecorateInterface : public PluginInterface
{
public:

	/** The DecorationClass enum represents the set of keywords that must be used to categorize a filter.
	Each filter can belong to one or more filtering class, or-ed together.
	*/
	enum DecorationClass
	{
		Generic = 0x00000, /*!< Should be avoided if possible. */  //
		PerMesh = 0x00001, /*!<  Decoration that are applied on a single mesh */
		PerDocument = 0x00002, /*!<  Decoration that are applied on a single mesh */
		PreRendering = 0x00004, /*!<  Decoration that are applied <i>before</i> the rendering of the document/mesh */
		PostRendering = 0x00008  /*!<  Decoration that are applied <i>after</i> the rendering of the document/mesh */
	};

	MeshDecorateInterface() : PluginInterface() {}
	virtual ~MeshDecorateInterface() {}
	/** The very short string (a few words) describing each filtering action
	// This string is used also to define the menu entry
	*/
	virtual QString decorationName(FilterIDType) const = 0;
	virtual QString decorationInfo(FilterIDType) const = 0;

	virtual QString decorationName(QAction *a) const { return decorationName(ID(a)); }
	virtual QString decorationInfo(QAction *a) const { return decorationInfo(ID(a)); }


	virtual bool startDecorate(QAction *, MeshDocument &, const RichParameterList *, GLArea *) { return false; }
	virtual bool startDecorate(QAction *, MeshModel &, const RichParameterList *, GLArea *) { return false; }
	virtual void decorateMesh(QAction *, MeshModel &, const RichParameterList *, GLArea *, QPainter *, GLLogStream &) = 0;
	virtual void decorateDoc(QAction *, MeshDocument &, const RichParameterList *, GLArea *, QPainter *, GLLogStream &) = 0;
	virtual void endDecorate(QAction *, MeshModel &, const RichParameterList *, GLArea *) {}
	virtual void endDecorate(QAction *, MeshDocument &, const RichParameterList *, GLArea *) {}

	/** \brief tests if a decoration is applicable to a mesh.
	* used only for PerMesh Decorators.
	For instance curvature cannot be shown on a mesh without curvature.
	On failure (returning false) the function fills the MissingItems list with strings describing the missing items.
	It is invoked only for decoration of \i PerMesh class;
	*/
	virtual bool isDecorationApplicable(QAction *, const MeshModel&, QString&) const { return true; }

	virtual int getDecorationClass(QAction *) const = 0;

	virtual QList<QAction *> actions() const { return actionList; }
	virtual QList<FilterIDType> types() const { return typeList; }
protected:
	QList <QAction *> actionList;
	QList <FilterIDType> typeList;
	virtual FilterIDType ID(QAction *a) const
	{
		QString aa=a->text();
		foreach(FilterIDType tt, types())
			if (a->text() == this->decorationName(tt)) return tt;
		aa.replace("&","");
		foreach(FilterIDType tt, types())
			if (aa == this->decorationName(tt)) return tt;

		qDebug("unable to find the id corresponding to action  '%s'", qUtf8Printable(a->text()));
		assert(0);
		return -1;
	}
	virtual FilterIDType ID(QString name) const
	{
		QString n = name;
		foreach(FilterIDType tt, types())
			if (name == this->decorationName(tt)) return tt;
		n.replace("&","");
		foreach(FilterIDType tt, types())
			if (n == this->decorationName(tt)) return tt;

		qDebug("unable to find the id corresponding to action  '%s'", qUtf8Printable(name));
		assert(0);
		return -1;
	}
public:
	virtual QAction *action(QString name) const
	{
		QString n = name;
		foreach(QAction *tt, actions())
			if (name == this->decorationName(ID(tt))) return tt;
		n.replace("&","");
		foreach(QAction *tt, actions())
			if (n == this->decorationName(ID(tt))) return tt;

		qDebug("unable to find the id corresponding to action  '%s'", qUtf8Printable(name));
		return 0;
	}
};


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

#define MESH_RENDER_INTERFACE_IID  "vcg.meshlab.MeshRenderInterface/1.0"
#define MESH_DECORATE_INTERFACE_IID  "vcg.meshlab.MeshDecorateInterface/1.0"
#define MESH_EDIT_INTERFACE_IID  "vcg.meshlab.MeshEditInterface/1.0"
#define MESH_EDIT_INTERFACE_FACTORY_IID  "vcg.meshlab.MeshEditInterfaceFactory/1.0"

Q_DECLARE_INTERFACE(MeshRenderInterface, MESH_RENDER_INTERFACE_IID)
Q_DECLARE_INTERFACE(MeshDecorateInterface, MESH_DECORATE_INTERFACE_IID)
Q_DECLARE_INTERFACE(MeshEditInterface, MESH_EDIT_INTERFACE_IID)
Q_DECLARE_INTERFACE(MeshEditInterfaceFactory, MESH_EDIT_INTERFACE_FACTORY_IID)

#endif
