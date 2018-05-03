#ifndef XML_FILTER_INFO_H
#define XML_FILTER_INFO_H

#include<QStringList>
#include<QtXmlPatterns/QAbstractMessageHandler>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QSettings>
//#include <QtXmlPatterns/QXmlQuery>
#include <QUrl>
#include<QAction>
#include<QBuffer>

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
    const QString mfiCurrentVersion("2.0");

    const QString mfiTag("MESHLAB_FILTER_INTERFACE");
    const QString pluginTag("PLUGIN");
    const QString filterTag("FILTER");
    const QString filterHelpTag("FILTER_HELP");
    const QString filterJSCodeTag("FILTER_JSCODE");

    const QString paramTag("PARAM");
    const QString paramHelpTag("PARAM_HELP");

    const QString editTag("EDIT_GUI");
    const QString checkBoxTag("CHECKBOX_GUI");
    const QString absPercTag("ABSPERC_GUI");
    const QString vec3WidgetTag("VEC3_GUI");
    const QString colorWidgetTag("COLOR_GUI");
    const QString sliderWidgetTag("SLIDER_GUI");
    const QString enumWidgetTag("ENUM_GUI");
    const QString meshWidgetTag("MESH_GUI");
    const QString shotWidgetTag("SHOT_GUI");
    const QString stringWidgetTag("STRING_GUI");

    const QString mfiVersion("mfiVersion");

    const QString pluginScriptName("pluginName");
    const QString pluginAuthor("pluginAuthor");
    const QString pluginEmail("pluginEmail");

    const QString filterName("filterName");
    const QString filterScriptFunctName("filterFunction");
    const QString filterClass("filterClass");
    const QString filterPreCond("filterPre");
    const QString filterPostCond("filterPost");
    const QString filterArity("filterArity");
    const QString filterRasterArity("filterRasterArity");
    const QString filterIsInterruptible("filterIsInterruptible");

    //filterHelp == name to access to the value of FILTER_HELP inside the Map produced by the XMLFilterInfo
    //const QString filterHelp("f_help");

    const QString paramType("parType");
    const QString paramName("parName");
    const QString paramDefExpr("parDefault");
    const QString paramIsImportant("parIsImportant");
	const QString paramIsPersistent("parIsPersistent");
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
    const QString intType("Int");
    const QString vec3Type("Vec3");
    const QString colorType("Color");
    const QString enumType("Enum");
    const QString meshType("Mesh");
    const QString shotType("CameraShot");
    const QString stringType("String");

    //ariety values
    const QString singleMeshArity("SingleMesh");
    const QString fixedArity("Fixed");
    const QString variableArity("Variable");
    const QString functionDefinedArity("FunctionDefined");

    //raster ariety values
    const QString zeroRasterArity("NoRaster");
    const QString singleRasterArity("SingleRaster");
    const QString fixedRasterArity("Fixed");
    const QString variableRasterArity("Variable");
    const QString functionDefinedRasterArity("FunctionDefined");

    void initMLXMLTypeList(QStringList& ls);
    void initMLXMLGUITypeList(QStringList& ls);
    void initMLXMLArityValuesList(QStringList& ls);
    void initMLXMLGUIListForType( const QString& mlxmltype,QStringList& ls );
    //
    void initMLXMLTreeTag(QStringList& ls);

    void initMLXMLPluginAttributesTag(QStringList& ls);

    void initMLXMLFilterAttributesTag(QStringList& ls);
    void initMLXMLFilterElemsTag(QStringList& ls);

    void initMLXMLParamAttributesTag(QStringList& ls);
    void initMLXMLParamElemsTag(QStringList& ls);

    void initMLXMLGUIAttributesTag(const QString& guiType,QStringList& ls);
}

class MLXMLInfo
{
private:
    static const QString inputDocName() {return QString("inputDocument");}
protected:
    inline static QString doc(const QString& file) {return QString("doc($" + file + ")");}

    //QXmlQuery xmlq;
    QString fileName;
public:
    MLXMLInfo(const QString& file);
    MLXMLInfo();
    ~MLXMLInfo();

    //QStringList query(const QString& qry);
    //QStringList query(const QByteArray& indata, const QString& qry);
    QString filevarname;
};


