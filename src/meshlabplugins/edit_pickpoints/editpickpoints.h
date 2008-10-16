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
		//free memory used by the gui
		delete pickPointsDialog;
	}
	
	static const QString Info();

	virtual void StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void mousePressEvent(QMouseEvent *event, MeshModel &, GLArea * ) ;
	virtual void mouseMoveEvent(QMouseEvent *event, MeshModel &, GLArea * ) ;
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );

	//basically copied from void AlignPairWidget::drawPickedPoints in editalign plugin
	//Draws all the picked points on the screen
	//boundingBox - gives some indication how to scale the normal flags
	void drawPickedPoints(std::vector<PickedPointTreeWidgetItem*> &pointVector, vcg::Box3f &boundingBox);
	
private:
	//the current place the mouse clicked
	QPoint currentMousePosition;

	//flag that tells the decorate function whether to notify the dialog of this point
	bool registerPoint;
	
	//flag that tells the decorate function whether we are moving points
	bool moveSelectPoint;
	
	//the gui dialog for this plugin
	PickPointsDialog *pickPointsDialog;
	
	//we need this in order to redraw the points
	GLArea *glArea;

	//model we currently have
	MeshModel *currentModel;
	
	int overrideCursorShape;
};
	
#endif
