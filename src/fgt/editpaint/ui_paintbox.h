/********************************************************************************
** Form generated from reading ui file 'paintbox.ui'
**
** Created: Sat Mar 8 16:19:07 2008
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PAINTBOX_H
#define UI_PAINTBOX_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <colorframe.h>

class Ui_Paintbox
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabs_container;
    QWidget *tab;
    QGridLayout *gridLayout1;
    QWidget *widget;
    QHBoxLayout *hboxLayout;
    QToolButton *pen_button;
    QToolButton *fill_button;
    QToolButton *gradient_button;
    QToolButton *smooth_button;
    QToolButton *clone_button;
    QToolButton *pick_button;
    QSpacerItem *spacerItem;
    QFrame *line;
    QFrame *color_frame;
    QToolButton *default_colors;
    QToolButton *switch_colors;
    Colorframe *background_frame;
    Colorframe *foreground_frame;
    QFrame *undo_frame;
    QToolButton *redo_button;
    QToolButton *undo_button;
    QFrame *pen_frame;
    QGridLayout *gridLayout2;
    QSpinBox *deck_box;
    QLabel *label_deck;
    QLabel *label_penmodus;
    QComboBox *pen_modus;
    QSlider *opacity_slider;
    QFrame *smooth_frame;
    QGridLayout *gridLayout3;
    QSlider *percentual_slider;
    QLabel *label_percentual;
    QSpinBox *percentual_box;
    QFrame *pen_extra_frame;
    QGridLayout *gridLayout4;
    QLabel *label_pen;
    QComboBox *brush_box;
    QLabel *label_percentual_3;
    QSlider *hardness_slider;
    QSpinBox *decrease_box;
    QLabel *label_percentual_4;
    QSlider *size_slider;
    QSpinBox *decrease_box_3;
    QLabel *label_percentual_5;
    QComboBox *pen_unit;
    QLabel *label;
    QCheckBox *backface_culling;
    QCheckBox *invisible_painting;
    QFrame *pick_frame;
    QGridLayout *gridLayout5;
    QComboBox *pick_mode;
    QLabel *label_pick;
    QFrame *gradient_frame;
    QGridLayout *gridLayout6;
    QLabel *label_4;
    QComboBox *gradient_type;
    QLabel *label_3;
    QComboBox *gradient_form;
    QFrame *pressure_frame;
    QGridLayout *gridLayout7;
    QLabel *label_7;
    QCheckBox *opacity_box;
    QCheckBox *hardness_box;
    QCheckBox *size_box;
    QSpacerItem *spacerItem1;
    QWidget *tab_2;
    QVBoxLayout *vboxLayout;
    QWidget *widget_2;
    QHBoxLayout *hboxLayout1;
    QToolButton *mesh_pick_button;
    QToolButton *mesh_smooth_button;
    QToolButton *mesh_sculpt_button;
    QToolButton *mesh_add_button;
    QSpacerItem *spacerItem2;
    QFrame *line_2;
    QFrame *mesh_undo_frame;
    QHBoxLayout *hboxLayout2;
    QToolButton *mesh_undo_button;
    QToolButton *mesh_redo_button;
    QFrame *mesh_smooth_frame;
    QGridLayout *gridLayout8;
    QSlider *mesh_percentual_slider;
    QLabel *label_percentual_2;
    QSpinBox *percentual_box_2;
    QFrame *mesh_pen_extra_frame;
    QGridLayout *gridLayout9;
    QLabel *label_pen_2;
    QComboBox *mesh_brush_box;
    QLabel *label_percentual_6;
    QSlider *mesh_hardness_slider;
    QSpinBox *decrease_box_2;
    QLabel *label_percentual_7;
    QSlider *mesh_size_slider;
    QSpinBox *decrease_box_4;
    QLabel *label_percentual_8;
    QComboBox *mesh_pen_unit;
    QLabel *label_5;
    QCheckBox *backface_culling_2;
    QCheckBox *invisible_painting_2;
    QFrame *mesh_displacement_frame;
    QGridLayout *gridLayout10;
    QLabel *label_6;
    QSlider *mesh_displacement_slider;
    QSpinBox *spinBox;
    QLabel *label_8;
    QComboBox *mesh_displacement_direction;
    QFrame *mesh_pressure_frame;
    QGridLayout *gridLayout11;
    QLabel *label_9;
    QCheckBox *displacement_box;
    QCheckBox *mesh_hardness_box;
    QCheckBox *mesh_size_box;
    QSpacerItem *spacerItem3;
    QWidget *tab_3;
    QGridLayout *gridLayout12;
    QComboBox *search_mode;
    QLabel *label_2;
    QSpacerItem *spacerItem4;

    void setupUi(QWidget *Paintbox)
    {
    if (Paintbox->objectName().isEmpty())
        Paintbox->setObjectName(QString::fromUtf8("Paintbox"));
    Paintbox->resize(354, 721);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(Paintbox->sizePolicy().hasHeightForWidth());
    Paintbox->setSizePolicy(sizePolicy);
    gridLayout = new QGridLayout(Paintbox);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    tabs_container = new QTabWidget(Paintbox);
    tabs_container->setObjectName(QString::fromUtf8("tabs_container"));
    sizePolicy.setHeightForWidth(tabs_container->sizePolicy().hasHeightForWidth());
    tabs_container->setSizePolicy(sizePolicy);
    tabs_container->setMinimumSize(QSize(0, 0));
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    gridLayout1 = new QGridLayout(tab);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    widget = new QWidget(tab);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setLayoutDirection(Qt::LeftToRight);
    hboxLayout = new QHBoxLayout(widget);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    pen_button = new QToolButton(widget);
    pen_button->setObjectName(QString::fromUtf8("pen_button"));
    pen_button->setIcon(QIcon(QString::fromUtf8(":/images/paintbrush-22.png")));
    pen_button->setCheckable(true);
    pen_button->setChecked(true);
    pen_button->setAutoExclusive(true);

    hboxLayout->addWidget(pen_button);

    fill_button = new QToolButton(widget);
    fill_button->setObjectName(QString::fromUtf8("fill_button"));
    fill_button->setIcon(QIcon(QString::fromUtf8(":/images/bucket-fill-22.png")));
    fill_button->setCheckable(true);
    fill_button->setAutoExclusive(true);

    hboxLayout->addWidget(fill_button);

    gradient_button = new QToolButton(widget);
    gradient_button->setObjectName(QString::fromUtf8("gradient_button"));
    gradient_button->setIcon(QIcon(QString::fromUtf8(":/images/blend-22.png")));
    gradient_button->setCheckable(true);
    gradient_button->setAutoExclusive(true);

    hboxLayout->addWidget(gradient_button);

    smooth_button = new QToolButton(widget);
    smooth_button->setObjectName(QString::fromUtf8("smooth_button"));
    smooth_button->setEnabled(false);
    smooth_button->setIcon(QIcon(QString::fromUtf8(":/images/blur-22.png")));
    smooth_button->setCheckable(true);
    smooth_button->setAutoExclusive(true);

    hboxLayout->addWidget(smooth_button);

    clone_button = new QToolButton(widget);
    clone_button->setObjectName(QString::fromUtf8("clone_button"));
    clone_button->setEnabled(true);
    clone_button->setIcon(QIcon(QString::fromUtf8(":/images/clone-22.png")));
    clone_button->setCheckable(true);
    clone_button->setAutoExclusive(true);

    hboxLayout->addWidget(clone_button);

    pick_button = new QToolButton(widget);
    pick_button->setObjectName(QString::fromUtf8("pick_button"));
    pick_button->setEnabled(true);
    pick_button->setIcon(QIcon(QString::fromUtf8(":/images/color-picker-22.png")));
    pick_button->setCheckable(true);
    pick_button->setAutoExclusive(true);

    hboxLayout->addWidget(pick_button);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);


    gridLayout1->addWidget(widget, 0, 0, 1, 2);

    line = new QFrame(tab);
    line->setObjectName(QString::fromUtf8("line"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(line->sizePolicy().hasHeightForWidth());
    line->setSizePolicy(sizePolicy1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    gridLayout1->addWidget(line, 1, 0, 1, 2);

    color_frame = new QFrame(tab);
    color_frame->setObjectName(QString::fromUtf8("color_frame"));
    color_frame->setMinimumSize(QSize(0, 90));
    color_frame->setFrameShape(QFrame::StyledPanel);
    color_frame->setFrameShadow(QFrame::Raised);
    default_colors = new QToolButton(color_frame);
    default_colors->setObjectName(QString::fromUtf8("default_colors"));
    default_colors->setGeometry(QRect(20, 60, 25, 24));
    default_colors->setIcon(QIcon(QString::fromUtf8(":/images/bw.png")));
    default_colors->setIconSize(QSize(15, 15));
    default_colors->setAutoRaise(true);
    switch_colors = new QToolButton(color_frame);
    switch_colors->setObjectName(QString::fromUtf8("switch_colors"));
    switch_colors->setGeometry(QRect(110, 0, 25, 24));
    switch_colors->setIcon(QIcon(QString::fromUtf8(":/images/swap-colors-12.png")));
    switch_colors->setIconSize(QSize(15, 15));
    switch_colors->setCheckable(false);
    switch_colors->setAutoRaise(true);
    switch_colors->setArrowType(Qt::NoArrow);
    background_frame = new Colorframe(color_frame);
    background_frame->setObjectName(QString::fromUtf8("background_frame"));
    background_frame->setGeometry(QRect(60, 30, 61, 41));
    QPalette palette;
    QBrush brush(QColor(255, 255, 255, 255));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Base, brush);
    palette.setBrush(QPalette::Active, QPalette::Window, brush);
    palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
    palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
    palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
    palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
    background_frame->setPalette(palette);
    background_frame->setAutoFillBackground(true);
    background_frame->setFrameShape(QFrame::StyledPanel);
    background_frame->setFrameShadow(QFrame::Plain);
    background_frame->setLineWidth(2);
    foreground_frame = new Colorframe(color_frame);
    foreground_frame->setObjectName(QString::fromUtf8("foreground_frame"));
    foreground_frame->setGeometry(QRect(40, 20, 61, 41));
    QPalette palette1;
    palette1.setBrush(QPalette::Active, QPalette::Base, brush);
    QBrush brush1(QColor(0, 0, 0, 255));
    brush1.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Window, brush1);
    palette1.setBrush(QPalette::Inactive, QPalette::Base, brush);
    palette1.setBrush(QPalette::Inactive, QPalette::Window, brush1);
    palette1.setBrush(QPalette::Disabled, QPalette::Base, brush1);
    palette1.setBrush(QPalette::Disabled, QPalette::Window, brush1);
    foreground_frame->setPalette(palette1);
    foreground_frame->setAutoFillBackground(true);
    foreground_frame->setFrameShape(QFrame::StyledPanel);
    foreground_frame->setFrameShadow(QFrame::Plain);
    foreground_frame->setLineWidth(2);

    gridLayout1->addWidget(color_frame, 2, 0, 1, 1);

    undo_frame = new QFrame(tab);
    undo_frame->setObjectName(QString::fromUtf8("undo_frame"));
    undo_frame->setFrameShape(QFrame::StyledPanel);
    undo_frame->setFrameShadow(QFrame::Raised);
    redo_button = new QToolButton(undo_frame);
    redo_button->setObjectName(QString::fromUtf8("redo_button"));
    redo_button->setEnabled(false);
    redo_button->setGeometry(QRect(11, 47, 131, 30));
    redo_button->setIcon(QIcon(QString::fromUtf8(":/images/redo-24.png")));
    redo_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    undo_button = new QToolButton(undo_frame);
    undo_button->setObjectName(QString::fromUtf8("undo_button"));
    undo_button->setEnabled(false);
    undo_button->setGeometry(QRect(11, 11, 131, 30));
    undo_button->setIcon(QIcon(QString::fromUtf8(":/images/undo-24.png")));
    undo_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    gridLayout1->addWidget(undo_frame, 2, 1, 1, 1);

    pen_frame = new QFrame(tab);
    pen_frame->setObjectName(QString::fromUtf8("pen_frame"));
    QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(pen_frame->sizePolicy().hasHeightForWidth());
    pen_frame->setSizePolicy(sizePolicy2);
    pen_frame->setFrameShape(QFrame::StyledPanel);
    pen_frame->setFrameShadow(QFrame::Raised);
    gridLayout2 = new QGridLayout(pen_frame);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    gridLayout2->setHorizontalSpacing(6);
    gridLayout2->setVerticalSpacing(6);
    gridLayout2->setContentsMargins(9, 9, 9, 9);
    deck_box = new QSpinBox(pen_frame);
    deck_box->setObjectName(QString::fromUtf8("deck_box"));
    deck_box->setMaximum(100);
    deck_box->setValue(100);

    gridLayout2->addWidget(deck_box, 0, 2, 1, 1);

    label_deck = new QLabel(pen_frame);
    label_deck->setObjectName(QString::fromUtf8("label_deck"));

    gridLayout2->addWidget(label_deck, 0, 0, 1, 1);

    label_penmodus = new QLabel(pen_frame);
    label_penmodus->setObjectName(QString::fromUtf8("label_penmodus"));
    label_penmodus->setEnabled(false);

    gridLayout2->addWidget(label_penmodus, 1, 0, 1, 1);

    pen_modus = new QComboBox(pen_frame);
    pen_modus->setObjectName(QString::fromUtf8("pen_modus"));
    pen_modus->setEnabled(false);
    sizePolicy2.setHeightForWidth(pen_modus->sizePolicy().hasHeightForWidth());
    pen_modus->setSizePolicy(sizePolicy2);

    gridLayout2->addWidget(pen_modus, 1, 1, 1, 2);

    opacity_slider = new QSlider(pen_frame);
    opacity_slider->setObjectName(QString::fromUtf8("opacity_slider"));
    opacity_slider->setMaximum(100);
    opacity_slider->setValue(100);
    opacity_slider->setOrientation(Qt::Horizontal);

    gridLayout2->addWidget(opacity_slider, 0, 1, 1, 1);


    gridLayout1->addWidget(pen_frame, 3, 0, 1, 2);

    smooth_frame = new QFrame(tab);
    smooth_frame->setObjectName(QString::fromUtf8("smooth_frame"));
    sizePolicy2.setHeightForWidth(smooth_frame->sizePolicy().hasHeightForWidth());
    smooth_frame->setSizePolicy(sizePolicy2);
    smooth_frame->setFrameShape(QFrame::StyledPanel);
    smooth_frame->setFrameShadow(QFrame::Raised);
    gridLayout3 = new QGridLayout(smooth_frame);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    gridLayout3->setHorizontalSpacing(6);
    gridLayout3->setVerticalSpacing(6);
    gridLayout3->setContentsMargins(9, 9, 9, 9);
    percentual_slider = new QSlider(smooth_frame);
    percentual_slider->setObjectName(QString::fromUtf8("percentual_slider"));
    percentual_slider->setMaximum(100);
    percentual_slider->setValue(25);
    percentual_slider->setSliderPosition(25);
    percentual_slider->setOrientation(Qt::Horizontal);

    gridLayout3->addWidget(percentual_slider, 0, 1, 1, 1);

    label_percentual = new QLabel(smooth_frame);
    label_percentual->setObjectName(QString::fromUtf8("label_percentual"));

    gridLayout3->addWidget(label_percentual, 0, 0, 1, 1);

    percentual_box = new QSpinBox(smooth_frame);
    percentual_box->setObjectName(QString::fromUtf8("percentual_box"));
    percentual_box->setMaximum(100);
    percentual_box->setValue(24);

    gridLayout3->addWidget(percentual_box, 0, 2, 1, 1);


    gridLayout1->addWidget(smooth_frame, 4, 0, 1, 2);

    pen_extra_frame = new QFrame(tab);
    pen_extra_frame->setObjectName(QString::fromUtf8("pen_extra_frame"));
    pen_extra_frame->setEnabled(true);
    sizePolicy2.setHeightForWidth(pen_extra_frame->sizePolicy().hasHeightForWidth());
    pen_extra_frame->setSizePolicy(sizePolicy2);
    pen_extra_frame->setFrameShape(QFrame::StyledPanel);
    pen_extra_frame->setFrameShadow(QFrame::Raised);
    gridLayout4 = new QGridLayout(pen_extra_frame);
    gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
    label_pen = new QLabel(pen_extra_frame);
    label_pen->setObjectName(QString::fromUtf8("label_pen"));

    gridLayout4->addWidget(label_pen, 0, 0, 1, 1);

    brush_box = new QComboBox(pen_extra_frame);
    brush_box->setObjectName(QString::fromUtf8("brush_box"));
    brush_box->setEnabled(false);
    sizePolicy2.setHeightForWidth(brush_box->sizePolicy().hasHeightForWidth());
    brush_box->setSizePolicy(sizePolicy2);
    brush_box->setEditable(false);

    gridLayout4->addWidget(brush_box, 0, 2, 1, 3);

    label_percentual_3 = new QLabel(pen_extra_frame);
    label_percentual_3->setObjectName(QString::fromUtf8("label_percentual_3"));

    gridLayout4->addWidget(label_percentual_3, 1, 0, 1, 2);

    hardness_slider = new QSlider(pen_extra_frame);
    hardness_slider->setObjectName(QString::fromUtf8("hardness_slider"));
    hardness_slider->setMaximum(100);
    hardness_slider->setValue(50);
    hardness_slider->setSliderPosition(50);
    hardness_slider->setOrientation(Qt::Horizontal);

    gridLayout4->addWidget(hardness_slider, 1, 2, 1, 2);

    decrease_box = new QSpinBox(pen_extra_frame);
    decrease_box->setObjectName(QString::fromUtf8("decrease_box"));
    decrease_box->setMaximum(100);
    decrease_box->setValue(47);

    gridLayout4->addWidget(decrease_box, 1, 4, 1, 1);

    label_percentual_4 = new QLabel(pen_extra_frame);
    label_percentual_4->setObjectName(QString::fromUtf8("label_percentual_4"));

    gridLayout4->addWidget(label_percentual_4, 2, 0, 1, 2);

    size_slider = new QSlider(pen_extra_frame);
    size_slider->setObjectName(QString::fromUtf8("size_slider"));
    size_slider->setMaximum(100);
    size_slider->setValue(20);
    size_slider->setSliderPosition(20);
    size_slider->setOrientation(Qt::Horizontal);

    gridLayout4->addWidget(size_slider, 2, 2, 1, 2);

    decrease_box_3 = new QSpinBox(pen_extra_frame);
    decrease_box_3->setObjectName(QString::fromUtf8("decrease_box_3"));
    decrease_box_3->setMaximum(100);
    decrease_box_3->setValue(20);

    gridLayout4->addWidget(decrease_box_3, 2, 4, 1, 1);

    label_percentual_5 = new QLabel(pen_extra_frame);
    label_percentual_5->setObjectName(QString::fromUtf8("label_percentual_5"));

    gridLayout4->addWidget(label_percentual_5, 3, 0, 1, 2);

    pen_unit = new QComboBox(pen_extra_frame);
    pen_unit->setObjectName(QString::fromUtf8("pen_unit"));
    sizePolicy2.setHeightForWidth(pen_unit->sizePolicy().hasHeightForWidth());
    pen_unit->setSizePolicy(sizePolicy2);

    gridLayout4->addWidget(pen_unit, 3, 2, 1, 3);

    label = new QLabel(pen_extra_frame);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout4->addWidget(label, 4, 0, 1, 1);

    backface_culling = new QCheckBox(pen_extra_frame);
    backface_culling->setObjectName(QString::fromUtf8("backface_culling"));

    gridLayout4->addWidget(backface_culling, 4, 1, 1, 2);

    invisible_painting = new QCheckBox(pen_extra_frame);
    invisible_painting->setObjectName(QString::fromUtf8("invisible_painting"));

    gridLayout4->addWidget(invisible_painting, 4, 3, 1, 2);


    gridLayout1->addWidget(pen_extra_frame, 5, 0, 1, 2);

    pick_frame = new QFrame(tab);
    pick_frame->setObjectName(QString::fromUtf8("pick_frame"));
    pick_frame->setFrameShape(QFrame::StyledPanel);
    pick_frame->setFrameShadow(QFrame::Raised);
    gridLayout5 = new QGridLayout(pick_frame);
    gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
    gridLayout5->setHorizontalSpacing(6);
    gridLayout5->setVerticalSpacing(6);
    gridLayout5->setContentsMargins(9, 9, 9, 9);
    pick_mode = new QComboBox(pick_frame);
    pick_mode->setObjectName(QString::fromUtf8("pick_mode"));
    sizePolicy2.setHeightForWidth(pick_mode->sizePolicy().hasHeightForWidth());
    pick_mode->setSizePolicy(sizePolicy2);

    gridLayout5->addWidget(pick_mode, 0, 1, 1, 1);

    label_pick = new QLabel(pick_frame);
    label_pick->setObjectName(QString::fromUtf8("label_pick"));

    gridLayout5->addWidget(label_pick, 0, 0, 1, 1);


    gridLayout1->addWidget(pick_frame, 6, 0, 1, 2);

    gradient_frame = new QFrame(tab);
    gradient_frame->setObjectName(QString::fromUtf8("gradient_frame"));
    sizePolicy2.setHeightForWidth(gradient_frame->sizePolicy().hasHeightForWidth());
    gradient_frame->setSizePolicy(sizePolicy2);
    gradient_frame->setFrameShape(QFrame::StyledPanel);
    gradient_frame->setFrameShadow(QFrame::Raised);
    gridLayout6 = new QGridLayout(gradient_frame);
    gridLayout6->setObjectName(QString::fromUtf8("gridLayout6"));
    gridLayout6->setHorizontalSpacing(6);
    gridLayout6->setVerticalSpacing(6);
    gridLayout6->setContentsMargins(9, 9, 9, 9);
    label_4 = new QLabel(gradient_frame);
    label_4->setObjectName(QString::fromUtf8("label_4"));

    gridLayout6->addWidget(label_4, 0, 0, 1, 1);

    gradient_type = new QComboBox(gradient_frame);
    gradient_type->setObjectName(QString::fromUtf8("gradient_type"));
    sizePolicy2.setHeightForWidth(gradient_type->sizePolicy().hasHeightForWidth());
    gradient_type->setSizePolicy(sizePolicy2);

    gridLayout6->addWidget(gradient_type, 0, 1, 1, 2);

    label_3 = new QLabel(gradient_frame);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout6->addWidget(label_3, 1, 0, 1, 2);

    gradient_form = new QComboBox(gradient_frame);
    gradient_form->setObjectName(QString::fromUtf8("gradient_form"));

    gridLayout6->addWidget(gradient_form, 1, 2, 1, 1);


    gridLayout1->addWidget(gradient_frame, 7, 0, 1, 2);

    pressure_frame = new QFrame(tab);
    pressure_frame->setObjectName(QString::fromUtf8("pressure_frame"));
    pressure_frame->setEnabled(false);
    pressure_frame->setFrameShape(QFrame::StyledPanel);
    pressure_frame->setFrameShadow(QFrame::Raised);
    gridLayout7 = new QGridLayout(pressure_frame);
    gridLayout7->setObjectName(QString::fromUtf8("gridLayout7"));
    label_7 = new QLabel(pressure_frame);
    label_7->setObjectName(QString::fromUtf8("label_7"));

    gridLayout7->addWidget(label_7, 0, 0, 1, 3);

    opacity_box = new QCheckBox(pressure_frame);
    opacity_box->setObjectName(QString::fromUtf8("opacity_box"));

    gridLayout7->addWidget(opacity_box, 1, 0, 1, 1);

    hardness_box = new QCheckBox(pressure_frame);
    hardness_box->setObjectName(QString::fromUtf8("hardness_box"));

    gridLayout7->addWidget(hardness_box, 1, 1, 1, 1);

    size_box = new QCheckBox(pressure_frame);
    size_box->setObjectName(QString::fromUtf8("size_box"));

    gridLayout7->addWidget(size_box, 1, 2, 1, 1);


    gridLayout1->addWidget(pressure_frame, 8, 0, 1, 2);

    spacerItem1 = new QSpacerItem(311, 61, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout1->addItem(spacerItem1, 9, 0, 1, 2);

    tabs_container->addTab(tab, QString());
    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    vboxLayout = new QVBoxLayout(tab_2);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    widget_2 = new QWidget(tab_2);
    widget_2->setObjectName(QString::fromUtf8("widget_2"));
    sizePolicy2.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
    widget_2->setSizePolicy(sizePolicy2);
    widget_2->setMinimumSize(QSize(0, 0));
    hboxLayout1 = new QHBoxLayout(widget_2);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    hboxLayout1->setContentsMargins(0, 0, 0, 0);
    mesh_pick_button = new QToolButton(widget_2);
    mesh_pick_button->setObjectName(QString::fromUtf8("mesh_pick_button"));
    mesh_pick_button->setEnabled(true);
    mesh_pick_button->setIcon(QIcon(QString::fromUtf8(":/images/pickbrush-22.png")));
    mesh_pick_button->setCheckable(true);
    mesh_pick_button->setChecked(true);
    mesh_pick_button->setAutoExclusive(true);

    hboxLayout1->addWidget(mesh_pick_button);

    mesh_smooth_button = new QToolButton(widget_2);
    mesh_smooth_button->setObjectName(QString::fromUtf8("mesh_smooth_button"));
    mesh_smooth_button->setEnabled(false);
    mesh_smooth_button->setIcon(QIcon(QString::fromUtf8(":/images/blur-22.png")));
    mesh_smooth_button->setCheckable(true);
    mesh_smooth_button->setAutoExclusive(true);

    hboxLayout1->addWidget(mesh_smooth_button);

    mesh_sculpt_button = new QToolButton(widget_2);
    mesh_sculpt_button->setObjectName(QString::fromUtf8("mesh_sculpt_button"));
    mesh_sculpt_button->setIcon(QIcon(QString::fromUtf8(":/images/sculpt-22.png")));
    mesh_sculpt_button->setCheckable(true);
    mesh_sculpt_button->setAutoExclusive(true);

    hboxLayout1->addWidget(mesh_sculpt_button);

    mesh_add_button = new QToolButton(widget_2);
    mesh_add_button->setObjectName(QString::fromUtf8("mesh_add_button"));
    mesh_add_button->setEnabled(false);
    mesh_add_button->setIcon(QIcon(QString::fromUtf8(":/images/add-24.png")));
    mesh_add_button->setCheckable(true);
    mesh_add_button->setAutoExclusive(true);

    hboxLayout1->addWidget(mesh_add_button);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem2);


    vboxLayout->addWidget(widget_2);

    line_2 = new QFrame(tab_2);
    line_2->setObjectName(QString::fromUtf8("line_2"));
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);

    vboxLayout->addWidget(line_2);

    mesh_undo_frame = new QFrame(tab_2);
    mesh_undo_frame->setObjectName(QString::fromUtf8("mesh_undo_frame"));
    mesh_undo_frame->setFrameShape(QFrame::StyledPanel);
    mesh_undo_frame->setFrameShadow(QFrame::Raised);
    hboxLayout2 = new QHBoxLayout(mesh_undo_frame);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    mesh_undo_button = new QToolButton(mesh_undo_frame);
    mesh_undo_button->setObjectName(QString::fromUtf8("mesh_undo_button"));
    mesh_undo_button->setEnabled(false);
    QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(mesh_undo_button->sizePolicy().hasHeightForWidth());
    mesh_undo_button->setSizePolicy(sizePolicy3);
    mesh_undo_button->setLayoutDirection(Qt::RightToLeft);
    mesh_undo_button->setIcon(QIcon(QString::fromUtf8(":/images/undo-24.png")));
    mesh_undo_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    hboxLayout2->addWidget(mesh_undo_button);

    mesh_redo_button = new QToolButton(mesh_undo_frame);
    mesh_redo_button->setObjectName(QString::fromUtf8("mesh_redo_button"));
    mesh_redo_button->setEnabled(false);
    sizePolicy3.setHeightForWidth(mesh_redo_button->sizePolicy().hasHeightForWidth());
    mesh_redo_button->setSizePolicy(sizePolicy3);
    mesh_redo_button->setLayoutDirection(Qt::LeftToRight);
    mesh_redo_button->setIcon(QIcon(QString::fromUtf8(":/images/redo-24.png")));
    mesh_redo_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    hboxLayout2->addWidget(mesh_redo_button);


    vboxLayout->addWidget(mesh_undo_frame);

    mesh_smooth_frame = new QFrame(tab_2);
    mesh_smooth_frame->setObjectName(QString::fromUtf8("mesh_smooth_frame"));
    mesh_smooth_frame->setFrameShape(QFrame::StyledPanel);
    mesh_smooth_frame->setFrameShadow(QFrame::Raised);
    gridLayout8 = new QGridLayout(mesh_smooth_frame);
    gridLayout8->setObjectName(QString::fromUtf8("gridLayout8"));
    gridLayout8->setHorizontalSpacing(6);
    gridLayout8->setVerticalSpacing(6);
    gridLayout8->setContentsMargins(9, 9, 9, 9);
    mesh_percentual_slider = new QSlider(mesh_smooth_frame);
    mesh_percentual_slider->setObjectName(QString::fromUtf8("mesh_percentual_slider"));
    mesh_percentual_slider->setMaximum(100);
    mesh_percentual_slider->setValue(25);
    mesh_percentual_slider->setSliderPosition(25);
    mesh_percentual_slider->setOrientation(Qt::Horizontal);

    gridLayout8->addWidget(mesh_percentual_slider, 0, 1, 1, 1);

    label_percentual_2 = new QLabel(mesh_smooth_frame);
    label_percentual_2->setObjectName(QString::fromUtf8("label_percentual_2"));

    gridLayout8->addWidget(label_percentual_2, 0, 0, 1, 1);

    percentual_box_2 = new QSpinBox(mesh_smooth_frame);
    percentual_box_2->setObjectName(QString::fromUtf8("percentual_box_2"));
    percentual_box_2->setMaximum(100);
    percentual_box_2->setValue(25);

    gridLayout8->addWidget(percentual_box_2, 0, 2, 1, 1);


    vboxLayout->addWidget(mesh_smooth_frame);

    mesh_pen_extra_frame = new QFrame(tab_2);
    mesh_pen_extra_frame->setObjectName(QString::fromUtf8("mesh_pen_extra_frame"));
    mesh_pen_extra_frame->setEnabled(true);
    mesh_pen_extra_frame->setFrameShape(QFrame::StyledPanel);
    mesh_pen_extra_frame->setFrameShadow(QFrame::Raised);
    gridLayout9 = new QGridLayout(mesh_pen_extra_frame);
    gridLayout9->setObjectName(QString::fromUtf8("gridLayout9"));
    label_pen_2 = new QLabel(mesh_pen_extra_frame);
    label_pen_2->setObjectName(QString::fromUtf8("label_pen_2"));

    gridLayout9->addWidget(label_pen_2, 0, 0, 1, 1);

    mesh_brush_box = new QComboBox(mesh_pen_extra_frame);
    mesh_brush_box->setObjectName(QString::fromUtf8("mesh_brush_box"));
    mesh_brush_box->setEnabled(false);
    mesh_brush_box->setEditable(false);

    gridLayout9->addWidget(mesh_brush_box, 0, 2, 1, 3);

    label_percentual_6 = new QLabel(mesh_pen_extra_frame);
    label_percentual_6->setObjectName(QString::fromUtf8("label_percentual_6"));

    gridLayout9->addWidget(label_percentual_6, 1, 0, 1, 2);

    mesh_hardness_slider = new QSlider(mesh_pen_extra_frame);
    mesh_hardness_slider->setObjectName(QString::fromUtf8("mesh_hardness_slider"));
    mesh_hardness_slider->setMaximum(100);
    mesh_hardness_slider->setValue(50);
    mesh_hardness_slider->setSliderPosition(50);
    mesh_hardness_slider->setOrientation(Qt::Horizontal);

    gridLayout9->addWidget(mesh_hardness_slider, 1, 2, 1, 2);

    decrease_box_2 = new QSpinBox(mesh_pen_extra_frame);
    decrease_box_2->setObjectName(QString::fromUtf8("decrease_box_2"));
    decrease_box_2->setMaximum(100);
    decrease_box_2->setValue(47);

    gridLayout9->addWidget(decrease_box_2, 1, 4, 1, 1);

    label_percentual_7 = new QLabel(mesh_pen_extra_frame);
    label_percentual_7->setObjectName(QString::fromUtf8("label_percentual_7"));

    gridLayout9->addWidget(label_percentual_7, 2, 0, 1, 2);

    mesh_size_slider = new QSlider(mesh_pen_extra_frame);
    mesh_size_slider->setObjectName(QString::fromUtf8("mesh_size_slider"));
    mesh_size_slider->setMaximum(100);
    mesh_size_slider->setValue(50);
    mesh_size_slider->setSliderPosition(50);
    mesh_size_slider->setOrientation(Qt::Horizontal);

    gridLayout9->addWidget(mesh_size_slider, 2, 2, 1, 2);

    decrease_box_4 = new QSpinBox(mesh_pen_extra_frame);
    decrease_box_4->setObjectName(QString::fromUtf8("decrease_box_4"));
    decrease_box_4->setMaximum(100);
    decrease_box_4->setValue(47);

    gridLayout9->addWidget(decrease_box_4, 2, 4, 1, 1);

    label_percentual_8 = new QLabel(mesh_pen_extra_frame);
    label_percentual_8->setObjectName(QString::fromUtf8("label_percentual_8"));

    gridLayout9->addWidget(label_percentual_8, 3, 0, 1, 2);

    mesh_pen_unit = new QComboBox(mesh_pen_extra_frame);
    mesh_pen_unit->setObjectName(QString::fromUtf8("mesh_pen_unit"));
    QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(mesh_pen_unit->sizePolicy().hasHeightForWidth());
    mesh_pen_unit->setSizePolicy(sizePolicy4);

    gridLayout9->addWidget(mesh_pen_unit, 3, 2, 1, 3);

    label_5 = new QLabel(mesh_pen_extra_frame);
    label_5->setObjectName(QString::fromUtf8("label_5"));

    gridLayout9->addWidget(label_5, 4, 0, 1, 1);

    backface_culling_2 = new QCheckBox(mesh_pen_extra_frame);
    backface_culling_2->setObjectName(QString::fromUtf8("backface_culling_2"));

    gridLayout9->addWidget(backface_culling_2, 4, 1, 1, 2);

    invisible_painting_2 = new QCheckBox(mesh_pen_extra_frame);
    invisible_painting_2->setObjectName(QString::fromUtf8("invisible_painting_2"));

    gridLayout9->addWidget(invisible_painting_2, 4, 3, 1, 2);


    vboxLayout->addWidget(mesh_pen_extra_frame);

    mesh_displacement_frame = new QFrame(tab_2);
    mesh_displacement_frame->setObjectName(QString::fromUtf8("mesh_displacement_frame"));
    mesh_displacement_frame->setEnabled(false);
    mesh_displacement_frame->setFrameShape(QFrame::StyledPanel);
    mesh_displacement_frame->setFrameShadow(QFrame::Raised);
    gridLayout10 = new QGridLayout(mesh_displacement_frame);
    gridLayout10->setObjectName(QString::fromUtf8("gridLayout10"));
    label_6 = new QLabel(mesh_displacement_frame);
    label_6->setObjectName(QString::fromUtf8("label_6"));

    gridLayout10->addWidget(label_6, 0, 0, 1, 1);

    mesh_displacement_slider = new QSlider(mesh_displacement_frame);
    mesh_displacement_slider->setObjectName(QString::fromUtf8("mesh_displacement_slider"));
    mesh_displacement_slider->setOrientation(Qt::Horizontal);

    gridLayout10->addWidget(mesh_displacement_slider, 0, 1, 1, 1);

    spinBox = new QSpinBox(mesh_displacement_frame);
    spinBox->setObjectName(QString::fromUtf8("spinBox"));

    gridLayout10->addWidget(spinBox, 0, 2, 1, 1);

    label_8 = new QLabel(mesh_displacement_frame);
    label_8->setObjectName(QString::fromUtf8("label_8"));

    gridLayout10->addWidget(label_8, 1, 0, 1, 1);

    mesh_displacement_direction = new QComboBox(mesh_displacement_frame);
    mesh_displacement_direction->setObjectName(QString::fromUtf8("mesh_displacement_direction"));

    gridLayout10->addWidget(mesh_displacement_direction, 1, 1, 1, 2);


    vboxLayout->addWidget(mesh_displacement_frame);

    mesh_pressure_frame = new QFrame(tab_2);
    mesh_pressure_frame->setObjectName(QString::fromUtf8("mesh_pressure_frame"));
    mesh_pressure_frame->setEnabled(false);
    mesh_pressure_frame->setFrameShape(QFrame::StyledPanel);
    mesh_pressure_frame->setFrameShadow(QFrame::Raised);
    gridLayout11 = new QGridLayout(mesh_pressure_frame);
    gridLayout11->setObjectName(QString::fromUtf8("gridLayout11"));
    label_9 = new QLabel(mesh_pressure_frame);
    label_9->setObjectName(QString::fromUtf8("label_9"));

    gridLayout11->addWidget(label_9, 0, 0, 1, 3);

    displacement_box = new QCheckBox(mesh_pressure_frame);
    displacement_box->setObjectName(QString::fromUtf8("displacement_box"));

    gridLayout11->addWidget(displacement_box, 1, 0, 1, 1);

    mesh_hardness_box = new QCheckBox(mesh_pressure_frame);
    mesh_hardness_box->setObjectName(QString::fromUtf8("mesh_hardness_box"));

    gridLayout11->addWidget(mesh_hardness_box, 1, 1, 1, 1);

    mesh_size_box = new QCheckBox(mesh_pressure_frame);
    mesh_size_box->setObjectName(QString::fromUtf8("mesh_size_box"));

    gridLayout11->addWidget(mesh_size_box, 1, 2, 1, 1);


    vboxLayout->addWidget(mesh_pressure_frame);

    spacerItem3 = new QSpacerItem(314, 151, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem3);

    tabs_container->addTab(tab_2, QString());
    tab_3 = new QWidget();
    tab_3->setObjectName(QString::fromUtf8("tab_3"));
    gridLayout12 = new QGridLayout(tab_3);
    gridLayout12->setObjectName(QString::fromUtf8("gridLayout12"));
    gridLayout12->setHorizontalSpacing(6);
    gridLayout12->setVerticalSpacing(6);
    gridLayout12->setContentsMargins(9, 9, 9, 9);
    search_mode = new QComboBox(tab_3);
    search_mode->setObjectName(QString::fromUtf8("search_mode"));

    gridLayout12->addWidget(search_mode, 0, 1, 1, 1);

    label_2 = new QLabel(tab_3);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout12->addWidget(label_2, 0, 0, 1, 1);

    spacerItem4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout12->addItem(spacerItem4, 1, 0, 1, 1);

    tabs_container->addTab(tab_3, QString());

    gridLayout->addWidget(tabs_container, 0, 0, 1, 1);


    retranslateUi(Paintbox);
    QObject::connect(mesh_percentual_slider, SIGNAL(valueChanged(int)), percentual_box_2, SLOT(setValue(int)));
    QObject::connect(mesh_hardness_slider, SIGNAL(valueChanged(int)), decrease_box_2, SLOT(setValue(int)));
    QObject::connect(mesh_displacement_slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
    QObject::connect(spinBox, SIGNAL(valueChanged(int)), mesh_displacement_slider, SLOT(setValue(int)));
    QObject::connect(percentual_box_2, SIGNAL(valueChanged(int)), mesh_percentual_slider, SLOT(setValue(int)));
    QObject::connect(opacity_slider, SIGNAL(valueChanged(int)), deck_box, SLOT(setValue(int)));
    QObject::connect(deck_box, SIGNAL(valueChanged(int)), opacity_slider, SLOT(setValue(int)));
    QObject::connect(mesh_size_slider, SIGNAL(valueChanged(int)), decrease_box_4, SLOT(setValue(int)));
    QObject::connect(decrease_box_4, SIGNAL(valueChanged(int)), mesh_size_slider, SLOT(setValue(int)));
    QObject::connect(decrease_box_2, SIGNAL(valueChanged(int)), mesh_hardness_slider, SLOT(setValue(int)));
    QObject::connect(percentual_slider, SIGNAL(valueChanged(int)), percentual_box, SLOT(setValue(int)));
    QObject::connect(percentual_box, SIGNAL(valueChanged(int)), percentual_slider, SLOT(setValue(int)));
    QObject::connect(hardness_slider, SIGNAL(valueChanged(int)), decrease_box, SLOT(setValue(int)));
    QObject::connect(mesh_pick_button, SIGNAL(toggled(bool)), mesh_pen_extra_frame, SLOT(setVisible(bool)));
    QObject::connect(decrease_box, SIGNAL(valueChanged(int)), hardness_slider, SLOT(setValue(int)));
    QObject::connect(size_slider, SIGNAL(valueChanged(int)), decrease_box_3, SLOT(setValue(int)));
    QObject::connect(pen_button, SIGNAL(toggled(bool)), pen_frame, SLOT(setVisible(bool)));
    QObject::connect(pen_button, SIGNAL(toggled(bool)), pressure_frame, SLOT(setVisible(bool)));
    QObject::connect(pen_button, SIGNAL(toggled(bool)), pen_extra_frame, SLOT(setVisible(bool)));
    QObject::connect(decrease_box_3, SIGNAL(valueChanged(int)), size_slider, SLOT(setValue(int)));
    QObject::connect(gradient_button, SIGNAL(toggled(bool)), gradient_frame, SLOT(setVisible(bool)));
    QObject::connect(fill_button, SIGNAL(toggled(bool)), pen_frame, SLOT(setVisible(bool)));
    QObject::connect(gradient_button, SIGNAL(toggled(bool)), pen_frame, SLOT(setVisible(bool)));
    QObject::connect(smooth_button, SIGNAL(toggled(bool)), smooth_frame, SLOT(setVisible(bool)));
    QObject::connect(mesh_smooth_button, SIGNAL(toggled(bool)), mesh_smooth_frame, SLOT(setVisible(bool)));
    QObject::connect(smooth_button, SIGNAL(toggled(bool)), pen_extra_frame, SLOT(setVisible(bool)));
    QObject::connect(smooth_button, SIGNAL(toggled(bool)), pressure_frame, SLOT(setVisible(bool)));
    QObject::connect(pick_button, SIGNAL(toggled(bool)), pick_frame, SLOT(setVisible(bool)));
    QObject::connect(mesh_sculpt_button, SIGNAL(toggled(bool)), mesh_displacement_frame, SLOT(setVisible(bool)));
    QObject::connect(mesh_add_button, SIGNAL(toggled(bool)), mesh_displacement_frame, SLOT(setVisible(bool)));
    QObject::connect(mesh_sculpt_button, SIGNAL(toggled(bool)), mesh_pen_extra_frame, SLOT(setVisible(bool)));
    QObject::connect(mesh_sculpt_button, SIGNAL(toggled(bool)), mesh_pressure_frame, SLOT(setVisible(bool)));
    QObject::connect(mesh_add_button, SIGNAL(toggled(bool)), mesh_pressure_frame, SLOT(setVisible(bool)));
    QObject::connect(mesh_add_button, SIGNAL(toggled(bool)), mesh_pen_extra_frame, SLOT(setVisible(bool)));
    QObject::connect(mesh_pick_button, SIGNAL(toggled(bool)), mesh_pressure_frame, SLOT(setVisible(bool)));
    QObject::connect(mesh_smooth_button, SIGNAL(toggled(bool)), mesh_pen_extra_frame, SLOT(setVisible(bool)));

    tabs_container->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(Paintbox);
    } // setupUi

    void retranslateUi(QWidget *Paintbox)
    {
    Paintbox->setWindowTitle(QApplication::translate("Paintbox", "Vertex Painting", 0, QApplication::UnicodeUTF8));
    pen_button->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    fill_button->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    gradient_button->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    smooth_button->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    clone_button->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    pick_button->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    default_colors->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    switch_colors->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    redo_button->setText(QApplication::translate("Paintbox", "Redo", 0, QApplication::UnicodeUTF8));
    undo_button->setText(QApplication::translate("Paintbox", "Undo", 0, QApplication::UnicodeUTF8));
    deck_box->setToolTip(QApplication::translate("Paintbox", "Change the opacity of the color (this is not the opengl alpha value)", 0, QApplication::UnicodeUTF8));
    label_deck->setText(QApplication::translate("Paintbox", "Opacity", 0, QApplication::UnicodeUTF8));
    label_penmodus->setText(QApplication::translate("Paintbox", "Modus", 0, QApplication::UnicodeUTF8));
    pen_modus->clear();
    pen_modus->insertItems(0, QStringList()
     << QApplication::translate("Paintbox", "Normal", 0, QApplication::UnicodeUTF8)
    );
    opacity_slider->setToolTip(QApplication::translate("Paintbox", "Change the opacity of the color (this is not the opengl alpha value)", 0, QApplication::UnicodeUTF8));
    percentual_slider->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_percentual->setText(QApplication::translate("Paintbox", "Percentual", 0, QApplication::UnicodeUTF8));
    percentual_box->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_pen->setText(QApplication::translate("Paintbox", "Brush", 0, QApplication::UnicodeUTF8));
    brush_box->clear();
    brush_box->addItem(QIcon(QString::fromUtf8(":/brushes/circle.png")), QApplication::translate("Paintbox", "Circle", 0, QApplication::UnicodeUTF8));
    label_percentual_3->setText(QApplication::translate("Paintbox", "Hardness", 0, QApplication::UnicodeUTF8));
    hardness_slider->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    decrease_box->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_percentual_4->setText(QApplication::translate("Paintbox", "Size", 0, QApplication::UnicodeUTF8));
    size_slider->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    decrease_box_3->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_percentual_5->setText(QApplication::translate("Paintbox", "Units", 0, QApplication::UnicodeUTF8));
    pen_unit->clear();
    pen_unit->insertItems(0, QStringList()
     << QApplication::translate("Paintbox", "pixel", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Paintbox", "percentual of bounding box", 0, QApplication::UnicodeUTF8)
    );
    label->setText(QApplication::translate("Paintbox", "Paint on:", 0, QApplication::UnicodeUTF8));
    backface_culling->setText(QApplication::translate("Paintbox", "backfaces", 0, QApplication::UnicodeUTF8));
    invisible_painting->setText(QApplication::translate("Paintbox", "hidden polygons", 0, QApplication::UnicodeUTF8));
    pick_mode->clear();
    pick_mode->insertItems(0, QStringList()
     << QApplication::translate("Paintbox", "Nearest vertex color", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Paintbox", "Pixel color", 0, QApplication::UnicodeUTF8)
    );
    label_pick->setText(QApplication::translate("Paintbox", "Picking mode", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("Paintbox", "Type", 0, QApplication::UnicodeUTF8));
    gradient_type->clear();
    gradient_type->insertItems(0, QStringList()
     << QApplication::translate("Paintbox", "Foreground to background (RGB)", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Paintbox", "Foreground to transparency", 0, QApplication::UnicodeUTF8)
    );
    label_3->setText(QApplication::translate("Paintbox", "Form", 0, QApplication::UnicodeUTF8));
    gradient_form->clear();
    gradient_form->insertItems(0, QStringList()
     << QApplication::translate("Paintbox", "Linear", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Paintbox", "Circular", 0, QApplication::UnicodeUTF8)
    );
    label_7->setText(QApplication::translate("Paintbox", "Pen pressure maps to:", 0, QApplication::UnicodeUTF8));
    opacity_box->setText(QApplication::translate("Paintbox", "Opacity", 0, QApplication::UnicodeUTF8));
    hardness_box->setText(QApplication::translate("Paintbox", "Hardness", 0, QApplication::UnicodeUTF8));
    size_box->setText(QApplication::translate("Paintbox", "Size", 0, QApplication::UnicodeUTF8));
    tabs_container->setTabText(tabs_container->indexOf(tab), QApplication::translate("Paintbox", "Painting", 0, QApplication::UnicodeUTF8));
    mesh_pick_button->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    mesh_smooth_button->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    mesh_sculpt_button->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    mesh_add_button->setText(QApplication::translate("Paintbox", "...", 0, QApplication::UnicodeUTF8));
    mesh_undo_button->setText(QApplication::translate("Paintbox", "Undo", 0, QApplication::UnicodeUTF8));
    mesh_redo_button->setText(QApplication::translate("Paintbox", "Redo", 0, QApplication::UnicodeUTF8));
    mesh_percentual_slider->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_percentual_2->setText(QApplication::translate("Paintbox", "Percentual", 0, QApplication::UnicodeUTF8));
    percentual_box_2->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_pen_2->setText(QApplication::translate("Paintbox", "Brush", 0, QApplication::UnicodeUTF8));
    mesh_brush_box->clear();
    mesh_brush_box->addItem(QIcon(QString::fromUtf8(":/brushes/circle.png")), QApplication::translate("Paintbox", "Circle", 0, QApplication::UnicodeUTF8));
    label_percentual_6->setText(QApplication::translate("Paintbox", "Hardness", 0, QApplication::UnicodeUTF8));
    mesh_hardness_slider->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    decrease_box_2->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_percentual_7->setText(QApplication::translate("Paintbox", "Size", 0, QApplication::UnicodeUTF8));
    mesh_size_slider->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    decrease_box_4->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_percentual_8->setText(QApplication::translate("Paintbox", "Units", 0, QApplication::UnicodeUTF8));
    mesh_pen_unit->clear();
    mesh_pen_unit->insertItems(0, QStringList()
     << QApplication::translate("Paintbox", "pixel", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Paintbox", "percentual of bbox diagonal", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Paintbox", "percentual of bbox height", 0, QApplication::UnicodeUTF8)
    );
    label_5->setText(QApplication::translate("Paintbox", "Paint on:", 0, QApplication::UnicodeUTF8));
    backface_culling_2->setText(QApplication::translate("Paintbox", "backfaces", 0, QApplication::UnicodeUTF8));
    invisible_painting_2->setText(QApplication::translate("Paintbox", "hidden polygons", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("Paintbox", "Displacement", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("Paintbox", "Direction", 0, QApplication::UnicodeUTF8));
    mesh_displacement_direction->clear();
    mesh_displacement_direction->insertItems(0, QStringList()
     << QApplication::translate("Paintbox", "Averaged Normals", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Paintbox", "Per Vertex Normals", 0, QApplication::UnicodeUTF8)
    );
    label_9->setText(QApplication::translate("Paintbox", "Pen pressure maps to:", 0, QApplication::UnicodeUTF8));
    displacement_box->setText(QApplication::translate("Paintbox", "Displacement", 0, QApplication::UnicodeUTF8));
    mesh_hardness_box->setText(QApplication::translate("Paintbox", "Hardness", 0, QApplication::UnicodeUTF8));
    mesh_size_box->setText(QApplication::translate("Paintbox", "Size", 0, QApplication::UnicodeUTF8));
    tabs_container->setTabText(tabs_container->indexOf(tab_2), QApplication::translate("Paintbox", "Selecting && Editing", 0, QApplication::UnicodeUTF8));
    search_mode->clear();
    search_mode->insertItems(0, QStringList()
     << QApplication::translate("Paintbox", "fast", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Paintbox", "slow but accurate", 0, QApplication::UnicodeUTF8)
    );
    search_mode->setToolTip(QApplication::translate("Paintbox", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">fast:</span> works better with big meshes, but can loose some vertexes when faces are not direct connected<br /><span style=\" font-weight:600;\">slow but accurate: </span>sholud be used with small meshes, because it finds every vertex during painting</p></body></html>", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("Paintbox", "Vertex search:", 0, QApplication::UnicodeUTF8));
    tabs_container->setTabText(tabs_container->indexOf(tab_3), QApplication::translate("Paintbox", "Configuration", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Paintbox);
    } // retranslateUi

};

namespace Ui {
    class Paintbox: public Ui_Paintbox {};
} // namespace Ui

#endif // UI_PAINTBOX_H
