#include "u3d_gui.h"

U3D_GUI::U3D_GUI(vcg::tri::io::u3dparametersclasses::Movie15Parameters& param,QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags),_param(param)
{
	ui.setupUi(this);
	saveParameters();
}

U3D_GUI::~U3D_GUI()
{

}

void U3D_GUI::saveParameters()
{
	vcg::Point3f from_target_to_camera = vcg::Point3f(
		ui.tarx_lab->text().toFloat() - ui.posx_lnedt->text().toFloat(),
		ui.tarz_lab->text().toFloat()  - ui.posz_lnedt->text().toFloat(),
		ui.tary_lab->text().toFloat()  - ui.posy_lnedt->text().toFloat());
	if (_param._campar != NULL) 
		delete _param._campar;
	else
		_param._campar = new vcg::tri::io::u3dparametersclasses::Movie15Parameters::CameraParameters(
		(float) ui.fov_spn->value(),0.0f,from_target_to_camera,from_target_to_camera.Norm());
}

void U3D_GUI::on_buttonBox_accepted()
{
	saveParameters();
	close();
}

void U3D_GUI::on_buttonBox_rejected()
{
	close();
}