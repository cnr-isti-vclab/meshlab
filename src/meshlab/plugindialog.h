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
#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>
#include <QIcon>
#include <QTextEdit>
#include <QSpacerItem>


class QLabel;
class QPushButton;
class QStringList;
class QTreeWidget;
class QTreeWidgetItem;
class QGroupBox;

class PluginDialog : public QDialog
{
    Q_OBJECT

public:
  PluginDialog(const QString &path, const QStringList &fileNames,QWidget *parent = 0);

private:
  void populateTreeWidget(const QString &path, const QStringList &fileNames);
  void addItems(QTreeWidgetItem *pluginItem, const QStringList &features);

  QLabel *label;
  QTreeWidget *treeWidget;
  QLabel * labelInfo;
	QPushButton *okButton;
  QIcon interfaceIcon;
	QIcon featureIcon;
	QSpacerItem *spacerItem;
	QGroupBox *groupBox;
//	QGroupBox *groupBox;
	QString pathDirectory;
public slots:
	void displayInfo(QTreeWidgetItem* item,int ncolumn);

};

#endif
