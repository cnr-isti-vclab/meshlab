#include "slicedialog.h"
#include <qfiledialog.h>
#include<vcg/complex/intersection.h>

Slicedialog::Slicedialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	numPlane=1;
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
{
numPlane=x;
}

void Slicedialog::on_spinBoxPlane_valueChanged(int x)
{
  numPlane=x;
}

void Slicedialog::on_spinBoxDistance_valueChanged(int x)
{
 distance=x;
}

void Slicedialog::on_SliderPlaneDistance_valueChanged(int x)
{
distance=x;
}


void Slicedialog::on_ExportButton_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save polyline File"),
                                                "/",
                                                tr("Mesh (*.obj)"));
}

void Slicedialog::on_DefultButton_clicked()
{
 restoreDefalut=true;
}

void Slicedialog::on_radioButton_toggled(bool f)
{
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
