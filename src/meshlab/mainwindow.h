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

class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;
class GLArea;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    
	void open(QString fileName=QString());
    bool saveAs();
    void about();
    void aboutPlugins();
    void applyFilter();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void loadPlugins();
    void addToMenu(QObject *plugin, const QStringList &texts, QMenu *menu,
                   const char *member, QActionGroup *actionGroup = 0);

    QWorkspace *workspace;

    vector<MeshModel *> VM;
    GLArea *paintArea;
    QScrollArea *scrollArea;
    QDir pluginsDir;
    QStringList pluginFileNames;
    QToolBar *mainToolBar;
    QToolBar *renderToolBar;
    
    QMenu *fileMenu;
    QMenu *filterMenu;
    QMenu *helpMenu;
    QAction *openAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
	QAction *aboutPluginsAct;
};

#endif
