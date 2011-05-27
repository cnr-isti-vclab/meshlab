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
#ifndef PARTICLE_H
#define PARTICLE_H

#include<vector>
#include <common/meshmodel.h>
#include <common/interfaces.h>
#include<vcg/simplex/vertex/base.h>
#include<vcg/simplex/face/base.h>
#include<vcg/complex/complex.h>

template <class MeshType>

class Particle{
    typedef typename MeshType::CoordType    CoordType;
    typedef typename MeshType::VertexType   VertexType;
    typedef typename MeshType::FaceType     FaceType;
    typedef typename MeshType::FacePointer  FacePointer;

public:
    Particle(){
        mass=1.0f;
        velocity=0;
        face=0;
    }

    Particle(float m,float v){
        mass=m;
        velocity=v;
    }

    ~Particle(){

    }

public:
    FacePointer face;
    CoordType bar_coord;/*To delete?*/
    float mass;
    float velocity;
    float ad_coeff;/*Adhesion Coefficient*/
    CoordType v;
};





#endif // PARTICLE_H
