/********************************************************************************
** Form generated from reading ui file 'PaintToolbox.ui'
**
** Created: Wed Jan 17 18:05:13 2007
**      by: Qt User Interface Compiler version 4.2.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PAINTTOOLBOX_H
#define UI_PAINTTOOLBOX_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>
#include "colorwid.h"

class Ui_PaintToolbox
{
public:
    QGridLayout *gridLayout;
    QFrame *top_frame;
    QToolButton *pen_button;
    QToolButton *fill_button;
    QFrame *frame_3;
    QFrame *color_frame;
    ColorWid *back;
    ColorWid *front;
    QToolButton *set_bw;
    QToolButton *switch_me;
    QFrame *bottom_frame;
    QGridLayout *gridLayout1;
    QLabel *label_pen;
    QComboBox *pen_type;
    QDoubleSpinBox *pen_radius;
    QSpacerItem *spacerItem;
    QSlider *deck_slider;
    QComboBox *pen_modus;
    QLabel *label_penmodus;
    QLabel *label_deck;
    QSpinBox *deck_box;

    void setupUi(QWidget *PaintToolbox)
    {
    PaintToolbox->setObjectName(QString::fromUtf8("PaintToolbox"));
    gridLayout = new QGridLayout(PaintToolbox);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(9);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    top_frame = new QFrame(PaintToolbox);
    top_frame->setObjectName(QString::fromUtf8("top_frame"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(top_frame->sizePolicy().hasHeightForWidth());
    top_frame->setSizePolicy(sizePolicy);
    top_frame->setMinimumSize(QSize(16, 100));
    top_frame->setFrameShape(QFrame::StyledPanel);
    top_frame->setFrameShadow(QFrame::Raised);
    pen_button = new QToolButton(top_frame);
    pen_button->setObjectName(QString::fromUtf8("pen_button"));
    pen_button->setGeometry(QRect(10, 10, 48, 48));
    pen_button->setIcon(QIcon(QString::fromUtf8(":/images/pinsel.png")));
    pen_button->setIconSize(QSize(48, 48));
    pen_button->setCheckable(true);
    pen_button->setAutoExclusive(true);
    pen_button->setAutoRaise(false);
    fill_button = new QToolButton(top_frame);
    fill_button->setObjectName(QString::fromUtf8("fill_button"));
    fill_button->setGeometry(QRect(70, 10, 48, 48));
    fill_button->setIcon(QIcon(QString::fromUtf8(":/images/fill.png")));
    fill_button->setIconSize(QSize(48, 48));
    fill_button->setCheckable(true);
    fill_button->setAutoExclusive(true);
    fill_button->setAutoRaise(false);

    gridLayout->addWidget(top_frame, 0, 0, 1, 2);

    frame_3 = new QFrame(PaintToolbox);
    frame_3->setObjectName(QString::fromUtf8("frame_3"));
    frame_3->setFrameShape(QFrame::StyledPanel);
    frame_3->setFrameShadow(QFrame::Raised);

    gridLayout->addWidget(frame_3, 1, 1, 1, 1);

    color_frame = new QFrame(PaintToolbox);
    color_frame->setObjectName(QString::fromUtf8("color_frame"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(color_frame->sizePolicy().hasHeightForWidth());
    color_frame->setSizePolicy(sizePolicy1);
    color_frame->setMinimumSize(QSize(70, 70));
    color_frame->setMaximumSize(QSize(16777215, 70));
    color_frame->setFrameShape(QFrame::StyledPanel);
    color_frame->setFrameShadow(QFrame::Raised);
    back = new ColorWid(color_frame);
    back->setObjectName(QString::fromUtf8("back"));
    back->setGeometry(QRect(10, 10, 40, 40));
    back->setAutoFillBackground(true);
    front = new ColorWid(color_frame);
    front->setObjectName(QString::fromUtf8("front"));
    front->setGeometry(QRect(60, 10, 40, 40));
    front->setAutoFillBackground(true);
    set_bw = new QToolButton(color_frame);
    set_bw->setObjectName(QString::fromUtf8("set_bw"));
    set_bw->setGeometry(QRect(100, 10, 25, 24));
    set_bw->setIcon(QIcon(QString::fromUtf8(":/images/bw.png")));
    set_bw->setIconSize(QSize(15, 15));
    set_bw->setAutoRaise(true);
    switch_me = new QToolButton(color_frame);
    switch_me->setObjectName(QString::fromUtf8("switch_me"));
    switch_me->setGeometry(QRect(100, 40, 25, 24));
    switch_me->setIcon(QIcon(QString::fromUtf8(":/images/switch.png")));
    switch_me->setIconSize(QSize(15, 15));
    switch_me->setAutoRaise(true);

    gridLayout->addWidget(color_frame, 1, 0, 1, 1);

    bottom_frame = new QFrame(PaintToolbox);
    bottom_frame->setObjectName(QString::fromUtf8("bottom_frame"));
    bottom_frame->setFrameShape(QFrame::StyledPanel);
    bottom_frame->setFrameShadow(QFrame::Raised);
    gridLayout1 = new QGridLayout(bottom_frame);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(9);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    label_pen = new QLabel(bottom_frame);
    label_pen->setObjectName(QString::fromUtf8("label_pen"));

    gridLayout1->addWidget(label_pen, 2, 0, 1, 1);

    pen_type = new QComboBox(bottom_frame);
    pen_type->setObjectName(QString::fromUtf8("pen_type"));

    gridLayout1->addWidget(pen_type, 2, 2, 1, 2);

    pen_radius = new QDoubleSpinBox(bottom_frame);
    pen_radius->setObjectName(QString::fromUtf8("pen_radius"));
    pen_radius->setDecimals(1);
    pen_radius->setMaximum(70);
    pen_radius->setMinimum(0.1);
    pen_radius->setSingleStep(0.1);
    pen_radius->setValue(25);

    gridLayout1->addWidget(pen_radius, 2, 1, 1, 1);

    spacerItem = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

    gridLayout1->addItem(spacerItem, 3, 2, 1, 1);

    deck_slider = new QSlider(bottom_frame);
    deck_slider->setObjectName(QString::fromUtf8("deck_slider"));
    deck_slider->setMaximum(100);
    deck_slider->setValue(100);
    deck_slider->setOrientation(Qt::Horizontal);

    gridLayout1->addWidget(deck_slider, 0, 1, 1, 2);

    pen_modus = new QComboBox(bottom_frame);
    pen_modus->setObjectName(QString::fromUtf8("pen_modus"));

    gridLayout1->addWidget(pen_modus, 1, 1, 1, 3);

    label_penmodus = new QLabel(bottom_frame);
    label_penmodus->setObjectName(QString::fromUtf8("label_penmodus"));

    gridLayout1->addWidget(label_penmodus, 1, 0, 1, 1);

    label_deck = new QLabel(bottom_frame);
    label_deck->setObjectName(QString::fromUtf8("label_deck"));

    gridLayout1->addWidget(label_deck, 0, 0, 1, 1);

    deck_box = new QSpinBox(bottom_frame);
    deck_box->setObjectName(QString::fromUtf8("deck_box"));
    deck_box->setMaximum(100);
    deck_box->setValue(100);

    gridLayout1->addWidget(deck_box, 0, 3, 1, 1);


    gridLayout->addWidget(bottom_frame, 2, 0, 1, 2);


    retranslateUi(PaintToolbox);

    QSize size(307, 414);
    size = size.expandedTo(PaintToolbox->minimumSizeHint());
    PaintToolbox->resize(size);


    QMetaObject::connectSlotsByName(PaintToolbox);
    } // setupUi

    void retranslateUi(QWidget *PaintToolbox)
    {
    PaintToolbox->setWindowTitle(QApplication::translate("PaintToolbox", "Vector Painting", 0, QApplication::UnicodeUTF8));
    pen_button->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    fill_button->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    set_bw->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    switch_me->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    label_pen->setText(QApplication::translate("PaintToolbox", "Pen", 0, QApplication::UnicodeUTF8));
    pen_type->clear();
    pen_type->addItem(QApplication::translate("PaintToolbox", "pixel", 0, QApplication::UnicodeUTF8));
    pen_type->addItem(QApplication::translate("PaintToolbox", "percentuale", 0, QApplication::UnicodeUTF8));
    label_penmodus->setText(QApplication::translate("PaintToolbox", "Modus", 0, QApplication::UnicodeUTF8));
    label_deck->setText(QApplication::translate("PaintToolbox", "Opacity", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(PaintToolbox);
    } // retranslateUi

};

namespace Ui {
    class PaintToolbox: public Ui_PaintToolbox {};
} // namespace Ui

#endif // UI_PAINTTOOLBOX_H
