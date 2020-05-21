#include "scriptinterface.h"
#include "pluginmanager.h"
#include "interfaces.h"
#include "filterparameter.h"
#include "meshmodel.h"
#include "mlexception.h"
#include "pluginmanager.h"
#include "scriptsyntax.h"

QString ScriptAdapterGenerator::parNames(const RichParameterSet& set) const
{
	QString names;
	int ii;
	for (ii = 0; ii < (set.paramList.size() - 1); ++ii)
		names += set.paramList[ii]->name + ", ";
	if (set.paramList.size() != 0)
		names += set.paramList[ii]->name;
	return names;
}

QString ScriptAdapterGenerator::funCodeGenerator(const QString&  filtername, const RichParameterSet& set) const
{
	QString code;
	code += "function (" + parNames(set) + ")\n";
	code += "{\n";
	code += "\tvar tmpRichPar = new IRichParameterSet();\n";
	code += "\tif (!_initParameterSet(\"" + filtername + "\",tmpRichPar)) return false;\n";
	for (int ii = 0; ii < set.paramList.size(); ++ii)
		code += "\ttmpRichPar.set" + set.paramList[ii]->val->typeName() + "(\"" + set.paramList[ii]->name + "\",arguments[" + QString::number(ii) + "]);\n";
	code += "\treturn _applyFilter(\"" + filtername + "\",tmpRichPar);\n";
	code += "};\n";
	return code;
}

QString ScriptAdapterGenerator::mergeOptParamsCodeGenerator() const
{
	QString code;
	code += "function __mergeOptions(argOptions, defaultOptions)\n{\n";
	code += "\tvar ret = { };\n";
	code += "\targOptions = argOptions || { };\n";
	code += "\tfor (var p in defaultOptions)\n";
	code += "\t\tret[p] = argOptions.hasOwnProperty(p) ? argOptions[p] : defaultOptions[p];\n";
	code += "\treturn ret;\n}\n";
	return code;
}


Q_DECLARE_METATYPE(MeshDocument*)

static bool TestCallback(const int, const char*)
{
	return true;
}

Q_DECLARE_METATYPE(RichParameterSet)
Q_DECLARE_METATYPE(RichParameterSet*)

QScriptValue IRichParameterSet_prototype_setBool(QScriptContext* c, QScriptEngine* e)
{
	RichParameterSet* rset = qscriptvalue_cast<RichParameterSet*>(c->thisObject());
	QString varname = c->argument(0).toString();
	bool val = c->argument(1).toBool();
	rset->setValue(varname, BoolValue(val));
	return e->undefinedValue();
}

QScriptValue IRichParameterSet_prototype_setInt(QScriptContext* c, QScriptEngine* e)
{
	RichParameterSet* rset = qscriptvalue_cast<RichParameterSet*>(c->thisObject());
	QString varname = c->argument(0).toString();
	int val = c->argument(1).toInt32();
	rset->setValue(varname, IntValue(val));
	return e->undefinedValue();
}


QScriptValue IRichParameterSet_prototype_setAbsPerc(QScriptContext* c, QScriptEngine* e)
{
	RichParameterSet* rset = qscriptvalue_cast<RichParameterSet*>(c->thisObject());
	QString varname = c->argument(0).toString();
	float val = (float)c->argument(1).toNumber();
	rset->setValue(varname, AbsPercValue(val));
	return e->undefinedValue();
}

QScriptValue IRichParameterSet_prototype_setFloat(QScriptContext* c, QScriptEngine* e)
{
	RichParameterSet* rset = qscriptvalue_cast<RichParameterSet*>(c->thisObject());
	QString varname = c->argument(0).toString();
	float val = (float)c->argument(1).toNumber();
	rset->setValue(varname, FloatValue(val));
	return e->undefinedValue();
}

QScriptValue IRichParameterSet_ctor(QScriptContext* /*c*/, QScriptEngine* e)
{
	RichParameterSet* p = new RichParameterSet();
	QScriptValue res = e->toScriptValue(*p);
	//res.setProperty("setBool",e->newFunction(IRichParameterSet_prototype_setBool,2));
	//res.setProperty("setInt",e->newFunction(IRichParameterSet_prototype_setInt,2));
	return res;
}


