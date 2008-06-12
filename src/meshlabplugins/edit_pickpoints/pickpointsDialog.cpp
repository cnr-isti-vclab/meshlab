/* A class to represent the ui for the pickpoints plugin
 *  
 * @author Oscar Barney
 */

#include <QtGui>

#include <math.h>

#include <meshlab/stdpardialog.h>
#include <meshlab/meshmodel.h>

//#include <vcg/complex/trimesh/closest.h>
//#include <vcg/space/index/grid_static_ptr.h>

#include "editpickpoints.h"
#include "pickpointsDialog.h"

using namespace vcg;

int PickedPointTreeWidgetItem::pointCounter = 0;

PickedPointTreeWidgetItem::PickedPointTreeWidgetItem(
		Point3f intputPoint, CFaceO::NormalType faceNormal) : QTreeWidgetItem(1001) {
	point = intputPoint;
	
	normal[0] = faceNormal[0];
	normal[1] = faceNormal[1];
	normal[2] = faceNormal[2];
	
	QString tempString;
	tempString.setNum(pointCounter);
	
	//name
	setText(0, tempString);
	pointCounter++;
	
	//x
	tempString.setNum(point[0]);
	setText(1, tempString);
	
	//y
	tempString.setNum(point[1]);
	setText(2, tempString);
	
	//z
	tempString.setNum(point[2]);
	setText(3, tempString);
	
	isPointSet = true;
}

PickedPointTreeWidgetItem::PickedPointTreeWidgetItem(QString name){
	isPointSet = false;
	
	setText(0, name);
}
		
void PickedPointTreeWidgetItem::setName(QString name){
	setText(0, name);
}

QString PickedPointTreeWidgetItem::getName(){
	return text(0);
}

void PickedPointTreeWidgetItem::setPoint(Point3f intputPoint){
	point = intputPoint;
	
	QString tempString;
	//x
	tempString.setNum(point[0]);
	setText(1, tempString);
		
	//y
	tempString.setNum(point[1]);
	setText(2, tempString);
	
	//z
	tempString.setNum(point[2]);
	setText(3, tempString);
	
	isPointSet = true;
}

void PickedPointTreeWidgetItem::setNormal(CFaceO::NormalType faceNormal)
{
	normal[0] = faceNormal[0];
	normal[1] = faceNormal[1];
	normal[2] = faceNormal[2];
}

vcg::Point3f PickedPointTreeWidgetItem::getPoint(){
	return point;
}

vcg::Point3f PickedPointTreeWidgetItem::getNormal(){
	return normal;
}

void PickedPointTreeWidgetItem::clearPoint(){
	point.Zero();
	isPointSet = false;

	//x
	setText(1, "");
			
	//y
	setText(2, "");
		
	//z
	setText(3, "");	
}

bool PickedPointTreeWidgetItem::isSet(){
	return isPointSet;
}

PickPointsDialog::PickPointsDialog(EditPickPointsPlugin *plugin,
		QWidget *parent) : QDockWidget(parent)    
{ 
	parentPlugin = plugin;
	
	//qt standard setup step
	PickPointsDialog::ui.setupUi(this);

	//setup borrowed from alighnDialog.cpp
	this->setWidget(ui.frame);
	this->setFeatures(QDockWidget::AllDockWidgetFeatures);
	this->setAllowedAreas(Qt::LeftDockWidgetArea);
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setFloating(true);
	this->setGeometry(p.x()+(parent->width()-width()),p.y()+40,width(),height() );

	//now stuff specific to pick points
	QStringList headerNames;
	headerNames << "Point Name" << "X" << "Y" << "Z";
	
	ui.pickedPointsTreeWidget->setHeaderLabels(headerNames);
	
	//init some variables
	
	//set to nothing for now
	itemToMove = 0;
	meshModel = 0;
	
	//start with no template
	templateLoaded = false;
	ui.templateNameLabel->setText("No Template Loaded");
	
	currentMode = ADD_POINT;
	
	//signals and slots
	connect(ui.removeHighlightedButton, SIGNAL(clicked()), this, SLOT(removeHighlightedPoint()));
	connect(ui.renamePointButton, SIGNAL(clicked()), this, SLOT(renameHighlightedPoint()));
	connect(ui.pickPointModeRadioButton, SIGNAL(toggled(bool)), this, SLOT(togglePickMode(bool)));
	connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(savePointsToFile()));
	connect(ui.loadPointsButton, SIGNAL(clicked()), this, SLOT(askUserForFileAndLoadPoints()));
	connect(ui.clearButton, SIGNAL(clicked()), this, SLOT(clearPoints()));
	connect(ui.saveTemplateButton, SIGNAL(clicked()), this, SLOT(savePointTemplate()));
	connect(ui.loadTemplateButton, SIGNAL(clicked()), this, SLOT(askUserForFileAndloadTemplate()));
	connect(ui.clearTemplateButton, SIGNAL(clicked()), this, SLOT(clearPickPointsTemplate()));
	connect(ui.addPointToTemplateButton, SIGNAL(clicked()), this, SLOT(addPointToTemplate()));
	connect(ui.removePointFromTemplateButton, SIGNAL(clicked()), this, SLOT(removePointFromTemplate()));
	
}

