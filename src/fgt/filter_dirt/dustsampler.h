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
#ifndef _DUSTSAMPLER_H_
#define _DUSTSAMPLER_H_

#include <vcg/math/random_generator.h>
#include <vcg/complex/trimesh/closest.h>
#include <vcg/space/index/spatial_hashing.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/space/box2.h>
#include "dustparticle.h"

template <class MeshType>
        class DustSampler
{


public:
    typedef typename MeshType::CoordType    CoordType;
    typedef typename MeshType::VertexType   VertexType;
    typedef typename MeshType::FaceType     FaceType;

    DustSampler(){
        sampleVec= new std::vector<CoordType>;
        dpVec=new std::vector<DustParticle <MeshType> >;
        vectorOwner=true;
    };
    DustSampler(std::vector<CoordType> &pointVec,std::vector<DustParticle <MeshType> > &infoVec){
        sampleVec = &pointVec;
        dpVec = &infoVec;
        dpVec->clear();
        sampleVec->clear();
        vectorOwner=false;
    };
    ~DustSampler(){
        if(vectorOwner)delete sampleVec;
    };

    void AddVert(const VertexType &p){
        sampleVec->push_back(p.cP());
    };

    void AddFace(FaceType &f, CoordType p){
        sampleVec->push_back(f.P(0)*p[0] + f.P(1)*p[1] +f.P(2)*p[2] );
        DustParticle<MeshType> part;
        part.setFace(f);
        part.setBarCoord(p);
        dpVec->push_back(part);

    };
    //void AddTextureSample(const FaceType &, const CoordType &, const Point2i &){};
private:
    std::vector<CoordType> *sampleVec;
    std::vector< DustParticle<MeshType> > *dpVec;
    bool vectorOwner;
};

#endif
