#include <QFile>
#include <QResource>
#include "xmlfilterinfo.h"
#include "mlexception.h"
#include <assert.h>

MLXMLInfo::MLXMLInfo( const QString& file ) 
:fileName(file),filevarname(inputDocName())
{
}

MLXMLInfo::MLXMLInfo()
:filevarname(inputDocName())
{
}

MLXMLInfo::~MLXMLInfo()
{
}

QStringList MLXMLInfo::query( const QString& qry)
{
	QFile sourceDocument(fileName);
	QString text;
	if (sourceDocument.open(QIODevice::ReadOnly))
	{
		text = QString(sourceDocument.readAll());
		sourceDocument.close();
	}
	else
		throw MeshLabException("File " + fileName + " has not been opened.");
	QBuffer document;
	document.setData(text.toUtf8());
	document.open(QIODevice::ReadOnly);
	xmlq.bindVariable(filevarname, &document);
	XMLMessageHandler errQuery; 
	xmlq.setQuery(qry);
	QAbstractMessageHandler * oldHandler = xmlq.messageHandler();
	xmlq.setMessageHandler(&errQuery);
	QStringList result;

	if (!xmlq.isValid())
	{
		//errQuery = xmlq.messageHandler();
		xmlq.setMessageHandler(oldHandler);
		document.close();
		throw QueryException(QString("line: ") + QString::number(errQuery.line()) + " column: " + QString::number(errQuery.column()) + " - " + errQuery.statusMessage());
		
	}
	xmlq.evaluateTo(&result);
	QString err = errQuery.statusMessage();
	xmlq.setMessageHandler(oldHandler);
	document.close();
	return result;
}

QStringList MLXMLInfo::query(const QByteArray& indata, const QString& qry)
{
	QByteArray tmp(indata);
	QBuffer document(&tmp);
	document.open(QIODevice::ReadOnly);
	xmlq.bindVariable(filevarname, &document);
	XMLMessageHandler errQuery; 
	xmlq.setQuery(qry);
	QAbstractMessageHandler * oldHandler = xmlq.messageHandler();
	xmlq.setMessageHandler(&errQuery);
	QStringList result;

	if (!xmlq.isValid())
	{
		//errQuery = xmlq.messageHandler();
		xmlq.setMessageHandler(oldHandler);
		document.close();
		throw QueryException(QString("line: ") + QString::number(errQuery.line()) + " column: " + QString::number(errQuery.column()) + " - " + errQuery.statusMessage());

	}
	xmlq.evaluateTo(&result);
	xmlq.setMessageHandler(oldHandler);
	document.close();
	return result;
}



MLXMLPluginInfo* MLXMLPluginInfo::createXMLPluginInfo( const QString& XMLFileName,const QString& XMLSchemaFileName,XMLMessageHandler& errXML)
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
		return new MLXMLPluginInfo(XMLFileName);
	}
	return NULL;
}

QString MLXMLPluginInfo::defaultGuiInfo(const QString& guiType,const QString& xmlvariable)
{
	return QString(MLXMLElNames::guiType+"=" + guiType + externalSep() + MLXMLElNames::guiLabel+"={data(" + xmlvariable + "/@" + MLXMLElNames::guiLabel+")}");
}

QString MLXMLPluginInfo::floatGuiInfo(const QString& guiType,const QString& xmlvariable)
{
	return defaultGuiInfo(guiType,xmlvariable) + externalSep() + MLXMLElNames::guiMinExpr + "={data(" + xmlvariable + "/@" + MLXMLElNames::guiMinExpr + ")}" + externalSep() + MLXMLElNames::guiMaxExpr + "={data(" + xmlvariable + "/@" + MLXMLElNames::guiMaxExpr + ")}";
}

