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
#include "meshmodel.h"
#include "filterparameter.h"
#include "xmlfilterinfo.h"
#include "vcg/space/point3.h"


class ScriptAdapterGenerator
{
private:
    QString parNames(const RichParameterSet& set) const;
    static inline QString optName() {return QString("optional");}
    static inline QString meshID() {return QString ("meshID");}
public:

    inline static const QString meshDocVarName() {return QString("meshDoc");}

    QString mergeOptParamsCodeGenerator() const;
    QString parNames(const QString&  filterName,MLXMLPluginInfo& xmlInfo) const;
    QString funCodeGenerator(const QString&  filtername,const RichParameterSet& set) const;
    QString funCodeGenerator(const QString&  filtername,MLXMLPluginInfo& xmlInfo) const;

};


//QScriptValue PluginInterfaceInit(QScriptContext *context, QScriptEngine *engine, void * param);
//QScriptValue PluginInterfaceApply(QScriptContext *context, QScriptEngine *engine, void * param);
QScriptValue PluginInterfaceApplyXML(QScriptContext *context, QScriptEngine *engine, void * param);

QScriptValue IRichParameterSet_prototype_setBool(QScriptContext* c,QScriptEngine* e);
QScriptValue IRichParameterSet_prototype_setInt(QScriptContext* c,QScriptEngine* e);
QScriptValue IRichParameterSet_prototype_setAbsPerc(QScriptContext* c,QScriptEngine* e);
QScriptValue IRichParameterSet_prototype_setFloat(QScriptContext* c,QScriptEngine* e);

QScriptValue IRichParameterSet_ctor(QScriptContext* c,QScriptEngine* e);

QScriptValue myprint (QScriptContext* sc, QScriptEngine* se);


typedef Point3m VCGPoint3SI;
typedef QVector<QVector<Scalarm> > Point3Vector;


class ScriptInterfaceUtilities
{
public:
    static QVector<Scalarm> vcgPoint2ToVector2(const Point2m& p);
    static Point2m vector2ToVcgPoint2(const QVector<Scalarm>& v);
    static vcg::Point2i vector2ToVcgPoint2i(const QVector<Scalarm>& v);
    static QVector<Scalarm> vcgPoint3ToVector3(const Point3m &p);
    static Point3m vector3ToVcgPoint3(const QVector<Scalarm>& v);
    static QVector<Scalarm> vcgPoint4ToVector4(const Point4m& p);
    static Point4m vector4ToVcgPoint4(const QVector<Scalarm>& v);
    static QVector<Scalarm> vcgMatrix44ToVector16(const Matrix44m& m);
    static Matrix44m vector16ToVcgMatrix44(const QVector<Scalarm>& v);
};

//class VCGPoint3fSI : public QObject
//{
//	Q_OBJECT
//public:
//
//	VCGPoint3fSI& operator =(const VCGPoint3fSI& b);
//	VCGPoint3fSI();
//	VCGPoint3fSI(const float x,const float y,const float z);
//	Q_INVOKABLE void add(const VCGPoint3fSI& p);
//	Q_INVOKABLE void mult(const float s);
//	//Q_INVOKABLE VCGPoint3fSI& set(const float x,const float y,const float z);
//	vcg::Point3f pp;
//};



class VCGVertexSI : public QObject
{
    Q_OBJECT
public:
    VCGVertexSI(CMeshO::VertexType& v);

    //Q_INVOKABLE vcg::Point3f* p();
    Q_INVOKABLE QVector<Scalarm> getP();
    Q_INVOKABLE VCGPoint3SI getPoint();
    Q_INVOKABLE void setPC(const Scalarm x,const Scalarm y,const Scalarm z);
    Q_INVOKABLE void setP(const QVector<Scalarm>& p);
    Q_INVOKABLE void setPoint(const VCGPoint3SI& p);
    Q_INVOKABLE QVector<Scalarm> getN();
    Q_INVOKABLE VCGPoint3SI getNormal();
    Q_INVOKABLE void setNormal(const VCGPoint3SI& p);
    Q_INVOKABLE void setN(const Scalarm x,const Scalarm y,const Scalarm z);