PickedPointTreeWidgetItem * PickPointsDialog::addPoint(Point3f point, CFaceO::NormalType faceNormal){
	
	PickedPointTreeWidgetItem *widgetItem;
	if(currentMode == ADD_POINT){
		
		//if we are in template mode
		if(templateLoaded){
			QTreeWidgetItem *item = ui.pickedPointsTreeWidget->currentItem();
				
			//test to see if there is actually a highlighted item
			if(NULL != item){
				widgetItem =
					dynamic_cast<PickedPointTreeWidgetItem *>(item);

				widgetItem->setPoint(point);
				widgetItem->setNormal(faceNormal);
				
				item = ui.pickedPointsTreeWidget->itemBelow(widgetItem);
				if(NULL != item){
					//set the next item to be selected
					ui.pickedPointsTreeWidget->setCurrentItem(item);
				} else
				{
					//if we just picked the last point go into move mode
					togglePickMode(false);
				}
			}
		} else {
			widgetItem =
				new PickedPointTreeWidgetItem(point, faceNormal);
		
			pickedPointTreeWidgetItemVector.push_back(widgetItem);
	
			ui.pickedPointsTreeWidget->addTopLevelItem(widgetItem);
			//select the newest item
			ui.pickedPointsTreeWidget->setCurrentItem(widgetItem);
		}
	} else if(currentMode == MOVE_POINT)
	{
		//test to see if there is actually a highlighted item
		if(NULL != itemToMove){
			itemToMove->setPoint(point);
			itemToMove->setNormal(faceNormal);
		}
	}
	return widgetItem;
}

void PickPointsDialog::moveThisPoint(Point3f point){
	qDebug() << "point is: " << point[0] << " " << point[1] << " " << point[2];
	
	//the item closest to the given point
	PickedPointTreeWidgetItem *closestItem = 0;
	
	//the smallest distance from the given point to one in the list
	//so far....
	float minDistanceSoFar = -1.0;
	
	for(int i = 0; i < pickedPointTreeWidgetItemVector.size(); i++){
		PickedPointTreeWidgetItem *item =
			pickedPointTreeWidgetItemVector.at(i);
		
		Point3f tempPoint = item->getPoint();
		
		qDebug() << "tempPoint is: " << tempPoint[0] << " " << tempPoint[1] << " " << tempPoint[2];
		
		float temp = sqrt(pow(point[0]-tempPoint[0],2) +
						pow(point[1]-tempPoint[1],2) + 
						pow(point[2]-tempPoint[2],2));
		qDebug() << "distance is: " << temp;
		
		if(minDistanceSoFar < 0 || minDistanceSoFar > temp){
			minDistanceSoFar = temp;
			closestItem = item;
		}
	}
	
	//if we found an itme
	if(NULL != closestItem){
		itemToMove = closestItem;
		
		qDebug() << "Try to move: " << closestItem->getName();
	}
	
}

