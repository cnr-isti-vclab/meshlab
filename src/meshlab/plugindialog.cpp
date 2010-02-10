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

#include <QtGui>
#include <QStringList>
#include <common/interfaces.h>
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

		groupBox=new QGroupBox(tr("Info Plugin"));
		
    okButton = new QPushButton(tr("OK"));
    okButton->setDefault(true);
		
		spacerItem = new QSpacerItem(363, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		
		labelInfo=new QLabel(groupBox);
		labelInfo->setWordWrap(true);
		//tedit->hide();

    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
		connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(displayInfo(QTreeWidgetItem*,int)));

    QGridLayout *mainLayout = new QGridLayout;
		QHBoxLayout *gboxLayout = new QHBoxLayout(groupBox);
		gboxLayout->addWidget(labelInfo);
    //mainLayout->setColumnStretch(0, 1);
    //mainLayout->setColumnStretch(2, 1);
    mainLayout->addWidget(label, 0, 0, 1, 2);
    mainLayout->addWidget(treeWidget, 1, 0, 4, 2);
    mainLayout->addWidget(groupBox,5,0,1,2);
		mainLayout->addItem(spacerItem, 6, 0, 1, 1);
		mainLayout->addWidget(okButton,6,1,1,1);


		//mainLayout->addWidget(okButton, 3, 1,Qt::AlignHCenter);
		//mainLayout->addLayout(buttonLayout,3,1);			
		setLayout(mainLayout);

    interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),QIcon::Normal, QIcon::On);
    interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),QIcon::Normal, QIcon::Off);
    featureIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    setWindowTitle(tr("Plugin Information"));
    populateTreeWidget(path, fileNames);
		pathDirectory=path;
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
									foreach(const MeshIOInterface::Format f,iMeshIO->importFormats()){
										QString formats;
										foreach(const QString s,f.extensions) formats+="Importer_"+s+" ";
										Templist.push_back(formats);
									}
									foreach(const MeshIOInterface::Format f,iMeshIO->exportFormats()){
										QString formats;
										foreach(const QString s,f.extensions) formats+="Exporter_"+s+" ";
										Templist.push_back(formats);
									}
                  addItems(pluginItem,Templist);
								}
                MeshDecorateInterface *iDecorate = qobject_cast<MeshDecorateInterface *>(plugin);
								if (iDecorate){
									QStringList Templist;
									foreach(QAction *a,iDecorate->actions()){Templist.push_back(a->text());}
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
								MeshEditInterfaceFactory *iEdit = qobject_cast<MeshEditInterfaceFactory *>(plugin);
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


void PluginDialog::displayInfo(QTreeWidgetItem* item,int /* ncolumn*/)
{
	QString parent;
	QString actionName;
  if(item==NULL) return;
	if (item->parent()!=NULL)	{parent=item->parent()->text(0);actionName=item->text(0);}
	else parent=item->text(0);
	QString fileName=pathDirectory+"/"+parent;
	QPluginLoader loader(fileName);
    qDebug("Trying to load the plugin '%s'",qPrintable(fileName));
	QObject *plugin = loader.instance();
	if (plugin) {
		MeshIOInterface *iMeshIO = qobject_cast<MeshIOInterface *>(plugin);
		if (iMeshIO){
			foreach(const MeshIOInterface::Format f,iMeshIO->importFormats()){
				QString formats;
				foreach(const QString s,f.extensions) formats+="Importer_"+s+" ";
				if (actionName==formats) labelInfo->setText(f.description);
			}
			foreach(const MeshIOInterface::Format f,iMeshIO->exportFormats()){
				QString formats;
				foreach(const QString s,f.extensions) formats+="Exporter_"+s+" ";
				if (actionName==formats) labelInfo->setText(f.description);
			}
		}
		MeshDecorateInterface *iDecorate = qobject_cast<MeshDecorateInterface *>(plugin);
		if (iDecorate)
		{
			foreach(QAction *a,iDecorate->actions())
				if (actionName==a->text()) labelInfo->setText(iDecorate->filterInfo(a));
		}
		MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
		if (iFilter)
		{
			foreach(QAction *a,iFilter->actions())
							if (actionName==a->text()) labelInfo->setText(iFilter->filterInfo(iFilter->ID(a)));
		}
		MeshRenderInterface *iRender = qobject_cast<MeshRenderInterface *>(plugin);
		if (iRender){
		}
		MeshEditInterfaceFactory *iEditFactory = qobject_cast<MeshEditInterfaceFactory *>(plugin);
		if (iEditFactory)
		{
			foreach(QAction *a, iEditFactory->actions())
			{
				if(iEditFactory) labelInfo->setText(iEditFactory->getEditToolDescription(a));
			}
		}
	}
}	
