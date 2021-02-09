/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#include "plugin_info_dialog.h"
#include "ui_plugin_info_dialog.h"

#include <QDir>
#include <QPluginLoader>

#include <common/plugins/interfaces/filter_plugin_interface.h>
#include <common/plugins/interfaces/iomesh_plugin_interface.h>
#include <common/plugins/interfaces/decorate_plugin_interface.h>
#include <common/plugins/interfaces/render_plugin_interface.h>
#include <common/plugins/interfaces/edit_plugin_interface.h>
#include <common/globals.h>
#include <common/plugins/plugin_manager.h>
#include <common/plugins/meshlab_plugin_type.h>

PluginInfoDialog::PluginInfoDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PluginInfoDialog)
{
	ui->setupUi(this);
	
	interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),QIcon::Normal, QIcon::On);
	interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),QIcon::Normal, QIcon::Off);
	featureIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));
	uninstallIcon.addPixmap(style()->standardPixmap(QStyle::SP_DialogCancelButton));
	
	populateTreeWidget();
}

PluginInfoDialog::~PluginInfoDialog()
{
	delete ui;
}

void PluginInfoDialog::populateTreeWidget()
{
	ui->treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
	ui->treeWidget->header()->setStretchLastSection(false);
	ui->treeWidget->header()->setSectionResizeMode(PLUGINS, QHeaderView::Stretch);
	PluginManager& pm = meshlab::pluginManagerInstance();
	if (pm.size() == 0){
		ui->label->setText(tr("No plugin has been loaded."));
		ui->treeWidget->hide();
	}
	else {
		for (PluginFileInterface* fp : pm.pluginIterator()){
			MeshLabPluginType type(fp);
			QString pluginType = type.pluginTypeString();
			QStringList tmplist;
			if (type.isDecoratePlugin()){
				DecoratePluginInterface* dpi = dynamic_cast<DecoratePluginInterface*>(fp);
				for(QAction *a: dpi->actions())
					tmplist.push_back(a->text());
			}
			if (type.isEditPlugin()){
				EditPluginInterfaceFactory* epi = dynamic_cast<EditPluginInterfaceFactory*>(fp);
				for(QAction *a: epi->actions())
					tmplist.push_back(a->text());
			}
			if (type.isFilterPlugin()){
				FilterPluginInterface* fpi = dynamic_cast<FilterPluginInterface*>(fp);
				for(QAction *a: fpi->actions())
					tmplist.push_back(a->text());
			}
			if (type.isIOMeshPlugin()){
				IOMeshPluginInterface* iopi = dynamic_cast<IOMeshPluginInterface*>(fp);
				for(const FileFormat& f: iopi->importFormats()){
					QString formats;
					for(const QString& s : f.extensions)
						formats+="Importer_"+s+" ";
					tmplist.push_back(formats);
				}
				for(const FileFormat& f: iopi->exportFormats()){
					QString formats;
					for(const QString& s: f.extensions)
						formats+="Exporter_"+s+" ";
					tmplist.push_back(formats);
				}
			}
			if (type.isIORasterPlugin()){
				IORasterPluginInterface* iorpi = dynamic_cast<IORasterPluginInterface*>(fp);
				for(const FileFormat& f: iorpi->importFormats()){
					QString formats;
					for(const QString& s : f.extensions)
						formats+="Importer_"+s+" ";
					tmplist.push_back(formats);
				}
			}
			if (type.isRenderPlugin()){
				RenderPluginInterface* rpi = dynamic_cast<RenderPluginInterface*>(fp);
				for(QAction *a: rpi->actions())
					tmplist.push_back(a->text());
			}
			addItems(fp->pluginName(), pluginType, tmplist);
		}
		
		std::string lbl = "Number of plugin loaded: " + std::to_string(pm.size());
		ui->label->setText(tr(lbl.c_str()).arg(QDir::toNativeSeparators(meshlab::defaultPluginPath())));
	}
}

void PluginInfoDialog::addItems(const QString& pluginName, const QString& pluginType, const QStringList& features)
{
	QTreeWidgetItem *pluginItem = new QTreeWidgetItem(ui->treeWidget);
	pluginItem->setText(PLUGINS, pluginName);
	pluginItem->setIcon(PLUGINS, interfaceIcon);
	pluginItem->setText(TYPE, pluginType);
	//pluginItem->setIcon(UNINSTALL, uninstallIcon);
	ui->treeWidget->setItemExpanded(pluginItem, false);
	QFont boldFont = pluginItem->font(PLUGINS);
	boldFont.setBold(true);
	pluginItem->setFont(PLUGINS, boldFont);
	for (const QString& feature: features) {
		QTreeWidgetItem *featureItem = new QTreeWidgetItem(pluginItem);
		featureItem->setText(PLUGINS, feature);
		featureItem->setIcon(PLUGINS, featureIcon);
	}
}

