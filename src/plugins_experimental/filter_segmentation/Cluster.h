//-------------------------------------------------------//
// File:  Cluster.h                                      //
//                                                       //
// Description: Cluster Header File                      //
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
#ifndef CLUSTER_INCLUDED
#define CLUSTER_INCLUDED

#define BUCKET_SIZE 128

#include <vector>
#include <map>
#include <set>
#include <common/interfaces.h>
#include "Utils.h"

class Segmenter;

class Cluster {
public:
    std::map<Cluster*, std::set<CMeshO::FacePointer> > neighbors_;

    std::vector<CMeshO::FacePointer> faces_;
    std::set<CMeshO::FacePointer> borderFaces_;

    unsigned int id_;
    float area_;
    bool disjoined_;

    vcg::Point3f normal_;
    vcg::Point3f centroid_;

    vcg::Point3d faceNormalSum_;
    vcg::Point3d centroidSum_;

    Cluster* nearestCluster_;
    float nearestDistance_;

    int mark;

    Cluster() : id_(0), area_(0.f), disjoined_(false),
        normal_(vcg::Point3f(0.f, 0.f, 0.f)), centroid_(vcg::Point3f(0.f, 0.f, 0.f)),
        faceNormalSum_(vcg::Point3d(0.f, 0.f, 0.f)), centroidSum_(vcg::Point3d(0.f, 0.f, 0.f)),
        nearestCluster_(NULL), nearestDistance_(0.f), mark(-1) {}

    // public set functions
    void setArea(float area) { area_ = area; }
    void setId(unsigned int i) { id_ = i; }
    void setAvgNormal(const vcg::Point3f& normal) { normal_ = normal; }
    void setCentroid(const vcg::Point3f& center) { centroid_ = center; }

    // public get functions
    float getArea() { return area_; }
    int  getId() { return id_; }
    const vcg::Point3f& getAvgNormal() { return normal_; }
    const vcg::Point3f& getCentroid() { return centroid_; }

    void updateAvgNormal() {   normal_.Import(faceNormalSum_.Normalize()); }
    void updateCentroid() {  centroid_.Import(centroidSum_.Normalize()); }
};

#endif // CLUSTER_INCLUDED
