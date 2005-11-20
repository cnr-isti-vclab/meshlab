/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QWorkspace>
#include <QStringList>
#include "meshmodel.h"

#define MAXRECENTFILES 4

class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;
class GLArea;
class QSignalMapper;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

private slots:

	void open(QString fileName=QString());
	void openRecentFile();							
	bool saveAs();
	void about();
	void aboutPlugins();
	void applyFilter();
	void windowsTile();
	void windowsCascade();
	void updateWindowMenu();
	void viewToolbarFile();
	void viewToolbarRender();
	void RenderPoint();
	void RenderWire();
	void RenderFlat();
	void RenderFlatLine();
	void RenderHiddenLines();
	void RenderSmooth();
	

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void loadPlugins();
	void updateRecentFileActions();				
	void setCurrentFile(const QString &fileName);			
	void addToMenu(QObject *plugin, const QStringList &texts, QMenu *menu,
								const char *member, QActionGroup *actionGroup = 0);

	QWorkspace *workspace;
	QSignalMapper *windowMapper;

	GLArea *gla;
	vector<MeshModel *> VM;
	QScrollArea *scrollArea;
	QDir pluginsDir;
	QStringList pluginFileNames;
	
	QToolBar *mainToolBar;
	QToolBar *renderToolBar;

	QMenu *fileMenu;
	QMenu *filterMenu;
	QMenu *viewMenu;
	QMenu *toolBarMenu;
	QMenu *windowsMenu;
	QMenu *helpMenu;
	
	QAction *openAct;
	QAction *saveAsAct;
	
	QAction *viewModePoints;
	QAction *viewModeWire;
	QAction *viewModeLines;
	QAction *viewModeFlatLines;
	QAction *viewModeFlat;
	QAction *viewModeSmooth;
	
	QAction *exitAct;
	QAction *aboutAct;
	QAction *aboutQtAct;
	QAction *aboutPluginsAct;
	QAction *viewToolbarStandardAct;
	QAction *viewToolbarRenderAct;
	QAction *windowsTileAct;
	QAction *windowsCascadeAct;
	QAction *closeAct;
	QAction *closeAllAct;
	QAction *recentFileActs[MAXRECENTFILES];		
	QAction *separatorAct;										
};

#endif
