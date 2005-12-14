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
Revision 1.42  2005/12/14 00:25:51  cignoni
completed multiple texture support

Revision 1.41  2005/12/14 00:11:32  fmazzant
update method SaveAs  for a using generic (not still optimized)

Revision 1.40  2005/12/13 14:31:51  alemochi
Changed names of member functions.

Revision 1.39  2005/12/13 00:31:23  davide_portelli
Cleaned commented code, and dummy code.

Revision 1.38  2005/12/12 17:57:39  buzzelli
"All Files" filter replaced with "All known formats" filter

Revision 1.37  2005/12/12 11:31:15  cignoni
removed an unuseful update of the progress dialog

Revision 1.36  2005/12/12 00:31:22  buzzelli
applyImportExport method removed (not used anymore)

Revision 1.35  2005/12/12 00:03:05  buzzelli
now open method provides also a generic "All Files" filter

Revision 1.34  2005/12/10 06:09:56  davide_portelli
A little change

Revision 1.33  2005/12/09 18:16:12  fmazzant
added generic obj save with plugin arch.

Revision 1.31  2005/12/09 10:43:04  fmazzant
added tools -> set mask obj file

Revision 1.30  2005/12/09 03:50:40  davide_portelli
A little change

Revision 1.29  2005/12/09 03:05:49  davide_portelli
A little change in fullScreen and KeyPressEvent

Revision 1.28  2005/12/09 00:26:25  buzzelli
io importing mechanism adapted in order to be fully transparent towards the user

Revision 1.27  2005/12/07 08:01:09  fmazzant
exporter obj temporany

Revision 1.26  2005/12/07 00:56:40  fmazzant
added support for exporter generic obj file (level base)

Revision 1.25  2005/12/06 16:27:43  fmazzant
added obj file in generic open dialog

Revision 1.24  2005/12/06 10:42:03  vannini
Snapshot dialog now works

Revision 1.23  2005/12/05 18:15:27  vannini
Added snapshot save dialog (not used yet)

Revision 1.22  2005/12/05 18:09:42  ggangemi
Added MeshRenderInterface and shader support

Revision 1.21  2005/12/05 12:18:58  ggangemi
Added support for MeshDecorateInterface Plugins

Revision 1.20  2005/12/05 11:38:39  ggangemi
workaround: added RenderMode parameter to MeshColorizePlugin::compute

Revision 1.19  2005/12/05 10:27:39  vannini
Snapshot in png format instead of ppm

Revision 1.18  2005/12/04 17:47:18  davide_portelli
Added menu windows->Next and Shortcut "CTRL+PageDown"
Added reset trackbal Shortcut "CTRL+H"
Optimize fullscreen

Revision 1.17  2005/12/04 16:51:57  glvertex
Changed some action accelerator keys
Renamed preference menu and dialog

Revision 1.16  2005/12/04 14:45:30  glvertex
gla now is a local variable used only if needed
texture button now works properly

Revision 1.15  2005/12/04 11:49:40  glvertex
solved some little bugs
now texture button works (not always correct: TO FIX)

Revision 1.14  2005/12/04 02:44:39  davide_portelli
Added texture icon in toolbar

Revision 1.13  2005/12/04 00:22:46  cignoni
Switched from progresBar widget to progressbar dialog

Revision 1.12  2005/12/03 23:53:27  cignoni
Re added filter and io plugins

Revision 1.11  2005/12/03 23:40:31  davide_portelli
Added FullScreen menu and TrackBall->Reset trackBall

Revision 1.10  2005/12/03 23:25:40  ggangemi
re-added meshcolorizeplugin support

Revision 1.9  2005/12/03 21:29:34  davide_portelli
Correct bug

Revision 1.8  2005/12/03 19:05:39  davide_portelli
Added About menu.

Revision 1.7  2005/12/03 17:04:34  glvertex
Added backface culling action and slots
Added shortcuts for fancy and double lighting

