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
		
	qDebug("FilterParameter Warning: Unable to find a parameter with name '%s',\n"
				 "      Please check types and names of the parameter in the calling filter",qPrintable(name));
	return 0;
}

const FilterParameter *FilterParameterSet::findParameter(QString name) const
{
	QList<FilterParameter>::const_iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
		if((*fpli).fieldName==name)
				return &*fpli;
		
	qDebug("FilterParameter Warning: Unable to find a parameter with name '%s',\n"
				 "      Please check types and names of the parameter in the calling filter",qPrintable(name));
	return 0;
}

void FilterParameterSet::removeParameter(QString name){
	paramList.removeAll(*findParameter(name));
}

int FilterParameterSet::getDynamicFloatMask()
{
	int maskFound=0;
		QList<FilterParameter>::const_iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
		if((*fpli).fieldType==FilterParameter::PARDYNFLOAT)
			maskFound |= (*fpli).mask;
	
	return maskFound;
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
bool FilterParameterSet::getBool(QString name) const
{
	const FilterParameter *p=findParameter(name);
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
int	 FilterParameterSet::getInt(QString name) const
{
	const FilterParameter *p=findParameter(name);
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
float FilterParameterSet::getFloat(QString name) const
{
	const FilterParameter *p=findParameter(name);
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

Color4b FilterParameterSet::getColor4b(QString name) const
{
	QColor c=getColor(name);
	return Color4b(c.red(),c.green(),c.blue(),255);
}

QColor FilterParameterSet::getColor(QString name) const
{
	const FilterParameter *p=findParameter(name);
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
QString FilterParameterSet::getString(QString name) const
{
	const FilterParameter *p=findParameter(name);
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


Matrix44f		FilterParameterSet::getMatrix44(QString name) const
{
	const FilterParameter *p=findParameter(name);

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


void FilterParameterSet::setMatrix44(QString , Matrix44f )
{
	assert(0);
}

//--------------------------------------

Point3f		FilterParameterSet::getPoint3f(QString name) const
{
	const FilterParameter *p=findParameter(name);
	
	assert(p);
	assert(p->fieldType==FilterParameter::PARPOINT3F);
	assert(p->fieldVal.type()==QVariant::List);
	
	Point3f point;
	QList<QVariant> pointVals = p->fieldVal.toList();
	assert(pointVals.size()==3);
	for(int i=0;i<3;++i)
		point[i]=pointVals[i].toDouble();
	return point;
}
void FilterParameterSet::addPoint3f (QString name, Point3f defaultVal, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
	
	QList<QVariant> pointVals;
	for(int i=0;i<3;++i)
		pointVals.append(defaultVal[i]);
	p.fieldVal=pointVals;
	p.fieldType=FilterParameter::PARPOINT3F;
	paramList.push_back(p);		
}
void  FilterParameterSet::setPoint3f(QString name, Point3f newVal)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARPOINT3F);
	QList<QVariant> pointVals;
	for(int i=0;i<3;++i)
		pointVals.append(newVal[i]);
	p->fieldVal=pointVals;
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
float		FilterParameterSet::getAbsPerc(QString name) const
{
	const FilterParameter *p=findParameter(name);
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

int FilterParameterSet::getEnum(QString name) const {
	const FilterParameter *p=findParameter(name);
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

QList<float> FilterParameterSet::getFloatList(QString name) const
{
	const FilterParameter *p = findParameter(name);
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

/* ---- */

void FilterParameterSet::addMesh (QString name, MeshModel *defaultVal, QString desc, QString tooltip) {
	FilterParameter p(name,desc,tooltip);
  p.pointerVal= defaultVal;
	p.fieldType=FilterParameter::PARMESH;
	paramList.push_back(p);	
}

//make the default the mesh that is at the given position in the mesh document
void FilterParameterSet::addMesh(QString name, int position,  QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
	p.fieldVal = position;
	p.pointerVal = NULL;
	p.fieldType = FilterParameter::PARMESH;
	paramList.push_back(p);	
}

MeshModel * FilterParameterSet::getMesh(QString name) const {
	const FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARMESH);
	return (MeshModel *)(p->pointerVal);
}

void FilterParameterSet::setMesh(QString name, MeshModel * newVal, int position)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARMESH);
	p->fieldVal = position;
	p->pointerVal= newVal;
}

/* ---- */
/* Dynamic Float Members*/
/* ---- */

void FilterParameterSet::addDynamicFloat(QString name, float defaultVal, float minVal, float maxVal, int changeMask, QString desc , QString tooltip )
{
	FilterParameter p(name,desc,tooltip);
	assert(defaultVal<=maxVal);
	assert(defaultVal>=minVal);
  p.fieldVal=defaultVal;
	p.fieldType=FilterParameter::PARDYNFLOAT;
	p.min=minVal;
	p.max=maxVal;
	p.mask=changeMask;
	paramList.push_back(p);	
}

float FilterParameterSet::getDynamicFloat(QString name) const
{
	const FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARDYNFLOAT);
	return float(p->fieldVal.toDouble());
}

void  FilterParameterSet::setDynamicFloat(QString name, float newVal)
{
	FilterParameter *p=findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARDYNFLOAT);
	p->fieldVal=QVariant(newVal);	
}

/* PAROPENFILENAME */

void FilterParameterSet::addOpenFileName(QString name, QString defaultVal, QString extension, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
	p.fieldVal = defaultVal; 
	p.fieldType = FilterParameter::PAROPENFILENAME;
	
	//add the extension to this unused variable because i think it is cleaner than adding another variable not used anywhere else
	p.enumValues.push_back(extension);
	
	paramList.push_back(p);
}

QString FilterParameterSet::getOpenFileName(QString name) const
{
	const FilterParameter *p = findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PAROPENFILENAME);
	return p->fieldVal.toString();
}

void FilterParameterSet::setOpenFileName(QString name, QString newVal)
{
	FilterParameter *p = findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PAROPENFILENAME);
	p->fieldVal = QVariant(newVal);	
}

/* PARSAVEFILENAME */

void FilterParameterSet::addSaveFileName(QString name, QString defaultVal, QString extension, QString desc, QString tooltip)
{
	FilterParameter p(name,desc,tooltip);
	p.fieldVal = defaultVal; 
	p.fieldType = FilterParameter::PARSAVEFILENAME;
		
	//add the extension to this unused variable because i think it is cleaner than adding another variable not used anywhere else
	p.enumValues.push_back(extension);
	
	paramList.push_back(p);
}

QString FilterParameterSet::getSaveFileName(QString name) const
{
	const FilterParameter *p = findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARSAVEFILENAME);
	return p->fieldVal.toString();
}

void FilterParameterSet::setSaveFileName(QString name, QString newVal)
{
	FilterParameter *p = findParameter(name);
	assert(p);
	assert(p->fieldType == FilterParameter::PARSAVEFILENAME);
	p->fieldVal = QVariant(newVal);	
}
