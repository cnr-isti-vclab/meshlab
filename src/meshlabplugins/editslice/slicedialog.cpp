#include "slicedialog.h"

dialogslice::dialogslice(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);
	numPlane=1;
	distance=1;
	muldistance=1;//order of
	defaultTrackball=false;
	restoreDefalut=false;
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setGeometry(p.x()+parent->width()-width(),p.y()+30,width(),height() );
	//this->setWindowFlags(Qt::Popup);
	this->setFloating(true);
    
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




void dialogslice::on_SliderPlane_valueChanged(int x)
{emit Update_glArea();
numPlane=x;
}

void dialogslice::on_spinBoxPlane_valueChanged(int x)
{emit Update_glArea();
  numPlane=x;
}

void dialogslice::on_spinBoxDistance_valueChanged(int x)
{emit Update_glArea();
 distance=x;
}

void dialogslice::on_SliderPlaneDistance_valueChanged(int x)
{emit Update_glArea();
distance=x;
}




void dialogslice::on_DefultButton_clicked()
{
 emit Update_glArea();
 restoreDefalut=true;
 numPlane=1;
 distance=1;
 this->ui.SliderPlaneDistance->setValue(1);
 this->ui.SliderPlane->setValue(1);

}

void dialogslice::on_diasbledistance_toggled(bool f)
{emit Update_glArea();

if (f){
		this->ui.SliderPlaneDistance->setEnabled(false);
		this->ui.spinBoxDistance->setEnabled(false);
		this->ui.distanceOrder->setEnabled(false);
		distanceDefault=true;
	
	}
	else {this->ui.SliderPlaneDistance->setEnabled(true);
	this->ui.spinBoxDistance->setEnabled(true);

	this->ui.distanceOrder->setEnabled(true);
	distanceDefault=false;
	}

}

void dialogslice::on_ExportButton_clicked()
{
emit exportMesh();
}




void dialogslice::on_decdec_clicked()
{
muldistance=0.01f;
emit Update_glArea();
}

void dialogslice::on_dec_clicked()
{
muldistance=0.1f;
emit Update_glArea();
}

void dialogslice::on_unit_clicked()
{
muldistance=1;
emit Update_glArea();
}


void dialogslice::on_tenmulti_clicked()
{
muldistance=10;
emit Update_glArea();
}
