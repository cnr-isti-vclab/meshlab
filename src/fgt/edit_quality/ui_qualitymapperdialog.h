/********************************************************************************
** Form generated from reading ui file 'qualitymapperdialog.ui'
**
** Created: Mon 14. Jan 17:45:12 2008
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
    QVBoxLayout *vboxLayout;
    QGroupBox *transferFunctionGroupBox;
    QVBoxLayout *vboxLayout1;
    QGridLayout *gridLayout;
    QGraphicsView *transferFunctionView;
    QGroupBox *transferFunctionEditBox;
    QVBoxLayout *vboxLayout2;
    QVBoxLayout *vboxLayout3;
    QGroupBox *presetGroupBox;
    QVBoxLayout *vboxLayout4;
    QComboBox *presetComboBox;
    QGroupBox *channelBox;
    QHBoxLayout *hboxLayout;
    QHBoxLayout *hboxLayout1;
    QRadioButton *redButton;
    QRadioButton *greenButton;
    QRadioButton *blueButton;
    QHBoxLayout *hboxLayout2;
    QGroupBox *selectedPointBox;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout1;
    QLabel *xLabel;
    QSpinBox *xSpinBox;
    QLabel *yLabel;
    QSpinBox *ySpinBox;
    QVBoxLayout *vboxLayout5;
    QPushButton *addPointButton;
    QPushButton *removePointButton;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout3;
    QPushButton *saveTransferFunctionButton;
    QLabel *colorbandLabel;
    QSpacerItem *spacerItem1;
    QGroupBox *equalizerGroupBox;
    QVBoxLayout *vboxLayout6;
    QHBoxLayout *hboxLayout4;
    QGraphicsView *equalizerGraphicsView;
    QGridLayout *gridLayout2;
    QLabel *minLabel;
    QSpinBox *minSpinBox;
    QLabel *midLabel;
    QSpinBox *midSpinBox;
    QSpacerItem *spacerItem2;
    QLabel *maxLabel;
    QSpinBox *maxSpinBox;
    QSpacerItem *spacerItem3;
    QGraphicsView *gammaGraphicsView;
    QHBoxLayout *hboxLayout5;
    QPushButton *applyButton;
    QPushButton *previewButton;
    QPushButton *resetButton;
    QPushButton *cancelButton;
    QSpacerItem *spacerItem4;

    void setupUi(QDialog *QualityMapperDialogClass)
    {
    if (QualityMapperDialogClass->objectName().isEmpty())
        QualityMapperDialogClass->setObjectName(QString::fromUtf8("QualityMapperDialogClass"));
    QualityMapperDialogClass->resize(705, 591);
    layoutWidget = new QWidget(QualityMapperDialogClass);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(20, 0, 618, 571));
    vboxLayout = new QVBoxLayout(layoutWidget);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(11);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    transferFunctionGroupBox = new QGroupBox(layoutWidget);
    transferFunctionGroupBox->setObjectName(QString::fromUtf8("transferFunctionGroupBox"));
    vboxLayout1 = new QVBoxLayout(transferFunctionGroupBox);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(11);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    transferFunctionView = new QGraphicsView(transferFunctionGroupBox);
    transferFunctionView->setObjectName(QString::fromUtf8("transferFunctionView"));

    gridLayout->addWidget(transferFunctionView, 0, 0, 1, 1);

    transferFunctionEditBox = new QGroupBox(transferFunctionGroupBox);
    transferFunctionEditBox->setObjectName(QString::fromUtf8("transferFunctionEditBox"));
    vboxLayout2 = new QVBoxLayout(transferFunctionEditBox);
    vboxLayout2->setSpacing(6);
    vboxLayout2->setMargin(11);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    vboxLayout3 = new QVBoxLayout();
    vboxLayout3->setSpacing(6);
    vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
    presetGroupBox = new QGroupBox(transferFunctionEditBox);
    presetGroupBox->setObjectName(QString::fromUtf8("presetGroupBox"));
    vboxLayout4 = new QVBoxLayout(presetGroupBox);
    vboxLayout4->setSpacing(6);
    vboxLayout4->setMargin(11);
    vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
    presetComboBox = new QComboBox(presetGroupBox);
    presetComboBox->setObjectName(QString::fromUtf8("presetComboBox"));

    vboxLayout4->addWidget(presetComboBox);


    vboxLayout3->addWidget(presetGroupBox);

    channelBox = new QGroupBox(transferFunctionEditBox);
    channelBox->setObjectName(QString::fromUtf8("channelBox"));
    hboxLayout = new QHBoxLayout(channelBox);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(11);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    redButton = new QRadioButton(channelBox);
    redButton->setObjectName(QString::fromUtf8("redButton"));
    redButton->setChecked(true);

    hboxLayout1->addWidget(redButton);

    greenButton = new QRadioButton(channelBox);
    greenButton->setObjectName(QString::fromUtf8("greenButton"));

    hboxLayout1->addWidget(greenButton);

    blueButton = new QRadioButton(channelBox);
    blueButton->setObjectName(QString::fromUtf8("blueButton"));

    hboxLayout1->addWidget(blueButton);


    hboxLayout->addLayout(hboxLayout1);


    vboxLayout3->addWidget(channelBox);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    selectedPointBox = new QGroupBox(transferFunctionEditBox);
    selectedPointBox->setObjectName(QString::fromUtf8("selectedPointBox"));
    layoutWidget1 = new QWidget(selectedPointBox);
    layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
    layoutWidget1->setGeometry(QRect(10, 20, 56, 48));
    gridLayout1 = new QGridLayout(layoutWidget1);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout1->setContentsMargins(0, 0, 0, 0);
    xLabel = new QLabel(layoutWidget1);
    xLabel->setObjectName(QString::fromUtf8("xLabel"));

    gridLayout1->addWidget(xLabel, 0, 0, 1, 1);

    xSpinBox = new QSpinBox(layoutWidget1);
    xSpinBox->setObjectName(QString::fromUtf8("xSpinBox"));

    gridLayout1->addWidget(xSpinBox, 0, 1, 1, 1);

    yLabel = new QLabel(layoutWidget1);
    yLabel->setObjectName(QString::fromUtf8("yLabel"));

    gridLayout1->addWidget(yLabel, 1, 0, 1, 1);

    ySpinBox = new QSpinBox(layoutWidget1);
    ySpinBox->setObjectName(QString::fromUtf8("ySpinBox"));

    gridLayout1->addWidget(ySpinBox, 1, 1, 1, 1);


    hboxLayout2->addWidget(selectedPointBox);

    vboxLayout5 = new QVBoxLayout();
    vboxLayout5->setSpacing(6);
    vboxLayout5->setObjectName(QString::fromUtf8("vboxLayout5"));
    addPointButton = new QPushButton(transferFunctionEditBox);
    addPointButton->setObjectName(QString::fromUtf8("addPointButton"));

    vboxLayout5->addWidget(addPointButton);

    removePointButton = new QPushButton(transferFunctionEditBox);
    removePointButton->setObjectName(QString::fromUtf8("removePointButton"));

    vboxLayout5->addWidget(removePointButton);

    spacerItem = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout5->addItem(spacerItem);


    hboxLayout2->addLayout(vboxLayout5);


    vboxLayout3->addLayout(hboxLayout2);


    vboxLayout2->addLayout(vboxLayout3);


    gridLayout->addWidget(transferFunctionEditBox, 0, 1, 1, 1);

    hboxLayout3 = new QHBoxLayout();
    hboxLayout3->setSpacing(6);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    saveTransferFunctionButton = new QPushButton(transferFunctionGroupBox);
    saveTransferFunctionButton->setObjectName(QString::fromUtf8("saveTransferFunctionButton"));

    hboxLayout3->addWidget(saveTransferFunctionButton);

    colorbandLabel = new QLabel(transferFunctionGroupBox);
    colorbandLabel->setObjectName(QString::fromUtf8("colorbandLabel"));
    colorbandLabel->setFrameShape(QFrame::NoFrame);
    colorbandLabel->setFrameShadow(QFrame::Plain);

    hboxLayout3->addWidget(colorbandLabel);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout3->addItem(spacerItem1);


    gridLayout->addLayout(hboxLayout3, 1, 0, 1, 2);


    vboxLayout1->addLayout(gridLayout);


    vboxLayout->addWidget(transferFunctionGroupBox);

    equalizerGroupBox = new QGroupBox(layoutWidget);
    equalizerGroupBox->setObjectName(QString::fromUtf8("equalizerGroupBox"));
    vboxLayout6 = new QVBoxLayout(equalizerGroupBox);
    vboxLayout6->setSpacing(6);
    vboxLayout6->setMargin(11);
    vboxLayout6->setObjectName(QString::fromUtf8("vboxLayout6"));
    hboxLayout4 = new QHBoxLayout();
    hboxLayout4->setSpacing(6);
    hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
    equalizerGraphicsView = new QGraphicsView(equalizerGroupBox);
    equalizerGraphicsView->setObjectName(QString::fromUtf8("equalizerGraphicsView"));

    hboxLayout4->addWidget(equalizerGraphicsView);

    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    minLabel = new QLabel(equalizerGroupBox);
    minLabel->setObjectName(QString::fromUtf8("minLabel"));

    gridLayout2->addWidget(minLabel, 0, 0, 1, 1);

    minSpinBox = new QSpinBox(equalizerGroupBox);
    minSpinBox->setObjectName(QString::fromUtf8("minSpinBox"));

    gridLayout2->addWidget(minSpinBox, 0, 1, 1, 1);

    midLabel = new QLabel(equalizerGroupBox);
    midLabel->setObjectName(QString::fromUtf8("midLabel"));

    gridLayout2->addWidget(midLabel, 2, 0, 1, 1);

    midSpinBox = new QSpinBox(equalizerGroupBox);
    midSpinBox->setObjectName(QString::fromUtf8("midSpinBox"));

    gridLayout2->addWidget(midSpinBox, 2, 1, 1, 1);

    spacerItem2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem2, 3, 1, 1, 1);

    maxLabel = new QLabel(equalizerGroupBox);
    maxLabel->setObjectName(QString::fromUtf8("maxLabel"));

    gridLayout2->addWidget(maxLabel, 4, 0, 1, 1);

    maxSpinBox = new QSpinBox(equalizerGroupBox);
    maxSpinBox->setObjectName(QString::fromUtf8("maxSpinBox"));

    gridLayout2->addWidget(maxSpinBox, 4, 1, 1, 1);

    spacerItem3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem3, 1, 1, 1, 1);


    hboxLayout4->addLayout(gridLayout2);

    gammaGraphicsView = new QGraphicsView(equalizerGroupBox);
    gammaGraphicsView->setObjectName(QString::fromUtf8("gammaGraphicsView"));

    hboxLayout4->addWidget(gammaGraphicsView);


    vboxLayout6->addLayout(hboxLayout4);


    vboxLayout->addWidget(equalizerGroupBox);

    hboxLayout5 = new QHBoxLayout();
    hboxLayout5->setSpacing(6);
    hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
    applyButton = new QPushButton(layoutWidget);
    applyButton->setObjectName(QString::fromUtf8("applyButton"));
    applyButton->setDefault(true);

    hboxLayout5->addWidget(applyButton);

    previewButton = new QPushButton(layoutWidget);
    previewButton->setObjectName(QString::fromUtf8("previewButton"));
    previewButton->setCheckable(true);

    hboxLayout5->addWidget(previewButton);

    resetButton = new QPushButton(layoutWidget);
    resetButton->setObjectName(QString::fromUtf8("resetButton"));

    hboxLayout5->addWidget(resetButton);

    cancelButton = new QPushButton(layoutWidget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout5->addWidget(cancelButton);

    spacerItem4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout5->addItem(spacerItem4);


    vboxLayout->addLayout(hboxLayout5);


    retranslateUi(QualityMapperDialogClass);
    QObject::connect(addPointButton, SIGNAL(clicked()), addPointButton, SLOT(showMenu()));

    QMetaObject::connectSlotsByName(QualityMapperDialogClass);
    } // setupUi

    void retranslateUi(QDialog *QualityMapperDialogClass)
    {
    QualityMapperDialogClass->setWindowTitle(QApplication::translate("QualityMapperDialogClass", "QualityMapperDialog", 0, QApplication::UnicodeUTF8));
    transferFunctionGroupBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Transfer Function", 0, QApplication::UnicodeUTF8));
    transferFunctionEditBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Transfer Function Edit", 0, QApplication::UnicodeUTF8));
    presetGroupBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Load Preset", 0, QApplication::UnicodeUTF8));
    channelBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Edit Channel", 0, QApplication::UnicodeUTF8));
    redButton->setText(QApplication::translate("QualityMapperDialogClass", "Red", 0, QApplication::UnicodeUTF8));
    greenButton->setText(QApplication::translate("QualityMapperDialogClass", "Green", 0, QApplication::UnicodeUTF8));
    blueButton->setText(QApplication::translate("QualityMapperDialogClass", "Blue", 0, QApplication::UnicodeUTF8));
    selectedPointBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Selected Point", 0, QApplication::UnicodeUTF8));
    xLabel->setText(QApplication::translate("QualityMapperDialogClass", "x", 0, QApplication::UnicodeUTF8));
    yLabel->setText(QApplication::translate("QualityMapperDialogClass", "y", 0, QApplication::UnicodeUTF8));
    addPointButton->setText(QApplication::translate("QualityMapperDialogClass", "Add", 0, QApplication::UnicodeUTF8));
    removePointButton->setText(QApplication::translate("QualityMapperDialogClass", "Remove", 0, QApplication::UnicodeUTF8));
    saveTransferFunctionButton->setText(QApplication::translate("QualityMapperDialogClass", "Save", 0, QApplication::UnicodeUTF8));
    colorbandLabel->setText(QApplication::translate("QualityMapperDialogClass", "ColorBandLabel", 0, QApplication::UnicodeUTF8));
    equalizerGroupBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Equalizer", 0, QApplication::UnicodeUTF8));
    minLabel->setText(QApplication::translate("QualityMapperDialogClass", "min", 0, QApplication::UnicodeUTF8));
    midLabel->setText(QApplication::translate("QualityMapperDialogClass", "mid", 0, QApplication::UnicodeUTF8));
    maxLabel->setText(QApplication::translate("QualityMapperDialogClass", "max", 0, QApplication::UnicodeUTF8));
    applyButton->setText(QApplication::translate("QualityMapperDialogClass", "Apply", 0, QApplication::UnicodeUTF8));
    previewButton->setText(QApplication::translate("QualityMapperDialogClass", "Preview", 0, QApplication::UnicodeUTF8));
    resetButton->setText(QApplication::translate("QualityMapperDialogClass", "Reset", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("QualityMapperDialogClass", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(QualityMapperDialogClass);
    } // retranslateUi

};

namespace Ui {
    class QualityMapperDialogClass: public Ui_QualityMapperDialogClass {};
} // namespace Ui

#endif // UI_QUALITYMAPPERDIALOG_H
