/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#ifndef __VCG_ALIGNPAIRWIDGET
#define __VCG_ALIGNPAIRWIDGET

#include <GL/glew.h>
#include <QObject>
#include <QDialog>
#include <QGLWidget>
#include <vcg/space/point2.h>
#include <wrap/gui/trackball.h>
class MeshTree;
class MeshNode;

class AlignPairWidget: public QGLWidget
{
	Q_OBJECT 
public:
  AlignPairWidget (QWidget * parent = 0);
	public slots:
		/// widget-based user interaction slots
signals:
		/// signal for setting the statusbar message
		void setStatusBar(QString message);
protected:
		/// opengl initialization and drawing calls
		void initializeGL ();
  void paintGL ();
  /// keyboard and mouse event callbacks
  void keyReleaseEvent(QKeyEvent * e);
  void keyPressEvent(QKeyEvent * e);
  void mousePressEvent(QMouseEvent*e);
  void mouseMoveEvent(QMouseEvent*e);
  void mouseReleaseEvent(QMouseEvent*e);
  void wheelEvent(QWheelEvent*e); 
	void mouseDoubleClickEvent(QMouseEvent * e);
	void drawPickedPoints(std::vector<vcg::Point3f> &, vcg::Color4b color);

private:
		/// the active mesh instance
		MeshNode* freeMesh;
		MeshTree* gluedTree;
		
	/// the active manipulator
  vcg::Trackball trackLeft,trackRight;
	vcg::Trackball* tt[2];
	vcg::Trackball* currentTrack;
public:	
	std::vector<vcg::Point3f> freePickedPointVec;
	std::vector<vcg::Point3f> gluedPickedPointVec;
		
  /// mesh data structure initializer
		bool hasToPick;
		vcg::Point2i pointToPick;
public:
			void initMesh(MeshNode *left, MeshTree *right);
};

#endif
