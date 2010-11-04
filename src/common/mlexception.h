#ifndef ML_EXCEPTION
#define ML_EXCEPTION

class ParsingException : public std::exception
{
public:
        ParsingException(const QString& text)
                :excText(text){excText = QString("Parsing Error: ") + excText;_ba = excText.toLocal8Bit();}

        ~ParsingException() throw() {}
     inline const char* what() const throw() {return _ba.constData();}

private:
	QString excText;
	QByteArray _ba;
};

class ValueNotFoundException : public std::exception
{
public:
        ValueNotFoundException(const QString& valName)
                :valNM(valName){valNM = QString("Value Name: ") + valNM +  QString(" has not been found in current environment.");_ba = valNM.toLocal8Bit();}

        ~ValueNotFoundException() throw() {}
    inline const char* what() const throw() { return _ba.constData();}
		
private:
	QString valNM;
	QByteArray _ba;
};

#endif
