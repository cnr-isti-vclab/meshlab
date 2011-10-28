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
	QString parNames(const QString&  filterName,const MLXMLPluginInfo& xmlInfo) const;
	static inline QString optName() {return QString("optional");}
public:
	inline static const QString meshDocVarName() {return QString("meshDoc");}
	static const QStringList javaScriptLibraryFiles();

	QString mergeOptParamsCodeGenerator() const;
	QString funCodeGenerator(const QString&  filtername,const RichParameterSet& set) const;
	QString funCodeGenerator(const QString&  filtername,const MLXMLPluginInfo& xmlInfo) const;
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

class ScriptInterfaceUtilities
{
public:
	static QVector<float> vcgPointToVector(const vcg::Point3f& p);
};

//class VCGPoint3fScriptInterface : public QObject
//{
//	Q_OBJECT
//public:
//	static QString getterSetterCode();
//	VCGPoint3fScriptInterface(vcg::Point3f& p);
//
//	float operator[](const int ii);
//	vcg::Point3f& pp;
//};
//
//QScriptValue VCGPoint3fScriptInterface_ctor(QScriptContext *context, QScriptEngine *engine);

class VCGVertexScriptInterface : public QObject
{
	Q_OBJECT 
public:
	VCGVertexScriptInterface(CMeshO::VertexType& v);

	//Q_INVOKABLE vcg::Point3f* p();
	Q_INVOKABLE QVector<float> getP();
	Q_INVOKABLE void setP(const float x,const float y,const float z);
	Q_INVOKABLE QVector<float> getN();
	Q_INVOKABLE void setN(const float x,const float y,const float z);

	CMeshO::VertexType& vv;
};

class MeshDocumentScriptInterface : public QObject
{
	Q_OBJECT

public:
	MeshDocumentScriptInterface(MeshDocument* doc);
	~MeshDocumentScriptInterface(){}

	Q_INVOKABLE MeshModelScriptInterface* getMesh(const int meshId);
	Q_INVOKABLE MeshModelScriptInterface* operator[](const QString& name);
	Q_INVOKABLE MeshModelScriptInterface* getMeshByName(const QString& name);
	Q_INVOKABLE MeshModelScriptInterface* current();
	Q_INVOKABLE int currentId();
	Q_INVOKABLE int setCurrent(const int meshId);
	MeshDocument* md;
};

class ShotScriptInterface;

class MeshModelScriptInterface : public QObject
{
	Q_OBJECT

public:
	MeshModelScriptInterface(MeshModel& meshModel,MeshDocumentScriptInterface* mdsi);
	
	Q_INVOKABLE int id() const;
	Q_INVOKABLE float bboxDiag() const;
	Q_INVOKABLE QVector<float> bboxMin() const;
	Q_INVOKABLE QVector<float> bboxMax() const;
Q_INVOKABLE inline float computeMinVQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(mm.cm).first;  }
Q_INVOKABLE inline float computeMaxVQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(mm.cm).second; }
Q_INVOKABLE inline float computeMinFQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(mm.cm).first;  }
Q_INVOKABLE inline float computeMaxFQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(mm.cm).second; }

	Q_INVOKABLE int vn() const;
	Q_INVOKABLE int fn() const;
	Q_INVOKABLE VCGVertexScriptInterface* v(const int ind);
	Q_INVOKABLE ShotScriptInterface* shot();

	MeshModel& mm;
}; 

Q_DECLARE_METATYPE(MeshDocumentScriptInterface*)
QScriptValue MeshDocumentScriptInterfaceToScriptValue(QScriptEngine* eng,MeshDocumentScriptInterface* const& in);

void MeshDocumentScriptInterfaceFromScriptValue(const QScriptValue& val,MeshDocumentScriptInterface*& out);

Q_DECLARE_METATYPE(MeshModelScriptInterface*)

QScriptValue MeshModelScriptInterfaceToScriptValue(QScriptEngine* eng,MeshModelScriptInterface* const& in);

void MeshModelScriptInterfaceFromScriptValue(const QScriptValue& val,MeshModelScriptInterface*& out);

Q_DECLARE_METATYPE(VCGVertexScriptInterface*)

QScriptValue VCGVertexScriptInterfaceToScriptValue(QScriptEngine* eng,VCGVertexScriptInterface* const& in);

void VCGVertexScriptInterfaceFromScriptValue(const QScriptValue& val,VCGVertexScriptInterface*& out);

Q_DECLARE_METATYPE(QVector<float>)

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
	int evalInt( const QString& nm );
	vcg::Point3f evalVec3(const QString& nm);
	QColor evalColor(const QString& nm);
	/*QString getExpType(const QString& nm);*/
	QString evalString(const QString& nm);
	int evalEnum( const QString& nm );
	MeshModel* evalMesh(const QString& nm);
};

QScriptValue EnvWrap_ctor(QScriptContext* c,QScriptEngine* e);

class ShotScriptInterface : public QObject
{
	Q_OBJECT
public:
	ShotScriptInterface(vcg::Shotf& st);
	~ShotScriptInterface() {};

private:
	vcg::Shotf& shot; 
};

Q_DECLARE_METATYPE(ShotScriptInterface*)
QScriptValue ShotScriptInterfaceToScriptValue(QScriptEngine* eng,ShotScriptInterface* const& in);

void ShotScriptInterfaceFromScriptValue(const QScriptValue& val,ShotScriptInterface*& out);
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
