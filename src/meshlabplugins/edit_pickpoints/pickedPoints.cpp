/*  A class representing a set of points and the format
 * that we will be storing them
 * 
 * 
 * @author Oscar Barney
 */

#include "pickedPoints.h"

#include <QtGui>
#include <QMap>
#include <QList>

//xml stuff
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

//Define Constants
const QString PickedPoints::fileExtension = ".xml";
const QString PickedPoints::rootName = "PickedPoints";
const QString PickedPoints::pointElementName = "point";
const QString PickedPoints::pointName = "name";
const QString PickedPoints::xCoordinate = "x";
const QString PickedPoints::yCoordinate = "y";
const QString PickedPoints::zCoordinate = "z";

PickedPoints::PickedPoints(){
	pointVector = new std::vector<PickedPoint *>();
}

PickedPoints::~PickedPoints(){
	delete pointVector;
}

bool PickedPoints::open(QString filename){
	QDomDocument doc;
	pointVector->clear();
	
	QFile file(filename);
	
	QString errorMessage;
	if (file.open(QIODevice::ReadOnly) && doc.setContent(&file, &errorMessage)) 
	{
		file.close();
		QDomElement root = doc.documentElement();
		if (root.nodeName() == rootName) 
	    {
			qDebug() << "About to read a " << rootName << " xml document";
			
			for(QDomElement element = root.firstChildElement(pointElementName);
				!element.isNull();
				element = element.nextSiblingElement(pointElementName))
			{
				QString name = element.attribute(pointName);
				qDebug() << "Reading point with name " << name;
				
				QString x = element.attribute(xCoordinate);
				QString y = element.attribute(yCoordinate);
				QString z = element.attribute(zCoordinate);
				
				vcg::Point3f point(x.toFloat(), y.toFloat(), z.toFloat());
				
				addPoint(name, point);
				
			}
	    } else {
	    	//file is of unknown type
	    	qDebug() << "Failed, tried to read a " << rootName << " xml document";
	    	return false;
	    }
		
	} else {
		// problem opening the file
		qDebug() << "problem reading from the file, setContent error: " <<  errorMessage;
		return false;
	}
	return true;
}



bool PickedPoints::save(QString filename){
	QDomDocument doc(rootName);
	QDomElement root = doc.createElement(rootName);
	doc.appendChild(root);
	
	//create an element for each point
	for (int i = 0; i < pointVector->size(); ++i) {
		PickedPoint *pickedPoint = pointVector->at(i);
		
		QDomElement tag = doc.createElement(pointElementName);
		tag.setAttribute(pointName,  pickedPoint->name);
		vcg::Point3f point = pickedPoint->point;
		
		tag.setAttribute(xCoordinate, point[0] );
		tag.setAttribute(yCoordinate, point[1] );
		tag.setAttribute(zCoordinate, point[2] );
		
		//append the element to the root
		root.appendChild(tag);
	}
	
	//create a file and write the data
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QTextStream qstream(&file);
	doc.save(qstream,1);
	file.close();
	return true;	 
}

void PickedPoints::addPoint(QString name, vcg::Point3f point){
	if(NULL != pointVector){
		PickedPoint *pickedPoint = new PickedPoint(name,point);
		pointVector->push_back(pickedPoint);
	} else
	{
		qDebug("NULL pointVector!");
	}
}

std::vector<PickedPoint*> * PickedPoints::getPickedPointVector()
{
	return pointVector;
}

std::vector<vcg::Point3f> * PickedPoints::getPoint3fVector()
{
	std::vector<vcg::Point3f> *points = new std::vector<vcg::Point3f>();
	
	for(int i = 0; i < pointVector->size(); i++)
	{
		points->push_back(pointVector->at(i)->point);
	}
	
	return points;
}

void PickedPoints::translatePoints(vcg::Matrix44f &translation)
{
	for(int i = 0; i < pointVector->size(); i++)
	{
		PickedPoint* temp = pointVector->at(i);
	
		//qDebug() << " point was x" << temp->point[0] << " y " << temp->point[1] << " z " << temp->point[2];
		
		vcg::Point4f inputPoint(temp->point[0], temp->point[1], temp->point[2], 1);
		
		vcg::Point4f resultPoint = translation * inputPoint;
		
		temp->point[0] = resultPoint[0];
		temp->point[1] = resultPoint[1];
		temp->point[2] = resultPoint[2];
		
		//qDebug() << " point is now x" << temp->point[0] << " y " << temp->point[1] << " z " << temp->point[2];
	}
}

/*
const MeshMetaDataInterface::MetaDataType PickedPoints::getKey()
{
	return MeshMetaDataInterface::PICKED_POINTS;
}
*/

QString PickedPoints::getSuggestedPickedPointsFileName(const MeshModel &meshModel){
	QString outputFileName(meshModel.fileName.c_str());
	
	//remove postfix
	outputFileName.truncate(outputFileName.length()-4);
				
	//add new postfix
	outputFileName.append("_picked_points" + fileExtension);
	
	return outputFileName;
}

