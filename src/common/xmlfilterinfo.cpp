#include <QFile>
#include <QResource>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QtXmlPatterns/QXmlQuery>
#include <QUrl>
#include "xmlfilterinfo.h"
#include "mlexception.h"
#include <assert.h>

XMLFilterInfo* XMLFilterInfo::createXMLFileInfo( const QString& XMLFileName,const QString& XMLSchemaFileName,XMLMessageHandler& errXML)
{
	QXmlSchema schema;
	QAbstractMessageHandler * oldHandler = schema.messageHandler();
	schema.setMessageHandler(&errXML);
	QFile fi(XMLSchemaFileName);
	bool opened = fi.open(QFile::ReadOnly);
	if ((!opened) || (!schema.load(&fi)))
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
	return QString(MLXMLElNames::guiType+"=" + guiType + externalSep() + MLXMLElNames::guiLabel+"={data(" + xmlvariable + "/@" + MLXMLElNames::guiLabel+")}");
}

QString XMLFilterInfo::floatGuiInfo(const QString& guiType,const QString& xmlvariable)
{
	return defaultGuiInfo(guiType,xmlvariable) + externalSep() + MLXMLElNames::guiMinExpr + "={data(" + xmlvariable + "/@" + MLXMLElNames::guiMinExpr + ")}externalSep()" + MLXMLElNames::guiMaxExpr + "={data(" + xmlvariable + "/@" + MLXMLElNames::guiMaxExpr + ")}";
}

QString XMLFilterInfo::enumGuiInfo( const QString& guiType,const QString& xmlvariable )
{
	return defaultGuiInfo(guiType,xmlvariable) + externalSep() + MLXMLElNames::guiValuesList + "={data(" + xmlvariable + "/@" + MLXMLElNames::guiValuesList + ")}";

}

QString XMLFilterInfo::guiTypeSwitchQueryText(const QString& var)
{
	QString base("typeswitch(" + var + ")");
	QString caseABS("case element (" + MLXMLElNames::absPercTag + ") return <p>" + floatGuiInfo(MLXMLElNames::absPercTag,var) + "</p>/string()");
	QString caseBOOL("case element (" + MLXMLElNames::checkBoxTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::checkBoxTag,var) + "</p>/string()");
	QString caseEDIT("case element (" + MLXMLElNames::editTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::editTag,var) + "</p>/string()");
	QString caseVEC("case element (" + MLXMLElNames::vec3WidgetTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::vec3WidgetTag,var) + "</p>/string()");
	QString caseCOLOR("case element (" + MLXMLElNames::colorWidgetTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::colorWidgetTag,var) + "</p>/string()");
	QString caseSLIDER("case element (" + MLXMLElNames::sliderWidgetTag + ") return <p>" + floatGuiInfo(MLXMLElNames::sliderWidgetTag,var) + "</p>/string()");
	QString caseENUM("case element (" + MLXMLElNames::enumWidgetTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::enumWidgetTag,var) + "</p>/string()");
	QString errorMsg = "default return \"" + XMLFilterInfo::guiErrorMsg() + "\"";
	return base + " " + caseABS + " " + caseBOOL + " " + caseEDIT + " " + caseVEC + " " + caseCOLOR + " " + caseSLIDER + " " + caseENUM + " " + errorMsg;
}

QStringList XMLFilterInfo::filterNames() const
{
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER/<p>{data(@name)}</p>/string()";
	QString namesQuery = docMFIPluginFilter(fileName) + "/<p>"+ attrVal(MLXMLElNames::filterName)+"</p>/string()";
	try
	{
		return query(namesQuery);
	}
	catch(QueryException e)
	{
    qDebug("Caught a QueryException %s",e.what());
	}
	
  assert(0);
  return QStringList();
}

QString XMLFilterInfo::filterHelp( const QString& filterName) const
{
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/FILTER_HELP/string()";
	QString namesQuery = docMFIPluginFilterName(fileName,filterName) + "/" + MLXMLElNames::filterHelpTag + "/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("There is not help tag for filter " + filterName);
		return res[0];
	}
	catch(QueryException q)
	{
    qDebug("Caught a QueryException %s",q.what());
	}
  assert(0);
  return QString();
}


QString XMLFilterInfo::filterAttribute( const QString& filterName,const QString& attribute) const
{	
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/@" + attribute + "/string()";
	QString namesQuery = docMFIPluginFilterName(fileName,filterName) + "/@" + attribute + "/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("Attribute " + attribute + " has not been specified for filter " + filterName);
		return res[0]; 
	}	
	catch (QueryException e)
	{
    qDebug("Caught a QueryException %s",e.what());
	}
  assert(0);
  return QString();
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


XMLFilterInfo::XMLMapList XMLFilterInfo::filterParametersExtendedInfo( const QString& filterName) const
{
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM/<p>{data(@name)}</p>/string()";
	//QString namesQuery = docMFIPluginFilterNameParam(fileName,filterName) + "/<p>{data(@" + MLXMLElNames::paramName + ")}</p>/string()";
	
	//in order to resolve the ordering problem
	QString namesQuery = "for $a in " + docMFIPluginFilterNameParam(fileName,filterName) + " return <p>{data($a/@" + MLXMLElNames::paramName + ")}</p>/string()";
	
	XMLMapList mplist;
	try
	{
		QStringList nameList = query(namesQuery);
		foreach(QString paramName,nameList)
		{
			XMLFilterInfo::XMLMap ss = filterParameterExtendedInfo(filterName,paramName);
			mplist.push_back(ss);
		}
		return mplist;
	}
	catch (QueryException e)
	{
    qDebug("Caught a QueryException %s",e.what());
	}
  assert(0);
  return XMLFilterInfo::XMLMapList();
}