    CMeshO::VertexType& vv;
};

class MeshModelSI;

class MeshDocumentSI : public QObject
{
    Q_OBJECT

public:
    MeshDocumentSI(MeshDocument* doc);
    ~MeshDocumentSI(){}

    Q_INVOKABLE MeshModelSI* getMesh(const int meshId);
    Q_INVOKABLE MeshModelSI* operator[](const QString& name);
    Q_INVOKABLE MeshModelSI* getMeshByName(const QString& name);
    Q_INVOKABLE MeshModelSI* current();
    Q_INVOKABLE int currentId();
    Q_INVOKABLE int setCurrent(const int meshId);
    MeshDocument* md;
};

class ShotSI;

class MeshModelSI : public QObject
{
    Q_OBJECT

public:
    MeshModelSI(MeshModel& meshModel,MeshDocumentSI* mdsi);

    Q_INVOKABLE int id() const;
    Q_INVOKABLE Scalarm bboxDiag() const;
    Q_INVOKABLE QVector<Scalarm> bboxMin() const;
    Q_INVOKABLE QVector<Scalarm> bboxMax() const;
Q_INVOKABLE inline Scalarm computeMinVQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(mm.cm).first;  }
Q_INVOKABLE inline Scalarm computeMaxVQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(mm.cm).second; }
Q_INVOKABLE inline Scalarm computeMinFQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(mm.cm).first;  }
Q_INVOKABLE inline Scalarm computeMaxFQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(mm.cm).second; }

    Q_INVOKABLE QVector<VCGVertexSI*> vert();
    Q_INVOKABLE Point3Vector getVertPosArray();
    Q_INVOKABLE Point3Vector getVertNormArray();
    Q_INVOKABLE void setVertPosArray(const Point3Vector& pa);
    Q_INVOKABLE void setVertNormArray(const Point3Vector& na);
    //Q_INVOKABLE void setV(const QVector<VCGVertexSI*>& v);
    Q_INVOKABLE QString label() const { return mm.label(); }
    Q_INVOKABLE int vn() const;
    Q_INVOKABLE int fn() const;
    Q_INVOKABLE VCGVertexSI* v(const int ind);
    Q_INVOKABLE ShotSI* shot();

    MeshModel& mm;
};


//Q_DECLARE_METATYPE(Scalarm)
//QScriptValue ScalarmToScriptValue(QScriptEngine* eng,Scalarm* const& in);
//
//void ScalarmFromScriptValue(const QScriptValue& val,Scalarm*& out);

Q_DECLARE_METATYPE(MeshDocumentSI*)
QScriptValue MeshDocumentScriptInterfaceToScriptValue(QScriptEngine* eng,MeshDocumentSI* const& in);

void MeshDocumentScriptInterfaceFromScriptValue(const QScriptValue& val,MeshDocumentSI*& out);

Q_DECLARE_METATYPE(MeshModelSI*)

QScriptValue MeshModelScriptInterfaceToScriptValue(QScriptEngine* eng,MeshModelSI* const& in);

void MeshModelScriptInterfaceFromScriptValue(const QScriptValue& val,MeshModelSI*& out);

Q_DECLARE_METATYPE(VCGVertexSI*)

QScriptValue VCGVertexScriptInterfaceToScriptValue(QScriptEngine* eng,VCGVertexSI* const& in);

void VCGVertexScriptInterfaceFromScriptValue(const QScriptValue& val,VCGVertexSI*& out);


Q_DECLARE_METATYPE(QVector<Scalarm>)
Q_DECLARE_METATYPE(QVector<Scalarm>*)
Q_DECLARE_METATYPE(Point3Vector)
Q_DECLARE_METATYPE(QVector<VCGVertexSI*>)

