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

#ifndef ML_DEFAULT_DECORATORS_H
#define ML_DEFAULT_DECORATORS_H

#include <common/ml_shared_data_context.h>
#include <common/ml_document/mesh_model.h>
#include <common/GLLogStream.h>

class MainWindow;

class MLDefaultMeshDecorators
{
public:
	MLDefaultMeshDecorators(MainWindow* mw);

    typedef std::pair<Point3m,vcg::Color4b> PointPC; // this type is used to have a simple coord+color pair to rapidly draw non manifold faces

    //the initMeshDecorationData is called when a mesh has been updated by a filter execution
    bool initMeshDecorationData(MeshModel & mesh,const MLRenderingData& dt);
    void decorateMesh(MeshModel & mesh,const MLRenderingData& dt, QPainter* painter, GLLogStream& log);
    bool cleanMeshDecorationData(MeshModel& mesh,const MLRenderingData& dt);

    //The updateMeshDecorationData is called when a decoration button is clicked on/off
    bool updateMeshDecorationData(MeshModel& mesh,const MLRenderingData& previousdata,const MLRenderingData& currentdata);

	static const char* selectionAttName() { return "SelectionBuffers"; }

	static const char* boundaryVertAttName() { return "BoundaryVertVector"; }
	static const char* boundaryEdgeAttName() { return "BoundaryEdgeVector"; }
	static const char* boundaryFaceAttName() { return "BoundaryFaceVector"; }

	static const char* nonManifEdgeAttName() { return "NonManifEdgeVector"; }
	static const char* nonManifEdgeFaceAttName() { return "NonManifEdgeFaceVector"; }

	static const char* nonManifVertAttName() { return "NonManifVertVector"; }
	static const char* nonManifVertFaceAttName() { return "NonManifVertFaceVector"; }

	static const char* boundaryTextVertAttName() { return "BoundaryTexVector"; }

private:
    void initBoundaryDecoratorData(MeshModel& mm,bool edgeboundary,bool faceboundary);
    void cleanBoundaryDecoratorData(MeshModel& mm,bool edgeboundary,bool faceboundary);

	void initSelectionDecoratorData(MeshModel& mm,bool vertsel,bool facesel);
	void cleanSelectionDecoratorData(MeshModel& mm, bool vertsel, bool facesel);

    void initNonManifEdgeDecoratorData(MeshModel& mm);
    void cleanNonManifEdgeDecoratorData(MeshModel& mm);

	void initNonManifVertDecoratorData(MeshModel& mm);
	void cleanNonManifVertDecoratorData(MeshModel& mm);

    void initBoundaryTextDecoratorData(MeshModel& mm);
    void cleanBoundaryTextDecoratorData(MeshModel& mm);



    static void drawLineVector(const Matrix44m& tr,std::vector<PointPC> &EV);
    static void drawTriVector(const Matrix44m& tr, std::vector<PointPC> &TV);
    static void drawDotVector(const Matrix44m& tr, std::vector<PointPC> &TV, float baseSize=4.0f);
    static void drawQuotedBox(MeshModel &m,QPainter *gla,QFont& qf);
    static void drawQuotedLine(const vcg::Point3d &a,const vcg::Point3d &b, float aVal, float bVal, float tickScalarDistance, QPainter *painter, QFont& qf,float angle = 0,bool rightAlign = false);
    static void chooseX(Box3m &box,double *mm,double *mp,GLint *vp,vcg::Point3d &x1,vcg::Point3d &x2);
    static void chooseY(Box3m &box,double *mm,double *mp,GLint *vp,vcg::Point3d &y1,vcg::Point3d &y2);
    static void chooseZ(Box3m &box,double *mm,double *mp,GLint *vp,vcg::Point3d &z1,vcg::Point3d &z2);

	MainWindow* _mw;
};

#endif
