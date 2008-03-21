/********************************************************************************
** Form generated from reading ui file 'textureeditor.ui'
**
** Created: Fri 21. Mar 15:51:46 2008
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
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

class Ui_TextureEditorClass
{
public:
    QTabWidget *tabWidget;
    QWidget *tab;
    QGroupBox *groupBox_3;
    QPushButton *moveButton;
    QPushButton *selectButton;
    QPushButton *editButton;
    QPushButton *optionButton;
    QGroupBox *groupBox_2;
    QGroupBox *groupBox;
    QRadioButton *radioButtonMove;
    QRadioButton *radioButtonSelect;
    QGroupBox *groupBox_4;
    QPushButton *clampButton;
    QPushButton *modulusButton;
    QGroupBox *groupBox_6;
    QSpinBox *spinBoxScale;
    QLabel *label;
    QPushButton *scaleButton;
    QGroupBox *groupBox_7;
    QProgressBar *progressBar;
    QLabel *StatusLabel;
    QGroupBox *groupBox_5;
    QSpinBox *spinBoxAngle;
    QLabel *label_2;
    QPushButton *rotateButton;

    void setupUi(QWidget *TextureEditorClass)
    {
    TextureEditorClass->setObjectName(QString::fromUtf8("TextureEditorClass"));
    TextureEditorClass->setMinimumSize(QSize(331, 668));
    tabWidget = new QTabWidget(TextureEditorClass);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    tabWidget->setGeometry(QRect(10, 10, 415, 435));
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    tabWidget->addTab(tab, QApplication::translate("TextureEditorClass", "Texture", 0, QApplication::UnicodeUTF8));
    groupBox_3 = new QGroupBox(TextureEditorClass);
    groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
    groupBox_3->setGeometry(QRect(10, 450, 411, 71));
    moveButton = new QPushButton(groupBox_3);
    moveButton->setObjectName(QString::fromUtf8("moveButton"));
    moveButton->setGeometry(QRect(20, 20, 70, 40));
    moveButton->setCheckable(true);
    moveButton->setChecked(true);
    selectButton = new QPushButton(groupBox_3);
    selectButton->setObjectName(QString::fromUtf8("selectButton"));
    selectButton->setGeometry(QRect(120, 20, 70, 40));
    selectButton->setCheckable(true);
    editButton = new QPushButton(groupBox_3);
    editButton->setObjectName(QString::fromUtf8("editButton"));
    editButton->setGeometry(QRect(220, 20, 70, 40));
    editButton->setCheckable(true);
    optionButton = new QPushButton(groupBox_3);
    optionButton->setObjectName(QString::fromUtf8("optionButton"));
    optionButton->setGeometry(QRect(320, 20, 70, 40));
    optionButton->setCheckable(true);
    groupBox_2 = new QGroupBox(TextureEditorClass);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setGeometry(QRect(10, 520, 411, 241));
    groupBox = new QGroupBox(groupBox_2);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(10, 20, 131, 81));
    radioButtonMove = new QRadioButton(groupBox);
    radioButtonMove->setObjectName(QString::fromUtf8("radioButtonMove"));
    radioButtonMove->setGeometry(QRect(10, 20, 111, 18));
    radioButtonMove->setChecked(true);
    radioButtonSelect = new QRadioButton(groupBox);
    radioButtonSelect->setObjectName(QString::fromUtf8("radioButtonSelect"));
    radioButtonSelect->setGeometry(QRect(10, 50, 81, 18));
    groupBox_4 = new QGroupBox(groupBox_2);
    groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
    groupBox_4->setGeometry(QRect(10, 110, 131, 91));
    clampButton = new QPushButton(groupBox_4);
    clampButton->setObjectName(QString::fromUtf8("clampButton"));
    clampButton->setGeometry(QRect(30, 20, 75, 23));
    modulusButton = new QPushButton(groupBox_4);
    modulusButton->setObjectName(QString::fromUtf8("modulusButton"));
    modulusButton->setGeometry(QRect(30, 50, 75, 23));
    groupBox_6 = new QGroupBox(groupBox_2);
    groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
    groupBox_6->setGeometry(QRect(170, 80, 231, 51));
    spinBoxScale = new QSpinBox(groupBox_6);
    spinBoxScale->setObjectName(QString::fromUtf8("spinBoxScale"));
    spinBoxScale->setGeometry(QRect(20, 20, 44, 22));
    spinBoxScale->setMaximum(300);
    spinBoxScale->setMinimum(10);
    spinBoxScale->setSingleStep(5);
    spinBoxScale->setValue(100);
    label = new QLabel(groupBox_6);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(70, 20, 16, 16));
    scaleButton = new QPushButton(groupBox_6);
    scaleButton->setObjectName(QString::fromUtf8("scaleButton"));
    scaleButton->setGeometry(QRect(140, 20, 75, 23));
    groupBox_7 = new QGroupBox(groupBox_2);
    groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
    groupBox_7->setGeometry(QRect(170, 140, 231, 91));
    progressBar = new QProgressBar(groupBox_7);
    progressBar->setObjectName(QString::fromUtf8("progressBar"));
    progressBar->setGeometry(QRect(120, 60, 100, 20));
    progressBar->setMaximum(100);
    progressBar->setValue(0);
    progressBar->setOrientation(Qt::Horizontal);
    StatusLabel = new QLabel(groupBox_7);
    StatusLabel->setObjectName(QString::fromUtf8("StatusLabel"));
    StatusLabel->setGeometry(QRect(30, 60, 71, 21));
    QFont font;
    font.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font.setPointSize(10);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setWeight(50);
    font.setStrikeOut(false);
    StatusLabel->setFont(font);
    StatusLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    groupBox_5 = new QGroupBox(groupBox_2);
    groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
    groupBox_5->setGeometry(QRect(170, 20, 231, 51));
    spinBoxAngle = new QSpinBox(groupBox_5);
    spinBoxAngle->setObjectName(QString::fromUtf8("spinBoxAngle"));
    spinBoxAngle->setGeometry(QRect(20, 20, 44, 22));
    spinBoxAngle->setMaximum(360);
    spinBoxAngle->setMinimum(-360);
    label_2 = new QLabel(groupBox_5);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(70, 30, 46, 14));
    rotateButton = new QPushButton(groupBox_5);
    rotateButton->setObjectName(QString::fromUtf8("rotateButton"));
    rotateButton->setGeometry(QRect(140, 20, 75, 23));

    retranslateUi(TextureEditorClass);

    QSize size(434, 775);
    size = size.expandedTo(TextureEditorClass->minimumSizeHint());
    TextureEditorClass->resize(size);


    tabWidget->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(TextureEditorClass);
    } // setupUi

    void retranslateUi(QWidget *TextureEditorClass)
    {
    TextureEditorClass->setWindowTitle(QApplication::translate("TextureEditorClass", "TextureEditor", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("TextureEditorClass", "Texture", 0, QApplication::UnicodeUTF8));
    groupBox_3->setTitle(QApplication::translate("TextureEditorClass", " Action selection ", 0, QApplication::UnicodeUTF8));
    moveButton->setText(QApplication::translate("TextureEditorClass", "View", 0, QApplication::UnicodeUTF8));
    selectButton->setText(QApplication::translate("TextureEditorClass", "Select", 0, QApplication::UnicodeUTF8));
    editButton->setText(QApplication::translate("TextureEditorClass", "Edit", 0, QApplication::UnicodeUTF8));
    optionButton->setText(QApplication::translate("TextureEditorClass", "Option", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("TextureEditorClass", " UV Map editing ", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("TextureEditorClass", " Mode ", 0, QApplication::UnicodeUTF8));
    radioButtonMove->setText(QApplication::translate("TextureEditorClass", "Move component", 0, QApplication::UnicodeUTF8));
    radioButtonSelect->setText(QApplication::translate("TextureEditorClass", "Select origin", 0, QApplication::UnicodeUTF8));
    groupBox_4->setTitle(QApplication::translate("TextureEditorClass", " Remap ", 0, QApplication::UnicodeUTF8));
    clampButton->setText(QApplication::translate("TextureEditorClass", "Clamp", 0, QApplication::UnicodeUTF8));
    modulusButton->setText(QApplication::translate("TextureEditorClass", "Modulus", 0, QApplication::UnicodeUTF8));
    groupBox_6->setTitle(QApplication::translate("TextureEditorClass", " Scale ", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("TextureEditorClass", "%", 0, QApplication::UnicodeUTF8));
    scaleButton->setText(QApplication::translate("TextureEditorClass", "Apply", 0, QApplication::UnicodeUTF8));
    groupBox_7->setTitle(QApplication::translate("TextureEditorClass", " Optimize ", 0, QApplication::UnicodeUTF8));
    StatusLabel->setText(QApplication::translate("TextureEditorClass", "Loading...", 0, QApplication::UnicodeUTF8));
    groupBox_5->setTitle(QApplication::translate("TextureEditorClass", " Rotate ", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("TextureEditorClass", "degree", 0, QApplication::UnicodeUTF8));
    rotateButton->setText(QApplication::translate("TextureEditorClass", "Apply", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(TextureEditorClass);
    } // retranslateUi

};

namespace Ui {
    class TextureEditorClass: public Ui_TextureEditorClass {};
} // namespace Ui

#endif // UI_TEXTUREEDITOR_H
