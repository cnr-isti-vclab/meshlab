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

#include <vector>
#include <set>
#include <common/interfaces.h>
#include "Cluster.h"

class Colorizer {
    std::map<int, vcg::Point3f> colors;

    int numOfColors;
    float currentHue;
    float currentValue;
    float step;

    //source: http://www.mech.uq.edu.au/staff/jacobs/nm_lib/c_source/hsv2rgb.c
    vcg::Point3f HSV2RGB(const vcg::Point3f& hsv) {
        /*
        * Purpose:
        * Convert HSV values to RGB values
        * All values are in the range [0.0 .. 1.0]
        */
        float S, H, V, F, M, N, K;
        int   I;
        vcg::Point3f rgb;

        S = hsv[1];//->s;  /* Saturation */
        H = hsv[0];//->h;  /* Hue */
        V = hsv[2];//->v;  /* value or brightness */

        if (S == 0.0) {
            /*
            * Achromatic case, set level of grey
            */
            rgb[0] = V;
            rgb[1] = V;
            rgb[2] = V;
        }
        else {
            /*
            * Determine levels of primary colours.
            */
            if (H >= 1.0) {
                H = 0.0;
            }
            else {
                H = H * 6;
            } /* end if */
            I = (int)H;   /* should be in the range 0..5 */
            F = H - I;     /* fractional part */

            M = V * (1 - S);
            N = V * (1 - S * F);
            K = V * (1 - S * (1 - F));

            if (I == 0) { rgb[0] = V; rgb[1] = K; rgb[2] = M; }
            if (I == 1) { rgb[0] = N; rgb[1] = V; rgb[2] = M; }
            if (I == 2) { rgb[0] = M; rgb[1] = V; rgb[2] = K; }
            if (I == 3) { rgb[0] = M; rgb[1] = N; rgb[2] = V; }
            if (I == 4) { rgb[0] = K; rgb[1] = M; rgb[2] = V; }
            if (I == 5) { rgb[0] = V; rgb[1] = M; rgb[2] = N; }
        } /* end if */

        return rgb;
    }

    vcg::Point3f nextColor(){
        vcg::Point3f hsv = vcg::Point3f(currentHue, 0.5f, currentValue);
        vcg::Point3f col = HSV2RGB(hsv);
        currentHue += step;
        if (currentHue > 0.8){
            currentHue = 0.0f;
        }

        return col;
    }

public:
    Colorizer(){
        currentHue = 0.0f;
        currentValue = 85.f / 100.f;
        numOfColors = 0;
        step = 0.05f;
    }

    vcg::Point3f getColor(int p){
        std::map<int, vcg::Point3f>::iterator I = colors.find(p);

        if (I == colors.end()){
            //generate new color
            numOfColors++;
            vcg::Point3f new_c = nextColor();
            colors[p] = new_c;
            return new_c;
        }
        else{
            //find pre-assigned color
            return colors[p];
        }
    }

};

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
