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

#include <QtCore>
#include <QMap>
#include <QPair>
#include <QAction>
#include <vcg/math/matrix44.h>
#include <wrap/qt/col_qt_convert.h>

#include "filterparameter.h"


using namespace vcg;

// Very similar to the findParameter but this one does not print out debugstuff. 
bool RichParameterSet::hasParameter(QString name) const 
{
	QList<RichParameter*>::const_iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
	{
		if((*fpli != NULL) && (*fpli)->name==name)
			return true;
	}	
	return false; 
}
// You should never use this one to know if a given parameter is present. 
RichParameter* RichParameterSet::findParameter(QString name) const
{
	QList<RichParameter*>::const_iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
	{
		if((*fpli != NULL) && (*fpli)->name==name)
			return *fpli;
	}
	qDebug("FilterParameter Warning: Unable to find a parameter with name '%s',\n"
		"      Please check types and names of the parameter in the calling filter",qPrintable(name));
	assert(0);
	return 0;
}

RichParameterSet& RichParameterSet::removeParameter(QString name){
	paramList.removeAll(findParameter(name));
	return (*this);
}

RichParameterSet& RichParameterSet::addParam(RichParameter* pd )
{
	assert(!hasParameter(pd->name));
	paramList.push_back(pd);
	return (*this);
}

//--------------------------------------


void RichParameterSet::setValue(QString name,const Value& newval){ findParameter(name)->val->set(newval); }

//- All the get<TYPE> are very similar. Nothing interesting here. 

        bool RichParameterSet::getBool(QString name)     const { return findParameter(name)->val->getBool(); }
         int RichParameterSet::getInt(QString name)      const { return findParameter(name)->val->getInt();}
       float RichParameterSet::getFloat(QString name)    const { return findParameter(name)->val->getFloat();}
      QColor RichParameterSet::getColor(QString name)    const { return findParameter(name)->val->getColor();}
     Color4b RichParameterSet::getColor4b(QString name)  const { return ColorConverter::ToColor4b(findParameter(name)->val->getColor());}
     QString RichParameterSet::getString(QString name)   const { return findParameter(name)->val->getString();}
   Matrix44f RichParameterSet::getMatrix44(QString name) const { return findParameter(name)->val->getMatrix44f();}
     Point3f RichParameterSet::getPoint3f(QString name)  const { return findParameter(name)->val->getPoint3f();}  
       float RichParameterSet::getAbsPerc(QString name)  const { return findParameter(name)->val->getAbsPerc();}
				 int RichParameterSet::getEnum(QString name)     const { return findParameter(name)->val->getEnum();}
QList<float> RichParameterSet::getFloatList(QString name)    const { return findParameter(name)->val->getFloatList();}
 MeshModel * RichParameterSet::getMesh(QString name)         const { return findParameter(name)->val->getMesh();}
       float RichParameterSet::getDynamicFloat(QString name) const { return findParameter(name)->val->getDynamicFloat();}
     QString RichParameterSet::getOpenFileName(QString name) const { return findParameter(name)->val->getFileName();}
     QString RichParameterSet::getSaveFileName(QString name) const { return findParameter(name)->val->getFileName(); }

RichParameterSet& RichParameterSet::operator=( const RichParameterSet& rps )
{
	return copy(rps);
}

bool RichParameterSet::operator==( const RichParameterSet& rps )
{
	if (rps.paramList.size() != paramList.size())
		return false;

	bool iseq = true;
	int ii = 0;
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
	for(int ii = 0;ii < paramList.size();++ii)
		delete paramList.at(ii);
	paramList.clear();

}

RichParameterSet& RichParameterSet::copy( const RichParameterSet& rps )
{
	clear();

	RichParameterCopyConstructor copyvisitor;
	for(int ii = 0;ii < rps.paramList.size();++ii)
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
	for(int ii = 0;ii < rps.paramList.size();++ii)
	{
		rps.paramList.at(ii)->accept(copyvisitor);
		paramList.push_back(copyvisitor.lastCreated);
	}
}

RichParameterSet::RichParameterSet() :paramList()
{

}

