#include "scriptinterface.h"
#include "pluginmanager.h"
#include "interfaces.h"
#include "filterparameter.h"
#include "meshmodel.h"
#include "mlexception.h"

QString ScriptAdapterGenerator::parNames(const RichParameterSet& set) const
{
	QString names;
	int ii;
	for(ii = 0;ii < (set.paramList.size() - 1);++ii)
		names += set.paramList[ii]->name + ", ";
	if (set.paramList.size() != 0)
		names += set.paramList[ii]->name;
	return names;
}

QString ScriptAdapterGenerator::parNames( const QString& filterName,const XMLFilterInfo& xmlInfo ) const
{
	QString names;
	//the order is important !!! 
	XMLFilterInfo::XMLMapList params = xmlInfo.filterParametersExtendedInfo(filterName);
	int ii;
	bool optional = false;
	for(ii = 0;ii < params.size();++ii)
	{
		bool isImp = (params[ii][MLXMLElNames::paramIsImportant] == "true");
		if (names.isEmpty() && isImp)
			names += /*params[ii][MLXMLElNames::paramType] + "_" +*/ params[ii][MLXMLElNames::paramName];
		else
			if (isImp)
				names += ", " + /*params[ii][MLXMLElNames::paramType] + "_" + */params[ii][MLXMLElNames::paramName];
			else
				optional = true;
	}
	if (optional && !(names.isEmpty()))
		names += ", " + optName();
	if (optional && names.isEmpty())
		names += optName();
	return names;
}


QString ScriptAdapterGenerator::funCodeGenerator(const QString&  filtername,const RichParameterSet& set) const
{
	QString code;
	code += "function (" + parNames(set) + ")\n";
	code += "{\n";
	code += "\tvar tmpRichPar = new IRichParameterSet();\n";
	code += "\tif (!_initParameterSet(\""+ filtername + "\",tmpRichPar)) return false;\n";
	for(int ii = 0; ii < set.paramList.size();++ii)
		code += "\ttmpRichPar.set" + set.paramList[ii]->val->typeName() + "(\"" + set.paramList[ii]->name + "\",arguments[" + QString::number(ii) + "]);\n";
	code += "\treturn _applyFilter(\"" + filtername + "\",tmpRichPar);\n";
	code += "};\n";
	return code;
}

QString ScriptAdapterGenerator::funCodeGenerator( const QString& filterName,const XMLFilterInfo& xmlInfo ) const
{
	QString code;
	QString names = parNames(filterName,xmlInfo);
	QString ariet = xmlInfo.filterAttribute(filterName,MLXMLElNames::filterAriety);

	bool isSingle = (ariet == MLXMLElNames::singleMeshAriety);
	QString mid("meshID");
	if ((names.isEmpty()) && isSingle)
		names = mid;
	else
		if (isSingle)
			names = mid + ", " + names;

	code += "function (" + names + ")\n";
	code += "{\n";
	if (names.indexOf(optName()) != -1)
		code += "\t" + optName() + " = " + optName() + " || {};\n";
	XMLFilterInfo::XMLMapList mplist = xmlInfo.filterParametersExtendedInfo(filterName);
	code += "\tvar environ = new Env;\n";
	
	//if is singleMeshAriety i have to jump the first argument because is the meshID
	int arg = (int) isSingle;
	for(int ii = 0; ii < mplist.size();++ii)
	{
		if (mplist[ii][MLXMLElNames::paramIsImportant] == "true")
		{
			code += "\tenviron.insertExpressionBinding(\"" + mplist[ii][MLXMLElNames::paramName] + "\",arguments[" + QString::number(arg) + "]);\n";
			++arg;
		}
		else
		{
			code += "\tvar " + mplist[ii][MLXMLElNames::paramName] + " = " + optName() + "." + mplist[ii][MLXMLElNames::paramType] + "_" + mplist[ii][MLXMLElNames::paramName] + " || " + mplist[ii][MLXMLElNames::paramDefExpr] + ";\n";
			code += "\tenviron.insertExpressionBinding(\"" + mplist[ii][MLXMLElNames::paramName] + "\", " + mplist[ii][MLXMLElNames::paramName] + ");\n";
		}
	}
	code += "\tvar environWrap = new EnvWrap(environ);\n";
	if (isSingle)
	{
		code += "\tvar oldInd=" + meshDocVarName() + ".setCurrent(" + mid + ");\n";
		code += "\tif (oldInd == -1) return false;\n"; 
	}
	code += "\tvar result = _applyFilter(\"" + filterName + "\",environWrap);\n";
	if (isSingle)
		code += "\t" +meshDocVarName() + ".setCurrent(oldInd);\n";
	code += "\treturn result;\n";
	code += "};\n";
	return code;
}