QScriptValue myprint(QScriptContext* sc, QScriptEngine* se)
{
	QString st = sc->argument(0).toString();
	Env* myenv = qobject_cast<Env*>(se);
	if (myenv)
		myenv->appendOutput(st);
	return QScriptValue(se, 0);
}

MeshDocumentSI::MeshDocumentSI(MeshDocument* doc)
	:QObject(), md(doc)
{
}

Q_INVOKABLE MeshModelSI* MeshDocumentSI::getMesh(int meshId)
{
	MeshModel* model = md->getMesh(meshId);
	if (model != NULL)
		return new MeshModelSI(*model, this);
	else
		return NULL;
}

Q_INVOKABLE MeshModelSI* MeshDocumentSI::current()
{
	MeshModel* model = md->mm();
	if (model != NULL)
		return new MeshModelSI(*model, this);
	else
		return NULL;
}

Q_INVOKABLE int MeshDocumentSI::currentId()
{
	MeshModel* model = md->mm();
	if (model != NULL)
		return model->id();
	else
		return -1;
}

Q_INVOKABLE int MeshDocumentSI::setCurrent(const int meshId)
{
	int id = -1;
	if (md->mm() != NULL)
		id = md->mm()->id();
	if (md->getMesh(meshId) != NULL)
	{
		md->setCurrentMesh(meshId);
		return id;
	}
	else
		return -1;
}

Q_INVOKABLE MeshModelSI* MeshDocumentSI::operator[](const QString& name)
{
	MeshModel* mym = md->getMesh(name);
	if (mym != NULL)
		return new MeshModelSI(*mym, this);
	else
		return NULL;
}

Q_INVOKABLE MeshModelSI* MeshDocumentSI::getMeshByName(const QString& name)
{
	return (*this)[name];
}


MeshModelSI::MeshModelSI(MeshModel& meshModel, MeshDocumentSI* parent)
	:QObject(parent), mm(meshModel)
{

}

Q_INVOKABLE Scalarm MeshModelSI::bboxDiag() const
{
	return mm.cm.bbox.Diag();
}

Q_INVOKABLE QVector<Scalarm> MeshModelSI::bboxMin() const
{
	return ScriptInterfaceUtilities::vcgPoint3ToVector3(mm.cm.bbox.min);
}

Q_INVOKABLE QVector<Scalarm> MeshModelSI::bboxMax() const
{
	return ScriptInterfaceUtilities::vcgPoint3ToVector3(mm.cm.bbox.max);
}

Q_INVOKABLE int MeshModelSI::id() const
{
	return mm.id();
}

VCGVertexSI::VCGVertexSI(CMeshO::VertexType& v)
	:QObject(), vv(v)
{
}

Q_INVOKABLE VCGVertexSI* MeshModelSI::v(const int ind)
{
	unsigned int ii(ind);
	if (ii < mm.cm.vert.size())
		return new VCGVertexSI(mm.cm.vert[ii]);
	else
		return NULL;
}

Q_INVOKABLE ShotSI* MeshModelSI::shot()
{
	return new ShotSI(mm.cm.shot);
}

Q_INVOKABLE int MeshModelSI::vn() const
{
	return mm.cm.vn;
}

Q_INVOKABLE int MeshModelSI::fn() const
{
	return mm.cm.fn;
}

Q_INVOKABLE QVector<VCGVertexSI*> MeshModelSI::vert()
{
	QVector<VCGVertexSI*> v;
	for (int ii = 0; ii < mm.cm.vn; ++ii)
		v.push_back(new VCGVertexSI(mm.cm.vert[ii]));
	return v;
}

Q_INVOKABLE Point3Vector MeshModelSI::getVertPosArray()
{
	Point3Vector pv;
	for (int ii = 0; ii < mm.cm.vn; ++ii)
	{
		QVector<Scalarm> p;
		p << mm.cm.vert[ii].P().X() << mm.cm.vert[ii].P().Y() << mm.cm.vert[ii].P().Z();
		pv << p;
	}
	return pv;
}

Q_INVOKABLE void MeshModelSI::setVertPosArray(const Point3Vector& pa)
{
	for (int ii = 0; ii < mm.cm.vn; ++ii)
		mm.cm.vert[ii].P() = Point3m(pa[ii][0], pa[ii][1], pa[ii][2]);
}

