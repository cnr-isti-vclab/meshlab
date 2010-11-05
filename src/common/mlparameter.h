#ifndef ML_PARAMETER_H
#define ML_PARAMETER_H

#include"filterparameter.h"
#include<QScriptEngine>
#include"mlexception.h"

class Env;

class Expression
{
private:
	QString exp;
public:
	Expression();
	Expression(const QString& ex);
	~Expression();
	QString& expression();
	const QString& expression() const;
	//all the eval implementations could throw a ParsingException: you should catch it in the calling code
	virtual Value* eval(Env* env) const  = 0;
	virtual Value* eval(const QString& exp,Env* env) = 0;
protected:
	QScriptValue evaluate(Env* env ) const;
};

class BoolExpression : public Expression
{
public:
	BoolExpression();
	BoolExpression(const QString& ex);
	~BoolExpression();
	Value* eval(Env* env) const;
	Value* eval(const QString& boolExp,Env* env);
};

class FloatExpression : public Expression
{
public:
	FloatExpression();
	FloatExpression(const QString& ex);
	~FloatExpression();
	Value* eval(Env* env) const;
	Value* eval(const QString& floatExp,Env* env);
};

class IntExpression : public Expression
{
public:
	IntExpression();
	IntExpression(const QString& ex);
	~IntExpression();
	Value* eval(Env* env) const;
	Value* eval(const QString& intExp,Env* env);
};

class FilterEnv
{
private:
	const QMap<QString,Value*> evaluatedExpressions;
	QMap<QString,Value*>::const_iterator findValue(const QString& nm) const;
//
public:
	//FilterEnv(const Env& env);
	FilterEnv() {}

	//All these functions could throw ValueNotFoundException: you should catch it in the calling code
	bool getBool(const QString& nm) const;
	float getFloat(const QString& nm) const;
};

class ExpressionFactory
{
public:
	//all the eval implementations could throw a ParsingException: you should catch it in the calling code
	static Expression* create( const QString& type,const QString& defExpression);
};


//un istante prima della apply
//
//try 
//perogni parametro del filtro
//	valuta il parametro nell'ambiente 
//	metti il risuatato della valutazione nella mappa
//catch parsingerror
//
//
//ad ogni update dei valori mostrati nel dialogo
//
//
//QMap<QString, Expression> ExpressionToBeEvaluated
//
//QMap<QString, Expression> ::uteratir qmi;
//foreach( qmi in ExpressionToBeEvaluated);
//EvaluatedExpression [qmi.key] = qmi.val->Eval(currentEnv);
//
//Chi costruisce ExpressionToBeEvaluated
//
//parsing xml
//raccolta stringhe correnti dal dialogo (quindi la fa il dialog stesso)



class Env : public QScriptEngine
{
public:
	Env();

	//could throw a ParsingException: you should catch it in the calling code
	Value* insertLocalExpressionBinding(const QString& nm,Expression* val);
	Value* insertNewFieldToVariable(const QString& var,const QString& field,Expression* exp);
	
	//Value* removeLocalValueBinding(const QString& nm);
	//Value* insertGlobalValueBinding(const QString& nm,Value* val);
	//Value* removeGlobalValueBinding(const QString& nm);
};

#endif