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
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include <common/plugins/interfaces/filter_plugin_interface.h>
#include <common/plugins/interfaces/iomesh_plugin_interface.h>
#include <common/plugins/interfaces/decorate_plugin_interface.h>
#include <common/plugins/interfaces/render_plugin_interface.h>
#include <common/plugins/interfaces/edit_plugin_interface.h>
#include <common/globals.h>
#include <common/mlexception.h>
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

/**
 * @brief This function will be called every time the user sets
 * a plugin to enabled or disabled.
 */
void PluginInfoDialog::chechBoxStateChanged(int state)
{
	QCheckBox* cb = (QCheckBox*)QObject::sender(); 
	int nPlug = cb->property("np").toInt();
	PluginManager& pm = meshlab::pluginManagerInstance();
	PluginFileInterface* fpi = pm[nPlug];
	if (state == Qt::Checked){
		pm.enablePlugin(fpi);
	}
	else {
		pm.disablePlugin(fpi);
	}
}

/**
 * @brief This function will be called every time the user wants to uninstall 
 * a plugin.
 */
void PluginInfoDialog::uninstallPluginPushButtonClicked()
{
	QPushButton* pb = (QPushButton*)QObject::sender();
	int nPlug = pb->property("np").toInt();
	PluginManager& pm = meshlab::pluginManagerInstance();
	PluginFileInterface* fpi = pm[nPlug];
	QFileInfo fdel = fpi->pluginFileInfo();
	pm.unloadPlugin(fpi);
	QFile::remove(fdel.absoluteFilePath());
	ui->treeWidget->clear();
	populateTreeWidget();
	ui->treeWidget->update();
}

void PluginInfoDialog::on_loadPluginsPushButton_clicked()
{
#ifdef _WIN32
	QString pluginFileFormat = "*.dll MeshLab Plugin (*.dll)";
#else //other os
	QString pluginFileFormat = "*.so MeshLab Plugin (*.so)";
#endif
	QStringList fileList = QFileDialog::getOpenFileNames(this, "Load Plugins", "", pluginFileFormat);
	PluginManager& pm = meshlab::pluginManagerInstance();
	bool loadOk = false;
	for (const QString& fileName : fileList){
		QFileInfo finfo(fileName);
		QDir appDir(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).first());
		appDir.mkpath(appDir.absolutePath());
		
		appDir.mkdir("MeshLabExtraPlugins");
		appDir.cd("MeshLabExtraPlugins");
		
		try {
			PluginManager::checkPlugin(fileName);
			
			QString newFileName = appDir.absolutePath() + "/" +finfo.fileName();
			QFile::copy(fileName, newFileName);
			
			pm.loadPlugin(newFileName);
			loadOk = true;
		}
		catch(const MLException& e){
			QMessageBox::warning(this, "Error while loading plugin", fileName + ":\n" + e.what());
		}
	}
	if (loadOk){
		ui->treeWidget->clear();
		populateTreeWidget();
		ui->treeWidget->update();
	}
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
		int nPlug = 0;
		for (PluginFileInterface* fp : pm.pluginIterator(true)){
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
			addItems(fp, nPlug++, pluginType, tmplist);
		}
		
		std::string lbl = "Number of plugin loaded: " + std::to_string(pm.size());
		ui->label->setText(tr(lbl.c_str()));
	}
}

void PluginInfoDialog::addItems(const PluginFileInterface* fpi, int nPlug, const QString& pluginType, const QStringList& features)
{
	QTreeWidgetItem *pluginItem = new QTreeWidgetItem(ui->treeWidget);
	pluginItem->setText(PLUGINS, fpi->pluginName());
	pluginItem->setIcon(PLUGINS, interfaceIcon);
	pluginItem->setText(TYPE, pluginType);
	pluginItem->setText(FILE, fpi->pluginFileInfo().fileName());
	pluginItem->setToolTip(FILE, fpi->pluginFileInfo().absoluteFilePath());
	pluginItem->setText(VENDOR, fpi->vendor());
	
	QCheckBox* cb = new QCheckBox(this);
	cb->setProperty("np", nPlug);
	cb->setChecked(fpi->isEnabled());
	connect(cb, SIGNAL(stateChanged(int)),
			this, SLOT(chechBoxStateChanged(int)));
	ui->treeWidget->setItemWidget(pluginItem, ENABLED, cb);
	
	QPushButton* pb = new QPushButton(this);
	pb->setProperty("np", nPlug);
	pb->setIcon(uninstallIcon);
	connect(pb, SIGNAL(clicked()),
			this, SLOT(uninstallPluginPushButtonClicked()));
	if (fpi->pluginFileInfo().absolutePath() == meshlab::defaultPluginPath())
		pb->setEnabled(false);
	ui->treeWidget->setItemWidget(pluginItem, UNINSTALL, pb);
	
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
