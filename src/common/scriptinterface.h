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

#ifndef SCRIPTINTERFACE_H
#define SCRIPTINTERFACE_H

#include <QtScript>
#include "filterparameter.h"
#include "xmlfilterinfo.h"
#include "meshmodel.h"
#include "vcg/space/point3.h"

class ScriptAdapterGenerator
{
private:
	QString parNames(const RichParameterSet& set) const;
	QString parNames(const QString&  filterName,const XMLFilterInfo& xmlInfo) const;
	static inline QString optName() {return QString("optional");}
public:
	inline static const QString meshDocVarName() {return QString("meshDoc");}
	static const QString xmlSchemaFile() {return QString(":/script_system/meshlabfilterXMLspecificationformat.xsd");}
	static const QStringList javaScriptLibraryFiles();

	
	QString funCodeGenerator(const QString&  filtername,const RichParameterSet& set) const;
	QString funCodeGenerator(const QString&  filtername,const XMLFilterInfo& xmlInfo) const;
};


QScriptValue PluginInterfaceInit(QScriptContext *context, QScriptEngine *engine, void * param);
QScriptValue PluginInterfaceApply(QScriptContext *context, QScriptEngine *engine, void * param);
QScriptValue PluginInterfaceApplyXML(QScriptContext *context, QScriptEngine *engine, void * param);

QScriptValue IRichParameterSet_prototype_setBool(QScriptContext* c,QScriptEngine* e);
QScriptValue IRichParameterSet_prototype_setInt(QScriptContext* c,QScriptEngine* e);
QScriptValue IRichParameterSet_prototype_setAbsPerc(QScriptContext* c,QScriptEngine* e);
QScriptValue IRichParameterSet_prototype_setFloat(QScriptContext* c,QScriptEngine* e);

QScriptValue IRichParameterSet_ctor(QScriptContext* c,QScriptEngine* e);

QScriptValue myprint (QScriptContext* sc, QScriptEngine* se);

void registerTypes(QScriptEngine* eng);


class MeshDocumentScriptInterface : public QObject
{
	Q_OBJECT

public:
	MeshDocumentScriptInterface(MeshDocument* doc);
	~MeshDocumentScriptInterface(){}

	Q_INVOKABLE MeshModelScriptInterface* getMesh(const int meshId);
	Q_INVOKABLE MeshModelScriptInterface* current();
	Q_INVOKABLE int setCurrent(const int meshId);
	MeshDocument* md;
};

class MeshModelScriptInterface : public QObject
{
	Q_OBJECT

public:
	MeshModelScriptInterface(MeshModel& meshModel,MeshDocumentScriptInterface* mdsi);
	
	Q_INVOKABLE float bboxDiag() const;
	Q_INVOKABLE vcg::Point3f bboxMin() const;
	Q_INVOKABLE vcg::Point3f bboxMax() const;

	MeshModel& mm;
};

Q_DECLARE_METATYPE(MeshDocumentScriptInterface*)
QScriptValue MeshDocumentScriptInterfaceToScriptValue(QScriptEngine* eng,MeshDocumentScriptInterface* const& in);

void MeshDocumentScriptInterfaceFromScriptValue(const QScriptValue& val,MeshDocumentScriptInterface*& out);

Q_DECLARE_METATYPE(MeshModelScriptInterface*)

QScriptValue MeshModelScriptInterfaceToScriptValue(QScriptEngine* eng,MeshModelScriptInterface* const& in);

void MeshModelScriptInterfaceFromScriptValue(const QScriptValue& val,MeshModelScriptInterface*& out);

//Q_DECLARE_METATYPE(vcg::Point3f)
//
//QScriptValue Point3fToScriptValue(QScriptEngine* eng,const vcg::Point3f& in);
//
//void Point3fFromScriptValue(const QScriptValue& val,vcg::Point3f& out);




//QScriptValue MeshModelScriptInterface_ctor(QScriptContext* c,QScriptEngine* e)
//{
//	QString x = c->argument(0).toInt32();
//	return e->toScriptValue(MeshModelScriptInterface(x));
//}

class Env :public QScriptEngine
{
	Q_OBJECT

public:
	Env();
	Q_INVOKABLE void insertExpressionBinding(const QString& nm,const QString& exp);
};

QScriptValue Env_ctor(QScriptContext *context,QScriptEngine *engine);

Q_DECLARE_METATYPE(Env*)

class EnvWrap
{
private:
	bool constStatement(const QString& statement) const;
	QScriptValue evalExp( const QString& nm );
	Env* env;
public:
	EnvWrap():env(NULL){};
	EnvWrap(Env& envir);
	bool evalBool(const QString& nm);
	double evalDouble(const QString& nm);
	float evalFloat( const QString& nm );
	vcg::Point3f evalVec3(const QString& nm);
	QColor evalColor(const QString& nm);
	/*QString getExpType(const QString& nm);*/
	QString evalString(const QString& nm);
};

QScriptValue EnvWrap_ctor(QScriptContext* c,QScriptEngine* e);

//class EnvWrap : protected virtual QScriptEngine
//{
//private:
//	bool constStatement(const QString& statement) const;
//	QScriptValue getExp( const QString& nm );
//public:
//	EnvWrap();
//	bool getBool(const QString& nm);
//	float getFloat(const QString& nm);
//};
//
//
//class Env : public EnvWrap, public virtual QScriptEngine
//{
//public:
//	Env();
//	void insertExpressionBinding(const QString& nm,const QString& exp);
//};


#endif
