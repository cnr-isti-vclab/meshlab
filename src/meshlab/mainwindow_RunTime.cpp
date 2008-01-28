/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
History

$Log$
Revision 1.146  2008/01/28 13:02:00  cignoni
added support for filters on collection of meshes (layer filters)

Revision 1.145  2008/01/16 01:39:55  cignoni
added two slot for better managing the exit from editing tools

Revision 1.144  2008/01/06 20:45:11  cignoni
busy cursor when saving

Revision 1.143  2008/01/04 00:46:28  cignoni
Changed the decoration framework. Now it accept a, global, parameter set. Added static calls for finding important directories in a OS independent way.

Revision 1.142  2007/12/23 10:50:23  cignoni
disable lighting for point based mesh (with no faces)

Revision 1.141  2007/12/13 00:18:28  cignoni
added meshCreation class of filter, and the corresponding menu new under file

Revision 1.140  2007/11/25 09:48:38  cignoni
Changed the interface of the io filters. Now also a default bit set for the capabilities has to specified

Revision 1.139  2007/11/20 12:00:24  cignoni
added setcurrent slot

Revision 1.138  2007/11/05 23:45:34  cignoni
automatic closure of stdialog on other filters

Revision 1.137  2007/11/05 13:49:52  cignoni
better managment of the filter parameter dialog (stddialog)

Revision 1.136  2007/10/19 21:39:37  cignoni
Avoid recreation of normals for loaded mesh with explicit per vertex normal

Revision 1.135  2007/10/08 08:55:09  cignoni
moved out code for saving of aln

Revision 1.134  2007/10/06 23:29:51  cignoni
corrected management of suspeneded editing actions. Added filter toolbar

Revision 1.133  2007/10/02 07:59:40  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.132  2007/09/15 09:07:33  cignoni
Added missing return

Revision 1.131  2007/07/24 07:19:01  cignoni
managed failure in loading of project. Added safe cleaning of meshes with nan coords

Revision 1.130  2007/07/13 15:18:22  cignoni
Save and load of beloved aln files

Revision 1.129  2007/07/10 07:19:27  cignoni
** Serious Changes **
again on the MeshDocument, the management of multiple meshes, layers, and per mesh transformation

Revision 1.128  2007/05/16 15:02:06  cignoni
Better management of toggling between edit actions and camera movement

Revision 1.127  2007/04/20 09:57:00  cignoni
Smarter Callback that does not slow down the system when called too frequently

Revision 1.126  2007/04/16 09:24:37  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing...

Revision 1.125  2007/03/27 12:20:17  cignoni
Revamped logging iterface, changed function names in automatic parameters, better selection handling

Revision 1.124  2007/03/26 08:25:09  zifnab1974
added eol at the end of the files

Revision 1.123  2007/03/20 16:22:34  cignoni
Big small change in accessing mesh interface. First step toward layers

Revision 1.122  2007/03/05 22:31:33  cignoni
V100 Final patch of version checking

Revision 1.121  2007/03/03 02:03:25  cignoni
Reformatted lower bar, added number of selected faces. Updated about dialog

Revision 1.120  2007/03/02 15:58:32  cignoni
Progress bar bug removed

Revision 1.119  2007/03/01 04:56:28  cignoni
Added checks on the existence and readability of input files

Revision 1.118  2007/02/26 12:03:44  cignoni
Added Help online and check for updates

Revision 1.117  2007/02/08 23:45:27  pirosu
merged srcpar and par in the GetStdParameters() function

Revision 1.116  2007/02/08 16:04:18  cignoni
Corrected behaviour of edit actions

Revision 1.115  2007/01/18 13:35:02  gfrei
Resolved crash when a edit-plugin is selected without a mesh.
Resolves multiple-pushed edit-buttons problem.

Revision 1.114  2006/12/27 21:41:41  pirosu
Added improvements for the standard plugin window:
split of the apply button in two buttons:ok and apply
added support for parameters with absolute and percentage values

Revision 1.113  2006/12/13 17:37:02  pirosu
Added standard plugin window support


Revision 1.112  2006/11/30 23:23:13  cignoni
Open take care also of updating the bbox

Revision 1.111  2006/11/29 00:53:43  cignoni
Improved logging and added web based version checking

Revision 1.110  2006/11/08 15:51:00  cignoni
Corrected bug apply last filter on empty workspace

Revision 1.109  2006/11/08 01:04:48  cignoni
First version with http communications

Revision 1.108  2006/11/07 09:15:27  cignoni
Added Drag n drog opening of files (thanks to Valentino Fiorin)

****************************************************************************/


