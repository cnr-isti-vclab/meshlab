/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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
/****************************************************************************
History
$Log$
Revision 1.3  2007/12/11 16:19:10  corsini
make private member variable with accessors

Revision 1.2  2007/12/03 10:14:59  corsini
restyling code


****************************************************************************/


#ifndef GLSTATE_H
#define GLSTATE_H

// Qt headers
#include <QString>
#include <QDomElement>

/**
 * Class to handle OpenGL state (e.g. name, OpenGL code).
 */
class GlState
{

// private data members
private:

	QString name;
	int state;
	int value;
	bool valid;

// constructor
public:

	GlState(QString _name, int _state, int _value)
	{
		name = _name;
		state = _state;
		value = _value;
		valid = true;
	}

	GlState(QDomElement xml)
	{
		bool ok1,ok2;
		name = xml.attribute("NAME");
		state = xml.attribute("STATE").toUInt( &ok1 );
		value = xml.attribute("VALUE").toUInt( &ok2 );
		
		valid = ( xml.tagName() == "RmState" && xml.attribute("API") == "OpenGL" && ok1 && ok2 );
	}

// accessors 
public:

	bool isValid(){return valid;}
	int getValue(){return value;}
	int getState(){return state;}
	QString getName(){return name;}
};

#endif  /* GLSTATE_H */