Revision 1.6  2005/12/03 16:07:14  glvertex
Added samples for core-plugin calls

Revision 1.5  2005/12/02 17:39:07  glvertex
modified plugin import code. old plugins have been disabled cause of new interface.

Revision 1.4  2005/12/02 15:18:38  glvertex
added checks before setting visible some features

Revision 1.3  2005/12/02 11:57:59  glvertex
- show log
- show info area
- show trackball
- some renaming

Revision 1.2  2005/12/02 00:53:18  cignoni
Added a change of dir of the app for the subsequent texture loading.

Revision 1.1  2005/12/01 02:24:50  davide_portelli
Mainwindow Splitted----->[ mainwindow_Init.cpp ]&&[ mainwindow_RunTime.cpp ]

Revision 1.46  2005/11/30 16:26:56  cignoni
All the modification, restructuring seen during the 30/12 lesson...

Revision 1.45  2005/11/29 18:32:56  alemochi
Added customize menu to change colors of environment

Revision 1.44  2005/11/29 11:22:23  vannini
Added experimental snapshot saving function

Revision 1.43  2005/11/28 21:05:37  alemochi
Added menu preferences and configurable background

Revision 1.42  2005/11/28 01:06:04  davide_portelli
Now GLA contains a list of RenderMode, instead of a single RenderMode.
Thus it is possible to have more active RenderMode (MeshRenderInterface)
at the same time, and when there are many opened windows, the menù of rendering
is consisting.

Revision 1.41  2005/11/27 18:36:58  buzzelli
changed applyImportExport method in order to handle correctly the case of no opened subwindows

Revision 1.40  2005/11/27 04:09:53  glvertex
- Added full support for import/export plugins
- Added ViewLog Action and Slot (not working as well)
- Minor changes and clean up

Revision 1.39  2005/11/26 17:05:13  glvertex
Optimized SetLight method
Now using Log constants instead of int values in logging lines

Revision 1.37  2005/11/26 14:09:15  alemochi
Added double side lighting and fancy lighting (working only double side+fancy)

Revision 1.36  2005/11/25 18:15:10  ggangemi
Moved MeshColorizeInterface menu entries under "Color" sub-menu

Revision 1.35  2005/11/25 17:41:52  alemochi
Added categorization to render menu

Revision 1.34  2005/11/25 16:23:02  ggangemi
Added MeshColorizeInterface plugins support code

Revision 1.33  2005/11/25 16:02:28  davide_portelli
Added consistency of toolbar and menu with openGL

Revision 1.30  2005/11/25 12:58:58  alemochi
Added new icon that represent the changing of lighting state and modified function.

Revision 1.29  2005/11/25 11:55:59  alemochi
Added function to Enable/Disable lighting (work in progress)

Revision 1.28  2005/11/25 02:47:18  davide_portelli
Some cleanup

Revision 1.27  2005/11/24 15:46:57  davide_portelli
Added the check icon for menu Render->Show Normals

Revision 1.26  2005/11/24 14:59:31  davide_portelli
Correct a little bug in menu filter

Revision 1.25  2005/11/24 09:48:37  cignoni
changed invocation of applyfilter

Revision 1.24  2005/11/24 01:38:36  cignoni
Added new plugins intefaces, tested with shownormal render mode

Revision 1.23  2005/11/23 00:25:06  glvertex
Reverted plugin interface to prev version

Revision 1.22  2005/11/23 00:03:10  cignoni
Changed names view->render, added renderModeGroup to implement radio styles menu entries

Revision 1.21  2005/11/22 17:10:53  glvertex
MeshFilter Plugin STRONGLY reviewed and changed

Revision 1.20  2005/11/22 01:22:45  davide_portelli
Added the render menu.
I have rename the following variable:
viewModePoints      <====>    viewModePointsAct
viewModeWire        <====>    viewModeWireAct
viewModeLines       <====>    viewModeHiddenLinesAct
viewModeFlatLines  <====>    viewModeFlatLinesAct
viewModeFlat          <====>    viewModeFlatAct
viewModeSmooth    <====>    viewModeSmoothAct