#include <QtGui>
#include <QToolBar>
#include <QProgressBar>
#include <QHttp>
#include <QDesktopServices>

#include "meshmodel.h"
#include "interfaces.h"
#include "mainwindow.h"
#include "glarea.h"
#include "plugindialog.h"
#include "filterScriptDialog.h"
#include "customDialog.h"
#include "lightingDialog.h"
#include "saveSnapshotDialog.h"
#include "ui_aboutDialog.h"
#include "savemaskexporter.h"
#include "plugin_support.h"
#include "stdpardialog.h"
#include "layerDialog.h"
#include "alnParser.h"

#include <wrap/io_trimesh/io_mask.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/clean.h>


void MainWindow::updateRecentFileActions()
{
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();
	int numRecentFiles = qMin(files.size(), (int)MAXRECENTFILES);
	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MAXRECENTFILES; ++j)	recentFileActs[j]->setVisible(false);
	separatorAct->setVisible(numRecentFiles > 0);
}

// When we switch the current model (and we change the active window)
// we have to close the stddialog.
// this one is called when user switch current window.
void MainWindow::updateStdDialog()
{
	if(stddialog==0) return;
	if(stddialog->isHidden()) return;
	if(GLA()==0) 
	{
		stddialog->close();
		return;
	}
	if(stddialog->curModel != GLA()->mm()) stddialog->close();
}

void MainWindow::updateWindowMenu()
{
	windowsMenu->clear();
	windowsMenu->addAction(closeAllAct);
	windowsMenu->addSeparator();
	windowsMenu->addAction(windowsTileAct);
	windowsMenu->addAction(windowsCascadeAct);
	windowsMenu->addAction(windowsNextAct);
	windowsNextAct->setEnabled(workspace->windowList().size()>1);

	QWidgetList windows = workspace->windowList();

	if(windows.size() > 0)
			windowsMenu->addSeparator();

	int i=0;
	foreach(QWidget *w,windows)
	{
		QString text = tr("&%1. %2").arg(i+1).arg(QFileInfo(w->windowTitle()).fileName());
		QAction *action  = windowsMenu->addAction(text);
		action->setCheckable(true);
		action->setChecked(w == workspace->activeWindow());
		// Connect the signal to activate the selected window
		connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
		windowMapper->setMapping(action, w);
		++i;
	}
}

void MainWindow::setColorMode(QAction *qa)
{
	if(qa->text() == tr("&None"))					GLA()->setColorMode(GLW::CMNone);
	if(qa->text() == tr("Per &Vertex"))		GLA()->setColorMode(GLW::CMPerVert);
	if(qa->text() == tr("Per &Face"))			GLA()->setColorMode(GLW::CMPerFace);
}

