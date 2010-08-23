#ifndef SCRIPTINTERFACE_H
#define SCRIPTINTERFACE_H
#include <QAction>
#include <QPoint>
#include <QtScript\QtScript>
#include "interfaces.h"
#include "filterparameter.h"


//class ScriptFilterInterface : public QObject
//{
//	Q_OBJECT
//public:
//	
//};
//
//class ScriptAdapterGenerator
//{
//private:
//	QString code;
//
//	QString parNames(const RichParameterSet& set) const
//	{
//		QString names;
//		int ii;
//		for(ii = 0;ii < (set.paramList.size() - 1);++ii)
//			names += set.paramList[ii]->name + ", ";
//		names += set.paramList[ii]->name;
//	}
//
//	const QString& funCodeGenerator(const QString&  filtername,const MeshLabInterface::FilterIDType filterid,const QString& funcname,const RichParameterSet& set)
//	{
//		code += "function " + funcname + "(" + parNames(set) + ")\n";
//		code += "{\n";
//		code += "\tvar tmpRichPar = new RichParameterSet();\n";
//		code += "\tvar action = new QAction(filtername);";
//		code += "\tvar sfi = new ScriptFilterInterface(this.meshfilter);";
//		code += "\tsfi.initParameterSet(&act,*(mod->mm()),tmpRichPar);";
//		code += "\tfor(var i = 0; i < tmpRichPar.size();i++)";
//
//
//			tmpRichPar.setValue("Displacement",FloatValue(perc));
//		rp.setValue("UpdateNormals",BoolValue(updtnrml));
//		applyFilter(&act,*mod,rp,callback);
//		return code;
//	}
//
//};

Q_DECLARE_METATYPE(RichParameterSet)
Q_DECLARE_METATYPE(RichParameterSet*)

QScriptValue IRichParameterSet_ctor(QScriptContext* c,QScriptEngine* e)
{
	return e->toScriptValue(RichParameterSet());
}

QScriptValue IRichParameterSet_prototype_setBool(QScriptContext* c,QScriptEngine* e)
{
	RichParameterSet* rset = qscriptvalue_cast<RichParameterSet*>(c->thisObject());
	QString varname = c->argument(0).toString();
	bool val = c->argument(1).toBool();
	rset->setValue(varname,BoolValue(val));
	return e->undefinedValue();
}

QScriptValue IRichParameterSet_prototype_setInt(QScriptContext* c,QScriptEngine* e)
{
	RichParameterSet* rset = qscriptvalue_cast<RichParameterSet*>(c->thisObject());
	QString varname = c->argument(0).toString();
	int val = c->argument(1).toInt32();
	rset->setValue(varname,IntValue(val));
	return e->undefinedValue();
}

//Q_DECLARE_METATYPE(QAction*)
//Q_DECLARE_METATYPE(QAction)
//

//class IQAction 
//{
//public:
//	IQAction(const QString& filtername,QObject* parent)
//	{
//
//	}
//};
//
//Q_DECLARE_TYPEINFO(QAction, Q_MOVABLE_TYPE);

class Person
{
public:
	QString nm;

	Person()
	{

	}

	Person(QString& name)
		:nm(name)
	{

	}
};

Q_DECLARE_METATYPE(Person)
Q_DECLARE_METATYPE(Person*)

QScriptValue Person_prototype_toString(QScriptContext* c,QScriptEngine* e)
{
	//QScriptValue object;
	//object.setPrototype(ctx->callee().property("prototype"));
	Person* per = qscriptvalue_cast<Person*>(c->thisObject());
	qDebug() << qPrintable(per->nm);
	
	//qDebug(qPrintable(per->nm));
	return e->undefinedValue();
}


QScriptValue myprint (QScriptContext* sc, QScriptEngine* se)
{
	// do what you want
	qDebug(qPrintable(sc->argument(0).toString()));
	return QScriptValue(se, 0);
}

QScriptValue Person_ctor(QScriptContext* c,QScriptEngine* e)
{
	QString x = c->argument(0).toString();
	Person* p = new Person(x);
	QScriptValue res = e->toScriptValue(*p);
	//res.setProperty("toString",e->newFunction(Person_prototype_toString));
	//res.setProperty("myPrint",e->newFunction(Person_prototype_toString));
	return res;
}

void registerTypes(QScriptEngine* eng)
{
	eng->globalObject().setProperty("print", eng->newFunction(myprint, 1));
	QScriptValue richset_ctor = eng->newFunction(IRichParameterSet_ctor);
	QScriptValue val = richset_ctor.property("prototype");
	QScriptValue boolfun = eng->newFunction(IRichParameterSet_prototype_setBool,2);
	richset_ctor.property("prototype").setProperty("setBool",boolfun);
	QScriptValue res = richset_ctor.property("prototype").property("setBool");
	res = richset_ctor.property("prototype").property("setInt");
	richset_ctor.property("prototype").setProperty("setInt",eng->newFunction(IRichParameterSet_prototype_setInt,2));
	eng->globalObject().setProperty("IRichParameterSet",richset_ctor);
	

	/*
	QScriptValue personProto = eng->newObject();
	personProto.setProperty("toString",eng->newFunction(Person_prototype_toString));
	personProto.setProperty("myPrint",eng->newFunction(Person_prototype_toString));
	QScriptValue per_ctr = eng->newFunction(Person_ctor, personProto);
	*/

	QScriptValue per_ctr = eng->newFunction(Person_ctor);
	//eng->setDefaultPrototype(qMetaTypeId<Person>(),per_ctr);
	
	bool ob = per_ctr.property("prototype").isObject();
	per_ctr.property("prototype").setProperty("toString",eng->newFunction(Person_prototype_toString));
	//per_ctr.property("prototype").property("toString").call();
	per_ctr.property("prototype").setProperty("myPrint",eng->newFunction(Person_prototype_toString));
	//per_ctr.property("prototype").property("myPrint").call();
	/*per_ctr.prototype().setProperty("toString",eng->newFunction(Person_prototype_toString));
	per_ctr.prototype().setProperty("myPrint",eng->newFunction(Person_prototype_toString)); 
	per_ctr.prototype().*/
	eng->globalObject().setProperty("Person",per_ctr);
}



#endif