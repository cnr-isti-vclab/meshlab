/********************************************************************************
** Form generated from reading ui file 'textureeditor.ui'
**
** Created: Mon 24. Mar 18:26:52 2008
**      by: Qt User Interface Compiler version 4.2.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TEXTUREEDITOR_H
#define UI_TEXTUREEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_TextureEditorClass
{
public:
    QVBoxLayout *vboxLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGroupBox *groupBoxAction;
    QHBoxLayout *hboxLayout;
    QPushButton *moveButton;
    QPushButton *selectButton;
    QPushButton *editButton;
    QPushButton *optimizeButton;
    QPushButton *optionButton;
    QGroupBox *groupBoxUVMapEdit;
    QGridLayout *gridLayout;
    QGroupBox *groupBox_4;
    QVBoxLayout *vboxLayout1;
    QPushButton *clampButton;
    QPushButton *modulusButton;
    QGroupBox *groupBox_5;
    QHBoxLayout *hboxLayout1;
    QSpinBox *spinBoxAngle;
    QLabel *label_2;
    QSpacerItem *spacerItem;
    QPushButton *rotateButton;
    QGroupBox *groupBox;
    QVBoxLayout *vboxLayout2;
    QRadioButton *radioButtonMove;
    QRadioButton *radioButtonSelect;
    QGroupBox *groupBox_6;
    QHBoxLayout *hboxLayout2;
    QSpinBox *spinBoxScale;
    QLabel *label;
    QSpacerItem *spacerItem1;
    QPushButton *scaleButton;
    QGroupBox *groupBoxSelect;
    QHBoxLayout *hboxLayout3;
    QRadioButton *radioButtonArea;
    QRadioButton *radioButtonConnected;
    QSpacerItem *spacerItem2;
    QPushButton *buttonClear;
    QGroupBox *groupBoxOptimize;
    QGridLayout *gridLayout1;
    QPushButton *smoothButton;
    QSpinBox *spinBoxAlfa;
    QLabel *label_5;
    QLabel *label_4;
    QSpacerItem *spacerItem3;
    QProgressBar *progressBar;
    QLabel *StatusLabel;
    QGroupBox *groupBoxOption;
    QGridLayout *gridLayout2;
    QSpinBox *spinBox_2;
    QLabel *label_3;
    QSpinBox *spinBox_3;
    QSpinBox *spinBox;
    QSpacerItem *spacerItem4;

    void setupUi(QWidget *TextureEditorClass)
    {
    TextureEditorClass->setObjectName(QString::fromUtf8("TextureEditorClass"));
    vboxLayout = new QVBoxLayout(TextureEditorClass);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    tabWidget = new QTabWidget(TextureEditorClass);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    tabWidget->addTab(tab, QApplication::translate("TextureEditorClass", "Texture", 0, QApplication::UnicodeUTF8));

    vboxLayout->addWidget(tabWidget);

    groupBoxAction = new QGroupBox(TextureEditorClass);
    groupBoxAction->setObjectName(QString::fromUtf8("groupBoxAction"));
    hboxLayout = new QHBoxLayout(groupBoxAction);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(9);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    moveButton = new QPushButton(groupBoxAction);
    moveButton->setObjectName(QString::fromUtf8("moveButton"));
    moveButton->setCheckable(true);
    moveButton->setChecked(true);

    hboxLayout->addWidget(moveButton);

    selectButton = new QPushButton(groupBoxAction);
    selectButton->setObjectName(QString::fromUtf8("selectButton"));
    selectButton->setCheckable(true);

    hboxLayout->addWidget(selectButton);

    editButton = new QPushButton(groupBoxAction);
    editButton->setObjectName(QString::fromUtf8("editButton"));
    editButton->setCheckable(true);

    hboxLayout->addWidget(editButton);

    optimizeButton = new QPushButton(groupBoxAction);
    optimizeButton->setObjectName(QString::fromUtf8("optimizeButton"));
    optimizeButton->setCheckable(true);

    hboxLayout->addWidget(optimizeButton);

    optionButton = new QPushButton(groupBoxAction);
    optionButton->setObjectName(QString::fromUtf8("optionButton"));
    optionButton->setCheckable(true);

    hboxLayout->addWidget(optionButton);


    vboxLayout->addWidget(groupBoxAction);

    groupBoxUVMapEdit = new QGroupBox(TextureEditorClass);
    groupBoxUVMapEdit->setObjectName(QString::fromUtf8("groupBoxUVMapEdit"));
    gridLayout = new QGridLayout(groupBoxUVMapEdit);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(9);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    groupBox_4 = new QGroupBox(groupBoxUVMapEdit);
    groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
    vboxLayout1 = new QVBoxLayout(groupBox_4);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(9);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    clampButton = new QPushButton(groupBox_4);
    clampButton->setObjectName(QString::fromUtf8("clampButton"));

    vboxLayout1->addWidget(clampButton);

    modulusButton = new QPushButton(groupBox_4);
    modulusButton->setObjectName(QString::fromUtf8("modulusButton"));

    vboxLayout1->addWidget(modulusButton);


    gridLayout->addWidget(groupBox_4, 1, 0, 1, 1);

    groupBox_5 = new QGroupBox(groupBoxUVMapEdit);
    groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
    hboxLayout1 = new QHBoxLayout(groupBox_5);
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(9);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    spinBoxAngle = new QSpinBox(groupBox_5);
    spinBoxAngle->setObjectName(QString::fromUtf8("spinBoxAngle"));
    spinBoxAngle->setMaximum(360);
    spinBoxAngle->setMinimum(-360);

    hboxLayout1->addWidget(spinBoxAngle);

    label_2 = new QLabel(groupBox_5);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    hboxLayout1->addWidget(label_2);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    rotateButton = new QPushButton(groupBox_5);
    rotateButton->setObjectName(QString::fromUtf8("rotateButton"));

    hboxLayout1->addWidget(rotateButton);


    gridLayout->addWidget(groupBox_5, 0, 2, 1, 1);

    groupBox = new QGroupBox(groupBoxUVMapEdit);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    vboxLayout2 = new QVBoxLayout(groupBox);
    vboxLayout2->setSpacing(6);
    vboxLayout2->setMargin(9);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    radioButtonMove = new QRadioButton(groupBox);
    radioButtonMove->setObjectName(QString::fromUtf8("radioButtonMove"));
    radioButtonMove->setChecked(true);

    vboxLayout2->addWidget(radioButtonMove);

    radioButtonSelect = new QRadioButton(groupBox);
    radioButtonSelect->setObjectName(QString::fromUtf8("radioButtonSelect"));

    vboxLayout2->addWidget(radioButtonSelect);


    gridLayout->addWidget(groupBox, 0, 0, 1, 1);

    groupBox_6 = new QGroupBox(groupBoxUVMapEdit);
    groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
    hboxLayout2 = new QHBoxLayout(groupBox_6);
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(9);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    spinBoxScale = new QSpinBox(groupBox_6);
    spinBoxScale->setObjectName(QString::fromUtf8("spinBoxScale"));
    spinBoxScale->setMaximum(300);
    spinBoxScale->setMinimum(10);
    spinBoxScale->setSingleStep(5);
    spinBoxScale->setValue(100);

    hboxLayout2->addWidget(spinBoxScale);

    label = new QLabel(groupBox_6);
    label->setObjectName(QString::fromUtf8("label"));

    hboxLayout2->addWidget(label);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout2->addItem(spacerItem1);

    scaleButton = new QPushButton(groupBox_6);
    scaleButton->setObjectName(QString::fromUtf8("scaleButton"));

    hboxLayout2->addWidget(scaleButton);


    gridLayout->addWidget(groupBox_6, 1, 2, 1, 1);


    vboxLayout->addWidget(groupBoxUVMapEdit);

    groupBoxSelect = new QGroupBox(TextureEditorClass);
    groupBoxSelect->setObjectName(QString::fromUtf8("groupBoxSelect"));
    hboxLayout3 = new QHBoxLayout(groupBoxSelect);
    hboxLayout3->setSpacing(6);
    hboxLayout3->setMargin(9);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    radioButtonArea = new QRadioButton(groupBoxSelect);
    radioButtonArea->setObjectName(QString::fromUtf8("radioButtonArea"));
    radioButtonArea->setChecked(true);

    hboxLayout3->addWidget(radioButtonArea);

    radioButtonConnected = new QRadioButton(groupBoxSelect);
    radioButtonConnected->setObjectName(QString::fromUtf8("radioButtonConnected"));

    hboxLayout3->addWidget(radioButtonConnected);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout3->addItem(spacerItem2);

    buttonClear = new QPushButton(groupBoxSelect);
    buttonClear->setObjectName(QString::fromUtf8("buttonClear"));

    hboxLayout3->addWidget(buttonClear);


    vboxLayout->addWidget(groupBoxSelect);

    groupBoxOptimize = new QGroupBox(TextureEditorClass);
    groupBoxOptimize->setObjectName(QString::fromUtf8("groupBoxOptimize"));
    gridLayout1 = new QGridLayout(groupBoxOptimize);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(9);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    smoothButton = new QPushButton(groupBoxOptimize);
    smoothButton->setObjectName(QString::fromUtf8("smoothButton"));

    gridLayout1->addWidget(smoothButton, 0, 6, 1, 1);

    spinBoxAlfa = new QSpinBox(groupBoxOptimize);
    spinBoxAlfa->setObjectName(QString::fromUtf8("spinBoxAlfa"));

    gridLayout1->addWidget(spinBoxAlfa, 0, 4, 1, 1);

    label_5 = new QLabel(groupBoxOptimize);
    label_5->setObjectName(QString::fromUtf8("label_5"));

    gridLayout1->addWidget(label_5, 0, 3, 1, 1);

    label_4 = new QLabel(groupBoxOptimize);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    QFont font;
    font.setPointSize(10);
    label_4->setFont(font);

    gridLayout1->addWidget(label_4, 0, 0, 1, 2);

    spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout1->addItem(spacerItem3, 0, 7, 1, 1);

    progressBar = new QProgressBar(groupBoxOptimize);
    progressBar->setObjectName(QString::fromUtf8("progressBar"));
    progressBar->setMaximum(100);
    progressBar->setValue(0);
    progressBar->setOrientation(Qt::Horizontal);

    gridLayout1->addWidget(progressBar, 1, 4, 1, 4);

    StatusLabel = new QLabel(groupBoxOptimize);
    StatusLabel->setObjectName(QString::fromUtf8("StatusLabel"));
    QFont font1;
    font1.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font1.setPointSize(10);
    font1.setBold(false);
    font1.setItalic(false);
    font1.setUnderline(false);
    font1.setWeight(50);
    font1.setStrikeOut(false);
    StatusLabel->setFont(font1);
    StatusLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(StatusLabel, 1, 1, 1, 3);


    vboxLayout->addWidget(groupBoxOptimize);

    groupBoxOption = new QGroupBox(TextureEditorClass);
    groupBoxOption->setObjectName(QString::fromUtf8("groupBoxOption"));
    gridLayout2 = new QGridLayout(groupBoxOption);
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(9);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    spinBox_2 = new QSpinBox(groupBoxOption);
    spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));

    gridLayout2->addWidget(spinBox_2, 0, 2, 1, 1);

    label_3 = new QLabel(groupBoxOption);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout2->addWidget(label_3, 0, 0, 1, 1);

    spinBox_3 = new QSpinBox(groupBoxOption);
    spinBox_3->setObjectName(QString::fromUtf8("spinBox_3"));

    gridLayout2->addWidget(spinBox_3, 0, 3, 1, 1);

    spinBox = new QSpinBox(groupBoxOption);
    spinBox->setObjectName(QString::fromUtf8("spinBox"));

    gridLayout2->addWidget(spinBox, 0, 1, 1, 1);

    spacerItem4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout2->addItem(spacerItem4, 0, 4, 1, 1);


    vboxLayout->addWidget(groupBoxOption);


    retranslateUi(TextureEditorClass);

    QSize size(453, 954);
    size = size.expandedTo(TextureEditorClass->minimumSizeHint());
    TextureEditorClass->resize(size);


    tabWidget->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(TextureEditorClass);
    } // setupUi

    void retranslateUi(QWidget *TextureEditorClass)
    {
    TextureEditorClass->setWindowTitle(QApplication::translate("TextureEditorClass", "TextureEditor", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("TextureEditorClass", "Texture", 0, QApplication::UnicodeUTF8));
    groupBoxAction->setTitle(QApplication::translate("TextureEditorClass", " Action selection ", 0, QApplication::UnicodeUTF8));
    moveButton->setText(QApplication::translate("TextureEditorClass", "View", 0, QApplication::UnicodeUTF8));
    selectButton->setText(QApplication::translate("TextureEditorClass", "Select", 0, QApplication::UnicodeUTF8));
    editButton->setText(QApplication::translate("TextureEditorClass", "Edit", 0, QApplication::UnicodeUTF8));
    optimizeButton->setText(QApplication::translate("TextureEditorClass", "Optimize", 0, QApplication::UnicodeUTF8));
    optionButton->setText(QApplication::translate("TextureEditorClass", "Option", 0, QApplication::UnicodeUTF8));
    groupBoxUVMapEdit->setTitle(QApplication::translate("TextureEditorClass", " UV Map editing ", 0, QApplication::UnicodeUTF8));
    groupBox_4->setTitle(QApplication::translate("TextureEditorClass", " Remap ", 0, QApplication::UnicodeUTF8));
    clampButton->setText(QApplication::translate("TextureEditorClass", "Clamp", 0, QApplication::UnicodeUTF8));
    modulusButton->setText(QApplication::translate("TextureEditorClass", "Modulus", 0, QApplication::UnicodeUTF8));
    groupBox_5->setTitle(QApplication::translate("TextureEditorClass", " Rotate ", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("TextureEditorClass", "degree", 0, QApplication::UnicodeUTF8));
    rotateButton->setText(QApplication::translate("TextureEditorClass", "Apply", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("TextureEditorClass", " Mode ", 0, QApplication::UnicodeUTF8));
    radioButtonMove->setText(QApplication::translate("TextureEditorClass", "Move component", 0, QApplication::UnicodeUTF8));
    radioButtonSelect->setText(QApplication::translate("TextureEditorClass", "Select origin", 0, QApplication::UnicodeUTF8));
    groupBox_6->setTitle(QApplication::translate("TextureEditorClass", " Scale ", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("TextureEditorClass", "%", 0, QApplication::UnicodeUTF8));
    scaleButton->setText(QApplication::translate("TextureEditorClass", "Apply", 0, QApplication::UnicodeUTF8));
    groupBoxSelect->setTitle(QApplication::translate("TextureEditorClass", " Select mode  ", 0, QApplication::UnicodeUTF8));
    radioButtonArea->setText(QApplication::translate("TextureEditorClass", "Area", 0, QApplication::UnicodeUTF8));
    radioButtonConnected->setText(QApplication::translate("TextureEditorClass", "Connected component", 0, QApplication::UnicodeUTF8));
    buttonClear->setText(QApplication::translate("TextureEditorClass", "  Clear selection  ", 0, QApplication::UnicodeUTF8));
    groupBoxOptimize->setTitle(QApplication::translate("TextureEditorClass", " Optimize ", 0, QApplication::UnicodeUTF8));
    smoothButton->setText(QApplication::translate("TextureEditorClass", "Apply", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("TextureEditorClass", "Alfa:", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("TextureEditorClass", "Smooth texture cootdinates", 0, QApplication::UnicodeUTF8));
    StatusLabel->setText(QApplication::translate("TextureEditorClass", "Loading...", 0, QApplication::UnicodeUTF8));
    groupBoxOption->setTitle(QApplication::translate("TextureEditorClass", " Options ", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("TextureEditorClass", "Color:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(TextureEditorClass);
    } // retranslateUi

};

namespace Ui {
    class TextureEditorClass: public Ui_TextureEditorClass {};
} // namespace Ui

#endif // UI_TEXTUREEDITOR_H
