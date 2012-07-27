#ifndef ML_EXCEPTION_H
#define ML_EXCEPTION_H

#include <exception>

class MeshLabException : public std::exception
{
public:
	MeshLabException(const QString& text)
		:std::exception(),excText(text){_ba = excText.toLocal8Bit();}

	~MeshLabException() throw() {}
	inline const char* what() const throw() {return _ba.constData();}

protected:
	QString excText;
	QByteArray _ba;

};

class MeshLabXMLParsingException : public MeshLabException
{
public:
        MeshLabXMLParsingException(const QString& text)
                :MeshLabException(QString("Error While parsing the XML filter plugin descriptors: ") + text){}

        ~MeshLabXMLParsingException() throw() {}
};


class ParsingException : public MeshLabException
{
public:
        ParsingException(const QString& text)
                :MeshLabException(QString("Parsing Error: ") + text){}

        ~ParsingException() throw() {}
};

class ValueNotFoundException : public MeshLabException
{
public:
        ValueNotFoundException(const QString& valName)
                :MeshLabException(QString("Value Name: ") + valName +  QString(" has not been defined in current environment.")){}

        ~ValueNotFoundException() throw() {}
};

class NotConstException : public MeshLabException
{
public:
	NotConstException(const QString& exp)
		:MeshLabException(QString("Expression: ") + exp +  QString(" is not a const expression. Expression contains an assignment operator \"=\".")){}

	~NotConstException() throw() {}
};


class QueryException : public MeshLabException
{
public:
	QueryException(const QString& syntaxError)
		:MeshLabException(QString("Query Error: ") + syntaxError){}

	~QueryException() throw() {}
};

class JavaScriptException : public MeshLabException
{
public:
	JavaScriptException(const QString& syntaxError)
		:MeshLabException(QString("JavaScript Error: ") + syntaxError){}

	~JavaScriptException() throw() {}
};

class ExpressionHasNotThisTypeException :  public MeshLabException
{
public:
	ExpressionHasNotThisTypeException(const QString& expectedType,const QString& exp)
		:MeshLabException(QString("Expression: ") + exp + " cannot be evaluated to a " + expectedType + "'s value."){}

	~ExpressionHasNotThisTypeException() throw() {}
};

class InvalidInvariantException : public MeshLabException
{
public:
	InvalidInvariantException(const QString& invarianterror)
		:MeshLabException(QString("WARNING! Invalid Invariant: ") + invarianterror){}

	~InvalidInvariantException() throw() {}
};
#endif