void MainWindow::updateMenus()
{
	bool active = (bool) !workspace->windowList().empty() && workspace->activeWindow();
	closeAct->setEnabled(active);
	reloadAct->setEnabled(active);
	saveAsAct->setEnabled(active);
	saveSnapshotAct->setEnabled(active);
	filterMenu->setEnabled(active && !filterMenu->actions().isEmpty());
	editMenu->setEnabled(active && !editMenu->actions().isEmpty());
	renderMenu->setEnabled(active);
	fullScreenAct->setEnabled(active);
	trackBallMenu->setEnabled(active);
	logMenu->setEnabled(active);
  windowsMenu->setEnabled(active);
	preferencesMenu->setEnabled(active);
	
	renderToolBar->setEnabled(active);
	
	showToolbarRenderAct->setChecked(renderToolBar->isVisible());
	showToolbarStandardAct->setChecked(mainToolBar->isVisible());
	if(active){
		const RenderMode &rm=GLA()->getCurrentRenderMode();
		switch (rm.drawMode) {
			case GLW::DMBox:				renderBboxAct->setChecked(true);                break;
			case GLW::DMPoints:			renderModePointsAct->setChecked(true);      		break;
			case GLW::DMWire: 			renderModeWireAct->setChecked(true);      			break;
			case GLW::DMFlat:				renderModeFlatAct->setChecked(true);    				break;
			case GLW::DMSmooth:			renderModeSmoothAct->setChecked(true);  				break;
			case GLW::DMFlatWire:		renderModeFlatLinesAct->setChecked(true);				break;
			case GLW::DMHidden:			renderModeHiddenLinesAct->setChecked(true);			break;
		default: break;
		}
    colorModePerFaceAct->setEnabled(HasPerFaceColor(GLA()->mm()->cm)); 
		switch (rm.colorMode)
		{
			case GLW::CMNone:			colorModeNoneAct->setChecked(true);	      break;
			case GLW::CMPerVert:	colorModePerVertexAct->setChecked(true);  break;
  		case GLW::CMPerFace:	colorModePerFaceAct->setChecked(true);    break;
  		default: break;
		}

		lastFilterAct->setEnabled(false);
		if(GLA()->getLastAppliedFilter() != NULL)
		{
			lastFilterAct->setText(QString("Apply filter ") + GLA()->getLastAppliedFilter()->text());
			lastFilterAct->setEnabled(true);
		}
		else
		{
			lastFilterAct->setText(QString("Apply filter "));
		}


    // Management of the editing toolbar
		// when you enter in a editing mode you can toggle between editing 
		// and camera moving by esc;
		// you exit from editing mode by pressing again the editing button
		// When you are in a editing mode all the other editing are disabled.
		
    foreach (QAction *a,editActionList)
         {
						a->setChecked(false); 
						a->setEnabled( GLA()->getEditAction() == NULL ); 
				 }

    suspendEditModeAct->setChecked(GLA()->suspendedEditor);
		suspendEditModeAct->setDisabled(GLA()->getEditAction() == NULL);

    if(GLA()->getEditAction())   
				{
						GLA()->getEditAction()->setChecked(! GLA()->suspendedEditor);
						GLA()->getEditAction()->setEnabled(true);
				}
		
		showInfoPaneAct->setChecked(GLA()->infoAreaVisible);
		showTrackBallAct->setChecked(GLA()->isTrackBallVisible());
		backFaceCullAct->setChecked(GLA()->getCurrentRenderMode().backFaceCull);
		renderModeTextureAct->setEnabled(GLA()->mm() && !GLA()->mm()->cm.textures.empty());
		renderModeTextureAct->setChecked(GLA()->getCurrentRenderMode().textureMode != GLW::TMNone);
		
		setLightAct->setIcon(rm.lighting ? QIcon(":/images/lighton.png") : QIcon(":/images/lightoff.png") );
		setLightAct->setChecked(rm.lighting);

		setFancyLightingAct->setChecked(rm.fancyLighting);
		setDoubleLightingAct->setChecked(rm.doubleSideLighting);
		setSelectionRenderingAct->setChecked(rm.selectedFaces);

		foreach (QAction *a,decoratorActionList){a->setChecked(false);}
		if(GLA()->iDecoratorsList){
			pair<QAction *,FilterParameterSet *> p;
			foreach (p,*GLA()->iDecoratorsList){p.first->setChecked(true);}
		}
	}
	
	
	if(GLA()) 
	{
		showLayerDlgAct->setChecked(GLA()->layerDialog->isVisible());
		//if(GLA()->layerDialog->isVisible())
					GLA()->layerDialog->updateTable();
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	//qDebug("dragEnterEvent: %s",event->format());
	event->accept();
}

void MainWindow::dropEvent ( QDropEvent * event )  
{
  //qDebug("dropEvent: %s",event->format());
	const QMimeData * data = event->mimeData();
	if (data->hasUrls())
	{
		QList< QUrl > url_list = data->urls();
		for (int i=0, size=url_list.size(); i<size; i++)
		{
			QString path = url_list.at(i).toLocalFile();
			if( ( QApplication::keyboardModifiers () == Qt::ControlModifier ) && ( GLA() != NULL ) )
				open(path,GLA());
				else open(path);
		}
	}
}

void MainWindow::delCurrentMesh()
{
	GLA()->meshDoc.delMesh(GLA()->meshDoc.mm());
	stddialog->hide();
	GLA()->updateGL();
	updateMenus();	
}


void MainWindow::setCurrent(int meshId)
{
	GLA()->meshDoc.setCurrentMesh(meshId);
	stddialog->hide();
}

void MainWindow::updateGL()
{
	GLA()->updateGL();
}

void MainWindow::endEdit()
{
	GLA()->endEdit(); 
}
void MainWindow::applyLastFilter()
{
  if(GLA()==0) return;
  GLA()->getLastAppliedFilter()->activate(QAction::Trigger);
}
void MainWindow::showFilterScript()
{
  FilterScriptDialog dialog(this);
	dialog.setScript(&(GLA()->filterHistory));
	if (dialog.exec()==QDialog::Accepted) 
	{
			runFilterScript();
      return ;
	}

}

void MainWindow::runFilterScript()
{
  FilterScript::iterator ii;
  for(ii= GLA()->filterHistory.actionList.begin();ii!= GLA()->filterHistory.actionList.end();++ii)
  {
    QAction *action = filterMap[ (*ii).first];
	  MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());

    int req=iFilter->getRequirements(action);
    GLA()->mm()->updateDataMask(req);
    iFilter->applyFilter( action, *(GLA()->mm()), (*ii).second, QCallBack );
    GLA()->log.Logf(GLLogStream::Info,"Re-Applied filter %s",qPrintable((*ii).first));
	}
}

