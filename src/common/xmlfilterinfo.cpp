#include "xmlfilterinfo.h"
#include<QFile>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QUrl>
#include <QtXmlPatterns/QXmlQuery>
#include <QStringList>
#include "mlexception.h"

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
		return new XMLFilterInfo(XMLFileName);
	}
	return NULL;
}

QString XMLFilterInfo::defaultGuiInfo(const QString& guiType,const QString& xmlvariable)
{
	return QString("typeGui=" + guiType + "|label={data(" + xmlvariable + "/@label)}");
}

QString XMLFilterInfo::floatGuiInfo(const QString& guiType,const QString& xmlvariable)
{
	return defaultGuiInfo(guiType,xmlvariable) + "|minExpr={data(" + xmlvariable + "/@minExpr)}|maxExpr={data(" + xmlvariable + "/@maxExpr)}";
}

QString XMLFilterInfo::guiTypeSwitchQueryText(const QString& var)
{
	QString base("typeswitch(" + var + ")");
	QString caseABS("case element (ABSPERC_GUI) return <p>" + floatGuiInfo("ABSPERC_GUI",var) + "</p>/string()");
	QString caseFLOAT("case element (CHECKBOX_GUI) return <p>" + defaultGuiInfo("CHECKBOX_GUI",var) + "</p>/string()");
	QString errorMsg = "default return \"" + XMLFilterInfo::guiErrorMsg() + "\"";
	return base + " " + caseABS + " " + caseFLOAT + " " + " " + errorMsg;
}

QStringList XMLFilterInfo::filterNames() const
{
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER/<p>{data(@name)}</p>/string()";
	try
	{
		return query(namesQuery);
	}
	catch(QueryException e)
	{
		qDebug(e.what());
	}
	
}

QString XMLFilterInfo::filterHelp( const QString& filterName) const
{
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/FILTER_HELP/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("There is not help tag for filter " + filterName);
		return res[0];
	}
	catch(QueryException q)
	{
		qDebug(q.what());
	}
}


QString XMLFilterInfo::filterAttribute( const QString& filterName,const QString& attribute) const
{	
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/@" + attribute + "/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("Attribute " + attribute + " has not been specified for filter " + filterName);
		return res[0]; 
	}	
	catch (QueryException e)
	{
		qDebug(e.what());
	}
}

QStringList XMLFilterInfo::query( const QString& qry) const
{
	XMLMessageHandler errQuery; 
	QXmlQuery xmlq;
	xmlq.setQuery(qry);
	QAbstractMessageHandler * oldHandler = xmlq.messageHandler();
	xmlq.setMessageHandler(&errQuery);
	QStringList result;

	if (!xmlq.isValid())
	{
		//errQuery = xmlq.messageHandler();
		xmlq.setMessageHandler(oldHandler);
		throw QueryException(QString("line: ") + QString::number(errQuery.line()) + " column: " + QString::number(errQuery.column()) + " - " + errQuery.statusMessage());
		
	}
	xmlq.evaluateTo(&result);
	/*QString res;
	xmlq.evaluateTo(&res);*/
	xmlq.setMessageHandler(oldHandler);
	return result;
}


XMLFilterInfo::MapList XMLFilterInfo::filterParametersExtendedInfo( const QString& filterName) const
{
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM/<p>{data(@name)}</p>/string()";
	MapList mplist;
	try
	{
		QStringList nameList = query(namesQuery);
		foreach(QString paramName,nameList)
		{
			QMap<QString,QString> ss = filterParameterExtendedInfo(filterName,paramName);
			mplist.push_back(ss);
		}
		return mplist;
	}
	catch (QueryException e)
	{
		qDebug(e.what());
	}
}

XMLFilterInfo::MapList XMLFilterInfo::filterParameters( const QString& filterName) const
{
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM/<p>type={data(@type)}|name={data(@name)}|defaultExpression={data(@defaultExpression)}</p>/string()";
	try
	{
		QStringList res = query(namesQuery);
		return mapListFromStringList(res);
	}
	catch (QueryException e)
	{
		throw e;
	}
}

XMLFilterInfo::MapList XMLFilterInfo::mapListFromStringList( const QStringList& list )
{
	MapList result;
	//"attribute0=value0|attribute1=value1|...|attributeN=valueN" "attribute0=value0|attribute1=value1|...|attributeN=valueN" "attribute0=value0|attribute1=value1|...|attributeN=valueN"
	foreach(QString st, list)
	{
		QMap<QString,QString> attrValue = mapFromString(st);
		result.push_back(attrValue);
	}
	return result;
}

