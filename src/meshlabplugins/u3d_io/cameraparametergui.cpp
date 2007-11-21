#include "cameraparametergui.h"

Cameraparametergui::Cameraparametergui(QWidget *parent)
	: QFrame(parent)
{
	ui.setupUi(this);
}

Cameraparametergui::~Cameraparametergui()
{

}



void Cameraparametergui::on_fov_edt_textChanged(QString & input)
{
	//if (!ui.fov_edt.validator().validate(input,input.length())) ui.fov_edt.validator().fixup(input);
}