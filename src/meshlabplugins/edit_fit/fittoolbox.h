#ifndef FITTOOLBOX_H
#define FITTOOLBOX_H

//#include <QWidget>
#include <QMessageBox>
#include <QDockWidget>
#include "ui_fittoolbox.h"

class FitToolBox : public QDockWidget{
	Q_OBJECT

public:
	FitToolBox(QWidget *parent = 0);
	~FitToolBox();


public slots:
	//button:
	void change_buttonCylinder();
	void change_buttonBox();
	//checkBox:
	void change_checkBoxGesture3d(bool);
	void change_checkBoxLines(bool);
	void change_checkBoxCloudNearPoints(bool);
	void change_checkBoxSelectionMesh(bool);
	void change_checkBoxCloudSampling(bool);
	void change_checkBoxObjectSpace(bool);
	void change_checkBoxStepMode(bool);
	//spinBox
	void change_spinBoxStepMode(int);
	void slot_InitspinBoxStepMode(int);


signals:
	//button:
  void click_on_buttonCylinder();
	void click_on_buttonBox();
	//checkBox:
	void click_on_checkBoxGesture3d(bool);
	void click_on_checkBoxLines(bool);
	void click_on_checkBoxCloudNearPoints(bool);
	void click_on_checkBoxSelectionMesh(bool);
	void click_on_checkBoxCloudSampling(bool);
	void click_on_checkBoxObjectSpace(bool);
	void click_on_checkBoxStepMode(bool);
	//spinBox
	void click_on_spinBoxStepMode(int);

private:
	Ui::FitToolBoxClass ui;
};

#endif // FITTOOLBOX_H