Q_INVOKABLE void MeshModelSI::setVertNormArray(const Point3Vector& na)
{
	for (int ii = 0; ii < mm.cm.vn; ++ii)
		mm.cm.vert[ii].N() = Point3m(na[ii][0], na[ii][1], na[ii][2]);
}

Q_INVOKABLE Point3Vector MeshModelSI::getVertNormArray()
{
	Point3Vector pv;
	for (int ii = 0; ii < mm.cm.vn; ++ii)
	{
		QVector<Scalarm> p;
		p << mm.cm.vert[ii].N().X() << mm.cm.vert[ii].N().Y() << mm.cm.vert[ii].N().Z();
		pv << p;
	}
	return pv;
}

//Q_INVOKABLE void MeshModelSI::setV( const QVector<VCGVertexSI*>& v )
//{
//	for(unsigned int ii = 0; ii < mm.cm.vn;++ii)
//		mm.cm.vert[ii] = v[ii]->;
//}


Q_INVOKABLE QVector<Scalarm> VCGVertexSI::getP()
{
	return ScriptInterfaceUtilities::vcgPoint3ToVector3(vv.P());
}

Q_INVOKABLE void VCGVertexSI::setPC(const Scalarm x, const Scalarm y, const Scalarm z)
{
	vv.P() = Point3m(x, y, z);
}

Q_INVOKABLE void VCGVertexSI::setP(const QVector<Scalarm>& p)
{
	for (int ii = 0; ii < 3; ++ii)
		vv.P()[ii] = p[ii];
}

Q_INVOKABLE QVector<Scalarm> VCGVertexSI::getN()
{
	return ScriptInterfaceUtilities::vcgPoint3ToVector3(vv.N());
}

Q_INVOKABLE void VCGVertexSI::setN(const Scalarm x, const Scalarm y, const Scalarm z)
{
	vv.N() = Point3m(x, y, z);
}

Q_INVOKABLE VCGPoint3SI VCGVertexSI::getPoint()
{
	return vv.P();
}

Q_INVOKABLE void VCGVertexSI::setPoint(const VCGPoint3SI& p)
{
	vv.P() = p;
}

Q_INVOKABLE VCGPoint3SI VCGVertexSI::getNormal()
{
	return vv.N();
}

Q_INVOKABLE void VCGVertexSI::setNormal(const VCGPoint3SI& p)
{
	vv.N() = p;
}

QScriptValue MeshModelScriptInterfaceToScriptValue(QScriptEngine* eng, MeshModelSI* const& in)
{
	return eng->newQObject(in);
}

void MeshModelScriptInterfaceFromScriptValue(const QScriptValue& val, MeshModelSI*& out)
{
	out = qobject_cast<MeshModelSI*>(val.toQObject());
}

QScriptValue MeshDocumentScriptInterfaceToScriptValue(QScriptEngine* eng, MeshDocumentSI* const& in)
{
	return eng->newQObject(in);
}

void MeshDocumentScriptInterfaceFromScriptValue(const QScriptValue& val, MeshDocumentSI*& out)
{
	out = qobject_cast<MeshDocumentSI*>(val.toQObject());
}


QScriptValue VCGVertexScriptInterfaceToScriptValue(QScriptEngine* eng, VCGVertexSI* const& in)
{
	return eng->newQObject(in);
}

void VCGVertexScriptInterfaceFromScriptValue(const QScriptValue& val, VCGVertexSI*& out)
{
	out = qobject_cast<VCGVertexSI*>(val.toQObject());
}

QScriptValue ShotScriptInterfaceToScriptValue(QScriptEngine* eng, ShotSI* const& in)
{
	return eng->newQObject(in);
}

void ShotScriptInterfaceFromScriptValue(const QScriptValue& val, ShotSI*& out)
{
	out = qobject_cast<ShotSI*>(val.toQObject());
}

//QScriptValue VCGPoint3fScriptInterfaceToScriptValue( QScriptEngine* eng,VCGPoint3fSI* const& in )
//{
//	return eng->newQObject(in);
//}
//
//void VCGPoint3fScriptInterfaceFromScriptValue( const QScriptValue& val,VCGPoint3fSI*& out )
//{
//	out = qobject_cast<VCGPoint3fSI*>(val.toQObject());
//}

