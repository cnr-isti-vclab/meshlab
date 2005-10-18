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

#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>
#include <QIcon>

class QLabel;
class QPushButton;
class QStringList;
class QTreeWidget;
class QTreeWidgetItem;

class PluginDialog : public QDialog
{
    Q_OBJECT

public:
    PluginDialog(const QString &path, const QStringList &fileNames,
                 QWidget *parent = 0);

private:
    void populateTreeWidget(const QString &path, const QStringList &fileNames);
    void addItems(QTreeWidgetItem *pluginItem, const char *interfaceName,
                  const QStringList &features);

    QLabel *label;
    QTreeWidget *treeWidget;
    QPushButton *okButton;
    QIcon interfaceIcon;
    QIcon featureIcon;
};

#endif
