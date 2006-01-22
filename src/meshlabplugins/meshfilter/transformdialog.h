#ifndef TRANSFORM_DIALOG_H
#define TRANSFORM_DIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QDoubleValidator>
#include "ui_transform.h"
#include <vcg/math/matrix44.h>
#include <meshlab/meshmodel.h> // for CMeshO

using vcg::Matrix44f;
#define PI 3.14159265
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
	// ... and viceversa
	void on_rotateLE_textChanged(const QString &text);
	// do the real count when ok button is pressed
  void on_okButton_pressed();
	
	// move mesh center to origin
	// simply updates the move line edit
	void on_mvCenterOriginPB_clicked();

	// scale to unit box
	void on_scaleUnitPB_clicked();
	
	
public:

  TransformDialog();
	~TransformDialog();
	
	// used to compute transformation on meshfilter.cpp
	Matrix44f& getTransformation();
	void setMesh(CMeshO *mesh);
	QString& getLog();
private: // members
  CMeshO *mesh;
	Point3f minBbox, maxBbox; // min and max of bounding box
	Box3f bbox; // the mesh bounding box
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
	// without parameters are RESET functions
  void setMove(QString x = "0.0", QString y = "0.0", QString z = "0.0");
	void setRotate(int value = 0);
  void setScale(QString x = "1.0", QString y = "1.0", QString z = "1.0"); 

	



private slots:
	void on_rotateYUpPB_clicked();
	void on_rotateXUpPB_clicked();
};

#endif // TRANSFORM_DIALOG_H
