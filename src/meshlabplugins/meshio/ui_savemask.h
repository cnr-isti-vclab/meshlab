#ifndef UI_SAVEMASK_H
#define UI_SAVEMASK_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_Dialog
{
public:
    QWidget *horizontalLayout;
    QWidget *layoutWidget;
    QWidget *widget;
    QWidget *widget_2;
    QWidget *widget_3;
    QGroupBox *groupBox_2;
    QVBoxLayout *vboxLayout;
    QRadioButton *binaryButton;
    QRadioButton *asciiButton;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout;
    QCheckBox *faceCheck;
    QCheckBox *normalCheck;
    QCheckBox *materialCheck;
    QCheckBox *textureCheck;
    QCheckBox *vertexCheck;
    QCheckBox *colorVertexCheck;

    void setupUi(QDialog *Dialog)
    {
    Dialog->setObjectName(QString::fromUtf8("Dialog"));
    Dialog->resize(QSize(357, 150).expandedTo(Dialog->minimumSizeHint()));
    horizontalLayout = new QWidget(Dialog);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setGeometry(QRect(20, 150, 361, 31));
    layoutWidget = new QWidget(Dialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(20, 250, 351, 33));
    widget = new QWidget(Dialog);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(10, 10, 341, 241));
    widget_2 = new QWidget(Dialog);
    widget_2->setObjectName(QString::fromUtf8("widget_2"));
    widget_2->setGeometry(QRect(11, 94, 339, 125));
    widget_3 = new QWidget(Dialog);
    widget_3->setObjectName(QString::fromUtf8("widget_3"));
    widget_3->setGeometry(QRect(11, 225, 339, 25));
    groupBox_2 = new QGroupBox(Dialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setGeometry(QRect(250, 0, 95, 78));
    vboxLayout = new QVBoxLayout(groupBox_2);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(8);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    binaryButton = new QRadioButton(groupBox_2);
    binaryButton->setObjectName(QString::fromUtf8("binaryButton"));

    vboxLayout->addWidget(binaryButton);

    asciiButton = new QRadioButton(groupBox_2);
    asciiButton->setObjectName(QString::fromUtf8("asciiButton"));

    vboxLayout->addWidget(asciiButton);

    okButton = new QPushButton(Dialog);
    okButton->setObjectName(QString::fromUtf8("okButton"));
    okButton->setGeometry(QRect(250, 90, 101, 23));
    cancelButton = new QPushButton(Dialog);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
    cancelButton->setGeometry(QRect(250, 120, 101, 23));
    groupBox_3 = new QGroupBox(Dialog);
    groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
    groupBox_3->setGeometry(QRect(10, 0, 231, 141));
    gridLayout = new QGridLayout(groupBox_3);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(8);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    faceCheck = new QCheckBox(groupBox_3);
    faceCheck->setObjectName(QString::fromUtf8("faceCheck"));

    gridLayout->addWidget(faceCheck, 2, 1, 1, 1);

    normalCheck = new QCheckBox(groupBox_3);
    normalCheck->setObjectName(QString::fromUtf8("normalCheck"));

    gridLayout->addWidget(normalCheck, 2, 0, 1, 1);

    materialCheck = new QCheckBox(groupBox_3);
    materialCheck->setObjectName(QString::fromUtf8("materialCheck"));

    gridLayout->addWidget(materialCheck, 0, 1, 1, 1);

    textureCheck = new QCheckBox(groupBox_3);
    textureCheck->setObjectName(QString::fromUtf8("textureCheck"));

    gridLayout->addWidget(textureCheck, 1, 1, 1, 1);

    vertexCheck = new QCheckBox(groupBox_3);
    vertexCheck->setObjectName(QString::fromUtf8("vertexCheck"));

    gridLayout->addWidget(vertexCheck, 0, 0, 1, 1);

    colorVertexCheck = new QCheckBox(groupBox_3);
    colorVertexCheck->setObjectName(QString::fromUtf8("colorVertexCheck"));

    gridLayout->addWidget(colorVertexCheck, 1, 0, 1, 1);

    retranslateUi(Dialog);
    QObject::connect(cancelButton, SIGNAL(clicked()), Dialog, SLOT(reject()));
    QObject::connect(okButton, SIGNAL(clicked()), Dialog, SLOT(accept()));

    QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("Dialog", "Type Save", 0, QApplication::UnicodeUTF8));
    binaryButton->setText(QApplication::translate("Dialog", "Binary Obj", 0, QApplication::UnicodeUTF8));
    asciiButton->setText(QApplication::translate("Dialog", "ASCII Obj", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("Dialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("Dialog", "Cancel", 0, QApplication::UnicodeUTF8));
    groupBox_3->setTitle(QApplication::translate("Dialog", "Information Save", 0, QApplication::UnicodeUTF8));
    faceCheck->setText(QApplication::translate("Dialog", "Faces", 0, QApplication::UnicodeUTF8));
    normalCheck->setText(QApplication::translate("Dialog", "Normals Vertex", 0, QApplication::UnicodeUTF8));
    materialCheck->setText(QApplication::translate("Dialog", "Materials", 0, QApplication::UnicodeUTF8));
    textureCheck->setText(QApplication::translate("Dialog", "Texture Coords", 0, QApplication::UnicodeUTF8));
    vertexCheck->setText(QApplication::translate("Dialog", "Vertexs", 0, QApplication::UnicodeUTF8));
    colorVertexCheck->setText(QApplication::translate("Dialog", "Vertexs Color", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Dialog);
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

#endif // UI_SAVEMASK_H
