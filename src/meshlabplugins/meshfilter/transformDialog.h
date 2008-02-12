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

// History

/*

$Log$
Revision 1.4  2008/02/12 14:21:39  cignoni
changed the function getParameter into the more meaningful getCustomParameter and added the freeze option

Revision 1.3  2007/10/06 23:39:01  cignoni
Updated used defined dialog to the new filter interface.

Revision 1.2  2007/07/24 07:20:24  cignoni
Added Freeze transform and improved transformation dialog

Revision 1.1  2006/01/30 20:43:57  giec
Added filter dialog files

Revision 1.1  2006/01/27 13:28:22  mariolatronico
moved meshfilter dialogs implementation in src/meshlab

Revision 1.9  2006/01/26 18:08:28  mariolatronico
Code cleanup, added widget naming convention and GPL license header

Revision 1.8  2006/01/25 19:53:58  mariolatronico
dial start from 0 at north , 90 east and so on

Revision 1.7  2006/01/22 16:40:38  mariolatronico
- restored correct layout in .ui
- added rotate around origini / object center

Revision 1.6  2006/01/22 14:11:04  mariolatronico
added scale to unit box, move obj center. Rotate around object and origin are not working actually.

Revision 1.5  2006/01/21 14:20:38  mariolatronico
interface work in progress on new features , need implementation

Revision 1.4  2006/01/17 14:18:03  mariolatronico
- added connection between rotate Line edit and dial
- bugfix, angle were passed in degrees, must be in radians


*/


#ifndef TRANSFORM_DIALOG_H
#define TRANSFORM_DIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QDoubleValidator>
#include "ui_transformDialog.h"
#include <vcg/math/matrix44.h>
#include <meshlab/meshmodel.h> // for CMeshO
#include <meshlab/interfaces.h> // for CMeshO

using vcg::Matrix44f;
//#define PI 3.14159265

/*
 *	Naming conventions for widgets:
 *	LineEdit -> ends with LE (eg rotateLE )
 *	Label -> ends with LBL (eg rotateLBL )
 *	PushButton -> ends with PB (eg mvCenterOriginPB )
 *	CheckBox -> ends with CB (eg uniformScaleCB )
*/

class TransformDialog : public QDialog, Ui::TransformDialog {
	
  Q_OBJECT
public slots:
	// overloaded exec() function, sets some initial values
	// on start
	int exec(); 
	
private slots:
	// disable transformations group box differnt from the one
	// selected by top check box
  void selectTransform(QAbstractButton* button);

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
  void on_applyButton_clicked();
  void on_closeAndFreezeButton_clicked();
	
	// move mesh center to origin
	// simply updates the move line edit
	void on_mvCenterOriginPB_clicked();

	// scale to unit box
	void on_scaleUnitPB_clicked();
	
	// Rotate X / Z Up push buttons: simply fill Rotate fields
	void on_rotateZUpPB_clicked();
	void on_rotateXUpPB_clicked();
	//void freeze();
	
public:

  TransformDialog();
	~TransformDialog();
	
	// used to compute transformation on meshfilter.cpp
	Matrix44f& getTransformation();
	void setMesh(CMeshO *mesh);
	void updateMatrixWidget();
	void setMainWindow(MainWindowInterface *mw) {curmwi=mw;}
	void setAction(QAction *act) {curact=act;}
	QString& getLog();
private: // members
  CMeshO *mesh;
	Point3f minBbox, maxBbox; // min and max of bounding box
	Box3f bbox; // the mesh bounding box
  QButtonGroup *whichTransformBG;
  QButtonGroup *rotateBG;
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
	void setRotate(int value = 180); // 
  void setScale(QString x = "1.0", QString y = "1.0", QString z = "1.0"); 

	void resizeEvent ( QResizeEvent * event );
	void showEvent ( QShowEvent * event );

	MainWindowInterface *curmwi;
	QAction *curact;
};

#endif // TRANSFORM_DIALOG_H
