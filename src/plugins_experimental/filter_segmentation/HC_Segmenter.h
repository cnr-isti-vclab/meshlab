//--------------------------------------------------------------//
// File:  HC_Segmenter.h                                        //
//                                                              //
// Description: Hierarchical Clustering Segmenter Header File   //
//                                                              //
// Authors: Pavlos Mavridis     - http://pmavridis.com          //
//          Anthousis Andreadis - http://anthousis.com          //
// Date: 7-Oct-2015                                             //
//                                                              //
// Computer Graphics Group                                      //
// http://graphics.cs.aueb.gr/graphics/                         //
// AUEB - Athens University of Economics and Business           //
//                                                              //
//                                                              //
// This work was funded by the EU-FP7 - PRESIOUS project        //
//--------------------------------------------------------------//

#ifndef HC_SEGMENTER_INCLUDED
#define HC_SEGMENTER_INCLUDED

#include <vector>
#include <set>
#include "Segmenter.h"
#include <common/interfaces.h>

class HC_Segmenter : public Segmenter {
public:
    HC_Segmenter(vcg::CallBackPos *cb);

    static float(*error_metric_ptr_)(Cluster*, Cluster*, Segmenter *segmenter);

    static float orientation_norm(Cluster* a, Cluster * b, Segmenter *segmenter) {
        vcg::Point3d n1 = (a->faceNormalSum_ / a->getArea()).Normalize();
        vcg::Point3d n2 = (b->faceNormalSum_ / b->getArea()).Normalize();

        float cosTheta = (float)(n1 * n2);
        return 1.0f - fabsf(cosTheta);
    }

    static float local_boundary_norm(Cluster* a, Cluster * b, Segmenter *segmenter) {
        static int marker_counter = 0;

        //first we have to find the faces that connect the two segments
        std::map<Cluster*, std::set<CMeshO::FacePointer> >::iterator it = a->neighbors_.find(b);

        std::set<CMeshO::FacePointer> boundary = it->second;
        size_t size = boundary.size();

        float* distances = new float[size];
        ComputeNormalFaceFunctor *nff = new ComputeNormalFaceFunctor;

        int num = 0;
        for (std::set<CMeshO::FacePointer>::iterator I = boundary.begin(); I != boundary.end(); ++I) {
            CMeshO::FacePointer A = *I;
            CMeshO::FacePointer B;
            //iterate the 1-ring faces and find the neighboring facet inside segment B
            bool found = false;
            // For all face neighbors
            for (size_t i = 0; i < 3; ++i) {
                CMeshO::FacePointer nFacePointer = A->FFp(i);
                // Sanity checks
                if (!nFacePointer || nFacePointer == A || nFacePointer->IsD()) { continue; }

                Cluster* nc2 = faceClusterPairs[*nFacePointer];
                if (nc2 == b) {
                    B = nFacePointer;
                    found = true;
                    break;
                }
            }
            if (!found) {
                continue;
            }

            //compute local average (or MEDIAN) normal (strictly inside segment a)
            runOperatorAtFace(A, marker_counter, 4, nff, segmenter->getLnDist(), a);
            vcg::Point3f n1 = nff->getAverageNormal();
            //compute local average (or MEDIAN) normal (strictly inside segment b)
            runOperatorAtFace(B, marker_counter++, 4, nff, segmenter->getLnDist(), b);
            vcg::Point3f n2 = nff->getAverageNormal();

            n1.Normalize();
            n2.Normalize();

            float cosTheta = n1 * n2;
            distances[num++] = 1.0f - fabsf(cosTheta);
        }

        float median = 1.0;
        if (num > 0){
            std::sort(distances, distances + num);
            median = distances[num / 2];
        }
        delete[] distances;
        delete nff;

        return median;
    }

    // parametrization
    void setMetric(float(*error_metric_ptr)(Cluster*, Cluster*, Segmenter *)){
        error_metric_ptr_ = error_metric_ptr;
    }

    // public functions
    void createClusters();
private:
    struct TreeNode{
        int index;
        float weight;
    };

    TreeNode* level2_nodes;

    void hierarchicalMerging();

    void update_level2_item(int level1_index){
        int level2_index = level1_index / BUCKET_SIZE;
        float distance;
        /*Cluster* d = */clusterGetNearestCluster(clusterArray_[level1_index], &distance);
        if (distance < level2_nodes[level2_index].weight){
            level2_nodes[level2_index].weight = distance;
            level2_nodes[level2_index].index = level1_index;
        }
    }

    void update_level2_range(int start, int end){
        for (int i = start; i < end; ++i)
            update_level2_item(i);

    }

    void recompute_level2_at(int l2_index){
        level2_nodes[l2_index].weight = FLT_MAX;
        level2_nodes[l2_index].index = -1;
        int l1_range_start = BUCKET_SIZE * l2_index;
        int l1_range_end = std::min(l1_range_start + BUCKET_SIZE, active_clusters_);
        update_level2_range(l1_range_start, l1_range_end);
    }

    void recompute_level2_at2(int l2_index){
        level2_nodes[l2_index].weight = FLT_MAX;
        level2_nodes[l2_index].index = -1;
        int l1_range_start = BUCKET_SIZE * l2_index;
        int l1_range_end = std::min(l1_range_start + BUCKET_SIZE, active_clusters_ + 1);
        update_level2_range(l1_range_start, l1_range_end);
    }

    void clusterAddFace(Cluster *to, CMeshO::FacePointer &face);

    void clusterAddNeighbor(Cluster *to, Cluster* c, CMeshO::FacePointer &fh);

    void clusterUpdateIfNearestOneWay(Cluster *to, float& minWeight, Cluster** A, Cluster** B, Segmenter *segmenter);

    void clusterComputeNearestClusters(Cluster *to);

    void clusterGetNeighborClusterGreedy(Cluster *to);

    Cluster* clusterGetNearestCluster(Cluster *to, float *d);

    void clusterMerge(Cluster *to, Cluster *c);

    void clusterExchange(Cluster *to, Cluster *old_cluster, Cluster *new_cluster);

    void initClusters();

    virtual void finalizeClusters();

    Cluster** clusterArray_;
    Cluster *clusterPool_;

    int initialFaces_; //number of initial faces
    int active_clusters_;
};

#endif // HC_SEGMENTER_INCLUDED
