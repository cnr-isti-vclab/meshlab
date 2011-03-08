#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include<QScriptEngine>

class Env :public QScriptEngine
{
public:
	Env();
	void insertExpressionBinding(const QString& nm,const QString& exp);
};

class EnvWrap
{
private:
	bool constStatement(const QString& statement) const;
	QScriptValue getExp( const QString& nm );
	Env& env;
public:
	EnvWrap(Env& envir);
	bool getBool(const QString& nm);
	float getFloat(const QString& nm);
};

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