#ifndef VASEWIDGET_H
#define VASEWIDGET_H

#include <QWidget>
#include <QtGui/QDockWidget>
#include "ui_vasewidget.h"
#include <balloon.h> // balloon data structure
#include <vcg/space/point3.h> //Point3f, Point3i

using namespace vcg;

class VaseWidget : public QDockWidget
{
    Q_OBJECT

public:
    void update_slice();
    VaseWidget(QWidget *parent, MeshModel &m, GLArea* gla );
    ~VaseWidget();
    void decorate(MeshModel &m, GLArea* gla);

private:
    Ui::VaseWidget ui;
    QWidget* parent;
    GLArea* gla;
    Balloon* balloon;

    // Controls for the slice UI
    // int   slice_dim; // which dimension is slice taken from
    // float slice_off; // which offset from 0 the slice is taken from

private slots:
    void on_laplButton_released();
    void on_surf2acc_check_toggled(bool checked);
    void on_surf2vol_check_toggled(bool checked);
    void on_iterationButton_released();
    void on_initButton_released();
    void on_surface_toggled(bool checked);
    void on_viewDirs_toggled(bool checked);
    void on_interpButton_released();
    void on_refreshButton_released();
    void on_evolveButton_released();
    void on_slice_offset_sliderMoved(int position);
    void on_slice_dim_combo_currentIndexChanged(int index);
};

#endif