QScriptValue EnvWrap_ctor(QScriptContext* c, QScriptEngine* e)
{
	Env* env = qscriptvalue_cast<Env*>(c->argument(0));
	EnvWrap* p = new EnvWrap(*env);
	QScriptValue res = e->toScriptValue(*p);
	return res;
}

QScriptValue VCGPoint3ScriptInterface_ctor(QScriptContext *c, QScriptEngine *e)
{
	VCGPoint3SI* p = new VCGPoint3SI(Scalarm(c->argument(0).toNumber()), Scalarm(c->argument(1).toNumber()), Scalarm(c->argument(2).toNumber()));
	QScriptValue res = e->toScriptValue(*p);
	return res;
}

EnvWrap::EnvWrap(Env& envir)
	:env(&envir)
{
}

QScriptValue EnvWrap::evalExp(const QString& nm)
{
	if (!constStatement(nm))
		throw NotConstException(nm);
	QScriptValue result = env->evaluate(nm);
	QString errmsg = result.toString();
	if (result.isError())
		throw ValueNotFoundException(nm);
	return result;
}

bool EnvWrap::evalBool(const QString& nm)
{
	QScriptValue result = evalExp(nm);
	if (result.isBool())
		return result.toBool();
	else
		throw ExpressionHasNotThisTypeException("Bool", nm);
	return false;
}

double EnvWrap::evalDouble(const QString& nm)
{
	QScriptValue result = evalExp(nm);
	if (result.isNumber())
		return result.toNumber();
	else
		throw ExpressionHasNotThisTypeException("Double", nm);
	return double();
}

float EnvWrap::evalFloat(const QString& nm)
{
	try
	{
		double result = evalDouble(nm);
		return (float)result;
	}
	catch (ExpressionHasNotThisTypeException& /*exc*/)
	{
		throw ExpressionHasNotThisTypeException("Float", nm);
	}
	return float();
}

MESHLAB_SCALAR EnvWrap::evalReal(const QString& nm)
{
    try
    {
        double result = evalDouble(nm);
        return (MESHLAB_SCALAR)result;
    }
    catch (ExpressionHasNotThisTypeException& /*exc*/)
    {
        throw ExpressionHasNotThisTypeException("Real", nm);
    }
    return MESHLAB_SCALAR();
}

int EnvWrap::evalInt(const QString& nm)
{
	try
	{
		double result = evalDouble(nm);
		return (int)result;
	}
	catch (ExpressionHasNotThisTypeException&)
	{
		throw ExpressionHasNotThisTypeException("Int", nm);
	}
	return int();
}

vcg::Point3f EnvWrap::evalVec3(const QString& nm)
{
	QScriptValue result = evalExp(nm);
	QVariant resVar = result.toVariant();
	QVariantList resList = resVar.toList();
	if (resList.size() == 3)
		return vcg::Point3f(resList[0].toReal(), resList[1].toReal(), resList[2].toReal());
	else
		throw ExpressionHasNotThisTypeException("Vec3", nm);
	return vcg::Point3f();
}

vcg::Point3<MESHLAB_SCALAR> EnvWrap::evalVec3Real(const QString& nm)
{
    QScriptValue result = evalExp(nm);
    QVariant resVar = result.toVariant();
    QVariantList resList = resVar.toList();
    if (resList.size() == 3)
        return vcg::Point3<MESHLAB_SCALAR>(resList[0].toReal(), resList[1].toReal(), resList[2].toReal());
    else
        throw ExpressionHasNotThisTypeException("Vec3", nm);
    return vcg::Point3<MESHLAB_SCALAR>();
}

int EnvWrap::evalEnum(const QString& nm)
{
	return evalInt(nm);
}

MeshModel* EnvWrap::evalMesh(const QString& nm)
{
	int ii = evalInt(nm);
	QScriptValue mdsv = env->globalObject().property(ScriptAdapterGenerator::meshDocVarName());
	MeshDocumentSI* mdsi = dynamic_cast<MeshDocumentSI*>(mdsv.toQObject());
	if (mdsi != NULL)
		return mdsi->md->getMesh(ii);
	return NULL;
}

