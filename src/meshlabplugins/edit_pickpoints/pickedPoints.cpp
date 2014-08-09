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
const QString PickedPoints::fileExtension = ".pp";
const QString PickedPoints::rootName = "PickedPoints";

const QString PickedPoints::documentDataElementName = "DocumentData";
const QString PickedPoints::dateTimeElementName = "DateTime";
const QString PickedPoints::date = "date";
const QString PickedPoints::time = "time";
const QString PickedPoints::userElementName = "User";
const QString PickedPoints::dataFileElementName = "DataFileName";
const QString PickedPoints::templateElementName = "templateName";

const QString PickedPoints::pointElementName = "point";
const QString PickedPoints::name = "name";
const QString PickedPoints::active = "active";
const QString PickedPoints::xCoordinate = "x";
const QString PickedPoints::yCoordinate = "y";
const QString PickedPoints::zCoordinate = "z";
const QString PickedPoints::True = "1";
const QString PickedPoints::False = "0";

const std::string PickedPoints::Key = "PickedPoints";


PickedPoints::PickedPoints()
:pointVector()
{
}

PickedPoints::~PickedPoints()
{
        for(size_t ii = 0; ii < pointVector.size();++ii)  
            delete pointVector[ii];
        pointVector.clear();

}

bool PickedPoints::open(QString filename){
	QDomDocument doc;
	pointVector.clear();
	
	QFile file(filename);
	
	QString errorMessage;
	if (file.open(QIODevice::ReadOnly) && doc.setContent(&file, &errorMessage)) 
	{
		file.close();
		QDomElement root = doc.documentElement();
		if (root.nodeName() == rootName) 
	    {
			qDebug() << "About to read a " << rootName << " xml document";
			
			templateName = "";
			QDomElement dataElement = root.firstChildElement(documentDataElementName);
			if(!dataElement.isNull()){
				QDomElement templateElement = dataElement.firstChildElement(templateElementName);
				if(!templateElement.isNull()) templateName = templateElement.attribute(name);
			} 
			
			qDebug() << "Template loaded: " << templateName;
			
			for(QDomElement element = root.firstChildElement(pointElementName);
				!element.isNull();
				element = element.nextSiblingElement(pointElementName))
			{
				QString pointName = element.attribute(name);
				qDebug() << "Reading point with name " << pointName;
				
				QString x = element.attribute(xCoordinate);
				QString y = element.attribute(yCoordinate);
				QString z = element.attribute(zCoordinate);
				
				Point3m point(x.toDouble(), y.toDouble(), z.toDouble());
				
				QString presentString = element.attribute(active);
				bool present = true;
				if(False == presentString) present = false;
				
				addPoint(pointName, point, present);
				
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



bool PickedPoints::save(QString filename, QString dataFileName){
	QDomDocument doc(rootName);
	QDomElement root = doc.createElement(rootName);
	doc.appendChild(root);
	
	//put in the template name
	QDomElement dataTag = doc.createElement(documentDataElementName);
	root.appendChild(dataTag);
	
	//put in the template name
	QDomElement data = doc.createElement(dateTimeElementName);
	data.setAttribute(date, QDate::currentDate().toString(Qt::ISODate));
	data.setAttribute(time, QTime::currentTime().toString(Qt::ISODate));
	dataTag.appendChild(data);

	char *user = 0;
	user = getenv("USERNAME"); //windows xp/vista
	if(NULL == user) user = getenv("LOGNAME"); //linux and maybe apple? 
	if(NULL != user)
	{
		data = doc.createElement(userElementName);
		data.setAttribute(name, QString(user));
		dataTag.appendChild(data);
	}
	
	data = doc.createElement(dataFileElementName);
	data.setAttribute(name, dataFileName);
	dataTag.appendChild(data);
	
	data = doc.createElement(templateElementName);
	data.setAttribute(name, templateName);
	dataTag.appendChild(data);
		
	//create an element for each point
	for (int i = 0; i < pointVector.size(); ++i) {
		PickedPoint *pickedPoint = pointVector.at(i);
		
		QDomElement tag = doc.createElement(pointElementName);
		
		Point3m point = pickedPoint->point;
		
		tag.setAttribute(xCoordinate, point[0] );
		tag.setAttribute(yCoordinate, point[1] );
		tag.setAttribute(zCoordinate, point[2] );
		
		//if the point is not present indicate this
		if(pickedPoint->present)
			tag.setAttribute(active, QString(True));
		else 
			tag.setAttribute(active, QString(False));
		
		tag.setAttribute(name,  pickedPoint->name);
		
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

void PickedPoints::addPoint(QString name, Point3m point, bool present)
{	
	PickedPoint *pickedPoint = new PickedPoint(name, point, present);
	pointVector.push_back(pickedPoint);
}

std::vector<PickedPoint*>& PickedPoints::getPickedPointVector()
{
	return pointVector;
}

std::vector<Point3m> * PickedPoints::getPoint3Vector()
{
	std::vector<Point3m> *points = new std::vector<Point3m>();
	
	for(size_t i = 0; i < pointVector.size(); i++)
	{
		if(pointVector.at(i)->present )
			points->push_back(pointVector.at(i)->point);
	}
	
	return points;
}

void PickedPoints::translatePoints(Matrix44m &translation)
{
	for(size_t i = 0; i < pointVector.size(); i++)
	{
		PickedPoint* temp = pointVector.at(i);
	
		//qDebug() << " point was x" << temp->point[0] << " y " << temp->point[1] << " z " << temp->point[2];
		
		Point4m inputPoint(temp->point[0], temp->point[1], temp->point[2], 1);
		
		Point4m resultPoint = translation * inputPoint;
		
		temp->point[0] = resultPoint[0];
		temp->point[1] = resultPoint[1];
		temp->point[2] = resultPoint[2];
		
		//qDebug() << " point is now x" << temp->point[0] << " y " << temp->point[1] << " z " << temp->point[2];
	}
}

QString PickedPoints::getSuggestedPickedPointsFileName(const MeshModel &meshModel){
    QString outputFileName(meshModel.shortName());
	
	//remove postfix
	outputFileName.truncate(outputFileName.length()-4);
				
	//add new postfix
	outputFileName.append("_picked_points" + fileExtension);
	
	return outputFileName;
}

void PickedPoints::setTemplateName(QString name)
{
	templateName = name;
}

const QString & PickedPoints::getTemplateName()
{
	return templateName;
}