// /////////////////////////////////////////////////
// The Very Important Procedure of applying a filter
// /////////////////////////////////////////////////
// It is splitted in two part
// - startFilter that setup the dialogs and asks for parameters
// - executeFilter callback invoked when the params have been set up.


void MainWindow::startFilter()
{
	QAction *action = qobject_cast<QAction *>(sender());
	MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());

	if(GLA() == NULL && iFilter->getClass(action) != MeshFilterInterface::MeshCreation) return;

	if(iFilter->getClass(action) == MeshFilterInterface::MeshCreation)
	{
		int mask = 0;
		MeshModel *mm= new MeshModel();	
		GLArea *gla=new GLArea(workspace);
	  gla->meshDoc.addMesh(mm);			
		gla->setFileName("untitled.ply");
		workspace->addWindow(gla);
		if(workspace->isVisible()) gla->showMaximized();
	}
	// Ask for filter requirements (eg a filter can need topology, border flags etc)
  // and statisfy them
	
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));	  
  int req=iFilter->getRequirements(action);
  GLA()->mm()->updateDataMask(req);
  qApp->restoreOverrideCursor();	
	
	// (2) Ask for filter parameters (e.g. user defined threshold that could require a widget)
  // bool ret=iFilter->getStdParameters(action, GLA(),*(GLA()->mm()), *par);
	FilterParameterSet parList;
  
	//Hide the std dialog just in case to avoid that two different filters runs mixed
	stddialog->hide();
  
	if(iFilter->autoDialog(action))
	{
		/// Start the automatic dialog with the collected parameters
		stddialog->showAutoDialog(iFilter, GLA()->mm(), &(GLA()->meshDoc), action, this);
  }
	else if(iFilter->customDialog(action))
	{
		/// Start the custom dialog with the collected parameters
		iFilter->getParameters(action, GLA(),*(GLA()->mm()), parList, this);
  }	
	else executeFilter(action,parList);
}

/* 
	callback function that actually start the chosen filter. 
  it is called once the parameters have been filled. 
	It can be called 
	from the automatic dialog 
	from the user defined dialog 
*/
void MainWindow::executeFilter(QAction *action, FilterParameterSet &params)
{

	MeshFilterInterface         *iFilter    = qobject_cast<        MeshFilterInterface *>(action->parent());  
	
  // (3) save the current filter and its parameters in the history
  GLA()->filterHistory.actionList.append(qMakePair(action->text(),params));

  qb->show();
  iFilter->setLog(&(GLA()->log));
  // (4) Apply the Filter 
	bool ret;
  qApp->setOverrideCursor(QCursor(Qt::WaitCursor));	  
	GLA()->mm()->busy=true;
	ret=iFilter->applyFilter(action,   GLA()->meshDoc, params, QCallBack);
	GLA()->mm()->busy=false;
  qApp->restoreOverrideCursor();	

  // (5) Apply post filter actions (e.g. recompute non updated stuff if needed)

	if(ret)
	{
		GLA()->log.Logf(GLLogStream::Info,"Applied filter %s",qPrintable(action->text()));
		GLA()->setWindowModified(true);
		GLA()->setLastAppliedFilter(action);
		lastFilterAct->setText(QString("Apply filter ") + action->text());  
		lastFilterAct->setEnabled(true);
	}

  // at the end for filters that change the color set the appropriate color mode
  if(iFilter->getClass(action)==MeshFilterInterface::FaceColoring ) {
    GLA()->setColorMode(vcg::GLW::CMPerFace);
    GLA()->mm()->ioMask|=MeshModel::IOM_FACECOLOR;
  }
  if(iFilter->getClass(action)==MeshFilterInterface::VertexColoring ){
    GLA()->setColorMode(vcg::GLW::CMPerVert);
    GLA()->mm()->ioMask|=MeshModel::IOM_VERTCOLOR;
    GLA()->mm()->ioMask|=MeshModel::IOM_VERTQUALITY;
  }
if(iFilter->getClass(action)==MeshFilterInterface::Selection )
    GLA()->setSelectionRendering(true);

  qb->reset();
  updateMenus();
  GLA()->update();
}