const QStringList ScriptAdapterGenerator::javaScriptLibraryFiles()
{
	QStringList res;
	res.push_back(":/script_system/space_math.js");
	return res;
}

Q_DECLARE_METATYPE(MeshDocument*)

static bool TestCallback(const int , const char* )
{
	return true;
}

QScriptValue PluginInterfaceInit(QScriptContext *context, QScriptEngine *engine, void * param)
{
	QString filterName = context->argument(0).toString();
	PluginManager * pm = reinterpret_cast<PluginManager *>(param);
	QMap<QString, MeshFilterInterface*>::iterator it = pm->stringFilterMap.find(filterName);
	if (it == pm->stringFilterMap.end())
	{
		return false;
	}

	MeshDocumentScriptInterface* md = qscriptvalue_cast<MeshDocumentScriptInterface*>(engine->globalObject().property(ScriptAdapterGenerator::meshDocVarName()));
	RichParameterSet* rps = qscriptvalue_cast<RichParameterSet*>(context->argument(1));

	MeshFilterInterface * mi = it.value();
	QAction act(filterName, NULL);
	mi->initParameterSet(&act, (md->current()->mm), *rps);

	return true;
}

QScriptValue PluginInterfaceApply(QScriptContext *context, QScriptEngine *engine, void * param)
{
	QString filterName = context->argument(0).toString();
	PluginManager * pm = reinterpret_cast<PluginManager *>(param);
	QMap<QString, MeshFilterInterface*>::iterator it = pm->stringFilterMap.find(filterName);
	if (it == pm->stringFilterMap.end())
	{
		return false;
	}

	MeshDocumentScriptInterface* md = qscriptvalue_cast<MeshDocumentScriptInterface*>(engine->globalObject().property(ScriptAdapterGenerator::meshDocVarName()));
	RichParameterSet* rps = qscriptvalue_cast<RichParameterSet*>(context->argument(1));

	MeshFilterInterface * mi = it.value();
	QAction act(filterName, NULL);
	const bool res = mi->applyFilter(&act, *(md->md), *rps, TestCallback);

	return res;
}

QScriptValue PluginInterfaceApplyXML(QScriptContext *context, QScriptEngine *engine, void * param)
{
	QString filterName = context->argument(0).toString();
	PluginManager * pm = reinterpret_cast<PluginManager *>(param);
	QMap<QString, MeshLabXMLFilterContainer>::iterator it = pm->stringXMLFilterMap.find(filterName);
	if (it == pm->stringXMLFilterMap.end())
		return false;

	MeshDocumentScriptInterface* md = qscriptvalue_cast<MeshDocumentScriptInterface*>(engine->globalObject().property(ScriptAdapterGenerator::meshDocVarName()));
	EnvWrap* envWrap = qscriptvalue_cast<EnvWrap*>(context->argument(1));

	MeshLabFilterInterface * mi = it->filterInterface;
	const bool res = mi->applyFilter(filterName, *(md->md), *envWrap, TestCallback);
	return res;
}

Q_DECLARE_METATYPE(RichParameterSet)
Q_DECLARE_METATYPE(RichParameterSet*)

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


QScriptValue IRichParameterSet_prototype_setAbsPerc(QScriptContext* c,QScriptEngine* e)
{
	RichParameterSet* rset = qscriptvalue_cast<RichParameterSet*>(c->thisObject());
	QString varname = c->argument(0).toString();
	float val = (float) c->argument(1).toNumber();
	rset->setValue(varname,AbsPercValue(val));
	return e->undefinedValue();
}

QScriptValue IRichParameterSet_prototype_setFloat( QScriptContext* c,QScriptEngine* e )
{
	RichParameterSet* rset = qscriptvalue_cast<RichParameterSet*>(c->thisObject());
	QString varname = c->argument(0).toString();
	float val = (float) c->argument(1).toNumber();
	rset->setValue(varname,FloatValue(val));
	return e->undefinedValue();
}

