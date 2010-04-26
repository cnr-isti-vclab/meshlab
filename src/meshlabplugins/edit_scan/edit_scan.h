#ifndef VASEPLUGIN_H
#define VASEPLUGIN_H

#include <common/interfaces.h> // meshlab stuff
#include "meshlab/glarea.h" // required if you access members
#include <vector>
#include "widget.h"

using namespace vcg;
using namespace std;

#define SCANPOINTSIZE 2

class ScanLine{
public:
    // Screen offsets of scan points
    vector<Point2f> soff;
    Box2i bbox;
    bool isScanning;

    ScanLine(){}
    ScanLine(int N, Point2f&, Point2f& );
    void render(GLArea* gla);
};

class VirtualScan : public QObject, public MeshEditInterface, public MeshEditInterfaceFactory{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
    Q_INTERFACES(MeshEditInterfaceFactory)

private:
    QAction*     action;
    // Keeps track of editing document
    MeshDocument*    md;
    // Keeps the scanned cloud
    MeshModel*    cloud;
    ScanLine  sline;
    bool     isScanning;
    bool    sampleReady;
    // Function that performs the scan
    void scanpoints();
    // Timer to sample scanner
    QTimer *timer;
    // The UI of the plugin
    Widget* widget;
    // The parent
    GLArea* gla;

public slots:
    void laser_parameter_updated();

public:
    //--- Dummy implementation of MeshEditInterface
    static const QString Info(){ return tr("Virtual Scan "); }
    virtual bool StartEdit(MeshDocument &md, GLArea *parent);
    virtual void EndEdit(MeshModel &, GLArea* gla);
    virtual void Decorate(MeshModel& m, GLArea* gla);
    virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * );
    virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea* );
    virtual void mouseReleaseEvent(QMouseEvent *, MeshModel &, GLArea* );
    virtual void keyReleaseEvent(QKeyEvent*, MeshModel &/*m*/, GLArea *){
        isScanning = false;
    }
    virtual void keyPressEvent(QKeyEvent *e, MeshModel &/*m*/, GLArea *){
        if( e->key() == Qt::Key_S )
            isScanning = true;
    }

    //--- Dummy implementation of MeshEditInterfaceFactory, passes control to this MeshEditInterface
    VirtualScan(){
        action = new QAction(QIcon(":/images/scan.png"),"Virtual scanner", this);
        action->setCheckable(true);
        widget = NULL;
    }
    virtual ~VirtualScan(){ delete action; }
    virtual QList<QAction *> actions() const{
        QList<QAction *> actionList;
        return actionList << action;
    }
    virtual MeshEditInterface* getMeshEditInterface(QAction* ){ return this; }
    virtual QString getEditToolDescription(QAction *){  return this->Info(); }

private:
    //--- Virtual scan functions
    void scanPoints();
public slots:
    // called repeatedly by the timer
    void readytoscan(){ sampleReady = true; }
    void scan_requested(){
        isScanning = true;
        sampleReady = true;
        gla->update();
    }
    void save_requested();
};

#endif

