#ifndef XML_FILTER_INFO_H
#define XML_FILTER_INFO_H

#include<QStringList>
#include<QtXmlPatterns/QAbstractMessageHandler>
#include "interfaces.h"

class XMLMessageHandler : public QAbstractMessageHandler
{
public:
	XMLMessageHandler()
		: QAbstractMessageHandler(0)
	{
	}

	inline QString statusMessage() const
	{
		return m_description;
	}

	inline int line() const
	{
		return m_sourceLocation.line();
	}

	inline int column() const
	{
		return m_sourceLocation.column();
	}

protected:
	inline void handleMessage(QtMsgType type, const QString &description,
		const QUrl &identifier, const QSourceLocation &sourceLocation)
	{
		Q_UNUSED(type);
		Q_UNUSED(identifier);

		m_messageType = type;
		m_description = description;
		m_sourceLocation = sourceLocation;
	}

private:
	QtMsgType m_messageType;
	QString m_description;
	QSourceLocation m_sourceLocation;
};


//Query Exception should be managed by the XMLFilterInfo class (XMLFilterInfo is the class devoted to compose queries)
//Parsing Exception instead should be managed by the code calling the XMLFilterInfo's functions. 
//A Parsing Exception is raised every time an unexpected and/or missing tag or attribute in an XML has been encountered. 
//So this kind of info it's sensible for the plugin's programmer.  


class XMLFilterInfo
{
private:
	XMLFilterInfo(const QString& file)
		:fileName(file){}

	static QString defaultGuiInfo(const QString& guiType,const QString& xmlvariable);
	static QString floatGuiInfo(const QString& guiType,const QString& xmlvariable);
	QString fileName;
public:
	typedef QList< QMap<QString,QString> > MapList;
	static MapList mapListFromStringList(const QStringList& list);
	static XMLFilterInfo* createXMLFileInfo(const QString& XMLFileName,const QString& XMLSchemaFileName,XMLMessageHandler& errXML);
	inline static void deleteXMLFileInfo(XMLFilterInfo* xmlInfo) {delete xmlInfo;}
	QStringList filterNames() const;
	QString	filterHelp(const QString& filterName) const;
	
	//The function returns a QList<QMap<QString,QString>>. Each map contains "type", "name" and "defaultExpression" of a single parameter.
	MapList filterParameters(const QString& filterName) const;

	QMap<QString,QString> filterParameterGui(const QString& filter,const QString& parameter) const;
	QString filterAttribute(const QString& filterName,const QString& attribute) const;

	QStringList query(const QString& qry) const;
};

struct MeshLabXMLFilterContainer
{
	QAction* act;
	XMLFilterInfo* xmlInfo;
	MeshLabFilterInterface* filterInterface;
};

#endif