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

#ifndef OcmeEditPlugin_H
#define OcmeEditPlugin_H

#include <QObject>
#include <common/interfaces.h>
#include <common/meshmodel.h>
#include "ui_ocme.h"



/* OCME include */
#include <ocme/ocme.h>
#include <wrap/gui/trackball.h>
#include <wrap/gl/splatting_apss/splatrenderer.h>

class OcmeEditPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
		
public:
	OcmeEditPlugin();
	virtual ~OcmeEditPlugin() {}

    static const QString Info();

	virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);

	virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * ) ;
	virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * ) ;
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );

	void drawFace(CMeshO::FacePointer fp,MeshModel &m, GLArea * gla);
	void setTrackBall();
	void resetPlugin();
	void UpdateBoundingBox();

	QMutex rendering;
	

	QFont qFont;
	bool haveToPick;
	CMeshO::FacePointer curFacePtr;
	QDockWidget * ocme_panel;
	Ui::OcmeDockWidget * odw;
	GLArea * gla;
	unsigned int pickx,picky;
	bool pick;
	std::vector<Cell*> cells_to_edit;

	/* selection */
	bool showTouched;
	bool isDragging;
	int isToSelect;
	QPoint start;
	QPoint cur;
	QPoint prev;
	void DrawXORRect(GLArea * gla);
	std::vector<Cell*> selected_cells;

	/* use splatting */
	int  impostorRenderMode;
	/* OCME core */
	OCME * ocme;						// [v0.1] only one ocm database at time cna be opened
	bool ocme_loaded,initialized;

	/* debug */
	int n_poly;

	QString ocm_name;

	MeshModel * mm;						// mesh associated with ocme

	std::vector<CellKey> all_keys;		// all the dells in the database

	vcg::Box3f ocme_bbox;				// bounding box of the whole dataset

	vcg::Trackball curr_track;

void 	suspendEditMode();
void DrawCellsToEdit();
public slots:
	void loadOcm();				// load an ocm database
	void closeOcm();			// close an ocm database
	void createOcm();			// create an ocm database
	void edit();				// edit current selection
	void markEditable();		// mark as selected (flag S) the editable element on the editing mesh
	void drop();				// drop current selection
	void commit();				// commit
	void add();					// add new mesh
	void addFromDisk();			// add from disk
	void refreshImpostors();	// refresh impostor (debug)
	void toggleExtraction();
	void toggleShowTouched();
	void updateButtonsState();
	void editAll();
	void verify();
	void renderModeChanged(int);
	void toggleImpostors(int);
	void setvideoram(int);
	/* ui only*/
	void fillMeshAttribute();	// fill the list of trimesh attribute
	void clearMeshAttribute();	// fill the list of trimesh attribute
	void fillOcmAttribute();	// fill the list of ocm attributes
	void clearOcmAttribute();	// fill the list of ocm attributes
	void tri2ocmAttribute();	// copy from meshattr to ocmattr
	void ocm2triAttribute();	// copy from ocmattr to meshattr
};

#endif