void PickPointsDialog::addPoint(Point3f point, QString name){
	/*
		if(NULL != meshModel)
		{	
			CMeshO::ScalarType maxDistance = meshModel->cm.bbox.Diag()/100.0f;
			
			Point3f closestPoint;
			
			vcg::face::PointDistanceFunctor PDistFunct;
			
			//closest point??
			Point3f closestPt;
			
			trimesh::FaceTmark<CMeshO> markerFunction;
			markerFunction.SetMesh(&meshModel->cm);
			
			GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType> grid;
			grid.Set(meshModel->cm.face.begin(), meshModel->cm.face.end());
			
			CMeshO::FaceType *face = 0;		
			face = grid.GetClosest(PDistFunct,markerFunction,point,maxDistance,maxDistance,closestPoint);
			
			if(NULL != face)
			{
				CMeshO::FaceType::NormalType faceNormal = face->N();
				widgetItem->setNormal(faceNormal);
				
				//TODO now add htis point to the item below
			} else
				qDebug() << "no face found for point: " << name;	
		}*/
	
	PickedPointTreeWidgetItem *widgetItem = addPoint(point, vcg::Point3f());
	widgetItem->setName(name);
}

void PickPointsDialog::clearPoints(){
	if(templateLoaded){
		//when using templates just clear the points that were picked but not the names
		for(int i = 0; i < pickedPointTreeWidgetItemVector.size(); i++){
			pickedPointTreeWidgetItemVector.at(i)->clearPoint();
		}
		//if the size is greater than 0 set the first point to be selected
		if(pickedPointTreeWidgetItemVector.size() > 0){
			ui.pickedPointsTreeWidget->setCurrentItem(
					pickedPointTreeWidgetItemVector.at(0));
		}
		
	} else {
		clearAllPointsFromTreeWidget();
	}
}

void PickPointsDialog::clearAllPointsFromTreeWidget(){
	pickedPointTreeWidgetItemVector.clear();
		
	ui.pickedPointsTreeWidget->clear();
		
	PickedPointTreeWidgetItem::pointCounter = 0;
	
	togglePickMode(true);
}


void PickPointsDialog::loadPickPointsTemplate(QString filename){
	//clear the points tree
	clearAllPointsFromTreeWidget();
	
	std::vector<QString> pointNameVector;
	
	PickPointsTemplate::load(filename, &pointNameVector);
	
	for(int i = 0; i < pointNameVector.size(); i++){
		PickedPointTreeWidgetItem *widgetItem =
			new PickedPointTreeWidgetItem(pointNameVector.at(i));
			
		pickedPointTreeWidgetItemVector.push_back(widgetItem);

		ui.pickedPointsTreeWidget->addTopLevelItem(widgetItem);
	}
	
	//select the first item in the list if it exists
	if(pickedPointTreeWidgetItemVector.size() > 0){
		ui.pickedPointsTreeWidget->setCurrentItem(pickedPointTreeWidgetItemVector.at(0));
	}
	
	ui.templateNameLabel->setText(QFileInfo(filename).fileName());
	templateLoaded = true;
}

std::vector<PickedPointTreeWidgetItem*>& PickPointsDialog::getPickedPointTreeWidgetItemVector(){
	return pickedPointTreeWidgetItemVector;
}

PickPointsDialog::Mode PickPointsDialog::getMode(){
	return currentMode;
}

void PickPointsDialog::setCurrentMeshModel(MeshModel *newMeshModel){
	//qDebug() << "about to save any points we had before ";
	
	//save any points we did have
	savePointsToMetaData(getPickedPoints());
	
	//qDebug() << "done saving points about to clear ";
	
	clearPoints();
	
	meshModel = newMeshModel;
	
	//dont bother with the rest if mesh model is null
	if(NULL == meshModel){
		return;
	}
	
	//make sure we start in pick mode
	ui.pickPointModeRadioButton->setChecked(true);
	
	/*
	//Load the points from meta data if they are there
	MeshMetaDataInterface *value = meshModel->metaData.value(PickedPoints::getKey());
		
	if(NULL != value){
		PickedPoints *pickedPoints = static_cast<PickedPoints *>(value);
		
		if(NULL != pickedPoints){
			std::vector<PickedPoint*> * pickedPointVector = pickedPoints->getPickedPointVector();
			
			PickedPoint *point;
			for(int i = 0; i < pickedPointVector->size(); i++){
				point = pickedPointVector->at(i);
				addPoint(point->point, point->name);
			}
			
			//draw any points that may have been loaded
			parentPlugin->drawPickedPoints(pickedPointTreeWidgetItemVector);
		} else {
			qDebug() << "problem with cast!!";
		}
		
	} else {
	
		QString filename = PickedPoints::getSuggestedPickedPointsFileName(*meshModel);
		QFile file(filename);
		
		if(file.exists()){
			loadPoints(filename);
		} else 
		{
	*/
	//try loading the default template if there are not saved points already
	tryLoadingDefaultTemplate();
		
	//	}
	//}
}

