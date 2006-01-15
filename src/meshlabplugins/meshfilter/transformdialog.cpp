#include "transformdialog.h"
#include <QRegExp>
#include <QRegExpValidator>

/*
$Log$
Revision 1.2  2006/01/15 18:16:54  mariolatronico
- changed line edit behavior, now values are remembered among Apply Filter invocations
- added check on line edit values, they must be number

Revision 1.1  2006/01/15 17:15:17  mariolatronico
separated interface (.h) from implementation for Apply Transform dialog

*/

TransformDialog::TransformDialog() : QDialog() {

    setupUi(this);
    whichTransformBG = new QButtonGroup(this);
    rotateBG = new QButtonGroup(this);
		
		// top checkBox button group
    whichTransformBG->addButton(isMoveRB);
    whichTransformBG->addButton(isRotateRB);
    whichTransformBG->addButton(isScaleRB);
		
		// rotate button group, X, Y, Z check boxes
    rotateBG->addButton(xAxisRB);
    rotateBG->addButton(yAxisRB);
    rotateBG->addButton(zAxisRB);

		//    rotateValidator = new QDoubleValidator(this);
		// rotateValidator->setRange(0.0, 359.999, 3); // from 0 to 359.999 with 3 decimals
    //rotateLE->setValidator(rotateValidator);

		// when use the dial the line edit needs to change too
		connect(rotateDial,SIGNAL(valueChanged(int)),
						this, SLOT(updateRotateLE(int))); 
		
    connect(whichTransformBG,SIGNAL(buttonClicked(QAbstractButton* )),
						this, SLOT(selectTransform(QAbstractButton* )));
    connect(rotateBG, SIGNAL(buttonClicked(QAbstractButton* )),
						this, SLOT(rotateAxisChange(QAbstractButton* )));
		
		// default to AXIS_X for rotation and Move transformation
		rotateAxis = AXIS_X;
		matrix.SetIdentity();
		uniformScale = false; // default to non uniform scale
    resetMove();
    resetScale();
    resetRotate();

}

TransformDialog::~TransformDialog() {
  delete whichTransformBG;
	delete rotateBG;
	delete rotateValidator;
}

vcg::Matrix44f& TransformDialog::getTransformation() {
	return matrix;
}

// select the group box, disable the others
void TransformDialog::selectTransform(QAbstractButton* button) {

    if (button->text() == QString("Move") ) {
	  
      moveBox->setEnabled(true); 
      rotateBox->setEnabled(false);// resetRotate();
      scaleBox->setEnabled(false); //resetScale();
      whichTransform = TR_MOVE;
    }
	
    if (button->text() == QString("Rotate") ) {

      moveBox->setEnabled(false); //resetMove();
      rotateBox->setEnabled(true); 
      scaleBox->setEnabled(false); //resetScale();
      whichTransform = TR_ROTATE;

    }

    if (button->text() == QString("Scale") ) {

      rotateBox->setEnabled(false); //resetRotate();
			moveBox->setEnabled(false); //resetMove();
      scaleBox->setEnabled(true); // Scale
      whichTransform = TR_SCALE;

    }
		
}

// decorate exec from QDialog
int TransformDialog::exec() {
	//		resetMove();
	//	resetRotate();
	//	resetScale();
		// default to Move transform
		isMoveRB->setChecked(true);
		selectTransform(isMoveRB);
		return QDialog::exec();
}

void TransformDialog::on_uniformScaleCB_stateChanged(int state) {
	if (state == Qt::Checked)
		uniformScale = true;
	else // don't care tristate
			uniformScale = false;
	// in uniformScale is false we must enable Y and Z LineEdit
	yScaleLE->setEnabled( ! uniformScale );
	zScaleLE->setEnabled( ! uniformScale );
}
// select the rotation axis
void TransformDialog::rotateAxisChange(QAbstractButton* axis) {

	if (axis->text() == QString("X") )
		rotateAxis = AXIS_X;
	if (axis->text() == QString("Y") )
		rotateAxis = AXIS_Y;
	if (axis->text() == QString("Z") )
		rotateAxis = AXIS_Z;
	
	
}
// used with dial
void TransformDialog::updateRotateLE(int value) {
	QString strValue;
	strValue.setNum(value);
	rotateLE->setText(strValue);
}	

void TransformDialog::on_okButton_pressed() {
	
	Matrix44f currentMatrix;
	currentMatrix.SetIdentity();
	float xVal, yVal, zVal;
	bool okX, okY, okZ; // line edit values can be strings
	if (whichTransform == TR_MOVE) {
		xVal = xMoveLE->text().toFloat(&okX);
		yVal = yMoveLE->text().toFloat(&okY);
		zVal = zMoveLE->text().toFloat(&okZ);

		if ( ! okX || !okY || !okZ) {
			qDebug("c'era una stringa");
			reject();
			return;
		}

		currentMatrix.SetTranslate(xVal, yVal, zVal);
		qDebug("xVal %f\t yVal %f\t zVal %f\t", xVal, yVal, zVal); 
	}
	if (whichTransform == TR_ROTATE) {
		vcg::Point3f axisPoint;
		if ( rotateAxis == AXIS_X ) {
			axisPoint[0] = 1.0;	axisPoint[1] = 0.0;	axisPoint[2] = 0.0; 
		}
		if ( rotateAxis == AXIS_Y ) {
			axisPoint[0] = 0.0;	axisPoint[1] = 1.0;	axisPoint[2] = 0.0; 
		}
		if ( rotateAxis == AXIS_Z ) {
			axisPoint[0] = 0.0;	axisPoint[1] = 0.0;	axisPoint[2] = 1.0; 
		}
		float rotateVal = rotateLE->text().toFloat();
		qDebug("Axis : %f\t  %f\t  %f\t", axisPoint[0], axisPoint[1], axisPoint[2]);
		qDebug("Value: %f", rotateVal * PI / 180.0);
		// ANGLE MUST BE IN RADIANS !!!!
		
		currentMatrix.SetRotate(rotateVal, axisPoint);
		
	}
	if (whichTransform == TR_SCALE) {
		
		xVal = xScaleLE->text().toFloat(&okX);
		
		if ( ! uniformScale) {
			yVal = yScaleLE->text().toFloat(&okY);
			zVal = zScaleLE->text().toFloat(&okZ);
		}
		else 
			yVal = zVal = xVal;
		if ( ! okX || !okY || !okZ) {
			qDebug("c'era una stringa");
			reject();
			return;
		}
		currentMatrix.SetScale(xVal, yVal, zVal);
		qDebug("xVal %f\t yVal %f\t zVal %f\t", xVal, yVal, zVal); 			
	}
	
	//matrix = matrix * currentMatrix;
	matrix = currentMatrix;
	accept();
	
}

// ------- Private Functions --------

void TransformDialog::resetMove() {

	xMoveLE->setText("0.0");
	yMoveLE->setText("0.0");
	zMoveLE->setText("0.0");
	
}

void TransformDialog::resetRotate() {
	rotateDial->setValue(0);
	xAxisRB->setChecked(true);
}
	
void TransformDialog::resetScale() {
	
	xScaleLE->setText("1.0");
	yScaleLE->setText("1.0");
	zScaleLE->setText("1.0");
	
}
