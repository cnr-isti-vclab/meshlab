#include "slicedialog.h"


#include<vcg/complex/intersection.h>

Slicedialog::Slicedialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	numPlane=1;
	distance=1;
	muldistance=1;//order of
	defaultTrackball=false;
	restoreDefalut=false;
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	
	this->setGeometry(p.x()+parent->width()-width(),p.y(),width(),height() );
	
	
		
}

Slicedialog::~Slicedialog()
{
  

  
}


void Slicedialog::on_DefaultTrackball_clicked(bool f)
{
defaultTrackball=f;

}

void Slicedialog::on_on_slideTrackBall_clicked(bool f)
{
defaultTrackball=!f;
}




void Slicedialog::on_SliderPlane_valueChanged(int x)
{emit Update_glArea();
numPlane=x;
}

void Slicedialog::on_spinBoxPlane_valueChanged(int x)
{emit Update_glArea();
  numPlane=x;
}

void Slicedialog::on_spinBoxDistance_valueChanged(int x)
{emit Update_glArea();
 distance=x;
}

void Slicedialog::on_SliderPlaneDistance_valueChanged(int x)
{emit Update_glArea();
distance=x;
}




void Slicedialog::on_DefultButton_clicked()
{emit Update_glArea();
 restoreDefalut=true;
 numPlane=1;
 distance=1;
 this->ui.SliderPlaneDistance->setValue(1);
 this->ui.SliderPlane->setValue(1);
}

void Slicedialog::on_radioButton_toggled(bool f)
{emit Update_glArea();

if (!f){
		this->ui.SliderPlaneDistance->setEnabled(false);
		this->ui.spinBoxDistance->setEnabled(false);
		distanceDefault=true;
	
	}
	else {this->ui.SliderPlaneDistance->setEnabled(true);
	this->ui.spinBoxDistance->setEnabled(true);
	distanceDefault=false;
	}

}

void Slicedialog::on_ExportButton_clicked()
{
emit exportMesh();
}




void Slicedialog::on_decdec_clicked()
{
muldistance=0.01f;
emit Update_glArea();
}

void Slicedialog::on_dec_clicked()
{
muldistance=0.1f;
emit Update_glArea();
}

void Slicedialog::on_unit_clicked()
{
muldistance=1;
emit Update_glArea();
}


void Slicedialog::on_tenmulti_clicked()
{
muldistance=10;
emit Update_glArea();
}