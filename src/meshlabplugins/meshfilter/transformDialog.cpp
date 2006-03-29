
/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

//History

/*
$Log$
Revision 1.3  2006/03/29 07:30:54  zifnab1974
use fabs instead of abs for floats, needed for gcc 3.4.5

Revision 1.2  2006/01/31 18:24:44  mariolatronico
- dial updated only if rotate line edit has non negative numbers.
  This allows negative numbers on rotate line edit
- connections between line edits and on_okButton_pressed() to allow
  rapid edit and return actions

Revision 1.1  2006/01/30 20:43:57  giec
Added filter dialog files

Revision 1.1  2006/01/27 13:28:22  mariolatronico
moved meshfilter dialogs implementation in src/meshlab

Revision 1.10  2006/01/26 18:08:28  mariolatronico
Code cleanup, added widget naming convention and GPL license header

Revision 1.9  2006/01/25 19:53:58  mariolatronico
dial start from 0 at north , 90 east and so on

Revision 1.8  2006/01/23 14:03:19  mariolatronico
Rotate X/Z axis had wrong axes, now rotate around x has z axis
and viceversa

Revision 1.7  2006/01/22 16:40:38  mariolatronico
- restored correct layout in .ui
- added rotate around origini / object center

*/

#include "transformDialog.h"
#include <vcg/complex/trimesh/update/bounding.h>
#include <QRegExp>
#include <QRegExpValidator>

TransformDialog::TransformDialog() : QDialog() {
	
	setupUi(this);
	
	
	whichTransformBG = new QButtonGroup(this);
	rotateBG = new QButtonGroup(this);
	log = QString(" ");
	// top checkBox button group
	whichTransformBG->addButton(isMoveRB);
	whichTransformBG->addButton(isRotateRB);
	whichTransformBG->addButton(isScaleRB);
	
	// rotate button group, X, Y, Z check boxes
	rotateBG->addButton(xAxisRB);
	rotateBG->addButton(yAxisRB);
	rotateBG->addButton(zAxisRB);
	
	// when use the dial the line edit needs to change too
	connect(rotateDial,SIGNAL(valueChanged(int)),
					this, SLOT(updateRotateLE(int))); 
	
	connect(whichTransformBG,SIGNAL(buttonClicked(QAbstractButton* )),
					this, SLOT(selectTransform(QAbstractButton* )));
	connect(rotateBG, SIGNAL(buttonClicked(QAbstractButton* )),
					this, SLOT(rotateAxisChange(QAbstractButton* )));

	connect(xMoveLE, SIGNAL(returnPressed()), this, SLOT(on_okButton_pressed()) );
	connect(yMoveLE, SIGNAL(returnPressed()), this, SLOT(on_okButton_pressed()) );
	connect(zMoveLE, SIGNAL(returnPressed()), this, SLOT(on_okButton_pressed()) );
	connect(xScaleLE, SIGNAL(returnPressed()), this, SLOT(on_okButton_pressed()) );
	connect(yScaleLE, SIGNAL(returnPressed()), this, SLOT(on_okButton_pressed()) );
	connect(zScaleLE, SIGNAL(returnPressed()), this, SLOT(on_okButton_pressed()) );
	connect(rotateLE, SIGNAL(returnPressed()), this, SLOT(on_okButton_pressed()) );


	// default to AXIS_X for rotation and Move transformation
	rotateAxis = AXIS_X;
	matrix.SetIdentity();
	uniformScale = false; // default to non uniform scale
	setMove();
	setScale();
	setRotate();
	
	
}
TransformDialog::~TransformDialog() {
  delete whichTransformBG;
	delete rotateBG;
}

vcg::Matrix44f& TransformDialog::getTransformation() {
	return matrix;
}

void TransformDialog::setMesh(CMeshO *mesh) {
	
	this->mesh = mesh;
	this->bbox = mesh->bbox;
	vcg::tri::UpdateBounding<CMeshO>::Box(*mesh);

 	minBbox = mesh->bbox.min;
 	maxBbox = mesh->bbox.max;
}

