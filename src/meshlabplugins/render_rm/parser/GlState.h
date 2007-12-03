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
Revision 1.2  2007/12/03 10:14:59  corsini
restyling code


****************************************************************************/


#ifndef __GLSTATE_H__
#define __GLSTATE_H__

#include <QString>
#include <QDomElement>

class GlState
{

public:

	QString name;
	int state;
	int value;
	bool valid;

	GlState( QString & _name, int _state, int _value )
	{
		name = _name;
		state = _state;
		value = _value;
		valid = true;
	}

	GlState( QDomElement xml ) 
	{
		bool ok1,ok2;
		name = xml.attribute("NAME");
		state = xml.attribute("STATE").toUInt( &ok1 );
		value = xml.attribute("VALUE").toUInt( &ok2 );
		
		valid = ( xml.tagName() == "RmState" && xml.attribute("API") == "OpenGL" && ok1 && ok2 );
	}

	bool isValid() { return valid; }
	int getValue() { return value; }
	int getState() { return state; }
	QString & getName() { return name; }
};

#endif

