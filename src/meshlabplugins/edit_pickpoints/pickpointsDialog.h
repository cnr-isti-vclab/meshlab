/* A class to represent the ui for the pickpoints plugin
 *  
 * @author Oscar Barney
 */

#ifndef PICKPOINTS_DIALOG_H
#define PICKPOINTS_DIALOG_H

#include <QtGui>

#include <meshlab/meshmodel.h>

#include "pickedPoints.h"
#include "ui_pickpointsDialog.h"
#include "pickPointsTemplate.h"

class EditPickPointsPlugin;

class PickedPointTreeWidgetItem : public QTreeWidgetItem
{

public:
	//used when a point has just been picked and
	//gives it an integer name
	PickedPointTreeWidgetItem(vcg::Point3f intputPoint);
	
	//used when a pont has not been picked yet at all
	//as when this tree widget item is created by loading
	//a template
	PickedPointTreeWidgetItem(QString name);
	
	//set the name
	void setName(QString name);
	
	//return the name of the point
	QString getName();
	
	//change the point
	void setPoint(vcg::Point3f intputPoint);
	
	//return the Point3f
	vcg::Point3f getPoint();
	
	//clear the ponint datas
	void clearPoint();
	
	//return if the point is set
	bool isSet();
	
	//basically just so we have a unique default name for new points that are picked
	static int pointCounter;
	
private:
	//the point
	vcg::Point3f point;
	
	//tells us if the point is set
	bool isPointSet;
};


class PickPointsDialog : public QDockWidget
{
	Q_OBJECT

public:
	PickPointsDialog(EditPickPointsPlugin *plugin,
			QWidget *parent = 0);

	enum Mode { ADD_POINT, MOVE_POINT };
	
	
	//add a point that was just picked(could be moving a point)
	PickedPointTreeWidgetItem * addPoint(vcg::Point3f point);
	
	//we need to move the point closest to this one
	void moveThisPoint(vcg::Point3f point);
	
	//add a new point and call it something 
	void addPoint(vcg::Point3f point, QString name);
	
	//return the vector
	//useful if you want to draw the points
	std::vector<PickedPointTreeWidgetItem*>& getPickedPointTreeWidgetItemVector();
	
	//return the mode of this ui
	PickPointsDialog::Mode getMode();
	
	
	//sets the currentMesh we are working with
	void setCurrentMeshModel(MeshModel *newMeshModel);
	
	//return the current mesh model
	MeshModel* getCurrentMeshModel();
	
private:
	//get the points from the UI
	PickedPoints * getPickedPoints();
	
	//allows the ability to save to metaData only even if the ui says save to a file
	void savePointsToMetaData(PickedPoints *pickedPoints);
		
	//makes the picked point tree widget empty
	void clearAllPointsFromTreeWidget();
	
	//the current mode of the GUI
	Mode currentMode;
	
	//QT patern - the ui pointer
	Ui::pickpointsDialog ui;

	//a map of the tree items to the points they represent
	std::vector<PickedPointTreeWidgetItem*> pickedPointTreeWidgetItemVector; 
	
	//the parrent plugin
	EditPickPointsPlugin *parentPlugin;
	
	//in MOVE_POINT Mode this holds a pointer to the item
	//found on mouse button down so that on mouse button
	//up it can be moved
	PickedPointTreeWidgetItem *itemToMove;
	
	//the template we have loaded
	bool templateLoaded;
	
	//we need this in order to save to the meta data and get the filename
	MeshModel *meshModel;
	
private slots:
	//remove the point highlighted in the pickedPointTree
	void removeHighlightedPoint();
	
	//rename the point highlighted in the pickedPointTree
	void renameHighlightedPoint();
	
	//move the point  highlighted in the pickedPointTree
	void togglePickMode(bool checked);
	
	//save the points to a file
	void savePointsToFile();
	
	//load the points from a file
	void loadPoints();
	
	//remove all the points and start over when ui button
	//is pressed
	void clearPoints();
	
	//save the point names currently listed as a template
	void savePointTemplate();
	
	//load a point template
	void loadPickPointsTemplate();
	
	//clear the loaded template if one is loaded
	void clearPickPointsTemplate();
	
	//Add a point to the loaded template.  When in template 
	//mode the default is to now allow extra points
	void addPointToTemplate();
	
	
};

#endif
