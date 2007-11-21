#ifndef CAMERAPARAMETERGUI_H
#define CAMERAPARAMETERGUI_H

#include <QFrame>
#include "ui_cameraparametergui.h"

class Cameraparametergui : public QFrame
{
	Q_OBJECT

public:
	Cameraparametergui(QWidget *parent = 0);
	~Cameraparametergui();

private:
	Ui::CameraparameterguiClass ui;

private slots:
	void on_fov_edt_textChanged(QString & input);
};

#endif // CAMERAPARAMETERGUI_H
