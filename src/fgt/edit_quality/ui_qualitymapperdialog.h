/********************************************************************************
** Form generated from reading ui file 'qualitymapperdialog.ui'
**
** Created: Thu 17. Jan 22:33:05 2008
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
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

class Ui_QualityMapperDialogClass
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *transferFunctionGroupBox;
    QVBoxLayout *vboxLayout1;
    QGridLayout *gridLayout;
    QGraphicsView *transferFunctionView;
    QGroupBox *transferFunctionEditBox;
    QVBoxLayout *vboxLayout2;
    QVBoxLayout *vboxLayout3;
    QGroupBox *channelBox;
    QHBoxLayout *hboxLayout;
    QRadioButton *redButton;
    QRadioButton *greenButton;
    QRadioButton *blueButton;
    QHBoxLayout *hboxLayout1;
    QGroupBox *selectedPointBox;
    QVBoxLayout *vboxLayout4;
    QGridLayout *gridLayout1;
    QLabel *xLabel;
    QDoubleSpinBox *xSpinBox;
    QLabel *yLabel;
    QDoubleSpinBox *ySpinBox;
    QVBoxLayout *vboxLayout5;
    QPushButton *addPointButton;
    QPushButton *removePointButton;
    QLabel *colorbandLabel;
    QGroupBox *presetGroupBox;
    QHBoxLayout *hboxLayout2;
    QComboBox *presetComboBox;
    QPushButton *savePresetButton;
    QPushButton *removePresetButton;
    QGroupBox *equalizerGroupBox;
    QGridLayout *gridLayout2;
    QGraphicsView *equalizerGraphicsView;
    QGroupBox *groupBox;
    QHBoxLayout *hboxLayout3;
    QGraphicsView *gammaCorrectionGraphicsView;
    QHBoxLayout *hboxLayout4;
    QDoubleSpinBox *minSpinBox;
    QSpacerItem *spacerItem;
    QDoubleSpinBox *midSpinBox;
    QSpacerItem *spacerItem1;
    QDoubleSpinBox *maxSpinBox;
    QHBoxLayout *hboxLayout5;
    QPushButton *applyButton;
    QPushButton *previewButton;
    QPushButton *resetButton;
    QPushButton *cancelButton;
    QSpacerItem *spacerItem2;

    void setupUi(QDialog *QualityMapperDialogClass)
    {
    if (QualityMapperDialogClass->objectName().isEmpty())
        QualityMapperDialogClass->setObjectName(QString::fromUtf8("QualityMapperDialogClass"));
    QualityMapperDialogClass->resize(500, 600);
    QualityMapperDialogClass->setMinimumSize(QSize(500, 600));
    QualityMapperDialogClass->setMaximumSize(QSize(500, 600));
    QualityMapperDialogClass->setWindowIcon(QIcon(QString::fromUtf8(":/images/qualitymapper.png")));
    vboxLayout = new QVBoxLayout(QualityMapperDialogClass);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(11);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    transferFunctionGroupBox = new QGroupBox(QualityMapperDialogClass);
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
    channelBox = new QGroupBox(transferFunctionEditBox);
    channelBox->setObjectName(QString::fromUtf8("channelBox"));
    hboxLayout = new QHBoxLayout(channelBox);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(11);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    redButton = new QRadioButton(channelBox);
    redButton->setObjectName(QString::fromUtf8("redButton"));
    redButton->setChecked(true);

    hboxLayout->addWidget(redButton);

    greenButton = new QRadioButton(channelBox);
    greenButton->setObjectName(QString::fromUtf8("greenButton"));

    hboxLayout->addWidget(greenButton);

    blueButton = new QRadioButton(channelBox);
    blueButton->setObjectName(QString::fromUtf8("blueButton"));

    hboxLayout->addWidget(blueButton);


    vboxLayout3->addWidget(channelBox);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    selectedPointBox = new QGroupBox(transferFunctionEditBox);
    selectedPointBox->setObjectName(QString::fromUtf8("selectedPointBox"));
    vboxLayout4 = new QVBoxLayout(selectedPointBox);
    vboxLayout4->setSpacing(6);
    vboxLayout4->setMargin(11);
    vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
    gridLayout1 = new QGridLayout();
    gridLayout1->setSpacing(6);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    xLabel = new QLabel(selectedPointBox);
    xLabel->setObjectName(QString::fromUtf8("xLabel"));

    gridLayout1->addWidget(xLabel, 0, 0, 1, 1);

    xSpinBox = new QDoubleSpinBox(selectedPointBox);
    xSpinBox->setObjectName(QString::fromUtf8("xSpinBox"));
    xSpinBox->setMaximum(1);
    xSpinBox->setSingleStep(0.01);

    gridLayout1->addWidget(xSpinBox, 0, 1, 1, 1);

    yLabel = new QLabel(selectedPointBox);
    yLabel->setObjectName(QString::fromUtf8("yLabel"));

    gridLayout1->addWidget(yLabel, 1, 0, 1, 1);

    ySpinBox = new QDoubleSpinBox(selectedPointBox);
    ySpinBox->setObjectName(QString::fromUtf8("ySpinBox"));
    ySpinBox->setMaximum(1);
    ySpinBox->setSingleStep(0.01);

    gridLayout1->addWidget(ySpinBox, 1, 1, 1, 1);


    vboxLayout4->addLayout(gridLayout1);


    hboxLayout1->addWidget(selectedPointBox);

    vboxLayout5 = new QVBoxLayout();
    vboxLayout5->setSpacing(6);
    vboxLayout5->setObjectName(QString::fromUtf8("vboxLayout5"));
    addPointButton = new QPushButton(transferFunctionEditBox);
    addPointButton->setObjectName(QString::fromUtf8("addPointButton"));

    vboxLayout5->addWidget(addPointButton);

    removePointButton = new QPushButton(transferFunctionEditBox);
    removePointButton->setObjectName(QString::fromUtf8("removePointButton"));

    vboxLayout5->addWidget(removePointButton);


    hboxLayout1->addLayout(vboxLayout5);


    vboxLayout3->addLayout(hboxLayout1);


    vboxLayout2->addLayout(vboxLayout3);


    gridLayout->addWidget(transferFunctionEditBox, 0, 1, 1, 1);

    colorbandLabel = new QLabel(transferFunctionGroupBox);
    colorbandLabel->setObjectName(QString::fromUtf8("colorbandLabel"));
    colorbandLabel->setMaximumSize(QSize(256, 20));
    colorbandLabel->setFrameShape(QFrame::Box);
    colorbandLabel->setFrameShadow(QFrame::Plain);
    colorbandLabel->setPixmap(QPixmap(QString::fromUtf8(":/images/colorband.png")));
    colorbandLabel->setScaledContents(true);
    colorbandLabel->setAlignment(Qt::AlignCenter|Qt::AlignHCenter|Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    gridLayout->addWidget(colorbandLabel, 1, 0, 1, 1);

    presetGroupBox = new QGroupBox(transferFunctionGroupBox);
    presetGroupBox->setObjectName(QString::fromUtf8("presetGroupBox"));
    hboxLayout2 = new QHBoxLayout(presetGroupBox);
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(11);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    presetComboBox = new QComboBox(presetGroupBox);
    presetComboBox->setObjectName(QString::fromUtf8("presetComboBox"));
    presetComboBox->setMaximumSize(QSize(16777215, 20));
    presetComboBox->setEditable(true);

    hboxLayout2->addWidget(presetComboBox);

    savePresetButton = new QPushButton(presetGroupBox);
    savePresetButton->setObjectName(QString::fromUtf8("savePresetButton"));
    savePresetButton->setMaximumSize(QSize(20, 20));

    hboxLayout2->addWidget(savePresetButton);

    removePresetButton = new QPushButton(presetGroupBox);
    removePresetButton->setObjectName(QString::fromUtf8("removePresetButton"));
    removePresetButton->setMaximumSize(QSize(20, 20));

    hboxLayout2->addWidget(removePresetButton);


    gridLayout->addWidget(presetGroupBox, 1, 1, 1, 1);


    vboxLayout1->addLayout(gridLayout);


    vboxLayout->addWidget(transferFunctionGroupBox);

    equalizerGroupBox = new QGroupBox(QualityMapperDialogClass);
    equalizerGroupBox->setObjectName(QString::fromUtf8("equalizerGroupBox"));
    gridLayout2 = new QGridLayout(equalizerGroupBox);
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(11);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    equalizerGraphicsView = new QGraphicsView(equalizerGroupBox);
    equalizerGraphicsView->setObjectName(QString::fromUtf8("equalizerGraphicsView"));
    equalizerGraphicsView->setMinimumSize(QSize(300, 200));
    equalizerGraphicsView->setMaximumSize(QSize(16777215, 200));
    equalizerGraphicsView->setFrameShape(QFrame::StyledPanel);
    equalizerGraphicsView->setFrameShadow(QFrame::Sunken);

    gridLayout2->addWidget(equalizerGraphicsView, 0, 0, 1, 1);

    groupBox = new QGroupBox(equalizerGroupBox);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setAlignment(Qt::AlignLeading);
    hboxLayout3 = new QHBoxLayout(groupBox);
    hboxLayout3->setSpacing(6);
    hboxLayout3->setMargin(11);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    gammaCorrectionGraphicsView = new QGraphicsView(groupBox);
    gammaCorrectionGraphicsView->setObjectName(QString::fromUtf8("gammaCorrectionGraphicsView"));
    gammaCorrectionGraphicsView->setMaximumSize(QSize(196, 196));
    gammaCorrectionGraphicsView->setAlignment(Qt::AlignCenter);

    hboxLayout3->addWidget(gammaCorrectionGraphicsView);


    gridLayout2->addWidget(groupBox, 0, 1, 1, 1);

    hboxLayout4 = new QHBoxLayout();
    hboxLayout4->setSpacing(6);
    hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
    minSpinBox = new QDoubleSpinBox(equalizerGroupBox);
    minSpinBox->setObjectName(QString::fromUtf8("minSpinBox"));

    hboxLayout4->addWidget(minSpinBox);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout4->addItem(spacerItem);

    midSpinBox = new QDoubleSpinBox(equalizerGroupBox);
    midSpinBox->setObjectName(QString::fromUtf8("midSpinBox"));

    hboxLayout4->addWidget(midSpinBox);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout4->addItem(spacerItem1);

    maxSpinBox = new QDoubleSpinBox(equalizerGroupBox);
    maxSpinBox->setObjectName(QString::fromUtf8("maxSpinBox"));

    hboxLayout4->addWidget(maxSpinBox);


    gridLayout2->addLayout(hboxLayout4, 1, 0, 1, 1);


    vboxLayout->addWidget(equalizerGroupBox);

    hboxLayout5 = new QHBoxLayout();
    hboxLayout5->setSpacing(6);
    hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
    applyButton = new QPushButton(QualityMapperDialogClass);
    applyButton->setObjectName(QString::fromUtf8("applyButton"));
    applyButton->setDefault(true);

    hboxLayout5->addWidget(applyButton);

    previewButton = new QPushButton(QualityMapperDialogClass);
    previewButton->setObjectName(QString::fromUtf8("previewButton"));
    previewButton->setCheckable(true);

    hboxLayout5->addWidget(previewButton);

    resetButton = new QPushButton(QualityMapperDialogClass);
    resetButton->setObjectName(QString::fromUtf8("resetButton"));

    hboxLayout5->addWidget(resetButton);

    cancelButton = new QPushButton(QualityMapperDialogClass);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout5->addWidget(cancelButton);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout5->addItem(spacerItem2);


    vboxLayout->addLayout(hboxLayout5);


    retranslateUi(QualityMapperDialogClass);
    QObject::connect(cancelButton, SIGNAL(clicked()), QualityMapperDialogClass, SLOT(reject()));
    QObject::connect(applyButton, SIGNAL(clicked()), QualityMapperDialogClass, SLOT(accept()));

    QMetaObject::connectSlotsByName(QualityMapperDialogClass);
    } // setupUi

    void retranslateUi(QDialog *QualityMapperDialogClass)
    {
    QualityMapperDialogClass->setWindowTitle(QApplication::translate("QualityMapperDialogClass", "Quality Mapper Dialog", 0, QApplication::UnicodeUTF8));
    QualityMapperDialogClass->setToolTip(QApplication::translate("QualityMapperDialogClass", "gne gne gne!", 0, QApplication::UnicodeUTF8));
    transferFunctionGroupBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Transfer Function", 0, QApplication::UnicodeUTF8));
    transferFunctionEditBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Transfer Function Edit", 0, QApplication::UnicodeUTF8));
    channelBox->setToolTip(QApplication::translate("QualityMapperDialogClass", "Choose the channel to edit", 0, QApplication::UnicodeUTF8));
    channelBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Edit Channel", 0, QApplication::UnicodeUTF8));
    redButton->setText(QApplication::translate("QualityMapperDialogClass", "Red", 0, QApplication::UnicodeUTF8));
    greenButton->setText(QApplication::translate("QualityMapperDialogClass", "Green", 0, QApplication::UnicodeUTF8));
    blueButton->setText(QApplication::translate("QualityMapperDialogClass", "Blue", 0, QApplication::UnicodeUTF8));
    selectedPointBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Selected Point", 0, QApplication::UnicodeUTF8));
    xLabel->setText(QApplication::translate("QualityMapperDialogClass", "x", 0, QApplication::UnicodeUTF8));
    yLabel->setText(QApplication::translate("QualityMapperDialogClass", "y", 0, QApplication::UnicodeUTF8));
    addPointButton->setToolTip(QApplication::translate("QualityMapperDialogClass", "Add a new point to current channel", 0, QApplication::UnicodeUTF8));
    addPointButton->setText(QApplication::translate("QualityMapperDialogClass", "Add", 0, QApplication::UnicodeUTF8));
    removePointButton->setToolTip(QApplication::translate("QualityMapperDialogClass", "Remove selected point", 0, QApplication::UnicodeUTF8));
    removePointButton->setText(QApplication::translate("QualityMapperDialogClass", "Remove", 0, QApplication::UnicodeUTF8));
    colorbandLabel->setToolTip(QApplication::translate("QualityMapperDialogClass", "Color band", 0, QApplication::UnicodeUTF8));
    colorbandLabel->setText(QString());
    presetGroupBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Load Preset", 0, QApplication::UnicodeUTF8));
    savePresetButton->setToolTip(QApplication::translate("QualityMapperDialogClass", "Add/Save preset", 0, QApplication::UnicodeUTF8));
    savePresetButton->setText(QApplication::translate("QualityMapperDialogClass", "+", 0, QApplication::UnicodeUTF8));
    removePresetButton->setToolTip(QApplication::translate("QualityMapperDialogClass", "Remove preset", 0, QApplication::UnicodeUTF8));
    removePresetButton->setText(QApplication::translate("QualityMapperDialogClass", "-", 0, QApplication::UnicodeUTF8));
    equalizerGroupBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Equalizer", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("QualityMapperDialogClass", "Gamma correction", 0, QApplication::UnicodeUTF8));
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