Revision 1.19  2005/11/22 00:03:21  davide_portelli
Correct a little bug in menu filter

Revision 1.18  2005/11/21 22:08:25  cignoni
added a cast to a 8bit char from unicode using locale

Revision 1.17  2005/11/20 19:33:09  glvertex
Logging filters events (still working on...)

Revision 1.16  2005/11/20 18:54:33  davide_portelli
Improved menu behavior

Revision 1.15  2005/11/20 17:57:26  davide_portelli
Modification the menu:
- Menu disable when not there is active windows
- Menu enable when there is active windows

Revision 1.14  2005/11/20 04:34:34  davide_portelli
Adding in the file menù, the list of the last open file (Recent File).

Revision 1.13  2005/11/19 18:15:20  glvertex
- Some bug removed.
- Interface more friendly.
- Background.

Revision 1.12  2005/11/19 12:14:20  glvertex
Some cleanup and renaming

Revision 1.11  2005/11/19 04:59:12  davide_portelli
I have added the modifications to the menù view and to the menu windows:
- View->Toolbar->File ToolBar
- View->Toolbar->Render ToolBar
- windows->Close
- windows->Close All
- windows->and the list of the open windows

Revision 1.10  2005/11/18 18:25:35  alemochi
Rename function in glArea.h

Revision 1.9  2005/11/18 18:10:28  alemochi
Aggiunto slot cambiare la modalita' di rendering

Revision 1.8  2005/11/18 02:12:04  glvertex
- Added icons to the project file [meshlab.qrc]
- Enabled renderToolbar with icons

Revision 1.7  2005/11/18 00:36:50  davide_portelli
Added View->Toolbar and Windows->Tile and Windows->Cascade

Revision 1.6  2005/11/17 22:15:52  davide_portelli
Added menu View->Tile

Revision 1.5  2005/11/17 20:51:00  davide_portelli
Added Help->About Plugins

Revision 1.4  2005/11/16 23:18:54  cignoni
Added plugins management

Revision 1.3  2005/10/31 17:17:47  cignoni
Sketched the interface  of the odd/even refine function

Revision 1.2  2005/10/22 10:23:28  mariolatronico
on meshlab.pro added a lib dependency , on other files modified #include directive to allow compilation on case sensitive o.s.

Revision 1.1  2005/10/18 10:38:02  cignoni
First rough version. It simply load a mesh.

****************************************************************************/


#include <QtGui>
#include <QToolBar>
#include <QProgressBar>


#include "meshmodel.h"
#include "interfaces.h"
#include "mainwindow.h"
#include "glarea.h"
#include "plugindialog.h"
#include "customDialog.h"		
#include "saveSnapshotDialog.h"
#include "ui_aboutForm.h"



