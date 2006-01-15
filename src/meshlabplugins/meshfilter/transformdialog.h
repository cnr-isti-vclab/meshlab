#ifndef TRANSFORM_DIALOG_H
#define TRANSFORM_DIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QDoubleValidator>
#include "ui_transform.h"
#include <vcg/math/matrix44.h>

using vcg::Matrix44f;
#define PI 3.14159265
/*

$Log$
Revision 1.3  2006/01/15 19:23:57  mariolatronico
added log for Apply Transform

Revision 1.2  2006/01/15 17:15:18  mariolatronico
separated interface (.h) from implementation for Apply Transform dialog

Revision 1.1  2006/01/15 13:55:11  mariolatronico
file for Apply Transform dialog (implementation and user interface)


*/

class TransformDialog : public QDialog, Ui::TransformDialog {
	
  Q_OBJECT

public slots:
	// disable transformations group box differnt from the one
	// selected by top check box
  void selectTransform(QAbstractButton* button);

	// overloaded exec() function, sets some initial values
	// on start
	int exec(); 
	// disable buttons when uniformScale check box is
	// selected
	void on_uniformScaleCB_stateChanged(int state);
	
	// set the rotateAxis value
  void rotateAxisChange(QAbstractButton* axis);
	
	// this is a slot to update the rotation LineEdit 
	// from the dial
	void updateRotateLE(int value);
	

	// do the real count when ok button is pressed
  void on_okButton_pressed();
	
	
public:

  TransformDialog();
	~TransformDialog();
	
	// used to compute transformation on meshfilter.cpp
	Matrix44f& getTransformation();
 
	QString& getLog();
private: // members
  
  QButtonGroup *whichTransformBG;
  QButtonGroup *rotateBG;
  QDoubleValidator *rotateValidator;
	enum AxisType {AXIS_X,AXIS_Y,AXIS_Z}  rotateAxis;
  enum TransformType { TR_MOVE, TR_ROTATE, TR_SCALE } whichTransform; 
  // store the transformation
	Matrix44f matrix;
	bool uniformScale; // true if want same scale for X,Y,Z
	QString log; // used to store string to log
private: // functions

	// reset initial values for Move, Rotate and Scale

  void resetMove();
	void resetRotate();
  void resetScale(); 

	


};

#endif // TRANSFORM_DIALOG_H
