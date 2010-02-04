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

#ifndef EXTRADECORATEPLUGIN_H
#define EXTRADECORATEPLUGIN_H

#include <common/interfaces.h>
#include <wrap/gui/coordinateframe.h>

class ExtraMeshDecoratePlugin : public QObject, public MeshDecorateInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshDecorateInterface)
  virtual QString filterInfo(QAction *) const;
  
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
        DP_SHOW_TEXPARAM,
  };

  QString filterName(FilterIDType filter) const;

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
        DP_SHOW_CAMERA <<
        DP_SHOW_TEXPARAM;

    FilterIDType tt;
    foreach(tt , types()){
          actionList << new QAction(filterName(tt), this);
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
    void DrawTexParam(MeshModel &m,QGLWidget *gla, QFont qf);

	QHash<MeshModel *, bool> isMeshOk;
	
	virtual void Decorate(QAction *a, MeshModel &m,  GLArea *gla,QFont qf);
	virtual bool StartDecorate(QAction * /*mode*/, MeshModel &/*m*/, RichParameterSet *, GLArea * /*parent*/);

    void initGlobalParameterSet(QAction *, RichParameterSet &/*globalparam*/);
    inline QString TextureStyleParam() const { return  "MeshLab::Decoration::TextureStyle" ; }
    bool textureWireParam;
};

#endif
