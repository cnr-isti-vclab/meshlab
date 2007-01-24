/********************************************************************************
** Form generated from reading ui file 'PaintToolbox.ui'
**
** Created: Wed Jan 24 14:41:01 2007
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
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>
#include "colorwid.h"

class Ui_PaintToolbox
{
public:
    QGridLayout *gridLayout;
    QFrame *advanced_frame;
    QGridLayout *gridLayout1;
    QComboBox *search_mode;
    QLabel *label_2;
    QFrame *color_frame;
    QToolButton *set_bw;
    QToolButton *switch_me;
    ColorWid *front;
    ColorWid *back;
    QFrame *pen_extra_frame;
    QGridLayout *gridLayout2;
    QCheckBox *backface_culling;
    QLabel *label_pen;
    QLabel *label;
    QComboBox *pen_type;
    QDoubleSpinBox *pen_radius;
    QCheckBox *invisible_painting;
    QFrame *pen_frame;
    QGridLayout *gridLayout3;
    QSpinBox *deck_box;
    QLabel *label_deck;
    QLabel *label_penmodus;
    QComboBox *pen_modus;
    QSlider *deck_slider;
    QFrame *frame_3;
    QFrame *top_frame;
    QToolButton *fill_button;
    QToolButton *pen_button;
    QToolButton *pick_button;
    QToolButton *advanced_button;
    QFrame *pick_frame;
    QGridLayout *gridLayout4;
    QComboBox *pick_mode;
    QLabel *label_pick;

    void setupUi(QWidget *PaintToolbox)
    {
    PaintToolbox->setObjectName(QString::fromUtf8("PaintToolbox"));
    gridLayout = new QGridLayout(PaintToolbox);
    gridLayout->setSpacing(2);
    gridLayout->setMargin(2);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    advanced_frame = new QFrame(PaintToolbox);
    advanced_frame->setObjectName(QString::fromUtf8("advanced_frame"));
    advanced_frame->setFrameShape(QFrame::StyledPanel);
    advanced_frame->setFrameShadow(QFrame::Raised);
    gridLayout1 = new QGridLayout(advanced_frame);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(9);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    search_mode = new QComboBox(advanced_frame);
    search_mode->setObjectName(QString::fromUtf8("search_mode"));

    gridLayout1->addWidget(search_mode, 0, 1, 1, 1);

    label_2 = new QLabel(advanced_frame);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout1->addWidget(label_2, 0, 0, 1, 1);


    gridLayout->addWidget(advanced_frame, 5, 0, 1, 2);

    color_frame = new QFrame(PaintToolbox);
    color_frame->setObjectName(QString::fromUtf8("color_frame"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(color_frame->sizePolicy().hasHeightForWidth());
    color_frame->setSizePolicy(sizePolicy);
    color_frame->setMinimumSize(QSize(70, 70));
    color_frame->setMaximumSize(QSize(16777215, 70));
    color_frame->setFrameShape(QFrame::StyledPanel);
    color_frame->setFrameShadow(QFrame::Raised);
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
    front = new ColorWid(color_frame);
    front->setObjectName(QString::fromUtf8("front"));
    front->setGeometry(QRect(50, 20, 40, 40));
    front->setAutoFillBackground(true);
    back = new ColorWid(color_frame);
    back->setObjectName(QString::fromUtf8("back"));
    back->setGeometry(QRect(10, 10, 40, 40));
    back->setAutoFillBackground(true);

    gridLayout->addWidget(color_frame, 1, 0, 1, 1);

    pen_extra_frame = new QFrame(PaintToolbox);
    pen_extra_frame->setObjectName(QString::fromUtf8("pen_extra_frame"));
    pen_extra_frame->setFrameShape(QFrame::StyledPanel);
    pen_extra_frame->setFrameShadow(QFrame::Raised);
    gridLayout2 = new QGridLayout(pen_extra_frame);
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(9);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    backface_culling = new QCheckBox(pen_extra_frame);
    backface_culling->setObjectName(QString::fromUtf8("backface_culling"));

    gridLayout2->addWidget(backface_culling, 1, 1, 1, 2);

    label_pen = new QLabel(pen_extra_frame);
    label_pen->setObjectName(QString::fromUtf8("label_pen"));

    gridLayout2->addWidget(label_pen, 0, 0, 1, 1);

    label = new QLabel(pen_extra_frame);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout2->addWidget(label, 1, 0, 1, 1);

    pen_type = new QComboBox(pen_extra_frame);
    pen_type->setObjectName(QString::fromUtf8("pen_type"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(pen_type->sizePolicy().hasHeightForWidth());
    pen_type->setSizePolicy(sizePolicy1);

    gridLayout2->addWidget(pen_type, 0, 2, 1, 2);

    pen_radius = new QDoubleSpinBox(pen_extra_frame);
    pen_radius->setObjectName(QString::fromUtf8("pen_radius"));
    pen_radius->setDecimals(1);
    pen_radius->setMaximum(70);
    pen_radius->setMinimum(0.1);
    pen_radius->setSingleStep(0.1);
    pen_radius->setValue(25);

    gridLayout2->addWidget(pen_radius, 0, 1, 1, 1);

    invisible_painting = new QCheckBox(pen_extra_frame);
    invisible_painting->setObjectName(QString::fromUtf8("invisible_painting"));

    gridLayout2->addWidget(invisible_painting, 1, 3, 1, 1);


    gridLayout->addWidget(pen_extra_frame, 3, 0, 1, 2);

    pen_frame = new QFrame(PaintToolbox);
    pen_frame->setObjectName(QString::fromUtf8("pen_frame"));
    pen_frame->setFrameShape(QFrame::StyledPanel);
    pen_frame->setFrameShadow(QFrame::Raised);
    gridLayout3 = new QGridLayout(pen_frame);
    gridLayout3->setSpacing(6);
    gridLayout3->setMargin(9);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    deck_box = new QSpinBox(pen_frame);
    deck_box->setObjectName(QString::fromUtf8("deck_box"));
    deck_box->setMaximum(100);
    deck_box->setValue(100);

    gridLayout3->addWidget(deck_box, 0, 2, 1, 1);

    label_deck = new QLabel(pen_frame);
    label_deck->setObjectName(QString::fromUtf8("label_deck"));

    gridLayout3->addWidget(label_deck, 0, 0, 1, 1);

    label_penmodus = new QLabel(pen_frame);
    label_penmodus->setObjectName(QString::fromUtf8("label_penmodus"));

    gridLayout3->addWidget(label_penmodus, 1, 0, 1, 1);

    pen_modus = new QComboBox(pen_frame);
    pen_modus->setObjectName(QString::fromUtf8("pen_modus"));

    gridLayout3->addWidget(pen_modus, 1, 1, 1, 2);

    deck_slider = new QSlider(pen_frame);
    deck_slider->setObjectName(QString::fromUtf8("deck_slider"));
    deck_slider->setMaximum(100);
    deck_slider->setValue(100);
    deck_slider->setOrientation(Qt::Horizontal);

    gridLayout3->addWidget(deck_slider, 0, 1, 1, 1);


    gridLayout->addWidget(pen_frame, 2, 0, 1, 2);

    frame_3 = new QFrame(PaintToolbox);
    frame_3->setObjectName(QString::fromUtf8("frame_3"));
    frame_3->setFrameShape(QFrame::StyledPanel);
    frame_3->setFrameShadow(QFrame::Raised);

    gridLayout->addWidget(frame_3, 1, 1, 1, 1);

    top_frame = new QFrame(PaintToolbox);
    top_frame->setObjectName(QString::fromUtf8("top_frame"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(7));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(top_frame->sizePolicy().hasHeightForWidth());
    top_frame->setSizePolicy(sizePolicy2);
    top_frame->setMinimumSize(QSize(16, 120));
    top_frame->setFrameShape(QFrame::StyledPanel);
    top_frame->setFrameShadow(QFrame::Raised);
    fill_button = new QToolButton(top_frame);
    fill_button->setObjectName(QString::fromUtf8("fill_button"));
    fill_button->setGeometry(QRect(60, 10, 48, 48));
    fill_button->setIcon(QIcon(QString::fromUtf8(":/images/fill.png")));
    fill_button->setIconSize(QSize(48, 48));
    fill_button->setCheckable(true);
    fill_button->setAutoExclusive(true);
    fill_button->setAutoRaise(false);
    pen_button = new QToolButton(top_frame);
    pen_button->setObjectName(QString::fromUtf8("pen_button"));
    pen_button->setGeometry(QRect(10, 10, 48, 48));
    pen_button->setIcon(QIcon(QString::fromUtf8(":/images/pinsel2.png")));
    pen_button->setIconSize(QSize(48, 48));
    pen_button->setCheckable(true);
    pen_button->setAutoExclusive(true);
    pen_button->setAutoRaise(false);
    pick_button = new QToolButton(top_frame);
    pick_button->setObjectName(QString::fromUtf8("pick_button"));
    pick_button->setGeometry(QRect(110, 10, 48, 48));
    pick_button->setIcon(QIcon(QString::fromUtf8(":/images/pick.png")));
    pick_button->setIconSize(QSize(48, 48));
    pick_button->setCheckable(true);
    pick_button->setAutoExclusive(true);
    pick_button->setAutoRaise(false);
    advanced_button = new QToolButton(top_frame);
    advanced_button->setObjectName(QString::fromUtf8("advanced_button"));
    advanced_button->setGeometry(QRect(10, 60, 48, 48));
    advanced_button->setIcon(QIcon(QString::fromUtf8(":/images/kcontrol.png")));
    advanced_button->setIconSize(QSize(48, 48));
    advanced_button->setCheckable(true);
    advanced_button->setAutoExclusive(true);
    advanced_button->setAutoRaise(false);

    gridLayout->addWidget(top_frame, 0, 0, 1, 2);

    pick_frame = new QFrame(PaintToolbox);
    pick_frame->setObjectName(QString::fromUtf8("pick_frame"));
    pick_frame->setFrameShape(QFrame::StyledPanel);
    pick_frame->setFrameShadow(QFrame::Raised);
    gridLayout4 = new QGridLayout(pick_frame);
    gridLayout4->setSpacing(6);
    gridLayout4->setMargin(9);
    gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
    pick_mode = new QComboBox(pick_frame);
    pick_mode->setObjectName(QString::fromUtf8("pick_mode"));

    gridLayout4->addWidget(pick_mode, 0, 1, 1, 1);

    label_pick = new QLabel(pick_frame);
    label_pick->setObjectName(QString::fromUtf8("label_pick"));

    gridLayout4->addWidget(label_pick, 0, 0, 1, 1);


    gridLayout->addWidget(pick_frame, 4, 0, 1, 2);


    retranslateUi(PaintToolbox);

    QSize size(275, 467);
    size = size.expandedTo(PaintToolbox->minimumSizeHint());
    PaintToolbox->resize(size);


    QMetaObject::connectSlotsByName(PaintToolbox);
    } // setupUi

    void retranslateUi(QWidget *PaintToolbox)
    {
    PaintToolbox->setWindowTitle(QApplication::translate("PaintToolbox", "Vertex Painting", 0, QApplication::UnicodeUTF8));
    search_mode->clear();
    search_mode->addItem(QApplication::translate("PaintToolbox", "fast", 0, QApplication::UnicodeUTF8));
    search_mode->addItem(QApplication::translate("PaintToolbox", "slow but accurate", 0, QApplication::UnicodeUTF8));
    search_mode->setToolTip(QApplication::translate("PaintToolbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">fast:</span> works better with big meshes, but can loose some vertexes when faces are not direct connected<br /><span style=\" font-weight:600;\">slow but accurate: </span>sholud be used with small meshes, because it finds every vertex during painting</p></body></html>", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("PaintToolbox", "Vertex search:", 0, QApplication::UnicodeUTF8));
    set_bw->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    switch_me->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    backface_culling->setText(QApplication::translate("PaintToolbox", "backfaces", 0, QApplication::UnicodeUTF8));
    label_pen->setText(QApplication::translate("PaintToolbox", "Pen", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("PaintToolbox", "Paint on:", 0, QApplication::UnicodeUTF8));
    pen_type->clear();
    pen_type->addItem(QApplication::translate("PaintToolbox", "pixel", 0, QApplication::UnicodeUTF8));
    pen_type->addItem(QApplication::translate("PaintToolbox", "percentual of bbox diagonal", 0, QApplication::UnicodeUTF8));
    pen_type->addItem(QApplication::translate("PaintToolbox", "percentual of bbox height", 0, QApplication::UnicodeUTF8));
    invisible_painting->setText(QApplication::translate("PaintToolbox", "hidden polygons", 0, QApplication::UnicodeUTF8));
    deck_box->setToolTip(QApplication::translate("PaintToolbox", "Change the opacity of the color (this is not the opengl alpha value)", 0, QApplication::UnicodeUTF8));
    label_deck->setText(QApplication::translate("PaintToolbox", "Opacity", 0, QApplication::UnicodeUTF8));
    label_penmodus->setText(QApplication::translate("PaintToolbox", "Modus", 0, QApplication::UnicodeUTF8));
    pen_modus->clear();
    pen_modus->addItem(QApplication::translate("PaintToolbox", "Normal", 0, QApplication::UnicodeUTF8));
    deck_slider->setToolTip(QApplication::translate("PaintToolbox", "Change the opacity of the color (this is not the opengl alpha value)", 0, QApplication::UnicodeUTF8));
    fill_button->setToolTip(QApplication::translate("PaintToolbox", "Fill an area", 0, QApplication::UnicodeUTF8));
    fill_button->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    pen_button->setToolTip(QApplication::translate("PaintToolbox", "Paint with the pen", 0, QApplication::UnicodeUTF8));
    pen_button->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    pick_button->setToolTip(QApplication::translate("PaintToolbox", "Pick a color", 0, QApplication::UnicodeUTF8));
    pick_button->setText(QString());
    advanced_button->setToolTip(QApplication::translate("PaintToolbox", "Change advanced settings", 0, QApplication::UnicodeUTF8));
    advanced_button->setText(QString());
    pick_mode->clear();
    pick_mode->addItem(QApplication::translate("PaintToolbox", "Nearest vertex color", 0, QApplication::UnicodeUTF8));
    pick_mode->addItem(QApplication::translate("PaintToolbox", "Pixel color", 0, QApplication::UnicodeUTF8));
    label_pick->setText(QApplication::translate("PaintToolbox", "Picking mode", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(PaintToolbox);
    } // retranslateUi

};

namespace Ui {
    class PaintToolbox: public Ui_PaintToolbox {};
} // namespace Ui

#endif // UI_PAINTTOOLBOX_H
