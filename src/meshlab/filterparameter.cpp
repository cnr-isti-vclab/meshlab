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
	if (dec->defVal != NULL)
		lastCreated = new RichMesh(pd.name,dec->defVal->getMesh(),dec->meshdoc,dec->fieldDesc,dec->tooltip);
	else
		lastCreated = new RichMesh(pd.name,dec->meshindex);
}

void RichParameterXMLVisitor::fillRichParameterAttribute(const QString& type,const QString& name)
{
	parElem = docdom.createElement("Param");
	parElem.setAttribute("type",type);
	parElem.setAttribute("name",name);
}

void RichParameterXMLVisitor::fillRichParameterAttribute(const QString& type,const QString& name,const QString& val)
{
	fillRichParameterAttribute(type,name);
	parElem.setAttribute("value",val);
}

void RichParameterXMLVisitor::visit( RichBool& pd )
{
	QString v;
	if (pd.val->getBool()) 
		v = "true";
	else	
		v = "false";
	fillRichParameterAttribute("RichBool",pd.name,v);
}

void RichParameterXMLVisitor::visit( RichInt& pd )
{
	fillRichParameterAttribute("RichInt",pd.name,QString::number(pd.val->getInt()));
}

void RichParameterXMLVisitor::visit( RichFloat& pd )
{
	fillRichParameterAttribute("RichFloat",pd.name,QString::number(pd.val->getFloat()));
}

void RichParameterXMLVisitor::visit( RichString& pd )
{
	fillRichParameterAttribute("RichString",pd.name,pd.val->getString());
}

void RichParameterXMLVisitor::visit( RichMatrix44f& pd )
{
	fillRichParameterAttribute("RichMatrix44f",pd.name);
	vcg::Matrix44f mat = pd.val->getMatrix44f();
	for(unsigned int ii = 0;ii < 16;++ii)
		parElem.setAttribute(QString("val")+QString::number(ii),QString::number(mat.V()[ii]));
}

void RichParameterXMLVisitor::visit( RichPoint3f& pd )
{
	fillRichParameterAttribute("RichPoint3f",pd.name);
	vcg::Point3f p = pd.val->getPoint3f();
	parElem.setAttribute("x",QString::number(p.X()));
	parElem.setAttribute("y",QString::number(p.Y()));
	parElem.setAttribute("z",QString::number(p.Z()));
}

void RichParameterXMLVisitor::visit( RichColor& pd )
{
	fillRichParameterAttribute("RichColor",pd.name);
	QColor p = pd.val->getColor();
	parElem.setAttribute("r",QString::number(p.red()));
	parElem.setAttribute("g",QString::number(p.green()));
	parElem.setAttribute("b",QString::number(p.blue()));
	parElem.setAttribute("a",QString::number(p.alpha()));
}

void RichParameterXMLVisitor::visit( RichColor4b& pd )
{
	assert(0);	
}

void RichParameterXMLVisitor::visit( RichAbsPerc& pd )
{
	fillRichParameterAttribute("RichAbsPerc",pd.name,QString::number(pd.val->getAbsPerc()));
	AbsPercDecoration* dec = reinterpret_cast<AbsPercDecoration*>(pd.pd);
	parElem.setAttribute("min",QString::number(dec->min));
	parElem.setAttribute("max",QString::number(dec->max));
}

void RichParameterXMLVisitor::visit( RichEnum& pd )
{
	fillRichParameterAttribute("RichEnum",pd.name,QString::number(pd.val->getEnum()));
	EnumDecoration* dec = reinterpret_cast<EnumDecoration*>(pd.pd);
	parElem.setAttribute("enum_cardinality",dec->enumvalues.size());
	for(unsigned int ii = 0; ii < dec->enumvalues.size();++ii)
		parElem.setAttribute(QString("enum_val")+QString::number(ii),dec->enumvalues.at(ii));

}

void RichParameterXMLVisitor::visit( RichFloatList& pd )
{
	assert(0);
}

void RichParameterXMLVisitor::visit(RichDynamicFloat& pd)
{
	fillRichParameterAttribute("RichDynamicFloat",pd.name,QString::number(pd.val->getDynamicFloat()));
	DynamicFloatDecoration* dec = reinterpret_cast<DynamicFloatDecoration*>(pd.pd);
	parElem.setAttribute("min",QString::number(dec->min));
	parElem.setAttribute("max",QString::number(dec->max));
}

void RichParameterXMLVisitor::visit( RichOpenFile& pd )
{
	assert(0);
}

void RichParameterXMLVisitor::visit( RichSaveFile& pd )
{
	assert(0);
}

void RichParameterXMLVisitor::visit( RichMesh& pd )
{
	MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(pd.pd);
	fillRichParameterAttribute("RichMesh",pd.name,QString::number(dec->meshindex));
}

void RichParameterFactory::create( const QDomElement& np,RichParameter** par )
{
	QString name=np.attribute("name");
	QString type=np.attribute("type");

	qDebug("    Reading Param with name %s : %s",qPrintable(name),qPrintable(type));

	if(type=="RichBool")    
	{ 
		*par = new RichBool(name,np.attribute("value")!=QString("false")); 
		return; 
	}

	if(type=="RichInt")     
	{ 
		*par = new RichInt(name,np.attribute("value").toInt()); 
		return; 
	}

	if(type=="RichFloat")   
	{ 
		*par = new RichFloat(name,np.attribute("value").toFloat());
		return;
	}

	if(type=="RichString")  
	{ 
		*par = new RichString(name,np.attribute("value")); 
		return; 
	}
	
	if(type=="RichAbsPerc") 
	{ 
		*par = new RichAbsPerc(name,np.attribute("value").toFloat(),np.attribute("min").toFloat(),np.attribute("max").toFloat()); 
		return; 
	}

	if(type=="RichColor")		
	{ 
		unsigned int r = np.attribute("r").toUInt(); 
		unsigned int g = np.attribute("g").toUInt(); 
		unsigned int b = np.attribute("b").toUInt(); 
		unsigned int a = np.attribute("a").toUInt(); 
		QColor col(r,g,b,a);
		*par= new RichColor(name,col); 
		return; 
	}

	if(type=="RichMatrix44f")
	{
		Matrix44f mm;
		for(int i=0;i<16;++i)
			mm.V()[i]=np.attribute(QString("val")+QString::number(i)).toFloat();
		*par = new RichMatrix44f(name,mm);    
		return;                    
	}

	if(type=="RichEnum")
	{
		QStringList list = QStringList::QStringList();
		unsigned int enum_card = np.attribute(QString("enum_cardinality")).toUInt();

		for(int i=0;i<enum_card;++i)
			list<<np.attribute(QString("enum_val")+QString::number(i));

		*par = new RichEnum(name,np.attribute("value").toInt(),list);
		return;
	}

	if(type == "RichMesh")  
	{ 
		*par = new RichMesh(name, np.attribute("value").toInt()); 
		return; 
	}

	if(type == "RichFloatList")
	{
		//to be implemented
		assert(0);
	}

	if(type == "RichOpenFile")  
	{ 
		//to be implemented
		assert(0);
	}

	if(type == "RichSaveFile")  
	{ 
		//par.addOpenFileName(name, np.attribute(ValueName())); return; 
		
		//to be implemented
		assert(0);
	}

	if(type=="Point3f") 
	{
		Point3f val;
		val[0]=np.attribute("x").toFloat();
		val[1]=np.attribute("y").toFloat();
		val[2]=np.attribute("z").toFloat();
		*par = new RichPoint3f(name, val);  
		return; 
	}

	assert(0); // we are trying to parse an unknown xml element
}