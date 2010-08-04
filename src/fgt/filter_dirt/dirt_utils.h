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
#ifndef DIRT_UTILS_H
#define DIRT_UTILS_H

//Include Files
#include <common/meshmodel.h>
#include <common/interfaces.h>
#include<vector>
#include<vcg/simplex/vertex/base.h>
#include<vcg/simplex/face/base.h>
#include<vcg/complex/trimesh/base.h>
#include <vcg/space/point3.h>


CMeshO::CoordType fromBarCoords(CMeshO::CoordType b,CMeshO::FaceType &face){
    CMeshO::CoordType c;
    CMeshO::CoordType p0=face.P(0);
    CMeshO::CoordType p1=face.P(1);
    CMeshO::CoordType p2=face.P(2);
    c[0]=p0[0]*b[0]+p1[0]*b[1]+p2[0]*b[2];
    c[1]=p0[1]*b[0]+p1[1]*b[1]+p2[1]*b[2];
    c[2]=p0[2]*b[0]+p1[2]*b[1]+p2[2]*b[2];
    return c;
};

/*
TO DELETE

CMeshO::CoordType toBarCoords(CMeshO::CoordType c,CMeshO::FaceType &face){

    CMeshO::CoordType b;
    CMeshO::CoordType p0=face.P(0);
    CMeshO::CoordType p1=face.P(1);
    CMeshO::CoordType p2=face.P(2);
    //x
    float A=p0[0]-p2[0];
    float B=p1[0]-p2[0];
    float C=p2[0]-c[0];
    //y
    float D=p0[1]-p2[1];
    float E=p1[1]-p2[1];
    float F=p2[1]-c[1];
    //z
    float G=p0[1]-p2[1];
    float H=p1[1]-p2[1];
    float I=p2[1]-c[1];

    b[0]=( B*(F+I)-C*(E+H))/(  A*(E+H)-B*(D+G) );
    b[1]=( A*(F+I)-C*(D+G))/( B*(D+G)- A*(E+H) );
    b[2]=1-b[0]-b[1];
    return b;
};

*/
/*
  @description Simulate the movement of a point, affected by a force "dir" on a face.

  @parameter CMeshO::CoordType p - coordinates of the point
  @parameter CMeshO::FaceType face - pointer to the face
  @parameter CmeshO;;CoordType dir
  @return new barycentric coordinates of the point
*/

CMeshO::CoordType StepForward(CMeshO::CoordType p, CMeshO::FaceType &face, CMeshO::CoordType dir){

    int angle = acos(p.dot(dir)/(p.Norm()*dir.Norm()));

    Point3<float> new_pos;

    if(angle!=0){
        new_pos=face.P(0);/*Just to do something*/
    }

    /*Conversion to barycentric coords*/
    Point3<float> bar_coords;
    InterpolationParameters(face,face.N(),new_pos,bar_coords);

    return bar_coords;

};

void DrawDirt(MeshDocument &md,std::vector<Point3f> &dp){

//TODO
};

/*
Reference: http://mathworld.wolfram.com/Line-LineIntersection.html
*/
CMeshO::CoordType ComputeIntersection(CMeshO::CoordType p1,CMeshO::CoordType p2,CMeshO::CoordType p3,CMeshO::CoordType p4){

    Point3<float> a=p2-p1;
    Point3<float> b=p4-p3;
    Point3<float> c=p3-p1;

    float s = ((c^b)*((a^b)))  /   pow(((a^b).Norm()),2);

    Point3<float> int_point=p1+a*s;

    return int_point;
};


#endif // DIRT_UTILS_H
