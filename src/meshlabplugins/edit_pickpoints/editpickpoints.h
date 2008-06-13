/*  A class implementing Meshlab's Edit interface that is for picking points in 3D
 * 
 * 
 * @author Oscar Barney
 */

#ifndef EDIT_PickPoints_PLUGIN_H
#define EDIT_PickPoints_PLUGIN_H

#include <QObject>
#include <QString>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include "pickpointsDialog.h"

class EditPickPointsPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
public:
	//constructor
	EditPickPointsPlugin();
	
	//destructor
	virtual ~EditPickPointsPlugin() {
		//free memory for each action
		foreach(QAction *editAction, actionList)
			delete editAction;
		
		//free memory used by the gui
		delete pickPointsDialog;
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

	//basically copied from void AlignPairWidget::drawPickedPoints in editalign plugin
	//Draws all the picked points on the screen
	//boundingBox - gives some indication how to scale the normal flags
	void drawPickedPoints(std::vector<PickedPointTreeWidgetItem*> &pointVector, vcg::Box3f &boundingBox);
	
private:
	//the list of possible actions
	QList <QAction *> actionList;
	
	//constant for the name of the action for picking points
	static const char* pickPointsActionName;
	
	//the current place the mouse clicked
	QPoint currentMousePosition;

	//flag that tells the decorate function whether to add the latest point
	bool addPoint;
	
	//flag that tells the decorate function whether we are moving points
	bool movePoint;
	
	//the gui dialog for this plugin
	PickPointsDialog *pickPointsDialog;
	
	//we need this in order to redraw the points
	GLArea *glArea;

	//model we currently have
	MeshModel *currentModel;
};
	
#endif
