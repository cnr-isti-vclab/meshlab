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

class InvalidInvariantException : public MLException
{
public:
	InvalidInvariantException(const QString& invarianterror)
		:MLException(QString("WARNING! Invalid Invariant: ") + invarianterror){}

	~InvalidInvariantException() throw() {}
};

class InvalidPluginException: public MLException
{
public:
	InvalidPluginException(const QString& error)
		:MLException(QString("Invalid Plugin Exception: ") + error){}

	~InvalidPluginException() throw() {}
};

#endif
