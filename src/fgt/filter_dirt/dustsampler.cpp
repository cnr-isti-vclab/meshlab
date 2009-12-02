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

#include "dustsampler.h"

using namespace vcg;
using namespace tri;

template <class MeshType> DustSampler<MeshType>::DustSampler(){
    sampleVec= new std::vector<Coordtype>;
    vectorOwner=true;
}


template <class MeshType> DustSampler<MeshType>::DustSampler(std::vector<CoordType> &Vec){
    sampleVec = &Vec;
    sampleVec->clear();
    vectorOwner=false;
}

template <class MeshType> DustSampler<MeshType>::~DustSampler(){
    if(vectorOwner)delete sampleVec;
}

template <class MeshType> void DustSampler<MeshType>::AddVert(const VertexType &p){
    sampleVec->push_back(p.cP());
}

template <class MeshType> void DustSampler<MeshType>::AddFace(const FaceType &f, const CoordType &p){
    sampleVec->push_back(f.P(0)*p[0] + f.P(1)*p[1] +f.P(2)*p[2] );
}

/*void DustSampler::AddTextureSample(const FaceType &, const CoordType &, const Point2i &){

}*/