XMLFilterInfo::XMLMapList XMLFilterInfo::filterParameters( const QString& filterName) const
{
  QString namesQuery = docMFIPluginFilterNameParam(fileName,filterName) + "/<p>" + attrNameAttrVal(MLXMLElNames::paramType) + externalSep() + attrNameAttrVal(MLXMLElNames::paramName) + externalSep() + attrNameAttrVal(MLXMLElNames::paramDefExpr) + externalSep() + attrNameAttrVal(MLXMLElNames::paramIsImportant) + "</p>/string()";
  QStringList res = query(namesQuery);
  return mapListFromStringList(res);
}

XMLFilterInfo::XMLMapList XMLFilterInfo::mapListFromStringList( const QStringList& list )
{
	XMLMapList result;
	//"attribute0=value0|attribute1=value1|...|attributeN=valueN" "attribute0=value0|attribute1=value1|...|attributeN=valueN" "attribute0=value0|attribute1=value1|...|attributeN=valueN"
	foreach(QString st, list)
	{
		XMLFilterInfo::XMLMap attrValue = mapFromString(st);
		result.push_back(attrValue);
	}
	return result;
}

XMLFilterInfo::XMLMap XMLFilterInfo::mapFromString(const QString& st,const QRegExp& extsep,const QRegExp& intsep)
{
	QStringList coupleList = st.split(extsep);
	XMLFilterInfo::XMLMap attrValue;
	foreach(QString couple,coupleList)
	{
		QStringList cl = couple.split(intsep);
		if (cl.size() == 2)
			attrValue[cl[0].trimmed()]=cl[1].trimmed();
	}
	return attrValue;
}

XMLFilterInfo::XMLMap XMLFilterInfo::filterParameterGui( const QString& filterName,const QString& parameterName) const
{ 
	QString var("$gui");
	//QString totQuery("for " + var + " in doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + parameterName + "\"]/(* except PARAM_HELP) return " + guiTypeSwitchQueryText(var));
	QString totQuery("for " + var + " in " + docMFIPluginFilterNameParamName(fileName,filterName,parameterName) + "/(* except PARAM_HELP) return " + guiTypeSwitchQueryText(var));
	XMLFilterInfo::XMLMap mp;
	try
	{
		QStringList res = query(totQuery);
		XMLFilterInfo::XMLMapList tmp = mapListFromStringList(res);
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
    qDebug("Caught a QueryException %s",e.what());
	}
  assert(0);
  return XMLFilterInfo::XMLMap();
}

QString XMLFilterInfo::filterParameterHelp( const QString& filterName,const QString& paramName ) const
{
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + paramName + "\"]/PARAM_HELP/string()";
	QString namesQuery = docMFIPluginFilterNameParamName(filterName,fileName,paramName) + "/PARAM_HELP/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() == 0)
			throw ParsingException("Help section has not been defined for Parameter: " + paramName + " in filter: " + filterName);
		return res[0];
	}
	catch (QueryException e)
	{
    qDebug("Caught a QueryException %s",e.what());
	}
  assert(0);
  return QString();
}


XMLFilterInfo::XMLMap XMLFilterInfo::filterParameterExtendedInfo( const QString& filterName,const QString& paramName ) const
{
	//QString namesQuery = "for $x in doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name=\"" + filterName + "\"]/PARAM[@name=\"" + paramName + "\"] return <p>type={data($x/@type)}|name={data($x/@name)}|defaultExpression={data($x/@defaultExpression)}|help={$x/PARAM_HELP}</p>/string()";
	QString namesQuery = "for $x in " + docMFIPluginFilterNameParamName(fileName,filterName,paramName) + " return <p>" + attrNameAttrVal(MLXMLElNames::paramType,"$x/") + externalSep() + attrNameAttrVal(MLXMLElNames::paramName,"$x/") + externalSep() + attrNameAttrVal(MLXMLElNames::paramDefExpr,"$x/") + externalSep() + attrNameAttrVal(MLXMLElNames::paramIsImportant,"$x/") + externalSep() + MLXMLElNames::paramHelpTag + "={$x/" + MLXMLElNames::paramHelpTag + "}</p>/string()";
	try
	{
		XMLFilterInfo::XMLMap res;
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
				XMLFilterInfo::XMLMap tmpres = filterParameterGui(filterName,paramName);
				for(XMLFilterInfo::XMLMap::const_iterator it = tmpres.constBegin();it != tmpres.constEnd();++it)
					res[it.key()] = it.value();
			}
		}
		return res;
	}
	catch (QueryException e)
	{
    qDebug("Caught a QueryException %s",e.what());
	}
  assert(0);
  return XMLFilterInfo::XMLMap();
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
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + paramName + "\"]/@" + attribute + "/string()";
	QString namesQuery = docMFIPluginFilterNameParamName(fileName,filterName,paramName) + "/@" + attribute + "/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("Attribute " + attribute + " has not been specified for parameter " + paramName + " in filter " + filterName);
		return res[0]; 
	}	
	catch (QueryException e)
	{
    qDebug("Caught a QueryException %s",e.what());
	}
  assert(0);
  return QString();
}

QString XMLFilterInfo::pluginName() const
{
	QString namesQuery = docMFIPlugin(fileName) + "/<p>" +attrVal(MLXMLElNames::pluginScriptName)+"</p>/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("Attribute " + MLXMLElNames::pluginScriptName + " has not been specified for plugin.");
		return res[0]; 
	}
	catch(QueryException e)
	{
		qDebug("Caught a QueryException %s",e.what());
	}

	assert(0);
	return QString();
}

