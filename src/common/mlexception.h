#ifndef ML_EXCEPTION
#define ML_EXCEPTION

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
                :MeshLabException(QString("Value Name: ") + valName +  QString(" has not been found in current environment.")){}

        ~ValueNotFoundException() throw() {}
};

class QueryException : public MeshLabException
{
public:
	QueryException(const QString& syntaxError)
		:MeshLabException(QString("Query Error: ") + syntaxError){}

	~QueryException() throw() {}
};
#endif