Q_DECLARE_METATYPE(VCGPoint3SI*)
Q_DECLARE_METATYPE(VCGPoint3SI)

QScriptValue VCGPoint3ScriptInterface_ctor(QScriptContext *context, QScriptEngine *engine);

//QScriptValue VCGPoint3fScriptInterfaceToScriptValue(QScriptEngine* eng,VCGPoint3fSI* const& in);
//void VCGPoint3fScriptInterfaceFromScriptValue(const QScriptValue& val,VCGPoint3fSI*& out);

class PluginManager;

class Env :public QScriptEngine
{
    Q_OBJECT

    QString out;
    QList<QObject*> _tobedeleted;
public:
    Env();
    ~Env();
    Q_INVOKABLE void insertExpressionBinding(const QString& nm,const QString& exp);

    QString output();
    void appendOutput(const QString& output);
    QScriptValue loadMLScriptEnv(MeshDocument& md,PluginManager& pm);

	//global map contains purified name (i.e. Env::convertToAMLScriptValidName(paramname) fun is called) of the global parameters and their values
	QScriptValue loadMLScriptEnv(MeshDocument& md, PluginManager& pm, const QMap<QString, QString>& global);
	////global map contains purified name (i.e. Env::convertToAMLScriptValidName(paramname) fun is called) of the global parameters and their values
	//QScriptValue loadMLScriptEnv(MeshDocument& md, PluginManager& pm, const QString & xmlpluginnamespace, const QString & pluginname, const QString & filtername, const QMap<QString,QString>& global);

private:
	void insertParamsExpressionBinding(const QString & xmlpluginnamespace, const QString & pluginname, const QString & filtername, const QMap<QString, QString>& parvalmap);

	static QString convertToAMLScriptValidName(const QString& name);
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
    EnvWrap():env(NULL){}
    EnvWrap(Env& envir);
    bool evalBool(const QString& nm);
    double evalDouble(const QString& nm);
    float evalFloat( const QString& nm );
    MESHLAB_SCALAR evalReal(const QString& nm);
    int evalInt( const QString& nm );
    vcg::Point3f evalVec3(const QString& nm);
    vcg::Point3<MESHLAB_SCALAR> evalVec3Real(const QString& nm);
    QColor evalColor(const QString& nm);
    /*QString getExpType(const QString& nm);*/
    QString evalString(const QString& nm);
    int evalEnum( const QString& nm );
    MeshModel* evalMesh(const QString& nm);
    Shotm evalShot(const QString& nm);
};

QScriptValue EnvWrap_ctor(QScriptContext* c,QScriptEngine* e);


class ShotSI : public QObject
{
    Q_OBJECT
public:
    ShotSI();
    ShotSI(const Shotm &st);
    ~ShotSI() {}

    Q_INVOKABLE ShotSI* itSelf();

    Shotm shot;
};

Q_DECLARE_METATYPE(ShotSI*)
QScriptValue ShotScriptInterfaceToScriptValue(QScriptEngine* eng,ShotSI* const& in);
void ShotScriptInterfaceFromScriptValue(const QScriptValue& val,ShotSI*& out);
QScriptValue ShotSI_ctor(QScriptContext* c,QScriptEngine* e);
QScriptValue ShotSI_defctor(QScriptContext* c,QScriptEngine* e);




    inline QScriptValue VCGPoint3SI_addV3(QScriptContext * c,QScriptEngine *e )
    {
        return e->toScriptValue(*qscriptvalue_cast<VCGPoint3SI*>(c->argument(0)) + *qscriptvalue_cast<VCGPoint3SI*>(c->argument(1)));
    }

    inline QScriptValue VCGPoint3SI_multV3S( QScriptContext * c,QScriptEngine *e )
    {
        return e->toScriptValue(*qscriptvalue_cast<VCGPoint3SI*>(c->argument(0)) * (Scalarm) c->argument(1).toNumber());
    }


//QScriptValue VCGPoint3SI_multV3S(QScriptContext * c,QScriptEngine *e );
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