MeshModel* PickPointsDialog::getCurrentMeshModel(){
	return meshModel;
}

//loads the default template if there is one
void PickPointsDialog::tryLoadingDefaultTemplate()
{
	QString filename = PickPointsTemplate::getDefaultTemplateFileName();
	QFile file(filename);
	
	if(file.exists()){
		loadPickPointsTemplate(filename);
	}
}

void PickPointsDialog::removeHighlightedPoint(){
	//get highlighted point
	QTreeWidgetItem *item = ui.pickedPointsTreeWidget->currentItem();
	
	//test to see if there is actually a highlighted item
	if(NULL != item){
		PickedPointTreeWidgetItem* pickedItem =
			dynamic_cast<PickedPointTreeWidgetItem *>(item);
		
		if(templateLoaded)
			pickedItem->clearPoint();
		else 
		{
			//remove the point completely
			std::vector<PickedPointTreeWidgetItem*>::iterator iterator;
			iterator = std::find(pickedPointTreeWidgetItemVector.begin(),
					pickedPointTreeWidgetItemVector.end(),
					pickedItem);
			//remove item from vector
			pickedPointTreeWidgetItemVector.erase(iterator);
				
			//free memory used by widget
			delete pickedItem;
		}
		
		//redraw without deleted point
		parentPlugin->drawPickedPoints(pickedPointTreeWidgetItemVector, meshModel->cm.bbox);
	} else
	{
		qDebug("no item picked");
	}
	
}

void PickPointsDialog::renameHighlightedPoint(){
	//get highlighted point
	QTreeWidgetItem *item = ui.pickedPointsTreeWidget->currentItem();
		
	//test to see if there is actually a highlighted item
	if(NULL != item){
			PickedPointTreeWidgetItem* pickedItem =
				dynamic_cast<PickedPointTreeWidgetItem *>(item);
			
		QString name = pickedItem->getName();
		//qDebug("Rename \n");
		//qDebug() << name;

		const QString newName = "newName";
		
		FilterParameterSet parameterSet;
		parameterSet.addString(newName, name, "New Name", "Enter the new name");
	
		GenericParamDialog getNameDialog(this,&parameterSet);
		
		//display dialog
		int result = getNameDialog.exec();
		if(result == QDialog::Accepted){
			name = parameterSet.getString(newName);
			//qDebug("New name os \n");
			//qDebug() << name;
			
			pickedItem->setName(name);
		
			//redraw with new point name
			parentPlugin->drawPickedPoints(pickedPointTreeWidgetItemVector, meshModel->cm.bbox);
		}
		
	}
}

void PickPointsDialog::togglePickMode(bool checked){
	
	//qDebug() << "Toggle pick mode " << checked;
	
	if(checked){
		currentMode = ADD_POINT;
		//make sure radio button reflects this change
		ui.pickPointModeRadioButton->setChecked(true);
	} else {
		currentMode = MOVE_POINT;
		//make sure radio button reflects the change
		ui.movePointRadioButton->setChecked(true);
	}
}

PickedPoints * PickPointsDialog::getPickedPoints()
{
	PickedPoints *pickedPoints = new PickedPoints();
	//add all the points
	for(int i = 0; i < pickedPointTreeWidgetItemVector.size(); i++){
		PickedPointTreeWidgetItem *item =
			pickedPointTreeWidgetItemVector.at(i);
		
		pickedPoints->addPoint(item->getName(), item->getPoint() );
	}
	return pickedPoints;
}

void PickPointsDialog::loadPoints(QString filename){
	//clear the points tree and template in case it was loaded
	clearPickPointsTemplate();
		
	//get the points from file
	PickedPoints pickedPoints;
	pickedPoints.open(filename);
	
	std::vector<PickedPoint*> *points = pickedPoints.getPickedPointVector();
	
	for(int i = 0; i < points->size(); i++){
		PickedPoint *pickedPoint = points->at(i);
		
		addPoint(pickedPoint->point, pickedPoint->name);		
	}
	
	//redraw with new point name
	parentPlugin->drawPickedPoints(pickedPointTreeWidgetItemVector, meshModel->cm.bbox);
}

