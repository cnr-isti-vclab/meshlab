#ifndef VASEPLUGIN_H
#define VASEPLUGIN_H

#include <common/interfaces.h> // meshlab stuff
#include <vasewidget.h> // vase widget GUI class
#include "balloon.h" // all balloon logic in here

using namespace vcg;

// This plugin only defines one action
//static QAction act(QIcon(":/images/vase.png"),"Volume Aware Surface Extraction", NULL);

class EditVasePlugin : public QObject, public MeshEditInterface {
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
//    Q_INTERFACES(MeshEditInterfaceFactory)

private:
    // Instance of dialog window
    VaseWidget* gui;

public:
    //--- Dummy implementation of MeshEditInterface, passes all control to Widget
    static QString Info(){ return tr("VASE"); }
    virtual bool StartEdit(MeshModel &m, GLArea* gla);
    virtual void EndEdit(MeshModel &, GLArea*){ delete gui; }
    virtual void Decorate(MeshModel& m, GLArea* gla){ gui->decorate(m,gla); }
    virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * );
    virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea* );
    virtual void mouseReleaseEvent(QMouseEvent *, MeshModel &, GLArea* );
    //--- Dummy implementation of MeshEditInterfaceFactory, passes control to this MeshEditInterface
    EditVasePlugin(){};
    virtual ~EditVasePlugin(){}
//    virtual QList<QAction *> actions() const{ QList<QAction *> actionList; return actionList << &act; }
//    virtual MeshEditInterface* getMeshEditInterface(QAction* ){ return this; }
//    virtual QString getEditToolDescription(QAction *){  return this->Info(); }
};

#endif

