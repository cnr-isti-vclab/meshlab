#ifndef XML_FILTER_INFO_H
#define XML_FILTER_INFO_H

#include<QStringList>
#include<QtXmlPatterns/QAbstractMessageHandler>

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


class XMLFilterInfo
{
private:
	XMLFilterInfo(const QString& file)
		:fileName(file){}


	QString fileName;
public:
	static XMLFilterInfo* createXMLFileInfo(const QString& XMLFileName,const QString& XMLSchemaFileName,XMLMessageHandler& errXML);
	inline static void deleteXMLFileInfo(XMLFilterInfo* xmlInfo) {delete xmlInfo;}
	QStringList filterNames() const;
	/*QStringList filterParameters(const QString& filter);
	QStringList filterParametersAndGui(const QString& filter);*/
	QStringList query(const QString& qry,XMLMessageHandler& errQuery) const;
};

#endif