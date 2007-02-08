/********************************************************************************
** Form generated from reading ui file 'PaintToolbox.ui'
**
** Created: Thu Feb 8 12:24:24 2007
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
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>
#include "colorwid.h"

class Ui_PaintToolbox
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout1;
    QWidget *widget;
    QToolButton *fill_button;
    QToolButton *gradient_button;
    QToolButton *smooth_button;
    QToolButton *pick_button;
    QToolButton *pen_button;
    QFrame *frame_3;
    QToolButton *redo_button;
    QToolButton *undo_button;
    QFrame *color_frame;
    QToolButton *set_bw;
    QToolButton *switch_me;
    ColorWid *back;
    ColorWid *front;
    QFrame *gradient_frame;
    QGridLayout *gridLayout2;
    QLabel *label_4;
    QComboBox *gradient_type;
    QLabel *label_3;
    QComboBox *gradient_form;
    QFrame *pick_frame;
    QGridLayout *gridLayout3;
    QComboBox *pick_mode;
    QLabel *label_pick;
    QFrame *pen_extra_frame;
    QGridLayout *gridLayout4;
    QSpinBox *decrease_box;
    QCheckBox *backface_culling;
    QCheckBox *invisible_painting;
    QLabel *label;
    QDoubleSpinBox *pen_radius;
    QComboBox *pen_type;
    QLabel *label_pen;
    QSlider *decrease_slider;
    QLabel *label_percentual_3;
    QFrame *smooth_frame;
    QGridLayout *gridLayout5;
    QSlider *percentual_slider;
    QLabel *label_percentual;
    QSpinBox *percentual_box;
    QFrame *pen_frame;
    QGridLayout *gridLayout6;
    QSpinBox *deck_box;
    QLabel *label_deck;
    QLabel *label_penmodus;
    QComboBox *pen_modus;
    QSlider *deck_slider;
    QWidget *tab_2;
    QGridLayout *gridLayout7;
    QSpacerItem *spacerItem;
    QFrame *pen_select_frame;
    QGridLayout *gridLayout8;
    QLabel *label_5;
    QDoubleSpinBox *pen_radius_2;
    QLabel *label_pen_2;
    QSlider *decrease_slider_2;
    QSpinBox *decrease_box_2;
    QLabel *label_6;
    QComboBox *pen_type_2;
    QCheckBox *backface_culling_2;
    QCheckBox *invisible_painting_2;
    QFrame *poly_smooth_frame;
    QGridLayout *gridLayout9;
    QSlider *percentual_slider_2;
    QLabel *label_percentual_2;
    QSpinBox *percentual_box_2;
    QWidget *widget_2;
    QToolButton *poly_smooth_button;
    QToolButton *select_button;
    QWidget *tab_3;
    QGridLayout *gridLayout10;
    QComboBox *search_mode;
    QLabel *label_2;
    QSpacerItem *spacerItem1;
    QSpacerItem *spacerItem2;

    void setupUi(QWidget *PaintToolbox)
    {
    PaintToolbox->setObjectName(QString::fromUtf8("PaintToolbox"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(PaintToolbox->sizePolicy().hasHeightForWidth());
    PaintToolbox->setSizePolicy(sizePolicy);
    gridLayout = new QGridLayout(PaintToolbox);
    gridLayout->setSpacing(2);
    gridLayout->setMargin(2);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    tabWidget = new QTabWidget(PaintToolbox);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
    tabWidget->setSizePolicy(sizePolicy1);
    tabWidget->setMinimumSize(QSize(0, 320));
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    gridLayout1 = new QGridLayout(tab);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(9);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    widget = new QWidget(tab);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setMinimumSize(QSize(0, 60));
    fill_button = new QToolButton(widget);
    fill_button->setObjectName(QString::fromUtf8("fill_button"));
    fill_button->setGeometry(QRect(50, 10, 48, 48));
    fill_button->setIcon(QIcon(QString::fromUtf8(":/images/fill.png")));
    fill_button->setIconSize(QSize(48, 48));
    fill_button->setCheckable(true);
    fill_button->setAutoExclusive(true);
    fill_button->setAutoRaise(false);
    gradient_button = new QToolButton(widget);
    gradient_button->setObjectName(QString::fromUtf8("gradient_button"));
    gradient_button->setGeometry(QRect(100, 10, 48, 48));
    gradient_button->setIcon(QIcon(QString::fromUtf8(":/images/gradient.png")));
    gradient_button->setIconSize(QSize(48, 48));
    gradient_button->setCheckable(true);
    gradient_button->setAutoExclusive(true);
    gradient_button->setAutoRaise(false);
    smooth_button = new QToolButton(widget);
    smooth_button->setObjectName(QString::fromUtf8("smooth_button"));
    smooth_button->setGeometry(QRect(150, 10, 48, 48));
    smooth_button->setIcon(QIcon(QString::fromUtf8(":/images/smooth_me.png")));
    smooth_button->setIconSize(QSize(48, 48));
    smooth_button->setCheckable(true);
    smooth_button->setAutoExclusive(true);
    smooth_button->setAutoRaise(false);
    pick_button = new QToolButton(widget);
    pick_button->setObjectName(QString::fromUtf8("pick_button"));
    pick_button->setGeometry(QRect(200, 10, 48, 48));
    pick_button->setIcon(QIcon(QString::fromUtf8(":/images/pick.png")));
    pick_button->setIconSize(QSize(48, 48));
    pick_button->setCheckable(true);
    pick_button->setAutoExclusive(true);
    pick_button->setAutoRaise(false);
    pen_button = new QToolButton(widget);
    pen_button->setObjectName(QString::fromUtf8("pen_button"));
    pen_button->setGeometry(QRect(0, 10, 48, 48));
    pen_button->setIcon(QIcon(QString::fromUtf8(":/images/pinsel2.png")));
    pen_button->setIconSize(QSize(48, 48));
    pen_button->setCheckable(true);
    pen_button->setAutoExclusive(true);
    pen_button->setAutoRaise(false);

    gridLayout1->addWidget(widget, 0, 0, 1, 2);

    frame_3 = new QFrame(tab);
    frame_3->setObjectName(QString::fromUtf8("frame_3"));
    frame_3->setFrameShape(QFrame::StyledPanel);
    frame_3->setFrameShadow(QFrame::Raised);
    redo_button = new QToolButton(frame_3);
    redo_button->setObjectName(QString::fromUtf8("redo_button"));
    redo_button->setGeometry(QRect(60, 10, 48, 48));
    redo_button->setIcon(QIcon(QString::fromUtf8(":/images/redo.png")));
    redo_button->setIconSize(QSize(48, 48));
    undo_button = new QToolButton(frame_3);
    undo_button->setObjectName(QString::fromUtf8("undo_button"));
    undo_button->setGeometry(QRect(10, 10, 48, 48));
    undo_button->setIcon(QIcon(QString::fromUtf8(":/images/undo.png")));
    undo_button->setIconSize(QSize(48, 48));

    gridLayout1->addWidget(frame_3, 1, 1, 1, 1);

    color_frame = new QFrame(tab);
    color_frame->setObjectName(QString::fromUtf8("color_frame"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(color_frame->sizePolicy().hasHeightForWidth());
    color_frame->setSizePolicy(sizePolicy2);
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
    back = new ColorWid(color_frame);
    back->setObjectName(QString::fromUtf8("back"));
    back->setGeometry(QRect(10, 10, 40, 40));
    back->setAutoFillBackground(true);
    front = new ColorWid(color_frame);
    front->setObjectName(QString::fromUtf8("front"));
    front->setGeometry(QRect(50, 20, 40, 40));
    front->setAutoFillBackground(true);

    gridLayout1->addWidget(color_frame, 1, 0, 1, 1);

    gradient_frame = new QFrame(tab);
    gradient_frame->setObjectName(QString::fromUtf8("gradient_frame"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(gradient_frame->sizePolicy().hasHeightForWidth());
    gradient_frame->setSizePolicy(sizePolicy3);
    gradient_frame->setFrameShape(QFrame::StyledPanel);
    gradient_frame->setFrameShadow(QFrame::Raised);
    gridLayout2 = new QGridLayout(gradient_frame);
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(9);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    label_4 = new QLabel(gradient_frame);
    label_4->setObjectName(QString::fromUtf8("label_4"));

    gridLayout2->addWidget(label_4, 0, 0, 1, 1);

    gradient_type = new QComboBox(gradient_frame);
    gradient_type->setObjectName(QString::fromUtf8("gradient_type"));

    gridLayout2->addWidget(gradient_type, 0, 1, 1, 2);

    label_3 = new QLabel(gradient_frame);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout2->addWidget(label_3, 1, 0, 1, 2);

    gradient_form = new QComboBox(gradient_frame);
    gradient_form->setObjectName(QString::fromUtf8("gradient_form"));

    gridLayout2->addWidget(gradient_form, 1, 2, 1, 1);


    gridLayout1->addWidget(gradient_frame, 6, 0, 1, 2);

    pick_frame = new QFrame(tab);
    pick_frame->setObjectName(QString::fromUtf8("pick_frame"));
    pick_frame->setFrameShape(QFrame::StyledPanel);
    pick_frame->setFrameShadow(QFrame::Raised);
    gridLayout3 = new QGridLayout(pick_frame);
    gridLayout3->setSpacing(6);
    gridLayout3->setMargin(9);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    pick_mode = new QComboBox(pick_frame);
    pick_mode->setObjectName(QString::fromUtf8("pick_mode"));

    gridLayout3->addWidget(pick_mode, 0, 1, 1, 1);

    label_pick = new QLabel(pick_frame);
    label_pick->setObjectName(QString::fromUtf8("label_pick"));

    gridLayout3->addWidget(label_pick, 0, 0, 1, 1);


    gridLayout1->addWidget(pick_frame, 5, 0, 1, 2);

    pen_extra_frame = new QFrame(tab);
    pen_extra_frame->setObjectName(QString::fromUtf8("pen_extra_frame"));
    pen_extra_frame->setFrameShape(QFrame::StyledPanel);
    pen_extra_frame->setFrameShadow(QFrame::Raised);
    gridLayout4 = new QGridLayout(pen_extra_frame);
    gridLayout4->setSpacing(6);
    gridLayout4->setMargin(9);
    gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
    decrease_box = new QSpinBox(pen_extra_frame);
    decrease_box->setObjectName(QString::fromUtf8("decrease_box"));
    decrease_box->setMaximum(100);
    decrease_box->setValue(50);

    gridLayout4->addWidget(decrease_box, 1, 4, 1, 1);

    backface_culling = new QCheckBox(pen_extra_frame);
    backface_culling->setObjectName(QString::fromUtf8("backface_culling"));

    gridLayout4->addWidget(backface_culling, 2, 1, 1, 2);

    invisible_painting = new QCheckBox(pen_extra_frame);
    invisible_painting->setObjectName(QString::fromUtf8("invisible_painting"));

    gridLayout4->addWidget(invisible_painting, 2, 3, 1, 2);

    label = new QLabel(pen_extra_frame);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout4->addWidget(label, 2, 0, 1, 1);

    pen_radius = new QDoubleSpinBox(pen_extra_frame);
    pen_radius->setObjectName(QString::fromUtf8("pen_radius"));
    pen_radius->setDecimals(1);
    pen_radius->setMaximum(100);
    pen_radius->setMinimum(0.1);
    pen_radius->setSingleStep(0.1);
    pen_radius->setValue(40);

    gridLayout4->addWidget(pen_radius, 0, 1, 1, 1);

    pen_type = new QComboBox(pen_extra_frame);
    pen_type->setObjectName(QString::fromUtf8("pen_type"));
    QSizePolicy sizePolicy4(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(pen_type->sizePolicy().hasHeightForWidth());
    pen_type->setSizePolicy(sizePolicy4);

    gridLayout4->addWidget(pen_type, 0, 2, 1, 3);

    label_pen = new QLabel(pen_extra_frame);
    label_pen->setObjectName(QString::fromUtf8("label_pen"));

    gridLayout4->addWidget(label_pen, 0, 0, 1, 1);

    decrease_slider = new QSlider(pen_extra_frame);
    decrease_slider->setObjectName(QString::fromUtf8("decrease_slider"));
    decrease_slider->setMaximum(100);
    decrease_slider->setValue(50);
    decrease_slider->setSliderPosition(50);
    decrease_slider->setOrientation(Qt::Horizontal);

    gridLayout4->addWidget(decrease_slider, 1, 2, 1, 2);

    label_percentual_3 = new QLabel(pen_extra_frame);
    label_percentual_3->setObjectName(QString::fromUtf8("label_percentual_3"));

    gridLayout4->addWidget(label_percentual_3, 1, 0, 1, 2);


    gridLayout1->addWidget(pen_extra_frame, 4, 0, 1, 2);

    smooth_frame = new QFrame(tab);
    smooth_frame->setObjectName(QString::fromUtf8("smooth_frame"));
    smooth_frame->setFrameShape(QFrame::StyledPanel);
    smooth_frame->setFrameShadow(QFrame::Raised);
    gridLayout5 = new QGridLayout(smooth_frame);
    gridLayout5->setSpacing(6);
    gridLayout5->setMargin(9);
    gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
    percentual_slider = new QSlider(smooth_frame);
    percentual_slider->setObjectName(QString::fromUtf8("percentual_slider"));
    percentual_slider->setMaximum(100);
    percentual_slider->setValue(25);
    percentual_slider->setSliderPosition(25);
    percentual_slider->setOrientation(Qt::Horizontal);

    gridLayout5->addWidget(percentual_slider, 0, 1, 1, 1);

    label_percentual = new QLabel(smooth_frame);
    label_percentual->setObjectName(QString::fromUtf8("label_percentual"));

    gridLayout5->addWidget(label_percentual, 0, 0, 1, 1);

    percentual_box = new QSpinBox(smooth_frame);
    percentual_box->setObjectName(QString::fromUtf8("percentual_box"));
    percentual_box->setMaximum(100);
    percentual_box->setValue(25);

    gridLayout5->addWidget(percentual_box, 0, 2, 1, 1);


    gridLayout1->addWidget(smooth_frame, 3, 0, 1, 2);

    pen_frame = new QFrame(tab);
    pen_frame->setObjectName(QString::fromUtf8("pen_frame"));
    pen_frame->setFrameShape(QFrame::StyledPanel);
    pen_frame->setFrameShadow(QFrame::Raised);
    gridLayout6 = new QGridLayout(pen_frame);
    gridLayout6->setSpacing(6);
    gridLayout6->setMargin(9);
    gridLayout6->setObjectName(QString::fromUtf8("gridLayout6"));
    deck_box = new QSpinBox(pen_frame);
    deck_box->setObjectName(QString::fromUtf8("deck_box"));
    deck_box->setMaximum(100);
    deck_box->setValue(100);

    gridLayout6->addWidget(deck_box, 0, 2, 1, 1);

    label_deck = new QLabel(pen_frame);
    label_deck->setObjectName(QString::fromUtf8("label_deck"));

    gridLayout6->addWidget(label_deck, 0, 0, 1, 1);

    label_penmodus = new QLabel(pen_frame);
    label_penmodus->setObjectName(QString::fromUtf8("label_penmodus"));

    gridLayout6->addWidget(label_penmodus, 1, 0, 1, 1);

    pen_modus = new QComboBox(pen_frame);
    pen_modus->setObjectName(QString::fromUtf8("pen_modus"));

    gridLayout6->addWidget(pen_modus, 1, 1, 1, 2);

    deck_slider = new QSlider(pen_frame);
    deck_slider->setObjectName(QString::fromUtf8("deck_slider"));
    deck_slider->setMaximum(100);
    deck_slider->setValue(100);
    deck_slider->setOrientation(Qt::Horizontal);

    gridLayout6->addWidget(deck_slider, 0, 1, 1, 1);


    gridLayout1->addWidget(pen_frame, 2, 0, 1, 2);

    tabWidget->addTab(tab, QApplication::translate("PaintToolbox", "Painting", 0, QApplication::UnicodeUTF8));
    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    gridLayout7 = new QGridLayout(tab_2);
    gridLayout7->setSpacing(6);
    gridLayout7->setMargin(9);
    gridLayout7->setObjectName(QString::fromUtf8("gridLayout7"));
    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout7->addItem(spacerItem, 3, 0, 1, 1);

    pen_select_frame = new QFrame(tab_2);
    pen_select_frame->setObjectName(QString::fromUtf8("pen_select_frame"));
    pen_select_frame->setFrameShape(QFrame::StyledPanel);
    pen_select_frame->setFrameShadow(QFrame::Raised);
    gridLayout8 = new QGridLayout(pen_select_frame);
    gridLayout8->setSpacing(6);
    gridLayout8->setMargin(9);
    gridLayout8->setObjectName(QString::fromUtf8("gridLayout8"));
    label_5 = new QLabel(pen_select_frame);
    label_5->setObjectName(QString::fromUtf8("label_5"));

    gridLayout8->addWidget(label_5, 2, 0, 1, 2);

    pen_radius_2 = new QDoubleSpinBox(pen_select_frame);
    pen_radius_2->setObjectName(QString::fromUtf8("pen_radius_2"));
    pen_radius_2->setDecimals(1);
    pen_radius_2->setMaximum(100);
    pen_radius_2->setMinimum(0.1);
    pen_radius_2->setSingleStep(0.1);
    pen_radius_2->setValue(40);

    gridLayout8->addWidget(pen_radius_2, 0, 1, 1, 2);

    label_pen_2 = new QLabel(pen_select_frame);
    label_pen_2->setObjectName(QString::fromUtf8("label_pen_2"));

    gridLayout8->addWidget(label_pen_2, 0, 0, 1, 1);

    decrease_slider_2 = new QSlider(pen_select_frame);
    decrease_slider_2->setObjectName(QString::fromUtf8("decrease_slider_2"));
    decrease_slider_2->setMaximum(100);
    decrease_slider_2->setValue(50);
    decrease_slider_2->setSliderPosition(50);
    decrease_slider_2->setOrientation(Qt::Horizontal);

    gridLayout8->addWidget(decrease_slider_2, 1, 3, 1, 2);

    decrease_box_2 = new QSpinBox(pen_select_frame);
    decrease_box_2->setObjectName(QString::fromUtf8("decrease_box_2"));
    decrease_box_2->setMaximum(100);
    decrease_box_2->setValue(50);

    gridLayout8->addWidget(decrease_box_2, 1, 5, 1, 1);

    label_6 = new QLabel(pen_select_frame);
    label_6->setObjectName(QString::fromUtf8("label_6"));

    gridLayout8->addWidget(label_6, 1, 0, 1, 3);

    pen_type_2 = new QComboBox(pen_select_frame);
    pen_type_2->setObjectName(QString::fromUtf8("pen_type_2"));
    QSizePolicy sizePolicy5(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(pen_type_2->sizePolicy().hasHeightForWidth());
    pen_type_2->setSizePolicy(sizePolicy5);

    gridLayout8->addWidget(pen_type_2, 0, 3, 1, 3);

    backface_culling_2 = new QCheckBox(pen_select_frame);
    backface_culling_2->setObjectName(QString::fromUtf8("backface_culling_2"));
    backface_culling_2->setChecked(true);

    gridLayout8->addWidget(backface_culling_2, 2, 2, 1, 2);

    invisible_painting_2 = new QCheckBox(pen_select_frame);
    invisible_painting_2->setObjectName(QString::fromUtf8("invisible_painting_2"));
    invisible_painting_2->setChecked(false);

    gridLayout8->addWidget(invisible_painting_2, 2, 4, 1, 2);


    gridLayout7->addWidget(pen_select_frame, 2, 0, 1, 1);

    poly_smooth_frame = new QFrame(tab_2);
    poly_smooth_frame->setObjectName(QString::fromUtf8("poly_smooth_frame"));
    poly_smooth_frame->setFrameShape(QFrame::StyledPanel);
    poly_smooth_frame->setFrameShadow(QFrame::Raised);
    gridLayout9 = new QGridLayout(poly_smooth_frame);
    gridLayout9->setSpacing(6);
    gridLayout9->setMargin(9);
    gridLayout9->setObjectName(QString::fromUtf8("gridLayout9"));
    percentual_slider_2 = new QSlider(poly_smooth_frame);
    percentual_slider_2->setObjectName(QString::fromUtf8("percentual_slider_2"));
    percentual_slider_2->setMaximum(100);
    percentual_slider_2->setValue(25);
    percentual_slider_2->setSliderPosition(25);
    percentual_slider_2->setOrientation(Qt::Horizontal);

    gridLayout9->addWidget(percentual_slider_2, 0, 1, 1, 1);

    label_percentual_2 = new QLabel(poly_smooth_frame);
    label_percentual_2->setObjectName(QString::fromUtf8("label_percentual_2"));

    gridLayout9->addWidget(label_percentual_2, 0, 0, 1, 1);

    percentual_box_2 = new QSpinBox(poly_smooth_frame);
    percentual_box_2->setObjectName(QString::fromUtf8("percentual_box_2"));
    percentual_box_2->setMaximum(100);
    percentual_box_2->setValue(25);

    gridLayout9->addWidget(percentual_box_2, 0, 2, 1, 1);


    gridLayout7->addWidget(poly_smooth_frame, 1, 0, 1, 1);

    widget_2 = new QWidget(tab_2);
    widget_2->setObjectName(QString::fromUtf8("widget_2"));
    QSizePolicy sizePolicy6(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy6.setHorizontalStretch(0);
    sizePolicy6.setVerticalStretch(0);
    sizePolicy6.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
    widget_2->setSizePolicy(sizePolicy6);
    widget_2->setMinimumSize(QSize(0, 60));
    poly_smooth_button = new QToolButton(widget_2);
    poly_smooth_button->setObjectName(QString::fromUtf8("poly_smooth_button"));
    poly_smooth_button->setGeometry(QRect(50, 10, 48, 48));
    poly_smooth_button->setIcon(QIcon(QString::fromUtf8(":/images/smooth_me.png")));
    poly_smooth_button->setIconSize(QSize(48, 48));
    poly_smooth_button->setCheckable(true);
    poly_smooth_button->setAutoExclusive(true);
    poly_smooth_button->setAutoRaise(false);
    select_button = new QToolButton(widget_2);
    select_button->setObjectName(QString::fromUtf8("select_button"));
    select_button->setGeometry(QRect(0, 10, 48, 48));
    select_button->setIcon(QIcon(QString::fromUtf8(":/images/pinsel_select.png")));
    select_button->setIconSize(QSize(48, 48));
    select_button->setCheckable(true);
    select_button->setAutoExclusive(true);
    select_button->setAutoRaise(false);

    gridLayout7->addWidget(widget_2, 0, 0, 1, 1);

    tabWidget->addTab(tab_2, QApplication::translate("PaintToolbox", "Selecting && Editing", 0, QApplication::UnicodeUTF8));
    tab_3 = new QWidget();
    tab_3->setObjectName(QString::fromUtf8("tab_3"));
    gridLayout10 = new QGridLayout(tab_3);
    gridLayout10->setSpacing(6);
    gridLayout10->setMargin(9);
    gridLayout10->setObjectName(QString::fromUtf8("gridLayout10"));
    search_mode = new QComboBox(tab_3);
    search_mode->setObjectName(QString::fromUtf8("search_mode"));

    gridLayout10->addWidget(search_mode, 0, 1, 1, 1);

    label_2 = new QLabel(tab_3);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout10->addWidget(label_2, 0, 0, 1, 1);

    spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout10->addItem(spacerItem1, 1, 0, 1, 1);

    tabWidget->addTab(tab_3, QApplication::translate("PaintToolbox", "Configuration", 0, QApplication::UnicodeUTF8));

    gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

    spacerItem2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem2, 1, 0, 1, 1);


    retranslateUi(PaintToolbox);

    QSize size(284, 645);
    size = size.expandedTo(PaintToolbox->minimumSizeHint());
    PaintToolbox->resize(size);


    tabWidget->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(PaintToolbox);
    } // setupUi

    void retranslateUi(QWidget *PaintToolbox)
    {
    PaintToolbox->setWindowTitle(QApplication::translate("PaintToolbox", "Vertex Painting", 0, QApplication::UnicodeUTF8));
    fill_button->setToolTip(QApplication::translate("PaintToolbox", "Fill an area", 0, QApplication::UnicodeUTF8));
    fill_button->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    gradient_button->setToolTip(QApplication::translate("PaintToolbox", "Fill with gradients", 0, QApplication::UnicodeUTF8));
    gradient_button->setText(QString());
    smooth_button->setToolTip(QApplication::translate("PaintToolbox", "Smooth", 0, QApplication::UnicodeUTF8));
    smooth_button->setText(QString());
    pick_button->setToolTip(QApplication::translate("PaintToolbox", "Pick a color", 0, QApplication::UnicodeUTF8));
    pick_button->setText(QString());
    pen_button->setToolTip(QApplication::translate("PaintToolbox", "Paint with the pen", 0, QApplication::UnicodeUTF8));
    pen_button->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    redo_button->setText(QString());
    undo_button->setText(QString());
    set_bw->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    switch_me->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("PaintToolbox", "Type", 0, QApplication::UnicodeUTF8));
    gradient_type->clear();
    gradient_type->addItem(QApplication::translate("PaintToolbox", "Foreground to background (RGB)", 0, QApplication::UnicodeUTF8));
    gradient_type->addItem(QApplication::translate("PaintToolbox", "Foreground to transparency", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("PaintToolbox", "Form", 0, QApplication::UnicodeUTF8));
    gradient_form->clear();
    gradient_form->addItem(QApplication::translate("PaintToolbox", "Linear", 0, QApplication::UnicodeUTF8));
    gradient_form->addItem(QApplication::translate("PaintToolbox", "Circular", 0, QApplication::UnicodeUTF8));
    pick_mode->clear();
    pick_mode->addItem(QApplication::translate("PaintToolbox", "Nearest vertex color", 0, QApplication::UnicodeUTF8));
    pick_mode->addItem(QApplication::translate("PaintToolbox", "Pixel color", 0, QApplication::UnicodeUTF8));
    label_pick->setText(QApplication::translate("PaintToolbox", "Picking mode", 0, QApplication::UnicodeUTF8));
    decrease_box->setToolTip(QApplication::translate("PaintToolbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    backface_culling->setText(QApplication::translate("PaintToolbox", "backfaces", 0, QApplication::UnicodeUTF8));
    invisible_painting->setText(QApplication::translate("PaintToolbox", "hidden polygons", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("PaintToolbox", "Paint on:", 0, QApplication::UnicodeUTF8));
    pen_type->clear();
    pen_type->addItem(QApplication::translate("PaintToolbox", "pixel", 0, QApplication::UnicodeUTF8));
    pen_type->addItem(QApplication::translate("PaintToolbox", "percentual of bbox diagonal", 0, QApplication::UnicodeUTF8));
    pen_type->addItem(QApplication::translate("PaintToolbox", "percentual of bbox height", 0, QApplication::UnicodeUTF8));
    label_pen->setText(QApplication::translate("PaintToolbox", "Pen", 0, QApplication::UnicodeUTF8));
    decrease_slider->setToolTip(QApplication::translate("PaintToolbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_percentual_3->setText(QApplication::translate("PaintToolbox", "Smooth borders", 0, QApplication::UnicodeUTF8));
    percentual_slider->setToolTip(QApplication::translate("PaintToolbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_percentual->setText(QApplication::translate("PaintToolbox", "Percentual", 0, QApplication::UnicodeUTF8));
    percentual_box->setToolTip(QApplication::translate("PaintToolbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    deck_box->setToolTip(QApplication::translate("PaintToolbox", "Change the opacity of the color (this is not the opengl alpha value)", 0, QApplication::UnicodeUTF8));
    label_deck->setText(QApplication::translate("PaintToolbox", "Opacity", 0, QApplication::UnicodeUTF8));
    label_penmodus->setText(QApplication::translate("PaintToolbox", "Modus", 0, QApplication::UnicodeUTF8));
    pen_modus->clear();
    pen_modus->addItem(QApplication::translate("PaintToolbox", "Normal", 0, QApplication::UnicodeUTF8));
    deck_slider->setToolTip(QApplication::translate("PaintToolbox", "Change the opacity of the color (this is not the opengl alpha value)", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("PaintToolbox", "Painting", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("PaintToolbox", "On:", 0, QApplication::UnicodeUTF8));
    label_pen_2->setText(QApplication::translate("PaintToolbox", "Pen", 0, QApplication::UnicodeUTF8));
    decrease_slider_2->setToolTip(QApplication::translate("PaintToolbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("PaintToolbox", "Smooth borders", 0, QApplication::UnicodeUTF8));
    pen_type_2->clear();
    pen_type_2->addItem(QApplication::translate("PaintToolbox", "pixel", 0, QApplication::UnicodeUTF8));
    pen_type_2->addItem(QApplication::translate("PaintToolbox", "percentual of bbox diagonal", 0, QApplication::UnicodeUTF8));
    pen_type_2->addItem(QApplication::translate("PaintToolbox", "percentual of bbox height", 0, QApplication::UnicodeUTF8));
    backface_culling_2->setText(QApplication::translate("PaintToolbox", "backfaces", 0, QApplication::UnicodeUTF8));
    invisible_painting_2->setText(QApplication::translate("PaintToolbox", "hidden polygons", 0, QApplication::UnicodeUTF8));
    percentual_slider_2->setToolTip(QApplication::translate("PaintToolbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_percentual_2->setText(QApplication::translate("PaintToolbox", "Percentual", 0, QApplication::UnicodeUTF8));
    percentual_box_2->setToolTip(QApplication::translate("PaintToolbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    poly_smooth_button->setToolTip(QApplication::translate("PaintToolbox", "Smooth the mesh", 0, QApplication::UnicodeUTF8));
    poly_smooth_button->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    select_button->setToolTip(QApplication::translate("PaintToolbox", "Select faces with the pen", 0, QApplication::UnicodeUTF8));
    select_button->setText(QApplication::translate("PaintToolbox", "...", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("PaintToolbox", "Selecting && Editing", 0, QApplication::UnicodeUTF8));
    search_mode->clear();
    search_mode->addItem(QApplication::translate("PaintToolbox", "fast", 0, QApplication::UnicodeUTF8));
    search_mode->addItem(QApplication::translate("PaintToolbox", "slow but accurate", 0, QApplication::UnicodeUTF8));
    search_mode->setToolTip(QApplication::translate("PaintToolbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">fast:</span> works better with big meshes, but can loose some vertexes when faces are not direct connected<br /><span style=\" font-weight:600;\">slow but accurate: </span>sholud be used with small meshes, because it finds every vertex during painting</p></body></html>", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("PaintToolbox", "Vertex search:", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("PaintToolbox", "Configuration", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(PaintToolbox);
    } // retranslateUi

};

namespace Ui {
    class PaintToolbox: public Ui_PaintToolbox {};
} // namespace Ui

#endif // UI_PAINTTOOLBOX_H
