#ifndef U3D_GUI_H
#define U3D_GUI_H

#include <QtGui/QDialog>
#include "ui_u3d_gui.h"
#include <wrap/io_trimesh/export_u3d.h>


class U3D_GUI : public QDialog
{
	Q_OBJECT

public:
	U3D_GUI(vcg::tri::io::u3dparametersclasses::Movie15Parameters& param,QWidget *parent = 0, Qt::WFlags flags = 0);
	~U3D_GUI();

private:
	void saveParameters();
	void defaultParameters();

	Ui::U3D_GUIClass ui;
	vcg::tri::io::u3dparametersclasses::Movie15Parameters& _param;

private slots:
	void on_buttonBox_rejected();
	void on_buttonBox_accepted();
};

#endif // U3D_GUI_H