QMap<QString,QString> XMLFilterInfo::mapFromString(const QString& st)
{
	QStringList coupleList = st.split('|');
	QMap<QString,QString> attrValue;
	foreach(QString couple,coupleList)
	{
		QStringList cl = couple.split('=');
		if (cl.size() == 2)
			attrValue[cl[0]]=cl[1];
	}
	return attrValue;
}

QMap<QString,QString> XMLFilterInfo::filterParameterGui( const QString& filterName,const QString& parameterName) const
{ 
	QString var("$gui");
	QString totQuery("for " + var + " in doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + parameterName + "\"]/(* except PARAM_HELP) return " + guiTypeSwitchQueryText(var));
	QMap<QString,QString> mp;
	try
	{
		QStringList res = query(totQuery);
		XMLFilterInfo::MapList tmp = mapListFromStringList(res);
		//MUST BE FOR EACH PARAMETER ONLY ONE GUI DECLARATION
		if (tmp.size() != 1)
			throw ParsingException("In filter " + filterName + " more than a single GUI declaration has been found for parameter " + parameterName);
		else if (res[0] == XMLFilterInfo::guiErrorMsg())
				//GUI NOT DEFINED
				throw ParsingException("In filter " + filterName + " no valid GUI declaration has been found for parameter " + parameterName);
			 else return tmp[0];
	}
	catch(QueryException e)
	{
		qDebug(e.what());
	}
}

QString XMLFilterInfo::filterParameterHelp( const QString& filterName,const QString& paramName ) const
{
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + paramName + "\"]/PARAM_HELP/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() == 0)
			throw ParsingException("Help section has not been defined for Parameter: " + paramName + " in filter: " + filterName);
		return res[0];
	}
	catch (QueryException e)
	{
		qDebug(e.what());
	}
}


QMap<QString,QString> XMLFilterInfo::filterParameterExtendedInfo( const QString& filterName,const QString& paramName ) const
{
	QString namesQuery = "for $x in doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name=\"" + filterName + "\"]/PARAM[@name=\"" + paramName + "\"] return <p>type={data($x/@type)}|name={data($x/@name)}|defaultExpression={data($x/@defaultExpression)}|help={$x/PARAM_HELP}</p>/string()";
	try
	{
		QMap<QString,QString> res;
		QStringList par = query(namesQuery);
		if (par.size() == 0)
			throw ParsingException("Parameter: " + paramName + " has not been defined in filter: " + filterName);
		else 
		{
			if (par.size() != 1)
				throw ParsingException("Parameter: " + paramName + " in filter: " + filterName + " is not unique. Each parameter in the same filter must have a unique name.");
			else
			{
				res = mapFromString(par[0]);
				QMap<QString,QString> tmpres = filterParameterGui(filterName,paramName);
				for(QMap<QString,QString>::const_iterator it = tmpres.constBegin();it != tmpres.constEnd();++it)
					res[it.key()] = it.value();
			}
		}
		return res;
	}
	catch (QueryException e)
	{
		qDebug(e.what());
	}
}

//QMap<QString,QString> XMLFilterInfo::filterParameter( const QString& filterName,const QString& paramName ) const
//{
//	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + paramName + "\"]/<p>type={data(@type)}|name={data(@name)}|defaultExpression={data(@defaultExpression)}</p>/string()";
//	try
//	{
//		QMap<QString,QString> res;
//		QStringList par = query(namesQuery);
//		if (par.size() == 0)
//			throw ParsingException("Parameter: " + paramName + " has not been defined in filter: " + filterName);
//		else 
//		{
//			if (par.size() != 1)
//				throw ParsingException("Parameter: " + paramName + " in filter: " + filterName + " is not unique. Each parameter in the same filter must have a unique name.");
//			else
//			{
//				QMap<QString,QString> res = mapFromString(par[0]);
//				QMap<QString,QString> tmpres = filterParameterGui(filterName,paramName);
//				for(QMap<QString,QString>::const_iterator it = tmpres.constBegin();it != tmpres.constEnd();++it)
//					res[it.key()] = it.value();
//				res["param_help"] = filterParameterHelp(filterName,paramName);
//			}
//		}
//		return res;
//	}
//	catch (QueryException e)
//	{
//		qDebug(e.what());
//	}
//}

QString XMLFilterInfo::filterParameterAttribute( const QString& filterName,const QString& paramName,const QString& attribute ) const
{
	QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + paramName + "\"]/@" + attribute + "/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("Attribute " + attribute + " has not been specified for parameter " + paramName + " in filter " + filterName);
		return res[0]; 
	}	
	catch (QueryException e)
	{
		qDebug(e.what());
	}
}