#include "xmlfilterinfo.h"
#include<QFile>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QUrl>
#include <QtXmlPatterns/QXmlQuery>
#include <QStringList>

XMLFilterInfo* XMLFilterInfo::createXMLFileInfo( const QString& XMLFileName,const QString& XMLSchemaFileName,XMLMessageHandler& errXML)
{
	QXmlSchema schema;
	QAbstractMessageHandler * oldHandler = schema.messageHandler();
	schema.setMessageHandler(&errXML);
	if (!schema.load(QUrl::fromLocalFile(XMLSchemaFileName)))
	{	
		schema.setMessageHandler(oldHandler);
		return NULL;
	}
	schema.setMessageHandler(oldHandler);

	if (schema.isValid()) 
	{
		QFile file(XMLFileName);
		file.open(QIODevice::ReadOnly);
		QXmlSchemaValidator validator(schema);
		oldHandler = validator.messageHandler();
		validator.setMessageHandler(&errXML);
		if (!validator.validate(&file, QUrl::fromLocalFile(file.fileName())))
		{
			//errXML = *(validator.messageHandler());
			validator.setMessageHandler(oldHandler);
			return NULL;
		}
		file.close();
		validator.setMessageHandler(oldHandler);
	}
	return new XMLFilterInfo(XMLFileName);
}

QStringList XMLFilterInfo::filterNames(XMLMessageHandler& errQuery) const
{
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER/<p>{data(@name)}</p>/string()";
	return query(namesQuery,errQuery);
}

QString XMLFilterInfo::filterHelp( const QString& filterName,bool& isValid,XMLMessageHandler& errQuery) const
{
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/FILTER_HELP/string()";
	QStringList res = query(namesQuery,errQuery);
	if (res.size() != 1)
	{
		isValid = false;
		return QString();
	}
	else
	{
		isValid = true;
		return res[0];
	}
}


QString XMLFilterInfo::filterAttribute( const QString& filterName,const QString& attribute,bool& isValid,XMLMessageHandler& errQuery) const
{	
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/@" + attribute + "/string()";
	QStringList res = query(namesQuery,errQuery);
	if (res.size() != 1)
	{
		isValid = false;
		return QString();
	}
	else
	{
		isValid = true;
		return res[0];
	}
}

QStringList XMLFilterInfo::query( const QString& qry,XMLMessageHandler& errQuery ) const
{
	QXmlQuery xmlq;
	xmlq.setQuery(qry);
	QAbstractMessageHandler * oldHandler = xmlq.messageHandler();
	xmlq.setMessageHandler(&errQuery);
	QStringList result;

	if (!xmlq.isValid())
	{
		//errQuery = xmlq.messageHandler();
		xmlq.setMessageHandler(oldHandler);
		QString mes = errQuery.statusMessage();
		return result;
	}
	xmlq.evaluateTo(&result);
	xmlq.setMessageHandler(oldHandler);
	return result;
}

QStringList XMLFilterInfo::filterParameters( const QString& filterName,bool& isValid,XMLMessageHandler& errQuery) const
{
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM/<p>{data(@type)},{data(@name)},{data(@defaultExpression)}</p>/string()";
	QStringList res = query(namesQuery,errQuery);
	if (res.size() == 0)
		isValid = false;
	else
		isValid = true;
	return res;
}
