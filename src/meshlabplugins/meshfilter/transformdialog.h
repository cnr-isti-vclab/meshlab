#ifndef TRANSFORM_DIALOG_H
#define TRANSFORM_DIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QDoubleValidator>
#include "ui_transform.h"
#include <vcg/math/matrix44.h>

using vcg::Matrix44f;

/*

$Log$
Revision 1.1  2006/01/15 13:55:11  mariolatronico
file for Apply Transform dialog (implementation and user interface)


*/

class TransformDialog : public QDialog, Ui::TransformDialog {
	
  Q_OBJECT

public slots:

  void selectTransform(QAbstractButton* button) {

    if (button->text() == QString("Move") ) {
	  
      moveBox->setEnabled(true); 
      rotateBox->setEnabled(false); resetRotate();
      scaleBox->setEnabled(false); resetScale();
      whichTransform = TR_MOVE;
    }
	
    if (button->text() == QString("Rotate") ) {

      moveBox->setEnabled(false); resetMove();
      rotateBox->setEnabled(true); 
      scaleBox->setEnabled(false); resetScale();
      whichTransform = TR_ROTATE;

    }

    if (button->text() == QString("Scale") ) {

      rotateBox->setEnabled(false); resetRotate();
			moveBox->setEnabled(false); resetMove();
      scaleBox->setEnabled(true); // Scale
      whichTransform = TR_SCALE;

    }
	
	
  }
	// decorator
	int exec() {
		
		resetMove();
		resetRotate();
		resetScale();
		// default to Move transform
		isMoveRB->setChecked(true);
		selectTransform(isMoveRB);
		return QDialog::exec();
	}

	void on_uniformScaleCB_stateChanged(int state) {
		if (state == Qt::Checked)
			uniformScale = true;
		else
			uniformScale = false;
		// in uniformScale is false we must enable Y and Z LineEdit
		yScaleLE->setEnabled( ! uniformScale );
		zScaleLE->setEnabled( ! uniformScale );
	}

  void rotateAxisChange(QAbstractButton* axis) {

    if (axis->text() == QString("X") )
      rotateAxis = AXIS_X;
    if (axis->text() == QString("Y") )
      rotateAxis = AXIS_Y;
    if (axis->text() == QString("Z") )
      rotateAxis = AXIS_Z;

		
  }
	void updateRotateLE(int value) {
		QString strValue;
		strValue.setNum(value);
		rotateLE->setText(strValue);
	}	

  void on_okButton_pressed() {

		Matrix44f currentMatrix;
		currentMatrix.SetIdentity();
		float xVal, yVal, zVal;

    if (whichTransform == TR_MOVE) {
			xVal = xMoveLE->text().toFloat();
			yVal = yMoveLE->text().toFloat();
			zVal = zMoveLE->text().toFloat();
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
			qDebug("Value: %f", rotateVal);
      currentMatrix.SetRotate(rotateVal, axisPoint);
			
    }
    if (whichTransform == TR_SCALE) {
			
			xVal = xScaleLE->text().toFloat();

			if ( ! uniformScale) {
				yVal = yScaleLE->text().toFloat();
				zVal = zScaleLE->text().toFloat();
			}
			else 
				yVal = zVal = xVal;

			currentMatrix.SetScale(xVal, yVal, zVal);
			qDebug("xVal %f\t yVal %f\t zVal %f\t", xVal, yVal, zVal); 			
    }

		matrix = matrix * currentMatrix;

    accept();

  }

	
	
public:


  TransformDialog() : QDialog() 
  {
    setupUi(this);
    whichTransformBG = new QButtonGroup(this);
    rotateBG = new QButtonGroup(this);
		
		
    whichTransformBG->addButton(isMoveRB);
    whichTransformBG->addButton(isRotateRB);
    whichTransformBG->addButton(isScaleRB);

    rotateBG->addButton(xAxisRB);
    rotateBG->addButton(yAxisRB);
    rotateBG->addButton(zAxisRB);

    rotateLE->setInputMask("000.000");
		
    rotateValidator = new QDoubleValidator(this);
    rotateValidator->setRange(0.0, 359.999, 3); // from 0 to 359.999 with 3 decimals
    rotateLE->setValidator(rotateValidator);


		connect(rotateDial,SIGNAL(valueChanged(int)),
						this, SLOT(updateRotateLE(int))); 

    connect(whichTransformBG,SIGNAL(buttonClicked(QAbstractButton* )),
						this, SLOT(selectTransform(QAbstractButton* )));
    connect(rotateBG, SIGNAL(buttonClicked(QAbstractButton* )),
						this, SLOT(rotateAxisChange(QAbstractButton* )));
		
    connect(cancelButton, SIGNAL(clicked()), 
						this, SLOT(reject()));

		
		rotateAxis = AXIS_X;
		matrix.SetIdentity();
		uniformScale = false;
    resetMove();
    resetScale();
    resetRotate();
  }

  ~TransformDialog() {
    delete whichTransformBG;
		delete rotateBG;
		delete rotateValidator;
  }

   Matrix44f &getTransformation() {
     return matrix;
   }





private: // members
  
  QButtonGroup *whichTransformBG;
  QButtonGroup *rotateBG;
  QDoubleValidator *rotateValidator;
	enum AxisType {AXIS_X,AXIS_Y,AXIS_Z}  rotateAxis;
  enum TransformType { TR_MOVE, TR_ROTATE, TR_SCALE } whichTransform; 
  Matrix44f matrix;
	bool uniformScale; // same scale for X,Y,Z
private: // functions
	
  void resetMove() {

    xMoveLE->setText("0.0");
    yMoveLE->setText("0.0");
    zMoveLE->setText("0.0");
		
  }
	
  void resetRotate() {
		rotateDial->setValue(0);
    xAxisRB->setChecked(true);
  }
	
  void resetScale() {

    xScaleLE->setText("1.0");
    yScaleLE->setText("1.0");
    zScaleLE->setText("1.0");

  }

	


};

#endif // TRANSFORM_DIALOG_H
