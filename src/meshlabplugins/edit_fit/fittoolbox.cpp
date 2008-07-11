#include "fittoolbox.h"

FitToolBox::FitToolBox(QWidget *parent):QDockWidget(parent){
	ui.setupUi(this);
	//button:
	QObject::connect(ui.buttonCylinder,					SIGNAL(clicked())			,this,SLOT(change_buttonCylinder()));
	QObject::connect(ui.buttonBox,							SIGNAL(clicked())			,this,SLOT(change_buttonBox()));
	//checkBox:
	QObject::connect(ui.checkBoxGesture3d,			SIGNAL(toggled(bool))	,this,SLOT(change_checkBoxGesture3d(bool)));
	QObject::connect(ui.checkBoxLine,						SIGNAL(toggled(bool))	,this,SLOT(change_checkBoxLines(bool)));
	QObject::connect(ui.checkBoxCloudNearPoints,SIGNAL(toggled(bool))	,this,SLOT(change_checkBoxCloudNearPoints(bool)));
	QObject::connect(ui.checkBoxSelectionMesh,	SIGNAL(toggled(bool))	,this,SLOT(change_checkBoxSelectionMesh(bool)));
	QObject::connect(ui.checkBoxCloudSampling,	SIGNAL(toggled(bool))	,this,SLOT(change_checkBoxCloudSampling(bool)));
	QObject::connect(ui.checkBoxStepMode,				SIGNAL(toggled(bool))	,this,SLOT(change_checkBoxStepMode(bool)));
	QObject::connect(ui.checkBoxObjectSpace,		SIGNAL(toggled(bool))	,this,SLOT(change_checkBoxObjectSpace(bool)));
	//spinBox
	QObject::connect(ui.spinBoxStepMode,				SIGNAL(valueChanged(int)),this,SLOT(change_spinBoxStepMode(int)));
}

//button:
void FitToolBox::change_buttonCylinder(){
	emit click_on_buttonCylinder();
	ui.buttonCylinder->setChecked(true);
	ui.buttonBox->setChecked(false);
}
void FitToolBox::change_buttonBox(){
	emit click_on_buttonBox();
	ui.buttonCylinder->setChecked(false);
	ui.buttonBox->setChecked(true);	
}
//checkBox:
void FitToolBox::change_checkBoxGesture3d(bool b){
	emit click_on_checkBoxGesture3d(b);
}
void FitToolBox::change_checkBoxLines(bool b){
	emit click_on_checkBoxLines(b);
}
void FitToolBox::change_checkBoxCloudNearPoints(bool b){
	emit click_on_checkBoxCloudNearPoints(b);
}
void FitToolBox::change_checkBoxSelectionMesh(bool b){
	emit click_on_checkBoxSelectionMesh(b);
}
void FitToolBox::change_checkBoxCloudSampling(bool b){
	emit click_on_checkBoxCloudSampling(b);
}
void FitToolBox::change_checkBoxObjectSpace(bool b){
	emit click_on_checkBoxObjectSpace(b);
}
void FitToolBox::change_checkBoxStepMode(bool b){
	emit click_on_checkBoxStepMode(b);
}
//spinBox
void FitToolBox::change_spinBoxStepMode(int a){
	emit click_on_spinBoxStepMode(a);
}
void FitToolBox::slot_InitspinBoxStepMode(int a){
	ui.spinBoxStepMode->setMaximum(a);
}
FitToolBox::~FitToolBox(){
}