QColor EnvWrap::evalColor(const QString& nm)
{
	QScriptValue result = evalExp(nm);
	QVariant resVar = result.toVariant();
	QVariantList resList = resVar.toList();
	int colorComp = resList.size();
	if ((colorComp >= 3) && (colorComp <= 4))
	{
		bool isInt0255 = true;
		for (int ii = 0; ii < colorComp; ++ii)
		{
			bool isScalarInt = false;
			int resInt = resList[ii].toInt(&isScalarInt);

			if (!isScalarInt)
				throw ExpressionHasNotThisTypeException("Color", nm);
			else
				if ((resInt < 0) || (resInt > 255))
					isInt0255 = false;
		}
		if (isInt0255)
		{
			if (colorComp == 3)
				return QColor(resList[0].toInt(), resList[1].toInt(), resList[2].toInt());
			if (colorComp == 4)
				return QColor(resList[0].toInt(), resList[1].toInt(), resList[2].toInt(), resList[3].toInt());
		}
		else
			throw ExpressionHasNotThisTypeException("Color", nm);
	}
	else
		throw ExpressionHasNotThisTypeException("Color", nm);
	return QColor();
}

bool EnvWrap::constStatement(const QString& statement) const
{
	/*QRegExp exp("\\S+\\s*=\\s*S++;");*/
	QRegExp exp("\\S+\\s*=\\s*\\S+;");
	int ii = statement.indexOf(exp);
	return (ii == -1);
}

QString EnvWrap::evalString(const QString& nm)
{
	QScriptValue result = evalExp(nm);
	return result.toString();
}

Shotm EnvWrap::evalShot(const QString& nm)
{
	QScriptValue result = evalExp(nm);
	ShotSI* shot = qscriptvalue_cast<ShotSI*>(result);
	if (shot != NULL)
		return shot->shot;
	else
		throw ExpressionHasNotThisTypeException("Shotm", nm);
	return Shotm();

}

Q_DECLARE_METATYPE(EnvWrap)
Q_DECLARE_METATYPE(EnvWrap*)

QScriptValue Env_ctor(QScriptContext * /*context*/, QScriptEngine *engine)
{
	Env * env = new Env();
	return engine->newQObject(env, QScriptEngine::ScriptOwnership);
}

QScriptValue ShotSI_ctor(QScriptContext* c, QScriptEngine* e)
{
	if (c->argumentCount() != 8)
		return e->nullValue();
	ShotSI* shot = new ShotSI();
	QVector<Scalarm> m = qscriptvalue_cast<QVector<Scalarm>>(c->argument(0));
	if (m.size() != 16)
		return e->nullValue();
	QVector<Scalarm> tr = qscriptvalue_cast<QVector<Scalarm>>(c->argument(1));
	if (tr.size() != 3)
		return e->nullValue();
	Scalarm focal(c->argument(2).toNumber());
	QVector<Scalarm> pixelsize = qscriptvalue_cast<QVector<Scalarm>>(c->argument(3));
	if (pixelsize.size() != 2)
		return e->nullValue();
	QVector<Scalarm> centerpx = qscriptvalue_cast<QVector<Scalarm>>(c->argument(4));
	if (centerpx.size() != 2)
		return e->nullValue();
	QVector<Scalarm> viewportpx = qscriptvalue_cast<QVector<Scalarm>>(c->argument(5));
	if (viewportpx.size() != 2)
		return e->nullValue();
	QVector<Scalarm> distpx = qscriptvalue_cast<QVector<Scalarm>>(c->argument(6));
	if (distpx.size() != 2)
		return e->nullValue();
	QVector<Scalarm> k = qscriptvalue_cast<QVector<Scalarm>>(c->argument(7));
	if (k.size() != 4)
		return e->nullValue();
	Matrix44m mat = ScriptInterfaceUtilities::vector16ToVcgMatrix44(m);
	shot->shot.Extrinsics.SetRot(mat);
	Point3m tra = ScriptInterfaceUtilities::vector3ToVcgPoint3(tr);
	shot->shot.Extrinsics.SetTra(tra);
	shot->shot.Intrinsics.FocalMm = focal;
	Point2m pxsize = ScriptInterfaceUtilities::vector2ToVcgPoint2(pixelsize);
	shot->shot.Intrinsics.PixelSizeMm = pxsize;
	Point2m cent = ScriptInterfaceUtilities::vector2ToVcgPoint2(centerpx);
	shot->shot.Intrinsics.CenterPx = cent;
	vcg::Point2i vw = ScriptInterfaceUtilities::vector2ToVcgPoint2i(viewportpx);
	shot->shot.Intrinsics.ViewportPx = vw;
	Point2m d = ScriptInterfaceUtilities::vector2ToVcgPoint2(distpx);
	shot->shot.Intrinsics.DistorCenterPx = d;
	for (int ii = 0; ii < 4; ++ii)
		shot->shot.Intrinsics.k[ii] = k[ii];
	return e->newQObject(shot, QScriptEngine::ScriptOwnership);
}

