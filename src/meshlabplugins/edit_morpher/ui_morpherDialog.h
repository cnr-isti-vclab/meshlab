/********************************************************************************
** Form generated from reading ui file 'morpherDialog.ui'
**
** Created: Tue May 27 10:54:52 2008
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MORPHERDIALOG_H
#define UI_MORPHERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_morpherDialog
{
public:
    QFrame *frame;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QPushButton *pickSourceMeshButton;
    QLabel *souceLabel;
    QPushButton *pickDestMeshButton;
    QLabel *destLabel;
    QTreeWidget *layerTreeWidget;
    QWidget *layoutWidget1;
    QHBoxLayout *hboxLayout;
    QLabel *label_2;
    QSpinBox *startSpinBox;
    QLabel *label_3;
    QSpinBox *endSpinBox;
    QWidget *widget;
    QHBoxLayout *hboxLayout1;
    QLabel *label;
    QLabel *sliderValueLabel;
    QSlider *morphSlider;

    void setupUi(QWidget *morpherDialog)
    {
    if (morpherDialog->objectName().isEmpty())
        morpherDialog->setObjectName(QString::fromUtf8("morpherDialog"));
    morpherDialog->resize(540, 196);
    frame = new QFrame(morpherDialog);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setGeometry(QRect(10, 10, 521, 171));
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    layoutWidget = new QWidget(frame);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(10, 10, 271, 65));
    gridLayout = new QGridLayout(layoutWidget);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setContentsMargins(0, 0, 0, 0);
    pickSourceMeshButton = new QPushButton(layoutWidget);
    pickSourceMeshButton->setObjectName(QString::fromUtf8("pickSourceMeshButton"));

    gridLayout->addWidget(pickSourceMeshButton, 0, 0, 1, 1);

    souceLabel = new QLabel(layoutWidget);
    souceLabel->setObjectName(QString::fromUtf8("souceLabel"));

    gridLayout->addWidget(souceLabel, 0, 1, 1, 1);

    pickDestMeshButton = new QPushButton(layoutWidget);
    pickDestMeshButton->setObjectName(QString::fromUtf8("pickDestMeshButton"));

    gridLayout->addWidget(pickDestMeshButton, 1, 0, 1, 1);

    destLabel = new QLabel(layoutWidget);
    destLabel->setObjectName(QString::fromUtf8("destLabel"));

    gridLayout->addWidget(destLabel, 1, 1, 1, 1);

    layerTreeWidget = new QTreeWidget(frame);
    layerTreeWidget->setObjectName(QString::fromUtf8("layerTreeWidget"));
    layerTreeWidget->setGeometry(QRect(290, 10, 221, 151));
    layerTreeWidget->setColumnCount(2);
    layoutWidget1 = new QWidget(frame);
    layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
    layoutWidget1->setGeometry(QRect(10, 130, 190, 25));
    hboxLayout = new QHBoxLayout(layoutWidget1);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    label_2 = new QLabel(layoutWidget1);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    hboxLayout->addWidget(label_2);

    startSpinBox = new QSpinBox(layoutWidget1);
    startSpinBox->setObjectName(QString::fromUtf8("startSpinBox"));
    startSpinBox->setMinimum(-400);

    hboxLayout->addWidget(startSpinBox);

    label_3 = new QLabel(layoutWidget1);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    hboxLayout->addWidget(label_3);

    endSpinBox = new QSpinBox(layoutWidget1);
    endSpinBox->setObjectName(QString::fromUtf8("endSpinBox"));
    endSpinBox->setMaximum(400);
    endSpinBox->setValue(100);

    hboxLayout->addWidget(endSpinBox);

    widget = new QWidget(frame);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(12, 82, 271, 31));
    hboxLayout1 = new QHBoxLayout(widget);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    hboxLayout1->setContentsMargins(0, 0, 0, 0);
    label = new QLabel(widget);
    label->setObjectName(QString::fromUtf8("label"));

    hboxLayout1->addWidget(label);

    sliderValueLabel = new QLabel(widget);
    sliderValueLabel->setObjectName(QString::fromUtf8("sliderValueLabel"));

    hboxLayout1->addWidget(sliderValueLabel);

    morphSlider = new QSlider(widget);
    morphSlider->setObjectName(QString::fromUtf8("morphSlider"));
    morphSlider->setMaximum(100);
    morphSlider->setOrientation(Qt::Horizontal);

    hboxLayout1->addWidget(morphSlider);


    retranslateUi(morpherDialog);

    QMetaObject::connectSlotsByName(morpherDialog);
    } // setupUi

    void retranslateUi(QWidget *morpherDialog)
    {
    morpherDialog->setWindowTitle(QApplication::translate("morpherDialog", "Form", 0, QApplication::UnicodeUTF8));
    pickSourceMeshButton->setText(QApplication::translate("morpherDialog", "Pick Source Mesh", 0, QApplication::UnicodeUTF8));
    souceLabel->setText(QApplication::translate("morpherDialog", "Souce Mesh Name", 0, QApplication::UnicodeUTF8));
    pickDestMeshButton->setText(QApplication::translate("morpherDialog", "Pick Dest. Mesh", 0, QApplication::UnicodeUTF8));
    destLabel->setText(QApplication::translate("morpherDialog", "Dest. Mesh Name", 0, QApplication::UnicodeUTF8));
    layerTreeWidget->headerItem()->setText(0, QApplication::translate("morpherDialog", "1", 0, QApplication::UnicodeUTF8));
    layerTreeWidget->headerItem()->setText(1, QApplication::translate("morpherDialog", "2", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("morpherDialog", "Start:", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("morpherDialog", "End:", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("morpherDialog", "Morph:", 0, QApplication::UnicodeUTF8));
    sliderValueLabel->setText(QApplication::translate("morpherDialog", "000", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(morpherDialog);
    } // retranslateUi

};

namespace Ui {
    class morpherDialog: public Ui_morpherDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MORPHERDIALOG_H
