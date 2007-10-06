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
$Log: filterparameter.h,v $
****************************************************************************/

#include <QtCore>
#include <QMap>
#include <QPair>
#include <QAction>
#include <vcg/math/matrix44.h>
#include "filterparameter.h"

using namespace vcg;

FilterParameter &FilterParameterSet::findParameter(QString name)
{
	static FilterParameter tempFP;
  QList<FilterParameter>::iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
		if((*fpli).fieldName==name)
				return *fpli;
		
	assert(0);
	return tempFP;
}

//--------------------------------------

void FilterParameterSet::addBool (QString name, bool defaultVal, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
  p.fieldVal=defaultVal;
	p.fieldType=FilterParameter::PARBOOL;
	paramList.push_back(p);	
}

void FilterParameterSet::setBool(QString name, bool newVal)
{
	FilterParameter &p=findParameter(name);
	assert(p.fieldType == FilterParameter::PARBOOL);
	p.fieldVal=QVariant(newVal);
}
bool FilterParameterSet::getBool(QString name)
{
	FilterParameter &p=findParameter(name);
	assert(p.fieldType == FilterParameter::PARBOOL);
	return p.fieldVal.toBool();
}

//--------------------------------------

void FilterParameterSet::addInt(QString name, int       defaultVal, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
  p.fieldVal=defaultVal;
	p.fieldType=FilterParameter::PARINT;
	paramList.push_back(p);		
}
int	 FilterParameterSet::getInt(QString name)
{
	FilterParameter &p=findParameter(name);
	assert(p.fieldType == FilterParameter::PARINT);
	return p.fieldVal.toInt();
}
void FilterParameterSet::setInt(QString name, int newVal)
{
	FilterParameter &p=findParameter(name);
	assert(p.fieldType == FilterParameter::PARINT);
	p.fieldVal=QVariant(newVal);	
}

//--------------------------------------

void  FilterParameterSet::addFloat(QString name, float defaultVal, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
  p.fieldVal=defaultVal;
	p.fieldType=FilterParameter::PARFLOAT;
	paramList.push_back(p);		
}
float FilterParameterSet::getFloat(QString name)
{
	FilterParameter &p=findParameter(name);
	assert(p.fieldType == FilterParameter::PARFLOAT);
	return float(p.fieldVal.toDouble());
}
void  FilterParameterSet::setFloat(QString name, float newVal)
{
	FilterParameter &p=findParameter(name);
	assert(p.fieldType == FilterParameter::PARFLOAT);
	p.fieldVal=QVariant(double(newVal));	
}

//--------------------------------------


void FilterParameterSet::addString   (QString name, QString   defaultVal, QString desc, QString tooltip)
{
	assert(0);
}
QString			FilterParameterSet::getString(QString name)
{
	assert(0);
}
void FilterParameterSet::setString(QString name, QString newVal)
{
	assert(0);
}

//--------------------------------------


Matrix44f		FilterParameterSet::getMatrix44(QString name)
{
    FilterParameter &p=findParameter(name);

    assert(p.fieldType==FilterParameter::PARMATRIX);
		assert(p.fieldVal.type()==QVariant::List);
   
    Matrix44f matrix;
    QList<QVariant> matrixVals = p.fieldVal.toList();
    assert(matrixVals.size()==16);
    for(int i=0;i<16;++i)
      matrix.V()[i]=matrixVals[i].toDouble();
        return matrix;

}
void FilterParameterSet::addMatrix44 (QString name, Matrix44f defaultVal, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
 
	QList<QVariant> matrixVals;
    for(int i=0;i<16;++i)
        matrixVals.append(defaultVal.V()[i]);
	p.fieldVal=matrixVals;
	p.fieldType=FilterParameter::PARMATRIX;
	paramList.push_back(p);		
}


void FilterParameterSet::setMatrix44(QString name, Matrix44f newVal)
{
	assert(0);
}


//--------------------------------------

void FilterParameterSet::addAbsPerc (QString name, float defaultVal, float minVal, float maxVal, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
  p.fieldVal=defaultVal;
	p.fieldType=FilterParameter::PARABSPERC;
	p.min=minVal;
	p.max=maxVal;
	paramList.push_back(p);	
}
float		FilterParameterSet::getAbsPerc(QString name)
{
	FilterParameter &p=findParameter(name);
	assert(p.fieldType == FilterParameter::PARABSPERC);
	return float(p.fieldVal.toDouble());
}
void FilterParameterSet::setAbsPerc(QString name, float newVal)
{
	FilterParameter &p=findParameter(name);
	assert(p.fieldType == FilterParameter::PARABSPERC);
	p.fieldVal=QVariant(double(newVal));	
}



