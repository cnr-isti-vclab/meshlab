#ifndef ML_PARAMETER_H
#define ML_PARAMETER_H

#include"filterparameter.h"
#include<QScriptEngine>

class Env;

class ParsingException 
{
public:
	ParsingException(const QString& text)
		:excText(text){}

	inline const QString errorString() const {return excText;}
private:
	QString excText;
};

class Expression
{
private:
	QString exp;
public:
	Expression(const QString& ex);
	~Expression();
	QString& expression();
	const QString& expression() const;
	virtual Value* eval(Env* env) const  = 0;
};

class BoolExpression : public Expression
{
public:
	BoolExpression(const QString& ex);
	~BoolExpression();
	Value* eval(Env* env) const;

	
};

class FloatExpression : public Expression
{
public:
	FloatExpression(const QString& ex);
	~FloatExpression();
	Value* eval(Env* env) const;
};


class FilterEnv
{
//private:
//	const QMap<QString,Value*> evaluatedExpressions;
//
public:
	//FilterEnv(const QMap<QString,Value*>& evalExpress);
	FilterEnv() {}
//
//	bool getBool(const QString& nm) const;
//	float getFloat(const QString& nm) const;
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



class Env
{
public:
	Env(QScriptEngine* scriptEng);

	//Value* insertLocalValueBinding(const QString& nm,Value* val);
	//Value* removeLocalValueBinding(const QString& nm);
	//Value* insertGlobalValueBinding(const QString& nm,Value* val);
	//Value* removeGlobalValueBinding(const QString& nm);
	//void pushFrame();
	//void popFrame();


	QScriptEngine* eng;
};

#endif