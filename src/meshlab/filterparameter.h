/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2008                                                \/)\/    *
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

#ifndef MESHLAB_FILTERPARAMETER_H
#define MESHLAB_FILTERPARAMETER_H
#include <QtCore>
#include <QMap>
#include <QPair>
#include <QAction>
#include <vcg/math/matrix44.h>
#include <meshlab/meshmodel.h>


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
		pointerVal = 0;
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
		PARENUM = 8,
		PARMESH = 9,
		PARFLOATLIST = 10,
		PARDYNFLOAT = 11,
		PAROPENFILENAME = 12,
		PARSAVEFILENAME = 13,
		PARPOINT3F = 14
	};
	
	QString  fieldName;
  QString  fieldDesc;
  QString  fieldToolTip;
  QVariant fieldVal;
	
	// The type of the parameter
  int fieldType;  
  
	float min;  // used by the AbsPerc and DynFloat types
  float max;
	int mask; // used by the DynFloat types
	void *pointerVal;
	QStringList enumValues;
	
	//set the value to be that of the input paramter
	//needed because min, max, enumValues complicate things
	void setValue(const FilterParameter &inputParameter)
	{
		assert(fieldType == inputParameter.fieldType);
		fieldVal = inputParameter.fieldVal;
		min = inputParameter.min;
		max = inputParameter.max;
		pointerVal = inputParameter.pointerVal;
	
		//clear any old values
		enumValues.clear();
		//add all the new ones
		enumValues += inputParameter.enumValues;
	}
	
	//an equals operator that compares the field name and field type
	bool operator==(const FilterParameter &inputParameter) const
	{
		return (fieldName == inputParameter.fieldName);
	}
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
	bool isEmpty() const {return paramList.isEmpty();} 
	// Members 

	void addBool     (QString name, bool      defaultVal, QString desc=QString(), QString tooltip=QString());
	void addInt      (QString name, int       defaultVal, QString desc=QString(), QString tooltip=QString());
	void addFloat    (QString name, float     defaultVal, QString desc=QString(), QString tooltip=QString());
	void addString   (QString name, QString   defaultVal, QString desc=QString(), QString tooltip=QString());
	void addMatrix44 (QString name, vcg::Matrix44f defaultVal, QString desc=QString(), QString tooltip=QString());
	void addPoint3f   (QString name, vcg::Point3f defaultVal, QString desc=QString(), QString tooltip=QString());
	void addColor    (QString name, QColor defaultVal, QString desc=QString(), QString tooltip=QString());
	void addAbsPerc  (QString name, float     defaultVal, float minVal, float maxVal,  QString desc=QString(), QString tooltip=QString());
	void addEnum     (QString name, int defaultVal, QStringList values, QString desc=QString(), QString tooltip=QString());
	void addMesh     (QString name, MeshModel* m,  QString desc=QString(), QString tooltip=QString());
	
	//make the default the mesh that is at the given position in the mesh document
	//if the filter is run in a script and has added a mesh for a position that does not exist in the MeshDocument, then the 
	//script will fail to run.  this is useful for filters that need more than one mesh to work.
	//if position is set to -1 no mesh will be chosen by default and a blank option will be put in the Enum
	void addMesh     (QString name, int position,  QString desc=QString(), QString tooltip=QString());
	
	/*  A way to collect an arbitrary number of floats. Useful if you want the user to input an array of numbers */
	void addFloatList(QString name, QList<float> &defaultValue, QString desc=QString(), QString tooltip=QString());
	void addDynamicFloat(QString name, float defaultVal, float minVal, float maxVal, int changeMask, QString desc=QString(), QString tooltip=QString());
	
	/*  A way to use the built in QT file picker widget: QFileDialog::getOpenFileName
	 *  - QString extension - the regular exprssion used to decide what files to display in QT's file picker window
	 */
	void addOpenFileName(QString name, QString defaultVal, QString extension=QString(".*"), QString desc=QString(), QString tooltip=QString());
	
	/*  A way to use the built in QT file picker widget: QFileDialog::getSaveFileName
	 *  - QString extension - the regular exprssion used to decide what files to display in QT's file picker window
	 */
	void addSaveFileName(QString name, QString defaultVal, QString extension=QString(".*"), QString desc=QString(), QString tooltip=QString());
		
	
	
	bool				getBool(QString name) const;
	int					getInt(QString name) const;
	float				getFloat(QString name) const;
	QString			getString(QString name) const;
	vcg::Matrix44f		getMatrix44(QString name) const;
	vcg::Point3f getPoint3f(QString name) const;
	QColor		   getColor(QString name) const;
	vcg::Color4b getColor4b(QString name) const;
	float		     getAbsPerc(QString name) const;
  int					 getEnum(QString name) const;	
	MeshModel*   getMesh(QString name) const;
	QList<float> getFloatList(QString name) const;
	float        getDynamicFloat(QString name) const;
	QString getOpenFileName(QString name) const;
	QString getSaveFileName(QString name) const;
	
	void setBool(QString name, bool newVal) ;
	void setInt(QString name, int newVal) ;
	void setFloat(QString name, float newVal);
	void setString(QString name, QString newVal);
	void setMatrix44(QString name, vcg::Matrix44f newVal);
	void setPoint3f(QString name, vcg::Point3f newVal);
	void setColor(QString name, QColor newVal);
	void setAbsPerc(QString name, float newVal);
	void setEnum(QString name, int newVal);
	
	//position is the position of this mesh in the MeshDocument, this is needed for saving this parameter to a script
	void setMesh(QString name, MeshModel* newVal, int position = 0);
	void setFloatList(QString name, QList<float> &newValue);
	void setDynamicFloat(QString name, float newVal);
	void setOpenFileName(QString name, QString newVal);
	void setSaveFileName(QString name, QString newVal);
	
	int getDynamicFloatMask();
	
	FilterParameter *findParameter(QString name);
  const FilterParameter *findParameter(QString name) const;

	//remove a parameter from the set by name
	void removeParameter(QString name);
	
	void clear() { paramList.clear(); }
};


#endif
