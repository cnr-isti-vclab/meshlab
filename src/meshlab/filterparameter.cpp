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

//bool RichParameterSet::hasParameter(QString name)
//{
//	QList<FilterParameter>::iterator fpli;
//	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
//		if((*fpli).fieldName==name)
//				return true;
//				
//	return false;
//}
//FilterParameter *RichParameterSet::findParameter(QString name)
//{
//	QList<FilterParameter>::iterator fpli;
//	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
//		if((*fpli).fieldName==name)
//				return &*fpli;
//		
//	qDebug("FilterParameter Warning: Unable to find a parameter with name '%s',\n"
//				 "      Please check types and names of the parameter in the calling filter",qPrintable(name));
//	return 0;
//}
//
//const FilterParameter *RichParameterSet::findParameter(QString name) const
//{
//	QList<FilterParameter>::const_iterator fpli;
//	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
//		if((*fpli).fieldName==name)
//				return &*fpli;
//		
//	qDebug("FilterParameter Warning: Unable to find a parameter with name '%s',\n"
//				 "      Please check types and names of the parameter in the calling filter",qPrintable(name));
//	return 0;
//}
//
//void RichParameterSet::removeParameter(QString name){
//	paramList.removeAll(*findParameter(name));
//}
//
////--------------------------------------
//
//void RichParameterSet::addBool (QString name, bool defaultVal, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//  p.fieldVal=defaultVal;
//	p.fieldType=FilterParameter::PARBOOL;
//	paramList.push_back(p);	
//}
//
//void RichParameterSet::setBool(QString name, bool newVal)
//{
//	FilterParameter *p=findParameter(name);
//	assert(p);
//	p->fieldVal=QVariant(newVal);
//}
//bool RichParameterSet::getBool(QString name) const
//{
//	const FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARBOOL);
//	return p->fieldVal.toBool();
//}
//
////--------------------------------------
//
//void RichParameterSet::addInt(QString name, int       defaultVal, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//  p.fieldVal=defaultVal;
//	p.fieldType=FilterParameter::PARINT;
//	paramList.push_back(p);		
//}
//int	 RichParameterSet::getInt(QString name) const
//{
//	const FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARINT);
//	return p->fieldVal.toInt();
//}
//void RichParameterSet::setInt(QString name, int newVal)
//{
//	FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARINT);
//	p->fieldVal=QVariant(newVal);	
//}
//
////--------------------------------------
//
//void  RichParameterSet::addFloat(QString name, float defaultVal, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//  p.fieldVal=defaultVal;
//	p.fieldType=FilterParameter::PARFLOAT;
//	paramList.push_back(p);		
//}
//float RichParameterSet::getFloat(QString name) const
//{
//	const FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARFLOAT);
//	return float(p->fieldVal.toDouble());
//}
//void  RichParameterSet::setFloat(QString name, float newVal)
//{
//	FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARFLOAT);
//	p->fieldVal=QVariant(double(newVal));	
//}
//
////--------------------------------------
//void  RichParameterSet::addColor(QString name, QColor defaultVal, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//  p.fieldVal=defaultVal.rgb(); // it is converted to an unsigned int
//	p.fieldType=FilterParameter::PARCOLOR;
//	paramList.push_back(p);		
//}
//
//Color4b RichParameterSet::getColor4b(QString name) const
//{
//	QColor c=getColor(name);
//	return Color4b(c.red(),c.green(),c.blue(),255);
//}
//
//QColor RichParameterSet::getColor(QString name) const
//{
//	const FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARCOLOR);
//	return QColor(QRgb(p->fieldVal.toUInt()));
//}
//
//void  RichParameterSet::setColor(QString name, QColor newVal)
//{
//	FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARCOLOR);
//	p->fieldVal=QVariant(newVal.rgb());	
//}
//
////--------------------------------------
//
//void RichParameterSet::addString   (QString name, QString   defaultVal, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//  p.fieldVal=defaultVal; 
//	p.fieldType=FilterParameter::PARSTRING;
//	paramList.push_back(p);		
//}
//QString RichParameterSet::getString(QString name) const
//{
//	const FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARSTRING);
//	return p->fieldVal.toString();
//}
//void RichParameterSet::setString(QString name, QString newVal)
//{
//	FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARSTRING);
//	p->fieldVal=QVariant(newVal);	
//}
//
////--------------------------------------
//
//
//Matrix44f		RichParameterSet::getMatrix44(QString name) const
//{
//	const FilterParameter *p=findParameter(name);
//
//	assert(p);
//	assert(p->fieldType==FilterParameter::PARMATRIX);
//	assert(p->fieldVal.type()==QVariant::List);
//
//	Matrix44f matrix;
//	QList<QVariant> matrixVals = p->fieldVal.toList();
//	assert(matrixVals.size()==16);
//	for(int i=0;i<16;++i)
//		matrix.V()[i]=matrixVals[i].toDouble();
//			return matrix;
//}
//void RichParameterSet::addMatrix44 (QString name, Matrix44f defaultVal, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
// 
//	QList<QVariant> matrixVals;
//    for(int i=0;i<16;++i)
//        matrixVals.append(defaultVal.V()[i]);
//	p.fieldVal=matrixVals;
//	p.fieldType=FilterParameter::PARMATRIX;
//	paramList.push_back(p);		
//}
//
//
//void RichParameterSet::setMatrix44(QString , Matrix44f )
//{
//	assert(0);
//}
//
////--------------------------------------
//
//Point3f		RichParameterSet::getPoint3f(QString name) const
//{
//	const FilterParameter *p=findParameter(name);
//	
//	assert(p);
//	assert(p->fieldType==FilterParameter::PARPOINT3F);
//	assert(p->fieldVal.type()==QVariant::List);
//	
//	Point3f point;
//	QList<QVariant> pointVals = p->fieldVal.toList();
//	assert(pointVals.size()==3);
//	for(int i=0;i<3;++i)
//		point[i]=pointVals[i].toDouble();
//	return point;
//}
//void RichParameterSet::addPoint3f (QString name, Point3f defaultVal, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//	
//	QList<QVariant> pointVals;
//	for(int i=0;i<3;++i)
//		pointVals.append(defaultVal[i]);
//	p.fieldVal=pointVals;
//	p.fieldType=FilterParameter::PARPOINT3F;
//	paramList.push_back(p);		
//}
//void  RichParameterSet::setPoint3f(QString name, Point3f newVal)
//{
//	FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARPOINT3F);
//	QList<QVariant> pointVals;
//	for(int i=0;i<3;++i)
//		pointVals.append(newVal[i]);
//	p->fieldVal=pointVals;
//}
////--------------------------------------
//
//void RichParameterSet::addAbsPerc (QString name, float defaultVal, float minVal, float maxVal, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//  p.fieldVal=defaultVal;
//	p.fieldType=FilterParameter::PARABSPERC;
//	p.min=minVal;
//	p.max=maxVal;
//	paramList.push_back(p);	
//}
//float		RichParameterSet::getAbsPerc(QString name) const
//{
//	const FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARABSPERC);
//	return float(p->fieldVal.toDouble());
//}
//void RichParameterSet::setAbsPerc(QString name, float newVal)
//{
//	FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARABSPERC);
//	p->fieldVal=QVariant(double(newVal));	
//}
//
//void RichParameterSet::addEnum (QString name, int defaultVal, QStringList values, QString desc, QString tooltip) {
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//  p.fieldVal=defaultVal;
//	p.fieldType=FilterParameter::PARENUM;
//	p.enumValues = values;
//	paramList.push_back(p);	
//}
//
//int RichParameterSet::getEnum(QString name) const {
//	const FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARENUM);
//	return float(p->fieldVal.toInt());
//}
//
//void RichParameterSet::setEnum(QString name, int newVal)
//{
//	FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARENUM);
//	p->fieldVal=QVariant(int(newVal));	
//}
//
//void RichParameterSet::addFloatList(QString name, QList<float> &defaultValue, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//	
//	QList<QVariant> tempList;
//	for(int i = 0; i < defaultValue.size(); ++i)
//	{
//		//if you put the float in directly int converts to a double which we do not want
//		tempList.push_back(QVariant(QString().setNum(defaultValue.at(i), 'g', 12)));
//		//qDebug() << "putting down " << QString().setNum(defaultValue.at(i), 'g', 12) ;
//	}
//	
//	p.fieldVal = tempList;
//	p.fieldType = FilterParameter::PARFLOATLIST;
//	paramList.push_back(p);
//}
//
//QList<float> RichParameterSet::getFloatList(QString name) const
//{
//	const FilterParameter *p = findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARFLOATLIST);
//	
//	QList<float> floatList;
//	QList<QVariant> internalList = p->fieldVal.toList();
//	for(int i = 0; i < internalList.size(); ++i)
//		floatList.push_back(internalList.at(i).toString().toFloat());	
//	
//	return floatList;
//}
//
//void RichParameterSet::setFloatList(QString name, QList<float> &newValue)
//{
//	FilterParameter *p = findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARFLOATLIST);
//	
//	QList<QVariant> tempList;
//	for(int i = 0; i < newValue.size(); ++i)
//	{
//		tempList.push_back(QVariant(QString().setNum(newValue.at(i), 'g', 12)));
//		//qDebug() << "insetfloatlist " << QVariant(QString().setNum(newValue.at(i), 'g', 12));
//	}
//	p->fieldVal = tempList;
//}
//
///* ---- */
//
//void RichParameterSet::addMesh (QString name, MeshModel *defaultVal, QString desc, QString tooltip) {
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//  p.pointerVal= defaultVal;
//	p.fieldType=FilterParameter::PARMESH;
//	paramList.push_back(p);	
//}
//
////make the default the mesh that is at the given position in the mesh document
//void RichParameterSet::addMesh(QString name, int position,  QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//	p.fieldVal = position;
//	p.pointerVal = NULL;
//	p.fieldType = FilterParameter::PARMESH;
//	paramList.push_back(p);	
//}
//
//MeshModel * RichParameterSet::getMesh(QString name) const {
//	const FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARMESH);
//	return (MeshModel *)(p->pointerVal);
//}
//
//void RichParameterSet::setMesh(QString name, MeshModel * newVal, int position)
//{
//	FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARMESH);
//	p->fieldVal = position;
//	p->pointerVal= newVal;
//}
//
///* ---- */
///* Dynamic Float Members*/
///* ---- */
//
//void RichParameterSet::addDynamicFloat(QString name, float defaultVal, float minVal, float maxVal, QString desc , QString tooltip )
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//	assert(defaultVal<=maxVal);
//	assert(defaultVal>=minVal);
//  p.fieldVal=defaultVal;
//	p.fieldType=FilterParameter::PARDYNFLOAT;
//	p.min=minVal;
//	p.max=maxVal;
//	//p.mask=changeMask;
//	paramList.push_back(p);	
//}
//
//float RichParameterSet::getDynamicFloat(QString name) const
//{
//	const FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARDYNFLOAT);
//	return float(p->fieldVal.toDouble());
//}
//
//void  RichParameterSet::setDynamicFloat(QString name, float newVal)
//{
//	FilterParameter *p=findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARDYNFLOAT);
//	p->fieldVal=QVariant(newVal);	
//}
//
///* PAROPENFILENAME */
//
//void RichParameterSet::addOpenFileName(QString name, QString defaultVal, QString extension, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//	p.fieldVal = defaultVal; 
//	p.fieldType = FilterParameter::PAROPENFILENAME;
//	
//	//add the extension to this unused variable because i think it is cleaner than adding another variable not used anywhere else
//	p.enumValues.push_back(extension);
//	
//	paramList.push_back(p);
//}
//
//QString RichParameterSet::getOpenFileName(QString name) const
//{
//	const FilterParameter *p = findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PAROPENFILENAME);
//	return p->fieldVal.toString();
//}
//
//void RichParameterSet::setOpenFileName(QString name, QString newVal)
//{
//	FilterParameter *p = findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PAROPENFILENAME);
//	p->fieldVal = QVariant(newVal);	
//}
//
///* PARSAVEFILENAME */
//
//void RichParameterSet::addSaveFileName(QString name, QString defaultVal, QString extension, QString desc, QString tooltip)
//{
//	assert(!hasParameter(desc));
//	FilterParameter p(name,desc,tooltip);
//	p.fieldVal = defaultVal; 
//	p.fieldType = FilterParameter::PARSAVEFILENAME;
//		
//	//add the extension to this unused variable because i think it is cleaner than adding another variable not used anywhere else
//	p.enumValues.push_back(extension);
//	
//	paramList.push_back(p);
//}
//
//QString RichParameterSet::getSaveFileName(QString name) const
//{
//	const FilterParameter *p = findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARSAVEFILENAME);
//	return p->fieldVal.toString();
//}
//
//void RichParameterSet::setSaveFileName(QString name, QString newVal)
//{
//	FilterParameter *p = findParameter(name);
//	assert(p);
//	assert(p->fieldType == FilterParameter::PARSAVEFILENAME);
//	p->fieldVal = QVariant(newVal);	
//}
//
//QDomElement FilterParameter::createElement(QDomDocument &doc)
//{
//	QDomElement parElem = doc.createElement("Param");
//	//parElem.setAttribute("name",this->fieldName);
//	//switch (this->fieldType)
//	//{
//	//	case FilterParameter::PARBOOL:
// //       parElem.setAttribute("type","Bool");
// //       parElem.setAttribute("value",this->fieldVal.toString());
//	//	break;
// //   case FilterParameter::PARSTRING:
// //       parElem.setAttribute("type","String");
// //       parElem.setAttribute("value",this->fieldVal.toString());
// //   break;
// //   case FilterParameter::PARINT:
// //       parElem.setAttribute("type","Int");
// //       parElem.setAttribute("value",this->fieldVal.toInt());
// //   break;
// //   case FilterParameter::PARFLOAT:
// //       parElem.setAttribute("type","Float");
// //       parElem.setAttribute("value",this->fieldVal.toString());
// //   break;
// //   case FilterParameter::PARABSPERC:
// //       parElem.setAttribute("type","AbsPerc");
// //       parElem.setAttribute("value",this->fieldVal.toString());
// //       parElem.setAttribute("min",QString::number(this->min));
// //       parElem.setAttribute("max",QString::number(this->max));
// //   break;
// //   case FilterParameter::PARCOLOR:
// //       parElem.setAttribute("type","Color");
// //       parElem.setAttribute("rgb",this->fieldVal.toString());
// //   break;
// //   case FilterParameter::PARENUM:
//	//	{
// //       parElem.setAttribute("type","Enum");
// //       parElem.setAttribute("value",this->fieldVal.toString());
// //       QStringList::iterator kk;
// //       for(kk = this->enumValues.begin();kk!=this->enumValues.end();++kk){
// //       	QDomElement sElem = doc.createElement("EnumString");
// //       	sElem.setAttribute("value",(*kk));
// //       	parElem.appendChild(sElem);
// //       }
//	//	}
// //   break;
// //   case FilterParameter::PARMATRIX:
// //       {
//	//			parElem.setAttribute("type","Matrix44");
// //       QList<QVariant> matrixVals = this->fieldVal.toList();
// //       for(int i=0;i<16;++i)
// //         parElem.setAttribute(QString("val")+QString::number(i),matrixVals[i].toString());
//	//				}
// //   break;
// //   case FilterParameter::PARMESH:
// //   	  parElem.setAttribute(TypeName(), MeshPointerName());
// //   	  //this is the mesh's position in the mesh document that was used
// //   	  parElem.setAttribute(ValueName(),(this->fieldVal.toString()));
// //   break;
// //   case FilterParameter::PARFLOATLIST:
//	//	{
// //   	parElem.setAttribute(TypeName(), FloatListName());
// //   	QList<QVariant> values = this->fieldVal.toList();
//	//		for(int i=0; i < values.size(); ++i)
//	//			{
//	//				QDomElement listElement = doc.createElement(ItemName());
//	//				listElement.setAttribute(ValueName(), values[i].toString());
//	//				parElem.appendChild(listElement);
// //       }
//	//	}
// //   break;
// //   case FilterParameter::PARDYNFLOAT:
// //   	  parElem.setAttribute(TypeName(), DynamicFloatName());
// //   	  parElem.setAttribute(ValueName(), this->fieldVal.toString());
// //   	  parElem.setAttribute(MinName(), QString::number(this->min));
// //   	  parElem.setAttribute(MaxName(), QString::number(this->max));
// //   	  //parElem.setAttribute(MaskName(),QString::number(this->mask));
// //   break;
// //   case FilterParameter::PAROPENFILENAME:
// //   	  parElem.setAttribute(TypeName(), OpenFileNameName());
// //   	  parElem.setAttribute(ValueName(), this->fieldVal.toString());
// //   break;
// //   case FilterParameter::PARSAVEFILENAME:
// //       parElem.setAttribute(TypeName(), SaveFileNameName());
// //       parElem.setAttribute(ValueName(), this->fieldVal.toString());
//	//	break;
//	//	    case FilterParameter::PARPOINT3F:
//	//			{
// //       QList<QVariant> pointVals = this->fieldVal.toList();
//	//			parElem.setAttribute("type","Point3f");
// //       parElem.setAttribute("x",QString::number(pointVals[0].toDouble()));
// //       parElem.setAttribute("y",QString::number(pointVals[1].toDouble()));
// //       parElem.setAttribute("z",QString::number(pointVals[2].toDouble()));
//	//			}
//	//			break;
// //   default: assert(0);
// //   }
//	return parElem;
//	}
//	
//void FilterParameter::addQDomElement(RichParameterSet &par, QDomElement &np)
//{
//		QString name=np.attribute("name");
//		QString type=np.attribute("type");
//
//		qDebug("    Reading Param with name %s : %s",qPrintable(name),qPrintable(type));
//
//		if(type=="Bool")    { par.addParam(new RichBool(name,np.attribute("value")!=QString("false")); return; }
//		if(type=="Int")     { par.addParam(new RichInt(name,np.attribute("value").toInt()); return; }
//		if(type=="Float")   { par.addParam(new RichFloat(name,np.attribute("value").toDouble()); return; }
//		if(type=="String")  { par.addParam(new RichString(name,np.attribute("value")); return; }
//		if(type=="AbsPerc") { par.addParam(new RichAbsPerc(name,np.attribute("value").toFloat(),np.attribute("min").toFloat(),np.attribute("max").toFloat()); return; }
//		if(type=="Color")		{ par.addParam(new RichColor(name,QColor::QColor(np.attribute("rgb").toUInt())); return; }
//		if(type=="Matrix44")
//		{
//		  Matrix44f mm;
//			for(int i=0;i<16;++i)
//					mm.V()[i]=np.attribute(QString("val")+QString::number(i)).toDouble();
//			par.addParam(new RichMatrix44(name,mm);    
//			return;                    
//		}
//		if(type=="Enum")
//		{
//			QStringList list = QStringList::QStringList();
//			for(QDomElement ns = np.firstChildElement("EnumString"); !ns.isNull(); ns = ns.nextSiblingElement("EnumString")){
//				list<<ns.attribute("value");
//			}
//			par.addParam(new RichEnum(name,np.attribute("value").toInt(),list);
//			return;
//		}
//		
//		if(type == MeshPointerName())  { par.addParam(new RichMesh(name, np.attribute(ValueName()).toInt()); return; }
//		if(type == FloatListName())
//		{
//			QList<float> values;
//			for(QDomElement listItem = np.firstChildElement(ItemName());
//					!listItem.isNull();
//					listItem = listItem.nextSiblingElement(ItemName()))
//			{
//					values.append(listItem.attribute(ValueName()).toFloat()); 
//				}
//			par.addParam(new RichFloatList(name,values);
//			return;
//		}
//			
//		if(type == DynamicFloatName())  { par.addParam(new RichDynamicFloat(name, np.attribute(ValueName()).toFloat(), np.attribute(MinName()).toFloat(), np.attribute(MaxName()).toFloat()); return; }
//		if(type == OpenFileNameName())  { par.addParam(new RichOpenFileName(name, np.attribute(ValueName())); return; }
//		if(type == SaveFileNameName())  { par.addParam(new RichSaveFileName(name, np.attribute(ValueName())); return; }
//		if(type=="Point3f") 
//		{
//			Point3f val;
//			val[0]=np.attribute("x").toFloat();
//			val[1]=np.attribute("y").toFloat();
//			val[2]=np.attribute("z").toFloat();
//			par.addParam(new RichPoint3f(name, val);  
//			return; 
//		}
//
//		assert(0); // we are trying to parse an unknown xml element
//}

/********************/

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