//Query Exception should be managed by the XMLFilterInfo class (XMLFilterInfo is the class devoted to compose queries)
//Parsing Exception instead should be managed by the code calling the XMLFilterInfo's functions.
//A Parsing Exception is raised every time an unexpected and/or missing tag or attribute in an XML has been encountered.

class MLXMLPluginInfo : public MLXMLInfo
{
private:
    MLXMLPluginInfo(const QString& file);
    ~MLXMLPluginInfo();

    static QString defaultGuiInfo(const QString& guiType,const QString& xmlvariable);
    static QString floatGuiInfo(const QString& guiType,const QString& xmlvariable);
    static QString guiErrorMsg() {return QString("Error: Unknown GUI widget requested");}
    static QString guiTypeSwitchQueryText(const QString& var);
    inline static const QString externalSep() {return QString("^");}
    inline static const QRegExp extSep() {return QRegExp("\\" + externalSep());}
    inline static const QRegExp intSep() {return QRegExp("\\s*=\\s*");}
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
public:

    typedef QMap<QString,QString> XMLMap;
    typedef QList< XMLMap > XMLMapList;
    static MLXMLPluginInfo* createXMLPluginInfo( const QString& XMLFileName,const QString& XMLSchemaFileName,XMLMessageHandler& errXML);
    static void destroyXMLPluginInfo(MLXMLPluginInfo* plug);
    static XMLMap mapFromString(const QString& st,const QRegExp& extsep = extSep(),const QRegExp& intsep = intSep());
    static XMLMapList mapListFromStringList(const QStringList& list);

    QString interfaceAttribute(const QString& attribute);

    QString pluginScriptName();
    QString pluginAttribute(const QString& attribute );

    QStringList filterNames();
    QString	filterHelp(const QString& filterName);
    QString filterScriptCode(const QString& filterName);
    QString filterElement(const QString& filterName,const QString& filterElement);

    //The function returns a QList<QMap<QString,QString>>. Each map contains "type", "name" and "defaultExpression" of a single parameter.
    XMLMapList filterParameters(const QString& filterName);
    XMLMapList filterParametersExtendedInfo( const QString& filterName);
    QString filterAttribute(const QString& filterName,const QString& attribute);

    QString filterParameterHelp(const QString& filterName,const QString& paramName);
    //XMLMap filterParameterGui(const QString& filter,const QString& parameter);
    XMLMap filterParameterExtendedInfo(const QString& filter,const QString& parameter);
    QString filterParameterAttribute(const QString& filterName,const QString& paramName,const QString& attribute);
    QString filterParameterElement( const QString& filterName,const QString& paramName,const QString& elemName );

    QString pluginFilePath() const;
};

struct MLXMLGUISubTree
{
    MLXMLPluginInfo::XMLMap guiinfo;
};

struct MLXMLParamSubTree
{
    MLXMLPluginInfo::XMLMap paraminfo;
    MLXMLGUISubTree gui;
};

struct MLXMLFilterSubTree
{
    MLXMLPluginInfo::XMLMap filterinfo;
    QList<MLXMLParamSubTree> params;
};

struct MLXMLPluginSubTree
{
    MLXMLPluginInfo::XMLMap pluginfo;
    QList<MLXMLFilterSubTree> filters;
};

struct MLXMLTree
{
    MLXMLPluginInfo::XMLMap interfaceinfo;
    MLXMLPluginSubTree plugin;
};

class MLXMLUtilityFunctions
{
    static QString xmlAttrNameValue(const MLXMLPluginInfo::XMLMap& map,const QString& attname);
    static QString generateMeshLabCodeFilePreamble();
    static QString generateNameClassPlugin(const MLXMLPluginSubTree& plugtree);
    static QString generateEvalParam( const MLXMLParamSubTree& param,const QString& envname );
public:
    static const QString xmlSchemaFile() {return QString(":/script_system/meshlabfilterXMLspecificationformat.xsd");}

    static bool getEnumNamesValuesFromString(const QString& st,QMap<int,QString>& mp);
    static QString generateMeshLabXML(const MLXMLTree& tree);
    static QString generateXMLPlugin(const MLXMLPluginSubTree& plugin);
    static QString generateXMLFilter(const MLXMLFilterSubTree& filter);
    static QString generateXMLParam(const MLXMLParamSubTree& param);
    static QString generateXMLGUI(const MLXMLGUISubTree& gui);

