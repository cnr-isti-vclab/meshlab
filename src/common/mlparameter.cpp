#include "mlparameter.h"
#include <QScriptValue>

Expression::Expression(const QString& ex) 
: exp(ex) {}

Expression::~Expression() {}

QString& Expression::expression()
{ return exp; }

const QString& Expression::expression() const
{ return exp; }

BoolExpression::BoolExpression(const QString& ex) 
: Expression(ex) {}

BoolExpression::~BoolExpression() 
{}

Value* BoolExpression::eval( Env* env ) const
{
	QScriptValue val = env->eng->evaluate(expression());
	if(val.isError()) 
		throw ParsingException(val.toString());
	return new FloatValue(val.toBool());
}

FloatExpression::FloatExpression(const QString& ex) : Expression(ex) {}

FloatExpression::~FloatExpression() {}

Value* FloatExpression::eval( Env* env ) const
{
	QScriptValue val = env->eng->evaluate(expression());
	if(val.isError()) 
		throw ParsingException(val.toString());
	return new FloatValue(val.toNumber());
}

//FilterEnv::FilterEnv(const QMap<QString,Value*>& evalExpress)
//:evaluatedExpressions(evalExpress)
//{
//}
//
//
//
//bool FilterEnv::getBool( const QString& nm ) const
//{
//	return evaluatedExpressions->find(nm)->getBool();
//}
//
//float FilterEnv::getFloat( const QString& nm ) const
//{
//	return evaluatedExpressions->find(nm)->getFloat();
//}
//
Env::Env( QScriptEngine* scriptEng )
{
	eng = scriptEng;
}
//
//Value* Env::insertLocalValueBinding( const QString& nm,Value* val )
//{
//
//}
//
//Value* Env::removeLocalValueBinding( const QString& nm )
//{
//
//}
//
//Value* Env::insertGlobalValueBinding( const QString& nm,Value* val )
//{
//
//}
//
//Value* Env::removeGlobalValueBinding( const QString& nm )
//{
//
//}
//
//void Env::pushFrame()
//{
//	eng->pushContext();
//}
//
//void Env::popFrame()
//{
//	eng->popContext();
//}