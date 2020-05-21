#ifndef XML_FILTER_INFO_H
#define XML_FILTER_INFO_H

#include<QStringList>
#include<QtXmlPatterns/QAbstractMessageHandler>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QSettings>
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

#endif
