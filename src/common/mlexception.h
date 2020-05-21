#ifndef ML_EXCEPTION_H
#define ML_EXCEPTION_H

#include <exception>
#include <QString>

class MLException : public std::exception
{
public:
	MLException(const QString& text)
		:std::exception(),excText(text){_ba = excText.toLocal8Bit();}

	~MLException() throw() {}
	inline const char* what() const throw() {return _ba.constData();}

protected:
	QString excText;
	QByteArray _ba;

};

class ParsingException : public MLException
{
public:
        ParsingException(const QString& text)
                :MLException(QString("Parsing Error: ") + text){}

        ~ParsingException() throw() {}
};

class ValueNotFoundException : public MLException
{
public:
        ValueNotFoundException(const QString& valName)
                :MLException(QString("Value Name: ") + valName +  QString(" has not been defined in current environment.")){}

        ~ValueNotFoundException() throw() {}
};

class NotConstException : public MLException
{
public:
	NotConstException(const QString& exp)
		:MLException(QString("Expression: ") + exp +  QString(" is not a const expression. Expression contains an assignment operator \"=\".")){}

	~NotConstException() throw() {}
};


class QueryException : public MLException
{
public:
	QueryException(const QString& syntaxError)
		:MLException(QString("Query Error: ") + syntaxError){}

	~QueryException() throw() {}
};

class JavaScriptException : public MLException
{
public:
	JavaScriptException(const QString& syntaxError)
		:MLException(QString("JavaScript Error: ") + syntaxError){}

	~JavaScriptException() throw() {}
};

class ExpressionHasNotThisTypeException :  public MLException
{
public:
	ExpressionHasNotThisTypeException(const QString& expectedType,const QString& exp)
		:MLException(QString("Expression: ") + exp + " cannot be evaluated to a " + expectedType + "'s value."){}

	~ExpressionHasNotThisTypeException() throw() {}
};

class InvalidInvariantException : public MLException
{
public:
	InvalidInvariantException(const QString& invarianterror)
		:MLException(QString("WARNING! Invalid Invariant: ") + invarianterror){}

	~InvalidInvariantException() throw() {}
};
#endif
