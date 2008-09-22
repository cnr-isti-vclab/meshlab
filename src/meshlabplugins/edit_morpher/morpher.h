/* A class implementing Meshlab's Edit interface that is for morphing
 * from one mesh to another.  Works as long as they have the same
 * number of vertices and they are in correspondence.
 * 
 * 
 * @author Oscar Barney
 */

#ifndef EDIT_Morpher_PLUGIN_H
#define EDIT_Morpher_PLUGIN_H

#include <QObject>
#include <QString>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include "morpherDialog.h"

class MorpherPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
public:
	//constructor
	MorpherPlugin();
	
	//destructor
	virtual ~MorpherPlugin() {
		//free memory for each action
		foreach(QAction *editAction, actionList)
			delete editAction;
		
		//free memory used by the gui
		delete morpherDialog;
	}

	virtual QList<QAction *> actions() const ;
	
	virtual const QString Info(QAction *);
	virtual const PluginInfo &Info();

	virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void mousePressEvent(QAction *, QMouseEvent *event, MeshModel &, GLArea * ) ;
	virtual void mouseMoveEvent(QAction *, QMouseEvent *event, MeshModel &, GLArea * ) ;
	virtual void mouseReleaseEvent(QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );

	
private:
	//the list of possible actions
	QList <QAction *> actionList;
	
	//constant for the name of the action for picking points
	static const char* morpherActionName;
	
	//the gui dialog for this plugin
	MorpherDialog *morpherDialog;

signals:
	void suspendEditToggle();

};
	
#endif