void MainWindow::updateRecentFileActions()
{
	QSettings settings("Recent Files");
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

void MainWindow::updateWindowMenu()
{
	windowsMenu->clear();
	windowsMenu->addAction(closeAct);
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
void MainWindow::updateMenus()
{
	bool active = (bool)workspace->activeWindow();
	saveAsAct->setEnabled(active);
	saveSnapshotAct->setEnabled(active);
	filterMenu->setEnabled(active && !filterMenu->actions().isEmpty());
	renderMenu->setEnabled(active);
	fullScreenAct->setEnabled(active);
	trackBallMenu->setEnabled(active);
	logMenu->setEnabled(active);
  windowsMenu->setEnabled(active);
	preferencesMenu->setEnabled(active);
	renderToolBar->setEnabled(active);
	if(active){
		const RenderMode &rm=GLA()->getCurrentRenderMode();
		switch (rm.drawMode) {
			case GLW::DMBox:
				renderBboxAct->setChecked(true);
				break;
			case GLW::DMPoints:
				renderModePointsAct->setChecked(true);
				break;
			case GLW::DMWire:
				renderModeWireAct->setChecked(true);
				break;
			case GLW::DMFlat:
				renderModeFlatAct->setChecked(true);
				break;
			case GLW::DMSmooth:
				renderModeSmoothAct->setChecked(true);
				break;
			case GLW::DMFlatWire:
				renderModeFlatLinesAct->setChecked(true);
				break;
			case GLW::DMHidden:
				renderModeHiddenLinesAct->setChecked(true);
				break;
		}

		showLogAct->setChecked(GLA()->isLogVisible());
		showInfoPaneAct->setChecked(GLA()->isInfoAreaVisible());
		showTrackBallAct->setChecked(GLA()->isTrackBallVisible());
		backFaceCullAct->setChecked(GLA()->getCurrentRenderMode().backFaceCull);
		renderModeTextureAct->setEnabled(!GLA()->mm->cm.textures.empty());
		renderModeTextureAct->setChecked(GLA()->getCurrentRenderMode().textureMode != GLW::TMNone);
		
		setLightAct->setIcon(rm.lighting ? QIcon(":/images/lighton.png") : QIcon(":/images/lightoff.png") );
		setLightAct->setChecked(rm.lighting);

		setFancyLightingAct->setChecked(rm.fancyLighting);
		setDoubleLightingAct->setChecked(rm.doubleSideLighting);

		foreach (QAction *a,TotalRenderList){a->setChecked(false);}
		if(GLA()->iDecoratorsList){
			pair<QAction *,MeshDecorateInterface *> p;
			foreach (p,*GLA()->iDecoratorsList){p.first->setChecked(true);}
		}
	}
}

void MainWindow::applyFilter()
{
	QAction *action = qobject_cast<QAction *>(sender());
	MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());
	qb->show();
	iFilter->applyFilter(action,*(GLA()->mm ),this,QCallBack);
	GLA()->log.Log(GLLogStream::Info,"Applied filter %s",action->text().toLocal8Bit().constData());
  qb->hide();
}


void MainWindow::applyRenderMode()
{
	QAction *action = qobject_cast<QAction *>(sender());		// find the action which has sent the signal 
	// Make the call to the plugin core
	MeshRenderInterface *iRenderTemp = qobject_cast<MeshRenderInterface *>(action->parent());
	iRenderTemp->Init(action,*(GLA()->mm),GLA());

	if(iRenderTemp->isSupported()) {
		GLA()->setRender(iRenderTemp);
	} else {
		GLA()->setRender(0); //vertex and fragment programs not supported
	}
	GLA()->log.Log(GLLogStream::Info,"%s",action->text().toLocal8Bit().constData());	// Prints out action name
}


void MainWindow::applyColorMode()
{
	QAction *action = qobject_cast<QAction *>(sender());
	MeshColorizeInterface *iColorTemp = qobject_cast<MeshColorizeInterface *>(action->parent());
  iColorTemp->Compute(action,*(GLA()->mm ),GLA()->getCurrentRenderMode(), GLA());
	if (action->isChecked()) {
		action->setChecked(true);
    GLA()->log.Log(GLLogStream::Info,"Applied colorize %s",action->text().toLocal8Bit().constData());
	} else {
		action->setChecked(false);
		GLA()->log.Log(GLLogStream::Info,"Turning off colorize %s",action->text().toLocal8Bit().constData());
	}
}

void MainWindow::applyDecorateMode()
{
	QAction *action = qobject_cast<QAction *>(sender());		// find the action which has sent the signal 

	MeshDecorateInterface *iDecorateTemp = qobject_cast<MeshDecorateInterface *>(action->parent());
	if(GLA()->iDecoratorsList==0){
		GLA()->iDecoratorsList= new list<pair<QAction *,MeshDecorateInterface *> >;
		GLA()->iDecoratorsList->push_back(make_pair(action,iDecorateTemp));
		GLA()->log.Log(GLLogStream::Info,"Enable Decorate mode %s",action->text().toLocal8Bit().constData());
	}else{
		bool found=false;
		pair<QAction *,MeshDecorateInterface *> p;
		foreach(p,*GLA()->iDecoratorsList){
			if(iDecorateTemp==p.second && p.first->text()==action->text()){
				GLA()->iDecoratorsList->remove(p);
				GLA()->log.Log(0,"Disabled Decorate mode %s",action->text().toLocal8Bit().constData());
				found=true;
			} 
		}
		if(!found){
			GLA()->iDecoratorsList->push_back(make_pair(action,iDecorateTemp));
			GLA()->log.Log(GLLogStream::Info,"Enable Decorate mode %s",action->text().toLocal8Bit().constData());
		}
	}
}

bool MainWindow::QCallBack(const int pos, const char * str)
{
	if(qb==0) return true;
	qb->setWindowTitle (str);
	qb->setValue(pos);
	return true;
}

void MainWindow::setLight()			     
{
	GLA()->setLight(!GLA()->getCurrentRenderMode().lighting);
	updateMenus();
};


void MainWindow::setDoubleLighting()
{
	const RenderMode &rm=GLA()->getCurrentRenderMode();
	if (rm.doubleSideLighting) GLA()->setLightMode(false,LDOUBLE);
	else GLA()->setLightMode(true,LDOUBLE);
}

void MainWindow::setFancyLighting()
{
	const RenderMode &rm=GLA()->getCurrentRenderMode();
	if (rm.fancyLighting) GLA()->setLightMode(false,LFANCY);
	else GLA()->setLightMode(true,LFANCY);
}

void MainWindow::toggleBackFaceCulling()
{
	RenderMode &rm = GLA()->getCurrentRenderMode();

	GLA()->setBackFaceCulling(!rm.backFaceCull);
}

void MainWindow::open(QString fileName)
{
	// Opening files in a transparent form (IO plugins contribution is hidden to user)
	const QString defaultFilter					= tr("Mesh files (*.ply *.off *.stl)");
	QStringList filters;
	
	filters	<< defaultFilter;
	
	// HashTable storing all supported formats, preserving for each
	// of them, the index of first plugin which is able to open it
	QHash<QString, int> allKnownFormats;
	
	QString selectedFilter;  // this will be filled with actual selected filter
	std::vector<MeshIOInterface*>::iterator itIOPlugin = meshIOPlugins.begin();
	for (int i = 0; itIOPlugin != meshIOPlugins.end(); ++itIOPlugin, ++i)  // cycle among loaded IO plugins
	{
		MeshIOInterface* pMeshIOPlugin = *itIOPlugin;

		QString currentDescription;
		QStringList currentFormats = pMeshIOPlugin->formats(currentDescription);

		QString currentFilterEntry = currentDescription + " (*.";
		QStringListIterator itFormat(currentFormats);
		if (itFormat.hasNext())
		{
			QString currentFormat = itFormat.next().toLower();
			if (!allKnownFormats.contains(currentFormat))
				allKnownFormats.insert(currentFormat, i);
			currentFilterEntry.append(currentFormat);
		}
		while (itFormat.hasNext())
		{
			QString currentFormat = itFormat.next().toLower();
			if (!allKnownFormats.contains(currentFormat))
				allKnownFormats.insert(currentFormat, i);
			currentFilterEntry.append(tr(" *."));
			currentFilterEntry.append(currentFormat);
		}
		currentFilterEntry.append(')');
		
		filters.append(currentFilterEntry);
	}
	QString allKnownFormatsFilter = tr("All known formats (*.ply"); 
	QHash<QString, int>::iterator itHash = allKnownFormats.begin();
	for(; itHash != allKnownFormats.end(); ++itHash)
	{
		allKnownFormatsFilter.append(tr(" *."));
		allKnownFormatsFilter.append(itHash.key());
	}
	allKnownFormatsFilter.append(')');
	filters << allKnownFormatsFilter;

	if (fileName.isEmpty())
		fileName = QFileDialog::getOpenFileName(this,tr("Open File"),".", filters.join("\n"), &selectedFilter);
	else selectedFilter = allKnownFormatsFilter;

	if (fileName.isEmpty())
		return;

	// this change of dir is needed for subsequent texture loading
	QString fileNameDir = fileName.left(fileName.lastIndexOf("/")); 
	QDir::setCurrent(fileNameDir);

	
	MeshModel *mm= new MeshModel();	
	qb->show();

	QString extension = fileName;
	extension.remove(0, fileName.lastIndexOf('.')+1);
	
	bool success = false;
	if (extension.toUpper() == tr("PLY"))	// default format
		success = mm->Open(fileName.toAscii(),QCallBack);
	else																	// additional formats supported via plugin
	{
		// an alternative solution could delegate the opening stuff to the first plugin
		// in the vector which is able to treats that format, in current implementation
		// I use the plugin corresponding to the respective item in the combo
		QStringListIterator itFilter(filters);
		int idx = 0;
		while (itFilter.hasNext() && (itFilter.next() != selectedFilter))
			++idx;
		--idx;  // subtracting 1 since first filter was the default one
		if ((idx > -1) && (idx <= (int)meshIOPlugins.size()))
		{
			if (idx == (int)meshIOPlugins.size())  // "All files" filter was selected
			{
				QString lowerCaseExt = extension.toLower();
				if (allKnownFormats.contains(lowerCaseExt))
					idx = allKnownFormats[lowerCaseExt];
				else
				{
					QMessageBox::warning(this, tr("Open File"), tr("Unknown file format"));
					delete mm;
					qb->hide();
					return;
				}
			}
			MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx];
			int mask = -1;
			success = pCurrentIOPlugin->open(extension, fileName, *mm ,mask,QCallBack,this /*gla?*/);
		}
	}

	if (!success)
	{
		QMessageBox::warning(this, tr("Error"),tr("Cannot load %1.").arg(fileName));
    delete mm;
	}
	else{
		GLArea *gla;
		gla=new GLArea(workspace);
		gla->mm=mm;
		gla->setWindowTitle(QFileInfo(fileName).fileName());
		gla->showInfoArea(true);
		workspace->addWindow(gla);
		if(workspace->isVisible()) gla->showMaximized();
		setCurrentFile(fileName);
		renderModeTextureAct->setChecked(false);
		renderModeTextureAct->setEnabled(false);
		if(!GLA()->mm->cm.textures.empty())
		{
			renderModeTextureAct->setChecked(true);
			renderModeTextureAct->setEnabled(true);
			GLA()->setTextureMode(GLW::TMPerWedgeMulti);
		}
	}
	qb->hide();
}

void MainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)	open(action->data().toString());
}

bool MainWindow::saveAs()
{	
	QStringList filters;
	QString selectedFilter;

	//************************************** mettere in una funzione ****************************
	std::vector<MeshIOInterface*>::iterator itIOPlugin = meshIOPlugins.begin();
	for (; itIOPlugin != meshIOPlugins.end(); ++itIOPlugin)  // cycle among loaded IO plugins
	{
		MeshIOInterface* pMeshIOPlugin = *itIOPlugin;

		QString currentDescription;
		QStringList currentFormats = pMeshIOPlugin->formats(currentDescription);
		QString currentFilterEntry;
		QStringListIterator itFormat(currentFormats);
		QString alltype = "All Mesh Files Known (";

		while (itFormat.hasNext())
		{
			currentFilterEntry = "Mesh Files (*.";
			QString ex = itFormat.next().toLower();
			currentFilterEntry.append(ex);
			alltype.append("*.*"); //+ex+" ");//costruisce la stringa per gli All Mesh files support!
			currentFilterEntry.append(")");
			filters.append(currentFilterEntry);
		}
		
		filters << tr("Mesh files (*.ply *.off *.stl)");

		alltype.append(")");//aggiunge l'ultima parentesi 
		filters.insert(0,alltype);
	}
	//************************************ fine ***********************************************

	QString fileName;

	if (fileName.isEmpty())
		fileName = QFileDialog::getSaveFileName(this,tr("Save File"),".", filters.join("\n"), &selectedFilter);
	
	bool ret = false;

	if (!fileName.isEmpty())
	{
		QString extension = fileName;
		extension.remove(0, fileName.lastIndexOf('.')+1);
	
		if (extension.toUpper() == tr("PLY"))
		{
			qb->show();
			ret = this->GLA()->mm->Save(fileName.toStdString().c_str(),QCallBack);
			qb->hide();
		}
		else													
		{
			QStringListIterator itFilter(filters);
			int idx = 0;
			while (itFilter.hasNext() && (itFilter.next() != selectedFilter))
				++idx;
			--idx;  // subtracting 1 since first filter was the default one
			if ((idx > -1) && (idx < (int)meshIOPlugins.size()))
			{
				if(extension.toUpper() == tr("OBJ"))
				{
					if(maskobj.isfirst)
					{
						SaveMaskDialog dialog(&maskobj,new QWidget());
						dialog.ReadMask();
						dialog.exec();
					}
				}

				int mask = maskobj.MaskObjToInt();
				MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx];
				qb->show();
				ret = pCurrentIOPlugin->save(extension, fileName, *this->GLA()->mm ,mask,QCallBack,this);
				qb->hide();
				
			}
		}
	}	

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
	about_dialog->show();
}

