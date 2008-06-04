/* A class to represent the ui for the morpher plugin
 *  
 * @author Oscar Barney
 */

#include <GL/glew.h>

#include <QtGui>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/glarea.h>

#include "morpher.h"
#include "morpherDialog.h"


void MeshModelTreeWidgetItem::setMeshModel(MeshModel *mm){
	meshModel = mm;
}
	
MeshModel * MeshModelTreeWidgetItem::getMeshModel(){
		return meshModel;
}


MorpherDialog::MorpherDialog(MorpherPlugin *plugin,
		QWidget *parent) : QDockWidget(parent)    
{ 
	parentPlugin = plugin;
	
	//qt standard setup step
	MorpherDialog::ui.setupUi(this);

	//setup borrowed from alighnDialog.cpp
	this->setWidget(ui.frame);
	this->setFeatures(QDockWidget::AllDockWidgetFeatures);
	this->setAllowedAreas(Qt::LeftDockWidgetArea);
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setFloating(true);
	this->setGeometry(p.x()+(parent->width()-width()),p.y()+40,width(),height() );	
	
	glArea = 0;
	sourceVertexVector = 0;
	sourceMeshModel = 0;
	destMeshModel = 0;
	
	QStringList headerNames;
	headerNames << "Mesh Name" << "Number of Vertices";
		
	ui.layerTreeWidget->setHeaderLabels(headerNames);

	
	
	//signals and slots
	connect(ui.startSpinBox, SIGNAL(valueChanged(int)), this, SLOT(startValueChanged(int)));
	connect(ui.endSpinBox, SIGNAL(valueChanged(int)), this, SLOT(endValueChanged(int)));
	connect(ui.pickSourceMeshButton, SIGNAL(clicked()), this, SLOT(pickSourceMesh()));
	connect(ui.pickDestMeshButton, SIGNAL(clicked()), this, SLOT(pickDestMesh()));
	connect(ui.morphSlider, SIGNAL(sliderMoved(int)), this, SLOT(sliderChanged(int)));
	//connect(ui.morphSlider, SIGNAL(sliderReleased()), this, SLOT(recalculateMorph()));
	
}

MorpherDialog::~MorpherDialog(){
	//delete the vertices that we had
	if(NULL != sourceVertexVector){
		delete sourceVertexVector;
	}
}

void MorpherDialog::setCurrentGLArea(GLArea *gla){
	glArea = gla;
	
	//reset any morphs we may have been doing
	sourceMeshModel = 0;
	destMeshModel = 0;

	ui.souceLabel->setText(":");
	ui.destLabel->setText(":");
	
	//now clear the tree start over
	ui.layerTreeWidget->clear();
	
	//get the current items
	QList<MeshModel *> &meshList = gla->meshDoc.meshList;
	qDebug("Items in list: %d", meshList.size());
	
	for(int i=0; i<meshList.size(); ++i)
	{
		MeshModelTreeWidgetItem *item = new MeshModelTreeWidgetItem();
		
		MeshModel *meshModel = meshList.at(i);
		item->setMeshModel(meshModel);
		
		//fileName
		item->setText(0, QFileInfo(meshList.at(i)->fileName.c_str()).fileName());
		
		//number of vertices
		int numberVertices = meshModel->cm.vn;
		QString numberString;
		numberString.setNum(numberVertices);
		item->setText(1, numberString);
		
		//add the new item
		ui.layerTreeWidget->addTopLevelItem(item);
	}
}

GLArea * MorpherDialog::getCurrentGLArea(){
	return glArea;
}

