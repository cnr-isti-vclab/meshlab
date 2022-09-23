#ifndef FILTERDEVELOPABILITY_ENERGY_H
#define FILTERDEVELOPABILITY_ENERGY_H

#include <vector>

#define FILTERDEVELOPABILITY_AVOID_BRANCHING


template<typename MeshType>
struct StarPartitioning
{
    Star<MeshType>* star;
    int rBegin;   // index of the starting face within star for the first region
    int rSize;    // cardinality of the first region
};


/*
 * Compute the combinatorial energy of a mesh
 * as defined in section 4.1.1 of the paper "Developability of Triangle Meshes" by Stein et al. 2018
 */
template<typename MeshType>
double combinatorialEnergy(MeshType& m,
                           StarVertAttrHandle<MeshType>& vAttrStar)
{
    using VertexIterator = typename MeshType::VertexIterator;
    
    double totEnergy = 0.0;

    for(VertexIterator vIter = m.vert.begin(); vIter != m.vert.end(); vIter++)
        totEnergy += localCombinatorialEnergy(&(*vIter), m, vAttrStar);

    return totEnergy;
}


/*
 * Compute the combinatorial energy of a vertex star
 * as defined in section 4.1.1 of the paper "Developability of Triangle Meshes" by Stein et al. 2018
 */
template<typename MeshType>
double localCombinatorialEnergy(typename MeshType::VertexPointer v,
                                MeshType& m,
                                StarVertAttrHandle<MeshType>& vAttrStar,
                                StarPartitioning<MeshType>* outP = nullptr)
{
    double energy = -1.0;
    StarPartitioning<MeshType> currP = {
        &(vAttrStar[v]),    // star of faces around v
        0,                  // rBegin
        0                   // rSize
    };

    if(outP != nullptr)
        outP->star = currP.star;

    if(currP.star->size() <= 3 || v->IsB())
        energy = 0.0;
    else
    {
        // consider all possible cardinalities of a region resulting from a partitioning of the star
        for(currP.rSize = 2; currP.rSize <= (currP.star->size() - 2); currP.rSize++)
        {
            // consider all possible regions having rSize faces and not crossing the vector boundaries.
            // rBegin is the index of the first face in the region
            for(currP.rBegin = 0; currP.rBegin < (currP.star->size() - currP.rSize); currP.rBegin++)
            {                
                double currRegionEnergy  = regionNormalDeviation(currP, 0, m);
                double otherRegionEnergy = regionNormalDeviation(currP, 1, m);     

                double currPartitioningEnergy;
                #ifdef FILTERDEVELOPABILITY_AVOID_BRANCHING
                    currPartitioningEnergy = std::max(currRegionEnergy, otherRegionEnergy);
                #else
                    currPartitioningEnergy = currRegionEnergy + otherRegionEnergy;
                #endif

                if(energy < 0 || currPartitioningEnergy < energy)
                {
                    energy = currPartitioningEnergy;
                    if(outP != nullptr)
                    {
                        outP->rBegin = currP.rBegin;
                        outP->rSize = currP.rSize;
                    }
                }
            }
        }
    }

    return energy;
}


/*
 * Compute the normal deviation of a region of a vertex star
 * as defined in equation 1 of the paper "Developability of Triangle Meshes" by Stein et al. 2018
 */
template<typename MeshType>
double regionNormalDeviation(const StarPartitioning<MeshType>& partitioning,
                             bool region,
                             MeshType& m)
{
    // compute the begin index and the cardinality of the desired region
    int rBegin = region ? (partitioning.rBegin + partitioning.rSize)       : partitioning.rBegin;
    int rSize  = region ? (partitioning.star->size() - partitioning.rSize) : partitioning.rSize;
    int starSize = partitioning.star->size();

    vcg::Point3d normDiff;
    normDiff.SetZero();

    double currRegionNormalDev;
    double regionNormalDev = 0.0;

    // iterate through each pair of faces within the region
    for(int i = rBegin; i < (rBegin + rSize - 1); i++)
        for(int j = i+1; j < (rBegin + rSize); j++)
        {
            normDiff = ( partitioning.star->at(i % starSize)->N() - partitioning.star->at(j % starSize)->N() );
            currRegionNormalDev = normDiff.SquaredNorm();

            #ifdef FILTERDEVELOPABILITY_AVOID_BRANCHING
                regionNormalDev = std::max(regionNormalDev, currRegionNormalDev);
            #else
                regionNormalDev += ( currRegionNormalDev / std::pow(rSize, 2) );
            #endif
        }

    return regionNormalDev;
}


#endif
