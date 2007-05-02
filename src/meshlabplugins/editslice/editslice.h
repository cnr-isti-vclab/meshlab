
#ifndef SLIDEPLUGIN_H
#define SLIDEPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include "slicedialog.h"
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <wrap/gui/trackball.h>


class ExtraMeshSlidePlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	QList <QAction *> actionList;
	
public:
	ExtraMeshSlidePlugin();
    vcg::Trackball trackball_slice;
	virtual ~ExtraMeshSlidePlugin() {}
	virtual const QString Info(QAction *);
    virtual const PluginInfo &Info();
	virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent    (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseMoveEvent     (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseReleaseEvent  (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual QList<QAction *> actions() const ;
	void restoreDefault();
private:
    bool first;
	Slicedialog *slicedialog;
    void DrawPlane(GLArea * gla,MeshModel &m);
	bool  activeDefaultTrackball;
    bool disableTransision;
};
#endif
