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

#ifndef EditAlignPLUGIN_H
#define EditAlignPLUGIN_H

#include <common/interfaces/edit_plugin_interface.h>
#include <vcg/complex/algorithms/align_pair.h>
#include "align/OccupancyGrid.h"
#include "meshtree.h"
#include <wrap/gui/trackball.h>
#include "alignDialog.h"

class EditAlignPlugin : public QObject, public EditPluginInterface
{
	Q_OBJECT
		Q_INTERFACES(EditPluginInterface)

		enum
	{
		ALIGN_IDLE = 0x01,
		ALIGN_INSPECT_ARC = 0x02,
		ALIGN_MOVE = 0x03
	};

	void Pick(MeshModel &m, GLArea * gla);

public:
	EditAlignPlugin();
	virtual ~EditAlignPlugin() {
	}

	static const QString Info();

	bool isSingleMeshEdit() const { return false; }
	void suggestedRenderingData(MeshModel &m, MLRenderingData& dt);
	bool StartEdit(MeshDocument &md, GLArea *parent, MLSceneGLSharedDataContext* cont);
	void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
	void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);
	void mousePressEvent(QMouseEvent *, MeshModel &, GLArea *);
	void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea *);
	void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *);

	QFont qFont;
	int mode;

	AlignDialog *alignDialog; //the widget with the list of the meshes.

	void toggleButtons();

	vcg::Trackball trackball;
	GLArea* _gla;
	MeshDocument* _md;
	MeshNode *currentNode() { return meshTree.find(_md->mm()); }
	vcg::AlignPair::Result *currentArc() { return  alignDialog->currentArc; }
	MeshTree meshTree;
public:
	vcg::AlignPair::Param defaultAP;  // default alignment parameters
	MeshTree::Param defaultMTP;  // default MeshTree parameters

	// this callback MUST be redefined because we are able to manage internally the layer change.
	void LayerChanged(MeshDocument & /*md*/, MeshModel & /*oldMeshModel*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* )
	{
		// add code here to manage the external layer switching
	}

public slots:
	void process();
	void recalcCurrentArc();
	void glueHere();
	void glueHereVisible();
	void selectBadArc();
	void glueManual();
	void glueByPicking();
	void alignParam();
	void setAlignParamMM();
	void setAlignParamM();
	void meshTreeParam();
	void alignParamCurrent();
	void setBaseMesh();
	void hideRevealGluedMesh();

	void DrawArc(vcg::AlignPair::Result *A);
private:
	MLSceneGLSharedDataContext* _shared;
signals:
	void suspendEditToggle();

};

#endif
