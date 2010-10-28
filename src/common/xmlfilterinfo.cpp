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
	schema.load(QUrl::fromLocalFile(XMLSchemaFileName));

	if (schema.isValid()) 
	{
		QFile file(XMLFileName);
		file.open(QIODevice::ReadOnly);

		QXmlSchemaValidator validator(schema);
		validator.setMessageHandler(&errXML);
		if (!validator.validate(&file, QUrl::fromLocalFile(file.fileName())))
		{
			//errXML = *(validator.messageHandler());
			return NULL;
		}
		file.close();
	}
	return new XMLFilterInfo(XMLFileName);
}

QStringList XMLFilterInfo::filterNames() const
{
	XMLMessageHandler errQuery;
	QString namesQuery = "doc(" + this->fileName + ")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER/<p>{data(@name)}</p>/string()";
	return query(namesQuery,errQuery);
}

QStringList XMLFilterInfo::query( const QString& qry,XMLMessageHandler& errQuery ) const
{
	QXmlQuery xmlq;
	xmlq.setQuery(qry);
	xmlq.setMessageHandler(&errQuery);
	QStringList result;

	if (!xmlq.isValid())
	{
		//errQuery = xmlq.messageHandler();
		return result;
	}
	xmlq.evaluateTo(&result);
	return result;
}