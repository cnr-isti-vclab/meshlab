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

#include <QObject>
#include <common/interfaces.h>
#include "align/AlignPair.h" 
#include "align/OccupancyGrid.h" 
#include "meshtree.h"
#include <wrap/gui/trackball.h>
#include "alignDialog.h"

class EditAlignPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
	enum 
	{
		ALIGN_IDLE = 0x01,
		ALIGN_INSPECT_ARC = 0x02,
		ALIGN_MOVE = 0x03
	};
		
	void Pick( MeshModel &m, GLArea * gla);

public:
    EditAlignPlugin();
		virtual ~EditAlignPlugin() {
			mode=ALIGN_IDLE;
		}

    static const QString Info();

    virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * ) ;
    virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * ) ;
    virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );

		QFont qFont;
  	int mode;			

    AlignDialog *alignDialog; //the widget with the list of the meshes. 
		
		void toggleButtons();

		vcg::Trackball trackball;
		
		MeshDocument *md; 
		MeshNode *currentNode() {return meshTree.find(md->mm());}
		vcg::AlignPair::Result *currentArc() {return  alignDialog->currentArc;};
		MeshTree meshTree;
		GLArea * gla;

public:
	vcg::AlignPair::Param defaultAP;  // default alignment parameters
	
	// this callback MUST be redefined because we are able to manage internally the layer change.
  virtual void LayerChanged(MeshDocument &/*md*/, MeshModel &/*oldMeshModel*/, GLArea */*parent*/)
	{ 
	// add code here to manage the external layer switching
	}

public slots:
		void process();
		void recalcCurrentArc();
		void glueHere();
		void glueHereAll();
		void glueManual();
		void glueByPicking();
		void alignParam();
		void alignParamCurrent();
    void hideRevealGluedMesh();
		
		void DrawArc( vcg::AlignPair::Result *A );
signals:
	void suspendEditToggle();

};

#endif
