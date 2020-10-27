//-------------------------------------------------------//
// File:  Segmenter.h                                    //
//                                                       //
// Description: Segmenter Header File                    //
//                                                       //
// Authors: Anthousis Andreadis - http://anthousis.com   //
// Date: 7-Oct-2015                                      //
//                                                       //
// Computer Graphics Group                               //
// http://graphics.cs.aueb.gr/graphics/                  //
// AUEB - Athens University of Economics and Business    //
//                                                       //
//                                                       //
// This work was funded by the EU-FP7 - PRESIOUS project //
//-------------------------------------------------------//

#ifndef SEGMENTER_INCLUDED
#define SEGMENTER_INCLUDED

#include <common/interfaces.h>
#include "Cluster.h"

class Segmenter {
protected:
    CMeshO *meshObj_;
    float errorThreshold_;
    float eDist_;
    float totalArea_;
    size_t initNumClusters_;
    vcg::CallBackPos *cb_;

    void computeTotalAreaAndInitPFA();

    void clusterAddFace(Cluster *to, CMeshO::FacePointer face) {
        float faceArea = Segmenter::faceArea[*face];
        vcg::Point3f &faceNormal = face->N();
        vcg::Point3f &faceCentroid = Segmenter::faceCentroid[*face];
        // Compute the weighted new centroid (Before adding faces to the new segment)
        vcg::Point3f new_to_centroid = to->getCentroid() * to->getArea() + faceCentroid * faceArea;
        // Add it to the new segments face list
        to->faces_.push_back(face);
        // Update the belonging segment id on the face
        faceClusterPairs[face] = to;
        // Calc weighted normal of the merged segment
        vcg::Point3f new_to_norm = to->getAvgNormal() * (to->getArea()) + faceNormal * faceArea;
        // Update area of the merged segment
        to->setArea(to->getArea() + faceArea);
        // Update average normal of the merged segment
        to->setAvgNormal((new_to_norm / to->getArea()).Normalize());
        // Update the new segments centroid
        to->setCentroid(new_to_centroid / to->getArea());
    }

    // Decomposes a cluster face-per-face (faces are added to their best neighbors). The best face for removal is always selected.
    void decomposeClusterBestFirst(Cluster *segToMerge);
public:
    Segmenter(vcg::CallBackPos *cb) 
        : meshObj_(NULL), errorThreshold_(0.032f), eDist_(0.f), 
          totalArea_(0.f), initNumClusters_(0), cb_(cb) {}

    ~Segmenter() {
        for (size_t i = 0; i != clusterList_.size(); ++i) {
            if (clusterList_[i] != NULL) {
                delete clusterList_[i];
                clusterList_[i] = NULL;
            }
        }
        vcg::tri::Allocator<CMeshO>::DeletePerFaceAttribute<Cluster *>(*meshObj_, faceClusterPairs);
        vcg::tri::Allocator<CMeshO>::DeletePerFaceAttribute<int>(*meshObj_, faceMarks);
        vcg::tri::Allocator<CMeshO>::DeletePerFaceAttribute<int>(*meshObj_, faceTmpMarks);
        vcg::tri::Allocator<CMeshO>::DeletePerFaceAttribute<float>(*meshObj_, faceArea);
        vcg::tri::Allocator<CMeshO>::DeletePerFaceAttribute<vcg::Point3f>(*meshObj_, faceCentroid);
    }

    static CMeshO::PerFaceAttributeHandle<int> faceMarks;
    static CMeshO::PerFaceAttributeHandle<int> faceTmpMarks;
    static CMeshO::PerFaceAttributeHandle<Cluster *> faceClusterPairs;
    static CMeshO::PerFaceAttributeHandle<float> faceArea;
    static CMeshO::PerFaceAttributeHandle<vcg::Point3f> faceCentroid;

    std::vector<Cluster *> clusterList_;

    void setMesh(CMeshO *meshObj);

    void setWeightThreshold(float error) { errorThreshold_ = error; }

    void setLnDistance(float eDist) { eDist_ = eDist; }

    size_t getNumClusters() { return clusterList_.size();  }

    virtual void createClusters() = 0;

    void postProcessClusters(float areaThreshold);

    inline float getLnDist() { return eDist_; }
    inline float getWeightThreshold() { return errorThreshold_; }
};

#endif // SEGMENTER_INCLUDED
