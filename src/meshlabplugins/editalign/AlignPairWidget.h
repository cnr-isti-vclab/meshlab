/*
 *  editAlignPair.h
 *  editalign_debug
 *
 *  Created by Paolo Cignoni on 11/6/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __VCG_ALIGNPAIRWIDGET
#define __VCG_ALIGNPAIRWIDGET

#include <GL/glew.h>
#include <QObject>
#include <QDialog>
#include <QGLWidget>
#include <wrap/gui/trackball.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
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
	std::vector<Point3f> freePickedPointVec;
	std::vector<Point3f> gluedPickedPointVec;
		
  /// mesh data structure initializer
		bool hasToPick;
		Point2i pointToPick;
public:
			void initMesh(MeshNode *left, MeshTree *right);
};

#endif