QScriptValue IRichParameterSet_ctor(QScriptContext* /*c*/,QScriptEngine* e)
{
	RichParameterSet* p = new RichParameterSet();
	QScriptValue res = e->toScriptValue(*p);
	//res.setProperty("setBool",e->newFunction(IRichParameterSet_prototype_setBool,2));
	//res.setProperty("setInt",e->newFunction(IRichParameterSet_prototype_setInt,2));
	return res;
}


QScriptValue myprint (QScriptContext* sc, QScriptEngine* se)
{
	// do what you want
  qDebug("%s",qPrintable(sc->argument(0).toString()));
	return QScriptValue(se, 0);
}

void registerTypes(QScriptEngine* eng)
{
	eng->globalObject().setProperty("print", eng->newFunction(myprint, 1));

	QScriptValue richset_ctor = eng->newFunction(IRichParameterSet_ctor);
	eng->setDefaultPrototype(qMetaTypeId<RichParameterSet>(), richset_ctor.property("prototype"));
	QScriptValue boolfun = eng->newFunction(IRichParameterSet_prototype_setBool,2);
	richset_ctor.property("prototype").setProperty("setBool",boolfun);
	QScriptValue intfun = eng->newFunction(IRichParameterSet_prototype_setInt,2);
	richset_ctor.property("prototype").setProperty("setInt",intfun);
	QScriptValue abspercfun = eng->newFunction(IRichParameterSet_prototype_setAbsPerc,2);
	richset_ctor.property("prototype").setProperty("setAbsPerc",abspercfun);
	QScriptValue floatfun = eng->newFunction(IRichParameterSet_prototype_setFloat,2);
	richset_ctor.property("prototype").setProperty("setFloat",floatfun);
	eng->globalObject().setProperty("IRichParameterSet",richset_ctor);

	QScriptValue envwrap_ctor = eng->newFunction(EnvWrap_ctor);
	//eng->setDefaultPrototype(qMetaTypeId<EnvWrap>(), envwrap_ctor.property("prototype"));
	eng->globalObject().setProperty("EnvWrap",envwrap_ctor);

	QScriptValue env_ctor = eng->newFunction(Env_ctor);
	QScriptValue metaObject = eng->newQMetaObject(&Env::staticMetaObject, env_ctor);
	eng->globalObject().setProperty("Env", metaObject);
}

MeshModelScriptInterface::MeshModelScriptInterface(MeshModel& meshModel,MeshDocumentScriptInterface* parent)
:QObject(parent),mm(meshModel)
{

}

Q_INVOKABLE float MeshModelScriptInterface::bboxDiag() const
{
	return mm.cm.bbox.Diag();
}

Q_INVOKABLE vcg::Point3f MeshModelScriptInterface::bboxMin() const
{
	return mm.cm.bbox.min;
}

Q_INVOKABLE vcg::Point3f MeshModelScriptInterface::bboxMax() const
{
	return mm.cm.bbox.max;
}

QScriptValue MeshModelScriptInterfaceToScriptValue(QScriptEngine* eng,MeshModelScriptInterface* const& in)
{
	return eng->newQObject(in);
}

void MeshModelScriptInterfaceFromScriptValue(const QScriptValue& val,MeshModelScriptInterface*& out)
{
	out = qobject_cast<MeshModelScriptInterface*>(val.toQObject());
}

QScriptValue MeshDocumentScriptInterfaceToScriptValue( QScriptEngine* eng,MeshDocumentScriptInterface* const& in )
{
	return eng->newQObject(in);
}

void MeshDocumentScriptInterfaceFromScriptValue( const QScriptValue& val,MeshDocumentScriptInterface*& out )
{
	out = qobject_cast<MeshDocumentScriptInterface*>(val.toQObject());
}

QScriptValue Point3fToScriptValue(QScriptEngine* eng,const vcg::Point3f& in)
{
	QScriptValue arrRes = eng->newArray(3);
	for(unsigned int ii = 0;ii < 3;++ii)
		arrRes.setProperty(ii,in[ii]);
	return arrRes;
}

void Point3fFromScriptValue(const QScriptValue& val,vcg::Point3f& out)
{
	for(unsigned int ii = 0;ii < 3;++ii)
		out[ii] = val.property(ii).toNumber();
}

