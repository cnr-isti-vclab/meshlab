/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

/*  A class implementing Meshlab's Edit interface that is for picking points in 3D
 * 
 * 
 * @author Oscar Barney
 */

#ifndef EDIT_PickPoints_PLUGIN_H
#define EDIT_PickPoints_PLUGIN_H

#include <common/interfaces.h>
#include "pickpointsDialog.h"

class EditPickPointsPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
public:
	//constructor
	EditPickPointsPlugin();
	
	//destructor
	virtual ~EditPickPointsPlugin() {
		//free memory used by the gui
		delete pickPointsDialog;
	}
	
	static const QString Info();

	virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void mousePressEvent(QMouseEvent *event, MeshModel &, GLArea * ) ;
	virtual void mouseMoveEvent(QMouseEvent *event, MeshModel &, GLArea * ) ;
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );

	//basically copied from void AlignPairWidget::drawPickedPoints in editalign plugin
	//Draws all the picked points on the screen
	//boundingBox - gives some indication how to scale the normal flags
	void drawPickedPoints(std::vector<PickedPointTreeWidgetItem*> &pointVector, vcg::Box3f &boundingBox);
	
private:
	//the current place the mouse clicked
	QPoint currentMousePosition;

	//flag that tells the decorate function whether to notify the dialog of this point
	bool registerPoint;
	
	//flag that tells the decorate function whether we are moving points
	bool moveSelectPoint;
	
	//the gui dialog for this plugin
	PickPointsDialog *pickPointsDialog;
	
	//we need this in order to redraw the points
	GLArea *glArea;

	//model we currently have
	MeshModel *currentModel;
	
	int overrideCursorShape;
};
	
#endif
