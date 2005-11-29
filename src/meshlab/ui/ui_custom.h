#ifndef UI_CUSTOM_H
#define UI_CUSTOM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

class Ui_CDialogCustom
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *labelBCB;
    QPushButton *ButtonBCB;
    QHBoxLayout *hboxLayout1;
    QLabel *labelBCT;
    QPushButton *ButtonBCT;
    QHBoxLayout *hboxLayout2;
    QLabel *labelLC;
    QPushButton *ButtonLC;
    QHBoxLayout *hboxLayout3;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *CDialogCustom)
    {
    CDialogCustom->setObjectName(QString::fromUtf8("CDialogCustom"));
    CDialogCustom->resize(QSize(197, 159).expandedTo(CDialogCustom->minimumSizeHint()));
    vboxLayout = new QVBoxLayout(CDialogCustom);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(8);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    labelBCB = new QLabel(CDialogCustom);
    labelBCB->setObjectName(QString::fromUtf8("labelBCB"));
    QFont font;
    font.setFamily(QString::fromUtf8("MS Shell Dlg"));
    font.setPointSize(11);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setWeight(50);
    font.setStrikeOut(false);
    labelBCB->setFont(font);

    hboxLayout->addWidget(labelBCB);

    ButtonBCB = new QPushButton(CDialogCustom);
    ButtonBCB->setObjectName(QString::fromUtf8("ButtonBCB"));
    QSizePolicy sizePolicy((QSizePolicy::Policy)1, (QSizePolicy::Policy)0);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(ButtonBCB->sizePolicy().hasHeightForWidth());
    ButtonBCB->setSizePolicy(sizePolicy);
    ButtonBCB->setMinimumSize(QSize(21, 21));
    ButtonBCB->setMaximumSize(QSize(21, 21));

    hboxLayout->addWidget(ButtonBCB);


    vboxLayout->addLayout(hboxLayout);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    labelBCT = new QLabel(CDialogCustom);
    labelBCT->setObjectName(QString::fromUtf8("labelBCT"));
    QFont font1;
    font1.setFamily(QString::fromUtf8("MS Shell Dlg"));
    font1.setPointSize(11);
    font1.setBold(false);
    font1.setItalic(false);
    font1.setUnderline(false);
    font1.setWeight(50);
    font1.setStrikeOut(false);
    labelBCT->setFont(font1);

    hboxLayout1->addWidget(labelBCT);

    ButtonBCT = new QPushButton(CDialogCustom);
    ButtonBCT->setObjectName(QString::fromUtf8("ButtonBCT"));
    ButtonBCT->setMinimumSize(QSize(21, 21));
    ButtonBCT->setMaximumSize(QSize(21, 21));

    hboxLayout1->addWidget(ButtonBCT);


    vboxLayout->addLayout(hboxLayout1);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    labelLC = new QLabel(CDialogCustom);
    labelLC->setObjectName(QString::fromUtf8("labelLC"));
    QFont font2;
    font2.setFamily(QString::fromUtf8("MS Shell Dlg"));
    font2.setPointSize(11);
    font2.setBold(false);
    font2.setItalic(false);
    font2.setUnderline(false);
    font2.setWeight(50);
    font2.setStrikeOut(false);
    labelLC->setFont(font2);

    hboxLayout2->addWidget(labelLC);

    ButtonLC = new QPushButton(CDialogCustom);
    ButtonLC->setObjectName(QString::fromUtf8("ButtonLC"));
    ButtonLC->setMinimumSize(QSize(21, 21));
    ButtonLC->setMaximumSize(QSize(21, 21));

    hboxLayout2->addWidget(ButtonLC);


    vboxLayout->addLayout(hboxLayout2);

    hboxLayout3 = new QHBoxLayout();
    hboxLayout3->setSpacing(6);
    hboxLayout3->setMargin(0);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout3->addItem(spacerItem);

    okButton = new QPushButton(CDialogCustom);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout3->addWidget(okButton);

    cancelButton = new QPushButton(CDialogCustom);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout3->addWidget(cancelButton);


    vboxLayout->addLayout(hboxLayout3);

    retranslateUi(CDialogCustom);
    QObject::connect(okButton, SIGNAL(clicked()), CDialogCustom, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), CDialogCustom, SLOT(reject()));

    QMetaObject::connectSlotsByName(CDialogCustom);
    } // setupUi

    void retranslateUi(QDialog *CDialogCustom)
    {
    CDialogCustom->setWindowTitle(QApplication::translate("CDialogCustom", "Customize", 0, QApplication::UnicodeUTF8));
    labelBCB->setText(QApplication::translate("CDialogCustom", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Background Color Bottom</p></body></html>", 0, QApplication::UnicodeUTF8));
    ButtonBCB->setText(QApplication::translate("CDialogCustom", "", 0, QApplication::UnicodeUTF8));
    labelBCT->setText(QApplication::translate("CDialogCustom", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Background Color Top</p></body></html>", 0, QApplication::UnicodeUTF8));
    ButtonBCT->setText(QApplication::translate("CDialogCustom", "", 0, QApplication::UnicodeUTF8));
    labelLC->setText(QApplication::translate("CDialogCustom", "Log Color", 0, QApplication::UnicodeUTF8));
    ButtonLC->setText(QApplication::translate("CDialogCustom", "", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("CDialogCustom", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("CDialogCustom", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(CDialogCustom);
    } // retranslateUi

};

namespace Ui {
    class CDialogCustom: public Ui_CDialogCustom {};
} // namespace Ui

#endif // UI_CUSTOM_H