QString MLXMLPluginInfo::guiTypeSwitchQueryText(const QString& var)
{
	QString base("typeswitch(" + var + ")");
	QString caseABS("case element (" + MLXMLElNames::absPercTag + ") return <p>" + floatGuiInfo(MLXMLElNames::absPercTag,var) + "</p>/string()");
	QString caseBOOL("case element (" + MLXMLElNames::checkBoxTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::checkBoxTag,var) + "</p>/string()");
	QString caseEDIT("case element (" + MLXMLElNames::editTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::editTag,var) + "</p>/string()");
	QString caseVEC("case element (" + MLXMLElNames::vec3WidgetTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::vec3WidgetTag,var) + "</p>/string()");
	QString caseCOLOR("case element (" + MLXMLElNames::colorWidgetTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::colorWidgetTag,var) + "</p>/string()");
	QString caseSLIDER("case element (" + MLXMLElNames::sliderWidgetTag + ") return <p>" + floatGuiInfo(MLXMLElNames::sliderWidgetTag,var) + "</p>/string()");
	QString caseENUM("case element (" + MLXMLElNames::enumWidgetTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::enumWidgetTag,var) + "</p>/string()");
	QString caseMESH("case element (" + MLXMLElNames::meshWidgetTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::meshWidgetTag,var) + "</p>/string()");
	QString caseSHOT("case element (" + MLXMLElNames::shotWidgetTag + ") return <p>" + defaultGuiInfo(MLXMLElNames::shotWidgetTag,var) + "</p>/string()");
	QString errorMsg = "default return \"" + MLXMLPluginInfo::guiErrorMsg() + "\"";
	return base + " " + caseABS + " " + caseBOOL + " " + caseEDIT + " " + caseVEC + " " + caseCOLOR + " " + caseSLIDER + " " + caseENUM + " " + caseMESH + " " + caseSHOT + " " + errorMsg;
}

QStringList MLXMLPluginInfo::filterNames()
{
	QString namesQuery = "for $a in " + docMFIPluginFilter(filevarname) + " return <p>{data($a/@" + MLXMLElNames::filterName + ")}</p>/string()";
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

QString MLXMLPluginInfo::filterHelp( const QString& filterName)
{
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/FILTER_HELP/string()";
	QString namesQuery = docMFIPluginFilterName(filevarname,filterName) + "/" + MLXMLElNames::filterHelpTag + "/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("There is not help tag for filter " + filterName);
		return res[0].trimmed();
	}
	catch(QueryException q)
	{
    qDebug("Caught a QueryException %s",q.what());
	}
  assert(0);
  return QString();
}

QString MLXMLPluginInfo::filterElement( const QString& filterName,const QString& filterElement)
{
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/FILTER_HELP/string()";
	QString namesQuery = docMFIPluginFilterName(filevarname,filterName) + "/" + filterElement + "/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("There is not " + filterElement + " tag for filter " + filterName);
		return res[0].trimmed();
	}
	catch(QueryException q)
	{
		qDebug("Caught a QueryException %s",q.what());
	}
	assert(0);
	return QString();
}


QString MLXMLPluginInfo::filterAttribute( const QString& filterName,const QString& attribute)
{	
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/@" + attribute + "/string()";
	QString namesQuery = docMFIPluginFilterName(filevarname,filterName) + "/@" + attribute + "/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("Attribute " + attribute + " has not been specified for filter " + filterName);
		return res[0].trimmed(); 
	}	
	catch (QueryException e)
	{
    qDebug("Caught a QueryException %s",e.what());
	}
  assert(0);
  return QString();
}

MLXMLPluginInfo::XMLMapList MLXMLPluginInfo::filterParametersExtendedInfo( const QString& filterName)
{
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM/<p>{data(@name)}</p>/string()";
	//QString namesQuery = docMFIPluginFilterNameParam(fileName,filterName) + "/<p>{data(@" + MLXMLElNames::paramName + ")}</p>/string()";
	
	//in order to resolve the ordering problem
	QString namesQuery = "for $a in " + docMFIPluginFilterNameParam(filevarname,filterName) + " return <p>{data($a/@" + MLXMLElNames::paramName + ")}</p>/string()";
	
	XMLMapList mplist;
	try
	{
		QStringList nameList = query(namesQuery);
		foreach(QString paramName,nameList)
		{
			MLXMLPluginInfo::XMLMap ss = filterParameterExtendedInfo(filterName,paramName);
			mplist.push_back(ss);
		}
		return mplist;
	}
	catch (QueryException e)
	{
    qDebug("Caught a QueryException %s",e.what());
	}
  assert(0);
  return MLXMLPluginInfo::XMLMapList();
}

MLXMLPluginInfo::XMLMapList MLXMLPluginInfo::filterParameters( const QString& filterName)
{
  QString namesQuery = docMFIPluginFilterNameParam(filevarname,filterName) + "/<p>" + attrNameAttrVal(MLXMLElNames::paramType) + externalSep() + attrNameAttrVal(MLXMLElNames::paramName) + externalSep() + attrNameAttrVal(MLXMLElNames::paramDefExpr) + externalSep() + attrNameAttrVal(MLXMLElNames::paramIsImportant) + "</p>/string()";
  QStringList res = query(namesQuery);
  return mapListFromStringList(res);
}

