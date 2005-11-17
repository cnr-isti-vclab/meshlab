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
/****************************************************************************
  History

$Log$
Revision 1.2  2005/11/17 14:54:27  glvertex
Some little changes to allow differents rendering modes (not working yet)

Revision 1.1  2005/10/18 10:38:02  cignoni
First rough version. It simply load a mesh.

****************************************************************************/
#ifndef GLAREA_H
#define GLAREA_H
#include <wrap/gl/trimesh.h>
#include <wrap/gui/trackball.h>

#include <QGLWidget>

#include <QColor>
#include <QImage>
#include <QPainterPath>
#include <QWidget>
#include <QGLWidget>



class BrushInterface;
class MeshModel;
class GLArea : public QGLWidget
{
    Q_OBJECT

public:
     GLArea(QWidget *parent = 0);
    ~GLArea(){}
     MeshModel *mm;
     vcg::Trackball trackball;
     QSize minimumSizeHint() const;
     QSize sizeHint() const;
    
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent*e);
  
  

private:
	vcg::GLW::DrawMode	renderMode;
	vcg::GLW::ColorMode	renderColor;
};

#endif
