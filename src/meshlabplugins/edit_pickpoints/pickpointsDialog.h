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

/* A class to represent the ui for the pickpoints plugin
 *  
 * @author Oscar Barney
 */

#ifndef PICKPOINTS_DIALOG_H
#define PICKPOINTS_DIALOG_H

#include <QDockWidget>


#include <common/mesh_data_structures/meshmodel.h>

#include "pickedPoints.h"
#include "ui_pickpointsDialog.h"
#include "pickPointsTemplate.h"

class QGLWidget;

class EditPickPointsPlugin;

class GetClosestFace;

class PickedPointTreeWidgetItem : public QTreeWidgetItem
{
	
public:
	//used when a point has just been picked and
	//gives it an integer name
	PickedPointTreeWidgetItem(Point3m &intputPoint, CMeshO::FaceType::NormalType &faceNormal,
			QString name, bool _active);
	
	//set the name
	void setName(QString name);
	
	//return the name of the point
	QString getName();
	
	//change the point and normal
	void setPointAndNormal(Point3m &intputPoint, CMeshO::FaceType::NormalType &faceNormal);
	
	//return the Point3m
	Point3m getPoint();
	
	//get the normal
	Point3m getNormal();
	
	//clear the ponint datas
	void clearPoint();
		
	//return if the point is set
	bool isActive();

	//set the checkbox and active value
	void setActive(bool value);
	
	//only for use within the checkbox calls to toggle the active variable
	//when it is toggled
	void toggleActive(bool value);
	
private:
	//the point
	Point3m point;
	
	//the normal of this point
	Point3m normal;
	
	//whether this point is active
	//inactive points are not drawn and when saved this is indicated
	bool active;
};


class PickPointsDialog : public QDockWidget
{
	Q_OBJECT

public:
	PickPointsDialog(EditPickPointsPlugin *plugin,
			QWidget *parent = 0);

	~PickPointsDialog();
	
	enum Mode { ADD_POINT, MOVE_POINT, SELECT_POINT };
	
	//do something with the point that was just picked(could be add,moving or select)
	void addMoveSelectPoint(Point3m point, CMeshO::FaceType::NormalType faceNormal);
	
	//we need to move the point closest to this one or select it depending on the mode
	void selectOrMoveThisPoint(Point3m point);
		
	//return the vector
	//useful if you want to draw the points
	std::vector<PickedPointTreeWidgetItem*>& getPickedPointTreeWidgetItemVector();
	
	//return the mode of this ui
	PickPointsDialog::Mode getMode();
	
	//sets the currentMesh we are working with
	void setCurrentMeshModel(MeshModel *, QGLWidget *gla);
	
	//allows the ability to save to metaData only even if the ui says save to a file
	void savePointsToMetaData();
	
	bool showNormal();
	
	bool drawNormalAsPin();
	
	//set flag that says the next value will overwrite one we may want to jump back to
	void recordNextPointForUndo();

public slots:	
	//redraw the points on the screen
	void redrawPoints();

private:
	//get the points from the UI
	PickedPoints * getPickedPoints();
	
	//add a new point and call it something 
	//bool present tells us if the point has been picked yet
	void addPoint(Point3m &point, QString &name,  bool present);
	
	//handle everything involved with adding a point to the tree widget
	PickedPointTreeWidgetItem * addTreeWidgetItemForPoint(Point3m &point, QString &name, CMeshO::FaceType::NormalType &faceNormal, bool present);
	
	//load the points from a file
	void loadPoints(QString filename);
		
	//loads the default template if there is one
	void tryLoadingDefaultTemplate();
	
	//load a point template
	void loadPickPointsTemplate(QString filename);
	
	//if true only remove coord (keep template), if false remove all points from tree widget (start over)
	void clearPoints(bool clearOnlyXYZValues);
	
	//clears the template, does not ask the user anything
	void clearTemplate();	
	
	//set the TemplateName
	void setTemplateName(QString name);
	
	//the current mode of the GUI
	Mode currentMode;
	
	//QT pattern - the ui pointer
	Ui::pickpointsDialog ui;

	//a map of the tree items to the points they represent
	std::vector<PickedPointTreeWidgetItem*> pickedPointTreeWidgetItemVector; 
	
	//the parent plugin
	EditPickPointsPlugin *parentPlugin;
	
	//in MOVE_POINT Mode this holds a pointer to the item
	//found on mouse button down so that on mouse button
	//up it can be moved
	PickedPointTreeWidgetItem *itemToMove;
	
	//the template we have loaded
	bool templateLoaded;
	
	//the template that was used to pick these points
	//will be "" if not template was used or the template was invalidated (by a modification for example)
	QString templateName;
	
	//we need this in order to save to the meta data and get the filename, etc
	MeshModel *meshModel;
	
	//the glarea to update
	QGLWidget*_glArea;
	
	GetClosestFace *getClosestFace;
	
	//basically just so we have a unique default name for new points that are picked
	int pointCounter;
	
	//variables needed for undo
	PickedPointTreeWidgetItem *lastPointToMove;
	Point3m lastPointPosition;
	Point3m lastPointNormal;
	bool recordPointForUndo;
	
	QString templateWorkingDirectory;
	
private slots:
	//remove the point highlighted in the pickedPointTree
	void removeHighlightedPoint();
	
	//rename the point highlighted in the pickedPointTree
	void renameHighlightedPoint();
	
	//clear the point highlighted in the pickedPointTree
	void clearHighlightedPoint();
	
	//change mode to pick mode
	void togglePickMode(bool checked);
	
	//change mode to move mode
	void toggleMoveMode(bool checked);
	
	//change mode to select mode
	void toggleSelectMode(bool checked);
	
	//save the points to a file
	void savePointsToFile();
	
	//ask the user for the filename and then load the points
	void askUserForFileAndLoadPoints();
	
	//clear the points when the clear button is clicked
	void clearPointsButtonClicked();
	
	//save the point names currently listed as a template
	void savePointTemplate();
	
	//ask user for which template and then load a point template
	void askUserForFileAndloadTemplate();
	
	//clear the loaded template if one is loaded when the user clicks the clear
	//template button
	void clearTemplateButtonClicked();
	
	//Add a point to the loaded template.  When in template 
	//mode the default is to now allow extra points
	void addPointToTemplate();
	
	//undo the last move
	void undo();
};

//because QT is really dumb and TreeWidgetItems can receive signals
class TreeCheckBox : public QCheckBox 
{
	Q_OBJECT
public:
	TreeCheckBox(QWidget * parent, PickedPointTreeWidgetItem *_twi, PickPointsDialog *_ppd) : QCheckBox(parent)
	{
		twi = _twi;
		ppd = _ppd;
	}

public slots:
	void toggleAndDraw ( bool checked )
	{
		//qDebug() << "toggled!";
		twi->toggleActive(checked);
		ppd->redrawPoints();
	}
	
private:
	PickedPointTreeWidgetItem *twi;
	
	PickPointsDialog *ppd;
	
};

#endif
