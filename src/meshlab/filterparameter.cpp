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

FilterParameter *FilterParameterSet::findParameter(QString name)
{
	QList<FilterParameter>::iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
		if((*fpli).fieldName==name)
				return &*fpli;
		
	//assert(0);
	return 0;
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
	FilterParameter *p=findParameter(name);
	assert(p);
	p->fieldVal=QVariant(newVal);
}
bool FilterParameterSet::getBool(QString name)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARBOOL);
	return p->fieldVal.toBool();
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
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARINT);
	return p->fieldVal.toInt();
}
void FilterParameterSet::setInt(QString name, int newVal)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARINT);
	p->fieldVal=QVariant(newVal);	
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
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARFLOAT);
	return float(p->fieldVal.toDouble());
}
void  FilterParameterSet::setFloat(QString name, float newVal)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARFLOAT);
	p->fieldVal=QVariant(double(newVal));	
}

//--------------------------------------
void  FilterParameterSet::addColor(QString name, QColor defaultVal, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
  p.fieldVal=defaultVal.rgb(); // it is converted to an unsigned int
	p.fieldType=FilterParameter::PARCOLOR;
	paramList.push_back(p);		
}
QColor FilterParameterSet::getColor(QString name)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARCOLOR);
	return QColor(QRgb(p->fieldVal.toUInt()));
}

void  FilterParameterSet::setColor(QString name, QColor newVal)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARCOLOR);
	p->fieldVal=QVariant(newVal.rgb());	
}

//--------------------------------------

void FilterParameterSet::addString   (QString name, QString   defaultVal, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
  p.fieldVal=defaultVal; 
	p.fieldType=FilterParameter::PARSTRING;
	paramList.push_back(p);		
}
QString FilterParameterSet::getString(QString name)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARSTRING);
	return p->fieldVal.toString();
}
void FilterParameterSet::setString(QString name, QString newVal)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARSTRING);
	p->fieldVal=QVariant(newVal);	
}

//--------------------------------------


Matrix44f		FilterParameterSet::getMatrix44(QString name)
{
	FilterParameter *p=findParameter(name);

	assert(p);
	assert(p->fieldType==FilterParameter::PARMATRIX);
	assert(p->fieldVal.type()==QVariant::List);

	Matrix44f matrix;
	QList<QVariant> matrixVals = p->fieldVal.toList();
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
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARABSPERC);
	return float(p->fieldVal.toDouble());
}
void FilterParameterSet::setAbsPerc(QString name, float newVal)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARABSPERC);
	p->fieldVal=QVariant(double(newVal));	
}

void FilterParameterSet::addEnum (QString name, int defaultVal, QStringList values, QString desc, QString tooltip) {
	FilterParameter p(name,desc,tooltip);
  p.fieldVal=defaultVal;
	p.fieldType=FilterParameter::PARENUM;
	p.enumValues = values;
	paramList.push_back(p);	
}

int FilterParameterSet::getEnum(QString name) {
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARENUM);
	return float(p->fieldVal.toInt());
}

void FilterParameterSet::setEnum(QString name, int newVal)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARENUM);
	p->fieldVal=QVariant(int(newVal));	
}

void FilterParameterSet::addFloatList(QString name, QList<float> &defaultValue, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
	
	QList<QVariant> tempList;
	for(int i = 0; i < defaultValue.size(); ++i)
	{
		//if you put the float in directly int converts to a double which we do not want
		tempList.push_back(QVariant(QString().setNum(defaultValue.at(i), 'g', 12)));
		//qDebug() << "putting down " << QString().setNum(defaultValue.at(i), 'g', 12) ;
	}
	
	p.fieldVal = tempList;
	p.fieldType = FilterParameter::PARFLOATLIST;
	paramList.push_back(p);
}

QList<float> FilterParameterSet::getFloatList(QString name)
{
	FilterParameter *p = findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARFLOATLIST);
	
	QList<float> floatList;
	QList<QVariant> internalList = p->fieldVal.toList();
	for(int i = 0; i < internalList.size(); ++i)
		floatList.push_back(internalList.at(i).toString().toFloat());	
	
	return floatList;
}

void FilterParameterSet::setFloatList(QString name, QList<float> &newValue)
{
	FilterParameter *p = findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARFLOATLIST);
	
	QList<QVariant> tempList;
	for(int i = 0; i < newValue.size(); ++i)
	{
		tempList.push_back(QVariant(QString().setNum(newValue.at(i), 'g', 12)));
		//qDebug() << "insetfloatlist " << QVariant(QString().setNum(newValue.at(i), 'g', 12));
	}
	p->fieldVal = tempList;
}
