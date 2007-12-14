#include "u3d_gui.h"
#include <cmath>

float avoidExponentialNotation(const float n,const float bboxdiag)
{
	float val_min = std::min(1000.0f,bboxdiag * 1000.0f);
	return val_min * floor(n) / val_min;
}

vcg::Point3f avoidExponentialNotation(const vcg::Point3f& p,const float bboxdiag)
{
	return vcg::Point3f(avoidExponentialNotation(p.X(),bboxdiag),
											avoidExponentialNotation(p.Y(),bboxdiag),
											avoidExponentialNotation(p.Z(),bboxdiag));
}

U3D_GUI::U3D_GUI(vcg::tri::io::u3dparametersclasses::Movie15Parameters& param,QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags),_param(param)
{
	ui.setupUi(this);
	defaultParameters();
	saveParameters();
}

U3D_GUI::~U3D_GUI()
{

} 

void U3D_GUI::saveParameters()
{
	vcg::Point3f from_target_to_camera = vcg::Point3f(
		ui.tarx_lab->text().toFloat() - ui.posx_lnedt->text().toFloat(),
		ui.tary_lab->text().toFloat()  - ui.posy_lnedt->text().toFloat(),
		ui.tarz_lab->text().toFloat()  - ui.posz_lnedt->text().toFloat());
	vcg::tri::io::u3dparametersclasses::Movie15Parameters::CameraParameters* sw = _param._campar;
	vcg::Point3f p = avoidExponentialNotation(sw->_obj_pos,_param._campar->_obj_bbox_diag);
	_param._campar = new vcg::tri::io::u3dparametersclasses::Movie15Parameters::CameraParameters(
		(float) ui.fov_spn->value(),0.0f,from_target_to_camera,from_target_to_camera.Norm(),sw->_obj_bbox_diag,p);
	delete sw;
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

void U3D_GUI::defaultParameters()
{
	QString tmp;
	vcg::Point3f pos = avoidExponentialNotation(_param._campar->_obj_pos,_param._campar->_obj_bbox_diag);

	tmp.setNum(pos.X());
	ui.tarx_lnedt->setText(tmp);
	tmp.setNum(pos.Y());
	ui.tary_lnedt->setText( tmp);
	tmp.setNum(pos.Z());
	ui.tarz_lnedt->setText(tmp);
	tmp.setNum(0.0);
	ui.posx_lnedt->setText(tmp);
	ui.posy_lnedt->setText(tmp);
	tmp.setNum(avoidExponentialNotation(-1.0f * _param._campar->_obj_bbox_diag,_param._campar->_obj_bbox_diag));
	ui.posz_lnedt->setText(tmp);
}