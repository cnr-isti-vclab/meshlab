/* History

$Log$
Revision 1.1  2006/02/05 11:15:50  mariolatronico
QEdgeLength Plugin widget for Qt Designer 4, it appears under Input Widgets


*/

#ifndef QEDGELENGTHPLUGIN_H
#define QEDGELENGTHPLUGIN_H
#include <QDesignerCustomWidgetInterface>

class QEdgeLengthPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

        public:
    QEdgeLengthPlugin(QObject *parent = 0);

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString codeTemplate() const;
    QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget *createWidget(QWidget *parent);
    void initialize(QDesignerFormEditorInterface *core);

private:
    bool initialized;
};

#endif
