/*  A class implementing Meshlab's Edit interface that is for picking points in 3D
 * 
 * 
 * @author Oscar Barney
 */

#include <QtGui>

#include "morpher.h"

#include <meshlab/meshmodel.h>
#include <meshlab/mainwindow.h>


using namespace vcg;


MorpherPlugin::MorpherPlugin(){
	actionList << new QAction(QIcon(":/images/morpher.png"), morpherActionName, this);
	
	foreach(QAction *editAction, actionList)
	    editAction->setCheckable(true);

	morpherDialog = 0;
}

//Constants

//name of the actions
const char* MorpherPlugin::morpherActionName = "Morpher";



//just returns the action list
QList<QAction *> MorpherPlugin::actions() const {
	return actionList;
}

const QString MorpherPlugin::Info(QAction *action) 
{
	if( action->text() != tr(morpherActionName) )
		assert (0);
  	
	return tr("Morph from one object to another");
}

const PluginInfo &MorpherPlugin::Info() 
{
	static PluginInfo ai; 
	ai.Date=tr(__DATE__);
	ai.Version = tr("1.0");
	ai.Author = ("Oscar Barney");
	return ai;
} 

//called
void MorpherPlugin::Decorate(QAction * /*ac*/, MeshModel &mm, GLArea *gla)
{
	//qDebug() << "Decorate: " << mm.fileName.c_str() << " ...";
	
	//make sure we are morphing the right meshes!
	if(gla != morpherDialog->getCurrentGLArea()){
		morpherDialog->setCurrentGLArea(gla);
	}
}

void MorpherPlugin::StartEdit(QAction * /*mode*/, MeshModel &mm, GLArea *gla )
{
	//qDebug() << "Start Morpher: \n" << mm.fileName.c_str() << " ...";
	
	//Creat GUI window
	if(morpherDialog == 0)
	{
		morpherDialog = new MorpherDialog(this, gla->window());
	}

	
	morpherDialog->setCurrentGLArea(gla);
	
	//show the dialog
	morpherDialog->show();
	
	//suspend edit mode!
	connect(this, SIGNAL(suspendEditToggle()), gla, SLOT(suspendEditToggle()));
	emit suspendEditToggle();
	
}

void MorpherPlugin::EndEdit(QAction * /*mode*/, MeshModel &mm, GLArea *gla)
{
	// some cleaning at the end.
	//qDebug() << "EndEdit: cleaning everything" << mm.fileName.c_str() << " ...";

	//unsuspend edit mode
	//emit suspendEditToggle(); //<- doesnt solve the problem of double clicking on the morpher button
	disconnect(this, SIGNAL(suspendEditToggle()), gla, SLOT(suspendEditToggle()));
	
	//this will ask the user if they want to keep the changes
	morpherDialog->verifyKeepChanges();
	
	morpherDialog->hide();
}

void MorpherPlugin::mousePressEvent(QAction *, QMouseEvent *event, MeshModel &mm, GLArea * )
{
	//qDebug() << "Mouse press event: "<< mm.fileName.c_str() << " ...";
}

void MorpherPlugin::mouseMoveEvent(QAction *, QMouseEvent *event, MeshModel &mm, GLArea * ) 
{
	//qDebug() << "Mouse move event: "<< mm.fileName.c_str() << " ...";
}

void MorpherPlugin::mouseReleaseEvent(QAction *, QMouseEvent *event, MeshModel &mm, GLArea * gla)
{
	//qDebug() << "mouse release event: " << mm.fileName.c_str() << " ...";
}

Q_EXPORT_PLUGIN(MorpherPlugin)
