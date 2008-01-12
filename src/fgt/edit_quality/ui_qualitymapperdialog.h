/********************************************************************************
** Form generated from reading ui file 'qualitymapperdialog.ui'
**
** Created: Fri 11. Jan 19:33:35 2008
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QUALITYMAPPERDIALOG_H
#define UI_QUALITYMAPPERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_QualityMapperDialogClass
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QPushButton *applyButton;
    QSpacerItem *spacerItem;
    QPushButton *previewButton;
    QSpacerItem *spacerItem1;
    QPushButton *resetButton;
    QFrame *transferFunctionFrame;
    QPushButton *saveTransferFunctionButton;
    QGraphicsView *transferFunctionView;
    QLabel *colorbandLabel;
    QGroupBox *transferFunctionEditBox;
    QGroupBox *channelBox;
    QWidget *widget;
    QHBoxLayout *hboxLayout1;
    QRadioButton *redButton;
    QRadioButton *greenButton;
    QRadioButton *blueButton;
    QWidget *widget1;
    QVBoxLayout *vboxLayout;
    QLabel *presetLabel;
    QComboBox *presetComboBox;
    QWidget *widget2;
    QHBoxLayout *hboxLayout2;
    QGroupBox *selectedPointBox;
    QWidget *widget3;
    QGridLayout *gridLayout;
    QLabel *xLabel;
    QSpinBox *xSpinBox;
    QLabel *yLabel;
    QSpinBox *ySpinBox;
    QVBoxLayout *vboxLayout1;
    QPushButton *addButton;
    QPushButton *removeButton;
    QSpacerItem *spacerItem2;

    void setupUi(QDialog *QualityMapperDialogClass)
    {
    if (QualityMapperDialogClass->objectName().isEmpty())
        QualityMapperDialogClass->setObjectName(QString::fromUtf8("QualityMapperDialogClass"));
    QualityMapperDialogClass->resize(649, 629);
    layoutWidget = new QWidget(QualityMapperDialogClass);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(80, 470, 337, 27));
    hboxLayout = new QHBoxLayout(layoutWidget);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(11);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    applyButton = new QPushButton(layoutWidget);
    applyButton->setObjectName(QString::fromUtf8("applyButton"));

    hboxLayout->addWidget(applyButton);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    previewButton = new QPushButton(layoutWidget);
    previewButton->setObjectName(QString::fromUtf8("previewButton"));

    hboxLayout->addWidget(previewButton);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem1);

    resetButton = new QPushButton(layoutWidget);
    resetButton->setObjectName(QString::fromUtf8("resetButton"));

    hboxLayout->addWidget(resetButton);

    transferFunctionFrame = new QFrame(QualityMapperDialogClass);
    transferFunctionFrame->setObjectName(QString::fromUtf8("transferFunctionFrame"));
    transferFunctionFrame->setGeometry(QRect(40, 10, 511, 381));
    transferFunctionFrame->setFrameShape(QFrame::StyledPanel);
    transferFunctionFrame->setFrameShadow(QFrame::Raised);
    saveTransferFunctionButton = new QPushButton(transferFunctionFrame);
    saveTransferFunctionButton->setObjectName(QString::fromUtf8("saveTransferFunctionButton"));
    saveTransferFunctionButton->setGeometry(QRect(20, 350, 75, 23));
    transferFunctionView = new QGraphicsView(transferFunctionFrame);
    transferFunctionView->setObjectName(QString::fromUtf8("transferFunctionView"));
    transferFunctionView->setGeometry(QRect(30, 110, 256, 192));
    colorbandLabel = new QLabel(transferFunctionFrame);
    colorbandLabel->setObjectName(QString::fromUtf8("colorbandLabel"));
    colorbandLabel->setGeometry(QRect(120, 350, 74, 14));
    transferFunctionEditBox = new QGroupBox(transferFunctionFrame);
    transferFunctionEditBox->setObjectName(QString::fromUtf8("transferFunctionEditBox"));
    transferFunctionEditBox->setGeometry(QRect(300, 140, 211, 241));
    channelBox = new QGroupBox(transferFunctionEditBox);
    channelBox->setObjectName(QString::fromUtf8("channelBox"));
    channelBox->setGeometry(QRect(0, 70, 174, 40));
    widget = new QWidget(channelBox);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(10, 10, 154, 20));
    hboxLayout1 = new QHBoxLayout(widget);
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(11);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    hboxLayout1->setContentsMargins(0, 0, 0, 0);
    redButton = new QRadioButton(widget);
    redButton->setObjectName(QString::fromUtf8("redButton"));

    hboxLayout1->addWidget(redButton);

    greenButton = new QRadioButton(widget);
    greenButton->setObjectName(QString::fromUtf8("greenButton"));

    hboxLayout1->addWidget(greenButton);

    blueButton = new QRadioButton(widget);
    blueButton->setObjectName(QString::fromUtf8("blueButton"));

    hboxLayout1->addWidget(blueButton);

    widget1 = new QWidget(transferFunctionEditBox);
    widget1->setObjectName(QString::fromUtf8("widget1"));
    widget1->setGeometry(QRect(10, 20, 161, 42));
    vboxLayout = new QVBoxLayout(widget1);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(11);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    presetLabel = new QLabel(widget1);
    presetLabel->setObjectName(QString::fromUtf8("presetLabel"));

    vboxLayout->addWidget(presetLabel);

    presetComboBox = new QComboBox(widget1);
    presetComboBox->setObjectName(QString::fromUtf8("presetComboBox"));

    vboxLayout->addWidget(presetComboBox);

    widget2 = new QWidget(transferFunctionEditBox);
    widget2->setObjectName(QString::fromUtf8("widget2"));
    widget2->setGeometry(QRect(11, 111, 174, 82));
    hboxLayout2 = new QHBoxLayout(widget2);
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(11);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    hboxLayout2->setContentsMargins(0, 0, 0, 0);
    selectedPointBox = new QGroupBox(widget2);
    selectedPointBox->setObjectName(QString::fromUtf8("selectedPointBox"));
    widget3 = new QWidget(selectedPointBox);
    widget3->setObjectName(QString::fromUtf8("widget3"));
    widget3->setGeometry(QRect(10, 20, 56, 48));
    gridLayout = new QGridLayout(widget3);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setContentsMargins(0, 0, 0, 0);
    xLabel = new QLabel(widget3);
    xLabel->setObjectName(QString::fromUtf8("xLabel"));

    gridLayout->addWidget(xLabel, 0, 0, 1, 1);

    xSpinBox = new QSpinBox(widget3);
    xSpinBox->setObjectName(QString::fromUtf8("xSpinBox"));

    gridLayout->addWidget(xSpinBox, 0, 1, 1, 1);

    yLabel = new QLabel(widget3);
    yLabel->setObjectName(QString::fromUtf8("yLabel"));

    gridLayout->addWidget(yLabel, 1, 0, 1, 1);

    ySpinBox = new QSpinBox(widget3);
    ySpinBox->setObjectName(QString::fromUtf8("ySpinBox"));

    gridLayout->addWidget(ySpinBox, 1, 1, 1, 1);


    hboxLayout2->addWidget(selectedPointBox);

    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    addButton = new QPushButton(widget2);
    addButton->setObjectName(QString::fromUtf8("addButton"));

    vboxLayout1->addWidget(addButton);

    removeButton = new QPushButton(widget2);
    removeButton->setObjectName(QString::fromUtf8("removeButton"));

    vboxLayout1->addWidget(removeButton);

    spacerItem2 = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout1->addItem(spacerItem2);


    hboxLayout2->addLayout(vboxLayout1);


    retranslateUi(QualityMapperDialogClass);

    QMetaObject::connectSlotsByName(QualityMapperDialogClass);
    } // setupUi

    void retranslateUi(QDialog *QualityMapperDialogClass)
    {
    QualityMapperDialogClass->setWindowTitle(QApplication::translate("QualityMapperDialogClass", "QualityMapperDialog", 0, QApplication::UnicodeUTF8));
    applyButton->setText(QApplication::translate("QualityMapperDialogClass", "Apply", 0, QApplication::UnicodeUTF8));
    previewButton->setText(QApplication::translate("QualityMapperDialogClass", "Preview", 0, QApplication::UnicodeUTF8));
    resetButton->setText(QApplication::translate("QualityMapperDialogClass", "Reset", 0, QApplication::UnicodeUTF8));
    saveTransferFunctionButton->setText(QApplication::translate("QualityMapperDialogClass", "Save", 0, QApplication::UnicodeUTF8));
    colorbandLabel->setText(QApplication::translate("QualityMapperDialogClass", "ColorBandLabel", 0, QApplication::UnicodeUTF8));
    transferFunctionEditBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Transfer Function Edit", 0, QApplication::UnicodeUTF8));
    channelBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Edit Channel", 0, QApplication::UnicodeUTF8));
    redButton->setText(QApplication::translate("QualityMapperDialogClass", "Red", 0, QApplication::UnicodeUTF8));
    greenButton->setText(QApplication::translate("QualityMapperDialogClass", "Green", 0, QApplication::UnicodeUTF8));
    blueButton->setText(QApplication::translate("QualityMapperDialogClass", "Blue", 0, QApplication::UnicodeUTF8));
    presetLabel->setText(QApplication::translate("QualityMapperDialogClass", "Load Preset", 0, QApplication::UnicodeUTF8));
    selectedPointBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Selected Point", 0, QApplication::UnicodeUTF8));
    xLabel->setText(QApplication::translate("QualityMapperDialogClass", "x", 0, QApplication::UnicodeUTF8));
    yLabel->setText(QApplication::translate("QualityMapperDialogClass", "y", 0, QApplication::UnicodeUTF8));
    addButton->setText(QApplication::translate("QualityMapperDialogClass", "Add", 0, QApplication::UnicodeUTF8));
    removeButton->setText(QApplication::translate("QualityMapperDialogClass", "Remove", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(QualityMapperDialogClass);
    } // retranslateUi

};

namespace Ui {
    class QualityMapperDialogClass: public Ui_QualityMapperDialogClass {};
} // namespace Ui

#endif // UI_QUALITYMAPPERDIALOG_H
