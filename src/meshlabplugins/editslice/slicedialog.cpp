#include "slicedialog.h"

dialogslice::dialogslice(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);
	numPlane=1;
	distance=1;
	
	defaultTrackball=false;
	restoreDefalut=false;
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setGeometry(p.x()+parent->width()-width(),p.y()+30,width(),height() );
	this->setFloating(true);
	multi_sigleExportFile=false;
    
}

dialogslice::~dialogslice()
{

}
void dialogslice::on_DefaultTrackball_clicked(bool f)
{
defaultTrackball=f;

}

void dialogslice::on_on_slideTrackBall_clicked(bool f)
{
defaultTrackball=!f;
}


void dialogslice::on_spinBoxPlane_valueChanged(int x)
{emit Update_glArea();
  numPlane=x;
}
void dialogslice::on_DefultButton_clicked()
{
 emit Update_glArea();
 emit  RestoreDefault();
 
 numPlane=1;
 distance=1;
 this->ui.SliderPlaneDistance->setValue(1);
 this->ui.spinBoxPlane->setValue(1);
 

}

void dialogslice::on_diasbledistance_toggled(bool f)
{

if (f){
		this->ui.SliderPlaneDistance->setEnabled(false);
		ui.Distnace_edit_group->setEnabled(false);
		
		distanceDefault=true;
	
	}
else {
	this->ui.SliderPlaneDistance->setEnabled(true);
	ui.Distnace_edit_group->setEnabled(true);
	this->ui.SliderPlaneDistance->setValue(defaultdistance/distanceRange);
	this->distance=defaultdistance/distanceRange;
	distanceDefault=false;
	}
emit Update_glArea();
}

void dialogslice::on_ExportButton_clicked()
{
emit exportMesh();
}



void dialogslice::on_Update_Val_clicked()
{
bool ok;
float val=this->ui.DistanceEdit->text().toFloat(&ok);
 if (ok)
 {
	 this->ui.SliderPlaneDistance->setValue(val/distanceRange); 
	 distance=val/distanceRange;
	 QString c;
	
 }
 else
 this->ui.SliderPlaneDistance->setValue(1); 
 emit Update_glArea();
}

void dialogslice::on_SliderPlaneDistance_sliderMoved(int x)
{

distance=x;
QString c;
ui.DistanceEdit->setText(c.setNum(distance*distanceRange));
emit Update_glArea();
}






void dialogslice::on_Exporter_singleFile_clicked()
{//not implemented yet
//this->multi_sigleExportFile=true;
}

void dialogslice::on_Exporter_MultiFile_clicked()
{
this->multi_sigleExportFile=false;
}