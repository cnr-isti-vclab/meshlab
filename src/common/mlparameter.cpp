#include "mlparameter.h"
#include <QScriptValue>
#include "scriptinterface.h"

Expression::Expression(const QString& ex) 
: exp(ex) {}

Expression::Expression()
:exp(){}

Expression::~Expression() {}

QString& Expression::expression()
{ return exp; }

const QString& Expression::expression() const
{ return exp; }

QScriptValue Expression::evaluate(Env* env ) const
{
	QScriptValue val = env->evaluate(expression());
	if(val.isError()) 
		throw ParsingException(QString(val.toString()));
	return val;
}

BoolExpression::BoolExpression() 
: Expression() {}

BoolExpression::BoolExpression(const QString& ex) 
: Expression(ex) {}

BoolExpression::~BoolExpression() 
{}

Value* BoolExpression::eval( Env* env ) const
{
	return new BoolValue(evaluate(env).toBool());
}

Value* BoolExpression::eval(const QString& boolExp,Env* env ) 
{
	expression() = boolExp;
	return new BoolValue(evaluate(env).toBool());
}

FloatExpression::FloatExpression() : Expression() {}
FloatExpression::FloatExpression(const QString& ex) : Expression(ex) {}

FloatExpression::~FloatExpression() {}

Value* FloatExpression::eval( Env* env ) const
{
	return new FloatValue(evaluate(env).toNumber());
}

Value* FloatExpression::eval(const QString& floatExp,Env* env ) 
{
	expression() = floatExp;
	return new FloatValue(evaluate(env).toNumber());
}

IntExpression::IntExpression() : Expression() {}
IntExpression::IntExpression(const QString& ex) : Expression(ex) {}

IntExpression::~IntExpression() {}

Value* IntExpression::eval( Env* env ) const
{
	return new IntValue(evaluate(env).toInt32());
}

Value* IntExpression::eval(const QString& intExp,Env* env ) 
{
	expression() = intExp;
	return new IntValue(evaluate(env).toInt32());
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
Env::Env()
:QScriptEngine()
{
	qScriptRegisterMetaType(this,MeshModelScriptInterfaceToScriptValue,MeshModelScriptInterfaceFromScriptValue);
}

Value* Env::insertNewFieldToVariable(const QString& var,const QString& field,Expression* exp)
{
	QString decl(var + "." + field + " = " + exp->expression() + ";");
	return exp->eval(decl,this);
}

Value* Env::insertLocalExpressionBinding( const QString& nm,Expression* exp )
{
	QString decl("var " + nm + " = " + exp->expression() + ";");
	return exp->eval(decl,this);
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

bool FilterEnv::getBool( const QString& nm ) const
{
	return findValue(nm).value()->getBool();
}

float FilterEnv::getFloat( const QString& nm ) const
{
	return findValue(nm).value()->getFloat();
}

QMap<QString,Value*>::const_iterator FilterEnv::findValue( const QString& nm ) const
{
	QMap<QString,Value*>::const_iterator it = evaluatedExpressions.find(nm);
	if (it == evaluatedExpressions.end())
		throw ValueNotFoundException(nm);
	return it;
}

Expression* ExpressionFactory::create( const QString& type,const QString& defExpression)
{
	//in order to avoid 
	Expression* exp = NULL;
	QString cleanType = type.trimmed();
	if (type == QString("Boolean"))
	{
		exp = new BoolExpression(defExpression);
		return exp;
	}

	if (type == QString("Real"))
	{
		exp= new FloatExpression(defExpression);
		return exp;
	}
	
	throw ParsingException("Expression type has been not recognized. Should never happened because we validate XML file through XML Schema!");
	//return NULL;
}