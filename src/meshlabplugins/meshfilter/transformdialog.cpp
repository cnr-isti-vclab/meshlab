#include "transformdialog.h"
#include <vcg/complex/trimesh/update/bounding.h>
#include <QRegExp>
#include <QRegExpValidator>

/*
$Log$
Revision 1.6  2006/01/22 14:11:04  mariolatronico
added scale to unit box, move obj center. Rotate around object and origin are not working actually.

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
	setMove();
	setScale();
	setRotate();
	
	
}
TransformDialog::~TransformDialog() {
  delete whichTransformBG;
	delete rotateBG;
	delete rotateValidator;
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
 	QString bboxString = QString("X:%1    Y:%2    Z:%3")
 		.arg(minBbox[0])
 		.arg(minBbox[1])
 		.arg(minBbox[2]);
 	bboxValueMinLBL->setText(bboxString);
 	bboxString = QString("X:%1     Y:%2     Z:%3")
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
	
	Box3f bbox = mesh->bbox;
	Point3f center = bbox.Center();
	setMove(QString().setNum(-center[0]), QString().setNum(-center[1]), QString().setNum(-center[2]));
	
}

// scale to unit box
void TransformDialog::on_scaleUnitPB_clicked() {
	// get the bounding box longest edge
	float scale =1.0 / (float)( max(abs(maxBbox[0] - minBbox[0]),
											max(abs(maxBbox[1] - minBbox[1]), abs(maxBbox[2] - minBbox[2]))));
	setScale(QString().setNum(scale), QString().setNum(scale), QString().setNum(scale));
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
		// SetTranslate, SetScale and SetRotate set initalially the identity
		if (centerRotateRB->isChecked()) // rotate around obj center
		{
			Matrix44f transMat = currentMatrix.SetTranslate( - bbox.Center() );
			// ANGLE MUST BE IN RADIANS !!!!
			Matrix44f rotMat = currentMatrix.SetRotate(rotateVal * PI / 180.0, axisPoint);
			Matrix44f trans2Mat = currentMatrix.SetTranslate(  bbox.Center() );
			currentMatrix = transMat * rotMat * trans2Mat;	
		}
			
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
	xAxisRB->setChecked(true);
}

void TransformDialog::on_rotateYUpPB_clicked()
{
	rotateDial->setValue(90);
	zAxisRB->setChecked(true);
}