QScriptValue ShotSI_defctor(QScriptContext* /*c*/, QScriptEngine* e)
{
	ShotSI* shot = new ShotSI();
	return e->newQObject(shot, QScriptEngine::ScriptOwnership);
}



Env::Env()
	:QScriptEngine()
{
	qRegisterMetaType<Scalarm>("Scalarm");
	qScriptRegisterSequenceMetaType< QVector<Scalarm> >(this);
	qScriptRegisterSequenceMetaType<Point3Vector>(this);
	qScriptRegisterSequenceMetaType<QVector<VCGVertexSI*> >(this);
	qScriptRegisterMetaType(this, MeshModelScriptInterfaceToScriptValue, MeshModelScriptInterfaceFromScriptValue);
	qScriptRegisterMetaType(this, VCGVertexScriptInterfaceToScriptValue, VCGVertexScriptInterfaceFromScriptValue);

	QScriptValue fun = newFunction(myprint, 1);
	globalObject().setProperty("print", fun);

	QScriptValue addfun = newFunction(VCGPoint3SI_addV3, 2);
	globalObject().setProperty("addV3", addfun);

	QScriptValue multfun = newFunction(VCGPoint3SI_multV3S, 2);
	globalObject().setProperty("multV3S", multfun);

	QScriptValue envwrap_ctor = newFunction(EnvWrap_ctor);
	//eng->setDefaultPrototype(qMetaTypeId<EnvWrap>(), envwrap_ctor.property("prototype"));
	globalObject().setProperty("EnvWrap", envwrap_ctor);

	QScriptValue env_ctor = newFunction(Env_ctor);
	QScriptValue metaObject = newQMetaObject(&Env::staticMetaObject, env_ctor);
	globalObject().setProperty("Env", metaObject);

	QScriptValue point_ctor = newFunction(VCGPoint3ScriptInterface_ctor);
	//QScriptValue pointmetaObject = newQMetaObject(&VCGPoint3fSI::staticMetaObject, point_ctor);
	setDefaultPrototype(qMetaTypeId<VCGPoint3SI>(), point_ctor.property("prototype"));
	globalObject().setProperty("VCGPoint3", point_ctor);
	//qScriptRegisterMetaType(this,Point3fToScriptValue,Point3fFromScriptValue);
	QScriptValue shot_ctor = newFunction(ShotSI_ctor);
	globalObject().setProperty(MLXMLElNames::shotType, shot_ctor);
	QScriptValue shot_defctor = newFunction(ShotSI_defctor);
	globalObject().setProperty(MLXMLElNames::shotType + "DefCtor", shot_defctor);
}

Env::~Env()
{
	for (int ii = 0; ii < _tobedeleted.size(); ++ii)
		delete _tobedeleted[ii];
	_tobedeleted.clear();
}

void Env::insertExpressionBinding(const QString& nm, const QString& exp)
{
	QString decl(nm + " = " + exp + ";");
	if (!nm.contains('.'))
		decl = "var " + decl;

	QScriptValue res = evaluate(decl);
	if (res.isError())
		throw JavaScriptException(res.toString());
}

