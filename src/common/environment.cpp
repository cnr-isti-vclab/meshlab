#include "environment.h"
#include <QScriptValue>
#include"mlexception.h"
#include "scriptinterface.h"

EnvWrap::EnvWrap(Env& envir)
:env(envir)
{
}

QScriptValue EnvWrap::getExp( const QString& nm )
{
	if (!constStatement(nm))
		throw NotConstException(nm);
	QScriptValue result = env.evaluate(nm);
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

bool EnvWrap::constStatement( const QString& statement ) const
{
	QRegExp exp("\\S+\\s*=\\s*S++;");
	int ii = statement.indexOf(exp);
	return (ii == -1);
}

Env::Env()
{
	qScriptRegisterMetaType(this,MeshModelScriptInterfaceToScriptValue,MeshModelScriptInterfaceFromScriptValue);
}

void Env::insertExpressionBinding( const QString& nm,const QString& exp )
{
	QString decl("var " + nm + " = " + exp + ";");
	QScriptValue res = evaluate(decl);
	if	(res.isError())
		throw JavaScriptException(res.toString());
}
