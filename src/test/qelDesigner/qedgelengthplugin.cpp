/* History

$Log$
Revision 1.1  2006/02/05 11:15:50  mariolatronico
QEdgeLength Plugin widget for Qt Designer 4, it appears under Input Widgets


*/
#include "qedgelengthplugin.h"
#include "../qedgelenght/QEdgeLength.h"
#include <QtPlugin>

QEdgeLengthPlugin::QEdgeLengthPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

void QEdgeLengthPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}
bool QEdgeLengthPlugin::isInitialized() const
{
    return initialized;
}
QWidget * QEdgeLengthPlugin::createWidget(QWidget *parent)
{
    return new QEdgeLength( parent);
}
QString QEdgeLengthPlugin::name() const
{
    return "QEdgeLength";
}
QString QEdgeLengthPlugin::group() const
{
    return "Input Widgets";
}
QIcon QEdgeLengthPlugin::icon() const
{
    return QIcon();
}
QString QEdgeLengthPlugin::toolTip() const
{
    return "Edge Length widget";
}
QString QEdgeLengthPlugin::whatsThis() const
{
    return "Edge Length widget set a length based on diagonal values";
}
bool QEdgeLengthPlugin::isContainer() const
{
    return false;
}
QString QEdgeLengthPlugin::domXml() const
{
    return "<widget class=\"QEdgeLength\" name=\"edgeLength\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        " <property name=\"toolTip\" >\n"
        "  <string>Edge Length widget</string>\n"
        " </property>\n"
        " <property name=\"whatsThis\" >\n"
        "  <string>Edge Length widget set a length based on "
        "diagonal values.</string>\n"
        " </property>\n"
        "</widget>\n";
}
QString QEdgeLengthPlugin::includeFile() const
{
    return "QEdgeLength.h";
}
QString QEdgeLengthPlugin::codeTemplate() const
{
    return "";
}

Q_EXPORT_PLUGIN2(qedgelengthplugin, QEdgeLengthPlugin)

