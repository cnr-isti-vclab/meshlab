/********************************************************************************
** Form generated from reading ui file 'layerDialog.ui'
**
** Created: Thu 8. Apr 16:11:46 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_LAYERDIALOG_H
#define UI_LAYERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_layerDialog
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QTableWidget *layerTableWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *menuButton;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPlainTextEdit *logPlainTextEdit;

    void setupUi(QDockWidget *layerDialog)
    {
        if (layerDialog->objectName().isEmpty())
            layerDialog->setObjectName(QString::fromUtf8("layerDialog"));
        layerDialog->resize(178, 407);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        layerTableWidget = new QTableWidget(dockWidgetContents);
        layerTableWidget->setObjectName(QString::fromUtf8("layerTableWidget"));

        verticalLayout->addWidget(layerTableWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(28, 23, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        menuButton = new QPushButton(dockWidgetContents);
        menuButton->setObjectName(QString::fromUtf8("menuButton"));

        horizontalLayout->addWidget(menuButton);

        addButton = new QPushButton(dockWidgetContents);
        addButton->setObjectName(QString::fromUtf8("addButton"));

        horizontalLayout->addWidget(addButton);

        deleteButton = new QPushButton(dockWidgetContents);
        deleteButton->setObjectName(QString::fromUtf8("deleteButton"));
        deleteButton->setFlat(false);

        horizontalLayout->addWidget(deleteButton);


        verticalLayout->addLayout(horizontalLayout);

        logPlainTextEdit = new QPlainTextEdit(dockWidgetContents);
        logPlainTextEdit->setObjectName(QString::fromUtf8("logPlainTextEdit"));

        verticalLayout->addWidget(logPlainTextEdit);

        layerDialog->setWidget(dockWidgetContents);

        retranslateUi(layerDialog);

        QMetaObject::connectSlotsByName(layerDialog);
    } // setupUi

    void retranslateUi(QDockWidget *layerDialog)
    {
        layerDialog->setWindowTitle(QApplication::translate("layerDialog", "Layer Dialog", 0, QApplication::UnicodeUTF8));
        menuButton->setText(QApplication::translate("layerDialog", "...", 0, QApplication::UnicodeUTF8));
        addButton->setText(QApplication::translate("layerDialog", "+", 0, QApplication::UnicodeUTF8));
        deleteButton->setText(QApplication::translate("layerDialog", "-", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(layerDialog);
    } // retranslateUi

};

namespace Ui {
    class layerDialog: public Ui_layerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LAYERDIALOG_H
