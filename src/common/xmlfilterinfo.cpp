#include <QFile>
#include <QResource>
#include "xmlfilterinfo.h"
#include "mlexception.h"
#include <assert.h>
#include <QtXml>

#include "mlapplication.h"

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
    ls << MLXMLElNames::stringType;
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
    ls << MLXMLElNames::stringWidgetTag;
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

    if (mlxmltype == MLXMLElNames::stringType)
        ls << MLXMLElNames::stringWidgetTag;

    if (mlxmltype == MLXMLElNames::enumType)
        ls << MLXMLElNames::enumWidgetTag;

    if (mlxmltype == MLXMLElNames::shotType)
        ls << MLXMLElNames::shotWidgetTag;

    if (mlxmltype == MLXMLElNames::stringType)
        ls << MLXMLElNames::stringWidgetTag;
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
	ls << MLXMLElNames::paramType << MLXMLElNames::paramName << MLXMLElNames::paramDefExpr << MLXMLElNames::paramIsImportant << MLXMLElNames::paramIsPersistent;
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

