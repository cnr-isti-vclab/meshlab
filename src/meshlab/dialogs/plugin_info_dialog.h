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

#ifndef MESHLAB_PLUGIN_INFO_DIALOG_H
#define MESHLAB_PLUGIN_INFO_DIALOG_H

#include <QDialog>
#include <QIcon>

namespace Ui {
class PluginInfoDialog;
}

class QTreeWidgetItem;

class PluginInfoDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit PluginInfoDialog(QWidget *parent = nullptr);
	~PluginInfoDialog();

private slots:
	void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
	
private:
	void populateTreeWidget();
	void addItems(const QString& pluginName, const QString& pluginType, const QStringList &features);
	
	Ui::PluginInfoDialog *ui;
	QIcon interfaceIcon;
	QIcon featureIcon;
	QString pathDirectory;
};

#endif // MESHLAB_PLUGIN_INFO_DIALOG_H
