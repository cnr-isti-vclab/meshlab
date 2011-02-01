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
#include "meshmodel.h"

class ScriptAdapterGenerator
{
private:
	QString parNames(const RichParameterSet& set) const;
public:
	QString funCodeGenerator(const QString&  filtername,const RichParameterSet& set);
};


QScriptValue PluginInterfaceInit(QScriptContext *context, QScriptEngine *engine, void * param);
QScriptValue PluginInterfaceApply(QScriptContext *context, QScriptEngine *engine, void * param);

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

	MeshDocument* md;
};

class MeshModelScriptInterface : public QObject
{
	Q_OBJECT

public:
	MeshModelScriptInterface(MeshModel& meshModel,MeshDocumentScriptInterface* mdsi);
	
	Q_INVOKABLE float bboxDiag() const;

	MeshModel& mm;
};

Q_DECLARE_METATYPE(MeshDocumentScriptInterface*)
QScriptValue MeshDocumentScriptInterfaceToScriptValue(QScriptEngine* eng,MeshDocumentScriptInterface* const& in);

void MeshDocumentScriptInterfaceFromScriptValue(const QScriptValue& val,MeshDocumentScriptInterface*& out);

Q_DECLARE_METATYPE(MeshModelScriptInterface*)

QScriptValue MeshModelScriptInterfaceToScriptValue(QScriptEngine* eng,MeshModelScriptInterface* const& in);

void MeshModelScriptInterfaceFromScriptValue(const QScriptValue& val,MeshModelScriptInterface*& out);


//QScriptValue MeshModelScriptInterface_ctor(QScriptContext* c,QScriptEngine* e)
//{
//	QString x = c->argument(0).toInt32();
//	return e->toScriptValue(MeshModelScriptInterface(x));
//}


#endif