QString& TransformDialog::getLog() {
	
	return log;

}
// select the group box, disable the others
void TransformDialog::selectTransform(QAbstractButton* button) {

	assert(button);
	if (button->text() == QString("Move") ) {
	  
		moveBox->setEnabled(true); 
		rotateBox->setEnabled(false);// resetRotate();
		scaleBox->setEnabled(false); //resetScale();
		whichTransform = TR_MOVE;
		log = "Move: ";
	}
	
	if (button->text() == QString("Rotate") ) {
		
		moveBox->setEnabled(false); //resetMove();
		rotateBox->setEnabled(true); 
		scaleBox->setEnabled(false); //resetScale();
		whichTransform = TR_ROTATE;
		log = "Rotate: ";
		
	}
	
	if (button->text() == QString("Scale") ) {
		
		rotateBox->setEnabled(false); //resetRotate();
		moveBox->setEnabled(false); //resetMove();
		scaleBox->setEnabled(true); // Scale
		whichTransform = TR_SCALE;
		log = "Scale: ";
		
	}
		
}

// decorate exec from QDialog
int TransformDialog::exec() {
	
 	isMoveRB->setChecked(true);
	//	 set the min and max Label
 	QString bboxString = QString("X: %1     Y: %2     Z: %3")
 		.arg(minBbox[0])
 		.arg(minBbox[1])
 		.arg(minBbox[2]);
 	bboxValueMinLBL->setText(bboxString);
 	bboxString = QString("X: %1     Y: %2     Z: %3")
 		.arg(maxBbox[0])
 		.arg(maxBbox[1])
 		.arg(maxBbox[2]);
 	bboxValueMaxLBL->setText(bboxString);

	log = "";
	selectTransform(isMoveRB);
	return QDialog::exec();
}

void TransformDialog::on_uniformScaleCB_stateChanged(int state) {
	if (state == Qt::Checked)
		uniformScale = true;
	else // don't care tristate
			uniformScale = false;
	// if uniformScale is false enable Y and Z LineEdit
	yScaleLE->setEnabled( ! uniformScale );
	zScaleLE->setEnabled( ! uniformScale );
}
// select the rotation axis
void TransformDialog::rotateAxisChange(QAbstractButton* axis) {

	if (axis->text() == "X" )
		rotateAxis = AXIS_X;
	if (axis->text() == "Y" )
		rotateAxis = AXIS_Y;
	if (axis->text() == "Z" )
		rotateAxis = AXIS_Z;
	
}
// used with dial
void TransformDialog::updateRotateLE(int value) {
	QString strValue;
	strValue.setNum((value + 180)%360);
	rotateLE->setText(strValue);
}	

void TransformDialog::on_rotateLE_textChanged(const QString &text) {

	bool isNumber = false;
	// type coercion
	int value = text.toFloat(&isNumber);
	if ( isNumber && value > 0 ) {
		rotateDial->setValue((value + 180)%360);
	}

}
// move mesh center to origin
// simply updates the move line edit
void TransformDialog::on_mvCenterOriginPB_clicked() {
	
	Box3f bbox = mesh->bbox;
	Point3f center = bbox.Center();
	setMove(QString().setNum(-center[0]), QString().setNum(-center[1]), QString().setNum(-center[2]));
	
}