MLXMLPluginInfo::XMLMapList MLXMLPluginInfo::mapListFromStringList( const QStringList& list )
{
	XMLMapList result;
	//"attribute0=value0|attribute1=value1|...|attributeN=valueN" "attribute0=value0|attribute1=value1|...|attributeN=valueN" "attribute0=value0|attribute1=value1|...|attributeN=valueN"
	foreach(QString st, list)
	{
		MLXMLPluginInfo::XMLMap attrValue = mapFromString(st.trimmed());
		result.push_back(attrValue);
	}
	return result;
}

MLXMLPluginInfo::XMLMap MLXMLPluginInfo::mapFromString(const QString& st,const QRegExp& extsep,const QRegExp& intsep)
{
	QStringList coupleList = st.split(extsep);
	MLXMLPluginInfo::XMLMap attrValue;
	foreach(QString couple,coupleList)
	{
		QStringList cl = couple.split(intsep);
		if (cl.size() == 2)
			attrValue[cl[0].trimmed()]=cl[1].trimmed();
	}
	return attrValue;
}

MLXMLPluginInfo::XMLMap MLXMLPluginInfo::filterParameterGui( const QString& filterName,const QString& parameterName)
{ 
	QString var("$gui");
	//QString totQuery("for " + var + " in doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + parameterName + "\"]/(* except PARAM_HELP) return " + guiTypeSwitchQueryText(var));
	QString totQuery("for " + var + " in " + docMFIPluginFilterNameParamName(filevarname,filterName,parameterName) + "/(* except PARAM_HELP) return " + guiTypeSwitchQueryText(var));
	MLXMLPluginInfo::XMLMap mp;
	try
	{
		QStringList res = query(totQuery);
		MLXMLPluginInfo::XMLMapList tmp = mapListFromStringList(res);
		//MUST BE FOR EACH PARAMETER ONLY ONE GUI DECLARATION
		if (tmp.size() != 1)
			throw ParsingException("In filter " + filterName + " more than a single GUI declaration has been found for parameter " + parameterName);
		else if (res[0] == MLXMLPluginInfo::guiErrorMsg())
				//GUI NOT DEFINED
				throw ParsingException("In filter " + filterName + " no valid GUI declaration has been found for parameter " + parameterName);
			 else return tmp[0];
	}
	catch(QueryException e)
	{
    qDebug("Caught a QueryException %s",e.what());
	}
  assert(0);
  return MLXMLPluginInfo::XMLMap();
}

QString MLXMLPluginInfo::filterParameterHelp( const QString& filterName,const QString& paramName )
{
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + paramName + "\"]/PARAM_HELP/string()";
	QString namesQuery = docMFIPluginFilterNameParamName(filterName,filevarname,paramName) + "/" + MLXMLElNames::paramHelpTag + "/string()";
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

QString MLXMLPluginInfo::filterParameterElement( const QString& filterName,const QString& paramName,const QString& elemName )
{
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + paramName + "\"]/PARAM_HELP/string()";
	QString namesQuery = docMFIPluginFilterNameParamName(filevarname,filterName,paramName) + "/" + elemName + "/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() == 0)
			throw ParsingException(elemName + " section has not been defined for Parameter: " + paramName + " in filter: " + filterName);
		return res[0];
	}
	catch (QueryException e)
	{
		qDebug("Caught a QueryException %s",e.what());
	}
	assert(0);
	return QString();
}

MLXMLPluginInfo::XMLMap MLXMLPluginInfo::filterParameterExtendedInfo( const QString& filterName,const QString& paramName )
{
	//QString namesQuery = "for $x in doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name=\"" + filterName + "\"]/PARAM[@name=\"" + paramName + "\"] return <p>type={data($x/@type)}|name={data($x/@name)}|defaultExpression={data($x/@defaultExpression)}|help={$x/PARAM_HELP}</p>/string()";
	QString namesQuery = "for $x in " + docMFIPluginFilterNameParamName(filevarname,filterName,paramName) + " return <p>" + attrNameAttrVal(MLXMLElNames::paramType,"$x/") + externalSep() + attrNameAttrVal(MLXMLElNames::paramName,"$x/") + externalSep() + attrNameAttrVal(MLXMLElNames::paramDefExpr,"$x/") + externalSep() + attrNameAttrVal(MLXMLElNames::paramIsImportant,"$x/") + externalSep() + MLXMLElNames::paramHelpTag + "={$x/" + MLXMLElNames::paramHelpTag + "}</p>/string()";
	try
	{
		MLXMLPluginInfo::XMLMap res;
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
				MLXMLPluginInfo::XMLMap tmpres = filterParameterGui(filterName,paramName);
				for(MLXMLPluginInfo::XMLMap::const_iterator it = tmpres.constBegin();it != tmpres.constEnd();++it)
					res[it.key()] = it.value();
			}
		}
		return res;
	}
	catch (QueryException e)
	{
		qDebug("Caught a QueryException %s",e.what());
	}
  return MLXMLPluginInfo::XMLMap();
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