    static void loadMeshLabXML(MLXMLTree& tree,MLXMLPluginInfo& pinfo);
    static void loadXMLPlugin(MLXMLPluginSubTree& plugin,MLXMLPluginInfo& pinfo);
    static void loadXMLFilter(const QString& name,MLXMLFilterSubTree& filter,MLXMLPluginInfo& pinfo);
    static void loadXMLParam(const QString& filtername,const QString& paramname,MLXMLParamSubTree& param,MLXMLPluginInfo& pinfo );
    static void loadXMLGUI(const QString& filtername,const QString& paramname,MLXMLGUISubTree& gui,MLXMLPluginInfo& pinfo);

    static QString generateH(const QString& basefilename,const MLXMLTree& tree );
    static QString generateCPP(const QString& basefilename,const MLXMLTree& tree );

	/*HORRIBLE!
	public PluginManager::xmlPluginsNameSpace -> for the namespace of the temporary environment create for the evaluation of the parameters and for the complete naming of the persistent parameters
	private PluginManager::pluginNameSpace -> for calling from the XMLScriptingSystem the MeshLab Filters...but it's private even if at the end it's the one that a final user will deal with
	WHY TWO AND NOT ONE?!?!?!? because for making the things easy from the programming point of view I need to have two different Plugins namespace
	*/

	//static QString xmlPluginsNameSpace() { return QString("XML") + pluginsNameSpace(); }
	static QString pluginsNameSpace() { return QString("Plugins"); }

	static QString completeFilterProgrammingName(const QString& xmlpluginsnamespace, const QString& xmlpluginsname, const QString& filterprogname);
	static QString completeVariableProgrammingName(const QString& xmlpluginsnamespace, const QString& xmlpluginsname, const QString& filterprogname, const QString& varname);
	//static QString completeVariableProgrammingName(const QString& vaname, const QString& filtername, MLXMLPluginInfo* info);

	//static QString namespaceVariableAssignment(const QString& variable, const QString& val);
};


class MeshLabFilterInterface;

struct MeshLabXMLFilterContainer
{
    MeshLabXMLFilterContainer()
    {
        act = NULL;
        xmlInfo = NULL;
        filterInterface = NULL;
    }

    QAction* act;
    MLXMLPluginInfo* xmlInfo;
    MeshLabFilterInterface* filterInterface;

	bool isValid() const
	{
		return ((act != NULL) && (xmlInfo != NULL) && (filterInterface != NULL));
	}

	QString filterName() const
	{
		if (act != nullptr)
			return act->text();
		return QString();
	}

	QString readPersistentValueFromSettings(const QString& varname) const
	{
		QString expr;
		if ((act == nullptr) || (xmlInfo == nullptr))
			return expr;
		QString filtname = filterName();
		QSettings settings;
		QString filterscriptname = xmlInfo->filterAttribute(filtname, MLXMLElNames::filterScriptFunctName);
		QString paramnamepath = MLXMLUtilityFunctions::completeVariableProgrammingName(MLXMLUtilityFunctions::pluginsNameSpace(), xmlInfo->pluginScriptName(), filterscriptname, varname);
		QString defval = xmlInfo->filterParameterExtendedInfo(filtname, varname)[MLXMLElNames::paramDefExpr];
		expr = settings.value(paramnamepath, defval).toString();
		return expr;
	}

	void writePersistentValueIntoSettings(const QString& varname, const QString& expr) const
	{
		if ((act == nullptr) || (xmlInfo == nullptr))
			return;
		QString filtname = filterName();
		QSettings settings;
		QString filterscriptname = xmlInfo->filterAttribute(filtname, MLXMLElNames::filterScriptFunctName);
		QString paramnamepath = MLXMLUtilityFunctions::completeVariableProgrammingName(MLXMLUtilityFunctions::pluginsNameSpace(), xmlInfo->pluginScriptName(), filterscriptname, varname);
		settings.setValue(paramnamepath, expr);
	}
};

#endif
