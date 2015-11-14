//-------------------------------------------------------//
// File:  Utils.h                                        //
//                                                       //
// Description: Utils Header File                        //
//                                                       //
//                                                       //
// Authors: Anthousis Andreadis - http://anthousis.com   //
//          Pavlos Mavridis     - http://pmavridis.com   //
// Date: 7-Oct-2015                                      //
//                                                       //
// Computer Graphics Group                               //
// http://graphics.cs.aueb.gr/graphics/                  //
// AUEB - Athens University of Economics and Business    //
//                                                       //
//                                                       //
// This work was funded by the EU-FP7 - PRESIOUS project //
//-------------------------------------------------------//

#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <common/interfaces.h>

class Cluster;
class Segmenter;

inline float metricNormalDeviation(const vcg::Point3f& segNormal, const vcg::Point3f& faceNormal) {
    float dotP = segNormal * faceNormal;
    return 1.f - fabsf(dotP);
}

inline float sqrDist(const vcg::Point3f &A, const vcg::Point3f &B) {
    vcg::Point3f tmp = A - B;
    return tmp * tmp;
}

inline float metricNormalAndDistance(const vcg::Point3f &segNormal, const vcg::Point3f &segCentroid,
    const vcg::Point3f &faceNormal, const vcg::Point3f &faceCentroid) {
    float dotPAvg = segNormal * faceNormal;
    float avgFactor = 1.f - dotPAvg;
    return avgFactor * avgFactor * sqrDist(segCentroid, faceCentroid);
}

class MeshFaceFunctor {

protected:
    CMeshO *mesh_;
public:
    MeshFaceFunctor(CMeshO *mesh) : mesh_(mesh) {}
    MeshFaceFunctor() : mesh_(NULL) {}
    virtual ~MeshFaceFunctor() {}
    //called for the center face
    virtual void initCenter(const CMeshO::FacePointer &){}
    //called after all faces are visited
    virtual void saveResults(const CMeshO::FacePointer &){}
    //called for each neighboring face
    virtual void operator()(const CMeshO::FacePointer&){}

};

class ComputeNormalFaceFunctor : public MeshFaceFunctor {

    //  std::vector<Vec3f> normals;
    int num;
    vcg::Point3f N_sum;
    float area_sum;
public:
    ComputeNormalFaceFunctor(CMeshO *m) : MeshFaceFunctor(m){}
    ComputeNormalFaceFunctor() : MeshFaceFunctor(NULL){}

    void initCenter(const CMeshO::FacePointer &fh){
        N_sum = fh->N();
        num = 1;
        area_sum = vcg::DoubleArea(*fh);
    }
    //called for each neighboring face
    void operator()(const CMeshO::FacePointer& fh){
        N_sum += fh->N();
        num++;
        area_sum += vcg::DoubleArea(*fh);
    };

    vcg::Point3f getAverageNormal(){
        return N_sum / (float)num;
    }

    float getSummedArea(){
        return area_sum;
    }
};

void runOperatorAtFace(CMeshO::FacePointer f_it, int faceID, int radius, MeshFaceFunctor* F, float euclidian, Cluster* centralSegment);
#endif // UTILS_INCLUDED
