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

#include <common/gl_defs.h>
#include <QObject>
#include <QDialog>
#include <QGLWidget>
#include <vcg/space/point2.h>
#include <wrap/gui/trackball.h>

#include <common/ml_shared_data_context.h>

class MeshTree;
class MeshNode;

class GLArea;

class AlignPairWidget: public QGLWidget
{
    Q_OBJECT
public:
    AlignPairWidget (GLArea* ar, QWidget * parent);
public slots:
        /// widget-based user interaction slots
	void cleanDataOnClosing(int);
signals:
        /// signal for setting the statusbar message
        void setStatusBar(QString message);
protected:
    /// opengl initialization and drawing calls
    void initializeGL ();
    void paintEvent(QPaintEvent *);
//    void paintGL ();
    /// keyboard and mouse event callbacks
    void keyReleaseEvent(QKeyEvent * e);
    void keyPressEvent(QKeyEvent * e);
    void mousePressEvent(QMouseEvent*e);
    void mouseMoveEvent(QMouseEvent*e);
    void mouseReleaseEvent(QMouseEvent*e);
    void wheelEvent(QWheelEvent*e);
    void mouseDoubleClickEvent(QMouseEvent * e);
    void drawPickedPoints(QPainter *qp, std::vector<vcg::Point3f> &, vcg::Color4b color);


private:
    /// the active mesh instance
    MeshNode* freeMesh;
    MeshTree* gluedTree;

    /// the active manipulator
    vcg::Trackball trackLeft,trackRight;
    vcg::Trackball* tt[2];
    vcg::Trackball* currentTrack;
	MLSceneGLSharedDataContext* shared;
	GLArea* gla;


	void createRenderingData(MeshModel* mm, MLRenderingData& dt);
public:
    std::vector<vcg::Point3f> freePickedPointVec;
    std::vector<vcg::Point3f> gluedPickedPointVec;

    /// mesh data structure initializer
    bool hasToPick;
    bool hasToDelete;
    vcg::Point2i pointToPick;

    // use mesh vertex colors
    bool isUsingVertexColor;

	bool allowscaling;

    // force point rendering
    bool usePointRendering;
public:
    void initMesh(MeshNode *left, MeshTree *right);
};

#endif
