/********************************************************************************
** Form generated from reading ui file 'qualitymapperdialog.ui'
**
** Created: Fri 11. Jan 19:01:30 2008
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
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGraphicsView>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
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
    QGroupBox *groupBox;

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
    transferFunctionFrame->setGeometry(QRect(50, 30, 511, 381));
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
    transferFunctionEditBox->setGeometry(QRect(310, 130, 191, 161));
    channelBox = new QGroupBox(transferFunctionEditBox);
    channelBox->setObjectName(QString::fromUtf8("channelBox"));
    channelBox->setGeometry(QRect(10, 20, 174, 40));
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

    groupBox = new QGroupBox(transferFunctionEditBox);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(30, 70, 120, 80));

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
    channelBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Channel", 0, QApplication::UnicodeUTF8));
    redButton->setText(QApplication::translate("QualityMapperDialogClass", "Red", 0, QApplication::UnicodeUTF8));
    greenButton->setText(QApplication::translate("QualityMapperDialogClass", "Green", 0, QApplication::UnicodeUTF8));
    blueButton->setText(QApplication::translate("QualityMapperDialogClass", "Blue", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("QualityMapperDialogClass", "GroupBox", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(QualityMapperDialogClass);
    } // retranslateUi

};

namespace Ui {
    class QualityMapperDialogClass: public Ui_QualityMapperDialogClass {};
} // namespace Ui

#endif // UI_QUALITYMAPPERDIALOG_H
