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

bool RichParameterSet::hasParameter(QString name)
{
	QList<RichParameter*>::iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
		if((*fpli)->name ==name)
			return true;

	return false;
}
RichParameter* RichParameterSet::findParameter(QString name)
{
	QList<RichParameter*>::iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
		if((*fpli)->name ==name)
			return *fpli;

	qDebug("FilterParameter Warning: Unable to find a parameter with name '%s',\n"
		"      Please check types and names of the parameter in the calling filter",qPrintable(name));
	return 0;
}

const RichParameter* RichParameterSet::findParameter(QString name) const
{
	QList<RichParameter*>::const_iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
	{
		if((*fpli != NULL) && (*fpli)->name==name)
			return *fpli;
	}
	qDebug("FilterParameter Warning: Unable to find a parameter with name '%s',\n"
		"      Please check types and names of the parameter in the calling filter",qPrintable(name));
	return 0;
}

RichParameterSet& RichParameterSet::removeParameter(QString name){
	paramList.removeAll(findParameter(name));
	return (*this);
}

RichParameterSet& RichParameterSet::addParam(RichParameter* pd )
{
	paramList.push_back(pd);
	return (*this);
}

//--------------------------------------


void RichParameterSet::setValue(QString name,const Value& newval)
{
	RichParameter *p=findParameter(name);
	assert(p);
	p->val->set(newval);
}

//--------------------------------------

bool RichParameterSet::getBool(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getBool();
}

//--------------------------------------

int	 RichParameterSet::getInt(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getInt();
}

//--------------------------------------

float RichParameterSet::getFloat(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getFloat();
}

//--------------------------------------

QColor RichParameterSet::getColor(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getColor();
}

Color4b RichParameterSet::getColor4b(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getColor4b();
}

//--------------------------------------


QString RichParameterSet::getString(QString name) const
{
	const RichParameter*p=findParameter(name);
	assert(p);
	return p->val->getString();
}

//--------------------------------------


Matrix44f		RichParameterSet::getMatrix44(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getMatrix44f();
}

//--------------------------------------

Point3f		RichParameterSet::getPoint3f(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getPoint3f();
}

//--------------------------------------

float		RichParameterSet::getAbsPerc(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return float(p->val->getAbsPerc());
}

int RichParameterSet::getEnum(QString name) const {
	const RichParameter *p=findParameter(name);
	assert(p);
	return float(p->val->getEnum());
}

QList<float> RichParameterSet::getFloatList(QString name) const
{
	const RichParameter *p = findParameter(name);
	assert(p);
	return p->val->getFloatList();
}

/* ---- */

MeshModel * RichParameterSet::getMesh(QString name) const {
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getMesh();
}

/* ---- */
/* Dynamic Float Members*/
/* ---- */

float RichParameterSet::getDynamicFloat(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return float(p->val->getDynamicFloat());
}

QString RichParameterSet::getOpenFileName(QString name) const
{
	const RichParameter *p = findParameter(name);
	assert(p);
	return p->val->getFileName();
}


QString RichParameterSet::getSaveFileName(QString name) const
{
	const RichParameter *p = findParameter(name);
	assert(p);
	return p->val->getFileName();
}

RichParameterSet& RichParameterSet::operator=( const RichParameterSet& rps )
{
	return copy(rps);
}

bool RichParameterSet::operator==( const RichParameterSet& rps )
{
	if (rps.paramList.size() != paramList.size())
		return false;

	bool iseq = true;
	unsigned int ii = 0;
	while((ii < rps.paramList.size()) && iseq)
	{
		if (!(*rps.paramList.at(ii) == *paramList.at(ii)))
			iseq = false;
		++ii;
	}
	
	return iseq;
}

RichParameterSet::~RichParameterSet()
{
	//int val = _CrtCheckMemory( );
	for(unsigned int ii = 0;ii < paramList.size();++ii)
		delete paramList.at(ii);
	paramList.clear();

}

RichParameterSet& RichParameterSet::copy( const RichParameterSet& rps )
{
	clear();

	RichParameterCopyConstructor copyvisitor;
	for(unsigned int ii = 0;ii < rps.paramList.size();++ii)
	{
		rps.paramList.at(ii)->accept(copyvisitor);
		paramList.push_back(copyvisitor.lastCreated);
	}
	return (*this);
}


RichParameterSet::RichParameterSet( const RichParameterSet& rps )
{
	clear();

	RichParameterCopyConstructor copyvisitor;
	for(unsigned int ii = 0;ii < rps.paramList.size();++ii)
	{
		rps.paramList.at(ii)->accept(copyvisitor);
		paramList.push_back(copyvisitor.lastCreated);
	}
}

/****************************************/

void RichParameterCopyConstructor::visit( RichBool& pd )
{
	lastCreated = new RichBool(pd.name,pd.pd->defVal->getBool(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichInt& pd )
{
	lastCreated = new RichInt(pd.name,pd.pd->defVal->getInt(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichFloat& pd )
{
	lastCreated = new RichFloat(pd.name,pd.pd->defVal->getFloat(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichString& pd )
{
	lastCreated = new RichString(pd.name,pd.pd->defVal->getString(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichMatrix44f& pd )
{
	lastCreated = new RichMatrix44f(pd.name,pd.pd->defVal->getMatrix44f(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichPoint3f& pd )
{
	lastCreated = new RichPoint3f(pd.name,pd.pd->defVal->getPoint3f(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichColor& pd )
{
	lastCreated = new RichColor(pd.name,pd.pd->defVal->getColor(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichColor4b& pd )
{
	/*lastCreated = new Color4bWidget(par,&pd);*/
}

void RichParameterCopyConstructor::visit( RichAbsPerc& pd )
{
	AbsPercDecoration* dec = reinterpret_cast<AbsPercDecoration*>(pd.pd); 
	lastCreated = new RichAbsPerc(pd.name,pd.pd->defVal->getAbsPerc(),dec->min,dec->max,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichEnum& pd )
{
	EnumDecoration* dec = reinterpret_cast<EnumDecoration*>(pd.pd);
	lastCreated = new RichEnum(pd.name,pd.pd->defVal->getEnum(),dec->enumvalues,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichFloatList& pd )
{
	/*lastCreated = new FloatListWidget(par,&pd);*/
}

void RichParameterCopyConstructor::visit( RichDynamicFloat& pd )
{
	DynamicFloatDecoration* dec = reinterpret_cast<DynamicFloatDecoration*>(pd.pd); 
	lastCreated = new RichDynamicFloat(pd.name,pd.pd->defVal->getDynamicFloat(),dec->min,dec->max,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichOpenFile& pd )
{
	/*lastCreated = new OpenFileWidget(par,&pd);*/
}

void RichParameterCopyConstructor::visit( RichSaveFile& pd )
{
	/*lastCreated = new SaveFileWidget(par,&pd);*/
}

void RichParameterCopyConstructor::visit( RichMesh& pd )
{
	MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(pd.pd); 
	lastCreated = new RichMesh(pd.name,dec->defVal->getMesh(),dec->meshdoc,dec->fieldDesc,dec->tooltip);
}
