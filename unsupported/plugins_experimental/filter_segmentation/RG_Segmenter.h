//-------------------------------------------------------//
// File:  RG_Segmenter.h                                 //
//                                                       //
// Description: Region Growing Segmenter Header File     //
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

#ifndef RG_SEGMENTER_INCLUDED
#define RG_SEGMENTER_INCLUDED

#include <vector>
#include <set>
#include "Segmenter.h"
#include <common/interfaces.h>

class RG_Segmenter : public Segmenter {
public:
    RG_Segmenter(vcg::CallBackPos *cb);

    // public functions
    void createClusters();

    enum rg_type_t {
        FIFO,
        BEST_FIRST,
        BF_LOCAL_NEIGHBORS
    };

    // parametrization
    void regionGrowindMethod(RG_Segmenter::rg_type_t type);
    void mergeClustersMethod(RG_Segmenter::rg_type_t type);

    static float(*error_metric_ptr_)(Cluster *, CMeshO::FacePointer &, Segmenter *segmenter);

    static float orientation_norm(Cluster *cl, CMeshO::FacePointer &face, Segmenter *segmenter) {
        return metricNormalDeviation(cl->getAvgNormal(), face->N());
    }

    static float local_boundary_norm(Cluster *cl, CMeshO::FacePointer &face, Segmenter *segmenter) {
        static int marker_counter = -1;
        float minWeight = FLT_MAX;
        for (size_t i = 0; i < 3; ++i) {
            CMeshO::FacePointer nqFacePointer = face->FFp(i);
            // Sanity checks
            if (!nqFacePointer || nqFacePointer == face || nqFacePointer->IsD()) { continue; }
            // If the neighbor belongs to the current segment 
            // At least one of its neighbor will belong for sure, we need to find the best one
            if (faceClusterPairs[*nqFacePointer] == cl) {
                ComputeNormalFaceFunctor *nff = new ComputeNormalFaceFunctor;
                runOperatorAtFace(nqFacePointer, ++marker_counter, 4, nff, segmenter->getLnDist(), cl);
                vcg::Point3f n1 = nff->getAverageNormal();
                //compute local average (or MEDIAN) normal (strictly inside segment b)
                runOperatorAtFace(face, ++marker_counter, 4, nff, segmenter->getLnDist(), NULL);
                vcg::Point3f n2 = nff->getAverageNormal();
                delete nff;
                // Full Segment Neighborhood inside Segment, Full inter-Segment Neighborhood outside
                minWeight = metricNormalDeviation(n1, n2);
                // We store the it as this is the face that will be added to the segment
                if (minWeight < segmenter->getWeightThreshold()) {
                    break;
                }
            }
        }
        return minWeight;
    }

    // parametrization
    void setMetric(float (*error_metric_ptr)(Cluster *, CMeshO::FacePointer &, Segmenter *)){
        error_metric_ptr_ = error_metric_ptr;
    }
private:
    struct lnFaceHandle {
        lnFaceHandle(CMeshO::FacePointer f)
            : face(f), ln_area(Segmenter::faceArea[*f]), lnAvgNormal(f->N()) {
            lnNormalAreaSum = lnAvgNormal * ln_area;
        }

        lnFaceHandle(CMeshO::FacePointer f, float area, vcg::Point3f normal)
            : face(f), ln_area(area), lnAvgNormal(normal) {
            lnNormalAreaSum = lnAvgNormal * ln_area;
        }

        void update(float lnf_area, const vcg::Point3f &lnf_normal) {
            ln_area += lnf_area;
            lnNormalAreaSum += lnf_normal * lnf_area;
            lnAvgNormal = lnNormalAreaSum / ln_area;
            lnAvgNormal.Normalize();
        }

        CMeshO::FacePointer face;
        float ln_area;
        vcg::Point3f lnAvgNormal;
    private:
        vcg::Point3f lnNormalAreaSum;
        lnFaceHandle(): ln_area(0.f), lnAvgNormal(0.f, 0.f, 0.f), lnNormalAreaSum(0.f, 0.f, 0.f) { }
    };

    class UpdateLocalNeighborsStructure : public MeshFaceFunctor {
        float bfArea_;
        vcg::Point3f bfNormal_;
        std::map<CMeshO::FacePointer, lnFaceHandle> *sfMap_;
    public:
        UpdateLocalNeighborsStructure(CMeshO *m) : MeshFaceFunctor(m){}
        UpdateLocalNeighborsStructure(std::map<CMeshO::FacePointer, lnFaceHandle> *sfMap) 
            : MeshFaceFunctor(NULL), sfMap_(sfMap){}

        void initCenter(const CMeshO::FacePointer &fh){
            bfArea_ = Segmenter::faceArea[*fh];
            bfNormal_ = fh->N();
        }
        //called for each neighboring face
        void operator()(const CMeshO::FacePointer& fh){
            std::map<CMeshO::FacePointer, lnFaceHandle>::iterator sf_it = sfMap_->find(fh);
            if (sf_it != sfMap_->end()) {
                sf_it->second.update(bfArea_, bfNormal_);
            }
        };
    };

    void clusterAddFace(Cluster *to, CMeshO::FacePointer &face);
    Cluster *finalizeCluster(Cluster *seg);
    void updateProcessList(Cluster *seg, CMeshO::FacePointer &face, std::vector<CMeshO::FacePointer> &processList);
    void updateProcessList(Cluster *seg, CMeshO::FacePointer &face, std::deque<CMeshO::FacePointer> &processList);

    void greedyFIFOSegmentation(void);              
    void greedyBestFirstSegmentation(void);
    void greedyBestFirstSegmentationLocalNeighbors(void);       

    typedef void (RG_Segmenter::*methodFunction)();

    methodFunction rgFunction_;

    static CMeshO::FacePointer(*next_candidate_)(Cluster *cl, void *);

    static CMeshO::FacePointer fifo(Cluster *cl, void *container) {
        std::deque<CMeshO::FacePointer> *fQueue = reinterpret_cast<std::deque<CMeshO::FacePointer> *>(container);
        CMeshO::FacePointer fqPointer = fQueue->front();
        fQueue->pop_front();
        return fqPointer;
    }

    static CMeshO::FacePointer best(Cluster *cl, void *container) {
        std::vector<CMeshO::FacePointer> *fVector = reinterpret_cast<std::vector<CMeshO::FacePointer> *>(container);

        float minWeight = FLT_MAX;
        CMeshO::FacePointer f_best;
        int i = 0, erase_index = 0;
        // Set current seg avg_normal and centroid
        for (std::vector<CMeshO::FacePointer>::iterator v_it = fVector->begin(); v_it != fVector->end(); ++v_it) {
            CMeshO::FacePointer vFacePointer = (*v_it);
            float weight = metricNormalAndDistance(cl->getAvgNormal(), cl->getCentroid(),
                vFacePointer->N(), Segmenter::faceCentroid[*vFacePointer]);
            if (minWeight >= weight) {
                minWeight = weight;
                f_best = vFacePointer;
                erase_index = i;
            }
            i++;
        }
        // Remove the item from the Vector
        fVector->erase(fVector->begin() + erase_index);
        return f_best;
    }

    void setNexCandidate(CMeshO::FacePointer(*next_candidate)(Cluster *, void *)){
        next_candidate_ = next_candidate;
    }
};

#endif // RG_SEGMENTER_INCLUDED