QString MLXMLPluginInfo::filterParameterAttribute( const QString& filterName,const QString& paramName,const QString& attribute )
{
	//QString namesQuery = "doc(\"" + this->fileName + "\")/MESHLAB_FILTER_INTERFACE/PLUGIN/FILTER[@name = \"" + filterName + "\"]/PARAM[@name = \"" + paramName + "\"]/@" + attribute + "/string()";
	QString namesQuery = docMFIPluginFilterNameParamName(filevarname,filterName,paramName) + "/@" + attribute + "/string()";
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

QString MLXMLPluginInfo::interfaceAttribute( const QString& attribute )
{
	QString namesQuery = docMFI(filevarname) + "/<p>" +attrVal(attribute)+"</p>/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("Attribute " + attribute + " has not been specified for MeshLab XML Interface Info.");
		return res[0]; 
	}
	catch(QueryException e)
	{
		qDebug("Caught a QueryException %s",e.what());
	}

	assert(0);
	return QString();
}


QString MLXMLPluginInfo::pluginAttribute(const QString& attribute )
{
	QString namesQuery = docMFIPlugin(filevarname) + "/<p>" +attrVal(attribute)+"</p>/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() != 1)
			throw ParsingException("Attribute " + attribute + " has not been specified for plugin.");
		return res[0]; 
	}
	catch(QueryException e)
	{
		qDebug("Caught a QueryException %s",e.what());
	}

	assert(0);
	return QString();
}


