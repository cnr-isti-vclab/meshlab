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

#ifndef MESHLAB_DECORATE_PLUGIN_H
#define MESHLAB_DECORATE_PLUGIN_H

#include "meshlab_plugin.h"

class GLArea;

/**
 * @class The DecoratePlugin is the base class of all <b> decorators </b>
 * Decorators are 'read-only' visualization aids that helps to show some data about a document.
 * Decorators can make some permesh precomputation but the rendering has to be efficient.
 * Decorators should save the additional data into per-mesh attribute.

 * There are two classes of Decorations
 * - PerMesh
 * - PerDocument

 * PerMesh Decorators are associated to each mesh/view
 * Some example of PerDocument Decorations
 * - backgrounds
 * - trackball icon
 * - axis
 * - shadows
 * - screen space Ambient occlusion (think it as a generic 'darkner')

 * Some example of PerMesh Decorations
 * - coloring of selected vertex/face
 * - displaying of normals/curvature directions
 * - display of specific tagging
 **/

class DecoratePlugin : public MeshLabPlugin
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

	DecoratePlugin() : MeshLabPlugin() {}
	virtual ~DecoratePlugin() {}
	/** The very short string (a few words) describing each filtering action
	// This string is used also to define the menu entry
	*/
	virtual QString decorationName(ActionIDType) const = 0;
	virtual QString decorationInfo(ActionIDType) const = 0;

	virtual QString decorationName(const QAction *a) const { return decorationName(ID(a)); }
	virtual QString decorationInfo(const QAction *a) const { return decorationInfo(ID(a)); }

	// See source file for documentation
	virtual void initGlobalParameterList(const QAction* format, RichParameterList& globalparam);

	virtual bool startDecorate(const QAction *, MeshDocument &, const RichParameterList *, GLArea *) { return false; }
	virtual bool startDecorate(const QAction *, MeshModel &, const RichParameterList *, GLArea *) { return false; }
	virtual void decorateMesh(const QAction *, MeshModel &, const RichParameterList *, GLArea *, QPainter *, GLLogStream &) = 0;
	virtual void decorateDoc(const QAction *, MeshDocument &, const RichParameterList *, GLArea *, QPainter *, GLLogStream &) = 0;
	virtual void endDecorate(const QAction *, MeshModel &, const RichParameterList *, GLArea *) {}
	virtual void endDecorate(const QAction *, MeshDocument &, const RichParameterList *, GLArea *) {}

	/** \brief tests if a decoration is applicable to a mesh.
	* used only for PerMesh Decorators.
	For instance curvature cannot be shown on a mesh without curvature.
	On failure (returning false) the function fills the MissingItems list with strings describing the missing items.
	It is invoked only for decoration of \i PerMesh class;
	*/
	virtual bool isDecorationApplicable(const QAction *, const MeshModel&, QString&) const { return true; }

	virtual int getDecorationClass(const QAction *) const = 0;

	virtual QList<QAction *> actions() const { return actionList; }
	virtual QList<ActionIDType> types() const { return typeList; }

	virtual QAction *action(QString name) const;

protected:
	QList <QAction *> actionList;
	QList <ActionIDType> typeList;
	virtual ActionIDType ID(const QAction *a) const;
	virtual ActionIDType ID(QString name) const;
};

#define DECORATE_PLUGIN_IID  "vcg.meshlab.DecoratePlugin/1.0"
Q_DECLARE_INTERFACE(DecoratePlugin, DECORATE_PLUGIN_IID)

#endif // MESHLAB_DECORATE_PLUGIN_H
