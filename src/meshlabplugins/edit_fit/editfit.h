/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2008                                                \/)\/    *
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
/****************************************************************************
History
$Log: editfit.h,v $
****************************************************************************/

#ifndef EditFitPLUGIN_H
#define EditFitPLUGIN_H

#include <QObject>
#include <QList>
#include <QtGui>
#include <QMessageBox>
#include <QImageWriter>
#include <QByteArray>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/glarea.h>
#include <QGLFramebufferObject>

#include <vcg/space/point2.h>
#include <vcg/math/histogram.h>
#include <vcg/math/base.h>
#include <vcg/space/box3.h>
#include <wrap/gl/space.h>
#include <wrap/gui/view.h>

#include <bitset>
#include <iostream> 
#include <fstream> 
#include <limits>
#include <vcg/space/obox3.h>
#include <vcg/complex/trimesh/closest.h>
#include <vcg/space/index/grid_static_ptr.h>
//#include <../code/lib/non_linear_minimization/non_linear_minimization.h>
#include <vcg/math/point_matching.h>
#include <sstream>
#include "primitive.h"
#include "cylinder.h"
#include "box.h"
#include "fittoolbox.h"

class EditFitPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)

enum { CYLINDER, BOX };

public:
	EditFitPlugin();
	~EditFitPlugin();
	static const QString Info();

	void StartEdit(MeshModel &, GLArea *);
	void EndEdit(MeshModel &, GLArea *);
	void Decorate(MeshModel &, GLArea *);

	void mousePressEvent(QMouseEvent *, MeshModel &, GLArea *);
	void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea *);
	void mouseReleaseEvent(QMouseEvent *, MeshModel &, GLArea *);

public slots:
	void change_buttonCylinder();
	void change_buttonBox();
	void change_checkBoxGesture3d(bool);
	void slot_UpdateGlArea();
	
private:

	int  selectedPrimitive;
	bool pickMode;
	bool selectionMode;
	bool startGesture;
	bool showGesture3D;

	FitToolBox																				 *toolBox;								
	QFont																							 qFont;
	vcg::Point2i																			 lastPoint;								//L'ultimo punto 2d

	std::vector<vcg::Point2i>													 gesture2D;								//La selezione dei punti 2d
	std::vector<vcg::Point3f>													 gesture3D;								//La selezione dei punti 3d
	std::vector<Primitive *>													 listaPrimitive;


	void  DrawGesture3D		  ();
	void  DrawGesture2D		  (GLArea *);
	//void  ProcessPick			  (GLArea *);
	void  ProcessPick			  (const QImage &,GLArea *);
	void  DrawInPickingMode (GLArea *);
	void  LineRasterDDA			(float x1,float y1,float x2,float y2);
};

#endif