QString MLXMLPluginInfo::pluginScriptName()
{
	QString namesQuery = docMFIPlugin(filevarname) + "/<p>" +attrVal(MLXMLElNames::pluginScriptName)+"</p>/string()";
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

void MLXMLPluginInfo::destroyXMLPluginInfo( MLXMLPluginInfo* plug )
{
	delete plug;
}

MLXMLPluginInfo::MLXMLPluginInfo( const QString& file )
:MLXMLInfo(file)
{
}

MLXMLPluginInfo::~MLXMLPluginInfo()
{
}

QString MLXMLPluginInfo::filterScriptCode( const QString& filterName )
{
	QString namesQuery = docMFIPluginFilterName(filevarname,filterName) + "/" + MLXMLElNames::filterJSCodeTag + "/string()";
	try
	{
		QStringList res = query(namesQuery);
		if (res.size() == 0)
			return QString();
		return res[0];
	}
	catch (QueryException e)
	{
		qDebug("Caught a QueryException %s",e.what());
	}
	assert(0);
	return QString();
}

QString MLXMLPluginInfo::pluginFilePath() const
{
	return fileName;
}


bool MLXMLUtilityFunctions::getEnumNamesValuesFromString( const QString& st,QMap<int,QString>& mp )
{
	QString cp(st);
	cp = cp.trimmed();
	QRegExp valid("Enum\\s*\\{(\\s*\\S+\\s*\\:\\s*\\d+\\s*\\|?)+\\}");
	bool res = valid.exactMatch(cp);
	QRegExp enumexp("Enum\\s*\\{");
	QRegExp spaces("\\s*");
	cp = cp.remove(enumexp).remove('}').remove(spaces);
	QRegExp extsp("\\|");
	QRegExp intsp("\\:");
	MLXMLPluginInfo::XMLMap xmlmp = MLXMLPluginInfo::mapFromString(cp,extsp,intsp);
	bool conv = true;
	for(MLXMLPluginInfo::XMLMap::iterator it = xmlmp.begin();it != xmlmp.end();++it)
	{
		mp.insert(it.value().toInt(&conv),it.key());
		res = conv && res;
	}
	return res;
}

QString MLXMLUtilityFunctions::xmlAttrNameValue( const MLXMLPluginInfo::XMLMap& map,const QString& attname )
{
	return QString(attname + "=\"" + map[attname] + "\"");
}

QString MLXMLUtilityFunctions::generateXMLGUI( const MLXMLGUISubTree& gui )
{
	QString result;
	QString guitype = gui.guiinfo[MLXMLElNames::guiType];
	result += "<" + guitype + " " + xmlAttrNameValue(gui.guiinfo,MLXMLElNames::guiLabel);
	if ((guitype == MLXMLElNames::absPercTag) || (guitype == MLXMLElNames::sliderWidgetTag))	
	{
		result += " " + xmlAttrNameValue(gui.guiinfo,MLXMLElNames::guiMinExpr) + " " 
		+ xmlAttrNameValue(gui.guiinfo,MLXMLElNames::guiMaxExpr);
	}
	result += "/>";
	return result;
}

QString MLXMLUtilityFunctions::generateXMLParam( const MLXMLParamSubTree& param )
{
	QString result;
	result += "<" + MLXMLElNames::paramTag + " " + xmlAttrNameValue(param.paraminfo,MLXMLElNames::paramType) + " " 
		+ xmlAttrNameValue(param.paraminfo,MLXMLElNames::paramName) + " " 
		+ xmlAttrNameValue(param.paraminfo,MLXMLElNames::paramDefExpr) + " "
		+ xmlAttrNameValue(param.paraminfo,MLXMLElNames::paramIsImportant) + ">";
	result += "<" + MLXMLElNames::paramHelpTag + ">" + param.paraminfo[MLXMLElNames::paramHelpTag] + "</" + MLXMLElNames::paramHelpTag + ">";
	result += generateXMLGUI(param.gui) ;
	result += "</" + MLXMLElNames::paramTag + ">";
	return result;
}

QString MLXMLUtilityFunctions::generateXMLFilter( const MLXMLFilterSubTree& filter )
{
	QString result;
	result += "<" + MLXMLElNames::filterTag + " " + xmlAttrNameValue(filter.filterinfo,MLXMLElNames::filterName) + " " 
		+ xmlAttrNameValue(filter.filterinfo,MLXMLElNames::filterScriptFunctName) + " " 
		+ xmlAttrNameValue(filter.filterinfo,MLXMLElNames::filterClass) + " "
		+ xmlAttrNameValue(filter.filterinfo,MLXMLElNames::filterPreCond) + " "
		+ xmlAttrNameValue(filter.filterinfo,MLXMLElNames::filterPostCond) + " "
		+ xmlAttrNameValue(filter.filterinfo,MLXMLElNames::filterArity) + " "
		+ MLXMLElNames::filterRasterArity + "=\"" + MLXMLElNames::singleRasterArity + "\" " 
		+ xmlAttrNameValue(filter.filterinfo,MLXMLElNames::filterIsInterruptible) + ">";
	result += "<" + MLXMLElNames::filterHelpTag + ">" + filter.filterinfo[MLXMLElNames::filterHelpTag] + "</" + MLXMLElNames::filterHelpTag + ">";
	if (!(filter.filterinfo[MLXMLElNames::filterJSCodeTag].isEmpty()))
		result += "<" + MLXMLElNames::filterJSCodeTag + ">" + filter.filterinfo[MLXMLElNames::filterJSCodeTag] + "</" + MLXMLElNames::filterJSCodeTag + ">";
	for(int ii = 0;ii < filter.params.size();++ii)
		result += generateXMLParam(filter.params[ii]);
	result += "</" + MLXMLElNames::filterTag + ">";
	return result;
}

QString MLXMLUtilityFunctions::generateXMLPlugin( const MLXMLPluginSubTree& plugin )
{
	QString result;
	result += "<" + MLXMLElNames::pluginTag + " " + xmlAttrNameValue(plugin.pluginfo,MLXMLElNames::pluginScriptName) + " " 
			+ xmlAttrNameValue(plugin.pluginfo,MLXMLElNames::pluginAuthor) + " " 
			+ xmlAttrNameValue(plugin.pluginfo,MLXMLElNames::pluginEmail) + ">";
	for(int ii = 0;ii < plugin.filters.size();++ii)
		result += generateXMLFilter(plugin.filters[ii]);
	result += "</" + MLXMLElNames::pluginTag + ">";
	return result;
}

QString MLXMLUtilityFunctions::generateMeshLabXML( const MLXMLTree& tree )
{
	QString result;
	result += "<" + MLXMLElNames::mfiTag + " " + xmlAttrNameValue(tree.interfaceinfo,MLXMLElNames::mfiVersion) + ">";
	result += generateXMLPlugin(tree.plugin);
	result += "</" + MLXMLElNames::mfiTag + ">";
	return result;
}



QString MLXMLUtilityFunctions::generateH(const QString& basefilename,const MLXMLTree& tree )
{
	QString result = MLXMLUtilityFunctions::generateMeshLabCodeFilePreamble() + "\n";
	QString upper = basefilename.toUpper();
	result += "#ifndef " + upper + "_H\n";
	result += "#define " + upper + "_H\n";
	
	result += "#include <QObject>\n";

	result += "#include <common/interfaces.h>\n\n";
	QString classname = MLXMLUtilityFunctions::generateNameClassPlugin(tree.plugin);
	result += "class " + classname + "	: public MeshLabFilterInterface\n";
	result += "{\n";
	result += "\tQ_OBJECT\n";
	result += "\tQ_INTERFACES(MeshLabFilterInterface)\n";
	result += "public:\n";
	result += "\t" + classname + "(): MeshLabFilterInterface() {}\n";
	result += "\tbool applyFilter( const QString& filterName,MeshDocument& md,EnvWrap& env, vcg::CallBackPos * cb );\n";
	result += "};\n";
	result += "#endif\n";
	return result;
}

QString MLXMLUtilityFunctions::generateCPP(const QString& basefilename,const MLXMLTree& tree )
{
	QString result = MLXMLUtilityFunctions::generateMeshLabCodeFilePreamble() + "\n";
	result += "#include \"" + basefilename + ".h\"\n";

	QString classname = MLXMLUtilityFunctions::generateNameClassPlugin(tree.plugin); 
	QString envname("env");
	result += "bool " + classname + "::applyFilter( const QString& filterName,MeshDocument& md,EnvWrap&" + envname + ", vcg::CallBackPos * cb )\n";
	result += "{\n";
	result += "\tif (md.mm() == NULL)\n";
	result += "\t\treturn false;\n";
	result += "\tCMeshO &m=md.mm()->cm;\n";
	QList<MLXMLFilterSubTree> treelist = tree.plugin.filters;
	for(int ii = 0; ii < treelist.size();++ii)
	{
		result += "\tif (filterName == \"" + treelist[ii].filterinfo[MLXMLElNames::filterName] + "\")\n";
		result += "\t{\n";
		QList<MLXMLParamSubTree> paramlist = tree.plugin.filters[ii].params;
		for(int jj = 0;jj < paramlist.size();++jj)
			result += "\t\t" + MLXMLUtilityFunctions::generateEvalParam(paramlist[jj],envname) + "\n";
		result += "\t\treturn true;\n";
		result += "\t}\n";
	}
	result += "\treturn false;\n";
	result += "}\n";
	result += "Q_EXPORT_PLUGIN(" + classname + ")\n";
	return result;
}

QString MLXMLUtilityFunctions::generateMeshLabCodeFilePreamble()
{
	QString result;
	result += "/****************************************************************************\n";
	result += "* MeshLab                                                           o o     *\n";
	result += "* A versatile mesh processing toolbox                             o     o   *\n";
	result += "*                                                                _   O  _   *\n";
	result += "* Copyright(C) 2005                                                \\/)\\/    *\n";
	result += "* Visual Computing Lab                                            /\\/|      *\n";
	result += "* ISTI - Italian National Research Council                           |      *\n";
	result += "*                                                                    \\      *\n";
	result += "* All rights reserved.                                                      *\n";
	result += "*                                                                           *\n";
	result += "* This program is free software; you can redistribute it and/or modify      *\n";   
	result += "* it under the terms of the GNU General Public License as published by      *\n";
	result += "* the Free Software Foundation; either version 2 of the License, or         *\n";
	result += "* (at your option) any later version.                                       *\n";
	result += "*                                                                           *\n";
	result += "* This program is distributed in the hope that it will be useful,           *\n";
	result += "* but WITHOUT ANY WARRANTY; without even the implied warranty of            *\n";
	result += "* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *\n";
	result += "* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *\n";
	result += "* for more details.                                                         *\n";
	result += "*                                                                           *\n";
	result += "****************************************************************************/\n";
	return result;
}

QString MLXMLUtilityFunctions::generateNameClassPlugin( const MLXMLPluginSubTree& plugtree )
{
	return QString(plugtree.pluginfo[MLXMLElNames::pluginScriptName] + "Plugin");
}

QString MLXMLUtilityFunctions::generateEvalParam( const MLXMLParamSubTree& param,const QString& envname )
{
	QString result;
	QString ptype = param.paraminfo[MLXMLElNames::paramType];
	QString varname = param.paraminfo[MLXMLElNames::paramName];
	QString lowvarname = varname.toLower();
	if (ptype == MLXMLElNames::intType)
		result += "int " + lowvarname + " = " + envname + ".evalInt(\"" + varname + "\");";

	if (ptype == MLXMLElNames::realType)
		result += "float " + lowvarname + " = " + envname + ".evalFloat(\"" + varname + "\");";

	if (ptype == MLXMLElNames::vec3Type)
		result += "vcg::Point3f " + lowvarname + " = " + envname + ".evalVec3(\"" + varname + "\");";

	if (ptype == MLXMLElNames::colorType)
		result += "QColor " + lowvarname + " = " + envname + ".evalColor(\"" + varname + "\");";

	if (ptype == MLXMLElNames::meshType)
		result += "MeshModel* " + lowvarname + " = " + envname + ".evalMesh(\"" + varname + "\");";

	//Enum has also the values declaration
	if (ptype.contains(MLXMLElNames::enumType))
		result += "int " + lowvarname + " = " + envname + ".evalEnum(\"" + varname + "\");";

	if (ptype == MLXMLElNames::boolType)
		result += "bool " + lowvarname + " = " + envname + ".evalBool(\"" + varname + "\");";

	if (ptype == MLXMLElNames::shotType)
		result += "vcg::Shotf " + lowvarname + " = " + envname + ".evalShot(\"" + varname + "\");";
	return result;
}

void MLXMLUtilityFunctions::loadMeshLabXML(MLXMLTree& tree,MLXMLPluginInfo& pinfo)
{
	tree.interfaceinfo[MLXMLElNames::mfiVersion] = pinfo.interfaceAttribute(MLXMLElNames::mfiVersion);
	loadXMLPlugin(tree.plugin,pinfo);
}

void MLXMLUtilityFunctions::loadXMLPlugin( MLXMLPluginSubTree& plugin,MLXMLPluginInfo& pinfo )
{
	QStringList tag;
	MLXMLElNames::initMLXMLPluginAttributesTag(tag);
	for(int ii = 0; ii < tag.size(); ++ii)
		plugin.pluginfo[tag[ii]] = pinfo.pluginAttribute(tag[ii]);
	QStringList names = pinfo.filterNames();
	for(int ii = 0;ii < names.size();++ii)
	{
		MLXMLFilterSubTree filter;
		loadXMLFilter(names[ii],filter,pinfo);
		plugin.filters.push_back(filter);
	}
}

void MLXMLUtilityFunctions::loadXMLFilter( const QString& filterName,MLXMLFilterSubTree& filter,MLXMLPluginInfo& pinfo )
{
	QStringList tag;
	MLXMLElNames::initMLXMLFilterAttributesTag(tag);
	for(int ii = 0; ii < tag.size(); ++ii)
		filter.filterinfo[tag[ii]] = pinfo.filterAttribute(filterName,tag[ii]);
	tag.clear();
	MLXMLElNames::initMLXMLFilterElemsTag(tag);	
	for(int ii = 0; ii < tag.size(); ++ii)
	{
		try
		{
			filter.filterinfo[tag[ii]] = pinfo.filterElement(filterName,tag[ii]);
		}
		catch (ParsingException& e)
		{
			//Should happen for example when a filter has not a jsscriptcode element defined inside. it should not block the execution.
			qDebug("loadXMLFilter Parsing Exception: %s",e.what());
		}
	}

	MLXMLPluginInfo::XMLMapList params = pinfo.filterParametersExtendedInfo(filterName);
	for(int ii = 0;ii < params.size();++ii)
	{
		MLXMLParamSubTree param;
		loadXMLParam(filterName,params[ii][MLXMLElNames::paramName],param,pinfo);
		filter.params.push_back(param);
	}
}

void MLXMLUtilityFunctions::loadXMLParam(const QString& filtername,const QString& paramname,MLXMLParamSubTree& param,MLXMLPluginInfo& pinfo )
{
	QStringList tag;
	MLXMLElNames::initMLXMLParamAttributesTag(tag);
	for(int ii = 0; ii < tag.size(); ++ii)
		param.paraminfo[tag[ii]] = pinfo.filterParameterAttribute(filtername,paramname,tag[ii]);
	tag.clear();
	MLXMLElNames::initMLXMLParamElemsTag(tag);	
	for(int ii = 0; ii < tag.size(); ++ii)
		param.paraminfo[tag[ii]] = pinfo.filterParameterElement(filtername,paramname,tag[ii]);
	loadXMLGUI(filtername,paramname,param.gui,pinfo);
}

void MLXMLUtilityFunctions::loadXMLGUI(const QString& filtername,const QString& paramname,MLXMLGUISubTree& gui,MLXMLPluginInfo& pinfo )
{
	//XMLPluginInfo::   pinfo.filterParameterExtendedInfo(filtername,paramname)[MLXMLElNames::guiType],tag);
	MLXMLPluginInfo::XMLMap map = pinfo.filterParameterExtendedInfo(filtername,paramname);
	gui.guiinfo[MLXMLElNames::guiType] = map[MLXMLElNames::guiType];
	gui.guiinfo[MLXMLElNames::guiLabel] = map[MLXMLElNames::guiLabel];
	if ((map[MLXMLElNames::guiType] == MLXMLElNames::absPercTag) || (map[MLXMLElNames::guiType] == MLXMLElNames::sliderWidgetTag))
	{
		gui.guiinfo[MLXMLElNames::guiMinExpr] = map[MLXMLElNames::guiMinExpr];
		gui.guiinfo[MLXMLElNames::guiMaxExpr] = map[MLXMLElNames::guiMaxExpr];
	}
}

void MLXMLElNames::initMLXMLTypeList( QStringList& ls )
{
	ls << MLXMLElNames::intType;
	ls << MLXMLElNames::realType;
	ls << MLXMLElNames::boolType;
	ls << MLXMLElNames::colorType;
	ls << MLXMLElNames::vec3Type;
	ls << MLXMLElNames::enumType;
	ls << MLXMLElNames::meshType;
	ls << MLXMLElNames::shotType;
}

void MLXMLElNames::initMLXMLGUITypeList( QStringList& ls )
{
	ls << MLXMLElNames::editTag;
	ls << MLXMLElNames::checkBoxTag;
	ls << MLXMLElNames::sliderWidgetTag;
	ls << MLXMLElNames::absPercTag;
	ls << MLXMLElNames::vec3WidgetTag;
	ls << MLXMLElNames::meshWidgetTag;
	ls << MLXMLElNames::colorWidgetTag;
	ls << MLXMLElNames::enumWidgetTag;
	ls << MLXMLElNames::shotWidgetTag;
}

void MLXMLElNames::initMLXMLArityValuesList( QStringList& ls )
{
	ls << MLXMLElNames::singleMeshArity;
	ls << MLXMLElNames::variableArity;
	ls << MLXMLElNames::fixedArity;
}

void MLXMLElNames::initMLXMLGUIListForType( const QString& mlxmltype,QStringList& ls )
{
	if (mlxmltype == MLXMLElNames::boolType)
	{
		ls << MLXMLElNames::checkBoxTag;
		ls << MLXMLElNames::editTag;
	}

	if ((mlxmltype == MLXMLElNames::intType) || (mlxmltype == MLXMLElNames::realType))
	{
		ls << MLXMLElNames::sliderWidgetTag;
		ls << MLXMLElNames::absPercTag;
		ls << MLXMLElNames::editTag;
	}

	if (mlxmltype == MLXMLElNames::vec3Type)
	{
		ls << MLXMLElNames::vec3WidgetTag;
		ls << MLXMLElNames::editTag;
	}

	if (mlxmltype == MLXMLElNames::colorType)
	{
		ls << MLXMLElNames::colorWidgetTag;
		ls << MLXMLElNames::editTag;
	}

	if (mlxmltype == MLXMLElNames::meshType)
		ls << MLXMLElNames::meshWidgetTag;

	if (mlxmltype == MLXMLElNames::enumType)
		ls << MLXMLElNames::enumWidgetTag;

	if (mlxmltype == MLXMLElNames::shotType)
		ls << MLXMLElNames::shotWidgetTag;
}

void MLXMLElNames::initMLXMLTreeTag( QStringList& ls )
{
	ls << MLXMLElNames::mfiVersion;
}

void MLXMLElNames::initMLXMLPluginAttributesTag( QStringList& ls )
{
	ls << MLXMLElNames::pluginScriptName << MLXMLElNames::pluginAuthor << MLXMLElNames::pluginEmail;
}

void MLXMLElNames::initMLXMLFilterAttributesTag( QStringList& ls )
{
	ls << MLXMLElNames::filterName << MLXMLElNames::filterScriptFunctName << MLXMLElNames::filterClass << MLXMLElNames::filterArity << MLXMLElNames::filterPreCond << MLXMLElNames::filterPostCond << MLXMLElNames::filterIsInterruptible;
}

void MLXMLElNames::initMLXMLFilterElemsTag( QStringList& ls )
{
	ls << MLXMLElNames::filterHelpTag << MLXMLElNames::filterJSCodeTag; 
}

void MLXMLElNames::initMLXMLParamAttributesTag( QStringList& ls )
{
	ls << MLXMLElNames::paramType << MLXMLElNames::paramName << MLXMLElNames::paramDefExpr << MLXMLElNames::paramIsImportant;
}

void MLXMLElNames::initMLXMLParamElemsTag( QStringList& ls )
{
	ls << MLXMLElNames::paramHelpTag;
}

void MLXMLElNames::initMLXMLGUIAttributesTag( const QString& guiType,QStringList& ls )
{
	ls << MLXMLElNames::guiLabel;
	if ((guiType == MLXMLElNames::sliderWidgetTag) && (guiType == MLXMLElNames::absPercTag))
		ls << MLXMLElNames::guiMinExpr << MLXMLElNames::guiMaxExpr;
}