// scale to unit box
void TransformDialog::on_scaleUnitPB_clicked() {
	// get the bounding box longest edge
	float scale =1.0 / (float)( max(fabs(maxBbox[0] - minBbox[0]),
											max(fabs(maxBbox[1] - minBbox[1]), fabs(maxBbox[2] - minBbox[2]))));
	setScale(QString().setNum(scale), QString().setNum(scale), QString().setNum(scale));
}
// do the real calculations
void TransformDialog::on_okButton_pressed() {
	
	Matrix44f currentMatrix;
	currentMatrix.SetIdentity();
	float xVal, yVal, zVal;
	bool okX, okY, okZ; // line edit values can be strings
	okX = okY = okZ = false;
	if (whichTransform == TR_MOVE) {
		xVal = xMoveLE->text().toFloat(&okX);
		yVal = yMoveLE->text().toFloat(&okY);
		zVal = zMoveLE->text().toFloat(&okZ);

		if ( ! okX || !okY || !okZ) {
			log = "Invalid values entered";
			reject();
			return;
		}

		currentMatrix.SetTranslate(xVal, yVal, zVal);
		log += QString("X:   %1    Y:   %2    Z:   %3").arg(xVal).arg(yVal).arg(zVal);
		//		qDebug("xVal %f\t yVal %f\t zVal %f\t", xVal, yVal, zVal); 
	}
	if (whichTransform == TR_ROTATE) {
		vcg::Point3f axisPoint;
		if ( rotateAxis == AXIS_X ) {
			axisPoint[0] = 1.0;	axisPoint[1] = 0.0;	axisPoint[2] = 0.0; 
			log += "X";
		}
		if ( rotateAxis == AXIS_Y ) {
			axisPoint[0] = 0.0;	axisPoint[1] = 1.0;	axisPoint[2] = 0.0; 
			log += "Y";

		}
		if ( rotateAxis == AXIS_Z ) {
			axisPoint[0] = 0.0;	axisPoint[1] = 0.0;	axisPoint[2] = 1.0; 
			log += "Z";

		}
		bool isNumber = false;
		float rotateVal = rotateLE->text().toFloat(&isNumber);
		if ( ! isNumber ) {
			log += "Invalid values entered";
			reject();
			return;
		}
		log += QString(" %1 degrees").arg(rotateVal);
		// SetTranslate, SetScale and SetRotate set initially the identity
		if (centerRotateRB->isChecked()) // rotate around obj center
		{
			Matrix44f transMat = currentMatrix.SetTranslate(  bbox.Center() );
			// ANGLE MUST BE IN RADIANS !!!!
			Matrix44f rotMat = currentMatrix.SetRotate(rotateVal * PI / 180.0, axisPoint);
			Matrix44f trans2Mat = currentMatrix.SetTranslate( -  bbox.Center() );
			currentMatrix = transMat * rotMat * trans2Mat;	
		} else {
			
			// ANGLE MUST BE IN RADIANS !!!!
			currentMatrix.SetRotate(rotateVal * PI / 180.0, axisPoint);
		}
	}
	if (whichTransform == TR_SCALE) {
		
		xVal = xScaleLE->text().toFloat(&okX);
		
		if ( ! uniformScale) {
			yVal = yScaleLE->text().toFloat(&okY);
			zVal = zScaleLE->text().toFloat(&okZ);
		}
		else {
			yVal = zVal = xVal;
			okY = okZ = okX;
		}

		if ( ! okX || !okY || !okZ) {
			log = "Invalid values entered";
			reject();
			return;
		}
		currentMatrix.SetScale(xVal, yVal, zVal);
		log += QString("X:   %1    Y:   %2    Z:   %3").arg(xVal).arg(yVal).arg(zVal);

	}
	
	//matrix = matrix * currentMatrix;
	// store the current matrix as matrix because we left
	// the values in the line edit widgets
	matrix = currentMatrix; 
	accept();
	
}

// ------- Private Functions --------


void TransformDialog::setMove(QString x , QString y , QString z) {

	xMoveLE->setText(x);
	yMoveLE->setText(y);
	zMoveLE->setText(z);
	
}

void TransformDialog::setRotate(int value) {
	rotateDial->setValue(value);
	xAxisRB->setChecked(true);
	centerRotateRB->setChecked(true);
}
	
void TransformDialog::setScale(QString x , QString y , QString z ) {
	
	xScaleLE->setText(x);
	yScaleLE->setText(y);
	zScaleLE->setText(z);
	
}

// rotate 90 degrees in z axis
void TransformDialog::on_rotateXUpPB_clicked()
{
	rotateDial->setValue(90);
	zAxisRB->setChecked(true);
}

void TransformDialog::on_rotateZUpPB_clicked()
{
	rotateDial->setValue(90);
	xAxisRB->setChecked(true);
}