void PickPointsDialog::savePointsToFile(){
	
	PickedPoints *pickedPoints = getPickedPoints();
	
	//save to a file if so desired and there are some points to save
	if(pickedPointTreeWidgetItemVector.size() > 0){

		QString suggestion(".");
		if(NULL != meshModel){
			suggestion = PickedPoints::getSuggestedPickedPointsFileName(*meshModel); 
		}
		QString filename = QFileDialog::getSaveFileName(this,tr("Save File"),suggestion, "*"+PickedPoints::fileExtension);
	
		pickedPoints->save(filename);
	}
	
	savePointsToMetaData(pickedPoints);
}

void PickPointsDialog::savePointsToMetaData(PickedPoints *pickedPoints)
{
	//save the points to the metadata
	if(NULL != meshModel){
		//meshModel->metaData.insert(PickedPoints::getKey(), pickedPoints);
		//qDebug() << "saved points";
	}	
}

void PickPointsDialog::askUserForFileAndLoadPoints()
{
	QString filename = QFileDialog::getOpenFileName(this,tr("Load File"),".", "*"+PickedPoints::fileExtension);
	loadPoints(filename);
}

void PickPointsDialog::savePointTemplate(){
	std::vector<QString> pointNameVector;

	//add all the points
	for(int i = 0; i < pickedPointTreeWidgetItemVector.size(); i++){
		PickedPointTreeWidgetItem *item =
			pickedPointTreeWidgetItemVector.at(i);
			
		pointNameVector.push_back(item->getName());
	}
	
	//default if for the filename to be that of the default template
	QString filename = PickPointsTemplate::getDefaultTemplateFileName();
	qDebug() << "default " << filename;
	if(!ui.defaultTemplateCheckBox->isChecked())
	{
		filename = QFileDialog::getSaveFileName(this,tr("Save File"),".", "*"+PickPointsTemplate::fileExtension);
	}
	PickPointsTemplate::save(filename, &pointNameVector);

	templateLoaded = true;
	ui.templateNameLabel->setText(QFileInfo(filename).fileName());
	
	if(ui.defaultTemplateCheckBox->isChecked())
	{
		QMessageBox::information(this,  "MeshLab", "Default Template Saved!",
		               		QMessageBox::Ok);
	}
}

void PickPointsDialog::askUserForFileAndloadTemplate()
{
	QString filename = QFileDialog::getOpenFileName(this,tr("Load File"),".", "*"+PickPointsTemplate::fileExtension);
	loadPickPointsTemplate(filename);
}

void PickPointsDialog::clearPickPointsTemplate(){
	templateLoaded = false;
	ui.templateNameLabel->setText("No Template Loaded");
	
	togglePickMode(true);
	
	//clear the points?
	clearPoints();
}

void PickPointsDialog::addPointToTemplate()
{
	if(templateLoaded){
		PickedPointTreeWidgetItem *widgetItem =
			new PickedPointTreeWidgetItem("new point");
					
		pickedPointTreeWidgetItemVector.push_back(widgetItem);
	
		ui.pickedPointsTreeWidget->addTopLevelItem(widgetItem);
		
		//select the newest item
		ui.pickedPointsTreeWidget->setCurrentItem(widgetItem);
	}
}

void PickPointsDialog::removePointFromTemplate()
{
	//get highlighted point
	QTreeWidgetItem *item = ui.pickedPointsTreeWidget->currentItem();
	
	//test to see if there is actually a highlighted item
	if(NULL != item && templateLoaded){
		PickedPointTreeWidgetItem* pickedItem =
				dynamic_cast<PickedPointTreeWidgetItem *>(item);
			
		QString name = pickedItem->getName();

		std::vector<PickedPointTreeWidgetItem*>::iterator iterator;
		iterator = std::find(pickedPointTreeWidgetItemVector.begin(),
				pickedPointTreeWidgetItemVector.end(),
				pickedItem);
		//remove item from vector
		pickedPointTreeWidgetItemVector.erase(iterator);
					
		//free memory used by widget
		delete pickedItem;
			
		//redraw without deleted point
		parentPlugin->drawPickedPoints(pickedPointTreeWidgetItemVector, meshModel->cm.bbox);
	} else
	{
		qDebug("no item picked");
	}
		

}