void MainWindow::aboutPlugins()
{
	PluginDialog dialog(pluginsDir.path(), pluginFileNames, this);
	dialog.exec();
}
void MainWindow::showToolbarFile(){
		mainToolBar->setVisible(!mainToolBar->isVisible());
}

void MainWindow::showToolbarRender(){
	renderToolBar->setVisible(!renderToolBar->isVisible());
}

void MainWindow::showLog()			 {if(GLA() != 0)	GLA()->showLog(!GLA()->isLogVisible());}
void MainWindow::showInfoPane()  {if(GLA() != 0)	GLA()->showInfoArea(!GLA()->isInfoAreaVisible());}
void MainWindow::showTrackBall() {if(GLA() != 0) 	GLA()->showTrackBall(!GLA()->isTrackBallVisible());}
void MainWindow::resetTrackBall(){if(GLA() != 0)	GLA()->resetTrackBall();}
void MainWindow::setCustomize()
{
	CustomDialog dialog(this);
	ColorSetting cs=GLA()->getCustomSetting();
	dialog.loadCurrentSetting(cs.bColorBottom,cs.bColorTop,cs.lColor);
	if (dialog.exec()==QDialog::Accepted) 
	{
		// If press Ok set the selected colors in glArea
		cs.bColorBottom=dialog.getBkgBottomColor();
		cs.bColorTop=dialog.getBkgTopColor();
		cs.lColor=dialog.getLogAreaColor();
    GLA()->setCustomSetting(cs);	
	}	
}

void MainWindow::setSaveMaskObj()
{
	SaveMaskDialog objdialog(&maskobj,new QWidget());
	objdialog.ReadMask();
	objdialog.exec();
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
	toolbarState = saveState();
	menuBar()->hide();
	mainToolBar->hide();
	renderToolBar->hide();
	setWindowState(windowState()^Qt::WindowFullScreen);
	bool found=true;
	//Caso di piu' finestre aperte in tile:
	if((workspace->windowList()).size()>1){
		foreach(QWidget *w,workspace->windowList()){if(w->isMaximized()) found=false;}
		if (found)workspace->tile();
	}
}
void MainWindow::keyPressEvent(QKeyEvent *e){
	if(e->key()==Qt::Key_Escape && isFullScreen()){
		menuBar()->show();
		restoreState(toolbarState);
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