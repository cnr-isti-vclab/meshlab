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

#include <common/plugins/interfaces/edit_plugin.h>
#include "pickpointsDialog.h"

class EditPickPointsPlugin : public QObject, public EditPlugin
{
	Q_OBJECT
	MESHLAB_EDIT_PLUGIN
	Q_INTERFACES(EditPlugin)
	
public:
	//constructor
	EditPickPointsPlugin();
	
	//destructor
	virtual ~EditPickPointsPlugin() {
		//free memory used by the gui
		delete pickPointsDialog;
	}
	
	static const QString Info();

	virtual bool startEdit(MeshModel & mm, GLArea * gla, MLSceneGLSharedDataContext* /*cont*/);
	virtual void endEdit(MeshModel & mm, GLArea * gla, MLSceneGLSharedDataContext* /*cont*/);
  virtual void decorate(MeshModel &/*m*/, GLArea * /*parent*/, QPainter *);
	virtual void mousePressEvent(QMouseEvent *event, MeshModel &, GLArea * ) ;
	virtual void mouseMoveEvent(QMouseEvent *event, MeshModel &, GLArea * ) ;
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );

	//basically copied from void AlignPairWidget::drawPickedPoints in editalign plugin
	//Draws all the picked points on the screen
	//boundingBox - gives some indication how to scale the normal flags
  void drawPickedPoints(std::vector<PickedPointTreeWidgetItem*> &pointVector, Box3m &boundingBox, QPainter *painter);
	
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
