/********************************************************************************
** Form generated from reading ui file 'meshcutdialog.ui'
**
** Created: Tue 5. Jun 15:47:45 2007
**      by: Qt User Interface Compiler version 4.2.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MESHCUTDIALOG_H
#define UI_MESHCUTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>

class Ui_MeshCutDialogClass
{
public:
    QPushButton *meshcutButton;


    void setupUi(QWidget *MeshCutDialogClass)
    {
    MeshCutDialogClass->setObjectName(QString::fromUtf8("MeshCutDialogClass"));
    

		QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(MeshCutDialogClass->sizePolicy().hasHeightForWidth());
    MeshCutDialogClass->setSizePolicy(sizePolicy);


		meshcutButton = new QPushButton(MeshCutDialogClass);
    meshcutButton->setObjectName(QString::fromUtf8("meshcutButton"));
    meshcutButton->setGeometry(QRect(70, 30, 75, 23));


		retranslateUi(MeshCutDialogClass);


		/*QSize size(210, 102);
    size = size.expandedTo(MeshCutDialogClass->minimumSizeHint());
		
    MeshCutDialogClass->resize(size);*/


    QMetaObject::connectSlotsByName(MeshCutDialogClass);
    } // setupUi

    void retranslateUi(QWidget *MeshCutDialogClass)
    {
    MeshCutDialogClass->setWindowTitle(QApplication::translate("MeshCutDialogClass", "MeshCutDialog", 0, QApplication::UnicodeUTF8));
    meshcutButton->setText(QApplication::translate("MeshCutDialogClass", "MeshCut!", 0, QApplication::UnicodeUTF8));
    

    Q_UNUSED(MeshCutDialogClass);
    } // retranslateUi

};

namespace Ui {
    class MeshCutDialogClass: public Ui_MeshCutDialogClass {};
} // namespace Ui

#endif // UI_MESHCUTDIALOG_H
