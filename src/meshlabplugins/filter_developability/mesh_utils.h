#ifndef FILTERDEVELOPABILITY_MESHUTILS_H
#define FILTERDEVELOPABILITY_MESHUTILS_H

#include <vector>
#include <vcg/space/point3.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/simplex/face/topology.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/normal.h>


template<typename MeshType>
using Star = std::vector<typename MeshType::FacePointer>;

template<typename MeshType>
using StarVertAttrHandle = typename MeshType:: template PerVertexAttributeHandle<Star<MeshType>>;

template<typename MeshType>
using AreaFaceAttrHandle = typename MeshType:: template PerFaceAttributeHandle<double>;

template<typename MeshType>
using GradientVertAttrHandle = typename MeshType:: template PerVertexAttributeHandle<vcg::Point3d>;


template<typename MeshType>
void updateFaceStars(MeshType& m, StarVertAttrHandle<MeshType>& stars)
{
    using VertexPointer = typename MeshType::VertexPointer;
    using VertexIterator = typename MeshType::VertexIterator;
    using FaceIterator = typename MeshType::FaceIterator;
    using FaceType = typename MeshType::FaceType;
    
    int vIndex;
    VertexPointer v;
    std::vector<vcg::face::Pos<FaceType>> currStarPos;

    for(VertexIterator vIter = m.vert.begin(); vIter != m.vert.end(); vIter++)
        stars[vIter].clear();

    vcg::tri::UpdateFlags<MeshType>::VertexClearV(m);
    for(FaceIterator fIter = m.face.begin(); fIter != m.face.end(); fIter++)
    {        
        for(vIndex = 0; vIndex < 3; vIndex++)
        {
            v = fIter->V(vIndex);
            if(v->IsV())
                continue;
            v->SetV();

            vcg::face::VFOrderedStarFF(vcg::face::Pos<FaceType>(&(*fIter), v), currStarPos);
            for(vcg::face::Pos<FaceType> p : currStarPos)
                stars[v].push_back(p.F());
        }
    }
}

template<typename MeshType>
void updateNormalsAndAreas(MeshType& m, AreaFaceAttrHandle<MeshType>& areas)
{
    using FaceIterator = typename MeshType::FaceIterator;
    
    vcg::tri::UpdateNormal<MeshType>::PerFace(m);
    double currNorm;
    
    for(FaceIterator fIter = m.face.begin(); fIter != m.face.end(); fIter++)
    {
        currNorm = fIter->N().Norm();
        areas[fIter] = currNorm / 2.0;
        fIter->N().Normalize();
    }
}


#endif