bool RichParameterSet::isEmpty() const
{
	return paramList.isEmpty();
}

void RichParameterSet::clear()
{
	paramList.clear();
}

RichParameterSet& RichParameterSet::join( const RichParameterSet& rps )
{
	RichParameterCopyConstructor copyvisitor;
	for(int ii = 0;ii < rps.paramList.size();++ii)
	{
		rps.paramList.at(ii)->accept(copyvisitor);
		paramList.push_back(copyvisitor.lastCreated);
	}
	return (*this);
}
/****************************************/

void RichParameterCopyConstructor::visit( RichBool& pd )
{
	lastCreated = new RichBool(pd.name,pd.val->getBool(),pd.pd->defVal->getBool(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichInt& pd )
{
	lastCreated = new RichInt(pd.name,pd.val->getInt(),pd.pd->defVal->getInt(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichFloat& pd )
{
	lastCreated = new RichFloat(pd.name,pd.val->getFloat(),pd.pd->defVal->getFloat(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichString& pd )
{
	lastCreated = new RichString(pd.name,pd.val->getString(),pd.pd->defVal->getString(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichMatrix44f& pd )
{
	lastCreated = new RichMatrix44f(pd.name,pd.val->getMatrix44f(),pd.pd->defVal->getMatrix44f(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichPoint3f& pd )
{
	lastCreated = new RichPoint3f(pd.name,pd.val->getPoint3f(),pd.pd->defVal->getPoint3f(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichColor& pd )
{
	lastCreated = new RichColor(pd.name,pd.val->getColor(),pd.pd->defVal->getColor(),pd.pd->fieldDesc,pd.pd->tooltip);
}


void RichParameterCopyConstructor::visit( RichAbsPerc& pd )
{
	AbsPercDecoration* dec = reinterpret_cast<AbsPercDecoration*>(pd.pd); 
	lastCreated = new RichAbsPerc(pd.name,pd.val->getAbsPerc(),pd.pd->defVal->getAbsPerc(),dec->min,dec->max,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichEnum& pd )
{
	EnumDecoration* dec = reinterpret_cast<EnumDecoration*>(pd.pd);
	lastCreated = new RichEnum(pd.name,pd.val->getEnum(),pd.pd->defVal->getEnum(),dec->enumvalues,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichFloatList& /*pd*/ )
{
	/*lastCreated = new FloatListWidget(par,&pd);*/
}

void RichParameterCopyConstructor::visit( RichDynamicFloat& pd )
{
	DynamicFloatDecoration* dec = reinterpret_cast<DynamicFloatDecoration*>(pd.pd); 
	lastCreated = new RichDynamicFloat(pd.name,pd.val->getDynamicFloat(),pd.pd->defVal->getDynamicFloat(),dec->min,dec->max,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichOpenFile& pd )
{
	OpenFileDecoration* dec = reinterpret_cast<OpenFileDecoration*>(pd.pd);
	lastCreated = new RichOpenFile(pd.name,pd.pd->defVal->getFileName(),dec->exts,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichSaveFile& pd )
{
	SaveFileDecoration* dec = reinterpret_cast<SaveFileDecoration*>(pd.pd);
	lastCreated = new RichSaveFile(pd.name,pd.val->getFileName(),dec->ext,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichMesh& pd )
{
	MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(pd.pd); 
	if (dec->defVal != NULL)
		lastCreated = new RichMesh(pd.name,pd.val->getMesh(),dec->defVal->getMesh(),dec->meshdoc,dec->fieldDesc,dec->tooltip);
	else
		lastCreated = new RichMesh(pd.name,dec->meshindex);
}

void RichParameterXMLVisitor::fillRichParameterAttribute(const QString& type,const QString& name,const QString& desc,const QString& tooltip)
{
	parElem = docdom.createElement("Param");
	parElem.setAttribute("type",type);
	parElem.setAttribute("name",name);
	parElem.setAttribute("description",desc);
	parElem.setAttribute("tooltip",tooltip);
}

void RichParameterXMLVisitor::fillRichParameterAttribute(const QString& type,const QString& name,const QString& val,const QString& desc,const QString& tooltip)
{
	fillRichParameterAttribute(type,name,desc,tooltip);
	parElem.setAttribute("value",val);
}

void RichParameterXMLVisitor::visit( RichBool& pd )
{
	QString v;
	if (pd.val->getBool()) 
		v = "true";
	else	
		v = "false";
	fillRichParameterAttribute("RichBool",pd.name,v,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterXMLVisitor::visit( RichInt& pd )
{
	fillRichParameterAttribute("RichInt",pd.name,QString::number(pd.val->getInt()),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterXMLVisitor::visit( RichFloat& pd )
{
	fillRichParameterAttribute("RichFloat",pd.name,QString::number(pd.val->getFloat()),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterXMLVisitor::visit( RichString& pd )
{
	fillRichParameterAttribute("RichString",pd.name,pd.val->getString(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterXMLVisitor::visit( RichMatrix44f& pd )
{
	fillRichParameterAttribute("RichMatrix44f",pd.name,pd.pd->fieldDesc,pd.pd->tooltip);
	vcg::Matrix44f mat = pd.val->getMatrix44f();
	for(unsigned int ii = 0;ii < 16;++ii)
		parElem.setAttribute(QString("val")+QString::number(ii),QString::number(mat.V()[ii]));
}

void RichParameterXMLVisitor::visit( RichPoint3f& pd )
{
	fillRichParameterAttribute("RichPoint3f",pd.name,pd.pd->fieldDesc,pd.pd->tooltip);
	vcg::Point3f p = pd.val->getPoint3f();
	parElem.setAttribute("x",QString::number(p.X()));
	parElem.setAttribute("y",QString::number(p.Y()));
	parElem.setAttribute("z",QString::number(p.Z()));
}

void RichParameterXMLVisitor::visit( RichColor& pd )
{
	fillRichParameterAttribute("RichColor",pd.name,pd.pd->fieldDesc,pd.pd->tooltip);
	QColor p = pd.val->getColor();
	parElem.setAttribute("r",QString::number(p.red()));
	parElem.setAttribute("g",QString::number(p.green()));
	parElem.setAttribute("b",QString::number(p.blue()));
	parElem.setAttribute("a",QString::number(p.alpha()));
}

void RichParameterXMLVisitor::visit( RichAbsPerc& pd )
{
	fillRichParameterAttribute("RichAbsPerc",pd.name,QString::number(pd.val->getAbsPerc()),pd.pd->fieldDesc,pd.pd->tooltip);
	AbsPercDecoration* dec = reinterpret_cast<AbsPercDecoration*>(pd.pd);
	parElem.setAttribute("min",QString::number(dec->min));
	parElem.setAttribute("max",QString::number(dec->max));
}

void RichParameterXMLVisitor::visit( RichEnum& pd )
{
	fillRichParameterAttribute("RichEnum",pd.name,QString::number(pd.val->getEnum()),pd.pd->fieldDesc,pd.pd->tooltip);
	EnumDecoration* dec = reinterpret_cast<EnumDecoration*>(pd.pd);
	parElem.setAttribute("enum_cardinality",dec->enumvalues.size());
	for(int ii = 0; ii < dec->enumvalues.size();++ii)
		parElem.setAttribute(QString("enum_val")+QString::number(ii),dec->enumvalues.at(ii));

}

void RichParameterXMLVisitor::visit( RichFloatList& /*pd*/ )
{
	assert(0);
}

void RichParameterXMLVisitor::visit(RichDynamicFloat& pd)
{
	fillRichParameterAttribute("RichDynamicFloat",pd.name,QString::number(pd.val->getDynamicFloat()),pd.pd->fieldDesc,pd.pd->tooltip);
	DynamicFloatDecoration* dec = reinterpret_cast<DynamicFloatDecoration*>(pd.pd);
	parElem.setAttribute("min",QString::number(dec->min));
	parElem.setAttribute("max",QString::number(dec->max));
}

void RichParameterXMLVisitor::visit( RichOpenFile& pd )
{
	fillRichParameterAttribute("RichOpenFile",pd.name,pd.val->getFileName(),pd.pd->fieldDesc,pd.pd->tooltip);
	OpenFileDecoration* dec = reinterpret_cast<OpenFileDecoration*>(pd.pd);
	parElem.setAttribute("exts_cardinality",dec->exts.size());
	for(int ii = 0; ii < dec->exts.size();++ii)
		parElem.setAttribute(QString("ext_val")+QString::number(ii),dec->exts[ii]);
}

void RichParameterXMLVisitor::visit( RichSaveFile& pd )
{
	fillRichParameterAttribute("RichOpenFile",pd.name,pd.val->getFileName(),pd.pd->fieldDesc,pd.pd->tooltip);
	SaveFileDecoration* dec = reinterpret_cast<SaveFileDecoration*>(pd.pd);
	parElem.setAttribute("ext",dec->ext);
}

void RichParameterXMLVisitor::visit( RichMesh& pd )
{
	MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(pd.pd);
	fillRichParameterAttribute("RichMesh",pd.name,QString::number(dec->meshindex),pd.pd->fieldDesc,pd.pd->tooltip);
}

bool RichParameterFactory::create( const QDomElement& np,RichParameter** par )
{
	QString name=np.attribute("name");
	QString type=np.attribute("type");
	QString desc=np.attribute("description");
	QString tooltip=np.attribute("tooltip");


	qDebug("    Reading Param with name %s : %s",qPrintable(name),qPrintable(type));

	bool corrconv = false;
	if(type=="RichBool")    
	{ 
		QString val = np.attribute("value").toLower();
        if ((val != QString("true")) && (val != QString("false")))
			return false;
		*par = new RichBool(name,np.attribute("value")!=QString("false"),desc,tooltip); 
		return true; 
	}

	if(type=="RichInt")     
	{ 
		int val = np.attribute("value").toInt(&corrconv);
		if (!corrconv)
			return false;
		*par = new RichInt(name,val,desc,tooltip); 
		return true;
	}

	if(type=="RichFloat")   
	{ 
		float val = np.attribute("value").toFloat(&corrconv);
		if (!corrconv)
			return false;	
		*par = new RichFloat(name,val,desc,tooltip);
		return true;
	}

	if(type=="RichString")  
	{ 
		*par = new RichString(name,np.attribute("value"),desc,tooltip); 
		return true; 
	}
	
	if(type=="RichAbsPerc") 
	{ 
		float val = np.attribute("value").toFloat(&corrconv);
		if ((!corrconv) && (val >= 0.0f) && (val <= 100.0f))
			return false;
		float min = np.attribute("min").toFloat(&corrconv);
		if (!corrconv) 
			return false;
		float max = np.attribute("max").toFloat(&corrconv);
		if (!corrconv) 
			return false;
		*par = new RichAbsPerc(name,val,min,max,desc,tooltip); 
		return true; 
	}

	if(type=="RichColor")		
	{ 
		unsigned int r = np.attribute("r").toUInt(&corrconv);
    if ((!corrconv) && (r <= 255))
			return false;
		unsigned int g = np.attribute("g").toUInt(&corrconv); 
    if ((!corrconv) && (g <= 255))
			return false;
		unsigned int b = np.attribute("b").toUInt(&corrconv); 
    if ((!corrconv) && (b <= 255))
			return false;
		unsigned int a = np.attribute("a").toUInt(&corrconv);
    if ((!corrconv) && (a <= 255))
			return false;
		QColor col(r,g,b,a);
		*par= new RichColor(name,col,desc,tooltip); 
		return true; 
	}

	if(type=="RichMatrix44f")
	{
		Matrix44f mm;
		for(int i=0;i<16;++i)
		{
			float val = np.attribute(QString("val")+QString::number(i)).toFloat(&corrconv);
			if (!corrconv)
				return false;
			mm.V()[i]=val;
		}
		*par = new RichMatrix44f(name,mm,desc,tooltip);    
		return true;                    
	}

	if(type=="RichEnum")
	{
    QStringList list = QStringList();
		int enum_card = np.attribute(QString("enum_cardinality")).toUInt(&corrconv);
		if (!corrconv) 
			return false;

		for(int i=0;i<enum_card;++i)
			list<<np.attribute(QString("enum_val")+QString::number(i));

		int val = np.attribute("value").toInt(&corrconv);
		if ((!corrconv) && (val >=0) && (val < enum_card)) 
			return false;
		*par = new RichEnum(name,val,list,desc,tooltip);
		return true;
	}

	if(type == "RichMesh")  
	{ 
		int val = np.attribute("value").toInt(&corrconv);
		
		if (!corrconv) 
			return false;

		*par = new RichMesh(name, val,desc,tooltip); 
		return true; 
	}

	if(type == "RichFloatList")
	{
		//to be implemented
		assert(0);
	}

	if(type == "RichDynamicFloat")
	{
		float min = np.attribute("min").toFloat(&corrconv);
		if (!corrconv) 
			return false;
		float max = np.attribute("max").toFloat(&corrconv);
		if (!corrconv) 
			return false;

		float val = np.attribute("value").toFloat(&corrconv);
		if ((!corrconv) && (val >= min) && (val <= max))
			return false;

		*par = new RichDynamicFloat(name, val, min, max, desc, tooltip);
		return true;
	}

	if(type == "RichOpenFile")
	{ 
    QStringList list = QStringList();
		int exts_card = np.attribute(QString("exts_cardinality")).toUInt(&corrconv);
		if (!corrconv) 
			return false;

		for(int i=0;i<exts_card;++i)
			list<<np.attribute(QString("exts_val")+QString::number(i));

		QString defdir = np.attribute("value");

		*par = new RichOpenFile(name,defdir,list,desc,tooltip);
		return true;
	}

	if(type == "RichSaveFile")  
	{ 
		QString deffile = np.attribute("value");
		QString ext = np.attribute("ext");
		*par = new RichSaveFile(name,deffile,ext,desc,tooltip);
		return true;
	}

	if(type=="RichPoint3f")
	{
		Point3f val;
		val[0]=np.attribute("x").toFloat(&corrconv);
		if (!corrconv) 
			return false;
		val[1]=np.attribute("y").toFloat(&corrconv);
		if (!corrconv) 
			return false;
		val[2]=np.attribute("z").toFloat(&corrconv);
		if (!corrconv) 
			return false;

		*par = new RichPoint3f(name, val,desc,tooltip);  
		return true; 
	}

	return false;
}

BoolValue::BoolValue( const bool val ) : pval(val)
{

}

ParameterDecoration::ParameterDecoration( Value* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :fieldDesc(desc),tooltip(tltip),defVal(defvalue)
{

}

ParameterDecoration::~ParameterDecoration()
{
	delete defVal;
}

BoolDecoration::BoolDecoration( BoolValue* defvalue,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

IntDecoration::IntDecoration( IntValue* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

FloatDecoration::FloatDecoration( FloatValue* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

StringDecoration::StringDecoration( StringValue* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

Matrix44fDecoration::Matrix44fDecoration( Matrix44fValue* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

Point3fDecoration::Point3fDecoration( Point3fValue* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

ColorDecoration::ColorDecoration( ColorValue* defvalue,const QString desc /*= QString()*/,const QString tltip/*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

AbsPercDecoration::AbsPercDecoration( AbsPercValue* defvalue,const float minVal,const float maxVal,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip),min(minVal),max(maxVal)
{

}

EnumDecoration::EnumDecoration( EnumValue* defvalue, QStringList values,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip),enumvalues(values)
{

}

DynamicFloatDecoration::DynamicFloatDecoration( DynamicFloatValue* defvalue, const float minVal,const float maxVal,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip),min(minVal),max(maxVal)
{

}

SaveFileDecoration::SaveFileDecoration( FileValue* defvalue,const QString extension/*=QString(".*")*/,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip),ext(extension)
{

}

OpenFileDecoration::OpenFileDecoration( FileValue* directorydefvalue,const QStringList extensions,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(directorydefvalue,desc,tltip),exts(extensions)
{

}

MeshDecoration::MeshDecoration( MeshValue* defvalue,MeshDocument* doc,const QString desc/*=QString()*/, const QString tltip/*=QString()*/ ) :ParameterDecoration(defvalue,desc,tltip),meshdoc(doc)
{
	meshindex = -1;
	if (doc != NULL) meshindex = doc->meshList.indexOf(defvalue->getMesh()); 
	assert((meshindex != -1) || (doc == NULL));
}

MeshDecoration::MeshDecoration( int meshind,MeshDocument* doc,const QString desc/*=QString()*/, const QString tltip/*=QString()*/ ) :ParameterDecoration(NULL,desc,tltip),meshdoc(doc)
{
	assert(meshind < doc->size() && meshind >= 0); 
	meshindex = meshind;
	if (doc != NULL)
		defVal = new MeshValue(doc->meshList.at(meshind));
}

MeshDecoration::MeshDecoration( int meshind,const QString desc/*=QString()*/,const QString tooltip/*=QString()*/) :ParameterDecoration(NULL,desc,tooltip),meshdoc(NULL),meshindex(meshind)
{

}

RichParameter::RichParameter( const QString nm,Value* v,ParameterDecoration* prdec ) :name(nm),val(v),pd(prdec)
{

}

RichParameter::~RichParameter()
{
	delete val;delete pd;
}
/*
RichBool::RichBool( const QString nm,const bool defval) : RichParameter(nm,new BoolValue(defval),new BoolDecoration(new BoolValue(defval),"",""))
{

}
RichBool::RichBool( const QString nm,const bool defval,const QString desc) : RichParameter(nm,new BoolValue(defval),new BoolDecoration(new BoolValue(defval),desc,""))
{

}
*/
RichBool::RichBool( const QString nm,const bool defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) : RichParameter(nm,new BoolValue(defval),new BoolDecoration(new BoolValue(defval),desc,tltip))
{}


RichBool::RichBool( const QString nm,const bool val,const bool defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) : RichParameter(nm,new BoolValue(val),new BoolDecoration(new BoolValue(defval),desc,tltip))
{}

void RichBool::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichBool::operator==( const RichParameter& rb )
{
	return (rb.val->isBool() && (name == rb.name) && (val->getBool() == rb.val->getBool()));
}

RichBool::~RichBool()
{

}

RichInt::RichInt( const QString nm,const int defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new IntValue(defval),new IntDecoration(new IntValue(defval),desc,tltip))
{

}

RichInt::RichInt( const QString nm,const int val,const int defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ):RichParameter(nm,new IntValue(val),new IntDecoration(new IntValue(defval),desc,tltip))
{

}
void RichInt::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichInt::operator==( const RichParameter& rb )
{
	return (rb.val->isInt() &&(name == rb.name) && (val->getInt() == rb.val->getInt()));
}

RichInt::~RichInt()
{

}

RichFloat::RichFloat( const QString nm,const float defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new FloatValue(defval),new FloatDecoration(new FloatValue(defval),desc,tltip))
{

}

RichFloat::RichFloat( const QString nm,const float val,const float defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new FloatValue(val),new FloatDecoration(new FloatValue(defval),desc,tltip))
{

}
void RichFloat::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichFloat::operator==( const RichParameter& rb )
{
	return (rb.val->isFloat() &&(name == rb.name) && (val->getFloat() == rb.val->getFloat()));
}

RichFloat::~RichFloat()
{

}

RichString::RichString( const QString nm,const QString defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new StringValue(defval),new StringDecoration(new StringValue(defval),desc,tltip))
{

}

RichString::RichString( const QString nm,const QString val,const QString defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ):RichParameter(nm,new StringValue(val),new StringDecoration(new StringValue(defval),desc,tltip))
{

}

RichString::RichString( const QString nm,const QString defval ) : RichParameter(nm,new StringValue(defval),new StringDecoration(new StringValue(defval),"",""))
{

}

RichString::RichString( const QString nm,const QString defval,const QString desc ) : RichParameter(nm,new StringValue(defval),new StringDecoration(new StringValue(defval),desc,""))
{

}

void RichString::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichString::operator==( const RichParameter& rb )
{
	return (rb.val->isString() &&(name == rb.name) && (val->getString() == rb.val->getString()));
}

RichString::~RichString()
{

}

RichMatrix44f::RichMatrix44f( const QString nm,const vcg::Matrix44f& defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new Matrix44fValue(defval),new Matrix44fDecoration(new Matrix44fValue(defval),desc,tltip))
{

}

RichMatrix44f::RichMatrix44f( const QString nm,const vcg::Matrix44f& val,const vcg::Matrix44f& defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ):RichParameter(nm,new Matrix44fValue(val),new Matrix44fDecoration(new Matrix44fValue(defval),desc,tltip))
{

}
void RichMatrix44f::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichMatrix44f::operator==( const RichParameter& rb )
{
	return (rb.val->isMatrix44f() &&(name == rb.name) && (val->getMatrix44f() == rb.val->getMatrix44f()));
}

RichMatrix44f::~RichMatrix44f()
{

}

RichPoint3f::RichPoint3f( const QString nm,const vcg::Point3f defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new Point3fValue(defval),new Point3fDecoration(new Point3fValue(defval),desc,tltip))
{

}

RichPoint3f::RichPoint3f( const QString nm,const vcg::Point3f val,const vcg::Point3f defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ):RichParameter(nm,new Point3fValue(val),new Point3fDecoration(new Point3fValue(defval),desc,tltip))
{

}
void RichPoint3f::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichPoint3f::operator==( const RichParameter& rb )
{
	return (rb.val->isPoint3f() &&(name == rb.name) && (val->getPoint3f() == rb.val->getPoint3f()));
}

RichPoint3f::~RichPoint3f()
{

}

RichColor::RichColor( const QString nm,const QColor defval,const QString desc,const QString tltip ) :RichParameter(nm,new ColorValue(defval),new ColorDecoration(new ColorValue(defval),desc,tltip))
{

}

RichColor::RichColor( const QString nm,const QColor val,const QColor defval,const QString desc,const QString tltip ) :RichParameter(nm,new ColorValue(val),new ColorDecoration(new ColorValue(defval),desc,tltip))
{

}

RichColor::RichColor( const QString nm,const QColor defval )
:RichParameter(nm,new ColorValue(defval),new ColorDecoration(new ColorValue(defval),"",""))
{
	
}

RichColor::RichColor( const QString nm,const QColor defval,const QString desc )
:RichParameter(nm,new ColorValue(defval),new ColorDecoration(new ColorValue(defval),desc,""))
{
}

void RichColor::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichColor::operator==( const RichParameter& rb )
{
	return (rb.val->isColor() &&(name == rb.name) && (val->getColor() == rb.val->getColor()));
}

RichColor::~RichColor()
{

}

RichAbsPerc::RichAbsPerc( const QString nm,const float defval,const float minval,const float maxval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm, new AbsPercValue(defval), new AbsPercDecoration(new AbsPercValue(defval),minval,maxval,desc,tltip))
{

}

RichAbsPerc::RichAbsPerc( const QString nm,const float val,const float defval,const float minval,const float maxval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ):RichParameter(nm, new AbsPercValue(val), new AbsPercDecoration(new AbsPercValue(defval),minval,maxval,desc,tltip))
{

}
void RichAbsPerc::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichAbsPerc::operator==( const RichParameter& rb )
{
	return (rb.val->isAbsPerc() &&(name == rb.name) && (val->getAbsPerc() == rb.val->getAbsPerc()));
}

RichAbsPerc::~RichAbsPerc()
{

}

RichEnum::RichEnum( const QString nm,const int defval,const QStringList values,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new EnumValue(defval),new EnumDecoration(new EnumValue(defval),values,desc,tltip))
{

}

RichEnum::RichEnum( const QString nm,const int val,const int defval,const QStringList values,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ):RichParameter(nm,new EnumValue(val),new EnumDecoration(new EnumValue(defval),values,desc,tltip))
{

}
void RichEnum::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichEnum::operator==( const RichParameter& rb )
{
	return (rb.val->isEnum() &&(name == rb.name) && (val->getEnum() == rb.val->getEnum()));
}

RichEnum::~RichEnum()
{

}

RichMesh::RichMesh( const QString nm,MeshModel* defval,MeshDocument* doc,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm, new MeshValue(defval),new MeshDecoration( new MeshValue(defval),doc,desc,tltip))
{

}

RichMesh::RichMesh( const QString nm,int meshindex,MeshDocument* doc,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,NULL, new MeshDecoration(meshindex,doc,desc,tltip))
{
	assert(meshindex < doc->size() && meshindex >= 0); 
	val = new MeshValue(doc->meshList.at(meshindex));
}

RichMesh::RichMesh( const QString nm,int meshindex,const QString /*desc*//*=QString()*/,const QString /*tltip*//*=QString()*/  ) :RichParameter(nm,new MeshValue(NULL),new MeshDecoration(meshindex))
{

}

RichMesh::RichMesh( const QString nm,MeshModel* val,MeshModel* defval,MeshDocument* doc,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm, new MeshValue(val),new MeshDecoration( new MeshValue(defval),doc,desc,tltip))
{

}
void RichMesh::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichMesh::operator==( const RichParameter& rb )
{
	return (rb.val->isMesh() &&(name == rb.name) && (val->getMesh() == rb.val->getMesh()));
}

RichMesh::~RichMesh()
{

}

RichFloatList::RichFloatList( const QString nm,FloatListValue* v,FloatListDecoration* prdec ) :RichParameter(nm,v,prdec)
{

}

RichFloatList::RichFloatList( const QString nm,FloatListValue* /*val*/,FloatListValue* v,FloatListDecoration* prdec ) :RichParameter(nm,v,prdec)
{

}

void RichFloatList::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichFloatList::operator==( const RichParameter& rb )
{
	return (rb.val->isFloatList() &&(name == rb.name) && (val->getFloatList() == rb.val->getFloatList()));
}

RichFloatList::~RichFloatList()
{

}

RichDynamicFloat::RichDynamicFloat( const QString nm,const float defval,const float minval,const float maxval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new DynamicFloatValue(defval),new DynamicFloatDecoration(new DynamicFloatValue(defval),minval,maxval,desc,tltip))
{

}

RichDynamicFloat::RichDynamicFloat( const QString nm,const float val,const float defval,const float minval,const float maxval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new DynamicFloatValue(val),new DynamicFloatDecoration(new DynamicFloatValue(defval),minval,maxval,desc,tltip))
{

}

void RichDynamicFloat::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichDynamicFloat::operator==( const RichParameter& rb )
{
	return (rb.val->isDynamicFloat() &&(name == rb.name) && (val->getDynamicFloat() == rb.val->getDynamicFloat()));
}

RichDynamicFloat::~RichDynamicFloat()
{

}

RichOpenFile::RichOpenFile( const QString nm,const QString directorydefval,const QStringList exts /*= QString("*.*")*/,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ):RichParameter(nm,new FileValue(directorydefval),new OpenFileDecoration(new FileValue(directorydefval),exts,desc,tltip))
{

}

void RichOpenFile::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichOpenFile::operator==( const RichParameter& rb )
{
	return (rb.val->isFileName() &&(name == rb.name) && (val->getFileName() == rb.val->getFileName()));
}

RichOpenFile::~RichOpenFile()
{

}

RichSaveFile::RichSaveFile( const QString nm,const QString filedefval,const QString ext,const QString desc/*=QString()*/,const QString tltip/*=QString()*/) :RichParameter(nm,new FileValue(filedefval),new SaveFileDecoration(new FileValue(filedefval),ext,desc,tltip))
{

}

void RichSaveFile::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichSaveFile::operator==( const RichParameter& rb )
{
	return (rb.val->isFileName() &&(name == rb.name) && (val->getFileName() == rb.val->getFileName()));
}

RichSaveFile::~RichSaveFile()
{

}
