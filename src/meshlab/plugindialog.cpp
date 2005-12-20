/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
Revision 1.5  2005/12/20 03:33:16  davide_portelli
Modified PluginDialog.

Revision 1.4  2005/12/04 02:44:39  davide_portelli
Added texture icon in toolbar

Revision 1.3  2005/11/24 01:38:36  cignoni
Added new plugins intefaces, tested with shownormal render mode

Revision 1.2  2005/11/21 12:12:54  cignoni
Added copyright info

****************************************************************************/

#include <QtGui>
#include <QStringList>
#include "meshmodel.h"
#include "interfaces.h"
#include "plugindialog.h"

PluginDialog::PluginDialog(const QString &path, const QStringList &fileNames,QWidget *parent): QDialog(parent)
{
    label = new QLabel;
    label->setWordWrap(true);

    QStringList headerLabels;
    headerLabels << tr("Components");

    treeWidget = new QTreeWidget;
    treeWidget->setAlternatingRowColors(false);
    treeWidget->setHeaderLabels(headerLabels);
    treeWidget->header()->hide();

    okButton = new QPushButton(tr("OK"));
    okButton->setDefault(true);

    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(2, 1);
    mainLayout->addWidget(label, 0, 0, 1, 3);
    mainLayout->addWidget(treeWidget, 1, 0, 1, 3);
    mainLayout->addWidget(okButton, 2, 1);
    setLayout(mainLayout);

    interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),QIcon::Normal, QIcon::On);
    interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),QIcon::Normal, QIcon::Off);
    featureIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    setWindowTitle(tr("Plugin Information"));
    populateTreeWidget(path, fileNames);
}

void PluginDialog::populateTreeWidget(const QString &path,const QStringList &fileNames)
{
    if (fileNames.isEmpty()) {
        label->setText(tr("Can't find any plugins in the %1 " "directory.").arg(QDir::convertSeparators(path)));
        treeWidget->hide();
    } else {
        label->setText(tr("Found the following plugins in the %1 " "directory:").arg(QDir::convertSeparators(path)));
        QDir dir(path);
        foreach (QString fileName, fileNames) {
            QPluginLoader loader(dir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();

            QTreeWidgetItem *pluginItem = new QTreeWidgetItem(treeWidget);
            pluginItem->setText(0, fileName);
						pluginItem->setIcon(0, interfaceIcon);
            treeWidget->setItemExpanded(pluginItem, false);

            QFont boldFont = pluginItem->font(0);
            boldFont.setBold(true);
            pluginItem->setFont(0, boldFont);

            if (plugin) {
                MeshIOInterface *iMeshIO = qobject_cast<MeshIOInterface *>(plugin);
								if (iMeshIO){
									QStringList Templist;
									foreach(const MeshIOInterface::Format f,iMeshIO->formats()){Templist.push_back(f.desctiption);}
                  addItems(pluginItem,Templist);
								}
                MeshDecorateInterface *iDecorate = qobject_cast<MeshDecorateInterface *>(plugin);
								if (iDecorate){
									QStringList Templist;
									foreach(QAction *a,iDecorate->actions()){Templist.push_back(a->text());}
									addItems(pluginItem,Templist);
								}
								MeshColorizeInterface *iColorize = qobject_cast<MeshColorizeInterface *>(plugin);
								if (iColorize){
									QStringList Templist;
									foreach(QAction *a,iColorize->actions()){Templist.push_back(a->text());}
									addItems(pluginItem,Templist);
								}
								MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
								if (iFilter){
									QStringList Templist;
									foreach(QAction *a,iFilter->actions()){Templist.push_back(a->text());}
									addItems(pluginItem,Templist);
								}
								MeshRenderInterface *iRender = qobject_cast<MeshRenderInterface *>(plugin);
								if (iRender){
									QStringList Templist;
									foreach(QAction *a,iRender->actions()){Templist.push_back(a->text());}
									addItems(pluginItem,Templist);
								}
								MeshEditInterface *iEdit = qobject_cast<MeshEditInterface *>(plugin);
								if (iEdit){
									QStringList Templist;
									foreach(QAction *a,iEdit->actions()){Templist.push_back(a->text());}
									addItems(pluginItem,Templist);
								}
           }
        }
    }
}

void PluginDialog::addItems(QTreeWidgetItem *pluginItem,const QStringList &features){

	foreach (QString feature, features) {
		QTreeWidgetItem *featureItem = new QTreeWidgetItem(pluginItem);
    featureItem->setText(0, feature);
    featureItem->setIcon(0, featureIcon);
  }
}