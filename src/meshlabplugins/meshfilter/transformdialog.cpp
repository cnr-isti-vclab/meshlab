#include "transformdialog.h"
#include <vcg/complex/trimesh/update/bounding.h>
#include <QRegExp>
#include <QRegExpValidator>

/*
$Log$
Revision 1.5  2006/01/21 14:20:38  mariolatronico
interface work in progress on new features , need implementation

Revision 1.4  2006/01/17 14:18:03  mariolatronico
- added connection between rotate Line edit and dial
- bugfix, angle were passed in degrees, must be in radians

Revision 1.3  2006/01/15 19:23:57  mariolatronico
added log for Apply Transform

Revision 1.2  2006/01/15 18:16:54  mariolatronico
- changed line edit behavior, now values are remembered among Apply Filter invocations
- added check on line edit values, they must be number

Revision 1.1  2006/01/15 17:15:17  mariolatronico
separated interface (.h) from implementation for Apply Transform dialog

*/
TransformDialog::TransformDialog(/*CMeshO *mesh*/) : QDialog() {
	
	setupUi(this);
	
	//	this->mesh = mesh;

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
int TransformDialog::exec(CMeshO *mesh) {
	//		resetMove();
	//	resetRotate();
	//	resetScale();
		// default to Move transform
	// get the bounding box
// 	vcg::tri::UpdateBounding<CMeshO>::Box(*mesh);
// 	minBbox = mesh->bbox.min;
// 	maxBbox = mesh->bbox.max;
// 	isMoveRB->setChecked(true);
	// set the min and max Label
// 	QString bboxString = QString("X:%1 Y:%2 Y:%3")
// 		.arg(minBbox[0])
// 		.arg(minBbox[1])
// 		.arg(minBbox[2]);
// 	bboxValueMinLBL->setText(bboxString);
// 	bboxString = QString("X:%1 Y:%2 Y:%3")
// 		.arg(minBbox[0])
// 		.arg(minBbox[1])
// 		.arg(minBbox[2]);
// 	bboxValueMaxLBL->setText(bboxString);


	log = "";
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
	strValue.setNum(value);
	rotateLE->setText(strValue);
}	

void TransformDialog::on_rotateLE_textChanged(const QString &text) {
	// type coercion
	bool isNumber = false;
	int value = text.toFloat(&isNumber);
	if (isNumber) 
		rotateDial->setValue(value);

}
// move mesh center to origin
// simply updates the move line edit
void TransformDialog::on_mvCenterOriginPB_clicked() {
	
}

	// scale to unit box
void TransformDialog::on_scaleUnitPB_clicked() {

}
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
		// ANGLE MUST BE IN RADIANS !!!!
		currentMatrix.SetRotate(rotateVal * PI / 180.0, axisPoint);
		
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
