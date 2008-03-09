/********************************************************************************
** Form generated from reading ui file 'textureeditor.ui'
**
** Created: Sun 9. Mar 11:18:35 2008
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
#include <QtGui/QDial>
#include <QtGui/QDoubleSpinBox>
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
    QGroupBox *groupBox;
    QRadioButton *faceRB;
    QRadioButton *vertRB;
    QRadioButton *remapRB;
    QGroupBox *groupBox_2;
    QPushButton *applyButton;
    QProgressBar *progressBar;
    QLabel *StatusLabel;
    QTabWidget *tabWidgetEdit;
    QWidget *tab_point;
    QLabel *label_11;
    QDoubleSpinBox *SpinBoxU;
    QDoubleSpinBox *SpinBoxV;
    QLabel *label_9;
    QLabel *label_10;
    QWidget *tab_comp;
    QTabWidget *tabWidget_2;
    QWidget *tab_2;
    QLabel *label_19;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_2;
    QSpinBox *spinBoxX;
    QSpinBox *spinBoxY;
    QWidget *tab_3;
    QLabel *label_15;
    QLabel *label_14;
    QPushButton *ApplyRotate;
    QDial *angle;
    QSpinBox *spinBoxAngle;
    QWidget *tab_4;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_16;
    QPushButton *ApplyScale;
    QSpinBox *spinBoxScale;
    QLabel *label_6;
    QLabel *LabelFaceNum;
    QWidget *tab_smooth;
    QTabWidget *tabWidget;
    QWidget *tab;

    void setupUi(QWidget *TextureEditorClass)
    {
    TextureEditorClass->setObjectName(QString::fromUtf8("TextureEditorClass"));
    TextureEditorClass->setMinimumSize(QSize(331, 668));
    groupBox = new QGroupBox(TextureEditorClass);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(10, 350, 311, 41));
    faceRB = new QRadioButton(groupBox);
    faceRB->setObjectName(QString::fromUtf8("faceRB"));
    faceRB->setGeometry(QRect(12, 15, 61, 19));
    faceRB->setChecked(true);
    vertRB = new QRadioButton(groupBox);
    vertRB->setObjectName(QString::fromUtf8("vertRB"));
    vertRB->setGeometry(QRect(106, 15, 81, 19));
    remapRB = new QRadioButton(groupBox);
    remapRB->setObjectName(QString::fromUtf8("remapRB"));
    remapRB->setGeometry(QRect(199, 15, 81, 19));
    groupBox_2 = new QGroupBox(TextureEditorClass);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setGeometry(QRect(10, 398, 311, 261));
    applyButton = new QPushButton(groupBox_2);
    applyButton->setObjectName(QString::fromUtf8("applyButton"));
    applyButton->setGeometry(QRect(10, 230, 100, 25));
    progressBar = new QProgressBar(groupBox_2);
    progressBar->setObjectName(QString::fromUtf8("progressBar"));
    progressBar->setGeometry(QRect(200, 230, 100, 20));
    progressBar->setMaximum(100);
    progressBar->setValue(0);
    progressBar->setOrientation(Qt::Horizontal);
    StatusLabel = new QLabel(groupBox_2);
    StatusLabel->setObjectName(QString::fromUtf8("StatusLabel"));
    StatusLabel->setGeometry(QRect(120, 230, 71, 21));
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
    tabWidgetEdit = new QTabWidget(groupBox_2);
    tabWidgetEdit->setObjectName(QString::fromUtf8("tabWidgetEdit"));
    tabWidgetEdit->setGeometry(QRect(10, 20, 293, 201));
    tab_point = new QWidget();
    tab_point->setObjectName(QString::fromUtf8("tab_point"));
    label_11 = new QLabel(tab_point);
    label_11->setObjectName(QString::fromUtf8("label_11"));
    label_11->setGeometry(QRect(10, 20, 101, 17));
    QFont font1;
    font1.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font1.setPointSize(10);
    font1.setBold(false);
    font1.setItalic(false);
    font1.setUnderline(false);
    font1.setWeight(50);
    font1.setStrikeOut(false);
    label_11->setFont(font1);
    SpinBoxU = new QDoubleSpinBox(tab_point);
    SpinBoxU->setObjectName(QString::fromUtf8("SpinBoxU"));
    SpinBoxU->setEnabled(false);
    SpinBoxU->setGeometry(QRect(190, 20, 51, 22));
    SpinBoxU->setMaximum(10);
    SpinBoxU->setMinimum(-10);
    SpinBoxU->setSingleStep(0.01);
    SpinBoxV = new QDoubleSpinBox(tab_point);
    SpinBoxV->setObjectName(QString::fromUtf8("SpinBoxV"));
    SpinBoxV->setEnabled(false);
    SpinBoxV->setGeometry(QRect(190, 60, 51, 22));
    SpinBoxV->setMaximum(10);
    SpinBoxV->setMinimum(-10);
    SpinBoxV->setSingleStep(0.01);
    label_9 = new QLabel(tab_point);
    label_9->setObjectName(QString::fromUtf8("label_9"));
    label_9->setGeometry(QRect(160, 20, 16, 16));
    QFont font2;
    font2.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font2.setPointSize(10);
    font2.setBold(false);
    font2.setItalic(false);
    font2.setUnderline(false);
    font2.setWeight(50);
    font2.setStrikeOut(false);
    label_9->setFont(font2);
    label_10 = new QLabel(tab_point);
    label_10->setObjectName(QString::fromUtf8("label_10"));
    label_10->setGeometry(QRect(160, 60, 16, 16));
    QFont font3;
    font3.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font3.setPointSize(10);
    font3.setBold(false);
    font3.setItalic(false);
    font3.setUnderline(false);
    font3.setWeight(50);
    font3.setStrikeOut(false);
    label_10->setFont(font3);
    tabWidgetEdit->addTab(tab_point, QApplication::translate("TextureEditorClass", "Single Points", 0, QApplication::UnicodeUTF8));
    tab_comp = new QWidget();
    tab_comp->setObjectName(QString::fromUtf8("tab_comp"));
    tabWidget_2 = new QTabWidget(tab_comp);
    tabWidget_2->setObjectName(QString::fromUtf8("tabWidget_2"));
    tabWidget_2->setWindowModality(Qt::NonModal);
    tabWidget_2->setGeometry(QRect(10, 40, 272, 131));
    tabWidget_2->setUsesScrollButtons(false);
    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    label_19 = new QLabel(tab_2);
    label_19->setObjectName(QString::fromUtf8("label_19"));
    label_19->setGeometry(QRect(10, 10, 193, 17));
    QFont font4;
    font4.setPointSize(10);
    label_19->setFont(font4);
    label_12 = new QLabel(tab_2);
    label_12->setObjectName(QString::fromUtf8("label_12"));
    label_12->setGeometry(QRect(130, 40, 16, 16));
    QFont font5;
    font5.setPointSize(10);
    label_12->setFont(font5);
    label_13 = new QLabel(tab_2);
    label_13->setObjectName(QString::fromUtf8("label_13"));
    label_13->setGeometry(QRect(130, 70, 16, 16));
    QFont font6;
    font6.setPointSize(10);
    label_13->setFont(font6);
    label_2 = new QLabel(tab_2);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(10, 50, 98, 17));
    QFont font7;
    font7.setPointSize(10);
    label_2->setFont(font7);
    spinBoxX = new QSpinBox(tab_2);
    spinBoxX->setObjectName(QString::fromUtf8("spinBoxX"));
    spinBoxX->setEnabled(false);
    spinBoxX->setGeometry(QRect(160, 40, 46, 22));
    spinBoxX->setMaximum(300);
    spinBoxX->setMinimum(-300);
    spinBoxY = new QSpinBox(tab_2);
    spinBoxY->setObjectName(QString::fromUtf8("spinBoxY"));
    spinBoxY->setEnabled(false);
    spinBoxY->setGeometry(QRect(160, 70, 46, 22));
    spinBoxY->setMaximum(300);
    spinBoxY->setMinimum(-300);
    tabWidget_2->addTab(tab_2, QApplication::translate("TextureEditorClass", "      Move     ", 0, QApplication::UnicodeUTF8));
    tab_3 = new QWidget();
    tab_3->setObjectName(QString::fromUtf8("tab_3"));
    label_15 = new QLabel(tab_3);
    label_15->setObjectName(QString::fromUtf8("label_15"));
    label_15->setGeometry(QRect(10, 10, 193, 17));
    QFont font8;
    font8.setPointSize(10);
    label_15->setFont(font8);
    label_14 = new QLabel(tab_3);
    label_14->setObjectName(QString::fromUtf8("label_14"));
    label_14->setGeometry(QRect(10, 40, 41, 17));
    QFont font9;
    font9.setPointSize(10);
    label_14->setFont(font9);
    ApplyRotate = new QPushButton(tab_3);
    ApplyRotate->setObjectName(QString::fromUtf8("ApplyRotate"));
    ApplyRotate->setEnabled(false);
    ApplyRotate->setGeometry(QRect(180, 70, 75, 24));
    angle = new QDial(tab_3);
    angle->setObjectName(QString::fromUtf8("angle"));
    angle->setWindowModality(Qt::NonModal);
    angle->setEnabled(true);
    angle->setGeometry(QRect(70, 40, 55, 55));
    angle->setMinimum(-360);
    angle->setMaximum(360);
    angle->setPageStep(45);
    angle->setValue(0);
    angle->setSliderPosition(0);
    angle->setTracking(true);
    angle->setOrientation(Qt::Horizontal);
    angle->setInvertedAppearance(false);
    angle->setWrapping(true);
    angle->setNotchTarget(10);
    angle->setNotchesVisible(true);
    spinBoxAngle = new QSpinBox(tab_3);
    spinBoxAngle->setObjectName(QString::fromUtf8("spinBoxAngle"));
    spinBoxAngle->setEnabled(true);
    spinBoxAngle->setGeometry(QRect(10, 70, 46, 22));
    spinBoxAngle->setMaximum(360);
    spinBoxAngle->setMinimum(-360);
    tabWidget_2->addTab(tab_3, QApplication::translate("TextureEditorClass", "       Rotate      ", 0, QApplication::UnicodeUTF8));
    tab_4 = new QWidget();
    tab_4->setObjectName(QString::fromUtf8("tab_4"));
    label_17 = new QLabel(tab_4);
    label_17->setObjectName(QString::fromUtf8("label_17"));
    label_17->setGeometry(QRect(120, 50, 16, 17));
    QFont font10;
    font10.setPointSize(10);
    label_17->setFont(font10);
    label_18 = new QLabel(tab_4);
    label_18->setObjectName(QString::fromUtf8("label_18"));
    label_18->setGeometry(QRect(20, 50, 41, 17));
    QFont font11;
    font11.setPointSize(10);
    label_18->setFont(font11);
    label_16 = new QLabel(tab_4);
    label_16->setObjectName(QString::fromUtf8("label_16"));
    label_16->setGeometry(QRect(10, 10, 193, 17));
    QFont font12;
    font12.setPointSize(10);
    label_16->setFont(font12);
    ApplyScale = new QPushButton(tab_4);
    ApplyScale->setObjectName(QString::fromUtf8("ApplyScale"));
    ApplyScale->setEnabled(false);
    ApplyScale->setGeometry(QRect(180, 70, 75, 24));
    spinBoxScale = new QSpinBox(tab_4);
    spinBoxScale->setObjectName(QString::fromUtf8("spinBoxScale"));
    spinBoxScale->setEnabled(true);
    spinBoxScale->setGeometry(QRect(70, 50, 46, 22));
    spinBoxScale->setMaximum(200);
    spinBoxScale->setMinimum(10);
    spinBoxScale->setSingleStep(5);
    spinBoxScale->setValue(100);
    tabWidget_2->addTab(tab_4, QApplication::translate("TextureEditorClass", "       Scale        ", 0, QApplication::UnicodeUTF8));
    label_6 = new QLabel(tab_comp);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setGeometry(QRect(20, 10, 111, 17));
    QFont font13;
    font13.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font13.setPointSize(10);
    font13.setBold(false);
    font13.setItalic(false);
    font13.setUnderline(false);
    font13.setWeight(50);
    font13.setStrikeOut(false);
    label_6->setFont(font13);
    LabelFaceNum = new QLabel(tab_comp);
    LabelFaceNum->setObjectName(QString::fromUtf8("LabelFaceNum"));
    LabelFaceNum->setGeometry(QRect(140, 10, 61, 17));
    QFont font14;
    font14.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font14.setPointSize(10);
    font14.setBold(false);
    font14.setItalic(false);
    font14.setUnderline(false);
    font14.setWeight(50);
    font14.setStrikeOut(false);
    LabelFaceNum->setFont(font14);
    tabWidgetEdit->addTab(tab_comp, QApplication::translate("TextureEditorClass", "Connected component", 0, QApplication::UnicodeUTF8));
    tab_smooth = new QWidget();
    tab_smooth->setObjectName(QString::fromUtf8("tab_smooth"));
    tabWidgetEdit->addTab(tab_smooth, QApplication::translate("TextureEditorClass", "   Smooth   ", 0, QApplication::UnicodeUTF8));
    tabWidget = new QTabWidget(TextureEditorClass);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    tabWidget->setGeometry(QRect(10, 10, 315, 335));
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    tabWidget->addTab(tab, QApplication::translate("TextureEditorClass", "Texture", 0, QApplication::UnicodeUTF8));

    retranslateUi(TextureEditorClass);

    QSize size(331, 668);
    size = size.expandedTo(TextureEditorClass->minimumSizeHint());
    TextureEditorClass->resize(size);

    QObject::connect(spinBoxAngle, SIGNAL(valueChanged(int)), angle, SLOT(setValue(int)));
    QObject::connect(angle, SIGNAL(valueChanged(int)), spinBoxAngle, SLOT(setValue(int)));

    tabWidgetEdit->setCurrentIndex(0);
    tabWidget_2->setCurrentIndex(0);
    tabWidget->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(TextureEditorClass);
    } // setupUi

    void retranslateUi(QWidget *TextureEditorClass)
    {
    TextureEditorClass->setWindowTitle(QApplication::translate("TextureEditorClass", "TextureEditor", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("TextureEditorClass", " Border managment ", 0, QApplication::UnicodeUTF8));
    faceRB->setText(QApplication::translate("TextureEditorClass", "Repeat", 0, QApplication::UnicodeUTF8));
    vertRB->setText(QApplication::translate("TextureEditorClass", "Clamp", 0, QApplication::UnicodeUTF8));
    remapRB->setText(QApplication::translate("TextureEditorClass", "Modulus", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("TextureEditorClass", " UV Map editing ", 0, QApplication::UnicodeUTF8));
    applyButton->setText(QApplication::translate("TextureEditorClass", "Save Changes", 0, QApplication::UnicodeUTF8));
    StatusLabel->setText(QApplication::translate("TextureEditorClass", "Loading...", 0, QApplication::UnicodeUTF8));
    label_11->setText(QApplication::translate("TextureEditorClass", "UV Coordinates:", 0, QApplication::UnicodeUTF8));
    label_9->setText(QApplication::translate("TextureEditorClass", "U:", 0, QApplication::UnicodeUTF8));
    label_10->setText(QApplication::translate("TextureEditorClass", "V:", 0, QApplication::UnicodeUTF8));
    tabWidgetEdit->setTabText(tabWidgetEdit->indexOf(tab_point), QApplication::translate("TextureEditorClass", "Single Points", 0, QApplication::UnicodeUTF8));
    label_19->setText(QApplication::translate("TextureEditorClass", "Move selected component", 0, QApplication::UnicodeUTF8));
    label_12->setText(QApplication::translate("TextureEditorClass", "X =", 0, QApplication::UnicodeUTF8));
    label_13->setText(QApplication::translate("TextureEditorClass", "Y =", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("TextureEditorClass", "Relative position:", 0, QApplication::UnicodeUTF8));
    tabWidget_2->setTabText(tabWidget_2->indexOf(tab_2), QApplication::translate("TextureEditorClass", "      Move     ", 0, QApplication::UnicodeUTF8));
    label_15->setText(QApplication::translate("TextureEditorClass", "Rotate around the selected vertex", 0, QApplication::UnicodeUTF8));
    label_14->setText(QApplication::translate("TextureEditorClass", "Angle:", 0, QApplication::UnicodeUTF8));
    ApplyRotate->setText(QApplication::translate("TextureEditorClass", "Apply", 0, QApplication::UnicodeUTF8));
    tabWidget_2->setTabText(tabWidget_2->indexOf(tab_3), QApplication::translate("TextureEditorClass", "       Rotate      ", 0, QApplication::UnicodeUTF8));
    label_17->setText(QApplication::translate("TextureEditorClass", "%", 0, QApplication::UnicodeUTF8));
    label_18->setText(QApplication::translate("TextureEditorClass", "Scale:", 0, QApplication::UnicodeUTF8));
    label_16->setText(QApplication::translate("TextureEditorClass", "Scale from the selected vertex", 0, QApplication::UnicodeUTF8));
    ApplyScale->setText(QApplication::translate("TextureEditorClass", "Apply", 0, QApplication::UnicodeUTF8));
    tabWidget_2->setTabText(tabWidget_2->indexOf(tab_4), QApplication::translate("TextureEditorClass", "       Scale        ", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("TextureEditorClass", "Selected Vertexes:", 0, QApplication::UnicodeUTF8));
    LabelFaceNum->setText(QApplication::translate("TextureEditorClass", "0", 0, QApplication::UnicodeUTF8));
    tabWidgetEdit->setTabText(tabWidgetEdit->indexOf(tab_comp), QApplication::translate("TextureEditorClass", "Connected component", 0, QApplication::UnicodeUTF8));
    tabWidgetEdit->setTabText(tabWidgetEdit->indexOf(tab_smooth), QApplication::translate("TextureEditorClass", "   Smooth   ", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("TextureEditorClass", "Texture", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(TextureEditorClass);
    } // retranslateUi

};

namespace Ui {
    class TextureEditorClass: public Ui_TextureEditorClass {};
} // namespace Ui

#endif // UI_TEXTUREEDITOR_H