// Edit Mode Managment
// At any point there can be a single editing plugin active. 
// When a plugin is active it intercept the mouse actions. 
// Each active editing tools 
//
//

	
void MainWindow::suspendEditMode()
{
   // return if no window is open
  if(!GLA()) return;  
	
	// return if no editing action is currently ongoing
  if(!GLA()->getEditAction()) return;

	GLA()->suspendEditToggle();
	updateMenus();
}
void MainWindow::applyEditMode()
{
	if(!GLA()) { //prevents crash without mesh
		QAction *action = qobject_cast<QAction *>(sender()); 
		action->setChecked(false);
		return;
	}

	QAction *action = qobject_cast<QAction *>(sender());

	if(GLA()->getEditAction()) //prevents multiple buttons pushed
		{
		  if(action==GLA()->getEditAction()) // We have double pressed the same action and that means disable that actioon
			{
				if(GLA()->suspendedEditor) 
				{
					suspendEditMode();
					return;
				}
				GLA()->endEdit();
				updateMenus();
				return;
			}
			assert(0); // it should be impossible to start an action without having ended the previous one.
			return;
		}
		
	MeshEditInterface *iEdit = qobject_cast<MeshEditInterface *>(action->parent());
  GLA()->setEdit(iEdit,action);
//  GLA()->setLastAppliedEdit(action);

  iEdit->StartEdit(action,*(GLA()->mm()),GLA());
	GLA()->log.Logf(GLLogStream::Info,"Started Mode %s",qPrintable (action->text()));
  //GLA()->setSelectionRendering(true);
  updateMenus();
}

void MainWindow::applyRenderMode()
{
	QAction *action = qobject_cast<QAction *>(sender());		// find the action which has sent the signal 
	
	// Make the call to the plugin core
	MeshRenderInterface *iRenderTemp = qobject_cast<MeshRenderInterface *>(action->parent());
	iRenderTemp->Init(action,*(GLA()->mm()),GLA()->getCurrentRenderMode(),GLA());

	if(action->text() == tr("None"))
	{
		GLA()->log.Logf(GLLogStream::Info,"No Shader");
		GLA()->setRenderer(0,0); //vertex and fragment programs not supported
	} else {
		if(iRenderTemp->isSupported())
		{
			GLA()->setRenderer(iRenderTemp,action);
			GLA()->log.Logf(GLLogStream::Info,"%s",qPrintable(action->text()));	// Prints out action name
		}
		else
		{
			GLA()->setRenderer(0,0); //vertex and fragment programs not supported
			GLA()->log.Logf(GLLogStream::Warning,"Shader not supported!");
		}
	}
}


void MainWindow::applyDecorateMode()
{
	QAction *action = qobject_cast<QAction *>(sender());		// find the action which has sent the signal 

	MeshDecorateInterface *iDecorateTemp = qobject_cast<MeshDecorateInterface *>(action->parent());
	if(GLA()->iDecoratorsList==0)
		GLA()->iDecoratorsList= new list<pair<QAction *,FilterParameterSet *> >;

	bool found=false;
	pair<QAction *,FilterParameterSet *> p;
	foreach(p,*GLA()->iDecoratorsList)
	{
		if(p.first->text()==action->text()){
			delete p.second; 
		  //p.second=0;
			GLA()->iDecoratorsList->remove(p);
			GLA()->log.Logf(0,"Disabled Decorate mode %s",qPrintable(action->text()));
			found=true;
		} 
	}
	
	if(!found){
	  FilterParameterSet * decoratorParams = new FilterParameterSet();
		iDecorateTemp->initGlobalParameterSet(action,decoratorParams);
		GLA()->iDecoratorsList->push_back(make_pair(action,decoratorParams));
		GLA()->log.Logf(GLLogStream::Info,"Enable Decorate mode %s",qPrintable(action->text()));
	}
	GLA()->update();
}

bool MainWindow::QCallBack(const int pos, const char * str)
{
  int static lastPos=-1;
	if(pos==lastPos) return true;
	lastPos=pos;
	
  static QTime currTime;
	if(currTime.elapsed()< 100) return true;
	currTime.start();
  MainWindow::globalStatusBar()->showMessage(str,5000);
	qb->show();
	qb->setEnabled(true);
	qb->setValue(pos);
	MainWindow::globalStatusBar()->update();
  qApp->processEvents();
	return true;
}

void MainWindow::setLight()			     
{
	GLA()->setLight(!GLA()->getCurrentRenderMode().lighting);
	updateMenus();
};

void MainWindow::setDoubleLighting()
{
	const RenderMode &rm = GLA()->getCurrentRenderMode();
	GLA()->setLightMode(!rm.doubleSideLighting,LDOUBLE);
}

void MainWindow::setFancyLighting()
{
	const RenderMode &rm = GLA()->getCurrentRenderMode();
	GLA()->setLightMode(!rm.fancyLighting,LFANCY);
}

