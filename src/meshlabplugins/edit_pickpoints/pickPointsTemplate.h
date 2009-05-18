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

/*  A class representing a set of point names that is used
 * as a template a user can fill out to create a PickedPoints
 * 
 * 
 * @author Oscar Barney
 */

#include <QString>
#include <vector>

class PickPointsTemplate
{
public:
	
	static bool save(QString filename, std::vector<QString> *pointNameVector);
	
	static bool load(QString filename, std::vector<QString> *pointNameVector);
	
	static QString getDefaultTemplateFileName();
	
	//extension of the filetype for PickPointsTemplate
	static const QString fileExtension;
	
private:
	PickPointsTemplate(){};
	
	//root name of the xml document
	static const QString rootName;

	//point element's name
	static const QString pointElementName;

	//point's name
	static const QString pointName;

};
