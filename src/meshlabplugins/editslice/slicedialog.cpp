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

void dialogslice::on_spinBoxDistance_valueChanged(int x)
{emit Update_glArea();
 distance=x;
}

void dialogslice::on_SliderPlaneDistance_valueChanged(int x)
{emit Update_glArea();
distance=x;
ui.lcdPlaneDistance->display(distance*distanceRange);
}




void dialogslice::on_DefultButton_clicked()
{
 emit Update_glArea();
 restoreDefalut=true;
 numPlane=1;
 distance=1;
 this->ui.SliderPlaneDistance->setValue(1);
 

}

void dialogslice::on_diasbledistance_toggled(bool f)
{emit Update_glArea();

if (f){
		this->ui.SliderPlaneDistance->setEnabled(false);
		
		
		distanceDefault=true;
	
	}
	else {this->ui.SliderPlaneDistance->setEnabled(true);
	

	
	distanceDefault=false;
	}

}

void dialogslice::on_ExportButton_clicked()
{
emit exportMesh();
}