void Env::insertParamsExpressionBinding(const QString& xmlpluginnamespace, const QString& pluginname, const QString& filtername, const QMap<QString, QString>& parvalmap)
{
	/****************************************************************************************************************************************************************/
	/*the function takes the filter parameters in the map and it generates for each parameter a couple of declarations*/
	/*XMLPlugins.PluginName.FilterName.paramname = paramvalue;*/
	/*var paramname = XMLPlugins.PluginName.FilterName.paramname;*/
	/*WHY NOT JUST var paramname = paramvalue;??????????????????????????????????*/
	/*BECAUSE in this way it is possible to avoid problems with the naming of the persistent parameters inside the system register*/
	/*without having to call them inside the MeshLab XML file with really long name. In this way persistent parameters and non-persistent ones behave in the same way*/
	/*****************************************************************************************************************************************************************/

	//XmlPlugins = {};
	QString namespaceassignment(" = {};\n");
	QString code = xmlpluginnamespace + namespaceassignment;
	//XmlPlugins.PluginName = {};
	code += xmlpluginnamespace + "." + pluginname + namespaceassignment;
	QString completenamespace = xmlpluginnamespace + "." + pluginname + "." + filtername;
	//XmlPlugins.PluginName.FilterName = {};
	code += completenamespace + namespaceassignment;
	for (QMap<QString, QString>::const_iterator cit = parvalmap.begin(); cit != parvalmap.end(); ++cit)
	{
		//XmlPlugins.PluginName.FilterName.varname
		QString vardecl = completenamespace + "." + cit.key();
		//XmlPlugins.PluginName.FilterName.varname = value;
		code += vardecl + " = " + cit.value() + ";\n";
	}

	for (QMap<QString, QString>::const_iterator cit = parvalmap.begin(); cit != parvalmap.end(); ++cit)
	{
		//var varname = XmlPlugins.PluginName.Filtername.varname;
		QString decl("var " + cit.key() + " = " + completenamespace + "." + cit.key() + ";\n");
		code += decl;
	}

	QScriptValue res = evaluate(code);
	if (res.isError())
		throw JavaScriptException(res.toString());
}

QString Env::output()
{
	return out;
}

void Env::appendOutput(const QString& output)
{
	out = out + output;
}

QString Env::convertToAMLScriptValidName(const QString& name)
{
	QString cp(name);
	return cp.replace("::", "_");
}


ShotSI::ShotSI(const Shotm& st)
	:shot()
{
	shot.Intrinsics = st.Intrinsics;
	shot.Extrinsics = st.Extrinsics;
}

ShotSI::ShotSI()
	:shot()
{

}

Q_INVOKABLE ShotSI* ShotSI::itSelf()
{
	return this;
}

QVector<Scalarm> ScriptInterfaceUtilities::vcgPoint2ToVector2(const Point2m &p)
{
	QVector<Scalarm> vfl(2);
	for (int ii = 0; ii < 2; ++ii)
		vfl[ii] = p[ii];
	return vfl;
}

Point2m ScriptInterfaceUtilities::vector2ToVcgPoint2(const QVector<Scalarm>& v)
{
	return Point2m(v[0], v[1]);
}


QVector<Scalarm> ScriptInterfaceUtilities::vcgPoint3ToVector3(const Point3m& p)
{
	QVector<Scalarm> vfl(3);
	for (int ii = 0; ii < 3; ++ii)
		vfl[ii] = p[ii];
	return vfl;
}

Point3m ScriptInterfaceUtilities::vector3ToVcgPoint3(const QVector<Scalarm>& v)
{
	return Point3m(v[0], v[1], v[2]);
}

QVector<Scalarm> ScriptInterfaceUtilities::vcgPoint4ToVector4(const Point4m& p)
{
	QVector<Scalarm> vfl(4);
	for (int ii = 0; ii < 4; ++ii)
		vfl[ii] = p[ii];
	return vfl;
}

Point4m ScriptInterfaceUtilities::vector4ToVcgPoint4(const QVector<Scalarm>& v)
{
	Point4m p;
	for (int ii = 0; ii < 4; ++ii)
		p[ii] = v[ii];
	return p;
}

QVector<Scalarm> ScriptInterfaceUtilities::vcgMatrix44ToVector16(const Matrix44m& m)
{
	QVector<Scalarm> vfl(16);
	for (int ii = 0; ii < 16; ++ii)
		vfl[ii] = m[ii / 4][ii % 4];
	return vfl;
}

Matrix44m ScriptInterfaceUtilities::vector16ToVcgMatrix44(const QVector<Scalarm>& v)
{
	Matrix44m m;
	for (int ii = 0; ii < 4; ++ii)
		for (int jj = 0; jj < 4; ++jj)
			m[ii][jj] = v[ii * 4 + jj];
	return m;
}

vcg::Point2i ScriptInterfaceUtilities::vector2ToVcgPoint2i(const QVector<Scalarm>& v)
{
	vcg::Point2i p;
	for (int ii = 0; ii < 2; ++ii)
		p[ii] = int(v[ii]);
	return p;
}






