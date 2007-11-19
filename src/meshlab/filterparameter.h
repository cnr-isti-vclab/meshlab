/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
Revision 1.12  2007/11/19 17:09:20  ponchio
added enum value. [untested].

Revision 1.11  2007/11/05 12:03:01  cignoni
added color as a possible parameter

Revision 1.10  2007/10/24 10:34:26  ponchio
removed extra FilterParameterSet:: in findParameter(QString name);

Revision 1.9  2007/10/02 07:59:34  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.8  2007/06/25 13:31:55  zifnab1974
passing char* instead of const char* is about to be deprecated in gcc

Revision 1.7  2007/03/27 12:20:13  cignoni
Revamped logging iterface, changed function names in automatic parameters, better selection handling

Revision 1.6  2007/02/09 09:09:39  pirosu
Added ToolTip support for standard parameters

Revision 1.5  2007/02/08 23:45:26  pirosu
merged srcpar and par in the GetStdParameters() function

Revision 1.4  2006/12/27 21:41:41  pirosu
Added improvements for the standard plugin window:
split of the apply button in two buttons:ok and apply
added support for parameters with absolute and percentage values

Revision 1.3  2006/12/13 17:37:02  pirosu
Added standard plugin window support

Revision 1.2  2006/06/18 20:40:06  cignoni
Completed Open/Save of scripts

Revision 1.1  2006/06/15 21:24:35  cignoni
First ver


****************************************************************************/

#ifndef MESHLAB_FILTERPARAMETER_H
#define MESHLAB_FILTERPARAMETER_H
#include <QtCore>
#include <QMap>
#include <QPair>
#include <QAction>

/*
The Filter Parameter class 
The plugin for each filter exposes a set of typed parameters.
The invocation of a filter requires that these parameters have the specified values.
The Specification of the parameters can be done in two ways, either by an automatic dialog constructed by the MeshLab framework or by a user built dialog.

*/

class FilterParameter
{
 public:
 FilterParameter()
	{
		fieldType=-1;
	}
	
	FilterParameter(QString name, QString desc, QString tooltip)
	{
		fieldType=-1;
		fieldName=name;
		fieldDesc=desc;
		fieldToolTip=tooltip;
	}
	
	enum ParType
	{
		PARBOOL    = 1,
		PARINT     = 2,
		PARFLOAT   = 3,
		PARSTRING  = 4,
		PARABSPERC = 5,
		PARMATRIX  = 6,
		PARCOLOR = 7,
		PARENUM = 8
	};
	
	QString  fieldName;
  QString  fieldDesc;
  QString  fieldToolTip;
  QVariant fieldVal;
	
	// The type of the parameter
  int fieldType;  
  
	float min;
  float max;
	QStringList enumValues;
};

/*
 The Filter Parameter class 
 */

class FilterParameterSet
{
public:

  FilterParameterSet(){}	
	// The data is just a list of Parameters
  //QMap<QString, FilterParameter *> paramMap;  
	QList<FilterParameter> paramList;  
	
	// Members 

	void addBool     (QString name, bool      defaultVal, QString desc=QString(), QString tooltip=QString());
	void addInt      (QString name, int       defaultVal, QString desc=QString(), QString tooltip=QString());
	void addFloat    (QString name, float     defaultVal, QString desc=QString(), QString tooltip=QString());
	void addString   (QString name, QString   defaultVal, QString desc=QString(), QString tooltip=QString());
	void addMatrix44 (QString name, vcg::Matrix44f defaultVal, QString desc=QString(), QString tooltip=QString());
	void addColor    (QString name, QColor defaultVal, QString desc=QString(), QString tooltip=QString());
  void addAbsPerc  (QString name, float     defaultVal, float minVal, float maxVal,  QString desc=QString(), QString tooltip=QString());
	void addEnum     (QString name, int defaultVal, QStringList values, QString desc=QString(), QString tooltip=QString());
		
	bool				getBool(QString name);
	int					getInt(QString name);
	float				getFloat(QString name);
	QString			getString(QString name);
	vcg::Matrix44f		getMatrix44(QString name);
	QColor		  getColor(QString name);
	float		    getAbsPerc(QString name);
  int					getEnum(QString name);
	
	void setBool(QString name, bool newVal) ;
	void setInt(QString name, int newVal) ;
	void setFloat(QString name, float newVal);
	void setString(QString name, QString newVal);
	void setMatrix44(QString name, vcg::Matrix44f newVal);
	void setColor(QString name, QColor newVal);
	void setAbsPerc(QString name, float newVal);
	void setEnum(QString name, int newVal);

	FilterParameter &findParameter(QString name);

	void clear() { paramList.clear(); }
};


#endif
