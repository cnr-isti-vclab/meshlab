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
/****************************************************************************
  History
$Log$
Revision 1.27  2008/04/18 17:42:19  cignoni
added showing of facenormals

Revision 1.26  2008/04/11 10:11:55  cignoni
added visualization of vertex and face label

Revision 1.25  2008/04/04 10:03:49  cignoni
Solved namespace ambiguities caused by the removal of a silly 'using namespace' in meshmodel.h

Revision 1.24  2008/03/02 16:55:26  benedetti
removed DrawAxis() in favor of VCG's CoordinateFrame class

Revision 1.23  2008/01/04 18:23:34  cignoni
Corrected a wrong type (glwidget instead of glarea) in the decoration callback.

Revision 1.22  2008/01/04 00:46:29  cignoni
Changed the decoration framework. Now it accept a, global, parameter set. Added static calls for finding important directories in a OS independent way.

Revision 1.21  2007/10/23 07:16:34  cignoni
added absolute box corner decoration

Revision 1.20  2007/10/02 08:13:50  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.19  2007/02/25 21:23:05  cignoni
Added casts for mac compiling

Revision 1.18  2006/11/29 00:59:18  cignoni
Cleaned plugins interface; changed useless help class into a plain string

Revision 1.17  2006/11/07 09:24:10  cignoni
Removed shorthHelp and reformatted the code

Revision 1.16  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.15  2006/02/22 12:24:41  cignoni
Restructured Quoted Box.

Revision 1.14  2006/02/19 22:17:17  glvertex
Applied gcc patch

Revision 1.13  2006/02/17 16:09:31  glvertex
Partial restyle in drawAxis and drawQuotedBox
A lot of optimizations

Revision 1.12  2006/02/15 16:27:33  glvertex
- Added labels to the quoted box
- Credits

Revision 1.11  2006/02/03 16:36:24  glvertex
- Some renaming
- Quoted axis now also draw lines

Revision 1.10  2006/02/03 11:05:14  alemochi
Modified axis and added arrows.

Revision 1.9  2006/01/26 00:38:59  glvertex
Quoted box: draws xyz axes candidates

Revision 1.8  2006/01/22 23:37:59  glvertex
Choosing axes candidates

Revision 1.7  2006/01/22 14:47:16  glvertex
Puts ticks on X axis... Still working on...

Revision 1.6  2006/01/19 23:56:44  glvertex
Starting quoted box (simply draws xyz axes)

****************************************************************************/
#ifndef EXTRADECORATEPLUGIN_H
#define EXTRADECORATEPLUGIN_H

#include <QObject>
#include <QAction>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <wrap/gui/coordinateframe.h>
//#include "../../meshlab/mainwindow.h"

class ExtraMeshDecoratePlugin : public QObject, public MeshDecorateInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshDecorateInterface)
  virtual const QString Info(QAction *);
  
  enum {
    DP_SHOW_FACE_NORMALS,
    DP_SHOW_VERT_NORMALS,
    DP_SHOW_VERT,
    DP_SHOW_EDGE,
    DP_SHOW_NON_FAUX_EDGE,
		DP_SHOW_VERT_PRINC_CURV_DIR,
    DP_SHOW_BOX_CORNERS,
    DP_SHOW_BOX_CORNERS_ABS,
    DP_SHOW_AXIS,
		DP_SHOW_QUOTED_BOX,
		DP_SHOW_VERT_LABEL, 
		DP_SHOW_FACE_LABEL,
		DP_SHOW_CAMERA,
  };

  virtual const QString ST(FilterIDType filter) const;

private:
	float niceRound2(float value,float base);
	float niceRound(float value);

	void	drawQuotedLine(const vcg::Point3d &a,const vcg::Point3d &b,float aVal, float bVal,float tickDist,QGLWidget *gla, QFont qf);

	void	chooseX(vcg::Box3f &box,double *modelview,double *projection,GLint *viewport,vcg::Point3d &x1,vcg::Point3d &x2);
	void	chooseY(vcg::Box3f &box,double *modelview,double *projection,GLint *viewport,vcg::Point3d &y1,vcg::Point3d &y2);
	void	chooseZ(vcg::Box3f &box,double *modelview,double *projection,GLint *viewport,vcg::Point3d &z1,vcg::Point3d &z2);

public:
     
	ExtraMeshDecoratePlugin()
	{
    typeList << 
    DP_SHOW_VERT <<
    DP_SHOW_NON_FAUX_EDGE <<
    DP_SHOW_FACE_NORMALS <<
    DP_SHOW_VERT_NORMALS <<
		DP_SHOW_VERT_PRINC_CURV_DIR <<
    DP_SHOW_BOX_CORNERS <<
    DP_SHOW_BOX_CORNERS_ABS <<
    DP_SHOW_AXIS <<
    DP_SHOW_QUOTED_BOX <<
		DP_SHOW_VERT_LABEL << 
		DP_SHOW_FACE_LABEL << 
		DP_SHOW_CAMERA;

    FilterIDType tt;
    foreach(tt , types()){
	      actionList << new QAction(ST(tt), this);
    }
    QAction *ap;
    foreach(ap,actionList){
        ap->setCheckable(true);
    }
  }

	QList<QAction *> actions () const {return actionList;}

	void DrawBBoxCorner(MeshModel &m, bool absBBoxFlag=true);
	void DrawQuotedBox(MeshModel &m,GLArea *gla, QFont qf);
	void DrawVertLabel(MeshModel &m, QGLWidget *gla, QFont qf);
	void DrawFaceLabel(MeshModel &m, QGLWidget *gla, QFont qf);
	void DrawCamera(MeshModel &m, QGLWidget *gla, QFont qf);
  
	QHash<MeshModel *, bool> isMeshOk;
	
  virtual void Decorate(QAction *a, MeshModel &m,  GLArea *gla,QFont qf);
	 virtual bool StartDecorate(QAction * /*mode*/, MeshModel &/*m*/, RichParameterSet *, GLArea * /*parent*/);
		
};

#endif