void MainWindow::setLightingProperties()
{
	// retrieve current lighting settings
	GLLightSetting GLlightsetting = GLA()->getLightSettings();
	
	// customize them
	LightingDialog dlg(GLlightsetting, this);
	if (dlg.exec() == QDialog::Accepted)
	{
		// update light settings
		dlg.lightSettingsToGL(GLlightsetting);
		GLA()->setLightSettings(GLlightsetting);

		// update lighting model
		GLA()->setLightModel();
	}
}

void MainWindow::toggleBackFaceCulling()
{
	RenderMode &rm = GLA()->getCurrentRenderMode();
	GLA()->setBackFaceCulling(!rm.backFaceCull);
}

void MainWindow::toggleSelectionRendering()
{
	RenderMode &rm = GLA()->getCurrentRenderMode();
	GLA()->setSelectionRendering(!rm.selectedFaces);
}

bool MainWindow::openIn(QString /* fileName */)
{
	return open(QString(),GLA());
}

void MainWindow::saveProject()
{

	QString fileName = QFileDialog::getSaveFileName(this,tr("Save Project File"),"untitled.aln", tr("*.aln"));
	qDebug("Saving aln file %s\n",qPrintable(fileName));
	if (fileName.isEmpty()) return;
	
	MeshDocument &meshDoc=GLA()->meshDoc;
	vector<string> meshNameVector;
	vector<Matrix44f> transfVector;

  foreach(MeshModel * mp, meshDoc.meshList) 
	{
		meshNameVector.push_back(mp->fileName.c_str());
		transfVector.push_back(mp->cm.Tr);
	}
	bool ret= ALNParser::SaveALN(qPrintable(fileName),meshNameVector,transfVector);
	
	if(!ret)
	 QMessageBox::critical(this, tr("Meshlab Saving Error"), QString("Unable to save project file %1\n").arg(fileName));

}

bool MainWindow::openProject(QString fileName, GLArea *gla)
{
	bool openRes=true;
	if (fileName.isEmpty())
	    fileName = QFileDialog::getOpenFileName(this,tr("Open Project File"),".", "*.aln");
	if (fileName.isEmpty()) return false;
	vector<RangeMap> rmv;
	
	ALNParser::ParseALN(rmv,qPrintable(fileName));
	// this change of dir is needed for subsequent textures/materials loading
	QFileInfo fi(fileName);
	QDir::setCurrent(fi.absoluteDir().absolutePath());

	vector<RangeMap>::iterator ir;
	for(ir=rmv.begin();ir!=rmv.end() && openRes;++ir)
	{
		if(ir==rmv.begin()) openRes = open((*ir).filename.c_str());
		else								openRes = open((*ir).filename.c_str(),GLA());
		
		GLA()->mm()->cm.Tr=(*ir).trasformation;
	}
	return true;
}

