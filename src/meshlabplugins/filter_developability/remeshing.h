#ifndef FILTERDEVELOPABILITY_REMESHING_H
#define FILTERDEVELOPABILITY_REMESHING_H

#include "mesh_utils.h"


/*
 * To improve numerical stability of the energy optimization, meshes should not present
 * small interior angles;
 * this class implements a post-processing phase that can be applied between iterations
 * in order to get rid of these angles by means of edge flipping and/or edge collapsing
 */
template <class MeshType>
class MeshPostProcessing
{
public:
    MeshPostProcessing(bool doEdgeFlipping,
                       bool doEdgeCollapsing,
                       double angleThreshold) :
        doEdgeFlipping(doEdgeFlipping),
        doEdgeCollapsing(doEdgeCollapsing),
        angleThreshold(angleThreshold)  // the radiants under which the angle is considered to be small
    {}

    /*
     * Perform the post-processing on a mesh
     */
    bool process(MeshType& m)
    {
        typedef typename MeshType::FaceType FaceType;
        typedef typename MeshType::FaceIterator FaceIterator;
        
        double faceAngles[3];
        int edgeToFlip;
        int edgeToCollapse;
        bool meshModified = false;
        
        for(FaceIterator fIter = m.face.begin(); fIter != m.face.end(); fIter++)
        {
            if(fIter->IsD())
                continue;
            
            faceAngles[0] = vcg::face::WedgeAngleRad<FaceType>(*fIter, 0);
            faceAngles[1] = vcg::face::WedgeAngleRad<FaceType>(*fIter, 1);
            faceAngles[2] = vcg::face::WedgeAngleRad<FaceType>(*fIter, 2);

            edgeToFlip = -1;
            edgeToCollapse = -1;
            for(int i = 0; i < 2; i++)
                if(faceAngles[i] < angleThreshold)
                {
                    if(faceAngles[(i+1)%3] < angleThreshold)
                    {
                        edgeToFlip = i;
                        break;
                    }
                    else if(faceAngles[(i-1)%3] < angleThreshold)
                    {
                        edgeToFlip = (i-1)%3;
                        break;
                    }
                    else
                    {
                        edgeToCollapse = (i+1)%3;
                        break;
                    }
                }

            if(doEdgeFlipping && edgeToFlip >= 0 && vcg::face::CheckFlipEdge(*fIter, edgeToFlip))
            {
                vcg::face::FlipEdge<FaceType>(*fIter, edgeToFlip);
                meshModified = true;
            }
            else if(doEdgeCollapsing && edgeToCollapse >= 0 && vcg::face::FFLinkCondition(*fIter, edgeToCollapse))
            {
                vcg::face::FFEdgeCollapse<MeshType>(m, *fIter, edgeToCollapse);
                meshModified = true;
            }
        }

        if(meshModified)
        {
            vcg::tri::Allocator<MeshType>::CompactFaceVector(m);
            vcg::tri::Allocator<MeshType>::CompactVertexVector(m);
            vcg::tri::UpdateFlags<MeshType>::VertexBorderFromFaceAdj(m);
        }

        return meshModified;
    }

private:
    bool doEdgeFlipping;
    bool doEdgeCollapsing;
    double angleThreshold;
};


#endif