QScriptValue EnvWrap_ctor( QScriptContext* c,QScriptEngine* e )
{
	Env* env = qscriptvalue_cast<Env*>(c->argument(0));
	EnvWrap* p = new EnvWrap(*env);
	QScriptValue res = e->toScriptValue(*p);
	return res;
}

MeshDocumentScriptInterface::MeshDocumentScriptInterface( MeshDocument* doc )
:QObject(doc),md(doc)
{
}

Q_INVOKABLE MeshModelScriptInterface* MeshDocumentScriptInterface::getMesh( int meshId )
{
	MeshModel* model = md->getMesh(meshId);
	if (model != NULL)
		return new MeshModelScriptInterface(*model,this);
	else
		return NULL;
}

Q_INVOKABLE MeshModelScriptInterface* MeshDocumentScriptInterface::current()
{
	MeshModel* model = md->mm();
	if (model != NULL)
		return new MeshModelScriptInterface(*model,this);
	else
		return NULL;
}

Q_INVOKABLE int MeshDocumentScriptInterface::setCurrent(const int meshId)
{
	int id = md->mm()->id();
	if (md->getMesh(meshId) != NULL)
	{
		md->setCurrentMesh(meshId);
		return id;
	}
	else
		return -1;
}


EnvWrap::EnvWrap(Env& envir)
:env(&envir)
{
}

QScriptValue EnvWrap::getExp( const QString& nm )
{
	if (!constStatement(nm))
		throw NotConstException(nm);
	QScriptValue result = env->evaluate(nm);
	if (result.isError())
		throw ValueNotFoundException(nm);
	return result;
}

bool EnvWrap::getBool( const QString& nm )
{
	QScriptValue result = getExp(nm);
	if (result.isBool())
		return result.toBool();
	else
		throw ExpressionHasNotThisTypeException("Bool",nm);
	return false;
}

float EnvWrap::getFloat( const QString& nm )
{
	QScriptValue result = getExp(nm);
	if (result.isNumber())
		return result.toNumber();
	else
		throw ExpressionHasNotThisTypeException("Float",nm);
	return float();
}

vcg::Point3f EnvWrap::getVec3( const QString& nm )
{
	QScriptValue result = getExp(nm);
	QVariant resVar = result.toVariant();
	QVariantList resList = resVar.toList();
	if (resList.size() == 3)
		return vcg::Point3f(resList[0].toDouble(),resList[1].toDouble(),resList[2].toDouble());
	else
		throw ExpressionHasNotThisTypeException("Vec3",nm);
	return vcg::Point3f();
}

bool EnvWrap::constStatement( const QString& statement ) const
{
	QRegExp exp("\\S+\\s*=\\s*S++;");
	int ii = statement.indexOf(exp);
	return (ii == -1);
}
//
//QString EnvWrap::getExpType( const QString& exp )
//{
//	QScriptValue result = getExp(exp);
//	QVariant resVariant = result.toVariant();
//	switch (resVariant.type())
//	{
//		case(QVariant::Bool):
//		{
//			return MLXMLElNames::boolType;
//			break;
//		}
//		
//		case(QVariant::Int):
//		{
//				return MLXMLElNames::intType;
//				break;
//		}
//
//		case(QVariant::Double):
//		{
//			return MLXMLElNames::realType;
//			break;
//		}
//
//		case(QVariant::List)
//		{
//			QVariantList resList = resVariant.toList();
//			switch(resList.size())
//			{
//				case(0):
//				{
//					break;
//				}
//
//				case()
//			}
//		}
//	}
//}

QString EnvWrap::getString( const QString& nm )
{
	QScriptValue result = getExp(nm);
	return result.toString();
}

Q_DECLARE_METATYPE(EnvWrap)
Q_DECLARE_METATYPE(EnvWrap*)

QScriptValue Env_ctor( QScriptContext *context,QScriptEngine *engine )
{
	Env * env = new Env();
	return engine->newQObject(env, QScriptEngine::ScriptOwnership);
}

Env::Env()
{
	qScriptRegisterMetaType(this,MeshModelScriptInterfaceToScriptValue,MeshModelScriptInterfaceFromScriptValue);
	qScriptRegisterMetaType(this,Point3fToScriptValue,Point3fFromScriptValue);
}

void Env::insertExpressionBinding( const QString& nm,const QString& exp )
{
	QString decl("var " + nm + " = " + exp + ";");
	QScriptValue res = evaluate(decl);
	if	(res.isError())
		throw JavaScriptException(res.toString());
}
