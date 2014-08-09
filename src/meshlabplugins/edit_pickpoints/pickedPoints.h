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


#ifndef PICKED_POINTS_H
#define PICKED_POINTS_H

#include <common/meshmodel.h>

//our points
class PickedPoint
{
public:
	PickedPoint(QString _name, Point3m _point, bool _present){
		name = _name;
		point = _point;
		present = _present;
	}
	
	//name of point
	QString name;  
	
	//if we are using a template and a point we need to label does not exist we can use
	//this flag to indicate the point could not be picked
	bool present;

	//point
	Point3m point;
};

class PickedPoints
{
public:
	
	PickedPoints();
	
	~PickedPoints();
	
	//opens a file containing the picked points
	bool open(QString filename);
	
	//save 
	bool save(QString filename, QString dataFileName);

	//add a point to the map
	void addPoint(QString name, Point3m point, bool present);
	
	std::vector<PickedPoint*>& getPickedPointVector();
	
	//get a vector containing only active points
	std::vector<Point3m> * getPoint3Vector();
	
	//translate each point using the matrix
	//if the mesh moves you can then translate the points useing this function
	void translatePoints(Matrix44m &translation);
	
	//get the suggested filename for the points.  will be based on the mesh's filename
	static QString getSuggestedPickedPointsFileName(const MeshModel &meshModel);
	
	//set the template name
	void setTemplateName(QString name);
	
	//get the template name
	const QString & getTemplateName();
		
	//extension of the filetype for Picked Points
	static const QString fileExtension;
	
	//for use with per mesh attributes
	static const std::string Key;
	
private:
	
	//data
	std::vector<PickedPoint*> pointVector;
	
	//the template that was used to pick these points
	//will be "" if no template was used
	QString templateName;
	
	//Declare Constants
	
	//root name of the xml document
	static const QString rootName;
	
	static const QString documentDataElementName;
	
	static const QString dateTimeElementName;
	
	static const QString date;
	
	static const QString time;
	
	static const QString userElementName;
	
	//the file the points were placed on
	static const QString dataFileElementName;
	
	//the templateName if one was used
	static const QString templateElementName;
	
	//point element's name
	static const QString pointElementName;
	
	//point's name
	static const QString name;
	
	//tells whether the point is active
	//can really indicate whatever you want but is mostly used to indicate
	//whether the point should be used.  a point can be inactive if it is part of a 
	//template and could not be found or if you want to indicate that the value picked
	//is not reliable
	static const QString active;
	
	//point's x coord
	static const QString xCoordinate;
	
	//point's y coord
	static const QString yCoordinate;
	
	//points z coord
	static const QString zCoordinate;
	
	//to indicate whether the point is active
	static const QString True;
	
	//to indicate whether the point is inactive
	static const QString False;
	
	
	

	
};

#endif