bool MainWindow::open(QString fileName, GLArea *gla)
{
	// Opening files in a transparent form (IO plugins contribution is hidden to user)
	QStringList filters;
	
	// HashTable storing all supported formats togheter with
	// the (1-based) index  of first plugin which is able to open it
	QHash<QString, int> allKnownFormats;
	
	LoadKnownFilters(meshIOPlugins, filters, allKnownFormats,IMPORT);
	filters.push_back("ALN project ( *.aln)");
	filters.front().chop(1);
	filters.front().append(" *.aln)");
	QStringList fileNameList;
	if (fileName.isEmpty())
		fileNameList = QFileDialog::getOpenFileNames(this,tr("Open File"),".", filters.join("\n"));
	else fileNameList.push_back(fileName);
	if (fileNameList.isEmpty())	return false;
	
  foreach(fileName,fileNameList)
	{
			QFileInfo fi(fileName);
			if(fi.suffix().toLower()=="aln") openProject(fileName, NULL);
			else
			{					
				if(!fi.exists()) 	{	
					QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 does not exist.";
					QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
					return false;
				}
				if(!fi.isReadable()) 	{	
					QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 is not readable.";
					QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
					return false;
				}

				// this change of dir is needed for subsequent textures/materials loading
				QDir::setCurrent(fi.absoluteDir().absolutePath());
				
				QString extension = fi.suffix();
				
				// retrieving corresponding IO plugin
				int idx = allKnownFormats[extension.toLower()];
				if (idx == 0)
				{	
					QString errorMsgFormat = "Error encountered while opening file:\n\"%1\"\n\nError details: The \"%2\" file extension does not correspond to any supported format.";
					QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName, extension));
					return false;
				}
				MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx-1];
				
				qb->show();
				int mask = 0;
				MeshModel *mm= new MeshModel();	
				if (!pCurrentIOPlugin->open(extension, fileName, *mm ,mask,QCallBack,this /*gla*/))
					delete mm;
				else{
					if(gla==0) gla=new GLArea(workspace);
					gla->meshDoc.addMesh(mm);
					gla->mm()->ioMask |= mask;				// store mask into model structure
					
					gla->setFileName(fileName);
					workspace->addWindow(gla);
					if(workspace->isVisible()) gla->showMaximized();
					setCurrentFile(fileName);
					
					if( mask & vcg::tri::io::Mask::IOM_FACECOLOR)
						gla->setColorMode(GLW::CMPerFace);
					if( mask & vcg::tri::io::Mask::IOM_VERTCOLOR)
					{
						gla->mm()->storeVertexColor();
						gla->setColorMode(GLW::CMPerVert);
					}
					renderModeTextureAct->setChecked(false);
					renderModeTextureAct->setEnabled(false);
					if(!GLA()->mm()->cm.textures.empty())
					{
						renderModeTextureAct->setChecked(true);
						renderModeTextureAct->setEnabled(true);
						GLA()->setTextureMode(GLW::TMPerWedgeMulti);
					}
					if( mask & vcg::tri::io::Mask::IOM_VERTNORMAL)
								vcg::tri::UpdateNormals<CMeshO>::PerFace(mm->cm);																																			 
					else 
								vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(mm->cm);																																			 
					vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);					// updates bounding box
					if(gla->mm()->cm.fn==0){
						gla->setDrawMode(vcg::GLW::DMPoints);
						gla->setLight(false);
					}
					updateMenus();
					int delVertNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateVertex(mm->cm);
					int delFaceNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(mm->cm);
					
					if(delVertNum>0 || delFaceNum>0 ) 
						QMessageBox::warning(this, "MeshLab Warning", QString("Warning mesh contains %1 vertices with NAN coords and %2 degenerated faces.\nCorrected.").arg(delVertNum).arg(delFaceNum) );
					GLA()->mm()->busy=false;
				}
			}
	}// end foreach file of the input list
	qb->reset();
	return true;
}

void MainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)	open(action->data().toString());
}

void MainWindow::reload()
{
	// Discards changes and reloads current file 
	// save current file name
	QString file = GLA()->getFileName();

	// close current window
	workspace->closeActiveWindow();

	// open a new window with old file
	open(file);
}


bool MainWindow::saveAs()
{	
	QStringList filters;
	
	QHash<QString, int> allKnownFormats;
	
	LoadKnownFilters(meshIOPlugins, filters, allKnownFormats,EXPORT);

	QString fileName;

	if (fileName.isEmpty())
		fileName = QFileDialog::getSaveFileName(this,tr("Save File"),".", filters.join("\n"));
	
	bool ret = false;

	QStringList fs = fileName.split(".");
	
	if(!fileName.isEmpty() && fs.size() < 2)
	{
		QMessageBox::warning(new QWidget(),"Save Error","You must specify file extension!!");
		return ret;
	}

	if (!fileName.isEmpty())
	{
		QString extension = fileName;
		extension.remove(0, fileName.lastIndexOf('.')+1);
	
		QStringListIterator itFilter(filters);

		int idx = allKnownFormats[extension.toLower()];
		if (idx == 0)
		{
			QMessageBox::warning(this, "Unknown type", "File extension not supported!");
			return false;
		}
		MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx-1];
		
		int capability=0,defaultBits=0;
		pCurrentIOPlugin->GetExportMaskCapability(extension,capability,defaultBits);
		
		int mask = vcg::tri::io::SaveMaskToExporter::GetMaskToExporter(this->GLA()->mm(), capability,defaultBits);
		if(mask == -1) 
			return false;
			
		qApp->setOverrideCursor(QCursor(Qt::WaitCursor));	  	
		qb->show();
		ret = pCurrentIOPlugin->save(extension, fileName, *this->GLA()->mm() ,mask,QCallBack,this);
		qb->reset();
		qApp->restoreOverrideCursor();	

    QSettings settings;
    int savedMeshCounter=settings.value("savedMeshCounter",0).toInt();
    settings.setValue("savedMeshCounter",savedMeshCounter+1);
	}	
  GLA()->setWindowModified(false);
	return ret;
}

