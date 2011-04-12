#ifndef XML_FILTER_INFO_H
#define XML_FILTER_INFO_H

#include<QStringList>
#include<QtXmlPatterns/QAbstractMessageHandler>
#include<QAction>

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

namespace MLXMLElNames
{
	const QString mfiTag("MESHLAB_FILTER_INTERFACE");
	const QString pluginTag("PLUGIN");
	const QString filterTag("FILTER");
	const QString filterHelpTag("FILTER_HELP");
	const QString paramTag("PARAM");
	const QString paramHelpTag("PARAM_HELP");
	
	const QString editTag("EDIT_GUI");
	const QString checkBoxTag("CHECKBOX_GUI");
	const QString absPercTag("ABSPERC_GUI");
	const QString vec3WidgetTag("VEC3_GUI");
	


	const QString mfiVersion("mfiVersion");

	const QString pluginScriptName("pluginName");

	const QString filterName("filterName");
	const QString filterScriptFunctName("filterFunction");
	const QString filterClass("filterClass");
	const QString filterPreCond("filterPre");
	const QString filterPostCond("filterPost");
	const QString filterAriety("filterAriety");

	//filterHelp == name to access to the value of FILTER_HELP inside the Map produced by the XMLFilterInfo 
	//const QString filterHelp("f_help");
	
	const QString paramType("parType");
	const QString paramName("parName");
	const QString paramDefExpr("parDefault");
	const QString paramIsImportant("parIsImportant");
	//paramHelp == name to access to the value of PARAM_HELP inside the Map produced by the XMLFilterInfo 
	//const QString paramHelp("p_help");

	//guiType == name to access to the type of gui (ABSPERC_GUI, CHECKBOX_GUI etc.) inside the Map produced by the XMLFilterInfo 
	const QString guiType("guiType");
	const QString guiLabel("guiLabel");
	const QString guiMinExpr("guiMin");
	const QString guiMaxExpr("guiMax");

	//types' names inside the XML MeshLab file format
	const QString boolType("Boolean");
	const QString realType("Real");
	const QString intType("Integer");
	const QString vec3Type("Vec3");

	//ariety values
	const QString singleMeshAriety("SingleMesh");
	const QString fixedAriety("Fixed");
	const QString variableAriety("Variable");
}

//Query Exception should be managed by the XMLFilterInfo class (XMLFilterInfo is the class devoted to compose queries)
//Parsing Exception instead should be managed by the code calling the XMLFilterInfo's functions. 
//A Parsing Exception is raised every time an unexpected and/or missing tag or attribute in an XML has been encountered. 

class XMLFilterInfo
{
private:
	XMLFilterInfo(const QString& file)
		:fileName(file){}

	static QString defaultGuiInfo(const QString& guiType,const QString& xmlvariable);
	static QString floatGuiInfo(const QString& guiType,const QString& xmlvariable);
	static QString guiErrorMsg() {return QString("Error: Unknown GUI widget requested");}
	static QString guiTypeSwitchQueryText(const QString& var);
	inline static QString doc(const QString& file) {return QString("doc(\"" + file + "\")");}
	inline static QString docMFI(const QString& file) {return doc(file) + "/" + MLXMLElNames::mfiTag;}
	inline static QString docMFIPlugin(const QString& file) {return docMFI(file) + "/" + MLXMLElNames::pluginTag;}
	inline static QString docMFIPluginFilter(const QString& file) {return docMFIPlugin(file) + "/" + MLXMLElNames::filterTag;}
	inline static QString docMFIPluginFilterName(const QString& file,const QString& fname) {return docMFIPluginFilter(file) + "[@" + MLXMLElNames::filterName + " = \"" + fname + "\"]";}
	inline static QString docMFIPluginFilterParam(const QString& file) {return docMFIPluginFilter(file) + "/" + MLXMLElNames::paramTag;}
	inline static QString docMFIPluginFilterParamName(const QString& file,const QString& pname) {return docMFIPluginFilterParam(file) + "[@" + MLXMLElNames::paramName + " = \"" + pname + "\"]";}
	inline static QString docMFIPluginFilterNameParam(const QString& file,const QString& fname) {return docMFIPluginFilterName(file,fname) + "/" + MLXMLElNames::paramTag;}
	inline static QString docMFIPluginFilterNameParamName(const QString& file,const QString& fname,const QString& pname) {return docMFIPluginFilterNameParam(file,fname) + "[@" + MLXMLElNames::paramName + " = \"" + pname + "\"]";}
	inline static QString attrVal(const QString& attr,const QString& var = QString("")) {return QString("{data(" + var + "@" + attr + ")}");}
	inline static QString attrNameAttrVal(const QString& attr,const QString& var = QString("")) {return QString(attr + "=" + attrVal(attr,var));}
	QString fileName;
public:

	typedef QMap<QString,QString> XMLMap;
	typedef QList< XMLMap > XMLMapList;
	static XMLMap mapFromString(const QString& st);
	static XMLMapList mapListFromStringList(const QStringList& list);
	static XMLFilterInfo* createXMLFileInfo(const QString& XMLFileName,const QString& XMLSchemaFileName,XMLMessageHandler& errXML);
	inline static void deleteXMLFileInfo(XMLFilterInfo* xmlInfo) {delete xmlInfo;}
	
	QString pluginName() const;
	
	QStringList filterNames() const;
	QString	filterHelp(const QString& filterName) const;
	
	//The function returns a QList<QMap<QString,QString>>. Each map contains "type", "name" and "defaultExpression" of a single parameter.
	XMLMapList filterParameters(const QString& filterName) const;
	XMLMapList filterParametersExtendedInfo( const QString& filterName) const;
	QString filterAttribute(const QString& filterName,const QString& attribute) const;

	QString filterParameterHelp(const QString& filterName,const QString& paramName) const;
	XMLMap filterParameterGui(const QString& filter,const QString& parameter) const;
	XMLMap filterParameterExtendedInfo(const QString& filter,const QString& parameter) const;
	QString filterParameterAttribute(const QString& filterName,const QString& paramName,const QString& attribute) const;

	QStringList query(const QString& qry) const;
};

class MeshLabFilterInterface;

struct MeshLabXMLFilterContainer
{
	QAction* act;
	XMLFilterInfo* xmlInfo;
	MeshLabFilterInterface* filterInterface;
};

#endif