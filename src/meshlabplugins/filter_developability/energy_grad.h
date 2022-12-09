#ifndef FILTERDEVELOPABILITY_ENERGYGRAD_H
#define FILTERDEVELOPABILITY_ENERGYGRAD_H

#include "mesh_utils.h"
#include "energy.h"

#include <vcg/math/matrix33.h>


/*
 * Compute the combinatorial energy of a mesh and its gradient
 * as defined in section B.2 of the paper "Developability of Triangle Meshes" by Stein et al. 2018
 */
template<typename MeshType>
double combinatorialEnergyGrad(MeshType& m,
                               AreaFaceAttrHandle<MeshType>& fAttrArea,
                               StarVertAttrHandle<MeshType>& vAttrStar,
                               GradientVertAttrHandle<MeshType>& vAttrGrad)
{
    using VertexIterator = typename MeshType::VertexIterator;

    // reset the gradient associated with each vertex
    for(VertexIterator vIter = m.vert.begin(); vIter != m.vert.end(); vIter++)
        vAttrGrad[vIter].SetZero();

    double currEnergy;
    double totEnergy = 0.0;
    StarPartitioning<MeshType> currPart;
    for(VertexIterator vIter = m.vert.begin(); vIter != m.vert.end(); vIter++)
    {
        currEnergy = localCombinatorialEnergy(&(*vIter), m, vAttrStar, &currPart);
        totEnergy += currEnergy;

        if(currPart.star->size() <= 3 || vIter->IsB())
            continue;

        regionNormalDeviationGrad(&(*vIter), currPart, 0, m, fAttrArea, vAttrStar, vAttrGrad);
        regionNormalDeviationGrad(&(*vIter), currPart, 1, m, fAttrArea, vAttrStar, vAttrGrad);
    }

    return totEnergy;
}


/*
 * Compute the gradient of the normal deviation of a region
 * as defined in section B.2 of the paper "Developability of Triangle Meshes" by Stein et al. 2018
 */
template<typename MeshType>
void regionNormalDeviationGrad(typename MeshType::VertexPointer v,
                               StarPartitioning<MeshType>& partitioning,
                               bool region,
                               MeshType& m,
                               AreaFaceAttrHandle<MeshType>& fAttrArea,
                               StarVertAttrHandle<MeshType>& vAttrStar,
                               GradientVertAttrHandle<MeshType>& vAttrGrad)
{
    using FacePointer = typename MeshType::FacePointer;
    using VertexPointer = typename MeshType::VertexPointer;
    
    // compute the begin index and the cardinality of the desired region
    int rBegin = region ? (partitioning.rBegin + partitioning.rSize)       : partitioning.rBegin;
    int rSize  = region ? (partitioning.star->size() - partitioning.rSize) : partitioning.rSize;
    int starSize = partitioning.star->size();
    
    FacePointer faceA, faceB;
    vcg::Point3d normalDiff;

    int faceA_vIndex, faceB_vIndex;
    VertexPointer vertA, vertB;
    
    // iterate through each pair of faces within the region
    for(int i = rBegin; i < (rBegin + rSize - 1); i++)
        for(int j = i+1; j < (rBegin + rSize); j++)
        {
            faceA = vAttrStar[v][i % starSize];
            faceB = vAttrStar[v][j % starSize];

            normalDiff = faceA->N() - faceB->N();

            for(faceA_vIndex = 0; faceA_vIndex < 3; faceA_vIndex++)
            {
                vertA = faceA->V(faceA_vIndex);
                vAttrGrad[vertA] += (faceNormalGrad(faceA, faceA_vIndex, m, fAttrArea).transpose() * normalDiff * 2/std::pow(rSize, 2));
            }

            for(faceB_vIndex = 0; faceB_vIndex < 3; faceB_vIndex++)
            {
                vertB = faceB->V(faceB_vIndex);
                vAttrGrad[vertB] -= (faceNormalGrad(faceB, faceB_vIndex, m, fAttrArea).transpose() * normalDiff* 2/std::pow(rSize, 2));
            }            
        }
}


/*
 * Compute the gradient of the normal of a face wrt one of its vertices
 * as defined in section B.1 of the paper "Developability of Triangle Meshes" by Stein et al. 2018
 */
template<typename MeshType>
vcg::Matrix33d faceNormalGrad(typename MeshType::FacePointer f,
                              int vIndex,
                              MeshType& m,
                              AreaFaceAttrHandle<MeshType>& fAttrArea)
{    
    vcg::Point3d oppositeEdge = f->V2(vIndex)->P() - f->V1(vIndex)->P();
    vcg::Matrix33d grad;

    grad.ExternalProduct(oppositeEdge ^ f->N(), f->N());
    grad /= fAttrArea[f];

    return grad;
}


#endif
