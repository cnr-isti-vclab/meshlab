/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software: you can redistribute it and/or modify      *
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
#ifndef DIRT_UTILS_H
#define DIRT_UTILS_H

//Include Files
#include <QDir>
#include <QPainter>
#include <QColor>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include <common/meshmodel.h>
#include <common/interfaces.h>
#include<vector>
#include<vcg/space/index/base.h>
#include<vcg/complex/complex.h>
#include <vcg/space/point3.h>
#include <vcg/space/intersection2.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/closest.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/complex/algorithms/geodesic.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/complex/algorithms/create/resampler.h>
#include <vcg/complex/algorithms/clustering.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/space/distance3.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/intersection3.h>
#include "particle.h"

using namespace vcg;
using namespace tri;

typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshFaceGrid;
typedef GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType > MetroMeshVertexGrid;
typedef FaceTmark<CMeshO> MarkerFace;

#define PI 3.14159265
#define EPSILON 0.0001




CMeshO::CoordType RandomBaricentric();
CMeshO::CoordType fromBarCoords(Point3m bc,CMeshO::FacePointer f);
CMeshO::CoordType getBaricenter(CMeshO::FacePointer f);
CMeshO::CoordType GetSafePosition(CMeshO::CoordType p,CMeshO::FacePointer f);
CMeshO::CoordType StepForward(CMeshO::CoordType p,CMeshO::CoordType v,MeshLabScalar m,CMeshO::FacePointer &face,CMeshO::CoordType force,MeshLabScalar l,MeshLabScalar t=1);
CMeshO::CoordType getRandomDirection();
CMeshO::CoordType getVelocityComponent(MeshLabScalar v,CMeshO::FacePointer f,CMeshO::CoordType g);
CMeshO::CoordType GetNewVelocity(CMeshO::CoordType i_v,CMeshO::FacePointer face,CMeshO::FacePointer new_face,CMeshO::CoordType force,CMeshO::CoordType g,MeshLabScalar m,MeshLabScalar t);

int ComputeIntersection(CMeshO::CoordType p1,CMeshO::CoordType p2,CMeshO::FacePointer &f,CMeshO::FacePointer &new_f,CMeshO::CoordType &int_point);
MeshLabScalar GetElapsedTime(CMeshO::CoordType p1,CMeshO::CoordType p2, CMeshO::CoordType p3, MeshLabScalar t,MeshLabScalar l);

bool CheckFallPosition(CMeshO::FacePointer f,Point3m g,MeshLabScalar a);
bool IsOnFace(Point3m p, CMeshO::FacePointer f);
bool GenerateParticles(MeshModel* m,std::vector<CMeshO::CoordType> &cpv,int d,MeshLabScalar threshold);


void ColorizeMesh(MeshModel* m);
void DrawDust(MeshModel *base_mesh,MeshModel *cloud_mesh);
void ComputeNormalDustAmount(MeshModel* m,CMeshO::CoordType u,MeshLabScalar k,MeshLabScalar s);
void ComputeSurfaceExposure(MeshModel* m,int r,int n_ray);
void ComputeParticlesFallsPosition(MeshModel* cloud_mesh,MeshModel* base_mesh,CMeshO::CoordType dir);
void associateParticles(MeshModel* b_m,MeshModel* c_m,MeshLabScalar &m,MeshLabScalar &v,CMeshO::CoordType g);
void prepareMesh(MeshModel* m);
void MoveParticle(Particle<CMeshO> &info,CMeshO::VertexPointer p,MeshLabScalar l,int t,Point3m dir,Point3m g,MeshLabScalar a);
void ComputeRepulsion(MeshModel* b_m,MeshModel *c_m,int k,MeshLabScalar l,Point3m g,MeshLabScalar a);
void MoveCloudMeshForward(MeshModel *cloud,MeshModel *base,Point3m g,Point3m force,MeshLabScalar l,MeshLabScalar a,MeshLabScalar t,int r_step);


#endif // DIRT_UTILS_H