void MorpherDialog::verifyKeepChanges()
{
	QMessageBox::StandardButton ret = QMessageBox::question(
			this,  "MeshLab", "Do you want to keep all changes?",
			QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if(ret == QMessageBox::No)
	{
		//set the morph back to zero
		ui.morphSlider->setSliderPosition(0);
		recalculateMorph();
	}
}



void MorpherDialog::startValueChanged(int newValue){
	//qDebug() << "New start Value: " << newValue;
	ui.morphSlider->setMinimum(newValue);
}
	
void MorpherDialog::endValueChanged(int newValue){
	//qDebug() << "New end Value: " << newValue;
	ui.morphSlider->setMaximum(newValue);
}

void MorpherDialog::pickSourceMesh(){
	QTreeWidgetItem *item = ui.layerTreeWidget->currentItem();
	
	//test to see if there is actually a highlighted item
	if(NULL != item){
		MeshModelTreeWidgetItem *widgetItem =
			dynamic_cast<MeshModelTreeWidgetItem *>(item);
		
		MeshModel *tempModel = widgetItem->getMeshModel(); 
		
		if(NULL != destMeshModel && destMeshModel->cm.vn != tempModel->cm.vn){
			ui.souceLabel->setText("Vert. Number !=");
		} else {		
			//delete the vertices that we had
			if(NULL != sourceVertexVector){
				delete sourceVertexVector;
			}
			
			sourceMeshModel = tempModel;
		
			//now copy all the vertices
			sourceVertexVector = new std::vector<CVertexO>(tempModel->cm.vert);
			
			ui.souceLabel->setText(QFileInfo(sourceMeshModel->fileName.c_str()).fileName());
		}
	}
}
	
void MorpherDialog::pickDestMesh(){
	QTreeWidgetItem *item = ui.layerTreeWidget->currentItem();
		
	//test to see if there is actually a highlighted item
	if(NULL != item){
		MeshModelTreeWidgetItem *widgetItem =
			dynamic_cast<MeshModelTreeWidgetItem *>(item);
			
		MeshModel *tempModel = widgetItem->getMeshModel();
				
		if(NULL != sourceMeshModel && sourceMeshModel->cm.vn != tempModel->cm.vn){
			destMeshModel = 0;
			ui.destLabel->setText("Vert. Number !=");
		} else {
			destMeshModel = tempModel;
			ui.destLabel->setText(QFileInfo(destMeshModel->fileName.c_str()).fileName());
		}
	}
}


//called when the slider's value changes (update the label for now)
void MorpherDialog::sliderChanged(int newValue){
	//qDebug() << "Slider is changed to " << newValue;
	ui.sliderValueLabel->setNum(newValue);
	recalculateMorph();
}

void MorpherDialog::recalculateMorph(){
	//if two models are not selected return
	if(NULL == sourceMeshModel || NULL == destMeshModel){
		qDebug() << "trying to calculate with a null model";
		return;
	}
	
	//qDebug() << "Slider is now at " << ui.morphSlider->value() ; 

	float morphRatio = ui.morphSlider->value()/100.0;
	//qDebug() << "Ratio is: " << morphRatio;
	
	//make sure this will be a valid morph and that we didnt loose any vertices
	//since these meshes were picked
	if(sourceMeshModel->cm.vn == destMeshModel->cm.vn)
	{
		std::vector<CVertexO> &destVertexVector = destMeshModel->cm.vert;
		std::vector<CVertexO> &vertexToChangeVector = sourceMeshModel->cm.vert;
		
		for(int i=0; i < sourceVertexVector->size(); i++)
		{
		/*
			qDebug() << "before changing x: " << vertexToChangeVector.at(i).P()[0]
			         << " y: " << vertexToChangeVector.at(i).P()[1]
			         << " z: " << vertexToChangeVector.at(i).P()[2];

		*/
			
			//get new x
			vertexToChangeVector.at(i).P()[0] = calcNewPoint(sourceVertexVector->at(i).P()[0],
								destVertexVector.at(i).P()[0], morphRatio);
			//get new y
			vertexToChangeVector.at(i).P()[1] = calcNewPoint(sourceVertexVector->at(i).P()[1],
								destVertexVector.at(i).P()[1], morphRatio);
			//get new z
			vertexToChangeVector.at(i).P()[2] = calcNewPoint(sourceVertexVector->at(i).P()[2],
								destVertexVector.at(i).P()[2], morphRatio);
		/*
			qDebug() << "after changing x: " << vertexToChangeVector.at(i).P()[0]
					 << " y: " << vertexToChangeVector.at(i).P()[1]
					 << " z: " << vertexToChangeVector.at(i).P()[2];
		*/
			
			//now adjust the normals
			//get new x for normal
			vertexToChangeVector.at(i).N()[0] = calcNewPoint(sourceVertexVector->at(i).N()[0],
								destVertexVector.at(i).N()[0], morphRatio);
			//get new y for normal
			vertexToChangeVector.at(i).N()[1] = calcNewPoint(sourceVertexVector->at(i).N()[1],
								destVertexVector.at(i).N()[1], morphRatio);
			//get new z for normal
			vertexToChangeVector.at(i).N()[2] = calcNewPoint(sourceVertexVector->at(i).N()[2],
								destVertexVector.at(i).N()[2], morphRatio);
			
			//normalize the normal
			vertexToChangeVector.at(i).N().Normalize();
		}
		
		//qDebug() << "NOW, fix face normals for the mesh so that the lighting is correct";
		
		assert(sourceMeshModel->cm.face.size()==size_t(sourceMeshModel->cm.fn));
		
		for(int i=0; i<sourceMeshModel->cm.face.size(); ++i)
		{
			//NOTE: this if/else is coppied from vcg library's import_obj.h line 559
			if ( sourceMeshModel->ioMask & vcg::tri::io::Mask::IOM_WEDGNORMAL )
			{
				// face normal is computed as an average of wedge normals
				sourceMeshModel->cm.face[i].N().Import( sourceMeshModel->cm.face[i].WN(0) +
						sourceMeshModel->cm.face[i].WN(1) + sourceMeshModel->cm.face[i].WN(2) );
			} else
			{
				// computing face normal from position of face vertices
				vcg::face::ComputeNormalizedNormal(sourceMeshModel->cm.face[i]);
			}
		}

		//update the scene to show the new vertex positions
		glArea->update();
		
	} else {
		sourceMeshModel = 0;
		destMeshModel = 0;
		
		ui.souceLabel->setText("Vert. Number !=");
		ui.destLabel->setText("Vert. Number !=");
	}
	
}