bool MainWindow::saveSnapshot()
{

	SaveSnapshotDialog dialog(this);
	
	SnapshotSetting ss = GLA()->getSnapshotSetting();
	dialog.setValues(ss);

	if (dialog.exec()==QDialog::Accepted) 
	{
		ss=dialog.getValues();
		GLA()->setSnapshotSetting(ss);
		GLA()->saveSnapshot();
		return true;
	}

	return false;
}
void MainWindow::about()
{
	QDialog *about_dialog = new QDialog();
	Ui::aboutDialog temp;
	temp.setupUi(about_dialog);
	temp.labelMLName->setText(appName()+"   ("+__DATE__+")");
	//about_dialog->setFixedSize(566,580);
	about_dialog->show();
}

void MainWindow::aboutPlugins()
{
	PluginDialog dialog(pluginsDir.path(), pluginFileNames, this);
	dialog.exec();
}

void MainWindow::helpOnscreen()
{
if(GLA()) GLA()->toggleHelpVisible();
}

void MainWindow::helpOnline()
{
  checkForUpdates(false);
	QDesktopServices::openUrl(QUrl("http://meshlab.sourceforge.net/wiki"));
}


void MainWindow::showToolbarFile(){
		mainToolBar->setVisible(!mainToolBar->isVisible());
}

void MainWindow::showToolbarRender(){
	renderToolBar->setVisible(!renderToolBar->isVisible());
}

void MainWindow::showInfoPane()  {if(GLA() != 0)	GLA()->infoAreaVisible =!GLA()->infoAreaVisible;}
void MainWindow::showTrackBall() {if(GLA() != 0) 	GLA()->showTrackBall(!GLA()->isTrackBallVisible());}
void MainWindow::resetTrackBall(){if(GLA() != 0)	GLA()->resetTrackBall();}
void MainWindow::showLayerDlg() {if(GLA() != 0) 	GLA()->layerDialog->setVisible( !GLA()->layerDialog->isVisible() );}

void MainWindow::setCustomize()
{
	CustomDialog dialog(this);
	ColorSetting cs=GLA()->getCustomSetting();
	dialog.loadCurrentSetting(cs.bColorBottom,cs.bColorTop,cs.lColor,GLA()->getLogLevel());
	if (dialog.exec()==QDialog::Accepted) 
	{
		// If press Ok set the selected colors in glArea
		cs.bColorBottom=dialog.getBkgBottomColor();
		cs.bColorTop=dialog.getBkgTopColor();
		cs.lColor=dialog.getLogAreaColor();
    GLA()->setCustomSetting(cs);
		GLA()->setLogLevel(dialog.getLogLevel());
	}	
}

void MainWindow::renderBbox()        { GLA()->setDrawMode(GLW::DMBox     ); }
void MainWindow::renderPoint()       { GLA()->setDrawMode(GLW::DMPoints  ); }
void MainWindow::renderWire()        { GLA()->setDrawMode(GLW::DMWire    ); }
void MainWindow::renderFlat()        { GLA()->setDrawMode(GLW::DMFlat    ); }
void MainWindow::renderFlatLine()    { GLA()->setDrawMode(GLW::DMFlatWire); }
void MainWindow::renderHiddenLines() { GLA()->setDrawMode(GLW::DMHidden  ); }
void MainWindow::renderSmooth()      { GLA()->setDrawMode(GLW::DMSmooth  ); }
void MainWindow::renderTexture()
{
	QAction *a = qobject_cast<QAction* >(sender());
	GLA()->setTextureMode(!a->isChecked() ? GLW::TMNone : GLW::TMPerWedgeMulti);	
}


void MainWindow::fullScreen(){
  if(!isFullScreen())
  {
	  toolbarState = saveState();
	  menuBar()->hide();
	  mainToolBar->hide();
	  renderToolBar->hide();
    globalStatusBar()->hide();
	  setWindowState(windowState()^Qt::WindowFullScreen);
	  bool found=true;
	  //Caso di piu' finestre aperte in tile:
	  if((workspace->windowList()).size()>1){
		  foreach(QWidget *w,workspace->windowList()){if(w->isMaximized()) found=false;}
		  if (found)workspace->tile();
	  }
  }
  else
  {
    menuBar()->show();
		restoreState(toolbarState);
    globalStatusBar()->show();

		setWindowState(windowState()^ Qt::WindowFullScreen);
		bool found=true;
		//Caso di piu' finestre aperte in tile:
		if((workspace->windowList()).size()>1){
			foreach(QWidget *w,workspace->windowList()){if(w->isMaximized()) found=false;}
			if (found){workspace->tile();}
		}
		fullScreenAct->setChecked(false);
  }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
  if(e->key()==Qt::Key_Return && e->modifiers()==Qt::AltModifier)
  {
    fullScreen();
    e->accept();
  }
  else e->ignore();
}
